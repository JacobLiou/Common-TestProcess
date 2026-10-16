// 211GTester.h: interface for the CTestTaskManager class.
//
//////////////////////////////////////////////////////////////////////
//#include "Datatype.h"
#include "GraphPro.h"
#include "NetCommand.h"
#include "TPInfoManager.h"
#include "OpCRC32.h"
#include "XMLControl.h"
#include "OpComm.h"
#include "OpCommModBus.h"
#include "Calpolyn.h"
#include <afxdisp.h>
#include "DGEModule.h"
#include "MyselfDef.h"
#include "BasicClass.h"
#include "FSTPScan.h"
#include "SwitchCtrl.h"

#if !defined(AFX_211GTESTER_H__A5022C69_5027_4754_BFDC_F1D47EBEB79F__INCLUDED_)
#define AFX_211GTESTER_H__A5022C69_5027_4754_BFDC_F1D47EBEB79F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Color definition
#define COLOR_BLACK			RGB(0, 0, 0)
#define COLOR_BLUE			RGB(0, 0, 255)
#define COLOR_GREEN			RGB(0, 255, 0)
#define COLOR_RED			RGB(255, 0, 0)
#define COLOR_YELLOW		RGB(255, 255, 0)
#define COLOR_LIGHT_BLUE	RGB(0, 255, 255)
#define COLOR_PINK			RGB(255, 0, 255)
#define COLOR_WHITE			RGB(255, 255, 255)
// 公用宏定义
#define	BIT(x)				(DWORD)(0x00000001 << x)

// PD定标/验证相关定义
#define	PD_CONFIG_NONE					0
#define	PD_CONFIG_OUTPUT				1
#define	PD_CONFIG_INPUT					2
#define MAX_DOWN_DATA					32768 //32k
#define PD_LUT_SIZE						30
#define PD_LUT_MIN_PWR					-36
#define PD_LUT_MAX_PWR					22
#define PD_LUT_STEP						2
#define MAX_PD_CAL_SAMPLE				700
// 测试项目
#define	DOWNLOADDEFAULTPARAM     		0  //下载默认参数
#define	INPUTPDCAL     					1  //Input PD定标
#define	OUTPCCAL					    2  //Output PD定标
#define	ASECAL    						3  //ASE定标
#define	PDLutDOWNLOAD    				4  //PD数据下载
#define	ASELUTDOWNLOAD    				5  //ASE数据下载
#define	PD_IN_PUMPOFF_VERIFY   		    6  //IN端PD Pump关闭的验证
#define	PD_IN_PUMPON_VERIFY				7  //IN端PD Pump开启设置150mA下验证
#define	PD_OUT_VERIFY			        8  //OUT端PD验证
#define	Gain_ROOM_VERIFY_1       		9  //-5.5dBm输入—8dBm输出增益验证
#define	Gain_ROOM_VERIFY_2   			10 //-5.5dBm输入—2dBm输出增益验证
#define	Gain_ROOM_VERIFY_3       	    11 //-5dBm输入—8dBm输出增益验证
#define	Gain_ROOM_VERIFY_4       	    12 //-2dBm输入—-2dBm输出增益验证
#define	POWER_ROOM_STABLE       	    13 //常温输出功率稳定性验证
#define	Gain_LOW_VERIFY_1       	    14 //-5.5dBm输入—8dBm输出增益验证
#define	Gain_LOW_VERIFY_2       	    15 //-5.5dBm输入—2dBm输出增益验证
#define	Gain_LOW_VERIFY_3       	    16 //-5dBm输入—8dBm输出增益验证
#define	Gain_LOW_VERIFY_4       	    17 //-2dBm输入—-2dBm输出增益验证
#define	Gain_LOW_VERIFY_5       	    18 //2dBm输入—2dBm输出增益验证
#define	POWER_LOW_STABLE       			19 //低温输出功率稳定性验证
#define	Gain_HIGH_VERIFY_1       	    20 //-5.5dBm输入—8dBm输出增益验证
#define	Gain_HIGH_VERIFY_2   			21 //-5.5dBm输入—2dBm输出增益验证
#define	Gain_HIGH_VERIFY_3       	    22 //-5dBm输入—8dBm输出增益验证
#define	Gain_HIGH_VERIFY_4       	    23 //-2dBm输入—-2dBm输出增益验证
#define	Gain_HIGH_VERIFY_5       	    24 //2dBm输入—2dBm输出增益验证
#define	POWER_HIGH_STABLE       		25 //高温输出功率稳定性验证
#define	PUMP_HIGH_MARGIN	       		26 //高温Pump Margin验证

//温度个数
#define TEMPT_COUNT 3
//
//========================================================================//
// 系统校正相关结构体定义
typedef struct tagRefConfigure
{
	BOOL    m_bRefDone; 	
	BYTE	m_bCalibChanCount;
	LONG	m_lCalibTimeOutHour;
	int     nWLCount;
	double  dbIL1MarginMin;
	double  dbIL1MarginMax;
	double  dbIL2MarginMin;
	double  dbIL2MarginMax;
	double	m_pdblLDWavelength[100];
}stRefConfigure,*pRefConfigure;
//通道配制
typedef struct tagPortPathConfigure //最多支持64
{	
	int			nSetSW1[64];//1x64开关1
	int			nSetSW2[64];//1X64开关2
	CString		m_pszMsgName[64];	//通道名
}stPortPathConfigure,*pPortPathConfigure;
//归零IL结果结构体
typedef struct tagSystemRefResult
{
	double dbSW1TapMonIL[100][64];
	double dbSW2OutputIL[100][64];
}stSystemRefResult,*pSystemRefResult;

//BYTE g_byDownloadBuf[40];

#define	UDATEFLAG			            0x00DD
#define	WRITEFWDATA			            0x00DE
#define	READFWUPDATASTATUS			    0x00FE
#define	ARMNEWFWIMAGE  			        0x00FF

#define CONV_PN_OPLINK_MOLEX        1      //从oplink PN转Molex PN
#define CONV_PN_MOLEX_OPLINK        2      // 从molex PN 转 oplink PN
#define CONV_PROCESS_MES_TEST       1      // 从MES 工序转内部工序
#define CONV_PROCESS_TEST_MES       2      // 从内部工序转MES工序


class CTestTaskManager
{
public:
	static UINT DoShowLogThread(LPVOID lpParam);
	BOOL DownLoadTemplate();
	BOOL DoWithPDLStresstesting();
	BOOL DoWithPDLScanReference();
	BOOL DoNoPDLStresstesting();
	BOOL DoWithPDLScanTest();
	BOOL DoNoPDLScanTest();
	BOOL DoNoPDLScanReference();
	BOOL DoRBeforeTestCheck(int nTaskIndex);
	BOOL JudgeCurrentSlotSN(int nModuleIndex);
	BOOL RunTestInit();
	BOOL RunTestTask(int iTaskIndex);
	BOOL RunDefaultTask(int nDefaultTaskIndex);
	BOOL InitGraphShow(int nGraphIndex, CString X_strTemp, CString Y_strTemp,CString strGraphTitle);
	void SetGraphPro(CGraphPro* pGraph);
	void SetTPInfoManager(CTPInfoManager* pInfoManger);
	void SetNetCommand(CNetCommand *pNetCommand);
	BOOL SetFileByMSMQ(CString strFile, CString &strErr);
	BOOL UploadTestResult(int iDataID, CString strOptContInfo, CString strDataName,double dblMinRngData, double dblMaxRngData, double dblResult);
	void LogInfo(CString strLogInfo, int nLength, BYTE bLogType = LOG_NORMAL, int nModuleIndex=0);
	void CreateFullPath(CString strPath);
	BOOL InitialConfigSystemParam();
	BOOL CreateProductionDataDirectory(int nModuleIndex,LPCSTR lpstrRootFolder, LPCSTR lpstrPN, LPCSTR lpstrSN);
	BOOL CreatSeverDirectory();
	BOOL SystemReference(void);
	BOOL CheckRefValide(CString strSection, BYTE bIndex);

	void RecordStopTestTime();
	void RecordStartTestTime();
	void InitialParam();
	BOOL FindFileExist(LPCTSTR lpstrFileFullName);
	CString LoadIniDataString(CString strFilePath,LPCTSTR strSection, LPCTSTR szID);
	BOOL CalculateLinearADC(double dbPower,double * pdbPowerArry,int * npADC,int nPointCount,int *nCalADC);
	BOOL OpenTestOpticDevice();
	BOOL LinearFitArray(double *pdblX, double *pdblY, int nCount, double &k, double &c);
	BOOL GetIniFileString(CString strSelection, CString strKeyWork, CString strFileName,  CString* pstRspMsg);
	BOOL OpenSerialPortDevice();
	BOOL ConnectDut(int n_Com,CString strComPort); //开关盒、Tester板串口驱动函数
    BOOL SetTLSWavelength(double m_pdblLDWavelength);
	BOOL CheckGoldenSample(CString GoldenSampleSN, CString TestSN);	
	BOOL EndFWUpdate();
	BOOL ArmNewFWImage();
	BOOL CheckFWUpdateStatus();
	BOOL FWTranSportFW(PBYTE byDownloadBuf);
	BOOL StartFWUpdate();
	WORD SwapWORD(WORD wInData);
	DWORD SwapDWORD(DWORD dwInData);
	BOOL OpenASEComPort(CString strComPort);
	
	double Sum(double *pdblArray, int nCount);	
	//循环箱
	void TemperatureIndex(int index);
	BOOL RefreshModuleTemperature();
	BOOL ControlChamberQuickly(stSettingTemp stChamberSetTemp);
	BOOL AdjustFileExit(LPCSTR lpstrFileName);

	////////////////////////////////////////////////////////
	BOOL DoFSILClient(int nTaskIndex);
	BOOL DoFSILPDLClient(int nTaskIndex);

///////////////////////测试前函数检查/////////////////////////////////////

	BOOL DoRefOpticalCheck();
	BOOL DoCirculator12Check();
	BOOL DoCirculator23Check();
	BOOL DoProductionSlotCheck();
	BOOL DoProductionOpticalCheck();

/////////////////////////定标函数////////////////////////////////////////
	BOOL DownloadProductionInfo(int nTaskIndex);
	BOOL DGECalibrationFor30C(int nTaskIndex);
	BOOL DGECalibrationForF5C(int nTaskIndex);
	BOOL DGECalibrationFor5C(int nTaskIndex);
	BOOL DGECalibrationFor15C(int nTaskIndex);
	BOOL DGECalibrationFor50C(int nTaskIndex);
	BOOL DGECalibrationFor70C(int nTaskIndex);
	BOOL DownloadCalLut(int nTaskIndex);

	BOOL DGEVerifyFor70C(int nTaskIndex);
	BOOL DGEVerifyFor55C(int nTaskIndex);
	BOOL DGEVerifyFor40C(int nTaskIndex);
	BOOL DGEVerifyFor25C(int nTaskIndex);
	BOOL DGEVerifyForF5C(int nTaskIndex);

	BOOL ConnectLocalEquipment();
	BOOL ConnectChamber();
	BOOL ConnectSwitchBox();

	BOOL fnDownloadLutTables(int nLutType);
	BOOL WaitChamberCycle(stSettingTemp stCfg);
	BOOL fnDownloadMappingTable();
	BOOL fnDownloadMfgInfo();
	BOOL fnDownloadParamInfo();
	BOOL fnDownloadWavCalibrationTable();
	BOOL fnDownloadAttenCalibrationTable();
	BOOL fnDownloadAttenOffsetTable();
	BOOL fnDownloadILCalibartionTable();
	BOOL fnDownloadBeamCalibartionTable();



	//BOOL fnCalPixelCenterWave(double dbTemp);
	BOOL fnCal_PixelAtten();
	void fnSleepLongTime(DWORD dwSleep_ms);

	BOOL fnGetTemperWriteIni(int nModuleIndex, double &dblTemp, LPCTSTR lpName, BOOL bCalibrate);
	BOOL fnReadAttenVsGhz(LPCTSTR lpstPath, DWORD dwScanCount, float *pfDac, double *pdblAtten, int *pnLine, double *pdblGhz);
	BOOL fnReadPixelWaveIni(LPCTSTR lpstPath, stPixelInfo *pstPixel, BOOL bCal);
	BOOL SaveCaliPixelAtten(LPCTSTR lpstPath, int nLine, double *pdblPixelGhz, float *pdwDac, double *pdblScanGhz, double *pdblAtten);

	BOOL fnTestScanOne();
	BOOL ReadScanConfig();
	BOOL fnPixelAttenScan(WORD wDacSetting, CString strName, int nType);
	BOOL SaveScanAttenData(LPCTSTR lpstPath, DWORD dwChannelCount, float fData);
	BOOL fnCalFined_PixelAtten();
	BOOL fnCalFined_PixelCenterWave();
	BOOL fnPixelCenterWaveScan(int nLoop);
	BOOL SaveScanILData(LPCTSTR lpstPath, DWORD dwChannelCount, DWORD dwSampleCount);
	BOOL fnSavePixelWaveIni(LPCTSTR lpstPath, stPixelInfo stPixel_info,int nMoudleIndex);
	BOOL fnCal_AttenLutAfterFindedPixelCenter();
	BOOL fnCalcAttenVsDac(int nModuleIndex, double *pdblPixelGhz, double *pdblScanGHz, double *pdblAtten, float *pdwDac, int nLine);

	BOOL fnSavePixelWaveIniNew(LPCTSTR lpstPath, stPixelInfo stPixel_info);
	BOOL fnSavePixelWaveCsvFile(LPCTSTR lpstPath, stPixelInfo stPixel_info);
	double fnLineCalc(double x1, double x2, double y1, double y2, double XX);
	BOOL fnCalcAttenVsDac_AveByPolynomial(int nModuleIndex);
	BOOL SaveAttenVsDac(LPCSTR lpstrFile, double *pdbDAC, int nPixelCount);
	BOOL fnCal_PixelIL();
	BOOL fnPixelILScan();
	BOOL fnCalOffset_PixelCenterWave();
	BOOL fnPixelWaveOffsetScan(int nLoop);
	BOOL fnCalcPixelOffset(int nModuleIndex);
	BOOL fnReadILCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength);
	BOOL SaveCalcILOffset(LPCTSTR lpstPath, double *pdblPixelWave, double *pdblPixelGhz, double *pdblIL);
	BOOL fnCal_PixelBeam();
	BOOL SearchPixelMinIL(LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength);

	BOOL HandleGroupPixelMinIL(int nModuleIndex,int nGoupIndex, double dbPixelCenterWL, double *pdblWave, double *pdblGhz, double *pdblIL);
	BOOL fnHandlePixelChanelIL(int nModuleIndex, int nGroupIndex, double dbCenterWL, double *pdblWave, double *pdblGhz, double *pdblIL);
	BOOL LinearInsertValue(double *pdblXArray, double *pdblYArray, BOOL bXAxisIncrease, DWORD dwDataCount, double dblX, double *pdblY);
	BOOL SearchPixelChannelIL(int nModuleIndex,LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength);
	BOOL HandleRitioData(int nCenterDeta, int nConitueGrouth, double dbSetValue, double* dbHandleRatio, int nRatioLength);
	BOOL SaveILRatio_Beam(LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblWave, double *pdblGhz, double *pdblIL, double *pdblRatio1, double *pdblRatio2 /* = NULL */);
	BOOL fnReadBeamCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, double *pdblRatio1, double *pdblRatio2, int *nLength);
	
	BOOL fnVer_PixelAtten();

	BOOL fnGetCurDataAndSave(int nModuleIndex, CString strItem);
	
	BOOL fnCreatReportFile(LPCTSTR lpstPath);
	BOOL fnSaveReportFile(int nModuleIndex, LPCTSTR lpstPath, CString strIndex, double *dblData, DWORD dwScanCount);
	BOOL fnSavePixelData(int nModuleIndex);
	BOOL fnReadPixelWaveCsv(LPCTSTR lpstPath, stPixelInfo *pstPixel);
	BOOL fnSaveReportFile(int nModuleIndex,LPCTSTR lpstPath, BOOL bPDL, int nPixelCount, float *pfSetAtten, double *pdblAtten, int nLine, double *pdblGhz);
	BOOL fnSaveScanChFiles(int nModuleIndex, CString strTargetFileName);
	BOOL fnReadChScanFile(int nModuleIndex, LPCTSTR lpstPath, double *dblIL, double *dblPDL, double *dblGhz = NULL);
	BOOL SaveCurTemperIL(LPCTSTR lpstPath, int nPoints, double *pdblIL, double *pdblFre);
	BOOL SaveCurTemperFreq(LPCTSTR lpstPath, int nLine, float *pfFreq);
	BOOL SaveCurTemperDac(LPCTSTR lpstPath, int nChannels, WORD *wDac);
	BOOL fnVer_OECurve();
	BOOL fnOECurveScan(int nIndex);

	//BOOL SearchPixel_New2(LPCTSTR lpstPath, double *pdblPixelWave, double *pdblPixelGhz, double *pdblGetIL, int* PixelCount, int nLoopIndex);
	//BOOL SearchPixel_New(LPCTSTR lpstPath, double *pdblPixelWave, double *pdblPixelGhz, double *pdblGetIL, int* PixelCount);
	//BOOL ReadScanDatatFromFile(LPCSTR lpstrFile, double*pdbWL, double*pdbFreq, double *pdbIL, int* pnGetCount);
	BOOL fnSaveVerWLShiftData(int nModuleIndex);
	BOOL fnCalcPixcelCenterGHz(double *pdbFre, double *pdbIL, int nSampleCount, double *pdbPixelWLCenter, double *pdbPixelCenterIL, int *nGetPixelCount);
//	BOOL fnCalcPixcelCenterGHzNew(double *pdbFre, double *pdbIL, int nSampleCount, double *pdbPixelWLCenter, double *pdbPixelCenterIL, int *nGetPixelCount);
	BOOL fnVer_WLShift();
	BOOL fnAllPixelAttenScan(LPCTSTR lpstPath, float *fAtten);
	BOOL fnVer_0DACScanData();

	BOOL SwitchBoxByPort(int nPort, int nMoudleIndex);
	BOOL ReadProductionInfo(int nModuleIndex);
	void SetDGEModuleCurrentTemp(double dbSetValue);
	BOOL CreateAttenScanVerFile(LPCSTR lpstrFileName);
	BOOL CreateEOCurveScanVerFile(LPCSTR lpstrFileName);
	void AddPlotCount(int nGraphIndex,LPCSTR lpstrCaption);
	void Bubble_SortDacVSAtten(float *pfDAC, double *pdblAtten, int nCount, int nAttenScanPointCount);
	int MySort(float *pfDAC, double *pdblAtten, int nCount, int nScanCont);
	BOOL fnSaveAttenVsGhz(LPCTSTR lpstPath, int nScanCount, float *pfDac, double *pdblAtten, int nLine);
	BOOL fnCalcAttenVsDac_Ave(int nModuleIndex, double *pdblPixelGhz, double *pdblScanGHz, double *pdblAtten, float *pfDac, int nLine);
	BOOL ReadProductionTestingInfo(int nModuleIndex);
	BOOL Circulator12Reference();
	BOOL Circulator23Reference();
	BOOL CombineCirculatorRef();
	BOOL DoCirculatorRef();
	CTestTaskManager();
	virtual ~CTestTaskManager();

protected:
	//////////////// 连接测试/////////////////
	CString m_strRunTaskTemp;
	/////// 测试用户名 ///////////////////
	CString m_strRunTaskUserName;
	/////////////////////////////////////////////////
	CTPInfoManager* m_pInfoManger;
	CNetCommand*	m_pNetCommand;
	// 图形操作类
	CGraphPro*		m_pGraph;
	stGraphInitInfo m_Graphinfo;


//	stInfoGraph     InitGraph;
	CDGEModule     m_DGEModule[TEST_MODULE_COUNT];
	int            m_nDGECount;
	int            m_nChamberOnline;
	int            m_nSwitchBoxOnline;
	BOOL           m_bChamberOn;
	BOOL           m_bSwithBoxOn;
	CBasicClass    m_GernalObj;
	CString m_strCaliTemlateFolder;
	CString m_strTestTemplateDir;
	//DWORD m_dwScanCount;
	BOOL m_bRefDone;
	double m_dbStartFre;
	double m_dbStopFre;
	stSettingTemp   m_stCfg;
	int nGetSNCount;


private:
	CStringArray m_strSNArray;
	CStringArray m_strReadSNArray;
	// 文件路径
	TCHAR			m_tszAppFolder [MAX_PATH];	// 当前运行路径
	CString			m_strSysCfgFilePath;		// 系统配置文件名
	CString			m_strServerFloder;			// 服务器（网络文件夹或本地）文件路径（不含PN）
	CString         m_strLoacalFloder;          // 本地路径路径文件夹
	CString			m_strTestDataDir;			// 测试数据存放目录（含PN）
	CString			m_strCfgFileName;			// 产品配置信息路径
	CString         m_strConfigPath;            // 系统配置路径

	BOOL			m_bOpenTestDevice;          // 是否连接光学测试设备   
	stPortPathConfigure m_strPortPath;          //光开关盒路由配置结构体，最大支持64个通道
	

	stSystemRefResult m_stSystemRef;
	COpCRC32		m_CRC;				//CRC

	// 温度循环箱控制相关
	COpCommModBus	m_cChamber;
	CCalpolyn  m_Calpolyn; //计算斜率截距

	CSwitchBoxCtrl  m_switchctrl;

	CString m_strPN;
	CString m_strSN;
	BOOL			m_bServerOn;
	BOOL			m_bStartTestTime;
	CString			m_strStartTestTime;
	CString			m_strStopTestTime;
	double			dbGoldenSampleValue[32];	
	BOOL			dbGoldenSampleDo[32];
	CFSTPScan       m_FSTPObj;
	stFSTPScanParam m_stScanParameter;
	int m_nStreestestingTime;//
	CString m_strFSTPCfg;

		
};

#endif // !defined(AFX_211GTESTER_H__A5022C69_5027_4754_BFDC_F1D47EBEB79F__INCLUDED_)