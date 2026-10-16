// MainTestPro.h: interface for the CMainTestPro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINTESTPRO_H__643E1721_F8AE_4DA4_B495_6B636E345EF1__INCLUDED_)
#define AFX_MAINTESTPRO_H__643E1721_F8AE_4DA4_B495_6B636E345EF1__INCLUDED_

#include "NetCommand.h"
//#include "CfgFilePro.h"
//#include "DevicePro.h"
#include "GraphPro.h"
#include "TPInfoManager.h"
#include "TaskPro.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainTestPro  
{
public:
	CMainTestPro();
	virtual ~CMainTestPro();

//Export function
public:	
	BOOL Run(void);
	void Stop(void);
	BOOL ShowDlg(void);
	TCHAR* GetLastError(TCHAR** lpszErr = NULL);
//Task message process
private:
	void MsgTaskPro(Command_RunTask data);

//Private function
private:
	BOOL StartSocket(void);
	void WaitForClient(void);
	void EventPro(void);
	void MessagePro(void);
	void WriteLogFile(LPCTSTR tszlogMsg, LOGLEVEL eLevel = LOG_NORMAL);

//Thread function
private:
	static UINT EventWorkThread(LPVOID lpParam);
	static UINT MsgMonitorThread(LPVOID lpParam);
	static UINT SingleEventWorker(LPVOID lpParam);

//Private variable
private:
	//Socket服务运行状态、主线程启动标志位
	BOOL m_bSockState, m_bStartPro;
	//错误信息
	CString m_strErr, m_strUser;
	//主线程流程事件、消息通知事件
	HANDLE m_hMainEvent, m_hMsgEvent;
	//主线程事件类型
	int m_iAct;
	//测试任务所有信息包
//	stTaskInfoPack m_stTaskInfo;
	//底层所需配置接口信息
//	stTransInfo m_stGeneralInterface;
	//网络通信接收内容
	ALL_MSG_DATA m_stSockInfo;
	//Socket通信对象
	CNetCommand m_commSock;	
	//配置文件类成员
//	CCfgFilePro m_cfg;
	//设备操作类成员
//	CDevicePro m_dev;
	// 图形操作类
//	CGraphPro	m_graph;
	// PN索引号，根据PN最后2位数的大小来排列
//	int			m_nPnIndex;
//	CString		m_strProcessType; // 工艺步骤

	CTPInfoManager m_InfoManager;
	CTaskPro        m_TestTaskRuner;
};

#endif // !defined(AFX_MAINTESTPRO_H__643E1721_F8AE_4DA4_B495_6B636E345EF1__INCLUDED_)
