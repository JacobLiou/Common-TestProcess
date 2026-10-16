// GraphPro.cpp: implementation of the CGraphPro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestProcess.h"
#include "GraphPro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraphPro::CGraphPro()
{
	m_pNetCommand = NULL;
	m_iPlotCount = 0;
	m_bGraph1Init = FALSE;
	m_bGraph2Init = FALSE;
	m_bGraph3Init = FALSE;
	m_bGraph4Init = FALSE;
	ZeroMemory(m_pGraph1PlotHead,10*sizeof(stGraphDataInfo));


}

CGraphPro::~CGraphPro()
{

}

//获取当前错误信息
TCHAR* CGraphPro::GetLastError(TCHAR** lpszErr /* = NULL */)
{
	if(lpszErr != NULL)
	{
		*lpszErr = m_strErr.GetBuffer(0);
	}
	return m_strErr.GetBuffer(0);
}

//初始化图形
BOOL CGraphPro::GraphInit(stGraphInitInfo& stInfo)
{
	ASSERT(m_pNetCommand != NULL);
	if(m_pNetCommand == NULL)
	{
		m_strErr = "数据交互指针为空";
		return FALSE;
	}

	//stInfo.iGraphIndex = 0;
	if(!m_pNetCommand->XmlGraphInit(stInfo))
	{
		m_strErr = m_pNetCommand->GetLastError();
		return FALSE;
	}
	m_iPlotCount = 0;
	return TRUE;
}

//清空图形数据
BOOL CGraphPro::GraphClear(int iGraphIndex)
{
	ASSERT(m_pNetCommand != NULL);
	if(m_pNetCommand == NULL)
	{
		m_strErr = "数据交互指针为空";
		return FALSE;
	}

	stGraphDataInfo stInfo;
	stInfo.iGraphIndex = iGraphIndex;
	stInfo.opt = GRAPH_OPT_NEW;
	stInfo.iDataCount = 0;
	
	if(!m_pNetCommand->XmlGraphData(stInfo))
	{
		m_strErr = m_pNetCommand->GetLastError();
		return FALSE;
	}
	m_iPlotCount = 0;
	return TRUE;
}

//增加主轴曲线
BOOL CGraphPro::PlotAdd(int nGraphIndex,int iColor, LPCTSTR lpstTitle, BOOL bMainY)
{
	ASSERT(m_pNetCommand != NULL);
	if(m_pNetCommand == NULL)
	{
		m_strErr = "数据交互指针为空";
		return FALSE;
	}
	stGraphDataInfo stInfo;
	stInfo.opt = GRAPH_OPT_ADDPLOT;
	stInfo.iPlotIndex = m_iPlotCount + 1;
	stInfo.iColor = iColor;
	double xData = 0;
	double yData = 0;

	if(bMainY)
	{
		stInfo.iYScaleIndex = 0;
	}
	else
	{
		stInfo.iYScaleIndex = 1;
	}
	stInfo.iGraphIndex = nGraphIndex;
	CString strTemp = lpstTitle;
	memcpy(stInfo.pszPlotTitle, strTemp, strTemp.GetLength() + 1);
	stInfo.pdblXData = &xData;
	stInfo.pdblYData  = &yData;
	stInfo.iDataCount = 0;
	if(!m_pNetCommand->XmlGraphData(stInfo))
	{
		m_strErr = m_pNetCommand->GetLastError();
		return FALSE;
	}
	m_pGraph1PlotHead[m_iPlotCount].opt = stInfo.opt;
	m_pGraph1PlotHead[m_iPlotCount].iPlotIndex = stInfo.iPlotIndex;
	m_pGraph1PlotHead[m_iPlotCount].iColor = stInfo.iColor;
	m_pGraph1PlotHead[m_iPlotCount].iYScaleIndex = stInfo.iYScaleIndex;
	memcpy(m_pGraph1PlotHead[m_iPlotCount].pszPlotTitle, strTemp, strTemp.GetLength() + 1);
	m_iPlotCount++;
	return TRUE;
}

//清除曲线
BOOL CGraphPro::PlotClear(int nGraphIndex, int iPlotIndex /* = 1 */)
{
	ASSERT(m_pNetCommand != NULL);
	if(m_pNetCommand == NULL)
	{
		m_strErr = "数据交互指针为空";
		return FALSE;
	}
	stGraphDataInfo stInfo;
	if((iPlotIndex + 1) > m_iPlotCount || iPlotIndex < 0)
	{
		m_strErr = _T("不存在该曲线.");
		return FALSE;
	}
	stInfo.iGraphIndex = nGraphIndex;
	stInfo.opt = GRAPH_OPT_DELPLOT;
	stInfo.iPlotIndex = iPlotIndex + 1;
	if(!m_pNetCommand->XmlGraphData(stInfo))
	{
		m_strErr = m_pNetCommand->GetLastError();
		return FALSE;
	}
	m_iPlotCount--;
	return TRUE;
}

//增加单个点//x曲线索引从0开始
BOOL CGraphPro::PlotXY(int nGraphIndex,double X, double Y, int iPlotIndex)
{
	ASSERT(m_pNetCommand != NULL);
	if(m_pNetCommand == NULL)
	{
		m_strErr = "数据交互指针为空";
		return FALSE;
	}
	stGraphDataInfo stInfo;
	stInfo.opt = GRAPH_OPT_ADDDATA;
	stInfo.iGraphIndex = nGraphIndex;
	stInfo.iPlotIndex = iPlotIndex + 1;
	stInfo.iYScaleIndex = 0;
	stInfo.iDataCount = 1;
	stInfo.pdblXData = &X;
	stInfo.pdblYData = &Y;

	memcpy(stInfo.pszPlotTitle,m_pGraph1PlotHead[iPlotIndex].pszPlotTitle,LENGTH_64);
	stInfo.iColor = m_pGraph1PlotHead[iPlotIndex].iColor;
	stInfo.iGraphIndex = 0;
	if(!m_pNetCommand->XmlGraphData(stInfo))
	{
		m_strErr = m_pNetCommand->GetLastError();
		return FALSE;
	}
	return TRUE;
}

//增加多个点
BOOL CGraphPro::PlotXYS(int nGraphIndex,double* X, double* Y, int iDataCount, int iPlotIndex /* = 0 */, BOOL bRedraw /* = TRUE */)
{
	ASSERT(m_pNetCommand != NULL);
	if(m_pNetCommand == NULL)
	{
		m_strErr = "数据交互指针为空";
		return FALSE;
	}
	stGraphDataInfo stInfo;
	if(bRedraw)
	{
		stInfo.opt = GRAPH_OPT_NEW;
	}
	else
	{
		stInfo.opt = GRAPH_OPT_ADDDATA;
	}
	stInfo.iGraphIndex = nGraphIndex;
	stInfo.iPlotIndex = iPlotIndex + 1;
	stInfo.iYScaleIndex = 0;
	stInfo.iDataCount = iDataCount;
	stInfo.pdblXData = X;
	stInfo.pdblYData = Y;
	memcpy(stInfo.pszPlotTitle,m_pGraph1PlotHead[iPlotIndex].pszPlotTitle,LENGTH_64);
	stInfo.iColor = m_pGraph1PlotHead[iPlotIndex].iColor;

	if(!m_pNetCommand->XmlGraphData(stInfo))
	{
		m_strErr = m_pNetCommand->GetLastError();
		return FALSE;
	}
	return TRUE;
}

void CGraphPro::SetNetCommand(CNetCommand *pNetCommand)
{
	ASSERT(pNetCommand != NULL);
	if(pNetCommand == NULL)
	{
		return;
	}
	m_pNetCommand = pNetCommand;
}
