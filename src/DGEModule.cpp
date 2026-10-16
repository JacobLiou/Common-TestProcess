#include "stdafx.h"
#include "DGEModule.h"
#include "BundleFile.h"
#include "math.h"


CDGEModule::CDGEModule()
{
	m_strLogFileName = "";
	ZeroMemory(m_fTargetCurveFreq, sizeof(m_fTargetCurveFreq));
	ZeroMemory(m_fTargetCurveIL, sizeof(m_fTargetCurveIL));
	ZeroMemory(m_nTargetCurvePoint, sizeof(m_nTargetCurvePoint));
}


CDGEModule::~CDGEModule()
{
}
/////////////////////////////////////////////////////////
//函数说明：
// 从模块中读取模块SN号
//Add by wanxin
//Time:20200518
//////////////////////////////////////////////////////////
BOOL CDGEModule::GetModuleSerialNO(CString &strSN)
{
	CString strRecvData;
	BOOL bResponse = FALSE;
	BYTE byDataBuffer[32];
	ZeroMemory(byDataBuffer, sizeof(BYTE) * 32);
	//SPI_FLASH_READ,IIC_FLASH_READ
	bResponse = m_Module.fnGetMemoryData(FLASH_SN_ADDRESS, 32, strRecvData, byDataBuffer);
	if (!bResponse)
	{
		AfxMessageBox("Read SN Error!");
		return FALSE;
	}

	strSN.Format("%s", byDataBuffer);

	return TRUE;
}
//////////////////////////////////////////////////////////////////
//函数说明：
//   下载Lut表函数
//Add by wanxin
//Time:202005020
///////////////////////////////////////////////////////////////////
BOOL CDGEModule::DownloadUpdateFile(LPCSTR lpstrImageBinfile)
{
	BOOL bFunctionOK = TRUE;
	CString strLogoInfo;
	int  i;
	int  nSetLens;
	CString strTemp;
	float fProcess = 0;
	char chRet[MAX_BUFFER_LEN] = "";
	long lfilelens = 0;
	FILE *fUpdateFile = NULL;
	try
	{
		if (!m_Module.fnSelectTableLoading())
		{
			throw m_Module.GetLastError();
		}

		//if (!m_Module.fnFirmwareStart())
		//{
		//	throw m_Module.GetLastError();
		//}

		fUpdateFile = fopen(lpstrImageBinfile, "rb");
		if (fUpdateFile != NULL)
		{
			fseek(fUpdateFile, 0, SEEK_END);
			lfilelens = ftell(fUpdateFile);
			rewind(fUpdateFile);
			//2. Send Start firmware loading command
			//   format: 0x41 0x00 0x02 0xA5 0xC3 CS
			memset(chRet, 0, MAX_BUFFER_LEN);
			// 3. 开始循环传递具体数据   			
			int nSendTimes = lfilelens / EVERY_UPDATE_DATA_LEN;
			int nRemaid = lfilelens%EVERY_UPDATE_DATA_LEN;
			if (0 != nRemaid)
			{
				nSendTimes += 1;
			}

			for (i = 0; i < nSendTimes; i++)
			{
				// The last time and nRemaid not zero 
				if (((nSendTimes - 1) == i) && (0 != nRemaid))
				{
					nSetLens = nRemaid;
				}
				else
				{
					nSetLens = EVERY_UPDATE_DATA_LEN;
				}

				fread(chRet, nSetLens, 1, fUpdateFile);  // the remaid bytes have been set to 0;

				if (!m_Module.fnFirmwareDownload(chRet, nSetLens))
				{
					throw m_Module.GetLastError();
				}

				Sleep(50);
			}
		}
		else
		{
			throw "文件无法打开，无法写入Flash！";
		}
	}
	catch (char* pszErrorMsg)
	{
		m_strErrorMsg = *pszErrorMsg;
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		m_strErrorMsg = "未知的错误";
		bFunctionOK = FALSE;
	}
	if (fUpdateFile != NULL)
	{
		fclose(fUpdateFile);
	}

	return bFunctionOK;
}

BOOL CDGEModule::SetSoftWareReset()
{
	CString strRecvData;
	if (m_Module.fnRunNewFw(strRecvData) != TRUE)
	{
		m_strErrorMsg = strRecvData;
		return FALSE;
	}
	return TRUE;
}


BOOL CDGEModule::fnReadMappingCfgCsv(LPCTSTR lpstPath, BYTE *pbyChannel, int *pnLine)
{
	CString strTemp;
	BOOL bFuctionOK = TRUE;
	int			nLine = 0;
	char   szlineBuf[MAX_PATH];
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;
	LPTSTR		pStr;

	FILE * fpFile = NULL;

	try
	{
		//读取文件
		if (lpstPath == "")
		{
			strTemp.Format("File name can't be empty: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		fpFile = fopen(lpstPath, "r");
		if (fpFile == NULL)
		{
			strTemp.Format("Open %s file error!", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		ZeroMemory(szlineBuf, sizeof(char)*MAX_PATH);
		fgets(szlineBuf, MAX_PATH, fpFile);

		while (TRUE)
		{
			memset(szlineBuf, 0, MAX_PATH * sizeof(char));
			pStr = fgets(szlineBuf, MAX_PATH, fpFile);;
			if (NULL == pStr)
				break;

			token = strtok(szlineBuf, seps);
			token = strtok(NULL, seps);
			pbyChannel[nLine] = (BYTE)atoi(token);

			nLine++;
		}

		*pnLine = nLine;
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFuctionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return bFuctionOK;
}


ULONG CDGEModule::CalcCRC32(ULONG ulInitCRC, UCHAR* uszDataBuf, ULONG ulDataLen)
{
	ULONG ulCRC32 = NULL_DWORD;
	ULONG ulCRCIndex = NULL_DWORD;
	//Record the CRC into CRC table.
	ulCRC32 = ulInitCRC;
	while (ulDataLen--)
	{
		ulCRCIndex = (ulCRC32 ^ (*uszDataBuf++)) & 0x000000FF;
		ulCRC32 = ((ulCRC32 >> 8) & 0x00FFFFFF) ^ m_aulCRCTable[ulCRCIndex];
	}
	return ulCRC32;
}

BOOL CDGEModule::CreateBinFile(LPCTSTR lpstPath, BYTE* pBuf, int iLength)
{
	BOOL bFunctionOK = TRUE;
	BYTE* pbyBuf = NULL;
	FILE* pFile = NULL;
	pbyBuf = new BYTE[iLength];
	memset(pbyBuf, 0, iLength);
	try
	{
		int iPos = sizeof(ULONG);
		if (iLength <= 0)
		{
			throw "Lut表数据长度不可用.";
		}
		memcpy(pbyBuf, pBuf, iLength);

		pFile = fopen(lpstPath, "wb");
		if (pFile == NULL)
		{
			throw "创建Lut表文件错误.";
		}
		int iWtLens = fwrite(pbyBuf, sizeof(char), iLength, pFile);
		if (iWtLens != iLength)
		{
			throw "写Lut表文件错误.";
		}
	}
	catch (char *pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		bFunctionOK = FALSE;
	}
	if (pFile != NULL)
	{
		fclose(pFile);
	}

	delete[] pbyBuf;

	return bFunctionOK;
}


BOOL CDGEModule::BundleBinFile(DWORD dwDataLength, LPCTSTR lpstFilePath, DWORD dwAddress)
{
	BundleFile bf;
	SCF_IMG_INFO_ST cfInfo = { 0 };
	SCF_IMG_HEADER_INFO_ST cfHd = { 0, FALSE, NULL };
	cfInfo.iFileIndex = 0;

	memcpy(cfInfo.aszSN, "SUPER_SN", strlen("SUPER_SN"));
	memcpy(cfInfo.aszPN, "SUPER_PN", strlen("SUPER_PN"));

	cfInfo.ulImgLen = dwDataLength;
	memcpy(cfInfo.aszImageTag, "IBFH", strlen("IBFH"));
	cfInfo.uszImageType = 129;
	cfInfo.uszImageHitless = 0;
	cfInfo.uszStorageID = 3;
	cfInfo.uszImageIndex = 1;
	cfInfo.uszSectorCnt = 1;
	cfInfo.dwImageBaseAddr = dwAddress;

	DWORD dwFileNameLength = strlen(lpstFilePath);
	if (dwFileNameLength > SCF_MOD_NAME_LEN_2)
	{
		m_strErrorMsg = "Bin文件名长度大于256!";
		return FALSE;
	}

	memcpy(cfInfo.aszName, lpstFilePath, strlen(lpstFilePath));
	//CString strFileName = lpstFilePath; 
	int iBndLens = dwDataLength % SCF_XMD_PAGE_SIZE ? \
		(dwDataLength / SCF_XMD_PAGE_SIZE + 2) * SCF_XMD_PAGE_SIZE : \
		dwDataLength + SCF_XMD_PAGE_SIZE;
	if (!bf.BundleFiles(1, iBndLens, SCF_XMD_PAGE_SIZE, "1.0.0001", "DGE", FALSE, &cfInfo, &cfHd, NULL))
	{
		m_strErrorMsg = "创建Bundle文件存在错误!";
		return FALSE;
	}
	return TRUE;
}


BOOL CDGEModule::fnCreateMappingTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strFileName;
	CString strMsg;
	CString strTemplatePath;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	int nChannelNum = 0;
	DWORD dwFirstAddr = 0;
	BOOL bFunctionOK = TRUE;
	BYTE *pbyChannel = NULL;
	pbyChannel = new BYTE[PIXEL_NUMBER_64];
	memset(pbyChannel, 0, PIXEL_NUMBER_64 * sizeof(BYTE));
	try
	{
		ZeroMemory(&m_stMappingTab, sizeof(m_stMappingTab));
		//reserved fill in 0xFF
		//memset(m_stMappingTab.pbyReserve, 0xFF, sizeof(m_stMappingTab.pbyReserve));
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strCalTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strFileName.Format("%s\\Pixel_Mapping_Cal_Template.csv", strTemplatePath);

		if (!fnReadMappingCfgCsv(strFileName, pbyChannel, &nChannelNum))
		{
			strTemp.Format("Get file error: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		//cfg pixel mapping 
		for (int i = 0;i < nChannelNum; i++)
		{
			m_stMappingTab.pbyPixel[i] = pbyChannel[i];
		}
		for (int i = 64;i < PIXEL_NUMBER_96;i++)
		{
			m_stMappingTab.pbyPixel[i] = i;
		}

		pbyBuffer = (BYTE *)&m_stMappingTab;
		iLength = sizeof(m_stMappingTab);

		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer + 4, iLength - 4);
		inCrc32 ^= 0XFFFFFFFF;

		m_stMappingTab.byCrc32[0] = (byte)(inCrc32 & 0x000000FF);
		m_stMappingTab.byCrc32[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
		m_stMappingTab.byCrc32[2] = (byte)((inCrc32 >> 16) & 0x00FF);
		m_stMappingTab.byCrc32[3] = (byte)(inCrc32 >> 24);

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Mapping Lut文件存在错误";
		}
		dwFirstAddr = ADDR_MAPPING_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Mapping Bundle文件存在错误";
		}

		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Mapping Bundle文件存在错误";
		}
		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}

		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}
	if (pbyChannel != NULL)
	{
		delete[] pbyChannel;
		pbyChannel = NULL;
	}

	return bFunctionOK;
}

BOOL CDGEModule::CopyInformationData()
{
	//UpdateData();
	char chBuf[256];
	int	index = 0;
	int nWeek = 0, nYear = 0;
	CString  strTempFile, strTemp, strVersion, strInfo;
	ZeroMemory(chBuf, 256);

	//CLEAR TABLE;
	//ZeroMemory(m_stModuleInfo,sizeof(m_stModuleInfo));
	memset(m_stModuleInfo.pbySN, 0, sizeof(m_stModuleInfo.pbySN));
	memset(m_stModuleInfo.pbyPN, 0, sizeof(m_stModuleInfo.pbyPN));

	//write SN,PN;
	memcpy(&m_stModuleInfo.pbySN, m_DGEInfo.strSN, strlen(m_DGEInfo.strSN));
	memcpy(&m_stModuleInfo.pbyPN, m_DGEInfo.strPN, strlen(m_DGEInfo.strPN));

	//WRITE HW_VERSION;
	memset(m_stModuleInfo.pbyHV, 0, sizeof(m_stModuleInfo.pbyHV));
	strVersion.Format("%s", m_DGEInfo.strHWVersion);
	index = strVersion.Find(".", 0);
	m_stModuleInfo.pbyHV[0] = (BYTE)atoi(strVersion.Left(index));
	strTemp = strVersion.Right(strVersion.GetLength() - index - 1);
	index = strTemp.Find(".", 0);
	m_stModuleInfo.pbyHV[1] = (BYTE)atoi(strTemp.Left(index));
	index = strTemp.Find(".", 0);
	m_stModuleInfo.pbyHV[2] = (BYTE)atoi(strTemp.Right(strTemp.GetLength() - index - 1));

	//WRITE CALIBRATE_DATE;
	memset(m_stModuleInfo.pbyCaliDate, 0, sizeof(m_stModuleInfo.pbyCaliDate));
	CTime ti = CTime::GetCurrentTime();
	strTemp = ti.Format("%Y%m%d");

	m_stModuleInfo.pbyCaliDate[0] = (BYTE)(atoi(strTemp.Left(2)));
	m_stModuleInfo.pbyCaliDate[1] = (BYTE)(atoi(strTemp.Mid(2, 2)));
	m_stModuleInfo.pbyCaliDate[2] = (BYTE)(atoi(strTemp.Mid(4, 2)));
	m_stModuleInfo.pbyCaliDate[3] = (BYTE)(atoi(strTemp.Right(2)));

	//从DGE表格式1.6和FW110C开始，生产信息表增加了slice frequency, slice step, slice number
	m_stModuleInfo.fSliceFre = 11250; //默认值-180000G
	m_stModuleInfo.fSliceStep = 6.25;
	m_stModuleInfo.nSliceNumber = 768;

	return TRUE;
}


BOOL CDGEModule::GetModuleDeviceType()
{
	//	UpdateData();
	char chBuf[256];
	int	index = 0;
	int nWeek = 0, nYear = 0;
	CString  strTempFile, strTemp, strVersion, strInfo;
	ZeroMemory(chBuf, 256);
	memset(m_stMessageInfo.pbyDeviceType, 0, sizeof(m_stMessageInfo.pbyDeviceType));

	for (int i = 0;i < 32;i++)
	{
		m_MappingNew[i] = i + 1;
	}

	m_iPNNewDeviceNmb = 0;

	m_stModuleInfo.iModType = (BYTE)m_DGEInfo.nModuleType;

	//屏蔽从数据库中获取设备数量：ruij
	//m_stMessageInfo.iDeviceNO = (BYTE)atoi(m_strArrayInforamtion.GetAt(1));

	for (int j = 0;j < m_DGEInfo.nDevcieTTNum;j++)
	{
		m_stMessageInfo.pbyDeviceMapping[j] = m_MappingNew[j];
	}
	m_stMessageInfo.iActDeviceNmb = m_iPNNewDeviceNmb > 0 ? m_iPNNewDeviceNmb : m_DGEInfo.nDevcieTTNum;

	//HW_DATE;
	ZeroMemory(chBuf, 256);
	memset(m_stModuleInfo.pbyHD, 0, sizeof(m_stModuleInfo.pbyHD));
	strTemp = m_DGEInfo.strHWDate;
	nYear = atoi(strTemp.Right(4));
	index = strTemp.Find("W", 0);
	nWeek = atoi(strTemp.Left(index));
	m_stModuleInfo.pbyHD[1] = (BYTE)nWeek;
	m_stModuleInfo.pbyHD[0] = (BYTE)(nYear - YEAR_CENTURY_NOW);
	for (i = 0;i < m_stMessageInfo.iDeviceNO;i++)
	{
		m_stMessageInfo.pbyDeviceType[i] = (BYTE)m_DGEInfo.nModuleType;
	}
	m_stModuleInfo.pStrfreq = m_DGEInfo.nFirstSliceFreq;
	m_stModuleInfo.pEndfreq = m_DGEInfo.nLastSliceFreq;

	//RESERVE;
	memset(m_stModuleInfo.pbyReserve, 0xff, sizeof(m_stModuleInfo.pbyReserve));

	return TRUE;
}

BOOL CDGEModule::fnCreateMfgInfoTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;

	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	DWORD dwFirstAddr;
	try
	{
		//initial struct 
		ZeroMemory(&m_stModuleInfo, sizeof(m_stModuleInfo));

		//get hw versin and cal date from ModuleConfig.ini
		if (!CopyInformationData())
		{
			throw "获取模块信息存在错误";
		}

		//get start frq, end frq, module type, hw date from database
		if (!GetModuleDeviceType())
		{
			throw "获取模块类型存在错误";
		}

		pbyBuffer = (BYTE *)&m_stModuleInfo;
		iLength = sizeof(m_stModuleInfo);

		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer + 4, iLength - 4);
		inCrc32 ^= 0XFFFFFFFF;

		m_stModuleInfo.iCrc32 = inCrc32;

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Mfg Lut文件存在错误";
		}
		dwFirstAddr = ADDR_MFG_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Mfg Bundle文件存在错误";
		}

		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Mfg Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}

		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	return TRUE;
}


CString CDGEModule::ReadIniFileString(LPCTSTR lpstPath, CString strSection, CString strKey)
{
	CString strValue;
	CString strMsg;
	char chBuf[MAX_PATH] = "";
	GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, lpstPath);
	strValue = (LPCTSTR)chBuf;
	if (strValue.Find("error") == 0)
	{
		strMsg.Format("open file error:%s(%s)", lpstPath, strKey);
		m_strErrorMsg = strMsg;
	}

	CString strRet;
	strRet.Format("%s", chBuf);

	return strRet;
}

BOOL CDGEModule::fnCreateDutParaTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;
	CString strSection;
	CString strKey;
	CString strFileName;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	int nTemp = 0;
	DWORD dwFirstAddr;
	BOOL bFunctionOK = TRUE;
	try
	{
		//initial struct 
		ZeroMemory(&m_stDevParaTab, sizeof(m_stDevParaTab));
		//reserved fill in 0xFF
		memset(m_stDevParaTab.pbyReserve, 0xFF, sizeof(m_stDevParaTab.pbyReserve));

		//device parameter configuration
		strFileName.Format(".\\config\\ProuctionInfo.ini");

		//default value for this table
		strSection.Format("PARA_CONFIG");

		//1. pixel number
		strKey.Format("Pixel number");
		strTemp = ReadIniFileString(strFileName, strSection, strKey);
		nTemp = atoi(strTemp);
		if (nTemp < PIXEL_NUMBER_64)
		{
			strTemp.Format("Config pixel number: %d is not correct", nTemp);
			throw strTemp.GetBuffer(0);
		}
		m_stDevParaTab.wPixelNum = atoi(strTemp);

		//2. Temperature hysteresis
		strKey.Format("Temperature hysteresis");
		strTemp = ReadIniFileString(strFileName, strSection, strKey);
		nTemp = atoi(strTemp);
		if (nTemp < 1)
		{
			strTemp.Format("Config Temperature hysteresisr: %d is not correct", nTemp);
			throw strTemp.GetBuffer(0);
		}
		m_stDevParaTab.wTempHys = atoi(strTemp);

		for (int i = 0; i < 4; i++)
		{
			//3. Delay parameter 1 ~4
			strKey.Format("Delay parameter %d", i + 1);
			strTemp = ReadIniFileString(strFileName, strSection, strKey);
			m_stDevParaTab.pDelay[i] = atof(strTemp);

			//4. Inter Sensor parameter 1 ~4
			strKey.Format("Inter Sensor parameter %d", i + 1);
			strTemp = ReadIniFileString(strFileName, strSection, strKey);
			m_stDevParaTab.pInterSensor[i] = atof(strTemp);

			//5. Sensor1 parameter 1 ~4
			strKey.Format("Sensor1 parameter %d", i + 1);
			strTemp = ReadIniFileString(strFileName, strSection, strKey);
			m_stDevParaTab.pSensor1[i] = atof(strTemp);

			//6. Sensor2 parameter 1 ~4
			strKey.Format("Sensor2 parameter %d", i + 1);
			strTemp = ReadIniFileString(strFileName, strSection, strKey);
			m_stDevParaTab.pSensor2[i] = atof(strTemp);
		}

		//add 2019.1.22 new pcba
		//0 BOARD TEMP,1 SEL_TS1_TEMP,2 SEL TS2 TEMP
		for (i = 0; i < 3; i++)
		{
			m_stDevParaTab.nSensorType[i] = i;
		}
		//7. ASIC sct value
		strKey.Format("ASIC sct value");
		strTemp = ReadIniFileString(strFileName, strSection, strKey);
		m_stDevParaTab.wASICsctvalue = atoi(strTemp);

		pbyBuffer = (BYTE *)&m_stDevParaTab;
		iLength = sizeof(m_stDevParaTab);

		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer + 4, iLength - 4);
		inCrc32 ^= 0XFFFFFFFF;

		m_stDevParaTab.byCrc32[0] = (byte)(inCrc32 & 0x000000FF);
		m_stDevParaTab.byCrc32[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
		m_stDevParaTab.byCrc32[2] = (byte)((inCrc32 >> 16) & 0x00FF);
		m_stDevParaTab.byCrc32[3] = (byte)(inCrc32 >> 24);

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Param Lut文件存在错误";
		}
		dwFirstAddr = ADDR_PARA_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Param Bundle文件存在错误";
		}

		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Param Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}

		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnCreateWaveTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;
	CString strTemperSP;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	CString strCfgFileName;
	CString strCalFileName;
	CString strSection, strKey;
	char    chBuf[MAX_PATH];
	DWORD dwFirstAddr;
	CString  strPath;

	try
	{
		//initial struct 
		ZeroMemory(&m_stWaveTab, sizeof(m_stWaveTab));
		//reserved fill in 0xFF
		memset(m_stWaveTab.stWaveInfo[0].pbyReserve, 0xFF, sizeof(m_stWaveTab.stWaveInfo[0].pbyReserve));
		memset(m_stWaveTab.stWaveInfo[1].pbyReserve, 0xFF, sizeof(m_stWaveTab.stWaveInfo[1].pbyReserve));
		memset(m_stWaveTab.stWaveInfo[2].pbyReserve, 0xFF, sizeof(m_stWaveTab.stWaveInfo[2].pbyReserve));
		memset(m_stWaveTab.stWaveInfo[3].pbyReserve, 0xFF, sizeof(m_stWaveTab.stWaveInfo[3].pbyReserve));
		memset(m_stWaveTab.stWaveInfo[4].pbyReserve, 0xFF, sizeof(m_stWaveTab.stWaveInfo[4].pbyReserve));

		//5 type of temperatures

		strPath.Format("%s\\config", m_strCaliFolder);
	
		strCfgFileName.Format("%s\\ModuleConfig.ini", strPath);

		for (int loop = 0; loop < CALIBRATION_TEMPER_MAX; loop++)
		{
			if (loop >= 5)
			{
				continue; 
			}
			//cal temperature
			strSection.Format("CAL_TEMP");
			strTemperSP.Format("%.0f", m_stConfigTable.dbDWTemp[loop]);
			strKey.Format("Temp_WAVE_%s", strTemperSP);
			GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strCfgFileName);
			strTemp = chBuf;
			if ("error" == strTemp)
			{
				strMsg.Format("%s value can't be empty", strKey);
				throw strMsg.GetBuffer(0);
			}
			else if ((atof(strTemp) > 80) || (atof(strTemp) < -20))
			{
				strMsg.Format("Error value: %s = %s", strKey, strTemp);
				throw strMsg.GetBuffer(0);
			}
			//actual temp value
			m_stWaveTab.fTemp[loop] = atof(strTemp);

			strTemperSP.Format("%.0f",m_stConfigTable.dbDWTemp[loop]);
			strCalFileName.Format("%s\\Calibrate\\%s\\PixelWave.ini", m_strCaliFolder, strTemperSP);

			for (int i = 0; i < m_DGEInfo.nPixcelCount; i++)
			{
				strSection.Format("CENTER_WAVE");
				strKey.Format("GHZ_%d", i);
				GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strCalFileName);
				strTemp = chBuf;
				if ("error" == strTemp)
				{
					strMsg.Format("%s value can't be empty", strKey);
					throw strMsg.GetBuffer(0);
				}

				//center wavelength:  
				//频率从小到大排列，即波长从长波长到短波长
				m_stWaveTab.stWaveInfo[loop].pPixelWaveGhz[i] = atof(strTemp) - 180000; //minus 180000
			}
		}

		//1. CRC32 for Temperature
		pbyBuffer = (BYTE *)&m_stWaveTab.fTemp;
		iLength = sizeof(m_stWaveTab.fTemp);

		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
		inCrc32 ^= 0XFFFFFFFF;

		m_stWaveTab.byCrc32[0] = (byte)(inCrc32 & 0x000000FF);
		m_stWaveTab.byCrc32[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
		m_stWaveTab.byCrc32[2] = (byte)((inCrc32 >> 16) & 0x00FF);
		m_stWaveTab.byCrc32[3] = (byte)(inCrc32 >> 24);

		//2. CRC32 for pixel wavelength
		for (loop = 0; loop < CALIBRATION_TEMPER_MAX; loop++)
		{
			if (loop >= 5)
			{
				continue; //by luke.liu 2018.2.14只取5个温度
			}

			pbyBuffer = (BYTE *)&m_stWaveTab.stWaveInfo[loop];
			iLength = sizeof(m_stWaveTab.stWaveInfo[loop]);

			inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer + 4, iLength - 4);
			inCrc32 ^= 0XFFFFFFFF;

			m_stWaveTab.stWaveInfo[loop].byCrc32Temp[0] = (byte)(inCrc32 & 0x000000FF);
			m_stWaveTab.stWaveInfo[loop].byCrc32Temp[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
			m_stWaveTab.stWaveInfo[loop].byCrc32Temp[2] = (byte)((inCrc32 >> 16) & 0x00FF);
			m_stWaveTab.stWaveInfo[loop].byCrc32Temp[3] = (byte)(inCrc32 >> 24);
		}


		pbyBuffer = (BYTE *)&m_stWaveTab;
		iLength = sizeof(m_stWaveTab);

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Wavelength Lut文件存在错误";
		}
		dwFirstAddr = ADDR_WAVE_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Wavelength Bundle文件存在错误";
		}


		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Wavelength Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}

		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	return TRUE;
}


BOOL CDGEModule::IsTheFileExist(LPCTSTR lpstrFileName)
{
	CString strTemp;
	HANDLE hr = NULL;
	WIN32_FIND_DATA wndFind;

	try
	{
		hr = FindFirstFile(lpstrFileName, &wndFind);

		if (INVALID_HANDLE_VALUE == hr)
		{
			strTemp.Format("Can't find file: %s", lpstrFileName);
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char* error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	if (NULL != hr)
	{
		FindClose(hr);
	}

	return TRUE;
}




BOOL CDGEModule::fnReadAttenLut256Csv(LPCTSTR lpstPath, double *pdblDac, int *pnLine)
{
	CString strTemp;

	int			nLine = 0;
	DWORD        dwBufSize = (sizeof(double) + 1)*(ATTEN_LUT_POINT + 1);
	char		*lineBuf = new char[dwBufSize];
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;
	LPTSTR		pStr;
	FILE* fpFile = NULL;
	BOOL bFunctionOK = TRUE;

	try
	{

		//读取文件
		if (lpstPath == "")
		{
			strTemp.Format("File name can't be empty: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		fpFile = fopen(lpstPath, "r");

		if (fpFile == NULL)
		{
			strTemp.Format("Open file error: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		
		while (TRUE)
		{
			memset(lineBuf, 0, dwBufSize);
			pStr = fgets(lineBuf, dwBufSize, fpFile);
			if (NULL == pStr)
				break;

			token = strtok(lineBuf, seps);
			pdblDac[nLine*ATTEN_LUT_POINT] = atof(token);

			for (int i = 1; i < ATTEN_LUT_POINT; i++)
			{
				token = strtok(NULL, seps);
				pdblDac[nLine*ATTEN_LUT_POINT + i] = atof(token);
			}

			nLine++;
		}
		//total row for DAC 
		*pnLine = nLine;

	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
	}
	if (lineBuf != NULL)
	{
		delete [] lineBuf;
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnCreateAttenTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	int nPixelCount = 0;
	double  *pdblDac = NULL;
	double dblLutDac[CALIBRATION_TEMPER_MAX][PIXEL_NUMBER_64][ATTEN_LUT_POINT] = { 0 };
	double dblStepLutDac[CALIBRATION_TEMPER_MAX] = { 0 }; //0.1dB per step
	double dblActCaliTemper[CALIBRATION_TEMPER_MAX] = { 0 };
	double *pdblCoefficent = NULL;
	CString strFileName;
	CString strCaliTemper;
	CString strSection;
	CString strKey;
	char    chBuf[MAX_PATH] = { 0 };
	CString strTemperature[2][CALIBRATION_TEMPER_MAX] = { { "0" },{ "0" } };
	int     nTempOrderCount[2] = { 0,0 };
	int     nTempCount[2] = { 0,0 };
	int     nTempMaxCount = 6;
	BOOL  bFunctionOK = TRUE;
	DWORD dwFirstAddr;
	try
	{
		//initial struct 
		ZeroMemory(&m_stAttenTab, sizeof(m_stAttenTab));

		strFileName.Format(".\\config\\ProuctionInfo.ini");

		strSection.Format("ATT_FITTING_TEMP");
		strKey.Format("FittingTEMP");

		ZeroMemory(chBuf, sizeof(chBuf));
		GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
		strTemp = chBuf;
		if ("error" == strTemp)
		{
			strMsg.Format("Can't find the value of %s", strKey);
			throw strMsg.GetBuffer(0);
		}
		else
		{
			m_stAttenTab.Temp = atof(strTemp);
		}

		for (int i = 0; i < nTempMaxCount; i++)
		{
			strSection.Format("ATT_FITTING_TEMP");
			strKey.Format("T1_%d", i + 1);

			ZeroMemory(chBuf, sizeof(chBuf));
			GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
			strTemp = chBuf;
			if ("END" == strTemp)
			{
				strSection.Format("ATT_FITTING_TEMP");
				strKey.Format("T1_Order");

				ZeroMemory(chBuf, sizeof(chBuf));
				GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
				strTemp = chBuf;
				nTempOrderCount[0] = atoi(strTemp);
				break;
			}

			if ("error" == strTemp)
			{
				strMsg.Format("Can't find the value of %s", strKey);
				throw strMsg.GetBuffer(0);
			}
			else
			{
				strTemperature[0][i] = strTemp;
			}
			nTempCount[0]++;
		}

		for (i = 0; i < nTempMaxCount; i++)
		{
			strSection.Format("ATT_FITTING_TEMP");
			strKey.Format("T2_%d", i + 1);
			ZeroMemory(chBuf, sizeof(chBuf));
			GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
			strTemp = chBuf;
			if ("END" == strTemp)
			{
				strSection.Format("ATT_FITTING_TEMP");
				strKey.Format("T2_Order");

				ZeroMemory(chBuf, sizeof(chBuf));
				GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
				strTemp = chBuf;
				nTempOrderCount[1] = atoi(strTemp);
				break;
			}

			if ("error" == strTemp)
			{
				strMsg.Format("Can't find the value of %s", strKey);
				throw strMsg.GetBuffer(0);
			}
			else
			{
				strTemperature[1][i] = strTemp;
			}
			nTempCount[1]++;
		}

		//读取5个温度的LUT表： 0.1dB的256个数据
		//pdwDac = new DWORD[PIXEL_NUMBER_64*ATTEN_LUT_POINT];
		pdblDac = new double[PIXEL_NUMBER_64*ATTEN_LUT_POINT];

		for (i = 0; i < 2; i++)
		{
			for (int loop = 0; loop < nTempCount[i]; loop++)
			{
				if (loop >= nTempMaxCount)
				{
					continue; //by luke.liu 2018.2.22只取5个温度
				}

				//cal temperature
				strCaliTemper.Format("%s", strTemperature[i][loop]);
				nPixelCount = 0;
				//memset(pdwDac, 0, sizeof(DWORD)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);
				memset(pdblDac, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);

				strFileName.Format("%s\\Calibrate\\%s\\256vsLut.csv", m_strCaliFolder, strCaliTemper);
				if (!IsTheFileExist(strFileName))
				{
					strTemp.Format("file is not exist: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}
				if (!fnReadAttenLut256Csv(strFileName, pdblDac, &nPixelCount))
				{
					strTemp.Format("Get %sC Lut256Csv file error!", strCaliTemper);
					throw strTemp.GetBuffer(0);
				}
				if (PIXEL_NUMBER_64 != nPixelCount)
				{
					strTemp.Format("Get %sC Atten Lut256Csv Line: %d != Pixel num %d", strCaliTemper, nPixelCount, PIXEL_NUMBER_64);
					throw strTemp.GetBuffer(0);
				}

				//memcpy(&dwLutDac[loop], pdwDac, PIXEL_NUMBER_64*ATTEN_LUT_POINT*sizeof(DWORD));
				memcpy(&dblLutDac[loop], pdblDac, PIXEL_NUMBER_64*ATTEN_LUT_POINT * sizeof(double));

				//读取实际温度值
				strFileName.Format("%s\\Config\\ModuleConfig.ini",m_strCaliFolder);

				//Current temperature
				strSection.Format("CAL_TEMP");
				strKey.Format("Temp_ATTEN_%s", strCaliTemper);

				ZeroMemory(chBuf, sizeof(chBuf));
				GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
				strTemp = chBuf;
				if ("error" == strTemp)
				{
					strMsg.Format("Can't find the value of %s", strKey);
					throw strMsg.GetBuffer(0);
				}
				dblActCaliTemper[loop] = atof(strTemp); //cal temperature
			}

			//calculate A,B,C,D coefficient per 0.1dB
			//
			pdblCoefficent = new double[10];
			memset(pdblCoefficent, 0, 10 * sizeof(double));

			for (int nPixelIndex = 0; nPixelIndex < PIXEL_NUMBER_64; nPixelIndex++)
			{
				for (int nLutIndex = 0; nLutIndex < ATTEN_LUT_POINT; nLutIndex++)
				{
					ZeroMemory(dblStepLutDac, sizeof(dblStepLutDac));
					memset(pdblCoefficent, 0, 10 * sizeof(double));
					for (int nTemper = 0; nTemper < nTempCount[i]; nTemper++)
					{
						dblStepLutDac[nTemper] = dblLutDac[nTemper][nPixelIndex][nLutIndex];
					}
					//coefficicent for every 0.1dB atten
					//Pixel频率从小到大
					//Edit by yud for fouth-order function
					//			m_calPolyn.EMatrix(dblActCaliTemper, dblStepLutDac, CALIBRATION_TEMPER_MAX, 4, pdblCoefficent);
					//			m_stAttenTab.pPixelAttenCoefficent[nPixelIndex][nLutIndex][3] = pdblCoefficent[1];//D
					//			m_stAttenTab.pPixelAttenCoefficent[nPixelIndex][nLutIndex][2] = pdblCoefficent[2];//C
					//			m_stAttenTab.pPixelAttenCoefficent[nPixelIndex][nLutIndex][1] = pdblCoefficent[3];//B
					//			m_stAttenTab.pPixelAttenCoefficent[nPixelIndex][nLutIndex][0] = pdblCoefficent[4];//A
					m_calPolyn.EMatrix(dblActCaliTemper, dblStepLutDac, nTempCount[i], nTempOrderCount[i], pdblCoefficent);
					if (i == 0)
					{
						if (nTempOrderCount[i] == 3) //2阶
						{
							//	m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][4] = 0;//E
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][3] = pdblCoefficent[1];//D
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][2] = pdblCoefficent[2];//C
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][1] = pdblCoefficent[3];//B
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][0] = 0;//A
						}
						else if (nTempOrderCount[i] == 4)//3阶
						{
							//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][4] = 0;//E
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][3] = pdblCoefficent[1];//D
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][2] = pdblCoefficent[2];//C
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][1] = pdblCoefficent[3];//B
							m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][0] = pdblCoefficent[4];//A
						}
						//else if(nTempOrderCount[i] == 5)//4阶
						//{
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][4] = pdblCoefficent[1];//E
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][3] = pdblCoefficent[2];//D
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][2] = pdblCoefficent[3];//C
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][1] = pdblCoefficent[4];//B
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][0] = pdblCoefficent[5];//A
						//	}
					}
					else if (i == 1)
					{
						if (nTempOrderCount[i] == 3) //2阶
						{
							//	m_stAttenTab.pPixelAttenCoefficentTemp1[nPixelIndex][nLutIndex][4] = 0;//E
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][3] = pdblCoefficent[1];//D
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][2] = pdblCoefficent[2];//C
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][1] = pdblCoefficent[3];//B
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][0] = 0;//A
						}
						else if (nTempOrderCount[i] == 4)//3阶
						{
							//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][4] = 0;//E
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][3] = pdblCoefficent[1];//D
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][2] = pdblCoefficent[2];//C
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][1] = pdblCoefficent[3];//B
							m_stAttenTab.pPixelAttenCoefficentTemp2[nPixelIndex][nLutIndex][0] = pdblCoefficent[4];//A
						}
						//else if(nTempOrderCount[i] == 5)//4阶
						//{
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][4] = pdblCoefficent[1];//E
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][3] = pdblCoefficent[2];//D
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][2] = pdblCoefficent[3];//C
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][1] = pdblCoefficent[4];//B
						//	m_stAttenTab.pPixelAttenCoefficent[i][nPixelIndex][nLutIndex][0] = pdblCoefficent[5];//A
						//	}


					}
				}
			}
		}

		pbyBuffer = (BYTE *)&m_stAttenTab.pPixelAttenCoefficentTemp1;
		iLength = sizeof(m_stAttenTab.pPixelAttenCoefficentTemp1);
		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
		inCrc32 ^= 0XFFFFFFFF;
		m_stAttenTab.Temp1Crc = inCrc32;


		pbyBuffer = (BYTE *)&m_stAttenTab.pPixelAttenCoefficentTemp2;
		iLength = sizeof(m_stAttenTab.pPixelAttenCoefficentTemp2);
		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
		inCrc32 ^= 0XFFFFFFFF;
		m_stAttenTab.Temp2Crc = inCrc32;


		pbyBuffer = (BYTE *)&m_stAttenTab.Temp;
		iLength = sizeof(m_stAttenTab.Temp);
		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
		inCrc32 ^= 0XFFFFFFFF;
		m_stAttenTab.TempCrc = inCrc32;
 
		pbyBuffer = (BYTE *)&m_stAttenTab;
		iLength = sizeof(m_stAttenTab);
		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Attenuation Lut文件存在错误";
		}
		dwFirstAddr = ADDR_ATTEN_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Attenuation Bundle文件存在错误";
		}

		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Attenuation Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}

		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	if (pdblDac != NULL)
	{
		delete[] pdblDac;
		pdblDac = NULL;
	}

	if (pdblCoefficent != NULL)
	{
		delete[] pdblCoefficent;
		pdblCoefficent = NULL;
	}

	return bFunctionOK;
}


BOOL CDGEModule::SaveAttenLut256Csv(LPCTSTR lpstPath, double *pdblDac)
{
	BOOL         bFunctionOK = TRUE;

	CString      strTemp;
	CString      strValue;
	CString      strFileName;
	FILE*        pFile = NULL;

	try
	{

		strFileName = lpstPath;

		//read the IL data
		pFile = fopen(lpstPath, "wt");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}


		for (int i = 0; i < PIXEL_NUMBER_64; i++)
		{
			for (int k = 0; k < ATTEN_LUT_POINT; k++)
			{
				//strTemp.Format("%d,", pwDac[ATTEN_LUT_POINT*i+k]);
				strTemp.Format("%.3f,", pdblDac[ATTEN_LUT_POINT*i + k]);
				fprintf(pFile, "%s", strTemp);
			}

			fprintf(pFile, "\n");
		}

	}
	catch (TCHAR* ptszErrorMsg)
	{
		m_strErrorMsg = ptszErrorMsg;

		bFunctionOK = FALSE;
	}
	catch (...)
	{
		m_strErrorMsg = "Other fatal exception occurred";
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnReadPixelWaveIni(LPCTSTR lpstPath, stPixelInfo *pstPixel, BOOL bCal)
{
	CString strTemp;
	CString strMsg;
	CString strSection;
	CString strKey;

	try
	{
		if (bCal)
		{
			for (int nIndex = 2; nIndex < PIXEL_NUMBER_64 - 2; nIndex++)
			{
				strSection.Format("CENTER_WAVE");
				strKey.Format("WAVE_%d", nIndex);

				//Wavelength nm
				strTemp = ReadIniFileString(lpstPath, strSection, strKey);
				if (strTemp.IsEmpty())
				{
					strMsg.Format("Pixel(%d) Wave: %.3fnm, it can't be empty!", nIndex + 1, strTemp);
					throw strMsg.GetBuffer(0);
				}
				pstPixel->dblPixelWav[nIndex] = atof(strTemp);

				//
				strKey.Format("GHZ_%d", nIndex);
				strTemp = ReadIniFileString(lpstPath, strSection, strKey);
				if (strTemp.IsEmpty())
				{
					strMsg.Format("Pixel(%d) GHZ: %.3fGhz, it can't be empty!", nIndex + 1, strTemp);
					throw strMsg.GetBuffer(0);
				}
				pstPixel->dblPixelGhz[nIndex] = atof(strTemp);

				strKey.Format("IL_%d", nIndex);
				strTemp = ReadIniFileString(lpstPath, strSection, strKey);
				if (strTemp.IsEmpty())
				{
					strMsg.Format("Pixel(%d) IL: %.3dB, it can't be empty!", nIndex + 1, strTemp);

					throw strMsg.GetBuffer(0);
				}
				pstPixel->dblPixelIL[nIndex] = atof(strTemp);

			}

			pstPixel->nPixelWavNum = nIndex;

		}
		else
		{
			for (int nIndex = 0; nIndex < PIXEL_NUMBER_64; nIndex++)
			{
				strSection.Format("CENTER_WAVE");
				strKey.Format("WAVE_%d", nIndex);

				//Wavelength nm
				strTemp = ReadIniFileString(lpstPath, strSection, strKey);
				if (strTemp.IsEmpty())
				{
					strMsg.Format("Pixel(%d) Wave: %.3fnm, it can't be empty!", nIndex + 1, strTemp);

					throw strMsg.GetBuffer(0);
				}
				pstPixel->dblPixelWav[nIndex] = atof(strTemp);

				//
				strKey.Format("GHZ_%d", nIndex);
				strTemp = ReadIniFileString(lpstPath, strSection, strKey);
				if (strTemp.IsEmpty())
				{
					strMsg.Format("Pixel(%d) GHZ: %.3fGhz, it can't be empty!", nIndex + 1, strTemp);

					throw strMsg.GetBuffer(0);
				}
				pstPixel->dblPixelGhz[nIndex] = atof(strTemp);


				strKey.Format("IL_%d", nIndex);
				strTemp = ReadIniFileString(lpstPath, strSection, strKey);
				if (strTemp.IsEmpty())
				{
					strMsg.Format("Pixel(%d) IL: %.3dB, it can't be empty!", nIndex + 1, strTemp);
					throw strMsg.GetBuffer(0);
				}
				pstPixel->dblPixelIL[nIndex] = atof(strTemp);

			}

			pstPixel->nPixelWavNum = nIndex;

			if (PIXEL_NUMBER_64 != pstPixel->nPixelWavNum)
			{
				strTemp.Format("Ini file的Pixel数量只有: %d", nIndex);
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	return TRUE;
}


BOOL CDGEModule::ReadILRatio_Beam(LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblPixelWave, double *pdblPixelGHz, double *pdblGetIL, double *pdblRatio1, double *pdbRatio2, int *pnTotalLength)
{
	FILE  *fpFile = NULL;
	CString strTemp;
	char szReadBuffer[MAX_PATH];
	char *token = NULL;
	char szSpec[] = ",";

	fpFile = fopen(lpstPath, "r");
	if (fpFile == NULL)
	{
		m_strErrorMsg.Format("Open %s file error!");
		return FALSE;
	}

	ZeroMemory(szReadBuffer, sizeof(char)*MAX_PATH);

	fgets(szReadBuffer, MAX_PATH, fpFile);//Title
	int nLineCount = 0;
	while (TRUE)
	{
		if (fgets(szReadBuffer, MAX_PATH, fpFile) == NULL)
		{
			break;
		}
		token = strtok(szReadBuffer, szSpec);//Pixel Center WL
		ASSERT(token);

		token = strtok(NULL, szSpec);//Pixel Beam WL
		ASSERT(token);
		pdblPixelWave[nLineCount] = atof(token);

		token = strtok(NULL, szSpec);//Pixel Beam GHz
		ASSERT(token);
		pdblPixelGHz[nLineCount] = atof(token);

		token = strtok(NULL, szSpec);//Pixel Beam IL
		ASSERT(token);
		pdblGetIL[nLineCount] = atof(token);

		token = strtok(NULL, szSpec);//Ratio 1
		ASSERT(token);
		pdblRatio1[nLineCount] = atof(token);

		token = strtok(NULL, szSpec);//Ratio 2
		ASSERT(token);
		pdbRatio2[nLineCount] = atof(token);

		nLineCount++;
	}
	*pnTotalLength = nLineCount;

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return TRUE;
}

BOOL CDGEModule::CalcConvolutionFilter()
{
	CString strFileName;
	stPixelInfo  stPixelData1;
	BOOL bFunctionOK;
	double * pdblWave_Beam = NULL;
	double * pdblGhz_Beam = NULL;
	double * pdblIL_Beam = NULL;
	double * pdblR1_Beam = NULL;
	double * pdblR2_Beam = NULL;
	double pdbFileterArry[PIXEL_BEAM_NUMBER][3];
	//	double pdbLeftWL[PIXEL_NUMBER_64];
	//	double pdbRightWL[PIXEL_NUMBER_64];
	FILE * fpFile = NULL;
	char szBuffer[MAX_PATH];
	HANDLE hr;
	CString strTemp;
	WIN32_FIND_DATA winFindFile;
	try
	{
		if (pdblWave_Beam == NULL)
		{
			pdblWave_Beam = (double*)VirtualAlloc(NULL, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ZeroMemory(pdblWave_Beam, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER);
		}
		if (pdblGhz_Beam == NULL)
		{
			pdblGhz_Beam = (double*)VirtualAlloc(NULL, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ZeroMemory(pdblGhz_Beam, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER);
		}
		if (pdblIL_Beam == NULL)
		{
			pdblIL_Beam = (double*)VirtualAlloc(NULL, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ZeroMemory(pdblIL_Beam, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER);
		}
		if (pdblR1_Beam == NULL)
		{
			pdblR1_Beam = (double*)VirtualAlloc(NULL, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ZeroMemory(pdblR1_Beam, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER);
		}
		if (pdblR2_Beam == NULL)
		{
			pdblR2_Beam = (double*)VirtualAlloc(NULL, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ZeroMemory(pdblR2_Beam, sizeof(double)*PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER);
		}
		strFileName.Format("%s\\Calibrate\\%s\\PixelWave.ini", m_strCaliFolder, m_strCurrentTemp);
		

		//1. 检查pixel中心波长定标是否测试过
		hr = FindFirstFile(strFileName, &winFindFile);
		if (INVALID_HANDLE_VALUE == hr)
		{
			FindClose(hr); //没找到pixel中心波长定标ini文件
			strTemp.Format("Can't find the PixelWave.ini file, next will excute testing of pixel wave calibration!");
			throw strTemp.GetBuffer(0);
		}
		else
		{
			FindClose(hr);
		}

		//2. Get the 64 pieces of pixel center wavelength
		if (!fnReadPixelWaveIni(strFileName, &stPixelData1, FALSE)) //全部中心波长
		{
			strTemp.Format("Failed at reading the pixel wavelength ini file!");
			throw	strTemp.GetBuffer(0);
		}

		strFileName.Format("%s\\Calibrate\\%s\\Beam_ILRatio.csv", m_strCaliFolder, m_strCurrentTemp);

		int nTotalGetLength = 0;
		if (!ReadILRatio_Beam(strFileName, stPixelData1, pdblWave_Beam, pdblGhz_Beam, pdblIL_Beam, pdblR1_Beam, pdblR2_Beam, &nTotalGetLength))
		{
			strTemp.Format("Failed at saving file: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
		//开始计算Convoluatin Filter
		for (int nPixelIndex = 0;nPixelIndex < PIXEL_NUMBER_64;nPixelIndex++)
		{
			for (int m = 0;m < PIXEL_BEAM_NUMBER;m++)
			{
				if (nPixelIndex == 0)
				{
					pdbFileterArry[nPixelIndex][0] = 0.0;
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex + 1]) < 0.01)
					{
						pdbFileterArry[nPixelIndex][2] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex]) < 0.01)
					{
						pdbFileterArry[nPixelIndex][1] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
				}
				else if (nPixelIndex == 63)
				{
					pdbFileterArry[nPixelIndex][2] = 0.0;
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex - 1]) < 0.005)
					{
						pdbFileterArry[nPixelIndex][0] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex]) < 0.005)
					{
						pdbFileterArry[nPixelIndex][1] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
				}
				else
				{
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex - 1]) < 0.005)
					{
						pdbFileterArry[nPixelIndex][0] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex]) < 0.005)
					{
						pdbFileterArry[nPixelIndex][1] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
					if (fabs(pdblWave_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER] - stPixelData1.dblPixelWav[nPixelIndex + 1]) < 0.005)
					{
						pdbFileterArry[nPixelIndex][2] = pdblR1_Beam[m + nPixelIndex*PIXEL_BEAM_NUMBER];
					}
				}
			}
		}

		strFileName.Format("%s\\Calibrate\\%s\\AttenDacOffset_ConvolutionFilter.csv", m_strCaliFolder, m_strCurrentTemp);

		fpFile = fopen(strFileName.GetBuffer(0), "w");
		if (fpFile == NULL)
		{
			strTemp.Format("Open file error: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
		for (int j = 0;j < PIXEL_NUMBER_64;j++)
		{
			ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);
			sprintf(szBuffer, "%lf,%lf,%lf", pdbFileterArry[j][0], pdbFileterArry[j][1], pdbFileterArry[j][2]);
			fprintf(fpFile, "%s\n", szBuffer);
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
	}
	if (pdblWave_Beam != NULL)
	{
		VirtualFree(pdblWave_Beam, 0, MEM_RELEASE);
	}
	if (pdblGhz_Beam == NULL)
	{
		VirtualFree(pdblGhz_Beam, 0, MEM_RELEASE);
	}
	if (pdblIL_Beam == NULL)
	{
		VirtualFree(pdblIL_Beam, 0, MEM_RELEASE);
	}
	if (pdblR1_Beam == NULL)
	{
		VirtualFree(pdblR1_Beam, 0, MEM_RELEASE);
	}
	if (pdblR2_Beam == NULL)
	{
		VirtualFree(pdblR2_Beam, 0, MEM_RELEASE);
	}
	return bFunctionOK;
}



///////////////////////////////////////////////////////
//函数说明：
//    读取ConvolutionFilter
//Add by wanxin
//Time:20181025
//////////////////////////////////////////////////////
BOOL CDGEModule::ReadConvolutionFilter()
{
	FILE  *fpFile = NULL;
	CString strTemp;
	CString strFileName;
	char szReadBuffer[MAX_PATH];
	char *token = NULL;
	char szSpec[] = ",";
	strFileName.Format("%s\\Calibrate\\%s\\AttenDacOffset_ConvolutionFilter.csv", m_strCaliFolder, m_strCurrentTemp);
	fpFile = fopen(strFileName.GetBuffer(0), "r");
	if (fpFile == NULL)
	{
		m_strErrorMsg.Format("Open %s file error!", strFileName);
		return FALSE;
	}

	ZeroMemory(szReadBuffer, sizeof(char)*MAX_PATH);
	int nLineCount = 0;
	while (TRUE)
	{
		if (fgets(szReadBuffer, MAX_PATH, fpFile) == NULL)
		{
			break;
		}
		token = strtok(szReadBuffer, szSpec);
		ASSERT(token);
		m_pdbFilterArry[nLineCount][0] = atof(token);

		token = strtok(NULL, szSpec);
		ASSERT(token);
		m_pdbFilterArry[nLineCount][1] = atof(token);

		token = strtok(NULL, szSpec);
		ASSERT(token);
		m_pdbFilterArry[nLineCount][2] = atof(token);

		nLineCount++;
	}
	double dbSum = 0.0;
	for (int i = 0;i < nLineCount;i++)
	{
		dbSum = 0.0;
		for (int j = 0;j < 3;j++)
		{
			dbSum += m_pdbFilterArry[i][j];
		}
		m_pdbFilterArry[i][0] = m_pdbFilterArry[i][0] / dbSum;
		m_pdbFilterArry[i][1] = m_pdbFilterArry[i][1] / dbSum;
		m_pdbFilterArry[i][2] = m_pdbFilterArry[i][2] / dbSum;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//函数说明：
//    卷积函数，卷积模板必须是2n+1(奇数)
//Add by wanxin
//Time:20181023
/////////////////////////////////////////////////////////////////////////////
BOOL CDGEModule::fnConvolution(double *pdbSource, int nSourceLength, double *pdbOut)
{
	int nAllLenth;
	double *pdbTemp = NULL;
	double *pdbCalTemp = NULL;
	int i;
	double dbTempResult;
	double pdFilter[3];

	nAllLenth = nSourceLength + 2;

	if (pdbTemp == NULL)
	{
		pdbTemp = (double*)VirtualAlloc(NULL, sizeof(double)*nAllLenth, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		ZeroMemory(pdbTemp, sizeof(double)*nAllLenth);
	}

	if (pdbCalTemp == NULL)
	{
		pdbCalTemp = (double*)VirtualAlloc(NULL, sizeof(double)*nAllLenth, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		ZeroMemory(pdbCalTemp, sizeof(double)*nAllLenth);
	}

	int nTempLength = (3 - 1) / 2;

	memcpy(pdbTemp + nTempLength, pdbSource, sizeof(double)*nSourceLength);
	dbTempResult = 0.0;
	for (i = nTempLength;i < nAllLenth - nTempLength + 1;i++)
	{
		for (int x = 0;x < 3;x++)
		{
			pdFilter[x] = m_pdbFilterArry[i - nTempLength][x];
		}
		for (int k = 0;k < 3;k++)
		{
			dbTempResult += pdbTemp[i - nTempLength + k] * pdFilter[k];
		}
		pdbCalTemp[i] = dbTempResult;
		dbTempResult = 0.0;
	}

	memcpy(pdbOut, pdbCalTemp + nTempLength, sizeof(double)*nSourceLength);

	if (pdbTemp != NULL)
	{
		VirtualFree(pdbTemp, 0, MEM_RELEASE);
		pdbTemp = NULL;
	}

	if (pdbCalTemp != NULL)
	{
		VirtualFree(pdbCalTemp, 0, MEM_RELEASE);
		pdbCalTemp = NULL;
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////
//函数说明：
//   对DAC offset数据进行卷积处理
//Add by wanxin
//Time:20181023
///////////////////////////////////////////////////////////////
BOOL CDGEModule::HandleAttenDACOffsetByConvolution(double *pdbOffDac, int nLenth)
{
	double * pdbDACTemp = NULL;
	double pdbDACOffsetTemp[PIXEL_NUMBER_64];
	double pdbDACOffsetOrignal[PIXEL_NUMBER_64];
	double pdbCalError[PIXEL_NUMBER_64];
	double pdbCalResult[64];
	double pdbFilter[3] = { 0.0,0.0,0.0 };
	CString strTemp;
	BOOL bFunctionOK = TRUE;
	ASSERT(pdbOffDac);
	if (pdbDACTemp == NULL)
	{
		pdbDACTemp = (double*)VirtualAlloc(NULL, sizeof(double)*nLenth, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		ZeroMemory(pdbDACTemp, sizeof(double)*nLenth);
	}

	try
	{
		ZeroMemory(pdbDACOffsetTemp, sizeof(double)*PIXEL_NUMBER_64);
		ZeroMemory(pdbDACOffsetOrignal, sizeof(double)*PIXEL_NUMBER_64);
		ZeroMemory(pdbCalError, sizeof(double)*PIXEL_NUMBER_64);
		memcpy(pdbDACTemp, pdbOffDac, sizeof(double)*nLenth);

		if (!ReadConvolutionFilter())
		{
			throw"Read convolution filter file error";
		}

		//利用卷积方法进行处理数据
		for (int k = 0;k < ATTEN_LUT_POINT;k++)
		{
			for (int m = 0;m < PIXEL_NUMBER_64;m++)
			{
				pdbDACOffsetTemp[m] = pdbDACTemp[k + ATTEN_LUT_POINT*m];
				pdbDACOffsetOrignal[m] = pdbDACOffsetTemp[m];
			}
			for (int i = 0;i < DACOFFSETCONVOLUTIONCOUNT;i++) //迭代12次
			{
				if (!fnConvolution(pdbDACOffsetTemp, PIXEL_NUMBER_64, pdbCalResult))
				{
					strTemp.Format("%d Lut convolution error!", k);
					throw strTemp.GetBuffer(0);
				}

				for (int m = 0;m < PIXEL_NUMBER_64;m++)
				{
					pdbCalError[m] = pdbDACOffsetOrignal[m] - pdbCalResult[m];
					pdbDACOffsetTemp[m] += pdbCalError[m];
				}
			}

			for (m = 0;m < PIXEL_NUMBER_64;m++)
			{
				pdbOffDac[k + ATTEN_LUT_POINT*m] = pdbDACOffsetTemp[m];
			}
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}


	if (pdbDACTemp != NULL)
	{
		VirtualFree(pdbDACTemp, 0, MEM_RELEASE);
		pdbDACTemp = NULL;
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnCreateAttenDacOffsetTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	int nPixelCount = 0;
	double  *pdblDac_Raw = NULL;
	double  *pdblDac_Ave = NULL;
	char    *pchDac_Offset = NULL;
	double  *pdblDacOffset = NULL;

	double  dblLutDac_Offset[CALIBRATION_TEMPER_MAX][PIXEL_NUMBER_64][ATTEN_LUT_POINT] = { 0 };

	CString strFileName;
	CString strCaliTemper;
	CString strSection;
	CString strKey;
	char    chBuf[MAX_PATH] = { 0 };
	BOOL bFunctionOK = TRUE;

	DWORD dwFirstAddr;

	try
	{
		//initial struct 
		ZeroMemory(&m_stDacOffsetTab, sizeof(m_stDacOffsetTab));
		//reserved fill in 0xFF
		memset(m_stDacOffsetTab.stDacOffset[0].pbyReserve, 0xFF, sizeof(m_stDacOffsetTab.stDacOffset[0].pbyReserve));
		memset(m_stDacOffsetTab.stDacOffset[1].pbyReserve, 0xFF, sizeof(m_stDacOffsetTab.stDacOffset[1].pbyReserve));
		memset(m_stDacOffsetTab.stDacOffset[2].pbyReserve, 0xFF, sizeof(m_stDacOffsetTab.stDacOffset[2].pbyReserve));
		memset(m_stDacOffsetTab.stDacOffset[3].pbyReserve, 0xFF, sizeof(m_stDacOffsetTab.stDacOffset[3].pbyReserve));
		memset(m_stDacOffsetTab.stDacOffset[4].pbyReserve, 0xFF, sizeof(m_stDacOffsetTab.stDacOffset[4].pbyReserve));

		//读取5个温度的LUT表： 0.1dB的256个数据
		pdblDac_Raw = new double[PIXEL_NUMBER_64*ATTEN_LUT_POINT];
		pdblDac_Ave = new double[PIXEL_NUMBER_64*ATTEN_LUT_POINT];
		pchDac_Offset = new char[PIXEL_NUMBER_64*ATTEN_LUT_POINT];

		memset(dblLutDac_Offset, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT*CALIBRATION_TEMPER_MAX);

		for (int loop = 0; loop < CALIBRATION_TEMPER_MAX; loop++)
		{
			if (loop >= 5)
			{
				continue;
			}
			//指针指向每个温度的DAC offset
			pdblDacOffset = &dblLutDac_Offset[loop][0][0];

			//cal temperature
			strCaliTemper.Format("%.0f", m_stConfigTable.dbDWTemp[loop]);
			m_strCurrentTemp.Format("%.0f", m_stConfigTable.dbDWTemp[loop]);
			nPixelCount = 0;
			memset(pdblDac_Raw, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);
			memset(pdblDac_Ave, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);
			memset(pchDac_Offset, 0, sizeof(char)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);

			//1. 读衰减256个点的LUT表
			strFileName.Format("%s\\Calibrate\\%s\\256vsLut_Raw.csv", m_strCaliFolder, strCaliTemper);
			if (!IsTheFileExist(strFileName))
			{
				strTemp.Format("file is not exist: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}
			if (!fnReadAttenLut256Csv(strFileName, pdblDac_Raw, &nPixelCount))
			{
				strTemp.Format("Get %sC Lut256Csv file error!", strCaliTemper);
				throw strTemp.GetBuffer(0);
			}
			if (PIXEL_NUMBER_64 != nPixelCount)
			{
				strTemp.Format("Get %sC Atten Lut256Csv Line: %d != Pixel num %d", strCaliTemper, nPixelCount, PIXEL_NUMBER_64);
				throw strTemp.GetBuffer(0);
			}

			//2. 读带宽内平均后的衰减256个点的LUT表
			strFileName.Format("%s\\Calibrate\\%s\\256vsLut.csv", m_strCaliFolder, strCaliTemper);
			if (!IsTheFileExist(strFileName))
			{
				strTemp.Format("file is not exist: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}
			if (!fnReadAttenLut256Csv(strFileName, pdblDac_Ave, &nPixelCount))
			{
				strTemp.Format("Get %sC Lut256Csv file error!", strCaliTemper);
				throw strTemp.GetBuffer(0);
			}
			if (PIXEL_NUMBER_64 != nPixelCount)
			{
				strTemp.Format("Get %sC Atten Lut256Csv Line: %d != Pixel num %d", strCaliTemper, nPixelCount, PIXEL_NUMBER_64);
				throw strTemp.GetBuffer(0);
			}

			//计算dac offset
			for (int nIndex = 0; nIndex < PIXEL_NUMBER_64*ATTEN_LUT_POINT; nIndex++)
			{
				pdblDacOffset[nIndex] = (pdblDac_Raw[nIndex] - pdblDac_Ave[nIndex])*ATTENOFFSETCOEFFICIENT; //Modify by wanxin,权重系数
				pchDac_Offset[nIndex] = (char)(int)(pdblDacOffset[nIndex] + 0.5);
			}
			strFileName.Format("%s\\Calibrate\\%s\\256vsLut_Offset_Old.csv", m_strCaliFolder, strCaliTemper);

			// Save new DAC Offset lut table
			if (!SaveAttenLut256Csv(strFileName, pdblDacOffset))
			{
				strTemp.Format("Save %sC DAC Offset Lut256Csv file error!", strCaliTemper);
				throw strTemp.GetBuffer(0);
			}

			if (!CalcConvolutionFilter())
			{
				strTemp.Format("%sC calculate convolution filter error!", strCaliTemper);
				throw strTemp.GetBuffer(0);
			}

			if (!HandleAttenDACOffsetByConvolution(pdblDacOffset, PIXEL_NUMBER_64*ATTEN_LUT_POINT))
			{
				strTemp.Format("%sC calculate convolution error!");
				throw strTemp.GetBuffer(0);
			}

			for (nIndex = 0; nIndex < PIXEL_NUMBER_64*ATTEN_LUT_POINT; nIndex++)
			{
				pchDac_Offset[nIndex] = (char)(int)(pdblDacOffset[nIndex] + 0.5);//2018.5.31: +0.5
			}
			//DAC Offset赋值给结构体
			//memcpy(&chLutDac_Offset[loop], pchDac_Offset, PIXEL_NUMBER_64*ATTEN_LUT_POINT*sizeof(char));
			//memcpy(&m_stDacOffsetTab.stDacOffset[loop].byDacOffset, chLutDac_Offset[loop], PIXEL_NUMBER_64*ATTEN_LUT_POINT*sizeof(char));

			//2018.4.4
			memcpy(&m_stDacOffsetTab.stDacOffset[loop].byDacOffset, pchDac_Offset, PIXEL_NUMBER_64*ATTEN_LUT_POINT * sizeof(char));

			//读取实际温度值
			strFileName.Format("%s\\Config\\ModuleConfig.ini", m_strCaliFolder);

			//Current temperature
			strSection.Format("CAL_TEMP");
			strKey.Format("Temp_ATTEN_%s", strCaliTemper);

			ZeroMemory(chBuf, sizeof(chBuf));
			GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
			strTemp = chBuf;

			if ("error" == strTemp)
			{
				strMsg.Format("%s value can't be empty", strKey);
				throw strMsg.GetBuffer(0);
			}
			else if ((atof(strTemp) > 80) || (atof(strTemp) < -20))
			{
				strMsg.Format("Error value: %s = %s", strKey, strTemp);
				throw strMsg.GetBuffer(0);
			}
			//actual temp value
			m_stDacOffsetTab.fTemp[loop] = atof(strTemp);

			//by luke.liu 2018.3.28
			//保存通道dac offset数据
			strFileName.Format("%s\\Calibrate\\%s\\256vsLut_Offset.csv", m_strCaliFolder, strCaliTemper);
			// Save new DAC Offset lut table
			if (!SaveAttenLut256Csv(strFileName, pdblDacOffset))//2018.4.4 pchDac_Offset
			{
				strTemp.Format("Save %sC DAC Offset Lut256Csv file error!", strCaliTemper);
				throw strTemp.GetBuffer(0);
			}
		}


		//1. CRC32 for Temperature
		pbyBuffer = (BYTE *)&m_stDacOffsetTab.fTemp;
		iLength = sizeof(m_stDacOffsetTab.fTemp);

		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
		inCrc32 ^= 0XFFFFFFFF;

		m_stDacOffsetTab.byCrc32Temp[0] = (byte)(inCrc32 & 0x000000FF);
		m_stDacOffsetTab.byCrc32Temp[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
		m_stDacOffsetTab.byCrc32Temp[2] = (byte)((inCrc32 >> 16) & 0x00FF);
		m_stDacOffsetTab.byCrc32Temp[3] = (byte)(inCrc32 >> 24);

		//2. CRC32 for pixel wavelength
		for (loop = 0; loop < CALIBRATION_TEMPER_MAX; loop++)
		{
			if (loop >= 5)
			{
				continue; //by luke.liu 2018.2.14只取5个温度
			}

			pbyBuffer = (BYTE *)&m_stDacOffsetTab.stDacOffset[loop];
			iLength = sizeof(m_stDacOffsetTab.stDacOffset[loop]);

			inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer + 4, iLength - 4);
			inCrc32 ^= 0XFFFFFFFF;

			m_stDacOffsetTab.stDacOffset[loop].byCrc32[0] = (byte)(inCrc32 & 0x000000FF);
			m_stDacOffsetTab.stDacOffset[loop].byCrc32[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
			m_stDacOffsetTab.stDacOffset[loop].byCrc32[2] = (byte)((inCrc32 >> 16) & 0x00FF);
			m_stDacOffsetTab.stDacOffset[loop].byCrc32[3] = (byte)(inCrc32 >> 24);
		}
		dwFirstAddr = ADDR_DAC_OFFSET_TBL;
		pbyBuffer = (BYTE *)&m_stDacOffsetTab;
		iLength = sizeof(m_stDacOffsetTab);

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Attenuation DAC Offset Lut文件存在错误";
		}
		dwFirstAddr = ADDR_DAC_OFFSET_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Attenuation DAC Offset Bundle文件存在错误";
		}

		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Attenuation DAC Offset Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}

		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	if (pdblDac_Raw != NULL)
	{
		delete[] pdblDac_Raw;
		pdblDac_Raw = NULL;
	}

	if (pdblDac_Ave != NULL)
	{
		delete[] pdblDac_Ave;
		pdblDac_Ave = NULL;
	}

	if (pchDac_Offset != NULL)
	{
		delete[] pchDac_Offset;
		pchDac_Offset = NULL;
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnReadILCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength)
{
	CString strTemp;
	CString strToken;
	int     nCount = 0;
	FILE *fpFile = NULL;
	char    chRead[MAX_PATH];
	ZeroMemory(chRead, MAX_PATH);
	BOOL bFunctionOK = TRUE;

	try
	{
		//Open file
		fpFile = fopen(lpstPath, "r");
		if (fpFile == NULL)
		{
			strTemp.Format("open file error:%s ", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		//first line
		fgets(chRead, MAX_PATH, fpFile);
		nCount = 0;
		while (TRUE)
		{
			ZeroMemory(chRead, MAX_PATH);
			if (fgets(chRead, MAX_PATH, fpFile) == NULL)
				break;
			strToken = strtok(chRead, ",");
			pdblWave[nCount] = atof(strToken);
			strToken = strtok(NULL, ",");
			pdblGhz[nCount] = atof(strToken);
			strToken = strtok(NULL, ",");
			pdblIL[nCount] = atof(strToken);

			nCount++;
		}
		*nLength = nCount;
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return bFunctionOK;
}


BOOL CDGEModule::LinearInsertValue(double *pdblXArray, double *pdblYArray, BOOL bXAxisIncrease, DWORD dwDataCount, double dblX, double *pdblY)
{
	BOOL     bFunctionOK = TRUE;
	DWORD    dwDataIndex = 0;
	double   dblDelta = 0.0001;

	// formula:(Y-Y1)/(X-X1) = (Y2-Y1)/(X2-X1)
	// ==> Y = Y1 + (Y2-Y1)/(X2-X1)*(X-X1)
	if (NULL == pdblXArray || NULL == pdblYArray || dwDataCount <= 1)
	{
		m_strErrorMsg = "the input array parameter is NULL, please check it again!";
		bFunctionOK = FALSE;
		return bFunctionOK;
	}

	// check the X axis data is increase or decrease
	if (bXAxisIncrease) // increase
	{
		if (dblX < pdblXArray[0])
		{
			// use the index =0 and 1 to do the linear insert function
			*pdblY = pdblYArray[0] + (pdblYArray[1] - pdblYArray[0]) / (pdblXArray[1] - pdblXArray[0])*(dblX - pdblXArray[0]);
		}
		else if (dblX < pdblXArray[dwDataCount - 1])
		{
			// find the X array value is more than dblX,
			// skip the for loop
			for (dwDataIndex = 1; dwDataIndex < dwDataCount; dwDataIndex++)
			{
				if (dblX < pdblXArray[dwDataIndex])
				{
					break;
				}
			}
			// 
			if (fabs(dblX - pdblXArray[dwDataIndex]) < dblDelta)
			{
				*pdblY = pdblYArray[dwDataIndex];
			}
			else if (fabs(dblX - pdblXArray[dwDataIndex - 1]) < dblDelta)
			{
				*pdblY = pdblYArray[dwDataIndex - 1];
			}
			else
			{
				// calculate the Y value
				*pdblY = pdblYArray[dwDataIndex - 1] +
					(pdblYArray[dwDataIndex] - pdblYArray[dwDataIndex - 1]) / (pdblXArray[dwDataIndex] - pdblXArray[dwDataIndex - 1])*(dblX - pdblXArray[dwDataIndex - 1]);
			}

		}
		else if (dblX < pdblXArray[dwDataCount - 1] + dblDelta)
		{
			*pdblY = pdblYArray[dwDataCount - 1];
		}
		else
		{
			// use the index = dwDataCount-2 and dwDataCount-1 to do the linear insert function
			*pdblY = pdblYArray[dwDataCount - 2] +
				(pdblYArray[dwDataCount - 1] - pdblYArray[dwDataCount - 2]) / (pdblXArray[dwDataCount - 1] - pdblXArray[dwDataCount - 2])*(dblX - pdblXArray[dwDataCount - 2]);

		}
	}
	else  // decrease
	{
		if (dblX > pdblXArray[0])
		{
			// use the index =0 and 1 to do the lineart insert function
			*pdblY = pdblYArray[0] + (pdblYArray[1] - pdblYArray[0]) / (pdblXArray[1] - pdblXArray[0])*(dblX - pdblXArray[0]);
		}
		else if (dblX > pdblXArray[dwDataCount - 1])
		{
			// find the X array value is less than dblX,
			// skip the for loop
			for (dwDataIndex = 1; dwDataIndex < dwDataCount; dwDataIndex++)
			{
				if (pdblXArray[dwDataIndex] < dblX)
				{
					break;
				}
			}
			// 
			if (fabs(dblX - pdblXArray[dwDataIndex]) < dblDelta)
			{
				*pdblY = pdblYArray[dwDataIndex];
			}
			else if (fabs(dblX - pdblXArray[dwDataIndex - 1]) < dblDelta)
			{
				*pdblY = pdblYArray[dwDataIndex - 1];
			}
			else
			{
				// calculate the Y value
				*pdblY = pdblYArray[dwDataIndex - 1] +
					(pdblYArray[dwDataIndex] - pdblYArray[dwDataIndex - 1]) / (pdblXArray[dwDataIndex] - pdblXArray[dwDataIndex - 1])*(dblX - pdblXArray[dwDataIndex - 1]);
			}

		}
		else if (dblX > pdblXArray[dwDataCount - 1] - dblDelta)
		{
			*pdblY = pdblYArray[dwDataCount - 1];
		}
		else
		{
			// use the index = dwDataCount-2 and dwDataCount-1 to do the linear insert function
			*pdblY = pdblYArray[dwDataCount - 2] +
				(pdblYArray[dwDataCount - 1] - pdblYArray[dwDataCount - 2]) / (pdblXArray[dwDataCount - 1] - pdblXArray[dwDataCount - 2])*(dblX - pdblXArray[dwDataCount - 2]);

		}
	}

	//until to here, the function is OK
	bFunctionOK = TRUE;
	return bFunctionOK;
}


BOOL CDGEModule::fnCreateILTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;
	CString strSection, strKey;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	CString strFileName;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	int     nDataNum = 0;
	double  dblStepGhz = 0;
	int     loop = 0;
	int     nIndex = 0;
	char    chBuf[MAX_PATH];
	double  dblILFreq = 0;
	double  dblTargetIL = 0;
	BOOL bFunctionOK = TRUE;
	DWORD dwFirstAddr;
	DWORD dwSampleCount;
	
	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		//initial struct 
		ZeroMemory(&m_stILTab, sizeof(m_stILTab));

		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);


		//step = 45/1000 = 0.045nm
		//by luke.liu 2018.2.8 根据不同的PN选择不同的频率范围
		dblStepGhz = (double)(m_DGEInfo.nLastSliceFreq - m_DGEInfo.nFirstSliceFreq) / (IL_LUT_POINT - 1);

		for (int i = 0; i < CALIBRATION_TEMPER_MAX; i++)
		{
			if (i >= 5)
			{
				continue;
			}

			strFileName.Format("%s\\config\\ModuleConfig.ini", m_strCaliFolder);
			m_strCurrentTemp.Format("%.0f", m_stConfigTable.dbDWTemp[i]);
			strSection.Format("CAL_TEMP");
			strKey.Format("Temp_IL_%s", m_strCurrentTemp);

			ZeroMemory(chBuf, sizeof(chBuf));
			GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
			strTemp = chBuf;
			if ("error" == strTemp)
			{
				strMsg.Format("Can't find the value of %s", strKey);
				throw strMsg.GetBuffer(0);
			}
			m_stILTab.fTemp[i] = atof(strTemp); //cal temperature

			//数组清零
			ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));

			//读取pixel Min IL定标时的数据
			//strFileName.Format("%s\\%s\\Pixel_MinIL_Scan_%s.csv", m_strCaliFolder, m_strTemperArray.GetAt(i), m_strTemperArray.GetAt(i));

			strFileName.Format("%s\\Calibrate\\%s\\Pixel_MinIL_Scan.csv", m_strCaliFolder, m_strCurrentTemp);
			if (!IsTheFileExist(strFileName))
			{
				strTemp.Format("file is not exist: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			if (!fnReadILCsvFile(strFileName, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
			{
				strTemp.Format("Error on reading file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			if (0 == i)//first section of temperature for frq and IL, others for IL
			{
				//frq and IL: 频率点从小到大
				for (loop = 0; loop < IL_LUT_POINT; loop++)
				{
					//IL插值
					dblILFreq = m_DGEInfo.nFirstSliceFreq + 180000 + loop*dblStepGhz;
					LinearInsertValue(pdblGhzBuf, pdblILBuf, FALSE, nDataNum, dblILFreq, &dblTargetIL);
					//nIndex = (SCAN_WAVE_COUNT-1) - loop*(dblStepWL/m_dblStepSize); //scan index
					//m_stILTab.freq[loop] = (float)pdblGhzBuf[SCAN_WAVE_COUNT-nIndex] - 180000;//minus 180000
					//m_stILTab.stIL[i].pwPointIL[loop] = (USHORT)(pdblILBuf[SCAN_WAVE_COUNT-nIndex]*100); //multi 100 times
					m_stILTab.freq[loop] = (float)(dblILFreq - 180000);
					m_stILTab.stIL[i].pwPointIL[loop] = (USHORT)(dblTargetIL * 100);
				}

				//CRC32 for frq
				pbyBuffer = (BYTE *)&m_stILTab.freq;
				iLength = sizeof(m_stILTab.freq);

				inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
				inCrc32 ^= 0XFFFFFFFF;

				m_stILTab.iCrc32freq = inCrc32;

				//CRC32 for IL of 1 temperature
				pbyBuffer = (BYTE *)&m_stILTab.stIL[i].pwPointIL;
				iLength = sizeof(m_stILTab.stIL[i].pwPointIL);

				inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
				inCrc32 ^= 0XFFFFFFFF;

				m_stILTab.stIL[i].iCrc32IL = inCrc32;

			}
			else
			{
				//only IL
				for (loop = 0; loop < IL_LUT_POINT; loop++)
				{
					//IL插值
					dblILFreq = m_DGEInfo.nFirstSliceFreq + 180000 + loop*dblStepGhz;
					LinearInsertValue(pdblGhzBuf, pdblILBuf, FALSE, nDataNum, dblILFreq, &dblTargetIL);
					//nIndex = (SCAN_WAVE_COUNT-1) - loop*(dblStepWL/m_dblStepSize); //scan index
					//m_stILTab.freq[loop] = (float)pdblGhzBuf[SCAN_WAVE_COUNT-nIndex] - 180000;//minus 180000
					//m_stILTab.stIL[i].pwPointIL[loop] = (USHORT)(pdblILBuf[SCAN_WAVE_COUNT-nIndex]*100); //multi 100 times
					m_stILTab.freq[loop] = (float)(dblILFreq - 180000);
					m_stILTab.stIL[i].pwPointIL[loop] = (USHORT)(dblTargetIL * 100);
				}

				//CRC32 for IL of 4 temperature
				pbyBuffer = (BYTE *)&m_stILTab.stIL[i].pwPointIL;
				iLength = sizeof(m_stILTab.stIL[i].pwPointIL);

				inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
				inCrc32 ^= 0XFFFFFFFF;

				m_stILTab.stIL[i].iCrc32IL = inCrc32;
			}

		}

		pbyBuffer = (BYTE *)&m_stILTab.fTemp;
		iLength = sizeof(m_stILTab.fTemp);

		inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer, iLength);
		inCrc32 ^= 0XFFFFFFFF;

		m_stILTab.byCrc32Temp[0] = (byte)(inCrc32 & 0x000000FF);
		m_stILTab.byCrc32Temp[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
		m_stILTab.byCrc32Temp[2] = (byte)((inCrc32 >> 16) & 0x00FF);
		m_stILTab.byCrc32Temp[3] = (byte)(inCrc32 >> 24);

		pbyBuffer = (BYTE *)&m_stILTab;
		iLength = sizeof(m_stILTab);

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Attenuation DAC Offset Lut文件存在错误";
		}
		dwFirstAddr = ADDR_IL_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建IL Bundle文件存在错误";
		}

		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载IL Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}
		Sleep(3000);

	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
		return bFunctionOK;
	}


	if (pdblWaveBuf != NULL)
	{
		VirtualFree(pdblWaveBuf, 0, MEM_RELEASE);
		pdblWaveBuf = NULL;
	}
	if (pdblGhzBuf != NULL)
	{
		VirtualFree(pdblGhzBuf, 0, MEM_RELEASE);
		pdblGhzBuf = NULL;
	}
	if (pdblILBuf != NULL)
	{
		VirtualFree(pdblILBuf, 0, MEM_RELEASE);
		pdblILBuf = NULL;
	}



	return TRUE;
}


BOOL CDGEModule::fnReadBeamCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, double *pdblRatio1, double *pdblRatio2, int *nLength)
{
	CString strTemp;
	CString strToken;
	int     nCount = 0;
	FILE * fpFile = NULL;
	char    chRead[MAX_PATH];
	BOOL bFunctinOK = TRUE;

	ZeroMemory(chRead, MAX_PATH);


	try
	{
		//Open file
		fpFile = fopen(lpstPath, "r");
		if (fpFile ==NULL)
		{
			strTemp.Format("open file error:%s ", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		//first line
		fgets(chRead, MAX_PATH, fpFile);
		nCount = 0;
		while (TRUE)
		{
			ZeroMemory(chRead, MAX_PATH);
			if (fgets(chRead, MAX_PATH, fpFile) == NULL)
				break;

			strToken = strtok(chRead, ","); //中心波长
			strToken = strtok(NULL, ",");
			pdblWave[nCount] = atof(strToken);//beam波长
			strToken = strtok(NULL, ",");
			pdblGhz[nCount] = atof(strToken); //Ghz
			strToken = strtok(NULL, ",");
			pdblIL[nCount] = atof(strToken); //IL
			strToken = strtok(NULL, ",");
			pdblRatio1[nCount] = atof(strToken); //ratio1
			strToken = strtok(NULL, ",");
			pdblRatio2[nCount] = atof(strToken);//ratio2

			nCount++;
		}

		*nLength = nCount;

		if (nCount < PIXEL_BEAM_NUMBER)
		{
			strTemp.Format("Beam num: %d is not correct!", nCount);
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctinOK =  FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return bFunctinOK;
}


BOOL CDGEModule::fnCreateBeamTab(LPCSTR lpstrBinFile)
{
	CString strTemp;
	CString strMsg;
	CString strFileName;
	BYTE *pbyBuffer = NULL;
	int iLength = 0;
	int inCrc32 = 0;
	int nGroupNum = 0;
	int nDataNum = 0;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	double  *pdblRatio1Buf = NULL;
	double  *pdblRatio2Buf = NULL;
	float   *pfValue = NULL;
	BOOL bFunctionOK = TRUE;
	DWORD dwFirstAddr;
	DWORD dwSampleCount;

	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		//initial struct 
		ZeroMemory(&m_stBeamTab, sizeof(m_stBeamTab));
		//reserved fill in 0xFF
		memset(m_stBeamTab.pbyReserve, 0xFF, sizeof(m_stBeamTab.pbyReserve));

		if (("30" != m_strTempFolder) || ("30" != m_strTempFolder))
		{
			//Beam calculation only with 30C
			m_strCurrentTemp.Format("30");
		}

		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblRatio1Buf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblRatio2Buf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		pfValue = new float[dwSampleCount];
		memset(pfValue, 0, dwSampleCount * sizeof(float));

		//插值计算每个Pixel中心波长+/-200G范围内每1G波长的IL和百分比
		int nGroupNum = PIXEL_NUMBER_64 / (PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL);//扫描次数
		for (int nGroupIndex = 0; nGroupIndex < nGroupNum; nGroupIndex++)
		{
			strFileName.Format("%s\\Calibrate\\%s\\Beam_ILRatio_loop%d.csv", m_strCaliFolder, m_strCurrentTemp, nGroupIndex + 1);

			if (!IsTheFileExist(strFileName))
			{
				strTemp.Format("file is not exist: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			//数组清零
			ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblRatio1Buf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblRatio2Buf, dwSampleCount * sizeof(double));

			if (!fnReadBeamCsvFile(strFileName, pdblWaveBuf, pdblGhzBuf, pdblILBuf, pdblRatio1Buf, pdblRatio2Buf, &nDataNum))
			{
				strTemp.Format("Get data error from: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			for (int i = 0; i < nDataNum; i++)
			{
				pfValue[i] = (float)pdblRatio2Buf[i];
			}

			//Pixel 1 to 64: 定义频率从小到大 by luke.liu 2018.2.9
			for (i = 0; i < PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL; i++)
			{
				memcpy(m_stBeamTab.stBeamInfo[i * 8 + nGroupIndex].pPixelbeam, pfValue + i*PIXEL_BEAM_NUMBER, PIXEL_BEAM_NUMBER * sizeof(float));
			}

		}

		for (int i = 0; i < PIXEL_NUMBER_64; i++)
		{
			//crc32 for every pixel of 64 piecs
			pbyBuffer = (BYTE *)&m_stBeamTab.stBeamInfo[i];
			iLength = sizeof(m_stBeamTab.stBeamInfo[i]);

			inCrc32 = CalcCRC32(NULL_DWORD, pbyBuffer + 4, iLength - 4);
			inCrc32 ^= 0XFFFFFFFF;

			m_stBeamTab.stBeamInfo[i].byCrc32[0] = (byte)(inCrc32 & 0x000000FF);
			m_stBeamTab.stBeamInfo[i].byCrc32[1] = (byte)((inCrc32 >> 8) & 0x0000FF);
			m_stBeamTab.stBeamInfo[i].byCrc32[2] = (byte)((inCrc32 >> 16) & 0x00FF);
			m_stBeamTab.stBeamInfo[i].byCrc32[3] = (byte)(inCrc32 >> 24);
		}
		pbyBuffer = (BYTE *)&m_stBeamTab;
		iLength = sizeof(m_stBeamTab);

		if (!CreateBinFile(lpstrBinFile, pbyBuffer, iLength))
		{
			throw "创建Beam Lut文件存在错误";
		}
		dwFirstAddr = ADDR_BEAM_TBL;
		if (!BundleBinFile(iLength, lpstrBinFile, dwFirstAddr))
		{
			throw "创建Beam Bundle文件存在错误";
		}
		if (!DownloadUpdateFile(lpstrBinFile))
		{
			throw "下载Beam Bundle文件存在错误";
		}

		Sleep(1000);
		if (!SetSoftWareReset())
		{
			throw"软复位DGE存在错误";
		}
		Sleep(3000);
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
		return FALSE;
	}
	if (pfValue != NULL)
	{
		delete[] pfValue;
		pfValue = NULL;
	}
	if (pdblWaveBuf != NULL)
	{
		VirtualFree(pdblWaveBuf, 0, MEM_RELEASE);
		pdblWaveBuf = NULL;
	}
	if (pdblGhzBuf != NULL)
	{
		VirtualFree(pdblGhzBuf, 0, MEM_RELEASE);
		pdblGhzBuf = NULL;
	}
	if (pdblILBuf != NULL)
	{
		VirtualFree(pdblILBuf, 0, MEM_RELEASE);
		pdblILBuf = NULL;
	}
	if (pdblRatio1Buf != NULL)
	{
		VirtualFree(pdblRatio1Buf, 0, MEM_RELEASE);
		pdblRatio1Buf = NULL;
	}
	if (pdblRatio2Buf != NULL)
	{
		VirtualFree(pdblRatio2Buf, 0, MEM_RELEASE);
		pdblRatio2Buf = NULL;
	}


	return bFunctionOK;
}
////////////////////////////////////////////////////////////////
//函数说明：
//   获取数字板MCU温度
//Add by wanxin
//Time:20200525
/////////////////////////////////////////////////////////////////
BOOL CDGEModule::GetModuleMCUTemp(double * pdbMCUTemp)
{
	CString strMsg;
	double dbGetTemp;

	if (!m_Module.GetTemper(strMsg, dbGetTemp))
	{
		Sleep(2000);
		if (!m_Module.GetTemper(strMsg, dbGetTemp))
		{
			m_strErrorMsg = m_Module.m_strLastError;
			return FALSE;
		}
	}
	*pdbMCUTemp = dbGetTemp;
	return TRUE;
}

////////////////////////////////////////////////////////////////
//函数说明：
//   获取DGE模拟板的Sensor温度
//Add by wanxin
//Time:20200525
/////////////////////////////////////////////////////////////////
BOOL CDGEModule::GetSensorTemp(int nIndex, WORD* pwADC, double* pdbGetSensorTemp)
{
	WORD wADC;
	double dbGetTemp;
	CString strMsg;
	if (nIndex == 0)
	{
		if (!m_Module.GetATDData(0, &wADC, &dbGetTemp, strMsg))
		{
			if (!m_Module.GetATDData(0, &wADC, &dbGetTemp, strMsg))
			{
				m_strErrorMsg = strMsg;
				return FALSE;
			}
		}
	}
	else if (nIndex == 1)
	{
		if (!m_Module.GetATDData(1, &wADC, &dbGetTemp, strMsg))
		{
			if (!m_Module.GetATDData(1, &wADC, &dbGetTemp, strMsg))
			{
				m_strErrorMsg = strMsg;
				return FALSE;
			}
		}
	}

	*pwADC = wADC;
	*pdbGetSensorTemp = dbGetTemp;

	return TRUE;
}



//////////////////////////////////////////////////////////////////
//函数说明：
//    读取Atten定标配置文件
//Add by wanxin
//Time:20200615
/////////////////////////////////////////////////////////////////
BOOL CDGEModule::fnReadAttenCaliCfg()
{
	CString strTemp;
	CString strToken;
	int     nCount = 0;
	FILE  *fpFile = NULL;
	char    chRead[MAX_PATH];
	int     nGetIndex, nDAC;
	int     nGetIndexBuf[PIXEL_ATTEN_CALI_NUMBER];
	int     nDACBuf[PIXEL_ATTEN_CALI_NUMBER];

	ZeroMemory(chRead, MAX_PATH);
	ZeroMemory(&nGetIndexBuf, sizeof(nGetIndexBuf));
	ZeroMemory(&nDACBuf, sizeof(nDACBuf));
	BOOL bFunctionOK = TRUE;
	int nRow = 0;
	CString strTemplatePath;
	CString strFileName;

	try
	{
		//Open file
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strCalTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strFileName.Format("%s\\Pixel_Atten_Cal_Template.csv", strTemplatePath);
		fpFile = fopen(strFileName.GetBuffer(0), "r");
		if (fpFile == NULL)
		{
			strTemp.Format("open file error:%s ", strFileName);
			throw strTemp.GetBuffer(0);
		}
		//first line
		ZeroMemory(chRead, MAX_PATH);
		fgets(chRead, MAX_PATH, fpFile);
		nRow = 0;
		nCount = 0;
		while (1)
		{
			ZeroMemory(chRead, MAX_PATH);
			if (fgets(chRead, MAX_PATH, fpFile) == NULL)
			{
				break;
			}
			strToken = strtok(chRead, ",");
			nGetIndex = atoi(strToken);
			strToken = strtok(NULL, ",");
			nDAC = atoi(strToken);

			nGetIndexBuf[nRow] = nGetIndex;
			nDACBuf[nRow] = nDAC;
			nRow++;
			nCount++;
		}

		memcpy(m_stPixelDac.iIndex, nGetIndexBuf, sizeof(int)*nCount);
		memcpy(m_stPixelDac.iSetDac, nDACBuf, sizeof(int)*nCount);
		m_nAttenCfgNumber = nCount;
		m_stPixelDac.iPixelCount = nCount;
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return bFunctionOK;
}
/////////////////////////////////////////////////////////////////////
//函数说明：
//   设置所有Pixel 的DAC值
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////////////////
BOOL CDGEModule::fnSetAllPixelDAC(WORD *wDac)
{
	CString strTemp;

	try
	{
		for (int index = 0; index < PIXEL_NUMBER_64; index++)
		{
			if (!m_Module.SetOneDACValue(index + 1, wDac[index]))
			{
				strTemp.Format("Set Pixel failed: channel=%d; DAC=%d", index + 1, wDac[index]);
				throw strTemp.GetBuffer(0);
			}

			Sleep(10);
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//函数说明：
//   按照分组7关1开或者7开1关来设置64个Pixel
//Add by wanxin
//Time:20200617
/////////////////////////////////////////////////////////////////////////////
BOOL CDGEModule::fnSetPixelGroupDAC(int nloop, stPixelOpen *stPixel, BOOL bPixelOpen)
{
	CString strTemp;

	try
	{

		if (bPixelOpen)//如果pixel是一组开的情况
		{
			for (int index = 0; index < PIXEL_NUMBER_64; index++)
			{
				if (!m_Module.SetOneDACValue(index + 1, stPixel->iBlockDac[index]))
				{
					strTemp.Format("Set Pixel failed: channel=%d; DAC=%d", index + 1, stPixel->iBlockDac[index]);
					throw strTemp.GetBuffer(0);
				}
				Sleep(10);
			}

			Sleep(1000);

			//set spcific channel to desired value
			for (int i = 0; i < PIXEL_NUMBER_64; i++)
			{
				if (i == 0 || i == 1 || i == 62 || i == 63) //Modify by wanxin，这里考虑两边的pixel不适用
				{
					if (!m_Module.SetOneDACValue(i + 1, stPixel->iBlockDac[i]))
					{
						strTemp.Format("Set Pixel failed: channel=%d; DAC=%d", i + 1, stPixel->iOpenDac[i]);
						throw strTemp.GetBuffer(0);
					}
					Sleep(20);
				}
				else
				{
					if (i%PIXEL_SET_INTERVAL == nloop)//
					{
						if (!m_Module.SetOneDACValue(i + 1, stPixel->iOpenDac[i]))
						{
							strTemp.Format("Set Pixel failed: channel=%d; DAC=%d", i + 1, stPixel->iOpenDac[i]);
							throw strTemp.GetBuffer(0);
						}

						Sleep(20);
					}
				}
			}
		}
		else//如果pixel是一组关其他开的情况
		{
			for (int index = 0; index < PIXEL_NUMBER_64; index++)
			{
				if (!m_Module.SetOneDACValue(index + 1, stPixel->iOpenDac[index]))
				{
					strTemp.Format("Set Pixel failed: channel=%d; DAC=%d", index + 1, stPixel->iOpenDac[index]);
					throw strTemp.GetBuffer(0);
				}

				Sleep(10);
			}

			Sleep(1000);

			for (int i = 0; i < PIXEL_NUMBER_64; i++)
			{
				if (i%PIXEL_SET_INTERVAL == nloop)
				{
					if (!m_Module.SetOneDACValue(i + 1, stPixel->iBlockDac[i]))
					{
						strTemp.Format("Set Pixel failed: channel=%d; DAC=%d", i + 1, stPixel->iBlockDac[i]);
						throw strTemp.GetBuffer(0);
					}
				}
			}
		}

	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	return TRUE;
}

BOOL CDGEModule::fnSetOECurve(float *pfCurveFreq, float *pfCurveIL, WORD nPoint)
{

	return m_Module.fnSetOECurve(pfCurveFreq, pfCurveIL, nPoint);

}

BOOL CDGEModule::fnSetAllPixelAtten(float *pfAtten, int nChannelNum)
{
	CString strTemp;

	try
	{
		if ((nChannelNum < PIXEL_NUMBER_64) || (nChannelNum > 96))
		{
			strTemp.Format("Wrong attenuation channel number: %d", nChannelNum);
			throw strTemp.GetBuffer(0);
		}

		if (!m_Module.SetAllPixelAtten(pfAtten, nChannelNum))
		{
			strTemp.Format("Set Pixel atten failed: channel=ALL; atten=%.2fdB", pfAtten[0]);
			throw strTemp.GetBuffer(0);
		}

	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	return TRUE;
}

BOOL CDGEModule::fnGetCurTemperIL(double *pdblIL, double* pdblFre, int *nPoints)
{
	CString strRecvData;
	BOOL bResponse = FALSE;
	CString strTemp;
	try
	{
		//SPI_FLASH_READ,IIC_FLASH_READ
		bResponse = m_Module.fnGetCurTemperIL(pdblIL, pdblFre, nPoints);
		if (!bResponse)
		{
			strTemp.Format("Read current IL Error!");
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char* pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		return FALSE;
	}

	return TRUE;
}

BOOL CDGEModule::fnGetCurTemperFreq(float *fFreq, int *nPoints)
{
	CString strRecvData;
	BOOL bResponse = TRUE;
	try
	{
		bResponse = m_Module.fnGetCurTemperFreq(fFreq, nPoints);
		if (!bResponse)
		{
			strRecvData = m_Module.GetLastError();
			throw strRecvData.GetBuffer(0);
		}
	}
	catch (char *pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		return FALSE;
	}

	return TRUE;
}



BOOL CDGEModule::fnGetCurTemperDac(WORD *wDac, int nChannel)
{
	CString strRecvData;
	BOOL bResponse = FALSE;
	WORD wData = 0;


	try
	{
		//SPI_FLASH_READ,IIC_FLASH_READ
		bResponse = m_Module.GetChannelDACValue((WORD)nChannel, wData, strRecvData);
		if (!bResponse)
		{
			throw  "Read current IL Error!";
		}

		*wDac = wData;
	}
	catch (char *pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		return FALSE;
	}

	return TRUE;
}

BOOL CDGEModule::fnSetDgeOECurve(const WORD wCurvePoint, float *pfCurveFreq, float *pfCurveIL)
{
	CString strTemp;

	try
	{
		if (!m_Module.fnSetOECurve(pfCurveFreq, pfCurveIL, wCurvePoint))
		{
			strTemp.Format("Set flatness curve error!");
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}


	return TRUE;
}


BOOL CDGEModule::fnReadAttenVerCfgFromTemplate(LPCTSTR lpstPath, float *pfAtten, int *pnLine)
{
	CString strTemp;

	int			nLine = 0;
	DWORD        dwBufSize = MAX_PATH; //"8000,"，每个频率最大字符数
	char		*lineBuf = new char[dwBufSize];
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;
	LPTSTR		pStr;
	FILE *fpFile = NULL;
	BOOL bFunctionOK = TRUE;

	try
	{

		//读取文件
		if (lpstPath == "")
		{
			strTemp.Format("File name can't be empty: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		fpFile = fopen(lpstPath, "r");
		if (fpFile == NULL)
		{
			strTemp.Format("Open File error: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		//title
		ZeroMemory(lineBuf, sizeof(char)*dwBufSize);
		fgets(lineBuf, dwBufSize, fpFile);
		//fileRawData.ReadString(lineBuf, dwBufSize);

		while (TRUE)
		{
			memset(lineBuf, 0, dwBufSize * sizeof(char));
			pStr = fgets(lineBuf, dwBufSize, fpFile);;
			if (NULL == pStr)
				break;

			token = strtok(lineBuf, seps);
			//token = strtok(NULL, seps);
			pfAtten[nLine] = atof(token);
			nLine++;
		}

		//total row for DAC 
		*pnLine = nLine;
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	if (lineBuf != NULL)
	{
		delete[] lineBuf;
	}

	return bFunctionOK;
}

////////////////////////////////////////////////////
//函数说明：
//   从终测模板配置文件中读取相应的配置值
//Add by wanxin
//Time:20200627
////////////////////////////////////////////////////
BOOL CDGEModule::fnReadAttenVerCfg(int &nSetNumber)
{
	CString strTemplatePath;
	CString strFileName;
	float* pfAttenCfg;

	strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
	strFileName.Format("%s\\Atten_Verify_Template.csv", strTemplatePath);

	//read attenuation verify configuration
	pfAttenCfg = m_fAttenCfg; //float point to array 
	if (!fnReadAttenVerCfgFromTemplate(strFileName, pfAttenCfg, &nSetNumber))
	{
		return FALSE;
	}

	return TRUE;

}

BOOL CDGEModule::fnReadOECurveCfgFile(LPCTSTR lpstPath, float *pCurveGhz, float *pCurveIL, int *pnPoint)
{
	FILE* pFile = NULL;
	CString strErrMsg;
	char pszLine[1024];
	char* pszToken = NULL;
	char pszSeps[] = ",\t\r";
	int  nCount = 0;
	CString strTemp;
	BOOL bFunctionOK = TRUE;
	try
	{
		pFile = fopen(lpstPath, "rt");
		if (pFile == NULL)
		{
			strTemp.Format("Open file error:%s", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		fgets(pszLine, 1024, pFile);
		pszToken = strtok(pszLine, pszSeps);
		pszToken = strtok(NULL, pszSeps);
		nCount = atoi(pszToken);

		if (nCount < 2 || nCount>4800)
		{
			throw "CSV point count overline";
		}
		*pnPoint = nCount;

		for (int i = 0; ; i++)
		{
			ZeroMemory(pszLine, 1024);
			if (NULL != fgets(pszLine, 1024, pFile))
			{
				pszToken = strtok(pszLine, pszSeps);
				pszToken = strtok(NULL, pszSeps);
				pCurveGhz[i] = atof(pszToken);
				pszToken = strtok(NULL, pszSeps);
				pszToken = strtok(NULL, pszSeps);
				pCurveIL[i] = atof(pszToken);
			}
			else
			{
				break;
			}
		}
	}
	catch (char* e)
	{
		m_strErrorMsg = e;
		return FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return bFunctionOK;
}


///////////////////////////////////////////////////////////
//函数说明：
//    这里通过使用Curve来自动切换到custom Mode状态
//Add by wanxin
//Time:20180822
//////////////////////////////////////////////////////////
BOOL CDGEModule::fnSetCustomMode()
{
	CString strTemp;
	CString strFileName;
	CString strMsg;
	int     nCurveTemplateNum = 0;
	int     nFlatnessCurvePoint = 0;
	float   *pfCurveFreq = new float[IL_LUT_POINT]; //1000 points
	float   *pfCurveIL = new float[IL_LUT_POINT];
	BOOL  bFunction = TRUE;
	CString strCurvePlan;
	CString strTemplatePath;


	try
	{
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strCurvePlan.Format("%s\\FlatnessCurves\\DGE_Flatness Curve%d.csv", strTemplatePath, 1);
		if (!fnReadOECurveCfgFile(strCurvePlan, pfCurveFreq, pfCurveIL, &nFlatnessCurvePoint))
		{
			strTemp.Format("Get OE Curve Plan Error");
			throw strTemp.GetBuffer(0);
		}

		//copy to large array 	  
		if (!fnSetOECurve(pfCurveFreq, pfCurveIL, nFlatnessCurvePoint))
		{
			strTemp.Format("Set OE curve failed!");
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunction = FALSE;
	}

	if (pfCurveFreq != NULL)
	{
		delete[] pfCurveFreq;
		pfCurveFreq = NULL;
	}
	if (pfCurveIL != NULL)
	{
		delete[] pfCurveIL;
		pfCurveIL = NULL;
	}

	return bFunction;
}

BOOL CDGEModule::fnSetAllPixelAttenForVer(int nLoopIndex)
{
	float fAtten;
	float pfAttenSet[PIXEL_NUMBER_64];
	fAtten = m_fAttenCfg[nLoopIndex];
	for (int k = 0; k < PIXEL_NUMBER_64; k++)
	{
		pfAttenSet[k] = fAtten;
	}

	if (!fnSetAllPixelAtten(pfAttenSet, PIXEL_NUMBER_64))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CDGEModule::AdjustFileExit(LPCSTR lpstrFileName)
{
	CString strErrMsg;
	HANDLE	bResult = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;

	bResult = FindFirstFile(lpstrFileName, &winFindFileData);

	if (bResult == INVALID_HANDLE_VALUE)
	{
		return FALSE;

	}
	FindClose(bResult);
	return TRUE;
}


BOOL CDGEModule::fnCreatReportFile(LPCTSTR lpstPath)
{
	BOOL         bFunctionOK = TRUE;
	CString      strTemp;
	CString      strFileName;
	CString      strValue;
	FILE*        pFile = NULL;
	double       dblStartWL = 0;
	double       dblStep = 0;

	try
	{
		strFileName = lpstPath;

		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		if (strFileName.Find("Curve_") != -1)
		{
			strTemp.Format("TEMP,Index");
		}
		else if (strFileName.Find("Atten_") != -1)
		{
			strTemp.Format("TEMP,Atten");
		}

		fprintf(pFile, "%s", strTemp);

		strValue.Empty();
		dblStartWL = m_stScanParameter.dblStartWL;
		dblStep = m_stScanParameter.dblStepSize;

		for (int k = 0; k < m_stScanParameter.dwSampleCount; k++)
		{
			double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
			strValue.Format(",%.6f", dblThisGHz);
			fprintf(pFile, "%s", strValue);
		}

		fprintf(pFile, "\n");
	}
	catch (TCHAR* ptszErrorMsg)
	{
		m_strErrorMsg = ptszErrorMsg;
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		CString strError("Other fatal exception occurred");
		m_strErrorMsg = strError;
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}
BOOL CDGEModule::fnReadChScanFile(LPCTSTR lpstPath, double *dblIL, double *dblPDL, double *dblGhz)
{
	CString strDir, strSaveDir, strSaveDir1;
	CString strTemp;
	char szBuffer[MAX_PATH];
	char *token = NULL;

	FILE * fpFile = NULL;

	fpFile = fopen(lpstPath, "r");
	if (fpFile == NULL)
	{
		m_strErrorMsg.Format("Open file error:%s", lpstPath);
		return FALSE;
	}

	ZeroMemory(szBuffer, MAX_PATH);
	fgets(szBuffer, MAX_PATH, fpFile);
	int nLine = 0;
	while (1)
	{
		ZeroMemory(szBuffer, MAX_PATH);
		if (fgets(szBuffer, MAX_PATH, fpFile) == NULL)
		{
			break;
		}
		token = strtok(szBuffer, ",");
		ASSERT(token);

		token = strtok(NULL, ",");
		ASSERT(token);
		dblGhz[nLine] = atof(token);
		token = strtok(NULL, ",");
		ASSERT(token);
		dblIL[nLine] = atof(token);

		if (m_stScanParameter.bDoPDL)
		{
			token = strtok(NULL, ",");
			ASSERT(token);
			dblPDL[nLine] = atof(token);
		}
		nLine++;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return TRUE;
}

BOOL CDGEModule::fnSaveReportFile(LPCTSTR lpstPath, CString strIndex, double *dblData, DWORD dwScanCount)
{
	CString strTemp;
	CString strValue;

	char	*token = NULL;
	FILE    *pFile = NULL;
	BOOL bFunctionOK = TRUE;


	try
	{

		//读取文件
		if (lpstPath == "")
		{
			strTemp.Format("File name can't be empty: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		pFile = fopen(lpstPath, "a+");		

		if (INVALID_HANDLE_VALUE == pFile)
		{
			strTemp.Format("Open file err: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		strTemp.Format("%sC, %s", m_strTempFolder, strIndex);
		//title 
		fprintf(pFile, "%s", strTemp);

		for (int nGhzIndex = 0; nGhzIndex < dwScanCount; nGhzIndex++)
		{
			strTemp.Format(",%lf", dblData[nGhzIndex]);
			fprintf(pFile, "%s", strTemp);
		}

		fprintf(pFile, "\n");

	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	if (NULL != pFile)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnSaveILScanData()
{
	CString strTemp;
	CString strMsg;
	CString strFileName;
	double  *pdblIL = NULL;
	double  *pdblPDL = NULL;
	double  *pdbGHz = NULL;
	int     nScanLoops = 0;
	float   fAttenCfg[PIXEL_NUMBER_96] = { 0 };
	float   *pfAttenCfg = NULL;
	DWORD   dwScanCount;
	CString strTemplatePath;
	int nSetNumber;
	CString strPath;
	try
	{
		dwScanCount = m_stScanParameter.dwSampleCount;

		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strFileName.Format("%s\\Atten_Verify_Template.csv", strTemplatePath);

		//read attenuation verify configuration
		pfAttenCfg = m_fAttenCfg; //float point to array 
		if (!fnReadAttenVerCfgFromTemplate(strFileName, pfAttenCfg, &nSetNumber))
		{
			return FALSE;
		}

		//创建全温汇总文件
		strPath.Format("%s\\Test", m_strTestPath);
		strFileName.Format("%s\\Atten_Scan_IL_Report.csv", strPath);

		if (!AdjustFileExit(strFileName))
		{
			if (!fnCreatReportFile(strFileName))
			{
				strTemp.Format("Creating file failed: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			CString strTargetName;
			strTargetName.Format("%s\\Atten_Scan_PDL_Report.csv", strPath);
			CopyFile(strFileName, strTargetName, FALSE);
		}

		pdblIL = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblPDL = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdbGHz = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		for (int nloop = 0; nloop < nSetNumber; nloop++)
		{
			memset(pdblIL, 0, dwScanCount * sizeof(double));
			memset(pdblPDL, 0, dwScanCount * sizeof(double));
			memset(pdbGHz, 0, dwScanCount * sizeof(double));


			strFileName.Format("%s\\%s\\Pixel_Atten_Scan_Ver_%d.txt", strPath, m_strTempFolder, nloop + 1);

			if (!fnReadChScanFile(strFileName, pdblIL, pdblPDL, pdbGHz))
			{
				strTemp.Format("Read file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			//保存IL汇总扫描数据
			strFileName.Format("%s\\Atten_Scan_IL_Report.csv", strPath);
			strMsg.Format("%.2fdB", pfAttenCfg[nloop]);

			if (!fnSaveReportFile(strFileName, strMsg, pdblIL, dwScanCount))
			{
				strTemp.Format("Save file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			//保存PDL汇总扫描数据
			strFileName.Format("%s\\Atten_Scan_PDL_Report.csv", strPath);

			if (!fnSaveReportFile(strFileName, strMsg, pdblPDL, dwScanCount))
			{
				strTemp.Format("Save file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		return FALSE;
	}

	if (pdblIL != NULL)
	{
		VirtualFree(pdblIL, 0, MEM_RELEASE);
		pdblIL = NULL;
	}
	if (pdblPDL != NULL)
	{
		VirtualFree(pdblPDL, 0, MEM_RELEASE);
		pdblPDL = NULL;
	}
	if (pdbGHz != NULL)
	{
		VirtualFree(pdbGHz, 0, MEM_RELEASE);
		pdbGHz = NULL;
	}


	return TRUE;
}


BOOL CDGEModule::fnSetDgeOECurveForVer(int nLoopIndex)
{
	CString strCurvePlan;
	CString strTemplatePath;
	double  *pdblAtten = NULL;
	float   *pfAttenBuf = NULL;
	double *pdblScanGhz = NULL;
	float   *pfCurveFreq = NULL;
	float   *pfCurveIL = NULL;
	BOOL bFunctionOK = TRUE;
	int nFlatnessCurvePoint;
	CString strTemp;

	try
	{
		pdblAtten = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pfAttenBuf = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblScanGhz = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pfCurveFreq = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pfCurveIL = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strCurvePlan.Format("%s\\FlatnessCurves\\DGE_Flatness Curve%d.csv", strTemplatePath, nLoopIndex + 1);
		
		if (!fnReadOECurveCfgFile(strCurvePlan, pfCurveFreq, pfCurveIL, &nFlatnessCurvePoint))
		{
			strTemp.Format("Get OE Curve Plan Error");
			throw strTemp.GetBuffer(0);
		}

		memcpy(&m_fTargetCurveFreq[nLoopIndex], pfCurveFreq, nFlatnessCurvePoint * sizeof(float));
		memcpy(&m_fTargetCurveIL[nLoopIndex], pfCurveIL, nFlatnessCurvePoint * sizeof(float));
		m_nTargetCurvePoint[nLoopIndex] = nFlatnessCurvePoint;

		if (nFlatnessCurvePoint > IL_LUT_POINT)
		{
			strTemp.Format("Curve point: %d > limit point: %d, Please take action...", nFlatnessCurvePoint, IL_LUT_POINT);
			throw strTemp.GetBuffer(0);
		}
		if (!fnSetDgeOECurve(nFlatnessCurvePoint, pfCurveFreq, pfCurveIL))
		{
			strTemp.Format("Set OE curve failed!");
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}
	if (pdblAtten != NULL)
	{
		VirtualFree(pdblAtten, 0, MEM_RELEASE);
		pdblAtten = NULL;
	}
	if (pfAttenBuf != NULL)
	{
		VirtualFree(pfAttenBuf, 0, MEM_RELEASE);
		pfAttenBuf = NULL;
	}
	if (pdblScanGhz != NULL)
	{
		VirtualFree(pdblScanGhz, 0, MEM_RELEASE);
		pdblScanGhz = NULL;
	}
	if (pfCurveFreq != NULL)
	{
		VirtualFree(pfCurveFreq, 0, MEM_RELEASE);
		pfCurveFreq = NULL;
	}
	if (pfCurveIL != NULL)
	{
		VirtualFree(pfCurveIL, 0, MEM_RELEASE);
		pfCurveIL = NULL;
	}
	return bFunctionOK;
}

BOOL CDGEModule::ReadCurveCountCfgFromFile()
{
	CString strTemplatePath;
	CString strFileName;
	CString strTemp;
	CString strSection;
	CString strKey;
	char szBuffer[MAX_PATH];
	//1. Read attenuation template
	strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
	strFileName.Format("%s\\FlatnessCurves\\CurveConfig.ini", strTemplatePath);

	//backup the old file
	if (!AdjustFileExit(strFileName))
	{
		strTemp.Format("No exits for CurveConfig.ini: %s", strFileName);
		m_strErrorMsg = strTemp;
		return FALSE;
	}

	//读取模板数量
	strSection.Format("CURVE_CFG");
	strKey.Format("Curve_Number");
	ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);

	GetPrivateProfileString(strSection, strKey, "error", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if ("error" == strTemp)
	{
		strTemp.Format("Can't find the value of %s", strKey);
		m_strErrorMsg = strTemp;
		return FALSE;
	}
	m_nCurveTemplateNum = atoi(szBuffer);
	return TRUE;
}

BOOL CDGEModule::fnCalcCurveFlatness(int nIndex, double *pdblIL, double *pdblActualIL, double *pdblFitGhz, double *pdblFitIL, double *pdblAccuracy)
{
	CString strTemp, strCurvePlan;
	CString strMsg;
	CString strFileName;
	double  *pdblIL_Fitting = NULL;
	double  *pdblGhz_Fitting = NULL;
	//double  *pdblActualGhz = NULL;
	float   *pfCurveFreq; //1000 points
	float   *pfCurveIL;
	double   *pdblGhzBuf; //1000 points
	double   *pdblILBuf;
	int     nFlatnessCurvePoint = 0;

	double  dblTargetIL = 0;
	double  dblStepGhz = m_stScanParameter.dblStepSize; //0.005nm
	double  dblStartWL = m_stScanParameter.dblStartWL; //1525.00
	int     nIndex_FirstSliceFreq = 0;
	int     nIndex_LastSliceFreq = 0;
	double  dblStepSlice = 0;
	DWORD   dwScanCount;
	char    chBuf[MAX_PATH] = { 0 };
	CString strTemplatePath;
	BOOL bFunctionOK = TRUE;

	try
	{
		dwScanCount = m_stScanParameter.dwSampleCount;
		pfCurveFreq = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pfCurveIL = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		memset(pfCurveFreq, 0, sizeof(pfCurveFreq));
		memset(pfCurveIL, 0, sizeof(pfCurveIL));
		memset(pdblGhzBuf, 0, sizeof(pdblGhzBuf));
		memset(pdblILBuf, 0, sizeof(pdblILBuf));

		//1. Read attenuation template
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strCurvePlan.Format("%s\\FlatnessCurves\\DGE_Flatness Curve%d.csv", strTemplatePath, nIndex);
		if (!fnReadOECurveCfgFile(strCurvePlan, pfCurveFreq, pfCurveIL, &nFlatnessCurvePoint))
		{
			strTemp.Format("Get OE Curve Plan Error");
			throw strTemp.GetBuffer(0);
		}

		pdblIL_Fitting = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblGhz_Fitting = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		memset(pdblIL_Fitting, 0, dwScanCount * sizeof(double));
		memset(pdblGhz_Fitting, 0, dwScanCount * sizeof(double));

		//Curve模板的freqency从小到大(GHZ),扫描的顺序freqency从大到小，保持一致
		for (int i = 0; i < nFlatnessCurvePoint; i++)
		{
			pdblGhzBuf[i] = (double)pfCurveFreq[nFlatnessCurvePoint - 1 - i];
			pdblILBuf[i] = (double)pfCurveIL[nFlatnessCurvePoint - 1 - i];
		}

		//only IL and freq
		for (int loop = 0; loop < dwScanCount; loop++)
		{
			//IL插值
			double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStepGhz * loop);
			LinearInsertValue(pdblGhzBuf, pdblILBuf, FALSE, nFlatnessCurvePoint, dblThisGHz, &dblTargetIL);

			pdblGhz_Fitting[loop] = dblThisGHz; //freq
			pdblIL_Fitting[loop] = dblTargetIL; //fitting IL

		}

		//find index of m_iFirstSliceFreq and m_iLastSliceFreq
		for (loop = 0; loop < dwScanCount; loop++)
		{
			if (pdblGhz_Fitting[loop] >(m_DGEInfo.nFirstSliceFreq + 180000) && pdblGhz_Fitting[loop + 1] <= (m_DGEInfo.nFirstSliceFreq + 180000))
			{
				nIndex_LastSliceFreq = loop;
				break;
			}

			if (pdblGhz_Fitting[loop] > (m_DGEInfo.nLastSliceFreq + 180000) && pdblGhz_Fitting[loop + 1] <= (m_DGEInfo.nLastSliceFreq + 180000))
			{
				nIndex_FirstSliceFreq = loop;
			}
		}

		//没有找到起始或结束频率点
		if ((0 == nIndex_FirstSliceFreq) || (0 == nIndex_LastSliceFreq))
		{
			throw "Can't find the index of Slice freqency in scan data!";
		}

		//计算spec要求范围的频率带宽取样1000个点的精度
		dblStepSlice = (m_DGEInfo.nLastSliceFreq - m_DGEInfo.nFirstSliceFreq) / 1000.0;
		double dblFreq = 0;
		for (int k = 0; k < 1000; k++)
		{
			dblFreq = pdblGhz_Fitting[nIndex_FirstSliceFreq] - k*dblStepSlice; //平均递减的频率点 
			for (loop = 0; loop < dwScanCount; loop++)
			{
				if (dblFreq < pdblGhz_Fitting[loop] && (dblFreq >= pdblGhz_Fitting[loop + 1]))
				{
					pdblActualIL[k] = pdblIL[loop];
					pdblFitGhz[k] = pdblGhz_Fitting[loop]; //Ghz for 1000 point 
					pdblFitIL[k] = pdblIL_Fitting[loop];          //IL for 1000 point
					pdblAccuracy[k] = pdblIL_Fitting[loop] - pdblIL[loop];
					break;
				}
			}
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK =  FALSE;
	}

	if (pdblIL_Fitting != NULL)
	{
		VirtualFree(pdblIL_Fitting, 0, MEM_RELEASE);
		pdblIL_Fitting = NULL;
	}

	if (pdblGhz_Fitting != NULL)
	{
		VirtualFree(pdblGhz_Fitting, 0, MEM_RELEASE);
		pdblGhz_Fitting = NULL;
	}
	if (pfCurveFreq != NULL)
	{
		VirtualFree(pfCurveFreq, 0, MEM_RELEASE);
		pfCurveFreq = NULL;
	}

	if (pfCurveIL != NULL)
	{
		VirtualFree(pfCurveIL, 0, MEM_RELEASE);
		pfCurveIL = NULL;
	}

	if (pdblGhzBuf != NULL)
	{
		VirtualFree(pdblGhzBuf, 0, MEM_RELEASE);
		pdblGhzBuf = NULL;
	}

	if (pdblILBuf != NULL)
	{
		VirtualFree(pdblILBuf, 0, MEM_RELEASE);
		pdblILBuf = NULL;
	}

	return bFunctionOK;
}


BOOL CDGEModule::fnSaveCurvePDLScanData()
{
	CString strTemp;
	CString strMsg;
	CString strSection;
	CString strKey;
	CString strFileName;
	double  *pdblIL = NULL;
	double  *pdblPDL = NULL;
	double  *pdbGHz = NULL;
	int     nScanCount = 0;
	float   fAttenCfg[PIXEL_NUMBER_96] = { 0 };
	float   *pfAttenCfg = NULL;
	char    chBuf[MAX_PATH] = { 0 };
	double   *pdblActualIL = NULL;
	double   *pdblAccuracy = NULL;
	double   *pdblFitGhz = NULL;
	double   *pdblFitIL = NULL;
	CString strTemplatePath;
	CString strPath;
	BOOL bFunctionOK = TRUE;

	try
	{

		nScanCount = m_stScanParameter.dwSampleCount;
		pdblActualIL = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblAccuracy = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblFitGhz = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblFitIL = (double*)VirtualAlloc(NULL, sizeof(double)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strFileName.Format("%s\\FlatnessCurves\\CurveConfig.ini", strTemplatePath);

		if (!AdjustFileExit(strFileName))
		{
			strTemp.Format("No exits for CurveConfig.ini: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		//读取模板数量
		strSection.Format("CURVE_CFG");
		strKey.Format("Curve_Number");
		ZeroMemory(chBuf, sizeof(chBuf));

		GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, strFileName);
		strTemp = chBuf;
		if ("error" == strTemp)
		{
			strMsg.Format("Can't find the value of %s", strKey);
			throw strMsg.GetBuffer(0);
		}
		m_nCurveTemplateNum = atoi(chBuf);

		strPath.Format("%s\\Test", m_strTestPath);
		strFileName.Format("%s\\Curve_Scan_IL_Report.csv", strPath);

		if (AdjustFileExit(strFileName))
		{
			CString strTargetName;
			strTargetName.Format("%s\\Curve_Scan_PDL_Report.csv", strPath);
			CopyFile(strFileName, strTargetName, FALSE);
		}

		pdblIL = (double*)VirtualAlloc(NULL, nScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblPDL = (double*)VirtualAlloc(NULL, nScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdbGHz = (double*)VirtualAlloc(NULL, nScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		for (int nloop = 0; nloop < m_nCurveTemplateNum; nloop++)
		{
			memset(pdblIL, 0, nScanCount * sizeof(double));
			memset(pdblPDL, 0, nScanCount * sizeof(double));
			memset(pdbGHz, 0, nScanCount * sizeof(double));

			strFileName.Format("%s\\%s\\OE_Curve_Scan_%d.txt", strPath,m_strTempFolder, nloop + 1);

			if (!fnReadChScanFile(strFileName, pdblIL, pdblPDL, pdbGHz))
			{
				strTemp.Format("Read file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			strFileName.Format("%s\\Curve_Scan_IL_Report.csv", strPath);

			if (!fnCalcCurveFlatness(nloop + 1, pdblIL, pdblActualIL, pdblFitGhz, pdblFitIL, pdblAccuracy))
			{
				strTemp.Format("Save file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			for (int i = 0; i < 4; i++)//4 rows: raw data(10k), fitting Ghz, raw data(1k), fitting IL(1k), accuracy
			{
				switch (i)
				{
				case 0:
					memset(pdblIL, 0, sizeof(pdblIL));
					memcpy(pdblIL, pdblFitGhz, FITTING_POINT * sizeof(double));
					strMsg.Format("Curve%d, Fitting_Ghz", nloop + 1);
					nScanCount = FITTING_POINT;
					break;
				case 1:
					memset(pdblIL, 0, sizeof(pdblIL));
					memcpy(pdblIL, pdblActualIL, FITTING_POINT * sizeof(double));
					strMsg.Format("Curve%d, Actual_IL", nloop + 1);
					nScanCount = FITTING_POINT;
					break;
				case 2:
					memset(pdblIL, 0, sizeof(pdblIL));
					memcpy(pdblIL, pdblFitIL, FITTING_POINT * sizeof(double));
					strMsg.Format("Curve%d, Fitting_IL", nloop + 1);
					nScanCount = FITTING_POINT;
					break;
				case 3:
					memset(pdblIL, 0, sizeof(pdblIL));
					memcpy(pdblIL, pdblAccuracy, FITTING_POINT * sizeof(double));
					strMsg.Format("Curve%d, Accuracy", nloop + 1);
					nScanCount = FITTING_POINT;
					break;
				}
				if (!fnSaveReportFile(strFileName, strMsg, pdblIL, nScanCount))
				{
					strTemp.Format("Save file error: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}
			}

			strFileName.Format("%s\\Curve_Scan_PDL_Report.csv", strPath);
			if (!fnSaveReportFile(strFileName, strMsg, pdblPDL, nScanCount))
			{
				strTemp.Format("Save file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFunctionOK = FALSE;
	}

	if (pdblIL != NULL)
	{
		VirtualFree(pdblIL, 0, MEM_RELEASE);
		pdblIL = NULL;
	}
	if (pdblPDL != NULL)
	{
		VirtualFree(pdblPDL, 0, MEM_RELEASE);
		pdblPDL = NULL;
	}
	if (pdblActualIL != NULL)
	{
		VirtualFree(pdblActualIL, 0, MEM_RELEASE);
		pdblActualIL = NULL;
	}

	if (pdblAccuracy != NULL)
	{
		VirtualFree(pdblAccuracy, 0, MEM_RELEASE);
		pdblAccuracy = NULL;
	}
	if (pdblFitGhz != NULL)
	{
		VirtualFree(pdblFitGhz, 0, MEM_RELEASE);
		pdblFitGhz = NULL;
	}
	if (pdblFitIL != NULL)
	{
		VirtualFree(pdblFitIL, 0, MEM_RELEASE);
		pdblFitIL = NULL;
	}

	return bFunctionOK;
}

void CDGEModule::ClearCurveScanVerMemory()
{
	ZeroMemory(m_fTargetCurveFreq, sizeof(m_fTargetCurveFreq));
	ZeroMemory(m_fTargetCurveIL, sizeof(m_fTargetCurveIL));
	ZeroMemory(m_nTargetCurvePoint, sizeof(m_nTargetCurvePoint));
}
BOOL CDGEModule::fnSavedCurveScanVer()
{
	CString strFileName;
	CString strPath;
	FILE *pFile;
	CString strTemp;
	CString strFreq;
	CString strIL;
	CString strValue;
	strPath.Format("%s\\Test\\%s", m_strTestPath,m_strTempFolder);
	strFileName.Format("%s\\OE_Curve_Scan_Ver.csv", strPath);

	pFile = fopen(strFileName, "a+");//from last line
	if (NULL == pFile)
	{
		m_strErrorMsg.Format("Create file failed: %s", strFileName);
		return FALSE;
	}

	strTemp.Format("Curve_Index");
	fprintf(pFile, "%s", strTemp);

	for (int k = 0; k < m_nCurveTemplateNum; k++)
	{
		strFreq.Empty();
		strIL.Empty();
		strFreq.Format("Target_%d_Freq", k + 1);
		strIL.Format("Target_%d_IL", k + 1);
		for (int m = 0; m < m_nTargetCurvePoint[k]; m++)
		{
			strValue.Format(",%.2f", m_fTargetCurveFreq[k][m]);
			strFreq += strValue;

			strValue.Format(",%.2f", m_fTargetCurveIL[k][m]);
			strIL += strValue;
		}
		fprintf(pFile, "%s\n", strFreq);
		fprintf(pFile, "%s\n", strIL);
	}
	

	if (pFile != NULL)
	{
		fflush(pFile);
		fclose(pFile);
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////////
//函数说明：
//    用来读取DGE WL Shift 验证配置模板
//Add by wanxin
//Time:20180726
//////////////////////////////////////////////////////////////////
BOOL CDGEModule::ReadDGEWLShiftVerifyCfgFromFile(LPCTSTR strFileName, pDGEWLShiftVerCfg pGetParam, int *npLineCount)
{
	BOOL  bFuntionOK = TRUE;
	int			nLine = 0;
	char        szReadBuffer[MAX_PATH];
	char* token = NULL;
	char  szSep[] = " ,\n";
	FILE * fpFile = NULL;
	CString strTemp;

	try
	{
		if (!AdjustFileExit(strFileName))
		{
			strTemp.Format("No exits for WL shift test template: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		if ((fpFile = fopen(strFileName, "r")) == NULL)
		{
			strTemp.Format("Open %s file error!", strFileName);
			throw strTemp.GetBuffer(0);
		}

		ZeroMemory(szReadBuffer, sizeof(char)*MAX_PATH);

		fgets(szReadBuffer, MAX_PATH, fpFile);
		int nLineCount = 0;
		while (TRUE)
		{
			ZeroMemory(szReadBuffer, sizeof(char)*MAX_PATH);
			if (fgets(szReadBuffer, MAX_PATH, fpFile) == NULL)
			{
				break;
			}
			token = strtok(szReadBuffer, szSep);
			ASSERT(token);
			pGetParam[nLineCount].nPixelIndex = atoi(token);
			token = strtok(NULL, szSep);
			pGetParam[nLineCount].dbAttenValue = atof(token);
			nLineCount++;
		}
		*npLineCount = nLineCount;
	}
	catch (char *error)
	{
		m_strErrorMsg = error;
		bFuntionOK = FALSE;
	}
	if (!fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return bFuntionOK;
}


BOOL CDGEModule::fnReadDGEWLShiftVerifyCfg()
{
	CString strFileName;
	CString strTemplatePath;
	strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strTestTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
	strFileName.Format("%s\\WL_Verify_Template.csv", strTemplatePath);
	if (!AdjustFileExit(strFileName))
	{
		m_strErrorMsg.Format("No exits for WL shift test template: %s", strFileName);
		return FALSE;
	}
	if (!ReadDGEWLShiftVerifyCfgFromFile(strFileName, m_stWLShiftCfg, &m_nCfgWLShiftLineCount))
	{
		m_strErrorMsg.Format("Error with reading: %s", strFileName);
		return FALSE;
	}

	if (m_nCfgWLShiftLineCount > PIXEL_NUMBER_64)
	{
		m_strErrorMsg.Format("WL Shift cfg file line count is bigger than %d", PIXEL_NUMBER_64);
		return FALSE;
	}

	return TRUE;
}

BOOL CDGEModule::fnSaveDGEWLShiftVer()
{
	CString strFileName;
	CString strPath;
	FILE * fpFile=NULL;
	CString strTemp;
	CString strValue;
	double dblStartWL;
	double dblStep;
	strPath.Format("%s\\Test\\%s", m_strTestPath,m_strTempFolder);
	strFileName.Format("%s\\Pixel_WL_Shift_Scan_Ver.csv", strPath);
	fpFile = fopen(strFileName, "w");
	if (NULL == fpFile)
	{
		m_strErrorMsg.Format("Create file failed: %s", strFileName);
		return FALSE;
	}

	strTemp.Format("Atten");
	fprintf(fpFile, "%s", strTemp);

	strValue.Empty();
	dblStartWL = m_stScanParameter.dblStartWL;
	dblStep = m_stScanParameter.dblStepSize;
	for (int k = 0; k < m_stScanParameter.dwSampleCount; k++)
	{
		double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
		strValue.Format(",%.6f", dblThisGHz);
		fprintf(fpFile, "%s", strValue);
	}
	fprintf(fpFile, "\n", strTemp);

	if (fpFile != NULL)
	{
		fflush(fpFile);
		fclose(fpFile);
	}
	return TRUE;
}

BOOL CDGEModule::fnSave0VDACScanData()
{
	CString strFileName;
	CString strPath;
	CString strTemp;
	CString strValue;
	double dblStep;
	double dblStartWL;
	FILE *fpFile = NULL;
	strPath.Format("%s\\Test\\%s", m_strTestPath, m_strTempFolder);
	strFileName.Format("%s\\Pixel_0_DAC_Scan_Ver.csv", strPath);

	fpFile = fopen(strFileName, "w");
	if (NULL == fpFile)
	{
		m_strErrorMsg.Format("Create file failed: %s", strFileName);
		return FALSE;
	}

	strTemp.Format("DAC");
	fprintf(fpFile, "%s", strTemp);

	strValue.Empty();
	dblStartWL = m_stScanParameter.dblStartWL;
	dblStep = m_stScanParameter.dblStepSize;
	for (int k = 0; k < m_stScanParameter.dwSampleCount; k++)
	{
		double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
		strValue.Format(",%.6f", dblThisGHz);
		fprintf(fpFile, "%s", strValue);
	}
	fprintf(fpFile, "\n");

	if (fpFile != NULL)
	{
		fflush(fpFile);
		fclose(fpFile);
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////
//函数说明：
//    将0 DAC扫描测试曲线汇总成report文件
//Add by wanxin
//Time:20180823
//////////////////////////////////////////////////////////////
BOOL CDGEModule::fnSumary0DACScanDataReport()
{
	CString strTemp;
	CString strValue;
	CString strFileName;
	int     nScanNumber = 0;
	WORD    wSetDac = 0;
	WORD    *pwSetDac = NULL;
	double  dblTemper = 0;
	FILE    *fpFile = NULL;
	double  dblStartWL = 0;
	double  dblStep = 0;
	char    *szReadBuffer = NULL;
	BOOL bFunction = TRUE;
	int nLength = (m_stScanParameter.dwSampleCount + 1) * 14;
	CString strPath;
	try
	{
		szReadBuffer = (char*)VirtualAlloc(NULL, sizeof(char)*nLength, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		strPath.Format("%s\\Test", m_strTestPath);
		strFileName.Format("%s\\0_DAC_Scan_Report.csv", strPath);

		fpFile = fopen(strFileName, "a+");
		if (NULL == fpFile)
		{
			strTemp.Format("Create file failed: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
		strTemp.Format("Temp,DAC");
		fprintf(fpFile, "%s", strTemp);
		strValue.Empty();
		dblStartWL = m_stScanParameter.dblStartWL;
		dblStep = m_stScanParameter.dblStepSize;
		for (int k = 0; k < m_stScanParameter.dwSampleCount; k++)
		{
			double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
			strValue.Format(",%.6f", dblThisGHz);
			fprintf(fpFile, "%s", strValue);
		}
		fprintf(fpFile, "\n", strTemp);
		
		if (fpFile != NULL)
		{
			fflush(fpFile);
			fclose(fpFile);
			fpFile = NULL;
		}
		
		strFileName.Format("%s\\%s\\Pixel_0_DAC_Scan_Ver.csv", strPath, m_strTempFolder);

		fpFile = fopen(strFileName, "rb");
		if (NULL == fpFile)
		{
			strTemp.Format("Create file failed: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
		ZeroMemory(szReadBuffer, sizeof(char)*nLength);
		if (fgets(szReadBuffer, nLength, fpFile) == NULL)//Title
		{
			strTemp.Format("Read file failed: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		ZeroMemory(szReadBuffer, sizeof(char)*nLength);
		if (fgets(szReadBuffer, nLength, fpFile) == NULL)//Title
		{
			strTemp.Format("Read file failed: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
		if (fpFile != NULL)
		{
			fclose(fpFile);
			fpFile = NULL;
		}

		strFileName.Format("%s\\0_DAC_Scan_Report.csv", strPath);

		fpFile = fopen(strFileName, "a+");
		if (NULL == fpFile)
		{
			strTemp.Format("Create file failed: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
		//这里多一个\n,需要去除掉
		int nTempLength = strlen(szReadBuffer);
		szReadBuffer[nTempLength - 1] = '\0';

		strTemp.Format("%s,%s",m_strTempFolder, szReadBuffer);
		fprintf(fpFile, "%s", strTemp);
		if (fpFile != NULL)
		{
			fclose(fpFile);
			fpFile = NULL;
		}
	}
	catch (char *e)
	{
		m_strErrorMsg = e;
		bFunction = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
	}
	if (szReadBuffer != NULL)
	{
		VirtualFree(szReadBuffer, 0, MEM_RELEASE);
		szReadBuffer = NULL;
	}
	return bFunction;
}

BOOL CDGEModule::fnCalcPixcelCenterGHzNew(double *pdbFre, double *pdbIL, int nSampleCount, double *pdbPixelWLCenter, double *pdbPixelCenterIL, int *nGetPixelCount)
{
	double dbStrtFre, dbEndFre;
	double dblMaxIL, dblMinIL;
	int nLeftIndex = -1;
	int nRightIndex = -1;
	double dblGetIL=0.0;
	double dbFullMinIL=0.0;
	double dbBaseline=0.0;

	dbStrtFre = m_DGEInfo.nFirstSliceFreq + 180000;
	dbEndFre = m_DGEInfo.nLastSliceFreq + 180000;

	double dbStartWL = VELOCITY_LIGHT / dbEndFre;
	double dbEndWL = VELOCITY_LIGHT / dbStrtFre;
	CString strTemp;
	int  nMinILIndex, nMaxILIndex;
	try
	{

		for (int nIndex = 0; nIndex < nSampleCount; nIndex++)
		{
			if ((VELOCITY_LIGHT / pdbFre[nIndex]) >= dbStartWL)
			{
				nLeftIndex = nIndex;
				break;
			}
		}
		for (nIndex = 0; nIndex < nSampleCount; nIndex++)
		{
			if ((VELOCITY_LIGHT / pdbFre[nIndex]) >= dbEndWL)
			{
				nRightIndex = nIndex;
				break;

			}
		}
		dblMaxIL = -99999.0;
		dblMinIL = 99999.0;
		//找到扫描曲线中最小插损，以最小插损-4dB作为baseline
		for (nIndex = nLeftIndex; nIndex < nRightIndex; nIndex++)
		{
			dblGetIL = pdbIL[nIndex];
			if (dblGetIL < dblMinIL)
			{
				dblMinIL = dblGetIL; //Find Minmum IL
				nMinILIndex = nIndex;
			}

			if (dblGetIL > dblMaxIL)
			{
				dblMaxIL = dblGetIL;
				nMaxILIndex = nIndex;
			}
		}

		if (dblMaxIL > 25.0)
			dblMaxIL = 25.0;

		dbFullMinIL = dblMinIL;
		dbBaseline = (dblMaxIL + dblMinIL) / 2 - 3;

		double dblTempData = 0;
		double dblMinIL = 999;
		int  nMinILIndex = -1;
		double dblMaxIL = -999;
		int  nMaxILIndex = -1;
		BOOL   bFindBasePoint = FALSE;
		BOOL   bFindFangxiang = FALSE;

		int nPixelCount = 0;

		int nLeftTempIndex = nLeftIndex;
		int nSearchCount = 0;
		while (TRUE)
		{
			int nSelectLeftIndex = -1;
			int nSelectRightIndex = -1;
			int nCount = 0;
			double dbChMinIL = 99999.0;
			int    nChMinIndex = -1;
			double dbSeletLeftFreq = 0.0;
			double dbSelecRightFreq = 0.0;

			for (nIndex = nLeftTempIndex + 1; nIndex < nRightIndex; nIndex++)
			{
				dblGetIL = pdbIL[nIndex];

				if (dblGetIL <= dbBaseline)
				{
					nSelectLeftIndex = nIndex;
					dbSeletLeftFreq = pdbFre[nIndex];
					break;
				}
			}
			if (nSelectLeftIndex != -1)
			{
				for (int j = nSelectLeftIndex + 1; j < nRightIndex; j++)
				{
					dblGetIL = pdbIL[j];
					if (dblGetIL >= dbBaseline)
					{
						nSelectRightIndex = j;
						dbSelecRightFreq = pdbFre[j];
						break;
					}
				}
				if (fabs(dbSelecRightFreq - dbSeletLeftFreq) < 50 || fabs(dbSelecRightFreq - dbSeletLeftFreq) > 150)
				{
					nLeftTempIndex = nSelectRightIndex;
					continue;
				}
			}

			if (nSelectLeftIndex == -1 && nSelectRightIndex == -1
				|| nPixelCount >= 8
				|| nSelectLeftIndex != -1 && nSelectRightIndex == -1 && nPixelCount >= 7)
			{
				break;
			}

			if (nSelectLeftIndex == -1 && nSelectRightIndex == -1)
			{
				throw" basic baseline to select left index and right index error";
			}

			if (nSelectLeftIndex >= nSelectRightIndex)
			{
				throw" basic baseline to select left index and right index error";
			}

			dbChMinIL = 99999.0;
			nChMinIndex = -1;
			double dbPeakFre;
			double dbPeakWave;
			for (int j = nSelectLeftIndex; j < nSelectRightIndex; j++)
			{
				dblGetIL = pdbIL[j];
				if (dblGetIL < dbChMinIL)
				{
					dbChMinIL = dblGetIL;
					nChMinIndex = j;
					dbPeakFre = pdbFre[j];
					dbPeakWave = VELOCITY_LIGHT / pdbFre[j];
				}
			}



			double dbLeftBWIL=0.0;
			double dbLeftBWFre=0.0;
			int  nLeftBWIndex = -1;

			double dbRightBWIL=0.0;
			double dbRightBWFre=0.0;
			int nRightBWIndex = -1;
			for (j = nSelectLeftIndex; j < nChMinIndex; j++)
			{
				dblGetIL = pdbIL[j];
				if (dblGetIL - dbChMinIL <= 2.0)
				{
					dbLeftBWIL = dblGetIL;
					dbLeftBWFre = pdbFre[j];
					nLeftBWIndex = j;
					break;

				}
			}
			for (j = nChMinIndex; j < nSelectRightIndex; j++)
			{
				dblGetIL = pdbIL[j];
				if (dblGetIL - dbChMinIL >= 2.0)
				{
					dbRightBWIL = dblGetIL;
					dbRightBWFre = pdbFre[j];
					nRightBWIndex = j;
					break;
				}
			}

			pdbPixelCenterIL[nPixelCount] = dbChMinIL;

			double dbCenterFre = (dbRightBWFre + dbLeftBWFre) / 2.0;
			pdbPixelWLCenter[nPixelCount] = VELOCITY_LIGHT / dbCenterFre;

			strTemp.Format("Center WL: Index=%d; wave=%.3f; IL=%.3f, Ghz=%.3f",
				nPixelCount + 1,
				pdbPixelWLCenter[nPixelCount],
				dbChMinIL,
				dbCenterFre);
			//LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);

			nLeftTempIndex = nSelectRightIndex;
			nPixelCount++;

		}
		*nGetPixelCount = nPixelCount;
	}
	catch (char* pstrMsg)
	{
		m_strErrorMsg = pstrMsg;
		//LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}


BOOL CDGEModule::fnSaveAttenCalScanData()
{
	CString strFileName;
	CString strTemp;
	CString strPath;
	CString strValue;
	double dblStartWL;
	double dblStep;
	FILE *pFile = NULL;
	DWORD dwSampleCount = m_stScanParameter.dwSampleCount;

	strPath.Format("%s\\Calibrate\\%s", m_strCaliFolder, m_strTempFolder);
	strFileName.Format("%s\\Pixel_Atten_Scan_1.csv",strPath);

	pFile = fopen(strFileName.GetBuffer(0), "w");
	if (NULL == pFile)
	{
		strTemp.Format("Create file failed: %s", strFileName);
		m_strErrorMsg = strTemp;
		return FALSE;
	}

	strTemp.Format("DAC");
	fprintf(pFile, "%s", strTemp);

	strValue.Empty();
	dblStartWL = m_stScanParameter.dblStartWL;
	dblStep = m_stScanParameter.dblStepSize;
	for (int k = 0; k < dwSampleCount; k++)
	{
		double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
		strValue.Format(",%.3f", dblThisGHz);
		fprintf(pFile, "%s", strValue);
	}
	fprintf(pFile, "\n");

	if (pFile != NULL)
	{
		fflush(pFile);
		fclose(pFile);
	}
	return TRUE;
}



BOOL CDGEModule::fnSaveFineAttenCalScanData()
{
	CString strFileName;
	CString strTemp;
	CString strPath;
	CString strValue;
	double dblStartWL;
	double dblStep;
	FILE *pFile = NULL;
	DWORD dwSampleCount = m_stScanParameter.dwSampleCount;

	strPath.Format("%s\\Calibrate\\%s", m_strCaliFolder, m_strTempFolder);
	strFileName.Format("%s\\Pixel_Atten_Scan_2.csv", strPath);

	pFile = fopen(strFileName.GetBuffer(0), "w");
	if (NULL == pFile)
	{
		strTemp.Format("Create file failed: %s", strFileName);
		m_strErrorMsg = strTemp;
		return FALSE;
	}

	strTemp.Format("DAC");
	fprintf(pFile, "%s", strTemp);

	strValue.Empty();
	dblStartWL = m_stScanParameter.dblStartWL;
	dblStep = m_stScanParameter.dblStepSize;
	for (int k = 0; k < dwSampleCount; k++)
	{
		double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
		strValue.Format(",%.3f", dblThisGHz);
		fprintf(pFile, "%s", strValue);
	}
	fprintf(pFile, "\n");

	if (pFile != NULL)
	{
		fflush(pFile);
		fclose(pFile);
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
//函数说明：
//   从文件中读取Pixcel wavelength校准的的pixel设置配置
//Add by wanxin
//Time:20200612
/////////////////////////////////////////////////////////////////////
BOOL CDGEModule::fnReadPixelMatrixCfg()
{
	CString strTemp;

	int			nLine = 0;
	DWORD        dwBufSize = MAX_PATH;
	char		lineBuf[MAX_PATH];
	char		seps[] = " ,\n";
	char		*token = NULL;
	LPTSTR		pStr;
	FILE * fpFile = NULL;
	BOOL bFunctionOK = TRUE;
	int *pnPixelMartrix;
	CString strFileName;
	CString strTemplatePath;

	try
	{
		pnPixelMartrix = (int*)&m_nPixelMatrix;
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strCalTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strFileName.Format("%s\\Pixel_Matrix_Template.csv", strTemplatePath);

		fpFile = fopen(strFileName.GetBuffer(0), "r");
		if (fpFile == NULL)
		{
			strTemp.Format("Open file name : %s error ");
			m_strErrorMsg = strTemp;
		}

		memset(lineBuf, 0, dwBufSize * sizeof(char));
		fgets(lineBuf, dwBufSize, fpFile);

		while (TRUE)
		{
			memset(lineBuf, 0, dwBufSize * sizeof(char));
			pStr = fgets(lineBuf, dwBufSize, fpFile);
			if (NULL == pStr)
				break;

			token = strtok(lineBuf, seps);//loop index

			for (int i = 0; i < PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL; i++)
			{
				token = strtok(NULL, seps);
				pnPixelMartrix[nLine * 8 + i] = atoi(token);
			}

			nLine++; //total 8 loop			
		}
		m_nPixeGroupg = nLine;
	}
	catch (char *error)
	{
		strTemp = error;
		m_strErrorMsg = strTemp;
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return bFunctionOK;
}


////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    读取Attenuation定标的Lut CSV文件
//Add by wanxin
//Time:20200617
////////////////////////////////////////////////////////////////////////////////
BOOL CDGEModule::fnReadAttenLut256Csv(double *pdblDac, int *pnLine)
{
	CString strTemp;

	int			nLine = 0;
	DWORD        dwBufSize = (sizeof(double) + 1)*(ATTEN_LUT_POINT + 1);
	char		*lineBuf = new char[dwBufSize];
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;

	FILE  *fpFile = NULL;
	BOOL  bFunctionOK = TRUE;
	CString strPath;
	CString strFileName;

	try
	{
		strPath.Format("%s\\Calibrate\\%s", m_strCaliFolder, m_strTempFolder);
		strFileName.Format("%s\\256vsLut.csv", strPath);
		fpFile = fopen(strFileName.GetBuffer(0), "r");
		if (fpFile == NULL)
		{
			strTemp.Format("Open file %s error！", strFileName);
			throw strTemp.GetBuffer(0);
		}

		while (TRUE)
		{
			memset(lineBuf, 0, dwBufSize);
			if (fgets(lineBuf, dwBufSize, fpFile) == NULL)
			{
				break;
			}

			token = strtok(lineBuf, seps);
			pdblDac[nLine*ATTEN_LUT_POINT] = atof(token);//atoi(token) 2018.4.4

			for (int i = 1; i < ATTEN_LUT_POINT; i++)
			{
				token = strtok(NULL, seps);
				pdblDac[nLine*ATTEN_LUT_POINT + i] = atof(token);//atoi(token) 2018.4.4
			}

			nLine++;
		}

		//total row for DAC 
		*pnLine = nLine;

	}
	catch (char *error)
	{
		strTemp = error;
		m_strErrorMsg = strTemp;
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	if (lineBuf != NULL)
	{
		delete[] lineBuf;
		lineBuf = NULL;
	}

	return bFunctionOK;
}

BOOL CDGEModule::CopyDefaultCenterWLTemplate()
{
	CString  strWavelengthTemlate;
	CString  strLocalFile;
	CString strTemp;
	CString strTemplatePath;
	CString strPath;
	BOOL bFunctionOK = TRUE;
	

	try
	{
		strTemplatePath.Format("%s\\%s_Rev%d", m_DGETemplateInfo.strCalTemplateFolder, m_DGEInfo.strPN, m_DGEInfo.nModuleVersion);
		strWavelengthTemlate.Format("%s\\PixelWave.ini", strTemplatePath);


		strPath.Format("%s\\Calibrate\\%s", m_strCaliFolder, m_strTempFolder);
		strLocalFile.Format("%s\\PixelWave.ini", strPath);

		CopyFile(strWavelengthTemlate, strLocalFile,FALSE);
		//backup the old file
		if (!IsTheFileExist(strLocalFile))
		{
			strTemp.Format("There is no PixelWave template file. Please check!:%s", strLocalFile);
			throw strTemp.GetBuffer();
		}

		strWavelengthTemlate.Format("%s\\PixelWave.Csv", strTemplatePath);
		strLocalFile.Format("%s\\PixelWave.Csv", strPath);
		CopyFile(strWavelengthTemlate, strLocalFile, FALSE);
		//backup the old file
		if (!IsTheFileExist(strLocalFile))
		{
			strTemp.Format("There is no PixelWave.csv file. Please check!:%s", strLocalFile);
			throw strTemp.GetBuffer();
		}

	}
	catch (char *e)
	{
		strTemp = e;
		m_strErrorMsg = strTemp;
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}

BOOL CDGEModule::SearchPixel_New2(double *pdblPixelWave, double *pdblPixelGhz, double *pdblGetIL, int* PixelCount, int nLoopIndex)
{
	CString strTemp;
	CString strToken;
	double dblGetWL = 0;
	double dblGetGhz = 0;
	double dblGetIL = 0;
	//double dblPixelWave_Peak[PIXEL_NUMBER_64];
	//double dblPixelGhz_Peak[PIXEL_NUMBER_64];
	//double dblPixelIL_Peak[PIXEL_NUMBER_64];
	double dblPixelWave_Center[PIXEL_NUMBER_64];
	double dblPixelGhz_Center[PIXEL_NUMBER_64];
	double dblPixelIL_Center[PIXEL_NUMBER_64];
	//int    nPixelIndex_Peak[PIXEL_NUMBER_64];
	int nIndex = 0, nIndex2 = 0, nIndex3 = 0;
	int nIndex_MinIL = 0;
	BOOL bSearchBegin = FALSE;
	double dblBasePoint = 0; 
	double dblBasePointOffet = 10;
	BOOL   bDirectDown = TRUE;
	double dblDeltaIL = 0;
	double dblPeakIL = 0;
	int    nIndex_Pixel = 0;
	int    nIndex_PeakIL = 0;
	int    nPixelCount = 0;
	double dblLeftPixelWav = 0;
	double dblRightPixelWav = 0;
	double dblLeftPixelIL = 0;
	double dblRightPixelIL = 0;
	double dblLeftPixelGhz = 0;
	double dblRightPixelGhz = 0;
	int    nIndexOffset = 0;
	double dblTarGetOffset = 2.0; //3dB
	double dbStrtFre;
	double dbEndFre;
	PDWORD     pdwDataArray = NULL;
	int   nGetScanCount = -1;
	int nLeftIndex = -1;
	int nRightIndex = -1;
	double dbFullMinIL;
	double dbBaseline;
	int nMinILIndex;
	int nMaxILIndex;
	double dbMaxIL = 0.0;
	double dbMinIL = 0.0;
	CString strPath;
	CString strFileName;
	int   nSumCount = m_stScanParameter.dwSampleCount;
	BOOL bFunctionOK = TRUE;
	double *pdblGetGhzBuf = NULL;
	double *pdblGetILBuf = NULL;
	double *pdblGetWaveBuf = NULL;

	try
	{

		pdblGetGhzBuf = (double*)VirtualAlloc(NULL,sizeof(double)*nSumCount,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
		pdblGetILBuf = (double*)VirtualAlloc(NULL, sizeof(double)*nSumCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblGetWaveBuf = (double*)VirtualAlloc(NULL, sizeof(double)*nSumCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		ZeroMemory(pdblGetGhzBuf, sizeof(double)*nSumCount);
		ZeroMemory(pdblGetILBuf, sizeof(double)*nSumCount);
		ZeroMemory(pdblGetWaveBuf, sizeof(double)*nSumCount);

		//ZeroMemory(dblPixelWave_Peak, sizeof(double)*PIXEL_NUMBER_64);
		//ZeroMemory(dblPixelIL_Peak, sizeof(double)*PIXEL_NUMBER_64);
		//ZeroMemory(dblPixelGhz_Peak, sizeof(double)*PIXEL_NUMBER_64);
		//ZeroMemory(nPixelIndex_Peak, sizeof(int)*PIXEL_NUMBER_64);

		//ZeroMemory(dblPixelWave_Center, sizeof(double)*PIXEL_NUMBER_64);
		//ZeroMemory(dblPixelIL_Center, sizeof(double)*PIXEL_NUMBER_64);
		//ZeroMemory(dblPixelGhz_Center, sizeof(double)*PIXEL_NUMBER_64);


		strPath.Format("%s\\Calibrate\\%s", m_strCaliFolder, m_strTempFolder);
		strFileName.Format("%s\\Pixel_Wave_Scan_loop%d.csv", strPath, nLoopIndex+1);

		dbMaxIL = -99999.0;
		dbMinIL = 99999.0;

		if (!ReadScanDatatFromFile(strFileName, pdblGetWaveBuf, pdblGetGhzBuf, pdblGetILBuf, &nGetScanCount))
		{
			return FALSE;
		}
		if (nGetScanCount == -1)
		{
			strTemp.Format("Read file:%s error, The scan point count is -1", strFileName);
			throw strTemp.GetBuffer(0);
		}

		if (nLoopIndex == 0 || nLoopIndex == 1 || nLoopIndex == 6 || nLoopIndex == 7) //Oper WL
		{
			dbStrtFre = m_DGEInfo.nFirstSliceFreq + 180000;
			dbEndFre = m_DGEInfo.nLastSliceFreq + 180000;

			double dbStartWL = VELOCITY_LIGHT / dbEndFre;
			double dbEndWL = VELOCITY_LIGHT / dbStrtFre;

			for (nIndex = 0; nIndex < nGetScanCount; nIndex++)
			{
				if (pdblGetWaveBuf[nIndex] >= dbStartWL)
				{
					nLeftIndex = nIndex;
					break;
				}
			}
			for (nIndex = 0; nIndex < nGetScanCount; nIndex++)
			{
				if (pdblGetWaveBuf[nIndex] >= dbEndWL)
				{
					nRightIndex = nIndex;
					break;
				}
			}
		}
		else //Full WL
		{
			nLeftIndex = 0;
			nRightIndex = nGetScanCount;
		}

		//找到扫描曲线中最小插损，以最小插损-4dB作为baseline
		for (nIndex = nLeftIndex; nIndex < nRightIndex; nIndex++)
		{
			dblGetIL = pdblGetILBuf[nIndex];
			if (dblGetIL < dbMinIL)
			{
				dbMinIL = dblGetIL;
				nMinILIndex = nIndex;
			}

			if (dblGetIL > dbMaxIL)
			{
				dbMaxIL = dblGetIL;
				nMaxILIndex = nIndex;
			}
		}

		if (dbMaxIL > 25.0)
			dbMaxIL = 25.0;

		dbFullMinIL = dbMinIL;
		dbBaseline = (dbMaxIL + dbMinIL) / 2 - 3;


		double dblTempData = 0;
		double dblMinIL = 999;
		int  nMinILIndex = -1;
		double dblMaxIL = -999;
		int  nMaxILIndex = -1;
		BOOL   bFindBasePoint = FALSE;
		BOOL   bFindFangxiang = FALSE;
		nIndex2 = 0;
		nPixelCount = 0;

		int nLeftTempIndex = nLeftIndex;
		int nSearchCount = 0;
		double dbselectLeftIL;
		double dbSelectRightIL;

		while (TRUE)
		{
			int nSelectLeftIndex = -1;
			int nSelectRightIndex = -1;
			int nCount = 0;
			double dbChMinIL = 99999.0;
			int    nChMinIndex = -1;
			double dbSeletLeftFreq= pdblGetGhzBuf[0];
			double dbSelecRightFreq= pdblGetGhzBuf[0];

			for (nIndex = nLeftTempIndex + 1; nIndex < nRightIndex; nIndex++)
			{
				dblGetIL = pdblGetILBuf[nIndex];

				if (dblGetIL <= dbBaseline)
				{
					nSelectLeftIndex = nIndex;
					dbSeletLeftFreq = pdblGetGhzBuf[nIndex];
					dbselectLeftIL = dblGetIL;
					break;
				}
			}
			if (nSelectLeftIndex != -1)
			{
				for (int j = nSelectLeftIndex + 1; j < nRightIndex; j++)
				{
					dblGetIL = pdblGetILBuf[j];
					if (dblGetIL >= dbBaseline)
					{
						nSelectRightIndex = j;
						dbSelecRightFreq = pdblGetGhzBuf[j];
						dbSelectRightIL = dblGetIL;
						break;
					}
				}

				if (fabs(dbselectLeftIL - dbSelectRightIL) > 1.0)
				{
					nLeftTempIndex = nSelectRightIndex;
					continue;
				}
				if (fabs(dbSelecRightFreq - dbSeletLeftFreq) < 50 || fabs(dbSelecRightFreq - dbSeletLeftFreq) > 250)
				{
					nLeftTempIndex = nSelectRightIndex;
					continue;
				}
			}

			if (nLoopIndex == 0 || nLoopIndex == 1 || nLoopIndex == 6 || nLoopIndex == 7)
			{
				if (nPixelCount >= 7)
				{
					break;
				}
			}


			if (nSelectLeftIndex == -1 && nSelectRightIndex == -1
				|| nPixelCount >= 8
				|| nSelectLeftIndex != -1 && nSelectRightIndex == -1 && nPixelCount >= 7)
			{
				break;
			}

			if (nSelectLeftIndex == -1 && nSelectRightIndex == -1)
			{
				throw" basic baseline to select left index and right index error";
			}

			if (nSelectLeftIndex >= nSelectRightIndex)
			{
				throw" basic baseline to select left index and right index error";
			}

			dbChMinIL = 99999.0;
			nChMinIndex = -1;
			double dbPeakFre;
			double dbPeakWave;
			for (int j = nSelectLeftIndex; j < nSelectRightIndex; j++)
			{
				dblGetIL = pdblGetILBuf[j];
				if (dblGetIL < dbChMinIL)
				{
					dbChMinIL = dblGetIL;
					nChMinIndex = j;
					dbPeakFre = pdblGetGhzBuf[j];
					dbPeakWave = pdblGetWaveBuf[j];
				}
			}

			double dbLeftBWIL;
			double dbLeftBWFre;
			int  nLeftBWIndex = -1;

			double dbRightBWIL;
			double dbRightBWFre;
			int nRightBWIndex = -1;
			for (j = nSelectLeftIndex; j < nChMinIndex; j++)
			{
				dblGetIL = pdblGetILBuf[j];
				if (dblGetIL - dbChMinIL <= 2.0)
				{
					dbLeftBWIL = dblGetIL;
					dbLeftBWFre = pdblGetGhzBuf[j];
					nLeftBWIndex = j;
					break;

				}
			}
			for (j = nChMinIndex; j < nSelectRightIndex; j++)
			{
				dblGetIL = pdblGetILBuf[j];
				if (dblGetIL - dbChMinIL >= 2.0)
				{
					dbRightBWIL = dblGetIL;
					dbRightBWFre = pdblGetGhzBuf[j];
					nRightBWIndex = j;
					break;
				}
			}



			dblPixelIL_Center[nPixelCount] = dbChMinIL;
			pdblGetIL[nPixelCount] = dbChMinIL;
			dblPixelGhz_Center[nPixelCount] = (dbRightBWFre + dbLeftBWFre) / 2.0;
			pdblPixelGhz[nPixelCount] = dblPixelGhz_Center[nPixelCount];
			dblPixelWave_Center[nPixelCount] = VELOCITY_LIGHT / dblPixelGhz_Center[nPixelCount];
			pdblPixelWave[nPixelCount] = dblPixelWave_Center[nPixelCount];


			//strTemp.Format("%d Loop  center WL: Index=%d; wave=%.3f; IL=%.3f, Ghz=%.3f",
			//	nLoopIndex,
			//	nPixelCount + 1,
			//	dblPixelWave_Center[nPixelCount],
			//	dbChMinIL,
			//	pdblPixelGhz[nPixelCount]);
			//LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

			nLeftTempIndex = nSelectRightIndex;
			nPixelCount++;
		}

		*PixelCount = nPixelCount;
	}
	catch (char*err)
	{
		strTemp = err;
		m_strErrorMsg = strTemp;
		bFunctionOK =  FALSE;
	}

	if (pdblGetGhzBuf != NULL)
	{
		VirtualFree(pdblGetGhzBuf, 0, MEM_RELEASE);
		pdblGetGhzBuf = NULL;
	}
	if (pdblGetILBuf != NULL)
	{
		VirtualFree(pdblGetILBuf, 0, MEM_RELEASE);
		pdblGetILBuf = NULL;
	}
	if (pdblGetWaveBuf != NULL)
	{
		VirtualFree(pdblGetWaveBuf, 0, MEM_RELEASE);
		pdblGetWaveBuf = NULL;
	}

	return bFunctionOK;
}

BOOL CDGEModule::ReadScanDatatFromFile(LPCSTR lpstrFile, double*pdbWL, double*pdbFreq, double *pdbIL, int* pnGetCount)
{
	FILE *fpFile = NULL;
	char szReadBuffer[MAX_PATH];
	CString strMsg;
	double dblGetWL;
	double dblGetGhz;
	double dblGetIL;
	CString strToken;
	*pnGetCount = -1;
	fpFile = fopen(lpstrFile, "r");
	if (fpFile == NULL)
	{
		strMsg.Format("Open file:%s error", lpstrFile);
		m_strErrorMsg = strMsg;
		return FALSE;
	}
	ZeroMemory(szReadBuffer, sizeof(char)*MAX_PATH);
	fgets(szReadBuffer, MAX_PATH, fpFile);
	int nCount = 0;
	while (TRUE)
	{
		ZeroMemory(szReadBuffer, sizeof(char)*MAX_PATH);
		if (fgets(szReadBuffer, MAX_PATH, fpFile) == NULL)
		{
			break;
		}

		strToken = strtok(szReadBuffer, ",\t");
		dblGetWL = atof(strToken);
		strToken = strtok(NULL, ",\t");
		dblGetGhz = atof(strToken);
		strToken = strtok(NULL, ",\t");
		dblGetIL = atof(strToken);


		pdbWL[nCount] = dblGetWL;
		pdbFreq[nCount] = dblGetGhz;
		pdbIL[nCount] = dblGetIL;
		nCount++;
	}
	*pnGetCount = nCount;

	if (fpFile!=NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return TRUE;
}
BOOL CDGEModule::fnCalLineCenterWL(stPixelInfo *pstPixel)
{
	double dbWL3, dbWL4;
	double dbIL3, dbIL4;
	double dbWL61, dbWL62;
	double dbIL61, dbIL62;
	double dbK, dbC;
	double dbWL1, dbWL2;
	double dbIL1, dbIL2;
	double dbWL63, dbWL64;
	double dbIL63, dbIL64;
	double dbDeta;

	dbWL3 = pstPixel->dblPixelWav[2];
	dbWL4 = pstPixel->dblPixelWav[3];
	dbIL3 = pstPixel->dblPixelIL[2];
	dbIL4 = pstPixel->dblPixelIL[3];

	dbWL61 = pstPixel->dblPixelWav[60];
	dbWL62 = pstPixel->dblPixelWav[61];
	dbIL61 = pstPixel->dblPixelIL[60];
	dbIL62 = pstPixel->dblPixelIL[61];


	dbK = (dbIL4 - dbIL3) / (dbWL4 - dbWL3);
	dbC = dbIL4 - dbK*dbWL4;

	dbDeta = dbWL4 - dbWL3;
	dbWL2 = dbWL3 - dbDeta;
	dbWL1 = dbWL2 - dbDeta;
	dbIL1 = dbK*dbWL1 + dbC;
	dbIL2 = dbK*dbWL2 + dbC;

	if (dbWL1 > m_stScanParameter.dblStopWL)
	{
		dbWL1 = dbWL3;
		dbIL1 = dbIL3;
	}
	if (dbWL2 > m_stScanParameter.dblStopWL)
	{
		dbWL2 = dbWL4;
		dbIL2 = dbIL4;
	}


	pstPixel->dblPixelWav[0] = dbWL1;
	pstPixel->dblPixelWav[1] = dbWL2;
	pstPixel->dblPixelIL[0] = dbIL1;
	pstPixel->dblPixelIL[1] = dbIL2;
	pstPixel->dblPixelGhz[0] = VELOCITY_LIGHT / dbWL1;
	pstPixel->dblPixelGhz[1] = VELOCITY_LIGHT / dbWL2;


	dbK = (dbIL62 - dbIL61) / (dbWL62 - dbWL61);
	dbC = dbIL62 - dbK*dbWL62;

	dbDeta = dbWL62 - dbWL61;
	dbWL63 = dbWL62 + dbDeta;
	dbWL64 = dbWL63 + dbDeta;
	dbIL63 = dbK*dbWL63 + dbC;
	dbIL64 = dbK*dbWL64 + dbC;

	if (dbWL63 < m_stScanParameter.dblStartWL)
	{
		dbWL63 = dbWL61;
		dbIL63 = dbIL61;
	}
	if (dbWL64 < m_stScanParameter.dblStartWL)
	{
		dbWL64 = dbWL62;
		dbIL64 = dbIL62;
	}

	pstPixel->dblPixelWav[62] = dbWL63;
	pstPixel->dblPixelWav[63] = dbWL64;
	pstPixel->dblPixelIL[62] = dbIL63;
	pstPixel->dblPixelIL[63] = dbIL64;
	pstPixel->dblPixelGhz[62] = VELOCITY_LIGHT / dbWL63;
	pstPixel->dblPixelGhz[63] = VELOCITY_LIGHT / dbWL64;

	return TRUE;
}
