// Dlg_AgentClassification.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "Dlg_VehicleClassification.h"


// CDlg_AgentClassification dialog

IMPLEMENT_DYNAMIC(CDlg_AgentClassification, CBaseDialog)

CDlg_AgentClassification::CDlg_AgentClassification(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CDlg_AgentClassification::IDD, pParent)
	, m_Message(_T(""))
	, m_MaxColumnSize(100)
{

	m_XSelectionNo = CLS_demand_type;
	m_YSelectionNo = CLS_Agent_count;
	m_AgentSelectionNo = CLS_network;

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_iItem = -1;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bHighlightSelectedAgents = true;

}

CDlg_AgentClassification::~CDlg_AgentClassification()
{
	m_pDoc->HighlightSelectedAgents(false);
}

void CDlg_AgentClassification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_XAxis, m_ComboX);
	DDX_Control(pDX, IDC_COMBO_YAxis, m_ComboY);
	DDX_Control(pDX, IDC_COMBO_AgentSelection, m_ComboAgentSelection);
	DDX_Control(pDX, ID_HIGHLIGHT_AgentS, m_HighlightAgentButton);
	DDX_Control(pDX, IDC_LIST1, m_MessageList);
	DDX_Control(pDX, IDC_COMBO_MAX_COLUMNS, m_ColumnSizeList);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_MessageEdit);
}


BEGIN_MESSAGE_MAP(CDlg_AgentClassification, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO1, OnRadioButtons)
	ON_BN_CLICKED(IDC_RADIO3, OnRadioColors)
	ON_NOTIFY(NSCS_SELECTEDITEM, IDC_Agent_CHART1, OnChartSelectedItem)
	ON_BN_CLICKED(IDC_RADIO2, OnRadioButtons)
	ON_BN_CLICKED(IDC_RADIO4, OnRadioColors)
	ON_BN_CLICKED(IDC_RADIO5, OnRadioColors)
	ON_CBN_SELCHANGE(IDC_COMBO_XAxis, &CDlg_AgentClassification::OnCbnSelchangeComboXaxis)
	ON_CBN_SELCHANGE(IDC_COMBO_YAxis, &CDlg_AgentClassification::OnCbnSelchangeComboYaxis)
	ON_BN_CLICKED(ID_EXPORT_DATA, &CDlg_AgentClassification::OnBnClickedExportData)
	ON_CBN_SELCHANGE(IDC_COMBO_AgentSelection, &CDlg_AgentClassification::OnCbnSelchangeComboAgentselection)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_HIGHLIGHT_AgentS, &CDlg_AgentClassification::OnBnClickedHighlightAgents)
	ON_CBN_SELCHANGE(IDC_COMBO_MAX_COLUMNS, &CDlg_AgentClassification::OnCbnSelchangeComboMaxColumns)
	ON_BN_CLICKED(ID_EXPORT_ALL_DATA, &CDlg_AgentClassification::OnBnClickedExportAllData)
END_MESSAGE_MAP()


// CDlg_AgentClassification message handlers

BOOL CDlg_AgentClassification::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Subclassing

	m_chart.SubclassDlgItem(IDC_Agent_CHART1,this);

	// Add "About..." menu item to system menu.

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	m_chart.PrepareColors(SimpleColors);
	
	if(m_PresetChartTitle.GetLength () > 0)
		m_chart.m_Caption = m_PresetChartTitle;
	else
		m_chart.m_Caption = "Project Title: " + m_pDoc->m_ProjectTitle ;

	// processing available x and y options

	CWaitCursor wc;

	for(int c = 1; c<=100; c++)
	{
		CString str;
		str.Format("%d",c);
		m_ColumnSizeList.AddString (str);

	}
	m_ColumnSizeList.SetCurSel (99);

// enum Agent_X_CLASSIFICATION {CLS_demand_type=0,CLS_VOT_10,CLS_time_interval_15_min,CLS_information_class,CLS_Agent_type};

	m_ComboX.AddString("All Agents/Agents"); // 0;
	m_ComboX.AddString("Demand Type");
	m_ComboX.AddString ("--"); 
	m_ComboX.AddString("5-min Departure Time Interval"); //2
	m_ComboX.AddString("15-min Departure Time Interval"); //2
	m_ComboX.AddString("30-min Departure Time Interval"); //2
	m_ComboX.AddString("1-hour Departure Time Interval"); 
	m_ComboX.AddString("2-hour Departure Time Interval"); 
	m_ComboX.AddString("4-hour Departure Time Interval"); 
	m_ComboX.AddString ("--"); 
	m_ComboX.AddString("Distance Bin (0.2 dist Interval)"); //2
	m_ComboX.AddString("Distance Bin (1 dist Interval)"); 
	m_ComboX.AddString("Distance Bin (2 dist Interval)"); //2
	m_ComboX.AddString("Distance Bin (5 dist Interval)"); //2
	m_ComboX.AddString("Distance Bin (10 dist Interval)"); //2
	m_ComboX.AddString ("--"); 
	m_ComboX.AddString("Travel Time Bin (2 min Interval)"); //2
	m_ComboX.AddString("Travel Time Bin (5 min Interval)"); //2
	m_ComboX.AddString("Travel Time Bin (10 min Interval)"); //2
	m_ComboX.AddString("Travel Time Bin (30 min Interval)"); //2
	m_ComboX.AddString ("--"); 
	m_ComboX.AddString("Agent Type"); //4

// enum Agent_Y_CLASSIFICATION {CLS_Agent_count=0,CLS_total_travel_time,CLS_avg_travel_time,CLS_total_toll_cost,CLS_avg_toll_cost,CLS_total_generalized_cost,CLS_avg_generalized_cost,CLS_total_travel_distance, CLS_avg_travel_distance,CLS_total_CO2,CLS_avg_CO2};

	m_ComboY.AddString ("Number of Agents");
	m_ComboY.AddString ("Cumulative Number of Agents");
	m_ComboY.AddString ("Total Travel Time (min)"); 
	m_ComboY.AddString ("Total Travel Distance "); 
	m_ComboY.AddString ("--"); 
	m_ComboY.AddString ("Avg Travel Time (min)"); 
	m_ComboY.AddString ("Avg Travel Distance "); 
	m_ComboY.AddString ("Avg Speed (distance/hour)"); 
	m_ComboY.AddString ("Avg Travel Time Per distance (min/distance)"); 
	m_ComboY.AddString ("Travel Time Index: mean/FFTT"); 
	m_ComboY.AddString ("--"); 
	m_ComboY.AddString ("Avg Travel Time STD (min)"); 
	m_ComboY.AddString ("95th Percentile Travel Time (min)");
	m_ComboY.AddString ("90th Percentile Travel Time (min)");
	m_ComboY.AddString ("80th Percentile Travel Time (min)");
	m_ComboY.AddString ("Planning Time Index: 95th percentile/FFTT"); 
	m_ComboY.AddString ("Buffer Index: (95th percentile - mean)/mean"); //(95th percentile travel time ?mean travel time) / mean travel time
	m_ComboY.AddString ("Skew Index: (90th %ile-median)/(median-10th %ile)"); // (90th percentile travel time ?median travel time) / (median travel time ?10th percentile travel time)


	m_ComboY.AddString ("--"); 
	m_ComboY.AddString ("Travel Time Per Distance Unit STD");
	m_ComboY.AddString ("95th Percentile Travel Time Per Distance Unit");
	m_ComboY.AddString ("90th Percentile Travel Time Per Distance Unit");
	m_ComboY.AddString ("80th Percentile Travel Time Per Distance Unit");
	m_ComboY.AddString ("--"); 

	m_ComboX.SetCurSel (m_XSelectionNo);
	m_ComboY.SetCurSel (m_YSelectionNo);

	// CLS_network=0, CLS_OD,CLS_link_set,CLS_path_trip,CLS_subarea
	m_ComboAgentSelection.AddString("Network-wide");
	m_ComboAgentSelection.AddString("Selected OD Pairs from Agent Path Dialog");
	m_ComboAgentSelection.AddString("Passing Selected Link Set");
	m_ComboAgentSelection.AddString ("--"); 
	m_ComboAgentSelection.AddString("Trips Passing Selected Path");
	m_ComboAgentSelection.AddString("Partial Trips Passing Selected Path");

//CLS_subarea_generated,CLS_subarea_traversing_through,CLS_subarea_internal_to_external,CLS_subarea_external_to_internal,
	//CLS_subarea_internal_to_internal_trip};

	m_ComboAgentSelection.AddString ("--"); 
	m_ComboAgentSelection.AddString("Originating from Subarea");
	m_ComboAgentSelection.AddString("Traversing through Subarea");
	m_ComboAgentSelection.AddString("Subarea Internal-to-External Trips");
	m_ComboAgentSelection.AddString("Subarea External-to-Internal Trips");
	m_ComboAgentSelection.AddString("Subarea Internal-to-Internal Trips");
	m_ComboAgentSelection.AddString("Complete and Partial Trips inside Subarea ");
	m_ComboAgentSelection.AddString("Subarea Boundary-to-Boundary Trips");


	m_ComboAgentSelection.SetCurSel (m_pDoc->m_AgentSelectionMode);

	if(m_pDoc==NULL)
		return false;

	m_pDoc->m_ClassificationTable.clear ();
	m_pDoc->GenerateAgentClassificationData(m_pDoc->m_AgentSelectionMode,m_XSelectionNo);
	m_pDoc->GenerateClassificationForDisplay(m_XSelectionNo,m_YSelectionNo);

	// Genernate Agent Classification Data

	AddChartData();
	m_chart.SetChartStyle(NSCS_BAR);

	CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO2);
	CheckRadioButton(IDC_RADIO3,IDC_RADIO5,IDC_RADIO4);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlg_AgentClassification::AddChartData()
{
	m_MessageList.ResetContent ();

	if(m_AgentSelectionNo == CLS_network)
		m_MessageList.AddString("All Agents in the network.");

	if(m_AgentSelectionNo == CLS_OD)
		m_MessageList.AddString("Selected OD pairs in Agent Path Dialog");

	if(m_AgentSelectionNo == CLS_link_set)
	{
			for (std::list<DTALink*>::iterator iLink =  m_pDoc->m_LinkSet.begin(); iLink !=  m_pDoc->m_LinkSet.end(); iLink++)
			{

			if( (*iLink)->m_DisplayLinkID>=0)  // in one of selected links
			{

				CString msg;
				msg.Format("Link %s:%d -> %d",(*iLink)->m_Name .c_str (), (*iLink)->m_FromNodeNumber , (*iLink)->m_ToNodeNumber);
				m_MessageList.AddString(msg);
			}
			
			}
		
	}

	if(m_AgentSelectionNo == CLS_path_trip)
	{

		if(m_pDoc->m_PathDisplayList.size() == 0 || m_pDoc->m_SelectPathNo >= m_pDoc->m_PathDisplayList.size() )
		{
			m_MessageList.AddString("No path has been selected");
			m_MessageList.AddString("Please go to the path dialog to define/select a path.");
		}else
		{
				CString msg;
				msg.Format("There are %d links along the selected path.", m_pDoc->m_PathDisplayList[m_pDoc->m_SelectPathNo].m_LinkVector.size());
				m_MessageList.AddString(msg);
		}

	}


	 m_chart.ResetChart ();
	std::map<int, AgentStatistics> ::const_iterator iter;

	if(m_YSelectionNo == CLS_Agent_count)
		m_chart.m_bIntegerValue = true;
	else
		m_chart.m_bIntegerValue = false;

	m_MaxColumnSize = m_ColumnSizeList.GetCurSel()+1;

	int count = 0;

	 for ( iter = m_pDoc->m_ClassificationTable.begin(); iter != m_pDoc->m_ClassificationTable.end(); ++iter, count++ )
	 {
		 int index = iter->first;
		 if(count<m_MaxColumnSize)
		 {
		 m_chart.AddValue(m_pDoc->m_ClassificationTable[index].DisplayValue ,m_pDoc->m_ClassificationTable[index].Label );
		 }
	 }


}
void CDlg_AgentClassification::OnSysCommand(UINT nID, LPARAM lParam)
{
		CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDlg_AgentClassification::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlg_AgentClassification::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlg_AgentClassification::OnRadioButtons() 
{
	int state = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2);
	
	if(state == IDC_RADIO1)
	{
		m_chart.SetChartStyle(NSCS_PIE);
	}
	else
	{
		m_chart.SetChartStyle(NSCS_BAR);
	}		
}

void CDlg_AgentClassification::OnRadioColors() 
{
	int state = GetCheckedRadioButton(IDC_RADIO3,IDC_RADIO5);
	
	switch(state){
		case IDC_RADIO3:
			m_chart.PrepareColors(GrayScale);
			break;
		case IDC_RADIO4:
			m_chart.PrepareColors(SimpleColors);
			break;
		case IDC_RADIO5:
			m_chart.ResetColors();
			m_chart.AddBrush(new CBrush(HS_BDIAGONAL,0x0));
			m_chart.AddBrush(new CBrush(HS_HORIZONTAL,0x0));
			m_chart.AddBrush(new CBrush(HS_VERTICAL,0x0));
			m_chart.AddBrush(new CBrush(HS_CROSS,0x0));
			m_chart.AddBrush(new CBrush(HS_DIAGCROSS,0x0));
			m_chart.AddBrush(new CBrush(HS_FDIAGONAL,0x0));
			break;
	}
	m_chart.Invalidate(FALSE);
}


// CDlg_AgentClassification message handlers


void CDlg_AgentClassification::OnCbnSelchangeComboXaxis()
{
	CWaitCursor wc;
	m_XSelectionNo = (Agent_X_CLASSIFICATION)(m_ComboX.GetCurSel ());

	m_pDoc->GenerateAgentClassificationData(m_pDoc->m_AgentSelectionMode,m_XSelectionNo);  // regeneate classification based on the new axis category
	m_pDoc->GenerateClassificationForDisplay(m_XSelectionNo,m_YSelectionNo);
	// Genernate Agent Classification Data
	AddChartData();

	Invalidate();
}

void CDlg_AgentClassification::OnCbnSelchangeComboYaxis()
{
	
	m_YSelectionNo = (Agent_Y_CLASSIFICATION)(m_ComboY.GetCurSel ());

	m_pDoc->GenerateClassificationForDisplay(m_XSelectionNo,m_YSelectionNo);
	// Genernate Agent Classification Data

	AddChartData();

	Invalidate();
}
void CDlg_AgentClassification::ExportAllData(CString fname)
{
	   FILE* st;
      fopen_s(&st,fname,"w");

     if(st!=NULL)
      {
	 int CurSel = m_ComboAgentSelection.GetCurSel ();
	 CString AgentSelection_Title;
	 m_ComboAgentSelection.GetLBText (CurSel,AgentSelection_Title);

	 fprintf(st,"Agent Selection Mode,--,%s,--\n",AgentSelection_Title );

		  CWaitCursor wait;
	m_MaxColumnSize = m_ColumnSizeList.GetCurSel()+1;

	for(int x_i= 0; x_i < min(17,m_ComboX.GetCount ()); x_i++)
	{
		m_XSelectionNo = (Agent_X_CLASSIFICATION) x_i; 
		m_pDoc->GenerateAgentClassificationData(m_pDoc->m_AgentSelectionMode,m_XSelectionNo);  // regeneate classification based on the new axis category

		 CString X_Title;
		 m_ComboX.GetLBText (x_i,X_Title);

		CString message;
		message.Format("Processing category %s...", X_Title );
		SetDlgItemTextA(IDC_STATIC_MESSAGE,message);

	for(int y_i= 0; y_i < max(20,m_ComboY.GetCount ()); y_i++)
	 {

		m_YSelectionNo = (Agent_Y_CLASSIFICATION)y_i;

		CString x_value_str,y_value_str;

		m_ComboX.GetLBText(x_i,x_value_str);
		m_ComboY.GetLBText(y_i,y_value_str);

	if(x_value_str.Find("--") >=0 || y_value_str.Find("--") >=0)
		continue;


		m_pDoc->GenerateClassificationForDisplay(m_XSelectionNo,m_YSelectionNo);
	

	string x_str= m_pDoc->CString2StdString (x_value_str);
	string y_str= m_pDoc->CString2StdString (y_value_str);
	

		std::replace( x_str.begin(), x_str.end(), ',', ' '); 
		std::replace( y_str.begin(), y_str.end(), ',', ' '); 

	 std::map<int, AgentStatistics> ::const_iterator iter;
	 int data_count = 0;
	 float data_value = 0;
	 for ( iter = m_pDoc->m_ClassificationTable.begin(); iter != m_pDoc->m_ClassificationTable.end(); ++iter )
	 {
		 int index = iter->first;
		 if(m_pDoc->m_ClassificationTable[index].DisplayValue>0.01f)
		 {
			data_count++;
			data_value = m_pDoc->m_ClassificationTable[index].DisplayValue;
		 }
	 }

	 if(data_count>=1)
	 {
	 fprintf(st,"Category,%s,%s,",x_str.c_str (),y_str.c_str () );


	 if(data_count==1)
	 {
	 CWaitCursor wc;

			 fprintf(st,"%5.2f\n",data_value); 
	 }
	 else  // more than one items
	 {
	 CWaitCursor wc;

		 fprintf(st,"\n");

	 for ( iter = m_pDoc->m_ClassificationTable.begin(); iter != m_pDoc->m_ClassificationTable.end(); ++iter )
	 {
		 int index = iter->first;
		 if(m_pDoc->m_ClassificationTable[index].DisplayValue>0.01f)
		 {

		string label= m_pDoc->CString2StdString ( m_pDoc->m_ClassificationTable[index].Label);
	

		std::replace( label.begin(), label.end(), '-', '_'); 

			fprintf(st,",,%s,%5.2f\n",label.c_str(),m_pDoc->m_ClassificationTable[index].DisplayValue);
		 }
	 }
	 }
	 }
	 }
	}  // x_i selection
	SetDlgItemTextA(IDC_STATIC_MESSAGE,"");
	fclose(st);
		m_pDoc->OpenCSVFileInExcel(fname);

	}else
	 {
		 CString str;
		 str.Format("File %s cannot be opened.",fname);
		AfxMessageBox(str);
	}
}

void CDlg_AgentClassification::ExportData(CString fname)
{
	   FILE* st;
      fopen_s(&st,fname,"w");

     if(st!=NULL)
      {
		  CString y_value_str;

		  m_ComboY.GetLBText(m_ComboY.GetCurSel (),y_value_str);

	 CWaitCursor wc;
	 fprintf(st,"Category,%s\n",y_value_str );

	 std::map<int, AgentStatistics> ::const_iterator iter;
	 for ( iter = m_pDoc->m_ClassificationTable.begin(); iter != m_pDoc->m_ClassificationTable.end(); ++iter )
	 {
		 int index = iter->first;
		 
	    fprintf(st,"%s,%5.2f\n",m_pDoc->m_ClassificationTable[index].Label,m_pDoc->m_ClassificationTable[index].DisplayValue);
	 }
 
		fclose(st);
	}else
	 {
		AfxMessageBox("The selected data file cannot be opened.");
	}
}
void CDlg_AgentClassification::OnBnClickedExportData()
{

	int CurSel = m_ComboX.GetCurSel ();
	 CString X_Title;
	 m_ComboX.GetLBText (CurSel,X_Title);

	CurSel = m_ComboY.GetCurSel ();
	 CString Y_Title;
	 m_ComboY.GetLBText (CurSel,Y_Title);

	CurSel = m_ComboAgentSelection.GetCurSel ();
	 CString AgentSelection_Title;
	 m_ComboAgentSelection.GetLBText (CurSel,AgentSelection_Title);

	 CString Chart_File_Name = m_pDoc->m_ProjectDirectory + AgentSelection_Title + "_"+ X_Title + "_" + Y_Title + ".csv";
	CWaitCursor wait;
	ExportData(Chart_File_Name);

	m_pDoc->OpenCSVFileInExcel(Chart_File_Name);
}

void CDlg_AgentClassification::OnChartSelectedItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCHARTCTRL nmchart = (LPNMCHARTCTRL)pNMHDR;
	
	m_iItem = nmchart->iItem;

	if(m_iItem >= 0)
	{
		m_dValue = (float)(nmchart->dValue);  // convert to float to avoid many decimals
		m_sLabel = nmchart->sLabel;
	}else
	{
		m_dValue = 0;
		m_sLabel = "";
	}
	UpdateData(FALSE);
	UpdateDialogControls(this,FALSE);
	*pResult = FALSE;
}

void CDlg_AgentClassification::OnCbnSelchangeComboAgentselection()
{
	CWaitCursor wait;

	m_AgentSelectionNo = (Agent_CLASSIFICATION_SELECTION)(m_ComboAgentSelection.GetCurSel ());
	m_pDoc->m_AgentSelectionMode = m_AgentSelectionNo;

	m_Message = "";
    //test Agent selection mode here
	// subarea
	if(m_pDoc->m_AgentSelectionMode >= CLS_subarea_generated && m_pDoc->m_AgentSelectionMode <=CLS_subarea_internal_to_internal_trip)
	{
			if(m_pDoc->m_SubareaLinkSet.size() == 0)
			{
				m_Message = "Please define a subarea first.";
				UpdateData(1);
				return;
			}else
			{
			// mark all links in subarea
			m_pDoc->MarkLinksInSubarea();
			}
	}

	m_pDoc->GenerateAgentClassificationData(m_pDoc->m_AgentSelectionMode,m_XSelectionNo);  // regeneate classification based on the new axis category
	// Genernate Agent Classification Data
	m_pDoc->GenerateClassificationForDisplay(m_XSelectionNo,m_YSelectionNo);
	AddChartData();

	UpdateData(1);



	Invalidate();
}

void CDlg_AgentClassification::OnClose()
{
	// TODO: Add your message handler code here and/or call default

   int nRet = 5; 
   EndDialog(nRet); 

}

void CDlg_AgentClassification::OnBnClickedHighlightAgents()
{
	m_pDoc->HighlightSelectedAgents(m_bHighlightSelectedAgents);
	if(m_bHighlightSelectedAgents)
		m_HighlightAgentButton.SetWindowTextA ("Hide Selected Agents");
	else
		m_HighlightAgentButton.SetWindowTextA ("Highligh Selected Agents");

	m_bHighlightSelectedAgents = !m_bHighlightSelectedAgents;

}



void CDlg_AgentClassification::OnCbnSelchangeComboMaxColumns()
{
	AddChartData();

	Invalidate();
}

void CDlg_AgentClassification::OnBnClickedExportAllData()
{

	 int CurSel = m_ComboAgentSelection.GetCurSel ();
	 CString AgentSelection_Title;
	 m_ComboAgentSelection.GetLBText (CurSel,AgentSelection_Title);

	 CString Chart_File_Name = m_pDoc->m_ProjectDirectory + AgentSelection_Title +"_all_data.csv";

	 ExportAllData(Chart_File_Name);

}
