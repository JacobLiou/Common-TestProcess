// GraphPro.h: interface for the CGraphPro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHPRO_H__B29F01F5_AADF_44B2_94EA_A7263DE4D7BF__INCLUDED_)
#define AFX_GRAPHPRO_H__B29F01F5_AADF_44B2_94EA_A7263DE4D7BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\include\\TpData.h"
#include "NetCommand.h"

class CGraphPro : public CGraphInterface
{
public:
	void SetNetCommand(CNetCommand *pNetCommand);
//	CGraphPro(pstTransInfo pstTotalInfo);
	CGraphPro();
	virtual ~CGraphPro();

	virtual TCHAR* GetLastError(TCHAR** lpszErr = NULL);
	virtual int GetCurPlotCount(void) { return m_iPlotCount; };
	virtual BOOL GraphInit(stGraphInitInfo& stInfo);
	virtual BOOL GraphClear(int iGraphIndex = 0);
	virtual BOOL PlotAdd(int nGraphIndex,int iColor, LPCTSTR lpstTitle = NULL, BOOL bMainY = TRUE);
	virtual BOOL PlotClear(int nGraphIndex, int iPlotIndex = 0);
	virtual BOOL PlotXY(int nGraphIndex,double X, double Y, int iPlotIndex = 0);
	virtual BOOL PlotXYS(int nGraphIndex,double* X, double* Y, int iDataCount, int iPlotIndex = 0, BOOL bRedraw = TRUE);
private:
	//曲线条数统计
	int m_iPlotCount;
	//错误信息
	CString m_strErr;
//	//相关参数传递
//	pstTransInfo m_pstTrans;
	CNetCommand*  m_pNetCommand;


/////   标识 最大4个图形初始化的索引
	BOOL	m_bGraph1Init;
	BOOL	m_bGraph2Init;
	BOOL	m_bGraph3Init;
	BOOL	m_bGraph4Init;
//////// 图形1的曲线参数头，暂时只用在30项目,最大曲线数目为10条
	stGraphDataInfo  m_pGraph1PlotHead[10];

};

#endif // !defined(AFX_GRAPHPRO_H__B29F01F5_AADF_44B2_94EA_A7263DE4D7BF__INCLUDED_)
