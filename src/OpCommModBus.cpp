// OpCommModBus.cpp: implementation of the COpComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpCommModBus.h"


#if	HAS_GPIB
#include <gpib.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//extern HANDLE m_hCommPort_ModBus;

#define  WM_DISPLAY_TX_MSG                        WM_USER+108
#define  WM_DISPLAY_RX_MSG                        WM_USER+109

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpCommModBus::COpCommModBus()
{
	//g_hCommPort_ModBus = INVALID_HANDLE_VALUE;
	m_hCommPort_ModBus = INVALID_HANDLE_VALUE;
	m_nModBusAddr = 1;
	m_nTemperatureChamberUD = 0;
	m_bRs232OrGPIB = TRUE;

	m_byChamberType = 0;
	m_bZHChamberRun = FALSE;
}

COpCommModBus::~COpCommModBus()
{
	ClosePort();
}

BOOL COpCommModBus::OpenPort(LPTSTR strPortName, DWORD dwBaudRate, 
					   BYTE bByteSize, BYTE bParity, BYTE bStopBit)
{
	BOOL			bPortReady = FALSE;
	DCB				PortDCB;
	COMMTIMEOUTS	PortTimeOuts;

	//	If the port was previously open, close it first
	ClosePort();

	//	Open the port and do new settings
	m_hCommPort_ModBus = CreateFile(strPortName, 
							 GENERIC_READ | GENERIC_WRITE, 
							 0, 
							 NULL,
							 CREATE_ALWAYS, //OPEN_EXISTING,
							 0, 
							 NULL);
	if(m_hCommPort_ModBus == INVALID_HANDLE_VALUE)
		return bPortReady;

	bPortReady = GetCommState(m_hCommPort_ModBus, &PortDCB);
	if(!bPortReady)
		return bPortReady;

	PortDCB.BaudRate = dwBaudRate;
	PortDCB.ByteSize = bByteSize;
	PortDCB.Parity = bParity;
	PortDCB.StopBits = bStopBit;
	PortDCB.fAbortOnError = TRUE;

	bPortReady = SetCommState(m_hCommPort_ModBus, &PortDCB);
	if(!bPortReady)
		return bPortReady;

	bPortReady = GetCommTimeouts(m_hCommPort_ModBus, &PortTimeOuts);
	if(!bPortReady)
		return bPortReady;

	PortTimeOuts.ReadIntervalTimeout = 50;
	PortTimeOuts.ReadTotalTimeoutConstant = 50;
	PortTimeOuts.ReadTotalTimeoutMultiplier = 10;
	PortTimeOuts.WriteTotalTimeoutConstant = 50;
	PortTimeOuts.WriteTotalTimeoutMultiplier = 10;

	bPortReady = SetCommTimeouts(m_hCommPort_ModBus, &PortTimeOuts);
	if(!bPortReady)
		return bPortReady;

	bPortReady = SetupComm(m_hCommPort_ModBus, 2048, 2048);
	if(!bPortReady)
		return bPortReady;

	bPortReady = PurgeComm(m_hCommPort_ModBus, PURGE_TXCLEAR | PURGE_RXCLEAR);
	m_bRs232OrGPIB = TRUE;
	return bPortReady;
}

VOID COpCommModBus::ClosePort()
{
	if(m_hCommPort_ModBus != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCommPort_ModBus);
		m_hCommPort_ModBus = INVALID_HANDLE_VALUE;
	}
}

BOOL COpCommModBus::WriteBuffer(char* pOutBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesWritten;

	if(m_hCommPort_ModBus == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!PurgeComm(m_hCommPort_ModBus, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		return FALSE;
	}

	//PostTxDisplayMsg(pOutBuffer,dwBufferSize);
	//SetFilePointer(m_hCommPort_ModBus,0,0,FILE_BEGIN);
	WriteFile(m_hCommPort_ModBus, pOutBuffer, dwBufferSize, &dwBytesWritten, NULL);
	if(dwBytesWritten != dwBufferSize)
	{
		return FALSE;
	}
	//先把写文件缓冲区的数据强制写入磁盘。
   // FlushFileBuffers(m_hCommPort_ModBus);

	return TRUE;
}

BOOL COpCommModBus::ReadBuffer(char* pInBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesRead;

	if(m_hCommPort_ModBus == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(m_hCommPort_ModBus, pInBuffer, dwBufferSize, &dwBytesRead, NULL);
	if(dwBytesRead == 0)
		return FALSE;

//	PostRxDisplayMsg(pInBuffer,dwBytesRead);

	return TRUE;
}

 

VOID  COpCommModBus::PostTxDisplayMsg(char* pszMsg,DWORD dwLength)
{
    HWND hWnd;

	hWnd = AfxGetMainWnd()->GetSafeHwnd();

	::SendMessage(hWnd, WM_DISPLAY_TX_MSG, (DWORD)pszMsg, dwLength);
}

VOID  COpCommModBus::PostRxDisplayMsg(char* pszMsg,DWORD dwLength)
{
    HWND hWnd;

	hWnd = AfxGetMainWnd()->GetSafeHwnd();

	::SendMessage(hWnd, WM_DISPLAY_RX_MSG, (DWORD)pszMsg, dwLength);
}

 


/************************************************************************/
/*                                                                      */
/************************************************************************/
unsigned short   COpCommModBus::fnModBusCRC16(BYTE* pbyData, BYTE byCmdLength)
{
	//unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
    //unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
    
	
	BYTE *buf = pbyData;			// point to buffer
	int n = byCmdLength;			// get the lengh to calculate
	

	unsigned short crc=0xffff; // crc= (uchCRCHi << 8 | uchCRCLo); 
	for(int i=0;i<n;i++) 
	{ 
		crc^=buf[i]; 
		for(int b=0;b<8;b++) 
		{ 
			bool f=crc&1; 
			crc>>=1; 
			if(f) 
			crc^=0xa001; // 多项式 
		} 
	} 
	return(crc); 	
	 
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL  COpCommModBus::fnModBusReadSetPoint(double &dblSetPoint)
{
	if(m_byChamberType == 2)
	{
		BYTE	byTxBuf[MAX_BUF_DATA];
		BYTE	byRxBuf[MAX_BUF_DATA];
		BYTE	byCmdLen;
		
		WORD    wSetPointAddr = F4_SETPOINT;
		WORD    wReadDataLengh = 0x0001;
		WORD    wCrc = 0xFFFF;
		
		byCmdLen = MODBUS_READ_LENGTH;
		
		
		ZeroMemory(byTxBuf, MAX_BUF_DATA);
		
		// read buffer
		// nn | nn | nn | nn | nn | nn | nn | nn 
		// Δ   Δ   Δ   Δ   Δ   Δ   Δ   Δ    
		// controller address (one byte)
		// command  read = 03
		// address (two byte)
		// want read data lengh (two byte)
		// CRC low byte   //  definitely
		// CRC high byte  //  definitely
		
		byTxBuf[0] = (BYTE)m_nModBusAddr;
		byTxBuf[1] = MODBUS_READ_CMD;
		byTxBuf[2] = (BYTE)(wSetPointAddr >> 8);
		byTxBuf[3] = (BYTE)wSetPointAddr;
		byTxBuf[4] = (BYTE)(wReadDataLengh>>8);
		byTxBuf[5] = (BYTE)wReadDataLengh;  
		
		
		wCrc = fnModBusCRC16(byTxBuf, byCmdLen-2);
		
		byTxBuf[6] = (BYTE)wCrc;		// first CRC low byte    
		byTxBuf[7] = (BYTE)(wCrc >> 8);  // CRC high byte
		
		if(!WriteBuffer((char*)byTxBuf,byCmdLen))
		{
//			MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
		byCmdLen = MODBUS_READ_LENGTH-4 +1 + wReadDataLengh+1;   // read back 2 byte in this case
		
		ZeroMemory(byRxBuf, MAX_BUF_DATA);
		
		Sleep(20);
		
		if(!ReadBuffer((char*)byRxBuf, byCmdLen))
		{
//			MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
		// check back error
		if (!fnCheckModBusResponse((char*)byRxBuf))
		{
			return FALSE;
		}	
		
		dblSetPoint = (short)((byRxBuf[3]<<8) + byRxBuf[3+1]);
		dblSetPoint /= 10;
	}
	else if(m_byChamberType == 1)
	{
		if(!GetGpibChamberSetPoint(dblSetPoint))
		{
			return FALSE;
		}
	}
	else if(m_byChamberType == 3)
	{
		Sleep(100);
		if(!GetZHChamberSetPoint(dblSetPoint))
		{
			return FALSE;
		}	
	}
	else 
	{
		return FALSE;
	}
	

	return TRUE;
}



// Error code or valid data may be contained in ModBusBag 
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL  COpCommModBus::fnCheckModBusResponse(char* pInBuffer)
{
	/*
	When a controller cannot process a command it returns an exception response and sets
	the high bit (0x80) of the command.
	0x01 illegal command
	0x02 illegal data address
	0x03 illegal data value
	Packet returned by controller:| nn | nn | nn | nn nn | |
	Δ Δ Δ Δ Δ
	controller address (one byte)
	command + 0x80
	exception code (0x01 or 0x02 or 0x03)
	CRC low byte
	CRC high byte
	*/

	CString strTemp; 

	if ((pInBuffer[1] & 0x80 )!=0)
	{	 
		switch(pInBuffer[2])
		{
		case ILLEGAL_COMMAND:
			strTemp.Format("illegal command\n");
			break;
		case ILLEGAL_DATA_ADDRESS:
			strTemp.Format("illegal data address\n");
			break;
		case ILLEGAL_DATA_VALUE:
			strTemp.Format("illegal data value\n");
			break;
		default:
			strTemp.Format("error \n");
			break;	 
		}

//		AfxMessageBox(strTemp);
		return FALSE;
	}

	return TRUE;
	
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL  COpCommModBus::fnModBusSetSetPoint(double dblSetPoint)
{
	if(m_byChamberType == 2)
	{
		BYTE	byTxBuf[MAX_BUF_DATA];
		BYTE	byRxBuf[MAX_BUF_DATA];
		BYTE	byCmdLen;
		
		WORD    wSetPointAddr = F4_SETPOINT;
		WORD    wWriteValue;
		
		
		wWriteValue = (short)(dblSetPoint*10);
		
		
		WORD    wCrc = 0xFFFF;
		
		byCmdLen = MODBUS_READ_LENGTH;
		
		
		ZeroMemory(byTxBuf, MAX_BUF_DATA);
		
		// write one word buffer
		// nn | nn | nn | nn | nn | nn | nn | nn 
		// Δ   Δ   Δ   Δ   Δ   Δ   Δ   Δ    
		// controller address (one byte)
		// command  read = 06
		// address (two byte)
		// want write data word (two byte)
		// CRC low byte   //  definitely
		// CRC high byte  //  definitely
		
		byTxBuf[0] = (BYTE)m_nModBusAddr;
		byTxBuf[1] = MODBUS_WRITE_CMD;
		byTxBuf[2] = (BYTE)(wSetPointAddr >> 8);
		byTxBuf[3] = (BYTE)wSetPointAddr;
		byTxBuf[4] = (BYTE)(wWriteValue>>8);
		byTxBuf[5] = (BYTE)wWriteValue;  
		
		
		wCrc = fnModBusCRC16(byTxBuf, byCmdLen-2);
		
		byTxBuf[6] = (BYTE)wCrc;        // CRC low byte 
		byTxBuf[7] = (BYTE)(wCrc >> 8); // CRC high byte
		
		if(!WriteBuffer((char*)byTxBuf,byCmdLen))
		{
//			MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
		byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
		// OK write reponse  = write buffer                         
		
		ZeroMemory(byRxBuf, MAX_BUF_DATA);
		
		Sleep(20);
		
		if(!ReadBuffer((char*)byRxBuf, byCmdLen))
		{
//			MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
		// check back error
		if (!fnCheckModBusResponse((char*)byRxBuf))
		{
			return FALSE;
		}
	}
	else if(m_byChamberType == 1)
	{
		if(!SetGpibChamberPoint(dblSetPoint))
		{
			return FALSE;
		}
	}
	else if(m_byChamberType == 3)
	{
		Sleep(100);
		if(!SetZHChamberSetPoint(dblSetPoint))
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	 
	return TRUE;
 
}


BOOL  COpCommModBus::fnModBusReadCurrentPoint(double &dblCurrentPoint)
{
	
	if(m_byChamberType == 2)
	{
		BYTE	byTxBuf[MAX_BUF_DATA];
		BYTE	byRxBuf[MAX_BUF_DATA];
		BYTE	byCmdLen;
		
		WORD    wSetPointAddr = 100;
		WORD    wReadDataLengh = 0x0001;
		WORD    wCrc = 0xFFFF;
		
		byCmdLen = MODBUS_READ_LENGTH;
		
		
		ZeroMemory(byTxBuf, MAX_BUF_DATA);
		
		// read buffer
		// nn | nn | nn | nn | nn | nn | nn | nn 
		// Δ   Δ   Δ   Δ   Δ   Δ   Δ   Δ    
		// controller address (one byte)
		// command  read = 03
		// address (two byte)
		// want read data lengh (two byte)
		// CRC low byte   //  definitely
		// CRC high byte  //  definitely
		
		byTxBuf[0] = (BYTE)m_nModBusAddr;
		byTxBuf[1] = MODBUS_READ_CMD;
		byTxBuf[2] = (BYTE)(wSetPointAddr >> 8);
		byTxBuf[3] = (BYTE)wSetPointAddr;
		byTxBuf[4] = (BYTE)(wReadDataLengh>>8);
		byTxBuf[5] = (BYTE)wReadDataLengh;  
		
		
		wCrc = fnModBusCRC16(byTxBuf, byCmdLen-2);
		
		byTxBuf[6] = (BYTE)wCrc;		// first CRC low byte    
		byTxBuf[7] = (BYTE)(wCrc >> 8);  // CRC high byte
		
		if(!WriteBuffer((char*)byTxBuf,byCmdLen))
		{
//			MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
		byCmdLen = MODBUS_READ_LENGTH-4 +1 + wReadDataLengh+1;   // read back 2 byte in this case
		
		ZeroMemory(byRxBuf, MAX_BUF_DATA);
		
		Sleep(20);
		
		if(!ReadBuffer((char*)byRxBuf, byCmdLen))
		{
//			MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
		// check back error
		if (!fnCheckModBusResponse((char*)byRxBuf))
		{
			return FALSE;
		}	
		
		dblCurrentPoint = (short)((byRxBuf[3]<<8) + byRxBuf[3+1]);
		dblCurrentPoint /= 10;
	}
	else if(m_byChamberType == 1)
	{
		if(!GetGpibChamberCurrentTemp(dblCurrentPoint))
		{
			return FALSE;
		}
	}
	else if(m_byChamberType == 3)
	{
		Sleep(100);
		if(!GetZHChamberCurrentTemp(dblCurrentPoint))
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	if(dblCurrentPoint < -10 || dblCurrentPoint > 100)
	{
		return FALSE;
	}
	return TRUE;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL  COpCommModBus::fnModBusPowerOn_Compressor(void) 
{
	
	BYTE	byTxBuf[MAX_BUF_DATA];
	BYTE	byRxBuf[MAX_BUF_DATA];
	BYTE	byCmdLen;

	WORD    wSetPointAddr = F4_POWERON;
	WORD    wWriteValue;

	 
	wWriteValue = 0xFF9c;  // power on  //-100 compressor on 
	//wWriteValue = 100;  // compressor off
	 
	WORD    wCrc = 0xFFFF;

	byCmdLen = MODBUS_READ_LENGTH;


	ZeroMemory(byTxBuf, MAX_BUF_DATA);

	// write one word buffer
    // nn | nn | nn | nn | nn | nn | nn | nn 
    // Δ   Δ   Δ   Δ   Δ   Δ   Δ   Δ    
	// controller address (one byte)
	// command  read = 06
	// address (two byte)
	// want write data word (two byte)
	// CRC low byte   //  definitely
	// CRC high byte  //  definitely

	byTxBuf[0] = (BYTE)m_nModBusAddr;
	byTxBuf[1] = MODBUS_WRITE_CMD;
	byTxBuf[2] = (BYTE)(wSetPointAddr >> 8);
	byTxBuf[3] = (BYTE)wSetPointAddr;
	byTxBuf[4] = (BYTE)(wWriteValue>>8);
	byTxBuf[5] = (BYTE)wWriteValue;  
 
 
	wCrc = fnModBusCRC16(byTxBuf, byCmdLen-2);

	byTxBuf[6] = (BYTE)wCrc;        // CRC low byte 
	byTxBuf[7] = (BYTE)(wCrc >> 8); // CRC high byte
	
	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
				
	ZeroMemory(byRxBuf, MAX_BUF_DATA);

	Sleep(20);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
//		MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	// check back error
	if (!fnCheckModBusResponse((char*)byRxBuf))
	{
		return FALSE;
	}
	
	
	
	
	return TRUE;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL  COpCommModBus::fnModBusPowerOff_Compressor(void) 
{
	
	BYTE	byTxBuf[MAX_BUF_DATA];
	BYTE	byRxBuf[MAX_BUF_DATA];
	BYTE	byCmdLen;

	WORD    wSetPointAddr = F4_POWEROFF;
	WORD    wWriteValue;

	 
	wWriteValue = 0x0064;  // power off
 
	 
	WORD    wCrc = 0xFFFF;

	byCmdLen = MODBUS_READ_LENGTH;


	ZeroMemory(byTxBuf, MAX_BUF_DATA);

	// write one word buffer
    // nn | nn | nn | nn | nn | nn | nn | nn 
    // Δ   Δ   Δ   Δ   Δ   Δ   Δ   Δ    
	// controller address (one byte)
	// command  read = 06
	// address (two byte)
	// want write data word (two byte)
	// CRC low byte   //  definitely
	// CRC high byte  //  definitely

	byTxBuf[0] = (BYTE)m_nModBusAddr;
	byTxBuf[1] = MODBUS_WRITE_CMD;
	byTxBuf[2] = (BYTE)(wSetPointAddr >> 8);
	byTxBuf[3] = (BYTE)wSetPointAddr;
	byTxBuf[4] = (BYTE)(wWriteValue>>8);
	byTxBuf[5] = (BYTE)wWriteValue;  
 
 
	wCrc = fnModBusCRC16(byTxBuf, byCmdLen-2);

	byTxBuf[6] = (BYTE)wCrc;        // CRC low byte 
	byTxBuf[7] = (BYTE)(wCrc >> 8); // CRC high byte
	
	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
				
	ZeroMemory(byRxBuf, MAX_BUF_DATA);

	Sleep(20);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
//		MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	// check back error
	if (!fnCheckModBusResponse((char*)byRxBuf))
	{
		return FALSE;
	}
	
	
	
	return TRUE;
}

BOOL COpCommModBus::OpenGpibChamber(int nGpibAddress)
{

#if HAS_GPIB

	m_nTemperatureChamberUD = ibdev(0,nGpibAddress,0,11,1,0);

	if(m_nTemperatureChamberUD == 0)
	{
		return FALSE;
	}
	m_bRs232OrGPIB = FALSE;
#endif

	return TRUE;
}

BOOL COpCommModBus::SetGpibChamberPoint(double dbSetPoint)
{

#if HAS_GPIB
	CString strSetString;
	int		nSetResult = 0;
	int		nSendStringCount = 0;
	if(m_nTemperatureChamberUD == 0)
	{
		return FALSE;
	}
	char chrReadBuffer[200];
	ZeroMemory(chrReadBuffer,200*sizeof(char));

	strSetString.Format("L1S%.1f;DEC.",dbSetPoint);
	nSendStringCount = strSetString.GetLength();
	if(nSendStringCount<9)
	{
		return FALSE;
	}
	if(!ibwrt(m_nTemperatureChamberUD,strSetString.GetBuffer(nSendStringCount),nSendStringCount))
	{
		return FALSE;
	}
	Sleep(200);
	ZeroMemory(chrReadBuffer,200*sizeof(char));
	if(!ibrd(m_nTemperatureChamberUD,(void*)chrReadBuffer,200))
	{
		return FALSE;
	}
	nSetResult = atoi(chrReadBuffer);
	if(nSetResult > 0)
	{
		return FALSE;
	}

	strSetString.Format("RM;DEC.");
	nSendStringCount = strSetString.GetLength();
	if(!ibwrt(m_nTemperatureChamberUD,strSetString.GetBuffer(0),nSendStringCount))
	{
		return FALSE;
	}
	ZeroMemory(chrReadBuffer,200*sizeof(char));
	if(!ibrd(m_nTemperatureChamberUD,(void*)chrReadBuffer,200))
	{
		return FALSE;
	}
	nSetResult = atoi(chrReadBuffer);

#endif

	return TRUE;

}

BOOL COpCommModBus::GetGpibChamberSetPoint(double &dbSetPoint)
{
#if HAS_GPIB
	if(m_nTemperatureChamberUD == 0)
	{
		return FALSE;
	}

	char chrReadBuffer[200];
	ZeroMemory(chrReadBuffer,200*sizeof(char));

	if(!ibwrt(m_nTemperatureChamberUD, "D1S.", 4 ))
	{
		return FALSE;
	}
	Sleep(200);
	if(!ibrd(m_nTemperatureChamberUD,(void*)chrReadBuffer,200))
	{
		return FALSE;
	}

	dbSetPoint = atof(chrReadBuffer);
#endif
	return TRUE;
}

BOOL COpCommModBus::GetGpibChamberCurrentTemp(double &dbCurrTemp)
{
#if HAS_GPIB

	if(m_nTemperatureChamberUD == 0)
	{
		return FALSE;
	}

	char chrReadBuffer[200];
	ZeroMemory(chrReadBuffer,200*sizeof(char));

	if(!ibwrt(m_nTemperatureChamberUD, "D1V.", 4 ))
	{
		return FALSE;
	}
	Sleep(200);
	ZeroMemory(chrReadBuffer,200*sizeof(char));
	if(!ibrd(m_nTemperatureChamberUD,(void*)chrReadBuffer,200))
	{
		return FALSE;
	}

	dbCurrTemp = atof(chrReadBuffer);
#endif

	return TRUE;
}

BOOL COpCommModBus::OpenChamber(BYTE byCommunicationType, WORD wAddress)
{
	CString strComPort;
	m_byChamberType = byCommunicationType;
	if (1 == byCommunicationType) //GPIB
	{
		if (!OpenGpibChamber(wAddress)) 
		{
			return FALSE;
		}
		m_bRs232OrGPIB = FALSE;
	}
	else if (2 == byCommunicationType)//Serial port
	{
		if (wAddress < 10&&wAddress > 0) 
		{
			strComPort.Format("COM%d", wAddress);
		}
		else if (wAddress > 10)
		{
			strComPort.Format("\\\\.\\COM%d", wAddress);
		}
		else
		{
			MessageBox(NULL, "Chamber Communication port configure faild!", "ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
			return FALSE;
		}
		if (!OpenPort((LPSTR)(LPCSTR)strComPort, 19200))
		{
			return FALSE;
		}

		m_bRs232OrGPIB = TRUE;
	}
	else if(3 == byCommunicationType) // ZH-CHAMBER
	{
		if (wAddress < 10&&wAddress > 0) 
		{
			strComPort.Format("COM%d", wAddress);
		}
		else if (wAddress > 10)
		{
			strComPort.Format("\\\\.\\COM%d", wAddress);
		}
		else
		{
			MessageBox(NULL, "Chamber Communication port configure faild!", "ERROR", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
			return FALSE;
		}
		if (!OpenPort((LPSTR)(LPCSTR)strComPort, 9600))
		{
			return FALSE;
		}

		m_bRs232OrGPIB = TRUE;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


int COpCommModBus::fnHexStrToInt(CString CTem)
{
	char*  pcIndex;
	int    iReturnValue = 0;
	DWORD  dwCstringLength;

	pcIndex = (LPSTR)(LPCTSTR)CTem;
	dwCstringLength = CTem.GetLength();

	while (dwCstringLength--)
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



//dblSetPoint = (short)((byRxBuf[3]<<8) + byRxBuf[3+1]);
//		dblSetPoint /= 10;
BOOL COpCommModBus::GetZHChamberCurrentTemp(double &dbCurrTemp)
{
	BYTE	byTxBuf[256];
	BYTE	byRxBuf[256];
	BYTE	byCmdLen;

	CString strWrite = ".01RRD,30,1,2,10,12,13,14,20,21,25,26,27,28,29,30,31,32,33,34,39,101,104,122,200,201,202,203,204,902,911,91299";

	ZeroMemory(byTxBuf, 256);
	LPTSTR lpstrWrite =	strWrite.GetBuffer(strWrite.GetLength());
	ASSERT(lpstrWrite != NULL);
	if(lpstrWrite == NULL)
	{
		return FALSE;
	}
	int nCmdLen = strWrite.GetLength();
	for (int i = 0 ;i<nCmdLen;i++)
	{
		if(i==0)
		{
			byTxBuf[i] = 0x02;
		}
		else
		{
			byTxBuf[i] = lpstrWrite[i];
		}
		
	}
	byTxBuf[i] = 0X0D;
	byTxBuf[i+1] = 0X0A;
	byCmdLen = nCmdLen+2;
	
	strWrite.ReleaseBuffer();

	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
//	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
	byCmdLen = 255;		
	ZeroMemory(byRxBuf, 256);

	Sleep(200);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
		return FALSE;
	}
	
	if(byRxBuf[7]!='O' && byRxBuf[8]!='K')
	{
		return FALSE;
	}
	char chtemp[4];
	ZeroMemory(chtemp,4*sizeof(char));
	chtemp[0] = byRxBuf[10];
	chtemp[1] = byRxBuf[11];
	chtemp[2] = byRxBuf[12];
	chtemp[3] = byRxBuf[13];
	
	short sTemp=0;
	sscanf(chtemp,"%x", &sTemp);
	dbCurrTemp = (double)sTemp /10.0;

#if _DEBUG
	memset((char*)&sTemp + sizeof(short), 0xCC, sizeof(short)); //恢复编译器插入的gap的值
#endif
	
	return TRUE;
}

BOOL COpCommModBus::GetZHChamberSetPoint(double &dbSetPoint)
{
	BYTE	byTxBuf[256];
	BYTE	byRxBuf[256];
	BYTE	byCmdLen;

	CString strWrite = ".01RRD,30,1,2,10,12,13,14,20,21,25,26,27,28,29,30,31,32,33,34,39,101,104,122,200,201,202,203,204,902,911,91299";

	ZeroMemory(byTxBuf, 256);
	LPTSTR lpstrWrite =	strWrite.GetBuffer(strWrite.GetLength());
	ASSERT(lpstrWrite != NULL);
	if(lpstrWrite == NULL)
	{
		return FALSE;
	}
	int nCmdLen = strWrite.GetLength();
	for (int i = 0 ;i<nCmdLen;i++)
	{
		if(i==0)
		{
			byTxBuf[i] = 0x02;
		}
		else
		{
			byTxBuf[i] = lpstrWrite[i];
		}
		
	}
	byTxBuf[i] = 0X0D;
	byTxBuf[i+1] = 0X0A;
	byCmdLen = nCmdLen+2;
	
	strWrite.ReleaseBuffer();

	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
//	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
	byCmdLen = 255;		
	ZeroMemory(byRxBuf, 256);

	Sleep(20);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
//		MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	
	if(byRxBuf[7]!='O' && byRxBuf[8]!='K')
	{
		return FALSE;
	}
	char chtemp[4];
	ZeroMemory(chtemp,4*sizeof(char));
	chtemp[0] = byRxBuf[15];
	chtemp[1] = byRxBuf[16];
	chtemp[2] = byRxBuf[17];
	chtemp[3] = byRxBuf[18];
//	int nCurrentTemp = 0;
	int  nTemp = 0;
	sscanf(chtemp,"%x",&nTemp);
//	sscanf(chtemp,"%x",&nCurrentTemp);
	dbSetPoint = (double)nTemp /10.0;

	if(dbSetPoint < -20 || dbSetPoint > 100)
	{
		return FALSE;
	}
	return TRUE;

}
BOOL COpCommModBus::SetZHChamberSetPoint(double dbSetPoint)
{
	BYTE	byTxBuf[MAX_BUF_DATA];
	BYTE	byRxBuf[MAX_BUF_DATA];
	BYTE	byCmdLen;
	WORD	wSetPointValue = 0;
	wSetPointValue = (WORD)dbSetPoint*10;
	CString strSetPoint;
	strSetPoint.Format("%04X",wSetPointValue);
	if(strSetPoint.GetLength() != 4)
	{
		return FALSE;
	}

	CString strWrite = ".01WSD,05,0200,0001,0190,FE3E,FE3E,FE3E";

	ZeroMemory(byTxBuf, MAX_BUF_DATA);
	LPTSTR lpstrWrite =	strWrite.GetBuffer(strWrite.GetLength());
	ASSERT(lpstrWrite != NULL);
	if(lpstrWrite == NULL)
	{
		return FALSE;
	}

	LPTSTR lpSetPoint = strSetPoint.GetBuffer(strSetPoint.GetLength());
	ASSERT(lpSetPoint != NULL);
	if(lpSetPoint == NULL)
	{
		return FALSE;
	}
	
	int nCmdLen = strWrite.GetLength();
	BYTE byCheckSum = 0;
	for (int i = 0 ;i<nCmdLen;i++)
	{
		if(i==0)
		{
			byTxBuf[i] = 0x02;
		}
		else if(i >19 && i<24)
		{
			byTxBuf[i] = lpSetPoint[i-20];
		}
		else
		{
			byTxBuf[i] = lpstrWrite[i];
		}
		if(i >0)
		{
			byCheckSum+=byTxBuf[i];
		}
		
	}
	strSetPoint.ReleaseBuffer();
	strWrite.ReleaseBuffer();

	strSetPoint.Format("%02X",byCheckSum);
	LPTSTR lpCheckSum = strSetPoint.GetBuffer(strSetPoint.GetLength());
	ASSERT(lpCheckSum != NULL);
	if(lpCheckSum == NULL)
	{
		return FALSE;
	}
	byTxBuf[i] = lpCheckSum[0];
	byTxBuf[i+1] = lpCheckSum[1];

	byTxBuf[i+2] = 0X0D;
	byTxBuf[i+3] = 0X0A;
	byCmdLen = nCmdLen+4;
	strSetPoint.ReleaseBuffer();

	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
//	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
	byCmdLen = 64;		
	ZeroMemory(byRxBuf, MAX_BUF_DATA);

	Sleep(100);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
//		MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	
	if(byRxBuf[7]!='O' && byRxBuf[8]!='K')
	{
		return FALSE;
	}
	if(!m_bZHChamberRun)
	{
		if(!SetZHChamberRunStart())
		{
			return FALSE;
		}
		else
		{
			m_bZHChamberRun = TRUE;	
		}
	}
//	if(!SetZHChamberRunStart())
//	{
//		return FALSE;
//	}
	return TRUE;
}

BOOL COpCommModBus::SetZHChamberRunStart()
{
		
	BYTE	byTxBuf[MAX_BUF_DATA];
	BYTE	byRxBuf[MAX_BUF_DATA];
	BYTE	byCmdLen;

	CString strWrite = ".01WRD,02,0104,0001,0101,000195";

	ZeroMemory(byTxBuf, MAX_BUF_DATA);
	LPTSTR lpstrWrite =	strWrite.GetBuffer(strWrite.GetLength());
	ASSERT(lpstrWrite != NULL);
	if(lpstrWrite == NULL)
	{
		return FALSE;
	}
	int nCmdLen = strWrite.GetLength();
	for (int i = 0 ;i<nCmdLen;i++)
	{
		if(i==0)
		{
			byTxBuf[i] = 0x02;
		}
		else
		{
			byTxBuf[i] = lpstrWrite[i];
		}
	}
	byTxBuf[i] = 0X0D;
	byTxBuf[i+1] = 0X0A;
	byCmdLen = nCmdLen+2;


	strWrite.ReleaseBuffer();

	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
//	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
	byCmdLen = 64;		
	ZeroMemory(byRxBuf, MAX_BUF_DATA);

	Sleep(20);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
//		MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	
	if(byRxBuf[7]!='O' && byRxBuf[8]!='K')
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL COpCommModBus::SetZHChamberRunStop()
{
	BYTE	byTxBuf[MAX_BUF_DATA];
	BYTE	byRxBuf[MAX_BUF_DATA];
	BYTE	byCmdLen;

	CString strWrite = ".01WRD,02,0104,0001,0101,000498";

	ZeroMemory(byTxBuf, MAX_BUF_DATA);
	LPTSTR lpstrWrite =	strWrite.GetBuffer(strWrite.GetLength());
	ASSERT(lpstrWrite != NULL);
	if(lpstrWrite == NULL)
	{
		return FALSE;
	}
	int nCmdLen = strWrite.GetLength();
	for (int i = 0 ;i<nCmdLen;i++)
	{
		if(i==0)
		{
			byTxBuf[i] = 0x02;
		}
		else
		{
			byTxBuf[i] = lpstrWrite[i];
		}
	}
	byTxBuf[i] = 0X0D;
	byTxBuf[i+1] = 0X0A;
	byCmdLen = nCmdLen+2;
	

	strWrite.ReleaseBuffer();

	if(!WriteBuffer((char*)byTxBuf,byCmdLen))
	{
//		MessageBox(NULL, "MODBUS send data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
//	byCmdLen = MODBUS_READ_LENGTH ;   // read back 2 byte in this case
	// OK write reponse  = write buffer                         
	byCmdLen = 64;		
	ZeroMemory(byRxBuf, MAX_BUF_DATA);

	Sleep(20);
	
	if(!ReadBuffer((char*)byRxBuf, byCmdLen))
	{
//		MessageBox(NULL,"MODBUS get data error!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	
	if(byRxBuf[7]!='O' && byRxBuf[8]!='K')
	{
		return FALSE;
	}
	
	return TRUE;
}
