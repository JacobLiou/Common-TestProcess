#include "stdafx.h"
#include "FSTPScan.h"

#pragma comment(lib,"..\\lib\\UDL.FSTP.lib")


CFSTPScan::CFSTPScan()
{
	m_stScan.dblStartWL = 1525.0;
	m_stScan.dblStepSize = 0.005;
	m_stScan.dblStopWL = 1570;
	m_stScan.nPMCount = 0;
	m_stScan.dwSampleCount = 0;

	m_stScanResult.pdbWLArry = NULL;
	m_stScanRefResult.pdbWLArry = NULL;
	m_stScanResult.pdbTapResult = NULL;
	m_stScanRefResult.pdbTapResult = NULL;
	for (int i = 0;i < MAX_PM_COUNT;i++)
	{
		m_stScanResult.pdbPDLResult[i] = NULL;
		m_stScanResult.pdbPMResult[i] = NULL;
		m_stScanRefResult.pdbPDLResult[i] = NULL;
		m_stScanRefResult.pdbPMResult[i] = NULL;
	}
}


CFSTPScan::~CFSTPScan()
{
	FreeScanStruct(&m_stScanResult);
	FreeScanStruct(&m_stScanRefResult);
	FreeScanStruct(&m_stCirculator1);
	FreeScanStruct(&m_stCirculator2);
	FreeScanStruct(&m_stCirculator);
	DisConnectFSTP();
}

CString CFSTPScan::GetErrorMsg()
{
	return m_strErrorMsg;
}
///////////////////////////////////////////////////////
//函数说明：
//   连接FSTP服务器端
//Add by wanxin
//Time：20200615
//////////////////////////////////////////////////////
BOOL CFSTPScan::OpenFSTPDevice(LPCSTR lpstrConfigXML)
{
	long lDeviceIndex = 0;
	long hdlDevice;
	char szTempBuf[MAX_PATH];
	char* token = NULL;
	char szSpec[] = ",";
	CString strConfig;
	CString strMsg;
	CString strServerIP;
	CString strServerPort;
	
	ZeroMemory(szTempBuf, sizeof(char)*MAX_PATH);
	sprintf(szTempBuf, "%s", lpstrConfigXML);
	token = strtok(szTempBuf, szSpec);//IP
	strServerIP = token;
	token = strtok(NULL, szSpec);//Port
	strServerPort = token;

	strConfig.Format("FSTP-1XXX:LAN,0,%s,%s,NA,NA", strServerIP, strServerPort);

	CHAR pstrDeviceConfig[256] = "";
	strcpy_s(pstrDeviceConfig, 256, strConfig);
	if (OpenFSTP(lDeviceIndex, pstrDeviceConfig, &m_lDeviceIndex) != S_OK)
	{
		m_strErrorMsg.Format("%s", GetFSTPLastErrorMessage());
		AfxMessageBox(m_strErrorMsg);
		return FALSE;
	}

	return TRUE;
}


BOOL CFSTPScan::DisConnectFSTP()
{

	::CoUninitialize();
	return TRUE;
}

///////////////////////////////////////////////////////
//函数说明：
//   设置FSTP扫描参数
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////
BOOL CFSTPScan::PrepareScan(long lchIndex, stFSTPScanParam stParam)
{
	HRESULT hr = S_OK;
	long *plPMChArry = NULL;
	double *pdbPMLevel = NULL;
	char szBuffer[MAX_PATH];
	
	m_stScan = stParam;
	//m_stScan.bDoPDL = stParam.bDoPDL;
	//m_stScan.bCirculator = stParam.bCirculator;
	//m_stScan.nPMCount = stParam.nPMCount;
	//m_stScan.dblStartWL = stParam.dblStartWL;
	//m_stScan.dblStepSize = stParam.dblStepSize;
	//m_stScan.dblStopWL = stParam.dblStopWL;
	//m_stScan.dwSampleCount = stParam.dwSampleCount;

	hr = SetFSTPParameters(lchIndex, m_stScan.dblStartWL, m_stScan.dblStopWL, m_stScan.dblStepSize);
	if (hr == S_FALSE)
	{
		m_strErrorMsg.Format("%s", GetFSTPLastErrorMessage());
		return FALSE;
	}

	if (pdbPMLevel == NULL)
	{
		pdbPMLevel = (double*)VirtualAlloc(NULL, sizeof(long)*m_stScan.nPMCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}
	ZeroMemory(pdbPMLevel, sizeof(double)*m_stScan.nPMCount);

	if (plPMChArry == NULL)
	{
		plPMChArry = (long*)VirtualAlloc(NULL, sizeof(long)*m_stScan.nPMCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}
	ZeroMemory(plPMChArry, sizeof(long)*m_stScan.nPMCount);
	for (int i = 0;i < m_stScan.nPMCount;i++)
	{
		plPMChArry[i] = i+1;
		pdbPMLevel[i] = m_stScan.dbPMLevel;
	}
	hr = SetAllPMParameters(lchIndex, 0, NULL, pdbPMLevel, 0, m_stScan.nPMCount, plPMChArry);
	if (hr == S_FALSE)
	{
		m_strErrorMsg.Format("%s", GetFSTPLastErrorMessage());
		return FALSE;
	}

	if (pdbPMLevel != NULL)
	{
		VirtualFree(plPMChArry, 0, MEM_RELEASE);
	}
	if (plPMChArry != NULL)
	{
		VirtualFree(plPMChArry, 0, MEM_RELEASE);
	}
	return TRUE;
}
////////////////////////////////////////////////////
//函数说明：
//   获取扫描设置参数
//Add by wanxin
//Time:20200615
///////////////////////////////////////////////////
void CFSTPScan::GetScanParam(pFSTPScanParam pParam)
{
	pParam = &m_stScan;
}
////////////////////////////////////////////////////////
//函数说明：
//   执行FSTP扫描
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////
BOOL CFSTPScan::ExcuteScan(long lchIndex, pFSTPScanResult pResult)
{
	HRESULT hr = S_OK;
	int wait = 0;
	CString strMsg;
	BOOL bFunctionOK = TRUE;
	char szBuffer[MAX_PATH];
	ASSERT(pResult);
	try
	{
		if (pResult->bDoPDL)
		{
			hr = ExecutePDLSingleSweep(lchIndex);
			if (hr == S_FALSE)
			{
				strMsg.Format("%s", GetFSTPLastErrorMessage());
				throw  strMsg.GetBuffer(0);
			}
		}
		else
		{
			hr = ExecuteILSingleSweep(lchIndex);
			if (hr == S_FALSE)
			{
				strMsg.Format("%s", GetFSTPLastErrorMessage());
				throw  strMsg.GetBuffer(0);
			}
		}

		while (wait<60) //默认一分钟
		{
			long lTime, lStatus;
			GetSweepStatus(lchIndex, &lStatus, &lTime);
			if (lStatus == 0)
			{
				Sleep(1000);
				wait++;
				continue;
			}
			else if (lStatus == -1) 
			{
				strMsg.Format("查询状态:%d,错误信息：%s,扫描失败!", lStatus, GetFSTPLastErrorMessage());
				throw strMsg.GetBuffer(0);
			}
			else if (lStatus == 1) 
			{
				strMsg = "扫描成功!";
				Sleep(1000);
				break;
			}
			else 
			{
				throw "未知扫描状态";
			}
			if (wait == 60)
			{
				strMsg = "扫描超时!";
				throw strMsg.GetBuffer(0);
			}
		}
		Sleep(1000);
		long lGetPointCount;
		for (int i = 0;i <pResult->nPMCount;i++)
		{
			hr = GetMeasureResult(lchIndex, i, pResult->pdbWLArry, pResult->pdbPMResult[i], pResult->pdbPDLResult[i], pResult->pdbTapResult, &lGetPointCount);
			if (hr != S_OK)
			{
				Sleep(2000);
				hr = GetMeasureResult(lchIndex, i, pResult->pdbWLArry, pResult->pdbPMResult[i], pResult->pdbPDLResult[i], pResult->pdbTapResult, &lGetPointCount);
				if (hr != S_OK)
				{
					strMsg.Format("获取%d通道扫描数据存在错误!:%s", i + 1, GetFSTPLastErrorMessage());
					throw strMsg.GetBuffer(0);
				}
			}
		}
	}
	catch (char *pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}

/////////////////////////////////////////////////////////////////
//函数说明：
//    读取FSTP扫描文件数据
//Add by wanxin
//20150225
/////////////////////////////////////////////////////////////////
BOOL CFSTPScan::GetFSTPSCanData(LPCSTR lpstrFileName,pFSTPScanResult pResult)
{
	CString strRefPath;
	CString strMsg;
	FILE * fpFile = NULL;
	BOOL bFunctionOK = TRUE;
	char szBuffer[1024];
	char *token = NULL;
	ZeroMemory(szBuffer, sizeof(char) * 1024);

	try
	{
		ASSERT(pResult);
		//strRefPath.Format(".\\Reference");
		//CreateFullPath(strRefPath);
		//strFileName.Format("%s\\PDL_RefScan.csv", strRefPath);
		/*strFileName.Format("%s\\NoPDL_RefScan.csv", strRefPath);*/
		if (pResult->bDoPDL)
		{
			fpFile = fopen(lpstrFileName, "r");
			if (fpFile == NULL)
			{
				strMsg.Format("打开%s文件存在错误", lpstrFileName);
				throw strMsg.GetBuffer(0);
			}
			int nLineCount = 0;
			while (1)
			{
				ZeroMemory(szBuffer, sizeof(char) * 1024);

				if (fgets(szBuffer, 1024, fpFile) == NULL)
				{
					break;
				}
				token = strtok(szBuffer, ",");
				ASSERT(token);
				pResult->pdbWLArry[nLineCount] = atof(token);

				for (int j = 0;j < m_stScan.nPMCount;j++)
				{
					token = strtok(NULL, ",");
					ASSERT(token);
					pResult->pdbPMResult[j][nLineCount] = atof(token);
					token = strtok(NULL, ",");
					ASSERT(token);
					pResult->pdbPDLResult[j][nLineCount] = atof(token);
				}
				nLineCount++;
			}
			if (nLineCount != m_stScan.dwSampleCount)
			{
				strMsg.Format("从文件中读取的归零文件扫描点数与实际点数不匹配");
				throw strMsg.GetBuffer(0);
			}
		}
		else
		{
			fpFile = fopen(lpstrFileName, "r");
			if (fpFile == NULL)
			{
				strMsg.Format("打开%s文件存在错误", lpstrFileName);
				throw strMsg.GetBuffer(0);
			}
			int nLineCount = 0;
			while (1)
			{
				ZeroMemory(szBuffer, sizeof(char) * 1024);
				if (fgets(szBuffer, 1024, fpFile) == NULL)
				{
					break;
				}
				token = strtok(szBuffer, ",");
				ASSERT(token);
				pResult->pdbWLArry[nLineCount] = atof(token);

				for (int j = 0;j < m_stScan.nPMCount;j++)
				{
					token = strtok(NULL, ",");
					ASSERT(token);
					pResult->pdbPMResult[j][nLineCount] = atof(token);
				}
				nLineCount++;
			}
			if (nLineCount != m_stScan.dwSampleCount)
			{
				strMsg.Format("从文件中读取的文件扫描点数与实际点数不匹配");
				throw strMsg.GetBuffer(0);
			}
		}
	}
	catch (TCHAR* pstrErrorMsg)
	{
		m_strErrorMsg = pstrErrorMsg;
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return	bFunctionOK;
}

///////////////////////////////////////////////////////////
//函数说明：
//   保存FSTP扫描文件
//Add by wanxin
//Time:20200615
//////////////////////////////////////////////////////////
BOOL CFSTPScan::SaveFSTPScanResult(LPCSTR lpstrFileName, stFSTPScanResult pResult)
{
	CString strRefPath;
	CString strFileName;
	CString strMsg;

	FILE * fpFile = NULL;
	BOOL bFunctionOK = TRUE;
	char szBuffer[MAX_PATH];
	ZeroMemory(szBuffer, sizeof(char) * MAX_PATH);

	try
	{
		if (pResult.bDoPDL)
		{
			fpFile = fopen(lpstrFileName, "w");
			if (fpFile == NULL)
			{
				strMsg.Format("打开%s文件存在错误", lpstrFileName);
				throw strMsg.GetBuffer(0);
			}

			//按照波长,PM1,PDL1,PM2,PDL2,PM3,PDL3,PM4,PDL4
			for (int i = 0;i < m_stScan.dwSampleCount;i++)
			{
				strMsg.Empty();
				ZeroMemory(szBuffer, sizeof(char) * MAX_PATH);
				sprintf(szBuffer, "%.3f,", pResult.pdbWLArry[i]);
				strMsg += szBuffer;
				for (int j = 0;j < m_stScan.nPMCount;j++)
				{
					ZeroMemory(szBuffer, sizeof(char) * MAX_PATH);
					sprintf(szBuffer, "%.3f,%.3f,", pResult.pdbPMResult[j][i], pResult.pdbPDLResult[j][i]);
					strMsg += szBuffer;
				}
				fprintf(fpFile, "%s\n", strMsg);
			}
		}
		else
		{
			fpFile = fopen(lpstrFileName, "w");
			if (fpFile == NULL)
			{
				strMsg.Format("打开%s文件存在错误", strFileName);
				throw strMsg.GetBuffer(0);
			}

			//按照波长,PM1,PM2,PM3,PM4
			for (int i = 0;i < m_stScan.dwSampleCount;i++)
			{
				strMsg.Empty();
				ZeroMemory(szBuffer, sizeof(char) * MAX_PATH);
				sprintf(szBuffer, "%.3f,", pResult.pdbWLArry[i]);
				strMsg += szBuffer;
				for (int j = 0;j < m_stScan.nPMCount;j++)
				{
					ZeroMemory(szBuffer, sizeof(char) * MAX_PATH);
					sprintf(szBuffer, "%.3f,", pResult.pdbPMResult[j][i]);
					strMsg += szBuffer;
				}
				fprintf(fpFile, "%s\n", strMsg);
			}
		}
	}
	catch (char *pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return bFunctionOK;
}


BOOL CFSTPScan::AllocScanStruct(pFSTPScanResult pResult)
{
	DWORD dwSampleCount = m_stScan.dwSampleCount;
	if (pResult->pdbWLArry != NULL)
	{
		VirtualFree(pResult->pdbWLArry, 0, MEM_RELEASE);
		pResult->pdbWLArry = NULL;
	}
	pResult->pdbWLArry = (double*)VirtualAlloc(NULL, sizeof(double)*dwSampleCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	pResult->pdbTapResult = NULL;
	for (int j = 0;j < m_stScan.nPMCount;j++)
	{
		if (pResult->pdbPMResult[j] != NULL)
		{
			VirtualFree(pResult->pdbPMResult[j], 0, MEM_RELEASE);
			pResult->pdbPMResult[j] = NULL;
		}
		pResult->pdbPMResult[j] = (double*)VirtualAlloc(NULL, sizeof(double)*dwSampleCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		if (pResult->bDoPDL) //带PDL扫描
		{
			if (pResult->pdbPDLResult[j] != NULL)
			{
				VirtualFree(pResult->pdbPDLResult[j], 0, MEM_RELEASE);
				pResult->pdbPDLResult[j] = NULL;
			}
			pResult->pdbPDLResult[j] = (double*)VirtualAlloc(NULL, sizeof(double)*dwSampleCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		}
	}

	return TRUE;
}

BOOL CFSTPScan::FreeScanStruct(pFSTPScanResult pResult)
{
	DWORD dwSampleCount = m_stScan.dwSampleCount;
	if (pResult->pdbWLArry != NULL)
	{
		VirtualFree(pResult->pdbWLArry, 0, MEM_RELEASE);
		pResult->pdbWLArry = NULL;
	}
	pResult->pdbTapResult = NULL;
	for (int j = 0;j < m_stScan.nPMCount;j++)
	{
		if (pResult->pdbPMResult[j] != NULL)
		{
			VirtualFree(pResult->pdbPMResult[j], 0, MEM_RELEASE);
			pResult->pdbPMResult[j] = NULL;
		}

		if (pResult->bDoPDL) //带PDL扫描
		{
			if (pResult->pdbPDLResult[j] != NULL)
			{
				VirtualFree(pResult->pdbPDLResult[j], 0, MEM_RELEASE);
				pResult->pdbPDLResult[j] = NULL;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////
//函数说明：
//   归零函数
//Add by wanxin
//Time:20200616
////////////////////////////////////////////////////////////
BOOL CFSTPScan::DoReference(long lChIndex)
{
	m_stScanRefResult.bDoPDL = m_stScan.bDoPDL;
	m_stScanRefResult.nPMCount = m_stScan.nPMCount;

	if (!AllocScanStruct(&m_stScanRefResult))
	{
		return FALSE;
	}
	if (!ExcuteScan(lChIndex, &m_stScanRefResult))
	{
		return FALSE;
	}


	if (!SaveFSTPScanResult(m_strRefFileName, m_stScanRefResult))
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////
//函数说明：
//   扫描函数
//Add by wanxin
//Time:20200616
///////////////////////////////////////////////////////
BOOL CFSTPScan::DoTLSScaning(long lChIndex, BOOL bDoRef)
{
	double dbRefValue;
	m_stScanRefResult.bDoPDL = m_stScan.bDoPDL;
	m_stScanRefResult.nPMCount = m_stScan.nPMCount;
	m_stScanResult.bDoPDL = m_stScan.bDoPDL;
	m_stScanResult.nPMCount = m_stScan.nPMCount;
	m_stCirculator.bDoPDL= m_stScan.bDoPDL;
	m_stCirculator.nPMCount = m_stScan.nPMCount;

	if (!AllocScanStruct(&m_stScanRefResult))
	{
		return FALSE;
	}

	if (!GetFSTPSCanData(m_strRefFileName,&m_stScanRefResult))
	{
		return FALSE;
	}

	if (!AllocScanStruct(&m_stScanResult))
	{
		return FALSE;
	}

	if (!ExcuteScan(lChIndex, &m_stScanResult))
	{
		Sleep(2000);
		if (!ExcuteScan(lChIndex, &m_stScanResult))
		{
			return FALSE;
		}
	}

	//if (m_stScan.bCirculator) //如果带circulator需要读取circulator归零数据
	//{
	//	if (!AllocScanStruct(&m_stCirculator))
	//	{
	//		return FALSE;
	//	}

	//	if (!GetFSTPSCanData(m_strCirculatorName, &m_stCirculator))
	//	{
	//		return FALSE;
	//	}
	//}

	//根据归零值计算扫描器件的IL值
	for (int i = 0;i < m_stScan.dwSampleCount;i++)
	{
		for (int j = 0;j < m_stScan.nPMCount;j++)
		{
			dbRefValue = m_stScanRefResult.pdbPMResult[j][i];
		    
			m_stScanResult.pdbPMResult[j][i] = dbRefValue - m_stScanResult.pdbPMResult[j][i];
		}
	}

	if (!SaveFSTPScanResult(m_strScanFileName, m_stScanResult))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CFSTPScan::ReadReferenceDataFromFile()
{
	if (!AllocScanStruct(&m_stScanRefResult))
	{
		return FALSE;
	}

	if (!GetFSTPSCanData(m_strRefFileName, &m_stScanRefResult))
	{
		return FALSE;
	}
	return TRUE;

}


/////////////////////////////////////////////////////////////
//函数说明：
//   计算Circulator Ref 值并保存文件
//Add by wanxin
//Time:20200708
//////////////////////////////////////////////////////////////
BOOL CFSTPScan::CalculateCirculatorRefData()
{
	m_stScanResult.bDoPDL = m_stScan.bDoPDL;
	if (!AllocScanStruct(&m_stScanResult))
	{
		return FALSE;
	}

	for (int i = 0;i < m_stScan.dwSampleCount;i++)
	{
		m_stScanResult.pdbWLArry[i] = m_stCirculator1.pdbWLArry[i];

		for (int j = 0;j < m_stScan.nPMCount;j++)
		{
			double dbRefValue = m_stCirculator1.pdbPMResult[j][i] + m_stCirculator2.pdbPMResult[j][i];
			m_stScanResult.pdbPMResult[j][i] = dbRefValue;

			if (m_stScan.bDoPDL)
			{
				m_stScanResult.pdbPDLResult[j][i] = m_stCirculator2.pdbPDLResult[j][i];
			}
		}
	}

	if (!SaveFSTPScanResult(m_strCirculatorName, m_stScanResult))
	{
		return FALSE;
	}

	return TRUE;

}

/////////////////////////////////////////////////////////////
//函数说明：
//   读取circulator文件
//Add by wanxin
//Time:20200708
////////////////////////////////////////////////////////////
BOOL CFSTPScan::ReadCirculatorRefDataFromFile(int nType)
{
	
	if (nType == 0) //读取1-2
	{
		m_stCirculator1.bDoPDL = m_stScan.bDoPDL;
		if (!AllocScanStruct(&m_stCirculator1))
		{
			return FALSE;
		}
		if (!GetFSTPSCanData(m_strCirculatorName, &m_stCirculator1))
		{
			return FALSE;
		}
	}
	else  //读取2-3
	{
		m_stCirculator2.bDoPDL = m_stScan.bDoPDL;
		if (!AllocScanStruct(&m_stCirculator2))
		{
			return FALSE;
		}

		if (!GetFSTPSCanData(m_strCirculatorName, &m_stCirculator2))
		{
			return FALSE;
		}
	}

	return TRUE;
}