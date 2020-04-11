// Dlg_DisplayConfiguration.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "Dlg_DisplayConfiguration.h"
#include "MainFrm.h"

// CDlg_DisplayConfiguration dialog

IMPLEMENT_DYNAMIC(CDlg_DisplayConfiguration, CBaseDialog)

CDlg_DisplayConfiguration::CDlg_DisplayConfiguration(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CDlg_DisplayConfiguration::IDD, pParent)
	, m_bShowSignalNodeMovementOnly(TRUE)
{
	m_ShowNodeTextMode = node_display_none;

}

CDlg_DisplayConfiguration::~CDlg_DisplayConfiguration()
{
}

void CDlg_DisplayConfiguration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LINK_TEXT_LABEL, m_Link_Label);
	DDX_Control(pDX, IDC_LIST_NODE_TEXT_LABEL, m_Node_Label);
	//	DDX_Control(pDX, IDC_MOE_AGGREGATION_INTERVAL_LIST, m_AggregationIntervalList);
	DDX_Control(pDX, IDC_LIST_MOVEMENT_TEXT_LABEL, m_Movement_Label);
	//	DDX_Control(pDX, IDC_LIST_GPS_TEXT_LABEL, m_GPS_Label);
	DDX_Check(pDX, IDC_CHECK_SIGNAL_NODE_ONLY, m_bShowSignalNodeMovementOnly);
}


BEGIN_MESSAGE_MAP(CDlg_DisplayConfiguration, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_LINK_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnSelchangeListLinkTextLabel)
	ON_LBN_SELCHANGE(IDC_LIST_NODE_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnSelchangeListNodeTextLabel)
	ON_LBN_SELCHANGE(IDC_MOE_AGGREGATION_INTERVAL_LIST, &CDlg_DisplayConfiguration::OnLbnSelchangeMoeAggregationIntervalList)
	ON_LBN_SELCHANGE(IDC_LIST_ZONE_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnSelchangeListZoneTextLabel)
	ON_BN_CLICKED(IDOK, &CDlg_DisplayConfiguration::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST_MOVEMENT_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnSelchangeListMovementTextLabel)
	ON_LBN_SELCHANGE(IDC_LIST_GPS_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnSelchangeListGpsTextLabel)
	ON_LBN_DBLCLK(IDC_LIST_Size_Text_Control, &CDlg_DisplayConfiguration::OnLbnDblclkListSizeTextControl)
	ON_CBN_SELCHANGE(IDC_COMBO_WalkingDistance, &CDlg_DisplayConfiguration::OnCbnSelchangeComboWalkingdistance)
	ON_CBN_SELCHANGE(IDC_COMBO_TransitTime, &CDlg_DisplayConfiguration::OnCbnSelchangeComboTransittime)
	ON_LBN_SELCHANGE(IDC_LIST_Size_Text_Control, &CDlg_DisplayConfiguration::OnLbnSelchangeListSizeTextControl)
	ON_BN_CLICKED(IDC_BUTTON_INCREASE_SIZE, &CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseSize)
	ON_BN_CLICKED(IDC_BUTTON_DECREASE_SIZE, &CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseSize)
	ON_BN_CLICKED(IDC_CHECK_SIGNAL_NODE_ONLY, &CDlg_DisplayConfiguration::OnBnClickedCheckSignalNodeOnly)
	ON_BN_CLICKED(IDC_BUTTON_INCREASE_TEXT_SIZE, &CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseTextSize)
	ON_BN_CLICKED(IDC_BUTTON_DECREASE_TEXT_SIZE, &CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseTextSize)
	ON_BN_CLICKED(IDC_BUTTON_INCREASE_NODE_TEXT_SIZE, &CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseNodeTextSize)
	ON_BN_CLICKED(IDC_BUTTON_DECREASE_NODE_TEXT_SIZE, &CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseNodeTextSize)
	ON_BN_CLICKED(IDC_BUTTON_INCREASE_LINK_TEXT_SIZE2, &CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseLinkTextSize2)
	ON_BN_CLICKED(IDC_BUTTON_DECREASE_LINK_TEXT_SIZE2, &CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseLinkTextSize2)
	ON_CBN_SELCHANGE(IDC_COMBOTIMINGPLAN, &CDlg_DisplayConfiguration::OnCbnSelchangeCombotimingplan)
	ON_CBN_SELCHANGE(IDC_COMBO_Aggregation_Interval, &CDlg_DisplayConfiguration::OnCbnSelchangeComboAggregationInterval)
	ON_LBN_SELCHANGE(IDC_LIST_GPS_CAR_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnSelchangeListGpsCarTextLabel)
	ON_LBN_DBLCLK(IDC_LIST_GPS_CAR_TEXT_LABEL, &CDlg_DisplayConfiguration::OnLbnDblclkListGpsCarTextLabel)
	ON_LBN_SELCHANGE(IDC_LIST_GPS_DAY_TEXT_LABEL2, &CDlg_DisplayConfiguration::OnLbnSelchangeListGpsDayTextLabel2)
	ON_LBN_DBLCLK(IDC_LIST_GPS_DAY_TEXT_LABEL2, &CDlg_DisplayConfiguration::OnLbnDblclkListGpsDayTextLabel2)
END_MESSAGE_MAP()


// CDlg_DisplayConfiguration message handlers

void CDlg_DisplayConfiguration::OnLbnSelchangeListLinkTextLabel()
{
	pView->m_ShowLinkTextMode  = (link_text_display_mode)m_Link_Label.GetCurSel();;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnLbnSelchangeListNodeTextLabel()
{
	int m_ShowNodeTextMode =  m_Node_Label.GetCurSel();
	pView->m_ShowNodeTextMode = (node_display_mode)m_ShowNodeTextMode;
	pView->Invalidate ();

}

BOOL CDlg_DisplayConfiguration::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_Node_Label.AddString("None");
	m_Node_Label.AddString("Node ID");
	m_Node_Label.AddString("Sequential Node No.");
	m_Node_Label.AddString("Node Name");
	//m_Node_Label.AddString("Control Type");
	//m_Node_Label.AddString("Distance from Source Node in Mile");


	m_Node_Label.SetCurSel ((int)(m_ShowNodeTextMode));

	m_Link_Label.AddString("None");
	m_Link_Label.AddString("Name");
	m_Link_Label.AddString("Link ID");
	m_Link_Label.AddString("From ID -> To ID");

	CTLiteDoc* m_pDoc = pView->GetDocument();

	m_Link_Label.AddString("Length");
	m_Link_Label.AddString("Free Speed");

	m_Link_Label.AddString("# of Lanes ");

	m_Link_Label.AddString("Lane Capacity");
	m_Link_Label.AddString("Link Capacity");
//	m_Link_Label.AddString("Demand Type Code");

	m_Link_Label.AddString("Free Flow Travel Time");

	m_Link_Label.AddString("Link type In Text");
	m_Link_Label.AddString("Link type In Number");

//	m_Link_Label.AddString("Total Link Volume");
//	m_Link_Label.AddString("Level Of Service");

//	m_Link_Label.AddString("Avg Speed");
//	m_Link_Label.AddString("Avg Travel Time");
//	m_Link_Label.AddString("Avg Delay");


	m_Link_Label.SetCurSel ((int)(pView->m_ShowLinkTextMode));



	m_Movement_Label.AddString("None");
	m_Movement_Label.AddString ("Turn Type");
	m_Movement_Label.AddString("Turn Direction");
	m_Movement_Label.AddString("ib_lane");
	m_Movement_Label.AddString("ob_lane");
	m_Movement_Label.AddString("penalty");
	m_Movement_Label.AddString("capacity");

		//	m_Movement_Label.AddString ("# of Lanes");
//	m_Movement_Label.AddString ("Simulated Hourly Count");
//	m_Movement_Label.AddString ("Simulated Turning %");
//	m_Movement_Label.AddString ("Simulated Turn Delay (sec)");


	m_Movement_Label.SetCurSel ((int)(pView->m_ShowMovementTextMode));

	//m_GPS_Label.AddString("None");
	//m_GPS_Label.AddString("Agent ID");
	//m_GPS_Label.AddString("Timestamp in min");
	//m_GPS_Label.AddString("Time Gap in min");
	//m_GPS_Label.AddString("GPS Speed");
	//m_GPS_Label.AddString("All Trajectories");
	//m_GPS_Label.SetCurSel ((int)(pView->m_ShowGPSTextMode));


	movement_text_size_vector.push_back(10);
	movement_text_size_vector.push_back(50);
	movement_text_size_vector.push_back(100);
	movement_text_size_vector.push_back(150);
	movement_text_size_vector.push_back(200);
	movement_text_size_vector.push_back(250);
	movement_text_size_vector.push_back(300);
	movement_text_size_vector.push_back(500);
	movement_text_size_vector.push_back(1000);
	movement_text_size_vector.push_back(2000);
	movement_text_size_vector.push_back(5000);

	



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_DisplayConfiguration::OnLbnSelchangeMoeAggregationIntervalList()
{
//	if (m_AggregationValueVector.size() >=  m_AggregationIntervalList.GetCurSel()1)
//	g_MOEAggregationIntervalInMin = m_AggregationValueVector [ m_AggregationIntervalList.GetCurSel()];
//
////
//	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnLbnSelchangeListZoneTextLabel()
{
	// TODO: Add your control notification handler code here
}

void CDlg_DisplayConfiguration::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlg_DisplayConfiguration::OnLbnSelchangeListMovementTextLabel()
{
	pView->m_ShowMovementTextMode  = (movement_text_display_mode)m_Movement_Label.GetCurSel();

	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnLbnSelchangeListGpsTextLabel()
{

	//pView->m_ShowGPSTextMode  = (GPS_display_mode)m_GPS_Label.GetCurSel();

	//pView->Invalidate ();
}


void CDlg_DisplayConfiguration::OnBnClickedButtonIncreasenodesize()
{
	CTLiteDoc* pDoc = pView->GetDocument();

	pDoc->m_NodeDisplaySize *=1.2;

	pView->Invalidate ();
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->UpdateLegendView();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonDecreasenodesize()
{
	CTLiteDoc* pDoc = pView->GetDocument();

	pDoc->m_NodeDisplaySize /=1.2;
	pView->Invalidate ();
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->UpdateLegendView();

}

void CDlg_DisplayConfiguration::OnBnClickedButtonIncreasenodetextsize()
{
	CTLiteDoc* pDoc = pView->GetDocument();

	pDoc->m_NodeTextDisplayRatio *=1.1;

	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonDecreasenodetextsize()
{
	CTLiteDoc* pDoc = pView->GetDocument();

	pDoc->m_NodeTextDisplayRatio /=1.1;

	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnLbnDblclkListSizeTextControl()
{
	CTLiteDoc* pDoc = pView->GetDocument();



	int CurSelection = m_SizeTextControl_List.GetCurSel();

	switch (CurSelection)
	{
	case 0: pDoc->m_NodeDisplaySize *=1.2; break;
	case 1: pDoc->m_NodeDisplaySize /=1.2; break;
	case 2: 	pDoc->m_NodeTextDisplayRatio *=1.1; break;
	case 3: 	pDoc->m_NodeTextDisplayRatio /=1.1; break;
	case 4: 	pView->m_LinkTextFontSize = max(pView->m_LinkTextFontSize+1,pView->m_LinkTextFontSize * 1.1); break;
	case 5: 	pView->m_LinkTextFontSize = max(1,min (pView->m_LinkTextFontSize-1,pView->m_LinkTextFontSize/1.1)); break;

	}

	pView->Invalidate ();
		
}

void CDlg_DisplayConfiguration::OnCbnSelchangeComboWalkingdistance()
{
}

void CDlg_DisplayConfiguration::OnCbnSelchangeComboTransittime()
{
	// pView->GetDocument ()->m_max_accessible_transit_time_in_min = transit_time_vector [ m_ComboxBox_TransitTime.GetCurSel()];
	//pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnLbnSelchangeListSizeTextControl()
{
	// TODO: Add your control notification handler code here
}

void CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseSize()
{
	CTLiteDoc* pDoc = pView->GetDocument();

	pDoc->m_MovementTextBoxSizeInDistance *=1.1;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseSize()
{
	CTLiteDoc* pDoc = pView->GetDocument();

	pDoc->m_MovementTextBoxSizeInDistance /=1.1;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedCheckSignalNodeOnly()
{
	UpdateData();
	CTLiteDoc* pDoc = pView->GetDocument();
	pDoc->m_bShowSignalNodeMovementOnly = m_bShowSignalNodeMovementOnly;

	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseTextSize()
{
	pView->m_MovmentTextSize*=1.1;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseTextSize()
{
	pView->m_MovmentTextSize/=1.1;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseNodeTextSize()
{
	pView->OnViewIncreasenodesize();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseNodeTextSize()
{
	pView->OnViewDecreatenodesize();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonIncreaseLinkTextSize2()
{
	pView->m_LinkTextSize*=1.1;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnBnClickedButtonDecreaseLinkTextSize2()
{
	pView->m_LinkTextSize/=1.1;
	pView->Invalidate ();
}

void CDlg_DisplayConfiguration::OnCbnSelchangeCombotimingplan()
{
	CString str;
	m_TimingPlanComboBox.GetLBText(m_TimingPlanComboBox.GetCurSel(), str);

	CTLiteDoc* m_pDoc = pView->GetDocument();

	 m_pDoc->m_CurrentDisplayTimingPlanName =  m_pDoc->CString2StdString (str); 
}


void CDlg_DisplayConfiguration::OnCbnSelchangeComboAggregationInterval()
{

	g_MOEAggregationIntervalInMin = aggregation_interval_vector [ m_Combo_Aggregation_Interval.GetCurSel()];

	Invalidate ();
}


void CDlg_DisplayConfiguration::OnLbnSelchangeListGpsCarTextLabel()
{
	CString s;
	m_AgentIDList.GetText(m_AgentIDList.GetCurSel(),s);

	g_SelectedAgentID = pView->GetDocument()->CString2StdString(s);
	pView->Invalidate();
}


void CDlg_DisplayConfiguration::OnLbnDblclkListGpsCarTextLabel()
{
	CString s;
	m_AgentIDList.GetText(m_AgentIDList.GetCurSel(), s);

	g_SelectedAgentID = pView->GetDocument()->CString2StdString(s);
	pView->Invalidate();	// TODO: Add your control notification handler code here
}


void CDlg_DisplayConfiguration::OnLbnSelchangeListGpsDayTextLabel2()
{
	CString s;
	m_PassengerIDList.GetText(m_PassengerIDList.GetCurSel(), s);

	g_SelectedPassengerID = pView->GetDocument()->CString2StdString(s);

	pView->Invalidate();	// TODO: Add your control notification handler code here
}


void CDlg_DisplayConfiguration::OnLbnDblclkListGpsDayTextLabel2()
{
	CString s;
	m_PassengerIDList.GetText(m_PassengerIDList.GetCurSel(), s);

	g_SelectedPassengerID = pView->GetDocument()->CString2StdString(s);
	pView->Invalidate();
}
