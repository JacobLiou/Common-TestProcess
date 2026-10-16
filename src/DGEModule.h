#pragma once
#include "ProcessMsg.h"
#include "Calpolyn.h"
#include "FSTPScan.h"


typedef struct tagDGEModuleInfo
{
	CString strPN;
	CString strSN;
	CString strHWDate;
	CString strHWVersion;
	int nModuleType;
	int nDevcieTTNum;
	int nCal_Template_Rev;
	int nTest_Template_Rev;
	int nPixcelCount;
	int nModuleVersion;
	int nFirstSliceFreq;
	int nSliceStep;
	int nLastSliceFreq;
	double dbStartWL;
	double dbStartFreq;
	double dbEndWL;
	double dbEndFreq;


}stDGEModuleInfo,*pDGEModuleInfo;


typedef struct tagDGETemplateInfo
{
	CString strDGETestCfgFolder;
	CString strCalTemplateFolder;
	CString strTestTemplateFolder;
	CString strMFGInfoTemplate;
	CString strDutParamTemplat;

}stDGETemplateInfo,*pDGETemplateInfo;



class CDGEModule
{
public:
	CDGEModule();
	~CDGEModule();

	BOOL GetSensorTemp(int nIndex, WORD* pwADC, double* pdbGetSensorTemp);

	BOOL GetModuleMCUTemp(double * pdbMCUTemp);
	BOOL GetModuleSerialNO(CString &strSN);
	BOOL DownloadUpdateFile(LPCSTR lpstrImageBinfile);
	BOOL SetSoftWareReset();
	BOOL fnCreateMappingTab(LPCSTR lpstrBinFile);
	BOOL fnReadMappingCfgCsv(LPCTSTR lpstPath, BYTE *pbyChannel, int *pnLine);
	ULONG CalcCRC32(ULONG ulInitCRC, UCHAR* uszDataBuf, ULONG ulDataLen);
	BOOL CreateBinFile(LPCTSTR lpstPath, BYTE* pBuf, int iLength);
	BOOL BundleBinFile(DWORD dwDataLength, LPCTSTR lpstFilePath, DWORD dwAddress);
	BOOL fnCreateMfgInfoTab(LPCSTR lpstrBinFile);
	BOOL CopyInformationData();
	BOOL GetModuleDeviceType();
	BOOL fnCreateDutParaTab(LPCSTR lpstrBinFile);
	CString ReadIniFileString(LPCTSTR lpstPath, CString strSection, CString strKey);
	BOOL fnCreateWaveTab(LPCSTR lpstrBinFile);
	BOOL fnCreateAttenTab(LPCSTR lpstrBinFile);
	BOOL IsTheFileExist(LPCTSTR lpstrFileName);
	BOOL fnReadAttenLut256Csv(LPCTSTR lpstPath, double *pdblDac, int *pnLine);
	BOOL fnCreateAttenDacOffsetTab(LPCSTR lpstrBinFile);
	BOOL SaveAttenLut256Csv(LPCTSTR lpstPath, double *pdblDac);
	BOOL CalcConvolutionFilter();
	BOOL fnReadPixelWaveIni(LPCTSTR lpstPath, stPixelInfo *pstPixel, BOOL bCal);
	BOOL ReadILRatio_Beam(LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblPixelWave, double *pdblPixelGHz, double *pdblGetIL, double *pdblRatio1, double *pdbRatio2, int *pnTotalLength);
	BOOL HandleAttenDACOffsetByConvolution(double *pdbOffDac, int nLenth);
	BOOL ReadConvolutionFilter();
	BOOL fnConvolution(double *pdbSource, int nSourceLength, double *pdbOut);
	BOOL fnCreateILTab(LPCSTR lpstrBinFile);
	BOOL fnCreateBeamTab(LPCSTR lpstrBinFile);
	BOOL fnReadILCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength);
	BOOL LinearInsertValue(double *pdblXArray, double *pdblYArray, BOOL bXAxisIncrease, DWORD dwDataCount, double dblX, double *pdblY);
	BOOL fnReadBeamCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, double *pdblRatio1, double *pdblRatio2, int *nLength);
	BOOL fnReadAttenCaliCfg();
	BOOL fnSetAllPixelDAC(WORD *wDac);
	BOOL fnSetPixelGroupDAC(int nloop, stPixelOpen *stPixel, BOOL bOn = TRUE);
	BOOL fnSetOECurve(float *pfCurveFreq, float *pfCurveIL, WORD nPoint);
	BOOL fnSetAllPixelAtten(float *pfAtten, int nChannelNum);
	BOOL fnGetCurTemperIL(double *pdblIL, double* pdblFre, int *nPoints);
	BOOL fnGetCurTemperFreq(float *fFreq, int *nPoints);
	BOOL fnGetCurTemperDac(WORD *wDac, int nChannel);
	BOOL fnSetDgeOECurve(const WORD wCurvePoint, float *pfCurveFreq, float *pfCurveIL);
	BOOL fnReadAttenVerCfg(int &nSetNumber);
	BOOL fnReadAttenVerCfgFromTemplate(LPCTSTR lpstPath, float *pfAtten, int *pnLine);
	BOOL fnSetAllPixelAttenForVer(int nLoopIndex);
	BOOL fnSetCustomMode();
	BOOL fnReadOECurveCfgFile(LPCTSTR lpstPath, float *pCurveGhz, float *pCurveIL, int *pnPoint);
	BOOL AdjustFileExit(LPCSTR lpstrFileName);
	BOOL fnCreatReportFile(LPCTSTR lpstPath);
	BOOL fnSaveILScanData();
	BOOL fnReadChScanFile(LPCTSTR lpstPath, double *dblIL, double *dblPDL, double *dblGhz=NULL);
	BOOL fnSaveReportFile(LPCTSTR lpstPath, CString strIndex, double *dblData, DWORD dwScanCount);
	BOOL fnSetDgeOECurveForVer(int nLoopIndex);
	BOOL ReadCurveCountCfgFromFile();
	BOOL fnSaveCurvePDLScanData();
	BOOL fnCalcCurveFlatness(int nIndex, double *pdblIL, double *pdblActualIL, double *pdblFitGhz, double *pdblFitIL, double *pdblAccuracy);
	BOOL fnSavedCurveScanVer();
	BOOL fnReadDGEWLShiftVerifyCfg();
	BOOL ReadDGEWLShiftVerifyCfgFromFile(LPCTSTR strFileName, pDGEWLShiftVerCfg pGetParam, int *npLineCount);
	BOOL fnSaveDGEWLShiftVer();
	BOOL fnSave0VDACScanData();
	BOOL fnSumary0DACScanDataReport();
	void ClearCurveScanVerMemory();
	BOOL fnCalcPixcelCenterGHzNew(double *pdbFre, double *pdbIL, int nSampleCount, double *pdbPixelWLCenter, double *pdbPixelCenterIL, int *nGetPixelCount);
	BOOL fnSaveAttenCalScanData();
	BOOL fnReadPixelMatrixCfg();
	BOOL fnReadAttenLut256Csv(double *pdblDac, int *pnLine);
	BOOL CopyDefaultCenterWLTemplate();
	BOOL fnSaveFineAttenCalScanData();
	BOOL SearchPixel_New2(double *pdblPixelWave, double *pdblPixelGhz, double *pdblGetIL, int* PixelCount, int nLoopIndex);
	BOOL ReadScanDatatFromFile(LPCSTR lpstrFile, double*pdbWL, double*pdbFreq, double *pdbIL, int* pnGetCount);
	BOOL fnCalLineCenterWL(stPixelInfo *pstPixel);
	BOOL OpenSerialPort();
public:
	double m_pdbFilterArry[64][3];
	CCalpolyn    m_calPolyn;
	CString m_strCaliFolder;
	stConfigTable m_stConfigTable;
	CString m_strTempFolder;
	CString  m_strDataRoot;
	stWaveTable       m_stWaveTab;
	stAttenTable      m_stAttenTab;
	stDACOffsetTable  m_stDacOffsetTab;
	stILTable         m_stILTab;
	stBeamTable       m_stBeamTab;
	stModuleParameter m_stDevParaTab;
	int  m_iPNNewDeviceNmb;
	int	 m_MappingNew[32];
	stMessgetable	m_stMessageInfo;
	stManuTable		m_stModuleInfo;
	stDGETemplateInfo  m_DGETemplateInfo;
	stPixelMapping    m_stMappingTab;
	stDGEModuleInfo m_DGEInfo;
	int    m_nModuleIndex;
	CProcessMsg  m_Module;
	CString		m_strLogFileName;
	CString     m_strErrorMsg;
	BOOL   m_bConnect;
	double m_dbCalTemp;

	stPixelInfo  m_stPixelInfo;
	CString m_strTestPath;
	CString  m_strComPort;
	int  m_nAnologDGEIndex;
	int m_nCurveTemplateNum;
	float   m_fAttenCfg[PIXEL_NUMBER_96];
	float   m_fTargetCurveFreq[PIXEL_NUMBER_64][IL_LUT_POINT];
	float   m_fTargetCurveIL[PIXEL_NUMBER_64][IL_LUT_POINT];
	int     m_nTargetCurvePoint[PIXEL_NUMBER_64];
	stFSTPScanParam m_stScanParameter;
	stDGEWLShiftVerCfg   m_stWLShiftCfg[PIXEL_NUMBER_96];
	int  m_nCfgWLShiftLineCount;
	int  m_nPixelMatrix[PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL][PIXEL_SET_INTERVAL];
	int  m_nPixeGroupg;
	stPixelAtten m_stPixelDac;
	int m_nAttenCfgNumber;
	CString m_strCurrentTemp;
};

