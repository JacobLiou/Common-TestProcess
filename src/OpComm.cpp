// Copyright @2005, Oplink Communication,inc
// OpComm.cpp: implementation of the COpComm class.
// Revision 1.0 05-17-2005
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "..\\include\\Extend\\OpComm.h"
#include "OpComm.h"

#define COM_READINTERVALTIMEOUT				500
#define COM_READTOTALTIMEOUTCONSTANT		500
#define COM_READTOTALTIMEOUTMULTIPLIER		500
#define COM_WRITETOTALTIMEOUTCONSTANT		500
#define COM_WRITETOTALTIMEOUTMULTIPLIER		500


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpComm::COpComm()
{
	m_hCommPort = INVALID_HANDLE_VALUE;
	//InitializeCriticalSection(&m_taskcs);
}

COpComm::~COpComm()
{
	ClosePort();
	//DeleteCriticalSection(&m_taskcs);
}

BOOL COpComm::OpenPort(LPTSTR strPortName, DWORD dwBaudRate, 
					   BYTE bByteSize, BYTE bParity, BYTE bStopBit)
{
	BOOL			bPortReady = FALSE;
	DCB				PortDCB;
	COMMTIMEOUTS	PortTimeOuts;

	

	//	If the port was previously open, close it first
	ClosePort();

	//	Open the port and do new settings
	m_hCommPort = CreateFile(strPortName, 
							 GENERIC_READ | GENERIC_WRITE, 
							 0, 
							 NULL,
							 OPEN_EXISTING, 
							 0, 
							 NULL);
	if(m_hCommPort == INVALID_HANDLE_VALUE)
		return bPortReady;

	bPortReady = GetCommState(m_hCommPort, &PortDCB);
	if(!bPortReady)
		return bPortReady;

	PortDCB.BaudRate = dwBaudRate;
	PortDCB.ByteSize = bByteSize;
	PortDCB.fParity  = FALSE;   //Disable parity checking 
	PortDCB.StopBits = bStopBit;
	PortDCB.fAbortOnError = TRUE;

	bPortReady = SetCommState(m_hCommPort, &PortDCB);
	if(!bPortReady)
		return bPortReady;

	bPortReady = GetCommTimeouts(m_hCommPort, &PortTimeOuts);
	if(!bPortReady)
		return bPortReady;

	//PortTimeOuts.ReadIntervalTimeout = COM_READINTERVALTIMEOUT;
	//PortTimeOuts.ReadTotalTimeoutConstant = 100;
	//PortTimeOuts.ReadTotalTimeoutMultiplier = 100;
	//PortTimeOuts.WriteTotalTimeoutConstant = 2000;
	//PortTimeOuts.WriteTotalTimeoutMultiplier = 50;

	PortTimeOuts.ReadIntervalTimeout = 50;
	PortTimeOuts.ReadTotalTimeoutConstant = 50;
	PortTimeOuts.ReadTotalTimeoutMultiplier = 10;
	PortTimeOuts.WriteTotalTimeoutConstant = 50;
	PortTimeOuts.WriteTotalTimeoutMultiplier = 10;

	bPortReady = SetCommTimeouts(m_hCommPort, &PortTimeOuts);
	if(!bPortReady)
		return bPortReady;

	bPortReady = SetupComm(m_hCommPort, 2048, 2048);
	if(!bPortReady)
		return bPortReady;

	bPortReady = PurgeComm(m_hCommPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	DWORD dwErrors;								// 错误信息
	COMSTAT Rcs;								// COMSTAT结构通信设备的当前信息
	int Len = 0;
	ClearCommError(m_hCommPort,&dwErrors,&Rcs);

	return bPortReady;
}

VOID COpComm::ClosePort()
{
	if(m_hCommPort != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCommPort);
		m_hCommPort = INVALID_HANDLE_VALUE;
	}
}

BOOL COpComm::WriteBuffer( char* pOutBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesWritten;

	if(m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;
	// Clear the TX and RX buffer Error information
	//if (!PurgeComm(m_hCommPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR))
	//{
	//	return FALSE;
	//}

	WriteFile(m_hCommPort, pOutBuffer, dwBufferSize, &dwBytesWritten, NULL);
	if (dwBytesWritten != dwBufferSize)
	{
		return FALSE;
	}	

	return TRUE;
}

BOOL COpComm::ReadBuffer(char* pInBuffer, DWORD dwBufferSize,BOOL bReadOnlyOnce)
{
	DWORD dwBytesRead;
	WORD  wDataLength;
	DWORD dwBytePoint = 0;
	BOOL  bReadAdd = FALSE;

	if(m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;
	BYTE *pbyBuffer = new BYTE[dwBufferSize];
	if (bReadOnlyOnce)
	{
		if (!ReadFile(m_hCommPort, pbyBuffer, dwBufferSize, &dwBytesRead, NULL))
		{
			delete[] pbyBuffer;
			return FALSE;
		}

		if (dwBytesRead <= dwBufferSize && dwBytesRead != 0)
		{
			memcpy(pInBuffer, pbyBuffer, dwBytesRead);
			delete[] pbyBuffer;
			return TRUE;
		}
		else
		{
			delete[] pbyBuffer;
			return FALSE;
		}
	}
	
    if(dwBufferSize < 256)
	{
		if(!ReadFile(m_hCommPort, pbyBuffer, dwBufferSize, &dwBytesRead, NULL))
		{
			delete [] pbyBuffer;
			return FALSE;
		}
		
		if(dwBytesRead <=dwBufferSize && dwBytesRead !=0)
		{
			memcpy(pInBuffer,pbyBuffer,dwBytesRead);
			delete [] pbyBuffer;
			return TRUE;
		}
        else
		{
			delete [] pbyBuffer;
			return FALSE;
		}
	}
    else
	{
		int nCount = 25;
		while (nCount--)
		{
			ReadFile(m_hCommPort, pbyBuffer, dwBufferSize, &dwBytesRead, NULL);
			
			// if the received buffer is empty, Read again 
			if(dwBytesRead == 0)
			{
				//BOOL bPortReady = PurgeComm(m_hCommPort, PURGE_RXCLEAR);
				if(!bReadAdd)
					Sleep(20);
				else 
				{
					bReadAdd = FALSE;
					delete pbyBuffer;
					return TRUE;
				}			
			}
			
			// if the received buffer < dwBufferSize
			else if (dwBytesRead < dwBufferSize)
			{ 
				wDataLength = (WORD)((pbyBuffer[1]<<8) + pbyBuffer[2]);
				if ((wDataLength == dwBytesRead-3) && (0 == dwBytePoint))
				{
					// finished read
					memcpy(pInBuffer,pbyBuffer,dwBytesRead);
					delete pbyBuffer;
					return TRUE;
				}
				
				// if should read more 
				memcpy(pInBuffer+dwBytePoint,pbyBuffer,dwBytesRead);
				dwBytePoint += dwBytesRead;
				
				if (dwBytePoint>= dwBufferSize)
				{
					// finished read
					delete pbyBuffer;
					return TRUE;
				}
				else
				{
					// read again
					bReadAdd = TRUE;
				} 
				
			}
			// if the received buffer =  dwBufferSize
			else if(dwBytesRead == dwBufferSize)
			{
				memcpy(pInBuffer,pbyBuffer,dwBufferSize);
				delete pbyBuffer;
				return TRUE;
			}	
		}	
	}

	delete pbyBuffer;
	return FALSE;
}

