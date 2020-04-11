// TLiteDoc.h : interface of the CTLiteDoc class
//
//  Portions Copyright 2010 Xuesong Zhou (xzhou99@gmail.com), Jinjin Tang ()

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

#pragma warning(disable:4477)  // warning C4995: 'CDaoDatabase': name was marked as #pragma deprecated
#pragma once
#pragma warning(disable:4995)  // warning C4995: 'CDaoDatabase': name was marked as #pragma deprecated

#define _MAX_STRING_SIZE _MAX_PATH
#define _MAX_TRANSIT_GRID_SIZE 5
#define _MAX_TRANSIT_TIME_SIZE 1
#define _MAX_TRANSIT_TIME_STEP_IN_MIN 60
#include "afx.h"

#include "atlimage.h"
#include "math.h"
#include "Network.h"
#include ".\\cross-resolution-model\\SignalNode.h"


#include <iostream>
#include <fstream>


enum _GIS_DATA_TYPE {GIS_Point_Type=0,GIS_Line_Type, GIS_Polygon_Type}; 
enum eSEARCHMODE { efind_node = 0, efind_link, efind_path, efind_Agent, efind_path_node_sequence };
enum layer_mode
{ 
	layer_node = 0,
	layer_link,
	layer_movement,
	layer_zone,
	layer_ODMatrix,
	layer_link_MOE,
	layer_path, 
	layer_grid,
	layer_background_image,
	layer_connector,
	layer_workzone,
	layer_reference_line,
	layer_transit_accessibility,
	layer_transit,
	layer_Agent_position,
	layer_subarea
};
enum Network_Data_Settings {_NODE_DATA = 0,_LINK_DATA, _MOVEMENT_DATA, _AGENT_DATA, MAX_NUM_OF_NETWORK_DATA_FILES};
enum Demand_Data_Settings { _DEMAND_FILE_LIST = 0, _DEMAND_TYPE_DATA, _Agent_TYPE_DATA, MAX_NUM_OF_DEMAND_DATA_FILES };
enum Link_MOE {MOE_none,MOE_volume, MOE_speed, MOE_queue_length,MOE_impact, MOE_bottleneck,MOE_safety,MOE_user_defined,MOE_density,MOE_traveltime,MOE_capacity, MOE_speedlimit, MOE_reliability, MOE_fftt, MOE_length, MOE_queuelength,MOE_fuel,MOE_Agent, MOE_volume_copy, MOE_speed_copy, MOE_density_copy, MOE_emissions, MOE_CO2,MOE_NOX, MOE_CO,MOE_HC};

enum OD_MOE {odnone,critical_volume};

enum Agent_CLASSIFICATION_SELECTION {CLS_network=0, CLS_OD,CLS_link_set,
CLS_sep_c_1,
CLS_path_trip,CLS_path_partial_trip,
CLS_sep_c_2,
CLS_subarea_generated,
CLS_subarea_traversing_through,
CLS_subarea_internal_to_external,
CLS_subarea_external_to_internal,
CLS_subarea_internal_to_internal_trip,
CLS_subarea_internal_to_internal_subtrip,
CLS_subarea_boundary_to_bounary_subtrip};
enum Agent_X_CLASSIFICATION {CLS_all_Agents=0,CLS_demand_type,
CLS_sep_1,
CLS_time_interval_5_min, CLS_time_interval_15_min, CLS_time_interval_30_min, CLS_time_interval_60_min, CLS_time_interval_2_hour, CLS_time_interval_4_hour,
CLS_sep_2,
CLS_distance_bin_0_2,CLS_distance_bin_1,CLS_distance_bin_2,CLS_distance_bin_5,CLS_distance_bin_10,
CLS_sep_3,
CLS_travel_time_bin_2,CLS_travel_time_bin_5,CLS_travel_time_bin_10,CLS_travel_time_bin_30,
CLS_sep_4};
enum Agent_Y_CLASSIFICATION {
	CLS_Agent_count=0,CLS_cumulative_Agent_count,CLS_total_travel_time,CLS_total_travel_distance, 
	CLS_seperator_1,
	CLS_avg_travel_time,
	CLS_avg_travel_distance,
	CLS_avg_speed,
	CLS_avg_travel_time_per_mile,
	CLS_travel_time_Travel_Time_Index,
	CLS_seperator_2,
	CLS_travel_time_STD,
	CLS_travel_time_95_percentile,
	CLS_travel_time_90_percentile,
	CLS_travel_time_80_percentile,
	CLS_travel_time_Planning_Time_Index,
	CLS_travel_time_Buffer_Index,
	CLS_travel_time_Skew_Index,
	CLS_seperator_3,
	CLS_travel_time_per_mile_STD,
	CLS_travel_time_per_mile_95_percentile,
	CLS_travel_time_per_mile_90_percentile,
	CLS_travel_time_per_mile_80_percentile,
};
enum LINK_BAND_WIDTH_MODE {LBW_number_of_lanes = 0, LBW_link_volume,LBW_number_of_marked_Agents, LBW_congestion_duration};


class MovementBezier
{
public: 
	CPoint P0,P1,P2;




	MovementBezier(CPoint p0, CPoint p1, CPoint p2)
	{
		P0 = p0;
		P1 = p1;
		P2 = p2;

	}


	float GetMinDistance(CPoint pt)
	{
		int x1 = P0.x;
		int y1 = P0.y;
		int x2 = P1.x;
		int y2 = P1.y;
		int x3 = P2.x;
		int y3 = P2.y;

		int i;
		float min_distance  = 99999;
		for (i=0; i < 100; ++i)
		{
			double t = (double)i /100.0;
			double a = pow((1.0 - t), 2.0);
			double b = 2.0 * t * (1.0 - t);
			double c = pow(t, 2.0);
			double x = a * x1 + b * x2 + c * x3;
			double y = a * y1 + b * y2 + c * y3;
			min_distance = min(sqrt( (x-pt.x)*(x-pt.x) + (y-pt.y)*(y-pt.y)),min_distance);
		}
		return min_distance;
	}

};
#define _TOTAL_NUMBER_OF_PROJECTS 5


class GridNodeSet
{
public:
	double x;
	double y;

	int x_int;
	int y_int;

	std::vector<int> m_NodeVector;
	std::vector<int> m_LinkNoVector;

	std::vector<float> m_NodeX;
	std::vector<float> m_NodeY;

	//for transit
	std::vector<int> m_TripIDVector;
	std::vector<int> m_StopIDVector;

};

class PathStatistics
{
public: 

	std::vector<float> m_TravelTimeVector;
	std::vector<float> m_TravelTimePerMileVector;

	float GetSimulatedTravelTimeStandardDeviation()
	{
		if( m_TravelTimeVector.size() ==0)
			return 0;

		float total_travel_time = 0 ;
		for(unsigned int i=0; i< m_TravelTimeVector.size(); i++)
		{
		total_travel_time+= m_TravelTimeVector[i];
		}

		float avg_travel_time  = total_travel_time /  m_TravelTimeVector.size();

		float total_variance = 0 ;
		for(unsigned int i=0; i< m_TravelTimeVector.size(); i++)
		{
		total_variance+= (m_TravelTimeVector[i]-avg_travel_time)* (m_TravelTimeVector[i]-avg_travel_time);
		}

		return sqrt(total_variance/m_TravelTimeVector.size());

	}

	float GetSimulatedTravelTimePerMileStandardDeviation()
	{
		if( m_TravelTimePerMileVector.size() ==0)
			return 0;

		float total_travel_time_per_mile = 0 ;
		for(unsigned int i=0; i< m_TravelTimePerMileVector.size(); i++)
		{
		total_travel_time_per_mile+= m_TravelTimePerMileVector[i];
		}

		float avg_travel_time_per_mile  = total_travel_time_per_mile /  m_TravelTimePerMileVector.size();

		float total_variance = 0 ;
		for(unsigned int i=0; i< m_TravelTimePerMileVector.size(); i++)
		{
		total_variance+= (m_TravelTimePerMileVector[i]-avg_travel_time_per_mile)* (m_TravelTimePerMileVector[i]-avg_travel_time_per_mile);
		}

		return sqrt(total_variance/m_TravelTimePerMileVector.size());

	}


	PathStatistics()
	{
		Accessibility_Factor = 0;
		TotalAgentSize = 0;
		TotalTravelTime = 0;
		TotalDistance = 0;
		TotalTravelTimeVariance = 0;
		TotalTravelTimePerMileVariance = 0;

		TotalCost = 0;
		TotalPM= 0;

		for(int i = 0; i< _TOTAL_NUMBER_OF_PROJECTS; i++)
		{
		 TotalAgentSizeVector[i] = 0;
		 TotalTravelTimeVector[i] = 0;
		 TotalDistanceVector[i] = 0;
		 TotalCostVector[i] = 0;
		}
	}

	float GetAvgTravelTime()
	{
		return TotalTravelTime/max(1,TotalAgentSize);
	}

	int	  Origin;
	int	  Destination;
	int   NodeNumberSum;
	int   NodeSize;



	CString GetPathLabel()
	{
		CString label;
		label.Format("%d,%d,%d,%d", Origin , Destination, NodeNumberSum, NodeSize);
		return label;
	}

	std::vector<int> m_LinkVector;
	std::vector<int> m_NodeVector;
	std::vector<DTALink*> m_LinkPointerVector;  // used when generating physical side streets from centroids
	std::vector<GDPoint> m_ShapePoints;
	std::vector<DTAAgent*> m_AgentVector;


	int date_id;
	float departure_time_in_min;

	float Accessibility_Factor;
	int   TotalAgentSize;
	float TotalTravelTime;
	float	TotalTravelTimeVariance;
	float	TotalTravelTimePerMileVariance;

	float TotalDistance;
	float TotalCost;
	float TotalPM;


	int   TotalAgentSizeVector[_TOTAL_NUMBER_OF_PROJECTS];
	float TotalTravelTimeVector[_TOTAL_NUMBER_OF_PROJECTS];
	float TotalDistanceVector[_TOTAL_NUMBER_OF_PROJECTS];
	float TotalCostVector[_TOTAL_NUMBER_OF_PROJECTS];

};

class Movement3Node
{
public:
	int TotalAgentSize;
	int Phase1;
	int PermPhase1;
	int DetectPhase1;
	int EffectiveGreen;
	int Capacity;
	float VOC;
	float Delay;
	char LOS;
	float DischargeRate;




	Movement3Node()
	{
		TotalAgentSize  = 0;
		Phase1 = -1;  // default value
		PermPhase1 = -1;
		DetectPhase1 = -1;

		EffectiveGreen = 0;
		Capacity = 0;
		VOC = 0;
		Delay = 0;
		LOS = 'A';
		DischargeRate = 0;
	}

};

class CTLiteDoc : public CDocument
{
public: // create from serialization only

	float m_DemandAlpha;
	eLinkDataType m_PrimaryDataSource;
	float*** TDDemandSOVMatrix;
	float*** TDDemandHOVMatrix;
	float*** TDDemandTruckMatrix;

	void CreateAgents(int origin_zone, int destination_zone, float number_of_Agents, 
		int demand_type, float starting_time_in_min, float ending_time_in_min);

	std::string m_CurrentDisplayTimingPlanName;
	
	std::vector<DTATimingPlan> m_TimingPlanVector;

	DTATimingPlan GetTimingPlanInfo(std::string timing_plan_name)
	{ 
		DTATimingPlan element;

		for(int tp = 0 ; tp < m_TimingPlanVector.size(); tp++)
		{
			if(m_TimingPlanVector[tp].timing_plan_name == timing_plan_name)
				return m_TimingPlanVector[tp];
			
		}
	
		return element;
	}

	int GetTimingPlanNo(std::string timing_plan_name)
	{ 
		for(int tp = 0 ; tp < m_TimingPlanVector.size(); tp++)
		{
			if(m_TimingPlanVector[tp].timing_plan_name == timing_plan_name)
				return tp;
			
		}
	
		return 0;
	}

	DTATimingPlan FindTimingPlanFromStartTime(int Agent_time_in_min)
	{

		DTATimingPlan element;

		element.start_time_in_min = m_DemandLoadingStartTimeInMin;
		element.end_time_in_min = m_DemandLoadingEndTimeInMin;

		for(int i = 0; i< m_TimingPlanVector.size(); i++)
		{
			if(element.timing_plan_name !="0" && Agent_time_in_min >= m_TimingPlanVector[i].start_time_in_min && 
				Agent_time_in_min <= m_TimingPlanVector[i].end_time_in_min )  
				{
					
					element = m_TimingPlanVector[i] ;

				}
		}

		if(element.timing_plan_name .size() ==0)
			element.timing_plan_name = "0";
		return element;
	
	}

	CString GetPhasingMapKey(int NodeID, std::string TimingPlanName)
	{

		if(TimingPlanName.size() == 0)
			TimingPlanName = "0";

	CString str;
	str.Format("%d:%s", NodeID, TimingPlanName.c_str ());

	return str;
	
	}

	bool AddNameIntoTimingPlanVector(int start_time_in_min, int end_time_in_min, std::string Name)
	{
		bool bDefined = false;

		for(int i = 0; i< m_TimingPlanVector.size(); i++)
		{
			if(m_TimingPlanVector[i].timing_plan_name.compare (Name) ==0 )  //exist
			{
				bDefined =true;

				if(end_time_in_min > m_TimingPlanVector[i].end_time_in_min )  // update end_time in min
				{
				m_TimingPlanVector[i].end_time_in_min  = end_time_in_min;

				}

				if(start_time_in_min < m_TimingPlanVector[i].start_time_in_min )  // update end_time in min
				{
				m_TimingPlanVector[i].end_time_in_min  = start_time_in_min;
				
				}


				return false;
		
			}
		}
	
		if(!bDefined)
		{

			DTATimingPlan element;
			element.timing_plan_name = Name;
			element.start_time_in_min = start_time_in_min;
			element.end_time_in_min = end_time_in_min;
			m_TimingPlanVector.push_back (element);
		}

		return true;
	}

	void ScanAMSTimingPlanCSVFile(LPCTSTR lpszFileName, int scenario_no);


	bool m_hide_non_specified_movement_on_freeway_and_ramp;

	GridNodeSet*** m_GridMatrix;

	float m_PeakHourFactor;
	CTLiteDoc();



	DECLARE_DYNCREATE(CTLiteDoc)

	// Attributes
public:

	std::vector<int> m_OD_data_vector;

	bool m_bIdentifyBottleneckAndOnOffRamps;
	void IdentifyBottleNeckAndOnOffRamps();
	void Modify(BOOL bModified=true)
{

   SetModifiedFlag(bModified);


   CString string_title = GetTitle();

   if(IsModified()&& string_title.Find(" *") == -1)
   {
      string_title += " *";
   }
   else if(!IsModified() && string_title.Find(" *") != -1)
   {
      string_title.Replace(" *", "");
   }

   SetTitle(string_title);
}

	bool m_bUseMileVsKMFlag;
	int m_bRightHandTrafficFlag;
	double m_ScreenWidth_InMile;
	CString m_LatLongA;
	CString m_LatLongB;


	//	std::vector<DTA_demand> m_ImportedDemandVector;
	std::map<CString, float> m_DemandMatrixMap;

	CString GetODDemandKey(int FileNo, int OriginZone, int DestinationZone)
	{
		CString str;
		str.Format("%d:%d:%d",FileNo,OriginZone,DestinationZone );

		return str;
	}

	void ParseODDemandKey(CString Key, int& FileNo, int& OriginZone, int& DestinationZone)
	{
		scanf(Key,"%d:%d:%d",Key,FileNo,OriginZone,DestinationZone );
	}

	float GetODDemandValue(int FileNo, int OriginZone, int DestinationZone)
	{
		CString Key = GetODDemandKey(FileNo,OriginZone,DestinationZone);
		if(m_DemandMatrixMap.find (Key)!= m_DemandMatrixMap.end())
			return m_DemandMatrixMap[Key];
		else
			return 0;
	
	}

	void SetODDemandValue(int FileNo, int OriginZone, int DestinationZone, float Value)
	{
		CString Key = GetODDemandKey(FileNo,OriginZone,DestinationZone);
			m_DemandMatrixMap[Key] = Value;
	}


	bool m_bSummaryDialog;
	int m_DocumentNo;
	int m_calibration_data_start_time_in_min;
	int m_calibration_data_end_time_in_min;

	double m_GridXStep;
	double m_GridYStep;

	bool m_bRunCrashPredictionModel;
	
	GDRect m_GridRect;

	std::map<int,int> m_AccessibleTripIDMap;

	double m_max_walking_distance;
	double m_max_accessible_transit_time_in_min;


	int FindClosestNode(double x, double y, double min_distance = 99999, int step_size = 1,double time_stamp_in_min = 9999);
	int FindClosestZone(double x, double y, double min_distance = 99999, int step_size = 1);


	std::map <int, int> m_ZoneIDToNodeNoMap;
	std::map <int, int> m_ZoneNotoZoneIDMap;
	std::map <int, int> m_ZoneIDtoZoneNoMap;


	AgentStatistics*** m_ODMOEMatrix;

	int m_DemandTypeSize; 

	int m_PreviousDemandTypeSize;
	int m_PreviousZoneNoSize;


	void ResetZoneIDVector();
	void ResetODMOEMatrix();

	void ShowTextLabel();

	void BuildGridSystem();

	int FindClosestNode(GDPoint point)
	{

		int SelectedNodeID = -1;

		double min_distance = 99999;
		std::list<DTANode*>::iterator iNode;

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			double cx = point.x - (*iNode)->pt.x;
			double cy = point.y - (*iNode)->pt.y;

			double distance = pow((cx*cx + cy*cy),0.5);
			if( distance < min_distance)
			{
				SelectedNodeID = (*iNode)->m_NodeNo ;
				min_distance = distance;
			}

		}

		return SelectedNodeID;

	}



	int m_OriginOnBottomFlag;
	bool m_LongLatFlag;
	int m_StartNodeNumberForNewNodes;
	bool m_bShowCalibrationResults;

	int m_TrafficFlowModelFlag;
	COLORREF m_colorLOS[MAX_LOS_SIZE];

	COLORREF m_ColorDirectionVector[DTA_MAX_Direction];

	int m_ColorDirection;


	float m_LOSBound[40][MAX_LOS_SIZE];
	bool m_bShowLegend;
	bool m_bShowPathList;
	float m_NodeDisplaySize;
	float m_BottleneckDisplaySize;

	bool m_bLoadMovementData;

	float m_AgentDisplaySize;
	float m_NodeTextDisplayRatio;

	GDPoint m_Origin;

	int NPtoSP_X(GDPoint net_point,double Resolution) // convert network coordinate to screen coordinate
	{
		return int((net_point.x-m_Origin.x)*Resolution+0.5);
	}
	int NPtoSP_Y(GDPoint net_point,double Resolution) // convert network coordinate to screen coordinate
	{
		return int((net_point.y-m_Origin.y)*Resolution+0.5);

	}

	void ChangeNetworkCoordinates(int LayerNo, float XScale, float YScale, float deltaX_ratio, float deltaY_ratio);

	void HighlightPath(	std::vector<int>	m_LinkVector, int DisplayID);

	std::vector<GDPoint> m_HighlightGDPointVector;

	void HighlightSelectedAgents(bool bSelectionFlag);
	int m_CurrentViewID;
	bool m_bSetView;
	GDPoint m_Doc_Origin;
	float m_Doc_Resolution;

	int m_ODSize;
	int m_PreviousODSize;
	int m_ZoneNoSize;
	Link_MOE m_LinkMOEMode;
	Link_MOE m_PrevLinkMOEMode;

	OD_MOE m_ODMOEMode;

	std::vector<DTAPath>	m_PathDisplayList;
	bool m_PathMOEDlgShowFlag;
	int m_SelectPathNo;

	int m_FromNodeID;
	int m_ToNodeID;

	double m_UnitDistance;
	double m_OffsetInDistance;

	bool m_bLoadNetworkDataOnly;

	COLORREF m_ColorFreeway, m_ColorHighway, m_ColorArterial;

	ofstream m_AMSLogFile;

	BOOL OnOpenDocument(CString FileName, bool bLoadNetworkOnly =false);
	void FieldNameNotExistMessage(CString FieldName, CString KeyName, CString FileName);
	void FieldNameNotExistMessage(std::string FieldName, std::string KeyName, std::string FileName);
	BOOL OnOpenAMSDocument(CString FileName);
	bool RunGravityModel();
	bool ReadDemandMatrixFile(LPCTSTR lpszFileName,int demand_type);
	BOOL OnOpenTrafficNetworkDocument(CString ProjectFileName, bool bNetworkOnly = false, bool bImportShapeFiles = false);
	
	bool ReadGPSData(string FileName);
	int m_YCorridonateFlag;
	bool m_bGPSDataSet;
	bool m_bEmissionDataAvailable;

	
	// Open Graph drawing framework 
	void OGDF_WriteGraph(CString FileName);


	std::ofstream m_NEXTALOGFile;

	void OpenWarningLogFile(CString directory);
	// two basic input
	bool ReadNodeCSVFile(LPCTSTR lpszFileName, int LayerNo=0);   // for road network
	bool ReadLinkCSVFile(LPCTSTR lpszFileName, bool bCreateNewNodeFlag, int LayerNo);   // for road network
	bool ReadSensorCSVFile(LPCTSTR lpszFileName);   // for road network
	
	bool ReadGPSCSVFile(LPCTSTR lpszFileName);   // for road network
	bool ReadGPSDataFile(LPCTSTR lpszFileName);   // for road network
	
	float m_MovementTextBoxSizeInDistance;

	bool m_bShowSignalNodeMovementOnly;


	std::vector <int> m_LinkIDRecordVector;  // used to record if a unique link id has been used;
	int FindUniqueLinkID();

	
	void OffsetLink();
	bool m_bBezierCurveFlag;


	void GenerateOffsetLinkBand();

	void ReCalculateLinkBandWidth();
	float GetLinkBandWidth(float Value);
	bool ReadDemandCSVFile(LPCTSTR lpszFileName);   // for road network
	bool ReadMetaDemandCSVFile(LPCTSTR lpszFileName);   // for road network
	bool ReadMetaSignalCSVFile(LPCTSTR lpszFileName);   // for road network
	bool ReadScenarioSettingCSVFile(LPCTSTR lpszFileName);   // for road network
	bool WriteScenarioSettingCSVFile(LPCTSTR lpszFileName);

	   
	bool ReadDemandTypeCSVFile(LPCTSTR lpszFileName);
	bool ReadLinkTypeCSVFile(LPCTSTR lpszFileName); 

	//scenario data

	int m_number_of_iterations;
	int m_number_of_reporting_days;
	int m_traffic_flow_model;
	int m_signal_reresentation_model;
	int m_traffic_analysis_method;

	int m_ODME_mode;
	float m_demand_multiplier;

	int m_NumberOfScenarioSettings;

	//

	int m_DemandLoadingStartTimeInMin;
	int m_DemandLoadingEndTimeInMin;



	// structure for demand file

	struct DemandRecordData
	{
		INT32 origin_zone, destination_zone;
		INT32 starting_time_in_min;
		INT32 ending_time_in_min;
		float number_of_Agents[5] ;
	};


	int ReadWorkZoneScenarioData(int RemoveLinkFromNodeNumber= -1, int RemoveLinkToNodeNumber= -1);

	bool WriteWorkZoneScenarioData();

	bool ReadZoneShapeCSVFile(LPCTSTR lpszFileName); 

	bool Read3ColumnTripTxtFile(LPCTSTR lpszFileName);  

	// additional input
	void LoadSimulationOutput();
	void LoadGPSData();
	bool ReadSensorTrajectoryData(LPCTSTR lpszFileName);
	bool ReadSimulationLinkMOEData_Parser(LPCTSTR lpszFileName);

	int m_SimulationStartTime_in_min;
	int m_SimulationEndTime_in_min;


	std::string CString2StdString(CString str)
	{	 // Convert a TCHAR string to a LPCSTR
	  CT2CA pszConvertedAnsiString (str);

	  // construct a std::string using the LPCSTR input
	  std::string strStd (pszConvertedAnsiString);

	  return strStd;
	  }


	CString m_lpstrInitialDir; 
	std::vector <CString> m_MessageStringVector;

	CString m_NodeDataLoadingStatus;
	CString m_SignalDataLoadingStatus;
	CString m_LinkDataLoadingStatus;
	CString m_DemandDataLoadingStatus;
	CString m_ScenarioDataLoadingStatus;

	CString m_BackgroundImageFileLoadingStatus;

	CString m_SimulationLinkTDMOEDataLoadingStatus;
	bool m_bSimulationDataLoaded;
	CString m_SimulationAgentDataLoadingStatus;
	CString m_PathDataLoadingStatus;
	CString m_MovementDataLoadingStatus;

	CString m_SensorLocationLoadingStatus;

	CString m_AgentLocationLoadingStatus;

	CString m_StrLoadingTime;


	DTALink* FindLinkFromSensorLocation(float x, float y, CString orientation);

	DTALink* FindLinkFromCoordinateLocation(float x1, float y1, float x2, float y2, float min_distance_in_mile);


	int GetVehilePosition(DTAAgent* pAgent, double CurrentTime, int &link_sequence_no, float& ratio );
	bool GetAgentPosition(string agent_id, double CurrentTime, GDPoint& pt);
	bool GetGPSVehilePosition(DTAAgent* pAgent, double CurrentTime, GDPoint & pt);

	float GetLinkMOE(DTALink* pLink, Link_MOE LinkMOEMode, int CurrentTime,  int AggregationIntervalInMin, float &value);

	CString GetTurnString(DTA_Turn turn)
	{
		CString str;
		switch (turn)
		{
		case DTA_LeftTurn:  str.Format("Left"); break;
		case DTA_Through:  str.Format("Through"); break;
		case DTA_RightTurn:  str.Format("Right"); break;
		default :  str.Format("Other"); break;
		}

		return str;
	}

	CString GetTurnShortString(DTA_Turn turn)
	{
		CString str;
		switch (turn)
		{
		case DTA_LeftTurn:  str.Format("L"); break;
		case DTA_Through:  str.Format("T"); break;
		case DTA_RightTurn:  str.Format("R"); break;
		default:  str.Format("O"); break;
		}

		return str;
	}

	DTA_SIG_MOVEMENT GetTurnDirectionFromString(CString str) 
	{
		if(m_TurnDirectionStringMap.find(str) != m_TurnDirectionStringMap.end())
		{
			return m_TurnDirectionStringMap[str];
		}
			return DTA_LANES_COLUME_init;
	}
	std::map<CString, DTA_SIG_MOVEMENT> m_TurnDirectionStringMap;
	CString GetTurnDirectionString(DTA_SIG_MOVEMENT turn_dir)
	{
		CString str;
		switch (turn_dir)
		{
		case DTA_LANES_COLUME_init: str.Format("N/A"); break;
		case DTA_NBL2: str.Format("NBL2"); break;
		case DTA_NBL: str.Format("NBL"); break;
		case DTA_NBT: str.Format("NBT"); break;
		case DTA_NBR: str.Format("NBR"); break;
		case DTA_NBR2: str.Format("NBR2"); break;
		case DTA_SBL2: str.Format("SBL2"); break;
		case DTA_SBL: str.Format("SBL"); break;
		case DTA_SBT: str.Format("SBT"); break;
		case DTA_SBR: str.Format("SBR"); break;
		case DTA_SBR2: str.Format("SBR2"); break;
		case DTA_EBL2: str.Format("EBL2"); break;
		case DTA_EBL: str.Format("EBL"); break;
		case DTA_EBT: str.Format("EBT"); break;
		case DTA_EBR: str.Format("EBR"); break;
		case DTA_EBR2: str.Format("EBR2"); break;
		case DTA_WBL2: str.Format("WBL2"); break;
		case DTA_WBL: str.Format("WBL"); break;
		case DTA_WBT: str.Format("WBT"); break;
		case DTA_WBR: str.Format("WBR"); break;
		case DTA_WBR2: str.Format("WBR2"); break;
		case DTA_NEL: str.Format("NEL"); break;
		case DTA_NET: str.Format("NET"); break;
		case DTA_NER: str.Format("NER"); break;
		case DTA_NWL: str.Format("NWL"); break;
		case DTA_NWT: str.Format("NWT"); break;
		case DTA_NWR: str.Format("NWR"); break;
		case DTA_SEL: str.Format("SEL"); break;
		case DTA_SET: str.Format("SET"); break;
		case DTA_SER: str.Format("SER"); break;
		case DTA_SWL: str.Format("SWL"); break;
		case DTA_SWT: str.Format("SWT"); break;
		case DTA_SWR: str.Format("SWR"); break;

		default :  str.Format("N/A");
		}

		return str;
	}

	int GetNEMAPhase_from_TurnDirectionString(DTA_SIG_MOVEMENT turn_dir)
	{
		int NEMA_phase_no = 0;
		switch (turn_dir)
		{
		case DTA_LANES_COLUME_init: NEMA_phase_no=0; break;
		case DTA_NBL2: NEMA_phase_no = 3; break;
		case DTA_NBL: NEMA_phase_no = 3; break;
		case DTA_NBT: NEMA_phase_no = 8; break;
		case DTA_SBL2: NEMA_phase_no = 7; break;
		case DTA_SBL: NEMA_phase_no = 7; break;
		case DTA_SBT: NEMA_phase_no = 4; break;
		case DTA_EBL2: NEMA_phase_no = 5; break;
		case DTA_EBL: NEMA_phase_no = 5; break;
		case DTA_EBT: NEMA_phase_no = 2; break;
		case DTA_WBL2: NEMA_phase_no = 1; break;
		case DTA_WBL: NEMA_phase_no = 1; break;
		case DTA_WBT: NEMA_phase_no = 6; break;
		case DTA_NEL: NEMA_phase_no = 3; break;
		case DTA_NET: NEMA_phase_no = 8; break;
		case DTA_NWL: NEMA_phase_no = 1; break;
		case DTA_NWT: NEMA_phase_no = 6; break;
		case DTA_SEL: NEMA_phase_no = 5; break;
		case DTA_SET: NEMA_phase_no = 2; break;
		case DTA_SWL: NEMA_phase_no = 7; break;
		case DTA_SWT: NEMA_phase_no = 4; break;

		default:  NEMA_phase_no = 0;
		}

		return NEMA_phase_no;
	}


	int GetLOSCode(float Value)
	{

		int MOE_checking_index  = m_LinkMOEMode;
				
			
		for(int los = 1; los < MAX_LOS_SIZE-1; los++)
		{
			if( (m_LOSBound[MOE_checking_index][los] <= Value && Value < m_LOSBound[MOE_checking_index][los+1]) ||
				(m_LOSBound[MOE_checking_index][los] >= Value && Value > m_LOSBound[MOE_checking_index][los+1]))

				return los;
		}

		if(m_LinkMOEMode != MOE_speed )
		{

		if(Value < m_LOSBound[MOE_checking_index][1])
			return 1;

		if(Value > m_LOSBound[MOE_checking_index][MAX_LOS_SIZE-2])
			return MAX_LOS_SIZE-2;
		}else
		{ // m_LinkMOEMode != MOE_speed, reverse legend
		if(Value > m_LOSBound[MOE_checking_index][1])
			return 1;

		if(Value < m_LOSBound[MOE_checking_index][MAX_LOS_SIZE-2])
			return MAX_LOS_SIZE-2;
		
		}
		return 1;

	}

public:

	void SetStatusText(CString StatusText);

	std::list<DTANode*>		m_NodeSet;
	std::list<DTALink*>		m_LinkSet;

	std::vector<DTANode*>		m_SubareaNodeSet;
	std::list<DTALink*>		m_SubareaLinkSet;
	std::map<int, DTAZone>	m_ZoneMap;



	std::list<DTAPoint*>	m_DTAPointSet;
	std::list<DTALine*>		m_DTALineSet;



	bool m_bSaveProjectFromSubareaCut;
	bool m_bSaveProjectFromImporting;



	int m_ActivityLocationCount;
	std::vector<CString> m_DemandFileVector;
	int m_CriticalOriginZone;
	int m_CriticalDestinationZone;

	int GetZoneID(GDPoint pt)
	{
		// for all zones
		std::map<int, DTAZone>	:: const_iterator itr;

		for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
		{
			DTAZone Zone = itr->second;

			if(Zone.IsInside (pt))
				return itr->first;
		}

		return -1;
	}

	int GetZoneIDFromShapePoints(GDPoint pt)
	{
		// for all zones
		std::map<int, DTAZone>	:: const_iterator itr;
		double min_distance = 999999;
		int ZoneID = -1;
		for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
		{
			DTAZone Zone = itr->second;

			float distance = g_CalculateP2PDistanceInMileFromLatitudeLongitude(pt, Zone.GetCenter());  // go through each GPS location point
			if(distance < min_distance)
			{
				min_distance = distance;
				ZoneID = itr->first;
			}
		}

		return ZoneID;
	}

	std::list<DTAAgent*>	m_AgentSet;
	std::map<long, DTAAgent*> m_AgentIDMap;

	std::list<DTAAgent*>	m_ProbeSet;
	std::map<long, DTAAgent*> m_ProbeMap;

	std::map<long, CAVISensorPair> m_AVISensorMap;

	
	std::map<int, DTANode*> m_NodeNoMap;
	std::map<int, DTANode*> m_NodeNumberMap;
	std::map<long, DTALink*> m_LinkNoMap;
	std::map<unsigned long, DTALink*> m_NodeNotoLinkMap;
	std::map<__int64, DTALink*> m_NodeNumbertoLinkMap;
	std::map<long, DTALink*> m_LinkNotoLinkMap;
	std::map<string, DTALink*> m_LinkIDtoLinkMap;


	class NetworkState
	{
	public:
	std::map<int, DTANode*> l_NodeNoMap;
	std::map<int, DTANode*> l_NodeNumberMap;
	std::map<long, DTALink*> l_LinkNoMap;
	std::map<unsigned long, DTALink*> l_NodeNotoLinkMap;
	std::map<__int64, DTALink*> l_NodeNumbertoLinkMap;
	std::map<long, DTALink*> l_LinkNotoLinkMap;
	std::map<string, DTALink*> l_LinkIDtoLinkMap;

	std::list<DTANode*>		l_NodeSet;
	std::list<DTALink*>		l_LinkSet;

	std::vector<DTANode*>	l_SubareaNodeSet;
	std::list<DTALink*>		l_SubareaLinkSet;
	std::map<int, DTAZone>	l_ZoneMap;

	std::map<long, long> l_NodeNumbertoNodeNoMap;
	std::map<long, long> l_NodeNotoZoneNameMap;

	std::vector<GDPoint> l_SubareaShapePoints;

	};




	std::vector <NetworkState> m_NetworkState;
	std::vector <NetworkState> m_RedoNetworkState;

	void PushBackNetworkState();
	void  Undo();
	void Redo();

	void GenerateMovementShapePoints();

	std::map<long, DTALink*> m_SensorIDtoLinkMap;
	std::map<long, long> m_AVISensorIDtoNodeIDMap;



	std::map<int, DTANode*> m_SubareaNodeIDMap;
	bool CTLiteDoc::WriteSubareaFiles();


	bool m_EmissionDataFlag;

	int m_AdjLinkSize;

	DTANetworkForSP* m_pNetwork;
	DTANetworkForSP m_Network;

	float m_RandomRoutingCoefficient;

	std::vector<int> m_IntermediateDestinationVector;

	int Routing(bool bCheckConnectivity, bool bRebuildNetwork = true);
	int AlternativeRouting(int NumberOfRoutes);

	CString GetWorkspaceTitleName(CString strFullPath);
	CString GetLocalFileName(CString strFullPath);
	CString m_ProjectTitle;

	void AdjustCoordinateUnitToMile();

	
	bool WriteSelectAgentDataToCSVFile(LPCTSTR lpszFileName, std::vector<DTAAgent*> AgentVector);
	void ReadAgentCSVFile_Parser(LPCTSTR lpszFileName);

	int ReadAMSMovementCSVFile(LPCTSTR lpszFileName, int NodeID);
	int ReadAMSSignalControlCSVFile(LPCTSTR lpszFileName);

	bool ReadAgentBinFile(LPCTSTR lpszFileName,int version_number);
	bool ReadAgentCSVFile(LPCTSTR lpszFileName, int version_number);
	

	void UpdateMovementDataFromAgentTrajector();
	vector<int> ParseLineToIntegers(string line)
	{
		vector<int> SeperatedIntegers;
		string subStr;
		istringstream ss(line);


		char Delimiter = ';';


		while (std::getline(ss, subStr, Delimiter))
		{
			int integer = atoi(subStr.c_str());
			SeperatedIntegers.push_back(integer);
		}
		return SeperatedIntegers;
	}
	vector<float>ParseLineToFloat(string line)
	{
		vector<float> SeperatedIntegers;
		string subStr;
		istringstream ss(line);


		char Delimiter = ';';


		while (std::getline(ss, subStr, Delimiter))
		{
			float value = atof(subStr.c_str());
			SeperatedIntegers.push_back(value);
		}
		return SeperatedIntegers;
	}
	std::map<long,AgentLocationTimeIndexedMap> m_AgentLocationMap;
	std::map<string,AgentLocationTimeIndexedMap> m_AgentWithLocationVectorMap;

	void AddLocationRecord(AgentLocationRecord element)
	{

		m_AgentLocationMap[element.time_stamp_in_second].AgentLocationRecordVector.push_back(element);


		m_AgentWithLocationVectorMap[element.agent_id ].AgentLocationRecordVector.push_back(element);
	
	}

	bool ReadModelAgentTrajectory(LPCTSTR lpszFileName);
	bool ReadGPSTrajectory(LPCTSTR lpszFileName);

	std::vector<string > m_PassengerIDStringVector;
	std::vector<string > m_AgentIDStringVector;


	
	CString GetDemandTypeStr(int PricingType)
	{

		if(PricingType == 1) return "SOV";
		if(PricingType == 2) return "HOV";
		if(PricingType == 3) return "truck";

		return "NULL";
	}

	Agent_CLASSIFICATION_SELECTION m_AgentSelectionMode;
	LINK_BAND_WIDTH_MODE m_LinkBandWidthMode;
	float m_MaxLinkWidthAsNumberOfLanes;
	float m_MaxLinkWidthAsLinkVolume;

	std::map<int, AgentStatistics> m_ClassificationTable;

	bool SelectAgentForAnalysis(DTAAgent* pAgent, Agent_CLASSIFICATION_SELECTION Agent_selection);
	void MarkLinksInSubarea();
	void GenerateClassificationForDisplay(Agent_X_CLASSIFICATION x_classfication, Agent_Y_CLASSIFICATION y_classfication);

	CString FindClassificationLabel(Agent_X_CLASSIFICATION x_classfication, int index);
	int FindClassificationNo(DTAAgent* pAgent, Agent_X_CLASSIFICATION x_classfication);
	void GenerateAgentClassificationData(Agent_CLASSIFICATION_SELECTION Agent_selection, Agent_X_CLASSIFICATION x_classfication);

	bool ReadBackgroundImageFile(LPCTSTR lpszFileName, bool bAskForInput = false);
	int m_PathNodeVectorSP[MAX_NODE_SIZE_IN_A_PATH];
	long m_NodeSizeSP;

	std::map<long, long> m_NodeNotoNumberMap;
	std::map<long, long> m_NodeNumbertoNodeNoMap;
	std::map<long, long> m_NodeNotoZoneNameMap;

	int m_SelectedLinkNo;
	bool m_ZoomToSelectedObject;
	void ZoomToSelectedLink(int SelectedLinkNo);
	void ZoomToSelectedNode(int SelectedNodeNumber);

	int m_SelectedNodeID;
	int m_SelectedZoneID;
	int m_SelectedAgentID;
	string m_SelectedTrainHeader;


	std::vector<DTADemandType> m_DemandTypeVector;
	std::map<int,DTALinkType> m_LinkTypeMap;
	std::map<int, string> m_NodeTypeMap;

	int m_ControlType_UnknownControl;
	int m_ControlType_NoControl;
	int m_ControlType_ExternalNode;
	int m_ControlType_YieldSign;
	int m_ControlType_2wayStopSign;
	int m_ControlType_4wayStopSign;
	int m_ControlType_PretimedSignal;
	int m_ControlType_ActuatedSignal;
	int m_ControlType_Roundabout;

	int m_LinkTypeFreeway;
	int m_LinkTypeArterial;
	int m_LinkTypeHighway;





	CString m_ProjectDirectory;
	

	GDRect m_NetworkRect;

	float m_DefaultNumLanes;
	float m_DefaultSpeedLimit;
	float m_DefaultCapacity;
	float m_DefaultLinkType;

	bool m_bLinkToBeShifted;

	int m_LaneWidthInMeter;

	void ShowLegend(bool ShowLegendStatus);
	DTALink* AddNewLinkWithNodeNumbers(int FromNodeNumber, int ToNodeNumber, bool bOffset = false, bool bLongLatFlag = false)	
	{
		int FromNodeID =  -1;
			if (m_NodeNumberMap.find(FromNodeNumber)!= m_NodeNumberMap.end())
			{
			FromNodeID = m_NodeNumberMap[FromNodeNumber]->m_NodeNo ;
			}

		int ToNodeID = -1;
			if (m_NodeNumberMap.find(ToNodeNumber)!= m_NodeNumberMap.end())
			{
			ToNodeID = m_NodeNumberMap[ToNodeNumber]->m_NodeNo ;
			}

			if(FromNodeID>=0 && ToNodeID>=0)
			{
				return AddNewLink(FromNodeID,ToNodeID,bOffset,bLongLatFlag);
			}else

				return NULL;
			 
	}

		DTALink* AddNewLink(int FromNodeID, int ToNodeID, bool bOffset = false, bool bLongLatFlag = false)
	{


		Modify();
		DTALink* pLink = 0;

		pLink = FindLinkWithNodeIDs(FromNodeID,ToNodeID);

			if(pLink != NULL)
				return NULL;  // a link with the same from and to node numbers exists!

		pLink = new DTALink(1);
		pLink->m_LinkNo = (int)(m_LinkSet.size());
		pLink->m_FromNodeNumber = m_NodeNotoNumberMap[FromNodeID];
		pLink->m_ToNodeNumber = m_NodeNotoNumberMap[ToNodeID];
		pLink->m_FromNodeID = FromNodeID;
		pLink->m_ToNodeID= ToNodeID;

		pLink->m_FromPoint = m_NodeNoMap[pLink->m_FromNodeID]->pt;
		pLink->m_ToPoint = m_NodeNoMap[pLink->m_ToNodeID]->pt;

		m_NodeNoMap[FromNodeID ]->m_Connections+=1;

		m_NodeNoMap[FromNodeID ]->m_OutgoingLinkVector.push_back(pLink->m_LinkNo);
		m_NodeNoMap[ToNodeID]->m_IncomingLinkVector.push_back(pLink->m_LinkNo);

		m_NodeNoMap[ToNodeID ]->m_Connections+=1;

		if( m_LinkTypeMap[pLink->m_link_type].IsFreeway () ||  m_LinkTypeMap[pLink->m_link_type].IsRamp  ())
		{
		m_NodeNoMap[pLink->m_FromNodeID ]->m_bConnectedToFreewayORRamp = true;
		m_NodeNoMap[pLink->m_ToNodeID ]->m_bConnectedToFreewayORRamp = true;
		}


		unsigned long LinkKey = GetLinkKey( pLink->m_FromNodeID, pLink->m_ToNodeID);
		m_NodeNotoLinkMap[LinkKey] = pLink;

		__int64  LinkKey2 = GetLink64Key(pLink-> m_FromNodeNumber,pLink->m_ToNodeNumber);
		m_NodeNumbertoLinkMap[LinkKey2] = pLink;

		pLink->m_NumberOfLanes= m_DefaultNumLanes;
		pLink->m_FreeSpeed= m_DefaultSpeedLimit;
		pLink->m_SpeedAtCapacity = m_DefaultSpeedLimit - 20;
		
		pLink->m_ReversedSpeedLimit = m_DefaultSpeedLimit;
		pLink->m_avg_simulated_speed = m_DefaultSpeedLimit;

		double length  = pLink->DefaultDistance()/max(0.0000001,m_UnitDistance);

		if(bLongLatFlag || m_LongLatFlag)
		{  // bLongLatFlag is user input,  m_LongLatFlag is the system input from the project file 
			length  =  g_CalculateP2PDistanceInMileFromLatitudeLongitude(pLink->m_FromPoint , pLink->m_ToPoint);
			m_UnitDistance = 1.0/62/1000;
		}
		else
		{
			length  = pLink->DefaultDistance()/max(0.0000001,m_UnitDistance);
		}

			pLink->m_Length = max(0.00001,length);  // alllow mimum link length


		pLink->m_FreeFlowTravelTime = pLink->m_Length / pLink->m_FreeSpeed;
		pLink->m_StaticTravelTime = pLink->m_FreeFlowTravelTime;

		pLink->m_MaximumServiceFlowRatePHPL= m_DefaultCapacity;
		pLink->m_LaneCapacity  = m_DefaultCapacity;
		pLink->m_link_type= m_DefaultLinkType;

		pLink->m_FromPoint = m_NodeNoMap[FromNodeID]->pt;
		pLink->m_ToPoint = m_NodeNoMap[ToNodeID]->pt;

	
		if(bOffset)
		{
			double link_offset = m_UnitDistance*m_OffsetInDistance;
			double DeltaX = pLink->m_ToPoint.x - pLink->m_FromPoint.x ;
			double DeltaY = pLink->m_ToPoint.y - pLink->m_FromPoint.y ;
			double theta = atan2(DeltaY, DeltaX);

			pLink->m_FromPoint.x += link_offset* cos(theta-PI/2.0f);
			pLink->m_ToPoint.x += link_offset* cos(theta-PI/2.0f);

			pLink->m_FromPoint.y += link_offset* sin(theta-PI/2.0f);
			pLink->m_ToPoint.y += link_offset* sin(theta-PI/2.0f);
		}
		pLink->m_ShapePoints.push_back(pLink->m_FromPoint);
		pLink->m_ShapePoints.push_back(pLink->m_ToPoint);

		pLink->CalculateShapePointRatios();

		double lane_offset = m_UnitDistance*m_LaneWidthInMeter;  // 20 feet per lane

		unsigned int last_shape_point_id = pLink ->m_ShapePoints .size() -1;
		double DeltaX = pLink->m_ShapePoints[last_shape_point_id].x - pLink->m_ShapePoints[0].x;
		double DeltaY = pLink->m_ShapePoints[last_shape_point_id].y - pLink->m_ShapePoints[0].y;
		double theta = atan2(DeltaY, DeltaX);

		for(unsigned int si = 0; si < pLink ->m_ShapePoints .size(); si++)
		{
			GDPoint pt;

			pt.x = pLink->m_ShapePoints[si].x - lane_offset* cos(theta-PI/2.0f);
			pt.y = pLink->m_ShapePoints[si].y - lane_offset* sin(theta-PI/2.0f);

			pLink->m_BandLeftShapePoints.push_back (pt);

			pt.x  = pLink->m_ShapePoints[si].x + max(1,pLink->m_NumberOfLanes - 1)*lane_offset* cos(theta-PI/2.0f);
			pt.y = pLink->m_ShapePoints[si].y + max(1,pLink->m_NumberOfLanes - 1)*lane_offset* sin(theta-PI/2.0f);

			pLink->m_BandRightShapePoints.push_back (pt);
		}


		m_LinkSet.push_back (pLink);
		m_LinkNoMap[pLink->m_LinkNo]  = pLink;

		return pLink;
	}

	
	void SplitLinksForOverlappingNodeOnLinks(int ThisNodeID, bool bOffset = false, bool bLongLatFlag = false)
	{
		std::vector<DTALink*> OverlappingLinks;

		if(m_NodeNoMap.find(ThisNodeID)== m_NodeNoMap.end())
			return;

		GDPoint p0 = m_NodeNoMap[ThisNodeID]->pt ;

		// step 1: find overlapping links

		double threshold_in_pixels = 3;
		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		for(int si = 0; si < (*iLink) ->m_ShapePoints .size()-1; si++)
		{

			GDPoint pfrom, pto;
			pfrom = (*iLink)->m_ShapePoints[si];
			pto =  (*iLink)->m_ShapePoints[si+1];


			float distance_in_screen_unit =  g_GetPoint2LineDistance(p0, pfrom, pto,m_UnitDistance)*m_Doc_Resolution;

			if(distance_in_screen_unit<= 3) 
			{
			OverlappingLinks.push_back((*iLink));
			}

		}
	}

		//step 2: create new links

		for(int i = 0; i < OverlappingLinks.size(); i++)
		{

		int ExistingFromNodeID = OverlappingLinks[i]->m_FromNodeID ;
		int ExistingToNodeID = OverlappingLinks[i]->m_ToNodeID ;


		for(int add_link_index = 1; add_link_index <=2; add_link_index++)
		{
		int FromNodeID, ToNodeID;
		
		if(add_link_index==1)  // first link
		{
		FromNodeID= ExistingFromNodeID;
		ToNodeID = ThisNodeID;
		}
		if(add_link_index==2) // second link
		{
		FromNodeID= ThisNodeID;
		ToNodeID = ExistingToNodeID;
		}


		DTALink* pLink = 0;

		pLink = FindLinkWithNodeIDs(FromNodeID,ToNodeID);

		if(pLink != NULL)
				continue;  // a link with the same from and to node numbers exists!

		pLink = new DTALink(1);
		pLink->m_LinkNo = (int)(m_LinkSet.size());
		pLink->m_FromNodeNumber = m_NodeNotoNumberMap[FromNodeID];
		pLink->m_ToNodeNumber = m_NodeNotoNumberMap[ToNodeID];
		pLink->m_FromNodeID = FromNodeID;
		pLink->m_ToNodeID= ToNodeID;

		if(m_NodeNoMap.find(FromNodeID) == m_NodeNoMap.end())
		{
		
		return;
		}

		if(m_NodeNoMap.find(ToNodeID) == m_NodeNoMap.end())
		{
		
		return;
		}
		pLink->m_FromPoint = m_NodeNoMap[pLink->m_FromNodeID]->pt;
		pLink->m_ToPoint = m_NodeNoMap[pLink->m_ToNodeID]->pt;

		m_NodeNoMap[FromNodeID ]->m_Connections+=1;

		m_NodeNoMap[FromNodeID ]->m_OutgoingLinkVector.push_back(pLink->m_LinkNo);



		m_NodeNoMap[ToNodeID ]->m_Connections+=1;

		unsigned long LinkKey = GetLinkKey( pLink->m_FromNodeID, pLink->m_ToNodeID);
		m_NodeNotoLinkMap[LinkKey] = pLink;

		__int64  LinkKey2 = GetLink64Key(pLink-> m_FromNodeNumber,pLink->m_ToNodeNumber);
		m_NodeNumbertoLinkMap[LinkKey2] = pLink;

		pLink->m_NumberOfLanes= OverlappingLinks[i]->m_NumberOfLanes ;
		pLink->m_FreeSpeed=  OverlappingLinks[i]->m_FreeSpeed ;
		pLink->m_ReversedSpeedLimit =  OverlappingLinks[i]->m_FreeSpeed ;
		pLink->m_avg_simulated_speed =  OverlappingLinks[i]->m_FreeSpeed ;

		double length;  

		if(bLongLatFlag || m_LongLatFlag)  // bLongLatFlag is user input,  m_LongLatFlag is the system input from the project file 
			length  = g_CalculateP2PDistanceInMileFromLatitudeLongitude(pLink->m_FromPoint , pLink->m_ToPoint);
		else 
			length  = pLink->DefaultDistance()/max(0.0000001,m_UnitDistance);

		pLink->m_Length = max(0.00001,length);  // alllow mimum link length
		pLink->m_FreeFlowTravelTime = pLink->m_Length / pLink->m_FreeSpeed *60.0f;
		pLink->m_StaticTravelTime = pLink->m_FreeFlowTravelTime;

		pLink->m_MaximumServiceFlowRatePHPL= OverlappingLinks[i]->m_MaximumServiceFlowRatePHPL ;
		pLink->m_LaneCapacity  = OverlappingLinks[i]->m_LaneCapacity;
		pLink->m_link_type= OverlappingLinks[i]->m_link_type;

		pLink->m_FromPoint = m_NodeNoMap[FromNodeID]->pt;
		pLink->m_ToPoint = m_NodeNoMap[ToNodeID]->pt;


		if(bOffset)
		{
			double link_offset = m_UnitDistance*m_OffsetInDistance;
			double DeltaX = pLink->m_ToPoint.x - pLink->m_FromPoint.x ;
			double DeltaY = pLink->m_ToPoint.y - pLink->m_FromPoint.y ;
			double theta = atan2(DeltaY, DeltaX);

			pLink->m_FromPoint.x += link_offset* cos(theta-PI/2.0f);
			pLink->m_ToPoint.x += link_offset* cos(theta-PI/2.0f);

			pLink->m_FromPoint.y += link_offset* sin(theta-PI/2.0f);
			pLink->m_ToPoint.y += link_offset* sin(theta-PI/2.0f);
		}
		pLink->m_ShapePoints.push_back(pLink->m_FromPoint);
		pLink->m_ShapePoints.push_back(pLink->m_ToPoint);

		pLink->CalculateShapePointRatios();

		double lane_offset = m_UnitDistance*m_LaneWidthInMeter;  // 20 feet per lane

		unsigned int last_shape_point_id = pLink ->m_ShapePoints .size() -1;
		double DeltaX = pLink->m_ShapePoints[last_shape_point_id].x - pLink->m_ShapePoints[0].x;
		double DeltaY = pLink->m_ShapePoints[last_shape_point_id].y - pLink->m_ShapePoints[0].y;
		double theta = atan2(DeltaY, DeltaX);

		for(unsigned int si = 0; si < pLink ->m_ShapePoints .size(); si++)
		{
			GDPoint pt;

			pt.x = pLink->m_ShapePoints[si].x - lane_offset* cos(theta-PI/2.0f);
			pt.y = pLink->m_ShapePoints[si].y - lane_offset* sin(theta-PI/2.0f);

			pLink->m_BandLeftShapePoints.push_back (pt);

			pt.x  = pLink->m_ShapePoints[si].x + max(1,pLink->m_NumberOfLanes - 1)*lane_offset* cos(theta-PI/2.0f);
			pt.y = pLink->m_ShapePoints[si].y + max(1,pLink->m_NumberOfLanes - 1)*lane_offset* sin(theta-PI/2.0f);

			pLink->m_BandRightShapePoints.push_back (pt);
		}


		m_LinkSet.push_back (pLink);
		m_LinkNoMap[pLink->m_LinkNo]  = pLink;

		}

		}

	// step 3: delete overlapping links
			
		for(int i = 0; i < OverlappingLinks.size(); i++)
		{

			DeleteLink(OverlappingLinks[i]);  // delete link according to this link's pointer
		}

	
	}
	

	DTANode* AddNewNode(GDPoint newpt, int NewNodeNumber=0 , int LayerNo =0, bool ActivityLocation = false, bool bSplitLink = false)
	{
		Modify();
		DTANode* pNode = new DTANode;
		pNode->pt = newpt;
		pNode->m_LayerNo = LayerNo;
		pNode->m_NodeNo = GetUnusedNodeID();

		TRACE("Adding Node ID: %d\n", pNode->m_NodeNo );

		if(pNode->m_NodeNo ==31)
		{
		TRACE("");
		}

		if(NewNodeNumber ==0 )
		{
			pNode->m_NodeID = GetUnusedNodeNumber();
		}
		else
		{
			pNode->m_NodeID = NewNodeNumber;
		}


		pNode->m_ZoneID = 0;
		pNode->m_bZoneActivityLocationFlag = ActivityLocation;
		m_NodeSet.push_back(pNode);
		m_NodeNoMap[pNode->m_NodeNo] = pNode;
		m_NodeNumberMap[pNode->m_NodeID] = pNode;
		m_NodeNotoNumberMap[pNode->m_NodeNo ] = pNode->m_NodeID;
		m_NodeNumbertoNodeNoMap[pNode->m_NodeID] = pNode->m_NodeNo;

		if(bSplitLink)
		{
		SplitLinksForOverlappingNodeOnLinks(pNode->m_NodeNo,false,false);
		}
		return pNode;
	}

	bool DeleteNode(int NodeID)
	{
		std::list<DTANode*>::iterator iNode;

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if((*iNode)->m_Connections  == 0 && (*iNode)->m_NodeNo  == NodeID)
			{

				int ZoneID = (*iNode)->m_ZoneID;

				
				m_NodeNoMap[(*iNode)->m_NodeNo ] = NULL;
				m_NodeNoMap.erase ((*iNode)->m_NodeNo);
	
				m_NodeNumbertoNodeNoMap[(*iNode)->m_NodeID  ] = -1;

				m_NodeSet.erase  (iNode);
				return true;
			}
		}

		return false;
	}

	bool DeleteLink(DTALink* pLink)
	{


		int FromNodeID   = pLink->m_FromNodeID ;
		int ToNodeID   = pLink->m_ToNodeID ;
		unsigned long LinkKey = GetLinkKey( FromNodeID , ToNodeID );
		
		m_NodeNotoLinkMap.erase (LinkKey);  
		m_NodeNotoLinkMap[LinkKey] =NULL;

		m_NodeNoMap[FromNodeID ]->m_Connections-=1;

		for(int ii = 0; ii< m_NodeNoMap[FromNodeID ]->m_OutgoingLinkVector.size();ii++)
		{
			if(m_NodeNoMap[FromNodeID ]->m_OutgoingLinkVector[ii] == pLink->m_LinkNo)
			{
				m_NodeNoMap[FromNodeID ]->m_OutgoingLinkVector.erase(m_NodeNoMap[FromNodeID ]->m_OutgoingLinkVector.begin()+ii);

				break;
			}
		}

		m_NodeNoMap[ToNodeID ]->m_Connections-=1;
		m_LinkNoMap.erase (pLink->m_LinkNo);  

		m_LinkNoMap[pLink->m_LinkNo]  = NULL;

		std::list<DTALink*>::iterator iLink;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if((*iLink) == pLink)
			{
				m_LinkSet.erase  (iLink);
				break;
			}
		}

		// 
		//resort link no;

		m_LinkNotoLinkMap.clear();

		m_LinkNoMap.clear();
		int i= 0;
		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++, i++)
		{
			(*iLink)->m_LinkNo = i;
			m_LinkNotoLinkMap[i] = (*iLink);
			m_LinkNoMap[i] = (*iLink);
		}


		// remove isolated nodes
		DeleteNode (FromNodeID);
		DeleteNode (ToNodeID);

		return true;
	
	}

	bool DeleteLink(int LinkNo)
	{
		DTALink* pLink = m_LinkNoMap[LinkNo];

		if(pLink == NULL)
			return false;  // a link with the same from and to node numbers exists!

		return DeleteLink(pLink);
	}


	int GetUnusedNodeNumber()
	{
		int NewNodeNumber = m_StartNodeNumberForNewNodes;

		for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if(NewNodeNumber <= (*iNode)->m_NodeID  )  // this node number has been used
				NewNodeNumber = (*iNode)->m_NodeID +1;
		}

		return NewNodeNumber;

	}

	int GetUnusedNodeID()
	{
		int NewNodeID = 0;

		for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if(NewNodeID <= (*iNode)->m_NodeNo)
				NewNodeID = (*iNode)->m_NodeNo +1;
		}

		return NewNodeID;

	}

	char GetApproachChar(DTA_Direction approach)
	{
		char c;
		switch (approach) 
		{
		case DTA_North: c = 'N'; break;
		case DTA_East: c = 'E'; break;
		case DTA_South: c = 'S'; break;
		case DTA_West: c = 'W'; break;
		default: c = '0'; break;

		}
		return c;
	};


	std::map<CString, DTA_Movement_Data_Matrix> m_DTAMovementMap;
	std::map<CString, DTA_Phasing_Data_Matrix> m_DTAPhasingMap;

	DTA_Phasing_Data_Matrix GetPhaseData(int node_id, std::string timing_plan_name);

	BOOL IfMovementIncludedInPhase(int node_id, std::string timing_plan_name, int phase_no, long from_node_id, int destination_node_id); 
	BOOL IfMovementDirIncludedInPhase(int node_id, std::string timing_plan_name, int phase_no, int movement_index);

	void SetupPhaseData(int node_id, std::string timing_plan_name, int phase_numbr, DTA_SIG_PHASE_ROW attribute, float value);
	void SetupPhaseData(int node_id, std::string timing_plan_name, int phase_numbr, DTA_SIG_PHASE_ROW attribute, int value);
	void SetupPhaseData(int node_id, std::string timing_plan_name,int phase_numbr, DTA_SIG_PHASE_ROW attribute, std::string value_str);
	void SetupPhaseData(int node_id, std::string timing_plan_name,int phase_numbr, DTA_SIG_PHASE_ROW attribute, CString value_str);

	void SetupSignalValue(int node_id, std::string timing_plan_name, DTA_SIG_PHASE_ROW attribute, float value);
	void SetupSignalValue(int node_id, std::string timing_plan_name, DTA_SIG_PHASE_ROW attribute, int value);
	void SetupSignalValue(int node_id, std::string timing_plan_name, DTA_SIG_PHASE_ROW attribute, CString value_str);

	// 	void ConstructMovementVector(bool flag_Template);
	// function declaration for Synchro /////////////////////////////////////////////////////////////////////////////////
	void ConstructMovementVector();

	void ExportSingleSynchroFile(CString SynchroProjectFile);


	std::map<CString, PathStatistics> m_PathMap;
	std::map<CString, DTALink*> m_LinkKeyMap;
	std::map<std::string, DTALink*> m_SpeedSensorIDMap;
	std::map<std::string, DTALink*> m_CountSensorIDMap;

	std::map<CString, PathStatistics> m_ODMatrixMap;

	std::map<CString, PathStatistics> m_ODProbeMatrixMap;

	std::map<CString, Movement3Node> m_Movement3NodeMap;  // turnning movement count

	std::map<CString, int> m_LinkFlowProportionMap;  // link-turnning movement count (i,j,tau, from node, to node, destination node)
	std::map<CString, int> m_LinkFlowProportionODMap;  // link-turnning movement count (for each OD, time pair: i,j,tau)


	std::map<CString, DTANodeMovement*> m_MovementPointerMap;  // turnning movement pointer




	DTANodeMovement* FindMovement(int FromNodeNumber,int ToNodeNumber, int DestNodeNumber)
	{
		DTANodeMovement*  pMovement = NULL;

	
			CString label;
		int up_node_id = m_NodeNoMap[FromNodeNumber]->m_NodeNo     ;
		long to_node_id = m_NodeNoMap[ToNodeNumber]->m_NodeNo     ;
		int dest_node_id = m_NodeNoMap[DestNodeNumber ]->m_NodeNo ;
		label.Format("%d;%d;%d", up_node_id,to_node_id,dest_node_id);

		if(m_MovementPointerMap.find(label)!= m_MovementPointerMap.end())
		{
		pMovement = m_MovementPointerMap[label]; // store pointer
		}
		return pMovement;
	}

	
	void ReadAMSPhasingFile(LPCTSTR lpszPathName);
	void SaveTimingData();

	
	std::map<std::string, DTALink*>  m_TMC2LinkMap;
	CString m_GISMessage;


	void ExportSynchroVersion6Files(std::string TimingPlanName = "0");
	bool m_bMovementAvailableFlag;
	bool ReadSynchroPreGeneratedLayoutFile(LPCTSTR lpszFileName);
	CString m_Synchro_ProjectDirectory;


	bool m_ImportNetworkAlready;


	std::map<CString,DTA_Direction> m_PredefinedApproachMap;

	int Find_P2P_Angle(GDPoint p1, GDPoint p2);
	double Find_P2P_Distance(GDPoint p1, GDPoint p2);
	DTA_Turn Find_RelativeAngle_to_Left_OR_Right_Turn(int relative_angle);
	DTA_Turn Find_RelativeAngle_to_Left_OR_Right_Turn_1_OR_2(int relative_angle);


	DTA_Turn Find_RelativeAngle_to_Turn(int relative_angle);

	DTA_Direction Find_Angle_to_Approach_8_direction(int angle);
	DTA_Direction Find_Angle_to_Approach_4_direction(int angle, int &relative_angel_difference_from_main_direction);
	DTA_Direction Find_Angle_to_Approach_4_directionWithoutGivenDirection(int angle, DTA_Direction not_use_DTA_Direction);
	
	DTA_Direction Find_Closest_Angle_to_Approach(int angle);

	std::map<DTA_Direction,int> m_ApproachMap;

	std::map<DTA_Direction,DTA_Direction> m_OpposingDirectionMap;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	int Find_PPP_RelativeAngle(GDPoint p1, GDPoint p2, GDPoint p3);
	DTA_Turn Find_PPP_to_Turn(GDPoint p1, GDPoint p2, GDPoint p3);

	DTA_Turn Find_PPP_to_Turn_with_DTAApproach(GDPoint p1, GDPoint p2, GDPoint p3,DTA_Direction approach1, DTA_Direction approach2);

	eSEARCHMODE m_SearchMode;
	int MaxNodeKey;
	int MaxNode64Key;
	unsigned long GetLinkKey(int FromNodeID, int ToNodeID)
	{

		unsigned long LinkKey = FromNodeID*MaxNodeKey+ToNodeID;
		return LinkKey;
	}

	__int64 GetLink64Key(int FromNodeNumber, int ToNodeNumber)
	{

		__int64 LinkKey = FromNodeNumber*MaxNode64Key+ToNodeNumber;
		return LinkKey;
	}

	DTANode* FindNodeWithNodeNumber(int NodeID)
	{
		if(m_NodeSet.size()==0)
			return NULL;
		for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if(NodeID == (*iNode)->m_NodeID )
				return (*iNode);
		}
		return NULL;
	}

		DTANode* FindSignalNodeWithCoordinate(double x, double y, int LayerNo = 0, double min_distance = 9999999)
	{
		
		DTANode* pNode= NULL;

		int NodeID = -1;
		for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if((*iNode)->m_LayerNo == LayerNo && (*iNode)->m_ControlType == m_ControlType_PretimedSignal)
			{
			double distance = sqrt( ((*iNode)->pt.x - x)*((*iNode)->pt.x - x) + ((*iNode)->pt.y - y)*((*iNode)->pt.y - y));
			if( distance <  min_distance)
			{
				min_distance= distance;
				pNode = (*iNode);
			}
			}
		}
		return pNode;
	}

		
	int FindNodeNumberWithCoordinate(double x, double y, double min_distance = 0.0000001)
	{
		
		DTANode* pNode= NULL;

		min_distance = 0.00000001;
		int NodeID = -1;
		for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			double distance = sqrt( ((*iNode)->pt.x - x)*((*iNode)->pt.x - x) + ((*iNode)->pt.y - y)*((*iNode)->pt.y - y));
			if( distance <  min_distance)
			{
				min_distance= distance;
				pNode = (*iNode);
			}
		}
		if(pNode != NULL)
			return pNode->m_NodeID;
		else
			return NULL;
	}

	int FindNodeIDWithCoordinate(double x, double y, double min_distance = 0.0000001)
	{
		
		DTANode* pNode= NULL;

		min_distance = 0.00000001;
		int NodeID = -1;
		for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			double distance = sqrt( ((*iNode)->pt.x - x)*((*iNode)->pt.x - x) + ((*iNode)->pt.y - y)*((*iNode)->pt.y - y));
			if( distance <  min_distance)
			{
				min_distance= distance;
				pNode = (*iNode);
			}
		}
		if(pNode != NULL)
			return pNode->m_NodeNo;
		else
			return NULL;
	}


	int FindNonCentroidNodeNumberWithCoordinate(double x, double y, int this_node_name);

	float GetNodeTotalDelay(int ToNodeNumber, int time, int& LOS);

	DTALink* FindFreewayLinkWithFromNodeNumber(int FromNodeNumber)
	{

		if(m_NodeNumberMap.find(FromNodeNumber)!= m_NodeNumberMap.end())
		{
		
		DTANode* pFromNode = m_NodeNumberMap[FromNodeNumber];

		for(unsigned int i = 0; i< pFromNode->m_OutgoingLinkVector.size(); i++)
		{
			
			DTALink* pLink = m_LinkNoMap[ pFromNode->m_OutgoingLinkVector[i]];

			if(pLink->m_FromNodeNumber == FromNodeNumber && m_LinkTypeMap[pLink->m_link_type].IsFreeway() )
				return pLink;
		
		}
		
		}
		return NULL;
	}
	DTALink* FindFreewayLinkWithToNodeNumber(int ToNodeNumber)
	{

		if(m_NodeNumberMap.find(ToNodeNumber)!= m_NodeNumberMap.end())
		{
		
		DTANode* pToNode = m_NodeNumberMap[ToNodeNumber];

		for(unsigned int i = 0; i< pToNode->m_IncomingLinkVector.size(); i++)
		{
			
			DTALink* pLink = m_LinkNoMap[pToNode->m_IncomingLinkVector[i]];

			if(pLink->m_ToNodeNumber == ToNodeNumber && m_LinkTypeMap[pLink->m_link_type].IsFreeway() )
				return pLink;
		
		}
		
		}
		return NULL;
	}

	DTALink* FindLinkWithNodeNumbers(int FromNodeNumber, int ToNodeNumber, CString FileName = "", bool bWarmingFlag = false)
	{
		if(m_NodeNumberMap.find(FromNodeNumber)!= m_NodeNumberMap.end())
		{
		
		DTANode* pFromNode = m_NodeNumberMap[FromNodeNumber];

		for(unsigned int i = 0; i< pFromNode->m_OutgoingLinkVector.size(); i++)
		{
			if(m_LinkNoMap.find (pFromNode->m_OutgoingLinkVector[i])== m_LinkNoMap.end())
				return NULL;

			DTALink* pLink = m_LinkNoMap[pFromNode->m_OutgoingLinkVector[i]];

			if(pLink->m_FromNodeNumber == FromNodeNumber && pLink->m_ToNodeNumber == ToNodeNumber)
				return pLink;
		
		}
		}
			CString msg;

			if(FileName.GetLength() == 0)
			{
							return NULL;

			}
			else if (bWarmingFlag == true)
			{
			msg.Format ("Link %d-> %d cannot be found in file %s.", FromNodeNumber, ToNodeNumber,FileName);
			AfxMessageBox(msg);
			return NULL;
			}
			return NULL;
	}

	DTALink* FastFindLinkWithNodeNumbers(int FromNodeNumber, int ToNodeNumber)
	{
		return FindLinkWithNodeNumbers(FromNodeNumber, ToNodeNumber);
	}
	//DTALink* FindLinkWithNodeNumbers(int FromNodeNumber, int ToNodeNumber, CString FileName = "", bool bWarmingFlag = false)
	//{
	//	int FromNodeID = m_NodeNumbertoNodeNoMap[FromNodeNumber];
	//	int ToNodeID = m_NodeNumbertoNodeNoMap[ToNodeNumber];

	//	unsigned long LinkKey = GetLinkKey( FromNodeID, ToNodeID);

	//	map <unsigned long, DTALink*> :: const_iterator m_Iter = m_NodeNotoLinkMap.find(LinkKey);

	//	if(m_Iter == m_NodeNotoLinkMap.end( ) && bWarmingFlag)
	//	{
	//		CString msg;

	//		if(FileName.GetLength() == 0)
	//		{
	//			msg.Format ("Link %d-> %d cannot be found.", FromNodeNumber, ToNodeNumber);
	//		}else
	//		{
	//			msg.Format ("Link %d-> %d cannot be found in file %s.", FromNodeNumber, ToNodeNumber,FileName);
	//		}
	//		AfxMessageBox(msg);
	//		return NULL;
	//	}
	//	return m_NodeNotoLinkMap[LinkKey];
	//}

	void ClearNetworkData();

	DTALink* FindLinkWithNodeIDs(int FromNodeID, int ToNodeID)
	{

		unsigned long LinkKey = GetLinkKey( FromNodeID, ToNodeID);
		if(m_NodeNotoLinkMap.find(LinkKey)!=m_NodeNotoLinkMap.end())
			return m_NodeNotoLinkMap[LinkKey];
		else
			return NULL;
	}


	DTALink* FindLinkWithLinkNo(int LinkNo)
	{
	if(m_LinkNotoLinkMap.find(LinkNo) != m_LinkNotoLinkMap.end())
		return m_LinkNotoLinkMap[LinkNo];
	else
		return NULL;
	}


	DTALink* FindLinkWithLinkID(string LinkID)
	{
		if(m_LinkIDtoLinkMap.find(LinkID) != m_LinkIDtoLinkMap.end())
		{
		return m_LinkIDtoLinkMap[LinkID];
		}else
		return NULL;
	}
	CString GetTimeStampStrFromIntervalNo(int time_interval, bool with_single_quote=false);
	CString GetTimeStampFloatingPointStrFromIntervalNo(int time_interval);

	CString GetTimeStampString(int time_stamp_in_min);
	int GetTimeStampfromString(CString str);
	CString GetTimeStampString24HourFormat(int time_stamp_in_min);
	int* m_ZoneCentroidSizeAry;  //Number of centroids per zone
	int** m_ZoneCentroidNodeAry; //centroid node Id per zone

	std::vector<CString> m_SearchHistoryVector;


	COLORREF m_ZoneTextColor;

	bool m_BackgroundBitmapLoaded;
	bool m_LongLatCoordinateFlag;
	CImage m_BackgroundBitmap;  // background bitmap
	
	double m_ImageX1,m_ImageX2,m_ImageY1,m_ImageY2, m_ImageWidth, m_ImageHeight;
	double m_ImageXResolution, m_ImageYResolution;
	double m_ImageMoveSize;

	double m_SensorMapX, m_SensorMapY, m_SensorMapXResolution, m_SensorMapYResolution;
	double m_SensorMapMoveSize;

	double m_ImageWidthInMile;  // in mile

	// Operations
public:

public: // subarea
	std::vector<GDPoint> m_SubareaShapePoints;

	bool CheckControlData();
	void SendTexttoStatusBar(CString str,int Index = 0);


	// Overrides
public:

	void ShowPathListDlg(bool bShowFlag);
	void OpenCSVFileInExcel(CString filename);
	void Constructandexportsignaldata();

	bool bSynchroImportReadyToSaveFlag;
	void ReadSynchroUniversalDataFiles();

	bool ReadSynchroCombinedCSVFile(LPCTSTR lpszFileName);


	void ConvertOriginBasedDemandFile(LPCTSTR lpszFileName);
	bool ReadSynchroLayoutFile(LPCTSTR lpszFileName);
	bool ReadSynchroLayoutFile_And_AddOutgoingLinks_For_ExternalNodes(LPCTSTR lpszFileName);
	bool ReadSynchroLaneFile(LPCTSTR lpszFileName);
	bool ReadSynchroPhasingFile(LPCTSTR lpszFileName);

	bool m_bFitNetworkInitialized;
	void CalculateDrawingRectangle(bool NodeLayerOnly = false);

	DWORD ProcessExecute(CString & strCmd, CString & strArgs,  CString & strDir, BOOL bWait);
	DWORD ProcessWait(DWORD PID);

	CString m_ProjectFile;
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	bool m_bExport_Link_MOE_in_input_link_CSF_File;
	BOOL SaveProject(LPCTSTR lpszPathName,int SelectedLayNo=0);
	BOOL SaveNodeFile();
	BOOL SaveLinkData(LPCTSTR lpszPathName,bool bExport_Link_MOE_in_input_link_CSF_File, int SelectedLayNo);
	BOOL SaveDefaultInputSignalData(LPCTSTR lpszPathName);

	bool CheckIfFileExsits(LPCTSTR lpszFileName)
	{

	FILE* pFile = NULL;
	fopen_s(&pFile,lpszFileName,"rb");
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		int Length = ftell(pFile);
		fclose(pFile);

		if(Length> 10) // minimum size
		return true;
	}		
		return false;

	}

	CString m_DefaultDataFolder;


	int FindCloseDTANode_WithNodeNumber(GDPoint pt, double threadshold, int this_node_number = -1)
	{
		 double min_distance  = 99999;
		 int NodeID = 0;
		for (std::list<DTANode*>::iterator iPoint = m_NodeSet.begin(); iPoint != m_NodeSet.end(); iPoint++)
		{

				double distance = Find_P2P_Distance((*iPoint)->pt,pt);
				if(min_distance > distance && (*iPoint)->m_NodeID != this_node_number )
				{
					min_distance = distance;
					NodeID = (*iPoint)->m_NodeID;
				}
		}
	
		if(min_distance < threadshold)
			return NodeID;
		else 
			return 0;
	}


	void WriteStringToLogFile(CString FileName, CString LogMessage)
	{
		ofstream outFile;

		outFile.open(m_ProjectDirectory + FileName);
		
		if(outFile.is_open ())
		{
		  outFile<< LogMessage;
		   outFile.close();
		}


		
	}
	double m_PointA_x,m_PointA_y,m_PointB_x,m_PointB_y;
	double m_PointA_long,m_PointA_lat,m_PointB_long,m_PointB_lat;
	bool m_bPointA_Initialized, m_bPointB_Initialized;

	void ResetBackgroundImageCoordinate();

	void OnImportdataImportExcelFile();

	int SelectLink(GDPoint point, double& final_matching_distance);
	// For demonstration
	CString m_SampleExcelNetworkFile;
	CString m_SampleOutputProjectFile;
	CString m_SampleExcelSensorFile;
	CString m_SampleNGSIMDataFile;

	bool FindObject(eSEARCHMODE SearchMode, int value1, int value12);


	void UpdateMovementGreenStartAndEndTimeFromPhasingData(int NodeID, std::string timing_plan_name);
	void UpdateAllMovementGreenStartAndEndTime(std::string timing_plan_name);

	// Implementation
	void GenerateMovementCountFromAgentFile(float PeakHourFactor);
	void MapSignalDataAcrossProjects();
	void UpdateUnitMile();
public:
	virtual ~CTLiteDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	int m_sensor_data_aggregation_type;
	void SensortoolsConverttoHourlyVolume();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveimagelocation();
	COLORREF GetLinkTypeColor(int LinkType);
	afx_msg void OnShowShowpathmoe();
	afx_msg void OnUpdateShowShowpathmoe(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveProject();
	afx_msg void OnFileSaveProjectAs();
	afx_msg void OnImageImportbackgroundimage();
	afx_msg void OnFileDataloadingstatus();
	afx_msg void OnMoeVolume();
	afx_msg void OnMoeSpeed();
	afx_msg void OnMoeDensity();
	afx_msg void OnMoeQueuelength();
	afx_msg void OnMoeFuelconsumption();
	afx_msg void OnMoeEmissions();
	afx_msg void OnUpdateMoeVolume(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMoeSpeed(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMoeDensity(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMoeQueuelength(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMoeFuelconsumption(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMoeEmissions(CCmdUI *pCmdUI);
	afx_msg void OnMoeNone();
	afx_msg void OnUpdateMoeNone(CCmdUI *pCmdUI);
	afx_msg void OnToolsCarfollowingsimulation();
	afx_msg void OnToolsPerformtrafficassignment();
	afx_msg void OnMoeVcRatio();
	afx_msg void OnUpdateMoeVcRatio(CCmdUI *pCmdUI);
	afx_msg void OnMoeTraveltime();
	afx_msg void OnUpdateMoeTraveltime(CCmdUI *pCmdUI);
	afx_msg void OnMoeCapacity();
	afx_msg void OnUpdateMoeCapacity(CCmdUI *pCmdUI);
	afx_msg void OnMoeSpeedlimit();
	afx_msg void OnUpdateMoeSpeedlimit(CCmdUI *pCmdUI);
	afx_msg void OnMoeFreeflowtravletime();
	afx_msg void OnUpdateMoeFreeflowtravletime(CCmdUI *pCmdUI);
	afx_msg void OnEditDeleteselectedlink();
	afx_msg void OnImportAgentFile();
	afx_msg void OnImportNgsimFile();

	afx_msg void OnMoeLength();
	afx_msg void OnUpdateMoeLength(CCmdUI *pCmdUI);
	afx_msg void OnEditSetdefaultlinkpropertiesfornewlinks();
	afx_msg void OnUpdateEditSetdefaultlinkpropertiesfornewlinks(CCmdUI *pCmdUI);
	afx_msg void OnToolsProjectfolder();
	afx_msg void OnToolsOpennextaprogramfolder();
	afx_msg void OnMoeNoodmoe();
	afx_msg void OnUpdateMoeNoodmoe(CCmdUI *pCmdUI);
	afx_msg void OnOdtableImportOdTripFile();
	afx_msg void OnToolsEditassignmentsettings();
	afx_msg void OnSearchLinklist();
	afx_msg void OnMoeAgent();
	afx_msg void OnUpdateMoeAgent(CCmdUI *pCmdUI);
	afx_msg void OnToolsViewsimulationsummary();
	afx_msg void OnToolsViewassignmentsummarylog();
	afx_msg void OnHelpVisitdevelopmentwebsite();
	afx_msg void OnToolsRuntrafficassignment();
	afx_msg void OnToolsPerformscheduling();
	afx_msg void OnFileChangecoordinatestolong();
	afx_msg void OnToolsExportopmodedistribution();
	afx_msg void OnToolsEnumeratepath();
	afx_msg void OnResearchtoolsExporttodtalitesensordataformat();
	afx_msg void OnScenarioConfiguration();
	afx_msg void OnMoeViewmoes();
	afx_msg void OnImportdataImport();
	afx_msg void OnMoeAgentpathanalaysis();
	afx_msg void OnFileConstructandexportsignaldata();
	afx_msg void OnLinkUserDefinedMOE();
	afx_msg void OnUpdateUserDefinedMOE(CCmdUI *pCmdUI);
	afx_msg void OnEditOffsetlinks();
	afx_msg void OnFileOpenNetworkOnly();
	afx_msg void OnLinkAddlink();
	afx_msg void OnProjectEdittime();
	afx_msg void OnLinkAgentstatisticsanalaysis();
	afx_msg void OnSubareaDeletesubarea();
	afx_msg void OnSubareaViewAgentstatisticsassociatedwithsubarea();
	afx_msg void OnToolsTraveltimereliabilityanalysis();
	afx_msg void OnLinkLinkbar();
	afx_msg void OnLinkIncreaseoffsetfortwo();
	afx_msg void OnLinkDecreaseoffsetfortwo();
	afx_msg void OnLinkNooffsetandnobandwidth();
	afx_msg void OnViewShowhideLegend();
	afx_msg void OnMoeViewlinkmoesummaryfile();
	afx_msg void OnViewCalibrationview();
	afx_msg void OnUpdateViewCalibrationview(CCmdUI *pCmdUI);
	afx_msg void OnMoeViewtrafficassignmentsummaryplot();
	afx_msg void OnMoeViewoddemandestimationsummaryplot();
	afx_msg void OnProjectEditpricingscenariodata();
	afx_msg void OnLinkViewlink();
	afx_msg void OnDeleteSelectedLink();
	afx_msg void OnImportRegionalplanninganddtamodels();
	afx_msg void OnExportGenerateTravelTimeMatrix();
	afx_msg void OnExportGenerateshapefiles();
	afx_msg void OnLinkmoedisplayQueuelength();
	afx_msg void OnUpdateLinkmoedisplayQueuelength(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLinkmoeTraveltimereliability(CCmdUI *pCmdUI);


	afx_msg void OnMoePathlist();
	afx_msg void OnViewShowmoe();
	afx_msg void OnUpdateViewShowmoe(CCmdUI *pCmdUI);
	afx_msg void OnFileUploadlinkdatatogooglefusiontable();
	afx_msg void On3Viewdatainexcel();
	afx_msg void On5Viewdatainexcel();
	afx_msg void OnMoeViewnetworktimedependentmoe();
	afx_msg void On2Viewdatainexcel33398();
	afx_msg void On2Viewnetworkdata();
	afx_msg void On3Viewoddatainexcel();
	afx_msg void OnMoeOpenallmoetables();
	afx_msg void OnBnClickedButtonDatabase();
	afx_msg void OnToolsUnittesting();
	afx_msg void OnImportAmsdataset();
	afx_msg void OnDemandfileOddemandmatrix();
	afx_msg void OnDemandfileHovoddemandmatrix();
	afx_msg void OnDemandfileTruckoddemandmatrix();
	afx_msg void OnDemandfileIntermodaloddemandmatrix();
	afx_msg void OnLinkAddIncident();
	afx_msg void OnToolsGeneratephysicalzonecentroidsonroadnetwork();
	afx_msg void OnImportDemanddataset();
	afx_msg void OnNodeIncreasenodetextsize();
	afx_msg void OnNodeDecreasenodetextsize();
	afx_msg void OnImportSynchroutdfcsvfiles();
	afx_msg void OnProjectEditmoesettings();
	afx_msg void OnProjectMultiScenarioResults();
	afx_msg void OnProject12();
	afx_msg void OnViewMovementMoe();
	afx_msg void OnProjectTimeDependentLinkMoe();
	afx_msg void OnViewOdmeResult();
	afx_msg void OnProjectOdmatrixestimationinput();
	afx_msg void OnProjectInputsensordataforodme();
	afx_msg void OnLinkattributedisplayLinkname();
	afx_msg void OnUpdateLinkattributedisplayLinkname(CCmdUI *pCmdUI);
	afx_msg void OnAssignmentSimulatinSettinsClicked();
	afx_msg void OnProjectNetworkData();
	afx_msg void OnLinkAddsensor();
	afx_msg void OnImportSynchrocombinedcsvfile();
	afx_msg void OnMoeTableDialog();
	afx_msg void OnSensortoolsConverttoHourlyVolume();
	afx_msg void OnImportInrixshapefileandspeeddata();

	afx_msg void OnTrafficcontroltoolsTransfermovementdatafromreferencenetworktocurrentnetwork();
	afx_msg void OnDemandtoolsGenerateinput();
	afx_msg void OnNetworktoolsResetlinklength();
	afx_msg void OnSubareaCreatezonefromsubarea();
	afx_msg void OnDemandConvert();
	afx_msg void OnTrafficcontroltoolsTransfersignaldatafromreferencenetworktocurrentnetwork();
	afx_msg void OnImportBackgroundimage();
	afx_msg void OnZoneDeletezone();
	afx_msg void OnNodeViewnodedata();
	afx_msg void OnLinkViewlinkdata();
	afx_msg void OnMovementViewmovementdatatable();
	afx_msg void OnOdmatrixOddemandmatrix();
	afx_msg void OnWorkzoneViewworkzonedata();
	afx_msg void OnVmsViewvmsdatatable();
	afx_msg void OnTollViewtolldatatable();
	afx_msg void OnDetectorViewsensordatatable();
	afx_msg void OnLinkmoeExportlinkmoedatatoshapefile();
	afx_msg void OnNodeExportnodelayertogisshapefile();
	afx_msg void OnZoneExportzonelayertogisshapefile();
	afx_msg void OnGridUsemileasunitoflength();
	afx_msg void OnUpdateGridUsemileasunitoflength(CCmdUI *pCmdUI);
	afx_msg void OnGridUsekmasunitoflength();
	afx_msg void OnUpdateGridUsekmasunitoflength(CCmdUI *pCmdUI);
	afx_msg void OnGridUselong();
	afx_msg void OnUpdateGridUselong(CCmdUI *pCmdUI);
	afx_msg void OnShowMoePathlist();
	afx_msg void OnExportExportaggregatedlinkmoefile();
	afx_msg void OnHelpReportbug();

	afx_msg void OnDetectorExportlinkflowproportionmatrixtocsvfile();

	afx_msg void OnLinkApplydefaultlinkattributestoalllinks();
	afx_msg void OnLinkApplydefaultlanecapacitytoalllinks();
	afx_msg void OnLinkApplydefaultnumberoflanestoalllinks();
	afx_msg void OnTrafficcapacitySetdefault();
	afx_msg void OnMovementHidenon();
	afx_msg void OnUpdateMovementHidenon(CCmdUI *pCmdUI);
	afx_msg void OnMovementSetpeakhourfactor();
	afx_msg void OnZoneChangezonenumber();
	afx_msg void OnUpdateZoneDeletezone(CCmdUI *pCmdUI);
	afx_msg void OnUpdateZoneChangezonenumber(CCmdUI *pCmdUI);
	afx_msg void OnChangelinktypecolorFreeway();
	afx_msg void OnChangelinktypecolorRamp();
	afx_msg void OnChangelinktypecolorArterial();
	afx_msg void OnChangelinktypecolorConnector();
	afx_msg void OnChangelinktypecolorTransit();
	afx_msg void OnChangelinktypecolorWalkingmode();
	afx_msg void OnChangelinktypecolorResettodefaultcolorschema();
	afx_msg void OnNodeChangenodecolor();
	afx_msg void OnNodeChangenodebackgroundcolor();
	afx_msg void OnZoneChangezonecolor();
	afx_msg void OnEditUndo33707();
	afx_msg void OnUpdateEditUndo33707(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo33709();
	afx_msg void OnUpdateEditRedo33709(CCmdUI *pCmdUI);
	afx_msg void OnMovementSetupnumberofleftturnlanesforsignalizednodes();
	afx_msg void OnMovementOptimizephasingandtimingdataforalltrafficsignalsthroughqem();
	afx_msg void OnLinkmoedisplayImpact();
	afx_msg void OnUpdateLinkmoedisplayImpact(CCmdUI *pCmdUI);
	afx_msg void OnLinkmoedisplayBottleneck();
	afx_msg void OnUpdateLinkmoedisplayBottleneck(CCmdUI *pCmdUI);
	afx_msg void OnGisplanningdatasetConfigureimportingsettingfile();
	afx_msg void OnSubareaExporttotalnumberofAgentsinsubarea();
	afx_msg void OnMoeViewoddemandestimationsummaryplotLanedensity();
	afx_msg void OnToolsConfiguration();
	afx_msg void OnPathClearallpathdisplay();
	afx_msg void OnToolsImportsynchroncombinedfile();
	afx_msg void OnOdmatrixRapid();
	afx_msg void OnOdmatrixRapidDecrease();
	afx_msg void OnButtonAbm();
	afx_msg void OnHelpVisitdevelopmentwebsiteDtalite();
};
extern std::list<CTLiteDoc*>	g_DocumentList;
extern bool g_TestValidDocument(CTLiteDoc* pDoc);