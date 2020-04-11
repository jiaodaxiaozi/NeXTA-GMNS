// TLite.h : main header file for the TLite application
//
//  Portions Copyright 2010 Xuesong Zhou (xzhou99@gmail.com), Jeff Taylor (jeffrey.taylor.d@gmail.com)

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
#include "Geometry.h"
#include "CSVParser.h"
#include "TLite.h"
#include "MainFrm.h"
#include "Shellapi.h"
#include "Network.h"
#include "TLiteDoc.h"
#ifndef _WIN64
#include "Data-Interface//include//ogrsf_frmts.h"
#endif 
class AMSMovementData
{
public:

	int UpNode,DestNode,Lanes,Speed,SatFlow,Volume,Grade;

	int Phase1, Phase2,Phase3,Phase4;
	int PermPhase1,PermPhase2,PermPhase3,PermPhase4;
	AMSMovementData ()
	{
		UpNode = 0;
		DestNode = 0;
		Lanes = 0;
		Speed = 0;
		Phase1 = 0;
		Phase2 = 0;
		Phase3 = 0;
		Phase4 = 0;

		PermPhase1 = 0;
		PermPhase2 = 0;
		PermPhase3 = 0;
		PermPhase4 = 0;

		SatFlow = 0;
		Volume = 0;
		Grade = 0;

	}

};
bool compare_MovementData (DTANodeMovement first, DTANodeMovement second)
{
	if(first.movement_approach_turn < second.movement_approach_turn) return true;
	else 
		return false;
}

void CTLiteDoc::OGDF_WriteGraph(CString FileName)
{
	std::ofstream GMLFile;
	int nextId = 0;

	GMLFile.open (FileName, ios::out);
	if (GMLFile.is_open())
	{
		GMLFile.width(15);
		GMLFile.precision(7) ;
		GMLFile.setf(ios::fixed);
	}
	else
	{
		return;
	}
	GMLFile << "Creator \"NeXTA::writeGML\"\n";
	GMLFile << "directed 1\n";

	GMLFile << "graph [\n";

	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		GMLFile << "node [\n";
		GMLFile << "id " << (*iNode)->m_NodeID << "\n";
		GMLFile << "label  " << "\"" << (*iNode)->m_Name.c_str () << "\"\n";
		GMLFile << "x  " << (*iNode)->pt.x << "\n";
		GMLFile << "y  " << (*iNode)->pt.y << "\n";
		GMLFile << "]\n"; // node
	}

	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		GMLFile << "edge [\n";
		GMLFile << "source " << (*iLink)->m_FromNodeNumber << "\n";
		GMLFile << "target " << (*iLink)->m_ToNodeNumber << "\n";
		GMLFile << "weight " << (*iLink)->m_Length << "\n";
		GMLFile << "]\n"; // edge

	}

	GMLFile << "]\n"; // graph
	GMLFile.close();

}
float ComputeCapacity(float capacity_in_pcphpl,int link_capacity_flag, float free_speed, int number_of_lanes)
{

	if(link_capacity_flag == 1)
		capacity_in_pcphpl = capacity_in_pcphpl/max(1,number_of_lanes);

	if(capacity_in_pcphpl == 2)  // link capacity flag == 2, use default
	{  // generate default capacity 
		if( free_speed <=30)
			return 600;
		if( free_speed <=50)
			return 900;
		else if (free_speed <=60)
			return 1200;
		else 
			return 1800;
	}
	//default 0;

	return capacity_in_pcphpl;
}

void CTLiteDoc::FieldNameNotExistMessage(CString FieldName, CString KeyName, CString FileName = "import_GIS_settings.csv")
{
	CString message;
	message.Format("Field %s does not exist for %s. Please check file %s.",FieldName, KeyName, FileName);
	AfxMessageBox(message);
}

void CTLiteDoc::FieldNameNotExistMessage(std::string FieldName, std::string KeyName, std::string FileName = "import_GIS_settings.csv")
{
	CString message;
	message.Format("Field %s does not exist for %s. Please check file %s.",FieldName.c_str(), KeyName.c_str(), FileName.c_str () );
	AfxMessageBox(message);
}

BOOL CTLiteDoc::OnOpenAMSDocument(CString FileName)
{
	return true;
}





void  CTLiteDoc::ReadSynchroUniversalDataFiles()
{
	CWaitCursor wait;
	static char BASED_CODE szFilter[] = "Synchro UTDF LAYOUT File (LAYOUT.csv)|LAYOUT.csv||";
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter);

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CString NetworkFile = pMainFrame->m_CurrentDirectory;
	dlg.m_ofn.lpstrInitialDir = NetworkFile;

	if(dlg.DoModal() == IDOK)
	{
		CString ProjectFile = dlg.GetPathName();
		CString Synchro_directory = ProjectFile.Left(ProjectFile.ReverseFind('\\') + 1);


		if(ReadSynchroLayoutFile(dlg.GetPathName()))
		{
			ReadSynchroLaneFile(Synchro_directory+"lanes.csv");  // we need lower cases of lanes.csv, rather than LANES.CSV
			ReadSynchroPhasingFile(Synchro_directory + "phasing.csv");
			OffsetLink();
			CalculateDrawingRectangle(true);
			m_bFitNetworkInitialized  = false;

		}

		UpdateAllViews(0);

	}

}

bool  CTLiteDoc::ReadSynchroLayoutFile_And_AddOutgoingLinks_For_ExternalNodes(LPCTSTR lpszFileName)
{

	string direction_vector[8] = {"NID","SID","EID","WID","NEID","NWID","SEID","SWID"};

	CCSVParser parser;
	parser.m_bSkipFirstLine  = true;  // skip the first line  
	if (parser.OpenCSVFile(lpszFileName))
	{
		int i=0;
		while(parser.ReadRecord())
		{
			int node_id;
			string name;
			DTANode* pNode = 0;

			int node_type;
			double X;
			double Y;
			if(parser.GetValueByFieldName("INTID",node_id) == false)
				break;

			if(node_id==89)
			{
				TRACE("");

			}
			if(!parser.GetValueByFieldName("INTNAME",name))
				name = "";

			if(!parser.GetValueByFieldName("TYPE",node_type))
				node_type = 0;

			// use the X and Y as default values first
			bool bFieldX_Exist = parser.GetValueByFieldName("X",X);
			parser.GetValueByFieldName("Y",Y);

			//if(node_type != 1)  // not external node //XUESONG
			{

				for(int direction = 0; direction < 8; direction++)
				{
					int outgoing_node_number;
					if(parser.GetValueByFieldName(direction_vector[direction],outgoing_node_number))// value exits
					{
						// add a new link
						long from_node_id = m_NodeNumbertoNodeNoMap[node_id];
						long to_node_id = m_NodeNumbertoNodeNoMap[outgoing_node_number];

						if(m_NodeNoMap.find(to_node_id) != m_NodeNoMap.end())
						{
							//if(m_NodeNoMap[to_node_id]->m_ControlType == m_ControlType_ExternalNode) XUESONG
							{  // add new link if the outbound node is an external node
								AddNewLink(from_node_id, to_node_id,false, false);
								AddNewLink(to_node_id,from_node_id,false, false);
								TRACE("Add New Link = %d, %d\n", node_id, outgoing_node_number);
							}

						}


					}


				}

			}

		}
	}

	return true;
}


bool CTLiteDoc::ReadSynchroLayoutFile(LPCTSTR lpszFileName)
{
	CWaitCursor wait;
	m_NodeTypeMap[0] = "signalized intersection";
	m_NodeTypeMap[1] = "external node";
	m_NodeTypeMap[2] = "bend";
	m_NodeTypeMap[3] = "unsignalized";
	m_NodeTypeMap[4] = "roundabout";
	m_NodeTypeMap[5] = "";
	m_NodeTypeMap[6] = "";

	float unit = 1; 
	CCSVParser parser;
	parser.m_bSkipFirstLine  = true;  // skip the first line  
	if (parser.OpenCSVFile(lpszFileName))
	{
		int i=0;
		while(parser.ReadRecord())
		{
			int node_id;
			string name;
			DTANode* pNode = 0;

			int node_type;
			double X;
			double Y;
			if(parser.GetValueByFieldName("INTID",node_id) == false)
				break;

			if(!parser.GetValueByFieldName("INTNAME",name))
				name = "";

			if(!parser.GetValueByFieldName("TYPE",node_type))
				node_type = 0;

			// use the X and Y as default values first
			bool bFieldX_Exist = parser.GetValueByFieldName("X",X,false);
			parser.GetValueByFieldName("Y",Y,false);

			if(m_NodeNumbertoNodeNoMap.find(node_id) != m_NodeNumbertoNodeNoMap.end())
			{
				CString error_message;
				error_message.Format ("Node %d in node.csv has been defined twice. Please check.", node_id);
				AfxMessageBox(error_message);
				return 0;
			}

			pNode = new DTANode;
			pNode->m_Name = name;

			if(node_type == 0) 
				pNode->m_ControlType = m_ControlType_PretimedSignal;
			if(node_type == 1) 
				pNode->m_ControlType = m_ControlType_ExternalNode;
			if(node_type == 2) 
				pNode->m_ControlType = m_ControlType_NoControl;


			pNode->pt.x = X/5280.0f;  // feet to mile
			pNode->pt.y = Y/5280.0f;  // feet to mile

			pNode->m_NodeID = node_id;
			pNode->m_NodeNo = i;
			pNode->m_ZoneID = 0;
			m_NodeSet.push_back(pNode);
			m_NodeNoMap[i] = pNode;
			m_NodeNotoNumberMap[i] = node_id;
			m_NodeNumbertoNodeNoMap[node_id] = i;
			i++;

			TRACE("node = %d, X: %f, Y: %f\n", node_id, X, Y);

		}

		parser.CloseCSVFile ();

		// add links
		ReadSynchroLayoutFile_And_AddOutgoingLinks_For_ExternalNodes(lpszFileName);

		m_NodeDataLoadingStatus.Format ("%d nodes are loaded from file %s.",m_NodeSet.size(),lpszFileName);
		return true;
	}else
	{
		AfxMessageBox("Error: File LAYOUT.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return false;
		//		g_ProgramStop();
	}


}


bool CTLiteDoc::ReadSynchroLaneFile(LPCTSTR lpszFileName)
{
	bool bCreateNewNodeFlag = false;
	int LayerNo = 0;

	long i = 0;
	DTALink* pLink = 0;
	float default_distance_sum=0;
	float length_sum = 0;

	CString error_message;

	bool bTwoWayLinkFlag = false;
	CCSVParser parser;
	parser.m_bSkipFirstLine  = true;  // skip the first line  : Lane Group Data

	const int LaneColumnSize = 32;
	//	string lane_Column_name_str[LaneColumnSize] = { "NBL2","NBL","NBT","NBR","NBR2","SBL2","SBL","SBT","SBR","SBR2","EBL2","EBL","EBT","EBR","EBR2","WBL2","WBL","WBT","WBR","WBR2","NEL","NET","NER","NWL","NWT","NWR","SEL","SET","SER","SWL","SWT","SWR"};
	string lane_Column_name_str[LaneColumnSize] = {
		"NBT","NBL2","NBL","NBR","NBR2",
		"SBT","SBL2","SBL","SBR","SBR2",
		"EBT","EBL2","EBL","EBR","EBR2",
		"WBT","WBL2","WBL","WBR","WBR2",
		"NET","NEL","NER",
		"NWT","NWL","NWR",
		"SET","SEL","SER",
		"SWT","SWL","SWR"};


	if (parser.OpenCSVFile(lpszFileName))
	{
		bool bNodeNonExistError = false;

		std::map<string,AMSMovementData> LaneDataMap;

		bool NewLinkFlag=false;

		bool Lane_data_read_flag = false;
		while(parser.ReadRecord())
		{
			Lane_data_read_flag = true;
			int link_id = 0;
			long from_node_id = -1;
			long to_node_id = -1;
			int direction = 0;
			double length_in_mile;
			int number_of_lanes= 1;
			int free_speed= 60;
			double capacity_in_pcphpl= 2000;
			int type = 3;  //principle arterial streets
			string name;
			float k_jam = 180;

			float wave_speed_in_mph = 12;
			string mode_code = "";

			float grade = 0;

			parser.GetValueByFieldName("RECORDNAME",name);
			int INTID;
			parser.GetValueByFieldName("INTID",INTID);

			int UpNode = 0;
			int DestNode = 0;
			int Lanes = 0;
			int Speed = 0;
			int Phase1= 0;
			int SatFlow = 0;
			int Volume = 0;

			int m;
			//			switch (name)
			if (name == "Up Node" || name == "UpNodeID")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					UpNode = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],UpNode);
					if(UpNode>0)
					{
						TRACE("UpNode -> CurNode:%d -> %d ",UpNode,INTID);
						LaneDataMap[lane_Column_name_str[m]].UpNode = UpNode;
					}
				}
			}

			if (name == "Dest Node"  || name == "DestNodeID")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int DestNode = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],DestNode);
					if(Lanes>0)
					{

						LaneDataMap[lane_Column_name_str[m]].DestNode  = DestNode;
					}
				}

			}
			if (name == "Lanes")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Lanes = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Lanes);
					if(Lanes>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Lanes  = Lanes;
					}
				}
			}

			if (name == "Speed")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Speed = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Speed);
					if(Speed>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Speed  = Speed;
					}
				}
			}

			if (name == "Phase1")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase1 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase1);
					if(Phase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase1  = Phase1;
					}
				}
			}

			if (name == "Phase2")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase2 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase2);
					if(Phase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase2  = Phase2;
					}
				}
			}

			if (name == "Phase3")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase3 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase3);
					if(Phase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase3  = Phase3;
					}
				}
			}

			if (name == "Phase4")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase4 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase4);
					if(Phase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase4  = Phase4;
					}
				}
			}


			// permitted phase
			if (name == "PermPhase1")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase1 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase1);
					if(PermPhase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase1  = PermPhase1;
					}
				}
			}

			if (name == "PermPhase2")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase2 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase2);
					if(PermPhase2>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase2  = PermPhase2;
					}
				}
			}

			if (name == "PermPhase3")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase3 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase3);
					if(PermPhase3>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase3  = PermPhase3;
					}
				}
			}

			if (name == "PermPhase4")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase4 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase4);
					if(PermPhase4>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase4  = PermPhase4;
					}
				}
			}


			if (name == "SatFlowRatePerLaneGroup")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int SatFlow = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],SatFlow);
					if(SatFlow>0)
					{

						LaneDataMap[lane_Column_name_str[m]].SatFlow  = SatFlow;
					}
				}
			}

			if (name == "Grade")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Grade = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Grade);
					if(SatFlow>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Grade  = Grade;
					}
				}
			}

			if (name == "TravelTime")  // we have finished reading all required data
			{
				//add links and set up phases;

				int max_phase_number = 0;

				long to_node_id  = INTID;

				// set control type
				for(m = 0; m< LaneColumnSize; m++)
				{

					if(LaneDataMap.find(lane_Column_name_str[m]) != LaneDataMap.end() )  // data exists
					{


						from_node_id  = LaneDataMap[lane_Column_name_str[m]].UpNode;
						int dest_node_id  = LaneDataMap[lane_Column_name_str[m]].DestNode ;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].Phase1 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].Phase1;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].Phase2 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].Phase2;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].Phase3 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].Phase3;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].Phase4 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].Phase4;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].PermPhase1 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].PermPhase1;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].PermPhase2 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].PermPhase2;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].PermPhase3 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].PermPhase3;

						if( max_phase_number < LaneDataMap[lane_Column_name_str[m]].PermPhase4 )
							max_phase_number = LaneDataMap[lane_Column_name_str[m]].PermPhase4;

						TRACE("\nmovement %s, from node %d to node %d",
							lane_Column_name_str[m].c_str (), from_node_id, to_node_id);



						if(from_node_id<=0 || to_node_id <= 0)
							continue;

						// add link
						DTALink* pExistingLink =  FindLinkWithNodeIDs(m_NodeNumbertoNodeNoMap[from_node_id],m_NodeNumbertoNodeNoMap[to_node_id]);

						if(pExistingLink)
						{
							//the approach has been processed.
							continue;
						}

						if(from_node_id == to_node_id)
						{
							continue;
						}

						if(pLink->m_FromNodeNumber ==0 || pLink->m_ToNodeNumber ==0 )
						{
							//skip
							continue;
						}

						TRACE("\n add ->> movement %s, from node %d to node %d, with %d links",
							lane_Column_name_str[m].c_str (), from_node_id, to_node_id,  LaneDataMap[lane_Column_name_str[m]].Lanes );

						bool bToBeShifted = true;
						int number_of_lanes = LaneDataMap[lane_Column_name_str[m]].Lanes ;
						float free_speed = LaneDataMap[lane_Column_name_str[m]].Speed ;
						float capacity_in_pcphpl =  LaneDataMap[lane_Column_name_str[m]].SatFlow / max(1,number_of_lanes);  // convert per link flow rate to per lane flow rate

						int m_SimulationHorizon = 1;
						DTALink* pLink = new DTALink(m_SimulationHorizon);
						pLink->m_LayerNo = LayerNo;
						pLink->m_LinkNo = m_LinkSet.size();
						pLink->m_LinkID = m_LinkSet.size();


						pLink->m_FromNodeNumber = from_node_id;

						pLink->m_ToNodeNumber = to_node_id;
						pLink->m_Direction  = 1;

						if(pLink->m_FromNodeNumber == 12 && pLink->m_ToNodeNumber == 2)
							TRACE("");

						pLink->m_FromNodeID = m_NodeNumbertoNodeNoMap[from_node_id];
						pLink->m_ToNodeID= m_NodeNumbertoNodeNoMap[to_node_id];

						pLink->m_FromPoint = m_NodeNoMap[pLink->m_FromNodeID]->pt;
						pLink->m_ToPoint = m_NodeNoMap[pLink->m_ToNodeID]->pt;
						float length_in_mile =  pLink->DefaultDistance();  // cooridnates have been changed to mile for unit

						pLink->m_bToBeShifted = bToBeShifted; 

						pLink->m_NumberOfLanes= number_of_lanes;
						pLink->m_FreeSpeed= max(10,free_speed);  // minimum Free Speed is 10 mph
						pLink->m_avg_simulated_speed = pLink->m_FreeSpeed;

						//				pLink->m_Length= max(length_in_mile, pLink->m_FreeSpeed*0.1f/60.0f);  // minimum distance, special note: we do not consider the minimum constraint here, but a Agent cannot travel longer then 0.1 seconds
						pLink->m_Length= length_in_mile;
						pLink->m_FreeFlowTravelTime = pLink->m_Length/pLink->m_FreeSpeed*60.0f;  // convert from hour to min
						pLink->m_StaticTravelTime = pLink->m_FreeFlowTravelTime;

						pLink->m_MaximumServiceFlowRatePHPL= capacity_in_pcphpl;
						pLink->m_LaneCapacity  = pLink->m_MaximumServiceFlowRatePHPL;
						pLink->m_link_type= 4;  //arterial
						pLink->m_Grade = LaneDataMap[lane_Column_name_str[m]].Grade;

						int k_jam = 180;
						int wave_speed_in_mph = 12;

						pLink->m_Kjam = k_jam;
						pLink->m_Wave_speed_in_mph  = wave_speed_in_mph;

						m_NodeNoMap[pLink->m_FromNodeID ]->m_Connections+=1;
						m_NodeNoMap[pLink->m_FromNodeID ]->m_OutgoingLinkVector.push_back(pLink->m_LinkNo);
						m_NodeNoMap[pLink->m_ToNodeID ]->m_Connections+=1;

						pLink->m_FromPoint = m_NodeNoMap[pLink->m_FromNodeID]->pt;
						pLink->m_ToPoint = m_NodeNoMap[pLink->m_ToNodeID]->pt;

						unsigned long LinkKey = GetLinkKey( pLink->m_FromNodeID, pLink->m_ToNodeID);

						m_NodeNotoLinkMap[LinkKey] = pLink;

						__int64  LinkKey2 = GetLink64Key(pLink-> m_FromNodeNumber,pLink->m_ToNodeNumber);
						m_NodeNumbertoLinkMap[LinkKey2] = pLink;

						m_LinkNotoLinkMap[m_LinkSet.size()] = pLink;

						default_distance_sum+= pLink->DefaultDistance();
						length_sum += pLink ->m_Length;
						//			pLink->SetupMOE();

						GDPoint	pt;
						pt.x = pLink->m_FromPoint.x;
						pt.y = pLink->m_FromPoint.y;
						pLink->m_Original_ShapePoints .push_back (pt);
						pLink->m_ShapePoints .push_back (pt);


						pt.x = pLink->m_ToPoint.x;
						pt.y = pLink->m_ToPoint.y;
						pLink->m_Original_ShapePoints .push_back (pt);
						pLink->m_ShapePoints .push_back (pt);

						TRACE("\nAdd link no.%d,  %d -> %d",i,pLink->m_FromNodeNumber, pLink->m_ToNodeNumber );
						m_LinkNoMap[m_LinkSet.size()]  = pLink;
						m_LinkSet.push_back (pLink);
					}  // per major approach

				} // for each movement

				DTANode* pNode = m_NodeNoMap[m_NodeNumbertoNodeNoMap[to_node_id]];	

				LaneDataMap.clear();  // clear data after adding a set of links
			}
		


			}
			if(Lane_data_read_flag == false)
			{
				AfxMessageBox("Please check if Lanes.csv follows CSV format, as 0 record has been read in NeXTA.");
			
			}
	}

	m_UnitDistance  = 1.0f;
	if(length_sum>0.000001f)
		m_UnitDistance=  default_distance_sum /length_sum;

	m_LinkDataLoadingStatus.Format ("%d links are loaded.",m_LinkSet.size());


	// add movement
	parser.CloseCSVFile ();

	if (parser.OpenCSVFile(lpszFileName))
	{
		bool bNodeNonExistError = false;

		std::map<string,AMSMovementData> LaneDataMap;

		bool NewLinkFlag=false;

		while(parser.ReadRecord())
		{
			int link_id = 0;
			long from_node_id;
			long to_node_id ;
			int direction = 0;
			double length_in_mile;
			int number_of_lanes= 1;
			int free_speed= 60;
			double capacity_in_pcphpl= 2000;
			int type = 1;
			string name;
			float k_jam = 180;

			float wave_speed_in_mph = 12;
			string mode_code = "";

			float grade = 0;

			parser.GetValueByFieldName("RECORDNAME",name);
			int INTID;
			parser.GetValueByFieldName("INTID",INTID);

			int UpNode = 0;
			int DestNode = 0;
			int Lanes = 0;
			int Speed = 0;
			int Phase1= 0;
			int SatFlow = 0;
			int Volume = 0;

			int m;
			//			switch (name)
			if (name == "Up Node")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					UpNode = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],UpNode);
					if(UpNode>0)
					{
						TRACE("UpNode -> CurNode:%d -> %d ",UpNode,INTID);
						LaneDataMap[lane_Column_name_str[m]].UpNode = UpNode;
					}
				}
			}

			if (name == "Dest Node")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int DestNode = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],DestNode);

					LaneDataMap[lane_Column_name_str[m]].DestNode  = DestNode;
				}

			}
			if (name == "Lanes")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Lanes = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Lanes);
					if(Lanes>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Lanes  = Lanes;
					}
				}
			}

			if (name == "Speed")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Speed = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Speed);
					if(Speed>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Speed  = Speed;
					}
				}
			}

			if (name == "Phase1")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase1 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase1);
					if(Phase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase1  = Phase1;
					}
				}
			}

			if (name == "Phase2")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase2 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase2);
					if(Phase2>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase2  = Phase2;
					}
				}
			}

			if (name == "Phase3")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase3 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase3);
					if(Phase3>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase3  = Phase3;
					}
				}
			}

			if (name == "Phase4")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Phase4 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Phase4);
					if(Phase4>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Phase4  = Phase4;
					}
				}
			}


			// permitted phase
			if (name == "PermPhase1")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase1 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase1);
					if(PermPhase1>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase1  = PermPhase1;
					}
				}
			}

			if (name == "PermPhase2")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase2 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase2);
					if(PermPhase2>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase2  = PermPhase2;
					}
				}
			}

			if (name == "PermPhase3")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase3 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase3);
					if(PermPhase3>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase3  = PermPhase3;
					}
				}
			}

			if (name == "PermPhase4")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int PermPhase4 = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],PermPhase4);
					if(PermPhase4>0)
					{

						LaneDataMap[lane_Column_name_str[m]].PermPhase4  = PermPhase4;
					}
				}
			}

			if (name == "SatFlowRatePerLaneGroup")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int SatFlow = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],SatFlow);
					if(SatFlow>0)
					{

						LaneDataMap[lane_Column_name_str[m]].SatFlow  = SatFlow;
					}
				}
			}

			if (name == "Grade")
			{
				for(m = 0; m< LaneColumnSize; m++)
				{
					int Grade = 0;
					parser.GetValueByFieldName(lane_Column_name_str[m],Grade);
					if(SatFlow>0)
					{

						LaneDataMap[lane_Column_name_str[m]].Grade  = Grade;
					}
				}
			}

			if (name == "TravelTime")  // we have finished reading all required data
			{
				//add movement and links
				for(m = 0; m< LaneColumnSize; m++)
				{

					if(LaneDataMap.find(lane_Column_name_str[m]) != LaneDataMap.end() )  // data exists
					{

						long from_node_id  = LaneDataMap[lane_Column_name_str[m]].UpNode;
						long to_node_id  = INTID;
						int dest_node_id  = LaneDataMap[lane_Column_name_str[m]].DestNode ;

						TRACE("\nmovement %s, from node %d to node %d -> %d",lane_Column_name_str[m].c_str (), from_node_id, to_node_id,dest_node_id);

						DTANodeMovement element;

						element.in_link_from_node_id = m_NodeNumbertoNodeNoMap[from_node_id];		
						element.in_link_to_node_id = m_NodeNumbertoNodeNoMap[to_node_id];						
						element.out_link_to_node_id = m_NodeNumbertoNodeNoMap[dest_node_id];	

						DTALink* pIncomingLink =  FindLinkWithNodeIDs(m_NodeNumbertoNodeNoMap[from_node_id],m_NodeNumbertoNodeNoMap[to_node_id]);

						if(pIncomingLink)
							element.IncomingLinkNo = pIncomingLink->m_LinkNo  ;

						DTALink* pOutcomingLink =  FindLinkWithNodeIDs(m_NodeNumbertoNodeNoMap[to_node_id],m_NodeNumbertoNodeNoMap[dest_node_id]);

						if(pOutcomingLink)
							element.OutgoingLinkNo = pOutcomingLink->m_LinkNo ;


						GDPoint p1, p2, p3;
						p1  = m_NodeNoMap[element.in_link_from_node_id]->pt;
						p2  = m_NodeNoMap[element.in_link_to_node_id]->pt;
						p3  = m_NodeNoMap[element.out_link_to_node_id]->pt;

						int relative_angel_difference_from_main_direction = 0;
						element.movement_direction = Find_Angle_to_Approach_4_direction(Find_P2P_Angle(p1,p2),relative_angel_difference_from_main_direction);
						element.movement_turn = Find_PPP_to_Turn(p1,p2,p3);

						// determine  movement type /direction here
						element.movement_approach_turn = DTA_LANES_COLUME_init;

						switch (element.movement_direction)
						{
						case DTA_North:
							switch (element.movement_turn)
							{
							case DTA_LeftTurn: element.movement_approach_turn = DTA_NBL; break;
							case DTA_Through: element.movement_approach_turn = DTA_NBT; break;
							case DTA_RightTurn: element.movement_approach_turn = DTA_NBR; break;
							}
							break;
						case DTA_East:

							switch (element.movement_turn)
							{
							case DTA_LeftTurn: element.movement_approach_turn = DTA_EBL; break;
							case DTA_Through: element.movement_approach_turn = DTA_EBT; break;
							case DTA_RightTurn: element.movement_approach_turn = DTA_EBR; break;
							}
							break;
						case DTA_South:
							switch (element.movement_turn)
							{
							case DTA_LeftTurn: element.movement_approach_turn = DTA_SBL; break;
							case DTA_Through: element.movement_approach_turn = DTA_SBT; break;
							case DTA_RightTurn: element.movement_approach_turn = DTA_SBR; break;
							}
							break;
						case DTA_West:

							switch (element.movement_turn)
							{
							case DTA_LeftTurn: element.movement_approach_turn = DTA_WBL; break;
							case DTA_Through: element.movement_approach_turn = DTA_WBT; break;
							case DTA_RightTurn: element.movement_approach_turn = DTA_WBR; break;
							}
							break;
						}


						DTANode* pNode = m_NodeNoMap[m_NodeNumbertoNodeNoMap[to_node_id]];	

						ASSERT(pNode!=NULL);


						std::string timing_plan_name = "1";
						if(pOutcomingLink!=NULL)
						{ 
						DTA_Phasing_Data_Matrix phasing_element = GetPhaseData(to_node_id, timing_plan_name);

						int SelectedPhaseNumber = LaneDataMap[lane_Column_name_str[m]].Phase1;

						if(SelectedPhaseNumber>0)
						{
						CString movement_vector = phasing_element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + SelectedPhaseNumber), PHASE_MOVEMENT_VECTOR);
						CString sub_movement_str;

						sub_movement_str.Format(";%d_%d_%s", from_node_id, pOutcomingLink->m_ToNodeNumber,
								GetTurnShortString(element.movement_turn));

						movement_vector += sub_movement_str;
						//update movement vector string
						SetupPhaseData(to_node_id, timing_plan_name, SelectedPhaseNumber, PHASE_MOVEMENT_VECTOR, movement_vector);

						CString movement_dir_vector = phasing_element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + SelectedPhaseNumber), PHASE_MOVEMENT_DIR_VECTOR);
						movement_dir_vector += GetTurnDirectionString(element.movement_approach_turn);
						//update movement vector string
						movement_dir_vector += ";";
						SetupPhaseData(to_node_id, timing_plan_name, SelectedPhaseNumber, PHASE_MOVEMENT_DIR_VECTOR, movement_dir_vector);

						}
						int SelectedPhaseNumber2 = LaneDataMap[lane_Column_name_str[m]].PermPhase1;

						if (SelectedPhaseNumber2>0)
						{
							CString movement_vector = phasing_element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + SelectedPhaseNumber2), PHASE_MOVEMENT_VECTOR);
							CString sub_movement_str;

							sub_movement_str.Format(";%d_%d_%s", from_node_id, pOutcomingLink->m_ToNodeNumber,
								GetTurnShortString(element.movement_turn));

							movement_vector += sub_movement_str;
							//update movement vector string
							SetupPhaseData(to_node_id, timing_plan_name, SelectedPhaseNumber2, PHASE_MOVEMENT_VECTOR, movement_vector);

							CString movement_dir_vector = phasing_element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + SelectedPhaseNumber2), PHASE_MOVEMENT_DIR_VECTOR);
							movement_dir_vector += GetTurnDirectionString(element.movement_approach_turn);
							//update movement vector string
							movement_dir_vector += ";";
							SetupPhaseData(to_node_id, timing_plan_name, SelectedPhaseNumber2, PHASE_MOVEMENT_DIR_VECTOR, movement_dir_vector);


						}
						}
					// educated guess about the associatd phase, as a movement can be associated with multiple phases 
						//element.phase_index = max(,LaneDataMap[lane_Column_name_str[m]].PermPhase1);

						pNode->m_MovementDataMap.m_MovementVector.push_back(element);

					}  // per major approach



				} // for each movement
				LaneDataMap.clear();  // clear data after adding a set of links
			}

		}
	}

		for(int tp = 0; tp< m_TimingPlanVector.size(); tp++)  // first loop for each timing plan
		{


			std::list<DTANode*>::iterator iNode;
			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{

				(*iNode)->SortMovementVector();

			}
		}
	ConstructMovementVector();
//		GenerateOffsetLinkBand();
	return 1;
}


bool CTLiteDoc::ReadSynchroPhasingFile(LPCTSTR lpszFileName)
{

	CString error_message;

	CCSVParser parser;
	parser.m_bSkipFirstLine = true;  // skip the first line  : Lane Group Data

	const int PhasingColumnSize = 8;
	string phasing_Column_name_str[PhasingColumnSize] = {
		"D1","D2","D3","D4","D5",
		"D6","D7","D8"};


	if (parser.OpenCSVFile(lpszFileName))
	{
		bool bNodeNonExistError = false;

		while (parser.ReadRecord())
		{

			string name;
			parser.GetValueByFieldName("RECORDNAME", name);
			int INTID;
			parser.GetValueByFieldName("INTID", INTID);

			int m;
			//			switch (name)
			if (name == "ActGreen")
			{
				for (m = 0; m < PhasingColumnSize; m++)
				{
					int ActGreen = 0;
					parser.GetValueByFieldName(phasing_Column_name_str[m], ActGreen);
					if (ActGreen > 0)
					{
						std::string timing_plan_name = "1";
						long to_node_id = m_NodeNumbertoNodeNoMap[INTID];
						DTA_Phasing_Data_Matrix phasing_element = GetPhaseData(to_node_id, timing_plan_name);

						int SelectedPhaseNumber = m + 1;


						CString actgreen_str;

						actgreen_str.Format("%d", ActGreen);

						//update movement vector string
						SetupPhaseData(INTID, timing_plan_name, SelectedPhaseNumber, PHASE_GreenDuration, actgreen_str);


					}
				}
			}



		}
	}

	return 1;
}

void CTLiteDoc::OnImportDemanddataset()
{
	//CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	//	_T("Importing Configuration (*.ini)|*.ini|"));
	//if(dlg.DoModal() == IDOK)
	//{
	//CTime LoadingStartTime = CTime::GetCurrentTime();

	//FILE* st = NULL;
	////	cout << "Reading file node.csv..."<< endl;

	//CString directory;
	//CString ProjectFile = dlg.GetPathName();
	//directory = ProjectFile.Left(ProjectFile.ReverseFind('\\') + 1);

	//// default data type definition files

	//m_AMSLogFile.open ( m_ProjectDirectory + "AMS_demand_conversion_log.csv", ios::out);
	//if (m_AMSLogFile.is_open())
	//{
	//	m_AMSLogFile.width(12);
	//	m_AMSLogFile.precision(3) ;
	//	m_AMSLogFile.setf(ios::fixed);
	//	m_AMSLogFile << "Start AMS demand reading..." << endl;
	//}else
	//{
	//	AfxMessageBox("File AMS_demand_conversion_log.csv cannot be opened, and it might be locked by another program or the target data folder is read-only.");
	//	return false;
	//}

	//int demand_format_flag = 0;
	//char demand_file_name[_MAX_STRING_SIZE] = "input_demand.csv";
	//char demand_file_field_name[_MAX_STRING_SIZE] = "demand_file_name";
	//char demand_type_field_name[_MAX_STRING_SIZE] ;
	//char demand_start_time_field_name[_MAX_STRING_SIZE];
	//char demand_end_field_name[_MAX_STRING_SIZE];

	//WritePrivateProfileString("demand_table","format_definition","0: AMS Demand CSV; 1: OD Matrix CSV; 2: 3-column format; 3: TransCAD 3-column CSV;4:VISUM matrix 8; 10: Gravity model",ProjectFileName);
	//demand_format_flag = g_GetPrivateProfileInt("demand_table","demand_format",1,ProjectFileName);

	//int number_of_tables = g_GetPrivateProfileInt("demand_table","number_of_files ",1,ProjectFileName);

	//for(int t = 1; t<= number_of_tables; t++)
	//{
	//fprintf(demand_file_field_name,"demand_file_name_table%d", t);
	//fprintf(demand_start_time_field_name,"demand_type_table%d", t);
	//fprintf(demand_end_field_name,"start_time_in_min_table%d", t);
	//fprintf(demand_end_field_name,"end_time_in_min_table%d", t);

	//g_GetProfileString("demand_table",demand_file_field_name,"input_demand.csv",demand_file_name,sizeof(demand_file_name),ProjectFileName);
	//
	//int demand_type = g_GetPrivateProfileInt("demand_table","number_of_files ",1,ProjectFileName);
	//int start_time_in_min = g_GetPrivateProfileInt("demand_table","number_of_files ",1,ProjectFileName);
	//int end_time_in_min = g_GetPrivateProfileInt("demand_table","number_of_files ",1,ProjectFileName);

	//CString msg;
	//msg.Format("demand_format= %d specified in %s is not supported. Please contact developers.",demand_format_flag,ProjectFileName);
	//
	//switch (demand_format_flag)
	//{
	//case 0:	ReadDemandCSVFile(directory+demand_file_name); break;
	//case 1: ReadDemandMatrixFile(directory+demand_file_name,1); break;
	//case 2: ReadTransCADDemandCSVFile(directory+demand_file_name); break;
	//case 3: ReadTransCADDemandCSVFile(directory+demand_file_name); break;
		//case 10: RunGravityModel(); break;
	//	
	//default:
	//	{
	//	AfxMessageBox(msg);
	//	}
	//}
	//}  //for demand table

	//m_AMSLogFile.close();

	//}
}

void CTLiteDoc::OnImportSynchrocombinedcsvfile()
{
	CWaitCursor wait;
	static char BASED_CODE szFilter[] = "Synchro Combined CSV File (*.csv)|*.csv||";
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter);

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CString NetworkFile = pMainFrame->m_CurrentDirectory;
	dlg.m_ofn.lpstrInitialDir = NetworkFile;

	if(dlg.DoModal() == IDOK)
	{
		CString ProjectFile = dlg.GetPathName();
		CString Synchro_directory = ProjectFile.Left(ProjectFile.ReverseFind('\\') + 1);

		if(ReadSynchroCombinedCSVFile(dlg.GetPathName()))
		{
			OffsetLink();
			CalculateDrawingRectangle(true);
			m_bFitNetworkInitialized  = false;
		}

		m_LinkMOEMode = MOE_none;
		ShowTextLabel();

		UpdateAllViews(0);

	}


}

void CTLiteDoc::MapSignalDataAcrossProjects()
{

	class CrossReferenceNodeInfo
	{
	public:
		string intersection_name;
		string intersection_name2;
		int reference_node_id;
		int baseline_node_id;
	};

	// step 0: make sure two networks have been opened

	CTLiteDoc* pReferenceDoc = NULL;
	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	int project_index = 0 ;
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_NodeSet.size() >0 && (*iDoc)!=this)  
		{

			pReferenceDoc = (*iDoc);

			project_index++;
		}
		iDoc++;
	}

	if(pReferenceDoc == NULL)
	{

		AfxMessageBox("Please open the reference network data set in anoter window.");
		return;
	}

	int count = 0 ;
	std::list<DTANode*>::iterator  iNode;

	CFileDialog dlg (TRUE, "*.csv", "*.csv",OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_LONGNAMES,
		"Cross-reference Data File (*.csv)|*.csv||", NULL);
	if(dlg.DoModal() == IDOK)
	{

		CString SynchroProjectFile = dlg.GetPathName();

		CCSVParser parser;

		std::map<int, CrossReferenceNodeInfo> CrossReferenceNodeInfoMap;

		// step 1: read reference map
		CString cs = dlg.GetPathName();
		CT2CA pszConvertedAnsiString (cs);
		// construct a std::string using the LPCSTR input
		std::string strStd (pszConvertedAnsiString);

		if (parser.OpenCSVFile( strStd) )
		{

			string intersection_name;
			string intersection_name2;
			int reference_node_id;
			int baseline_node_id;

			while(parser.ReadRecord())
			{
				parser.GetValueByFieldName("baseline_node_id",baseline_node_id);

				if(baseline_node_id>=1)
				{
					parser.GetValueByFieldName("reference_node_id",reference_node_id);		
					parser.GetValueByFieldName("intersection_name",intersection_name);			
					parser.GetValueByFieldName("intersection_name2",intersection_name2);	

					CrossReferenceNodeInfoMap[baseline_node_id]. reference_node_id = reference_node_id;
					CrossReferenceNodeInfoMap[baseline_node_id]. intersection_name = intersection_name;
					CrossReferenceNodeInfoMap[baseline_node_id]. intersection_name2 = intersection_name2;


				}

			}


		}else
		{

		}

		// step 2: map cycle length and offset



		FILE* st = NULL;

		CString str_log_file;
		str_log_file.Format ("%s\\signal_data_reading_log.csv",m_ProjectDirectory);

		fopen_s(&st,str_log_file,"w");


		int count = 0;
		if( st !=NULL)
		{


			//	fprintf(st,"intersection_name1,intersection_name2, baseline_node_id,reference_node_id,base_line_cycle_length_in_second,reference_cycle_length_in_second\n");

			fprintf(st,"------\n");
			fprintf(st,"Step 1: Check node control type,\n");

			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{

				int baseline_node_id = (*iNode)->m_NodeID  ;
				if(CrossReferenceNodeInfoMap.find(baseline_node_id) != CrossReferenceNodeInfoMap.end())
				{

					if((*iNode)->m_ControlType != m_ControlType_PretimedSignal && (*iNode)->m_ControlType != m_ControlType_ActuatedSignal)
					{
						fprintf(st,"Baseline,Node,%d,is not an signalized intersection, control type is changed to Pretimed Signal \n",  baseline_node_id);
						(*iNode)->m_ControlType = m_ControlType_PretimedSignal;
					}

				}
			}



			fprintf(st,"------\n");
			fprintf(st,"Step 2: Read Cycle length and offset,\n");


			// step 3: construct node, movement direction map for reference project


			//step 3: map movement information
			fprintf(st,"------\n");
			fprintf(st,"Step 3: Find mapping movement,\n");


			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{
				int baseline_node_id = (*iNode)->m_NodeID  ;
				if(CrossReferenceNodeInfoMap.find(baseline_node_id) != CrossReferenceNodeInfoMap.end())
				{
					if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
					{

						DTANode* pRefNode = NULL;
						int reference_node_id =  CrossReferenceNodeInfoMap [baseline_node_id].reference_node_id;

						if( pReferenceDoc->m_NodeNumbertoNodeNoMap.find(reference_node_id) !=  pReferenceDoc->m_NodeNumbertoNodeNoMap.end())
						{

							int ReferenceNodeNo = pReferenceDoc->m_NodeNumbertoNodeNoMap[reference_node_id];

							for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap.m_MovementVector .size(); m++)
							{

								DTANodeMovement baseline_movement = (*iNode)->m_MovementDataMap.m_MovementVector[m];

								int MovementIndex = pReferenceDoc->m_NodeNoMap [ReferenceNodeNo] ->FindMovementIndexFromDirecion(baseline_movement.movement_approach_turn );

								if(baseline_movement.movement_approach_turn >=0 && MovementIndex>=0)
								{
									DTANodeMovement* pThisMovement  = &((*iNode)->m_MovementDataMap.m_MovementVector[m]);
									DTANodeMovement reference_movement  =   pReferenceDoc->m_NodeNoMap [ReferenceNodeNo] ->m_MovementDataMap.m_MovementVector[MovementIndex];
									pThisMovement->QEM_TurnVolume = reference_movement.QEM_TurnVolume;


									//we use this function as it is possible th movements in the current network is not fully matched with the synchro network
									pThisMovement->QEM_LinkVolume  =
										pReferenceDoc->m_NodeNoMap [ReferenceNodeNo] ->FindHourlyCountFromDirection(reference_movement.movement_direction);

									pThisMovement->QEM_Lanes = reference_movement.QEM_Lanes;
									pThisMovement->QEM_Shared = reference_movement.QEM_Shared;
									pThisMovement->QEM_Width = reference_movement.QEM_Width;
									pThisMovement->QEM_Storage = reference_movement.QEM_Storage;
									pThisMovement->QEM_StLanes = reference_movement.QEM_StLanes;
									pThisMovement->QEM_Grade = reference_movement.QEM_Grade;
									pThisMovement->QEM_Speed = reference_movement.QEM_Speed;
									pThisMovement->QEM_IdealFlow = reference_movement.QEM_IdealFlow;
									pThisMovement->QEM_LostTime = reference_movement.QEM_LostTime;
									pThisMovement->QEM_Phase1 = reference_movement.QEM_Phase1;
									pThisMovement->QEM_DetectPhase1 = reference_movement.QEM_DetectPhase1;
									pThisMovement->QEM_TurnPercentage = reference_movement.QEM_TurnPercentage;
									pThisMovement->QEM_EffectiveGreen = reference_movement.QEM_EffectiveGreen;
									pThisMovement->QEM_Capacity = reference_movement.QEM_Capacity;
									pThisMovement->QEM_SatFlow = reference_movement.QEM_SatFlow;
									pThisMovement->QEM_VOC = reference_movement.QEM_VOC;
									pThisMovement->QEM_SatFlow = reference_movement.QEM_SatFlow;
									pThisMovement->QEM_Delay = reference_movement.QEM_Delay;

									fprintf(st,"Baseline,Node,%d,Up Node,%d,Dest Node,%d,%s,%s, obtains # of lanes =,%d,shared=,%d,Width=,%d,Storage=,%d\n",  
										baseline_node_id, 
										m_NodeNoMap[pThisMovement-> in_link_from_node_id]->m_NodeID,
										m_NodeNoMap[pThisMovement-> out_link_to_node_id]->m_NodeID,
										GetTurnDirectionString( pThisMovement-> movement_approach_turn),
										GetTurnString( pThisMovement->movement_turn),
										pThisMovement->QEM_Lanes,
										pThisMovement->QEM_Shared,
										pThisMovement->QEM_Width,
										pThisMovement->QEM_Storage);
								}else
								{
									fprintf(st,"Baseline,Node,%d,Up Node,%d,Dest Node,%d,%s,%s,does not find reference movement.\n",  
										baseline_node_id, 
										m_NodeNoMap[(*iNode)->m_MovementDataMap.m_MovementVector[m]. in_link_from_node_id]->m_NodeID,
										m_NodeNoMap[(*iNode)->m_MovementDataMap.m_MovementVector[m]. out_link_to_node_id]->m_NodeID,
										GetTurnDirectionString((*iNode)->m_MovementDataMap.m_MovementVector[m]. movement_approach_turn),
										GetTurnString((*iNode)->m_MovementDataMap.m_MovementVector[m].movement_turn));

								}

							} // for each movement
						}  // with reference node
					}  // signal control

				}
			}


			fclose(st);
		}else
		{
			CString str;
			str.Format("File %s cannot be opened.", str_log_file);
			AfxMessageBox(str);
		}


		OpenCSVFileInExcel(str_log_file);

		CString str;
		str.Format("%d nodes have obtained cycle length from the reference network. To accept the changes, please save the network.",count);
		AfxMessageBox(str, MB_ICONINFORMATION);

	}
}

void CTLiteDoc::OnImportInrixshapefileandspeeddata()
{
}


void CTLiteDoc::OnDemandConvert()
{
	static char BASED_CODE szFilter_demand[] = "Origin-based Static Demand Files (*.txt;*.oba))|*.txt;*.oba||";
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter_demand);
	if(dlg.DoModal() == IDOK)
	{
		ConvertOriginBasedDemandFile(dlg.GetPathName());
	} 

}

void  CTLiteDoc::ConvertOriginBasedDemandFile(LPCTSTR lpszFileName)
{
	CString directory;
	CString PathFile;
	PathFile.Format("%s",lpszFileName);
	directory = PathFile.Left(PathFile.ReverseFind('\\') + 1);


	FILE* st=fopen(lpszFileName,"r");
	FILE* outfile=fopen(directory+"input_demand_converted.csv","w");

	if(outfile==NULL)
	{
		AfxMessageBox("File input_demand.csv cannot be opened to write.");

		return;

	}
	if(st!=NULL)
	{

		fprintf(outfile, "from_zone_id,to_zone_id,number_of_trips_demand_type1\n");

		// Number of matrices and the multiplication factor

		int ODSize = g_read_integer(st);
		float totoal_OD_volume =  g_read_float(st);


		// Destinations

		// There are two formats available

		for(int i = 1; i <= ODSize; i++)
		{
			// Read each demand
			int from_zone_id = g_read_integer(st);

			if(from_zone_id<=0) break;

			if(from_zone_id>i)  //advance
				i= from_zone_id;


			for(int j = 1; j <= ODSize; j++)
			{
				int to_zone_id = g_read_integer_with_special_character(st,true,'O');  // break when reading "Origin"
				if(to_zone_id<=0) break;

				float demand = g_read_float(st);


				if(to_zone_id>j)  //advance
					j= to_zone_id;

				fprintf(outfile,"%d,%d,%.4f\n", from_zone_id, to_zone_id,demand);

			}

		}


		fclose(st);
		fclose(outfile);

		AfxMessageBox("File input_demand.csv has been generated.",MB_ICONINFORMATION    );

	}
}

void CTLiteDoc::IdentifyBottleNeckAndOnOffRamps()
{
	if(m_bIdentifyBottleneckAndOnOffRamps == true)
		return;
	else  // false
		m_bIdentifyBottleneckAndOnOffRamps = true;

	// keep working

	CWaitCursor wait;
	// ! there is an freeway or highway downstream with less number of lanes
	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		DTALink * pLink = (*iLink);
		if( m_LinkTypeMap[pLink->m_link_type].IsFreeway () 
			&&  m_NodeNoMap[pLink->m_ToNodeID ]->m_OutgoingLinkVector.size()==1)  // freeway or highway
		{
			int FromID = pLink->m_FromNodeID;
			int ToID   = pLink->m_ToNodeID;

			for(int i=0; i< m_NodeNoMap[ToID]->m_OutgoingLinkVector.size(); i++)
			{
				DTALink* pNextLink =  m_LinkNoMap[m_NodeNoMap[ToID]->m_OutgoingLinkVector[i]];
				if(m_LinkTypeMap[pNextLink->m_link_type ].IsFreeway () && pNextLink->m_NumberOfLanes  < pLink->m_NumberOfLanes && pNextLink->m_ToNodeID != FromID)
				{
					//					pLink->m_StochaticCapcityFlag = StochasticCapacityFlag;  //lane drop from current link to next link
					//g_LogFile << "lane drop:" << g_NodeVector[pLink->m_FromNodeID].m_NodeID << " ->" << g_NodeVector[pLink->m_ToNodeID].m_NodeID << endl;
				}

			}

		}
	}


	//// merge: one outgoing link, two more incoming links with at least freeway link

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		DTALink * pLink = (*iLink);
		int incoming_link_freeway_and_ramp_count = 0;
		bool no_arterial_incoming_link = true;

		DTANode* pFromNode = m_NodeNoMap[pLink->m_FromNodeID];

		for(int incoming_link = 0; incoming_link <  pFromNode->m_IncomingLinkVector .size(); incoming_link++) // one outgoing link without considering u-turn
		{
			DTALink* pIncomingLink = m_LinkNoMap[pFromNode->m_IncomingLinkVector[incoming_link]];

			if(pIncomingLink!=NULL && (pIncomingLink->m_FromNodeID != pLink->m_ToNodeID)) // non-uturn link
			{
				if(m_LinkTypeMap[pIncomingLink->m_link_type ].IsFreeway() //freeway link
					|| m_LinkTypeMap[pIncomingLink->m_link_type].IsRamp ())
				{
					incoming_link_freeway_and_ramp_count++;

				}else
				{
					no_arterial_incoming_link = false;

				}
			}

		}
		if(incoming_link_freeway_and_ramp_count >=2 && no_arterial_incoming_link)
		{
			TRACE("\nMerge link: %d->%d",pLink->m_FromNodeNumber , pLink->m_ToNodeNumber );
			pLink->m_bMergeFlag = 1;
		}

	}


	//// first count # of incoming freeway, highway or ramp links to each freeway/highway link
	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		DTALink * pLink = (*iLink);
		int FromID = pLink->m_FromNodeID;
		DTANode* pFromNode = m_NodeNoMap[FromID];
		if(pLink->m_bMergeFlag ==1 && pFromNode->m_IncomingLinkVector .size() == 2)  // is a merge bottlebeck link with two incoming links
		{
			int il;
			bool bRampExistFlag = false;
			bool bFreewayExistFlag = false;


			for(il = 0; il< pFromNode->m_IncomingLinkVector.size(); il++)
			{
				if(m_LinkNoMap.find(pFromNode->m_IncomingLinkVector[il]) == m_LinkNoMap.end())
					continue; 

				DTALink* pInLink = m_LinkNoMap[ pFromNode->m_IncomingLinkVector[il]];
				if(m_LinkTypeMap[pInLink->m_link_type].IsRamp ())  // on ramp as incoming link
				{
					bRampExistFlag = true;

					pInLink->m_bOnRampType = true;


					pLink->m_MergeOnrampLinkID = pFromNode->m_IncomingLinkVector[il];
				}
				if(m_LinkTypeMap[pInLink->m_link_type ].IsFreeway () || 
					m_LinkTypeMap[pInLink->m_link_type ].IsHighway ())  // freeway or highway
				{
					bFreewayExistFlag = true;
					pLink->m_MergeMainlineLinkID = pFromNode->m_IncomingLinkVector[il];
				}
				if(bRampExistFlag && bFreewayExistFlag)
				{
					pLink->m_bMergeFlag = 2; // merge with ramp and mainline street
					//g_LogFile << "merge with ramp:" << g_NodeVector[pLink->m_FromNodeID].m_NodeID  << " ->" << g_NodeVector[pLink->m_ToNodeID].m_NodeID ;
					//g_LogFile << " with onramp:" << g_NodeVector[m_LinkNoMap[pLink->m_MergeOnrampLinkID]->m_FromNodeID].m_NodeID  << " ->" << g_NodeVector[m_LinkNoMap[pLink->m_MergeOnrampLinkID]->m_ToNodeID].m_NodeID ;
					//g_LogFile << " and freeway mainline:" << g_NodeVector[m_LinkNoMap[pLink->m_MergeMainlineLinkID]->m_FromNodeID ].m_NodeID << " ->" << g_NodeVector[m_LinkNoMap[pLink->m_MergeMainlineLinkID]->m_ToNodeID].m_NodeID << endl;
					break;
				}

			}


		}

		if(pLink->m_bMergeFlag ==1)
		{
			// merge with several merging ramps
			int ij;
			int TotalNumberOfLanes = 0;
			for( ij= 0; ij< pFromNode->m_IncomingLinkVector.size(); ij++)
			{
				TotalNumberOfLanes += m_LinkNoMap[  pFromNode->m_IncomingLinkVector[ij]]->m_NumberOfLanes  ;
			}


		}

	}

}

