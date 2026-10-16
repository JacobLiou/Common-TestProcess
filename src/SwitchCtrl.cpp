// TesterControl.cpp: implementation of the CTesterControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SwitchCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSwitchBoxCtrl::CSwitchBoxCtrl()
{
	m_nmodule = 1;
}

CSwitchBoxCtrl::~CSwitchBoxCtrl()
{
	
}

BOOL CSwitchBoxCtrl::SelPathByType(CString strModuleType, int nPath)
{
	BOOL	bRunResult = TRUE;
	if(nUseSwitchType > 0)
	{
		bRunResult = SelPathByTypeZH(strModuleType,nPath);
	}
	else
	{
		bRunResult = SelPathByTypeHQ(strModuleType,nPath);
	}
	return bRunResult;	
}

BOOL CSwitchBoxCtrl::SetTester(int nSw1Port, int nSw2Port)
{
	BOOL	bRunResult= TRUE;
	switch (nUseSwitchType)
	{
	case 0: //总部开关盒
		{
            bRunResult = SetTesterHQ(nSw1Port,nSw2Port);
		}
		break;
	case 1: //ZH 1代开关盒
		{
			
		}
		break;
	case 2: //ZH 2代开光盒
		{
            bRunResult = SetTesterZH(nSw1Port,nSw2Port);
		}
		break;
	case 3: //ZH 3代开关盒
		{
			bRunResult = SetTesterZH3(m_nmodule,nSw1Port,nSw2Port);
		}
		break;		
	}
	
	return bRunResult;
}

BOOL CSwitchBoxCtrl::SetTesterHQ(int nSw1Port, int nSw2Port)
{
	CString strCmd;
	strCmd.Format("SW1PORT%d\n\r", nSw1Port);
	
	if (!WriteBuffer(strCmd.GetBuffer(1024), strCmd.GetLength()))
	{
		return FALSE;
	}
	
	char chRet[1024] = "";
	ZeroMemory(chRet, 1024);
	Sleep(10);
	if (!ReadBuffer((char*)chRet, 1024))
	{
		return FALSE;
	}
	
	if (0 != memcmp(chRet, "Success", 7))
	{
		return FALSE;
	}
	
	strCmd.Format("SW2PORT%d\n\r", nSw2Port);
	Sleep(10);
	if (!WriteBuffer(strCmd.GetBuffer(1024), strCmd.GetLength()))
	{
		return FALSE;
	}
	
	ZeroMemory(chRet, 1024);
	Sleep(10);
	if (!ReadBuffer((char*)chRet, 1024))
	{
		return FALSE;
	}
	
	if (0 != memcmp(chRet, "Success", 7))
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CSwitchBoxCtrl::SelPathByTypeHQ(CString strModuleType, int nPath)
{
	BOOL bFunRes = FALSE;
	int nPos = strModuleType.Find("1X");
	if (nPos > 0) 
	{
		CString strTemp = strModuleType;
		strTemp.Delete(0, nPos+2);
		nPos = strTemp.Find(' ');
		int nPort = atoi(strTemp.Left(nPos));
		
		bFunRes = SetTesterHQ(1, nPath);
	}
	
	nPos = strModuleType.Find("X1");
	if (nPos > 0)
	{
		CString strTemp = strModuleType.Left(nPos);
		//nPos = strModuleType.ReverseFind(' ');
		//strModuleType.Delete(0, nPos+1);
		int nPort = atoi(strTemp);
		
		bFunRes = SetTesterHQ(nPath, 1);
	}
	
	return bFunRes;
}

BOOL CSwitchBoxCtrl::SetTesterZH(int nSw1Port, int nSw2Port)
{
	CString  strMsg;
	char*   cSendBuffer = NULL;
	BYTE    cReadBuffer[250];
	ZeroMemory(cReadBuffer,250);
	int     nSendCount = 0;
	BYTE    byFlagCount = 0;
	strMsg.Format("SET:0:SW:%d::Channel=%d;",1,nSw1Port);
	nSendCount = strMsg.GetLength();
	cSendBuffer = strMsg.GetBuffer(nSendCount);
	ASSERT(cSendBuffer != NULL);
	
	if(!WriteBuffer((char*)cSendBuffer,nSendCount))
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	Sleep(50);
	while (byFlagCount < 2)
	{
		
		if(!ReadBuffer((char*)cReadBuffer,100))
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
	if(byFlagCount >= 2)
	{
		return FALSE;
	}
	else if(cReadBuffer[0] == '?' && cReadBuffer[1] == 'E')
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	////// 第二个
	
	strMsg.Format("SET:0:SW:%d::Channel=%d;",2,nSw2Port);
	nSendCount = strMsg.GetLength();
	cSendBuffer = strMsg.GetBuffer(nSendCount);
	ASSERT(cSendBuffer != NULL);
	
	if(!WriteBuffer((char*)cSendBuffer,nSendCount))
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	Sleep(50);
	while (byFlagCount < 2)
	{
		
		if(!ReadBuffer((char*)cReadBuffer,100))
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
	if(byFlagCount >= 2)
	{
		return FALSE;
	}
	else if(cReadBuffer[0] == '?' && cReadBuffer[1] == 'E')
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CSwitchBoxCtrl::SelPathByTypeZH(CString strModuleType, int nPath)
{
	BOOL bFunRes = FALSE;
	int nPos = strModuleType.Find("1X");
	if (nPos > 0) 
	{
		CString strTemp = strModuleType;
		strTemp.Delete(0, nPos+2);
		nPos = strTemp.Find(' ');
		int nPort = atoi(strTemp.Left(nPos));
		
		bFunRes = SetTesterZH(1, nPath);
	}
	
	nPos = strModuleType.Find("X1");
	if (nPos > 0)
	{
		CString strTemp = strModuleType.Left(nPos);
		//nPos = strModuleType.ReverseFind(' ');
		//strModuleType.Delete(0, nPos+1);
		int nPort = atoi(strTemp);
		
		bFunRes = SetTesterZH(nPath, 1);
	}
	
	return bFunRes;
}
/////////////////////////////////////////////////////////////
//函数说明：
//    切换ZH第三代开关盒Port,其中nmodule为模块类型，0为NX1，1为1XN,nSw1Port为模块在1X4上的索引号
//    nSw2Port为Port索引号
//add by wanxinw
////////////////////////////////////////////////////////////
BOOL CSwitchBoxCtrl::SetTesterZH3(int nmodule,int nSw1Port,int nSw2Port)
{
	CString  strMsg;
	char*   cSendBuffer = NULL;
	BYTE    cReadBuffer[250];
	ZeroMemory(cReadBuffer,250);
	int     nSendCount = 0;
	BYTE    byFlagCount = 0;
	//判断模块类型，是1XN,NX1；	

	//切换通光Port
	if (nmodule>0)
	{
		Sleep(50);
		strMsg.Format("SET:0:SW:%d::Channel=%d;",0,nSw1Port*10+nSw2Port);
		nSendCount = strMsg.GetLength();
		cSendBuffer = strMsg.GetBuffer(nSendCount);
		ASSERT(cSendBuffer != NULL);	
		if(!WriteBuffer((char*)cSendBuffer,nSendCount))
		{
			return FALSE;
		}
		strMsg.ReleaseBuffer();
		Sleep(50);
		while (byFlagCount < 2)
		{
			
			if(!ReadBuffer((char*)cReadBuffer,100))
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
		if(byFlagCount >= 2)
		{
			return FALSE;
		}
		else if(cReadBuffer[0] == '?' && cReadBuffer[1] == 'E')
		{
			return FALSE;
		}
		strMsg.ReleaseBuffer();
		Sleep(50);
		strMsg.Format("SET:0:SW:%d::Channel=%d;",2,nSw1Port-1);
	}
	else
	{
			strMsg.Format("SET:0:SW:%d::Channel=%d;",0,nSw1Port*10+nSw2Port);
	}
	
	nSendCount = strMsg.GetLength();
	cSendBuffer = strMsg.GetBuffer(nSendCount);
	ASSERT(cSendBuffer != NULL);
	
	if(!WriteBuffer((char*)cSendBuffer,nSendCount))
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	Sleep(50);
	while (byFlagCount < 2)
	{
		
		if(!ReadBuffer((char*)cReadBuffer,100))
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
	if(byFlagCount >= 2)
	{
		return FALSE;
	}
	else if(cReadBuffer[0] == '?' && cReadBuffer[1] == 'E')
	{
		return FALSE;
	}
	return TRUE;
	
}
////////////////////////////////////////////////////////////
//函数说明：
//   切换ZH第三代开关盒进光H(1)或者L(0),默认情况下之切换一次即可,
//add by wanxinw
/////////////////////////////////////////////////////////////
BOOL CSwitchBoxCtrl::SetZH3HL(int nSW1Port)
{
	CString  strMsg;
	char*   cSendBuffer = NULL;
	BYTE    cReadBuffer[250];
	ZeroMemory(cReadBuffer,250);
	int     nSendCount = 0;
	BYTE    byFlagCount = 0;
	strMsg.Format("SET:0:SW:%d::Channel=%d;",1,nSW1Port);
	nSendCount = strMsg.GetLength();
	cSendBuffer = strMsg.GetBuffer(nSendCount);
	ASSERT(cSendBuffer != NULL);
	
	if(!WriteBuffer((char*)cSendBuffer,nSendCount))
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	Sleep(50);
	while (byFlagCount < 2)
	{
		
		if(!ReadBuffer((char*)cReadBuffer,100))
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
	if(byFlagCount >= 2)
	{
		return FALSE;
	}
	else if(cReadBuffer[0] == '?' && cReadBuffer[1] == 'E')
	{
		return FALSE;
	}
	strMsg.ReleaseBuffer();
	return TRUE;
}
