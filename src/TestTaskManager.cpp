// 211GTester.cpp: implementation of the CTestTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestTaskManager.h"
#include "..\\include\\DataSrc.h"
#include <math.h>
#include "..\\include\\dllpro.h"
#include <algorithm>
#include <afxmt.h>



using namespace std;

BOOL g_taskstop = FALSE;

CCriticalSection g_Loglock;


// UDL 设备接口
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTestTaskManager::CTestTaskManager()
{
	::CoInitialize(NULL);
	m_nChamberOnline = -1;
	m_bChamberOn = FALSE;
	InitialParam();
}

CTestTaskManager::~CTestTaskManager()
{
	::CoUninitialize();
}


//////////////////////////////////////////////
//函数说明：
//    出事化定义参数
//Add by wanxin
//20160214
/////////////////////////////////////////////////
void CTestTaskManager::InitialParam()
{
	m_pInfoManger = NULL;
	m_pNetCommand = NULL;
	m_pGraph = NULL;
	m_bOpenTestDevice = FALSE;
	m_bStartTestTime = FALSE;
}

BOOL CTestTaskManager::RunDefaultTask(int nDefaultTaskIndex)
{
	CString strMsg, m_strUIProcessType;

	m_strCfgFileName.Format("\\\\zh-file-srv\\mfg\\sf07\\testing_templet\\SFP_VOA\\Config\\%s.ini", m_pInfoManger->GetUISendPN());
	m_strTestDataDir.Format("%s\\%s", m_strServerFloder, m_pInfoManger->GetUISendPN());

	// 无确定SN号码Log信息存在PN目录下，不到SN目录
	//m_strLogFileName.Format("%s\\LogInfo.txt", m_strTestDataDir);
	switch (nDefaultTaskIndex)
	{
	case 100://测试校准	
		//连接测试夹具
		if (!OpenSerialPortDevice())
			return FALSE;
		m_strUIProcessType = m_pInfoManger->GetUISendProcess();
		if ((m_strUIProcessType == "SFP_VOA_25") || (m_strUIProcessType == "SFP_VOA_18"))
		{
			break;
		}
		/// 连接光学测试设备
		if (!OpenTestOpticDevice())
			return FALSE;

		/// 进行光学归零//////
		if (!SystemReference())
			return FALSE;

		break;

	case 101://定标校准
		break;

	default:
		strMsg.Format("任务项为%d 不为保留任务项!", nDefaultTaskIndex);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    通过输入的功率点，按照线性内插方式计算相应功率点对应的ADC值
//Add by wanxin
//Time:20160229
///////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::CalculateLinearADC(double dbPower, double * pdbPowerArry, int * npADC, int nPointCount, int *nCalADC)
{
	double dbK, dbC;
	int nADC;

	if (dbPower < pdbPowerArry[nPointCount - 1])
	{
		dbK = (pdbPowerArry[nPointCount - 1] - pdbPowerArry[nPointCount - 2]) / (npADC[nPointCount - 1] - npADC[nPointCount - 2]);
		dbC = pdbPowerArry[nPointCount - 1] - dbK*npADC[nPointCount - 1];
		nADC = (int)((dbPower - dbC) / dbK);
	}
	else if (dbPower > pdbPowerArry[0])
	{
		dbK = (pdbPowerArry[1] - pdbPowerArry[0]) / (npADC[1] - npADC[0]);
		dbC = pdbPowerArry[1] - dbK*npADC[1];
		nADC = (int)((dbPower - dbC) / dbK);
	}
	for (int nIndex = 0; nIndex < nPointCount - 1; nIndex++)
	{
		if ((dbPower - pdbPowerArry[nIndex])*(dbPower - pdbPowerArry[nIndex + 1]) <= 0)
		{
			dbK = (pdbPowerArry[nIndex + 1] - pdbPowerArry[nIndex]) / (npADC[nIndex + 1] - npADC[nIndex]);
			dbC = pdbPowerArry[nIndex + 1] - dbK*npADC[nIndex + 1];
			nADC = (int)((dbPower - dbC) / dbK);
		}
	}
	*nCalADC = nADC;

	return TRUE;
}

///////////////////////////////////////////////////////////////////
//函数说明：
//   下载Lut表共用函数
//Add by wanxin
//Time:20200520
////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadLutTables(int nLutType)
{
	CString strTemp;
	CString strMsg;
	CString strFileName;
	CString strRecvData;
	BOOL bFunctionOK = TRUE;

	try
	{
		switch (nLutType)
		{
		case DOWNLOAD_MAPPING:
		{
			if (!fnDownloadMappingTable())
			{
				throw "所有模块下载Mapping table存在了错误";
			}
		}
		break;
		case DOWNLOAD_MFG_INFO:
		{
			if (!fnDownloadMfgInfo())
			{
				throw "所有模块下载Mfg table存在了错误";
			}
		}
		break;
		case DOWNLOAD_PARAMETER:
		{
			if (!fnDownloadParamInfo())
			{
				throw "所有模块下载param table存在了错误";
			}
		}
		break;
		case DOWNLOAD_WAVE_CALIBRATION:
		{
			if (!fnDownloadWavCalibrationTable())
			{
				throw "所有模块下载波长校准表存在了错误";
			}
		}
		break;
		case DOWNLOAD_ATTEN_CALIBRATION:
		{
			if (!fnDownloadAttenCalibrationTable())
			{
				throw "所有模块下载Attenuation table存在了错误";
			}
		}
		break;
		case DOWNLOAD_ATTEN_DAC_OFFSET:
		{
			if (!fnDownloadAttenOffsetTable())
			{
				throw "所有模块下载atten DAC Offset table存在了错误";
			}
		}
		break;
		case DOWNLOAD_IL_CALIBRATION:
		{
			if (!fnDownloadILCalibartionTable())
			{
				throw "所有模块下载IL table存在了错误";
			}
		}
		break;
		case DOWNLOAD_PIXELBEAM_CALIBRATION:
		{
			if (!fnDownloadBeamCalibartionTable())
			{
				throw "所有模块下载Beam table存在了错误";
			}
		}
		break;
		}
	}
	catch (char *error)
	{
		CString strMsg = error;
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}


///////////////////////////////////////////////////////////////////
//函数说明：
//   循环箱温度平衡函数
//Add by wanxin
//Time:20200525
//////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::WaitChamberCycle(stSettingTemp stCfg)
{
	CString strMsg;
	CTime ti0, ti1, ti2;//ti0.Format("%H_%M_%S"); 
	CTime ti3;
	CTime ti4;
	CTimeSpan ts;
	CString strTemp;
	double dblCurrentChamberTemp;
	BOOL bBreak = TRUE;
	double dblOffset = 0;
	double dblTolerance = 0;
	double  dbTempDiff;
	double  dbMCUTemp[TEST_MODULE_COUNT];
	double  dbSensor1[TEST_MODULE_COUNT];
	WORD    wSensor1ADC[TEST_MODULE_COUNT];
	double  dbSensor2[TEST_MODULE_COUNT];
	WORD    wSensor2ADC[TEST_MODULE_COUNT];
	BOOL   bFunctionOK = TRUE;
	double *pdbXData[TEST_MODULE_COUNT] = { NULL };
	double *pdbYData[TEST_MODULE_COUNT] = { NULL };
	int  nSampleCount;
	int nPlotPointIndex = 0;
	double dbXValue;
	double dbYValue;
	BOOL bFunctionOk = TRUE;

	try
	{

		InitGraphShow(0, "点数", "温度", "温度循环");
		AddPlotCount(0, "温度循环");

		if (stCfg.dbSetTemp < stCfg.dbMinTemp)
		{
			stCfg.dbSetTemp = stCfg.dbMinTemp;
			strTemp.Format("循环箱预设定温度: %.2fC,超出循环箱设定最低温度：%.2fC", stCfg.dbSetTemp, stCfg.dbMinTemp);
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}

		if (stCfg.dbSetTemp > stCfg.dbMaxTemp)
		{
			stCfg.dbSetTemp = stCfg.dbMaxTemp;
			strTemp.Format("循环箱预设定温度: %.2fC,超出循环箱设定高温度：%.2fC", stCfg.dbSetTemp, stCfg.dbMaxTemp);
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}

		strTemp.Format("循环箱预设定温度: %.2fC", stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
		{
			Sleep(3000);
			if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
			{
				throw "读取循环箱当前温度2次存在错误";
			}
		}

		strTemp.Format("当前循环箱读取温度: %.2fC", dblCurrentChamberTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		dbTempDiff = dblCurrentChamberTemp - stCfg.dbSetTemp;
		strTemp.Format("当前循环箱读取温度与预设定温度差异: %.2fC", dbTempDiff);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
		//
		ti0 = CTime::GetCurrentTime();

		if (fabs(dblCurrentChamberTemp - stCfg.dbSetTemp) < 1.0) //+/- 1度内
		{
			while (fabs(stCfg.dbSetTemp - dblCurrentChamberTemp) <= stCfg.dbTolerance)
			{
				if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
				{
					Sleep(3000);
					if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
					{
						strTemp.Format("设置循环箱温度%.2fC存在错误", stCfg.dbSetTemp);
						throw strTemp.GetBuffer(0);
					}
				}
				ti1 = CTime::GetCurrentTime();
				if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
				{
					Sleep(3000);
					if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
					{
						throw "读取循环箱当前温度2次存在错误";
					}
				}
				nPlotPointIndex++;

				for (int i = 0; i < m_nDGECount; i++)
				{
					//读取MCU温度
					if (!m_DGEModule[i].GetModuleMCUTemp(&dbMCUTemp[i]))
					{
						OpenSerialPortDevice();
						if (!m_DGEModule[i].GetModuleMCUTemp(&dbMCUTemp[i]))
						{
							strTemp.Format("读取模块%dMCU温度存在错误:%s", i, m_DGEModule[i].m_strErrorMsg);
							throw strTemp.GetBuffer(0);
						}
					}
					//读取Sensor 1温度

					if (!m_DGEModule[i].GetSensorTemp(0, &wSensor1ADC[i], &dbSensor1[i]))
					{
						OpenSerialPortDevice();
						if (!m_DGEModule[i].GetSensorTemp(0, &wSensor1ADC[i], &dbSensor1[i]))
						{
							strTemp.Format("读取模块%d Sernsor 1温度存在错误:%s", i, m_DGEModule[i].m_strErrorMsg);
							throw strTemp.GetBuffer(0);
						}
					}

					//读取Sensor 2温度
					if (!m_DGEModule[i].GetSensorTemp(1, &wSensor2ADC[i], &dbSensor2[i]))
					{
						OpenSerialPortDevice();
						if (!m_DGEModule[i].GetSensorTemp(1, &wSensor2ADC[i], &dbSensor2[i]))
						{
							strTemp.Format("读取模块%d Sernsor 2温度存在错误:%s", i);
							throw strTemp.GetBuffer(0);
						}
					}
					strTemp.Format("%s;循环箱温度%.2f;模块%d: MCU温度=%.2f; Sensor1=%.2f; Sensor2=%.2f;", ti1.Format("%H:%M:%S"), dblCurrentChamberTemp, i + 1, dbMCUTemp[i], dbSensor1[i], dbSensor2[i]);
					LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
				}

				dbXValue = nPlotPointIndex;
				dbYValue = dblCurrentChamberTemp;
				m_pGraph->PlotXY(0, dbXValue, dbYValue);

				CTimeSpan ts = ti1 - ti0;
				int nT = ts.GetTotalSeconds();
				if (nT > 120)
				{
					break;
				}
				Sleep(1000);
			}
		}
		else
		{
			if (fabs(dblCurrentChamberTemp - stCfg.dbSetTemp) > 5.0)
			{
				if (dblCurrentChamberTemp < stCfg.dbSetTemp)
				{
					//设置循环箱温度到最大温度
					if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbMaxTemp))
					{
						Sleep(200);
						if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbMaxTemp))
						{
							throw "设置循环箱到最大温度存在错误";
						}
					}
				}
				else
				{
					//设置循环箱温度到最低温度
					if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbMinTemp))
					{
						Sleep(200);
						if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbMinTemp))
						{
							throw "设置循环箱到最低温度存在错误";
						}
					}
				}
			}
			else
			{
				if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
				{
					Sleep(3000);
					if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
					{
						strTemp.Format("设置循环箱温度%.2fC存在错误", stCfg.dbSetTemp);
						throw strTemp.GetBuffer(0);
					}
				}
			}
		}


		for (int i = 0; i < m_nDGECount; i++)
		{
			if (pdbXData[i] == NULL)
			{
				pdbXData[i] = (double*)VirtualAlloc(NULL, sizeof(double) * 5000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			}
			ZeroMemory(pdbXData[i], sizeof(double) * 5000);

			if (pdbYData[i] == NULL)
			{
				pdbYData[i] = (double*)VirtualAlloc(NULL, sizeof(double) * 5000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			}
			ZeroMemory(pdbYData[i], sizeof(double) * 5000);
		}
		nSampleCount = 0;
		BOOL bSetTempFlag = FALSE;
		BOOL bTempTargetFlag = FALSE;
		while (TRUE)
		{
			ti1 = CTime::GetCurrentTime();
			if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
			{
				Sleep(3000);
				if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
				{
					throw "读取循环箱当前温度2次存在错误";
				}
			}
			nPlotPointIndex++;
			dbXValue = nPlotPointIndex;
			dbYValue = dblCurrentChamberTemp;
			m_pGraph->PlotXY(0, dbXValue, dbYValue);

			for (int i = 0; i < m_nDGECount; i++)
			{
				//读取MCU温度
				if (!m_DGEModule[i].GetModuleMCUTemp(&dbMCUTemp[i]))
				{
					OpenSerialPortDevice();
					if (!m_DGEModule[i].GetModuleMCUTemp(&dbMCUTemp[i]))
					{
						strTemp.Format("读取模块%d MCU温度存在错误:%s", i, m_DGEModule[i].m_strErrorMsg);
						throw strTemp.GetBuffer(0);
					}
				}
				//读取Sensor 1温度
				Sleep(20);
				if (!m_DGEModule[i].GetSensorTemp(0, &wSensor1ADC[i], &dbSensor1[i]))
				{
					OpenSerialPortDevice();
					if (!m_DGEModule[i].GetSensorTemp(0, &wSensor1ADC[i], &dbSensor1[i]))
					{
						strTemp.Format("读取模块%d Sensor1温度存在错误:%s", i, m_DGEModule[i].m_strErrorMsg);
						throw strTemp.GetBuffer(0);
					}
				}
				Sleep(20);
				//读取Sensor 2温度
				if (!m_DGEModule[i].GetSensorTemp(1, &wSensor2ADC[i], &dbSensor2[i]))
				{
					OpenSerialPortDevice();
					if (!m_DGEModule[i].GetSensorTemp(1, &wSensor2ADC[i], &dbSensor2[i]))
					{
						strTemp.Format("读取模块%d Sensor2温度存在错误:%s", i, m_DGEModule[i].m_strErrorMsg);
						throw strTemp.GetBuffer(0);
					}
				}
				Sleep(20);
				strTemp.Format("%s;循环箱温度%.2f;模块%d: MCU温度=%.2f; Sensor1=%.2f; Sensor2=%.2f;", ti1.Format("%H:%M:%S"), dblCurrentChamberTemp, i + 1, dbMCUTemp[i], dbSensor1[i], dbSensor2[i]);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
			}

			if (!bSetTempFlag)
			{
				ti2 = CTime::GetCurrentTime();
				ts = ti2 - ti1;
				if (ts.GetTotalMinutes() >= 30)
				{
					if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
					{
						Sleep(3000);
						if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
						{
							strTemp.Format("设置循环箱温度%.2fC存在错误", stCfg.dbSetTemp);
							throw strTemp.GetBuffer(0);
						}
					}
					bSetTempFlag = TRUE;
				}
				else
				{
					if (fabs(dblCurrentChamberTemp - stCfg.dbSetTemp) < 5.0)
					{
						if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
						{
							Sleep(200);
							if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
							{
								strTemp.Format("设置循环箱温度%.2fC存在错误", stCfg.dbSetTemp);
								throw strTemp.GetBuffer(0);
							}
						}
						bSetTempFlag = TRUE;
					}
				}
			}
			//开始计算温度的斜率截距
			for (i = 0; i < m_nDGECount; i++)
			{
				pdbXData[i][nSampleCount] = nSampleCount;
				pdbYData[i][nSampleCount] = dbSensor1[i];
			}
			nSampleCount++;

			if (nSampleCount >= 120)
			{
				double dbK;
				double dbC;
				double dbMaxT, dbMinT;
				int nMaxTIndex, nMinTTIndex;

				for (int i = 0; i < m_nDGECount; i++)
				{
					m_GernalObj.LinearFitArray(pdbXData[i], pdbYData[i], nSampleCount, dbK, dbC);

					m_GernalObj.FindMinMaxValue(pdbYData[i], nSampleCount, &dbMaxT, &nMaxTIndex, &dbMinT, &nMinTTIndex);

					strTemp.Format("模块%d Sensor1温度循环的斜率: %.6f; 温度最大值: %.2fC; 温度最小值: %.2fC", i + 1, dbK, dbMaxT, dbMinT);
					LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

					if ((fabs(dbK) < 0.002) && (fabs(dbMaxT - dbMinT) < 0.5)) //有一个模块满足即满足温度循环
					{
						strTemp.Format("模块%d Sensor1温度循环的斜率: %.6f; 温度最大值: %.2fC; 温度最小值: %.2fC，满足温度平衡要求，即将退出循环", i + 1, dbK, dbMaxT, dbMinT);
						LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
						bTempTargetFlag = TRUE;
					}
					ZeroMemory(pdbXData[i], sizeof(double) * 5000);
					ZeroMemory(pdbYData[i], sizeof(double) * 5000);
				}
				nSampleCount = 0; //重新计数
			}

			if (bTempTargetFlag)
			{
				break;
			}

			ti3 = CTime::GetCurrentTime();

			ts = ti3 - ti0;
			if (ts.GetTotalMinutes() > 60)//默认1个半小时自动退出温度循环
			{
				strTemp.Format("循环箱温度循环时间达到最大限，自动跳出温度循环");
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
				break;
			}

			Sleep(1000);
		}
		if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
		{
			Sleep(3000);
			if (!m_cChamber.fnModBusSetSetPoint(stCfg.dbSetTemp))
			{
				strTemp.Format("设置循环箱温度%.2fC存在错误", stCfg.dbSetTemp);
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *pstrMsg)
	{
		strMsg = pstrMsg;
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		bFunctionOk = FALSE;
	}

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (pdbXData[i] != NULL)
		{
			VirtualFree(pdbXData[i], 0, MEM_RELEASE);
			pdbXData[i] = NULL;
		}

		if (pdbYData[i] != NULL)
		{
			VirtualFree(pdbYData[i], 0, MEM_RELEASE);
			pdbYData[i] = NULL;
		}
	}

	return bFunctionOk;
}


//////////////////////////////////////////////////////////////
//函数说明：
//   下载产品相关信息的任务
//Add by wanxin
//Time:20200520
////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::DownloadProductionInfo(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case MFGInfoDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_MFG_INFO);
		strTaskCaption.Format("DGE默认数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("MFG信息下载");
		if (bFunctionOK)
		{
			UploadTestResult(MFGInfoDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(MFGInfoDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case PixelMappingDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_MAPPING);
		strTaskCaption.Format("DGE默认数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("Mapping信息下载");
		if (bFunctionOK)
		{
			UploadTestResult(PixelMappingDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(PixelMappingDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}

	case DefaultParamDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_PARAMETER);
		strTaskCaption.Format("DGE默认数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("默认参数信息下载");
		if (bFunctionOK)
		{
			UploadTestResult(DefaultParamDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(DefaultParamDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::DoRBeforeTestCheck(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case RefCheck:
	{



		strTaskCaption.Format("归零检查", m_stCfg.dbSetTemp);
		strDataName.Format("归零光路检查", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(RefCheck, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(RefCheck, strTaskCaption, strDataName, 0, 2, 0);
		}
	}
	break;
	case Circulator1_2Check:
	{
		strTaskCaption.Format("归零检查", m_stCfg.dbSetTemp);
		strDataName.Format("环形器Port1-Port2检查", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(RefCheck, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(RefCheck, strTaskCaption, strDataName, 0, 2, 0);
		}
	}
	break;
	case Circulator2_3Check:
	{
		strTaskCaption.Format("归零检查", m_stCfg.dbSetTemp);
		strDataName.Format("环形器Port2-Port3检查", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(RefCheck, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(RefCheck, strTaskCaption, strDataName, 0, 2, 0);
		}
	}
	break;
	case Product_SlotCheck:
	{
		strTaskCaption.Format("产品测试检查", m_stCfg.dbSetTemp);
		strDataName.Format("槽位检查", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(Product_SlotCheck, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(Product_SlotCheck, strTaskCaption, strDataName, 0, 2, 0);
		}
	}
	break;
	case Product_OpticalCheck:
	{
		strTaskCaption.Format("产品测试检查", m_stCfg.dbSetTemp);
		strDataName.Format("接线检查", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(Product_OpticalCheck, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(Product_OpticalCheck, strTaskCaption, strDataName, 0, 2, 0);
		}
	}
	break;
	}


	return TRUE;
}

BOOL CTestTaskManager::DGECalibrationFor30C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T30CCycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T30CCycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CCycle, strTaskCaption, strDataName, 0, 2, 0);
		}

		break;
	}
	case T30CAttenCal:
	{
		if (!fnCal_PixelAtten())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CAttenCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CAttenCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T30CAttenFineCal:
	{
		if (!fnCalFined_PixelAtten())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CAttenFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CAttenFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T30CCenterWLFineCal:
	{
		if (!fnCalFined_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Center Wavelength Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T30CAtteLutOptimization:
	{
		if (!fnCal_AttenLutAfterFindedPixelCenter())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Atten Lut 优化定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T30CILCal:
	{
		if (!fnCal_PixelIL())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel IL 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CILCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CILCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T30CBeamOffsetCal:
	{
		if (!fnCalOffset_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel beam offset定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T30CBeamRelationCal:
	{
		if (!fnCal_PixelBeam())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Beam Relation定标");
		if (bFunctionOK)
		{
			UploadTestResult(T30CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T30CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}

	return bFunctionOK;

}
BOOL CTestTaskManager::DGECalibrationForF5C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;

	switch (nTaskIndex)
	{
	case TF5CCycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(TF5CCycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CCycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CAttenCal:
	{
		if (!fnCal_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten 定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CAttenCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CAttenCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CAttenFineCal:
	{
		if (!fnCalFined_PixelAtten())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CAttenFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CAttenFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CCenterWLFineCal:
	{
		if (!fnCalFined_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Center Wavelength Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CAtteLutOptimization:
	{
		if (!fnCal_AttenLutAfterFindedPixelCenter())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Atten Lut 优化定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CILCal:
	{
		if (!fnCal_PixelIL())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel IL 定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CILCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CILCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CBeamOffsetCal:
	{
		if (!fnCalOffset_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel beam offset定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5CBeamRelationCal:
	{
		if (!fnCal_PixelBeam())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Beam Relation 定标");
		if (bFunctionOK)
		{
			UploadTestResult(TF5CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;
}
BOOL CTestTaskManager::DGECalibrationFor5C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T5CCycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T5CCycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CCycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CAttenCal:
	{
		if (!fnCal_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CAttenCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CAttenCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CAttenFineCal:
	{
		if (!fnCalFined_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CAttenFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CAttenFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CCenterWLFineCal:
	{
		if (!fnCalFined_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Center Wavelength Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CAtteLutOptimization:
	{
		if (!fnCal_AttenLutAfterFindedPixelCenter())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Atten Lut 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CILCal:
	{
		if (!fnCal_PixelIL())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel IL 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CILCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CILCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CBeamOffsetCal:
	{
		if (!fnCalOffset_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel beam offset定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T5CBeamRelationCal:
	{
		if (!fnCal_PixelBeam())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Beam Relation 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T5CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T5CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}

	return bFunctionOK;
}
BOOL CTestTaskManager::DGECalibrationFor15C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T15CCycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T15CCycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CCycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CAttenCal:
	{
		if (!fnCal_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CAttenCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CAttenCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CAttenFineCal:
	{
		if (!fnCalFined_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CAttenFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CAttenFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CCenterWLFineCal:
	{
		if (!fnCalFined_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Center Wavelength Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CAtteLutOptimization:
	{
		if (!fnCal_AttenLutAfterFindedPixelCenter())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Atten Lut 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CILCal:
	{
		if (!fnCal_PixelIL())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel IL 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CILCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CILCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CBeamOffsetCal:
	{
		if (!fnCalOffset_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel beam offset定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T15CBeamRelationCal:
	{
		if (!fnCal_PixelBeam())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Beam Relation 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T15CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T15CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}

	return bFunctionOK;
}
BOOL CTestTaskManager::DGECalibrationFor50C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;

	switch (nTaskIndex)
	{
	case T50CCycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T50CCycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CCycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CAttenCal:
	{
		if (!fnCal_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CAttenCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CAttenCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CAttenFineCal:
	{
		if (!fnCalFined_PixelAtten())
		{
			bFunctionOK = FALSE;
		}

		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Center Wavelength Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CAttenFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CAttenFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CCenterWLFineCal:
	{
		if (!fnCalFined_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CAtteLutOptimization:
	{
		if (!fnCal_AttenLutAfterFindedPixelCenter())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Atten Lut 优化定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CILCal:
	{
		if (!fnCal_PixelIL())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel IL 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CILCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CILCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CBeamOffsetCal:
	{
		if (!fnCalOffset_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel beam offset定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T50CBeamRelationCal:
	{
		if (!fnCal_PixelBeam())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Beam Relation 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T50CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T50CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::DGECalibrationFor70C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T70CCycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T70CCycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CCycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CAttenCal:
	{
		if (!fnCal_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CAttenCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CAttenCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CAttenFineCal:
	{
		if (!fnCalFined_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Atten Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CAttenFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CAttenFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CCenterWLFineCal:
	{
		if (!fnCalFined_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Center Wavelength Fine 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CAtteLutOptimization:
	{
		if (!fnCal_AttenLutAfterFindedPixelCenter())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Atten Lut 优化定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CAtteLutOptimization, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CCenterWLFineCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CILCal:
	{
		if (!fnCal_PixelIL())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel IL 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CILCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CILCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CBeamOffsetCal:
	{
		if (!fnCalOffset_PixelCenterWave())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel beam offset定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CBeamOffsetCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70CBeamRelationCal:
	{
		if (!fnCal_PixelBeam())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0fC温度定标", m_stCfg.dbSetTemp);
		strDataName.Format("Pixel Beam Relation 定标");
		if (bFunctionOK)
		{
			UploadTestResult(T70CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70CBeamRelationCal, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::DownloadCalLut(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;

	switch (nTaskIndex)
	{
	case CenterWLLutDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_WAVE_CALIBRATION);

		strTaskCaption.Format("DGE数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("下载中心波长Lut表");
		if (bFunctionOK)
		{
			UploadTestResult(CenterWLLutDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(CenterWLLutDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case AttenLutDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_ATTEN_CALIBRATION);
		strTaskCaption.Format("DGE数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("下载衰减Lut表");
		if (bFunctionOK)
		{
			UploadTestResult(AttenLutDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(AttenLutDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case AttenOffsetDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_ATTEN_DAC_OFFSET);
		strTaskCaption.Format("DGE数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("下载衰减Offset Lut表");
		if (bFunctionOK)
		{
			UploadTestResult(AttenOffsetDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(AttenOffsetDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case ILLutDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_IL_CALIBRATION);
		strTaskCaption.Format("DGE数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("下载IL Lut表");
		if (bFunctionOK)
		{
			UploadTestResult(ILLutDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(ILLutDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case BeamLutDW:
	{
		bFunctionOK = fnDownloadLutTables(DOWNLOAD_PIXELBEAM_CALIBRATION);
		strTaskCaption.Format("DGE数据下载", m_stCfg.dbSetTemp);
		strDataName.Format("下载Beam Lut表");
		if (bFunctionOK)
		{
			UploadTestResult(BeamLutDW, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(BeamLutDW, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;
}

BOOL CTestTaskManager::DGEVerifyFor70C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T70Cycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T70Cycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70Cycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70AttenVerify:
	{
		if (!fnVer_PixelAtten())//70度重新创建report文件
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Atten 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T70AttenVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70AttenVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70FlatnessCurveVerify:
	{
		if (!fnVer_OECurve())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Flatness Curve 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T70FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T70WLShiftVerify:
	{
		if (!fnVer_WLShift())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("波长shif验证");
		if (bFunctionOK)
		{
			UploadTestResult(T70WLShiftVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T70WLShiftVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T700VDACScanVerify:
	{
		if (!fnVer_0DACScanData())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("0V曲线验证");
		if (bFunctionOK)
		{
			UploadTestResult(T700VDACScanVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T700VDACScanVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;
}
BOOL CTestTaskManager::DGEVerifyFor55C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;

	switch (nTaskIndex)
	{
	case T55Cycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T55Cycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T55Cycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T55AttenVerify:
	{
		if (!fnVer_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Atten 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T55AttenVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T55AttenVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T55FlatnessCurveVerify:
	{
		if (!fnVer_OECurve())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Flatness Curve 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T55FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T55FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T55WLShiftVerify:
	{
		if (!fnVer_WLShift())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("波长shif验证");
		if (bFunctionOK)
		{
			UploadTestResult(T55WLShiftVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T55WLShiftVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T550VDACScanVerify:
	{
		if (!fnVer_0DACScanData())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("0V曲线验证");
		if (bFunctionOK)
		{
			UploadTestResult(T550VDACScanVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T550VDACScanVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;
}


BOOL CTestTaskManager::DGEVerifyFor40C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T40Cycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T40Cycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T40Cycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T40AttenVerify:
	{
		if (!fnVer_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Atten 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T40AttenVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T40AttenVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T40FlatnessCurveVerify:
	{
		if (!fnVer_OECurve())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Flatness Curve 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T40FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T40FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T40WLShiftVerify:
	{
		if (!fnVer_WLShift())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("波长shif验证");
		if (bFunctionOK)
		{
			UploadTestResult(T40WLShiftVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T40WLShiftVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T400VDACScanVerify:
	{
		if (!fnVer_0DACScanData())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("0V曲线验证");
		if (bFunctionOK)
		{
			UploadTestResult(T400VDACScanVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T400VDACScanVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;
}
BOOL CTestTaskManager::DGEVerifyFor25C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;
	switch (nTaskIndex)
	{
	case T25Cycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(T25Cycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T25Cycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T25AttenVerify:
	{
		if (!fnVer_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Atten 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T25AttenVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T25AttenVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T25FlatnessCurveVerify:
	{
		if (!fnVer_OECurve())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Flatness Curve 验证");
		if (bFunctionOK)
		{
			UploadTestResult(T25FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T25FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T25WLShiftVerify:
	{
		if (!fnVer_WLShift())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("波长shif验证");
		if (bFunctionOK)
		{
			UploadTestResult(T25WLShiftVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T25WLShiftVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case T250VDACScanVerify:
	{
		if (!fnVer_0DACScanData())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("0V曲线验证");
		if (bFunctionOK)
		{
			UploadTestResult(T250VDACScanVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(T250VDACScanVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;
}
BOOL CTestTaskManager::DGEVerifyForF5C(int nTaskIndex)
{
	BOOL bFunctionOK = TRUE;
	CString strTaskCaption;
	CString strDataName;

	switch (nTaskIndex)
	{
	case TF5Cycle:
	{
		if (!WaitChamberCycle(m_stCfg))
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE温度循环", m_stCfg.dbSetTemp);
		strDataName.Format("%.0f温度循环", m_stCfg.dbSetTemp);
		if (bFunctionOK)
		{
			UploadTestResult(TF5Cycle, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5Cycle, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5AttenVerify:
	{
		if (!fnVer_PixelAtten())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Atten 验证");
		if (bFunctionOK)
		{
			UploadTestResult(TF5AttenVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5AttenVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5FlatnessCurveVerify:
	{
		if (!fnVer_OECurve())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("Flatness Curve 验证");
		if (bFunctionOK)
		{
			UploadTestResult(TF5FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5FlatnessCurveVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF5WLShiftVerify:
	{
		if (!fnVer_WLShift())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("波长shif验证");
		if (bFunctionOK)
		{
			UploadTestResult(TF5WLShiftVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF5WLShiftVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	case TF50VDACScanVerify:
	{
		if (!fnVer_0DACScanData())
		{
			bFunctionOK = FALSE;
		}
		strTaskCaption.Format("DGE%.0f温度验证", m_stCfg.dbSetTemp);
		strDataName.Format("0V曲线验证");
		if (bFunctionOK)
		{
			UploadTestResult(TF50VDACScanVerify, strTaskCaption, strDataName, 0, 2, 1);
		}
		else
		{
			UploadTestResult(TF50VDACScanVerify, strTaskCaption, strDataName, 0, 2, 0);
		}
		break;
	}
	}
	return bFunctionOK;

}


//////////////////////////////////////////////////////////
//函数说明：
//   做非PDL扫描归零
//Add by wanxin
//Time:20200822
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::DoNoPDLScanReference()
{
	CString strRefPath;
	CString strMsg;
	CString strTemp;
	CString	strFileName;
	BOOL  bFunctionOK = TRUE;
	int i = 0;
	try
	{

		strRefPath.Format(".\\Reference");
		CreateFullPath(strRefPath);
		m_bRefDone = FALSE;
		if (!m_bOpenTestDevice)
		{
			throw "请先初始化相应光学测试设备!";
		}
		if (!ReadScanConfig())
		{
			throw "从配置文件中读取扫描配置存在错误!";

		}
		m_stScanParameter.bDoPDL = FALSE;
		m_FSTPObj.m_strRefFileName.Format("%s\\NoPDL_Reference.csv", strRefPath);


		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
	//	InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

		strMsg = _T("是否调用现在的归零参数?\n" "是: 调用上次归零参数!\n" "否: 执行归零步骤!");
		DWORD dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
		if (XSB_YES == dwBoxRet)
		{
			if (!AdjustFileExit(m_FSTPObj.m_strRefFileName))
			{
				strMsg.Format("%s reference file not exsit", m_FSTPObj.GetErrorMsg());
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return	FALSE;
			}
			if (!m_FSTPObj.ReadReferenceDataFromFile())
			{
				strMsg.Format("%s", m_FSTPObj.GetErrorMsg());
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return	FALSE;
			}

			return TRUE;
		}

		strMsg = _T("请将光路系统In端接Out端");
		dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);


		strMsg.Format("开始进行归零扫描.......");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		if (!m_FSTPObj.DoReference(m_FSTPObj.m_nGUID))
		{
			strMsg = m_FSTPObj.GetErrorMsg();
			throw strMsg.GetBuffer(0);
		}
		int nPlotIndex = 0;
		for (int i = 0; i < m_stScanParameter.nPMCount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanRefResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanRefResult.pdbPMResult[i];

			strTemp.Format("通道%d", i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
		}

		strMsg.Format("不带PDL系统归零完成!");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		m_bRefDone = TRUE;
	}
	catch (TCHAR* pstrErrorMsg)
	{
		strMsg.Format("%s", pstrErrorMsg);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	return bFunctionOK;
}


/////////////////////////////////////////////////////////
//函数说明：
//	 不带PDL扫描函数
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////
BOOL CTestTaskManager::DoNoPDLScanTest()
{
	CString strMsg;
	CString strTemp;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	CTime time;
	if (!ReadScanConfig())
	{
		strMsg.Format("从配置文件中读取扫描配置存在错误!");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		return FALSE;
	}
	time = ::GetCurrentTime();
	strTemp = time.Format("%y%m%d%H%M%S");
	strTempPath.Format(".\\Data\\ScanTemp");
	CreateFullPath(strTempPath);
	m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw_NoPDL_%s.csv", strTempPath, strTemp);
	m_stScanParameter.bDoPDL = FALSE;
	InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL

	if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
	{
		Sleep(2000);
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			strMsg = m_FSTPObj.GetErrorMsg();
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}
	int nPlotIndex = 0;
	for (int i = 1; i < m_stScanParameter.nPMCount; i++)
	{
		double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
		double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

		strTemp.Format("通道%d", i + 1);
		//m_pGraph->PlotClear(0,i);
		AddPlotCount(0, strTemp);
		m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
		nPlotIndex++;
	}

	return TRUE;
}

///////////////////////////////////////////////////////
//函数说明：
//   不带PDL扫描压力测试
//Add by wanxin
//Time:20200822
///////////////////////////////////////////////////////
BOOL CTestTaskManager::DoNoPDLStresstesting()
{
	CString strMsg;
	CString strTemp;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	CTime time1, time2;
	CTimeSpan  ts;
	BOOL bDoagain = FALSE;
	time1 = CTime::GetCurrentTime();
	if (!ReadScanConfig())
	{
		return FALSE;
	}
	int nIndex = 0;
	while (1)
	{
		if (g_taskstop)
		{
			break;
		}

		time2 = CTime::GetCurrentTime();
		ts = time2 - time1;
		int nTemp = ts.GetTotalSeconds();
		if (nTemp < m_nStreestestingTime &&bDoagain)
		{
			continue;
		}
		time1 = CTime::GetCurrentTime();
		bDoagain = TRUE;
		strTemp = time1.Format("%y%m%d%H%M%S");
		strTempPath.Format(".\\Data\\ScanTemp");
		CreateFullPath(strTempPath);
		m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw_NoPDL_%s.csv", strTempPath, strTemp);
		m_stScanParameter.bDoPDL = FALSE;
		strMsg.Format("FSTP快扫压力测试%d次扫描", nIndex++);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL

		CTime t1 = CTime::GetCurrentTime();
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}
		CTime t2 = CTime::GetCurrentTime();
		ts = t2 - t1;
		nTemp = ts.GetTotalSeconds();
		strMsg.Format("FSTP快扫压力测试%d次扫描时间:%d秒", nIndex++, nTemp);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);

		int nPlotIndex = 0;
		for (int i = 1; i < m_stScanParameter.nPMCount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

			strTemp.Format("通道%d", i);
			//m_pGraph->PlotClear(0,i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
		}

	}
	return TRUE;
}

/////////////////////////////////////////////////////////
//函数说明：
//   FSIL 客户端
//Add by wanxin
//Time:20200810
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::DoFSILClient(int nTaskIndex)
{
	switch (nTaskIndex)
	{
	case 101: //归零扫描
	{
		if (!DoNoPDLScanReference())
		{
			return FALSE;
		}
		break;
	}
	case 102: //单次扫描
	{
		if (!DoNoPDLScanTest())
		{
			return FALSE;
		}
		break;
	}
	case 103: //压力测试
	{
		if (!DoNoPDLStresstesting())
		{
			return FALSE;
		}
		break;
	}

	}

	return TRUE;
}



//////////////////////////////////////////////////////////
//函数说明：
//   做带PDL扫描归零
//Add by wanxin
//Time:20200822
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::DoWithPDLScanReference()
{
	CString strRefPath;
	CString strMsg;
	CString strTemp;
	CString	strFileName;
	BOOL  bFunctionOK = TRUE;
	int i = 0;
	try
	{
		if (!ReadScanConfig())
		{
			throw "从config中读取扫描配置文件存在错误";
		}
		strRefPath.Format(".\\Reference");
		CreateFullPath(strRefPath);
		m_bRefDone = FALSE;
		if (!m_bOpenTestDevice)
		{
			throw "请先初始化相应光学测试设备!";
		}
		m_stScanParameter.bDoPDL = TRUE;
		m_FSTPObj.m_strRefFileName.Format("%s\\DoPDL_Reference.csv", strRefPath);


		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
		InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

		strMsg = _T("是否调用现在的归零参数?\n" "是: 调用上次归零参数!\n" "否: 执行归零步骤!");
		DWORD dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
		if (XSB_YES == dwBoxRet)
		{
			if (!AdjustFileExit(m_FSTPObj.m_strRefFileName))
			{
				strMsg.Format("%s reference file not exsit", m_FSTPObj.GetErrorMsg());
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return	FALSE;
			}
			if (!m_FSTPObj.ReadReferenceDataFromFile())
			{
				strMsg.Format("%s", m_FSTPObj.GetErrorMsg());
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return	FALSE;
			}

			return TRUE;
		}

		strMsg = _T("请将光路系统In端接Out端");
		dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);


		strMsg.Format("开始进行归零扫描.......");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		if (!m_FSTPObj.DoReference(m_FSTPObj.m_nGUID))
		{
			strMsg = m_FSTPObj.GetErrorMsg();
			throw strMsg.GetBuffer(0);
		}
		int nPlotIndex = 0;
		for (int i = 0; i < m_stScanParameter.nPMCount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanRefResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanRefResult.pdbPMResult[i];
			double *pdbPDLArry = m_FSTPObj.m_stScanRefResult.pdbPDLResult[i];

			strTemp.Format("通道%d", i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;

			m_pGraph->PlotXYS(0, pdbWLArry, pdbPDLArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;

		}

		strMsg.Format("带PDL系统归零完成!");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		m_bRefDone = TRUE;
	}
	catch (TCHAR* pstrErrorMsg)
	{
		strMsg.Format("%s", pstrErrorMsg);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	return bFunctionOK;
}


/////////////////////////////////////////////////////////
//函数说明：
//	 不带PDL扫描函数
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////
BOOL CTestTaskManager::DoWithPDLScanTest()
{
	CString strMsg;
	CString strTemp;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	CTime time;
	if (!ReadScanConfig())
	{
		strMsg.Format("从配置文件中读取扫描配置存在错误!");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		return FALSE;
	}
	m_stScanParameter.bDoPDL = TRUE;

	time = ::GetCurrentTime();
	strTemp = time.Format("%y%m%d%H%M%S");
	strTempPath.Format(".\\Data\\ScanTemp");
	CreateFullPath(strTempPath);
	m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw_PDL_%s.csv", strTempPath, strTemp);

	InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
	InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

	if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
	{
		Sleep(2000);
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			strMsg = m_FSTPObj.GetErrorMsg();
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}
	int nPlotIndex = 0;
	for (int i = 0; i < m_nDGECount; i++)
	{
		double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
		double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

		strTemp.Format("DGE模块%d", i + 1);
		//m_pGraph->PlotClear(0,i);
		AddPlotCount(0, strTemp);
		m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
		nPlotIndex++;
		if (m_stScanParameter.bDoPDL)
		{
			AddPlotCount(1, strTemp);
			double *pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i];
			m_pGraph->PlotXYS(1, pdbWLArry, pdbPDLArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
		}
	}


	return TRUE;
}


///////////////////////////////////////////////////////
//函数说明：
//   不带PDL扫描压力测试
//Add by wanxin
//Time:20200822
///////////////////////////////////////////////////////
BOOL CTestTaskManager::DoWithPDLStresstesting()
{
	CString strMsg;
	CString strTemp;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	CTime time1, time2;
	CTimeSpan  ts;
	BOOL bDoagain = FALSE;
	time1 = ::GetCurrentTime();
	if (!ReadScanConfig())
	{
		return FALSE;
	}
	m_stScanParameter.bDoPDL = TRUE;
	int nIndex = 0;
	int nTemp = 0;
	while (1)
	{
		if (g_taskstop)
		{
			break;
		}
		time2 = CTime::GetCurrentTime();
		ts = time2 - time1;
		int nTemp = ts.GetTotalSeconds();
		if (nTemp < m_nStreestestingTime &&bDoagain)
		{
			continue;
		}
		time1 = CTime::GetCurrentTime();
		bDoagain = TRUE;
		strTemp = time1.Format("%Y%m%d%H%M%S");
		strTempPath.Format(".\\Data\\ScanTemp");
		CreateFullPath(strTempPath);
		m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw_PDL_%s.csv", strTempPath, strTemp);
		m_stScanParameter.bDoPDL = TRUE;
		strMsg.Format("PDL快扫压力测试%d次扫描", nIndex++);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
		InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//IL

		CTime t1 = CTime::GetCurrentTime();
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}
		CTime t2 = CTime::GetCurrentTime();
		ts = t2 - t1;
		nTemp = ts.GetTotalSeconds();
		strMsg.Format("PDL快扫压力测试%d次扫描时间:%d秒", nIndex++, nTemp);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);

		int nPlotIndex = 0;
		for (int i = 0; i < m_nDGECount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];
			double *pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

			strTemp.Format("DGE模块%d", i + 1);
			//m_pGraph->PlotClear(0,i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			AddPlotCount(1, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPDLArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
		}

	}
	return TRUE;
}


////////////////////////////////////////////////////////
//函数说明：
//   FSIL PDL 客户端
//Add by wanxin
//Time:20200810
//////////////////////////////////////////////////////////
BOOL CTestTaskManager::DoFSILPDLClient(int nTaskIndex)
{
	switch (nTaskIndex)
	{
	case 201: //归零扫描
	{
		if (!DoWithPDLScanReference())
		{
			return FALSE;
		}
		break;
	}
	case 202: //单次扫描
	{
		if (!DoWithPDLScanTest())
		{
			return FALSE;
		}
		break;
	}
	case 203: //压力测试
	{
		if (!DoWithPDLStresstesting())
		{
			return FALSE;
		}
		break;
	}

	}

	return TRUE;

}


///////////////////////////////////////////////////////////////
//函数说明：
//    根据xml配置文件中的任务ID进行各个任务测试
//Add by wanxin
//Time:20160225
/////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::RunTestTask(int iTaskIndex)
{
	//根据UI索引来设置循环箱温度
	CString strMsg;
	BOOL bFunctionOK = TRUE;
	BOOL bResult = TRUE;

	// 无确定SN号码Log信息存在PN目录下，不到SN目录
	//m_strLogFileName.Format("%s\\%s\\%s\\LogInfo.txt", m_strLoacalFloder,m_strPN,m_strSN);
	//////////////////////////////////////////////
	//bResult = DoSystemRef(iTaskIndex);
	//TemperatureIndex(iTaskIndex);
	//bResult = DownloadProductionInfo(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGECalibrationFor70C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGECalibrationForF5C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGECalibrationFor5C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGECalibrationFor15C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGECalibrationFor50C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGECalibrationFor30C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DownloadCalLut(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGEVerifyFor40C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGEVerifyFor70C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGEVerifyFor55C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGEVerifyForF5C(iTaskIndex);
	//bFunctionOK &= bResult;
	//bResult = DGEVerifyFor25C(iTaskIndex);
	//bFunctionOK &= bResult;
	bResult = DoFSILClient(iTaskIndex);
	bFunctionOK &= bResult;
	bResult = DoFSILPDLClient(iTaskIndex);
	bFunctionOK &= bResult;

	return bFunctionOK;
}
void CTestTaskManager::SetNetCommand(CNetCommand *pNetCommand)
{
	ASSERT(pNetCommand != NULL);
	if (pNetCommand == NULL)
	{
		return;
	}
	m_pNetCommand = pNetCommand;
}

void CTestTaskManager::SetTPInfoManager(CTPInfoManager *pInfoManger)
{
	ASSERT(pInfoManger != NULL);
	if (pInfoManger == NULL)
	{
		return;
	}
	m_pInfoManger = pInfoManger;
}

void CTestTaskManager::SetGraphPro(CGraphPro *pGraph)
{
	ASSERT(pGraph != NULL);
	if (pGraph == NULL)
	{
		return;
	}
	m_pGraph = pGraph;
}
////////////////////////////////////////////////////////////////////
//函数说明：
//    从系统配置ini文件中读取相应的配置信息
//Add by wanxin
//Time:20160225
///////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::InitialConfigSystemParam()
{
	CString strTemp;
	CString strMsg, strFileName;

	////////////// ini 配置文件的绝对路径////////////////
	ZeroMemory(m_tszAppFolder, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, m_tszAppFolder);
	LogInfo(m_tszAppFolder, MAX_PATH + 1);
	m_strSysCfgFilePath.Format(".\\Config\\SystemConfig.ini");	// INI 配置文件路径
	strTemp = LoadIniDataString(m_strSysCfgFilePath, "SystemConfig", "SERVER_PATH");
	if (strTemp == "ERROR")
	{
		strMsg.Format("从配置文件中读取是否网络存储配置参数错误");
		LogInfo(strMsg, strMsg.GetLength() + 1);
		return FALSE;
	}
	m_bServerOn = atoi(strTemp);
	if (m_bServerOn) //如果打开服务器配置，数据保存到服务器相应路径下
	{
		m_strServerFloder = LoadIniDataString(m_strSysCfgFilePath, "SystemConfig", "ServerPath");// 服务器文件路径
	}
	else
	{
		// 本地路径
		m_strLoacalFloder.Format("%s\\data", m_tszAppFolder);
	}

	////读取相应PN的配置文件
	//strTemp = LoadIniDataString(m_strSysCfgFilePath,"SystemConfig","CONFIG_PATH");
	//if(strTemp=="ERROR")
	//{
	//	strMsg.Format("从SystemConfig.ini配置文件中读取配置参数错误");
	//	LogInfo(strMsg,strMsg.GetLength()+1);
	//	return FALSE;
	//}
	//m_strConfigPath = strTemp;
	//m_strCfgFileName.Format("%s\\%s.ini",strTemp,m_pInfoManger->GetUISendPN());

	return TRUE;
}


BOOL CTestTaskManager::DownLoadTemplate()
{
	TCHAR tchrConfigPath[1024];
	TCHAR tchrDataPath[1024];
	TCHAR tchrLanguage[1024];
	TCHAR tchrLocation[1024];
	TCHAR tchrFactory[1024];
	TCHAR tStationName[1024];
	TCHAR tStationID[1024];
	ZeroMemory(tchrConfigPath, 1024 * sizeof(TCHAR));
	ZeroMemory(tchrDataPath, 1024 * sizeof(TCHAR));
	ZeroMemory(tchrLanguage, 1024 * sizeof(TCHAR));
	ZeroMemory(tchrLocation, 1024 * sizeof(TCHAR));
	ZeroMemory(tchrFactory, 1024 * sizeof(TCHAR));
	ZeroMemory(tStationName, 1024 * sizeof(TCHAR));
	ZeroMemory(tStationID, 1024 * sizeof(TCHAR));
	CString strErrorMsg;
	CString strProcessCode = m_pInfoManger->GetUISendProcess();
	////获取配置路径
	//if (!m_pNetCommand->XmlGetConfigPath(tchrDataPath, 1024, tchrConfigPath, 1024, tchrFWPath, 1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//strErrorMsg.Format("%s,%s,%s", tchrDataPath, tchrConfigPath, tchrFWPath);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);

	// 下模板WS7-EBOX-T定标
	if (!m_pNetCommand->XmlDownloadTemplate("ZHS24150001", "WS7-CP-T写信息","\\\\zh-mfs-srv.oplink.com.cn\\Passive\\EDFATemp\\WWW\\FinalTest_1.xml", 0))
	{
		strErrorMsg = m_pNetCommand->GetLastError();
		LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	}


	//if (!m_pNetCommand->XmlGetStationInfo("ITNB150007", tStationID, tStationName,1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}

	//if (!m_pNetCommand->XmlCreateSNDir("E2878562","C:\\", tchrDataPath,1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}

	//if (!m_pNetCommand->XmlGetFWStandardPath("E2878562", tchrDataPath, tchrFWPath, 1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//if (!m_pNetCommand->XmlTriggerMIMSIDLoginVerify("1", "1"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//if (!m_pNetCommand->XmlTriggerWorkStationVerify("1", "1","1"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//if (!m_pNetCommand->XmlTriggerWorkStationVerify("1", "1", "1"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//if (!m_pNetCommand->XmlGetOplinkUserInfo("1", tchrConfigPath, tchrDataPath, tchrFWPath, tchrSubSNList,1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//if (!m_pNetCommand->XmlTriggerUserSkillVerify("1","1"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}

	//strErrorMsg.Format("Statin ID: %s" , tStationID);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);

	//strErrorMsg.Format("Statin Name: %s", tStationName);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);


	//if (!m_pNetCommand->XmlDownloadTemplate("E2437670", "LLCC-LLCC-终测点测", "\\\\zh-mfs-srv.oplink.com.cn\\Passive\\EDFATemp\\WWX\\FinalTest__.xml", 0))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}



	 //下工单表头,Position 当需要同时获取多个器件是，采用逗号分隔，如 SW1,SW2,SW3  ,回复的SN号也将用逗号分隔 SN1,SN2,SN3
	//if (!m_pNetCommand->XmlDownloadTaskList("15153-02","MEMS-SWITCH-组装15153", "MEMSSWITCH-1-SN,MEMSSWITCH-2-SN,MEMSSWITCH-3-SN,MEMSSWITCH-4-SN", tchrSubSNList, 1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//}
	//strErrorMsg.Format("%s", tchrSubSNList);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);

	//if (!m_pNetCommand->XmlDownloadTaskList(strSN, "MEMS-SWITCH-组装15153", "MEMSSWITCH-1-SN,MEMSSWITCH-2-SN,MEMSSWITCH-3-SN,MEMSSWITCH-4-SN", tchrSubSNList, 1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	strErrorMsg = strMsg;
	//	m_pNetCommand->XmlSendLog(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
	//	// return FALSE;
	//}

	//上传数据
	//if (!m_pNetCommand->XmlUploadTestData("20210120F35", "C:\\Users\\wwang89\\OneDrive - kochind.com\\WorkShop\\Project\\Platfrom\\MIMS System\\MIMS APP\\FSTP_TPClient\\bin\\data\\20210120F35_SFPVOA_定标.xml"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//}
	//TCHAR tchrTPN[128];
	//ZeroMemory(tchrTPN, 128 * sizeof(TCHAR));
	//if (!m_pNetCommand->XmlTriggerConversationPN("1835070008", tchrTPN, 128, CONV_PN_MOLEX_OPLINK))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//}
	//strErrorMsg.Format("%s", tchrTPN);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//转换工序
	//CString strProdPN = "T1234455";
	//CString strEmsProcess = "MES-TF-终测";
	//TCHAR tchrTargetPN[128];
	////ZeroMemory(tchrTPN, 128 * sizeof(TCHAR));
	//if (!m_pNetCommand->XmlTriggerConverProcess(strProdPN, strEmsProcess, tchrTargetPN, 128, CONV_PROCESS_MES_TEST))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}

	//获取工位信息
	//CString strPCName = "ITPC10000";
	//TCHAR tchrStationID[256];
	//ZeroMemory(tchrStationID, 256 * sizeof(TCHAR));
	//TCHAR tchrStationName[256];
	//ZeroMemory(tchrStationName, 256 * sizeof(TCHAR));
	//if (!m_pNetCommand->XmlGetStationInfo(strPCName, tchrStationID, tchrStationName, 256))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//strErrorMsg.Format("Station ID:%s,StationName:%s", tchrStationID, tchrStationName);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//CString strProdSN = "15153001";
	//if (!m_pNetCommand->XmlTriggerMoveIn(strProdSN))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//strErrorMsg.Format("Trigger Move In-%s", strProdSN);
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);


	//if (!m_pNetCommand->XmlTriggerTestResultUpload2OPC("14580001"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//}

	//strErrorMsg.Format("Result -%s", "14580001");
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);


	//if (!m_pNetCommand->XmlTriggerMoveOut("1223456"))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//strErrorMsg.Format("Trigger Move Out-%s", "123456");
	//LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);

	if (!m_pNetCommand->XmlLocation(tchrLanguage, tchrLocation, tchrFactory,1024))
	{
		strErrorMsg = m_pNetCommand->GetLastError();
		LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	}
	strErrorMsg.Format("Language:%s", tchrLanguage);
	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1,LOG_NORMAL);
	strErrorMsg.Format("Location:%s", tchrLocation);
	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_NORMAL);
	strErrorMsg.Format("Factory:%s", tchrFactory);
	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_NORMAL);


	//TCHAR *tchrUserID[10] = {NULL};
	//TCHAR *tchrOutMsg[10] = {NULL};
	//for (int i = 0; i < 10; i++)
	//{
	//	tchrUserID[i] = (TCHAR*) VirtualAlloc(NULL, sizeof(TCHAR) * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//	ZeroMemory(tchrUserID[i], 1024 * sizeof(TCHAR));
	//	tchrOutMsg[i] = (TCHAR*)VirtualAlloc(NULL, sizeof(TCHAR) * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//	ZeroMemory(tchrOutMsg[i], 1024 * sizeof(TCHAR));
	//}
	//strcpy(tchrUserID[0], "E6215981");
	//strcpy(tchrUserID[1], "1834020583(15673)盒体标签");
	//strcpy(tchrUserID[2], "PN");

	//if (!m_pNetCommand->XmlGenericFunc("GetPrintLabelInfo",3,tchrUserID,2, tchrOutMsg, 1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//CString strTemp = tchrOutMsg[1];
	//if (strTemp.IsEmpty())
	//{
	//	strErrorMsg.Format("return:%s", tchrOutMsg[0]);
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_NORMAL);
	//}
	//else
	//{
	//	strErrorMsg.Format("error:%s", tchrOutMsg[1]);
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_ERROR);
	//}
	//for (int i = 0; i < 10; i++)
	//{
	//	ZeroMemory(tchrUserID[i], 1024 * sizeof(TCHAR));
	//	ZeroMemory(tchrOutMsg[i], 1024 * sizeof(TCHAR));
	//}
	//strcpy(tchrUserID[0], "E6215981");
	//strcpy(tchrUserID[1], "1834020583(15673)盒体标签");
	//strcpy(tchrUserID[2], "PN,SN");

	//if (!m_pNetCommand->XmlGenericFunc("GetPrintLabelInfo", 3, tchrUserID, 2, tchrOutMsg, 1024))
	//{
	//	strErrorMsg = m_pNetCommand->GetLastError();
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	//}
	//strTemp = tchrOutMsg[1];
	//if (strTemp.IsEmpty())
	//{
	//	strErrorMsg.Format("return:%s", tchrOutMsg[0]);
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_NORMAL);
	//}
	//else
	//{
	//	strErrorMsg.Format("error:%s", tchrOutMsg[1]);
	//	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_ERROR);
	//}

	//for (int i = 0; i < 10; i++)
	//{
	//	if (tchrUserID[i] != NULL)
	//	{
	//		VirtualFree(tchrUserID[i], 0, MEM_RELEASE);
	//	}
	//	ZeroMemory(tchrOutMsg[i], 1024 * sizeof(TCHAR));
	//}

	double dbIL1, dbIL2;
	if (!m_pNetCommand->XmlGetRoutingTheoryIL("MA6523330085", "AF74-62","1550",&dbIL1,&dbIL2))
	{
		strErrorMsg = m_pNetCommand->GetLastError();
		LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1);
	}
	strErrorMsg.Format("return:%.2f,%.2f", dbIL1, dbIL2);
	LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_NORMAL);

	return TRUE;
}



UINT CTestTaskManager::DoShowLogThread(LPVOID lpParam)
{
	CString strMsg;
	CTestTaskManager* p = (CTestTaskManager*)lpParam;
	int nSendIndex = 0;
	while (TRUE)
	{
		g_Loglock.Lock();
		strMsg.Format("Thread线程中Log日志压力测试：%d次", nSendIndex);
		p->LogInfo(strMsg, strMsg.GetLength() + 1);
		nSendIndex++;
		g_Loglock.Unlock();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////
//函数说明：
//    执行测试的初始化函数
//Add by wanxin
///////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::RunTestInit()
{
	CString strMsg, strErrMsg, m_strUIProcessType, strAppFolder, strTemp;
	CString strPortPath;
	CString strDataPath;
	WORD wCmdCount = 0;
	//////////////////////////////  区分SN号，根据逗号分隔///////////////////////		
	TCHAR		seps[] = ";";
	TCHAR		*token = NULL;

	//int  nT = sizeof(double);
	CString strTempSn = m_pInfoManger->GetUISendSN();
	int nSnCount = strTempSn.Replace(";", ";");
	if (nSnCount != 3 && nSnCount != 7)
	{
		strMsg.Format("输入的SN号个数为%d 为兼容的测试!", nSnCount + 1);
		LogInfo(strMsg, strMsg.GetLength() + 1);
	}
	m_strSNArray.RemoveAll();
	m_strPN = m_pInfoManger->GetUISendPN();


	strMsg.Format("Log日志测试1\r\n,2\r\rn,3\r\n");
	LogInfo(strMsg, strMsg.GetLength() + 1);


	CString     tempstr;
	LPTSTR lpSNStr = strTempSn.GetBuffer(0);
	for (int i = 0; i < nSnCount + 1; i++)
	{
		if (i == 0)
		{
			token = strtok(lpSNStr, seps);
			tempstr = token;
		}
		else
		{
			token = strtok(NULL, seps);
			tempstr = token;
		}
		m_strSNArray.Add(tempstr);
		//m_DGEModule[i].m_DGEInfo.strSN = tempstr;
		//m_DGEModule[i].m_DGEInfo.strPN = m_strPN;
		//m_DGEModule[i].m_nModuleIndex = i + 1;


		//m_DGEModule[i].m_strLogFileName.Format("%s\\%s\\%s\\LogInfo.txt", m_strLoacalFloder, m_strPN, tempstr);
	}
	m_strSN = m_strSNArray[0];
	//nGetSNCount = i;
	//int nSendIndex = 0;
	//AfxBeginThread((AFX_THREADPROC)DoShowLogThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

	//return TRUE;
	//while (TRUE)
	//{
	//	strMsg.Format("主线程中：Log日志压力测试：%d次", nSendIndex);
	//	LogInfo(strMsg, strMsg.GetLength() + 1);
	//	nSendIndex++;
	//}

	strMsg = _T("是否开始进行增加消息的测试?\n");
	DWORD dwBoxRetx = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (dwBoxRetx == XSB_YES)
	{
		DownLoadTemplate();
	}


	//从系统配置文件中读取相应配置信息
	if (!InitialConfigSystemParam())
	{
		return FALSE;
	}

	for (int i = 0; i < nGetSNCount; i++)
	{
		if (m_bServerOn)
		{
			strDataPath.Format("%s\\%s\\%s", m_strServerFloder, m_strPN, m_strSNArray.GetAt(i));
		}
		else
		{
			strDataPath.Format("%s\\%s\\%s", m_strLoacalFloder, m_strPN, m_strSNArray.GetAt(i));
		}
	}

	strMsg = _T("是否需要进行光学测试?\n" "是: 连接光学测试相关设备!\n" "否: 不连接光学设备!");
	DWORD dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);

	if (XSB_YES == dwBoxRet)
	{
		/// 连接光学测试设备
		if (!OpenTestOpticDevice())
		{
			return FALSE;
		}

		///// 进行光学归零//////
		//if (!SystemReference())
		//{
		//	return FALSE;
		//}
	}
	else
	{
		m_bOpenTestDevice = FALSE;
	}


	strMsg = _T("是否开始进行光学扫描？\n是: 开始测试!\n 否: 取消测试!");
	dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (XSB_NO == dwBoxRet)
	{
		return FALSE;
	}

	return	TRUE;
}
///////////////////////////////////////////////////////
//函数说明：
//   从配置文件中读取扫描配置
//Add by wanxin
//Time:20200615
//////////////////////////////////////////////////////
BOOL CTestTaskManager::ReadScanConfig()
{
	CString strSysConfigPath, strMsg;
	CString strKeyName;


	strSysConfigPath.Format(".\\Config\\ScanConfig.ini");
	//LogInfo(strSysConfigPath, strSysConfigPath.GetLength() + 1, LOG_NORMAL);

	strKeyName.Format("DOPDL");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_stScanParameter.bDoPDL = atoi(strMsg);

	strKeyName.Format("START_WL");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_stScanParameter.dblStartWL = atof(strMsg);

	strKeyName.Format("STOP_WL");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_stScanParameter.dblStopWL = atof(strMsg);

	if (m_stScanParameter.dblStopWL < m_stScanParameter.dblStartWL)
	{
		strMsg.Format("扫描终止波长小于开始波长,扫描配置存在问题");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	strKeyName.Format("WL_STEP");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_stScanParameter.dblStepSize = atof(strMsg);

	if (m_stScanParameter.dblStepSize > 5.0)
	{
		strMsg.Format("扫描步长设置5nm,不符合设置标准,扫描配置存在问题");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	strKeyName.Format("PMCHCOUNT");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_stScanParameter.nPMCount = atoi(strMsg);

	strKeyName.Format("GUID");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_FSTPObj.m_nGUID = atoi(strMsg);

	m_stScanParameter.dwSampleCount = (DWORD)((m_stScanParameter.dblStopWL - m_stScanParameter.dblStartWL) / m_stScanParameter.dblStepSize) + 1;


	strKeyName.Format("FSTPCfg");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_strFSTPCfg = strMsg;

	//LogInfo(m_strFSTPCfg, m_strFSTPCfg.GetLength() + 1, LOG_NORMAL);

	strKeyName.Format("StresstestingTime");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_nStreestestingTime = atoi(strMsg);


	strKeyName.Format("CirculatorFlag");
	strMsg = LoadIniDataString(strSysConfigPath, "FSTPSCAN_CONFIG", strKeyName);
	m_stScanParameter.bCirculator = atoi(strMsg);






	return TRUE;
}




/////////////////////////////////////////////////////////
//函数说明：
//    连接光学测试设备,这里采用UDL控件
//Add by wanxin
//Time:20150225
//////////////////////////////////////////////////////////
BOOL CTestTaskManager::OpenTestOpticDevice()
{
	CString  strConfigFile, m_strUIProcessType;
	CString  strMsg;
	char szBuffer[MAX_PATH];
	ZeroMemory(szBuffer, MAX_PATH);
	m_bOpenTestDevice = FALSE;
	if (!ReadScanConfig())
	{
		return FALSE;
	}
	::CoInitialize(NULL);
	//ReleaseUDL();z
	if (!m_FSTPObj.OpenFSTPDevice(m_strFSTPCfg))
	{
		strMsg = m_FSTPObj.GetErrorMsg();
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	//从文件中读取扫描的配置
	if (!ReadScanConfig())
	{
		return FALSE;
	}
	if (!m_FSTPObj.PrepareScan(m_FSTPObj.m_nGUID, m_stScanParameter))
	{
		strMsg = m_FSTPObj.GetErrorMsg();
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	m_bOpenTestDevice = TRUE;

	return TRUE;
}
/////////////////////////////////////////////////////////
//函数说明：
//    按照输入的路径创建相应的文件夹
//Add by wanxin
////////////////////////////////////////////////////////
void CTestTaskManager::CreateFullPath(CString strPath)
{
	if (-1 == GetFileAttributes(strPath))
	{
		int nPos = strPath.ReverseFind('\\');
		CreateFullPath(strPath.Left(nPos));
		CreateDirectory(strPath, NULL);
	}
	else
	{
		return;
	}
}

//////////////////////////////////////////////////////////////
//函数说明：
//    创建本地相应测试的文件夹
//Add by wanxin
//Time:20160225
//////////////////////////////////////////////////////////////
BOOL CTestTaskManager::CreateProductionDataDirectory(int nModuleIndex, LPCSTR lpstrRootFolder, LPCSTR lpstrPN, LPCSTR lpstrSN)
{
	CString		strTempSN, strTmptValue;
	CString		strPNDir, strSNDir, strFileDir;

	////////// 创建PN 目录 /////////////////////// 
	strSNDir.Format("%s\\%s\\%s", lpstrRootFolder, lpstrPN, lpstrSN);
	if (!FindFileExist(strSNDir))
	{
		CreateFullPath(strSNDir);
	}
	m_DGEModule[nModuleIndex].m_strCaliFolder = strSNDir;
	m_DGEModule[nModuleIndex].m_strTestPath = strSNDir;
	m_DGEModule[nModuleIndex].m_strLogFileName.Format("%s\\%s_log.txt", strSNDir, lpstrSN);
	return TRUE;
}
//////////////////////////////////////////////////////////////
//函数说明：
//    创建本地相应测试的文件夹
//Add by wanxin
//Time:20160225
//////////////////////////////////////////////////////////////
BOOL CTestTaskManager::CreatSeverDirectory()
{
	CString		strTempSN, strTmptValue;
	CString		strPNDir, strSNDir, strFileDir;
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;

	////////// 创建PN 目录 /////////////////////// 
	strPNDir.Format("%s\\%s", m_strServerFloder, m_pInfoManger->GetUISendPN());
	if (!FindFileExist(strPNDir))
	{
		CreateFullPath(strPNDir);
	}
	////////// SN 目录 /////////////////////// 
	int nSnCount = m_strSNArray.GetSize();
	for (int i = 0; i < nSnCount; i++)
	{
		strTempSN = m_strSNArray.GetAt(i);
		strSNDir.Format("%s\\%s", strPNDir, strTempSN);
		m_strReadSNArray.Add(strTempSN);///
		hDirectory = FindFirstFile(strSNDir, &winFindFileData);
		if (hDirectory == INVALID_HANDLE_VALUE)
		{
			if (!CreateDirectory(strSNDir, NULL))
			{
				strTmptValue.Format("创建%s 失败 ", strSNDir);
				LogInfo(strTmptValue, strTmptValue.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}
		else
		{
			FindClose(hDirectory);
		}
	}
	return TRUE;
}

CString CTestTaskManager::LoadIniDataString(CString strFilePath, LPCTSTR strSection, LPCTSTR szID)
{
	CString szValue;
	//	DWORD dwSize = 1000;
	char szBuffer[MAX_PATH];

	GetPrivateProfileString(strSection, szID, "ERROR", szBuffer, MAX_PATH, strFilePath);
	//	szValue.ReleaseBuffer();

	szValue = szBuffer;

	//	szValue.Replace("\\n","\n");	//替换回换行符号
	return szValue;
}

/////////////////////////////////////////////////////////////
//函数说明:
//    读取DGE模块串口地址
//Add by wanxin
//Time:20200518
//////////////////////////////////////////////////////////////
BOOL CTestTaskManager::OpenSerialPortDevice()
{
	CString strSysConfigPath, strMsg;
	CString strKeyName, strCOMPort;
	CString strTemp;
	int nTempDutCount = -1;

	strSysConfigPath.Format(".\\Config\\DeviceConfig.ini");

	strKeyName.Format("Dut_Count");
	strMsg = LoadIniDataString(strSysConfigPath, "Dut_Config", strKeyName);
	nTempDutCount = atoi(strMsg);

	//if (nTempDutCount != m_nDGECount)
	//{
	//	strMsg.Format("输入的DGE SN个数与配置文件中DGE串口个数不一致");
	//	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
	//	return FALSE;
	//}
	//连接产品
	for (int i = 0; i < m_nDGECount; i++)
	{
		strKeyName.Format("Dut_%d_Address", i + 1);
		strCOMPort = LoadIniDataString(strSysConfigPath, "Dut_Config", strKeyName);
		m_DGEModule[i].m_strComPort = strCOMPort;

		strKeyName.Format("Dut_%d_AnologIndex", i + 1);
		strTemp = LoadIniDataString(strSysConfigPath, "Dut_Config", strKeyName);
		m_DGEModule[i].m_nAnologDGEIndex = atoi(strTemp);

		if (!ConnectDut(i, strCOMPort))
			return FALSE;
		m_DGEModule[i].m_Module.SetDGEIndex(m_DGEModule[i].m_nAnologDGEIndex);
	}

	return TRUE;
}

BOOL CTestTaskManager::FindFileExist(LPCTSTR lpstrFileFullName)
{
	CString       strErrMsg;

	//	int           nSelection=0;

	HANDLE		  hResultFile = INVALID_HANDLE_VALUE;

	WIN32_FIND_DATA	winFindFileData;

	hResultFile = FindFirstFile(lpstrFileFullName, &winFindFileData);

	// find the exist file
	if (hResultFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hResultFile);
		return TRUE;
	}
	// clost the handle
	FindClose(hResultFile);
	// find not the exist file
	return FALSE;
}


//// 将文件上传到无纸化系统


BOOL CTestTaskManager::SetFileByMSMQ(CString strFile, CString &strErr)
{
	CStdioFile file(strFile, CFile::modeRead | CFile::typeText);
	CString strRead, strSend;
	strSend.Empty();
	do
	{
		file.ReadString(strRead);
		DWORD	dwCodeLen = WideCharToMultiByte(CP_UTF8, NULL, strRead.AllocSysString(), -1, NULL, NULL, 0, FALSE);
		TCHAR	*ptStr = new TCHAR[dwCodeLen];
		WideCharToMultiByte(CP_UTF8, NULL, strRead.AllocSysString(), -1, ptStr, dwCodeLen, NULL, FALSE);
		strSend += ptStr;
		delete[] ptStr;
	} while (!strRead.IsEmpty());

	if (!UploadXMLMessage((LPSTR)(LPCTSTR)strSend, (LPSTR)(LPCTSTR)strRead))
	{
		//		AfxMessageBox("发送数据失败，请确认网络和服务器工作是否正常");
		return FALSE;
	}
	else
		return TRUE;
}

void CTestTaskManager::RecordStartTestTime()
{
	int nYear, nMonth, nDay, nHour, nMun, nSec;

	if (!m_bStartTestTime)
	{
		CTime time = CTime::GetCurrentTime();
		nYear = time.GetYear();
		nMonth = time.GetMonth();
		nDay = time.GetDay();
		nHour = time.GetHour();
		nMun = time.GetMinute();
		nSec = time.GetSecond();
		m_strStartTestTime.Format("%d-%02d-%02d %02d:%02d:%02d", nYear, nMonth, nDay, nHour, nMun, nSec);
		m_bStartTestTime = TRUE;
	}
}

void CTestTaskManager::RecordStopTestTime()
{
	int nYear, nMonth, nDay, nHour, nMun, nSec;
	CTime time = CTime::GetCurrentTime();
	nYear = time.GetYear();
	nMonth = time.GetMonth();
	nDay = time.GetDay();
	nHour = time.GetHour();
	nMun = time.GetMinute();
	nSec = time.GetSecond();
	m_strStopTestTime.Format("%d-%02d-%02d %02d:%02d:%02d", nYear, nMonth, nDay, nHour, nMun, nSec);

}


//打开光源串口
BOOL CTestTaskManager::OpenASEComPort(CString strComPort)
{
	CString strTempCom;
	CString strMsg;
	if (strComPort.GetLength() < 4)
	{
		return FALSE;
	}
	if (strComPort.GetLength() > 4)
	{
		strTempCom.Format("\\\\.\\%s", strComPort);
	}
	else
	{
		strTempCom = strComPort;
	}


	//m_bOpenSwitch = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////
//函数说明：
//   连接DUT
//Add by wanxin
//Time:20200520
///////////////////////////////////////////////////
BOOL CTestTaskManager::ConnectDut(int n_Com, CString strComPort)
{
	CString strTempCom;
	CString strMsg;
	CString strSNTemp;

	if (strComPort.GetLength() < 4)
	{
		strMsg.Format("串口号%s存在错误", strComPort);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	if (strComPort.GetLength() > 4)
	{
		strTempCom.Format("\\\\.\\%s", strComPort);
	}
	else
	{
		strTempCom = strComPort;
	}

	if (m_DGEModule[n_Com].m_nAnologDGEIndex == 0)
	{
		if (!m_DGEModule[n_Com].m_Module.OpenPort(strTempCom.GetBuffer(0)))
		{
			strMsg.Format("打开DGE模块%d串口%s存在错误", n_Com + 1, strTempCom);
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}
	else if (m_DGEModule[n_Com].m_nAnologDGEIndex == 1)
	{

		if (m_DGEModule[n_Com - 1].m_bConnect)
		{
			m_DGEModule[n_Com].m_Module = m_DGEModule[n_Com - 1].m_Module;
		}
		else
		{
			if (!m_DGEModule[n_Com].m_Module.OpenPort(strTempCom.GetBuffer(0)))
			{
				strMsg.Format("打开DGE模块%d串口%s存在错误", n_Com + 1, strTempCom);
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}

	}

	m_DGEModule[n_Com].m_bConnect = TRUE;

	strMsg.Format("打开DGE模块%d串口%s成功", n_Com + 1, strTempCom);
	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
	double dbTemp;
	m_DGEModule[n_Com].m_Module.GetTemper(strMsg, dbTemp);


	////从模块中读取SN号与输入SN进行比较
	//if (!m_DGEModule[n_Com].GetModuleSerialNO(strSNTemp))
	//{
	//	strMsg.Format("读取DGE模块%dSN号存在错误", n_Com + 1);
	//	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
	//	return FALSE;
	//}
	//if (m_DGEModule[n_Com].m_DGEInfo.strSN.CompareNoCase(strSNTemp) != 0)
	//{
	//	strMsg.Format("读取DGE模块%dSN号与输入的SN不一致,读取SN:%s,输入SN:%s", n_Com + 1, m_DGEModule[n_Com].m_DGEInfo.strSN, strSNTemp);
	//	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
	//	return FALSE;
	//}
	return TRUE;
}


BOOL CTestTaskManager::ConnectChamber()
{
	CString strKeyName;
	CString strTemp;
	CString strCOMPort;
	CString strSysConfigPath;

	int nChamberType;
	int nChamberComType;

	m_bChamberOn = FALSE;

	strSysConfigPath.Format(".\\Config\\DeviceConfig.ini");

	strKeyName.Format("ChamberOnline");
	strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
	m_nChamberOnline = atoi(strTemp);

	if (m_nChamberOnline == 1)
	{
		strKeyName.Format("ChamberType");
		strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
		nChamberType = atoi(strTemp);

		strKeyName.Format("ChamberComType");
		strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
		nChamberComType = atoi(strTemp);

		strKeyName.Format("ChamberAddress");
		strCOMPort = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
		WORD wAddress = atoi(strCOMPort);

		if (!m_cChamber.OpenChamber(nChamberType, wAddress))
		{
			strTemp.Format("打开循环箱%d串口存在错误", wAddress);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
		//读取循环箱当前温度
		double dbCurrentTmp;
		if (!m_cChamber.GetZHChamberCurrentTemp(dbCurrentTmp))
		{
			strTemp.Format("读取循环箱当前温度存在错误", wAddress);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
		strTemp.Format("读取循环箱当前温度：%.1fC", dbCurrentTmp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
		m_bChamberOn = TRUE;
	}


	return TRUE;
}



//////////////////////////////////////////////////////////
//函数说明：
//    根据输入Port 索引切换光开关到指定Port
//Add by wanxin
//Time：20200623
////////////////////////////////////////////////////////////
BOOL CTestTaskManager::SwitchBoxByPort(int nPort, int nMoudleIndex)
{
	CString strErrorMsg;
	CString strTemp;
	try
	{
		//1为H，0为L
		if (!m_switchctrl.SetZH3HL(0))
		{
			strTemp.Format("驱动MT第三代开关盒到低光出口存在错误");
			throw strTemp.GetBuffer(0);
		}

		if (!m_switchctrl.SetTester(nMoudleIndex, nPort))
		{
			if (!m_switchctrl.SetTester(nMoudleIndex, nPort))
			{
				throw "驱动珠海第三代开关错误!";

				strTemp.Format("驱动MT第三代开关盒%d模块到%dPort存在错误", nMoudleIndex, nPort);
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *pstrMsg)
	{
		strErrorMsg = pstrMsg;;
		LogInfo(strErrorMsg, strErrorMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}


////////////////////////////////////////////////////////
//函数说明：
//    连接WSS产线第三代MT开关盒
//Add by wanxin
//Time:20200623
////////////////////////////////////////////////////////
BOOL CTestTaskManager::ConnectSwitchBox()
{
	CString strKeyName;
	CString strTemp;
	CString strCOMPort;
	CString strSysConfigPath;

	int nSwitchBoxType;
	int nSwitchBoxOpticalType;
	int nSwitchBoxComType;

	m_bChamberOn = FALSE;

	strSysConfigPath.Format(".\\Config\\DeviceConfig.ini");

	strKeyName.Format("SwitchBoxOnline");
	strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
	m_nSwitchBoxOnline = atoi(strTemp);

	if (m_nSwitchBoxOnline == 1)
	{
		strKeyName.Format("SwitchBoxType");
		strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
		nSwitchBoxType = atoi(strTemp);

		strKeyName.Format("SwitchBoxOpticalType");
		strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
		nSwitchBoxOpticalType = atoi(strTemp);


		strKeyName.Format("SwitchComType");
		strTemp = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);
		nSwitchBoxComType = atoi(strTemp);

		strKeyName.Format("SwitchAddress");
		strCOMPort = LoadIniDataString(strSysConfigPath, "DeviceConfig", strKeyName);

		//赋值开关类型
		m_switchctrl.nUseSwitchType = nSwitchBoxType;
		//默认第三代开关是1XN还是NX1
		m_switchctrl.m_nmodule = nSwitchBoxOpticalType;

		WORD wAddress = atoi(strCOMPort);
		//赋值WSS串口
		if (wAddress < 10 && wAddress > 0)
		{
			strCOMPort.Format("COM%d", wAddress);
		}
		else if (wAddress >= 10)
		{
			strCOMPort.Format("\\\\.\\COM%d", wAddress);
		}
		//测试系统2个1x8开关
		if (!m_switchctrl.OpenPort(strCOMPort.GetBuffer(1024), 115200))
		{
			strTemp.Format("打开系统光路开关盒存在错误");
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
		else
		{
			if (!SwitchBoxByPort(1, 1))
			{
				return FALSE;
			}
		}
		m_bSwithBoxOn = TRUE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////
//函数说明：
//   连接本地设备
//Add by wanxin
//Time:20200520
//////////////////////////////////////////////////////////////
BOOL CTestTaskManager::ConnectLocalEquipment()
{
	if (!ConnectChamber())
	{
		return FALSE;
	}


	if (!ConnectSwitchBox())
	{
		return FALSE;
	}

	return TRUE;
}


double CTestTaskManager::Sum(double *pdblArray, int nCount)
{

	double dblSum = 0;
	for (int i = 0; i < nCount; i++)
	{
		dblSum += pdblArray[i];
	}

	return dblSum;

}
BOOL CTestTaskManager::LinearFitArray(double *pdblX, double *pdblY, int nCount, double &k, double &c)
{
	double *pdblTempArray1 = new double[nCount];
	double	*pdblTempArray2 = new double[nCount];


	for (int i = 0; i < nCount; i++)
	{
		pdblTempArray1[i] = pdblX[i] * pdblY[i];
		pdblTempArray2[i] = pdblX[i] * pdblX[i];
	}

	k = (nCount * Sum(pdblTempArray1, nCount) - Sum(pdblX, nCount) * Sum(pdblY, nCount))
		/ (nCount * Sum(pdblTempArray2, nCount) - Sum(pdblX, nCount) * Sum(pdblX, nCount));
	c = Sum(pdblY, nCount) / nCount - k * Sum(pdblX, nCount) / nCount;


	if (pdblTempArray1 != NULL)
	{
		delete[] pdblTempArray1;
		pdblTempArray1 = NULL;
	}
	if (pdblTempArray2 != NULL)
	{
		delete[] pdblTempArray2;
		pdblTempArray2 = NULL;
	}
	return TRUE;
}

BOOL CTestTaskManager::GetIniFileString(CString strSelection, CString strKeyWork, CString strFileName, CString* pstRspMsg)
{
	CString strTemp, strMsg;
	BOOL bFunctionOK = FALSE;
	char charTemp[MAX_PATH] = { 0 };

	//Get start FW_PATH				
	GetPrivateProfileString(strSelection, strKeyWork, "ERROR", charTemp, 255, strFileName);
	strTemp = charTemp;
	if (strTemp == "ERROR")
	{
		pstRspMsg->Format("从文件: %s 获取[%s]->[%s]参数失败", strFileName, strSelection, strKeyWork);
		return	FALSE;
	}

	pstRspMsg->Format("%s", charTemp);

	return	TRUE;
}



BOOL CTestTaskManager::RefreshModuleTemperature()
{
	double dbReadTemp = -100;
	CString ErrMsg;
	double dbCurrentChamberTemp = -30;

	if (dbReadTemp < -30)
	{
		return FALSE;
	}

	//读取当前炉子的温度
	if (!m_cChamber.fnModBusReadCurrentPoint(dbCurrentChamberTemp))
	{
		if (!m_cChamber.fnModBusReadCurrentPoint(dbCurrentChamberTemp))
		{
			return FALSE;
		}
	}

	//读取当前炉子设定温度
	if (!m_cChamber.fnModBusReadSetPoint(dbCurrentChamberTemp))
	{
		if (!m_cChamber.fnModBusReadSetPoint(dbCurrentChamberTemp))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CTestTaskManager::ControlChamberQuickly(stSettingTemp stChamberSetTemp)
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	CString strTemper;
	double dbAVTemp[MAX_PATH];
	int    nIntervalCount = 0;
	int iIndex = 0;
	int iTemperIndex = 0;
	BOOL bSetBack = FALSE;
	BOOL bSetOverShowPoint;
	CString strDacFile, ErrMsg;
	int nInterval = 0;
	double dblCurrentChamberTemp = 0.0;
	double dblCurrentTemp = 0.0;
	double dblChamberPrepSetTemp = 0.0;
	double dblTempTotal = 0.0;
	CTime m_StartTime, m_EndTime;
	time_t time1, time2;
	double dbTargetTemp;
	//	BOOL g_bStop,g_bSkip;

	for (int i = 0; i < MAX_PATH; i++)
	{
		dbAVTemp[i] = 100.0;
	}
	//	g_bStop = FALSE;
	//	g_bSkip = FALSE;

	//SFP理论上的Target 温度
	dbTargetTemp = stChamberSetTemp.dbSetTemp + stChamberSetTemp.dbDiffer;
	strTemp.Format("SFP VOA Target Temp : %.2fC", dbTargetTemp);
	LogInfo(strTemp, strTemp.GetLength() + 1);
	//	LogInfo(strTemp, COLOR_BLUE);

	//获得当前炉子的温度
	if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
	{
		m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp);
	}
	strTemp.Format("Current Chamber Temp : %.2fC", dblCurrentChamberTemp);
	LogInfo(strTemp, strTemp.GetLength() + 1);
	//	LogInfo(strTemp, COLOR_BLUE);

	//判断当前炉子温度与预设炉子温度的差异，
	//如果炉子预设温度比当前炉子温度大于5度
	if (stChamberSetTemp.dbSetTemp - dblCurrentChamberTemp > 5)
	{
		//直接将炉子温度设置到最大温度
		dblChamberPrepSetTemp = stChamberSetTemp.dbSetTemp + 30;

		if (dblChamberPrepSetTemp > stChamberSetTemp.dbMaxTemp)
		{
			dblChamberPrepSetTemp = stChamberSetTemp.dbMaxTemp;
		}
		if (!m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp))
		{
			m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp);
		}
	}
	else if (stChamberSetTemp.dbSetTemp - dblCurrentChamberTemp < -5)
	{
		//直接将炉子的温度设置到最小温度
		dblChamberPrepSetTemp = stChamberSetTemp.dbSetTemp - 30;
		if (dblChamberPrepSetTemp < stChamberSetTemp.dbMinTemp)
		{
			dblChamberPrepSetTemp = stChamberSetTemp.dbMinTemp;
		}
		if (!m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp))
		{
			m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp);
		}
	}
	else
	{
		//直接将炉子的温度设置到预设值
		dblChamberPrepSetTemp = stChamberSetTemp.dbSetTemp;

		if (!m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp))
		{
			m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp);
		}
		bSetBack = TRUE;
	}
	strTemp.Format("Chamber claim Setting Temp : %.2fC,Chamber Real Setting Temp: %.2fC", stChamberSetTemp.dbSetTemp, dblChamberPrepSetTemp);
	LogInfo(strTemp, strTemp.GetLength() + 1);

	bSetOverShowPoint = FALSE;

	//记录开始循环时间
	m_StartTime = CTime::GetCurrentTime();
	time1 = m_StartTime.GetTime();

	while (TRUE)
	{
		//读取当前VOA温度


		//读取当前炉子的温度
		if (!m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp))
		{
			m_cChamber.fnModBusReadCurrentPoint(dblCurrentChamberTemp);
		}
		//增加,读到的SFPVOA 温度与设置循环箱温度小于3度就直接退出
		if (fabs(dblCurrentTemp - stChamberSetTemp.dbSetTemp) <= 5) //测试用
		{
			dblChamberPrepSetTemp = stChamberSetTemp.dbSetTemp;
			if (!m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp))
			{
				m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp);
			}
			return TRUE;
			//bSetBack = TRUE;
		}


		//如果LC与炉子的温度还没有达到预设的差异内，则一直循环查询
		//如果已经达到差异内则设置SetBack为TRUE	
		strTemp.Format("Current SFP VOA Temp: %.2fC,Current Chamber Temp: %.2fC", dblCurrentTemp, dblCurrentChamberTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1);
		if (!bSetBack)
		{
			strTemp.Format("Current SFP VOA Temp: %.2fC,Current Chamber Temp: %.2fC", dblCurrentTemp, dblCurrentChamberTemp);
			LogInfo(strTemp, strTemp.GetLength() + 1);

			//如果当前LC的温度与LC理论上的Target温度差异在+/-6度内，设置预设值标志为TRUE,并记录此刻开始的时间
			if (fabs(dblCurrentTemp - dbTargetTemp) < 6.0)
			{
				bSetBack = TRUE;
				//炉子预设值标志设置为TRUE，设置炉子到预设值
				dblChamberPrepSetTemp = stChamberSetTemp.dbSetTemp;
				if (!m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp))
				{
					m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp);
				}
				//记录当前的开始时间
				m_StartTime = CTime::GetCurrentTime();
				time1 = m_StartTime.GetTime();
			}
			else
			{
				bSetBack = FALSE;
				//记录当前时间
				m_EndTime = CTime::GetCurrentTime();
				time2 = m_EndTime.GetTime();
				//如果从Overshoot开始到现在已经大于30分钟
				//则直接设置预设温度标志为TRUE
				if (time2 - time1 > 1800)
				{
					bSetBack = TRUE;
					time1 = time2; //将当前的时间作为开始时间
					//设置到预设值温度
					dblChamberPrepSetTemp = stChamberSetTemp.dbSetTemp;
					if (!m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp))
					{
						m_cChamber.fnModBusSetSetPoint(dblChamberPrepSetTemp);
					}
				}
			}
		}
		else
		{
			//在炉子设置到预设温度后前10分钟等待
			//10分钟后开始每3分钟做一次采集平均
			//当前后两次采集平均值小于0.1度
			//认为温度达到稳定状态
			m_EndTime = CTime::GetCurrentTime();
			time2 = m_EndTime.GetTime();
			if (time2 - time1 > 300)
			{
				//设置每3分钟进行一次平均值
				int nMonitorTime = 150;
				if (iTemperIndex == nMonitorTime)
				{
					//获得LC的平均值
					dbAVTemp[nInterval % 2] = dblTempTotal / nMonitorTime;
					strTemp.Format("SFP VOA AVE temperature:%.2f", dbAVTemp[nInterval % 2]);
					LogInfo(strTemp, strTemp.GetLength() + 1);

					if (fabs(dbAVTemp[1] - dbAVTemp[0]) < 0.3)
					{
						break;
					}
					dblTempTotal = 0.0;
					iTemperIndex = 0;
					nInterval++;
				}
				else
				{
					dblTempTotal += dblCurrentTemp;
				}
				iTemperIndex++;
			}
		}
		iIndex++;
		Sleep(1000);
	}

	return TRUE;
}





BOOL CTestTaskManager::AdjustFileExit(LPCSTR lpstrFileName)
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

/////////////////////////////////////////////////////////////////////////
//函数说明：
//    通过归零的插损计算归零文件的CRC校验码
//通过比对校验码判断是否认为改动过归零文件
//
//
//////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::CheckRefValide(CString strSection, BYTE bIndex)
{
	CString	strFileName, strMsg;
	CTime	time = CTime::GetCurrentTime();
	time_t	ltime, lCaliTime;
	DWORD	dwCRC32, dwCaliCRC32;
	CString strWL;
	long	lTemp;
	BYTE	i, j;
	PBYTE	pbDataByte = NULL;
	int   nYear, nMonth, nDate, nHour, nMin, nSec;
	try
	{
		strFileName.Format("%s\\Config\\Reference.ini", m_tszAppFolder);
		m_CRC.InitCRC32();
		if (!GetIniFileString(strSection, "CALI_TIME", strFileName, &strMsg))
			throw (char*)(LPCTSTR)strMsg;

		sscanf(strMsg, "%d-%d-%d   %d:%d:%d", &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec);
		CTime   ct(nYear, nMonth, nDate, nHour, nMin, nSec);
		lCaliTime = ct.GetTime();

		ltime = time.GetTime();
		CTimeSpan tsp(ltime - lCaliTime);
		long templ = tsp.GetTotalHours();


		if (!GetIniFileString(strSection, "CALI_CRC32", strFileName, &strMsg))
			throw (char*)(LPCTSTR)strMsg;

		sscanf(strMsg, "%X", &dwCaliCRC32);	//十六进制    
		// dwCaliCRC32 = atoi(strMsg);

		// 读取CRC32检查数据是否被人为修改过
		pbDataByte = (PBYTE)&lCaliTime;
		for (i = 0; i < 4; i++)
		{
			dwCRC32 = m_CRC.GetThisCRC(pbDataByte[i]);
		}

		pbDataByte = (PBYTE)&lTemp;
		//SW1与监控Tap端IL1


		// 		//Tap监控端到输出端的IL3
		// 		strWL.Format("SYS_REF_TOTAL_OFFSET_%.2f",m_stRefParameter.m_pdblLDWavelength[bIndex]);
		// 		if(strSection == strWL)
		// 		{
		// 			for(i = 0; i < m_stRefParameter.m_bCalibChanCount; i ++)
		// 			{
		// 				lTemp = (long)((m_stSystemRef.dbMonToOutpuIL[bIndex][i]+0.00005*(m_stSystemRef.dbMonToOutpuIL[bIndex][i]/fabs(m_stSystemRef.dbMonToOutpuIL[bIndex][i]))) * 10000);
		// 				for(j = 0; j < 4; j ++)
		// 				{
		// 					dwCRC32 = m_CRC.GetThisCRC(pbDataByte[j]);
		// 				}
		// 
		// 			}
		// 		}
		// 						
		// 		dwCRC32 = ~dwCRC32;
		// 		if(dwCaliCRC32 != dwCRC32)
		// 		{
		// 			strMsg.Format("校准文件[%s]的CRC32校验码为: %d, 实际校验数据计算CRC32校验码为: %d，不匹配，数据被人为修正过，请重新校准系统！！",
		// 				strFileName,dwCaliCRC32, dwCRC32);
		// 			//throw (char*)(LPCTSTR)strMsg;
		// 		}
	}
	catch (TCHAR* pstrErrorMsg)
	{
		strMsg.Format("%s", pstrErrorMsg);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);

		return	FALSE;
	}

	return	TRUE;
}

/////////////////////////////////////////////////////////////
//函数说明：
//    根据circulator1-2，2-3归零合并成一个文件，用于后续的计算
//Add by wanxin
//Time:20200708
////////////////////////////////////////////////////////////
BOOL CTestTaskManager::CombineCirculatorRef()
{
	CString strRefPath;
	CString strMsg;
	CString strTemp;
	strRefPath.Format(".\\Reference\\Circulator");
	CreateFullPath(strRefPath);
	InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
	InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL
	if (m_stScanParameter.bDoPDL)
	{
		m_FSTPObj.m_strCirculatorName.Format("%s\\DoPDL_Circulator_Ref_1_2.csv", strRefPath);
	}
	else
	{
		m_FSTPObj.m_strCirculatorName.Format("%s\\NoPDL_Circulator_Ref_1_2.csv", strRefPath);
	}


	if (!m_FSTPObj.ReadCirculatorRefDataFromFile(0))
	{
		strMsg.Format("%s", m_FSTPObj.GetErrorMsg());
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	if (m_stScanParameter.bDoPDL)
	{
		m_FSTPObj.m_strCirculatorName.Format("%s\\DoPDL_Circulator_Ref_2_3.csv", strRefPath);
	}
	else
	{
		m_FSTPObj.m_strCirculatorName.Format("%s\\NoPDL_Circulator_Ref_2_3.csv", strRefPath);
	}

	if (!m_FSTPObj.ReadCirculatorRefDataFromFile(1))
	{
		strMsg.Format("%s", m_FSTPObj.GetErrorMsg());
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	if (m_stScanParameter.bDoPDL)
	{
		m_FSTPObj.m_strCirculatorName.Format("%s\\DoPDL_Circulator_Ref.csv", strRefPath);
	}
	else
	{
		m_FSTPObj.m_strCirculatorName.Format("%s\\NoPDL_Circulator_Ref.csv", strRefPath);
	}
	if (!m_FSTPObj.CalculateCirculatorRefData())
	{
		strMsg.Format("%s", m_FSTPObj.GetErrorMsg());
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}


	int nPlotIndex = 0;
	for (int i = 1; i < m_stScanParameter.nPMCount; i++)
	{
		double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
		double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

		strTemp.Format("Circulator%d", i);
		AddPlotCount(0, strTemp);
		m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
		nPlotIndex++;
		if (m_stScanParameter.bDoPDL)
		{
			double *dbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i];
			m_pGraph->PlotXYS(1, pdbWLArry, dbPDLArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////
//函数说明：
//   Circulator 2-3插损归零
//Add by wanxin
//Time:20200708
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::Circulator23Reference()
{
	CString strRefPath;
	CString strMsg;
	DWORD  dwBoxRet;
	CString strTemp;

	strRefPath.Format(".\\Reference\\Circulator");
	CreateFullPath(strRefPath);
	InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
	InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

	if (m_stScanParameter.bDoPDL)
	{
		m_FSTPObj.m_strScanFileName.Format("%s\\DoPDL_Circulator_Ref_2_3.csv", strRefPath);
	}
	else
	{
		m_FSTPObj.m_strScanFileName.Format("%s\\NoPDL_Circulator_Ref_2_3.csv", strRefPath);
	}

	strMsg = _T("请将In1接环形器1的2端口，Out1接环形器的3端口\n,In2接环形器2的2端口,Out2接环形器2的3端口\n \
                In3接环形器3的2端口,Out接环形器3的3端口\n");
	dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (XSB_YES == dwBoxRet)
	{
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID, TRUE))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID, TRUE))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}

		int nPlotIndex = 0;
		for (int i = 1; i < m_stScanParameter.nPMCount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

			strTemp.Format("Circulator%d", i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			if (m_stScanParameter.bDoPDL)
			{
				double *dbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i];
				m_pGraph->PlotXYS(1, pdbWLArry, dbPDLArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
				nPlotIndex++;
			}
		}
	}
	else
	{
		strMsg.Format("不带circultor测试,必须要归零circulator,不能人工取消");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////
//函数说明：
//   Circulator 1-2插损归零
//Add by wanxin
//Time:20200708
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::Circulator12Reference()
{
	CString strRefPath;
	CString strMsg;
	DWORD  dwBoxRet;
	CString strTemp;

	strRefPath.Format(".\\Reference\\Circulator");
	CreateFullPath(strRefPath);
	InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
	InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

	if (m_stScanParameter.bDoPDL)
	{
		m_FSTPObj.m_strScanFileName.Format("%s\\DoPDL_Circulator_Ref_1_2.csv", strRefPath);
	}
	else
	{
		m_FSTPObj.m_strScanFileName.Format("%s\\NoPDL_Circulator_Ref_1_2.csv", strRefPath);
	}

	strMsg = _T("请将In1接环形器1的1端口，Out1接环形器的2端口\n,In2接环形器2的1端口,Out2接环形器2的2端口\n \
                In3接环形器3的1端口,Out接环形器3的2端口\n");
	dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (XSB_YES == dwBoxRet)
	{
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID, TRUE))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID, TRUE))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}

		int nPlotIndex = 0;
		for (int i = 1; i < m_stScanParameter.nPMCount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i];

			strTemp.Format("Circulator%d", i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			if (m_stScanParameter.bDoPDL)
			{
				double *dbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i];
				m_pGraph->PlotXYS(1, pdbWLArry, dbPDLArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
				nPlotIndex++;
			}
		}
	}
	else
	{
		strMsg.Format("不带circultor测试,必须要归零circulator,不能人工取消");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}



	return TRUE;
}

BOOL CTestTaskManager::DoCirculatorRef()
{
	CString strMsg;
	strMsg.Format("开始Circulator1-2归零扫描.......");
	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
	if (!Circulator12Reference())
	{
		return FALSE;
	}
	strMsg.Format("Circulator1-2归零扫描完成");
	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);

	strMsg.Format("开始Circulator2-3归零扫描.......");
	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
	if (!Circulator23Reference())
	{
		return FALSE;
	}
	strMsg.Format("Circulator2-3归零扫描完成");
	LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
	if (!CombineCirculatorRef())
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////
//函数说明:
//    系统光学归零
//Add by wanxin
//Time:20160225
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::SystemReference()
{
	CString strRefPath;
	CString strMsg;
	CString strTemp;
	CString	strFileName;
	BOOL  bFunctionOK = TRUE;
	int i = 0;
	try
	{
		strRefPath.Format(".\\Reference");
		CreateFullPath(strRefPath);
		m_bRefDone = FALSE;
		if (!m_bOpenTestDevice)
		{
			throw "请先初始化相应光学测试设备!";
		}
		if (m_stScanParameter.bDoPDL)
		{
			m_FSTPObj.m_strRefFileName.Format("%s\\DoPDL_Reference.csv", strRefPath);
		}
		else
		{
			m_FSTPObj.m_strRefFileName.Format("%s\\NoPDL_Reference.csv", strRefPath);
		}

		if (m_stScanParameter.bCirculator)
		{
			if (m_stScanParameter.bDoPDL)
			{
				m_FSTPObj.m_strCirculatorName.Format("%s\\Circulator\\DoPDL_Circulator_Ref.csv", strRefPath);
			}
			else
			{
				m_FSTPObj.m_strCirculatorName.Format("%s\\Circulator\\NoPDL_Circulator_Ref.csv", strRefPath);
			}
		}

		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
		InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

		strMsg = _T("是否调用现在的归零参数?\n" "是: 调用上次归零参数!\n" "否: 执行归零步骤!");
		DWORD dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
		if (XSB_YES == dwBoxRet)
		{
			if (!AdjustFileExit(m_FSTPObj.m_strRefFileName))
			{
				strMsg.Format("%s reference file not exsit", m_FSTPObj.GetErrorMsg());
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return	FALSE;
			}
			if (!m_FSTPObj.ReadReferenceDataFromFile())
			{
				strMsg.Format("%s", m_FSTPObj.GetErrorMsg());
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return	FALSE;
			}
			if (m_stScanParameter.bCirculator)
			{
				if (!AdjustFileExit(m_FSTPObj.m_strCirculatorName))
				{
					strMsg.Format("%s reference file not exsit", m_FSTPObj.GetErrorMsg());
					LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
					return	FALSE;
				}
			}

			return TRUE;
		}

		strMsg = _T("请将");
		for (int i = 1; i < m_stScanParameter.nPMCount; i++)
		{
			strTemp.Format("In%d端接Out%d端,", i, i, i);
			strMsg += strTemp;
		}
		dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);

		if (m_nSwitchBoxOnline == 1)
		{
			for (int i = 0; i < m_stScanParameter.nPMCount; i++)
			{
				if (!SwitchBoxByPort(1, i + 1))
				{
					return FALSE;
				}
			}
		}
		strMsg.Format("开始进行归零扫描.......");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		if (!m_FSTPObj.DoReference(m_FSTPObj.m_nGUID))
		{
			strMsg = m_FSTPObj.GetErrorMsg();
			throw strMsg.GetBuffer(0);
		}
		int nPlotIndex = 0;
		for (int i = 1; i < m_stScanParameter.nPMCount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanRefResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanRefResult.pdbPMResult[i];

			strTemp.Format("通道%d", i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			if (m_stScanParameter.bDoPDL)
			{
				double *dbPDLArry = m_FSTPObj.m_stScanRefResult.pdbPDLResult[i];
				m_pGraph->PlotXYS(1, pdbWLArry, dbPDLArry, m_stScanParameter.dwSampleCount, nPlotIndex, 0);
				nPlotIndex++;
			}
		}
		strMsg.Format("归零扫描完成");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);

		if (m_stScanParameter.bCirculator)
		{
			strMsg.Format("开始Circulator归零扫描.......");
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
			if (!DoCirculatorRef())
			{
				return FALSE;
			}
			strMsg.Format("Circulator归零扫描完成");
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		}

		strMsg.Format("系统校准完成!");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_NORMAL);
		m_bRefDone = TRUE;
	}
	catch (TCHAR* pstrErrorMsg)
	{
		strMsg.Format("%s", pstrErrorMsg);
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	return bFunctionOK;
}


///////////////////////////////////////////////////////////////
//函数说明：
//    根据任务ID判断当前测试温度，并复制测试温度结构体
//Add by wanxin
//20160225
///////////////////////////////////////////////////////////////
void CTestTaskManager::TemperatureIndex(int index)
{
	//增加获取当前温度进行测试

	m_stCfg.dbMinTemp = -5.0;
	m_stCfg.dbMaxTemp = 80;
	m_stCfg.dbDiffer = 1.0;
	m_stCfg.dbTolerance = 1.0;
	m_stCfg.dbTempSlope = 0.002;
	if (index >= 200 && index <= 299)
	{
		m_stCfg.nSetType = 0;//定标
		m_stCfg.dbSetTemp = 30.0;

	}
	if (index >= 300 && index <= 399)
	{
		m_stCfg.nSetType = 0;//定标
		m_stCfg.dbSetTemp = -5.0;
	}
	if (index >= 400 && index <= 499)
	{
		m_stCfg.nSetType = 0;//定标
		m_stCfg.dbSetTemp = 5.0;
	}
	if (index >= 500 && index <= 599)
	{
		m_stCfg.nSetType = 0;//定标
		m_stCfg.dbSetTemp = 13.0;
	}
	if (index >= 600 && index <= 799)
	{
		m_stCfg.nSetType = 0;//定标
		m_stCfg.dbSetTemp = 50.0;
	}
	if (index >= 700 && index <= 799)
	{
		m_stCfg.nSetType = 0;//定标
		m_stCfg.dbSetTemp = 70;
	}
	if (index >= 900 && index <= 999)
	{
		m_stCfg.nSetType = 1;//验证
		m_stCfg.dbSetTemp = 70;
	}
	if (index >= 1000 && index <= 1099)
	{
		m_stCfg.nSetType = 1;//验证
		m_stCfg.dbSetTemp = 55;
	}
	if (index >= 1100 && index <= 1199)
	{
		m_stCfg.nSetType = 1;//验证
		m_stCfg.dbSetTemp = 10;
	}
	if (index >= 1200 && index <= 1299)
	{
		m_stCfg.nSetType = 1;//验证
		m_stCfg.dbSetTemp = -5;
	}
	if (index >= 1300 && index <= 1399)
	{
		m_stCfg.nSetType = 1;//验证
		m_stCfg.dbSetTemp = 20;
	}
	SetDGEModuleCurrentTemp(m_stCfg.dbSetTemp);
}

BOOL CTestTaskManager::UploadTestResult(int iDataID, CString strOptContInfo, CString strDataName,
	double dblMinRngData, double dblMaxRngData, double dblResult)
{
	CString	strMsg;
	stProcessDataInfo stProcessResult;

	// 传递测试结果到界面 
	//	ZeroMemory(&stProcessResult,sizeof(stProcessDataInfo));

	stProcessResult.iDataID = iDataID;

	// 光学连接方式
	sprintf(stProcessResult.pszOpLine, "%s", strOptContInfo);

	// 光学参数
	sprintf(stProcessResult.pszDataName, "%s", strDataName);

	// 数据判断门限
	stProcessResult.dbDataRangeLow = dblMinRngData;
	stProcessResult.dbDataRangeUp = dblMaxRngData;
	// 测试数据
	stProcessResult.dbDataResult = dblResult;
	if (!m_pNetCommand->XmlSendProcessResult(stProcessResult))
	{
		strMsg = "发送验证数据到UI失败!";
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);

		return	FALSE;
	}
	return	TRUE;
}

void CTestTaskManager::LogInfo(CString strLogInfo, int nLength, BYTE bLogType, int nModuleIndex)
{
	CString strFileName;
	CString	strLogMsg;
	FILE*	pLogFile = NULL;
	SYSTEMTIME time;

	m_pNetCommand->XmlSendLog(strLogInfo, nLength, (LOGLEVEL)bLogType);

	if (m_DGEModule[nModuleIndex].m_strLogFileName != "")
	{
		strFileName = m_DGEModule[nModuleIndex].m_strLogFileName;
		GetLocalTime(&time);

		// 增加系统时间
		strLogMsg.Format("%4d-%2d-%2d %2d:%2d:%2d %s", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, strLogInfo);

		pLogFile = fopen(strFileName.GetBuffer(0), "at");

		if (pLogFile != NULL)
		{
			fprintf(pLogFile, "%s\n", strLogMsg);
			fclose(pLogFile);
		}
	}
}

BOOL CTestTaskManager::CheckGoldenSample(CString GoldenSampleSN, CString TestSN)
{

	DWORD   dwBuf[256];
	int     nNodeCount = 0;
	CString strTemp = _T("");
	CString	strURL = _T("");
	CString	strLocalFile = _T("");
	CString	strErrMsg = _T("");
	int     iRandNo = rand();
	CString strSection = _T("");
	CXMLControl	m_ctrXMLCtr;

	strURL.Format("http://172.18.1.101/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&workstation=%s", TestSN, iRandNo, GoldenSampleSN);//终测
	strLocalFile.Format("%s\\Test.xml", m_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		if (!m_ctrXMLCtr.CloseXMLFile())
		{
			strTemp.Format("无纸化关闭失败,请通知相关工程师!");
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		}
		strTemp.Format("模板下载错误!");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Error", dwBuf, nNodeCount);
	if (nNodeCount > 0)
	{
		m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
		if (strTemp.GetLength() > 0)
		{
			if (!m_ctrXMLCtr.CloseXMLFile())
			{
				strTemp.Format("无纸化关闭失败,请通知相关工程师!");
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			}
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strTemp.Format("无纸化关闭失败,请通知相关工程师!");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
	}
	return TRUE;
}

BOOL CTestTaskManager::InitGraphShow(int nGraphIndex, CString X_strTemp, CString Y_strTemp, CString strGraphTitle)
{
	//if (!m_pGraph->GraphClear())
	//{

	//}

	_tcscpy(m_Graphinfo.pszTitle, strGraphTitle);
	m_Graphinfo.iGraphIndex = nGraphIndex;
	m_Graphinfo.stScale[0].bScaleAuto = 1;
	m_Graphinfo.stScale[0].bScaleUse = 1;
	//info.stScale[0].dblScaleStart=0;
	//info.stScale[0].dblScaleEnd=50;
	strcpy(m_Graphinfo.stScale[0].pszScaleName, X_strTemp.GetBuffer(X_strTemp.GetLength()));
	m_Graphinfo.stScale[1].bScaleAuto = 1;
	m_Graphinfo.stScale[1].bScaleUse = 1;
	//info.stScale[1].dblScaleStart=-2;
	//info.stScale[1].dblScaleEnd=2;	
	strcpy(m_Graphinfo.stScale[1].pszScaleName, Y_strTemp.GetBuffer(Y_strTemp.GetLength()));

	m_pGraph->GraphInit(m_Graphinfo);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
//函数说明：
//    设置激光器函数
//Add by wanxin
//Time:20160225
//////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::SetTLSWavelength(double m_pdblLDWavelength)
{
	CString strMsg;
	//如果设置不成功，总共设置3次
	return TRUE;
}




DWORD CTestTaskManager::SwapDWORD(DWORD dwInData)
{
	BYTE* pch = (BYTE*)&dwInData;

	pch[0] ^= pch[3];
	pch[3] ^= pch[0];
	pch[0] ^= pch[3];

	pch[1] ^= pch[2];
	pch[2] ^= pch[1];
	pch[1] ^= pch[2];

	return dwInData;
}

WORD CTestTaskManager::SwapWORD(WORD wInData)
{
	BYTE* pch = (BYTE*)&wInData;

	pch[0] ^= pch[1];
	pch[1] ^= pch[0];
	pch[0] ^= pch[1];

	return wInData;

}
BOOL CTestTaskManager::StartFWUpdate()
{
	BYTE bytReadLength;
	BYTE bySetValueH = 0, bySetValueL = 0;
	BYTE byIndex;
	BYTE pbyHiAddr;
	BYTE pbyLoAddr;
	BYTE bytSlaveAddr = 0xB8;
	CString strCommand, strMsg;
	char        chSetCommand[MAX_PATH];
	char        chReadCommand[MAX_PATH];

	ZeroMemory(chSetCommand, MAX_PATH);
	ZeroMemory(chReadCommand, MAX_PATH);

	byIndex = 0;
	bytReadLength = 1;
	//bySetValueH = 0x00;
	bySetValueL = 0x01;
	pbyHiAddr = HIBYTE(UDATEFLAG);
	pbyLoAddr = LOBYTE(UDATEFLAG);

	strCommand.Format("SetI2C %02x %02x %02x%02x%02x\r", byIndex, bytSlaveAddr,
		pbyHiAddr, pbyLoAddr, bySetValueL);

	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	// 	if (!m_CmdVOA.WriteBuffer(chSetCommand,strCommand.GetLength()))
	// 	{
	// 		LogInfo("串口通讯错误！",TRUE);
	// 		return FALSE;
	// 	}
	// 	Sleep(100);	
	// 	if (!m_CmdVOA.ReadBuffer(chReadCommand,MAX_PATH,&dwFeedBackLength))
	// 	{
	// 		LogInfo("读取I2C数据错误！",TRUE);
	// 		return FALSE;
	// 	}
	strMsg.Format("%s", &chReadCommand);
	LogInfo(strMsg, strMsg.GetLength() + 1);
	return TRUE;

}
BOOL CTestTaskManager::FWTranSportFW(PBYTE byDownloadBuf)
{
	BYTE bytReadLength;
	BYTE bySetValueH = 0, bySetValueL = 0;
	BYTE byIndex;
	BYTE pbyHiAddr;
	BYTE pbyLoAddr;
	BYTE bytSlaveAddr = 0xB8;
	//	BYTE ptCommand[128];
	CString strCommand, strMsg, strData;
	char        chSetCommand[MAX_PATH];
	char        chReadCommand[MAX_PATH];
	int count = 0;
	int i = 0;

	ZeroMemory(chSetCommand, MAX_PATH);
	ZeroMemory(chReadCommand, MAX_PATH);

	byIndex = 0;
	bytReadLength = 1;
	pbyHiAddr = HIBYTE(WRITEFWDATA);
	pbyLoAddr = LOBYTE(WRITEFWDATA);

	strCommand.Format("SetI2C %02x %02x %02x%02x", byIndex, bytSlaveAddr,
		pbyHiAddr, pbyLoAddr);
	strMsg = "";
	for (i = 0; i < 32; i++)
	{
		strData.Format("%02x", byDownloadBuf[i]);
		strMsg += strData;
	}
	strMsg = strMsg + "\r";
	strCommand = strCommand + strMsg;

	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	// 	if (!m_CmdVOA.WriteBuffer(chSetCommand,strCommand.GetLength()))
	// 	{
	// 		//LogInfo("串口通讯错误！",TRUE);
	// 		return FALSE;
	// 	}
	// 	Sleep(100);	
	// 	if (!m_CmdVOA.ReadBuffer(chReadCommand,MAX_PATH,&dwFeedBackLength))
	// 	{
	// 		//LogInfo("读取I2C数据错误！",TRUE);
	// 		return FALSE;
	// 	}
	strMsg.Format("%s", &chReadCommand);
	//LogInfo(strMsg,strMsg.GetLength()+1);
	if (strcmp("E:Invalid command\r>", strMsg) == 0)
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CTestTaskManager::CheckFWUpdateStatus()
{
	BYTE bytReadLength;
	BYTE bySetValueH = 0, bySetValueL = 0;
	BYTE byIndex;
	BYTE pbyHiAddr;
	BYTE pbyLoAddr;
	BYTE bytSlaveAddr = 0xB8;
	CString strCommand, strMsg, strData;
	char        chSetCommand[MAX_PATH];
	char        chReadCommand[MAX_PATH];
	int count = 0;
	int i = 0;

	ZeroMemory(chSetCommand, MAX_PATH);
	ZeroMemory(chReadCommand, MAX_PATH);

	byIndex = 0;
	bytReadLength = 1;
	pbyHiAddr = HIBYTE(READFWUPDATASTATUS);
	pbyLoAddr = LOBYTE(READFWUPDATASTATUS);

	strCommand.Format("SetI2C %02x %02x %02x%02x\r", byIndex, bytSlaveAddr,
		pbyHiAddr, pbyLoAddr);

	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	// 	if (!m_CmdVOA.WriteBuffer(chSetCommand,strCommand.GetLength()))
	// 	{
	// 		LogInfo("串口通讯错误！",TRUE);
	// 		return FALSE;
	// 	}
	// 	Sleep(100);	
	// 	if (!m_CmdVOA.ReadBuffer(chReadCommand,MAX_PATH,&dwFeedBackLength))
	// 	{
	// 		LogInfo("读取I2C数据错误！",TRUE);
	// 		return FALSE;
	// 	}
	strMsg.Format("%s", &chReadCommand);
	//	LogInfo(strMsg,strMsg.GetLength()+1);

	//Read
	DWORD dwReadLength = 1;
	ZeroMemory(chReadCommand, MAX_PATH);
	bytSlaveAddr = 0xB9;
	strCommand.Format("GetI2C %02x %02x %02x\r", byIndex, bytSlaveAddr,
		dwReadLength);

	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	// 	if (!m_CmdVOA.WriteBuffer(chSetCommand,strCommand.GetLength()))
	// 	{
	// 		//LogInfo("串口通讯错误！",TRUE);
	// 		return FALSE;
	// 	}
	// 	Sleep(100);	
	// 	if (!m_CmdVOA.ReadBuffer(chReadCommand,dwReadLength,&dwFeedBackLength))
	// 	{
	// 		//LogInfo("读取I2C数据错误！",TRUE);
	// 		return FALSE;
	// 	}
	strMsg.Format("%s", &chReadCommand);
	//LogInfo(strMsg,strMsg.GetLength()+1);
	if (strMsg.Find("0") != -1)
	{
		strMsg = "ok";
		LogInfo(strMsg, strMsg.GetLength() + 1);
		//LogInfo("OK",TRUE);
		return TRUE;
	}
	else if (strMsg.Find("1") != -1)
	{
		//LogInfo("BUSY",TRUE);
		strMsg = "BUSY";
		LogInfo(strMsg, strMsg.GetLength() + 1);
	}
	else if (strMsg.Find("2") != -1)
	{

		strMsg = "File size doesn't match";
		LogInfo(strMsg, strMsg.GetLength() + 1);
	}
	else if (strMsg.Find("3") != -1)
	{
		//	LogInfo("BUSY",TRUE);
		strMsg = "3";
		LogInfo(strMsg, strMsg.GetLength() + 1);
	}
	else if (strMsg.Find("4") != -1)
	{
		//	LogInfo("Flash Programming Error",TRUE);
		strMsg = "Flash Programming Error";
		LogInfo(strMsg, strMsg.GetLength() + 1);
	}
	else
	{
		strMsg = "未知状态";
		LogInfo(strMsg, strMsg.GetLength() + 1);
	}
	return FALSE;

}

BOOL CTestTaskManager::ArmNewFWImage()
{
	BYTE bytReadLength;
	BYTE bySetValueH = 0, bySetValueL = 0;
	BYTE byIndex;
	BYTE pbyHiAddr;
	BYTE pbyLoAddr;
	BYTE bytSlaveAddr = 0xB8;
	CString strCommand, strMsg, strData;
	char        chSetCommand[MAX_PATH];
	char        chReadCommand[MAX_PATH];
	int count = 0;
	int i = 0;
	BYTE       byValue = 0x01;

	ZeroMemory(chSetCommand, MAX_PATH);
	ZeroMemory(chReadCommand, MAX_PATH);

	byIndex = 0;
	bytReadLength = 1;
	pbyHiAddr = HIBYTE(ARMNEWFWIMAGE);
	pbyLoAddr = LOBYTE(ARMNEWFWIMAGE);

	strCommand.Format("SetI2C %02x %02x %02x%02x%02x\r", byIndex, bytSlaveAddr,
		pbyHiAddr, pbyLoAddr, byValue);

	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	// 	if (!m_CmdVOA.WriteBuffer(chSetCommand,strCommand.GetLength()))
	// 	{
	// 		//LogInfo("串口通讯错误！",TRUE);
	// 		return FALSE;
	// 	}
	// 	Sleep(100);	
	// 	if (!m_CmdVOA.ReadBuffer(chReadCommand,MAX_PATH,&dwFeedBackLength))
	// 	{
	// 		//LogInfo("读取I2C数据错误！",TRUE);
	// 		return FALSE;
	// 	}
	strMsg.Format("%s", &chReadCommand);
	//LogInfo(strMsg,strMsg.GetLength()+1);
	if (strcmp("E:Invalid command\r>", strMsg) == 0)
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CTestTaskManager::EndFWUpdate()
{
	BYTE bytReadLength;
	BYTE bySetValueH = 0, bySetValueL = 0;
	BYTE byIndex;
	BYTE pbyHiAddr;
	BYTE pbyLoAddr;
	BYTE bytSlaveAddr = 0xB8;
	CString strCommand, strMsg;
	char        chSetCommand[MAX_PATH];
	char        chReadCommand[MAX_PATH];

	ZeroMemory(chSetCommand, MAX_PATH);
	ZeroMemory(chReadCommand, MAX_PATH);

	byIndex = 0;
	bytReadLength = 1;
	bySetValueH = 0x00;
	//bySetValueL = 0x00;
	pbyHiAddr = HIBYTE(UDATEFLAG);
	pbyLoAddr = LOBYTE(UDATEFLAG);

	strCommand.Format("SetI2C %02x %02x %02x%02x%02x\r", byIndex, bytSlaveAddr,
		pbyHiAddr, pbyLoAddr, bySetValueH);

	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	// 	if (!m_CmdVOA.WriteBuffer(chSetCommand,strCommand.GetLength()))
	// 	{
	// 		//LogInfo("串口通讯错误！",TRUE);
	// 		return FALSE;
	// 	}
	// 	Sleep(100);	
	// 	if (!m_CmdVOA.ReadBuffer(chReadCommand,MAX_PATH,&dwFeedBackLength))
	// 	{
	// 		//LogInfo("读取I2C数据错误！",TRUE);
	// 		return FALSE;
	// 	}
	strMsg.Format("%s", &chReadCommand);
	//LogInfo(strMsg,strMsg.GetLength()+1);
	return TRUE;

}
/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE Mapping table
//Add by wanxin
//Time:@0200608
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadMappingTable()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionInfo(i))
		{
			return FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		CreateFullPath(strLutFolder);
		strFileName.Format("%s\\Mapping_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateMappingTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateMappingTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载Mapping出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;
}


/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE MFG info
//Add by wanxin
//Time:@0200608
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadMfgInfo()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionInfo(i))
		{
			return FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		CreateFullPath(strLutFolder);
		strFileName.Format("%s\\ModuleInfo_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateMfgInfoTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateMfgInfoTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载Mfg信息出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}


/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE pram info
//Add by wanxin
//Time:@0200612
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadParamInfo()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		CreateFullPath(strLutFolder);
		strFileName.Format("%s\\ModulePara_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateDutParaTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateDutParaTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载Param信息出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}

/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE wavelength校准表
//Add by wanxin
//Time:@0200612
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadWavCalibrationTable()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionTestingInfo(i))
		{
			strTemp.Format("DGE模块%d读取产品测试信息出现错误", i + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
			m_DGEModule[i].m_bConnect = FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		strFileName.Format("%s\\Wave_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateWaveTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateWaveTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载波长校准表出现错误:%s", i + 1, m_DGEModule[i].m_strErrorMsg);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}

/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE Attenuation校准表
//Add by wanxin
//Time:@0200612
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadAttenCalibrationTable()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionTestingInfo(i))
		{
			strTemp.Format("DGE模块%d读取产品测试信息出现错误", i + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
			m_DGEModule[i].m_bConnect = FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		strFileName.Format("%s\\Atten_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateAttenTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateAttenTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载衰减校准表出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}


/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE attenuation offset校准表
//Add by wanxin
//Time:@0200612
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadAttenOffsetTable()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionTestingInfo(i))
		{
			strTemp.Format("DGE模块%d读取产品测试信息出现错误", i + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
			m_DGEModule[i].m_bConnect = FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		strFileName.Format("%s\\DAC_Offset_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateAttenDacOffsetTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateAttenDacOffsetTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载衰减offset校准表出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}



/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE attenuation offset校准表
//Add by wanxin
//Time:@0200612
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadILCalibartionTable()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionTestingInfo(i))
		{
			strTemp.Format("DGE模块%d读取产品测试信息出现错误", i + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
			m_DGEModule[i].m_bConnect = FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		strFileName.Format("%s\\IL_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateILTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateILTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载插损校准表出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}



/////////////////////////////////////////////////////////
//函数说明：
//    下载DGE attenuation offset校准表
//Add by wanxin
//Time:@0200612
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnDownloadBeamCalibartionTable()
{
	CString strLutFolder;
	CString strFileName;
	CString strTemp;
	BOOL bFunctionOk = FALSE;

	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!ReadProductionTestingInfo(i))
		{
			strTemp.Format("DGE模块%d读取产品测试信息出现错误", i + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
			m_DGEModule[i].m_bConnect = FALSE;
		}
		strLutFolder.Format("%s\\DownLut", m_DGEModule[i].m_strCaliFolder);
		strFileName.Format("%s\\Beam_%s.bin", strLutFolder, m_DGEModule[i].m_DGEInfo.strSN);

		if (!m_DGEModule[i].fnCreateBeamTab(strFileName))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[i].fnCreateBeamTab(strFileName))
			{
				strTemp.Format("DGE模块%d下载Beam校准表出现错误", i + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, i);
				m_DGEModule[i].m_bConnect = FALSE;
				continue;
			}
		}
	}

	for (i = 0; i < m_nDGECount; i++)
	{
		bFunctionOk |= m_DGEModule[i].m_bConnect;
	}

	return bFunctionOk;

}


////////////////////////////////////////////////////////////
//函数说明：
//   延迟时间函数
//Add by wanxin
//Time:20200615
///////////////////////////////////////////////////////////
void CTestTaskManager::fnSleepLongTime(DWORD dwSleep_ms)
{
	if (dwSleep_ms < 500)
	{
		Sleep(dwSleep_ms);
	}
	else
	{
		DWORD dwPer500ms = dwSleep_ms / 500;
		for (int i = 0; i < dwPer500ms; i++)
		{
			Sleep(500);
		}
	}
}


////////////////////////////////////////////////////////////////////
//函数说明：
//   获取到模块当前LC温度并写入文件中
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnGetTemperWriteIni(int nModuleIndex, double &dblTemp, LPCTSTR lpName, BOOL bCalibrate)
{
	CString strTemp;
	CString strData;

	// read 3 
	int iSum = 3;
	dblTemp = 0.0;
	while (iSum--)
	{
		if (!m_DGEModule[nModuleIndex].m_Module.GetLCTemper(strData))
		{
			OpenSerialPortDevice();
			Sleep(1000);
			if (!m_DGEModule[nModuleIndex].m_Module.GetLCTemper(strData))
			{
				strTemp = m_DGEModule[nModuleIndex].m_strErrorMsg;
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			}
		}
		dblTemp += atof(strData);
	}
	dblTemp /= 3;

	strTemp.Format("模块%d当前LC Temper: %.2fC", nModuleIndex + 1, dblTemp);
	LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

	CString strPath;
	strPath.Format("%s\\Config", m_DGEModule[nModuleIndex].m_strCaliFolder);
	CreateFullPath(strPath);
	CString strFileName;
	CString strSection, strKey;
	strFileName.Format("%s\\Config\\ModuleConfig.ini", m_DGEModule[nModuleIndex].m_strCaliFolder);

	if (bCalibrate)
	{
		strSection.Format("CAL_TEMP");
	}
	else
	{
		strSection.Format("VER_TEMP");
	}
	strTemp.Format("%s", m_DGEModule[nModuleIndex].m_strTempFolder);
	strKey.Format("Temp_%s_%s", lpName, strTemp);
	strTemp.Format("%.2f", dblTemp);
	WritePrivateProfileString(strSection, strKey, strTemp, strFileName);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////
// 函数说明
//    读取Atten定标扫描文件
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnReadAttenVsGhz(LPCTSTR lpstPath, DWORD dwScanCount, float *pfDac, double *pdblAtten, int *pnLine, double *pdblGhz)
{
	CString strTemp;

	int			nLine = 0;
	//DWORD        dwScanCount   = m_stScanParameter.m_8164ScanParam.m_dwSampleCount;
	DWORD        dwBufSize = (dwScanCount + 1) * 14; //"196571.700000,"，每个频率最大字符数

	char		    *lineBuf = new char[dwBufSize];
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;
	LPTSTR		pStr;
	FILE * fpFile = NULL;
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
			strTemp.Format("Open file %s failed!", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		// first line: GHZ
		memset(lineBuf, 0, sizeof(char)*dwBufSize);
		pStr = fgets(lineBuf, dwBufSize, fpFile);
		if (pStr == NULL)
		{
			strTemp.Format("Read file %s failed!", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		token = strtok(lineBuf, seps);

		for (int i = 0; i < (int)dwScanCount; i++)
		{
			token = strtok(NULL, seps);
			pdblGhz[i] = atof(token);
		}

		while (TRUE)
		{
			memset(lineBuf, 0, sizeof(char)*dwBufSize);
			pStr = fgets(lineBuf, dwBufSize, fpFile);
			if (NULL == pStr)
				break;

			token = strtok(lineBuf, seps);
			//pdwDac[nLine] = atoi(token);
			pfDac[nLine] = atof(token);
			for (int i = 0; i < (int)dwScanCount; i++)
			{
				token = strtok(NULL, seps);
				pdblAtten[nLine*dwScanCount + i] = atof(token);
			}

			nLine++;
		}

		*pnLine = nLine;
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
	}

	if (lineBuf != NULL)
	{
		delete[] lineBuf;
		lineBuf = NULL;
	}

	return bFunctionOK;
}

///////////////////////////////////////////////////////////
//函数说明：
//    从ini文件中读取Pixel中心波长
//Add by wanxin
//Time:20200615
//////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnReadPixelWaveIni(LPCTSTR lpstPath, stPixelInfo *pstPixel, BOOL bCal)
{
	CString strTemp;
	CString strMsg;
	CString strSection;
	CString strKey;

	try
	{
		for (int nIndex = 0; nIndex < PIXEL_NUMBER_64; nIndex++)
		{
			strSection.Format("CENTER_WAVE");
			strKey.Format("WAVE_%d", nIndex);

			//Wavelength nm
			strTemp = m_GernalObj.ReadIniFileString(lpstPath, strSection, strKey);
			if (strTemp.IsEmpty())
			{
				strMsg.Format("Pixel(%d) Wave: %.3fnm, it can't be empty!", nIndex + 1, strTemp);
				throw strMsg.GetBuffer(0);
			}
			pstPixel->dblPixelWav[nIndex] = atof(strTemp);

			strKey.Format("GHZ_%d", nIndex);
			strTemp = m_GernalObj.ReadIniFileString(lpstPath, strSection, strKey);
			if (strTemp.IsEmpty())
			{
				strMsg.Format("Pixel(%d) GHZ: %.3fGhz, it can't be empty!", nIndex + 1, strTemp);
				throw strMsg.GetBuffer(0);
			}
			pstPixel->dblPixelGhz[nIndex] = atof(strTemp);


			strKey.Format("IL_%d", nIndex);
			strTemp = m_GernalObj.ReadIniFileString(lpstPath, strSection, strKey);
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
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////
//函数说明：
//   保存Atten校准的文件
//Add by wqanxin
//Time:20200615
/////////////////////////////////////////////////////////////
BOOL CTestTaskManager::SaveCaliPixelAtten(LPCTSTR lpstPath, int nLine, double *pdblPixelGhz, float *pdwDac, double *pdblScanGhz, double *pdblAtten)
{
	BOOL         bFunctionOK = TRUE;

	CString      strTemp;
	CString      strValue;
	CString      strFileName;
	FILE*        pFile = NULL;

	double       dblThisGHz = 0;
	double       dblPixelAtten[PIXEL_NUMBER_64][ATTEN_LUT_POINT];
	double       dblStartGhz = VELOCITY_LIGHT / m_stScanParameter.dblStartWL;
	double       dblStopGhz = VELOCITY_LIGHT / m_stScanParameter.dblStopWL;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;

	try
	{

		strFileName = lpstPath;

		//read the IL data
		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		//title for content: DAC, Ghz1, Ghz2, .....Ghz64
		if (strFileName.Find("Atten_Ver") != -1) //衰减验证测试
		{
			strTemp.Format("AttenSet");
		}
		else if (strFileName.Find("PDL_Ver") != -1) //衰减验证测试
		{
			strTemp.Format("AttenSet");
		}
		else if (strFileName.Find("Curve_Scan_Ver") != -1) //Curve验证测试
		{
			strTemp.Format("Curve_Index");
		}
		else
		{
			strTemp.Format("DAC");
		}
		fprintf(pFile, "%s", strTemp);

		strValue.Empty();
		for (int k = 0; k < PIXEL_NUMBER_64; k++)
		{
			dblThisGHz = pdblPixelGhz[k];
			strValue.Format(",%.3f", dblThisGHz);
			fprintf(pFile, "%s", strValue);
		}
		fprintf(pFile, "\n", strTemp);

		//check the pixel Ghz count
		//if (PIXEL_NUMBER_64 != k)
		//{
		//	strTemp.Format("保存Pixel number: %d和配置数量: %d不一致!", k, PIXEL_NUMBER_64);
		//	throw strTemp.GetBuffer(0);
		//}

		for (int i = 0; i < PIXEL_NUMBER_64; i++)//查找每一个Pixel Ghz
		{
			int nGHzIdx = 0;

			dblThisGHz = pdblPixelGhz[i];

			// Search GHz index
			for (int j = 1; j < dwSampleCount; j++)
			{
				if (dblThisGHz >= pdblScanGhz[0])
				{
					nGHzIdx = 0;
					break;
				}
				else if (dblThisGHz <= pdblScanGhz[dwSampleCount - 1])
				{
					nGHzIdx = dwSampleCount - 1;
					break;
				}
				else
				{
					if ((dblThisGHz < pdblScanGhz[j - 1]) && (dblThisGHz >= pdblScanGhz[j]))
					{
						nGHzIdx = j;
						break;
					}
				}
			}// get GHz index


			 // save to array
			for (j = 0; j < nLine; j++)
			{
				dblPixelAtten[i][j] = pdblAtten[j*dwSampleCount + nGHzIdx];
			}
		}// end for loop

		 //save the pixel file
		for (int loop = 0; loop < nLine; loop++)//atten行数
		{
			strValue.Empty();
			strValue.Format("%.2f", pdwDac[loop]);

			for (i = 0; i < PIXEL_NUMBER_64; i++)//pixel Ghz数量
			{
				//strTemp.Format("%d,%.3f", pdwDac[loop], dblPixelAtten[i][loop]);
				strTemp.Format(",%.3f", dblPixelAtten[i][loop]);
				strValue += strTemp;
			}
			fprintf(pFile, "%s\n", strValue);
		}
	}
	catch (char* ptszErrorMsg)
	{
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		CString strError("Other fatal exception occurred");
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return bFunctionOK;
}


/////////////////////////////////////////////////////////
//函数说明：
//	 扫描函数
//Add by wanxin
//Time:20200615
////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnTestScanOne()
{
	CString strMsg;
	CString strTemp;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	strTempPath.Format(".\\Data\\ScanTemp");
	CreateFullPath(strTempPath);
	m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw.csv", strTempPath);

	InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
	InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

	if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
	{
		Sleep(2000);
		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			strMsg = m_FSTPObj.GetErrorMsg();
			LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
	}
	int nPlotIndex = 0;
	for (int i = 0; i < m_nDGECount; i++)
	{
		double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
		double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i + 1];

		strTemp.Format("DGE模块%d", i + 1);
		//m_pGraph->PlotClear(0,i);
		AddPlotCount(0, strTemp);
		m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
		nPlotIndex++;
		if (m_stScanParameter.bDoPDL)
		{
			AddPlotCount(1, strTemp);
			double *pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i + 1];
			m_pGraph->PlotXYS(1, pdbWLArry, pdbPDLArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
		}
	}

	return TRUE;
}


BOOL CTestTaskManager::SaveScanAttenData(LPCTSTR lpstPath, DWORD dwChannelCount, float fData)
{
	BOOL         bFunctionOK = TRUE;
	double       *pdblWaveArray = NULL;
	double       *pdblPMArray = NULL;
	CString      strTemp;

	FILE*        pFile = NULL;

	try
	{

		//read the IL data
		pFile = fopen(lpstPath, "a+");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		pdblWaveArray = m_FSTPObj.m_stScanResult.pdbWLArry;

		pdblPMArray = m_FSTPObj.m_stScanResult.pdbPMResult[dwChannelCount];

		if (pdblWaveArray == NULL)
		{
			throw "The result raw data pointer is NULL";
		}

		if (pdblPMArray == NULL)
		{
			throw "The result Wave pointer is NULL";
		}

		strTemp.Format("%.2f", fData);
		fprintf(pFile, "%s", strTemp);

		for (int i = 0; i < m_stScanParameter.dwSampleCount; i++)
		{
			strTemp.Format(",%lf", pdblPMArray[i]);
			fprintf(pFile, "%s", strTemp);
		}

		fprintf(pFile, "\n", strTemp);
	}
	catch (TCHAR* ptszErrorMsg)
	{
		strTemp = ptszErrorMsg;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		strTemp.Format("出现未知错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::fnPixelAttenScan(WORD wDacSetting, CString strName, int nType)
{
	CString strTemp;
	CString strFileName;
	CString strpath;

	try
	{
		// 1. scan
		if (!fnTestScanOne())
		{
			return FALSE;
		}

		for (int i = 0; i < m_nDGECount; i++)
		{
			if (nType == 0)
			{
				strpath.Format("%s\\Calibrate\\%s", m_DGEModule[i].m_strTestPath, m_DGEModule[i].m_strTempFolder);
				strFileName.Format("%s\\%s.csv", strpath, strName);

			}
			else
			{
				strpath.Format("%s\\Test\\%s", m_DGEModule[i].m_strTestPath, m_DGEModule[i].m_strTempFolder);
				strFileName.Format("%s\\%s.csv", strpath, strName);
			}

			if (!SaveScanAttenData(strFileName, i + 1, (float)wDacSetting))
			{
				throw " save the result raw data to file error";
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//函数说明：
//   Pixel Attenuation定标
//Add by wanxin
//Time：20200615
//////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnCal_PixelAtten()
{
	CString strTemp;
	CString strValue;
	CString strFileName;
	stPixelInfo  stPixelData[MAXMODULECOUNT];
	int     nScanNumber = 0;
	WORD    wSetDac = 0;
	WORD    *pwSetDac = NULL;
	double  dblTemper = 0;
	double  dblStartWL = 0;
	double  dblStep = 0;
	int     nLine = 0;
	DWORD   dwSampleCount = m_stScanParameter.dwSampleCount;
	WORD    wSetDacArray[PIXEL_NUMBER_64] = { 0 };
	BOOL bFunctionOK = TRUE;
	CString strpath;
	CString strTaskCaption;
	CString strDataName;
	double  *pdblAtten = new double[ATTEN_LUT_POINT*dwSampleCount];
	float   *pfDac = new float[ATTEN_LUT_POINT];
	double *pdblScanGhz = new double[dwSampleCount];

	try
	{
		strTemp.Format("开始%.0fC温度Pixel Atten 定标扫描测试......", m_stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//if (!JudgeCurrentSlotSN(nIndex))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			if (!ReadProductionInfo(nIndex))
			{
				strTemp.Format("DGE模块%d读取产品测试信息出现错误", nIndex + 1);
				throw strTemp.GetBuffer(0);
			}

			if (!m_DGEModule[nIndex].fnReadAttenCaliCfg())
			{
				strTemp.Format("%s", m_DGEModule[nIndex].m_strErrorMsg);
				throw strTemp.GetBuffer(0);
			}
			strpath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strpath);

			if (!m_DGEModule[nIndex].CopyDefaultCenterWLTemplate())
			{
				strTemp.Format("%s", m_DGEModule[nIndex].m_strErrorMsg);
				throw strTemp.GetBuffer(0);
			}

			if (!m_DGEModule[nIndex].fnSaveAttenCalScanData())
			{
				strTemp = m_DGEModule[nIndex].m_strErrorMsg;
				throw strTemp.GetBuffer(0);
			}
			nScanNumber = m_DGEModule[nIndex].m_nAttenCfgNumber;
		}

		for (int nloop = 0; nloop < nScanNumber; nloop++)
		{
			for (int nModule = 0; nModule < m_nDGECount; nModule++)
			{
				//Set all pixels to open
				wSetDac = m_DGEModule[nModule].m_stPixelDac.iSetDac[nloop];
				strTemp.Format("模块%d第%d次设置Pixel所有通道: DAC = %d", nModule + 1, nloop + 1, wSetDac);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nModule);
				for (int k = 0; k < PIXEL_NUMBER_64; k++)
				{
					wSetDacArray[k] = wSetDac;
				}
				pwSetDac = wSetDacArray;

				if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac))
				{
					OpenSerialPortDevice();
					Sleep(1000);
					if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac))
					{
						strTemp = m_DGEModule[nModule].m_strErrorMsg;
						throw strTemp.GetBuffer(0);
					}
				}
			}

			fnSleepLongTime(2500);

			if (!fnPixelAttenScan(wSetDac, "Pixel_Atten_Scan_1", 0))
			{
				strTemp.Format("Pixel atten scan Error");
				throw strTemp.GetBuffer(0);
			}
		}

		for (nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			strpath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			if (!fnGetTemperWriteIni(nIndex, dblTemper, "ATTEN", TRUE))
			{
				strTemp.Format("保存模块%dAttenu LC定标温度存在错误!");
				throw strTemp.GetBuffer(0);
			}

			strFileName.Format("%s\\Pixel_Atten_Scan_1.csv", strpath);
			//5. 读取64个中心波长的衰减定标值
			memset(pdblAtten, 0, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			memset(pfDac, 0, ATTEN_LUT_POINT * sizeof(float));
			memset(pdblScanGhz, 0, dwSampleCount * sizeof(double));
			if (!fnReadAttenVsGhz(strFileName, dwSampleCount, pfDac, pdblAtten, &nLine, pdblScanGhz))
			{
				strTemp.Format("读取中心波长衰减定标值失败!");
				throw strTemp.GetBuffer(0);
			}
			//Get the 64 pieces of pixel center wavelength
			strFileName.Format("%s\\PixelWave.ini", strpath);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nIndex);

			if (!fnReadPixelWaveIni(strFileName, &stPixelData[nIndex], FALSE)) //全部中心波长
			{
				strTemp.Format("Failed at reading the pixel wavelength ini file!");
				throw	strTemp.GetBuffer(0);
			}

			//保存64个pixel的定标衰减值
			strFileName.Format("%s\\Pixel_Atten_1_Cal.csv", strpath);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nIndex);

			if (!SaveCaliPixelAtten(strFileName, nLine, &stPixelData[nIndex].dblPixelGhz[0], pfDac, pdblScanGhz, pdblAtten))
			{
				strTemp.Format("Error on saving cal attenuation of 64 pixels!");
				throw strTemp.GetBuffer(0);
			}
			Sleep(1000);
			strTemp.Format("%sC温度下Pixel衰减定标完成", m_DGEModule[nIndex].m_strTempFolder);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nIndex);
		}
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	//delete pointor
	if (pdblAtten != NULL)
	{
		delete[] pdblAtten;
		pdblAtten = NULL;
	}
	if (pdblScanGhz != NULL)
	{
		delete[] pdblScanGhz;
		pdblScanGhz = NULL;
	}
	if (pfDac != NULL)
	{
		delete[] pfDac;
		pfDac = NULL;
	}

	return bFunctionOK;
}





BOOL CTestTaskManager::SaveScanILData(LPCTSTR lpstPath, DWORD dwChannelCount, DWORD dwSampleCount)
{
	BOOL         bFunctionOK = TRUE;
	double       *pdblWaveArray = NULL;
	double       *pdblPMArray = NULL;
	double       *pdblPDLArray = NULL;

	CString      strTemp;
	double       dblFrqTemp = 0;
	double       dblWaveTemp = 0;
	FILE*        pFile = NULL;


	try
	{

		//read the IL data
		pFile = fopen(lpstPath, "wt");
		if (NULL == pFile)
		{
			strTemp.Format("Open file: %s error", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		pdblPMArray = m_FSTPObj.m_stScanResult.pdbPMResult[dwChannelCount];
		pdblWaveArray = m_FSTPObj.m_stScanResult.pdbWLArry;


		if (pdblPMArray == NULL)
		{
			throw "The result raw data pointer is NULL";
		}

		if (pdblWaveArray == NULL)
		{
			throw "The result Wave pointer is NULL";
		}

		if (m_stScanParameter.bDoPDL)
		{
			pdblPDLArray = m_FSTPObj.m_stScanResult.pdbPDLResult[dwChannelCount];
			if (pdblPDLArray == NULL)
			{
				throw "The result Wave pointer is NULL";
			}
			strTemp.Format("WL,Ghz,IL,PDL\n");
			fprintf(pFile, "%s", strTemp);

			for (int i = 0; i < dwSampleCount; i++)
			{
				dblWaveTemp = pdblWaveArray[i];
				dblFrqTemp = VELOCITY_LIGHT / dblWaveTemp;
				strTemp.Format("%lf,%lf,%lf,%lf", dblWaveTemp, dblFrqTemp, pdblPMArray[i], pdblPDLArray[i]);
				fprintf(pFile, "%s\n", strTemp);
			}
		}
		else
		{
			strTemp.Format("WL,Ghz,IL\n");
			fprintf(pFile, "%s", strTemp);

			for (int i = 0; i < dwSampleCount; i++)
			{
				dblWaveTemp = pdblWaveArray[i];
				dblFrqTemp = VELOCITY_LIGHT / dblWaveTemp;
				strTemp.Format("%lf,%lf,%1f", dblWaveTemp, dblFrqTemp, pdblPMArray[i]);
				fprintf(pFile, "%s\n", strTemp);
			}
		}
	}
	catch (TCHAR* ptszErrorMsg)
	{
		CString strError = ptszErrorMsg;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		//		AfxMessageBox("Other fatal exception occurred",  MB_OK | MB_ICONERROR);
		CString strError("Other fatal exception occurred");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::fnSavePixelWaveIni(LPCTSTR lpstPath, stPixelInfo stPixel_info, int nMoudleIndex)
{
	CString strTemp;
	CString strSection;
	CString strKey;
	CString strValue;
	int     nIndexOffset = 0;
	double  dblWave = 0;
	double  dblGhz = 0;
	double  dblIL = 0.0;
	int     nloop = 0;

	try
	{
		nIndexOffset = stPixel_info.nLoop;
		nloop = stPixel_info.nLoop;
		for (int i = 0; i < stPixel_info.nPixelWavNum; i++)
		{
			if (nloop == 6 || nloop == 7) //Add by wanxin，这里根据配置模板，第6、7组的63、64 Pixel不保存，通过外插计算得到
			{
				//wavelength ~nm
				dblWave = stPixel_info.dblPixelWav[i];
				strValue.Format("%.3f", dblWave);
				strSection.Format("CENTER_WAVE");
				strKey.Format("WAVE_%d", m_DGEModule[nMoudleIndex].m_nPixelMatrix[nloop][i + 1]);
				m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);

				//wavelength ~Ghz
				dblGhz = stPixel_info.dblPixelGhz[i];
				strValue.Format("%.3f", dblGhz);
				strKey.Format("GHZ_%d", m_DGEModule[nMoudleIndex].m_nPixelMatrix[nloop][i + 1]);
				m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);

				//IL
				dblIL = stPixel_info.dblPixelIL[i];
				strValue.Format("%.3f", dblIL);
				//strKey.Format("GHZ_%d", nIndexOffset+PIXEL_SET_INTERVAL*i);
				strKey.Format("IL_%d", m_DGEModule[nMoudleIndex].m_nPixelMatrix[nloop][i + 1]); //by luke.liu 2018.2.13
				m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);
			}
			else
			{
				//wavelength ~nm
				dblWave = stPixel_info.dblPixelWav[i];
				strValue.Format("%.3f", dblWave);
				strSection.Format("CENTER_WAVE");

				strKey.Format("WAVE_%d", m_DGEModule[nMoudleIndex].m_nPixelMatrix[nloop][i]);
				m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);

				//wavelength ~Ghz
				dblGhz = stPixel_info.dblPixelGhz[i];
				strValue.Format("%.3f", dblGhz);

				strKey.Format("GHZ_%d", m_DGEModule[nMoudleIndex].m_nPixelMatrix[nloop][i]);
				m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);
				//IL
				dblIL = stPixel_info.dblPixelIL[i];
				strValue.Format("%.3f", dblIL);

				strKey.Format("IL_%d", m_DGEModule[nMoudleIndex].m_nPixelMatrix[nloop][i]);
				m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}







/************************************************************************/
/*
//Funtion: 查找每个Pexel的中心波长： 最小插损波长为中心各取左右3dB的波长的平均值                                                                */
/************************************************************************/
//BOOL CTestTaskManager::SearchPixel_New(LPCTSTR lpstPath, double *pdblPixelWave, double *pdblPixelGhz, double *pdblGetIL, int* PixelCount)
//{
//	CString strTemp;
//	CStdioFile stdFile;
//	char    chRead[256];
//	CString strToken;
//	double dblGetWL = 0;
//	double dblGetGhz = 0;
//	double dblGetIL = 0;
//	double dblGetGhzBuf[SCAN_WAVE_COUNT];
//	double dblGetILBuf[SCAN_WAVE_COUNT];
//	double dblGetWaveBuf[SCAN_WAVE_COUNT];
//	double dblPixelWave_Peak[PIXEL_NUMBER_64];
//	double dblPixelGhz_Peak[PIXEL_NUMBER_64];
//	double dblPixelIL_Peak[PIXEL_NUMBER_64];
//	double dblPixelWave_Center[PIXEL_NUMBER_64];
//	double dblPixelGhz_Center[PIXEL_NUMBER_64];
//	double dblPixelIL_Center[PIXEL_NUMBER_64];
//	int    nPixelIndex_Peak[PIXEL_NUMBER_64];
//
//	int nIndex = 0, nIndex2 = 0, nIndex3 = 0;
//	int nIndex_MinIL = 0;
//
//	BOOL bSearchBegin = FALSE;
//	double dblBasePoint = 0; //
//	double dblBasePointOffet = 10;
//	double dblMinIL = 0;
//	BOOL   bDirectDown = TRUE;
//	double dblDeltaIL = 0;
//	double dblPeakIL = 0;
//	int    nIndex_Pixel = 0;
//	int    nIndex_PeakIL = 0;
//	int    nPixelCount = 0;
//	double dblLeftPixelWav = 0;
//	double dblRightPixelWav = 0;
//	double dblLeftPixelIL = 0;
//	double dblRightPixelIL = 0;
//	double dblLeftPixelGhz = 0;
//	double dblRightPixelGhz = 0;
//	int    nIndexOffset = 0;
//	double dblTarGetOffset = 2.0; //3dB
//
//	PDWORD     pdwDataArray = NULL;
//
//	int   nChannelNum = m_stScanParameter.nPMCount;
//	int   nSumCount = m_stScanParameter.dwSampleCount;
//
//	try
//	{
//
//		if (!stdFile.Open(lpstPath, CFile::modeRead))
//		{
//			strTemp.Format("open file error:%s", lpstPath);
//			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
//			return FALSE;
//		}
//		ZeroMemory(chRead, sizeof(chRead));
//		for (nIndex = 0; nIndex < 1; nIndex++)
//		{
//			stdFile.ReadString(chRead, 256);
//		}
//		// ********** Seek Data Trends **********
//		ZeroMemory(&dblGetGhzBuf, sizeof(dblGetGhzBuf));
//		ZeroMemory(&dblGetILBuf, sizeof(dblGetILBuf));
//		ZeroMemory(&dblGetWaveBuf, sizeof(dblGetWaveBuf));
//
//		ZeroMemory(&dblPixelWave_Peak, sizeof(dblPixelWave_Peak));
//		ZeroMemory(&dblPixelIL_Peak, sizeof(dblPixelIL_Peak));
//		ZeroMemory(&dblPixelGhz_Peak, sizeof(dblPixelGhz_Peak));
//		ZeroMemory(&nPixelIndex_Peak, sizeof(nPixelIndex_Peak));
//
//		ZeroMemory(&dblPixelWave_Center, sizeof(dblPixelWave_Peak));
//		ZeroMemory(&dblPixelIL_Center, sizeof(dblPixelIL_Peak));
//		ZeroMemory(&dblPixelGhz_Center, sizeof(dblPixelGhz_Peak));
//
//		for (nIndex = 0; nIndex < nSumCount; nIndex++)
//		{
//			stdFile.ReadString(chRead, 256);
//			strToken = strtok(chRead, ",\t");
//			dblGetWL = atof(strToken);
//			strToken = strtok(NULL, ",\t");
//			dblGetGhz = atof(strToken);
//			strToken = strtok(NULL, ",\t");
//			dblGetIL = atof(strToken);
//
//			dblGetWaveBuf[nIndex] = dblGetWL;
//			dblGetGhzBuf[nIndex] = dblGetGhz;
//			dblGetILBuf[nIndex] = dblGetIL;
//		}
//
//		//close file
//		stdFile.Close();
//
//		double dblTempData = 0;
//		double dblMinIL = 999;
//		int  nMinILIndex = -1;
//		double dblMaxIL = -999;
//		int  nMaxILIndex = -1;
//		BOOL   bFindBasePoint = FALSE;
//		BOOL   bFindFangxiang = FALSE;
//		nIndex2 = 0; //MinIL Peak后面的index
//		nPixelCount = 0;
//		double dbAllPointMinIL;
//		double  pdbMiniILBuffer[8];
//		int     pnDataIndexBuffer[8];
//		for (int iBuffer = 0; iBuffer < 8; iBuffer++)
//		{
//			pdbMiniILBuffer[iBuffer] = 999;
//			pnDataIndexBuffer[iBuffer] = 0;
//		}
//
//		double dbStrtFre = m_dbStartFre + 180000;
//		double dbEndFre = m_dbStopFre + 180000;
//
//		int nLeftIndex = -1;
//		int nRightIndex = -1;
//		double dbStartWL = VELOCITY_LIGHT / dbEndFre;
//		double dbEndWL = VELOCITY_LIGHT / dbStrtFre;
//
//		for (int nIndex = 0; nIndex < nSumCount; nIndex++)
//		{
//			if (dblGetWaveBuf[nIndex] >= dbStartWL)
//			{
//				nLeftIndex = nIndex;
//				break;
//			}
//		}
//		for (nIndex = 0; nIndex < nSumCount; nIndex++)
//		{
//			if (dblGetWaveBuf[nIndex] >= dbEndWL)
//			{
//				nRightIndex = nIndex;
//				break;
//			}
//		}
//
//		//找到扫描曲线中最小插损，以最小插损-4dB作为baseline
//		for (nIndex = nLeftIndex; nIndex < nRightIndex; nIndex++)
//		{
//			dblGetIL = dblGetILBuf[nIndex];
//			if (dblGetIL < dblMinIL)
//			{
//				dblMinIL = dblGetIL; //Find Minmum IL
//				dbAllPointMinIL = dblMinIL;
//				nMinILIndex = nIndex;
//			}
//
//			if (dblGetIL > dblMaxIL)
//			{
//				dblMaxIL = dblGetIL;
//				nMaxILIndex = nIndex;
//			}
//		}
//
//		for (int nRunIndex = 0; nRunIndex < 8; nRunIndex++)
//		{
//			if (nRunIndex == 0)
//			{
//				for (nIndex = 1000; nIndex < nSumCount - 1000; nIndex++)
//				{
//					dblGetIL = dblGetILBuf[nIndex];
//					if (dblGetIL < dblMinIL)
//					{
//						dblMinIL = dblGetIL;
//						dbAllPointMinIL = dblMinIL;
//						pdbMiniILBuffer[0] = dbAllPointMinIL;
//						pnDataIndexBuffer[0] = nIndex;
//					}
//				}
//			}
//			else
//			{
//				for (nIndex = 1000; nIndex < nSumCount - 1000; nIndex++)
//				{
//					dblGetIL = dblGetILBuf[nIndex];
//					if (dblGetIL < pdbMiniILBuffer[nRunIndex] && dblGetIL > pdbMiniILBuffer[nRunIndex - 1])
//					{
//						pdbMiniILBuffer[nRunIndex] = dblGetIL;
//						pnDataIndexBuffer[nRunIndex] = nIndex;
//					}
//				}
//			}
//		}
//
//		strTemp.Format("MinILArray:%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,", pdbMiniILBuffer[0],
//			pdbMiniILBuffer[1], pdbMiniILBuffer[2], pdbMiniILBuffer[3], pdbMiniILBuffer[4], pdbMiniILBuffer[5]
//			, pdbMiniILBuffer[6], pdbMiniILBuffer[7]);
//		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
//		double dbTempMax;
//		int nTempMaxIndex;;
//		double dbTempMin;
//		int nTempMinIndex;
//		m_GernalObj.SearchMinMaxValur(pdbMiniILBuffer, 8, &dbTempMin, &nTempMinIndex, &dbTempMax, &nTempMaxIndex);
//
//		dblBasePointOffet = dbTempMin + 1.5;
//
//		dblTarGetOffset = 2.0;
//
//		dblMinIL = 999;
//		dblMaxIL = -999;
//
//		for (nIndex = 200; nIndex < nSumCount - 200; nIndex++)
//		{
//			dblGetWL = dblGetWaveBuf[nIndex];
//			dblGetGhz = dblGetGhzBuf[nIndex];
//			dblGetIL = dblGetILBuf[nIndex];
//
//			//if((dblGetIL - dblMinIL) < 0.0001)
//			if (dblGetIL < dblMinIL)
//			{
//				dblMinIL = dblGetIL; //Find Minmum IL
//				nIndex2 = 0; //重新开始
//				nIndex_MinIL = nIndex; //by luke.liu 2018.2.14
//			}
//
//			//by luke.liu 2018.2.7
//			if (!bFindBasePoint)
//			{
//				if (fabs(dblGetIL - dblBasePointOffet) < 0.2) //base line
//				{
//					bFindBasePoint = TRUE;
//					dblBasePoint = dblGetIL;
//					dblMinIL = 999; //找到门限值后再找最小IL点
//				}
//				else
//				{
//					continue;
//				}
//			}
//			if (!bFindFangxiang) //Modify by wanxin
//			{
//				//判断方向
//				//比base line IL小3dB
//				dblDeltaIL = dblBasePoint - dblGetIL;
//				//如果插损变小，且差异大于10dB时开始查找MinIL
//				if (dblDeltaIL > dblTarGetOffset)//3.0dB, 因抖动取消方向判断
//				{
//					bSearchBegin = TRUE; //Flag for every pixel search beginning
//					bFindFangxiang = TRUE;
//				}
//				else
//				{
//					bSearchBegin = FALSE;
//					nIndex2 = 0; //重新开始
//				}
//			}
//
//			if (bSearchBegin) //开始查找，从第一个Pixel开始
//			{
//				if (dblGetIL > dblMinIL)
//				{
//					nIndex2++; //找IL的拐点
//				}
//				else
//				{
//					nIndex2 = 0; //重新开始
//				}
//
//				if (nIndex2 > 10 && (dblGetIL - dblMinIL > 3.0))
//				{
//					////////////////
//					dblPixelWave_Peak[nPixelCount] = dblGetWaveBuf[nIndex - nIndex2];
//					dblPixelGhz_Peak[nPixelCount] = dblGetGhzBuf[nIndex - nIndex2];
//					dblPixelIL_Peak[nPixelCount] = dblGetILBuf[nIndex - nIndex2];
//					dblPeakIL = dblGetILBuf[nIndex - nIndex2];
//					nIndex_Pixel = nIndex - nIndex2;
//					nPixelIndex_Peak[nPixelCount] = nIndex_Pixel;
//					dblMinIL = 999;
//					dblMaxIL = -999;
//					nIndex2 = 0;
//					bFindFangxiang = FALSE;
//
//					strTemp.Format("第%d个Pixel Peak: Index=%d; wave=%.3f; IL=%.3f, Ghz=%.3f",
//						nPixelCount + 1,
//						nIndex_Pixel,
//						dblPixelWave_Peak[nPixelCount],
//						dblPeakIL,
//						dblPixelGhz_Peak[nPixelCount]);
//					LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
//				}
//				else
//				{
//					continue; //继续下一个点
//				}
//
//				//找到PeakIL点以后，开始+/-3dB查找中心波长
//				for (nIndex3 = nIndex_Pixel; nIndex3 >= 0; nIndex3--)
//				{
//					if (fabs(dblGetILBuf[nIndex3] - dblPeakIL) > 3.0)
//					{
//						dblLeftPixelWav = dblGetWaveBuf[nIndex3];
//						dblLeftPixelIL = dblGetILBuf[nIndex3];
//						dblLeftPixelGhz = dblGetGhzBuf[nIndex3];
//
//						break;
//					}
//				}
//
//				for (nIndex3 = nIndex_Pixel; nIndex3 < nSumCount; nIndex3++)
//				{
//					if (fabs(dblGetILBuf[nIndex3] - dblPeakIL) > 3.0)
//					{
//						dblRightPixelWav = dblGetWaveBuf[nIndex3];
//						dblRightPixelIL = dblGetILBuf[nIndex3];
//						dblRightPixelGhz = dblGetGhzBuf[nIndex3];
//
//						break;
//					}
//				}
//
//				//Average pixel center wavelength
//				dblPixelWave_Center[nPixelCount] = (dblLeftPixelWav + dblRightPixelWav) / 2;
//
//
//				if ((dblPixelWave_Center[nPixelCount] - dblPixelWave_Peak[nPixelCount]) > 0.005)
//				{
//					nIndexOffset = (dblPixelWave_Center[nPixelCount] - dblPixelWave_Peak[nPixelCount]) / 0.005;
//					dblPixelIL_Center[nPixelCount] = dblGetILBuf[nIndex_Pixel + nIndexOffset];//IL
//					dblPixelGhz_Center[nPixelCount] = dblGetGhzBuf[nIndex_Pixel + nIndexOffset];//GHZ
//				}
//				else if ((dblPixelWave_Center[nPixelCount] - dblPixelWave_Peak[nPixelCount]) < -0.005)
//				{
//					nIndexOffset = (dblPixelWave_Peak[nPixelCount] - dblPixelWave_Center[nPixelCount]) / 0.005;
//					dblPixelIL_Center[nPixelCount] = dblGetILBuf[nIndex_Pixel - nIndexOffset];//IL
//					dblPixelGhz_Center[nPixelCount] = dblGetGhzBuf[nIndex_Pixel - nIndexOffset];//GHZ
//				}
//				else
//				{
//					dblPixelIL_Center[nPixelCount] = dblGetILBuf[nIndex_Pixel];//IL
//					dblPixelGhz_Center[nPixelCount] = dblGetGhzBuf[nIndex_Pixel];//GHZ
//				}
//
//
//				nPixelCount++;
//
//				strTemp.Format("Calculated第%d个Pixel Center: wave=%.3f; IL=%.3f, Ghz=%.3f",
//					nPixelCount + 1,
//					dblPixelWave_Center[nPixelCount],
//					dblPixelIL_Center[nPixelCount],
//					dblPixelGhz_Center[nPixelCount]);
//
//
//				bSearchBegin = FALSE;
//				bFindBasePoint = FALSE;
//			}
//		}
//
//		memcpy(pdblGetIL, dblPixelIL_Center, sizeof(double)*nPixelCount);
//		memcpy(pdblPixelGhz, dblPixelGhz_Center, sizeof(double)*nPixelCount);
//		memcpy(pdblPixelWave, dblPixelWave_Center, sizeof(double)*nPixelCount);
//
//		*PixelCount = nPixelCount;
//	}
//	catch (char*err)
//	{
//		strTemp = err;
//		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
//		return FALSE;
//	}
//
//	return TRUE;
//}


BOOL CTestTaskManager::fnPixelCenterWaveScan(int nLoop)
{
	CString strTemp;
	CString strFileName;
	double *PixelGhz = NULL;
	double *PixelIL = NULL;
	double *PixelWave = NULL;
	stPixelInfo  stPixel_info;
	int      nCount = 0;
	CString strPath;
	try
	{
		ZeroMemory(&stPixel_info, sizeof(stPixel_info));

		if (!fnTestScanOne())
			return FALSE;

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			strFileName.Format("%s\\Pixel_Wave_Scan_loop%d.csv", strPath, nLoop + 1);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!SaveScanILData(strFileName, nIndex + 1, m_stScanParameter.dwSampleCount))
			{
				throw "Save the result raw data to file error";
			}

			//保存扫描后的pixel中心波长
			PixelGhz = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_96 * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			PixelIL = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_96 * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			PixelWave = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_96 * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			ZeroMemory(PixelGhz, PIXEL_NUMBER_96 * sizeof(double));
			ZeroMemory(PixelIL, PIXEL_NUMBER_96 * sizeof(double));
			ZeroMemory(PixelWave, PIXEL_NUMBER_96 * sizeof(double));

			if (!m_DGEModule[nIndex].SearchPixel_New2(PixelWave, PixelGhz, PixelIL, &nCount, nLoop))
			{
				throw "查找Pixel中心波长错误!";
			}

			if (nLoop == 0 || nLoop == 1 || nLoop == 6 || nLoop == 7)
			{

			}
			else
			{
				if ((PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL) != nCount) //查找的Pixel中心波长是否和打开pixel数量一致
				{
					strTemp.Format("第%d次扫描中心波长数量: %d, 与打开pixel数量: %d不一致!", nLoop + 1, nCount, PIXEL_SET_INTERVAL);
					throw strTemp.GetBuffer(0);
				}
			}

			strTemp.Format("第%d次扫描中心波长数量: %d", nLoop + 1, nCount);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

			stPixel_info.nLoop = nLoop;
			stPixel_info.nPixelWavNum = nCount;
			memcpy(&stPixel_info.dblPixelWav, PixelWave, sizeof(double)*nCount);
			memcpy(&stPixel_info.dblPixelGhz, PixelGhz, sizeof(double)*nCount);
			memcpy(&stPixel_info.dblPixelIL, PixelIL, sizeof(double)*nCount);

			strFileName.Format("%s\\PixelWave.ini", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			if (!fnSavePixelWaveIni(strFileName, stPixel_info, nIndex))
			{
				strTemp.Format("Save center wave ini Error");
				throw strTemp.GetBuffer(0);
			}

			for (int i = 0; i < nCount; i++)
			{
				m_DGEModule[nIndex].m_stPixelInfo.dblPixelWav[m_DGEModule[nIndex].m_nPixelMatrix[nLoop][i]] = stPixel_info.dblPixelWav[i];
				m_DGEModule[nIndex].m_stPixelInfo.dblPixelGhz[m_DGEModule[nIndex].m_nPixelMatrix[nLoop][i]] = stPixel_info.dblPixelGhz[i];
				m_DGEModule[nIndex].m_stPixelInfo.dblPixelIL[m_DGEModule[nIndex].m_nPixelMatrix[nLoop][i]] = stPixel_info.dblPixelIL[i];
			}

			m_DGEModule[nIndex].m_stPixelInfo.nPixelWavNum += nCount;
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	if (PixelGhz != NULL)
	{
		VirtualFree(PixelGhz, 0, MEM_RELEASE);
		PixelGhz = NULL;
	}

	if (PixelIL != NULL)
	{
		VirtualFree(PixelIL, 0, MEM_RELEASE);
		PixelIL = NULL;
	}

	if (PixelWave != NULL)
	{
		VirtualFree(PixelWave, 0, MEM_RELEASE);
		PixelWave = NULL;
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    Save 64 pixel中心波长函数
//Add by wanxin
//Time:20181016
//////////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnSavePixelWaveIniNew(LPCTSTR lpstPath, stPixelInfo stPixel_info)
{
	CString strTemp;
	CString strSection;
	CString strKey;
	CString strValue;
	int     nIndexOffset = 0;
	double  dblWave = 0;
	double  dblGhz = 0;
	double  dblIL = 0.0;

	try
	{
		//save the pixel wav to ini file
		for (int i = 0; i < stPixel_info.nPixelWavNum; i++)
		{
			//wavelength ~nm
			dblWave = stPixel_info.dblPixelWav[i];
			strValue.Format("%.3f", dblWave);
			strSection.Format("CENTER_WAVE");

			strKey.Format("WAVE_%d", i);
			m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);

			//wavelength ~Ghz
			dblGhz = stPixel_info.dblPixelGhz[i];
			strValue.Format("%.3f", dblGhz);

			strKey.Format("GHZ_%d", i);
			m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);
			//IL
			dblIL = stPixel_info.dblPixelIL[i];
			strValue.Format("%.3f", dblIL);

			strKey.Format("IL_%d", i);
			m_GernalObj.SaveIniFile(strSection, strKey, strValue, lpstPath);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}



BOOL CTestTaskManager::fnSavePixelWaveCsvFile(LPCTSTR lpstPath, stPixelInfo stPixel_info)
{
	CString strTemp;
	CString strSection;
	CString strKey;
	CString strValue;
	int     nIndexOffset = 0;
	double  dblWave = 0;
	double  dblGhz = 0;
	FILE    *pFile = NULL;
	BOOL bFunctionOK = TRUE;

	try
	{
		//第几次扫描, e.g. 第一次：1,9,17,25,33,41,49,57

		pFile = fopen(lpstPath, "wt");
		if (INVALID_HANDLE_VALUE == pFile)
		{
			strTemp.Format("Open file err: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		strTemp.Format("WL,Ghz,IL\n");
		fprintf(pFile, "%s", strTemp);

		//save the pixel wav to ini file
		for (int i = 0; i < stPixel_info.nPixelWavNum; i++)
		{
			//wavelength ~nm
			strTemp.Format("%.3f,%.3f,%.3f\n", stPixel_info.dblPixelWav[i], stPixel_info.dblPixelGhz[i], stPixel_info.dblPixelIL[i]);
			fprintf(pFile, "%s", strTemp);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (NULL != pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return TRUE;
}


////////////////////////////////////////////////////////////////////
//函数说明：
//   精细atten定标后重新进行中心波长定标
//Add by wanxin
//Time：20200617
///////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnCalFined_PixelCenterWave()
{
	CString strTemp;
	CString strMsg;
	CString strFileName;
	int     i = 0;
	int     nGroupNum = 0;
	stPixelOpen  stPixel[MAXMODULECOUNT];
	double  dblTemper = 0;
	double   *pdblDac = NULL;
	int     nPixelCount = 0;
	ZeroMemory(&stPixel, sizeof(stPixel));
	BOOL bFunctionOK = TRUE;
	CString strpath;
	try
	{
		strTemp.Format("开始%.0fC温度Pixel Center Wavelength Fine 定标扫描测试......", m_stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		for (int i = 0; i < m_nDGECount; i++)
		{
			//if (!JudgeCurrentSlotSN(i))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", i + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			if (!ReadProductionInfo(i))
			{
				strTemp.Format("获取%d模块产品信息存在错误", i + 1);
				throw strTemp.GetBuffer(0);
			}
		}

		pdblDac = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*ATTEN_LUT_POINT * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(pdblDac, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);

		nGroupNum = PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL;
		for (int nGPIndex = 0; nGPIndex < nGroupNum; nGPIndex++) //about 8 times to scan 
		{
			for (int nModule = 0; nModule < m_nDGECount; nModule++)
			{
				strpath.Format("%s\\Calibrate\\%s", m_DGEModule[nModule].m_strCaliFolder, m_DGEModule[nModule].m_strTempFolder);
				CreateFullPath(strpath);
				if (!m_DGEModule[nModule].fnReadPixelMatrixCfg())
				{
					strTemp = m_DGEModule[nModule].m_strErrorMsg;
					throw strTemp.GetBuffer(0);
				}

				if (!m_DGEModule[nModule].fnReadAttenLut256Csv(pdblDac, &nPixelCount))
				{
					strTemp = m_DGEModule[nModule].m_strErrorMsg;
					throw strTemp.GetBuffer(0);
				}

				for (int i = 0; i < nPixelCount; i++)
				{
					stPixel[nModule].iOpenDac[i] = (int)(pdblDac[i*ATTEN_LUT_POINT]);
					stPixel[nModule].iBlockDac[i] = (int)(pdblDac[i*ATTEN_LUT_POINT + ATTEN_LUT_POINT - 1]);
				}
				strTemp.Format("第%d次中心波长扫描......", nGPIndex + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

				//Set the desired pixel to open and others for block

				if (!m_DGEModule[nModule].fnSetPixelGroupDAC(nGPIndex, &stPixel[nModule]))
				{
					OpenSerialPortDevice();
					Sleep(1000);
					if (!m_DGEModule[nModule].fnSetPixelGroupDAC(nGPIndex, &stPixel[nModule]))
					{
						strTemp = m_DGEModule[nModule].m_strErrorMsg;
						throw strTemp.GetBuffer(0);
					}
				}
			}

			fnSleepLongTime(2500);

			if (!fnPixelCenterWaveScan(nGPIndex))
			{
				strTemp.Format("Pixel wave scan Error");
				throw strTemp.GetBuffer(0);
			}
		}

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			strpath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			if (!fnGetTemperWriteIni(nIndex, dblTemper, "WAVE", TRUE))
			{
				strTemp.Format("读取LC温度错误!");
				throw strTemp.GetBuffer(0);
			}

			strFileName.Format("%s\\PixelWave.ini", strpath);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

			if (!fnReadPixelWaveIni(strFileName, &m_DGEModule[nIndex].m_stPixelInfo, TRUE))
			{
				strTemp.Format("Failed at reading the pixel wavelength ini file!");
				throw	strTemp.GetBuffer(0);
			}
			m_DGEModule[nIndex].m_stPixelInfo.nPixelWavNum = PIXEL_NUMBER_64;
			//由于1、2、63、64 Pixel没有找中心波长，这里需要用3、4外插
			//61、62外插
			if (!m_DGEModule[nIndex].fnCalLineCenterWL(&m_DGEModule[nIndex].m_stPixelInfo))
			{
				strTemp.Format("Failed calculate center wavlenght!");
				throw	strTemp.GetBuffer(0);
			}
			m_DGEModule[nIndex].m_stPixelInfo.nPixelWavNum = PIXEL_NUMBER_64;

			if (!fnSavePixelWaveIniNew(strFileName, m_DGEModule[nIndex].m_stPixelInfo))
			{
				strTemp.Format("Failed in save the pixel wavelength ini file!");
				throw	strTemp.GetBuffer(0);
			}

			//added wavelength for extra csv file for data review 
			strFileName.Format("%s\\PixelWave.Csv", strpath);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

			if (!fnSavePixelWaveCsvFile(strFileName, m_DGEModule[nIndex].m_stPixelInfo))
			{
				strTemp.Format("Save center wave Csv Error");
				throw strTemp.GetBuffer(0);
			}
		}

		strTemp.Format("Pixel center wavelength calibration finished !");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	if (pdblDac != NULL)
	{
		VirtualFree(pdblDac, 0, MEM_RELEASE);
		pdblDac = NULL;
	}

	return bFunctionOK;
}

double CTestTaskManager::fnLineCalc(double x1, double x2, double y1, double y2, double XX)
{
	double YY = 0;

	if (fabs(x1 - x2) <= 0.1) //0.00001: 两个点DAC很近的时候，拟合错误
	{
		YY = (y1 + y2) / 2;
		return YY;
	}
	else if (fabs(x1 - XX) <= 0.01)//2018.3.4
	{
		return y1;//不拟合计算直接返回
	}
	else if (fabs(x2 - XX) <= 0.01)//2018.3.4
	{
		return y2;//不拟合计算直接返回
	}

	double K = (y1 - y2) / (x1 - x2);

	double C = y1 - K * x1;

	YY = K * XX + C;

	return YY;
}

BOOL CTestTaskManager::fnCalcAttenVsDac(int nModuleIndex, double *pdblPixelGhz, double *pdblScanGHz, double *pdblAtten, float *pdwDac, int nLine)
{
	CString strTemp;
	CString strFileName;

	CString strPixelFileName;
	CString strPixSection;
	CString strPixKey;
	CString strData[1024];
	double   dblSaveGray[256] = { 0 };

	FILE *pfCsvFile = NULL;
	DWORD dwScanCount = m_stScanParameter.dwSampleCount;
	BOOL  bHasWP = FALSE;
	double *pdblDac = NULL;
	double *pdblAttnLastPixel = NULL;
	BOOL bFunctionOK = TRUE;
	try
	{
		strFileName.Format("%s\\Calibrate\\%s\\256vsLut_Raw.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		pdblDac = (double*)VirtualAlloc(NULL, nLine * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		memset(pdblDac, 0, nLine * sizeof(double));

		for (int i = 0; i < nLine; i++)
		{
			pdblDac[i] = pdwDac[i];
		}

		pdblAttnLastPixel = (double*)VirtualAlloc(NULL, nLine * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		memset(pdblAttnLastPixel, 0, nLine * sizeof(double));


		pfCsvFile = fopen(strFileName.GetBuffer(0), "w");
		if (pfCsvFile == NULL)
		{
			strTemp.Format("Open %s file error", strFileName);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}

		double dblThisGHz;//Modify  by wanxin
		for (int nPixIdx = 0; nPixIdx < PIXEL_NUMBER_64; nPixIdx++)
		{
			if (nPixIdx == 0)
			{
				dblThisGHz = pdblPixelGhz[1];
			}
			else if (nPixIdx == 63)
			{
				dblThisGHz = pdblPixelGhz[62];
			}
			else
			{
				dblThisGHz = pdblPixelGhz[nPixIdx];
			}

			int nGHzIdx = 0;

			// Search GHz index
			for (int j = 1; j < dwScanCount; j++)
			{
				if (dblThisGHz >= pdblScanGHz[0])
				{
					nGHzIdx = 0;
					break;
				}
				else if (dblThisGHz <= pdblScanGHz[dwScanCount - 1])
				{
					nGHzIdx = dwScanCount - 1;
					break;
				}
				else
				{
					if ((dblThisGHz < pdblScanGHz[j - 1]) && (dblThisGHz >= pdblScanGHz[j]))
					{
						nGHzIdx = j;
						break;
					}
				}
			}// get GHz index


			int nThisIdx;
			int nLastIdx;

			if ((nGHzIdx == 0) || (nGHzIdx == dwScanCount - 1))
			{
				nThisIdx = nGHzIdx;
				nLastIdx = nGHzIdx;
			}
			else
			{
				nThisIdx = nGHzIdx;
				//nLastIdx = nGHzIdx-1;
				nLastIdx = nGHzIdx;
			}

			// save to array
			for (j = 0; j < nLine; j++)
			{
				pdblAttnLastPixel[j] = pdblAtten[j*dwScanCount + nLastIdx];
			}


			// search Max and Min	
			int nMaxIdxLast = 0;
			int nMinIdxLast = 0;
			double dblMaxLast = -100.0;
			double dblMinLast = 100.0;

			//最大衰减index by luke.liu
			for (j = 0; j < nLine; j++)
			{
				if (pdblAttnLastPixel[j] >= dblMaxLast)
				{
					dblMaxLast = pdblAttnLastPixel[j];
					nMaxIdxLast = j;
				}
			}

			//最小衰减index, 从最大衰减index往左查找 by luke.liu
			for (int n = 0; n < nLine; n++)
			{
				if ((pdblAttnLastPixel[n] <= dblMinLast) && (pdwDac[n] > 4000))
				{
					dblMinLast = pdblAttnLastPixel[n];
					nMinIdxLast = n;
				}
			}
			//Atten定标顺序都是有大DAC到小DAC
			//因此如果最大插损的Index <最小插损的
			//表示不带拨片，否则带拨片
			if (nMaxIdxLast < nMinIdxLast) //表示不带拨片
			{
				bHasWP = FALSE;
			}
			else//表示带拨片
			{
				bHasWP = TRUE;
			}
			bHasWP = TRUE;  //统一使用带拨片，1*N再改成配置的
			double x1 = 0;
			double x2 = 0;
			double y1 = 0;
			double y2 = 0;
			double XX = 0;
			double YY = 0;

			// reference
			for (j = 0; j < nLine; j++)
			{
				pdblAttnLastPixel[j] -= dblMinLast;
			}

			dblMaxLast -= dblMinLast;
			dblMinLast = 0;
			double dblThisAttn = 0;


			//////////////////////////////////////////////////////////////////////////		
			for (int nAttn = 0; nAttn <= 255; nAttn++)
			{
				dblThisAttn = (double)nAttn / 10; //0.1dB interval

				if (0 == nAttn)
				{
					dblSaveGray[nAttn] = pdblDac[nMinIdxLast];
				}
				else if (nAttn == 255)
				{
					dblSaveGray[nAttn] = pdblDac[nMaxIdxLast];
				}
				else
				{
					//////////////////////////////////////////////////////////////////////
					int nLastArrayIdx = 1;
					// search Last array 
					if (dblThisAttn >= dblMaxLast)
					{
						dblSaveGray[nAttn] = pdblDac[nMaxIdxLast];
					}
					else
					{
						if (bHasWP)
						{
							for (int k = nMinIdxLast; k <= nMaxIdxLast - 1; k++)//有可能只有最后一点满足
							{
								if (pdblAttnLastPixel[k] >= dblThisAttn)
								{
									nLastArrayIdx = k;//原来为k, 但是nLastArrayIdx不能等于nMaxIdxLast，否则下面数组拟合溢出
									break;
								}
							}

							x1 = pdblAttnLastPixel[nLastArrayIdx - 1];
							x2 = pdblAttnLastPixel[nLastArrayIdx];//pdblAttnLastPixel[nLastArrayIdx+1] 2018.3.4
							y1 = pdblDac[nLastArrayIdx - 1];
							y2 = pdblDac[nLastArrayIdx];//pdblDac[nLastArrayIdx+1] 2018.3.4
							XX = dblThisAttn;

							if (XX > x1 && XX < x2)//这里保证是线性内插
							{
								YY = fnLineCalc(x1, x2, y1, y2, XX);
							}
							else if (XX == x1)
							{
								YY = pdblDac[nLastArrayIdx - 1];
							}
							else if (XX == x2)
							{
								YY = pdblDac[nLastArrayIdx];
							}
							else
							{
								strTemp.Format("Pixel DAC value must be inline linear calculate:x1=%.3f,x2=%.3f,cur=%.3f", x1, x2, XX);
								LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
							}

							if (YY < 0)
							{
								strTemp.Format("Pixel DAC value can't be negative: %.2f", YY);
								LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
								//throw strTemp.GetBuffer(0);
							}

							dblSaveGray[nAttn] = YY;

						}
						else //不带拨片
						{
							for (int k = nMinIdxLast; k >= nMaxIdxLast; k--)//有可能只有最后一点满足
							{
								if (pdblAttnLastPixel[k] >= dblThisAttn)
								{
									nLastArrayIdx = k;//原来为k,但是nLastArrayIdx不能等于nMaxIdxLast，否则下面数组拟合溢出
									break;
								}
							}

							x1 = pdblAttnLastPixel[nLastArrayIdx];
							x2 = pdblAttnLastPixel[nLastArrayIdx + 1];
							y1 = pdblDac[nLastArrayIdx];
							y2 = pdblDac[nLastArrayIdx + 1];
							XX = dblThisAttn;

							if (XX > x1 && XX < x2)//这里保证是线性内插
							{
								YY = fnLineCalc(x1, x2, y1, y2, XX);
							}
							else if (XX == x1)
							{
								YY = pdblDac[nLastArrayIdx];
							}
							else if (XX == x2)
							{
								YY = pdblDac[nLastArrayIdx + 1];
							}
							else
							{
								strTemp.Format("Pixel DAC value must be inline linear calculate:x1=%.3f,x2=%.3f,cur=%.3f", x1, x2, XX);
								LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
							}

							if (YY < 0)
							{
								strTemp.Format("Pixel DAC value can't be negative: %.2f", YY);
								LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
							}

							dblSaveGray[nAttn] = YY;
						}
					}
				}
			}

			strData[nPixIdx].Empty();
			for (int k = 0; k <= 255; k++)
			{
				if (k < 255)
				{
					strTemp.Format("%.3f,", dblSaveGray[k]);
					strData[nPixIdx] += strTemp;
				}
				else
				{
					strTemp.Format("%.3f\n", dblSaveGray[k]);
					strData[nPixIdx] += strTemp;
				}
			}
		}
		for (nPixIdx = 0; nPixIdx < PIXEL_NUMBER_64; nPixIdx++)
		{
			fprintf(pfCsvFile, "%s", strData[nPixIdx]);
			//stdFile.WriteString(strData[nPixIdx]);
		}

		strTemp.Format("Calculate Pixel Atten vs DAC finished ! ");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pfCsvFile != NULL)
	{
		fclose(pfCsvFile);
		pfCsvFile = NULL;
	}
	if (pdblDac != NULL)
	{
		VirtualFree(pdblDac, 0, MEM_RELEASE);
		pdblDac = NULL;
	}

	if (pdblAttnLastPixel != NULL)
	{
		VirtualFree(pdblAttnLastPixel, 0, MEM_RELEASE);
		pdblAttnLastPixel = NULL;
	}

	return bFunctionOK;
}

///////////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    保存计算的衰减Lut表数据
//Add by wanxin
//Time:20181019
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::SaveAttenVsDac(LPCSTR lpstrFile, double *pdbDAC, int nPixelCount)
{
	FILE *pfCsvFile = NULL;
	char szBuffer[MAX_PATH];
	CString strTemp;
	pfCsvFile = fopen(lpstrFile, "w");
	if (pfCsvFile == NULL)
	{
		strTemp.Format("Save %s file error!", lpstrFile);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	for (int j = 0; j < nPixelCount; j++)
	{
		for (int k = 0; k < ATTEN_LUT_POINT; k++)
		{
			ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);
			if (k < 255)
			{
				sprintf(szBuffer, "%.2f", pdbDAC[k + j*ATTEN_LUT_POINT]);
				fprintf(pfCsvFile, "%s,", szBuffer);
			}
			else
			{
				sprintf(szBuffer, "%.2f", pdbDAC[k + j*ATTEN_LUT_POINT]);
				fprintf(pfCsvFile, "%s\n", szBuffer);
			}
		}
	}
	if (pfCsvFile != NULL)
	{
		fclose(pfCsvFile);
		pfCsvFile = NULL;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    重新计算Atten平均Lut表，采用fitting二次曲线后计算出DAC
//Add by wanxin
//Time"20181019
/////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnCalcAttenVsDac_AveByPolynomial(int nModuleIndex)
{
	CString strFileName;
	BOOL bFuntionOK = TRUE;
	DWORD dwScanCount = m_stScanParameter.dwSampleCount;
	double * pdblDac = NULL;
	double *pdbAtten = NULL;
	int nPixelCount;
	double pdbAttenArry[256];
	double pdblDacTemp[256];
	double dbPixeArry[PIXEL_NUMBER_64];
	double pdblCoefficent[10];
	double ax;
	double bx;
	double cx;
	double dbCalValue;
	CString strTemp;

	try
	{
		pdblDac = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*ATTEN_LUT_POINT * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		memset(pdblDac, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);

		//1. Get DAC of mimum IL from attenuation calibration test
		strFileName.Format("%s\\Calibrate\\%s\\256vsLut_Raw.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder);
		if (!m_DGEModule[nModuleIndex].fnReadAttenLut256Csv(strFileName, pdblDac, &nPixelCount))
		{
			strTemp.Format("Get %sC Lut256Csv file error!", m_DGEModule[nModuleIndex].m_strTempFolder);
			throw strTemp.GetBuffer(0);
		}
		ZeroMemory(pdbAttenArry, sizeof(double) * 256);
		ZeroMemory(pdblDacTemp, sizeof(double) * 256);

		for (int nIndex = 0; nIndex < 256; nIndex++)
		{
			for (int nPixelIndex = 0; nPixelIndex < PIXEL_NUMBER_64; nPixelIndex++)
			{
				pdblDacTemp[nPixelIndex] = pdblDac[nIndex + (nPixelIndex)*ATTEN_LUT_POINT];
				dbPixeArry[nPixelIndex] = nPixelIndex + 1;
			}

			//二次拟合获得系数
			// AX^2+BX+C
			ZeroMemory(pdblCoefficent, sizeof(double) * 10);
			m_Calpolyn.EMatrix(dbPixeArry, pdblDacTemp, PIXEL_NUMBER_64, 3, pdblCoefficent);
			ax = pdblCoefficent[3];
			bx = pdblCoefficent[2];
			cx = pdblCoefficent[1];
			//计算拟合的DAC值

			for (nPixelIndex = 0; nPixelIndex < PIXEL_NUMBER_64; nPixelIndex++)
			{
				dbCalValue = ax*((nPixelIndex + 1)*(nPixelIndex + 1)) + bx*(nPixelIndex + 1) + cx;
				pdblDac[nIndex + (nPixelIndex)*ATTEN_LUT_POINT] = dbCalValue;
			}
		}

		strFileName.Format("%s\\Calibrate\\%s\\256vsLut.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder);

		if (!SaveAttenVsDac(strFileName, pdblDac, PIXEL_NUMBER_64))
		{
			strTemp.Format("Save calculate ave atten dac lut error");
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFuntionOK = FALSE;
	}

	if (pdblDac != NULL)
	{
		VirtualFree(pdblDac, 0, MEM_RELEASE);
	}

	return bFuntionOK;
}


/////////////////////////////////////////////////////////////////
//函数说明：
//    根据中心波长定标重新计算Atten Lut文件
//Add by wanxin
//Time:20200617
//////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnCal_AttenLutAfterFindedPixelCenter()
{
	CString strFileName, strFileFolder;
	float   fGetAttenDAC[MAX_PATH];
	double *pdblAtten = NULL;
	int nGetLine = 0;
	double *pdblScanGhz = NULL;
	stPixelInfo  stPixelData;
	BOOL bFunctionOK = TRUE;
	CString strTemp;
	CString strPath;

	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	try
	{
		if (pdblAtten == NULL)
		{
			pdblAtten = (double*)VirtualAlloc(NULL, ATTEN_LUT_POINT*dwSampleCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		}
		ASSERT(pdblAtten);
		if (pdblScanGhz == NULL)
		{
			pdblScanGhz = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		}
		ASSERT(pdblScanGhz);

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			ZeroMemory(pdblScanGhz, sizeof(double)*dwSampleCount);
			ZeroMemory(pdblAtten, sizeof(double)*ATTEN_LUT_POINT*dwSampleCount);

			//从精确Atten定标的文件中读取相应的数据
			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strPath);
			strFileName.Format("%s\\Pixel_Atten_Scan_Fine.csv", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!fnReadAttenVsGhz(strFileName, dwSampleCount, fGetAttenDAC, pdblAtten, &nGetLine, pdblScanGhz))
			{
				strTemp.Format("读取atten精确定标数据存在错误");
				throw strTemp.GetBuffer(0);
			}

			//读取标定的Pixel中心波长
			strFileName.Format("%s\\PixelWave.ini", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!fnReadPixelWaveIni(strFileName, &stPixelData, FALSE)) //全部中心波长
			{
				strTemp.Format("Failed at reading the pixel wavelength ini file!");
				throw	strTemp.GetBuffer(0);
			}

			//13. 计算衰减LUT值，255 point per 0.1dB
			if (!fnCalcAttenVsDac(nIndex, &stPixelData.dblPixelGhz[0], pdblScanGhz, pdblAtten, fGetAttenDAC, nGetLine))
			{
				strTemp.Format("计算Pixel Attenuation value失败!");
				throw strTemp.GetBuffer(0);
			}

			//采用二阶拟合来计算
			if (!fnCalcAttenVsDac_AveByPolynomial(nIndex))
			{
				strTemp.Format("Calculate Pixel Attenuation vs DAC value failed");
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	if (pdblAtten != NULL)
	{
		VirtualFree(pdblAtten, 0, MEM_RELEASE);
		pdblAtten = NULL;
	}
	if (pdblScanGhz != NULL)
	{
		VirtualFree(pdblScanGhz, 0, MEM_RELEASE);
		pdblScanGhz = NULL;
	}

	return bFunctionOK;

}



BOOL CTestTaskManager::fnPixelILScan()
{
	CString strTemp;
	CString strFileName;
	int      nCount = 0;
	CString strPath;
	try
	{
		// 1. scan
		if (!fnTestScanOne())
			return FALSE;

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//保存通道IL数据
			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			strFileName.Format("%s\\Pixel_MinIL_Scan.csv", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!SaveScanILData(strFileName, nIndex + 1, m_stScanParameter.dwSampleCount))
			{
				throw " save the result raw data to file error";
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}


BOOL CTestTaskManager::fnCal_PixelIL()
{
	CString strTemp;
	CString strMsg;
	double  dblTemper;
	CString strFileName;

	int     i = 0;
	int     nPixelNum = 0;
	int     nGroupNum = 0;
	WORD    *pwSetDac = NULL;
	double   *pdblDac = NULL;
	stPixelOpen  stPixel;
	BOOL bFunctionOK = TRUE;
	int nPixelCount = 0;
	CString strPath;

	ZeroMemory(&stPixel, sizeof(stPixel));

	try
	{
		strTemp.Format("开始%.0fC温度Pixel IL 定标扫描测试......", m_stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		pdblDac = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*ATTEN_LUT_POINT * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pwSetDac = (WORD*)VirtualAlloc(NULL, PIXEL_NUMBER_64 * sizeof(WORD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//if (!JudgeCurrentSlotSN(nIndex))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			memset(pdblDac, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);
			memset(pwSetDac, 0, sizeof(WORD)*PIXEL_NUMBER_64);

			//1. Get DAC of mimum IL from attenuation calibration test
			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strPath);
			strFileName.Format("%s\\256vsLut.csv", strPath);
			if (!m_DGEModule[nIndex].fnReadAttenLut256Csv(strFileName, pdblDac, &nPixelCount))
			{
				strTemp.Format("Get %sC Lut256Csv file error!", m_DGEModule[nIndex].m_strTempFolder);
				throw strTemp.GetBuffer(0);
			}
			if (PIXEL_NUMBER_64 != nPixelCount)
			{
				strTemp.Format("Get %sC Atten Lut256Csv Line: %d != Pixel num %d", m_DGEModule[nIndex].m_strTempFolder, nPixelCount, PIXEL_NUMBER_64);
				throw strTemp.GetBuffer(0);
			}

			//2. set DAC of MinIL
			//Set all pixels to open
			for (int i = 0; i < PIXEL_NUMBER_64; i++)
			{
				pwSetDac[i] = (WORD)(pdblDac[i*ATTEN_LUT_POINT]);
			}

			if (!m_DGEModule[nIndex].fnSetAllPixelDAC(pwSetDac))
			{
				OpenSerialPortDevice();
				Sleep(1000);
				if (!m_DGEModule[nIndex].fnSetAllPixelDAC(pwSetDac))
				{
					strTemp = m_DGEModule[nIndex].m_strErrorMsg;
					throw strTemp.GetBuffer(0);
				}
			}

			//4. write calibration temperature
			if (!fnGetTemperWriteIni(nIndex, dblTemper, "IL", TRUE))
			{
				strTemp.Format("Read LC temperature error!");
				throw strTemp.GetBuffer(0);
			}
		}

		fnSleepLongTime(2500); //2018.5.18

		strTemp.Format("Set Pixel ok: Channel=ALL, DAC1=%d......DAC64=%d", pwSetDac[0], pwSetDac[63]);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		//3. TLS scan for pixel ADC open
		if (!fnPixelILScan())
		{
			strTemp.Format("Pixel IL scan Error");
			throw strTemp.GetBuffer(0);
		}

		strTemp.Format("Pixel Mini IL %sC calibration finished !", m_DGEModule[nIndex].m_strTempFolder);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pdblDac != NULL)
	{
		VirtualFree(pdblDac, 0, MEM_RELEASE);
		pdblDac = NULL;
	}
	if (pwSetDac != NULL)
	{
		VirtualFree(pwSetDac, 0, MEM_RELEASE);
		pwSetDac = NULL;
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::fnPixelWaveOffsetScan(int nLoop)
{
	CString strTemp;
	CString strFileName;
	int      nCount = 0;
	CString strPath;
	try
	{
		if (!fnTestScanOne())
			return FALSE;

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//保存通道IL数据
			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			strFileName.Format("%s\\Pixel_Wave_Scan_Offset_loop%d.csv", strPath, nLoop + 1);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!SaveScanILData(strFileName, nIndex + 1, m_stScanParameter.dwSampleCount))
			{
				throw " save the result raw data to file error";
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}


BOOL CTestTaskManager::fnReadILCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength)
{
	CString strTemp;
	CString strToken;
	int     nCount = 0;
	FILE *fpFile = NULL;
	char    chRead[MAX_PATH];
	BOOL bFunctionOK = TRUE;

	try
	{
		fpFile = fopen(lpstPath, "r");
		if (fpFile == NULL)
		{
			strTemp.Format("open file error:%s ", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		ZeroMemory(chRead, MAX_PATH);
		fgets(chRead, MAX_PATH, fpFile);

		int nRow = 0;
		while (1)
		{
			ZeroMemory(chRead, MAX_PATH);
			if (fgets(chRead, MAX_PATH, fpFile) == NULL)
			{
				break;
			}
			strToken = strtok(chRead, ",");
			pdblWave[nRow] = atof(strToken);
			strToken = strtok(NULL, ",");
			pdblGhz[nRow] = atof(strToken);
			strToken = strtok(NULL, ",");
			pdblIL[nRow] = atof(strToken);
			nRow++;

		}

		*nLength = nRow;

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;

	}

	return bFunctionOK;
}


BOOL CTestTaskManager::SaveCalcILOffset(LPCTSTR lpstPath, double *pdblPixelWave, double *pdblPixelGhz, double *pdblIL)
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
		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		if (strFileName.Find("IL_Offset") != -1)
		{
			//title for content: Ghz1, Ghz2, .....Ghz64
			strTemp.Format("IL Old,IL New,IL Offset(dB)\n");
		}
		else
		{
			//title for content: Ghz1, Ghz2, .....Ghz64
			strTemp.Format("Wave,Ghz,IL\n");
		}

		fprintf(pFile, "%s", strTemp);

		for (int i = 0; i < m_stScanParameter.dwSampleCount; i++)
		{
			strTemp.Format("%.3f,%.3f,%.3f\n", pdblPixelWave[i], pdblPixelGhz[i], pdblIL[i]);
			fprintf(pFile, "%s", strTemp);
		}
	}
	catch (TCHAR* ptszErrorMsg)
	{
		//		AfxMessageBox(ptszErrorMsg, MB_OK | MB_ICONERROR);
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		//		AfxMessageBox("Other fatal exception occurred",  MB_OK | MB_ICONERROR);
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::fnCalcPixelOffset(int nModuleIndex)
{
	CString strTemp;
	CString strFileName;

	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	double  *pdblWaveOld = NULL;
	double  *pdblGhzOld = NULL;
	double  *pdblILOld = NULL;
	double  *pdblWaveNew = NULL;
	double  *pdblGhzNew = NULL;
	double  *pdblILNew = NULL;
	double  *pdblOffset = NULL;
	double  *pdblSumIL = NULL;
	int     nDataNum = 0;
	BOOL   bFunctionOK = TRUE;
	CString strPath;
	DWORD dwSampleCount;

	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		//1. 检查pixel IL定标是否测试过
		strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder);
		strFileName.Format("%s\\Pixel_MinIL_Scan.csv", strPath);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!AdjustFileExit(strFileName))
		{
			strTemp.Format("Can't find file: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblSumIL = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblOffset = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		//数组清零
		ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblSumIL, dwSampleCount * sizeof(double));
		ZeroMemory(pdblOffset, dwSampleCount * sizeof(double));

		//2. 读取pixel Min IL定标时的数据
		if (!fnReadILCsvFile(strFileName, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
		{
			strTemp.Format("Error on reading file: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		for (int i = 0; i < PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL; i++)
		{

			strFileName.Format("%s\\Pixel_Wave_Scan_Offset_loop%d.csv", strPath, i + 1);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!AdjustFileExit(strFileName))
			{
				strTemp.Format("Can't find file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			//分配内存
			pdblWaveNew = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblGhzNew = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblILNew = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);


			//数组清零
			ZeroMemory(pdblWaveNew, dwSampleCount * sizeof(double));
			ZeroMemory(pdblGhzNew, dwSampleCount * sizeof(double));
			ZeroMemory(pdblILNew, dwSampleCount * sizeof(double));

			//3. 读取pixel wave cal offset loop 1定标时的数据
			if (!fnReadILCsvFile(strFileName, pdblWaveNew, pdblGhzNew, pdblILNew, &nDataNum))
			{
				strTemp.Format("Error on reading file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			strFileName.Format("%s\\Pixel_Wave_Scan_loop%d.csv", strPath, i + 1);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!AdjustFileExit(strFileName))
			{
				strTemp.Format("Can't find file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			//分配内存
			pdblWaveOld = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblGhzOld = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblILOld = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);


			//数组清零
			ZeroMemory(pdblWaveOld, dwSampleCount * sizeof(double));
			ZeroMemory(pdblGhzOld, dwSampleCount * sizeof(double));
			ZeroMemory(pdblILOld, dwSampleCount * sizeof(double));

			//4. 读取pixel wave cal loop 1定标时的数据
			if (!fnReadILCsvFile(strFileName, pdblWaveOld, pdblGhzOld, pdblILOld, &nDataNum))
			{
				strTemp.Format("Error on reading file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}


			//计算offset: mw
			//offset = MinIL - old_loop1 - new_loop1
			for (int k = 0; k < dwSampleCount; k++)
			{
				double total_mw = pow(10, -pdblILOld[k] / 10) + pow(10, -pdblILNew[k] / 10);
				//double offset = -pdblILBuf[k]-10*log10(total_mw);
				double offset = pow(10, -pdblILBuf[k] / 10) - total_mw; //改为mw计算offset差值
				if (offset < 0)//
				{
					offset = 0;
				}
				//pdblOffset[k] = offset;

				//target = old_loop1 + offset = MinIL - new_loop1
				//pdblSumIL[k] = pdblILOld[k]-offset;
				//double target_mw = pow(10, -pdblILOld[k]/10) + offset;
				double target_mw = pow(10, -pdblILOld[k] / 10) + offset / 2; //因pixel开/关两种状态都对电压产生影响，所以offset除以2
				pdblSumIL[k] = -10 * log10(target_mw); //插损为正值 
				pdblOffset[k] = pdblILOld[k] - pdblSumIL[k];

			}

			//update the wave cal loop data file
			if (!SaveCalcILOffset(strFileName, pdblWaveOld, pdblGhzOld, pdblSumIL))
			{
				throw " save the calclation IL offset to file error";
			}

			strFileName.Format("%s\\IL_Offset_loop%d.csv", strPath, i + 1);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			//update the wave cal loop data file
			if (!SaveCalcILOffset(strFileName, pdblILOld, pdblILNew, pdblOffset))
			{
				throw " save the calclation IL offset to file error";
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
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

	if (pdblSumIL != NULL)
	{
		VirtualFree(pdblSumIL, 0, MEM_RELEASE);
		pdblSumIL = NULL;
	}

	if (pdblOffset != NULL)
	{
		VirtualFree(pdblOffset, 0, MEM_RELEASE);
		pdblOffset = NULL;
	}

	if (pdblWaveOld != NULL)
	{
		VirtualFree(pdblWaveOld, 0, MEM_RELEASE);
		pdblWaveOld = NULL;
	}
	if (pdblGhzOld != NULL)
	{
		VirtualFree(pdblGhzOld, 0, MEM_RELEASE);
		pdblGhzOld = NULL;
	}
	if (pdblILOld != NULL)
	{
		VirtualFree(pdblILOld, 0, MEM_RELEASE);
		pdblILOld = NULL;
	}


	if (pdblWaveNew != NULL)
	{
		VirtualFree(pdblWaveNew, 0, MEM_RELEASE);
		pdblWaveNew = NULL;
	}
	if (pdblGhzNew != NULL)
	{
		VirtualFree(pdblGhzNew, 0, MEM_RELEASE);
		pdblGhzNew = NULL;
	}
	if (pdblILNew != NULL)
	{
		VirtualFree(pdblILNew, 0, MEM_RELEASE);
		pdblILNew = NULL;
	}

	return TRUE;
}


BOOL CTestTaskManager::fnCalOffset_PixelCenterWave()
{
	CString strTemp;
	CString strMsg;
	CString strFileName;
	int     i = 0;
	int     nGroupNum = 0;
	stPixelOpen  stPixel[MAXMODULECOUNT];
	double  dblTemper = 0;
	double   *pdblDac = NULL;
	int     nPixelCount = 0;
	BOOL   bFunctionOK = TRUE;
	CString strPath;
	ZeroMemory(&stPixel, sizeof(stPixel));

	try
	{

		strTemp.Format("开始%.0fC温度Pixel beam offset定标扫描测试......", m_stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		//Read pixel setting
		pdblDac = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*ATTEN_LUT_POINT * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//if (!JudgeCurrentSlotSN(nIndex))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			memset(pdblDac, 0, sizeof(double)*PIXEL_NUMBER_64*ATTEN_LUT_POINT);

			//1. Get DAC of mimum IL from attenuation calibration test
			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strPath);
			strFileName.Format("%s\\256vsLut.csv", strPath);
			if (!m_DGEModule[nIndex].fnReadAttenLut256Csv(strFileName, pdblDac, &nPixelCount))
			{
				strTemp.Format("Get %sC Lut256Csv file error!", m_DGEModule[nIndex].m_strTempFolder);
				throw strTemp.GetBuffer(0);
			}
			if (PIXEL_NUMBER_64 != nPixelCount)
			{
				strTemp.Format("Get %sC Atten Lut256Csv Line: %d != Pixel num %d", m_DGEModule[nIndex].m_strTempFolder, nPixelCount, PIXEL_NUMBER_64);
				throw strTemp.GetBuffer(0);
			}

			//2. set DAC of MinIL
			//Set all pixels to open
			for (int i = 0; i < PIXEL_NUMBER_64; i++)
			{
				stPixel[nIndex].iOpenDac[i] = (int)(pdblDac[i*ATTEN_LUT_POINT]);
				stPixel[nIndex].iBlockDac[i] = (int)(pdblDac[i*ATTEN_LUT_POINT + ATTEN_LUT_POINT - 1]);
			}
		}


		//64 pixel to 8 pcs as one group
		nGroupNum = PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL;
		for (int nGPIndex = 0; nGPIndex < nGroupNum; nGPIndex++) //about 8 times to scan 
		{
			for (int nModule = 0; nModule < m_nDGECount; nModule++)
			{
				strTemp.Format("第%d次中心波长Offset扫描......", nGPIndex + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
				//3. Set the desired pixel to block and others for open
				if (!m_DGEModule[nModule].fnSetPixelGroupDAC(nGPIndex, &stPixel[nModule], FALSE)) //关一开七的情形
				{
					OpenSerialPortDevice();
					Sleep(1000);
					if (!m_DGEModule[nModule].fnSetPixelGroupDAC(nGPIndex, &stPixel[nModule], FALSE)) //关一开七的情形
					{
						strTemp = m_DGEModule[nModule].m_strErrorMsg;
						throw strTemp.GetBuffer(0);
					}
				}

			}
			fnSleepLongTime(1500);

			//4. TLS scan for pixel ADC setting
			if (!fnPixelWaveOffsetScan(nGPIndex))
			{
				strTemp.Format("Pixel wave scan Error");
				throw strTemp.GetBuffer(0);
			}
		}

		for (nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//calculation of pixel offset compersation
			if (!fnCalcPixelOffset(nIndex))
			{
				strTemp.Format("Calculate the pixe offset error!");
				throw strTemp.GetBuffer(0);
			}
		}
		strTemp.Format("Pixel center wavelength Offset calibration finished !");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pdblDac != NULL)
	{
		VirtualFree(pdblDac, 0, MEM_RELEASE);
		pdblDac = NULL;
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::SearchPixelMinIL(LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength)
{
	CString strTemp;
	double  dblCurWave = 0;
	double  dblTemp = 0;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	int     nDataNum = 0;
	DWORD dwSampleCount = 0;
	BOOL bFunctionOK = TRUE;

	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		//数组清零
		ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));

		//读取pixel Min IL定标时的数据
		if (!fnReadILCsvFile(lpstPath, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
		{
			strTemp.Format("Error on reading file: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		if (nDataNum != dwSampleCount)
		{
			strTemp.Format("Read IL scan point count is not equal actual point count: %s");
			throw strTemp.GetBuffer(0);
		}
		//7. 查找中心波长的最小IL
		BOOL bFind;
		for (int nIndex = 0; nIndex < stPixelData.nPixelWavNum; nIndex++)
		{
			bFind = FALSE;
			dblCurWave = stPixelData.dblPixelWav[nIndex];

			for (int nScanIndex = 1; nScanIndex < nDataNum; nScanIndex++)
			{
				//中心波长介于两个扫描波长之间，取插损IL平均值
				if ((dblCurWave < pdblWaveBuf[nScanIndex]) && (dblCurWave > pdblWaveBuf[nScanIndex - 1]))
				{
					pdblWave[nIndex] = dblCurWave;
					pdblIL[nIndex] = (pdblILBuf[nScanIndex] + pdblILBuf[nScanIndex - 1]) / 2;
					pdblGhz[nIndex] = (pdblGhzBuf[nScanIndex] + pdblGhzBuf[nScanIndex - 1]) / 2;
					bFind = TRUE;
				}
				else if (dblCurWave == pdblWaveBuf[nScanIndex])
				{
					pdblWave[nIndex] = dblCurWave;
					pdblIL[nIndex] = pdblILBuf[nScanIndex];
					pdblGhz[nIndex] = pdblGhzBuf[nScanIndex];
					bFind = TRUE;
				}
			}

			if (!bFind)
			{
				strTemp.Format("Can't find the wavelength: %.3fnm(index=%d)", dblCurWave, nIndex + 1);
				throw strTemp.GetBuffer(0);
			}
		}

		*nLength = nIndex; //Pixel number

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
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

	return bFunctionOK;
}
//////////////////////////////////////////////////////////////////////////////////
//函数说明:
//	  该函数主要为解决由于1、2、63、64 Pixel没有中心波长导致8组扫描数据没有
//Pixel 1 - Pixel 4、Pixel2 - Pixel 4、Pixel63 -Pixel62、Pixel64 - Pixel62
//Add by wanxin
//Time:20181018
///////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::HandleGroupPixelMinIL(int nModuleIndex, int nGoupIndex, double dbPixelCenterWL, double *pdblWave, double *pdblGhz, double *pdblIL)
{
	CString strTemp;
	double  dblCurWave = 0;
	double  dblTargetIL = 0;
	double  dblTemp = 0;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	int     nDataNum = 0;
	double  dblBaseWave = 0; //from first point at +/-200G
	BOOL  bFunctionOK = TRUE;
	CString strFileName;
	DWORD dwSampleCount = 0;
	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		if (nGoupIndex == 0 || nGoupIndex == 1)
		{
			strFileName.Format("%s\\Calibrate\\%s\\Pixel_Wave_Scan_loop%d.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder, 4);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
		}
		else if (nGoupIndex == 6 || nGoupIndex == 7)
		{
			strFileName.Format("%s\\Calibrate\\%s\\Pixel_Wave_Scan_loop%d.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder, 5);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
		}

		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		//数组清零
		ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));


		//读取中心波长定标时的通道IL数据，每次可以读取8个Pixel的中心波长
		//Pixel_Wave_Scan_loop%d.csv
		if (!fnReadILCsvFile(strFileName, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
		{
			strTemp.Format("Error on reading file: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		BOOL bFind = FALSE;
		dblCurWave = dbPixelCenterWL;

		for (int nScanIndex = 0; nScanIndex < nDataNum; nScanIndex++)
		{
			//中心波长介于两个扫描波长之间，取插损IL平均值
			if ((dblCurWave < pdblWaveBuf[nScanIndex]) && (dblCurWave > pdblWaveBuf[nScanIndex - 1]))
			{
				*pdblWave = dblCurWave;
				*pdblGhz = (pdblGhzBuf[nScanIndex] + pdblGhzBuf[nScanIndex - 1]) / 2;
				*pdblIL = (pdblILBuf[nScanIndex] + pdblILBuf[nScanIndex - 1]) / 2;
				bFind = TRUE;
			}
			else if (dblCurWave == pdblWaveBuf[nScanIndex])
			{
				*pdblWave = dblCurWave;
				*pdblGhz = pdblGhzBuf[nScanIndex];
				*pdblIL = pdblILBuf[nScanIndex];
				bFind = TRUE;
			}
		}

		if (!bFind)
		{
			strTemp.Format("Can't find the wavelength: %.3fnm", dblCurWave);
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	VirtualFree(pdblWaveBuf, 0, MEM_RELEASE);
	VirtualFree(pdblGhzBuf, 0, MEM_RELEASE);
	VirtualFree(pdblILBuf, 0, MEM_RELEASE);

	pdblWaveBuf = NULL;
	pdblGhzBuf = NULL;
	pdblILBuf = NULL;

	return bFunctionOK;
}


BOOL CTestTaskManager::LinearInsertValue(double *pdblXArray, double *pdblYArray, BOOL bXAxisIncrease, DWORD dwDataCount, double dblX, double *pdblY)
{
	BOOL     bFunctionOK = FALSE;
	DWORD    dwDataIndex = 0;
	double   dblDelta = 0.0001;
	CString strTemp;

	// formula:(Y-Y1)/(X-X1) = (Y2-Y1)/(X2-X1)
	// ==> Y = Y1 + (Y2-Y1)/(X2-X1)*(X-X1)
	if (NULL == pdblXArray || NULL == pdblYArray || dwDataCount <= 1)
	{
		strTemp.Format("the input array parameter is NULL, please check it again!");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
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


//////////////////////////////////////////////////////////////////////////////////
//函数说明:
//	  该函数主要为解决由于1、2、63、64 Pixel没有中心波长导致8组扫描数据没有
//实际谱行，1、2用第三个Pixel谱行替换，64、64用第62pixel代替
//Add by wanxin
//Time:20181017
///////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnHandlePixelChanelIL(int nModuleIndex, int nGroupIndex, double dbCenterWL, double *pdblWave, double *pdblGhz, double *pdblIL)
{
	CString strTemp;
	double  dblCurWave = 0;
	double  dblTargetIL = 0;
	double  dblTemp = 0;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	int     nDataNum = 0;
	double  dblBaseWave = 0; //from first point at +/-200G
	BOOL  bFunctionOK = TRUE;
	CString strFileName;
	DWORD dwSampleCount;
	double dblCurFreq = 0.0;
	double dblBaseFreq = 0.0;

	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		if (nGroupIndex == 0 || nGroupIndex == 1)
		{
			strFileName.Format("%s\\Calibrate\\%s\\Pixel_Wave_Scan_loop%d.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder, 4);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
		}
		else if (nGroupIndex == 6 || nGroupIndex == 7)
		{
			strFileName.Format("%s\\Calibrate\\%s\\Pixel_Wave_Scan_loop%d.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder, 5);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
		}



		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		//数组清零
		ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));


		//读取中心波长定标时的通道IL数据，每次可以读取8个Pixel的中心波长
		//Pixel_Wave_Scan_loop%d.csv
		if (!fnReadILCsvFile(strFileName, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
		{
			strTemp.Format("Error on reading file: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		dblCurWave = dbCenterWL;
		dblCurFreq = VELOCITY_LIGHT / dblCurWave;

		//if (m_DGEModule[nModuleIndex].m_nBeamBinVersion == 2)
		//{
		//	dblBaseWave = dblCurWave - 200 * PIXEL_WAVE_STEP2;
		//	dblBaseFreq = dblCurFreq + 200 * PIXEL_FRE_STEP2;
		//}
		//else
		//{
		//	dblBaseWave = dblCurWave - 200 * PIXEL_WAVE_STEP;
		//	dblBaseFreq = dblCurFreq + 200 * PIXEL_FRE_STEP;
		//}



		int nCalIndex = 0;
		for (int iStart = PIXEL_BEAM_NUMBER; iStart >= 0; iStart--)
		{
			//if (m_DGEModule[nModuleIndex].m_nBeamBinVersion == 2)
			//{
			//	dblCurFreq = dblBaseFreq - iStart*PIXEL_FRE_STEP2;
			//	//dblCurWave = dblBaseWave + iStart*PIXEL_WAVE_STEP;
			//	dblCurWave = VELOCITY_LIGHT / dblCurFreq;
			//}
			//else
			//{
			//	dblCurFreq = dblBaseFreq - iStart*PIXEL_FRE_STEP;
			//	//dblCurWave = dblBaseWave + iStart*PIXEL_WAVE_STEP;
			//	dblCurWave = VELOCITY_LIGHT / dblCurFreq;
			//}



			//IL插值
			LinearInsertValue(pdblWaveBuf, pdblILBuf, TRUE, nDataNum, dblCurWave, &dblTargetIL);
			pdblWave[nCalIndex] = dblCurWave;
			pdblGhz[nCalIndex] = VELOCITY_LIGHT / dblCurWave;
			pdblIL[nCalIndex] = dblTargetIL;
			nCalIndex++;
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	VirtualFree(pdblWaveBuf, 0, MEM_RELEASE);
	VirtualFree(pdblGhzBuf, 0, MEM_RELEASE);
	VirtualFree(pdblILBuf, 0, MEM_RELEASE);

	pdblWaveBuf = NULL;
	pdblGhzBuf = NULL;
	pdblILBuf = NULL;

	return bFunctionOK;
}

BOOL CTestTaskManager::SearchPixelChannelIL(int nModuleIndex, LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblWave, double *pdblGhz, double *pdblIL, int *nLength)
{
	CString strTemp;
	double  dblCurWave = 0;
	double  dblTargetIL = 0;
	double  dblTemp = 0;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	int     nDataNum = 0;
	double  dblBaseWave = 0; //from first point at +/-200G
	DWORD dwSampleCount;
	double dblCurFreq = 0.0;
	double dblBaseFreq = 0.0;

	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		//分配内存
		pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		//数组清零
		ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
		ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));


		//读取中心波长定标时的通道IL数据，每次可以读取8个Pixel的中心波长
		//Pixel_Wave_Scan_loop%d.csv
		if (!fnReadILCsvFile(lpstPath, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
		{
			strTemp.Format("Error on reading file: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		for (int nIndex = 0; nIndex < stPixelData.nPixelWavNum; nIndex++)
		{
			dblCurWave = stPixelData.dblPixelWav[nIndex];
			dblCurFreq = VELOCITY_LIGHT / dblCurWave;
			//if (m_DGEModule[nModuleIndex].m_nBeamBinVersion == 2)
			//{
			//	dblBaseFreq = dblCurFreq + 200 * PIXEL_FRE_STEP2;
			//	//dblBaseWave = dblCurWave - 200 * PIXEL_WAVE_STEP;
			//	dblBaseWave = VELOCITY_LIGHT / dblBaseFreq;
			//}
			//else
			//{
			//	dblBaseFreq = dblCurFreq + 200 * PIXEL_FRE_STEP;
			//	//dblBaseWave = dblCurWave - 200 * PIXEL_WAVE_STEP;
			//	dblBaseWave = VELOCITY_LIGHT / dblBaseFreq;
			//}


			//for(int iStart = 0; iStart < PIXEL_BEAM_NUMBER; iStart++)
			int nCalIndex = 0;
			for (int iStart = PIXEL_BEAM_NUMBER; iStart >= 0; iStart--)//Modify by wanxin，这里发现实际上是高低频反了，需要修正
			{

				//if (m_DGEModule[nModuleIndex].m_nBeamBinVersion == 2)
				//{
				//	dblCurFreq = dblBaseFreq - iStart*PIXEL_FRE_STEP2;
				//	//dblCurWave = dblBaseWave + iStart*PIXEL_WAVE_STEP;
				//	dblCurWave = VELOCITY_LIGHT / dblCurFreq;
				//}
				//else
				//{

				//}
				//dblCurFreq = dblBaseFreq - iStart*PIXEL_FRE_STEP;
				//dblCurWave = dblBaseWave + iStart*PIXEL_WAVE_STEP;
				//dblCurWave = VELOCITY_LIGHT / dblCurFreq;

				//IL插值
				LinearInsertValue(pdblWaveBuf, pdblILBuf, TRUE, nDataNum, dblCurWave, &dblTargetIL);

				pdblWave[nCalIndex + nIndex*PIXEL_BEAM_NUMBER] = dblCurWave;
				pdblGhz[nCalIndex + nIndex*PIXEL_BEAM_NUMBER] = VELOCITY_LIGHT / dblCurWave;
				pdblIL[nCalIndex + nIndex*PIXEL_BEAM_NUMBER] = dblTargetIL;
				nCalIndex++;
			}
		}

		*nLength = stPixelData.nPixelWavNum*PIXEL_BEAM_NUMBER;

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	VirtualFree(pdblWaveBuf, 0, MEM_RELEASE);
	VirtualFree(pdblGhzBuf, 0, MEM_RELEASE);
	VirtualFree(pdblILBuf, 0, MEM_RELEASE);

	pdblWaveBuf = NULL;
	pdblGhzBuf = NULL;
	pdblILBuf = NULL;

	return TRUE;
}


//////////////////////////////////////////////////////////////
//函数说明：
//   用来对计算的Ratio进行处理，主要是处理边缘翘起问题
//Add by wanxin
//Time:20181018
////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::HandleRitioData(int nCenterDeta, int nConitueGrouth, double dbSetValue, double* dbHandleRatio, int nRatioLength)
{

	double *pdbRatioBuf = NULL;
	if (pdbRatioBuf == NULL)
	{
		pdbRatioBuf = (double*)VirtualAlloc(NULL, nRatioLength * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		ZeroMemory(pdbRatioBuf, nRatioLength * sizeof(double));
	}
	memcpy(pdbRatioBuf, dbHandleRatio, nRatioLength * sizeof(double));

	double dbLeftMinRatio = 9999;
	int nLeftMinRatioIndex = 0;

	for (int i = 0; i < nCenterDeta; i++)
	{
		if (pdbRatioBuf[i] < dbLeftMinRatio)
		{
			dbLeftMinRatio = pdbRatioBuf[i];
			nLeftMinRatioIndex = i;
		}
	}
	int nCalCount = 0;
	if (nLeftMinRatioIndex > nConitueGrouth)
	{
		for (int m = nLeftMinRatioIndex; m > 0; m--)
		{
			if (pdbRatioBuf[m] < pdbRatioBuf[m - 1])
			{
				nCalCount++;
			}
		}
	}
	if (nCalCount > nConitueGrouth)
	{
		for (int m = nLeftMinRatioIndex; m >= 0; m--)
		{
			pdbRatioBuf[m] = dbSetValue;
		}
	}
	else
	{
		for (int m = nLeftMinRatioIndex; m >= 0; m--)
		{
			if (pdbRatioBuf[m] - dbLeftMinRatio > 0.1)
			{
				pdbRatioBuf[m] = dbSetValue;
			}
		}
	}

	double dbRightMinRatio = 999;
	int nRightMinRatioIndex = 0;
	for (int j = nRatioLength - nCenterDeta; j < nRatioLength; j++)
	{
		if (pdbRatioBuf[j] < dbRightMinRatio)
		{
			dbRightMinRatio = pdbRatioBuf[i];
			nRightMinRatioIndex = j;
		}
	}


	nCalCount = 0;
	if ((nRatioLength - nRightMinRatioIndex) > nConitueGrouth)
	{
		for (int m = nRightMinRatioIndex; m < nRatioLength - 1; m++)
		{
			if (pdbRatioBuf[m] < pdbRatioBuf[m + 1])
			{
				nCalCount++;
			}
		}
	}
	if (nCalCount > nConitueGrouth)
	{
		for (int m = nRightMinRatioIndex; m < nRatioLength; m++)
		{
			pdbRatioBuf[m] = dbSetValue;
		}
	}
	else
	{
		for (int m = nRightMinRatioIndex; m < nRatioLength; m++)
		{
			if (pdbRatioBuf[m] - nRightMinRatioIndex > 0.1)
			{
				pdbRatioBuf[m] = dbSetValue;
			}
		}
	}
	memcpy(dbHandleRatio, pdbRatioBuf, nRatioLength * sizeof(double));

	if (pdbRatioBuf != NULL)
	{
		VirtualFree(pdbRatioBuf, 0, MEM_RELEASE);
	}
	return TRUE;
}


BOOL CTestTaskManager::SaveILRatio_Beam(LPCTSTR lpstPath, stPixelInfo  stPixelData, double *pdblWave, double *pdblGhz, double *pdblIL, double *pdblRatio1, double *pdblRatio2 /* = NULL */)
{
	BOOL         bFunctionOK = TRUE;

	CString      strTemp;
	CString      strFileName;
	double       dblWaveTemp = 0;
	FILE*        pFile = NULL;
	int          nDataNum = 0;
	int          nPixelNum = 0;



	try
	{

		strFileName = lpstPath;
		nPixelNum = stPixelData.nPixelWavNum;

		if (nPixelNum < 1)
		{
			strTemp.Format("无效的Wave Number: %d", nPixelNum);
			throw strTemp.GetBuffer(0);
		}

		//read the IL data
		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}


		//title for content
		strTemp.Format("WL,Beam,Ghz,IL,Ratio1(%%),Ratio2(%%)\n");
		fprintf(pFile, "%s", strTemp);

		if ((PIXEL_NUMBER_64 == nPixelNum) && strFileName.Find("Beam_ILRatio") == -1)
		{
			nDataNum = nPixelNum;
		}
		else
		{
			nDataNum = nPixelNum*PIXEL_BEAM_NUMBER;
		}
		for (int i = 0; i < nDataNum; i++)
		{
			if ((PIXEL_NUMBER_64 == nPixelNum) && strFileName.Find("Beam_ILRatio") == -1)
			{
				dblWaveTemp = stPixelData.dblPixelWav[i];
			}
			else
			{
				dblWaveTemp = stPixelData.dblPixelWav[i / PIXEL_BEAM_NUMBER];
			}

			//from here the beam data
			if (NULL != pdblRatio2)
			{
				//64个中心波长的全开IL和开一关八MinIL百分比
				strTemp.Format("%.3f,%.3f,%.3f,%.3f,%.5f,%.5f\n",
					dblWaveTemp,
					pdblWave[i],
					pdblGhz[i],
					pdblIL[i],
					pdblRatio1[i],
					pdblRatio2[i]);
			}
			else
			{
				strTemp.Format("%.3f,%.3f,%.3f,%.3f,%.5f\n",
					dblWaveTemp,
					pdblWave[i],
					pdblGhz[i],
					pdblIL[i],
					pdblRatio1[i]);
			}

			fprintf(pFile, "%s", strTemp);

		}// end for loop

	}
	catch (TCHAR* ptszErrorMsg)
	{
		//		AfxMessageBox(ptszErrorMsg, MB_OK | MB_ICONERROR);
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		//		AfxMessageBox("Other fatal exception occurred",  MB_OK | MB_ICONERROR);
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::fnReadBeamCsvFile(LPCTSTR lpstPath, double *pdblWave, double *pdblGhz, double *pdblIL, double *pdblRatio1, double *pdblRatio2, int *nLength)
{
	CString strTemp;
	CString strToken;
	int     nCount = 0;
	FILE * fpFile = NULL;
	char    chRead[MAX_PATH];
	BOOL bFunctionOK = TRUE;
	ZeroMemory(chRead, MAX_PATH);


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
			//from second line
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
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::fnCal_PixelBeam()
{
	CString strTemp;
	CString strMsg;
	WIN32_FIND_DATA winFindFile;
	HANDLE  hr = INVALID_HANDLE_VALUE;
	CString strFileName;
	double  *pdblWaveBuf = NULL;
	double  *pdblGhzBuf = NULL;
	double  *pdblILBuf = NULL;
	double  *pdblRatio1 = NULL;
	double  *pdblRatio2 = NULL;

	double  *pdblWave_Beam = NULL;
	double  *pdblGhz_Beam = NULL;
	double  *pdblIL_Beam = NULL;
	double  *pdblR1_Beam = NULL;
	double  *pdblR2_Beam = NULL;

	int     nPixelNum = 0;
	int     nDataNum = 0;
	stPixelInfo  stPixelData1;  //全部Pixel, Pixel全开时的数据
	stPixelInfo  stPixelData2;  //全部Pixel，Pixel开一关8时的数据
	stPixelInfo  stPixelTemp;  //分组，每组8个Pixel的临时数据

	double dbCalWLBuf[512];
	double dbCalGHzBuf[512];
	double dbCalILBuf[512];
	BOOL bFunctionOK = TRUE;
	CString strPath;
	DWORD dwSampleCount;
	try
	{
		dwSampleCount = m_stScanParameter.dwSampleCount;
		strTemp.Format("开始%.0fC温度Pixel Beam Relation 定标扫描测试......", m_stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			ZeroMemory(&stPixelData1, sizeof(stPixelInfo));
			ZeroMemory(&stPixelData2, sizeof(stPixelInfo));
			int nGroupNum = PIXEL_NUMBER_64 / (PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL);//扫描次数

			strPath.Format("%s\\Calibrate\\%s", m_DGEModule[nIndex].m_strCaliFolder, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strPath);
			strFileName.Format("%s\\PixelWave.ini", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			//2. Get the 64 pieces of pixel center wavelength
			if (!fnReadPixelWaveIni(strFileName, &stPixelData1, FALSE)) //全部中心波长
			{
				strTemp.Format("Failed at reading the pixel wavelength ini file!");
				throw	strTemp.GetBuffer(0);
			}

			//3. 检查pixel IL定标是否测试过
			strFileName.Format("%s\\Pixel_MinIL_Scan.csv", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);


			//分配内存
			pdblWaveBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblGhzBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblILBuf = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblRatio1 = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblRatio2 = (double*)VirtualAlloc(NULL, dwSampleCount * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);


			//数组清零
			ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));
			ZeroMemory(pdblRatio1, dwSampleCount * sizeof(double));
			ZeroMemory(pdblRatio2, dwSampleCount * sizeof(double));


			//3. 中心波长插损IL
			if (!SearchPixelMinIL(strFileName, stPixelData1, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nPixelNum))
			{
				strTemp.Format("Error on reading file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}//第一次全开时扫描数据

			memcpy(&stPixelData1.dblPixelGhz, pdblGhzBuf, sizeof(double)*nPixelNum);
			memcpy(&stPixelData1.dblPixelIL, pdblILBuf, sizeof(double)*nPixelNum);

			//插值计算每个Pixel中心波长+/-200G范围内每1G波长的IL和百分比
			//int nGroupNum = PIXEL_NUMBER_64/(PIXEL_NUMBER_64/PIXEL_SET_INTERVAL);//扫描次数
			for (int nGroupIndex = 0; nGroupIndex < nGroupNum; nGroupIndex++)
			{
				//4. 检查pixel中心波长csv定标文件是否存在
				strFileName.Format("%s\\Pixel_Wave_Scan_loop%d.csv", strPath, nGroupIndex + 1);
				LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

				hr = FindFirstFile(strFileName, &winFindFile);
				if (INVALID_HANDLE_VALUE == hr)
				{
					FindClose(hr); //没找到pixel mini IL定标csv文件
					strTemp.Format("Can't find file: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}
				else
				{
					FindClose(hr);
				}


				//数组清零
				ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));

				ZeroMemory(&stPixelTemp, sizeof(stPixelInfo));

				strTemp.Format("第%d组：", nGroupIndex + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

				int k = 0;
				for (int i = 0; i < stPixelData1.nPixelWavNum; i++)//分为8组数据
				{
					if (i % 8 == nGroupIndex)//间隔只取8个波长
					{
						stPixelTemp.dblPixelWav[k] = stPixelData1.dblPixelWav[i];
						stPixelTemp.dblPixelGhz[k] = stPixelData1.dblPixelGhz[i];
						stPixelTemp.dblPixelIL[k] = stPixelData1.dblPixelIL[i];

						strTemp.Format("Center WL%d=%.3fnm; IL=%.3fdB", k + 1, stPixelTemp.dblPixelWav[k], stPixelTemp.dblPixelIL[k]);
						LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
						k++;//
					}
				}

				stPixelTemp.nPixelWavNum = PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL;

				//5. 中心波长插损IL，每组8个中心波长
				if (!SearchPixelMinIL(strFileName, stPixelTemp, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nPixelNum))
				{
					strTemp.Format("Error on reading file: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}//第二次开一关八时扫描数据

				 //这里单独对第一组Pixel 1、第二组Pixel2、第7组Pixel63、第8组Pixel64进行处理
				double dbCalWL;
				double dbCalGHz;
				double dbCalIL;
				if (nGroupIndex == 0 || nGroupIndex == 1)
				{
					if (!HandleGroupPixelMinIL(nIndex, nGroupIndex, stPixelData1.dblPixelWav[3], &dbCalWL, &dbCalGHz, &dbCalIL))
					{
						strTemp.Format("Handle pixel 1 or pixel 2 center WL min IL error");
						throw strTemp.GetBuffer(0);
					}
					pdblILBuf[0] = dbCalIL;
					stPixelData1.dblPixelIL[nGroupIndex] = stPixelData1.dblPixelIL[3];

				}
				else if (nGroupIndex == 6 || nGroupIndex == 7)
				{
					if (!HandleGroupPixelMinIL(nIndex, nGroupIndex, stPixelData1.dblPixelWav[60], &dbCalWL, &dbCalGHz, &dbCalIL))
					{
						strTemp.Format("Handle pixel 63 or pixel 64 center WL min IL error");
						throw strTemp.GetBuffer(0);
					}
					pdblILBuf[7] = dbCalIL;
					stPixelData1.dblPixelIL[nGroupIndex + 56] = stPixelData1.dblPixelIL[60]; //63\64 Pixel
				}

				//copy
				memcpy(&stPixelData2.dblPixelWav[nGroupIndex*PIXEL_SET_INTERVAL], pdblWaveBuf, sizeof(double)*nPixelNum);
				memcpy(&stPixelData2.dblPixelGhz[nGroupIndex*PIXEL_SET_INTERVAL], pdblGhzBuf, sizeof(double)*nPixelNum);
				memcpy(&stPixelData2.dblPixelIL[nGroupIndex*PIXEL_SET_INTERVAL], pdblILBuf, sizeof(double)*nPixelNum);

				//数组清零
				ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblRatio1, dwSampleCount * sizeof(double));
				ZeroMemory(pdblRatio2, dwSampleCount * sizeof(double));

				//6. 读取每个Pixel的401个频率点插损, nDataNum = 401*8
				if (!SearchPixelChannelIL(nIndex, strFileName, stPixelTemp, pdblWaveBuf, pdblGhzBuf, pdblILBuf, &nDataNum))
				{
					strTemp.Format("Error on reading file: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}//第二次开一关八时扫描数据
				int nDataNumTemp = nDataNum;

				if (nGroupIndex == 0 || nGroupIndex == 1)
				{
					ZeroMemory(dbCalWLBuf, sizeof(double) * 512);
					ZeroMemory(dbCalGHzBuf, sizeof(double) * 512);
					ZeroMemory(dbCalILBuf, sizeof(double) * 512);

					if (!fnHandlePixelChanelIL(nIndex, nGroupIndex, stPixelData1.dblPixelWav[3], dbCalWLBuf, dbCalGHzBuf, dbCalILBuf))
					{
						strTemp.Format("Handle pixel 1 or pixel 2 ratio data error");
						throw strTemp.GetBuffer(0);
					}
					memcpy(pdblILBuf, dbCalILBuf, sizeof(double)*PIXEL_BEAM_NUMBER);
				}
				else if (nGroupIndex == 6 || nGroupIndex == 7)
				{
					ZeroMemory(dbCalWLBuf, sizeof(double) * 512);
					ZeroMemory(dbCalGHzBuf, sizeof(double) * 512);
					ZeroMemory(dbCalILBuf, sizeof(double) * 512);

					if (!fnHandlePixelChanelIL(nIndex, nGroupIndex, stPixelData1.dblPixelWav[60], dbCalWLBuf, dbCalGHzBuf, dbCalILBuf))
					{
						strTemp.Format("Handle pixel 1 or pixel 2 ratio data error");
						throw strTemp.GetBuffer(0);
					}
					memcpy(pdblILBuf + 7 * PIXEL_BEAM_NUMBER, dbCalILBuf, sizeof(double)*PIXEL_BEAM_NUMBER);
				}


				//7. Save 401/per 1G IL ratio 
				//////////////////////////////////////////////////////////////////////////
				//ratio1 format: 10^((beam_IL - wl_IL)/10)
				//ratio2 format: ratio1*10^((wl_IL-wl_IL(all pixel open))/10)

				int nPeakIndex = 0;
				double offset1 = 0;
				double offset2 = 0;
				double dblWL_IL1 = 0;
				double dblWL_IL2 = 0;
				int    nloop = 0;
				for (i = 0; i < nDataNumTemp; i++)
				{
					nloop = i / PIXEL_BEAM_NUMBER;
					nPeakIndex = PIXEL_BEAM_NUMBER / 2; //index = 200中心波长
					dblWL_IL2 = stPixelData2.dblPixelIL[nloop + nGroupIndex*PIXEL_SET_INTERVAL];
					offset2 = dblWL_IL2 - pdblILBuf[i];

					pdblRatio1[i] = pow(10, offset2 / 10);//ratio1

					dblWL_IL1 = stPixelData1.dblPixelIL[nloop * 8 + nGroupIndex];

					offset1 = dblWL_IL1 - dblWL_IL2;

					pdblRatio2[i] = pdblRatio1[i] * pow(10, offset1 / 10);//ratio2
				}
				//处理ratio数据
				for (i = 0; i < nDataNumTemp / PIXEL_BEAM_NUMBER; i++)
				{
					if (!HandleRitioData(100, 10, 0.0, pdblRatio2 + i*PIXEL_BEAM_NUMBER, PIXEL_BEAM_NUMBER))
					{
						strTemp.Format("Handle ratio 2 error!");
						throw strTemp.GetBuffer(0);
					}
				}
				stPixelTemp.nPixelWavNum = PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL;
				//保存结果，每个文件保存8个Pixel的beam
				strFileName.Format("%s\\Beam_ILRatio_loop%d.csv", strPath, nGroupIndex + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
				if (!SaveILRatio_Beam(strFileName, stPixelTemp, pdblWaveBuf, pdblGhzBuf, pdblILBuf, pdblRatio1, pdblRatio2))
				{
					strTemp.Format("Failed at saving file: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}
			}

			//7. Save center wavelength and IL ratio 
			//////////////////////////////////////////////////////////////////////////
			for (int loop = 0; loop < stPixelData1.nPixelWavNum; loop++)
			{
				//data2的波长分8组，每组间隔8个波长，data1的波长是按顺序排列
				for (int index = 0; index < stPixelData1.nPixelWavNum; index++)
				{
					//找到data2中和data1对应的波长
					if (fabs(stPixelData1.dblPixelWav[loop] - stPixelData2.dblPixelWav[index]) < 0.001)
					{
						break;
					}
				}
				if (index == stPixelData1.nPixelWavNum)
				{
					strTemp.Format("Can't find the IL of wave %.3fnm in stPixelData2", stPixelData1.dblPixelWav[loop]);
					throw strTemp.GetBuffer(0);
				}
				double dblOffset = stPixelData1.dblPixelIL[loop] - stPixelData2.dblPixelIL[index];
				pdblRatio1[loop] = pow(10, dblOffset / 10);
			}

			strFileName.Format("%s\\Pixel_ILRatio.csv", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			if (!SaveILRatio_Beam(strFileName, stPixelData1, (double*)&stPixelData1.dblPixelWav, (double*)&stPixelData1.dblPixelGhz, (double*)&stPixelData1.dblPixelIL, pdblRatio1, NULL))
			{
				strTemp.Format("Failed at saving file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			//分配内存
			pdblWave_Beam = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblGhz_Beam = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblIL_Beam = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblR1_Beam = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			pdblR2_Beam = (double*)VirtualAlloc(NULL, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			//数组清零
			ZeroMemory(pdblWave_Beam, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double));
			ZeroMemory(pdblGhz_Beam, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double));
			ZeroMemory(pdblIL_Beam, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double));
			ZeroMemory(pdblR1_Beam, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double));
			ZeroMemory(pdblR2_Beam, PIXEL_NUMBER_64*PIXEL_BEAM_NUMBER * sizeof(double));

			//8. 读取64个Pixel Beam文件
			nGroupNum = PIXEL_NUMBER_64 / (PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL);//扫描次数
			for (int m = 0; m < nGroupNum; m++)
			{
				strFileName.Format("%s\\Beam_ILRatio_loop%d.csv", strPath, m + 1);
				LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

				//数组清零
				ZeroMemory(pdblWaveBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblGhzBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblILBuf, dwSampleCount * sizeof(double));
				ZeroMemory(pdblRatio1, dwSampleCount * sizeof(double));
				ZeroMemory(pdblRatio2, dwSampleCount * sizeof(double));

				if (!fnReadBeamCsvFile(strFileName, pdblWaveBuf, pdblGhzBuf, pdblILBuf, pdblRatio1, pdblRatio2, &nDataNum))
				{
					strTemp.Format("读文件错误: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}

				//每个文件有8*401个数据
				for (int k = 0; k < PIXEL_NUMBER_64 / PIXEL_SET_INTERVAL; k++)
				{
					//pixel group: 1,9,17....; 2,10,18;......8,18,26....64
					int nStartPos = m*PIXEL_BEAM_NUMBER + k*PIXEL_SET_INTERVAL*PIXEL_BEAM_NUMBER;
					memcpy(&pdblWave_Beam[nStartPos], &pdblWaveBuf[k*PIXEL_BEAM_NUMBER], PIXEL_BEAM_NUMBER * sizeof(double));
					memcpy(&pdblGhz_Beam[nStartPos], &pdblGhzBuf[k*PIXEL_BEAM_NUMBER], PIXEL_BEAM_NUMBER * sizeof(double));
					memcpy(&pdblIL_Beam[nStartPos], &pdblILBuf[k*PIXEL_BEAM_NUMBER], PIXEL_BEAM_NUMBER * sizeof(double));
					memcpy(&pdblR1_Beam[nStartPos], &pdblRatio1[k*PIXEL_BEAM_NUMBER], PIXEL_BEAM_NUMBER * sizeof(double));
					memcpy(&pdblR2_Beam[nStartPos], &pdblRatio2[k*PIXEL_BEAM_NUMBER], PIXEL_BEAM_NUMBER * sizeof(double));
				}
			}

			//9. 保存64个Pixel Beam的汇总文件
			strFileName.Format("%s\\Beam_ILRatio.csv", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			if (!SaveILRatio_Beam(strFileName, stPixelData1, pdblWave_Beam, pdblGhz_Beam, pdblIL_Beam, pdblR1_Beam, pdblR2_Beam))
			{
				strTemp.Format("Failed at saving file: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}
			strTemp.Format("Pixel beam relation calibration finished !");
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
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
	if (pdblRatio1 != NULL)
	{
		VirtualFree(pdblRatio1, 0, MEM_RELEASE);
		pdblRatio1 = NULL;
	}
	if (pdblRatio2 != NULL)
	{
		VirtualFree(pdblRatio2, 0, MEM_RELEASE);
		pdblRatio2 = NULL;
	}

	if (pdblWave_Beam != NULL)
	{
		VirtualFree(pdblWave_Beam, 0, MEM_RELEASE);
		pdblWave_Beam = NULL;
	}
	if (pdblGhz_Beam != NULL)
	{
		VirtualFree(pdblGhz_Beam, 0, MEM_RELEASE);
		pdblGhz_Beam = NULL;
	}
	if (pdblIL_Beam != NULL)
	{
		VirtualFree(pdblIL_Beam, 0, MEM_RELEASE);
		pdblIL_Beam = NULL;
	}
	if (pdblR1_Beam != NULL)
	{
		VirtualFree(pdblR1_Beam, 0, MEM_RELEASE);
		pdblR1_Beam = NULL;
	}
	if (pdblR2_Beam != NULL)
	{
		VirtualFree(pdblR2_Beam, 0, MEM_RELEASE);
		pdblR2_Beam = NULL;
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::SaveCurTemperIL(LPCTSTR lpstPath, int nPoints, double *pdblIL, double *pdblFre)
{
	BOOL         bFunctionOK = TRUE;
	CString      strTemp;
	CString      strValue;
	CString      strFileName;
	FILE*        pFile = NULL;


	try
	{
		if (nPoints < 1)
		{
			strTemp.Format("IL Data can't be: %d", nPoints);
			throw strTemp.GetBuffer(0);
		}

		strFileName = lpstPath;

		//read the IL data
		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		for (int i = 0; i < nPoints; i++)
		{
			strTemp.Format("%.2f,%.1f\n", pdblIL[i], pdblFre[i] + 180000);
			fprintf(pFile, "%s", strTemp);
		}
	}
	catch (TCHAR* ptszErrorMsg)
	{
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::SaveCurTemperFreq(LPCTSTR lpstPath, int nLine, float *pfFreq)
{
	BOOL         bFunctionOK = TRUE;
	CString      strTemp;
	CString      strValue;
	CString      strFileName;
	FILE*        pFile = NULL;
	try
	{

		if (nLine < 1)
		{
			strTemp.Format("Freq Data can't be: %d", nLine);
			throw strTemp.GetBuffer(0);
		}

		strFileName = lpstPath;

		//read the IL data
		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		for (int i = 0; i < nLine; i++)
		{
			strTemp.Format("%.3f\n", pfFreq[i]);
			fprintf(pFile, "%s", strTemp);
		}
	}
	catch (TCHAR* ptszErrorMsg)
	{
		//		AfxMessageBox(ptszErrorMsg, MB_OK | MB_ICONERROR);
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		//		AfxMessageBox("Other fatal exception occurred",  MB_OK | MB_ICONERROR);
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::SaveCurTemperDac(LPCTSTR lpstPath, int nChannels, WORD *wDac)
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
		pFile = fopen(lpstPath, "w");
		if (NULL == pFile)
		{
			strTemp.Format("打开文件失败: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		for (int i = 0; i < nChannels; i++)
		{
			strTemp.Format("%d\n", wDac[i]);
			fprintf(pFile, "%s", strTemp);
		}

	}
	catch (TCHAR* ptszErrorMsg)
	{
		//		AfxMessageBox(ptszErrorMsg, MB_OK | MB_ICONERROR);
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		//		AfxMessageBox("Other fatal exception occurred",  MB_OK | MB_ICONERROR);
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::fnGetCurDataAndSave(int nModuleIndex, CString strItem)
{
	BOOL         bFunctionOK = TRUE;
	CString      strTemp;
	CString      strFileName;
	double       *pdblAtten = new double[IL_LUT_POINT];
	double       *pdblFre = new double[IL_LUT_POINT];
	float        *pfFreqBuf = new float[PIXEL_NUMBER_96];
	WORD         *pwDacBuf = new WORD[PIXEL_NUMBER_96];
	WORD         wPixelDac = 0;
	int          nLine = 0;
	CString strpath;

	try
	{
		strpath.Format("%s\\Test\\%s", m_DGEModule[nModuleIndex].m_strTestPath, m_DGEModule[nModuleIndex].m_strTempFolder);
		CreateFullPath(strpath);
		//1. 读取当前温度的IL
		memset(pdblAtten, 0, IL_LUT_POINT * sizeof(double));
		memset(pdblFre, 0, IL_LUT_POINT * sizeof(double));
		if (!m_DGEModule[nModuleIndex].fnGetCurTemperIL(pdblAtten, pdblFre, &nLine))
		{
			Sleep(2000);
			if (!m_DGEModule[nModuleIndex].fnGetCurTemperIL(pdblAtten, pdblFre, &nLine))
			{
				strTemp.Format("Error on saving Cur Temp IL!");
				throw strTemp.GetBuffer(0);
			}
		}

		//2. 保存当前温度的IL
		strFileName.Format("%s\\1k_Point_IL_%sC.csv", strpath, m_DGEModule[nModuleIndex].m_strTempFolder);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!SaveCurTemperIL(strFileName, nLine, pdblAtten, pdblFre))
		{
			strTemp.Format("Error on saving cal attenuation of 64 pixels!");
			throw strTemp.GetBuffer(0);
		}

		//3. 读取当前温度的波长频率
		memset(pfFreqBuf, 0, PIXEL_NUMBER_96 * sizeof(float));
		if (!m_DGEModule[nModuleIndex].fnGetCurTemperFreq(pfFreqBuf, &nLine))
		{
			Sleep(2000);
			if (!m_DGEModule[nModuleIndex].fnGetCurTemperFreq(pfFreqBuf, &nLine))
			{
				strTemp.Format("Error on getting freq of 96 pixels!");
				throw strTemp.GetBuffer(0);
			}
		}

		//4. 保存当前温度的波长频率
		strFileName.Format("%s\\Get_Pixel_Freq_%sC.csv", strpath, m_DGEModule[nModuleIndex].m_strTempFolder);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!SaveCurTemperFreq(strFileName, nLine, pfFreqBuf))
		{
			strTemp.Format("Error on saving freq of 96 pixels!");
			throw strTemp.GetBuffer(0);
		}

		//5. 读取当前温度的Pixel DAC
		memset(pwDacBuf, 0, PIXEL_NUMBER_96 * sizeof(WORD));
		for (int i = 0; i < PIXEL_NUMBER_64; i++)
		{
			if (!m_DGEModule[nModuleIndex].fnGetCurTemperDac(&wPixelDac, i + 1))
			{
				Sleep(2000);
				if (!m_DGEModule[nModuleIndex].fnGetCurTemperDac(&wPixelDac, i + 1))
				{
					strTemp.Format("Error on getting DAC of pixels!");
					throw strTemp.GetBuffer(0);
				}
			}

			//pixel DAC of channel
			pwDacBuf[i] = wPixelDac;
		}

		//6. 保存当前温度的Pixel DAC
		strFileName.Format("%s\\Get_Pixel_DAC_%sC_%s.csv", strpath, m_DGEModule[nModuleIndex].m_strTempFolder, strItem);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!SaveCurTemperDac(strFileName, PIXEL_NUMBER_64, pwDacBuf))
		{
			strTemp.Format("Error on saving freq of 64 pixels!");
			throw strTemp.GetBuffer(0);
		}

		bFunctionOK = TRUE;
	}
	catch (TCHAR* ptszErrorMsg)
	{
		CString strError;
		strError = ptszErrorMsg;
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	if (pdblAtten != NULL)
	{
		delete[] pdblAtten;
	}
	if (pdblFre != NULL)
	{
		delete[] pdblFre;
	}
	if (pfFreqBuf != NULL)
	{
		delete[] pfFreqBuf;
	}
	if (pwDacBuf != NULL)
	{
		delete[] pwDacBuf;
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::fnCreatReportFile(LPCTSTR lpstPath)
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

		fprintf(pFile, "\n", strValue);
	}
	catch (TCHAR* ptszErrorMsg)
	{
		CString strError;
		strError.Format("%s", ptszErrorMsg);
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	catch (...)
	{
		CString strError("Other fatal exception occurred");
		LogInfo(strError, strError.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}


BOOL CTestTaskManager::fnSaveReportFile(int nModuleIndex, LPCTSTR lpstPath, CString strIndex, double *dblData, DWORD dwScanCount)
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

		strTemp.Format("%sC, %s", m_DGEModule[nModuleIndex].m_strTempFolder, strIndex);
		//title 
		fprintf(pFile, "%s", strTemp);

		for (int nGhzIndex = 0; nGhzIndex < dwScanCount; nGhzIndex++)
		{
			strTemp.Format(",%lf", dblData[nGhzIndex]);
			fprintf(pFile, "%s", strTemp);
		}

		fprintf(pFile, "\n", strTemp);

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (NULL != pFile)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}




BOOL CTestTaskManager::fnReadPixelWaveCsv(LPCTSTR lpstPath, stPixelInfo *pstPixel)
{
	CString strTemp;
	CString strToken;
	int     nCount = 0;
	//CStdioFile  stdFile;
	FILE *fpFile = NULL;
	char    chRead[MAX_PATH];
	char    sep[] = ", \n";
	BOOL bFunctionOK = TRUE;
	ZeroMemory(chRead, MAX_PATH);

	try
	{
		//Open file
		fpFile = fopen(lpstPath, "r");
		if (fpFile == NULL)
		{
			strTemp.Format("open file error:%s ", lpstPath);
			throw strTemp.GetBuffer(0);
		}
		nCount = 0;
		while (1)
		{
			//from second line
			ZeroMemory(chRead, MAX_PATH);
			if (fgets(chRead, MAX_PATH, fpFile) == NULL)
			{
				break;
			}

			strToken = strtok(chRead, sep);
			pstPixel->dblPixelGhz[nCount] = atof(strToken) + 180000; //180000 GHZ

			nCount++;

		}

		pstPixel->nPixelWavNum = nCount;

		if (nCount != PIXEL_NUMBER_96)
		{
			strTemp.Format("Data num: %d is not same to Pixel num: %d", nCount, PIXEL_NUMBER_64);
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return bFunctionOK;
}


BOOL CTestTaskManager::fnSaveReportFile(int nModuleIndex, LPCTSTR lpstPath, BOOL bPDL, int nPixelCount, float *pfSetAtten, double *pdblAtten, int nLine, double *pdblGhz)
{
	CString strTemp;
	CString strValue;

	TCHAR		*token = NULL;
	FILE         *pFile = NULL;
	BOOL  bFunctionOK = TRUE;

	try
	{

		//读取文件
		if (lpstPath == "")
		{
			strTemp.Format("File name can't be empty: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		pFile = fopen(lpstPath, "a+");


		if (pFile == NULL)
		{
			strTemp.Format("Open file err: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		if (bPDL)
		{
			strTemp.Format("TEMP(C), PDL(dB)");
		}
		else
		{
			strTemp.Format("TEMP(C), IL(dB)");
		}
		//strTemp.Format("TEMP, IL");
		fprintf(pFile, "%s", strTemp);

		for (int i = 0; i < nPixelCount; i++)
		{
			strTemp.Format(",%lf", pdblGhz[i]);
			fprintf(pFile, "%s", strTemp);
		}

		fprintf(pFile, "\n", strTemp);

		for (int nIndex = 0; nIndex < nLine; nIndex++)
		{
			strTemp.Format("%s,%.2f", m_DGEModule[nModuleIndex].m_strTempFolder, pfSetAtten[nIndex]);
			fprintf(pFile, "%s", strTemp);

			for (int nAttenIndex = 0; nAttenIndex < nPixelCount; nAttenIndex++)
			{
				strTemp.Format(",%lf", pdblAtten[nIndex*nPixelCount + nAttenIndex]);
				fprintf(pFile, "%s", strTemp);
			}

			fprintf(pFile, "\n", strTemp);
		}


	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (NULL != pFile)
	{
		fclose(pFile);
	}

	return TRUE;
}

BOOL CTestTaskManager::fnReadChScanFile(int nModuleIndex, LPCTSTR lpstPath, double *dblIL, double *dblPDL, double *dblGhz)
{
	CString strDir, strSaveDir, strSaveDir1;
	CString strTemp;
	char szBuffer[MAX_PATH];
	char *token = NULL;


	FILE * fpFile = NULL;


	fpFile = fopen(lpstPath, "r");
	if (fpFile == NULL)
	{
		strTemp.Format("Open file error:%s", lpstPath);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
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



BOOL CTestTaskManager::fnSavePixelData(int nModuleIndex)
{

	CString strTemp;
	CString strFileName;
	double  *pdblIL = NULL;
	double  *pdblPDL = NULL;
	double  *pdblGhz = NULL;
	DWORD   dwScanCount;
	int     nLine = 0;
	stPixelInfo  stPixelData;
	BOOL  bFunctionOK = TRUE;
	double  *pdblAtten;
	float   *pfAttenBuf;
	double  *pdblScanGhz;
	CString strpath;
	try
	{
		dwScanCount = m_stScanParameter.dwSampleCount;
		pdblAtten = (double*)VirtualAlloc(NULL, ATTEN_LUT_POINT*dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pfAttenBuf = (float*)VirtualAlloc(NULL, ATTEN_LUT_POINT * sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblScanGhz = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


		memset(pdblAtten, 0, ATTEN_LUT_POINT*dwScanCount * sizeof(double));
		memset(pfAttenBuf, 0, ATTEN_LUT_POINT * sizeof(float));
		memset(pdblScanGhz, 0, dwScanCount * sizeof(double));

		pdblGhz = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblIL = (double*)VirtualAlloc(NULL, dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdblPDL = (double*)VirtualAlloc(NULL, ATTEN_LUT_POINT*dwScanCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		ZeroMemory(pdblGhz, dwScanCount * sizeof(double));
		ZeroMemory(pdblIL, dwScanCount * sizeof(double));
		ZeroMemory(pdblPDL, ATTEN_LUT_POINT*dwScanCount * sizeof(double));

		strpath.Format("%s\\Test\\%s", m_DGEModule[nModuleIndex].m_strTestPath, m_DGEModule[nModuleIndex].m_strTempFolder);

		strFileName.Format("%s\\Pixel_Atten_Scan_Ver.csv", strpath);
		if (!fnReadAttenVsGhz(strFileName, dwScanCount, pfAttenBuf, pdblAtten, &nLine, pdblScanGhz))
		{
			strTemp.Format("读取中心波长衰减定标值失败!");
			throw strTemp.GetBuffer(0);
		}

		strFileName.Format("%s\\Get_Pixel_Freq_%sC.csv", strpath, m_DGEModule[nModuleIndex].m_strTempFolder);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		ZeroMemory(&stPixelData, sizeof(stPixelData));
		if (!fnReadPixelWaveCsv(strFileName, &stPixelData)) //全部中心波长
		{
			strTemp.Format("Failed at reading the pixel wavelength ini file!");
			throw	strTemp.GetBuffer(0);
		}

		strFileName.Format("%s\\Pixel_Atten_Ver.csv", strpath);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!SaveCaliPixelAtten(strFileName, nLine, &stPixelData.dblPixelGhz[0], pfAttenBuf, pdblScanGhz, pdblAtten))
		{
			strTemp.Format("Error on saving cal attenuation of 64 pixels!");
			throw strTemp.GetBuffer(0);
		}

		memset(pdblAtten, 0, ATTEN_LUT_POINT*PIXEL_NUMBER_64 * sizeof(double));
		memset(pfAttenBuf, 0, ATTEN_LUT_POINT * sizeof(float));
		memset(pdblScanGhz, 0, PIXEL_NUMBER_64 * sizeof(double));

		if (!fnReadAttenVsGhz(strFileName, (DWORD)PIXEL_NUMBER_64, pfAttenBuf, pdblAtten, &nLine, pdblScanGhz))
		{
			strTemp.Format("读取中心波长衰减定标值失败!");
			throw strTemp.GetBuffer(0);
		}

		//保存PIXEL IL汇总扫描数据
		strFileName.Format("%s\\Test\\Atten_IL_Report.csv", m_DGEModule[nModuleIndex].m_strTestPath);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		//IL
		if (!fnSaveReportFile(nModuleIndex, strFileName, FALSE, PIXEL_NUMBER_64, pfAttenBuf, pdblAtten, nLine, pdblScanGhz))
		{
			strTemp.Format("Save file error: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}

		memset(pdblGhz, 0, dwScanCount * sizeof(double));
		memset(pdblIL, 0, dwScanCount * sizeof(double));
		memset(pdblPDL, 0, ATTEN_LUT_POINT*dwScanCount * sizeof(double));

		for (int nloop = 0; nloop < nLine; nloop++)
		{
			strFileName.Format("%s\\Pixel_Atten_Scan_Ver_%d.txt", strpath, nloop + 1);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			if (!fnReadChScanFile(nModuleIndex, strFileName, pdblIL, pdblPDL + nloop*dwScanCount, pdblGhz))
			{
				strTemp.Format("Read file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}
		}

		//7. 保存64个pixel的PDL值
		strFileName.Format("%s\\Pixel_PDL_Ver.csv", strpath);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!SaveCaliPixelAtten(strFileName, nLine, &stPixelData.dblPixelGhz[0], pfAttenBuf, pdblGhz, pdblPDL))
		{
			strTemp.Format("Error on saving cal attenuation of 64 pixels!");
			throw strTemp.GetBuffer(0);
		}

		memset(pdblAtten, 0, ATTEN_LUT_POINT*PIXEL_NUMBER_64 * sizeof(double));
		memset(pfAttenBuf, 0, ATTEN_LUT_POINT * sizeof(float));
		memset(pdblScanGhz, 0, PIXEL_NUMBER_64 * sizeof(double));

		if (!fnReadAttenVsGhz(strFileName, (DWORD)PIXEL_NUMBER_64, pfAttenBuf, pdblAtten, &nLine, pdblScanGhz))
		{
			strTemp.Format("读取中心波长衰减定标值失败!");
			throw strTemp.GetBuffer(0);
		}

		//保存PIXEL PDL汇总扫描数据
		strFileName.Format("%s\\Test\\Atten_IL_Report.csv", m_DGEModule[nModuleIndex].m_strTestPath);
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

		if (!fnSaveReportFile(nModuleIndex, strFileName, TRUE, PIXEL_NUMBER_64, pfAttenBuf, pdblAtten, nLine, pdblScanGhz))
		{
			strTemp.Format("Save file error: %s", strFileName);
			throw strTemp.GetBuffer(0);
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
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
	if (pdblGhz != NULL)
	{
		VirtualFree(pdblGhz, 0, MEM_RELEASE);
		pdblGhz = NULL;
	}
	return TRUE;
}


BOOL CTestTaskManager::fnSaveScanChFiles(int nModuleIndex, CString strTargetFileName)
{
	CString strDir, strSaveDir, strSaveDir1;
	CString strTemp;
	char szBuffer[MAX_PATH];
	double *pdbWavArry;
	double *pdbPMArry;
	double *pdbPDLArry;


	FILE * fpFile = NULL;

	pdbWavArry = m_FSTPObj.m_stScanResult.pdbWLArry;
	pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[nModuleIndex + 1];
	pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[nModuleIndex + 1];

	fpFile = fopen(strTargetFileName, "w");
	if (fpFile == NULL)
	{
		strTemp.Format("Open file error:%s", strTargetFileName);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	if (m_stScanParameter.bDoPDL)
	{
		ZeroMemory(szBuffer, MAX_PATH);
		sprintf(szBuffer, "WL,Fre,IL,PDL");
		fprintf(fpFile, "%s\n", szBuffer);

		for (int i = 0; i < m_stScanParameter.dwSampleCount; i++)
		{
			ZeroMemory(szBuffer, MAX_PATH);
			sprintf(szBuffer, "%lf,%lf,%lf,%lf", pdbWavArry[i], VELOCITY_LIGHT / pdbWavArry[i], pdbPMArry[i], pdbPDLArry[i]);
			fprintf(fpFile, "%s\n", szBuffer);
		}
	}
	else
	{
		ZeroMemory(szBuffer, MAX_PATH);
		sprintf(szBuffer, "WL,Fre,IL");
		fprintf(fpFile, "%s\n", szBuffer);

		for (int i = 0; i < m_stScanParameter.dwSampleCount; i++)
		{
			ZeroMemory(szBuffer, MAX_PATH);
			sprintf(szBuffer, "%lf,%lf,%lf", pdbWavArry[i], VELOCITY_LIGHT / pdbWavArry[i], pdbPMArry[i]);
			fprintf(fpFile, "%s\n", szBuffer);
		}
	}

	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}

	return TRUE;
}



BOOL CTestTaskManager::CreateAttenScanVerFile(LPCSTR lpstrFileName)
{
	FILE    *pFile = NULL;
	double  dblStartWL = 0;
	double  dblStep = 0;
	CString  strTemp;
	CString strValue;
	pFile = fopen(lpstrFileName, "w");
	if (NULL == pFile)
	{
		strTemp.Format("Create file failed: %s", lpstrFileName);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	strTemp.Format("Atten");
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
	fprintf(pFile, "\n", strTemp);

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return TRUE;
}


void CTestTaskManager::AddPlotCount(int nGraphIndex, LPCSTR lpstrCaption)
{
	m_pGraph->PlotAdd(nGraphIndex, RGB(rand() % 255, rand() % 255, rand() % 255), lpstrCaption);
}

BOOL CTestTaskManager::fnVer_PixelAtten()
{
	CString strTemp;
	CString strValue;
	CString strFileName;
	CString strMsg;

	int     nSetNumber = 0;
	float   fAtten = 0;
	FILE    *pFile = NULL;
	double  dblStartWL = 0;
	double  dblStep = 0;
	CString  strpath;


	try
	{

		strTemp.Format("开始Pixel Atten Verification验证扫描测试......");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		InitGraphShow(0, "Wavelength(nm)", "IL(dB)", "DGE衰减验证");//IL
		InitGraphShow(1, "Wavelength(nm)", "PDL(dB)", "DGE衰减验证");//PDL


		//2. 创建Atten文件
		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//if (!JudgeCurrentSlotSN(nIndex))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			strTemp.Format("DGE模块%d", nIndex + 1);
			AddPlotCount(0, strTemp);
			AddPlotCount(1, strTemp);

			strpath.Format("%s\\Test\\%s", m_DGEModule[nIndex].m_strTestPath, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strpath);
			strFileName.Format("%s\\Pixel_Atten_Scan_Ver.csv", strpath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			if (!CreateAttenScanVerFile(strFileName))
			{
				throw "创建Pxiel_Aten_Scan_Ver.csv文件存在错误";
			}

			if (!m_DGEModule[nIndex].fnReadAttenVerCfg(nSetNumber))
			{
				strTemp.Format("从Atten验证配置文件中读取配置信息存在错误");
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
				m_DGEModule[nIndex].m_bConnect = FALSE;
				continue;
			}
		}

		//3. loop scan for many time against differenc DAC setting
		for (int nloop = 0; nloop < nSetNumber; nloop++)
		{
			for (int nModule = 0; nModule < m_nDGECount; nModule++)
			{
				fAtten = m_DGEModule[nModule].m_fAttenCfg[nloop];

				strTemp.Format("第%d次设置%d模块custom mode", nloop + 1, nModule + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
				if (!m_DGEModule[nModule].fnSetCustomMode())
				{
					OpenSerialPortDevice();
					Sleep(1000);
					if (!m_DGEModule[nModule].fnSetCustomMode())
					{
						strTemp = m_DGEModule[nModule].m_strErrorMsg;
						throw strTemp.GetBuffer(0);
					}
				}
				strTemp.Format("第%d次设置Pixel所有通道: Atten = %.2fdB", nloop + 1, fAtten);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
				if (!m_DGEModule[nModule].fnSetAllPixelAttenForVer(nloop))
				{
					OpenSerialPortDevice();
					Sleep(1000);
					if (!m_DGEModule[nModule].fnSetAllPixelAttenForVer(nloop))
					{
						strTemp = m_DGEModule[nModule].m_strErrorMsg;
						throw strTemp.GetBuffer(0);
					}
				}
			}

			Sleep(3000);
			strTemp.Format("开始%d次扫描......", nloop + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
			WORD wAtten = (WORD)fAtten;
			if (!fnPixelAttenScan(wAtten, "Pixel_Atten_Scan_Ver", 1))
			{
				strTemp.Format("Pixel atten scan Error");
				throw strTemp.GetBuffer(0);
			}
			strTemp.Format("%d次扫描完成......", nloop + 1);
			for (nModule = 0; nModule < m_nDGECount; nModule++)
			{
				//scan result
				strpath.Format("%s\\Test\\%s", m_DGEModule[nModule].m_strTestPath, m_DGEModule[nModule].m_strTempFolder);
				strFileName.Format("%s\\Pixel_Atten_Scan_Ver_%d.txt", strpath, nloop + 1);
				if (!fnSaveScanChFiles(nModule, strFileName))
				{
					strTemp.Format("Copy file error: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}
				strTemp.Format("Atten-%.2fdB", fAtten);
				if (!fnGetCurDataAndSave(nModule, strTemp))
				{
					strTemp.Format("Save current status data failed!");
					throw strTemp.GetBuffer(0);
				}
			}
		}

		for (int nModule = 0; nModule < m_nDGECount; nModule++)
		{
			//4. save total scan IL and PDL data
			if (!m_DGEModule[nModule].fnSaveILScanData())
			{
				strTemp.Format("Save total scan data failed!");
				throw strTemp.GetBuffer(0);
			}
			if (!fnSavePixelData(nModule))
			{
				strTemp.Format("Save total pixel data failed!");
				throw strTemp.GetBuffer(0);
			}

			double dblTemper = 0;
			if (!fnGetTemperWriteIni(nModule, dblTemper, "ATTEN_VER", FALSE))
			{
				strTemp.Format("读取LC温度错误!");
				throw strTemp.GetBuffer(0);
			}
		}
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}


	if (NULL != pFile)
	{
		fclose(pFile);
	}


	strTemp.Format("Pixel attenuation Verification finished!");
	LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

	return TRUE;
}

BOOL CTestTaskManager::fnOECurveScan(int nIndex)
{
	CString strTemp;
	CString strFileName;
	int      nCount = 0;
	CString strPath;
	try
	{
		if (!fnTestScanOne())
			return FALSE;

		for (int nModule = 0; nModule < m_nDGECount; nModule++)
		{
			strPath.Format("%s\\Test\\%s", m_DGEModule[nModule].m_strTestPath, m_DGEModule[nModule].m_strTempFolder);
			strFileName.Format("%s\\OE_Curve_Scan_Ver.csv", strPath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!SaveScanAttenData(strFileName, nModule + 1, (float)nIndex))
			{
				throw " save the result raw data to file error";
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}


BOOL CTestTaskManager::CreateEOCurveScanVerFile(LPCSTR lpstrFileName)
{
	FILE    *pFile = NULL;
	double  dblStartWL = 0;
	double  dblStep = 0;
	CString  strTemp;
	CString strValue;
	pFile = fopen(lpstrFileName, "w");
	if (NULL == pFile)
	{
		strTemp.Format("Create file failed: %s", lpstrFileName);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	strTemp.Format("Curve_Index");
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
	fprintf(pFile, "\n", strTemp);
	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return TRUE;
}
BOOL CTestTaskManager::fnVer_OECurve()
{
	CString strTemp;
	CString strFileName;
	CString strMsg;
	CString strValue;
	int     nLine = 0;
	FILE    *pFile = NULL;
	double  dblStartWL = 0;
	double  dblStep = 0;

	CString strSection;
	CString strKey;
	CString strCurvePlan;
	CString strFreq;
	CString strIL;
	char    chBuf[MAX_PATH];
	int     nCurveTemplateNum = 0;
	int     nFlatnessCurvePoint = 0;
	CString strPath;
	DWORD  dwScanCount;
	float   *pfCurveFreq = NULL;
	float   *pfCurveIL = NULL;
	BOOL bFunctionOK = TRUE;
	CString strTemplatePath;
	CString strpath;

	try
	{
		dwScanCount = m_stScanParameter.dwSampleCount;
		pfCurveFreq = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pfCurveIL = (float*)VirtualAlloc(NULL, sizeof(float)*IL_LUT_POINT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		strTemp.Format("开始OE Curve Verification验证扫描测试......");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//if (!JudgeCurrentSlotSN(nIndex))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			strpath.Format("%s\\Test\\%s", m_DGEModule[nIndex].m_strTestPath, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strpath);
			strFileName.Format("%d模块SN：%s", nIndex + 1, m_DGEModule[nIndex].m_DGEInfo.strSN);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);

			if (!m_DGEModule[nIndex].ReadCurveCountCfgFromFile())
			{
				strTemp.Format("%s", m_DGEModule[nIndex].m_strErrorMsg);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
				m_DGEModule[nIndex].m_bConnect = FALSE;
				continue;
			}
			nCurveTemplateNum = m_DGEModule[nIndex].m_nCurveTemplateNum;

			//2. 创建Curve汇总文件
			strFileName.Format("%s\\Test\\%s\\OE_Curve_Scan_Ver.csv", m_DGEModule[nIndex].m_strTestPath, m_DGEModule[nIndex].m_strTempFolder);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL);
			if (!CreateEOCurveScanVerFile(strFileName))
			{
				strTemp.Format("%d模块创建OE_Curve_Scan_Ver.csv文件存在错误", nIndex + 1);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
				m_DGEModule[nIndex].m_bConnect = FALSE;
				continue;
			}

			m_DGEModule[nIndex].ClearCurveScanVerMemory();
		}
		//3. loop scan for many time against differenc DAC setting
		for (int nloop = 0; nloop < nCurveTemplateNum; nloop++)
		{
			//Set all pixels to open
			strTemp.Format("第%d次设置Pixel Target Curve: Index = %d", nloop + 1, nloop + 1);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
			for (int j = 0; j < m_nDGECount; j++)
			{
				if (m_DGEModule[j].m_bConnect)
				{
					if (!m_DGEModule[j].fnSetDgeOECurveForVer(nloop))
					{
						OpenSerialPortDevice();
						Sleep(1000);
						if (!m_DGEModule[j].fnSetDgeOECurveForVer(nloop))
						{
							strTemp = m_DGEModule[j].m_strErrorMsg;
							LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
							m_DGEModule[j].m_bConnect = FALSE;
							continue;
						}
					}
				}
			}

			fnSleepLongTime(5000);

			//scan result
			if (!fnOECurveScan(nloop + 1))
			{
				strTemp.Format("Pixel atten scan Error");
				throw strTemp.GetBuffer(0);
			}

			for (int nModule = 0; nModule < m_nDGECount; nModule++)
			{
				strPath.Format("%s\\Test\\%s", m_DGEModule[nModule].m_strTestPath, m_DGEModule[nModule].m_strTempFolder);
				strFileName.Format("%s\\OE_Curve_Scan_%d.txt", strPath, nloop + 1);
				if (!fnSaveScanChFiles(nModule, strFileName))
				{
					strTemp.Format("Copy file error: %s", strFileName);
					throw strTemp.GetBuffer(0);
				}

				strTemp.Format("Curve-%d", nloop + 1);
				if (!fnGetCurDataAndSave(nModule, strTemp))
				{
					strTemp.Format("Save current status data failed!");
					throw strTemp.GetBuffer(0);
				}
			}
		}

		for (nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			strPath.Format("%s\\Test\\%s", m_DGEModule[nIndex].m_strTestPath, m_DGEModule[nIndex].m_strTempFolder);

			if (!m_DGEModule[nIndex].fnSaveCurvePDLScanData())
			{
				strTemp.Format("Save PDL data failed!");
				throw strTemp.GetBuffer(0);
			}

			if (!m_DGEModule[nIndex].fnSavedCurveScanVer())
			{
				strTemp = m_DGEModule[nIndex].m_strErrorMsg;
				throw strTemp.GetBuffer(0);
			}

			double dblTemper = 0;
			if (!fnGetTemperWriteIni(nIndex, dblTemper, "CURVE_VER", FALSE))
			{
				strTemp.Format("读取LC温度错误!");
				throw strTemp.GetBuffer(0);
			}
		}
		strTemp = "Test EO curve finished !";
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (pfCurveFreq != NULL)
	{
		VirtualFree(pfCurveFreq, 0, MEM_RELEASE);
	}
	if (pfCurveIL != NULL)
	{
		VirtualFree(pfCurveIL, 0, MEM_RELEASE);
	}

	return bFunctionOK;
}


///////////////////////////////////////////////////////////////////
//函数说明：
//    根据WL Shift扫描数据计算对应通道的中心频率
//Add by wanxin
//Time:20180726
//////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnCalcPixcelCenterGHz(double *pdbFre, double *pdbIL, int nSampleCount, double *pdbPixelWLCenter, double *pdbPixelCenterIL, int *nGetPixelCount)
{
	CString strTemp;
	double dblGetGhz;
	double dblGetIL;
	double dblBasePointOffet = 10.0;
	double dblTarGetOffset = 1.0;

	int    nPixelIndex_Peak[PIXEL_NUMBER_64];
	double dblPixelGhz_Peak[PIXEL_NUMBER_64];
	double dblPixelIL_Peak[PIXEL_NUMBER_64];
	//	double dblPixel_Center[PIXEL_NUMBER_64];
	double dblPeakIL;
	double dblTempData = 0;
	double dblMinIL = 999;
	double dblMaxIL = -999;
	BOOL   bFindBasePoint = FALSE;
	int nIndex2 = 0;
	int nPixelCount = 0;
	int nIndex_MinIL = 0;
	double dblBasePoint;
	double dblDeltaIL;
	BOOL bSearchBegin = FALSE;
	int nIndex_Pixel = 0;
	double dblLeftPixelIL;
	double dblLeftPixelGhz;
	double dblRightPixelIL;
	double dblRightPixelGhz;
	//	int nIndexOffset;

	BOOL bFindFangxiang = FALSE;
	double dbAllPointMinIL = 0.0;


	//找到扫描曲线中最小插损，以最小插损-4dB作为baseline
	for (int nIndex = 1000; nIndex < nSampleCount - 1000; nIndex++)
	{
		dblGetIL = pdbIL[nIndex];
		if (dblGetIL < dblMinIL)
		{
			dblMinIL = dblGetIL; //Find Minmum IL
			dbAllPointMinIL = dblMinIL;
		}
	}

	dblBasePointOffet = dbAllPointMinIL + 5; //考虑到WDL影响
	dblTarGetOffset = 2.0;

	dblMinIL = 999;
	dblMaxIL = -999;
	for (nIndex = 0; nIndex < nSampleCount; nIndex++)
	{
		dblGetGhz = pdbFre[nIndex];
		dblGetIL = pdbIL[nIndex];

		if (dblGetIL < dblMinIL)
		{
			dblMinIL = dblGetIL; //Find Minmum IL
			nIndex2 = 0; //重新开始
			nIndex_MinIL = nIndex;
		}

		if (!bFindBasePoint)
		{
			if (fabs(dblGetIL - dblBasePointOffet) < 0.2) //base line
			{
				bFindBasePoint = TRUE;
				dblBasePoint = dblGetIL;
				dblMinIL = 999; //找到门限值后再找最小IL点
			}
			else
			{
				continue;
			}
		}


		if (!bFindFangxiang)
		{
			dblDeltaIL = dblBasePoint - dblGetIL;

			if (dblDeltaIL > dblTarGetOffset)//因抖动取消方向判断
			{
				bSearchBegin = TRUE; //Flag for every pixel search beginning
				bFindFangxiang = TRUE;
			}
			else
			{
				bSearchBegin = FALSE;
				nIndex2 = 0; //重新开始
			}
		}


		if (bSearchBegin) //开始查找，从第一个Pixel开始
		{
			if (dblGetIL > dblMinIL)
			{
				nIndex2++; //找IL的拐点
			}
			else
			{
				nIndex2 = 0; //重新开始
			}

			if (nIndex2 > 10 && ((dblGetIL - dblMinIL) > 4.0) && (dblMinIL < dblBasePoint)) //连续10个点的IL变大
			{
				dblPixelGhz_Peak[nPixelCount] = pdbFre[nIndex - nIndex2];
				dblPixelIL_Peak[nPixelCount] = pdbIL[nIndex - nIndex2];
				dblPeakIL = pdbIL[nIndex - nIndex2];
				nIndex_Pixel = nIndex - nIndex2; //current peak pixel index

				nPixelIndex_Peak[nPixelCount] = nIndex_Pixel;

				dblMinIL = 999;
				dblMaxIL = -999;
				nIndex2 = 0; //重新开始下一个Pixel
				bFindFangxiang = FALSE;

				strTemp.Format("第%d个Pixel Peak: Index=%d; IL=%.3f, Ghz=%.3f",
					nPixelCount + 1,
					nIndex_Pixel,
					dblPeakIL,
					dblPixelGhz_Peak[nPixelCount]);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
			}
			else
			{
				continue; //继续下一个点
			}

			//找到PeakIL点以后，开始+/-3dB查找中心波长
			for (int nIndex3 = nIndex_Pixel; nIndex3 >= 0; nIndex3--)
			{
				if (fabs(pdbIL[nIndex3] - dblPeakIL) > 3.0)
				{
					dblLeftPixelIL = pdbIL[nIndex3];
					dblLeftPixelGhz = pdbFre[nIndex3];
					break;
				}
			}

			for (nIndex3 = nIndex_Pixel; nIndex3 < nSampleCount; nIndex3++)
			{
				if (fabs(pdbIL[nIndex3] - dblPeakIL) > 3.0)
				{
					dblRightPixelIL = pdbIL[nIndex3];
					dblRightPixelGhz = pdbFre[nIndex3];
					break;
				}
			}

			pdbPixelWLCenter[nPixelCount] = (dblLeftPixelGhz + dblRightPixelGhz) / 2;
			pdbPixelCenterIL[nPixelCount] = dblPeakIL;

			strTemp.Format("Calculated第%d个Pixel Center: IL=%.3f, Ghz=%.3f",
				nPixelCount + 1,
				pdbPixelCenterIL[nPixelCount],
				pdbPixelWLCenter[nPixelCount]);

			nPixelCount++;
			bSearchBegin = FALSE; //直到下一个Pixel开始为TRUE
			bFindBasePoint = FALSE;
		}
	}
	*nGetPixelCount = nPixelCount++;

	return TRUE;
}

////////////////////////////////////////////////////////////////////
//函数说明：
//    保存和处理WL Shift数据
//Add by wanxin
//Time:20180726
////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnSaveVerWLShiftData(int nModuleIndex)
{
	BOOL bFuntionOK = TRUE;
	CString strFileName;
	CString strTemp;
	double *pdbFre = NULL;
	double *pdbIL = NULL;
	double *pdbPDL = NULL;
	double dbPixelCenter[PIXEL_NUMBER_64];
	double dbPixelCenterIL[PIXEL_NUMBER_64];
	char szBuffer[MAX_PATH];
	float pfFreqBuf[PIXEL_NUMBER_96];

	int nGetPixelCenterCount = 0;
	FILE * fpFile = NULL;
	int nLine = 0;
	DWORD dwSampleCount = m_stScanParameter.dwSampleCount;
	try
	{

		ZeroMemory(dbPixelCenter, sizeof(double)*PIXEL_NUMBER_64);
		ZeroMemory(dbPixelCenterIL, sizeof(double)*PIXEL_NUMBER_64);

		pdbFre = (double*)VirtualAlloc(NULL, sizeof(double)*dwSampleCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdbIL = (double*)VirtualAlloc(NULL, sizeof(double)*dwSampleCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		pdbPDL = (double*)VirtualAlloc(NULL, sizeof(double)*dwSampleCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		strFileName.Format("%s\\Test\\%s\\Pixel_WL_Shift_Ver_1.txt", m_DGEModule[nModuleIndex].m_strTestPath, m_DGEModule[nModuleIndex].m_strTempFolder);
		if (!fnReadChScanFile(nModuleIndex + 1, strFileName, pdbIL, pdbPDL, pdbFre))
		{
			throw"Read WL shift scan data from file error!";
		}
		//根据扫描数据，寻找对应pixel中心波长
		if (!m_DGEModule[nModuleIndex].fnCalcPixcelCenterGHzNew(pdbFre, pdbIL, dwSampleCount, dbPixelCenter, dbPixelCenterIL, &nGetPixelCenterCount))
		{
			throw"Calculate pixel center GHz error!";
		}
		//根据设置的模板确定计算pixel center，对应Pixel序号
		strTemp.Format("Read WL Shift Verification CFG File");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
		if (!m_DGEModule[nModuleIndex].fnReadDGEWLShiftVerifyCfg())
		{
			strTemp.Format("读取波长shift验证配置文件存在错误");
			throw strTemp.GetBuffer(0);
		}

		ZeroMemory(pfFreqBuf, sizeof(float)*PIXEL_NUMBER_96);
		if (!m_DGEModule[nModuleIndex].fnGetCurTemperFreq(pfFreqBuf, &nLine))
		{
			strTemp.Format("Error on getting freq of 96 pixels!");
			throw strTemp.GetBuffer(0);
		}

		strFileName.Format("%s\\Test\\%s\\Pixel_WL_Shift_Ver_Result.csv", m_DGEModule[nModuleIndex].m_strTestPath, m_DGEModule[nModuleIndex].m_strTempFolder);

		if ((fpFile = fopen(strFileName.GetBuffer(0), "w")) == NULL)
		{
			strTemp.Format("Open file %s error!", strFileName);
			throw strTemp.GetBuffer(0);
		}
		ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);
		sprintf(szBuffer, "PixelIndex,CalFre,IL,ReadFre");
		fprintf(fpFile, "%s\n", szBuffer);

		int nIndex = 0;
		int nReverIndex;
		nReverIndex = nGetPixelCenterCount;

		for (int n = 0; n < m_DGEModule[nModuleIndex].m_nCfgWLShiftLineCount; n++)
		{
			if (fabs(m_DGEModule[nModuleIndex].m_stWLShiftCfg[n].dbAttenValue - 0) < 0.0001)//如果是开的状态
			{
				for (int i = 0; i < nGetPixelCenterCount; i++)
				{
					double dbCalFre = VELOCITY_LIGHT / dbPixelCenter[i];
					double dbGetFre = pfFreqBuf[n] + 180000;
					if (fabs(dbCalFre - dbGetFre) < 15.0)
					{
						ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);
						sprintf(szBuffer, "%d,%lf,%lf,%lf", m_DGEModule[nModuleIndex].m_stWLShiftCfg[n].nPixelIndex, VELOCITY_LIGHT / dbPixelCenter[i], dbPixelCenterIL[i], pfFreqBuf[n] + 180000);
						fprintf(fpFile, "%s\n", szBuffer);
						break;
					}
				}
			}
		}
		if (fpFile != NULL)
		{
			fclose(fpFile);
			fpFile = NULL;
		}


		//保存到汇总的文件中
		//保存当前温度的结果文件
		strFileName.Format("%s\\Test\\Pixel_WL_Shift_Ver_Report.csv", m_DGEModule[nModuleIndex].m_strTestPath);

		if ((fpFile = fopen(strFileName.GetBuffer(0), "a+")) == NULL)
		{
			strTemp.Format("Open file %s error!", strFileName);
			throw strTemp.GetBuffer(0);
		}
		ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);
		sprintf(szBuffer, "Temp,%s,PixelIndex,CalFre,IL,ReadFre", m_DGEModule[nModuleIndex].m_strTempFolder);
		fprintf(fpFile, "%s\n", szBuffer);

		nReverIndex = nGetPixelCenterCount;


		for (n = 0; n < m_DGEModule[nModuleIndex].m_nCfgWLShiftLineCount; n++)
		{
			if (fabs(m_DGEModule[nModuleIndex].m_stWLShiftCfg[n].dbAttenValue - 0) < 0.0001)//如果是开的状态
			{
				for (int i = 0; i < nGetPixelCenterCount; i++)
				{
					double dbCalFre = VELOCITY_LIGHT / dbPixelCenter[i];
					double dbGetFre = pfFreqBuf[n] + 180000;
					if (fabs(dbCalFre - dbGetFre) < 15.0)
					{
						ZeroMemory(szBuffer, sizeof(char)*MAX_PATH);
						sprintf(szBuffer, "Temp,%s,%d,%lf,%lf,%lf", m_DGEModule[nModuleIndex].m_strTempFolder, m_DGEModule[nModuleIndex].m_stWLShiftCfg[n].nPixelIndex, VELOCITY_LIGHT / dbPixelCenter[i], dbPixelCenterIL[i], pfFreqBuf[n] + 180000);
						fprintf(fpFile, "%s\n", szBuffer);
						break;
					}
				}
			}
		}
		if (fpFile != NULL)
		{
			fclose(fpFile);
			fpFile = NULL;
		}
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFuntionOK = FALSE;
	}
	if (fpFile != NULL)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	if (pdbFre != NULL)
	{
		VirtualFree(pdbFre, 0, MEM_RELEASE);
		pdbFre = NULL;
	}
	if (pdbIL != NULL)
	{
		VirtualFree(pdbIL, 0, MEM_RELEASE);
		pdbIL = NULL;
	}
	if (pdbPDL != NULL)
	{
		VirtualFree(pdbPDL, 0, MEM_RELEASE);
		pdbPDL = NULL;
	}

	return bFuntionOK;
}


////////////////////////////////////////////////////////////////////////////////////////
//函数说明:
//    按照设置的Atten Cfg File进行扫描
//Add by wanxin
//Time:20180726
//////////////////////////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnAllPixelAttenScan(LPCTSTR lpstPath, float *fAtten)
{
	CString strTemp;
	CString strFileName;
	int      nCount = 0;
	CString strpath;

	try
	{
		strTemp.Format("Setting all pixel atten from cfg file");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
		for (int nModule = 0; nModule < m_nDGECount; nModule++)
		{
			if (!m_DGEModule[nModule].fnSetAllPixelAtten(fAtten, PIXEL_NUMBER_64))
			{
				OpenSerialPortDevice();
				Sleep(1000);
				if (!m_DGEModule[nModule].fnSetAllPixelAtten(fAtten, PIXEL_NUMBER_64))
				{
					strTemp = m_DGEModule[nModule].m_strErrorMsg;
					throw strTemp.GetBuffer(0);
				}
			}
		}
		Sleep(3000);

		if (!fnTestScanOne())
			throw "Test scan error";

		for (nModule = 0; nModule < m_nDGECount; nModule++)
		{
			strpath.Format("%s\\Test\\%s", m_DGEModule[nModule].m_strTestPath, m_DGEModule[nModule].m_strTempFolder);
			strFileName.Format("%s\\Pixel_WL_Shift_Scan_Ver.csv", strpath);

			if (!SaveScanAttenData(strFileName, nModule + 1, 255.0))
			{
				throw " save the result raw data to file error";
			}
		}
	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
//函数说明：
//    通过1开7关设置Attenation来进行验证某些通道的pixel中心波长
//Add by wanxin
//Time:20180726
///////////////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnVer_WLShift()
{
	CString strTemp;
	CString strValue;
	CString strFileName;
	CString strMsg;
	float   fAttenSet[PIXEL_NUMBER_96] = { 0 };
	CString strPath;

	try
	{

		strTemp.Format("Begin Pixel WL Shift Verification Scan......");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);


		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			/*		if (!JudgeCurrentSlotSN(nIndex))
					{
						strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
						throw strTemp.GetBuffer(0);
					}*/
			strPath.Format("%s\\Test\\%s", m_DGEModule[nIndex].m_strTestPath, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strPath);
			if (!m_DGEModule[nIndex].fnReadDGEWLShiftVerifyCfg())
			{
				strTemp = m_DGEModule[nIndex].m_strErrorMsg;
				throw strTemp.GetBuffer(0);
			}

			if (!m_DGEModule[nIndex].fnSaveDGEWLShiftVer())
			{
				strTemp = m_DGEModule[nIndex].m_strErrorMsg;
				throw strTemp.GetBuffer(0);
			}

			for (int k = 0; k < PIXEL_NUMBER_64; k++)
			{
				fAttenSet[k] = m_DGEModule[nIndex].m_stWLShiftCfg[k].dbAttenValue;
			}
		}

		if (!fnAllPixelAttenScan(strFileName, fAttenSet))
		{
			strTemp.Format("Pixel atten scan Error");
			throw strTemp.GetBuffer(0);
		}

		for (int nModule = 0; nModule < m_nDGECount; nModule++)
		{
			strPath.Format("%s\\Test\\%s", m_DGEModule[nModule].m_strTestPath, m_DGEModule[nModule].m_strTempFolder);
			strFileName.Format("%s\\Pixel_WL_Shift_Ver_%d.txt", strPath, 1);
			if (!fnSaveScanChFiles(nModule, strFileName))
			{
				strTemp.Format("Save scan channel file error: %s", strFileName);
				throw strTemp.GetBuffer(0);
			}

			if (!fnSaveVerWLShiftData(nModule))
			{
				strTemp.Format("Save WL shift data error!");
				throw strTemp.GetBuffer(0);
			}

			//6. 记录当前定标温度
			double dblTemper = 0;
			if (!fnGetTemperWriteIni(nModule, dblTemper, "WLSHIFT_VER", FALSE))//not calibrate
			{
				if (!fnGetTemperWriteIni(nModule, dblTemper, "WLSHIFT_VER", FALSE))//not calibrate
				{
					strTemp.Format("读取LC温度错误!");
					throw strTemp.GetBuffer(0);
				}
			}
		}
		strTemp.Format("Pixel WL Shift Verification finished!");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////
//函数说明：
//    终测扫描0DACIL曲线
//Add by wanxin
//Time:20180821
/////////////////////////////////////////////////////////
BOOL CTestTaskManager::fnVer_0DACScanData()
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
	WORD    wSetDacArray[PIXEL_NUMBER_64] = { 0 };
	CString strPath;
	try
	{
		strTemp.Format("开始0 DAC插损曲线扫描");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		for (int nIndex = 0; nIndex < m_nDGECount; nIndex++)
		{
			//if (!JudgeCurrentSlotSN(nIndex))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nIndex + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			strPath.Format("%s\\Test\\%s", m_DGEModule[nIndex].m_strTestPath, m_DGEModule[nIndex].m_strTempFolder);
			CreateFullPath(strPath);
			if (!m_DGEModule[nIndex].fnSave0VDACScanData())
			{
				strTemp = m_DGEModule[nIndex].m_strErrorMsg;
				throw strTemp.GetBuffer(0);
			}

			//Set all pixels to open
			wSetDac = 0;//第n次DAC设置值
			strTemp.Format("设置所有Pixel DAC=0");
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

			for (int k = 0; k < PIXEL_NUMBER_64; k++)
			{
				wSetDacArray[k] = wSetDac;
			}
			pwSetDac = wSetDacArray;

			if (!m_DGEModule[nIndex].fnSetAllPixelDAC(pwSetDac))
			{
				OpenSerialPortDevice();
				Sleep(1000);
				if (!m_DGEModule[nIndex].fnSetAllPixelDAC(pwSetDac))
				{
					strTemp = m_DGEModule[nIndex].m_strErrorMsg;
					throw strTemp.GetBuffer(0);
				}
			}
		}
		fnSleepLongTime(2500);

		//scan result
		if (!fnPixelAttenScan(wSetDac, "Pixel_0_DAC_Scan_Ver", 1))
		{
			strTemp.Format("Pixel atten scan Error");
			throw strTemp.GetBuffer(0);
		}

		for (int nModule = 0; nModule < m_nDGECount; nModule++)
		{
			double dblTemper = 0;
			if (!fnGetTemperWriteIni(0, dblTemper, "0DAC_SCAN", FALSE))//not calibrate
			{
				strTemp.Format("读取LC温度错误!");
				throw strTemp.GetBuffer(0);
			}
			//汇总report
			if (!m_DGEModule[nModule].fnSumary0DACScanDataReport())
			{
				strTemp = m_DGEModule[nModule].m_strErrorMsg;
				throw strTemp.GetBuffer(0);
			}
		}
		strTemp.Format("0 DAC IL Scan Finished!");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}


///////////////////////////////////////////////////
//函数说明：
//    精细Attenuation定标
//Add by wanxin
//Time:20200617
////////////////////////////////////////////////////
BOOL CTestTaskManager::fnCalFined_PixelAtten()
{
	CString strTemp;
	CString strValue;
	CString strFileName;
	stPixelInfo  stPixelData;
	int     nScanNumber = 0;
	WORD    wSetDac = 0;
	WORD    *pwSetDac = NULL;
	double  dblTemper = 0;
	//	FILE    *pFile = NULL;
	double  dblStartWL = 0;
	double  dblStep = 0;
	int     nLine = 0;
	WORD    wSetDacArray[PIXEL_NUMBER_64] = { 0 };
	double  *pdblAtten_1 = NULL;
	double  *pdblAtten_2 = NULL;
	float   *pfDac_1 = NULL;
	float   *pfDac_2 = NULL;
	int     nLine_1 = 0;
	int     nLine_2 = 0;
	CString strpath;
	BOOL bFunctionOK = TRUE;
	DWORD dwSampleCount = m_stScanParameter.dwSampleCount;
	double *pdblScanGhz = new double[dwSampleCount];
	double *pdblAtten = new double[ATTEN_LUT_POINT*dwSampleCount];
	float  *pfDac = new float[ATTEN_LUT_POINT];
	BOOL bFunctionOk = TRUE;
	try
	{

		strTemp.Format("开始%.0fC温度Pixel Atten Fine 定标扫描测试......", m_stCfg.dbSetTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

		for (int nModule = 0; nModule < m_nDGECount; nModule++)
		{
			//if (!JudgeCurrentSlotSN(nModule))
			//{
			//	strTemp.Format("DGE%d模块输入SN与产品读取SN存在不一致，停止测试", nModule + 1);
			//	throw strTemp.GetBuffer(0);
			//}
			if (!ReadProductionInfo(nModule))
			{
				strTemp.Format("从配置文件中读取产品信息存在错误!");
				throw strTemp.GetBuffer(0);
			}
			strpath.Format("%s\\Calibrate\\%s", m_DGEModule[nModule].m_strCaliFolder, m_DGEModule[nModule].m_strTempFolder);
			CreateFullPath(strpath);

			strFileName.Format("%s\\Pixel_Atten_1_Cal.csv", strpath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL, nModule);

			memset(pdblAtten, 0, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			memset(pfDac, 0, ATTEN_LUT_POINT * sizeof(float));
			memset(pdblScanGhz, 0, dwSampleCount * sizeof(double));
			if (!fnReadAttenVsGhz(strFileName, (DWORD)PIXEL_NUMBER_64, pfDac, pdblAtten, &nLine, pdblScanGhz))
			{
				strTemp.Format("读取中心波长衰减定标值失败!");
				throw strTemp.GetBuffer(0);
			}


			if (!m_DGEModule[nModule].fnSaveFineAttenCalScanData())
			{
				strTemp.Format("%s", m_DGEModule[nModule].m_strErrorMsg);
				throw strTemp.GetBuffer(0);
			}

			//这里需要修改成从64Pixel中找出最大、最小插损值及对应的索引
			int nMaxIndex[PIXEL_NUMBER_64];
			double dbMaxAttnDac[PIXEL_NUMBER_64];
			double dbMaxAtten[PIXEL_NUMBER_64];

			int nMinIndex[PIXEL_NUMBER_64];
			double dbMinAttenDac[PIXEL_NUMBER_64];
			double dbMinAtten[PIXEL_NUMBER_64];
			double dblCurrentAtten = 0.0;
			double dblPixelAtten[ATTEN_LUT_POINT] = { 0 };

			for (int iCh = 0; iCh < PIXEL_NUMBER_64; iCh++)
			{
				dbMaxAttnDac[iCh] = 0;
				dbMaxAtten[iCh] = -100;
				nMaxIndex[iCh] = -1;

				nMinIndex[iCh] = -1;
				dbMinAttenDac[iCh] = 0;
				dbMinAtten[iCh] = 100;

			}

			for (iCh = 0; iCh < PIXEL_NUMBER_64; iCh++)
			{
				for (int j = 0; j < nLine; j++)
				{
					dblCurrentAtten = pdblAtten[j*PIXEL_NUMBER_64 + iCh];
					if (iCh == 31)
					{
						dblPixelAtten[j] = dblCurrentAtten;
					}
					if (dblCurrentAtten >= dbMaxAtten[iCh])
					{
						dbMaxAtten[iCh] = dblCurrentAtten;
						dbMaxAttnDac[iCh] = pfDac[j];
						nMaxIndex[iCh] = j;
					}
					if ((dblCurrentAtten <= dbMinAtten[iCh]) && (pfDac[j] > 4000))
					{
						dbMinAtten[iCh] = dblCurrentAtten;
						dbMinAttenDac[iCh] = pfDac[j];
						nMinIndex[iCh] = j;
					}
				}
			}

			strTemp.Format("Chanel:%d,Max Atten:%.3f,Max DAC:%.2f,Min Atten:%.3f,Min DAC:%.2f", 32, dbMaxAtten[31], dbMaxAttnDac[31], dbMinAtten[31], dbMinAttenDac[31]);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nModule);

			//从64通道中寻找到最大值中最大、最小值对应的DAC
			double dblChMaxDAC = -100000;
			double dblChMinDAC = 100000;
			int nChMaxDACIndex = -1;
			int nChMinDACIndex = -1;

			for (iCh = 0; iCh < PIXEL_NUMBER_64; iCh++)
			{
				if (iCh == 0 || iCh == 1 || iCh == 62 || iCh == 63)
				{
					continue;
				}
				if (dbMaxAttnDac[iCh] > dblChMaxDAC)
				{
					dblChMaxDAC = dbMaxAttnDac[iCh];
					nChMaxDACIndex = iCh;
				}
				if (dbMaxAttnDac[iCh] < dblChMinDAC)
				{
					dblChMinDAC = dbMaxAttnDac[iCh];
					nChMinDACIndex = iCh;
				}
			}
			strTemp.Format("The max atten vs DAC. The Max DAC:%.2f,ChannelIndex:%d,The Min DAC:%.2f,ChannelIndex:%d", dblChMaxDAC, nChMaxDACIndex, dblChMinDAC, nChMinDACIndex);
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nModule);

			//这里主要是 避免出现最大插损DAC为的情况存在
			if (dblChMinDAC < 1500)
			{
				dblChMinDAC = 1500;
			}
			if (dblChMaxDAC < 1500)
			{
				dblChMaxDAC = 1500;
			}

			//3. loop scan for many time against differenc DAC setting
			int  nInsertNum = 0;
			int  nDacIndex = 0;
			int nMaxDacLeft = 100;
			int nMaxDacRight = 100;
			double dblSetDAC;
			BOOL bScaned = FALSE;
			double pdbScanDac[ATTEN_LUT_POINT];
			ZeroMemory(pdbScanDac, sizeof(double)*ATTEN_LUT_POINT);
			int i = 0;
			int nSetIndex = 0;
			while (1)
			{
				bScaned = FALSE;
				dblSetDAC = (dblChMaxDAC + nMaxDacRight) - nSetIndex * 10;//10 DAC
				nSetIndex++;

				for (int nIndex = 0; nIndex < nLine; nIndex++)
				{
					if (fabs(dblSetDAC - pfDac[nIndex]) < 0.000000001)
					{
						bScaned = TRUE;
						break;
					}
				}
				if (bScaned)
				{
					continue;
				}

				if (dblSetDAC < (dblChMinDAC - nMaxDacLeft))
				{
					break;
				}

				if (nDacIndex > 255)
				{
					break;
				}

				//set dac
				strTemp.Format("第%d次设置Pixel所有通道: DAC = %.1f", nDacIndex + 1, dblSetDAC);
				LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nModule);
				pdbScanDac[nDacIndex] = dblSetDAC;
				//added by luke.liu 2018.2.6
				for (int k = 0; k < PIXEL_NUMBER_64; k++)
				{
					wSetDacArray[k] = (WORD)dblSetDAC;
				}
				wSetDac = (WORD)dblSetDAC;
				pwSetDac = wSetDacArray;


				if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac))
				{
					OpenSerialPortDevice();
					Sleep(1000);
					if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac))
					{
						strTemp = m_DGEModule[nModule].m_strErrorMsg;
						throw strTemp.GetBuffer(0);
					}
				}

				fnSleepLongTime(2500);

				if (!fnPixelAttenScan(wSetDac, "Pixel_Atten_Scan_2", 0))
				{
					strTemp.Format("Pixel atten scan Error");
					throw strTemp.GetBuffer(0);
				}

				nDacIndex++;
			}
			int nFirstInsertCount = nDacIndex;

			for (int nloop = 0; nloop < nLine; nloop++)
			{
				if (0 == nloop)
				{
					continue;
				}
				else if ((pfDac[nloop] - dbMinAttenDac[31])*(pfDac[nloop] - dbMaxAttnDac[31]) <= 0.0)
				{
					if (dblPixelAtten[nloop - 1] - dbMinAtten[31] <= 25.5)
					{
						//0.5dB插入一个定标点
						if (dbMinAttenDac[31] < dbMaxAttnDac[31]) //No WP
						{
							if (nloop < (nLine - 1))
							{
								nInsertNum = (int)(fabs(dblPixelAtten[nloop] - dblPixelAtten[nloop + 1]) / 0.5);
								if (nInsertNum < 2)//小于1dB不处理
								{
									continue;
								}

								for (i = 0; i < (nInsertNum - 1); i++)
								{
									//Set all pixels to open
									wSetDac = pfDac[nloop] - (i + 1)*(pfDac[nloop] - pfDac[nloop + 1]) / nInsertNum;

									double dbTempSetDAC = wSetDac;

									for (int nIndex = 0; nIndex < nFirstInsertCount; nIndex++)
									{
										if (fabs(dbTempSetDAC - pdbScanDac[nIndex]) < 0.000000001)
										{
											bScaned = TRUE;
											break;
										}
									}
									if (bScaned)
									{
										continue;
									}

									strTemp.Format("第%d次设置Pixel所有通道: DAC = %d", nDacIndex + 1, wSetDac);
									LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nModule);

									//added by luke.liu 2018.2.6
									for (int k = 0; k < PIXEL_NUMBER_64; k++)
									{
										wSetDacArray[k] = wSetDac;
									}
									pwSetDac = wSetDacArray;

									//if(!fnSetAllPixelDAC(wSetDac))

									if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac))
									{
										OpenSerialPortDevice();
										Sleep(1000);
										if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac))
										{
											strTemp = m_DGEModule[nModule].m_strErrorMsg;
											throw strTemp.GetBuffer(0);
										}
									}

									//scan result

									if (!fnPixelAttenScan(wSetDac, "Pixel_Atten_Scan_2", 0))
									{
										strTemp.Format("Pixel atten scan Error");
										throw strTemp.GetBuffer(0);
									}

									nDacIndex++;
									// }
								}
							}
						}
						else
						{
							nInsertNum = (int)(fabs(dblPixelAtten[nloop] - dblPixelAtten[nloop - 1]) / 0.5);
							if (nInsertNum < 2)//小于1dB不处理
							{
								continue;
							}

							for (i = 0; i < (nInsertNum - 1); i++)
							{
								//Set all pixels to open
								wSetDac = pfDac[nloop - 1] + (i + 1)*(pfDac[nloop] - pfDac[nloop - 1]) / nInsertNum;

								double dbTempSetDAC = wSetDac;

								for (int nIndex = 0; nIndex < nFirstInsertCount; nIndex++)
								{
									if (fabs(dbTempSetDAC - pdbScanDac[nIndex]) < 0.000000001)
									{
										bScaned = TRUE;
										break;
									}
								}
								if (bScaned)
								{
									continue;
								}

								strTemp.Format("第%d次设置Pixel所有通道: DAC = %d", nDacIndex + 1, wSetDac);
								LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL, nModule);

								//added by luke.liu 2018.2.6
								for (int k = 0; k < PIXEL_NUMBER_64; k++)
								{
									wSetDacArray[k] = wSetDac;
								}
								pwSetDac = wSetDacArray;

								//if(!fnSetAllPixelDAC(wSetDac))


								if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac)) //by luke.liu
								{
									OpenSerialPortDevice();
									Sleep(1000);
									if (!m_DGEModule[nModule].fnSetAllPixelDAC(pwSetDac)) //by luke.liu
									{
										strTemp = m_DGEModule[nModule].m_strErrorMsg;
										throw strTemp.GetBuffer(0);
									}
								}

								if (!fnPixelAttenScan(wSetDac, "Pixel_Atten_Scan_2", 0))
								{
									strTemp.Format("Pixel atten scan Error");
									throw strTemp.GetBuffer(0);
								}

								nDacIndex++;
							}
						}
					}
				}
			}
			if (nDacIndex < 1)
			{
				strTemp.Format("Pixel Fine Calibration Error! No Dac setting");
				throw strTemp.GetBuffer(0);
			}

			//4. 记录当前定标温度
			if (!fnGetTemperWriteIni(nModule, dblTemper, "ATTEN", TRUE))
			{
				strTemp.Format("读取LC温度错误!");
				throw strTemp.GetBuffer(0);
			}

			//5. 读取64个中心波长的衰减定标值
			memset(pdblAtten, 0, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			memset(pfDac, 0, ATTEN_LUT_POINT * sizeof(float));
			memset(pdblScanGhz, 0, dwSampleCount * sizeof(double));
			strFileName.Format("%s\\Pixel_Atten_Scan_2.csv", strpath);
			if (!fnReadAttenVsGhz(strFileName, dwSampleCount, pfDac, pdblAtten, &nLine, pdblScanGhz))
			{
				strTemp.Format("读取中心波长衰减定标值失败!");
				throw strTemp.GetBuffer(0);
			}

			//6. 读取64个中心波长
			//Get the 64 pieces of pixel center wavelength
			strFileName.Format("%s\\PixelWave.ini", strpath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL, nModule);

			if (!fnReadPixelWaveIni(strFileName, &stPixelData, FALSE)) //全部中心波长
			{
				strTemp.Format("Failed at reading the pixel wavelength ini file!");
				throw	strTemp.GetBuffer(0);
			}

			//7. 保存64个pixel的定标衰减值
			strFileName.Format("%s\\Pixel_Atten_2_Cal.csv", strpath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL, nModule);

			if (!SaveCaliPixelAtten(strFileName, nLine, &stPixelData.dblPixelGhz[0], pfDac, pdblScanGhz, pdblAtten))
			{
				strTemp.Format("Error on saving cal attenuation of 64 pixels!");
				throw strTemp.GetBuffer(0);
			}

			memset(pdblAtten, 0, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			memset(pfDac, 0, ATTEN_LUT_POINT * sizeof(float));
			memset(pdblScanGhz, 0, dwSampleCount * sizeof(double));

			//8. 第一次定标数组指针
			pdblAtten_1 = (double*)VirtualAlloc(NULL, ATTEN_LUT_POINT*dwSampleCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			pfDac_1 = (float*)VirtualAlloc(NULL, ATTEN_LUT_POINT * sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			memset(pdblAtten_1, 0, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			memset(pfDac_1, 0, ATTEN_LUT_POINT * sizeof(float));

			strFileName.Format("%s\\Pixel_Atten_Scan_1.csv", strpath);

			if (!fnReadAttenVsGhz(strFileName, dwSampleCount, pfDac_1, pdblAtten_1, &nLine_1, pdblScanGhz))
			{
				strTemp.Format("读取中心波长衰减定标值失败!");
				throw strTemp.GetBuffer(0);
			}

			//9. 第二次定标数组指针
			pdblAtten_2 = (double*)VirtualAlloc(NULL, ATTEN_LUT_POINT*dwSampleCount * sizeof(double), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			pfDac_2 = (float*)VirtualAlloc(NULL, ATTEN_LUT_POINT * sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			memset(pdblAtten_2, 0, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			memset(pfDac_2, 0, ATTEN_LUT_POINT * sizeof(float));

			strFileName.Format("%s\\Pixel_Atten_Scan_2.csv", strpath);

			if (!fnReadAttenVsGhz(strFileName, dwSampleCount, pfDac_2, pdblAtten_2, &nLine_2, pdblScanGhz))
			{
				strTemp.Format("读取中心波长衰减定标值失败!");
				throw strTemp.GetBuffer(0);
			}
			ZeroMemory(pdblAtten, ATTEN_LUT_POINT*dwSampleCount * sizeof(double));
			float fTempDAC[256];
			//合并两次扫描结果，按照DAC由大到小的顺序进行排序合并     
			for (i = 0; i < nLine_1; i++)
			{
				pfDac[i] = pfDac_1[i];
				fTempDAC[i] = pfDac_1[i];
				memcpy(&pdblAtten[i*dwSampleCount], &pdblAtten_1[i*dwSampleCount], dwSampleCount * sizeof(double));
			}

			for (i = 0; i < nLine_2; i++)
			{
				pfDac[i + nLine_1] = pfDac_2[i];
				fTempDAC[i + nLine_1] = pfDac_2[i];
				memcpy(&pdblAtten[(i + nLine_1)*dwSampleCount], &pdblAtten_2[i*dwSampleCount], dwSampleCount * sizeof(double));
			}

			nDacIndex = nLine_1 + nLine_2;

			//排序
			Bubble_SortDacVSAtten(fTempDAC, pdblAtten, nDacIndex, dwSampleCount);


			//11. 读取64个中心波长的衰减定标值
			strFileName.Format("%s\\Pixel_Atten_Scan_Fine.csv", strpath);
			nLine = nDacIndex; //两次定标数量之和

			if (!fnSaveAttenVsGhz(strFileName, dwSampleCount, fTempDAC, pdblAtten, nLine))
			{
				strTemp.Format("读取中心波长衰减定标值失败!");
				throw strTemp.GetBuffer(0);
			}

			//12. 保存64个pixel的定标衰减值
			strFileName.Format("%s\\Pixel_Atten_Fine_Cal.csv", strpath);
			LogInfo(strFileName, strFileName.GetLength() + 1, LOG_NORMAL, nModule);

			if (!SaveCaliPixelAtten(strFileName, nLine, &stPixelData.dblPixelGhz[0], fTempDAC, pdblScanGhz, pdblAtten))
			{
				strTemp.Format("Error on saving cal attenuation of 64 pixels!");
				throw strTemp.GetBuffer(0);
			}

			//13. 计算衰减LUT值，255 point per 0.1dB
			if (!fnCalcAttenVsDac(nModule, &stPixelData.dblPixelGhz[0], pdblScanGhz, pdblAtten, fTempDAC, nLine))
			{
				strTemp.Format("计算Pixel Attenuation value失败!");
				throw strTemp.GetBuffer(0);
			}

			//14. 计算衰减LUT值，255 point per 0.1dB
			//Pixel 1, 2, 63, 64: average@central frequency +/-35G
			//Pixel N: average @Central frequency +/-167G
			//by luke.liu 2018.3.26
			if (!fnCalcAttenVsDac_Ave(nModule, &stPixelData.dblPixelGhz[0], pdblScanGhz, pdblAtten, fTempDAC, nLine))
			{
				strTemp.Format("计算Pixel Attenuation vs DAC value失败!");
				throw strTemp.GetBuffer(0);
			}
		}
		strTemp.Format("Pixel attenuation calibration finished");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	catch (char *e)
	{
		strTemp = e;
		LogInfo(strFileName, strFileName.GetLength() + 1, LOG_ERROR);
		bFunctionOk = FALSE;
	}

	if (pdblAtten != NULL)
	{
		delete[] pdblAtten;
		pdblAtten = NULL;
	}
	if (pdblScanGhz != NULL)
	{
		delete[] pdblScanGhz;
		pdblScanGhz = NULL;
	}
	if (pfDac != NULL)
	{
		delete[] pfDac;
		pfDac = NULL;
	}

	if (pdblAtten_1 != NULL)
	{
		VirtualFree(pdblAtten_1, 0, MEM_RELEASE);
		pdblAtten_1 = NULL;
	}
	if (pdblAtten_2 != NULL)
	{
		VirtualFree(pdblAtten_2, 0, MEM_RELEASE);
		pdblAtten_2 = NULL;
	}
	if (pfDac_1 != NULL)
	{
		VirtualFree(pfDac_1, 0, MEM_RELEASE);
		pfDac_1 = NULL;
	}
	if (pfDac_2 != NULL)
	{
		VirtualFree(pfDac_2, 0, MEM_RELEASE);
		pfDac_2 = NULL;
	}

	return bFunctionOk;
}

///////////////////////////////////////////////////////////////
//函数说明：
//   从产品配置文件中读取产品相关配置信息
//Add by wanxin
//Time:20200626
///////////////////////////////////////////////////////////////
BOOL CTestTaskManager::ReadProductionInfo(int nModuleIndex)
{
	char szBuffer[MAX_PATH];
	CString strFileName;
	CString strKeyName;
	CString strAppName;
	CString strTemp;
	ZeroMemory(szBuffer, MAX_PATH);

	strFileName.Format(".\\config\\ProuctionInfo.ini");

	strAppName.Format("%s", m_DGEModule[nModuleIndex].m_DGEInfo.strPN);

	strKeyName.Format("PN_Rev");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nModuleVersion = atoi(strTemp);


	strKeyName.Format("ModuleType");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nModuleType = atoi(strTemp);

	strKeyName.Format("DeviceTTNumber");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nDevcieTTNum = atoi(strTemp);

	strKeyName.Format("Cal_Template_Rev");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nCal_Template_Rev = atoi(strTemp);


	strKeyName.Format("Test_Template_Rev");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nTest_Template_Rev = atoi(strTemp);

	strKeyName.Format("HW_Version");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.strHWVersion = strTemp;

	strKeyName.Format("HW_Date");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.strHWDate = strTemp;

	strKeyName.Format("FirstSliceFreq");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nFirstSliceFreq = atoi(strTemp);


	strKeyName.Format("SliceStep");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nSliceStep = atoi(strTemp);

	strKeyName.Format("EndSliceFreq");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nLastSliceFreq = atoi(strTemp);


	strKeyName.Format("Pixel_Num");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.nPixcelCount = atoi(strTemp);

	strKeyName.Format("StartFreq");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.dbStartFreq = atof(strTemp);
	m_DGEModule[nModuleIndex].m_DGEInfo.dbEndWL = SPEED_LIGHT / m_DGEModule[nModuleIndex].m_DGEInfo.dbStartFreq;

	strKeyName.Format("StopFreq");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取PN信息存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR, nModuleIndex);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_DGEInfo.dbEndFreq = atof(strTemp);
	m_DGEModule[nModuleIndex].m_DGEInfo.dbStartWL = SPEED_LIGHT / m_DGEModule[nModuleIndex].m_DGEInfo.dbEndFreq;

	return TRUE;
}
void CTestTaskManager::SetDGEModuleCurrentTemp(double dbSetValue)
{
	for (int i = 0; i < m_nDGECount; i++)
	{
		m_DGEModule[i].m_strTempFolder.Format("%.0f", dbSetValue);
	}

}
/////////////////////////////////////////////////////////////////////////////////
//函数说明：
//   递归冒泡排序
//Add by wanxin
//Time:20180905
//////////////////////////////////////////////////////////////////////////////
int CTestTaskManager::MySort(float *pfDAC, double *pdblAtten, int nCount, int nScanCont)
{
	int ordered = true;
	int i;
	float fTemp;
	double *dbAttenTemp = NULL;

	if (dbAttenTemp == NULL)
	{
		dbAttenTemp = (double*)VirtualAlloc(NULL, sizeof(double)*nScanCont, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}
	ZeroMemory(dbAttenTemp, sizeof(double)*nScanCont);
	for (i = 1; i < nCount; i++)
	{
		if (pfDAC[i - 1] < pfDAC[i])
		{
			ordered = false;
			double *pdblAttenTemp1 = &pdblAtten[i*nScanCont];
			double *pdblAttenTemp2 = &pdblAtten[(i - 1)*nScanCont];

			fTemp = pfDAC[i];
			memcpy(dbAttenTemp, pdblAttenTemp1, sizeof(double)*nScanCont);
			pfDAC[i] = pfDAC[i - 1];
			memcpy(pdblAttenTemp1, pdblAttenTemp2, sizeof(double)*nScanCont);
			pfDAC[i - 1] = fTemp;
			memcpy(pdblAttenTemp2, dbAttenTemp, sizeof(double)*nScanCont);
		}
	}
	if (dbAttenTemp != NULL)
	{
		VirtualFree(dbAttenTemp, 0, MEM_RELEASE | MEM_COMMIT);
	}
	return ordered;
}
//////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    按照冒泡法依据DAC进行由大到小的顺序进行排序
//Add by wanxin
//Time：20180830
/////////////////////////////////////////////////////////////////////////////////
void  CTestTaskManager::Bubble_SortDacVSAtten(float *pfDAC, double *pdblAtten, int nCount, int nAttenScanPointCount)
{
	//	int i,j;
	int nLength = nAttenScanPointCount;
	int nCountTemp = nCount;

	if (nCount == 1)
	{
		return;
	}
	else
	{
		int ordered = MySort(pfDAC, pdblAtten, nCount, nLength);
		if (ordered)
		{
			return;
		}
		else
		{
			Bubble_SortDacVSAtten(pfDAC, pdblAtten, nCount - 1, nLength);
		}
	}
}

BOOL CTestTaskManager::fnSaveAttenVsGhz(LPCTSTR lpstPath, int nScanCount, float *pfDac, double *pdblAtten, int nLine)
{
	CString strTemp;
	CString strValue;
	DWORD        dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD        dwBufSize = (dwScanCount + 1) * 14; //"196571.700000,"，每个频率最大字符数

	char		    *lineBuf = new char[dwBufSize];
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;
	FILE         *pFile = NULL;
	BOOL  bFunctionOK = TRUE;

	try
	{

		DWORD  dwCount = dwScanCount;

		if (dwCount != nScanCount)
		{
			strTemp.Format("Scan Count: %d is not valid", nScanCount);
			throw strTemp.GetBuffer(0);
		}

		//读取文件
		if (lpstPath == "")
		{
			strTemp.Format("File name can't be empty: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		pFile = fopen(lpstPath, "wt");
		if (INVALID_HANDLE_VALUE == pFile)
		{
			strTemp.Format("Open file err: %s", lpstPath);
			throw strTemp.GetBuffer(0);
		}

		strTemp.Format("DAC");
		fprintf(pFile, "%s", strTemp);

		strValue.Empty();
		double dblStartWL = m_stScanParameter.dblStartWL;
		double dblStep = m_stScanParameter.dblStepSize;
		for (int k = 0; k < dwScanCount; k++)
		{
			double dblThisGHz = VELOCITY_LIGHT / (dblStartWL + dblStep * k);
			strValue.Format(",%.3f", dblThisGHz);
			fprintf(pFile, "%s", strValue);
		}

		//title 
		fprintf(pFile, "\n", strTemp);

		for (int i = 0; i < nLine; i++) //DAC line
		{
			strTemp.Format("%.2f", pfDac[i]);
			fprintf(pFile, "%s", strTemp);

			for (int nGhzIndex = 0; nGhzIndex < nScanCount; nGhzIndex++)
			{
				strTemp.Format(",%lf", pdblAtten[i*dwScanCount + nGhzIndex]); //added IL colum
				fprintf(pFile, "%s", strTemp);
			}

			fprintf(pFile, "\n", strTemp);
		}

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}

	if (NULL != pFile)
	{
		fclose(pFile);
	}

	return bFunctionOK;
}

BOOL CTestTaskManager::fnCalcAttenVsDac_Ave(int nModuleIndex, double *pdblPixelGhz, double *pdblScanGHz, double *pdblAtten, float *pfDac, int nLine)
{
	CString strTemp;
	CString strFileName;

	CString strPixelFileName;
	CString strPixSection;
	CString strPixKey;
	CString strData[1024];

	double   dblSaveGray[256] = { 0 };

	FILE *pfCsvFile = NULL;
	//CStdioFile stdFile;
	DWORD dwScanCount = m_stScanParameter.dwSampleCount;

	double *dblPixelAtten_Ave = NULL;
	BOOL bHasWP = FALSE;
	BOOL bFunctionOK = TRUE;

	dblPixelAtten_Ave = new double[nLine * PIXEL_NUMBER_64];
	double *pdblDac = new double[nLine];
	double *pdblAttnLastPixel = new double[nLine];
	try
	{
		//保存通道IL数据
		strFileName.Format("%s\\calibrate\\%s\\256vsLut.csv", m_DGEModule[nModuleIndex].m_strCaliFolder, m_DGEModule[nModuleIndex].m_strTempFolder);

		memset(pdblDac, 0, nLine * sizeof(double));

		for (int i = 0; i < nLine; i++)
		{
			pdblDac[i] = pfDac[i];
		}


		memset(pdblAttnLastPixel, 0, nLine * sizeof(double));

		pfCsvFile = fopen(strFileName.GetBuffer(0), "w");
		if (pfCsvFile == NULL)
		{
			strTemp.Format("Open %s file error", strFileName);
			throw strTemp.GetBuffer(0);
		}


		double dblThisGHz;
		for (int nPixIdx = 0; nPixIdx < PIXEL_NUMBER_64; nPixIdx++)
		{

			if (nPixIdx == 0)
			{
				dblThisGHz = pdblPixelGhz[1];
			}
			else if (nPixIdx == 63)
			{
				dblThisGHz = pdblPixelGhz[62];
			}
			else
			{
				dblThisGHz = pdblPixelGhz[nPixIdx];
			}

			int nGHzIdx = 0;

			// Search GHz index
			for (int j = 1; j < dwScanCount; j++)
			{
				if (dblThisGHz >= pdblScanGHz[0])
				{
					nGHzIdx = 0;
					break;
				}
				else if (dblThisGHz <= pdblScanGHz[dwScanCount - 1])
				{
					nGHzIdx = dwScanCount - 1;
					break;
				}
				else
				{
					if ((dblThisGHz < pdblScanGHz[j - 1]) && (dblThisGHz >= pdblScanGHz[j]))
					{
						nGHzIdx = j;
						break;
					}
				}
			}// get GHz index


			int nThisIdx;
			int nLastIdx;

			if ((nGHzIdx == 0) || (nGHzIdx == dwScanCount - 1))
			{
				nThisIdx = nGHzIdx;
				nLastIdx = nGHzIdx;
			}
			else
			{
				nThisIdx = nGHzIdx;
				//nLastIdx = nGHzIdx-1;
				nLastIdx = nGHzIdx;
			}

			// save to array
			//这里每个Pixel的衰减取平均值
			for (j = 0; j < nLine; j++)
			{
				double dblSumIL = 0;
				int    nNum_70G = 0;
				int    nNum_334G = 0;
				//前后各两个中心波长左右35G范围
				if ((PIXEL_1 == nPixIdx) ||
					(PIXEL_2 == nPixIdx) ||
					(PIXEL_63 == nPixIdx) ||
					(PIXEL_64 == nPixIdx))
				{
					//中心波长左边35G
					for (int m = nLastIdx; ; m--)
					{
						dblSumIL += pdblAtten[j*dwScanCount + m];
						nNum_70G++;

						if (pdblScanGHz[m] >= pdblScanGHz[nLastIdx] + 35.0)//35G
						{
							break;
						}
					}

					//中心波长右边35G
					for (m = nLastIdx; ; m++)
					{
						dblSumIL += pdblAtten[j*dwScanCount + m];
						nNum_70G++;

						if (pdblScanGHz[m] <= pdblScanGHz[nLastIdx] - 35.0)//35G
						{
							break;
						}
					}

					//Pixel 1,2,63,64平均后的IL
					pdblAttnLastPixel[j] = dblSumIL / nNum_70G;
				}
				else if (PIXEL_3 == nPixIdx || PIXEL_62 == nPixIdx)//Modify by wanxin add 3\62 Pixel  handle,+/-70G
				{
					//中心波长左边35G
					for (int m = nLastIdx; ; m--)
					{
						dblSumIL += pdblAtten[j*dwScanCount + m];
						nNum_70G++;

						if (pdblScanGHz[m] >= pdblScanGHz[nLastIdx] + 70.0)//35G
						{
							break;
						}
					}

					//中心波长右边35G
					for (m = nLastIdx; ; m++)
					{
						dblSumIL += pdblAtten[j*dwScanCount + m];
						nNum_70G++;

						if (pdblScanGHz[m] <= pdblScanGHz[nLastIdx] - 70.0)//35G
						{
							break;
						}
					}

					//Pixel 1,2,63,64平均后的IL
					pdblAttnLastPixel[j] = dblSumIL / nNum_70G;
				}
				else//其他中心波长左右167G范围
				{
					//中心波长左边167G
					for (int m = nLastIdx; ; m--)
					{
						dblSumIL += pdblAtten[j*dwScanCount + m];
						nNum_334G++;

						if (pdblScanGHz[m] >= pdblScanGHz[nLastIdx] + 167.0)//35G
						{
							break;
						}
					}

					//中心波长右边167G
					for (m = nLastIdx; ; m++)
					{
						dblSumIL += pdblAtten[j*dwScanCount + m];
						nNum_334G++;

						if (pdblScanGHz[m] <= pdblScanGHz[nLastIdx] - 167.0)//35G
						{
							break;
						}
					}

					//其他pixel平均后的IL
					pdblAttnLastPixel[j] = dblSumIL / nNum_334G;
				}
			}


			// search Max and Min	
			int nMaxIdxLast = 0;
			int nMinIdxLast = 0;
			double dblMaxLast = -100.0;
			double dblMinLast = 100.0;

			//最大衰减index by luke.liu
			for (j = 0; j < nLine; j++)
			{
				if (pdblAttnLastPixel[j] >= dblMaxLast)
				{
					dblMaxLast = pdblAttnLastPixel[j];
					nMaxIdxLast = j;
				}
			}

			//最小衰减index, 从最大衰减index往左查找 by luke.liu
			for (int n = 0; n < nLine; n++)
			{
				if (pdblAttnLastPixel[n] <= dblMinLast && (pfDac[n] > 4000))
				{
					dblMinLast = pdblAttnLastPixel[n];
					nMinIdxLast = n;
				}
			}

			//Atten定标顺序都是有大DAC到小DAC
			//因此如果最大插损的Index <最小插损的
			//表示不带拨片，否则带拨片
			if (nMaxIdxLast < nMinIdxLast) //表示不带拨片
			{
				bHasWP = FALSE;
			}
			else//表示带拨片
			{
				bHasWP = TRUE;
			}

			bHasWP = TRUE;
			///////////////////////////////////////////////////////////////////////////
			//initial value by luke.liu 2018.2.1
			double x1 = 0;
			double x2 = 0;
			double y1 = 0;
			double y2 = 0;
			double XX = 0;
			double YY = 0;


			// reference
			for (j = 0; j < nLine; j++)
			{
				pdblAttnLastPixel[j] -= dblMinLast;
			}

			dblMaxLast -= dblMinLast;
			dblMinLast = 0;
			double dblThisAttn = 0;


			//////////////////////////////////////////////////////////////////////////		
			for (int nAttn = 0; nAttn <= 255; nAttn++)
			{
				dblThisAttn = (double)nAttn / 10; //0.1dB interval

				if (0 == nAttn)  // search Min
				{
					dblSaveGray[nAttn] = pdblDac[nMinIdxLast];
				}
				else if (nAttn == 255) //保证25.5为block值
				{
					dblSaveGray[nAttn] = pdblDac[nMaxIdxLast];
				}
				else
				{
					//////////////////////////////////////////////////////////////////////
					int nLastArrayIdx = 1;
					// search Last array				
					if (dblThisAttn >= dblMaxLast)
					{
						dblSaveGray[nAttn] = pdblDac[nMaxIdxLast];
					}
					else
					{
						if (bHasWP)
						{
							for (int k = nMinIdxLast; k <= nMaxIdxLast - 1; k++)//有可能只有最后一点满足
							{
								if (pdblAttnLastPixel[k] >= dblThisAttn)
								{
									nLastArrayIdx = k;//原来为k, by luke.liu 2018.3.1恢复为k, 但是nLastArrayIdx不能等于nMaxIdxLast，否则下面数组拟合溢出
									break;
								}
							}

							x1 = pdblAttnLastPixel[nLastArrayIdx - 1];
							x2 = pdblAttnLastPixel[nLastArrayIdx];
							y1 = pdblDac[nLastArrayIdx - 1];
							y2 = pdblDac[nLastArrayIdx];//pdblDac[nLastArrayIdx+1] 2018.3.4
							XX = dblThisAttn;

							if (XX > x1 && XX < x2)//这里保证是线性内插
							{
								YY = fnLineCalc(x1, x2, y1, y2, XX);
							}
							else if (XX == x1)
							{
								YY = pdblDac[nLastArrayIdx - 1];
							}
							else if (XX == x2)
							{
								YY = pdblDac[nLastArrayIdx];
							}
							else
							{
								strTemp.Format("Pixel DAC value must be inline linear calculate:x1=%.3f,x2=%.3f,cur=%.3f", x1, x2, XX);

							}


							if (YY < 0)
							{
								strTemp.Format("Pixel DAC value can't be negative: %.2f", YY);

							}

							dblSaveGray[nAttn] = YY; //by luke.liu 2018.4.4

						}
						else //不带拨片
						{
							for (int k = nMinIdxLast; k >= nMaxIdxLast; k--)//有可能只有最后一点满足
							{
								if (pdblAttnLastPixel[k] >= dblThisAttn)
								{
									nLastArrayIdx = k;//原来为k, by luke.liu 2018.3.1恢复为k, 但是nLastArrayIdx不能等于nMaxIdxLast，否则下面数组拟合溢出
									break;
								}
							}

							x1 = pdblAttnLastPixel[nLastArrayIdx];
							x2 = pdblAttnLastPixel[nLastArrayIdx + 1];//pdblAttnLastPixel[nLastArrayIdx+1] 2018.3.4
							y1 = pdblDac[nLastArrayIdx];
							y2 = pdblDac[nLastArrayIdx + 1];//pdblDac[nLastArrayIdx+1] 2018.3.4
							XX = dblThisAttn;

							if (XX > x1 && XX < x2)//这里保证是线性内插
							{
								YY = fnLineCalc(x1, x2, y1, y2, XX);
							}
							else if (XX == x1)
							{
								YY = pdblDac[nLastArrayIdx];
							}
							else if (XX == x2)
							{
								YY = pdblDac[nLastArrayIdx + 1];
							}
							else
							{
								strTemp.Format("Pixel DAC value must be inline linear calculate:x1=%.3f,x2=%.3f,cur=%.3f", x1, x2, XX);

							}

							if (YY < 0)
							{
								strTemp.Format("Pixel DAC value can't be negative: %.2f", YY);

							}

							dblSaveGray[nAttn] = YY; //by luke.liu 2018.4.4
						}
					}
				}
			}

			strData[nPixIdx].Empty();
			for (int k = 0; k <= 255; k++)
			{
				if (k < 255)
				{
					//strTemp.Format("%d,",dblSaveGray[k]);
					strTemp.Format("%.3f,", dblSaveGray[k]);//by luke.liu 2018.4.4
					strData[nPixIdx] += strTemp;
				}
				else
				{
					//strTemp.Format("%d\n",dblSaveGray[k]);
					strTemp.Format("%.3f\n", dblSaveGray[k]);//by luke.liu 2018.4.4
					strData[nPixIdx] += strTemp;
				}
			}
		}

		for (nPixIdx = 0; nPixIdx < PIXEL_NUMBER_64; nPixIdx++)
		{
			fprintf(pfCsvFile, "%s", strData[nPixIdx]);
		}

		strTemp.Format("Calculate Pixel Atten vs DAC finished ! ");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);

	}
	catch (char *error)
	{
		strTemp = error;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		bFunctionOK = FALSE;
	}
	if (pdblDac != NULL)
	{
		delete[] pdblDac;
	}
	if (pdblAttnLastPixel != NULL)
	{
		delete[] pdblAttnLastPixel;
	}
	if (dblPixelAtten_Ave != NULL)
	{
		delete[] dblPixelAtten_Ave;
	}

	if (pfCsvFile != NULL)
	{
		fclose(pfCsvFile);
		pfCsvFile = NULL;
	}

	return bFunctionOK;
}

///////////////////////////////////////////////////////////////
//函数说明：
//   从产品配置文件中读取产品相关配置信息
//Add by wanxin
//Time:20200626
///////////////////////////////////////////////////////////////
BOOL CTestTaskManager::ReadProductionTestingInfo(int nModuleIndex)
{
	char szBuffer[MAX_PATH];
	CString strFileName;
	CString strKeyName;
	CString strAppName;
	CString strTemp;
	ZeroMemory(szBuffer, MAX_PATH);

	strFileName.Format(".\\config\\ProuctionInfo.ini");

	strAppName.Format("CAL_TEMP");

	strKeyName.Format("TEMPCOUNT");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取定标温度个数存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_stConfigTable.nCaliTempCount = atoi(strTemp);

	for (int i = 0; i < m_DGEModule[nModuleIndex].m_stConfigTable.nCaliTempCount; i++)
	{
		strKeyName.Format("CAL_T%d", i + 1);
		ZeroMemory(szBuffer, MAX_PATH);
		GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
		strTemp = szBuffer;
		if (strTemp == "ERROR")
		{
			strTemp.Format("从ProductionInfo文件中读取定标温度存在错误");
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
		m_DGEModule[nModuleIndex].m_stConfigTable.dbCalTemp[i] = atof(strTemp);
	}

	strAppName.Format("DWL_TEMP");
	strKeyName.Format("TEMPCOUNT");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取下载温度个数存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_stConfigTable.nDWTempCount = atoi(strTemp);
	for (i = 0; i < m_DGEModule[nModuleIndex].m_stConfigTable.nDWTempCount; i++)
	{
		strKeyName.Format("DWL_T%d", i + 1);
		ZeroMemory(szBuffer, MAX_PATH);
		GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
		strTemp = szBuffer;
		if (strTemp == "ERROR")
		{
			strTemp.Format("从ProductionInfo文件中读取下载温度存在错误");
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
		m_DGEModule[nModuleIndex].m_stConfigTable.dbDWTemp[i] = atof(strTemp);
	}

	std::sort(m_DGEModule[nModuleIndex].m_stConfigTable.dbDWTemp, m_DGEModule[nModuleIndex].m_stConfigTable.dbDWTemp + 5);

	strAppName.Format("VER_TEMP");
	strKeyName.Format("TEMPCOUNT");
	ZeroMemory(szBuffer, MAX_PATH);
	GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
	strTemp = szBuffer;
	if (strTemp == "ERROR")
	{
		strTemp.Format("从ProductionInfo文件中读取产品终测温度个数存在错误");
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	m_DGEModule[nModuleIndex].m_stConfigTable.nTestTempCount = atoi(strTemp);

	for (i = 0; i < m_DGEModule[nModuleIndex].m_stConfigTable.nTestTempCount; i++)
	{
		strKeyName.Format("VER_T%d", i + 1);
		ZeroMemory(szBuffer, MAX_PATH);
		GetPrivateProfileString(strAppName, strKeyName, "ERROR", szBuffer, MAX_PATH, strFileName);
		strTemp = szBuffer;
		if (strTemp == "ERROR")
		{
			strTemp.Format("从ProductionInfo文件中读取下载温度存在错误");
			LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
			return FALSE;
		}
		m_DGEModule[nModuleIndex].m_stConfigTable.dbTestTemp[i] = atof(strTemp);
	}

	return TRUE;
}

BOOL CTestTaskManager::JudgeCurrentSlotSN(int nModuleIndex)
{
	CString strSN;
	CString strTemp;

	if (!m_DGEModule[nModuleIndex].m_Module.GetSerialNo(strSN))
	{
		strTemp = m_DGEModule[nModuleIndex].m_strErrorMsg;
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	strSN.MakeUpper();
	strSN.TrimLeft();
	strSN.TrimRight();
	strTemp = m_DGEModule[nModuleIndex].m_DGEInfo.strSN;
	strTemp.MakeUpper();
	strTemp.TrimLeft();
	strTemp.TrimRight();

	if (strSN.CompareNoCase(strTemp) == 0)
	{
		strTemp.Format("模块%d读取产品SN:%s,输入SN:%s,两者一致", strSN, strTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_NORMAL);
	}
	else
	{
		strTemp.Format("模块%d读取产品SN:%s,输入SN:%s,两者不一致", strSN, strTemp);
		LogInfo(strTemp, strTemp.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}

BOOL CTestTaskManager::DoRefOpticalCheck()
{
	CString strMsg;
	CString strTemp;
	BOOL  bCirculatorFlag = FALSE;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	DWORD dwBoxRet;

	strMsg = _T("请将");
	for (int i = 1; i < m_stScanParameter.nPMCount; i++)
	{
		strTemp.Format("模块%d的In%d端接Out%d端,", i, i, i);
		strMsg += strTemp;
	}
	dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (dwBoxRet == XSB_YES)
	{
		strTempPath.Format(".\\Data\\ScanTemp");
		CreateFullPath(strTempPath);
		bCirculatorFlag = m_FSTPObj.m_stScan.bCirculator;
		m_FSTPObj.m_stScan.bCirculator = FALSE;
		m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw.csv", strTempPath);

		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
		InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}
		int nPlotIndex = 0;
		for (int i = 0; i < m_nDGECount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i + 1];

			strTemp.Format("DGE模块%d", i + 1);
			//m_pGraph->PlotClear(0,i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			if (m_stScanParameter.bDoPDL)
			{
				AddPlotCount(1, strTemp);
				double *pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i + 1];
				m_pGraph->PlotXYS(1, pdbWLArry, pdbPDLArry, dwSampleCount, nPlotIndex, 0);
				nPlotIndex++;
			}
		}
		m_FSTPObj.m_stScan.bCirculator = bCirculatorFlag;
		for (int j = 0; j < m_nDGECount; j++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[j + 1];
			for (int i = 0; i < dwSampleCount; i++)
			{
				if (fabs(pdbPMArry[i]) > 0.05)
				{
					strMsg.Format("%d模块通道 - 波长:%.2fnm - IL:%.3fdB,超过控制范围0.05dB", j + 1, pdbWLArry[i], pdbPMArry[i]);
					LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
					return FALSE;
				}
			}
		}
	}
	else
	{
		strMsg.Format("人工取消了归零光路验证");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}


	return TRUE;

}
BOOL CTestTaskManager::DoCirculator12Check()
{

	CString strMsg;
	CString strTemp;
	BOOL  bCirculatorFlag = FALSE;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	DWORD dwBoxRet;

	strMsg = _T("请将");
	for (int i = 1; i < m_stScanParameter.nPMCount; i++)
	{
		strTemp.Format("模块%d的环形器1端口接2端口,", i);
		strMsg += strTemp;
	}
	dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (dwBoxRet == XSB_YES)
	{
		strTempPath.Format(".\\Data\\ScanTemp");
		CreateFullPath(strTempPath);
		bCirculatorFlag = m_FSTPObj.m_stScan.bCirculator;
		m_FSTPObj.m_stScan.bCirculator = FALSE;
		m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw.csv", strTempPath);

		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
		InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}
		int nPlotIndex = 0;
		for (int i = 0; i < m_nDGECount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i + 1];

			strTemp.Format("DGE模块%d", i + 1);
			//m_pGraph->PlotClear(0,i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			if (m_stScanParameter.bDoPDL)
			{
				AddPlotCount(1, strTemp);
				double *pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i + 1];
				m_pGraph->PlotXYS(1, pdbWLArry, pdbPDLArry, dwSampleCount, nPlotIndex, 0);
				nPlotIndex++;
			}
		}
		m_FSTPObj.m_stScan.bCirculator = bCirculatorFlag;
		for (int j = 0; j < m_nDGECount; j++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[j + 1];
			for (int i = 0; i < dwSampleCount; i++)
			{
				if (fabs(pdbPMArry[i]) > 1.0)
				{
					strMsg.Format("%d模块通道 - 波长:%.2fnm - IL:%.3fdB,超过环形器1-2端口控制范围1.0dB", j + 1, pdbWLArry[i], pdbPMArry[i]);
					LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
					return FALSE;
				}
			}
		}
	}
	else
	{
		strMsg.Format("人工取消了环形器端口1-端口2的验证");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}


	return TRUE;

}
BOOL CTestTaskManager::DoCirculator23Check()
{
	CString strMsg;
	CString strTemp;
	BOOL  bCirculatorFlag = FALSE;
	//m_dwScanCount = m_stScanParameter.dwSampleCount;
	DWORD  dwSampleCount = m_stScanParameter.dwSampleCount;
	CString strTempPath;
	DWORD dwBoxRet;

	strMsg = _T("请将");
	for (int i = 1; i < m_stScanParameter.nPMCount; i++)
	{
		strTemp.Format("模块%d的环形器2端口接3端口,", i);
		strMsg += strTemp;
	}
	dwBoxRet = m_pNetCommand->XmlShowBox(strMsg, NULL, XSB_YES | XSB_NO | XSB_ICONQUESTION);
	if (dwBoxRet == XSB_YES)
	{
		strTempPath.Format(".\\Data\\ScanTemp");
		CreateFullPath(strTempPath);
		bCirculatorFlag = m_FSTPObj.m_stScan.bCirculator;
		m_FSTPObj.m_stScan.bCirculator = FALSE;
		m_FSTPObj.m_strScanFileName.Format("%s\\ScanRaw.csv", strTempPath);

		InitGraphShow(0, "IL(dB)", "Wavelength(nm)", "IL测试");//IL
		InitGraphShow(1, "PDL(dB)", "Wavelength(nm)", "PDL测试");//PDL

		if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
		{
			Sleep(2000);
			if (!m_FSTPObj.DoTLSScaning(m_FSTPObj.m_nGUID))
			{
				strMsg = m_FSTPObj.GetErrorMsg();
				LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
				return FALSE;
			}
		}
		int nPlotIndex = 0;
		for (int i = 0; i < m_nDGECount; i++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[i + 1];

			strTemp.Format("DGE模块%d", i + 1);
			//m_pGraph->PlotClear(0,i);
			AddPlotCount(0, strTemp);
			m_pGraph->PlotXYS(0, pdbWLArry, pdbPMArry, dwSampleCount, nPlotIndex, 0);
			nPlotIndex++;
			if (m_stScanParameter.bDoPDL)
			{
				AddPlotCount(1, strTemp);
				double *pdbPDLArry = m_FSTPObj.m_stScanResult.pdbPDLResult[i + 1];
				m_pGraph->PlotXYS(1, pdbWLArry, pdbPDLArry, dwSampleCount, nPlotIndex, 0);
				nPlotIndex++;
			}
		}
		m_FSTPObj.m_stScan.bCirculator = bCirculatorFlag;
		for (int j = 0; j < m_nDGECount; j++)
		{
			double *pdbWLArry = m_FSTPObj.m_stScanResult.pdbWLArry;
			double *pdbPMArry = m_FSTPObj.m_stScanResult.pdbPMResult[j + 1];
			for (int i = 0; i < dwSampleCount; i++)
			{
				if (fabs(pdbPMArry[i]) > 1.0)
				{
					strMsg.Format("%d模块通道 - 波长:%.2fnm - IL:%.3fdB,超过环形器1-2端口控制范围1.0dB", j + 1, pdbWLArry[i], pdbPMArry[i]);
					LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
					return FALSE;
				}
			}
		}
	}
	else
	{
		strMsg.Format("人工取消了环形器2-3端口的验证");
		LogInfo(strMsg, strMsg.GetLength() + 1, LOG_ERROR);
		return FALSE;
	}


	return TRUE;
}
BOOL CTestTaskManager::DoProductionSlotCheck()
{
	for (int i = 0; i < m_nDGECount; i++)
	{
		if (!JudgeCurrentSlotSN(i))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CTestTaskManager::DoProductionOpticalCheck()
{





	return TRUE;
}

