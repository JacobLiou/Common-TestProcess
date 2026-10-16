// TestProcess.h : main header file for the TESTPROCESS DLL
//

#if !defined(AFX_TESTPROCESS_H__11006444_C0BC_4485_9C8C_F4AE5FE4C29E__INCLUDED_)
#define AFX_TESTPROCESS_H__11006444_C0BC_4485_9C8C_F4AE5FE4C29E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
/////////////////////////////////////////////////////////////////////////////
// CTestProcessApp
// See TestProcess.cpp for the implementation of this class
//

class CTestProcessApp : public CWinApp
{
public:
	CTestProcessApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestProcessApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTestProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_TESTPROCESS_H__11006444_C0BC_4485_9C8C_F4AE5FE4C29E__INCLUDED_)
