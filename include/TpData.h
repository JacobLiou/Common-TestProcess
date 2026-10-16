#ifndef _TP_HEAD_DATA
#define _TP_HEAD_DATA

#include "DataSrc.h"
#include "NetCommInterface.h"
#include "GraphInterface.h"

enum MODBASICINFOLENS
{
	//Info
	BSLENS_MODULE_TYPE = 32,
	BSLENS_VENDOR = 20,
	BSLENS_SN = 32,
	BSLENS_PN = 32,
	BSLENS_MFG_DATA = 16,
	BSLENS_CAL_DATA = 16,
	BSLENS_INFO_REVERSE = 24,
	//Version
	BSLENS_BUNDLE_VER = 16,
	BSLENS_BOOT_VER = 16,
	BSLENS_MAIN_VER = 16,
	BSLENS_FM_VER = 16,
	BSLENS_HW_VER = 16,
	BSLENS_PCB_VER = 16,
	BSLENS_PCBA_VER = 16,
	BSLENS_FPGA_VER = 16,
	BSLENS_VER_REVERSE = 24
};

//Serial port information
typedef struct STSERIALPORTINFO
{
	//Name of com
	int iCom;
	//Baud rate
	int iBaudrate;
	//Time out
	int iTimeOut;
	//Length if byte data
	int iByteSize;
	//Parity check
	int iParity;
	//Stop bit
	int iStopBits;
}stSerialPortInfo, *pstSerialPortInfo;

typedef struct STMODBASEINFO
{
	//Basic Information
	TCHAR tszProductType[LENGTH_32];
	TCHAR tszVendorNumber[LENGTH_16];
	TCHAR tszSerialNumber[LENGTH_32];
	TCHAR tszPartNumber[LENGTH_32];
	TCHAR tszMFGDate[LENGTH_12];
	TCHAR tszCaliDate[LENGTH_12];
	TCHAR tszBootVer[LENGTH_12];
	TCHAR tszMainVer[LENGTH_12];
	TCHAR tszFwVer[LENGTH_12];
	TCHAR tszHwVer[LENGTH_12];
	TCHAR tszPCBVer[LENGTH_12];
	TCHAR tszPCBAVer[LENGTH_12];
	TCHAR tszFPGAVer[LENGTH_12];
	TCHAR tszReverse[LENGTH_32];
}stModBaseInfo, *pstModBaseInfo;


//Total information
typedef struct STMODTOTALINFO
{
	stModBaseInfo stBase;
//	stModVerInfo stVer;
}stModTotalInfo, *pstModTotalInfo;


typedef struct STSINGLEDEVCFG
{
	TCHAR lpstName[LENGTH_32];
	int iDevType;
	int iConnType;
	int iConnAddr;
}stSingleDevCfg, *pstSingleDevCfg;

//typedef struct STDEVICECONFIG
//{
//	stSingleDevCfg stDev[LENGTH_16];
//	stSerialPortInfo stSerialPort;
//	int iSWCom;
//	int iOCMCom;
//	int iATTCom;
//	int iVMUXCom;
//	int iDevConnState;
//	int aiDevDetail[LENGTH_64];
//	int aiChannelCheck[LENGTH_96];
// }stDeviceCfg, *pstDeviceCfg;

typedef struct STTRANSINFO
{
	CCfgFileInterface* pCfg;
	CNetCommandInterface* pNet;
	CDeviceOptInterface* pDev;
	CGraphInterface* pGraph;
	int iModulePart;
	TCHAR pszSpec[LENGTH_32];
	TCHAR pszPN[LENGTH_32];
	TCHAR pszSN[LENGTH_32];
	TCHAR pszCfgPath[MAX_PATH];//config path
	TCHAR pszDataPath[MAX_PATH];//config path
	TCHAR pszCaliPath[MAX_PATH];//data path to SN
}stTransInfo, *pstTransInfo;

#endif