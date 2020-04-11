// Traffic Time Estimation, part of TLiteDoc.cpp : implementation of the CTLiteDoc class
//

//   If you help write or modify the code, please also list your names here.
//   The reason of having Copyright info here is to ensure all the modified version, as a whole, under the GPL 
//   and further prevent a violation of the GPL.

// More about "How to use GNU licenses for your own software"
// http://www.gnu.org/licenses/gpl-howto.html


//    This file is part of NeXTA Version 3 (Open-source).

//    NEXTA is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    NEXTA is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with NEXTA.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "CSVParser.h"
#include "TLite.h"
#include "Network.h"
#include "TLiteDoc.h"
#include "TLiteView.h"
#include "DlgMOE.h"
#include "DlgFileLoading.h"
#include "MainFrm.h"
#include "Shellapi.h"
#include "DlgAssignmentSettings.h"
#include "DlgLinkList.h"

#include "Shellapi.h"
#include "DlgNetworkAlignment.h"
#include "Dlg_VehEmissions.h"
#include "DlgPathList.h"
extern CDlgPathList* g_pPathListDlg;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int CTLiteDoc::AlternativeRouting(int NumberOfRoutes = 2)
{

	return 0;
}


int CTLiteDoc::Routing(bool bCheckConnectivity, bool bRebuildNetwork)
{
	CWaitCursor cws;
	m_NodeSizeSP = 0;  // reset 

	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_OverlappingCost  = 0;  // randomize link cost to avoid overlapping

			if(m_LinkTypeMap[(*iLink)->m_link_type].IsConnector ())
				(*iLink)->m_bConnector = true;

			if(m_LinkTypeMap[(*iLink)->m_link_type].IsTransit  ())
				(*iLink)->m_bTransit  = true;
	
			if(m_LinkTypeMap[(*iLink)->m_link_type].IsConnector ())
				(*iLink)->m_bWalking  = true;

		// consider intermediate destination here
	}


	
	if(bCheckConnectivity == false)
	{
		if(m_FromNodeID < 0 || m_ToNodeID <0)
	{
		m_SelectPathNo = -1;
		return 0;
	}
	}



 // create network every time, because we might add nodes/links on the fly
	
	if(m_pNetwork ==NULL)  
		{
		m_pNetwork = new DTANetworkForSP(m_NodeSet.size(), m_LinkSet.size(), 1, 1, m_AdjLinkSize);  //  network instance for single processor in multi-thread environment
		m_pNetwork->BuildPhysicalNetwork(&m_NodeSet, &m_LinkSet, m_RandomRoutingCoefficient, false, m_FromNodeID, m_ToNodeID);
		}

	if(bRebuildNetwork)  // link cost changed
	{
		m_pNetwork->BuildPhysicalNetwork(&m_NodeSet, &m_LinkSet, m_RandomRoutingCoefficient, false,m_FromNodeID, m_ToNodeID);

	}
		int NodeNodeSum = 0;

		int PathLinkList[MAX_NODE_SIZE_IN_A_PATH];

			float TotalCost;
			bool distance_flag = true;

			
			int     NodeSize ;
			
			if(bCheckConnectivity==true)
			{
				m_pNetwork->SimplifiedTDLabelCorrecting_DoubleQueue(m_FromNodeID, 0, m_ToNodeID, 1, 10.0f,PathLinkList,TotalCost, distance_flag, true, false,0);   // Pointer to previous node (node)

					for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
					{
						(*iNode)->m_DistanceToRoot  = m_pNetwork->LabelCostAry[(*iNode)->m_NodeNo ];
					}
		
				return 0;
			}

			//the code below is not checking connectivity

			std::vector<int> ODNodeSequenceVector;

			ODNodeSequenceVector.push_back(m_FromNodeID);

			// add intermediate destinations 
			for(unsigned int idest = 0; idest < m_IntermediateDestinationVector.size(); idest++)
			{
			ODNodeSequenceVector.push_back(m_IntermediateDestinationVector[idest]);
			}

			ODNodeSequenceVector.push_back(m_ToNodeID);


				DTAPath path_element;
				path_element.Init (0,g_Simulation_Time_Horizon);

			for(unsigned int ODSequenceNo = 0; ODSequenceNo < ODNodeSequenceVector.size()-1; ODSequenceNo++)
			{
			NodeSize= m_pNetwork->SimplifiedTDLabelCorrecting_DoubleQueue(ODNodeSequenceVector[ODSequenceNo], 0, ODNodeSequenceVector[ODSequenceNo+1], 1, 10.0f,PathLinkList,TotalCost, distance_flag, false, false,m_RandomRoutingCoefficient);   // Pointer to previous node (node)
			
			// update m_PathDisplayList
			if(NodeSize <= 1)
			{
				TRACE("error");
				return 0;
			}


				for (int i=0 ; i < NodeSize-1; i++)
				{

						path_element.m_LinkVector.push_back(PathLinkList[i]) ; //starting from m_NodeSizeSP-2, to 0

						CString label;
						label.Format ("%d", 1);
						path_element.m_PathLabelVector.push_back(label);

						DTALink* pLink = m_LinkNotoLinkMap[PathLinkList[i]];

					
						if(pLink!=NULL)
						{ 
							path_element.m_Distance += m_LinkNotoLinkMap[PathLinkList[i]]->m_Length ;
							path_element.m_NumberOfSensorsPassed += pLink->m_bSensorData;

						if(path_element.m_LinkVector.size()==1) // first link
						{
							path_element.m_TravelTime = pLink->GetDynamicTravelTime(g_Simulation_Time_Stamp,m_PrimaryDataSource );
						}else
						{
							path_element.m_TravelTime = path_element.m_TravelTime + pLink->GetDynamicTravelTime(path_element.m_TravelTime,m_PrimaryDataSource );

						}
						}
				}
			}  // for each origin sequence


			m_SelectPathNo = min( m_SelectPathNo, m_PathDisplayList.size()-1);
			if(m_PathDisplayList.size()> 0) 
			{
				string old_path_name  = m_PathDisplayList[m_SelectPathNo] .m_path_name ;
				m_PathDisplayList[m_SelectPathNo]  = path_element;
				m_PathDisplayList[m_SelectPathNo].m_path_name = old_path_name;
			
			}else{
					m_PathDisplayList.push_back (path_element);
					m_SelectPathNo = 0;
				}
			
				UpdateAllViews(0);

				if( g_pPathListDlg!= NULL && g_pPathListDlg->GetSafeHwnd())
					 g_pPathListDlg ->ReloadData();


	// calculate time-dependent travel time


		for(unsigned int p = 0; p < m_PathDisplayList.size(); p++) // for each path
		{
			DTAPath path_element = m_PathDisplayList[p];

			for(int t=0; t< g_Simulation_Time_Horizon; t+= TIME_DEPENDENT_TRAVLE_TIME_CALCULATION_INTERVAL)  // for each starting time
			{
				path_element.m_TimeDependentTravelTime[t] = t;  // t is the departure time

				for (int i=0 ; i < path_element.m_LinkVector.size(); i++)  // for each pass link
				{
					DTALink* pLink = m_LinkNoMap[m_PathDisplayList[p].m_LinkVector[i]];
					if(pLink == NULL)
						break;

					path_element.m_TimeDependentTravelTime[t] += pLink->GetDynamicTravelTime(path_element.m_TimeDependentTravelTime[t], m_PrimaryDataSource );

				}

				path_element.m_TimeDependentTravelTime[t] -= t; // remove the starting time, so we have pure travel time;

				ASSERT(path_element.m_TimeDependentTravelTime[t]>=0);

				if( path_element.m_MaxTravelTime < path_element.m_TimeDependentTravelTime[t])
					path_element.m_MaxTravelTime = path_element.m_TimeDependentTravelTime[t];

				for(int tt=1; tt<TIME_DEPENDENT_TRAVLE_TIME_CALCULATION_INTERVAL; tt++)
				{
					path_element.m_TimeDependentTravelTime[t+tt] = path_element.m_TimeDependentTravelTime[t];
				}


				//                              TRACE("\n path %d, time at %d = %f",p, t,path_element.m_TimeDependentTravelTime[t]  );

			}

	

		}


/*
	if(g_pPathMOEDlg  && g_pPathMOEDlg ->GetSafeHwnd ())
	{
		m_PathMOEDlgShowFlag = true;
		if(m_PathDisplayList.size() > 0)
		{
			if(g_pPathMOEDlg==NULL)
			{
				g_pPathMOEDlg = new CDlgPathMOE();
				g_pPathMOEDlg->m_pDoc  = this;

				g_pPathMOEDlg->Create(IDD_DIALOG_PATHMOE);
			}
			g_pPathMOEDlg->InsertPathMOEItem();

			g_pPathMOEDlg->ShowWindow(SW_SHOW);
		}
	}
	*/
		return 1;
}


DTALink* CTLiteDoc::FindLinkFromSensorLocation(float x, float y, CString orientation)
{
	double Min_distance = m_NetworkRect.Width()/100;  // set the selection threshod

	std::list<DTALink*>::iterator iLink;

	int SelectedLinkID = -1;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		GDPoint p0, pfrom, pto;
		p0.x  = x; p0.y  = y;
		pfrom.x  = (*iLink)->m_FromPoint.x; pfrom.y  = (*iLink)->m_FromPoint.y;
		pto.x  = (*iLink)->m_ToPoint.x; pto.y  = (*iLink)->m_ToPoint.y;

		if(orientation.MakeUpper().Find('E')>0 && pfrom.x > pto.x)  // East, Xfrom should be < XTo
			continue;  //skip

		if(orientation.MakeUpper().Find('S')>0 && pfrom.y < pto.y)  // South, Yfrom should be > YTo
			continue;

		if(orientation.MakeUpper().Find('W')>0 && pfrom.x < pto.x)  // West, Xfrom should be > XTo
			continue;

		if(orientation.MakeUpper().Find('N')>0 && pfrom.y > pto.y)  // North, Yfrom should be < YTo
			continue;

		float distance = g_GetPoint2LineDistance(p0, pfrom, pto, m_UnitDistance);

		if(distance >=0 && distance < Min_distance)
		{
			return (*iLink);

		}
	}

	return NULL;

}

void CapacityReduction::GenerateAdditionalDelayDistribution(float EntranceTime,int AgentID)
{
// to do: 
	// 1. from mean, COV -> lognormal 
	// without incidents -> random capacity reduction due to irregularity 
	// with incidents, start with capacity reduction 
	// 2. additional delay

	int RandomSeed = AgentID;

	for (int i=0;i<100;i++)
	{

	}

	// call g_GetRandomRatio() 100 times;
	// for incidents, first percentage

}