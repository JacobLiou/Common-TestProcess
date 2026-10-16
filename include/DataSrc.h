#ifndef _DATA_SRC_H_
#define _DATA_SRC_H_

#include <WINSOCK.H>

//Length define
#define LENGTH_2			(2)
#define LENGTH_3			(3)
#define LENGTH_4			(4)
#define LENGTH_5			(5)
#define LENGTH_10			(10)
#define LENGTH_12			(12)
#define LENGTH_16			(16)
#define LENGTH_20			(20)
#define LENGTH_32			(32)
#define LENGTH_64			(64)
#define LENGTH_96			(96)
#define LENGTH_256			(256)
#define LENGTH_512			(512)
#define LENGTH_1K			(1024)
#define LENGTH_2K			(LENGTH_1K * 2)
#define LENGTH_4K			(LENGTH_1K * 4)
#define LENGTH_10K			(LENGTH_1K * 10)
#define LENGTH_40K			(LENGTH_1K * 40)

//Data
#define SPEED_LIGHT				(299791962.0f)
#define STEP_POW_DAC_INC		(0.04)
#define STEP_POW_ASE_INC		(0.08)
#define ATT_MAX					(50.0)
#define ADDR_PUMP_DAC_OFF		(0x2000)
#define ADDR_PUMP_ASE_OFF		(0x800)
#define THRESHOLD_OFFSET_35		(35)
#define THRESHOLD_OFFSET_30		(30)
#define THRESHOLD_OFFSET_25		(25)

//Folder and File define
#define PATH_ROOT			(".\\LocalFiles")
#define PATH_DLG_ROOT		("CaliDlg")
#define PATH_DATA_ROOT		("Data")
#define PATH_CFG_ROOT		("Config")
#define PATH_LOG_ROOT		("Log")

#define FILE_DATA_CALI		("Calibrate")
#define FILE_DATA_MIDGAIN	("ImitateCaliData")
#define FILE_DATA_VERI		("Verify")
#define FILE_DATA_LUT		("Lut")

#define FILE_DATA_CALI_CH	("ChData")
#define FILE_DATA_CALI_GT	("GainTiltChData")
#define FILE_DATA_CALI_VT	("VOATempChData")
#define FILE_DATA_CALI_PR	("PreCaliData")

#define FILE_DATA_LUT_PUMP	("Pump_DAC_ASE")
#define FILE_DATA_LUT_BIN	("BIN")

#define NAME_DATA_VOACALI	("VOACaliData.ini")
#define NAME_DATA_PUMPSPEC	("PumpPreCalib")

#define NAME_TP				("TpStart.exe")
#define NAME_DLG			("EDFATestSystem.exe")
#define NAME_CFG_BND		("module_bundle.cfg")
#define NAME_CFG_PN			("PN_Config.ini")
#define NAME_CFG_TP			("TPConfig.ini")
#define NAME_CFG_DEV		("DeviceConfig.ini")
#define NAME_CFG_SYS		("SystemConfig.ini")
#define NAME_CFG_DIRECT		("DataDirectory.ini")
#define NAME_CFG_BASEINFO	("BasicInfoCfg.ini")
#define NAME_CFG_CALI		("CalibrateConfig.ini")
#define NAME_CFG_DEVPARAM	("DeviceParamConfig.ini")
#define NAME_CFG_PDPARAM	("DevicePdParamConfig.ini")
#define NAME_CFG_DOWNLOAD	("DownloadConfig.ini")
#define NAME_CFG_GAINCALI	("GainAseConfig-Calib.ini")
#define NAME_CFG_GAINVERI	("GainAseConfig-Verify.ini")
#define NAME_CFG_GAINSET	("GainConfig.ini")
#define NAME_CFG_GAINNF		("GAIN-NFCriteria.ini")
#define NAME_CFG_PDSETCALI	("PDCfg-Calibrate.ini")
#define NAME_CFG_PDSETVERI	("PDCfg-Verify.ini")
#define NAME_CFG_PRODUCT	("ProductCfg.ini")
#define NAME_CFG_PUMPPRE	("PumpPreCalibCfg.ini")
#define NAME_CFG_VOASETCALI	("VOACfg-Calibrate.ini")
#define NAME_CFG_VOASETVERI	("VOACfg-Verify.ini")
#define NAME_CFG_REF		("RefData.ini")
#define NAME_CFG_TASK		("TaskMenu.xml")

//Baud rate
#define BAUDRATE_075		(75)
#define BAUDRATE_110		(110)
#define BAUDRATE_134        (134)
#define BAUDRATE_150        (150)
#define BAUDRATE_300        (300)
#define BAUDRATE_600		(600)
#define BAUDRATE_1200		(1200)
#define BAUDRATE_1800		(1800)
#define BAUDRATE_2400		(2400)
#define BAUDRATE_4800		(4800)
#define BAUDRATE_7200		(7200)
#define BAUDRATE_9600		(9600)
#define BAUDRATE_14400		(14400)
#define BAUDRATE_19200		(19200)
#define BAUDRATE_38400		(38400)
#define BAUDRATE_56K		(56000)
#define BAUDRATE_57600		(57600)
#define BAUDRATE_115200		(115200)
#define BAUDRATE_128K		(128000)



//Special words
#define NULL_DWORD			(0xFFFFFFFFL)
#define NULL_WORD			(0xFFFF)
#define HIGH_DWORD			(0xFFFF0000L)
#define LOW_DWORD			(0x0000FFFFL)
#define FLOAT_0DOT5			(0.5)
#define AMP_10				(10)
#define AMP_100				(100)
#define END_SQC				(1)
#define DISCONN_SQC			(NULL_DWORD)

//Time definition
#define TIME_100MS			(100)
#define TIME_200MS			(200)
#define TIME_300MS			(300)
#define TIME_500MS			(500)
#define TIME_1MIN			(60 * 1000)
#define TIME_5MIN			(TIME_1MIN * 5)
#define TIME_50MIN			(TIME_1MIN * 50)
#define TIME_8HOUR			(8)

//Message box define
#define XSB_NONE			(0x00000000L)

#define XSB_OK				(0x00000001L)
#define XSB_YES				(0x00000002L)
#define XSB_NO				(0x00000004L)
#define XSB_IGNORE			(0x00000008L)
#define XSB_RETRY			(0x00000010L)
#define XSB_ABORT			(0x00000020L)
#define XSB_CANCEL			(0x00000040L)

#define XSB_ICONERROR		(0x00020000L)
#define XSB_ICONWARNING		(0x00040000L)
#define XSB_ICONINFORMATION (0x00060000L)
#define XSB_ICONQUESTION    (0x00080000L)

//Port&IP define
#define IP_DEFAULT			("127.0.0.1")
#define PORT_BASE			(37291)

//Words define
#define WORDS_FEEDBACK		("FeedBack")

//Language define
#define UTHROW(T)			(throw _T(T))
#define DBM2MW(T)			(pow(10, (T)/10))
#define THROWT(T)			(throw _T(T))

#define BYTE_TO_WORD(usTarget,ucHigh,ucLow) \
	do{usTarget  = (USHORT)(ucHigh) << 8;  \
	usTarget |= (USHORT)(ucLow);}while(0)

#define WORD_TO_BYTE(ucTargetHigh,ucTargetLow,usWORD) \
	do{ucTargetHigh  = (BYTE)((usWORD) >> 8);  \
	ucTargetLow = (BYTE)(usWORD);}while(0)

#define BYTE_TO_DWORD(dwTarget,ucHigher,ucHigh,ucLow,ucLower) \
	do{dwTarget  = (DWORD)(ucHigher) << 24;  \
    dwTarget |= (DWORD)(ucHigh) << 16;	\
	dwTarget |= (DWORD)(ucLow) << 8;	\
	dwTarget |= (DWORD)(ucLower);}while(0)

#define DWORD_TO_BYTE(ucTargetHigher,ucTargetHigh,ucTargetLow,ucTargetLower, dwDWORD) \
	do{ucTargetHigher = (BYTE)((dwDWORD) >> 24);  \
    ucTargetHigh = (BYTE)((dwDWORD) >> 16);	\
	ucTargetLow = (BYTE)((dwDWORD) >> 8);	\
	ucTargetLower = (BYTE)(dwDWORD);}while(0)

//enum
enum CONNTYPE
{
	CONN_TP = 0,
	CONN_GUI,
	CONN_DRV
};

enum LOGLEVEL
{
	LOG_NORMAL = 0,
	LOG_WARNING,
	LOG_ERROR
};

enum GRAPHOPT
{
	GRAPH_OPT_NEW = 0,
	GRAPH_OPT_ADDPLOT,
	GRAPH_OPT_ADDDATA,
	GRAPH_OPT_DELPLOT
};

enum TASKRESULT
{
	TASK_START = 0,
	TASK_STOP,
	TASK_FAIL,
	TASK_DONE
};

enum TASKRANGETYPE
{
	TASK_RG_CLASS = 0,
	TASK_RG_PARENT,
	TASK_RG_CHILD,
	TASK_RG_OFFSET
};

enum TASKRANGENUM
{
	TASK_RG_NUM1 = 10000,
	TASK_RG_NUM2 = 20000,
	TASK_RG_NUM3 = 30000,
	TASK_RG_NUM4 = 40000,
	TASK_RG_NUM5 = 50000,
	TASK_RG_NUM6 = 60000,
	TASK_RG_NUM7 = 70000,
	TASK_RG_NUM8 = 80000,
	TASK_RG_NUM9 = 90000
};

enum TASKOFFSET
{
	TASK_OFF_0 = 0,
	TASK_OFF_1000 = 10000,
	TASK_OFF_2000 = 20000,
	TASK_OFF_3000 = 30000,
	TASK_OFF_4000 = 40000,
	TASK_OFF_5000 = 50000,
	TASK_OFF_6000 = 60000,
	TASK_OFF_7000 = 70000,
	TASK_OFF_8000 = 80000,
	TASK_OFF_9000 = 90000,
};

enum TASKINFOKEY
{
///// 线圈测试
	ROOM_25_WOI07_B_BLUE = 1,				//1
	ROOM_25_WOI03_MODULE2,					//2
	ROOM_25_WOI03_MODULE3,					//3
	ROOM_25_WOI03_MODULE4,
	ROOM_25_WOI03_UPIL_MODULE1,
	ROOM_25_WOI03_UPIL_MODULE2,
	ROOM_25_WOI03_UPIL_MODULE3,
	ROOM_25_WOI03_UPIL_MODULE4,

///// 干涉计组装测试
	ROOM_25_WOI06_MODULE1,					
	ROOM_25_WOI06_MODULE2,					
	ROOM_25_WOI06_MODULE3,	
	ROOM_25_WOI06_MODULE4,
	ROOM_25_WOI06_MODULE5,
	ROOM_25_WOI06_MODULE6,
	ROOM_25_WOI06_MODULE7,
	ROOM_25_WOI06_MODULE8,
	ROOM_25_WOI06_UPIL_MODULE1,
	ROOM_25_WOI06_UPIL_MODULE2,
	ROOM_25_WOI06_UPIL_MODULE3,
	ROOM_25_WOI06_UPIL_MODULE4,
	ROOM_25_WOI06_UPIL_MODULE5,
	ROOM_25_WOI06_UPIL_MODULE6,
	ROOM_25_WOI06_UPIL_MODULE7,
	ROOM_25_WOI06_UPIL_MODULE8,


///// 干涉计盘盒测试
	ROOM_25_WOI08_MODULE1,					//
	ROOM_25_WOI08_MODULE2,					//
	ROOM_25_WOI08_MODULE3,					//
	ROOM_25_WOI08_MODULE4,
	ROOM_25_WOI08_MODULE5,
	ROOM_25_WOI08_MODULE6,
	ROOM_25_WOI08_MODULE7,
	ROOM_25_WOI08_MODULE8,
	
	ROOM_25_WOI08_UPIL_MODULE1,
	ROOM_25_WOI08_UPIL_MODULE2,
	ROOM_25_WOI08_UPIL_MODULE3,
	ROOM_25_WOI08_UPIL_MODULE4,
	ROOM_25_WOI08_UPIL_MODULE5,
	ROOM_25_WOI08_UPIL_MODULE6,
	ROOM_25_WOI08_UPIL_MODULE7,
	ROOM_25_WOI08_UPIL_MODULE8,
};

enum KEY_ITEM
{
	TEST_INIT  = 99,
	DEVICE_REF,
	TEST_REF,
};
enum CALI_POSITION
{
	POS_NULL = -1,//无
	POS_0,			// 盘盒 VOA定标
	POS_1,			// 增益预定标
	POS_2,			// 增益定标及PD定标
	POS_3,			// 常温验证，增益及PD
	POS_4,			// 高低温验证，主要为增益验证
	POS_5,			// 未启用
	POS_6			// 未启用
};
typedef enum SOCKACT
{
	MIMS_NONE = 0,
	//Command
	MIMS_COM_RUNTASK,
	MIMS_COM_RUNDEV,
	MIMS_COM_STOPRUN,
	MIMS_COM_DEBUGMOD,
	//Message
	MIMS_MSG_BASEINFO,
	MIMS_MSG_TASKINFO,
	MIMS_MSG_SHOWBOX,
	MIMS_MSG_FEEDBACK,
	MIMS_MSG_LOG,
	//Data
	MIMS_DAT_PROCESS,
	MIMS_DAT_PROGRESS,
	MIMS_DAT_GRAPH,
	//Setting
	MIMS_SET_SYSTEM,

	//Quit TP
	MIMS_QUIT
}eSockAct;

//Socket initialize information
typedef struct STSOCKINITINFO
{	
	//Socket client type
	int iConnType;
	//Server IP, NULL means the IP is 127.0.0.1
	TCHAR* pszSrvIP;
	
}stSockInfo, *pstSockInfo;

//Receive head message structure
typedef struct STCOMMHEAD
{
	//Sequence number
	DWORD dwSequence;
	//Connect client address
	SOCKADDR_IN addr;
	//Connect client type
	int iConnType;
	//Message length
	DWORD dwMsgLens;
}stCommHead, *pstCommHead;

//Receive message structure
typedef struct STCOMMMSG
{
	//The head information of message
	stCommHead stHead;
	//Buffer of the message
	TCHAR buf[LENGTH_10K];
}stCommMsg, *pstCatchMsg;

//Catch message structure
typedef struct STCOMMCATCH
{
	//The head information of message
	stCommHead stHead;
	//Pointer of the catch to receive volatile length of the message
	TCHAR* cat;
}stCommCatch, *pstCommCatch;

//Socket structure
typedef struct STTHREADINFO
{
	//Socket handle
	LPVOID lpSocket;
	//socket address to send to
	LPVOID lpSocketAddr;
	//CWnd based pointer
	LPVOID lpDlg;
}stThreadInfo, *pstThreadInfo;

//Test items ID
typedef struct STTASKITEMSID
{
	TASKRANGETYPE type;//Item type
	int	iID;//Item ID

	TCHAR pszProcessTypeList[LENGTH_64];//生效的无纸化步骤列表
	TCHAR pszPnList[LENGTH_64];//生效的PN列表
////////////End ///////////////////////////////////
	TCHAR pszName[LENGTH_64];//Item Name
	TCHAR pszDecription[LENGTH_512];//Item description
}stTaskItemsID, *pstTaskItemsID;

//One test task status
typedef struct STONETASKSTATUS
{
	int iID;
	int iStatus;
}stOneTaskStatus, *pstOneTaskStatus;
//Task information packet
//typedef struct STINFOGRAPH
//{
//	CString  X_strTemp;//X轴名称
//	CString  Y_strTemp;//Y轴名称
//	double   X_dbTemp;//X轴尺度
//	double   Y_dbTemp;//Y轴尺度
//}stInfoGraph, *pstInfoGraph;
//Task information packet
typedef struct STTASKINFOPACK
{
	int iTaskCount;
	pstOneTaskStatus pstAllTaskState;
}stTaskInfoPack, *pstTaskInfoPack;

//Graphic
//X or Y scale detail information
typedef struct STSCALEINFO
{
	BOOL bScaleUse;
	TCHAR pszScaleName[LENGTH_64];
	BOOL bScaleAuto;
	double dblScaleStart;
	double dblScaleEnd;
}stScaleInfo, *pstScaleInfo;

//Graphic initialize information 
typedef struct STGRAPHINITINFO
{
	int iGraphIndex;
	TCHAR pszTitle[LENGTH_64];
	stScaleInfo stScale[LENGTH_3];
}stGraphInitInfo, *pstGraphInitInfo;

//Graphic data information 
typedef struct STGRAPHDATAINFO
{
	GRAPHOPT opt;
	int iGraphIndex;
	int iPlotIndex;
	TCHAR pszPlotTitle[LENGTH_64];
	int iColor;
	int iYScaleIndex;
	int iDataCount;
	double* pdblXData;
	double* pdblYData;
}stGraphDataInfo, *pstGraphDataInfo;

typedef struct STPROGRESSDATAINFO
{
	int iFullStep;
	int iStep;
}stProgressDataInfo , *pstProgressDataInfo;
typedef struct STPROCESSDATAINFO 
{
	int		iDataID;
	TCHAR	pszOpLine[LENGTH_256];
	TCHAR	pszDataName[LENGTH_256];
	double	dbDataResult;
	double	dbDataRangeUp;
	double	dbDataRangeLow;
	
}stProcessDataInfo , *pstProcessDataInfo;

class CCfgFileInterface;
class CNetCommandInterface;
class CDeviceOptInterface;

//Function define
typedef void (*GetDllVer)(LPVOID pszVersion, LPVOID pszCompileDate);
typedef LPVOID (*GetLastErr)(LPVOID* lpszErr);
typedef BOOL (*StartSrv)(stSockInfo stInitSock);
typedef BOOL (*StopSrv)(void);
typedef int (*AcqMsgCount)(void);
typedef int (*AcqClientCount)(void);
typedef BOOL (*RecvMsg)(LPVOID lpszMsg, DWORD& dwLength);
typedef BOOL (*SendMsg)(LPVOID lpszMsg, DWORD dwLength);
typedef BOOL (*SendMsgTo)(LPVOID lpszMsg, DWORD dwLength, int iTarType);

//XML function
typedef BOOL (*ParseXMLStr)(BSTR, LPVOID); //宏定义解析函数指针类型
typedef BOOL (*CreateXMLStr)(LPVOID, BSTR*);
typedef	BOOL (*GetXMLErrStr)(BSTR*);//
typedef BOOL (*CreateXMLGraphDataStr)(PVOID pInData ,double* pdbGraphX,double* pdbGraphY,int nDataCount, BSTR* pbstrGraphOutString);

#endif