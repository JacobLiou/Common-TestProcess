// NetCommand.cpp: implementation of the CNetCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestProcess.h"
#include "NetCommand.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetCommand::CNetCommand()
{
	m_dwFeedback = 0;
	m_dwMsgBoxID = 0;
	m_bStopTest = FALSE;
	m_bOk = TRUE;
	//Event
	m_hFeedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//Critical section
	InitializeCriticalSection(&m_cs);
	//Socket communication interface
	m_pstUUISendInfo = NULL;
	Initialize();
}

CNetCommand::~CNetCommand()
{
	DeleteCriticalSection(&m_cs);
	FreeLibrary(m_hInst);
	FreeLibrary(m_hXmlInst);
}

//网络消息初始化任务
void CNetCommand::Initialize()
{
	//Initialize
	try
	{
		//Load Socket communication interface
		m_hInst = LoadLibrary("SockCommDll.dll");
		if(m_hInst == NULL)
		{
			throw _T("加载通信模块SockCommDll.dll失败.");
		}
		getLastError = (GetLastErr)GetProcAddress(m_hInst, "GetError");
		if(getLastError == NULL)
		{
			throw _T("加载通信模块接口GetError失败.");
		}
		startSrv = (StartSrv)GetProcAddress(m_hInst, "StartService");
		if(startSrv == NULL)
		{
			throw _T("加载通信模块接口StartService失败.");
		}
		stopSrv = (StopSrv)GetProcAddress(m_hInst, "StopService");
		if(stopSrv == NULL)
		{
			throw _T("加载通信模块接口StopService失败.");
		}
		acqMsgCount = (AcqMsgCount)GetProcAddress(m_hInst, "AcquireMsgCount");
		if(acqMsgCount == NULL)
		{
			throw _T("加载通信模块接口AcquireMsgCount失败.");
		}
		acqClientCount = (AcqClientCount)GetProcAddress(m_hInst, "AcquireClientCount");
		if(acqClientCount == NULL)
		{
			throw _T("加载通信模块接口AcquireClientCount失败.");
		}
		recvMsg = (RecvMsg)GetProcAddress(m_hInst, "ReceiveMsg");
		if(recvMsg == NULL)
		{
			throw _T("加载通信模块接口ReceiveMsg失败.");
		}
		sendMsg = (SendMsg)GetProcAddress(m_hInst, "PostMsg");
		if(sendMsg == NULL)
		{
			throw _T("加载通信模块接口PostMsg失败.");
		}
		sendMsgTo = (SendMsgTo)GetProcAddress(m_hInst, "PostMsgTo");
		if(sendMsgTo == NULL)
		{
			throw _T("加载通信模块接口PostMsgTo失败.");
		}
		//Load XML process interface
		m_hXmlInst = LoadLibrary("CParseXMLDll.dll");
		if(m_hXmlInst == NULL)
		{
			throw _T("加载XML模块CParseXMLDll.dll失败.");
		}
		xmlResolve = (ParseXMLStr)GetProcAddress(m_hXmlInst, "DllParseXMLStr");
		if(xmlResolve == NULL)
		{
			throw _T("加载XML模块接口DllParseXMLStr失败.");
		}
		xmlCreate = (CreateXMLStr)GetProcAddress(m_hXmlInst, "DllCreateXMLStr");
		if(xmlCreate == NULL)
		{
			throw _T("加载XML模块接口DllCreateXMLStr失败.");
		}
		xmlGetError = (GetXMLErrStr)GetProcAddress(m_hXmlInst, "DllGetLastErrorStr");
		if(xmlGetError == NULL)
		{
			throw _T("加载XML模块接口DllGetLastErrorStr失败.");
		} 
		xmlGraphData = (CreateXMLGraphDataStr)GetProcAddress(m_hXmlInst, "DllCreateGraphDataXMLStr");
		if(xmlGraphData == NULL)
		{
			throw _T("加载XML模块接口DllCreateGraphDataXMLStr失败.");
		}
		m_bOk = TRUE;
	}
	catch(char* pszError)
	{
		m_bOk = FALSE;
		m_strErr = pszError;
	}
	catch(...)
	{
		m_bOk = FALSE;
		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
	}
	return;
}

//获取当前错误信息
TCHAR* CNetCommand::GetLastError(TCHAR** lpszErr /* = NULL */)
{
	if(lpszErr != NULL)
	{
		*lpszErr = m_strErr.GetBuffer(0);
	}
	return m_strErr.GetBuffer(0);
}

//接收XML消息
BOOL CNetCommand::XmlRecv(ALL_MSG_DATA& data)
{
	DWORD dwCount = 0;
	BSTR strInput = NULL;
	m_bOk = TRUE;
	try
	{
		//Reset error state	
		//Enter section protect
		EnterCriticalSection(&m_cs);
		//Clear buffer
		memset(m_pszBuf, 0, LENGTH_40K);
		//Receive message
		if(!recvMsg(m_pszBuf, dwCount))
		{
			throw (TCHAR*)getLastError(NULL);
		}
		CString strTemp = m_pszBuf;
		strInput = strTemp.AllocSysString();
		//XML transform
		if(!xmlResolve(strInput, (LPVOID)&data))
		{
			throw _T("XML文件转换失败.");
		}
	}
	catch(char* pszError)
	{
		m_bOk = FALSE;
		m_strErr = pszError;
	}
	catch(...)
	{
		m_bOk = FALSE;
		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
	}
	//Leave section protect
	LeaveCriticalSection(&m_cs);
	if(strInput != NULL)
	{
		::SysFreeString(strInput);
	}
	return m_bOk;
}

//发送XML消息
BOOL CNetCommand::XmlSend(ALL_MSG_DATA& data)
{
	BSTR xmlData = NULL;
	try
	{
		Sleep(20);
		//Enter section protect
		EnterCriticalSection(&m_cs);
		//XML transform
		if(!xmlCreate((LPVOID)&data, (BSTR*)&xmlData))
		{
			throw _T("XML文件转换失败.");
		}
		//Send message
		if(!sendMsgTo(xmlData, *((int*)xmlData - 1), CONN_GUI))
		{
			throw (TCHAR*)getLastError(NULL);
		}
	}
	catch(TCHAR* pszError)
	{
		m_bOk = FALSE;
		m_strErr = pszError;
	}
	catch(...)
	{
		m_bOk = FALSE;
		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
	}
	//Leave section protect
	LeaveCriticalSection(&m_cs);
	//Free memory
	if(xmlData != NULL)
	{
		::SysFreeString(xmlData);
	}
	return m_bOk;
}

//发送对话框消息
DWORD CNetCommand::XmlShowBox(LPCTSTR lpText, LPCTSTR lpCaption, DWORD dwType)
{
	ALL_MSG_DATA commInfo;
	DWORD dwRet = XSB_NONE;
	CString strBtnName, strBtnID, strCaption, strTemp;
	strBtnName.Empty();
	strBtnID.Empty();
	strCaption.Empty();
	//Judge afferent parameters
	if(lpText == NULL)
	{
		m_strErr = _T("XmlShowBox输入参数有误.");
		return FALSE;
	}
	//Set Message type 
	commInfo.nDataType = MSG_MSG_MSGBOX;
	//Set button name
	for(DWORD dwBtn = XSB_OK; dwBtn < XSB_CANCEL; dwBtn <<= 1)
	{
		switch(dwType & dwBtn)
		{
		case XSB_OK:
			strBtnName += _T(";确定");
			strTemp.Format(_T(";%u"), XSB_OK);
			strBtnID += strTemp;
			break;
		case XSB_YES:
			strBtnName += _T(";是");
			strTemp.Format(_T(";%u"), XSB_YES);
			strBtnID += strTemp;
			break;
		case XSB_NO:
			strBtnName += _T(";否");
			strTemp.Format(_T(";%u"), XSB_NO);
			strBtnID += strTemp;
			break;
		case XSB_IGNORE:
			strBtnName += _T(";忽略");
			strTemp.Format(_T(";%u"), XSB_IGNORE);
			strBtnID += strTemp;
			break;
		case XSB_RETRY:
			strBtnName += _T(";重试");
			strTemp.Format(_T(";%u"), XSB_RETRY);
			strBtnID += strTemp;
			break;
		case XSB_ABORT:
			strBtnName += _T(";退出");
			strTemp.Format(_T(";%u"), XSB_ABORT);
			strBtnID += strTemp;
			break;
		case XSB_CANCEL:
			strBtnName += _T(";取消");
			strTemp.Format(_T(";%u"), XSB_CANCEL);
			strBtnID += strTemp;
			break;	
		default:
			break;
		}
	}
	strBtnName = strBtnName.Right(strBtnName.GetLength() - 1);
	strBtnID = strBtnID.Right(strBtnID.GetLength() - 1);
	//Set Message box caption
	if(lpCaption == NULL)
	{
		switch(dwType & HIGH_DWORD)
		{
		case XSB_ICONERROR:
			strCaption = _T("错误");
			break;
		case XSB_ICONWARNING:
			strCaption = _T("警告");
			break;
		case XSB_ICONINFORMATION:
			strCaption = _T("信息");
			break;
		case XSB_ICONQUESTION:
			strCaption = _T("询问");
			break;
		default:
			break;
		}
	}
	else
	{
		strCaption = lpCaption;
	}
	//Set message box format
	//Operate
	strTemp = "ShowBox";
	memcpy(commInfo.unDataStruct.MsgBox.szOperate, strTemp, strTemp.GetLength() + 1);
	//ID
	strTemp.Format("%d", m_dwMsgBoxID++);
	memcpy(commInfo.unDataStruct.MsgBox.szMsgId, strTemp, strTemp.GetLength() + 1);
	//Button ID
	memcpy(commInfo.unDataStruct.MsgBox.szButtonId, strBtnID, strBtnID.GetLength() + 1);
	//Button title
	memcpy(commInfo.unDataStruct.MsgBox.szButtonTitle, strBtnName, strBtnName.GetLength() + 1);
	//Message box title
	memcpy(commInfo.unDataStruct.MsgBox.szMsgTitle, strCaption, strCaption.GetLength() + 1);
	//Message box content
	memcpy(commInfo.unDataStruct.MsgBox.szDescription, lpText, strlen(lpText) + 1);	
	//Send box

	if(!XmlSend(commInfo))
	{
		dwRet = XSB_NONE;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch(dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		return 1;
	default:
		if (m_pstUUISendInfo == NULL)
		{
			return 1;
		}
		dwRet = atoi(m_pstUUISendInfo->unDataStruct.MsgBox.szButtonId);
	}
	ResetEvent(m_hFeedEvent);
	return dwRet;
}

//发送Log信息
BOOL CNetCommand::XmlSendLog(LPCTSTR lpstLog, int iLength, LOGLEVEL eLevel /* = LOG_NORMAL */)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp;
	//Judge afferent parameters
	if(lpstLog == NULL || iLength <= 0)
	{
		m_strErr = _T("XmlSendLog输入参数有误.");
		return FALSE;
	}
	//Set message type
	commInfo.nDataType = MSG_MSG_LOG;
	//Operate
	strTemp = "Show";
	memcpy(commInfo.unDataStruct.MsgLog.szOperate, strTemp, strTemp.GetLength() + 1);
	//Level
	switch(eLevel)
	{
	case LOG_NORMAL:
		strTemp = "Normal";
		break;
	case LOG_WARNING:
		strTemp = "Warning";
		break;
	case LOG_ERROR:
		strTemp = "Error";
		break;
	default:
		strTemp = "Normal";
		break;
	}
	memcpy(commInfo.unDataStruct.MsgLog.szLevel, strTemp, strTemp.GetLength() + 1);
	//Content of log
	memcpy(commInfo.unDataStruct.MsgLog.szDescription, lpstLog, iLength);
	//Send

	return XmlSend(commInfo);
}

//设置反馈接收标志
void CNetCommand::XmlFeedback(ALL_MSG_DATA* pstUUISendInfo)
{
//	m_dwFeedback = dwFeedValue;
	m_pstUUISendInfo = pstUUISendInfo;
	SetEvent(m_hFeedEvent);
}

//发送任务完成结束信息
BOOL CNetCommand::XmlTaskDone(int iTaskIndex, LPCTSTR lpstUserID, TASKRESULT eState /* = TASK_DONE */)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp;
	//Judge afferent parameters
	if(iTaskIndex < 0 || lpstUserID == NULL)
	{
		m_strErr = _T("XmlTaskDone输入参数有误.");
		return FALSE;
	}
	//Set message type
	commInfo.nDataType = MSG_COMMAND_RUNDONE;
	//Result
	switch(eState)
	{
	case TASK_START:
		strTemp = "Start";
		break;
	case TASK_STOP:
		strTemp = "Stop";
		break;
	case TASK_FAIL:
		strTemp = "Fail";
		break;
	case TASK_DONE:
		strTemp = "Done";
		break;
	default:
		strTemp = "Done";
		break;
	}
	memcpy(commInfo.unDataStruct.ComRunDone.szResult, strTemp, strTemp.GetLength() + 1);
	//User
	strTemp = lpstUserID;
	memcpy(commInfo.unDataStruct.ComRunDone.szUser, strTemp, strTemp.GetLength() + 1);
	//Send
/////// add by yu 20130723 //////////////////////////////
	strTemp.Format("%d",iTaskIndex);
	memcpy(commInfo.unDataStruct.ComRunDone.szIndex, strTemp, strTemp.GetLength() + 1);
///////End //////////////////////////////////////////////

	return XmlSend(commInfo);
}

//发送测试任务信息
BOOL CNetCommand::XmlTaskInfo(LPCTSTR lpstIndex, LPCTSTR lpstTitle)
{
	//Judge afferent parameters
	if(lpstIndex == NULL || lpstIndex == NULL)
	{
		m_strErr = _T("XmlTaskInfo输入参数有误.");
		return FALSE;
	}
	ALL_MSG_DATA commInfo = {0};
	CString strTemp = WORDS_FEEDBACK;
	//Set message type
	commInfo.nDataType = MSG_MSG_TASKINFO;
	//Operate
	memcpy(commInfo.unDataStruct.MsgTaskInfo.szOperate, strTemp, strTemp.GetLength() + 1);
	//Task index
	memcpy(commInfo.unDataStruct.MsgTaskInfo.szIndex, lpstIndex, strlen(lpstIndex) + 1);
	//Title
	memcpy(commInfo.unDataStruct.MsgTaskInfo.szTitle, lpstTitle, strlen(lpstTitle) + 1);
	//Send

	return XmlSend(commInfo);
}

//发送基本信息
BOOL CNetCommand::XmlBaseInfo(LPCTSTR lpstAuthor, LPCTSTR lpstVersion, LPCTSTR lpstSpec, LPCTSTR lpstPN, LPCTSTR lpstDate,LPCTSTR lpstProcessType)
{
	//Judge afferent parameters
	if(lpstAuthor == NULL || lpstVersion == NULL || lpstSpec == NULL || lpstPN == NULL || lpstDate == NULL)
	{
		m_strErr = _T("XmlBaseInfo输入参数有误.");
		return FALSE;
	}
	ALL_MSG_DATA commInfo = {0};
	CString strTemp = WORDS_FEEDBACK;
	//Set message type
	commInfo.nDataType = MSG_MSG_BASEINFO;
	//Operate
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szOperate, strTemp, strTemp.GetLength() + 1);
	//Author
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szAuthor, lpstAuthor, strlen(lpstAuthor) + 1);
	//Version
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szVersion, lpstVersion, strlen(lpstVersion) + 1);
	//Spec
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szSpec, lpstSpec, strlen(lpstSpec) + 1);
	//PN
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szPnNumber, lpstPN, strlen(lpstPN) + 1);
	//Date
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szDate, lpstDate, strlen(lpstDate) + 1);
	//ProcessType
	memcpy(commInfo.unDataStruct.MsgBaseInfo.szProcessType, lpstProcessType, strlen(lpstProcessType) + 1);
	//Send

	return XmlSend(commInfo);
}

//发送测试任务进度状态
BOOL CNetCommand::XmlTaskStatus(pstTaskInfoPack pstTaskInfo)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp = WORDS_FEEDBACK, strIndex, strStatus;
	//Set message type
	commInfo.nDataType = MSG_MSG_OLDRATE;
	//Operate
	memcpy(commInfo.unDataStruct.MsgOldRate.szOperate, strTemp, strTemp.GetLength() + 1);
	//Message process
	strIndex.Empty();
	strStatus.Empty();
	for(int i = 0; i < pstTaskInfo->iTaskCount; i++)
	{
		strTemp.Format("%d;", pstTaskInfo->pstAllTaskState->iID);
		strIndex += strTemp;
		strTemp.Format("%d;", pstTaskInfo->pstAllTaskState->iStatus);
		strStatus += strTemp;
	}
	//Index
	memcpy(commInfo.unDataStruct.MsgOldRate.szIndex, strIndex, strIndex.GetLength() + 1);
	//Status
	memcpy(commInfo.unDataStruct.MsgOldRate.szRate, strStatus, strStatus.GetLength() + 1);
	//Send

	return XmlSend(commInfo);
}

//发送测试任务详细描述
BOOL CNetCommand::XmlTaskDescrip(int iTaskIndex, LPCTSTR lpstDescrip, int iLength)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp = WORDS_FEEDBACK;
	//Set message type
	commInfo.nDataType = MSG_MSG_TASKDESCRIPTION;
	//Operate
	memcpy(commInfo.unDataStruct.MsgTaskDescription.szOperate, strTemp, strTemp.GetLength() + 1);
	//Index
	strTemp.Format("%d", iTaskIndex);
	memcpy(commInfo.unDataStruct.MsgOldRate.szIndex, strTemp, strTemp.GetLength() + 1);
	//Description
	memcpy(commInfo.unDataStruct.MsgOldRate.szRate, lpstDescrip, iLength);
	//Send

	return XmlSend(commInfo);
}

//发送图像初始化信息
BOOL CNetCommand::XmlGraphInit(stGraphInitInfo& stInfo)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp;
	//Set message type
	commInfo.nDataType = MSG_TESTDATA_GRAPHINIT;
	//Title
	strTemp = stInfo.pszTitle;
	memcpy(commInfo.unDataStruct.TestDataGraphInit.szTitle, strTemp, strTemp.GetLength() + 1);
	//Index
	strTemp.Format("%d", stInfo.iGraphIndex);
	memcpy(commInfo.unDataStruct.TestDataGraphInit.szGraphIndex, strTemp, strTemp.GetLength() + 1);
	//Scale
	for(int i = 0; i < LENGTH_3; i++)
	{
		strTemp.Format("%d;%s;%d;%f;%f", stInfo.stScale[i].bScaleUse, \
			stInfo.stScale[i].pszScaleName, stInfo.stScale[i].bScaleAuto, \
			stInfo.stScale[i].dblScaleStart, stInfo.stScale[i].dblScaleEnd);
		switch(i)
		{
		case 0:
			memcpy(commInfo.unDataStruct.TestDataGraphInit.szX, strTemp, strTemp.GetLength() + 1);
			break;
		case 1:
			memcpy(commInfo.unDataStruct.TestDataGraphInit.szY1, strTemp, strTemp.GetLength() + 1);
			break;
		case 2:
			memcpy(commInfo.unDataStruct.TestDataGraphInit.szY2, strTemp, strTemp.GetLength() + 1);
			break;
		}
	}

	//Send

	return XmlSend(commInfo);
}

//发送图像数据信息
BOOL CNetCommand::XmlGraphData(stGraphDataInfo& stInfo)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp;
	BSTR xmlData = NULL;
	try
	{
		//Set message type
		commInfo.nDataType = MSG_TESTDATA_GRAPHDATAHEAD;
		//Operate
		switch(stInfo.opt)
		{
		case GRAPH_OPT_NEW:
			strTemp = "NewGraph";
			break;
		case GRAPH_OPT_ADDPLOT:
			strTemp = "AddPlot";
			break;
		case GRAPH_OPT_ADDDATA:
			strTemp = "AddData";
			break;
		case GRAPH_OPT_DELPLOT:
			strTemp = "DeletePlot";
			break;
		}
		memcpy(commInfo.unDataStruct.TestDataGraphHead.szOperate, strTemp, strTemp.GetLength() + 1);
		//Title
		strTemp = stInfo.pszPlotTitle;
		memcpy(commInfo.unDataStruct.TestDataGraphHead.szPlotTitle, strTemp, strTemp.GetLength() + 1);
		// Graph Index
		strTemp.Format("%d", stInfo.iGraphIndex);
		memcpy(commInfo.unDataStruct.TestDataGraphHead.szGraphIndex, strTemp, strTemp.GetLength() + 1);
		//plot Index
		strTemp.Format("%d", stInfo.iPlotIndex);
		memcpy(commInfo.unDataStruct.TestDataGraphHead.szPlotIndex, strTemp, strTemp.GetLength() + 1);
		//Color
		strTemp.Format("%u;%u;%u", (BYTE)stInfo.iColor, (BYTE)(stInfo.iColor >> 8), (BYTE)(stInfo.iColor >> 16));
		memcpy(commInfo.unDataStruct.TestDataGraphHead.szColor, strTemp, strTemp.GetLength() + 1);
		//Y scale index
		strTemp.Format("%d", stInfo.iYScaleIndex);
		memcpy(commInfo.unDataStruct.TestDataGraphHead.szYIndex, strTemp, strTemp.GetLength() + 1);
		//Create XML data
		Sleep(20);
		if(!xmlGraphData((LPVOID)&commInfo, stInfo.pdblXData, stInfo.pdblYData, stInfo.iDataCount, &xmlData))
		{
			THROW("XML图像数据文件转换失败.");
		}
		//Send message
		if(!sendMsgTo(xmlData, *((int*)xmlData - 1), CONN_GUI))
		{
			throw (TCHAR*)getLastError(NULL);
		}
	}
	catch(char* pszError)
	{
		m_bOk = FALSE;
		m_strErr = pszError;
	}
	catch(...)
	{
		m_bOk = FALSE;
		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
	}
	//Free memory
	if(xmlData != NULL)
	{
		::SysFreeString(xmlData);
	}
	return m_bOk;
}

//发送进度条
BOOL CNetCommand::XmlSendProgressData(int iFullStep, int iCurrStep)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp;
	//Set message type
	commInfo.nDataType = MSG_TESTDATA_PROGRESS;
	// FULL STEP
	strTemp.Format("%d",iFullStep);
	if(strTemp.GetLength() > TESTDATA_PROGRESS_FULLSTEP_LEN-1)
	{
		m_strErr="进度条数据总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProgress.szFullStep, strTemp, strTemp.GetLength() + 1);
	// Curr Step
	strTemp.Format("%d",iCurrStep);
	if(strTemp.GetLength() > TESTDATA_PROGRESS_STEP_LEN-1)
	{
		m_strErr="进度条数据长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProgress.szStep, strTemp, strTemp.GetLength() + 1);
	//DATE
	CTime sendtime = CTime::GetCurrentTime();
	strTemp = sendtime.Format("%Y-%m-%d %H:%M:%S");
	if(strTemp.GetLength() > TESTDATA_PROGRESS_DATE_LEN-1)
	{
		m_strErr="进度条时间的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProgress.szDate, strTemp, strTemp.GetLength() + 1);

	//Send
	return XmlSend(commInfo);
}

//发送进度条结果
BOOL CNetCommand::XmlSendProcessResult(stProcessDataInfo &stProcessResult)
{
	ALL_MSG_DATA commInfo = {0};
	CString strTemp;
	//Set message type
	commInfo.nDataType = MSG_TESTDATA_PROCESS;
	// ID
//	strTemp.Format("%d",stProcessResult.iDataID);
	strTemp.Format("%d",m_dwMsgBoxID++);
	if(strTemp.GetLength() > TESTDATA_PROCESS_ID_LEN-1)
	{
		m_strErr="测试数据ID总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szId, strTemp, strTemp.GetLength() + 1);

	//OpLine
	strTemp.Format("%s",stProcessResult.pszOpLine);
	if(strTemp.GetLength() > TESTDATA_PROCESS_OPLINE_LEN-1)
	{
		m_strErr="测试数据OPLINE总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szOpLine, strTemp, strTemp.GetLength() + 1);
	//Description
	strTemp.Format("%s",stProcessResult.pszDataName);
	if(strTemp.GetLength() > TESTDATA_PROCESS_DESCRIPTION_LEN-1)
	{
		m_strErr="测试数据DESCRIPTION总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szDescription, strTemp, strTemp.GetLength() + 1);
	//Result
	if(stProcessResult.dbDataResult > -0.01 && stProcessResult.dbDataResult < 0.01)
	{
		strTemp.Format("%.6f",stProcessResult.dbDataResult);
	}
	else
	{
		strTemp.Format("%.3f",stProcessResult.dbDataResult);
	}
	
	if(strTemp.GetLength() > TESTDATA_PROCESS_RESULT_LEN-1)
	{
		m_strErr="测试数据Result总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szResult, strTemp, strTemp.GetLength() + 1);
	//Result
	strTemp.Format("%.3f-%.3f",stProcessResult.dbDataRangeUp,stProcessResult.dbDataRangeLow);
	if(strTemp.GetLength() > TESTDATA_PROCESS_THRESHOLD_LEN-1)
	{
		m_strErr="测试数据Result总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szThreshold, strTemp, strTemp.GetLength() + 1);
	//PASSfail
	if(stProcessResult.dbDataResult > stProcessResult.dbDataRangeLow-0.000001 
		&& stProcessResult.dbDataResult < stProcessResult.dbDataRangeUp+0.000001)
	{
		strTemp = "Pass";
	}
	else
	{
		strTemp = "Fail";
	}

	if(strTemp.GetLength() > TESTDATA_PROCESS_PASSFAIL_LEN-1)
	{
		m_strErr="测试数据PASSfail总长度的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szPassFail, strTemp, strTemp.GetLength() + 1);
	//DATE
	CTime sendtime = CTime::GetCurrentTime();
	strTemp = sendtime.Format("%Y-%m-%d %H:%M:%S");
	if(strTemp.GetLength() > TESTDATA_PROGRESS_DATE_LEN-1)
	{
		m_strErr="进度条时间的字节数大于XML允许的内容!";
		return FALSE;
	}
	memcpy(commInfo.unDataStruct.TestDataProcess.szDate, strTemp, strTemp.GetLength() + 1);
	//Send

	return XmlSend(commInfo);
}

//获取配置路径
BOOL CNetCommand::XmlGetConfigPath(TCHAR* lpszTestDataPath, int iDataPathLen, TCHAR* lpszConfigPath, int iConfigPathLen, TCHAR* lpszFWFilePath, int iFWFilePathLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = true;
	DWORD dwRet = 0;
	CString strTestDataPath, strConfigPath, strFwFilePath;
	strTestDataPath.Empty();
	strConfigPath.Empty();
	strFwFilePath.Empty();
	
	//Set Message type 
	commInfo.nDataType = MSG_MSG_CONFIGPATH;
	
	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgConfigPath.szOperate, strTemp, strTemp.GetLength() + 1);
	
	memcpy(commInfo.unDataStruct.MsgConfigPath.szConfigPath, strConfigPath, strConfigPath.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgConfigPath.szTestDataPath, strTestDataPath, strTestDataPath.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgConfigPath.szFWFilePath, strFwFilePath, strFwFilePath.GetLength() + 1);
	

	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			bRunFlag = true;
			strcpy_s(lpszTestDataPath, iDataPathLen, m_pstUUISendInfo->unDataStruct.MsgConfigPath.szTestDataPath);
			strcpy_s(lpszConfigPath, iConfigPathLen, m_pstUUISendInfo->unDataStruct.MsgConfigPath.szConfigPath);
			strcpy_s(lpszFWFilePath, iFWFilePathLen, m_pstUUISendInfo->unDataStruct.MsgConfigPath.szFWFilePath);
		}
		
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//下载模板
BOOL CNetCommand::XmlDownloadTemplate(LPCTSTR lpProdSN, LPCTSTR lpProcess,LPCTSTR lpFileNme, int iShowData)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = true;
	DWORD dwRet = 0;
	CString strOperate, strSN, strProcess,strFileName,strEmpty,strShowData;
	strOperate.Empty();
	strSN.Empty();
	strProcess.Empty();
	strShowData.Empty();
	strFileName.Empty();

	strSN = lpProdSN;
	strProcess = lpProcess;
	strShowData.Format("%d", iShowData);
	strFileName = lpFileNme;
	//Set Message type 
	commInfo.nDataType = MSG_MSG_TEMPLATE;

	//Operate
	strOperate = "GetTemplate";
	memcpy(commInfo.unDataStruct.MsgConfigPath.szOperate, strOperate, strOperate.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgTemplate.szProdSN, strSN, strSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTemplate.szPorcess, strProcess, strProcess.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTemplate.szKeyData, strShowData, strShowData.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgTemplate.szSpec, strEmpty, strEmpty.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTemplate.szWO, strEmpty, strEmpty.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTemplate.szNoteMsg, strFileName, strFileName.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			CString strResult = m_pstUUISendInfo->unDataStruct.MsgTemplate.szKeyData;
			bRunFlag = atoi(strResult);
			if (!bRunFlag)
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgTemplate.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//下载工单表头
BOOL CNetCommand::XmlDownloadTaskList(LPCTSTR lpProdSN, LPCTSTR lpGroup, LPCTSTR lpPosition,
	TCHAR* lpszSubSN,int iSubSNBufLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = true;
	DWORD dwRet = 0;
	CString strSN, strPosion, strGroup, strEmpty;
	strSN.Empty();
	strPosion.Empty();
	strGroup.Empty();
	strEmpty.Empty();
	if (lpszSubSN == NULL)
	{
		m_strErr = "传入接收指针为空";
		return false;
	}
	//Set Message type 
	commInfo.nDataType = MSG_MSG_TASKLIST;

	//Operate
	CString strTemp = "Ask";
	strSN = lpProdSN;
	strPosion = lpPosition;
	strGroup = lpGroup;
	memcpy(commInfo.unDataStruct.MsgTaskList.szOperate, strTemp, strTemp.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTaskList.szProdSN, strSN, strSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTaskList.szGroup, strGroup, strGroup.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTaskList.szPosition, strPosion, strPosion.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgTaskList.szResult, strEmpty, strEmpty.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTaskList.szNoteMsg, strEmpty, strEmpty.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgTaskList.szSubSN, strEmpty, strEmpty.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			strEmpty = m_pstUUISendInfo->unDataStruct.MsgTaskList.szResult;
			bRunFlag = atoi(strEmpty);
			if (bRunFlag)
			{
				strcpy_s(lpszSubSN, iSubSNBufLen, m_pstUUISendInfo->unDataStruct.MsgTaskList.szSubSN);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgTaskList.szNoteMsg;
			}
			
		}

	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//上传测试数据
BOOL CNetCommand::XmlUploadTestData(LPCTSTR lpProdSN, LPCTSTR lpDataFileName)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = true;
	DWORD dwRet = 0;
	CString strSN, strFileName, strEmpty;
	strSN.Empty();
	strFileName.Empty();
	strEmpty.Empty();

	//Set Message type 
	commInfo.nDataType = MSG_MSG_UPLOADTESTDATA;

	//Operate
	CString strTemp = "Upload";
	strSN = lpProdSN;
	strFileName = lpDataFileName;
	memcpy(commInfo.unDataStruct.MsgUploadData.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgUploadData.szProdSN, strSN, strSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgUploadData.szFileName, strFileName, strFileName.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgUploadData.szResult, strEmpty, strEmpty.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgUploadData.szNoteMsg, strEmpty, strEmpty.GetLength() + 1);
	


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			strEmpty = m_pstUUISendInfo->unDataStruct.MsgUploadData.szResult;
			bRunFlag = atoi(strEmpty);
			if (!bRunFlag)
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgUploadData.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

// add 2021
BOOL CNetCommand::XmlTriggerConverProcess(LPCTSTR lpProdPN, LPCTSTR lpOriginProcess, 
	TCHAR* lpszTargetProcess, int iBufferLen, int iFindType)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strProdPN, strOriginProcess,strType, strTargetProcess;
	strProdPN = lpProdPN;
	strOriginProcess = lpOriginProcess;
	strType.Format("%d", iFindType);
	strTargetProcess.Empty();

	//Set Message type 
	commInfo.nDataType = MSG_MSG_CONVPROCESS;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgConvProcess.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgConvProcess.szProdPN, strProdPN, strProdPN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgConvProcess.szOriginProcess, strOriginProcess, strOriginProcess.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgConvProcess.szConvTyoe, strType, strType.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgConvProcess.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(lpszTargetProcess, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgConvProcess.szReslutProcess);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgConvProcess.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

BOOL CNetCommand::XmlTriggerConversationPN(LPCTSTR lpOriginPN, TCHAR* lpszTargetPN, int iBufferLen, int iCType)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strOriginPN, strType, strTargetPN;

	strOriginPN = lpOriginPN;
	strType.Format("%d", iCType);
	strTargetPN.Empty();

	//Set Message type 
	commInfo.nDataType = MSG_MSG_CONVPN;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgConvPN.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgConvPN.szOriginPN, strOriginPN, strOriginPN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgConvPN.szConvTyoe, strType, strType.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgConvPN.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(lpszTargetPN, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgConvPN.szResultPN);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgConvPN.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//获取工位信息
BOOL CNetCommand::XmlGetStationInfo(LPCTSTR lpStationComputerName, 
	TCHAR* lpszStationID, TCHAR* lpszStationName, int iBufferLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strStationComputerName, strStationID, strStationName;

	strStationComputerName = lpStationComputerName;
	strStationName.Empty();
	strStationID.Empty();

	//Set Message type 
	commInfo.nDataType = MSG_MSG_STATIONINFO;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgStationInfo.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgStationInfo.szPCName, strStationComputerName, strStationComputerName.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgStationInfo.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(lpszStationID, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgStationInfo.szStationID);
				strcpy_s(lpszStationName, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgStationInfo.szStationName);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgStationInfo.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//上传测试结果到OPC
BOOL CNetCommand::XmlTriggerTestResultUpload2OPC(LPCTSTR lpProdSN)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strProdSN;

	strProdSN = lpProdSN;

	//Set Message type 
	commInfo.nDataType = MSG_MSG_TESTRESULTOPC;

	//Operate
	CString strTemp = "Upload";
	memcpy(commInfo.unDataStruct.MsgResultToOPC.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgResultToOPC.szSN, strProdSN, strProdSN.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgResultToOPC.szResult);
			if (iResult >= 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgResultToOPC.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//Trigger Move In
BOOL CNetCommand::XmlTriggerMoveIn(LPCTSTR lpProdSN)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strProdSN;

	strProdSN = lpProdSN;

	//Set Message type 
	commInfo.nDataType = MSG_MSG_MOVEIN;

	//Operate
	CString strTemp = "Trigger";
	memcpy(commInfo.unDataStruct.MsgMoveIn.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgMoveIn.szSN, strProdSN, strProdSN.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgMoveIn.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgMoveIn.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//Trigger Move Out
BOOL CNetCommand::XmlTriggerMoveOut(LPCTSTR lpProdSN)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strProdSN;

	strProdSN = lpProdSN;

	//Set Message type 
	commInfo.nDataType = MSG_MSG_MOVEOUT;

	//Operate
	CString strTemp = "Trigger";
	memcpy(commInfo.unDataStruct.MsgMoveOut.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgMoveOut.szSN, strProdSN, strProdSN.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgMoveOut.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgMoveOut.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//Trigger MIMS账号登录验证
BOOL CNetCommand::XmlTriggerMIMSIDLoginVerify(LPCTSTR lpUserID, LPCTSTR lpPassWord)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strUserID, strPassWord;

	strUserID = lpUserID;
	strPassWord = lpPassWord;

	//Set Message type 
	commInfo.nDataType = MSG_MSG_USERLOGINVERIFY;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgLoginVerify.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgLoginVerify.szUserName, strUserID, strUserID.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgLoginVerify.szPassWord, strPassWord, strPassWord.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgLoginVerify.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgLoginVerify.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//Trigger工位验证
BOOL CNetCommand::XmlTriggerWorkStationVerify(LPCTSTR lpWorkStationID, LPCTSTR lpUserID, LPCTSTR lpCheckType)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strWorkStationID, strUserID, strCheckType;

	strWorkStationID = lpWorkStationID;
	strUserID = lpUserID;
	strCheckType = lpCheckType;
	//Set Message type 
	commInfo.nDataType = MSG_MSG_STATIONVERIFY;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgStationVerify.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgStationVerify.szStationID, strWorkStationID, strWorkStationID.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgStationVerify.szUserID, strUserID, strUserID.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgStationVerify.szType, strCheckType, strCheckType.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgStationVerify.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgStationVerify.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//获取到oplink用户信息
BOOL CNetCommand::XmlGetOplinkUserInfo(LPCTSTR lpOplinkID, TCHAR* ptchrUserName, TCHAR* ptchrDptName, TCHAR* ptchrPosName, TCHAR* ptchrOPCID, int iBufferLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strOplinkID;

	strOplinkID = lpOplinkID;


	//Set Message type 
	commInfo.nDataType = MSG_MSG_OPLINKUSERINFO;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgOplinkUserInfo.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgOplinkUserInfo.szOplinkID, strOplinkID, strOplinkID.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szResult);
			if (iResult > 0)
			{
				
				if (strlen(m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szUserName) > iBufferLen ||
					strlen(m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szDptName) > iBufferLen ||
					strlen(m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szPosName) > iBufferLen ||
					strlen(m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szOPCID) > iBufferLen)
				{
					bRunFlag = false;
					m_strErr = "数据长度超出存储空间长度，请联系开发工程师！";
				}
				else
				{
					strcpy_s(ptchrUserName, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szUserName);
					strcpy_s(ptchrDptName, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szDptName);
					strcpy_s(ptchrPosName, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szPosName);
					strcpy_s(ptchrOPCID, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szOPCID);
					bRunFlag = true;
				}
				

			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgOplinkUserInfo.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//验证员工技能
BOOL CNetCommand::XmlTriggerUserSkillVerify(LPCTSTR lpOPCID, LPCTSTR lpProcess)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strOPCID, strProcess;
	strOPCID = lpOPCID;
	strProcess = lpProcess;
	

	//Set Message type 
	commInfo.nDataType = MSG_MSG_USERSKILLVERIFY;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgUserSkillVerify.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgUserSkillVerify.szOPCID, strOPCID, strOPCID.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgUserSkillVerify.szProcessType, strProcess, strProcess.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgUserSkillVerify.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgUserSkillVerify.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//创建SN路径
BOOL CNetCommand::XmlCreateSNDir(LPCTSTR lpDutSN, LPCTSTR lpRootDir, TCHAR* ptchrOutSNDir, int iBufferLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strDutSN, strRootDir;

	strDutSN = lpDutSN;
	strRootDir = lpRootDir;

	//Set Message type 
	commInfo.nDataType = MSG_MSG_CREATESNDIR;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgCreateSNDir.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgCreateSNDir.szSN, strDutSN, strDutSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgCreateSNDir.szRootDir, strRootDir, strRootDir.GetLength() + 1);

	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgCreateSNDir.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(ptchrOutSNDir, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgCreateSNDir.szNoteMsg);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgCreateSNDir.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//add 2023/9/5
BOOL CNetCommand::XmlGetFWStandardPath(LPCTSTR lpDutPN, TCHAR* ptchrOutFWPN, TCHAR* ptchrOutConfigDir, int iBufferLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strDutPN, strFWPN, strFWPath;

	strDutPN = lpDutPN;
	strFWPN.Empty();
	strFWPath.Empty();

	//Set Message type 
	commInfo.nDataType = MSG_MSG_FWSTANDARDPATH;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgFWStandardPath.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgFWStandardPath.szDUTMTSPN, strDutPN, strDutPN.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgFWStandardPath.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(ptchrOutFWPN, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgFWStandardPath.szFWPN);
				strcpy_s(ptchrOutConfigDir, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgFWStandardPath.szNoteMsg);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgFWStandardPath.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//add 2023/9/5
//获取当前Site/语言/工厂
BOOL CNetCommand::XmlLocation(TCHAR* ptchrLanguage, TCHAR* ptchrLocation, TCHAR* ptchrFactory, int iBufferLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strLanguage, strLocation,strFactory;


	//Set Message type 
	commInfo.nDataType = MSG_MSG_LOCATIONINFO;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgLocationInfo.szOperate, strTemp, strTemp.GetLength() + 1);

	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(ptchrLanguage, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szLanguage);
				strcpy_s(ptchrLocation, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szLocation);
				strcpy_s(ptchrFactory, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szFactory);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//Add 2023/9/5
//通用消息函数
BOOL CNetCommand::XmlGenericFunc(LPCSTR lpstrFuncName, int nInputCount, TCHAR*pInputParam[10], int nOutputCount, char* pOutputResult[10], int iBufferLen)
{
	ALL_MSG_DATA commInfo;
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strFuncName;

	strFuncName = lpstrFuncName;
	//Set Message type 
	commInfo.nDataType = MSG_MSG_GENERICFUNC;

	if (nInputCount > 10 || nOutputCount > 10)
	{
		return false;
	}
	
	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOperate, strTemp, strTemp.GetLength() + 1);

	memcpy(commInfo.unDataStruct.MsgGenericFunc.szFuncName ,strFuncName, strFuncName.GetLength() + 1);

	strTemp.Format("%d", nInputCount);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInputCount, strTemp, strTemp.GetLength() + 1);
	strTemp.Format("%d", nOutputCount);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOutputCount, strTemp, strTemp.GetLength() + 1);
	for (int i = 0; i < MSG_GENERIC_PAR_MAX; i++)
	{
		if (i < nInputCount)
		{
			strTemp = pInputParam[i];
			memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[i], strTemp, strTemp.GetLength() + 1);
		}
		else
		{
			strTemp = "";			
			memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[i], strTemp, strTemp.GetLength() + 1);
		}
		strTemp = "";
		memcpy(commInfo.unDataStruct.MsgGenericFunc.szOutput[i], strTemp, strTemp.GetLength() + 1);
	}


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				for (int nIndex = 0; nIndex < nOutputCount; nIndex++)
				{
					strcpy_s(pOutputResult[nIndex], iBufferLen, m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szOutput[nIndex]);
				}
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

BOOL CNetCommand::XmlGetPrintLabelInfo(LPCTSTR lpDutSN, LPCTSTR lpLableID, LPCTSTR lpPosition, TCHAR* ptchrOutContent, int iBufferLen)
{
	ALL_MSG_DATA commInfo = { 0 };
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strDutSN, strLableID, strPosition, strOutContent;

	strDutSN = lpDutSN;
	strLableID = lpLableID;
	strPosition = lpPosition;
	strOutContent.Empty();
	//Set Message type 
	commInfo.nDataType = MSG_MSG_GENERICFUNC;

	//Operate
	CString strTemp = "Ask";
	CString strFuncName = "GetPrintLabelInfo";
	CString strInputCount = "3";
	CString strOutputCount = "1";
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOperate, strTemp, strTemp.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szFuncName, strFuncName, strFuncName.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInputCount, strInputCount, strInputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOutputCount, strOutputCount, strOutputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[0], strDutSN, strDutSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[1], strLableID, strLableID.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[2], strPosition, strPosition.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(ptchrOutContent, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szOutput[0]);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//上传测试校准时间
BOOL CNetCommand::XmlUploadTestSystemCailbrationTime(LPCTSTR lpUserID)
{
	ALL_MSG_DATA commInfo = { 0 };
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strUserID;

	strUserID = lpUserID;
	//Set Message type 
	commInfo.nDataType = MSG_MSG_GENERICFUNC;

	//Operate
	CString strTemp = "Ask";
	CString strFuncName = "UploadTestSystemCailbrationTime";
	CString strInputCount = "1";
	CString strOutputCount = "0";
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOperate, strTemp, strTemp.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szFuncName, strFuncName, strFuncName.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInputCount, strInputCount, strInputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOutputCount, strOutputCount, strOutputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[0], strUserID, strUserID.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//返回语言/site/工厂
BOOL CNetCommand::XmlGetLocationInfo(TCHAR* ptchrOutLanguge, TCHAR* ptchrOutLocation, TCHAR* ptchrOutFactory, int iBufferLen)
{
	ALL_MSG_DATA commInfo = { 0 };
	BOOL bRunFlag = false;
	DWORD dwRet = 0;

	//Set Message type 
	commInfo.nDataType = MSG_MSG_LOCATIONINFO;

	//Operate
	CString strTemp = "Ask";
	memcpy(commInfo.unDataStruct.MsgLocationInfo.szOperate, strTemp, strTemp.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(ptchrOutLanguge, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szLanguage);
				strcpy_s(ptchrOutLocation, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szLocation);
				strcpy_s(ptchrOutFactory, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szFactory);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgLocationInfo.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//20231024 获取MAC 地址
BOOL CNetCommand::XmlGetMACAddress(LPCTSTR lpDutSN, LPCTSTR lpDutPN, int nMACCount, TCHAR* ptchrOutContent, int iBufferLen)
{
	ALL_MSG_DATA commInfo = { 0 };
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strDutSN, strDutPN, strMACCount;

	strDutSN = lpDutSN;
	strDutPN = lpDutPN;
	strMACCount.Format("%d", nMACCount);
	//Set Message type 
	commInfo.nDataType = MSG_MSG_GENERICFUNC;

	//Operate
	CString strTemp = "Ask";
	CString strFuncName = "GetMACAddress";
	CString strInputCount = "3";
	CString strOutputCount = "1";
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOperate, strTemp, strTemp.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szFuncName, strFuncName, strFuncName.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInputCount, strInputCount, strInputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOutputCount, strOutputCount, strOutputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[0], strDutSN, strDutSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[1], strDutPN, strDutPN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[2], strMACCount, strMACCount.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				strcpy_s(ptchrOutContent, iBufferLen, m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szOutput[0]);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}

//20231024 获取 无源光路理论IL 值
BOOL CNetCommand::XmlGetRoutingTheoryIL(LPCTSTR lpDutSN, LPCTSTR lpDutPortName, LPCTSTR lpWavelength, double* pdbOutTheoryIL, double* pdbOutTheoryILWSplice)
{
	ALL_MSG_DATA commInfo = { 0 };
	BOOL bRunFlag = false;
	DWORD dwRet = 0;
	CString strDutSN, strPortName, strWL;

	strDutSN = lpDutSN;
	strPortName = lpDutPortName;
	strWL = lpWavelength;
	//Set Message type 
	commInfo.nDataType = MSG_MSG_GENERICFUNC;

	//Operate
	CString strTemp = "Ask";
	CString strFuncName = "GetRoutingTheoryIL";
	CString strInputCount = "3";
	CString strOutputCount = "3";
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOperate, strTemp, strTemp.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szFuncName, strFuncName, strFuncName.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInputCount, strInputCount, strInputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szOutputCount, strOutputCount, strOutputCount.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[0], strDutSN, strDutSN.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[1], strPortName, strPortName.GetLength() + 1);
	memcpy(commInfo.unDataStruct.MsgGenericFunc.szInput[2], strWL, strWL.GetLength() + 1);


	if (!XmlSend(commInfo))
	{
		return false;
	}
	//Wait for feedback
	DWORD dwEvent = WaitForSingleObject(m_hFeedEvent, TIME_50MIN);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		break;
	default:
		if (m_pstUUISendInfo != NULL)
		{
			int iResult = atoi(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szResult);
			if (iResult > 0)
			{
				bRunFlag = true;
				*pdbOutTheoryIL = atof(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szOutput[0]);
				*pdbOutTheoryILWSplice = atof(m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szOutput[1]);
			}
			else
			{
				m_strErr = m_pstUUISendInfo->unDataStruct.MsgGenericFunc.szNoteMsg;
			}
		}
	}
	ResetEvent(m_hFeedEvent);
	return bRunFlag;
}