// Dlg_Find_Agent.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "Dlg_Find_Vehicle.h"


// CDlg_Find_Agent dialog

IMPLEMENT_DYNAMIC(CDlg_Find_Agent, CDialog)

CDlg_Find_Agent::CDlg_Find_Agent(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Find_Agent::IDD, pParent)
	, m_AgentID(0)
{

}

CDlg_Find_Agent::~CDlg_Find_Agent()
{
}

void CDlg_Find_Agent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Agent_ID, m_AgentID);
}


BEGIN_MESSAGE_MAP(CDlg_Find_Agent, CDialog)
END_MESSAGE_MAP()


// CDlg_Find_Agent message handlers
