//TLiteDoc.cpp : implementation of the CTLiteDoc class
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
#include "Geometry.h"
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
#include "CGridListCtrlEx\\CGridListCtrlEx.h"


#include "DlgLinkList.h"
#include "DlgPathList.h"

#include "Dlg_ImageSettings.h"
#include "Shellapi.h"

#include "DlgNetworkAlignment.h"
#include "Dlg_VehEmissions.h"
#include "DlgMOETabView.h"



#include "Dlg_VehicleClassification.h"
#include "Dlg_Find_Vehicle.h"

#include "Dlg_Legend.h"

#include "Page_Node_Movement.h"

#include "AssignmentSimulationSettingDlg.h"
#include "NetworkDataDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define _max_number_of_movements  32

extern std::vector<float> g_link_speed_vector;

extern void g_SetCursor(_cursor_type cursor_type);


extern int g_ParserIntSequence(std::string str, std::vector<int> &vect); 
extern int g_ParserFloatSequence(std::string str, std::vector<float> &vect); 
extern int g_ParserStringSequence(std::string str, std::vector<string> &vect);

CDlgMOE *g_LinkMOEDlg = NULL;
CDlg_Legend* g_pLegendDlg = NULL;
CDlgLinkList* g_pLinkListDlg = NULL;
CDlg_VehPathAnalysis* g_pAgentPathDlg = NULL;
CDlgPathList* g_pPathListDlg = NULL;

bool g_bShowLinkList = false;
bool g_bShowAgentPathDialog = false;



extern float g_Simulation_Time_Stamp;
bool g_LinkMOEDlgShowFlag = false;

std::list<s_link_selection>	g_LinkDisplayList;
std::list<CTLiteDoc*>	g_DocumentList;

std::vector<CDlg_AgentClassification*>	g_SummaryDialogVector;

bool g_bValidDocumentChanged = false;
std::list<CTLiteView*>	g_ViewList;

void g_ClearLinkSelectionList()
{
	//std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	//while (iDoc != g_DocumentList.end())
	//{

	//	if((*iDoc)!=NULL)
	//	{
	//	for (std::list<DTALink*>::iterator iLink = (*iDoc)->m_LinkSet.begin(); iLink != (*iDoc)->m_LinkSet.end(); iLink++)
	//	{
	//		if((*iLink)!=NULL)
	//		{
	//		(*iLink)->m_DisplayLinkID = -1;
	//		}
	//	}
	//	}
	//	iDoc++;
	//}

	for (std::list<s_link_selection>::iterator iLinkDisplay = g_LinkDisplayList.begin(); iLinkDisplay != g_LinkDisplayList.end();iLinkDisplay++ )
	{

		if((*iLinkDisplay).pLink !=NULL)
			(*iLinkDisplay).pLink -> m_DisplayLinkID =  -1;
	}


	g_LinkDisplayList.clear ();
}

void g_AddLinkIntoSelectionList(DTALink* pLink, int link_no, int document_no, bool b_SelectOtherDocuments, double x, double y)
{
	s_link_selection element;
	element.link_no = link_no;
	element.document_no = document_no;
	element.pLink = pLink;

	g_LinkDisplayList.push_back(element);

	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_DocumentNo != document_no)  // not the current document
		{
			GDPoint point;
			point.x = x;
			point.y = y;
			double matching_distance = 0;
			int sel_link_no = (*iDoc)->SelectLink(point, matching_distance);
			if(sel_link_no>=0)  //select link
			{
				element.link_no = sel_link_no;
				element.document_no = (*iDoc)->m_DocumentNo ;
				g_LinkDisplayList.push_back(element);
			}
			//update anyway
			(*iDoc)->UpdateAllViews (0);

		}

		iDoc++;
	}

}
// CTLiteDoc

vector<float> g_time_parser(string str)
{
	vector<float> output_global_minute;

	int string_lenghth = str.length();

	ASSERT(string_lenghth < 100);

	const char *string_line = str.data(); //string to char*

	int char_length = strlen(string_line);

	char ch, buf_ddhhmm[32] = { 0 }, buf_SS[32] = { 0 }, buf_sss[32] = { 0 };
	char dd1, dd2, hh1, hh2, mm1, mm2, SS1, SS2, sss1, sss2, sss3;
	float ddf1, ddf2, hhf1, hhf2, mmf1, mmf2, SSf1, SSf2, sssf1, sssf2, sssf3;
	float global_minute = 0;
	float dd = 0, hh = 0, mm = 0, SS = 0, sss = 0;
	int i = 0;
	int buffer_i = 0, buffer_k = 0, buffer_j = 0;
	int num_of_colons = 0;

	//DDHHMM:SS:sss or HHMM:SS:sss

	while (i < char_length)
	{
		ch = string_line[i++];

		if (num_of_colons == 0 && ch != '_' && ch != ':') //input to buf_ddhhmm until we meet the colon
		{
			buf_ddhhmm[buffer_i++] = ch;
		}
		else if (num_of_colons == 1 && ch != ':') //start the Second "SS"
		{
			buf_SS[buffer_k++] = ch;
		}
		else if (num_of_colons == 2 && ch != ':') //start the Millisecond "sss"
		{
			buf_sss[buffer_j++] = ch;
		}

		if (ch == '_' || i == char_length) //start a new time string
		{
			if (buffer_i == 4) //"HHMM"
			{
				//HHMM, 0123
				hh1 = buf_ddhhmm[0]; //read each first
				hh2 = buf_ddhhmm[1];
				mm1 = buf_ddhhmm[2];
				mm2 = buf_ddhhmm[3];

				hhf1 = ((float)hh1 - 48); //convert a char to a float
				hhf2 = ((float)hh2 - 48);
				mmf1 = ((float)mm1 - 48);
				mmf2 = ((float)mm2 - 48);

				dd = 0;
				hh = hhf1 * 10 * 60 + hhf2 * 60;
				mm = mmf1 * 10 + mmf2;
			}
			else if (buffer_i == 6) //"DDHHMM"
			{
				//DDHHMM, 012345
				dd1 = buf_ddhhmm[0]; //read each first
				dd2 = buf_ddhhmm[1];
				hh1 = buf_ddhhmm[2];
				hh2 = buf_ddhhmm[3];
				mm1 = buf_ddhhmm[4];
				mm2 = buf_ddhhmm[5];

				ddf1 = ((float)dd1 - 48); //convert a char to a float
				ddf2 = ((float)dd2 - 48);
				hhf1 = ((float)hh1 - 48);
				hhf2 = ((float)hh2 - 48);
				mmf1 = ((float)mm1 - 48);
				mmf2 = ((float)mm2 - 48);

				dd = ddf1 * 10 * 24 * 60 + ddf2 * 24 * 60;
				hh = hhf1 * 10 * 60 + hhf2 * 60;
				mm = mmf1 * 10 + mmf2;
			}

			if (num_of_colons == 1 || num_of_colons == 2)
			{
				//SS, 01
				SS1 = buf_SS[0]; //read each first
				SS2 = buf_SS[1];

				SSf1 = ((float)SS1 - 48); //convert a char to a float
				SSf2 = ((float)SS2 - 48);

				SS = (SSf1 * 10 + SSf2) / 60;
			}

			if (num_of_colons == 2)
			{
				//sss, 012
				sss1 = buf_sss[0]; //read each first
				sss2 = buf_sss[1];
				sss3 = buf_sss[2];

				sssf1 = ((float)sss1 - 48); //convert a char to a float
				sssf2 = ((float)sss2 - 48);
				sssf3 = ((float)sss3 - 48);

				sss = (sssf1 * 100 + sssf2 * 10 + sssf3) / 1000;
			}

			global_minute = dd + hh + mm + SS + sss;

			output_global_minute.push_back(global_minute);

			//initialize the parameters
			buffer_i = 0;
			buffer_k = 0;
			buffer_j = 0;
			num_of_colons = 0;
		}

		if (ch == ':')
		{
			num_of_colons += 1;
		}
	}

	return output_global_minute;
}

IMPLEMENT_DYNCREATE(CTLiteDoc, CDocument)

BEGIN_MESSAGE_MAP(CTLiteDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CTLiteDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_PROJECT, &CTLiteDoc::OnFileSaveProject)
	ON_COMMAND(ID_FILE_SAVE_PROJECT_AS, &CTLiteDoc::OnFileSaveProjectAs)
	ON_COMMAND(ID_IMAGE_IMPORTBACKGROUNDIMAGE, &CTLiteDoc::OnImageImportbackgroundimage)
	ON_COMMAND(ID_FILE_DATALOADINGSTATUS, &CTLiteDoc::OnFileDataloadingstatus)
	ON_COMMAND(ID_MOE_VOLUME, &CTLiteDoc::OnMoeVolume)
	ON_COMMAND(ID_MOE_SPEED, &CTLiteDoc::OnMoeSpeed)
	ON_UPDATE_COMMAND_UI(ID_MOE_VOLUME, &CTLiteDoc::OnUpdateMoeVolume)
	ON_UPDATE_COMMAND_UI(ID_MOE_SPEED, &CTLiteDoc::OnUpdateMoeSpeed)
	ON_COMMAND(ID_MOE_NONE, &CTLiteDoc::OnMoeNone)
	ON_UPDATE_COMMAND_UI(ID_MOE_NONE, &CTLiteDoc::OnUpdateMoeNone)
	ON_COMMAND(ID_MOE_VC_Ratio, &CTLiteDoc::OnMoeVcRatio)
	ON_UPDATE_COMMAND_UI(ID_MOE_VC_Ratio, &CTLiteDoc::OnUpdateMoeVcRatio)
	ON_COMMAND(ID_MOE_TRAVELTIME, &CTLiteDoc::OnMoeTraveltime)
	ON_UPDATE_COMMAND_UI(ID_MOE_TRAVELTIME, &CTLiteDoc::OnUpdateMoeTraveltime)
	ON_COMMAND(ID_MOE_CAPACITY, &CTLiteDoc::OnMoeCapacity)
	ON_UPDATE_COMMAND_UI(ID_MOE_CAPACITY, &CTLiteDoc::OnUpdateMoeCapacity)
	ON_COMMAND(ID_MOE_SPEEDLIMIT, &CTLiteDoc::OnMoeSpeedlimit)
	ON_UPDATE_COMMAND_UI(ID_MOE_SPEEDLIMIT, &CTLiteDoc::OnUpdateMoeSpeedlimit)
	ON_COMMAND(ID_MOE_FREEFLOWTRAVLETIME, &CTLiteDoc::OnMoeFreeflowtravletime)
	ON_UPDATE_COMMAND_UI(ID_MOE_FREEFLOWTRAVLETIME, &CTLiteDoc::OnUpdateMoeFreeflowtravletime)
	ON_COMMAND(ID_EDIT_DELETESELECTEDLINK, &CTLiteDoc::OnEditDeleteselectedlink)
	ON_COMMAND(ID_EDIT_SETDEFAULTLINKPROPERTIESFORNEWLINKS, &CTLiteDoc::OnEditSetdefaultlinkpropertiesfornewlinks)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETDEFAULTLINKPROPERTIESFORNEWLINKS, &CTLiteDoc::OnUpdateEditSetdefaultlinkpropertiesfornewlinks)
	ON_COMMAND(ID_TOOLS_PROJECTFOLDER, &CTLiteDoc::OnToolsProjectfolder)
	ON_COMMAND(ID_TOOLS_OPENNEXTAPROGRAMFOLDER, &CTLiteDoc::OnToolsOpennextaprogramfolder)
	ON_COMMAND(ID_ODTABLE_IMPORT_OD_TRIP_FILE, &CTLiteDoc::OnOdtableImportOdTripFile)
	ON_COMMAND(ID_SEARCH_LINKLIST, &CTLiteDoc::OnSearchLinklist)
	ON_COMMAND(ID_MOE_Agent, &CTLiteDoc::OnMoeAgent)
	ON_UPDATE_COMMAND_UI(ID_MOE_Agent, &CTLiteDoc::OnUpdateMoeAgent)
	ON_COMMAND(ID_TOOLS_VIEWSIMULATIONSUMMARY, &CTLiteDoc::OnToolsViewsimulationsummary)
	ON_COMMAND(ID_TOOLS_VIEWASSIGNMENTSUMMARYLOG, &CTLiteDoc::OnToolsViewassignmentsummarylog)
	ON_COMMAND(ID_HELP_VISITDEVELOPMENTWEBSITE, &CTLiteDoc::OnHelpVisitdevelopmentwebsite)
	ON_COMMAND(ID_TOOLS_RUNTRAFFICASSIGNMENT, &CTLiteDoc::OnToolsRuntrafficassignment)
	ON_COMMAND(ID_FILE_CHANGECOORDINATESTOLONG, &CTLiteDoc::OnFileChangecoordinatestolong)
	ON_COMMAND(ID_TOOLS_EXPORTOPMODEDISTRIBUTION, &CTLiteDoc::OnToolsExportopmodedistribution)
	ON_COMMAND(ID_RESEARCHTOOLS_EXPORTTODTALITESENSORDATAFORMAT, &CTLiteDoc::OnResearchtoolsExporttodtalitesensordataformat)
	ON_COMMAND(ID_SCENARIO_CONFIGURATION, &CTLiteDoc::OnScenarioConfiguration)
	ON_COMMAND(ID_MOE_AgentPATHANALAYSIS, &CTLiteDoc::OnMoeAgentpathanalaysis)
	ON_COMMAND(ID_IMPORT_NGSIM_FILE, &CTLiteDoc::OnImportNgsimFile)
	ON_COMMAND(ID_LINK_AgentSTATISTICSANALAYSIS, &CTLiteDoc::OnLinkAgentstatisticsanalaysis)
	ON_COMMAND(ID_SUBAREA_DELETESUBAREA, &CTLiteDoc::OnSubareaDeletesubarea)
	ON_COMMAND(ID_TOOLS_TRAVELTIMERELIABILITYANALYSIS, &CTLiteDoc::OnToolsTraveltimereliabilityanalysis)
	ON_COMMAND(ID_LINK_LINKBAR, &CTLiteDoc::OnLinkLinkbar)
	ON_COMMAND(ID_LINK_INCREASEOFFSETFORTWO, &CTLiteDoc::OnLinkIncreaseoffsetfortwo)
	ON_COMMAND(ID_LINK_DECREASEOFFSETFORTWO, &CTLiteDoc::OnLinkDecreaseoffsetfortwo)
	ON_COMMAND(ID_VIEW_SHOWHIDE_LEGEND, &CTLiteDoc::OnViewShowhideLegend)
	ON_COMMAND(ID_MOE_VIEWLINKMOESUMMARYFILE, &CTLiteDoc::OnMoeViewlinkmoesummaryfile)
	ON_COMMAND(ID_VIEW_CALIBRATIONVIEW, &CTLiteDoc::OnViewCalibrationview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CALIBRATIONVIEW, &CTLiteDoc::OnUpdateViewCalibrationview)
	ON_COMMAND(ID_MOE_VIEWODDEMANDESTIMATIONSUMMARYPLOT, &CTLiteDoc::OnMoeViewoddemandestimationsummaryplot)
	ON_COMMAND(ID_DELETE_SELECTED_LINK, &CTLiteDoc::OnDeleteSelectedLink)
	ON_COMMAND(ID_EXPORT_GENERATEZONE, &CTLiteDoc::OnExportGenerateTravelTimeMatrix)
	ON_COMMAND(ID_EXPORT_GENERATESHAPEFILES, &CTLiteDoc::OnExportGenerateshapefiles)
	ON_COMMAND(ID_LINKMOEDISPLAY_QUEUELENGTH, &CTLiteDoc::OnLinkmoedisplayQueuelength)
	ON_UPDATE_COMMAND_UI(ID_LINKMOEDISPLAY_QUEUELENGTH, &CTLiteDoc::OnUpdateLinkmoedisplayQueuelength)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TRAVELTIMERELIABILITYANALYSIS, &CTLiteDoc::OnUpdateLinkmoeTraveltimereliability)

	ON_COMMAND(ID_MOE_PATHLIST, &CTLiteDoc::OnMoePathlist)
	ON_COMMAND(ID_VIEW_SHOWMOE, &CTLiteDoc::OnViewShowmoe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWMOE, &CTLiteDoc::OnUpdateViewShowmoe)
	ON_COMMAND(ID_FILE_UPLOADLINKDATATOGOOGLEFUSIONTABLE, &CTLiteDoc::OnFileUploadlinkdatatogooglefusiontable)
	ON_COMMAND(ID_3_VIEWDATAINEXCEL, &CTLiteDoc::On3Viewdatainexcel)
	ON_COMMAND(ID_5_VIEWDATAINEXCEL, &CTLiteDoc::On5Viewdatainexcel)
	ON_COMMAND(ID_MOE_VIEWNETWORKTIMEDEPENDENTMOE, &CTLiteDoc::OnMoeViewnetworktimedependentmoe)
	ON_COMMAND(ID_2_VIEWDATAINEXCEL33398, &CTLiteDoc::On2Viewdatainexcel33398)
	ON_COMMAND(ID_2_VIEWNETWORKDATA, &CTLiteDoc::On2Viewnetworkdata)
	ON_COMMAND(ID_3_VIEWODDATAINEXCEL, &CTLiteDoc::On3Viewoddatainexcel)
	ON_COMMAND(ID_MOE_OPENALLMOETABLES, &CTLiteDoc::OnMoeOpenallmoetables)
	ON_BN_CLICKED(IDC_BUTTON_Database, &CTLiteDoc::OnBnClickedButtonDatabase)
	ON_COMMAND(ID_TOOLS_UNITTESTING, &CTLiteDoc::OnToolsUnittesting)
	ON_COMMAND(ID_NODE_INCREASENODETEXTSIZE, &CTLiteDoc::OnNodeIncreasenodetextsize)
	ON_COMMAND(ID_NODE_DECREASENODETEXTSIZE, &CTLiteDoc::OnNodeDecreasenodetextsize)
	ON_COMMAND(ID_IMPORT_SYNCHROUTDFCSVFILES, &CTLiteDoc::OnImportSynchroutdfcsvfiles)
	ON_COMMAND(ID_PROJECT_EDITMOESETTINGS, &CTLiteDoc::OnProjectEditmoesettings)
	ON_COMMAND(ID_PROJECT_12, &CTLiteDoc::OnProject12)
	ON_COMMAND(ID_VIEW_MOVEMENT_MOE, &CTLiteDoc::OnViewMovementMoe)
	ON_COMMAND(ID_PROJECT_TIME_DEPENDENT_LINK_MOE, &CTLiteDoc::OnProjectTimeDependentLinkMoe)
	ON_COMMAND(ID_LINKATTRIBUTEDISPLAY_LINKNAME, &CTLiteDoc::OnLinkattributedisplayLinkname)
	ON_UPDATE_COMMAND_UI(ID_LINKATTRIBUTEDISPLAY_LINKNAME, &CTLiteDoc::OnUpdateLinkattributedisplayLinkname)
	ON_COMMAND(ID_PROJECT_1_NETWORK, &CTLiteDoc::OnProjectNetworkData)
	ON_COMMAND(ID_MOE_MOE, &CTLiteDoc::OnMoeTableDialog)
	ON_COMMAND(ID_SENSORTOOLS_CONVERTTOHOURCOUNT, &CTLiteDoc::OnSensortoolsConverttoHourlyVolume)
	ON_COMMAND(ID_IMPORT_INRIXSHAPEFILEANDSPEEDDATA, &CTLiteDoc::OnImportInrixshapefileandspeeddata)
	ON_COMMAND(ID_NETWORKTOOLS_RESETLINKLENGTH, &CTLiteDoc::OnNetworktoolsResetlinklength)
	ON_COMMAND(ID_SUBAREA_CREATEZONEFROMSUBAREA, &CTLiteDoc::OnSubareaCreatezonefromsubarea)
	ON_COMMAND(ID_NODE_VIEWNODEDATA, &CTLiteDoc::OnNodeViewnodedata)
	ON_COMMAND(ID_LINK_VIEWLINKDATA, &CTLiteDoc::OnLinkViewlinkdata)
	ON_COMMAND(ID_MOVEMENT_VIEWMOVEMENTDATATABLE, &CTLiteDoc::OnMovementViewmovementdatatable)
	ON_COMMAND(ID_ODMATRIX_ODDEMANDMATRIX, &CTLiteDoc::OnOdmatrixOddemandmatrix)
	ON_COMMAND(ID_SHOW_MOE_PATHLIST, &CTLiteDoc::OnShowMoePathlist)
	ON_COMMAND(ID_EXPORT_EXPORTAGGREGATEDLINKMOEFILE, &CTLiteDoc::OnExportExportaggregatedlinkmoefile)
	ON_COMMAND(ID_DETECTOR_EXPORTLINKFLOWPROPORTIONMATRIXTOCSVFILE, &CTLiteDoc::OnDetectorExportlinkflowproportionmatrixtocsvfile)
	ON_COMMAND(ID_TRAFFICCAPACITY_SETDEFAULT, &CTLiteDoc::OnTrafficcapacitySetdefault)
	ON_COMMAND(ID_MOVEMENT_HIDENON, &CTLiteDoc::OnMovementHidenon)
	ON_UPDATE_COMMAND_UI(ID_MOVEMENT_HIDENON, &CTLiteDoc::OnUpdateMovementHidenon)
	ON_COMMAND(ID_MOVEMENT_SETPEAKHOURFACTOR, &CTLiteDoc::OnMovementSetpeakhourfactor)
	ON_COMMAND(ID_ZONE_CHANGEZONENUMBER, &CTLiteDoc::OnZoneChangezonenumber)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_FREEWAY, &CTLiteDoc::OnChangelinktypecolorFreeway)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_RAMP, &CTLiteDoc::OnChangelinktypecolorRamp)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_ARTERIAL, &CTLiteDoc::OnChangelinktypecolorArterial)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_CONNECTOR, &CTLiteDoc::OnChangelinktypecolorConnector)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_TRANSIT, &CTLiteDoc::OnChangelinktypecolorTransit)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_WALKINGMODE, &CTLiteDoc::OnChangelinktypecolorWalkingmode)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_RESETTODEFAULTCOLORSCHEMA, &CTLiteDoc::OnChangelinktypecolorResettodefaultcolorschema)
	ON_COMMAND(ID_NODE_CHANGENODECOLOR, &CTLiteDoc::OnNodeChangenodecolor)
	ON_COMMAND(ID_NODE_CHANGENODEBACKGROUNDCOLOR, &CTLiteDoc::OnNodeChangenodebackgroundcolor)
	ON_COMMAND(ID_ZONE_CHANGEZONECOLOR, &CTLiteDoc::OnZoneChangezonecolor)
	ON_COMMAND(ID_EDIT_UNDO33707, &CTLiteDoc::OnEditUndo33707)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO33707, &CTLiteDoc::OnUpdateEditUndo33707)
	ON_COMMAND(ID_EDIT_REDO33709, &CTLiteDoc::OnEditRedo33709)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO33709, &CTLiteDoc::OnUpdateEditRedo33709)
	ON_COMMAND(ID_MOVEMENT_SETUPNUMBEROFLEFTTURNLANESFORSIGNALIZEDNODES, &CTLiteDoc::OnMovementSetupnumberofleftturnlanesforsignalizednodes)
	ON_COMMAND(ID_MOVEMENT_OPTIMIZEPHASINGANDTIMINGDATAFORALLTRAFFICSIGNALSTHROUGHQEM, &CTLiteDoc::OnMovementOptimizephasingandtimingdataforalltrafficsignalsthroughqem)
	ON_COMMAND(ID_LINKMOEDISPLAY_IMPACT, &CTLiteDoc::OnLinkmoedisplayImpact)
	ON_UPDATE_COMMAND_UI(ID_LINKMOEDISPLAY_IMPACT, &CTLiteDoc::OnUpdateLinkmoedisplayImpact)
	ON_COMMAND(ID_LINKMOEDISPLAY_BOTTLENECK, &CTLiteDoc::OnLinkmoedisplayBottleneck)
	ON_UPDATE_COMMAND_UI(ID_LINKMOEDISPLAY_BOTTLENECK, &CTLiteDoc::OnUpdateLinkmoedisplayBottleneck)
	ON_COMMAND(ID_GISPLANNINGDATASET_CONFIGUREIMPORTINGSETTINGFILE, &CTLiteDoc::OnGisplanningdatasetConfigureimportingsettingfile)
	ON_COMMAND(ID_SUBAREA_EXPORTTOTALNUMBEROFAgentSINSUBAREA, &CTLiteDoc::OnSubareaExporttotalnumberofAgentsinsubarea)
	ON_COMMAND(ID_TOOLS_CONFIGURATION, &CTLiteDoc::OnToolsConfiguration)
	ON_COMMAND(ID_PATH_CLEARALLPATHDISPLAY, &CTLiteDoc::OnPathClearallpathdisplay)
		ON_COMMAND(ID_TOOLS_IMPORTSYNCHRONCOMBINEDFILE, &CTLiteDoc::OnToolsImportsynchroncombinedfile)
		ON_COMMAND(ID_ODMATRIX_RAPID, &CTLiteDoc::OnOdmatrixRapid)
		ON_COMMAND(ID_ODMATRIX_RAPID_Decrease, &CTLiteDoc::OnOdmatrixRapidDecrease)
		ON_COMMAND(ID_BUTTON_ABM, &CTLiteDoc::OnButtonAbm)
		ON_COMMAND(ID_HELP_VISITDEVELOPMENTWEBSITE_DTALite, &CTLiteDoc::OnHelpVisitdevelopmentwebsiteDtalite)
		END_MESSAGE_MAP()


// CTLiteDoc construction/destruction

CTLiteDoc::CTLiteDoc()
{
	m_DemandAlpha = 1.0;
	bSynchroImportReadyToSaveFlag = false;

	m_calibration_data_start_time_in_min = 0;
	m_calibration_data_end_time_in_min = 1440;

	m_ImageMoveSize = 0.0001;
	m_SensorMapX = 0;
	m_SensorMapY = 0;
	
	m_SensorMapXResolution = 1;
	m_SensorMapYResolution = 1;
	m_SensorMapMoveSize = 0.0001;

	m_PrimaryDataSource = eSimulationData;

	AddNameIntoTimingPlanVector(0, 1440,"0");

	m_CurrentDisplayTimingPlanName = "0";



	m_MovementTextBoxSizeInDistance = 50;
	m_bShowSignalNodeMovementOnly = true;

	m_hide_non_specified_movement_on_freeway_and_ramp = true;

	m_sensor_data_aggregation_type = 0;  // no processing, 1:  hourly data: 2: hourly and lane-based data 

	m_GridMatrix = Allocate3DDynamicArray<GridNodeSet>(_MAX_TRANSIT_GRID_SIZE,_MAX_TRANSIT_GRID_SIZE ,_MAX_TRANSIT_TIME_SIZE );

	m_TurnDirectionStringMap["NBL2"] =  DTA_NBL2;
	m_TurnDirectionStringMap["NBL"] =  DTA_NBL;
	m_TurnDirectionStringMap["NBT"] =  DTA_NBT;
	m_TurnDirectionStringMap["NBR"] =  DTA_NBR;
	m_TurnDirectionStringMap["NBR2"] =  DTA_NBR2;

	m_TurnDirectionStringMap["SBL2"] =  DTA_SBL2;
	m_TurnDirectionStringMap["SBL"] =  DTA_SBL;
	m_TurnDirectionStringMap["SBT"] =  DTA_SBT;
	m_TurnDirectionStringMap["SBR"] =  DTA_SBR;
	m_TurnDirectionStringMap["SBR2"] =  DTA_SBR2;

	m_TurnDirectionStringMap["EBL2"] =  DTA_EBL2;
	m_TurnDirectionStringMap["EBL"] =  DTA_EBL;
	m_TurnDirectionStringMap["EBT"] =  DTA_EBT;
	m_TurnDirectionStringMap["EBR"] =  DTA_EBR;
	m_TurnDirectionStringMap["EBR2"] =  DTA_EBR2;

	m_TurnDirectionStringMap["WBL2"] =  DTA_WBL2;
	m_TurnDirectionStringMap["WBL"] =  DTA_WBL;
	m_TurnDirectionStringMap["WBT"] =  DTA_WBT;
	m_TurnDirectionStringMap["WBR"] =  DTA_WBR;
	m_TurnDirectionStringMap["WBR2"] =  DTA_WBR2;

	m_PeakHourFactor = 1.0;
	m_bIdentifyBottleneckAndOnOffRamps = false;
	m_ScreenWidth_InMile = 10;

	m_bUseMileVsKMFlag = true;
	m_bRightHandTrafficFlag = 1;
	m_ImageWidthInMile = 1;
	m_PointA_x = m_PointA_y = m_PointB_x = m_PointB_y = 0;
	m_PointA_long = m_PointA_lat = m_PointB_long = m_PointB_lat= 0;
	m_bPointA_Initialized =  m_bPointB_Initialized = false;

	m_bSummaryDialog = false;
	m_StartNodeNumberForNewNodes = 1;
	m_demand_multiplier = 1;

	m_number_of_iterations = 20;
	m_number_of_reporting_days = 1;
	m_traffic_flow_model = 1;
	m_signal_reresentation_model = 0;
	m_traffic_analysis_method = 0;

	m_ActivityLocationCount = 0;
	m_SearchMode= efind_node;
	m_LongLatFlag = false; // default, we do not know if the coordinate system is long or lat
	g_bValidDocumentChanged = true;

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

	m_DefaultDataFolder.Format ("%s\\default_data_folder\\",pMainFrame->m_CurrentDirectory);

	m_ZoneNoSize  = 0;
	m_bRunCrashPredictionModel = false;
	m_ZoomToSelectedObject = true;
	m_max_walking_distance = 0.25;
	m_max_accessible_transit_time_in_min = 15;

	m_DemandTypeSize = 1; // just one demand trip type:
	m_ODMOEMatrix = NULL;

	m_bMovementAvailableFlag = false;

	m_ImportNetworkAlready = false;
	m_DemandLoadingStartTimeInMin = 420;
	m_DemandLoadingEndTimeInMin = 480;

	m_bSaveProjectFromSubareaCut = false;
	m_bSaveProjectFromImporting = false;
	m_bExport_Link_MOE_in_input_link_CSF_File = false;
	m_bBezierCurveFlag = true;

	m_CriticalOriginZone = -1;
	m_CriticalDestinationZone = -1;

	m_ControlType_UnknownControl = 0;
	m_ControlType_NoControl = 1;
	m_ControlType_YieldSign = 2;
	m_ControlType_2wayStopSign = 3;
	m_ControlType_4wayStopSign = 4;
	m_ControlType_PretimedSignal = 5;
	m_ControlType_ActuatedSignal = 6;
	m_ControlType_Roundabout = 7;
	m_ControlType_ExternalNode = 100;


	m_LinkTypeMap[1].link_type = 1;
	m_LinkTypeMap[1].link_type_name = "Freeway";
	m_LinkTypeMap[1].type_code = "f";

	m_LinkTypeMap[2].link_type = 2;
	m_LinkTypeMap[2].link_type_name = "Highway";
	m_LinkTypeMap[2].type_code = "h";

	m_LinkTypeMap[3].link_type = 3;
	m_LinkTypeMap[3].link_type_name = "Principal arterial";
	m_LinkTypeMap[3].type_code = "a";

	m_LinkTypeMap[4].link_type = 4;
	m_LinkTypeMap[4].link_type_name = "Major arterial";
	m_LinkTypeMap[4].type_code = "a";

	m_LinkTypeMap[5].link_type = 5;
	m_LinkTypeMap[5].link_type_name = "Minor arterial";
	m_LinkTypeMap[5].type_code = "a";

	m_LinkTypeMap[6].link_type = 6;
	m_LinkTypeMap[6].link_type_name = "Collector";
	m_LinkTypeMap[6].type_code = "a";

	m_LinkTypeMap[7].link_type = 7;
	m_LinkTypeMap[7].link_type_name = "Local";
	m_LinkTypeMap[7].type_code = "a";

	m_LinkTypeMap[8].link_type = 8;
	m_LinkTypeMap[8].link_type_name = "Frontage road";
	m_LinkTypeMap[8].type_code = "a";

	m_LinkTypeMap[9].link_type = 9;
	m_LinkTypeMap[9].link_type_name = "Ramp";
	m_LinkTypeMap[9].type_code = "r";

	m_LinkTypeMap[10].link_type = 10;
	m_LinkTypeMap[10].link_type_name = "Zonal connector";
	m_LinkTypeMap[10].type_code = "c";

	m_LinkTypeMap[100].link_type = 100;
	m_LinkTypeMap[100].link_type_name = "Transit link";
	m_LinkTypeMap[100].type_code = "t";

	m_LinkTypeMap[200].link_type = 200;
	m_LinkTypeMap[200].link_type_name = "Walking link";
	m_LinkTypeMap[200].type_code = "w";

	m_LinkTypeFreeway = 1;
	m_LinkTypeArterial = 3;
	m_LinkTypeHighway = 2;


	m_OriginOnBottomFlag = 1;

	m_DocumentNo = g_DocumentList.size();
	g_DocumentList.push_back (this);

	m_RandomRoutingCoefficient = 0.0f;
	m_bGPSDataSet = false;
	m_bEmissionDataAvailable = false;
	m_AdjLinkSize = 20;  // initial value
	m_ColorDirection = -1;
	m_colorLOS[0] = RGB(190,190,190);
	m_colorLOS[1] = RGB(0,255,0);
	m_colorLOS[2] = RGB(255,250,117);
	m_colorLOS[3] = RGB(255,250,0);
	m_colorLOS[4] = RGB(255,216,0);
	m_colorLOS[5] = RGB(255,153,0);
	m_colorLOS[6] = RGB(255,0,0);


	m_ColorDirectionVector[DTA_South] = RGB(255,69,0 );  // orange red
	m_ColorDirectionVector[DTA_North] = RGB(255,255,255); // white
	m_ColorDirectionVector[DTA_East] = RGB(192,192,192 ); // Silver
	m_ColorDirectionVector[DTA_West] = RGB(169,169,169); // gray

	m_ColorDirectionVector[DTA_NorthEast] = RGB(255,69,0);  //orange red
	m_ColorDirectionVector[DTA_NorthWest] = RGB(255,69,0); //orange red
	m_ColorDirectionVector[DTA_SouthEast] = RGB(255,255,255 ); // white
	m_ColorDirectionVector[DTA_SouthWest] = RGB(255,255,255); // white
	m_ColorDirectionVector[DTA_NotDefined] = RGB(169,169,169); // grey



	m_MaxLinkWidthAsNumberOfLanes = 5;
	m_MaxLinkWidthAsLinkVolume = 1000;

	m_AgentSelectionMode = CLS_network;
	m_LinkBandWidthMode = LBW_number_of_lanes;
	m_bLoadNetworkDataOnly = false;

	m_bSimulationDataLoaded  = false;
	m_EmissionDataFlag = false;
	m_bLinkToBeShifted = true;
	m_SimulationStartTime_in_min = 0;  // 6 AM
	m_SimulationEndTime_in_min = 1440;


	m_LinkMOEMode = MOE_none;
	m_PrevLinkMOEMode = MOE_fftt;  // make sure the document gets a change to initialize the display view as the first mode is MOE_none
	m_ODMOEMode = odnone;

	MaxNodeKey = 60000;  // max: unsigned short 65,535;
	m_BackgroundBitmapLoaded  = false;
	m_LongLatCoordinateFlag = false;
	m_ColorFreeway = RGB(198,226,255);
	m_ColorHighway = RGB(100,149,237);
	m_ColorArterial = RGB(0,0,0);
	m_pNetwork = NULL;
	m_FromNodeID = -1;

	m_ToNodeID = -1;
	m_NodeSizeSP = 0;


	m_PathMOEDlgShowFlag = false;
	m_SelectPathNo = -1;

	m_ImageX1 = 0;
	m_ImageX2 = 1000;
	m_ImageY1 = 0;
	m_ImageY2 = 1000;

	m_ZoneTextColor = RGB(0,191,255);

	m_bLoadMovementData = true;

	if(theApp.m_VisulizationTemplate == e_traffic_assignment)
	{

		m_NodeDisplaySize = 100;  // in feet
		m_BottleneckDisplaySize = 1;
		m_AgentDisplaySize = 10; // in feet
		theApp.m_BackgroundColor =  RGB(255,255,255);  //white
		m_NodeTextDisplayRatio = 4;

	}

	if(theApp.m_VisulizationTemplate == e_train_scheduling)
	{
		m_NodeDisplaySize = 2000;  // in feet
		m_AgentDisplaySize= 1000; // in feet
		theApp.m_BackgroundColor =  RGB(255,255,255);
		m_NodeTextDisplayRatio = 8;
	}


	char CurrentDirectory[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, CurrentDirectory);

	CString NEXTASettingsPath;
	NEXTASettingsPath.Format("%s\\NEXTA_Settings.ini", CurrentDirectory);

	m_NetworkRect.top  = g_GetPrivateProfileDouble("Grid", "top", 50, NEXTASettingsPath);
	m_NetworkRect.bottom = g_GetPrivateProfileDouble("Grid", "bottom", 0, NEXTASettingsPath);

	m_NetworkRect.left   = g_GetPrivateProfileDouble("Grid", "left", 0, NEXTASettingsPath);
	m_NetworkRect.right = g_GetPrivateProfileDouble("Grid", "right", 100, NEXTASettingsPath);

	m_UnitDistance = 1;


	m_OffsetInDistance = 15;
	m_LaneWidthInMeter = 10;
	m_bFitNetworkInitialized = false; 

	m_DefaultNumLanes = 1;
	m_DefaultSpeedLimit = 65.0f;
	m_DefaultCapacity = 1900.0f;
	m_DefaultLinkType = 1;

	m_ODSize = 0;
	m_PreviousODSize = -1;
	m_SelectedLinkNo = -1;
	m_SelectedNodeID = -1;
	m_SelectedZoneID = -1;
	m_SelectedAgentID = -1;

	m_bSetView = false;
	m_bShowLegend = false;
	m_bShowPathList = true;

	for(int i=0; i<40;i++)
	{
		for(int los= 0; los < MAX_LOS_SIZE; los++)
		{
			m_LOSBound[i][los] = 0;
		}
	}

	// speed LOS bound
	m_LOSBound[MOE_speed][1] = 100;
	m_LOSBound[MOE_speed][2] = 80;
	m_LOSBound[MOE_speed][3] = 65;
	m_LOSBound[MOE_speed][4] = 50;
	m_LOSBound[MOE_speed][5] = 40;
	m_LOSBound[MOE_speed][6] = 33;
	m_LOSBound[MOE_speed][7] = 0;

	m_LOSBound[MOE_reliability][1] = 0;
	m_LOSBound[MOE_reliability][2] = 0.1f;
	m_LOSBound[MOE_reliability][3] = 0.2f;
	m_LOSBound[MOE_reliability][4] = 0.3f;
	m_LOSBound[MOE_reliability][5] = 0.5f;
	m_LOSBound[MOE_reliability][6] = 0.7f;
	m_LOSBound[MOE_reliability][7] = 999;

	m_LOSBound[MOE_density][1] = 0;
	m_LOSBound[MOE_density][2] = 11;
	m_LOSBound[MOE_density][3] = 17;
	m_LOSBound[MOE_density][4] = 25;
	m_LOSBound[MOE_density][5] = 35;
	m_LOSBound[MOE_density][6] = 45;
	m_LOSBound[MOE_density][7] = 999;

	m_LOSBound[MOE_impact][7] = 100;
	m_LOSBound[MOE_impact][6] = 83.3;
	m_LOSBound[MOE_impact][5] = 66.7;
	m_LOSBound[MOE_impact][4] = 50.0;
	m_LOSBound[MOE_impact][3] = 33.3;
	m_LOSBound[MOE_impact][2] = 16.7;
	m_LOSBound[MOE_impact][1] = 0;


	m_LOSBound[MOE_bottleneck][7] = 100;
	m_LOSBound[MOE_bottleneck][6] = 83.3;
	m_LOSBound[MOE_bottleneck][5] = 66.7;
	m_LOSBound[MOE_bottleneck][4] = 50.0;
	m_LOSBound[MOE_bottleneck][3] = 33.3;
	m_LOSBound[MOE_bottleneck][2] = 16.7;
	m_LOSBound[MOE_bottleneck][1] = 0;



	// http://www.epa.gov/otaq/consumer/420f08024.pdf
	m_LOSBound[MOE_emissions ][0] = 100.0;
	m_LOSBound[MOE_emissions ][1] = 40.0;
	m_LOSBound[MOE_emissions][2] =  35.0;
	m_LOSBound[MOE_emissions ][3] = 30.0;;
	m_LOSBound[MOE_emissions][4] = 25.0;;
	m_LOSBound[MOE_emissions][5] = 20.0;;
	m_LOSBound[MOE_emissions][6] = 15.0;;
	m_LOSBound[MOE_emissions][7] =  10.0;


	float max_value[10];
	max_value[1]= 2.0000;
	max_value[2]= 5.0000;
	max_value[3]= 5.0000;
	max_value[4]= 0.6;

	for(int type =1; type<=4; type++)
	{
		for(int los = 0; los<=7; los++)
		{
			m_LOSBound[MOE_emissions + type][los] = max_value[type]*(los-1)/7.0;
		}
	}
	m_TrafficFlowModelFlag = 1;  // static traffic assignment as default
	m_Doc_Resolution = 1;
	m_bShowCalibrationResults = false;

	m_SampleExcelNetworkFile = "\\sample_data_sets\\2. Importing_Creating_Traffic_Modeling_Data_Sets\\0. Import_Excel_files\\";
	//	m_SampleExcelSensorFile = "\\Sample_Import_Excel_Files\\input_Portland_sensor_data.xls";

	TDDemandSOVMatrix = NULL;
	TDDemandHOVMatrix = NULL;
	TDDemandTruckMatrix = NULL;

}

static bool DeleteLinkPointer( DTALink * theElement ) { delete theElement; return true; }

void CTLiteDoc::ClearNetworkData()
{
	m_LinkKeyMap.clear ();
	m_SpeedSensorIDMap.clear ();
	m_CountSensorIDMap.clear ();

	m_NodeSet.clear ();
	m_LinkSet.clear ();

	m_NodeNotoZoneNameMap.clear();
	m_NodeNotoLinkMap.clear();

	m_NodeNumbertoNodeNoMap.clear();
	m_ODSize = 0;
	m_PathDisplayList.clear();
	m_LinkIDRecordVector.clear();
	m_MessageStringVector.clear();

	m_DTAPointSet.clear();
	m_DTALineSet.clear();


	m_SubareaNodeSet.clear();
	m_SubareaLinkSet.clear();
	m_ZoneMap.clear();

	m_DemandFileVector.clear();




}

CTLiteDoc::~CTLiteDoc()
{
	if(m_GridMatrix)
		Deallocate3DDynamicArray<GridNodeSet>(m_GridMatrix, _MAX_TRANSIT_GRID_SIZE, _MAX_TRANSIT_GRID_SIZE);


	g_bValidDocumentChanged = true;
	CWaitCursor wait;
	if(m_ODMOEMatrix !=NULL)
		Deallocate3DDynamicArray<AgentStatistics>(m_ODMOEMatrix,	m_PreviousDemandTypeSize, m_PreviousZoneNoSize);


	if(g_LinkMOEDlg!=NULL && g_LinkMOEDlg->m_pDoc == this)
	{
		delete g_LinkMOEDlg;
		g_LinkMOEDlg= NULL;

	}

	

	if(g_pLegendDlg!=NULL &&  g_pLegendDlg->m_pDoc == this)
	{
		delete g_pLegendDlg;
		g_pLegendDlg= NULL;

	}

	if(g_pLinkListDlg!=NULL && g_pLinkListDlg->m_pDoc == this)
	{
		delete g_pLinkListDlg;
		g_pLinkListDlg= NULL;

	}

	if(g_pAgentPathDlg!=NULL && g_pAgentPathDlg->m_pDoc == this)
	{
		delete g_pAgentPathDlg;
		g_pAgentPathDlg= NULL;

	}
	if(g_pPathListDlg!=NULL && g_pPathListDlg->m_pDoc == this)
	{
		delete g_pPathListDlg;
		g_pPathListDlg= NULL;

	}


	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc) == this)
		{
			g_DocumentList.erase (iDoc);  // remove the document to be deleted
			break;

		}
		iDoc++;
	}

	m_NEXTALOGFile.close();

	if(m_pNetwork!=NULL)
		delete m_pNetwork;


	//	m_LinkSet.remove_if (DeleteLinkPointer);

	/*
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
	DTANode* pNode = (*iNode);

	if(pNode!=NULL)
	delete pNode;

	}
	*/
}

BOOL CTLiteDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CTLiteDoc serialization

void CTLiteDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CTLiteDoc diagnostics

#ifdef _DEBUG
void CTLiteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTLiteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



void CTLiteDoc::SetStatusText(CString StatusText)
{
	CTLiteView* pView = 0;
	POSITION pos = GetFirstViewPosition();
	if(pos != NULL)
	{
		pView = (CTLiteView*) GetNextView(pos);
		if(pView!=NULL)
			pView->SetStatusText(StatusText);
	}


}

bool CTLiteDoc::ReadSimulationLinkMOEData_Parser(LPCTSTR lpszFileName)
{
	CCSVParser parser;
	int i= 0;
	if (parser.OpenCSVFile(lpszFileName))
	{

		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			(*iLink)->ResetMOEAry(g_Simulation_Time_Horizon);  // use one day horizon as the default value
		}

		while(parser.ReadRecord())
		{

			string road_link_id;
			if(parser.GetValueByFieldName("road_link_id", road_link_id) == false)
				break;

			DTALink* pLink = FindLinkWithLinkID(road_link_id);

			if(pLink!=NULL)
			{


					string time_period;

					if (parser.GetValueByFieldName("time_period", time_period) == false)
						break;

					vector<float> time_stamp_vector;
					time_stamp_vector = g_time_parser(time_period);


					if (time_stamp_vector.size() != 2)
						continue; 

						float travel_time_in_min = 0;
						float link_volume = 0;
						float density = 0;
						float speed = 0;
						float queue_length_percentage = 0;
						float number_of_queued_agents = 0;

						parser.GetValueByFieldName("travel_time", travel_time_in_min);
						parser.GetValueByFieldName("volume", link_volume);
						parser.GetValueByFieldName("density", density);
						parser.GetValueByFieldName("speed", speed);
						//parser.GetValueByFieldName("queue_length", queue_length_percentage);
						//parser.GetValueByFieldName("number_of_queued_agents", number_of_queued_agents);


						for (int tt = time_stamp_vector[0]; tt < time_stamp_vector[1]; tt++)
						{
							if (tt < pLink ->m_LinkMOEArySize)
							{
								pLink->m_LinkMOEAry[tt].TravelTime = travel_time_in_min;
								pLink->m_LinkMOEAry[tt].LinkFlow = link_volume;
								pLink->m_LinkMOEAry[tt].Density = density;
								pLink->m_LinkMOEAry[tt].Speed = speed;
								//pLink->m_LinkMOEAry[tt].QueueLength = queue_length_percentage;
								//pLink->m_LinkMOEAry[tt].number_of_queued_Agents = number_of_queued_agents;

							}
						}

						i++;
				

			}else
			{
				CString msg;
				msg.Format ("Please check if link %s at file %s still exists inroad_ink.csv.", road_link_id.c_str(), lpszFileName);  // +2 for the first field name line
				AfxMessageBox(msg);
				break;
			}

		}

		m_bSimulationDataLoaded = true;

		g_Simulation_Time_Stamp = 0; // reset starting time
		g_SimulationStartTime_in_min = 0;

		m_SimulationLinkTDMOEDataLoadingStatus.Format ("%d link time-dependent MOE records are loaded from file %s.",i,lpszFileName);
		return true;
	}
	else
	{
		return false;
	}
}





BOOL CTLiteDoc::OnOpenTrafficNetworkDocument(CString ProjectFileName, bool bNetworkOnly, bool bImportShapeFiles)
{


	m_NodeDisplaySize = 50;  // in feet


	CTime LoadingStartTime = CTime::GetCurrentTime();

	m_bLoadNetworkDataOnly = bNetworkOnly;
	FILE* st = NULL;
	//	cout << "Reading file node.csv..."<< endl;

	CString directory;
	m_ProjectFile = ProjectFileName;
	directory = m_ProjectFile.Left(ProjectFileName.ReverseFind('\\') + 1);

	m_ProjectDirectory = directory;
	m_ProjectTitle = GetWorkspaceTitleName(ProjectFileName);
	SetTitle(m_ProjectTitle);

	ReadModelAgentTrajectory(m_ProjectDirectory + "model_trajectory.csv");

	ReadGPSTrajectory(m_ProjectDirectory + "CAR_GPS_data.csv");

	// read users' prespecified control type
	ReadLinkTypeCSVFile(directory+"link_type.csv");

	if(bNetworkOnly ==false)
	{  // we need to check the data consistency here
		ReadDemandTypeCSVFile(directory+"demand_type.csv");
	}

	CWaitCursor wc;
	OpenWarningLogFile(directory);

	ClearNetworkData();
	m_ODSize = 0;


	//m_bUseMileVsKMFlag = (bool)(g_GetPrivateProfileDouble("coordinate_info", "mile_as_unit_of_length", 1, ProjectFileName));

	//m_bRightHandTrafficFlag = g_GetPrivateProfileDouble("coordinate_info", "right_hand_traffic_flag", 1, ProjectFileName);

	//if (m_bRightHandTrafficFlag == 0)
	//	m_bRightHandTrafficFlag = -1;
	//ReadBackgroundImageFile(ProjectFileName);


	if (ReadNodeCSVFile(directory + "node.csv") || m_BackgroundBitmapLoaded == true)
	{
	
	if(!ReadLinkCSVFile(directory+"road_link.csv",false,false)) 
		return false;
	}


	ConstructMovementVector();
	ReadAMSMovementCSVFile(directory + "movement.csv", -1);
//	ReadAMSSignalControlCSVFile(directory + "input_timing.csv");

	if (bNetworkOnly == false)
	{
		LoadSimulationOutput();
	}

	CalculateDrawingRectangle(false);
	m_bFitNetworkInitialized  = false;


	CTime LoadingEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = LoadingEndTime  - LoadingStartTime;
	CString str_running_time;

	str_running_time.Format ("Network loading time: %d min(s) %d sec(s)...",ts.GetMinutes(), ts.GetSeconds());

	SetStatusText(str_running_time);



//	OffsetLink(); we do not need to offset here as the importing function has done so. 

	ReadBackgroundImageFile(ProjectFileName);


	m_AMSLogFile.close();
	return true;
}


BOOL CTLiteDoc::OnOpenDocument(CString ProjectFileName, bool bLoadNetworkOnly )
{
	CWaitCursor wait;

	//m_NodeDisplaySize = max(1, g_GetPrivateProfileDouble("GUI", "node_display_size", 50, ProjectFileName));

	//m_NodeTextDisplayRatio = max(1, g_GetPrivateProfileDouble("GUI", "node_text_display_ratio", 4, ProjectFileName));

	//m_bLoadMovementData = (bool)(g_GetPrivateProfileInt("GUI", "load_movement_data", 1, ProjectFileName));


	CTime LoadingStartTime = CTime::GetCurrentTime();

	if(ProjectFileName.Find("csv")>=0)  //Transportation network project format
	{

		OnOpenTrafficNetworkDocument(ProjectFileName,bLoadNetworkOnly);
	}else
	{
		AfxMessageBox("The selected file type is not selected.");
		return false;	

	}


	CTime LoadingEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = LoadingEndTime  - LoadingStartTime;

	m_StrLoadingTime.Format ("Overall loading time: %d min(s) %d sec(s)...",ts.GetMinutes(), ts.GetSeconds());

	CDlgFileLoading dlg;
	dlg.m_pDoc = this;
	dlg.DoModal ();

	UpdateAllViews(0);
	return true;

}

bool CTLiteDoc::ReadBackgroundImageFile(LPCTSTR lpszFileName, bool bAskForInput)
{
	//read impage file Background.bmp

	if(m_BackgroundBitmapLoaded)
		m_BackgroundBitmap.Detach ();

	m_BackgroundBitmap.Load(m_ProjectDirectory + "background_image.bmp");

	m_BackgroundBitmapLoaded = !(m_BackgroundBitmap.IsNull ());
	//	m_BackgroundBitmapLoaded = true;

	m_ImageXResolution = 1;
	m_ImageYResolution = 1;


	if(m_BackgroundBitmapLoaded)
	{

		if(bAskForInput)
		{

			m_ImageX1 = 0;
			m_ImageY1 = 0;
			m_ImageX2 = 20;
			m_ImageY2 = 10;

			m_OriginOnBottomFlag = 1;
		}else
		{
			m_OriginOnBottomFlag = (int)(g_GetPrivateProfileDouble("coordinate_info", "origin_on_bottom_flag", m_OriginOnBottomFlag, lpszFileName));
			m_ImageX1 = g_GetPrivateProfileDouble("background_image_coordinate_info", "left", m_NetworkRect.left, lpszFileName);
			m_ImageY1 = g_GetPrivateProfileDouble("background_image_coordinate_info", "bottom", m_NetworkRect.bottom, lpszFileName);
			m_ImageX2 = g_GetPrivateProfileDouble("background_image_coordinate_info", "right", m_NetworkRect.right, lpszFileName);
			m_ImageY2 = g_GetPrivateProfileDouble("background_image_coordinate_info", "top", m_NetworkRect.top, lpszFileName);
			m_ImageWidthInMile = g_GetPrivateProfileDouble("background_image_coordinate_info", "width",1, lpszFileName);


		}

		m_ImageWidth = fabs(m_ImageX2 - m_ImageX1);
		m_ImageHeight = fabs(m_ImageY2 - m_ImageY1);

		m_ImageMoveSize = m_ImageWidth/2000.0f;

		m_BackgroundImageFileLoadingStatus.Format ("Optional background image file is loaded.");

	}
	else
		m_BackgroundImageFileLoadingStatus.Format ("Optional background image file is not loaded.");

	return m_BackgroundBitmapLoaded;
}
// CTLiteDoc commands

void CTLiteDoc::OnFileOpen()
{

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_LONGNAMES|OFN_ENABLESIZING,
		_T("GMNS Data Files (node.csv)|node.csv|"),NULL,0,true);
	if(dlg.DoModal() == IDOK)
	{
		OnOpenDocument(dlg.GetPathName());
	}
}

void CTLiteDoc::OnFileSaveimagelocation()
{

	TCHAR IniFilePath[_MAX_PATH];
	sprintf_s(IniFilePath,"%s", m_ProjectFile);

	char lpbuffer[64];

	sprintf_s(lpbuffer,"%f",m_ImageX1);
	WritePrivateProfileString("background_image_coordinate_info","left",lpbuffer,IniFilePath);
	sprintf_s(lpbuffer,"%f",m_ImageY1);
	WritePrivateProfileString("background_image_coordinate_info","bottom",lpbuffer,IniFilePath);

	sprintf_s(lpbuffer,"%f",m_ImageX2);
	WritePrivateProfileString("background_image_coordinate_info","right",lpbuffer,IniFilePath);
	sprintf_s(lpbuffer,"%f",m_ImageY2);
	WritePrivateProfileString("background_image_coordinate_info","top",lpbuffer,IniFilePath);

	sprintf_s(lpbuffer,"%f",m_ImageWidthInMile);
	WritePrivateProfileString("background_image_coordinate_info","width",lpbuffer,IniFilePath);


}

COLORREF CTLiteDoc::GetLinkTypeColor(int LinkType)
{
	COLORREF color;
	switch (LinkType)
	{
	case 1: color = m_ColorFreeway; break;
	case 2: color = m_ColorHighway; break;
	default: color = m_ColorArterial;

	}
	return color;
}

void CTLiteDoc::OnShowShowpathmoe()
{

}

void CTLiteDoc::OnUpdateShowShowpathmoe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_LinkMOEDlgShowFlag);
}





bool CTLiteDoc::ReadNodeCSVFile(LPCTSTR lpszFileName, int LayerNo)
{


	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{
		int i= m_NodeSet.size();
		while(parser.ReadRecord())
		{
			int node_id;
			string name;
			int zone_id = 0;
			DTANode* pNode = 0;
			string control_type;
			double X;
			double Y;

			node_id =0;

			int level_no = 0;
//			parser.GetValueByFieldName("level_no", level_no);

			parser.GetValueByFieldName("node_id", node_id);
			
			parser.GetValueByFieldName("name", name);

			parser.GetValueByFieldName("zone_id", zone_id);

			string node_type;
			parser.GetValueByFieldName("node_type", node_type);
			parser.GetValueByFieldName("ctrl_type", control_type);
			bool bFieldX_Exist = parser.GetValueByFieldName("x_coord",X,false);
			parser.GetValueByFieldName("y_coord",Y,false);

			string geo_string;

			if(parser.GetValueByFieldName("geometry",geo_string))
			{
				// overwrite when the field "geometry" exists
				CGeometry geometry(geo_string);

				std::vector<CCoordinate> CoordinateVector = geometry.GetCoordinateList();

				if(CoordinateVector.size()>0)
				{
					X = CoordinateVector[0].X;
					Y = CoordinateVector[0].Y;
				}

			}

			if(m_NodeNumbertoNodeNoMap.size()!= 0 && m_NodeNumbertoNodeNoMap.find(node_id) != m_NodeNumbertoNodeNoMap.end())
			{
				CString error_message;
				error_message.Format ("Node %d in node.csv has been defined twice. Please check.", node_id);
				AfxMessageBox(error_message);
				return 0;
			}

			pNode = new DTANode;
			pNode->m_NodeID = node_id;

			pNode->m_ctrl_type = control_type;
			pNode->m_node_type = node_type;
			//pNode->m_SignalOffsetInSecond = offset_in_second;
			pNode->m_LayerNo = level_no;


			pNode->pt.x = X;
			pNode->pt.y = Y;

			pNode->m_NodeNo = i;
			pNode->m_ZoneID = zone_id;

			//m_NodeNotoZoneNameMap[m_NodeNumbertoNodeNoMap[node_id]] = zone_number;
			//m_NodeNoMap[m_NodeNumbertoNodeNoMap[node_id]]->m_ZoneID = zone_number;


			//if (m_ODSize < zone_number)
			//	m_ODSize = zone_number;

			m_NodeSet.push_back(pNode);
			m_NodeNoMap[i] = pNode;
			m_NodeNumberMap[node_id] = pNode;
			m_NodeNotoNumberMap[i] = node_id;
			m_NodeNumbertoNodeNoMap[node_id] = i;

			m_ZoneIDToNodeNoMap[zone_id] = i;
			i++;

			//			cout << "node = " << node << ", X= " << X << ", Y = " << Y << endl;

		}

		m_ZoneNoSize = m_ZoneIDToNodeNoMap.size();

		std::map<int, int>	::const_iterator itr_o;

		int z = 0;
		for (itr_o = m_ZoneIDToNodeNoMap.begin(); itr_o != m_ZoneIDToNodeNoMap.end(); itr_o++)
		{
			m_ZoneNotoZoneIDMap[z] = itr_o->first;
			m_ZoneIDtoZoneNoMap[itr_o->first] = z;
			z++;
		}
		

		

		m_NodeDataLoadingStatus.Format ("%d nodes are loaded from file %s.",m_NodeSet.size(),lpszFileName);
		return true;
	}else
	{
		if (LayerNo == 0)
		{
			AfxMessageBox("Error: File node.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		}return false;
		//		g_ProgramStop();
	}

}
void CTLiteDoc::BuildGridSystem()
{

	bool bRectInitialized = false;
	m_AdjLinkSize = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		if ((*iNode)->m_LayerNo == 0)  //base line network only
		{
			if (!bRectInitialized)
			{
				m_GridRect.left = (*iNode)->pt.x;
				m_GridRect.right = (*iNode)->pt.x;
				m_GridRect.top = (*iNode)->pt.y;
				m_GridRect.bottom = (*iNode)->pt.y;
				bRectInitialized = true;
			}

			m_GridRect.Expand((*iNode)->pt);
		}
	}

	m_GridXStep = max(0.0001,m_GridRect.Width () /_MAX_TRANSIT_GRID_SIZE);

	m_GridYStep = max(0.0001,m_GridRect.Height ()/_MAX_TRANSIT_GRID_SIZE);

	int time_interval_no = 0;
	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		if ((*iNode)->m_LayerNo == 0)  //base line network only
		{
			int x_key = ((*iNode)->pt.x - m_GridRect.left) / m_GridXStep;
			int y_key = ((*iNode)->pt.y - m_GridRect.bottom) / m_GridYStep;

			//feasible region
			x_key = max(0, x_key);
			x_key = min(_MAX_TRANSIT_GRID_SIZE - 1, x_key);

			y_key = max(0, y_key);
			y_key = min(_MAX_TRANSIT_GRID_SIZE - 1, y_key);

			m_GridMatrix[x_key][y_key][0].m_NodeVector.push_back((*iNode)->m_NodeNo);
			m_GridMatrix[x_key][y_key][0].m_NodeX.push_back((*iNode)->pt.x);
			m_GridMatrix[x_key][y_key][0].m_NodeY.push_back((*iNode)->pt.y);

			(*iNode)->m_AreaCode = x_key*(100) + y_key;
			(*iNode)->m_GateFlag = 0;
		}
	}

	int m_GateFlagCount[10] = { 0 };

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if ((*iLink)->m_LayerNo == 0)  //base line network only
		{
			int x_key = ((*iLink)->m_FromPoint.x  - m_GridRect.left) / m_GridXStep;
			int y_key = ((*iLink)->m_FromPoint.y - m_GridRect.bottom) / m_GridYStep;

			//feasible region
			x_key = max(0, x_key);
			x_key = min(_MAX_TRANSIT_GRID_SIZE - 1, x_key);

			y_key = max(0, y_key);
			y_key = min(_MAX_TRANSIT_GRID_SIZE - 1, y_key);

			m_GridMatrix[x_key][y_key][0].m_LinkNoVector.push_back((*iLink)->m_LinkNo);

			(*iLink)->From_xGridNo = x_key;
			(*iLink)->From_yGridNo = y_key;
			(*iLink)->From_xyGridNo = x_key*(100)+ y_key;


			x_key = ((*iLink)->m_ToPoint.x - m_GridRect.left) / m_GridXStep;
			y_key = ((*iLink)->m_ToPoint.y - m_GridRect.bottom) / m_GridYStep;

			//feasible region
			x_key = max(0, x_key);
			x_key = min(_MAX_TRANSIT_GRID_SIZE - 1, x_key);

			y_key = max(0, y_key);
			y_key = min(_MAX_TRANSIT_GRID_SIZE - 1, y_key);

			(*iLink)->To_xGridNo = x_key;
			(*iLink)->To_yGridNo = y_key;
			(*iLink)->To_xyGridNo = x_key*(100) + y_key;

	
			if ((*iLink)->From_xyGridNo == (*iLink)->To_xyGridNo)
			{
				(*iLink)->m_GateFlag = 0;
				m_GateFlagCount[0] +=1;
			}
			else
			{
				if (m_LinkTypeMap[(*iLink)->m_link_type].IsFreeway() ||
					m_LinkTypeMap[(*iLink)->m_link_type].IsHighway())  // freeway or highway
				{
					(*iLink)->m_GateFlag = 2;
					m_NodeNoMap[(*iLink)->m_FromNodeID]->m_GateFlag = 2;
					m_NodeNoMap[(*iLink)->m_ToNodeID]->m_GateFlag = 2;

					m_GateFlagCount[2] += 1;

				}
				else
				{
					(*iLink)->m_GateFlag = 1;
					m_NodeNoMap[(*iLink)->m_FromNodeID]->m_GateFlag = 1;
					m_NodeNoMap[(*iLink)->m_ToNodeID]->m_GateFlag = 1;
					m_GateFlagCount[1] += 1;
				}
	
			}
		
		}
	}
	m_GateFlagCount[9] = 0;
}


int CTLiteDoc::FindClosestNode(double x, double y, double min_distance,  int step_size, double time_stamp_in_min)
{

	step_size = int(min_distance/m_GridXStep+1);

	int x_key = (x - m_GridRect.left)/ m_GridXStep;
	int y_key = (y -m_GridRect.bottom)/ m_GridYStep;

	//feasible region
	x_key = max(0,x_key);
	x_key = min(99,x_key);

	y_key = max(0,y_key);
	y_key = min(99,y_key);

	int NodeId = -1;


	for(int x_i = max(0,x_key- step_size); x_i <= min(99,x_key+ step_size); x_i++)
		for(int y_i = max(0,y_key-step_size); y_i <= min(99,y_key+step_size ); y_i++)
		{

			GridNodeSet element = m_GridMatrix[x_i][y_i][0];

			for(unsigned int i = 0; i < element.m_NodeVector.size(); i++)
			{

				double distance = sqrt( (x-element.m_NodeX[i])*(x-element.m_NodeX[i]) + (y-element.m_NodeY[i])*(y-element.m_NodeY[i]))/m_UnitDistance;

				if(distance < min_distance)
				{

					min_distance = distance;

					NodeId =  element.m_NodeVector[i];

				}



			}	// per node in a grid cell

		} // for nearby cell

		return NodeId;
}

int CTLiteDoc::FindClosestZone(double x, double y, double min_distance,  int step_size)
{

	step_size = int(min_distance/m_GridXStep+1);

	int x_key = (x - m_GridRect.left)/ m_GridXStep;
	int y_key = (y -m_GridRect.bottom)/ m_GridYStep;

	//feasible region
	x_key = max(0,x_key);
	x_key = min(99,x_key);

	y_key = max(0,y_key);
	y_key = min(99,y_key);

	int NodeId = -1;
	int ZoneNumber = -1;


	for(int x_i = max(0,x_key- step_size); x_i <= min(99,x_key+ step_size); x_i++)
		for(int y_i = max(0,y_key-step_size); y_i <= min(99,y_key+step_size ); y_i++)
		{

			GridNodeSet element = m_GridMatrix[x_i][y_i][0];

			for(unsigned int i = 0; i < element.m_NodeVector.size(); i++)
			{
				if( m_NodeNoMap[element.m_NodeVector[i]]->m_ZoneID >0)

				{
					double distance = sqrt( (x-element.m_NodeX[i])*(x-element.m_NodeX[i]) + (y-element.m_NodeY[i])*(y-element.m_NodeY[i]));

					if(distance < min_distance)
					{

						min_distance = distance;

						NodeId =  element.m_NodeVector[i];
						ZoneNumber  = m_NodeNoMap[NodeId]->m_ZoneID;
					}
				}



			}	// per node in a grid cell

		} // for nearby cell

		return ZoneNumber;
}


float CTLiteDoc::GetLinkBandWidth(float Value)
{ //output: m_BandWidthValue for each link
	if(m_MaxLinkWidthAsLinkVolume < 10)
		m_MaxLinkWidthAsLinkVolume = 10;

	float VolumeRatio = 1/m_MaxLinkWidthAsLinkVolume;  // 1000 Agents flow rate as 1 lanes

	float LaneVolumeEquivalent = 200;
	float DurationInMinEquivalent = 20;

	float link_volume = 0;
	// default mode
	float BandWidthValue = 0;

	BandWidthValue =  max(0,Value*LaneVolumeEquivalent*VolumeRatio);

	if(m_LinkBandWidthMode == LBW_number_of_lanes)
	{
		BandWidthValue =  min(7,max(1,Value*LaneVolumeEquivalent*VolumeRatio));


	}else if(m_LinkBandWidthMode == LBW_link_volume)
	{
		BandWidthValue = Value*VolumeRatio; 
	}else if (m_LinkBandWidthMode == LBW_number_of_marked_Agents)
	{
		//			pLink->m_BandWidthValue =  pLink->m_NumberOfMarkedAgents *VolumeRatio;
	}else if (m_LinkBandWidthMode == LBW_congestion_duration)
	{
		BandWidthValue =  Value *DurationInMinEquivalent*VolumeRatio;
	}
	else
		// default value
	{
		BandWidthValue =  5*LaneVolumeEquivalent*VolumeRatio;
	}

	return BandWidthValue;

}

void CTLiteDoc::ReCalculateLinkBandWidth()
{ //output: m_BandWidthValue for each link
	std::list<DTALink*>::iterator iLink;

	if(m_MaxLinkWidthAsLinkVolume < 10)
		m_MaxLinkWidthAsLinkVolume = 10;

	float VolumeRatio = 1/m_MaxLinkWidthAsLinkVolume;  // 1000 Agents flow rate as 1 lanes

	float max_total_volume = 1;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink)->m_total_link_volume > max_total_volume)
		{
			max_total_volume = (*iLink)->m_total_link_volume;
		}
	}

	float TotalVolumeRatio = VolumeRatio;

	float LaneVolumeEquivalent = 200;
	float DurationInMinEquivalent = 20;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink* pLink = (*iLink);
		if (pLink->m_FromNodeNumber == 31 && pLink->m_ToNodeNumber == 30)
			TRACE("");

		float link_volume = 0;
		// default mode
		pLink->m_BandWidthValue =  max(0,pLink->m_NumberOfLanes*LaneVolumeEquivalent*VolumeRatio);

		if(m_LinkBandWidthMode == LBW_number_of_lanes)
		{
			if(m_LinkTypeMap[pLink->m_link_type ].IsConnector ())  // 1 lane as connector
				pLink->m_BandWidthValue =  min(1,pLink->m_NumberOfLanes)*LaneVolumeEquivalent*VolumeRatio;
			else
				pLink->m_BandWidthValue =  min(7, max(1,pLink->m_NumberOfLanes)*LaneVolumeEquivalent*VolumeRatio);


		}else if(m_LinkBandWidthMode == LBW_link_volume)
		{
			if(m_LinkMOEMode == MOE_safety)  // safety
			{
			}else
			{
				if(g_Simulation_Time_Stamp>=1) // dynamic traffic assignment mode
				{

					GetLinkMOE((*iLink), MOE_volume,g_Simulation_Time_Stamp, g_MOEAggregationIntervalInMin, link_volume);

					pLink->m_BandWidthValue = link_volume*VolumeRatio; 
				}else  // total volume
				{

					GetLinkMOE((*iLink), MOE_volume,m_DemandLoadingStartTimeInMin, m_DemandLoadingEndTimeInMin-m_DemandLoadingStartTimeInMin, link_volume);

					pLink->m_BandWidthValue = link_volume*VolumeRatio;
				}
			}

			
		}else if (m_LinkBandWidthMode == LBW_number_of_marked_Agents)
		{
			pLink->m_BandWidthValue =  pLink->m_NumberOfMarkedAgents *VolumeRatio;
		}else if (m_LinkBandWidthMode == LBW_congestion_duration)
		{
			pLink->m_BandWidthValue =  pLink->GetImpactDuration (g_Simulation_Time_Stamp, this->m_PrimaryDataSource ) *DurationInMinEquivalent*VolumeRatio;
		}
		else
			// default value
		{
			pLink->m_BandWidthValue =  pLink->m_NumberOfLanes*LaneVolumeEquivalent*VolumeRatio;
		}

	}

}
void CTLiteDoc::GenerateOffsetLinkBand()
{


	ReCalculateLinkBandWidth();

	std::list<DTALink*>::iterator iLink;

	m_UnitDistance = max(0.00001,m_UnitDistance);

	m_LaneWidthInMeter = 12;
	double lane_offset = m_UnitDistance*m_LaneWidthInMeter;  // 20 feet per lane

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		(*iLink)->m_BandLeftShapePoints.clear();
		(*iLink)->m_BandRightShapePoints.clear();

		(*iLink)->m_ReferenceBandLeftShapePoints.clear();
		(*iLink)->m_ReferenceBandRightShapePoints.clear();

		if ((*iLink)->m_FromNodeNumber == 31 && (*iLink)->m_ToNodeNumber == 30)
			TRACE("");

		if((*iLink)->m_ShapePoints.size() ==0)
			continue;

		int last_shape_point_id = (*iLink) ->m_ShapePoints .size() -1;
		double DeltaX = (*iLink)->m_ShapePoints[last_shape_point_id].x - (*iLink)->m_ShapePoints[0].x;
		double DeltaY = (*iLink)->m_ShapePoints[last_shape_point_id].y - (*iLink)->m_ShapePoints[0].y;
		double theta = 0;

		if(fabs(DeltaY)>0.0000001)
			theta = atan2(DeltaY, DeltaX);


		for(unsigned int si = 0; si < (*iLink) ->m_ShapePoints .size(); si++)
		{

			// calculate theta for each feature point segment
			// longer than 0.5 miles
			if(si>= 1 && (*iLink) ->m_Length > 0.5 &&((*iLink) ->m_ShapePoints .size() >4 || m_LinkTypeMap[(*iLink)->m_link_type].IsRamp ()))  // ramp or >4 feature points
			{
				last_shape_point_id = si;
				DeltaX = (*iLink)->m_ShapePoints[last_shape_point_id].x - (*iLink)->m_ShapePoints[si-1].x;
				DeltaY = (*iLink)->m_ShapePoints[last_shape_point_id].y - (*iLink)->m_ShapePoints[si-1].y;

				if(fabs(DeltaY)>0.00001)
					theta = atan2(DeltaY, DeltaX);
				else {
					if (DeltaX > 0)
						theta = 0;
					else 
						theta = PI;
					}
			}

			GDPoint pt;

			pt.x = (*iLink)->m_ShapePoints[si].x ;
			pt.y = (*iLink)->m_ShapePoints[si].y ;

			(*iLink)->m_BandLeftShapePoints.push_back (pt);

			double x_offset = (*iLink)->m_BandWidthValue*lane_offset* cos(theta - PI / 2.0f);
			double y_offset = (*iLink)->m_BandWidthValue*lane_offset* sin(theta - PI / 2.0f);
			pt.x = (*iLink)->m_ShapePoints[si].x + x_offset;
			pt.y = (*iLink)->m_ShapePoints[si].y + y_offset;

			(*iLink)->m_BandRightShapePoints.push_back (pt);

			if((*iLink)->m_bSensorData) // refernece band
			{

				pt.x = (*iLink)->m_ShapePoints[si].x ;
				pt.y = (*iLink)->m_ShapePoints[si].y ;

				(*iLink)->m_ReferenceBandLeftShapePoints.push_back (pt);

				pt.x  = (*iLink)->m_ShapePoints[si].x + (*iLink)->m_ReferenceBandWidthValue*lane_offset* cos(theta-PI/2.0f);
				pt.y = (*iLink)->m_ShapePoints[si].y + (*iLink)->m_ReferenceBandWidthValue*lane_offset* sin(theta-PI/2.0f);
				(*iLink)->m_ReferenceBandRightShapePoints.push_back (pt);
			}

		}

	}

}

void CTLiteDoc::OffsetLink()
{
	CWaitCursor wait;
	std::list<DTALink*>::iterator iLink;

	double length_sum_in_mile = 0;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		length_sum_in_mile += (*iLink) ->m_Length;

		if( (*iLink) ->m_Original_ShapePoints .size() ==0)// no original shape points
		{

			(*iLink) ->m_Original_ShapePoints = (*iLink) ->m_ShapePoints;
		}

		// comment it out for future examination
		//if( (*iLink) ->m_Original_ShapePoints .size() ==2)// two shape points only, overwite them using upsteram node and downstream node number 
		//{

		//		if((*iLink)->m_FromNodeNumber == 53718 && (*iLink)->m_ToNodeNumber == 53810)
		//		{
		//		TRACE("");
		//		}

		//	(*iLink) ->m_Original_ShapePoints.clear ();
		//	(*iLink) ->m_Original_ShapePoints.push_back (m_NodeNoMap[(*iLink) ->m_FromNodeID]->pt);
		//	(*iLink) ->m_Original_ShapePoints.push_back (m_NodeNoMap[(*iLink) ->m_ToNodeID]->pt );

		//}
	}

		double AvgLinkLengthInMeter = length_sum_in_mile/max(1,m_LinkSet.size());

		m_NodeDisplaySize = max(100, AvgLinkLengthInMeter*0.05);  // in feet


	if(m_bLinkToBeShifted)
	{
		std::list<DTALink*>::iterator iLink;

		double minimum_link_length = 999999;
		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if((*iLink) -> m_bToBeShifted)
			{
				if((*iLink)->m_Length <  minimum_link_length)
					minimum_link_length = (*iLink)->m_Length ;
			}
		}
		double link_offset = max(minimum_link_length*0.025*m_UnitDistance, m_UnitDistance*m_OffsetInDistance);  // 80 feet

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_FromNodeNumber == 31 && (*iLink)->m_ToNodeNumber == 30)
				TRACE("");


			if((*iLink) -> m_bToBeShifted)
			{
				//Test if an opposite link exits
				unsigned long OppositeLinkKey = GetLinkKey((*iLink)->m_ToNodeID,(*iLink)->m_FromNodeID);
				if ( m_NodeNotoLinkMap.find(OppositeLinkKey) != m_NodeNotoLinkMap.end())
				{
					// set two links as two-way links
					(*iLink)->m_bOneWayLink = false;

					DTALink* pLink = m_NodeNotoLinkMap[OppositeLinkKey];
					if(pLink!=NULL)
						pLink->m_bOneWayLink = false;

				}
			}
		}



		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_FromNodeNumber == 31 && (*iLink)->m_ToNodeNumber == 30)
			{

				TRACE("");
			}

			if( (*iLink)->m_bOneWayLink == false)// apply link split to two way links
			{


				int last_shape_point_id = (*iLink) ->m_Original_ShapePoints .size() -1;

				ASSERT(last_shape_point_id>=1);

				double DeltaX = (*iLink)->m_Original_ShapePoints[last_shape_point_id].x - (*iLink)->m_Original_ShapePoints[0].x;
				double DeltaY = (*iLink)->m_Original_ShapePoints[last_shape_point_id].y - (*iLink)->m_Original_ShapePoints[0].y;
				double theta = 0;			
				if(fabs(DeltaY)>0.00001)
					theta= atan2(DeltaY, DeltaX);
				else {
					if (DeltaX > 0)
						theta = 0;
					else
						theta = PI;
					}

				for(unsigned int si = 0; si < (*iLink) ->m_Original_ShapePoints .size(); si++)
				{
					// calculate theta for each feature point segment
					if(si>= 1 && ( (*iLink) ->m_Original_ShapePoints .size() >4 || m_LinkTypeMap[(*iLink)->m_link_type].IsRamp ()))  // ramp or >4 feature points
					{
						last_shape_point_id = si;
						DeltaX = (*iLink)->m_Original_ShapePoints[last_shape_point_id].x - (*iLink)->m_Original_ShapePoints[si-1].x;
						DeltaY = (*iLink)->m_Original_ShapePoints[last_shape_point_id].y - (*iLink)->m_Original_ShapePoints[si-1].y;


						//if(fabs(DeltaY)>0.001)
						//	theta= atan2(DeltaY, DeltaX);
					}
					double cos_offset =  cos(theta-PI/2.0);
					double sin_offset = sin(theta-PI/2.0);
					(*iLink)->m_ShapePoints[si].x = (*iLink)->m_Original_ShapePoints[si].x + m_bRightHandTrafficFlag*link_offset* cos_offset;
					(*iLink)->m_ShapePoints[si].y = (*iLink)->m_Original_ShapePoints[si].y + m_bRightHandTrafficFlag*link_offset* sin_offset;
				}
			}


		}
	}

	m_bBezierCurveFlag = false;

	if(m_bBezierCurveFlag)  //do not apply m_bBezierCurveFlag 
	{
		BezierCurve bezier_smoothing;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			// bezier smoothing
			std::vector<GDPoint> NewShapePoints;

			if((*iLink) ->m_ShapePoints .size() >2)
			{
				// general smooth curve
				bezier_smoothing.Bezier2D((*iLink) ->m_ShapePoints, NewShapePoints);

				// re-write to data
				(*iLink) ->m_ShapePoints = NewShapePoints;

			}
		}

	}
}

bool CTLiteDoc::ReadLinkCSVFile(LPCTSTR lpszFileName, bool bCreateNewNodeFlag = false,  int LayerNo = 0)
{

	long i = m_LinkSet.size();
	DTALink* pLink = 0;
	float default_coordinate_distance_sum=0;
	float length_sum_in_mile = 0;

	CString error_message;
	CString warning_message = "";
	int warning_message_no = 0;

	bool bTwoWayLinkFlag = false;

	bool length_field_warning = false;
	bool number_of_lanes_field_warning = false;
	bool speed_limit_field_warning = false;
	bool capacity_field_warning = false;
	bool type_field_warning = false;


	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{
		bool bNodeNonExistError = false;
		while (parser.ReadRecord())
		{
			string road_link_id;
			long from_node_id = 0;
			long to_node_id = 0;
			int direction = 0;
			double length_in_mile;
			int number_of_lanes = 1;
			int free_speed = 60;
			double capacity_in_pcphpl = 2000;
			string link_type_str;
			string name;
			float k_jam = 180;
			string mode_code = "";
			string demand_type_code = "";

			float grade = 0;

			if (!parser.GetValueByFieldName("road_link_id", road_link_id))  // no value
			{
				// mark it as 0 first, and we then find a new unique link id after reading all links
			}

			if (!parser.GetValueByFieldName("name", name))
				name = "";

			if (!parser.GetValueByFieldName("from_node_id", from_node_id))
			{
				AfxMessageBox("Field from_node_id has not been defined in fileroad_ink.csv. Please check.");
				break;
			}
			if (!parser.GetValueByFieldName("to_node_id", to_node_id))
			{
				AfxMessageBox("Field to_node_id has not been defined in fileroad_ink.csv. Please check.");
				break;
			}


			if (m_NodeNumbertoNodeNoMap.find(from_node_id) == m_NodeNumbertoNodeNoMap.end())
			{
				if (bCreateNewNodeFlag == false)  // not create new node
				{
					CString warning;
					warning.Format("from_node_id %d in road_ink.csv has not been defined in node.csv.\n", from_node_id);
					bNodeNonExistError = true;

					if (warning_message.GetLength() < 3000)  // not adding and showing too many links
					{
						warning_message += warning;
					}

					continue;
				}
				else  // need to create a new node with geometry information
				{
					string geo_string;
					std::vector<CCoordinate> CoordinateVector;
					if (parser.GetValueByFieldName("geometry", geo_string))
					{
						// overwrite when the field "geometry" exists
						CGeometry geometry(geo_string);
						CoordinateVector = geometry.GetCoordinateList();
						if (CoordinateVector.size() >= 2)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[0].X;
							pt.y = CoordinateVector[0].Y;
							AddNewNode(pt, from_node_id, LayerNo);

						}
						else
						{

							error_message.Format("Field geometry in Link %s has less than 2 feature points, coordinate information of upstream node cannot be extracted. Please check.", name.c_str());
							AfxMessageBox(error_message);
							return false;
						}

					}
					else
					{
						AfxMessageBox("Field geometry cannot be found inroad_ink.csv. Please check.");
						return false;
					}

				}
			}

			if (m_NodeNumbertoNodeNoMap.find(to_node_id) == m_NodeNumbertoNodeNoMap.end())
			{
				if (bCreateNewNodeFlag == false)  // not create new node
				{
					CString warning;
					warning.Format("to_node_id %d in road_ink.csv has not been defined in node.csv\n", to_node_id);
					bNodeNonExistError = true;

					if (warning_message.GetLength() < 3000)  // not adding and showing too many links
					{
						warning_message += warning;
					}
					continue;
				}
				else  // create new node
				{
					string geo_string;
					std::vector<CCoordinate> CoordinateVector;
					if (parser.GetValueByFieldName("geometry", geo_string))
					{
						// overwrite when the field "geometry" exists
						CGeometry geometry(geo_string);
						CoordinateVector = geometry.GetCoordinateList();
						if (CoordinateVector.size() >= 2)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[CoordinateVector.size() - 1].X;
							pt.y = CoordinateVector[CoordinateVector.size() - 1].Y;
							AddNewNode(pt, to_node_id, LayerNo);

						}
						else
						{
							error_message.Format("Field geometry in Link %s has less than 2 feature points, coordinate information of upstream node cannot be extracted. Please check.", name.c_str());
							AfxMessageBox(error_message);
							return false;
						}

					}
					else
					{
						AfxMessageBox("Field geometry cannot be found inroad_ink.csv. Please check.");
						return false;
					}


				}

			}


			DTALink* pExistingLink = FindLinkWithNodeNumbers(from_node_id, to_node_id);
			bool bDonotAllowDuplicatedLinks = false;
			if (bDonotAllowDuplicatedLinks && pExistingLink)
			{

				CString warning;
				warning.Format("Link %d-> %d is duplicated.\n", from_node_id, to_node_id);
				if (warning_message.GetLength() < 3000)  // not adding and showing too many links
				{
					warning_message += warning;
				}
				continue;
			}


			if (!parser.GetValueByFieldName("length", length_in_mile))
			{
				if (!length_field_warning)
				{
					AfxMessageBox("Field length has not been defined in file road_ink.csv. Please check.");
					length_field_warning = true;
				}

				length_in_mile = 1;
			}

			if (!parser.GetValueByFieldName("direction", direction))
				direction = 1;



			if (!parser.GetValueByFieldName("lanes", number_of_lanes))
			{
				if (!number_of_lanes_field_warning)
				{
					AfxMessageBox("Field lanes has not been defined in file road_ink.csv. Please check.");
					number_of_lanes_field_warning = true;
				}
				number_of_lanes = 1;

			}


			if (!parser.GetValueByFieldName("free_speed", free_speed))
			{
				if (!speed_limit_field_warning)
				{
					error_message.Format("Link %s: Field free_speed has not been defined in fileroad_ink.csv. Please check.", name.c_str());
					AfxMessageBox(error_message);
					speed_limit_field_warning = true;
				}
				free_speed = 60;
			}

			if (!parser.GetValueByFieldName("capacity", capacity_in_pcphpl))
			{
				if (!capacity_field_warning)
				{
					error_message.Format("Link %s: Field capacity has not been defined in fileroad_ink.csv. Please check.", name.c_str());
					AfxMessageBox(error_message);
					capacity_field_warning = true;
				}

			}

			parser.GetValueByFieldName("facility_type", link_type_str);
			int link_type = 1;
			parser.GetValueByFieldName("link_type", link_type);


			parser.GetValueByFieldName("demand_type_code", demand_type_code);


			float BPR_alpha_term = 0.15;
			float BPR_beta_term = 4;

			parser.GetValueByFieldName("BPR_alpha_term", BPR_alpha_term);
			parser.GetValueByFieldName("BPR_beta_term", BPR_beta_term);

			string link_key;
			parser.GetValueByFieldName("link_key", link_key);

			string geo_string;

			std::vector<CCoordinate> Original_CoordinateVector;
			if (parser.GetValueByFieldName("original_geometry", geo_string))
			{
				// overwrite when the field "geometry" exists
				CGeometry geometry(geo_string);
				Original_CoordinateVector = geometry.GetCoordinateList();
			}

			bool bToBeShifted = true;
			bool bWithCoordinateVector = false;
			std::vector<CCoordinate> CoordinateVector;
			if (parser.GetValueByFieldName("geometry", geo_string))
			{
				// overwrite when the field "geometry" exists
				CGeometry geometry(geo_string);
				CoordinateVector = geometry.GetCoordinateList();
				if (CoordinateVector.size() >= 2)
				{
					m_bLinkToBeShifted = false;
					bToBeShifted = false;
					bWithCoordinateVector = true;
				}
			}
			if (bWithCoordinateVector == false)
			{
				// no geometry information
				CCoordinate cc_from, cc_to;
				cc_from.X = m_NodeNoMap[m_NodeNumbertoNodeNoMap[from_node_id]]->pt.x;
				cc_from.Y = m_NodeNoMap[m_NodeNumbertoNodeNoMap[from_node_id]]->pt.y;

				cc_to.X = m_NodeNoMap[m_NodeNumbertoNodeNoMap[to_node_id]]->pt.x;
				cc_to.Y = m_NodeNoMap[m_NodeNumbertoNodeNoMap[to_node_id]]->pt.y;

				CoordinateVector.push_back(cc_from);
				CoordinateVector.push_back(cc_to);
			}


			int link_code_start = 1;
			int link_code_end = 1;

			if (direction == -1) // reversed
			{
				link_code_start = 2; link_code_end = 2;
			}

			if (direction == 0) // two-directional link
			{
				link_code_start = 1; link_code_end = 2;
				bTwoWayLinkFlag = true;
			}

			string SpeedSensorID;
			parser.GetValueByFieldName("sensor_id", SpeedSensorID);

			int layer_no = 0;

			for (int link_code = link_code_start; link_code <= link_code_end; link_code++)
			{

				bool bNodeNonExistError = false;
				int m_SimulationHorizon = 1440;
				pLink = new DTALink(m_SimulationHorizon);

				pLink->m_LayerNo = layer_no;
				pLink->m_LinkNo = i;
				pLink->m_Name = name;
				pLink->m_OrgDir = direction;
				pLink->m_LinkID = road_link_id;
				pLink->m_LinkKey = link_key.c_str();

				m_LinkKeyMap[pLink->m_LinkKey] = pLink;

				pLink->m_Mode_code = mode_code;
				pLink->m_demand_type_code = demand_type_code;

				pLink->m_geo_string = geo_string;

				if (link_code == 1)  //AB link
				{
					pLink->m_FromNodeNumber = from_node_id;

					pLink->m_ToNodeNumber = to_node_id;
					pLink->m_Direction = 1;

					pLink->m_FromNodeID = m_NodeNumbertoNodeNoMap[from_node_id];
					pLink->m_ToNodeID = m_NodeNumbertoNodeNoMap[to_node_id];

					int si;

					if (LayerNo <= 1)  // not the mapping layer 
					{

						for (si = 0; si < CoordinateVector.size(); si++)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[si].X;
							pt.y = CoordinateVector[si].Y;
							pLink->m_ShapePoints.push_back(pt);

						}
					}

				}

				if (link_code == 2)  //BA link
				{
					pLink->m_FromNodeNumber = to_node_id;
					pLink->m_ToNodeNumber = from_node_id;
					pLink->m_Direction = 1;

					pLink->m_FromNodeID = m_NodeNumbertoNodeNoMap[to_node_id];
					pLink->m_ToNodeID = m_NodeNumbertoNodeNoMap[from_node_id];

					for (int si = CoordinateVector.size() - 1; si >= 0; si--)
					{
						GDPoint	pt;
						pt.x = CoordinateVector[si].X;
						pt.y = CoordinateVector[si].Y;
						pLink->m_ShapePoints.push_back(pt);
					}

					pLink->m_bToBeShifted = bToBeShifted;
				}
				// original geometry

				if (pLink->m_ShapePoints.size() == 0)
				{
					TRACE("");
				}


				if (Original_CoordinateVector.size() > 0) // data available
				{
					for (unsigned int si = 0; si < Original_CoordinateVector.size(); si++)
					{
						GDPoint	pt;
						pt.x = Original_CoordinateVector[si].X;
						pt.y = Original_CoordinateVector[si].Y;
						pLink->m_Original_ShapePoints.push_back(pt);

					}
				}
				else
				{  //   data not available, we might use the previously offet shape points to construct the "base-line" shape points
					pLink->m_Original_ShapePoints = pLink->m_ShapePoints;

					m_OffsetInDistance = 0;

				}
				pLink->m_NumberOfLanes = number_of_lanes;

				pLink->m_FreeSpeed = max(10, free_speed);  // minimum Free Speed is 1 mph

				pLink->m_avg_simulated_speed = pLink->m_FreeSpeed;

				//	pLink->m_Length= max(length_in_mile, pLink->m_FreeSpeed*0.1f/60.0f);  // minimum distance, special note: we do not consider the minimum constraint here, but a Agent cannot travel longer then 0.1 seconds
				pLink->m_Length = length_in_mile;
				pLink->m_FreeFlowTravelTime = pLink->m_Length / pLink->m_FreeSpeed*60.0f;  // convert from hour to min
				pLink->m_StaticTravelTime = pLink->m_FreeFlowTravelTime;

				pLink->m_BPR_alpha_term = BPR_alpha_term;
				pLink->m_BPR_beta_term = BPR_beta_term;


				pLink->m_MaximumServiceFlowRatePHPL = capacity_in_pcphpl;
				pLink->m_LaneCapacity = pLink->m_MaximumServiceFlowRatePHPL;
				pLink->m_link_type_str = link_type_str;
				pLink->m_link_type = link_type;
				pLink->m_Grade = grade;
				
				pLink->m_Kjam = k_jam;
				m_NodeNoMap[pLink->m_FromNodeID]->m_Connections += 1;
				m_NodeNoMap[pLink->m_ToNodeID]->m_Connections += 1;


				if (m_LinkTypeMap[pLink->m_link_type].IsFreeway() || m_LinkTypeMap[pLink->m_link_type].IsRamp())
				{
					m_NodeNoMap[pLink->m_FromNodeID]->m_bConnectedToFreewayORRamp = true;
					m_NodeNoMap[pLink->m_ToNodeID]->m_bConnectedToFreewayORRamp = true;

				}

				m_NodeNoMap[pLink->m_FromNodeID]->m_OutgoingLinkVector.push_back(pLink->m_LinkNo);
				m_NodeNoMap[pLink->m_ToNodeID]->m_IncomingLinkVector.push_back(pLink->m_LinkNo);

				if (m_LinkTypeMap[pLink->m_link_type].IsConnector() == false)
					m_NodeNoMap[pLink->m_ToNodeID]->m_IncomingNonConnectors++;

				//estimation node level production and attraction
				m_NodeNoMap[pLink->m_FromNodeID]->m_NodeProduction += min(pLink->m_LaneCapacity, pLink->m_Saturation_flow_rate_in_vhc_per_hour_per_lane) * min(4, pLink->m_NumberOfLanes) / 2.0f; // min() is used to avoid artefial large capacity and lare number of lanes
				m_NodeNoMap[pLink->m_ToNodeID]->m_NodeAttraction += min(pLink->m_LaneCapacity, pLink->m_Saturation_flow_rate_in_vhc_per_hour_per_lane) * min(4, pLink->m_NumberOfLanes) / 2.0f;  // /2 is used to get reasonable demand


				unsigned long LinkKey = GetLinkKey(pLink->m_FromNodeID, pLink->m_ToNodeID);

				m_NodeNotoLinkMap[LinkKey] = pLink;

				__int64  LinkKey2 = GetLink64Key(pLink->m_FromNodeNumber, pLink->m_ToNodeNumber);
				m_NodeNumbertoLinkMap[LinkKey2] = pLink;

				m_LinkNotoLinkMap[i] = pLink;
				m_LinkIDtoLinkMap[road_link_id] = pLink;


				pLink->m_FromPoint = m_NodeNoMap[pLink->m_FromNodeID]->pt;
				pLink->m_ToPoint = m_NodeNoMap[pLink->m_ToNodeID]->pt;

				if (pLink->m_Length >= 0.00001)
				{
					default_coordinate_distance_sum += pLink->DefaultDistance();
					length_sum_in_mile += pLink->m_Length;
				}

			


				if (!bNodeNonExistError)
				{
					//TRACE("\nAdd link no.%d,  %d -> %d",i,pLink->m_FromNodeNumber, pLink->m_ToNodeNumber );
					m_LinkSet.push_back(pLink);
					m_LinkNoMap[i] = pLink;
					i++;
				}
			}

		}


		if (bNodeNonExistError)
		{
			ofstream m_WarningLogFile;

			m_WarningLogFile.open(m_ProjectDirectory + "NeXTA.log", ios::out);
			if (m_WarningLogFile.is_open())
			{
				m_WarningLogFile.width(12);
				m_WarningLogFile.precision(3);
				m_WarningLogFile.setf(ios::fixed);
				m_WarningLogFile << warning_message << endl;

			}


			m_WarningLogFile.close();

			AfxMessageBox("Some nodes inroad_ink.csv have not been defined in node.csv. Please check file NeXTA.log.");
		}

		m_UnitDistance = 1.0f;

		if (length_sum_in_mile > 0.000001f)
		{
			m_UnitDistance = default_coordinate_distance_sum / length_sum_in_mile;

		}


		double AvgLinkLength = length_sum_in_mile / max(1, m_LinkSet.size());

		m_NodeDisplaySize = min(10, AvgLinkLength*0.05);  



		m_MovementTextBoxSizeInDistance = AvgLinkLength * 0.2; // 20% of average link length

		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_Length <= 0.00001)  //reset zero length link
			{
				(*iLink)->m_Length = pLink->DefaultDistance() / m_UnitDistance;

			}

		}

		// count of # of transit and walking links

		int transit_link_count = 0;
		int walking_link_count = 0;
		std::list<DTALink*>::iterator iLink;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{


			if (m_LinkTypeMap[(*iLink)->m_link_type].IsTransit())
				transit_link_count++;

			if (m_LinkTypeMap[(*iLink)->m_link_type].IsWalking())
				walking_link_count++;

		}

		if (walking_link_count > 0 || transit_link_count > 0)
		{
			m_LinkDataLoadingStatus.Format("%d links (%d transit links, %d walking links) are loaded from file %s.",
				m_LinkSet.size(), transit_link_count, walking_link_count, lpszFileName);

		}
		else
		{
			m_LinkDataLoadingStatus.Format("%d links are loaded from file %s.", m_LinkSet.size(), lpszFileName);
		}


		if (bTwoWayLinkFlag == true)
			m_bLinkToBeShifted = true;

		GenerateOffsetLinkBand();

		BuildGridSystem();

		if (warning_message.GetLength() >= 1)
		{
			CString final_message;
			final_message = "Warning messages:\n" + warning_message + "\n\nIf you save the network in *.tnp again, then duplicated links and links with non-existings nodes will be automatically removed.\n";
			AfxMessageBox(final_message);
		}
		return true;
		}
		else
		{
			if (LayerNo == 0)
			{
				AfxMessageBox("Error: File road_ink.csv cannot be opened.\n It might be currently used and locked by EXCEL.");
			}
			return false;
			//		g_ProgramStop();
		}


}

bool CTLiteDoc::ReadSensorCSVFile(LPCTSTR lpszFileName)
{

	long i = m_LinkSet.size();
	DTALink* pLink = 0;
	float default_coordinate_distance_sum = 0;
	float length_sum_in_mile = 0;

	CString error_message;
	CString warning_message = "";
	int warning_message_no = 0;

	bool bTwoWayLinkFlag = false;

	bool length_field_warning = false;
	bool number_of_lanes_field_warning = false;
	bool speed_limit_field_warning = false;
	bool capacity_field_warning = false;
	bool type_field_warning = false;


	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{
		bool bNodeNonExistError = false;
		while (parser.ReadRecord())
		{
			long link_id = 0;
			long from_node_id;
			long to_node_id ;


			string road_link_id;
			parser.GetValueByFieldName("road_link_id", road_link_id);
			

			DTALink* pExistingLink = FindLinkWithLinkID(road_link_id);

			if(pExistingLink!=NULL)
			{
				pLink->ResetMOEAry(1440);  // use one day horizon as the default value
				CString str_MOE_15Min;

				int time_interval_in_min = 15;

				int Start15Min = 0;
				int End15Min =1440;
				int min;
				for (min = Start15Min; min <= End15Min; min += 15)
				{
					float travel_time_in_min = 0;
					float link_volume = 0;
					float density = -1;
					float speed = -1;
					float capacity = -1;
					std::string column_name;

					str_MOE_15Min.Format("%s_vol,", GetTimeStampString(min));
					column_name = CString2StdString(str_MOE_15Min);
					parser.GetValueByFieldName(column_name.c_str(), link_volume);

					str_MOE_15Min.Format("%s_spd,", GetTimeStampString(min));
					column_name = CString2StdString(str_MOE_15Min);
					parser.GetValueByFieldName(column_name, speed);

					str_MOE_15Min.Format("%s_tt,", GetTimeStampString(min));
					column_name = CString2StdString(str_MOE_15Min);
					parser.GetValueByFieldName(column_name, travel_time_in_min);

					str_MOE_15Min.Format("%s_density,", GetTimeStampString(min));
					column_name = CString2StdString(str_MOE_15Min);
					parser.GetValueByFieldName(column_name, capacity);

					str_MOE_15Min.Format("%s_cap,", GetTimeStampString(min));
					column_name = CString2StdString(str_MOE_15Min);
					parser.GetValueByFieldName(column_name, capacity);

					int t = Start15Min;
					for (int tt = t; tt < t + time_interval_in_min; tt++)
					{
						if (tt < pLink->m_LinkMOEArySize)
						{
							pLink->m_LinkMOEAry[tt].TravelTime = travel_time_in_min;
							pLink->m_LinkMOEAry[tt].LinkFlow = link_volume;
							pLink->m_LinkMOEAry[tt].Density = density;
							pLink->m_LinkMOEAry[tt].Speed = speed;

						}
					}

				}
			}
		}

		if (warning_message.GetLength() >= 1)
		{
			CString final_message;
			final_message = "Warning messages:\n" + warning_message + "\n\nIf you save the network in *.tnp again, then duplicated links and links with non-existings nodes will be automatically removed.\n";
			AfxMessageBox(final_message);
		}
		return true;
	}

}

bool CTLiteDoc::ReadScenarioSettingCSVFile(LPCTSTR lpszFileName)
{
	CCSVParser parser_scenario;
	if (parser_scenario.OpenCSVFile(lpszFileName))
	{

		m_NumberOfScenarioSettings = 0;
		while(parser_scenario.ReadRecord())
		{
			parser_scenario.GetValueByFieldName("number_of_iterations",m_number_of_iterations);
			parser_scenario.GetValueByFieldName("traffic_flow_model",m_traffic_flow_model);
			parser_scenario.GetValueByFieldName("signal_representation_model",m_signal_reresentation_model);

			parser_scenario.GetValueByFieldName("traffic_analysis_method",m_traffic_analysis_method);
			
			parser_scenario.GetValueByFieldName("calibration_data_start_time_in_min", m_calibration_data_start_time_in_min);
			parser_scenario.GetValueByFieldName("calibration_data_end_time_in_min", m_calibration_data_end_time_in_min);



			m_NumberOfScenarioSettings++;
		}

		if(m_NumberOfScenarioSettings >=1)
			return true;

	}
	return false;
}


bool CTLiteDoc::WriteScenarioSettingCSVFile(LPCTSTR lpszFileName)
{
	int scenario_no;
	string scenario_name;
	int random_seed;
	int ODME_start_iteration;

	float ODME_max_percentage_deviation_wrt_hist_demand;
	float ODME_step_size;
	int ue_gap_calculation_method;
	int agent_demand_input_mode;
	int calibration_data_start_time_in_min;
	int calibration_data_end_time_in_min;
	int routing_movement_delay_mode;

	int accessibility_mode;
	int LearningRate[400] = {5};

	CCSVParser parser_scenario;
	if (parser_scenario.OpenCSVFile(lpszFileName))
	{
		m_NumberOfScenarioSettings = 0;

		while(parser_scenario.ReadRecord())
		{
			parser_scenario.GetValueByFieldName("scenario_no",scenario_no);
			parser_scenario.GetValueByFieldName("scenario_name",scenario_name);

			// the following parameters can be changed through dialog
			//parser_scenario.GetValueByFieldName("number_of_iterations",m_number_of_iterations);//
			//parser_scenario.GetValueByFieldName("traffic_flow_model",m_traffic_flow_model);
			//parser_scenario.GetValueByFieldName("traffic_analysis_method",m_traffic_analysis_method);
			//parser_scenario.GetValueByFieldName("demand_multiplier",m_demand_multiplier);
			//parser_scenario.GetValueByFieldName("emission_data_output",m_emission_data_output);

			parser_scenario.GetValueByFieldName("random_seed",random_seed);

			parser_scenario.GetValueByFieldName("ODME_start_iteration",ODME_start_iteration);
			parser_scenario.GetValueByFieldName("ODME_max_percentage_deviation_wrt_hist_demand",ODME_max_percentage_deviation_wrt_hist_demand);
			parser_scenario.GetValueByFieldName("ODME_step_size",ODME_step_size);
			parser_scenario.GetValueByFieldName("ue_gap_calculation_method",ue_gap_calculation_method);
			parser_scenario.GetValueByFieldName("calibration_data_start_time_in_min",calibration_data_start_time_in_min);
			parser_scenario.GetValueByFieldName("calibration_data_end_time_in_min",calibration_data_end_time_in_min);
			parser_scenario.GetValueByFieldName("routing_movement_delay_mode",routing_movement_delay_mode);


			m_NumberOfScenarioSettings++;
		}

		parser_scenario.CloseCSVFile ();


	}


	if(m_NumberOfScenarioSettings==1)
	{
		CCSVWriter ScenarioFile;

		if(ScenarioFile.Open(lpszFileName))
		{


			ScenarioFile.SetFieldNameAndValue("scenario_no",scenario_no);
			ScenarioFile.SetFieldNameAndValue("scenario_name",scenario_name);
			ScenarioFile.SetFieldNameAndValue("number_of_iterations",m_number_of_iterations);//
			ScenarioFile.SetFieldNameAndValue("traffic_flow_model",m_traffic_flow_model);
			ScenarioFile.SetFieldNameAndValue("signal_representation_model",m_signal_reresentation_model);
			ScenarioFile.SetFieldNameAndValue("traffic_analysis_method",m_traffic_analysis_method);
			ScenarioFile.SetFieldNameAndValue("random_seed",random_seed);
			ScenarioFile.SetFieldNameAndValue("ODME_start_iteration",ODME_start_iteration);
			ScenarioFile.SetFieldNameAndValue("ODME_max_percentage_deviation_wrt_hist_demand",ODME_max_percentage_deviation_wrt_hist_demand);
			ScenarioFile.SetFieldNameAndValue("ODME_step_size",ODME_step_size);
			ScenarioFile.SetFieldNameAndValue("calibration_data_start_time_in_min",calibration_data_start_time_in_min);
			ScenarioFile.SetFieldNameAndValue("calibration_data_end_time_in_min",calibration_data_end_time_in_min);

			ScenarioFile.WriteHeader ();
			ScenarioFile.WriteRecord ();

		}
	}else if (m_NumberOfScenarioSettings >=2)
	{
		AfxMessageBox("There are multiple scenarios in input_scenario_settings.csv. Please use Excel to make changes.", MB_ICONINFORMATION);

		OpenCSVFileInExcel(lpszFileName);

	}

	return true;
}


bool CTLiteDoc::ReadMetaDemandCSVFile(LPCTSTR lpszFileName)
{
	CString directory;

	// reset
	m_DemandLoadingStartTimeInMin = 1440;
	m_DemandLoadingEndTimeInMin = 0;

	m_DemandFileVector.clear ();

	if(m_ProjectFile.GetLength () ==0 )
	{
		AfxMessageBox("The project directory has not been specified. Please save the project to a new folder first.");
		return false;
	}
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);


	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{

		while(parser.ReadRecord())
		{
			string file_name;
			string format_type;
			int demand_type= 1;
			int start_time_in_min,end_time_in_min;
			float subtotal_demand_volume;

			parser.GetValueByFieldName("file_name",file_name);

			m_DemandFileVector.push_back (file_name.c_str ());
			parser.GetValueByFieldName("format_type",format_type);
			parser.GetValueByFieldName("demand_type",demand_type);

			if(demand_type >= m_DemandTypeVector.size())  // demand type out of bound
				break;

			parser.GetValueByFieldName("start_time_in_min",start_time_in_min);
			parser.GetValueByFieldName("end_time_in_min",end_time_in_min);
			parser.GetValueByFieldName("subtotal_demand_volume",subtotal_demand_volume);

			if(m_DemandLoadingStartTimeInMin > start_time_in_min)
				m_DemandLoadingStartTimeInMin = start_time_in_min;

			if(m_DemandLoadingEndTimeInMin < end_time_in_min)
				m_DemandLoadingEndTimeInMin = end_time_in_min;


			CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

			if(pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin==-1)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin = m_DemandLoadingStartTimeInMin;
			}else if ( pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin > m_DemandLoadingStartTimeInMin)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin = m_DemandLoadingStartTimeInMin;

			}

			if(pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin==-1)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin = m_DemandLoadingEndTimeInMin;
			}else if ( pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin < m_DemandLoadingEndTimeInMin)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin = m_DemandLoadingEndTimeInMin;

			}

			pMainFrame->m_wndPlayerSeekBar.Invalidate ();

		}

	}  // for each record
	return true;

}

bool CTLiteDoc::ReadMetaSignalCSVFile(LPCTSTR lpszFileName)
{
	CString directory;


	m_DemandFileVector.clear ();

	if(m_ProjectFile.GetLength () ==0 )
	{
		AfxMessageBox("The project directory has not been specified. Please save the project to a new folder first.");
		return false;
	}
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);


	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{

		while(parser.ReadRecord())
		{
			string file_name;
			string format_type;
			int demand_type= 1;
			int start_time_in_min,end_time_in_min;
			float subtotal_demand_volume;

			parser.GetValueByFieldName("file_name",file_name);

			m_DemandFileVector.push_back (file_name.c_str ());
			parser.GetValueByFieldName("format_type",format_type);
			parser.GetValueByFieldName("demand_type",demand_type);

			if(demand_type >= m_DemandTypeVector.size())  // demand type out of bound
				break;

			parser.GetValueByFieldName("start_time_in_min",start_time_in_min);
			parser.GetValueByFieldName("end_time_in_min",end_time_in_min);
			parser.GetValueByFieldName("subtotal_demand_volume",subtotal_demand_volume);

			if(m_DemandLoadingStartTimeInMin > start_time_in_min)
				m_DemandLoadingStartTimeInMin = start_time_in_min;

			if(m_DemandLoadingEndTimeInMin < end_time_in_min)
				m_DemandLoadingEndTimeInMin = end_time_in_min;


			CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

			if(pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin==-1)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin = m_DemandLoadingStartTimeInMin;
			}else if ( pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin > m_DemandLoadingStartTimeInMin)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin = m_DemandLoadingStartTimeInMin;

			}

			if(pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin==-1)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin = m_DemandLoadingEndTimeInMin;
			}else if ( pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin < m_DemandLoadingEndTimeInMin)
			{
				pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin = m_DemandLoadingEndTimeInMin;

			}

			pMainFrame->m_wndPlayerSeekBar.Invalidate ();

		}

	}  // for each record
	return true;

}


bool CTLiteDoc::ReadDemandCSVFile(LPCTSTR lpszFileName)
{
	float LengthinMB;
	FILE* pFile;
	long Length;
	fopen_s(&pFile,lpszFileName,"rb");
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		Length = ftell(pFile);
		fclose(pFile);

		LengthinMB= Length*1.0/1024/1024;
		if(LengthinMB > 100)
		{
			CString msg;
			msg.Format("The input_demand.csv file is %5.1f MB in size.\nIt could take quite a while to load this file.\nWould you like to load the input_demand file?",LengthinMB);
			if(AfxMessageBox(msg,MB_YESNO|MB_ICONINFORMATION)==IDNO)
				return true;
		}
	}

	//  // step 1: unzip
	//
	//CString directory;

	//if(m_ProjectFile.GetLength () ==0 )
	//{
	//	AfxMessageBox("The project directory has not been specified. Please save the project to a new folder first.");
	//	return false;
	//}
	//directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	//HZIP hz = OpenZip(directory+"TNP_data.zip",0);
	//   
	//SetCurrentDirectory(directory);

	//   ZIPENTRY ze; GetZipItem(hz,-1,&ze); int numitems=ze.index;
	// 	for (int zi=0; zi<numitems; zi++)
	//  { 
	//GetZipItem(hz,zi,&ze);
	//   UnzipItem(hz,zi,ze.name);
	//  }
	//   CloseZip(hz);


	//// step 2: read bin file

	// // obtain file size:
	//// obtain file size:
	// pFile = fopen ( directory+"input_demand.bin" , "rb" );
	// if (pFile!=NULL)
	// {
	//  float total_demand = 0;
	// fseek (pFile , 0 , SEEK_END);
	// long lSize = ftell (pFile);
	// rewind (pFile);
	// int RecordCount = lSize/sizeof(DemandRecordData);
	// if(RecordCount >=1)
	// {
	// // allocate memory to contain the whole file:
	// DemandRecordData* pDemandData = new DemandRecordData [RecordCount];
	// // copy the file into the buffer:
	// fread (pDemandData,1,sizeof(DemandRecordData)*RecordCount,pFile);
	// int demand_type;
	// int demand_type_size = min(5,m_DemandTypeVector.size());
	// for(int i = 0; i < RecordCount; i++)
	// {
	//for(demand_type = 0; demand_type < demand_type_size; demand_type++)
	//{
	//	total_demand +=  pDemandData[i].number_of_Agents[demand_type];
	//	m_ZoneMap[pDemandData[i].origin_zone ].m_ODDemandMatrix [pDemandData[i].destination_zone].SetValue (demand_type+1,pDemandData[i].number_of_Agents[demand_type]);
	//}
	// 
	// }
	// 
	// /* the whole file is now loaded in the memory buffer. */

	// // terminate
	// fclose (pFile);

	//	m_AMSLogFile << RecordCount << "demand entries are loaded from file input_demand.bin "  ". Total demand =  " 
	//		<< total_demand << endl;
	//	m_DemandDataLoadingStatus.Format ("%d demand entries are loaded from file input_demand.bin. Total demand = %f",RecordCount,total_demand);


	// if(pDemandData!=NULL)
	//  delete pDemandData;

	//
	//  return true;
	// }
	// }

	////


	long lineno = 0;
	float total_demand = 0;
	FILE* st;
	fopen_s(&st,lpszFileName, "r");
	if (st!=NULL)
	{
		char  str_line[2000]; // input string
		int str_line_size;
		g_read_a_line(st,str_line, str_line_size); //  skip the first line

		int origin_zone, destination_zone;
		float number_of_Agents ;
		float starting_time_in_min;
		float ending_time_in_min;


		while( fscanf_s(st,"%d,%d,%f,%f,",&origin_zone,&destination_zone,&starting_time_in_min, &ending_time_in_min) >0)
		{
			// static traffic assignment, set the demand loading horizon to [0, 60 min]
			for(unsigned int demand_type = 1; demand_type <= m_DemandTypeVector.size(); demand_type++)
			{
				number_of_Agents = 0;
				if( fscanf_s(st,"%f,",&number_of_Agents) == 0)
				{

					CString msg;
					msg.Format ("Demand type %d in line %d of file %s cannot be found. ",demand_type,lineno+1,lpszFileName);
					AfxMessageBox(msg,MB_OK|MB_ICONINFORMATION);
					return false;
				}

				if(number_of_Agents < -0.0001)
					number_of_Agents = 0;

			}

			lineno++;
		}

		fclose(st);
		m_AMSLogFile << lineno << "demand entries are loaded from file " << lpszFileName << ". Total demand =  " << total_demand << endl;
		m_DemandDataLoadingStatus.Format ("%d demand entries are loaded from file %s. Total demand = %f",lineno,lpszFileName,total_demand);
		return true;
	}else
	{
		//		AfxMessageBox("Error: File input_demand.csv cannot be found or opened.\n It might be currently used and locked by EXCEL.");
		return false;
		//		g_ProgramStop();
	}

}


bool CTLiteDoc::ReadDemandTypeCSVFile(LPCTSTR lpszFileName)
{
	long lineno = 1;
	CCSVParser parser;

	if (parser.OpenCSVFile(lpszFileName))
	{
		m_DemandTypeVector.clear();

		while(parser.ReadRecord())
		{
			int demand_type;
			float averageVOT;

			if(parser.GetValueByFieldName("demand_type_id",demand_type) == false)
				break;
			string demand_type_name, pricing_type_name;
			parser.GetValueByFieldName("demand_type",demand_type_name);
			parser.GetValueByFieldName("VOT", averageVOT);


			DTADemandType element;
			element.demand_type = demand_type;
			element.demand_type_name  = demand_type_name.c_str ();
			element.average_VOT = max(0,min(averageVOT, 1000));

			m_DemandTypeVector.push_back(element);

			lineno++;
		}
		m_AMSLogFile << "Read " << m_DemandTypeVector.size() << " demand types from file "  << lpszFileName << endl; 

		CString msg;
		msg.Format("Imported %d demand types from file %s",m_DemandTypeVector.size(),lpszFileName);
		m_MessageStringVector.push_back (msg);

		return true;
	}else
	{
		CString msg;
		msg.Format("Imported 0 demand types from file %s",lpszFileName);
		m_MessageStringVector.push_back (msg);

		return false;
	}

}


bool CTLiteDoc::ReadLinkTypeCSVFile(LPCTSTR lpszFileName)
{
	long lineno = 0;
	CCSVParser parser;

	if (parser.OpenCSVFile(lpszFileName))
	{
		m_LinkTypeMap.clear();
		while(parser.ReadRecord())
		{
			DTALinkType element;

			if(parser.GetValueByFieldName("link_type",element.link_type ) == false)
			{
				if(lineno==0)
				{
					AfxMessageBox("Field link_type cannot be found in link_type.csv.");
				}
				break;
			}



			if(element.link_type<0)
			{
				CString str;
				str.Format ("Field link_type in file link_type.csv has an invalid value of %d at line %d. Please check.",element.link_type, lineno);
				AfxMessageBox(str);
				break;
			}

			if(parser.GetValueByFieldName("link_type_name",element.link_type_name ) == false)
			{
				AfxMessageBox("Field link_type_name cannot be found in link_type.csv.");
				break;
			}



			if(parser.GetValueByFieldName("type_code",element.type_code   ) == false)
			{
				AfxMessageBox("Field type_code cannot be found in link_type.csv.");
				break;
			}

			parser.GetValueByFieldName("travel_time_bias_factor",element.travel_time_bias_factor   ) ;
			parser.GetValueByFieldName("approximate_cycle_length_in_second",element.approximate_cycle_length_in_second   ) ;
			parser.GetValueByFieldName("saturation_flow_rate_in_vhc_per_hour_per_lane",element.saturation_flow_rate_in_vhc_per_hour_per_lane   ) ;
			parser.GetValueByFieldName("capacity_adjustment_factor",element.capacity_adjustment_factor   ) ;

			if(element.type_code .find('f') != string::npos)
			{
				m_LinkTypeFreeway = element.link_type;
			}

			if(element.type_code .find('a') != string::npos)
			{
				m_LinkTypeArterial = element.link_type;
			}

			if(element.type_code .find('h') != string::npos)
			{
				m_LinkTypeHighway = element.link_type;
			}


			m_LinkTypeMap[element.link_type] = element;

			lineno++;
		}

		CString msg;
		msg.Format("Imported %d link types from file %s",m_LinkTypeMap.size(),lpszFileName);
		m_MessageStringVector.push_back (msg);
		m_AMSLogFile << "Read " << m_LinkTypeMap.size() << " link types from file "  << lpszFileName << endl; 

		return true;
	}else
	{
		return false;

	}

}

void CTLiteDoc::SendTexttoStatusBar(CString str, int index)
{
	POSITION pos = GetFirstViewPosition();
	CView *p = (CView *)this->GetNextView(pos);
	CStatusBar* bar = (CStatusBar*)(p->GetParentFrame()->GetMessageBar());
	bar->SetPaneText(index, str);
}




BOOL CTLiteDoc::SaveLinkData(LPCTSTR lpszPathName,bool bExport_Link_MOE_in_input_link_CSF_File, int SelectedLayNo)
{
	CWaitCursor wait;

	// reidentify bottlenecks;

	int StartHour = m_DemandLoadingStartTimeInMin/60;
	int EndHour = int((m_DemandLoadingEndTimeInMin+59)/60);

	int Start15Min =  0;
	int End15Min =  1440;


	FILE* st;
	fopen_s(&st,lpszPathName,"w");
	if(st!=NULL)
	{
		std::list<DTALink*>::iterator iLink;
		fprintf(st,"road_link_id,name,from_node_id,to_node_id,facility_type,link_type,direction,length,lanes,free_speed,capacity,demand_type_code,geometry,");
		fprintf(st,"\n");	

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if( (*iLink)->m_FromNodeID != (*iLink)->m_ToNodeID)
			{
				int ToNodeID = (*iLink)->m_ToNodeID ;
				DTANode* pNode = m_NodeNoMap[ToNodeID];
	
				CString link_type_name = " ";

				if(m_LinkTypeMap.find((*iLink)->m_link_type) != m_LinkTypeMap.end())
				{
					link_type_name = m_LinkTypeMap[(*iLink)->m_link_type].link_type_name.c_str ();
				}

				int sign_flag = 1;

				std::replace( (*iLink)->m_Name.begin(), (*iLink)->m_Name.end(), ',', ' '); 

				fprintf(st, "%d,%s,%d,%d,%s,%d,%d,",
					(*iLink)->m_LinkID,
					(*iLink)->m_Name.c_str(),
					(*iLink)->m_FromNodeNumber,
					(*iLink)->m_ToNodeNumber,
					(*iLink)->m_link_type_str.c_str(),
					(*iLink)->m_link_type,
					(*iLink)->m_Direction);

				fprintf(st, "%.5f,%d,%.1f,%.1f,\"%s\",",
					(*iLink)->m_Length,
					(*iLink)->m_NumberOfLanes,
					(*iLink)->m_FreeSpeed,
					(*iLink)->m_LaneCapacity,
					(*iLink)->m_demand_type_code.c_str());

				// geometry
				fprintf(st,"\"LINESTRING (");

				for(unsigned int si = 0; si< (*iLink)->m_ShapePoints.size(); si++)
				{
					fprintf(st,"%f %f ",(*iLink)->m_ShapePoints[si].x, (*iLink)->m_ShapePoints[si].y);

					if(si!=(*iLink)->m_ShapePoints.size()-1)
						fprintf(st,",");
				}

				fprintf(st,")\"");

				fprintf(st,"\n");
			}  // layer no ==0
		}

		fclose(st);
	}else
	{
		CString message;
		message.Format("Error: File %s cannot be opened.\nIt might be currently used and locked by EXCEL.", lpszPathName);
		AfxMessageBox(message);
		return false;
	}
}
BOOL CTLiteDoc::SaveNodeFile()
{

	CString  directory = m_ProjectDirectory;
	FILE* st = NULL;


	fopen_s(&st,directory+"node.csv","w");
	if(st!=NULL)
	{
		std::list<DTANode*>::iterator iNode;
		fprintf(st, "node_id,name,x_coord,y_coord,node_type,ctrl_type,zone_id,geometry\n");
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

				CString control_type_name="";

				if( m_NodeTypeMap.find((*iNode)->m_ControlType)!= m_NodeTypeMap.end())				{
					control_type_name = m_NodeTypeMap[(*iNode)->m_ControlType].c_str() ;
				}

				

				std::replace( (*iNode)->m_Name.begin(), (*iNode)->m_Name.end(), ',', ' '); 

				fprintf(st, "%d,%s,%f,%f,%s,%s,%d,",
					(*iNode)->m_NodeID,
					(*iNode)->m_Name.c_str(),
					(*iNode)->pt.x, (*iNode)->pt.y, 
					(*iNode)->m_node_type.c_str(),
					(*iNode)->m_ctrl_type.c_str(),
					(*iNode)->m_ZoneID);

				fprintf(st, "\"POINT (%f %f)\"\n",
					(*iNode)->pt.x, (*iNode)->pt.y);

		}

		fclose(st);
	}else
	{
		AfxMessageBox("Error: File node.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return false;
	}
}


BOOL CTLiteDoc::SaveProject(LPCTSTR lpszPathName, int SelectedLayNo)
{
	Modify(false);
	FILE* st = NULL;
	CString directory;
	CString prj_file = lpszPathName;
	directory = prj_file.Left(prj_file.ReverseFind('\\') + 1);

	CWaitCursor wc;

	CString OldDirectory = m_ProjectDirectory;
	// update m_ProjectDirectory
	m_ProjectDirectory = directory;

	char lpbuffer[64];

	sprintf_s(lpbuffer,"%f",m_NodeDisplaySize);

	WritePrivateProfileString("GUI", "node_display_size",lpbuffer,lpszPathName);

	sprintf_s(lpbuffer,"%f",m_NodeTextDisplayRatio);
	WritePrivateProfileString("GUI","node_text_display_ratio",lpbuffer,lpszPathName);

	SaveNodeFile();

	SaveLinkData(directory+"road_link.csv",true,SelectedLayNo);

	//	SaveInputLanesFile(directory+"input_lanes.csv");



	if (bSynchroImportReadyToSaveFlag)
	{
		SaveTimingData();
		bSynchroImportReadyToSaveFlag = false;
	}

	//fopen_s(&st,directory+"input_phase.csv","w");
	//if(st!=NULL)
	//{
	//	std::list<DTANode*>::iterator iNode;
	//	fprintf(st, "node_name,node_id,phase_index,incoming_link_from_node_id,outgoing_link_to_node_id,turning_direction\n");
	//	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	//	{
	//		if((*iNode)->m_LayerNo == SelectedLayNo) 
	//		{
	//			for(unsigned int p = 0; p< (*iNode)->m_PhaseVector .size(); p++)
	//			{
	//				for(unsigned int m = 0; m< (*iNode)->m_PhaseVector[p].movement_index_vector.size() ; m++)
	//				{
	//					int movement_index = (*iNode)->m_PhaseVector[p].movement_index_vector[m];;
	//					DTANodeMovement movement = (*iNode)->m_MovementDataMap.m_MovementVector[movement_index];


	//					fprintf(st,"%s,%d,%d,%d,%d,%s\n",(*iNode)->m_Name.c_str (), 
	//						(*iNode)->m_NodeID ,
	//						p+1,
	//						m_NodeNotoNumberMap[movement.in_link_from_node_id],
	//						m_NodeNotoNumberMap[movement.out_link_to_node_id ],
	//						GetTurnString(movement.movement_turn));
	//				}
	//			}
	//		}
	//	}

	//	fclose(st);
	//}else
	//{
	//	AfxMessageBox("Error: File input_phase.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
	//	return false;
	//}
	if(m_bLoadNetworkDataOnly)  // only network data are loaded, no need to save the other data.
		return true; 


	OnFileSaveimagelocation();

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

	CString NEXTASettingsPath;
	NEXTASettingsPath.Format ("%s\\NEXTA_Settings.ini", pMainFrame->m_CurrentDirectory);

	CString color_str;
	color_str.Format("%d", theApp.m_FreewayColor);
	WritePrivateProfileString("initialization", "FreewayColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_RampColor);
	WritePrivateProfileString("initialization", "RampColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_ArterialColor );
	WritePrivateProfileString("initialization", "ArterialColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_ConnectorColor  );
	WritePrivateProfileString("initialization", "ConnectorColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_TransitColor  );
	WritePrivateProfileString("initialization", "TransitColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_WalkingColor   );
	WritePrivateProfileString("initialization", "WalkingColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_BackgroundColor   );
	WritePrivateProfileString("initialization", "BackgroundColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_NodeColor    );
	WritePrivateProfileString("initialization", "NodeColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_NodeBrushColor    );
	WritePrivateProfileString("initialization", "NodeBackgroundColor",color_str,NEXTASettingsPath);

	color_str.Format("%d", theApp.m_ZoneColor   );
	WritePrivateProfileString("initialization", "ZoneColor",color_str,NEXTASettingsPath);

	return true;
}
void CTLiteDoc::OnFileSaveProject()
{
	if(theApp.m_VisulizationTemplate == e_train_scheduling)
		return;

	if(m_ProjectDirectory.GetLength ()>3 && m_ProjectFile.GetLength()>3)
		SaveProject(m_ProjectFile);
	else
		OnFileSaveProjectAs();
}

void CTLiteDoc::OnFileSaveProjectAs()
{
	Modify(false);

	int number_of_signals  = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{


		if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
		{
			number_of_signals++;
		}

	}

	try{
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

		CFileDialog fdlg (FALSE, "*.tnp", "*.tnp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ENABLESIZING,
			"Transportation Network Project (*.tnp)|*.tnp|",pMainFrame,0,true);


		if(fdlg.DoModal()==IDOK)
		{
			CString path = fdlg.GetPathName  ();
			CWaitCursor wait;
			m_ProjectFile = path;

			m_ProjectTitle = GetWorkspaceTitleName(m_ProjectFile);

			bool bSavedFlag = false;
			if(path.Find("tnp")>=0)  //Transportation network project format
			{
				bSavedFlag = SaveProject(path);
			}

			if(bSavedFlag)
			{
				CString msg;
				if(m_NodeSet.size()>0)
				{
					msg.Format ("Files node.csv and road_ink.csv  have been successfully saved with %d nodes, %d signals, %d links, %d zones.",
						m_NodeSet.size(), 
						number_of_signals,
						m_LinkSet.size(), 
						m_ZoneMap.size()
						);
					AfxMessageBox(msg,MB_OK|MB_ICONINFORMATION);
				}
				SetTitle(m_ProjectTitle);


			}
		}

	} catch (char *str  ) 
	{
		AfxMessageBox(str,MB_ICONINFORMATION);
	}
	// TODO: Add your command handler code here
}

void CTLiteDoc::CalculateDrawingRectangle(bool NodeLayerOnly)
{   

	CWaitCursor wait;

	std::list<DTALink*>::iterator iLink;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->CalculateShapePointRatios();

	}

	bool bRectInitialized = false;
	m_AdjLinkSize = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		if(NodeLayerOnly || (*iNode)->m_Connections >0 )  // we might try import node layer only from shape file, so all nodes have no connected links. 
		{
			if(!bRectInitialized)
			{
				m_NetworkRect.left = (*iNode)->pt.x ;
				m_NetworkRect.right = (*iNode)->pt.x;
				m_NetworkRect.top = (*iNode)->pt.y;
				m_NetworkRect.bottom = (*iNode)->pt.y;
				bRectInitialized = true;
			}

			if((*iNode)->m_Connections > m_AdjLinkSize)
				m_AdjLinkSize = (*iNode)->m_Connections;

			m_NetworkRect.Expand((*iNode)->pt);
		}

	}

	for (std::list<DTAPoint*>::iterator iPoint = m_DTAPointSet.begin(); iPoint != m_DTAPointSet.end(); iPoint++)
	{
		if(!bRectInitialized)
		{
			m_NetworkRect.left = (*iPoint)->pt.x ;
			m_NetworkRect.right = (*iPoint)->pt.x;
			m_NetworkRect.top = (*iPoint)->pt.y;
			m_NetworkRect.bottom = (*iPoint)->pt.y;
			bRectInitialized = true;
		}

		m_NetworkRect.Expand((*iPoint)->pt);

	}

	if(m_BackgroundBitmapLoaded)  // if there is a bitmap background
	{

		if(!bRectInitialized)  // there is no node or link layer
		{
			m_NetworkRect.left = m_ImageX1;
			m_NetworkRect.right = m_ImageX2;
			m_NetworkRect.top = m_ImageY2;
			m_NetworkRect.bottom = m_ImageY1;
			bRectInitialized = true;

		}

	}

	for (std::list<DTALine*>::iterator iLine = m_DTALineSet.begin(); iLine != m_DTALineSet.end(); iLine++)
	{
		for(unsigned int i = 0; i< (*iLine)->m_ShapePoints .size(); i++)
		{
			if(!bRectInitialized)
			{
				m_NetworkRect.left = (*iLine)->m_ShapePoints[i].x ;
				m_NetworkRect.right = (*iLine)->m_ShapePoints[i].x;
				m_NetworkRect.top = (*iLine)->m_ShapePoints[i].y;
				m_NetworkRect.bottom = (*iLine)->m_ShapePoints[i].y;

				bRectInitialized = true;
			}else
			{
				m_NetworkRect.Expand((*iLine)->m_ShapePoints[i]);
			}
		}

	}



	if(theApp.m_VisulizationTemplate == e_train_scheduling)
	{

		CString str;
		str.Format("%d nodes and %d links are loaded.", m_NodeSet.size(), m_LinkSet.size());
		AfxMessageBox(str,MB_ICONINFORMATION);
	}


	IdentifyBottleNeckAndOnOffRamps();
}


bool CTLiteDoc::WriteSelectAgentDataToCSVFile(LPCTSTR lpszFileName, std::vector<DTAAgent*> AgentVector)
{

	// Agent_id,  origin_zone_id, destination_zone_id, departure_time,
	//	arrival_time, complete_flag, trip_time, demand_type, occupancy, information_type,
	//	value_of_time, path_min_cost,distance_in_mile, number_of_nodes,
	//	node id, node arrival time

	ofstream AgentDataFile;
	AgentDataFile.open (lpszFileName, ios::out);
	if (AgentDataFile.is_open())
	{
		AgentDataFile.width(12);
		AgentDataFile.precision(4) ;
		AgentDataFile.setf(ios::fixed);
		AgentDataFile << "Agent_id,origin_zone_id,destination_zone_id,departure_time, trip_time, complete_flag, demand_type,pricing_type,Agent_type, information_class,value_of_time, toll_dollar_cost,emissions,distance_in_mile, number_of_nodes, <node id;node arrival time>" << endl;

		for(int v = 0; v < AgentVector.size(); v++)
		{

			DTAAgent* pAgent = AgentVector[v] ;

			if(pAgent!=NULL)
			{
				AgentDataFile << pAgent->m_AgentID << ",";
				AgentDataFile << pAgent->m_FromZoneID << ",";
				AgentDataFile << pAgent->m_ToZoneID << ",";
				AgentDataFile << pAgent->m_DepartureTime << ",";
				AgentDataFile << pAgent->m_TripTime << ",";
				if(pAgent->m_bComplete == 0)
					AgentDataFile << "0,";
				else
					AgentDataFile << "1,";

				AgentDataFile << pAgent->m_DemandType << ",";
				AgentDataFile << pAgent->m_Distance << ",";
				AgentDataFile << pAgent->m_NodeSize << ",\"";

				//for(int i=0; i< pAgent->m_NodeSize-1; i++)
				//{
				//	AgentDataFile << "<"; 

				//	if(pAgent->m_NodeAry[i].LinkNo!=-1 && m_LinkNoMap.find (pAgent->m_NodeAry[i].LinkNo) != m_LinkNoMap.end())
				//	{
				//	DTALink* pLink = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];
				//
				//	AgentDataFile << pLink->m_FromNodeNumber << ";";
				//	AgentDataFile << pAgent->m_NodeAry[i].ArrivalTimeOnDSN << ">";
				//	}
				//}
				//AgentDataFile << pAgent->m_NodeSize << "\"";
				AgentDataFile << endl;
			}
		}
		AgentDataFile.close();
		return true;
	}

	return false;
}
void CTLiteDoc::ReadAgentCSVFile_Parser(LPCTSTR lpszFileName)
{

	//   cout << "Read Agent file... "  << endl;
	// Agent_id,  origin_zone_id, destination_zone_id, departure_time,
	//	arrival_time, complete_flag, trip_time, demand_type, occupancy, information_type,
	//	value_of_time, path_min_cost,distance_in_mile, number_of_nodes,
	//	node id, node arrival time

	float LengthinMB;
	FILE* pFile;
	fopen_s(&pFile,lpszFileName,"rb");
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		int Length = ftell(pFile);
		fclose(pFile);
		LengthinMB= Length*1.0/1024/1024;
		if(LengthinMB>50)
		{
			CString msg;
			msg.Format("The %s file is %5.1f MB in size.\nIt could take quite a while to load this file.\nWould you like to load the Agent file?",lpszFileName,LengthinMB);
			if(AfxMessageBox(msg,MB_YESNO|MB_ICONINFORMATION)==IDNO)
				return;
		}
	}

	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{
		m_AgentSet.clear();
		int count = 0;
		while(parser.ReadRecord())
		{


			int m_AgentID= -1;
			parser.GetValueByFieldName("agent_id",m_AgentID);
			if(m_AgentID == -1)
				break;

			DTAAgent* pAgent = 0;
			pAgent = new DTAAgent;
			pAgent->m_AgentID		= m_AgentID;
			parser.GetValueByFieldName("o_zone_id",pAgent->m_FromZoneID);
			parser.GetValueByFieldName("d_zone_id",pAgent->m_ToZoneID);
			parser.GetValueByFieldName("d_node_id", pAgent->m_FromNodeID);
			parser.GetValueByFieldName("d_node_id", pAgent->m_ToNodeID);

			pAgent->m_bComplete = true;
			pAgent->m_DepartureTime = 0;

			string time_period;
			vector<float> timestamp_vector;
			parser.GetValueByFieldName("time_period", time_period);

			timestamp_vector = g_time_parser(time_period);
				
			if(timestamp_vector.size() >=1)
				pAgent->m_DepartureTime = timestamp_vector[0];

			if (timestamp_vector.size() == 2)
				pAgent->m_ArrivalTime = timestamp_vector[1];

			if(g_Simulation_Time_Horizon < pAgent->m_ArrivalTime)
				g_Simulation_Time_Horizon = pAgent->m_ArrivalTime;

			parser.GetValueByFieldName("travel_time",pAgent->m_TripTime );

			float distance_in_mile = 0;

			parser.GetValueByFieldName("distance",pAgent->m_Distance  );

			parser.GetValueByFieldName("demand_type",pAgent->m_DemandType );
			
			pAgent->m_Distance = distance_in_mile;

			std::string path_node_sequence, path_time_sequence, path_state_sequence;
			parser.GetValueByFieldName("node_sequence",path_node_sequence );
			parser.GetValueByFieldName("time_sequence",path_time_sequence );

			std::vector<int> node_sequence;
			std::vector<float> time_sequence;
			std::vector<string> state_sequence;

			g_ParserIntSequence(path_node_sequence, node_sequence);

			pAgent->m_NodeSize = node_sequence.size();
			g_ParserFloatSequence(path_time_sequence, time_sequence);
			

			std::string node_sequence_node_timestamp;
			std::vector<float> node_time_sequence;


			if(pAgent->m_NodeSize>=1)  // in case reading error
			{
				pAgent->m_NodeAry = new SAgentLink[pAgent->m_NodeSize];

				pAgent->m_NodeNumberSum = 0;
				for(int i=0; i< pAgent->m_NodeSize; i++)
				{
					m_PathNodeVectorSP[i] = node_sequence[i];
					pAgent->m_NodeNumberSum += m_PathNodeVectorSP[i];
					if(i>=1)
					{
						DTALink* pLink = FindLinkWithNodeNumbers(m_PathNodeVectorSP[i-1],m_PathNodeVectorSP[i]);
						if(pLink==NULL)
						{

							CString str;
							str.Format("path node sequence error for trip_id %d: link %d ->%d does not exist. Please check.", 
							m_AgentID, m_PathNodeVectorSP[i-1],m_PathNodeVectorSP[i]  );

							continue;
						}
						pAgent->m_NodeAry[i].LinkNo  = pLink->m_LinkNo ;
						pLink->m_TotalTravelTime +=  pAgent->m_NodeAry[i].ArrivalTimeOnDSN - pAgent->m_NodeAry[i-1].ArrivalTimeOnDSN;
					
				
					}

					// random error beyond 6 seconds for better ainimation

					
					float random_value = g_RNNOF()*0.01; // 0.1 min = 6 seconds


					if(time_sequence.size()>0)
					{
					pAgent->m_NodeAry[i].ArrivalTimeOnDSN = time_sequence[i]+random_value;
					}
				}

				m_AgentSet.push_back (pAgent);
				m_AgentIDMap[pAgent->m_AgentID]  = pAgent;


				count++;
			} 
		}

		UpdateMovementDataFromAgentTrajector();

		m_SimulationAgentDataLoadingStatus.Format ("%d agents are loaded from file %s.",count,lpszFileName);

	}
}




void CTLiteDoc::ScanAMSTimingPlanCSVFile(LPCTSTR lpszFileName, int scenario_no = 0)
{

	//reset
	m_TimingPlanVector.clear();

	AddNameIntoTimingPlanVector(m_DemandLoadingStartTimeInMin , m_DemandLoadingEndTimeInMin,"0");

	CCSVParser parser;

	int count = 0;
	if (!parser.OpenCSVFile(lpszFileName))  //no such file exists.
	{
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

		CString DefaultDataFolder;

		DefaultDataFolder.Format ("%s\\default_data_folder\\",pMainFrame->m_CurrentDirectory);

		//CopyDefaultFile(DefaultDataFolder,m_ProjectDirectory,m_ProjectDirectory,"AMS_timing_plan.csv","AMS_timing_plan.csv");

	}

	parser.CloseCSVFile ();

	if (parser.OpenCSVFile(lpszFileName))
	{
		while(parser.ReadRecord())
		{
			int scenario_no;
			int start_day_no;
			int end_day_no;


			if(parser.GetValueByFieldName("scenario_no",scenario_no) == false)
				break;
			if(parser.GetValueByFieldName("start_day_no",start_day_no) == false)
				break;
			if(parser.GetValueByFieldName("end_day_no",end_day_no) == false)
				break;

			for(int time = m_DemandLoadingStartTimeInMin ; time <m_DemandLoadingEndTimeInMin; time+=15)
			{

				CString str;
				str.Format("'%s", GetTimeStampString24HourFormat (time));

				std::string str_time =  CString2StdString(str);


				std::string timing_plan_name;

				parser.GetValueByFieldName(str_time,timing_plan_name);

				if(timing_plan_name.size () > 0 && timing_plan_name != "0")
				{
					AddNameIntoTimingPlanVector(time, time+15,timing_plan_name);
				}

			}


		}

	}


}
int CTLiteDoc::ReadAMSMovementCSVFile(LPCTSTR lpszFileName, int NodeID = -1)
{

	int number_of_nodes = 0;

	m_MovementPointerMap.clear();

	std::list<DTALink*>::iterator iLink;

	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

				for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap.m_MovementVector .size(); m++)
				{

					DTANodeMovement movement = (*iNode)->m_MovementDataMap.m_MovementVector[m];

					CString label;
					string ib_link_id = m_LinkNoMap[movement.IncomingLinkNo]->m_LinkID;
					string ob_link_id = m_LinkNoMap[movement.OutgoingLinkNo]->m_LinkID;
					label.Format("%s;%d;%s", ib_link_id.c_str (),(*iNode)->m_NodeID, ob_link_id.c_str());

					m_MovementPointerMap[label] = &((*iNode)->m_MovementDataMap.m_MovementVector[m]); // store pointer

					// set default value for through turns

					DTALink* pLink = m_LinkNoMap[movement.IncomingLinkNo];
					if(pLink !=NULL)
					{

						switch ((*iNode)->m_MovementDataMap.m_MovementVector[m].movement_turn)
						{
						case DTA_Through: (*iNode)->m_MovementDataMap.m_MovementVector[m].QEM_Lanes = pLink->m_NumberOfLanes; break;
						case DTA_LeftTurn: 
						case DTA_LeftTurn2:
							(*iNode)->m_MovementDataMap.m_MovementVector[m].QEM_Lanes = max((*iNode)->m_MovementDataMap.m_MovementVector[m].QEM_Lanes,
								pLink->m_NumberOfLeftTurnLanes) ; break;


						case DTA_RightTurn:
						case DTA_RightTurn2: 
							(*iNode)->m_MovementDataMap.m_MovementVector[m].QEM_Lanes = max( pLink->m_NumberOfRightTurnLanes,
								(*iNode)->m_MovementDataMap.m_MovementVector[m].QEM_Lanes); break;
						}

						(*iNode)->m_MovementDataMap.m_MovementVector[m].QEM_Speed  = pLink->m_FreeSpeed ;
					}else
					{
						return 0;
					}


		}
	}

	CCSVParser parser_movement;

	int count = 0;

	int current_node_id = -1;
	if (parser_movement.OpenCSVFile(lpszFileName))
	{
		while(parser_movement.ReadRecord())
		{
			string ib_link_id;
			int node_id = 0;
			string ob_link_id;


			if(parser_movement.GetValueByFieldName("node_id",node_id) == false)
				break;

			if(current_node_id != node_id)
			{
				current_node_id = node_id;
				number_of_nodes++;
			}


			parser_movement.GetValueByFieldName("ib_link_id", ib_link_id);
			parser_movement.GetValueByFieldName("ob_link_id", ob_link_id);


			DTANode* pNode = FindNodeWithNodeNumber (node_id);


			if(m_LinkIDtoLinkMap.find(ib_link_id)== m_LinkIDtoLinkMap.end()
				|| m_NodeNumbertoNodeNoMap.find(node_id)== m_NodeNumbertoNodeNoMap.end() 
				|| m_LinkIDtoLinkMap.find(ob_link_id)== m_LinkIDtoLinkMap.end() )
			{
			continue;
			
			}
			CString movement_index = pNode->GetMovementIndex(m_LinkIDtoLinkMap[ib_link_id]->m_FromNodeID ,
				m_NodeNumbertoNodeNoMap[node_id], m_LinkIDtoLinkMap[ob_link_id]->m_ToNodeID);


			CString label;
			label.Format("%s;%d;%s", ib_link_id.c_str(),node_id, ob_link_id.c_str());

			if(m_MovementPointerMap.find(label) != m_MovementPointerMap.end())
			{
				DTANodeMovement* pMovement = m_MovementPointerMap[label];

				parser_movement.GetValueByFieldName ("prohibited_flag",pMovement->turning_prohibition_flag);
				parser_movement.GetValueByFieldName ("protected_flag",pMovement->turning_protected_flag );
				parser_movement.GetValueByFieldName ("permitted_flag",pMovement->turning_permitted_flag);

				std::string turn_type;

				std::string QEM_dir_string;
				parser_movement.GetValueByFieldName ("turn_type",turn_type );
				parser_movement.GetValueByFieldName ("turn_direction", QEM_dir_string);

				pMovement->movement_approach_turn  = GetTurnDirectionFromString(QEM_dir_string.c_str ());

			count++;

			}else
			{
				// if the movement has not been defined in the construction part, ignore the data. 
			}
		}
	}




	m_MovementDataLoadingStatus.Format ("%d movements are loaded from file %s.",count,lpszFileName);
	return number_of_nodes;

}

int CTLiteDoc::ReadAMSSignalControlCSVFile(LPCTSTR lpszFileName)
{

	CCSVParser parser_signal;

	if (parser_signal.OpenCSVFile(lpszFileName, true))  // not required
	{
		int up_node_id = 0;
		int dest_node_id = 0;

		while (parser_signal.ReadRecord())
		{
			int node_id = -1;
			int phase_id = -1;
			int next_phase_id = -1;
			int green_duration = -1;
			std::string timing_plan_name, movement_str, movement_dir_str;

			parser_signal.GetValueByFieldName("int_id", node_id);

			DTANode* pNode = FindNodeWithNodeNumber(node_id);

			if (pNode != NULL)
			{
				pNode->m_withSignalPlans = true;
				parser_signal.GetValueByFieldName("timing_plan_no", timing_plan_name);
				parser_signal.GetValueByFieldName("movement_str", movement_str);
				parser_signal.GetValueByFieldName("green_duration", green_duration);
				parser_signal.GetValueByFieldName("next_phase_id", next_phase_id,false);
				parser_signal.GetValueByFieldName("phase_id", phase_id);

				SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_GreenDuration, green_duration);
				SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_NextPhaseNo, next_phase_id);
				SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_MOVEMENT_VECTOR, movement_str);

				parser_signal.GetValueByFieldName("movement_dir_str", movement_dir_str);
				SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_MOVEMENT_DIR_VECTOR, movement_dir_str);


				if (movement_str.size() == 0 && movement_dir_str.size() > 0)
				{
					for (unsigned int m = 0; m < pNode->m_MovementDataMap.m_MovementVector.size(); m++)
					{
						bool bMovementIncluded = IfMovementDirIncludedInPhase(node_id,
							timing_plan_name, phase_id, m);

						if (bMovementIncluded)
						{

							CString sub_movement_str;

							sub_movement_str.Format(";%d_%d_%s", m_NodeNoMap[pNode->m_MovementDataMap.m_MovementVector[m].in_link_from_node_id]->m_NodeID,
								m_NodeNoMap[pNode->m_MovementDataMap.m_MovementVector[m].out_link_to_node_id]->m_NodeID,
								GetTurnShortString(pNode->m_MovementDataMap.m_MovementVector[m].movement_turn));
							//select
							movement_str += sub_movement_str;
						}

					}

					SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_MOVEMENT_VECTOR, movement_str);

				}

			}
		}
	}


	/// auto generate the phasing data
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		DTANode* pNode = (*iNode);
		if (pNode != NULL && pNode->m_withSignalPlans!=true && pNode->m_ControlType == 5)
		{
			int node_id = pNode->m_NodeID;
			int phase_id = -1;
			int next_phase_id = -1;
			int green_duration = 30;

			for (unsigned int m = 0; m < pNode->m_MovementDataMap.m_MovementVector.size(); m++)
			{
				int NEMAPhase_no = 0;
				NEMAPhase_no = GetNEMAPhase_from_TurnDirectionString(pNode->m_MovementDataMap.m_MovementVector[m].movement_approach_turn);
				if (NEMAPhase_no > 0)
				{
					if(NEMAPhase_no%2==1) // left turn
						green_duration = 10;
					else {
						 green_duration = 30;

						}


					phase_id = NEMAPhase_no;

					std::string timing_plan_name = "1";
					std::string movement_str, movement_dir_str;
					movement_dir_str += ";";
					SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_GreenDuration, green_duration);
					SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_NextPhaseNo, next_phase_id);
					SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_MOVEMENT_VECTOR, movement_str);
					movement_dir_str += GetTurnDirectionString(pNode->m_MovementDataMap.m_MovementVector[m].movement_approach_turn);

					SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_MOVEMENT_DIR_VECTOR, movement_dir_str);
					CString sub_movement_str;
					sub_movement_str.Format(";%d_%d_%s", m_NodeNoMap[pNode->m_MovementDataMap.m_MovementVector[m].in_link_from_node_id]->m_NodeID,
						m_NodeNoMap[pNode->m_MovementDataMap.m_MovementVector[m].out_link_to_node_id]->m_NodeID,
						GetTurnShortString(pNode->m_MovementDataMap.m_MovementVector[m].movement_turn));
					//select
						movement_str += sub_movement_str;

					SetupPhaseData(node_id, timing_plan_name, phase_id, PHASE_MOVEMENT_VECTOR, movement_str);
				}
			}

		}
	}
	return true;
}

bool CTLiteDoc::ReadAgentBinFile(LPCTSTR lpszFileName, int version_number = 2)
{
	//   cout << "Read Agent file... "  << endl;
	// Agent_id,  origin_zone_id, destination_zone_id, departure_time,
	//	arrival_time, complete_flag, trip_time, demand_type, occupancy, information_type,
	//	value_of_time, path_min_cost,distance_in_mile, number_of_nodes,
	//	node id, node arrival time

	bool bCreateMovementMap = true;
	float LengthinMB;
	FILE* pFile;
	fopen_s(&pFile,lpszFileName,"rb");
	
	int Length = 0;
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		Length = ftell(pFile);
		fclose(pFile);
		LengthinMB= Length*1.0/1024/1024;
		if(LengthinMB>50)
		{
			CString msg;
			msg.Format("The %s file is %5.1f MB in size.\nIt could take quite a while to load this file.\nWould you like to load the file?", lpszFileName, LengthinMB);
			if(AfxMessageBox(msg,MB_YESNO|MB_ICONINFORMATION)==IDNO)
			{
				fclose(pFile);
				return true;
			}
		}

		//if(LengthinMB>20)  // if the file size is greater then 20 MB, ask the question
		//{
		//	CString msg;

		//	msg.Format("Do you want to create the movement mapping table (necessary for Synchro exporting function)?",LengthinMB);
		//	if(AfxMessageBox(msg,MB_YESNO|MB_ICONINFORMATION)==IDNO)
		//		bCreateMovementMap = false;
		//}
		fclose(pFile);
	}

	CString SettingsFile;
	SettingsFile.Format ("%sDTASettings.txt",m_ProjectDirectory);


	typedef struct  
	{
		int Agent_id;
		int from_zone_id;
		int to_zone_id;
		float departure_time;
		float arrival_time;
		int complete_flag;
		float trip_time;
		int demand_type;
		int Agent_type;
		int information_type;
		float value_of_time;
		float toll_cost_in_dollar;
		float PM;
		float distance_in_mile;
		int number_of_nodes;

	} struct_Agent_Header_version_1;

	typedef struct  
	{
		int age;
		int version_no;

		int day_no;	
		float PM2_5;
		int number_of_VMS_response_links;


	} struct_Agent_Header_extension_version_2;

	typedef  struct  
	{
		int NodeName;
		float AbsArrivalTimeOnDSN;
	} struct_Agent_Node;


	bool bLoadedMovement = false;
	if (m_LinkSet.size() <= _MAX_LINK_FOR_LOAD_MOVEMENT_DATA && m_bLoadMovementData == true)
		bLoadedMovement = true;


	FILE* st = NULL;
	fopen_s(&st,lpszFileName,"rb");
	if(st!=NULL)
	{
		m_AgentSet.clear();

		if (bLoadedMovement)
		{
			m_Movement3NodeMap.clear();
		}

		struct_Agent_Header_version_1 header;

		struct_Agent_Header_extension_version_2 header_extension;

		CString str_loading;
		int count =0;


		while(!feof(st))
		{

			if (count % 10000 == 0)
			{
				str_loading.Format("loading %dk agents", count/1000);
				SetStatusText(str_loading);
			}

			size_t result;
			size_t result_extension;

			result = fread(&header,sizeof(struct_Agent_Header_version_1),1,st);
			if(version_number == 2)
				result_extension = fread(&header_extension,sizeof(struct_Agent_Header_extension_version_2),1,st);

			if( header.Agent_id < 0)
				break;

			if(result!=1)  // read end of file
				break;

			DTAAgent* pAgent = 0;

			pAgent = new DTAAgent;

			header_extension.day_no =0;

			pAgent->m_DayNo  = header_extension.day_no;

			g_SimulatedDayDataMap[ header_extension.day_no] = true;
			g_SimulatedLastDayNo= max(g_SimulatedLastDayNo, pAgent->m_DayNo);
			g_SimulationDayNo = g_SimulatedLastDayNo;

			
			pAgent->m_AgentID		= header.Agent_id;

			// TRACE("Agent ID = %d\n", header.Agent_id);

			pAgent->m_FromZoneID	= header.from_zone_id;
			pAgent->m_ToZoneID= header.to_zone_id;
			pAgent->m_DepartureTime	=  header.departure_time;
			pAgent->m_ArrivalTime =  header.arrival_time;

			if(g_Simulation_Time_Horizon < pAgent->m_ArrivalTime)
				g_Simulation_Time_Horizon = pAgent->m_ArrivalTime;

			if(header.complete_flag==0) 
				pAgent->m_bComplete = false;
			else
				pAgent->m_bComplete = true;

			pAgent->m_TripTime  = header.trip_time;

			pAgent->m_DemandType = header.demand_type;
		
			pAgent->m_Distance = header.distance_in_mile;
			pAgent->m_NodeSize	= header.number_of_nodes;

			if (pAgent->m_NodeSize >= MAX_NODE_SIZE_IN_A_PATH)
			{
				AfxMessageBox("Too many nodes in a path.");
				return true;
			
			}
			if(version_number == 2)
			{
				pAgent->m_Age = header_extension.age;
			}

			if(pAgent->m_NodeSize>=1)  // in case reading error
			{
				pAgent->m_NodeAry = new SAgentLink[pAgent->m_NodeSize];

//				TRACE("Node size = %d", pAgent->m_NodeSize);

				pAgent->m_NodeNumberSum = 0;
				for(int i=0; i< pAgent->m_NodeSize; i++)
				{


					struct_Agent_Node node_element;
					fread(&node_element,sizeof(node_element),1,st);

					m_PathNodeVectorSP[i] = node_element.NodeName;
					pAgent->m_NodeNumberSum += m_PathNodeVectorSP[i];
					if (i >= 1)
					{
						DTALink* pLink = FastFindLinkWithNodeNumbers(m_PathNodeVectorSP[i - 1], m_PathNodeVectorSP[i]);
						if (pLink != NULL)
						{
							//	AfxMessageBox("Error in reading file agent.bin");




							pAgent->m_NodeAry[i].LinkNo = pLink->m_LinkNo;

							// random error beyond 6 seconds for better ainimation

							float random_value = g_RNNOF()*0.01; // 0.1 min = 6 seconds

							pAgent->m_NodeAry[i].ArrivalTimeOnDSN = node_element.AbsArrivalTimeOnDSN;

							pLink->m_total_assigned_link_volume += 1;

							if (pAgent->m_NodeAry[i].ArrivalTimeOnDSN < 10000) //feasible arrival time
							{


								float travel_time = 0;

								if (i >= 2)
									travel_time = pAgent->m_NodeAry[i].ArrivalTimeOnDSN - pAgent->m_NodeAry[i - 1].ArrivalTimeOnDSN;
								else // first link
									travel_time = pAgent->m_NodeAry[i].ArrivalTimeOnDSN - pAgent->m_DepartureTime;


								if (travel_time - pLink->m_FreeFlowTravelTime > 100)
									TRACE("");


								if (travel_time >= 100)
								{
									TRACE("");

								}
								pLink->m_TotalTravelTime += travel_time;

								float delay = travel_time - pLink->m_FreeFlowTravelTime;
								pLink->m_total_delay += delay;
							}
							else
							{
								//TRACE("");

								//TRACE("infeasible link; %f, %d\n", pLink->m_total_link_volume, pAgent->m_AgentID);

							}


							pLink->m_total_link_volume_of_incomplete_trips = pLink->m_total_link_volume - pLink->m_total_assigned_link_volume;

						}
						else
						{  // missing link
							
							fclose(st);
							return false;
						}
					}


				}

				// movement count


				m_AgentSet.push_back (pAgent);
				m_AgentIDMap[pAgent->m_AgentID]  = pAgent;


				count++;
			} 
		}

		if (bLoadedMovement)
		{
			UpdateMovementDataFromAgentTrajector();
		}
		fclose(st);
		m_SimulationAgentDataLoadingStatus.Format ("%d Agents are loaded from file %s.",count,lpszFileName);
		return true;

	}
	return false;
}

bool CTLiteDoc::ReadAgentCSVFile(LPCTSTR lpszFileName, int version_number = 2)
{
	CCSVParser parser_agent;

	float total_number_of_Agents_to_be_generated = 0;

	if (parser_agent.OpenCSVFile(lpszFileName))
	{
		int count = 0;

		int line_no = 1;

		int i = 0;
		while (parser_agent.ReadRecord())
		{

			string agent_id = "0";

			parser_agent.GetValueByFieldName("agent_id", agent_id);
			DTAAgent* pAgent = 0;


			pAgent = new DTAAgent;

			//pAgent->m_AgentID = agent_id;
			pAgent->m_RandomSeed = pAgent->m_AgentID;

			parser_agent.GetValueByFieldName("from_zone_id", pAgent->m_FromZoneID);
			parser_agent.GetValueByFieldName("to_zone_id", pAgent->m_ToZoneID);

			int origin_node_id = -1;
			int origin_node_number = -1;


			std::vector<CCoordinate> CoordinateVector;
			string geo_string;

			//if (parser_agent.GetValueByFieldName("geometry", geo_string) == false)
			//{
			//	// overwrite when the field "geometry" exists
			//	CGeometry geometry(geo_string);
			//	CoordinateVector = geometry.GetCoordinateList();
			//	for (int si = 0; si < CoordinateVector.size(); si++)
			//	{
			//		GDPoint	pt;
			//		pt.x = CoordinateVector[si].X;
			//		pt.y = CoordinateVector[si].Y;
			//		pAgent->m_ShapePoints.push_back(pt);

			//	}

			//}


			parser_agent.GetValueByFieldName("origin_node_id", origin_node_number);

			if (m_NodeNoMap.find(origin_node_number) != m_NodeNoMap.end())  // convert node number to internal node id
			{
				origin_node_id = m_NodeNoMap[origin_node_number]->m_NodeNo;
			}

			int destination_node_id = -1;
			int destination_node_number = -1;
			parser_agent.GetValueByFieldName("destination_node_id", destination_node_number);

			if (m_NodeNoMap.find(destination_node_number) != m_NodeNoMap.end()) // convert node number to internal node id
			{
				destination_node_id = m_NodeNoMap[destination_node_number]->m_NodeNo;
			}

			pAgent->m_FromNodeID = origin_node_id;
			pAgent->m_ToNodeID = destination_node_id;


			//if (origin_node_id == destination_node_id)
			//{  // do not simulate intra zone traffic
			//	continue;
			//}

			if (m_ZoneMap.find(pAgent->m_FromZoneID) != m_ZoneMap.end())
			{
				m_ZoneMap[pAgent->m_FromZoneID].m_Demand += 1;
				m_ZoneMap[pAgent->m_FromZoneID].m_OriginAgentSize += 1;

			}

			float departure_time = 0;
			parser_agent.GetValueByFieldName("departure_time", departure_time);

			pAgent->m_DepartureTime = departure_time;
			int beginning_departure_time = departure_time;


			parser_agent.GetValueByFieldName("demand_type", pAgent->m_DemandType);

			int day_no;
			parser_agent.GetValueByFieldName("day_no", day_no);
			std::string agent_type;
			parser_agent.GetValueByFieldName("agent_type", agent_type);

			if (agent_type == "p")
				m_PassengerIDStringVector.push_back(agent_id);
			else
				m_AgentIDStringVector.push_back(agent_id);



			int number_of_nodes = 0;
			//	parser_agent.GetValueByFieldName("number_of_nodes", number_of_nodes);

			std::vector<int> path_node_sequence;

			string path_node_sequence_str;
			if (parser_agent.GetValueByFieldName("path_node_sequence", path_node_sequence_str) == true)
			{

				path_node_sequence = ParseLineToIntegers(path_node_sequence_str);
				number_of_nodes = path_node_sequence.size();


			}

			std::vector<float> path_time_sequence;

			string path_time_sequence_str;
			if (parser_agent.GetValueByFieldName("path_time_sequence", path_time_sequence_str) == true)
			{

				path_time_sequence = ParseLineToFloat(path_time_sequence_str);
				int number_of_time_nodes = path_time_sequence_str.size();

			}

			pAgent->m_Distance = 0;

			for (int i = 0; i < path_node_sequence.size(); i++)
			{
				int node_number = path_node_sequence[i];
				if (m_NodeNumbertoNodeNoMap.find(node_number) != m_NodeNumbertoNodeNoMap.end())  // convert node number to internal node id
				{
					int NodeNo = m_NodeNumbertoNodeNoMap[node_number];

					AgentLocationRecord element;
					element.agent_no = count;
						element.agent_id = agent_id;
						element.agent_type = agent_type;

						element.x = m_NodeNoMap[NodeNo]->pt.x;
						element.y = m_NodeNoMap[NodeNo]->pt.y;

						if (i < path_time_sequence.size())  // the same elements in node and time sequences
						{
							element.time_stamp_in_second = path_time_sequence[i] * 60;  //min to seconds
						}
						else //default
						{
							element.time_stamp_in_second = 0;
						}

						element.day_no = day_no;

						if (path_node_sequence.size()>=2 && i < path_node_sequence.size() - 2)
						{
							DTALink* pLink = FindLinkWithNodeNumbers(path_node_sequence[i], path_node_sequence[i + 1]);

							if (pLink != NULL)
								element.link_no = pLink->m_LinkNo;

						}
						AddLocationRecord(element);
				}  //node fount
			}
			count++;

		}
		m_SimulationAgentDataLoadingStatus.Format("%d agent data are loaded from file %s.", count, lpszFileName);
		return true;
	}
}


void  CTLiteDoc::UpdateMovementDataFromAgentTrajector()
{
	// movement count
	std::list<DTANode*>::iterator iNode;

	for(int tp = 0; tp< m_TimingPlanVector.size(); tp++)  // first loop for each timing plan
	{

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			(*iNode)->ResetMovementMOE ();		
		}
	}

	std::list<DTAAgent*>::iterator iAgent;
	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		if(pAgent==NULL)
			break;

		if(pAgent->m_AgentID  == 35)
		{
			TRACE("\nveh = %d",pAgent->m_AgentID );

		}
		if(pAgent ->m_bComplete  == false)
			continue;

		for(int i= 2; i<pAgent->m_NodeSize; i++)
		{

			if(pAgent->m_NodeAry == NULL)
				return;

			int link = pAgent->m_NodeAry[i-1].LinkNo;
			if( m_LinkNoMap.find(link)== m_LinkNoMap.end())
				return;  // error 

			DTALink* pLink0 = m_LinkNoMap[pAgent->m_NodeAry[i-1].LinkNo];
			DTALink* pLink1 = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];

			if(pLink0==NULL || pLink1== NULL)
				break;

			float traveltime = pAgent->m_NodeAry[i-1].ArrivalTimeOnDSN -  max(pAgent->m_DepartureTime , pAgent->m_NodeAry[i-2].ArrivalTimeOnDSN);
			float delay = traveltime - pLink0->m_FreeFlowTravelTime ;

			if(delay <0)
				delay = 0;

			if(traveltime < pLink0->m_FreeFlowTravelTime)
				traveltime = pLink0->m_FreeFlowTravelTime;


			if(pLink0->m_FromNodeNumber == 1 && pLink0->m_ToNodeNumber == 5 && pLink1->m_ToNodeNumber == 3)
			{
				TRACE("delay: %f\n", delay);
			}

			DTANode* pNode = m_NodeNoMap[ pLink0->m_ToNodeID ];

			if(pNode->m_NodeID == 36)
			{
				TRACE("");
			}

			if(pLink0 !=NULL && pLink1!=NULL)
			{
				pLink0->m_TotalVolumeForMovementCount++;
				pLink0->m_total_travel_time +=traveltime;



				//
				////TO DO: fetch timing_plan no according to day no and time period
				//DTATimingPlan timing_plan = FindTimingPlanFromStartTime( pAgent->m_NodeAry[i].ArrivalTimeOnDSN);

				//pNode->AddMovementCountAndDelay(pLink0->m_FromNodeID, pLink1->m_ToNodeID, delay , timing_plan);

				//{
				//	DTATimingPlan timing_plan_all_day;

				//	timing_plan_all_day.timing_plan_name = "0";

				//	timing_plan_all_day.start_time_in_min = m_DemandLoadingStartTimeInMin;
				//	timing_plan_all_day.end_time_in_min = m_DemandLoadingEndTimeInMin;

				//	pNode->AddMovementCountAndDelay(pLink0->m_FromNodeID, pLink1->m_ToNodeID, delay);
				//}

			}

		}
	}

	// turning percentage
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap.m_MovementVector .size(); m++)
		{
			DTANodeMovement* pMovement = &((*iNode)->m_MovementDataMap.m_MovementVector[m]);
			DTALink* pLink0 = m_LinkNoMap[pMovement->IncomingLinkNo  ];

			if(pLink0->m_FromNodeNumber == 1)
			{
				TRACE("");
			}

			int total_link_count = 0;
			for(unsigned int j = 0; j< (*iNode)->m_MovementDataMap.m_MovementVector .size(); j++)
			{

				if((*iNode)->m_MovementDataMap.m_MovementVector[j].IncomingLinkNo == pMovement->IncomingLinkNo )
				{
					total_link_count+= (*iNode)->m_MovementDataMap.m_MovementVector[j].sim_turn_count ;
				}

			}

			if (pMovement->sim_turn_count >=1)
			{
				pMovement->turning_percentage = 
					pMovement->sim_turn_count * 100.0f / max(1,total_link_count);

				pMovement->sim_turn_delay  = 
					pMovement->sim_turn_delay/ max(1,pMovement->sim_turn_count);

			}else
			{
				pMovement->sim_turn_delay  = 0 ;
			}
		}

	}


	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_avg_simulated_speed =  (*iLink)->m_Length / ((*iLink)->m_total_travel_time/max(1,(*iLink)->m_total_link_volume))*60; 

	}

}
int CTLiteDoc::GetVehilePosition(DTAAgent* pAgent, double CurrentTime, int &link_sequence_no, float& ratio)
{
	GDPoint pt;
	pt.x = 0;
	pt.y = 0;

	int search_counter = 0;
	int beg, end, mid;
	beg = 1;
	end = pAgent->m_NodeSize-1;
	mid = (beg+end)/2;
	int i = mid;
	ratio = 0;
	float link_travel_time;
	float traveling_time;

	link_sequence_no = -1;

	while(beg<=end)
	{
		if(CurrentTime >= pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN &&
			CurrentTime <= pAgent->m_NodeAry [i].ArrivalTimeOnDSN )	// find the link between the time interval
		{

			link_travel_time = pAgent->m_NodeAry [i].ArrivalTimeOnDSN - pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN;

			traveling_time = CurrentTime - pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN;

			ratio = traveling_time/link_travel_time;

			if(ratio <0)
				ratio = 0;

			if(ratio >1)
				ratio = 1;

			if(pAgent->m_NodeAry [i].LinkNo  == 457)
				TRACE("");

			link_sequence_no = i;
			return pAgent->m_NodeAry [i].LinkNo ;

			//			pt.x =  ratio*pLink->m_FromPoint .x  +  (1-ratio)*ratio*pLink->m_ToPoint .x;
			//			pt.y =  ratio*pLink->m_FromPoint .y  +  (1-ratio)*ratio*pLink->m_ToPoint .y;

		}else  // not found
		{
			if( CurrentTime>= pAgent->m_NodeAry [i].ArrivalTimeOnDSN)  // time stamp is after i
			{
				// shift to the right
				beg=mid+1;
				mid=(beg+end)/2;
				i = mid;
			}else //CurrentTime < pAgent->m_NodeAry[i].ArrivalTimeOnDSN// time stamp is before i
			{   //shift to the left
				end=mid-1;
				mid=(beg+end)/2;
				i = mid;
			}

			search_counter++;

			if(search_counter > pAgent->m_NodeSize)  // exception handling
			{
				//				ASSERT(false);
				return 0;
			}

		}
	}

	return 0;
}

bool CTLiteDoc::GetAgentPosition(string agent_id, double CurrentTime_in_min, GDPoint& pt)
{
	float ratio = 0;
	double CurrentTime = CurrentTime_in_min * 60;  // in seconds
	pt.x = 0;
	pt.y = 0;

	int search_counter = 0;
	int beg, end, mid;
	beg = 1;
	end = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector.size() - 1;
	mid = (beg + end) / 2;
	int i = mid;
	ratio = 0;
	float link_travel_time;
	float traveling_time;

	while (beg <= end)
	{
		if (CurrentTime >= m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i - 1].time_stamp_in_second &&
			CurrentTime <= m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second)	// find the link between the time interval
		{

			link_travel_time = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second 
				- m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second;

			traveling_time = CurrentTime - m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i - 1].time_stamp_in_second;

			ratio = traveling_time / link_travel_time;

			if (ratio <0)
				ratio = 0;

			if (ratio >1)
				ratio = 1;


			int LinkNo = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].link_no;

			if (LinkNo >= 0)
			{
				DTALink* pLink = m_LinkNoMap[LinkNo];
				pt = pLink->GetRelativePosition(ratio);
				return true;
			}
			else
			{
				
				pt.x = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i - 1].x +

					m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].x;
				return true;

			}
		}
		else  // not found
		{
			if (CurrentTime >= m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second)  // time stamp is after i
			{
				// shift to the right
				beg = mid + 1;
				mid = (beg + end) / 2;
				i = mid;
			}
			else //CurrentTime < pAgent->m_NodeAry[i].ArrivalTimeOnDSN// time stamp is before i
			{   //shift to the left
				end = mid - 1;
				mid = (beg + end) / 2;
				i = mid;
			}

			search_counter++;

			if (search_counter > m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector.size())  // exception handling
			{
				//				ASSERT(false);
				return false;
			}

		}
	}

	return false;
}
bool CTLiteDoc::GetGPSVehilePosition(DTAAgent* pAgent, double CurrentTime, GDPoint& pt)
{

	pt.x = pAgent->m_LocationRecordAry[0].x;
	pt.y = pAgent->m_LocationRecordAry[0].y;

	int search_counter = 0;
	int beg, end, mid;
	beg = 1;
	end = pAgent->m_NodeSize-1;
	mid = (beg+end)/2;
	int i = mid;
	double ratio = 0;
	float link_travel_time;
	float remaining_time;

	while(beg<=end)
	{
		if(CurrentTime >= pAgent->m_LocationRecordAry [i-1].time_stamp_in_min &&
			CurrentTime <= pAgent->m_LocationRecordAry [i].time_stamp_in_min )	// find the link between the time interval
		{

			link_travel_time = pAgent->m_LocationRecordAry [i].time_stamp_in_min - pAgent->m_LocationRecordAry [i-1].time_stamp_in_min;

			remaining_time = CurrentTime - pAgent->m_LocationRecordAry [i-1].time_stamp_in_min;

			ratio = 1-remaining_time/link_travel_time;

			if(ratio <0)
				ratio = 0;

			if(ratio >1)
				ratio = 1;

			pt.x = ratio * pAgent->m_LocationRecordAry [i-1].x + (1- ratio)* pAgent->m_LocationRecordAry [i].x ;
			pt.y = ratio * pAgent->m_LocationRecordAry [i-1].y + (1- ratio)* pAgent->m_LocationRecordAry [i].y ;

			return true ;

		}else  // not found
		{
			if( CurrentTime>= pAgent->m_LocationRecordAry [i].time_stamp_in_min)  // time stamp is after i
			{
				// shift to the right
				beg=mid+1;
				mid=(beg+end)/2;
				i = mid;
			}else //CurrentTime < pAgent->m_NodeAry[i].ArrivalTimeOnDSN// time stamp is before i
			{   //shift to the left
				end=mid-1;
				mid=(beg+end)/2;
				i = mid;
			}

			search_counter++;

			if(search_counter > pAgent->m_NodeSize)  // exception handling
			{
				//				ASSERT(false);
				return false;
			}

		}
	}

	return false;
}

void CTLiteDoc::OnImageImportbackgroundimage()
{

	if(!m_BackgroundBitmapLoaded)
		ReadBackgroundImageFile(m_ProjectFile);

	m_ImageX2  = m_ImageX1+ m_ImageWidth * m_ImageXResolution;
	m_ImageY2  = m_ImageY1+ m_ImageHeight * m_ImageYResolution;

	CDlg_ImageSettings dlg_image;

	dlg_image.m_pDoc = this;

	dlg_image.m_X1 = m_ImageX1;
	dlg_image.m_Y1 = m_ImageY1;

	dlg_image.m_X2 = m_ImageX2;
	dlg_image.m_Y2 = m_ImageY2;

	if(m_BackgroundBitmapLoaded)
	{
		dlg_image.m_Image_File_Message  = "Background bitmap file has been loaded.";

	}
	else 
	{
		dlg_image.m_Image_File_Message  = "Please prepare background bitmap file as\nbackground_image.bmp.";
	}

	if(dlg_image.DoModal ()==IDOK)
	{
		m_ImageX1 = dlg_image.m_X1;
		m_ImageY1 = dlg_image.m_Y1;

		m_ImageX2 = dlg_image.m_X2;
		m_ImageY2 = dlg_image.m_Y2;

		OnFileSaveimagelocation();

	}

	m_bFitNetworkInitialized = false;
	UpdateAllViews(0);
}


void CTLiteDoc::OnFileDataloadingstatus()
{
	CDlgFileLoading dlg;
	dlg.m_pDoc = this;
	dlg.DoModal ();
}

void CTLiteDoc::OnMoeVolume()
{
	m_LinkMOEMode = MOE_volume;
	m_LinkBandWidthMode = LBW_link_volume;
	ShowLegend(true);

	GenerateOffsetLinkBand();
	UpdateAllViews(0);}

void CTLiteDoc::OnMoeSpeed()
{
	m_LinkMOEMode = MOE_speed;
	m_LinkBandWidthMode = LBW_link_volume;

	ShowLegend(true);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnMoeDensity()
{
	m_LinkMOEMode = MOE_density;
	m_LinkBandWidthMode = LBW_link_volume;
	ShowLegend(true);

	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnMoeQueuelength()
{
	m_LinkMOEMode = MOE_queuelength;
	m_LinkBandWidthMode = LBW_number_of_lanes;


	GenerateOffsetLinkBand();
	UpdateAllViews(0);}

void CTLiteDoc::OnMoeFuelconsumption()
{
	m_LinkMOEMode = MOE_fuel;
	m_LinkBandWidthMode = LBW_link_volume;

	UpdateAllViews(0);}

void CTLiteDoc::OnMoeEmissions()
{
	m_LinkMOEMode = MOE_emissions;
	m_LinkBandWidthMode = LBW_link_volume;
	ShowLegend(true);
	UpdateAllViews(0);}

void CTLiteDoc::OnUpdateMoeVolume(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_volume);
}

void CTLiteDoc::OnUpdateMoeSpeed(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_speed);
}

void CTLiteDoc::OnUpdateMoeDensity(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_density);
}

void CTLiteDoc::OnUpdateMoeQueuelength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_queuelength);
}

void CTLiteDoc::OnUpdateMoeFuelconsumption(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_fuel);
}

void CTLiteDoc::OnUpdateMoeEmissions(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_emissions);

}

void CTLiteDoc::OnMoeNone()
{
	m_LinkMOEMode = MOE_none;

	ShowLegend(false);

	// visualization configuration
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeNone(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_none);
}

float CTLiteDoc::GetLinkMOE(DTALink* pLink, Link_MOE LinkMOEMode,int CurrentTime, int AggregationIntervalInMin, float &value)
{

	float power = 0.0f;
	float max_link_volume = 8000.0f;
	float max_speed_ratio = 2.0f; 
	float max_density = 45.0f;

	switch (LinkMOEMode)
	{
		//case MOE_volume:  power = pLink->m_total_link_volume/max_link_volume; 
		//	value = pLink->m_total_link_volume;
		//	break;
	case MOE_speed:   power = pLink->m_avg_simulated_speed/max(1, pLink->m_FreeSpeed) * 100;
		value = pLink->m_avg_simulated_speed;
		break;

	case MOE_volume:   
		value = pLink->m_total_link_volume;
		break;
		

	case MOE_reliability:   power = pLink->m_volume_over_capacity_ratio;
		value = pLink->m_volume_over_capacity_ratio;
		break;

	case MOE_density: power = 0;
		value = 0;
		break;
	case MOE_traveltime:  
		value = pLink->m_StaticTravelTime;
		break;

	case MOE_capacity:  
		value = pLink->m_LaneCapacity * pLink->m_NumberOfLanes ;
		break;

	case MOE_speedlimit:  
		value = pLink->m_FreeSpeed  ;
		break;

	case MOE_fftt: 
		value = pLink->m_FreeFlowTravelTime ;
		break;

	case MOE_length: 
		value = pLink->m_Length  ;
		break;

	case MOE_queue_length:
		value =  0;
		break;


	default: power = 0.0;

	}

	if(m_TrafficFlowModelFlag > 0)
	{
		int StartTime = CurrentTime;
		int EndTime  = CurrentTime + AggregationIntervalInMin;

		float max_power = 0; 
		float min_power = 1;

		if(LinkMOEMode == MOE_reliability)
		{
			CurrentTime = int(CurrentTime/60)*60;  // hour by hour
			AggregationIntervalInMin = 60;
			EndTime  = CurrentTime + AggregationIntervalInMin;

		}
		float total_value = 0;
		int total_measurement_count = 0;

		//		value = pLink->GetWithinDayLinkMOE(LinkMOEMode, m_PrimaryDataSource, CurrentTime);

		for(CurrentTime  = StartTime; CurrentTime < EndTime; CurrentTime ++)
		{


			if(pLink->IsSimulatedDataAvailable (CurrentTime) && pLink->GetSimulatedLinkVolume (CurrentTime)>=1 ||
				pLink->GetSimulatedDensity(CurrentTime) >= 0.1 )
			{

				total_measurement_count++;

				switch (LinkMOEMode)
				{
				case MOE_volume:  

					total_value+= pLink->GetDynamicLinkVolume (CurrentTime,m_PrimaryDataSource);

					break;
				case MOE_speed: 
					total_value+= pLink->GetDynamicSpeed(CurrentTime,m_PrimaryDataSource);
					break;
				case MOE_traveltime:
					total_value+= pLink->GetDynamicTravelTime (CurrentTime);
					break;
				case MOE_density:
					total_value+= pLink->GetSimulatedDensity(CurrentTime);
					break;
				case MOE_queue_length: 
					total_value+= pLink->GetQueueLengthPercentage(CurrentTime);
					break;


				}

			}
		}  // end of for loop

		//update value when there is at one valid data point
		if(total_measurement_count>=1)
		{
			value = total_value/max(1,total_measurement_count);
		}


		switch (LinkMOEMode)
		{
		case MOE_volume:  
			power = value/max_link_volume;
			break;
		case MOE_speed: 
			power =  min(100,value / pLink->m_FreeSpeed*100);
			break;
		case MOE_reliability: 
			power =  value / pLink->m_FreeSpeed;
			break;
		case MOE_traveltime:
			if(value <=0.1)  // no data
				power = 0;
			else 
				power = pLink->m_FreeSpeed / max(1,value)*100; 

			break;
		case MOE_density:
			power = value; 
			break;
		case MOE_queue_length: 
			power = value  ; 
			break;



		}

		if(LinkMOEMode == MOE_speed && CurrentTime >= 1020&& pLink->m_FromNodeNumber == 1354 &&  pLink->m_ToNodeNumber == 756)
		{
			int LOS = GetLOSCode(power);
			TRACE("");
		}

	}

	return power;
}

void CTLiteDoc::OnToolsCarfollowingsimulation()
{
	if(m_SelectedLinkNo==-1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}


}

DWORD CTLiteDoc::ProcessWait(DWORD PID) 
{
	DWORD dwRetVal = DWORD(-1);
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
	if(hProcess) {
		do 
		{
			::Sleep(500);
			::GetExitCodeProcess(hProcess, &dwRetVal);
		} while(dwRetVal == STILL_ACTIVE);
		CloseHandle(hProcess);
	}
	return dwRetVal;
}

DWORD CTLiteDoc::ProcessExecute(CString & strCmd, CString & strArgs,  CString & strDir, BOOL bWait)
{

	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION ProcessInfo;
	DWORD dwRetVal = DWORD(-1);

	/*
	TCHAR szExe[1024];
	if( int(FindExecutable(
	strPath,									// pointer to filename
	strDir.IsEmpty() ? 0 : LPCTSTR(strDir),					// pointer to default directory
	szExe										// result-buffer
	)) <= 32 ) {
	OnExecError(GetLastError(), strCmd);
	return DWORD(-1);
	}
	*/
	BOOL bStat =	CreateProcess(
		strCmd,						// pointer to name of executable module
		0,			// pointer to command line string
		0,						// pointer to process security attributes
		0,						// pointer to thread security attributes
		TRUE,						// handle inheritance flag
		0,						// creation flags
		0,						// pointer to new environment block
		strDir.IsEmpty() ? 0 : LPCTSTR(strDir),
		&StartupInfo,				// pointer to STARTUPINFO
		&ProcessInfo				// pointer to PROCESS_INFORMATION
		);
	if( bStat ) 
	{
		if( bWait ) 
		{
			::WaitForInputIdle(ProcessInfo.hProcess, INFINITE);
			dwRetVal = ProcessWait(ProcessInfo.dwProcessId);
		} else {
			// before we return to the caller, we wait for the currently
			// started application until it is ready to work.
			::WaitForInputIdle(ProcessInfo.hProcess, INFINITE);
			dwRetVal = ProcessInfo.dwProcessId;
		}

		::CloseHandle(ProcessInfo.hThread);
	}

	return dwRetVal;
}


void CTLiteDoc::OnToolsPerformtrafficassignment()
{
	STARTUPINFO si = { 0 };  
	PROCESS_INFORMATION pi = { 0 };  

	si.cb = sizeof(si); 

	CWaitCursor curs;
	if(!SetCurrentDirectory(m_ProjectDirectory))
	{
		if(m_ProjectDirectory.GetLength()==0)
		{
			AfxMessageBox("The project directory has not been specified.");
			return;
		}
		CString msg;
		msg.Format ("The specified project directory %s does not exist.", m_ProjectDirectory);
		AfxMessageBox(msg);
		return;
	}

	if(IsModified())
		SaveProject(m_ProjectFile);


	CString sCommand;
	CString strParam;
	CTime ExeStartTime = CTime::GetCurrentTime();

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();


		sCommand.Format("%s\\%s", pMainFrame->m_CurrentDirectory,theApp.m_SimulatorString_64);

	ProcessExecute(sCommand, strParam, m_ProjectDirectory, true);


	FILE* pFile;

	int OutputAgentFileSize = 0;
	fopen_s(&pFile,m_ProjectDirectory + "\\output_LinkMOE.csv","rb");
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		OutputAgentFileSize = ftell(pFile);
		fclose(pFile);
	}

	CTime ExeEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = ExeEndTime  - ExeStartTime;
	CString str_running_time;

	FILE* st = NULL;

	CString directory = m_ProjectDirectory;

	if (OutputAgentFileSize >= 1 && ts.GetTotalSeconds() >= 1)
	{


		str_running_time.Format("Simulation program execution has completed.\nProgram execution time: %d hour(s) %d min(s) %d sec(s).\nPlease check time-dependent link MOEs during period %s->%s.\nDo you want to load the simulation results now?",

			ts.GetHours(), ts.GetMinutes(), ts.GetSeconds(),
			GetTimeStampString24HourFormat(m_DemandLoadingStartTimeInMin) );

			
				str_running_time.Format("Simulation program execution has completed.\nProgram execution time: %d hour(s) %d min(s) %d sec(s).\nPlease check time-dependent link MOEs during period %s->%s.\nDo you want to view the output summary file now?",

					ts.GetHours(), ts.GetMinutes(), ts.GetSeconds(),
					GetTimeStampString24HourFormat(m_DemandLoadingStartTimeInMin),
					GetTimeStampString24HourFormat(m_DemandLoadingEndTimeInMin));

				if (AfxMessageBox(str_running_time, MB_YESNO | MB_ICONINFORMATION) == IDYES)
				{
					OnToolsViewassignmentsummarylog();
				}
				LoadSimulationOutput();
			
		UpdateAllViews(0);


	}

}



void CTLiteDoc::LoadSimulationOutput()
{

	ReadMetaDemandCSVFile(m_ProjectDirectory + "demand_file_list.csv");
	ReadScenarioSettingCSVFile(m_ProjectDirectory + "input_scenario_settings.csv");

	CString DTASettingsPath = m_ProjectDirectory + "DTASettings.txt";
	g_Simulation_Time_Horizon = 1440;
	SetStatusText("Loading output link time-dependent data");

	CCSVParser parser;

	bool b_link_MOE_data_flag = true; 
	b_link_MOE_data_flag = ReadSimulationLinkMOEData_Parser(m_ProjectDirectory + "link_performance.csv");

	ReadAgentCSVFile_Parser(m_ProjectDirectory+ "agent.csv");
	//RecalculateLinkMOEFromAgentTrajectoryFile();


	int speed_data_aggregation_interval = 15;

	//char microsimulation_file_name[_MAX_STRING_SIZE];
	//g_GetProfileString("microsimulation_data","file_name","",microsimulation_file_name,sizeof(microsimulation_file_name),m_ProjectFile);

	//CString microsimulation_file_str;

	//microsimulation_file_str.Format("%s",microsimulation_file_name);




	
	SetStatusText("Generating OD statistics...");

	ResetODMOEMatrix();
}


void CTLiteDoc::OnMoeVcRatio()
{
	m_LinkMOEMode = MOE_density;
	ShowLegend(true);
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeVcRatio(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_density);
}

void CTLiteDoc::OnMoeTraveltime()
{
	m_LinkMOEMode = MOE_traveltime;

	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeTraveltime(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_traveltime);
}

void CTLiteDoc::OnMoeCapacity()
{
	m_LinkMOEMode = MOE_capacity;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeCapacity(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_capacity);
}

void CTLiteDoc::OnMoeSpeedlimit()
{
	m_LinkMOEMode = MOE_speedlimit;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeSpeedlimit(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_speedlimit);
}

void CTLiteDoc::OnMoeFreeflowtravletime()
{
	m_LinkMOEMode = MOE_fftt;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeFreeflowtravletime(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_fftt);
}


CString CTLiteDoc::GetLocalFileName(CString strFullPath)
{
	int len = strFullPath.GetLength();
	/////////////////////////////////

	CString	strPathTitleName;

	strPathTitleName.Empty ();

	for(int k=len-1;k>=0;k--)
	{
		if(strFullPath[k]=='\\')
			break;

			strPathTitleName+=strFullPath[k];

	}

	strPathTitleName.MakeReverse();

	return strPathTitleName;

}

CString CTLiteDoc::GetWorkspaceTitleName(CString strFullPath)
{
	int len = strFullPath.GetLength();
	/////////////////////////////////

	CString	strPathTitleName;

	strPathTitleName.Empty ();
	bool StartFlag = false;

	for(int k=len-1;k>=0;k--)
	{
		if(strFullPath[k]=='\\')
			break;

		if(strFullPath[k]=='.' && StartFlag == false)
		{
			StartFlag = true;
			continue;
		}

		if(StartFlag == true)
			strPathTitleName+=strFullPath[k];

	}

	strPathTitleName.MakeReverse();

	return strPathTitleName;

}
void CTLiteDoc::OnEditDeleteselectedlink()
{

	// TODO: Add your command handler code here
}


void CTLiteDoc::OnMoeLength()
{
	m_LinkMOEMode = MOE_length;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeLength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_length);
}

void CTLiteDoc::OnEditSetdefaultlinkpropertiesfornewlinks()
{

	
}

void CTLiteDoc::OnUpdateEditSetdefaultlinkpropertiesfornewlinks(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CTLiteDoc::OnToolsProjectfolder()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project directory has not been specified.");
		return;
	}

	ShellExecute( NULL,  "explore", m_ProjectDirectory, NULL,  NULL, SW_SHOWNORMAL );
}

void CTLiteDoc::OnToolsOpennextaprogramfolder()
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ShellExecute( NULL,  "explore", pMainFrame->m_CurrentDirectory, NULL,  NULL, SW_SHOWNORMAL );
}

void CTLiteDoc::OnMoeNoodmoe()
{
	m_ODMOEMode = odnone;
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeNoodmoe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ODMOEMode == MOE_none);
}


void CTLiteDoc::OnOdtableImportOdTripFile()
{

}

void CTLiteDoc::OnToolsEditassignmentsettings()
{
}


void CTLiteDoc::OnSearchLinklist()
{

	g_bShowLinkList = !g_bShowLinkList;

	if(g_bShowLinkList)
	{
		if(g_pLinkListDlg==NULL)
		{
			g_pLinkListDlg = new CDlgLinkList();
			g_pLinkListDlg->Create(IDD_DIALOG_LINK_LIST);
		}

		// update using pointer to the active document; 

		if(g_pLinkListDlg->GetSafeHwnd())
		{
			// we udpate the pointer list for document every time we open this link list window
			std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
			while (iDoc != g_DocumentList.end())
			{
				if ((*iDoc)->m_NodeSet.size()>0)
				{

					if(g_pLinkListDlg->m_pDoc ==NULL)
						g_pLinkListDlg->m_pDoc = (*iDoc);
					else if((*iDoc)!= g_pLinkListDlg->m_pDoc)
					{
						g_pLinkListDlg->m_pDoc2 = (*iDoc);
						g_pLinkListDlg->m_bDoc2Ready = true;
					}

				}
				iDoc++;

			}

			g_pLinkListDlg->ReloadData ();
			g_pLinkListDlg->ShowWindow(SW_HIDE);
			g_pLinkListDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_pLinkListDlg!=NULL && g_pLinkListDlg->GetSafeHwnd())
		{
			g_pLinkListDlg->ShowWindow(SW_HIDE);
		}
	}

}

void CTLiteDoc::OnMoeAgent()
{
	m_LinkMOEMode = MOE_Agent;
	GenerateOffsetLinkBand();
	ShowLegend(false);

	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeAgent(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_Agent);
}
LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

	if (retval == ERROR_SUCCESS) {
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, NULL, data, &datasize);
		lstrcpy(retdata,data);
		RegCloseKey(hkey);
	}

	return retval;
}
HINSTANCE g_OpenDocument(LPCTSTR url, int showcmd)
{
	TCHAR key[MAX_PATH + MAX_PATH];

	// First try ShellExecute()
	HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

	// If it failed, get the .htm regkey and lookup the program
	if ((UINT)result <= HINSTANCE_ERROR) {

		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".csv"), key) == ERROR_SUCCESS) {
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) {			   // No quotes found
					pos = strstr(key, _T("%1"));	   // Check for %1, without quotes
					if (pos == NULL)			   // No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';			 // Remove the parameter
				}
				else
					*pos = '\0';			  // Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);
				result = (HINSTANCE) WinExec(key,showcmd);
			}
		}
	}

	return result;
}

HINSTANCE g_GotoURL(LPCTSTR url, int showcmd)
{
	TCHAR key[MAX_PATH + MAX_PATH];

	// First try ShellExecute()
	HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

	// If it failed, get the .htm regkey and lookup the program
	if ((UINT)result <= HINSTANCE_ERROR) {

		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) {			   // No quotes found
					pos = strstr(key, _T("%1"));	   // Check for %1, without quotes
					if (pos == NULL)			   // No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';			 // Remove the parameter
				}
				else
					*pos = '\0';			  // Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);
				result = (HINSTANCE) WinExec(key,showcmd);
			}
		}
	}

	return result;
}

void CTLiteDoc::OnToolsViewsimulationsummary()
{
	CDlg_AgentClassification* m_pDlg = new CDlg_AgentClassification; 

	g_SummaryDialogVector.push_back (m_pDlg);  // collect memory block
	m_pDlg->m_pDoc = this;
	m_pDlg->SetModelessFlag(true); // voila! this is all it takes to make your dlg modeless!
	m_pDlg->Create(IDD_DIALOG_Summary); 
	m_pDlg->ShowWindow(SW_SHOW); 

	m_bSummaryDialog = true;
}


void CTLiteDoc::OnToolsViewassignmentsummarylog()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_summary.csv");
}

void CTLiteDoc::OnHelpVisitdevelopmentwebsite()
{
	g_OpenDocument("https://github.com/zephyr-data-specs/GMNS/", SW_SHOW);
}

bool CTLiteDoc::CheckControlData()
{
	return true;
	std::vector <int> NodeVector;
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
		{

	
		}
	}

	if(NodeVector.size()>0 && m_signal_reresentation_model >=1)
	{

		CString message;
		message.Format ("There are %d signalized nodes with zero cycle length (e.g. %d).\n Do you want to continue?\nDTALite will use a continuous flow model with link capacity constraints for those nodes.", NodeVector.size(),NodeVector[0]);

		if(AfxMessageBox(message,MB_YESNO|MB_ICONINFORMATION)==IDYES)
		{

			return true;
		}


		return false;
	}

	return true;

}


void CTLiteDoc::OnToolsRuntrafficassignment()
{



}


void CTLiteDoc::OnToolsPerformscheduling()
{
	STARTUPINFO si = { 0 };  
	PROCESS_INFORMATION pi = { 0 };  

	si.cb = sizeof(si); 

	CWaitCursor curs;
	if(!SetCurrentDirectory(m_ProjectDirectory))
	{
		if(m_ProjectDirectory.GetLength()==0)
		{
			AfxMessageBox("The project directory has not been specified.");
			return;
		}
		CString msg;
		msg.Format ("The specified project directory %s does not exist.", m_ProjectDirectory);
		AfxMessageBox(msg);
		return;
	}

	CString sCommand;
	CString strParam;
	CTime ExeStartTime = CTime::GetCurrentTime();

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

	sCommand.Format("%s\\FastTrain.exe", pMainFrame->m_CurrentDirectory);

	ProcessExecute(sCommand, strParam, m_ProjectDirectory, true);

	CTime ExeEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = ExeEndTime  - ExeStartTime;
	CString str_running_time;

	FILE* st = NULL;

	CString directory = m_ProjectDirectory;
	char simulation_short_summary1[200];
	char simulation_short_summary2[200];
	char simulation_short_summary3[200];

	fopen_s(&st,directory+"short_summary.csv","r");
	if(st!=NULL)
	{  
		fgets (simulation_short_summary1, 200 , st);
		fgets (simulation_short_summary2 , 200 , st);
		fgets (simulation_short_summary3, 200 , st);
		fclose(st);
	}

	str_running_time.Format ("Program execution has completed.\nSimulation Statistics: %s\n%s\n%sProgram execution time: %d hour(s) %d min(s) %d sec(s) \nDo you want to load the output now?",
		simulation_short_summary1,simulation_short_summary2, simulation_short_summary3,ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());

	if( AfxMessageBox(str_running_time, MB_YESNO| MB_ICONINFORMATION)==IDYES)
	{
		LoadSimulationOutput();
		UpdateAllViews(0);
	}
}


void CTLiteDoc::ResetBackgroundImageCoordinate()
{
	m_LongLatFlag = true;

	float m_XScale = 1;
	float m_YScale = 1;

	if(fabs(m_PointB_x- m_PointA_x)>0.000001)
	{
		m_XScale = (m_PointB_long - m_PointA_long)/(m_PointB_x- m_PointA_x);
	}else
	{

		AfxMessageBox("Horizontal distance between two points is too short. Please redefine two points.");
		return;
	}

	if(fabs(m_PointB_y- m_PointA_y)>0.000001)
	{
		m_YScale = (m_PointB_lat - m_PointA_lat)/(m_PointB_y- m_PointA_y);
	}else
	{

		AfxMessageBox("Vertical distance between two points is too short. Please redefine two points.");
		return;
	}

	float m_XOrigin = m_PointA_x - m_PointA_long /m_XScale;

	float m_YOrigin = m_PointA_y- m_PointA_lat /m_YScale;


	/////////////////////////////////adjustment


	// adjust node coordinates
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		(*iNode)->pt .x  = ((*iNode)->pt .x - m_XOrigin)*m_XScale;
		(*iNode)->pt .y  = ((*iNode)->pt .y - m_YOrigin)*m_YScale;
	}
	//adjust link cooridnates

	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_FromPoint.x = ((*iLink)->m_FromPoint.x -m_XOrigin)*m_XScale;
		(*iLink)->m_FromPoint.y = ((*iLink)->m_FromPoint.y -m_YOrigin)*m_YScale;

		(*iLink)->m_ToPoint.x = ((*iLink)->m_ToPoint.x -m_XOrigin)*m_XScale;
		(*iLink)->m_ToPoint.y = ((*iLink)->m_ToPoint.y -m_YOrigin)*m_YScale;


		for(unsigned int si = 0; si< (*iLink)->m_Original_ShapePoints.size(); si++)
		{

			(*iLink)->m_Original_ShapePoints[si].x = ((*iLink)->m_Original_ShapePoints[si].x - m_XOrigin)*m_XScale;
			(*iLink)->m_Original_ShapePoints[si].y = ((*iLink)->m_Original_ShapePoints[si].y - m_YOrigin)*m_YScale;

		}

		for(unsigned int si = 0; si< (*iLink)->m_ShapePoints.size(); si++)
		{

			(*iLink)->m_ShapePoints[si].x = ((*iLink)->m_ShapePoints[si].x - m_XOrigin)*m_XScale;
			(*iLink)->m_ShapePoints[si].y = ((*iLink)->m_ShapePoints[si].y - m_YOrigin)*m_YScale;

		}

	}

	// zone layer
	std::map<int, DTAZone>	:: iterator itr;

	for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); ++itr)
	{
		for(unsigned int si = 0; si< itr->second.m_ShapePoints.size(); si++)
		{
			itr->second.m_ShapePoints[si].x = (itr->second.m_ShapePoints[si].x - m_XOrigin)*m_XScale;
			itr->second.m_ShapePoints[si].y = (itr->second.m_ShapePoints[si].y - m_YOrigin)*m_YScale;

		}

	}

	// image layer

	m_ImageX1 = (m_ImageX1- m_XOrigin)*m_XScale;
	m_ImageY1 = (m_ImageY1- m_YOrigin)*m_YScale;
	m_ImageX2 = (m_ImageX2- m_XOrigin)*m_XScale;
	m_ImageY2 =  (m_ImageY2- m_YOrigin)*m_YScale;

	m_ImageWidth = fabs(m_ImageX2 - m_ImageX1);
	m_ImageHeight = fabs(m_ImageY2 - m_ImageY1);

	GDPoint pt1, pt2;
	pt1.x = m_ImageX1;
	pt1.y = m_ImageY1;

	pt2.x = m_ImageX2;
	pt2.y = m_ImageY2;

	// update image width in miles
	m_ImageWidthInMile = max(m_ImageWidthInMile,g_CalculateP2PDistanceInMileFromLatitudeLongitude(pt1,pt2));

	m_ImageMoveSize = m_ImageWidth/2000.0f;

	CString str_result;
	str_result.Format ("The coordinates has been adjusted to long/lat format.\nNEXTA will reload this project to reflect changed coordinates.");
	if(AfxMessageBox(str_result, MB_ICONINFORMATION)==IDOK)
	{
		SaveProject(m_ProjectFile);  // save time-dependent MOE to input_link MOE file

		OnOpenDocument(m_ProjectFile);
	}

	m_bFitNetworkInitialized = false;
	CalculateDrawingRectangle(false);
	UpdateAllViews(0);
}

void CTLiteDoc::OnFileChangecoordinatestolong()
{
	m_LongLatFlag = true;
	CDlgNetworkAlignment  dlg;
	if(dlg.DoModal() ==IDOK)
	{
		if(dlg.m_Node1!=dlg.m_Node2 && m_NodeNumbertoNodeNoMap.find(dlg.m_Node1)!= m_NodeNumbertoNodeNoMap.end() && 
			m_NodeNumbertoNodeNoMap.find(dlg.m_Node2)!= m_NodeNumbertoNodeNoMap.end())
		{

			GDPoint m_Node1OrgPt, m_Node2OrgPt;
			m_Node1OrgPt = m_NodeNoMap[m_NodeNumbertoNodeNoMap[dlg.m_Node1]]->pt;
			m_Node2OrgPt = m_NodeNoMap[m_NodeNumbertoNodeNoMap[dlg.m_Node2]]->pt;

			float m_XScale = 1;
			float m_YScale = 1;

			if(m_Node1OrgPt.x - m_Node2OrgPt.x)
			{
				m_XScale = (dlg.m_NodeX1 - dlg.m_NodeX2)/(m_Node1OrgPt.x - m_Node2OrgPt.x);
			}

			if(m_Node1OrgPt.y - m_Node2OrgPt.y)
			{
				m_YScale = (dlg.m_NodeY1 - dlg.m_NodeY2)/(m_Node1OrgPt.y - m_Node2OrgPt.y);
			}

			float m_XOrigin = m_Node1OrgPt.x - dlg.m_NodeX1 /m_XScale;

			float m_YOrigin = m_Node1OrgPt.y - dlg.m_NodeY1 /m_YScale;

			// adjust node coordinates
			std::list<DTANode*>::iterator iNode;
			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{

				(*iNode)->pt .x  = ((*iNode)->pt .x - m_XOrigin)*m_XScale;
				(*iNode)->pt .y  = ((*iNode)->pt .y - m_YOrigin)*m_YScale;
			}
			//adjust link cooridnates

			std::list<DTALink*>::iterator iLink;

			for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				(*iLink)->m_FromPoint.x = ((*iLink)->m_FromPoint.x -m_XOrigin)*m_XScale;
				(*iLink)->m_FromPoint.y = ((*iLink)->m_FromPoint.y -m_YOrigin)*m_YScale;

				(*iLink)->m_ToPoint.x = ((*iLink)->m_ToPoint.x -m_XOrigin)*m_XScale;
				(*iLink)->m_ToPoint.y = ((*iLink)->m_ToPoint.y -m_YOrigin)*m_YScale;


				for(unsigned int si = 0; si< (*iLink)->m_Original_ShapePoints.size(); si++)
				{

					(*iLink)->m_Original_ShapePoints[si].x = ((*iLink)->m_Original_ShapePoints[si].x - m_XOrigin)*m_XScale;
					(*iLink)->m_Original_ShapePoints[si].y = ((*iLink)->m_Original_ShapePoints[si].y - m_YOrigin)*m_YScale;

				}

				for(unsigned int si = 0; si< (*iLink)->m_ShapePoints.size(); si++)
				{

					(*iLink)->m_ShapePoints[si].x = ((*iLink)->m_ShapePoints[si].x - m_XOrigin)*m_XScale;
					(*iLink)->m_ShapePoints[si].y = ((*iLink)->m_ShapePoints[si].y - m_YOrigin)*m_YScale;

				}

			}

			// zone layer
			std::map<int, DTAZone>	:: iterator itr;

			for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); ++itr)
			{
				for(unsigned int si = 0; si< itr->second.m_ShapePoints.size(); si++)
				{
					itr->second.m_ShapePoints[si].x = (itr->second.m_ShapePoints[si].x - m_XOrigin)*m_XScale;
					itr->second.m_ShapePoints[si].y = (itr->second.m_ShapePoints[si].y - m_YOrigin)*m_YScale;

				}

			}

			// image layer

			m_ImageX1 = (m_ImageX1- m_XOrigin)*m_XScale;
			m_ImageY1 = (m_ImageY1- m_YOrigin)*m_YScale;
			m_ImageX2 = (m_ImageX2- m_XOrigin)*m_XScale;
			m_ImageY2 =  (m_ImageY2- m_YOrigin)*m_YScale;

			m_ImageWidth = fabs(m_ImageX2 - m_ImageX1);
			m_ImageHeight = fabs(m_ImageY2 - m_ImageY1);

			m_ImageMoveSize = m_ImageWidth/2000.0f;

			CString str_result;
			str_result.Format ("The coordinates of %d nodes, %d links and %d zones have been adjusted to long/lat format.\nPleaes save the network to confirm the change.\nYou can use NEXTA_32.exe ->menu->Tools->GIS tools->Export GIS shape files to check the changed network on Google Maps",m_NodeSet.size(),m_LinkSet.size(),m_ZoneMap.size());
			AfxMessageBox(str_result, MB_ICONINFORMATION);
		}


		UpdateAllViews(0);
	}
}

void CTLiteDoc::OpenWarningLogFile(CString directory)
{
	m_NEXTALOGFile.open (directory+"NeXTA.log", ios::out);
	if (m_NEXTALOGFile.is_open())
	{
		m_NEXTALOGFile.width(12);
		m_NEXTALOGFile.precision(3) ;
		m_NEXTALOGFile.setf(ios::fixed);
	}else
	{
		AfxMessageBox("File NeXTA.log cannot be opened, and it might be locked by another program!");
	}
}
void CTLiteDoc::OnToolsExportopmodedistribution()
{

}

void CTLiteDoc::OnToolsEnumeratepath()
{

	CWaitCursor cws;

	int OD_index;

	int O_array[10]={53661,101846,165091,226988,209476,41017,54466,94991,78110,16589};

	int D_array[10]={144304,33737,70979,72725,79930,101989,89676,21233,84885,156041};

	float TravelTime_array[10]={132.3,112.87,65,92.12,124.03,147.83,152.58,82.9,54.63,65.49};

	for(OD_index = 0; OD_index<10; OD_index++)
	{
		m_FromNodeID = m_NodeNumbertoNodeNoMap[O_array[OD_index]];
		m_ToNodeID = m_NodeNumbertoNodeNoMap[D_array[OD_index]];
		float TravelTimeBound  = TravelTime_array[OD_index];
		if(m_FromNodeID>0 && m_ToNodeID>0)
		{
			if(m_pNetwork !=NULL)
			{
				delete m_pNetwork;
				m_pNetwork = NULL;
			}

			m_pNetwork = new DTANetworkForSP(m_NodeSet.size(), m_LinkSet.size(), 1, 1, m_AdjLinkSize);  //  network instance for single processor in multi-thread environment

			m_pNetwork->BuildPhysicalNetwork(&m_NodeSet, &m_LinkSet, m_RandomRoutingCoefficient, false);

			m_pNetwork->GenerateSearchTree (m_FromNodeID,m_ToNodeID,m_NodeSet.size()*5,TravelTimeBound);

			FILE* st = NULL;

			//		CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			//			_T("Path file (*.csv)|*.csv|"));
			//		if(dlg.DoModal() == IDOK)
			//		{
			CString str;
			str.Format ("C:\\path_set_%d.csv",OD_index);
			fopen_s(&st,str,"w");

			int NodeList[1000];

			int PathNo = 0;

			int i;
			for(i = 0; i < m_pNetwork->m_TreeListTail; i++)
			{
				if(m_pNetwork->m_SearchTreeList[i].CurrentNode == m_ToNodeID)
				{
					int nodeindex = 0;
					NodeList[nodeindex++] = m_pNetwork->m_SearchTreeList[i].CurrentNode;
					int Pred = m_pNetwork->m_SearchTreeList[i].PredecessorNode ;

					while(Pred!=0)
					{
						NodeList[nodeindex++] = m_pNetwork->m_SearchTreeList[Pred].CurrentNode;

						Pred = m_pNetwork->m_SearchTreeList[Pred].PredecessorNode ;
					}
					NodeList[nodeindex++] = m_pNetwork->m_SearchTreeList[Pred].CurrentNode;

					fprintf(st,"%d,%d,", PathNo,nodeindex);

					for(int n = nodeindex-1; n>=0; n--)
					{
						fprintf(st,"%d,", m_NodeNotoNumberMap[NodeList[n]]);
					}

					fprintf(st, "\n");
					PathNo++;
				}

			}
			fclose(st);
		}
	}

}

void CTLiteDoc::OnResearchtoolsExporttodtalitesensordataformat()
{
	CWaitCursor wc;
	int max_day = 23;
	FILE* st = NULL;

	for(int day = 0; day < max_day; day++)
	{
		CString str;
		str.Format( "SensorDataDay%03d.csv",day+1);
		fopen_s(&st,str,"w");
		if(st!=NULL)
		{
			std::list<DTALink*>::iterator iLink;

			fprintf(st,"Unix Timestamp (local time),StationID,Total Flow_per_obs_interval,Avg Density,Avg Speed\n");
			for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				if((*iLink)->m_bSensorData )
				{

					for(int t= day*1440; t<(day+1)*1440; t+=5)
					{
						//
						int hour = (t-day*1440)/60;
						int min =  (t-day*1440-hour*60);
						fprintf(st,"07/%02d/2010 %02d:%02d, %d %4.1f, 0, %4.1f\n", day, hour, min, (*iLink)->m_LinkNo+1,
							(*iLink)->m_LinkMOEAry[ t].LinkFlow/12, (*iLink)->m_LinkMOEAry[t].Speed);
					}


				}
			}

			fclose(st);

		}

	}


}
void CTLiteDoc::OnScenarioConfiguration()
{

}

void CTLiteDoc::OnMoeViewmoes()
{
	// TODO: Add your command handler code here

	CDlgMOETabView dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CTLiteDoc::OnImportdataImportExcelFile()
{

}

void CTLiteDoc::OnImportdataImport()
{

}


void CTLiteDoc::AdjustCoordinateUnitToMile()
{

	m_AdjLinkSize +=2;  // add two more elements to be safe

	std::list<DTANode*>::iterator iNode;
	//adjust XY coordinates if the corrdinate system is not consistenty
	if(fabs(m_UnitDistance-1.00)>0.10)  // ask users if we need to adjust the XY coordinates
	{
		if(AfxMessageBox("The link length information inroad_ink.csv is not consistent with the X/Y coordinates in node.csv.\n Do you want to adjust the the X/Y coordinate unit to mile in node.csv?", MB_YESNO) == IDYES)

			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{
				(*iNode)->pt.x = ((*iNode)->pt.x - min(m_NetworkRect.left,m_NetworkRect.right))*m_UnitDistance;
				(*iNode)->pt.y = ((*iNode)->pt.y - min(m_NetworkRect.top, m_NetworkRect.bottom))*m_UnitDistance;
			}

			std::list<DTALink*>::iterator iLink;
			for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				(*iLink)->m_FromPoint = m_NodeNoMap[(*iLink)->m_FromNodeID]->pt;
				(*iLink)->m_ToPoint = m_NodeNoMap[(*iLink)->m_ToNodeID]->pt;
			}

			//if(m_LongLatCoordinateFlag)
			//	m_UnitDistance = m_UnitDistance/62/1000.0f;  // 62 is 1 long = 62 miles

			CalculateDrawingRectangle();
			UpdateAllViews(0);

	}
}



void CTLiteDoc::OnMoeAgentpathanalaysis()
{
	if(g_bValidDocumentChanged && g_pAgentPathDlg!=NULL)  // either a new document is added or an old document is delete. 
	{
		//	delete g_pAgentPathDlg; potential memory leak, but operator delete is problematic for gridlist
		g_pAgentPathDlg= NULL;
		g_bValidDocumentChanged = false;
	}

	g_bShowAgentPathDialog = !g_bShowAgentPathDialog;

	if(g_bShowAgentPathDialog)
	{
		if(g_pAgentPathDlg==NULL)
		{
			g_pAgentPathDlg = new CDlg_VehPathAnalysis();
			g_pAgentPathDlg->m_pDoc = this;
			g_pAgentPathDlg->Create(IDD_DIALOG_Agent_PATH);
		}

		// update using pointer to the active document; 

		if(g_pAgentPathDlg->GetSafeHwnd())
		{
			// we udpate the pointer list for document every time we open this link list window
			g_pAgentPathDlg->m_pDoc = this;
			g_pAgentPathDlg->ShowWindow(SW_HIDE);
			g_pAgentPathDlg->ShowWindow(SW_SHOW);
			g_pAgentPathDlg->FilterOriginDestinationPairs();
		}
	}else
	{
		if(g_pAgentPathDlg!=NULL && g_pAgentPathDlg->GetSafeHwnd())
		{
			g_pAgentPathDlg->ShowWindow(SW_HIDE);
		}
	}

}

void CTLiteDoc::HighlightPath(	std::vector<int>	m_LinkVector, int DisplayID = 1)
{
	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_DisplayLinkID = -1;
	}

	for (unsigned int l = 0; l < m_LinkVector.size(); l++)
	{
		DTALink* pLink = FindLinkWithLinkNo (m_LinkVector[l]);
		if(pLink!=NULL)
		{
			pLink->m_DisplayLinkID = DisplayID;
		}

	}
	UpdateAllViews(0);
}



void CTLiteDoc::HighlightSelectedAgents(bool bSelectionFlag)
{
	CWaitCursor wc;

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_DisplayLinkID = -1;
		(*iLink)->m_NumberOfMarkedAgents++;
	}

	if(bSelectionFlag) // selection
	{
		std::list<DTAAgent*>::iterator iAgent;

		bool bTraceFlag = true;

		for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
		{
			DTAAgent* pAgent = (*iAgent);
			if(pAgent->m_bMarked)
			{
				for(int link= 1; link<pAgent->m_NodeSize; link++)
				{
					if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())
					{
						m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo ]->m_DisplayLinkID = 1;
						m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo ]->m_NumberOfMarkedAgents++;
					}
				}
			}		
		}
	} // de-selection: do nothing
	UpdateAllViews(0);

}





void CTLiteDoc::OnLinkUserDefinedMOE()
{

}

void CTLiteDoc::OnUpdateUserDefinedMOE(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_user_defined);
}

void CTLiteDoc::OnImportAgentFile()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::OnImportNgsimFile()
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->OnShowTimetable();

}

void CTLiteDoc::OpenCSVFileInExcel(CString filename)
{


}

bool CTLiteDoc::WriteSubareaFiles()
{
	
	return true;
}






int CTLiteDoc::ReadWorkZoneScenarioData(int RemoveLinkFromNodeNumber, int RemoveLinkToNodeNumber)
{

	int i = 0;

	CString workzone_file = m_ProjectDirectory+"Scenario_Work_Zone.csv";
	int error_count  = 0; 
	FILE* st =NULL;
	fopen_s(&st,workzone_file,"r");
	if(st!=NULL)
	{
		while(true)
		{
			int usn  = g_read_integer(st);
			if(usn == -1)
				break;

			int dsn =  g_read_integer(st);

			DTALink* plink = FindLinkWithNodeNumbers(usn,dsn,workzone_file );

			if(plink!=NULL)
			{
				CapacityReduction cs;
				cs.bWorkzone  = true; 

				cs.ScenarioNo =  g_read_integer(st); 
				cs.StartDayNo  = g_read_integer(st);
				cs.EndDayNo	   = g_read_integer(st);
				cs.StartTime = g_read_float(st);
				cs.EndTime = g_read_float(st);
				cs.LaneClosurePercentage= g_read_float(st);
				cs.SpeedLimit = g_read_float(st);

				if(RemoveLinkFromNodeNumber==usn && RemoveLinkToNodeNumber== dsn)
				{
					//skip
				}else
				{
					plink->CapacityReductionVector.push_back(cs);
					i++;
				}
			}else
			{
				error_count ++;
				if(error_count >= 3) break; // no more than 3 warnings

			}
		}
		fclose(st);
	}

	return i;
}


void CTLiteDoc::OnEditOffsetlinks()
{
	m_bLinkToBeShifted = true;
	OffsetLink();
	UpdateAllViews(0);
}



void CTLiteDoc::ChangeNetworkCoordinates(int LayerNo, float XScale, float YScale, float delta_x, float delta_y)
{
	GDRect NetworkRect;

	bool bRectInitialized = false;


		for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if ((*iNode)->m_Connections > 0)   // for selected layer only
			{
				if (!bRectInitialized)
				{
					NetworkRect.left = (*iNode)->pt.x;
					NetworkRect.right = (*iNode)->pt.x;
					NetworkRect.top = (*iNode)->pt.y;
					NetworkRect.bottom = (*iNode)->pt.y;
					bRectInitialized = true;
				}

				NetworkRect.Expand((*iNode)->pt);
			}

		}

		float m_XOrigin = NetworkRect.Center().x;

		float m_YOrigin = NetworkRect.Center().y;

		// adjust node coordinates
		std::list<DTANode*>::iterator iNode;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			if ((*iNode)->m_LayerNo == LayerNo)  // for selected layer only
			{
				(*iNode)->pt.x = ((*iNode)->pt.x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iNode)->pt.y = ((*iNode)->pt.y - m_YOrigin)*YScale + m_YOrigin + delta_y;
			}
		}

		//adjust link cooridnates

		std::list<DTALink*>::iterator iLink;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_LayerNo == LayerNo)   // for selected layer only
			{

				(*iLink)->m_FromPoint.x = ((*iLink)->m_FromPoint.x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iLink)->m_FromPoint.y = ((*iLink)->m_FromPoint.y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				(*iLink)->m_ToPoint.x = ((*iLink)->m_ToPoint.x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iLink)->m_ToPoint.y = ((*iLink)->m_ToPoint.y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				for (unsigned int si = 0; si < (*iLink)->m_ShapePoints.size(); si++)
				{

					(*iLink)->m_ShapePoints[si].x = ((*iLink)->m_ShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
					(*iLink)->m_ShapePoints[si].y = ((*iLink)->m_ShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				}

				for (unsigned int si = 0; si < (*iLink)->m_BandLeftShapePoints.size(); si++)
				{

					(*iLink)->m_BandLeftShapePoints[si].x = ((*iLink)->m_BandLeftShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
					(*iLink)->m_BandLeftShapePoints[si].y = ((*iLink)->m_BandLeftShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				}

				for (unsigned int si = 0; si < (*iLink)->m_BandRightShapePoints.size(); si++)
				{

					(*iLink)->m_BandRightShapePoints[si].x = ((*iLink)->m_BandRightShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
					(*iLink)->m_BandRightShapePoints[si].y = ((*iLink)->m_BandRightShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				}


			}


		}
	if (LayerNo == 1) //reference layer
	{
		std::list<DTALine*>::iterator iLine;
		for (iLine = m_DTALineSet.begin(); iLine != m_DTALineSet.end(); iLine++)
		{

			for (unsigned int si = 0; si < (*iLine)->m_ShapePoints.size(); si++)
			{

				(*iLine)->m_ShapePoints[si].x = ((*iLine)->m_ShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iLine)->m_ShapePoints[si].y = ((*iLine)->m_ShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

			}


		}

	}

		
}
void CTLiteDoc::OnFileOpenNetworkOnly()
{
	static char BASED_CODE szFilter[] = "NEXTA Data Files (*.dws;*.tnp)|*.dws; *.tnp|Transportation Network Projects (*.tnp)|*.tnp|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if(dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;

		CString ProjectFileName = dlg.GetPathName ();

		if(ProjectFileName.Find("tnp")>=0)  //Transportation network project format
		{
			OnOpenTrafficNetworkDocument(ProjectFileName,true);
		}
		CDlgFileLoading dlg;
		dlg.m_pDoc = this;
		dlg.DoModal ();

		UpdateAllViews(0);

	}

}

void CTLiteDoc::OnLinkAddlink()
{
	if(m_SelectedLinkNo==-1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}

	DTALink* pLink= m_LinkNoMap [m_SelectedLinkNo];
	if(pLink!=NULL)
	{

		UpdateAllViews(0);
	}
}


CString CTLiteDoc::GetTimeStampStrFromIntervalNo(int time_interval, bool with_single_quote)
{
	CString str;
	int hour = time_interval/4;
	int min = (time_interval - hour*4)*15;

	if(with_single_quote)
	{
		if(hour<10)
			str.Format ("'0%d:%02d",hour,min);
		else
			str.Format ("'%2d:%02d",hour,min);
	}else  //without typewriter single quotes
	{
		if(hour<10)
			str.Format ("0%d:%02d",hour,min);
		else
			str.Format ("%2d:%02d",hour,min);
	}


	return str;

}


CString CTLiteDoc::GetTimeStampFloatingPointStrFromIntervalNo(int time_interval)
{
	CString str;
	int hour = time_interval/4;
	int min = (time_interval - hour*4)*15;

	str.Format ("%dh%02d",hour,min);

	return str;

}

CString CTLiteDoc::GetTimeStampString(int time_stamp_in_min)
{
	CString str;
	int hour = time_stamp_in_min/60;
	int min = time_stamp_in_min - hour*60;

	if(hour<12)
		str.Format ("%02d:%02d AM",hour,min);
	else if(hour==12)
		str.Format ("%02d:%02d PM",12,min);
	else
		str.Format ("%02d:%02d PM",hour-12,min);

	return str;

}


int CTLiteDoc::GetTimeStampfromString(CString str)
{

	int time_stamp_in_min = 0;
	//char m_Text[100];

	//sscanf_s(m_Text, "%d,%s", &LinkType, LinkTypeString, sizeof(LinkTypeString));
	return time_stamp_in_min;

}

CString CTLiteDoc::GetTimeStampString24HourFormat(int time_stamp_in_min)
{
	CString str;
	int hour = time_stamp_in_min/60;
	int min = time_stamp_in_min - hour*60;

	str.Format ("%2d:%02d",hour,min);

	return str;

}
void CTLiteDoc::OnProjectEdittime()
{
}

int CTLiteDoc::FindClassificationNo(DTAAgent* pAgent, Agent_X_CLASSIFICATION x_classfication)
{

	int index = -1;  // no classification

	switch(x_classfication)
	{
	case CLS_all_Agents: index =0 ; break;
	case CLS_time_interval_5_min: index = pAgent->m_DepartureTime / 5;

		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 5;

		break;
	case CLS_time_interval_15_min: index = pAgent->m_DepartureTime /15; 
		
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 15;

		break;
	case CLS_time_interval_30_min: index = pAgent->m_DepartureTime /30; 
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 30;
		break;
	case CLS_time_interval_60_min: index = pAgent->m_DepartureTime /60; 
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 60;
		break;
	case CLS_time_interval_2_hour: index = pAgent->m_DepartureTime /120;
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 120;
		break;
	case CLS_time_interval_4_hour: index = pAgent->m_DepartureTime /240; 
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 240;
		break;

	case CLS_distance_bin_0_2: index = pAgent->m_Distance /0.2; break;
	case CLS_distance_bin_1: index = pAgent->m_Distance /1; break;

	case CLS_distance_bin_2: index = pAgent->m_Distance /2; break;
	case CLS_distance_bin_5: index = pAgent->m_Distance /5; break;
	case CLS_distance_bin_10: index = pAgent->m_Distance /10; break;
	case CLS_travel_time_bin_2: index = pAgent->m_TripTime /2; break;
	case CLS_travel_time_bin_5: index = pAgent->m_TripTime /5; break;
	case CLS_travel_time_bin_10: index = pAgent->m_TripTime /10; break;
	case CLS_travel_time_bin_30: index = pAgent->m_TripTime /30; break;
	default: 
		TRACE("ERROR. No classification available!");
	};

	return index;
}


CString CTLiteDoc::FindClassificationLabel(Agent_X_CLASSIFICATION x_classfication, int index)
{

	CString label;

	switch(x_classfication)
	{
	case CLS_demand_type: 
		label = GetDemandTypeStr(index);
		break;

	case CLS_time_interval_5_min:
		label.Format("%s-%s", GetTimeStampStrFromIntervalNo(index, false), GetTimeStampStrFromIntervalNo((index + 1), false));
		break;
	case CLS_time_interval_15_min: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index,false),GetTimeStampStrFromIntervalNo((index+1),false));
		break;

	case CLS_time_interval_30_min: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*2,false),GetTimeStampStrFromIntervalNo((index+1)*2,false));
		break;

	case CLS_time_interval_60_min: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*4,false),GetTimeStampStrFromIntervalNo((index+1)*4,false));
		break;

	case CLS_time_interval_2_hour: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*8,false),GetTimeStampStrFromIntervalNo((index+1)*8,false));
		break;

	case CLS_time_interval_4_hour: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*16,false),GetTimeStampStrFromIntervalNo((index+1)*16,false));
		break;

	case CLS_distance_bin_0_2: 
		label.Format ("%.1f-%.1f",index*0.2,(index+1)*0.2);
		break;
	case CLS_distance_bin_1: 
		label.Format ("%d-%d",index*1,(index+1)*1);
		break;
	case CLS_distance_bin_2: 
		label.Format ("%d-%d",index*2,(index+1)*2);
		break;

	case CLS_distance_bin_5: 
		label.Format ("%d-%d",index*5,(index+1)*5);
		break;
	case CLS_distance_bin_10: 
		label.Format ("%d-%d",index*10,(index+1)*10);
		break;
	case CLS_travel_time_bin_2: 
		label.Format ("%d-%d",index*2,(index+1)*2);
		break;
	case CLS_travel_time_bin_5: 
		label.Format ("%d-%d",index*5,(index+1)*5);
		break;
	case CLS_travel_time_bin_10: 
		label.Format ("%d-%d",index*10,(index+1)*10);
		break;
	case CLS_travel_time_bin_30: 
		label.Format ("%d-%d",index*30,(index+1)*30);
		break;

	default: 
		TRACE("ERROR. No classification available!");
	};

	return label;
}

bool CTLiteDoc::SelectAgentForAnalysis(DTAAgent* pAgent, Agent_CLASSIFICATION_SELECTION Agent_selection)
{
	if(!pAgent->m_bComplete)
		return false;

	if(Agent_selection == CLS_network)
		return  true;  // all the Agents

	if(Agent_selection == CLS_OD)
		return  pAgent->m_bODMarked;  // marked by Agent path dialog


	if(Agent_selection == CLS_link_set)
	{
		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{

			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_DisplayLinkID>=0)  // in one of selected links
			{
				return true;		
			}
		}
		return false;
	}

	if(Agent_selection == CLS_path_trip)
	{
		if(m_PathDisplayList.size() == 0)
			return false;

		if(m_SelectPathNo >= m_PathDisplayList.size())
			return false;

		int count_of_links_in_selected_path = 0;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedBySelectedPath )
			{
				count_of_links_in_selected_path++;	
			}
		}

		if(count_of_links_in_selected_path == m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() && m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() >0)
			return true;
		else
			return false;
	}

	if(Agent_selection == CLS_path_partial_trip)
	{
		if(m_PathDisplayList.size() == 0)
			return false;

		if(m_SelectPathNo >= m_PathDisplayList.size())
			return false;

		int count_of_links_in_selected_path = 0;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedBySelectedPath )
			{
				count_of_links_in_selected_path++;	
			}
		}

		if(count_of_links_in_selected_path == m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() && m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() >0)
		{
			//update subtrip travel time

			float subtrip_distance = 0;
			float subtrip_free_flow_travel_time = 0;
			bool b_distance_counting_flag = false;
			for(int link= 1; link<pAgent->m_NodeSize; link++)
			{

				if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bFirstPathLink  )
				{
					pAgent->m_path_start_node_departure_time = pAgent->m_NodeAry[link-1].ArrivalTimeOnDSN;
					b_distance_counting_flag = true;
				}

				if(b_distance_counting_flag)  // count from the first link to the last link
				{
					subtrip_distance += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo] ->m_Length ;
					subtrip_free_flow_travel_time += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo] ->m_FreeFlowTravelTime  ;
				}


				if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bLastPathLink  )
				{
					pAgent->m_path_end_node_arrival_time  = pAgent->m_NodeAry[link].ArrivalTimeOnDSN;

					pAgent->m_path_travel_time = pAgent->m_path_end_node_arrival_time - pAgent->m_path_start_node_departure_time;

					b_distance_counting_flag = false;

					break;

				}


			}

			pAgent->m_path_distance  = subtrip_distance;
			pAgent->m_path_free_flow_travel_time = subtrip_free_flow_travel_time;

			return true;
		}
		else
			return false;
	}

	if(Agent_selection == CLS_subarea_generated)
	{
		// if the first link of a Agent is marked, then return true

		if(pAgent->m_NodeSize>2)
		{

			for(int link= 1; link< pAgent->m_NodeSize; link++)
			{

				if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())  // first  valide link
				{
					if(m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedinSubarea == true)
						return true;
					else
						return false;
				}

			}

		}
		return false;
	}

	if(Agent_selection == CLS_subarea_traversing_through)
	{

		if(pAgent->m_NodeSize<3)
			return false;
		// now pAgent->m_NodeSize>=3;

		// condition 1: if the first link of a Agent is marked (in the subarea), then return false
		if(m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end())
		{
			if( m_LinkNoMap[pAgent->m_NodeAry[1].LinkNo]->m_bIncludedinSubarea == true)
				return false;
		}

		// now the first link is not in subarea
		// condition 2: a link is in subarea
		bool bPassingSubareaFlag = false;
		for(int link= 2; link<pAgent->m_NodeSize-1; link++)
		{

			if( m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedinSubarea )
			{
				bPassingSubareaFlag = true;	
				break;
			}
		}

		if(bPassingSubareaFlag == false)
			return false;
		// now there is a link is in subarea
		// condition 3: test if the last link is out of subarea

		int last_link  = pAgent->m_NodeSize-1;
		if(m_LinkNoMap.find(pAgent->m_NodeAry[last_link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[last_link].LinkNo]->m_bIncludedinSubarea == false)
			return true;  // last link is outside of the subarea area, yes!
		else
			return false;

	}

	if(Agent_selection == CLS_subarea_internal_to_external)
	{
		if(pAgent->m_NodeSize<2)
			return false;
		// now pAgent->m_NodeSize>=2;

		// condition 1: if the first link of a Agent is not marked (not in the subarea), then return false
		if( m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[1].LinkNo]->m_bIncludedinSubarea == false)
			return false;

		// now the first link is in subarea
		// condition 2: test if the last link is out of subarea

		int last_link  = pAgent->m_NodeSize-1;
		if(m_LinkNoMap.find(pAgent->m_NodeAry[last_link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[last_link].LinkNo]->m_bIncludedinSubarea  == false)
			return true;  // outside, yes,
		else
			return false;
	}

	if(Agent_selection == CLS_subarea_external_to_internal)
	{

		if(pAgent->m_NodeSize<2)
			return false;
		// now pAgent->m_NodeSize>=2;

		// condition 1: if the first link of a Agent is marked (in the subarea), then return false
		if(m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[1].LinkNo]->m_bIncludedinSubarea == true)
			return false;

		// now the first link is not in the subarea
		// condition 2: test if the last link is in subarea

		int last_link  = pAgent->m_NodeSize-1;
		if( m_LinkNoMap.find(pAgent->m_NodeAry[last_link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[last_link].LinkNo]->m_bIncludedinSubarea  == true)
			return true;  // inside, yes!
		else
			return false;
	}
	if(Agent_selection == CLS_subarea_internal_to_internal_trip)
	{

		if(pAgent->m_NodeSize<2)  
			return false;
		// now pAgent->m_NodeSize>=2;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if( m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedinSubarea == false )
				return false; // this is a link outside of the subarea
		}

		// pass all the tests
		return true;
	}


	if(Agent_selection == CLS_subarea_internal_to_internal_subtrip)
	{

		if(pAgent->m_NodeSize<2)  
			return false;
		// now pAgent->m_NodeSize>=2;

		float subtrip_distance = 0;
		float subtrip_free_flow_travel_time = 0;
		int distance_counting_flag = 0;
		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())
			{
				DTALink *pLink = m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo];
				if(pLink->m_bIncludedinSubarea == true )
				{
					if(distance_counting_flag==0)
					{
						distance_counting_flag = 1;
						pAgent->m_subarea_start_node_departure_time = pAgent->m_NodeAry [link-1].ArrivalTimeOnDSN ;

					}

					if(distance_counting_flag==1)
					{
						subtrip_distance+= m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_Length ;
						subtrip_free_flow_travel_time += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_FreeFlowTravelTime ;


						// keep the last record
						pAgent->m_subarea_distance = subtrip_distance; // for inside path
						pAgent->m_subarea_free_flow_travel_time = subtrip_free_flow_travel_time;
						pAgent->m_subarea_end_node_arrival_time  = pAgent->m_NodeAry [link].ArrivalTimeOnDSN ;
						pAgent->m_subarea_travel_time = pAgent->m_subarea_end_node_arrival_time  - pAgent->m_subarea_start_node_departure_time;

					}


				}else if(distance_counting_flag == 1) 
				{  //leaving subarea

					distance_counting_flag = 2;

					break;
				}

			}
		}

		// pass all the tests
		if(distance_counting_flag>=1)
			return true;
	}
	if(Agent_selection == CLS_subarea_boundary_to_bounary_subtrip)
	{

		if(pAgent->m_NodeSize<2)  
			return false;
		// now pAgent->m_NodeSize>=2;

		float subtrip_distance = 0;
		float subtrip_free_flow_travel_time = 0;
		int distance_counting_flag = 0;
		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if( m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())
			{
				DTALink* pLink = m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo];
				if(pLink->m_bIncludedinSubarea == true )
				{
					if(distance_counting_flag==0)
					{
						distance_counting_flag = 1;
						//record the boundary arrival time
						pAgent->m_subarea_start_node_departure_time = pAgent->m_NodeAry [link-1].ArrivalTimeOnDSN ;

					}

					if(distance_counting_flag==1) // when distance_counting_flag ==0, we will go to this condition right away
					{
						//start counting we  have boundary start time
						subtrip_distance+= m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_Length ;
						subtrip_free_flow_travel_time += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_FreeFlowTravelTime ;

						// keep the last record
						pAgent->m_subarea_distance = subtrip_distance; // for inside path
						pAgent->m_subarea_free_flow_travel_time = subtrip_free_flow_travel_time;
						pAgent->m_subarea_end_node_arrival_time  = pAgent->m_NodeAry [link].ArrivalTimeOnDSN ;
						pAgent->m_subarea_travel_time = pAgent->m_subarea_end_node_arrival_time  - pAgent->m_subarea_start_node_departure_time;

						ASSERT(pAgent->m_subarea_travel_time<900);

					}

				}else if(distance_counting_flag == 1) 
				{  //leaving subarea

					distance_counting_flag = 2;
					break;
				}else
				{

					TRACE("\nAgent %d: Link: %d->%d ", pAgent->m_AgentID , pLink->m_FromNodeNumber , pLink->m_ToNodeNumber );

				}

			}

		}

		// pass all the tests
		if(distance_counting_flag==2)
			return true;
	}
	return false;
}
void CTLiteDoc::MarkLinksInSubarea()
{
	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_bIncludedinSubarea = false;  // reset all the links are not selected by the path
	}

	for (iLink = m_SubareaLinkSet.begin(); iLink != m_SubareaLinkSet.end(); iLink++)
	{
		(*iLink)->m_bIncludedinSubarea = true;
	}
}

void CTLiteDoc::GenerateAgentClassificationData(Agent_CLASSIFICATION_SELECTION Agent_selection, Agent_X_CLASSIFICATION x_classfication)
{
	m_ClassificationTable.clear ();  //reset

	std::list<DTAAgent*>::iterator iAgent;

	//calculate free-flow travel time
	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		if(pAgent->m_FreeflowTripTime <0.00001f)  // freeflow travel time has not been defined yet
		{

			DTAAgent* pAgent = (*iAgent);

			for(int i=1; i< pAgent->m_NodeSize ; i++)
			{
				if(m_LinkNoMap.find(pAgent->m_NodeAry[i].LinkNo)!= m_LinkNoMap.end()) 
				{
					DTALink* pLinkCurrent = m_LinkNoMap[ pAgent->m_NodeAry[i].LinkNo];
					pAgent->m_FreeflowTripTime+= pLinkCurrent->m_FreeFlowTravelTime ;
				}
			}

		}
	}

	bool bTraceFlag = true;

	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		pAgent->m_bMarked = false;
		if(SelectAgentForAnalysis(pAgent, m_AgentSelectionMode) == true)  // belong this classification 
		{
			pAgent->m_bMarked = true;
			if(bTraceFlag)  // trace single Agent id
			{
				TRACE("Agent name: %d\n",pAgent->m_AgentID+1  );
				bTraceFlag = false;
			}
			int index = FindClassificationNo(pAgent,x_classfication);

			if(index>=0)  // -1 will not be consideded
			{
				m_ClassificationTable[index].TotalAgentSize+=1;


				if(Agent_selection== CLS_path_partial_trip)  // subpath
				{
					m_ClassificationTable[index].TotalTravelTime  +=  pAgent->m_path_travel_time;
					m_ClassificationTable[index].TotalFreeflowTravelTime  +=  pAgent->m_path_free_flow_travel_time;

					m_ClassificationTable[index].TotalDistance   += pAgent->m_path_distance;				
					m_ClassificationTable[index].TotalTravelTimePerMile += ( pAgent->m_path_travel_time /max(0.01,pAgent->m_path_distance));

					m_ClassificationTable[index].m_data_vector_travel_time .AddData (pAgent->m_path_travel_time);
					m_ClassificationTable[index].m_data_vector_travel_time_per_mile .AddData ( pAgent->m_path_travel_time /max(0.01,pAgent->m_path_distance));


				}else if (Agent_selection == CLS_subarea_internal_to_internal_subtrip || Agent_selection == CLS_subarea_boundary_to_bounary_subtrip)
				{
					m_ClassificationTable[index].TotalTravelTime  += pAgent->m_subarea_travel_time ;
					m_ClassificationTable[index].TotalFreeflowTravelTime  += pAgent->m_subarea_free_flow_travel_time ;
					m_ClassificationTable[index].TotalDistance   += pAgent->m_subarea_distance;				
					m_ClassificationTable[index].TotalTravelTimePerMile += (pAgent->m_subarea_travel_time)/max(0.01,pAgent->m_subarea_distance);

					m_ClassificationTable[index].m_data_vector_travel_time .AddData (pAgent->m_subarea_travel_time);
					m_ClassificationTable[index].m_data_vector_travel_time_per_mile .AddData (( pAgent->m_subarea_travel_time) /max(0.01,pAgent->m_subarea_distance));
				}
				else  //
				{
					m_ClassificationTable[index].TotalTravelTime  += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
					m_ClassificationTable[index].TotalFreeflowTravelTime  += (pAgent->m_FreeflowTripTime);

					m_ClassificationTable[index].TotalDistance   += pAgent->m_Distance;				
					m_ClassificationTable[index].TotalTravelTimePerMile += ( (pAgent->m_ArrivalTime-pAgent->m_DepartureTime)/pAgent->m_Distance);

					m_ClassificationTable[index].m_data_vector_travel_time .AddData (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
					m_ClassificationTable[index].m_data_vector_travel_time_per_mile .AddData (
						( pAgent->m_ArrivalTime-pAgent->m_DepartureTime) /max(0.01,pAgent->m_Distance));
			}

			}

		}

	}

	//variability
	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);


		if(SelectAgentForAnalysis(pAgent, m_AgentSelectionMode) == true) 
		{
			int index = FindClassificationNo(pAgent,x_classfication);

			if(index>=0)  // -1 will not be consideded
			{
				float deviation = 0;
				float travel_time_per_mile_deviation = 0;


				if(Agent_selection== CLS_path_partial_trip)  //  subpath
				{
					deviation = (pAgent->m_path_travel_time)- 
						(m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize));


					travel_time_per_mile_deviation = 
						(pAgent->m_path_travel_time )/max(0.001,pAgent->m_path_distance  ) -  
						(m_ClassificationTable[index].TotalTravelTimePerMile/max(1,m_ClassificationTable[index].TotalAgentSize));
				}else if(Agent_selection== CLS_subarea_internal_to_internal_subtrip || Agent_selection == CLS_subarea_boundary_to_bounary_subtrip)  // subarea partial path
				{
					deviation = (pAgent->m_subarea_travel_time )- 
						(m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize));


					travel_time_per_mile_deviation = 
						(pAgent->m_subarea_travel_time )/max(0.001,pAgent->m_subarea_distance  ) -  
						(m_ClassificationTable[index].TotalTravelTimePerMile/max(1,m_ClassificationTable[index].TotalAgentSize));

				}else
				{
					deviation = (pAgent->m_ArrivalTime-pAgent->m_DepartureTime)- 
						(m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize));

					travel_time_per_mile_deviation = 
						(pAgent->m_ArrivalTime-pAgent->m_DepartureTime)/max(0.001,pAgent->m_Distance ) -  
						(m_ClassificationTable[index].TotalTravelTimePerMile/max(1,m_ClassificationTable[index].TotalAgentSize));
				}

				m_ClassificationTable[index].TotalTravelTimeVariance   +=  deviation*deviation;
				m_ClassificationTable[index].TotalTravelTimePerMileVariance   +=  travel_time_per_mile_deviation*travel_time_per_mile_deviation;

			}

		}

	}
}


void CTLiteDoc::GenerateClassificationForDisplay(Agent_X_CLASSIFICATION x_classfication, Agent_Y_CLASSIFICATION y_classfication)
{
	// input: 	m_ClassificationTable
	// output: label and display value

	std::map< int, AgentStatistics >::iterator iter;

	for ( iter = m_ClassificationTable.begin(); iter != m_ClassificationTable.end(); iter++ )
	{
		int index = iter->first;
		float value= 0;

		m_ClassificationTable[index].Label = FindClassificationLabel(x_classfication,index);

		// enum Agent_Y_CLASSIFICATION {CLS_Agent_count,CLS_total_travel_time,CLS_avg_travel_time,CLS_total_toll_cost,CLS_avg_toll_cost,CLS_total_generalized_cost,CLS_avg_generalized_cost,CLS_total_travel_distance, CLS_avg_travel_distance,CLS_total_CO2,CLS_avg_CO2};


		switch(y_classfication)
		{
		case CLS_Agent_count: 
			value = m_ClassificationTable[index].TotalAgentSize;
			break;
		case CLS_cumulative_Agent_count: 

			value  = 0;
			for(int ii = 0; ii <= index; ii++)
				if(m_ClassificationTable.find(ii) != m_ClassificationTable.end())
				{
					value += m_ClassificationTable[ii].TotalAgentSize;
				}
				break;

		case CLS_total_travel_time: 
			value = m_ClassificationTable[index].TotalTravelTime ;
			break;
		case CLS_avg_travel_time: 
			value = m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize);
			break;

		case CLS_travel_time_95_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (95);
			break;

		case CLS_travel_time_90_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (90);
			break;

		case CLS_travel_time_80_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (80);
			break;

		case CLS_travel_time_per_mile_95_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time_per_mile .GetDataVectorPercentage (95);
			break;

		case CLS_travel_time_per_mile_90_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time_per_mile .GetDataVectorPercentage (90);
			break;

		case CLS_travel_time_per_mile_80_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time_per_mile .GetDataVectorPercentage (80);
			break;
			//Buffer Index = (95th percentile travel time ?mean travel time) / mean travel time
		case CLS_travel_time_Buffer_Index: 
			value = (m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (95)- m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorMean ())/max(0.01,m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorMean ());
			break;
			//Skew Index = (90th percentile travel time ?median travel time) / (median travel time ?10th percentile travel time)
		case CLS_travel_time_Skew_Index: 
			value = (m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (90)- m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorPercentage(50))/
				max(0.01,m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorPercentage (50) -m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorPercentage (10) );
			break;
		case CLS_travel_time_Travel_Time_Index:  // mean of travel time distribution/free flow travel time for a given path
			value = m_ClassificationTable[index].TotalTravelTime/max(0.01,m_ClassificationTable[index].TotalFreeflowTravelTime);
			break;
		case CLS_travel_time_Planning_Time_Index: //95th percentile of travel time distribution/free flow travel time for a given path
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (95)/max(0.01,m_ClassificationTable[index].TotalFreeflowTravelTime/m_ClassificationTable[index].TotalAgentSize);
			break;

		case CLS_total_travel_distance: 
			value = m_ClassificationTable[index].TotalDistance   ;
			break;
		case CLS_avg_travel_distance: 
			value = m_ClassificationTable[index].TotalDistance /max(1,m_ClassificationTable[index].TotalAgentSize);
			break;
		case CLS_avg_speed: 
			value = m_ClassificationTable[index].TotalDistance /max(0.1,m_ClassificationTable[index].TotalTravelTime/60.0f);
			break;
		case CLS_travel_time_STD: 
			value = sqrt(m_ClassificationTable[index].TotalTravelTimeVariance /max(1,m_ClassificationTable[index].TotalAgentSize));
			break;
		case CLS_travel_time_per_mile_STD: 
			value = sqrt(m_ClassificationTable[index].TotalTravelTimePerMileVariance /max(1,m_ClassificationTable[index].TotalAgentSize));
			break;
		case CLS_avg_travel_time_per_mile: 
			value = m_ClassificationTable[index].TotalTravelTimePerMile /max(1,m_ClassificationTable[index].TotalAgentSize);
			break;


	


		default: 
			value = 0;
			TRACE("ERROR. No classification available!");
		};

		m_ClassificationTable[index].DisplayValue = value;

	}
}


void CTLiteDoc::OnLinkAgentstatisticsanalaysis()
{
	if(m_SelectedLinkNo==-1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}

	m_AgentSelectionMode = CLS_link_set;  // select link analysis

	CDlg_AgentClassification dlg;

	dlg.m_AgentSelectionNo = CLS_link_set;
	dlg.m_pDoc = this;
	dlg.DoModal ();

}



void CTLiteDoc::OnSubareaDeletesubarea()
{
	m_SubareaShapePoints.clear ();
	UpdateAllViews(0);
}

void CTLiteDoc::OnSubareaViewAgentstatisticsassociatedwithsubarea()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::OnToolsTraveltimereliabilityanalysis()
{

	//std::vector<float> LinkCapacity;
	//std::vector<float> LinkTravelTime;

	//LinkCapacity.push_back(1800.0f);
	//LinkTravelTime.push_back(20.0f);

	//CDlg_TravelTimeReliability dlg;
	//dlg.m_pDoc= this;
	//dlg.LinkCapacity = LinkCapacity;
	//dlg.LinkTravelTime = LinkTravelTime;

	//dlg.m_BottleneckIdx = 0;
	//dlg.m_PathFreeFlowTravelTime = 20.0f;  // for a predefined corridor
	//dlg.DoModal ();

	m_LinkMOEMode = MOE_reliability;
	m_LinkBandWidthMode = LBW_link_volume;
	ShowLegend(true);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);

}


void CTLiteDoc::OnLinkLinkbar()
{


}




void CTLiteDoc::OnLinkIncreaseoffsetfortwo()
{
	std::list<DTALink*>::iterator iLink;

	double minimum_link_length = 999999;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink) -> m_bToBeShifted)
		{
			if((*iLink)->m_Length <  minimum_link_length)
				minimum_link_length = (*iLink)->m_Length ;
		}
	}

	double min_offset_value = max(minimum_link_length*0.01, 2);
	m_OffsetInDistance += min_offset_value;
	m_bLinkToBeShifted  = true;
	OffsetLink();  // offset shape points
	GenerateOffsetLinkBand();  // from shape points to reference points
	UpdateAllViews(0);
}

void CTLiteDoc::OnLinkDecreaseoffsetfortwo()
{
	std::list<DTALink*>::iterator iLink;

	double minimum_link_length = 999999;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink) -> m_bToBeShifted)
		{
			if((*iLink)->m_Length <  minimum_link_length)
				minimum_link_length = (*iLink)->m_Length ;
		}
	}

	double min_offset_value = max(minimum_link_length*0.01, 2);
	m_OffsetInDistance -= min_offset_value;

	//if(m_OffsetInDistance<0)
	//	m_OffsetInDistance = 0;


	//	m_OffsetInDistance = max (5,m_OffsetInDistance);  // minimum 5 feet
	m_bLinkToBeShifted  = true;
	OffsetLink();
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnLinkNooffsetandnobandwidth()
{
	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		(*iLink)->m_ShapePoints.clear();

		(*iLink)->m_ShapePoints .push_back ((*iLink)->m_FromPoint);
		(*iLink)->m_ShapePoints .push_back ((*iLink)->m_ToPoint);
	}

	m_OffsetInDistance=10;
	m_bLinkToBeShifted  = true;
	m_LaneWidthInMeter = 2;
	OffsetLink();
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnViewShowhideLegend()
{
	m_bShowLegend = !m_bShowLegend;

	ShowLegend(m_bShowLegend);
}

void CTLiteDoc::ShowLegend(bool ShowLegendStatus)
{
	m_bShowLegend = ShowLegendStatus;
	if(m_bShowLegend)
	{
		if(g_pLegendDlg==NULL)
		{
			g_pLegendDlg = new CDlg_Legend();
			g_pLegendDlg->m_pDoc = this;
			g_pLegendDlg->Create(IDD_DIALOG_Legend);
		}

		// update using pointer to the active document; 

		if(g_pLegendDlg->GetSafeHwnd())
		{
			g_pLegendDlg->m_pDoc = this;
			g_pLegendDlg->ShowWindow(SW_HIDE);
			g_pLegendDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_pLegendDlg!=NULL && g_pLegendDlg->GetSafeHwnd())
		{
			g_pLegendDlg->ShowWindow(SW_HIDE);
		}
	}
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->UpdateLegendView();

}
void CTLiteDoc::OnMoeViewlinkmoesummaryfile()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkMOE.csv");
}

void CTLiteDoc::OnViewCalibrationview()
{
	m_bShowCalibrationResults = !m_bShowCalibrationResults;

	if(m_bShowCalibrationResults == true)
	{
		m_LinkMOEMode = MOE_volume;
		OnMoeViewoddemandestimationsummaryplot();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnUpdateViewCalibrationview(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowCalibrationResults);
}

void CTLiteDoc::OnMoeViewtrafficassignmentsummaryplot()
{
}

void CTLiteDoc::OnMoeViewoddemandestimationsummaryplot()
{

}

void CTLiteDoc::OnMoeViewoddemandestimationsummaryplotLanedensity()
{
}

void CTLiteDoc::OnProjectEditpricingscenariodata()
{

}

void CTLiteDoc::OnLinkViewlink()
{

}

void CTLiteDoc::OnDeleteSelectedLink()
{
	if(m_SelectedLinkNo == -1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}
	DeleteLink(m_SelectedLinkNo);
	m_SelectedLinkNo = -1;
	UpdateAllViews(0);

}


void CTLiteDoc::OnImportRegionalplanninganddtamodels()
{
}

void CTLiteDoc::OnExportGenerateTravelTimeMatrix()
{


}

void CTLiteDoc::OnExportGenerateshapefiles()
{

}

void CTLiteDoc::OnLinkmoedisplayQueuelength()
{
	m_LinkMOEMode = MOE_queue_length;
	m_LinkBandWidthMode = LBW_number_of_lanes;

	ShowLegend(false);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);

}

void CTLiteDoc::OnUpdateLinkmoedisplayQueuelength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_queue_length);
}

void CTLiteDoc::OnUpdateLinkmoeTraveltimereliability(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_reliability);
}



void CTLiteDoc::ZoomToSelectedNode(int SelectedNodeNumber)
{
	if(m_NodeNumbertoNodeNoMap.find(SelectedNodeNumber) != m_NodeNumbertoNodeNoMap.end())
	{
		DTANode* pNode= m_NodeNoMap[m_NodeNumbertoNodeNoMap[SelectedNodeNumber]];
		m_Origin = pNode->pt;
		m_SelectedLinkNo = -1;
		m_SelectedNodeID = pNode->m_NodeNo ;

		CTLiteView* pView = 0;
		POSITION pos = GetFirstViewPosition();
		if(pos != NULL)
		{
			pView = (CTLiteView*) GetNextView(pos);
			if(pView!=NULL)
			{
				pView->m_Origin = m_Origin;
				pView->Invalidate ();
			}
		}

	}
}

void CTLiteDoc::ZoomToSelectedLink(int SelectedLinkNo)
{
	if(m_LinkNoMap.find(SelectedLinkNo) != m_LinkNoMap.end())
	{
		DTALink* pLink= m_LinkNoMap [SelectedLinkNo];
		GDPoint pt;
		pt.x = (pLink->m_FromPoint.x + pLink->m_ToPoint.x)/2;
		pt.y = (pLink->m_FromPoint.y + pLink->m_ToPoint.y)/2;
		m_Origin = pt;

		CTLiteView* pView = 0;
		POSITION pos = GetFirstViewPosition();
		if(pos != NULL)
		{
			pView = (CTLiteView*) GetNextView(pos);
			if(pView!=NULL)
			{
				pView->m_Origin = m_Origin;
				pView->Invalidate ();
			}
		}

	}
}

void CTLiteDoc::ShowPathListDlg(bool bShowFlag)
{
	if(bShowFlag)
	{
		if(g_pPathListDlg==NULL)
		{
			g_pPathListDlg = new CDlgPathList();
			g_pPathListDlg->m_pDoc = this;
			g_pPathListDlg->Create(IDD_DIALOG_PATH_LIST);
		}

		// update using pointer to the active document; 

		if(g_pPathListDlg->GetSafeHwnd())
		{
			g_pPathListDlg->m_pDoc = this;
			g_pPathListDlg->ReloadData ();
			g_pPathListDlg->ShowWindow(SW_HIDE);
			g_pPathListDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_pPathListDlg!=NULL && g_pPathListDlg->GetSafeHwnd())
		{
			g_pPathListDlg->ShowWindow(SW_HIDE);
		}
	}
}

void CTLiteDoc::OnMoePathlist()
{
	//	m_bShowPathList = !m_bShowPathList;
	ShowPathListDlg(m_bShowPathList);
}


bool g_TestValidDocument(CTLiteDoc* pDoc)
{
	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if(pDoc == (*iDoc))
			return true;

		iDoc++;
	}

	return false;
}

void CTLiteDoc::OnViewShowmoe()
{
	g_LinkMOEDlgShowFlag = !g_LinkMOEDlgShowFlag;
	if(g_LinkMOEDlgShowFlag)
	{
		if(g_LinkMOEDlg==NULL)
		{
			g_LinkMOEDlg = new CDlgMOE();

			g_LinkMOEDlg->m_TimeLeft = 0 ;
			g_LinkMOEDlg->m_TimeRight = g_Simulation_Time_Horizon ;
			g_LinkMOEDlg->m_pDoc = this;
			g_LinkMOEDlg->SetModelessFlag(TRUE);
			g_LinkMOEDlg->Create(IDD_DIALOG_MOE);

		}

		if(g_LinkMOEDlg->GetSafeHwnd())
		{
			// assignemnt document pointers
			std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
			while (iDoc != g_DocumentList.end())
			{
				if ((*iDoc)->m_NodeSet.size()>0)
				{
					if(g_LinkMOEDlg->m_pDoc ==NULL)
						g_LinkMOEDlg->m_pDoc = (*iDoc);
					else if((*iDoc)!= g_LinkMOEDlg->m_pDoc)
					{
						g_LinkMOEDlg->m_pDoc2 = (*iDoc);
						g_LinkMOEDlg->m_bDoc2Ready = true;
					}
				}
				iDoc++;
			}
			g_LinkMOEDlg->ShowWindow(SW_HIDE);
			g_LinkMOEDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_LinkMOEDlg!=NULL && g_LinkMOEDlg->GetSafeHwnd())
		{
			g_LinkMOEDlg->ShowWindow(SW_HIDE);
		}
	}

}

void CTLiteDoc::OnUpdateViewShowmoe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_LinkMOEDlgShowFlag);
}

void CTLiteDoc::OnFileUploadlinkdatatogooglefusiontable()
{
	AfxMessageBox("Please first save the data set to a folder, and they you can uploadroad_ink.csv to Goolge Fusion Tables.");
}

void CTLiteDoc::On3Viewdatainexcel()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_ODMOE.csv");
}

void CTLiteDoc::On5Viewdatainexcel()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkTDMOE.csv");
}

void CTLiteDoc::OnMoeViewnetworktimedependentmoe()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
}

void CTLiteDoc::On2Viewdatainexcel33398()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::On2Viewnetworkdata()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
}

void CTLiteDoc::On3Viewoddatainexcel()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_ODMOE.csv");
}

void CTLiteDoc::OnMoeOpenallmoetables()
{


	OpenCSVFileInExcel(m_ProjectDirectory+"output_summary.csv");
	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
	OpenCSVFileInExcel(m_ProjectDirectory+"output_ODMOE.csv");
	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkMOE.csv");



	//	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
	//	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkTDMOE.csv");
}



void CTLiteDoc::OnBnClickedButtonDatabase()
{

}

void CTLiteDoc::OnToolsUnittesting()
{


	// update using pointer to the active document; 


}





void CTLiteDoc::OnToolsGeneratephysicalzonecentroidsonroadnetwork()
{
	CWaitCursor cursor;
	// step 1: mark old centroids, remove old activity locations 

	
	// step 2: create new centriods: find a pair of incoming and outgoing links to centroid, if exist

	std::list<DTALink*>::iterator iLink;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink * pLink = (*iLink);

		if(pLink==NULL)
			break;
		DTALink * pRevLink = NULL; //reversed link
		unsigned long ReversedLinkKey = GetLinkKey(pLink->m_ToNodeID, pLink->m_FromNodeID);

		int reversed_link_id = 0;
		if ( m_NodeNotoLinkMap.find ( ReversedLinkKey) != m_NodeNotoLinkMap.end())
		{
			pRevLink = m_NodeNotoLinkMap[ReversedLinkKey];
		}

		DTANode* pFromNode = m_NodeNoMap[pLink->m_FromNodeID ];
		DTANode* pToNode = m_NodeNoMap[pLink->m_ToNodeID ];

		if(pFromNode->m_NodeID == 220 &&  pToNode->m_NodeID == 57185)
		{
			TRACE("%d ->%d\n ",  pFromNode->m_NodeID ,  pToNode->m_NodeID);
		}

		if(pFromNode->m_CentroidUpdateFlag >=1 && pRevLink != NULL && m_LinkTypeMap[pLink->m_link_type ].IsConnector()) // old centroid and no-external origin node // if it is an external origin node, then we do not need to add nodes
		{
			int zone_number = pFromNode->m_ZoneID ;
			int new_node_number;
			// construct new node number based on from and to node ids
			if(pFromNode->m_NodeNo < pToNode->m_NodeNo)
				new_node_number = pFromNode->m_NodeNo * 10000 + pToNode->m_NodeNo ;  // fromID*10000+ToID
			else
				new_node_number = pToNode->m_NodeNo * 10000 +  pFromNode->m_NodeNo ;  // ToID*10000+fromID

			// add new node and update from_node
			if(m_NodeNumbertoNodeNoMap.find(new_node_number) == m_NodeNumbertoNodeNoMap.end() )
			{
				GDPoint	pt;
				pt.x = (pFromNode->pt.x + pToNode->pt.x) /2;
				pt.y = (pFromNode->pt.y + pToNode->pt.y) /2;

				AddNewNode(pt, new_node_number,false);


				int new_node_id = m_NodeNumbertoNodeNoMap[new_node_number];

				DTANode* pNewNode = m_NodeNoMap [new_node_id ] ;
				m_NodeNotoZoneNameMap[new_node_id] = zone_number;

				pNewNode -> m_ZoneID = zone_number;
				pNewNode ->m_Name == "New_Node";


				//update from node of this link
				pLink->m_FromNodeNumber = new_node_number;
				pLink->m_FromNodeID  = new_node_id;
				pLink->m_FromPoint = pt;
				pLink->m_CentroidUpdateFlag = 1;

				// do not look at the downstream node as centroid again!
				pToNode->m_CentroidUpdateFlag  = 0;

				if( pToNode->m_NodeID == 54317)
					TRACE("");

				pLink->m_ShapePoints .clear();
				pLink->m_ShapePointRatios  .clear();
				pLink->m_ShapePoints.push_back (pLink->m_FromPoint);
				pLink->m_ShapePoints.push_back (pLink->m_ToPoint);

				//update downstream node of the reversed link
				pRevLink->m_CentroidUpdateFlag = 1;
				pRevLink->m_ToNodeNumber = new_node_number;
				pRevLink->m_ToNodeID  = new_node_id;
				pRevLink->m_ToPoint = pt;

				pRevLink->m_ShapePoints .clear();
				pRevLink->m_ShapePointRatios  .clear();
				pRevLink->m_ShapePoints.push_back (pLink->m_ToPoint);
				pRevLink->m_ShapePoints.push_back (pLink->m_FromPoint);

			}  //two-way link  from node

			if( pFromNode->m_NodeID == 54317)
				TRACE("");
			pFromNode->m_CentroidUpdateFlag+=1; // +1 as to be removed, we "touch" this node

		}  // centriod 

	}  // for each link


	// step 2.1: do not delete adjacent nodes of physical links


	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink * pLink = (*iLink);
		if(m_LinkTypeMap[pLink->m_link_type ].IsConnector()== false)  // not a connector, so the ajacent node number will still appear inroad_ink.csv file after generating physical links 
		{

			m_NodeNoMap [pLink->m_FromNodeID ] -> m_CentroidUpdateFlag = 0; // no change
			m_NodeNoMap [pLink->m_ToNodeID ] -> m_CentroidUpdateFlag = 0; // no change


		}

	}
	// step 3: remove old centroids
	std::list<DTANode*>::iterator iNode;
	iNode = m_NodeSet.begin();

	while(iNode != m_NodeSet.end())
	{
		if((*iNode)->m_CentroidUpdateFlag >= 2)
		{
			int ZoneID = (*iNode)->m_ZoneID;

			m_NodeNoMap[(*iNode)->m_NodeNo ] = NULL;
			m_NodeNumbertoNodeNoMap[(*iNode)->m_NodeID  ] = -1;
			iNode = m_NodeSet.erase  (iNode); //  correctly update the iterator to point to the location after the iterator we removed.


		}else
		{
			++iNode;
		}
	}

	// update m_path map 
	m_Movement3NodeMap.clear();

	std::map<CString, PathStatistics> ::iterator itr_path;
	for (itr_path = m_PathMap.begin(); itr_path != m_PathMap.end(); itr_path++)
	{

		// update node numbers
		(*itr_path).second.m_NodeVector.clear();

		int i;
		for(i = 0; i < (*itr_path).second.m_LinkPointerVector.size(); i++)
		{

			DTALink* pLink = (*itr_path).second.m_LinkPointerVector[i];

			if(pLink!=NULL)
			{
				int from_node_number = pLink->m_FromNodeNumber   ;
				int to_node_number = pLink->m_ToNodeNumber ;

				if(i==0) 
				{
					(*itr_path).second.m_NodeVector.push_back(from_node_number);
				}

				(*itr_path).second.m_NodeVector.push_back(to_node_number);
			}


		}


		// update movement counts
		for(i = 0; i < (*itr_path).second.m_NodeVector.size(); i++)
		{
			if(i>=2)
			{
				CString movement_label;
				movement_label.Format ("%d;%d;%d",(*itr_path).second.m_NodeVector[i-2],(*itr_path).second.m_NodeVector[i-1],(*itr_path).second.m_NodeVector[i]);

				m_Movement3NodeMap[movement_label].TotalAgentSize ++;


			}

		}


		// one Agent type for now

	}

	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}



void CTLiteDoc::OnNodeIncreasenodetextsize()
{

	m_NodeTextDisplayRatio *=1.1;

	UpdateAllViews(0);

}

void CTLiteDoc::OnNodeDecreasenodetextsize()
{
	m_NodeTextDisplayRatio /=1.1;

	if(m_NodeTextDisplayRatio <2)  
		m_NodeTextDisplayRatio = 2;

	UpdateAllViews(0);
}



void CTLiteDoc::OnImportSynchroutdfcsvfiles()
{
	ReadSynchroUniversalDataFiles();
	bSynchroImportReadyToSaveFlag = true;

	AfxMessageBox("Please first save and reload the data set to check movement attributes", MB_ICONINFORMATION);

}

void CTLiteDoc::OnProjectEditmoesettings()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"input_MOE_settings.csv");
}

void CTLiteDoc::OnProjectMultiScenarioResults()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_multi_scenario_results.csv");
}

void CTLiteDoc::OnProject12()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_Path.csv");
}

void CTLiteDoc::OnViewMovementMoe()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_MovementMOE.csv");
}

void CTLiteDoc::OnProjectTimeDependentLinkMoe()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"link_performance.csv");
}

void CTLiteDoc::OnViewOdmeResult()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"agent.csv");
}


void CTLiteDoc::OnProjectOdmatrixestimationinput()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"ODME_Settings.txt");
}

void CTLiteDoc::OnProjectInputsensordataforodme()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"sensor_count.csv");
}




void CTLiteDoc::ShowTextLabel()
{
}
void CTLiteDoc::OnLinkattributedisplayLinkname()
{
	m_LinkMOEMode = MOE_none;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateLinkattributedisplayLinkname(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_none);
}


void CTLiteDoc::GenerateMovementCountFromAgentFile(float PeakHourFactor)
{

	CWaitCursor wait;
	m_PeakHourFactor = PeakHourFactor;


	if(m_Movement3NodeMap.size()>=1)
		return;

	if(m_AgentSet.size()>=50000)
	{
		AfxMessageBox("Generating movement count from Agent/agent files might take a while...", MB_ICONINFORMATION);

	}

	std::list<DTAAgent*>::iterator iAgent;

	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		for(int i= 2; i<pAgent->m_NodeSize; i++)
		{

			if(m_LinkNoMap.find(pAgent->m_NodeAry[i-1].LinkNo) != m_LinkNoMap.end()
				&&  m_LinkNoMap.find(pAgent->m_NodeAry[i].LinkNo) != m_LinkNoMap.end())
			{
				DTALink* pLink0 = m_LinkNoMap[pAgent->m_NodeAry[i-1].LinkNo]; // i=0, LinkNo = -1;
				DTALink* pLink1 = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];

				CString movement_label;
				movement_label.Format ("%d;%d;%d", pLink0->m_FromNodeNumber  ,  pLink0->m_ToNodeNumber , pLink1->m_ToNodeNumber);
				m_Movement3NodeMap[movement_label].TotalAgentSize ++;

			}
		}
	}

	FILE* st;

	CString directory;
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	fopen_s(&st,directory+"input_movement_3_node_format.csv","w");
	if(st!=NULL)
	{
		fprintf(st,"movement_index,three-node key,count\n");

		int movement_index = 1;
		std::map<CString, Movement3Node> ::const_iterator itr;
		for (itr = m_Movement3NodeMap.begin(); itr != m_Movement3NodeMap.end(); itr++)
		{
			fprintf(st, "%d,%s,%d\n", 
				movement_index++,
				(*itr).first, 
				(*itr).second.TotalAgentSize );

		}
		fclose(st);
	}else
	{
		AfxMessageBox("File movement.csv cannot be opened.");

	}


}

void CTLiteDoc::OnAssignmentSimulatinSettinsClicked()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}

	
	if(ReadScenarioSettingCSVFile(m_ProjectDirectory+"input_scenario_settings.csv") == false)
	{
		AfxMessageBox("The project does not have scenario data. If this is a data set prepared for traffic assignment/simulation, please save the project first to create scenario files.");
		return;
	
	}


	CAssignmentSimulationSettingDlg dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CTLiteDoc::OnProjectNetworkData()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();

}

void CTLiteDoc::OnLinkAddsensor()
{

	//if(m_SelectedLinkNo==-1)
	//{
	//	AfxMessageBox("Please select a link first.");
	//	return;
	//}

	//DTALink* pLink= m_LinkNoMap [m_SelectedLinkNo];
	//if(pLink!=NULL)
	//{

	//	CNetworkDataSettingDlg dlg;
	//	dlg.m_pDoc = this;
	//	dlg.m_SelectTab = 4; // _SENSOR_DATA
	//	dlg.m_SelectedFromNodeName = pLink->m_FromNodeNumber;
	//	dlg.m_SelectedToNodeName = pLink->m_ToNodeNumber;

	//	if(dlg.DoModal()==IDOK)
	//	{
	//		ReadSensorCountData();
	//		UpdateAllViews(0);
	//	}

	//}
}



void CTLiteDoc::OnMoeTableDialog()
{
	CDlgMOETabView dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();

}

DTALink* CTLiteDoc::FindLinkFromCoordinateLocation(float x1, float y1, float x2, float y2, float min_distance_in_mile)
{
	DTALink* pSelectedLink = NULL;
	float min_distance = 999999;

	int x_key = (x1 - m_GridRect.left) / m_GridXStep;
	int y_key = (y1 - m_GridRect.bottom) / m_GridYStep;

	//feasible region
	x_key = max(0, x_key);
	x_key = min(_MAX_TRANSIT_GRID_SIZE - 1, x_key);

	y_key = max(0, y_key);
	y_key = min(_MAX_TRANSIT_GRID_SIZE - 1, y_key);

	for (int i = 0; i < m_GridMatrix[x_key][y_key][0].m_LinkNoVector.size(); i++)
	{
		int linkno = m_GridMatrix[x_key][y_key][0].m_LinkNoVector[i];
		DTALink* pLink = m_LinkNoMap[linkno];
		float distance = sqrt(pow(x1 - pLink->m_FromPoint.x, 2) + pow(y1 - pLink->m_FromPoint.y, 2) + pow(x2 - pLink->m_ToPoint.x, 2) + pow(y2 - pLink->m_ToPoint.y, 2));
		if (distance < min_distance)
		{
			min_distance = distance;
			pSelectedLink = pLink;

		}

	}

	int from_x_key = x_key;
	int from_y_key = y_key;


	x_key = (x2 - m_GridRect.left) / m_GridXStep;
	y_key = (y2 - m_GridRect.bottom) / m_GridYStep;

	//feasible region
	x_key = max(0, x_key);
	x_key = min(_MAX_TRANSIT_GRID_SIZE - 1, x_key);

	y_key = max(0, y_key);
	y_key = min(_MAX_TRANSIT_GRID_SIZE - 1, y_key);

	if (from_x_key != x_key || from_y_key != y_key)
	{
		for (int i = 0; i < m_GridMatrix[x_key][y_key][0].m_LinkNoVector.size(); i++)
		{
			int linkno = m_GridMatrix[x_key][y_key][0].m_LinkNoVector[i];
			DTALink* pLink = m_LinkNoMap[linkno];
			float distance = sqrt(pow(x1 - pLink->m_FromPoint.x, 2) + pow(y1 - pLink->m_FromPoint.y, 2) + pow(x2 - pLink->m_ToPoint.x, 2) + pow(y2 - pLink->m_ToPoint.y, 2));
			if (distance < min_distance)
			{
				min_distance = distance;
				pSelectedLink = pLink;

			}

		}
	}


	float distance_in_mile = min_distance / max(0.0000001, m_UnitDistance);
	if (distance_in_mile < min_distance_in_mile)
		return pSelectedLink;
	else
		return NULL;
}

bool CTLiteDoc::ReadSensorTrajectoryData(LPCTSTR lpszFileName)
{
	CCSVParser parser;
	int i= 0;

	bool bRectInitialized = false;

	if (parser.OpenCSVFile(CString2StdString(lpszFileName)))
	{

		while(parser.ReadRecord())
		{

			AgentLocationRecord element;
			
			if(parser.GetValueByFieldName("time_stamp_in_second",element.time_stamp_in_second) == false)
				break;

			if(parser.GetValueByFieldName("trace_id",element.agent_id) == false)
				break;


			if(parser.GetValueByFieldName("x",element.x) == false)
				break;

			if(parser.GetValueByFieldName("y",element.y) == false)
				break;


			AddLocationRecord(element);
			
			if(!bRectInitialized)
			{
				m_NetworkRect.left = element.x ;
				m_NetworkRect.right = element.x;
				m_NetworkRect.top = element.y;
				m_NetworkRect.bottom = element.y;
				bRectInitialized = true;
			}

			GDPoint point;
			point.x = element.x;
			point.y = element.y;

			m_NetworkRect.Expand(point);
		
			
			i ++;

		}
	
		m_AgentLocationLoadingStatus.Format("%d trace location records are loaded from file %s.",i,lpszFileName);	
	}

		return true;
}

void CTLiteDoc::ResetZoneIDVector()
{

	//update m_ODSize
	std::map<int, DTAZone>	:: const_iterator itr;

	for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
	{
		if( itr->first > m_ODSize)
		{

			m_ODSize = itr->first;
		}
	}

	

}

void CTLiteDoc::ResetODMOEMatrix()
{

	if(m_ODSize== m_PreviousODSize)
		return; 


	ResetZoneIDVector();

	if(m_AgentSet.size() ==0)  // no vheicle data, no need to process OD matrix
		return;

	if(m_ODMOEMatrix == NULL  )
	{
		m_ODMOEMatrix = Allocate3DDynamicArray<AgentStatistics>(m_DemandTypeSize,m_ZoneNoSize,m_ZoneNoSize);
		m_PreviousDemandTypeSize = m_DemandTypeSize;
		m_PreviousZoneNoSize = m_ZoneNoSize ;
	}
	else
	{
		Deallocate3DDynamicArray<AgentStatistics>(m_ODMOEMatrix,m_PreviousDemandTypeSize, m_PreviousZoneNoSize);
		m_ODMOEMatrix = Allocate3DDynamicArray<AgentStatistics>(m_DemandTypeSize,m_ZoneNoSize,m_ZoneNoSize);
		m_PreviousDemandTypeSize = m_DemandTypeSize;
		m_PreviousZoneNoSize = m_ZoneNoSize ;
	}

	int p, i, j;

	for(p=0;p<m_DemandTypeSize; p++)
	{
		for(i= 0; i < m_ZoneNoSize ; i++)
		{
			for(j= 0; j< m_ZoneNoSize ; j++)
			{
				m_ODMOEMatrix[p][i][j].Reset ();
			}

		}

	}



	int count  = 0;
	std::list<DTAAgent*>::iterator iAgent;


	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++, count++)
	{
		DTAAgent* pAgent = (*iAgent);
		if(/*pAgent->m_NodeSize >= 2 && */pAgent->m_bComplete )
		{
			int p = pAgent->m_DemandType ;
			p = 0;

			if(m_ZoneIDToNodeNoMap.find(pAgent->m_FromZoneID)== m_ZoneIDToNodeNoMap.end())
			{

				CString msg;
				msg.Format("Zone ID %d for Agent %d has not been defined in the input_zone.csv file. Please check.",pAgent->m_FromZoneID , pAgent->m_AgentID );
				AfxMessageBox(msg);
				return;
			}


			if(m_ZoneIDToNodeNoMap.find(pAgent->m_FromZoneID)== m_ZoneIDToNodeNoMap.end())
				continue;

			if (m_ZoneIDToNodeNoMap.find(pAgent->m_ToZoneID) == m_ZoneIDToNodeNoMap.end())
				continue;

			int OrgNo = m_ZoneIDtoZoneNoMap[pAgent->m_FromZoneID];
			int DesNo = m_ZoneIDtoZoneNoMap[pAgent->m_ToZoneID];

			if(OrgNo<0 || DesNo< 0)
				continue;
			m_ODMOEMatrix[p][OrgNo][DesNo].TotalAgentSize+=1;
			m_ODMOEMatrix[p][OrgNo][DesNo].TotalTravelTime += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
			m_ODMOEMatrix[p][OrgNo][DesNo].TotalDistance += pAgent->m_Distance;



		}
	}

	m_PreviousODSize = m_ODSize;

}


void CTLiteDoc::OnSensortoolsConverttoHourlyVolume()
{
	SensortoolsConverttoHourlyVolume();
	OpenCSVFileInExcel( m_ProjectDirectory +"export_validation_results.csv");
}
void CTLiteDoc::SensortoolsConverttoHourlyVolume()
{
}

int CTLiteDoc::SelectLink(GDPoint point, double& final_matching_distance)
{
	double Min_distance  = 99; // at least interset with a link

	int SelectedLinkNo = -1;

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink !=m_LinkSet.end(); iLink++)
	{

		for(int si = 0; si < (*iLink) ->m_ShapePoints .size()-1; si++)
		{

			GDPoint p0 = point;
			GDPoint pfrom =  (*iLink)->m_ShapePoints[si];
			GDPoint pto = (*iLink)->m_ShapePoints[si+1];;

			float distance = g_GetPoint2LineDistance(p0, pfrom, pto, m_UnitDistance);

			if(distance >0 && distance < Min_distance && (*iLink)->m_DisplayLinkID <0)  // not selected
			{

				m_SelectedLinkNo = (*iLink)->m_LinkNo ;
				Min_distance = distance;
				SelectedLinkNo = (*iLink)->m_LinkNo ;
			}
		}
	}

	final_matching_distance = Min_distance;

	return SelectedLinkNo;
}



void CTLiteDoc::OnTrafficcontroltoolsTransfermovementdatafromreferencenetworktocurrentnetwork()
{

	MapSignalDataAcrossProjects();
}

void CTLiteDoc::OnDemandtoolsGenerateinput()
{

}

bool CTLiteDoc::ReadModelAgentTrajectory(LPCTSTR lpszFileName)
{
	CCSVParser parser;
	int i= 0;

	bool bRectInitialized = false;

	if (parser.OpenCSVFile(CString2StdString(lpszFileName)))
	{

			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->m_bShowLayerMap[layer_Agent_position] = true;
			//			m_bGPSDataSet  = true;
		while(parser.ReadRecord())
		{

			AgentLocationRecord element;
			
			if(parser.GetValueByFieldName("time_stamp_in_second",element.time_stamp_in_second) == false)
				continue;

			if(parser.GetValueByFieldName("agent_id",element.agent_id) == false)
				continue;


			if(parser.GetValueByFieldName("x",element.x, false) == false)
				continue;

			if(parser.GetValueByFieldName("y",element.y, false) == false)
				continue;



			if(parser.GetValueByFieldName("to_x",element.to_x) == true)
			{
				if(parser.GetValueByFieldName("to_y",element.to_y) == true)
					element.b_to_data_flag  = true;
			}
				
			AddLocationRecord(element);
			
			if(!bRectInitialized)
			{
				m_NetworkRect.left = element.x ;
				m_NetworkRect.right = element.x;
				m_NetworkRect.top = element.y;
				m_NetworkRect.bottom = element.y;
				bRectInitialized = true;
			}

			GDPoint point;
			point.x = element.x;
			point.y = element.y;

			m_NetworkRect.Expand(point);
			
			
			i ++;

		}


///

			m_ZoneMap[0].m_ZoneID = 0;

				std::map<string,AgentLocationTimeIndexedMap>::iterator itr2;

				for(itr2 = m_AgentWithLocationVectorMap.begin();
					itr2 != m_AgentWithLocationVectorMap .end(); itr2++)
				{		//scan all Agent records at this timestamp


			DTAAgent* pAgent = 0;
			pAgent = new DTAAgent;
			pAgent->m_bGPSAgent = true;
			pAgent->m_DayNo 		= 1;

			pAgent->m_AgentID		= m_AgentSet.size();

			pAgent->m_FromZoneID	= 0;
			pAgent->m_ToZoneID= 0;

			pAgent->m_Distance = 10; // km to miles
			pAgent->m_DepartureTime	=  itr2->second .AgentLocationRecordVector[0].time_stamp_in_second/10;
			pAgent->m_ArrivalTime =  itr2->second .AgentLocationRecordVector[itr2->second .AgentLocationRecordVector.size()-1].time_stamp_in_second;


			pAgent->m_TripTime  = pAgent->m_ArrivalTime - pAgent->m_DepartureTime;

			if(g_Simulation_Time_Horizon < pAgent->m_ArrivalTime)
				g_Simulation_Time_Horizon = pAgent->m_ArrivalTime;

			pAgent->m_bComplete = true;
			pAgent->m_DemandType = 1;


			//pAgent->m_AgentType = (unsigned char)g_read_integer(pFile);

				m_AgentSet.push_back (pAgent);
				m_AgentIDMap[pAgent->m_AgentID]  = pAgent;


				}
	
	//	m_AgentLocationLoadingStatus.Format("%d agent location records for %d agents are loaded from file %s.",i,m_AgentWithLocationVectorMap.size(),lpszFileName);	
	}
return true;
}



bool CTLiteDoc::ReadGPSTrajectory(LPCTSTR lpszFileName)
{
	CCSVParser parser;
	int i = 0;

	bool bRectInitialized = false;

	if (parser.OpenCSVFile(CString2StdString(lpszFileName)))
	{

		while (parser.ReadRecord())
		{

			AgentLocationRecord element;

	

			if (parser.GetValueByFieldName("car_id", element.agent_id) == false)
				continue;


			string geo_string;
			if (parser.GetValueByFieldName("location", geo_string) == false)
				continue;

			std::vector<float> vect;

			std::stringstream ss(geo_string);
			float value;

			while (ss >> value)
			{
				vect.push_back(value);

				if (ss.peek() == ',')
					ss.ignore();
			}


			if (vect.size() == 2)
			{
				element.y = vect[0];
				element.x = vect[1];
			}


			string date_string;
			if (parser.GetValueByFieldName("date", date_string) == false)
				continue;

			std::vector<int> date_vect;

			std::stringstream ss_date(date_string);
			int date_value;

			while (ss_date >> date_value)
			{
				date_vect.push_back(date_value);

				if (ss_date.peek() == ',' || ss_date.peek() == ' ' || ss_date.peek() == '-' || ss_date.peek() == ':')
					ss_date.ignore();
			}

			if (date_vect.size() == 6)
			{
				int sec = date_vect[5] - date_vect[5] % 20;
				element.time_stamp_in_second = date_vect[3] * 3600 + date_vect[4] * 60 + sec;  // hour, min, second
			
				element.day_no = date_vect[2];
				if (element.time_stamp_in_second == 30780 && element.agent_no == 3)
				{ 
					TRACE("line no. %d", i);
				}
			}



			AddLocationRecord(element);

			if (!bRectInitialized)
			{
				m_NetworkRect.left = element.x;
				m_NetworkRect.right = element.x;
				m_NetworkRect.top = element.y;
				m_NetworkRect.bottom = element.y;
				bRectInitialized = true;
			}

			GDPoint point;
			point.x = element.x;
			point.y = element.y;

			m_NetworkRect.Expand(point);


			i++;

		}


		///

		m_ZoneMap[0].m_ZoneID = 0;

		std::map<string, AgentLocationTimeIndexedMap>::iterator itr2;

		for (itr2 = m_AgentWithLocationVectorMap.begin();
			itr2 != m_AgentWithLocationVectorMap.end(); itr2++)
		{		//scan all Agent records at this timestamp


			DTAAgent* pAgent = 0;
			pAgent = new DTAAgent;
			pAgent->m_bGPSAgent = true;
			pAgent->m_DayNo = 1;

			pAgent->m_AgentID = m_AgentSet.size();

			pAgent->m_FromZoneID = 0;
			pAgent->m_ToZoneID = 0;

			pAgent->m_Distance = 10; // km to miles
			pAgent->m_DepartureTime = itr2->second.AgentLocationRecordVector[0].time_stamp_in_second / 10;
			pAgent->m_ArrivalTime = itr2->second.AgentLocationRecordVector[itr2->second.AgentLocationRecordVector.size() - 1].time_stamp_in_second;


			pAgent->m_TripTime = pAgent->m_ArrivalTime - pAgent->m_DepartureTime;

			if (g_Simulation_Time_Horizon < pAgent->m_ArrivalTime)
				g_Simulation_Time_Horizon = pAgent->m_ArrivalTime;

			pAgent->m_bComplete = true;
			pAgent->m_DemandType = 1;


			//pAgent->m_AgentType = (unsigned char)g_read_integer(pFile);

			m_AgentSet.push_back(pAgent);
			m_AgentIDMap[pAgent->m_AgentID] = pAgent;


		}

	///	m_AgentLocationLoadingStatus.Format("%d agent location records for %d agents are loaded from file %s.", i, m_AgentWithLocationVectorMap.size(), lpszFileName);
	}
	return true;
}





void CTLiteDoc::OnNetworktoolsResetlinklength()
{
	if(AfxMessageBox("Do you want to use long/lat coordinate data in the link shape file to recalculate the link length?", MB_YESNO) == IDYES)
	{
		CWaitCursor wait;

		double total_length = 0;

		double default_coordinate_distance_sum = 0;

		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			(*iLink)->m_Length  = 0;


			for(unsigned int si = 0; si < (*iLink) ->m_ShapePoints .size()-1; si++)
			{
				double distance  =  g_CalculateP2PDistanceInMileFromLatitudeLongitude((*iLink)->m_ShapePoints[si] , (*iLink)->m_ShapePoints[si+1]);
				(*iLink)->m_Length += distance;
			}

			total_length+= (*iLink)->m_Length;

			default_coordinate_distance_sum+= (*iLink)->DefaultDistance();


		}

		if(total_length>0.000001f)
		{
			m_UnitDistance= default_coordinate_distance_sum / total_length ;

		}


		CString str;
		str.Format("The updated average link length per link is now %.3f.\nPlease save the project to confirm the change.", total_length/max(1,m_LinkSet.size() ));
		AfxMessageBox(str,MB_ICONINFORMATION);

	}
}

void CTLiteDoc::UpdateUnitMile()
{
	CWaitCursor wait;

	double total_length_in_mile = 0;

	double default_coordinate_distance_sum = 0;

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		total_length_in_mile+= (*iLink)->m_Length;

		default_coordinate_distance_sum+= (*iLink)->DefaultDistance();


	}

	if(total_length_in_mile>0.000001f)
	{
		m_UnitDistance= default_coordinate_distance_sum / total_length_in_mile ;

	}


}

void CTLiteDoc::OnSubareaCreatezonefromsubarea()
{
	int zone_number = -1;  // starting default number 


//	PushBackNetworkState();


	Modify ();
	if(m_SubareaNodeSet.size()>0 )
	{
		m_PreviousODSize  = -1; // require to regenerate od zone vector

		DTANode* pNode = m_SubareaNodeSet[0];



		if(m_ZoneMap.find(pNode->m_NodeID)==m_ZoneMap.end())
		{
			zone_number = pNode->m_NodeID ;

			m_ZoneMap [zone_number].m_ZoneID = zone_number;

		}else
		{  // Zone has been defined. we can just update zone boundary

			// this node has a zone Id already, we just update their zone boundary 
			if(m_ZoneMap.find(pNode->m_ZoneID)!=m_ZoneMap.end())
			{

				zone_number = pNode->m_ZoneID;
				m_ZoneMap [zone_number].m_ShapePoints.clear ();

				for (unsigned int sub_i= 0; sub_i < m_SubareaShapePoints.size(); sub_i++)
				{
					m_ZoneMap [zone_number].m_ShapePoints .push_back (m_SubareaShapePoints[sub_i]);

				}



			}


			return;
		}

	}
	if(zone_number==-1)
	{ // find largest zone number for the new zone


		std::map<int, DTAZone>	:: const_iterator itr;

		for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
		{
			if( zone_number <= itr->first)
				zone_number = itr->first +1;

		}

		if(zone_number==-1)  // no zone has been defined yet
			zone_number = 1; 

		m_ZoneMap [zone_number].m_ZoneID = max(1,zone_number);

	}


	for (unsigned int sub_i= 0; sub_i < m_SubareaShapePoints.size(); sub_i++)
	{
		m_ZoneMap [zone_number].m_ShapePoints .push_back (m_SubareaShapePoints[sub_i]);

	}


	// add activity locations if there is no activity location being assigned yet
	m_SubareaShapePoints.clear();
	UpdateAllViews(0);
}


bool CTLiteDoc::FindObject(eSEARCHMODE SearchMode, int value1, int value2)
{

	if(SearchMode == efind_link)
	{

		DTALink* pLink = FindLinkWithNodeNumbers(value1 ,value2 );

		if(pLink !=NULL)
		{
			m_SelectedLinkNo = pLink->m_LinkNo ;
			m_SelectedNodeID = -1;

			ZoomToSelectedLink(pLink->m_LinkNo);


		}
	}

	if(SearchMode == efind_node)
	{
		DTANode* pNode = FindNodeWithNodeNumber (value1);
		if(pNode !=NULL)
		{
			m_SelectedLinkNo = -1;
			m_SelectedNodeID = pNode->m_NodeNo ;

			ZoomToSelectedNode(value1);

		} return false;
	}

	if(SearchMode == efind_path)
	{

		DTANode* pFromNode = FindNodeWithNodeNumber (value1);
		if(pFromNode ==NULL)
		{
			CString str;
			str.Format ("From Node %d cannot be found.",value1);
			AfxMessageBox(str);
			return false; 
		}else
		{
			m_FromNodeID = pFromNode->m_NodeNo;

		}
		DTANode* pToNode = FindNodeWithNodeNumber (value2);
		if(pToNode ==NULL)
		{
			CString str;
			str.Format ("To Node %d cannot be found.",value2);
			AfxMessageBox(str);
			return false;
		}else
		{
			m_ToNodeID = pToNode->m_NodeNo;
		}

		Routing(false);

	}

	if(SearchMode == efind_Agent)
	{
		int SelectedAgentID = value1; // internal Agent index starts from zero
		std::vector<int> LinkVector;

		if(m_AgentIDMap.find(SelectedAgentID) == m_AgentIDMap.end())
		{
			if(SelectedAgentID>=0)
			{
				CString str_message;
				str_message.Format ("Agent Id %d cannot be found.", SelectedAgentID+1);
				AfxMessageBox(str_message);
			}

			m_SelectedAgentID = -1;
			HighlightPath(LinkVector,1);

			return true;
		}

		m_SelectedAgentID = SelectedAgentID;

		DTAAgent* pAgent = m_AgentIDMap[SelectedAgentID];

		CPlayerSeekBar m_wndPlayerSeekBar;


		// set departure time to the current time of display
		g_Simulation_Time_Stamp = pAgent->m_DepartureTime +1;

		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

		pMainFrame->m_wndPlayerSeekBar.SetPos(g_Simulation_Time_Stamp);

		m_LinkMOEMode = MOE_Agent;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			LinkVector.push_back (pAgent->m_NodeAry[link].LinkNo);
		}

		HighlightPath(LinkVector,1);

		m_HighlightGDPointVector.clear();

		m_HighlightGDPointVector = pAgent->m_GPSLocationVector ; // assign the GPS points to be highlighted

	}
	UpdateAllViews(0);

	return true;
}


void CTLiteDoc::OnTrafficcontroltoolsTransfersignaldatafromreferencenetworktocurrentnetwork()
{
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

	int count_node = 0 ;
	int count_link = 0 ;
	std::list<DTANode*>::iterator  iNode;


	FILE* st = NULL;

	CString str_log_file;
	str_log_file.Format ("%s\\signal_data_reading_log.csv",m_ProjectDirectory);

	fopen_s(&st,str_log_file,"w");


	if( st !=NULL)
	{


		//	fprintf(st,"intersection_name1,intersection_name2, baseline_node_id,reference_node_id,base_line_cycle_length_in_second,reference_cycle_length_in_second\n");


		fprintf(st,"------\n");
		fprintf(st,"Step 2: Read Cycle length and offset,\n");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			int baseline_node_id = (*iNode)->m_NodeID  ;
			if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
			{

				DTANode* pRefNode = NULL;
				int reference_node_number =  (*iNode)->m_NodeID;



			}

		}

		// step 2: fetch data for all links
		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pThisLink = (*iLink);
			DTALink* pReferenceLink =  pReferenceDoc->FindLinkWithNodeNumbers (pThisLink->m_FromNodeNumber ,pThisLink->m_ToNodeNumber );

			DTANode* pDownstreamNode = m_NodeNumberMap[pThisLink->m_ToNodeNumber ];

			if(pReferenceLink!=NULL && 
				(pDownstreamNode->m_ControlType == m_ControlType_PretimedSignal || pDownstreamNode->m_ControlType == m_ControlType_ActuatedSignal))
			{
				DTANode* pRefNode = pReferenceDoc->m_NodeNumberMap[pThisLink->m_ToNodeNumber ];

				if(pRefNode->m_ControlType == pReferenceDoc->m_ControlType_PretimedSignal || pRefNode->m_ControlType == pReferenceDoc->m_ControlType_ActuatedSignal)
				{

					fprintf(st,"Link,%d->%d,  Old # of left turn lanes,%d, Updated of left turn lanes,%d,Old # of right turn lanes,%d, Updated of right turn lanes,%d\n", 
						pThisLink->m_FromNodeNumber,pThisLink->m_ToNodeNumber,
						pThisLink->m_NumberOfLeftTurnLanes,
						pReferenceLink->m_NumberOfLeftTurnLanes,
						pThisLink->m_NumberOfRightTurnLanes ,
						pReferenceLink->m_NumberOfRightTurnLanes
						);

					pThisLink->m_NumberOfLeftTurnLanes   = pReferenceLink->m_NumberOfLeftTurnLanes;
					pThisLink->m_NumberOfRightTurnLanes    = pReferenceLink->m_NumberOfRightTurnLanes;


					count_link++;
				}
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
	str.Format("%d nodes and %d links have obtained cycle length and signal timing from the reference network. To accept the changes, please save the network.",count_node, count_link);
	AfxMessageBox(str, MB_ICONINFORMATION);

}

void CTLiteDoc::OnImportBackgroundimage()
{

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_LONGNAMES|OFN_ENABLESIZING,
		_T("background image file(*.bmp)|*.bmp|"),NULL,0);
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CString bitmap_string  = "//importing_sample_data_sets";
	CString NetworkFile = pMainFrame->m_CurrentDirectory + bitmap_string;
	dlg.m_ofn.lpstrInitialDir = NetworkFile ;


	if(dlg.DoModal() == IDOK)
	{

			CopyFile(dlg.GetPathName (),m_ProjectDirectory+"background_image.bmp",false);
			m_ImageX1  = m_NetworkRect.left;
			m_ImageY1  = m_NetworkRect.bottom;
			m_ImageX2 = m_NetworkRect.right;
			m_ImageY2 = m_NetworkRect.top;
			m_ImageWidth = fabs(m_ImageX2 - m_ImageX1);
			m_ImageHeight = fabs(m_ImageY2 - m_ImageY1);


	}
	m_bFitNetworkInitialized  = false;
	CalculateDrawingRectangle(false);
	UpdateAllViews(0);
}

void CTLiteDoc::OnZoneDeletezone()
{
	Modify ();

	m_ZoneMap.erase (this->m_SelectedZoneID );
	this->m_SelectedZoneID = -1;
	UpdateAllViews(0);


}





void CTLiteDoc::OnNodeViewnodedata()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_SelectTab = _NODE_DATA; 
	dlg.m_pDoc = this;

	if(m_SelectedNodeID>= 0 )
	{
		dlg.m_SelectRowByUser  = m_SelectedNodeID;

	}
	dlg.DoModal();
}

void CTLiteDoc::OnLinkViewlinkdata()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_SelectTab = _LINK_DATA; 
	dlg.m_pDoc = this;

	dlg.m_SelectRowByUser =  m_SelectedLinkNo;
	dlg.DoModal();


}

void CTLiteDoc::OnMovementViewmovementdatatable()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_SelectTab = _MOVEMENT_DATA; 
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CTLiteDoc::OnOdmatrixOddemandmatrix()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::OnWorkzoneViewworkzonedata()
{

}

void CTLiteDoc::OnVmsViewvmsdatatable()
{

}

void CTLiteDoc::OnTollViewtolldatatable()
{

}

void CTLiteDoc::OnDetectorViewsensordatatable()
{
}



void CTLiteDoc::OnLinkmoeExportlinkmoedatatoshapefile()
{

}


void CTLiteDoc::OnNodeExportnodelayertogisshapefile()
{

}

void CTLiteDoc::OnZoneExportzonelayertogisshapefile()
{
}

void CTLiteDoc::OnGridUsemileasunitoflength()
{
	m_bUseMileVsKMFlag = true;
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateGridUsemileasunitoflength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bUseMileVsKMFlag == true);
}

void CTLiteDoc::OnGridUsekmasunitoflength()
{
	m_bUseMileVsKMFlag = false;
	UpdateAllViews(0);

}

void CTLiteDoc::OnUpdateGridUsekmasunitoflength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bUseMileVsKMFlag == false);
}

void CTLiteDoc::OnGridUselong()
{
	m_LongLatFlag = !m_LongLatFlag;
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateGridUselong(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LongLatFlag);
}








void CTLiteDoc::OnShowMoePathlist()
{
	m_bShowPathList = true;
	ShowPathListDlg(m_bShowPathList);}

void CTLiteDoc::OnExportExportaggregatedlinkmoefile()
{
	if(m_ProjectDirectory.GetLength () >0)
	{
		SaveLinkData(m_ProjectDirectory+"AMS_link.csv",true,0);
		OpenCSVFileInExcel(m_ProjectDirectory + "AMS_link.csv");

	}

}

void CTLiteDoc::OnHelpReportbug()
{
	g_OpenDocument("http://code.google.com/p/nexta/issues/list", SW_SHOW);
}







int CTLiteDoc:: FindNonCentroidNodeNumberWithCoordinate(double x, double y, int this_node_name)
{

	DTANode* pNode= NULL;

	double min_distance = 999999;
	int NodeID = -1;
	for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		if((*iNode)->m_NodeID  != this_node_name && (*iNode)->m_ZoneID <=0)  // non centroid node
		{
			double distance = sqrt( ((*iNode)->pt.x - x)*((*iNode)->pt.x - x) + ((*iNode)->pt.y - y)*((*iNode)->pt.y - y));
			if( distance <  min_distance)
			{
				min_distance= distance;
				pNode = (*iNode);
			}
		}
	}
	if(pNode != NULL)
		return pNode->m_NodeID;
	else
		return NULL;
}


void CTLiteDoc::OnLinkApplydefaultlinkattributestoalllinks()
{
	CWaitCursor wait;

	std::list<DTALink*>::iterator iLink;
	Modify();

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink* pLink = (*iLink);

		pLink->m_FreeSpeed = m_LinkTypeMap[pLink->m_link_type ].default_speed ;

	}

}

void CTLiteDoc::OnLinkApplydefaultlanecapacitytoalllinks()
{
	CWaitCursor wait;

	std::list<DTALink*>::iterator iLink;

	Modify();
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink* pLink = (*iLink);

		pLink->m_LaneCapacity = m_LinkTypeMap[pLink->m_link_type ].default_lane_capacity ;

	}
}

void CTLiteDoc::OnLinkApplydefaultnumberoflanestoalllinks()
{
	CWaitCursor wait;

	std::list<DTALink*>::iterator iLink;

	Modify();
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink* pLink = (*iLink);

		pLink->m_NumberOfLanes  = m_LinkTypeMap[pLink->m_link_type ].default_number_of_lanes ;

	}
}



void CTLiteDoc::OnTrafficcapacitySetdefault()
{

}






void CTLiteDoc::OnMovementHidenon()
{
	m_hide_non_specified_movement_on_freeway_and_ramp = !m_hide_non_specified_movement_on_freeway_and_ramp;
}

void CTLiteDoc::OnUpdateMovementHidenon(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_hide_non_specified_movement_on_freeway_and_ramp);
}

void CTLiteDoc::OnMovementSetpeakhourfactor()
{

}

void CTLiteDoc::OnZoneChangezonenumber()
{
	
}

void CTLiteDoc::OnUpdateZoneDeletezone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (m_SelectedZoneID>=1);
}

void CTLiteDoc::OnUpdateZoneChangezonenumber(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (m_SelectedZoneID>=1);

}

void CTLiteDoc::OnChangelinktypecolorFreeway()
{

	CColorDialog dlg(theApp.m_FreewayColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_FreewayColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnChangelinktypecolorRamp()
{
	CColorDialog dlg(theApp.m_RampColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_RampColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnChangelinktypecolorArterial()
{
	CColorDialog dlg(theApp.m_ArterialColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_ArterialColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnChangelinktypecolorConnector()
{
	CColorDialog dlg(theApp.m_ConnectorColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_ConnectorColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnChangelinktypecolorTransit()
{
	CColorDialog dlg(theApp.m_TransitColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_TransitColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnChangelinktypecolorWalkingmode()
{
	CColorDialog dlg(theApp.m_WalkingColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_WalkingColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnChangelinktypecolorResettodefaultcolorschema()
{

	theApp.m_FreewayColor = RGB(030,144,255);
	theApp.m_RampColor = RGB(160,032,240); 
	theApp.m_ArterialColor = RGB(034,139,034);
	theApp.m_ConnectorColor = RGB(255,165,000);
	theApp.m_TransitColor = RGB(255,0,255);
	theApp.m_WalkingColor = RGB(127,255,0);
	theApp.m_ParkingandRideColor = RGB(0, 255, 0);
}

void CTLiteDoc::OnNodeChangenodecolor()
{
	CColorDialog dlg(theApp.m_NodeColor , CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_NodeColor= dlg.GetColor();
		UpdateAllViews(0);
	}

}

void CTLiteDoc::OnNodeChangenodebackgroundcolor()
{
	CColorDialog dlg(theApp.m_NodeBrushColor , CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_NodeBrushColor= dlg.GetColor();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnZoneChangezonecolor()
{
	CColorDialog dlg(theApp.m_ZoneColor , CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_ZoneColor= dlg.GetColor();
		UpdateAllViews(0);
	}

}

void CTLiteDoc::OnEditUndo33707()
{
	Undo();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateEditUndo33707(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_NetworkState.size()>0);
}

void CTLiteDoc::OnEditRedo33709()
{
	Redo();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateEditRedo33709(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_RedoNetworkState.size()>0);
}
void CTLiteDoc::PushBackNetworkState()
{
	CWaitCursor wait;

	m_RedoNetworkState.clear();

	NetworkState element;

	element.l_NodeNoMap = m_NodeNoMap;
	element.l_NodeNumberMap = m_NodeNumberMap;
	element.l_LinkNoMap = m_LinkNoMap;
	element.l_NodeNotoLinkMap = m_NodeNotoLinkMap;
	element.l_NodeNumbertoLinkMap = m_NodeNumbertoLinkMap;
	element.l_LinkNotoLinkMap = m_LinkNotoLinkMap;
	element.l_LinkIDtoLinkMap = m_LinkIDtoLinkMap;


	element.l_NodeSet = m_NodeSet;
	element.l_LinkSet = m_LinkSet;

	element.l_SubareaNodeSet = m_SubareaNodeSet;
	element.l_SubareaLinkSet = m_SubareaLinkSet;
	element.l_ZoneMap =  m_ZoneMap;

	element.l_NodeNumbertoNodeNoMap = m_NodeNumbertoNodeNoMap;
	element.l_NodeNotoZoneNameMap = m_NodeNotoZoneNameMap ;
	element.l_SubareaShapePoints = m_SubareaShapePoints;


	m_NetworkState.push_back(element);

	if(m_NetworkState.size()>=20)  // no more than 20 steps in undo
	{
		m_NetworkState.erase (m_NetworkState.begin ());

	}



}
void CTLiteDoc::Undo()
{

	NetworkState current_element;

	current_element.l_NodeSet.clear ();

	current_element.l_NodeNoMap = m_NodeNoMap;
	current_element.l_NodeNumberMap = m_NodeNumberMap;
	current_element.l_LinkNoMap = m_LinkNoMap;
	current_element.l_NodeNotoLinkMap = m_NodeNotoLinkMap;
	current_element.l_NodeNumbertoLinkMap = m_NodeNumbertoLinkMap;
	current_element.l_LinkNotoLinkMap = m_LinkNotoLinkMap;
	current_element.l_LinkIDtoLinkMap = m_LinkIDtoLinkMap;


	current_element.l_NodeSet = m_NodeSet;
	current_element.l_LinkSet = m_LinkSet;

	current_element.l_SubareaNodeSet = m_SubareaNodeSet;
	current_element.l_SubareaLinkSet = m_SubareaLinkSet;
	current_element.l_ZoneMap =  m_ZoneMap;

	current_element.l_NodeNumbertoNodeNoMap = m_NodeNumbertoNodeNoMap;
	current_element.l_NodeNotoZoneNameMap = m_NodeNotoZoneNameMap ;
	current_element.l_SubareaShapePoints = m_SubareaShapePoints;
	TRACE("\n%d links", current_element.l_LinkSet.size());

	m_RedoNetworkState.push_back(current_element);		

	if(m_NetworkState.size() >0)
	{
		CWaitCursor wait;
		NetworkState element =  m_NetworkState[m_NetworkState.size ()-1];


		m_NodeSet = element.l_NodeSet;
		m_LinkSet = element.l_LinkSet;

		m_SubareaNodeSet = element.l_SubareaNodeSet;
		m_SubareaLinkSet = element.l_SubareaLinkSet;

		m_NodeNotoLinkMap = element.l_NodeNotoLinkMap;

		m_ZoneMap = element.l_ZoneMap ;

		m_NodeNumbertoNodeNoMap = element.l_NodeNumbertoNodeNoMap;
		m_NodeNotoZoneNameMap = element.l_NodeNotoZoneNameMap;

		m_NodeNoMap = element.l_NodeNoMap;
		m_NodeNumberMap = element.l_NodeNumberMap;
		m_LinkNoMap = element.l_LinkNoMap;
		m_NodeNumbertoLinkMap = element.l_NodeNumbertoLinkMap;
		m_LinkNotoLinkMap = element.l_LinkNotoLinkMap;
		m_LinkIDtoLinkMap = element.l_LinkIDtoLinkMap;
		m_SubareaShapePoints = element.l_SubareaShapePoints;

		m_NetworkState.pop_back ();

		TRACE("\nRedo: %d links", element.l_LinkSet.size());

		//	m_RedoNetworkState.push_back (element);




		UpdateAllViews(0);
	}


}

void CTLiteDoc::Redo()
{	

	if(m_RedoNetworkState.size() >0)
	{
		CWaitCursor wait;

		NetworkState current_element;

		current_element.l_NodeSet.clear ();

		current_element.l_NodeNoMap = m_NodeNoMap;
		current_element.l_NodeNumberMap = m_NodeNumberMap;
		current_element.l_LinkNoMap = m_LinkNoMap;
		current_element.l_NodeNotoLinkMap = m_NodeNotoLinkMap;
		current_element.l_NodeNumbertoLinkMap = m_NodeNumbertoLinkMap;
		current_element.l_LinkNotoLinkMap = m_LinkNotoLinkMap;
		current_element.l_LinkIDtoLinkMap = m_LinkIDtoLinkMap;


		current_element.l_NodeSet = m_NodeSet;
		current_element.l_LinkSet = m_LinkSet;

		current_element.l_SubareaNodeSet = m_SubareaNodeSet;
		current_element.l_SubareaLinkSet = m_SubareaLinkSet;
		current_element.l_ZoneMap =  m_ZoneMap;

		current_element.l_NodeNumbertoNodeNoMap = m_NodeNumbertoNodeNoMap;
		current_element.l_NodeNotoZoneNameMap = m_NodeNotoZoneNameMap ;
		current_element.l_SubareaShapePoints = m_SubareaShapePoints;

		m_NetworkState.push_back (current_element);

		NetworkState element =  m_RedoNetworkState[m_RedoNetworkState.size()-1];

		TRACE("\nRedo to %d links", element.l_LinkSet.size());

		m_NodeSet = element.l_NodeSet;
		m_LinkSet = element.l_LinkSet;

		m_SubareaNodeSet = element.l_SubareaNodeSet;
		m_SubareaLinkSet = element.l_SubareaLinkSet;

		m_NodeNotoLinkMap = element.l_NodeNotoLinkMap;

		m_ZoneMap = element.l_ZoneMap ;

		m_NodeNumbertoNodeNoMap = element.l_NodeNumbertoNodeNoMap;
		m_NodeNotoZoneNameMap = element.l_NodeNotoZoneNameMap;

		m_NodeNoMap = element.l_NodeNoMap;
		m_NodeNumberMap = element.l_NodeNumberMap;
		m_LinkNoMap = element.l_LinkNoMap;
		m_NodeNumbertoLinkMap = element.l_NodeNumbertoLinkMap;
		m_LinkNotoLinkMap = element.l_LinkNotoLinkMap;
		m_LinkIDtoLinkMap = element.l_LinkIDtoLinkMap;
		m_SubareaShapePoints = element.l_SubareaShapePoints;



		m_RedoNetworkState.pop_back ();

		TRACE("\n%d links", element.l_LinkSet.size());


		UpdateAllViews(0);
	}


}


void CTLiteDoc::GenerateMovementShapePoints()
{

	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		DTANode* pNode = (*iNode);

		double size_ratio= m_MovementTextBoxSizeInDistance/250;

		double node_set_back = size_ratio*150 * m_UnitDistance;

		double link_length = size_ratio*250 * m_UnitDistance;
		double movement_length = size_ratio*100 * m_UnitDistance;
		double lane_width = size_ratio*10 * m_UnitDistance;


		double text_length =  m_MovementTextBoxSizeInDistance * 0.15 *  m_UnitDistance ;

		std::map<CString, double> Turn_Degree_map;

		for (unsigned int i=0;i< pNode->m_MovementDataMap.m_MovementVector .size();i++)
		{
			DTANodeMovement movement = pNode->m_MovementDataMap.m_MovementVector[i];

			pNode->m_MovementDataMap.m_MovementVector[i].m_ShapePoints .clear ();


			if(pNode->m_ControlType != m_ControlType_PretimedSignal && 
				pNode->m_ControlType != m_ControlType_ActuatedSignal)
				continue;


			if(m_LinkNoMap.find(movement.IncomingLinkNo) ==  m_LinkNoMap.end())
				return;

			if(m_LinkNoMap.find(movement.OutgoingLinkNo) ==  m_LinkNoMap.end())
				return;

			DTALink* pInLink  = m_LinkNoMap [movement.IncomingLinkNo];
			DTALink* pOutLink  = m_LinkNoMap [movement.OutgoingLinkNo ];

			int destination_node  = pOutLink->m_ToNodeNumber; 


			GDPoint p1, p2, p3, p_text;
			// 1: fetch all data
			p1  = m_NodeNoMap[movement.in_link_from_node_id ]->pt;
			p2  = m_NodeNoMap[movement.in_link_to_node_id ]->pt;
			p3  = m_NodeNoMap[movement.out_link_to_node_id]->pt;

			// reserved direction
			double DeltaX = p2.x - p1.x ;
			double DeltaY = p2.y - p1.y ;
			double theta = atan2(DeltaY, DeltaX);

			double movement_approach_turnection_theta = atan2(p3.y - p1.y, p3.x - p1.x);


			GDPoint p1_new, p2_new, p3_new;
			GDPoint p1_text, p3_text;
			GDPoint pt_movement[3];


			// 2. set new origin
			p2_new.x = pNode->pt.x +(-1)*node_set_back*cos(theta);  
			p2_new.y = pNode->pt.y + (-1)*node_set_back*sin(theta);

			p1_new.x = pNode->pt.x +(-1)*link_length*cos(theta);
			p1_new.y = pNode->pt.y + (-1)*link_length*sin(theta);

			p_text.x = p1_new.x - text_length*cos(theta);
			p_text.y = p1_new.y - text_length*sin(theta);


			// 3 determine the control point for  PolyBezier
			double lane_width =  size_ratio*3*m_LaneWidthInMeter * m_UnitDistance;
			double movement_offset = 0;
			double text_offset = 0.7*lane_width;
			int mid_lane_number = 4+ pInLink->m_NumberOfLanes / 2;
			float control_point_ratio = 0;

			float MovementTextFontSize = 8;

			CString movement_approach_turnection_label;
			movement_approach_turnection_label.Format ("%d,%s", movement.in_link_from_node_id , GetTurnString(movement.movement_turn));

			if(movement.movement_turn == DTA_Through ) 
			{

				if(Turn_Degree_map.find (movement_approach_turnection_label ) != Turn_Degree_map.end())
				{  // this direction has been used/defined.

					if( Turn_Degree_map[movement_approach_turnection_label ] <  movement_approach_turnection_theta)
						movement_offset = lane_width * (mid_lane_number - 0.7);
					else
						movement_offset = lane_width * (mid_lane_number + 0.7);

				}else
				{
					movement_offset = lane_width * mid_lane_number;

				}

			}else if(movement.movement_turn == DTA_LeftTurn || movement.movement_turn == DTA_LeftTurn2 ) 
			{
				movement_offset = lane_width * (mid_lane_number-1);
			}else
			{
				movement_offset = lane_width * (mid_lane_number+1);
			}

			// keep a record
			Turn_Degree_map[movement_approach_turnection_label] = movement_approach_turnection_theta;


			GDPoint pt_from, pt_to, pt_text;
			pt_from.x = p1_new.x + movement_offset* cos(theta-PI/2.0f);
			pt_from.y = p1_new.y + movement_offset* sin(theta-PI/2.0f);


			double alpha  = 0.3;

			pt_to.x  = p2_new.x + movement_offset* cos(theta-PI/2.0f);
			pt_to.y  =  p2_new.y + movement_offset* sin(theta-PI/2.0f);

			// direction

			DeltaX = p2.x - p3.x ;
			DeltaY = p2.y - p3.y ;
			theta = atan2(DeltaY, DeltaX);


			if(movement.movement_turn == DTA_Through ) 
			{
				p3_new.x = pt_to.x - movement_length * cos(theta);
				p3_new.y = pt_to.y - movement_length * sin(theta);

			}else if(movement.movement_turn == DTA_LeftTurn ) 
			{
				p3_new.x = pt_to.x - movement_length * cos(theta -PI/4.0f);
				p3_new.y = pt_to.y - movement_length * sin(theta -PI/4.0f);
			}else
			{
				p3_new.x = pt_to.x - movement_length * cos(theta +PI/4.0f);
				p3_new.y = pt_to.y - movement_length * sin(theta +PI/4.0f);
			}

			// 		GDPoint p1_new, p2_new, p3_new;
			pNode->m_MovementDataMap.m_MovementVector[i].m_ShapePoints.push_back(p1_new);
			pNode->m_MovementDataMap.m_MovementVector[i].m_ShapePoints.push_back(p2_new);
			pNode->m_MovementDataMap.m_MovementVector[i].m_ShapePoints.push_back(p3_new);

		}  // per movement
	} // per node

}
void CTLiteDoc::OnMovementSetupnumberofleftturnlanesforsignalizednodes()
{


}

void CTLiteDoc::OnMovementOptimizephasingandtimingdataforalltrafficsignalsthroughqem()
{
}

void CTLiteDoc::OnLinkmoedisplayImpact()
{
	m_LinkMOEMode = MOE_impact;
	m_LinkBandWidthMode = LBW_congestion_duration;

	ShowLegend(true);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateLinkmoedisplayImpact(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_impact);
}

void CTLiteDoc::OnLinkmoedisplayBottleneck()
{
	m_LinkMOEMode = MOE_bottleneck;
	m_LinkBandWidthMode = LBW_number_of_lanes;

	ShowLegend(true);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateLinkmoedisplayBottleneck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_bottleneck);
}
float CTLiteDoc::GetNodeTotalDelay(int ToNodeNumber, int time, int& LOS)
{
	float total_delay = 0;
	DTANode* pToNode = m_NodeNumberMap[ToNodeNumber];

	int RelativeStartTime = 9999;
	for(unsigned int i = 0; i< pToNode->m_IncomingLinkVector.size(); i++)
	{

		DTALink* pLink = m_LinkNoMap[pToNode->m_IncomingLinkVector[i]];
		total_delay+= pLink->GetDynamicNodeDelay(time, this->m_PrimaryDataSource );

		RelativeStartTime = min(RelativeStartTime, pLink->GetImpactRelativeStartTime(g_Simulation_Time_Stamp,this->m_PrimaryDataSource ));

	}
	float power = RelativeStartTime*1.0/g_ImpactStudyPeriodInMin*100;
	if(power>100)
		power = 100;

	LOS = GetLOSCode(power);

	return total_delay/max(1,pToNode->m_IncomingLinkVector.size());

}



void CTLiteDoc::OnGisplanningdatasetConfigureimportingsettingfile()
{

	//CDlg_DataImportWizard dlgWizard;
	//dlgWizard.DoModal();

	AfxMessageBox("NEXTA 64-bit version does not support shape file importing function. Please use NEXTA_32.exe. ");
	return;



	// TODO: Add your command handler code here
}


void CTLiteDoc::OnSubareaExporttotalnumberofAgentsinsubarea()
{

	CString file_name  = m_ProjectDirectory+"Export_total_number_of_Agents_in_subarea.csv";
	FILE* pFile;
	fopen_s(&pFile,file_name,"w");
	if(pFile!=NULL)
	{
		fprintf(pFile, "time_in_min,time_clock,total_number_of_Agents_in_subarea,avg_speed_in_mile,avg_speed_in_km\n");
		for(int t = 0; t< 1440; t++)
		{

			float total_number_of_Agents = 0;

			float total_speed_value = 0;

				for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				if((*iLink)->m_bIncludedinSubarea && m_SubareaShapePoints.size()>=3)
				{

					float number_of_Agents = (*iLink)->GetSimulatedDensity (t)*(*iLink)->m_Length * (*iLink)->m_NumberOfLanes;
				
					total_number_of_Agents += number_of_Agents;
					total_speed_value+= (*iLink)->GetSimulatedSpeed (t) * number_of_Agents;
				
				}


			}

				float avg_speed = total_speed_value/max(1,total_number_of_Agents);
				float avg_speed_in_km = avg_speed*1.609344;


				fprintf(pFile,"%d,%s,%.0f,%.2f,%.2f\n",t,GetTimeStampString24HourFormat (t),total_number_of_Agents,avg_speed,avg_speed_in_km);

		}

		fclose(pFile);

	this->OpenCSVFileInExcel (file_name);

	}

}



void CTLiteDoc::OnToolsConfiguration()
{

}








void CTLiteDoc::OnPathClearallpathdisplay()
{
	for (unsigned int p = 0; p < m_PathDisplayList.size(); p++) // for each path
	{
		m_PathDisplayList[p].m_LinkVector.clear();
	}
	m_PathDisplayList.clear();

	m_FromNodeID = -1;
	m_ToNodeID = -1;

	UpdateAllViews(0);
}



void CTLiteDoc::SaveTimingData()
{
	CString directory;
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	CString AMS_File = directory + "input_timing.csv";
	FILE* st = NULL;

	fopen_s(&st, AMS_File, "w");
	if (st != NULL)
	{
		fprintf(st, "int_id,timing_plan_no,phase_id,next_phase_id,green_duration,movement_str,movement_dir_str,movement_sim_hourly_volume_delay_in_sec,\n");

		// print out matrix 
		for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if (((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal))
			{


				CString timing_plan_name;

				int p;
				for (int timeplan = 1; timeplan < 20; timeplan++)
				{

					timing_plan_name.Format("%d", timeplan);

					for (p = 1; p < 20; p++)
					{

						//refetch data 
						DTA_Phasing_Data_Matrix element = GetPhaseData((*iNode)->m_NodeID, timing_plan_name.GetString());

						if (element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_GreenDuration).GetLength() > 0 ||
							element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_VECTOR).GetLength() > 0
							)
						{
							fprintf(st, "%d,%d,%d,%s,%s,%s,%s,",
								(*iNode)->m_NodeID,
								timeplan, p,
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_NextPhaseNo).GetString(),
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_GreenDuration).GetString(),
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_VECTOR).GetString(),
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_DIR_VECTOR).GetString());

							string movement_str = element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_VECTOR).GetString();

							CString movement_MOE_str;
							if (movement_str.size() > 0)
							{
								for (unsigned int m = 0; m < (*iNode)->m_MovementDataMap.m_MovementVector.size(); m++)
								{
									bool bMovementIncluded = IfMovementDirIncludedInPhase((*iNode)->m_NodeID,
										timing_plan_name.GetString(), p, m);

									if (bMovementIncluded)
									{

										CString sub_movement_str;

										float number_of_hours = max(0.01, (m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin) / 60.0);

										float sim_turn_hourly_count = (*iNode)->m_MovementDataMap.m_MovementVector[m].sim_turn_count / number_of_hours;

										if(sim_turn_hourly_count>1)
										{ 
										sub_movement_str.Format(";%d_%d_%s:%.1f:%.1f", m_NodeNoMap[(*iNode)->m_MovementDataMap.m_MovementVector[m].in_link_from_node_id]->m_NodeID,
											m_NodeNoMap[(*iNode)->m_MovementDataMap.m_MovementVector[m].out_link_to_node_id]->m_NodeID,
											GetTurnShortString((*iNode)->m_MovementDataMap.m_MovementVector[m].movement_turn),
											sim_turn_hourly_count,
											(*iNode)->m_MovementDataMap.m_MovementVector[m].sim_turn_delay*60);

										movement_MOE_str += sub_movement_str;
										}


									}

								}


								//
							}

							fprintf(st, "%s,", movement_MOE_str);
							fprintf(st, "\n");
						}


					} //p
				}

			}
		}

			fclose(st);
		}
	else
	{
		AfxMessageBox("File input_timing_backup.csv cannot be opened.");
	}

	}



void CTLiteDoc::OnToolsImportsynchroncombinedfile()
{
	// TODO: Add your command handler code here
}


void CTLiteDoc::OnOdmatrixRapid()
{
	CWaitCursor ws;
	m_TrafficFlowModelFlag = 0;
	m_DemandAlpha += 0.1;
	

	m_LinkBandWidthMode = LBW_link_volume;
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}


void CTLiteDoc::OnOdmatrixRapidDecrease()
{

	CWaitCursor ws;
	m_TrafficFlowModelFlag = 0;
	m_DemandAlpha -= 0.1;

	m_LinkBandWidthMode = LBW_link_volume;
	GenerateOffsetLinkBand();

	UpdateAllViews(0);
}


void CTLiteDoc::OnButtonAbm()
{

	CWaitCursor curs;
	if (!SetCurrentDirectory(m_ProjectDirectory))
	{
		if (m_ProjectDirectory.GetLength() == 0)
		{
			AfxMessageBox("The project directory has not been specified.");
			return;
		}
		CString msg;
		msg.Format("The specified project directory %s does not exist.", m_ProjectDirectory);
		AfxMessageBox(msg);
		return;
	}
	CString sCommand;
	CString strParam;
	CTime ExeStartTime = CTime::GetCurrentTime();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	sCommand.Format("%s\\%s", pMainFrame->m_CurrentDirectory, "AgBM.exe");

	ProcessExecute(sCommand, strParam, m_ProjectDirectory, true);

}


void CTLiteDoc::OnHelpVisitdevelopmentwebsiteDtalite()
{
	g_OpenDocument("https://github.com/xzhou99/dtalite_software_release", SW_SHOW);
}
