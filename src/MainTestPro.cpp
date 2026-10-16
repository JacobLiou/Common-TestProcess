// MainTestPro.cpp: implementation of the CMainTestPro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestProcess.h"
#include "MainTestPro.h"
#include "TaskPro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTestPro::CMainTestPro() //: m_dev(&m_stGeneralInterface)
{
	//Socket运行状态标志初始化
	m_bSockState = FALSE;
	//主线程是否启动
	m_bStartPro = FALSE;

	//初始化句柄
	m_hMsgEvent = m_hMainEvent = INVALID_HANDLE_VALUE;
	//清理信息
	m_strErr.Empty();
	m_strUser.Empty();
	// 
	m_InfoManager.SetNetCommand(&m_commSock);
	m_InfoManager.SetSockDataInfo(&m_stSockInfo);

	m_TestTaskRuner.SetNetCommand(&m_commSock);
	m_TestTaskRuner.SetSockDataInfo(&m_stSockInfo);
	m_TestTaskRuner.SetTPInfoManager(&m_InfoManager);
}

CMainTestPro::~CMainTestPro()
{
	//退出
	m_bSockState = FALSE;
	m_bStartPro = FALSE;
	//	//释放内存
	//	if(m_stTaskInfo.pstAllTaskState != NULL)
	//	{
	//		delete []m_stTaskInfo.pstAllTaskState;
	//		m_stTaskInfo.pstAllTaskState = NULL;
	//	}
		//关闭
	if (m_hMsgEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hMsgEvent);
	}
	if (m_hMainEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hMainEvent);
	}
}

//启动后门窗口程序
BOOL CMainTestPro::ShowDlg()
{
	//	STARTUPINFO si = {sizeof(si)};
	//	PROCESS_INFORMATION pi;
	//	CString strPath, strRoot;
	//	strRoot.Format("%s\\%s", PATH_ROOT, PATH_DLG_ROOT);
	//	strPath.Format("%s\\EDFATestSystem.exe", strRoot);
	//	if(CreateProcess(strPath, NULL, NULL, NULL, FALSE, 0, NULL, strRoot, &si, &pi))
	//	{
	//		CloseHandle(pi.hProcess);
	//		CloseHandle(pi.hThread);
	//	}
	return TRUE;
}

//启动TP服务
BOOL CMainTestPro::Run()
{
	//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//	CSample dlg;
	//	dlg.DoModal();
		//启动服务端网络
	if (!StartSocket())
	{
		return FALSE;
	}
	//主线程消息事件
	m_hMsgEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//TP服务主框架流程标志
	m_hMainEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//主线程启动标志位
	m_bStartPro = TRUE;
	//启动主work线程
	AfxBeginThread((AFX_THREADPROC)EventWorkThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	//启动Socket消息监听线程
	AfxBeginThread((AFX_THREADPROC)MsgMonitorThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

	return TRUE;
}

//停止TP服务
void CMainTestPro::Stop()
{
	m_bStartPro = FALSE;
	m_bSockState = FALSE;
	//关闭Socket服务
	m_commSock.stopSrv();
	//退出
	SetEvent(m_hMainEvent);
	return;
}

//消息工作管理线程
UINT CMainTestPro::EventWorkThread(LPVOID lpParam)
{
	CMainTestPro* p = (CMainTestPro*)lpParam;
	DWORD dwEvent;
	while (p->m_bStartPro)
	{
		dwEvent = WaitForSingleObject(p->m_hMsgEvent, INFINITE);
		switch (dwEvent)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			return 1;
		default:
			AfxBeginThread((AFX_THREADPROC)SingleEventWorker, lpParam, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			break;
		}
		//Reset event
		ResetEvent(p->m_hMsgEvent);
	}
	return 0;
}

//单任务消息处理线程
UINT CMainTestPro::SingleEventWorker(LPVOID lpParam)
{
	((CMainTestPro*)lpParam)->EventPro();
	return 0;
}

//优先消息处理事件
void CMainTestPro::MessagePro()
{
	int iAct = 0;
	DWORD dwTemp = 0;
	//Get the whole message
	memset(&m_stSockInfo, 0, sizeof(ALL_MSG_DATA));
	if (!m_commSock.XmlRecv(m_stSockInfo))
	{
		//Receive message failed
		m_strErr = m_commSock.GetLastError();
		WriteLogFile(m_strErr, LOG_ERROR);
		m_commSock.XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
		return;
	}
	//if the message effective, make it
	iAct = m_stSockInfo.nDataType;
	int nRunCount = 0;
	BOOL	bRunResult = TRUE;
	CString strLogTemp;
	switch (iAct)
	{
	case MSG_MSG_MSGBOX:
		dwTemp = atoi(m_stSockInfo.unDataStruct.MsgBox.szButtonId);
		m_commSock.XmlFeedback(&m_stSockInfo);
		break;
	case MSG_MSG_CONFIGPATH:
	case MSG_MSG_TEMPLATE:
	case MSG_MSG_TASKLIST:
	case MSG_MSG_UPLOADTESTDATA:
	case MSG_MSG_CONVPN:
	case MSG_MSG_CONVPROCESS:
	case MSG_MSG_STATIONINFO:
	case MSG_MSG_TESTRESULTOPC:
	case MSG_MSG_MOVEIN:
	case MSG_MSG_MOVEOUT:
	case MSG_MSG_USERLOGINVERIFY:
	case MSG_MSG_STATIONVERIFY:
	case MSG_MSG_OPLINKUSERINFO:
	case MSG_MSG_USERSKILLVERIFY:
	case MSG_MSG_CREATESNDIR:
	case MSG_MSG_LOCATIONINFO:
	case MSG_MSG_GENERICFUNC:
	case MSG_MSG_FWSTANDARDPATH:
		m_commSock.XmlFeedback(&m_stSockInfo);
		break;
	case MSG_COMMAND_STOPRUN:
		m_strUser = m_stSockInfo.unDataStruct.ComStopRunData.szUser;
		m_commSock.XmlSetStop();
		break;
	case MSG_COMMAND_QUIT:
		do {
			bRunResult = m_TestTaskRuner.QuitTestTask();
			nRunCount++;
			Sleep(100);
		} while ((!bRunResult) && nRunCount < 50);
		if (bRunResult)
		{
			strLogTemp.Format("退出命令执行成功，即将结束TP运行");
			WriteLogFile(m_strErr, LOG_NORMAL);
			m_commSock.XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_NORMAL);
			m_bStartPro = FALSE;
		}
		else
		{
			strLogTemp.Format("退出命令执行失败，请重试");
			WriteLogFile(m_strErr, LOG_ERROR);
			m_commSock.XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
		}
		break;
	default:
		m_iAct = iAct;
		SetEvent(m_hMsgEvent);
		break;
	}
	return;
}

//次级消息处理事件
void CMainTestPro::EventPro()
{
	CString strLogTemp;
	switch (m_iAct)
	{
	case MSG_COMMAND_RUNTASK:
		if (!m_TestTaskRuner.RunTestTaskItem())
		{
			strLogTemp.Format("执行 测试任务 信息失败!");
			WriteLogFile(strLogTemp, LOG_ERROR);
		}
		break;
	case MSG_COMMAND_RUNCOMMAND:
	{

	}
	break;
	case MSG_COMMAND_RUNINIT:
		if (!m_TestTaskRuner.RunTestInit())
		{
			strLogTemp.Format("初始化测试环境失败!");
			WriteLogFile(strLogTemp, LOG_ERROR);
		}
		break;

	case MSG_MSG_TASKINFO:
		if (!m_InfoManager.SendTaskInfo())
		{
			strLogTemp.Format("发送 测试任务 信息失败!");
			WriteLogFile(strLogTemp, LOG_ERROR);
			m_commSock.XmlSendLog(strLogTemp, strLogTemp.GetLength() + 1, LOG_ERROR);
		}
		else
		{
			strLogTemp.Format("发送 测试任务 信息成功!");
			WriteLogFile(strLogTemp, LOG_NORMAL);
		}
		break;
	case MSG_MSG_COMMANDINFO:
		break;
	case MSG_MSG_BASEINFO:
		if (!m_InfoManager.SendTPDllInfo())
		{
			strLogTemp.Format("发送BaseInfo 信息失败!");
			WriteLogFile(strLogTemp, LOG_ERROR);
			m_commSock.XmlSendLog(strLogTemp, strLogTemp.GetLength() + 1, LOG_ERROR);
		}
		else
		{
			strLogTemp.Format("发送BaseInfo 信息成功!");
			WriteLogFile(strLogTemp, LOG_NORMAL);
		}
		break;
	case MSG_MSG_OLDRATE://获取已经测试项目
		break;
	case MSG_MSG_TASKDESCRIPTION:
		if (!m_InfoManager.SendTPTaskDescription())
		{
			strLogTemp.Format("发送 TaskDescription 信息失败!");
			WriteLogFile(strLogTemp, LOG_ERROR);
			m_commSock.XmlSendLog(strLogTemp, strLogTemp.GetLength() + 1, LOG_ERROR);
		}
		else
		{
			strLogTemp.Format("发送 TaskDescription 信息成功!");
			WriteLogFile(strLogTemp, LOG_NORMAL);
		}
		break;
	default:
		break;
	}
	return;
}

//Socket消息监听线程
UINT CMainTestPro::MsgMonitorThread(LPVOID lpParam)
{
	CMainTestPro* p = (CMainTestPro*)lpParam;
	while (p->m_bStartPro)
	{
		//Whether the message count more than or equal to 1
		if (p->m_commSock.acqMsgCount() > 0)
		{
			p->MessagePro();
		}
		//Delay 100ms for next message
		Sleep(150);
	}
	return 0;
}

//获取当前错误信息
TCHAR* CMainTestPro::GetLastError(TCHAR** lpszErr /* = NULL */)
{
	if (lpszErr != NULL)
	{
		*lpszErr = m_strErr.GetBuffer(0);
	}
	return m_strErr.GetBuffer(0);
}

//启动socket服务
BOOL CMainTestPro::StartSocket()
{
	//Initialize socket information

	CString	strTemp;
	stSockInfo stInitSock;
	CString strPath, strIP;
	try
	{
		//Get server IP
//		strPath.Format("%s\\%s", PATH_CFG_ROOT, NAME_CFG_TP);
//		if(!m_cfg.GetServerInfo(strPath, strIP.GetBuffer(MAX_PATH)))
//		{
//			throw m_cfg.GetLastError(NULL);
//		}
//		strIP.ReleaseBuffer();
		strIP = "172.0.0.1";
		//初始化socket连接类型及服务端IP
		stInitSock.iConnType = CONN_TP;
		stInitSock.pszSrvIP = strIP.GetBuffer(0);
		//启动服务
		if (!m_commSock.startSrv(stInitSock))
		{
			throw (TCHAR*)m_commSock.getLastError(NULL);
		}

		m_bSockState = TRUE;
	}
	catch (TCHAR* pszErr)
	{
		m_bSockState = FALSE;
		m_strErr = pszErr;
		WriteLogFile(m_strErr, LOG_ERROR);
	}
	catch (...)
	{
		m_bSockState = FALSE;
		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
		WriteLogFile(m_strErr, LOG_ERROR);
	}
	return m_bSockState;
}

//等待客户连接
void CMainTestPro::WaitForClient()
{
	while (TRUE)
	{
		if (m_commSock.acqClientCount() > 0)
		{
			break;
		}
		Sleep(TIME_200MS);
	}
}


//写本地log文件
void CMainTestPro::WriteLogFile(LPCTSTR tszlogMsg, LOGLEVEL eLevel /* = LOG_NORMAL */)
{
	/*	FILE*	pLogFile = NULL;
		CTime ti = CTime::GetCurrentTime();
		CString strPath, strState;
		strPath.Format("%s\\Log_%s(%s).txt", PATH_LOG_ROOT, ti.Format("%Y-%m-%d"));
		pLogFile = fopen(strPath, "at");
		if(pLogFile == NULL)
		{
			return;
		}
		//Level
		switch(eLevel)
		{
		case LOG_NORMAL:
			strState = "日常";
			break;
		case LOG_WARNING:
			strState = "警告";
			break;
		case LOG_ERROR:
			strState = "错误";
			break;
		default:
			strState = "日常";
			break;
		}
		fprintf(pLogFile, "%s:(%s) %s\n", ti.Format("%Y-%m-%d-%H-%M-%S"), strState, tszlogMsg);
		fclose(pLogFile);*/
	return;
}

//检查模块信息
//DEL BOOL CMainTestPro::CheckModInfo(stModTotalInfo& stModInfo)
//DEL {
//DEL /*	CString strPN, strPath;
//DEL 	try
//DEL 	{
//DEL 
//DEL 		//Get module information
//DEL // 		if(!m_dev.ModuleBaseInfo(stModInfo))
//DEL // 		{
//DEL // 			throw m_dev.GetLastError();
//DEL // 		}
//DEL 		//Get PN in config file
//DEL 		strPath.Format("%s\\%s", m_stGeneralInterface.pszCfgPath, NAME_CFG_PN);
//DEL 		if(!m_cfg.GetPN(strPath, strPN.GetBuffer(MAX_PATH)))
//DEL 		{
//DEL 			throw m_cfg.GetLastError(NULL);
//DEL 		}
//DEL 		strPN.ReleaseBuffer();
//DEL 		//Compare PN
//DEL 		if(strPN != stModInfo.stBase.tszPartNumber)
//DEL 		{
//DEL 			throw _T("模块PN核对有误, 请核查PN设置.");
//DEL 		}
//DEL 	}
//DEL 	catch(TCHAR* pszErr)
//DEL 	{
//DEL 		m_strErr = pszErr;
//DEL 		return FALSE;
//DEL 	}
//DEL 	catch(...)
//DEL 	{
//DEL 		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
//DEL 		return FALSE;
//DEL 	}*/
//DEL 	return TRUE;
//DEL }

void CMainTestPro::MsgTaskPro(Command_RunTask data)
{
	CString strTemp;
	BOOL bFuncOk = TRUE;
	TASKRESULT tResult = TASK_DONE;
	int nRawTaskIndex = atoi(data.szIndex);
	int iTaskIndex = atoi(data.szIndex) % TASK_OFF_1000;
	if (iTaskIndex < 0)
	{
		m_strErr.Format(_T("接收任务信息有误. 任务序号:%s, 任务名称:%s"), data.szIndex, data.szTitle);
		WriteLogFile(m_strErr, LOG_ERROR);
		m_commSock.XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
		return;
	}
	//Log information
	strTemp.Format(_T("正在处理任务. 任务序号:%s, 任务名称:%s"), data.szIndex, data.szTitle);
	WriteLogFile(m_strErr);
	m_commSock.XmlSendLog(strTemp, strTemp.GetLength() + 1);
	//Start task process
	bFuncOk = m_TestTaskRuner.TaskProcess(iTaskIndex);
	if (bFuncOk)
	{
		strTemp.Format(_T("处理任务成功. 任务序号:%s, 任务名称:%s"), data.szIndex, data.szTitle);
		WriteLogFile(m_strErr);
		m_commSock.XmlSendLog(strTemp, strTemp.GetLength() + 1);
	}
	else
	{
		strTemp.Format(_T("正在处理失败. 任务序号:%s, 任务名称:%s"), data.szIndex, data.szTitle);
		WriteLogFile(m_strErr, LOG_ERROR);
		m_commSock.XmlSendLog(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		if (m_commSock.XmlStopTask())
		{
			tResult = TASK_STOP;
			m_commSock.XmlResetStop();
		}
		else
		{
			tResult = TASK_FAIL;
		}
	}
	if (!m_commSock.XmlTaskDone(nRawTaskIndex, m_strUser, tResult))
	{
		m_strErr = m_commSock.GetLastError();
		WriteLogFile(m_strErr, LOG_ERROR);
		m_commSock.XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
	}
	return;
}

//DEL void CMainTestPro::MsgTaskResult()
//DEL {
//DEL 	CString strOpLine;
//DEL 	return;
//DEL }