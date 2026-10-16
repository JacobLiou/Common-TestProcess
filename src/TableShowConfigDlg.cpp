// TableShowConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WSSAutoTestSystem.h"
#include "TableShowConfigDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableShowConfigDlg dialog


CTableShowConfigDlg::CTableShowConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTableShowConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableShowConfigDlg)
	//}}AFX_DATA_INIT
}


void CTableShowConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableShowConfigDlg)
	DDX_Control(pDX, IDC_CHECK_TABLE4, m_ctrlCheck4);
	DDX_Control(pDX, IDC_CHECK_TABLE3, m_ctrlCheck3);
	DDX_Control(pDX, IDC_CHECK_TABLE2, m_ctrlCheck2);
	DDX_Control(pDX, IDC_CHECK_TABLE1, m_ctrlCheck1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableShowConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CTableShowConfigDlg)
	ON_BN_CLICKED(IDC_CHECK_TABLE1, OnCheckTable1)
	ON_BN_CLICKED(IDC_CHECK_TABLE2, OnCheckTable2)
	ON_BN_CLICKED(IDC_CHECK_TABLE3, OnCheckTable3)
	ON_BN_CLICKED(IDC_CHECK_TABLE4, OnCheckTable4)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableShowConfigDlg message handlers

void CTableShowConfigDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CTableShowConfigDlg::OnCheckTable1() 
{
	// TODO: Add your control notification handler code here
/*	CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (m_ctrlCheck1.GetCheck()==0)
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph1.ShowWindow(SW_HIDE);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í1±»Òþ²Ø",COLOR_BLUE);
	}
	else
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph1.ShowWindow(SW_SHOW);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í1»Ö¸´ÏÔÊ¾",COLOR_BLUE);
	}
	*/
}

void CTableShowConfigDlg::OnCheckTable2() 
{
	// TODO: Add your control notification handler code here
/*	CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (m_ctrlCheck2.GetCheck()==0)
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph2.ShowWindow(SW_HIDE);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í2±»Òþ²Ø",COLOR_BLUE);
	}
	else
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph2.ShowWindow(SW_SHOW);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í2»Ö¸´ÏÔÊ¾",COLOR_BLUE);
	}	*/
}

void CTableShowConfigDlg::OnCheckTable3() 
{
	// TODO: Add your control notification handler code here
/*	CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (m_ctrlCheck3.GetCheck()==0)
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph3.ShowWindow(SW_HIDE);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í3±»Òþ²Ø",COLOR_BLUE);
	}
	else
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph3.ShowWindow(SW_SHOW);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í3»Ö¸´ÏÔÊ¾",COLOR_BLUE);
	}*/
}

void CTableShowConfigDlg::OnCheckTable4() 
{
	// TODO: Add your control notification handler code here
/*	CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (m_ctrlCheck4.GetCheck()==0)
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph4.ShowWindow(SW_HIDE);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í4±»Òþ²Ø",COLOR_BLUE);
	}
	else
	{
        pMainWnd->m_pShowView->m_Graphdlg.m_Graph4.ShowWindow(SW_SHOW);
		pMainWnd->m_plogDlg->LogInfo("Í¼±í4»Ö¸´ÏÔÊ¾",COLOR_BLUE);
	}*/	
}

BOOL CTableShowConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame *pMainWnd = (CMainFrame*)AfxGetApp()->GetMainWnd();
// 	if (pMainWnd->m_pShowView->m_bGraph1)
// 	{
//        	m_ctrlCheck1.SetCheck(1);
// 	}
// 	else
// 	{
//         m_ctrlCheck1.SetCheck(0);
// 	}
// 	if (pMainWnd->m_pShowView->m_bGraph2)
// 	{
//        	m_ctrlCheck2.SetCheck(1);
// 	}
// 	else
// 	{
//         m_ctrlCheck2.SetCheck(0);
// 	}
// 	if (pMainWnd->m_pShowView->m_bGraph3)
// 	{
//        	m_ctrlCheck3.SetCheck(1);
// 	}
// 	else
// 	{
//         m_ctrlCheck3.SetCheck(0);
// 	}
// 	if (pMainWnd->m_pShowView->m_bGraph4)
// 	{
//        	m_ctrlCheck4.SetCheck(1);
// 	}
// 	else
// 	{
//         m_ctrlCheck4.SetCheck(0);
// 	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTableShowConfigDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}
