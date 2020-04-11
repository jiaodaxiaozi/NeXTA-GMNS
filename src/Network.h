//  Portions Copyright 2010 Xuesong Zhou (xzhou99@gmail.com)

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
#pragma once
#pragma	warning(disable: 4018) // signed/unsigned mismatch
#pragma	warning(disable: 4305) //  truncation from 'double' to 'float'
#pragma	warning(disable: 4995) //  'CDaoRecordset': name was marked as #pragma deprecated
#pragma	warning(disable: 4267) //  'initializing' : conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable: 4244)  // stop warning: "conversion from 'int' to 'float', possible loss of data"
#pragma warning(disable: 4996)  // Consider using sscanf_s instead
#pragma warning(disable: 4101)  // unreferenced local variable


#include "resource.h"
#include "Utility.h"

#include <math.h>
#include <deque>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;
// NB	SB	EB	WB	NE	NW SE	SW
enum DTA_Direction
{
	DTA_North = 0,
	DTA_South,
	DTA_East,
	DTA_West,
	DTA_NorthEast,
	DTA_NorthWest,
	DTA_SouthEast,
	DTA_SouthWest,
	DTA_NotDefined,
	DTA_MAX_Direction
};

enum DTA_Turn
{
	DTA_LeftTurn,
	DTA_Through,
	DTA_RightTurn,
	DTA_OtherTurn,
	DTA_LeftTurn2,
	DTA_RightTurn2,
};


enum DTA_SIG_MOVEMENT
   {
		DTA_SIG_MOVEMENT_NODE_ID = 0,
		DTA_SIG_MOVEMENT_SCENARIO_NO,
		DTA_SIG_MOVEMENT_START_DAY_NO,
		DTA_SIG_MOVEMENT_END_DAY_NO,
		DTA_SIG_MOVEMENT_START_MIN_IN_SECOND,
		DTA_SIG_MOVEMENT_END_MIN_IN_SECOND,
		DTA_SIG_MOVEMENT_NOTES,
		DTA_SIG_MOVEMENT_RECORD_NAME,
		DTA_SIG_MOVEMENT_REFERENCE_ID,
		DTA_NBL2,
		DTA_NBL,
		DTA_NBT,
		DTA_NBR,
		DTA_NBR2,
		DTA_SBL2,
		DTA_SBL,
		DTA_SBT,
		DTA_SBR,
		DTA_SBR2,
		DTA_EBL2,
		DTA_EBL,
		DTA_EBT,
		DTA_EBR,
		DTA_EBR2,
		DTA_WBL2,
		DTA_WBL,
		DTA_WBT,
		DTA_WBR,
		DTA_WBR2,
		DTA_NEL,
		DTA_NET,
		DTA_NER,
		DTA_NWL,
		DTA_NWT,
		DTA_NWR,
		DTA_SEL,
		DTA_SET,
		DTA_SER,
		DTA_SWL,
		DTA_SWT,
		DTA_SWR,
		DTA_PED,
		DTA_HOLD,
		DTA_SIG_MOVEMENT_GEOMETRY,
		DTA_SIG_MOVEMENT_MAX_COLUMN,
		DTA_LANES_COLUME_init
   };


enum DTA_SIG_MOVEMENT_ROW
   {
	    DTA_MOVEMENT_ATTRIBUTE_UpNode = 0,
		DTA_MOVEMENT_ATTRIBUTE_DestNode,
		DTA_MOVEMENT_ATTRIBUTE_StreetName,
		DTA_MOVEMENT_ATTRIBUTE_TurnVolume,
		DTA_MOVEMENT_ATTRIBUTE_Lanes,
		DTA_MOVEMENT_ATTRIBUTE_Shared,
		DTA_MOVEMENT_ATTRIBUTE_Width,
		DTA_MOVEMENT_ATTRIBUTE_Storage,
		DTA_MOVEMENT_ATTRIBUTE_StLanes,
		DTA_MOVEMENT_ATTRIBUTE_Grade,
		DTA_MOVEMENT_ATTRIBUTE_Speed,
		DTA_MOVEMENT_ATTRIBUTE_FirstDetect,
		DTA_MOVEMENT_ATTRIBUTE_LastDetect,
		DTA_MOVEMENT_ATTRIBUTE_Phase1,
		DTA_MOVEMENT_ATTRIBUTE_PermPhase1,
		DTA_MOVEMENT_ATTRIBUTE_DetectPhase1,
		DTA_MOVEMENT_ATTRIBUTE_EffectiveGreen,
		DTA_MOVEMENT_ATTRIBUTE_Capacity,
		DTA_MOVEMENT_ATTRIBUTE_VOC,
		DTA_MOVEMENT_ATTRIBUTE_Delay,
		DTA_MOVEMENT_ATTRIBUTE_LOS,
		DTA_MOVEMENT_ATTRIBUTE_IdealFlow,
		DTA_MOVEMENT_ATTRIBUTE_LostTime,
		DTA_MOVEMENT_ATTRIBUTE_SatFlow,
		DTA_MOVEMENT_ATTRIBUTE_SatFlowPerm,
		DTA_MOVEMENT_ATTRIBUTE_SatFlowRTOR,
		DTA_MOVEMENT_ATTRIBUTE_HeadwayFact,
		DTA_MOVEMENT_ATTRIBUTE_Volume,
		DTA_MOVEMENT_ATTRIBUTE_Peds,
		DTA_MOVEMENT_ATTRIBUTE_Bicycles,
		DTA_MOVEMENT_ATTRIBUTE_PHF,
		DTA_MOVEMENT_ATTRIBUTE_Growth,
		DTA_MOVEMENT_ATTRIBUTE_HeavyAgents,
		DTA_MOVEMENT_ATTRIBUTE_BusStops,
		DTA_MOVEMENT_ATTRIBUTE_Midblock,
		DTA_MOVEMENT_ATTRIBUTE_Distance,
		DTA_MOVEMENT_ATTRIBUTE_TravelTime,
		DTA_MOVEMENT_ATTRIBUTE_Street_Name,
		DTA_MOVEMENT_ATTRIBUTE_MAX_ROW
   };



enum DTA_SIG_PHASE
   {
		DTA_SIG_PHASE_TIMING_PLAN_NAME = 0,
		DTA_SIG_PHASE_NODE_ID,
		DTA_SIG_PHASE_KEY,
		DTA_SIG_PHASE_VALUE,
		DTA_SIG_PHASE_D1,
		DTA_SIG_PHASE_D2,
		DTA_SIG_PHASE_D3,
		DTA_SIG_PHASE_D4,
		DTA_SIG_PHASE_D5,
		DTA_SIG_PHASE_D6,
		DTA_SIG_PHASE_D7,
		DTA_SIG_PHASE_D8,
		DTA_SIG_PHASE_D9,
		DTA_SIG_PHASE_D10,
		DTA_SIG_PHASE_D11,
		DTA_SIG_PHASE_D12,
		DTA_SIG_PHASE_D13,
		DTA_SIG_PHASE_D14,
		DTA_SIG_PHASE_D15,
		DTA_SIG_PHASE_D16,
		DTA_SIG_PHASE_D17,
		DTA_SIG_PHASE_D18,
		DTA_SIG_PHASE_P1,
		DTA_SIG_PHASE_P2,
		DTA_SIG_PHASE_P3,
		DTA_SIG_PHASE_P4,
		DTA_SIG_PHASE_P5,
		DTA_SIG_PHASE_P6,
		DTA_SIG_PHASE_P7,
		DTA_SIG_PHASE_P8,
		DTA_SIG_Geometry,
		DTA_SIG_PHASE_MAX_COLUMN
};

enum DTA_SIG_PHASE_ROW
{
	PHASE_GreenDuration=0,
	PHASE_NextPhaseNo,
	PHASE_MOVEMENT_VECTOR,
	PHASE_MOVEMENT_DIR_VECTOR,
	DTA_PHASE_ATTRIBUTE_MAX_ROW};

enum eLinkMOEMode {no_display,lane_volume,speed_kmh, cummulative_volume, oblique_cummulative_volume, link_inflow_volume,link_outflow_volume,link_in_and_outflow_volume,link_travel_time,speed_mph,link_density,link_queue_length_ratio,number_of_queued_Agents,link_traveltime, link_travel_time_plus_prediction, Agent_trajectory,cumulative_SOV_count,cumulative_HOV_count,cumulative_truck_count,cumulative_intermodal_count, energy_miles_per_gallon, emission_CO,emission_CO2,emission_NOX,emission_HC};
enum eLinkDataType { eSimulationData, eFloatingCarData };

class DTA_Movement_Data_Matrix
{
public: 

	string m_AMSMovementData[DTA_MOVEMENT_ATTRIBUTE_MAX_ROW][DTA_SIG_MOVEMENT_MAX_COLUMN];

	float GetValue(DTA_SIG_MOVEMENT movement_index, DTA_SIG_MOVEMENT_ROW attribute_index)
	{
		return atof(m_AMSMovementData[attribute_index][movement_index] .c_str());	
		
	}

	string GetString(DTA_SIG_MOVEMENT movement_index, DTA_SIG_MOVEMENT_ROW attribute_index)
	{
	
		return m_AMSMovementData[attribute_index][movement_index]. c_str();
	}

	void SetValue(DTA_SIG_MOVEMENT movement_index, DTA_SIG_MOVEMENT_ROW attribute_index, float value)
	{

		std::ostringstream ss;
		ss << value;
		std::string s(ss.str());
		m_AMSMovementData[attribute_index][movement_index]  	= s;
		
	}
	void SetString(DTA_SIG_MOVEMENT movement_index, DTA_SIG_MOVEMENT_ROW attribute_index, string s)
	{
		m_AMSMovementData[attribute_index][movement_index] 	= s;
		
	}

	void InitializeKeyValue(int node_id, string geometry_str);

	void AddMovementData()
	{
	
	
	}
	

};

class DTA_Phasing_Data_Matrix
{
	public: 
	string m_AMSPhasingData[DTA_PHASE_ATTRIBUTE_MAX_ROW][DTA_SIG_PHASE_MAX_COLUMN];


	float GetValue(DTA_SIG_PHASE phase_index, DTA_SIG_PHASE_ROW attribute_index)
	{
		return atof(m_AMSPhasingData[attribute_index][phase_index] .c_str());	
		
	}

	CString GetString(DTA_SIG_PHASE phase_index, DTA_SIG_PHASE_ROW attribute_index)
	{
	
		return m_AMSPhasingData[attribute_index][phase_index]. c_str();
	}

	std::string GetSTDString(DTA_SIG_PHASE phase_index, DTA_SIG_PHASE_ROW attribute_index)
	{
	
		return m_AMSPhasingData[attribute_index][phase_index];
	}

	void SetValue(DTA_SIG_PHASE phase_index, DTA_SIG_PHASE_ROW attribute_index, float value)
	{

		std::ostringstream ss;
		ss << value;
		std::string s(ss.str());
		m_AMSPhasingData[attribute_index][phase_index]  	= s;
		
	}
	void SetString(DTA_SIG_PHASE phase_index, DTA_SIG_PHASE_ROW attribute_index, string s)
	{
		m_AMSPhasingData[attribute_index][phase_index] 	= s;
		
	}

	void InitializeKeyValue(int node_id, string geometry_str);

};


#include ".\\cross-resolution-model\\SignalNode.h"

extern float g_GetRandomRatio();

using namespace std;
using std::string;
#define PI 3.1415926
#define MAX_LOS_SIZE  8

#define MAX_RANDOM_SAMPLE_SIZE 100

#define MAX_AdjLinkSize 15
#define	MAX_SPLABEL 99999
#define MAX_NODE_SIZE_IN_A_PATH 4000
#define _MAX_LINK_FOR_LOAD_MOVEMENT_DATA  20000

#define NUM_PATHMOES 8  // Distance, Travel Time, Emissions, Safety
#define NUM_VEHPATHMOES 8  // Distance, Travel Time, Emissions, Safety
#define NUM_PATHS   6
#define MAX_Agent_TYPE_SIZE 10
#define MAX_TIME_INTERVAL_SIZE 96
#define MAX_INFO_CLASS_SIZE 4
#define MAX_DEMAND_TYPE_SIZE  10


#define _MAXIMUM_OPERATING_MODE_SIZE 41
#define MAX_TRAIN_TYPE_SIZE 2
#define TIME_DEPENDENT_TRAVLE_TIME_CALCULATION_INTERVAL 5
#define MAX_DAY_SIZE 1 

extern int 	g_MOEAggregationIntervalInMin;
extern std::string g_SelectedAgentID;
extern std::string  g_SelectedPassengerID;
extern int 	g_ImpactStudyPeriodInMin;
extern bool g_bShowRadarChart;

extern int g_SimulationDayNo;

extern int g_SimulatedLastDayNo;
extern int g_SensorLastDayNo;
extern int g_ImpactThreshold_QueueLengthPercentage;

extern std::map <int,bool> g_SimulatedDayDataMap;
extern  std::map <int,bool> g_SensorDayDataMap;
struct DTA_demand
{
	int from_zone_id;
	int to_zone_id;
	double number_of_Agents;

};


struct GDPoint
{
	double x;
	double y;
};


struct GDRect
{
	double left, right,top, bottom;

	double Height() { return top - bottom; }
	double Width()  { return right - left; }

	bool PtInRect(GDPoint& pt)
	{
		return left <= pt.x && pt.x <= right && bottom <= pt.y && pt.y <= top;
	}

	GDPoint Center(){
		GDPoint pt;
		pt.x = left + (right - left) / 2;
		pt.y = bottom + (top - bottom) / 2;
		return pt;
	}

	void Expand(GDPoint& pt)  // Inflate by a point
	{
		left = min(left, pt.x);
		top = max(top, pt.y);
		right = max(right, pt.x);
		bottom = min(bottom, pt.y);
	}

	void Expand(GDRect& rect)  // Inflate by another Rectangle
	{
		left = min(left, rect.left);
		top = max(top, rect.top);
		right = max(right, rect.right);
		bottom = min(bottom, rect.bottom);
	}

};

struct GDArea
{
	std::vector<GDPoint> m_GDPoint_vector;

	GDPoint GetCenter(std::vector<GDPoint> GDPoint_vector)
	{
		m_GDPoint_vector = GDPoint_vector;

		GDPoint pt;
		pt.x = 0;
		pt.y = 0;

		for(int i = 0 ; i< GDPoint_vector.size(); i++)
		{
		pt.x += GDPoint_vector[i].x;
		pt.y += GDPoint_vector[i].y;
		}
		pt.x = pt.x / max(1,GDPoint_vector.size());
		pt.y = pt.y / max(1,GDPoint_vector.size());

		return pt;
	}

	double left, right,top, bottom;

	double Height() { return top - bottom; }
	double Width()  { return right - left; }

	bool PtInRect(GDPoint& pt)
	{
		return left <= pt.x && pt.x <= right && bottom <= pt.y && pt.y <= top;
	}

	GDPoint Center(){
		GDPoint pt;
		pt.x = left + (right - left) / 2;
		pt.y = bottom + (top - bottom) / 2;
		return pt;
	}

	void Expand(GDPoint& pt)  // Inflate by a point
	{
		left = min(left, pt.x);
		top = max(top, pt.y);
		right = max(right, pt.x);
		bottom = min(bottom, pt.y);
	}

	void Expand(GDRect& rect)  // Inflate by another Rectangle
	{
		left = min(left, rect.left);
		top = max(top, rect.top);
		right = max(right, rect.right);
		bottom = min(bottom, rect.bottom);
	}

};

typedef struct{
	float speed;
	float traveltime;
}struc_traffic_state;


extern double g_GetPoint2Point_Distance(GDPoint p1, GDPoint p2);

extern DTA_Turn g_RelativeAngle_to_Turn(int RelativeAngle);

extern double g_GetPoint2LineDistance(GDPoint pt, GDPoint FromPt, GDPoint ToPt, double UnitFeet = 1, bool no_intersection_requirement = false);
extern double g_CalculateP2PDistanceInMileFromLatitudeLongitude(GDPoint p1, GDPoint p2);
extern bool g_get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y) ;


/////


class DTADemandVolume
{
public:
	std::map<int, float> TypeValue;

	float total_demand;
	float travel_cost;
	float friction;

	DTADemandVolume()
	{
	travel_cost = 1;
	friction = 1;
	total_demand = 0;
	}

	void SetValue(int demand_type, float value)
	{
	TypeValue[demand_type]= value;
	total_demand += value;
	}

	float GetValue(int demand_type)
	{
	if( TypeValue.find(demand_type) != TypeValue.end())
		return TypeValue[demand_type];
	else
		return 0;
	}

	float GetSubTotalValue()
	{
	return total_demand;
	}


};


class DTAZone
{ 
public:

	string notes;
	string color_code;
	float  m_Height;

	bool m_bWithinSubarea;

	std::vector<GDPoint> m_ShapePoints;

	int m_ZoneID;
	int m_OriginAgentSize;  // number of Agents from this origin, for fast acessing

	bool bInitialized;
	GDPoint m_Center;

	GDPoint GetCenter()
	{
		if(bInitialized)
			return m_Center;
		else
		{  // not initialized yet
			m_Center.x = 0;
			m_Center.y = 0;
	
		bInitialized = true;
				return m_Center;
		}

	}

	
	


	bool FindANode(int NodeID, int External_OD_flag =0)
	{


	
		return false;
	}

	int m_DSPZoneNo;
	DTAZone()
	{
		m_DSPZoneNo = 0;

		m_Height = 0;
		color_code = "red";

		m_Demand = 0;
		m_OriginAgentSize = 0;
		bInitialized = false;



		m_bWithinSubarea = true;

	}

	BOOL IsInside(GDPoint pt)
	{

	double MinX  = pt.x ; 
	double MinY  = pt.y;

	double MaxX  = pt.x ; 
	double MaxY  = pt.y;

  int num_points = (int)(m_ShapePoints.size());
  for(unsigned i = 0; i < num_points; i++)
   {
     MinX = min(m_ShapePoints[i].x, MinX);
     MinY = min(m_ShapePoints[i].y, MinY);

     MaxX = max(m_ShapePoints[i].x, MaxX);
     MaxY = max(m_ShapePoints[i].y, MaxY);

  }

  double resolution_x = 1000/max(0.0000001,(MaxX - MinX));
  double resolution_y = 1000/max(0.0000001,(MaxY - MinY));

   LPPOINT zpts = new POINT[num_points];

   for(unsigned i = 0; i < num_points; i++)
   {
      zpts[i].x =  (int)((m_ShapePoints[i].x - MinX)*resolution_x+0.5);
      zpts[i].y =  (int)((m_ShapePoints[i].y - MinY)*resolution_y+0.5);
   }

   // Create a polygonal region
   HRGN hrgn = CreatePolygonRgn(zpts, num_points, WINDING);

   POINT current_point;
     current_point.x =  (int)((pt.x - MinX)*resolution_x+0.5);
      current_point.y =  (int)((pt.y - MinY)*resolution_y+0.5);


    BOOL bInside =  PtInRegion(hrgn, current_point.x , current_point.y );

	if(zpts!=NULL)
		delete zpts;

	return bInside;
	}

	float m_Demand;


};

class DTADemand
{
public:
	int from_zone_id;
	int to_zone_id;
	int starting_time_in_min;
	int ending_time_in_min;
	std::vector<float> number_of_Agents_per_demand_type;

	DTADemand()
	{


	}	

};




class DTATimingPlan
{
	public:
	int start_time_in_min;
	int end_time_in_min;
	string timing_plan_name;


	DTATimingPlan()
	{
	start_time_in_min = 0;
	end_time_in_min = 1440;
	timing_plan_name = "0";

	}


};


class DTADemandType
{
public:
	int demand_type;
	float average_VOT;

	DTADemandType()
	{
		average_VOT = 10;
	}

	void SetDefaultAgentTypeDistribution()
	{
	
	}

	CString demand_type_name;
};

class DTALinkType
{
public:
	DTALinkType()
	{
	capacity_adjustment_factor = 1;

	travel_time_bias_factor = 1;
	saturation_flow_rate_in_vhc_per_hour_per_lane = 1800;
	approximate_cycle_length_in_second = 0;
	link_type = 0;
	default_lane_capacity = 1000;  // per hour per lane
	default_speed = 50;
	default_number_of_lanes = 2;
	}

	float capacity_adjustment_factor;
	float approximate_cycle_length_in_second;
	float saturation_flow_rate_in_vhc_per_hour_per_lane;
	float travel_time_bias_factor; 
	float default_lane_capacity;
	float default_speed;
	int default_number_of_lanes;
	int link_type;
	string link_type_name;
	string type_code;
	int safety_prediction_model_id;
	bool IsFreeway()
	{
		if(type_code.find('f')!= string::npos)
			return true;
		else
			return false;
	}

	bool IsHighway()
	{
		if(type_code.find('h')!= string::npos)
			return true;
		else
			return false;
	}

	bool IsArterial()
	{
		if(type_code.find('a')!= string::npos)
			return true;
		else
			return false;
	}

	bool IsRamp()
	{
		if(type_code.find('r')!= string::npos)
			return true;
		else
			return false;
	}

	bool IsConnector()
	{
		if(type_code.find('c')!= string::npos)
			return true;
		else
			return false;
	}

	bool IsTransit()
	{
		if(type_code.find('t')!= string::npos)
			return true;
		else
			return false;
	}
	bool IsWalking()
	{
		if(type_code.find('w')!= string::npos)
			return true;
		else
			return false;
	}
	bool IsPickup()
	{
		if (type_code.find('p') != string::npos)
			return true;
		else
			return false;
	}
	bool IsDropoff()
	{
		if (type_code.find('d') != string::npos)
			return true;
		else
			return false;
	}
};

class DTANodeType
{
public:
	int node_type;
	string node_type_name;
	int pretimed_signal_flag;
	int actuated_signal_flag;
	int stop_sign_flag;
	int yield_sign_flag;
};


class DTANodeMovement
{
public:

	DTANodeMovement()
	{
	bOverlappingTurnFlag = false;

	pair_key ="";
	starting_time_in_min = 0;
	ending_time_in_min = 1440;
	turning_percentage = 0;
	turning_prohibition_flag = 0;
	turning_protected_flag = 0;
	turning_permitted_flag = 0;
	phase_index = 0;
	
	movement_hourly_capacity = 10000;

	for(int h = 0; h<24; h++)
		HourlyCount[h] = 0;


sim_turn_count = 0;
sim_turn_percentage = 0;
sim_turn_delay = 0; 

obs_turn_count = 0;
obs_turn_hourly_count = 0;
obs_turn_percentage = 0; 
obs_turn_delay = 0; 


   QEM_TurnVolume = 0;
   QEM_LinkVolume = 0;
   QEM_Lanes = 0;
   QEM_Shared = 0;
   QEM_Width = 12; 
   QEM_Storage = 0;
   QEM_StLanes = 0;
   QEM_Grade = 0;
   QEM_Speed = 40;
   QEM_IdealFlow = 1900;
   QEM_LostTime = 4;
   QEM_Phase1 = 0;
   QEM_PermPhase1 = 0;
   QEM_DetectPhase1 = 0;

   QEM_TurnPercentage = 0;
   QEM_EffectiveGreen = 0;
   QEM_Capacity = 0;
   QEM_VOC = 0;
   QEM_SatFlow = 0;
   QEM_Delay = 0;
   QEM_reference_node_number = 0;
   angle = -100;
   QEM_StartTime = 0;
   QEM_EndTime = 0;


   bNonspecifiedTurnDirectionOnFreewayAndRamps = false;
	}

std::vector<GDPoint> m_ShapePoints;

bool bNonspecifiedTurnDirectionOnFreewayAndRamps;
CString pair_key;
int HourlyCount[24];


int IncomingLinkNo;
int OutgoingLinkNo;
DTA_Turn movement_turn;
DTA_Direction movement_direction;
int angle;
DTA_SIG_MOVEMENT movement_approach_turn;
bool bOverlappingTurnFlag;
string QEM_dir_string;

int QEM_reference_node_number;

int in_link_from_node_id;
int in_link_to_node_id;  
int out_link_to_node_id;

int movement_hourly_capacity;
int starting_time_in_min;
int ending_time_in_min;
float turning_percentage;
int turning_prohibition_flag;
int turning_protected_flag;
int turning_permitted_flag;


float sim_turn_count; 
float sim_turn_percentage; 
float sim_turn_delay; 

int obs_turn_count; 
int obs_turn_hourly_count;
float obs_turn_percentage; 
int obs_turn_delay; 

   int QEM_TurnVolume;
   int QEM_LinkVolume;
   float QEM_TurnPercentage;

   int QEM_Lanes;
   int QEM_Shared;
   int QEM_Width;
   int QEM_Storage;
   int QEM_StLanes;
   float QEM_Grade;
   float QEM_Speed;
   float QEM_IdealFlow;
   float QEM_LostTime;
   int QEM_Phase1;
   int QEM_PermPhase1;
   int QEM_DetectPhase1;

   float QEM_EffectiveGreen;
   float QEM_Capacity;
   float QEM_VOC;
   float QEM_SatFlow;
   float QEM_Delay;
   CString QEM_LOS;


   float QEM_StartTime;
   float QEM_EndTime;


 //  std::map<int,bool> m_PhaseMap;

int phase_index;

};

class DTANodeLaneTurn
{
public:
	DTANodeMovement movement_info;

int in_lane_index;
int out_lane_index;

};


  class DTANodePhase
   {
   public:
      DTANodePhase()
	  {
      min_green = 0;
	  max_green = 0;
	  amber = 0;
	  VehExt1 = 0;
	  }

      int phase_number;
      int min_green;
	  int max_green;
	  int amber;
	  float VehExt1;


      std::map<CString,bool> movement_index_map;
      std::vector<int> movement_type_vector;  // prohibited, permitted, protected, free


	  // if a link is added or deleted from a link, the corresponding movement array should be adjusted. 


	};

extern bool compare_MovementData (DTANodeMovement first, DTANodeMovement second);



class DTANodeMovementVector {
public:
	std::vector <DTANodeMovement> m_MovementVector;
};

class DTANode
{
public:
	DTANode()
	{
		m_withSignalPlans = false;

		m_bCreatedbyNEXTA = false;
		m_bConnectedToFreewayORRamp = false;
		m_bNodeAvoidance  = false;
		m_IntermediateDestinationNo = 0;
		m_NodeProduction =0;
		m_NodeAttraction =0;

		m_NodeID = 0;
		m_NodeOriginalNumber = -1;
		m_ControlType = 0;
		m_ZoneID = 0;

		m_Connections = 0;
		m_LayerNo = 0;
		m_DistanceToRoot = 0;
		m_NumberofPhases = 0;
		m_bSignalData = false;
		m_External_OD_flag = 0;
		m_tobeRemoved = false;
		m_bSubareaFlag = 0;  // when the associated link is removed as it is outside the boundary, then we mark its from and t nodes as subarea boundary node 
		m_CentroidUpdateFlag = 0;

		m_IncomingNonConnectors = 0;

	

	};


	bool m_withSignalPlans;

	bool m_bCreatedbyNEXTA;  // not by users

	~DTANode(){};

	bool m_bNodeAvoidance;
	float m_NodeProduction;
	float m_NodeAttraction;

	int m_CentroidUpdateFlag;  // used by node splitting for subarea
	int m_bSubareaFlag;
	bool m_tobeRemoved;
	bool m_bZoneActivityLocationFlag; 

	int m_External_OD_flag;

	std::vector<int> m_OutgoingLinkVector;
	std::vector<int> m_IncomingLinkVector;
	
	int m_IncomingNonConnectors;

	DTANodeMovementVector m_MovementDataMap;  // key is the timing plan name; default is FREE


	void SetupNodeMovementVector(DTANodeMovement movement_element)
	{
	
		m_MovementDataMap.m_MovementVector.push_back(movement_element);
		
	}

	DTANodeMovement GetNodeMovementVector( int MovementIndex)
	{
			// FREE as the backup value
			return m_MovementDataMap.m_MovementVector[MovementIndex];
		
	}

	bool IsEmptyPhaseNumber()
	{
		for(unsigned int i  = 0; i < m_MovementDataMap.m_MovementVector.size(); i++)
		{
			if(m_MovementDataMap.m_MovementVector[i].QEM_Phase1 >0)
			{
				return false;
			}
		}

		return true;
	
	}


	void ResetToDefaultPhaseNumbers()
	{
		for(unsigned int i  = 0; i < m_MovementDataMap. m_MovementVector.size(); i++)
		{
			switch(m_MovementDataMap.m_MovementVector[i].movement_direction)
			{
			case DTA_North: m_MovementDataMap.m_MovementVector[i].QEM_Phase1 = 2; 
				break;
			case DTA_South: m_MovementDataMap.m_MovementVector[i].QEM_Phase1 = 6; 
				break;
			case DTA_East: m_MovementDataMap.m_MovementVector[i].QEM_Phase1 = 4; 
				break;
			case DTA_West: m_MovementDataMap.m_MovementVector[i].QEM_Phase1 = 8; 
				break;
			}
	
		}
	}
	void SortMovementVector()
	{

	std::sort(m_MovementDataMap.m_MovementVector.begin(), m_MovementDataMap.m_MovementVector.end(), compare_MovementData);
	
	
	}

	int FindMovementIndexFromDirecion(DTA_SIG_MOVEMENT movement_approach_turnection)
	{
	
		for(unsigned int i  = 0; i < m_MovementDataMap.m_MovementVector.size(); i++)
		{
			if(m_MovementDataMap.m_MovementVector[i].movement_approach_turn == movement_approach_turnection)
				return i;

		}

		return -1;
	}

	int FindHourlyCountFromDirection( DTA_Direction movement_approach)
	{
	
		int link_count = 0;
		// sum up all movement along the same approach
		for(unsigned int i  = 0; i < m_MovementDataMap.m_MovementVector.size(); i++)
		{
			if(m_MovementDataMap.m_MovementVector[i].movement_direction   == movement_approach)
				link_count+= m_MovementDataMap.m_MovementVector[i].QEM_TurnVolume;

		}

		return link_count;
	}

	CString pair_key;

	CString get_link_pair_key(int in_link_from_node_id, int out_link_to_node_id)
	{

	pair_key.Format ("%d,%d",in_link_from_node_id,out_link_to_node_id);
		

	return pair_key;
	}


	void make_Link_Pair_to_Movement_Map()
	{

		for(unsigned int i  = 0; i < m_MovementDataMap.m_MovementVector.size(); i++)
		{
			CString pair_key = get_link_pair_key( m_MovementDataMap.m_MovementVector[i].in_link_from_node_id,m_MovementDataMap.m_MovementVector[i].out_link_to_node_id);

			m_Link_Pair_to_Movement_Map[pair_key] = i;

			if(i <  m_MovementDataMap.m_MovementVector.size())
			{
			m_MovementDataMap.m_MovementVector[i].pair_key = pair_key;
			}else
			{
			TRACE("");
			
			}
		}

	}

	void ResetMovementMOE()
	{

		for(unsigned int i  = 0; i < m_MovementDataMap.m_MovementVector.size(); i++)
		{

			m_MovementDataMap.m_MovementVector[i].sim_turn_count  = 0;
			m_MovementDataMap.m_MovementVector[i].turning_percentage   = 0;
			m_MovementDataMap.m_MovementVector[i].sim_turn_delay = 0;
		}

	}

	std::map<CString, int > m_Link_Pair_to_Movement_Map;

	void AddMovementCountAndDelay(int in_link_from_node_id, int out_link_to_node_id, float delay)
	{
	
		if(m_Link_Pair_to_Movement_Map.size()==0)
			make_Link_Pair_to_Movement_Map();

		if(m_MovementDataMap.m_MovementVector.size()==0)
			return;

		CString link_pair_key = get_link_pair_key( in_link_from_node_id,out_link_to_node_id);

		if(m_Link_Pair_to_Movement_Map.find(link_pair_key) == m_Link_Pair_to_Movement_Map.end())
			return;

		int movement_index = m_Link_Pair_to_Movement_Map[link_pair_key];

		
		if(movement_index>=0 && movement_index < m_MovementDataMap.m_MovementVector.size())
		{
		m_MovementDataMap.m_MovementVector[movement_index].sim_turn_count++;
		m_MovementDataMap.m_MovementVector[movement_index].sim_turn_delay+=delay;
		}


	}


	

	CString GetMovementIndex(int in_link_from_node_id, int in_link_to_node_id, int out_link_to_node_id)
	{
		if(m_Link_Pair_to_Movement_Map.size()==0) //movement vector empy
			make_Link_Pair_to_Movement_Map();

		return get_link_pair_key(in_link_from_node_id, out_link_to_node_id);

	//	return "1";  //not found

	}


	int GetMovementNo(int in_link_from_node_id, int in_link_to_node_id, int out_link_to_node_id)
	{

		for(unsigned int i  = 0; i < m_MovementDataMap.m_MovementVector.size(); i++)
		{

		if(m_MovementDataMap. m_MovementVector[i].in_link_from_node_id== in_link_from_node_id
		&& m_MovementDataMap.m_MovementVector[i].in_link_to_node_id== in_link_to_node_id
		&& m_MovementDataMap.m_MovementVector[i].out_link_to_node_id== out_link_to_node_id)
		return i;
		}

		return -1;  //not found

	}

	std::vector <DTANodeLaneTurn> m_LaneTurnVector;

	
	bool m_bConnectedToFreewayORRamp;
	std::string stop_id, transit_demand_type_code;

	int m_NumberofPhases;
	float m_DistanceToRoot;
	string m_Name;
	GDPoint pt;


	int m_GateFlag;
	int m_AreaCode;

	GDPoint UTDF_pt;
	int UTDF_node_id;
	int UTDF_control_type;


	int m_IntermediateDestinationNo;  ///id, starting from zero, continuous sequence

	bool m_bSignalData;
	int m_LayerNo;
	int m_NodeID;  //  original node number
	int m_NodeOriginalNumber;  //  original node number
	int m_NodeNo;  ///id, starting from zero, continuous sequence
	int m_ZoneID;  // If ZoneID > 0 --> centroid,  otherwise a physical node.
	int m_ControlType; // Type: ....
	int m_NodeType;
	string m_node_type;
	string m_ctrl_type;

	int m_Connections;  // number of connections

	//DTA_NodeMovementSet m_Movementt;
	// signal data

};

class DTAPoint
{
public:
	DTAPoint(){
		m_NodeID = 0;
		m_ControlType = 0;
		m_ZoneID = 0;
		m_TotalCapacity = 0;
		m_Connections = 0;
		m_LayerNo = 0;
		m_DistanceToRoot = 0;
	};
	~DTAPoint(){};


	float m_DistanceToRoot;
	string m_Name;
	GDPoint pt;
	int m_LayerNo;
	int m_NodeID;  //  original node number
	int m_NodeNo;  ///id, starting from zero, continuous sequence
	int m_ZoneID;  // If ZoneID > 0 --> centroid,  otherwise a physical node.
	int m_ControlType; // Type: ....
	float m_TotalCapacity;
	int m_Connections;  // number of connections

};

class DTALine
{
public:

	DTALine ()
	{
		line_type = 0;
		direction = 1;
	}

	int line_type;
	int direction;

	std::string m_geo_string;
	std::string m_LineID;
	std::vector<GDPoint> m_ShapePoints;

	std::vector<float> m_ShapePointRatios;
	void CalculateShapePointRatios()
	{

		m_ShapePointRatios.clear();

		float total_distance = 0; 
		unsigned int si;

		if(m_ShapePoints.size()==0)
			return;

		for(si = 0; si < m_ShapePoints .size()-1; si++)
		{
			total_distance += g_GetPoint2Point_Distance(m_ShapePoints[si],m_ShapePoints[si+1]); 
		}

		if(total_distance < 0.0000001f)
			total_distance = 0.0000001f;

		float distance_ratio = 0;
		float P2Origin_distance = 0;
		m_ShapePointRatios.push_back(0.0f);
		for(si = 0; si < m_ShapePoints .size()-1; si++)
		{
			P2Origin_distance += g_GetPoint2Point_Distance(m_ShapePoints[si],m_ShapePoints[si+1]);
			m_ShapePointRatios.push_back(P2Origin_distance/total_distance);
		}
	}
	GDPoint GetRelativePosition(float ratio)
	{

		GDPoint Pt;
		Pt.x = 0;
		Pt.y = 0;


		Pt.x = (m_ShapePoints[0].x + m_ShapePoints[m_ShapePoints.size() - 1].x) / 2;
		Pt.y = (m_ShapePoints[0].y + m_ShapePoints[m_ShapePoints.size() - 1].y) / 2;

		unsigned	int si;

		if (m_ShapePointRatios.size() == m_ShapePoints.size())
		{

			for (si = 0; si < m_ShapePoints.size() - 1; si++)
			{

				if (ratio > m_ShapePointRatios[si] && ratio < m_ShapePointRatios[si + 1])
				{

					float SectionRatio = m_ShapePointRatios[si + 1] - m_ShapePointRatios[si];

					float RelateveRatio = 0;
					if (SectionRatio >0)
						RelateveRatio = (ratio - m_ShapePointRatios[si]) / SectionRatio;

					Pt.x = m_ShapePoints[si].x + RelateveRatio*(m_ShapePoints[si + 1].x - m_ShapePoints[si].x);
					Pt.y = m_ShapePoints[si].y + RelateveRatio*(m_ShapePoints[si + 1].y - m_ShapePoints[si].y);

					return Pt;
				}
			}

		}
		return Pt;
	}
};
// event structure in this "event-basd" traffic simulation
typedef struct{
	int veh_id;
	float time_stamp;
}struc_Agent_item;



class SLinkMOE  // time-dependent link MOE
{
public:
	float QueueLength;
	float TravelTime;

	float Speed;  // speed
	float LinkFlow;   // flow volume
	float Density;   // Density

	float ArrivalCumulativeFlow;   // flow volume
	float DepartureCumulativeFlow;   // flow volume

	float AgentInflowCount;
	float AgentOutflowCount;

	int number_of_queued_Agents;

	//   Density can be derived from CumulativeArrivalCount and CumulativeDepartureCount
	//   Flow can be derived from CumulativeDepartureCount
	//   AvgTravel time can be derived from CumulativeArrivalCount and TotalTravelTime

	SLinkMOE()
	{

		number_of_queued_Agents  = 0;
		

		AgentInflowCount = 0;
		AgentOutflowCount = 0;

		QueueLength = 0;
		TravelTime = 0;
		Speed = 0;
		LinkFlow = 0;
		ArrivalCumulativeFlow = 0;
		DepartureCumulativeFlow = 0;
		Density = 0;

				
		// these four copies are used to compare simulation results and observed results
		Speed = 0;
		LinkFlow = 0;
		Density = 0;

	};

	void SetupMOE(float FreeFlowTravelTime, float SpeedLimit)
	{
		AgentInflowCount = 0;
		AgentOutflowCount = 0;
	
		QueueLength = 0;
		TravelTime = FreeFlowTravelTime;

		Speed = SpeedLimit;
		LinkFlow = 0;
		ArrivalCumulativeFlow = 0;
		Density = 0;


	}

} ;



class CapacityReduction
{
public:
	CapacityReduction()
	{
		StartDayNo = 1;
		EndDayNo = 1;
		ScenarioNo = 0;
		StartTime = 0;
		EndTime = 1440;

		SpeedLimit = -1; // no change
		LaneClosurePercentage = 0;

		for (int i=0;i<MAX_RANDOM_SAMPLE_SIZE;i++)
		{
			CapacityReductionSamples[i] = 0.f;
			AdditionalDelaySamples[i] = 0.f;
		}

		COVRegularCapacityReduction = 0.1f;

		bWorkzone = false;
		bIncident = false;
		bGenericTrafficControl = false;
	}

	bool bWorkzone;
	bool bIncident;
	bool bGenericTrafficControl;

	float StartTime;
	float EndTime;
	int StartDayNo;
	int ScenarioNo;
	int EndDayNo;
	float LaneClosurePercentage;
	float SpeedLimit;
	float ServiceFlowRate;

	float MeanRegularCapacityReduction;  // from link data
	float COVRegularCapacityReduction;


	float MeanIncidentCapacityReduction;
	float COVIncidentCapacityReduction;

	float ProbabilityIncidentOccurance;

	float CapacityReductionSamples[MAX_RANDOM_SAMPLE_SIZE];
	float AdditionalDelaySamples[MAX_RANDOM_SAMPLE_SIZE];

	void GenerateAdditionalDelayDistribution(float EntranceTime,int AgentID);

};

class DTALink
{
public:


	DTALink(int TimeHorizon)  // TimeHorizon's unit: per min
	{
		m_Length = 10;
		m_network_design_flag = 0;
	
		m_total_assigned_link_volume = 0;
		m_total_link_volume_of_incomplete_trips = 0;

		m_UserDefinedHeight = 1;
		relative_angel_difference_from_main_direction = 0;

		m_AdditionalCost = 0;
		m_CentroidUpdateFlag = 0; 
		m_bTrainFromTerminal = false;
		m_bTrainToTerminal = false;
		m_bConnector = false;
		m_bTransit = false;
	    m_bWalking = false;
		m_ConnectorZoneID = 0;

		m_NumberOfLeftTurnLanes = 0;
		m_NumberOfRightTurnLanes = 0;
		m_LeftTurnLaneLength = 0;	
		m_RightTurnLaneLength = 0;	

		m_LeftTurnCapacity = 0;

		m_bOneWayLink = true;
		m_BandWidthValue = 1;
		m_ReferenceBandWidthValue = 0;
		m_SetBackStart = 0;
		m_SetBackEnd = 0;
		m_FreeSpeed  = 50;
		m_SpeedAtCapacity = 30;
		m_ReversedSpeedLimit  = 50;
		m_Saturation_flow_rate_in_vhc_per_hour_per_lane = 1800;

		m_total_link_volume = 0;
		m_total_travel_time = 0;
		m_total_delay = 0;

		m_TotalVolumeForMovementCount = 0;
		m_MeanSpeed  = m_FreeSpeed;
		m_TotalTravelTime = 0;
		m_TotalDiffValue = 0;
		m_NumberOfMarkedAgents = 0;

		m_LayerNo = 0;
		m_GateFlag = 0;
		m_ParentLinkID = -1;
		m_OrgDir = 1;
		m_RailBidirectionalFlag = 1;
		m_Direction = 1;
		m_SimulationHorizon	= TimeHorizon;

		m_StochaticCapcityFlag = 0;
		m_bMergeFlag = 0;
		m_bOnRampType =  m_bOffRampType = false;
		m_MergeOnrampLinkID = -1;
		m_MergeMainlineLinkID = -1;
		m_OverlappingCost = 0;
		m_DisplayLinkID = -1;

		m_Kjam = 180;
		m_Wave_speed_in_mph = 12;

		m_LinkMOEAry = NULL;
		m_LinkMOEArySize = 0;


		m_prohibited_u_turn = 0;

		m_MinSpeed = 40;
		m_MaxSpeed = 40;

		m_LevelOfService = 'A';
		m_avg_waiting_time_on_loading_buffer = 0;

		m_avg_simulated_speed = 0;
		m_total_link_volume = 0;
		m_total_travel_time = 0;
		m_volume_over_capacity_ratio  = 0;
		m_volume_over_capacity_ratio = 0;
		m_StaticTravelTime = 0;
		m_Grade = 0;

		input_line_no = 0;

		m_bIncludedBySelectedPath = false;
		m_bIncludedinSubarea = false;
		m_bIncludedinBoundaryOfSubarea = false;

		m_bFirstPathLink = false;
		m_bLastPathLink = false;

		CFlowArrivalCount = 0;
		CFlowDepartureCount = 0;

	
		m_bToBeShifted = true;

	m_BPR_alpha_term = 0.15f;
	m_BPR_beta_term = 4.0f;


	};


	
	long m_FromNodeNumber;
	long m_ToNodeNumber;
	long m_FromNodeID;  // index starting from 0
	long m_ToNodeID;    // index starting from 0
	bool m_bSensorData;

	std::string m_CountSensorID;
	float m_BPR_alpha_term;
	float m_BPR_beta_term;


	float m_UserDefinedHeight;



	
	float m_StaticTravelTime;
	int m_CentroidUpdateFlag;

	int  m_NumberOfMarkedAgents;
	float m_BandWidthValue;
	float m_ReferenceBandWidthValue;

	bool m_bIncludedBySelectedPath;
	bool m_bIncludedinSubarea;
	bool m_bIncludedinBoundaryOfSubarea;
	

	bool m_bFirstPathLink;
	bool m_bLastPathLink;


	int m_LayerNo;
	float m_Grade;
	string m_Name; 
	string m_loop_code;
	string m_orientation_code;



	// overall information

	float m_total_link_volume;
	float m_total_travel_time;
	float m_total_delay;
	float m_total_assigned_link_volume;
	float m_total_link_volume_of_incomplete_trips;

	float m_volume_over_capacity_ratio;
	std::string m_LevelOfService;
	float m_avg_waiting_time_on_loading_buffer;
	float m_avg_simulated_speed;
	// end of overall information

	int input_line_no;

	std::vector<GDPoint> m_ShapePoints;
	std::vector<GDPoint> m_Original_ShapePoints;  // original shape points from GIS (without offset)


	bool m_bToBeShifted;

	std::vector<GDPoint> m_BandLeftShapePoints;
	std::vector<GDPoint> m_BandRightShapePoints;

	std::vector<GDPoint> m_ReferenceBandLeftShapePoints;  // second band for observations
	std::vector<GDPoint> m_ReferenceBandRightShapePoints;

	std::vector<float> m_ShapePointRatios;


	void UpdateShapePointsBasedOnEndPoints(double lane_offset)
	{
		m_ShapePoints.clear();
		m_BandLeftShapePoints.clear();
		m_BandRightShapePoints.clear();
		
		m_ShapePoints.push_back(m_FromPoint);
		m_ShapePoints.push_back(m_ToPoint);


		m_Original_ShapePoints.clear();
		m_Original_ShapePoints.push_back(m_FromPoint);
		m_Original_ShapePoints.push_back(m_ToPoint);

		CalculateShapePointRatios();

		unsigned int last_shape_point_id = m_ShapePoints .size() -1;
		double DeltaX = m_ShapePoints[last_shape_point_id].x - m_ShapePoints[0].x;
		double DeltaY = m_ShapePoints[last_shape_point_id].y - m_ShapePoints[0].y;
		double theta = atan2(DeltaY, DeltaX);

		for(unsigned int si = 0; si < m_ShapePoints .size(); si++)
		{
			GDPoint pt;

			pt.x = m_ShapePoints[si].x - lane_offset* cos(theta-PI/2.0f);
			pt.y = m_ShapePoints[si].y - lane_offset* sin(theta-PI/2.0f);

			m_BandLeftShapePoints.push_back (pt);

			pt.x  = m_ShapePoints[si].x + max(1,m_NumberOfLanes - 1)*lane_offset* cos(theta-PI/2.0f);
			pt.y = m_ShapePoints[si].y + max(1,m_NumberOfLanes - 1)*lane_offset* sin(theta-PI/2.0f);

			m_BandRightShapePoints.push_back (pt);
		}
	}
	void CalculateShapePointRatios()
	{

		m_ShapePointRatios.clear();

		float total_distance = 0; 
		unsigned int si;

		if(m_ShapePoints.size()==0)
			return;

		for(si = 0; si < m_ShapePoints .size()-1; si++)
		{
			total_distance += g_GetPoint2Point_Distance(m_ShapePoints[si],m_ShapePoints[si+1]); 
		}

		if(total_distance < 0.0000001f)
			total_distance = 0.0000001f;

		float distance_ratio = 0;
		float P2Origin_distance = 0;
		m_ShapePointRatios.push_back(0.0f);
		for(si = 0; si < m_ShapePoints .size()-1; si++)
		{
			P2Origin_distance += g_GetPoint2Point_Distance(m_ShapePoints[si],m_ShapePoints[si+1]);
			m_ShapePointRatios.push_back(P2Origin_distance/total_distance);
		}
	}



	//for timetabling use
	std::map<long, long> m_RuningTimeMap;  //indexed by train type

	int GetTrainRunningTime(int TrainType)
	{
		if(m_LaneCapacity<0.001)
			return 1440;

		map <long, long> :: iterator mIter  = m_RuningTimeMap.find(TrainType);

		if ( mIter == m_RuningTimeMap.end( ) )
			return 1440; // very large number for prohibited train type, one day
		else
			return  mIter -> second;  // return the running time value matching the train type
	}



	void ResetMOEAry(int TimeHorizon)
	{
		if(m_LinkMOEAry == NULL)
			delete m_LinkMOEAry;

		m_LinkMOEAry = new SLinkMOE [TimeHorizon];

		m_LinkMOEArySize = TimeHorizon;


	};


	bool m_bTrainFromTerminal;
	bool m_bTrainToTerminal;

	
	GDPoint m_FromPoint, m_ToPoint;
	GDPoint m_ScheduleFromPoint, m_ScheduleToPoint;

	double m_SetBackStart, m_SetBackEnd;
	GDPoint m_FromPointWithSetback, m_ToPointWithSetback;

	double DefaultDistance()
	{
		if(m_ShapePoints.size() == 0)   // no shape point info
		{
		return pow((m_FromPoint.x - m_ToPoint.x)*(m_FromPoint.x - m_ToPoint.x) + 
			(m_FromPoint.y - m_ToPoint.y)*(m_FromPoint.y - m_ToPoint.y),0.5);
		}else
		{

		double distance = 0;
		for(unsigned int si = 0; si < m_ShapePoints .size()-1; si++)
		{
			distance  +=  pow((m_ShapePoints[si].x - m_ShapePoints[si+1].x)*(m_ShapePoints[si].x - m_ShapePoints[si+1].x) + 
			(m_ShapePoints[si].y - m_ShapePoints[si+1].y)*(m_ShapePoints[si].y - m_ShapePoints[si+1].y),0.5);
		}

		return distance;
		}

	}

void AdjustLinkEndpointsWithSetBack()
{
   GDPoint Direction;

   double SetBackRatio = m_SetBackStart  /max(0.00001, DefaultDistance());

   Direction.x = (m_ToPoint.x - m_FromPoint.x)*SetBackRatio;
   Direction.y = (m_ToPoint.y - m_FromPoint.y)*SetBackRatio;

   // Adjust start location by this vector
   m_FromPointWithSetback.x = m_ShapePoints[0].x + Direction.x;
   m_FromPointWithSetback.y = m_ShapePoints[0].y + Direction.y;

   SetBackRatio = m_SetBackEnd  /max(0.000001, DefaultDistance());

   Direction.x = (m_FromPoint.x - m_ToPoint.x)*SetBackRatio;
   Direction.y = (m_FromPoint.y - m_ToPoint.y)*SetBackRatio;

   // Adjust start location by this vector
   m_ToPointWithSetback.x = m_ShapePoints[m_ShapePoints.size()-1].x + Direction.x;
   m_ToPointWithSetback.y = m_ShapePoints[m_ShapePoints.size()-1].y + Direction.y;

}
	float 	GetImpactedFlag(int DepartureTime = -1, bool bWorkZone = true)
	{ // if DepartureTime < -1, then we ignore the depature time attribute and show that any way
		for(unsigned int il = 0; il< CapacityReductionVector.size(); il++)
		{

		if( bWorkZone&&CapacityReductionVector[il].bWorkzone || !bWorkZone&&CapacityReductionVector[il].bIncident )
			{
				if(DepartureTime<=-1 || (DepartureTime >= CapacityReductionVector[il].StartTime && DepartureTime<=CapacityReductionVector[il].EndTime ))
						return CapacityReductionVector[il].LaneClosurePercentage;
			}
		}

		return 0;
	}




	SLinkMOE* m_LinkMOEAry; 

	int m_LinkMOEArySize;


	std::vector<CapacityReduction> CapacityReductionVector;


	int m_bMergeFlag;  // 1: freeway and freeway merge, 2: freeway and ramp merge
	bool m_bOnRampType, m_bOffRampType;
	int m_MergeOnrampLinkID;
	int m_MergeMainlineLinkID;

	
	~DTALink(){

		if(m_LinkMOEAry == NULL)
			delete m_LinkMOEAry;


	};

	float GetFreeMovingTravelTime(int TrafficModelFlag=2, float Time = -1)
	{
		if(TrafficModelFlag ==0) // BRP model
			return m_StaticTravelTime;
		else 
		{
			for(unsigned int il = 0; il< CapacityReductionVector.size(); il++)
			{
				if(Time>=CapacityReductionVector[il].StartTime && Time<=CapacityReductionVector[il].EndTime)
				{
					return m_Length/CapacityReductionVector[il].SpeedLimit*60.0f;  // convert from hour to min;
				}
			}
			return m_FreeFlowTravelTime;
		}
	}

	void SetupMOE()
	{
		m_JamTimeStamp = (float) m_SimulationHorizon;
		m_FreeFlowTravelTime = m_Length/m_FreeSpeed*60.0f;  // convert from hour to min
		m_StaticTravelTime = m_FreeFlowTravelTime;

		CFlowArrivalCount = 0;
		CFlowDepartureCount = 0;

		int t;


	}
	int m_LinkNo;
	
	int From_xGridNo;
	int From_yGridNo;
	int From_xyGridNo;

	int To_xGridNo;
	int To_yGridNo;
	int To_xyGridNo;

	int parent_link_id;

	int m_GateFlag;  /// 0 non gate: 1: low rank: 2: high rank
	int m_ParentLinkID;
	int m_OrgDir;
	CString m_LinkKey;

	int m_RailBidirectionalFlag;
	string m_TrackType;
	int m_Direction; 
	bool m_bOneWayLink;
	string m_LinkID;
	int m_network_design_flag;


	std::string m_geo_string;

	float m_Kjam;

	float m_Wave_speed_in_mph;
	int m_EffectiveLeftTurnGreenTimeInSecond;
	int m_GreenStartTimetInSecond;
	string m_Mode_code;
	string m_demand_type_code;

	int m_DisplayLinkID;


	int m_NumberOfLeftTurnLanes;
	double m_LeftTurnLaneLength;	
	int m_NumberOfRightTurnLanes;
	double m_RightTurnLaneLength;	

	int m_LeftTurnCapacity;

	DTA_Direction m_FromApproach;
	int relative_angel_difference_from_main_direction;
	DTA_Direction m_ToApproach;
	int m_ReverseLinkId;

	float	m_OriginalLength;  // in miles
	float	m_Length;  // in miles
	float   m_AdditionalCost;
	float    m_AgentSpaceCapacity; // in Agents
	int		m_NumberOfLanes;
	float	m_FreeSpeed;
	float   m_SpeedAtCapacity;
	float	m_ReversedSpeedLimit;
	int m_prohibited_u_turn;

	float	m_MaximumServiceFlowRatePHPL;  //Capacity used in BPR for each link, reduced due to link type and other factors.
	float   m_Saturation_flow_rate_in_vhc_per_hour_per_lane;

	float m_FromNodeY;  // From Node, Y value
	float m_ToNodeY;    // To Node, Y value

	float m_MinSpeed;
	float m_MaxSpeed;
	float m_MeanSpeed;
	float m_MeanVolume;

	/* For min-by-min train timetabling, m_LaneCapacity is 1 for each min. 
	Example in airspace scheduling
	a sector is a volume of airspace for which a single air traffic control team has responsibility.
	The number of aircraft that can safely occupy a sector simultaneously is determined by controllers. 
	A typical range is around 8-15. 
	During a 15 minute time interval in US en-route airspace, 
	the typical upper bound limits of the order of 15-20.

	*/

	int  m_StochaticCapcityFlag;  // 0: deterministic cacpty, 1: lane drop. 2: merge, 3: weaving
	// optional for display only
	int	m_link_type;
	string m_link_type_str;
	bool m_bConnector;
	bool m_bTransit;
	bool m_bWalking;

	int m_ConnectorZoneID;

	// for MOE data array
	int m_SimulationHorizon;

	// traffic flow propagation
	float m_FreeFlowTravelTime; // min
	int m_BackwardWaveTimeInSimulationInterval; // simulation time interval

	//  multi-day equilibirum: travel time for stochastic capacity
	float m_LaneCapacity;

	float m_AverageTravelTime;

	float m_OverlappingCost;

	float m_JamTimeStamp;

	int CFlowArrivalCount;
	int CFlowDepartureCount;

	int m_TotalVolumeForMovementCount;
	float m_TotalTravelTime;
	float m_TotalDiffValue;


	bool IsSimulatedDataAvailable(int time)
	{
		if(time < m_LinkMOEArySize)
			return true;
		else
		return false;
	}

	

	float GetSimulatedNodeDelay(int current_time)  // time-mean delay
	{
		//float total_value = 0;
		//int total_count = 0;

		//current_time = current_time + g_SimulationDayNo*1440;
 
		//for(int t = max(0,current_time - g_ImpactStudyPeriodInMin); t< current_time ; t++)
		//{
		//	if(t< m_LinkMOEArySize)
		//	{
		//		if(t < m_LinkMOEArySize && m_LinkMOEAry[t].number_of_Agents_at_downstream >=1) // with flow
		//		{
		//			total_value+= (m_LinkMOEAry[t].total_Agent_delay/m_LinkMOEAry[t].number_of_Agents_at_downstream);
		//		}
		//			total_count ++;
		//	}
		//}

		//	if(total_count>=1)
		//		return total_value/total_count;
		//	else
				return 0 ;
	}

	float GetDynamicNodeDelay(int current_time, eLinkDataType data_type = eSimulationData)
	{
		float value = 0;
			value =  GetSimulatedNodeDelay(current_time);
	
		return value;
	}
	float GetSimulatedSpeed(int current_time)
	{
		float total_value = 0;
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo*1440;
 
		for(int t = current_time; t< current_time + g_MOEAggregationIntervalInMin; t++)
		{
			if(t < m_LinkMOEArySize)
			{
				if(t < m_LinkMOEArySize && m_LinkMOEAry[t].Density >=0.1) // with flow
				{
					total_count++;
					total_value+= m_LinkMOEAry[t].Speed;
				}
			}
		}

			if(total_count>=1)
				return total_value/total_count;
			else
				return this->m_FreeSpeed ;
	}

	float GetSimulatedSpeed(int current_time, int end_time)
	{
		float total_value = 0;
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo * 1440;

		for (int t = current_time; t< end_time; t++)
		{
			if (t < m_LinkMOEArySize)
			{
				if (t < m_LinkMOEArySize && m_LinkMOEAry[t].Density >= 0.1) // with flow
				{
					total_count++;
					total_value += m_LinkMOEAry[t].Speed;
				}
			}
		}

		if (total_count >= 1)
			return total_value / total_count;
		else
			return this->m_FreeSpeed;
	}

	float GetSimulationSpeed(int t)
	{
		t = t + g_SimulationDayNo * 1440;
		if(t < m_LinkMOEArySize  && m_LinkMOEAry[t].Density >=0.1)
		{
			return m_LinkMOEAry[t].Speed;
		}
			return this->m_FreeSpeed;
	}

			
	float GetSimulatedLaneVolume(int current_time)
	{

		float total_value = 0;
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo*144-0;

		for(int t = current_time; t< current_time+g_MOEAggregationIntervalInMin; t++)
		{
		if(t < m_LinkMOEArySize)
		{
			if(m_LinkMOEAry[t].LinkFlow >=1) // with flow
			{
				total_count++;
				total_value+= max(0,m_LinkMOEAry[t].LinkFlow)/m_NumberOfLanes;
			}
		}
		}

			if(total_count>=1)
				return total_value/total_count;
			else
			return 0;


	}

	

		
		float GetAvgLinkHourlyVolume(int start_time, int end_time)
	{

		start_time = start_time + g_SimulationDayNo*1440;
		end_time = end_time + g_SimulationDayNo*1440;


		float total_volume = 0;
		for(int t= start_time ; t< end_time; t++)
		{
		
		if(t < m_LinkMOEArySize)
			total_volume += max(0,m_LinkMOEAry[t].LinkFlow);
		}
		return total_volume/max(1, end_time-start_time);
	}


		float GetAvgLinkSpeed(int start_time, int end_time)
	{

		if(m_LinkMOEArySize == 0) // no time-dependent data 
				return m_avg_simulated_speed;

		start_time = start_time + g_SimulationDayNo*1440;
		end_time = end_time + g_SimulationDayNo*1440;

		float total_Speed = 0;
		for(int t= start_time ; t< end_time; t++)
		{
		
		if(t < m_LinkMOEArySize )
			total_Speed += m_LinkMOEAry[t].Speed;
		}
		return total_Speed/max(1, end_time-start_time);
	}


	




	float GetSimulatedLinkOutVolume(int current_time)
	{

		int total_count = 0;
		float total_value = 0;
		

		current_time = current_time + g_SimulationDayNo*1440;

		for(int t = current_time; t< current_time+g_MOEAggregationIntervalInMin; t++)
		{
		if(t < m_LinkMOEArySize)
		{

			total_value += m_LinkMOEAry[t].DepartureCumulativeFlow - 
				m_LinkMOEAry[t-1].DepartureCumulativeFlow;

				total_count++;
			}
		}
		

			if(total_count>=1)
				return total_value/total_count*60; // from min volume to hourly volume
			else
			return 0;  

	}


	float GetWithinDayLinkMOE(eLinkMOEMode  MOEType, eLinkDataType LinkDataType, int i)
	{

		float  value = 0;

		if(LinkDataType == eSimulationData)
		{

				float	HourlyBackgroundFlow = 0.8*GetSimulatedLinkInVolume(i);

				switch (MOEType)
				{
				case lane_volume: value= GetSimulatedLaneVolume(i); break;
				case speed_kmh: value= GetSimulatedSpeed (i)/0.621371192; break;
				case cummulative_volume: value= GetArrivalCumulativeFlow(i); break;
				case oblique_cummulative_volume: value= GetArrivalCumulativeFlow(i)-HourlyBackgroundFlow/60.0f*(i%1440); break;
				case link_inflow_volume: value= GetSimulatedLinkInVolume(i); break;
				case link_outflow_volume: value= GetSimulatedLinkOutVolume (i); break;
				case link_in_and_outflow_volume: value= max(GetSimulatedLinkInVolume (i), GetSimulatedLinkOutVolume (i)); break;
				case link_travel_time: value= GetSimulatedTravelTime (i); break;
				case speed_mph: value= GetSimulatedSpeed (i); break;
				case link_density: value= GetSimulatedDensity(i); break;
				case link_queue_length_ratio: value= GetQueueLengthPercentage(i); break;
				case number_of_queued_Agents: value = GetNumberOfQueuedVeicles(i); break;
					
				case link_traveltime: value= GetSimulatedTravelTime(i); break;


				default: value = 0;
					}
		
		}


						return value;
	}
	float GetSimulatedLinkVolume(int current_time)
	{
		float total_value = 0;
		int total_count = 0;
		current_time = current_time + g_SimulationDayNo*1440;

		for(int t = current_time; t< current_time+g_MOEAggregationIntervalInMin; t++)
		{
		if(t < m_LinkMOEArySize)
		{
			if(m_LinkMOEAry[t].LinkFlow >=1) // with flow
			{
				total_count++;
				total_value+= m_LinkMOEAry[t].LinkFlow;
			}
		}
		}

			if(total_count>=1)
				return total_value/total_count;
			else
			return 0;
	}


	float GetSimulatedLinkInVolume(int current_time)
	{
		float total_value = 0;
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo*1440;

		int StartTime = current_time;

		int EndTime = current_time +g_MOEAggregationIntervalInMin;

			total_count  = max(1,EndTime - StartTime);
			total_value = m_LinkMOEAry[EndTime].ArrivalCumulativeFlow - m_LinkMOEAry[StartTime].ArrivalCumulativeFlow;

			if(total_value < 0)
				total_value = 0;

			if(total_count>=1)
				return total_value/total_count*60;  // hourly volume
			else
			return 0;
	}

	


	float GetSimulatedTravelTime(int current_time)
	{

		float total_value = 0;
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo*1440;

		for(int t = current_time; t< current_time+g_MOEAggregationIntervalInMin; t++)
		{
		if(t < m_LinkMOEArySize)
		{
				total_count++;
				total_value+= m_LinkMOEAry[t].TravelTime;
		}
		}

			if(total_count>=1)
				return total_value/total_count;
			else
			return m_StaticTravelTime;

	}
	
	float GetDynamicLinkVolume(int current_time, eLinkDataType data_type = eSimulationData)
	{
		float value = 0;
			value =  GetSimulatedLinkVolume(current_time);


		return value;
	}
	float GetDynamicTravelTime(int current_time, eLinkDataType data_type = eSimulationData)
	{
		float value = 0;

			value =  GetSimulatedTravelTime(current_time);
		
	
		return value;
	}

	float GetDynamicSpeed(int current_time, eLinkDataType data_type = eSimulationData)
	{
		float value = 0;
			value =  GetSimulatedSpeed(current_time);
		return value;
	}

	
	float GetArrivalCumulativeFlow(int t)
	{
		t = t + g_SimulationDayNo*1440;

		if(t < m_LinkMOEArySize)
			return m_LinkMOEAry[t].ArrivalCumulativeFlow;  
		else
			return 0;
	}
	float GetDepartureCumulativeFlow(int t)
	{
		t = t + g_SimulationDayNo*1440;
		if(t < m_LinkMOEArySize)
			return m_LinkMOEAry[t].DepartureCumulativeFlow ;  
		else
			return 0;
	}
	
	void RecalculateLinkMOEBasedOnAgentCount()
	{
		float ArrivalCumulativeFlow = 0;
		float DepartureCumulativeFlow = 0;



		//cumulative arrival and deparutre counts
		for(int t =0; t< m_LinkMOEArySize; t++)
		{


			ArrivalCumulativeFlow = max(ArrivalCumulativeFlow, ArrivalCumulativeFlow + m_LinkMOEAry[t].AgentInflowCount ); // in case there are empty flow volumes
			DepartureCumulativeFlow = max(DepartureCumulativeFlow, DepartureCumulativeFlow + m_LinkMOEAry[t].AgentOutflowCount ); // in case there are empty flow volumes

			m_LinkMOEAry[t].ArrivalCumulativeFlow = ArrivalCumulativeFlow;
			m_LinkMOEAry[t].DepartureCumulativeFlow = DepartureCumulativeFlow;

			if(this->m_FromNodeNumber == 48 && this->m_ToNodeNumber == 41)
			{

				TRACE("\ntime t= %d, inflow  = %.1f,cumulative arrival count =%.1f, dep = %f, cd = %f ",t, m_LinkMOEAry[t].AgentInflowCount,ArrivalCumulativeFlow,m_LinkMOEAry[t].AgentOutflowCount, DepartureCumulativeFlow);
			
			}

		}
		//Link inflow count

		for(int t =0; t< m_LinkMOEArySize; t++)
		{
			m_LinkMOEAry[t].LinkFlow =  m_LinkMOEAry[t].AgentInflowCount* 60;  //* 60 to convert from min to hourly counts

					if(this->m_FromNodeNumber == 48 && this->m_ToNodeNumber == 41)
			{

				TRACE("\ntime t= %d, final inflow  = %.1f",t, m_LinkMOEAry[t].LinkFlow );
			
			}


		}

		//density
		for (int t = 0; t< m_LinkMOEArySize; t++)
		{
			int number_of_Agents_on_the_link = m_LinkMOEAry[t].ArrivalCumulativeFlow- m_LinkMOEAry[t].DepartureCumulativeFlow;

			m_LinkMOEAry[t].Density = number_of_Agents_on_the_link / (max(0.0001,m_Length * m_NumberOfLanes)); 

				if(number_of_Agents_on_the_link>=1)
			{

				TRACE("\ntime t= %d, v_count =%d,  den = %.1f",t, number_of_Agents_on_the_link, m_LinkMOEAry[t].Density  );
			
			}



			//ratio between 0 and 1
			m_LinkMOEAry[t].QueueLength = min(1.0,number_of_Agents_on_the_link/(max(0.0001, m_Length * m_NumberOfLanes * m_Kjam )));
		}

	}
	float GetSimulatedDensity(int current_time)
	{

		float total_value = 0;
		int total_count = 0;
		current_time = current_time + g_SimulationDayNo*1440;

		for(int t = current_time; t< current_time+g_MOEAggregationIntervalInMin; t++)
		{
		if(t < m_LinkMOEArySize)
			{

			total_value += max(0, m_LinkMOEAry[t].Density);  

			total_count++;
			}
		}
		
	
		if(total_count>=1)
			return total_value/total_count;
		else
			return 0;


			return 0;

}		

	float GetSimulatedDensityMOE(int start_time, int end_time, float &maximum)
	{
		maximum = 0;
		float average = 0;
		float total = 0;
		int count = 0;

		start_time = start_time + g_SimulationDayNo*1440;
		end_time = end_time + g_SimulationDayNo*1440;

		for(int t = start_time; t<= end_time; t++)
		{
		if(t < m_LinkMOEArySize)
		{
			if( m_LinkMOEAry[t].Density>0.01)
			{
				total+= m_LinkMOEAry[t].Density;
				count++;

			if( maximum < m_LinkMOEAry[t].Density)
					maximum = m_LinkMOEAry[t].Density;
			}

			
		}
		}

		average = total/max(1,count);
		return average;

	}		

	float GetSimulationVolumeMOE(int start_time, int end_time, float &maximum)
	{
		maximum = 0;
		float average = 0;
		float total = 0;
		int count = 0;

		start_time = start_time + g_SimulationDayNo*1440;
		end_time = end_time + g_SimulationDayNo*1440;

		for(int t = start_time; t<= end_time; t++)
		{
		if(t < m_LinkMOEArySize)
		{
			if( m_LinkMOEAry[t].Density>0.01)
			{
				total+= m_LinkMOEAry[t].LinkFlow;
				count++;

			if( maximum < m_LinkMOEAry[t].LinkFlow)
					maximum = m_LinkMOEAry[t].LinkFlow;
			}

			
		}
		}

		average = total/max(1,count);
		return average;

	}		

	float GetNumberOfQueuedVeicles(int current_time)
	{
		int total_count = 0;
		float total_value = 0;

		current_time = current_time + g_SimulationDayNo * 1440;

		for (int t = current_time; t< current_time + g_MOEAggregationIntervalInMin; t++)
		{
			if (t < m_LinkMOEArySize)
			{

				total_value += max(0, m_LinkMOEAry[t].number_of_queued_Agents);

				total_count++;
			}
		}

		if (total_count >= 1)
			return total_value / total_count;
		else
			return m_StaticTravelTime;


		return 0;
	}


	float GetQueueLengthPercentage(int current_time )
	{
		int total_count = 0;
		float total_value = 0;

		current_time = current_time + g_SimulationDayNo*1440;

		for(int t = current_time; t< current_time+g_MOEAggregationIntervalInMin; t++)
		{
		if(t < m_LinkMOEArySize)
		{

			total_value +=max(0, m_LinkMOEAry[t].QueueLength*100);

			total_count++;
			}
		}
	
		if(total_count>=1)
			return total_value/total_count;
		else
			return m_StaticTravelTime;


			return 0;
	}		


	
	int GetImpactFlag(int t,eLinkDataType data_type)
	{

	if( GetDynamicSpeed(t,data_type)< 0.33 * m_FreeSpeed) 
			return 1;

	if(data_type == eSimulationData && t>=0 && t < m_LinkMOEArySize)
	{

	if( ( m_Length < 0.2  && ( m_LinkMOEAry[t].QueueLength *100 >= 99  )
	|| (m_Length >= 0.2  && m_LinkMOEAry[t].QueueLength*100 >= g_ImpactThreshold_QueueLengthPercentage)))
	{
						return 1;
	}else 
	{
		return 0;
	}

	}

	return 0;
	}

	int GetImpactDuration(int current_time, eLinkDataType data_type = eSimulationData)
	{
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo*1440;

		int start_time = max(0,current_time - g_ImpactStudyPeriodInMin);
		for(int t = start_time; t<= current_time ; t++)
		{

			if(GetImpactFlag(t,data_type))
			{
				total_count++;
			}
		}
	
			return total_count;
	}		

	int GetImpactRelativeStartTime(int current_time,eLinkDataType data_type)
	{
		int total_count = 0;

		current_time = current_time + g_SimulationDayNo*1440;

		int start_time = max(0,current_time - g_ImpactStudyPeriodInMin);
		for(int t = start_time; t<= current_time ; t++)
		{
				if(GetImpactFlag(t,data_type))
				{
					return current_time -t;
				}
		}
	
			return 999999;
	}		


	float GetAggregatedSimulatedTravelTime(int starting_time, int time_interval = 1)
	{

		starting_time = starting_time + g_SimulationDayNo*1440;


		float travel_time  = max(m_StaticTravelTime,m_FreeFlowTravelTime);

			float total_travel_time = 0;
			for(int t=starting_time; t<  starting_time + time_interval; t++)
			{
				if(t < m_LinkMOEArySize)
				{
				total_travel_time +=  ( m_Length * 60/ max(1,m_LinkMOEAry[t].Speed));
				}
			}

			travel_time =  total_travel_time/time_interval;

			//if(travel_time < m_FreeFlowTravelTime)
			//	travel_time = m_FreeFlowTravelTime; // minimum travel time constraint for shortest path calculation



		return max(0.0001f,travel_time);;

	};
	GDPoint GetRelativePosition(float ratio)
	{

		GDPoint Pt;
		Pt.x = 0;
		Pt.y = 0;

		if (m_ShapePoints.size() <2)
		{
			Pt.x = ratio * this->m_FromPoint.x + (1 - ratio)* this->m_ToPoint.x;
			Pt.y = ratio * this->m_FromPoint.y + (1 - ratio)* this->m_ToPoint.y;
			return Pt;
		}


		Pt.x = (m_ShapePoints[0].x + m_ShapePoints[m_ShapePoints.size() - 1].x) / 2;
		Pt.y = (m_ShapePoints[0].y + m_ShapePoints[m_ShapePoints.size() - 1].y) / 2;

		unsigned	int si;

		if (m_ShapePointRatios.size() == m_ShapePoints.size())
		{

			for (si = 0; si < m_ShapePoints.size() - 1; si++)
			{

				if (ratio > m_ShapePointRatios[si] && ratio < m_ShapePointRatios[si + 1])
				{

					float SectionRatio = m_ShapePointRatios[si + 1] - m_ShapePointRatios[si];

					float RelateveRatio = 0;
					if (SectionRatio >0)
						RelateveRatio = (ratio - m_ShapePointRatios[si]) / SectionRatio;

					Pt.x = m_ShapePoints[si].x + RelateveRatio*(m_ShapePoints[si + 1].x - m_ShapePoints[si].x);
					Pt.y = m_ShapePoints[si].y + RelateveRatio*(m_ShapePoints[si + 1].y - m_ShapePoints[si].y);

					return Pt;
				}
			}

		}
		return Pt;
	}

};


class DTAPath
{
public:
	DTAPath()
	{
		m_bSavedPath = false;
		m_bWithSensorTravelTime = false;
		total_free_flow_travel_time = 0;
		total_distance = 0;
		
		for(int t=0; t<1440; t++)
		{
			m_TimeDependentTravelTime[t] = 0;
		}

		m_Distance =0;
	}

	void Init(int LinkSize, int TimeHorizon)
	{
		m_number_of_days = max(1,TimeHorizon/1440);
	}


	float GetTimeDependentMOE(int time, int MOEType, int MOEAggregationIntervalInMin = 1)
	{
		float total_value = 0;
		int total_count = 0;

		int agg_interval = MOEAggregationIntervalInMin;

		if(MOEType == 1)  //sensor data
			agg_interval = 1;

		for(int t = time; t< min(1440,time+agg_interval); t++)
		{
			float value = GetTimeDependentMOEBy1Min(t,MOEType);
			if(value>0.00001f) // with value
			{
				total_count++;
				total_value+= value;
			}
		}

		return total_value/max(1,total_count);
	}



	float GetTimeDependentMOEBy1Min(int time, int MOEType)
	{

		switch(MOEType)
		{
		case 0: return m_TimeDependentTravelTime[time];
		case 1: return m_SensorTimeDependentTravelTime[time];


		default: return m_TimeDependentTravelTime[time];
		}

	}

	~DTAPath()
	{
	}

	bool m_bSavedPath;
	bool m_bWithSensorTravelTime;
	float total_free_flow_travel_time;
	float total_distance;

	std::vector<int> m_LinkVector;
	std::vector<CString> m_PathLabelVector;
	std::string m_path_name;

	int m_NodeNodeSum;

	float m_TimeDependentTravelTime[1440];
	float m_SensorTimeDependentTravelTime[1440];


	float m_TimeDependentCount[1440];

	float m_MaxTravelTime;

	int m_number_of_days;

	float m_Distance;

	float m_TravelTime;
	float m_Reliability;
	float m_PM;
	float m_Safety;
	float m_Fuel;
	int m_NumberOfSensorsPassed;
	int m_NumberOfPassiveSensorsPassed;
	int m_NumberOfActiveSensorsPassed;


};






// link element of a Agent path
class SAgentLink
{  public:

int LinkNo;  // range:
float ArrivalTimeOnDSN;     // absolute arrvial time at downstream node of a link: 0 for the departure time, including delay/stop time
string State;
//   float LinkWaitingTime;   // unit: 0.1 seconds
SAgentLink()
{
	LinkNo = -1;
	ArrivalTimeOnDSN = 0;
	//		LinkWaitingTime = 0;

}

};


class DTAAgentAdditionalData   // this class contains non-essential information, we do not allocate memory for this additional info in the basic version
{
public:
	float m_TollDollar;
	float m_MinCost;
	float m_MeanTravelTime;
	float m_TravelTimeVariance;
	unsigned short m_NumberOfSamples;  // when switch a new path, the number of samples starts with 0

	DTAAgentAdditionalData()
	{
		m_NumberOfSamples =0;
		m_MinCost = 0;
	};

	void PostTripUpdate(float TripTime)   
	{
		float GainFactor = 0.2f;  // will use formula from Kalman Filtering, eventually

		m_MeanTravelTime = (1-GainFactor)*m_MeanTravelTime + GainFactor*TripTime;
		m_NumberOfSamples +=1;
	};


};

class AgentLocationRecord
{
	public:
	
	string agent_id;
	int  agent_no;
	int day_no;
	string agent_type;
	
	double x;
	double y;

	int link_no;
	bool b_to_data_flag;

	double to_x;
	double to_y;

	float time_stamp_in_min;
	
	long time_stamp_in_second;

	float distance_in_km;

	AgentLocationRecord()
	{

		link_no = -1;
		agent_type = -1;
		b_to_data_flag = false;
		time_stamp_in_second = -1;
	}
};


class AgentLocationTimeIndexedMap
{
	public:
		std::vector<AgentLocationRecord> AgentLocationRecordVector;  
				
};


class DTAAgent
{
public:

	int m_Age;
	bool m_bGPSAgent;  // GPS Agent
	int m_NodeSize;
	int m_NodeNumberSum;  // used for comparing two paths
	SAgentLink *m_NodeAry; // link list arrary of a Agent path
	int m_AgentLocationSize;
	AgentLocationRecord *m_LocationRecordAry; // link list arrary of a Agent path

	unsigned int m_RandomSeed;
	int m_AgentID;  //range: +2,147,483,647
	int m_DayNo;
	std::vector<GDPoint> m_ShapePoints;
	string m_AgentKey;
	int m_FromZoneID;  //range 0, 65535
	int m_ToZoneID;  // range 0, 65535
	int m_FromNodeID;
	int m_ToNodeID;
	short m_DemandType;     // 
	short m_SimLinkSequenceNo; //  range 0, 65535

	bool  m_bImpacted;

	float m_TimeToRetrieveInfo;
	float m_DepartureTime;
	float m_ArrivalTime;
	float m_TripTime;
	float m_FreeflowTripTime;

	float m_path_start_node_departure_time;
	float m_path_end_node_arrival_time;
	float m_path_travel_time;
	float m_path_free_flow_travel_time;
	float m_path_distance;

	float m_subarea_start_node_departure_time;
	float m_subarea_end_node_arrival_time;
	float m_subarea_travel_time;
	float m_subarea_free_flow_travel_time;
	float m_subarea_distance;
	
	float m_PCE;

	float m_Delay;
	float m_Distance;

	bool m_bSwitched;  // switch route in assignment

	// used for simulation
	bool m_bLoaded; // be loaded into the physical network or not
	bool m_bComplete;

	bool m_bODMarked;
	bool m_bMarked;

	   	DTAAgentAdditionalData* pVehData;

	std::vector<GDPoint> m_GPSLocationVector;


	DTAAgent()
	{

		m_subarea_start_node_departure_time = 0;
		m_subarea_end_node_arrival_time = 0;
		m_subarea_travel_time = 0;
		m_subarea_free_flow_travel_time = 0;
		m_subarea_distance = 0;

		m_path_distance = 0;
		m_path_start_node_departure_time = 0;
		m_path_end_node_arrival_time = 0;
		m_path_travel_time = 0;
		m_path_free_flow_travel_time = 0;


		m_AgentLocationSize = 0;
		m_bMarked = false;
		m_Age = 0;
		m_NodeNumberSum = 0;
		m_DayNo = 0;  // without day no specified from the data
		m_DemandType = 1;

		m_bGPSAgent = false;
		m_bODMarked = false;
		pVehData=NULL;
		m_TimeToRetrieveInfo = -1;

		m_NodeAry = NULL;
		m_NodeSize	= 0;
		m_bImpacted = false; 
		m_PCE = 1;
		m_ArrivalTime = 0;
		//      m_FinalArrivalTime = 0;
		m_bLoaded = false;
		m_bSwitched = false;
		m_bComplete = false;
		m_TripTime = 900;  // default: for incomplete Agents, they have an extremey long trip time
		m_FreeflowTripTime = 0;
		m_Distance =0;
		m_Delay = 0;


	};
	~DTAAgent()
	{
		if(m_NodeAry != NULL)
			delete m_NodeAry;

		if(pVehData!=NULL)
			delete pVehData;

		if(m_LocationRecordAry!=NULL)
			delete m_LocationRecordAry;
	};

public:



public:  // fetch additional data

	void SetMinCost(float MinCost)
	{
		if(pVehData!=NULL)
			pVehData->m_MinCost = MinCost;
	};

	float GetMinCost()
	{
		if(pVehData==NULL)
			return 0.0f;
		else
			return pVehData->m_MinCost;

	};

};

class DTA_vhc_simple // used in STL sorting only
{
public:

	int m_FromZoneID;
	int m_ToZoneID;
	int m_AgentType;
	float    m_DepartureTime;


	bool operator<(const DTA_vhc_simple &other) const
	{
		return m_DepartureTime < other.m_DepartureTime;
	}

};

template <typename T>
T **AllocateDynamicArray(int nRows, int nCols)
{
	T **dynamicArray;

	dynamicArray = new T*[nRows];

	for( int i = 0 ; i < nRows ; i++ )
	{
		dynamicArray[i] = new T [nCols];

		if (dynamicArray[i] == NULL)
		{
			cout << "Error: insufficent memory.";
			g_ProgramStop();
		}

	}

	return dynamicArray;
}

template <typename T>
void DeallocateDynamicArray(T** dArray,int nRows, int nCols)
{
	for(int x = 0; x < nRows; x++)
	{
		delete[] dArray[x];
	}

	delete [] dArray;

}


template <typename T>
T ***Allocate3DDynamicArray(int nX, int nY, int nZ)
{
	T ***dynamicArray;

	dynamicArray = new T**[nX];

	if (dynamicArray == NULL)
	{
		cout << "Error: insufficent memory.";
		g_ProgramStop();
	}

	for( int x = 0 ; x < nX ; x++ )
	{
		dynamicArray[x] = new T* [nY];

		if (dynamicArray[x] == NULL)
		{
			cout << "Error: insufficent memory.";
			g_ProgramStop();
		}

		for( int y = 0 ; y < nY ; y++ )
		{
			dynamicArray[x][y] = new T [nZ];
			if (dynamicArray[x][y] == NULL)
			{
				cout << "Error: insufficent memory.";
				g_ProgramStop();
			}

		}
	}

	return dynamicArray;

}

template <typename T>
void Deallocate3DDynamicArray(T*** dArray, int nX, int nY)
{
	for(int x = 0; x < nX; x++)
	{
		for(int y = 0; y < nY; y++)
		{
			delete[] dArray[x][y];
		}

		delete[] dArray[x];
	}

	delete[] dArray;

}
extern void g_ProgramStop();
extern bool g_read_a_line(FILE* f, char* aline, int & size);

class DTANetworkForSP  // mainly for shortest path calculation, not just physical network
	// for shortes path calculation between zone centroids, for origin zone, there are only outgoing connectors, for destination zone, only incoming connectors
	// different shortest path calculations have different network structures, depending on their origions/destinations
{
public:
	int m_OptimizationIntervalSize;
	int m_NodeSize;
	int m_PhysicalNodeSize;
	int m_OptimizationHorizon;
	int m_OptimizationTimeInveral;
	int m_ListFront;
	int m_ListTail;
	int m_LinkSize;

	int* m_LinkList;  // dimension number of nodes

	int** m_OutboundNodeAry; //Outbound node array
	int** m_OutboundLinkAry; //Outbound link array
	int* m_OutboundSizeAry;  //Number of outbound links

	int** m_InboundLinkAry; //inbound link array
	int* m_InboundSizeAry;  //Number of inbound links

	int* m_FromIDAry;
	int* m_ToIDAry;

	float** m_LinkTDTimeAry;
	float** m_LinkTDCostAry;

	int* NodeStatusAry;                // Node status array used in KSP;
	int* LinkNoAry;                // Node status array used in KSP;

	float* LabelTimeAry;               // label - time
	int* NodePredAry;
	float* LabelCostAry;


	//below are time-dependent cost label and predecessor arrays
	float** TD_LabelCostAry;
	int** TD_NodePredAry;  // pointer to previous NODE INDEX from the current label at current node and time
	int** TD_TimePredAry;  // pointer to previous TIME INDEX from the current label at current node and time


	int m_Number_of_CompletedAgents;
	int m_AdjLinkSize;

	// search tree
	struct SearchTreeElement
	{
		int CurrentNode;
		int PredecessorNode;
		int SearchLevel;
		float TravelTime;
	};

	SearchTreeElement* m_SearchTreeList;  // predecessor

	int  m_TreeListSize;
	int  m_TreeListFront;
	int  m_TreeListTail;



	//

	DTANetworkForSP(int NodeSize, int LinkSize, int TimeHorizon, int TimeInterval, int AdjLinkSize){
		m_NodeSize = NodeSize;
		m_LinkSize = LinkSize;

		m_SearchTreeList = NULL;

		m_OptimizationHorizon = TimeHorizon;
		m_OptimizationTimeInveral = TimeInterval;
		m_AdjLinkSize = AdjLinkSize;


		m_OutboundSizeAry = new int[m_NodeSize];
		m_InboundSizeAry = new int[m_NodeSize];


		m_OutboundNodeAry = AllocateDynamicArray<int>(m_NodeSize,m_AdjLinkSize);
		m_OutboundLinkAry = AllocateDynamicArray<int>(m_NodeSize,m_AdjLinkSize);
		m_InboundLinkAry = AllocateDynamicArray<int>(m_NodeSize,m_AdjLinkSize);


		m_LinkList = new int[m_NodeSize];

		m_OptimizationIntervalSize = int(m_OptimizationHorizon/m_OptimizationTimeInveral+0.1);  // make sure there is no rounding error
		m_LinkTDTimeAry   =  AllocateDynamicArray<float>(m_LinkSize,m_OptimizationIntervalSize);
		m_LinkTDCostAry   =  AllocateDynamicArray<float>(m_LinkSize,m_OptimizationIntervalSize);

		m_FromIDAry = new int[m_LinkSize];

		m_ToIDAry = new int[m_LinkSize];

		NodeStatusAry = new int[m_NodeSize];                    // Node status array used in KSP;
		LinkNoAry = new int[m_NodeSize];
		NodePredAry = new int[m_NodeSize];
		LabelTimeAry = new float[m_NodeSize];                     // label - time
		LabelCostAry = new float[m_NodeSize];                     // label - cost

		TD_LabelCostAry =  AllocateDynamicArray<float>(m_NodeSize,m_OptimizationIntervalSize);
		TD_NodePredAry = AllocateDynamicArray<int>(m_NodeSize,m_OptimizationIntervalSize);
		TD_TimePredAry = AllocateDynamicArray<int>(m_NodeSize,m_OptimizationIntervalSize);


		if(m_OutboundSizeAry==NULL || m_LinkList==NULL || m_FromIDAry==NULL || m_ToIDAry==NULL  ||
			NodeStatusAry ==NULL || NodePredAry==NULL || LabelTimeAry==NULL || LabelCostAry==NULL)
		{
			cout << "Error: insufficent memory.";
			g_ProgramStop();
		}

	};

	DTANetworkForSP()
	{
		m_OutboundSizeAry = NULL;
		m_OutboundNodeAry = NULL;
		m_SearchTreeList = NULL;
		m_InboundSizeAry = NULL;

		m_LinkList = NULL;

		m_FromIDAry = NULL;
		m_ToIDAry = NULL;

		NodeStatusAry  = NULL;
		LinkNoAry  = NULL;
		NodePredAry  = NULL;
		LabelTimeAry  = NULL;
		LabelCostAry  = NULL;


		m_NodeSize = 0;
		m_LinkSize = 0;
	};

	void Initialize(int NodeSize, int LinkSize, int TimeHorizon,int AdjLinkSize)
	{

		if(TimeHorizon <1)
		{
				AfxMessageBox("TimeHorizon <1!");
				TimeHorizon = 1;
				return;

		}
		if(m_OutboundNodeAry!=NULL)
		{
			FreeMemory();
		}

		m_NodeSize = NodeSize;
		m_LinkSize = LinkSize;

		m_OptimizationHorizon = TimeHorizon;
		m_OptimizationTimeInveral = 1;
		m_AdjLinkSize = AdjLinkSize;


		m_OutboundSizeAry = new int[m_NodeSize];
		m_InboundSizeAry = new int[m_NodeSize];


		m_OutboundNodeAry = AllocateDynamicArray<int>(m_NodeSize,m_AdjLinkSize);
		m_OutboundLinkAry = AllocateDynamicArray<int>(m_NodeSize,m_AdjLinkSize);
		m_InboundLinkAry = AllocateDynamicArray<int>(m_NodeSize,m_AdjLinkSize);


		m_LinkList = new int[m_NodeSize];

		m_OptimizationIntervalSize = int(TimeHorizon/m_OptimizationTimeInveral)+1;  // make sure it is not zero
		m_LinkTDTimeAry   =  AllocateDynamicArray<float>(m_LinkSize,m_OptimizationIntervalSize);
		m_LinkTDCostAry   =  AllocateDynamicArray<float>(m_LinkSize,m_OptimizationIntervalSize);

		m_FromIDAry = new int[m_LinkSize];

		m_ToIDAry = new int[m_LinkSize];

		NodeStatusAry = new int[m_NodeSize];                    // Node status array used in KSP;
		LinkNoAry = new int[m_NodeSize];                    // Node status array used in KSP;
		NodePredAry = new int[m_NodeSize];
		LabelTimeAry = new float[m_NodeSize];                     // label - time
		LabelCostAry = new float[m_NodeSize];                     // label - cost


		TD_LabelCostAry =  AllocateDynamicArray<float>(m_NodeSize,m_OptimizationIntervalSize);
		TD_NodePredAry = AllocateDynamicArray<int>(m_NodeSize,m_OptimizationIntervalSize);
		TD_TimePredAry = AllocateDynamicArray<int>(m_NodeSize,m_OptimizationIntervalSize);


		if(m_OutboundSizeAry==NULL || m_LinkList==NULL || m_FromIDAry==NULL || m_ToIDAry==NULL  ||
			NodeStatusAry ==NULL || NodePredAry==NULL || LabelTimeAry==NULL || LabelCostAry==NULL)
		{
			cout << "Error: insufficent memory.";
			g_ProgramStop();
		}

	};


	void FreeMemory()
	{
		if(m_SearchTreeList) delete m_SearchTreeList;

		if(m_OutboundSizeAry)  delete m_OutboundSizeAry;
		if(m_InboundSizeAry)  delete m_InboundSizeAry;

		if(m_NodeSize > 0 && m_AdjLinkSize >0 && m_OptimizationIntervalSize >0)
		{
		DeallocateDynamicArray<int>(m_OutboundNodeAry,m_NodeSize, m_AdjLinkSize);
		DeallocateDynamicArray<int>(m_OutboundLinkAry,m_NodeSize, m_AdjLinkSize);
		DeallocateDynamicArray<int>(m_InboundLinkAry,m_NodeSize, m_AdjLinkSize);
		DeallocateDynamicArray<float>(TD_LabelCostAry,m_NodeSize,m_OptimizationIntervalSize);
		DeallocateDynamicArray<int>(TD_NodePredAry,m_NodeSize,m_OptimizationIntervalSize);
		DeallocateDynamicArray<int>(TD_TimePredAry,m_NodeSize,m_OptimizationIntervalSize);
		}


		if(m_LinkList) delete m_LinkList;

		if(m_LinkSize > 0)
		{
		DeallocateDynamicArray<float>(m_LinkTDTimeAry,m_LinkSize,m_OptimizationIntervalSize);
		DeallocateDynamicArray<float>(m_LinkTDCostAry,m_LinkSize,m_OptimizationIntervalSize);
		}

		if(m_FromIDAry)		delete m_FromIDAry;
		if(m_ToIDAry)	delete m_ToIDAry;

		if(NodeStatusAry) delete NodeStatusAry;                 // Node status array used in KSP;
		if(LinkNoAry) delete LinkNoAry;                 // Node status array used in KSP;
		if(NodePredAry) delete NodePredAry;
		if(LabelTimeAry) delete LabelTimeAry;
		if(LabelCostAry) delete LabelCostAry;
	}
	~DTANetworkForSP()
	{

		FreeMemory();
	};

	float GetTollRateInMin(int LinkID, float Time, int AgentType);  // built-in function for each network_SP to avoid conflicts with OpenMP parallel computing


	void BuildNetwork(int ZoneID);
	void BuildHistoricalInfoNetwork(int CurZoneID, int CurrentTime, float Perception_error_ratio);
	void BuildTravelerInfoNetwork(int CurrentTime, float Perception_error_ratio);

	void BuildPhysicalNetwork(std::list<DTANode*>* p_NodeSet, std::list<DTALink*>* p_LinkSet, float RandomCostCoef, bool bOverlappingCost, int OriginNodeID = -1, int DestinationNode = -1);
	void BuildSpaceTimeNetworkForTimetabling(std::list<DTANode*>* p_NodeSet, std::list<DTALink*>* p_LinkSet, int TrainType);

	void IdentifyBottlenecks(int StochasticCapacityFlag);

	int SimplifiedTDLabelCorrecting_DoubleQueue(int origin, int departure_time, int destination, int pricing_type, float VOT,int PathLinkList[MAX_NODE_SIZE_IN_A_PATH],float &TotalCost, bool distance_flag, bool check_connectivity_flag, bool debug_flag, float RandomCostCoef);   // Pointer to previous node (node)

	// simplifed version use a single node-dimension of LabelCostAry, NodePredAry

	//these two functions are for timetabling
	bool OptimalTDLabelCorrecting_DoubleQueue(int origin, int departure_time);
	// optimal version use a time-node-dimension of TD_LabelCostAry, TD_NodePredAry
	bool GenerateSearchTree(int origin,  int destination, int node_size,float TravelTimeBound);

	// return node arrary from origin to destination, return travelling timestamp at each node
	// return number_of_nodes in path

	void AgentBasedPathAssignment(int zone,int departure_time_begin, int departure_time_end, int iteration);
	void HistInfoAgentBasedPathAssignment(int zone,int departure_time_begin, int departure_time_end);

	// SEList: Scan List implementation: the reason for not using STL-like template is to avoid overhead associated pointer allocation/deallocation
	void SEList_clear()
	{
		m_ListFront= -1;
		m_ListTail= -1;
	}

	void SEList_push_front(int node)
	{
		if(m_ListFront == -1)  // start from empty
		{
			m_LinkList[node] = -1;
			m_ListFront  = node;
			m_ListTail  = node;
		}
		else
		{
			m_LinkList[node] = m_ListFront;
			m_ListFront  = node;
		}

	}
	void SEList_push_back(int node)
	{
		if(m_ListFront == -1)  // start from empty
		{
			m_ListFront = node;
			m_ListTail  = node;
			m_LinkList[node] = -1;
		}
		else
		{
			m_LinkList[m_ListTail] = node;
			m_LinkList[node] = -1;
			m_ListTail  = node;
		}
	}

	bool SEList_empty()
	{
		return(m_ListFront== -1);
	}

	int SEList_front()
	{
		return m_ListFront;
	}

	void SEList_pop_front()
	{
		int tempFront = m_ListFront;
		m_ListFront = m_LinkList[m_ListFront];
		m_LinkList[tempFront] = -1;
	}

	int  GetLinkNoByNodeIndex(int usn_index, int dsn_index);

};

struct AgentCFData
{
	int AgentID;
	int AgentType; // 1 - motorcycle, 2 - auto, 3 - truck
	float StartTime; // in time interval, LinkStartTime, so it should be sorted
	float EndTime; // in time interval

};




class CAVISensorPair
{

public:
	CAVISensorPair()
	{
		pLink = NULL;
		number_of_samples = 0;
	}
	int sensor_pair_id;
	int number_of_samples;
	CString sensor_type;
	long from_node_id;
	long to_node_id ;
	DTALink* pLink;
};


class CDataVector
{
public: 
	std::vector<float> m_data_vector;

	
	void AddData(float value)
	{
	m_data_vector.push_back(value);

	if(value<-0.1)
		TRACE("");

	
	}

	float GetDataVectorMean()
	{
		if(m_data_vector.size() == 0) 
			return 0;
		float total_travel_time = 0; 
		for(unsigned int i = 0; i< m_data_vector.size(); i++)
		{
		total_travel_time+= m_data_vector[i];
		}

		return total_travel_time/m_data_vector.size();
	
	
	}
	float GetDataVectorPercentage(float Percentage)
	{
		if(m_data_vector.size() == 0) 
			return 0;

		sort(m_data_vector.begin(), m_data_vector.end());

		int index = min(m_data_vector.size()-1, m_data_vector.size()*Percentage/100);
		return m_data_vector[index];
	}

	void Reset()
	{
		if(m_data_vector.size() > 0)
			m_data_vector.clear();
	
	}
	
};

class AgentStatistics
{
public: 

	CDataVector m_data_vector_travel_time;
	CDataVector m_data_vector_travel_time_per_mile;






	int day_count[21];
	AgentStatistics()
	{

		TotalAgentSize = 0;
		TotalTravelTime = 0;
		TotalFreeflowTravelTime = 0;
		TotalTravelTimePerMile = 0;
		TotalDistance = 0;
		TotalCost = 0;
		TotalPM= 0;
		TotalGeneralizedCost = 0;
		TotalGeneralizedTime = 0;
		DisplayValue = 0;

		TotalTravelTimeVariance = 0;
		TotalTravelTimePerMileVariance = 0;


	}

	void Reset()
	{

	m_data_vector_travel_time.Reset();
	m_data_vector_travel_time_per_mile.Reset();

		for(int d= 0; d<= 20; d++)
			day_count[d]= 0;

		TotalAgentSize = 0;
		TotalTravelTime = 0;
		TotalFreeflowTravelTime = 0;
		TotalTravelTimePerMile = 0;
		TotalTravelTimeVariance = 0;
		TotalTravelTimePerMileVariance = 0;
		TotalDistance = 0;
		TotalCost = 0;
		TotalPM = 0;
		TotalGeneralizedCost = 0;
		TotalGeneralizedTime = 0;
		bImpactFlag = false;
		AvgTravelTime = 0;
		AvgDistance = 0;

	
	}

	CString Label;
	float DisplayValue;
	bool bImpactFlag;
	int   TotalAgentSize;
	float TotalTravelTime;
	float TotalFreeflowTravelTime;
	float TotalTravelTimePerMile;
	
	float TotalTravelTimePerMileVariance;
	float TotalTravelTimeVariance;

		float TotalDistance;
	float TotalGeneralizedCost;
	float TotalGeneralizedTime;
	float TotalCost;
	float TotalPM;
	float AvgTravelTime;
	float AvgDistance;


};

struct NetworkLoadingOutput
{
public:
	NetworkLoadingOutput()
	{
	ResetStatistics();
	}

	void ResetStatistics ()
	{
	AvgUEGap = 0;
	TotalDemandDeviation = 0;
	LinkVolumeAvgAbsError  =0 ;
	LinkVolumeAvgAbsPercentageError  =0 ;
	LinkVolumeRootMeanSquaredError = 0;
	CorrelationBetweenObservedAndSimulatedLinkVolume = 0;

	AvgTravelTime = 0;
	AvgDelay = 0;
	AvgTTI = 0;
	AvgDistance = 0;
	NumberofAgentsCompleteTrips = 0;
	NumberofAgentsGenerated = 0;
	SwitchPercentage = 0;
	}
	float AvgTravelTime;
	float AvgDelay;
	float AvgTTI;
	float AvgDistance;
	int   NumberofAgentsCompleteTrips;
	int   NumberofAgentsGenerated;
	float SwitchPercentage;
	float AvgUEGap;
	float TotalDemandDeviation;
	float LinkVolumeAvgAbsError;
	float LinkVolumeAvgAbsPercentageError;
	float LinkVolumeRootMeanSquaredError;
	float CorrelationBetweenObservedAndSimulatedLinkVolume;
};




extern float g_RNNOF();

extern std::vector<DTAPath>	m_PathDisplayList;
extern int m_SelectPathNo;
extern float g_Simulation_Time_Stamp;
struct s_link_selection
{
public:
	int link_no;
	int document_no;
	DTALink* pLink;
	s_link_selection()
	{
		link_no = -1;
		document_no = -1;
		pLink = NULL;
	}

};
extern void g_AddLinkIntoSelectionList(DTALink* pLink, int link_no, int document_no, bool b_SelectOtherDocuments = false, double x = 0, double y = 0);

extern std::list<s_link_selection>	g_LinkDisplayList;
extern int  g_SimulationStartTime_in_min;
