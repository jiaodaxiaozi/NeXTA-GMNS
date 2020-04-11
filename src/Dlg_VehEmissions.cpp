// Dlg_VehEmissions.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "Dlg_VehEmissions.h"
#include "CGridListCtrlEx\CGridColumnTraitEdit.h"
#include "CGridListCtrlEx\CGridColumnTraitCombo.h"
#include "CGridListCtrlEx\CGridRowTraitXP.h"
#include "Dlg_VehicleClassification.h"
#define MAX_STRING_LENGTH  100

extern CDlg_VehPathAnalysis* g_pAgentPathDlg;

// CDlg_VehPathAnalysis dialog

IMPLEMENT_DYNAMIC(CDlg_VehPathAnalysis, CBaseDialog)

CDlg_VehPathAnalysis::CDlg_VehPathAnalysis(CWnd* pParent /*=NULL*/)
: CBaseDialog(CDlg_VehPathAnalysis::IDD, pParent)
, m_SingleAgentID(0)

{
	m_SelectedPath = -1;
	m_SelectedOrigin = -1;
	m_SelectedDestination = -1;
	m_ZoneNoSize  = 0;
	m_ODMOEMatrix = NULL;

	m_ProjectSize =  0;
	m_OldProjectSize = 0;

}

CDlg_VehPathAnalysis::~CDlg_VehPathAnalysis()
{
	m_pDoc->m_CriticalOriginZone = -1;
	m_pDoc->m_CriticalDestinationZone  = -1;

	if(m_ODMOEMatrix !=NULL)
		Deallocate3DDynamicArray<AgentStatistics>(m_ODMOEMatrix,m_ProjectSize, m_pDoc->m_ZoneNoSize );

	std::vector<int> LinkVector;
	// empty vector
	m_pDoc->HighlightPath(LinkVector,1);

}

void CDlg_VehPathAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	DDX_Control(pDX, IDC_PATHLIST, m_PathListCtrl);
	DDX_Control(pDX, IDC_LIST_Agent, m_AgentList);
	DDX_Control(pDX, IDC_COMBO_Origin, m_OriginBox);
	DDX_Control(pDX, IDC_COMBO_Destination, m_DestinationBox);
	DDX_Control(pDX, IDC_COMBO_DepartureTime, m_DepartureTimeBox);
	DDX_Control(pDX, IDC_COMBO_Min_Number_of_Agents, m_MinAgentSizeBox);
	DDX_Control(pDX, IDC_COMBO_Min_Travel_Time, m_MinDistanceBox);
	DDX_Control(pDX, IDC_COMBO_Min_TravelTimeIndex, m_MaxSpeedBox);
	DDX_Control(pDX, IDC_COMBO_TimeInterval, m_TimeIntervalBox);
	DDX_Control(pDX, IDC_COMBO_ImpactLink, m_ImpactLinkBox);
	DDX_Control(pDX, IDC_SUMMARY_INFO, m_Summary_Info_Edit);
	DDX_Control(pDX, IDC_COMBO_DemandType, m_DemandTypeBox);
	DDX_Control(pDX, IDC_COMBO_DayNo, m_DayNo_Combobox);
}


BEGIN_MESSAGE_MAP(CDlg_VehPathAnalysis, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_LINK, &CDlg_VehPathAnalysis::OnLbnSelchangeListLink)
	ON_LBN_SELCHANGE(IDC_LIST_Agent, &CDlg_VehPathAnalysis::OnLbnSelchangeListAgent)
	ON_LBN_SELCHANGE(IDC_LIST_LINK2, &CDlg_VehPathAnalysis::OnLbnSelchangeListLink2)
	ON_CBN_SELCHANGE(IDC_COMBO_Origin, &CDlg_VehPathAnalysis::OnCbnSelchangeComboOrigin)
	ON_CBN_SELCHANGE(IDC_COMBO_Destination, &CDlg_VehPathAnalysis::OnCbnSelchangeComboDestination)
	ON_CBN_SELCHANGE(IDC_COMBO_DepartureTime, &CDlg_VehPathAnalysis::OnCbnSelchangeComboDeparturetime)
	ON_CBN_SELCHANGE(IDC_COMBO_AgentType, &CDlg_VehPathAnalysis::OnCbnSelchangeComboAgenttype)
	ON_CBN_SELCHANGE(IDC_COMBO_TimeInterval, &CDlg_VehPathAnalysis::OnCbnSelchangeComboTimeinterval)
	ON_CBN_SELCHANGE(IDC_COMBO_Min_Number_of_Agents, &CDlg_VehPathAnalysis::OnCbnSelchangeComboMinNumberofAgents)
	ON_CBN_SELCHANGE(IDC_COMBO_Min_Travel_Time, &CDlg_VehPathAnalysis::OnCbnSelchangeComboMinTravelTime)
	ON_CBN_SELCHANGE(IDC_COMBO_Min_TravelTimeIndex, &CDlg_VehPathAnalysis::OnCbnSelchangeComboMinTraveltimeindex)
	ON_LBN_SELCHANGE(IDC_LIST_OD, &CDlg_VehPathAnalysis::OnLbnSelchangeListOd)
	ON_LBN_SELCHANGE(IDC_LIST_PATH, &CDlg_VehPathAnalysis::OnLbnSelchangeListPath)
	ON_CBN_SELCHANGE(IDC_COMBO_ImpactLink, &CDlg_VehPathAnalysis::OnCbnSelchangeComboImpactlink)
	ON_LBN_DBLCLK(IDC_LIST_OD, &CDlg_VehPathAnalysis::OnLbnDblclkListOd)
	ON_BN_CLICKED(ID_EXPORT, &CDlg_VehPathAnalysis::OnBnClickedExport)
	ON_CBN_SELCHANGE(IDC_COMBO_DemandType, &CDlg_VehPathAnalysis::OnCbnSelchangeComboDemandtype)
	ON_BN_CLICKED(ID_EXPORT_PATH_DATA, &CDlg_VehPathAnalysis::OnBnClickedExportPathData)
	ON_BN_CLICKED(ID_EXPORT_Agent_DATA, &CDlg_VehPathAnalysis::OnBnClickedExportAgentData)
	ON_BN_CLICKED(ID_FindCriticalOD, &CDlg_VehPathAnalysis::OnBnClickedFindcriticalod)
	ON_BN_CLICKED(IDOK, &CDlg_VehPathAnalysis::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CDlg_VehPathAnalysis::OnLvnItemchangedList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PATHLIST, &CDlg_VehPathAnalysis::OnPathLvnItemchangedList)
	ON_CBN_SELCHANGE(IDC_COMBO_DayNo, &CDlg_VehPathAnalysis::OnCbnSelchangeComboDayno)
	ON_BN_CLICKED(ID_BarChart, &CDlg_VehPathAnalysis::OnBnClickedBarchart)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlg_VehPathAnalysis message handlers


BOOL CDlg_VehPathAnalysis::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;

	m_ImpactLinkBox.AddString ("N/A");
	std::list<DTALink*>::iterator iLink;
	if(m_pDoc->m_SelectedLinkNo >=0)
	{
		DTALink* pLink= m_pDoc->m_LinkNoMap [m_pDoc->m_SelectedLinkNo];

			str.Format ("%d->%d, Selected Link", pLink ->m_FromNodeNumber,pLink ->m_ToNodeNumber );
			m_ImpactLinkBox.AddString (str);

	}

	m_ImpactLinkBox.SetCurSel (0);

	m_OriginBox.AddString("All");
	m_DestinationBox.AddString("All");

	int i;
	std::map<int, int>	::const_iterator itr_o;


	for(itr_o = m_pDoc->m_ZoneIDToNodeNoMap.begin(); itr_o != m_pDoc->m_ZoneIDToNodeNoMap.end(); itr_o++)
	{
		str.Format ("%d", itr_o->first);
		m_TAZVector.push_back (itr_o->first);
		m_OriginBox.AddString(str);
		m_DestinationBox.AddString(str);
	}
	m_OriginBox.SetCurSel (0);
	m_DestinationBox.SetCurSel (0);



	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	int project_index = 0 ;
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_ProjectTitle.GetLength () >0)  
		{

		project_index++;
		}
		iDoc++;
	}

	m_ProjectSize = max(1,project_index);





	m_DemandTypeBox.AddString("All");
	for(i = 0; i< m_pDoc->m_DemandTypeVector .size(); i++)
	{
	m_DemandTypeBox.AddString (m_pDoc->m_DemandTypeVector[i].demand_type_name);
	}
	m_DemandTypeBox.SetCurSel(0);


	for(i=0; i <= 1440 ; i+=15)
	{
		str.Format ("%d (%s)", i,m_pDoc->GetTimeStampString24HourFormat (i));
		m_DepartureTimeBox.AddString(str);
	}
	m_DepartureTimeBox.SetCurSel (0);

	for(i=0; i<100; i++)
	{
		str.Format ("%d", i);
		m_DayNo_Combobox.AddString(str);

	}

	m_DayNo_Combobox.SetCurSel (0);

	m_TimeIntervalBox.AddString("1440");
	m_TimeIntervalBox.AddString("15");
	m_TimeIntervalBox.AddString("30");
	m_TimeIntervalBox.AddString("60");
	m_TimeIntervalBox.AddString("120");
	m_TimeIntervalBox.AddString("240");
	m_TimeIntervalBox.AddString("480");
	m_TimeIntervalBox.SetCurSel (0);

	m_MinAgentSizeBox.AddString ("0");
	m_MinAgentSizeBox.AddString ("2");
	m_MinAgentSizeBox.AddString ("5");
	m_MinAgentSizeBox.AddString ("10");
	m_MinAgentSizeBox.AddString ("20");
	m_MinAgentSizeBox.AddString ("50");
	m_MinAgentSizeBox.AddString ("75");
	m_MinAgentSizeBox.AddString ("100");
	m_MinAgentSizeBox.AddString ("200");
	m_MinAgentSizeBox.AddString ("500");

	if(m_pDoc->m_ZoneNoSize <100)
		m_MinAgentSizeBox.SetCurSel (1);  //2
	else if (m_pDoc->m_ZoneNoSize <200)
		m_MinAgentSizeBox.SetCurSel (2);  // 5
	else
		m_MinAgentSizeBox.SetCurSel (4); // 20


	m_MinDistanceBox.AddString ("0");
	m_MinDistanceBox.AddString ("1");
	m_MinDistanceBox.AddString ("2");
	m_MinDistanceBox.AddString ("3");
	m_MinDistanceBox.AddString ("4");
	m_MinDistanceBox.AddString ("5");
	m_MinDistanceBox.AddString ("10");
	m_MinDistanceBox.AddString ("15");
	m_MinDistanceBox.AddString ("20");
	m_MinDistanceBox.AddString ("30");
	m_MinDistanceBox.AddString ("50");
	m_MinDistanceBox.AddString ("100");
	m_MinDistanceBox.SetCurSel (0);

    m_MaxSpeedBox.AddString("300"); 
	m_MaxSpeedBox.AddString("200"); 
	m_MaxSpeedBox.AddString("150"); 
    m_MaxSpeedBox.AddString("100"); 
	m_MaxSpeedBox.AddString("80"); 
	m_MaxSpeedBox.AddString("60"); 
	m_MaxSpeedBox.AddString("50"); 
	m_MaxSpeedBox.AddString("40"); 
	m_MaxSpeedBox.AddString("30"); 
	m_MaxSpeedBox.AddString("20"); 
	m_MaxSpeedBox.AddString("10"); 
	m_MaxSpeedBox.AddString("5"); 
	m_MaxSpeedBox.SetCurSel (0);


	// initialize List Control
	m_ListCtrl.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;  // Hao: this ponter should be delete. 
	m_ListCtrl.SetDefaultRowTrait(pRowTrait);

	// initialize Path List Control
	m_PathListCtrl.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTraitPath = new CGridRowTraitXP;  // Hao: this ponter should be delete. 
	m_PathListCtrl.SetDefaultRowTrait(pRowTraitPath);

	std::vector<std::string> m_Column_names;

	std::vector<CString> ColumnLabelVector;

	ColumnLabelVector.push_back ("Origin Zone");
	ColumnLabelVector.push_back ("Destination Zone");

	m_GPS_start_day = 3;
	m_GPS_end_day = 17;


	project_index = 0 ;

	iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_ProjectTitle.GetLength () >0)  
		{

			CString str;
			str.Format ("%s Count:", (*iDoc)->m_ProjectTitle);
			ColumnLabelVector.push_back (str);
			ColumnLabelVector.push_back ("Avg Travel Time");
			ColumnLabelVector.push_back ("Avg Distance");
			ColumnLabelVector.push_back ("Avg Speed");


			if(project_index>=1)
			{
				ColumnLabelVector.push_back ("Diff of Distance");
				ColumnLabelVector.push_back ("Diff of Travel Time");
			}

			project_index++;
		}
		iDoc++;
	}


	//Add Columns and set headers
	for (unsigned i=0;i< ColumnLabelVector.size();i++)
	{
		CGridColumnTraitText* pTrait = NULL;

//		pTrait = new CGridColumnTraitEdit();
		m_ListCtrl.InsertColumnTrait((int)i,ColumnLabelVector[i],LVCFMT_LEFT,-1,-1, pTrait);
//		pTrait->SetSortFormatNumber(true);	
//		m_ListCtrl.SetColumnWidth((int)i,LVSCW_AUTOSIZE_USEHEADER);
		m_ListCtrl.SetColumnWidth((int)i,80);
	}

	if(m_ODMOEMatrix == NULL  && m_pDoc->m_ZoneNoSize >0 )
	{
		m_ODMOEMatrix = Allocate3DDynamicArray<AgentStatistics>(m_ProjectSize,m_pDoc->m_ZoneNoSize ,m_pDoc->m_ZoneNoSize );
			m_ZoneNoSize  = m_pDoc->m_ZoneNoSize ;
			m_OldProjectSize = m_ProjectSize;

	}
	else
	{
		if(  m_ZoneNoSize >0  && (m_ZoneNoSize !=  m_pDoc->m_ZoneNoSize  || m_OldProjectSize != m_ProjectSize))
		{
			Deallocate3DDynamicArray<AgentStatistics>(m_ODMOEMatrix,m_OldProjectSize, m_ZoneNoSize );
			m_ODMOEMatrix = Allocate3DDynamicArray<AgentStatistics>(m_ProjectSize,m_pDoc->m_ZoneNoSize ,m_pDoc->m_ZoneNoSize );
			m_OldProjectSize = m_ProjectSize;
			m_ZoneNoSize  = m_pDoc->m_ZoneNoSize ;
	
		}
	}
	
	std::vector<CString> ColumnPathLabelVector;

	ColumnPathLabelVector.push_back ("Path No");
	ColumnPathLabelVector.push_back ("Count");
	ColumnPathLabelVector.push_back ("Percentage");
	ColumnPathLabelVector.push_back ("Travel Time");
	ColumnPathLabelVector.push_back ("Distance");
	ColumnPathLabelVector.push_back ("Speed");


	
	for (unsigned i=0;i< ColumnPathLabelVector.size();i++)
	{
		CGridColumnTraitText* pTrait = NULL;

		m_PathListCtrl.InsertColumnTrait((int)i,ColumnPathLabelVector[i],LVCFMT_LEFT,-1,-1, pTrait);
		m_PathListCtrl.SetColumnWidth((int)i,80);
	}
	FilterOriginDestinationPairs();



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_VehPathAnalysis::FilterOriginDestinationPairs()
{
	m_pDoc->ResetODMOEMatrix();


	m_ListCtrl.DeleteAllItems();

	if(m_ZoneNoSize=0)
		return;

	UpdateData(1);

	CString SummaryInfoString;

	CWaitCursor wait;
	int i,j;
	int p;

	if(m_ODMOEMatrix ==NULL)
		return;

	for(p=0;p<m_ProjectSize; p++)
	{
	for(i=0; i < m_pDoc->m_ZoneNoSize  ; i++)
		for(j=0; j< m_pDoc->m_ZoneNoSize  ; j++)
		{
			m_ODMOEMatrix[p][i][j].Reset ();
		}
	}

	int ImpactLinkNo = -1;
	if(m_ImpactLinkBox.GetCurSel()>0)
	{
		char m_Text[MAX_STRING_LENGTH];
		int FromNodeNumber, ToNodeNumber;
		m_ImpactLinkBox.GetLBText (m_ImpactLinkBox.GetCurSel(), m_Text);
		sscanf(m_Text, "%d->%d", &FromNodeNumber, &ToNodeNumber);
		DTALink* pLink = m_pDoc->FindLinkWithNodeNumbers(FromNodeNumber, ToNodeNumber);
		if(pLink!=NULL)
			ImpactLinkNo = pLink->m_LinkNo;
	}


		int Origin_Row = m_OriginBox.GetCurSel();

		int Destination_Row = m_DestinationBox.GetCurSel();

		int Origin = 0;
		int Destination = 0;
		if(Origin_Row > 0)
		{
		Origin = m_TAZVector[Origin_Row-1];
		}

		if(Destination_Row > 0)
		{
		Destination = m_TAZVector[Destination_Row-1];
		}

		//Origin = 1737;
		//Destination = 1767;

		int DemandType = m_DemandTypeBox.GetCurSel();

		char str[MAX_STRING_LENGTH];
		m_DepartureTimeBox.GetLBText(m_DepartureTimeBox.GetCurSel(), str);
		int DepartureTime = atoi(str);

		m_TimeIntervalBox.GetLBText(m_TimeIntervalBox.GetCurSel(), str);
		int TimeInterval = atoi(str);

		m_MinAgentSizeBox.GetLBText(m_MinAgentSizeBox.GetCurSel(), str);
		int MinAgentSize = atoi(str);

		m_MinDistanceBox.GetLBText(m_MinDistanceBox.GetCurSel(), str);
		int MinDistance = atoi(str);

		m_MaxSpeedBox.GetLBText(m_MaxSpeedBox.GetCurSel(), str);
		float MaxSpeed = atof(str);

		int DayNo = m_DayNo_Combobox.GetCurSel();

		int count = 0;
		std::list<DTAAgent*>::iterator iAgent;

	std::list<CTLiteDoc*>::iterator  iDoc = g_DocumentList.begin ();

	 p = 0 ;
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_ProjectTitle.GetLength () >0)  
		{

			if(p >=m_ProjectSize)  // additional project loaded. 
				continue; 

		for (iAgent = (*iDoc)->m_AgentSet.begin(); iAgent != (*iDoc)->m_AgentSet.end(); iAgent++, count++)
		{
			DTAAgent* pAgent = (*iAgent);


			int OrgNo = (*iDoc)->m_ZoneIDtoZoneNoMap[pAgent->m_FromZoneID];
			int DesNo = (*iDoc)->m_ZoneIDtoZoneNoMap[pAgent->m_ToZoneID];


			if(OrgNo>=0 && DesNo >=0  && pAgent->m_bComplete )  // with physical path in the network
			{
				if( 
					(pAgent->m_DayNo == DayNo || DayNo == 0 ) &&
					(pAgent->m_FromZoneID == Origin ||Origin ==0)&&
					(pAgent->m_ToZoneID  == Destination ||Destination ==0)&&
					(pAgent->m_DemandType  == DemandType ||DemandType ==0)&&
					(pAgent->m_DepartureTime >= DepartureTime && pAgent->m_DepartureTime <= DepartureTime+TimeInterval))
				{

					m_ODMOEMatrix[p][OrgNo][DesNo].TotalAgentSize+=1;

					if( (*iDoc)->m_bGPSDataSet )
					{
					m_ODMOEMatrix[p][OrgNo][DesNo].day_count[pAgent->m_DayNo ]+=1;
					}

					m_ODMOEMatrix[p][OrgNo][DesNo].TotalTravelTime += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
					m_ODMOEMatrix[p][OrgNo][DesNo].TotalDistance += pAgent->m_Distance;
					

					if(ImpactLinkNo>=0)
					{
					for(int link= 1; link<pAgent->m_NodeSize; link++)
					{
						if ( pAgent->m_NodeAry[link].LinkNo  == ImpactLinkNo)
						{
							m_ODMOEMatrix[p][OrgNo][DesNo].bImpactFlag = true;
						}

					}
					}
				}
			}

		}

		count = 0;
//		m_ODList.ResetContent ();

		// variability measure
		for (iAgent = (*iDoc)->m_AgentSet.begin(); iAgent != (*iDoc)->m_AgentSet.end(); iAgent++, count++)
		{
			DTAAgent* pAgent = (*iAgent);
			int OrgNo = (*iDoc)->m_ZoneIDtoZoneNoMap[pAgent->m_FromZoneID];
			int DesNo = (*iDoc)->m_ZoneIDtoZoneNoMap[pAgent->m_ToZoneID];


			if(OrgNo>=0 && DesNo >=0 /*pAgent->m_NodeSize >= 2 && */ && pAgent->m_bComplete )  // with physical path in the network
			{
				if( 
					(pAgent->m_DayNo == DayNo || DayNo == 0 ) &&
					(pAgent->m_FromZoneID == Origin ||Origin ==0)&&
					(pAgent->m_ToZoneID  == Destination ||Destination ==0)&&
					(pAgent->m_DemandType  == DemandType ||DemandType ==0)&&
					(pAgent->m_DepartureTime >= DepartureTime && pAgent->m_DepartureTime <= DepartureTime+TimeInterval))
				{
					float AvgTravelTime = m_ODMOEMatrix[p][OrgNo][DesNo].TotalTravelTime/max(1,m_ODMOEMatrix[p][OrgNo][DesNo].TotalAgentSize );

					m_ODMOEMatrix[p][OrgNo][DesNo].AvgDistance = m_ODMOEMatrix[p][OrgNo][DesNo].TotalDistance /max(1,m_ODMOEMatrix[p][OrgNo][DesNo].TotalAgentSize );;
					m_ODMOEMatrix[p][OrgNo][DesNo].AvgTravelTime = AvgTravelTime;
					m_ODMOEMatrix[p][OrgNo][DesNo].TotalTravelTimeVariance  += (pAgent->m_TripTime - AvgTravelTime)*(pAgent->m_TripTime- AvgTravelTime);
					m_ODMOEMatrix[p][OrgNo][DesNo].TotalTravelTimePerMileVariance  += (pAgent->m_TripTime - AvgTravelTime)*(pAgent->m_TripTime- AvgTravelTime)/max(0.01,pAgent->m_Distance * pAgent->m_Distance);
				}
			}
		}

		p++;
		}
		iDoc++;
	}


		AgentStatistics total_summary;
		count = 0;


		m_SelectedOrigin = -1;
		m_SelectedDestination = -1;

		int row_index = 0;
		for(i=0; i < m_pDoc->m_ZoneNoSize  ; i++)
			for(j=0; j< m_pDoc->m_ZoneNoSize  ; j++)
			{

		int Index = 0;

		std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
		 p = 0 ;
		while (iDoc != g_DocumentList.end())
		{
			if((*iDoc)->m_ProjectTitle.GetLength () >0)  
			{

				
				if(  m_ODMOEMatrix[0][i][j].TotalAgentSize>0 && 
					(ImpactLinkNo<0 || (ImpactLinkNo >=0 && m_ODMOEMatrix[0][i][j].bImpactFlag == true)))
				{
					float AvgDistance = m_ODMOEMatrix[p][i][j].TotalDistance /m_ODMOEMatrix[p][i][j].TotalAgentSize;
					float AvgTravelTime = m_ODMOEMatrix[p][i][j].TotalTravelTime /m_ODMOEMatrix[p][i][j].TotalAgentSize;
					float AvgCost = m_ODMOEMatrix[p][i][j].TotalCost  /m_ODMOEMatrix[p][i][j].TotalAgentSize;




					float AvgSpeed = AvgDistance * 60 / max(0.1,AvgTravelTime);  // mph
					if(m_ODMOEMatrix[p][i][j].TotalAgentSize >= MinAgentSize && 
						AvgDistance >= MinDistance && AvgSpeed <=MaxSpeed)
					{

						CString ODInfoString;


		int column_index  = 0;
		char text[100];

		if(p==0)
		{
		sprintf_s(text, "%d",m_pDoc->m_ZoneNotoZoneIDMap[i]);
		Index = m_ListCtrl.InsertItem(LVIF_TEXT,row_index++,text , 0, 0, 0, NULL);
		column_index++;

		sprintf_s(text, "%d",m_pDoc->m_ZoneNotoZoneIDMap[j]);
		m_ListCtrl.SetItemText(Index,column_index++,text );

		}


		if(p==0)
			column_index = 2;
		else
			column_index = 8 + (p-1)*8 ;  // -1 no diff for base line

		sprintf_s(text, "%d",m_ODMOEMatrix[p][i][j].TotalAgentSize);
		m_ListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%3.1f",AvgTravelTime);
		m_ListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%3.1f",AvgDistance);
		m_ListCtrl.SetItemText(Index,column_index++,text );


		sprintf_s(text, "%3.1f",AvgSpeed);
		m_ListCtrl.SetItemText(Index,column_index++,text );

		
		if((*iDoc)-> m_bGPSDataSet)
			{

				int min = 10000;
				int max = 0;
				int total = 0;
				for(int day = m_GPS_start_day; day<=m_GPS_end_day; day++)
				{

					if(m_ODMOEMatrix[p][i][j].day_count [day] > max)
						max = m_ODMOEMatrix[p][i][j].day_count [day];

					if(m_ODMOEMatrix[p][i][j].day_count [day] < min)
						min = m_ODMOEMatrix[p][i][j].day_count [day];

					total +=  m_ODMOEMatrix[p][i][j].day_count [day];
				
				sprintf_s(text, "%d",m_ODMOEMatrix[p][i][j].day_count [day]);
				m_ListCtrl.SetItemText(Index,column_index++,text );

				}

				sprintf_s(text, "%.3f",(max-min)*1.0f/max(1,total)*(m_GPS_end_day+1-m_GPS_start_day));
				m_ListCtrl.SetItemText(Index,column_index++,text );
			
			}

		if(p>=1)
		{
		sprintf_s(text, "%3.1f",m_ODMOEMatrix[p][i][j].AvgDistance - m_ODMOEMatrix[0][i][j].AvgDistance);
		m_ListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%3.1f",m_ODMOEMatrix[p][i][j].AvgTravelTime - m_ODMOEMatrix[0][i][j].AvgTravelTime);
		m_ListCtrl.SetItemText(Index,column_index++,text );
		}


		count ++;

		if(count>=50000)
			break;
//
//						if(ImpactLinkNo<0)  // no impact link is selected
//						{
//						if(count<10000)
//						{
//							total_summary.TotalAgentSize +=m_ODMOEMatrix[p][i][j].TotalAgentSize;
//							total_summary.TotalTravelTime +=m_ODMOEMatrix[p][i][j].TotalTravelTime;
//							total_summary.TotalDistance +=m_ODMOEMatrix[p][i][j].TotalDistance;
//							total_summary.TotalCost +=m_ODMOEMatrix[p][i][j].TotalCost;
//							total_summary.emissiondata .Energy += m_ODMOEMatrix[p][i][j].emissiondata .Energy ;
//							total_summary.emissiondata .CO2  += m_ODMOEMatrix[p][i][j].emissiondata .CO2  ;
//
////							m_ODList.AddString (ODInfoString);
//						}
//
//						}else
//						{ // ImpactLinkNo>0: impact link is selected
//						
//						if(count<10000 && m_ODMOEMatrix[p][i][j].bImpactFlag == true)
//						{
//							total_summary.TotalAgentSize +=m_ODMOEMatrix[p][i][j].TotalAgentSize;
//							total_summary.TotalTravelTime +=m_ODMOEMatrix[p][i][j].TotalTravelTime;
//							total_summary.TotalDistance +=m_ODMOEMatrix[p][i][j].TotalDistance;
//							total_summary.TotalCost  +=m_ODMOEMatrix[p][i][j].TotalCost;
//							total_summary.emissiondata .Energy += m_ODMOEMatrix[p][i][j].emissiondata .Energy ;
//							total_summary.emissiondata .CO2  += m_ODMOEMatrix[p][i][j].emissiondata .CO2  ;
//
////							m_ODList.AddString (ODInfoString);
//						}

					
						}
					} // OD demand > 0 
			
		p++;
		}
			iDoc++;
		} //document
		}

		if(count < 50000)
		SummaryInfoString.Format("%d OD pair(s) selected.",count);
		else
		SummaryInfoString.Format("Up to %d OD pairs listed.",count);


		m_Summary_Info_Edit.SetWindowText (SummaryInfoString);

			FilterPaths();
			ShowSelectedPath();
			ShowAgents();
}
void CDlg_VehPathAnalysis::FilterPaths()
{
	m_PathVector.clear();
	m_AgentList.ResetContent ();
	m_PathListCtrl.DeleteAllItems();  // OD changed, refresh

	UpdateData(1);

	int Origin = m_SelectedOrigin;
	int Destination = m_SelectedDestination;

	int DemandType = m_DemandTypeBox.GetCurSel();
	
	char str[50];
	m_DepartureTimeBox.GetLBText(m_DepartureTimeBox.GetCurSel(), str);
	int DepartureTime = atoi(str);

	m_TimeIntervalBox.GetLBText(m_TimeIntervalBox.GetCurSel(), str);
	int TimeInterval = atoi(str);

	m_MinAgentSizeBox.GetLBText(m_MinAgentSizeBox.GetCurSel(), str);
	int MinAgentSize = atoi(str);

	m_MinDistanceBox.GetLBText(m_MinDistanceBox.GetCurSel(), str);
	int MinDistance = atoi(str);


		int DayNo = m_DayNo_Combobox.GetCurSel();

	int count = 0;
	int Agent_count = 0;
	std::list<DTAAgent*>::iterator iAgent;

	for (iAgent = m_pDoc->m_AgentSet.begin(); iAgent != m_pDoc->m_AgentSet.end(); iAgent++, count++)
	{
		DTAAgent* pAgent = (*iAgent);
			pAgent->m_bODMarked = false;

		if(pAgent->m_NodeSize >= 2 && pAgent->m_bComplete )  // with physical path in the network
		{
			if( 
				(pAgent->m_DayNo == DayNo || DayNo == 0 ) &&
				(pAgent->m_FromZoneID == Origin)&&
				(pAgent->m_ToZoneID  == Destination)&&
				(pAgent->m_DemandType  == DemandType ||DemandType ==0)&&
				(pAgent->m_DepartureTime >= DepartureTime && pAgent->m_DepartureTime <= DepartureTime+TimeInterval))
			{

				bool bFingFlag =  false;



			pAgent->m_bODMarked = true;

				for(int p = 0; p< m_PathVector.size(); p++)
				{
					

					//existing path
					if(pAgent->m_NodeNumberSum == m_PathVector[p].NodeNumberSum  && pAgent->m_NodeSize == m_PathVector[p].NodeSize )
					{
						
						m_PathVector[p].date_id = pAgent->m_DayNo ;
						m_PathVector[p].departure_time_in_min = pAgent->m_DepartureTime ;
						m_PathVector[p].TotalAgentSize+=1;
						m_PathVector[p].TotalTravelTime  += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
						m_PathVector[p].TotalDistance   += pAgent->m_Distance;

						m_PathVector[p].m_TravelTimeVector.push_back(pAgent->m_ArrivalTime-pAgent->m_DepartureTime);

						Agent_count ++;

						m_PathVector[p].m_AgentVector.push_back(pAgent);
						bFingFlag = true;
						break;
					}
				}
				if(bFingFlag == false)
				{
					// new path
					PathStatistics ps_element;
					ps_element.NodeNumberSum = pAgent->m_NodeNumberSum;
					ps_element.NodeSize = pAgent->m_NodeSize;
					ps_element.date_id = pAgent->m_DayNo ;
					ps_element.departure_time_in_min = pAgent->m_DepartureTime ;
					ps_element.TotalAgentSize = 1;
					ps_element.TotalTravelTime  += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
					ps_element.TotalDistance   += pAgent->m_Distance;
					ps_element.m_TravelTimeVector.push_back((pAgent->m_ArrivalTime-pAgent->m_DepartureTime));
					ps_element.m_TravelTimeVector.push_back((pAgent->m_ArrivalTime-pAgent->m_DepartureTime)/max(0.01,pAgent->m_Distance));

						Agent_count ++;


					ps_element.m_AgentVector.push_back(pAgent);


					if(true)
					{
						for(int link= 1; link<pAgent->m_NodeSize; link++)
						{
							ps_element.m_LinkVector.push_back(pAgent->m_NodeAry[link].LinkNo);
						}
					}

					m_PathVector.push_back (ps_element);
				}
			}
		}
	}

	int row_index = 0;
	for(int p = 0; p< m_PathVector.size(); p++)
	{
		float AvgDistance = m_PathVector[p].TotalDistance /m_PathVector[p].TotalAgentSize;
		float AvgTravelTime = m_PathVector[p].TotalTravelTime /m_PathVector[p].TotalAgentSize;
		float AvgTravelCost = m_PathVector[p].TotalCost /m_PathVector[p].TotalAgentSize;

		float TravelTimeSTD = m_PathVector[p].GetSimulatedTravelTimeStandardDeviation();
		float TravelTimePerMileSTD = m_PathVector[p].GetSimulatedTravelTimePerMileStandardDeviation();


		float AvgSpeed = AvgDistance * 60 / max(0.1,AvgTravelTime);  // mph

		int column_index  = 0;
		char text[100];
		int Index;


		sprintf_s(text, "%d",p+1);
		Index = m_PathListCtrl.InsertItem(LVIF_TEXT,row_index++,text , 0, 0, 0, NULL);
		column_index++;

		sprintf_s(text, "%d", m_PathVector[p].TotalAgentSize);
		m_PathListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%2.1f", m_PathVector[p].TotalAgentSize*100.0/Agent_count);
		m_PathListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%3.1f", AvgTravelTime);
		m_PathListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%3.1f", AvgDistance);
		m_PathListCtrl.SetItemText(Index,column_index++,text );

		sprintf_s(text, "%3.1f", AvgSpeed);
		m_PathListCtrl.SetItemText(Index,column_index++,text );

	}

	
//	m_PathList.SetCurSel(0);


	ShowSelectedPath();
	ShowAgents();

}
void CDlg_VehPathAnalysis::ShowAgents()
{
	m_pDoc->m_SelectedAgentID = -1;

	m_AgentList.ResetContent ();
	int PathNo = m_SelectedPath;
	if(PathNo>=0 && PathNo < m_PathVector.size())
	{
		for(unsigned int v = 0; v< m_PathVector[PathNo].m_AgentVector.size(); v++)
		{
			DTAAgent* pAgent = m_PathVector[PathNo].m_AgentVector[v];
			CString AgentInfoString;

			if( v == 0)  // only for the first  Agent
			{

			m_pDoc->m_SelectedAgentID = pAgent->m_AgentID ;



			}

			AgentInfoString.Format ("No. %d, %d, @%3.1f min, %3.1f min",pAgent->m_AgentID , pAgent->m_DemandType, pAgent->m_DepartureTime, pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
	
			if(m_AgentList.GetCount () <= 20000)
			{
				if(m_AgentList.GetCount () == 20000)
				{
					m_AgentList.AddString ("Reaching maximum number of items in a list..");
				}else
				m_AgentList.AddString (AgentInfoString);
			}
		}
	}
}
void CDlg_VehPathAnalysis::OnLbnSelchangeListLink()
{
	// TODO: Add your control notification handler code here
}


void CDlg_VehPathAnalysis::OnLbnSelchangeListAgent()
{
}

void CDlg_VehPathAnalysis::OnLbnSelchangeListLink2()
{
	// TODO: Add your control notification handler code here
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboOrigin()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboDestination()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboDeparturetime()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboAgenttype()
{
	FilterOriginDestinationPairs();
}


void CDlg_VehPathAnalysis::OnCbnSelchangeComboTimeinterval()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboMinNumberofAgents()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboMinTravelTime()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnCbnSelchangeComboMinTraveltimeindex()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnLbnSelchangeListOd()
{
	FilterPaths();
	//show OD pair

		int ODPairNo = m_ODList.GetCurSel();
	int Origin, Destination;

	if(ODPairNo>=0)	// if one of "all" options is selected, we need to narrow down to OD pair
	{
		char m_Text[MAX_STRING_LENGTH];
		m_ODList.GetText (ODPairNo, m_Text);
		sscanf(m_Text, "%d->%d", &Origin, &Destination);
		
		m_pDoc->m_CriticalOriginZone = Origin;
		m_pDoc->m_CriticalDestinationZone  = Destination;

		m_pDoc->UpdateAllViews (0);


	}


}

void CDlg_VehPathAnalysis::OnLbnSelchangeListPath()
{
	ShowSelectedPath();
	ShowAgents();

}

void CDlg_VehPathAnalysis::ShowSelectedPath()
{
	
	if(m_PathVector.size()==0)
		m_SelectedPath = -1;

	if(m_SelectedPath >= 0 && m_SelectedPath< m_PathVector.size())
		m_pDoc->HighlightPath(m_PathVector[m_SelectedPath].m_LinkVector,2);

}


void CDlg_VehPathAnalysis::OnCbnSelchangeComboImpactlink()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnLbnDblclkListOd()
{
	FilterPaths();
}


void CDlg_VehPathAnalysis::OnBnClickedExport()
{
	if(m_pDoc->m_ProjectDirectory .GetLength () > 0)
	{
	CString fname, str;
	
	fname = m_pDoc->m_ProjectDirectory + "export_OD_moe.csv"; 

      CWaitCursor wait;

      if(!ExportDataToCSVFileAllOD(fname))
      {
		 str.Format("The file %s could not be opened.\nPlease check if it is opened by Excel.", fname);
		 AfxMessageBox(str);
      }else
	  {
		m_pDoc->OpenCSVFileInExcel(fname);
	  }
	}
}
bool CDlg_VehPathAnalysis::ExportDataToCSVFileAllOD(CString csv_file)
{
     FILE* st;
     fopen_s(&st,csv_file,"w");
	 CWaitCursor wc;

     if(st!=NULL)
      {
			  for(int column = 0; column < m_ListCtrl.GetColumnCount (); column++)
			  {
			  fprintf(st,"%s,", m_ListCtrl.GetColumnHeading (column));
			  }
				fprintf(st,"\n");

		  for(int row = 0; row < m_ListCtrl.GetItemCount (); row++)
		  {

			  for(int column = 0; column < m_ListCtrl.GetColumnCount (); column++)
			  {
				char str[100];
				m_ListCtrl.GetItemText (row,column,str,100);

				fprintf(st,"%s,",str);
			  }
				fprintf(st,"\n");

		  }

		fclose(st);
		return true;
	 }

	 return false;
}



void CDlg_VehPathAnalysis::OnCbnSelchangeComboDemandtype()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnBnClickedFindSingleAgentId()
{
	// TODO: Add your control notification handler code here
}

void CDlg_VehPathAnalysis::OnBnClickedExportPathData()
{
	if(m_pDoc->m_ProjectDirectory .GetLength () > 0)
	{
	CString fname;
	
	fname = m_pDoc->m_ProjectDirectory + "export_path_moe.csv"; 
      CWaitCursor wait;

      if(!ExportPathDataToCSVFile(fname))
      {  CString str;
		 str.Format("The file %s could not be opened.\nPlease check if it is opened by Excel.", fname);
		 AfxMessageBox(str);
      }else
	  {
	  m_pDoc->OpenCSVFileInExcel(fname);
	  }
   }	
}


bool CDlg_VehPathAnalysis::ExportPathDataToCSVFile(CString csv_file)
{
    FILE* st;
      fopen_s(&st,csv_file,"w");

     if(st!=NULL)
      {
			  for(int column = 0; column < m_PathListCtrl.GetColumnCount (); column++)
			  {
			  fprintf(st,"%s,", m_PathListCtrl.GetColumnHeading (column));
			  }
				fprintf(st,"\n");

		  for(int row = 0; row < m_PathListCtrl.GetItemCount (); row++)
		  {

			  for(int column = 0; column < m_PathListCtrl.GetColumnCount (); column++)
			  {
				char str[100];
				m_PathListCtrl.GetItemText (row,column,str,100);

				fprintf(st,"%s,",str);
			  }
				fprintf(st,"\n");

		  }

		fclose(st);
		return true;
	 }


	 return false;
}

bool CDlg_VehPathAnalysis::ExportAgentDataToCSVFile(CString csv_file)
{
	int PathNo = m_SelectedPath;
	if(PathNo>=0)
	{
		return m_pDoc->WriteSelectAgentDataToCSVFile(csv_file,m_PathVector[PathNo].m_AgentVector);
	}
	return false;
}

void CDlg_VehPathAnalysis::OnBnClickedExportAgentData()
{
	if(m_pDoc->m_ProjectDirectory .GetLength () > 0)
	{
	CString fname;
	
		fname = m_pDoc->m_ProjectDirectory + "export_Agent_list.csv"; 
      CWaitCursor wait;

      if(!ExportAgentDataToCSVFile(fname))
      { CString str;
		 str.Format("The data might not be available. \n The file %s might not be opened.\nPlease check if it is opened by Excel.", fname);
		 AfxMessageBox(str);
      }else
	  {
	  m_pDoc->OpenCSVFileInExcel(fname);
	  }
   }	
}

void CDlg_VehPathAnalysis::OnBnClickedFindcriticalod()
{
	m_MinAgentSizeBox.SetCurSel(9);
	m_MinDistanceBox.SetCurSel(2);
	FilterOriginDestinationPairs();

}

void CDlg_VehPathAnalysis::OnBnClickedOk()
{
	OnOK();
	g_bShowAgentPathDialog = false;
}

void CDlg_VehPathAnalysis::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int nSelectedRow = m_ListCtrl.GetNextSelectedItem(pos);
		char str[100];
		m_ListCtrl.GetItemText (nSelectedRow,0,str,20);

		m_SelectedOrigin = atoi(str);
		m_ListCtrl.GetItemText (nSelectedRow,1,str,20);
		m_SelectedDestination = atoi(str);

			FilterPaths();
			ShowSelectedPath();
			ShowAgents();
	}
}

void CDlg_VehPathAnalysis::OnPathLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	POSITION pos = m_PathListCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int nSelectedRow = m_PathListCtrl.GetNextSelectedItem(pos);
		char str[100];
		m_PathListCtrl.GetItemText (nSelectedRow,0,str,20);

		m_SelectedPath = atoi(str)-1;  // start from 0

			ShowSelectedPath();
			ShowAgents();
	}
}



void CDlg_VehPathAnalysis::OnCbnSelchangeComboDayno()
{
	FilterOriginDestinationPairs();
}

void CDlg_VehPathAnalysis::OnLvnItemchangedList5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlg_VehPathAnalysis::OnBnClickedBarchart()
{

	CDlg_AgentClassification dlg;
	dlg.m_pDoc = m_pDoc;
	m_pDoc->m_AgentSelectionMode = CLS_OD;
	dlg.m_AgentSelectionNo  = CLS_OD;
	dlg.DoModal ();

}

void CDlg_VehPathAnalysis::OnClose()
{
	// TODO: Add your message handler code here and/or call default

 	g_bShowAgentPathDialog = false;
	int nRet = 5; 
   EndDialog(nRet); 

}
