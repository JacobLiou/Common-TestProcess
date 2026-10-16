// TaskPro.h: interface for the CTaskPro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKPRO_H__E8DC5FC6_174F_484E_BE1B_1B9B7CDC95EA__INCLUDED_)
#define AFX_TASKPRO_H__E8DC5FC6_174F_484E_BE1B_1B9B7CDC95EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetCommand.h"
#include "TPInfoManager.h"
#include "GraphPro.h"
#include "TestTaskManager.h"
#include "..\\include\TaskMenuInterface.h"

class CTaskPro  
{
public:
	BOOL RunTestInit();
	BOOL RunTestTaskItem();
	void SetTPInfoManager(CTPInfoManager* pInfoManger);
	void SetSockDataInfo(ALL_MSG_DATA* pstSockInfo);
	void SetNetCommand(CNetCommand* pNetCommand);
	void SetTaskMenuObj(CTaskMenuPro* pTaskMenu);
	BOOL QuitTestTask();
	CString GetLastErrorMsg();
	CTaskPro();
	virtual ~CTaskPro();
	BOOL TaskProcess(int iTaskIndex);

private:
	CNetCommand* m_pNetCommand;
	ALL_MSG_DATA* m_pstSockInfo;
	//TP信息相关的类
	CTPInfoManager* m_pInfoManger;
	// 图形操作相关的类
	CGraphPro	m_graph;
	//测试任务相关的类
	CTestTaskManager		m_TaskManger;
	//测试任务菜单处理类
	//CTaskMenuPro*  m_pMenu;
	CString m_strError;
	CString m_strProcess;
};

#endif // !defined(AFX_TASKPRO_H__E8DC5FC6_174F_484E_BE1B_1B9B7CDC95EA__INCLUDED_)
