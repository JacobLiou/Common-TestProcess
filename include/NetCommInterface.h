#ifndef _NETCOMMANDINTERFACE_H
#define _NETCOMMANDINTERFACE_H

#include "DataSrc.h"
#include "TPUseXMLDataType.h"

class CNetCommandInterface
{
public:
	virtual TCHAR* GetLastError(TCHAR** lpszErr = NULL) = 0;
	virtual BOOL XmlSend(ALL_MSG_DATA& data) = 0;
	virtual BOOL XmlRecv(ALL_MSG_DATA& data) = 0;
	virtual DWORD XmlShowBox(LPCTSTR lpText, LPCTSTR lpCaption = NULL, DWORD dwType = XSB_OK) = 0;
	virtual BOOL XmlSendLog(LPCTSTR lpstLog, int iLength, LOGLEVEL eLevel = LOG_NORMAL) = 0;
	virtual BOOL XmlTaskDone(int iTaskIndex, LPCTSTR lpstUserID, TASKRESULT eState = TASK_DONE) = 0;
	virtual BOOL XmlStopTask(void) = 0;
	virtual BOOL XmlTaskInfo(LPCTSTR lpstIndex, LPCTSTR lpstTitle) = 0;
	virtual BOOL XmlBaseInfo(LPCTSTR lpstAuthor, LPCTSTR lpstVersion, LPCTSTR lpstSpec, \
		LPCTSTR lpstPN, LPCTSTR lpstDate,LPCTSTR lpstProcessType) = 0;
	virtual BOOL XmlTaskStatus(pstTaskInfoPack pstTaskInfo) = 0;
	virtual BOOL XmlTaskDescrip(int iTaskIndex, LPCTSTR lpstDescrip, int iLength) = 0;
	virtual BOOL XmlGraphInit(stGraphInitInfo& stInfo) = 0;
	virtual BOOL XmlGraphData(stGraphDataInfo& stInfo) = 0;
	virtual BOOL XmlSendProgressData(int iFullStep , int iCurrStep) = 0;
	virtual BOOL XmlSendProcessResult(stProcessDataInfo& stProcessResult) =0;
	// add 2020
	virtual BOOL XmlGetConfigPath(TCHAR* lpszTestDataPath,int iDataPathLen, TCHAR* lpszConfigPath,int iConfigPathLen, TCHAR* lpszFWFilePath, int iFWFilePathLen) = 0;
	virtual BOOL XmlDownloadTemplate(LPCTSTR lpProdSN, LPCTSTR lpProcess, LPCTSTR lpFileNme, int iShowData) = 0;
	virtual BOOL XmlDownloadTaskList(LPCTSTR lpProdSN, LPCTSTR lpGroup, LPCTSTR lpPosition, TCHAR* lpszSubSN, int iSubSNBufLen) = 0;
	virtual BOOL XmlUploadTestData(LPCTSTR lpProdSN, LPCTSTR lpDataFileName) = 0;
	// add 2021
	virtual BOOL XmlTriggerConverProcess(LPCTSTR lpProdPN, LPCTSTR lpOriginProcess, TCHAR* lpszTargetProcess, int iBufferLen, int iFindType)=0 ;
	virtual BOOL XmlTriggerConversationPN(LPCTSTR lpOriginPN, TCHAR* lpszTargetPN, int iBufferLen, int iCType) = 0;
	virtual BOOL XmlGetStationInfo(LPCTSTR lpStationComputerName, TCHAR* lpszStationID, TCHAR* lpszStationName, int iBufferLen) = 0;
	virtual BOOL XmlTriggerTestResultUpload2OPC(LPCTSTR lpProdSN) = 0;
	virtual BOOL XmlTriggerMoveIn(LPCTSTR lpProdSN) = 0;
	virtual BOOL XmlTriggerMoveOut(LPCTSTR lpProdSN) = 0;
	// add 2021 09
	virtual BOOL XmlTriggerMIMSIDLoginVerify(LPCTSTR lpUserID, LPCTSTR lpPassWord) = 0;
	virtual BOOL XmlTriggerWorkStationVerify(LPCTSTR lpWorkStationID, LPCTSTR lpUserID, LPCTSTR lpCheckType) = 0;
	virtual BOOL XmlGetOplinkUserInfo(LPCTSTR lpOplinkID, TCHAR* ptchrUserName, TCHAR* ptchrDptName, TCHAR* ptchrPosName, TCHAR* ptchrOPCID, int iBufferLen) = 0;
	virtual BOOL XmlTriggerUserSkillVerify(LPCTSTR lpOPCID, LPCTSTR lpProcess) = 0;
	virtual BOOL XmlCreateSNDir(LPCTSTR lpDutSN, LPCTSTR lpRootDir, TCHAR* ptchrOutSNDir, int iBufferLen) = 0;
	virtual BOOL XmlGetFWStandardPath(LPCTSTR lpDutPN, TCHAR* ptchrOutFWPN, TCHAR* ptchrOutConfigDir, int iBufferLen) = 0;
	// add 202308
	//获取标签信息
	virtual BOOL XmlGetPrintLabelInfo(LPCTSTR lpDutSN, LPCTSTR lpLableID, LPCTSTR lpPosition, TCHAR* ptchrOutContent, int iBufferLen) = 0;
	//上传归零时间到TMS
	virtual BOOL XmlUploadTestSystemCailbrationTime(LPCTSTR lpUserID)=0;
	//获取location 信息
	virtual BOOL XmlGetLocationInfo(TCHAR* ptchrOutLanguge, TCHAR* ptchrOutLocation, TCHAR* ptchrOutFactory, int iBufferLen)=0;
	//20231024 获取MAC 地址
	virtual BOOL XmlGetMACAddress(LPCTSTR lpDutSN, LPCTSTR lpDutPN, int nMACCount, TCHAR* ptchrOutContent, int iBufferLen) = 0;
	//20231024 获取 无源光路理论IL 值
	virtual BOOL XmlGetRoutingTheoryIL(LPCTSTR lpDutSN, LPCTSTR lpDutPortName, LPCTSTR lpWavelength, double* pdbOutTheoryIL, double* pdbOutTheoryILWSplice) = 0;
public:
};

#endif