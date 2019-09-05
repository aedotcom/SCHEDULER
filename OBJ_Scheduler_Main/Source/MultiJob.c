#include "default.h"

//========================================================================================================================
// SEMI E5-0301   SEMI Equipment Communications Standard 2 Message Content (SECS-II) 
// SEMI E30-1000  Generic Model for Communications and Control of Manufacturing Equipment (GEM) 
// SEMI E37-0298  High-Speed SECS Message Services (HSMS) Generic Services 
// SEMI E37.1-96E High-Speed SECS Message Services Single - Session Mode (HSMS-SS) 
// SEMI E39-0600  Object Services Standard: Concepts, Behavior, and Services (OSS)
// SEMI E40-0301  Standard for Processing Management (PJM)
// SEMI E42-0299E Recipe Management Standard: Concepts, Behavior, and Service(RMS)
// SEMI E84-0301  Specification for Enhanced Carrier Handoff Parallel I/O Interface(PIO)
// SEMI E87-0301  Provisional Specification for Carrier Management (CMS)
// SEMI E90-0301  Specification for Substrate Tracking(STS)
// SEMI E94-1000  Provisional Specification for Control Job Management(CJM)
// SEMI E99-1000E The Carrier ID Reader/Writer Functional Standard: Specification of Concepts, Behavior, and Services
//========================================================================================================================
//
// CR 위험성 수정 API : 2013.02.13
//
//	SCHMULTI_RUNJOB_GET_TUNING_INFO
//	SCHMULTI_RUNJOB_DELETE_TUNING_INFO
//
//	_i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE
//	_i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK
//
// CR 위험성 있는 API : 2013.02.13
//
//		SCHMULTI_JOB_OVERTIME_DELETE


//
//
//	_i_SCH_PRM_GET_MTLOUT_INTERFACE()
//
//
//		DATA_MOVE	PICK_FROM_FM(POSSIBLE_CHECK)	AFTER_PICK_CM	AFTER_PLACE_CM	SCH_PATHOUT_RESET
//
//	0	X			X								X				X				X
//	1	O			X								X				UNLOAD			O
//	2	X			O								X				UNLOAD			O
//	3	X			O								UNLOAD			UNLOAD			O
//
//
//																				0		1		2		3
//
//	AFTER_PLACE_CM		ACTION	SCHMRDATA_Data_IN_Unload_Request_After_Place	UNLOAD	x		x		x
//
//	SCH_PATHOUT_RESET	ACTION	SCH_PATHOUT_RESET								x		O		O		O
//
//	PICK_FROM_FM		CHECK	SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS_IN		x		x		O		O
//
//	AFTER_PICK_CM		ACTION	SCHMULTI_CHECK_HANDOFF_OUT_CHECK				x		x		x		UNLOAD
//
//	AFTER_PLACE_CM		ACTION	SCHMULTI_CHECK_HANDOFF_OUT_CHECK				x		UNLOAD	UNLOAD	UNLOAD
//
//

#include "EQ_Enum.h"

#include "Multijob.h"
#include "MR_Data.h"
#include "DLL_Interface.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "system_tag.h"
#include "sch_main.h"
#include "sch_cassmap.h"
#include "sch_sub.h"
#include "sch_prm.h"
#include "sch_prm_cluster.h"
#include "sch_prm_Log.h"
#include "sch_prepost.h"
#include "sch_processmonitor.h"
#include "Multireg.h"
#include "RcpFile_Handling.h"
#include "FA_Handling.h"
#include "Gui_Handling.h"
#include "Errorcode.h"
#include "Resource.h"
#include "sch_HandOff.h"
#include "Utility.h"
#include "iolog.h"

#include "INF_sch_Commonuser.h"


int SCHEDULER_Get_CARRIER_INDEX( int Side ); // 2018.05.24


//void SCHEDULER_Make_CARRIER_INDEX( int side ); // 2011.09.14

//---------------------------------------------------------------------------------------------------------------
#define JOB_OVERTIME_CHECK_PERIOD		60		// min
#define JOB_OVERTIME_CHECK_HOUR_SEC		3600	// 1hour
//---------------------------------------------------------------------------------------------------------------
#define	MAX_MULTI_RUNJOB_SIZE	MAX_MULTI_QUEUE_SIZE
//---------------------------------------------------------------------------------------------------------------

enum { CASSETTE_VERIFY_READY , CASSETTE_VERIFY_WAIT_CONFIRM , CASSETTE_VERIFY_CONFIRMING , CASSETTE_VERIFY_CONFIRMED , CASSETTE_VERIFY_CONFIRMED2 , CASSETTE_VERIFY_RUNNING }; 
enum { PJ_SIGNAL_RUN , PJ_SIGNAL_WAIT , PJ_SIGNAL_FINISH , PJ_SIGNAL_PAUSE_RUN , PJ_SIGNAL_PAUSE_WAIT , PJ_SIGNAL_PAUSE_FINISH }; 

enum { PJIDRES_READY , PJIDRES_FIRST , PJIDRES_RUNNING , PJIDRES_LAST , PJIDRES_FINISH }; 

//---------------------------------------------------------------------------------------------------------------
CRITICAL_SECTION CR_MJ_LOG_MSG_QUEUE;

#define MJ_LOG_MSG_QUEUE_SIZE	256

int	  MJ_LOG_MSG_QUEUE_Index = 0;
char *MJ_LOG_MSG_QUEUE_Index_Time[ MJ_LOG_MSG_QUEUE_SIZE ];
char *MJ_LOG_MSG_QUEUE_Index_Data[ MJ_LOG_MSG_QUEUE_SIZE ];

//---------------------------------------------------------------------------------------------------------------
//BOOL MID_READ_COUNT[MAX_CASSETTE_SIDE]; // 2006.12.05 // 2010.05.06
int  PRE_RUN_POSSIBLE_CHECK[MAX_CASSETTE_SIDE]; // 2004.06.16 // 2006.12.05
int  STATUS_BUTTON_CHECK[MAX_CASSETTE_SIDE]; // 2011.04.14
//---------------------------------------------------------------------------------------------------------------
// 2012.04.22
char *GUI_INSERT_PROCESSJOB_NAME;
int   GUI_INSERT_PROCESSJOB_STARTMODE;
int   GUI_INSERT_PROCESSJOB_STARTSLOT;
int   GUI_INSERT_PROCESSJOB_ENDSLOT;
char *GUI_INSERT_PROCESSJOB_RECIPE;
char *GUI_INSERT_PROCESSJOB_CARRIERID;

char *GUI_INSERT_CONTROLJOB_NAME;
int   GUI_INSERT_CONTROLJOB_STARTMODE;
char *GUI_INSERT_CONREOLJOB_PRJOB_NAME[ MAX_MULTI_CTJOB_CASSETTE ]; // 2012.04.22
char *GUI_INSERT_CONREOLJOB_PRJOB_NAME2[ MAX_MULTI_CTJOB_CASSETTE ]; // 2014.02.03
//---------------------------------------------------------------------------------------------------------------

CRITICAL_SECTION CR_MULTIJOB;

Scheduling_Multi_ProcessJob	Sch_Multi_ProcessJob_Display_Buffer;
Scheduling_Multi_ControlJob	Sch_Multi_ControlJob_Display_Buffer; // 2012.09.27

int	Sch_Multi_RunJob_Select_Side[ MAX_MULTI_RUNJOB_SIZE ]; // 2018.08.30

#define MULTI_RUNJOB_RunSide( i )					Sch_Multi_RunJob_Select_Side[i] // 2018.08.30

#ifdef _SCH_MIF										// SchJobIntf2.lib

	//==================================================
	// EXTJOB(NEW)
	//
	#define MULTI_RUNJOB_INIT( i )					memset( Sch_Multi_Running_Select_Data[i] , 0 , sizeof(Scheduling_Multi_RunData) );

	#define MULTI_RUNJOB_RunStatus( i )				Sch_Multi_Running_Select_Data[i]->RunStatus
	#define MULTI_RUNJOB_Cassette( i )				Sch_Multi_Running_Select_Data[i]->Cassette
	#define MULTI_RUNJOB_SelectCJIndex( i )			Sch_Multi_Running_Select_Data[i]->SelectCJIndex
	#define MULTI_RUNJOB_SelectCJOrder( i )			Sch_Multi_Running_Select_Data[i]->SelectCJOrder
	#define MULTI_RUNJOB_CarrierID( i )				Sch_Multi_Running_Select_Data[i]->CarrierID
	#define MULTI_RUNJOB_MtlCJ_StartMode( i )		Sch_Multi_Running_Select_Data[i]->MtlCJ_StartMode
	#define MULTI_RUNJOB_MtlCJName( i )				Sch_Multi_Running_Select_Data[i]->MtlCJName
	#define MULTI_RUNJOB_CassetteIndex( i )			Sch_Multi_Running_Select_Data[i]->CassetteIndex

	#define MULTI_RUNJOB_MtlCount( i )				Sch_Multi_Running_Select_Data[i]->MtlCount

	#define MULTI_RUNJOB_Mtl_Side( i )				Sch_Multi_Running_Select_Data[i]->Mtl_Side
	#define MULTI_RUNJOB_Mtl_Pointer( i )			Sch_Multi_Running_Select_Data[i]->Mtl_Pointer

	#define MULTI_RUNJOB_MtlPJ_StartMode( i )		Sch_Multi_Running_Select_Data[i]->MtlPJ_StartMode
	#define MULTI_RUNJOB_MtlPJName( i )				Sch_Multi_Running_Select_Data[i]->MtlPJName
	#define MULTI_RUNJOB_MtlPJID( i )				Sch_Multi_Running_Select_Data[i]->MtlPJID
	#define MULTI_RUNJOB_MtlPJIDRes( i )			Sch_Multi_Running_Select_Data[i]->MtlPJIDRes
	#define MULTI_RUNJOB_MtlPJIDEnd( i )			Sch_Multi_Running_Select_Data[i]->MtlPJIDEnd
	#define MULTI_RUNJOB_MtlRecipeName( i )			Sch_Multi_Running_Select_Data[i]->MtlRecipeName
	#define MULTI_RUNJOB_MtlInCassette( i )			Sch_Multi_Running_Select_Data[i]->MtlInCassette
	#define MULTI_RUNJOB_MtlInCarrierID( i )		Sch_Multi_Running_Select_Data[i]->MtlInCarrierID
	#define MULTI_RUNJOB_MtlInCarrierIndex( i )		Sch_Multi_Running_Select_Data[i]->MtlInCarrierIndex
	#define MULTI_RUNJOB_MtlInSlot( i )				Sch_Multi_Running_Select_Data[i]->MtlInSlot
	#define MULTI_RUNJOB_MtlOutCassette( i )		Sch_Multi_Running_Select_Data[i]->MtlOutCassette
	#define MULTI_RUNJOB_MtlOutCarrierID( i )		Sch_Multi_Running_Select_Data[i]->MtlOutCarrierID
	#define MULTI_RUNJOB_MtlOutCarrierIndex( i )	Sch_Multi_Running_Select_Data[i]->MtlOutCarrierIndex
	#define MULTI_RUNJOB_MtlOutSlot( i )			Sch_Multi_Running_Select_Data[i]->MtlOutSlot
	#define MULTI_RUNJOB_MtlRecipeMode( i )			Sch_Multi_Running_Select_Data[i]->MtlRecipeMode

#else

	#ifdef PM_CHAMBER_USER							// SchJobIntf2.lib

		//==================================================
		// EXTJOB(NEW)
		//
		#define MULTI_RUNJOB_INIT( i )					memset( Sch_Multi_Running_Select_Data[i] , 0 , sizeof(Scheduling_Multi_RunData) );

		#define MULTI_RUNJOB_RunStatus( i )				Sch_Multi_Running_Select_Data[i]->RunStatus
		#define MULTI_RUNJOB_Cassette( i )				Sch_Multi_Running_Select_Data[i]->Cassette
		#define MULTI_RUNJOB_SelectCJIndex( i )			Sch_Multi_Running_Select_Data[i]->SelectCJIndex
		#define MULTI_RUNJOB_SelectCJOrder( i )			Sch_Multi_Running_Select_Data[i]->SelectCJOrder
		#define MULTI_RUNJOB_CarrierID( i )				Sch_Multi_Running_Select_Data[i]->CarrierID
		#define MULTI_RUNJOB_MtlCJ_StartMode( i )		Sch_Multi_Running_Select_Data[i]->MtlCJ_StartMode
		#define MULTI_RUNJOB_MtlCJName( i )				Sch_Multi_Running_Select_Data[i]->MtlCJName
		#define MULTI_RUNJOB_CassetteIndex( i )			Sch_Multi_Running_Select_Data[i]->CassetteIndex

		#define MULTI_RUNJOB_MtlCount( i )				Sch_Multi_Running_Select_Data[i]->MtlCount

		#define MULTI_RUNJOB_Mtl_Side( i )				Sch_Multi_Running_Select_Data[i]->Mtl_Side
		#define MULTI_RUNJOB_Mtl_Pointer( i )			Sch_Multi_Running_Select_Data[i]->Mtl_Pointer

		#define MULTI_RUNJOB_MtlPJ_StartMode( i )		Sch_Multi_Running_Select_Data[i]->MtlPJ_StartMode
		#define MULTI_RUNJOB_MtlPJName( i )				Sch_Multi_Running_Select_Data[i]->MtlPJName
		#define MULTI_RUNJOB_MtlPJID( i )				Sch_Multi_Running_Select_Data[i]->MtlPJID
		#define MULTI_RUNJOB_MtlPJIDRes( i )			Sch_Multi_Running_Select_Data[i]->MtlPJIDRes
		#define MULTI_RUNJOB_MtlPJIDEnd( i )			Sch_Multi_Running_Select_Data[i]->MtlPJIDEnd
		#define MULTI_RUNJOB_MtlRecipeName( i )			Sch_Multi_Running_Select_Data[i]->MtlRecipeName
		#define MULTI_RUNJOB_MtlInCassette( i )			Sch_Multi_Running_Select_Data[i]->MtlInCassette
		#define MULTI_RUNJOB_MtlInCarrierID( i )		Sch_Multi_Running_Select_Data[i]->MtlInCarrierID
		#define MULTI_RUNJOB_MtlInCarrierIndex( i )		Sch_Multi_Running_Select_Data[i]->MtlInCarrierIndex
		#define MULTI_RUNJOB_MtlInSlot( i )				Sch_Multi_Running_Select_Data[i]->MtlInSlot
		#define MULTI_RUNJOB_MtlOutCassette( i )		Sch_Multi_Running_Select_Data[i]->MtlOutCassette
		#define MULTI_RUNJOB_MtlOutCarrierID( i )		Sch_Multi_Running_Select_Data[i]->MtlOutCarrierID
		#define MULTI_RUNJOB_MtlOutCarrierIndex( i )	Sch_Multi_Running_Select_Data[i]->MtlOutCarrierIndex
		#define MULTI_RUNJOB_MtlOutSlot( i )			Sch_Multi_Running_Select_Data[i]->MtlOutSlot
		#define MULTI_RUNJOB_MtlRecipeMode( i )			Sch_Multi_Running_Select_Data[i]->MtlRecipeMode

	#else											// SchJobIntf.lib

		//==================================================
		// EXTJOB(OLD)
		//

		Scheduling_Multi_RunJob		Sch_Multi_RunJob_Select_Data[ MAX_MULTI_RUNJOB_SIZE ];

		int	_i_RunJob_Select_Data_CassetteIndex[ MAX_MULTI_RUNJOB_SIZE ]; // 2018.09.06
		//
		int	_i_RunJob_Select_Data_Side[ MAX_MULTI_RUNJOB_SIZE ][MAX_RUN_CASSETTE_SLOT_SIZE]; // 2018.09.06
		int	_i_RunJob_Select_Data_Pointer[ MAX_MULTI_RUNJOB_SIZE ][MAX_RUN_CASSETTE_SLOT_SIZE]; // 2018.09.06
		int	_i_RunJob_Select_Data_InCarrierIndex[ MAX_MULTI_RUNJOB_SIZE ][MAX_RUN_CASSETTE_SLOT_SIZE]; // 2018.09.06
		int	_i_RunJob_Select_Data_OutCarrierIndex[ MAX_MULTI_RUNJOB_SIZE ][MAX_RUN_CASSETTE_SLOT_SIZE]; // 2018.09.06

		#define MULTI_RUNJOB_INIT( i )					memset( &(Sch_Multi_RunJob_Select_Data[i]) , 0 , sizeof(Scheduling_Multi_RunJob) );

		#define MULTI_RUNJOB_RunStatus( i )				Sch_Multi_RunJob_Select_Data[i].RunStatus
		#define MULTI_RUNJOB_Cassette( i )				Sch_Multi_RunJob_Select_Data[i].Cassette
		#define MULTI_RUNJOB_SelectCJIndex( i )			Sch_Multi_RunJob_Select_Data[i].SelectCJIndex
		#define MULTI_RUNJOB_SelectCJOrder( i )			Sch_Multi_RunJob_Select_Data[i].SelectCJOrder
		#define MULTI_RUNJOB_CarrierID( i )				Sch_Multi_RunJob_Select_Data[i].CarrierID
		#define MULTI_RUNJOB_MtlCJ_StartMode( i )		Sch_Multi_RunJob_Select_Data[i].MtlCJ_StartMode
		#define MULTI_RUNJOB_MtlCJName( i )				Sch_Multi_RunJob_Select_Data[i].MtlCJName
		#define MULTI_RUNJOB_CassetteIndex( i )			_i_RunJob_Select_Data_CassetteIndex[i]

		#define MULTI_RUNJOB_MtlCount( i )				Sch_Multi_RunJob_Select_Data[i].MtlCount

		#define MULTI_RUNJOB_Mtl_Side( i )				_i_RunJob_Select_Data_Side[i]
		#define MULTI_RUNJOB_Mtl_Pointer( i )			_i_RunJob_Select_Data_Pointer[i]

		#define MULTI_RUNJOB_MtlPJ_StartMode( i )		Sch_Multi_RunJob_Select_Data[i].MtlPJ_StartMode
		#define MULTI_RUNJOB_MtlPJName( i )				Sch_Multi_RunJob_Select_Data[i].MtlPJName
		#define MULTI_RUNJOB_MtlPJID( i )				Sch_Multi_RunJob_Select_Data[i].MtlPJID
		#define MULTI_RUNJOB_MtlPJIDRes( i )			Sch_Multi_RunJob_Select_Data[i].MtlPJIDRes
		#define MULTI_RUNJOB_MtlPJIDEnd( i )			Sch_Multi_RunJob_Select_Data[i].MtlPJIDEnd
		#define MULTI_RUNJOB_MtlRecipeName( i )			Sch_Multi_RunJob_Select_Data[i].MtlRecipeName
		#define MULTI_RUNJOB_MtlInCassette( i )			Sch_Multi_RunJob_Select_Data[i].MtlInCassette
		#define MULTI_RUNJOB_MtlInCarrierID( i )		Sch_Multi_RunJob_Select_Data[i].MtlInCarrierID
		#define MULTI_RUNJOB_MtlInCarrierIndex( i )		_i_RunJob_Select_Data_InCarrierIndex[i]
		#define MULTI_RUNJOB_MtlInSlot( i )				Sch_Multi_RunJob_Select_Data[i].MtlInSlot
		#define MULTI_RUNJOB_MtlOutCassette( i )		Sch_Multi_RunJob_Select_Data[i].MtlOutCassette
		#define MULTI_RUNJOB_MtlOutCarrierID( i )		Sch_Multi_RunJob_Select_Data[i].MtlOutCarrierID
		#define MULTI_RUNJOB_MtlOutCarrierIndex( i )	_i_RunJob_Select_Data_OutCarrierIndex[i]
		#define MULTI_RUNJOB_MtlOutSlot( i )			Sch_Multi_RunJob_Select_Data[i].MtlOutSlot
		#define MULTI_RUNJOB_MtlRecipeMode( i )			Sch_Multi_RunJob_Select_Data[i].MtlRecipeMode

	#endif

#endif

//

//===================================================================================
Scheduling_Multi_Ins_ProcessJob	Sch_Multi_Ins_ProcessJob_Display_Buffer; // 2012.04.19
Scheduling_Multi_Ins_ControlJob	Sch_Multi_Ins_ControlJob_Display_Buffer; // 2012.09.27
//
Scheduling_Multi_Ins_ProcessJob	Sch_Multi_Ins_ProcessJob[ MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ]; // 2012.04.19
Scheduling_Multi_Ins_ControlJob	Sch_Multi_Ins_ControlJob[ MAX_MULTI_QUEUE_SIZE ]; // 2012.04.19

Scheduling_Multi_Ins_ControlJob	Sch_Multi_Ins_ControlJob_Select_Data[ MAX_MULTI_CTJOBSELECT_SIZE ]; // 2012.04.19

//===================================================================================
int		Sch_Multi_PRJOB_Buf_Sub_Error = 0;
int		Sch_Multi_CJJOB_Buf_Sub_Error = 0;

int		Sch_Multi_Cassette_Verify[MAX_CASSETTE_SIDE];
//
int		Sch_Multi_ControlJob_Selected_Pause_Signal[ MAX_MULTI_CTJOBSELECT_SIZE ];
int		Sch_Multi_ControlJob_Selected_Stop_Signal[ MAX_MULTI_CTJOBSELECT_SIZE ]; // 2002.05.20
//
//char	Sch_Multi_Cassette_MID[MAX_CASSETTE_SIDE][127+1]; // 2008.04.02
char	*Sch_Multi_Cassette_MID[MAX_CASSETTE_SIDE]; // 2008.04.02
//
char	*Sch_Multi_Cassette_MID_for_Start[MAX_CASSETTE_SIDE]; // 2011.03.10
//------------------------------------------------------------------------------------------
int Address_IO_CTJob_Remain;
int Address_IO_PRJob_Remain;
//
BOOL SCHMULTI_USABLE = FALSE;
BOOL SCHMULTI_SINGLE_MODE = FALSE;

BOOL SCHMULTI_CURRENT_MANUAL_MODE;

char SCHMULTI_DEFAULT_GROUP[65];

int SCHMULTI_DEFAULT_LOTRECIPE_READ = 0; // 2003.09.24

int SCHMULTI_MAKE_MANUAL_CJPJJOBMODE = 0; // 2004.05.11

int SCHMULTI_JOBNAME_DISPLAYMODE = 0; // 2005.05.11

int SCHMULTI_MESSAGE_SCENARIO = 0; // 2006.04.13

int SCHMULTI_RCPTUNE_EXTEND_USE = 0; // 2007.11.28

int SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_HH = 0; // 2004.05.11
int SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_MM = 0; // 2004.05.11
int SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_SS = 0; // 2004.05.11
int SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC = 0; // 2004.05.11

int SCHMULTI_AFTER_SELECT_CHECK = 0; // 2004.06.26

char *SCHMULTI_TUNE_INFO_DATA[ MAX_CHAMBER ]; // 2007.06.08

int SCHMULTI_CHECK_OVERTIME_HOUR = 0; // 2012.04.20

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_PROCESSJOB_SET_STATUS( int mode , char *name , int data );
int  SCHMULTI_PROCESSJOB_GET_STATUS( int mode , char *name );
void SCHMULTI_CONTROLJOB_SET_STATUS( int mode , char *name , int data );



int USER_FIXED_JOB_OUT_CASSETTE = -1; // 2018.07.17


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHMULTI_RCPLOCKING_GET_FILENAME( int mode , int fileorg , char *pjname , char *RunGroup , char *rcpName , char *retname , int cnt ) {
	char BufferN[511+1];
	char BufferP[511+1]; // 2012.06.10
	//
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	//
	if ( mode == -2 ) { // 2013.09.13
		//
		if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() <= 2 ) { // 2015.07.20
			//
//			sprintf( BufferN , "%s/tmp" , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2017.10.30
			sprintf( BufferN , "%s/tmp" , _i_SCH_PRM_GET_DFIX_TMP_PATH() ); // 2017.10.30
			SetFileAttributes( BufferN , FILE_ATTRIBUTE_HIDDEN );
			//
		}
		return TRUE;
	}
	//
	if ( mode == -1 ) {
//		sprintf( BufferN , "%s/tmp" , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2012.06.10
//		_mkdir( BufferN ); // 2012.06.10
		//
		//
		if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() <= 2 ) { // 2015.07.20
			//
			Directory_Make_Prepare_for_Filename( retname ); // 2012.06.10
			//
		}
		return TRUE;
	}
	//
	switch( mode ) {
	case 0 : // lot
		if ( ( fileorg == 0 ) || ( fileorg == 2 ) ) { // org
			_snprintf( retname , cnt , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() , RunGroup , _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH() , rcpName );
		}
		else { // tmp
//			_snprintf( BufferN , 511 , "%s$LOT$%s$%s$%s" , pjname , RunGroup , _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH() , rcpName ); // 2012.06.10
			//
			// 2012.06.10
			strcpy( BufferP , pjname );
//			_snprintf(BufferN, 511, "$LOT$%s$", RunGroup, _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH()); // 2019.03.07
			_snprintf( BufferN , 511 , "$LOT$%s$" , RunGroup ); // 2019.03.07
			//
			Change_Dirsep_to_Dollar( BufferP );
			Change_Dirsep_to_Dollar( BufferN );
			//
//			_snprintf( retname , cnt , "%s/tmp/JOB%s/%s/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferP , BufferN , rcpName ); // 2017.10.30
			_snprintf( retname , cnt , "%s/tmp/JOB%s/%s/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferP , BufferN , rcpName ); // 2017.10.30
		}
		break;
	case 1 : // cluster
		if ( ( fileorg == 0 ) || ( fileorg == 2 ) ) { // org
			_snprintf( retname , cnt , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() , RunGroup , _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() , rcpName );
		}
		else { // tmp
//			_snprintf( BufferN , 511 , "%s$CLUSTER$%s$%s$%s" , pjname , RunGroup , _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() , rcpName ); // 2012.06.10
			//
			// 2012.06.10
			strcpy( BufferP , pjname );
//			_snprintf(BufferN, 511, "$CLUSTER$%s$", RunGroup, _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH()); // 2019.03.07
			_snprintf( BufferN , 511 , "$CLUSTER$%s$" , RunGroup ); // 2019.03.07
			//
			Change_Dirsep_to_Dollar( BufferP );
			Change_Dirsep_to_Dollar( BufferN );
			//
//			_snprintf( retname , cnt , "%s/tmp/JOB%s/%s/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferP , BufferN , rcpName ); // 2017.10.30
			_snprintf( retname , cnt , "%s/tmp/JOB%s/%s/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferP , BufferN , rcpName ); // 2017.10.30
		}
		break;
	default : // mdl
		if ( ( fileorg == 0 ) || ( fileorg == 2 ) ) { // org
			_snprintf( retname , cnt , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( mode ) , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( mode ) , rcpName );
		}
		else { // tmp
//			_snprintf( BufferN , 511 , "%s$MDL%d$%s$%s$%s" , pjname , mode , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( mode ) , rcpName ); // 2012.06.10
			//
			// 2012.06.10
			strcpy( BufferP , pjname );
//			_snprintf(BufferN, 511, "$MDL%d$%s$", mode, RunGroup, _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF(mode)); // 2019.03.07
			_snprintf( BufferN , 511 , "$MDL%d$%s$" , mode , RunGroup ); // 2019.03.07
			//
			Change_Dirsep_to_Dollar( BufferP );
			Change_Dirsep_to_Dollar( BufferN );
			//
//			_snprintf( retname , cnt , "%s/tmp/JOB%s/%s/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferP , BufferN , rcpName ); // 2017.10.30
			_snprintf( retname , cnt , "%s/tmp/JOB%s/%s/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferP , BufferN , rcpName ); // 2017.10.30
		}
		break;
	}
	//
	/*
	// 2012.06.10
	if ( ( fileorg != 0 ) && ( fileorg != 2 ) ) { // tmp
		//
		Change_Dirsep_to_Dollar( BufferN );
		//
		_snprintf( retname , cnt , "%s/tmp/JOB%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferN );
		//
	}
	*/
	if ( ( fileorg == 2 ) || ( fileorg == 3 ) ) {
		//
		hFind = FindFirstFile( retname , &fd );
		//
		if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
		//
		FindClose( hFind );
		//
	}
	//
	return TRUE;
}

int SCHMULTI_RCPLOCKING_FILECOPY_Sub( char *pjname , int mode , char *RunGroup , char *rcpName ) { // 2011.09.20
	char BufferS[511+1];
	char BufferT[511+1];
	int errorcode; // 2014.09.15
	int rcpName_SHPT;
	//
	rcpName_SHPT = 0;
	if ( ( rcpName[0] == '+' ) && ( ( rcpName[1] == '\\' ) || ( rcpName[1] == '/' ) ) ) rcpName_SHPT = 2; // 2015.07.20
	//
	if ( mode > 1 ) {
		if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( mode ) != 0 ) return 0;
	}
	//
	if ( !SCHMULTI_RCPLOCKING_GET_FILENAME( mode , 2 , pjname , RunGroup , rcpName + rcpName_SHPT , BufferS , 511 ) ) {
//printf( "SCHMULTI_RCPLOCKING_GET_FILENAME Fail [%s] [%s][%d][%s][%s] [%d]\n" , BufferS , pjname , mode , RunGroup , rcpName , 2 );
		//
		_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_FILECOPY_Sub ERROR => SCHMULTI_RCPLOCKING_GET_FILENAME[2] [S=%s] [PJ=%s][mode=%d][RunGroup=%s][rcpName=%s]\n" , BufferS , pjname , mode , RunGroup , rcpName + rcpName_SHPT ); // 2015.03.27
		//
		return 1;
	}
	//
//	SCHMULTI_RCPLOCKING_GET_FILENAME( -1 , 0 , NULL , NULL , NULL , NULL , 0 ); // 2012.06.10
	//
	if ( !SCHMULTI_RCPLOCKING_GET_FILENAME( mode , 1 , pjname , RunGroup , rcpName + rcpName_SHPT , BufferT , 511 ) ) {
//printf( "SCHMULTI_RCPLOCKING_GET_FILENAME Fail [%s] [%s][%d][%s][%s] [%d]\n" , BufferT , pjname , mode , RunGroup , rcpName , 1 );
		//
		_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_FILECOPY_Sub ERROR => SCHMULTI_RCPLOCKING_GET_FILENAME[1] [T=%s] [PJ=%s][mode=%d][RunGroup=%s][rcpName=%s]\n" , BufferT , pjname , mode , RunGroup , rcpName + rcpName_SHPT ); // 2015.03.27
		//
		return 2;
	}
	//
	SCHMULTI_RCPLOCKING_GET_FILENAME( -1 , 0 , NULL , NULL , NULL , BufferT , 0 ); // 2012.06.10
	//
	/*
	//
	// 2014.09.25
	//
	if ( !CopyFile( BufferS , BufferT , FALSE ) ) {
//printf( "SCHMULTI_RCPLOCKING_GET_FILENAME Fail [%s][%s] [%s][%d][%s][%s]\n" , BufferS , BufferT , pjname , mode , RunGroup , rcpName );
		//
		DeleteFile( BufferT ); // 2013.09.13
		//
		if ( !CopyFile( BufferS , BufferT , FALSE ) ) { // 2013.09.13
			return 3;
		}
	}
	//
	SetFileAttributes( BufferT , FILE_ATTRIBUTE_HIDDEN ); // 2013.09.13
	//
	*/
	//
	// 2014.09.15
	//
	if ( !UTIL_CopyFile( ( rcpName_SHPT > 0 ) , BufferS , ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() >= 3 ) , BufferT , FALSE , 3 , TRUE , &errorcode ) ) { // 2014.09.15
		//
		_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_FILECOPY_Sub ERROR => UTIL_CopyFile(S=%d:%s,T=%s,ERR=%d)\n" , ( rcpName_SHPT > 0 ) , BufferS , BufferT , errorcode );
		//
		return 3;
		//
	}
	//
	//
	return 0;
}


BOOL SCHMULTI_RCPLOCKING_FILECOPY( char *pjname , int mode , int mode2 , char *RunGroup , char *rcpName ) { // 2011.09.20
	int z1 , z2  , Res;
	char Retdata[511+1];
	//
	z1 = 0;
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( rcpName , '|' , Retdata , z1 , 511 );
	if ( z1 == z2 ) return TRUE;
	//
	do {
		if ( Retdata[0] != 0 ) { // 2012.05.25
//			if ( !SCHMULTI_RCPLOCKING_FILECOPY_Sub( pjname , mode , RunGroup , Retdata ) ) {
			Res = SCHMULTI_RCPLOCKING_FILECOPY_Sub( pjname , mode , RunGroup , Retdata );
			//
			if ( Res != 0 ) {
//printf( "SCHMULTI_RCPLOCKING_FILECOPY_Sub Fail [%d] [%s] [%s][%d:%d][%s][%s] [%d]\n" , z1 , Retdata , pjname , mode , mode2 , RunGroup , rcpName , Res );
				return FALSE;
			}
		}
		//
		z1 = z2;
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( rcpName , '|' , Retdata , z1 , 511 );
		if ( z1 == z2 ) break;
	}
	while( TRUE );
	//
	return TRUE;
}



BOOL SCHMULTI_RCPLOCKING_DELETE( char *pjname ) { // 2011.09.20
	/*
	// 2012.06.10
	char BufferT[511+1];
	char BufferN[511+1];
	//
	HANDLE hFind;
	WIN32_FIND_DATA	fd;
	//
	BOOL bRet = TRUE;
	//
	_snprintf( BufferN , 511 , "JOB%s" , pjname );
	//
	Change_Dirsep_to_Dollar( BufferN );
	_snprintf( BufferT , 511 , "%s/tmp/%s$*.*" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferN );
	//
	hFind = FindFirstFile( BufferT , &fd );
	while ( ( hFind != INVALID_HANDLE_VALUE) && bRet ) {
		if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) {
			//
			sprintf( BufferN , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , fd.cFileName );
			//
			DeleteFile( BufferN );
		}
		bRet = FindNextFile( hFind , &fd );
	}
	FindClose( hFind );
	//
	*/
	// 2012.06.10
	char BufferT[511+1];
	char BufferN[511+1];
	//
	_snprintf( BufferN , 511 , "JOB%s" , pjname );
	Change_Dirsep_to_Dollar( BufferN );
	//
	if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() >= 3 ) { // 2015.07.20
//		_snprintf( BufferT , 511 , "%s/tmp/%s*" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferN ); // 2017.10.30
		_snprintf( BufferT , 511 , "%s/tmp/%s*" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferN ); // 2017.10.30
		_FILE2SM_DELETE( BufferT , TRUE );
	}
	else {
//		_snprintf( BufferT , 511 , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , BufferN ); // 2017.10.30
		_snprintf( BufferT , 511 , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferN ); // 2017.10.30
		//
		Directory_Delete_at_Sub( BufferT );
	}
	return TRUE;
}



//
CLUSTERStepTemplate2	J_CLUSTER_RECIPE;
//

//int SCHMULTI_RCPLOCKING_PROCESSJOB( int mode , char *JobID , char *rcpName ) { // 2011.09.20 // 2015.10.12
//int SCHMULTI_RCPLOCKING_PROCESSJOB( int mode , char *JobID , char *rcpName , BOOL manualmode ) { // 2011.09.20 // 2015.10.12 // 2016.09.01
int SCHMULTI_RCPLOCKING_PROCESSJOB( int mode , char *JobID , char *rcpName , BOOL manualmode , BOOL NoError ) { // 2011.09.20 // 2015.10.12 // 2016.09.01
	char RunGroup[4096+1];
	char DefGroup[4096+1];
	char RunFile[4096+1];
	//
	char oldclst[255+1];
	//
	LOTStepTemplate LOT_RECIPE;
	LOTStepTemplate2 LOT_RECIPE_DUMMY;
	LOTStepTemplate3 LOT_RECIPE_SPECIAL;
	//
	LOTStepTemplate_Dummy_Ex LOT_RECIPE_DUMMY_EX; // 2016.07.13
	//
	CLUSTERStepTemplate3 CLUSTER_RECIPE_SPECIAL;
	//
	int G_lspdata , selgrp , multigroupfind;
	int i , j , k , readclst;
	//
	int rcpName_SHPT;
	//
	BOOL hasSlotFix; // 2015.10.12
	//
	rcpName_SHPT = 0;
	if ( ( rcpName[0] == '+' ) && ( ( rcpName[1] == '\\' ) || ( rcpName[1] == '/' ) ) )  rcpName_SHPT = 2; // 2015.07.20
	//
//	if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 2 ) return 1; // 2015.07.20
	switch ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() ) { // 2015.07.20
	case 2 :
	case 3 :
		break;
	default :
		return 1;
		break;
	}
	//
	if ( mode == 0 ) { // delete
		SCHMULTI_RCPLOCKING_DELETE( JobID ); // 2011.09.20
	}
	else { // insert
		//
//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Start [%d][%s][%s]\n" , mode , JobID , rcpName );
		//
		strcpy( DefGroup , SCHMULTI_DEFAULT_GROUP_GET() );
		//
//		switch( SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() ) { // 2014.06.23
		switch( _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ) { // 2014.06.23
		case 0 :
			readclst = TRUE;
			break;
		case 1 :
		case 2 :
			readclst = FALSE;
			break;
		case 3 :
		case 4 :
			readclst = manualmode ? FALSE : TRUE;
			break;
		case 5 :
		case 6 :
			readclst = manualmode ? TRUE : FALSE;
			break;
		default :
			readclst = TRUE;
			break;
		}
		//
		if ( readclst ) { // Cluster
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) LOT_RECIPE.CLUSTER_USING[i] = 1;
			//
			LOT_RECIPE.CLUSTER_USING[0] = 0;
			strcpy( LOT_RECIPE.CLUSTER_RECIPE[0] , rcpName + rcpName_SHPT );
			//
		}
		else { // lot
			//
			UTIL_EXTRACT_GROUP_FILE( DefGroup , rcpName + rcpName_SHPT , RunGroup , 255 , RunFile , 255 ); // 2012.05.27
			//
//			if ( !RECIPE_FILE_LOT_DATA_READ( -1 , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , RunGroup , &G_lspdata , rcpName , -1 , NULL ) ) { // 2015.10.12
//			if ( !RECIPE_FILE_LOT_DATA_READ( -1 , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , RunGroup , &G_lspdata , rcpName , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13
//			if ( !RECIPE_FILE_LOT_DATA_READ( -1 , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , RunGroup , &G_lspdata , rcpName , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
			if ( !RECIPE_FILE_LOT_DATA_READ( FALSE , NULL , -1 , 0 , &LOT_RECIPE , &LOT_RECIPE_DUMMY , &LOT_RECIPE_SPECIAL , &LOT_RECIPE_DUMMY_EX , RunGroup , &G_lspdata , rcpName , -1 , NULL , &hasSlotFix ) ) { // 2015.10.12 // 2016.07.13 // 2016.12.10
//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s] -1\n" , mode , JobID , rcpName , RunFile );
				//
				_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => RECIPE_FILE_LOT_DATA_READ [L][mode=%d][Job=%s][rcp=%s][DefGrp=%s][Grp=%s][File=%s]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile ); // 2015.03.27
				//
//				return -1; // 2016.09.01
				if ( !NoError ) return -1; // 2016.09.01
			}
			else { // 2016.09.01
				//
	//			UTIL_EXTRACT_GROUP_FILE( SCHMULTI_DEFAULT_GROUP_GET() , rcpName , RunGroup , 255 , RunFile , 255 ); // 2012.05.27
				//
				if ( !SCHMULTI_RCPLOCKING_FILECOPY( JobID , 0 , 0 , RunGroup , RunFile ) ) {
	//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s][%s] -2\n" , mode , JobID , rcpName , RunGroup , RunFile );
					//
					_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => SCHMULTI_RCPLOCKING_FILECOPY [L][mode=%d][Job=%s][rcp=%s][DefGrp=%s][Grp=%s][File=%s]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile ); // 2015.03.27
					//
//					return -2; // 2016.09.01
					if ( !NoError ) return -2; // 2016.09.01
				}
				//
			}
			//
			strcpy( DefGroup , RunGroup );
			//
		}
		//
		strcpy( oldclst , "" );
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( LOT_RECIPE.CLUSTER_USING[i] != 0 ) continue;
			//
			if ( STRCMP_L( oldclst , LOT_RECIPE.CLUSTER_RECIPE[i] ) ) continue;
			//
			strcpy( oldclst , LOT_RECIPE.CLUSTER_RECIPE[i] );
			//
			UTIL_EXTRACT_GROUP_FILE( DefGroup , LOT_RECIPE.CLUSTER_RECIPE[i] , RunGroup , 4096 , RunFile , 4096 ); // 2012.05.26
			//
//			if ( !RECIPE_FILE_CLUSTER_DATA_READ( -1 , &J_CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , NULL , i + 1 , LOT_RECIPE.CLUSTER_RECIPE[i] , -1 , &selgrp , &multigroupfind , NULL ) ) { // 2012.05.26
			if ( !RECIPE_FILE_CLUSTER_DATA_READ( -1 , &J_CLUSTER_RECIPE , &CLUSTER_RECIPE_SPECIAL , RunGroup , i + 1 , RunFile , -1 , &selgrp , &multigroupfind , NULL ) ) { // 2012.05.26
//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s] [%s] -3\n" , mode , JobID , rcpName , RunFile , LOT_RECIPE.CLUSTER_RECIPE[i] );
				//
				_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => RECIPE_FILE_CLUSTER_DATA_READ [C][mode=%d][Job=%s][rcp=%s][DefGrp=%s][Grp=%s][File=%s][i=%d]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile , i ); // 2015.03.27
				//
//				return -3; // 2016.09.01
				if ( !NoError ) return -3; // 2016.09.01
			}
			else { // 2016.09.01
				//
	//			UTIL_EXTRACT_GROUP_FILE( SCHMULTI_DEFAULT_GROUP_GET() , LOT_RECIPE.CLUSTER_RECIPE[i] , RunGroup , 255 , RunFile , 255 ); // 2012.05.26
				//
				if ( !SCHMULTI_RCPLOCKING_FILECOPY( JobID , 1 , 0 , RunGroup , RunFile ) ) {
	//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s][%s] [%d][%s] -4\n" , mode , JobID , rcpName , RunGroup , RunFile , DefGroup , LOT_RECIPE.CLUSTER_RECIPE[i] );
					//
					_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => SCHMULTI_RCPLOCKING_FILECOPY [C][mode=%d][Job=%s][rcp=%s][DefGrp=%s][Grp=%s][File=%s][i=%d]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile , i ); // 2015.03.27
					//
//					return -4; // 2016.09.01
					if ( !NoError ) return -4; // 2016.09.01
				}
				//
				for ( j = 0 ; j < MAX_CLUSTER_DEPTH ; j++ ) {
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						if ( J_CLUSTER_RECIPE.MODULE[j][k] > 0 ) {
							//===================================================================================================================
							if ( J_CLUSTER_RECIPE.MODULE_IN_RECIPE2[j][k] != NULL ) {
								//
								UTIL_EXTRACT_GROUP_FILE( DefGroup , J_CLUSTER_RECIPE.MODULE_IN_RECIPE2[j][k] , RunGroup , 4096 , RunFile , 4096 );
								//
								if ( !SCHMULTI_RCPLOCKING_FILECOPY( JobID , k + PM1 , 0 , RunGroup , RunFile ) ) {
	//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s][%s] [%s][%s] -5\n" , mode , JobID , rcpName , RunGroup , RunFile , DefGroup , J_CLUSTER_RECIPE.MODULE_IN_RECIPE2[j][k] );
									//
									_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => SCHMULTI_RCPLOCKING_FILECOPY [0][mode=%d][Job=%s][rcp=%s][DefGrp=%s][Grp=%s][File=%s][i=%d][k=%d]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile , i , k ); // 2015.03.27
									//
//									return -5; // 2016.09.01
									if ( !NoError ) return -5; // 2016.09.01
								}
								//
							}
							if ( J_CLUSTER_RECIPE.MODULE_OUT_RECIPE2[j][k] != NULL ) {
								//
								UTIL_EXTRACT_GROUP_FILE( DefGroup , J_CLUSTER_RECIPE.MODULE_OUT_RECIPE2[j][k] , RunGroup , 4096 , RunFile , 4096 );
								//
								if ( !SCHMULTI_RCPLOCKING_FILECOPY( JobID , k + PM1 , 1 , RunGroup , RunFile ) ) {
	//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s][%s] [%s][%s] -6\n" , mode , JobID , rcpName , RunGroup , RunFile , DefGroup , J_CLUSTER_RECIPE.MODULE_OUT_RECIPE2[j][k] );
									//
									_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => SCHMULTI_RCPLOCKING_FILECOPY [1][mode=%d][Job=%s][rcpName=%s][DefGrp=%s][Grp=%s][File=%s][i=%d][k=%d]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile , i , k ); // 2015.03.27
									//
//									return -6; // 2016.09.01
									if ( !NoError ) return -6; // 2016.09.01
								}
								//
							}
							if ( J_CLUSTER_RECIPE.MODULE_PR_RECIPE2[j][k] != NULL ) {
								//
								UTIL_EXTRACT_GROUP_FILE( DefGroup , J_CLUSTER_RECIPE.MODULE_PR_RECIPE2[j][k] , RunGroup , 4096 , RunFile , 4096 );
								//
								if ( !SCHMULTI_RCPLOCKING_FILECOPY( JobID , k + PM1 , 2 , RunGroup , RunFile ) ) {
	//printf( "SCHMULTI_RCPLOCKING_PROCESSJOB Fail [%d][%s][%s][%s][%s] [%s][%s] -7\n" , mode , JobID , rcpName , RunGroup , RunFile , DefGroup , J_CLUSTER_RECIPE.MODULE_PR_RECIPE2[j][k] );
									//
									_IO_CIM_PRINTF( "SCHMULTI_RCPLOCKING_PROCESSJOB ERROR => SCHMULTI_RCPLOCKING_FILECOPY [2][mode=%d][Job=%s][rcp=%s][DefGrp=%s][Grp=%s][File=%s][i=%d][k=%d]\n" , mode , JobID , rcpName , DefGroup , RunGroup , RunFile , i , k ); // 2015.03.27
									//
//									return -7; // 2016.09.01
									if ( !NoError ) return -7; // 2016.09.01
								}
								//
							}
							//===================================================================================================================
						}
					}
				}
			}
		}
		//
		SCHMULTI_RCPLOCKING_GET_FILENAME( -2 , 0 , NULL , NULL , NULL , NULL , 0 ); // 2013.09.13
		//
	}
	//
	return 2;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_SET_LOG_MESSAGE( char *data ) {
	SYSTEMTIME		SysTime;
	char timestr[32];
	//
	EnterCriticalSection( &CR_MJ_LOG_MSG_QUEUE );
	if ( MJ_LOG_MSG_QUEUE_Index <                      0 ) MJ_LOG_MSG_QUEUE_Index = 0;
	if ( MJ_LOG_MSG_QUEUE_Index >= MJ_LOG_MSG_QUEUE_SIZE ) MJ_LOG_MSG_QUEUE_Index = 0;
	//
	GetLocalTime( &SysTime );
	//
//	sprintf(timestr, "%04d%/%02d/%02d %02d:%02d:%02d.%03d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds); // 2019.03.07
	sprintf( timestr , "%04d/%02d/%02d %02d:%02d:%02d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ); // 2019.03.07
	//
	STR_MEM_MAKE_COPY2( &(MJ_LOG_MSG_QUEUE_Index_Time[MJ_LOG_MSG_QUEUE_Index]) , timestr );
	STR_MEM_MAKE_COPY2( &(MJ_LOG_MSG_QUEUE_Index_Data[MJ_LOG_MSG_QUEUE_Index]) , data );
	//
	MJ_LOG_MSG_QUEUE_Index++;
	LeaveCriticalSection( &CR_MJ_LOG_MSG_QUEUE );
}




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_RCPTUNE_EXTEND_USE_SET( int use ) { // 2007.11.28
	SCHMULTI_RCPTUNE_EXTEND_USE = use;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char *SCHMULTI_TUNE_INFO_DATA_GET( int ch ) { // 2007.06.08
	return SCHMULTI_TUNE_INFO_DATA[ch];
}
void SCHMULTI_TUNE_INFO_DATA_RESET( int ch ) { // 2007.06.08
	if ( SCHMULTI_TUNE_INFO_DATA[ch] != NULL ) {
		free( SCHMULTI_TUNE_INFO_DATA[ch] );
		SCHMULTI_TUNE_INFO_DATA[ch] = NULL;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_CHECK_OVERTIME_HOUR_SET( int data ) { // 2012.04.20
	SCHMULTI_CHECK_OVERTIME_HOUR = data;
}
int  SCHMULTI_CHECK_OVERTIME_HOUR_GET() { // 2012.04.20
	return SCHMULTI_CHECK_OVERTIME_HOUR;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_DEFAULT_GROUP_SET( char *data ) {
	strncpy( SCHMULTI_DEFAULT_GROUP , data , 64 );
}
char *SCHMULTI_DEFAULT_GROUP_GET() {
	return SCHMULTI_DEFAULT_GROUP;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//void SCHMULTI_DEFAULT_LOTRECIPE_READ_SET( int data ) { // 2003.09.24 // 2014.06.23
void _i_SCH_MULTIJOB_SET_LOTRECIPE_READ( int data ) { // 2003.09.24 // 2014.06.23
	SCHMULTI_DEFAULT_LOTRECIPE_READ = data;
}
//int  SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() { // 2003.09.24 // 2014.06.23
int  _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() { // 2003.09.24 // 2014.06.23
	return SCHMULTI_DEFAULT_LOTRECIPE_READ;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_SET( int data ) { // 2004.05.11
	SCHMULTI_MAKE_MANUAL_CJPJJOBMODE = data;
}
int  SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() { // 2004.05.11
	return SCHMULTI_MAKE_MANUAL_CJPJJOBMODE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_AFTER_SELECT_CHECK_SET( int data ) { // 2004.06.26
	SCHMULTI_AFTER_SELECT_CHECK = data;
}
int  SCHMULTI_AFTER_SELECT_CHECK_GET() { // 2004.06.26
	return SCHMULTI_AFTER_SELECT_CHECK;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_JOBNAME_DISPLAYMODE_SET( int data ) { // 2005.05.11
	SCHMULTI_JOBNAME_DISPLAYMODE = data;
}
int  SCHMULTI_JOBNAME_DISPLAYMODE_GET() { // 2005.05.11
	return SCHMULTI_JOBNAME_DISPLAYMODE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_SCENARIO_SET( int data ) { // 2006.04.13
	SCHMULTI_MESSAGE_SCENARIO = data;
}
int  SCHMULTI_MESSAGE_SCENARIO_GET() { // 2006.04.13
	return SCHMULTI_MESSAGE_SCENARIO;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHMULTI_GET_AFTERSEL_WAITMODE( int side ) { // 2007.10.02
	if ( !SCHMULTI_CTJOB_USE_GET() ) return FALSE; // 2007.10.31
	if (
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 4 ) ||
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 5 ) ||
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 6 ) ||
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 7 )
		) {

//		if ( SCHMULTI_AFTER_SELECT_CHECK_GET() > 1 ) return TRUE; // 2007.11.30

		if ( PRE_RUN_POSSIBLE_CHECK[ side ] != 0 ) return TRUE;

	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_DATA( int index , char *cjname , char *pjname , BOOL *MultiCJ ) { // 2004.05.11
//	struct tm		Pres_Time; // 2006.10.02
	SYSTEMTIME		SysTime; // 2006.10.02
	if (
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 1 ) || // Name=Time    , SingleCJ
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 3 ) || // Name=Time    , MultiCJ
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 5 ) || // Name=Time    , SingleCJ  , ASel
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 7 )    // Name=Time    , MultiCJ   , ASel
		) {
		//======================================================================
		// 2006.10.02
		//======================================================================
//		_get-systime( &Pres_Time );
//		if (
//			( Pres_Time.tm_hour == SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_HH ) && 
//			( Pres_Time.tm_min == SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_MM ) && 
//			( Pres_Time.tm_sec == SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_SS ) ) {
//			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC++;
//		}
//		else {
//			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_HH = Pres_Time.tm_hour;
//			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_MM = Pres_Time.tm_min;
//			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_SS = Pres_Time.tm_sec;
//			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC = 0;
//		}
//		sprintf( cjname , "CJ%04d%02d%02d%02d%02d%02d%02d" , Pres_Time.tm_year+1900 , Pres_Time.tm_mon + 1 , Pres_Time.tm_mday , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec , SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC + 1 );
//		sprintf( pjname , "PJ%04d%02d%02d%02d%02d%02d%02d" , Pres_Time.tm_year+1900 , Pres_Time.tm_mon + 1 , Pres_Time.tm_mday , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec , SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC + 1 );
		//======================================================================
		GetLocalTime( &SysTime );
		if (
			( SysTime.wHour == SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_HH ) && 
			( SysTime.wMinute == SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_MM ) && 
			( SysTime.wSecond == SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_SS ) ) {
			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC++;
		}
		else {
			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_HH = SysTime.wHour;
			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_MM = SysTime.wMinute;
			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_SS = SysTime.wSecond;
			SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC = 0;
		}
		sprintf( cjname , "CJ%04d%02d%02d%02d%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC + 1 );
		sprintf( pjname , "PJ%04d%02d%02d%02d%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SCHMULTI_MAKE_MANUAL_CJPJJOBNAME_OLD_CC + 1 );
		//======================================================================
	}
	else {
//		strcpy( cjname , "$MANUAL$" ); // 2012.04.12
		sprintf( cjname , "$MANUAL%d$" , index + 1 ); // 2012.04.12
		sprintf( pjname , "$MANUAL%d$" , index + 1 );
	}
	if (
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 2 ) || // Name=Fixed   , MultiCJ
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 3 ) || // Name=Time    , MultiCJ
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 6 ) || // Name=Fixed   , MultiCJ , ASel
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 7 )    // Name=Time    , MultiCJ , ASel
		) {
		*MultiCJ = TRUE;
	}
	else {
		*MultiCJ = FALSE;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHMULTI_INIT() {
	int i;

	//---------------------------------------------------------------------------------------------------
	GUI_INSERT_CONTROLJOB_NAME = NULL;
	GUI_INSERT_CONTROLJOB_STARTMODE = 0;
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		GUI_INSERT_CONREOLJOB_PRJOB_NAME[ i ] = NULL;
		GUI_INSERT_CONREOLJOB_PRJOB_NAME2[ i ] = NULL;
	}
	//
	GUI_INSERT_PROCESSJOB_NAME = NULL;
	GUI_INSERT_PROCESSJOB_STARTMODE = 0;
	GUI_INSERT_PROCESSJOB_STARTSLOT = 1;
//	GUI_INSERT_PROCESSJOB_ENDSLOT = MAX_CASSETTE_SLOT_SIZE; // 2014.02.03
	GUI_INSERT_PROCESSJOB_ENDSLOT = -1; // 2014.02.03
	GUI_INSERT_PROCESSJOB_RECIPE = NULL;
	GUI_INSERT_PROCESSJOB_CARRIERID = NULL;
	//---------------------------------------------------------------------------------------------------
//	_SCH_MULTIJOB_INTERFACE_INITIALIZING(); // 2007.11.28
//	if ( !_SCH_MULTIJOB_INTERFACE_INITIALIZING( "" , TRUE , SCHMULTI_RCPTUNE_EXTEND_USE ) ) return FALSE; // 2007.11.28 // 2011.09.07

//	if ( !_SCH_MULTIJOB_INTERFACE_INITIALIZING( "" , 2 , SCHMULTI_RCPTUNE_EXTEND_USE ) ) return FALSE; // 2007.11.28 // 2011.09.07 // 2016.10.21
	//
	//
	// 2016.10.21
	//
	switch( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() ) {
	case 1 :
	case 3 :
	case 5 :
	case 7 :
		//
		if ( !_SCH_MULTIJOB_INTERFACE_INITIALIZING( GET_SYSTEM_NAME() , 2 , SCHMULTI_RCPTUNE_EXTEND_USE ) ) return FALSE;
		//
		break;
	default :
		//
		if ( !_SCH_MULTIJOB_INTERFACE_INITIALIZING( "" , 2 , SCHMULTI_RCPTUNE_EXTEND_USE ) ) return FALSE;
		//
		break;
	}
	//
	//---------------------------------------------------------------------------------------------------
	// 2004.10.06
	//---------------------------------------------------------------------------------------------------
	memset( Sch_Multi_ProcessJob_Buffer , 0 , sizeof(Scheduling_Multi_ProcessJob) );
	memset( Sch_Multi_ControlJob_Buffer , 0 , sizeof(Scheduling_Multi_ControlJob) );
	//
	for ( i = 0 ; i < MAX_MULTI_QUEUE_SIZE ; i++ ) {
		memset( Sch_Multi_ProcessJob[i] , 0 , sizeof(Scheduling_Multi_ProcessJob) );
		memset( Sch_Multi_ControlJob[i] , 0 , sizeof(Scheduling_Multi_ControlJob) );
		//
		memset( &(Sch_Multi_Ins_ProcessJob[i]) , 0 , sizeof(Scheduling_Multi_Ins_ProcessJob) ); // 2012.04.19
		memset( &(Sch_Multi_Ins_ControlJob[i]) , 0 , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		for ( i = 0 ; i < MAX_MULTI_PRJOB_EXT_SIZE ; i++ ) {
			//
			memset( Sch_Multi_ProcessJob[i+MAX_MULTI_QUEUE_SIZE] , 0 , sizeof(Scheduling_Multi_ProcessJob) );
			//
			memset( &(Sch_Multi_Ins_ProcessJob[i+MAX_MULTI_QUEUE_SIZE]) , 0 , sizeof(Scheduling_Multi_Ins_ProcessJob) ); // 2012.04.19
			//
		}
	}
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		//
		memset( Sch_Multi_ControlJob_Select_Data[i] , 0 , sizeof(Scheduling_Multi_ControlJob) );
		//
		memset( &(Sch_Multi_Ins_ControlJob_Select_Data[i]) , 0 , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
		//
	}
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		memset( Sch_Multi_CassRunJob_Data[i] , 0 , sizeof(Scheduling_Multi_CassRunJob) );
	}
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		MULTI_RUNJOB_INIT( i );
	}
	//---------------------------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2007.06.08
		SCHMULTI_TUNE_INFO_DATA[ i ] = NULL; // 2007.06.08
	} // 2007.06.08
	//---------------------------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		Sch_Multi_Cassette_Verify[i] = CASSETTE_VERIFY_READY;
//		strcpy( Sch_Multi_Cassette_MID[i] , "" ); // 2008.04.02
		Sch_Multi_Cassette_MID[i] = NULL; // 2008.04.02
		//
		Sch_Multi_Cassette_MID_for_Start[i] = NULL; // 2011.03.10
	}
	*Sch_Multi_ControlJob_Size = 0;
	*Sch_Multi_ProcessJob_Size = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_QUEUE_SIZE ; i++ ) {
		Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_NOTSTATE;
		Sch_Multi_ControlJob[i]->ControlStatus = CTLJOB_STS_NOTSTATE;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		for ( i = 0 ; i < MAX_MULTI_PRJOB_EXT_SIZE ; i++ ) {
			Sch_Multi_ProcessJob[i+MAX_MULTI_QUEUE_SIZE]->ControlStatus = PRJOB_STS_NOTSTATE;
		}
	}
	//
	*Sch_Multi_ProcessJob_OrderMode = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		Sch_Multi_ControlJob_Select_Data[i]->ControlStatus = CTLJOB_STS_NOTSTATE;
		Sch_Multi_ControlJob_Selected_Stop_Signal[i] = FALSE;
	}
	//
	Sch_Multi_ProcessJob_Buffer->ControlStatus = 0;
	Sch_Multi_ControlJob_Buffer->ControlStatus = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
	}
	//----------------------------------------------------------------------
	*Sch_Multi_ProcessJob_Result_Status = 0; // 2005.04.09
	*Sch_Multi_ControlJob_Result_Status = 0; // 2005.04.09
	//----------------------------------------------------------------------
	// 2007.11.28
	//----------------------------------------------------------------------
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) {
		memset( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer , 0 , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
		for ( i = 0 ; i < MAX_MULTI_QUEUE_SIZE ; i++ ) {
			memset( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i] , 0 , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
		}
		if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
			for ( i = 0 ; i < MAX_MULTI_PRJOB_EXT_SIZE ; i++ ) {
				memset( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i+MAX_MULTI_QUEUE_SIZE] , 0 , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
			}
		}
	}
	//----------------------------------------------------------------------
	// 2015.12.01
	//----------------------------------------------------------------------
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) {
		//
		memset( Sch_Multi_ProcessJob_Ext_Buffer , 0 , sizeof(Scheduling_Multi_PrJob_Ext) );
		memset( Sch_Multi_ControlJob_Ext_Buffer , 0 , sizeof(Scheduling_Multi_CrJob_Ext) );
		//
		for ( i = 0 ; i < MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ; i++ ) {
			memset( Sch_Multi_ProcessJob_Ext[i] , 0 , sizeof(Scheduling_Multi_PrJob_Ext) );
		}
		//
		for ( i = 0 ; i < MAX_MULTI_QUEUE_SIZE ; i++ ) {
			memset( Sch_Multi_ControlJob_Ext[i] , 0 , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
		for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
			memset( Sch_Multi_ControlJob_Ext_Select_Data[i] , 0 , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
	}
	//----------------------------------------------------------------------
	Address_IO_CTJob_Remain = _FIND_FROM_STRING( _K_D_IO , "CTC.JOB_CT_REMAIN_COUNT" );
	Address_IO_PRJob_Remain = _FIND_FROM_STRING( _K_D_IO , "CTC.JOB_PR_REMAIN_COUNT" );
	//----------------------------------------------------------------------
	SCHMULTI_CTJOB_IO_SET();
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	SCHMULTI_PRJOB_IO_SET();
	//----------------------------------------------------------------------
	SCHMRDATA_INIT(); // 2011.09.04
	//----------------------------------------------------------------------
	InitializeCriticalSection( &CR_MULTIJOB );
	//----------------------------------------------------------------------
	InitializeCriticalSection( &CR_MJ_LOG_MSG_QUEUE ); // 2011.04.25
	//
	for ( i = 0 ; i < MJ_LOG_MSG_QUEUE_SIZE ; i++ ) {
		MJ_LOG_MSG_QUEUE_Index_Time[ i ] = NULL;
		MJ_LOG_MSG_QUEUE_Index_Data[ i ] = NULL;
	}
	//----------------------------------------------------------------------
	return TRUE;
}
//=============================================================================
//=============================================================================
BOOL SCHMULTI_MESSAGE_GET_ALL_SUB( int side , int pointer , char *data , int size , int ppid ) { // 2011.07.18
	int c , p;
//	char ed_cj[65] , ed_pj[65] , ed_ppid[65] , ed_wid[65]; // 2014.01.30
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256]; // 2014.01.30
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2002.05.08
		strcpy( data , "" );
		return FALSE;
	}
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );

	//
	if ( c == -99 ) { // 2010.10.04 EIL
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) {
			strcpy( data , "" );
			return FALSE;
		}
		else {
//			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 64 ); // 2014.01.30
			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 255 ); // 2014.01.30
			//
			if ( ppid >= 1 ) {
				if ( ed_ppid[0] == 0 ) { // 2010.11.30
					strcpy( data , "" );
					return FALSE;
				}
				//
				_snprintf( data , size , "%s" , ed_ppid );
			}
			else {
				if ( ( ed_cj[0] == 0 ) && ( ed_pj[0] == 0 ) ) { // 2010.11.30
					strcpy( data , "" );
					return FALSE;
				}
				//
				_snprintf( data , size , "|%s|%s" , ed_cj , ed_pj );
			}
		}
	}
	else {
		//
		if ( ppid == 2 ) { // 2011.09.31
			if ( _i_SCH_CLUSTER_Get_Ex_PPID( side , pointer ) == NULL ) {
				strcpy( data , "" );
				return FALSE;
			}
			else if ( _i_SCH_CLUSTER_Get_Ex_PPID( side , pointer )[0] == 0 ) {
				strcpy( data , "" );
				return FALSE;
			}
			//
			_snprintf( data , size , "%s" , _i_SCH_CLUSTER_Get_Ex_PPID( side , pointer ) );
		}
		else {
			if ( ( c < 0 ) || ( p < 0 ) ) {
				strcpy( data , "" );
				return FALSE;
			}
			//
			if ( ( MULTI_RUNJOB_MtlCJName( c )[0] == 0 ) && ( MULTI_RUNJOB_MtlPJName( c )[p][0] == 0 ) ) { // 2010.11.30
				strcpy( data , "" );
				return FALSE;
			}
			//
			_snprintf( data , size , "|%s|%s" , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] );
		}
		//
	}
	return TRUE;
}
//=============================================================================
BOOL SCHMULTI_MESSAGE_GET_ALL( int side , int pointer , char *data , int size ) {
	return SCHMULTI_MESSAGE_GET_ALL_SUB( side , pointer , data , size , 0 ); // 2011.07.18
}
//---------------------------------------------------------------------------------------
//=============================================================================
//
// 2014.09.03
//
BOOL SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( int side , int pointer , int ch , int sx , int *mwafer , int *mside , int *mpointer , char *data , int size , BOOL incseps ) {
	int i;
	int c , p;
	int rc , rp;
//	int j , rc2 , rp2; // 2014.09.03
	int nt;
	char cxb29[8];
	char cxb[256];
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256];
	//
	
//	_IO_CIM_PRINTF( "SCHMULTI_MESSAGE_PROCESS_LOG\tCH=%d\t[side=%d][pointer=%d][sx=%d][size=%d][incseps=%d][%s]\n" , ch , side , pointer , sx , size , incseps , PROGRAM_FUNCTION_READ() );


//	for ( i = 0 ; i < 6 ; i++ ) {
//		if ( mwafer[i] <= 0 ) continue;
//		_IO_CIM_PRINTF( "\t\tSLOT=%d\t[side=%d][pointer=%d][wafer=%d][%s]\n" , i , mside[i] , mpointer[i] , mwafer[i] , PROGRAM_FUNCTION_READ() );
//		//
//	}

	//
	strcpy( data , "" );
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) {
		//
		if ( incseps ) strcat( data , "||" );
//
//_IO_CIM_PRINTF( "\t\tRETURN=1\t[data=%s][%s]\n" , data , PROGRAM_FUNCTION_READ() );
//
		return FALSE;
	}
	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
//
//_IO_CIM_PRINTF( "\t\tDATA=1\t[c=%d][p=%d][%s]\n" , c , p , PROGRAM_FUNCTION_READ() );
//
	if ( c == -99 ) {
		//========================================================================
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) {
			//
			if ( incseps ) strcat( data , "||" );
//
//_IO_CIM_PRINTF( "\t\tRETURN=12\t[data=%s][%s]\n" , data , PROGRAM_FUNCTION_READ() );
//
			return FALSE;
		}
		//========================================================================
		//
		STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 255 );
		//
		strncat( data , ed_cj , 255 );
		strncat( data , "_" , 255 );
		strncat( data , ed_pj , 255 );
		//
		if ( incseps ) {
			strncat( data , "|" , 255 );
			strncat( data , ed_cj , 255 );
			strncat( data , "|" , 255 );
			strncat( data , ed_pj , 255 );
		}
		//
//
//_IO_CIM_PRINTF( "\t\tRETURN=13\t[data=%s][%s]\n" , data , PROGRAM_FUNCTION_READ() );
//
	}
	else {
		//
		if ( ( c >= 0 ) && ( p >= 0 ) ) {
			//
			_snprintf( cxb , 255 , "%s_%s" , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] );
			strncat( data , cxb , 255 );
			//
			nt = 1;
			//
			strcpy( ed_cj , cxb ); // 2014.09.03
			//
		}
		else {
			//
			nt = 0;
			//
		}
		//
//
//_IO_CIM_PRINTF( "\t\tDATA=2\t[c=%d][p=%d][nt=%d][%s]\n" , c , p , nt , PROGRAM_FUNCTION_READ() );
//
		for ( i = sx ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
			//
			if ( mwafer[i] <= 0 ) continue;
			//
			rc = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( mside[i] , mpointer[i] );
			rp = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( mside[i] , mpointer[i] );
			//	
//
//_IO_CIM_PRINTF( "\t\tDATA=3\t[c=%d][p=%d][nt=%d][rc=%d][rp=%d][i=%d][%s]\n" , c , p , nt , rc , rp , i , PROGRAM_FUNCTION_READ() );
//
			if ( rc < 0 ) continue;
			if ( rp < 0 ) continue;
			//
			_snprintf( cxb , 255 , "%s_%s" , MULTI_RUNJOB_MtlCJName( rc ) , MULTI_RUNJOB_MtlPJName( rc )[rp] );
			//
			if ( nt >= 1 ) {
//
//_IO_CIM_PRINTF( "\t\tDATA=41\t[c=%d][p=%d][nt=%d][rc=%d][rp=%d][i=%d][cxb=%s][%s][%s]\n" , c , p , nt , rc , rp , i , cxb , ed_cj , PROGRAM_FUNCTION_READ() );
//
				if ( STRCMP_L( ed_cj , cxb ) ) continue;
			}
			if ( nt >= 2 ) {
//
//_IO_CIM_PRINTF( "\t\tDATA=42\t[c=%d][p=%d][nt=%d][rc=%d][rp=%d][i=%d][cxb=%s][%s][%s]\n" , c , p , nt , rc , rp , i , cxb , ed_pj , PROGRAM_FUNCTION_READ() );
//
				if ( STRCMP_L( ed_pj , cxb ) ) continue;
			}
			if ( nt >= 3 ) {
//
//_IO_CIM_PRINTF( "\t\tDATA=43\t[c=%d][p=%d][nt=%d][rc=%d][rp=%d][i=%d][cxb=%s][%s][%s]\n" , c , p , nt , rc , rp , i , cxb , ed_ppid , PROGRAM_FUNCTION_READ() );
//
				if ( STRCMP_L( ed_ppid , cxb ) ) continue;
			}
			if ( nt >= 4 ) {
//
//_IO_CIM_PRINTF( "\t\tDATA=44\t[c=%d][p=%d][nt=%d][rc=%d][rp=%d][i=%d][cxb=%s][%s][%s]\n" , c , p , nt , rc , rp , i , cxb , ed_wid , PROGRAM_FUNCTION_READ() );
//
				if ( STRCMP_L( ed_wid , cxb ) ) continue;
			}
			//
			if      ( nt == 0 ) {
				strcpy( ed_cj , cxb );
			}
			else if ( nt == 1 ) {
				strcpy( ed_pj , cxb );
			}
			else if ( nt == 2 ) {
				strcpy( ed_ppid , cxb );
			}
			else if ( nt == 3 ) {
				strcpy( ed_wid , cxb );
			}
			//
			if ( nt != 0 ) {
				_snprintf( cxb29 , 7 , "%c" , 29 );
				strncat( data , cxb29 , 255 );
			}
			else {
				c = rc;
				p = rp;
			}
			//
			nt++;
			//
			strncat( data , cxb , 255 );
			//
//
//_IO_CIM_PRINTF( "\t\tDATA=5\t[c=%d][p=%d][nt=%d][rc=%d][rp=%d][i=%d][data=%s][%s]\n" , c , p , nt , rc , rp , i , data , PROGRAM_FUNCTION_READ() );
//
		}
		//
		if ( ( c < 0 ) || ( p < 0 ) ) {
			//
			if ( incseps ) strncat( data , "||" , 255 );
			//
//
//_IO_CIM_PRINTF( "\t\tRETURN=8\t[data=%s][%s]\n" , data , PROGRAM_FUNCTION_READ() );
//
			return FALSE;
			//
		}
		else {
			if ( incseps ) {
				strncat( data , "|" , 255 );
				strncat( data , MULTI_RUNJOB_MtlCJName( c ) , 255 );
				strncat( data , "|" , 255 );
				strncat( data , MULTI_RUNJOB_MtlPJName( c )[p] , 255 );
			}
		}
//
//_IO_CIM_PRINTF( "\t\tRETURN=9\t[data=%s][%s]\n" , data , PROGRAM_FUNCTION_READ() );
//
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------

char *SCHMULTI_RUNJOB_GET_DATABASE_LINK_PJ( int mode , int side , int pt );

//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( int side , int pointer , char *data , int size ) {
	int c , p;
//	char ed_cj[65] , ed_pj[65] , ed_ppid[65] , ed_wid[65]; // 2014.01.30
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256]; // 2014.01.30

	if ( ( side / 10000 ) > 0 ) { // 2014.08.06
		//
		_snprintf( data , size , "%s" , SCHMULTI_RUNJOB_GET_DATABASE_LINK_PJ( 1 , side % 10000 , pointer ) );
		//
		return TRUE;
	}
	//
//	if ( ( side < 0 ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2014.08.06
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2014.08.06
		return FALSE;
	}
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
	if ( c == -99 ) { // 2010.10.04 EIL
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) {
			return FALSE;
		}
		else {
//			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 64 ); // 2014.01.30
			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 255 ); // 2014.01.30
			//
			_snprintf( data , size , "%s" , ed_pj );
		}
	}
	else {
		if ( ( c < 0 ) || ( p < 0 ) ) {
			return FALSE;
		}
		_snprintf( data , size , "%s" , MULTI_RUNJOB_MtlPJName( c )[p] );
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_GET_CONTROLJOB_NAME( int side , int pointer , char *data , int size ) { // 2012.09.13
	int c , p;
//	char ed_cj[65] , ed_pj[65] , ed_ppid[65] , ed_wid[65]; // 2014.01.30
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256]; // 2014.01.30

	if ( ( side < 0 ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) {
		return FALSE;
	}
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
	if ( c == -99 ) {
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) {
			return FALSE;
		}
		else {
//			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 64 ); // 2014.01.30
			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 255 ); // 2014.01.30
			//
			_snprintf( data , size , "%s" , ed_cj );
		}
	}
	else {
		if ( ( c < 0 ) || ( p < 0 ) ) {
			return FALSE;
		}
		_snprintf( data , size , "%s" , MULTI_RUNJOB_MtlCJName( c ) );
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_MESSAGE_GET_PPID( int side , int pointer , int mode , int wfrpos , int option , BOOL dual , int ch , char *data , int size ) { // 2007.08.14
	int c , p;
	//
//	char ed_cj[65] , ed_pj[65] , ed_ppid[65] , ed_wid[65]; // 2014.01.30
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256]; // 2014.01.30

	if ( ( side < 0 ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) {
		strcpy( data , "" );
		return FALSE;
	}
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
	if ( c == -99 ) { // 2010.10.04 EIL
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) {
			strcpy( data , "" );
			return FALSE;
		}
		else {
//			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 64 ); // 2014.01.30
			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 255 ); // 2014.01.30
			//
			if ( mode == 0 ) {
				_snprintf( data , size , "%s|%s|%s" , ed_cj , ed_pj , ed_ppid );
			}
			else { // 2010.08.30
				//
				if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , 1 ) ) {
//					_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_D%02d_PM%d" , wfrpos + 1 , ed_cj , ed_pj , ed_ppid , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch - PM1 + 1 ); // 2012.02.18
					_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_D%02d_PM%d" , wfrpos + 1 , ed_cj , ed_pj , ed_ppid , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option % 100 , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch - PM1 + 1 ); // 2012.02.18
				}
				else {
					//
//					_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , wfrpos + 1 , ed_cj , ed_pj , ed_ppid , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch - PM1 + 1 ); // 2012.02.18
					//
					if ( dual ) {
						_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , wfrpos + 1 , ed_cj , ed_pj , ed_ppid , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , ( option % 100 ) , ( option / 100 ) , ch - PM1 + 1 );
					}
					else {
						if ( ( option / 100 ) > 0 ) {
							_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , wfrpos + 1 , ed_cj , ed_pj , ed_ppid , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , ( option % 100 ) , ( option / 100 ) , ch - PM1 + 1 );
						}
						else {
							_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_PM%d" , wfrpos + 1 , ed_cj , ed_pj , ed_ppid , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option , ch - PM1 + 1 );
						}
					}
					//
				}
				//
			}
			//
		}
	}
	else {
		if ( ( c < 0 ) || ( p < 0 ) ) {
			strcpy( data , "" );
			return FALSE;
		}
		//
		if ( mode == 0 ) {
			_snprintf( data , size , "%s|%s|%s" , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] , MULTI_RUNJOB_MtlRecipeName( c )[p] );
		}
		else { // 2010.08.30
			//
			if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , 1 ) ) {
				_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_D%02d_PM%d" , wfrpos + 1 , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] , MULTI_RUNJOB_MtlRecipeName( c )[p] , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option % 100 , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch - PM1 + 1 ); // 2012.02.18
			}
			else {
				//
				if ( dual ) {
					_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , wfrpos + 1 , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] , MULTI_RUNJOB_MtlRecipeName( c )[p] , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option % 100 , option / 100 , ch - PM1 + 1 ); // 2012.02.18
				}
				else {
					if ( ( option / 100 ) > 0 ) {
						_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , wfrpos + 1 , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] , MULTI_RUNJOB_MtlRecipeName( c )[p] , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option % 100 , option / 100 , ch - PM1 + 1 ); // 2012.02.18
					}
					else {
						_snprintf( data , size , "[%d]%s|%s|%s|%s/LOTLOG/%s/PROCESS_%02d_PM%d" , wfrpos + 1 , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] , MULTI_RUNJOB_MtlRecipeName( c )[p] , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , option , ch - PM1 + 1 ); // 2012.02.18
					}
				}
				//
			}
			//
		}
	}
	//
	return TRUE;
}
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_PROCESSJOB_REJECT( int action , int code , int side , int pt ) {
	char Buffer[256];
	//===================================================================================
	Sch_Multi_ProcessJob_Buffer->ResultStatus = code;
	*Sch_Multi_ProcessJob_Result_Status = code; // 2005.04.09
	//===================================================================================
	switch( action ) {
	case ENUM_REJECT_PROCESSJOB_INSERT : // PJ Insert
		sprintf( Buffer , "PROCESSJOB ERR_INSERT|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_CHANGE : // PJ Change
		sprintf( Buffer , "PROCESSJOB ERR_CHANGE|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_UPDATE : // PJ Update
		sprintf( Buffer , "PROCESSJOB ERR_UPDATE|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_DELETE : // PJ Delete
		sprintf( Buffer , "PROCESSJOB ERR_DELETE|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_VERIFY : // PJ Verify
		sprintf( Buffer , "PROCESSJOB ERR_VERIFY|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_PAUSE : // PJ Pause
		sprintf( Buffer , "PROCESSJOB ERR_PAUSE|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_RESUME : // PJ Resume
		sprintf( Buffer , "PROCESSJOB ERR_RESUME|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_STOP : // PJ Stop
		sprintf( Buffer , "PROCESSJOB ERR_STOP|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_ABORT : // PJ Abort
		sprintf( Buffer , "PROCESSJOB ERR_ABORT|%d" , code );
		break;
	case ENUM_REJECT_PROCESSJOB_CANCEL : // PJ Cancel
		sprintf( Buffer , "PROCESSJOB ERR_CANCEL|%d" , code );
		break;
	default :
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 11 , side , pt , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( int action , int code , char *data , int side , int pt ) {
	char Buffer[256];
	//===================================================================================
	Sch_Multi_ProcessJob_Buffer->ResultStatus = 0;
	//
	*Sch_Multi_ProcessJob_Result_Status = 0; // 2005.04.09
	//
	//strncpy( Sch_Multi_ProcessJob_Buffer->JobID , data , 128 ); // 2005.04.09
	//===================================================================================
	switch( action ) {
	case ENUM_ACCEPT_PROCESSJOB_INSERT : // PJ Insert
		sprintf( Buffer , "PROCESSJOB INSERT|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_PROCESSJOB_CHANGE : // PJ Change
		sprintf( Buffer , "PROCESSJOB CHANGE|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_PROCESSJOB_UPDATE : // PJ Update
		sprintf( Buffer , "PROCESSJOB UPDATE|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_PROCESSJOB_DELETE : // PJ Delete
		sprintf( Buffer , "PROCESSJOB DELETE|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_PROCESSJOB_VERIFY : // PJ Verify
		sprintf( Buffer , "PROCESSJOB VERIFY|%s" , data );
		break;
	case ENUM_ACCEPT_PROCESSJOB_PAUSING : // PJ Pause => Pausing (2002.03.20)
		sprintf( Buffer , "PROCESSJOB PAUSING|%s" , data );
		break;
	case ENUM_ACCEPT_PROCESSJOB_RESUME : // PJ Resume
		sprintf( Buffer , "PROCESSJOB RESUME|%s" , data );
		break;
	case ENUM_ACCEPT_PROCESSJOB_STOP : // PJ Stop
		sprintf( Buffer , "PROCESSJOB STOP|%s|%d" , data , code ); // 2004.06.02
		break;
	case ENUM_ACCEPT_PROCESSJOB_ABORT : // PJ Abort
		sprintf( Buffer , "PROCESSJOB ABORT|%s|%d" , data , code ); // 2004.06.02
		break;
	case ENUM_ACCEPT_PROCESSJOB_CANCEL : // PJ Cancel
		sprintf( Buffer , "PROCESSJOB CANCEL|%s" , data );
		break;
	default :
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 12 , side , pt , Buffer );
}
//---------------------------------------------------------------------------------------

void SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( int action , int code , int option , char *data , int side , int pt ) {
//	char Buffer[256]; // 2008.02.13
	char Buffer[2555+127+64]; // 2008.02.13
	switch( action ) {
	case ENUM_NOTIFY_PROCESSJOB_FINISH : // PJ Finished(option==LastStatus)
		sprintf( Buffer , "PROCESSJOB FINISH|%s|%d|%d" , data , code , option );	// [ProcessJobName] / [FinishStyle] / [LastStatus]
		break;
	case ENUM_NOTIFY_PROCESSJOB_START : // PJ START
		sprintf( Buffer , "PROCESSJOB START|%s|%d" , data , code + 1 );	// [ProcessJobName] / [Side]
		break;
	case ENUM_NOTIFY_PROCESSJOB_WAITVERIFY : // PJ Confirm Wait
		sprintf( Buffer , "PROCESSJOB WAITVERIFY|%s" , data );	// [ProcessJobName]
		break;
	case ENUM_NOTIFY_PROCESSJOB_FIRSTSTART : // PJ First Process
		sprintf( Buffer , "PROCESSJOB FIRSTSTART|%s" , data );	// [ProcessJobName:PMName:WaferNo:ProcessRecipeName]
		break;
	case ENUM_NOTIFY_PROCESSJOB_LASTFINISH : // PJ Last Finish
		sprintf( Buffer , "PROCESSJOB LASTFINISH|%s" , data );	// [ProcessJobName:PMName:WaferNo:ProcessRecipeName]
		break;
	case ENUM_NOTIFY_PROCESSJOB_PAUSE : // PJ Paused 2002.03.20
		sprintf( Buffer , "PROCESSJOB PAUSE|%s" , data );	// [ProcessJobName]
		break;
	default :
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 13 , side , pt , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_CONTROLJOB_REJECT( int action , int code , int side , int pt ) {
	char Buffer[256];
	//===================================================================================
	Sch_Multi_ControlJob_Buffer->ResultStatus = code;
	*Sch_Multi_ControlJob_Result_Status = code; // 2005.04.09
	//===================================================================================
	switch( action ) {
	case ENUM_REJECT_CONTROLJOB_CHGSELECT : // CJ ChgSelected
		sprintf( Buffer , "CONTROLJOB ERR_CHGSELECT|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_INSERT : // CJ Insert
		sprintf( Buffer , "CONTROLJOB ERR_INSERT|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_CHANGE : // CJ Change
		sprintf( Buffer , "CONTROLJOB ERR_CHANGE|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_DELETE : // CJ Delete
		sprintf( Buffer , "CONTROLJOB ERR_DELETE|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_HEADOFQ : // CJ HOQ
		sprintf( Buffer , "CONTROLJOB ERR_HEADOFQ|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_DELSELECT : // CJ DELETE Selected
		sprintf( Buffer , "CONTROLJOB ERR_DELSELECT|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_VERIFY : // CJ Verify
		sprintf( Buffer , "CONTROLJOB ERR_VERIFY|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_PAUSE : // CJ Pause
		sprintf( Buffer , "CONTROLJOB ERR_PAUSE|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_RESUME : // CJ Resume
		sprintf( Buffer , "CONTROLJOB ERR_RESUME|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_STOP : // CJ Stop
		sprintf( Buffer , "CONTROLJOB ERR_STOP|%d" , code );
		break;
	case ENUM_REJECT_CONTROLJOB_ABORT : // CJ Abort
		sprintf( Buffer , "CONTROLJOB ERR_ABORT|%d" , code );
		break;
	default :
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 21 , side , pt , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( int action , int code , char *data , int side , int pt ) {
	char Buffer[256];
	//===================================================================================
	Sch_Multi_ControlJob_Buffer->ResultStatus = 0;
	//
	*Sch_Multi_ControlJob_Result_Status = 0; // 2005.04.09
	//
	//strncpy( Sch_Multi_ControlJob_Buffer->JobID , data , 128 ); // 2005.04.09
	//===================================================================================
	switch( action ) {
	case ENUM_ACCEPT_CONTROLJOB_CHGSELECT : // CJ DeSelected
		sprintf( Buffer , "CONTROLJOB CHGSELECT|%s" , data );
		break;
	case ENUM_ACCEPT_CONTROLJOB_INSERT : // CJ Insert
		sprintf( Buffer , "CONTROLJOB INSERT|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_CONTROLJOB_CHANGE : // CJ Change
		sprintf( Buffer , "CONTROLJOB CHANGE|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_CONTROLJOB_DELETE : // CJ Delete
		sprintf( Buffer , "CONTROLJOB DELETE|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_CONTROLJOB_HEADOFQ : // CJ HOQ
		sprintf( Buffer , "CONTROLJOB HEADOFQ|%s|%d" , data , code );
		break;
	case ENUM_ACCEPT_CONTROLJOB_DELSELECT : // CJ DELETE Selected
		sprintf( Buffer , "CONTROLJOB DELSELECT|%s" , data );
		break;
	case ENUM_ACCEPT_CONTROLJOB_VERIFY : // CJ Verify
		sprintf( Buffer , "CONTROLJOB VERIFY|%s" , data );
		break;
	case ENUM_ACCEPT_CONTROLJOB_PAUSE : // CJ Pause
		sprintf( Buffer , "CONTROLJOB PAUSE|%s" , data );
		break;
	case ENUM_ACCEPT_CONTROLJOB_RESUME : // CJ Resume
		sprintf( Buffer , "CONTROLJOB RESUME|%s" , data );
		break;
	case ENUM_ACCEPT_CONTROLJOB_STOP : // CJ Stop
		sprintf( Buffer , "CONTROLJOB STOP|%s" , data );
		break;
	case ENUM_ACCEPT_CONTROLJOB_ABORT : // CJ Abort
		sprintf( Buffer , "CONTROLJOB ABORT|%s" , data );
		break;
	default :
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 22 , side , pt , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( int action , int code , char *data , int side , int pt ) {
	char Buffer[256];
	switch( action ) {
	case ENUM_NOTIFY_CONTROLJOB_SELECT : // CJ Selected
		sprintf( Buffer , "CONTROLJOB SELECT|%s" , data );
		break;
	case ENUM_NOTIFY_CONTROLJOB_FINISH : // CJ Finished
		sprintf( Buffer , "CONTROLJOB FINISH|%s|%d" , data , code ); // 2004.06.02
		break;
	case ENUM_NOTIFY_CONTROLJOB_START : // CJ START
		sprintf( Buffer , "CONTROLJOB START|%s" , data );
		break;
	case ENUM_NOTIFY_CONTROLJOB_WAITVERIFY : // CJ Confirm Wait
		sprintf( Buffer , "CONTROLJOB WAITVERIFY|%s" , data );
		break;
	case ENUM_NOTIFY_CONTROLJOB_CANCEL : // CJ Cancel/Delete Receive // 2002.05.20
		sprintf( Buffer , "CONTROLJOB CANCEL|%s" , data );
		break;
	default :
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 23 , side , pt , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CASSETTE_VERIFY_WAIT_MESSAGE( int side , char *name ) {
	char Buffer[256];
	sprintf( Buffer , "CONTROLJOB WAITMAPVERIFY|CM%d|%s" , side + 1 , name );
	FA_SEND_MESSAGE_TO_SERVER( 24 , side , -1 , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CASSETTE_VERIFY_ERROR_MESSAGE( int side , int code ) {
	char Buffer[256];
	sprintf( Buffer , "CONTROLJOB ERR_MAPVERIFY|CM%d|%d" , side + 1 , code );
	FA_SEND_MESSAGE_TO_SERVER( 24 , side , -1 , Buffer );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CASSETTE_VERIFY_OK_MESSAGE( int side ) {
	char Buffer[256];
	sprintf( Buffer , "CONTROLJOB MAPVERIFY|PORT%d" , side + 1 );
	FA_SEND_MESSAGE_TO_SERVER( 24 , side , -1 , Buffer );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() {
	if ( Sch_Multi_ProcessJob_Buffer->ControlStatus == 1 ) return TRUE;
	return FALSE;
}
//
/*
// 2015.07.02
void SCHMULTI_PROCESSJOB_BUFFER_MAKE_POSSIBLE() { // 2004.06.21
	Sch_Multi_ProcessJob_Buffer->ControlStatus = 1;
}
*/
//
//
BOOL SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() {
	if ( Sch_Multi_ControlJob_Buffer->ControlStatus == 1 ) return TRUE;
	return FALSE;
}
//
void SCHMULTI_CONTROLJOB_BUFFER_MAKE_POSSIBLE() { // 2004.06.21
	Sch_Multi_ControlJob_Buffer->ControlStatus = 1;
}
//
void SCHMULTI_PROCESSJOB_BUFFER_CLEAR() {
	Sch_Multi_ProcessJob_Buffer->ControlStatus = 0;
}
//
void SCHMULTI_CONTROLJOB_BUFFER_CLEAR() {
	Sch_Multi_ControlJob_Buffer->ControlStatus = 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//void SCHMULTI_CASSETTE_VERIFY_CLEAR( int side ) { // 2010.03.25
void SCHMULTI_CASSETTE_VERIFY_CLEAR_LOW( int side , BOOL All ) {
	Sch_Multi_Cassette_Verify[side] = CASSETTE_VERIFY_READY;
	STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID[side]) , "" ); // 2008.04.02 // 2010.03.25
	if ( All ) STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID_for_Start[side]) , "" ); // 2011.03.10
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_CASSETTE_VERIFY_CLEAR( int side , BOOL All ) {
	EnterCriticalSection( &CR_MULTIJOB ); // 2010.03.25
	SCHMULTI_CASSETTE_VERIFY_CLEAR_LOW( side , All );
	LeaveCriticalSection( &CR_MULTIJOB ); // 2010.03.25
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CASSETTE_VERIFY_SET( int side , int data ) {
	if ( ( Sch_Multi_Cassette_Verify[side] == data ) && ( data == 0 ) ) return;
	EnterCriticalSection( &CR_MULTIJOB );
	Sch_Multi_Cassette_Verify[side] = data;
//	if ( data == CASSETTE_VERIFY_READY ) strcpy( Sch_Multi_Cassette_MID[side] , "" ); // 2008.04.02
	if ( data == CASSETTE_VERIFY_READY ) {
		if ( !STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID[side]) , "" ) ) { // 2008.04.02 // 2010.03.25
			_IO_CIM_PRINTF( "SCHMULTI_CASSETTE_VERIFY_SET Memory Allocate Error[%d,%d]\n" , side , data );
		}
	}
	LeaveCriticalSection( &CR_MULTIJOB );
}
//---------------------------------------------------------------------------------------
int SCHMULTI_CASSETTE_VERIFY_GET( int side ) {
	return Sch_Multi_Cassette_Verify[side];
}
//---------------------------------------------------------------------------------------
char *SCHMULTI_CASSETTE_MID_GET( int side ) {
	if ( Sch_Multi_Cassette_MID[side] == NULL ) return COMMON_BLANK_CHAR; // 2008.04.02
	return Sch_Multi_Cassette_MID[side];
}
//---------------------------------------------------------------------------------------
char *SCHMULTI_CASSETTE_MID_GET_FOR_START( int side ) { // 2011.03.10
	if ( Sch_Multi_Cassette_MID_for_Start[side] == NULL ) return COMMON_BLANK_CHAR;
	return Sch_Multi_Cassette_MID_for_Start[side];
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CASSETTE_MID_SET( int side , char *data ) {
//	strncpy( Sch_Multi_Cassette_MID[side] , data , 127 ); // 2008.04.02
	if ( !STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID[side]) , data ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "SCHMULTI_CASSETTE_MID_SET Memory Allocate Error[%d]\n" , side );
	}
	if ( !STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID_for_Start[side]) , data ) ) { // 2011.03.10
		_IO_CIM_PRINTF( "SCHMULTI_CASSETTE_MID_SET Memory Allocate Error 2[%d]\n" , side );
	}
}
//---------------------------------------------------------------------------------------
int SCHMULTI_CASSETTE_VERIFY_MAKE_OK( int side ) {
	EnterCriticalSection( &CR_MULTIJOB );
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_CASSETTE_VERIFY_ERROR_MESSAGE( side , 1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return 1;
	}
	if ( Sch_Multi_Cassette_Verify[side] > CASSETTE_VERIFY_CONFIRMING ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_CASSETTE_VERIFY_ERROR_MESSAGE( side , 2 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return 2;
	}
	//
	Sch_Multi_Cassette_Verify[side] = CASSETTE_VERIFY_CONFIRMING;
	//
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_CASSETTE_VERIFY_OK_MESSAGE( side );
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_SELECTDATA_FULL_MTLCOUNT( int index ) { // 2018.05.10
//	if ( MULTI_RUNJOB_MtlCount(index) >= MAX_CASSETTE_SLOT_SIZE ) return TRUE; // 2018.11.15
	if ( MULTI_RUNJOB_MtlCount(index) >= MAX_RUN_CASSETTE_SLOT_SIZE ) return TRUE; // 2018.11.15
	return FALSE;
}

int SCHMULTI_SELECTDATA_GET_MTLCOUNT( int index ) {
	return MULTI_RUNJOB_MtlCount(index);
}
//
void SCHMULTI_SELECTDATA_SET_SELECTCJINDEX( int index , int data ) { // 2003.07.23
	MULTI_RUNJOB_SelectCJIndex(index) = data;
}
//
void SCHMULTI_SELECTDATA_SET_SELECTCJORDER( int index , int data ) { // 2013.09.27
	MULTI_RUNJOB_SelectCJOrder(index) = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLCOUNT( int index , int data ) {
	MULTI_RUNJOB_MtlCount(index) = data;
}
//
void SCHMULTI_SELECTDATA_SET_RUNSTATUS( int index , int data ) {
	MULTI_RUNJOB_RunStatus(index) = data;
}
//
void SCHMULTI_SELECTDATA_SET_RUNSIDE( int index , int data ) { // 2018.08.30
	MULTI_RUNJOB_RunSide(index) = data;
}
//
void SCHMULTI_SELECTDATA_SET_CASSETTE( int index , int data ) {
	MULTI_RUNJOB_Cassette(index) = data;
}
//
void SCHMULTI_SELECTDATA_SET_CARRIERID( int index , char *data ) {
	strcpy( MULTI_RUNJOB_CarrierID( index ) , data );
}
//
void SCHMULTI_SELECTDATA_SET_MTLCJNAME( int index , char *data ) {
	strcpy( MULTI_RUNJOB_MtlCJName( index ) , data );
}
//
void SCHMULTI_SELECTDATA_SET_MTLCJSTARTMODE( int index , int data ) {
	MULTI_RUNJOB_MtlCJ_StartMode( index ) = data;
}
//
int SCHMULTI_SELECTDATA_GET_MTLINSLOT( int index , int subslot ) {
	return MULTI_RUNJOB_MtlInSlot(index)[subslot];
}
//
int SCHMULTI_SELECTDATA_GET_MTLOUTSLOT( int index , int subslot ) {
	return MULTI_RUNJOB_MtlOutSlot(index)[subslot];
}
//
void SCHMULTI_SELECTDATA_SET_MTLINSLOT( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlInSlot(index)[subslot] = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLINCASSETTE( int index , int subslot , int data ) { // 2011.04.01(Testing)
	MULTI_RUNJOB_MtlInCassette(index)[subslot] = data;
}

void SCHMULTI_SELECTDATA_SET_MTLINCARRIERID( int index , int subslot , char *data ) { // 2011.04.01(Testing)
	strcpy( MULTI_RUNJOB_MtlInCarrierID(index)[subslot] , data );
}

int SCHMULTI_SELECTDATA_GET_MTLINCASSETTE( int index , int subslot ) { // 2011.04.01(Testing)
	return MULTI_RUNJOB_MtlInCassette(index)[subslot];
}

char *SCHMULTI_SELECTDATA_GET_MTLINCARRIERID( int index , int subslot ) { // 2011.04.01(Testing)
	return MULTI_RUNJOB_MtlInCarrierID(index)[subslot];
}
//
void SCHMULTI_SELECTDATA_SET_MTLOUTCASSETTE( int index , int subslot , int data ) { // 2011.02.15(Testing) // 2011.05.21
	MULTI_RUNJOB_MtlOutCassette(index)[subslot] = data;
}

void SCHMULTI_SELECTDATA_SET_MTLOUTCARRIERID( int index , int subslot , char *data ) { // 2011.02.15(Testing) // 2011.05.21
	strcpy( MULTI_RUNJOB_MtlOutCarrierID(index)[subslot] , data );
}

int SCHMULTI_SELECTDATA_GET_MTLOUTCASSETTE( int index , int subslot ) { // 2011.02.15(Testing) // 2011.05.21
	return MULTI_RUNJOB_MtlOutCassette(index)[subslot];
}

char *SCHMULTI_SELECTDATA_GET_MTLOUTCARRIERID( int index , int subslot ) { // 2011.02.15(Testing) // 2011.05.21
	return MULTI_RUNJOB_MtlOutCarrierID(index)[subslot];
}
//
void SCHMULTI_SELECTDATA_SET_MTLOUTSLOT( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlOutSlot(index)[subslot] = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLPJSTARTMODE( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlPJ_StartMode(index)[subslot] = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLPJIDRES( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlPJIDRes(index)[subslot] = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLPJIDEND( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlPJIDEnd(index)[subslot] = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLPJID( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlPJID(index)[subslot] = data;
}
//
int SCHMULTI_SELECTDATA_GET_MTLPJID( int index , int subslot ) {
	return MULTI_RUNJOB_MtlPJID(index)[subslot];
}
//
void SCHMULTI_SELECTDATA_SET_MTLPJNAME( int index , int subslot , char *data ) {
	strcpy( MULTI_RUNJOB_MtlPJName( index )[subslot] , data );
}
//
void SCHMULTI_SELECTDATA_SET_MTLRECIPEMODE( int index , int subslot , int data ) {
	MULTI_RUNJOB_MtlRecipeMode(index)[subslot] = data;
}
//
void SCHMULTI_SELECTDATA_SET_MTLRECIPENAME( int index , int subslot , char *data ) {
	strcpy( MULTI_RUNJOB_MtlRecipeName( index )[subslot] , data );
}

//============================================================================================================
//
// JOBEXT2 UPDATE PART BEGIN
//
void SCHMULTI_SELECTDATA_SET_CASSETTEINDEX( int index , int data ) { // 2018.05.24
	MULTI_RUNJOB_CassetteIndex(index) = data;
}
void SCHMULTI_SELECTDATA_SET_MTLSIDE( int index , int subslot , int data ) { // 2018.05.10
	MULTI_RUNJOB_Mtl_Side(index)[subslot] = data;
}
int  SCHMULTI_SELECTDATA_GET_MTLSIDE( int index , int subslot ) { // 2018.05.10
	return MULTI_RUNJOB_Mtl_Side(index)[subslot];
}
void SCHMULTI_SELECTDATA_SET_MTLPOINTER( int index , int subslot , int data ) { // 2018.05.10
	MULTI_RUNJOB_Mtl_Pointer(index)[subslot] = data;
}
int  SCHMULTI_SELECTDATA_GET_MTLPOINTER( int index , int subslot ) { // 2018.05.10
	return MULTI_RUNJOB_Mtl_Pointer(index)[subslot];
}
void SCHMULTI_SELECTDATA_SET_MTLINCARRIERINDEX( int index , int subslot , int data ) { // 2018.05.10
	MULTI_RUNJOB_MtlInCarrierIndex(index)[subslot] = data;
}
int  SCHMULTI_SELECTDATA_GET_MTLINCARRIERINDEX( int index , int subslot ) { // 2018.05.10
	return MULTI_RUNJOB_MtlInCarrierIndex(index)[subslot];
}
void SCHMULTI_SELECTDATA_SET_MTLOUTCARRIERINDEX( int index , int subslot , int data ) { // 2018.05.10
	MULTI_RUNJOB_MtlOutCarrierIndex(index)[subslot] = data;
}
int  SCHMULTI_SELECTDATA_GET_MTLOUTCARRIERINDEX( int index , int subslot ) { // 2018.05.10
	return MULTI_RUNJOB_MtlOutCarrierIndex(index)[subslot];
}


void SCHMULTI_RUNJOB_DATA_SET( int side , int pt ) { // 2018.05.10
	int r , j;
	//
	r = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pt );
	j = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pt );
	//
	if ( ( r < 0 ) || ( j < 0 ) ) return;
	//
	MULTI_RUNJOB_Mtl_Side(r)[j] = side;
	MULTI_RUNJOB_Mtl_Pointer(r)[j] = pt;
	//
	MULTI_RUNJOB_MtlInCassette(r)[j] = _i_SCH_CLUSTER_Get_PathIn( side , pt );
	MULTI_RUNJOB_MtlInCarrierIndex(r)[j] = _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , pt );
	if ( _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) == NULL ) {
		strcpy( MULTI_RUNJOB_MtlInCarrierID(r)[j] , "" );
	}
	else {
		strcpy( MULTI_RUNJOB_MtlInCarrierID(r)[j] , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pt ) );
	}
	//
	if ( _i_SCH_CLUSTER_Get_PathOut( side , pt ) == 0 ) { // 2018.08.16
		MULTI_RUNJOB_MtlOutCassette(r)[j] = 0;
	}
	else if ( _i_SCH_CLUSTER_Get_PathOut( side , pt ) <= -100 ) {
		MULTI_RUNJOB_MtlOutCassette(r)[j] = ( -_i_SCH_CLUSTER_Get_PathOut( side , pt ) % 100 );
	}
	else {
		MULTI_RUNJOB_MtlOutCassette(r)[j] = _i_SCH_CLUSTER_Get_PathOut( side , pt );
	}
	MULTI_RUNJOB_MtlOutCarrierIndex(r)[j] = _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , pt );
	//
	if ( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , pt ) == NULL ) {
		strcpy( MULTI_RUNJOB_MtlOutCarrierID(r)[j] , "" );
	}
	else {
		strcpy( MULTI_RUNJOB_MtlOutCarrierID(r)[j] , _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , pt ) );
	}
	//
}

//---------------------------------------------------------------------------------------
int SCH_PATHOUT_RESET( int s , int cm ) { // 2018.07.17
	//
	int p , ps , cv , cnt;
	int r,j; // 2018.08.10
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) return -1;
	//
	if ( s < 0 ) return -2;
	if ( s >= MAX_CASSETTE_SIDE ) return -3;
	if ( cm < CM1 ) return -4;
	if ( cm >= PM1 ) return -5;
	//
	if ( BUTTON_GET_FLOW_STATUS_VAR( cm - CM1 ) != _MS_5_MAPPED ) {
		cv = -100 - cm;
	}
	else {
		cv = cm;
	}
	//
	cnt = 0;
	//
	for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
		//
		ps = _i_SCH_CLUSTER_Get_PathStatus( s , p );
		//
		if ( ps == SCHEDULER_CM_END ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathOut( s , p ) >= PM1 ) continue; // 2018.10.05
		//
		_i_SCH_CLUSTER_Set_PathOut( s , p , cv );
		//
		//================================================
		//
		// 2018.08.16
		//
		//
		r = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( s , p );
		j = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p );
		//
		if ( ( r >= 0 ) && ( j >= 0 ) ) {
			//
			MULTI_RUNJOB_MtlOutCassette(r)[j] = cm;
			//
			strcpy( MULTI_RUNJOB_MtlOutCarrierID(r)[j] , SCHMULTI_CASSETTE_MID_GET_FOR_START( cm - CM1 ) );
			//
			MULTI_RUNJOB_MtlOutCarrierIndex( r )[j] = SCHEDULER_Get_CARRIER_INDEX( cm - CM1 );
			//
		}
		//
		//================================================
		//
		cnt++;
		//
	}
	//
	return cnt;
	//
}

//
// JOBEXT2 UPDATE PART END
//
//============================================================================================================
//---------------------------------------------------------------------------------------
void SCHMULTI_SELECTDATA_CLEAR_CHECK_WITH_CJOB_AT_MANUAL( BOOL ManulmultiCJmode ) { // 2004.04.27
	int i , j;
	if ( ManulmultiCJmode ) {
		for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
			if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_SELECTED ) {
				break;
			}
		}
	}
	else {
		if ( Sch_Multi_ControlJob_Select_Data[0]->ControlStatus < CTLJOB_STS_SELECTED ) {
			i = MAX_MULTI_CTJOBSELECT_SIZE;
		}
		else {
			i = 0;
		}
	}
	if ( i == MAX_MULTI_CTJOBSELECT_SIZE ) {
		for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
			SCHMULTI_SELECTDATA_SET_RUNSTATUS( j , CASSRUNJOB_STS_READY );
		}
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int  SCHMULTI_CPJOB_RESET() {
	int i;
	EnterCriticalSection( &CR_MULTIJOB );
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus > CTLJOB_STS_SELECTED ) {
			LeaveCriticalSection( &CR_MULTIJOB );
			return SCH_ERROR_JOB_SELECT_ALREADY;
		}
	}
	*Sch_Multi_ProcessJob_OrderMode = 0;
	
	*Sch_Multi_ProcessJob_Size = 0;
	*Sch_Multi_ControlJob_Size = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_QUEUE_SIZE ; i++ ) {
		Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_NOTSTATE;
		Sch_Multi_ControlJob[i]->ControlStatus = CTLJOB_STS_NOTSTATE;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		for ( i = 0 ; i < MAX_MULTI_PRJOB_EXT_SIZE ; i++ ) {
			Sch_Multi_ProcessJob[i+MAX_MULTI_QUEUE_SIZE]->ControlStatus = PRJOB_STS_NOTSTATE;
		}
	}
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		Sch_Multi_ControlJob_Select_Data[i]->ControlStatus = CTLJOB_STS_NOTSTATE;
		Sch_Multi_ControlJob_Selected_Stop_Signal[i] = FALSE;
	}
	//
	Sch_Multi_ProcessJob_Buffer->ControlStatus = 0;
	Sch_Multi_ControlJob_Buffer->ControlStatus = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
	}
	//----------------------------------------------------------------------
	SCHMULTI_CTJOB_IO_SET();
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	SCHMULTI_PRJOB_IO_SET();
	//----------------------------------------------------------------------
	LeaveCriticalSection( &CR_MULTIJOB );
	return 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_GET_PROCESSJOB_ERRORDATA( char *data ) { // 2015.03.27
	int i;
	char Buffer[32];
	//
	// "NAME:4"
	//
	for ( i = 0 ; i < 31 ; i++ ) {
		//
		if ( Sch_Multi_ProcessJob_Buffer->JobID[i] == 0 ) break;
		//
		Buffer[i] = Sch_Multi_ProcessJob_Buffer->JobID[i];
	}
	//
	Buffer[i] = 0;
	//
	sprintf( data , "%s:%d" , Buffer , Sch_Multi_PRJOB_Buf_Sub_Error );
	//
}

void SCHMULTI_GET_CONTROLJOB_ERRORDATA( char *data ) { // 2015.03.27
	int i;
	char Buffer[32];
	//
	// "NAME:4"
	//
	for ( i = 0 ; i < 31 ; i++ ) {
		//
		if ( Sch_Multi_ControlJob_Buffer->JobID[i] == 0 ) break;
		//
		Buffer[i] = Sch_Multi_ControlJob_Buffer->JobID[i];
	}
	//
	Buffer[i] = 0;
	//
	sprintf( data , "%s:%d" , Buffer , Sch_Multi_CJJOB_Buf_Sub_Error );
	//
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_PRJOB_IO_SET() {
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		IO_ADD_WRITE_DIGITAL( Address_IO_PRJob_Remain , ( MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ) - *Sch_Multi_ProcessJob_Size );
	}
	else {
		IO_ADD_WRITE_DIGITAL( Address_IO_PRJob_Remain , MAX_MULTI_QUEUE_SIZE - *Sch_Multi_ProcessJob_Size );
	}
	//
}
//---------------------------------------------------------------------------------------
void SCHMULTI_PROCESSJOB_SET_STATUS( int mode , char *name , int data ) {
	int i;
	if ( mode >= 0 ) {
		Sch_Multi_ProcessJob[mode]->ControlStatus = data;
	}
	else if ( mode == -99 ) {
		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			Sch_Multi_ProcessJob[i]->ControlStatus = data;
		}
	}
	else {
		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			if ( STRCMP_L( name , Sch_Multi_ProcessJob[i]->JobID ) ) break;
		}
		if ( i == *Sch_Multi_ProcessJob_Size ) return;
		Sch_Multi_ProcessJob[i]->ControlStatus = data;
	}
}

int SCHMULTI_PROCESSJOB_GET_STATUS( int mode , char *name ) { // 2004.10.14
	int i;
	if ( mode >= 0 ) {
		return Sch_Multi_ProcessJob[mode]->ControlStatus;
	}
	else {
		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			if ( STRCMP_L( name , Sch_Multi_ProcessJob[i]->JobID ) ) break;
		}
		if ( i == *Sch_Multi_ProcessJob_Size ) return -1;
		return Sch_Multi_ProcessJob[i]->ControlStatus;
	}
}

int SCHMULTI_PROCESSJOB_GET_LAST_STATUS( char *name ) {
	int i;
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( name , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) return -1;
	return ( Sch_Multi_ProcessJob[i]->ControlStatus );
}

//---------------------------------------------------------------------------------------
int SCHMULTI_PROCESSJOB_Find( char *JobID ) {
	int i;
	if ( *Sch_Multi_ProcessJob_Size <= 0 ) return -1;
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) return -1;
	return i;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_PROCESSJOB_Data_Check( int all ) {
	int i , j , k , p , maxslot , maxunuse;
	//
	if ( all == 1 ) {
		if ( Sch_Multi_ProcessJob_Buffer->StartMode < 0 ) return 1;
		if ( Sch_Multi_ProcessJob_Buffer->StartMode > 1 ) return 2;
		for ( i = 0 ; i < MAX_MULTI_PRJOB_CASSETTE ; i++ ) {
			if ( Sch_Multi_ProcessJob_Buffer->MtlUse[i] ) break;
		}
		if ( i == MAX_MULTI_PRJOB_CASSETTE ) return 3;
		if ( strlen( Sch_Multi_ProcessJob_Buffer->MtlRecipeName ) <= 0 ) return 4;
		//
		maxslot = Scheduler_Get_Max_Slot( -1 , -1 , -1 , 31 , &maxunuse ); // 2010.12.17
		//
		for ( i = 0 ; i < MAX_MULTI_PRJOB_CASSETTE ; i++ ) {
			if ( Sch_Multi_ProcessJob_Buffer->MtlUse[i] ) {
				//
				/*
				// 2011.12.14
				if ( strlen( Sch_Multi_ProcessJob_Buffer->MtlCarrName[i] ) <= 0 ) { // 2008.04.23
					if ( Sch_Multi_ProcessJob_Buffer->MtlFormat < 100 ) { // 2008.04.23
						return FALSE;
					}
					// 2008.04.23
					for ( p = 0 ; p < *Sch_Multi_ProcessJob_Size ; p++ ) {
						if ( Sch_Multi_ProcessJob_Buffer->MtlFormat == Sch_Multi_ProcessJob[p]->MtlFormat ) return FALSE;
					}
					//
				}
				*/
				// 2011.12.14
				if ( Sch_Multi_ProcessJob_Buffer->MtlFormat >= 100 ) {
					for ( p = 0 ; p < *Sch_Multi_ProcessJob_Size ; p++ ) {
						if ( Sch_Multi_ProcessJob_Buffer->MtlFormat == Sch_Multi_ProcessJob[p]->MtlFormat ) return 5;
					}
				}
				else {
					if ( strlen( Sch_Multi_ProcessJob_Buffer->MtlCarrName[i] ) <= 0 ) {
						return 6;
					}
				}
				//
				for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
					if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[i][j] > maxslot ) return 7; // 2010.12.17
					if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[i][j] > 0 ) {
						for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[i][k] > 0 ) {
								if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[i][j] == Sch_Multi_ProcessJob_Buffer->MtlSlot[i][k] ) return 8;
							}
						}
					}
				}
			}
		}
	}
	else if ( all == 2 ) {
		if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[0][0] <= 0 ) return 11;
	}
	//
	/*
	// 2008.02.13
	if ( Sch_Multi_ProcessJob_Buffer->MtlRecipeMode != 0 ) {
		k = 0;
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE ; i++ ) {
			if ( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneModule[i] > 0 ) {
				k++;
				if ( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] < 0 ) return FALSE;
				if ( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] > 255 ) return FALSE;
				if ( strlen( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName[i] ) <= 0 ) return FALSE;
			}
			else {
				Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] = 0;
				strcpy( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName[i] , "" );
				strcpy( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneData[i] , "" );
			}
		}
		if ( all ) { // 2008.02.13
			if ( k == 0 ) Sch_Multi_ProcessJob_Buffer->MtlRecipeMode = 0;
		}
	}
	*/
	// 2008.02.13
	for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE ; i++ ) {
		if ( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneModule[i] > 0 ) {
			if ( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] < 0 ) return 21;
			if ( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] > 255 ) return 22;
			if ( strlen( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName[i] ) <= 0 ) return 23;
		}
		else {
			Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] = 0;
			strcpy( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName[i] , "" );
			strcpy( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneData[i] , "" );
		}
	}
	//----------
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; i++ ) {
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneModule[i] > 0 ) {
				if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneStep[i] < 0 ) return 31;
				if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneStep[i] > 255 ) return 32;
				if ( strlen( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneName[i] ) <= 0 ) return 33;
			}
			else {
				strcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneName[i] , "" );
				strcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneData[i] , "" );
			}
		}
	}
	//----------
	return 0;
}
//---------------------------------------------------------------------------------------
//int  SCHMULTI_PROCESSJOB_INSERT( int Cass ) { // 2016.09.01
int  SCHMULTI_PROCESSJOB_INSERT( int Cass , BOOL LockingNoError ) { // 2016.09.01
	int i , Res;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 1; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		if ( *Sch_Multi_ProcessJob_Size >= ( MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ) ) {
			//
			Sch_Multi_PRJOB_Buf_Sub_Error = 11; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_QUEUE_FULL , -1 , -1 );
			SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_FULL;
		}
	}
	else {
		if ( *Sch_Multi_ProcessJob_Size >= MAX_MULTI_QUEUE_SIZE ) {
			//
			Sch_Multi_PRJOB_Buf_Sub_Error = 21; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_QUEUE_FULL , -1 , -1 );
			SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_FULL;
		}
	}
	//
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) {
			//
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.06.27
				if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[0][0] != Sch_Multi_ProcessJob[i]->MtlSlot[0][0] ) continue;
			}
			//
			Sch_Multi_PRJOB_Buf_Sub_Error = 11; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_QUEUE_ALREADY_REGIST , -1 , -1 );
			SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_ALREADY_REGIST;
		}
		//
		if ( Cass >= 0 ) { // 2011.04.12
			if ( ( Cass + 100 ) == Sch_Multi_ProcessJob[i]->MtlFormat ) {
				//
				Sch_Multi_PRJOB_Buf_Sub_Error = 21; // 2015.03.27
				//
//				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_QUEUE_ALREADY_REGIST , -1 , -1 );
				SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				return SCH_ERROR_QUEUE_ALREADY_REGIST;
			}
		}
		//
	}
	//
//	if ( !SCHMULTI_PROCESSJOB_Data_Check( TRUE ) ) { // 2011.06.15
//	if ( SCHMULTI_PROCESSJOB_Data_Check( ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) ? 2 : 1 ) != 0 ) { // 2011.06.15 // 2013.04.02

	Res = SCHMULTI_PROCESSJOB_Data_Check( ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) ? 2 : 1 ); // 2013.04.02
	if ( Res != 0 ) { // 2011.06.15
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = ( Res * 100 ) + 10 + 1; // 2013.04.02
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//----------------------------------------------------------------------
//	if ( SCHMULTI_RCPLOCKING_PROCESSJOB( 1 , Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob_Buffer->MtlRecipeName , Sch_Multi_ProcessJob_Buffer->MtlFormat >= 100 ) <= 0 ) { // 2011.09.20 // 2013.04.02
//	Res = SCHMULTI_RCPLOCKING_PROCESSJOB( 1 , Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob_Buffer->MtlRecipeName , Sch_Multi_ProcessJob_Buffer->MtlFormat >= 100 ); // 2011.09.20 // 2013.04.02 // 2016.09.01
	Res = SCHMULTI_RCPLOCKING_PROCESSJOB( 1 , Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob_Buffer->MtlRecipeName , Sch_Multi_ProcessJob_Buffer->MtlFormat >= 100 , LockingNoError ); // 2011.09.20 // 2013.04.02 // 2016.09.01
	if ( Res <= 0 ) { // 2011.09.20 // 2013.04.02
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = ( Res * -100 ) + 20 + 1; // 2013.04.02
		//
//		SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , Sch_Multi_ProcessJob_Buffer->JobID , "" , FALSE ); // 2012.06.10 // 2016.09.01
		SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , Sch_Multi_ProcessJob_Buffer->JobID , "" , FALSE , FALSE ); // 2012.06.10 // 2016.09.01
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_INSERT , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//----------------------------------------------------------------------
	//
	memcpy( Sch_Multi_ProcessJob[*Sch_Multi_ProcessJob_Size] , Sch_Multi_ProcessJob_Buffer , sizeof(Scheduling_Multi_ProcessJob) );
	//
	time( &(Sch_Multi_Ins_ProcessJob[*Sch_Multi_ProcessJob_Size].InsertTime) ); // 2012.04.19
	//
	Sch_Multi_Ins_ProcessJob[*Sch_Multi_ProcessJob_Size].FixedResult = PRJOB_RESULT_NORMAL; // 2018.11.14
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
		memcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[*Sch_Multi_ProcessJob_Size] , Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ProcessJob_Ext[*Sch_Multi_ProcessJob_Size] , Sch_Multi_ProcessJob_Ext_Buffer , sizeof(Scheduling_Multi_PrJob_Ext) );
	}
	//
	Sch_Multi_ProcessJob[*Sch_Multi_ProcessJob_Size]->ControlStatus = PRJOB_STS_QUEUED;
	Sch_Multi_ProcessJob[*Sch_Multi_ProcessJob_Size]->ResultStatus = 0;
	//
	(*Sch_Multi_ProcessJob_Size)++;
	//----------------------------------------------------------------------
	SCHMULTI_PRJOB_IO_SET();
	//----------------------------------------------------------------------
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_INSERT , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob_Buffer->JobID , -1 , -1 );
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_CHANGE() {
	int i , c , d , Res;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 2; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( *Sch_Multi_ProcessJob_Size <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_EMPTY;
	}
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) {
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2011.06.27
				break;
			}
			else {
				if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[0][0] == Sch_Multi_ProcessJob[i]->MtlSlot[0][0] ) {
					break;
				}
			}
		}
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	if ( Sch_Multi_ProcessJob[i]->ControlStatus >= PRJOB_STS_SETTINGUP ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_QUEUE_DATA_RUNNING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_RUNNING;
	}
	//
	Res = SCHMULTI_PROCESSJOB_Data_Check( ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) ? 2 : 1 );
	if ( Res != 0 ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = ( Res * 100 ) + 10 + 2; // 2013.04.02
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//----------------------------------------------------------------------
//	Res = SCHMULTI_RCPLOCKING_PROCESSJOB( 1 , Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob_Buffer->MtlRecipeName , Sch_Multi_ProcessJob_Buffer->MtlFormat >= 100 ); // 2011.09.20 // 2016.09.01
	Res = SCHMULTI_RCPLOCKING_PROCESSJOB( 1 , Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob_Buffer->MtlRecipeName , Sch_Multi_ProcessJob_Buffer->MtlFormat >= 100 , FALSE ); // 2011.09.20 // 2016.09.01
	if ( Res <= 0 ) { // 2011.09.20
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = ( Res * -100 ) + 20 + 2; // 2013.04.02
		//
//		SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , Sch_Multi_ProcessJob_Buffer->JobID , "" , FALSE ); // 2012.06.10 // 2016.09.01
		SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , Sch_Multi_ProcessJob_Buffer->JobID , "" , FALSE , FALSE ); // 2012.06.10 // 2016.09.01
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_CHANGE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//----------------------------------------------------------------------
	c = Sch_Multi_ProcessJob[i]->ControlStatus;
	d = Sch_Multi_ProcessJob[i]->ResultStatus;
	//
	memcpy( Sch_Multi_ProcessJob[i] , Sch_Multi_ProcessJob_Buffer , sizeof(Scheduling_Multi_ProcessJob) );
	//
	time( &(Sch_Multi_Ins_ProcessJob[i].InsertTime) ); // 2012.04.19
	//
	Sch_Multi_Ins_ProcessJob[i].FixedResult = PRJOB_RESULT_NORMAL; // 2018.11.14
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
		memcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ProcessJob_Ext[i] , Sch_Multi_ProcessJob_Ext_Buffer , sizeof(Scheduling_Multi_PrJob_Ext) );
	}
	//
	Sch_Multi_ProcessJob[i]->ControlStatus = c;
	Sch_Multi_ProcessJob[i]->ResultStatus = d;
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_CHANGE , i , Sch_Multi_ProcessJob_Buffer->JobID , -1 , -1 );
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_UPDATE() { // 2007.12.09
	int i , j , Res;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 3; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_UPDATE , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( *Sch_Multi_ProcessJob_Size <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_UPDATE , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_EMPTY;
	}
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_UPDATE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) {
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2011.06.27
				break;
			}
			else {
				if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[0][0] == Sch_Multi_ProcessJob[i]->MtlSlot[0][0] ) {
					break;
				}
			}
		}
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_UPDATE , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
		//
	}

//	if ( Sch_Multi_ProcessJob[i]->ControlStatus >= PRJOB_STS_SETTINGUP ) {
//		LeaveCriticalSection( &CR_MULTIJOB );
//		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_UPDATE , SCH_ERROR_QUEUE_DATA_RUNNING );
//		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
//		return SCH_ERROR_QUEUE_DATA_RUNNING;
//	}
	//
	Res = SCHMULTI_PROCESSJOB_Data_Check( ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) ? 2 : 0 );
	if ( Res != 0 ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = ( Res * 100 ) + 10 + 3; // 2013.04.02
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_UPDATE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//----------
	Sch_Multi_ProcessJob[i]->MtlRecipeMode = Sch_Multi_ProcessJob_Buffer->MtlRecipeMode;
	//
	for ( j = 0 ; j < MAX_MULTI_PRJOB_RCPTUNE ; j++ ) {
		Sch_Multi_ProcessJob[i]->MtlRcpTuneModule[j] = Sch_Multi_ProcessJob_Buffer->MtlRcpTuneModule[j];
		Sch_Multi_ProcessJob[i]->MtlRcpTuneStep[j]   = Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[j];
		//
		memcpy( Sch_Multi_ProcessJob[i]->MtlRcpTuneName[j] , Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName[j] , MAX_MULTI_STRING_SIZE + 1 );
		memcpy( Sch_Multi_ProcessJob[i]->MtlRcpTuneData[j] , Sch_Multi_ProcessJob_Buffer->MtlRcpTuneData[j] , MAX_MULTI_STRING_SIZE + 1 );
	}
	//----------
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
		for ( j = 0 ; j < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; j++ ) {
			Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneModule[j] = Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneModule[j];
			Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneOrder[j]  = Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneOrder[j];
			Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneStep[j]   = Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneStep[j];
			Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneIndex[j]  = Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneIndex[j];
			//
			memcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneName[j] , Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneName[j] , MAX_MULTI_STRING_SIZE + 1 );
			memcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneData[j] , Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneData[j] , MAX_MULTI_STRING_SIZE + 1 );
		}
	}
	//----------
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_UPDATE , i , Sch_Multi_ProcessJob_Buffer->JobID , -1 , -1 );
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_DELETE( int dt , int index ) {
	int i , os , dmr , dma;
	char DelPID[256];
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 4; // 2013.04.02
	//
	if      ( dt == 0 ) { // Stop
		dmr = ENUM_REJECT_PROCESSJOB_STOP;
		dma = ENUM_ACCEPT_PROCESSJOB_STOP;
	}
	else if ( dt == 1 ) { // abort
		dmr = ENUM_REJECT_PROCESSJOB_ABORT;
		dma = ENUM_ACCEPT_PROCESSJOB_ABORT;
	}
	else if ( dt == 2 ) { // Cancel
		dmr = ENUM_REJECT_PROCESSJOB_CANCEL;
		dma = ENUM_ACCEPT_PROCESSJOB_CANCEL;
	}
	else if ( dt == 3 ) { // Delete
		dmr = ENUM_REJECT_PROCESSJOB_DELETE;
		dma = ENUM_ACCEPT_PROCESSJOB_DELETE;
	}
	else { // Delete
		dmr = ENUM_REJECT_PROCESSJOB_DELETE;
		dma = ENUM_ACCEPT_PROCESSJOB_DELETE;
	}
	//
	if ( dt >= 0 ) {
		//
		if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_PROCESSJOB_REJECT( dmr , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_NOT_GOOD_DATA;
		}
		//
	}
	if ( *Sch_Multi_ProcessJob_Size <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( dmr , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
		if ( dt >= 0 ) SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_EMPTY;
	}
	//
	if ( index == -1 ) { // 2005.06.10
		if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_PROCESSJOB_REJECT( dmr , SCH_ERROR_ERROR_DATA , -1 , -1 );
			if ( dt >= 0 ) SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_ERROR_DATA;
		}
		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
		}
		if ( i == *Sch_Multi_ProcessJob_Size ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_PROCESSJOB_REJECT( dmr , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
			if ( dt >= 0 ) SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_CANNOT_FIND;
		}
	}
	else { // 2005.05.10
		i = index;
	}
	//
	strncpy( DelPID , Sch_Multi_ProcessJob[i]->JobID , 255 );
	os = Sch_Multi_ProcessJob[i]->ControlStatus; // 2002.05.20
	//
	if ( Sch_Multi_ProcessJob[i]->ResultStatus > 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( dmr , SCH_ERROR_QUEUE_DATA_USING , -1 , -1 );
		if ( dt >= 0 ) SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_USING;
	}
	if ( Sch_Multi_ProcessJob[i]->ControlStatus >= PRJOB_STS_SETTINGUP ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( dmr , SCH_ERROR_QUEUE_DATA_RUNNING , -1 , -1 );
		if ( dt >= 0 ) SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_RUNNING;
	}
	//
	for ( ; i < *Sch_Multi_ProcessJob_Size - 1; i++ ) {	
		//
		memcpy( Sch_Multi_ProcessJob[i] , Sch_Multi_ProcessJob[i+1] , sizeof(Scheduling_Multi_ProcessJob) );
		//
		memcpy( &(Sch_Multi_Ins_ProcessJob[i]) , &(Sch_Multi_Ins_ProcessJob[i+1]) , sizeof(Scheduling_Multi_Ins_ProcessJob) ); // 2012.04.19
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
			memcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[i+1] , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
		}
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ProcessJob_Ext[i] , Sch_Multi_ProcessJob_Ext[i+1] , sizeof(Scheduling_Multi_PrJob_Ext) );
		}
		//
	}
	//
	//----------------------------------------------------------------------
//	SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , DelPID , "" , FALSE ); // 2011.09.20 // 2016.09.01
	SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , DelPID , "" , FALSE , FALSE ); // 2011.09.20 // 2016.09.01
	//----------------------------------------------------------------------
	SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_NOTSTATE );
	(*Sch_Multi_ProcessJob_Size)--;
	//----------------------------------------------------------------------
	SCHMULTI_PRJOB_IO_SET();
	//----------------------------------------------------------------------
	SCHMULTI_RUNJOB_PJID_DATA_REBUILD(); // 2005.05.19
	//----------------------------------------------------------------------
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//----------------------------------------------------------------------------------------------------------------------------
	// Spec Difference(Next Option Operation Need) // 2005.06.11
	//----------------------------------------------------------------------------------------------------------------------------
	if ( dt >= 0 ) {
		//
		if ( dt == 3 ) { // 2005.06.11
			SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( dma , *Sch_Multi_ProcessJob_Size , DelPID , -1 , -1 );
		} // 2005.06.11
		//====================================
		// Append 2002.05.20
		//====================================
		if ( dt != 3 ) {
			SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_CANCELDELETE , os , Sch_Multi_ProcessJob_Buffer->JobID , -1 , -1 );
			//====================================
			SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_DELETE , *Sch_Multi_ProcessJob_Size , DelPID , -1 , -1 );
		}
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		//
	}
	else {
		SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( dma , *Sch_Multi_ProcessJob_Size , DelPID , -1 , -1 );
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_PAUSE() {
	int i , Res;

	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 5; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_PAUSE , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_PAUSE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_PAUSE , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	if ( Sch_Multi_ProcessJob[i]->ResultStatus <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_PAUSE , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	if (
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PROCESSING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_EXECUTING ) ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 15; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_PAUSE , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	// Pause Operation
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_PAUSE( 0 , Sch_Multi_ProcessJob[i]->JobID );
	if ( Res == 0 ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 25; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_PAUSE , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	//
	SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_PAUSING );
//	SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_PAUSE ); // 2002.03.20
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_PAUSING , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[i]->JobID , -1 , -1 );
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_RESUME() {
	int i , Res;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 6; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_RESUME , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_RESUME , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_RESUME , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	if ( Sch_Multi_ProcessJob[i]->ResultStatus <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_RESUME , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	if (
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PAUSING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PAUSE ) ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 16; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_RESUME , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	// Resume Operation
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_RESUME( 0 , Sch_Multi_ProcessJob[i]->JobID );
	if ( Res == 0 ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 26; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_RESUME , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	//
	SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_EXECUTING );
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_RESUME , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[i]->JobID , -1 , -1 );
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_STOP() {
	int i , Res , pdt;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 7; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_STOP , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_STOP , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_STOP , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	if ( Sch_Multi_ProcessJob[i]->ResultStatus <= 0 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCHMULTI_PROCESSJOB_DELETE( 0 , i );
	}
	if (
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PAUSING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PAUSE ) && // 2004.06.03
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PROCESSING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_EXECUTING ) ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 17; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_STOP , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	//----------------------------------------------------------------------------
	// 2004.06.02
	//----------------------------------------------------------------------------
	if      ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_PAUSING )
		pdt = 1;
	else if ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_PAUSE )
		pdt = 1;
	else
		pdt = 0;
	//----------------------------------------------------------------------------
	// Sch_Multi_ProcessJob[i]->JobID Stop Operation
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_STOP( 0 , Sch_Multi_ProcessJob[i]->JobID );
	if ( Res == 0 ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 27; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_STOP , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	//
	SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_STOPPING );
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
//	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_STOP , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[i]->JobID ); // 2004.06.02
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_STOP , pdt , Sch_Multi_ProcessJob[i]->JobID , -1 , -1 ); // 2004.06.02
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_ABORT() {
	int i , Res;
//	int pdt; // 2004.06.15
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 8; // 2013.04.02
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_ABORT , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( strlen( Sch_Multi_ProcessJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_ABORT , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_ABORT , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	if ( Sch_Multi_ProcessJob[i]->ResultStatus <= 0 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCHMULTI_PROCESSJOB_DELETE( 1 , i );
	}
	if (
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_WAITINGFORSTART ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PROCESSING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_EXECUTING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PAUSING ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_PAUSE ) &&
		( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_STOPPING ) ) {
		//
		Sch_Multi_PRJOB_Buf_Sub_Error = 18; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_ABORT , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
//	//===================================================================================================
//	// 2004.06.02 // 2004.06.15
//	//===================================================================================================
//	if      ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_STOPPING )
//		pdt = 2;
//	else if ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_PAUSING )
//		pdt = 1;
//	else if ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_PAUSE )
//		pdt = 1;
//	else
//		pdt = 0;
//	//===================================================================================================
//	SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_ABORTING ); // 2005.06.12
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
//	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_ABORT , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[i]->JobID ); // 2004.06.02
//	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_ABORT , pdt , Sch_Multi_ProcessJob[i]->JobID ); // 2004.06.02 // 2004.06.15
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_ABORT( 0 , Sch_Multi_ProcessJob[i]->JobID ); // 2004.06.15
//	SCHMULTI_PROCESSJOB_BUFFER_CLEAR(); // 2004.06.15
	//---------------------------------------------------------------------------
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = (Res * 100) + 8; // 2013.04.02
	//
	// Abort Operation
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( i );
	Scheduler_All_Abort_Part( TRUE ); // 2003.06.03
//	SYSTEM_ALL_ABORT(); // 2003.06.03
	//
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR(); // 2004.06.15
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
//void SCHMULTI_PROCESSJOB_WAIT_TO_FINISH() {
//	int i;
//	EnterCriticalSection( &CR_MULTIJOB );
//	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
//		switch( Sch_Multi_ProcessJob[i]->ControlStatus ) {
//		case PRJOB_STS_SETTINGUP :
//		case PRJOB_STS_WAITINGFORSTART :
//			//===========================================================
//			//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
//			PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_FORCEFINISH , "" , Sch_Multi_ProcessJob[i]->JobID ); // 2002.05.20
//			//===========================================================
//			SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_FORCEFINISH , Sch_Multi_ProcessJob[i]->ControlStatus , Sch_Multi_ProcessJob[i]->JobID );
//			break;
//		}
//		Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_QUEUED;
//	}
//	LeaveCriticalSection( &CR_MULTIJOB );
//}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_PROCESSJOB_FORCE_FINISH_CHECK( int index ) {
	int i , Res , Find;
	EnterCriticalSection( &CR_MULTIJOB );
	Find = FALSE;
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[index]->CtrlSpec_Use[i] ) {
			Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob_Select_Data[index]->CtrlSpec_ProcessJobID[i] );
			if ( Res >= 0 ) {
				switch( Sch_Multi_ProcessJob[Res]->ControlStatus ) {
				case PRJOB_STS_SETTINGUP :
				case PRJOB_STS_WAITINGFORSTART :
					//===========================================================
					PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_FORCEFINISH , Sch_Multi_ControlJob_Select_Data[index]->JobID , Sch_Multi_ProcessJob[Res]->JobID );
					//===========================================================
					SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_FORCEFINISH , Sch_Multi_ProcessJob[Res]->ControlStatus , Sch_Multi_ProcessJob[Res]->JobID , -1 , -1 );
					//===========================================================
					Find = TRUE;
					break;
				}
				Sch_Multi_ProcessJob[Res]->ControlStatus = PRJOB_STS_QUEUED;
			}
		}
	}
	LeaveCriticalSection( &CR_MULTIJOB );
	return Find;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_PROCESSJOB_Reference( BOOL Cancel , char *JobID , int data , BOOL DelCheck , BOOL ForceDel ) {
	int i;
	if ( *Sch_Multi_ProcessJob_Size <= 0 ) return -1;
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) return -1;
	Sch_Multi_ProcessJob[i]->ResultStatus += data;
	if ( Sch_Multi_ProcessJob[i]->ResultStatus < 0 ) Sch_Multi_ProcessJob[i]->ResultStatus = 0;
	//====================================
	if ( Cancel ) { // 2002.05.20
		SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_CANCELDELETE , Sch_Multi_ProcessJob[i]->ControlStatus , JobID , -1 , -1 );
	}
	//====================================
	if ( DelCheck ) {
//		if ( ( Sch_Multi_ProcessJob[i]->ResultStatus == 0 ) && ( ForceDel ) ) { // 2004.06.28
		if ( Sch_Multi_ProcessJob[i]->ResultStatus == 0 ) {
			if ( ForceDel ) { // 2004.06.28
				//
				SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_DELETE , *Sch_Multi_ProcessJob_Size - 1 , JobID , -1 , -1 );
				//
				for ( ; i < *Sch_Multi_ProcessJob_Size - 1; i++ ) {
					//
					memcpy( Sch_Multi_ProcessJob[i] , Sch_Multi_ProcessJob[i+1] , sizeof(Scheduling_Multi_ProcessJob) );
					//
					memcpy( &(Sch_Multi_Ins_ProcessJob[i]) , &(Sch_Multi_Ins_ProcessJob[i+1]) , sizeof(Scheduling_Multi_Ins_ProcessJob) ); // 2012.04.19
					//
					if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
						memcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[i+1] , sizeof(Scheduling_Multi_ProcessJob_TuneData) );
					}
					//
					if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
						memcpy( Sch_Multi_ProcessJob_Ext[i] , Sch_Multi_ProcessJob_Ext[i+1] , sizeof(Scheduling_Multi_PrJob_Ext) );
					}
					//
				}
				//
				//----------------------------------------------------------------------
//				SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , JobID , "" , FALSE ); // 2011.09.20 // 2016.09.01
				SCHMULTI_RCPLOCKING_PROCESSJOB( 0 , JobID , "" , FALSE , FALSE ); // 2011.09.20 // 2016.09.01
				//----------------------------------------------------------------------
				SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_NOTSTATE );
				(*Sch_Multi_ProcessJob_Size)--;
				//----------------------------------------------------------------------
				SCHMULTI_PRJOB_IO_SET();
				//----------------------------------------------------------------------
				SCHMULTI_RUNJOB_PJID_DATA_REBUILD(); // 2005.05.19
				//----------------------------------------------------------------------
			}
			else {
				SCHMULTI_PROCESSJOB_SET_STATUS( i , "" , PRJOB_STS_QUEUED );
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------------
//int SCHMULTI_PROCESSJOB_ControlStatus( char *JobID , int data ) {
//	int i;
//	if ( *Sch_Multi_ProcessJob_Size <= 0 ) return -1;
//	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
//		if ( STRCMP_L( JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
//	}
//	if ( i == *Sch_Multi_ProcessJob_Size ) return -1;
//	Sch_Multi_ProcessJob[i]->ControlStatus = data;
//	return 0;
//}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_MANUAL_INSERT_and_SETUP( int Cass , char *pjname , char *recipename , char *midname ) {
	int i , j , index;
	EnterCriticalSection( &CR_MULTIJOB );
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( pjname , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i != *Sch_Multi_ProcessJob_Size ) {
		//----------------------------------------------------------------------
//		if ( SCHMULTI_RCPLOCKING_PROCESSJOB( 2 , pjname , recipename ) <= 0 ) { // 2011.09.20 // 2012.06.28
//			LeaveCriticalSection( &CR_MULTIJOB );
//			return -1;
//		}
		//----------------------------------------------------------------------
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		//----------------------------------------------------------------------
		// Setting Up // 2004.04.27
		//----------------------------------------------------------------------
		/*
		// 2011.09.20
		if ( Sch_Multi_ProcessJob[i]->ControlStatus <= PRJOB_STS_QUEUED ) {
			Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_SETTINGUP;
		}
		else if ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_PROCESSCOMPLETED ) {
			Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_SETTINGUP;
		}
		*/
		strcpy( Sch_Multi_ProcessJob[i]->MtlRecipeName , recipename );
		Sch_Multi_ProcessJob[i]->MtlUse[0] = TRUE;
		strcpy( Sch_Multi_ProcessJob[i]->MtlCarrName[0] , midname );
//		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
		for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) { // 2018.05.10
			Sch_Multi_ProcessJob[i]->MtlSlot[0][j] = 0;
		}
		// 2011.09.20
		if ( Sch_Multi_ProcessJob[i]->ControlStatus <= PRJOB_STS_QUEUED ) {
			Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_SETTINGUP;
		}
		else if ( Sch_Multi_ProcessJob[i]->ControlStatus == PRJOB_STS_PROCESSCOMPLETED ) {
			Sch_Multi_ProcessJob[i]->ControlStatus = PRJOB_STS_SETTINGUP;
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		//
		return i;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		if ( *Sch_Multi_ProcessJob_Size >= ( MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ) ) {
			LeaveCriticalSection( &CR_MULTIJOB );
			return -1;
		}
	}
	else {
		if ( *Sch_Multi_ProcessJob_Size >= MAX_MULTI_QUEUE_SIZE ) {
			LeaveCriticalSection( &CR_MULTIJOB );
			return -1;
		}
	}
	//----------------------------------------------------------------------
//	if ( SCHMULTI_RCPLOCKING_PROCESSJOB( 2 , pjname , recipename ) <= 0 ) { // 2011.09.20 // 2012.06.28
//		LeaveCriticalSection( &CR_MULTIJOB );
//		return -1;
//	}
	//----------------------------------------------------------------------
	//
	index = *Sch_Multi_ProcessJob_Size;
	//
	strcpy( Sch_Multi_ProcessJob[index]->JobID , pjname );
	Sch_Multi_ProcessJob[index]->StartMode = 0;
	Sch_Multi_ProcessJob[index]->MtlFormat = 13;
	Sch_Multi_ProcessJob[index]->ResultStatus = 1;
	Sch_Multi_ProcessJob[index]->ControlStatus = PRJOB_STS_QUEUED;
	strcpy( Sch_Multi_ProcessJob[index]->MtlRecipeName , "" );
	//
	for ( i = 0 ; i < MAX_MULTI_PRJOB_CASSETTE ; i++ ) {
		Sch_Multi_ProcessJob[index]->MtlUse[i] = FALSE;
		strcpy( Sch_Multi_ProcessJob[index]->MtlCarrName[i] , "" );
		for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
			Sch_Multi_ProcessJob[index]->MtlSlot[i][j] = 0;
		}
	}
	for ( i = 0 ; i < MAX_MULTI_CASS_SLOT_SIZE ; i++ ) {
		strcpy( Sch_Multi_ProcessJob[index]->MtlMIDName[i] , "" );
	}
	Sch_Multi_ProcessJob[index]->MtlRecipeMode = 0;
	for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE ; i++ ) {
		Sch_Multi_ProcessJob[index]->MtlRcpTuneModule[i] = 0;
		Sch_Multi_ProcessJob[index]->MtlRcpTuneStep[i] = 0;
		strcpy( Sch_Multi_ProcessJob[index]->MtlRcpTuneName[i] , "" );
		strcpy( Sch_Multi_ProcessJob[index]->MtlRcpTuneData[i] , "" );
	}
	Sch_Multi_ProcessJob[index]->DataID = 0;
	for ( i = 0 ; i < MAX_MULTI_PRJOB_EVENTINFO ; i++ ) {
		Sch_Multi_ProcessJob[index]->EventInfo[i] = 0;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2008.06.20
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; i++ ) {
			Sch_Multi_ProcessJob_Extend_RcpTuneArea[index]->MtlRcpTuneModule[i] = 0;
			strcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[index]->MtlRcpTuneName[i] , "" );
			strcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea[index]->MtlRcpTuneData[i] , "" );
		}
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memset( Sch_Multi_ProcessJob_Ext[index] , 0 , sizeof(Scheduling_Multi_PrJob_Ext) );
	}
	//
	(*Sch_Multi_ProcessJob_Size)++;
	//----------------------------------------------------------------------
	SCHMULTI_PRJOB_IO_SET();
	//----------------------------------------------------------------------
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_INSERT , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[index]->JobID , -1 , -1 );
	//----------------------------------------------------------------------
	// Setting Up // 2004.04.27
	//----------------------------------------------------------------------
	/*
	// 2011.09.20
	if ( Sch_Multi_ProcessJob[index]->ControlStatus <= PRJOB_STS_QUEUED ) {
		Sch_Multi_ProcessJob[index]->ControlStatus = PRJOB_STS_SETTINGUP;
	}
	else if ( Sch_Multi_ProcessJob[index]->ControlStatus == PRJOB_STS_PROCESSCOMPLETED ) {
		Sch_Multi_ProcessJob[index]->ControlStatus = PRJOB_STS_SETTINGUP;
	}
	*/
	//
	strcpy( Sch_Multi_ProcessJob[index]->MtlRecipeName , recipename );
	Sch_Multi_ProcessJob[index]->MtlUse[0] = TRUE;
	strcpy( Sch_Multi_ProcessJob[index]->MtlCarrName[0] , midname );
//	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
	for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) { // 2018.05.10
		Sch_Multi_ProcessJob[index]->MtlSlot[0][j] = 0;
	}
	// 2011.09.20
	if ( Sch_Multi_ProcessJob[index]->ControlStatus <= PRJOB_STS_QUEUED ) {
		Sch_Multi_ProcessJob[index]->ControlStatus = PRJOB_STS_SETTINGUP;
	}
	else if ( Sch_Multi_ProcessJob[index]->ControlStatus == PRJOB_STS_PROCESSCOMPLETED ) {
		Sch_Multi_ProcessJob[index]->ControlStatus = PRJOB_STS_SETTINGUP;
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return index;
}
//---------------------------------------------------------------------------------------
void SCHMULTI_PROCESSJOB_MANUAL_SET_MTLSLOT( int pjindex , int index , int data ) { // 2004.04.27
	Sch_Multi_ProcessJob[pjindex]->MtlSlot[0][index] = data;
}
//---------------------------------------------------------------------------------------
void SCHMULTI_PROCESSJOB_MAKE_BUFFER_DIRECT( int Cass , BOOL wait , char *ManualPJName , char *recipename , int startslot , int endslot , char *CarrName ) { // 2008.06.11
	int i , j;
	Sch_Multi_ProcessJob_Buffer->StartMode = wait ? 1 : 0;
	//
	if ( Cass < 0 ) {
		Sch_Multi_ProcessJob_Buffer->MtlFormat = 13;
	}
	else {
		Sch_Multi_ProcessJob_Buffer->MtlFormat = 100 + Cass;
	}
	//
	Sch_Multi_ProcessJob_Buffer->ResultStatus = 1;
	Sch_Multi_ProcessJob_Buffer->ControlStatus = 1;
	strcpy( Sch_Multi_ProcessJob_Buffer->MtlRecipeName , recipename );
	strcpy( Sch_Multi_ProcessJob_Buffer->JobID , ManualPJName );
	//
	Sch_Multi_ProcessJob_Buffer->MtlUse[0] = TRUE;
	strcpy( Sch_Multi_ProcessJob_Buffer->MtlCarrName[0] , CarrName );
	for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
		if ( ( endslot - startslot ) >= j ) {
			Sch_Multi_ProcessJob_Buffer->MtlSlot[0][j] = startslot + j;
		}
		else {
			Sch_Multi_ProcessJob_Buffer->MtlSlot[0][j] = 0;
		}
	}
	//
	for ( i = 1 ; i < MAX_MULTI_PRJOB_CASSETTE ; i++ ) {
		Sch_Multi_ProcessJob_Buffer->MtlUse[i] = FALSE;
		strcpy( Sch_Multi_ProcessJob_Buffer->MtlCarrName[i] , "" );
		for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
			Sch_Multi_ProcessJob_Buffer->MtlSlot[i][j] = 0;
		}
	}
	//----------
	Sch_Multi_ProcessJob_Buffer->DataID = 0;
	for ( i = 0 ; i < MAX_MULTI_PRJOB_EVENTINFO ; i++ ) {
		Sch_Multi_ProcessJob_Buffer->EventInfo[i] = 0;
	}
	//----------
	for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE ; i++ ) {
//		Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep[i] = 0; // 2013.05.10
		Sch_Multi_ProcessJob_Buffer->MtlRcpTuneModule[i] = 0; // 2013.05.10
		strcpy( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName[i] , "" );
		strcpy( Sch_Multi_ProcessJob_Buffer->MtlRcpTuneData[i] , "" );
	}
	//----------
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) {
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; i++ ) {
			Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneModule[i] = 0;
			strcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneName[i] , "" );
			strcpy( Sch_Multi_ProcessJob_Extend_RcpTuneArea_Buffer->MtlRcpTuneData[i] , "" );
		}
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memset( Sch_Multi_ProcessJob_Ext_Buffer , 0 , sizeof(Scheduling_Multi_PrJob_Ext) );
	}
	//
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( int CassIndex , int Mode ) { // 2019.04.12
	//----------
	if ( ( Mode == 0 ) || ( Mode == 1 ) ) {
		switch( BUTTON_GET_FLOW_STATUS_VAR( CassIndex ) ) {
		case _MS_0_IDLE :
		case _MS_2_LOADED :
		case _MS_3_LOADFAIL :
		case _MS_15_COMPLETE :
		case _MS_16_CANCEL :
		case _MS_17_ABORTED :
		case _MS_20_UNLOADED_C :
		case _MS_21_UNLOADED :
		case _MS_22_UNLOADFAIL :
			BUTTON_SET_FLOW_STATUS_VAR( CassIndex , _MS_5_MAPPED );
			break;
		default :
			return 1;
		}
		//----------
		SCHMULTI_CASSETTE_VERIFY_SET( CassIndex , CASSETTE_VERIFY_CONFIRMED2 );
		//----------
	}
	//----------
	if ( Mode == 0 ) Sleep(250); // 2019.04.12
	//----------
	if ( ( Mode == 0 ) || ( Mode == 2 ) ) {
		if ( SCHMULTI_CASSETTE_VERIFY_GET( CassIndex ) != CASSETTE_VERIFY_CONFIRMED2 ) {
			SCHMULTI_CASSETTE_VERIFY_SET( CassIndex , CASSETTE_VERIFY_CONFIRMED2 );
		}
	}
	//----------
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_PROCESSJOB_DIRECT_START_INSERT( int Cass , char *recipename , int cmout , int startslot , int endslot , char *CarrName , BOOL mappingskip , int StartStyle , int movemode ) { // 2008.04.23
	int i , j;
	//
	char ManualCJName[256];
	char ManualPJName[256];
	// StartStyle
		// P = 100
		// S = 200
		// O = 300
		//
		// E = 10
		// N = 1000

	if ( SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) return SCH_ERROR_NOT_GOOD_DATA;
	if ( SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) return SCH_ERROR_NOT_GOOD_DATA;
	//
	SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_DATA( Cass , ManualCJName , ManualPJName , &i );
	SCHMULTI_PROCESSJOB_MAKE_BUFFER_DIRECT( Cass , FALSE , ManualPJName , recipename , startslot , endslot , CarrName ); // 2008.06.11
	//----------
//	i = SCHMULTI_PROCESSJOB_INSERT( Cass ); // 2016.09.01
	i = SCHMULTI_PROCESSJOB_INSERT( Cass , TRUE ); // 2016.09.01
	//----------
	if ( i != 0 ) return i + 1000;
	//----------
	Sch_Multi_ControlJob_Buffer->StartMode = 0;
	Sch_Multi_ControlJob_Buffer->ResultStatus = 0;
	Sch_Multi_ControlJob_Buffer->ControlStatus = 1;
	strcpy( Sch_Multi_ControlJob_Buffer->JobID , ManualCJName );
	Sch_Multi_ControlJob_Buffer->PRJobDelete = 1;
	//
	Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[0] = TRUE;
	strcpy( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[0] , ManualPJName );
	//
	if ( ( Cass == cmout ) || ( cmout < 0 ) ) {
		Sch_Multi_ControlJob_Buffer->CassOutUse[0] = 0;
	}
	else {
		Sch_Multi_ControlJob_Buffer->CassOutUse[0] = ( 1000 * ( cmout + 1 ) ) + 1;
		//
		strcpy( Sch_Multi_ControlJob_Buffer->CassSrcID[0] , CarrName );
		strcpy( Sch_Multi_ControlJob_Buffer->CassOutID[0] , "" );
		//
		for ( i = 0 ; i < MAX_MULTI_CASS_SLOT_SIZE ; i++ ) {
			Sch_Multi_ControlJob_Buffer->CassSlotIn[0][i]  = startslot + i;
			Sch_Multi_ControlJob_Buffer->CassSlotOut[0][i] = startslot + i;
			if ( ( i + startslot ) == endslot ) {
				i++; // 2018.08.24 // STARTJ DIFF CASSETTE
				break;
			}
		}
		//
		for ( ; i < MAX_MULTI_CASS_SLOT_SIZE ; i++ ) {
			Sch_Multi_ControlJob_Buffer->CassSlotIn[0][i]  = 0;
			Sch_Multi_ControlJob_Buffer->CassSlotOut[0][i] = 0;
		}
		//
	}
	//
	for ( i = 1 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] = FALSE;
		strcpy( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] , "" );
	}
	//----------
//	i = SCHMULTI_CONTROLJOB_INSERT(); // 2008.09.12
	i = SCHMULTI_CONTROLJOB_INSERT( ( ( StartStyle % 100 ) / 10 ) , movemode ); // 2008.09.12
	//----------
	if ( i != 0 ) {
		j = SCHMULTI_PROCESSJOB_Find( ManualPJName );
		if ( j != -1 ) SCHMULTI_PROCESSJOB_DELETE( 3 , j );
		return i + 2000;
	}
	//----------
	if ( mappingskip ) {
		SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( Cass , 1 ); // 2019.04.12
	}
	else {
		SCHMULTI_CASSETTE_VERIFY_SET( Cass , CASSETTE_VERIFY_CONFIRMED2 ); // 2008.07.02
	}
	//
	//
	if ( ( Cass != cmout ) && ( cmout >= 0 ) ) { // 2009.04.07
		//----------
		if ( mappingskip ) {
			SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( cmout , 1 ); // 2019.04.12
		}
		else {
			SCHMULTI_CASSETTE_VERIFY_SET( cmout , CASSETTE_VERIFY_CONFIRMED2 );
		}
		//----------
	}
	//
	// 2011.04.14
	//----------
	Sleep(250);
	//----------
	SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( Cass , 2 );
	//
	if ( ( Cass != cmout ) && ( cmout >= 0 ) ) {
		SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( cmout , 2 );
	}
	//----------
	return 0;
}




//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
// 2012.04.23
//
int  SCHMULTI_PROCESSJOB_GUI_INSERT( char *pjname , BOOL wait , char *recipename , int startslot , int endslot , char *CarrName ) {
	//
	if ( SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) return SCH_ERROR_NOT_GOOD_DATA;
	//
	SCHMULTI_PROCESSJOB_MAKE_BUFFER_DIRECT( -1 , wait , pjname , recipename , startslot , endslot , CarrName );
	//----------
//	return SCHMULTI_PROCESSJOB_INSERT( -1 ); // 2016.09.01
	return SCHMULTI_PROCESSJOB_INSERT( -1 , FALSE ); // 2016.09.01
	//----------
}
//
int  SCHMULTI_CONTROLJOB_GUI_INSERT( char *cjname , BOOL filemode , BOOL wait ) {
	int i , j;
	//
	if ( SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) return SCH_ERROR_NOT_GOOD_DATA;
	//
	Sch_Multi_ControlJob_Buffer->StartMode = wait ? 1 : 0;
	Sch_Multi_ControlJob_Buffer->ResultStatus = 0;
	Sch_Multi_ControlJob_Buffer->ControlStatus = 1;
	strcpy( Sch_Multi_ControlJob_Buffer->JobID , cjname );
	Sch_Multi_ControlJob_Buffer->PRJobDelete = 1;
	//
	j = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		//
		if ( filemode ) { // 2014.02.03
			if ( STR_MEM_SIZE( GUI_INSERT_CONREOLJOB_PRJOB_NAME2[i] ) <= 0 ) continue;
			//
			Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] = TRUE;
			strcpy( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] , STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME2[i] ) );
		}
		else {
			if ( STR_MEM_SIZE( GUI_INSERT_CONREOLJOB_PRJOB_NAME[i] ) <= 0 ) continue;
			//
			Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] = TRUE;
			strcpy( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] , STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[i] ) );
		}
		//
		Sch_Multi_ControlJob_Buffer->CassOutUse[j] = 0;
		//
		j++;
		//
	}
	for ( ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
		Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] = FALSE;
		strcpy( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] , "" );
	}
	//----------
	return SCHMULTI_CONTROLJOB_INSERT( FALSE , 0 );
	//----------
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_CTJOB_IO_SET() {
	IO_ADD_WRITE_DIGITAL( Address_IO_CTJob_Remain , MAX_MULTI_QUEUE_SIZE - *Sch_Multi_ControlJob_Size );
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CONTROLJOB_SET_STATUS( int mode , char *name , int data ) {
	int i;
	if ( mode >= 0 ) {
		Sch_Multi_ControlJob[mode]->ControlStatus = data;
	}
	else if ( mode == -99 ) {
		for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
			Sch_Multi_ControlJob[i]->ControlStatus = data;
		}
	}
	else {
		for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
			if ( STRCMP_L( name , Sch_Multi_ControlJob[i]->JobID ) ) break;
		}
		if ( i == *Sch_Multi_ControlJob_Size ) return;
		Sch_Multi_ControlJob[i]->ControlStatus = data;
	}
}
//---------------------------------------------------------------------------------------
int SCHMULTI_CONTROLJOB_Buffer_Data_Check() {
	int i , j , k , l , y = 0 , maxslot , maxunuse;
	if ( Sch_Multi_ControlJob_Buffer->StartMode < 0 ) return -11;
	if ( Sch_Multi_ControlJob_Buffer->StartMode > 1 ) return -12;
	//
	maxslot = Scheduler_Get_Max_Slot( -1 , -1 , -1 , 32 , &maxunuse ); // 2010.12.17
	//
/* 
	// 2011.05.21
	// 2011.02.15(Testing)
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
//		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) return TRUE; // 2009.04.03
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) { // 2009.04.03
			//
			y++;
			//-------------------------------------------------------------
			// 2009.04.03
			//-------------------------------------------------------------
			if ( Sch_Multi_ControlJob_Buffer->CassOutUse[i] >= 1 ) {
				//
				if ( Sch_Multi_ControlJob_Buffer->CassOutUse[i] < 1000 ) {
					//
					if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , "" ) ) return -13;
					if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CassOutID[i] , "" ) ) return -14;
					//
					for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] <= 0 ) {
							Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] = 0;
							Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] = 0;
						}
						else {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] > maxslot ) return -15; // 2010.12.17
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] <= 0 ) {
								Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] = 0;
								Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] = 0;
							}
							else {
								//
								if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] > maxslot ) return -16;
								//
							}
						}
					}
					//
					for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] <= 0 ) continue;
						//
						for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] == Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] ) return -17;
							//
						}
					}
					//
					for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] <= 0 ) continue;
						//
						for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] == Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] ) return -18;
							//
						}
					}
					//
					if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , Sch_Multi_ControlJob_Buffer->CassOutID[i] ) ) {
						//
						for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] <= 0 ) continue;
							//
							for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
								//
								if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] == Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] ) return -19;
								//
							}
						}
						//
					}
					//
					for ( j = i + 1 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
						if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] ) {
							if ( !STRCMP_L( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , Sch_Multi_ControlJob_Buffer->CassSrcID[j] ) ) continue;
							if ( !STRCMP_L( Sch_Multi_ControlJob_Buffer->CassOutID[i] , Sch_Multi_ControlJob_Buffer->CassOutID[j] ) ) continue;
							//
							for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
								//
								if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] <= 0 ) continue;
								//
								for ( l = 0 ; l < MAX_MULTI_CASS_SLOT_SIZE ; l++ ) {
									//
									if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[j][l] <= 0 ) continue;
									//
									if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] != Sch_Multi_ControlJob_Buffer->CassSlotIn[j][l] ) continue;
									//
									if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] != Sch_Multi_ControlJob_Buffer->CassSlotOut[j][l] ) return -20;
								}
							}

						}
					}
					//
				}
			}
			//-------------------------------------------------------------
		}
	}
*/


	// 2011.05.21
	//
	// 2011.02.15(Testing)
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) y++;
	}
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		//
		if ( Sch_Multi_ControlJob_Buffer->CassOutUse[i] >= 1 ) {
			//
			if ( Sch_Multi_ControlJob_Buffer->CassOutUse[i] < 1000 ) {
				//
				if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , "" ) ) return -13;
				if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CassOutID[i] , "" ) ) return -14;
				//
				for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
					//
					if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] <= 0 ) {
						Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] = 0;
						Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] = 0;
					}
					else {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] > maxslot ) return -15; // 2010.12.17
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] <= 0 ) {
							Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] = 0;
							Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] = 0;
						}
						else {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] > maxslot ) return -16;
							//
						}
					}
				}
				//
				for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
					//
					if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] <= 0 ) continue;
					//
					for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] == Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] ) return -17;
						//
					}
				}
				//
				for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
					//
					if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] <= 0 ) continue;
					//
					for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][j] == Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] ) return -18;
						//
					}
				}
				//
				if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , Sch_Multi_ControlJob_Buffer->CassOutID[i] ) ) {
					//
					for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
						//
						if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] <= 0 ) continue;
						//
						for ( k = j + 1 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][j] == Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] ) return -19;
							//
						}
					}
					//
				}
				//
				for ( j = i + 1 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
					if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] ) {
						if ( !STRCMP_L( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , Sch_Multi_ControlJob_Buffer->CassSrcID[j] ) ) continue;
						if ( !STRCMP_L( Sch_Multi_ControlJob_Buffer->CassOutID[i] , Sch_Multi_ControlJob_Buffer->CassOutID[j] ) ) continue;
						//
						for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							//
							if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] <= 0 ) continue;
							//
							for ( l = 0 ; l < MAX_MULTI_CASS_SLOT_SIZE ; l++ ) {
								//
								if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[j][l] <= 0 ) continue;
								//
								if ( Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] != Sch_Multi_ControlJob_Buffer->CassSlotIn[j][l] ) continue;
								//
								if ( Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] != Sch_Multi_ControlJob_Buffer->CassSlotOut[j][l] ) return -20;
							}
						}

					}
				}
				//
			}
		}
		//
	}
	//
	//
	if ( y > 0 ) return 1; // 2009.04.03
	//
	return -1;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_INSERT( BOOL HeadInsert , int MoveMode ) {
	int i , j;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 1; // 2015.03.27
	//
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( *Sch_Multi_ControlJob_Size >= MAX_MULTI_QUEUE_SIZE ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_QUEUE_FULL , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_FULL;
	}
	if ( strlen( Sch_Multi_ControlJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_SELECTED ) {
			if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[i]->JobID ) ) {
				//
				Sch_Multi_CJJOB_Buf_Sub_Error = 11; // 2015.03.27
				//
//				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_QUEUE_ALREADY_REGIST , -1 , -1 );
				SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				return SCH_ERROR_QUEUE_ALREADY_REGIST;
			}
		}
	}
	for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob[i]->JobID ) ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 21; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_QUEUE_ALREADY_REGIST , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_ALREADY_REGIST;
		}
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) {
			for ( j = i + 1 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] ) {
					if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] , Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] ) ) {
//						LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
						SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_DUPLICATE_PRJOB , -1 , -1 );
						SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
						LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
						return SCH_ERROR_DUPLICATE_PRJOB;
					}
				}
			}
		}
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) {
			if ( SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] ) < 0 ) {
//				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_CANNOT_FIND_PRJOB , -1 , -1 );
				SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				return SCH_ERROR_CANNOT_FIND_PRJOB;
			}
		}
	}
	i = SCHMULTI_CONTROLJOB_Buffer_Data_Check();
	if ( i < 0 ) {
		//
		Sch_Multi_CJJOB_Buf_Sub_Error = ( i * -100 ) + 10 + 1; // 2015.03.27
		//
//		_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_INSERT Error for %s , E=%d\n" , Sch_Multi_ControlJob_Buffer->JobID , i ); // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_INSERT , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) {
			SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] , 1 , FALSE , FALSE );
		}
	}
	//
	if ( HeadInsert ) {
		i = *Sch_Multi_ControlJob_Size;
		for ( ; i > 0 ; i-- ) {
			//
			memcpy( Sch_Multi_ControlJob[i] , Sch_Multi_ControlJob[i-1] , sizeof(Scheduling_Multi_ControlJob) );
			//
			memcpy( &(Sch_Multi_Ins_ControlJob[i]) , &(Sch_Multi_Ins_ControlJob[i-1]) , sizeof(Scheduling_Multi_Ins_ControlJob) );
			//
			if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
				memcpy( Sch_Multi_ControlJob_Ext[i] , Sch_Multi_ControlJob_Ext[i-1] , sizeof(Scheduling_Multi_CrJob_Ext) );
			}
			//
		}
		//
		memcpy( Sch_Multi_ControlJob[0] , Sch_Multi_ControlJob_Buffer , sizeof(Scheduling_Multi_ControlJob) );
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ControlJob_Ext[0] , Sch_Multi_ControlJob_Ext_Buffer , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
		time( &(Sch_Multi_Ins_ControlJob[0].InsertTime) ); // 2012.04.19
		//
		Sch_Multi_Ins_ControlJob[0].FixedResult = PRJOB_RESULT_NORMAL; // 2018.11.14
		//
		Sch_Multi_Ins_ControlJob[0].Queued_to_Select_Wait = FALSE; // 2012.09.26
		//
		Sch_Multi_Ins_ControlJob[0].MoveMode = MoveMode; // 2013.09.03
		//
		Sch_Multi_ControlJob[0]->ControlStatus = CTLJOB_STS_QUEUED;
		//
	}
	else {
		memcpy( Sch_Multi_ControlJob[*Sch_Multi_ControlJob_Size] , Sch_Multi_ControlJob_Buffer , sizeof(Scheduling_Multi_ControlJob) );
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ControlJob_Ext[*Sch_Multi_ControlJob_Size] , Sch_Multi_ControlJob_Ext_Buffer , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
		time( &(Sch_Multi_Ins_ControlJob[*Sch_Multi_ControlJob_Size].InsertTime) ); // 2012.04.19
		//
		Sch_Multi_Ins_ControlJob[*Sch_Multi_ControlJob_Size].FixedResult = PRJOB_RESULT_NORMAL; // 2018.11.14
		//
		Sch_Multi_Ins_ControlJob[*Sch_Multi_ControlJob_Size].Queued_to_Select_Wait = FALSE; // 2012.09.26
		//
		Sch_Multi_Ins_ControlJob[*Sch_Multi_ControlJob_Size].MoveMode = MoveMode; // 2013.09.03
		//
		Sch_Multi_ControlJob[*Sch_Multi_ControlJob_Size]->ControlStatus = CTLJOB_STS_QUEUED;
		//
	}
	(*Sch_Multi_ControlJob_Size)++;
	//----------------------------------------------------------------------
	SCHMULTI_CTJOB_IO_SET();
	//----------------------------------------------------------------------
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_INSERT , *Sch_Multi_ControlJob_Size , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 );
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_CHANGE() {
	int i , j , sel , c;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 1; // 2015.03.27
	//
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( *Sch_Multi_ControlJob_Size <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_EMPTY;
	}
	if ( strlen( Sch_Multi_ControlJob_Buffer->JobID ) <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	for ( sel = 0 ; sel < *Sch_Multi_ControlJob_Size ; sel++ ) {
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob[sel]->JobID ) ) break;
	}
	if ( sel == *Sch_Multi_ControlJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) {
			for ( j = i + 1 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] ) {
					if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] , Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] ) ) {
//						LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
						SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_DUPLICATE_PRJOB , -1 , -1 );
						SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
						LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
						return SCH_ERROR_DUPLICATE_PRJOB;
					}
				}
			}
		}
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) {
			if ( SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] ) < 0 ) {
//				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_CANNOT_FIND_PRJOB , -1 , -1 );
				SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				return SCH_ERROR_CANNOT_FIND_PRJOB;
			}
		}
	}
	i = SCHMULTI_CONTROLJOB_Buffer_Data_Check();
	if ( i < 0 ) {
		//
		Sch_Multi_CJJOB_Buf_Sub_Error = ( i * -100 ) + 10 + 1; // 2015.03.27
		//
//		_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_CHANGE Error for %s , E=%d\n" , Sch_Multi_ControlJob_Buffer->JobID , i ); // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHANGE , SCH_ERROR_ERROR_DATA , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ERROR_DATA;
	}
	c = Sch_Multi_ControlJob[sel]->ControlStatus;
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob[sel]->CtrlSpec_Use[i] ) {
			SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob[sel]->CtrlSpec_ProcessJobID[i] , -1 , FALSE , FALSE );
		}
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] ) {
			SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] , 1 , FALSE , FALSE );
		}
	}
	//
	memcpy( Sch_Multi_ControlJob[sel] , Sch_Multi_ControlJob_Buffer , sizeof(Scheduling_Multi_ControlJob) );
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ControlJob_Ext[sel] , Sch_Multi_ControlJob_Ext_Buffer , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
	time( &(Sch_Multi_Ins_ControlJob[sel].InsertTime) ); // 2012.04.19
	//
	Sch_Multi_Ins_ControlJob[sel].FixedResult = PRJOB_RESULT_NORMAL; // 2018.11.14
	//
	Sch_Multi_ControlJob[sel]->ControlStatus = c;
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_CHANGE , sel , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 );
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_DELETE( int index ) {
	int i , sel;
	char DelJobID[256];
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 2; // 2015.03.27
	//
	if ( index < 0 ) {
		//
		if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_DELETE , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_NOT_GOOD_DATA;
		}
		if ( *Sch_Multi_ControlJob_Size <= 0 ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_DELETE , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_EMPTY;
		}
		if ( strlen( Sch_Multi_ControlJob_Buffer->JobID ) <= 0 ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_DELETE , SCH_ERROR_ERROR_DATA , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_ERROR_DATA;
		}
		for ( sel = 0 ; sel < *Sch_Multi_ControlJob_Size ; sel++ ) {
			if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob[sel]->JobID ) ) break;
		}
	}
	else { // 2012.04.19
		sel = index;
	}
	//
	if ( sel >= *Sch_Multi_ControlJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_DELETE , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		//
		if ( index < 0 ) SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		//
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	strncpy( DelJobID , Sch_Multi_ControlJob[sel]->JobID , 255 );
	//---------------------------
	SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_CANCEL , 0 , DelJobID , -1 , -1 ); // 2002.05.20
	//---------------------------
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob[sel]->CtrlSpec_Use[i] ) {
			//SCHMULTI_PROCESSJOB_Reference( TRUE , Sch_Multi_ControlJob[sel]->CtrlSpec_ProcessJobID[i] , -1 , FALSE , FALSE ); // 2002.05.20
			SCHMULTI_PROCESSJOB_Reference( TRUE , Sch_Multi_ControlJob[sel]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , TRUE ); // 2002.05.20
		}
	}
	for ( ; sel < *Sch_Multi_ControlJob_Size - 1; sel++ ) {
		//
		memcpy( Sch_Multi_ControlJob[sel] , Sch_Multi_ControlJob[sel+1] , sizeof(Scheduling_Multi_ControlJob) );
		//
		memcpy( &(Sch_Multi_Ins_ControlJob[sel]) , &(Sch_Multi_Ins_ControlJob[sel+1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ControlJob_Ext[sel] , Sch_Multi_ControlJob_Ext[sel+1] , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
	}
	SCHMULTI_CONTROLJOB_SET_STATUS( sel , "" , CTLJOB_STS_NOTSTATE );
	(*Sch_Multi_ControlJob_Size)--;
	//----------------------------------------------------------------------
	SCHMULTI_CTJOB_IO_SET();
	//----------------------------------------------------------------------
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_DELETE , *Sch_Multi_ControlJob_Size , DelJobID , -1 , -1 );
	//
	if ( index < 0 ) SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}

//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_DELETE_CM( int Cass ) { // 2011.04.12
	int i , j , k;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 2; // 2015.03.27
	//
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		//
		if ( ( Cass + 100 ) != Sch_Multi_ProcessJob[i]->MtlFormat ) continue;
		//
		for ( j = 0 ; j < *Sch_Multi_ControlJob_Size ; j++ ) {
			//
			for ( k = 0 ; k < MAX_MULTI_CTJOB_CASSETTE ; k++ ) {
				//
				if ( !Sch_Multi_ControlJob[j]->CtrlSpec_Use[k] ) continue;
				//
				if ( !STRCMP_L( Sch_Multi_ControlJob[j]->CtrlSpec_ProcessJobID[k] , Sch_Multi_ProcessJob[i]->JobID ) ) continue;
				//
				for ( k = 0 ; k < MAX_MULTI_CTJOB_CASSETTE ; k++ ) {
					if ( Sch_Multi_ControlJob[j]->CtrlSpec_Use[k] ) {
						SCHMULTI_PROCESSJOB_Reference( TRUE , Sch_Multi_ControlJob[j]->CtrlSpec_ProcessJobID[k] , -1 , TRUE , TRUE );
					}
				}
				//
				for ( ; j < *Sch_Multi_ControlJob_Size - 1; j++ ) {
					//
					memcpy( Sch_Multi_ControlJob[j] , Sch_Multi_ControlJob[j+1] , sizeof(Scheduling_Multi_ControlJob) );
					//
					memcpy( &(Sch_Multi_Ins_ControlJob[j]) , &(Sch_Multi_Ins_ControlJob[j+1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
					//
					if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
						memcpy( Sch_Multi_ControlJob_Ext[j] , Sch_Multi_ControlJob_Ext[j+1] , sizeof(Scheduling_Multi_CrJob_Ext) );
					}
					//
				}
				//
				SCHMULTI_CONTROLJOB_SET_STATUS( j , "" , CTLJOB_STS_NOTSTATE );
				(*Sch_Multi_ControlJob_Size)--;
				//----------------------------------------------------------------------
				SCHMULTI_CTJOB_IO_SET();
				//----------------------------------------------------------------------
				LeaveCriticalSection( &CR_MULTIJOB );
				return 0;
				//
			}
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCH_ERROR_ERROR_DATA;
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	return SCH_ERROR_QUEUE_CANNOT_FIND;
	//
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_PAUSE( char *name ) {
	int i , Res , ff;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 5; // 2015.03.27
	//
	//---------------------------------------------------------------------------------
	// 2004.06.21
	//---------------------------------------------------------------------------------
	if ( strlen( name ) > 0 ) {
		SCHMULTI_CONTROLJOB_BUFFER_MAKE_POSSIBLE();
		strncpy( Sch_Multi_ControlJob_Buffer->JobID , name , 128 );
	}
	//---------------------------------------------------------------------------------
	ff = -1;
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_PAUSE , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
//	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) { // 2003.07.23(bug)
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) { // 2003.07.23(bug)
		//
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus == CTLJOB_STS_NOTSTATE ) continue; // 2013.10.22
		//
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[i]->JobID ) ) {
			ff = i;
			break;
		}
	}
	if ( ff == -1 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_PAUSE , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_CANNOT_FIND_CTJOB;
	}
	switch( Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus ) {
	case CTLJOB_STS_NOTSTATE :
	case CTLJOB_STS_QUEUED :
	case CTLJOB_STS_COMPLETED :
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_PAUSE , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_NOT_SELECTED;
		break;
	case CTLJOB_STS_PAUSED :
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_PAUSE , SCH_ERROR_ALREADY_DOING , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ALREADY_DOING;
		break;
	case CTLJOB_STS_SELECTED :
//		Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_PAUSED; // 2004.06.16
//		Sch_Multi_ControlJob_Selected_Pause_Signal[ff] = 0; // 2004.06.16
		//
		Sch_Multi_CJJOB_Buf_Sub_Error = 15; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_PAUSE , SCH_ERROR_JOB_SELECT_ALREADY , -1 , -1 ); // 2004.06.16
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR(); // 2004.06.16
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_SELECT_ALREADY; // 2004.06.16
		break;
	case CTLJOB_STS_WAITINGFORSTART :
//		Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_PAUSED; // 2004.06.16
//		Sch_Multi_ControlJob_Selected_Pause_Signal[ff] = 1; // 2004.06.16
		//
		Sch_Multi_CJJOB_Buf_Sub_Error = 25; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_PAUSE , SCH_ERROR_JOB_SELECT_ALREADY , -1 , -1 ); // 2004.06.16
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR(); // 2004.06.16
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_SELECT_ALREADY; // 2004.06.16
		break;
	case CTLJOB_STS_EXECUTING :
		Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_PAUSED;
		Sch_Multi_ControlJob_Selected_Pause_Signal[ff] = 2;
		break;
	}
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_PAUSE , 0 , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 ); // 2004.06.02
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_PAUSE( 1 , Sch_Multi_ControlJob_Select_Data[ff]->JobID );
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_PAUSE , 0 , Sch_Multi_ControlJob_Buffer->JobID ); // 2004.06.02
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_RESUME( char *name ) {
	int i , Res , ff;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 6; // 2015.03.27
	//
	//---------------------------------------------------------------------------------
	// 2004.06.21
	//---------------------------------------------------------------------------------
	if ( strlen( name ) > 0 ) {
		SCHMULTI_CONTROLJOB_BUFFER_MAKE_POSSIBLE();
		strncpy( Sch_Multi_ControlJob_Buffer->JobID , name , 128 );
	}
	//---------------------------------------------------------------------------------
	ff = -1;
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_RESUME , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
//	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) { // 2003.07.23(bug)
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) { // 2003.07.23(bug)
		//
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus == CTLJOB_STS_NOTSTATE ) continue; // 2013.10.22
		//
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[i]->JobID ) ) {
			ff = i;
			break;
		}
	}
	if ( ff == -1 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_RESUME , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_CANNOT_FIND_CTJOB;
	}
	switch( Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus ) {
	case CTLJOB_STS_NOTSTATE :
	case CTLJOB_STS_QUEUED :
	case CTLJOB_STS_COMPLETED :
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_RESUME , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_NOT_SELECTED;
		break;
	case CTLJOB_STS_SELECTED :
	case CTLJOB_STS_WAITINGFORSTART :
	case CTLJOB_STS_EXECUTING :
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_RESUME , SCH_ERROR_ALREADY_DOING , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_ALREADY_DOING;
		break;
	case CTLJOB_STS_PAUSED :
		switch ( Sch_Multi_ControlJob_Selected_Pause_Signal[ff] ) {
		case 0 :
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_SELECTED;
			break;
		case 1 :
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_WAITINGFORSTART;
			break;
		case 2 :
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_EXECUTING;
			break;
		}
		break;
	}
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_RESUME , 0 , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 ); // 2004.06.02
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_RESUME( 1 , Sch_Multi_ControlJob_Select_Data[ff]->JobID );
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_RESUME , 0 , Sch_Multi_ControlJob_Buffer->JobID ); // 2004.06.02
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_STOP( char *name ) {
	int i , Res , ff;
	char DelJobID[256];
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 7; // 2015.03.27
	//
	//---------------------------------------------------------------------------------
	// 2004.06.21
	//---------------------------------------------------------------------------------
	if ( strlen( name ) > 0 ) {
		SCHMULTI_CONTROLJOB_BUFFER_MAKE_POSSIBLE();
		strncpy( Sch_Multi_ControlJob_Buffer->JobID , name , 128 );
	}
	//---------------------------------------------------------------------------------
	ff = -1;
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_STOP , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
//	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) { // 2003.07.23(bug)
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) { // 2003.07.23(bug)
		//
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus == CTLJOB_STS_NOTSTATE ) continue; // 2013.10.22
		//
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[i]->JobID ) ) {
			ff = i;
			break;
		}
	}
	if ( ff == -1 ) {
		for ( Res = 0 ; Res < *Sch_Multi_ControlJob_Size ; Res++ ) {
			if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob[Res]->JobID ) ) break;
		}
		if ( Res == *Sch_Multi_ControlJob_Size ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_STOP , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_CANNOT_FIND;
		}
		strncpy( DelJobID , Sch_Multi_ControlJob[Res]->JobID , 255 );
		//==========================================================================================================
		// 2006.04.13
		//==========================================================================================================
		SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_CANCEL , 0 , DelJobID , -1 , -1 );
		//==========================================================================================================
		for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
			if ( Sch_Multi_ControlJob[Res]->CtrlSpec_Use[i] ) {
//				SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob[Res]->CtrlSpec_ProcessJobID[i] , -1 , FALSE , FALSE ); // 2002.05.20
//				SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob[Res]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , TRUE ); // 2002.05.20 // 2005.06.11
				SCHMULTI_PROCESSJOB_Reference( TRUE , Sch_Multi_ControlJob[Res]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , TRUE ); // 2002.05.20 // 2005.06.11
			}
		}
		for ( ; Res < *Sch_Multi_ControlJob_Size - 1; Res++ ) {
			//
			memcpy( Sch_Multi_ControlJob[Res] , Sch_Multi_ControlJob[Res+1] , sizeof(Scheduling_Multi_ControlJob) );
			//
			memcpy( &(Sch_Multi_Ins_ControlJob[Res]) , &(Sch_Multi_Ins_ControlJob[Res+1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
			//
			if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
				memcpy( Sch_Multi_ControlJob_Ext[Res] , Sch_Multi_ControlJob_Ext[Res+1] , sizeof(Scheduling_Multi_CrJob_Ext) );
			}
			//
		}
		SCHMULTI_CONTROLJOB_SET_STATUS( Res , "" , CTLJOB_STS_NOTSTATE );
		(*Sch_Multi_ControlJob_Size)--;
		//----------------------------------------------------------------------
		SCHMULTI_CTJOB_IO_SET();
		//----------------------------------------------------------------------
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
//		SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( 4 , *Sch_Multi_ControlJob_Size , DelJobID ); // 2006.05.04
		SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_DELETE , *Sch_Multi_ControlJob_Size , DelJobID , -1 , -1 ); // 2006.05.04
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return 0;
	}

	switch( Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus ) {
	case CTLJOB_STS_NOTSTATE :
	case CTLJOB_STS_QUEUED :
	case CTLJOB_STS_COMPLETED :
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_STOP , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_NOT_SELECTED;
		break;
	case CTLJOB_STS_SELECTED :
//	case CTLJOB_STS_WAITINGFORSTART : // 2006.04.19
		//
		Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = TRUE; // 2016.09.13
		//
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCHMULTI_CONTROLJOB_DELSELECT( 0 , ff , 1 , FALSE );
//		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_STOP , SCH_ERROR_SELECT_DATA_ERROR );
//		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
//		return SCH_ERROR_SELECT_DATA_ERROR;
		break;
	case CTLJOB_STS_WAITINGFORSTART : // 2006.04.19
//		SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_FINISH , 3 , Sch_Multi_ControlJob_Select_Data[ff]->JobID ); // 2006.04.19 // 2006.05.04
		//
		Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = TRUE; // 2016.09.13
		//
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCHMULTI_CONTROLJOB_DELSELECT( 0 , ff , 1 , TRUE );
		break;
	case CTLJOB_STS_PAUSED : // 2004.06.02
		switch ( Sch_Multi_ControlJob_Selected_Pause_Signal[ff] ) {
		case 0 :
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_SELECTED;
			break;
		case 1 :
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_WAITINGFORSTART;
			break;
		case 2 :
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_EXECUTING;
			break;
		}
		break;
	}
	Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = TRUE; // 2002.05.20
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_STOP , 0 , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 ); // 2004.06.02
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_STOP( 1 , Sch_Multi_ControlJob_Select_Data[ff]->JobID );
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_STOP , 0 , Sch_Multi_ControlJob_Buffer->JobID ); // 2004.06.02
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_ABORT( char *name ) {
	int i , Res , ff;
	char DelJobID[256];
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 8; // 2015.03.27
	//
	//---------------------------------------------------------------------------------
	// 2004.06.21
	//---------------------------------------------------------------------------------
	if ( strlen( name ) > 0 ) {
		SCHMULTI_CONTROLJOB_BUFFER_MAKE_POSSIBLE();
		strncpy( Sch_Multi_ControlJob_Buffer->JobID , name , 128 );
	}
	//---------------------------------------------------------------------------------
	ff = -1;
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_ABORT , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}

//	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) { // 2003.07.23(bug)
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) { // 2003.07.23(bug)
		//
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus == CTLJOB_STS_NOTSTATE ) continue; // 2013.10.22
		//
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[i]->JobID ) ) {
			ff = i;
			break;
		}
	}
	if ( ff == -1 ) {
		for ( Res = 0 ; Res < *Sch_Multi_ControlJob_Size ; Res++ ) {
			if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob[Res]->JobID ) ) break;
		}
		if ( Res == *Sch_Multi_ControlJob_Size ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_ABORT , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_QUEUE_CANNOT_FIND;
		}
		strncpy( DelJobID , Sch_Multi_ControlJob[Res]->JobID , 255 );
		//==========================================================================================================
		// 2006.04.13
		//==========================================================================================================
		SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_CANCEL , 0 , DelJobID , -1 , -1 );
		//==========================================================================================================
		for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
			if ( Sch_Multi_ControlJob[Res]->CtrlSpec_Use[i] ) {
//				SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob[Res]->CtrlSpec_ProcessJobID[i] , -1 , FALSE , FALSE ); // 2002.05.20
//				SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob[Res]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , TRUE ); // 2002.05.20 // 2005.06.11
				SCHMULTI_PROCESSJOB_Reference( TRUE , Sch_Multi_ControlJob[Res]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , TRUE ); // 2002.05.20 // 2005.06.11
			}
		}
		for ( ; Res < *Sch_Multi_ControlJob_Size - 1; Res++ ) {
			//
			memcpy( Sch_Multi_ControlJob[Res] , Sch_Multi_ControlJob[Res+1] , sizeof(Scheduling_Multi_ControlJob) );
			//
			memcpy( &(Sch_Multi_Ins_ControlJob[Res]) , &(Sch_Multi_Ins_ControlJob[Res+1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
			//
			if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
				memcpy( Sch_Multi_ControlJob_Ext[Res] , Sch_Multi_ControlJob_Ext[Res+1] , sizeof(Scheduling_Multi_CrJob_Ext) );
			}
			//
		}
		SCHMULTI_CONTROLJOB_SET_STATUS( Res , "" , CTLJOB_STS_NOTSTATE );
		(*Sch_Multi_ControlJob_Size)--;
		//----------------------------------------------------------------------
		SCHMULTI_CTJOB_IO_SET();
		//----------------------------------------------------------------------
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_DELETE , *Sch_Multi_ControlJob_Size , DelJobID , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return 0;
	}
	switch( Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus ) {
	case CTLJOB_STS_NOTSTATE :
	case CTLJOB_STS_QUEUED :
	case CTLJOB_STS_COMPLETED :
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_ABORT , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_NOT_SELECTED;
		break;
	case CTLJOB_STS_SELECTED :
//	case CTLJOB_STS_WAITINGFORSTART : // 2006.04.19
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCHMULTI_CONTROLJOB_DELSELECT( 0 , ff , 2 , FALSE );
//		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_ABORT , SCH_ERROR_SELECT_DATA_ERROR );
//		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
//		return SCH_ERROR_SELECT_DATA_ERROR;
		break;
	case CTLJOB_STS_WAITINGFORSTART : // 2006.04.19
//		SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_FINISH , 1 , Sch_Multi_ControlJob_Select_Data[ff]->JobID ); // 2006.04.19 // 2006.05.04
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCHMULTI_CONTROLJOB_DELSELECT( 0 , ff , 2 , TRUE );
		break;
	}
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_ABORT , 0 , Sch_Multi_ControlJob_Select_Data[ff]->JobID , -1 , -1 );
	Res = SCHMULTI_RUNJOB_PROCESSJOB_MAKE_ABORT( 1 , Sch_Multi_ControlJob_Select_Data[ff]->JobID ); // 2004.06.02
//	SCHMULTI_CONTROLJOB_BUFFER_CLEAR(); // 2004.06.15
	//---------------------------------------------------------------------------
	// Abort Operation
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( i );
	Scheduler_All_Abort_Part( TRUE ); // 2003.06.03
//	SYSTEM_ALL_ABORT(); // 2003.06.03
	//
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR(); // 2004.06.15
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_GET_SELECT_STATUS( int *m ) {
	int i;
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_SELECTED ) {
			*m = i;
			return TRUE;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_GET_SELECT_ONLY_STATUS( int *m ) { // 2004.06.01
	int i;
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus == CTLJOB_STS_SELECTED ) {
			*m = i;
			return TRUE;
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CONTROLJOB_SET_RUN_STATUS( int m ) {
	Sch_Multi_ControlJob_Select_Data[m]->ControlStatus = CTLJOB_STS_EXECUTING;
	Sch_Multi_ControlJob_Selected_Stop_Signal[m] = FALSE; // 2002.05.20
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CONTROLJOB_SET_WAIT_STATUS( int m ) {
	Sch_Multi_ControlJob_Select_Data[m]->ControlStatus = CTLJOB_STS_WAITINGFORSTART;
	Sch_Multi_ControlJob_Selected_Stop_Signal[m] = FALSE; // 2002.05.20
}
//---------------------------------------------------------------------------------------
// 2011.05.21
/* int Sch_Get_Multi_OutData_from_ControlJob( int mm , int i , char *MtlCarrName , int slot ) {
	int j;
	//
	if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] == 0 ) return -1;
	//
	if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] < 1000 ) {
		if ( !STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , MtlCarrName ) ) return -1;
		if ( STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] , "" ) ) return -1;
		if ( STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] ) ) return -1;
	}
	//
	if ( slot == -1 ) {
		if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] >= 1000 ) return Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i];
		//
		return i;
	}
	//
	if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] < 1000 ) {
		for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
			if ( Sch_Multi_ControlJob_Select_Data[mm]->CassSlotIn[i][j] == slot ) {
				if ( Sch_Multi_ControlJob_Select_Data[mm]->CassSlotOut[i][j] > 0 ) {
					return Sch_Multi_ControlJob_Select_Data[mm]->CassSlotOut[i][j];
				}
			}
		}
	}
	return -1;
}
*/

// 2011.05.21
//int Sch_Get_Multi_OutData_from_ControlJob( int mm , char *MtlCarrName , int slot , char *CassOutID ) { // 2018.08.23 // STARTJ DIFF CASSETTE
int Sch_Get_Multi_OutData_from_ControlJob( int mm , char *MtlCarrName , int slot , int *coutcass , char *CassOutID ) { // 2018.08.23 // STARTJ DIFF CASSETTE
	int i , j;
	//
	*coutcass = USER_FIXED_JOB_OUT_CASSETTE; // 2018.08.23 // STARTJ DIFF CASSETTE
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		//
		if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] <= 0 ) continue;
		//
		if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] < 1000 ) {
			if ( !STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , MtlCarrName ) ) continue;
			if ( STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] , "" ) ) continue;
//			if ( STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] ) ) continue; // 2011.03.08
		}
		else {
			if ( !STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , MtlCarrName ) ) continue; // 2018.08.23 // STARTJ DIFF CASSETTE
		}
		//
		if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] < 1000 ) {
			for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
				if ( Sch_Multi_ControlJob_Select_Data[mm]->CassSlotIn[i][j] == slot ) {
					if ( Sch_Multi_ControlJob_Select_Data[mm]->CassSlotOut[i][j] > 0 ) {
						//
						if ( STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] ) ) { // 2011.03.08
							strcpy( CassOutID , "" );
						}
						else {
							strcpy( CassOutID , Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] );
						}
						//
						return Sch_Multi_ControlJob_Select_Data[mm]->CassSlotOut[i][j];
						//
					}
				}
			}
		}
		else if ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] > 1000 ) { // 2018.08.23 // STARTJ DIFF CASSETTE
			for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
				if ( Sch_Multi_ControlJob_Select_Data[mm]->CassSlotIn[i][j] == slot ) {
					//
					if ( STRCMP_L( Sch_Multi_ControlJob_Select_Data[mm]->CassSrcID[i] , Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] ) ) { // 2011.03.08
						strcpy( CassOutID , "" );
					}
					else {
						strcpy( CassOutID , Sch_Multi_ControlJob_Select_Data[mm]->CassOutID[i] );
					}
					//
					*coutcass = ( Sch_Multi_ControlJob_Select_Data[mm]->CassOutUse[i] - 1 ) / 1000;
					//
					if ( Sch_Multi_ControlJob_Select_Data[mm]->CassSlotOut[i][j] > 0 ) {
						return Sch_Multi_ControlJob_Select_Data[mm]->CassSlotOut[i][j];
					}
					else {
						return Sch_Multi_ControlJob_Select_Data[mm]->CassSlotIn[i][j];
					}
					//
				}
			}
		}
		//
	}
	return -1;
}

//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_MAKE_RUNJOB( int id ) {
	int i , j , k , l , Res , mm , s , mo , scm;
	//
	char CassOutID[ MAX_MULTI_STRING_SIZE + 1 ]; // 2011.02.15(Testing) // 2011.05.21

	//
	if ( id == -1 ) { // First Select
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
		}
		mm = 0;
	}
	else if ( id == -2 ) { // Change Select when no more selected // 2003.01.09
		SCHMULTI_PROCESSJOB_SET_STATUS( -99 , "" , PRJOB_STS_QUEUED );
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
		}
		mm = 0;
	}
	else if ( id == -3 ) { // Change Select when someone selected // 2003.01.09
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			if ( MULTI_RUNJOB_RunStatus(i) != CASSRUNJOB_STS_READY ) {
				if ( STRCMP_L( MULTI_RUNJOB_MtlCJName( i ) , Sch_Multi_ControlJob[0]->JobID ) ) {
					MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
				}
			}
		}
		for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
			if ( Sch_Multi_ControlJob[0]->CtrlSpec_Use[i] ) {
				Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob[0]->CtrlSpec_ProcessJobID[i] );
				if ( Res >= 0 ) {
					SCHMULTI_PROCESSJOB_SET_STATUS( Res , "" , PRJOB_STS_QUEUED );
				}
			}
		}
		mm = 0;
	}
	else {
//----------------------------------------------
		mm = id;
//----------------------------------------------
	}
	//

	mo = 0; // 2013.09.27

	// 2011.05.21
	//2011.02.15(Testing)
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[mm]->CtrlSpec_Use[i] ) {
			//
			Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob_Select_Data[mm]->CtrlSpec_ProcessJobID[i] );
			//
			if ( Res >= 0 ) {
				SCHMULTI_PROCESSJOB_SET_STATUS( Res , "" , PRJOB_STS_SETTINGUP );
				for ( j = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
					if ( Sch_Multi_ProcessJob[Res]->MtlUse[j] ) {
						//
						l = -1;
						//
						for ( k = 0 ; k < MAX_MULTI_RUNJOB_SIZE ; k++ ) {
							if ( MULTI_RUNJOB_RunStatus(k) == CASSRUNJOB_STS_READY ) {
								if ( l == -1 ) l = k;
							}
							else {
								if ( MULTI_RUNJOB_RunStatus(k) <= CASSRUNJOB_STS_RUNNING ) { // 2011.04.21
									//
									// 2011.12.14
									if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) {
										if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == MULTI_RUNJOB_Cassette(k) ) {
											l = k;
											break;
										}
									}
									else {
										if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[j] , MULTI_RUNJOB_CarrierID( k ) ) ) {
											if ( !STRCMP_L( "" , MULTI_RUNJOB_CarrierID( k ) ) ) {
												l = k;
												break;
											}
										}
									}
									//
								}
							}
						}
						//
						if ( k == MAX_MULTI_RUNJOB_SIZE ) {
							MULTI_RUNJOB_SelectCJIndex(l) = mm;
							MULTI_RUNJOB_SelectCJOrder(l) = mo;	mo++; // 2013.09.27
							//
							MULTI_RUNJOB_RunSide(l) = -1; // 2018.08.30
							//
							MULTI_RUNJOB_RunStatus(l) = CASSRUNJOB_STS_WAITING;
							//
							// 2011.12.14
							if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) {
								MULTI_RUNJOB_Cassette(l) = Sch_Multi_ProcessJob[Res]->MtlFormat - 100;
							}
							else {
								MULTI_RUNJOB_Cassette(l) = -1;
							}
							//
							MULTI_RUNJOB_CassetteIndex(l) = -1; // 2018.05.24
							//
							MULTI_RUNJOB_MtlCount(l) = 0;
							strncpy( MULTI_RUNJOB_MtlCJName( l ) , Sch_Multi_ControlJob_Select_Data[mm]->JobID , 128 );
							//
							strcpy( MULTI_RUNJOB_CarrierID( l ) , Sch_Multi_ProcessJob[Res]->MtlCarrName[j] );
							//
							MULTI_RUNJOB_MtlCJ_StartMode( l ) = Sch_Multi_ControlJob_Select_Data[mm]->StartMode;

							for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
								MULTI_RUNJOB_MtlPJIDRes(l)[k] = PJIDRES_READY;
								MULTI_RUNJOB_MtlPJIDEnd(l)[k] = 0;
							}
						}
						//
						for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							if ( Sch_Multi_ProcessJob[Res]->MtlSlot[j][k] > 0 ) {
								//
								if ( MULTI_RUNJOB_MtlCount(l) >= MAX_RUN_CASSETTE_SLOT_SIZE ) break; // 2018.05.10
								//
								MULTI_RUNJOB_Mtl_Side(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
								MULTI_RUNJOB_Mtl_Pointer(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
								//
								//=============================================================================================
								strncpy( MULTI_RUNJOB_MtlPJName( l )[MULTI_RUNJOB_MtlCount(l)] , Sch_Multi_ProcessJob[Res]->JobID , 128 );
								MULTI_RUNJOB_MtlPJID(l)[MULTI_RUNJOB_MtlCount(l)] = Res;
								MULTI_RUNJOB_MtlPJ_StartMode(l)[MULTI_RUNJOB_MtlCount(l)] = Sch_Multi_ProcessJob[Res]->StartMode;
								//=============================================================================================
								MULTI_RUNJOB_MtlInCassette(l)[MULTI_RUNJOB_MtlCount(l)] = 0; // 2012.04.01(Testing)
								strcpy( MULTI_RUNJOB_MtlInCarrierID(l)[MULTI_RUNJOB_MtlCount(l)] , "" ); // 2012.04.01(Testing)
								MULTI_RUNJOB_MtlInCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
								MULTI_RUNJOB_MtlInSlot(l)[MULTI_RUNJOB_MtlCount(l)] = Sch_Multi_ProcessJob[Res]->MtlSlot[j][k];
								//
								s = Sch_Get_Multi_OutData_from_ControlJob( mm , Sch_Multi_ProcessJob[Res]->MtlCarrName[j] , Sch_Multi_ProcessJob[Res]->MtlSlot[j][k] , &scm , CassOutID ); // 2009.04.03
								//
								if ( s == -1 ) { // 2009.04.03
									MULTI_RUNJOB_MtlOutCassette(l)[MULTI_RUNJOB_MtlCount(l)] = USER_FIXED_JOB_OUT_CASSETTE; // 2018.07.05
									strcpy( MULTI_RUNJOB_MtlOutCarrierID(l)[MULTI_RUNJOB_MtlCount(l)] , "" );
									MULTI_RUNJOB_MtlOutSlot(l)[MULTI_RUNJOB_MtlCount(l)] = MULTI_RUNJOB_MtlInSlot(l)[MULTI_RUNJOB_MtlCount(l)]; // 2009.04.03
									//
									MULTI_RUNJOB_MtlOutCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
									//
								}
								else {
									//
									MULTI_RUNJOB_MtlOutCassette(l)[MULTI_RUNJOB_MtlCount(l)] = scm; // 2018.07.05
									strcpy( MULTI_RUNJOB_MtlOutCarrierID(l)[MULTI_RUNJOB_MtlCount(l)] , CassOutID );
									MULTI_RUNJOB_MtlOutSlot(l)[MULTI_RUNJOB_MtlCount(l)] = s; // 2009.04.03
									//
									if ( scm >= CM1 )
										MULTI_RUNJOB_MtlOutCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = SCHEDULER_Get_CARRIER_INDEX( scm - CM1 ); // 2018.08.23
									else
										MULTI_RUNJOB_MtlOutCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
									//
								}
								//
								strncpy( MULTI_RUNJOB_MtlRecipeName( l )[MULTI_RUNJOB_MtlCount(l)] , Sch_Multi_ProcessJob[Res]->MtlRecipeName , 128 );
								//=============================================================================================
								MULTI_RUNJOB_MtlRecipeMode(l)[MULTI_RUNJOB_MtlCount(l)] = Sch_Multi_ProcessJob[Res]->MtlRecipeMode;
								//=============================================================================================
								MULTI_RUNJOB_MtlCount(l)++;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SCHMULTI_CONTROLJOB_MAKE_RUNJOB_PRECHECKING( int id ) { // 2004.06.17
	int i , j , k , l , Res , mm , s , mo , scm;

	char CassOutID[ MAX_MULTI_STRING_SIZE + 1 ]; // 2011.02.15(Testing) // 2011.05.21

	//
	mm = id;

	mo = 0; // 2013.09.27

	// 2011.05.21

	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[mm]->CtrlSpec_Use[i] ) {
			Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob_Select_Data[mm]->CtrlSpec_ProcessJobID[i] );
			if ( Res >= 0 ) {
				for ( j = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
					if ( Sch_Multi_ProcessJob[Res]->MtlUse[j] ) {
						l = -1;
						for ( k = 0 ; k < MAX_MULTI_RUNJOB_SIZE ; k++ ) {
							if ( MULTI_RUNJOB_RunStatus(k) == CASSRUNJOB_STS_READY ) {
								if ( l == -1 ) l = k;
							}
							else {
								if ( MULTI_RUNJOB_RunStatus(k) <= CASSRUNJOB_STS_RUNNING ) { // 2011.04.21
									//
									// 2011.12.14
									if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) {
										if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == MULTI_RUNJOB_Cassette(k) ) {
											l = k;
											break;
										}
									}
									else {
										if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[j] , MULTI_RUNJOB_CarrierID( k ) ) ) {
											if ( !STRCMP_L( "" , MULTI_RUNJOB_CarrierID( k ) ) ) {
												l = k;
												break;
											}
										}
									}
									//
								}
							}
						}
						if ( k == MAX_MULTI_RUNJOB_SIZE ) {
							MULTI_RUNJOB_SelectCJIndex(l) = mm;
							MULTI_RUNJOB_SelectCJOrder(l) = mo;	mo++; // 2013.09.27
							//
							MULTI_RUNJOB_RunSide(l) = -1; // 2018.08.30
							//
							// 2011.12.14
							if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) {
								MULTI_RUNJOB_Cassette(l) = Sch_Multi_ProcessJob[Res]->MtlFormat - 100;
							}
							else {
								MULTI_RUNJOB_Cassette(l) = -1;
							}
							//
							MULTI_RUNJOB_CassetteIndex(l) = -1; // 2018.05.24
							//
							MULTI_RUNJOB_MtlCount(l) = 0;
							strncpy( MULTI_RUNJOB_MtlCJName( l ) , Sch_Multi_ControlJob_Select_Data[mm]->JobID , 128 );
							//
							strcpy( MULTI_RUNJOB_CarrierID( l ) , Sch_Multi_ProcessJob[Res]->MtlCarrName[j] );
							//
							//
						}
						for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							if ( Sch_Multi_ProcessJob[Res]->MtlSlot[j][k] > 0 ) {
								//
								if ( MULTI_RUNJOB_MtlCount(l) >= MAX_RUN_CASSETTE_SLOT_SIZE ) break; // 2018.05.10
								//
								MULTI_RUNJOB_Mtl_Side(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
								MULTI_RUNJOB_Mtl_Pointer(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
								//
								//=============================================================================================
								strncpy( MULTI_RUNJOB_MtlPJName( l )[MULTI_RUNJOB_MtlCount(l)] , Sch_Multi_ProcessJob[Res]->JobID , 128 );
								//=============================================================================================
								MULTI_RUNJOB_MtlInCassette(l)[MULTI_RUNJOB_MtlCount(l)] = 0; // 2012.04.01(Testing)
								strcpy( MULTI_RUNJOB_MtlInCarrierID(l)[MULTI_RUNJOB_MtlCount(l)] , "" ); // 2012.04.01(Testing)
								MULTI_RUNJOB_MtlInCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
								MULTI_RUNJOB_MtlInSlot(l)[MULTI_RUNJOB_MtlCount(l)] = Sch_Multi_ProcessJob[Res]->MtlSlot[j][k];
								//
								s = Sch_Get_Multi_OutData_from_ControlJob( mm , Sch_Multi_ProcessJob[Res]->MtlCarrName[j] , Sch_Multi_ProcessJob[Res]->MtlSlot[j][k] , &scm , CassOutID ); // 2009.04.03
								//
								if ( s == -1 ) { // 2009.04.03
									MULTI_RUNJOB_MtlOutCassette(l)[MULTI_RUNJOB_MtlCount(l)] = USER_FIXED_JOB_OUT_CASSETTE; // 2018.07.05
									strcpy( MULTI_RUNJOB_MtlOutCarrierID(l)[MULTI_RUNJOB_MtlCount(l)] , "" );
									MULTI_RUNJOB_MtlOutSlot(l)[MULTI_RUNJOB_MtlCount(l)] = MULTI_RUNJOB_MtlInSlot(l)[MULTI_RUNJOB_MtlCount(l)]; // 2009.04.03
									//
									MULTI_RUNJOB_MtlOutCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
									//
								}
								else {
									MULTI_RUNJOB_MtlOutCassette(l)[MULTI_RUNJOB_MtlCount(l)] = scm; // 2018.07.05
									strcpy( MULTI_RUNJOB_MtlOutCarrierID(l)[MULTI_RUNJOB_MtlCount(l)] , CassOutID );
									MULTI_RUNJOB_MtlOutSlot(l)[MULTI_RUNJOB_MtlCount(l)] = s; // 2009.04.03
									//
									if ( scm >= CM1 )
										MULTI_RUNJOB_MtlOutCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = SCHEDULER_Get_CARRIER_INDEX( scm - CM1 ); // 2018.08.23
									else
										MULTI_RUNJOB_MtlOutCarrierIndex(l)[MULTI_RUNJOB_MtlCount(l)] = -1; // 2018.05.10
									//
								}
								//
								strncpy( MULTI_RUNJOB_MtlRecipeName( l )[MULTI_RUNJOB_MtlCount(l)] , Sch_Multi_ProcessJob[Res]->MtlRecipeName , 128 );
								//=============================================================================================
								MULTI_RUNJOB_MtlCount(l)++;
							}
						}
					}
				}
			}
		}
	}


	return l;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_JOB_OVERTIME_DELETE( int *ovcnt ) {
	int i , Res;
	long l;
	int sec;
	time_t finish;
	//
	if ( SCHMULTI_CHECK_OVERTIME_HOUR <= 0 ) return -1;
	//
	sec = ( SCHMULTI_CHECK_OVERTIME_HOUR * JOB_OVERTIME_CHECK_HOUR_SEC );
	//
	if ( *ovcnt < ( 4 * JOB_OVERTIME_CHECK_PERIOD ) ) {
		(*ovcnt)++;
		return -1;
	}
	else {
		*ovcnt = 0;
	}
	//
	time( &finish );
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		//
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus != CTLJOB_STS_SELECTED ) continue;
		//
		l = (long) difftime( finish , Sch_Multi_Ins_ControlJob_Select_Data[i].InsertTime );
		//
		if ( l > sec ) {
			//
			_IO_CIM_PRINTF( "CONTROLJOB(S) [%s] OVERTIME [%d]sec DELETE\n" , Sch_Multi_ControlJob_Select_Data[i]->JobID , sec );
			//
			Res = SCHMULTI_CONTROLJOB_DELSELECT( 2 , i , 0 , FALSE );
			//
			if ( Res != 0 ) _IO_CIM_PRINTF( "CONTROLJOB(S) [%s] OVERTIME [%d]sec FAIL[%d]\n" , Sch_Multi_ControlJob_Select_Data[i]->JobID , sec , Res );
			//
			return 1;
		}
		//
	}
	//
	for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
		//
		l = (long) difftime( finish , Sch_Multi_Ins_ControlJob[i].InsertTime );
		//
		if ( l > sec ) {
			//
			_IO_CIM_PRINTF( "CONTROLJOB [%s] OVERTIME [%d]sec DELETE\n" , Sch_Multi_ControlJob[i]->JobID , sec );
			//
			Res = SCHMULTI_CONTROLJOB_DELETE( i );
			//
			if ( Res != 0 ) _IO_CIM_PRINTF( "CONTROLJOB [%s] OVERTIME [%d]sec FAIL[%d]\n" , Sch_Multi_ControlJob[i]->JobID , sec , Res );
			return 2;
		}
		//
	}
	//
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		//
		if ( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_QUEUED ) continue;
		//
		if ( Sch_Multi_ProcessJob[i]->ResultStatus > 0 ) continue;
		//
		l = (long) difftime( finish , Sch_Multi_Ins_ProcessJob[i].InsertTime );
		//
		if ( l > sec ) {
			//
			_IO_CIM_PRINTF( "PROCESSJOB [%s] OVERTIME [%d]sec DELETE\n" , Sch_Multi_ProcessJob[i]->JobID , sec );
			//
			Res = SCHMULTI_PROCESSJOB_DELETE( -1 , i );
			//
			if ( Res != 0 ) _IO_CIM_PRINTF( "PROCESSJOB [%s] OVERTIME [%d]sec FAIL[%d]\n" , Sch_Multi_ProcessJob[i]->JobID , sec , Res );
			return 3;
		}
		//
	}
	//
	return 0;
}

BOOL SCHMULTI_REGISTER_DATA_SET_AND_CHECK_FOR_START( BOOL runstyle , int Res , int Side , int Cass , int CassOut , int *StartMode , int carrmode ) {
	Scheduling_Regist Scheduler_Reg;
	int l , m , maxunuse;
	//
	Scheduler_Reg.RunIndex  = Res;
	Scheduler_Reg.CassIn    = Cass;
	//
	if ( CassOut == -2 ) { // 2018.07.17
		Scheduler_Reg.CassOut   = -1;
	}
	else if ( CassOut == -1 ) {
		Scheduler_Reg.CassOut   = Cass;
	}
	else { // 2009.04.03
		Scheduler_Reg.CassOut   = CassOut;
	}
	//
	Scheduler_Reg.SlotStart = 1;
	Scheduler_Reg.SlotEnd   = Scheduler_Get_Max_Slot( Scheduler_Reg.CassIn , Scheduler_Reg.CassIn + 1 , Scheduler_Reg.CassOut + 1 , 12 , &maxunuse ); // 2010.12.17
	//
	if ( carrmode ) { // 2008.09.12
		m = _i_SCH_IO_GET_LOOP_COUNT( Cass );
		if ( m <= 0 ) {
			Scheduler_Reg.LoopCount = 1;
		}
		else {
			Scheduler_Reg.LoopCount = m;
		}
	}
	else {
		Scheduler_Reg.LoopCount = 1;
	}
	//
	switch( SCHMULTI_JOBNAME_DISPLAYMODE_GET() ) {
	case 1 : // P-J(0)
		strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlPJName( Res )[0] , 512 );
		break;
	case 2 : // P-J(A)
		strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlPJName( Res )[0] , 512 );
		for ( l = 1 ; l < MULTI_RUNJOB_MtlCount(Res) ; l++ ) {
			for ( m = 0 ; m < l ; m++ ) { // 2005.05.23
				if ( MULTI_RUNJOB_MtlPJID(Res)[l] == MULTI_RUNJOB_MtlPJID(Res)[m] ) break; // 2005.05.23
			} // 2005.05.23
			if ( m >= l ) { // 2005.05.23
				strcat( Scheduler_Reg.JobName , "|" );
				strcat( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlPJName( Res )[l] );
			}
		}
		break;
	case 3 : // LotID
		strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlRecipeName( Res )[0] , 512 );
		break;
	case 4 : // MID
		if ( strlen( MULTI_RUNJOB_CarrierID( Res ) ) <= 0 ) { // 2008.04.23
			if ( MULTI_RUNJOB_Cassette(Res) < 0 ) {
				sprintf( Scheduler_Reg.JobName , "CASSETTE?" );
			}
			else {
				sprintf( Scheduler_Reg.JobName , "CASSETTE%d" , MULTI_RUNJOB_Cassette(Res) + 1 );
			}
		}
		else {
//			strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_CarrierID( Res ) , 512 ); // 2011.12.14
			if ( MULTI_RUNJOB_Cassette(Res) < 0 ) {
				strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_CarrierID( Res ) , 512 );
			}
			else {
				_snprintf( Scheduler_Reg.JobName , 512 , "CASSETTE%d:%s" , MULTI_RUNJOB_Cassette(Res) + 1 , MULTI_RUNJOB_CarrierID( Res ) );
			}
		}
		break;
	case 5 : // C-J|P-J
		strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlCJName( Res ) , 512 );
		strcat( Scheduler_Reg.JobName , "|" );
		strcat( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlPJName( Res )[0] );
		for ( l = 1 ; l < MULTI_RUNJOB_MtlCount(Res) ; l++ ) {
			for ( m = 0 ; m < l ; m++ ) { // 2005.05.23
				if ( MULTI_RUNJOB_MtlPJID(Res)[l] == MULTI_RUNJOB_MtlPJID(Res)[m] ) break; // 2005.05.23
			} // 2005.05.23
			if ( m >= l ) { // 2005.05.23
				strcat( Scheduler_Reg.JobName , "|" );
				strcat( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlPJName( Res )[l] );
			}
		}
		break;
	default : // C-J
		//
		strncpy( Scheduler_Reg.JobName , MULTI_RUNJOB_MtlCJName( Res ) , 512 );
		//
/*
		// updating....
		if ( carrmode ) { // 2012.01.17
			switch( _i_SCH_PRM_GET_FA_JID_READ_POINT(Cass) ) {
			case 1 :
				IO_GET_JID_NAME_FROM_READ( Cass , Scheduler_Reg.JobName );
				break;
			case 2 :
				IO_GET_JID_NAME_FROM_READ( Cass , Scheduler_Reg.JobName );
				break;
			case 3 :
				IO_GET_JID_NAME( Cass , Scheduler_Reg.JobName );
				break;
			}
			//
		}
*/
		break;
	}
	//=================================================================================================================
	strncpy( Scheduler_Reg.LotName , MULTI_RUNJOB_MtlRecipeName( Res )[0] , 128 ); // 2002.12.16
	//
	if ( carrmode ) { // 2008.09.12
		if ( strlen( MULTI_RUNJOB_CarrierID( Res ) ) <= 0 ) {
			if      ( _i_SCH_PRM_GET_FA_MID_READ_POINT(Cass) == 1 ) {
				IO_GET_MID_NAME_FROM_READ( Cass , MULTI_RUNJOB_CarrierID( Res ) );
			}
			else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(Cass) == 2 ) {
			}
			else if ( _i_SCH_PRM_GET_FA_MID_READ_POINT(Cass) == 3 ) {
				IO_GET_MID_NAME( Cass , MULTI_RUNJOB_CarrierID( Res ) );
			}
		}
	}
	//
	strncpy( Scheduler_Reg.MidName , MULTI_RUNJOB_CarrierID( Res ) , 128 );
	//========================================================================================
	*StartMode = 1; // 2007.03.27
	//========================================================================================
//	if ( !USER_RECIPE_JOB_DEFAULT_MODE_CHANGE( runstyle , Res , Cass , Cass , Scheduler_Reg.JobName , Scheduler_Reg.LotName , Scheduler_Reg.MidName , &(Scheduler_Reg.SlotStart) , &(Scheduler_Reg.SlotEnd) , StartMode , &(Scheduler_Reg.LoopCount) ) ) return FALSE; // 2007.03.27 // 2013.04.02
//	if ( !_i_SCH_MULTIREG_SET_REGIST_DATA( Cass , &Scheduler_Reg ) ) return FALSE; // 2013.04.02
	//
	if ( !USER_RECIPE_JOB_DEFAULT_MODE_CHANGE( runstyle , Res , Side , Cass , Scheduler_Reg.JobName , Scheduler_Reg.LotName , Scheduler_Reg.MidName , &(Scheduler_Reg.SlotStart) , &(Scheduler_Reg.SlotEnd) , StartMode , &(Scheduler_Reg.LoopCount) ) ) return FALSE; // 2007.03.27 // 2013.04.02
	if ( !_i_SCH_MULTIREG_SET_REGIST_DATA( Side , &Scheduler_Reg ) ) return FALSE; // 2013.04.02
	return TRUE;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( int ch , BOOL actmode , int , char * , int );
//
BOOL SCHMULTI_CONTROLJOB_PARALLEL_GET_ORDERING_ONE_SELECT( int *s_side ) { // 2009.03.03
	int cass , j , k;
	int sel_cass = -1;
	int sel_cjp  = -1;
	int ff , Res , mmok , mmer;
	char Buffer[256];

	if ( *Sch_Multi_ControlJob_Size <= 0 ) return FALSE;

	for ( cass = 0 ; cass < MAX_CASSETTE_SIDE ; cass++ ) {
		//---------------------------------------------------------------------------------------------
		if ( BUTTON_GET_FLOW_STATUS_VAR( cass ) != _MS_5_MAPPED ) continue;
		//---------------------------------------------------------------------------------------------
		if ( !SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( cass , 1 , 2 , Buffer , 0 ) ) continue;
		//---------------------------------------------------------------------------------------------
		//if ( SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE( 0 , cass , SCHMULTI_CASSETTE_MID_GET( cass ) , &mmok , &Res , &mmer , &mmer ) != 0 ) continue; // 2012.04.03
		if ( SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE( 0 , cass , Buffer , &mmok , &Res , &mmer , &mmer , &mmer ) != 0 ) continue; // 2012.04.03
		//---------------------------------------------------------------------------------------------
		//
		EnterCriticalSection( &CR_MULTIJOB );
		//
		for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
			if ( MULTI_RUNJOB_RunStatus(j) == CASSRUNJOB_STS_RUNNING ) {
				if ( MULTI_RUNJOB_Cassette(j) == cass ) break;
			}
		}
		//--
		if ( j != MAX_MULTI_RUNJOB_SIZE ) {
			//
			LeaveCriticalSection( &CR_MULTIJOB );
			//
			continue;
		}
		//---------------------------------------------------------------------------------------------
		for ( ff = 0 ; ff < *Sch_Multi_ControlJob_Size ; ff++ ) {
			mmok = 0;
			mmer = 0;
			for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_ControlJob[ff]->CtrlSpec_Use[j] ) {
					Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob[ff]->CtrlSpec_ProcessJobID[j] );
					if ( Res >= 0 ) {
						switch( Sch_Multi_ProcessJob[Res]->ControlStatus ) {
						case PRJOB_STS_NOTSTATE :
						case PRJOB_STS_QUEUED :
							for ( k = 0 ; k < MAX_MULTI_PRJOB_CASSETTE ; k++ ) {
								if ( Sch_Multi_ProcessJob[Res]->MtlUse[k] ) {
									/*
									// 2011.12.14
									if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , "" ) ) {
										if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == cass ) {
											mmok++;
										}
										else {
											mmer++;
										}
									}
									else {
										if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , SCHMULTI_CASSETTE_MID_GET( cass ) ) ) {
											mmok++;
										}
										else {
											mmer++;
										}
									}
									*/
									// 2011.12.14
									if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) {
										if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == cass ) {
											mmok++;
										}
										else {
											mmer++;
										}
									}
									else {
										if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , SCHMULTI_CASSETTE_MID_GET( cass ) ) ) {
											mmok++;
										}
										else {
											mmer++;
										}
									}
								}
							}
							break;
						}
					}
				}
			}
			if ( ( mmok > 0 ) && ( mmer <= 0 ) ) {
				if ( sel_cass == -1 ) {
					sel_cass = cass;
					sel_cjp = ff;
				}
				else {
					if ( ff < sel_cjp ) {
						sel_cass = cass;
						sel_cjp = ff;
					}
				}
				break;
			}
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB );
		//
		//---------------------------------------------------------------------------------------------
	}
	//
	if ( sel_cass != -1 ) {
		*s_side = sel_cass;
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------

BOOL Scheduler_Job_Current_Side_is_Old( int currside , int *neworder ) { // 2017.10.31
	int i , currcj , cj , s;
	int min , max;
	BOOL find;
	//
	if (
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE != 4 ) &&
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE != 5 ) &&
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE != 6 ) &&
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE != 7 )
		) {
		return FALSE;
	}
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	currcj = -1;
	//
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_READY ) continue;
		//
//		if ( currside == MULTI_RUNJOB_Cassette(i) ) { // 2018.08.30
		if ( currside == MULTI_RUNJOB_RunSide(i) ) { // 2018.08.30
			//
			currcj = MULTI_RUNJOB_SelectCJIndex(i);
			//
			break;
		}
		//
	}
	//
	if ( currcj < 0 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return FALSE;
	}
	//
	if ( Sch_Multi_ControlJob_Select_Data[currcj]->ControlStatus < CTLJOB_STS_EXECUTING ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return FALSE;
	}
	//
	find = FALSE;
	min = 0;
	max = 0;
	//
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_READY ) continue;
		//
//		s = MULTI_RUNJOB_Cassette(i); // 2018.08.30
		s = MULTI_RUNJOB_RunSide(i); // 2018.08.30
		//
		if ( s < 0 ) continue;
		//
		if ( s >= MAX_CASSETTE_SIDE ) continue;
		//
		if ( s == currside ) continue;
		//
		cj = MULTI_RUNJOB_SelectCJIndex(i);
		//
		if ( cj < 0 ) continue;
		if ( Sch_Multi_ControlJob_Select_Data[cj]->ControlStatus < CTLJOB_STS_EXECUTING ) continue;
		//
		if ( Sch_Multi_Ins_ControlJob_Select_Data[currcj].InsertTime < Sch_Multi_Ins_ControlJob_Select_Data[cj].InsertTime ) {
			//
			if ( !find ) {
				find = TRUE;
				max = _i_SCH_SYSTEM_RUNORDER_GET( s );
			}
			else {
				if ( max > _i_SCH_SYSTEM_RUNORDER_GET( s ) ) {
					max = _i_SCH_SYSTEM_RUNORDER_GET( s );
				}
			}
		}
		else {
			if ( min < _i_SCH_SYSTEM_RUNORDER_GET( s ) ) {
				min = _i_SCH_SYSTEM_RUNORDER_GET( s );
			}
		}
		//
	}
	//
	if ( !find ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return FALSE;
	}
	//
	if ( min > max ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return FALSE;
	}
	//
	*neworder = ( min + max ) / 2;
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	return TRUE;
}

//BOOL Scheduler_Job_Current_Old( int side , int ff ) { // 2017.10.31 // 2018.08.16
BOOL Scheduler_Job_Current_Old( int side , int cass , int ff ) { // 2017.10.31 // 2018.08.16
	int i , j , s , Chamber;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		//
		if ( i == ff ) continue;
		//
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus < CTLJOB_STS_EXECUTING ) continue;
		//
		if ( Sch_Multi_Ins_ControlJob_Select_Data[ff].InsertTime >= Sch_Multi_Ins_ControlJob_Select_Data[i].InsertTime ) continue;
		//
		for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
			//
			if ( MULTI_RUNJOB_RunStatus(j) == CASSRUNJOB_STS_READY ) continue;
			//
			if ( MULTI_RUNJOB_SelectCJIndex(j) != i ) continue;
			//
//			s = MULTI_RUNJOB_Cassette(j); // 2018.08.30
			s = MULTI_RUNJOB_RunSide(j); // 2018.08.30
			//
//			if ( s == side ) continue; // 2018.08.16
//			if ( s == cass ) continue; // 2018.08.16 // 2018.08.30
			if ( s == side ) continue; // 2018.08.16 // 2018.08.30
			//
			if ( s < 0 ) continue;
			//
			if ( s >= MAX_CASSETTE_SIDE ) continue;
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
			//
			for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_01_USE ) {
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_98_USE_to_DISABLE_RAND ) {
						//
						LeaveCriticalSection( &CR_MULTIJOB );
						return TRUE;
					}
				}
			}
			//
		}
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	return FALSE;
}

//int SCHMULTI_CONTROLJOB_PARALLEL_SELECT( int cass , char *midstring , int *prepossiblecheck , BOOL *selecting , int mode ) { // 2018.08.16
int SCHMULTI_CONTROLJOB_PARALLEL_SELECT( int side , int cass , char *midstring , int *prepossiblecheck , BOOL *selecting , int mode ) { // 2018.08.16
	int ff , j , k , index , Res , mmok , fmok , mmer , mmcr , csize , cl , x , ck , pralskip , StartMode , carrmode , Result , hf;
	char errstr[256];

//printf( "============================================\n" );
//printf( "PS S[Cass=%d] [MID=%s] [%d] [mode=%d]\n" , midstring , SCHMULTI_CURRENT_MANUAL_MODE , mode );
//printf( "============================================\n" );


	EnterCriticalSection( &CR_MULTIJOB );

	hf = 0; // 2013.02.07
	//
	*selecting = FALSE; // 2008.09.18

	if ( *Sch_Multi_ControlJob_Size <= 0 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return -1;
	}

	for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
		if ( MULTI_RUNJOB_RunStatus(j) == CASSRUNJOB_STS_RUNNING ) {
			if ( MULTI_RUNJOB_Cassette(j) == cass ) {
				if ( STRCMP_L( midstring , MULTI_RUNJOB_CarrierID( j ) ) ) { // 2018.11.14
					LeaveCriticalSection( &CR_MULTIJOB ); // 2009.06.09
					return -2;
				}
			}
		}
	}
	//
	/*
	// 2011.04.20
	//=================================================================
	// 2008.09.23
	//=================================================================
	for ( ff = 0 ; ff < *Sch_Multi_ControlJob_Size ; ff++ ) {
		mmok = 0;
		mmer = 0;
		for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
			if ( Sch_Multi_ControlJob[ff]->CtrlSpec_Use[j] ) {
				Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob[ff]->CtrlSpec_ProcessJobID[j] );
				if ( Res >= 0 ) {
					switch( Sch_Multi_ProcessJob[Res]->ControlStatus ) {
					case PRJOB_STS_NOTSTATE :
					case PRJOB_STS_QUEUED :
						for ( k = 0 ; k < MAX_MULTI_PRJOB_CASSETTE ; k++ ) {
							if ( Sch_Multi_ProcessJob[Res]->MtlUse[k] ) {
								if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , "" ) ) {
									if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == cass ) {
										mmok++;
									}
									else {
										mmer++;
									}
								}
								else {
									if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , midstring ) ) {
										mmok++;
									}
									else {
										mmer++;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
		if ( ( mmok > 0 ) && ( mmer <= 0 ) ) {
			*selecting = TRUE; // 2008.09.18
			break;
		}
	}
	*/
	//=================================================================
	//=================================================================
	//=================================================================
	//=================================================================

	index = -1;

	//=================================================================
	// 2007.06.27
	//=================================================================
/*
	csize = 1;							// for Head only check
//	csize = *Sch_Multi_ControlJob_Size; // for All queue check
*/



	//=================================================================
	// 2007.06.27
	//=================================================================
//	if ( mode == 0 ) { // 2008.05.09 // 2009.05.11
	if ( ( mode == 0 ) || ( mode == 2 ) ) { // 2008.05.09 // 2009.05.11
		csize = 1;							// for Head only check
	}
	else {
		switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) {
		case 10 :
		case 11 : // randomize
			csize = *Sch_Multi_ControlJob_Size; // for All queue check
			break;
		default :
			csize = 1;							// for Head only check
			break;
		}
	}
//printf( "============================================\n" );
//printf( "PS S2[Cass=%d] [MID=%s] [%d] [mode=%d] [csize=%d]\n" , midstring , SCHMULTI_CURRENT_MANUAL_MODE , mode , csize );
//printf( "============================================\n" );
	//=================================================================
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		if ( x == 0 ) {
			cl = *Sch_Multi_ControlJob_Size;
		}
		else {
			cl = csize;
		}
		//
		for ( ff = 0 ; ff < cl ; ff++ ) {
			//
			mmok = 0;
			mmer = 0;
			mmcr = FALSE; // 2008.09.12
			fmok = 0; // 2011.04.20
			//
			for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				//---------------------------------------------------------------------
				if ( Sch_Multi_Ins_ControlJob[j].Queued_to_Select_Wait ) continue; // 2012.09.26
				//---------------------------------------------------------------------
				if ( Sch_Multi_ControlJob[ff]->CtrlSpec_Use[j] ) {
					Res = SCHMULTI_PROCESSJOB_Find( Sch_Multi_ControlJob[ff]->CtrlSpec_ProcessJobID[j] );
					if ( Res >= 0 ) {
						switch( Sch_Multi_ProcessJob[Res]->ControlStatus ) {
						case PRJOB_STS_NOTSTATE :
						case PRJOB_STS_QUEUED :
							for ( k = 0 ; k < MAX_MULTI_PRJOB_CASSETTE ; k++ ) {
								if ( Sch_Multi_ProcessJob[Res]->MtlUse[k] ) {
									/*
									// 2011.12.14
									if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , "" ) ) {
										if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == cass ) {
											//
											if ( fmok == 0 ) fmok = 1;
											//
											mmok++;
											mmcr = TRUE; // 2008.09.12
										}
										else {
											if ( fmok == 0 ) fmok = 2;
											mmer++;
										}
									}
									else {
										if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , midstring ) ) {
											//
											if ( fmok == 0 ) fmok = 1;
											//
											mmok++;
											mmcr = FALSE; // 2008.09.12
										}
										else {
											if ( fmok == 0 ) fmok = 2;
											mmer++;
										}
									}
									*/
									// 2011.12.14
									if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) {
										if ( ( Sch_Multi_ProcessJob[Res]->MtlFormat - 100 ) == cass ) {
											//
											if ( fmok == 0 ) fmok = 1;
											//
											mmok++;
											mmcr = TRUE; // 2008.09.12
										}
										else {
											if ( fmok == 0 ) fmok = 2;
											mmer++;
										}
									}
									else {
										if ( STRCMP_L( Sch_Multi_ProcessJob[Res]->MtlCarrName[k] , midstring ) ) {
											//
											if ( fmok == 0 ) fmok = 1;
											//
											mmok++;
											mmcr = FALSE; // 2008.09.12
										}
										else {
											if ( fmok == 0 ) fmok = 2;
											mmer++;
										}
									}
								}
							}
							break;
						}
					}
				}
			}
//			if ( ( mmok > 0 ) && ( mmer <= 0 ) ) { // 2011.04.20
			if ( ( fmok == 1 ) || ( ( mmok > 0 ) && ( mmer <= 0 ) ) ) { // 2011.04.20
				if ( x == 0 ) {
					*selecting = TRUE; // 2008.09.18
				}
				else {
					index = ff;
					carrmode = mmcr; // 2008.09.12
				}
				break;
			}
		}
	}

//printf( "============================================\n" );
//printf( "PS S3[Cass=%d] [MID=%s] [%d] [mode=%d][index=%d][carrmode=%d]\n" , midstring , SCHMULTI_CURRENT_MANUAL_MODE , mode , index , carrmode );
//printf( "============================================\n" );

	if ( index == -1 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return -11;
	}

	for ( ff = 0 ; ff < MAX_MULTI_CTJOBSELECT_SIZE ; ff++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus == CTLJOB_STS_NOTSTATE ) {
			break;
		}
	}
	if ( ff == MAX_MULTI_CTJOBSELECT_SIZE ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return -12;
	}
	//
	memcpy( Sch_Multi_ControlJob_Select_Data[ff] , Sch_Multi_ControlJob[index] , sizeof(Scheduling_Multi_ControlJob) );
	//
	memcpy( &(Sch_Multi_Ins_ControlJob_Select_Data[ff]) , &(Sch_Multi_Ins_ControlJob[index]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ControlJob_Ext_Select_Data[ff] , Sch_Multi_ControlJob_Ext[index] , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
//printf( "============================================\n" );
//printf( "PS S4[Cass=%d] [MID=%s] [%d] [mode=%d][index=%d]\n" , midstring , SCHMULTI_CURRENT_MANUAL_MODE , mode , index );
//printf( "============================================\n" );
	//=========================================================================================================================
	// 2004.06.16
	//=========================================================================================================================
	if (
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 4 ) ||
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 5 ) ||
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 6 ) ||
		( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE == 7 )
		) {
		//------------------------------------------
		// 2004.06.28
		//------------------------------------------
		switch( SCHMULTI_AFTER_SELECT_CHECK_GET() ) {
		case 0 :
			//-----------------------------------------------------------------------
			strcpy( Sch_Multi_ControlJob_Select_Data[ff]->JobID , "" ); // 2006.03.08
			//-----------------------------------------------------------------------
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE;
			Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
//			_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT REJECT AFTERTIME CHECK - ERROR0 [%d]\n" , cass );
			LeaveCriticalSection( &CR_MULTIJOB );
			return -21;
			break;
		case 1 :
			//-----------------------------------------------------------------------
			strcpy( Sch_Multi_ControlJob_Select_Data[ff]->JobID , "" ); // 2006.03.08
			//-----------------------------------------------------------------------
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE;
			Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
//			_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT REJECT AFTERTIME CHECK - ERROR1 [%d]\n" , cass );
			LeaveCriticalSection( &CR_MULTIJOB );
			return -22;
			break;
		case 2 :
			//-----------------------------------------------------------------------
			strcpy( Sch_Multi_ControlJob_Select_Data[ff]->JobID , "" ); // 2006.03.08
			//-----------------------------------------------------------------------
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE;
			Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
//			_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT REJECT AFTERTIME CHECK - ERROR2 [%d]\n" , cass );
			Sleep(500);
			SCHMULTI_AFTER_SELECT_CHECK_SET( 3 );
			LeaveCriticalSection( &CR_MULTIJOB );
			return -23;
			break;
		case 3 :
			//-----------------------------------------------------------------------
			strcpy( Sch_Multi_ControlJob_Select_Data[ff]->JobID , "" ); // 2006.03.08
			//-----------------------------------------------------------------------
			Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE;
			Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
//			_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT REJECT AFTERTIME CHECK - ERROR3 [%d]\n" , cass );
//			Sleep(2500); // 2007.11.30
			Sleep(500); // 2007.11.30
			SCHMULTI_AFTER_SELECT_CHECK_SET( 4 );
			LeaveCriticalSection( &CR_MULTIJOB );
			return -24;
			break;
		}
		//------------------------------------------
		ck = FALSE;
		if ( *prepossiblecheck == 0 ) {
			*prepossiblecheck = 1;
			Res = SCHMULTI_CONTROLJOB_MAKE_RUNJOB_PRECHECKING( ff ); // 2004.06.17
			if ( Res >= 0 ) {
//				if ( SCHMULTI_REGISTER_DATA_SET_AND_CHECK_FOR_START( FALSE , Res , cass , &StartMode , FALSE ) ) { // 2008.09.12
//				if ( SCHMULTI_REGISTER_DATA_SET_AND_CHECK_FOR_START( FALSE , Res , cass , cass , -1 , &StartMode , carrmode ) ) { // 2008.09.12 // 2018.08.16
				if ( SCHMULTI_REGISTER_DATA_SET_AND_CHECK_FOR_START( FALSE , Res , side , cass , -1 , &StartMode , carrmode ) ) { // 2008.09.12 // 2018.08.16
					//
					Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_QUEUED; // 2015.07.09
					//
					LeaveCriticalSection( &CR_MULTIJOB ); // 2015.07.09
					//
//					_i_SCH_SYSTEM_FA_SET( cass , 1 ); // 2018.08.16
					_i_SCH_SYSTEM_FA_SET( side , 1 ); // 2018.08.16
//					_i_SCH_SYSTEM_CPJOB_SET( cass , 1 ); // 2018.08.16
					_i_SCH_SYSTEM_CPJOB_SET( side , 1 ); // 2018.08.16
//					_i_SCH_SYSTEM_CPJOB_ID_SET( cass , Res ); // 2018.08.16
					_i_SCH_SYSTEM_CPJOB_ID_SET( side , Res ); // 2018.08.16
//					_i_SCH_SYSTEM_BLANK_SET( cass , 0 ); // 2011.04.20 // 2018.08.16
					_i_SCH_SYSTEM_BLANK_SET( side , 0 ); // 2011.04.20 // 2018.08.16
//					_i_SCH_SYSTEM_MOVEMODE_SET( cass , Sch_Multi_Ins_ControlJob_Select_Data[ff].MoveMode ); // 2013.09.03 // 2018.08.16
					_i_SCH_SYSTEM_MOVEMODE_SET( side , Sch_Multi_Ins_ControlJob_Select_Data[ff].MoveMode ); // 2013.09.03 // 2018.08.16
//					_i_SCH_SYSTEM_RESTART_SET( cass , 0 ); // 2011.09.23 // 2018.08.16
					_i_SCH_SYSTEM_RESTART_SET( side , 0 ); // 2011.09.23 // 2018.08.16
//					j = Scheduler_Run_Main_Handling_Code_CheckOnly( cass + 100 ); // 2007.03.27
					j = Scheduler_Run_Main_Handling_Code_CheckOnly( side + ( StartMode * 100 ) , errstr ); // 2007.03.27 // 2018.08.16
					//
					EnterCriticalSection( &CR_MULTIJOB ); // 2015.07.09
					//
					if ( j == 0 ) {
						ck = TRUE;
					}
					else {
						if ( j != 1000 ) { // 2007.03.22
							_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT WAITRUN CHECK - ERROR1 [%d] ===> [%d:%s]\n" , cass , j , errstr );
						}
					}
				}
				else {
					_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT WAITRUN CHECK - ERROR2 [%d]\n" , cass );
				}
			}
			else {
				_IO_CIM_PRINTF( "SCHMULTI_CONTROLJOB_PARALLEL_SELECT WAITRUN CHECK - ERROR3 [%d] \n" , cass );
			}
		}
		else {
			ck = TRUE;
		}
//printf( "============================================\n" );
//printf( "PS S5[Cass=%d] [MID=%s] [%d] [mode=%d]\n" , midstring , SCHMULTI_CURRENT_MANUAL_MODE , mode );
//printf( "============================================\n" );
		if ( ck ) {
			//
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.07.09
			//
			//
//			if ( Scheduler_Job_Current_Old( cass , ff ) ) { // 2017.10.31 // 2018.08.16
			if ( Scheduler_Job_Current_Old( side , cass , ff ) ) { // 2017.10.31 // 2018.08.16
				hf = 101;
			}
			//
			else {
//				if ( Scheduler_Main_Waiting( cass , 1 , &pralskip , 0 , &Result ) == SYS_ABORTED ) { // 2018.08.16
				if ( Scheduler_Main_Waiting( side , 1 , &pralskip , 0 , &Result ) == SYS_ABORTED ) { // 2018.08.16

	//printf( "============================================\n" );
	//printf( "PS W[%d] [%d] => Result is %d\n" , cass , SCHMULTI_CURRENT_MANUAL_MODE , Result );
	//printf( "============================================\n" );

					//
					EnterCriticalSection( &CR_MULTIJOB ); // 2015.07.09
					//
					//-----------------------------------------------------------------------
					strcpy( Sch_Multi_ControlJob_Select_Data[ff]->JobID , "" ); // 2006.03.08
					//-----------------------------------------------------------------------
					Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE;
					Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
					//
					LeaveCriticalSection( &CR_MULTIJOB );
					//
					// 2013.02.08
					if      ( Result == 0 )
						return -25;
					else if ( Result <  0 )
						return -26 + ( Result * 100 );
					else
						return -27 - ( Result * 100 );
					//
				}
				else {
					//
					if ( Result < 0 )
						hf = -Result;
					else
						hf = Result;
					//
				}
			}
			//
//			*prepossiblecheck = 2; // 2006.06.28 // 2009.05.11
			if ( mode != 1 ) *prepossiblecheck = 2; // 2006.06.28 // 2009.05.11
			//
			EnterCriticalSection( &CR_MULTIJOB ); // 2015.07.09
			//
		}
	}
	//=========================================================================================================================
//printf( "============================================\n" );
//printf( "PS SELECT[Cass=%d] [MID=%s] [%d] [mode=%d] [Select=%d]\n" , midstring , SCHMULTI_CURRENT_MANUAL_MODE , mode , index );
//printf( "============================================\n" );
	//=========================================================================================================================
	if ( mode == 1 ) {
		//
		Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE; // 2009.07.15
		Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE; // 2009.07.15
		//
		LeaveCriticalSection( &CR_MULTIJOB ); // 2009.06.09
		return ( hf * 100 ) + ff; // 2009.05.11 // 2013.02.08
	}
	//
	for ( j = index ; j < *Sch_Multi_ControlJob_Size - 1; j++ ) {
		//
		memcpy( Sch_Multi_ControlJob[j] , Sch_Multi_ControlJob[j+1] , sizeof(Scheduling_Multi_ControlJob) );
		//
		memcpy( &(Sch_Multi_Ins_ControlJob[j]) , &(Sch_Multi_Ins_ControlJob[j+1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ControlJob_Ext[j] , Sch_Multi_ControlJob_Ext[j+1] , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
	}
	SCHMULTI_CONTROLJOB_SET_STATUS( j , "" , CTLJOB_STS_NOTSTATE );
	(*Sch_Multi_ControlJob_Size)--;
	//----------------------------------------------------------------------
	SCHMULTI_CTJOB_IO_SET();
	//----------------------------------------------------------------------
	if ( SCHMULTI_CONTROLJOB_MAKE_RUNJOB( ff ) != 0 ) {
		//-----------------------------------------------------------------------
		strcpy( Sch_Multi_ControlJob_Select_Data[ff]->JobID , "" ); // 2006.03.08
		//-----------------------------------------------------------------------
		Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_NOTSTATE;
		Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
		LeaveCriticalSection( &CR_MULTIJOB );
		//-----------------------------------------------------------------------
		if ( *prepossiblecheck == 2 ) *prepossiblecheck = 1; // 2006.06.28
		//-----------------------------------------------------------------------
		return -31;
	}
	Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_SELECTED;
	Sch_Multi_ControlJob_Selected_Stop_Signal[ff] = FALSE;
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_SELECT , 0 , Sch_Multi_ControlJob_Select_Data[ff]->JobID , -1 , -1 );
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return ( hf * 100 ) + ff; // 2013.02.08
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_SELECT( BOOL *ManualMode ) {
	int i = 0;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	if ( *Sch_Multi_ControlJob_Size <= 0 ) {
		if ( Sch_Multi_ControlJob_Select_Data[0]->ControlStatus == CTLJOB_STS_SELECTED ) {
			*ManualMode = TRUE;
			LeaveCriticalSection( &CR_MULTIJOB );
			return 0;
		}
	}
	*ManualMode = FALSE;
	if ( *Sch_Multi_ControlJob_Size <= 0 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCH_ERROR_QUEUE_EMPTY;
	}
	//
	if ( Sch_Multi_ControlJob_Select_Data[0]->ControlStatus != CTLJOB_STS_NOTSTATE ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCH_ERROR_JOB_SELECT_ALREADY;
	}
	//
	if ( Sch_Multi_Ins_ControlJob[i].Queued_to_Select_Wait ) { // 2012.09.26
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	//
	memcpy( Sch_Multi_ControlJob_Select_Data[0] , Sch_Multi_ControlJob[i] , sizeof(Scheduling_Multi_ControlJob) );
	memcpy( &(Sch_Multi_Ins_ControlJob_Select_Data[0]) , &(Sch_Multi_Ins_ControlJob[i]) , sizeof(Scheduling_Multi_Ins_ControlJob) );
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ControlJob_Ext_Select_Data[0] , Sch_Multi_ControlJob_Ext[i] , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
	for ( ; i < *Sch_Multi_ControlJob_Size - 1; i++ ) {	
		//
		memcpy( Sch_Multi_ControlJob[i] , Sch_Multi_ControlJob[i+1] , sizeof(Scheduling_Multi_ControlJob) );
		memcpy( &(Sch_Multi_Ins_ControlJob[i]) , &(Sch_Multi_Ins_ControlJob[i+1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ControlJob_Ext[i] , Sch_Multi_ControlJob_Ext[i+1] , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
	}
	SCHMULTI_CONTROLJOB_SET_STATUS( i , "" , CTLJOB_STS_NOTSTATE );
	(*Sch_Multi_ControlJob_Size)--;
	//----------------------------------------------------------------------
	SCHMULTI_CTJOB_IO_SET();
	//----------------------------------------------------------------------
	if ( SCHMULTI_CONTROLJOB_MAKE_RUNJOB( -1 ) != 0 ) {
		Sch_Multi_ControlJob_Select_Data[0]->ControlStatus = CTLJOB_STS_NOTSTATE;
		Sch_Multi_ControlJob_Selected_Stop_Signal[0] = FALSE; // 2002.05.20
		SCHMULTI_PROCESSJOB_SET_STATUS( -99 , "" , PRJOB_STS_NOTSTATE );
		LeaveCriticalSection( &CR_MULTIJOB );
		return SCH_ERROR_SELECT_DATA_ERROR;
	}
	Sch_Multi_ControlJob_Select_Data[0]->ControlStatus = CTLJOB_STS_SELECTED;
	Sch_Multi_ControlJob_Selected_Stop_Signal[0] = FALSE; // 2002.05.20
	LeaveCriticalSection( &CR_MULTIJOB );
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_CHGSELECT() {
	int i;
	Scheduling_Multi_ControlJob SchBuf;
	Scheduling_Multi_Ins_ControlJob SchBuf2;
	Scheduling_Multi_CrJob_Ext SchBuf3; // 2015.12.01
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 4; // 2015.03.27
	//
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( *Sch_Multi_ControlJob_Size <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_EMPTY;
	}
	if ( Sch_Multi_ControlJob_Select_Data[0]->ControlStatus != CTLJOB_STS_SELECTED ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_JOB_NOT_SELECTED;
	}
	//
	if ( !STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[0]->JobID ) ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_CANNOT_FIND_CTJOB;
	}
	//
	if ( Sch_Multi_Ins_ControlJob[0].Queued_to_Select_Wait ) { // 2012.09.26
		//
		Sch_Multi_CJJOB_Buf_Sub_Error = 14; // 2015.03.27
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_CANNOT_FIND_CTJOB;
	}
	//
	memcpy( &SchBuf                           , Sch_Multi_ControlJob[0] , sizeof(Scheduling_Multi_ControlJob) );
	memcpy( &SchBuf2                          , &(Sch_Multi_Ins_ControlJob[0]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( &SchBuf3                                 , Sch_Multi_ControlJob_Ext[0]             , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
	//
	memcpy( Sch_Multi_ControlJob[0]           , Sch_Multi_ControlJob_Select_Data[0] , sizeof(Scheduling_Multi_ControlJob) );
	memcpy( &(Sch_Multi_Ins_ControlJob[0])    , &(Sch_Multi_Ins_ControlJob_Select_Data[0]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ControlJob_Ext[0]              , Sch_Multi_ControlJob_Ext_Select_Data[0] , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
	Sch_Multi_ControlJob[0]->ControlStatus = CTLJOB_STS_QUEUED;
	//
	memcpy( Sch_Multi_ControlJob_Select_Data[0]  , &SchBuf , sizeof(Scheduling_Multi_ControlJob) );
	memcpy( &(Sch_Multi_Ins_ControlJob_Select_Data[0])  , &SchBuf2 , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ControlJob_Ext_Select_Data[0]  , &SchBuf3                                , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
	//----------------------------------------------------------------------
	for ( i = 1 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) { // 2003.01.09
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus != CTLJOB_STS_NOTSTATE ) { // 2003.01.09
			break; // 2003.01.09
		} // 2003.01.09
	} // 2003.01.09
	if ( i == MAX_MULTI_CTJOBSELECT_SIZE ) {
		//SCHMULTI_PROCESSJOB_SET_STATUS( -99 , "" , PRJOB_STS_QUEUED ); // 2003.01.09 Move below api inside
		if ( SCHMULTI_CONTROLJOB_MAKE_RUNJOB( -2 ) != 0 ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 14; // 2015.03.27
			//
			Sch_Multi_ControlJob_Select_Data[0]->ControlStatus = CTLJOB_STS_NOTSTATE;
			Sch_Multi_ControlJob_Selected_Stop_Signal[0] = FALSE; // 2002.05.20
			SCHMULTI_PROCESSJOB_SET_STATUS( -99 , "" , PRJOB_STS_QUEUED );
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_SELECT_DATA_ERROR , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			//
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			//
			return SCH_ERROR_SELECT_DATA_ERROR;
		}
	}
	else { // 2003.01.09
		if ( SCHMULTI_CONTROLJOB_MAKE_RUNJOB( -3 ) != 0 ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 24; // 2015.03.27
			//
			Sch_Multi_ControlJob_Select_Data[0]->ControlStatus = CTLJOB_STS_NOTSTATE;
			Sch_Multi_ControlJob_Selected_Stop_Signal[0] = FALSE;
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_CHGSELECT , SCH_ERROR_SELECT_DATA_ERROR , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			//
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			//
			return SCH_ERROR_SELECT_DATA_ERROR;
		}
	}
	//----------------------------------------------------------------------
	Sch_Multi_ControlJob_Select_Data[0]->ControlStatus = CTLJOB_STS_SELECTED;
	Sch_Multi_ControlJob_Selected_Stop_Signal[0] = FALSE; // 2002.05.20
	//==================================================================
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_CHGSELECT , 0 , Sch_Multi_ControlJob[0]->JobID , -1 , -1 );
	//
	SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_SELECT , 0 , Sch_Multi_ControlJob_Select_Data[0]->JobID , -1 , -1 );
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_DELSELECT( int User , int index , int dt , BOOL waitfor ) {
	int m , i , s , e , ffer , ffok , dmr , dma , wfrindex;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 3; // 2015.03.27
	//
	if      ( dt == 0 ) {
		dmr = ENUM_REJECT_CONTROLJOB_DELSELECT;
		dma = ENUM_ACCEPT_CONTROLJOB_DELSELECT;
		wfrindex = 0; // 2006.05.04
	}
	else if ( dt == 1 ) {
		dmr = ENUM_REJECT_CONTROLJOB_STOP;
		dma = ENUM_ACCEPT_CONTROLJOB_STOP;
		wfrindex = 3; // 2006.05.04
	}
	else if ( dt == 2 ) {
		dmr = ENUM_REJECT_CONTROLJOB_ABORT;
		dma = ENUM_ACCEPT_CONTROLJOB_ABORT;
		wfrindex = 1; // 2006.05.04
	}
	if ( User == 0 ) {
		if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( dmr , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_NOT_GOOD_DATA;
		}
		if ( index < 0 ) {
			s = 0;
			e = MAX_MULTI_CTJOBSELECT_SIZE;
		}
		else {
			s = index;
			e = index + 1;
		}
		ffer = 0;
		ffok = 0;
		for ( m = s ; m < e ; m++ ) {
			if (
				( Sch_Multi_ControlJob_Select_Data[m]->ControlStatus != CTLJOB_STS_SELECTED ) && 
				( Sch_Multi_ControlJob_Select_Data[m]->ControlStatus != CTLJOB_STS_WAITINGFORSTART )
				) {
				ffer++;
			}
			else {
				ffok++;
				//--------------------------------------------------------------------------------------------------------
				// 2006.05.04
				//--------------------------------------------------------------------------------------------------------
				if ( dt != 0 ) {
					SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( dma , 0 , Sch_Multi_ControlJob_Select_Data[m]->JobID , -1 , -1 );
				}
				//--------------------------------------------------------------------------------------------------------
				for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
					if ( Sch_Multi_ControlJob_Select_Data[m]->CtrlSpec_Use[i] ) {
						SCHMULTI_PROCESSJOB_Reference( TRUE , Sch_Multi_ControlJob_Select_Data[m]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , Sch_Multi_ControlJob_Select_Data[m]->PRJobDelete ); // 2002.05.20
					}
				}
				Sch_Multi_ControlJob_Select_Data[m]->ControlStatus = CTLJOB_STS_NOTSTATE;
				Sch_Multi_ControlJob_Selected_Stop_Signal[m] = FALSE; // 2002.05.20
				//
				for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
					if ( MULTI_RUNJOB_SelectCJIndex(i) == m ) {
						MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
					}
				}
				//--------------------------------------------------------------------------------------------------------
				// 2006.05.04
				//--------------------------------------------------------------------------------------------------------
//				if ( dt != 0 ) { // 2006.05.04
//					SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_DELSELECT , 0 , Sch_Multi_ControlJob_Select_Data[m]->JobID ); // 2006.05.04
//				} // 2006.05.04
				//--------------------------------------------------------------------------------------------------------
				if ( waitfor ) {
					SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_FINISH , wfrindex , Sch_Multi_ControlJob_Select_Data[m]->JobID , -1 , -1 ); // 2006.05.04
				}
				//--------------------------------------------------------------------------------------------------------
				SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_DELSELECT , 0 , Sch_Multi_ControlJob_Select_Data[m]->JobID , -1 , -1 ); // 2006.05.04
				//--------------------------------------------------------------------------------------------------------
			}
		}
		if ( ( index >= 0 ) && ( ffer > 0 ) ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 13; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( dmr , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_JOB_NOT_SELECTED;
		}
		else if ( ( index < 0 ) && ( ffok == 0 ) ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 23; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( dmr , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
			SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_JOB_NOT_SELECTED;
		}
	}
	else {
		if ( index < 0 ) {
			s = 0;
			e = MAX_MULTI_CTJOBSELECT_SIZE;
		}
		else {
			s = index;
			e = index + 1;
		}
		ffer = 0;
		ffok = 0;
		for ( m = s ; m < e ; m++ ) {
			if ( ( User == 1 ) && ( Sch_Multi_ControlJob_Select_Data[m]->ControlStatus < CTLJOB_STS_EXECUTING ) ) {
				ffer++;
			}
			else if ( ( User == 2 ) && ( Sch_Multi_ControlJob_Select_Data[m]->ControlStatus != CTLJOB_STS_SELECTED ) ) {
				ffer++;
			}
			else {
				ffok++;
				//
				for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
					if ( Sch_Multi_ControlJob_Select_Data[m]->CtrlSpec_Use[i] ) {
						SCHMULTI_PROCESSJOB_Reference( FALSE , Sch_Multi_ControlJob_Select_Data[m]->CtrlSpec_ProcessJobID[i] , -1 , TRUE , Sch_Multi_ControlJob_Select_Data[m]->PRJobDelete ); // 2002.05.20
					}
				}
				Sch_Multi_ControlJob_Select_Data[m]->ControlStatus = CTLJOB_STS_NOTSTATE;
				Sch_Multi_ControlJob_Selected_Stop_Signal[m] = FALSE; // 2002.05.20
				//
				for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
					if ( MULTI_RUNJOB_SelectCJIndex(i) == m ) {
						MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_READY;
					}
				}
				//
				Sleep(250); // 2008.03.28
				//
				SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( dma , 0 , Sch_Multi_ControlJob_Select_Data[m]->JobID , -1 , -1 );
				//
				if ( dt != 0 ) {
					SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_DELSELECT , 0 , Sch_Multi_ControlJob_Select_Data[m]->JobID , -1 , -1 );
				}
			}
		}
		if ( ( index >= 0 ) && ( ffer > 0 ) ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 33; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( dmr , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_JOB_NOT_SELECTED;
		}
		else if ( ( index < 0 ) && ( ffok == 0 ) ) {
			//
			Sch_Multi_CJJOB_Buf_Sub_Error = 43; // 2015.03.27
			//
//			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			SCHMULTI_MESSAGE_CONTROLJOB_REJECT( dmr , SCH_ERROR_JOB_NOT_SELECTED , -1 , -1 );
			LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
			return SCH_ERROR_JOB_NOT_SELECTED;
		}
	}
	if ( index < 0 ) {
		SCHMULTI_PROCESSJOB_SET_STATUS( -99 , "" , PRJOB_STS_QUEUED );
	}
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	if ( User == 0 ) SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return 0;
}
//---------------------------------------------------------------------------------------
int  SCHMULTI_CONTROLJOB_HEADOFQ() {
	int i;
	Scheduling_Multi_ControlJob SchBuf;
	Scheduling_Multi_Ins_ControlJob SchBuf2;
	Scheduling_Multi_CrJob_Ext SchBuf3; // 2015.12.01
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 3; // 2015.03.27
	//
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_HEADOFQ , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	if ( *Sch_Multi_ControlJob_Size <= 0 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_HEADOFQ , SCH_ERROR_QUEUE_EMPTY , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_EMPTY;
	}
	for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ControlJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_HEADOFQ , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	//
	memcpy( &SchBuf , Sch_Multi_ControlJob[i] , sizeof(Scheduling_Multi_ControlJob) );
	memcpy( &SchBuf2 , &(Sch_Multi_Ins_ControlJob[i]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( &SchBuf3 , Sch_Multi_ControlJob_Ext[i] , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
	for ( ; i > 0 ; i-- ) {	
		//
		memcpy( Sch_Multi_ControlJob[i] , Sch_Multi_ControlJob[i-1] , sizeof(Scheduling_Multi_ControlJob) );
		//
		memcpy( &(Sch_Multi_Ins_ControlJob[i]) , &(Sch_Multi_Ins_ControlJob[i-1]) , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
			memcpy( Sch_Multi_ControlJob_Ext[i] , Sch_Multi_ControlJob_Ext[i-1] , sizeof(Scheduling_Multi_CrJob_Ext) );
		}
		//
	}
	//
	memcpy( Sch_Multi_ControlJob[0] , &SchBuf , sizeof(Scheduling_Multi_ControlJob) );
	memcpy( &(Sch_Multi_Ins_ControlJob[0]) , &SchBuf2 , sizeof(Scheduling_Multi_Ins_ControlJob) ); // 2012.04.19
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() > 0 ) { // 2015.12.01
		memcpy( Sch_Multi_ControlJob_Ext[0] , &SchBuf3 , sizeof(Scheduling_Multi_CrJob_Ext) );
	}
	//
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_HEADOFQ , 0 , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 );
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	return 0;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_CONTROLJOB_MANUAL_POSSIBLE() {
	int i;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
		if ( ( ( MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ) - *Sch_Multi_ProcessJob_Size ) <= 0 ) {
			LeaveCriticalSection( &CR_MULTIJOB );
			return FALSE;
		}
	}
	else {
		if ( ( MAX_MULTI_QUEUE_SIZE - *Sch_Multi_ProcessJob_Size ) <= 0 ) {
			LeaveCriticalSection( &CR_MULTIJOB );
			return FALSE;
		}
	}
	//
	if ( *Sch_Multi_ControlJob_Size > 0 ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return FALSE;
	}
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_SELECTED ) {
			if ( !SCHMULTI_CURRENT_MANUAL_MODE ) {
				LeaveCriticalSection( &CR_MULTIJOB );
				return FALSE;
			}
		}
	}
	LeaveCriticalSection( &CR_MULTIJOB );
	return TRUE;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_CONTROLJOB_Queued_to_Select_Wait( BOOL Reset ) { // 2012.09.26
	int i , c;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	c = 0;
	//
	for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
		//
		if ( !Sch_Multi_Ins_ControlJob[i].Queued_to_Select_Wait ) continue;
		//
		c++;
		if ( Reset ) Sch_Multi_Ins_ControlJob[i].Queued_to_Select_Wait = FALSE;
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	return c;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_PROCESSJOB_MAKE_PAUSE( int id , char *data ) {
	int i , j;
	int k = 0;
	BOOL CK = TRUE;
	BOOL RES;
	int p , pjdata[MAX_MULTI_RUNJOB_SIZE]; // 2004.06.02
	int pjdatacount = 0; // 2004.06.02

	if ( id != 0 ) CK = FALSE;
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( !CK ) {
			if ( !STRCMP_L( data , MULTI_RUNJOB_MtlCJName( i ) ) ) continue;
		}
		//
		if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2004.06.21 
			//============================================================================
			RES = TRUE;
			if ( CK ) {
				if ( !STRCMP_L( data , MULTI_RUNJOB_MtlPJName( i )[j] ) ) {
					RES = FALSE;
				}
			}
			if ( RES ) {
				switch( MULTI_RUNJOB_MtlPJ_StartMode(i)[j] ) {
				case PJ_SIGNAL_RUN :
					k = 1;
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_PAUSE_RUN;
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				case PJ_SIGNAL_WAIT :
					k = 1;
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_PAUSE_WAIT;
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				case PJ_SIGNAL_FINISH :
					k = 1;
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_PAUSE_FINISH;
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				case PJ_SIGNAL_PAUSE_RUN :
				case PJ_SIGNAL_PAUSE_WAIT :
				case PJ_SIGNAL_PAUSE_FINISH :
					if ( k != 1 ) k = 2;
					break;
				}
			}
		}
	}
	//=================================================================================================
	// 2004.06.02
	//=================================================================================================
	if ( !CK ) {
		for ( p = 0 ; p < pjdatacount ; p++ ) {
			j = SCHMULTI_PROCESSJOB_GET_STATUS( pjdata[p] , "" ); // 2004.10.14
			if ( ( j >= PRJOB_STS_PROCESSING ) && ( j != PRJOB_STS_PROCESSCOMPLETED ) && ( j != PRJOB_STS_PAUSING ) && ( j != PRJOB_STS_PAUSE ) ) { // 2004.10.14
				SCHMULTI_PROCESSJOB_SET_STATUS( pjdata[p] , "" , PRJOB_STS_PAUSING );
				SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_PAUSING , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[pjdata[p]]->JobID , -1 , -1 );
			}
		}
	}
	//=================================================================================================
	return k;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_PROCESSJOB_MAKE_RESUME( int id , char *data ) {
	int i , j;
	int k = 0;
	BOOL CK = TRUE;
	BOOL UPDATE;
	BOOL RES;
	int p , pjdata[MAX_MULTI_RUNJOB_SIZE]; // 2004.06.02
	int pjdatacount = 0; // 2004.06.02

	if ( id != 0 ) CK = FALSE;
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( !CK ) {
			if ( !STRCMP_L( data , MULTI_RUNJOB_MtlCJName( i ) ) ) continue;
		}
		//
		if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2004.06.21 
			//============================================================================
			RES = TRUE;
			UPDATE = TRUE;
			if ( CK ) {
				if ( !STRCMP_L( data , MULTI_RUNJOB_MtlPJName( i )[j] ) ) {
					RES = FALSE;
				}
				else {
					if ( Sch_Multi_ControlJob_Select_Data[MULTI_RUNJOB_SelectCJIndex(i)]->ControlStatus == CTLJOB_STS_PAUSED ) {
						UPDATE = FALSE;
					}
				}
			}
			if ( RES ) {
				switch( MULTI_RUNJOB_MtlPJ_StartMode(i)[j] ) {
				case PJ_SIGNAL_RUN :
				case PJ_SIGNAL_WAIT :
				case PJ_SIGNAL_FINISH :
					if ( k != 1 ) k = 2;
					break;
				case PJ_SIGNAL_PAUSE_RUN :
					k = 1;
					if ( UPDATE ) MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_RUN;
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				case PJ_SIGNAL_PAUSE_WAIT :
					k = 1;
					if ( UPDATE ) MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_WAIT;
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				case PJ_SIGNAL_PAUSE_FINISH :
					k = 1;
					if ( UPDATE ) MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_FINISH;
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				}
			}
		}
	}
	//=================================================================================================
	// 2004.06.02
	//=================================================================================================
	if ( !CK ) {
		for ( p = 0 ; p < pjdatacount ; p++ ) {
			j = SCHMULTI_PROCESSJOB_GET_STATUS( pjdata[p] , "" ); // 2004.10.14
			if ( ( j > PRJOB_STS_PROCESSING ) && ( j != PRJOB_STS_PROCESSCOMPLETED ) && ( j != PRJOB_STS_EXECUTING ) ) { // 2004.10.14
				SCHMULTI_PROCESSJOB_SET_STATUS( pjdata[p] , "" , PRJOB_STS_EXECUTING );
				SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_RESUME , *Sch_Multi_ProcessJob_Size , Sch_Multi_ProcessJob[pjdata[p]]->JobID , -1 , -1 );
			}
		}
	}
	//=================================================================================================
	return k;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_PROCESSJOB_MAKE_STOP( int id , char *data ) {
	int i , j;
	int k = 0;
	BOOL CK = TRUE;
	BOOL RES;
	int p , pjdata[MAX_MULTI_RUNJOB_SIZE]; // 2004.06.02
	int pjdata2[MAX_MULTI_RUNJOB_SIZE]; // 2004.06.02
	int pjdatacount = 0; // 2004.06.02

	if ( id != 0 ) CK = FALSE;
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( !CK ) {
			if ( !STRCMP_L( data , MULTI_RUNJOB_MtlCJName( i ) ) ) continue;
		}
		//
		if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2004.06.21 
			//============================================================================
			RES = TRUE;
			if ( CK ) {
				if ( !STRCMP_L( data , MULTI_RUNJOB_MtlPJName( i )[j] ) ) {
					RES = FALSE;
				}
			}
			if ( RES ) {
				//=================================================================================================
				// 2004.06.02
				//=================================================================================================
				switch( MULTI_RUNJOB_MtlPJ_StartMode(i)[j] ) {
				case PJ_SIGNAL_PAUSE_RUN :
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_RUN;
					break;
				case PJ_SIGNAL_PAUSE_WAIT :
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_WAIT;
					break;
				case PJ_SIGNAL_PAUSE_FINISH :
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_FINISH;
					break;
				}
				//=================================================================================================
				switch( MULTI_RUNJOB_MtlPJIDEnd(i)[j] ) {
				case 0 :
					k = 1;
					//=================================================================================================
					MULTI_RUNJOB_MtlPJIDEnd(i)[j] = 1;
					//=================================================================================================
					SCHEDULER_CONTROL_Set_GLOBAL_STOP_FOR_JOB( i , j , 1 ); // 2007.11.08 // 2012.04.01 // 2013.11.28 // 2014.05.21
					//=================================================================================================
					//=================================================================================================
					// 2004.06.02
					//=================================================================================================
					if ( !CK ) {
						for ( p = 0 ; p < pjdatacount ; p++ ) {
							if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
						}
						if ( p == pjdatacount ) {
							pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
							if      ( Sch_Multi_ProcessJob[pjdata[pjdatacount]]->ControlStatus == PRJOB_STS_PAUSING )
								pjdata2[pjdatacount] = 1;
							else if ( Sch_Multi_ProcessJob[pjdata[pjdatacount]]->ControlStatus == PRJOB_STS_PAUSE )
								pjdata2[pjdatacount] = 1;
							else
								pjdata2[pjdatacount] = 0;
							pjdatacount++;
						}
					}
					//=================================================================================================
					break;
				case 1 :
				case 2 :
					if ( k != 1 ) k = 2;
					break;
				}
			}
		}
	}
	//=================================================================================================
	// 2004.06.02
	//=================================================================================================
	if ( !CK ) {
		for ( p = 0 ; p < pjdatacount ; p++ ) {
			j = SCHMULTI_PROCESSJOB_GET_STATUS( pjdata[p] , "" ); // 2004.10.14
			if ( ( j >= PRJOB_STS_PROCESSING ) && ( j != PRJOB_STS_PROCESSCOMPLETED ) && ( j != PRJOB_STS_STOPPING ) ) { // 2004.10.14
				SCHMULTI_PROCESSJOB_SET_STATUS( pjdata[p] , "" , PRJOB_STS_STOPPING );
				SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_STOP , pjdata2[p] , Sch_Multi_ProcessJob[pjdata[p]]->JobID , -1 , -1 );
			}
		}
	}
	//=================================================================================================
	return k;
}
//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_PROCESSJOB_MAKE_ABORT( int id , char *data ) { // 2004.06.15
	int i , j;
	int k = 0;
	BOOL CK = TRUE;
	BOOL RES;
	int p , pjdata[MAX_MULTI_RUNJOB_SIZE]; // 2004.06.02
	int pjdata2[MAX_MULTI_RUNJOB_SIZE]; // 2004.06.02
	int pjdatacount = 0; // 2004.06.02

	if ( id != 0 ) CK = FALSE;
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( !CK ) {
			if ( !STRCMP_L( data , MULTI_RUNJOB_MtlCJName( i ) ) ) continue;
		}
		//
		if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2004.06.21 
			//============================================================================
			RES = TRUE;
			if ( CK ) {
				if ( !STRCMP_L( data , MULTI_RUNJOB_MtlPJName( i )[j] ) ) {
					RES = FALSE;
				}
			}
			if ( RES ) {
				switch( MULTI_RUNJOB_MtlPJIDEnd(i)[j] ) {
				case 0 :
					k = 1;
					MULTI_RUNJOB_MtlPJIDEnd(i)[j] = 1;
					break;
				case 1 :
				case 2 :
					if ( k != 1 ) k = 2;
					break;
				}
				//=================================================================================================
				// 2004.06.15
				//=================================================================================================
				for ( p = 0 ; p < pjdatacount ; p++ ) {
					if ( MULTI_RUNJOB_MtlPJID(i)[j] == pjdata[p] ) break;
				}
				if ( p == pjdatacount ) {
					pjdata[pjdatacount] = MULTI_RUNJOB_MtlPJID(i)[j];
					if      ( Sch_Multi_ProcessJob[pjdata[pjdatacount]]->ControlStatus == PRJOB_STS_STOPPING )
						pjdata2[pjdatacount] = 2;
					else if ( Sch_Multi_ProcessJob[pjdata[pjdatacount]]->ControlStatus == PRJOB_STS_PAUSING )
						pjdata2[pjdatacount] = 1;
					else if ( Sch_Multi_ProcessJob[pjdata[pjdatacount]]->ControlStatus == PRJOB_STS_PAUSE )
						pjdata2[pjdatacount] = 1;
					else
						pjdata2[pjdatacount] = 0;
					pjdatacount++;
				}
				//=================================================================================================
			}
		}
	}
	//=================================================================================================
	// 2004.06.02
	//=================================================================================================
	for ( p = 0 ; p < pjdatacount ; p++ ) {
		j = SCHMULTI_PROCESSJOB_GET_STATUS( pjdata[p] , "" ); // 2004.10.14
//		if ( ( j >= PRJOB_STS_PROCESSING ) && ( j != PRJOB_STS_PROCESSCOMPLETED ) && ( j != PRJOB_STS_ABORTING ) ) { // 2004.10.14 // 2005.06.10
		if ( ( j >= PRJOB_STS_PROCESSING ) && ( j != PRJOB_STS_PROCESSCOMPLETED ) ) { // 2004.10.14 // 2005.06.10
			SCHMULTI_PROCESSJOB_SET_STATUS( pjdata[p] , "" , PRJOB_STS_ABORTING );
			SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_ABORT , pjdata2[p] , Sch_Multi_ProcessJob[pjdata[p]]->JobID , -1 , -1 );
		}
	}
	//=================================================================================================
	return k;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_SET_PROCESSJOB_POSSIBLE() {
	int i , j;
	BOOL k = FALSE;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_PRJOB_Buf_Sub_Error = 9; // 2015.03.27
	//
	if ( !SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_VERIFY , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}
	for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
		if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob[i]->JobID ) ) break;
	}
	if ( i == *Sch_Multi_ProcessJob_Size ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_VERIFY , SCH_ERROR_QUEUE_CANNOT_FIND , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_CANNOT_FIND;
	}
	if ( Sch_Multi_ProcessJob[i]->ControlStatus != PRJOB_STS_WAITINGFORSTART ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_VERIFY , SCH_ERROR_QUEUE_DATA_NOT_USING , -1 , -1 );
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR();
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_QUEUE_DATA_NOT_USING;
	}
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2004.06.21 
			//============================================================================
			if ( STRCMP_L( Sch_Multi_ProcessJob_Buffer->JobID , MULTI_RUNJOB_MtlPJName( i )[j] ) ) {
				switch( MULTI_RUNJOB_MtlPJ_StartMode(i)[j] ) {
				case PJ_SIGNAL_RUN :
				case PJ_SIGNAL_WAIT :
				case PJ_SIGNAL_FINISH :
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_RUN;
					break;
				case PJ_SIGNAL_PAUSE_RUN :
				case PJ_SIGNAL_PAUSE_WAIT :
				case PJ_SIGNAL_PAUSE_FINISH :
					MULTI_RUNJOB_MtlPJ_StartMode(i)[j] = PJ_SIGNAL_PAUSE_RUN;
					break;
				}
				k = TRUE;
			}
		}
	}
	//
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
//	SCHMULTI_PROCESSJOB_BUFFER_CLEAR(); // 2013.02.13
	if ( k ) {
		//
		SCHMULTI_PROCESSJOB_SET_STATUS( -1 , Sch_Multi_ProcessJob_Buffer->JobID , PRJOB_STS_PROCESSING );
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_PROCESSJOB_BUFFER_CLEAR(); // 2013.02.13
		//
		SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( ENUM_ACCEPT_PROCESSJOB_VERIFY , 0 , Sch_Multi_ProcessJob_Buffer->JobID , -1 , -1 );
		//
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		//
		return 0;
	}
	//
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_PROCESSJOB_BUFFER_CLEAR(); // 2013.02.13
	//
	SCHMULTI_MESSAGE_PROCESSJOB_REJECT( ENUM_REJECT_PROCESSJOB_VERIFY , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return SCH_ERROR_CANNOT_FIND_CTJOB;
}
//
int SCHMULTI_RUNJOB_SET_CONTROLJOB_POSSIBLE() {
	int i , ff;
	BOOL k = FALSE;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	Sch_Multi_CJJOB_Buf_Sub_Error = 9; // 2015.03.27
	//
	if ( !SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_VERIFY , SCH_ERROR_NOT_GOOD_DATA , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_NOT_GOOD_DATA;
	}

	ff = -1;
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus == CTLJOB_STS_WAITINGFORSTART ) {
			if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Select_Data[i]->JobID ) ) {
				ff = i;
				break;
			}
		}
	}
	if ( ff == -1 ) {
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
		SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_VERIFY , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		return SCH_ERROR_CANNOT_FIND_CTJOB;
	}

	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_WAITING ) {
			if ( STRCMP_L( Sch_Multi_ControlJob_Buffer->JobID , MULTI_RUNJOB_MtlCJName( i ) ) ) {
				MULTI_RUNJOB_MtlCJ_StartMode( i ) = 0;
				k = TRUE;
			}
		}
	}
	if ( k ) {
		Sch_Multi_ControlJob_Select_Data[ff]->ControlStatus = CTLJOB_STS_EXECUTING;
		SCHMULTI_CONTROLJOB_BUFFER_CLEAR(); // 2004.05.11
		SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_VERIFY , 0 , Sch_Multi_ControlJob_Buffer->JobID , -1 , -1 ); // 2004.05.11
		LeaveCriticalSection( &CR_MULTIJOB );
//		SCHMULTI_CONTROLJOB_BUFFER_CLEAR(); // 2004.05.11
//		SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( ENUM_ACCEPT_CONTROLJOB_VERIFY , 0 , Sch_Multi_ControlJob_Buffer->JobID ); // 2004.05.11
		return 0;
	}
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	SCHMULTI_CONTROLJOB_BUFFER_CLEAR();
	SCHMULTI_MESSAGE_CONTROLJOB_REJECT( ENUM_REJECT_CONTROLJOB_VERIFY , SCH_ERROR_CANNOT_FIND_CTJOB , -1 , -1 );
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
	return SCH_ERROR_CANNOT_FIND_CTJOB;
}
//---------------------------------------------------------------------------------------
int _i_SCH_MULTIJOB_SET_CJPJJOB_RESULT( int side , int pt , int mode , int Result ) { // 2018.11.14
	//
	// mode == 0	:	CJ/PJ
	// mode == 1	:	PJ
	// mode == 2	:	CJ

	int c , p , x;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return 1;
	//
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 2;
	//
	if ( Result < 0 ) return 3;
	//
	if ( _i_SCH_SYSTEM_CPJOB_GET( side ) == 0 ) return 4;
	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pt );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pt );
	//
	if ( ( c < 0 ) || ( p < 0 ) ) return 5;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	if ( ( mode == 0 ) || ( mode == 2 ) ) { // CJ
		//
		Sch_Multi_Ins_ControlJob_Select_Data[MULTI_RUNJOB_SelectCJIndex(c)].FixedResult = Result;
		//
	}
	if ( ( mode == 0 ) || ( mode == 1 ) ) { // PJ
		//
		x = SCHMULTI_PROCESSJOB_Find( MULTI_RUNJOB_MtlPJName( c )[p] );
		//
		if ( x >= 0 ) {
			//
			Sch_Multi_Ins_ProcessJob[x].FixedResult = Result;
			//
		}
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	//
	return 0;
	//
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( int side , int pt , int mode ) {
	// mode == 0	:	Wait Not Check
	// mode == 1	:	Wait Check & Mesage Check(Switch in BM)
	// mode == 2	:	Wait Check & Mesage Not Check

//	int i , j , k , x , index , slot; // 2011.04.21
	int c , p , j , k , x; // 2011.04.21
	//
	int rs , rp; // 2018.09.06
	//
//	EnterCriticalSection( &CR_MULTIJOB );
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 2;
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return 2;
	if ( _i_SCH_SYSTEM_CPJOB_GET( side ) == 0 ) return 2;
	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pt ); // 2011.04.21
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pt ); // 2011.04.21
	if ( ( c < 0 ) || ( p < 0 ) ) return 3;

	//==========================================================================
	if ( mode == 101 ) { // cj 같은것 2018.08.24
		//
		x = 1;
		//
		EnterCriticalSection( &CR_MULTIJOB );
		//
		for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
			//
			if ( c == j ) continue;
			//
			//---------------------------------------
			//
			if ( MULTI_RUNJOB_SelectCJIndex(j) != MULTI_RUNJOB_SelectCJIndex( c ) ) continue;
			if ( MULTI_RUNJOB_SelectCJOrder(j) == MULTI_RUNJOB_SelectCJOrder( c ) ) continue;
			//
			//---------------------------------------
			//
			if ( MULTI_RUNJOB_RunStatus(j) == CASSRUNJOB_STS_WAITING ) {
				x = 0;
				break;
			}
			else if ( MULTI_RUNJOB_RunStatus(j) == CASSRUNJOB_STS_RUNNING ) {
				//
				for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(j) ; k++ ) {
					//
					if ( MULTI_RUNJOB_MtlPJID(j)[k] < 0 ) continue;
					//
					if ( MULTI_RUNJOB_MtlPJName( j )[k][0] == 0 ) continue;
					//
					if ( MULTI_RUNJOB_MtlPJIDRes(j)[k] != PJIDRES_READY ) break;
					//
					// 2018.09.06
					//
					rs = MULTI_RUNJOB_Mtl_Side( j )[k];
					rp = MULTI_RUNJOB_Mtl_Pointer( j )[k];
					//
//					if ( ( rs < 0 ) || ( rs >= MAX_CASSETTE_SIDE ) ) break; // 2018.10.12
//					if ( ( rp < 0 ) || ( rp >= MAX_CASSETTE_SLOT_SIZE ) ) break; // 2018.10.12
					//
//					if ( _i_SCH_CLUSTER_Get_PathStatus( rs , rp ) != SCHEDULER_READY ) break; // 2018.10.12
					//
					// 2018.10.12
					//
					if ( ( rs >= 0 ) && ( rs < MAX_CASSETTE_SIDE ) ) {
						if ( ( rp >= 0 ) && ( rp < MAX_CASSETTE_SLOT_SIZE ) ) {
							//
							if ( _i_SCH_CLUSTER_Get_PathStatus( rs , rp ) != SCHEDULER_READY ) break; // 2018.10.12
							//
						}
					}
					//
				}
				//
				if ( k == MULTI_RUNJOB_MtlCount(j) ) {
					x = 0;
					break;
				}
				//
			}
			//
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB );
		//
		return x;
		//
	}
	//==========================================================================

// 2011.04.21
	//
	switch( MULTI_RUNJOB_MtlPJ_StartMode(c)[p] ) {
	case PJ_SIGNAL_RUN :
		return 1;
		break;
	case PJ_SIGNAL_WAIT :
		if ( mode == 0 ) { // 2004.05.10
			return 1;
		}
		else {
			if ( mode == 1 ) {
				//
				EnterCriticalSection( &CR_MULTIJOB ); // 2013.02.13
				//
				SCHMULTI_PROCESSJOB_SET_STATUS( -1 , MULTI_RUNJOB_MtlPJName( c )[p] , PRJOB_STS_WAITINGFORSTART );
				//
//				LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13 // 2015.03.27
				//
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_WAITVERIFY , 0 , 0 , MULTI_RUNJOB_MtlPJName( c )[p] , side , pt );
				//
				x = MULTI_RUNJOB_MtlPJID(c)[p];
				for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
					//
					if ( MULTI_RUNJOB_RunStatus(j) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
					if ( MULTI_RUNJOB_RunStatus(j) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
					//
					for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(j) ; k++ ) {
						if ( x == MULTI_RUNJOB_MtlPJID(j)[k] ) {
							MULTI_RUNJOB_MtlPJ_StartMode(j)[k] = PJ_SIGNAL_FINISH;
						}
					}
				}
				//
				LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
				//
			}
			return 0;
		}
		break;
	case PJ_SIGNAL_FINISH :
		return 0;
		break;
	case PJ_SIGNAL_PAUSE_RUN :
	case PJ_SIGNAL_PAUSE_WAIT :
	case PJ_SIGNAL_PAUSE_FINISH :
		//
		EnterCriticalSection( &CR_MULTIJOB ); // 2013.02.13
		//
		if ( SCHMULTI_PROCESSJOB_GET_LAST_STATUS( MULTI_RUNJOB_MtlPJName( c )[p] ) == PRJOB_STS_PAUSING ) {
			SCHMULTI_PROCESSJOB_SET_STATUS( -1 , MULTI_RUNJOB_MtlPJName( c )[p] , PRJOB_STS_PAUSE );
			SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_PAUSE , 0 , 0 , MULTI_RUNJOB_MtlPJName( c )[p] , side , pt );
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
		//
		return 0;
		break;
	}

//	LeaveCriticalSection( &CR_MULTIJOB );
	return 1;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_RUNJOB_GET_CONTROLJOB_PJ_IN_CASSETTE_MODE( int i ) {
	int j , Res;
//	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
	for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
		//
		//============================================================================
		if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue;
		//============================================================================
		if ( MULTI_RUNJOB_MtlPJName( i )[j][0] == 0 ) continue;
		//============================================================================
		Res = SCHMULTI_PROCESSJOB_Find( MULTI_RUNJOB_MtlPJName( i )[j] );
		//
		if ( Res < 0 ) return FALSE;
		//
		if ( Sch_Multi_ProcessJob[Res]->MtlFormat >= 100 ) return TRUE;
		//
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE( int CheckMode , int cass , char *midstring , int *Mode , int *Res , int *carrmode , int *outcassmode , int *movemode ) {
	//
	// CheckMode = 0 : Mid  In/Out + Cass In
	//             1 : Cass In  Only
	//             2 : Mid  Out Only
	//
	int m , i , nfd;

	int j; // 2011.02.15(Testing) // 2011.05.21

	EnterCriticalSection( &CR_MULTIJOB );
	//
	*carrmode = FALSE; // 2008.07.30
	*outcassmode = FALSE; // 2009.04.03
	*movemode = 0; // 2013.09.03
	//
	for ( m = 0 ; m < MAX_MULTI_CTJOBSELECT_SIZE ; m++ ) {
		//
		nfd = 0;
		//
		switch ( Sch_Multi_ControlJob_Select_Data[m]->ControlStatus ) {
		case CTLJOB_STS_NOTSTATE :
		case CTLJOB_STS_QUEUED :
		case CTLJOB_STS_PAUSED :
		case CTLJOB_STS_COMPLETED :
			nfd = 1;
			break;
		}
		//
		if ( nfd == 1 ) continue;
		//
		nfd = -1;
		//
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			if ( MULTI_RUNJOB_SelectCJIndex(i) == m ) {
				if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_WAITING ) {
					if ( nfd == -1 ) {
						nfd = i;
					}
					else {
						if ( MULTI_RUNJOB_SelectCJOrder(i) < MULTI_RUNJOB_SelectCJOrder(nfd) ) {
							nfd = i;
						}
					}
				}
			}
		}
		//
		if ( nfd == -1 ) continue;
		//
		i = nfd;
		//
		if ( SCHMULTI_RUNJOB_GET_CONTROLJOB_PJ_IN_CASSETTE_MODE( i ) ) { // 2011.12.14
			//
			if ( CheckMode != 2 ) { // 2009.04.03
				if ( MULTI_RUNJOB_Cassette(i) == cass ) { // 2008.04.23 // Cass
					*Mode = m;
					*Res = i;
					*carrmode = 1; // 2008.07.30
					*movemode = Sch_Multi_Ins_ControlJob_Select_Data[m].MoveMode; // 2013.09.03
					LeaveCriticalSection( &CR_MULTIJOB );
					return MULTI_RUNJOB_MtlCJ_StartMode( i ) + 1;
				}
				else {
// 2011.05.21
					// 2011.02.15(Testing)
					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.06.02
						//
//						for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
						for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
							//
							//============================================================================
							if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2011.04.28
							//============================================================================
							if ( MULTI_RUNJOB_MtlPJName( i )[j][0] == 0 ) continue; // 2011.03.04
							//
							if ( MULTI_RUNJOB_MtlOutCassette(i)[j] == ( cass + CM1 ) ) { // Cass
								*Mode = m;
								*Res = i;
								*carrmode = 1;
								*outcassmode = TRUE;
								*movemode = Sch_Multi_Ins_ControlJob_Select_Data[m].MoveMode; // 2013.09.03
								LeaveCriticalSection( &CR_MULTIJOB );
								return MULTI_RUNJOB_MtlCJ_StartMode( i ) + 1;
							}
						}
						//
					}
				}
			}
		}
		else {
			if ( CheckMode != 1 ) { // 2009.04.03
				if ( CheckMode == 0 ) { // Mid In
					if ( STRCMP_L( midstring , MULTI_RUNJOB_CarrierID( i ) ) ) {
						*Mode = m;
						*Res = i;
						*carrmode = 0; // 2008.07.30
						*movemode = Sch_Multi_Ins_ControlJob_Select_Data[m].MoveMode; // 2013.09.03
						LeaveCriticalSection( &CR_MULTIJOB );
						return MULTI_RUNJOB_MtlCJ_StartMode( i ) + 1;
					}
				}
				//
// 2011.05.21
				// 2011.02.15
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.06.02
					//
//					for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
					for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
						//
						//============================================================================
						if ( MULTI_RUNJOB_MtlPJID(i)[j] < 0 ) continue; // 2011.04.28
						//============================================================================
						if ( MULTI_RUNJOB_MtlPJName( i )[j][0] == 0 ) continue; // 2011.03.04
						//
						if ( !STRCMP_L( "" , MULTI_RUNJOB_MtlOutCarrierID(i)[j] ) ) {
							if ( STRCMP_L( midstring , MULTI_RUNJOB_MtlOutCarrierID(i)[j] ) ) {
								*Mode = m;
								*Res = i;
								*carrmode = 0;
								*outcassmode = TRUE;
								*movemode = Sch_Multi_Ins_ControlJob_Select_Data[m].MoveMode; // 2013.09.03
								LeaveCriticalSection( &CR_MULTIJOB );
								return MULTI_RUNJOB_MtlCJ_StartMode( i ) + 1;
							}
						}
					}
					//
				}
			}
		}
	}
	LeaveCriticalSection( &CR_MULTIJOB );
	return 0;
}
//
//---------------------------------------------------------------------------------------

// 2011.05.21
// 2011.02.15(Testing)

void SCHMULTI_RUNJOB_CHANGE_OUTCASS_INFO( char *cjname , char *pjname , int slot , char *newmid ) { // 2011.05.19



}

int SCHMULTI_sub_CHECK_SUPPLY_POSSIBLE_FOR_OUTCASS( int manual , int pathout , char *mid , int *seloc , int esrcid ) { // 2012.04.01
	int i , l;
	//
	*seloc = 0;
	//
	for ( l = 0 ; l < 2 ; l++ ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( l == 0 ) {
				if ( i != pathout ) continue;
			}
			else {
				if ( i == pathout ) continue;
			}
			//
			if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( i ) > 0 ) continue;
			if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( i ) > 0 ) continue;
			//
			if ( manual ) {
				if ( BUTTON_GET_FLOW_STATUS_VAR( i ) != _MS_5_MAPPED ) continue;
			}
			//
			if ( !SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( i , manual ? 4 : 2 , ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i ) >= 2 ) ? 0 : 1 , mid , esrcid ) ) continue;
			//
			break;
		}
		//
		*seloc = i;
		//
		if ( i != MAX_CASSETTE_SIDE ) return TRUE;
		//
	}
	//
	return FALSE;
	//
}

int SCHMULTI_sub_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( int index , int pidx , int *seloc ) { // 2011.03.30
	//
	if ( index < 0 ) return 11;
	if ( index >= MAX_MULTI_RUNJOB_SIZE ) return 12;
	if ( pidx < 0 ) return 13;
//	if ( pidx >= MAX_CASSETTE_SLOT_SIZE ) return 14; // 2018.05.10
	if ( pidx >= MAX_RUN_CASSETTE_SLOT_SIZE ) return 14; // 2018.05.10
	//
	//============================================================================
	if ( MULTI_RUNJOB_MtlPJID(index)[pidx] < 0 ) return -1; // 2011.04.28
	//============================================================================
	if ( MULTI_RUNJOB_MtlPJName( index )[pidx][0] == 0 ) return -2;
	//
	if ( MULTI_RUNJOB_MtlOutCassette(index)[pidx] == 0 ) { // 2018.07.17
		//
		return -4;
		//
	}
	else if ( MULTI_RUNJOB_MtlOutCassette(index)[pidx] >= CM1 ) { // 2018.07.17 // STARTJ DIFF CASSETTE
		//
		if ( !SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( MULTI_RUNJOB_MtlOutCassette(index)[pidx] - CM1 , 2 , 0 , NULL , 0 ) ) return -3;
		//
		*seloc = MULTI_RUNJOB_MtlOutCassette(index)[pidx] - CM1;
		//
		return 0;
	}
	else {
		//
		if ( !STRCMP_L( "" , MULTI_RUNJOB_MtlOutCarrierID(index)[pidx] ) ) {
			//
			if ( !STRCMP_L( "-" , MULTI_RUNJOB_MtlOutCarrierID(index)[pidx] ) ) { // 2011.05.19
				//
				if ( !SCHMULTI_sub_CHECK_SUPPLY_POSSIBLE_FOR_OUTCASS( FALSE , -1 , MULTI_RUNJOB_MtlOutCarrierID(index)[pidx] , seloc , 0 ) ) return -11; // 2012.04.01
				//
				return 0;
				//
			}
			else { // 2011.05.19
				return -12;
			}
		}
		//
		return 1;
		//
	}
	//
	return 0;
	//
}


int SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE_FOR_OUTCASS( int cass , int ri , int *outcassmode ) { // 2011.03.08
	int j , seloc , Res , retv;
	BOOL notfixoutcass = FALSE; // 2018.07.17
	//
	*outcassmode = -1;
	//
	seloc = -1; // 2018.09.13 // STARTJ DIFF CASSETTE
	//
//	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
	for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
		//
		//============================================================================
		if ( MULTI_RUNJOB_MtlPJID(ri)[j] < 0 ) continue; // 2011.04.28
		//============================================================================
		if ( MULTI_RUNJOB_MtlPJName( ri )[j][0] == 0 ) continue; // 2011.03.04
		//
		if ( MULTI_RUNJOB_MtlOutCassette(ri)[j] == 0 ) { // 2018.08.16
			notfixoutcass = TRUE;
			break;
		}
		//
		if ( MULTI_RUNJOB_MtlOutCassette(ri)[j] != -1 ) break;
		//
		if ( !STRCMP_L( "" , MULTI_RUNJOB_MtlOutCarrierID(ri)[j] ) ) break;
	}
//	if ( j == MAX_CASSETTE_SLOT_SIZE ) return 1; // 2018.05.10
	if ( j == MAX_RUN_CASSETTE_SLOT_SIZE ) return 1; // 2018.05.10
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2011.05.21
		//
		if ( notfixoutcass ) { // 2018.07.17
			*outcassmode = -2;
		}
		else {
			//
			// 2018.08.28 // STARTJ DIFF CASSETTE
			//
			for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) {
				//
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(ri)[j] < 0 ) continue;
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJName( ri )[j][0] == 0 ) continue;
				//
				if ( 0 == SCHMULTI_sub_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( ri , j , &seloc ) ) {
					//
					*outcassmode = seloc;
					return 11;
					//
				}
			}
			//
			*outcassmode = -1;
			//
		}
		return 11; // 2011.04.14 (Testing)
	}
	else {
		//
		seloc = -1;
		//
		retv = 0;
		//
//		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
		for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
			//
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJID(ri)[j] < 0 ) continue; // 2011.04.28
			//============================================================================
			if ( MULTI_RUNJOB_MtlPJName( ri )[j][0] == 0 ) continue; // 2011.03.04
			//
			Res = SCHMULTI_sub_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( ri , j , &seloc ); // 2011.03.30
			//
			if      ( Res == 1 ) {
				//
				retv = 1; // 2011.03.30 (Testing)
				//
			}
			else if ( Res == 0 ) {
				//
				*outcassmode = seloc; // 2011.03.30 (Testing)
				return 11; // 2011.03.30 (Testing)
				//
			}
			else if ( Res <  0 ) {
				//
				if ( retv == 0 ) return Res; // 2011.03.30 (Testing)
				//
	//			return res; // 2011.03.30 (Testing)
			}
			//
		}
		//
		if ( seloc == -1 ) {
			if ( retv == 1 ) {
				//
				*outcassmode = -1;
				return 12;
				//
			}
			else {
				//
				return -3;
				//
			}
		}
		//
		*outcassmode = seloc;
		//
		return 2;
	}
}

int SCHMULTI_GET_VALID_CASSETTE( int cass , char *midname ) { // 2011.03.08
	int i;
	//
	if ( cass == 0 ) { // 2018.08.16
		return 0;
	}
	else if ( cass >= CM1 ) { // 2018.08.16 // STARTJ DIFF CASSETTE
		return cass;
	}
	else {
		if ( !STRCMP_L( midname , "" ) ) {
			//
			if ( !STRCMP_L( midname , "-" ) ) { // 2011.05.19
				//
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					//
					if ( !SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( i , 3 , 1 , midname , 0 ) ) continue;
					//
					return i + CM1;
					//
				}
				//
				return 0;
				//
			}
			else {
				return 0;
			}
		}
		else {
			return -1;
		}
	}
}


BOOL SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_INCASS( int side , int pointer , int cm ) {
	int j , po;
	BOOL outcm;
	//
	po = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
	//
	outcm = FALSE;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( j != pointer ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
			//
		}
		//
		if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) == po ) {
				//
				_i_SCH_CLUSTER_Set_PathIn( side , j , cm );
				//
				_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _MOTAG_14_NEED_OUT );
			}
			//
			if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) == po ) {
				//
				_i_SCH_CLUSTER_Set_PathOut( side , j , cm );
				//
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_14_NEED_OUT ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
				}
				else {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _MOTAG_13_NEED_IN );
				}
				//
				SCHMRDATA_Data_Setting_for_outcm( cm , side , j , 0 );
				//
				outcm = TRUE;
				//
			}
		}
		else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_13_NEED_IN ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != po ) continue;
			//
			_i_SCH_CLUSTER_Set_PathIn( side , j , cm );
			//
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
			//
		}
		//
		else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_14_NEED_OUT ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) != po ) continue;
			//
			_i_SCH_CLUSTER_Set_PathOut( side , j , cm ); // 2019.02.28
			//
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
			//
			SCHMRDATA_Data_Setting_for_outcm( cm , side , j , 0 );
			//
			outcm = TRUE;
			//
		}
		//
	}
	//
	//
	if ( outcm ) SCHMRDATA_Data_Setting_for_outcm( -1 , -1 , -1 , -1 ); // 2013.06.05
	//
	return TRUE;
}

BOOL SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS( int side , int pointer , int cm ) {
	int j , po;
	BOOL outcm;
	//
	po = _i_SCH_CLUSTER_Get_PathOut( side , pointer );
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( j == pointer ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) != po ) continue;
		//
		if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) != _MOTAG_12_NEED_IN_OUT /* IN+OUT */) { // 2013.07.02
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) != _MOTAG_14_NEED_OUT /* OUT */) continue; // 2013.07.02
		}
		//
		if ( _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , j ) != _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , pointer ) ) continue;
		//
		if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( side ) < 2 ) { // 2013.08.16
			if ( !STRCMP_L( STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , j ) ) , STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , pointer ) ) ) ) continue;
		}
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) != SCHEDULER_READY ) { // 2013.05.27
			if ( _i_SCH_IO_GET_MODULE_STATUS( cm , _i_SCH_CLUSTER_Get_SlotOut( side , j ) ) != CWM_ABSENT ) return FALSE;
		}
		//
	}
	//
	/*
	//
	// 2019.02.28
	//
	SCHMRDATA_Data_Setting_for_outcm( cm , side , pointer , 0 );
	//
//	if ( _i_SCH_CLUSTER_Get_PathOut( side , pointer ) <= 0 ) _i_SCH_CLUSTER_Set_PathOut( side , pointer , cm ); // 2013.07.02 // 2019.02.28
	_i_SCH_CLUSTER_Set_PathOut( side , pointer , cm ); // 2013.07.02 // 2019.02.28
	//
	_i_SCH_CLUSTER_Set_Ex_MtlOut( side , pointer , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() ); // 2013.06.12
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( j == pointer ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) != po ) continue;
		//
		if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) != _MOTAG_12_NEED_IN_OUT ) { // 2013.07.02
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) != _MOTAG_14_NEED_OUT ) continue; // 2013.07.02
		}
		//
		if ( _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , j ) != _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , pointer ) ) continue;
		//
		if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( side ) < 2 ) { // 2013.08.16
			if ( !STRCMP_L( STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , j ) ) , STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , pointer ) ) ) ) continue;
		}
		//
		SCHMRDATA_Data_Setting_for_outcm( cm , side , j , 0 );
		//
//		if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) <= 0 ) _i_SCH_CLUSTER_Set_PathOut( side , j , cm ); // 2019.02.28
		_i_SCH_CLUSTER_Set_PathOut( side , j , cm ); // 2019.02.28
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != _i_SCH_CLUSTER_Get_PathOut( side , j ) ) {
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) {
				if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_READY ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _MOTAG_13_NEED_IN );
				}
				else {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
				}
			}
			else {
				_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
			}
		}
		else {
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
		}
		//
	}
	//
	*/
	//
	//
	// 2019.02.28
	//
	outcm = FALSE;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( j != pointer ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , j ) != _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , pointer ) ) continue;
			//
			if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( side ) < 2 ) { // 2013.08.16
				if ( !STRCMP_L( STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , j ) ) , STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , pointer ) ) ) ) continue;
			}
			//
		}
		//
		if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) == po ) {
				//
				_i_SCH_CLUSTER_Set_PathIn( side , j , cm );
				//
				_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _MOTAG_14_NEED_OUT );
			}
			//
			if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) == po ) {
				//
				_i_SCH_CLUSTER_Set_PathOut( side , j , cm );
				//
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_14_NEED_OUT ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
				}
				else {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _MOTAG_13_NEED_IN );
				}
				//
				SCHMRDATA_Data_Setting_for_outcm( cm , side , j , 0 );
				//
				outcm = TRUE;
				//
			}
		}
		else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_13_NEED_IN ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != po ) continue;
			//
			_i_SCH_CLUSTER_Set_PathIn( side , j , cm );
			//
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
			//
		}
		//
		else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_14_NEED_OUT ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) != po ) continue;
			//
			_i_SCH_CLUSTER_Set_PathOut( side , j , cm ); // 2019.02.28
			//
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 200 + _i_SCH_PRM_GET_MTLOUT_INTERFACE() );
			//
			SCHMRDATA_Data_Setting_for_outcm( cm , side , j , 0 );
			//
			outcm = TRUE;
			//
		}
		//
	}
	//
	//
	if ( outcm ) SCHMRDATA_Data_Setting_for_outcm( -1 , -1 , -1 , -1 ); // 2013.06.05
	//
	return TRUE;
}

BOOL SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS_IN( int side , int pointer ) { // 2013.05.27
	int pi , sl;
	//
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) <= 1 ) return TRUE; // 2016.10.31
	//
	if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , pointer ) == MR_PRE_3_WAIT ) return FALSE; // 2016.11.02
	//
	if ( SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( side , pointer , TRUE , &pi , FALSE ) < 0 ) return FALSE;
	//
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */) return FALSE;
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_13_NEED_IN /* IN */) return FALSE; // 2013.07.02
	//
	pi = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
	sl = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
	if ( _i_SCH_IO_GET_MODULE_STATUS( pi , sl ) != CWM_PRESENT ) return FALSE;
	//
	return TRUE;
}


BOOL _i_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( int side , int pointer ) { // 2013.05.27
	int po , sl , Res , fail;
	//
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) <= 1 ) return TRUE; // 2016.10.31
	//
	if ( SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( side , pointer , FALSE , &po , TRUE ) < 0 ) return FALSE;
	//
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */) return FALSE;
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */) return FALSE; // 2013.07.02
	//
	Res = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( side , pointer , -1 , &po , &sl , &fail );
	//
	if ( fail ) return FALSE;
	if ( Res == -1 ) return FALSE;
	//
	if ( po >= PM1 ) return TRUE;
	//
	if ( _i_SCH_IO_GET_MODULE_STATUS( po , sl ) != CWM_ABSENT ) return FALSE;
	//
	return TRUE;
}


//int SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( int side , int cm , BOOL unload ); // 2011.06.10 // 2011.08.10
int SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( int side , int pt , int cm ); // 2011.06.10 // 2011.08.10
int SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_START_FOR_OUTCASS( int side , int cm , BOOL check ); // 2011.06.21

int SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( int side , int pointer , BOOL inmode , BOOL *return_impossible , BOOL allcheck ) { // 2011.03.30
	int c , p , Res , seloc , PathOut;
	//
	if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == 0 ) { // 2016.10.31
		//
		*return_impossible = FALSE;
		return 0;
		//
	}
	//

	*return_impossible = TRUE;

	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return 101;
	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
	if ( c == -99 ) {
		*return_impossible = FALSE;
		return 102;
	}
	//
	if ( inmode ) {
		//
		PathOut = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
		//
		// 2012.04.01
		if ( ( PathOut >= CM1 ) && ( PathOut < PM1 ) ) {
			//
			if ( ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_13_NEED_IN /* IN */ ) ) { // 2013.07.02
				//
				if ( SCHMULTI_sub_CHECK_SUPPLY_POSSIBLE_FOR_OUTCASS( TRUE , PathOut - CM1 , STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_MaterialID( side , pointer ) ) , &seloc , _i_SCH_SYSTEM_ESOURCE_ID_GET( side ) ) ) {
					//
//					if ( ( seloc + CM1 ) != PathOut ) { // 2019.02.28
//						return -71; // 2019.02.28
//					} // 2019.02.28
//					else { // 2019.02.28
						if ( _i_SCH_IO_GET_MODULE_STATUS( seloc + CM1 , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) ) == CWM_PRESENT ) {
							//
							if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , seloc + CM1 ) != MUF_01_USE ) {
								//
								_i_SCH_MODULE_Set_Mdl_Use_Flag( side , seloc + CM1 , MUF_01_USE );
								//
								if ( Scheduler_HandOffIn_Ready_Part( side , seloc + CM1 ) != SYS_SUCCESS ) {
									//
									_i_SCH_MODULE_Set_Mdl_Use_Flag( side , seloc + CM1 , MUF_00_NOTUSE );
									//
									return -81;
									//
								}
								//
							}
							//
							SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_INCASS( side , pointer , seloc + CM1 );
							//
						}
						else {
							return -183;
						}
						//
						return 0;
						//
//					} // 2019.02.28
				}
				else {
					return -184;
				}
			}
		}
		//
		return 0;
		//
	}
	else {
		//
		PathOut = _i_SCH_CLUSTER_Get_PathOut( side , pointer );
		//
		// 2012.04.01
		if ( ( PathOut >= CM1 ) && ( PathOut < PM1 ) ) {
			//
			if ( ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , pointer ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) ) { // 2013.07.02
				//
				if ( _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , pointer ) >= 0 ) {
					//
					if ( SCHMULTI_sub_CHECK_SUPPLY_POSSIBLE_FOR_OUTCASS( TRUE , PathOut - CM1 , STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , pointer ) ) , &seloc , _i_SCH_SYSTEM_ESOURCE_ID_GET( side ) ) ) {
						//
						if ( _i_SCH_IO_GET_MODULE_STATUS( seloc + CM1 , _i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ) == CWM_ABSENT ) {
							//
							if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , seloc + CM1 ) != MUF_01_USE ) {
								//
								_i_SCH_MODULE_Set_Mdl_Use_Flag( side , seloc + CM1 , MUF_01_USE );
								//
								if ( Scheduler_HandOffIn_Ready_Part( side , seloc + CM1 ) != SYS_SUCCESS ) {
									//
									_i_SCH_MODULE_Set_Mdl_Use_Flag( side , seloc + CM1 , MUF_00_NOTUSE );
									//
									return -81;
									//
								}
								//
							}
							//
							if ( !SCHMULTI_Sub_SET_SUPPLY_POSSIBLE_OUTCASS( side , pointer , seloc + CM1 ) ) {
								if ( !allcheck ) return 182;
								return -182;
							}
							//
						}
						else {
							if ( !allcheck ) return 183;
							return -183;
						}
						//
						return 0;
						//
					}
					else {
						if ( !allcheck ) return 184;
						return -184;
					}
				}
			}
		}
	}
	//
//	if ( ( c < 0 ) || ( p < 0 ) ) { // 2012.02.18
	if ( ( _i_SCH_SYSTEM_CPJOB_GET( side ) == 2 ) || ( c < 0 ) || ( p < 0 ) ) { // 2012.02.18
		//
		if ( PathOut <= -100 ) { // 2011.06.09
			//
			if ( PathOut <= -200 ) { // 2011.06.09
				//
				PathOut = -PathOut % 100;
				//
				if ( ( PathOut >= CM1 ) && ( PathOut < PM1 ) ) {
					//
					if ( allcheck ) {
						//
//						if ( SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( side , PathOut , TRUE ) == -1 ) return -111; // 2011.08.10
						if ( SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_REQUEST_FOR_OUTCASS( side , pointer , PathOut ) == -1 ) return -111; // 2011.08.10
						//
					}
					//
				}
				//
				if ( !allcheck ) return 112;
				return -112;
				//
			}
			else { // 2011.06.09
				//
				PathOut = -PathOut % 100;
				//
				if ( ( PathOut >= CM1 ) && ( PathOut < PM1 ) ) {
					//
					Res = BUTTON_GET_FLOW_STATUS_VAR( PathOut - CM1 );
					//
					if ( side != ( PathOut - CM1 ) ) {
						//
						if ( Res != _MS_5_MAPPED ) {
							//
							if ( !allcheck ) return 121;
							return -121;
							//
						}
						//
						if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
							//
						}
						else {
							if ( _i_SCH_SYSTEM_USING_GET( PathOut - CM1 ) > 0 ) { // 2011.06.21
								//
								if ( !allcheck ) return 122;
								return -122;
								//
							}
						}
						//
						Res = 0;
						//
					}
					else {
						//
						if ( ( Res != _MS_5_MAPPED ) && ( Res != _MS_12_RUNNING ) ) {
							//
							if ( !allcheck ) return 123;
							return -123;
							//
						}
						//
					}
					//
					if ( !SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_START_FOR_OUTCASS( side , PathOut , TRUE ) ) { // 2011.06.21
						//
						if ( !allcheck ) return 131;
						return -131;
						//
					}
					//
/*
					if ( _i_SCH_IO_GET_MODULE_STATUS( PathOut , _i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ) != CWM_ABSENT ) {
						//
						if ( !allcheck ) return 123;
						return -123;
						//
					}
*/
					//
					if ( Res == _MS_5_MAPPED ) {
						BUTTON_SET_FLOW_MTLOUT_STATUS( side , PathOut - CM1 , TRUE , _MS_12_RUNNING ); // 2013.06.08
					}
					//
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , PathOut ) != MUF_01_USE ) {
						//
						_i_SCH_MODULE_Set_Mdl_Use_Flag( side , PathOut , MUF_01_USE );
						//
						if ( Scheduler_HandOffIn_Ready_Part( side , PathOut ) != SYS_SUCCESS ) {
							//
							_i_SCH_MODULE_Set_Mdl_Use_Flag( side , PathOut , MUF_00_NOTUSE );
							//
							return -4;
							//
						}
						//
					}
					//
//					_i_SCH_CLUSTER_Set_PathOut( side , pointer , PathOut ); // 2011.06.21
					SCHMULTI_CHECK_HANDOFF_OUT_MULTI_UNLOAD_START_FOR_OUTCASS( side , PathOut , FALSE ); // 2011.06.21
					//
					return 0;
					//
				}
				else {
					//
					*return_impossible = TRUE;
					//
					return 141;
				}
			}
			//
		}
		else {
			//
			if ( ( PathOut >= CM1 ) && ( PathOut < PM1 ) ) { // 2019.01.09
				if ( _i_SCH_IO_GET_MODULE_STATUS( PathOut , _i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ) != CWM_ABSENT ) {
					return -11;
				}
			}
			//
			//
			*return_impossible = FALSE;
			//
			return 151;
		}
	}
	else {
		//
//		if ( PathOut >= CM1 ) { // 2019.01.09
		if ( ( PathOut >= CM1 ) && ( PathOut < PM1 ) ) { // 2019.01.09
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( PathOut , _i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ) != CWM_ABSENT ) {
				//
				return -11;
				//
			}
			//
			return 161;
		}
		//
		Res = SCHMULTI_sub_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( c , p , &seloc );
		//
		if ( Res == 0 ) {
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( seloc + CM1 , _i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ) == CWM_ABSENT ) {
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , seloc + CM1 ) != MUF_01_USE ) { // 2011.05.03
					//
					_i_SCH_MODULE_Set_Mdl_Use_Flag( side , seloc + CM1 , MUF_01_USE );
					//
					if ( Scheduler_HandOffIn_Ready_Part( side , seloc + CM1 ) != SYS_SUCCESS ) {
						//
						_i_SCH_MODULE_Set_Mdl_Use_Flag( side , seloc + CM1 , MUF_00_NOTUSE );
						//
						return -12;
						//
					}
					//
				}
				//
				SCHMRDATA_Data_Setting_for_outcm( seloc + CM1 , side , pointer , 1 ); // 2011.09.07
				//
				if ( _i_SCH_CLUSTER_Get_PathOut( side , pointer ) <= 0 ) _i_SCH_CLUSTER_Set_PathOut( side , pointer , seloc + CM1 ); // 2013.07.02
				//
			}
			else {
				//
				return -14;
				//
			}
			//
		}
		else if ( Res < 0 ) { // 2011.04.14 (Testing)
//			return 162; // 2018.07.10
			if ( !allcheck ) return 162; // 2018.07.10
			return -162;  // 2018.07.10
		}
		//
		return Res;
		//
	}
	//
	return 0;
}

BOOL SCHMULTI_CHECK_ANOTHER_OUT_CASS_USING( BOOL reset , BOOL incheck , BOOL alwayschk , int cm , BOOL *used ) { // 2011.03.08

	int s , p , ps , i , index , finish;
	//
	*used = FALSE;
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.21
		return FALSE;
	}
	//
	for ( index = 0 ; index < MAX_MULTI_RUNJOB_SIZE ; index++ ) {
		//
		if ( alwayschk ) {
			if ( MULTI_RUNJOB_RunStatus(index) != CASSRUNJOB_STS_RUNNING ) {
				if ( MULTI_RUNJOB_RunStatus(index) != CASSRUNJOB_STS_WAITING ) {
					continue;
				}
			}
		}
		else {
			if ( MULTI_RUNJOB_RunStatus(index) != CASSRUNJOB_STS_WAITING ) {
				continue;
			}
		}
		//
		if ( MULTI_RUNJOB_Cassette(index) == (cm - CM1) ) {
			//
			if ( !incheck ) continue;
			//
			if ( MULTI_RUNJOB_RunStatus(index) == CASSRUNJOB_STS_WAITING ) return TRUE;
			//
			for ( i = 0 ; i < MULTI_RUNJOB_MtlCount(index) ; i++ ) {
				//
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(index)[i] < 0 ) continue; // 2011.04.28
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJName( index )[i][0] == 0 ) continue;
				//
				if ( MULTI_RUNJOB_MtlPJID(index)[i] < 0 ) continue;
				//
				if ( MULTI_RUNJOB_MtlPJIDRes(index)[i] == PJIDRES_READY ) return TRUE;
				//
				if ( MULTI_RUNJOB_MtlPJIDRes(index)[i] != PJIDRES_FINISH ) {
					//
					for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
						//
						if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
						//
						for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
							//
							if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
							//
							if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( s , p ) != index ) continue;
							if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( s , p ) != i ) continue;
							//
							ps = _i_SCH_CLUSTER_Get_PathStatus( s , p );
							//
							if ( ps == SCHEDULER_CM_END ) continue;
							//
							if ( ps == SCHEDULER_RETURN_REQUEST ) return TRUE;
							//
							if ( ps == SCHEDULER_READY ) {
								if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) == cm ) {
									return TRUE;
								}
							}
							//
							if ( _i_SCH_CLUSTER_Get_PathOut( s , p ) == cm ) {
								return TRUE;
							}
							//
						}
					}
					//
				}
				//
			}
		}
		else {
			//
			for ( i = 0 ; i < MULTI_RUNJOB_MtlCount(index) ; i++ ) {
				//
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(index)[i] < 0 ) continue; // 2011.04.28
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJName( index )[i][0] == 0 ) continue;
				//
				if ( MULTI_RUNJOB_MtlPJID(index)[i] < 0 ) continue;
				//
				if ( MULTI_RUNJOB_MtlPJIDRes(index)[i] == PJIDRES_FINISH ) {
					//
					if ( *used ) continue;
					//
					finish = TRUE;
				}
				else {
					finish = FALSE;
				}
				//
				if ( MULTI_RUNJOB_MtlOutCassette(index)[i] == 0 ) { // 2018.08.16
				}
				else if ( MULTI_RUNJOB_MtlOutCassette(index)[i] >= CM1 ) { // 2018.08.16
					//
					if ( MULTI_RUNJOB_MtlOutCassette(index)[i] == cm ) {
						if ( finish ) {
							*used = TRUE;
							//
							if ( reset ) { // 2011.04.14
								strcpy( MULTI_RUNJOB_MtlOutCarrierID(index)[i] , "" );
								MULTI_RUNJOB_MtlOutCassette(index)[i] = -1;
							}
							//
						}
						else {
							return TRUE;
						}
					}
					//
				}
				else {
					if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( cm - CM1 ) < 2 ) { // 2013.08.16
						if ( !STRCMP_L( "" , MULTI_RUNJOB_MtlOutCarrierID(index)[i] ) ) {
							//
							if ( STRCMP_L( SCHMULTI_CASSETTE_MID_GET_FOR_START( cm - CM1 ) , MULTI_RUNJOB_MtlOutCarrierID(index)[i] ) ) {
								if ( finish ) {
									*used = TRUE;
									//
									if ( reset ) { // 2011.04.14
										strcpy( MULTI_RUNJOB_MtlOutCarrierID(index)[i] , "" );
										MULTI_RUNJOB_MtlOutCassette(index)[i] = -1;
									}
									//
								}
								else {
									return TRUE;
								}
							}
							//
						}
					}
					else {
						if ( finish ) {
							*used = TRUE;
							//
							if ( reset ) { // 2011.04.14
								strcpy( MULTI_RUNJOB_MtlOutCarrierID(index)[i] , "" );
								MULTI_RUNJOB_MtlOutCassette(index)[i] = -1;
							}
							//
						}
						else {
							return TRUE;
						}
					}
					//
				}
				//
			}
		}
	}
	return FALSE;
}
//
//void SCHMULTI_RUNJOB_SET_STARTING( int index , int Cass ) { // 2018.08.30
void SCHMULTI_RUNJOB_SET_STARTING( int index , int Side , int Cass ) { // 2018.08.30
	int i , j , c;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	MULTI_RUNJOB_RunSide(index) = Side; // 2018.08.30
	//
	MULTI_RUNJOB_Cassette(index) = Cass;
	MULTI_RUNJOB_RunStatus(index) = CASSRUNJOB_STS_RUNNING;
	MULTI_RUNJOB_CassetteIndex(index) = SCHEDULER_Get_CARRIER_INDEX( Cass ); // 2018.05.24
	//--------------------
	//
	// 2018.06.27
	//
	for ( i = 0 ; i < MULTI_RUNJOB_MtlCount(index) ; i++ ) {
		//
		MULTI_RUNJOB_MtlInCassette(index)[i] = Cass + CM1;
		//
		strcpy( MULTI_RUNJOB_MtlInCarrierID(index)[i] , MULTI_RUNJOB_CarrierID(index) );
		//
		MULTI_RUNJOB_MtlInCarrierIndex(index)[i] = MULTI_RUNJOB_CassetteIndex(index);
		//
	}
	//
	//
	//--------------------
	//
	strcpy( Sch_Multi_CassRunJob_Data[Cass]->CarrierID , MULTI_RUNJOB_CarrierID( index ) );
	strcpy( Sch_Multi_CassRunJob_Data[Cass]->MtlCJName , MULTI_RUNJOB_MtlCJName( index ) );
	//
//	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2018.05.10
	//
	for ( i = 0 ; i < MAX_MULTI_CASS_SLOT_SIZE ; i++ ) { // 2018.05.10
		strcpy( Sch_Multi_CassRunJob_Data[Cass]->MtlPJName[i] , "" );
		Sch_Multi_CassRunJob_Data[Cass]->MtlOutSlot[i] = i + 1;
	}
//	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
	for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) { // 2018.05.10
		//
		c = MULTI_RUNJOB_MtlCount(index); // 2013.05.30
		//
		for ( i = 0 ; i < c ; i++ ) { // 2013.05.30
			//
			if ( ( MULTI_RUNJOB_MtlInCassette(index)[i] <= 0 ) || ( MULTI_RUNJOB_MtlInCassette(index)[i] == ( Cass + CM1 ) ) ) { // 2012.04.01(Testing)
				if ( MULTI_RUNJOB_MtlInSlot(index)[i] == ( j + 1 ) ) {
					//
					strcpy( Sch_Multi_CassRunJob_Data[Cass]->MtlPJName[j] , MULTI_RUNJOB_MtlPJName(index)[i] );
					Sch_Multi_CassRunJob_Data[Cass]->MtlOutSlot[j] = MULTI_RUNJOB_MtlOutSlot(index)[i];
					//
					break;
				}
			}
		}
	}
	//--------------------
	LeaveCriticalSection( &CR_MULTIJOB );
}
//
BOOL _i_SCH_MULTIJOB_RUNJOB_REMAP_OTHERCASS( int index , int trgCass , int srcCass ) { // 2006.03.08
	int i;
	EnterCriticalSection( &CR_MULTIJOB );
	//
	if ( MULTI_RUNJOB_Cassette(index) != srcCass ) {
		LeaveCriticalSection( &CR_MULTIJOB );
		return FALSE;
	}
	MULTI_RUNJOB_Cassette(index) = trgCass;
	MULTI_RUNJOB_CassetteIndex(index) = SCHEDULER_Get_CARRIER_INDEX( trgCass ); // 2018.05.24
	//--------------------
	Sch_Multi_Cassette_Verify[trgCass] = Sch_Multi_Cassette_Verify[srcCass];
//	strcpy( Sch_Multi_Cassette_MID[trgCass] , Sch_Multi_Cassette_MID[srcCass] ); // 2008.04.02
	if ( !STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID[trgCass]) , Sch_Multi_Cassette_MID[srcCass] ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_MULTIJOB_RUNJOB_REMAP_OTHERCASS 1 Memory Allocate Error[%d,%d,%d]\n" , index , trgCass , srcCass );
	}
	//
	Sch_Multi_Cassette_Verify[srcCass] = CASSETTE_VERIFY_READY;
//	strcpy( Sch_Multi_Cassette_MID[srcCass] , "" ); // 2008.04.02
	if ( !STR_MEM_MAKE_COPY2( &(Sch_Multi_Cassette_MID[srcCass]) , "" ) ) { // 2008.04.02 // 2010.03.25
		_IO_CIM_PRINTF( "_i_SCH_MULTIJOB_RUNJOB_REMAP_OTHERCASS 2 Memory Allocate Error[%d,%d,%d]\n" , index , trgCass , srcCass );
	}
	//--------------------
	strcpy( Sch_Multi_CassRunJob_Data[trgCass]->CarrierID , Sch_Multi_CassRunJob_Data[srcCass]->CarrierID );
	strcpy( Sch_Multi_CassRunJob_Data[trgCass]->MtlCJName , Sch_Multi_CassRunJob_Data[srcCass]->MtlCJName );
	//
//	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2018.05.10
	for ( i = 0 ; i < MAX_MULTI_CASS_SLOT_SIZE ; i++ ) { // 2018.05.10
		strcpy( Sch_Multi_CassRunJob_Data[trgCass]->MtlPJName[i] , Sch_Multi_CassRunJob_Data[srcCass]->MtlPJName[i] );
		Sch_Multi_CassRunJob_Data[trgCass]->MtlOutSlot[i] = Sch_Multi_CassRunJob_Data[srcCass]->MtlOutSlot[i];
		//
		strcpy( Sch_Multi_CassRunJob_Data[srcCass]->MtlPJName[i] , "" );
		Sch_Multi_CassRunJob_Data[srcCass]->MtlOutSlot[i] = i + 1;
	}
	//--------------------
	LeaveCriticalSection( &CR_MULTIJOB );
	return TRUE;
}
//
BOOL SCHMULTI_RUNJOB_GET_STOPPED( int side , int pointer ) { // 2015.03.27
	int c , p;
	if ( ( side < 0 ) || ( pointer < 0 ) ) {
		return FALSE;
	}
	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );

	if ( ( c < 0 ) || ( p < 0 ) ) {
		return FALSE;
	}
	if ( MULTI_RUNJOB_MtlPJIDEnd(c)[p] == 1 ) return TRUE;
	//
	return FALSE;
	//
}
//
//
void SCHMULTI_RUNJOB_RESET_FOR_STOP( int side , int pointer ) {
	int c , p;
	if ( ( side < 0 ) || ( pointer < 0 ) ) {
		return;
	}
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );

	if ( ( c < 0 ) || ( p < 0 ) ) {
		return;
	}
	MULTI_RUNJOB_MtlPJIDEnd(c)[p] = 2;
}
//
BOOL SCHMULTI_RUNJOB_ALL_FINISHED( int index ) { // 2018.11.07
	//
	int k , rs , rp;
	//
	for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(index) ; k++ ) {
		//
		if ( MULTI_RUNJOB_MtlPJID(index)[k] < 0 ) continue;
		//
		if ( MULTI_RUNJOB_MtlPJName( index )[k][0] == 0 ) continue;
		//
		if ( MULTI_RUNJOB_MtlPJIDRes(index)[k] != PJIDRES_FINISH ) return FALSE;
		//
		rs = MULTI_RUNJOB_Mtl_Side( index )[k];
		rp = MULTI_RUNJOB_Mtl_Pointer( index )[k];
		//
		if ( ( rs >= 0 ) && ( rs < MAX_CASSETTE_SIDE ) ) {
			if ( ( rp >= 0 ) && ( rp < MAX_CASSETTE_SLOT_SIZE ) ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathStatus( rs , rp ) != SCHEDULER_CM_END ) return FALSE;
				//
			}
		}
		//
	}
	//
	return TRUE;
	//
}
//
//					//
//void SCHMULTI_RUNJOB_SET_FINISH_LOW( int Cass , int Result ) { // 2018.08.30
void SCHMULTI_RUNJOB_SET_FINISH_LOW( int Side , int Cass , int Result ) { // 2018.08.30
	int i;
	if ( Side == -1 ) { // 2018.08.30
		//
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_RUNNING ) {
				if ( MULTI_RUNJOB_Cassette(i) == Cass ) {
					//
					if ( !SCHMULTI_RUNJOB_ALL_FINISHED( i ) ) continue; // 2018.11.07
					//
					if ( Result == -1 ) { // 2004.06.02
						MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_FINISH_WITH_NORMAL;
					}
					else { // 2004.06.02
						MULTI_RUNJOB_RunStatus(i) = Result;
					}
					//
//					return; // 2018.11.07
					//
				}
			}
		}
	}
	else { // 2018.08.30
		//
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_RUNNING ) {
				if ( MULTI_RUNJOB_RunSide(i) == Side ) {
					//
					if ( Result == -1 ) { // 2004.06.02
						MULTI_RUNJOB_RunStatus(i) = CASSRUNJOB_STS_FINISH_WITH_NORMAL;
					}
					else { // 2004.06.02
						MULTI_RUNJOB_RunStatus(i) = Result;
					}
					return;
				}
			}
		}
	}
}
//
//void SCHMULTI_RUNJOB_SET_FINISH( int Cass , int Result ) { // 2018.08.30
void SCHMULTI_RUNJOB_SET_FINISH( int Side , int Cass , int Result ) { // 2018.08.30
	EnterCriticalSection( &CR_MULTIJOB );
//	SCHMULTI_RUNJOB_SET_FINISH_LOW( Cass , Result ); // 2018.08.30
	SCHMULTI_RUNJOB_SET_FINISH_LOW( Side , Cass , Result ); // 2018.08.30
	LeaveCriticalSection( &CR_MULTIJOB );
}
//
//BOOL SCHMULTI_RUNJOB_ALL_FINISH( int Cass , int index , int *result ) { // 2018.08.30
BOOL SCHMULTI_RUNJOB_ALL_FINISH( int Side , int index , int *result ) { // 2018.08.30
	int i , c;
	EnterCriticalSection( &CR_MULTIJOB );
	*result = 0; // 2004.06.02
	c = 0; // 2004.10.15
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_READY ) continue; // 2018.08.30
		//
		if ( MULTI_RUNJOB_SelectCJIndex(i) == index ) {
//			c++; // 2004.10.15 // 2005.03.10
//			if ( ( MULTI_RUNJOB_RunStatus(i) != CASSRUNJOB_STS_READY ) && ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_FINISH_WITH_NORMAL ) ) { // 2018.08.30
			if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_FINISH_WITH_NORMAL ) { // 2018.08.30
				LeaveCriticalSection( &CR_MULTIJOB );
				return FALSE;
			}
			else { // 2004.06.02
				//=====================================================================================================
				// 2007.07.04
				//=====================================================================================================
//				if ( MULTI_RUNJOB_Cassette(i) == Cass ) { // 2007.07.04 // 2018.08.30
				if ( MULTI_RUNJOB_RunSide(i) == Side ) { // 2007.07.04 // 2018.08.30
				//=====================================================================================================
//					if ( MULTI_RUNJOB_RunStatus(i) != CASSRUNJOB_STS_READY ) { // 2005.03.10 // 2018.08.30
						c++; // 2005.03.10
						switch( MULTI_RUNJOB_RunStatus(i) ) {
						case CASSRUNJOB_STS_FINISH_WITH_NORMAL :
							if ( Sch_Multi_ControlJob_Selected_Stop_Signal[index] ) {
								*result = 3;
							}
							else {
//								*result = 0; // 2018.11.20
								*result = Sch_Multi_Ins_ControlJob_Select_Data[index].FixedResult; // 2018.11.20
							}
							break;
						case CASSRUNJOB_STS_FINISH_WITH_ABORT :
							*result = 1;
							break;
						case CASSRUNJOB_STS_FINISH_WITH_ERROR :
							*result = 2;
							break;
						case CASSRUNJOB_STS_FINISH_WITH_ENDSTOP :
							*result = 3;
							break;
						case CASSRUNJOB_STS_FINISH_WITH_CANCELDELETE :
							*result = 4;
							break;
						case CASSRUNJOB_STS_FINISH_WITH_FORCEFINISH :
							*result = 5;
							break;
						}
//					} // 2018.08.30
				}
			}
		}
	}
	LeaveCriticalSection( &CR_MULTIJOB );
	if ( c == 0 ) return FALSE; // 2004.10.15
	return TRUE;
}
//


//
//BOOL SCHMULTI_RUNJOB_STOPPED( int Cass_Side ) { // 2016.09.13 // 2018.08.30
BOOL SCHMULTI_RUNJOB_STOPPED( int Side ) { // 2016.09.13 // 2018.08.30
	int i , c;

	if ( !SCHMULTI_CTJOB_USE_GET() ) return FALSE;
	//
	EnterCriticalSection( &CR_MULTIJOB );
	//
	for ( c = 0 ; c < MAX_MULTI_CTJOBSELECT_SIZE ; c++ ) {
		//
		if ( !Sch_Multi_ControlJob_Selected_Stop_Signal[c] ) continue;
		//
		if ( Sch_Multi_ControlJob_Select_Data[c]->ControlStatus < CTLJOB_STS_EXECUTING ) continue;
		//
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			//
			if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_READY ) continue;
			//
			if ( MULTI_RUNJOB_SelectCJIndex(i) != c ) continue;
			//
//			if ( MULTI_RUNJOB_Cassette(i) != Cass_Side ) continue; // 2018.08.30
			if ( MULTI_RUNJOB_RunSide(i) != Side ) continue; // 2018.08.30
			//
			LeaveCriticalSection( &CR_MULTIJOB );
			return TRUE;
			//
		}
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	//
	return FALSE;
}
//



void SCHMULTI_RUNJOB_SET_DATABASE_LINK( int side ) {
	int sr , r , j , i , count;

	sr = _i_SCH_SYSTEM_CPJOB_ID_GET( side );

	for ( r = 0 ; r < MAX_MULTI_RUNJOB_SIZE ; r++ ) {
		//
		if ( MULTI_RUNJOB_RunSide(r) != side ) continue; // 2018.11.07
		//
		if ( sr != r ) {
			//
			if ( MULTI_RUNJOB_RunStatus(r) < CASSRUNJOB_STS_WAITING ) continue;
			if ( MULTI_RUNJOB_RunStatus(r) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
			//
			if ( !STRCMP( MULTI_RUNJOB_MtlCJName( sr ) , MULTI_RUNJOB_MtlCJName( r ) ) ) continue;
		}
		//
		count = MULTI_RUNJOB_MtlCount(r);
		//
		for ( j = 0 ; j < count ; j++ ) {
			//
			MULTI_RUNJOB_MtlPJIDRes(r)[j] = 0;
			MULTI_RUNJOB_MtlPJIDEnd(r)[j] = 0;
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
				//
				if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , i ) , 0 ) ) continue;
				//
				if ( MULTI_RUNJOB_MtlInCassette(r)[j] <= 0 ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != ( MULTI_RUNJOB_Cassette(r) + CM1 ) ) continue;
				}
				else { // 2012.04.01(Testing)
					if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != MULTI_RUNJOB_MtlInCassette(r)[j] ) continue;
				}
				//
				if ( _i_SCH_CLUSTER_Get_SlotIn( side , i ) != MULTI_RUNJOB_MtlInSlot(r)[j] ) continue;
				//
				//
				// 2018.05.10
				//
				//
				MULTI_RUNJOB_Mtl_Side(r)[j] = side;
				MULTI_RUNJOB_Mtl_Pointer(r)[j] = i;
				//
//
// 2018.11.15
//				MULTI_RUNJOB_MtlInCassette(r)[j] = _i_SCH_CLUSTER_Get_PathIn( side , i );
//				MULTI_RUNJOB_MtlInCarrierIndex(r)[j] = _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , i );
//				if ( _i_SCH_CLUSTER_Get_Ex_MaterialID( side , i ) == NULL ) {
//					strcpy( MULTI_RUNJOB_MtlInCarrierID(r)[j] , "" );
//				}
//				else {
//					strcpy( MULTI_RUNJOB_MtlInCarrierID(r)[j] , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , i ) );
//				}
//				//
//				if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) == 0 ) { // 2018.07.17
//					MULTI_RUNJOB_MtlOutCassette(r)[j] = 0;
//				}
//				else if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) <= -100 ) {
//					MULTI_RUNJOB_MtlOutCassette(r)[j] = ( -_i_SCH_CLUSTER_Get_PathOut( side , i ) % 100 );
//				}
//				else {
//					MULTI_RUNJOB_MtlOutCassette(r)[j] = _i_SCH_CLUSTER_Get_PathOut( side , i );
//				}
//				MULTI_RUNJOB_MtlOutCarrierIndex(r)[j] = _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( side , i );
//				//
//				if ( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , i ) == NULL ) {
//					strcpy( MULTI_RUNJOB_MtlOutCarrierID(r)[j] , "" );
//				}
//				else {
//					strcpy( MULTI_RUNJOB_MtlOutCarrierID(r)[j] , _i_SCH_CLUSTER_Get_Ex_OutMaterialID( side , i ) );
//				}
				//
				_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , i , r );
				_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , i , j );
				//
				break;
			}
		}
	}
	//
}

char iSCHMULTI_RUNJOB_STR_DATABASE_LINK_PJ[256];

char *SCHMULTI_RUNJOB_GET_DATABASE_LINK_PJ( int mode , int side , int pt ) {
	int r , j , count;

	strcpy( iSCHMULTI_RUNJOB_STR_DATABASE_LINK_PJ , "" );
	//
	if ( mode == 1 ) {
		//
		r = _i_SCH_SYSTEM_CPJOB_ID_GET( side );
		//
		count = MULTI_RUNJOB_MtlCount(r);
		//
		for ( j = 0 ; j < count ; j++ ) {
			//
			if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) { // 2014.08.06
				//
				if ( MULTI_RUNJOB_MtlInCassette(r)[j] <= 0 ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) != ( MULTI_RUNJOB_Cassette(r) + CM1 ) ) continue;
				}
				else { // 2012.04.01(Testing)
					if ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) != MULTI_RUNJOB_MtlInCassette(r)[j] ) continue;
				}
				//
				if ( _i_SCH_CLUSTER_Get_SlotIn( side , pt ) != MULTI_RUNJOB_MtlInSlot(r)[j] ) continue;
				//
			}
			//
			strcpy( iSCHMULTI_RUNJOB_STR_DATABASE_LINK_PJ , MULTI_RUNJOB_MtlPJName( r )[j] );
			//
			return iSCHMULTI_RUNJOB_STR_DATABASE_LINK_PJ;
			//
		}
		//
	}
	//
	return iSCHMULTI_RUNJOB_STR_DATABASE_LINK_PJ;
	//
}

BOOL SCHMULTI_RUNJOB_GET_DATABASE_J_START( int side ) { // 2012.06.28
	int i , j , p;
	//
	i = _i_SCH_SYSTEM_CPJOB_ID_GET( side );
	//
	for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
		//============================================================================
		p = MULTI_RUNJOB_MtlPJID(i)[j];
		//============================================================================
		if ( p < 0 ) continue;
		//============================================================================
		if ( Sch_Multi_ProcessJob[p]->MtlFormat >= 100 ) return TRUE;
		//============================================================================
	}
	//
	return FALSE;
	//
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_PROCESSJOB_START_CHECK( int side , int pointer ) {
	int c , p , i , j;
//	if ( Sch_Multi_ControlJob_Select_Data->ControlStatus < CTLJOB_STS_EXECUTING ) return;
	if ( ( side < 0 ) || ( pointer < 0 ) ) return;
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	if ( ( c < 0 ) || ( p < 0 ) ) return;
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( MULTI_RUNJOB_SelectCJIndex(i) == MULTI_RUNJOB_SelectCJIndex(c) ) {
			//
			if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
			if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
			//
			for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == -1 ) continue; // 2004.06.21 
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
					if ( MULTI_RUNJOB_MtlPJIDRes(i)[j] != PJIDRES_READY ) {
						MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_FIRST;
						return;
					}
				}
			}
		}
	}
	MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_FIRST;
	//
	EnterCriticalSection( &CR_MULTIJOB ); // 2013.02.13
	//
	SCHMULTI_PROCESSJOB_SET_STATUS( -1 , MULTI_RUNJOB_MtlPJName( c )[p] , PRJOB_STS_PROCESSING );
	//
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13 // 2015.03.27
	//
	SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_START , side , 0 , MULTI_RUNJOB_MtlPJName( c )[p] , side , pointer );
	// Logging System Ready
	PROCESS_MONITOR_LOG_READY_for_PRJOB( MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] ); // 2002.05.20
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
	//
}
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( int side , int pointer ) { // 2004.05.10
//	int i , j , k , x , index , slot; // 2011.04.21
	int c , p , j , k , x; // 2011.04.21

//	SCHMULTI_MESSAGE_PROCESSJOB_START_CHECK( side , pointer ); // 2007.12.03

	if ( _i_SCH_SYSTEM_CPJOB_GET(side) == 0 ) return TRUE;

	SCHMULTI_MESSAGE_PROCESSJOB_START_CHECK( side , pointer ); // 2007.12.03

	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer ); // 2011.04.21
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer ); // 2011.04.21
	//
	if ( ( c < 0 ) || ( p < 0 ) ) return TRUE; // 2011.04.21

	// 2011.04.21
	switch( MULTI_RUNJOB_MtlPJ_StartMode(c)[p] ) {
	case PJ_SIGNAL_WAIT :
		//
		EnterCriticalSection( &CR_MULTIJOB ); // 2013.02.13
		//
		SCHMULTI_PROCESSJOB_SET_STATUS( -1 , MULTI_RUNJOB_MtlPJName( c )[p] , PRJOB_STS_WAITINGFORSTART );
		//
//		LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13 // 2015.03.27
		//
		SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_WAITVERIFY , 0 , 0 , MULTI_RUNJOB_MtlPJName( c )[p] , side , pointer );
		//
		x = MULTI_RUNJOB_MtlPJID(c)[p];
		for ( j = 0 ; j < MAX_MULTI_RUNJOB_SIZE ; j++ ) {
			//
			if ( MULTI_RUNJOB_RunStatus(j) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
			if ( MULTI_RUNJOB_RunStatus(j) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
			//
			for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(j) ; k++ ) {
				if ( x == MULTI_RUNJOB_MtlPJID(j)[k] ) {
					MULTI_RUNJOB_MtlPJ_StartMode(j)[k] = PJ_SIGNAL_FINISH;
				}
			}
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB ); // 2015.03.27
		//
		return FALSE;
		break;
	}

	return TRUE;
}
//---------------------------------------------------------------------------------------
void _i_SCH_MULTIJOB_PROCESSJOB_END_CHECK( int side , int pointer ) {
	int c , p , i , j , Res;
	int pindex , pres;
//	if ( Sch_Multi_ControlJob_Select_Data->ControlStatus < CTLJOB_STS_EXECUTING ) return;
	//------------------------------------------------------------------------------------------------------------------------
	if ( ( side < 0 ) || ( pointer < 0 ) ) return;
	//------------------------------------------------------------------------------------------------------------------------
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//------------------------------------------------------------------------------------------------------------------------
	if ( ( c < 0 ) || ( p < 0 ) ) return;
	//------------------------------------------------------------------------------------------------------------------------
	if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 0 , 2 ) < _i_SCH_CLUSTER_Get_PathRun( side , pointer , 10 , 2 ) ) return; // 2009.02.16
	//------------------------------------------------------------------------------------------------------------------------
	//
	// 2014.01.24
	//
	if ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) == PATHDO_WAFER_RETURN ) {
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , pointer ) == SCHEDULER_RETURN_REQUEST ) {
			return;
		}
	}
	//
	EnterCriticalSection( &CR_MULTIJOB ); // 2007.09.29
	//------------------------------------------------------------------------------------------------------------------------
	MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_FINISH;
	//
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( MULTI_RUNJOB_SelectCJIndex(i) == MULTI_RUNJOB_SelectCJIndex(c) ) {
			//
			if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
			if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
			//
			for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == -1 ) continue; // 2004.06.21 
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
					switch ( MULTI_RUNJOB_MtlPJIDRes(i)[j] ) {
					case PJIDRES_READY :
						if ( MULTI_RUNJOB_MtlPJIDEnd(i)[j] != 1 ) {
							//------------------------------------------------------------------------------------------------------------------------
							LeaveCriticalSection( &CR_MULTIJOB ); // 2007.09.29
							//------------------------------------------------------------------------------------------------------------------------
							return;
						}
						break;
					case PJIDRES_FIRST :
					case PJIDRES_RUNNING :
					case PJIDRES_LAST :
						//------------------------------------------------------------------------------------------------------------------------
						LeaveCriticalSection( &CR_MULTIJOB ); // 2007.09.29
						//------------------------------------------------------------------------------------------------------------------------
						return;
						break;
					default :
						break;
					}
				}
			}
		}
	}
	Res = SCHMULTI_PROCESSJOB_GET_LAST_STATUS( MULTI_RUNJOB_MtlPJName( c )[p] );
	SCHMULTI_PROCESSJOB_SET_STATUS( -1 , MULTI_RUNJOB_MtlPJName( c )[p] , PRJOB_STS_PROCESSCOMPLETED );
//	if ( _i_SCH_SYSTEM_MODE_LOOP_GET( side ) ) { // 2002.05.20
//	if ( ( _i_SCH_SYSTEM_MODE_LOOP_GET( side ) == 2 ) || ( Res == PRJOB_STS_STOPPING ) ) { // 2002.05.20 // 2006.10.26
	if ( ( _i_SCH_SYSTEM_MODE_LOOP_GET( side ) == 2 ) || ( _i_SCH_SYSTEM_MODE_END_GET( side ) != 0 ) || ( Res == PRJOB_STS_STOPPING ) ) { // 2002.05.20 // 2006.10.26
//		if ( Res != PRJOB_STS_QUEUED ) { // 2002.05.20 // 2007.11.05
		if ( ( Res != PRJOB_STS_QUEUED ) && ( Res != PRJOB_STS_PROCESSCOMPLETED ) ) { // 2002.05.20 // 2007.11.05
			//===========================================================
			//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
			PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_ENDSTOP , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] ); // 2002.05.20
			//===========================================================
			if ( ( Res != PRJOB_STS_STOPPING ) && ( SCHMULTI_MESSAGE_SCENARIO_GET() == 1 ) ) { // 2006.11.03
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_ABORT , Res , MULTI_RUNJOB_MtlPJName( c )[p] , side , pointer ); // 2006.11.03
			}
			else {
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_ENDSTOP , Res , MULTI_RUNJOB_MtlPJName( c )[p] , side , pointer );
			}
			//===========================================================
		}
	}
	else {
		if ( Res != PRJOB_STS_PROCESSCOMPLETED ) { // 2007.11.05
			//===========================================================
			//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
			PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_NORMAL , MULTI_RUNJOB_MtlCJName( c ) , MULTI_RUNJOB_MtlPJName( c )[p] ); // 2002.05.20
			//===========================================================
			//
//			SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_NORMAL , Res , MULTI_RUNJOB_MtlPJName( c )[p] , side , pointer ); // 2018.11.14
			//
			// 2018.11.14
			//
			pindex = SCHMULTI_PROCESSJOB_Find( MULTI_RUNJOB_MtlPJName( c )[p] );
			//
			if ( pindex >= 0 ) {
				pres = Sch_Multi_Ins_ProcessJob[pindex].FixedResult;
			}
			else {
				pres = PRJOB_RESULT_NORMAL;
			}
			//
			SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , pres , Res , MULTI_RUNJOB_MtlPJName( c )[p] , side , pointer );
			//
		}
	}
	//------------------------------------------------------------------------------------------------------------------------
	LeaveCriticalSection( &CR_MULTIJOB ); // 2007.09.29
	//------------------------------------------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( int side , int error ) {
	int Sel , i , j , k , x , m , Res , dres;
	int SelCJ; // 2018.11.07
	BOOL abortwaitchg; // 2018.11.07
	int pindex , pres; // 2018.11.14
	//------------------------------------------------------------------------------------------------------------------------
	EnterCriticalSection( &CR_MULTIJOB ); // 2007.09.29
	//------------------------------------------------------------------------------------------------------------------------
/*
// 2018.11.07
//
//	if ( SIGNAL_MODE_ABORT_GET( side ) ) { // 2004.07.12 // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( side ) > 0 ) { // 2004.07.12 // 2016.01.12
		dres = CASSRUNJOB_STS_FINISH_WITH_ABORT; // 2004.07.12
	}
	else { // 2004.07.12
		dres = CASSRUNJOB_STS_FINISH_WITH_NORMAL; // 2004.07.12
	}
	//
*/
//
// 2018.11.07
//
//
	if      ( error == SYS_ABORTED ) {
		abortwaitchg = TRUE; // 2018.11.07
		dres = CASSRUNJOB_STS_FINISH_WITH_ABORT;
	}
	else if ( error == SYS_ERROR ) {
		abortwaitchg = FALSE; // 2018.11.07
		dres = CASSRUNJOB_STS_FINISH_WITH_ERROR;
	}
	else {
		abortwaitchg = FALSE; // 2018.11.07
		if ( SIGNAL_MODE_ABORT_GET( side ) > 0 ) { // 2004.07.12 // 2016.01.12
			dres = CASSRUNJOB_STS_FINISH_WITH_ABORT; // 2004.07.12
		}
		else { // 2004.07.12
			dres = CASSRUNJOB_STS_FINISH_WITH_NORMAL; // 2004.07.12
		}
	}
	//
	//------------------------------------------------------------------------------------------------------------------------
	for ( i = 0 , Sel = -1; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
//		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_RUNNING ) { // 2018.12.20
		if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_RUNNING ) { // 2018.12.20
//			if ( MULTI_RUNJOB_Cassette(i) == side ) { // 2018.08.30
			if ( MULTI_RUNJOB_RunSide(i) == side ) { // 2018.08.30
				Sel = i;
				SelCJ = MULTI_RUNJOB_SelectCJIndex( i ); // 2018.11.07
				break;
			}
		}
	}

	if ( Sel == -1 ) {
//		SCHMULTI_RUNJOB_SET_FINISH( side , dres , FALSE ); // 2002.12.19
//		SCHMULTI_RUNJOB_SET_FINISH( side , dres , TRUE ); // 2002.12.19 // 2007.09.29 // 2010.03.25
//		SCHMULTI_RUNJOB_SET_FINISH_LOW( side , dres ); // 2002.12.19 // 2007.09.29 // 2010.03.25 // 2018.08.30
		SCHMULTI_RUNJOB_SET_FINISH_LOW( side , -1 , dres ); // 2002.12.19 // 2007.09.29 // 2010.03.25 // 2018.08.30
		//------------------------------------------------------------------------------------------------------------------------
		LeaveCriticalSection( &CR_MULTIJOB ); // 2007.09.29
		//------------------------------------------------------------------------------------------------------------------------
		return;
	}
	//
	for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(Sel) ; j++ ) {
		if ( MULTI_RUNJOB_MtlPJID(Sel)[j] >= 0 ) {
			m = 0;
			switch ( MULTI_RUNJOB_MtlPJIDRes(Sel)[j] ) {
			case PJIDRES_READY :
				if ( MULTI_RUNJOB_MtlPJIDEnd(Sel)[j] != 1 ) {
					m = 1;
				}
				break;
			case PJIDRES_FIRST :
			case PJIDRES_RUNNING :
			case PJIDRES_LAST :
				m = 1;
				break;
			default :
				break;
			}
			if ( m == 1 ) {
				x = 0;
				for ( i = 0 ; ( x == 0 ) && ( i < MAX_MULTI_RUNJOB_SIZE ); i++ ) {
					if ( i != Sel ) {
						//
						if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
						if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
						//
						for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(i) ; k++ ) {
							//============================================================================
							if ( MULTI_RUNJOB_MtlPJID(i)[k] == -1 ) continue; // 2004.06.21 
							//============================================================================
							if ( MULTI_RUNJOB_MtlPJID(i)[k] == MULTI_RUNJOB_MtlPJID(Sel)[j] ) {
								if ( MULTI_RUNJOB_MtlPJIDRes(i)[k] == PJIDRES_READY ) {
									if ( MULTI_RUNJOB_MtlPJIDEnd(i)[k] != 1 ) {
										x = 1;
										break;
									}
								}
								else if ( MULTI_RUNJOB_MtlPJIDRes(i)[k] < PJIDRES_FINISH ) {
									x = 1;
									break;
								}
							}
						}
					}
				}
				if ( x == 0 ) {
					Res = SCHMULTI_PROCESSJOB_GET_LAST_STATUS( MULTI_RUNJOB_MtlPJName( Sel )[j] );
					SCHMULTI_PROCESSJOB_SET_STATUS( -1 , MULTI_RUNJOB_MtlPJName( Sel )[j] , PRJOB_STS_PROCESSCOMPLETED );
					switch( error ) {
					case SYS_SUCCESS :
						//if ( _i_SCH_SYSTEM_MODE_LOOP_GET( side ) ) { // 2002.05.20
//						if ( ( _i_SCH_SYSTEM_MODE_LOOP_GET( side ) == 2 ) || ( Res == PRJOB_STS_STOPPING ) ) { // 2002.05.20 // 2006.10.26
						if ( ( _i_SCH_SYSTEM_MODE_LOOP_GET( side ) == 2 ) || ( _i_SCH_SYSTEM_MODE_END_GET( side ) != 0 ) || ( Res == PRJOB_STS_STOPPING ) ) { // 2002.05.20 // 2006.10.26
//							if ( Res != PRJOB_STS_QUEUED ) { // 2002.05.20 // 2007.11.05
							if ( ( Res != PRJOB_STS_QUEUED ) && ( Res != PRJOB_STS_PROCESSCOMPLETED ) ) { // 2002.05.20 // 2007.11.05
								//===========================================================
								//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
								PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_ENDSTOP , MULTI_RUNJOB_MtlCJName( Sel ) , MULTI_RUNJOB_MtlPJName( Sel )[j] ); // 2002.05.20
								//===========================================================
								if ( ( Res != PRJOB_STS_STOPPING ) && ( SCHMULTI_MESSAGE_SCENARIO_GET() == 1 ) ) { // 2006.11.03
									SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_ABORT , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 ); // 2006.11.03
								}
								else {
									SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_ENDSTOP , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 );
								}
								//===========================================================
								if ( dres == CASSRUNJOB_STS_FINISH_WITH_NORMAL ) { // 2004.06.02
									dres = CASSRUNJOB_STS_FINISH_WITH_ENDSTOP;
								}
								//===========================================================
							}
						}
						else {
							if ( Res != PRJOB_STS_PROCESSCOMPLETED ) { // 2007.11.05
								//===========================================================
								//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
								PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_NORMAL , MULTI_RUNJOB_MtlCJName( Sel ) , MULTI_RUNJOB_MtlPJName( Sel )[j] ); // 2002.05.20
								//===========================================================
//								SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_NORMAL , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 ); // 2018.11.14
								//
								//
								// 2018.11.14
								//
								pindex = SCHMULTI_PROCESSJOB_Find( MULTI_RUNJOB_MtlPJName( Sel )[j] );
								//
								if ( pindex >= 0 ) {
									pres = Sch_Multi_Ins_ProcessJob[pindex].FixedResult;
								}
								else {
									pres = PRJOB_RESULT_NORMAL;
								}
								//
								SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , pres , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 );
								//
								//===========================================================
							}
						}
						break;
					case SYS_ERROR :
//						if ( Res != PRJOB_STS_QUEUED ) { // 2002.05.20 // 2007.11.05
						if ( ( Res != PRJOB_STS_QUEUED ) && ( Res != PRJOB_STS_PROCESSCOMPLETED ) ) { // 2002.05.20 // 2007.11.05
							//===========================================================
							//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
							PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_ERROR , MULTI_RUNJOB_MtlCJName( Sel ) , MULTI_RUNJOB_MtlPJName( Sel )[j] ); // 2002.05.20
							//===========================================================
							SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_ERROR , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 );
							//===========================================================
							if ( dres == CASSRUNJOB_STS_FINISH_WITH_NORMAL ) { // 2004.06.02
								dres = CASSRUNJOB_STS_FINISH_WITH_ERROR;
							}
							//===========================================================
						}
						break;
					default :
//						if ( Res != PRJOB_STS_QUEUED ) { // 2002.05.20 // 2007.11.05
						if ( ( Res != PRJOB_STS_QUEUED ) && ( Res != PRJOB_STS_PROCESSCOMPLETED ) ) { // 2002.05.20 // 2007.11.05
							if ( ( Res == PRJOB_STS_SETTINGUP ) || ( Res == PRJOB_STS_WAITINGFORSTART )  ) { // 2004.10.14
								//===========================================================
								//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
								PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_ABORT , MULTI_RUNJOB_MtlCJName( Sel ) , MULTI_RUNJOB_MtlPJName( Sel )[j] ); // 2002.05.20
								//===========================================================
								SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_FORCEFINISH , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 );
								//===========================================================
								if ( dres == CASSRUNJOB_STS_FINISH_WITH_NORMAL ) { // 2004.06.02
									dres = CASSRUNJOB_STS_FINISH_WITH_ABORT;
								}
								//===========================================================
							}
							else {
								//===========================================================
								//-- 2002.05.20 for PRJOB Log // 2002.11.13(order change)
								PROCESS_MONITOR_LOG_END_for_PRJOB( PRJOB_RESULT_ABORT , MULTI_RUNJOB_MtlCJName( Sel ) , MULTI_RUNJOB_MtlPJName( Sel )[j] ); // 2002.05.20
								//===========================================================
								SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FINISH , PRJOB_RESULT_ABORT , Res , MULTI_RUNJOB_MtlPJName( Sel )[j] , side , -1 );
								//===========================================================
								if ( dres == CASSRUNJOB_STS_FINISH_WITH_NORMAL ) { // 2004.06.02
									dres = CASSRUNJOB_STS_FINISH_WITH_ABORT;
								}
								//===========================================================
							}
						}
						break;
					}
					for ( i = j + 1; i < MULTI_RUNJOB_MtlCount(Sel) ; i++ ) {
						if ( MULTI_RUNJOB_MtlPJID(Sel)[i] == MULTI_RUNJOB_MtlPJID(Sel)[j] ) {
							MULTI_RUNJOB_MtlPJIDRes(Sel)[i] = PJIDRES_FINISH;
						}
					}
				}
				MULTI_RUNJOB_MtlPJIDRes(Sel)[j] = PJIDRES_FINISH;
			}
		}
	}
	//
	//
	if ( abortwaitchg ) { // 2018.11.07
		//
		for ( i = 0; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			//
			if ( MULTI_RUNJOB_SelectCJIndex(i) != SelCJ ) continue;
			//
			if ( MULTI_RUNJOB_RunStatus(i) != CASSRUNJOB_STS_WAITING ) continue;
			//
			MULTI_RUNJOB_RunStatus(i) = dres;
			//
		}
	}
	//
	//
	//
	//----------------------------------------
//	SCHMULTI_RUNJOB_SET_FINISH( side , dres , FALSE ); // 2002.12.19
//	SCHMULTI_RUNJOB_SET_FINISH( side , dres , TRUE ); // 2002.12.19 // 2007.09.29 // 2010.03.25
//	SCHMULTI_RUNJOB_SET_FINISH_LOW( side , dres ); // 2002.12.19 // 2007.09.29 // 2010.03.25 // 2018.08.30
	SCHMULTI_RUNJOB_SET_FINISH_LOW( side , -1 , dres ); // 2002.12.19 // 2007.09.29 // 2010.03.25 // 2018.08.30
	//----------------------------------------
	//------------------------------------------------------------------------------------------------------------------------
	LeaveCriticalSection( &CR_MULTIJOB ); // 2007.09.29
	//------------------------------------------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_PROCESSJOB_PROCESS_RETURN_RESTORE( int side , int pointer ) { // 2017.09.08
	int c , p;
	//
	if ( ( side < 0 ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	//
	if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer ,  0 , 2 ) == 1 ) _i_SCH_CLUSTER_Set_PathRun( side , pointer , 0 , 2 , 0 ); // Picked Count
	//
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
	//--------------------------------------------------------------------
	if ( ( c < 0 ) || ( p < 0 ) ) return FALSE;
	//--------------------------------------------------------------------
	//
	MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_READY;
	//
	return TRUE;
	//
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_PROCESSJOB_PROCESS_FIRST_START_CHECK( int side , int pointer , int WfrID ) {
	int c , p , i , j , k , nx;
//	if ( Sch_Multi_ControlJob_Select_Data->ControlStatus < CTLJOB_STS_EXECUTING ) return FALSE;
//	if ( ( side < 0 ) || ( pointer < 0 ) ) return FALSE; // 2004.06.15
	if ( ( side < 0 ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE; // 2004.06.15
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//--------------------------------------------------------------------
	if ( ( c < 0 ) || ( p < 0 ) ) return FALSE;
	//--------------------------------------------------------------------
	if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 0 , 2 ) > 1 ) return FALSE; // 2009.02.02
	//--------------------------------------------------------------------
//	if ( MULTI_RUNJOB_MtlInCassette(c)[p] != -1 ) { // 2012.04.01(Testing) // 2012.05.08
	if ( MULTI_RUNJOB_MtlInCassette(c)[p] > 0 ) { // 2012.04.01(Testing) // 2012.05.08
		if ( MULTI_RUNJOB_MtlInCassette(c)[p] != _i_SCH_CLUSTER_Get_PathIn( side , pointer ) ) {
			return FALSE;
		}
		if ( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) >= PM1 ) return FALSE;
	}
	//
	// 2002.06.24
	i = WfrID % 100;
	j = WfrID / 100;
	if ( ( i > 0 ) && ( j > 0 ) ) {
		nx = 0;
		if ( MULTI_RUNJOB_MtlInSlot(c)[p] == i ) {
			nx = j;
		}
		else if ( MULTI_RUNJOB_MtlInSlot(c)[p] == j ) {
			nx = i;
		}
	}
	else {
		nx = 0;
	}
	//--------------------------------------------------------------------
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( MULTI_RUNJOB_SelectCJIndex(i) == MULTI_RUNJOB_SelectCJIndex(c) ) {
			//
			if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
			if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
			//
			for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == -1 ) continue; // 2004.06.21 
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
					switch( MULTI_RUNJOB_MtlPJIDRes(i)[j] ) {
					case PJIDRES_READY :
					case PJIDRES_FIRST :
						break;
					case PJIDRES_RUNNING :
					case PJIDRES_LAST :
					case PJIDRES_FINISH :
						MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_RUNNING;
						//--------------------------------------------------------------------
						// 2002.06.24
						//--------------------------------------------------------------------
						if ( nx > 0 ) {
							for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(c) ; k++ ) {
								if ( MULTI_RUNJOB_MtlInSlot(c)[k] == nx ) {
									if ( MULTI_RUNJOB_MtlPJID(c)[k] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
										MULTI_RUNJOB_MtlPJIDRes(c)[k] = PJIDRES_RUNNING;
										break;
									}
								}
							}
						}
						//--------------------------------------------------------------------
						return FALSE;
						break;
					}
				}
			}
		}
	}
	MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_RUNNING;
	//--------------------------------------------------------------------
	// 2002.06.24
	//--------------------------------------------------------------------
	if ( nx > 0 ) {
		for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(c) ; k++ ) {
			if ( MULTI_RUNJOB_MtlInSlot(c)[k] == nx ) {
				if ( MULTI_RUNJOB_MtlPJID(c)[k] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
					MULTI_RUNJOB_MtlPJIDRes(c)[k] = PJIDRES_RUNNING;
					break;
				}
			}
		}
	}
	//--------------------------------------------------------------------
	return TRUE;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_PROCESSJOB_PROCESS_LAST_END_CHECK( int side , int pointer , int WfrID , int Next ) {
	int c , p , i , j , k , nx;
	if ( Next != 1 ) return FALSE;
//	if ( Sch_Multi_ControlJob_Select_Data->ControlStatus < CTLJOB_STS_EXECUTING ) return FALSE;
//	if ( ( side < 0 ) || ( pointer < 0 ) ) return FALSE; // 2004.06.15
	if ( ( side < 0 ) || ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE; // 2004.06.15
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//--------------------------------------------------------------------
	if ( ( c < 0 ) || ( p < 0 ) ) return FALSE;
	//--------------------------------------------------------------------
	if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 0 , 2 ) < _i_SCH_CLUSTER_Get_PathRun( side , pointer , 10 , 2 ) ) return FALSE; // 2009.02.16
	//--------------------------------------------------------------------
	// 2002.06.24
	//--------------------------------------------------------------------
	i = WfrID % 100;
	j = WfrID / 100;
	if ( ( i > 0 ) && ( j > 0 ) ) {
		nx = 0;
		if ( MULTI_RUNJOB_MtlInSlot(c)[p] == i ) {
			nx = j;
		}
		else if ( MULTI_RUNJOB_MtlInSlot(c)[p] == j ) {
			nx = i;
		}
	}
	else {
		nx = 0;
	}
	//--------------------------------------------------------------------
	MULTI_RUNJOB_MtlPJIDRes(c)[p] = PJIDRES_LAST;
	//--------------------------------------------------------------------
	// 2002.06.24
	//--------------------------------------------------------------------
	if ( nx > 0 ) {
		for ( k = 0 ; k < MULTI_RUNJOB_MtlCount(c) ; k++ ) {
			if ( MULTI_RUNJOB_MtlInSlot(c)[k] == nx ) {
				if ( MULTI_RUNJOB_MtlPJID(c)[k] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
					MULTI_RUNJOB_MtlPJIDRes(c)[k] = PJIDRES_LAST;
					break;
				}
			}
		}
	}
	//--------------------------------------------------------------------
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		if ( MULTI_RUNJOB_SelectCJIndex(i) == MULTI_RUNJOB_SelectCJIndex(c) ) {
			//
			if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue; // 2018.12.20
			if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2018.12.20
			//
			for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == -1 ) continue; // 2004.06.21 
				//============================================================================
				if ( MULTI_RUNJOB_MtlPJID(i)[j] == MULTI_RUNJOB_MtlPJID(c)[p] ) {
					if ( MULTI_RUNJOB_MtlPJIDRes(i)[j] == PJIDRES_READY ) {
						if ( MULTI_RUNJOB_MtlPJIDEnd(i)[j] != 1 ) {
							return FALSE;
						}
					}
					else if ( MULTI_RUNJOB_MtlPJIDRes(i)[j] < PJIDRES_LAST ) {
						return FALSE;
					}
				}
			}
		}
	}
	//===============================================================================================================
	// 2004.10.14
	//===============================================================================================================
	i = SCHMULTI_PROCESSJOB_GET_STATUS( MULTI_RUNJOB_MtlPJID(c)[p] , "" );
	if ( ( i < PRJOB_STS_PROCESSING ) || ( i == PRJOB_STS_PROCESSCOMPLETED ) || ( i == PRJOB_STS_ABORTING ) ) return FALSE;
	//===============================================================================================================
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//int SCHMULTI_RUNJOB_GET_PJINDEX( int side , int pathin , int slot ) { // 2014.10.31 // 2018.08.10
int SCHMULTI_RUNJOB_GET_PJINDEX( int side , int pathin , int slot , int *cjindex ) { // 2014.10.31 // 2018.08.10
	//
	int sr , r , i , x , count;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	if ( ( pathin < CM1 ) || ( pathin >= PM1 ) ) return -2;
	if ( ( slot < 1 ) || ( slot > MAX_CASSETTE_SLOT_SIZE ) ) return -3;
	//
	if ( _i_SCH_SYSTEM_CPJOB_GET( side ) != 1 ) return -4;
	//
	if ( SCH_Inside_ThisIs_BM_OtherChamber( pathin , 0 ) ) return -5;
	//
	sr = _i_SCH_SYSTEM_CPJOB_ID_GET( side );
	//
	for ( r = 0 ; r < MAX_MULTI_RUNJOB_SIZE ; r++ ) {
		//
		if ( sr != r ) {
			if ( MULTI_RUNJOB_RunStatus(r) < CASSRUNJOB_STS_WAITING ) continue;
			if ( !STRCMP( MULTI_RUNJOB_MtlCJName( sr ) , MULTI_RUNJOB_MtlCJName( r ) ) ) continue;
		}
		//
		count = MULTI_RUNJOB_MtlCount(r);
		//
		for ( i = 0 ; i < count ; i++ ) {
			//
			if ( MULTI_RUNJOB_MtlInCassette(r)[i] <= 0 ) {
				if ( pathin != ( MULTI_RUNJOB_Cassette(r) + CM1 ) ) continue;
			}
			else { // 2012.04.01(Testing)
				if ( pathin != MULTI_RUNJOB_MtlInCassette(r)[i] ) continue;
			}
			//
			if ( slot != MULTI_RUNJOB_MtlInSlot(r)[i] ) continue;
			//
			for ( x = 0 ; x < *Sch_Multi_ProcessJob_Size ; x++ ) {
				if ( STRCMP_L( MULTI_RUNJOB_MtlPJName( r )[i] , Sch_Multi_ProcessJob[x]->JobID ) ) break;
			}
			//
			if ( x != *Sch_Multi_ProcessJob_Size ) {
				if ( cjindex != NULL ) { // 2018.08.10
					*cjindex = MULTI_RUNJOB_SelectCJIndex(r); // 2018.08.10
				}
				return x;
			}
			//
			break;
		}
	}
	//
	return -1;
}

BOOL SCHMULTI_RUNJOB_HAS_TUNE_DATA( int pjindex ) { // 2014.10.30
	int i;
	//
	if ( Sch_Multi_ProcessJob[pjindex]->MtlRecipeMode != 0 ) {
		//
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE ; i++ ) {
			//
			if ( Sch_Multi_ProcessJob[pjindex]->MtlRcpTuneStep[i] >= 0 ) return TRUE;
			//
		}
		//
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) {
		//
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; i++ ) {
			//
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] > 0 ) return TRUE;
			//
		}
	}
	//
	return FALSE;
}


BOOL SCHMULTI_RUNJOB_DIFF_TUNE_DATA( int pjindex1 , int pjindex2 ) { // 2014.10.30
	//
	int i;
	//
	if ( ( Sch_Multi_ProcessJob[pjindex1]->MtlRecipeMode != 0 ) && ( Sch_Multi_ProcessJob[pjindex2]->MtlRecipeMode != 0 ) ) {
		//
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE ; i++ ) {
			//
			if ( Sch_Multi_ProcessJob[pjindex1]->MtlRcpTuneStep[i] != Sch_Multi_ProcessJob[pjindex2]->MtlRcpTuneStep[i] ) return TRUE;
			//
			if ( Sch_Multi_ProcessJob[pjindex1]->MtlRcpTuneStep[i] < 0 ) continue;
			//
			if ( Sch_Multi_ProcessJob[pjindex1]->MtlRcpTuneModule[i] != Sch_Multi_ProcessJob[pjindex2]->MtlRcpTuneModule[i] ) return TRUE;
			//
			if ( !STRCMP( Sch_Multi_ProcessJob[pjindex1]->MtlRcpTuneName[i] , Sch_Multi_ProcessJob[pjindex2]->MtlRcpTuneName[i] ) ) return TRUE;
			//
			if ( !STRCMP( Sch_Multi_ProcessJob[pjindex1]->MtlRcpTuneData[i] , Sch_Multi_ProcessJob[pjindex2]->MtlRcpTuneData[i] ) ) return TRUE;
			//
		}
		//
	}
	else { // 2014.11.27
		if ( ( Sch_Multi_ProcessJob[pjindex1]->MtlRecipeMode != 0 ) || ( Sch_Multi_ProcessJob[pjindex2]->MtlRecipeMode != 0 ) ) return TRUE;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) {
		//
		for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; i++ ) {
			//
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneModule[i] != Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex2]->MtlRcpTuneModule[i] ) return TRUE;
			//
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneModule[i] <= 0 ) continue;
			//
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneOrder[i] != Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex2]->MtlRcpTuneOrder[i] ) return TRUE;
			//
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneStep[i] != Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex2]->MtlRcpTuneStep[i] ) return TRUE;
			//
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneIndex[i] != Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex2]->MtlRcpTuneIndex[i] ) return TRUE;
			//
			if ( !STRCMP( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneName[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex2]->MtlRcpTuneName[i] ) ) return TRUE;
			//
			if ( !STRCMP( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex1]->MtlRcpTuneData[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex2]->MtlRcpTuneData[i] ) ) return TRUE;
			//
		}
	}
	//
	return FALSE;
	//
}

BOOL SCHMULTI_RUNJOB_CHECK_TUNING_INFO_SAME( int side1 , int pathin1 , int slot1 , int side2 , int pathin2 , int slot2 , int mode ) { // 2014.10.31
	//
	int pjindex1 , pjindex2;
	int cjindex1 , cjindex2; // 2018.08.10

	//===================================================================
	EnterCriticalSection( &CR_MULTIJOB );
	//===================================================================
//	pjindex1 = SCHMULTI_RUNJOB_GET_PJINDEX( side1 , pathin1 , slot1 ); // 2018.08.10
//	pjindex2 = SCHMULTI_RUNJOB_GET_PJINDEX( side2 , pathin2 , slot2 ); // 2018.08.10
	pjindex1 = SCHMULTI_RUNJOB_GET_PJINDEX( side1 , pathin1 , slot1 , &cjindex1 ); // 2018.08.10
	pjindex2 = SCHMULTI_RUNJOB_GET_PJINDEX( side2 , pathin2 , slot2 , &cjindex2 ); // 2018.08.10
	//===================================================================
	//
	// CJ/PJ 가 다를때 같이 공급되지 못하도록 Cluster Index 를 다르게 설정하는 부분
	//
	if ( ( pjindex1 >= 0 ) && ( pjindex2 >= 0 ) ) { // 2018.08.10
		//
		if ( ( mode == 1 ) || ( mode == 3 ) ) { // 2018.08.22
			//
			if ( cjindex1 != cjindex2 ) { // 2018.08.10
				//
				LeaveCriticalSection( &CR_MULTIJOB );
				//
				return FALSE;
				//
			}
			//
		}
		//
		if ( ( mode == 2 ) || ( mode == 3 ) ) { // 2018.08.22
			//
			if ( pjindex1 != pjindex2 ) { // 2018.08.10
				//
				LeaveCriticalSection( &CR_MULTIJOB );
				//
				return FALSE;
				//
			}
			//
		}
		//
	}
	//
	if ( ( pjindex1 >= 0 ) || ( pjindex2 >= 0 ) ) {
		//
		if      ( pjindex1 < 0 ) {
			//
			if ( SCHMULTI_RUNJOB_HAS_TUNE_DATA( pjindex2 ) ) {
				//
				LeaveCriticalSection( &CR_MULTIJOB );
				//
				return FALSE;
				//
			}
			//
		}
		else if ( pjindex2 < 0 ) {
			//
			if ( SCHMULTI_RUNJOB_HAS_TUNE_DATA( pjindex1 ) ) {
				//
				LeaveCriticalSection( &CR_MULTIJOB );
				//
				return FALSE;
				//
			}
			//
		}
		else {
			if ( pjindex1 != pjindex2 ) {
				//
				if ( SCHMULTI_RUNJOB_DIFF_TUNE_DATA( pjindex1 , pjindex2 ) ) {
					//
					LeaveCriticalSection( &CR_MULTIJOB );
					//
					return FALSE;
					//
				}
				//
			}
		}
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB );
	//
	return TRUE;
}












































BOOL SCHMULTI_RUNJOB_SET_TUNING_AREA_DELIMITER( int ch , int *scnt , int *ccnt , BOOL TuneDataYes , int applen , char *appstr , int errorcode ) {
	char *imsi[MAX_CHAMBER];
	if ( !TuneDataYes ) {
		*scnt = 256;
		//
		SCHMULTI_TUNE_INFO_DATA[ ch ] = calloc( *scnt + 1 , sizeof( char ) );
		if ( SCHMULTI_TUNE_INFO_DATA[ ch ] == NULL ) {
			_IO_CIM_PRINTF( "SCHMULTI_TUNE_INFO_DATA Memory Allocation 1 Fail %d PM%d:%d:%d\n" , errorcode , ch - PM1 + 1 , *scnt , 0 );
			return FALSE;
		}
		*ccnt = 1;
		strcpy( SCHMULTI_TUNE_INFO_DATA[ ch ] , "$" );
	}
	else {
		if ( *ccnt + 1 > *scnt ) { // 2007.06.08
			imsi[ch] = calloc( *ccnt + 1 , sizeof( char ) );
			if ( imsi[ch] == NULL ) {
				_IO_CIM_PRINTF( "SCHMULTI_TUNE_INFO_DATA Memory Allocation 2 Fail %d PM%d:%d:%d\n" , errorcode , ch - PM1 + 1 , *scnt , *ccnt );
				return FALSE;
			}
			//
			strcpy( imsi[ch] , SCHMULTI_TUNE_INFO_DATA[ ch ] );
			//
			free( SCHMULTI_TUNE_INFO_DATA[ ch ] );
			//
			*scnt = *scnt + 256;
			//
			SCHMULTI_TUNE_INFO_DATA[ ch ] = calloc( *scnt + 1 , sizeof( char ) );
			if ( SCHMULTI_TUNE_INFO_DATA[ ch ] == NULL ) {
				free( imsi[ch] );
				_IO_CIM_PRINTF( "SCHMULTI_TUNE_INFO_DATA Memory Allocation 3 Fail %d PM%d:%d:%d\n" , errorcode , ch - PM1 + 1 , *scnt , *ccnt );
				return FALSE;
			}
			strcpy( SCHMULTI_TUNE_INFO_DATA[ ch ] , imsi[ch] );
			free( imsi[ch] );
		}
		//================================================================
		*ccnt = *ccnt + 1;
		strcat( SCHMULTI_TUNE_INFO_DATA[ ch ] , "|" );
		//================================================================
	}
	if ( *ccnt + applen > *scnt ) {
		imsi[ch] = calloc( *ccnt + 1 , sizeof( char ) );
		if ( imsi[ch] == NULL ) {
			_IO_CIM_PRINTF( "SCHMULTI_TUNE_INFO_DATA Memory Allocation 4 Fail %d PM%d:%d:%d\n" , errorcode , ch - PM1 + 1 , *scnt , *ccnt );
			return FALSE;
		}
		//
		strcpy( imsi[ch] , SCHMULTI_TUNE_INFO_DATA[ ch ] );
		//
		free( SCHMULTI_TUNE_INFO_DATA[ ch ] );
		//
		*scnt = *ccnt + applen + 256; // 2007.11.28
		//
		SCHMULTI_TUNE_INFO_DATA[ ch ] = calloc( *scnt + 1 , sizeof( char ) );
		if ( SCHMULTI_TUNE_INFO_DATA[ ch ] == NULL ) {
			free( imsi[ch] );
			_IO_CIM_PRINTF( "SCHMULTI_TUNE_INFO_DATA Memory Allocation 5 Fail %d PM%d:%d:%d\n" , errorcode , ch - PM1 + 1 , *scnt , *ccnt );
			return FALSE;
		}
		strcpy( SCHMULTI_TUNE_INFO_DATA[ ch ] , imsi[ch] );
		free( imsi[ch] );
	}
	//==============================================================================
	*ccnt = *ccnt + applen;
	strcat( SCHMULTI_TUNE_INFO_DATA[ ch ] , appstr );
	//==============================================================================
	return TRUE;
}

BOOL SCHMULTI_RUNJOB_SET_TUNING_AREA_ANALISYS( char *strdata , int mode , BOOL fullmode , int ch , int curorder , char *Buffer , BOOL *TuneDataYes , int *scnt , int *ccnt , int errorcode ) {
	int z1 , z2 , x , i , j;
	int order;
	if ( strlen( strdata ) <= 0 ) return TRUE;
	z1 = 0;
	while( TRUE ) {
		//==========================================================================================================================================================
		if ( fullmode ) {
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( strdata , ':' , Buffer , z1 , 255 );
			if ( z1 == z2 ) break;
			z1 = z2;
			//
			if ( Buffer[0] == 0 ) order = 0;
			else                  order = atoi( Buffer );
		}
		else {
			order = 0;
		}
		//==========================================================================================================================================================
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( strdata , '|' , Buffer , z1 , 255 );
		if ( z1 == z2 ) break;
		z1 = z2;
		//==========================================================================================================================================================
		if ( fullmode ) {
			i = -1;
			j = 0;
			//
			if ( order > 0 ) {
				if ( mode == 1 ) { // post
					if ( order != ( curorder - 1 ) ) continue;
				}
				else {
					if ( order != ( curorder     ) ) continue;
				}
			}
			//
			if      ( Buffer[0] == 'R' ) { // pre
				if ( mode != 2 ) continue;
				x = 1;
			}
			else if ( Buffer[0] == 'T' ) { // post
				if ( mode != 1 ) continue;
				x = 1;
			}
			else if ( Buffer[0] == 'A' ) { // All
				x = 1;
			}
			else {
				if ( mode != 0 ) continue;
				x = 0;
			}
			//==========================================================================================================================================================
			if      ( Buffer[x+1] == ':' ) {
				if ( ( Buffer[x+0] >= '0' ) && ( Buffer[x+0] <= '9' ) ) {
					i = Buffer[x+0] - '0';
					j = x+2;
				}
			}
			else if ( Buffer[x+2] == ':' ) {
				if ( ( Buffer[x+0] >= '0' ) && ( Buffer[x+0] <= '9' ) ) {
					if ( ( Buffer[x+1] >= '0' ) && ( Buffer[x+1] <= '9' ) ) {
						i = ( ( Buffer[x+0] - '0' ) * 10 ) + ( Buffer[x+1] - '0' );
						j = x+3;
					}
				}
			}
		}
		else {
			i = 0;
			j = 0;
		}
		//==========================================================================================================================================================
		if ( ( i == 0 ) || ( i == ( ch - PM1 + 1 ) ) ) {
			//==============================================================================
			x = strlen( Buffer + j ); // 2007.06.08
			//==============================================================================
			if ( !SCHMULTI_RUNJOB_SET_TUNING_AREA_DELIMITER( ch , scnt , ccnt , *TuneDataYes , x , Buffer + j , errorcode ) ) return FALSE;
			//==============================================================================
			*TuneDataYes = TRUE;
		}
		//==========================================================================================================================================================
	}
	return TRUE;
}

BOOL SCHMULTI_RUNJOB_SET_TUNING_AREA_ANALISYS2( int pjindex , int mode , int ch , int curorder , char *Buffer , BOOL *TuneDataYes , int *scnt , int *ccnt , int errorcode ) {
	int i;
	int Org_Use;// 2008.05.30
	int TBuffer_Use;// 2008.05.30
	char TBuffer[16];// 2008.05.30
//printf( "=============================================\n0.[%d]\n" , pjindex );
	for ( i = 0 ; i < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; i++ ) {
//printf( "1.[%d][%d] [%d]\n" , pjindex , i , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] );
		//==========================================================================================================================================================
		if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] <= 0 ) continue;
		//==========================================================================================================================================================
		// 2008.05.30
		//===============================================================================
		TBuffer_Use = _SCH_COMMON_TUNING_GET_MORE_APPEND_DATA( mode , ch , curorder , pjindex , i ,
						Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] ,
						Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneOrder[i] ,
						Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneStep[i] ,
						Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneName[i] ,
						Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneData[i] ,
						Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneIndex[i] ,
						Buffer
						);
		Org_Use = TRUE;
		//===============================================================================
//printf( "2.[%d][%d] [%d]\n" , pjindex , i , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] );
		if ( ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] % 100 ) != ( ch - PM1 + 1 ) ) {
//			continue; // 2008.05.30
			if ( !TBuffer_Use ) continue; // 2008.05.30
			Org_Use = FALSE; // 2008.05.30
		}
		//==========================================================================================================================================================
//printf( "3.[%d][%d] [%d]\n" , pjindex , i , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] );
		if ( Org_Use ) { // 2008.05.30
			if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneOrder[i] > 0 ) {
				if ( mode == 1 ) { // post
					if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneOrder[i] != ( curorder - 1 ) ) {
//						continue; // 2008.05.30
						if ( !TBuffer_Use ) continue; // 2008.05.30
						Org_Use = FALSE; // 2008.05.30
					}
				}
				else {
					if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneOrder[i] != ( curorder     ) ) {
//						continue; // 2008.05.30
						if ( !TBuffer_Use ) continue; // 2008.05.30
						Org_Use = FALSE; // 2008.05.30
					}
				}
			}
		}
//printf( "4.[%d][%d] [%d]\n" , pjindex , i , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] );
		//==========================================================================================================================================================
		if ( Org_Use ) { // 2008.05.30
			switch ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] / 100 ) {
			case 1 : // pre
				if ( mode != 2 ) {
//					continue; // 2008.05.30
					if ( !TBuffer_Use ) continue; // 2008.05.30
					Org_Use = FALSE; // 2008.05.30
				}
				break;
			case 2 : // post
				if ( mode != 1 ) {
//					continue; // 2008.05.30
					if ( !TBuffer_Use ) continue; // 2008.05.30
					Org_Use = FALSE; // 2008.05.30
				}
				break;
			case 3 : // all
				break;
			default : // main
				if ( mode != 0 ) {
//					continue; // 2008.05.30
					if ( !TBuffer_Use ) continue; // 2008.05.30
					Org_Use = FALSE; // 2008.05.30
				}
				break;
			}
		}
//printf( "5.[%d][%d] [%d]\n" , pjindex , i , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] );
		//==========================================================================================================================================================
		if ( Org_Use ) { // 2008.05.30
			if ( TBuffer_Use ) { // 2008.05.30
				strcat( Buffer , "|" );																		sprintf( TBuffer , "%d:" , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneStep[i] );
				strcat( Buffer , TBuffer );
				strcat( Buffer , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneName[i] );
				strcat( Buffer , ":" );
				strcat( Buffer , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneData[i] );		sprintf( TBuffer , ":%d" , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneIndex[i] );
				strcat( Buffer , TBuffer );
			}
			else {
				sprintf( Buffer , "%d:%s:%s:%d" , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneStep[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneName[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneData[i] , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneIndex[i] );
			}
		}
		else {
			if ( !TBuffer_Use ) continue; // 2008.05.30
		}
		//==========================================================================================================================================================
		if ( !SCHMULTI_RUNJOB_SET_TUNING_AREA_DELIMITER( ch , scnt , ccnt , *TuneDataYes , strlen( Buffer ) , Buffer , errorcode ) ) return FALSE;
		//==========================================================================================================================================================
//printf( "6.[%d][%d] [%d][%s]\n" , pjindex , i , Sch_Multi_ProcessJob_Extend_RcpTuneArea[pjindex]->MtlRcpTuneModule[i] , Buffer );
		*TuneDataYes = TRUE;
		//==========================================================================================================================================================
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SCHMULTI_RUNJOB_DELETE_TUNING_INFO( BOOL all , int side , int pointer ) { // 2011.06.15
	int i , scnt , ccnt , Wafer;
//	char ed_cj[65] , ed_pj[65] , ed_ppid[65] , ed_wid[65]; // 2014.01.30
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256]; // 2014.01.30
	//
	if ( all ) return SCHMULTI_CPJOB_RESET();
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 91;
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return 91;

	scnt = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	ccnt = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	//
	if ( scnt != -99 ) return 92;
	//
	if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) return 93;
	//
	Wafer = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
	//
//	STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 64 ); // 2014.01.30
	STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 255 ); // 2014.01.30
	//
	if ( ed_pj[0] == 0 ) return 94;
	//
	EnterCriticalSection( &CR_MULTIJOB ); // 2013.02.13
	//
	ccnt = 0;
	scnt = 99;
	//
	while ( TRUE ) {
		//
		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			if ( STRCMP_L( ed_pj , Sch_Multi_ProcessJob[i]->JobID ) ) {
				if ( Sch_Multi_ProcessJob[i]->MtlSlot[0][0] == Wafer ) {
					break;
				}
			}
		}
		//
		if ( i == *Sch_Multi_ProcessJob_Size ) {
			//
			LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
			//
			return ( ( ccnt * 100 ) + scnt );
		}
		//
		scnt = SCHMULTI_PROCESSJOB_DELETE( -1 , i );
		//
		ccnt++;
		//
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
	//
	return 0;
}

BOOL SCHMULTI_RUNJOB_GET_TUNING_INFO( int mode , int side , int pointer , int ch , int slot , char *allrcp ) {
	int i , j , tunedatayes , scnt , ccnt , pjindex , pjindex_org , Wafer;
	char ReadBuffer[4096];
//	char Buffer[256]; // 2008.05.30
	char Buffer[4096]; // 2008.05.30
//	char ed_cj[65] , ed_pj[65] , ed_ppid[65] , ed_wid[65]; // 2011.06.15 // 2014.01.30
	char ed_cj[256] , ed_pj[256] , ed_ppid[256] , ed_wid[256]; // 2011.06.15 // 2014.01.30
	//
	//===================================================================
	// 2007.06.08
	//===================================================================
	SCHMULTI_TUNE_INFO_DATA_RESET( ch ); // 2007.10.19
	//===================================================================
	if ( ( side < 0 ) || ( pointer < 0 ) ) return FALSE;
	//===================================================================
	EnterCriticalSection( &CR_MULTIJOB ); // 2013.02.13
	//===================================================================
	// JOB DEFAULT DATA
	//===================================================================
	scnt = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , pointer );
	ccnt = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , pointer );
	Wafer = 0;
	//
	if ( scnt == -99 ) { // 2011.06.15 EIL
		//
		Wafer = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
		//
		if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) == NULL ) {
			pjindex = -1;
		}
		else {
//			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 64 ); // 2014.01.30
			STR_SEPERATE_CHAR4( _i_SCH_CLUSTER_Get_Ex_EILInfo( side , pointer ) , '|' , ed_cj , ed_pj , ed_ppid , ed_wid , 256 ); // 2014.01.30
			//
			if ( ed_pj[0] == 0 ) {
				pjindex = -1;
			}
			else {
				for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
					if ( STRCMP_L( ed_pj , Sch_Multi_ProcessJob[i]->JobID ) ) {
						if ( Sch_Multi_ProcessJob[i]->MtlSlot[0][0] == Wafer ) {
							break;
						}
					}
				}
				if ( i == *Sch_Multi_ProcessJob_Size ) {
					pjindex = -1;
				}
				else {
					pjindex = i;
				}
			}
			//
		}
	}
	else if ( ( scnt < 0 ) || ( ccnt < 0 ) ) {
		pjindex = -1;
	}
	else {
		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			if ( STRCMP_L( MULTI_RUNJOB_MtlPJName( scnt )[ccnt] , Sch_Multi_ProcessJob[i]->JobID ) ) break;
		}
		if ( i == *Sch_Multi_ProcessJob_Size ) {
			pjindex = -1;
		}
		else {
			pjindex = i;
		}
	}
	//===================================================================
	pjindex_org = pjindex; // 2011.06.24
	//===================================================================
	scnt = 0; // 2007.06.08
	ccnt = 0; // 2007.06.08
	//
	tunedatayes = FALSE;
	//===================================================================
	if ( ( mode == 0 ) && ( pjindex != -1 ) ) { // 2007.06.04 // 2007.11.28
		//
		while ( TRUE ) { // 2011.06.24
			//
			if ( Sch_Multi_ProcessJob[pjindex]->MtlRecipeMode != 0 ) {
				//===================================================================
				for ( j = 0 ; j < MAX_MULTI_PRJOB_RCPTUNE ; j++ ) {
					if (
						( Sch_Multi_ProcessJob[pjindex]->MtlRcpTuneModule[j] == ( ch - PM1 + 1 ) ) && // 2007.06.08
						( Sch_Multi_ProcessJob[pjindex]->MtlRcpTuneStep[j] >= 0 ) ) {
						sprintf( Buffer , "%d:%s:%s" ,
							Sch_Multi_ProcessJob[pjindex]->MtlRcpTuneStep[j] ,
							Sch_Multi_ProcessJob[pjindex]->MtlRcpTuneName[j] ,
							Sch_Multi_ProcessJob[pjindex]->MtlRcpTuneData[j] );
						//==============================================================================
						i = strlen( Buffer ); // 2007.06.08
						//==============================================================================
						if ( !SCHMULTI_RUNJOB_SET_TUNING_AREA_DELIMITER( ch , &scnt , &ccnt , tunedatayes , i , Buffer , 1 ) ) {
							//
							LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
							//
							return FALSE;
						}
						//================================================================
						tunedatayes = TRUE;
						//
					}
				}
			}
			//
			if ( Wafer == 0 ) break; // 2011.06.24
			//
			for ( i = ( pjindex + 1 ) ; i < *Sch_Multi_ProcessJob_Size ; i++ ) { // 2011.06.24
				if ( STRCMP_L( ed_pj , Sch_Multi_ProcessJob[i]->JobID ) ) {
					if ( Sch_Multi_ProcessJob[i]->MtlSlot[0][0] == Wafer ) {
						break;
					}
				}
			}
			if ( i == *Sch_Multi_ProcessJob_Size ) break;
			//
			pjindex = i;
			//
		}
	}
	//=================================================================================================
	// CLUSTER DATA // 2007.06.01
	//=================================================================================================
	if ( _i_SCH_CLUSTER_Get_ClusterTuneData( side , pointer ) != NULL ) {
		if ( !SCHMULTI_RUNJOB_SET_TUNING_AREA_ANALISYS(
				_i_SCH_CLUSTER_Get_ClusterTuneData( side , pointer ) ,
				mode , TRUE , ch , _i_SCH_CLUSTER_Get_PathDo( side , pointer ) ,
				Buffer , &tunedatayes , &scnt , &ccnt , 2 ) ) {
			//
			LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
			//
			return FALSE;
		}
	}
	//=================================================================================================
	// USER DATA
	//=================================================================================================
	strcpy( ReadBuffer , "" ); // 2007.10.18
	//
	if ( USER_RECIPE_TUNE_DATA_APPEND( side , mode , ch , slot , allrcp , ReadBuffer ) ) {
		if ( !SCHMULTI_RUNJOB_SET_TUNING_AREA_ANALISYS(
				ReadBuffer ,
				mode , FALSE , ch , _i_SCH_CLUSTER_Get_PathDo( side , pointer ) ,
				Buffer , &tunedatayes , &scnt , &ccnt , 3 ) ) {
			//
			LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
			//
			return FALSE;
		}
	}
	//=================================================================================================
	// RCPTUNE EXTEND DATA
	//=================================================================================================
	pjindex = pjindex_org; // 2011.06.24
	//
	if ( ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) && ( pjindex != -1 ) ) { // 2007.11.28
		//
		while ( TRUE ) { // 2011.06.24
			//
			if ( !SCHMULTI_RUNJOB_SET_TUNING_AREA_ANALISYS2(
					pjindex ,
					mode , ch , _i_SCH_CLUSTER_Get_PathDo( side , pointer ) ,
					Buffer , &tunedatayes , &scnt , &ccnt , 4 ) ) {
				//
				LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
				//
				return FALSE;
			}
			//
			if ( Wafer == 0 ) break; // 2011.06.24
			//
			for ( i = ( pjindex + 1 ) ; i < *Sch_Multi_ProcessJob_Size ; i++ ) { // 2011.06.24
				if ( STRCMP_L( ed_pj , Sch_Multi_ProcessJob[i]->JobID ) ) {
					if ( Sch_Multi_ProcessJob[i]->MtlSlot[0][0] == Wafer ) {
						break;
					}
				}
			}
			if ( i == *Sch_Multi_ProcessJob_Size ) break;
			//
			pjindex = i;
			//
		}
	}
	//
	LeaveCriticalSection( &CR_MULTIJOB ); // 2013.02.13
	//
	//=================================================================================================
	return tunedatayes;
	//=================================================================================================
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SCHMULTI_RECIPE_LOT_DATA_MODE( int side ) { // 2015.10.12
	//
/*
											READ_CLUSTER(M)		READ_CLUSTER(A)

N/A											O					O
Read Cluster,Dummy Recipe,Error=>Fail		X(F)				X(F)
Read Dummy Recipe Only,Error=>Fail			O					O
Read Cluster,Dummy Recipe,Error=>Skip		X(S)				X(S)
Read Dummy Recipe Only,Error=>Skip			O					O

N/A											O					O
Lot Recipe,Error=>Fail						X(F)				X(F)
Lot Recipe,Error=>Skip						X(S)				X(S)
N/A:(M)Lot Recipe,Error=>Fail				X(F)				O
N/A:(M)Lot Recipe,Error=>Skip				X(S)				O
Lot Recipe,Error=>Fail:(M)N/A				O					X(F)
Lot Recipe,Error=>Skip:(M)N/A				O					X(S)
*/

	switch( _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ) {
	case 0 :
		return 0;
		break;
	case 1 :
		return 1;
		break;
	case 2 :
		return 2;
		break;
		//
	case 3 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			return 1;
		else
			return 0;
		break;
	case 4 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			return 2;
		else
			return 0;
		break;
	case 5 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			return 0;
		else
			return 1;
		break;
	case 6 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			return 0;
		else
			return 2;
		break;
	}
	//
	return 0;
	//
}


//BOOL SCHMULTI_RECIPE_LOT_DATA_READ( int side , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate1G *LOT_RECIPE_GROUP , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , char *currgrp , int *mode ) { // 2015.10.12
//BOOL SCHMULTI_RECIPE_LOT_DATA_READ( int side , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate1G *LOT_RECIPE_GROUP , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , char *currgrp ) { // 2015.10.12 // 2016.07.13
BOOL SCHMULTI_RECIPE_LOT_DATA_READ( int side , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate1G *LOT_RECIPE_GROUP , LOTStepTemplate2 *LOT_RECIPE_DUMMY , LOTStepTemplate3 *LOT_RECIPE_SPECIAL , LOTStepTemplate_Dummy_Ex *LOT_RECIPE_DUMMY_EX , char *currgrp ) { // 2015.10.12 // 2016.07.13
	int i , cs , index;
	int l;
	char Buf_rec1[256];
	char Buf_rec2[256];

	//-----------------------------------------------------------------
	index = _i_SCH_SYSTEM_CPJOB_ID_GET( side );
	//-----------------------------------------------------------------
	/*
	// 
	// 2015.10.12
	//
//	*mode = SCHMULTI_DEFAULT_LOTRECIPE_READ_GET(); // 2003.09.24 // 2012.06.28
	//
//	switch( SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() ) { // 2012.06.28 // 2014.06.23
	switch( _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ) { // 2012.06.28 // 2014.06.23
	case 0 :
		*mode = 0;
		break;
	case 1 :
		*mode = 1;
		break;
	case 2 :
		*mode = 2;
		break;
		//
	case 3 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			*mode =	1;
		else
			*mode = 0;
		break;
	case 4 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			*mode =	2;
		else
			*mode = 0;
		break;
	case 5 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			*mode =	0;
		else
			*mode = 1;
		break;
	case 6 :
		if ( SCHMULTI_RUNJOB_GET_DATABASE_J_START( side ) )
			*mode =	0;
		else
			*mode = 2;
		break;
	default :
		*mode = 0;
		break;
	}
	//
	*/
	//-----------------------------------------------------------------
	// Initial
	strcpy( LOT_RECIPE->LOT_ID , "" );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		strcpy( LOT_RECIPE->CLUSTER_RECIPE[i] , "" );
		strcpy( LOT_RECIPE->CLUSTER_RECIPE2[i] , "" );
		LOT_RECIPE->CLUSTER_USING[i] = 1; // Not Use
		//
		LOT_RECIPE_SPECIAL->SPECIAL_INSIDE_DATA[i] = 0; // 2004.11.16
		//
		strcpy( LOT_RECIPE_SPECIAL->SPECIAL_USER_DATA[i] , "" ); // 2005.01.24
		//
		LOT_RECIPE_SPECIAL->USER_CONTROL_MODE[i] = 0; // 2011.12.08
		strcpy( LOT_RECIPE_SPECIAL->USER_CONTROL_DATA[i] , "" ); // 2011.12.08
		//
		LOT_RECIPE_SPECIAL->USER_SLOTFIX_MODE[i] = 0; // 2015.10.12
		LOT_RECIPE_SPECIAL->USER_SLOTFIX_SLOT[i] = 0; // 2015.10.12
		//
	}
	//
	LOT_RECIPE->CLUSTER_EXTRA_STATION = 0;
	LOT_RECIPE->CLUSTER_EXTRA_TIME    = 0;
	//
	strcpy( LOT_RECIPE_DUMMY->CLUSTER_RECIPE , "" );
	LOT_RECIPE_DUMMY->CLUSTER_USING = 0;
	//
	// 2016.07.13
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		strcpy( LOT_RECIPE_DUMMY_EX->CLUSTER_RECIPE[i] , "" );
		LOT_RECIPE_DUMMY_EX->CLUSTER_USING[i] = 0;
	}
	//
	for ( i = 0 ; i < MULTI_RUNJOB_MtlCount(index) ; i++ ) {
		cs = MULTI_RUNJOB_MtlInSlot(index)[i];
		//
		LOT_RECIPE->CLUSTER_USING[cs-1] = 0;
		//
//		strcpy( LOT_RECIPE->CLUSTER_RECIPE[cs-1] , MULTI_RUNJOB_MtlRecipeName( index )[i] ); // 2009.09.28
		//
		STR_SEPERATE_CHAR( MULTI_RUNJOB_MtlRecipeName( index )[i] , ':' , Buf_rec1 , Buf_rec2 , 255 );
		l = strlen( Buf_rec2 );
		if ( l > 0 ) {
			if ( ( Buf_rec2[0] == '(' ) && ( Buf_rec2[l-1] == ')' ) ) {
				 Buf_rec2[l-1] = 0;
				l = 1;
			}
			else {
				l = 0;
			}
		}
		//
		strcpy( LOT_RECIPE->CLUSTER_RECIPE[cs-1] , Buf_rec1 ); // 2009.09.28
		//
		if ( l != 0 ) { // 2009.09.28
			if ( !STRCMP_L( currgrp , Buf_rec2 + 1 ) ) { // 2009.09.28
				strcpy( LOT_RECIPE_GROUP->CLUSTER_GROUP[cs-1] , Buf_rec2 + 1 ); // 2009.09.28
				LOT_RECIPE_GROUP->CLUSTER_USING[cs-1] = 1;
			}
		}
		//
	}
	return TRUE;
}


BOOL SCHMULTI_RECIPE_LOT_DATA_REMAP( int side , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate3 *LOT_RECIPE_SPECIAL ) { // 2015.10.12
	int i , j , index;
	//-----------------------------------------------------------------
	index = _i_SCH_SYSTEM_CPJOB_ID_GET( side );
	//-----------------------------------------------------------------
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( LOT_RECIPE->CLUSTER_USING[i] != 0 ) continue;
		//
		if ( LOT_RECIPE_SPECIAL->USER_SLOTFIX_MODE[i] != 1 ) continue;
		//
		if ( LOT_RECIPE_SPECIAL->USER_SLOTFIX_SLOT[i] == 0 ) continue;
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(index) ; j++ ) {
			//
			if ( LOT_RECIPE_SPECIAL->USER_SLOTFIX_SLOT[i] == MULTI_RUNJOB_MtlInSlot(index)[j] ) break;
			//
		}
		//
		if ( j != MULTI_RUNJOB_MtlCount(index) ) continue;
		//
		LOT_RECIPE->CLUSTER_USING[i] = 1;
		//
	}
	return TRUE;
}


BOOL SCHMULTI_RECIPE_LOT_DATA_SINGLE_READ( int side , int start , char *lotname , char *pjname , int *slot , int *nstart ) {
	int i , index;

	index = _i_SCH_SYSTEM_CPJOB_ID_GET( side ); // 2007.11.30

	if ( start == 0 ) {
		if ( MULTI_RUNJOB_MtlCount(index) <= 1 ) {
			*slot = -1;
			strcpy( lotname , MULTI_RUNJOB_MtlRecipeName( index )[0] );
			strcpy( pjname  , MULTI_RUNJOB_MtlPJName( index )[0] ); // 2011.09.20
			*nstart = start + 1;
			return TRUE;
		}
		else {
			for ( i = 1 ; i < MULTI_RUNJOB_MtlCount(index) ; i++ ) {
				if ( !STRCMP_L( MULTI_RUNJOB_MtlRecipeName( index )[0] , MULTI_RUNJOB_MtlRecipeName( index )[i] ) ) {
					break;
				}
			}
			if ( i >= MULTI_RUNJOB_MtlCount(index) ) {
				*slot = -1;
				strcpy( lotname , MULTI_RUNJOB_MtlRecipeName( index )[0] );
				strcpy( pjname  , MULTI_RUNJOB_MtlPJName( index )[0] ); // 2011.09.20
				*nstart = start + 1;
				return TRUE;
			}
		}
	}

	if ( start >= MULTI_RUNJOB_MtlCount(index) ) return FALSE;

	*slot = MULTI_RUNJOB_MtlInSlot(index)[start];

	strcpy( lotname , MULTI_RUNJOB_MtlRecipeName( index )[start] );
	strcpy( pjname  , MULTI_RUNJOB_MtlPJName( index )[start] ); // 2011.09.20

	*nstart = start + 1;

	return TRUE;
}

BOOL SCHMULTI_RECIPE_LOT_DATA_SINGLE_GET_NAME( int side , char *lotname , char *pjname ) { // 2015.10.12
	int i , index;
	//
	index = _i_SCH_SYSTEM_CPJOB_ID_GET( side );
	//
	if ( MULTI_RUNJOB_MtlCount(index) <= 1 ) {
		strcpy( lotname , MULTI_RUNJOB_MtlRecipeName( index )[0] );
		strcpy( pjname  , MULTI_RUNJOB_MtlPJName( index )[0] );
		return TRUE;
	}
	else {
		for ( i = 1 ; i < MULTI_RUNJOB_MtlCount(index) ; i++ ) {
			if ( !STRCMP_L( MULTI_RUNJOB_MtlRecipeName( index )[0] , MULTI_RUNJOB_MtlRecipeName( index )[i] ) ) {
				break;
			}
		}
		if ( i >= MULTI_RUNJOB_MtlCount(index) ) {
			strcpy( lotname , MULTI_RUNJOB_MtlRecipeName( index )[0] );
			strcpy( pjname  , MULTI_RUNJOB_MtlPJName( index )[0] );
			return TRUE;
		}
	}
	//
	return FALSE;
	//
}

//=============================================================================
void SCHMULTI_RUNJOB_PJID_DATA_REBUILD() { // 2005.05.19
	int i , j , k;
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( MULTI_RUNJOB_RunStatus(i) == CASSRUNJOB_STS_READY ) continue;
		if ( MULTI_RUNJOB_RunStatus(i) >= CASSRUNJOB_STS_FINISH_WITH_NORMAL ) continue; // 2011.04.28
		//
		for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
			k = SCHMULTI_PROCESSJOB_Find( MULTI_RUNJOB_MtlPJName( i )[j] );
			if ( k != -1 ) {
				MULTI_RUNJOB_MtlPJID(i)[j] = k;
			}
//			else {
//				printf( "ERROR Find PRJOB %s Error\n" , MULTI_RUNJOB_MtlPJName( i )[j] );
//			}
		}
		//
	}
}
//=============================================================================
//=============================================================================
//=============================================================================
void SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( int ch ) { // 2008.04.11
	char Buffer[127+1];
	switch( _i_SCH_PRM_GET_FA_MID_READ_POINT( ch ) ) {
	case 3 :
		IO_GET_MID_NAME( ch , Buffer );
		SCHMULTI_CASSETTE_MID_SET( ch , Buffer );
		break;
	default :
		IO_GET_MID_NAME_FROM_READ( ch , Buffer );
		SCHMULTI_CASSETTE_MID_SET( ch , Buffer );
		break;
	}
}
//=============================================================================
//void SCHMULTI_CASSETTE_VERIFY_MID_READ_SET( int ch ) { // 2008.04.11 // 2010.05.06
//	EnterCriticalSection( &CR_MULTIJOB ); // 2010.05.06
//	SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( ch ); // 2010.05.06
//	LeaveCriticalSection( &CR_MULTIJOB ); // 2010.05.06
//} // 2010.05.06
//=============================================================================

BOOL  SCHMULTI_CASSETTE_VERIFY_MAKE_SET_AGAIN( int Cass ) { // 2017.05.22
	if ( SCHMULTI_CASSETTE_VERIFY_GET( Cass ) == CASSETTE_VERIFY_RUNNING ) {
		SCHMULTI_CASSETTE_VERIFY_SET( Cass , CASSETTE_VERIFY_CONFIRMED2 );
		return TRUE;
	}
	return FALSE;
}
//


BOOL SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( int ch , int actmode , int mode , char *data , int esrcid ) { // 2008.04.11
	EnterCriticalSection( &CR_MULTIJOB );
	//
	if      ( SCHMULTI_CASSETTE_VERIFY_GET( ch ) == CASSETTE_VERIFY_READY ) {
		//
		if ( actmode == 0 ) SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( ch ); // 2010.03.25
		//
		if ( actmode != 3 ) { // 2011.03.08
			//
			if ( actmode == 4 ) { // 2012.04.03
				//
				SCHMULTI_CASSETTE_VERIFY_SET( ch , CASSETTE_VERIFY_CONFIRMED );
				//
				if      ( mode == 1 ) {
					if ( !STRCMP_L( SCHMULTI_CASSETTE_MID_GET_FOR_START( ch ) , data ) ) {
						LeaveCriticalSection( &CR_MULTIJOB );
						return FALSE;
					}
					//
					if ( esrcid > 0 ) { // 2019.02.28
						if ( data[0] == 0 ) {
							if ( _i_SCH_IO_GET_MODULE_SOURCE_E( ch + CM1 , -1 ) != esrcid ) {
								LeaveCriticalSection( &CR_MULTIJOB );
								return FALSE;
							}
						}
					}
					//
				}
				else if ( mode == 2 ) {
					strcpy( data , SCHMULTI_CASSETTE_MID_GET_FOR_START( ch ) );
				}
				//
				LeaveCriticalSection( &CR_MULTIJOB );
				return TRUE;
			}
			else {
				if ( strlen( SCHMULTI_CASSETTE_MID_GET( ch ) ) > 0 ) {
					SCHMULTI_CASSETTE_VERIFY_SET( ch , CASSETTE_VERIFY_WAIT_CONFIRM );
					SCHMULTI_CASSETTE_VERIFY_WAIT_MESSAGE( ch , SCHMULTI_CASSETTE_MID_GET( ch ) );
				}
			}
		}
		//
	}
	else if ( SCHMULTI_CASSETTE_VERIFY_GET( ch ) == CASSETTE_VERIFY_CONFIRMING ) {
		//
		if ( actmode == 0 ) { // 2010.05.14
			//
			if ( strlen( SCHMULTI_CASSETTE_MID_GET( ch ) ) <= 0 ) {
				SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( ch ); // 2010.05.14
			}
			//
		}
		//
		if ( actmode != 3 ) { // 2011.03.08
			SCHMULTI_CASSETTE_VERIFY_SET( ch , CASSETTE_VERIFY_CONFIRMED );
		}
		//
	}
	else if (
		( SCHMULTI_CASSETTE_VERIFY_GET( ch ) == CASSETTE_VERIFY_CONFIRMED ) ||
		( SCHMULTI_CASSETTE_VERIFY_GET( ch ) == CASSETTE_VERIFY_CONFIRMED2 ) ) {
		//
		if ( actmode == 0 ) { // 2010.05.14
			//
			if ( strlen( SCHMULTI_CASSETTE_MID_GET( ch ) ) <= 0 ) {
				SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( ch ); // 2010.05.14
			}
			//
		}
		//
		if      ( mode == 1 ) {
			if ( !STRCMP_L( SCHMULTI_CASSETTE_MID_GET_FOR_START( ch ) , data ) ) {
				LeaveCriticalSection( &CR_MULTIJOB );
				return FALSE;
			}
			//
			if ( esrcid > 0 ) { // 2019.02.28
				if ( data[0] == 0 ) {
					if ( _i_SCH_IO_GET_MODULE_SOURCE_E( ch + CM1 , -1 ) != esrcid ) {
						LeaveCriticalSection( &CR_MULTIJOB );
						return FALSE;
					}
				}
			}
			//
		}
		else if ( mode == 2 ) {
			strcpy( data , SCHMULTI_CASSETTE_MID_GET_FOR_START( ch ) );
		}
		//
		LeaveCriticalSection( &CR_MULTIJOB );
		return TRUE;
	}
	else if ( SCHMULTI_CASSETTE_VERIFY_GET( ch ) == CASSETTE_VERIFY_WAIT_CONFIRM ) {
		// N/A
		if ( actmode == 4 ) { // 2012.04.03
			//
			SCHMULTI_CASSETTE_VERIFY_SET( ch , CASSETTE_VERIFY_CONFIRMING );
			//
		}
		//
	}
	else if ( SCHMULTI_CASSETTE_VERIFY_GET( ch ) == CASSETTE_VERIFY_RUNNING ) {
		// N/A
		if ( ( actmode == 2 ) || ( actmode == 3 ) ) { // 2011.03.08
			//
			if      ( mode == 1 ) {
				if ( !STRCMP_L( SCHMULTI_CASSETTE_MID_GET_FOR_START( ch ) , data ) ) {
					LeaveCriticalSection( &CR_MULTIJOB );
					return FALSE;
				}
				//
				if ( esrcid > 0 ) { // 2019.02.28
					if ( data[0] == 0 ) {
						if ( _i_SCH_IO_GET_MODULE_SOURCE_E( ch + CM1 , -1 ) != esrcid ) {
							LeaveCriticalSection( &CR_MULTIJOB );
							return FALSE;
						}
					}
				}
				//
			}
			else if ( mode == 2 ) {
				strcpy( data , SCHMULTI_CASSETTE_MID_GET_FOR_START( ch ) );
			}
			//
			LeaveCriticalSection( &CR_MULTIJOB );
			return TRUE;
		}
		//
	}
	LeaveCriticalSection( &CR_MULTIJOB );
	return FALSE;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_ALL_QUEUE_DELETE_CHECK() { // 2016.06.10
	int i , c;
	//
	if ( !SCHMULTI_CTJOB_USE_GET() ) return;
	//
	c = ( *Sch_Multi_ControlJob_Size - 1);
	//
	for ( i = c ; i >= 0 ; i-- ) {
		//
		SCHMULTI_CONTROLJOB_DELETE( i );
		//
	}
	//
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//void SCHMULTI_ALL_FORCE_CHECK( int Cass ) { // 2004.10.14 // 2018.08.30
void SCHMULTI_ALL_FORCE_CHECK( int Side ) { // 2004.10.14 // 2018.08.30
	int i , j;
	if ( !SCHMULTI_CTJOB_USE_GET() ) return;
	for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
		if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_EXECUTING ) {
//			if ( SCHMULTI_RUNJOB_ALL_FINISH( Cass , i , &j ) ) { // 2018.08.30
			if ( SCHMULTI_RUNJOB_ALL_FINISH( Side , i , &j ) ) { // 2018.08.30
//				SCHMULTI_PROCESSJOB_FORCE_FINISH_CHECK( i ); // 2008.03.28
				if ( SCHMULTI_PROCESSJOB_FORCE_FINISH_CHECK( i ) ) Sleep(250); // 2008.03.28
				SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_FINISH , j , Sch_Multi_ControlJob_Select_Data[i]->JobID , -1 , -1 );
				Sleep(250); // 2008.03.28
				SCHMULTI_CONTROLJOB_DELSELECT( 1 , i , 0 , FALSE );
			}
		}
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int SchMulti_Controller_Run_Status = 0;

int SchMulti_Controller_Flow_Log   = 0;
int SchMulti_Controller_Flow_Log_Max = 0;
//---------------------------------------------------------------------------------------
void SCHMULTI_CONTROLLER_LOGGING( int cnt ) {
	//
	SCHEDULING_DATA_STRUCTURE_LOG( "" , "" , -1 , 0 , 0 ); // 2016.01.12
	SCHEDULING_DATA_STRUCTURE_JOB_LOG( "" , "" );
	//
	if ( cnt <= 0 ) SchMulti_Controller_Flow_Log_Max = 256;
	else            SchMulti_Controller_Flow_Log_Max = cnt;
	//
	SchMulti_Controller_Flow_Log = 1;
	//
}

void SCHMULTI_CONTROLLER_LOGSET( int Mode , int data ) { // 2011.03.03
	//
	if ( SchMulti_Controller_Flow_Log == 0 ) return;
	//
//	_IO_CIM_PRINTF( "SCHMT[%d] : [%d] = %d\n" , SchMulti_Controller_Flow_Log , Mode , data );
	/*
	// 2016.01.12
	_IO_CIM_PRINTF( "SCHMT[%d] : [%d] = %d  (%d,%d,%d)(%d,%d,%d)(%d,%d,%d)(%d,%d,%d)\n" , SchMulti_Controller_Flow_Log , Mode , data // 2011.05.11
		, BUTTON_GET_FLOW_STATUS_VAR(0) , BUTTON_GET_FLOW_IO_STATUS(0) , _i_SCH_IO_GET_MAIN_BUTTON(0)
		, BUTTON_GET_FLOW_STATUS_VAR(1) , BUTTON_GET_FLOW_IO_STATUS(1) , _i_SCH_IO_GET_MAIN_BUTTON(1)
		, BUTTON_GET_FLOW_STATUS_VAR(2) , BUTTON_GET_FLOW_IO_STATUS(2) , _i_SCH_IO_GET_MAIN_BUTTON(2)
		, BUTTON_GET_FLOW_STATUS_VAR(3) , BUTTON_GET_FLOW_IO_STATUS(3) , _i_SCH_IO_GET_MAIN_BUTTON(3)
		);
	//
	*/

	// 2016.01.12
	_IO_CIM_PRINTF( "SCHMT[%d] : [%d] = %d (%d,%d,%d,%d,%d)(%d,%d,%d,%d,%d)(%d,%d,%d,%d,%d)(%d,%d,%d,%d,%d)\n" , SchMulti_Controller_Flow_Log , Mode , data // 2011.05.11
		, _i_SCH_SYSTEM_USING_GET(0) , _i_SCH_SYSTEM_MODE_GET(0) , BUTTON_GET_FLOW_STATUS_VAR(0) , BUTTON_GET_FLOW_IO_STATUS(0) , _i_SCH_IO_GET_MAIN_BUTTON(0)
		, _i_SCH_SYSTEM_USING_GET(1) , _i_SCH_SYSTEM_MODE_GET(1) , BUTTON_GET_FLOW_STATUS_VAR(1) , BUTTON_GET_FLOW_IO_STATUS(1) , _i_SCH_IO_GET_MAIN_BUTTON(1)
		, _i_SCH_SYSTEM_USING_GET(2) , _i_SCH_SYSTEM_MODE_GET(2) , BUTTON_GET_FLOW_STATUS_VAR(2) , BUTTON_GET_FLOW_IO_STATUS(2) , _i_SCH_IO_GET_MAIN_BUTTON(2)
		, _i_SCH_SYSTEM_USING_GET(3) , _i_SCH_SYSTEM_MODE_GET(3) , BUTTON_GET_FLOW_STATUS_VAR(3) , BUTTON_GET_FLOW_IO_STATUS(3) , _i_SCH_IO_GET_MAIN_BUTTON(3)
		);
	//
	SchMulti_Controller_Flow_Log++;
	//
	if ( SchMulti_Controller_Flow_Log >= SchMulti_Controller_Flow_Log_Max ) SchMulti_Controller_Flow_Log = 0;
	//
}


void SCHMULTI_CONTROLLER_BUTTON() {
//	int i , j , imb , ims , sms , check; // 2018.11.07
	int i , imb , ims , sms , check; // 2018.11.07
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			ims = BUTTON_GET_FLOW_IO_STATUS( i );
			sms = BUTTON_GET_FLOW_STATUS_VAR( i );
			//
			if ( sms != _MS_5_MAPPED ) {
				if ( ims != sms ) {
					BUTTON_SET_FLOW_STATUS_EQUAL( i );
				}
			}
			else {
				if ( ims == sms ) {
					//
// 2018.11.07
//					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 2 ) { // 2013.07.02
//						//
//						if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
//							//
//							for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
//								//
//								if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
//								//
//								if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != ( i + CM1 ) ) continue;
//								//
//								BUTTON_SET_FLOW_IO_STATUS( i , _MS_12_RUNNING );
//								//
//								break;
//							}
//							//
//						}
//						//
//					}
//					else {
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i+CM1 ) > 0 ) {
							BUTTON_SET_FLOW_IO_STATUS( i , _MS_12_RUNNING );
						}
//					}
					//
				}
				else {
					//
// 2018.11.07
//					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 2 ) { // 2013.07.02
//						//
//						if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
//							//
//							for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
//								//
//								if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
//								//
//								if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != ( i + CM1 ) ) continue;
//								//
//								break;
//							}
//							//
//							if ( j == MAX_CASSETTE_SLOT_SIZE ) BUTTON_SET_FLOW_STATUS_EQUAL( i );
//						}
//						else {
//							BUTTON_SET_FLOW_STATUS_EQUAL( i );
//						}
//						//
//					}
//					else {
						//
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i+CM1 ) <= 0 ) BUTTON_SET_FLOW_STATUS_EQUAL( i );
						//
//					}
					//
				}
			}
		}
	}
	else {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			imb = _i_SCH_IO_GET_MAIN_BUTTON( i );
			ims = BUTTON_GET_FLOW_IO_STATUS( i );
			sms = BUTTON_GET_FLOW_STATUS_VAR( i );
			//
			if ( ( imb == CTC_IDLE ) && ( sms == _MS_5_MAPPED ) && ( ims == _MS_11_WAITING ) ) continue; // 2011.05.11
			if ( ( imb == CTC_WAITING ) && ( sms != _MS_11_WAITING ) && ( ims == _MS_11_WAITING ) ) continue;
			//
			if ( ims == sms ) {
				//
				switch( imb ) {
				case CTC_RUNNING :
				case CTC_PAUSING :
				case CTC_PAUSED :
				case CTC_ABORTING :
					//
					switch( sms ) {
					case _MS_0_IDLE : // Init
					case _MS_1_LOADING : // Loading
					case _MS_2_LOADED : // Loading(Success)
					case _MS_3_LOADFAIL : // Loading(Fail)
					case _MS_4_MAPPING : // Mapping
					case _MS_5_MAPPED : // Mapping(Success)
					case _MS_6_MAPFAIL : // Mapping(Fail)
					case _MS_15_COMPLETE : // Running(Success)
					case _MS_16_CANCEL : // Running(Success with Cancel)
					case _MS_17_ABORTED : // Running(Aborted)
					case _MS_18_UNLOADING_C :
					case _MS_19_UNLOADING : // Unloading
					case _MS_20_UNLOADED_C :
					case _MS_21_UNLOADED : // Unloading(Success)
					case _MS_22_UNLOADFAIL : // Unloading(Fail)
						//
						if ( ( GetTickCount() - STATUS_BUTTON_CHECK[i] ) >= 500 ) {
							STATUS_BUTTON_CHECK[i] = GetTickCount();
							BUTTON_SET_FLOW_IO_STATUS( i , _MS_12_RUNNING );
						}
						//
						break;
					}
					break;
				}
				//
			}
			else {
				//
				check = FALSE;
				//
				switch( imb ) {
				case CTC_RUNNING :
				case CTC_PAUSING :
				case CTC_PAUSED :
				case CTC_ABORTING :
					//
					switch( sms ) {
					case _MS_0_IDLE : // Init
					case _MS_1_LOADING : // Loading
					case _MS_2_LOADED : // Loading(Success)
					case _MS_3_LOADFAIL : // Loading(Fail)
					case _MS_4_MAPPING : // Mapping
					case _MS_5_MAPPED : // Mapping(Success)
					case _MS_6_MAPFAIL : // Mapping(Fail)
					case _MS_15_COMPLETE : // Running(Success)
					case _MS_16_CANCEL : // Running(Success with Cancel)
					case _MS_17_ABORTED : // Running(Aborted)
					case _MS_18_UNLOADING_C :
					case _MS_19_UNLOADING : // Unloading
					case _MS_20_UNLOADED_C :
					case _MS_21_UNLOADED : // Unloading(Success)
					case _MS_22_UNLOADFAIL : // Unloading(Fail)
						//
						if ( ( GetTickCount() - STATUS_BUTTON_CHECK[i] ) >= 500 ) {
							check = TRUE;
						}
						//
						break;
					default :
						check = TRUE;
						break;
					}
					break;
				default :
					check = TRUE;
					break;
				}
				//
				if ( check ) {
					//
					BUTTON_SET_FLOW_STATUS_EQUAL( i );
					//
					STATUS_BUTTON_CHECK[i] = GetTickCount();
				}
				//
			}
		}
	}
}

BOOL PRE_RUN_CHECK_RESET = FALSE; // 2016.03.28

void SCHMULTI_CONTROLLER() {
	int i , s , Mode , Mode2 , Res , l , carrmode , outcassmode , sres , f , findside;
	int movemode; // 2013.09.03
	//, sres2; // 2009.07.15
//	char Buffer[127+1]; // 2008.04.11
//	BOOL MID_READ_COUNT[MAX_CASSETTE_SIDE]; // 2006.12.05
//	BOOL PRE_RUN_POSSIBLE_CHECK[MAX_CASSETTE_SIDE]; // 2004.06.16 // 2006.12.05
//	BOOL CPS , CPW; // 2003.03.21
	int StartMode;
	int s_side , e_side; // 2009.03.03
	int fres; // 2011.03.04
	int ovcnt = 0; // 2012.04.19

	//======================================================
	// 2006.12.05
	//======================================================
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
//		MID_READ_COUNT[i] = FALSE; // 2010.05.06
		PRE_RUN_POSSIBLE_CHECK[i] = 0;
		//
		STATUS_BUTTON_CHECK[i] = GetTickCount(); // 2011.04.14
		//
	}
	//======================================================
	Scheduler_HandOffOut_Clear(); // 2011.06.01

	SchMulti_Controller_Run_Status = 0;

	Sleep(3000);

	SchMulti_Controller_Run_Status = 1;

	while ( TRUE ) {
		//
		SCHMULTI_CONTROLLER_LOGSET( 1 , 0 );
		//
		//--------------------------------------------------------------
		SCHMULTI_CONTROLJOB_SELECT( &SCHMULTI_CURRENT_MANUAL_MODE );
		//--------------------------------------------------------------
		if ( SCHMULTI_CONTROLJOB_GET_SELECT_STATUS( &Mode ) ) {
			//
			SCHMULTI_CONTROLLER_LOGSET( 11 , Mode );
			//
			SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_SELECT , 0 , Sch_Multi_ControlJob_Select_Data[Mode]->JobID , -1 , -1 );
			//
			SCHMULTI_CONTROLLER_LOGSET( 12 , Mode );
			//
			if ( SCHMULTI_CURRENT_MANUAL_MODE ) {
				//
				SCHMULTI_CONTROLLER_LOGSET( 13 , Mode );
				//
				SCHMULTI_CONTROLJOB_SET_RUN_STATUS( Mode );
				SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_START , 0 , Sch_Multi_ControlJob_Select_Data[Mode]->JobID , -1 , -1 );
			}
			SchMulti_Controller_Run_Status = 2;
			//CPS = FALSE; // 2003.03.21
			//CPW = FALSE; // 2003.03.21
			//------------------------------------------
			// Control Job Start
			//------------------------------------------
			//------------------------------------------
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) PRE_RUN_POSSIBLE_CHECK[ i ] = 0;
			SCHMULTI_AFTER_SELECT_CHECK_SET( 0 ); // 2004.06.26
			//------------------------------------------
			while ( SCHMULTI_CONTROLJOB_GET_SELECT_STATUS( &Mode ) ) {
				//
				if ( PRE_RUN_CHECK_RESET ) { // 2016.03.28
					PRE_RUN_CHECK_RESET = FALSE;
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) PRE_RUN_POSSIBLE_CHECK[ i ] = 0;
				}
				//
				SCHMULTI_CONTROLLER_LOGSET( 21 , Mode );
				//
				//====================================================================================
				//====================================================================================
				// 2004.06.01
				//====================================================================================
				//====================================================================================
				if ( SCHMULTI_CURRENT_MANUAL_MODE ) {
					//
					SCHMULTI_CONTROLLER_LOGSET( 31 , Mode );
					//
					if ( SCHMULTI_CONTROLJOB_GET_SELECT_ONLY_STATUS( &Mode2 ) ) {
						//
						SCHMULTI_CONTROLLER_LOGSET( 32 , Mode2 );
						//
						SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_SELECT , 0 , Sch_Multi_ControlJob_Select_Data[Mode2]->JobID , -1 , -1 );
						SCHMULTI_CONTROLJOB_SET_RUN_STATUS( Mode2 );
						SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_START , 0 , Sch_Multi_ControlJob_Select_Data[Mode2]->JobID , -1 , -1 );
					}
				}
				//====================================================================================
//				for ( s = 0 ; s < 2 ; s++ ) { // 2008.05.09
				for ( s = 0 ; s < 4 ; s++ ) { // 2009.05.11
					//
					SCHMULTI_CONTROLLER_LOGSET( 41 , s );
					//
					//------------------------------------------------
					// 2009.03.03
					//------------------------------------------------
//					if ( s == 0 ) { // 2009.05.09
					if ( ( s == 0 ) || ( s == 2 ) ) { // 2009.05.09
						//
						SCHMULTI_CONTROLLER_LOGSET( 42 , s );
						//
						if ( SCHMULTI_CTJOB_SINGLE_MODE_GET() || SCHMULTI_CURRENT_MANUAL_MODE ) { // 2009.03.03
							//
							SCHMULTI_CONTROLLER_LOGSET( 43 , s );
							//
							s_side = 0;
							e_side = MAX_CASSETTE_SIDE - 1;
						}
						else {
							//
							SCHMULTI_CONTROLLER_LOGSET( 44 , s );
							//
							if ( SCHMULTI_CONTROLJOB_PARALLEL_GET_ORDERING_ONE_SELECT( &s_side ) ) {
								//
								SCHMULTI_CONTROLLER_LOGSET( 45 , s_side );
								//
								e_side = s_side;
							}
							else {
								//
								SCHMULTI_CONTROLLER_LOGSET( 46 , s );
								//
								s_side = 0;
								e_side = MAX_CASSETTE_SIDE - 1;
							}
						}
					}
					else {
						//
						SCHMULTI_CONTROLLER_LOGSET( 47 , s );
						//
						s_side = 0;
						e_side = MAX_CASSETTE_SIDE - 1;
					}
					//------------------------------------------------
					//
					sres = 0; // 2009.05.11
					//
//					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2009.03.03
					for ( i = s_side ; i <= e_side ; i++ ) { // 2009.03.03
						//
						SCHMULTI_CONTROLLER_LOGSET( 51 , i );
						//
						if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) { // 2015.10.28
							switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i + CM1 ) ) {
							case 0 : // Disable
							case 2 : // Disable H/W
								//
								SCHMULTI_CONTROLLER_LOGSET( 52 , i );
								//
								SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
								//
								continue;
								//
								break;
							case 1 : // Enable
							case 3 : // Enable PM
								break;
							}
						}
						//
						switch( BUTTON_GET_FLOW_STATUS_VAR( i ) ) {
						case _MS_0_IDLE : // Init
						case _MS_1_LOADING : // Loading
						case _MS_2_LOADED : // Loading(Success)
						case _MS_3_LOADFAIL : // Loading(Fail)
							//
							SCHMULTI_CONTROLLER_LOGSET( 101 , i );
							//
							if ( SCHMULTI_CASSETTE_VERIFY_GET( i ) > CASSETTE_VERIFY_CONFIRMING ) {
								//
								SCHMULTI_CONTROLLER_LOGSET( 102 , i );
								//
								SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
							}
							//
//							SCHMULTI_RUNJOB_SET_FINISH( i , -1 ); // 2018.08.30
							SCHMULTI_RUNJOB_SET_FINISH( -1 , i , -1 ); // 2018.08.30
							//
//							MID_READ_COUNT[ i ] = FALSE; // 2010.05.06
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						case _MS_4_MAPPING : // Mapping
							//
							SCHMULTI_CONTROLLER_LOGSET( 111 , i );
							//
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						case _MS_5_MAPPED : // Mapping(Success)
							//
							SCHMULTI_CONTROLLER_LOGSET( 121 , i );
							//
							//---------------------------------------------------------------------------------------------
//							if ( SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( i , 1 ) ) { // 2008.04.11 // 2010.05.06
							if ( SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( i , 0 , 0 , NULL , 0 ) ) { // 2008.04.11 // 2010.05.06
								//
								SCHMULTI_CONTROLLER_LOGSET( 122 , i );
								//
								findside = i;
								//
								if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
									//
									if ( _i_SCH_SYSTEM_USING_GET( findside ) > 0 ) { // 2013.04.01
										//
										for ( f = 0 ; f < MAX_CASSETTE_SIDE ; f++ ) {
											if ( _i_SCH_SYSTEM_USING_GET( f ) <= 0 ) {
												findside = f;
												break;
											}
										}
										//
									}
								}
								//
								SCHMULTI_CONTROLLER_LOGSET( 129 , findside );
								//
								if ( _i_SCH_SYSTEM_USING_GET( findside ) > 0 ) break; // 2011.04.18
								//
								if ( SCHMULTI_CURRENT_MANUAL_MODE ) break;
								//
								SCHMULTI_CONTROLLER_LOGSET( 123 , i );
								//
								fres = SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE( 0 , i , SCHMULTI_CASSETTE_MID_GET( i ) , &Mode , &Res , &carrmode , &outcassmode , &movemode );
								//
								SCHMULTI_CONTROLLER_LOGSET( 124 , fres );
								SCHMULTI_CONTROLLER_LOGSET( 125 , Mode );
								SCHMULTI_CONTROLLER_LOGSET( 126 , Res );
								SCHMULTI_CONTROLLER_LOGSET( 127 , carrmode );
								SCHMULTI_CONTROLLER_LOGSET( 128 , outcassmode );
								//
								switch ( fres ) {
								case 0 :
									//
									SCHMULTI_CONTROLLER_LOGSET( 131 , Mode );
									//
//									if ( !SCHMULTI_CTJOB_SINGLE_MODE_GET() ) { // 2009.04.03
									if ( !SCHMULTI_CTJOB_SINGLE_MODE_GET() && !outcassmode ) { // 2009.04.03
										//
										SCHMULTI_CONTROLLER_LOGSET( 132 , Mode );
										//
										//==================================================================================================
										// 2006.06.28
										//==================================================================================================
										for ( l = 0 ; l < MAX_CASSETTE_SIDE ; l++ ) {
											if ( l != i ) {
												if ( PRE_RUN_POSSIBLE_CHECK[ l ] == 2 ) break;
											}
										}
										if ( l == MAX_CASSETTE_SIDE ) {
										//==================================================================================================
											//
											SCHMULTI_CONTROLLER_LOGSET( 133 , Mode );
											//
//											sres2 = SCHMULTI_CONTROLJOB_PARALLEL_SELECT( i , SCHMULTI_CASSETTE_MID_GET( i ) , &(PRE_RUN_POSSIBLE_CHECK[ i ]) , &l , s ); // 2009.07.15
											//
//											if ( ( sres2 > 0 ) && ( s == 1 ) ) sres = 1; // 2009.05.11
											//
//											fres = SCHMULTI_CONTROLJOB_PARALLEL_SELECT( i , SCHMULTI_CASSETTE_MID_GET( i ) , &(PRE_RUN_POSSIBLE_CHECK[ i ]) , &l , s ); // 2018.08.16
											fres = SCHMULTI_CONTROLJOB_PARALLEL_SELECT( findside , i , SCHMULTI_CASSETTE_MID_GET( i ) , &(PRE_RUN_POSSIBLE_CHECK[ i ]) , &l , s ); // 2018.08.16
											//
											SCHMULTI_CONTROLLER_LOGSET( 134 , fres );
											SCHMULTI_CONTROLLER_LOGSET( 134 , PRE_RUN_POSSIBLE_CHECK[ i ] );
											SCHMULTI_CONTROLLER_LOGSET( 134 , l );
											//
											if ( fres >= 0 ) { // 2009.07.15
												//
												SCHMULTI_CONTROLLER_LOGSET( 135 , l );
												//
												if ( s == 1 ) sres = 1; // 2009.05.11
											}
											//
											if ( l ) { // 2008.09.18
												//
												SCHMULTI_CONTROLLER_LOGSET( 136 , sres );
												//
												//============================================================================================
												if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
												}
												else {
													BUTTON_SET_FLOW_IO_STATUS( i , _MS_11_WAITING ); // 2008.09.18
													//============================================================================================
													_SCH_IO_SET_MAIN_BUTTON_ONLY( i , CTC_WAITING ); // 2008.09.23
													//============================================================================================
												}
											}
											else {
												//
												SCHMULTI_CONTROLLER_LOGSET( 137 , sres );
												//
												//============================================================================================
												if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
												}
												else {
													BUTTON_SET_FLOW_STATUS_EQUAL( i ); // 2008.09.18
													//============================================================================================
													_SCH_IO_SET_MAIN_BUTTON_EQUAL( i ); // 2008.09.23
													//============================================================================================
												}
											}
											//
											if ( PRE_RUN_POSSIBLE_CHECK[ i ] == 2 ) {
												//
												SCHMULTI_CONTROLLER_LOGSET( 138 , sres );
												//
												for ( l = 0 ; l < MAX_CASSETTE_SIDE ; l++ ) {
													if ( l != i ) {
														if ( PRE_RUN_POSSIBLE_CHECK[ l ] == 1 ) PRE_RUN_POSSIBLE_CHECK[ l ] = 0;
													}
												}
											}
										//==================================================================================================
										}
										//==================================================================================================
									}
									break;
								case 2 : // if Wait Possible
									//
									SCHMULTI_CONTROLLER_LOGSET( 141 , Mode );
									//
									if ( SCHMULTI_CASSETTE_VERIFY_GET( i ) == CASSETTE_VERIFY_CONFIRMED ) {
										//
										SCHMULTI_CONTROLLER_LOGSET( 142 , Mode );
										//
										SCHMULTI_CONTROLJOB_SET_WAIT_STATUS( Mode );
										//if ( !CPW ) { // 2003.03.21
										SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_WAITVERIFY , 0 , MULTI_RUNJOB_MtlCJName( Res ) , -1 , -1 );
										//	CPW = TRUE; // 2003.03.21
										//} // 2003.03.21
										SCHMULTI_CASSETTE_VERIFY_SET( i , CASSETTE_VERIFY_CONFIRMED2 );
									}
									else {
										//
										SCHMULTI_CONTROLLER_LOGSET( 143 , Mode );
										//
										if ( !outcassmode ) { // 2009.04.03
											//
											SCHMULTI_CONTROLLER_LOGSET( 144 , Mode );
											//
											//============================================================================================
											if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
											}
											else {
												BUTTON_SET_FLOW_IO_STATUS( i , _MS_11_WAITING ); // 2008.09.18
												//
												_SCH_IO_SET_MAIN_BUTTON_ONLY( findside , CTC_WAITING ); // 2008.09.23
											}
											//============================================================================================
										}
									}
									break;
								case 1 : // if Start Possible
									//
									SCHMULTI_CONTROLLER_LOGSET( 151 , Mode );
									//
									//------------------------------------------------------------
									if ( outcassmode ) break; // 2009.04.03
									//
									SCHMULTI_CONTROLLER_LOGSET( 152 , Mode );
									//
									//------------------------------------------------------------
									fres = SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE_FOR_OUTCASS( i , Res , &outcassmode );
									//
									SCHMULTI_CONTROLLER_LOGSET( 153 , fres );
									//
									if ( fres <= 0 ) break; // 2009.04.03
									//------------------------------------------------------------
									//
									SCHMULTI_CONTROLLER_LOGSET( 154 , outcassmode );
									//
									if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
									}
									else {
										BUTTON_SET_FLOW_STATUS_EQUAL( i ); // 2008.09.18
										//
										_SCH_IO_SET_MAIN_BUTTON_EQUAL( findside ); // 2008.09.23
									}
									//------------------------------------------------------------
									SCHMULTI_CONTROLJOB_SET_RUN_STATUS( Mode );
									SCHMULTI_CASSETTE_VERIFY_SET( i , CASSETTE_VERIFY_RUNNING );
									//------------------------------------------------------------
									// Start
									//------------------------------------------------------------
									if ( !SCHMULTI_REGISTER_DATA_SET_AND_CHECK_FOR_START( TRUE , Res , findside , i , outcassmode , &StartMode , carrmode ) ) {
										//
										SCHMULTI_CONTROLLER_LOGSET( 155 , StartMode );
										//
										Event_Message_Reject( "AUTO SYSTEM INFORMATION REGISTER ERROR" );
										FA_REJECT_MESSAGE( i , FA_START , ERROR_JOB_PARAM_ERROR , "" );
									}
									else {
										//
										SCHMULTI_CONTROLLER_LOGSET( 156 , StartMode );
										//
										//if ( !CPS ) { // 2003.03.21
										SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_START , 0 , MULTI_RUNJOB_MtlCJName( Res ) , -1 , -1 );
										//	CPS = TRUE; // 2003.03.21
										//} // 2003.03.21
										//
//										SCHEDULER_Make_CARRIER_INDEX( i ); // 2011.08.17 // 2011.09.14
										SCHMRDATA_Data_Setting_for_PreStarting( i + CM1 , findside ); // 2011.09.07
										//
//										FA_ACCEPT_MESSAGE( findside , FA_START , 0 ); // 2018.08.22
										//
										_i_SCH_SYSTEM_FA_SET( findside , 1 );
										_i_SCH_SYSTEM_CPJOB_SET( findside , 1 );
										_i_SCH_SYSTEM_CPJOB_ID_SET( findside , Res );
										_i_SCH_SYSTEM_BLANK_SET( findside , 0 ); // 2011.04.20
										_i_SCH_SYSTEM_MOVEMODE_SET( findside , movemode ); // 2013.09.03
										_i_SCH_SYSTEM_RESTART_SET( findside , 0 ); // 2011.09.23
										//
//										SCHMULTI_RUNJOB_SET_STARTING( Res , i ); // 2018.08.30
										SCHMULTI_RUNJOB_SET_STARTING( Res , findside , i ); // 2018.08.30
										//
										FA_ACCEPT_MESSAGE( findside , FA_START , 0 ); // 2018.08.22
										//
										//------------------------------------------
										SCHMULTI_AFTER_SELECT_CHECK_SET( 1 ); // 2004.06.26
										//------------------------------------------
	//									_beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) i ); // 2004.04.09
	//										if ( _beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) ( i + 100 ) ) == -1 ) { // 2004.04.09 // 2007.03.27
										if ( _beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) ( findside + ( StartMode * 100 ) ) ) == -1 ) { // 2004.04.09 // 2007.03.27
											//
											SCHMULTI_CONTROLLER_LOGSET( 157 , StartMode );
											//
											//----------------------------------------------------------------------------------------------------------------
											// 2004.08.18
											//----------------------------------------------------------------------------------------------------------------
											_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Run_Main_Handling_Code(3) %d\n" , findside + ( StartMode * 100 ) );
											//
											FA_REJECT_MESSAGE( findside , FA_START , ERROR_THREAD_FAIL , "" );
											_i_SCH_MULTIREG_DATA_RESET( findside );
											//----------------------------------------------------------------------------------------------------------------
										}
										else { // 2006.06.26
											//
											SCHMULTI_CONTROLLER_LOGSET( 158 , StartMode );
											//
	//										for ( l = 0 ; l < 500 ; l++ ) { // 2008.04.23
											for ( l = 0 ; l < 1000 ; l++ ) { // 2008.04.23
												//
												if ( SYSTEM_PREPARE_GET( findside ) != 0 ) l = 0; // 2008.04.23
												//
												if ( _i_SCH_SYSTEM_USING_GET( findside ) > 0 ) break;
												//
												Sleep(1);
												//
											}
											//
	//										for ( l = 0 ; l < 2500 ; l++ ) { // 2008.04.23
											for ( l = 0 ; l < 6000 ; l++ ) { // 2008.04.23
												if ( _i_SCH_SYSTEM_USING_GET( findside ) <= 0 ) break;
												if ( _i_SCH_SYSTEM_USING_GET( findside ) >= 3 ) break;
												Sleep(1);
											}
											//
											PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2006.06.28
										}
									}
									break;
								}
							}
							else { // 2008.04.24
								//
								SCHMULTI_CONTROLLER_LOGSET( 171 , i );
								//
								if ( SCHMULTI_CURRENT_MANUAL_MODE ) break;
								//
								SCHMULTI_CONTROLLER_LOGSET( 172 , i );
								//
								fres = SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE( 1 , i , "" , &Mode , &Res , &carrmode , &carrmode , &movemode );
								//
								SCHMULTI_CONTROLLER_LOGSET( 173 , fres );
								SCHMULTI_CONTROLLER_LOGSET( 174 , Mode );
								SCHMULTI_CONTROLLER_LOGSET( 175 , Res );
								SCHMULTI_CONTROLLER_LOGSET( 176 , carrmode );
								//
								if ( fres == 1 ) {
									//
									SCHMULTI_CONTROLLER_LOGSET( 177 , i );
									//
									SCHMULTI_CASSETTE_VERIFY_SET( i , CASSETTE_VERIFY_CONFIRMED2 );
									//
								}
								//
							}
							break;
						case _MS_6_MAPFAIL : // Mapping(Fail)
							//
							SCHMULTI_CONTROLLER_LOGSET( 201 , i );
							//
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						case _MS_7_START : // Running(Start)
						case _MS_8_HANDOFFIN : // Running(Run HandOff In)
						case _MS_9_BEGIN : // Running(Run Map In)
						case _MS_10_MAPIN : // Running(Waiting for Run)
						case _MS_11_WAITING : // Running(Run Looping)
						case _MS_12_RUNNING : // Running(End Looping)
						case _MS_13_MAPOUT : // Running(Run Map Out)
						case _MS_14_HANDOFFOUT : // Running(Run HandOff Out)
							//
							SCHMULTI_CONTROLLER_LOGSET( 211 , i );
							//
//							SCHMULTI_CASSETTE_VERIFY_CLEAR( i , FALSE ); // 2011.03.31
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						case _MS_15_COMPLETE : // Running(Success)
						case _MS_16_CANCEL : // Running(Success with Cancel)
							//
							SCHMULTI_CONTROLLER_LOGSET( 221 , i );
							//
//							if ( SCHMULTI_CURRENT_MANUAL_MODE ) { // 2010.05.06
//								MID_READ_COUNT[ i ] = FALSE; // 2010.05.06
//							} // 2010.05.06
//							SCHMULTI_CASSETTE_VERIFY_CLEAR( i , FALSE ); // 2011.03.31
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						case _MS_17_ABORTED : // Running(Aborted)
							//
							SCHMULTI_CONTROLLER_LOGSET( 231 , i );
							//
//							if ( SCHMULTI_CURRENT_MANUAL_MODE ) { // 2010.05.06
//								MID_READ_COUNT[ i ] = FALSE; // 2010.05.06
//							} // 2010.05.06
//							else { // 2010.05.06
//								//---------------------------------------
//								// This Part neverthless Begin Fail not Job delete
//								//---------------------------------------
//								//SCHMULTI_RUNJOB_SET_WAITING( i );
//								//---------------------------------------
//								// This Part Begin Fail Job delete
//								//---------------------------------------
//								MID_READ_COUNT[ i ] = FALSE; // 2010.05.06
//							} // 2010.05.06
//							SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE ); // 2011.03.31
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						case _MS_18_UNLOADING_C :
						case _MS_19_UNLOADING : // Unloading
						case _MS_20_UNLOADED_C :
						case _MS_21_UNLOADED : // Unloading(Success)
						case _MS_22_UNLOADFAIL : // Unloading(Fail)
							//
							SCHMULTI_CONTROLLER_LOGSET( 241 , i );
							//
							SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
//							MID_READ_COUNT[ i ] = FALSE; // 2010.05.06
							PRE_RUN_POSSIBLE_CHECK[ i ] = 0; // 2004.06.16
							break;
						} // switch
					} // for ( i = 
					//
					SCHMULTI_CONTROLLER_LOGSET( 281 , i );
					//
	//				SCHMULTI_ALL_FORCE_CHECK() { // 2004.10.14
	/*
					for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
						if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_EXECUTING ) {
							if ( SCHMULTI_RUNJOB_ALL_FINISH( i , &Mode2 ) ) {
								SCHMULTI_PROCESSJOB_FORCE_FINISH_CHECK( i );
								SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( ENUM_NOTIFY_CONTROLJOB_FINISH , Mode2 , Sch_Multi_ControlJob_Select_Data[i]->JobID );
								SCHMULTI_CONTROLJOB_DELSELECT( 1 , i , 0 );
							}
						}
					}
	*/
			//	Sleep(250); // 2009.05.11

					if ( s == 1 ) { // 2009.05.11
						//
						SCHMULTI_CONTROLLER_LOGSET( 282 , i );
						//
						if ( sres == 0 ) break; // 2009.05.11
					}
					//
					SCHMULTI_CONTROLLER_LOGSET( 283 , i );
					//

				//
				} // for ( s =
				//
				SCHMULTI_CONTROLLER_BUTTON();
				//
				SCHMULTI_JOB_OVERTIME_DELETE( &ovcnt ); // 2012.04.19
				//
				Sleep(250); // 2009.05.11
				//
			} // while
		} // if
		else {
			//
			Scheduler_HandOffOut_Clear(); // 2011.06.01
			//
			SCHMULTI_CONTROLLER_LOGSET( 501 , 0 );
			//
			//============================================================================================
			// 2006.12.05
			//============================================================================================
			//====================================================================================
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				SCHMULTI_CONTROLLER_LOGSET( 511 , i );
				//
				if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) { // 2015.10.28
					switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i + CM1 ) ) {
					case 0 : // Disable
					case 2 : // Disable H/W
						//
						SCHMULTI_CONTROLLER_LOGSET( 512 , i );
						//
						SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
						//
						continue;
						//
						break;
					case 1 : // Enable
					case 3 : // Enable PM
						break;
					}
				}
				//
				//
//				MID_READ_COUNT[ i ] = FALSE; // 2010.05.06
				PRE_RUN_POSSIBLE_CHECK[ i ] = 0;
				//
				fres = BUTTON_GET_FLOW_STATUS_VAR( i );
				//
				SCHMULTI_CONTROLLER_LOGSET( 521 , fres );
				//
				switch( fres ) {
				case _MS_4_MAPPING : // Mapping
					//
					SCHMULTI_CONTROLLER_LOGSET( 531 , i );
					//
					break;
				case _MS_5_MAPPED : // Mapping(Success)
					//
					SCHMULTI_CONTROLLER_LOGSET( 541 , i );
					//
					//---------------------------------------------------------------------------------------------
					SCHMULTI_CASSETTE_VERIFY_CHECK_AFTER_MAPPING( i , 0 , 0 , NULL , 0 ); // 2008.04.11
					//---------------------------------------------------------------------------------------------
					break;
				case _MS_0_IDLE : // Init
				case _MS_1_LOADING : // Loading
				case _MS_2_LOADED : // Loading(Success)
				case _MS_3_LOADFAIL : // Loading(Fail)
				case _MS_6_MAPFAIL : // Mapping(Fail)
					//
					SCHMULTI_CONTROLLER_LOGSET( 551 , i );
					//
					SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
					break;
				case _MS_7_START : // Running(Start)
				case _MS_8_HANDOFFIN : // Running(Run HandOff In)
				case _MS_9_BEGIN : // Running(Run Map In)
				case _MS_10_MAPIN : // Running(Waiting for Run)
				case _MS_11_WAITING : // Running(Run Looping)
				case _MS_12_RUNNING : // Running(End Looping)
				case _MS_13_MAPOUT : // Running(Run Map Out)
				case _MS_14_HANDOFFOUT : // Running(Run HandOff Out)
				case _MS_15_COMPLETE : // Running(Success)
				case _MS_16_CANCEL : // Running(Success with Cancel)
				case _MS_17_ABORTED : // Running(Aborted)
					//
					SCHMULTI_CONTROLLER_LOGSET( 552 , i );
					//
//					SCHMULTI_CASSETTE_VERIFY_CLEAR( i , FALSE ); // 2011.03.31
					break;
				case _MS_18_UNLOADING_C :
				case _MS_19_UNLOADING : // Unloading
				case _MS_20_UNLOADED_C :
				case _MS_21_UNLOADED : // Unloading(Success)
				case _MS_22_UNLOADFAIL : // Unloading(Fail)
					//
					SCHMULTI_CONTROLLER_LOGSET( 553 , i );
					//
					SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
					break;
				} // switch
				//============================================================================================
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
				}
				else {
					BUTTON_SET_FLOW_STATUS_EQUAL( i ); // 2008.09.18
					//
					_SCH_IO_SET_MAIN_BUTTON_EQUAL( i ); // 2008.09.23
				}
				//============================================================================================
			} // for
			//
			SCHMULTI_CONTROLLER_BUTTON();
			//
			SCHMULTI_JOB_OVERTIME_DELETE( &ovcnt ); // 2012.04.19
			//
			Sleep(250);
		}
	} // while
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CTJOB_USE_SET( BOOL data ) {
	SCHMULTI_USABLE = data;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_CTJOB_USE_GET() {
	return SCHMULTI_USABLE;
}
//---------------------------------------------------------------------------------------
void SCHMULTI_CTJOB_SINGLE_MODE_SET( BOOL data ) {
	SCHMULTI_SINGLE_MODE = data;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_CTJOB_SINGLE_MODE_GET() {
	return SCHMULTI_SINGLE_MODE;
}
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_CONTROLLER_MONITORING( int slinfo ) {
	if ( !SCHMULTI_CTJOB_USE_GET() ) return TRUE;
	//---------------------------------------------------
	// 2005.01.29
	//---------------------------------------------------
	switch ( slinfo ) {
	case 2 : // (002) Default- TM Style(200)
	case 4 : // (004) Default- FM+TM Style(200)
	case 6 : // (006) FM+TM(Twin) Style(200)
	case 8 : // (008) FM+TM(Inlign) Style(200)
	case 10 : // (010) FM+TM(Double) Style(200)
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program (Just Licensed for [(%03d) Under 200 Style])\n" , slinfo );
		return FALSE;
		break;
	}
	//---------------------------------------------------
	if ( _beginthread( (void *) SCHMULTI_CONTROLLER , 0 , 0 ) == -1 ) {
		_IO_CIM_PRINTF( "=> SCHMULTI_CONTROLLER Thread Spawn Failed\n" );
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_PROCESSJOB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int i;
	PAINTSTRUCT ps;
	char Buffer[256];
	char Buffer2[256];
	char *szStringC[] = { "No" , "CarrName" , "Slot" };
	int    szSizeC[3]  = {  30  ,        100 ,    300 };

	char *szStringM[] = { "No" , "MIDName" };
	int    szSizeM[2]  = {  30  ,       250 };

	char *szStringR[] = { "No" , "Module" , "Step" , "Name" , "Data" };
	int    szSizeR[5]  = {  30  ,       50 ,     39 ,    100 ,   100 };


	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MoveCenterWindow( hdlg );
		KWIN_Item_String_Display( hdlg , IDC_SCH_STATUS1 , Sch_Multi_ProcessJob_Display_Buffer.JobID );
		//
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_STATUS2 , Sch_Multi_ProcessJob_Display_Buffer.StartMode , "Auto|Manual" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_Item_Int_Display( hdlg , IDC_SCH_STATUS3 , Sch_Multi_ProcessJob_Display_Buffer.MtlFormat );
		KWIN_Item_String_Display( hdlg , IDC_SCH_STATUS4 , Sch_Multi_ProcessJob_Display_Buffer.MtlRecipeName );
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_STATUSTR , Sch_Multi_ProcessJob_Display_Buffer.MtlRecipeMode , "Only Recipe|Tunning" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_Item_Int_Display( hdlg , IDC_SCH2_STATUS1 , Sch_Multi_ProcessJob_Display_Buffer.DataID );
		strcpy( Buffer , "" );
		for ( i = 0 ; i < MAX_MULTI_PRJOB_EVENTINFO ; i++ ) {
			if ( i != 0 ) strcat ( Buffer , ":" );
			sprintf( Buffer2 , "%d" , Sch_Multi_ProcessJob_Display_Buffer.EventInfo[i] );
			strcat( Buffer , Buffer2 );
		}
		KWIN_Item_String_Display( hdlg , IDC_SCH2_STATUS2 , Buffer );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 3 , szStringC , szSizeC );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_MULTI_PRJOB_CASSETTE );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 2 , szStringM , szSizeM );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_MULTI_CASS_SLOT_SIZE );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , 5 , szStringR , szSizeR );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , MAX_MULTI_PRJOB_RCPTUNE );
		//
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i , j;
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
//								if ( !Sch_Multi_ProcessJob_Display_Buffer.MtlUse[i] ) { // 2016.02.02
//									lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//									return 0; // 2016.02.02
//								} // 2016.02.02
								//
								if ( Sch_Multi_ProcessJob_Display_Buffer.MtlUse[i] ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										lstrcpy( szString , Sch_Multi_ProcessJob_Display_Buffer.MtlCarrName[i] );
										break;
									case 2 :
										strcpy( Buffer , "" );
										for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
											if ( j != 0 ) strcat ( Buffer , ":" );
											sprintf( Buffer2 , "%d" , Sch_Multi_ProcessJob_Display_Buffer.MtlSlot[i][j] );
											strcat( Buffer , Buffer2 );
										}
										lstrcpy( szString , Buffer );
										break;
									}
								}
								//
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
//							if ( !Sch_Multi_ProcessJob_Display_Buffer.MtlUse[lpdi->item.iItem] ) { // 2016.02.02
//								lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0; // 2016.02.02
//								return 0; // 2016.02.02
//							} // 2016.02.02
							//
							if ( Sch_Multi_ProcessJob_Display_Buffer.MtlUse[lpdi->item.iItem] ) { // 2016.02.02
								//
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
								//
							}
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :		return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
//								lstrcpy( lpdi->item.pszText , Sch_Multi_ProcessJob_Display_Buffer.MtlMIDName[lpdi->item.iItem] ); // 2016.02.02
								lstrcpy( szString , Sch_Multi_ProcessJob_Display_Buffer.MtlMIDName[lpdi->item.iItem] ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :		return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								//
//								if ( !Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneModule[i] ) { // 2016.02.02
//									lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//									return 0; // 2016.02.02
//								} // 2016.02.02
								//
								if ( Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneModule[i] ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										sprintf( szString , "PM%d" , Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneModule[i] );
										break;
									case 2 :
										sprintf( szString , "%d" , Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneStep[i] );
										break;
									case 3 :
										lstrcpy( szString , Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneName[i] );
										break;
									case 4 :
										lstrcpy( szString , Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneData[i] );
										break;
									}
								}
								//
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								//
							}
						}
						else {
							//
//							if ( !Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneModule[lpdi->item.iItem] ) { // 2016.02.02
//								lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0; // 2016.02.02
//								return 0; // 2016.02.02
//							} // 2016.02.02
							//
							if ( Sch_Multi_ProcessJob_Display_Buffer.MtlRcpTuneModule[lpdi->item.iItem] ) { // 2016.02.02
								//
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
								//
							}
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :		return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
				}
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
			//
		case IDC_READY_USE_P1 : // Delete
		case IDC_READY_USE_P2 : // Verify
		case IDC_READY_USE_B1 : // Pause
		case IDC_READY_USE_B2 : // Resume
		case IDC_READY_USE_B3 : // Stop
		case IDC_READY_USE_B4 : // Abort
			//
			if ( SCHMULTI_PROCESSJOB_BUFFER_POSSIBLE() ) {
				MessageBox( hdlg , "ProcessJob System has not been Prepared" , "Error", MB_ICONHAND );
			}
			else {
				//
				Sch_Multi_ProcessJob_Buffer->ControlStatus = 1;
				strcpy( Sch_Multi_ProcessJob_Buffer->JobID , Sch_Multi_ProcessJob_Display_Buffer.JobID );
				//
				switch( wParam ) {
				case IDC_READY_USE_P1 : // Delete
					i = SCHMULTI_PROCESSJOB_DELETE( 3 , -1 );
					break;
				case IDC_READY_USE_P2 : // Verify
					i = SCHMULTI_RUNJOB_SET_PROCESSJOB_POSSIBLE();
					break;
				case IDC_READY_USE_B1 : // Pause
					i = SCHMULTI_PROCESSJOB_PAUSE();
					break;
				case IDC_READY_USE_B2 : // Resume
					i = SCHMULTI_PROCESSJOB_RESUME();
					break;
				case IDC_READY_USE_B3 : // Stop
					i = SCHMULTI_PROCESSJOB_STOP();
					break;
				case IDC_READY_USE_B4 : // Abort
					i = SCHMULTI_PROCESSJOB_ABORT();
					break;
				}
				//
				if ( i != 0 ) {
					sprintf( Buffer, "Operation has been failed with error %d" , i );
					MessageBox( hdlg , Buffer , "Error", MB_ICONHAND );
				}
				else {
					switch( wParam ) {
					case IDC_READY_USE_P1 : // Delete
						EndDialog( hdlg , 0 );
						break;
					}
				}
				//
			}
			//
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}

BOOL APIENTRY Gui_IDD_CONTROLJOB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int i;
	PAINTSTRUCT ps;
	char Buffer[256];
	char Buffer2[256];

	char *szStringC[] = { "No" , "PRJOB" , "CIN" , "CIN-ID" , "COUT" , "SRCID" , "OUTID" , "In" , "Out" };
	int    szSizeC[9]  = {  30  ,     70 ,     30 ,     50 ,      30 ,      30 ,      30 ,   30 ,    30 };

	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		//
		MoveCenterWindow( hdlg );
		KWIN_Item_String_Display( hdlg , IDC_SCH_STATUS1 , Sch_Multi_ControlJob_Display_Buffer.JobID );
		//
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_STATUS2 , Sch_Multi_ControlJob_Display_Buffer.StartMode , "Auto|Manual" , "????" );
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_STATUS3 , Sch_Multi_ControlJob_Display_Buffer.PRJobDelete , "Remain|Delete|Delete(Manual)" , "????" );
		//=============================================================================
		KWIN_Item_String_Display( hdlg , IDC_SCH_STATUS4 , Sch_Multi_ProcessJob_Display_Buffer.MtlRecipeName );
		//=============================================================================
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 9 , szStringC , szSizeC );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_MULTI_CTJOB_CASSETTE );
		//
		if ( (int) lParam == 0 ) { // cj
			KWIN_Item_Hide( hdlg , IDC_READY_USE_P5 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_P6 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
		}
		else { // cjsel
			KWIN_Item_Hide( hdlg , IDC_READY_USE_P1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_P2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_P3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_P4 );
		}
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i , j;
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								//
//								if ( !Sch_Multi_ControlJob_Display_Buffer.CtrlSpec_Use[i] ) { // 2016.02.02
//									lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//									return 0; // 2016.02.02
//								} // 2016.02.02
								//
								if ( Sch_Multi_ControlJob_Display_Buffer.CtrlSpec_Use[i] ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										lstrcpy( szString , Sch_Multi_ControlJob_Display_Buffer.CtrlSpec_ProcessJobID[i] );
										break;
									case 2 :
										if ( Sch_Multi_ControlJob_Display_Buffer.CassInUse[i] ) {
											lstrcpy( szString , "O" );
										}
										else {
											lstrcpy( szString , "" );
										}
										break;
									case 3 :
										lstrcpy( szString , Sch_Multi_ControlJob_Display_Buffer.CassInID[i] );
										break;
									case 4 :
										if ( Sch_Multi_ControlJob_Display_Buffer.CassOutUse[i] ) {
											lstrcpy( szString , "O" );
										}
										else {
											lstrcpy( szString , "" );
										}
										break;
									case 5 :
										lstrcpy( szString , Sch_Multi_ControlJob_Display_Buffer.CassSrcID[i] );
										break;
									case 6 :
										lstrcpy( szString , Sch_Multi_ControlJob_Display_Buffer.CassOutID[i] );
										break;
									case 7 :
										strcpy( Buffer , "" );
										for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
											if ( j != 0 ) strcat( Buffer , ":" );
											sprintf( Buffer2 , "%d" , Sch_Multi_ControlJob_Display_Buffer.CassSlotIn[i][j] );
											strcat( Buffer , Buffer2 );
										}
										lstrcpy( szString , Buffer );
										break;
									case 8 :
										strcpy( Buffer , "" );
										for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) {
											if ( j != 0 ) strcat( Buffer , ":" );
											sprintf( Buffer2 , "%d" , Sch_Multi_ControlJob_Display_Buffer.CassSlotOut[i][j] );
											strcat( Buffer , Buffer2 );
										}
										lstrcpy( szString , Buffer );
										break;
									}
									//
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
									//
								}
							}
						}
						else {
							//
//							if ( !Sch_Multi_ControlJob_Display_Buffer.CtrlSpec_Use[lpdi->item.iItem] ) { // 2016.02.02
//								lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0; // 2016.02.02
//								return 0; // 2016.02.02
//							} // 2016.02.02
							//
							if ( Sch_Multi_ControlJob_Display_Buffer.CtrlSpec_Use[lpdi->item.iItem] ) { // 2016.02.02
								//
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
							}
							//
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :		return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
				}
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
			//
		case IDC_READY_USE_P1 : // Delete
		case IDC_READY_USE_P2 : // CHGSELECT
		case IDC_READY_USE_P3 : // HEADOFQ
		case IDC_READY_USE_P4 : // WAIT_RELEASE
		case IDC_READY_USE_P5 : // DELETE_SEL
		case IDC_READY_USE_P6 : // VERIFY
		case IDC_READY_USE_B1 : // Pause
		case IDC_READY_USE_B2 : // Resume
		case IDC_READY_USE_B3 : // Stop
		case IDC_READY_USE_B4 : // Abort
			//
			if ( SCHMULTI_CONTROLJOB_BUFFER_POSSIBLE() ) {
				MessageBox( hdlg , "ControlJob System has not been Prepared" , "Error", MB_ICONHAND );
			}
			else {
				//
				if ( wParam != IDC_READY_USE_P4 ) {
					Sch_Multi_ControlJob_Buffer->ControlStatus = 1;
					strcpy( Sch_Multi_ControlJob_Buffer->JobID , Sch_Multi_ControlJob_Display_Buffer.JobID );
				}
				//
				switch( wParam ) {
				case IDC_READY_USE_P1 : // Delete
					i = SCHMULTI_CONTROLJOB_DELETE( -1 );
					break;
				case IDC_READY_USE_P2 : // CHGSELECT
					i = SCHMULTI_CONTROLJOB_CHGSELECT();
					break;
				case IDC_READY_USE_P3 : // HEADOFQ
					i = SCHMULTI_CONTROLJOB_HEADOFQ();
					break;
				case IDC_READY_USE_P4 : // WAIT_RELEASE
					SCHMULTI_CONTROLJOB_Queued_to_Select_Wait( TRUE );
					i = 0;
					break;
				case IDC_READY_USE_P5 : // DELETE_SEL
					i = SCHMULTI_CONTROLJOB_DELSELECT( 0 , -1 , 0 , FALSE );
					break;
				case IDC_READY_USE_P6 : // VERIFY
					i = SCHMULTI_RUNJOB_SET_CONTROLJOB_POSSIBLE();
					break;
				case IDC_READY_USE_B1 : // Pause
					i = SCHMULTI_CONTROLJOB_PAUSE( "" );
					break;
				case IDC_READY_USE_B2 : // Resume
					i = SCHMULTI_CONTROLJOB_RESUME( "" );
					break;
				case IDC_READY_USE_B3 : // Stop
					i = SCHMULTI_CONTROLJOB_STOP( "" );
					break;
				case IDC_READY_USE_B4 : // Abort
					i = SCHMULTI_CONTROLJOB_ABORT( "" );
					break;
				}
				//
				if ( i != 0 ) {
					sprintf( Buffer, "Operation has been failed with error %d" , i );
					MessageBox( hdlg , Buffer , "Error", MB_ICONHAND );
				}
				else {
					switch( wParam ) {
					case IDC_READY_USE_P1 : // Delete
					case IDC_READY_USE_P2 : // CHGSELECT
					case IDC_READY_USE_P3 : // HEADOFQ
					case IDC_READY_USE_P5 : // DELETE_SEL
						EndDialog( hdlg , 0 );
						break;
					}
				}
				//
			}
			//
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_RUNCARR_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	char Buffer[256];
	char Buffer2[256];
	char *szString[] = { "No" , "Index" , "CarrName" , "Status" , "Port" , "Slot" };
	int    szSize[6] = {  35  ,      50 ,        100 ,      100 ,     60 ,    200 };

	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MoveCenterWindow( hdlg );
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 6 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_MULTI_RUNJOB_SIZE );
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i , j , k;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO:	{
					LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
					TCHAR szString[ 256 ];
					//
					szString[0] = 0; // 2016.02.02
					//
					if ( lpdi->item.iSubItem ) {
						if ( lpdi->item.mask & LVIF_TEXT ) {
							i = lpdi->item.iItem;
							for ( k = 0 ; k < MAX_MULTI_CTJOBSELECT_SIZE ; k++ ) {
								if ( Sch_Multi_ControlJob_Select_Data[k]->ControlStatus >= CTLJOB_STS_SELECTED ) break;
							}
							//
							if ( k != MAX_MULTI_CTJOBSELECT_SIZE ) { // 2016.02.02
								//
								if ( MULTI_RUNJOB_RunStatus(i) != 0 ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										sprintf( szString , "%d" , MULTI_RUNJOB_SelectCJIndex(i) );
										break;
									case 2 :
										sprintf( szString , MULTI_RUNJOB_CarrierID( i ) );
										break;
									case 3 :
										switch( MULTI_RUNJOB_RunStatus(i) )	{
										case CASSRUNJOB_STS_READY	: sprintf( szString , "Ready" ); break;
										case CASSRUNJOB_STS_WAITING	: sprintf( szString , "Waiting" ); break;
										case CASSRUNJOB_STS_RUNNING	: sprintf( szString , "Running" ); break;
										case CASSRUNJOB_STS_FINISH_WITH_NORMAL			: sprintf( szString , "Finish" ); break;
										case CASSRUNJOB_STS_FINISH_WITH_ABORT			: sprintf( szString , "Finish(Abort)" ); break;
										case CASSRUNJOB_STS_FINISH_WITH_ERROR			: sprintf( szString , "Finish(Error)" ); break;
										case CASSRUNJOB_STS_FINISH_WITH_ENDSTOP			: sprintf( szString , "Finish(EndStop)" ); break;
										case CASSRUNJOB_STS_FINISH_WITH_CANCELDELETE	: sprintf( szString , "Finish(Cancel)" ); break;
										case CASSRUNJOB_STS_FINISH_WITH_FORCEFINISH		: sprintf( szString , "Finish(Force)" ); break;
										default	: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 4 :
										if ( MULTI_RUNJOB_RunStatus(i) > CASSRUNJOB_STS_WAITING ) {
											if ( MULTI_RUNJOB_Cassette(i) < 0 ) {
												sprintf( szString , "-" );
											}
											else {
												sprintf( szString , "%d" ,MULTI_RUNJOB_Cassette(i) + 1 );
											}
										}
										else {
											sprintf( szString , "-" );
										}
										break;
									case 5 :
										strcpy( Buffer , "" );
										for ( j = 0 ; j < MULTI_RUNJOB_MtlCount(i) ; j++ ) {
											if ( j != 0 ) strcat ( Buffer , ":" );
											sprintf( Buffer2 , "%d" , MULTI_RUNJOB_MtlInSlot(i)[j] );
											strcat( Buffer , Buffer2 );
										}
										lstrcpy( szString , Buffer );
										break;
									}
		//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
							}
						}
					}
					else {
						for ( k = 0 ; k < MAX_MULTI_CTJOBSELECT_SIZE ; k++ ) {
							if ( Sch_Multi_ControlJob_Select_Data[k]->ControlStatus >= CTLJOB_STS_SELECTED ) break;
						}
						//
						if ( k == MAX_MULTI_CTJOBSELECT_SIZE ) { // 2016.02.02
							lstrcpy( szString , "-" ); // 2016.02.02
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
						else {
							if ( MULTI_RUNJOB_RunStatus(lpdi->item.iItem) == 0 ) { // 2016.02.02
								lstrcpy( szString , "-" ); // 2016.02.02
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
							}
							else {
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
		//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
							}
						}
					}
					//
					strcpy( lpdi->item.pszText , szString ); // 2016.02.02
					//
				}
				return 0;
			case NM_DBLCLK :
				InvalidateRect( hdlg , NULL , TRUE );
				return 0;
			case LVN_ITEMCHANGED :
				return 0;
			case LVN_KEYDOWN :
				InvalidateRect( hdlg , NULL , TRUE );
				return 0;
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_RUNCARR2_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int i;
	char Buffer[256];
	PAINTSTRUCT ps;
	char *szString[] = { "CM" , "Verify" , "CarrName" , "Var" , "MRC" , "PRT" , "IO-MID" };
	int    szSize[7] = {  35  ,      90  ,        200 ,    45 ,    45 ,    45 ,  100     };

	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		SetWindowText( hdlg , "Carrier Handling Status" );
		MoveCenterWindow( hdlg );
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 7 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE );
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO:	{
					LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
					TCHAR szString[ 256 ];
					//
					szString[0] = 0; // 2016.02.02
					//
					if ( lpdi->item.iSubItem ) {
						if ( lpdi->item.mask & LVIF_TEXT ) {
							switch( lpdi->item.iSubItem ) {
							case 1 :
								switch( Sch_Multi_Cassette_Verify[lpdi->item.iItem] )	{
								case CASSETTE_VERIFY_READY			: sprintf( szString , "Ready" ); break;
								case CASSETTE_VERIFY_WAIT_CONFIRM	: sprintf( szString , "Wait-Confirm" ); break;
								case CASSETTE_VERIFY_CONFIRMING		: sprintf( szString , "Confirming" ); break;
								case CASSETTE_VERIFY_CONFIRMED		: sprintf( szString , "Confirmed" ); break;
								case CASSETTE_VERIFY_CONFIRMED2		: sprintf( szString , "Confirmed2" ); break;
								case CASSETTE_VERIFY_RUNNING		: sprintf( szString , "Running" ); break;
								default								: sprintf( szString , "Unknown" ); break;
								}
								break;
							case 2 :
								sprintf( szString , "%s" , SCHMULTI_CASSETTE_MID_GET( lpdi->item.iItem ) ); // 2012.04.01
/*
// 2012.04.01
								if ( Sch_Multi_Cassette_MID[lpdi->item.iItem] == NULL ) { // 2008.04.02
									sprintf( szString , "" );
								}
								else {
									sprintf( szString , Sch_Multi_Cassette_MID[lpdi->item.iItem] );
								}
*/
								break;
							case 3 :
								sprintf( szString , "%d" , BUTTON_GET_FLOW_STATUS_VAR(lpdi->item.iItem) );
								break;
							case 4 :
//								sprintf( szString , "%d" , MID_READ_COUNT[lpdi->item.iItem] ); // 2010.05.06
//								sprintf( szString , "%s" , SCHMULTI_CASSETTE_MID_GET( lpdi->item.iItem ) ); // 2010.05.06 // 2012.04.01
								sprintf( szString , "%s" , SCHMULTI_CASSETTE_MID_GET_FOR_START( lpdi->item.iItem ) ); // 2012.04.01
								break;
							case 5 :
								sprintf( szString , "%d" , PRE_RUN_POSSIBLE_CHECK[lpdi->item.iItem] );
								break;
							case 6 :
								switch( _i_SCH_PRM_GET_FA_MID_READ_POINT( lpdi->item.iItem ) ) {
								case 3 :
									IO_GET_MID_NAME( lpdi->item.iItem , szString );
									break;
								default :
									IO_GET_MID_NAME_FROM_READ( lpdi->item.iItem , szString );
									break;
								}
								break;
							}
//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
						}
					}
					else {
						if ( lpdi->item.mask & LVIF_TEXT ) {
							wsprintf( szString, "CM%d", lpdi->item.iItem + 1 );
//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
						}
						if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
					}
					//
					strcpy( lpdi->item.pszText , szString ); // 2016.02.02
					//
				}
				return 0;
			case NM_DBLCLK :
				i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
				if ( i < 0 ) return 0;
				//
				if ( Sch_Multi_Cassette_Verify[i] == CASSETTE_VERIFY_RUNNING ) {
					if ( IDYES == MessageBox( hdlg , "Do you want to Reset for Carrier?", "Check" , MB_ICONQUESTION | MB_YESNO ) ) {
						Sch_Multi_Cassette_Verify[i] = CASSETTE_VERIFY_READY;
					}
				}
				else {
					switch( BUTTON_GET_FLOW_STATUS_VAR( i ) ) {
					case _MS_0_IDLE :
					case _MS_2_LOADED :
					case _MS_3_LOADFAIL :
					case _MS_15_COMPLETE :
					case _MS_16_CANCEL :
					case _MS_17_ABORTED :
					case _MS_20_UNLOADED_C :
					case _MS_21_UNLOADED :
					case _MS_22_UNLOADFAIL :
						//
						if ( IDYES == MessageBox( hdlg , "Do you want to set for Properly Carrier?", "Check" , MB_ICONQUESTION | MB_YESNO ) ) {
							//
							strcpy( Buffer , "" ); // 2014.02.03
							//
							if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select MID Name" , Buffer ) ) {
								//
								switch( _i_SCH_PRM_GET_FA_MID_READ_POINT( i ) ) {
								case 3 :
									IO_SET_MID_NAME( i , Buffer );
									break;
								default :
									IO_SET_MID_NAME_FROM_READ( i , Buffer );
									break;
								}
								//
								if ( Buffer[0] != 0 ) BUTTON_SET_FLOW_STATUS_VAR( i , _MS_5_MAPPED );
								//
							}
						}
						break;
					default :
						//
						if ( Sch_Multi_Cassette_Verify[i] == CASSETTE_VERIFY_WAIT_CONFIRM ) {
							//
							if ( IDYES == MessageBox( hdlg , "Do you want to Verify for Carrier?", "Check" , MB_ICONQUESTION | MB_YESNO ) ) {
								//
								i = SCHMULTI_CASSETTE_VERIFY_MAKE_OK( i );
								//
								if ( i != 0 ) {
									sprintf( Buffer, "While Carrier Verifing , Operation has been failed with error %d" , i );
									MessageBox( hdlg , Buffer , "Error", MB_ICONHAND );
								}
								//
							}
							//
						}
						//
						break;
						//
					}
				}
				//
				InvalidateRect( hdlg , NULL , TRUE );
				return 0;
			case LVN_ITEMCHANGED :
				return 0;
			case LVN_KEYDOWN :
				InvalidateRect( hdlg , NULL , TRUE );
				return 0;
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int Job_Insert_File_Control( HWND hdlg , char *filename ) {
	//
	// PROCESSJOB	NAME	STARTMODE	RECIPE	MID	STARTSLOT	ENDSLOT
	//				""		0/1			""		""	1?			1?
	//
	// CONTROLJOB	NAME	STARTMODE	PRJOB...
	//				""		0/1			""
	//
	BOOL bres;
	FILE *fpt;
	//
	char Buffer[1024];
	//
	int m , sm , z1 , z2 , x , ss , es;
	//
	char Name[128];
	char Recipe[128];
	char mid[128];
	char TempBuffer[256];
	//
	if ( ( fpt = fopen( filename , "r" ) ) == NULL ) return 1;
	//
	do {
		//
		bres = Read_Line_From_File4( fpt , Buffer , 1023 );
		//
		if ( *Buffer != 0 ) {
			//------------------------------------------
			//
			z1 = 0;
			//
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z1 , 255 );	z1 = z2;
			//
			if      ( STRCMP_L( TempBuffer , "PROCESSJOB" ) ) {
				m = 0;
			}
			else if ( STRCMP_L( TempBuffer , "CONTROLJOB" ) ) {
				m = 1;
			}
			else {
				m = -1;
			}
			//
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Name , z1 , 127 );		UTIL_Extract_Reset_String( Name );	z1 = z2;	// NAME
			//
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z1 , 255 );	sm = atoi(TempBuffer);				z1 = z2;	// STARTMODE
			//
			if      ( m == 0 ) {
				//
				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , Recipe , z1 , 127 );		UTIL_Extract_Reset_String( Recipe );	z1 = z2;	// Recipe
				//
				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , mid , z1 , 127 );			UTIL_Extract_Reset_String( mid );		z1 = z2;	// MID
				//
				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z1 , 255 );	ss = atoi(TempBuffer);					z1 = z2;	// SS
				//
				z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z1 , 255 );	es = atoi(TempBuffer);					z1 = z2;	// ES
				//
				//===============================================================================
				//
				x = SCHMULTI_PROCESSJOB_GUI_INSERT( Name , ( sm == 1 ) ? 1 : 0 , Recipe , ( ss <= 0 ) ? 1 : ss , ( es <= 0 ) ? _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() : es , mid );
				//
				if ( x != 0 ) {
					sprintf( TempBuffer , "While PROCESS JOB[%s] Inserting , Operation has been failed with error %d" , Name , x );
					MessageBox( hdlg , TempBuffer , "Error" , MB_ICONHAND );
					break;
				}
				//
				//===============================================================================
				//
			}
			else if ( m == 1 ) {
				//
				for ( x = 0 ; x < MAX_MULTI_CTJOB_CASSETTE ; x++ ) STR_MEM_MAKE_COPY2( &(GUI_INSERT_CONREOLJOB_PRJOB_NAME2[x]) , "" );
				//
				x = 0;
				//
				while ( TRUE ) {
					//
					z2 = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z1 , 255 );	// PRJ
					//
					if ( z1 == z2 ) break;
					//
					if ( x >= MAX_MULTI_CTJOB_CASSETTE ) break;
					//
					UTIL_Extract_Reset_String( TempBuffer );
					//
					if ( TempBuffer[0] != 0 ) {
						//
						STR_MEM_MAKE_COPY2( &(GUI_INSERT_CONREOLJOB_PRJOB_NAME2[x]) , TempBuffer );
						//
						x++;
						//
					}
					//
					z1 = z2;
					//
				}
				//
				//===============================================================================
				if ( x > 0 ) {
					//
					x = SCHMULTI_CONTROLJOB_GUI_INSERT( Name , TRUE , ( sm == 1 ) ? 1 : 0 );
					//
					if ( x != 0 ) {
						sprintf( TempBuffer , "While CONTROL JOB[%s] Inserting , Operation has been failed with error %d" , Name , x );
						MessageBox( hdlg , TempBuffer , "Error" , MB_ICONHAND );
						break;
					}
					//
				}
				//===============================================================================
			}
			//
		}
	}
	while ( bres );
	fclose( fpt );
	return 0;
}



BOOL APIENTRY Gui_IDD_CONTROLJOB_INSERT_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	char temp_Buffer[256];
	int temp_i , i;
	//
	char *szString[] = { "Name" };
	int    szSize[1] = {  130 };
	//
	TCHAR pszString[ 256 ];

	switch ( msg ) {
	case WM_INITDIALOG:
		MoveCenterWindow( hdlg );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 1 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_MULTI_CTJOB_CASSETTE );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 1 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_MULTI_CTJOB_CASSETTE );
		//
		return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		//
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P1 , STR_MEM_GET_STR( GUI_INSERT_CONTROLJOB_NAME  ) );
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P2 , GUI_INSERT_CONTROLJOB_STARTMODE  == 0 ? "N/A" : "Wait" );
		//
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						//
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						//
						pszString[0] = 0; // 2016.02.02
						//
//						if ( lpdi->item.iItem >= *Sch_Multi_ProcessJob_Size ) { // 2016.02.02
//							lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//							return 0; // 2016.02.02
//						} // 2016.02.02
						//
//						lstrcpy( lpdi->item.pszText , Sch_Multi_ProcessJob[lpdi->item.iItem]->JobID ); // 2016.02.02
						//
						if ( lpdi->item.iItem < *Sch_Multi_ProcessJob_Size ) { // 2016.02.02
							lstrcpy( pszString , Sch_Multi_ProcessJob[lpdi->item.iItem]->JobID ); // 2016.02.02
						}
						//
						strcpy( lpdi->item.pszText , pszString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :
					temp_i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
					if ( temp_i < 0 ) return 0;
					if ( temp_i < *Sch_Multi_ProcessJob_Size ) {
						//
						for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
							//
							if ( STRCMP_L( STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[i] ) , Sch_Multi_ProcessJob[temp_i]->JobID ) ) break;
							//
						}
						//
						if ( i == MAX_MULTI_CTJOB_CASSETTE ) {
							//
							for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
								//
								if ( STRCMP_L( STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[i] ) , "" ) ) break;
								//
							}
							//
							if ( i != MAX_MULTI_CTJOB_CASSETTE ) {
								STR_MEM_MAKE_COPY2( &(GUI_INSERT_CONREOLJOB_PRJOB_NAME[i]) , Sch_Multi_ProcessJob[temp_i]->JobID );
							}
						}
						InvalidateRect( hdlg , NULL , TRUE );
					}
					return 0;
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						//
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						//
						pszString[0] = 0; // 2016.02.02
						//
//						lstrcpy( lpdi->item.pszText , STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[lpdi->item.iItem] ) ); // 2016.02.02
						lstrcpy( pszString , STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[lpdi->item.iItem] ) ); // 2016.02.02
						//
						strcpy( lpdi->item.pszText , pszString ); // 2016.02.02
						//
					}
					return 0;

				case NM_DBLCLK :
					temp_i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( temp_i < 0 ) return 0;
					if ( !STRCMP_L( STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[temp_i] ) , "" ) ) {
						STR_MEM_MAKE_COPY2( &(GUI_INSERT_CONREOLJOB_PRJOB_NAME[temp_i]) , "" );
						//
						for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
							if ( !STRCMP_L( STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[i] ) , "" ) ) break;
						}
						if ( i != MAX_MULTI_CTJOB_CASSETTE ) {
							for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
								if ( STRCMP_L( STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[i] ) , "" ) ) {
									for ( temp_i = (i + 1) ; temp_i < MAX_MULTI_CTJOB_CASSETTE ; temp_i++ ) {
										if ( !STRCMP_L( STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[temp_i] ) , "" ) ) {
											//
											STR_MEM_MAKE_COPY2( &(GUI_INSERT_CONREOLJOB_PRJOB_NAME[i]) , STR_MEM_GET_STR( GUI_INSERT_CONREOLJOB_PRJOB_NAME[temp_i] ) );
											STR_MEM_MAKE_COPY2( &(GUI_INSERT_CONREOLJOB_PRJOB_NAME[temp_i]) , "" );
											//
											break;
										}
									}
								}
							}
						}
						//
						InvalidateRect( hdlg , NULL , TRUE );
					}
					return 0;
				}
			}
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_P1 :
			strcpy( temp_Buffer , STR_MEM_GET_STR( GUI_INSERT_CONTROLJOB_NAME ) );
			if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select Job Name" , temp_Buffer ) ) {
				STR_MEM_MAKE_COPY2( &GUI_INSERT_CONTROLJOB_NAME , temp_Buffer );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			break;

		case IDC_READY_USE_P2 :
			temp_i = GUI_INSERT_CONTROLJOB_STARTMODE;
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Start Mode" , "N/A|Wait" , &temp_i ) ) {
				GUI_INSERT_CONTROLJOB_STARTMODE = temp_i;
				InvalidateRect( hdlg , NULL , TRUE );
			}
			break;

		case IDOK :
			//
			temp_i = SCHMULTI_CONTROLJOB_GUI_INSERT( STR_MEM_GET_STR( GUI_INSERT_CONTROLJOB_NAME ) , FALSE , GUI_INSERT_CONTROLJOB_STARTMODE );
			//
			if ( temp_i != 0 ) {
				sprintf( temp_Buffer, "While CONTROL JOB Inserting , Operation has been failed with error %d" , temp_i );
				MessageBox( hdlg , temp_Buffer , "Error", MB_ICONHAND );
			}
			//
			return TRUE;
			//
		case IDRETRY : // 2014.02.03
			//
			strcpy( temp_Buffer , "" );
			//
			if ( UTIL_Get_OpenSave_File_Select( hdlg , FALSE , temp_Buffer , 255 , "" , "*.*" , "Select Job List File" ) ) {
				//
				Job_Insert_File_Control( hdlg , temp_Buffer );
				//
				EndDialog( hdlg , 0 );
				//
			}
			//
			return TRUE;
			//
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	case WM_DESTROY:
		break;

	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_PROCESSJOB_INSERT_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	char Dir_Buffer[256];
	char temp_Buffer[256];
	int temp_i;
	//
	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		//
		MoveCenterWindow( hdlg );
		//
		if ( GUI_INSERT_PROCESSJOB_ENDSLOT < 0 ) GUI_INSERT_PROCESSJOB_ENDSLOT = _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT(); // 2014.02.03
		//
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		//
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P1 , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_NAME ) );
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P2 , GUI_INSERT_PROCESSJOB_STARTMODE == 0 ? "N/A" : "Wait" );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_P3 , GUI_INSERT_PROCESSJOB_STARTSLOT );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_P4 , GUI_INSERT_PROCESSJOB_ENDSLOT );
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P5 , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_RECIPE ) );
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P6 , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_CARRIERID ) );
		//
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_P1 :
			strcpy( temp_Buffer , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_NAME ) );
			if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select Job Name" , temp_Buffer ) ) {
				STR_MEM_MAKE_COPY2( &GUI_INSERT_PROCESSJOB_NAME , temp_Buffer );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			break;

		case IDC_READY_USE_P2 :
			temp_i = GUI_INSERT_PROCESSJOB_STARTMODE;
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select Start Mode" , "N/A|Wait" , &temp_i ) ) {
				GUI_INSERT_PROCESSJOB_STARTMODE = temp_i;
				InvalidateRect( hdlg , NULL , TRUE );
			}
			break;

		case IDC_READY_USE_P3 :
			temp_i = GUI_INSERT_PROCESSJOB_STARTSLOT;
			if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select Start Slot" , 1 , MAX_CASSETTE_SLOT_SIZE , &temp_i ) ) {
				if ( temp_i <= GUI_INSERT_PROCESSJOB_ENDSLOT ) {
					GUI_INSERT_PROCESSJOB_STARTSLOT = temp_i;
					InvalidateRect( hdlg , NULL , TRUE );
				}
				else { // 2014.02.03
					GUI_INSERT_PROCESSJOB_STARTSLOT = temp_i;
					GUI_INSERT_PROCESSJOB_ENDSLOT =  temp_i;
					InvalidateRect( hdlg , NULL , TRUE );
				}
			}
			break;

		case IDC_READY_USE_P4 :
			temp_i = GUI_INSERT_PROCESSJOB_ENDSLOT;
			if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Select End Slot" , 1 , MAX_CASSETTE_SLOT_SIZE , &temp_i ) ) {
				if ( temp_i >= GUI_INSERT_PROCESSJOB_STARTSLOT ) {
					GUI_INSERT_PROCESSJOB_ENDSLOT =  temp_i;
					InvalidateRect( hdlg , NULL , TRUE );
				}
				else { // 2014.02.03
					GUI_INSERT_PROCESSJOB_STARTSLOT = temp_i;
					GUI_INSERT_PROCESSJOB_ENDSLOT =  temp_i;
					InvalidateRect( hdlg , NULL , TRUE );
				}
			}
			break;

		case IDC_READY_USE_P5 :
			/*
			// 2014.02.03
			strcpy( temp_Buffer , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_RECIPE ) );
			if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select Recipe Name" , temp_Buffer ) ) {
				STR_MEM_MAKE_COPY2( &GUI_INSERT_PROCESSJOB_RECIPE , temp_Buffer );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			*/
			if ( GUI_GROUP_SELECT_GET() == 0 )
				sprintf( Dir_Buffer , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() , _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() );
			else
				sprintf( Dir_Buffer , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() , "%s" , _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() );
			//
			if ( MODAL_FILE_OPEN_BOX( hdlg , Dir_Buffer , "Select Cluster Recipe Name" , "*.*" , "" , temp_Buffer ) ) {
				//
				Ext_Name_From_FullFile( Dir_Buffer , temp_Buffer , 99 );
				//
				STR_MEM_MAKE_COPY2( &GUI_INSERT_PROCESSJOB_RECIPE , Dir_Buffer );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			//
			break;

		case IDC_READY_USE_P6 :
			strcpy( temp_Buffer , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_CARRIERID ) );
			if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select Carrier Name" , temp_Buffer ) ) {
				STR_MEM_MAKE_COPY2( &GUI_INSERT_PROCESSJOB_CARRIERID , temp_Buffer );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			break;

		case IDOK :
			//
			temp_i = SCHMULTI_PROCESSJOB_GUI_INSERT( STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_NAME ) , GUI_INSERT_PROCESSJOB_STARTMODE , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_RECIPE ) , GUI_INSERT_PROCESSJOB_STARTSLOT , GUI_INSERT_PROCESSJOB_ENDSLOT , STR_MEM_GET_STR( GUI_INSERT_PROCESSJOB_CARRIERID ) );
			//
			if ( temp_i != 0 ) {
				sprintf( temp_Buffer, "While PROCESS JOB Inserting , Operation has been failed with error %d" , temp_i );
				MessageBox( hdlg , temp_Buffer , "Error", MB_ICONHAND );
			}
			//
			return TRUE;
			//
		case IDRETRY : // 2014.02.03
			//
			strcpy( temp_Buffer , "" );
			//
			if ( UTIL_Get_OpenSave_File_Select( hdlg , FALSE , temp_Buffer , 255 , "" , "*.*" , "Select Job List File" ) ) {
				//
				Job_Insert_File_Control( hdlg , temp_Buffer );
				//
				EndDialog( hdlg , 0 );
				//
			}
			//
			return TRUE;
			//
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_CPJOB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	static char  TEMP_STR[256];
	static int   TEMP_DATA;
	static int   TEMP_DATA2;
	static int   TEMP_DATA3;
	static int   TEMP_DATA4; // 2005.05.11
	static int   TEMP_DATA5; // 2006.04.13
	static int   TEMP_DATA6; // 2012.04.20
	static BOOL Control;
	char Buffer[256];
	char *szStringP[] = { "No" , "Name" , "Status" , "Mode " , "Fmt" , "Use" , "Carr" , "Recipe" , "OverTime" };
	int    szSizeP[9]  = {  35  ,    85 ,       80 ,      60 ,    35 ,    35 ,    200 ,      200 ,  60 };

	char *szStringC[] = { "No" , "Name" , "Status" , "Mode" , "Order" ,  "Use" , "PJID" , "HowPJ" , "OverTime" };
	int    szSizeC[9]  = {  30  ,    85 ,       80 ,      55 ,    45 ,    35 ,    300   ,      80 , 60 };

	switch ( msg ) {

	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.01
		//
		//------------------------------------------------------------
		Control = (BOOL) lParam;
		//------------------------------------------------------------
		MoveCenterWindow( hdlg );
		if ( SCHMULTI_CTJOB_USE_GET() ) {
//			sprintf( Buffer , "Control Process Job(Running) [%d:%d]" , SCHMULTI_RCPTUNE_EXTEND_USE , _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() ); // 2015.12.01
			sprintf( Buffer , "Control Process Job(Running) [%d:%d:%d]" , SCHMULTI_RCPTUNE_EXTEND_USE , _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() , _SCH_MULTIJOB_CHECK_EXTEND_AREA_USE() ); // 2015.12.01
		}
		else {
			sprintf( Buffer , "Control Process Job(N/A)" );
		}
		//
		SetWindowText( hdlg , Buffer );
		//
		KWIN_Item_Disable( hdlg , IDOK );
		//
		strncpy( TEMP_STR , SCHMULTI_DEFAULT_GROUP_GET() , 255 );
		TEMP_DATA = PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB();
//		TEMP_DATA2 = SCHMULTI_DEFAULT_LOTRECIPE_READ_GET(); // 2014.06.23
		TEMP_DATA2 = _i_SCH_MULTIJOB_GET_LOTRECIPE_READ(); // 2014.06.23
		TEMP_DATA3 = SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET(); // 2004.05.11
		TEMP_DATA4 = SCHMULTI_JOBNAME_DISPLAYMODE_GET(); // 2005.05.11
		TEMP_DATA5 = SCHMULTI_MESSAGE_SCENARIO_GET(); // 2006.04.13
		TEMP_DATA6 = SCHMULTI_CHECK_OVERTIME_HOUR_GET(); // 2012.04.20
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 9 , szStringP , szSizeP );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() == 2 ) { // 2010.02.08
			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , ( MAX_MULTI_QUEUE_SIZE + MAX_MULTI_PRJOB_EXT_SIZE ) );
		}
		else {
			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_MULTI_QUEUE_SIZE );
		}
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 9 , szStringC , szSizeC );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_MULTI_QUEUE_SIZE );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , 9 , szStringC , szSizeC );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , MAX_MULTI_CTJOBSELECT_SIZE );
		//
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		KWIN_Item_Disable( hdlg , IDOK );
		if ( !STRCMP_L( TEMP_STR , SCHMULTI_DEFAULT_GROUP_GET() ) ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_DATA != PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ) KWIN_Item_Enable( hdlg , IDOK );
//		if ( TEMP_DATA2 != SCHMULTI_DEFAULT_LOTRECIPE_READ_GET() ) KWIN_Item_Enable( hdlg , IDOK ); // 2014.06.23
		if ( TEMP_DATA2 != _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ) KWIN_Item_Enable( hdlg , IDOK ); // 2014.06.23
		if ( TEMP_DATA3 != SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() ) KWIN_Item_Enable( hdlg , IDOK ); // 2004.05.11
		if ( TEMP_DATA4 != SCHMULTI_JOBNAME_DISPLAYMODE_GET() ) KWIN_Item_Enable( hdlg , IDOK ); // 2005.05.11
		if ( TEMP_DATA5 != SCHMULTI_MESSAGE_SCENARIO_GET() ) KWIN_Item_Enable( hdlg , IDOK ); // 2006.04.13
		if ( TEMP_DATA6 != SCHMULTI_CHECK_OVERTIME_HOUR_GET() ) KWIN_Item_Enable( hdlg , IDOK ); // 2012.04.20
		//
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_P1 , TEMP_STR );
		//
		//=============================================================================
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P2 , TEMP_DATA , "CARRIER|PROCESSJOB" , "????" ); // 2007.07.24 // 2019.04.05
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P2 , TEMP_DATA , "CARRIER|PROCESSJOB|CARR(V.PJ)|PJOB(V.PJ)" , "????" ); // 2007.07.24 // 2019.04.05
		//=============================================================================
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P3 , TEMP_DATA2 , "N/A|Read Cluster,Dummy Recipe,Error=>Fail|Read Dummy Recipe Only,Error=>Fail|Read Cluster,Dummy Recipe,Error=>Skip|Read Dummy Recipe Only,Error=>Skip" , "????" ); // 2007.07.24 // 2012.06.28
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P3 , TEMP_DATA2 , "N/A|Lot Recipe,Error=>Fail|Lot Recipe,Error=>Skip|N/A:(M)Lot Recipe,Error=>Fail|N/A:(M)Lot Recipe,Error=>Skip|Lot Recipe,Error=>Fail:(M)N/A|Lot Recipe,Error=>Skip:(M)N/A" , "????" ); // 2007.07.24 // 2012.06.28
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P4 , TEMP_DATA3 , "Default|Type1(D,S)|Type2(F,M)|Type3(D,M)|Type4(F,S,ASel)|Type5(D,S,ASel)|Type6(F,M,ASel)|Type7(D,M,ASel)" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , TEMP_DATA4 , "C-J|P-J(0)|P-J(A)|LotID|MID|C-J+P-J" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , TEMP_DATA5 , "Default|Case-1" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B3 , TEMP_DATA6 );
		//=============================================================================
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i , j , k , l , l2;
			time_t currtime;
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) { // ProcessJob
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ]; // 2010.04.20
						TCHAR szString2[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
//								if ( i >= *Sch_Multi_ProcessJob_Size ) { // 2016.02.02
//									lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//									return 0; // 2016.02.02
//								} // 2016.02.02
								//
								if ( i < *Sch_Multi_ProcessJob_Size ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										sprintf( szString , Sch_Multi_ProcessJob[i]->JobID );
										break;
									case 2 :
										switch( Sch_Multi_ProcessJob[i]->ControlStatus )	{
										case PRJOB_STS_NOTSTATE			: sprintf( szString , "Ready:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_QUEUED			: sprintf( szString , "Queued:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_SETTINGUP		: sprintf( szString , "Selected:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_WAITINGFORSTART	: sprintf( szString , "Waiting:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_PROCESSING		: sprintf( szString , "Running:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_PROCESSCOMPLETED	: sprintf( szString , "Complete:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_EXECUTING		: sprintf( szString , "Execute:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_PAUSING			: sprintf( szString , "Pausing:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_PAUSE			: sprintf( szString , "Pause:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_STOPPING			: sprintf( szString , "Stopping:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										case PRJOB_STS_ABORTING			: sprintf( szString , "Aborting:%d" , Sch_Multi_ProcessJob[i]->ResultStatus ); break;
										default							: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 3 :
										switch( Sch_Multi_ProcessJob[i]->StartMode )	{
										case 0 : sprintf( szString , "Auto" ); break;
										case 1 : sprintf( szString , "Manual" ); break;
										default: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 4 :
										sprintf( szString , "%d" , Sch_Multi_ProcessJob[i]->MtlFormat );
										break;
									case 5 :
										for ( j = 0 , k = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ProcessJob[i]->MtlUse[j] != 0 ) k++;
										}
										sprintf( szString , "%d" , k );
										break;
									case 6 :
										l = 0; // 2010.04.15
										//
										strcpy( szString , "" );
										//
										for ( j = 0 , k = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ProcessJob[i]->MtlUse[j] != 0 ) {
												sprintf( szString2 , Sch_Multi_ProcessJob[i]->MtlCarrName[j] );
												//
												/*
												// 2011.12.14
												if ( strlen( szString2 ) <= 0 ) { // 2008.04.23
													sprintf( szString2 , "<%d>" , Sch_Multi_ProcessJob[i]->MtlFormat - 100 ); // 2008.04.23
												}
												*/
												// 2011.12.14
												if ( Sch_Multi_ProcessJob[i]->MtlFormat >= 100 ) {
													sprintf( szString2 , "<%d>" , Sch_Multi_ProcessJob[i]->MtlFormat - 100 );
												}
												//
												l2 = strlen( szString2 ); // 2010.04.15
												//
												if ( l + l2 + k >= 250 ) { // 2010.04.15
													strcat( szString , " ..." );
													break;
												}
												//
												if ( k == 1 ) strcat( szString , "|" ); // 2010.04.15
												strcat( szString , szString2 ); // 2010.04.15
												//
												l = l + l2 + k; // 2010.04.15
												//
												if ( k == 0 ) k = 1;
											}
										}
										break;
									case 7 :
										l = 0; // 2010.04.15
										//
										strcpy( szString , "" );
										//
										for ( j = 0 , k = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ProcessJob[i]->MtlUse[j] != 0 ) {
												//
												sprintf( szString2 , Sch_Multi_ProcessJob[i]->MtlRecipeName );
												//
												l2 = strlen( szString2 ); // 2010.04.15
												//
												if ( l + l2 + k >= 250 ) { // 2010.04.15
													strcat( szString , " ..." );
													break;
												}
												//
												if ( k == 1 ) strcat( szString , "|" ); // 2010.04.15
												strcat( szString , szString2 ); // 2010.04.15
												//
												l = l + l2 + k; // 2010.04.15
												//
												if ( k == 0 ) k = 1;
											}
										}
										break;
									case 8 : // 2012.04.22
										time( &currtime );
										sprintf( szString , "%.2f" , (double) difftime( currtime , Sch_Multi_Ins_ProcessJob[i].InsertTime ) / 3600 );
										break;
									}
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
							}
						}
						else {
//							if ( lpdi->item.iItem >= *Sch_Multi_ProcessJob_Size ) { // 2016.02.02
//								lstrcpy( lpdi->item.pszText , "-" ); // 2016.02.02
//								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0; // 2016.02.02
//								return 0; // 2016.02.02
//							} // 2016.02.02
							//
							if ( lpdi->item.iItem < *Sch_Multi_ProcessJob_Size ) { // 2016.02.02
								//
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
							}
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if ( i >= *Sch_Multi_ProcessJob_Size ) {
//						MessageBeep(MB_ICONHAND); // 2012.04.24
						GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_INSERT_PROCESSJOB_PAD ) , hdlg , Gui_IDD_PROCESSJOB_INSERT_Proc , (LPARAM) NULL ); // 2012.04.22
					}
					else {
						memcpy( &Sch_Multi_ProcessJob_Display_Buffer , Sch_Multi_ProcessJob[i] , sizeof(Scheduling_Multi_ProcessJob) );
						memcpy( &Sch_Multi_Ins_ProcessJob_Display_Buffer , &(Sch_Multi_Ins_ProcessJob[i]) , sizeof(Scheduling_Multi_Ins_ProcessJob) ); // 2012.04.19
//						GoModalDialogBoxParam( GETHINST(NULL) , MAKEINTRESOURCE( IDD_PROCESSJOB_PAD ) , hdlg , Gui_IDD_PROCESSJOB_PAD_Proc , (LPARAM) NULL ); // 2002.06.17
//						GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_PROCESSJOB_PAD ) , hdlg , Gui_IDD_PROCESSJOB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19
//						GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_PROCESSJOB_PAD ) , hdlg , Gui_IDD_PROCESSJOB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_PROCESSJOB_PAD ) , hdlg , Gui_IDD_PROCESSJOB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
					}
					InvalidateRect( hdlg , NULL , TRUE );
					return 0;
				case LVN_ITEMCHANGED :
					return 0;
				case LVN_KEYDOWN :
					InvalidateRect( hdlg , NULL , TRUE );
					return 0;
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) { // ControlJob
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ]; // 2010.04.20
						TCHAR szString2[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								//
//								if ( i >= *Sch_Multi_ControlJob_Size ) { // 2016.02.02
//									lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//									return 0; // 2016.02.02
//								} // 2016.02.02
								//
								if ( i < *Sch_Multi_ControlJob_Size ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										sprintf( szString , Sch_Multi_ControlJob[i]->JobID );
										break;
									case 2 :
										if ( Sch_Multi_Ins_ControlJob[i].Queued_to_Select_Wait ) { // 2012.09.26
											switch( Sch_Multi_ControlJob[i]->ControlStatus )	{
											case CTLJOB_STS_NOTSTATE		: sprintf( szString , "Ready(W)" ); break;
											case CTLJOB_STS_QUEUED			: sprintf( szString , "Queued(W)" ); break;
											case CTLJOB_STS_SELECTED		: sprintf( szString , "Selected(W)" ); break;
											case CTLJOB_STS_WAITINGFORSTART	: sprintf( szString , "Waiting(W)" ); break;
											case CTLJOB_STS_EXECUTING		: sprintf( szString , "Running(W)" ); break;
											case CTLJOB_STS_PAUSED			: sprintf( szString , "Paused(W)" ); break;
											case CTLJOB_STS_COMPLETED		: sprintf( szString , "Complete(W)" ); break;
											default							: sprintf( szString , "Unknown(W)" ); break;
											}
										}
										else {
											switch( Sch_Multi_ControlJob[i]->ControlStatus )	{
											case CTLJOB_STS_NOTSTATE		: sprintf( szString , "Ready" ); break;
											case CTLJOB_STS_QUEUED			: sprintf( szString , "Queued" ); break;
											case CTLJOB_STS_SELECTED		: sprintf( szString , "Selected" ); break;
											case CTLJOB_STS_WAITINGFORSTART	: sprintf( szString , "Waiting" ); break;
											case CTLJOB_STS_EXECUTING		: sprintf( szString , "Running" ); break;
											case CTLJOB_STS_PAUSED			: sprintf( szString , "Paused" ); break;
											case CTLJOB_STS_COMPLETED		: sprintf( szString , "Complete" ); break;
											default							: sprintf( szString , "Unknown" ); break;
											}
										}
										break;
									case 3 :
										switch( Sch_Multi_ControlJob[i]->StartMode )	{
										case 0 : sprintf( szString , "Auto" ); break;
										case 1 : sprintf( szString , "Manual" ); break;
										default: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 4 :
										switch( Sch_Multi_ControlJob[i]->OrderMode )	{
										case 0 : sprintf( szString , "List" ); break;
										case 1 : sprintf( szString , "Arriv" ); break;
										case 2 : sprintf( szString , "Optim" ); break;
										default: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 5 :
										for ( j = 0 , k = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ControlJob[i]->CtrlSpec_Use[j] != 0 ) k++;
										}
										sprintf( szString , "%d" , k );
										break;
									case 6 :
										l = 0; // 2010.04.15
										//
										strcpy( szString , "" );
										//
										for ( j = 0 , k = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ControlJob[i]->CtrlSpec_Use[j] != 0 ) {
												//
												sprintf( szString2 , Sch_Multi_ControlJob[i]->CtrlSpec_ProcessJobID[j] );
												//
												l2 = strlen( szString2 ); // 2010.04.15
												//
												if ( l + l2 + k >= 250 ) { // 2010.04.15
													strcat( szString , " ..." );
													break;
												}
												//
												if ( k == 1 ) strcat( szString , "|" ); // 2010.04.15
												strcat( szString , szString2 ); // 2010.04.15
												//
												l = l + l2 + k; // 2010.04.15
												//
												if ( k == 0 ) k = 1;
												//
											}
										}
										break;
									case 7 :
										switch( Sch_Multi_ControlJob[i]->PRJobDelete )	{
										case 0 : sprintf( szString , "Remain" ); break;
										case 1 : sprintf( szString , "Delete" ); break;
										default: sprintf( szString , "Delete(M)" ); break;
										}
										break;
									case 8 : // 2012.04.22
										time( &currtime );
										sprintf( szString , "%.2f" , (double) difftime( currtime , Sch_Multi_Ins_ControlJob[i].InsertTime ) / 3600 );
										break;
									}
									//
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
									//
								}
							}
						}
						else {
							//
//							if ( lpdi->item.iItem >= *Sch_Multi_ControlJob_Size ) { // 2016.02.02
//								lstrcpy( lpdi->item.pszText , "-" ); // 2016.02.02
//								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0; // 2016.02.02
//								return 0; // 2016.02.02
//							} // 2016.02.02
							//
							if ( lpdi->item.iItem < *Sch_Multi_ControlJob_Size ) { // 2016.02.02
								//
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
								//
							}
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if ( i >= *Sch_Multi_ControlJob_Size ) {
						GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_INSERT_CONTROLJOB_PAD ) , hdlg , Gui_IDD_CONTROLJOB_INSERT_Proc , (LPARAM) NULL ); // 2012.04.22
					}
					else {
						memcpy( &Sch_Multi_ControlJob_Display_Buffer , Sch_Multi_ControlJob[i] , sizeof(Scheduling_Multi_ControlJob) );
						memcpy( &Sch_Multi_Ins_ControlJob_Display_Buffer , &(Sch_Multi_Ins_ControlJob[i]) , sizeof(Scheduling_Multi_Ins_ControlJob) );
						GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_CONTROLJOB_PAD ) , hdlg , Gui_IDD_CONTROLJOB_PAD_Proc , (LPARAM) 0 );
					}
					InvalidateRect( hdlg , NULL , TRUE );
					return 0;
				case LVN_ITEMCHANGED :
					return 0;
				case LVN_KEYDOWN :
					return 0;
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) { // ControlJobSelected
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						TCHAR szString2[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								//
//								if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus < CTLJOB_STS_SELECTED ) { // 2016.02.02
//									lstrcpy( lpdi->item.pszText , "" ); // 2016.02.02
//									return 0; // 2016.02.02
//								} // 2016.02.02
								//
								if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_SELECTED ) { // 2016.02.02
									//
									switch( lpdi->item.iSubItem ) {
									case 1 :
										sprintf( szString , Sch_Multi_ControlJob_Select_Data[i]->JobID );
										break;
									case 2 :
										switch( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus )	{
										case CTLJOB_STS_NOTSTATE		: sprintf( szString , "Ready" ); break;
										case CTLJOB_STS_QUEUED			: sprintf( szString , "Queued" ); break;
										case CTLJOB_STS_SELECTED		: sprintf( szString , "Selected" ); break;
										case CTLJOB_STS_WAITINGFORSTART	: sprintf( szString , "Waiting" ); break;
										case CTLJOB_STS_EXECUTING				:
											if ( Sch_Multi_ControlJob_Selected_Stop_Signal[i] ) {
												sprintf( szString , "Stopping" );
											}
											else {
												sprintf( szString , "Running" );
											}
											break;
										case CTLJOB_STS_PAUSED					:
											switch( Sch_Multi_ControlJob_Selected_Pause_Signal[i] ) {
											case 0 : sprintf( szString , "Paused" ); break;
											case 1 : sprintf( szString , "Paused(S)" ); break;
											case 2 : sprintf( szString , "Paused(W)" ); break;
											}
											break;
										case CTLJOB_STS_COMPLETED		: sprintf( szString , "Complete" ); break;
										default							: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 3 :
										switch( Sch_Multi_ControlJob_Select_Data[i]->StartMode )	{
										case 0 : sprintf( szString , "Auto" ); break;
										case 1 : sprintf( szString , "Manual" ); break;
										default: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 4 :
										switch( Sch_Multi_ControlJob_Select_Data[i]->OrderMode )	{
										case 0 : sprintf( szString , "List" ); break;
										case 1 : sprintf( szString , "Arriv" ); break;
										case 2 : sprintf( szString , "Optim" ); break;
										default: sprintf( szString , "Unknown" ); break;
										}
										break;
									case 5 :
										for ( j = 0 , k = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ControlJob_Select_Data[i]->CtrlSpec_Use[j] != 0 ) k++;
										}
										sprintf( szString , "%d" , k );
										break;
									case 6 :
										//
										l = 0; // 2010.04.15
										//
										strcpy( szString , "" );
										//
										for ( j = 0 , k = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
											if ( Sch_Multi_ControlJob_Select_Data[i]->CtrlSpec_Use[j] != 0 ) {
												sprintf( szString2 , Sch_Multi_ControlJob_Select_Data[i]->CtrlSpec_ProcessJobID[j] );
												//
												l2 = strlen( szString2 ); // 2010.04.15
												//
												if ( l + l2 + k >= 250 ) { // 2010.04.15
													strcat( szString , " ..." );
													break;
												}
												//
												if ( k == 1 ) strcat( szString , "|" ); // 2010.04.15
												strcat( szString , szString2 ); // 2010.04.15
												//
												l = l + l2 + k; // 2010.04.15
												//
												if ( k == 0 ) k = 1;
												//
											}
										}
										break;
									case 7 :
										switch( Sch_Multi_ControlJob_Select_Data[i]->PRJobDelete )	{
										case 0 : sprintf( szString , "Remain" ); break;
										case 1 : sprintf( szString , "Delete" ); break;
										default: sprintf( szString , "Delete(M)" ); break;
										}
										break;
									case 8 : // 2012.04.22
										time( &currtime );
										sprintf( szString , "%.2f" , (double) difftime( currtime , Sch_Multi_Ins_ControlJob_Select_Data[i].InsertTime ) / 3600 );
										break;
									}
									//
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
									//
								}
							}
						}
						else {
							i = lpdi->item.iItem;
							//
//							if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus < CTLJOB_STS_SELECTED ) { // 2016.02.02
//								lstrcpy( lpdi->item.pszText , "-" ); // 2016.02.02
//								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0; // 2016.02.02
//								return 0; // 2016.02.02
//							} // 2016.02.02
							//
							if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus >= CTLJOB_STS_SELECTED ) { // 2016.02.02
								//
								if ( lpdi->item.mask & LVIF_TEXT ) {
									wsprintf( szString, "%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
							}
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;
				case NM_DBLCLK :
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					//
					if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus != CTLJOB_STS_NOTSTATE ) {
						//
						memcpy( &Sch_Multi_ControlJob_Display_Buffer , Sch_Multi_ControlJob_Select_Data[i] , sizeof(Scheduling_Multi_ControlJob) );
						memcpy( &Sch_Multi_Ins_ControlJob_Display_Buffer , &(Sch_Multi_Ins_ControlJob_Select_Data[i]) , sizeof(Scheduling_Multi_Ins_ControlJob) );
						GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_CONTROLJOB_PAD ) , hdlg , Gui_IDD_CONTROLJOB_PAD_Proc , (LPARAM) 1 );
						//
					}
					InvalidateRect( hdlg , NULL , TRUE );
					return 0;
				case LVN_ITEMCHANGED :
					return 0;
				case LVN_KEYDOWN :
					return 0;
				}
			}
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_P1 :
			if ( Control ) return TRUE;
			strcpy( Buffer , TEMP_STR );
			if ( MODAL_STRING_BOX1( hdlg , "Select" , "Select Group Name" , Buffer ) ) {
				strncpy( TEMP_STR , Buffer , 255 );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			break;

		case IDC_READY_USE_P2 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Log Based Mode" , "Select" , "Carrier|ProcessJob" , &TEMP_DATA ); // 2019.04.05
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Log Based Mode" , "Select" , "Carrier|ProcessJob|Carrier(View.ProcessJob)|ProcessJob(View.ProcessJob)" , &TEMP_DATA ); // 2019.04.05
			break;

		case IDC_READY_USE_P3 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Lot Recipe Read Style for Cluster Data" , "Select" , "N/A|Read Cluster,Dummy Recipe,Error=>Fail|Read Dummy Recipe Only,Error=>Fail|Read Cluster,Dummy Recipe,Error=>Skip|Read Dummy Recipe Only,Error=>Skip" , &TEMP_DATA2 ); // 2012.06.28
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Lot Recipe Read Style for Cluster Data" , "Select" , "N/A|Lot Recipe,Error=>Fail|Lot Recipe,Error=>Skip|N/A:(M)Lot Recipe,Error=>Fail|N/A:(M)Lot Recipe,Error=>Skip|Lot Recipe,Error=>Fail:(M)N/A|Lot Recipe,Error=>Skip:(M)N/A" , &TEMP_DATA2 ); // 2012.06.28
			break;

		case IDC_READY_USE_P4 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Make Manual CJ/PJ Job Name" , "Select" , "Default(FixedName,SingleCJ)|Type 1(DateName,SingleCJ)|Type 2(FixedName,MultiCJ)|Type 3(DateName,MultiCJ)|Type 4(FixedName,SingleCJ,AfterSel)|Type 5(DateName,SingleCJ,AfterSel)|Type 6(FixedName,MultiCJ,AfterSel)|Type 7(DateName,MultiCJ,AfterSel)" , &TEMP_DATA3 );
			break;

		case IDC_READY_USE_B1 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Job Name Display Data?" , "Select" , "C-J(ControlJob)|P-J(0)(ProcessJob0)|P-J(A)(ProcessJob(ALL))|LotID(LotRecipe)|MID(MaterialID)|C-J,P-J(Control,ProcessJob)" , &TEMP_DATA4 );
			break;

		case IDC_READY_USE_B2 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Message Scenario?" , "Select" , "Default|Case-1" , &TEMP_DATA5 );
			break;

		case IDC_READY_USE_B3 : // 2012.04.20
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Select OverTime Delete Hour?" , "Select" , 0 , 9999 , &TEMP_DATA6 );
			break;

		case IDYES :
//			GoModalDialogBoxParam( GETHINST(NULL) , MAKEINTRESOURCE( IDD_CPJOB_CARR_PAD ) , hdlg , Gui_IDD_RUNCARR_PAD_Proc , (LPARAM) NULL ); // 2002.06.16
//			GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_CPJOB_CARR_PAD ) , hdlg , Gui_IDD_RUNCARR_PAD_Proc , (LPARAM) NULL ); // 2004.01.19
//			GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CPJOB_CARR_PAD ) , hdlg , Gui_IDD_RUNCARR_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
			GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_CPJOB_CARR_PAD ) , hdlg , Gui_IDD_RUNCARR_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
			return TRUE;

		case IDNO : // 2006.12.05
			GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_CPJOB_CARR_PAD ) , hdlg , Gui_IDD_RUNCARR2_PAD_Proc , (LPARAM) NULL );
			return TRUE;

		case IDOK :
			if ( Control ) return TRUE;
			//
			SCHMULTI_DEFAULT_GROUP_SET( TEMP_STR );
			PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( TEMP_DATA );
//			SCHMULTI_DEFAULT_LOTRECIPE_READ_SET( TEMP_DATA2 ); // 2003.09.24 // 2014.06.23
			_i_SCH_MULTIJOB_SET_LOTRECIPE_READ( TEMP_DATA2 ); // 2003.09.24 // 2014.06.23
			SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_SET( TEMP_DATA3 ); // 2004.05.11
			SCHMULTI_JOBNAME_DISPLAYMODE_SET( TEMP_DATA4 ); // 2005.05.11
			SCHMULTI_MESSAGE_SCENARIO_SET( TEMP_DATA5 ); // 2006.04.13
			SCHMULTI_CHECK_OVERTIME_HOUR_SET( TEMP_DATA6 ); // 2012.04.20
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//
			Set_RunPrm_IO_Setting( 7 ); // 2002.12.30
			Set_RunPrm_IO_Setting( 22 ); // 2008.10.23
			//
			EndDialog( hdlg , 0 );
			return TRUE;

		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void SCHMULTI_ReSet_For_Move( BOOL all , int tside , int tp , int side , int sp ) { // 2011.05.21
	int i , j , c , p;
	//
	//--------------------------------------------------------------------
	c = _i_SCH_CLUSTER_Get_CPJOB_CONTROL( side , sp );
	p = _i_SCH_CLUSTER_Get_CPJOB_PROCESS( side , sp );
	//--------------------------------------------------------------------
	if ( ( c < 0 ) || ( p < 0 ) ) return;
	//--------------------------------------------------------------------
	if ( all ) {
		//
		MULTI_RUNJOB_RunSide(c) = tside; // 2018.08.30
		//
		MULTI_RUNJOB_Cassette(c) = tside;
		MULTI_RUNJOB_CassetteIndex(c) = SCHEDULER_Get_CARRIER_INDEX( tside ); // 2018.05.24
		return;
	}
	//
	for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
		//
		if ( MULTI_RUNJOB_RunStatus(i) < CASSRUNJOB_STS_WAITING ) continue;
		//
//		if ( MULTI_RUNJOB_Cassette(i) != tside ) continue; // 2018.08.30
		if ( MULTI_RUNJOB_RunSide(i) != tside ) continue; // 2018.08.30
		//
		j = MULTI_RUNJOB_MtlCount(i);
		//
//		if ( MAX_CASSETTE_SLOT_SIZE <= j ) return; // 2018.05.10
		if ( MAX_RUN_CASSETTE_SLOT_SIZE <= j ) return; // 2018.05.10
		//
		MULTI_RUNJOB_Mtl_Side(i)[j]	= MULTI_RUNJOB_Mtl_Side(c)[p]; // 2018.05.10
		MULTI_RUNJOB_Mtl_Pointer(i)[j]	= MULTI_RUNJOB_Mtl_Pointer(c)[p]; // 2018.05.10
		//
		MULTI_RUNJOB_MtlPJ_StartMode(i)[j]	= MULTI_RUNJOB_MtlPJ_StartMode(c)[p];
		strcpy( MULTI_RUNJOB_MtlPJName( i )[j] , MULTI_RUNJOB_MtlPJName( c )[p] );
		MULTI_RUNJOB_MtlPJID(i)[j]			= MULTI_RUNJOB_MtlPJID(c)[p];
		MULTI_RUNJOB_MtlPJIDRes(i)[j]		= MULTI_RUNJOB_MtlPJIDRes(c)[p];
		MULTI_RUNJOB_MtlPJIDEnd(i)[j]		= MULTI_RUNJOB_MtlPJIDEnd(c)[p];
		strcpy( MULTI_RUNJOB_MtlRecipeName( i )[j] , MULTI_RUNJOB_MtlRecipeName( c )[p] );
		//
		MULTI_RUNJOB_MtlInCassette(i)[j]	= MULTI_RUNJOB_MtlInCassette(c)[p];
		strcpy( MULTI_RUNJOB_MtlInCarrierID(i)[j] , MULTI_RUNJOB_MtlInCarrierID(c)[p] );
		MULTI_RUNJOB_MtlInCarrierIndex(i)[j]	= MULTI_RUNJOB_MtlInCarrierIndex(c)[p]; // 2018.05.10
		MULTI_RUNJOB_MtlInSlot(i)[j]		= MULTI_RUNJOB_MtlInSlot(c)[p];
		//
		MULTI_RUNJOB_MtlOutCassette(i)[j]	= MULTI_RUNJOB_MtlOutCassette(c)[p];
		strcpy( MULTI_RUNJOB_MtlOutCarrierID(i)[j] , MULTI_RUNJOB_MtlOutCarrierID(c)[p] );
		MULTI_RUNJOB_MtlOutCarrierIndex(i)[j]	= MULTI_RUNJOB_MtlOutCarrierIndex(c)[p]; // 2018.05.10
		MULTI_RUNJOB_MtlOutSlot(i)[j]		= MULTI_RUNJOB_MtlOutSlot(c)[p];
		//
		MULTI_RUNJOB_MtlRecipeMode(i)[j]	= MULTI_RUNJOB_MtlRecipeMode(c)[p];
		//
		MULTI_RUNJOB_MtlCount(i)++;
		//
		_i_SCH_CLUSTER_Set_CPJOB_CONTROL( tside , tp , i );
		_i_SCH_CLUSTER_Set_CPJOB_PROCESS( tside , tp , j );
		//
		_i_SCH_CLUSTER_Set_CPJOB_CONTROL( side , sp , -1 );
		_i_SCH_CLUSTER_Set_CPJOB_PROCESS( side , sp , -1 );
		//
//		strcpy( MULTI_RUNJOB_MtlPJName( c )[p] , "" ); // 2011.04.28
		MULTI_RUNJOB_MtlPJID(c)[p] = -1;
		//
		return;
	}
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void SCHEDULING_DATA_STRUCTURE_JOB_LOG( char *filename , char *dispdata ) {
	int i , j , k;
	FILE *fpt;
//	struct tm Pres_Time; // 2006.10.02
	SYSTEMTIME		SysTime; // 2006.10.02
	struct tm		*Pres_Time; // 2018.11.14

	if ( SYSTEM_LOGSKIP_STATUS() ) return;
	//
	if ( filename[0] == 0 ) {
		fpt = fopen( SCHEDULER_DEBUG_DATA_FILE , "a" );
	}
	else {
		fpt = fopen( filename , "a" );
	}
	//
	if ( fpt == NULL ) return;
	//
	if ( SCHMULTI_CTJOB_USE_GET() ) {
		//
		fprintf( fpt , "==========================================================================================================\n" );
	//	_get-systime( &Pres_Time ); // 2006.10.02
	//	fprintf( fpt , "[MULTIJOB] %d:%d:%d\t%s\n" , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec , dispdata ); // 2006.10.02
		GetLocalTime( &SysTime ); // 2006.10.02
		fprintf( fpt , "[MULTIJOB] %d:%d:%d\t%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , dispdata ); // 2006.10.02
		fprintf( fpt , "==========================================================================================================\n" );

		fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
		fprintf( fpt , "<Sch_Multi_ProcessJob_Buffer>[%d]----------------------------------------------------------\n" , Sch_Multi_PRJOB_Buf_Sub_Error );
		fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
		switch( Sch_Multi_ProcessJob_Buffer->ControlStatus ) {
		case PRJOB_STS_NOTSTATE			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_NOTSTATE)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_QUEUED			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_QUEUED)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_SETTINGUP		:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_SETTINGUP)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_WAITINGFORSTART	:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_WAITINGFORSTART)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_PROCESSING		:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PROCESSING)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_PROCESSCOMPLETED :	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PROCESSCOMPLETED)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_EXECUTING		:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_EXECUTING)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_PAUSING			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PAUSING)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_PAUSE			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PAUSE)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_STOPPING			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_STOPPING)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		case PRJOB_STS_ABORTING			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_ABORTING)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		default							:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_UNKNOWN)\n" , Sch_Multi_ProcessJob_Buffer->ControlStatus );	break;
		}
		switch( Sch_Multi_ProcessJob_Buffer->ResultStatus ) {
		case PRJOB_RESULT_NORMAL		:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_NORMAL)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		case PRJOB_RESULT_ABORT			:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_ABORT)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		case PRJOB_RESULT_ERROR			:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_ERROR)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		case PRJOB_RESULT_ENDSTOP		:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_ENDSTOP)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		case PRJOB_RESULT_CANCELDELETE	:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_CANCELDELETE)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		case PRJOB_RESULT_FORCEFINISH	:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_FORCEFINISH)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		default							:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_UNKNOWN)\n" , Sch_Multi_ProcessJob_Buffer->ResultStatus );	break;
		}
		fprintf( fpt , "  JobID                      = %s\n" , Sch_Multi_ProcessJob_Buffer->JobID );
		fprintf( fpt , "  StartMode                  = %d\n" , Sch_Multi_ProcessJob_Buffer->StartMode );
		fprintf( fpt , "  MtlFormat                  = %d\n" , Sch_Multi_ProcessJob_Buffer->MtlFormat );
//		fprintf( fpt , "  MtlRecipeName              = %s\n" , Sch_Multi_ProcessJob_Buffer->MtlRecipeName ); // 2013.04.11
		fprintf( fpt , "  MtlRecipeName              = %c%s%c\n" , '"' , Sch_Multi_ProcessJob_Buffer->MtlRecipeName , '"' ); // 2013.04.11
		fprintf( fpt , "  MtlRecipeMode              = %d\n" , Sch_Multi_ProcessJob_Buffer->MtlRecipeMode );
		for ( j = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
			if ( Sch_Multi_ProcessJob_Buffer->MtlUse[j] ) {
				fprintf( fpt , "  [%02d] MtlUse                = %d\n" , j+1 , Sch_Multi_ProcessJob_Buffer->MtlUse[j] );
				/*
				// 2013.04.11
				fprintf( fpt , "       MtlCarrName           = %s\n" , Sch_Multi_ProcessJob_Buffer->MtlCarrName[j] );
				for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
					if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[j][k] != 0 ) {
						fprintf( fpt , "      [%02d] MtlSlot           = %d\n" , k+1 , Sch_Multi_ProcessJob_Buffer->MtlSlot[j][k] );
					}
				}
				*/
				// 2013.04.11
				fprintf( fpt , "       MtlCarrName           = %c%s%c\n" , '"' , Sch_Multi_ProcessJob_Buffer->MtlCarrName[j] , '"' );
				fprintf( fpt , "       MtlSlot               = " );
				for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
					if ( Sch_Multi_ProcessJob_Buffer->MtlSlot[j][k] != 0 ) {
						fprintf( fpt , "[%d,%d]" , k+1 , Sch_Multi_ProcessJob_Buffer->MtlSlot[j][k] );
					}
				}
				fprintf( fpt , "\n" );
				//
			}
		}
		//
		fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
		fprintf( fpt , "<Sch_Multi_ControlJob_Buffer>[%d]----------------------------------------------------------\n" , Sch_Multi_CJJOB_Buf_Sub_Error );
		fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
		switch( Sch_Multi_ControlJob_Buffer->ControlStatus ) {
		case CTLJOB_STS_NOTSTATE		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_NOTSTATE)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		case CTLJOB_STS_QUEUED			:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_QUEUED)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		case CTLJOB_STS_SELECTED		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_SELECTED)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		case CTLJOB_STS_WAITINGFORSTART	:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_WAITINGFORSTART)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		case CTLJOB_STS_EXECUTING		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_EXECUTING)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		case CTLJOB_STS_PAUSED			:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_PAUSED)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		case CTLJOB_STS_COMPLETED		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_COMPLETED)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		default							:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_UNKNOWN)\n" , Sch_Multi_ControlJob_Buffer->ControlStatus );	break;
		}
		fprintf( fpt , "  ResultStatus               = %d\n" , Sch_Multi_ControlJob_Buffer->ResultStatus );
//		fprintf( fpt , "  JobID                      = %s\n" , Sch_Multi_ControlJob_Buffer->JobID ); // 2013.04.11
		fprintf( fpt , "  JobID                      = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Buffer->JobID , '"' ); // 2013.04.11
		fprintf( fpt , "  StartMode                  = %d\n" , Sch_Multi_ControlJob_Buffer->StartMode );
		fprintf( fpt , "  PRJobDelete                = %d\n" , Sch_Multi_ControlJob_Buffer->PRJobDelete );
		fprintf( fpt , "  OrderMode                  = %d\n" , Sch_Multi_ControlJob_Buffer->OrderMode );

		for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
			if ( Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] ) {
				fprintf( fpt , "  [%02d] CtrlSpec_Use          = %d\n" , j+1 , Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[j] );
//				fprintf( fpt , "       CtrlSpec_ProcessJobID = %s\n" , Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] ); // 2013.04.11
				fprintf( fpt , "       CtrlSpec_ProcessJobID = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[j] , '"' ); // 2013.04.11
			}
		}
		//
	// 2011.05.21
		// 2011.02.15(Testing)
		for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
			if ( Sch_Multi_ControlJob_Buffer->CassOutUse[j] > 0 ) {
				fprintf( fpt , "  [%02d] CassInUse             = %d\n" , j+1 , Sch_Multi_ControlJob_Buffer->CassInUse[j] );
				fprintf( fpt , "        CassOutUse           = %d\n" , Sch_Multi_ControlJob_Buffer->CassOutUse[j] );
//				fprintf( fpt , "        CassSrcID            = %s\n" , Sch_Multi_ControlJob_Buffer->CassSrcID[j] ); // 2013.04.11
//				fprintf( fpt , "        CassOutID            = %s\n" , Sch_Multi_ControlJob_Buffer->CassOutID[j] ); // 2013.04.11
				fprintf( fpt , "        CassSrcID            = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Buffer->CassSrcID[j] , '"' ); // 2013.04.11
				fprintf( fpt , "        CassOutID            = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Buffer->CassOutID[j] , '"' ); // 2013.04.11
				//
				/*
				// 2013.04.11
				for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
					if ( ( Sch_Multi_ControlJob_Buffer->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob_Buffer->CassSlotOut[j][k] != 0 ) ) {
						fprintf( fpt , "         [%02d] CassSlotIn     = %d\n" , k + 1 , Sch_Multi_ControlJob_Buffer->CassSlotIn[j][k] );
						fprintf( fpt , "         [%02d] CassSlotOut    = %d\n" , k + 1 , Sch_Multi_ControlJob_Buffer->CassSlotOut[j][k] );
					}
				}
				*/
				//
				// 2013.04.11
				for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
					if ( ( Sch_Multi_ControlJob_Buffer->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob_Buffer->CassSlotOut[j][k] != 0 ) ) {
						break;
					}
				}
				if ( k != MAX_MULTI_CASS_SLOT_SIZE ) {
					fprintf( fpt , "        CassSlotIn/SlotOut   = " );
					for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						if ( ( Sch_Multi_ControlJob_Buffer->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob_Buffer->CassSlotOut[j][k] != 0 ) ) {
							fprintf( fpt , "[%d:%d->%d]" , k + 1 , Sch_Multi_ControlJob_Buffer->CassSlotIn[j][k] , Sch_Multi_ControlJob_Buffer->CassSlotOut[j][k] );
						}
					}
					fprintf( fpt , "\n" );
				}
				//
			}
		}
		//
		fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
		fprintf( fpt , "<Sch_Multi_ProcessJob_OrderMode = %02d>-----------------------------------------------------\n" , *Sch_Multi_ProcessJob_OrderMode );
		fprintf( fpt , "------------------------------------------------------------------------------------------\n" );

		for ( i = 0 ; i < *Sch_Multi_ProcessJob_Size ; i++ ) {
			fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
			fprintf( fpt , "<Sch_Multi_ProcessJob[%02d/%02d]>-------------------------------------------------------------\n" , i + 1 , *Sch_Multi_ProcessJob_Size );
			fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
			switch( Sch_Multi_ProcessJob[i]->ControlStatus ) {
			case PRJOB_STS_NOTSTATE			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_NOTSTATE)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_QUEUED			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_QUEUED)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_SETTINGUP		:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_SETTINGUP)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_WAITINGFORSTART	:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_WAITINGFORSTART)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_PROCESSING		:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PROCESSING)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_PROCESSCOMPLETED :	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PROCESSCOMPLETED)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_EXECUTING		:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_EXECUTING)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_PAUSING			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PAUSING)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_PAUSE			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_PAUSE)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_STOPPING			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_STOPPING)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			case PRJOB_STS_ABORTING			:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_ABORTING)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			default							:	fprintf( fpt , "  ControlStatus              = %d(PRJOB_STS_UNKNOWN)\n" , Sch_Multi_ProcessJob[i]->ControlStatus );	break;
			}
			//
			/*
			//
			// 2018.11.14
			//
			switch( Sch_Multi_ProcessJob[i]->ResultStatus ) {
			case PRJOB_RESULT_NORMAL		:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_NORMAL)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			case PRJOB_RESULT_ABORT			:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_ABORT)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			case PRJOB_RESULT_ERROR			:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_ERROR)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			case PRJOB_RESULT_ENDSTOP		:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_ENDSTOP)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			case PRJOB_RESULT_CANCELDELETE	:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_CANCELDELETE)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			case PRJOB_RESULT_FORCEFINISH	:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_FORCEFINISH)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			default							:	fprintf( fpt , "  ResultStatus               = %d(PRJOB_RESULT_UNKNOWN)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			}
			//
			*/
			//
			//
			// 2018.11.14
			//
			switch( Sch_Multi_ProcessJob[i]->ResultStatus ) {
			case 0							:	fprintf( fpt , "  ResultStatus               = %d(NOTUSE)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			default							:	fprintf( fpt , "  ResultStatus               = %d(Referred)\n" , Sch_Multi_ProcessJob[i]->ResultStatus );	break;
			}
			//
			//
//			fprintf( fpt , "  JobID                      = %s\n" , Sch_Multi_ProcessJob[i]->JobID ); // 2013.04.11
			fprintf( fpt , "  JobID                      = %c%s%c\n" , '"' , Sch_Multi_ProcessJob[i]->JobID , '"' ); // 2013.04.11
			fprintf( fpt , "  StartMode                  = %d\n" , Sch_Multi_ProcessJob[i]->StartMode );
			fprintf( fpt , "  MtlFormat                  = %d\n" , Sch_Multi_ProcessJob[i]->MtlFormat );
//			fprintf( fpt , "  MtlRecipeName              = %s\n" , Sch_Multi_ProcessJob[i]->MtlRecipeName ); // 2013.04.11
			fprintf( fpt , "  MtlRecipeName              = %c%s%c\n" , '"' , Sch_Multi_ProcessJob[i]->MtlRecipeName , '"' ); // 2013.04.11
			fprintf( fpt , "  MtlRecipeMode              = %d\n" , Sch_Multi_ProcessJob[i]->MtlRecipeMode );
			for ( j = 0 ; j < MAX_MULTI_PRJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_ProcessJob[i]->MtlUse[j] ) {
					fprintf( fpt , "  [%02d] MtlUse                = %d\n" , j+1 , Sch_Multi_ProcessJob[i]->MtlUse[j] );
					//
					/*
					// 2013.04.11
					fprintf( fpt , "       MtlCarrName           = %s\n" , Sch_Multi_ProcessJob[i]->MtlCarrName[j] );
					for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						if ( Sch_Multi_ProcessJob[i]->MtlSlot[j][k] != 0 ) {
							fprintf( fpt , "      [%02d] MtlSlot           = %d\n" , k+1 , Sch_Multi_ProcessJob[i]->MtlSlot[j][k] );
						}
					}
					*/
					// 2013.04.11
					fprintf( fpt , "       MtlCarrName           = %c%s%c\n" , '"' , Sch_Multi_ProcessJob[i]->MtlCarrName[j] , '"' );
					fprintf( fpt , "       MtlSlot               = " );
					for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						if ( Sch_Multi_ProcessJob[i]->MtlSlot[j][k] != 0 ) {
							fprintf( fpt , "[%d,%d]" , k+1 , Sch_Multi_ProcessJob[i]->MtlSlot[j][k] );
						}
					}
					fprintf( fpt , "\n" );
					//
				}
			}
			//----------
			// 2008.01.02
			//----------
			fprintf( fpt , "   : RECIPE TUNE = Mode=%d\n" , Sch_Multi_ProcessJob[i]->MtlRecipeMode );
			//
			for ( j = 0 ; j < MAX_MULTI_PRJOB_RCPTUNE ; j++ ) {
				if ( Sch_Multi_ProcessJob[i]->MtlRcpTuneModule[j] > 0 ) {
					fprintf( fpt , "      [%d] PM%d %d:%s:%s\n" , j + 1 , Sch_Multi_ProcessJob[i]->MtlRcpTuneModule[j] , Sch_Multi_ProcessJob[i]->MtlRcpTuneStep[j] , Sch_Multi_ProcessJob[i]->MtlRcpTuneName[j] , Sch_Multi_ProcessJob[i]->MtlRcpTuneData[j] );
				}
			}
			//----------
			if ( _SCH_MULTIJOB_CHECK_EXTEND_TUNE_USE() > 0 ) { // 2007.11.29
				fprintf( fpt , "   : RECIPE TUNE2\n" );
				for ( j = 0 ; j < MAX_MULTI_PRJOB_RCPTUNE_EXTEND ; j++ ) {
					if ( Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneModule[j] > 0 ) {
						fprintf( fpt , "      [%d] PM%d O=%d %d:%s:%s X=%d\n" , j + 1 ,
							Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneModule[j] ,
							Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneOrder[j] ,
							Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneStep[j] ,
							Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneName[j] ,
							Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneData[j] ,
							Sch_Multi_ProcessJob_Extend_RcpTuneArea[i]->MtlRcpTuneIndex[j] );
					}
				}
			}
			//----------
			Pres_Time = localtime( &(Sch_Multi_Ins_ProcessJob[i].InsertTime) ); // 2018.11.14
			fprintf( fpt , "   : Insert_Time = %04d/%02d/%02d %02d:%02d:%02d\n" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec ); // 2018.11.14
			fprintf( fpt , "   : FixedResult = %d\n" , Sch_Multi_Ins_ProcessJob[i].FixedResult ); // 2018.11.14
			//----------
		}
		//
		for ( i = 0 ; i < *Sch_Multi_ControlJob_Size ; i++ ) {
			fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
			fprintf( fpt , "<Sch_Multi_ControlJob[%02d/%02d]>-------------------------------------------------------------\n" , i + 1 , *Sch_Multi_ControlJob_Size );
			fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
			switch( Sch_Multi_ControlJob[i]->ControlStatus ) {
			case CTLJOB_STS_NOTSTATE		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_NOTSTATE)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			case CTLJOB_STS_QUEUED			:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_QUEUED)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			case CTLJOB_STS_SELECTED		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_SELECTED)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			case CTLJOB_STS_WAITINGFORSTART	:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_WAITINGFORSTART)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			case CTLJOB_STS_EXECUTING		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_EXECUTING)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			case CTLJOB_STS_PAUSED			:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_PAUSED)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			case CTLJOB_STS_COMPLETED		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_COMPLETED)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			default							:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_UNKNOWN)\n" , Sch_Multi_ControlJob[i]->ControlStatus );	break;
			}
			fprintf( fpt , "  ResultStatus               = %d\n" , Sch_Multi_ControlJob[i]->ResultStatus );
			fprintf( fpt , "  JobID                      = %c%s%c\n" , '"' , Sch_Multi_ControlJob[i]->JobID , '"' ); // 2013.04.11
			fprintf( fpt , "  StartMode                  = %d\n" , Sch_Multi_ControlJob[i]->StartMode );
			fprintf( fpt , "  PRJobDelete                = %d\n" , Sch_Multi_ControlJob[i]->PRJobDelete );
			fprintf( fpt , "  OrderMode                  = %d\n" , Sch_Multi_ControlJob[i]->OrderMode );
			fprintf( fpt , "  Queued_to_Select_Wait      = %d\n" , Sch_Multi_Ins_ControlJob[i].Queued_to_Select_Wait ); // 2012.09.26

			for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_ControlJob[i]->CtrlSpec_Use[j] ) {
					fprintf( fpt , "  [%02d] CtrlSpec_Use          = %d\n" , j+1 , Sch_Multi_ControlJob[i]->CtrlSpec_Use[j] );
					fprintf( fpt , "       CtrlSpec_ProcessJobID = %c%s%c\n" , '"' , Sch_Multi_ControlJob[i]->CtrlSpec_ProcessJobID[j] , '"' ); // 2013.04.11
				}
			}
			//
	// 2011.05.21
			// 2011.02.15(Testing)
			for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_ControlJob[i]->CassOutUse[j] > 0 ) {
					fprintf( fpt , "  [%02d] CassInUse             = %d\n" , j+1 , Sch_Multi_ControlJob[i]->CassInUse[j] );
					fprintf( fpt , "        CassOutUse           = %d\n" , Sch_Multi_ControlJob[i]->CassOutUse[j] );
					fprintf( fpt , "        CassSrcID            = %c%s%c\n" , '"' , Sch_Multi_ControlJob[i]->CassSrcID[j] , '"' ); // 2013.04.11
					fprintf( fpt , "        CassOutID            = %c%s%c\n" , '"' , Sch_Multi_ControlJob[i]->CassOutID[j] , '"' ); // 2013.04.11
					//
					/*
					// 2013.04.11
					for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						if ( ( Sch_Multi_ControlJob[i]->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob[i]->CassSlotOut[j][k] != 0 ) ) {
							fprintf( fpt , "         [%02d] CassSlotIn     = %d\n" , k + 1 , Sch_Multi_ControlJob[i]->CassSlotIn[j][k] );
							fprintf( fpt , "         [%02d] CassSlotOut    = %d\n" , k + 1 , Sch_Multi_ControlJob[i]->CassSlotOut[j][k] );
						}
					}
					//
					*/
					//
					// 2013.04.11
					for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
						if ( ( Sch_Multi_ControlJob[i]->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob[i]->CassSlotOut[j][k] != 0 ) ) {
							break;
						}
					}
					if ( k != MAX_MULTI_CASS_SLOT_SIZE ) {
						fprintf( fpt , "        CassSlotIn/SlotOut   = " );
						for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							if ( ( Sch_Multi_ControlJob[i]->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob[i]->CassSlotOut[j][k] != 0 ) ) {
								fprintf( fpt , "[%d:%d->%d]" , k + 1 , Sch_Multi_ControlJob[i]->CassSlotIn[j][k] , Sch_Multi_ControlJob[i]->CassSlotOut[j][k] );
							}
						}
						fprintf( fpt , "\n" );
					}
					//
				}
			}
			//----------
			Pres_Time = localtime( &(Sch_Multi_Ins_ControlJob[i].InsertTime) ); // 2018.11.14
			fprintf( fpt , "  : Insert_Time = %04d/%02d/%02d %02d:%02d:%02d\n" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec ); // 2018.11.14
			fprintf( fpt , "  : Queued_S_W  = %d\n" , Sch_Multi_Ins_ControlJob[i].Queued_to_Select_Wait ); // 2018.11.14
			fprintf( fpt , "  : Move_Mode   = %d\n" , Sch_Multi_Ins_ControlJob[i].MoveMode ); // 2018.11.14
			fprintf( fpt , "  : FixedResult = %d\n" , Sch_Multi_Ins_ControlJob[i].FixedResult ); // 2018.11.14
			//----------
		}
		//
		for ( i = 0 ; i < MAX_MULTI_CTJOBSELECT_SIZE ; i++ ) {
			if ( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus != CTLJOB_STS_NOTSTATE ) {
				fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
				fprintf( fpt , "<Sch_Multi_ControlJob_Select_Data[%02d/%02d]>-------------------------------------------------\n" , i + 1 , MAX_MULTI_CTJOBSELECT_SIZE );
				fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
				switch( Sch_Multi_ControlJob_Select_Data[i]->ControlStatus ) {
				case CTLJOB_STS_NOTSTATE		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_NOTSTATE)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				case CTLJOB_STS_QUEUED			:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_QUEUED)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				case CTLJOB_STS_SELECTED		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_SELECTED)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				case CTLJOB_STS_WAITINGFORSTART	:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_WAITINGFORSTART)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				case CTLJOB_STS_EXECUTING		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_EXECUTING)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				case CTLJOB_STS_PAUSED			:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_PAUSED)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				case CTLJOB_STS_COMPLETED		:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_COMPLETED)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				default							:	fprintf( fpt , "  ControlStatus              = %d(CTLJOB_STS_UNKNOWN)\n" , Sch_Multi_ControlJob_Select_Data[i]->ControlStatus );	break;
				}
				fprintf( fpt , "  ResultStatus               = %d\n" , Sch_Multi_ControlJob_Select_Data[i]->ResultStatus );
				fprintf( fpt , "  JobID                      = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Select_Data[i]->JobID , '"' ); // 2013.04.11
				fprintf( fpt , "  StartMode                  = %d\n" , Sch_Multi_ControlJob_Select_Data[i]->StartMode );
				fprintf( fpt , "  PRJobDelete                = %d\n" , Sch_Multi_ControlJob_Select_Data[i]->PRJobDelete );
				fprintf( fpt , "  OrderMode                  = %d\n" , Sch_Multi_ControlJob_Select_Data[i]->OrderMode );

				for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
					if ( Sch_Multi_ControlJob_Select_Data[i]->CtrlSpec_Use[j] ) {
						fprintf( fpt , "  [%02d] CtrlSpec_Use          = %d\n" , j+1 , Sch_Multi_ControlJob_Select_Data[i]->CtrlSpec_Use[j] );
						fprintf( fpt , "       CtrlSpec_ProcessJobID = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Select_Data[i]->CtrlSpec_ProcessJobID[j] , '"' ); // 2013.04.11
					}
				}
				//
	// 2011.05.21
				// 2011.02.15(Testing)
				for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
					if ( Sch_Multi_ControlJob_Select_Data[i]->CassOutUse[j] > 0 ) {
						fprintf( fpt , "  [%02d] CassInUse             = %d\n" , j+1 , Sch_Multi_ControlJob_Select_Data[i]->CassInUse[j] );
						fprintf( fpt , "        CassOutUse           = %d\n" , Sch_Multi_ControlJob_Select_Data[i]->CassOutUse[j] );
						fprintf( fpt , "        CassSrcID            = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Select_Data[i]->CassSrcID[j] , '"' ); // 2013.04.11
						fprintf( fpt , "        CassOutID            = %c%s%c\n" , '"' , Sch_Multi_ControlJob_Select_Data[i]->CassOutID[j] , '"' ); // 2013.04.11
						//
						/*
						//
						// 2013.04.11
						for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							if ( ( Sch_Multi_ControlJob_Select_Data[i]->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob_Select_Data[i]->CassSlotOut[j][k] != 0 ) ) {
								fprintf( fpt , "         [%02d] CassSlotIn     = %d\n" , k + 1 , Sch_Multi_ControlJob_Select_Data[i]->CassSlotIn[j][k] );
								fprintf( fpt , "         [%02d] CassSlotOut    = %d\n" , k + 1 , Sch_Multi_ControlJob_Select_Data[i]->CassSlotOut[j][k] );
							}
						}
						//
						//
						*/
						//
						// 2013.04.11
						for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
							if ( ( Sch_Multi_ControlJob_Select_Data[i]->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob_Select_Data[i]->CassSlotOut[j][k] != 0 ) ) {
								break;
							}
						}
						if ( k != MAX_MULTI_CASS_SLOT_SIZE ) {
							fprintf( fpt , "        CassSlotIn/SlotOut   = " );
							for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
								if ( ( Sch_Multi_ControlJob_Select_Data[i]->CassSlotIn[j][k] != 0 ) || ( Sch_Multi_ControlJob_Select_Data[i]->CassSlotOut[j][k] != 0 ) ) {
									fprintf( fpt , "[%d:%d->%d]" , k + 1 , Sch_Multi_ControlJob_Select_Data[i]->CassSlotIn[j][k] , Sch_Multi_ControlJob_Select_Data[i]->CassSlotOut[j][k] );
								}
							}
							fprintf( fpt , "\n" );
						}
						//
					}
				}
				//
				fprintf( fpt , "  #Selected_Pause_Signal     = %d\n" , Sch_Multi_ControlJob_Selected_Pause_Signal[i] );
				fprintf( fpt , "  #Selected_Stop_Signal      = %d\n" , Sch_Multi_ControlJob_Selected_Stop_Signal[i] );
				//----------
				Pres_Time = localtime( &(Sch_Multi_Ins_ControlJob_Select_Data[i].InsertTime) ); // 2018.11.14
				fprintf( fpt , "  : Insert_Time = %04d/%02d/%02d %02d:%02d:%02d\n" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec ); // 2018.11.14
				fprintf( fpt , "  : Queued_S_W  = %d\n" , Sch_Multi_Ins_ControlJob_Select_Data[i].Queued_to_Select_Wait ); // 2018.11.14
				fprintf( fpt , "  : Move_Mode   = %d\n" , Sch_Multi_Ins_ControlJob_Select_Data[i].MoveMode ); // 2018.11.14
				fprintf( fpt , "  : FixedResult = %d\n" , Sch_Multi_Ins_ControlJob_Select_Data[i].FixedResult ); // 2018.11.14
				//----------
			}
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
			fprintf( fpt , "<Sch_Multi_CassRunJob_Data[%02d]>-----------------------------------------------------------\n" , i + 1 );
			fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
			fprintf( fpt , "  CarrierID                  = %c%s%c\n" , '"' , Sch_Multi_CassRunJob_Data[i]->CarrierID , '"' ); // 2013.04.11
			fprintf( fpt , "  MtlCJName                  = %c%s%c\n" , '"' , Sch_Multi_CassRunJob_Data[i]->MtlCJName , '"' ); // 2013.04.11
			for ( j = 0 ; j < MAX_MULTI_CTJOB_CASSETTE ; j++ ) {
				if ( Sch_Multi_CassRunJob_Data[i]->MtlPJName[j][0] != 0 ) {
//					fprintf( fpt , "  [%02d] MtlPJName             = %c%s%c\n" , j+1 , '"' , Sch_Multi_CassRunJob_Data[i]->MtlPJName[j] , '"' ); // 2013.04.11 // 2013.05.27
//					fprintf( fpt , "       MtlOutSlot            = %d\n" , Sch_Multi_CassRunJob_Data[i]->MtlOutSlot[j] ); // 2013.05.27
					fprintf( fpt , "  [%02d] MtlOutSlot [%-3d]      = %c%s%c\n" , j+1 , Sch_Multi_CassRunJob_Data[i]->MtlOutSlot[j], '"' , Sch_Multi_CassRunJob_Data[i]->MtlPJName[j] , '"' ); // 2013.04.11
				}
			}
			//
			switch( Sch_Multi_Cassette_Verify[i] )	{
			case CASSETTE_VERIFY_READY			: fprintf( fpt , "  #Cassette_Verify           = Ready(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			case CASSETTE_VERIFY_WAIT_CONFIRM	: fprintf( fpt , "  #Cassette_Verify           = Wait-Confirm(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			case CASSETTE_VERIFY_CONFIRMING		: fprintf( fpt , "  #Cassette_Verify           = Confirming(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			case CASSETTE_VERIFY_CONFIRMED		: fprintf( fpt , "  #Cassette_Verify           = Confirmed(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			case CASSETTE_VERIFY_CONFIRMED2		: fprintf( fpt , "  #Cassette_Verify           = Confirmed2(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			case CASSETTE_VERIFY_RUNNING		: fprintf( fpt , "  #Cassette_Verify           = Running(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			default								: fprintf( fpt , "  #Cassette_Verify           = Unknown(%d)\n" , Sch_Multi_Cassette_Verify[i] ); break;
			}
			//
			if ( Sch_Multi_Cassette_MID[i] == NULL ) { // 2008.04.02
				fprintf( fpt , "  #Cassette_MID              = {NULL}\n" );
			}
			else {
				fprintf( fpt , "  #Cassette_MID              = %c%s%c\n" , '"' , Sch_Multi_Cassette_MID[i] , '"' ); // 2013.04.11
			}
			if ( Sch_Multi_Cassette_MID_for_Start[i] == NULL ) { // 2011.03.10
				fprintf( fpt , "  #Cassette_MID_for_Start    = {NULL}\n" );
			}
			else {
				fprintf( fpt , "  #Cassette_MID_for_Start    = %c%s%c\n" , '"' , Sch_Multi_Cassette_MID_for_Start[i] , '"' ); // 2013.04.11
			}
		}
		//
		for ( i = 0 ; i < MAX_MULTI_RUNJOB_SIZE ; i++ ) {
			if ( ( MULTI_RUNJOB_RunStatus(i) != CASSRUNJOB_STS_READY ) || ( MULTI_RUNJOB_MtlCount(i) > 0 ) ) { // 2011.04.25
				fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
				fprintf( fpt , "<Sch_Multi_RunJob_SelectData[%02d/%02d]>-----------------------------------------------------\n" , i + 1 , MAX_MULTI_RUNJOB_SIZE );
				fprintf( fpt , "------------------------------------------------------------------------------------------\n" );
				switch( MULTI_RUNJOB_RunStatus(i) ) {
				case CASSRUNJOB_STS_READY						:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_READY)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_WAITING						:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_WAITING)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_RUNNING						:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_RUNNING)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_FINISH_WITH_NORMAL			:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_FINISH_WITH_NORMAL)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_FINISH_WITH_ABORT			:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_FINISH_WITH_ABORT)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_FINISH_WITH_ERROR			:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_FINISH_WITH_ERROR)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_FINISH_WITH_ENDSTOP			:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_FINISH_WITH_ENDSTOP)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_FINISH_WITH_CANCELDELETE	:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_FINISH_WITH_CANCELDELETE)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				case CASSRUNJOB_STS_FINISH_WITH_FORCEFINISH		:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_FINISH_WITH_FORCEFINISH)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				default											:	fprintf( fpt , "  RunStatus                  = %d(CASSRUNJOB_STS_UNKNOWN)\n" , MULTI_RUNJOB_RunStatus(i) );	break;
				}

				fprintf( fpt , "  Side                       = %d\n" , MULTI_RUNJOB_RunSide(i) ); // 2018.08.30

				fprintf( fpt , "  Cassette                   = %d\n" , MULTI_RUNJOB_Cassette(i) );

				fprintf( fpt , "  CassetteIndex              = %d\n" , MULTI_RUNJOB_CassetteIndex(i) ); // 2018.11.15

				fprintf( fpt , "  SelectCJIndex              = %d\n" , MULTI_RUNJOB_SelectCJIndex(i) );
				fprintf( fpt , "  SelectCJOrder              = %d\n" , MULTI_RUNJOB_SelectCJOrder(i) ); // 2013.09.27
				fprintf( fpt , "  CarrierID                  = %c%s%c\n" , '"' , MULTI_RUNJOB_CarrierID( i ) , '"' ); // 2013.04.11

				fprintf( fpt , "  MtlCJ_StartMode            = %d\n" , MULTI_RUNJOB_MtlCJ_StartMode( i ) );
				fprintf( fpt , "  MtlCJName                  = %c%s%c\n" , '"' , MULTI_RUNJOB_MtlCJName( i ) , '"' ); // 2013.04.11
				fprintf( fpt , "  MtlCount                   = %d\n" , MULTI_RUNJOB_MtlCount(i) );


				// 2013.04.11
				fprintf( fpt , "       StartMode PJName               PJID PJIDRes    PJIDEnd InCass InCarrierID          InCIdx InSlot OutCass OutCarrierID         OutCIdx OutSlot RcpMode RecipeName                     Side Pointer\n" );
				//
//				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
				for ( j = 0 ; j < MAX_RUN_CASSETTE_SLOT_SIZE ; j++ ) { // 2018.05.10
					if ( MULTI_RUNJOB_MtlPJName(i)[j][0] != 0 ) {
						//
						//==================
						//
						/*
						// 2014.05.22
						//
						fprintf( fpt , "  [%02d] %-9d %-20s %-4d " , j+1 , MULTI_RUNJOB_MtlPJ_StartMode(i)[j]
																, MULTI_RUNJOB_MtlPJName( i )[j]
																, MULTI_RUNJOB_MtlPJID(i)[j] );
						*/
						//
						// 2014.05.22
						//
						fprintf( fpt , "  [%02d] " , j+1 );
						//
						switch ( MULTI_RUNJOB_MtlPJ_StartMode(i)[j] ) {
						case PJ_SIGNAL_RUN			:	fprintf( fpt , "%02d:RUN   " , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						case PJ_SIGNAL_WAIT			:	fprintf( fpt , "%02d:WAIT  " , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						case PJ_SIGNAL_FINISH		:	fprintf( fpt , "%02d:FINISH" , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						case PJ_SIGNAL_PAUSE_RUN	:	fprintf( fpt , "%02d:RUN.P " , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						case PJ_SIGNAL_PAUSE_WAIT	:	fprintf( fpt , "%02d:WAIT.P" , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						case PJ_SIGNAL_PAUSE_FINISH :	fprintf( fpt , "%02d:FINS.P" , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						default						:	fprintf( fpt , "%02d:      " , MULTI_RUNJOB_MtlPJ_StartMode(i)[j] );	break;
						}
						//
						fprintf( fpt , " %-20s %-4d " , MULTI_RUNJOB_MtlPJName( i )[j] , MULTI_RUNJOB_MtlPJID(i)[j] );
						//
						//==================
						//
						switch ( MULTI_RUNJOB_MtlPJIDRes(i)[j] ) {
						case PJIDRES_READY :	fprintf( fpt , "%02d:READY  " , MULTI_RUNJOB_MtlPJIDRes(i)[j] );	break;
						case PJIDRES_FIRST :	fprintf( fpt , "%02d:FIRST  " , MULTI_RUNJOB_MtlPJIDRes(i)[j] );	break;
						case PJIDRES_RUNNING :	fprintf( fpt , "%02d:RUNNING" , MULTI_RUNJOB_MtlPJIDRes(i)[j] );	break;
						case PJIDRES_LAST	 :	fprintf( fpt , "%02d:LAST   " , MULTI_RUNJOB_MtlPJIDRes(i)[j] );	break;
						case PJIDRES_FINISH :	fprintf( fpt , "%02d:FINISH " , MULTI_RUNJOB_MtlPJIDRes(i)[j] );	break;
						default :				fprintf( fpt , "%02d:       " , MULTI_RUNJOB_MtlPJIDRes(i)[j] );	break;
						}
						//
						fprintf( fpt , " %-7d %-6d %-20s %-6d %-6d %-7d %-20s %-7d %-7d %-7d %-30s %-4d %-7d\n" , MULTI_RUNJOB_MtlPJIDEnd(i)[j]
												, MULTI_RUNJOB_MtlInCassette(i)[j]
												, MULTI_RUNJOB_MtlInCarrierID(i)[j]
												, MULTI_RUNJOB_MtlInCarrierIndex(i)[j]
												, MULTI_RUNJOB_MtlInSlot(i)[j]
												, MULTI_RUNJOB_MtlOutCassette(i)[j]
												, MULTI_RUNJOB_MtlOutCarrierID(i)[j]
												, MULTI_RUNJOB_MtlOutCarrierIndex(i)[j]
												, MULTI_RUNJOB_MtlOutSlot(i)[j]
												, MULTI_RUNJOB_MtlRecipeMode(i)[j]
												, MULTI_RUNJOB_MtlRecipeName(i)[j]
												, MULTI_RUNJOB_Mtl_Side(i)[j]
												, MULTI_RUNJOB_Mtl_Pointer(i)[j]
												);
					}
				}
				//
			}
		}
		fprintf( fpt , "------------------------------------------------------------------------------------------\n" ); // 2005.04.09
		fprintf( fpt , "<Sch_Multi_ProcessJob_Result_Status %d\n" , *Sch_Multi_ProcessJob_Result_Status ); // 2005.04.09
		fprintf( fpt , "<Sch_Multi_ControlJob_Result_Status %d\n" , *Sch_Multi_ControlJob_Result_Status ); // 2005.04.09
		fprintf( fpt , "==========================================================================================================\n" );
	}
	//
	EnterCriticalSection( &CR_MJ_LOG_MSG_QUEUE ); // 2011.04.25
	//
	for ( i = 0 ; i < MJ_LOG_MSG_QUEUE_SIZE ; i++ ) {
		if ( MJ_LOG_MSG_QUEUE_Index_Time[i] == NULL ) continue;
		if ( MJ_LOG_MSG_QUEUE_Index_Data[i] == NULL ) continue;
		//
		fprintf( fpt , "[%02d][%s][%s]\n" , i , MJ_LOG_MSG_QUEUE_Index_Time[i] , MJ_LOG_MSG_QUEUE_Index_Data[i] );
		//
	}
	//
	LeaveCriticalSection( &CR_MJ_LOG_MSG_QUEUE );
	//
	fclose(fpt);
}
















































//// TESTING

/*


//---------------------------------------------------------------------------------------
void CP_Process_Job_Insert( char *JobName , char *MidName , char *RecipeName , int StartSlot , int EndSlot ) {
	int i , j , Res;

	//-----------------------------------------
	//- User Data Handling Part Begin
	//-----------------------------------------
	strcpy( Sch_Multi_ProcessJob_Buffer->JobID , JobName );
	Sch_Multi_ProcessJob_Buffer->StartMode = 0; // 0:Auto 1:Manual

	for ( i = 0 ; i < MAX_MULTI_PRJOB_CASSETTE ; i++ ) Sch_Multi_ProcessJob_Buffer->MtlUse[i] = FALSE;
	//
	for ( i = 0 ; i < 1 ; i++ ) { // Just 1 Cassettes
		Sch_Multi_ProcessJob_Buffer->MtlUse[i] = TRUE;
		//
		strcpy( Sch_Multi_ProcessJob_Buffer->MtlCarrName[i], MidName );
		strcpy( Sch_Multi_ProcessJob_Buffer->MtlRecipeName, RecipeName );
		//
		Sch_Multi_ProcessJob_Buffer->MtlRecipeMode = 0;
		//
		for ( j = 0 ; j < MAX_MULTI_CASS_SLOT_SIZE ; j++ ) Sch_Multi_ProcessJob_Buffer->MtlSlot[i][j] = 0;
		//
		for ( j = StartSlot ; j <= EndSlot ; j++ ) {
			Sch_Multi_ProcessJob_Buffer->MtlSlot[i][j-StartSlot] = j;
		}
	}
	//== Check Signal
	Sch_Multi_ProcessJob_Buffer->ControlStatus = 1;
	//-----------------------------------------
	//- User Data Handling Part End
	//-----------------------------------------
	Res = SCHMULTI_PROCESSJOB_INSERT( -1 );
	//
	printf( "PROCESSJOB (%s) INSERT = %d\n" , JobName , Res );
	//
}
//---------------------------------------------------------------------------------------
void CP_Control_Job_Insert( char *JobName ,
						   char *PjobName ,
						   char *MtlOut1 ,
						   char *MtlOut2 ,
						   char *MtlOut3 ,
						   char *MtlOut4 ,
						   char *MtlOut5 ,
						   char *MtlOut6 ,
						   char *MtlOut7 ,
						   char *MtlOut8
						   ) {
	int i , k , Res , z , z2 , s1 , s2;
	char Buffer[256];
	char BufferM[256];
	char BufferN1[256];
	char BufferN2[256];

	//-----------------------------------------
	//- User Data Handling Part Begin
	//-----------------------------------------
	//
	strcpy( Sch_Multi_ControlJob_Buffer->JobID , JobName );

	Sch_Multi_ControlJob_Buffer->StartMode = 0; // 0:Auto 1:Manual
	Sch_Multi_ControlJob_Buffer->OrderMode = 0;
	Sch_Multi_ControlJob_Buffer->PRJobDelete = 1; // 0:Remain 1:Delete

	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] = FALSE;
	//
	z = 0;
	i = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( PjobName , ':' , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Sch_Multi_ControlJob_Buffer->CtrlSpec_Use[i] = TRUE;
		//
		strcpy( Sch_Multi_ControlJob_Buffer->CtrlSpec_ProcessJobID[i] , Buffer );
		//
		i++;
		//
		if ( i >= MAX_MULTI_CTJOB_CASSETTE ) break;
		//
		z = z2;
	}
	//
	for ( i = 0 ; i < MAX_MULTI_CTJOB_CASSETTE ; i++ ) {
		Sch_Multi_ControlJob_Buffer->CassInUse[i] = 0;
		Sch_Multi_ControlJob_Buffer->CassOutUse[i] = 0;
		strcpy( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , "" );
		strcpy( Sch_Multi_ControlJob_Buffer->CassOutID[i] , "" );
		for ( k = 0 ; k < MAX_MULTI_CASS_SLOT_SIZE ; k++ ) {
			Sch_Multi_ControlJob_Buffer->CassSlotIn[i][k] = 0;
			Sch_Multi_ControlJob_Buffer->CassSlotOut[i][k] = 0;
		}
	}
	//
	i = 0;


	for ( k = 0 ; k < 8 ; k++ ) {
		//
		if      ( k == 0 ) strcpy( BufferM , MtlOut1 );
		else if ( k == 1 ) strcpy( BufferM , MtlOut2 );
		else if ( k == 2 ) strcpy( BufferM , MtlOut3 );
		else if ( k == 3 ) strcpy( BufferM , MtlOut4 );
		else if ( k == 4 ) strcpy( BufferM , MtlOut5 );
		else if ( k == 5 ) strcpy( BufferM , MtlOut6 );
		else if ( k == 6 ) strcpy( BufferM , MtlOut7 );
		else if ( k == 7 ) strcpy( BufferM , MtlOut8 );
		else break;
		//
		z = 0;
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( BufferM , ':' , BufferN1 , z , 255 );	if ( z == z2 ) continue;	z = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( BufferM , ':' , Buffer   , z , 255 );	if ( z == z2 ) continue;	z = z2;	s1 = atoi( Buffer );
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( BufferM , ':' , BufferN2 , z , 255 );	if ( z == z2 ) continue;	z = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( BufferM , ':' , Buffer   , z , 255 );	if ( z == z2 ) continue;	z = z2;	s2 = atoi( Buffer );
		//
		if ( ( BufferN1[0] != 0 ) && ( BufferN2[0] != 0 ) && ( s1 > 0 ) && ( s2 > 0 ) ) {
			Sch_Multi_ControlJob_Buffer->CassInUse[i] = 1;
			Sch_Multi_ControlJob_Buffer->CassOutUse[i] = 1;
			strcpy( Sch_Multi_ControlJob_Buffer->CassSrcID[i] , BufferN1 );
			strcpy( Sch_Multi_ControlJob_Buffer->CassOutID[i] , BufferN2 );
			Sch_Multi_ControlJob_Buffer->CassSlotIn[i][0] = s1;
			Sch_Multi_ControlJob_Buffer->CassSlotOut[i][0] = s2;
			//
			i++;
			//
			if ( i >= MAX_MULTI_CTJOB_CASSETTE ) break;
			//
		}
	}
	//
	//
	//== Check Signal
	Sch_Multi_ControlJob_Buffer->ControlStatus = 1;
	//-----------------------------------------
	//- User Data Handling Part End
	//-----------------------------------------
	Res = SCHMULTI_CONTROLJOB_INSERT( FALSE );
	//
	printf( "CONTROLJOB (%s) INSERT = %d\n" , JobName , Res );
	//
}


void CP_Job_Insert( char *data ) {
	//
	char Buffer[256];
	char Name[256];
	char Name2[256];
	char Buf1[256];
	char Buf2[256];
	char Buf3[256];
	char Buf4[256];
	char Buf5[256];
	char Buf6[256];
	char Buf7[256];
	char Buf8[256];
	int z , z2 , mode , s1 , s2;

	mode = -1;
	z = 0;
	//
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buffer , z , 255 );
	//
	if ( z == z2 ) return;
	//
	z = z2;
	//
	if      ( STRCMP_L( Buffer , "PJINSERT" ) ) {
		mode = 0;
	}
	else if ( STRCMP_L( Buffer , "CJINSERT" ) ) {
		mode = 1;
	}
	else {
		return;
	}
	//
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Name , z , 255 );	if ( z == z2 ) return;	z = z2;
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Name2 , z , 255 );	if ( z == z2 ) return;	z = z2;
	//
	if      ( mode == 0 ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf1 , z , 255 );	if ( z == z2 ) return;	z = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf2 , z , 255 );	if ( z == z2 ) return;	z = z2;	s1 = atoi( Buf2 );
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf2 , z , 255 );	if ( z == z2 ) return;	z = z2;	s2 = atoi( Buf2 );
		//
		CP_Process_Job_Insert( Name , Name2 , Buf1 , s1 , s2 );
		//
	}
	else {
		//
		strcpy( Buf1 , "" );
		strcpy( Buf2 , "" );
		strcpy( Buf3 , "" );
		strcpy( Buf4 , "" );
		strcpy( Buf5 , "" );
		strcpy( Buf6 , "" );
		strcpy( Buf7 , "" );
		strcpy( Buf8 , "" );
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf1 , z , 255 );
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf2 , z , 255 );
		}
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf3 , z , 255 );
		}
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf4 , z , 255 );
		}
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf5 , z , 255 );
		}
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf6 , z , 255 );
		}
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf7 , z , 255 );
		}
		if ( z != z2 ) {
			z = z2;
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( data , '|' , Buf8 , z , 255 );
		}
		//
		CP_Control_Job_Insert( Name ,
							   Name2 ,
							   Buf1 ,
							   Buf2 ,
							   Buf3 ,
							   Buf4 ,
							   Buf5 ,
							   Buf6 ,
							   Buf7 ,
							   Buf8
							   );
	}
}

int SCHEDULER_CM_LOCKED_for_HANDOFF( int cm );

void SCHMULTI_CONTROLJOB_TEST( int mode , char *data ) {
	int i , j;
	char Buffer[32];


	if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) || ( mode == 4 ) ) {
		//
		_i_SCH_PRM_SET_FA_LOADUNLOAD_SKIP( 7 );
		//
	}
	//
	if ( ( mode == 0 ) || ( mode == 11 ) || ( mode == 12 ) || ( mode == 13 ) || ( mode == 14 ) ) {

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( ( mode != 0 ) && ( ( mode - 11 ) != i ) ) continue;
			//
			j = SCHEDULER_CM_LOCKED_for_HANDOFF( i + CM1 );
			//
			if ( j > 0 ) {

				printf( "===============================================\n" );
				printf( "===============================================\n" );
				printf( "MAPPING SET IMPOSSIBLE (CM%d) LOCKED STATUS(%d)\n" , i + 1 , j );
				printf( "===============================================\n" );
				printf( "===============================================\n" );

				continue;
			}
			//
			switch( BUTTON_GET_FLOW_STATUS_VAR( i ) ) {
			case 0 : // Init
			case 2 : // Loading(Success)
			case 3 : // Loading(Fail)
			case 5 : // Mapping(Success)
			case 6 : // Mapping(Fail)
			case 15 : // Running(Success)
			case 16 : // Running(Success with Cancel)
			case 17 : // Running(Aborted)
			case 20 :
			case 21 : // Unloading(Success)
			case 22 : // Unloading(Fail)
				//
				for ( j = 0 ; j < 20 ; j++ ) {
					_i_SCH_IO_SET_MODULE_STATUS( CM1 + i , j+1 , 2 );
				}
				//
				for ( ; j < _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ; j++ ) {
					_i_SCH_IO_SET_MODULE_STATUS( CM1 + i , j+1 , 1 );
				}
				//
				SCHMULTI_CASSETTE_VERIFY_CLEAR( i , TRUE );
				if ( strlen( data ) <= 0 ) {
					sprintf( Buffer , "MID%d" , i + 1 );
				}
				else {
					sprintf( Buffer , "%sMID%d" , data , i + 1 );
				}
				//
				IO_SET_MID_NAME( i , Buffer );
				//
				SCHMULTI_CASSETTE_MID_SET( i , Buffer );
				BUTTON_SET_FLOW_STATUS_VAR( i , _MS_5_MAPPED );
				SCHMULTI_CASSETTE_VERIFY_MAKE_OK( i );
				break;
			default :

				printf( "===============================================\n" );
				printf( "===============================================\n" );
				printf( "MAPPING SET IMPOSSIBLE (CM%d) INVALID STATUS(%d)\n" , i + 1 , BUTTON_GET_FLOW_STATUS_VAR( i ) );
				printf( "===============================================\n" );
				printf( "===============================================\n" );

				break;
			}
		}
	}

	// User Test
	if ( mode == 4 ) {

		// A:1 -> A:25
		// B:1 -> A:24		B
		// C:1 -> A:23		C
		// D:1 -> A:22		D		A

		// Merge
// Case1

//		CP_Process_Job_Insert( "MPJ1" , "MMID1" , "TEST:(Test)" , 1 , 1 );
//		CP_Process_Job_Insert( "MPJ2" , "MMID2" , "TEST:(Test)" , 1 , 1 );
//		CP_Process_Job_Insert( "MPJ3" , "MMID3" , "TEST:(Test)" , 1 , 1 );
//		CP_Process_Job_Insert( "MPJ4" , "MMID4" , "TEST:(Test)" , 1 , 1 );
//
//		CP_Control_Job_Insert( "MCJ1" ,
//							   "MPJ1:MPJ2:MPJ3:MPJ4" ,
//							   "MMID1:1:MMID1:25" ,
//							   "MMID2:1:MMID1:24" ,
//							   "MMID3:1:MMID1:23" ,
//							   "MMID4:1:MMID1:22" ,
//							   "" ,
//							   "" ,
//							   "" ,
//							   ""
//							   );

// Case 2
//	CP_Job_Insert( "PJINSERT|BPJ2|BMID2|TEST:(Test)|1|4" );
//	CP_Job_Insert( "PJINSERT|BPJ4|BMID4|TEST:(Test)|1|3" );
//	CP_Job_Insert( "CJINSERT|BCJ|BPJ2:BPJ4|BMID2:1:BMID3:25|BMID2:2:BMID3:24|BMID2:3:BMID3:23|BMID4:1:BMID1:25|BMID4:2:BMID1:24|BMID4:3:BMID1:23" );

//	CP_Job_Insert( "PJINSERT|BPJ1|BMID1|TEST:(Test)|1|3" );
//	CP_Job_Insert( "PJINSERT|BPJ4|BMID4|TEST:(Test)|1|3" );
//	CP_Job_Insert( "CJINSERT|BCJ|BPJ1:BPJ4|BMID1:1:BMID4:25|BMID1:2:BMID4:24|BMID1:3:BMID4:23|BMID4:1:BMID1:25|BMID4:2:BMID1:24|BMID4:3:BMID1:23" );

	CP_Job_Insert( "PJINSERT|APJ2|AMID2|TEST:(Test)|1|3" );
	CP_Job_Insert( "CJINSERT|ACJ2|APJ2|AMID2:1:AMID3:25|AMID2:2:AMID3:24|AMID2:3:AMID3:23" );

	CP_Job_Insert( "PJINSERT|APJ4|AMID4|TEST:(Test)|1|3" );
	CP_Job_Insert( "CJINSERT|ACJ4|APJ4|AMID4:1:AMID1:25|AMID4:2:AMID1:24|AMID4:3:AMID1:23" );

//	CP_Job_Insert( "PJINSERT|BPJ4|BMID4|TEST:(Test)|1|3" );
//	CP_Job_Insert( "CJINSERT|BCJ|BPJ1:BPJ4|BMID1:1:BMID4:25|BMID1:2:BMID4:24|BMID1:3:BMID4:23|BMID4:1:BMID1:25|BMID4:2:BMID1:24|BMID4:3:BMID1:23" );


// Case 3
//	CP_Job_Insert( "PJINSERT|BPJ4|BMID4|TEST:(Test)|1|4" );
//	CP_Job_Insert( "CJINSERT|BCJ|BPJ4|BMID4:1:BMID1:21|BMID4:2:BMID2:22|BMID4:3:BMID3:21|BMID4:4:BMID4:24" );

//	CP_Job_Insert( "PJINSERT|CPJ2|CMID2|TEST:(Test)|1|2" );
//	CP_Job_Insert( "CJINSERT|CCJ|CPJ2|CMID2:1:CMID3:21|CMID2:2:CMID3:22" );

//	CP_Job_Insert( "PJINSERT|DPJ1|DMID1|TEST:(Test)|1|1" );
//	CP_Job_Insert( "PJINSERT|DPJ4|DMID4|TEST:(Test)|1|1" );
//	CP_Job_Insert( "PJINSERT|DPJ2|DMID2|TEST:(Test)|1|1" );
//	CP_Job_Insert( "PJINSERT|DPJ3|DMID3|TEST:(Test)|1|1" );
//	CP_Job_Insert( "CJINSERT|DCJ|DPJ1:DPJ4:DPJ2:DPJ3|DMID1:1:DMID1:21|DMID4:1:DMID1:22|DMID2:1:DMID1:23|DMID3:1:DMID2:23" );
//
//	CP_Job_Insert( "PJINSERT|EPJ4|EMID4|TEST:(Test)|1|2" );
//	CP_Job_Insert( "PJINSERT|EPJ2|EMID2|TEST:(Test)|1|2" );
//	CP_Job_Insert( "CJINSERT|ECJ|EPJ4:EPJ2|EMID4:1:EMID2:21|EMID4:2:EMID2:22|EMID2:1:EMID4:21|EMID2:2:EMID4:22" );
//
//	CP_Job_Insert( "PJINSERT|FPJ1|FMID1|TEST:(Test)|1|1" );
//	CP_Job_Insert( "PJINSERT|FPJ3|FMID3|TEST:(Test)|1|1" );
//	CP_Job_Insert( "PJINSERT|FPJ2|FMID2|TEST:(Test)|1|1" );
//	CP_Job_Insert( "CJINSERT|FCJ|FPJ1:FPJ3:FPJ2|FMID1:1:FMID4:21|FMID3:1:FMID4:22|FMID2:1:FMID4:23" );

//case 4

//		CP_Process_Job_Insert( "UPJ2" , "UMID2" , "TEST:(Test)" , 1 , 3 );

//		CP_Control_Job_Insert( "UCJ2" ,
//							   "UPJ2" ,
//							   "UMID2:1:UMID3:25" ,
//							   "UMID2:2:UMID3:24" ,
//							   "UMID2:3:UMID3:23" ,
//							   "" ,
//							   "" ,
//							   "" ,
//							   "" ,
//							   ""
//							   );

	}

	// Merge Test
	if ( mode == 1 ) {

		// A:1 -> A:25
		// B:1 -> A:24		B
		// C:1 -> A:23		C
		// D:1 -> A:22		D		A

		CP_Process_Job_Insert( "MPJ1" , "MMID1" , "TEST:(Test)" , 1 , 1 );
		CP_Process_Job_Insert( "MPJ2" , "MMID2" , "TEST:(Test)" , 1 , 1 );
		CP_Process_Job_Insert( "MPJ3" , "MMID3" , "TEST:(Test)" , 1 , 1 );
		CP_Process_Job_Insert( "MPJ4" , "MMID4" , "TEST:(Test)" , 1 , 1 );

		CP_Control_Job_Insert( "MCJ1" ,
							   "MPJ1:MPJ2:MPJ3:MPJ4" ,
							   "MMID1:1:MMID1:25" ,
							   "MMID2:1:MMID1:24" ,
							   "MMID3:1:MMID1:23" ,
							   "MMID4:1:MMID1:22" ,
							   "" ,
							   "" ,
							   "" ,
							   ""
							   );
	}


	// Split Test
	if ( mode == 2 ) {

		// A:1 -> A:25
		// A:2 -> B:25				B
		// A:3 -> C:25				C
		// A:4 -> D:25
		// A:5 -> D:24				D

		// A:6 -> A:6
		// A:7 -> A:7				A

		CP_Process_Job_Insert( "SPJ1" , "SMID4" , "TEST:(Test)" , 1 , 4 );

		CP_Control_Job_Insert( "SCJ1" ,
							   "SPJ1" ,
							   "SMID4:1:SMID4:25" ,
							   "SMID4:2:SMID3:25" ,
							   "SMID4:3:SMID2:25" ,
							   "SMID4:4:SMID1:25" ,
//							   "SMID1:5:SMID4:24" ,
							   "" ,
							   "" ,
							   "" ,
							   ""
							   );
	}

	// Merge & Split Test
	if ( mode == 3 ) {

		// A:1 -> A:25
//		// A:2 -> B:25				{B}
		// A:2 -> A:2
		// A:3 -> C:25
		//
		// B:1 -> A:24
		// B:2 -> D:25
		// B:3 -> D:24		B
		//
		// C:1 -> A:23
		// C:2 -> C:2
		// C:3 -> C:3				C
		//
		// D:1 -> A:22				A
		// D:2 -> D:2
		// D:3 -> D:3				D



		CP_Process_Job_Insert( "APJ1" , "AMID1" , "TEST:(Test)" , 1 , 3 );
		CP_Process_Job_Insert( "APJ2" , "AMID2" , "TEST:(Test)" , 1 , 3 );
		CP_Process_Job_Insert( "APJ3" , "AMID3" , "TEST:(Test)" , 1 , 3 );
		CP_Process_Job_Insert( "APJ4" , "AMID4" , "TEST:(Test)" , 1 , 3 );

		CP_Control_Job_Insert( "ACJ1" ,
							   "APJ1:APJ2:APJ3:APJ4" ,
							   "AMID1:1:AMID1:25" ,
//							   "AMID1:2:AMID2:25" ,
							   "AMID1:0:AMID2:25" ,
							   "AMID1:3:AMID3:25" ,
							   "AMID2:1:AMID1:24" ,
							   "AMID2:2:AMID4:25" ,
							   "AMID2:3:AMID4:24" ,
							   "AMID3:1:AMID1:23" ,
							   "AMID4:1:AMID1:22"
							   );

	}

}



*/