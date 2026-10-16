// NetCommand.h: interface for the CNetCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETCOMMAND_H__997DD977_045E_4B68_8905_8630684A1F22__INCLUDED_)
#define AFX_NETCOMMAND_H__997DD977_045E_4B68_8905_8630684A1F22__INCLUDED_

//XML head file 
#include "../include/NetCommInterface.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetCommand : public CNetCommandInterface
{
	//Function
public:
	CNetCommand();
	virtual ~CNetCommand();
	virtual TCHAR* GetLastError(TCHAR** lpszErr = NULL);
	virtual BOOL XmlSend(ALL_MSG_DATA& data);
	virtual BOOL XmlRecv(ALL_MSG_DATA& data);
	virtual DWORD XmlShowBox(LPCTSTR lpText, LPCTSTR lpCaption = NULL, DWORD dwType = XSB_OK);
	virtual BOOL XmlSendLog(LPCTSTR lpstLog, int iLength, LOGLEVEL eLevel = LOG_NORMAL);
	virtual BOOL XmlTaskDone(int iTaskIndex, LPCTSTR lpstUserID, TASKRESULT eState = TASK_DONE);
	virtual BOOL XmlStopTask(void) { return m_bStopTest; };
	virtual BOOL XmlTaskInfo(LPCTSTR lpstIndex, LPCTSTR lpstTitle);
	virtual BOOL XmlBaseInfo(LPCTSTR lpstAuthor, LPCTSTR lpstVersion, LPCTSTR lpstSpec, \
		LPCTSTR lpstPN, LPCTSTR lpstDate,LPCTSTR lpstProcessType);
	virtual BOOL XmlTaskStatus(pstTaskInfoPack pstTaskInfo);
	virtual BOOL XmlTaskDescrip(int iTaskIndex, LPCTSTR lpstDescrip, int iLength);
	virtual BOOL XmlGraphInit(stGraphInitInfo& stInfo);
	virtual BOOL XmlGraphData(stGraphDataInfo& stInfo);
	virtual BOOL XmlSendProgressData(int iFullStep , int iCurrStep);
	virtual BOOL XmlSendProcessResult(stProcessDataInfo& stProcessResult);

	// add 2020
	virtual BOOL XmlGetConfigPath(TCHAR* lpszTestDataPath, int iDataPathLen, TCHAR* lpszConfigPath, int iConfigPathLen, TCHAR* lpszFWFilePath, int iFWFilePathLen);
	virtual BOOL XmlDownloadTemplate(LPCTSTR lpProdSN, LPCTSTR lpProcess, LPCTSTR lpFileNme, int iShowData);
	virtual BOOL XmlDownloadTaskList(LPCTSTR lpProdSN, LPCTSTR lpGroup, LPCTSTR lpPosition, TCHAR* lpszSubSN, int iSubSNBufLen);
	virtual BOOL XmlUploadTestData(LPCTSTR lpProdSN, LPCTSTR lpDataFileName);
	// add 2021
	virtual BOOL XmlTriggerConverProcess(LPCTSTR lpProdPN, LPCTSTR lpOriginProcess, TCHAR* lpszTargetProcess, int iBufferLen, int iFindType);
	virtual BOOL XmlTriggerConversationPN(LPCTSTR lpOriginPN, TCHAR* lpszTargetPN, int iBufferLen, int iCType);
	virtual BOOL XmlGetStationInfo(LPCTSTR lpStationComputerName, TCHAR* lpszStationID, TCHAR* lpszStationName, int iBufferLen);
	virtual BOOL XmlTriggerTestResultUpload2OPC(LPCTSTR lpProdSN);
	virtual BOOL XmlTriggerMoveIn(LPCTSTR lpProdSN);
	virtual BOOL XmlTriggerMoveOut(LPCTSTR lpProdSN);
	// add 2021 09
	virtual BOOL XmlTriggerMIMSIDLoginVerify(LPCTSTR lpUserID, LPCTSTR lpPassWord);
	virtual BOOL XmlTriggerWorkStationVerify(LPCTSTR lpWorkStationID, LPCTSTR lpUserID, LPCTSTR lpCheckType);
	virtual BOOL XmlGetOplinkUserInfo(LPCTSTR lpOplinkID, TCHAR* ptchrUserName, TCHAR* ptchrDptName, TCHAR* ptchrPosName, TCHAR* ptchrOPCID, int iBufferLen);
	virtual BOOL XmlTriggerUserSkillVerify(LPCTSTR lpOPCID, LPCTSTR lpProcess);
	virtual BOOL XmlCreateSNDir(LPCTSTR lpDutSN, LPCTSTR lpRootDir, TCHAR* ptchrOutSNDir, int iBufferLen);
	virtual BOOL XmlGetFWStandardPath(LPCTSTR lpDutPN, TCHAR* ptchrOutFWPN, TCHAR* ptchrOutConfigDir, int iBufferLen);

	//add 2023/9/4
	virtual BOOL XmlLocation(TCHAR* ptchrLanguage, TCHAR* ptchrLocation, TCHAR* ptchrFactory, int iBufferLen);
	virtual BOOL XmlGenericFunc(LPCSTR lpstrFuncName, int nInputCount, TCHAR*pInputParam[10], int nOutputCount, char* pOutputResult[10], int iBufferLen);

	// add 202308
	//获取标签信息
	virtual BOOL XmlGetPrintLabelInfo(LPCTSTR lpDutSN, LPCTSTR lpLableID, LPCTSTR lpPosition, TCHAR* ptchrOutContent, int iBufferLen);
	//上传归零时间到TMS
	virtual BOOL XmlUploadTestSystemCailbrationTime(LPCTSTR lpUserID);
	//获取location 信息
	virtual BOOL XmlGetLocationInfo(TCHAR* ptchrOutLanguge, TCHAR* ptchrOutLocation, TCHAR* ptchrOutFactory, int iBufferLen);
	//20231024 获取MAC 地址
	virtual BOOL XmlGetMACAddress(LPCTSTR lpDutSN, LPCTSTR lpDutPN, int nMACCount, TCHAR* ptchrOutContent, int iBufferLen);
	//20231024 获取 无源光路理论IL 值
	virtual BOOL XmlGetRoutingTheoryIL(LPCTSTR lpDutSN, LPCTSTR lpDutPortName, LPCTSTR lpWavelength, double* pdbOutTheoryIL, double* pdbOutTheoryILWSplice);


public:
	BOOL IsOk(void) { return m_bOk; };
	void Initialize(void);
	void XmlFeedback(ALL_MSG_DATA* pstUUISendInfo);
	void XmlSetStop(void) { m_bStopTest = TRUE; };
	void XmlResetStop(void) { m_bStopTest = FALSE; };

private:
	BOOL m_bOk, m_bStopTest;
	DWORD m_dwFeedback, m_dwMsgBoxID;
	HANDLE m_hFeedEvent;
	HINSTANCE m_hInst, m_hXmlInst;
	//段保护句柄
	CRITICAL_SECTION m_cs;
	//错误信息
	CString m_strErr;
	CHAR m_pszBuf[LENGTH_40K];
	// add 2020 
	ALL_MSG_DATA* m_pstUUISendInfo;

public:
	
	
	//Socket
	GetLastErr getLastError;
	StartSrv startSrv;
	StopSrv stopSrv;
	AcqMsgCount acqMsgCount;
	AcqClientCount acqClientCount;
	RecvMsg recvMsg;
	SendMsg sendMsg;
	SendMsgTo sendMsgTo;
	//XML
	ParseXMLStr xmlResolve;
	CreateXMLStr xmlCreate;
	GetXMLErrStr xmlGetError;
	CreateXMLGraphDataStr xmlGraphData;
};

#endif // !defined(AFX_NETCOMMAND_H__997DD977_045E_4B68_8905_8630684A1F22__INCLUDED_)
