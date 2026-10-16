// BasicClass.cpp: implementation of the CBasicClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BasicClass.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBasicClass::CBasicClass()
{
   m_bStop = FALSE;
}

CBasicClass::~CBasicClass()
{

}
////////////////////////////////////////////////////////
//函数说明：
//    除去运行程序名后，获得文件的路径
//Add by wanxin
///////////////////////////////////////////////////////
CString  CBasicClass::ExtractFilePath(CString FileName)
{
	CString temp;
	int pos = -1;
	pos = FileName.ReverseFind('\\');
	temp = FileName.Left(pos+1);
	return temp;
}
////////////////////////////////////////////////////////
//函数说明：
//    根据输入的路径，创建目录结构
//Add by wanxin
////////////////////////////////////////////////////////
BOOL CBasicClass::CreateFullPath(LPCSTR lpPath)
{
	CString strTemp;
	strTemp = lpPath;
	if (-1 == GetFileAttributes(strTemp))
	{
		int nPos = strTemp.ReverseFind('\\');
		CreateFullPath(strTemp.Left(nPos));
		CreateDirectory(strTemp, NULL);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////
//函数说明：
//    判断文件是否存在
//Add by  wanxin
////////////////////////////////////////////////////////////////
BOOL CBasicClass::IsTheFileExist(LPCTSTR lpstrFileName)
{
	BOOL          bFunctionOK = FALSE;
	HANDLE		  hResultFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;
	hResultFile = FindFirstFile(lpstrFileName, &winFindFileData);
	//the file is exist 
	if (hResultFile != INVALID_HANDLE_VALUE)
	{
		bFunctionOK = TRUE;	
	}
	else // the file is not exist
	{
		bFunctionOK = FALSE;
	}
	// clost the handle
	FindClose(hResultFile);
	return bFunctionOK;
}
////////////////////////////////////////////////////////
//函数说明：
//   在使用循环时，同时响应处理界面消息，使界面实时显示
//Add by wanxin
///////////////////////////////////////////////////////
void CBasicClass::YieldToPeers()
{
	MSG	msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(!AfxGetThread()->PumpMessage())
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////
//函数说明：
//    根据输入数组和要查询的值，在数组中查找到对应索引号
//Add by wanxin
//////////////////////////////////////////////////////////////////////////////
BOOL CBasicClass::FindIndex(double *pdblRawArray, int nRawCount, double dblVal, int &nIndex, BOOL bInCrease)
{
	if (bInCrease)
	{
		for (int i = 1; i < nRawCount; i++)
		{
			double dblRes = (pdblRawArray[i-1] - dblVal) * (pdblRawArray[i] - dblVal);
			if (dblRes <= 0)
			{
				nIndex = i;
				return TRUE;
			}
		}
	}
	else
	{
		for (int i = nRawCount-1; i > 1; i++)
		{
			double dblRes = (pdblRawArray[i-1] - dblVal) * (pdblRawArray[i] - dblVal);
			if (dblRes <= 0)
			{
				nIndex = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    根据高阶拟合系数，采用逐步代入法法，计算DAC值
//Add by wanxin
///////////////////////////////////////////////////////////////////////////////////////////
double CBasicClass::CalculateDAC(double dblStratDAC, double dblTargetAtt, double *pdblCoeff)
{
	double dblAtt[7];
	double dblFinalAtt = 0;
	double dblLastAtt = 0;
	double dblDACAdd = dblStratDAC;
	while (TRUE) 
	{	
		dblAtt[6] = pdblCoeff[6];
		for (int i = 0; i < 6; i++)
		{
			dblAtt[6] *= dblDACAdd;
		}
		
		dblAtt[5] = pdblCoeff[5];
		for (i = 0; i < 5; i++)
		{
			dblAtt[5] *= dblDACAdd;
		}
		
		dblAtt[4] = pdblCoeff[4];
		for (i = 0; i < 4; i++)
		{
			dblAtt[4] *= dblDACAdd;
		}
		
		dblAtt[3] = pdblCoeff[3];
		for (i = 0; i < 3; i++)
		{
			dblAtt[3] *= dblDACAdd;
		}
		
		dblAtt[2] = pdblCoeff[2];
		for (i = 0; i < 2; i++)
		{
			dblAtt[2] *= dblDACAdd;
		}
		
		dblAtt[1] = pdblCoeff[1]*dblDACAdd;
		dblAtt[0] = pdblCoeff[0];
		
		dblFinalAtt = 0;
		for (i = 0; i < 7; i++)
		{
			dblFinalAtt += dblAtt[i];
		}
		
		if (fabs(dblTargetAtt - dblFinalAtt) < 0.02) 
		{
			break;
		}
		
		dblLastAtt = dblFinalAtt;
		
		dblDACAdd -= 1;
		
		if (dblDACAdd < 1) 
		{
			break;
		}
	}
	
	return dblDACAdd;
}
////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    从输入的字符串中根据“,”,分解出字符串为int型数组
//Add by wanxin
//Time：20131127
///////////////////////////////////////////////////////////////////////////////////
BOOL CBasicClass::GetSplitStringToIntArry(LPCSTR lpStrData,int *pnGetData,int& nCount)
{
	CString strTemp;
	strTemp  = lpStrData;
	int nIndex = 0;
	int nPos;

	while (TRUE) 
	{
		nPos = strTemp.Find(",");
		if(nPos > 0)
		{
			pnGetData[nIndex] = atoi(strTemp.Left(nPos));
			strTemp.Delete(0, nPos+1);

			if (strTemp.IsEmpty()) 
			{
				nIndex++; 
				break;
			}
		}
		else
		{
			pnGetData[nIndex] = atoi(strTemp);
			nIndex++; 
			break;
		}
		
		if (pnGetData[nIndex] > 10000 || pnGetData[nIndex] < -10000) 
		{
            return FALSE;
		}
		nIndex++; 
	}
	nCount = nIndex;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    从输入的字符串中根据“,”,分解出字符串为double型数组
//Add by wanxin
//Time：20131127
///////////////////////////////////////////////////////////////////////////////////
BOOL CBasicClass::GetSplitStringTodoubleArry(LPCSTR lpStrData,double *pdbGetData,int& nCount)
{
	CString strTemp;
	strTemp  = lpStrData;
	int nIndex = 0;
	int nPos;
	
	if (strTemp.IsEmpty())
	{
        nCount =0;		
	}
	else
	{
		while (TRUE) 
		{
			nPos = strTemp.Find(",");
			if(nPos > 0)
			{
				pdbGetData[nIndex] = atof(strTemp.Left(nPos));
				strTemp.Delete(0, nPos+1);
				
				if (strTemp.IsEmpty()) 
					break;
			}
			else
			{
				pdbGetData[nIndex] = atof(strTemp);
				break;
			}
			
			if (pdbGetData[nIndex] > 10000.0 || pdbGetData[nIndex] < -10000.0) 
			{
				return FALSE;
			}
			nIndex++; 
		}
		nCount = nIndex+1;
	}
	
	return TRUE;
}
////////////////////////////////////////////////////////
//函数说明：
//     时间延迟函数
//Add by wanxin
//Time：20131129
//////////////////////////////////////////////////////////
BOOL CBasicClass::SleepTime(int nSecondeTime)
{
	CTime  StartTime,EndTime;
	time_t time1,time2;
	m_bStop = FALSE;
	StartTime = CTime::GetCurrentTime();
	time1= StartTime.GetTime();
	
	while (TRUE)
	{
		YieldToPeers();
		EndTime = CTime::GetCurrentTime();
		time2= EndTime.GetTime();

		if (m_bStop) 
		{
			return FALSE;
		}

		if ((time2 - time1) > nSecondeTime)
		{
			break;
		}
		Sleep(1000);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////
//函数说明：
//    获得当前测试工位电脑主机名称
//Add by wanxin
//Time:20131129
/////////////////////////////////////////////////////////////////
void CBasicClass::GetWorkStationHostName(CString& strHostName)
{
	char szHostname[MAX_PATH];
	ZeroMemory(szHostname, sizeof(char)*MAX_PATH);
	gethostname(szHostname, MAX_PATH); 
	strHostName = szHostname;
// 	hostent*   ht;   
// 	ht = gethostbyname(g_szHostname);   
// 	if(ht != NULL&&ht->h_addr_list[0] != NULL)   
// 	{   
// 		LPCSTR lstr=inet_ntoa(*(in_addr*)ht->h_addr_list[0]);     
// 	}
}
/////////////////////////////////////////////////////////
//函数说明：
//    删除文件夹及文件夹下的文件
//Add by wanxin
//Time：20131205
////////////////////////////////////////////////////////
void CBasicClass::DeleteDirectory(CString strDir) 
{ 
	CFileFind	File; 
	BOOL		bFound;

	if(strDir.IsEmpty()) 
	{
		return; 
	}   
	//Delete the sub files and folders at first
	bFound = File.FindFile(strDir+"\\*", 0);

	while(bFound) 
	{ 
		bFound = File.FindNextFile(); 
		if(File.GetFileName() == "."||File.GetFileName() == "..") 
		{
			continue; 
		}
		//Remove the file attribute of only read
		SetFileAttributes(File.GetFilePath(), FILE_ATTRIBUTE_NORMAL); 
		if(File.IsDirectory()) 
		{ 
		  //Delete sub folder of recursion
		  DeleteDirectory(File.GetFilePath()); 
		  RemoveDirectory(File.GetFilePath()); 
		} 
		else 
		{ 
		  //Delete file 
		  DeleteFile(File.GetFilePath()); 
		} 
	} 
	File.Close(); 
   
	//Remove the directory
	RemoveDirectory(strDir); 
} 
/////////////////////////////////////////////////////////
//函数说明：
//    线性内插函数
//Add by wanxin
//Time：20131205
////////////////////////////////////////////////////////
void CBasicClass::RetriverArrayWithLinear(double *pdblRawX, double *pdblRawY, int nRaw, double *pdblResX, double *pdblResY, int nRes, BOOL bIncrease)
{
	double dblx1, dblx2, dbly1, dbly2;
	for (int j = 0; j < nRes; j++)
	{
		if (bIncrease)
		{
			if (pdblResX[j] < pdblRawX[0])
			{
				dblx1 = pdblRawX[0];
				dblx2 = pdblRawX[1];
				dbly1 = pdblRawY[0];
				dbly2 = pdblRawY[1];
			}
			else if (pdblResX[j] > pdblRawX[nRaw-1])
			{
				dblx1 = pdblRawX[nRaw-1];
				dblx2 = pdblRawX[nRaw-2];
				dbly1 = pdblRawY[nRaw-1];
				dbly2 = pdblRawY[nRaw-2];
			}
			else
			{
				for (int i = 0; i < nRaw-1; i++)
				{
					double dblRes = (pdblRawX[i] - pdblResX[j])*(pdblRawX[i+1] - pdblResX[j]);
					if (dblRes <= 0)
					{
						dblx1 = pdblRawX[i];
						dblx2 = pdblRawX[i+1];
						dbly1 = pdblRawY[i];
						dbly2 = pdblRawY[i+1];
						break;
					}
				}
			}
		}
		else
		{
			if (pdblResX[j] > pdblRawX[0])
			{
				dblx1 = pdblRawX[0];
				dblx2 = pdblRawX[1];
				dbly1 = pdblRawY[0];
				dbly2 = pdblRawY[1];
			}
			else if (pdblResX[j] < pdblRawX[nRaw-1])
			{
				dblx1 = pdblRawX[nRaw-1];
				dblx2 = pdblRawX[nRaw-2];
				dbly1 = pdblRawY[nRaw-1];
				dbly2 = pdblRawY[nRaw-2];
			}
			else
			{
				for (int i = 0; i < nRaw-1; i++)
				{
					double dblRes = (pdblRawX[i] - pdblResX[j])*(pdblRawX[i+1] - pdblResX[j]);
					if (dblRes <= 0)
					{
						dblx1 = pdblRawX[i];
						dblx2 = pdblRawX[i+1];
						dbly1 = pdblRawY[i];
						dbly2 = pdblRawY[i+1];
						break;
					}
				}
			}
		}
		if ((dblx2 - dblx1) != 0)
		{
			double k = (dbly2 - dbly1) / (dblx2 - dblx1);
			double c = dbly2 - k * dblx2;
			pdblResY[j] = k * pdblResX[j] + c;	
		}
		else
		{
			pdblResY[j] = dbly1;
		}
	}
}
/////////////////////////////////////////////////////////
//函数说明：
//      获得文件大小函数
//Add by wanxin
//Time：20131205
////////////////////////////////////////////////////////
unsigned long CBasicClass::get_file_size(const char *filename)
{
	struct stat buf;
	if(stat(filename, &buf)<0)
	{		
		return 0;		
	}	
	return (unsigned long)buf.st_size;	
}
/////////////////////////////////////////////////////////
//函数说明：
//     用来创建一个空文件
//Add by wanxin
//Time：20131205
////////////////////////////////////////////////////////
BOOL CBasicClass::CreateEmptyFile(LPCSTR lpFileName)
{
	FILE* fpFile=NULL;
	if ((fpFile= fopen(lpFileName,"w"))==NULL) 
	{
		return FALSE;
	}
	if (fpFile!=NULL)
	{
		fclose(fpFile);
		fpFile=NULL;
	}
	return TRUE;
}
BOOL CBasicClass::LinearFitArray(double *pdblX, double *pdblY, int nCount, double &k, double &c)
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
	
	
	if (pdblTempArray1!=NULL) 
	{
		delete [] pdblTempArray1;
		pdblTempArray1 = NULL;
	}
	if (pdblTempArray2!=NULL)
	{
		delete [] pdblTempArray2;
		pdblTempArray2 = NULL;
	}
	return TRUE;
}

double CBasicClass::Sum(double *pdblArray, int nCount)
{
	
	double dblSum = 0;
	for (int i = 0; i < nCount; i++)
	{
		dblSum += pdblArray[i];
	}
	
	return dblSum;
	
}
////////////////////////////////////////////////////////////////////////////////
//函数说明：
//   从数组中查找double型最大、最小值
//Add by wanxin
//Time:20200525
///////////////////////////////////////////////////////////////////////////////
BOOL CBasicClass::FindMinMaxValue(double *pdbArry, int nCount, double*pdbMaxVal, int *npMaxValIndex, double* dbpdbMinVal, int *npMinValIndex)
{
	double dbMaxVal = -9999999.0;
	int  nMaxIndex = -1;
	double dbMinVal = 99999999.0;
	int  nMinIndex = -1;

	for (int i = 0;i < nCount;i++)
	{
		if (pdbArry[i] > dbMaxVal)
		{
			dbMaxVal = pdbArry[i];
			nMaxIndex = i;
		}
		if (pdbArry[i] < dbMinVal)
		{
			dbMinVal = pdbArry[i];
			nMinIndex = i;
		}
	}

	*pdbMaxVal = dbMaxVal;
	*npMaxValIndex = nMaxIndex;

	*dbpdbMinVal = dbMinVal;
	*npMinValIndex = nMinIndex;

	if (nMaxIndex == -1 || nMinIndex == -1)
	{
		return FALSE;
	}

	return TRUE;
}

CString CBasicClass::ReadIniFileString(LPCTSTR lpstPath, CString strSection, CString strKey)
{
	CString strValue;
	CString strMsg;
	char chBuf[MAX_PATH] = "";
	GetPrivateProfileString(strSection, strKey, "error", chBuf, MAX_PATH, lpstPath);
	strValue = (LPCTSTR)chBuf;
	if (strValue.Find("error") == 0)
	{
		strMsg.Format("open file error:%s(%s)", lpstPath, strKey);
		strValue.Empty();
		return strValue;
	}

	CString strRet;
	strRet.Format("%s", chBuf);

	return strRet;
}
BOOL CBasicClass::SaveIniFile(CString strSection, CString strKey, CString strValue, LPCTSTR pchFilePath)
{
	BOOL   bWriteOK = FALSE;
	bWriteOK = WritePrivateProfileString(strSection, strKey, strValue, pchFilePath);
	return bWriteOK;
}


BOOL CBasicClass::SearchMinMaxValur(double *pdbValue, int nPointCount, double *pdbMinValue, int* pnMinIndex, double *pdbMaxValue, int *pnMaxIndex)
{

	double dbMin = 99999999;
	int nMinIndex = -1;
	double dbMax = -9999999;
	int nMaxIndex = -1;


	for (int i = 0; i < nPointCount; i++)
	{
		if (pdbValue[i] > dbMax)
		{
			dbMax = pdbValue[i];
			nMaxIndex = i;
		}
		if (pdbValue[i] < dbMin)
		{
			dbMin = pdbValue[i];
			nMinIndex = i;
		}
	}
	*pdbMinValue = dbMin;
	*pnMinIndex = nMinIndex;

	*pdbMaxValue = dbMax;
	*pnMaxIndex = nMaxIndex;

	return TRUE;
}