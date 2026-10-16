// SwitchDriver.cpp: implementation of the CSwitchDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testprocess.h"
#include "SwitchDriver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSwitchDriver::CSwitchDriver()
{

}

CSwitchDriver::~CSwitchDriver()
{

}

/////////////////////////////////////////////////////////////
//函数说明：
//    打开串口通信开关，这里打开所有需要的开关
//Add by wanxin
//Time:20160727
/////////////////////////////////////////////////////////////
BOOL CSwitchDriver::OpenSwitch()
{
	return TRUE;
}
BOOL CSwitchDriver::OPSwitch121()
{
	char comBuffer[256],readBuffer[256];
	//	DWORD dwGetLength;
	ZeroMemory(&comBuffer,256);
	////2A 30 30 30 39 30 31 30 31 30 30 30 30 4B 0D
	comBuffer[0]=0x2A;
	comBuffer[1]=0x30;
	comBuffer[2]=0x30;
	comBuffer[3]=0x30;
	comBuffer[4]=0x39;
	comBuffer[5]=0x30;
	comBuffer[6]=0x31;
	comBuffer[7]=0x30;
	comBuffer[8]=0x31;
	comBuffer[9]=0x30;
	comBuffer[10]=0x30;
	comBuffer[11]=0x30;
	comBuffer[12]=0x30;
	comBuffer[13]=0x4B;
	comBuffer[14]=0x0D;
	
	if (!m_Comm1X2Switch.WriteBuffer((char*)comBuffer,256))
	{
		AfxMessageBox("串口数据写入失败");
		return FALSE;
	}
	Sleep(20);
	if (!m_Comm1X2Switch.ReadBuffer((char*)readBuffer,256))
	{
		AfxMessageBox("串口数据读取失败");
		return FALSE;		
	}
	if (readBuffer[13]!=0x4C && readBuffer[14]!=0x5E)
	{
		if (!m_Comm1X2Switch.ReadBuffer((char*)readBuffer,256))
		{
			AfxMessageBox("1X2开关切换错误");
			return FALSE;
		}	
	}
	
	return TRUE;	
}

BOOL CSwitchDriver::OPSwitch122()
{
   	char comBuffer[256],readBuffer[256];
	//	DWORD dwGetLength;
	ZeroMemory(&comBuffer,256);
	ZeroMemory(&readBuffer,256);
	////2A 30 30 30 39 30 31 30 32 30 30 30 30 4D 0D
	comBuffer[0]=0x2A;
	comBuffer[1]=0x30;
	comBuffer[2]=0x30;
	comBuffer[3]=0x30;
	comBuffer[4]=0x39;
	comBuffer[5]=0x30;
	comBuffer[6]=0x32;
	comBuffer[7]=0x30;
	comBuffer[8]=0x32;
	comBuffer[9]=0x30;
	comBuffer[10]=0x30;
	comBuffer[11]=0x30;
	comBuffer[12]=0x30;
	comBuffer[13]=0x4D;
	comBuffer[14]=0x0D;
	
	if (!m_Comm1X2Switch.WriteBuffer((char*)comBuffer,256))
	{
		AfxMessageBox("串口数据写入失败");
		return FALSE;
	}
	Sleep(20);
	if (!m_Comm1X2Switch.ReadBuffer((char*)readBuffer,256))
	{
		AfxMessageBox("串口数据读取失败");
		return FALSE;
	}
	if (readBuffer[13]!=0x4E && readBuffer[14]!=0x5E)
	{
		Sleep(20);
		if (!m_Comm1X2Switch.ReadBuffer((char*)readBuffer,256))
		{
			AfxMessageBox("1X2开关切换错误");
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CSwitchDriver::DriveSingleSwitchForCriterion(int nmb,BYTE bySwitchCtrIndex, BYTE bySwitchIndex, BYTE bySwitchPortIndex)
{
	if(bySwitchCtrIndex <1 || bySwitchCtrIndex > 8)
	{
		//LogInfo("驱动标准开关，输入驱动开关编号或控制板编号出错",COLOR_RED);
		return FALSE;
	}
	CString  strMsg;
	BYTE	byRxBuffer[100];
	BYTE	byTxBuffer[100];
	COpComm  *OpCom=NULL;
	ZeroMemory(byRxBuffer,100*sizeof(BYTE));
	ZeroMemory(byTxBuffer,100*sizeof(BYTE));
	byTxBuffer[0]='*';
	if(nmb==1)
        OpCom = &m_CommSwitch1;
	else if(nmb==2)
		OpCom = &m_CommSwitch2;
	else 
		return FALSE;
	//控制板编号
	switch(bySwitchCtrIndex)
	{
	case 1:
		byTxBuffer[1]='0';
		byTxBuffer[2]='1';
		break;
	case 2:
		byTxBuffer[1]='0';
		byTxBuffer[2]='2';
		break;
	case 3:
		byTxBuffer[1]='0';
		byTxBuffer[2]='3';
		break;
	case 4:
		byTxBuffer[1]='0';
		byTxBuffer[2]='4';
		break;
	case 5:
		byTxBuffer[1]='0';
		byTxBuffer[2]='5';
		break;
	case 6:
		byTxBuffer[1]='0';
		byTxBuffer[2]='6';
		break;
	default:
	//	LogInfo("驱动开关-传入控制板编号出错!",COLOR_RED);
		return FALSE;
	}
	if(bySwitchPortIndex > 4 && bySwitchIndex != 2)
	{
		return FALSE;
	}
	//通道编号
	
	switch(bySwitchPortIndex) 
	{
	case 1:
		byTxBuffer[3]='0';
		byTxBuffer[4]='1';
		break;
	case 2:
		byTxBuffer[3]='0';
		byTxBuffer[4]='2';
		break;
	case 3:
		byTxBuffer[3]='0';
		byTxBuffer[4]='3';
		break;
	case 4:
		byTxBuffer[3]='0';
		byTxBuffer[4]='4';
		break;
	case 5:
		byTxBuffer[3]='0';
		byTxBuffer[4]='5';
		break;
	case 6:
		byTxBuffer[3]='0';
		byTxBuffer[4]='6';
		break;
	case 7:
		byTxBuffer[3]='0';
		byTxBuffer[4]='7';
		break;
	case 8:
		byTxBuffer[3]='0';
		byTxBuffer[4]='8';
		break;
	default:
	//	LogInfo("驱动开关-传入通道编号出错!",COLOR_RED);
		return FALSE;
	}
	//开关编号
	switch(bySwitchIndex)
	{
	case 1:
		byTxBuffer[5]='0';
		byTxBuffer[6]='1';
		break;
	case 2:
		byTxBuffer[5]='0';
		byTxBuffer[6]='2';
		break;
	case 3:
		byTxBuffer[5]='0';
		byTxBuffer[6]='3';
		break;
	default:
//		LogInfo("驱动开关-传入开关编号出错!",COLOR_RED);
		return FALSE;
	}
	//校验码
	BYTE byCheck = byTxBuffer[1]+byTxBuffer[2]+byTxBuffer[3]+byTxBuffer[4]+byTxBuffer[5]+byTxBuffer[6];
	strMsg.Format("%2x",byCheck);
	LPCTSTR lpTemp = strMsg.GetBuffer(strMsg.GetLength());
	ASSERT(lpTemp != NULL);
	byTxBuffer[7] = lpTemp[0];
	byTxBuffer[8] = lpTemp[1];
	byTxBuffer[9] = 0x0d;
	
	if(!OpCom->WriteBuffer((char*)byTxBuffer,10) )
	{
	//	LogInfo("驱动开关-写串口失败!",COLOR_RED);
		return FALSE;
	}
	// 读取控制板的开关状态 鉴定驱动是否成功
	BYTE byRunPort = 0;
	Sleep(100);
	if(!ReadSingleSwitchStateForCriterion(nmb,bySwitchCtrIndex,bySwitchIndex,&byRunPort))
	{
		return FALSE;
	}
	if(byRunPort != bySwitchPortIndex)
	{
//		LogInfo("驱动开关-驱动开关状态与实际开关状态不符!",COLOR_RED);
		return FALSE;
	}
	
	return TRUE;
}
//用于读取单块控制板上 开关的状态当前驱动的，参数前2个位输入参数，第三个位输出参数 
BOOL CSwitchDriver::ReadSingleSwitchStateForCriterion(int nmb,BYTE bySwtchCtrIndex, BYTE bySwitchIndex, BYTE *pbyPortIndex)
{
	CString  strMsg;
	BYTE	byRxBuffer[100];
	BYTE	byTxBuffer[100];
	ZeroMemory(byRxBuffer,100*sizeof(BYTE));
	ZeroMemory(byTxBuffer,100*sizeof(BYTE));
	byTxBuffer[0]='*';
	COpComm  *OpCom=NULL;
	if(nmb==1)
        OpCom = &m_CommSwitch1;
	else if(nmb==2)
		OpCom = &m_CommSwitch2;
	else 
		return FALSE;
	//控制板编号
	switch(bySwtchCtrIndex)
	{
	case 1:
		byTxBuffer[1]='0';
		byTxBuffer[2]='1';
		break;
	case 2:
		byTxBuffer[1]='0';
		byTxBuffer[2]='2';
		break;
	case 3:
		byTxBuffer[1]='0';
		byTxBuffer[2]='3';
		break;
	case 4:
		byTxBuffer[1]='0';
		byTxBuffer[2]='4';
		break;
	case 5:
		byTxBuffer[1]='0';
		byTxBuffer[2]='5';
		break;
	case 6:
		byTxBuffer[1]='0';
		byTxBuffer[2]='6';
		break;
	default:
//		LogInfo("读取开关状态-传入控制板编号出错!",COLOR_RED);
		return FALSE;
	}
	byTxBuffer[3]='8';
	byTxBuffer[4]='0';
	//开关编号
	switch(bySwitchIndex)
	{
	case 1:
		byTxBuffer[5]='0';
		byTxBuffer[6]='1';
		break;
	case 2:
		byTxBuffer[5]='0';
		byTxBuffer[6]='2';
		break;
	case 3:
		byTxBuffer[5]='0';
		byTxBuffer[6]='3';
		break;
	default:
//		LogInfo("读取开关状态-传入开关编号出错!",COLOR_RED);
		return FALSE;
	}
	//校验码
	BYTE byCheck = byTxBuffer[1]+byTxBuffer[2]+byTxBuffer[3]+byTxBuffer[4]+byTxBuffer[5]+byTxBuffer[6];
	strMsg.Format("%2x",byCheck);
	LPCTSTR lpTemp = strMsg.GetBuffer(strMsg.GetLength());
	ASSERT(lpTemp != NULL);
	byTxBuffer[7] = lpTemp[0];
	byTxBuffer[8] = lpTemp[1];
	byTxBuffer[9] = 0x0d;
	
	if(!OpCom->WriteBuffer((char*)byTxBuffer,10) )
	{
//		LogInfo("读取开关状态-传入开关编号出错!",COLOR_RED);
		return FALSE;
	}
	Sleep(500);
	if(!OpCom->ReadBuffer((char*)byRxBuffer,10))
	{
	//	LogInfo("读取开关状态-读串口失败!",COLOR_RED);
		return FALSE;
	}
	BYTE byTemp = byRxBuffer[4];
	//	strMsg.Format("%s",pchar);
	*pbyPortIndex = atoi((char*)&byTemp);
	return TRUE;
}

BOOL CSwitchDriver::Control1X64Switch(int nmb,int channel)
{
	CString strSendBuf;
	CString strRevBuf,strMsg;
	COpComm  *OpCom=NULL;
	char    chReadBuff[MAX_PATH];
	//DWORD   dwLength;
	ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
	int nerrorCount=0;
	if(nmb==1)
        OpCom = &m_CommSwitch1;
	else if(nmb==2)
		OpCom = &m_CommSwitch2;
	else 
		return FALSE;	
	//strSendBuf.Format("set:0:sw:0::channel=%d;",channel);
	strSendBuf.Format("SETSWS %d %d\r\n",nmb,channel);
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(500);
		if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
		{
			strMsg.Format("第%d开关，第%d通道，切换开关失败！",nmb,channel);
			//LogInfo(strMsg, TRUE, COLOR_RED);
			return FALSE;
		}
	}
	strSendBuf.ReleaseBuffer();
	
	if(nerrorCount>10)
	{
	//	LogInfo("1x8开关切换通道失败",COLOR_RED);
		return FALSE;
	}
	Sleep(100);
	if (!OpCom->ReadBuffer(chReadBuff,MAX_PATH))
	{
	//	LogInfo("获取开关信息失败！！请检查该光开关及控制串口！",COLOR_RED);
		return FALSE;
	}
	strRevBuf = chReadBuff;
	if(strRevBuf.Find("OK")==-1)
	{
		nerrorCount++;
	}
	else
	{
		return TRUE;
	}
	
	return TRUE;
}

//1x8开关切换
BOOL CSwitchDriver::Drive1X8Switch( BYTE bySwitchPortIndex,BYTE bySwitchCtrIndex)
{
	if(bySwitchCtrIndex <1 || bySwitchCtrIndex > 2)
	{
		return FALSE;
	}
	CString  strMsg,strTemp;
	char*   cSendBuffer = NULL;
	BYTE    cReadBuffer[250];
	ZeroMemory(cReadBuffer,250);
	int     nSendCount = 0;
	BYTE    byFlagCount = 0;
	
	strMsg.Format("SW%dPORT%d\n\r",bySwitchCtrIndex,bySwitchPortIndex);
	nSendCount = strMsg.GetLength();
	cSendBuffer = strMsg.GetBuffer(nSendCount);
	ASSERT(cSendBuffer != NULL);
	
	if(!m_Comm1X8Switch.WriteBuffer((char*)cSendBuffer,nSendCount))
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	Sleep(50);
	while (byFlagCount < 3)
	{
		
		if(!m_Comm1X8Switch.ReadBuffer((char*)cReadBuffer,100))
		{
			
			byFlagCount++;
			Sleep(10);
			continue;
		}
		else
		{
			break;
		}
	}
	strTemp=cReadBuffer;
	if(strTemp.Find("Success")==-1)
	{	
		return FALSE;
	}
	return TRUE;
}