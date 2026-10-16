  // ProcessMsg.cpp: implementation of the CProcessMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessMsg::CProcessMsg()
{
	//::AfxMessageBox("Construction ProcessMsg!!");
	m_bComPortOpen = FALSE;
}

CProcessMsg::~CProcessMsg()
{
	//::AfxMessageBox("Destruction ProcessMsg!!");
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/*************************************************
  Function:         fnSendMsg
  Description:     类成员函数，发送获取系列号命令
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnSendMsg(char *pszMsg, int nLength) // nLength: Msg except the checksum
{
    BOOL  hr;

	fnCreateCheckCode(pszMsg, nLength);

	sprintf(pszMsg+nLength, "%c", m_chCheckCode);

	hr = WriteBuffer((char*)pszMsg, nLength +1);
	if (!hr)
	{
		//DWORD dwerrorcode = ::GetLastError();
		m_strLastError.Format("Write Buffer error");
	}

	return hr;
}

BOOL CProcessMsg::SetTesterMode()
{
	int nFeedbackLen;
	HRESULT hr=FALSE;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_TESTER_MODE, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{	
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen -1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SERIAL_ID;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		hr=TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	::MessageBox(NULL,pszErrorMsg, "Error", MB_OK|MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	///	::MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}	
	
	return hr;
}

BOOL CProcessMsg::WrirteI2CAddress(BYTE bAddressIndex)
{
	int nFeedbackLen;
	HRESULT hr=FALSE;
	CString strTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
    m_pszCmdBuf[0] = (char)OBJ_ID_WRITE_I2C_ADDRESS;
    m_pszCmdBuf[1] = 0;
    m_pszCmdBuf[2] = 1;
	m_pszCmdBuf[3] = 0x58+bAddressIndex;

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SERIAL_ID;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
//		// process response
//		CString strLogoInfo;
////		strLogoInfo.Format("%02x%02x%02x%02x", 
////							(UCHAR)m_pszCmdBuf[3], (UCHAR)m_pszCmdBuf[4], 
////							(UCHAR)m_pszCmdBuf[5], (UCHAR)m_pszCmdBuf[6]); // big-endian
//
//		char chTempSN[8];  // SN must be 8 char length
//		memcpy(chTempSN,&m_pszCmdBuf[3],8);
//		strLogoInfo.Format("%s",  chTempSN);
//
//
//	
// 		strRecvData = strLogoInfo;
		hr=TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	::MessageBox(NULL,pszErrorMsg, "Error", MB_OK|MB_ICONERROR);
		hr = FALSE;
	
	}
	
	catch (...) 
	{
		//::MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}	
	
	return hr;
}

BOOL CProcessMsg::ReadI2CAddress(CString &strAddress)
{
	int nFeedbackLen;
	HRESULT hr=FALSE;
	CString strRecvData;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_READ_I2C_ADDRESS, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen -1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SERIAL_ID;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		//CString strLogoInfo;
		strAddress.Format("%02x", 
							(UCHAR)m_pszCmdBuf[3]); // big-endian

		//char chTempSN[8];  // SN must be 8 char length
		//memcpy(chTempSN,&m_pszCmdBuf[3],8);
		//strLogoInfo.Format("%s",  chTempSN);


	
 		//strRecvData = strLogoInfo;
		hr=TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	::MessageBox(NULL,pszErrorMsg, "Error", MB_OK|MB_ICONERROR);
		hr = FALSE;
	
	}
	
	catch (...) 
	{
	//	::MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}	
	
	return hr;
}

/*************************************************
  Function:         fnCreateCheckCode
  Description:     类成员函数，发送获取系列号命令
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnCreateCheckCode(const char *pszMsg, int nLength)
{
	BYTE bySum = 0;
	for (int ix = 0; ix < nLength; ++ix) 
	{
		bySum +=pszMsg[ix];
	}

	m_chCheckCode = 0X00 - bySum;

	return TRUE;
}

/*************************************************
  Function:         fnGetMsg
  Description:     类成员函数，发送获取系列号命令
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnGetMsg(char* pszMsg, int nPayloadLen)
{
	HRESULT hr;
	BYTE chStatus;
	CString strTemp;
	
	memset(m_pszCmdBuffer, 0, MAX_BUFFER_LEN);
	
	// read port
	hr = ReadBuffer((char*)m_pszCmdBuffer, nPayloadLen + CHECK_SUM_LENGTH);
	if (!hr) 
	{
		m_strLastError = "Error:Read Communication Error";
		return FALSE;
	}

	// check checksum
	BYTE byCheckSum = m_pszCmdBuffer[nPayloadLen];
	
	fnCreateCheckCode(m_pszCmdBuffer, nPayloadLen);
	if (byCheckSum != m_chCheckCode) 
	{
		m_strLastError.Format("Error: CheckCode Error:%c:%c",byCheckSum, m_chCheckCode);
		return FALSE;
	}
	
	// check status code
	chStatus = m_pszCmdBuffer[0];
	if(!fnParseCommandByte(chStatus))
	{
		return FALSE;
	}

	// return payload data
	memcpy(pszMsg, m_pszCmdBuffer, nPayloadLen);//Payload: Access(1) + ObjID(1) + Data(x)

	return TRUE;
}

BOOL CProcessMsg::fnParseCommandByte(BYTE chStatus)
{
	if (RES_SUCCESS == chStatus)
	{
		return TRUE;
	}
	else
	{
		switch(chStatus) 
		{
			case RES_CHECKSUM_ERROR:
			{
				m_strLastError = _T("Error: Checksum error !");
				break;
			}
			case RES_SYNTAX_ERROR:
			{
				m_strLastError = _T("Error: Syntax error !");
				break;
			}
			case RES_INVALID_PARAMETER:
			{
				m_strLastError = _T("Error: Invalid parameter !");
				break;
			}
			case RES_COMMAND_EXECUTION_FAILED:
			{
				m_strLastError = _T("Error: Command execution failed !");	
				break;
			}
			case RES_FIRMWARE_INSTALLATION_FAILED:
			{
				m_strLastError = _T("Error: firmware installation failed !");	
				break;
			}

			default:
				break;
 
		}

		return FALSE;
	}
}

/********************************************************************************
* Function Name:    HexStrToInt 			     	    				     	*
* Function:         Hex string convert to Int                  					*
* Input:            1. None	  													*
* Output:           1. None														*
* Return Value:	    None														*
* Return Type:      void														*
* Create Data: 		None										                *
* Creator:			None														*
* Revision History: None														*
*********************************************************************************/
int CProcessMsg::fnHexStrToInt(CString CTem)
{
	char*  pcIndex;
	int    iReturnValue = 0;
	DWORD  dwCstringLength;

	pcIndex = (LPSTR)(LPCTSTR)CTem;
	dwCstringLength = CTem.GetLength();
	
	while(dwCstringLength--)
	{
		if ((*pcIndex >= '0') && (*pcIndex <= '9'))
           iReturnValue = (iReturnValue * 0x10) + (*pcIndex - '0');

        else if ((*pcIndex >= 'a') && (*pcIndex <= 'f'))
           iReturnValue = (iReturnValue * 0x10) + (*pcIndex - 'a' + 0xa);

        else if ((*pcIndex >= 'A') && (*pcIndex <= 'F'))
           iReturnValue = (iReturnValue * 0x10) + (*pcIndex - 'A' + 0xA);
		else
		{
			//::AfxMessageBox("十六进制值不对,Return 0");
			return 0;
		}
        pcIndex++;

	}
	return iReturnValue;
} 



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/*************************************************
  Function:         CProcessMsg::GetSerialNo
  Description:     类成员函数，发送获取系列号命令
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetSerialNo(CString &strRecvData)
{	
//	int nFeedbackLen;
//	HRESULT hr=FALSE;
//	// construct msg
//	
//	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
//	int nDataLen = 0;
//	int nCmdLen = nDataLen + 4; //code + checksum + len_value
//	
//	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SERIAL_ID, nDataLen>>8,nDataLen&0x00FF);
//	
//	try
//	{
//		// send msg
//		if(fnSendMsg(m_pszCmdBuf, nCmdLen -1) != TRUE)
//		{
//			throw GetLastError();
//		}
//		
//		// get response
//		nDataLen = LEN_BACK_SERIAL_ID;
//		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
//		
//		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
//		
//		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
//		{
//			throw GetLastError();		
//		}
//		
//		// process response
//		CString strLogoInfo;
////		strLogoInfo.Format("%02x%02x%02x%02x", 
////							(UCHAR)m_pszCmdBuf[3], (UCHAR)m_pszCmdBuf[4], 
////							(UCHAR)m_pszCmdBuf[5], (UCHAR)m_pszCmdBuf[6]); // big-endian
//
//		char chTempSN[8];  // SN must be 8 char length
//		memcpy(chTempSN,&m_pszCmdBuf[3],8);
//		strLogoInfo.Format("%s",  chTempSN);
//
//
//	
//		strRecvData = strLogoInfo;
//		hr=TRUE;
//	}
//	catch (char* pszErrorMsg) 
//	{
//		::MessageBox(NULL,pszErrorMsg, "Error", MB_OK|MB_ICONERROR);
//		return hr;
//	
//	}
//	
//	catch (...) 
//	{
//		::MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
//		return hr;
//	}	
//	return hr;

//	CString strRecvData;	
	BOOL bResponse = FALSE;
	PBYTE pbyDataBuffer = new BYTE[64]; 
	ZeroMemory(pbyDataBuffer,64);
	//SPI_FLASH_READ,IIC_FLASH_READ
	bResponse = fnGetMemoryData(2,"0", 32, strRecvData,  pbyDataBuffer);
	if (!bResponse)
	{
		AfxMessageBox("Read SN Error!");
		return FALSE;
	}
	CString strSN;
	strSN.Format("%s",pbyDataBuffer);
	strRecvData = strSN;

	if (pbyDataBuffer != NULL)
	{
		delete [] pbyDataBuffer;
	}
	return TRUE;
}

/*************************************************
  Function:         CProcessMsg::CWSSDlg
  Description:     类成员函数，发送获取版本号命令
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetVersion(CString &strRecvData,BYTE *cVersion)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_VER, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_VER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		int nMajorHardVer, nMinorHardVer;
        int nMajorFirmVer, nMinorFirmVer;
		
		nMajorHardVer = m_pszCmdBuf[3];
        nMinorHardVer = m_pszCmdBuf[4];
        nMajorFirmVer = m_pszCmdBuf[5];
        nMinorFirmVer = m_pszCmdBuf[6];

		if(cVersion != NULL)
		{
			cVersion[0] = m_pszCmdBuf[3];
			cVersion[1] = m_pszCmdBuf[4];
			cVersion[2] = m_pszCmdBuf[5];
			cVersion[3] = m_pszCmdBuf[6];
		}
	
		CString strLogoInfo;
		strLogoInfo.Format("Hardware Ver is:V%d.%d", nMajorHardVer, nMinorHardVer);
		strRecvData = strLogoInfo;
		strLogoInfo.Format("Software Ver is:V%d.%d", nMajorFirmVer, nMinorFirmVer);
		strRecvData += "    "+strLogoInfo;

		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
}
/*************************************************
  Function:         CWSSDlg::SetRS232Rate
  Description:     类成员函数，设置串口通讯波特率
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnSetRS232Rate(BYTE byRateIndex, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
 
	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_RS232RATE;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
    
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_RS232_RATE, nDataLen>>8,nDataLen&0x00FF);

	if( 0 == byRateIndex)
    {
	    m_pszCmdBuf[3] = (byte)(96 >> 8);
	    m_pszCmdBuf[4] = (byte)(96 & 0xFF);
		strRecvData="The baud rate has set to 9600b/s";
    }
	else if(1 == byRateIndex)
    {
	    m_pszCmdBuf[3] = (byte)(144 >> 8);
	    m_pszCmdBuf[4] = (byte)(144 & 0xFF);
		strRecvData="The baud rate has set to 14400b/s";
    }     
    else if(2 == byRateIndex)
    {
	    m_pszCmdBuf[3] = (byte)(192 >> 8);
	    m_pszCmdBuf[4] = (byte)(192 & 0xFF);
		strRecvData="The baud rate has set to 19200b/s";
    }
	else if(3 == byRateIndex)
    {
	    m_pszCmdBuf[3] =(byte)( 288 >> 8);
	    m_pszCmdBuf[4] = (byte)(288 & 0xFF);
		strRecvData="The baud rate has set to 28800b/s";
    }
    else if(4 == byRateIndex)
    {
	    m_pszCmdBuf[3] =(byte)( 384 >> 8);
	    m_pszCmdBuf[4] = (byte)(384 & 0xFF);
		strRecvData="The baud rate has set to 38400b/s";
    }
    else if(5 == byRateIndex)
    {
	    m_pszCmdBuf[3] = (byte)(576 >> 8);
	    m_pszCmdBuf[4] = (byte)(576 & 0xFF);
		strRecvData="The baud rate has set to 57600b/s";
    }
    else if(6 == byRateIndex)
    {
	    m_pszCmdBuf[3] = (byte)(1152 >> 8);
	    m_pszCmdBuf[4] = (byte)(1152 & 0xFF);
		strRecvData="The baud rate has set to 115200b/s";
    }
	

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			strRecvData="Cannot set baud rate!";
			throw GetLastError();			
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if( fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			strRecvData="Cannot set baud rate!";
			throw GetLastError();		
		}
		
		// process response

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{		
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
		
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;

}


/*************************************************
  Function:         CProcessMsg::GetHardwareDate
  Description:     类成员函数，得到硬件生产时间
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetHardwareDate(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_HARDWARE_DATE, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		// get response
		nDataLen = LEN_BACK_HARDWARE_DATE;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		int nHardwareYear, nHardwareMonth;
	
		nHardwareYear = m_pszCmdBuf[3] + 2000;
        nHardwareMonth = m_pszCmdBuf[4];
	
		CString strLogoInfo;
		strLogoInfo.Format("Hardware date is: %d weeks / %d", nHardwareMonth,nHardwareYear);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;    
}

/*************************************************
  Function:         CProcessMsg::GetHardwareDate
  Description:     类成员函数，得到固件生产时间
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetFirmwareDate(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_HARDWARE_DATE, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
	
		// get response
		nDataLen = LEN_BACK_FIRMWARE_DATE;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response

        int nSoftwareYear, nSoftwareMonth;
		
        nSoftwareYear = m_pszCmdBuf[3] + 2000;
        nSoftwareMonth = m_pszCmdBuf[4];
	
		CString strLogoInfo;
		strLogoInfo.Format("Software date is: %d weeks / %d" ,nSoftwareMonth, nSoftwareYear);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;    
}
/*************************************************
  Function:         CProcessMsg::GetAlarm
  Description:     类成员函数，得到告警状态
  Calls:           
  Called By:      
  Input:             
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetAlarm(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	BOOL bHasAlarm = FALSE;
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_ALARM, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_ALARM;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		CString strLogoInfo;
		strRecvData = "";
		BYTE byAlarmByte = m_pszCmdBuf[FEED_HEAD_LENGTH];

		// Bit 0: P/S Voltage Out of Range 
		if(0x01 == (byAlarmByte & 0x01))
        {
            bHasAlarm = TRUE;
            strLogoInfo.Format("P/S Voltage Out of Range.");
            strRecvData += strLogoInfo; 
        }


		// Bit 1: Case Temperature Out of Range
		if(0x02 == (byAlarmByte & 0x02))
        {
            bHasAlarm = TRUE;
            strLogoInfo.Format("  Case Temperature Out of Range.");
            strRecvData += strLogoInfo;
            
        }

		// Bit 2: Hardware failure
		if(0x04 == (byAlarmByte & 0x04))
        {
            bHasAlarm = TRUE;
            strLogoInfo.Format("  Hardware failure.");
            strRecvData += strLogoInfo;
            
        }
		
		// Bit 3-7: Reserved
		if((byAlarmByte & 0xF8))
        {
            bHasAlarm = TRUE;
            strLogoInfo.Format("  other error .");
            strRecvData += strLogoInfo;
            
        }
 
        
        if(FALSE == bHasAlarm)
        {
            strLogoInfo.Format("No alarms.");
            strRecvData = strLogoInfo;

        }
        hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;
    
}
/*************************************************
  Function:         CProcessMsg::GetSupplyVoltage
  Description:     类成员函数，得到支持电压
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetSupplyVoltage(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
    byte byTemp;	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_SUPPLY_VOLTAGE, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SUPPLY_VOLTAGE;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		WORD shVoltage;
        byTemp = m_pszCmdBuf[3];
        m_pszCmdBuf[3] = m_pszCmdBuf[4];
        m_pszCmdBuf[4] = byTemp;
		
		shVoltage = *(WORD*)&m_pszCmdBuf[3];
	
		CString strLogoInfo;
		strLogoInfo.Format("Voltage: %4d mv", shVoltage);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
		
	return hr;

}
/*************************************************
  Function:         CProcessMsg::GetTemper
  Description:     类成员函数，得到主板温度
  Calls:           
  Called By:      
  Input:            
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetTemper(CString &strRecvData, double &dblTempVal)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	byte byTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_TEMPER, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(200);
		// get response
		nDataLen = LEN_BACK_TEMPER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byTemp = m_pszCmdBuf[3];
        m_pszCmdBuf[3] = m_pszCmdBuf[4];
        m_pszCmdBuf[4] = byTemp;
		short shTemperature = 0;
		shTemperature = *(short*)&m_pszCmdBuf[3];

        double dbTemperature;
        dbTemperature = (double)shTemperature/10.0;

		// send out temperature
		dblTempVal = dbTemperature;

        
		CString strLogoInfo;
		strLogoInfo.Format("The temperature is: %.1f℃", dbTemperature);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		strRecvData = pszErrorMsg;
		hr = FALSE;
	}
	
	catch (...) 
	{
		hr = FALSE;
	}

	return hr;

}	 


/*************************************************
  Function:         CProcessMsg::SetLowTemper
  Description:     类成员函数，设置温度低告警域值
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::SetLowTemperThreshold(double dbValue)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_TEMPER;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_LOW_TEMPER, nDataLen>>8,nDataLen&0x00FF);
	short shTempValue = ((short)dbValue)*10;
    m_pszCmdBuf[3] = shTempValue>>8;
    m_pszCmdBuf[4] = shTempValue&0xff;

	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen =  LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response

		hr = TRUE;
 	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;

}
/*************************************************
  Function:         CProcessMsg::GetLowTemper
  Description:     类成员函数，得到温度低告警域值
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetLowTemperThreshold(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	byte byTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_LOW_TEMPER, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response

		nDataLen = LEN_BACK_LOW_TEMPER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byTemp = m_pszCmdBuf[3];
        m_pszCmdBuf[3] = m_pszCmdBuf[4];
        m_pszCmdBuf[4] = byTemp;

		short shTemperature;
		shTemperature = *(short*)&m_pszCmdBuf[3];        
        double dbTemperature;
        dbTemperature = (double)shTemperature/10;

        
		CString strLogoInfo;
		strLogoInfo.Format("The low temperature alarm is: %.2f℃", dbTemperature);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;

	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/*************************************************
  Function:        CProcessMsg::SetHighTemper
  Description:     类成员函数，设置温度高告警域值
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::SetHighTemperThreshold(double dbValue)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_TEMPER;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_HIGH_TEMPER, nDataLen>>8,nDataLen&0x00FF);

	short shTempValue = ((short)dbValue)*10;
    m_pszCmdBuf[3] = shTempValue>>8;
    m_pszCmdBuf[4] = shTempValue&0xff;
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}		
		// process response

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/*************************************************
  Function:         CProcessMsg::SetHighTemper
  Description:     类成员函数，得到温度高告警域值
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetHighTemperThreshold(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	byte byTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_HIGH_TEMPER, nDataLen>>8,nDataLen&0x00FF);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response


		nDataLen = LEN_BACK_HIGH_TEMPER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byTemp = m_pszCmdBuf[3];
        m_pszCmdBuf[3] = m_pszCmdBuf[4];
        m_pszCmdBuf[4] = byTemp;

		short shTemperature;
		shTemperature = *(short*)&m_pszCmdBuf[3];        
        double dbTemperature;
        dbTemperature = (double)shTemperature/10;

        
		CString strLogoInfo;
		strLogoInfo.Format("The high temperature alarm is: %.2f℃", dbTemperature);
		strRecvData = strLogoInfo;

		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;
}
#if 0
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetAllChnWaveplan(pWavePlanAll pstWavePlan,BYTE byPort,CString &strRecvData)
{
	BYTE byPortNum = pstWavePlan->byPortNum;
	byPortNum = 3;
	

	WORD wThisChnNum;
	WORD wThisChnSlidStart;
	WORD wThisChnSlidEnd;

	int nDataLen = 0;  //<6xN+1_H> <6xN+1_L>
	WORD wThisPortChnSum = pstWavePlan->pwChnNumSum[byPort-1];

	if (wThisPortChnSum > 169) // Max = (1024 -4(code + checksum + len_value)-1<P>  ) / 6(<C1-1 C1-2 S1-1 S1-2 E1-1 E1-2>)
	{
		AfxMessageBox("wThisPortChnSum > 169, do not support !");
	}
	
	nDataLen = 6*wThisPortChnSum +1;
	
	// Set waveplan of all channels
	// Command: <0x60> <6xN+1_H> <6xN+1_L> <P> <C1-1 C1-2 S1-1 S1-2 E1-1 E1-2> <……> <CN-1 CN-2 SN-1 SN-2 EN-1 EN-2> <CS>
	// N = Total Channel Number.If N=0,that is there is only <P>, all channels will be deleted from this port.
	// <P> = Port Number; 1 ≤ P≤ Max Port Number
	// < Cx-1 Cx-2 > = Channel Number(1- Max Channel Number)
	// < Sx-1 Sx-2 > = the first slice of the channel x
	// < Ex-1 Ex-2 > = the last slice of the channel x
	// Sx < Ex < Sx+1 must be satisfied. Channel x and channel (x+1) won't overlap each other.
	// Response: <Status> <0x00> <0x00> <CS>

	
	HRESULT hr = FALSE;
	int nFeedbackLen;
 
	// construct msg	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN); 
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_ALL_CHN_WAVEPLAN, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = byPort;
	
	int nPastPortSlid=0;
	for (int j=0;j<byPort-1;j++)
	{
		nPastPortSlid += pstWavePlan->pwChnNumSum[j];
	}
		
	 
	for (int i=0;i<wThisPortChnSum;i++)
	{
		
		int nThisIdx = nPastPortSlid + i;
		
		wThisChnNum       = pstWavePlan->pwChnNumber[byPort][nThisIdx];
		wThisChnSlidStart = pstWavePlan->pwSlidStart[byPort][nThisIdx]; 
        wThisChnSlidEnd   = pstWavePlan->pwSlidEnd[byPort][nThisIdx]; 

		m_pszCmdBuf[4+i*6] = wThisChnNum >> 8;
		m_pszCmdBuf[5+i*6] = wThisChnNum &  0x00FF;

		m_pszCmdBuf[6+i*6] = wThisChnSlidStart >> 8;
		m_pszCmdBuf[7+i*6] = wThisChnSlidStart &  0x00FF;

		m_pszCmdBuf[8+i*6] = wThisChnSlidEnd   >> 8;
		m_pszCmdBuf[9+i*6] = wThisChnSlidEnd   &  0x00FF;

	}
	
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
 
		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 10000; nCount++)
		{
			Sleep(20);
			 
			 
			nDataLen = LEN_BACK_0;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			CString strTemp;
			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	
 
		
		// process response 
		CString strLogoInfo;
		strLogoInfo.Format(" Have set Port_%d All channel waveplan ! ",byPort);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
}
#endif

#if 0
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGetAllChnWaveplan(pWavePlanAll pstWavePlan,BYTE byPort,CString &strRecvData)
{ 
	HRESULT hr = FALSE;
	int nFeedbackLen;
    BYTE byTemp;
	// construct msg  
	
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_ALL_CHN_WAVEPLAN, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = byPort;
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response 

		nDataLen = MAX_BUFFER_LEN - FEED_HEAD_LENGTH - 1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byTemp = m_pszCmdBuf[3];

		if(byPort != byTemp)
			AfxMessageBox("fnGetAllChnWaveplan(), response port error !");

        WORD wChnNumSum = m_pszCmdBuf[1];
		wChnNumSum <<= 8;
		wChnNumSum = wChnNumSum | m_pszCmdBuf[2];
	 
		
		// This function always reads only one port.
		//pstWavePlan->pwChnNumSum[0];

		if (1 == wChnNumSum)
		{	
			strRecvData.Format("This port no channel waveplan setting !");
			pstWavePlan->pwChnNumSum[byPort-1] = 0;

			hr = TRUE;
			return hr;
		}

		wChnNumSum -= 1;
		wChnNumSum /= 6;

		pstWavePlan->pwChnNumSum[byPort-1] = wChnNumSum;
 
		for (int i=0;i<wChnNumSum;i++)
		{
			BYTE byBig;
			BYTE byLittle;
			WORD wThisWord;
			
			
			// channel number
			byBig		= m_pszCmdBuf[4 +i*6];
			byLittle	= m_pszCmdBuf[5 +i*6];
			wThisWord   = byBig;
			wThisWord   = wThisWord <<8 | byLittle;			 
			pstWavePlan->pwChnNumber[byPort-1][i] =  wThisWord;

			
			// channel slice start
			byBig		= m_pszCmdBuf[6 +i*6];
			byLittle	= m_pszCmdBuf[7 +i*6];
			wThisWord   = byBig;
			wThisWord   = wThisWord <<8 | byLittle;			 
			pstWavePlan->pwSlidStart[byPort-1][i] = wThisWord;
			
			// channel slice end
			byBig		= m_pszCmdBuf[8 +i*6];
			byLittle	= m_pszCmdBuf[9 +i*6];
			wThisWord   = byBig;
			wThisWord   = wThisWord <<8 | byLittle;			 
			pstWavePlan->pwSlidEnd[byPort-1][i]   = wThisWord;
		}
		

        
		CString strLogoInfo;
		strLogoInfo.Format("Have gotten All channel waveplan ! ");
		strRecvData = strLogoInfo;

		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	return hr;
	 
}
#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSet1ChnWaveplan(BYTE byPort, WORD wChnNum, WORD wStart, WORD wEnd, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

	int nDataLen = 7;
 
	// construct msg	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN); 
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_1_CHN_WAVEPLAN, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = byPort; 

	m_pszCmdBuf[4] = wChnNum >> 8;
	m_pszCmdBuf[5] = wChnNum &  0x00FF;

	m_pszCmdBuf[6] = wStart >> 8;
	m_pszCmdBuf[7] = wStart &  0x00FF;

	m_pszCmdBuf[8] = wEnd   >> 8;
	m_pszCmdBuf[9] = wEnd   &  0x00FF;

 
	
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 10000; nCount++)
		{
			Sleep(20);
			 
			 
			nDataLen = LEN_BACK_0;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			CString strTemp;
			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	

 
        
		CString strLogoInfo;
		strLogoInfo.Format(" Have set Start%4d, End:%4d ! " , wStart, wEnd);
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr; 
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGet1ChnWaveplan(BYTE byPort, WORD wChnNum, WORD *wStart, WORD *wEnd, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	BYTE byTemp;
	// construct msg 


	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 3;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_1_CHN_WAVEPLAN, nDataLen>>8,nDataLen&0x00FF);

	m_pszCmdBuf[3] = byPort;
	m_pszCmdBuf[4] = wChnNum >> 8;
	m_pszCmdBuf[5] = wChnNum & 0x00FF;

	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response 

		nDataLen = 7;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
	
		// process response
		byTemp = m_pszCmdBuf[6];
		m_pszCmdBuf[6] = m_pszCmdBuf[7];
		m_pszCmdBuf[7] = byTemp;
		
		*wStart = *(WORD*)&m_pszCmdBuf[6];

		byTemp = m_pszCmdBuf[8];
		m_pszCmdBuf[8] = m_pszCmdBuf[9];
		m_pszCmdBuf[9] = byTemp;

		*wEnd   = *(WORD*)&m_pszCmdBuf[8];	
 

  
		CString strLogoInfo;
		strLogoInfo.Format("WavePlan start: %4d  end: %4d ! ",*wStart,*wEnd);
		strRecvData = strLogoInfo;

		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;

	}

	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
 
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSet1ChnAttn(BYTE byPort, WORD wChnNum, double dblAttn, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

	short shtAttn = dblAttn *10;

	int nDataLen = 4;
 
	// construct msg	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN); 
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_1_CHN_ATTN, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = byPort; 

	m_pszCmdBuf[4] = wChnNum >> 8;
	m_pszCmdBuf[5] = wChnNum &  0x00FF;

	m_pszCmdBuf[6] = (BYTE)shtAttn; 
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response 
		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 10000; nCount++)
		{
			Sleep(20);
			 
			 
			nDataLen = LEN_BACK_0;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			CString strTemp;
			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	

		
		 
        
		CString strLogoInfo;
		strLogoInfo.Format(" Have set channel_%d Attn:%.1f ! ",wChnNum ,dblAttn );
		strRecvData = strLogoInfo;

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr; 
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGet1ChnAttn(BYTE byPort, WORD wChnNum, double *pdblAttn, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
 
	// construct msg 


	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 3;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_1_CHN_ATTN, nDataLen>>8,nDataLen&0x00FF);

	m_pszCmdBuf[3] = byPort;
	m_pszCmdBuf[4] = wChnNum >> 8;
	m_pszCmdBuf[5] = wChnNum & 0x00FF;

	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response 

		nDataLen = 4;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
	
		// process response
		*pdblAttn = (BYTE)m_pszCmdBuf[6];
		*pdblAttn /= 10; 
  
		CString strLogoInfo;
		strLogoInfo.Format("Attn:%.1f! ",*pdblAttn);
		strRecvData = strLogoInfo;

		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
 
}
 
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGetStartFreq(double *pdblFreq, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
    BYTE byTemp;
	// construct msg 


	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_Start_FREQ, nDataLen>>8,nDataLen&0x00FF);
 
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response 

		nDataLen = 4;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
	
		// process response
		byTemp = m_pszCmdBuf[3];
		m_pszCmdBuf[3] = m_pszCmdBuf[6];
		m_pszCmdBuf[6] = byTemp;

		byTemp = m_pszCmdBuf[4];
		m_pszCmdBuf[4] = m_pszCmdBuf[5];
		m_pszCmdBuf[5] = byTemp;
	 
		*pdblFreq = *(DWORD*)&m_pszCmdBuf[3];
		 
		CString strLogoInfo;
		strLogoInfo.Format("start Frequency:%.1f ! ",*pdblFreq);
		strRecvData = strLogoInfo;

		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;

	}

	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
 
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnsoftReset(CString &strRecvData)
{
	char chRet[MAX_BUFFER_LEN] = "";  	
	memset(chRet, 0, MAX_BUFFER_LEN);
	
	chRet[0] = OBJ_ID_RESTART_SYSTEM;
	chRet[1] = 0x00;
	chRet[2] = LEN_SET_PWM_DUTY;
	chRet[3] = 0x69;  // special byte 
	
	
 	// send msg
	if(fnSendMsg(chRet, 4) != TRUE)
	{
		strRecvData = GetLastError();
		return FALSE;
	}	

	strRecvData = "soft reset command sent out !";
	
	return TRUE;

} 


/*************************************************
  Function:         CProcessMsg::GetATDData
  Description:     类成员函数，获取ATD驱动电压
  Calls:           
  Called By:      
  Input:          

  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetATDData(BYTE byTypeATD,WORD *wAdcVal,double *pdblVal,CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	char chBuf[4] ={0};
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_ATEDATA;
	int nCmdLen = nDataLen + 4;  //code + checksum + len_value
	//sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_GET_ATD_DATA, nDataLen>>8,nDataLen&0x00FF, byTypeATD);
	sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_GET_MONITOR_DATA, nDataLen>>8,nDataLen&0x00FF, byTypeATD);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(200);
		// get response
		nDataLen = LEN_BACK_ATD_DATA;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response 大小端转换 by luke.liu
		chBuf[0] = m_pszCmdBuf[6];
		chBuf[1] = m_pszCmdBuf[5];
		chBuf[2] = m_pszCmdBuf[4];
		chBuf[3] = m_pszCmdBuf[3];

		*pdblVal = *(float*)&chBuf; 
        
		CString strLogoInfo;
		
		//("0x00: 5V");
		//("0x01: temperature1");
		//("0x02: temperature2");
		//("0x03: LC temperature");
		//("0x04: mainboard temperature");
		//("0x05: heater Voltage");
		//("0x06: 3.3V");
		//("0x07: temperature3");
		//("0x08: 1.3V");
		//("0x09: 1.2V");
		//("0x0A: 1.8V");
		//("0x0B: VCOM");
		if( (byTypeATD == 0x03) ||
		    (byTypeATD == 0x04) ||
			(byTypeATD == 0x05) ||
			(byTypeATD == 0x06) ||
			(byTypeATD == 0x07) ||
			(byTypeATD == 0x08) ||
			(byTypeATD == 0x09))
		{
			strLogoInfo.Format("Monitor Channel %d, Value: %.2fmv", byTypeATD, *pdblVal);
		}
		else
		{
			strLogoInfo.Format("Monitor Channel %d, Value: %.2f ℃", byTypeATD, *pdblVal);
		} 
			
		strRecvData = strLogoInfo;

		hr = TRUE;
    }
	catch (char* pszErrorMsg) 
	{
		strRecvData = pszErrorMsg;
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

    return hr;

}
/*************************************************
  Function:         CProcessMsg::fnGetSW_InsideVersion
  Description:     类成员函数，Read SW inside Version
  Calls:           
  Called By:      
  Input:          

  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnGetSW_InsideVersion(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c", OBJ_ID_GET_SW_INSIDE_VERSION, nDataLen>>8,nDataLen&0x00FF);	
	try
	{
			// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SW_INSIDE_VERSION;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
 		
 		// process response
		// Response:<Status> <0x00> <0x0B> <D1-11><CS>
		// <D1-2>:BOOT SW inside Version                     // m_pszCmdBuf[3_4];
		// <D3-4>: master MAIN(ARM App) SW inside Version    // m_pszCmdBuf[5_6];
		// <D5-6>: slave MAIN(DSP App) SW inside Version     // m_pszCmdBuf[7_8];
		// <D7-9>: HW Version                                // m_pszCmdBuf[9_11];
		// <D10-11>: TEC slave boot Version                  // m_pszCmdBuf[12_13];
 		// <D12-13>: TEC slave Version						 // m_pszCmdBuf[14_15];



		WORD wBoot;
		WORD wArmApp;
		WORD wDspApp;
		//WORD wHWVer;
		WORD wSlaveVer;
		WORD wSlaveVerMain;
		
		UCHAR getTemp[2];
		
		memset(getTemp,0x0,sizeof(getTemp));
		
		// BOOT version		 
		getTemp[0] = m_pszCmdBuf[4];
		getTemp[1] = m_pszCmdBuf[3];
		wBoot   = *(WORD*)&getTemp[0];
		 

		// ARM App MCU version 
		getTemp[0] = m_pszCmdBuf[6];
		getTemp[1] = m_pszCmdBuf[5];
		wArmApp    = *(WORD*)&getTemp[0];

		// DSP App MCU version 
		getTemp[0] = m_pszCmdBuf[8];
		getTemp[1] = m_pszCmdBuf[7];
		wDspApp    = *(WORD*)&getTemp[0];
		

		// slave boot Version   
		getTemp[0] = m_pszCmdBuf[13];
		getTemp[1] = m_pszCmdBuf[12];
		wSlaveVer    = *(WORD*)&getTemp[0];
		
		// slave main Version   
		getTemp[0] = m_pszCmdBuf[15];
		getTemp[1] = m_pszCmdBuf[14];
		wSlaveVerMain    = *(WORD*)&getTemp[0];

		CString strLogoInfo;
		 
		strLogoInfo.Format( "Boot:0x%04X; "
							"ARM App:0x%04X; "
							"DSP App:0x%04X; "
							"HW: %d.%d.%d;"
							"Slave boot ver:0x%04X;"
							"Slave main ver:0x%04X;"
			                ,wBoot,wArmApp,wDspApp,
							m_pszCmdBuf[9],m_pszCmdBuf[10],m_pszCmdBuf[11],
							wSlaveVer,wSlaveVerMain);

        strRecvData = strLogoInfo;
    
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

    return hr;
}

/*************************************************
  Function:       
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnGetRGB(WORD &wLine, WORD &wCol, WORD &wRGB565,CString &strRecvData )
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// <0xC1> <0x00><0x04><L1-2><C1-2><CS> 

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0x04;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_RGB, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = wLine >> 8;
	m_pszCmdBuf[4] = wLine & 0x00FF;

	m_pszCmdBuf[5] = wCol >> 8;
	m_pszCmdBuf[6] = wCol & 0x00FF;


	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		// :<Status><0x00><0x02> <D1-2><CS>
		nDataLen = LEN_BACK_2;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		
		BYTE byTemp;
		byTemp = m_pszCmdBuf[4];
		m_pszCmdBuf[4] = m_pszCmdBuf[3];
		m_pszCmdBuf[3] = byTemp;
				
		// This RGB565
		wRGB565 = *(WORD*)&m_pszCmdBuf[3];
		
		// RGB565 => gray
		BYTE bGray = wRGB565 & 0x00FF; 

		bGray = ((bGray & 0x1F) << 3) | ((bGray & 0xE0) >> 5);
		
		CString strLogoInfo;
		
		strLogoInfo.Format("Line;%d; Column:%d, RGB:%d;Gray:%d!" ,wLine,wCol,wRGB565,bGray);
		strRecvData = strLogoInfo;
		
		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;


} 
/*************************************************
  Function:       
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnGetsensorPara(BYTE byVal, float &fParaA, float &fParaB, float &fParaC, float &fParaD, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// <0xD3> <0x00> <0x01 > <T><CS>

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_SENSOR_PARA, nDataLen>>8,nDataLen&0x00FF);
	 
	m_pszCmdBuf[3] = byVal;

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SENSOR_PARA;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		// <Status> <0x00> <0x10><A1-4> <B1-4><C1-4><D1-4><CS>
		BYTE byTemp;
		byTemp = m_pszCmdBuf[6];
		m_pszCmdBuf[6] = m_pszCmdBuf[3];
		m_pszCmdBuf[3] = byTemp;

		byTemp = m_pszCmdBuf[5];
		m_pszCmdBuf[5] = m_pszCmdBuf[4];
		m_pszCmdBuf[4] = byTemp;
				
		fParaA = *(float *)&m_pszCmdBuf[3];


		byTemp = m_pszCmdBuf[10];
		m_pszCmdBuf[10] = m_pszCmdBuf[7];
		m_pszCmdBuf[7] = byTemp;

		byTemp = m_pszCmdBuf[9];
		m_pszCmdBuf[9] = m_pszCmdBuf[8];
		m_pszCmdBuf[8] = byTemp;
				
		fParaB = *(float *)&m_pszCmdBuf[7];

		byTemp = m_pszCmdBuf[14];
		m_pszCmdBuf[14] = m_pszCmdBuf[11];
		m_pszCmdBuf[11] = byTemp;

		byTemp = m_pszCmdBuf[13];
		m_pszCmdBuf[13] = m_pszCmdBuf[12];
		m_pszCmdBuf[12] = byTemp;
				
		fParaC = *(float *)&m_pszCmdBuf[11];

		byTemp = m_pszCmdBuf[18];
		m_pszCmdBuf[18] = m_pszCmdBuf[15];
		m_pszCmdBuf[15] = byTemp;

		byTemp = m_pszCmdBuf[17];
		m_pszCmdBuf[17] = m_pszCmdBuf[16];
		m_pszCmdBuf[16] = byTemp;
				
		fParaD = *(float *)&m_pszCmdBuf[15];
		
		 
		
		CString strLogoInfo;
		
		strLogoInfo.Format("Have got sensor %d parameters !" ,byVal+1);
		strRecvData = strLogoInfo;
		
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;

}
/*************************************************
  Function:       
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnSetsensorPara(BYTE byVal, float fParaA, float fParaB, float fParaC, float fParaD, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// Command:<0xD4> <0x00> <0x11 > < T><A1-4> <B1-4><C1-4><D1-4><CS>
 
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0x11;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_SENSOR_PARA, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = byVal;
	// set data
	BYTE * pbyTemp;
	pbyTemp = (BYTE*)&fParaA;	
	
	m_pszCmdBuf[4] = *(pbyTemp+3);
	m_pszCmdBuf[5] = *(pbyTemp+2);
	m_pszCmdBuf[6] = *(pbyTemp+1);
	m_pszCmdBuf[7] = *(pbyTemp+0);
	
	pbyTemp = (BYTE*)&fParaB;
	m_pszCmdBuf[8] = *(pbyTemp+3);
	m_pszCmdBuf[9] = *(pbyTemp+2);
	m_pszCmdBuf[10] = *(pbyTemp+1);
	m_pszCmdBuf[11] = *(pbyTemp+0);

	pbyTemp = (BYTE*)&fParaC;
	m_pszCmdBuf[12] = *(pbyTemp+3);
	m_pszCmdBuf[13] = *(pbyTemp+2);
	m_pszCmdBuf[14] = *(pbyTemp+1);
	m_pszCmdBuf[15] = *(pbyTemp+0);

	pbyTemp = (BYTE*)&fParaD;
	m_pszCmdBuf[16] = *(pbyTemp+3);
	m_pszCmdBuf[17] = *(pbyTemp+2);
	m_pszCmdBuf[18] = *(pbyTemp+1);
	m_pszCmdBuf[19] = *(pbyTemp+0);

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		 
		strRecvData.Format("Write A:%e,B:%e,C:%e,D:%e !",fParaA,fParaB,fParaC,fParaD);
		 
		
		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/*************************************************
  Function:       
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnSetPidPara(BYTE byVal,float fPara,CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// Command:<0xD4> <0x00> <0x11 > < T><A1-4> <B1-4><C1-4><D1-4><CS>
 
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_DATA_3;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_PID_PARA, nDataLen>>8,nDataLen&0x00FF);
	
	m_pszCmdBuf[3] = byVal;
	
	// set data
	BYTE * pbyTemp;

	int nTemp = fPara*10;

	pbyTemp = (BYTE*)&nTemp;	
	
	m_pszCmdBuf[4] = *(pbyTemp+1);
	m_pszCmdBuf[5] = *(pbyTemp);
	 

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		 
		strRecvData.Format("Write Para %d :%f!",byVal,fPara);
		 
		
		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/*************************************************
  Function:       
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnGetPidPara(BYTE byVal,float &fPara,CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// <0xD3> <0x00> <0x01 > <T><CS>

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_PID_PARA, nDataLen>>8,nDataLen&0x00FF);
	 
	m_pszCmdBuf[3] = byVal;

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_2;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		// <Status> <0x00> <0x10><A1-4> <B1-4><C1-4><D1-4><CS>
		BYTE byTemp;
		byTemp = m_pszCmdBuf[4];
		m_pszCmdBuf[4] = m_pszCmdBuf[3];
		m_pszCmdBuf[3] = byTemp; 
				
		fPara = *(USHORT *)&m_pszCmdBuf[3]; 

		fPara /= 10;
		 
		
		CString strLogoInfo;
		
		strLogoInfo.Format("Have got PID %d parameters: %f !" ,byVal,fPara);
		strRecvData = strLogoInfo;
		
		hr = TRUE;

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/*************************************************
  Function:         CProcessMsg::GetHeaterTemp
  Description:     类成员函数，得到 one Heater温度
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::fnGetsensorTemp(BYTE bHeaterNum, CString &strRecvData, double &dblTemp)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_TEMPER_HERTERS, nDataLen>>8,nDataLen&0x00FF);
	m_pszCmdBuf[3] = bHeaterNum;

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_HEATER_TEMPER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		USHORT shTemperature;
		UCHAR getTemper[2];
		memset(getTemper,0x0,sizeof(getTemper));
		
		getTemper[0]=m_pszCmdBuf[4];
		getTemper[1]=m_pszCmdBuf[3];
		
		shTemperature = *(USHORT*)&getTemper[0];
		double dblTemperature = (short)shTemperature;
        dblTemperature /= 100;

		// get the temperature
		dblTemp = dblTemperature;
		
		CString strLogoInfo;
		
		strLogoInfo.Format("Sensor %d TEMP:%.2f℃", 
			                            bHeaterNum, dblTemperature);
		strRecvData = strLogoInfo;
		
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnCopyFileToRam(CString &strRecvData)
{
	int nFeedbackLen;
 
	CString strTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_0;   // lenght = 0;
	int nCmdLen = nDataLen + 4; //code+ len_value + checksum 
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_COPY_IMAGE_TO_OMAPL138, nDataLen>>8,nDataLen&0x00FF);
	
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		Sleep(50);

		// get response
		//nDataLen = LEN_BACK_0;
		//nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		//memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		//if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		//{
		//	throw GetLastError();		
		//}


		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 100; nCount++)
		{
			Sleep(20);
			nDataLen = LEN_BACK_0;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}		
		// process response
        // put out
		 
		strRecvData.Format("Image Copy command send successfully!");
		return TRUE; 

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// return TRUE;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnLoadImageToAsic(CString &strRecvData,BYTE byVal)
{
	int nFeedbackLen;
 
	// construct msg	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_WRITE_PIXEL_SAME;  // length = 1;
	int nCmdLen = nDataLen + 4; //code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_LOAD_IMAGE_SEND_TO_ASIC, nDataLen>>8,nDataLen&0x00FF);	 

	// image file index 
	m_pszCmdBuf[3] = byVal; 
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		 
		strRecvData.Format("Load image file %d to ASIC OK!",byVal);
		 

	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSaveImageToFlash(CString &strRecvData,BYTE byVal)
{ 
	int nFeedbackLen;
 

	CString strTemp,strTemp2;

	// construct msg	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_WRITE_PIXEL_SAME;  // length = 1;
	int nCmdLen = nDataLen + 4; //code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SAVE_IMAGE_TO_FLASH, nDataLen>>8,nDataLen&0x00FF);	 

	// image file index 
	m_pszCmdBuf[3] = byVal; 
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		strRecvData.Format("save image file %d command sent!",byVal);
	 
	}
	catch (char* pszErrorMsg) 
	{
	//	MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch (...) 
	{
	//	MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}


	// must wait for save to flash
	// query copy status  
	int nIndex = 10000;
	BYTE byStatus;
	while(nIndex--)
	{		
		fnQueryCopyStatus(strTemp2,byStatus);
		if (0 != byStatus)
		{
			Sleep(1800);
		}
		else
		{			 
			break;
		}


		if (1 == nIndex)
		{
			AfxMessageBox("Cannot save to MCU ! \r\nExit !");
			return FALSE;
		}			
	}
	
	strRecvData.Format(strTemp+strTemp2);


	return TRUE;
	
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnQueryCopyStatus(CString &strRecvData,BYTE &byVal)
{
	int nFeedbackLen;
	byte byBackVal;
	CString strTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_0;   // lenght = 0;
	int nCmdLen = nDataLen + 4; //code+ len_value + checksum 
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_QUERY_IMAGE_COPY_STATUS, nDataLen>>8,nDataLen&0x00FF);	 
 
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		nDataLen = LEN_BACK_REGISTER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byBackVal = m_pszCmdBuf[3];
        // put out
		byVal =  byBackVal;

		if (0 == byBackVal)
		{
			strTemp.Format("Status: complete!");
		}
		else if(1 == byBackVal)
		{
			strTemp.Format("Status: copying!");
		}
		else  
		{
			strTemp.Format("Status: copy error!");
		}
		 
		strRecvData.Format("Image Copy %s",strTemp);
		 

	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;

}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnReadDevRegister(BYTE byDevType,BYTE byAddr, CString &strRecvData,BYTE &byVal)
{
	if ((DEV_TYPE_LCOS == byDevType) && (byAddr > 0x4E))
	{
		strRecvData = "((DEV_TYPE_LCOS == byDevType) && (byAddr > 0x4E))";
		return FALSE;
	}
	
	int nFeedbackLen;
	byte byBackVal;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_DEV_REGISTER;
	int nCmdLen = nDataLen + 4; //code+ len_value + checksum 
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_READ_DEVICE_REGISTER, nDataLen>>8,nDataLen&0x00FF);	 

	// device type
	m_pszCmdBuf[3] = byDevType;
	// device register address
	m_pszCmdBuf[4] = byAddr; 
 
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		CString strTemp;		
		BOOL bMessageFail = FALSE;
		
		for(int nCount = 0; nCount < 1000; nCount++)
		{
			Sleep(20);
			nDataLen = LEN_BACK_REGISTER;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	

		
		// process response
        byBackVal = m_pszCmdBuf[3];
        // put out
		byVal =  byBackVal;

		if (DEV_TYPE_LCOS == byDevType)
		{
			strRecvData.Format("LCOS register 0x%X:0x%X",byAddr,byBackVal); 
		}
		else if (DEV_TYPE_ASIC == byDevType)
		{
			strRecvData.Format("ASIC register 0x%X:0x%X",byAddr,byBackVal);
		}
		else
		{
			strRecvData.Format("Device type error!");
			return FALSE;
		}

	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;

}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnWriteDevRegister(BYTE byDevType,BYTE byAddr, CString &strRecvData,BYTE byVal)
{
	if ((DEV_TYPE_LCOS == byDevType) && (byAddr > 0x4E))
	{
		strRecvData = "((DEV_TYPE_LCOS == byDevType) && (byAddr > 0x4E))";
		return FALSE;
	}	
	
	
	int nFeedbackLen;
	//byte byBackVal;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_WRITE_DEV_REGISTER;
	int nCmdLen = nDataLen + 4; //code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_WRITE_DEVICE_REGISTER, nDataLen>>8,nDataLen&0x00FF);	 

	// device type
	m_pszCmdBuf[3] = byDevType;
	// device register address
	m_pszCmdBuf[4] = byAddr; 
 
	m_pszCmdBuf[5] = byVal; 
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		CString strTemp;		
		BOOL bMessageFail = FALSE;
		
		for(int nCount = 0; nCount < 300; nCount++)
		{
			Sleep(20);
			nDataLen = LEN_BACK_0;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	
		 
		
		// process response 
		strRecvData.Format("write addr: %3d value:  0x%x OK!",byAddr,byVal);
		
		return TRUE;

	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return FALSE;

 
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGetSensorThrd(BYTE byVal, float &fThreshold, CString &strRecvData)
{
	int nFeedbackLen;
	CString strTemp;
	// construct msg
	
	// Command:<0xC6> <0x00><0x01><T> <CS> 

	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0x01;
	int nCmdLen = nDataLen + 4; // code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_SENSOR_THRESHOLD, nDataLen>>8,nDataLen&0x00FF);	 

	// all pixel same value 
	m_pszCmdBuf[3] = byVal; 

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 100000; nCount++)
		{
			Sleep(20);
			// Response:<Status><0x00><0x04><D1-4> <CS> 
			nDataLen = LEN_BACK_VER;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN); 

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") >-1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	
 
		
		// process response         
		// Response:<Status><0x00><0x04><D1-4> <CS> 
		BYTE byTemp;
		byTemp = m_pszCmdBuf[6];
		m_pszCmdBuf[6] = m_pszCmdBuf[3];
		m_pszCmdBuf[3] = byTemp;

		byTemp = m_pszCmdBuf[5];
		m_pszCmdBuf[5] = m_pszCmdBuf[4];
		m_pszCmdBuf[4] = byTemp;

		int nTemp = *(int *)&m_pszCmdBuf[3];

		fThreshold = nTemp / 10.0;
		              
		strRecvData.Format("S:%d,Threshold:%f",byVal,fThreshold);


		 
		return TRUE;	
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return FALSE;

	
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetSensorThrd(BYTE byVal, float fThreshold, CString &strRecvData)
{
	int nFeedbackLen;
	CString strTemp;
	// construct msg
	
	//Command:<0xC7> <0x00><0x05><T> <D1-4><CS> 

	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0x05;
	int nCmdLen = nDataLen + 4; // code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_SENSOR_THRESHOLD, nDataLen>>8,nDataLen&0x00FF);	 

	// all pixel same value 
	m_pszCmdBuf[3] = byVal;

	int nThreshold = fThreshold * 10;

	BYTE * pbyTemp;
	pbyTemp = (BYTE*)&nThreshold;	
	
	m_pszCmdBuf[4] = *(pbyTemp+3);
	m_pszCmdBuf[5] = *(pbyTemp+2);
	m_pszCmdBuf[6] = *(pbyTemp+1);
	m_pszCmdBuf[7] = *(pbyTemp+0);

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 100000; nCount++)
		{
			Sleep(20);
			nDataLen = LEN_BACK_NORMAL_SETTING;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

 

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") >-1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	
 
		
		// process response         
		strRecvData.Format("S:%d, threshold:%f !",byVal,fThreshold);
		 
		return TRUE;	
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return FALSE;

	
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetPixelSameVal(CString &strRecvData,BYTE byVal)
{	
	int nFeedbackLen;
	CString strTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_WRITE_PIXEL_SAME;
	int nCmdLen = nDataLen + 4; //code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_PIXEL_SAME_VALUE, nDataLen>>8,nDataLen&0x00FF);	 

	// all pixel same value 
	m_pszCmdBuf[3] = byVal;

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		BOOL bMessageFail = FALSE;
		for(int nCount = 0; nCount < 1000; nCount++)
		{
			Sleep(20);
			nDataLen = LEN_BACK_NORMAL_SETTING;
			nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

 

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
					
				if(strTemp.Find("Communication Error") >-1) //Error: Communication Error					 
				{
					continue;
				}
				else
				{
					throw GetLastError();
				}
			}
			else
			{
				break;
			} 
		}	
 
		
		// process response         
		strRecvData.Format("write Pixel: %3d value OK!",byVal);
		 
		return TRUE;	
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return FALSE;


}
 

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGetLCDCStatus(CString &strRecvData,BYTE &byVal)
{
	int nFeedbackLen;
	byte byBackVal;
	CString strTemp;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_0;   // lenght = 0;
	int nCmdLen = nDataLen + 4; //code+ len_value + checksum 
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_LCDC_STATUS, nDataLen>>8,nDataLen&0x00FF);	 

 
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		nDataLen = LEN_BACK_REGISTER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byBackVal = m_pszCmdBuf[3];
        // put out
		byVal =  byBackVal;


		//<D1>=1 LCDC Enable
		//<D1>=0 LCDC Disable


		if (0 == byBackVal)
		{
			strTemp.Format("Status: LCDC Disable!");
		}
		else if(1 == byBackVal)
		{
			strTemp.Format("Status: LCDC Enable!");
		}
		else  
		{
			strTemp.Format("Status: error!");
		}
		 
		strRecvData.Format("Get %s",strTemp);
		 

	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;

}

/*************************************************
  Function:          
  Description:      
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/ 
BOOL CProcessMsg::GetHeaterTargetTemper(BYTE byHeaterIdx,double &dblTagTemper,CString &strRecvData)
{
	int nFeedbackLen; 
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_1;    
	int nCmdLen = nDataLen + 4; //code+ len_value + checksum 
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_TARGET_TEMPER, nDataLen>>8,nDataLen&0x00FF);	 

	m_pszCmdBuf[3] = byHeaterIdx;
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		nDataLen = LEN_BACK_6;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        // Response:<Status> <0x00> <0x06> < K1-2>< C1-2><D1-2><CS>
		BYTE byTemp;
		
		// < K1-2> : target temperature parameter K*100 
		byTemp = m_pszCmdBuf[4];
		m_pszCmdBuf[4] = m_pszCmdBuf[3];
		m_pszCmdBuf[3] = byTemp;

		short shTemp = *(short *)&m_pszCmdBuf[3];
 
		float fParaK = shTemp;
		fParaK /= 100;
	 
		
		// < C1-2> : target temperature parameter C*100 
		byTemp = m_pszCmdBuf[6];
		m_pszCmdBuf[6] = m_pszCmdBuf[5];
		m_pszCmdBuf[5] = byTemp;

		shTemp = *(short *)&m_pszCmdBuf[5];
 
		float fParaC = shTemp;
		fParaC /= 100;

		
		// <D1-2>= 100 ×T (°C)
		byTemp = m_pszCmdBuf[8];
		m_pszCmdBuf[8] = m_pszCmdBuf[7];
		m_pszCmdBuf[7] = byTemp;

		shTemp = *(short *)&m_pszCmdBuf[7];
 
		float fParaData = shTemp;
		fParaData /= 100;
		
		dblTagTemper = fParaData; 
		 
		strRecvData.Format("K:%f; C:%f, Target Temper:%f",fParaK,fParaC,fParaData);
 
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}
/*************************************************
  Function:         CProcessMsg::SetHeaterTargetTemper
  Description:     类成员函数，set one Heater target temperature
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
/* ******************************************************************* 
Set the target temperature'parameters of heaters 
Command:<0xE5><0x00> <0x05 ><T> < K1-2>< C1-2> <CS>
<T>= type
0x01:heater1(TEC); 0x02:heater2; 0x03:heater3
< K1-2> : target temperature parameter K*100. -3≤ K≤ 3
< C1-2> : target temperature parameter C *100 ,0≤ C≤ 255
Response:<Status> <0x00> <0x00> <CS>
***************************************************************************** */
BOOL CProcessMsg::SetHeaterTargetTemper(BYTE byHeaterNum,double dblTagTemperK,double dblTagTemperC,CString &strRecvData)
{
    HRESULT hr = FALSE;

	if ((dblTagTemperC>255) || (dblTagTemperC<0))
	{
		strRecvData.Format("Target Temperature parameter error !");
		return FALSE;
	}
    	if ((dblTagTemperK>3) || (dblTagTemperK<-3))
	{
		strRecvData.Format("Target Temperature parameter error !");
		return FALSE;
	}
	USHORT usTargetTemperC = (USHORT)(dblTagTemperC*100);
	USHORT usTargetTemperK = (USHORT)(dblTagTemperK*100);
    
    int nFeedbackLen;
    // construct msg
    
    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
    int nDataLen = LEN_SET_HEATER_TEMPER;
    int nCmdLen = nDataLen + 4; //code + checksum + len_value
    sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_HEATER_TARGET_TEMPER, nDataLen>>8,nDataLen&0x00FF);
	m_pszCmdBuf[3] = byHeaterNum;
	
	// K parameter
   m_pszCmdBuf[4] = usTargetTemperK >> 8;
    m_pszCmdBuf[5] = usTargetTemperK & 0xff;
	
    // C parameter
	m_pszCmdBuf[6] = usTargetTemperC >> 8;
    m_pszCmdBuf[7] = usTargetTemperC & 0xff;
    
    try
    {
        // send msg
        if(fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
        {
            throw GetLastError();
        }
        
        // get response
        nDataLen = LEN_BACK_MODULE_TYPE;
        nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
        if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
        {
            throw GetLastError();		
        }
        
        // process response
		CString strMsg;
		if (1 == byHeaterNum )  
			strMsg.Format("Have set TEC target temperature %.1f,%.1f",dblTagTemperC,dblTagTemperK);
		else if(2 == byHeaterNum )  
			strMsg.Format("ERROR heater target temperature %.1f,%.1f",dblTagTemperC,dblTagTemperK);
		else if(3 == byHeaterNum )  
			strMsg.Format("ERROR target temperature %.1f℃",dblTagTemperC);
		else  
			strMsg.Format("Have set error heater target temperature %.1f℃",dblTagTemperC);

		strRecvData = strMsg; 
        
        hr = TRUE;
    }
    catch (char* pszErrorMsg) 
    {
        //MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }
    
    catch (...) 
    {
        //MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }
    return hr;

}
/*************************************************
*************************************************/
BOOL CProcessMsg::GetHeaterState(BYTE byVal, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// <0xD3> <0x00> <0x01 > <T><CS>

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_1;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_HEATER_STATE, nDataLen>>8,nDataLen&0x00FF);
	 
	m_pszCmdBuf[3] = byVal;

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		// <Status> <0x00> <0x01> <D><CS>  //  <D>  0:off;1:on
		BYTE byTemp;
		byTemp = m_pszCmdBuf[3];
		 
		if (1 == byTemp)
		{
			strRecvData.Format("This Heater is on !");
		} 
		else if (0 == byTemp)
		{
			strRecvData.Format("This Heater is off !");
		}
		else
		{
			strRecvData.Format("Heater parameter error !");
		}
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}
	return hr;
}
/*************************************************
  Function:         CProcessMsg::SetHeaterState
  Description:     类成员函数，set one Heater State
  Calls:           
  Called By:      
  Input:           byHeaterOnOff 0:off; 1:on
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::SetHeaterState(BYTE byHeaterNum,BYTE byHeaterOnOff,CString &strRecvData)
{
    // Command:<0xE4> <0x00> <0x02 > <T>< D> <CS>
	
	HRESULT hr = FALSE;	 
    
    int nFeedbackLen;

	CString pAstrHeaterOffOn[2] = {"OFF","ON"};
    // construct msg
    
    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
    int nDataLen = LEN_SET_HEATER_STATE;
    int nCmdLen = nDataLen + 4; //code + checksum + len_value
    sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_HERTERS_STATE, nDataLen>>8,nDataLen&0x00FF);
	m_pszCmdBuf[3] = byHeaterNum;
    m_pszCmdBuf[4] = byHeaterOnOff; 
	
	
    
    try
    {
        // send msg
        if(fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
        {
            throw GetLastError();
        }
        
        // get response
        nDataLen = LEN_BACK_MODULE_TYPE;//LEN_BACK_NORMAL_SETTING;
        nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
        if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
        {
            throw GetLastError();		
        }
                
		// process response
		// 0x01:heater1; 
		// 0x02:heater2;
		// 0x03:heater3
		// 0x04:TEC

		CString strMsg;
		if (1 == byHeaterNum )  
			strMsg.Format("Have set heater1:%s",pAstrHeaterOffOn[byHeaterOnOff]);
		else if(2 == byHeaterNum )  
			strMsg.Format("Have set heater2:%s",pAstrHeaterOffOn[byHeaterOnOff]);
		else if(3 == byHeaterNum )  
			strMsg.Format("Have set heater3:%s",pAstrHeaterOffOn[byHeaterOnOff]);
		else if(4 == byHeaterNum )  
			strMsg.Format("Have set TEC:%s",pAstrHeaterOffOn[byHeaterOnOff]);
		else  
			strMsg.Format("Have set error heater %s",pAstrHeaterOffOn[byHeaterOnOff]);

		strRecvData = strMsg; 
        
        hr = TRUE;
    }
    catch (char* pszErrorMsg) 
    {
        //MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }
    
    catch (...) 
    {
        //MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }
    return hr;

}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetLCDC(CString &strRecvData,BYTE byVal)
{	
	int nFeedbackLen;
	 
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_WRITE_PIXEL_SAME;
	int nCmdLen = nDataLen + 4; //code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_LCDC, nDataLen>>8,nDataLen&0x00FF);	 

	// all pixel same value 
	m_pszCmdBuf[3] = byVal;

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		Sleep(1000);
		

		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response         
		strRecvData.Format("Set LCDC: %3d value OK!",byVal);
	 
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;


}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnCheckHexVal(CString strHexVal)
{
	char*  pcIndex;	 
	DWORD  dwCstringLength;

	pcIndex = (LPSTR)(LPCTSTR)strHexVal;
	dwCstringLength = strHexVal.GetLength();

	if (dwCstringLength >2)
	{
		return FALSE;
	}
	
	 
	if ((*pcIndex >= '0') && (*pcIndex <= '9'))
       return TRUE;
    else if ((*pcIndex >= 'a') && (*pcIndex <= 'f'))
       return TRUE;
    else if ((*pcIndex >= 'A') && (*pcIndex <= 'F'))
       return TRUE;
	else
	{
		//::AfxMessageBox("十六进制值不对,Return 0");
		return 0;
	}
         
 
	return TRUE;
}


 
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetVcom(double dblVcomp, double dblVcomn, double dblVDDA, CString &strRecvData)
{
	// 1. set DC balance disable
	// set disable first for DC balance
	BYTE byRegVal;
	fnReadDevRegister(LCOS_DEVICE,0x00,strRecvData,byRegVal);
	
	BYTE byTemp = byRegVal;
	byTemp =  byTemp << 5;
	byTemp =  byTemp >> 7;
	if (1 != byTemp)
	{	
		byRegVal |= LCoS_SCIIC_ADDR_bit;
		fnWriteDevRegister(LCOS_DEVICE,0x00,strRecvData,byRegVal);
	}

	// 2. set LCoS 0x0C, 0x0D, 0x4D, 0x4E
	// double dblVcc = 6.43;
	double dblVcc = dblVDDA;
#if 1	
	BYTE   byVcomP  = ((dblVcc - dblVcomp) / dblVcc) *255;
	//double dblVcomn = 6.0 + dblVcomp;
	BYTE   byVcomN  = ((dblVcc - dblVcomn) / dblVcc) *255;

	//BYTE byVcomN = 0;
#else
	BYTE   byVcomP  = 255 - dblVcomp;
	BYTE   byVcomN  = dblVcomn;
#endif

	Sleep(200);
	fnWriteDevRegister(0,0x0C,strRecvData,byVcomN);  // 0x0C = VcomN
	Sleep(200);
	fnWriteDevRegister(0,0x4D,strRecvData,byVcomN);  // 0x0D = VcomP
	Sleep(200);
	fnWriteDevRegister(0,0x0D,strRecvData,byVcomP);
	Sleep(200);
	fnWriteDevRegister(0,0x4E,strRecvData,byVcomP);
	Sleep(200);
	

	// 3. set DC balance enable
	// set enable for DC balance
	//BYTE byRegVal;
	fnReadDevRegister(LCOS_DEVICE,0x00,strRecvData,byRegVal);	

	byTemp = byRegVal;
	byTemp =  byTemp << 5;
	byTemp =  byTemp >> 7;
	if (0 != byTemp)
	{	
		BYTE byTemp2   = ~LCoS_SCIIC_ADDR_bit;
		byRegVal &= byTemp2;
		fnWriteDevRegister(LCOS_DEVICE,0x00,strRecvData,byRegVal);
	} 

	return TRUE;

}

 
/*************************************************
*************************************************/
BOOL CProcessMsg::GetHeaterPwr(BYTE byHeaterIdx,double &dblReadBackPwr,CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	CString strTemp;
	
	//  Command:<0xE0> <0x00> <0x01 ><T> <CS>

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_READ_1;
	int nCmdLen = nDataLen + 4; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_HERTERS_TOTAL_POWERS, nDataLen>>8,nDataLen&0x00FF);
	 
	m_pszCmdBuf[3] = byHeaterIdx;

	try
	{
 
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		// <Status> <0x00> <0x01> <D><CS>
		BYTE byTemp;
		byTemp = m_pszCmdBuf[3];
		 
		dblReadBackPwr =  byTemp;
		
		dblReadBackPwr /= 10;
		
		strTemp.Format("The power of heater %d is: %.2f",byHeaterIdx,dblReadBackPwr);
		strRecvData = strTemp;
		
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	return hr;
}
/*************************************************
  Function:         CProcessMsg::SetHeaterTotalPwr
  Description:     类成员函数，set all heaters total power
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::SetHeaterTotalPwr(BYTE byHeaterIdx,float fPower_w,CString &strRecvData)
{
    HRESULT hr = FALSE;	 
    
	if((fPower_w >= 10) || (fPower_w <= 0))
		return hr;

	short sPower = (short)fPower_w * 10;
	
    int nFeedbackLen;
    // construct msg
    
    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
    int nDataLen = LEN_SET_HEATER_TOTAL_PWR;
    int nCmdLen = nDataLen + 4; //code + checksum + len_value
    sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_HERTERS_TOTAL_POWERS, nDataLen>>8,nDataLen&0x00FF);
	
	// 0x01:heater1; 0x02:heater2; 0x03:heater3; 0x04:TEC
	m_pszCmdBuf[3] = byHeaterIdx;
    m_pszCmdBuf[4] = sPower & 0xFF;    
    
    try
    {
        // send msg
        if(fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
        {
            throw GetLastError();
        }
        
        // get response
        nDataLen = LEN_BACK_NORMAL_SETTING;
        nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
        if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
        {
            throw GetLastError();		
        }
        
		// process response
		CString strMsg;
		strMsg.Format("Have set heaters %d total power %.1fw",byHeaterIdx,fPower_w);
		strRecvData = strMsg; 
        
        hr = TRUE;
    }
    catch (char* pszErrorMsg) 
    {
       // MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
    }
    
    catch (...) 
    {
       // MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
    }
    return hr;

}



/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetDacForVccLCoS(CString &strRecvData,double dblVal)
{	
	WORD wVal;

	wVal = (dblVal * 1000)/1.57 ;
	
	
	int nFeedbackLen;
	 
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_BACK_0_2;
	int nCmdLen = nDataLen + 4; //code  + len_value + checksum
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_DAC_for_VCC_LCOS, nDataLen>>8,nDataLen&0x00FF);	 

	// all pixel same value 
	m_pszCmdBuf[3] = wVal>>8;
	m_pszCmdBuf[4] = wVal&0x00FF;

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		Sleep(1000);
		

		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response         
		strRecvData.Format("Set DAC: %d value OK!",wVal);
	 
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;


}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnSetHardwareReset(BYTE byFlashType,CString dataAddress, WORD wDataLength, CString &strRecvData, PBYTE pbyBuffer)
{
	// Command:<0xEA> <N+5_H> <N+5_L> <T>< A1-4> <D1-N> <CS>
    // Response:<Status> <0x00> <0x00> <CS>
	// N=data length
	// <T>=memory type
	// 0x00 :SPI flash
	// 0x01:Nand flash
	// 0x02:I2C memory
	// < A1-4>= beginning address
	// <D1-N> = data

	CString strLogoInfo;
	HRESULT hr = FALSE;
 
	int nFeedbackLen;
	int  iAddress = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = wDataLength + 1 + 4; // add byFlashType + address_length;
	int nCmdLen = nDataLen + LEN_SET_MEMORY_HEAD +1 ; // add CheckCode;
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_FLASH_DATA, nDataLen>>8, nDataLen&0x00FF);

	iAddress = fnHexStrToInt(dataAddress);
	
	// flash type
	m_pszCmdBuf[3] = byFlashType; 
	
	// data address
	m_pszCmdBuf[4] = (byte)(iAddress  >> 24 );
	m_pszCmdBuf[5] = (byte)((iAddress >> 16 ) & 0x00FF);
	m_pszCmdBuf[6] = (byte)((iAddress  >> 8 ) & 0x0000FF);
	m_pszCmdBuf[7] = (byte)(iAddress          & 0x000000FF);
	
	// data 
	for (int i=8;i<wDataLength+8;i++)
	{
		m_pszCmdBuf[i]= pbyBuffer[i-8];
	}

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(60000);
		// get response
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			//throw GetLastError();		
		}

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	return hr;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnWriteMemoryData(BYTE byFlashType,CString dataAddress, DWORD wDataLength, CString &strRecvData, PBYTE pbyBuffer,BOOL bChange)
{
	// Command:<0xEA> <N+5_H> <N+5_L> <T>< A1-4> <D1-N> <CS>
    // Response:<Status> <0x00> <0x00> <CS>
	// N=data length
	// <T>=memory type
	// 0x00 :SPI flash
	// 0x01:Nand flash
	// 0x02:I2C memory
	// < A1-4>= beginning address
	// <D1-N> = data
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;	
	CString strTemp;
	int  iAddress = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = wDataLength + 1 + 4; // add byFlashType + address_length;
	int nCmdLen = nDataLen + LEN_SET_MEMORY_HEAD +1 ; // add CheckCode;
	//m_pszCmdBuf[0]=0xea;
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_FLASH_DATA, nDataLen>>8, nDataLen&0x00FF);

	if (bChange) 
	{
		iAddress = fnHexStrToInt(dataAddress);
	}
	else
	{
		iAddress = atoi(dataAddress);
	}

	//m_pszCmdBuf[0]=OBJ_ID_SET_FLASH_DATA;
	// flash type
	m_pszCmdBuf[3] = byFlashType; 
	

	//iAddress = 1792;
	// data address
	m_pszCmdBuf[4] = (byte)(iAddress  >> 24 );
	m_pszCmdBuf[5] = (byte)((iAddress >> 16 ) & 0x00FF);
	m_pszCmdBuf[6] = (byte)((iAddress  >> 8 ) & 0x0000FF);
	m_pszCmdBuf[7] = (byte)(iAddress          & 0x000000FF);


	//DWORD_TO_BYTE(m_pszCmdBuf[1], m_pszCmdBuf[2], m_pszCmdBuf[3], m_pszCmdBuf[4], dataAddress);
	//DWORD_TO_BYTE(m_pszCmdBuf[4], m_pszCmdBuf[5], m_pszCmdBuf[6], m_pszCmdBuf[7], dataAddress);
	
	// data 
	for (int i=8;i<wDataLength+8;i++)
	{
		m_pszCmdBuf[i]= pbyBuffer[i-8];
	}


	try
	{

		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		// Response:<Status> <0x00> <0x00> <CS>
		nDataLen = LEN_BACK_0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

		BOOL bMessageFail = FALSE;
		CString strTemp;
		for(int nCount = 0; nCount < 1000; nCount++)
		{
			Sleep(20);			
			memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

			bMessageFail = fnGetMsg(m_pszCmdBuf, nFeedbackLen);

			if(!bMessageFail)
			{
				strTemp = GetLastError();
			 
				 if(strTemp.Find("Communication Error") > -1) //Error: Communication Error					 
				 {
					 continue;
				 }
				 else
				 {
					 throw GetLastError();
				 }
			 }
			 else
			 {
				 break;
			 } 
		}	
 
		strRecvData.Format("Write Memory data finished !");
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		strTemp=pszErrorMsg;
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	return hr;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGetMemoryData(BYTE byFlashType,CString dataAddress, DWORD wDataLength, CString &strRecvData, PBYTE pbyBuffer,BOOL bChange,BOOL bRegister)
{
	// Command:  <0xEB> <0x00> <0x07 ><T> <A1-4> <L1-2 > <CS>
    // Response: <Status> <L1-2> <D1-L> <CS>	  
	
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;	
	int  iTemp;
	int  iAddress = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_FLASH;
	int nCmdLen = nDataLen + 4;
	if (bRegister)
	{
		nDataLen = 9;
		nCmdLen = nDataLen + 4;
	}
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_FLASH_DATA, nDataLen>>8, nDataLen&0x00FF);

	if (bChange)
	{
		iAddress = fnHexStrToInt(dataAddress);
	}
	else
	{
		iAddress=atoi(dataAddress);
	}


	
	// flash type
	m_pszCmdBuf[3] = byFlashType; 
	
	// data address
	m_pszCmdBuf[4] = (byte)(iAddress  >> 24 );
	m_pszCmdBuf[5] = (byte)((iAddress >> 16 ) & 0x00FF);
	m_pszCmdBuf[6] = (byte)((iAddress  >> 8 ) & 0x0000FF);
	m_pszCmdBuf[7] = (byte)(iAddress          & 0x000000FF);

	if (!bRegister)
	{
		// data length
		m_pszCmdBuf[8]=(byte)((wDataLength >> 8 ) & 0x00FF);
		m_pszCmdBuf[9]=(byte)(wDataLength & 0x00FF);
	}
	else
	{
		// data length
		m_pszCmdBuf[8]=(byte)((wDataLength >> 24));
		m_pszCmdBuf[9]=(byte)((wDataLength >> 16 )& 0x00FF);
		// data length
		m_pszCmdBuf[10]=(byte)((wDataLength>> 8) & 0x0000FF);
		m_pszCmdBuf[11]=(byte)(wDataLength       & 0x000000FF);
	}
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = wDataLength;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response 
		// Response: <Status> <L1-2> <D1-L> <CS>
		for(iTemp=0; iTemp<wDataLength; iTemp++)
		{ 
			BYTE byTemp = m_pszCmdBuf[iTemp +3]; 

			pbyBuffer[iTemp] = byTemp;

			//strLogoInfo.Format("0x%2X ",byTemp);
			//strRecvData += strLogoInfo;
		}

		strRecvData.Format("Get flash data finished !");

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	return hr;
}

/************************************************************************/
/*by luke.liu 2018.1.11 for DGE module                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnGetMemoryData(CString dataAddress, WORD wDataLength, CString &strRecvData, PBYTE pbyBuffer,BOOL bChange,BOOL bRegister)
{
	// Command:  <0xEB> <0x00> <0x04 ><A1-4> <L> <CS>
    // Response: <Status> <0x00><L> <D1-L> <CS>	  
	
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;	
	int  iTemp;
	int  iAddress = 0;
	BYTE bDGEindex=0;

	if(m_nDGEIndex > 2)
	{
		m_strLastError.Format("DGE index is not more then 2");
		return FALSE;
	}

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_FLASH;
	int nCmdLen = nDataLen + 4;
	if (bRegister)
	{
		nDataLen = 9;
		nCmdLen = nDataLen + 4;
	}
	bDGEindex = (BYTE)(m_nDGEIndex+1);
	sprintf(m_pszCmdBuf, "%c%c%c%c",OBJ_ID_GET_FLASH_DATA, nDataLen>>8, nDataLen&0x00FF, bDGEindex);

	if (bChange)
	{
		iAddress = fnHexStrToInt(dataAddress);
	}
	else
	{
		iAddress=atoi(dataAddress);
	}
	
	// data address
	//m_pszCmdBuf[3] = (byte)(iAddress  >> 24 );
	//DGE keep 3 bytes for address
	m_pszCmdBuf[4] = (byte)((iAddress >> 16 ) & 0x00FF);
	m_pszCmdBuf[5] = (byte)((iAddress  >> 8 ) & 0x0000FF);
	m_pszCmdBuf[6] = (byte)(iAddress          & 0x000000FF);

	if (!bRegister)
	{
		// data length
		//m_pszCmdBuf[7]=(byte)((wDataLength >> 8 ) & 0x00FF);
		m_pszCmdBuf[7]=(byte)(wDataLength & 0x00FF);
	}
	else
	{
		// data length
		m_pszCmdBuf[8]=(byte)((wDataLength >> 24));
		m_pszCmdBuf[9]=(byte)((wDataLength >> 16 )& 0x00FF);
		// data length
		m_pszCmdBuf[10]=(byte)((wDataLength>> 8) & 0x0000FF);
		m_pszCmdBuf[11]=(byte)(wDataLength       & 0x000000FF);
	}
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = wDataLength;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response 
		// Response: <Status> <L1-2> <D1-L> <CS>
		for(iTemp=0; iTemp<wDataLength; iTemp++)
		{ 
			BYTE byTemp = m_pszCmdBuf[iTemp +3]; 

			pbyBuffer[iTemp] = byTemp;

			//strLogoInfo.Format("0x%2X ",byTemp);
			//strRecvData += strLogoInfo;
		}

		strRecvData.Format("Get flash data finished !");

		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	return hr;
}






//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
 










 

/********************************************************************************
* Function Name:    fnTxRxCommand				    					     	*
* Function:         Send command and receive response  Value   					*
* Input:            1. None	  													*
* Output:           1. None														*
* Return Value:	    None														*
* Return Type:      void														*
* Create Data: 		None										                *
* Creator:			None														*
* Revision History: None														*
*********************************************************************************/
BOOL CProcessMsg::fnTxRxCommand(BYTE byCmdObject,BYTE byReadWrite,DWORD dwAddr,DWORD dwWriteValue,DWORD &dwResponseValue,DWORD dwSleepTime)
{
	BYTE	byTxBuf[MAX_BUF_DATA];
	BYTE	byRxBuf[MAX_BUF_DATA];
	BYTE	byCmdLen;
	BYTE    byOmdReadWrite;	
	
	byCmdLen = MSG_BASIC_LENGTH;
	ZeroMemory(byTxBuf, MAX_BUF_DATA);

	if (CMD_READ == byReadWrite)
	{
		byOmdReadWrite = OMD_READ;
		//下发字节定义
		byCmdLen    = MSG_BASIC_LENGTH;
		byTxBuf[0]  = (BYTE)0xFF;
		byTxBuf[1]  = byCmdLen;
		byTxBuf[2]  = (BYTE)0x00;
		byTxBuf[3]  = (BYTE)CMD_READ;
		byTxBuf[4]  = (BYTE)byOmdReadWrite;
		byTxBuf[5]  = (BYTE)byCmdObject;
		byTxBuf[6]  = (BYTE)(dwAddr>>24);
		byTxBuf[7]  = (BYTE)(dwAddr>>16);
		byTxBuf[8]  = (BYTE)(dwAddr>>8); 
		byTxBuf[9]  = (BYTE)dwAddr;			
		byTxBuf[10] = GenerateCheckSum(byTxBuf, byCmdLen-1);
	}
	else if (CMD_WRITE == byReadWrite)
	{
		byOmdReadWrite = OMD_WRITE;
		//下发字节定义
		byCmdLen    = MSG_BASIC_LENGTH + 4;
		byTxBuf[0]  = (BYTE)0xFF;
		byTxBuf[1]  = byCmdLen;
		byTxBuf[2]  = (BYTE)0x00;
		byTxBuf[3]  = (BYTE)CMD_READ;
		byTxBuf[4]  = (BYTE)byOmdReadWrite;
		byTxBuf[5]  = byCmdObject; 
		byTxBuf[6]  = (BYTE)(dwAddr>>24);
		byTxBuf[7]  = (BYTE)(dwAddr>>16);
		byTxBuf[8]  = (BYTE)(dwAddr>>8); 
		byTxBuf[9]  = (BYTE)dwAddr;	
		byTxBuf[10] = (BYTE)(dwWriteValue >> 24);
		byTxBuf[11] = (BYTE)(dwWriteValue >> 16);
		byTxBuf[12] = (BYTE)(dwWriteValue >> 8);
		byTxBuf[13] = (BYTE)(dwWriteValue);		
		byTxBuf[14] = GenerateCheckSum(byTxBuf,byCmdLen - 1);		
	}

	
	///////发送
	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{	
		MessageBox(NULL, "RS232发送数据错误!", "提示", MB_OK|MB_ICONERROR);
		return FALSE;
	}	

	Sleep(dwSleepTime);

	//////////////////////////////接收数据
	if (CMD_READ == byReadWrite)
	{
		byCmdLen = MSG_BASIC_LENGTH + 4;
	}
	else if(CMD_WRITE == byReadWrite)
	{
		byCmdLen = MSG_BASIC_LENGTH;
	}
	else
	{
		//some error
		//MessageBox(NULL,"输入fnTxRxCommand(),应是正常的读写方向!!","错误提示",MB_OK|MB_ICONERROR);
		return FALSE;
	}
				
	ZeroMemory(byRxBuf, MAX_BUF_DATA);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
		//MessageBox(NULL,"RS232接收数据错误!", "提示", MB_OK|MB_ICONERROR);
		return FALSE;
	}


	//Check Response CMD
	if(!ParseErrorCode(byRxBuf[2]))
	{
		MessageBox(NULL,"对比byRxBuf[2]不对!!","错误提示",MB_OK|MB_ICONERROR);
		return FALSE;		
	}
	
	//获得读的返回值
	if (CMD_READ == byReadWrite)
	{		
		dwResponseValue = (DWORD)(byRxBuf[MSG_IDX_DATA]<<24) + (DWORD)(byRxBuf[MSG_IDX_DATA+1]<<16)
			             +(DWORD)(byRxBuf[MSG_IDX_DATA+2]<<8) + (DWORD)(byRxBuf[MSG_IDX_DATA+3]);
	}
	else
	{
		dwResponseValue = dwWriteValue;
	}	

	return TRUE;
}

/********************************************************************************
* Function Name:    GenerateCheckSum				    				     	*
* Function:         Generate Check Sum                      					*
* Input:            1. None	  													*
* Output:           1. None														*
* Return Value:	    None														*
* Return Type:      void														*
* Create Data: 		None										                *
* Creator:			None														*
* Revision History: None														*
*********************************************************************************/
BYTE CProcessMsg::GenerateCheckSum(BYTE* pbyData, BYTE byCmdLength)
{
	BYTE  byIndex;
	BYTE  byCheckSum, byThisByte;

	byCheckSum = 0;
	for(byIndex = 0; byIndex < byCmdLength; byIndex ++)
	{
		byThisByte = pbyData[byIndex];
		byCheckSum += byThisByte;
	}

	 byCheckSum = 0xFF - byCheckSum;

	 return byCheckSum;
}
/********************************************************************************
* Function Name:    ParseErrorCode			     	    				     	*
* Function:         Generate Check Sum                      					*
* Input:            1. None	  													*
* Output:           1. None														*
* Return Value:	    None														*
* Return Type:      void														*
* Create Data: 		None										                *
* Creator:			None														*
* Revision History: None														*
*********************************************************************************/
BOOL CProcessMsg::ParseErrorCode(BYTE byStatus)
{
	CString strErrorMsg;

	if(byStatus ==  ST_SUCCESS||byStatus == ST_MESSGAE_COMPLETE)
	{	
		return TRUE;
	}
	else
	{
		if(byStatus == ST_IDLE)
		    strErrorMsg = _T("Error: Command IDLE");

	    else if(byStatus == ST_Message_Incomple	)
	    	strErrorMsg = _T("Error: Command incomple");

    	else if(byStatus == ST_MESSAGE_VALIEL)
	    	strErrorMsg = _T("Error: Messgae Valid");

    	else if(byStatus == ST_INVALID_LENGTH)
	    	strErrorMsg = _T("Error: Invalid_Length");

    	else if(byStatus == ST_INVALID_ACCESS)
	    	strErrorMsg = _T("Error: Invalid Access");

    	else if(byStatus == ST_OBJECT_ID_OUTSIDE)
	    	strErrorMsg = _T("Error: Object ID Out Side");

    	else if(byStatus == ST_INVALID_DATA)
	     	strErrorMsg = _T("Error: Invalid data");

		else if(byStatus == ST_INVALID_CHECKSUM)
			strErrorMsg = _T("Error: Invalid Checksum");

		else if(byStatus == ST_COMMAND_IN_EXECUTION)
			strErrorMsg = _T("Error: Command In Execution");

		else if(byStatus == ST_AUTONOMOUS_MESSAGE)
			strErrorMsg = _T("Error: Autonomous Message");
        else 
	     	strErrorMsg = _T("Error: Reserved Failure");

		MessageBox(NULL, strErrorMsg, "Info", MB_OK|MB_ICONERROR);

		return FALSE;
	}
	return TRUE;
}

 




/********************************************************************************
* Function Name:    SaveMyFileName 			     	    				     	*
* Function:         保存文件对话框架                         					*
********************************************************************************/
BOOL CProcessMsg::fnSaveMyFileName(CString cFileType,CString &cFile)
{
	CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrTitle= "Txt文件保存对话框";
	if(dlg.DoModal() != IDOK)
	{
		return FALSE;
	}
	cFile=dlg.GetPathName(); 
	
	//Get Current Day
	CTime tm;
	tm=CTime::GetCurrentTime();
	CString strTime1;
	strTime1=tm.Format("%Y%m%d");
	//CString s=tm.Format("%H:%M:%S");//转换时间格式

	cFileType = strTime1 + cFileType;  //将时间加入文件名
	 
	CString sTableName;
	sTableName.Format(".%s",cFileType);
	if(cFile.Find(".txt")!=-1||cFile.Find(".TXT")!=-1)
	{
		cFile=cFile.Left(cFile.GetLength()-4);
	}
	if(cFile.Find(sTableName)==-1)//if(cFile.Find(sTableName)==-1&&cFile.Find(sTableName)==-1)
	{
		cFile+=sTableName;
	}
	
	cFile+=".txt";
	//m_cFile = cFile;
	
	return TRUE;
}

/*************************************************
  Function:         CProcessMsg::SetChannelsDACValue
  Description:     类成员函数，设置通道DAC值
  Calls:           
  Called By:      
  Input:           
                  byte byChannelType        要设置的通道ID类型
                  USHORT usChannelDac       要设置的DAC值               
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::SetAllDACValues(byte byChannelType, USHORT usChannelDac)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	//Command :<0xF1> <0x00> <0x03> <T> <D1-2> <CS>
	
	//DGE doesn't need offset
	usChannelDac += DAC_VALUE_OFFSET;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CHANNEL_DAC; 
	int nCmdLen = nDataLen + 4;

	m_pszCmdBuf[0] = (BYTE)OBJ_ID_SET_DAC_CHANNELS;
	m_pszCmdBuf[1] = (BYTE)(nDataLen>>8);
	m_pszCmdBuf[2] = (BYTE)nDataLen;
	m_pszCmdBuf[3] = (BYTE)m_nDGEIndex;
	m_pszCmdBuf[4] = byChannelType;
    m_pszCmdBuf[5] = (BYTE)(usChannelDac>>8);
    m_pszCmdBuf[6] = (BYTE)usChannelDac;


	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

		Sleep(500);
		
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;
}

/*************************************************
  Function:         CProcessMsg::SetHalfChDACValues
  Description:     类成员函数，设置通道DAC值
  Calls:           
  Called By:      
  Input:           
                  byte byChannelType        要设置的通道ID类型
                  USHORT usChannelDac       要设置的DAC值
                  
                   
  Output:        
  Return:         BOOL
*************************************************/

BOOL CProcessMsg::SetHalfDACValues(byte byChannelType, USHORT usChannelDac)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	usChannelDac += DAC_VALUE_OFFSET;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CHANNEL_DAC;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_HALF_DAC_CHANNELS, nDataLen, byChannelType);
    m_pszCmdBuf[3] = usChannelDac>>8;
    m_pszCmdBuf[4] = usChannelDac&0xff;


	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);

		Sleep(3);
		
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;

}

/* by luke.liu
BOOL CProcessMsg::SetOneDACValue(byte byChannel, USHORT usChannelDac)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

    if((0xFF ==  byChannel) ||
        ((usChannelDac < 0) || (usChannelDac > 8192)))
        return hr;
	
	usChannelDac += DAC_VALUE_OFFSET;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CHANNEL_DAC;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_SET_DAC_CHANNEL, 
		nDataLen, 
		byChannel		
        );
    m_pszCmdBuf[3] = usChannelDac>>8;
    m_pszCmdBuf[4] = usChannelDac&0xff;


	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;

}
*/

BOOL CProcessMsg::GetChannelDACValue(byte byChannel,WORD &wReadDAC, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
    byte byTemp;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_CHANNEL_DAC;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_DAC_CHANNEL, 
		nDataLen, 
		(byte)byChannel
        );
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_DAC_CHANNEL;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
        byTemp = m_pszCmdBuf[2];
        m_pszCmdBuf[2] = m_pszCmdBuf[3];
        m_pszCmdBuf[3] = byTemp;


		CString strLogoInfo;
        USHORT usDACValue;
        usDACValue = *(USHORT*)&m_pszCmdBuf[2];
		
		wReadDAC = (WORD)usDACValue;
		strLogoInfo.Format("Read DAC value of No.%d is: %d", byChannel, usDACValue);
        strRecvData = strLogoInfo;
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;


}
/*************************************************
  Function:         CProcessMsg::ReadTableData
  Description:     类成员函数，获取数据
  Calls:           
  Called By:      
  Input:          
            int iDataNumber     要获取的表格号

            USHORT usGetLens    要获取的长度
                   
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::ReadTableData(int iDataNumber,long ldataindex,USHORT usGetLens)
{
	HRESULT hr = FALSE;
    FILE *fGetDataFile = NULL;
	int nFeedbackLen;
    int i;
    byte byGetLens;
    
	int nDataLen = LEN_GET_TABLE_DATA;
	int nCmdLen = nDataLen + 3;
    
    for(i=0;i<=usGetLens/EVERY_DATA_LEN;i++)
    {
        if((usGetLens/EVERY_DATA_LEN) == i)
        {
            byGetLens = usGetLens%EVERY_DATA_LEN;
            if(0 == byGetLens)
            {
                break;
            }
        }
        else
        {
            byGetLens = EVERY_DATA_LEN;
        }
		
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	    sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_READ_TABLE_DATA, nDataLen);
		m_pszCmdBuf[2] = (byte)iDataNumber;
        m_pszCmdBuf[3] = (byte)(ldataindex>>16);
        m_pszCmdBuf[4] = (byte)(ldataindex>>8);
        m_pszCmdBuf[5] = (byte)ldataindex;
	    m_pszCmdBuf[6]=byGetLens;
        
	    try
	    {
		    // send msg
		    if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		    {
			    throw GetLastError();
		    }
		    
		    // get response
		    nDataLen = byGetLens;
		    nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		    if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		    {
			    throw GetLastError();		
		    }
		    if(NULL == fGetDataFile)
            {
                fGetDataFile = fopen("GetTable.bin","w+");
            }
            fwrite(&m_pszCmdBuf[2], nDataLen, 1, fGetDataFile);
			if (fGetDataFile != NULL)
			{
				fclose(fGetDataFile);
				fGetDataFile = NULL;
			}

            ldataindex += EVERY_DATA_LEN;
	    }
	    catch (char* pszErrorMsg) 
	    {
		    MessageBox( NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
			return hr;
	    }
	    
	    catch (...) 
	    {
		    MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
            return hr;
	    }


    }

    if((usGetLens/EVERY_DATA_LEN) < i )
    {
        		// process end
        if(fGetDataFile != NULL)
        {
            fflush(fGetDataFile);
            fclose(fGetDataFile);
        }
		CString strLogoInfo;
		strLogoInfo.Format("Reading Table data finished,saved to GetTable.bin");
    }

	hr = TRUE;
    return hr;

}


/*************************************************
  Function:         CProcessMsg::SetSpectrumAttenuation
  Description:     类成员函数，设置通道衰减值
  Calls:           
  Called By:      
  Input:           
                  int nChannel 对应通道端口号
                  double dbValue 要设置的衰减值
                   
  Output:        
  Return:         BOOL
*************************************************/

BOOL CProcessMsg::SetSpectrumAttenuation(int nPort,int nChannel, double dbValue)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_SINGLE_ATTENUATION;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c%c%c%c", OBJ_ID_SET_SINGLE_ATTENATION, 
		nDataLen, 
		(byte)nChannel,
		(byte)nPort,		
		(byte)(dbValue*10)
        );
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	return hr;   
	
}

/*************************************************
  Function:         CProcessMsg::GetSpectrumAttenuation
  Description:     类成员函数，得到通道衰减值
  Calls:           
  Called By:      
  Input:           
                  int nChannel 对应通道端口号
                  
                   
  Output:        
  Return:         BOOL
*************************************************/

BOOL CProcessMsg::GetSpectrumAttenuation(int nChannel,CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_SINGLE_ATTENUATION;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_SINGLE_ATTENATION, 
		nDataLen, 
		(byte)nChannel
        );
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
	
		// get response
		nDataLen = LEN_BACK_SINGLE_ATTENATION;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		CString strLogoInfo;
        double iAttenuation;
		int   strport;
		strport=(int)((byte)m_pszCmdBuf[2]);
        iAttenuation = (double)((byte)m_pszCmdBuf[3])/10;
		strLogoInfo.Format("The attenuation and port of channel%d are: %.1f dB,port%d",nChannel,iAttenuation,strport);
        strRecvData = strLogoInfo;
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	return hr;
}







/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnReadLcosVolADC(BYTE byType,WORD wCount,CString &strRecvData,PWORD pwADC)
{
	//BYTE byTemp;
	int nFeedbackLen;
	WORD nBackVal= 0;
	WORD wAveADC = 0;
	WORD wBuf[100] = {0};
    BYTE byCount1=0,byCount2=0;
	byCount1=wCount<<8;
	byCount2=wCount;
	// construct ms
	int nDataLen=0;
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nCmdLen = LEN_READ_VOLTAGE_ADC_6;//<0xC9> <0x00> <0x03><T><L1-2><CS>

//	sprintf(m_pszCmdBuf, "%c%c%c%c%c%c",OBJ_ID_READ_LCOS_VOLTAGE_ADC, 0x00,0x03,byType,byCount1,byCount2);	 

	 
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen) != TRUE)
		{
			throw GetLastError();
		}
	
		
		// get response
		nDataLen = wCount*2;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response  100 WORD = 200 BYTE
		

		for (int i=0;i<wCount;i++)  
		{	
			//byTemp = m_pszCmdBuf[2+ i*2];
			//m_pszCmdBuf[2+ i*2] = m_pszCmdBuf[2+ i*2+1];
			//m_pszCmdBuf[2+ i*2+1] = byTemp;
			
			pwADC[i] = *(PWORD)&m_pszCmdBuf[3+ i*2];
			//wAveADC += *(PWORD)&m_pszCmdBuf[2+ i*2];			 
		}

	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CProcessMsg::fnReSorting(PWORD pwSource,int nSum)
{ 
	WORD wTemp;

	for (int i=0;i<nSum;i++)
	{
		for (int k=i+1;k<nSum;k++)
		{
			if (pwSource[i] < pwSource[k])
			{
				wTemp = pwSource[k];
				pwSource[k] =pwSource[i];
				pwSource[i] = wTemp;
			}
		}
	}

	return TRUE;
}






/*************************************************
  Function:         CProcessMsg::GetSVNVersion
  Description:     类成员函数，得到SVN版本号
  Calls:           
  Called By:      
  Input:           

  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetSVNVersion(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 3; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c", OBJ_ID_GET_SVN_VERSION, nDataLen);	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SVN_VERSION;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response


		CString strLogoInfo;		
        strLogoInfo.Format("Master MCU FW inside version is %d.%d\n\r Slave MCU FW inside version is %d.%d",
			                (byte)m_pszCmdBuf[2],(byte)m_pszCmdBuf[3],
							(byte)m_pszCmdBuf[4],(byte)m_pszCmdBuf[5]);
        strRecvData = strLogoInfo;
    
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;
}

/*************************************************
  Function:         CProcessMsg::GetLCTemper
  Description:     类成员函数，得到LC温度
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetLCTemper(CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	int nCmdLen = nDataLen + 4; //by luke.liu code + checksum + len_value
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c",OBJ_ID_GET_LC_TEMPER, nDataLen>>8&0xff, nDataLen&0xff, bDGEIndex);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_LC_TEMPER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		USHORT shTemperature;
		UCHAR getTemper[3];
		memset(getTemper,0x0,sizeof(getTemper));
		
		getTemper[0]=m_pszCmdBuf[4];
		getTemper[1]=m_pszCmdBuf[3];
		
		shTemperature = *(USHORT*)&getTemper[0];
		double dblTemperature = (short)shTemperature;
        dblTemperature /= 100;
		
		CString strLogoInfo;
		
		strLogoInfo.Format("%.2f", dblTemperature);
		strRecvData = strLogoInfo;
		
		hr = TRUE;
 	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	return hr;
}

/*************************************************
  Function:         CProcessMsg::GetGratingTemp
  Description:     类成员函数，得到Grating温度
  Calls:           
  Called By:      
  Input:           
  Output:        
  Return:         BOOL
*************************************************/
BOOL CProcessMsg::GetGratingTemp(BYTE bGratingNum, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	int nCmdLen = nDataLen + 3; //code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_GET_GRATING_TEMP, nDataLen);
	m_pszCmdBuf[2] = bGratingNum;
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_GRATING_TEMPER;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		USHORT shTemperature;
		UCHAR getTemper[3];
		memset(getTemper,0x0,sizeof(getTemper));
		
		getTemper[0]=m_pszCmdBuf[3];
		getTemper[1]=m_pszCmdBuf[2];
		
		shTemperature = *(USHORT*)&getTemper[0];
		double dblTemperature = (short)shTemperature;
        dblTemperature /= 10;
		
		CString strLogoInfo;
		
		strLogoInfo.Format("The Grating %d temperature is: %.1f℃", 
			                            bGratingNum, dblTemperature);
		strRecvData = strLogoInfo;
		
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	return hr;
}

 

BOOL CProcessMsg::GetHeaterTempRange(CString &strRecvData)
{
    HRESULT hr = FALSE;
    int nFeedbackLen;
    WORD wLowLimit;
	WORD wHighLimit;

    // construct msg
    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
    int nDataLen = 0;
    int nCmdLen = nDataLen + 3; //code + checksum + len_value
    sprintf(m_pszCmdBuf, "%c%c", OBJ_ID_GET_TEMPER_RANGE, nDataLen);
    
    try
    {
        // send msg
        if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
        {
            throw GetLastError();
        }
        
        // get response
        nDataLen = LEN_BACK_GET_HEAT_TEMP_RANGE;
        nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
        if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
        {
            throw GetLastError();		
        }
        
        // process response
        wLowLimit  = m_pszCmdBuf[2] << 8;
        wLowLimit += m_pszCmdBuf[3];
		wHighLimit  = m_pszCmdBuf[4] << 8;
        wHighLimit += m_pszCmdBuf[5];
        
        CString strLogoInfo;
        strLogoInfo.Format("The low temperature Limit: %.1f\n The high temperature Limit: %.1f", 
			                (float)wLowLimit / 10.0, (float)wHighLimit / 10.0);


        strRecvData = strLogoInfo;
        hr = TRUE;
        
    }
    catch (char* pszErrorMsg) 
    {
        MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }
    
    catch (...) 
    {
        MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }
  
    return hr;
}

BOOL CProcessMsg::SetHeaterTempRange(WORD wLowLimit, WORD wHighLimt, CString &strRecvData)
{
    HRESULT hr = FALSE;
    int nFeedbackLen;
	CString strLogoInfo;

    // construct msg    
    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
    int nDataLen = LEN_SET_HEAT_TEMP_RANGE;
    int nCmdLen = nDataLen + 3; //code + checksum + len_value
    sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_TEMPER_RANGE, nDataLen);
	m_pszCmdBuf[2] = (byte)(wLowLimit >> 8 );
	m_pszCmdBuf[3] = (byte)(wLowLimit & 0xFF);
    m_pszCmdBuf[4] = (byte)(wHighLimt >> 8 );
	m_pszCmdBuf[5] = (byte)(wHighLimt & 0xFF);
        
    try
    {
        // send msg
        if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
        {
            throw GetLastError();
        }
        
        // get response
        nDataLen = LEN_BACK_NORMAL_SETTING;
        nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
        if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
        {
            throw GetLastError();		
        }
        
        // process response
        strLogoInfo.Format("%s", m_pszCmdBuf);
        strRecvData = strLogoInfo; 
		
        hr = TRUE;
    }
    catch (char* pszErrorMsg) 
    {
        MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
        hr = FALSE;
    }
    catch (...) 
    {
        MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
    }

	return hr;        
}




BOOL CProcessMsg::SetProfileInUse(byte bProfileNum,CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_PROFILE_IN_USE;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_FLATNESS_CURVE,nDataLen);

	m_pszCmdBuf[2]=bProfileNum+1;
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("Profile ID:%d has set in use",bProfileNum+1);
	strRecvData=strLogoInfo;
	return hr;
}

BOOL CProcessMsg::GetProfileInUse(CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_PROFILE_IN_USE;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_GET_PROFILE_IN_USE,nDataLen);

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_PROFILE_IN_USE;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("The profile ID:%d is being used",m_pszCmdBuf[2]);
	strRecvData=strLogoInfo;
	return hr;
}

BOOL CProcessMsg::SetConfToPro1(CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CONF_TO_PRO1;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SAVE_CONFIGURATION_TO_PROFILE1,nDataLen);

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		Sleep(200);
		// get response
		nDataLen = LEN_BACK_CONF_TO_PRO1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("Set complete");
	strRecvData=strLogoInfo;
	return hr;

}

BOOL CProcessMsg::SwitchPort(int nPort,int nChannel ,CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SWITCH_PORT;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SWITCH_PORT,nDataLen);
	m_pszCmdBuf[2]=nChannel;
	m_pszCmdBuf[3]=nPort;

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_CONF_TO_PRO1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("Have set channel:%d to port:%d",nChannel+1,nPort);
	strRecvData=strLogoInfo;
	return hr;

}


BOOL CProcessMsg::SetChannelAttenuation(int nChannel,double attenuation,CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CHANNEL_ATTENUATION;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_CHANNEL_ATTENUATION,nDataLen);
	m_pszCmdBuf[2]=nChannel;
	if(attenuation>25.5||attenuation<0)
	{
		MessageBox(NULL,"Please input right value","Error",MB_OK);
		return hr;
	}
	
	m_pszCmdBuf[3]=byte(attenuation*10);

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_CONF_TO_PRO1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("Have set %0.1fdB attenuation to channel:%d ",attenuation,nChannel+1);
	strRecvData=strLogoInfo;
	return hr;
}

BOOL CProcessMsg::SetChannelAttenuationChange(int nChannel,double deltaAttenuation,CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CHANNEL_DELTA_ATTENUATION;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_CHANNEL_DELTA_ATTENUATION,nDataLen);
	m_pszCmdBuf[2]=nChannel;
	
	if(deltaAttenuation>12.7||deltaAttenuation<-12.8)
	{
		MessageBox(NULL,"Please input right value","Error",MB_OK);
		return hr;
	}
	
	if(deltaAttenuation<=12.7&&deltaAttenuation>-0.1)
	{
		m_pszCmdBuf[3]=byte(deltaAttenuation*10);
	}
	if(deltaAttenuation<=-0.1&&deltaAttenuation>=-12.8)
	{
		m_pszCmdBuf[3]=byte(deltaAttenuation*10)+256;
	}

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_CONF_TO_PRO1;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("Have set %0.1fdB attenuation change to channel:%d",deltaAttenuation,nChannel+1);
	strRecvData=strLogoInfo;
	return hr;

}
/********************************************************************************
* Function Name:    SetMoudleSN 			     	    				     	*
* Function:         Set module serial number                  					*
* Input:            	  													    *
* Output:           													        *
* Return Value:	    														    *
* Return Type:      BOOL														*
* Create Data: 												                    *
* Creator:																	    *
* Revision History: 														    *
********************************************************************************/
BOOL CProcessMsg::SetModuleSN(CString strValue,CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_MODULE_SN;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_MODULE_SN,nDataLen);
    
	//save input serial number to buffer
	for(int i=0,j=2;i<strValue.GetLength();i++)
	{
		m_pszCmdBuf[j]=fnHexStrToInt(strValue.Mid(i,2));
		i++,j++;
	}
    
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_MODULE_SN;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("The module serial number has been set to:%s",strValue);
	strRecvData=strLogoInfo;
	return hr;

}
/********************************************************************************
* Function Name:    SetMoudleType 			     	    				     	*
* Function:         Set module Type                  					        *
* Input:            	  													    *
* Output:           													        *
* Return Value:	    														    *
* Return Type:      BOOL														*
* Create Data: 												                    *
* Creator:																	    *
* Revision History: 														    *
********************************************************************************/
BOOL CProcessMsg::SetModuleType(CString strModuleType,CString &strRecvData)
{
	CString strLogoInfo,strModuleTypeName;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	BYTE bModuleType[LEN_SET_MODULE_TYPE];

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_MODULE_TYPE;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_MODULE_TYPE,nDataLen);
    
	//save input type to a buffer
	for(int i=0,j=2,k=0;i<strModuleType.GetLength();i++,k++)
	{
		m_pszCmdBuf[j]=fnHexStrToInt(strModuleType.Mid(i,2));
		bModuleType[k]=m_pszCmdBuf[j];
		i++,j++;
	}
    
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SET_MODULE_TYPE;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	switch (bModuleType[0])
	{
		case 1:
			strModuleTypeName="1X1 WSS";
			break;
		case 2:
			strModuleTypeName="1X2 WSS";
			break;
		case 3:
			strModuleTypeName="1X4 WSS";
			break;
		case 4:
			strModuleTypeName="1X8 WSS";
			break;
		case 5:
			strModuleTypeName="2X1 WSS";
			break;
		case 6:
			strModuleTypeName="4X1 WSS";
			break;
		case 7:
			strModuleTypeName="8X1 WSS";
			break;
		default:
			break;
	}

	strLogoInfo.Format("The module type has been set to:%s",strModuleTypeName);
	strRecvData=strLogoInfo;
	return hr;

}
/********************************************************************************
* Function Name:    SetHWVersion 			     	    				     	*
* Function:         Set hardware version                  					    *
* Input:            	  													    *
* Output:           													        *
* Return Value:	    														    *
* Return Type:      BOOL														*
* Create Data: 												                    *
* Creator:																	    *
* Revision History: 														    *
********************************************************************************/
BOOL CProcessMsg::SetHWVersion(CString strHWVersion,CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	BYTE bHWVersion[LEN_SET_HW_VERSION];

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_HW_VERSION;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_HW_VERSION,nDataLen);
    
	//save input hardware version to a buffer
	for(int i=0,j=2;i<strHWVersion.GetLength();i++,j++)
	{
		m_pszCmdBuf[j]=fnHexStrToInt(strHWVersion.Mid(i,1));
		bHWVersion[i]=m_pszCmdBuf[j];
	}
    
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_HW_VERSION;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	strLogoInfo.Format("The hardware version has been set to:V %1d.%1d",bHWVersion[0],bHWVersion[1]);
	strRecvData=strLogoInfo;
	return hr;

}
/********************************************************************************
* Function Name:    SetHWDate 			     	    				     	    *
* Function:         Set hardware code date                  					*
* Input:            	  													    *
* Output:           													        *
* Return Value:	    														    *
* Return Type:      BOOL														*
* Create Data: 												                    *
* Creator:																	    *
* Revision History: 														    *
********************************************************************************/
BOOL CProcessMsg::SetHWDate(CString strHWDate, CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;
	BYTE bHWDate[LEN_SET_HW_DATE_CODE];

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_HW_DATE_CODE;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_HW_DATE_CODE,nDataLen);
    
	//save input code date to  a buffer
	for(int i=0,j=2,k=0;i<strHWDate.GetLength();i++,k++)
	{
		m_pszCmdBuf[j]=fnHexStrToInt(strHWDate.Mid(i,2));
		bHWDate[k]=m_pszCmdBuf[j];
		i++,j++;
	}
    
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_HW_DATE_CODE;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	strLogoInfo.Format("The hardware code date has been set to:20%02dyear/%dweeks",bHWDate[0],bHWDate[1]);
	strRecvData=strLogoInfo;
	return hr;

}
/********************************************************************************
* Function Name:    SetMaxChannNum 			     	    				     	*
* Function:         Set max channel number                  					*
* Input:            	  													    *
* Output:           													        *
* Return Value:	    														    *
* Return Type:      BOOL														*
* Create Data: 												                    *
* Creator:																	    *
* Revision History: 														    *
********************************************************************************/
BOOL CProcessMsg::SetMaxChannNum(CString strModuleTemp, CString &strRecvData)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	int nFeedbackLen;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_MAX_CHANNEL_NUM;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c",OBJ_ID_SET_MAX_CHANNEL_NUM, nDataLen);

	m_pszCmdBuf[2]=atoi(strModuleTemp);

#ifdef _WSS_50GHZ
	if(m_pszCmdBuf[2]>96|| m_pszCmdBuf[2]<1)
#else
	if(m_pszCmdBuf[2]>96|| m_pszCmdBuf[2]<1)
#endif
	{
		MessageBox(NULL,"Max channel number is wrong!","Error",MB_OK|MB_ICONERROR);
		return hr;
	}

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_SET_MAX_CHANNEL_NUM;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	strLogoInfo.Format("The max channel number has been set to:%s",strModuleTemp);
	strRecvData=strLogoInfo;
	return hr;

}
/********************************************************************************
* Function Name:    GetMaxChannNum 			     	    				     	*
* Function:         Get max channel number                  					*
* Input:            	  													    *
* Output:           													        *
* Return Value:	    														    *
* Return Type:      BYTE														*
* Create Data: 												                    *
* Creator:																	    *
* Revision History: 														    *
********************************************************************************/
BOOL CProcessMsg::GetMaxChannNum(BYTE &byChnnlNum)
{
	CString strLogoInfo;
	HRESULT hr = FALSE;
	BYTE bMaxChannelNum;
	int nFeedbackLen;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_MAX_CHANNEL_NUM;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c", OBJ_ID_GET_MAX_CHANNEL_NUM, nDataLen);

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_GET_MAX_CHANNEL_NUM;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}

		bMaxChannelNum = m_pszCmdBuf[2];
		
#ifdef _WSS_50GHZ
		if(bMaxChannelNum>96 || bMaxChannelNum<1)
#else
		if(bMaxChannelNum>96 || bMaxChannelNum<1)
#endif
		{
			MessageBox(NULL,"Max channel number is wrong!","Error",MB_OK|MB_ICONERROR);
			return hr;
		}
		
		// process response
		byChnnlNum = bMaxChannelNum;
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	return hr;
}











BOOL CProcessMsg::SetAllSwitchDac(WORD wDacVal)
{
	int nRet = TRUE;

	int tempDACIndex;
	for (int i = 0; i < 96; i++)
	{
		if (i <97)
		{
			// 1 ~ 5 & 49 ~53
			if(i < 11)  
			{
				tempDACIndex = 62 + i;
			}
			// 6 ~ 11 & 54 ~59
			else if(i < 23)
			{
				tempDACIndex = 73 - i;
			}
			// 23 ~ 24
			else if(i < 25) 
			{
				tempDACIndex = 50 + i;
			}
			// 13 ~ 17 & 61 ~ 65
			else if(i < 35)
			{
				tempDACIndex = 62 + i;
			}
			// 18 ~ 23 & 66 ~ 71
			else if(i < 47)
			{
				tempDACIndex = 121 - i;
			}
			// 24 & 72
			else if(i < 49)
			{
				tempDACIndex = i + 2;
			}
			// 25 & 73
			else if(i < 51) 
			{
				tempDACIndex = i - 2;
			}
			// 26 ~ 31 & 74 ~ 79
			else if(i < 63)
			{
				tempDACIndex = 73 - i;
			}		
			// 32 ~ 36 & 80 ~ 84
			else if(i < 73) 
			{
				tempDACIndex = i - 62;
			}		
			// 37 & 85
			else if(i < 75) 
			{
				tempDACIndex = i - 50;
			}		
			// 38 ~ 43 & 86 ~ 91
			else if(i < 87) 
			{
				tempDACIndex = 121 - i;
			}		
			else if(i < 97)
			{
				tempDACIndex = i - 62;
			}
		}
		else 
		{
			if(i < 107)  
			{
				tempDACIndex = 62 + i;
			}
			// 6 ~ 11 & 54 ~59
			else if(i < 119)
			{
				tempDACIndex = 265 - i;
			}
			// 23 ~ 24
			else if(i < 121) 
			{
				tempDACIndex = 50 + i;
			}
			// 13 ~ 17 & 61 ~ 65
			else if(i < 131)
			{
				tempDACIndex = 62 + i;
			}
			// 18 ~ 23 & 66 ~ 71
			else if(i < 143)
			{
				tempDACIndex = 313 - i;
			}
			// 24 & 72
			else if(i < 145)
			{
				tempDACIndex = i + 2;
			}
			// 25 & 73
			else if(i < 147) 
			{
				tempDACIndex = i - 2;
			}
			// 26 ~ 31 & 74 ~ 79
			else if(i < 159)
			{
				tempDACIndex = 265 - i;
			}		
			// 32 ~ 36 & 80 ~ 84
			else if(i < 169) 
			{
				tempDACIndex = i - 62;
			}		
			// 37 & 85
			else if(i < 171) 
			{
				tempDACIndex = i - 50;
			}		
			// 38 ~ 43 & 86 ~ 91
			else if(i < 183) 
			{
				tempDACIndex = 313 - i;
			}		
			else if(i < 193)
			{
				tempDACIndex = i - 62;
			}
		}
		
		nRet = nRet | SetOneDACValue(i, wDacVal);
	}

	return nRet;
}

BOOL CProcessMsg::SetAllAttenDac(WORD wDacVal)
{
	int nRet = TRUE;

	int tempDACIndex;
	for (int i = 96; i < 192; i++)
	{
		if (i <97)
		{
			// 1 ~ 5 & 49 ~53
			if(i < 11)  
			{
				tempDACIndex = 62 + i;
			}
			// 6 ~ 11 & 54 ~59
			else if(i < 23)
			{
				tempDACIndex = 73 - i;
			}
			// 23 ~ 24
			else if(i < 25) 
			{
				tempDACIndex = 50 + i;
			}
			// 13 ~ 17 & 61 ~ 65
			else if(i < 35)
			{
				tempDACIndex = 62 + i;
			}
			// 18 ~ 23 & 66 ~ 71
			else if(i < 47)
			{
				tempDACIndex = 121 - i;
			}
			// 24 & 72
			else if(i < 49)
			{
				tempDACIndex = i + 2;
			}
			// 25 & 73
			else if(i < 51) 
			{
				tempDACIndex = i - 2;
			}
			// 26 ~ 31 & 74 ~ 79
			else if(i < 63)
			{
				tempDACIndex = 73 - i;
			}		
			// 32 ~ 36 & 80 ~ 84
			else if(i < 73) 
			{
				tempDACIndex = i - 62;
			}		
			// 37 & 85
			else if(i < 75) 
			{
				tempDACIndex = i - 50;
			}		
			// 38 ~ 43 & 86 ~ 91
			else if(i < 87) 
			{
				tempDACIndex = 121 - i;
			}		
			else if(i < 97)
			{
				tempDACIndex = i - 62;
			}
		}
		else 
		{
			if(i < 107)  
			{
				tempDACIndex = 62 + i;
			}
			// 6 ~ 11 & 54 ~59
			else if(i < 119)
			{
				tempDACIndex = 265 - i;
			}
			// 23 ~ 24
			else if(i < 121) 
			{
				tempDACIndex = 50 + i;
			}
			// 13 ~ 17 & 61 ~ 65
			else if(i < 131)
			{
				tempDACIndex = 62 + i;
			}
			// 18 ~ 23 & 66 ~ 71
			else if(i < 143)
			{
				tempDACIndex = 313 - i;
			}
			// 24 & 72
			else if(i < 145)
			{
				tempDACIndex = i + 2;
			}
			// 25 & 73
			else if(i < 147) 
			{
				tempDACIndex = i - 2;
			}
			// 26 ~ 31 & 74 ~ 79
			else if(i < 159)
			{
				tempDACIndex = 265 - i;
			}		
			// 32 ~ 36 & 80 ~ 84
			else if(i < 169) 
			{
				tempDACIndex = i - 62;
			}		
			// 37 & 85
			else if(i < 171) 
			{
				tempDACIndex = i - 50;
			}		
			// 38 ~ 43 & 86 ~ 91
			else if(i < 183) 
			{
				tempDACIndex = 313 - i;
			}		
			else if(i < 193)
			{
				tempDACIndex = i - 62;
			}
		}
		
		nRet = nRet | SetOneDACValue(i, wDacVal);
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//add by xiaomingg 2011-07-19
//Set DAC for channel number more than 255
//////////////////////////////////////////////////////////////////////////

BOOL CProcessMsg::SetOneDACValue(WORD wChannel, USHORT usChannelDac)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

    if((0xFFFF ==  wChannel) ||
        ((usChannelDac < 0) || (usChannelDac > 8192)))
        return hr;
	
	usChannelDac += DAC_VALUE_OFFSET;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_SET_CHANNEL_DAC_MORE_THAN_255_NEW;
	int nCmdLen = nDataLen + 4;
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c%c%c",OBJ_ID_SET_DAC_CHANNEL, 
		nDataLen>>8, 
		(BYTE)nDataLen, 
		bDGEIndex,   //Add by wanxin
		(BYTE)(wChannel>>8),
		(BYTE)wChannel);

    m_pszCmdBuf[6] = (BYTE)(usChannelDac>>8);
    m_pszCmdBuf[7] = (BYTE)usChannelDac;


	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(20);
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//add by xiaomingg 2011-07-19
//Get DAC for channel number more than 255
//changed by luke.liu 2018.3.4
//////////////////////////////////////////////////////////////////////////

BOOL CProcessMsg::GetChannelDACValue(WORD wChannel, WORD &wReadDAC, CString &strRecvData)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	BYTE byBuf[2] = { 0 };

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = LEN_GET_CHANNEL_DAC_MORE_THAN_255_NEW;
	int nCmdLen = nDataLen + 4;
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c%c%c", OBJ_ID_GET_DAC_CHANNEL,
		(BYTE)(nDataLen >> 8),
		(BYTE)nDataLen,
		bDGEIndex,
		(BYTE)(wChannel >> 8),
		(BYTE)wChannel);

	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(20);
		// get response
		nDataLen = LEN_BACK_DAC_CHANNEL;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		// process response
		//2 by 2018.3.4
		byBuf[0] = m_pszCmdBuf[4]; //3
		byBuf[1] = m_pszCmdBuf[3];


		CString strLogoInfo;
		USHORT usDACValue;
		usDACValue = *(USHORT*)&byBuf; //2

		wReadDAC = (WORD)usDACValue;
		strLogoInfo.Format("Read DAC value of Pixel=%d is: %d", wChannel, usDACValue);
		strRecvData = strLogoInfo;
		hr = TRUE;
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
}

BOOL CProcessMsg::GetParkingPort(WORD *pwParkingPort)
{
	int nFeedbackLen;
	HRESULT hr=FALSE;
	// construct msg
	
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 3; //code + checksum + len_value
	
	sprintf(m_pszCmdBuf, "%c%c", OBJ_ID_GET_PARKING_PORT, nDataLen);
	
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen -1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_PARKING_PROT;
		nFeedbackLen = 3 + nDataLen;
		
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		if(m_pszCmdBuf[0] != 0x00)
		{
			throw GetLastError();	
		}
		else
			*pwParkingPort = m_pszCmdBuf[2];
	
		hr=TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//::MessageBox(NULL,pszErrorMsg, "Error", MB_OK|MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//::MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}	
	
	return hr;
}

BOOL CProcessMsg::SetParkingPort(WORD wParkingPort)
{
    HRESULT hr = FALSE;
    
    int nFeedbackLen;
    // construct msg
    
    memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
    int nDataLen = LEN_PARKING_PROT;
    int nCmdLen = nDataLen + 3; //code + checksum + len_value
    sprintf(m_pszCmdBuf, "%c%c%c", OBJ_ID_SET_PARKING_PORT, nDataLen, wParkingPort);
    
    try
    {
        // send msg
        if(fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
        {
            throw GetLastError();
        }
        
        // get response
        nDataLen = LEN_BACK_NORMAL_SETTING;
        nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
        memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
        if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
        {
            throw GetLastError();		
        }

        if(m_pszCmdBuf != 0x00)
	        hr = FALSE;
		else
			hr = TRUE;

        // process response
    }
    catch (char* pszErrorMsg) 
    {
       // MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
    }
    
    catch (...) 
    {
       // MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
    }
    return hr;
}

BOOL CProcessMsg::SetSpectrumAttenuation(byte bLCIndex, WORD wChannelCount, int *pnDacArray)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = wChannelCount*2+1;
	int nCmdLen = nDataLen + 3;
	sprintf(m_pszCmdBuf, "%c%c%c", OBJ_ID_SET_SPECTRUM_ATTENATION, 
		nDataLen, 
		bLCIndex
		);
	for (int i = 0; i < wChannelCount; i++)
	{
		m_pszCmdBuf[2*i+3] = HIBYTE(pnDacArray[i]+DAC_VALUE_OFFSET);
		m_pszCmdBuf[2*i+4] = LOBYTE(pnDacArray[i]+DAC_VALUE_OFFSET);
	}
	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		
		// get response
		nDataLen = LEN_BACK_NORMAL_SETTING;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		Sleep(200);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL,pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL,"Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	return hr;   
}

BOOL CProcessMsg::ReadOPMADC()
{
	return TRUE;
}

BOOL CProcessMsg::fnRunNewFw(CString &strRecvData)
{
	int nDataLen = 0;
	int nFeedbackLen = 0;

	char chRet[MAX_BUFFER_LEN] = "";  	
	memset(chRet, 0, MAX_BUFFER_LEN);
	
	chRet[0] = OBJ_ID_RUN_NEW_FIRMWARE;
	chRet[1] = 0x00;
	chRet[2] = 0x00;	
	
 	// send msg
	if(fnSendMsg(chRet, 3) != TRUE)
	{
		strRecvData = GetLastError();
		return FALSE;
	}
	
	Sleep(200);

	// get response
	nDataLen = 0;
	nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
	{
		throw GetLastError();		
	}

	strRecvData = "run new fw command sent out !";
	
	return TRUE;

} 

//////////////////////////////////////////////////////////////////////////
//
//Set attenuation for all channels
//////////////////////////////////////////////////////////////////////////

BOOL CProcessMsg::SetAllPixelAtten(float *pfAtten, int nChannels)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;

	if ((nChannels < 64) || (nChannels > 96))
		return hr;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = nChannels + 1;
	int nCmdLen = nDataLen + 4;
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_SET_ATTEN_ALL,
		(BYTE)(nDataLen >> 8),
		(BYTE)nDataLen,
		bDGEIndex); //added by wanxin, add dge index


					//all channels
	for (int i = 0; i < nChannels; i++)
	{
		m_pszCmdBuf[i + 4] = (BYTE)(pfAtten[i] * 10);
	}


	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(500);
		// get response
		nDataLen = 0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
//
//Get attenuation of all pixels
//////////////////////////////////////////////////////////////////////////

BOOL CProcessMsg::GetAllPixelAtten(float *pfAtten, int *pnChannels)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	int nDataNum = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 0;
	int nCmdLen = nDataLen + 4; //by luke.liu code + checksum + len_value
	sprintf(m_pszCmdBuf, "%c%c%c",OBJ_ID_GET_ATTEN_ALL, nDataLen, nDataLen);

	try
	{
		// send msg
		if(fnSendMsg(m_pszCmdBuf, nCmdLen-1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(500);
		// get response
		nDataLen = LEN_GET_CHANNEL_ALL;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if(fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();		
		}
		
		// process response
		nDataNum = m_pszCmdBuf[1]*256+m_pszCmdBuf[2];
		if(nDataNum < 64)
		{
			m_strLastError.Format("Wrong attenuation numbers: %d",nDataNum);
			throw m_strLastError.GetBuffer(0);
		}
		//channels number
		*pnChannels = nDataNum;

		//payload array
		for(int i = 0; i < nDataNum; i++)
		{
            *pfAtten = (float)m_pszCmdBuf[i+3]/10;
		}
 
		hr = TRUE;
	}
	catch (char* pszErrorMsg) 
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}
	
	catch (...) 
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;	
	}

    return hr;

}

//////////////////////////////////////////////////////////////////////////
//add by luke.liu 2018.2.13
//Set DGE Flatness curve
//////////////////////////////////////////////////////////////////////////

BOOL CProcessMsg::fnSetOECurve(float *pfCurveFreq, float *pfCurveIL, WORD nPoint)
{
	HRESULT hr = FALSE;
	int nFeedbackLen = 0;
	WORD  wFreq = 0;
	WORD  wIL = 0;

	if ((nPoint < 2) || (nPoint > 4800))
		return hr;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = nPoint * 4 + 1; //freq_H, freq_L, IL_H, IL_L
	int nCmdLen = nDataLen + 4;
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_SET_FLATNESS_CURVE,
		(BYTE)(nDataLen >> 8),
		(BYTE)nDataLen,
		bDGEIndex); //added by luke.liu

	for (int i = 0; i < nPoint; i++)
	{
		wFreq = (WORD)(pfCurveFreq[i] - 180000); //Minus 180000
		wIL = (WORD)(pfCurveIL[i] * 100); //x100
		m_pszCmdBuf[4 + 4 * i] = (BYTE)((wFreq >> 8) & 0x00FF);
		m_pszCmdBuf[4 + 4 * i + 1] = (BYTE)(wFreq & 0x00FF);

		m_pszCmdBuf[4 + 4 * i + 2] = (BYTE)((wIL >> 8) & 0x00FF);
		m_pszCmdBuf[4 + 4 * i + 3] = (BYTE)(wIL & 0x00FF);
	}


	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(500);
		// get response
		nDataLen = 0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		// process response
		hr = TRUE;
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
//
//Get attenuation of all pixels
//////////////////////////////////////////////////////////////////////////

BOOL CProcessMsg::fnGetCurTemperIL(double *pdblIL,double* pdblFre, int *pnPoints)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	int nDataNum = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	WORD wpoint = 0;
	int nCmdLen = nDataLen + 4; //by luke.liu code + checksum + len_value
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_GET_CUR_TEMPER_IL, (BYTE)(nDataLen >> 8), (BYTE)nDataLen, bDGEIndex);

	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(500);
		// get response
		nDataLen = LEN_GET_IL_ALL;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		// process response
		BYTE_TO_WORD(wpoint, (BYTE)m_pszCmdBuf[1], (BYTE)m_pszCmdBuf[2]);
		nDataNum = wpoint;

		//channels number
		*pnPoints = nDataNum / 4;

		//payload array
		for (int i = 0; i < nDataNum / 4; i++)
		{
			//dblTemp = m_pszCmdBuf[3+2*i]*256+m_pszCmdBuf[3+2*i+1];
			BYTE_TO_WORD((WORD)wpoint, (BYTE)m_pszCmdBuf[3 + 4 * i], (BYTE)m_pszCmdBuf[3 + 4 * i + 1]);
			pdblFre[i] = (double)wpoint;
			BYTE_TO_WORD((WORD)wpoint, (BYTE)m_pszCmdBuf[3 + 4 * i + 2], (BYTE)m_pszCmdBuf[3 + 4 * i + 3]);
			pdblIL[i] = (double)wpoint / 100.0;
		}

		hr = TRUE;
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;

}

//////////////////////////////////////////////////////////////////////////
//
//Get attenuation of all pixels
//////////////////////////////////////////////////////////////////////////
BOOL CProcessMsg::fnGetCurTemperFreq(float *pfWaveFreq, int *pnPoints)
{
	HRESULT hr = FALSE;
	int nFeedbackLen;
	int nDataNum = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 1;
	WORD wpoint = 0;
	BYTE bBuf[4] = { 0 };
	float    *pfFreq = NULL;

	int nCmdLen = nDataLen + 4; //by luke.liu code + checksum + len_value
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_GET_CUR_TEMPER_FREQ, (BYTE)(nDataLen >> 8), (BYTE)nDataLen, bDGEIndex);

	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(500);
		// get response
		nDataLen = LEN_GET_Freq_ALL * 4;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		// process response
		BYTE_TO_WORD(wpoint, (BYTE)m_pszCmdBuf[1], (BYTE)m_pszCmdBuf[2]);
		nDataNum = wpoint;

		//channels number
		*pnPoints = nDataNum / 4;

		//payload array
		for (int i = 0; i < nDataNum / 4; i++)
		{
			bBuf[3] = m_pszCmdBuf[4 * i + 3];
			bBuf[2] = m_pszCmdBuf[4 * i + 4];
			bBuf[1] = m_pszCmdBuf[4 * i + 5];
			bBuf[0] = m_pszCmdBuf[4 * i + 6];

			pfFreq = (float*)&bBuf;
			pfWaveFreq[i] = pfFreq[0];
		}

		hr = TRUE;
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		hr = FALSE;
	}

	return hr;
}
///////////////////////////////////////////////////////////////
//函数说明：
//   设置DGE Index
//Add by wanxin
//Time：20200417
//////////////////////////////////////////////////////////////
void CProcessMsg::SetDGEIndex(int nDGEIndex)
{
	m_nDGEIndex = nDGEIndex;
}	
///////////////////////////////////////////////////////////////
//函数说明：
//   用来开始下表前选择是给哪个DGE模拟板下载数据
//Add by wanxin
//Time:20200417
///////////////////////////////////////////////////////////////
BOOL CProcessMsg::fnSelectTableLoading()
{
	BOOL  bResult = TRUE;
	int nFeedbackLen;
	int nDataNum = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 3;
	WORD wpoint = 0;
	BYTE bBuf[4] = { 0 };

	int nCmdLen = nDataLen + 4; //by luke.liu code + checksum + len_value
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c%c", OBJ_ID_START_TABLE_LOADING, (nDataLen>>8)&0xff, nDataLen&0xff, bDGEIndex);
	m_pszCmdBuf[4] = (char)0xA5;
	m_pszCmdBuf[5] = (char)0xC3;

	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(100);
		// get response
		nDataLen = 0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		if (!fnParseCommandByteForDGE(m_pszCmdBuf[0]))
		{
			throw GetLastError();
		}
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		bResult = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		bResult = FALSE;
	}

	return bResult;

}
////////////////////////////////////////////////////////
//function descripe
//      Start Firmware loading 
//Add by wanxin
//Time:20200428
////////////////////////////////////////////////////////
BOOL CProcessMsg::fnStartFirmwareLoading()
{
	BOOL  bResult = TRUE;
	int nFeedbackLen;
	int nDataNum = 0;

	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	int nDataLen = 2;
	WORD wpoint = 0;
	BYTE bBuf[4] = { 0 };

	int nCmdLen = nDataLen + 3;
	BYTE bDGEIndex = (BYTE)m_nDGEIndex;
	sprintf(m_pszCmdBuf, "%c%c%c", OBJ_ID_START_FIRMWARE_UPDATE, (nDataLen >> 8) & 0xff, nDataLen & 0xff);
	m_pszCmdBuf[3] = (char)0xA5;
	m_pszCmdBuf[4] = (char)0xC3;

	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(100);

		// get response
		nDataLen = 0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		if (!fnParseCommandByteForDGE(m_pszCmdBuf[0]))
		{
			throw GetLastError();
		}
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		bResult = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		bResult = FALSE;
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////
//函数说明：
//   对DGE返回的status进行解析
//Add by wanxin
//Time:20200417
////////////////////////////////////////////////////////////////
BOOL CProcessMsg::fnParseCommandByteForDGE(BYTE chStatus)
{
	if (RES_SUCCESS == chStatus)
	{
		return TRUE;
	}
	else
	{
		if (chStatus & 1)
		{
			m_strLastError = _T("Error: Checksum error !");;
		}
		else if (chStatus & 2)
		{
			m_strLastError = _T("Error: Syntax Error!");
		}
		else if (chStatus & 4)
		{
			m_strLastError = _T("Error: Invalid Data error !");
		}
		else if (chStatus & 8)
		{
			m_strLastError = _T("Error: HW error !");
		}

		return FALSE;
	}

}

BOOL CProcessMsg::fnFirmwareDownload(char *pszBuffer, int nLength)
{

	BOOL  bResult = TRUE;
	int nFeedbackLen;
	int nDataNum = 0;
	int nDataLen;
	int nCmdLen;
	// construct msg
	memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
	
	WORD wpoint = 0;
	BYTE bBuf[4] = { 0 };

	nDataLen = nLength;
	nCmdLen = EVERY_UPDATE_DATA_LEN + 4;  // every times should be EVERY_UPDATE_DATA_LEN
										  // Format: 0x54 0x10 D1-16 CC
	sprintf(m_pszCmdBuf, "%c%c%c", OBJ_ID_LOAD_NEW_FIRMWARE, 0xff &(EVERY_UPDATE_DATA_LEN >> 8), EVERY_UPDATE_DATA_LEN & 0xFF);
	memcpy(&m_pszCmdBuf[3], pszBuffer, nLength);


	try
	{
		// send msg
		if (fnSendMsg(m_pszCmdBuf, nCmdLen - 1) != TRUE)
		{
			throw GetLastError();
		}
		Sleep(50);
		// get response
		nDataLen = 0;
		nFeedbackLen = FEED_HEAD_LENGTH + nDataLen;
		memset(m_pszCmdBuf, 0, MAX_BUFFER_LEN);
		if (fnGetMsg(m_pszCmdBuf, nFeedbackLen) != TRUE)
		{
			throw GetLastError();
		}

		if (!fnParseCommandByteForDGE(m_pszCmdBuf[0]))
		{
			throw GetLastError();
		}
	}
	catch (char* pszErrorMsg)
	{
		//MessageBox(NULL, pszErrorMsg, "Error", MB_OK | MB_ICONERROR);
		bResult = FALSE;
	}

	catch (...)
	{
		//MessageBox(NULL, "Unhandled exception", "Fatal Error", MB_OK | MB_ICONERROR);
		bResult = FALSE;
	}

	return bResult;




}