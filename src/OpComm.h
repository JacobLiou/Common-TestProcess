// Copyright @2005, Oplink Communication,inc
// OpComm.h: interface for the COpComm class.
// Revision 1.0 05-17-2005
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
#define AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COpComm  
{
public:
	// Read the data from the receive buffer of the COM port 
	BOOL ReadBuffer( char* pInBuffer, DWORD dwBufferSize,BOOL bReadOnlyOnce = FALSE);
	
	// write the data to the transmit buffer of the COM port
	BOOL WriteBuffer( char* pOutBuffer, DWORD dwBufferSize);

	// Close the COM port
	VOID ClosePort();

	// Configure the COM Port upon the input parameter,
	// Open the COM Port.
	BOOL OpenPort(LPTSTR strPortName,           // COM port index
		          DWORD dwBaudRate = 115200,    // Baud Rate:115200
				  BYTE bByteSize = 8,           // 8 data bits
				  BYTE bParity =  NOPARITY,     // no parity
				  BYTE bStopBits = ONESTOPBIT); // one stop bit
	COpComm();

	virtual ~COpComm();

	HANDLE	m_hCommPort; // COM port handler
	CRITICAL_SECTION    m_taskcs;

protected:
	

};

#endif // !defined(AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
