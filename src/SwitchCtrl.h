// TesterControl.h: interface for the CTesterControl class.
// 该类为WSS测试的SwitchBox的驱动类
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTERCONTROL_H__1FCF9F5B_E3C6_4E28_98BF_0334B3A144C6__INCLUDED_)
#define AFX_TESTERCONTROL_H__1FCF9F5B_E3C6_4E28_98BF_0334B3A144C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"

class CSwitchBoxCtrl : public COpComm  
{
public:

	BOOL SelPathByTypeZH(CString strModuleType, int nPath);
	//2代开关盒驱动函数
	BOOL SetTesterZH(int nSw1Port, int nSw2Port);
	BOOL SelPathByTypeHQ(CString strModuleType, int nPath);
	//总部开关盒驱动函数
	BOOL SetTesterHQ(int nSw1Port,int nSw2Port);
	BOOL SelPathByType(CString strModuleType, int nPath);

	//3代开关盒驱动函数
	BOOL SetTesterZH3(int nmodule,int nSw1Port,int nSw2Port);
	BOOL SetZH3HL(int nSW1Port);

	BOOL SetTester(int nSw1Port, int nSw2Port);
	CSwitchBoxCtrl();
	virtual ~CSwitchBoxCtrl();
public:
	int     nUseSwitchType; //使用开关盒的类型0 - 总部，1- ZH 1代，2- ZH 2代，3- ZH 3代
    int     m_nmodule;	
};

#endif // !defined(AFX_TESTERCONTROL_H__1FCF9F5B_E3C6_4E28_98BF_0334B3A144C6__INCLUDED_)
