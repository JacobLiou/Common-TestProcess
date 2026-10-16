// VCMUXCommand.cpp: implementation of the CVCMUXCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "1wiree2.h"
#include "VCMUXCommand.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVCMUXCommand::CVCMUXCommand()
{

}

CVCMUXCommand::~CVCMUXCommand()
{

}

BOOL CVCMUXCommand::GetVOASettingDac(int nBlock,int nVOAIndex, PWORD pnDACValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	BYTE pbySetValue[MAX_COUNT];
	WORD wSetLength=0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char chRead[256];
	ZeroMemory(chRead,sizeof(chRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(nBlock==0)
	{
		wAddress = GET_VOA_SETTING_DAC + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = GET_VOA_SETTING_DAC + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = GET_VOA_SETTING_DAC + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength,byReadLength,bSetCommand,chRead))
	{
		return FALSE;
	}
	WORD sValue = (WORD)strtol(chRead,NULL,16);;
	*pnDACValue = sValue;
	return TRUE;
}

BOOL CVCMUXCommand::GetVOAActualAtten(int nBlock,int nVOAIndex, double *pdblAttenValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	BYTE pbySetValue[MAX_COUNT];
	WORD wSetLength=0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char chRead[256];
	ZeroMemory(chRead,sizeof(chRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(nBlock==0)
	{
		wAddress = VOA_ACTUAL_ATTEN + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = VOA_ACTUAL_ATTEN + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = VOA_ACTUAL_ATTEN + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength,byReadLength,bSetCommand,chRead))
	{
		return FALSE;
	}
	SHORT sValue = (SHORT)strtol(chRead,NULL,16);
	//*pADCValue = sValue;
	//*pdblAttenValue = atof(chRead);
	*pdblAttenValue = (double)sValue/100.0;
	return TRUE;

}

BOOL CVCMUXCommand::SetVOAActualAtten(int nBlock,int nVOAIndex, double dblAttenValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	SHORT sValue = (SHORT)(dblAttenValue*100);
	BYTE pbySetValue[MAX_COUNT];
	WORD wSetLength=0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = TRUE;
	if(nBlock==0)
	{
		wAddress = VOA_ACTUAL_ATTEN + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = VOA_ACTUAL_ATTEN + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = VOA_ACTUAL_ATTEN + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	pbySetValue[0] = HIBYTE(sValue);
	pbySetValue[1] = LOBYTE(sValue);
	wSetLength = 2;
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength))
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CVCMUXCommand::SetVOAActualDAC(int nBlock,int nVOAIndex, int nDACValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	BYTE pbySetValue[MAX_COUNT];
	WORD wSetLength=0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = TRUE;
	if(nBlock==0)
	{
		wAddress = VOA_ACTUAL_DAC + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = VOA_ACTUAL_DAC + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = VOA_ACTUAL_DAC + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	pbySetValue[0] = HIBYTE(nDACValue);
	pbySetValue[1] = LOBYTE(nDACValue);
	wSetLength = 2;
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength))
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CVCMUXCommand::GetVOAActualDAC(int nBlock,int nVOAIndex, PWORD pDACValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	BYTE pbySetValue[MAX_COUNT];
	WORD wSetLength=0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char chRead[256];
	ZeroMemory(chRead,sizeof(chRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(nBlock==0)
	{
		wAddress = VOA_ACTUAL_DAC + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = VOA_ACTUAL_DAC + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = VOA_ACTUAL_DAC + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength,byReadLength,bSetCommand,chRead))
	{
		return FALSE;
	}
	pDACValue[0] = (WORD)strtol(chRead,NULL,16);
	return TRUE;
}

//ADD16: 0~31
//Drop4:32~39
//Add4: 40~47
BOOL CVCMUXCommand::GetPDADC(int nBlock,int nPDIndex,PWORD pADCValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char pchRead[256];

	if(nBlock==0)
	{
		wAddress = GET_PD_ADC + (ADD16_START_INDEX+nPDIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = GET_PD_ADC + (DROP4_START_INDEX+nPDIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = GET_PD_ADC + (ADD4_START_INDEX+nPDIndex)*2;
	}
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	ZeroMemory(pchRead,sizeof(pchRead));
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wAddress,byReadLength,bSetCommand,pchRead))
	{
		return FALSE;
	}
	pADCValue[0] = (WORD)strtol(pchRead,NULL,16);
	return TRUE;
}

//nPDMonitorPlace 0:Input Power
//nPDMonitorPlace 1:Actual Power
//nPDMonitorPlace 2:Out Power
BOOL CVCMUXCommand::GetPDPower(int nBlock,int nPDIndex, int nPDMonitorPlace, double *pdblPowerValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0x00;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char pchRead[256];
	if(nBlock==0)
	{
		wAddress = GET_PD_POWER  + (nPDIndex+nPDMonitorPlace*16)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = GET_PD_POWER + 0X62 + (nPDIndex+nPDMonitorPlace*4)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = GET_PD_POWER + 0X7C + (nPDIndex+nPDMonitorPlace*4)*2;
	}
	
	ZeroMemory(pchRead,sizeof(pchRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength,byReadLength,bSetCommand,pchRead))
	{
		return FALSE;
	}
	SHORT sValue = (SHORT)strtol(pchRead,NULL,16);
	//*pADCValue = sValue;
	*pdblPowerValue = (double)sValue/100.0; 
	return TRUE;
}

BOOL CVCMUXCommand::SetVOAAtten(int nBlock,int nVOAIndex, double dblAttenValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	SHORT sValue = (SHORT)(dblAttenValue*100);
	BYTE pbySetValue[MAX_COUNT];
    WORD wSetLength = 0;
    BOOL bSetCommand = TRUE;
	if(nBlock==0) //ADD16
	{
		wAddress = VOA_ATTEN_CUSTOMER + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) //drop4
	{
		wAddress = VOA_ATTEN_CUSTOMER + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) //ADD4
	{
		wAddress = VOA_ATTEN_CUSTOMER + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	wSetLength = 0;
    pbySetValue[wSetLength++] = HIBYTE(sValue);
	pbySetValue[wSetLength++] = LOBYTE(sValue);

	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength))
	{
		return FALSE;
	}
	return TRUE;

}
/////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    根据IIC地址发送相应内容到测试板或者模块
//Add wanxin
//Time:20160229
/////////////////////////////////////////////////////////////////////////////////
BOOL CVCMUXCommand::TxDataToI2C(BYTE byIndex, WORD wAddress, BYTE *pbySetValue, WORD wSetLength, BYTE bytReadLength,BOOL bSetCommand,char *pchRead)
{
	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	DWORD dwLength = 0;
	BYTE bytFLAG=0x00;
	BYTE bytSUM=0x0000;
	BYTE bytSlaveAddr;
	CString strCommand;
	CString strTemp="";
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	BYTE ptCommand[128];
	CString strMsg,strData,strTmp;
	char   chCommand[MAX_COUNT];
	int n=0;
	BYTE byHiAddr = HIBYTE(wAddress);
	BYTE byLoAddr = LOBYTE(wAddress);

	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	ZeroMemory(ptCommand,sizeof(BYTE)*128);
	ZeroMemory(chSetCommand,sizeof(char)*MAX_COUNT);
	ZeroMemory(chCommand,sizeof(char)*MAX_COUNT);

	bytSlaveAddr = SLAVE_WRITE;
	if(bSetCommand)
	{
		strCommand.Format("SetI2C %02x %02x %02x%02x",byIndex,bytSlaveAddr,byHiAddr,byLoAddr);
		for (n=0;n<wSetLength;n++) 
		{
			strTemp.Format("%02x",pbySetValue[n]);
			strCommand = strCommand +  strTemp;
		}
		strCommand = strCommand + "\r";
	}
	else
	{
		strCommand.Format("SetI2C %02x %02x %02x%02x\r",byIndex,bytSlaveAddr,
								byHiAddr,byLoAddr);
	}
	
	memcpy(chSetCommand,strCommand,strCommand.GetLength());
	
	if (!WriteBuffer(chSetCommand,strCommand.GetLength()))
	{
		AfxMessageBox("发送指令错误！");
		return FALSE;
	}
	Sleep(dwWaitTime);	
	if (!ReadBuffer(chReadCommand,MAX_COUNT,&dwFeedBackLength))
	{
		AfxMessageBox("读取I2C数据错误！");
		return FALSE;
	}
	strMsg.Format("%s",&chReadCommand);
//	LogInfo(strMsg);
	
    //发送读取I2C命令
	if(!bSetCommand)
	{
		ZeroMemory(chSetCommand,sizeof(char)*MAX_COUNT);
		ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
		bytSlaveAddr = SLAVE_READ;//Read
		strCommand.Format("GetI2C %02x %02x %02x\r",byIndex,bytSlaveAddr,bytReadLength);
		memcpy(chSetCommand,strCommand,strCommand.GetLength());
//		LogInfo(chSetCommand);
		if (!WriteBuffer(chSetCommand,strCommand.GetLength()))
		{
			AfxMessageBox("发送指令错误！");
			return FALSE;
		} 
		Sleep(50);		
		if (!ReadBuffer(pchRead,MAX_COUNT,&dwFeedBackLength))
		{
			AfxMessageBox("读取I2C数据错误！");
			return FALSE;
		}
	//	LogInfo(pchRead);
		//*pchRead = chReadCommand;
	}
	return	TRUE;

}

BOOL CVCMUXCommand::SetMFGMode(BOOL bMFGStatus)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = MFG_PASSWORD;
	WORD wSetLength=0;
	BYTE pbySetValue[MAX_COUNT];
    BOOL bSetCommand = TRUE;
	int n=0;
	char chMFGCode[6] = {'M','F','G','C','M','D'};
	
	if (bMFGStatus) 
	{
		for (n=0;n<6;n++)
		{
			pbySetValue[n] = (BYTE)chMFGCode[n];
		}
		
	}
	else
	{
		for (n=0;n<6;n++)
		{
			pbySetValue[n] = '*';
		}
	}
	wSetLength = 6;

	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength))
	{
		return FALSE;
	}
	return TRUE;

}

char * CVCMUXCommand::GetMFGMode()
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = MFG_PASSWORD;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0x06;
    BOOL bSetCommand = FALSE;
	char pchReadBuf[256];

	ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength,byReadLength,bSetCommand,pchReadBuf))
	{
		return NULL;
	}
	BYTE byValue[6];
	char pchRead[10];
	ZeroMemory(pchRead,sizeof(pchRead));
	//转化为16进制数
	sscanf(pchReadBuf,"%02X%02X%02X%02X%02X%02X>",&byValue[0],&byValue[1],&byValue[2],&byValue[3],&byValue[4],&byValue[5]);
	for (int n=0;n<6;n++)
	{
		pchRead[n] = (char)byValue[n];
	}
	//16进制转换为ASCII码：
	return (char*)pchRead;
}

BOOL CVCMUXCommand::ReadFlashData(DWORD dwStartAddress,char *pchReadBuf)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = FLASH_ADDR_FOR_READ;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0;
    BOOL bSetCommand = TRUE;
//	char pchReadBuf[256];
	ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));

	//WRITE ADDRESS
	pbySetValue[3] = (BYTE)dwStartAddress;
	pbySetValue[2] = (BYTE)(dwStartAddress>>8);
	pbySetValue[1] = (BYTE)(dwStartAddress>>16);
	pbySetValue[0] = (BYTE)(dwStartAddress>>24);
	wLength = 4;

	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength))
	{
		return FALSE;
	}
	
	//Read Data:32bytes
	ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	byReadLength = 32; 
	bSetCommand = FALSE;
	wAddress = FLASH_DATA_BY_READ;
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength,byReadLength,bSetCommand,pchReadBuf))
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CVCMUXCommand::GetMFGModeForDebug(char *pchReadBuf)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = MFG_PASSWORD;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0x06;
    BOOL bSetCommand = FALSE;
	char pchRead[256];

	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(pchRead,sizeof(pchRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength,byReadLength,bSetCommand,pchRead))
	{
		return FALSE;
	}
	BYTE byValue[6];
//	char pchRead[10];
	//ZeroMemory(pchRead,sizeof(pchRead));
	//转化为16进制数
	sscanf(pchRead,"%02X%02X%02X%02X%02X%02X>",&byValue[0],&byValue[1],&byValue[2],&byValue[3],&byValue[4],&byValue[5]);
	for (int n=0;n<6;n++)
	{
		pchRead[n] = (char)byValue[n];
		*pchReadBuf++ = pchRead[n];
	}
//	*pchReadBuf = pchRead[0];
	//16进制转换为ASCII码：
	return TRUE;

}

BOOL CVCMUXCommand::GetVOASettingDAC(int nBlock, int nVOAIndex, PWORD pDACValue)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = 0;
	BYTE pbySetValue[MAX_COUNT];
	WORD wSetLength=0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char chRead[256];
	ZeroMemory(chRead,sizeof(chRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(nBlock==0)
	{
		wAddress = GET_VOA_SETTING_DAC + (ADD16_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==1) 
	{
		wAddress = GET_VOA_SETTING_DAC + (DROP4_START_INDEX+nVOAIndex)*2;
	}
	else if (nBlock==2) 
	{
		wAddress = GET_VOA_SETTING_DAC + (ADD4_START_INDEX+nVOAIndex)*2;
	}
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wSetLength,byReadLength,bSetCommand,chRead))
	{
		return FALSE;
	}
	pDACValue[0] = (WORD)strtol(chRead,NULL,16);
	return TRUE;

}

BOOL CVCMUXCommand::ReadOPLINKFWVer(char *pchReadBuf)
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = OPLINK_FW_VER;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0x08;
    BOOL bSetCommand = FALSE;
	char pchRead[256];
	CString strMsg;

	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(pchRead,sizeof(pchRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength,byReadLength,bSetCommand,pchRead))
	{
		return FALSE;
	}

	//strMsg.Format("%s",&pchRead);
	//AfxMessageBox(strMsg);
	BYTE byValue[8];
//	char pchRead[10];
	//ZeroMemory(pchRead,sizeof(pchRead));
	//转化为16进制数
	sscanf(pchRead,"%02X%02X%02X%02X%02X%02X%02X%02X>",&byValue[0],&byValue[1],&byValue[2],&byValue[3],&byValue[4],&byValue[5],&byValue[6],&byValue[7]);
	for (int n=0;n<8;n++)
	{
		pchRead[n] = (char)byValue[n];
		*pchReadBuf++ = pchRead[n];
	}
//	*pchReadBuf = pchRead[0];
	//16进制转换为ASCII码：
	return TRUE;

}

double CVCMUXCommand::GetTemperature()
{
	BYTE byIndex = COMMAND_INDEX;
    WORD wAddress = READ_TEMP;
	BYTE pbySetValue[MAX_COUNT];
	WORD wLength = 0;
    BYTE byReadLength = 0x02;
    BOOL bSetCommand = FALSE;
	char pchRead[256];
	CString strMsg;

	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(pchRead,sizeof(pchRead));
	ZeroMemory(pbySetValue,sizeof(pbySetValue));
	if(!TxDataToI2C(byIndex,wAddress,pbySetValue,wLength,byReadLength,bSetCommand,pchRead))
	{
		return FALSE;
	}

	BYTE byValue[2];
	//转化为16进制数
	sscanf(pchRead,"%02X%02X>",&byValue[0],&byValue[1]);
	WORD wTemp = (byValue[0]<<8) + (byValue[1]);
	double dblTemp = wTemp/10.0;
	
	return dblTemp;

}
