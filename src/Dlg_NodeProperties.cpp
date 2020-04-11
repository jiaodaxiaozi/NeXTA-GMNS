// Dlg_NodeProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "Dlg_NodeProperties.h"


// CDlg_NodeProperties dialog

IMPLEMENT_DYNAMIC(CDlg_NodeProperties, CDialog)

CDlg_NodeProperties::CDlg_NodeProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_NodeProperties::IDD, pParent)
	, NodeID(0)
	, NodeName(_T(""))
	, ZoneID(0)
	, m_ControlType(_T(""))
	, m_NodeType(_T(""))
{

}

CDlg_NodeProperties::~CDlg_NodeProperties()
{
}

void CDlg_NodeProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NODE_ID, NodeID);
	DDX_Text(pDX, IDC_EDIT_STREET_NAME, NodeName);
	DDV_MaxChars(pDX, NodeName, 100);
	DDX_Text(pDX, IDC_EDIT_ZoneID, ZoneID);
	DDV_MinMaxLong(pDX, ZoneID, -1, 1000000);
	DDX_Text(pDX, IDC_EDIT_ControlType, m_ControlType);
	DDX_Text(pDX, IDC_EDIT_NodeType, m_NodeType);
}


BEGIN_MESSAGE_MAP(CDlg_NodeProperties, CDialog)
	ON_BN_CLICKED(IDOK, &CDlg_NodeProperties::OnBnClickedOk)
	ON_BN_CLICKED(ID_CANCEL, &CDlg_NodeProperties::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlg_NodeProperties message handlers

void CDlg_NodeProperties::OnBnClickedOk()
{
	UpdateData();


	OnOK();
}

BOOL CDlg_NodeProperties::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_NodeProperties::OnBnClickedCancel()
{
	OnCancel();
}






