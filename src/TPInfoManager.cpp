// TPInfoManager.cpp: implementation of the CTPInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TPInfoManager.h"

//Define
#define DLL_VERSION			("1.0.0.0")
#define DLL_CMPLDATE		__DATE__
#define DLL_CMPLTIME		__TIME__
#define DLL_AUTHOR			("WanxinW")
#define	DLL_SPEC			("testgroup")
#define	DLL_PN1				("testgroup")
#define	DLL_PN2				("testgroup")


CTPInfoManager::CTPInfoManager()
{
	m_pNetCommand = NULL;
	m_pstSockInfo = NULL;
	//测试任务信息初始化
	m_stTaskInfo.iTaskCount = 0;
	m_stTaskInfo.pstAllTaskState = NULL;
	m_nCfgPNCount = 0;
	m_nCurrentPNIndex = 0; // 
	m_bLoadTaskItemCfg = FALSE;
}

CTPInfoManager::~CTPInfoManager()
{
	if(m_stTaskInfo.pstAllTaskState != NULL)
	{
		delete []m_stTaskInfo.pstAllTaskState;
		m_stTaskInfo.pstAllTaskState = NULL;
	}
	int nTestItemCount = m_AllTestItemArray.GetSize();
	stTaskItemsID* pTemp = NULL;
	if(nTestItemCount > 0)
	{
		for (int i=0;i<nTestItemCount;i++)
		{
			pTemp = m_AllTestItemArray.GetAt(i);
			ASSERT(pTemp != NULL);
			if(pTemp == NULL)
			{
				continue;
			}
			delete pTemp;
			pTemp = NULL;
		}
		m_AllTestItemArray.RemoveAll();
	}	
}

BOOL CTPInfoManager::SendTaskInfo()
{
	//回馈测试任务信息
	CString strIndex, strTitle;
	if (!TaskInfoPro(strIndex, strTitle))
	{
		m_strErr = _T("处理测试任务信息设置出错.");
		m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	if (!m_pNetCommand->XmlTaskInfo(strIndex, strTitle))
	{
		m_strErr = m_pNetCommand->GetLastError();
		m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
	}
	return TRUE;
}

void CTPInfoManager::SetNetCommand(CNetCommand *pNetCommand)
{
	ASSERT(pNetCommand != NULL);
	if(pNetCommand == NULL)
	{
		return;
	}
	m_pNetCommand = pNetCommand;
}

void CTPInfoManager::SetSockDataInfo(ALL_MSG_DATA *pstSockInfo)
{
	ASSERT(pstSockInfo != NULL);
	if(pstSockInfo == NULL)
	{
		return;
	}
	m_pstSockInfo = pstSockInfo;
}


BOOL CTPInfoManager::SendTPTaskDescription()
{
	return TRUE;
}

BOOL CTPInfoManager::TaskInfoPro(CString &strIndex, CString &strTitle)
{
	int iTotalItems = 0;
	int iTaskItems = 0, iOffset = 0, iParent = 0;
	int* piItemsID = NULL;
	CString strTemp, strcfgPath;
	int  nStationFlag = 0;
	BOOL bFuncOk = TRUE;
	stTaskItemsID* pTempTaskItem = NULL;
	CString strPNList;
	CString strFindPN;
	strFindPN.Format("%s", m_strUISendPN);
	if (!m_bLoadTaskItemCfg)
	{
		if (!LoadTestItemXML2Array())
		{
			return FALSE;
		}
		else
		{
			m_bLoadTaskItemCfg = TRUE;
		}
	}
	iTotalItems = m_AllTestItemArray.GetSize();
	try
	{
		//Judge the variable
		if (iTotalItems <= 0)
		{
			throw _T("检测到测试任务数量为0, 请核查.");
		}
		///////////////////// edit by yu 20130722 ///////////////

		piItemsID = new int[iTotalItems];

		////////////////////// end 20130722 //////////////////////
		if (piItemsID == NULL)
		{
			throw _T("申请内存出错.");
		}
		//Clear string
		strIndex.Empty();
		strTitle.Empty();
		//Start resolve
		for (int i = 0; i < iTotalItems; i++)
		{
			pTempTaskItem = m_AllTestItemArray.GetAt(i);
			ASSERT(pTempTaskItem != NULL);
			if (pTempTaskItem == NULL)
			{
				continue;
			}
			switch (pTempTaskItem->type)
			{
			case TASK_RG_OFFSET:
				iOffset = pTempTaskItem->iID;
				break;
			case TASK_RG_CLASS:
				iParent = pTempTaskItem->iID;
				strTemp.Format("%d;", pTempTaskItem->iID);
				strIndex += strTemp;
				strTemp.Format("%s;", pTempTaskItem->pszName);
				strTitle += strTemp;
				break;
			case TASK_RG_PARENT:
				iParent = pTempTaskItem->iID;
				strTemp.Format("%d;", pTempTaskItem->iID);
				strIndex += strTemp;
				strTemp.Format("%s;", pTempTaskItem->pszName);
				strTitle += strTemp;
				break;
			case TASK_RG_CHILD:
				//////////////////////// Edit by yu 20130723 //////////////////////////////////////
				strTemp.Format("%s", pTempTaskItem->pszProcessTypeList);
				strPNList.Format("%s", pTempTaskItem->pszPnList);
				if (strTemp.Find(m_strUISendProcessType) >= 0 && strPNList.Find(strFindPN) >= 0)
				{

					//Save the effective task item ID
					piItemsID[iTaskItems] = pTempTaskItem->iID + iParent + iOffset;
					//Set XML information
					strTemp.Format("%d;", piItemsID[iTaskItems]);
					strIndex += strTemp;
					strTemp.Format("%s;", pTempTaskItem->pszName);
					strTitle += strTemp;
					//Next task item
					iTaskItems++;
				}

				//////////////////////////////////End  ////////////////////////////////////////
				break;
			default:
				return FALSE;
			}
		}
		//Check the count of effective task items
		if (iTaskItems == 0)
		{
			throw _T("检测到测试任务数量为0, 请核查.");
		}
		//Record task count
		m_stTaskInfo.iTaskCount = iTaskItems;
		//Tasks status initialize
		if (m_stTaskInfo.pstAllTaskState != NULL)
		{
			delete[]m_stTaskInfo.pstAllTaskState;
			m_stTaskInfo.pstAllTaskState = NULL;
		}
		m_stTaskInfo.pstAllTaskState = new stOneTaskStatus[iTaskItems];
		if (m_stTaskInfo.pstAllTaskState == NULL)
		{
			throw _T("申请内存出错.");
		}
		memset(m_stTaskInfo.pstAllTaskState, 0, sizeof(stOneTaskStatus) * iTaskItems);
		//Set task info
		memcpy(m_stTaskInfo.pstAllTaskState, piItemsID, iTaskItems * sizeof(int));
	}
	catch (TCHAR* pszErr)
	{
		bFuncOk = FALSE;
		m_strErr = pszErr;
		m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
	}
	catch (...)
	{
		bFuncOk = FALSE;
		m_strErr.Format(_T("未知错误. 错误代号: 0x08X"), ::GetLastError());
		m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
	}
	//Free memory
	if (piItemsID != NULL)
	{
		delete[]piItemsID;
	}
	//return
	return bFuncOk;
}
LPCTSTR CTPInfoManager::GetUISendProcess()
{
	return m_strUISendProcessType;
}
LPCTSTR CTPInfoManager::GetUISendPN()
{
	return m_strUISendPN;
}

LPCTSTR CTPInfoManager::GetUISendSN()
{
	return m_strUISendSN;
}

LPCTSTR CTPInfoManager::GetUISendSpec()
{
	return m_strUISendSpec;
}

void CTPInfoManager::InitTestUpdateDutSN()
{
	if(m_pstSockInfo->nDataType == MSG_COMMAND_RUNINIT)
	{	
		m_strUISendSN.Format("%s",m_pstSockInfo->unDataStruct.ComRunInitData.szSN);
		m_strUISendUser.Format("%s",m_pstSockInfo->unDataStruct.ComRunInitData.szUser);
	//	m_strUISendProcessType.Format("%s",m_pstSockInfo->unDataStruct.MsgBaseInfo.szProcessType);
	}
}

LPCTSTR CTPInfoManager::GetUISendUser()
{
	return m_strUISendUser;
}

BOOL CTPInfoManager::LoadTestItemXML2Array()
{
	TCHAR tszAppFolder[MAX_PATH];
	CString strItemCfgPath;
	GetCurrentDirectory(256, tszAppFolder);
	strItemCfgPath.Format("%s\\TP-TASK-INFO.xml", tszAppFolder);
	if (GetFileAttributes(strItemCfgPath) != -1)
	{
		if (!m_XmlControl.LoadXmlFile(strItemCfgPath))
		{
			m_strErr = "加载测试项目配置XML文件失败!";
			return FALSE;
		}
		DWORD   dwBuf[256];
		int		nNodeCount = 0;
		CString strTemp;
		CString strErrMsg;

		try
		{
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			nNodeCount = 0;
			m_XmlControl.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
			if (nNodeCount == 0)
				throw "XML文件结点不完整,TP-TASK 主节点不存在!";
			if (!m_XmlControl.GetNodeAttr("NodeValue", strTemp, dwBuf[0]))
			{
				throw "XML文件结点不完整,TP-TASK 主节点SPEC 属性不存在!";
			}
			m_strCfgSpec = strTemp;

			if (!m_XmlControl.GetNodeAttr("PNCount", strTemp, dwBuf[0]))
			{
				throw "XML文件结点不完整,TP-TASK 主节点 PN-Count 属性不存在!";
			}
			m_nCfgPNCount = atoi(strTemp);
			////PN ////// 		
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			nNodeCount = 0;
			m_XmlControl.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
			if (nNodeCount == 0)
				throw "XML文件结点不完整,TP-TASK 主节点不存在!";
			m_strCfgPNArray.RemoveAll();
			for (int i = 0;i<nNodeCount;i++)
			{
				if (!m_XmlControl.GetNodeAttr("NodeValue", strTemp, dwBuf[i]))
				{
					throw "XML文件结点不完整,TP-TASK 主节点 PN-Count 属性不存在!";
				}
				m_strCfgPNArray.Add(strTemp);
			}

			///// 根节点///////////////////////////////////////////////////////////
			stTaskItemsID* pTaskItemID = new stTaskItemsID();
			ASSERT(pTaskItemID != NULL);
			if (pTaskItemID != NULL)
			{
				pTaskItemID->type = TASK_RG_OFFSET;
				pTaskItemID->iID = TASK_OFF_0;
				//				pTaskItemID->pszProcessTypeList = "ALL";
				//				pTaskItemID->pszPnList = "1";
				strcpy(pTaskItemID->pszProcessTypeList, "ALL");
				strcpy(pTaskItemID->pszPnList, "1");
			}
			m_AllTestItemArray.Add(pTaskItemID);
			///////////////// LEVEL ///////////////////////////
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			nNodeCount = 0;
			m_XmlControl.FindNodeByTag(strErrMsg, "BaseName", "LEVEL", dwBuf, nNodeCount);
			if (nNodeCount == 0)
				throw "XML文件结点不完整,TP-TASK 主节点不存在!";
			for (int nLevelIndex = 0;nLevelIndex < nNodeCount;nLevelIndex++)
			{
				if (!LoadXMLLevelNode(dwBuf[nLevelIndex]))
				{
					throw "XML文件结点不完整,Level 节点解析出错不存在!";
				}
			}

		}
		catch (LPTSTR lpErr)
		{
			if (!m_XmlControl.CloseXMLFile())
			{
				m_strErr = "无纸化关闭失败,请通知相关工程师!";
				return FALSE;
			}
			m_strErr.Format("%s", lpErr);
			return FALSE;
		}

	}
	else
	{
		m_strErr = "TP-TASK-INFO.XML文件不存在";
		return FALSE;
	}
	if (!m_XmlControl.CloseXMLFile())
	{
		m_strErr = "无纸化关闭失败,请通知相关工程师!";
		return FALSE;
	}
	return TRUE;
}

BOOL CTPInfoManager::LoadXMLLevelNode(DWORD dwParent)
{
	DWORD   dwLevelBuf[256];
	int		nLevelNodeCount = 0;
	CString strTemp;
	CString strLevel;
	CString strTitle;
	CString strNodeValue;
	CString strMsg;
	int		 nEnable = 1;
	int		nTaskIndex = 0;
	CString strProcess;
	CString strPNIndexList;

	CString strErrMsg;


	TASKRANGETYPE Tasktype;//Item type
	int	TaskiID;//Item ID

	try
	{
		//// 解析自身主节点 ///////////////////		
		if (!m_XmlControl.GetNodeAttr("Title", strTitle, dwParent))
		{
			throw "XML文件结点不完整,Level 主节点 Title 属性不存在!";
		}

		if (!m_XmlControl.GetNodeAttr("Enable", strTemp, dwParent))
		{
			throw "XML文件结点不完整,Level 主节点 Enable 属性不存在!";
		}
		nEnable = atoi(strTemp);
		if (nEnable <1) // Enable 节点为0 及不解析该节点
		{
			return TRUE;
		}
		if (!m_XmlControl.GetNodeAttr("ProcessTypeList", strProcess, dwParent))
		{
			throw "XML文件结点不完整,Level 主节点 ProcessTypeList 属性不存在!";
		}
		if (!m_XmlControl.GetNodeAttr("PNList", strPNIndexList, dwParent))
		{
			throw "XML文件结点不完整,Level 主节点 PNList 属性不存在!";
		}
		if (!m_XmlControl.GetNodeAttr("Attribute", strLevel, dwParent))
		{
			throw "XML文件结点不完整,Level 主节点 Attribute 属性不存在!";
		}
		strNodeValue = "跟节点";
		//		if(!m_XmlControl.GetNodeAttr("NodeValue", strNodeValue, dwParent))
		//		{
		//			throw "XML文件结点不完整,Level 主节点值不存在!";
		//		}
		//////////////////////////////////////////////////////////////////////

		strTemp = strLevel.Right(1);
		switch (atoi(strTemp))
		{
		case 1:
			Tasktype = TASK_RG_CLASS;
			TaskiID = TASK_OFF_1000;
			break;
		case 2:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_2000;
			break;
		case 3:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_3000;
			break;
		case 4:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_4000;
			break;
		case 5:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_5000;
			break;
		case 6:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_6000;
			break;
		case 7:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_7000;
			break;
		case 8:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_8000;
			break;
		case 9:
			Tasktype = TASK_RG_PARENT;
			TaskiID = TASK_OFF_9000;
			break;
		default:
			throw "XML文件结点不完整,Level 应在1到9之间!";
		}
		stTaskItemsID* pTaskItemID = new stTaskItemsID();
		ASSERT(pTaskItemID != NULL);
		if (pTaskItemID != NULL)
		{

			pTaskItemID->type = Tasktype;
			pTaskItemID->iID = TaskiID;
			strcpy(pTaskItemID->pszProcessTypeList, strProcess.GetBuffer(strProcess.GetLength() + 1));
			strcpy(pTaskItemID->pszPnList, strPNIndexList.GetBuffer(strPNIndexList.GetLength() + 1));
			strcpy(pTaskItemID->pszName, strTitle.GetBuffer(strTitle.GetLength() + 1));
			strcpy(pTaskItemID->pszDecription, strNodeValue.GetBuffer(strNodeValue.GetLength() + 1));

			m_AllTestItemArray.Add(pTaskItemID);

			strProcess.ReleaseBuffer();
			strPNIndexList.ReleaseBuffer();
			strTitle.ReleaseBuffer();
			strNodeValue.ReleaseBuffer();
		}
		/////// 获取该节点下的TestNode 的值
		ZeroMemory(dwLevelBuf, 256 * sizeof(DWORD));
		nLevelNodeCount = 0;
		m_XmlControl.FindNodeByTag(strErrMsg, "BaseName", "TaskNode", dwLevelBuf, nLevelNodeCount, dwParent);
		for (int nTaskNodeIndex = 0;nTaskNodeIndex<nLevelNodeCount;nTaskNodeIndex++)
		{
			if (!m_XmlControl.GetNodeAttr("Index", strTemp, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点 Index 属性不存在!";
			}
			nTaskIndex = atoi(strTemp);
			if (!m_XmlControl.GetNodeAttr("Title", strTitle, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点 Title 属性不存在!";
			}

			if (!m_XmlControl.GetNodeAttr("Enable", strTemp, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点 Enable 属性不存在!";
			}
			nEnable = atoi(strTemp);
			if (nEnable <1) // Enable 节点为0 及不解析该节点
			{
				continue;
			}
			if (!m_XmlControl.GetNodeAttr("ProcessTypeList", strProcess, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点 ProcessTypeList 属性不存在!";
			}
			if (!m_XmlControl.GetNodeAttr("PNList", strPNIndexList, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点 PNList 属性不存在!";
			}
			if (!m_XmlControl.GetNodeAttr("Attribute", strLevel, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点 Attribute 属性不存在!";
			}
			if (!m_XmlControl.GetNodeAttr("NodeValue", strNodeValue, dwLevelBuf[nTaskNodeIndex]))
			{
				throw "XML文件结点不完整,Level 主节点值不存在!";
			}

			stTaskItemsID* pTaskItemID = new stTaskItemsID();
			ASSERT(pTaskItemID != NULL);
			if (pTaskItemID != NULL)
			{

				pTaskItemID->type = TASK_RG_CHILD;
				pTaskItemID->iID = nTaskIndex;
				strcpy(pTaskItemID->pszProcessTypeList, strProcess.GetBuffer(strProcess.GetLength() + 1));
				strcpy(pTaskItemID->pszPnList, strPNIndexList.GetBuffer(strPNIndexList.GetLength() + 1));
				strcpy(pTaskItemID->pszName, strTitle.GetBuffer(strTitle.GetLength() + 1));
				strcpy(pTaskItemID->pszDecription, strNodeValue.GetBuffer(strNodeValue.GetLength() + 1));

				m_AllTestItemArray.Add(pTaskItemID);

				strProcess.ReleaseBuffer();
				strPNIndexList.ReleaseBuffer();
				strTitle.ReleaseBuffer();
				strNodeValue.ReleaseBuffer();
			}
		}
		//////////// Level /////////////////////////
		ZeroMemory(dwLevelBuf, 256 * sizeof(DWORD));
		nLevelNodeCount = 0;
		m_XmlControl.FindNodeByTag(strErrMsg, "BaseName", "LEVEL", dwLevelBuf, nLevelNodeCount, dwParent);
		for (int nLevelIndex = 0;nLevelIndex < nLevelNodeCount;nLevelIndex++)
		{
			if (!LoadXMLLevelNode(dwLevelBuf[nLevelIndex]))
			{
				throw "XML文件结点不完整,Level 节点解析出错不存在!";
			}
		}

	}
	catch (LPTSTR lpErr)
	{
		m_strErr.Format("%s", lpErr);
		return FALSE;
	}

	return TRUE;
}
BOOL CTPInfoManager::SendTPDllInfo()
{
   //	TCHAR	pszSpecList[][16] = {};

	ASSERT(m_pNetCommand != NULL);
	ASSERT(m_pstSockInfo != NULL);
	if (m_pNetCommand == NULL || m_pstSockInfo == NULL)
	{
		return FALSE;
	}

	CString strAuthor = DLL_AUTHOR, strVersion = DLL_VERSION, strDate, strSpec;
	strDate.Format("%s %s", DLL_CMPLDATE, DLL_CMPLTIME);
	strSpec = DLL_SPEC;
	CString strPN1 = DLL_PN1;
	CString strPN2 = DLL_PN1;
	/////////////////// End 此处正式版本时收到BaseInfo消息后需要核对SPEC及PN///////////
	m_strUISendPN.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szPnNumber);
	m_strUISendSpec.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szSpec);
	m_strUISendProcessType.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szProcessType);
	// add 2021
	m_strTestMode.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szTestMode);
	m_strWorkOlder.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szMFGOrder);
	m_strprdFamily.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szProductFamily);
	m_strAliasName1.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szAliasName1);
	m_strAliasName2.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szAliasName2);
	m_strAliasName3.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szAliasName3);
	m_strMTOPN.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szMTOPN);
	m_strOplinkPN.Format("%s", m_pstSockInfo->unDataStruct.MsgBaseInfo.szOplinkPN);

	int iFlag = atoi(m_pstSockInfo->unDataStruct.MsgBaseInfo.szInProcess);
	if (iFlag > 0)
	{
		m_bInProcess = true;
	}
	else
	{
		m_bInProcess = true;
	}
	iFlag = atoi(m_pstSockInfo->unDataStruct.MsgBaseInfo.szInHold);
	if (iFlag > 0)
	{
		m_bInHold = true;
	}
	else
	{
		m_bInHold = false;
	}
	iFlag = atoi(m_pstSockInfo->unDataStruct.MsgBaseInfo.szInRework);
	if (iFlag > 0)
	{
		m_bInReWork = true;
	}
	else
	{
		m_bInReWork = false;
	}
	iFlag = atoi(m_pstSockInfo->unDataStruct.MsgBaseInfo.szIsTestSpec);
	if (iFlag > 0)
	{
		m_bIsTestProcess = true;
	}
	else
	{
		m_bIsTestProcess = false;
	}
	// end
	if (strPN1.CompareNoCase(m_strUISendPN) == 0)
	{
		if (!m_pNetCommand->XmlBaseInfo(strAuthor, strVersion, strSpec, strPN1, strDate, m_strUISendProcessType))
		{
			m_strErr = m_pNetCommand->GetLastError();
			m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}
	else if (strPN2.CompareNoCase(m_strUISendPN) == 0)
	{
		if (!m_pNetCommand->XmlBaseInfo(strAuthor, strVersion, strSpec, strPN2, strDate, m_strUISendProcessType))
		{
			m_strErr = m_pNetCommand->GetLastError();
			m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}
	else
	{

		m_strErr.Format("测试TP的PN为%s,而UI传入的PN 为 %s,两者不相符，请检查！", strPN1, strPN1, m_strUISendPN);
		m_pNetCommand->XmlSendLog(m_strErr, m_strErr.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

LPCTSTR CTPInfoManager::GetUISendMTOPN()
{
	return m_strMTOPN;
}
LPCTSTR CTPInfoManager::GetUISendOplinkPN()
{
	return m_strOplinkPN;
}
LPCTSTR CTPInfoManager::GetUISendTestMode()
{
	return m_strTestMode;
}
LPCTSTR CTPInfoManager::GetUISendWorkOder()
{
	return m_strWorkOlder;
}
LPCTSTR CTPInfoManager::GetUISendProdFamily()
{
	return m_strprdFamily;
}
LPCTSTR CTPInfoManager::GetUISendAliasName1()
{
	return m_strAliasName1;
}
LPCTSTR CTPInfoManager::GetUISendAliasName2()
{
	return m_strAliasName2;
}
LPCTSTR CTPInfoManager::GetUISendAliasName3()
{
	return m_strAliasName1;
}
BOOL CTPInfoManager::GetUISendIsTestProcess()
{
	return m_bIsTestProcess;
}
BOOL CTPInfoManager::GetUISendInProcess()
{
	return m_bInProcess;
}
BOOL CTPInfoManager::GetUISendInHold()
{
	return m_bInHold;
}
BOOL CTPInfoManager::GetUISendInReWork()
{
	return m_bInReWork;
}




