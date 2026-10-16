#pragma once

#include "StdAfx.h"
#include"..\\include\UDL.FSTP.h"
#include "MyselfDef.h"

class CFSTPScan
{
public:
	CFSTPScan();
	~CFSTPScan();

	BOOL OpenFSTPDevice(LPCSTR lpstrConfig);
	BOOL DisConnectFSTP();
	CString GetErrorMsg();
	BOOL PrepareScan(long lchIndex,stFSTPScanParam stParam);
	void GetScanParam(pFSTPScanParam pParam);
	BOOL ExcuteScan(long lchIndex, pFSTPScanResult pResult);
	BOOL GetFSTPSCanData(LPCSTR lpstrFileName, pFSTPScanResult pResult);
	BOOL SaveFSTPScanResult(LPCSTR lpstrFileName, stFSTPScanResult pResult);
	BOOL AllocScanStruct(pFSTPScanResult pResult);
	BOOL FreeScanStruct(pFSTPScanResult pResult);
	BOOL DoReference(long lChIndex);
	BOOL DoTLSScaning(long lChIndex,BOOL bDoRef= FALSE);
	BOOL ReadReferenceDataFromFile();
	BOOL ReadCirculatorRefDataFromFile(int nType);
	BOOL CalculateCirculatorRefData();

	stFSTPScanResult  m_stScanResult;
	stFSTPScanResult  m_stScanRefResult;
	stFSTPScanResult  m_stCirculator1;
	stFSTPScanResult  m_stCirculator2;
	stFSTPScanResult  m_stCirculator;
	CString m_strRefFileName;
	CString m_strScanFileName;
	CString m_strCirculatorName;
	int m_nGUID;
	stFSTPScanParam m_stScan;
	long m_lDeviceIndex;

private:
	CString m_strErrorMsg;
	
	
};

