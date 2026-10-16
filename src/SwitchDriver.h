// SwitchDriver.h: interface for the CSwitchDriver class.
//
//////////////////////////////////////////////////////////////////////
#include"OpComm.h"

#if !defined(AFX_SWITCHDRIVER_H__C68CCC46_5A37_43BA_8111_8C514DC7E6BE__INCLUDED_)
#define AFX_SWITCHDRIVER_H__C68CCC46_5A37_43BA_8111_8C514DC7E6BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSwitchDriver  
{
public:
	CSwitchDriver();
	virtual ~CSwitchDriver();
	BOOL OpenSwitch();
	BOOL OPSwitch121();
	BOOL OPSwitch122();
	BOOL DriveSingleSwitchForCriterion(int nmb,BYTE bySwitchCtrIndex=1, BYTE bySwitchIndex=2, BYTE bySwitchPortIndex=1);
    BOOL ReadSingleSwitchStateForCriterion(int nmb,BYTE bySwtchCtrIndex=1, BYTE bySwitchIndex=2, BYTE*pbyPortIndex=0);
	BOOL Control1X64Switch(int nmb=1,int channel=1);
	BOOL Drive1X8Switch( BYTE bySwitchPortIndex,BYTE bySwitchCtrIndex=1);
	COpComm m_Comm1X2Switch;
	COpComm m_CommSwitch1;
	COpComm m_CommSwitch2;
	COpComm m_Comm1X8Switch;


};

#endif // !defined(AFX_SWITCHDRIVER_H__C68CCC46_5A37_43BA_8111_8C514DC7E6BE__INCLUDED_)
