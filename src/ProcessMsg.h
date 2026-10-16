// OpComm.h: interface for the COpComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSMSG_H__12345678ASDFG__INCLUDED_)
#define AFX_PROCESSMSG_H__12345678ASDFG__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "..\\include\\Extend\\OpComm.h"
#include "OpComm.h"
#include "Defines.h"

#include <cstring>


//******************************************************
#define MAX_BUF_DATA							    64
#define MSG_BASIC_LENGTH 				     		11
#define MSG_IDX_DATA								0x06
//=======================================================
#define OMD_READ									0xC1
#define OMD_WRITE									0xC2
#define CMD_READ                                    0x01
#define CMD_WRITE                                   0x02
//=======================================================

//=======================================================
// status signal const description
#define ST_SUCCESS									0x00
#define ST_IDLE							            0x01
#define ST_Message_Incomple							0x02
#define ST_MESSGAE_COMPLETE							0x03
#define ST_MESSAGE_VALIEL							0x04
#define ST_INVALID_LENGTH							0x05
#define ST_INVALID_ACCESS							0x06
#define ST_OBJECT_ID_OUTSIDE						0x07
#define ST_INVALID_DATA								0x08
#define ST_INVALID_CHECKSUM							0x09
#define ST_COMMAND_IN_EXECUTION						0x0A
#define ST_AUTONOMOUS_MESSAGE						0xFF
//=======================================================


#define EVERY_DATA_LEN                              (64)//- 传送数据时每次的长度
#define EVERY_UPDATE_DATA_LEN                       (1024)//(16)//- 升级时传送数据时每次的长度
#define FEED_HEAD_LENGTH                            3  
#define MAX_BUFFER_LEN                              2048
#define CHECK_SUM_LENGTH                            1

/**************************************************************************/
//ObjID
#define OBJ_ID_SERIAL_ID                        0x01		
#define OBJ_ID_MODULE_TYPE                      0x02
#define OBJ_ID_VER                              0x03
#define OBJ_ID_RS232_RATE                       0x04
#define OBJ_ID_HARDWARE_DATE                    0x05
#define OBJ_ID_FIRMWARE_DATE                    0x06
#define OBJ_ID_SET_SINGLE_ATTENATION            0x11
#define OBJ_ID_GET_SINGLE_ATTENATION            0x12
#define OBJ_ID_SET_ATTEN_ALL                    0x13
#define OBJ_ID_GET_ATTEN_ALL                    0x14
#define OBJ_ID_SET_FLATNESS_CURVE               0x15
#define OBJ_ID_GET_PROFILE                      0x16
#define OBJ_ID_SET_PROFILE_IN_USE               0x17
#define OBJ_ID_GET_PROFILE_IN_USE               0x18
#define OBJ_ID_GET_CUR_TEMPER_IL                0x19
#define OBJ_ID_GET_CUR_TEMPER_FREQ              0x1A
#define OBJ_ID_GET_ALARM                        0x21
#define OBJ_ID_GET_SUPPLY_VOLTAGE               0x22
#define OBJ_ID_GET_TEMPER                       0x23
#define OBJ_ID_SET_LOW_TEMPER                   0x31
#define OBJ_ID_GET_LOW_TEMPER                   0x32
#define OBJ_ID_SET_HIGH_TEMPER                  0x33
#define OBJ_ID_GET_HIGH_TEMPER                  0x34

#define OBJ_ID_START_FIRMWARE_UPDATE            0x41
#define OBJ_ID_RUN_NEW_FIRMWARE                 0x43
#define OBJ_ID_RESTART_SYSTEM                   0x44
#define OBJ_ID_LOAD_NEW_FIRMWARE                0x45  

#define OBJ_ID_SWITCH_PORT                      0x51
#define OBJ_ID_SET_CHANNEL_ATTENUATION          0x52
#define OBJ_ID_SET_CHANNEL_DELTA_ATTENUATION    0x53

#define OBJ_ID_SAVE_CONFIGURATION_TO_PROFILE1   0x55

#define OBJ_ID_SET_ALL_CHN_WAVEPLAN             0x60
#define OBJ_ID_GET_ALL_CHN_WAVEPLAN             0x61  // Get waveplan of all channels
#define OBJ_ID_SET_1_CHN_WAVEPLAN               0x62  // Set waveplan of one channel
#define OBJ_ID_GET_1_CHN_WAVEPLAN               0x63  // Get waveplan of one channel
#define OBJ_ID_SET_ALL_CHN_ATTN		            0x64  // Set attenuation of all channels
#define OBJ_ID_GET_ALL_CHN_ATTN                 0x65  // Get attenuation of all channels
#define OBJ_ID_SET_1_CHN_ATTN                   0x66  // Set attenuation of one channel
#define OBJ_ID_GET_1_CHN_ATTN                   0x67  // Get attenuation of one channel 
#define OBJ_ID_GET_Start_FREQ	                0x68  // Get starting frequency
#define OBJ_ID_SET_Hardware_reset	            0x69  // Set Hardware reset


//debug objID
#define OBJ_ID_SET_HALF_DAC_CHANNELS            0xfd 
#define OBJ_ID_GET_GRATING_TEMP                 0xe6	//to be deleted  
#define OBJ_ID_GET_MAX_CHANNEL_NUM              0xe7
#define OBJ_ID_SET_MAX_CHANNEL_NUM              0xe8
#define OBJ_ID_GET_SVN_VERSION                  0xe9	//to be deleted
#define OBJ_ID_SET_FLASH_DATA                   0xea
#define OBJ_ID_GET_FLASH_DATA                   0xeb

#define OBJ_ID_SET_HW_DATE_CODE                 0xed
#define OBJ_ID_SET_HW_VERSION                   0xee
#define OBJ_ID_SET_MODULE_TYPE                  0xef
#define OBJ_ID_SET_MODULE_SN                    0xf0
#define OBJ_ID_SET_DAC_CHANNELS                 0xf1
#define OBJ_ID_GET_ATD_DATA                     0xf2
#define OBJ_ID_GET_PARKING_PORT                 0xf6
#define OBJ_ID_SET_PARKING_PORT                 0xf7

#define OBJ_ID_SET_SPECTRUM_ATTENATION			0xfe

#define OBJ_ID_READ_TABLE_DATA                  0xf8

#define OBJ_ID_GET_DAC_CHANNEL                  0xfa
#define OBJ_ID_SET_DAC_CHANNEL                  0xfb
#define OBJ_ID_GET_LC_TEMPER                    0xfc

#define OBJ_ID_SET_TEMPER_RANGE                 0xe5	//to be deleted
#define OBJ_ID_GET_TEMPER_RANGE                 0xe4	//to be deleted
#define OBJ_ID_SET_HERTERS_TOTAL_POWERS         0xe3	//Set total power of heaters
#define OBJ_ID_GET_TARGET_TEMPER                0xe2
#define OBJ_ID_GET_HEATER_STATE                 0xe1
#define OBJ_ID_GET_HERTERS_TOTAL_POWERS         0xe0	//Set total power of heaters
 

#define OBJ_ID_SET_PID_PARA						0xDD
#define OBJ_ID_GET_PID_PARA						0xDC

#define OBJ_ID_GET_MONITOR_DATA                 0xD5
#define OBJ_ID_SET_SENSOR_PARA                  0xD4
#define OBJ_ID_GET_SENSOR_PARA                  0xD3

#define OBJ_ID_COPY_IMAGE_TO_OMAPL138			0xCF //  00 - "Copy image to OMAPl138",	
#define	OBJ_ID_LOAD_IMAGE_SEND_TO_ASIC			0xCE //  00 - " Load image send to ASIC",	
#define	OBJ_ID_SAVE_IMAGE_TO_FLASH				0xCD //  00 - "Save image file to flash",	
#define	OBJ_ID_QUERY_IMAGE_COPY_STATUS			0xCC //  00 - "Query image file copy status",	
#define	OBJ_ID_READ_DEVICE_REGISTER				0xCB //  00 - "Read Device register",	
#define	OBJ_ID_WRITE_DEVICE_REGISTER			0xCA //  00 - "Write Device register",	
//#define	OBJ_ID_READ_LCOS_VOLTAGE_ADC			0xC9 //  00 - "Read 100 LCOS voltage ADC values",	
#define OBJ_ID_START_TABLE_LOADING               0XC9
#define	OBJ_ID_SET_PIXEL_SAME_VALUE				0xC8 //  00 - "Set all pixel RGB grayscale the same value",
#define OBJ_ID_SET_SENSOR_THRESHOLD				0xC7 //  00 - "Set Sensor threshold",
#define OBJ_ID_GET_SENSOR_THRESHOLD				0xC6 //  00 - "Get Sensor threshold",
	
#define	OBJ_ID_SET_LCDC							0xC4 //  00 - "Set LCDC Enable/Disable",	
#define	OBJ_ID_GET_LCDC_STATUS					0xC3 //  00 - "Get LCDC Status", 
#define	OBJ_ID_SET_DAC_for_VCC_LCOS				0xC2 //  00 - "Set DAC for VCC_LCOS", 
#define	OBJ_ID_GET_RGB							0xC1 //  00 - "Get RGB of pixel",

/************************************************************************/
// Add heats commands  20091203
//#define OBJ_ID_GET_SW_INSIDE_VERSION            0xe9	//Read SW inside Version   
//#define OBJ_ID_GET_TEMPER_HERTERS               0xe6	//Get temperature of heaters
//#define OBJ_ID_SET_HEATER_TARGET_TEMPER         0xe5	//Set the target temperature of heaters
//#define OBJ_ID_SET_HERTERS_STATE                0xe4	//Set the state of heaters
//#define OBJ_ID_SET_HERTERS_TOTAL_POWERS         0xe3	//Set total power of heaters
/************************************************************************/

#define OBJ_ID_GET_SW_INSIDE_VERSION            0xe9	//Read SW inside Version   
#define OBJ_ID_GET_TEMPER_HERTERS               0xe6	//Get temperature of heaters
#define OBJ_ID_SET_HEATER_TARGET_TEMPER         0xe5	//Set the target temperature of heaters
#define OBJ_ID_SET_HERTERS_STATE                0xe4	//Set the state of heaters
#define OBJ_ID_SET_TESTER_MODE					0xcc    //Set Tester mode,
#define OBJ_ID_READ_I2C_ADDRESS					0xb7    //Read i2c address,
#define OBJ_ID_WRITE_I2C_ADDRESS				0Xb8	//modify i2c address,

/**************************************************************************/


//Get response message lens
#define LEN_BACK_NORMAL_SETTING                 0
#define LEN_BACK_SERIAL_ID                      8
#define LEN_BACK_MODULE_TYPE                    1
#define LEN_BACK_VER                            6 //Modify by wanxin       
#define LEN_BACK_HARDWARE_DATE                  2
#define LEN_BACK_FIRMWARE_DATE                  2
#define LEN_BACK_SINGLE_ATTENATION              2
#define LEN_BACK_GET_GROUP                      DAC_CHANNEL_NUMS
#define LEN_BACK_GET_PROFILE                    DAC_CHANNEL_NUMS+1
#define LEN_BACK_GROUP_IN_USE                   1
#define LEN_BACK_ALARM                          1
#define LEN_BACK_PROFILE_IN_USE                 1
#define LEN_BACK_CONF_TO_PRO1                   1
#define LEN_BACK_MODULE_SN                      0
#define LEN_BACK_SET_MODULE_TYPE                0
#define LEN_BACK_HW_VERSION                     0
#define LEN_BACK_HW_DATE_CODE                   0
#define LEN_BACK_SET_MAX_CHANNEL_NUM            0
#define LEN_BACK_GET_MAX_CHANNEL_NUM            1

//add by lixiang
#define LEN_PARKING_PROT			            1
#define LEN_SET_PORT                            2
#define LEN_GET_ATEDATA                         1
#define LEN_BACK_ATD_DATA                       4//by luke.liu 6
#define LEN_ERASE_TABLE                         1
#define LEN_WRITE_DATA                          5
#define LEN_GET_TABLE_DATA                      5
#define LEN_START_LOADING_DATA                  1


// LCoS
#define DEV_TYPE_LCOS							0
#define DEV_TYPE_ASIC							1

#define LEN_READ_DEV_REGISTER                   2
#define LEN_WRITE_DEV_REGISTER                  3
#define LEN_READ_0								0
#define LEN_READ_VOLTAGE_ADC_6                  6
#define LEN_READ_1								1
#define LEN_WRITE_PIXEL_SAME                    1


#define LEN_BACK_REGISTER                       1
#define LEN_BACK_0								0
#define LEN_BACK_6								6
#define LEN_BACK_0_2							2
#define LEN_BACK_1                              1 
#define LEN_BACK_2                              2 

#define BACK_NO                                 0x00
#define LEN_BACK_VOLTAGE_ADC					200
//
#define LEN_BACK_SUPPLY_VOLTAGE                 2
#define LEN_BACK_SW_INSIDE_VERSION              12//by luke.liu 0x0d
#define LEN_BACK_TEMPER                         2
#define LEN_BACK_LOW_TEMPER                     2
#define LEN_BACK_HIGH_TEMPER                    2
#define LEN_BACK_ATD_TEMPER                     2
#define LEN_BACK_LC_TEMPER                      2
#define LEN_BACK_GRATING_TEMPER                 2
#define LEN_BACK_HEATER_TEMPER                  2
#define LEN_BACK_MAINBOARD_TEMPER               2
#define LEN_BACK_DAC_OFFSET                     2
#define LEN_BACK_DAC_CHANNEL                    2

#define LEN_BACK_SVN_VERSION                    4
#define LEN_BACK_UPDATE							2
#define LEN_BACK_HEAT_TEMP_RANGE                1
#define LEN_BACK_GET_HEAT_TEMP_RANGE            4

#define LEN_BACK_PWM_DUTY                       1
#define LEN_BACK_SENSOR_PARA                    0x10 
  
//Send message content lens
#define LEN_SET_RS232RATE                       2
#define LEN_SET_SINGLE_ATTENUATION              3
#define LEN_GET_SINGLE_ATTENUATION              1
#define LEN_SET_SWITCH                          (DAC_CHANNEL_NUMS/8)
#define LEN_SET_ATTEN_ALL                       DAC_CHANNEL_NUMS
#define LEN_GET_ATTEN_ALL                       0
#define LEN_SET_PROFILE                         DAC_CHANNEL_NUMS+1
#define LEN_GET_PROFILE                         1
#define LEN_SET_PROFILE_IN_USE                  1
#define LEN_GET_PROFILE_IN_USE                  0
#define LEN_SWITCH_PORT                         2
#define LEN_SET_CHANNEL_ATTENUATION             2
#define LEN_SET_CHANNEL_DELTA_ATTENUATION       2
#define LEN_SET_CONF_TO_PRO1                    0              
#define LEN_GET_FLASH                           4 //7  by luke.liu
#define LEN_SET_FLASH							5 //Add by wanxin
#define LEN_SET_TEMPER                          2
#define LEN_SET_HEATER_TEMPER                   5
#define LEN_SET_HEATER_STATE                    2
#define LEN_SET_HEATER_TOTAL_PWR                2
#define LEN_GET_DAC_OFFSET                      1
#define LEN_SET_DAC_OFFSET                      3
#define LEN_SET_HEAT_TEMP_RANGE                 4
#define LEN_SET_PWM_DUTY                        1

#define LEN_SET_CHANNEL_DAC                     4 //Modify by wanxin
#define LEN_SET_CHANNEL_DAC_MORE_THAN_255		4
#define LEN_SET_CHANNEL_DAC_MORE_THAN_255_NEW   5
#define LEN_GET_CHANNEL_DAC                     2//1 by luke.liu 2018.3.4
#define LEN_GET_CHANNEL_DAC_MORE_THAN_255		2
#define LEN_GET_CHANNEL_DAC_MORE_THAN_255_NEW   3 //Add by wanxin
#define LEN_SET_FREQUENCY                       1
#define LEN_ERASE_BLOCK                         4
#define LEN_ERASE_CHIP                          0

#define LEN_SET_DATA_3                          3

#define LEN_SET_MEMORY_HEAD						3
#define LEN_SET_UPDATE_START                    2
#define LEN_SET_MODULE_SN                       4
#define LEN_SET_MODULE_TYPE                     1
#define LEN_SET_HW_VERSION                      2
#define LEN_SET_HW_DATE_CODE                    2
#define LEN_SET_MAX_CHANNEL_NUM                 1
#define LEN_GET_MAX_CHANNEL_NUM                 0  

#define LEN_GET_CHANNEL_ALL                     64 //by luke.liu
#define LEN_GET_IL_ALL                          4000 //by luke.liu
#define LEN_GET_Freq_ALL                        96 //by luke.liu

// Status code
#define RES_SUCCESS								0	//0:  Command executed successfully; 
#define RES_CHECKSUM_ERROR						1	//1:  checksum ERROR;
#define RES_SYNTAX_ERROR						2	//2.  SYNTAX_ERROR;
#define RES_INVALID_PARAMETER					3	//3.  Invalid parameter
#define RES_COMMAND_EXECUTION_FAILED			4	//4.  Command execution failed
#define RES_FIRMWARE_INSTALLATION_FAILED		5	//5.  firmware installation failed
 

 


//Real module type
#define MODULE_TYPE_1D                          1  //1x1 WSS
#define MODULE_TYPE_2D                          2  //1x2 WSS 
#define MODULE_TYPE_4D                          3  //1x4 WSS
#define MODULE_TYPE_8D                          4  //1x8 WSS

#define MODULE_TYPE_2D_1                        5  //2x1 WSS 
#define MODULE_TYPE_4D_1                        6  //4x1 WSS
#define MODULE_TYPE_8D_1                        7  //8x1 WSS

/* with 5V offset */
#define	DAC_VALUE_OFFSET	                    0//DGE doesn't need offset by luke.liu 8192    
#define DAC_VALUE_BLOCK                         7000
#define LENGTH_COMM_5							(5)
/* Heater On off state */
#define HEATER_ON_STATE							1
#define HEATER_OFF_STATE						0
#define DWORD_TO_BYTE(ucTargetHigher,ucTargetHigh,ucTargetLow,ucTargetLower, dwDWORD) \
	do{ucTargetHigher = (BYTE)((dwDWORD) >> 24);  \
    ucTargetHigh = (BYTE)((dwDWORD) >> 16);	\
	ucTargetLow = (BYTE)((dwDWORD) >> 8);	\
	ucTargetLower = (BYTE)(dwDWORD);}while(0)

//by luke.liu 2018.2.16
#define BYTE_TO_WORD(usTarget,ucHigh,ucLow) \
	do{usTarget  = (USHORT)(ucHigh) << 8;  \
    usTarget |= (USHORT)(ucLow);}while(0)

#define WORD_TO_BYTE(ucTargetHigh,ucTargetLow,usWORD) \
	do{ucTargetHigh  = (BYTE)((usWORD) >> 8);  \
    ucTargetLow = (BYTE)(usWORD);}while(0)

/* Sensor index */
#define SENSOR_GRATING_1 						1
#define SENSOR_GRATING_2						2
#define SENSOR_LC_3								3
#define POS_COMM_DATA			(2)
#define COUNT_DATA_LINE			(10240)
//Command definition
#define LENGTH_COMM_GENERAL		(2)
typedef enum
{
	ERROR_NONE = 0,
	ERROR_CHECKSUM,
	ERROR_SYNTAX,
	ERROR_PARAM,
	ERROR_EXECUTE,
	ERROR_RECVLENGTH,
	ERROR_WRITEBUF,
	ERROR_READBUF,
	ERROR_CALCCHECKSUM,
	ERROR_OTHER
}COMMANDERRORINFO;
/////////////////////////////////////////////////////////////////////////////////
#define WORD_TO_BYTE(ucTargetHigh,ucTargetLow,usWORD) \
	do{ucTargetHigh  = (BYTE)((usWORD) >> 8);  \
ucTargetLow = (BYTE)(usWORD);}while(0)

#define WORD_TO_BYTE_LITTER(ucTargetHigh,ucTargetLow,usWORD) \
	do{ucTargetLow  = (BYTE)((usWORD) >> 8);  \
ucTargetHigh = (BYTE)(usWORD);}while(0)



class CProcessMsg :  public COpComm 
{
public:
	BOOL ReadOPMADC();
	BOOL ReadI2CAddress(CString &strAddress);
	BOOL WrirteI2CAddress(BYTE bAddressIndex);
	BOOL SetTesterMode();
	CProcessMsg();
	virtual ~CProcessMsg();

	char* GetLastError()
	{
		return (char*)(LPCTSTR)m_strLastError;
	} 

	char			m_pszCmdBuf[MAX_BUFFER_LEN];
	CString			m_strLastError;
	BYTE			m_chCheckCode;
	char			m_pszCmdBuffer[MAX_BUFFER_LEN];
	DWORD			m_dwCommLens;
	BOOL			m_bComPortOpen;
     

	
	// send message to MCU module
	BOOL fnSendMsg(char *pszMsg, int nLength);
	BOOL fnCreateCheckCode(const char *pszMsg, int nLength);
	// receive message from MCU module
	BOOL fnGetMsg(char* pszMsg, int nPayloadLen);
	BOOL fnParseCommandByte(BYTE chStatus); // Add by yyh
	int  fnHexStrToInt(CString CTem);

	
	BOOL GetSerialNo(CString &strRecvData);
	BOOL GetVersion(CString &strRecvData,BYTE *cVersion=NULL);
	BOOL fnSetRS232Rate(BYTE byRateIndex, CString &strRecvData);
    BOOL GetHardwareDate(CString &strRecvData);
    BOOL GetFirmwareDate(CString &strRecvData);
	
	BOOL GetAlarm(CString &strRecvData);
	BOOL GetSupplyVoltage(CString &strRecvData);
	BOOL GetTemper(CString &strRecvData,double &dblTempVal);
    BOOL SetLowTemperThreshold(double dbValue);
	BOOL GetLowTemperThreshold(CString &strRecvData);
    BOOL SetHighTemperThreshold(double dbValue);
	BOOL GetHighTemperThreshold(CString &strRecvData);
	
//	BOOL fnSetAllChnWaveplan(pWavePlanAll pstWavePlan,BYTE byPort,CString &strRecvData);
//	BOOL fnGetAllChnWaveplan(pWavePlanAll pstWavePlan,BYTE byPort,CString &strRecvData);
	BOOL fnSet1ChnWaveplan(BYTE byPort, WORD wChnNum, WORD wStart, WORD wEnd, CString &strRecvData);
	BOOL fnGet1ChnWaveplan(BYTE byPort, WORD wChnNum, WORD *wStart, WORD *wEnd, CString &strRecvData);
//	BOOL fnSetAllChnAttn(pWavePlanAll pstWavePlan,BYTE byPort,CString &strRecvData);
//	BOOL fnGetAllChnAttn(pWavePlanAll pstWavePlan,BYTE byPort,CString &strRecvData);
	BOOL fnSet1ChnAttn(BYTE byPort, WORD wChnNum, double dblAttn, CString &strRecvData);
	BOOL fnGet1ChnAttn(BYTE byPort, WORD wChnNum, double *pdblAttn, CString &strRecvData);
	BOOL fnGetStartFreq(double *pdblFreq, CString &strRecvData);
	

	BOOL fnsoftReset(CString &strRecvData);
	BOOL GetATDData(BYTE byTypeATD,WORD *wAdcVal,double *pdblVal,CString &strRecvData);
	BOOL fnGetMemoryData(BYTE byFlashType,CString dataAddress, DWORD wDataLength, CString &strRecvData, PBYTE pbyBuffer,BOOL bChange=TRUE,BOOL bRegister=FALSE);
	BOOL fnGetMemoryData(CString dataAddress, WORD wDataLength, CString &strRecvData, PBYTE pbyBuffer,BOOL bChange=TRUE,BOOL bRegister=FALSE);

	BOOL fnWriteMemoryData(BYTE byFlashType,CString dataAddress, DWORD wDataLength, CString &strRecvData, PBYTE pbyBuffer,BOOL bChange=TRUE);
	BOOL fnSetHardwareReset(BYTE byFlashType,CString dataAddress, WORD wDataLength, CString &strRecvData, PBYTE pbyBuffer);
	BOOL fnGetSW_InsideVersion(CString &strRecvData);


	BOOL fnGetsensorTemp(BYTE bHeaterNum, CString &strRecvData, double &dblTemp);
	BOOL fnSetPidPara(BYTE byVal,float fPara,CString &strRecvData);
	BOOL fnGetPidPara(BYTE byVal,float &fPara,CString &strRecvData);
	BOOL fnSetsensorPara(BYTE byVal, float fParaA, float fParaB, float fParaC, float fParaD, CString &strRecvData);
	BOOL fnGetsensorPara(BYTE byVal, float &fParaA, float &fParaB, float &fParaC, float &fParaD, CString &strRecvData);
	BOOL fnSetSensorThrd(BYTE byVal, float fThreshold, CString &strRecvData);
	BOOL fnGetSensorThrd(BYTE byVal, float &fThreshold, CString &strRecvData);
 
 
	BOOL SetHeaterTargetTemper(BYTE byHeaterNum,double dblTagTemperK,double dblTagTemperC,CString &strRecvData);
	BOOL GetHeaterTargetTemper(BYTE byHeaterIdx,double &dblTagTemper,CString &strRecvData);
	BOOL SetHeaterState(BYTE byHeaterNum,BYTE byHeaterOnOff,CString &strRecvData);
	BOOL GetHeaterState(BYTE byVal, CString &strRecvData);
	BOOL fnGetRGB(WORD &wLine, WORD &wCol, WORD &wRGB565,CString &strRecvData );

	/************************************************************************/ 
	
	// LCoS
	BOOL fnCopyFileToRam(CString &strRecvData);                // step 1
	BOOL fnQueryCopyStatus(CString &strRecvData,BYTE &byVal);  // step 2 
	BOOL fnLoadImageToAsic(CString &strRecvData,BYTE byVal=0);   // step 3
	
	BOOL fnSaveImageToFlash(CString &strRecvData,BYTE byVal);	
	BOOL fnReadDevRegister(BYTE byDevType,BYTE byAddr, CString &strRecvData,BYTE &byVal);
	BOOL fnWriteDevRegister(BYTE byDevType,BYTE byAddr, CString &strRecvData,BYTE byVal);
	BOOL fnReadLcosVolADC(BYTE byType,WORD wCount, CString &strRecvData,PWORD pwADC);
	BOOL fnSetPixelSameVal(CString &strRecvData,BYTE byVal);
	BOOL fnSetLCDC(CString &strRecvData,BYTE byVal);
	BOOL fnGetLCDCStatus(CString &strRecvData,BYTE &byVal);   
	BOOL fnSetDacForVccLCoS(CString &strRecvData,double dblVal);
	BOOL fnSetVcom(double dblVcomp,double dblVcomn, double dblVDDA, CString &strRecvData);
	BOOL fnReSorting(PWORD pwSource,int nSum);
	BOOL fnCheckHexVal(CString strHexVal);

	
	// end LCoS ////////////////////////////////////////////////////////////////////////////////

	// reserved /////////////////////////////////////////////////////////////////////////////////
	BOOL SetSpectrumAttenuation(byte bLCIndex, WORD wChannelCount, int *pnDacArray);
	BOOL SetParkingPort(WORD nParkingPort);
	BOOL GetParkingPort(WORD *nParkingPort);
	BOOL GetChannelDACValue(WORD wChannel,WORD &wReadDAC,CString &strRecvData);
	BOOL SetOneDACValue(WORD wChannel,USHORT usChannelDac);
	BOOL SetAllAttenDac(WORD wDacVal);
	BOOL SetAllSwitchDac(WORD wDacVal);
	BOOL GetMaxChannNum(BYTE &byChnnlNum);
	BOOL SetMaxChannNum(CString strModuleTemp,CString &strRecvData);
	BOOL SetHWDate(CString strHWDate,CString &strRecvData);
	BOOL SetHWVersion(CString strHWVersion,CString &strRecvData);
	BOOL SetModuleType(CString strMoudleType,CString &strRecvData);
	BOOL SetModuleSN(CString strValue,CString &strRecvData);
	BOOL SetChannelAttenuationChange(int nChannel,double deltaAttenuation,CString &strRecvData);
	BOOL SetChannelAttenuation(int nChannel,double attenuation,CString &strRecvData);
	BOOL SwitchPort(int nport,int nChannel,CString &strRecvData);
	BOOL SetConfToPro1(CString &strRecvData);
	BOOL GetProfileInUse(CString &strRecvData);
	BOOL SetProfileInUse(byte bProfileNum,CString &strRecvData);
	
	BOOL SetHalfDACValues(byte byChannelType, USHORT usChannelDac);
	 
    //BOOL SetOneDACValue(byte byChannel,USHORT usChannelDac);
    BOOL SetAllDACValues(byte byChannelType,USHORT usChannelDac);
    BOOL GetChannelDACValue(byte byChannel,WORD &wReadDAC,CString &strRecvData);
	BOOL ReadTableData(int iDataNumber,long ldataindex,USHORT usGetLens);
	BOOL SetSpectrumAttenuation(int nPort,int nChannel, double dbValue);
    BOOL GetSpectrumAttenuation(int nChannel,CString &strRecvData);
	
	BYTE GenerateCheckSum(BYTE* pbyData, BYTE byCmdLength);
	BOOL fnTxRxCommand(BYTE byCmdObject,BYTE byReadWrite,DWORD dwAddr,DWORD dwWriteValue,DWORD &dwResponseValue,DWORD dwSleepTime);
	BOOL ParseErrorCode(BYTE byStatus);
	BOOL fnSaveMyFileName(CString cFileType,CString &cFile);
 


    BOOL GetSVNVersion(CString &strRecvData);
    BOOL GetLCTemper(CString &strRecvData);
	BOOL GetGratingTemp(BYTE bGratingNum, CString &strRecvData);
 

    BOOL GetHeaterTempRange(CString &strRecvData);
    BOOL SetHeaterTempRange(WORD wLowLimit, WORD wHighLimt, CString &strRecvData);
	
	BOOL SetHeaterTotalPwr(BYTE byHeaterIdx,float fPower_w,CString &strRecvData);
	BOOL GetHeaterPwr(BYTE byHeaterIdx,double &dblReadBackPwr,CString &strRecvData);
 
	//by luke.liu 
	BOOL fnRunNewFw(CString &strRecvData);
    BOOL SetAllPixelAtten(float *pfAtten, int nChannels);
	BOOL GetAllPixelAtten(float *pfAtten, int *pnChannels);
	BOOL fnGetCurTemperIL(double *pdblIL,double* pdblFre, int *pnPoints);
	BOOL fnGetCurTemperFreq(float *pfFreq, int *pnPoints);
	BOOL fnSetOECurve(float *pfCurveFreq, float *pfCurveIL, WORD nPoint);
	void SetDGEIndex(int nDGEIndex);
	BOOL fnSelectTableLoading();

	BOOL fnParseCommandByteForDGE(BYTE chStatus);
	BOOL fnStartFirmwareLoading();
	BOOL fnFirmwareDownload(char *pszBuffer, int nLength);

	/************************************************************************/
	CString m_strErrorMsg;
protected: 
	
private:
	BYTE m_abyRxBuf[COUNT_DATA_LINE], m_abyTxBuf[COUNT_DATA_LINE];
	DWORD m_dwRxLens, m_dwTxLens;
	CTime m_tiRx, m_tiTx;
	int m_nDGEIndex;
	
};

#endif // !defined(AFX_PROCESSMSG_H__12345678ASDFG__INCLUDED_)
