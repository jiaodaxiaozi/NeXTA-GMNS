// DlgFileLoading.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "DlgFileLoading.h"


// CDlgFileLoading dialog

IMPLEMENT_DYNAMIC(CDlgFileLoading, CDialog)

CDlgFileLoading::CDlgFileLoading(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFileLoading::IDD, pParent)
{

}

CDlgFileLoading::~CDlgFileLoading()
{
}

void CDlgFileLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NETEWORK_FILE, m_ListBox_NetworkData);
	DDX_Control(pDX, IDC_LIST_OUTPUTDTA, m_ListBox_OutputData);
}


BEGIN_MESSAGE_MAP(CDlgFileLoading, CDialog)
END_MESSAGE_MAP()


// CDlgFileLoading message handlers

BOOL CDlgFileLoading::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(unsigned int i = 0; i< m_pDoc->m_MessageStringVector.size(); i++)
	{
	m_ListBox_NetworkData.AddString (m_pDoc->m_MessageStringVector[i]);
	}

	m_ListBox_NetworkData.AddString (m_pDoc->m_NodeDataLoadingStatus);
	
	if(m_pDoc->m_SignalDataLoadingStatus.GetLength() > 0)
	{
		m_ListBox_NetworkData.AddString(m_pDoc->m_SignalDataLoadingStatus);
	}
	m_ListBox_NetworkData.AddString (m_pDoc->m_LinkDataLoadingStatus);
	


	

	m_ListBox_NetworkData.AddString (m_pDoc->m_DemandDataLoadingStatus);
	
	if(m_pDoc->m_BackgroundBitmapLoaded)
	m_ListBox_NetworkData.AddString (m_pDoc->m_BackgroundImageFileLoadingStatus);
	

	if(m_pDoc->m_SimulationLinkTDMOEDataLoadingStatus.GetLength ()>0)
	{
	m_ListBox_OutputData.AddString (m_pDoc->m_SimulationLinkTDMOEDataLoadingStatus);
	}
	
	

	if(m_pDoc->m_SimulationAgentDataLoadingStatus.GetLength ()>0)
	{
	m_ListBox_OutputData.AddString(m_pDoc->m_SimulationAgentDataLoadingStatus);
	}

	
	if(m_pDoc->m_MovementDataLoadingStatus.GetLength ()>0)
	{
		m_ListBox_NetworkData.AddString(m_pDoc->m_MovementDataLoadingStatus);
	}

	if(m_pDoc->m_AgentLocationLoadingStatus.GetLength ()>0)
	{
	m_ListBox_OutputData.AddString(m_pDoc->m_AgentLocationLoadingStatus);
	}

	m_ListBox_OutputData.AddString(m_pDoc->m_StrLoadingTime);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgFileLoading::OnLbnSelchangeListSensorFile()
{
	// TODO: Add your control notification handler code here
}
