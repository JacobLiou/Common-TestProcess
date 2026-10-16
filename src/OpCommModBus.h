// OpCommModBus.h: interface for the COpComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCOMMMODBUS_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
#define AFX_OPCOMMMODBUS_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	HAS_GPIB			0

// define ModBus special
#define MODBUS_READ_CMD		0x03
#define MODBUS_WRITE_CMD	0x06

#define MODBUS_READ_LENGTH  8   // read one word alway 8 bytes                                 
								// nn | nn | nn | nn | nn | nn | nn | nn 
								// жд   жд   жд   жд   жд   жд   жд   жд    
								// controller address (one byte)
								// command  read = 03
								// address (two byte)
								// want read data lengh (two byte)
								// CRC low byte   //  definitely
								// CRC high byte  //  definitely

#define MAX_BUF_DATA		64
#define ILLEGAL_COMMAND			0x01
#define ILLEGAL_DATA_ADDRESS    0x02
#define ILLEGAL_DATA_VALUE      0x03
 


//#define F4_DATA_OFFSET  40001		// F4 MODBUS address communication offset
//#define F4_PV			40101		// F4 Input Reading MODBUS address
#define F4_SETPOINT		300			// F4 Set Point MODBUS address

#define F4_POWERON		2072		//2072 Power On 
#define F4_POWEROFF		2073		//2073 Power Off
 



class COpCommModBus  
{
public:
	BOOL SetZHChamberRunStop();
	BOOL SetZHChamberRunStart();
	BOOL SetZHChamberSetPoint(double dbSetPoint);
	BOOL GetZHChamberSetPoint(double& dbSetPoint);

	BOOL GetZHChamberCurrentTemp(double& dbCurrTemp);

	BOOL OpenChamber(BYTE byCommunicationType, WORD wAddress);
	BOOL GetGpibChamberCurrentTemp(double& dbCurrTemp);
	BOOL GetGpibChamberSetPoint(double& dbSetPoint);
	BOOL SetGpibChamberPoint(double dbSetPoint);
	BOOL OpenGpibChamber(int nGpibAddress);

	BOOL  fnModBusReadCurrentPoint(double &dblCurrentPoint);
	
	COpCommModBus();
	virtual ~COpCommModBus();
	
	BOOL  OpenPort(LPTSTR strPortName, 
				   DWORD  dwBaudRate = 9600, 
				   BYTE   bByteSize = 8, 
				   BYTE   bParity = NOPARITY, 
				   BYTE   bStopBits = ONESTOPBIT);
	VOID  ClosePort();
	BOOL  ReadBuffer(char* pInBuffer, DWORD dwBufferSize);
	BOOL  WriteBuffer(char* pOutBuffer, DWORD dwBufferSize);
	VOID  PostTxDisplayMsg(char* pszMsg,DWORD dwLength);
	VOID  PostRxDisplayMsg(char* pszMsg,DWORD dwLength);
	int fnHexStrToInt(CString CTem);


	// modbus RTU special 
	int m_nModBusAddr;
	
	unsigned short  fnModBusCRC16(BYTE* pbyData, BYTE byCmdLength);
	BOOL			fnCheckModBusResponse(char* pInBuffer);

	BOOL			fnModBusReadSetPoint(double &dblSetPoint);
	BOOL			fnModBusSetSetPoint(double dblSetPoint); 
	BOOL	fnModBusPowerOn_Compressor(void);
	BOOL	fnModBusPowerOff_Compressor(void);

	
	HANDLE m_hCommPort_ModBus;

	int			m_nTemperatureChamberUD;
	BOOL		m_bRs232OrGPIB;

	BOOL	m_bZHChamberRun;


public:

	BYTE			m_byChamberType;//1,GPIB chamber  2,HQ COM chamber 3,ZH COM chamber //add by yu 20120704
	

};

#endif // !defined(AFX_OPCOMMMODBUS_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
