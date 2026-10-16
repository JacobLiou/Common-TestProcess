// BasicClass.h: interface for the CBasicClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASICCLASS_H__3D837D54_B57D_4E4E_90EA_045A2D2AB393__INCLUDED_)
#define AFX_BASICCLASS_H__3D837D54_B57D_4E4E_90EA_045A2D2AB393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math.h"

class CBasicClass  
{
public:
	CBasicClass();
	virtual ~CBasicClass();
	BOOL CreateEmptyFile(LPCSTR lpFileName);
	unsigned long get_file_size(const char *filename);
	CString  ExtractFilePath(CString FileName);
	BOOL CreateFullPath(LPCSTR lpPath);
	BOOL IsTheFileExist(LPCTSTR lpstrFileName);
	void YieldToPeers();
	BOOL FindIndex(double *pdblRawArray, int nRawCount, double dblVal, int &nIndex, BOOL bInCrease = TRUE);
	double CalculateDAC(double dblStratDAC, double dblTargetAtt, double *pdblCoeff);
	BOOL GetSplitStringToIntArry(LPCSTR lpStrData,int *pnGetData,int& nCount);
	BOOL GetSplitStringTodoubleArry(LPCSTR lpStrData,double *pdbGetData,int& nCount);
	BOOL SleepTime(int nSecondeTime);
	void GetWorkStationHostName(CString& strHostName);
	void DeleteDirectory(CString strDir);
	void RetriverArrayWithLinear(double *pdblRawX, double *pdblRawY, int nRaw, double *pdblResX, double *pdblResY, int nRes, BOOL bIncrease);
    BOOL LinearFitArray(double *pdblX, double *pdblY, int nCount, double &k, double &c);
	double Sum(double *pdblArray, int nCount);
	BOOL FindMinMaxValue(double *pdbArry, int nCount, double*pdbMaxVal, int *npMaxValIndex, double *dbpdbMinVal, int *npMinValIndex);
	CString ReadIniFileString(LPCTSTR lpstPath, CString strSection, CString strKey);
	BOOL SaveIniFile(CString strSection, CString strKey, CString strValue, LPCTSTR pchFilePath);
	BOOL SearchMinMaxValur(double *pdbValue, int nPointCount, double *pdbMinValue, int* pnMinIndex, double *pdbMaxValue, int *pnMaxIndex);
public:
	BOOL m_bStop;

};

#endif // !defined(AFX_BASICCLASS_H__3D837D54_B57D_4E4E_90EA_045A2D2AB393__INCLUDED_)
