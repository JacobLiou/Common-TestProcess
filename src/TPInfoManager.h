// TPInfoManager.h: interface for the CTPInfoManager class.
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//------该类用于管理TP相关的信息内容，-----------------------------//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TPINFOMANAGER_H__FAB6E8A9_6498_4AA3_BECC_32D6A42D3C81__INCLUDED_)
#define AFX_TPINFOMANAGER_H__FAB6E8A9_6498_4AA3_BECC_32D6A42D3C81__INCLUDED_

#include "NetCommand.h"	// Added by ClassView
#include "XMLControl.h"
#include "..\\include\\TaskMenuInterface.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTPInfoManager  
{
public:
	LPCTSTR GetUISendMTOPN();
	LPCTSTR GetUISendOplinkPN();
	LPCTSTR GetUISendTestMode();
	LPCTSTR GetUISendWorkOder();
	LPCTSTR GetUISendProdFamily();
	LPCTSTR GetUISendAliasName1();
	LPCTSTR GetUISendAliasName2();
	LPCTSTR GetUISendAliasName3();
	BOOL GetUISendIsTestProcess();
	BOOL GetUISendInProcess();
	BOOL GetUISendInHold();
	BOOL GetUISendInReWork();
	BOOL LoadXMLLevelNode(DWORD dwParent);
	LPCTSTR GetUISendUser();
	void InitTestUpdateDutSN();
	LPCTSTR GetUISendSpec();
	LPCTSTR GetUISendSN();
	LPCTSTR GetUISendPN();
	LPCTSTR GetUISendProcess();
	BOOL SendTPTaskDescription();
	void SetSockDataInfo(ALL_MSG_DATA* pstSockInfo);
	void SetNetCommand(CNetCommand* pNetCommand);
	BOOL SendTaskInfo();
	BOOL SendTPDllInfo();
	CTPInfoManager();
	virtual ~CTPInfoManager();

private:
	BOOL LoadTestItemXML2Array();
	BOOL TaskInfoPro(CString& strIndex, CString& strTitle);
	/////////////////// XML 消息封装及发送类
	CNetCommand* m_pNetCommand;
	ALL_MSG_DATA* m_pstSockInfo;
	/////////////// UI 传递下来的PN////////////////
	CString	m_strUISendSN;
	CString m_strUISendPN;
	CString m_strUISendSpec;
	CString m_strUISendUser;
	CString m_strUISendProcessType;

	CString m_strErr;

	stTaskInfoPack m_stTaskInfo;
	// 测试任务序列
	CArray <stTaskItemsID*, stTaskItemsID*> m_AllTestItemArray;
	CXMLControl m_XmlControl;
	CStringArray m_strCfgPNArray; // 在配置文件中所配置的PN列表
	int				m_nCfgPNCount; // 在配置文件中所配置的PN个数
	int				m_nCurrentPNIndex; // 当前所测试的PN在PN列表中的序列编号,在XML配置中编号是从1开始
	CString			m_strCfgSpec; // 配置里的SPEC号

	BOOL	m_bLoadTaskItemCfg; //  是否读取配置，
	BOOL	m_bIsTestProcess; //  是否为测试工序
	CString	m_strTestMode;//测试模式
	CString m_strWorkOlder;//工单
	BOOL	m_bInProcess;//是否移入工序
	BOOL	m_bInHold;//是否被Hold
	BOOL	m_bInReWork;//是否在返修流程
	CString	m_strprdFamily;// 产品族名称
	CString m_strAliasName1;// 产品别名1
	CString m_strAliasName2;// 产品别名2
	CString m_strAliasName3;// 产品别名3
	CString	m_strMTOPN;// MTO PN
	CString m_strOplinkPN;// oplink PN
};

#endif // !defined(AFX_TPINFOMANAGER_H__FAB6E8A9_6498_4AA3_BECC_32D6A42D3C81__INCLUDED_)
