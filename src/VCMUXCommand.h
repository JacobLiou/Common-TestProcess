// VCMUXCommand.h: interface for the CVCMUXCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCMUXCOMMAND_H__708F5762_BDF3_4C60_A1BC_300741996EEE__INCLUDED_)
#define AFX_VCMUXCOMMAND_H__708F5762_BDF3_4C60_A1BC_300741996EEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"

#define SLAVE_WRITE      0XB8
#define SLAVE_READ       0XB9
#define COMMAND_INDEX    0X00

#define VOA_ATTEN_CUSTOMER  0X0100
#define GET_PD_POWER        0X0200
#define MFG_PASSWORD        0x04FA
#define OPLINK_FW_VER       0X0500
#define GET_VOA_SETTING_DAC 0x0508
#define VOA_ACTUAL_ATTEN    0x055C
#define VOA_ACTUAL_DAC      0x0532
#define GET_PD_ADC          0x0586
#define FLASH_ADDR_FOR_READ 0x05B0
#define FLASH_DATA_BY_READ  0x05B4
#define VOA_WORK_MODE       0X05D4
#define RESERVE             0X05E9
#define CRC32               0X05FC
#define READ_TEMP           0x0460

#define ADD16_START_INDEX         0
#define DROP4_START_INDEX         16
#define ADD4_START_INDEX          20

#define MAX_COUNT 1024

class CVCMUXCommand : public COpComm  
{
public:
	double GetTemperature();
	BOOL ReadOPLINKFWVer(char *pchRead);
	BOOL GetVOASettingDAC(int nBlock,int nVOAIndex, PWORD pDACValue);
	BOOL GetMFGModeForDebug(char *pchReadBuf);
	BOOL ReadFlashData(DWORD dwStartAddress,char *pchReadBuf);
	CHAR*  GetMFGMode();
	BOOL SetMFGMode(BOOL bMFGStatus);
	BOOL TxDataToI2C(BYTE byIndex, WORD wAddress, BYTE *pbySetValue, WORD wSetLength, BYTE bytReadLength=0X00,BOOL bSetCommand=TRUE,char *pchRead=NULL);
	//CUSTOMER Command
	BOOL SetVOAAtten(int nBlock,int nVOAIndex,double dblAttenValue);
	BOOL GetPDPower(int nBlock,int nPDIndex,int nPDMonitorPlace,double *pdblPowerValue);

	//MFG COMMAND
	BOOL GetPDADC(int nBlock,int nPDIndex,PWORD pADCValue);
	BOOL GetVOAActualDAC(int nBlock,int nVOAIndex,PWORD pDACValue);
	BOOL SetVOAActualDAC(int nBlock,int nVOAIndex,int nDACValue);
	BOOL SetVOAActualAtten(int nBlock,int nVOAIndex,double dblAttenValue);
	BOOL GetVOAActualAtten(int nBlock,int nVOAIndex,double *pdblAttenValue);
	BOOL GetVOASettingDac(int nBlock,int nVOAIndex,PWORD pnDACValue);

	CVCMUXCommand();
	virtual ~CVCMUXCommand();

};

#endif // !defined(AFX_VCMUXCOMMAND_H__708F5762_BDF3_4C60_A1BC_300741996EEE__INCLUDED_)
