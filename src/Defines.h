#ifndef	_DEFINITION_0123456789102345678_H_INCLUDED_
#define _DEFINITION_0123456789102345678_H_INCLUDED_

 
//Color definition
#define COLOR_BLACK			RGB(0, 0, 0)
#define COLOR_BLUE			RGB(0, 0, 255)
#define COLOR_GREEN			RGB(0, 255, 0)
#define COLOR_RED			RGB(255, 0, 0)
#define COLOR_YELLOW		RGB(255, 255, 0)
#define COLOR_LIGHT_BLUE	RGB(0, 255, 255)
#define COLOR_PINK			RGB(255, 0, 255)
#define COLOR_WHITE			RGB(255, 255, 255)


#define VELOCITY_LIGHT      (double)(299792458.458)
#define IMAGE_768			(1024*768)
#define PD_TABLE			(15*2*6)
#define PD_OFFSET			(400*2*6)
#define LCOS_SIZE_1024		0x000C0000L		// 1024 *768  = 786432


#define	GRAPH_WL_PW		0      //
#define	GRAPH_GRAY_PW	5
#define GRAPH_GHz_1024  6
#define GRAPH_WL_TYPE   12 
#define GRAPH_GHZ_TYPE  13
#define GRAPH_LINE_PW   15

 
#define CALIB_1024VsGHz_BWD  6

#define GHz_FIRST_NEW       191000
#define GHz_LAST_NEW        196200   


#define SLICENUM			1800   //1544
#define GHz_FIRST           191250			//191325
//#define GHz_LAST			GHz_FIRST+SLICENUM*CUSTOM_SLICE_GHz		// 196125 
#define GHz_LAST			196350			// 196125
#define GHz_CALCMAX			196200 

#define ATTENOFFSETCOEFFICIENT   1.0   //为了offset补偿更好，给出一个权重系数

#define DACOFFSETCONVOLUTIONCOUNT  12 //卷积迭代次数


#define CAL_768PORT_ADD_OFFSET 2

//#define POLYNOMIAL 0xEDB88320L

#define TLS_OPMREF_SWCHANNEL 24 // change 25 to 24
#define TLS_OPMREF_SWPORT	1
#define TLS_SCAN_SWPORT		0

#define TLS_POWER_SWCHANNEL		25
#define TLS_HIGH_POWER_SWPORT	1
#define TLS_LOW_POWER_SWPORT	0

// address Tables
#define ADDR_TBL_SLICE          0x0000 // 0x20000 
#define ADDR_TBL_HEIGHT         0x0001 // 0xC0000  
#define ADDR_TBL_ATTN_port1		0x0002 // 0x160000          
#define ADDR_TBL_ATTN_port2		0x0007 // 0x7A0000         
#define ADDR_TBL_ATTN_port3		0x000C // 0xDE0000          
#define ADDR_TBL_ATTN_port4		0x0011 // 0x1420000          
#define ADDR_TBL_ATTN_port5		0x0016 // 0x1A60000          
#define ADDR_TBL_ATTN_port6		0x001B // 0x20A0000          
#define ADDR_TBL_ATTN_port7		0x0020 // 0x26E0000          
#define ADDR_TBL_ATTN_port8		0x0025 // 0x2D20000          
#define ADDR_TBL_ATTN_port9		0x002A // 0x3360000          
#define ADDR_TBL_ATTN_port10	0x002F // 0x39A0000          
#define ADDR_TBL_ATTN_port11	0x0034 // 0x3FE0000          
#define ADDR_TBL_ATTN_port12	0x0039 // 0x4620000          
#define ADDR_TBL_ATTN_port13	0x003E // 0x4C60000         
#define ADDR_TBL_ATTN_port14	0x0043 // 0x52A0000          
#define ADDR_TBL_ATTN_port15	0x0048 // 0x58E0000         
#define ADDR_TBL_ATTN_port16	0x004D // 0x5F20000 

#define ADDR_TBL_DEF_RGB        0x0052 // 0x7000000
#define ADDR_TBL_CALI_OPM       0x0053 // 0x
#define ADDR_TBL_PD_OFFSET		0x005B // 0x
#define ADDR_TBL_ADC_CORRECTION		0x0063 // 0x
//Address

#define ADDR_MODULEPARA			(0x24000)


//DGE address
//by luke.liu
#define ADDR_MFG_TBL		    (0x0ED000)
#define ADDR_MAPPING_TBL        (0x060000)
#define ADDR_BEAM_TBL           (0x066000)
//edit by yud 20181106
//#define ADDR_ATTEN_TBL          (0x08C000)
#define ADDR_ATTEN_TBL          (0x200000)
// end yud 20181106
#define ADDR_WAVE_TBL           (0x065000)
#define ADDR_IL_TBL             (0x061000)
#define ADDR_PARA_TBL			(0x0EE000)

#define ADDR_DAC_OFFSET_TBL     (0x130000)



#define WBA_PORT_SUM            16

#define NULL_DWORD				(0xFFFFFFFF)
      
#define ALL_PORT_PASS			3
#define EVEN_PORT_PASS			2
#define ODD_PORT_PASS			1
#define	ALL_PORT_CLOSE			0
#define	OPM_COMBO_TYPE			12
#define RX2_MODULE_TYPE			11
#define RX1_MODULE_TYPE			10

#define DEVICE_TYPE_RX1			1
#define DEVICE_TYPE_DGE			3
#define DEVICE_WSS_2X1			2
#define DEVICE_TYPE_OPM			4

#define CALC_RX2_CORNER_GRAY	130
/************************************************************************/
/* 0,5,10,15......120,125,126,127,128......149,150                      */
/* 0   => 125 = 26 Po                                                   */
/* 126 => 150 = 25 Po      26 +25 = 51                                  */
/************************************************************************/
#define CAL_GRAY_PW_Po      256 

#define LCoS_SCIIC_ADDR_bit  0x04  

#define CUSTOM_SLICE_GHz      (3.125)


#define DC_BALANCE_DISABLE   0
#define DC_BALANCE_ENABLE    1

#define LCOS_DEVICE  0
#define ASIC_DEVICE  1

#define COM_READINTERVALTIMEOUT				500
#define COM_READTOTALTIMEOUTCONSTANT		500
#define COM_READTOTALTIMEOUTMULTIPLIER		500
#define COM_WRITETOTALTIMEOUTCONSTANT		500
#define COM_WRITETOTALTIMEOUTMULTIPLIER		500
 
#define CHG_AND_RGB_B10                  0x00
#define CHG_AND_RGB_G10                  0x01
#define CHG_AND_RGB_R10                  0x02
#define CHG_AND_RGB_2                    0x04
#define CHG_AND_RGB_2_0                  0x07
#define CHG_AND_RGB_7_3                  0xF8

#define GRAY_RGB565(ucGray)              ((((USHORT)(ucGray) & CHG_AND_RGB_2_0) << 11) | \
                                          (((USHORT)(ucGray) & CHG_AND_RGB_2_0) << 8)  | \
                                          (((USHORT)(ucGray) & CHG_AND_RGB_2_0) << 5)  | \
                                          (((USHORT)(ucGray) & CHG_AND_RGB_7_3) >> 3))




#define		TEST_DATA_CHANGE		-1
#define     AUTO_TEST_TYPE_IDLE		0
#define     AUTO_TEST_TYPE_REF		210
#define		AUTO_TEST_TYPE_SCAN	    211 
#define		AUTO_TEST_TYPE_CAIL		212
#define		AUTO_TEST_TYPE_TEST		213
#define		AUTO_TEST_TYPE_RUNALL	214


#define		SCAN_TYPE_IDLE			0
#define		SCAN_TYPE_REF			200
#define		SCAN_TYPE_SCAN			201
#define		SCAN_TYPE_PDL			202
#define		SCAN_TYPE_NOPDL			203
#define		SCAN_TYPE_NIONECURVE	205
#define		RX2_SWITCH_PORT1		0
#define		RX2_SWITCH_PORT2		(33554431)
#define		ZH_HIGH_TLS_PORT		1
#define		ZH_LOW_TLS_PORT			2
#define		STATE_NO 0
#define		STATE_TBD 1
#define		STATE_ING 2
#define		STATE_DONE 3
#define		STATE_FAIL 4
#define     SKIP_TEST 0
#define     SINGLE_ITEM_TEST 1
#define     NO_ACT 0xFF
#define		SHOW_STATE 2
#define		AUTO_RUN_TIME_TIMER_CALI		1
#define		AUTO_RUN_TIME_TIMER_OPTEST		2
#define		AUTO_RUN_TIME_TIMER_RUNALL		3

#define		AUTO_RUN_TEMP_TIMER_CALI		4
#define		AUTO_RUN_TEMP_TIMER_OPTEST		5
#define		AUTO_RUN_TEMP_TIMER_RUNALL		6
#define		WAIT_OPTICAL_TIME				2000
#define		CALIBRATION_MODULE_MAX	4

#define		CALIBRATION_TEMPER_MAX	5
#define     DOWNLOA_TEMPER_MAX 5
#define     TEST_TEMPER_MAX 5

#define		VERIFY_TEMPER_MAX	5//2018.5.5
#define		DEVICE_COUNT_MAX	16
#define		DEVICE_PORT_MAX		2
#define     OPM_BUFFER_MAX		96*2

#define INI_BOARD_SNR_PARA1              (-2.16E-2)
#define INI_BOARD_SNR_PARA2              (4.2784E2)

#define	   HQ_TLS_SW	0
#define	   ZH_TLS_SW	1

#define FRQ_CABLI_DATA_LEN				80
#define LCOS_LEFT_MARGIN_PIXEL			10
#define LCOS_RIGHT_MARGIN_PIXEL			10


//added by luke.liu 2018.1.7
#define     PIXEL_NUMBER_64              64
#define     PIXEL_NUMBER_96              96
#define     PIXEL_SET_INTERVAL           8
//#define     SCAN_WAVE_COUNT              9000     //(1570-1625)/0.005    
#define     PIXEL_CHANNEL_START          1
#define     PIXEL_SET_TYPE_ALL           1//Type: 1 = set all pixel dac
#define     PIXEL_ATTEN_CALI_NUMBER      128 //how many times for attenuation caibration loop
#define     PIXEL_WAVE_STEP              0.008 //1Ghz = 0.008nm、
#define     PIXEL_WAVE_STEP2             0.016 //1Ghz = 0.008nm
#define     PIXEL_BEAM_NUMBER            401
#define     IL_LUT_POINT                 1000
#define     ATTEN_LUT_POINT              256
#define     FITTING_POINT                 1000


#define MODULE_INFO_TYPE    0
#define PARAMTER_TABLE_TYPE 1

#define MODULE_INFO_SIZE    256
//#define PARAMTER_TABLE_SIZE 256
#define PARAMTER_TABLE_SIZE 512 //added by ris 2015-6-10


#define SPI_FLASH_READ  0
#define NAND_FLASH_READ 1
#define IIC_FLASH_READ  2

#define FIRST_OPM_ADDRESS 0x020000
#define FIRST_OPM_ADC_ADDRESS 0x010000

//added by luke.liu 2018.1.12
#define FLASH_SN_ADDRESS           "0ED004"
#define FLASH_PN_ADDRESS           "0ED024"


// user message definition
#define WM_THREAD_DESKTOP		(WM_USER + 2888)

// desktop message process type
#define DT_CHANGE_SCAN_BTN        0x0001
#define DT_DRAW_ONE_CURVE         0x0010
#define DT_DRAW_ADD_ONE_CURVE     0x0100


#define MAXMODULECOUNT 4



typedef struct tagUsbFileHead
{	
	DWORD       dwLength;		// 4  Bytes
	DWORD       dwFileType;		// 4  Bytes
	DWORD       dwAddr;			// 4  Bytes
	DWORD       dwCRC32;		// 4  Bytes
	DWORD       dwLastDW;       // 4  Bytes
	BYTE     	byReserved[12];	// 12  Bytes
	 
}stUsbFileHead, *pUsbFileHead;
	 
//by luke.liu
typedef struct PIXELWAV
{
	int nLoop;
	int nPixelWavNum;
	double  dblPixelWav[PIXEL_NUMBER_96];
	double  dblPixelGhz[PIXEL_NUMBER_96];
	double  dblPixelIL[PIXEL_NUMBER_96];
}stPixelInfo, *pstPixelInfo;



// edit by yud 20181106 
//typedef struct ATTENUATIONTABLE
//{
//	BYTE  byCrc32[4];
//	float pPixelAttenCoefficent[PIXEL_NUMBER_64][256][4];
//	BYTE  pbyReserve[32*256*4*4]; //pixelNum*AttenNum*CoeffNum*float
//}stAttenTable, *pstAttenTable;
typedef struct ATTENUATIONTABLE
{
	unsigned int  TempCrc ;//这个温度CRC?
    float Temp;//30 
    unsigned int Temp1Crc;  
	float pPixelAttenCoefficentTemp1[96][256][4];
    unsigned int Temp2Crc;  
	float pPixelAttenCoefficentTemp2[96][256][4];

}stAttenTable, *pstAttenTable; 

/// end yud 20181106
//by luke.liu 2018.3.28
typedef struct TagSinglDACOFFSETTABLE
{
	BYTE  byCrc32[4];
	char  byDacOffset[PIXEL_NUMBER_64][256];
	BYTE  pbyReserve[32*256*1];//pixelNum*AttenNum*Char
}stOneOffsetTable, *pstOneOffsetTable;

typedef struct STDACSET
{
	int	 iPixelCount;
	int  iIndex[PIXEL_NUMBER_96];
	int  iOpenDac[PIXEL_NUMBER_96];
	int	 iBlockDac[PIXEL_NUMBER_96];	
}stPixelOpen, *pstPixelOpen;

typedef struct STATTENSET
{
	int	 iPixelCount;
	int  iIndex[PIXEL_ATTEN_CALI_NUMBER]; //Max loop for atten calibration
	int  iSetDac[PIXEL_ATTEN_CALI_NUMBER];	
}stPixelAtten, *pstPixelAtten;



//by luke.liu 2018.2.8
typedef enum
{
   LUT_MFG_TBL,
   LUT_MAPPING_TBL,
   LUT_PARA_TBL,
   LUT_ATTEN_TBL,
   LUT_ATTEN_DAC_OFFSET_TBL, //by luke.liu 2018.3.28
   LUT_WAVE_TBL,
   LUT_IL_TBL,
   LUT_BEAM_TBL
}eLutType;

//by luke.liu 2018.3.26
typedef enum
{
   PIXEL_1,
   PIXEL_2,
   PIXEL_3,
   PIXEL_62 = 61,
   PIXEL_63 = 62,
   PIXEL_64 = 63
}ePixel;

typedef struct STSWITCHTABLE
{
	int	 InputPortSW[CALIBRATION_MODULE_MAX][DEVICE_COUNT_MAX][DEVICE_PORT_MAX];
	int	 OutputPortSW[CALIBRATION_MODULE_MAX][DEVICE_COUNT_MAX][DEVICE_PORT_MAX];	
}stSWTable, *pstSWTable;
typedef struct STCALIBRATIONTABLE
{
	int  iHeightCaliDeviceInPort[DEVICE_COUNT_MAX];
	int	 iHeightCaliDeviceOutPort[DEVICE_COUNT_MAX];
	BOOL bGrayStep1CaliEnable;
	BOOL bGrayStep2CaliEnable;
	int	 iStep1StartGray;
	int	 iStep1EndGray;
	int  iStep1GrayStep;
	int	 iStep2StartGray;
	int  iStep2EndGray;
	int  iStep2GrayStep;
	int  iStep1GrayCaliDeviceInPort[DEVICE_COUNT_MAX];
	int	 iStep1GrayCaliDeviceOutPort[DEVICE_COUNT_MAX];
	int  iStep2GrayCaliDeviceInPort[DEVICE_COUNT_MAX];
	int	 iStep2GrayCaliDeviceOutPort[DEVICE_COUNT_MAX];
	
}stCaliTable, *pstCaliTable;


typedef struct STTESTTABLE
{
	int	 iModuleCount;
	BOOL bEnable[CALIBRATION_MODULE_MAX];
	int  iTestSWInPort[DEVICE_COUNT_MAX];
	int	 iTestSWOutPort[DEVICE_COUNT_MAX];	
}stTestTable, *pstTestTable;
typedef struct STCONFIGTABLE
{
	int	 nCaliTempCount;
	int	 nTestTempCount;
	int  nDWTempCount;
	double dbCalTemp[CALIBRATION_TEMPER_MAX];
	double dbDWTemp[DOWNLOA_TEMPER_MAX];
	double dbTestTemp[TEST_TEMPER_MAX];
}stConfigTable, *pstConfigTable;


//from FW110C 生产信息表增加Slice frequency, Slice step, Slice number
typedef struct STMANUINFOTABLE
{
	int  iCrc32;
	BYTE pbySN[32];
	BYTE pbyPN[32];
	int iModType;
	int pStrfreq;
	int pEndfreq;
	float fSliceFre;
	float fSliceStep;
	int   nSliceNumber;
	BYTE pbyHV[4];
	BYTE pbyHD[2];
	BYTE pbyCaliDate[4];
 	BYTE pbyReserve[154];
}stManuTable, *pstManuTable;



typedef struct STPIXEIMAPPING
{
	BYTE byCrc32[4];
	BYTE pbyPixel[PIXEL_NUMBER_96];
	//BYTE pbyReserve[32];
    
}stPixelMapping, *pstPixelMapping;

typedef struct TagSingleWave
{
	BYTE  byCrc32Temp[4];
	float pPixelWaveGhz[PIXEL_NUMBER_64];
	BYTE  pbyReserve[32*4];
}stOneWaveInfo, *pstOneWaveInfo;

typedef struct WAVELENGTHTABLE
{
	BYTE  byCrc32[4];
	float fTemp[5];
	stOneWaveInfo stWaveInfo[5]; //5个温度
}stWaveTable, *pstWaveTable;

//by luke.liu 2018.3.28
typedef struct DACOFFSETTABLE
{
	BYTE  byCrc32Temp[4];
    float fTemp[5];
	stOneOffsetTable stDacOffset[5];
}stDACOffsetTable, *pstDACOffsetTable;



typedef struct TagSingleBeam
{
	BYTE  byCrc32[4];
	float pPixelbeam[PIXEL_BEAM_NUMBER];
}stOneBeam, *pstOneBeam;

typedef struct PIXELBEAMTABLE
{
	stOneBeam  stBeamInfo[PIXEL_NUMBER_64];
	BYTE       pbyReserve[32*sizeof(stOneBeam)]; //crc32+beam
}stBeamTable, *pstBeamTable;

typedef struct TagSingleIL
{
	int   iCrc32IL;
	USHORT pwPointIL[1000];
}stOneIL, *pstOneIL;

typedef struct ILTABLE
{
	BYTE  byCrc32Temp[4];
    float fTemp[5];
	int   iCrc32freq;
	float freq[1000];
    stOneIL stIL[5];		
}stILTable, *pstILTable;

typedef struct STMESSAGETABLE
{
	int  iDeviceNO;
	BYTE pbyDeviceType[16];
    BYTE pbyDeviceMapping[16];
	int  iActDeviceNmb;
	int  iPixel_Sum;

}stMessgetable, *pstMessgetable;


typedef struct STDATABASEINFO
{
	CHAR CHSN[32];
	CHAR CHPN[32];
	CHAR CaliStation[4];
	CHAR TestStation[4];
	CHAR CaliTemplateRev[4];
	CHAR TestTemplateRev[4];
	CHAR CaliID[32];
	CHAR TestID[32];
	CHAR CaliTime[32];
	CHAR TestTime[32];
 	CHAR CaliTemper[32];
	CHAR TestTemper[32];
}STDATABASE, *pstDATABASEINFO;

//ADD SOCKET
struct RECVPARAM
{
	SOCKET sock; //已创建的套接字
	HWND hwnd;  //对话框句柄
};



typedef struct tagDGEWLShiftVerCfg//Add by wanxin
{
	int nPixelIndex;
	double dbAttenValue;
}stDGEWLShiftVerCfg,*pDGEWLShiftVerCfg;

typedef struct tagDGEPixelOpticalInfo//Add by wanxin
{
	int nPixelIndex;
	double dbCenterWL;
	double dbCenterGHz;
	double dbMinIL;
}stDGEPixelOpticalInfo,*pDGEPixelOpticalInfo;


//default value by luke.liu
typedef struct MOUDLEPARAMETER
{
	BYTE byCrc32[4];
	USHORT wPixelNum; //64
	USHORT wTempHys; //20
	float pDelay[4]; //0,0,0,500
	float pInterSensor[4];//7.30908070E-09,- 3.22113312E-05,7.16609602E-02,2.96097735E+01
	float pSensor1[4];//7.30908070E-09,- 3.22113312E-05,7.16609602E-02,2.96097735E+01
	float pSensor2[4];//7.30908070E-09,- 3.22113312E-05,7.16609602E-02,2.96097735E+01
	USHORT  wASICsctvalue;//0
	int   nSensorType[3];
	BYTE  pbyReserve[170];//0xFF

//	BYTE byCrc32[4];
//	USHORT wPixelNum; //64
//	USHORT wTempHys; //20
//	float pDelay[4]; //0,0,0,500
//	float pInterSensor[4];//7.30908070E-09,- 3.22113312E-05,7.16609602E-02,2.96097735E+01
//	float pSensor1[4];//7.30908070E-09,- 3.22113312E-05,7.16609602E-02,2.96097735E+01
//	float pSensor2[4];//7.30908070E-09,- 3.22113312E-05,7.16609602E-02,2.96097735E+01
//	USHORT  wASICsctvalue;//0
//	BYTE  pbyReserve[182];//0xFF
    
}stModuleParameter, *pstModuleParameter;

#define  MAX_ONLINE 256
#define WM_RECVDATA WM_USER+1


//Max attenuation when block
#define BLOCK_ATTEN			25.5	// 25.5dB
#define MAX_CHNN_NUM        96     // 8*96 =768
#define ONE_LC_CHNN_100G    48
#define DAC_CHANNEL_NUMS    96      // DAC channel number
 

#define PROFILE_NUMS        64   //- 模板个数

#define ERICCSON_IMAGE_ADD  0x00


#define UPDATEINTERVAL			8
#define UPDATESTEPAFTER			3800

#define	 MAX_LINE              256

enum {MULTIPLE_CHANNEL_ALL = 1, MULTIPLE_CHANNEL_ODD, MULTIPLE_CHANNEL_EVEN};

typedef struct tagDACIndexMap
{
	int nSW1Index;
	int nSW2Index;
	int nSW3Index;
	int nSW4Index;

	int nAttenIndex;
	int nReserve3;
	int nReserve4;
	int nReserve5;
}STDACIndexMap, *PDACIndexMap;

// define all command const
enum {
	
	CMD_GET_SERIAL_NO,                  //  0  - "0x01 Read Module Serial Number", 
	CMD_GET_MODULE_TYPE,				//  1  - "0x02 Read Module Type",
	CMD_GET_VER,                        //  2  - "0x03 Read Module Version", 
	CMD_SET_RS232_RATE,					//  3  - "0x04 Set RS232 Baud Rate",
	CMD_GET_HARDWARE_DATE,				//  4  - "0x05 Read Hardware Date Code",
	CMD_GET_FIRMWARE_DATE,				//  5  - "0x06 Read Firmware Date Code", 
	
	CMD_GET_ALARM,                      //  14 - "0x21 Read Alarm", 
	CMD_GET_SUPPLY_VOLTAGE,             //  15 - "0x22 Read Power Supply Voltage",
	CMD_GET_TEMPER,                     //  16 - "0x23 Read temperature", 
	CMD_SET_LOW_TEMPER,                 //  17 - "0x31 Set Low Temperature Alarm Threshold", 
	CMD_GET_LOW_TEMPER,                 //  18 - "0x32 Read Low Temperature Alarm Threshold", 
	CMD_SET_HIGH_TEMPER,                //  19 - "0x33 Set High Temperature Alarm Threshold", 
	CMD_GET_HIGH_TEMPER,                //  20 - "0x34 Read high temperature alarm threshold",
	CMD_SET_ALL_CHN_WAVEPLAN,			//  21 - "0x60 Set all channel waveplan",
	CMD_GET_ALL_CHN_WAVEPLAN,           //  22 - "0x61 Get all channel waveplan",
	CMD_SET_1_CHN_WAVEPLAN,             //  23 - "0x62 Set waveplan of one channel",
	CMD_GET_1_CHN_WAVEPLAN,             //  24 - "0x63 Get waveplan of one channel",
	CMD_SET_ALL_CHN_ATTN,		        //  25 - "0x64 Set attenuation of all channels",
	CMD_GET_ALL_CHN_ATTN,               //  26 - "0x65 Get attenuation of all channels",
	CMD_SET_1_CHN_ATTN,                 //  27 - "0x66 Set attenuation of one channel",
	CMD_GET_1_CHN_ATTN,                 //  28 - "0x67 Get attenuation of one channel", 
	CMD_GET_Start_FREQ,	                //  29 - "0x68 Get starting frequency",
	CMD_GET_OPM_POWER_OFFSET,			//  30 - "0x69 Get OPM Power Offset"
	CMD_SET_OPM_POWER_OFFSET,			//  31 - "0x6A Set OPM Power Offset"
   	
	//debug commands
	CMD_DBG_SEPARATOR0,                 //  50 - "            ",
	CMD_DBG_SEPARATOR,                  //  50 - "------Debug Commands------------------------------------------", 	
	CMD_SOFT_RESET,						//	DB - "0x44 Soft Reset Linux", // Linux
	CMD_GET_ADC_DATA,					//  DB - "0xF2 Get ADC data",
	CMD_GET_MEMORY_DATA,                //  DB - "0xEB Read Memory Data",	
	CMD_SET_MEMORY_DATA,                //  DB - "0xEA Write Memory Data",
	CMD_GET_SW_INSIDE_VER,				//  DB - "0xE9 Read SW inside Version",	
	CMD_GET_SENSOR_TEMPERATURE,         //  DB - "0xE6 Get sensor temperature",
	CMD_SET_TEC_TARGET_TEMPER,			//  DB - "0xE5 Set TEC target temperature",
	CMD_SET_HEATER_STATE,				//	DB - "0xE4 Set the state of heaters",
	CMD_SET_HEATER_POWER,               //	DB - "0xE3 Set Heater Power",
	CMD_GET_TEC_TARGET_TEMPER,			//	DB - "0xE2 Get TEC target temperature",
	CMD_GET_HEATER_STATE,               //	DB - "0xE1 Get Heater State",
	CMD_GET_HEATER_POWER,               //	DB - "0xE0 Get Heater Power",
	CMD_SET_PID_PARA,					//  DB - "0xDD Set PID parameters",
	CMD_GET_PID_PARA,					//  DB - "0xDC Get PID parameters",
	CMD_SET_SENSOR_PARA,                //  DB - "0xD4 Set sensor parameters",
	CMD_GET_SENSOR_PARA,                //  DB - "0xD3 Get sensor parameters",

	CMD_COPY_IMAGE_TO_OMAPL138,			//  DB - "0xCF Copy image to OMAPl138",	
	CMD_LOAD_IMAGE_SEND_TO_ASIC,		//  DB - "0xCE Load image send to ASIC",	
	CMD_SAVE_IMAGE_TO_FLASH,			//  DB - "0xCD Save image file to flash",	
	CMD_QUERY_IMAGE_COPY_STATUS,		//  DB - "0xCC Query image file copy status",	
	CMD_READ_DEVICE_REGISTER,			//  DB - "0xCB Read Device register",	
	CMD_WRITE_DEVICE_REGISTER,			//  DB - "0xCA Write Device register",	
	CMD_READ_ADCS_CONTINUOUSLY,         //  DB - "0xC9 Read ADCs continuously",
	CMD_SET_PIXEL_SAME_VALUE,			//  DB - "0xC8 Set all pixel RGB gray scale the same value",
	CMD_SET_SENSOR_THRESHOLD,			//  DB - "0xC7 Set Sensor threshold",
	CMD_GET_SENSOR_THRESHOLD,			//  DB - "0xC6 Get Sensor threshold",
	CMD_SET_LCDC,						//	DB - "0xC4 Set LCDC status",
	CMD_GET_LCDC_STATUS,				//	DB - "0xC3 Get LCDC status",
	CMD_SET_DAC_VCC_LCOS,               //	DB - "0xC2 Set DAC for VCC_LCoS",
	CMD_GET_RGB,                        //  DB - "0xC1 Get RGB of pixel",
	 
	CMD_SET_DC_BALANCE,                 //	DB - "0xXX Set DC balance",
	CMD_SET_VcomP,                      //  DB - "0xXX Set Vcomp/VcomN",
	CMD_SET_Default_Setting,            //  DB - "0xXX Save current setting as default",
	CMD_Check_nand_Flash,	            //  DB - "0xXX Check nand flash error block",
    CMD_SET_CALIBRATION_TEC_T,          //  DB - "0xXX SET CALIBRATION S3 T",
	CMD_SET_Hardware_reset,				//	30 - "0xXX Set Hardware reset",
	CMD_MFG_LOAD_LCD_IMAGE,				//  0x00a117 load LCD image
 
	//	Manufacturing commands // Mfg commands
	CMD_MFG_SEPARATOR0,                  //  MM - "            ",
	CMD_MFG_SEPARATOR,                   //  MM - "------MFG Commands-------------------------------------------", 
	CMD_MFG_CALIBRATE_GAMMA,             //  MM - MFG: Calibrate Gamma Table",
	CMD_MFG_CALIB_768LINE_ALIGN_PORTx,   //  MM - MFG: Calibrate all ports 768 vs GHz
	CMD_MFG_CALIB_1024_GHz,		         //  MM - MFG: Calibrate all ports 1024 vs GHz
	
	CMD_MFG_CALIB_1024_GRAY,			 //	 MM	- MFG: Calibrate all ports 1024_Gray vs Attn
	CMD_MFG_RUN_CALIBRATION,			 //	 MM - MFG: Run all Ports Calibrate
	CMD_MFG_TEST_EO_CURVE,				 //	 MM - MFG: Test special GHz EO curve
	CMD_MFG_AUTO_TEST,                   //  MM - MFG: Auto test all temperature
	CMD_MFG_AUTO_CALIBRATION_TEST,       //72#  MM - MFG: Auto  CALIBRATION and test all temperature
	
	CMD_MFG_AUTO_TEST_PORTISO,			 //  MM - MFG: Auto test Port Isolation
	
	CMD_MFG_OPM_ADC_CORRECTION_CALI,     //added by ris

	CMD_MFG_RENAME_FILE,			     //	 MM	- MFG: Rename Files
	
 
	 
	CMD_MFG_ATTN_VERIFY,				 //	 MM - MFG: All Ports Attenuation Verify
	CMD_MFG_GET_PROFILE,                 //  MM - MFG: Save Profile,
	CMD_MFG_CREATE_PARA_FILE,			 //	 MM	- MFG: Create Para_info_file
	CMD_MFG_MAP138_UPDATE,				 //  MM - MFG: MAP138 firmware update
	CMD_MFG_DOWNLOAD_LUT,                //  MM - MFG: download LUT(mapping, Atten gray LUT)
    CMD_MFG_GET_CHAMBER_T,               //  MM - MFG:  Read Chamber T,
	CMD_MFG_SET_CHAMBER_T,               //  MM - MFG:  Set Chamber T,
	CMD_MFG_TEC_CONTINUE_TEST,
	CMD_MFG_PIXELSET_TEST,
	CMD_MFG_READ_I2C_ADDRESS,
	CMD_MFG_SET_I2C_ADDRESS,
	CMD_MFG_SET_TESTER_MODE,
	CMD_MFG_SET_SOFTWARE_RESET,
	CMD_MFG_READ_SENSOR_TEMPERATURE,
	CMD_MFG_VCOMRIPPLE_TEST,
	CMD_MFG_CONVERT_PN_TEST,
	CMD_MFG_GET_CHANNEL_POWER,
	CMD_COUNT,
}; 

//switch port/block  
enum 
{
	SWITCH_PORT_0,
	SWITCH_PORT_1,
	SWITCH_PORT_2,
	SWITCH_PORT_3,
	SWITCH_PORT_4,
	SWITCH_PORT_5,
	SWITCH_PORT_6,
	SWITCH_PORT_7,
	SWITCH_PORT_8,
	SWITCH_PORT_COUNT,
}; 


typedef enum
{
	CALI_TEMP_OFFSET=0,
	TEST_TEMP_OFFSET,
	DOWNLOAD_TEMP,
	ALL_TEMP_TYPE
}TEMP_TYPE;

typedef enum
{
	IL_TEST=0,
	DOWNLOAD_MAPPING,
	DOWNLOAD_MFG_INFO,
	DOWNLOAD_PARAMETER,
	WAIT_SENSOR_TEMP,
	CALIBRATE_DGE_WAVELENGTH, //模块DAC不一定是MinIL的DAC 
	CALIBRATE_DGE_ATTENUATION,//第一次衰减定标：粗调
	CALIBRATE_DGE_ATTEN_FINE, //第二次衰减定标：细调
	CALIBRATE_DGE_WAVE_FINE, //第二次波长定标： 设置MinIL的DAC
	CALIBRATE_DGE_ATTEN_LUT_OPTIMIZATION, //平均所有pixel同一衰减的DAC值	
	CALIBRATE_DGE_IL,
	CALIBRATE_DGE_BEAM_OFFSET,
	CALIBRATE_DGE_PIXELBEAM,
	DOWNLOAD_WAVE_CALIBRATION,
	DOWNLOAD_ATTEN_CALIBRATION,
	DOWNLOAD_ATTEN_DAC_OFFSET, //by luke.liu 2018.3.28
	DOWNLOAD_IL_CALIBRATION,
	DOWNLOAD_PIXELBEAM_CALIBRATION,
	TEST_DGE_ATTEN_VER,
	TEST_DGE_FUNCTION,
	TEST_DGE_WLSHIFT,
	TEST_0DAC_IL,
	FQC_FOR_DGE,
	STOP_CHAMBER,
	TEST_ITEM_COUNT,
} ACTION;

typedef enum MANUTABPOS
{
	POS_MI_SN = 0,
	POS_MI_PN,
	POS_MI_MODTYPE,
	POS_MI_HV,
	POS_MI_HD,
	POS_MI_END
}MOD_INFO_POS;
typedef enum MODTABPARAMPOS
{
	POS_MP_DAC = 0,
	POS_MP_STARTFREQ = POS_MP_DAC + 1,
	POS_MP_SENS1PARAM = POS_MP_STARTFREQ + 1,
	POS_MP_SENS2PARAM = POS_MP_SENS1PARAM + 4,
	POS_MP_SENS1THRES = POS_MP_SENS2PARAM + 4,
	POS_MP_SENS2THRES = POS_MP_SENS1THRES + 2,
	POS_MP_COMN = POS_MP_SENS2THRES + 2,
	POS_MP_COMP = POS_MP_COMN + 1,
	POS_MP_RGBPN = POS_MP_COMP + 1,
	POS_MP_SENS3PARAM = POS_MP_RGBPN + 48,
	POS_MP_TECTARK = POS_MP_SENS3PARAM + 4,
	POS_MP_TECTARC = POS_MP_TECTARK + 1,
	POS_MP_COLOFFK = POS_MP_TECTARC + 1,
	POS_MP_COLOFFC = POS_MP_COLOFFK + 1,
	POS_MP_CALITEMP = POS_MP_COLOFFC + 1,
	POS_MP_COLOFFPARA1 = POS_MP_CALITEMP + 5,
	POS_MP_COLOFFPARA2 = POS_MP_COLOFFPARA1 + 1,
	POS_MP_COLOFFPARA3 = POS_MP_COLOFFPARA2 + 1,
	POS_MP_COLOFFPARA4 = POS_MP_COLOFFPARA3 + 1,
	POS_MP_END = POS_MP_COLOFFPARA4 + 1
}MOD_PARAM_POS;
typedef enum
{
	VALUE_TYPE_CHAR = 0,
	VALUE_TYPE_INT,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_STRING
}VALUE_TYPE;
#define LENGTH_VALUE_BUF		(64)
#define AMP_10000				(10000)
#define AMP_1000				(1000)
#define AMP_100					(100)
#define AMP_10					(10.00)
#define YEAR_CENTURY_NOW		(2000)


const ULONG m_aulCRCTable[0x100] =
{
    0x0000000,  0x77073096, 0xEE0E612C, 0x990951BA,
		0x76DC419,  0x706AF48F, 0xE963A535, 0x9E6495A3,
		0xEDB8832,  0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x9B64C2B,  0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
		0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
		0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
		0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
		0x76DC4190, 0x1DB7106,  0x98D220BC, 0xEFD5102A,
		0x71B18589, 0x6B6B51F,  0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0xF00F934,  0x9609A88E, 0xE10E9818,
		0x7F6A0DBB, 0x86D3D2D,  0x91646C97, 0xE6635C01,
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
		0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
		0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
		0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
		0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
		0xEDB88320, 0x9ABFB3B6, 0x3B6E20C,  0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x4DB2615,  0x73DC1683,
		0xE3630B12, 0x94643B84, 0xD6D6A3E,  0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0xA00AE27,  0x7D079EB1,
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
		0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
		0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
		0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
		0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x26D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x5005713,
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0xCB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0xBDBDF21,
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
		0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
		0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
		0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
		0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

#endif  //_DEFINITION_0123456789102345678_H_INCLUDED_



