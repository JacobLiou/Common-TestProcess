#ifndef _GRAPH_INTERFACE_H
#define _GRAPH_INTERFACE_H

class CGraphInterface
{
public:
	virtual TCHAR* GetLastError(TCHAR** lpszErr = NULL) = 0;
	virtual int GetCurPlotCount(void) = 0;
	virtual BOOL GraphInit(stGraphInitInfo& stInfo) = 0;
	virtual BOOL GraphClear(int iGraphIndex = 1) = 0;
	virtual BOOL PlotAdd(int nGraphIndex,int iColor, LPCTSTR lpstTitle = NULL, BOOL bMainY = TRUE) = 0;
	virtual BOOL PlotClear(int nGraphIndex, int iPlotIndex = 1) = 0;
	virtual BOOL PlotXY(int nGraphIndex, double X, double Y, int iPlotIndex = 0) = 0;
	virtual BOOL PlotXYS(int nGraphIndex, double* X, double* Y, int iDataCount, int iPlotIndex = 0, BOOL bRedraw = TRUE) = 0;
};

#endif