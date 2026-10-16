// TaskPro.cpp: implementation of the CTaskPro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestProcess.h"
#include "TaskPro.h"
#include "../include/DataSrc.h"
//#include "CalibratePro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTaskPro::CTaskPro()
{
	m_pNetCommand = NULL;
	m_pstSockInfo = NULL;
	m_pInfoManger = NULL;
}

CTaskPro::~CTaskPro()
{

}

BOOL CTaskPro::TaskProcess(int iTaskIndex)
{
	BOOL bFuncOk = TRUE;
	bFuncOk = m_TaskManger.RunTestTask(iTaskIndex);
	return bFuncOk;
}
BOOL CTaskPro::QuitTestTask()
{
	return TRUE;
}
CString CTaskPro::GetLastErrorMsg()
{
	return m_strError;
}

void CTaskPro::SetNetCommand(CNetCommand *pNetCommand)
{
	ASSERT(pNetCommand != NULL);
	if(pNetCommand == NULL)
	{
		return;
	}
	m_pNetCommand = pNetCommand;
	m_graph.SetNetCommand(pNetCommand);
	m_TaskManger.SetNetCommand(pNetCommand);
	m_TaskManger.SetGraphPro(&m_graph);	
}
void CTaskPro::SetTaskMenuObj(CTaskMenuPro* pTaskMenu)
{
	ASSERT(pTaskMenu != NULL);
	if(pTaskMenu == NULL)
	{
		return;
	}
	//m_pMenu = pTaskMenu;
}

void CTaskPro::SetSockDataInfo(ALL_MSG_DATA *pstSockInfo)
{
	ASSERT(pstSockInfo != NULL);
	if(pstSockInfo == NULL)
	{
		return;
	}
	m_pstSockInfo = pstSockInfo;
}

void CTaskPro::SetTPInfoManager(CTPInfoManager *pInfoManger)
{
	ASSERT(pInfoManger != NULL);
	if(pInfoManger == NULL)
	{
		return;
	}
	m_pInfoManger = pInfoManger;
	m_TaskManger.SetTPInfoManager(pInfoManger);
}


BOOL CTaskPro::RunTestTaskItem()
{
	BOOL	bRunFalg = TRUE;
	TASKRESULT tResult = TASK_DONE;
	CString strTemp;
	CString strMsg;
	CString strErrorMsg;
	CString strRunTaskUser;
	strRunTaskUser.Format("%s", m_pstSockInfo->unDataStruct.ComRunTaskData.szUser);
	strRunTaskUser.TrimLeft();
	strRunTaskUser.TrimRight();
	int nRawTaskIndex = atoi(m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex);
	if (nRawTaskIndex < TASK_OFF_1000)
	{
		if (nRawTaskIndex < 0)
		{
			strTemp.Format(_T("接收任务信息有误. 任务序号:%s, 任务名称:%s"),
				m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
				m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);
			m_pNetCommand->XmlSendLog(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			bRunFalg = FALSE;
		}
		else
		{
			strTemp.Format(_T("正在处理预设任务. 任务序号:%s, 任务名称:%s"),
				m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
				m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);
			m_pNetCommand->XmlSendLog(strTemp, strTemp.GetLength() + 1);
			//	bRunFalg = m_CienaMiniEdfaTest.RunDefaultTask(nRawTaskIndex);

			strMsg.Format(_T("处理任务成功. 任务序号:%s, 任务名称:%s"),
				m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
				m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);


			strErrorMsg.Format(_T("正在处理失败. 任务序号:%s, 任务名称:%s"),
				m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
				m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);

			bRunFalg = m_TaskManger.RunTestTask(nRawTaskIndex);

		}
	}
	else
	{
		int iTaskIndex = atoi(m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex) % TASK_OFF_1000;
		strTemp.Format(_T("正在处理任务. 任务序号:%s, 任务名称:%s"),
			m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
			m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);
		m_pNetCommand->XmlSendLog(strTemp, strTemp.GetLength() + 1);

		strMsg.Format(_T("处理任务成功. 任务序号:%s, 任务名称:%s"),
			m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
			m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);


		strErrorMsg.Format(_T("正在处理失败. 任务序号:%s, 任务名称:%s"),
			m_pstSockInfo->unDataStruct.ComRunTaskData.szIndex,
			m_pstSockInfo->unDataStruct.ComRunTaskData.szTitle);

		bRunFalg = m_TaskManger.RunTestTask(iTaskIndex);

	}
	Sleep(100);
	if (bRunFalg)
	{
		if (!m_pNetCommand->XmlTaskDone(nRawTaskIndex, strRunTaskUser, tResult))
		{
			strTemp = m_pNetCommand->GetLastError();
			m_pNetCommand->XmlSendLog(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		}
		m_pNetCommand->XmlSendLog(strMsg, strMsg.GetLength() + 1);
	}
	else
	{

		m_pNetCommand->XmlSendLog(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_ERROR);
		tResult = TASK_FAIL;
		m_pNetCommand->XmlTaskDone(nRawTaskIndex, strRunTaskUser, tResult);
		//if(m_pNetCommand->XmlStopTask())
		//{
		//	tResult = TASK_STOP;
		//	m_pNetCommand->XmlResetStop();
		//}
		//else
		//{

		//}
	}

	return bRunFalg;
}


BOOL CTaskPro::RunTestInit()
{
	BOOL	bRunInit = TRUE;
	m_pInfoManger->InitTestUpdateDutSN();

	bRunInit = m_TaskManger.RunTestInit();
	
	CString strRunTaskUser = m_pInfoManger->GetUISendUser();
	CString strTemp;
	TASKRESULT tResult;
	if(bRunInit)
	{
		tResult = TASK_DONE;
		strTemp = "测试初始化完成!";
	}
	else
	{
		tResult = TASK_FAIL;
		strTemp = "测试初始化失败!";
	}
	
	
	if(!m_pNetCommand->XmlTaskDone(TEST_INIT,strRunTaskUser, tResult))
	{
		strTemp = m_pNetCommand->GetLastError();
		m_pNetCommand->XmlSendLog(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
	}
	m_pNetCommand->XmlSendLog(strTemp, strTemp.GetLength() + 1);
	return bRunInit;
}
