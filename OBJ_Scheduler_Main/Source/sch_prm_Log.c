#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "Equip_Handling.h"
#include "system_tag.h"
#include "User_Parameter.h"
#include "Robot_Handling.h"
#include "IO_Part_data.h"
#include "IOlog.h"
#include "MultiJob.h"
#include "sch_prePrcs.h"
#include "sch_prm.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_CassetteResult.h"
#include "sch_Cassettesupply.h"
#include "sch_prepost.h"
#include "sch_OneSelect.h"
#include "sch_Multi_ALIC.h"
#include "sch_sdm.h"
#include "sch_processmonitor.h"
#include "Timer_Handling.h"
#include "version.h"

#include "INF_sch_CommonUser.h"

//=======================================================================================
//=======================================================================================

extern int EXTEND_SOURCE_USE;

extern int SCHEDULER_CLUSTER_DefaultUse_Data[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2017.09.08

extern BOOL SDM_EVENT_INCREASE; // 2016.11.10


extern int  EQUIP_TM_RUNNING_FLAG[MAX_TM_CHAMBER_DEPTH]; // 2017.03.20

extern int  EQUIP_TM_ROBOT_FLAG[MAX_TM_CHAMBER_DEPTH]; // 2017.03.20
//=======================================================================================
//=======================================================================================
int _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( int ch , int * );
//
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_FOR_LOG( int side ); // 2014.10.14

int  _i_SCH_SYSTEM_MODE_END_BUF1_GET( int CHECKING_SIDE ); // 2016.08.12
int  _i_SCH_SYSTEM_MODE_END_BUF2_GET( int CHECKING_SIDE ); // 2016.08.12


int LOG_OPTION[MAX_CASSETTE_SIDE][256]; // 2015.06.25
//=======================================================================================
//=======================================================================================
extern int USER_RECIPE_START_TAG[USER_RECIPE_MAX_TAG][MAX_CASSETTE_SIDE];
extern int USER_RECIPE_START_END[USER_RECIPE_MAX_TAG][MAX_CASSETTE_SIDE];

extern int PM_INTERFACE_MONITOR_SIDE[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
extern int PM_INTERFACE_MONITOR_POINTER[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
extern int PM_INTERFACE_MONITOR_WAFER[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
extern int PM_INTERFACE_MONITOR_MODE[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
extern int PM_INTERFACE_MONITOR_COUNT[ MAX_CHAMBER ];

extern int _insdie_run_Tag[MAX_CASSETTE_SIDE]; // 2010.03.26

extern int _Trace_FEM_Flow[MAX_CASSETTE_SIDE];
extern int _Trace_TM_Flow[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH];

//
extern int _Trace_FM_Flow_In[MAX_CASSETTE_SIDE]; // 2014.03.12
extern int _Trace_TM_Flow_In[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH]; // 2014.03.12
extern int _Trace_TM_Flow_Kill[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH]; // 2017.02.08

extern char _In_RecipeFileName[MAX_CASSETTE_SIDE][256];
extern int  _In_SL_1[MAX_CASSETTE_SIDE];
extern int  _In_SL_2[MAX_CASSETTE_SIDE];
extern int  _In_Loop[MAX_CASSETTE_SIDE];
extern int  _In_Run_Finished_Check[MAX_CASSETTE_SIDE];

extern int  _In_Job_HandOff_In_Pre_Check[MAX_CASSETTE_SIDE];

//------------------------------------------------------------------------------------------
extern int  _in_HOT_LOT_CHECKING[MAX_CASSETTE_SIDE]; // 2011.06.13
extern int  _in_HOT_LOT_ORDERING_INVALID[MAX_CASSETTE_SIDE]; // 2011.06.13
extern int  _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[MAX_CASSETTE_SIDE]; // 2014.10.14
//------------------------------------------------------------------------------------------
extern int  _in_UNLOAD_USER_TYPE[MAX_CASSETTE_SIDE]; // 2010.04.26

extern int  _in_NOCHECK_RUNNING_TYPE[MAX_CASSETTE_SIDE]; // 2017.10.10

extern int  _in_LOTPRE_CLUSTER_DIFFER_MODE2[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2017.09.22

extern int SCH_MACRO_PROCESS_STS[MAX_CHAMBER];

extern int MSG_READYEND[MAX_CHAMBER]; // 2010.09.21

extern BOOL			Global_Chamber_Pre_NotUse_Flag[MAX_CHAMBER]; // 2002.03.25
//
extern int 			Global_Chamber_FirstPre_Use_Flag[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2003.11.11
//
extern BOOL			Global_Chamber_FirstPre_Done_Flag[ MAX_CASSETTE_SIDE ][MAX_CHAMBER]; // 2005.10.27
//
extern BOOL			Global_Chamber_FirstPre_NotUse_Flag[ MAX_CASSETTE_SIDE ][MAX_CASSETTE_SIDE]; // 2005.10.27
extern BOOL			Global_Chamber_FirstPre_NotUse2_Flag[ MAX_CASSETTE_SIDE ][MAX_CASSETTE_SIDE]; // 2007.09.07

extern int _Trace_Sleeping_Count[MAX_CASSETTE_SIDE]; // 2012.02.07
extern int _Trace_TM_Count[MAX_CASSETTE_SIDE]; // 2012.02.07

extern int SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[MAX_CHAMBER]; // 2012.09.26
//extern int SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM; // 2012.09.26

extern int SCH_MACRO_PROCESS_MOVE[MAX_CHAMBER]; // 2012.11.09


// 2015.08.26
extern int  FA_LOAD_CONTROLLER_RUN[ MAX_CASSETTE_SIDE ];

extern int  FA_MAPPING_SIGNAL[ MAX_CASSETTE_SIDE ];
extern int  FA_MAPPING_CANCEL[ MAX_CASSETTE_SIDE ];
extern BOOL FA_LOADING_SIGNAL[ MAX_CASSETTE_SIDE ];
extern BOOL FA_LOADREQ_SIGNAL[ MAX_CASSETTE_SIDE ];

extern int FA_UNLOAD_CONTROLLER_RUN[ MAX_CASSETTE_SIDE ];

extern BOOL FA_UNLOADING_SIGNAL[ MAX_CASSETTE_SIDE ];

//=======================================================================================
// 2016.03.18
extern int				_TM_PICKPLACING_CHAMBER[ MAX_CHAMBER ];

extern int				_TM_WILLPLACING_CHAMBER[MAX_TM_CHAMBER_DEPTH][MAX_FINGER_TM][ MAX_CHAMBER ];
extern int				_TM_WILLPLACING_MAX_TM;
extern int				_TM_WILLPLACING_MAX_ARM;
//=======================================================================================
void SCHEDULING_DATA_STRUCTURE_INIT() {
//	FILE *fpt; // 2006.02.28
	if ( SYSTEM_LOGSKIP_STATUS() ) return; // 2004.05.21
//	fpt = fopen( SCHEDULER_DEBUG_DATA_FILE , "w" );
//	if ( fpt != NULL ) fclose(fpt);
	DeleteFile( SCHEDULER_DEBUG_DATA_FILE );
}
//
void SCHEDULING_DATA_STRUCTURE_LOG( char *filename , char *dispdata , int lotlog , int logpos , int sd ) {
	int i , j , k , l , m , pmc , encryp_backup;
	char c;
	FILE *fpt;
//	struct tm Pres_Time; // 2006.10.20
	SYSTEMTIME		SysTime; // 2006.10.02
	char Revinfo[256];
	int lg; // 2014.10.28
	int Log_Opt[256]; // 2015.06.25

	if ( SYSTEM_LOGSKIP_STATUS() ) return; // 2004.05.21

	if ( filename[0] == 0 ) {
		fpt = fopen( SCHEDULER_DEBUG_DATA_FILE , "a" );
	}
	else {
		fpt = fopen( filename , "a" );
	}

	if ( fpt == NULL ) return;
	//
//	strcpy( Revinfo , "" ); // 2017.01.05
	_SCH_COMMON_REVISION_HISTORY( 2 , Revinfo , 255 );
	//
	if ( lotlog == -1 ) {
		//
		fprintf( fpt , "==================================================================\n" );
		GetLocalTime( &SysTime ); // 2006.10.02
		//
#ifdef _SCH_MIF // 2015.06.12
		if ( strlen( Revinfo ) <= 0 ) {
			fprintf( fpt , "%d:%d:%d\t%s [%s(%s)](%s %s) [%d][M]\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , dispdata , REGIST_INFO_STRING , SCH_LASTUPDATE_DATE , __DATE__ , __TIME__ , sd ); // 2006.10.02
		}
		else {
			fprintf( fpt , "%d:%d:%d\t%s [%s(%s)][%s](%s %s) [%d][M]\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , dispdata , REGIST_INFO_STRING , SCH_LASTUPDATE_DATE , Revinfo , __DATE__ , __TIME__ , sd ); // 2006.10.02
		}
#else
		if ( strlen( Revinfo ) <= 0 ) {
			fprintf( fpt , "%d:%d:%d\t%s [%s(%s)](%s %s) [%d]\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , dispdata , REGIST_INFO_STRING , SCH_LASTUPDATE_DATE , __DATE__ , __TIME__ , sd ); // 2006.10.02
		}
		else {
			fprintf( fpt , "%d:%d:%d\t%s [%s(%s)][%s](%s %s) [%d]\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , dispdata , REGIST_INFO_STRING , SCH_LASTUPDATE_DATE , Revinfo , __DATE__ , __TIME__ , sd ); // 2006.10.02
		}
#endif
		//
		fprintf( fpt , "==================================================================\n" );
		//
		fprintf( fpt , "<SYSTEM DATA AREA>\n" );
		fprintf( fpt , " [SYSTEM_ALG_STYLE] %d , %d\n" , SYSTEM_RUN_ALG_STYLE_GET() , SYSTEM_SUB_ALG_STYLE_GET() );
		//
		fprintf( fpt , " [SYSTEM_DEFINE] C-SIDE:%d,C-SLOT:%d CL-DEPTH:%d PM:%d,BM:%d,TM:%d PM-SLOT:%d MAX_CH:%d\n" , MAX_CASSETTE_SIDE , MAX_CASSETTE_SLOT_SIZE , MAX_CLUSTER_DEPTH , MAX_PM_CHAMBER_DEPTH , MAX_BM_CHAMBER_DEPTH , MAX_TM_CHAMBER_DEPTH , MAX_PM_SLOT_DEPTH , MAX_CHAMBER ); // 2015.01.29
		//
		for ( i = 0 ; i < 4 ; i++ ) {
			if      ( i == 0 ) fprintf( fpt , " [MODULE_MODE:%d:%d:%d]\n" , _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() , _i_SCH_PRM_GET_MODULE_COMMSTATUS_MODE() , _i_SCH_PRM_GET_MODULE_COMMSTATUS2_MODE() );
			else if ( i == 1 ) fprintf( fpt , " [MODULE_DATA]\n" );
			else if ( i == 2 ) fprintf( fpt , " [MODULE_SERVICE]\n" );
			else if ( i == 3 ) fprintf( fpt , " [MODULE_SIZE]\n" );
			//
			fprintf( fpt , "\t\t" );
			for ( j = CM1 ; j < MDL_LAST ; j++ ) {
				if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) fprintf( fpt , ":" );
				if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
					//
					fprintf( fpt , "(%s=" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) );
					//
					if      ( i == 0 ) {
						/*
						// 2013.09.03
						switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ) {
						case 0 : fprintf( fpt , "DI)" ); break;
						case 1 : fprintf( fpt , "EN)" ); break;
						case 2 : fprintf( fpt , "DH)" ); break;
						case 3 : fprintf( fpt , "EP)" ); break;
						default : fprintf( fpt , "?%d)" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ); break;
						}
						*/
						//
						if ( _i_SCH_PRM_GET_MODULE_STOCK( j ) ) { // 2015.06.18
							switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ) {
							case 0 : fprintf( fpt , "DI:%d:ST%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) , _i_SCH_PRM_GET_MODULE_STOCK( j ) ); break;
							case 1 : fprintf( fpt , "EN:%d:ST%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) , _i_SCH_PRM_GET_MODULE_STOCK( j ) ); break;
							case 2 : fprintf( fpt , "DH:%d:ST%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) , _i_SCH_PRM_GET_MODULE_STOCK( j ) ); break;
							case 3 : fprintf( fpt , "EP:%d:ST%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) , _i_SCH_PRM_GET_MODULE_STOCK( j ) ); break;
							default : fprintf( fpt , "?%d:%d:ST%d)" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) , _i_SCH_MODULE_GET_MOVE_MODE(j) , _i_SCH_PRM_GET_MODULE_STOCK( j ) ); break;
							}
						}
						else {
							if ( _i_SCH_MODULE_GET_MOVE_MODE(j) != 0 ) {
								switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ) {
								case 0 : fprintf( fpt , "DI:%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) ); break;
								case 1 : fprintf( fpt , "EN:%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) ); break;
								case 2 : fprintf( fpt , "DH:%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) ); break;
								case 3 : fprintf( fpt , "EP:%d)" , _i_SCH_MODULE_GET_MOVE_MODE(j) ); break;
								default : fprintf( fpt , "?%d:%d)" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) , _i_SCH_MODULE_GET_MOVE_MODE(j) ); break;
								}
							}
							else {
								switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ) {
								case 0 : fprintf( fpt , "DI)" ); break;
								case 1 : fprintf( fpt , "EN)" ); break;
								case 2 : fprintf( fpt , "DH)" ); break;
								case 3 : fprintf( fpt , "EP)" ); break;
								default : fprintf( fpt , "?%d)" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ); break;
								}
							}
						}
					}
					else if ( i == 1 ) {
						fprintf( fpt , "%2d)" , j );
					}
					else if ( i == 2 ) {
						switch( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( j ) ) {
						case 0 : fprintf( fpt , "LO)" ); break;
						case 1 : fprintf( fpt , "RE)" ); break;
						default : fprintf( fpt , "?%d)" , _i_SCH_PRM_GET_MODULE_SERVICE_MODE( j ) ); break;
						}
					}
					else if ( i == 3 ) {
						if      ( _i_SCH_PRM_GET_MODULE_SIZE_CHANGE( j ) == 0 ) {
							fprintf( fpt , "%2d)" , _i_SCH_PRM_GET_MODULE_SIZE( j ) );
						}
						else if ( _i_SCH_PRM_GET_MODULE_SIZE_CHANGE( j ) == 1 ) {
							fprintf( fpt , "%2d:C)" , _i_SCH_PRM_GET_MODULE_SIZE( j ) );
						}
						else {
							fprintf( fpt , "%2d:%d)" , _i_SCH_PRM_GET_MODULE_SIZE( j ) , _i_SCH_PRM_GET_MODULE_SIZE_CHANGE( j ) );
						}
					}
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		//
		for ( j = CM1 ; j < MDL_LAST ; j++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
				if ( _i_SCH_PRM_GET_MODULE_GROUP( j ) != 0 ) break;
			}
		}
		if ( j != MDL_LAST ) {
			//
			fprintf( fpt , " [MODULE_GROUP]\n" );
			//
			fprintf( fpt , "\t\t" );
			//
			for ( j = CM1 ; j < MDL_LAST ; j++ ) {
				//
				if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) fprintf( fpt , ":" );
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
					//
					fprintf( fpt , "(%s=%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_MODULE_GROUP( j ) );
					//
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		//
		for ( j = CM1 ; j < MDL_LAST ; j++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( j ) != 0 ) break;
			}
		}
		if ( j != MDL_LAST ) {
			//
			fprintf( fpt , " [MODULE_DOUBLE_WHAT_SIDE]\n" );
			//
			fprintf( fpt , "\t\t" );
			//
			for ( j = CM1 ; j < MDL_LAST ; j++ ) {
				//
				if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) fprintf( fpt , ":" );
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
					//
					fprintf( fpt , "(%s=%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( j ) );
					//
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		//
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			if ( _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( j ) != 0 ) break;
		}
		if ( j != MAX_TM_CHAMBER_DEPTH ) {
			//
			fprintf( fpt , " [MODULE_GROUP_HAS_SWITCH]\n" );
			fprintf( fpt , "		" );
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				if ( _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( j ) == 0 ) continue;
				fprintf( fpt , "(%s=%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j+TM) , _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( j ) );
			}
			fprintf( fpt , "\n" );
			//
		}
		//
		fprintf( fpt , " [MODULE_BUFFER_???]\n" );
		fprintf( fpt , "		SWITCH_SWAPPING(BA,BP,SW,FW,MW)=" );
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) fprintf( fpt , "%d," , _i_SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( j ) );
		fprintf( fpt , "DUMMY_BUFFER=%d,ALIGN_BUFFER=%d\n" , _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() );
		//
		fprintf( fpt , " [SYSTEM_OPTION]\n" );
		fprintf( fpt , "		" );
		fprintf( fpt , "CHECK_WAFER_SYNCH=%d" , _i_SCH_PRM_GET_UTIL_CHECK_WAFER_SYNCH() );
		fprintf( fpt , "\tMAPPING_WHEN_TMFREE=%d" , _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() );
		fprintf( fpt , "\tTM_GATE_SKIP_CONTROL=%d" , _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() );
		fprintf( fpt , "\tTM_MOVE_SR_CONTROL=%d" , _i_SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() );
		fprintf( fpt , "\tFIRST_MODULE_PUT_DELAY_TIME=%d" , _i_SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() );
		fprintf( fpt , "\n		" );
		fprintf( fpt , "SCH_START_OPTION=%d" , _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() );
		fprintf( fpt , "\tFM_MOVE_SR_CONTROL=%d" , _i_SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() );
		fprintf( fpt , "\tLOOP_MAP_ALWAYS=%d" , _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS() );
		fprintf( fpt , "\tSCH_LOG_DIRECTORY_FORMAT=%d" , _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() );
		fprintf( fpt , "\tSCH_CHANGE_PROCESS_WAFER_TO_RUN=%d" , _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN() );
		fprintf( fpt , "\n		" );
		fprintf( fpt , "ABORT_MESSAGE_USE_POINT=%d" , _i_SCH_PRM_GET_UTIL_ABORT_MESSAGE_USE_POINT() );
		fprintf( fpt , "\tCANCEL_MESSAGE_USE_POINT=%d" , _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT() );
		fprintf( fpt , "\tEND_MESSAGE_SEND_POINT=%d" , _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() );
		fprintf( fpt , "\tCASSETTE_SUPPLY_DEVIATION_TIME=%d" , _i_SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() );
		fprintf( fpt , "\tCHAMBER_PUT_PR_CHECK=%d" , _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() );
		fprintf( fpt , "\n		" );
		fprintf( fpt , "CHAMBER_GET_PR_CHECK=%d" , _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() );
		fprintf( fpt , "\tFIC_MULTI_WAITTIME=%d(%d)" , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , _i_SCH_MODULE_Get_MFIC_LAST_PLACED_SLOT() );
		fprintf( fpt , "\tFAL_MULTI_WAITTIME=%d" , _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() );
		fprintf( fpt , "\tMESSAGE_USE_WHEN_SWITCH=%d" , _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() );
		fprintf( fpt , "\tCM_SUPPLY_MODE=%d" , _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() );
		fprintf( fpt , "\n		" );
		fprintf( fpt , "PREPOST_PROCESS_DEPAND=%d" , _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() );
		fprintf( fpt , "\tSTART_PARALLEL_CHECK_SKIP=%d" , _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() );
		fprintf( fpt , "\tWAFER_SUPPLY_MODE=%d" , _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() );
		fprintf( fpt , "\tCLUSTER_INCLUDE_FOR_DISABLE=%d" , _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() );
		fprintf( fpt , "\tBMEND_SKIP_WHEN_RUNNING=%d" , _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() );
		fprintf( fpt , "\n		" );
		fprintf( fpt , "PMREADY_SKIP_WHEN_RUNNING=%d" , _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() );
		fprintf( fpt , "\tLOT_LOG_PRE_POST_PROCESS=%d" , _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() );
		fprintf( fpt , "\tMESSAGE_USE_WHEN_ORDER=%d" , _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() );
		fprintf( fpt , "\tSW_BM_SCHEDULING_FACTOR=%d" , _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() );
		fprintf( fpt , "\tMULTI_GROUP_AUTO_ARRANGE=%d" , _i_SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() ); // 2006.03.22
		fprintf( fpt , "\n		" );
		fprintf( fpt , "LOT_END_ESTIMATION=%d" , _i_SCH_PRM_GET_LOT_END_ESTIMATION() ); // 2006.08.11
		fprintf( fpt , "\tCLSOUT_BM_SINGLE_ONLY=%d" , _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() ); // 2007.05.11
		fprintf( fpt , "\tADAPTIVE_PROGRESSING=%d" , _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() ); // 2008.04.28
		fprintf( fpt , "\tDFIX_IOINTLKS_USE=%d" , _i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ); // 2008.04.28
		fprintf( fpt , "\tDFIX_CHAMBER_INTLKS_IOUSE=%d" , _i_SCH_PRM_GET_DFIX_CHAMBER_INTLKS_IOUSE() ); // 2013.11.21
		fprintf( fpt , "\n		" );
		fprintf( fpt , "PICKPLACE_CANCEL_MESSAGE=%d" , _i_SCH_PRM_GET_PICKPLACE_CANCEL_MESSAGE() ); // 2008.04.28
		fprintf( fpt , "\tTIMER_RESET_WHEN_NEW_START=%d" , _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START() ); // 2008.04.28
//		fprintf( fpt , "\tCHECK_DIFFERENT_METHOD=%d" , _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() ); // 2008.07.09
		fprintf( fpt , "\tCHECK_DIFFERENT_METHOD=%d" , _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() + ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD2() * 100 ) ); // 2018.08.22
		fprintf( fpt , "\tBATCH_SUPPLY_INTERRUPT=%d" , _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() ); // 2008.04.28
		fprintf( fpt , "\tDIFF_LOT_NOTSUP_MODE=%d" , _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() ); // 2008.04.28
		fprintf( fpt , "\tMETHOD1_TO_1BM_USING_MODE=%d" , _i_SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() ); // 2018.03.21
		fprintf( fpt , "\n		" );
		fprintf( fpt , "FA_LOADUNLOAD_SKIP=%d" , _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ); // 2008.04.28
		fprintf( fpt , "\tFA_MAPPING_SKIP=%d" , _i_SCH_PRM_GET_FA_MAPPING_SKIP() ); // 2008.04.28
		fprintf( fpt , "\tMAINT_MESSAGE_STYLE=%d" , _i_SCH_PRM_GET_MAINT_MESSAGE_STYLE() ); // 2015.06.17
		fprintf( fpt , "\tDFIX_CASSETTE_READ_MODE=%d" , _i_SCH_PRM_GET_DFIX_CASSETTE_READ_MODE() ); // 2008.04.28
		fprintf( fpt , "\tDFIX_MAX_CASSETTE_SLOT=%d" , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ); // 2015.06.18
		fprintf( fpt , "\n		" );
		fprintf( fpt , "DFIX_TM_DOUBLE_CONTROL=%d" , _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() ); // 2008.04.28
		fprintf( fpt , "\tDFIX_FM_DOUBLE_CONTROL=%d" , _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() ); // 2008.04.28
		fprintf( fpt , "\tDFIX_FIC_MULTI_CONTROL=%d" , _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() ); // 2008.04.28
		fprintf( fpt , "\tDFIX_FAL_MULTI_CONTROL=%d" , _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() ); // 2008.04.28
		fprintf( fpt , "\tINTERLOCK_FM_BM_PLACE_SEPARATE=%d" , _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() ); // 2008.04.28
		fprintf( fpt , "\n		" );
		fprintf( fpt , "MANUAL_CJPJJOBMODE=%d" , SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() ); // 2008.05.09
		fprintf( fpt , "\tMTLOUT_INTERFACE=%d" , _i_SCH_PRM_GET_MTLOUT_INTERFACE() ); // 2011.05.26
		fprintf( fpt , "\tREADY_MULTIJOB_MODE=%d" , _i_SCH_PRM_GET_READY_MULTIJOB_MODE() ); // 2014.02.03
		fprintf( fpt , "\tMULTIJOB_LOTRECIPE_READ=%d" , _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ); // 2015.06.17
		fprintf( fpt , "\tEIL_INTERFACE=%d" , _i_SCH_PRM_GET_EIL_INTERFACE() ); // 2015.06.17
		fprintf( fpt , "\n		" );
		fprintf( fpt , "DUMMY_MESSAGE=%d" , _i_SCH_PRM_GET_DUMMY_MESSAGE() ); // 2015.06.17
		fprintf( fpt , "\tDUMMY_PROCESS_CHAMBER=%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ); // 2015.06.17
		fprintf( fpt , "\tDUMMY_PROCESS_SLOT=%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() ); // 2015.06.17
		fprintf( fpt , "\tDFIX_RECIPE_LOCKING=%d" , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() ); // 2015.06.17
		fprintf( fpt , "\tPRCS_ZONE_RUN_TYPE=%d" , _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() ); // 2015.06.17
		fprintf( fpt , "\n		" );
		fprintf( fpt , "INTERLOCK_TM_BM_OTHERGROUP_SWAP=%d" , _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() ); // 2015.06.17
		fprintf( fpt , "\n		" );

		fprintf( fpt , "\n" );

		/*
		// 2014.07.18
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) != -1 ) || ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) != -1 ) ) break;
			}
			if ( i != MAX_CHAMBER ) {
				fprintf( fpt , " GROUP_PICKPLACE_DATA[%d] " , j );
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
					if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
					fprintf( fpt , "[%02d,%02d]" , _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) );
				}
				fprintf( fpt , "\n" );
			}
		}
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) != -1 ) || ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) != -1 ) ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP_PICKPLACE_DATAFM  " );
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
				fprintf( fpt , "[%02d,%02d]" , _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) );
			}
			fprintf( fpt , "\n" );
		}
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) != 0 ) || ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) != 0 ) ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP_PICKPLACE_DATAALL " );
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
				fprintf( fpt , "[%02d,%02d]" , _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) );
			}
			fprintf( fpt , "\n\n" );
		}
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( ( _i_SCH_PRM_GET_MODULE_TMS_GROUP( i ) != -1 ) || ( _i_SCH_PRM_GET_MODULE_TMS_GROUP( i ) != -1 ) ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP_PICKPLACE_DATATMS " );
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
				fprintf( fpt , "[%02d]" , _i_SCH_PRM_GET_MODULE_TMS_GROUP( i ) );
			}
			fprintf( fpt , "\n" );
		}
		//
		// 2014.06.11
		//
		for ( i = 1 ; i <= 255 ; i++ ) {
			if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i ) != 0 ) {
				fprintf( fpt , "MFI_INTERFACE_FLOW_USER_OPTION(%d)	%d\n" , i , _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i ) );
			}
		}
		*/

		//
		// 2014.07.18
		//-----------------------------------------------------------------------------------------------------------------------
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) != -1 ) break;
			}
			if ( i != MAX_CHAMBER ) {
				//
				fprintf( fpt , " GROUP[%2d] PICK  " , j );
				//
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
					switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) ) {
					case -1 :
						break;
					case 0 :
						fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
						break;
					case 1 :
						fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
						break;
					default :
						fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) );
						break;
					}
				}
				fprintf( fpt , "\n" );
			}
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) != -1 ) break;
			}
			if ( i != MAX_CHAMBER ) {
				//
				fprintf( fpt , " GROUP[%2d] PLACE " , j );
				//
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
					switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) ) {
					case -1 :
						break;
					case 0 :
						fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
						break;
					case 1 :
						fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
						break;
					default :
						fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) );
						break;
					}
				}
				fprintf( fpt , "\n" );
			}
			//
		}
		//-----------------------------------------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) != -1 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP[FM] PICK  " );
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) ) {
				case -1 :
					break;
				case 0 :
					fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				case 1 :
					fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) != -1 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP[FM] PLACE " );
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) ) {
				case -1 :
					break;
				case 0 :
					fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				case 1 :
					fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//-----------------------------------------------------------------------------------------------------------------------
		for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) > 0 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP-CNT PICK  " );
			//
			for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) {
				case -1 :
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) > 0 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP-CNT PLACE " );
			//
			for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) {
				case -1 :
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//-----------------------------------------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_TMS_GROUP( i ) != -1 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			fprintf( fpt , " GROUP TMS_GROUP " );
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_TMS_GROUP( i ) ) {
				case -1 :
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_TMS_GROUP( i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//-----------------------------------------------------------------------------------------------------------------------
		// 2015.06.18
		//
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			//
			for ( i = CM1 ; i < TM ; i++ ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i ) > 0 ) break;
			}
			if ( i != TM ) {
				//
				fprintf( fpt , " ARMINT[%2d] PICK " , j );
				//
				for ( i = CM1 ; i < TM ; i++ ) {
					if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i ) > 0 ) {
						fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( j , i ) );
					}
				}
				fprintf( fpt , "\n" );
			}
			//
			for ( i = CM1 ; i < TM ; i++ ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i ) > 0 ) break;
			}
			if ( i != TM ) {
				//
				fprintf( fpt , " ARMINT[%2d] PLCE " , j );
				//
				for ( i = CM1 ; i < TM ; i++ ) {
					if ( _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i ) > 0 ) {
						fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( j , i ) );
					}
				}
				fprintf( fpt , "\n" );
			}
			//
		}
		//
		for ( i = CM1 ; i < TM ; i++ ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i ) > 0 ) break;
		}
		if ( i != TM ) {
			//
//			fprintf(fpt, " ARMINT[FM] PICK ", j); // 2019.03.07
			fprintf( fpt , " ARMINT[FM] PICK " ); // 2019.03.07
			//
			for ( i = CM1 ; i < TM ; i++ ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i ) > 0 ) {
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i ) );
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		for ( i = CM1 ; i < TM ; i++ ) {
			if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i ) > 0 ) break;
		}
		if ( i != TM ) {
			//
//			fprintf(fpt, " ARMINT[FM] PLCE ", j); // 2019.03.07
			fprintf( fpt , " ARMINT[FM] PLCE " ); // 2019.03.07
			//
			for ( i = CM1 ; i < TM ; i++ ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i ) > 0 ) {
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i ) );
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		//
		for ( i = CM1 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_Getting_Priority( i ) != 0 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			//
//			fprintf(fpt, " PRIORITY PICK   ", j); // 2019.03.07
			fprintf( fpt , " PRIORITY PICK   " ); // 2019.03.07
			//
			for ( i = CM1 ; i < MAX_CHAMBER ; i++ ) {
				if ( _i_SCH_PRM_GET_Getting_Priority( i ) != 0 ) {
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_Getting_Priority( i ) );
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		for ( i = CM1 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_Putting_Priority( i ) != 0 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			//
//			fprintf(fpt, " PRIORITY PLACE  ", j); // 2019.03.07
			fprintf( fpt , " PRIORITY PLACE  " ); // 2019.03.07
			//
			for ( i = CM1 ; i < MAX_CHAMBER ; i++ ) {
				if ( _i_SCH_PRM_GET_Putting_Priority( i ) != 0 ) {
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_Putting_Priority( i ) );
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		//-----------------------------------------------------------------------------------------------------------------------
		// 2015.06.18
		//
		for ( i = CM1 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i ) != 0 ) break;
			if ( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i ) != 0 ) break;
		}
		//
		if ( i != MAX_CHAMBER ) {
			//
			fprintf( fpt , " MDL_OPTION=WR:RR:NP:PP:MA:RT:FS:SS\n" );
			fprintf( fpt , "\t\t" );
			//
			for ( i = CM1 ; i < MAX_CHAMBER ; i++ ) {
				//
				if (
					( _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) == 0 ) &&
					( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i ) == 0 ) &&
					( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) == 0 ) &&
					( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i ) == 0 ) &&
					( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i ) == 0 ) &&
					( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( i ) == 0 ) &&
					( _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i ) == 0 ) &&
					( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i ) == 0 )
					) {
					continue;
				}
				else {
					//
					fprintf( fpt , "(%s=" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					//
					if ( _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) != 0 ) {
						fprintf( fpt , "WR%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( i ) );
					}
					if ( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i ) != 0 ) {
						fprintf( fpt , "RR%d" , _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i ) );
					}
					if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) != 0 ) {
						fprintf( fpt , "NP%d" , _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) );
					}
					if ( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i ) != 0 ) {
						fprintf( fpt , "PP%d" , _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i ) );
					}
					if ( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i ) != 0 ) {
						fprintf( fpt , "MA%d" , _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i ) );
					}
					if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( i ) != 0 ) {
						fprintf( fpt , "RT%d" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( i ) );
					}
					if ( _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i ) != 0 ) {
						fprintf( fpt , "FS%d" , _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i ) );
					}
					if ( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i ) != 0 ) {
						fprintf( fpt , "SS%d" , _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i ) );
					}
					//
					fprintf( fpt , ")" );
				}
			}
			//
			fprintf( fpt , "\n" );
		}
		//
		//-----------------------------------------------------------------------------------------------------------------------
		//
		// 2014.06.11
		//
		for ( i = 1 ; i <= 255 ; i++ ) {
			if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i ) != 0 ) {
				fprintf( fpt , "MFI_INTERFACE_FLOW_USER_OPTION(%d)	%d\n" , i , _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i ) );
			}
		}
		//
		//
		// 2015.06.18
		//
		for ( i = 1 ; i <= 255 ; i++ ) {
			if ( _i_SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( i ) != -1 ) {
				fprintf( fpt , "EIL_INTERFACE_FLOW_USER_OPTION(%d)	%d\n" , i , _i_SCH_PRM_GET_EIL_INTERFACE_FLOW_USER_OPTION( i ) );
			}
		}
		//
		fprintf( fpt , "<CLUSTER DATA AREA>\n" );
	}
	else { // 2006.10.19
		//===================================================================================
		if ( logpos == 1 ) { // 2015.06.25
	#ifdef _SCH_MIF
			f_enc_printf( fpt , "[REVISION_INFO][M]\n" );
	#else
			f_enc_printf( fpt , "[REVISION_INFO]\n" );
	#endif

			GetLocalTime( &SysTime ); // 2006.10.02
			//
			f_enc_printf( fpt , "\tTIME\t%04d/%02d/%02d %02d:%02d:%02d\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
			f_enc_printf( fpt , "\tMAIN\t%s,%s,(%s %s)\n" , SCH_LASTUPDATE_DATE , REGIST_INFO_STRING , __DATE__ , __TIME__ );
			if ( strlen( Revinfo ) > 0 ) f_enc_printf( fpt , "\tUSER\t%s\n" , Revinfo );
			//===================================================================================
			f_enc_printf( fpt , "[MODULE_INFO]\n" );
			//
//			for ( i = 0 ; i < 5 ; i++ ) { // 2015.06.25
			for ( i = 0 ; i < 4 ; i++ ) { // 2015.06.25
				if      ( i == 0 ) f_enc_printf( fpt , "\tMODE \t[" );
				else if ( i == 1 ) f_enc_printf( fpt , "\tSERVC\t[" );
				else if ( i == 2 ) f_enc_printf( fpt , "\tGROUP\t[" );
				else if ( i == 3 ) f_enc_printf( fpt , "\tSIZE \t[" );
//				else if ( i == 4 ) f_enc_printf( fpt , "\tDBSID\t[" ); // 2015.06.25
				//
				for ( j = CM1 ; j < MDL_LAST ; j++ ) {
					if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) f_enc_printf( fpt , ":" );
					//
					if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
						if      ( i == 0 ) {
	//						f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) ); // 2013.09.03
	//						f_enc_printf( fpt , "%d:%d" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) , _i_SCH_MODULE_GET_MOVE_MODE(j) ); // 2013.09.03 // 2014.11.13
							//
							switch( _i_SCH_MODULE_GET_MOVE_MODE(j) ) { // 2014.11.13
							case 1 : // En
								f_enc_printf( fpt , "%c" , 'A' + _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
								break;
							case 2 : // Dh
								f_enc_printf( fpt , "%c" , 'a' + _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
								break;
							case 3 : // EP
								f_enc_printf( fpt , "%c" , 'K' + _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
								break;
							case 0 : //
								f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
								break;
							default :
								f_enc_printf( fpt , "%d.%d" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) , _i_SCH_MODULE_GET_MOVE_MODE(j) );
								break;
							}
							//
						}
						else if ( i == 1 ) {
							f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_SERVICE_MODE( j ) );
						}
						else if ( i == 2 ) {
							f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_GROUP( j ) );
						}
						else if ( i == 3 ) {
							if      ( _i_SCH_PRM_GET_MODULE_SIZE( j ) >= 36 ) {
								f_enc_printf( fpt , "%c" , _i_SCH_PRM_GET_MODULE_SIZE( j ) - 36 + 'a' );
							}
							else if ( _i_SCH_PRM_GET_MODULE_SIZE( j ) >= 10 ) {
								f_enc_printf( fpt , "%c" , _i_SCH_PRM_GET_MODULE_SIZE( j ) - 10 + 'A' );
							}
							else {
								f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_SIZE( j ) );
							}
						}
//						else if ( i == 4 ) { // 2015.06.25
//							f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( j ) ); // 2015.06.25
//						} // 2015.06.25
					}
					else {
						f_enc_printf( fpt , " " );
					}

				}
				f_enc_printf( fpt , "]\n" );
			}
			//===================================================================================
		}
		else { // 2015.06.25
			//===================================================================================
			f_enc_printf( fpt , "[MODULE_INFO]\n" );
			//
			f_enc_printf( fpt , "\tMODE \t[" );
			//
			for ( j = CM1 ; j < MDL_LAST ; j++ ) {
				if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) f_enc_printf( fpt , ":" );
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( j ) == 1 ) {
					switch( _i_SCH_MODULE_GET_MOVE_MODE(j) ) { // 2014.11.13
					case 1 : // En
						f_enc_printf( fpt , "%c" , 'A' + _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
						break;
					case 2 : // Dh
						f_enc_printf( fpt , "%c" , 'a' + _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
						break;
					case 3 : // EP
						f_enc_printf( fpt , "%c" , 'K' + _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
						break;
					case 0 : //
						f_enc_printf( fpt , "%d" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) );
						break;
					default :
						f_enc_printf( fpt , "%d.%d" , _i_SCH_PRM_GET_MODULE_MODE_for_SW( j ) , _i_SCH_MODULE_GET_MOVE_MODE(j) );
						break;
					}
				}
				else {
					f_enc_printf( fpt , " " );
				}

			}
			//
			f_enc_printf( fpt , "]\n" );
			//===================================================================================
		}
		//===================================================================================
		/*
		// 2015.06.25
		f_enc_printf( fpt , "[SYSTEM_OPTION]\n" );
		if ( _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() != 0 )			f_enc_printf( fpt , "\tMAPPING_WHEN_TMFREE=%d\n" , _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() );
		if ( _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() != 0 )				f_enc_printf( fpt , "\tSCH_START_OPTION=%d\n" , _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() );
		if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() != 0 )		f_enc_printf( fpt , "\tEND_MESSAGE_SEND_POINT=%d\n" , _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() );
		if ( _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() != 0 )			f_enc_printf( fpt , "\tCHAMBER_PUT_PR_CHECK=%d\n" , _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() );
		if ( _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 0 )			f_enc_printf( fpt , "\tCHAMBER_GET_PR_CHECK=%d\n" , _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() );
		if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() != 0 )				f_enc_printf( fpt , "\tCM_SUPPLY_MODE=%d\n" , _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() );
		if ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() != 0 )		f_enc_printf( fpt , "\tPREPOST_PROCESS_DEPAND=%d\n" , _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() );
		if ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() != 0 )		f_enc_printf( fpt , "\tSTART_PARALLEL_CHECK_SKIP=%d\n" , _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() );
		if ( _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() != 0 )				f_enc_printf( fpt , "\tWAFER_SUPPLY_MODE=%d\n" , _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() );
		if ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() != 0 )	f_enc_printf( fpt , "\tCLUSTER_INCLUDE_FOR_DISABLE=%d\n" , _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() );
		if ( _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() != 0 )		f_enc_printf( fpt , "\tBMEND_SKIP_WHEN_RUNNING=%d\n" , _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() );
		if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() != 0 )		f_enc_printf( fpt , "\tPMREADY_SKIP_WHEN_RUNNING=%d\n" , _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() );
		if ( _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() != 0 )		f_enc_printf( fpt , "\tSW_BM_SCHEDULING_FACTOR=%d\n" , _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() );
		if ( _i_SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() != 0 )			f_enc_printf( fpt , "\tMULTI_GROUP_AUTO_ARRANGE=%d\n" , _i_SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() );
		if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() != 0 )				f_enc_printf( fpt , "\tCLSOUT_BM_SINGLE_ONLY=%d\n" , _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() );
		if ( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() != 0 )			f_enc_printf( fpt , "\tADAPTIVE_PROGRESSING=%d\n" , _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() );
		if ( _i_SCH_PRM_GET_DFIX_IOINTLKS_USE() != 0 )					f_enc_printf( fpt , "\tDFIX_IOINTLKS_USE=%d\n" , _i_SCH_PRM_GET_DFIX_IOINTLKS_USE() );
		if ( _i_SCH_PRM_GET_DFIX_CHAMBER_INTLKS_IOUSE() != 0 )			f_enc_printf( fpt , "\tDFIX_CHAMBER_INTLKS_IOUSE=%d\n" , _i_SCH_PRM_GET_DFIX_CHAMBER_INTLKS_IOUSE() ); // 2013.11.21
		if ( _i_SCH_PRM_GET_PICKPLACE_CANCEL_MESSAGE() != 0 )			f_enc_printf( fpt , "\tPICKPLACE_CANCEL_MESSAGE=%d\n" , _i_SCH_PRM_GET_PICKPLACE_CANCEL_MESSAGE() );
		if ( _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START() != 0 )			f_enc_printf( fpt , "\tTIMER_RESET_WHEN_NEW_START=%d\n" , _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START() );
		if ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() != 0 )				f_enc_printf( fpt , "\tCHECK_DIFFERENT_METHOD=%d\n" , _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() );
		if ( _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() != 0 )				f_enc_printf( fpt , "\tBATCH_SUPPLY_INTERRUPT=%d\n" , _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() );
		if ( _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() != 0 )				f_enc_printf( fpt , "\tDIFF_LOT_NOTSUP_MODE=%d\n" , _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() );
		if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 0 )					f_enc_printf( fpt , "\tFA_LOADUNLOAD_SKIP=%d\n" , _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() );
		if ( _i_SCH_PRM_GET_FA_MAPPING_SKIP() != 0 )					f_enc_printf( fpt , "\tFA_MAPPING_SKIP=%d\n" , _i_SCH_PRM_GET_FA_MAPPING_SKIP() );
		if ( _i_SCH_PRM_GET_DFIX_CASSETTE_READ_MODE() != 0 )			f_enc_printf( fpt , "\tDFIX_CASSETTE_READ_MODE=%d\n" , _i_SCH_PRM_GET_DFIX_CASSETTE_READ_MODE() );
		if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() != 0 )				f_enc_printf( fpt , "\tDFIX_TM_DOUBLE_CONTROL=%d\n" , _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() );
		if ( _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 )				f_enc_printf( fpt , "\tDFIX_FM_DOUBLE_CONTROL=%d\n" , _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() );
		if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() != 0 )				f_enc_printf( fpt , "\tDFIX_FIC_MULTI_CONTROL=%d\n" , _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() );
		if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() != 0 )				f_enc_printf( fpt , "\tDFIX_FAL_MULTI_CONTROL=%d\n" , _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() );
		if ( _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() != 0 )		f_enc_printf( fpt , "\tINTERLOCK_FM_BM_PLACE_SEPARATE=%d\n" , _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() );
		if ( SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() != 0 )				f_enc_printf( fpt , "\tMANUAL_CJPJJOBMODE=%d\n" , SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET() ); // 2008.05.09
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() != 0 )					f_enc_printf( fpt , "\tMTLOUT_INTERFACE=%d\n" , _i_SCH_PRM_GET_MTLOUT_INTERFACE() ); // 2011.05.26
		if ( _i_SCH_PRM_GET_READY_MULTIJOB_MODE() != 0 )				f_enc_printf( fpt , "\tREADY_MULTIJOB_MODE=%d\n" , _i_SCH_PRM_GET_READY_MULTIJOB_MODE() ); // 2014.02.03
		//
		if ( _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() != 0 )				f_enc_printf( fpt , "\tMULTIJOB_LOTRECIPE_READ=%d\n" , _i_SCH_MULTIJOB_GET_LOTRECIPE_READ() ); // 2015.06.17
		if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 0 )		f_enc_printf( fpt , "\tLOT_LOG_PRE_POST_PROCESS=%d\n" , _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() ); // 2015.06.17
		if ( _i_SCH_PRM_GET_DUMMY_MESSAGE() != 0 )						f_enc_printf( fpt , "\tDUMMY_MESSAGE=%d\n" , _i_SCH_PRM_GET_DUMMY_MESSAGE() ); // 2015.06.17
		if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 0 )				f_enc_printf( fpt , "\tDFIX_RECIPE_LOCKING=%d\n" , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() ); // 2015.06.17
		if ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() != 0 )					f_enc_printf( fpt , "\tPRCS_ZONE_RUN_TYPE=%d\n" , _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() ); // 2015.06.17
		if ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() != 0 )	f_enc_printf( fpt , "\tINTERLOCK_TM_BM_OTHERGROUP_SWAP=%d\n" , _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() ); // 2015.06.17
		*/
		//
		// 2015.06.25
		//
		i = 0;
		//
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE();					i++;	// 1
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_SCH_START_OPTION();					i++;	// 2
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT();				i++;	// 3
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK();				i++;	// 4
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK();				i++;	// 5
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE();						i++;	// 6
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND();				i++;	// 7
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP();			i++;	// 8
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE();					i++;	// 9
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE();			i++;	// 10
		//
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING();				i++;	// 11
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING();			i++;	// 12
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR();				i++;	// 13
		Log_Opt[i] = _i_SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE();					i++;	// 14
		Log_Opt[i] = _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY();					i++;	// 15
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING();				i++;	// 16
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_IOINTLKS_USE();						i++;	// 17
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_CHAMBER_INTLKS_IOUSE();				i++;	// 18
		Log_Opt[i] = _i_SCH_PRM_GET_PICKPLACE_CANCEL_MESSAGE();					i++;	// 19
		Log_Opt[i] = _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START();				i++;	// 20
		//
//		Log_Opt[i] = _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD();					i++;	// 21 // 2018.08.22
		Log_Opt[i] = _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD() + ( _i_SCH_PRM_GET_CHECK_DIFFERENT_METHOD2() * 100 );					i++;	// 21 // 2018.08.22
		Log_Opt[i] = _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT();					i++;	// 22
		Log_Opt[i] = _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE();						i++;	// 23
		Log_Opt[i] = _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP();						i++;	// 24
		Log_Opt[i] = _i_SCH_PRM_GET_FA_MAPPING_SKIP();							i++;	// 25
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_CASSETTE_READ_MODE();					i++;	// 26
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL();					i++;	// 27
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL();					i++;	// 28
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL();					i++;	// 29
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL();					i++;	// 30
		//
		Log_Opt[i] = _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE();			i++;	// 31
		Log_Opt[i] = SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET();					i++;	// 32
		Log_Opt[i] = _i_SCH_PRM_GET_MTLOUT_INTERFACE();							i++;	// 33
		Log_Opt[i] = _i_SCH_PRM_GET_READY_MULTIJOB_MODE();						i++;	// 34
		Log_Opt[i] = _i_SCH_MULTIJOB_GET_LOTRECIPE_READ();						i++;	// 35
		Log_Opt[i] = _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS();			i++;	// 36
		Log_Opt[i] = _i_SCH_PRM_GET_DUMMY_MESSAGE();							i++;	// 37
		Log_Opt[i] = _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING();						i++;	// 38
		Log_Opt[i] = _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE();						i++;	// 39
		Log_Opt[i] = _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP();			i++;	// 40
		//
		Log_Opt[i] = _i_SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE();				i++;	// 41
		//
		if ( logpos == 1 ) { // 2015.06.25
			//
			f_enc_printf( fpt , "[SYSTEM_OPTION]\n" );
			//
			for ( j = 0 ; j < i ; j++ ) {
				//
				LOG_OPTION[lotlog][j] = Log_Opt[j];
				//
				if ( Log_Opt[j] != 0 )			f_enc_printf( fpt , "\t[%d]=%d\n" , j + 1 , Log_Opt[j] );
				//
			}
			//
		}
		else {
			//
			lg = 0;
			//
			for ( j = 0 ; j < i ; j++ ) {
				//
				if ( LOG_OPTION[lotlog][j] != Log_Opt[j] ) {
					lg = 1;
					break;
				}
				//
			}
			//
			if ( lg != 0 ) {
				//
				f_enc_printf( fpt , "[CHANGE_OPTION]\n" );
				//
				for ( j = 0 ; j < i ; j++ ) {
					//
					if ( LOG_OPTION[lotlog][j] != Log_Opt[j] ) {
						//
						LOG_OPTION[lotlog][j] = Log_Opt[j];
						//
						f_enc_printf( fpt , "\t[%d]=%d\n" , j + 1 , Log_Opt[j] );
						//
					}
					//
				}
				//
			}
		}
		//
		//===================================================================================
		j = 0;
		for ( i = CM1 ; i <= FM ; i++ ) {
			if ( _i_SCH_MODULE_Get_Mdl_Run_Flag(i) != 0 ) {
				j = 1;
				break;
			}
		}
		if ( j == 1 ) {
			f_enc_printf( fpt , "[ALL_MDL_RUN_FLAG]" );

			for ( i = CM1 ; i <= FM ; i++ ) {
				if      ( i == CM1 ) f_enc_printf( fpt , "\n\tCM " );
				else if ( i == PM1 ) f_enc_printf( fpt , "\n\tPM " );
				else if ( i == AL  ) f_enc_printf( fpt , "\n\tAI " );
				else if ( i == BM1 ) f_enc_printf( fpt , "\n\tBM " );
				else if ( i == TM  ) f_enc_printf( fpt , "\n\tTM " );
				else if ( i == FM  ) f_enc_printf( fpt , "\n\tFM " );
				f_enc_printf( fpt , "[%02d]" , _i_SCH_MODULE_Get_Mdl_Run_Flag(i) );
			}
			f_enc_printf( fpt , "\n" );
		}
		//===================================================================================
		if ( logpos == 1 ) { // 2015.06.25
			//
			j = 0;
			for ( i = CM1 ; i < TM ; i++ ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL(i) != 0 ) {
					j = 1;
					break;
				}
				if ( _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL(i) != 0 ) {
					j = 1;
					break;
				}
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(i) != 0 ) {
					j = 1;
					break;
				}
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(i) != 0 ) {
					j = 1;
					break;
				}
			}
			if ( j == 0 ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 0 ) j = 1;
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) != 0 ) j = 1;
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) != 0 ) j = 1;
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) != 0 ) j = 1;
			}
			if ( j == 1 ) {
				f_enc_printf( fpt , "[ARM_INTERLOCK]" );
				for ( i = CM1 ; i < TM ; i++ ) {
					if      ( i == CM1 ) f_enc_printf( fpt , "\n\tCM  " );
					else if ( i == PM1 ) f_enc_printf( fpt , "\n\tPM  " );
					else if ( i == AL  ) continue;
					else if ( i == IC  ) continue;
					else if ( i == BM1 ) f_enc_printf( fpt , "\n\tBM  " );
					f_enc_printf( fpt , "[%02d,%02d/%02d,%02d]" ,
						_i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL(i) ,
						_i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL(i) ,
						_i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(i) ,
						_i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(i)
						);
				}
				f_enc_printf( fpt , "\n\tFAL [-/%02d,%02d]" , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) );
				f_enc_printf( fpt , "\n\tFIC [-/%02d,%02d]" , _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) , _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) );
				f_enc_printf( fpt , "\n" );
			}
			//===================================================================================
			if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_USE() ) { // 2012.08.23
				//
				f_enc_printf( fpt , "[PICK_ORDERING]" );
				//
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					f_enc_printf( fpt , "[%02d:%02d]" , i , _i_SCH_PRM_GET_MODULE_PICK_ORDERING(i) );
				}
				//
				f_enc_printf( fpt , "[SKIP]" );
				//
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
					if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i ) != 0 ) fprintf( fpt , "[%s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
				}
				f_enc_printf( fpt , "\n" );
			}
			//===================================================================================
			// 2015.06.18
			f_enc_printf( fpt , "[PRIORITY]" );
			for ( i = CM1 ; i < TM ; i++ ) {
				if      ( i == CM1 ) f_enc_printf( fpt , "\n\tCM  " );
				else if ( i == PM1 ) f_enc_printf( fpt , "\n\tPM  " );
				else if ( i == AL  ) continue;
				else if ( i == IC  ) continue;
				else if ( i == BM1 ) f_enc_printf( fpt , "\n\tBM  " );
				f_enc_printf( fpt , "[%02d,%02d]" ,
					_i_SCH_PRM_GET_Getting_Priority(i) ,
					_i_SCH_PRM_GET_Putting_Priority(i)
					);
			}
			f_enc_printf( fpt , "\n" );
		}
		//
		//===================================================================================
		f_enc_printf( fpt , "[LOT_INFO=%d]\n" , lotlog + 1 );
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//==================================================================
		if ( ( lotlog != -1 ) && ( lotlog != i ) ) continue; // 2006.10.19
		//==================================================================
//		if ( _i_SCH_SYSTEM_USING_GET(i) <= 0 ) continue;
		if ( lotlog == -1 ) { // 2006.10.19
			//
			j = _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_FOR_LOG( i ); // 2014.10.14
			//
//			fprintf( fpt , " SIDE %d = %s , group = %s , MSTS = %d , TOTAL = %d\n" , i+1 , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( i ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( i ) , BUTTON_GET_FLOW_STATUS_VAR( i ) , _insdie_run_Tag[i] ); // 2013.07.29
			fprintf( fpt , " SIDE %d = %s , group = %s , MSTS = %d , TOTAL = %d\n" , i , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( i ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( i ) , BUTTON_GET_FLOW_STATUS_VAR( i ) , _insdie_run_Tag[i] ); // 2013.07.29
//			fprintf( fpt , "	USING=%3d,USING2=%3d,LASTING=%2d,FA=%2d,MODE=%2d,RUNORDER=%2d,BEGIN=%2d,END=%2d(%d),ENDCHECK=%2d,LOOPMODE=%d,RESUME=%d,PAUSE=%d/%d,WAITR=%d,CPJOB=%d,CPJOB_ID=%d,PM_MODE=%d,BLANK=%d,RESTART=%d\n" , _i_SCH_SYSTEM_USING_GET(i) , _i_SCH_SYSTEM_USING2_GET(i) , _i_SCH_SYSTEM_LASTING_GET(i) , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) , SYSTEM_BEGIN_GET( i ) , _i_SCH_SYSTEM_MODE_END_GET(i) , _i_SCH_SYSTEM_APPEND_END_GET(i) , SYSTEM_ENDCHECK_GET(i) , _i_SCH_SYSTEM_MODE_LOOP_GET(i) , _i_SCH_SYSTEM_MODE_RESUME_GET(i) , _i_SCH_SYSTEM_PAUSE_GET(i) , _i_SCH_SYSTEM_PAUSE_SW_GET() , _i_SCH_SYSTEM_MODE_WAITR_GET(i) , _i_SCH_SYSTEM_CPJOB_GET(i) , _i_SCH_SYSTEM_CPJOB_ID_GET(i) , _i_SCH_SYSTEM_PMMODE_GET(i) , _i_SCH_SYSTEM_BLANK_GET(i) , _i_SCH_SYSTEM_RESTART_GET(i) ); // 2013.09.03
//			fprintf( fpt , "	USING=%3d,USING2=%3d,LASTING=%2d,FA=%2d,MODE=%2d,RUNORDER=%2d,BEGIN=%2d,END=%2d(%d),ENDCHECK=%2d,LOOPMODE=%d,RESUME=%d,PAUSE=%d/%d,WAITR=%d,CPJOB=%d,CPJOB_ID=%d,PM_MODE=%d,MOVE_MODE=%d,BLANK=%d,RESTART=%d\n" , _i_SCH_SYSTEM_USING_GET(i) , _i_SCH_SYSTEM_USING2_GET(i) , _i_SCH_SYSTEM_LASTING_GET(i) , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) , SYSTEM_BEGIN_GET( i ) , _i_SCH_SYSTEM_MODE_END_GET(i) , _i_SCH_SYSTEM_APPEND_END_GET(i) , SYSTEM_ENDCHECK_GET(i) , _i_SCH_SYSTEM_MODE_LOOP_GET(i) , _i_SCH_SYSTEM_MODE_RESUME_GET(i) , _i_SCH_SYSTEM_PAUSE_GET(i) , _i_SCH_SYSTEM_PAUSE_SW_GET() , _i_SCH_SYSTEM_MODE_WAITR_GET(i) , _i_SCH_SYSTEM_CPJOB_GET(i) , _i_SCH_SYSTEM_CPJOB_ID_GET(i) , _i_SCH_SYSTEM_PMMODE_GET(i) , _i_SCH_SYSTEM_MOVEMODE_GET( i ) , _i_SCH_SYSTEM_BLANK_GET(i) , _i_SCH_SYSTEM_RESTART_GET(i) ); // 2013.09.03 // 2015.06.18
//			fprintf( fpt , "	USING=%3d,USING2=%3d,LASTING=%2d,FA=%2d,MODE=%2d,RUNORDER=%2d,BEGIN=%2d,END=%2d(%d),ENDCHECK=%2d,LOOPMODE=%d,RESUME=%d,PAUSE=%d/%d,WAITR=%d\n" , _i_SCH_SYSTEM_USING_GET(i) , _i_SCH_SYSTEM_USING2_GET(i) , _i_SCH_SYSTEM_LASTING_GET(i) , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) , SYSTEM_BEGIN_GET( i ) , _i_SCH_SYSTEM_MODE_END_GET(i) , _i_SCH_SYSTEM_APPEND_END_GET(i) , SYSTEM_ENDCHECK_GET(i) , _i_SCH_SYSTEM_MODE_LOOP_GET(i) , _i_SCH_SYSTEM_MODE_RESUME_GET(i) , _i_SCH_SYSTEM_PAUSE_GET(i) , _i_SCH_SYSTEM_PAUSE_SW_GET() , _i_SCH_SYSTEM_MODE_WAITR_GET(i) ); // 2013.09.03 // 2015.06.18 // 2015.07.28
//			fprintf( fpt , "	USING=%3d,USING2=%3d,LASTING=%2d,FA=%2d,MODE=%2d(%d,%d,%d),RUNORDER=%2d,BEGIN=%2d,END=%2d(%d),ENDCHECK=%2d,LOOPMODE=%d,RESUME=%d,PAUSE=%d/%d,WAITR=%d\n" , _i_SCH_SYSTEM_USING_GET(i) , _i_SCH_SYSTEM_USING2_GET(i) , _i_SCH_SYSTEM_LASTING_GET(i) , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , SYSTEM_MODE_BUFFER_GET(i) , SYSTEM_MODE_FIRST_GET(i) , SYSTEM_MODE_FIRST_GET_BUFFER(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) , SYSTEM_BEGIN_GET( i ) , _i_SCH_SYSTEM_MODE_END_GET(i) , _i_SCH_SYSTEM_APPEND_END_GET(i) , SYSTEM_ENDCHECK_GET(i) , _i_SCH_SYSTEM_MODE_LOOP_GET(i) , _i_SCH_SYSTEM_MODE_RESUME_GET(i) , _i_SCH_SYSTEM_PAUSE_GET(i) , _i_SCH_SYSTEM_PAUSE_SW_GET() , _i_SCH_SYSTEM_MODE_WAITR_GET(i) ); // 2013.09.03 // 2015.06.18 // 2015.07.28 // 2016.01.12
//			fprintf( fpt , "	USING=%3d,USING2=%3d,LASTING=%2d,FA=%2d,MODE=%2d(%d,%d,%d),RUNORDER=%2d,BEGIN=%2d,END=%2d(%d),ENDCHECK=%2d,LOOPMODE=%d,RESUME=%d,PAUSE=%d/%d,WAITR=%d,ABORT=%d\n" , _i_SCH_SYSTEM_USING_GET(i) , _i_SCH_SYSTEM_USING2_GET(i) , _i_SCH_SYSTEM_LASTING_GET(i) , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , SYSTEM_MODE_BUFFER_GET(i) , SYSTEM_MODE_FIRST_GET(i) , SYSTEM_MODE_FIRST_GET_BUFFER(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) , SYSTEM_BEGIN_GET( i ) , _i_SCH_SYSTEM_MODE_END_GET(i) , _i_SCH_SYSTEM_APPEND_END_GET(i) , SYSTEM_ENDCHECK_GET(i) , _i_SCH_SYSTEM_MODE_LOOP_GET(i) , _i_SCH_SYSTEM_MODE_RESUME_GET(i) , _i_SCH_SYSTEM_PAUSE_GET(i) , _i_SCH_SYSTEM_PAUSE_SW_GET() , _i_SCH_SYSTEM_MODE_WAITR_GET(i) , SIGNAL_MODE_ABORT_GET(i) ); // 2013.09.03 // 2015.06.18 // 2015.07.28 // 2016.01.12 // 2016.08.12
			//
			fprintf( fpt , "	USING=%3d,USING2=%3d,LASTING=%2d,FA=%2d,MODE=%2d(%d,%d,%d),RUNORDER=%2d,BEGIN=%2d,END=%2d(%d:%d,%d),ENDCHECK=%2d,LOOPMODE=%d,RESUME=%d,PAUSE=%d/%d,WAITR=%d,ABORT=%d\n" , _i_SCH_SYSTEM_USING_GET(i) , _i_SCH_SYSTEM_USING2_GET(i) , _i_SCH_SYSTEM_LASTING_GET(i) , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , SYSTEM_MODE_BUFFER_GET(i) , SYSTEM_MODE_FIRST_GET(i) , SYSTEM_MODE_FIRST_GET_BUFFER(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) , SYSTEM_BEGIN_GET( i ) , _i_SCH_SYSTEM_MODE_END_GET(i) , _i_SCH_SYSTEM_APPEND_END_GET(i) , _i_SCH_SYSTEM_MODE_END_BUF1_GET(i) , _i_SCH_SYSTEM_MODE_END_BUF2_GET(i) , SYSTEM_ENDCHECK_GET(i) , _i_SCH_SYSTEM_MODE_LOOP_GET(i) , _i_SCH_SYSTEM_MODE_RESUME_GET(i) , _i_SCH_SYSTEM_PAUSE_GET(i) , _i_SCH_SYSTEM_PAUSE_SW_GET() , _i_SCH_SYSTEM_MODE_WAITR_GET(i) , SIGNAL_MODE_ABORT_GET(i) ); // 2013.09.03 // 2015.06.18 // 2015.07.28 // 2016.01.12 // 2016.08.12
			//
			fprintf( fpt , "	CPJOB=%d,CPJOB_ID=%d,PM_MODE=%d,MOVE_MODE=%d,BLANK=%d,RESTART=%d,DBUPDATE=%d,FINISH=%d\n" , _i_SCH_SYSTEM_CPJOB_GET(i) , _i_SCH_SYSTEM_CPJOB_ID_GET(i) , _i_SCH_SYSTEM_PMMODE_GET(i) , _i_SCH_SYSTEM_MOVEMODE_GET( i ) , _i_SCH_SYSTEM_BLANK_GET(i) , _i_SCH_SYSTEM_RESTART_GET(i) , _i_SCH_SYSTEM_DBUPDATE_GET(i) , _i_SCH_SYSTEM_FINISH_GET(i) ); // 2013.09.03 // 2015.06.18
//			fprintf( fpt , "	JOB_ID=(%s),MID=(%s)\n" , _i_SCH_SYSTEM_GET_JOB_ID(i) , _i_SCH_SYSTEM_GET_MID_ID(i) ); // 2009.07.16
//			fprintf( fpt , "	JOB_ID=(%s),MID=(%s),RECIPE=(%s),SLOT=(%d/%d),LOOP=(%d),MAP=(%d/%d),(%d,%d)(%d,%d)(%d)\n" , _i_SCH_SYSTEM_GET_JOB_ID(i) , _i_SCH_SYSTEM_GET_MID_ID(i) , _In_RecipeFileName[i] , _In_SL_1[i] , _In_SL_2[i] , _In_Loop[i] , SYSTEM_IN_MODULE_GET(i) , SYSTEM_OUT_MODULE_GET(i) , _In_Job_HandOff_In_Pre_Check[i] , _In_Run_Finished_Check[i] , _in_HOT_LOT_CHECKING[i] , _in_HOT_LOT_ORDERING_INVALID[i] , _in_UNLOAD_USER_TYPE[i] ); // 2013.07.29
//			fprintf( fpt , "	JOB_ID=(%s),MID=(%s),RECIPE=(%s),SLOT=(%d/%d),LOOPDO=(%d),MAP=(%d/%d),(HandOff_In_Pre=%d,Run_Finished=%d)(HOT_LOT_CHECK=%d,HOT_LOT_ORDERING_INVALID=%d)(UNLOAD_USER_TYPE=%d)\n" , _i_SCH_SYSTEM_GET_JOB_ID(i) , _i_SCH_SYSTEM_GET_MID_ID(i) , _In_RecipeFileName[i] , _In_SL_1[i] , _In_SL_2[i] , _In_Loop[i] , SYSTEM_IN_MODULE_GET(i) , SYSTEM_OUT_MODULE_GET(i) , _In_Job_HandOff_In_Pre_Check[i] , _In_Run_Finished_Check[i] , _in_HOT_LOT_CHECKING[i] , _in_HOT_LOT_ORDERING_INVALID[i] , _in_UNLOAD_USER_TYPE[i] ); // 2013.07.29 // 2014.10.14
//			fprintf( fpt , "	JOB_ID=(%s),MID=(%s),RECIPE=(%s),SLOT=(%d/%d),LOOPDO=(%d),MAP=(%d/%d),(HandOff_In_Pre=%d,Run_Finished=%d)(HOT_LOT_CHECK=%d,HOT_LOT_ORDERING_INVALID=%d)(UNLOAD_USER_TYPE=%d)(FM_PICK_POSSIBLE=%d:%d)\n" , _i_SCH_SYSTEM_GET_JOB_ID(i) , _i_SCH_SYSTEM_GET_MID_ID(i) , _In_RecipeFileName[i] , _In_SL_1[i] , _In_SL_2[i] , _In_Loop[i] , SYSTEM_IN_MODULE_GET(i) , SYSTEM_OUT_MODULE_GET(i) , _In_Job_HandOff_In_Pre_Check[i] , _In_Run_Finished_Check[i] , _in_HOT_LOT_CHECKING[i] , _in_HOT_LOT_ORDERING_INVALID[i] , _in_UNLOAD_USER_TYPE[i] , _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[i] , j ); // 2013.07.29 // 2014.10.14 // 2015.06.18
			fprintf( fpt , "	JOB_ID=(%s),MID=(%s),ESRC=(%d),RECIPE=(%s),SLOT=(%d/%d),LOOPDO=(%d),MAP=(%d/%d)\n" , _i_SCH_SYSTEM_GET_JOB_ID(i) , _i_SCH_SYSTEM_GET_MID_ID(i) , _i_SCH_SYSTEM_ESOURCE_ID_GET(i) , _In_RecipeFileName[i] , _In_SL_1[i] , _In_SL_2[i] , _In_Loop[i] , SYSTEM_IN_MODULE_GET(i) , SYSTEM_OUT_MODULE_GET(i) ); // 2013.07.29 // 2014.10.14 // 2015.06.18
//			fprintf( fpt , "	HandOff_In_Pre=%d,Run_Finished=%d,HOT_LOT_CHECK=%d,HOT_LOT_ORDERING_INVALID=%d,UNLOAD_USER_TYPE=%d,FM_PICK_POSSIBLE=(%d:%d)\n" , _In_Job_HandOff_In_Pre_Check[i] , _In_Run_Finished_Check[i] , _in_HOT_LOT_CHECKING[i] , _in_HOT_LOT_ORDERING_INVALID[i] , _in_UNLOAD_USER_TYPE[i] , _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[i] , j ); // 2013.07.29 // 2014.10.14 // 2015.06.18 // 2017.10.10
			fprintf( fpt , "	HandOff_In_Pre=%d,Run_Finished=%d,HOT_LOT_CHECK=%d,HOT_LOT_ORDERING_INVALID=%d,UNLOAD_USER_TYPE=%d,NOCHECK_RUNNING_TYPE=%d,FM_PICK_POSSIBLE=(%d:%d)\n" , _In_Job_HandOff_In_Pre_Check[i] , _In_Run_Finished_Check[i] , _in_HOT_LOT_CHECKING[i] , _in_HOT_LOT_ORDERING_INVALID[i] , _in_UNLOAD_USER_TYPE[i] , _in_NOCHECK_RUNNING_TYPE[i] , _in_FM_PICK_POSSIBLE_LAST_RETURNCODE[i] , j ); // 2013.07.29 // 2014.10.14 // 2015.06.18 // 2017.10.10
			fprintf( fpt , "	FM::DoPointer=%2d,InCnt(RETURN)=%2d,OutCnt(SUPPLY)=%2d,MidCnt=%2d,RunFlg=%2d,HWait=%2d,SwWait=%2d,LastOutPointer=%2d\n" , _i_SCH_MODULE_Get_FM_DoPointer_Sub(i) , _i_SCH_MODULE_Get_FM_InCount(i) , _i_SCH_MODULE_Get_FM_OutCount(i) , _i_SCH_MODULE_Get_FM_MidCount() , _i_SCH_MODULE_Get_FM_Runinig_Flag(i) , _i_SCH_MODULE_Get_FM_HWait(i) , _i_SCH_MODULE_Get_FM_SwWait( i ) , _i_SCH_MODULE_Get_FM_LastOutPointer( i ) );
			fprintf( fpt , "	TM::DoPointer=%2d,InCnt(RETURN)=%2d,OutCnt(SUPPLY)=%2d,DblCnt=%2d,MoveSignal=%2d\n" , _i_SCH_MODULE_Get_TM_DoPointer(i) , _i_SCH_MODULE_Get_TM_InCount(i) , _i_SCH_MODULE_Get_TM_OutCount(i) , _i_SCH_MODULE_Get_TM_DoubleCount(i) , _i_SCH_MODULE_Get_TM_Move_Signal( 0 ) );
			fprintf( fpt , "	SideFlag::Check_Flag=%2d,Check_Last_Supplied_Flag=%2d,Check_Target_Cluster_Count=%2d,Check_Current_Run_Count=%2d,Last_Status=%2d\n" , SCHEDULER_CONTROL_Get_Side_Check_Flag( i ),SCHEDULER_CONTROL_Get_Side_Check_Supply_Flag( i ),_i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( i ),SCHEDULER_CONTROL_Get_Side_Check_Current_Run_Count( i ) , _i_SCH_SUB_Get_Last_Status( i ) ); // 2003.10.23
			//
			fprintf( fpt , "	PROCESS_MONITOR_Status\n" );
			fprintf( fpt , "		" );
			for ( j = PM1 ; j < TM ; j++ ) {
				if ( PROCESS_MONITOR_Get_Status( i , j ) != 0 ) {
					if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[j] ) { // 2012.09.26
						fprintf( fpt , "(%s=%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , PROCESS_MONITOR_Get_Status( i , j ) );
					}
					else {
						fprintf( fpt , "(%s=%2d:K)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , PROCESS_MONITOR_Get_Status( i , j ) );
					}
				}
				else { // 2012.08.21
					k = _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( j , &pmc );
					if ( k > 0 ) {
						if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[j] ) { // 2012.09.26
							fprintf( fpt , "(%s=PO%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , k );
						}
						else {
							fprintf( fpt , "(%s=PO%2d:K)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , k );
						}
					}
					else { // 2012.09.26
						if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[j] ) { // 2012.09.26
							fprintf( fpt , "(%s=K)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) );
						}
					}
				}
			}
			fprintf( fpt , "\n" );
		}
		else {
			f_enc_printf( fpt , "	Parameter\n" );
			f_enc_printf( fpt , "		LOGFOLDER=%s , GROUP=%s\n" , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( i ) , _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( i ) );
			f_enc_printf( fpt , "		FAMODE=%2d , STARTMODE=%2d , PM_MODE=%d , RUNORDER=%2d\n" , _i_SCH_SYSTEM_FA_GET(i) , _i_SCH_SYSTEM_MODE_GET(i) , _i_SCH_SYSTEM_PMMODE_GET(i) , _i_SCH_SYSTEM_RUNORDER_GET(i) );
			f_enc_printf( fpt , "		JOB_ID=(%s) , MID=(%s) , CPJOB=%d , CPJOB_ID=%d\n" , _i_SCH_SYSTEM_GET_JOB_ID(i) , _i_SCH_SYSTEM_GET_MID_ID(i) , _i_SCH_SYSTEM_CPJOB_GET(i) , _i_SCH_SYSTEM_CPJOB_ID_GET(i) );
			//
			f_enc_printf( fpt , "	PROCESS_MONITOR_Status\n" );
			f_enc_printf( fpt , "		" );
			for ( j = PM1 ; j < TM ; j++ ) {
				if ( PROCESS_MONITOR_Get_Status( i , j ) != 0 ) {
					f_enc_printf( fpt , "(%s=%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , PROCESS_MONITOR_Get_Status( i , j ) );
				}
				else { // 2012.08.21
					k = _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( j , &pmc );
					if ( k > 0 ) {
						fprintf( fpt , "(%s=O:%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , k );
					}
					else {
					}
				}
			}
			f_enc_printf( fpt , "\n" );
		}
		//=================================================================================================================================================
		//=================================================================================================================================================
		//=================================================================================================================================================
		if ( lotlog == -1 ) {
			encryp_backup = _i_SCH_PRM_GET_LOG_ENCRIPTION();
			_i_SCH_PRM_SET_LOG_ENCRIPTION( 0 );
		}
		//======================================================
//		f_enc_printf( fpt , "	Mdl_Use_Flag\n" ); // 2014.10.28
//		f_enc_printf( fpt , "		" ); // 2014.10.28
		lg = 0; // 2014.10.28
//		for ( j = CM1 ; j < TM ; j++ ) f_enc_printf( fpt , "(%s=%2d,%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_MODULE_Get_Mdl_Use_Flag( i , j ) , _i_SCH_MODULE_Get_Mdl_Spd_Flag( i , j ) ); // 2009.08.05
//		for ( j = CM1 ; j <= FM ; j++ ) f_enc_printf( fpt , "(%s=%2d,%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_MODULE_Get_Mdl_Use_Flag( i , j ) , _i_SCH_MODULE_Get_Mdl_Spd_Flag( i , j ) ); // 2009.08.05 // 2012.02.01
		for ( j = CM1 ; j <= FM ; j++ ) {
			//
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , j ) == 0 ) &&
			   ( _i_SCH_MODULE_Get_Mdl_Spd_Flag( i , j ) == 0 ) ) continue; // 2012.02.01
			//
			if ( lg == 0 ) { // 2014.10.28
				lg = 1;
				f_enc_printf( fpt , "	Mdl_Use_Flag\n" );
				f_enc_printf( fpt , "		" );
			}
			//
			f_enc_printf( fpt , "(%s=%2d,%2d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_MODULE_Get_Mdl_Use_Flag( i , j ) , _i_SCH_MODULE_Get_Mdl_Spd_Flag( i , j ) ); // 2009.08.05
		}
//		f_enc_printf( fpt , "\n" ); // 2014.10.28
		if ( lg != 0 ) f_enc_printf( fpt , "\n" ); // 2014.10.28
		//
//		f_enc_printf( fpt , "	Mdl_Pre_Data\n" ); // 2014.10.28
//		f_enc_printf( fpt , "		" ); // 2014.10.28
		lg = 0; // 2014.10.28
//		for ( j = PM1 ; j < TM ; j++ ) f_enc_printf( fpt , "(%s=%d,%d,%s,%d,[%d,%d])" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_USE( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME( i , j ) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag(i,j) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag(i,j) ); // 2012.02.01
		for ( j = PM1 ; j < TM ; j++ ) {
			//
			if ( ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , j ) == 0 ) &&
			   ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( i , j ) == 0 ) &&
			   ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( i , j ) == 0 ) && // 2017.10.16
			   ( _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , j )[0] == 0 ) &&
			   ( _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME( i , j ) == 0 ) &&
			   ( SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag(i,j) == 0 ) &&
			   ( _in_LOTPRE_CLUSTER_DIFFER_MODE2[i][j] == 0 ) && // 2017.09.22
			   ( SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag(i,j) == 0 ) ) continue; // 2012.02.01
			//
			if ( lg == 0 ) { // 2014.10.28
				lg = 1;
				f_enc_printf( fpt , "	Mdl_Pre_Data\n" );
				f_enc_printf( fpt , "		" );
			}
			//
//			f_enc_printf( fpt , "(%s=%d,%d,%s,%d,[%d,%d])" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_USE( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME( i , j ) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag(i,j) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag(i,j) ); // 2012.02.01 // 2017.09.22
//			f_enc_printf( fpt , "(%s=%d,%d,%s,%d,[%d,%d][%d])" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_USE( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME( i , j ) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag(i,j) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag(i,j) , _in_LOTPRE_CLUSTER_DIFFER_MODE2[i][j] ); // 2012.02.01 // 2017.09.22 // 2017.10.16
			f_enc_printf( fpt , "(%s=<%d>,%d,%s,%d,[%d,%d][%d](%d))" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_inside_READY_RECIPE_USE( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , j ) , _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME( i , j ) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Use_Flag(i,j) , SCHEDULER_GLOBAL_Get_Chamber_FirstPre_Done_Flag(i,j) , _in_LOTPRE_CLUSTER_DIFFER_MODE2[i][j] , _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , j ) ); // 2012.02.01 // 2017.09.22 // 2017.10.16
			//
		}
//		f_enc_printf( fpt , "\n" ); // 2014.10.28
		if ( lg != 0 ) f_enc_printf( fpt , "\n" ); // 2014.10.28
		//
//		f_enc_printf( fpt , "	Mdl_Post_Data\n" ); // 2014.10.28
//		f_enc_printf( fpt , "		" ); // 2014.10.28
		lg = 0; // 2014.10.28
//		for ( j = PM1 ; j < TM ; j++ ) f_enc_printf( fpt , "(%s=%d,%s,%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_inside_END_RECIPE_USE( i , j ) , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( i , j ) , _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( i , j ) ); // 2012.02.01
		for ( j = PM1 ; j < TM ; j++ ) {
			//
			if ( ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( i , j ) == 0 ) &&
			   ( _i_SCH_PRM_GET_inside_END_RECIPE_NAME( i , j )[0] == 0 ) &&
			   ( _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( i , j ) == 0 ) ) continue; // 2012.02.01
			//
			if ( lg == 0 ) { // 2014.10.28
				lg = 1;
				f_enc_printf( fpt , "	Mdl_Post_Data\n" );
				f_enc_printf( fpt , "		" );
			}
			//
			f_enc_printf( fpt , "(%s=%d,%s,%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(j) , _i_SCH_PRM_GET_inside_END_RECIPE_USE( i , j ) , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( i , j ) , _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( i , j ) ); // 2012.02.01
		}
//		f_enc_printf( fpt , "\n" ); // 2014.10.28
		if ( lg != 0 ) f_enc_printf( fpt , "\n" ); // 2014.10.28
		//
//		f_enc_printf( fpt , "	SCH_Data\n" ); // 2014.10.28
		//
		lg = 0; // 2014.10.28
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//=======================================================
//			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) <   0 ) continue; // 2008.04.23
			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) == -1 ) continue; // 2008.04.23
			//=======================================================
			if ( _i_SCH_SYSTEM_USING_GET( i ) <= 0 ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) == 0 ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) == 0 ) {
						if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) == 0 ) {
							if ( _i_SCH_CLUSTER_Get_SlotIn( i , j ) == 0 ) {
								if ( _i_SCH_CLUSTER_Get_SlotOut( i , j ) == 0 ) {
									if ( _i_SCH_CLUSTER_Get_PathDo( i , j ) == 0 ) {
										if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == 0 ) {
											if ( _i_SCH_CLUSTER_Get_PathHSide( i , j ) == 0 ) {
												if ( _i_SCH_CLUSTER_Get_SwitchInOut( i , j ) == 0 ) {
													if ( _i_SCH_CLUSTER_Get_FailOut( i , j ) == 0 ) {
														if ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( i , j ) == -1 ) {
															if ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( i , j ) == -1 ) {
																if ( _i_SCH_CLUSTER_Get_Buffer( i , j ) == 0 ) {
																	if ( _i_SCH_CLUSTER_Get_User_PathIn( i , j ) == 0 ) {
																		if ( _i_SCH_CLUSTER_Get_User_PathOut( i , j ) == 0 ) {
																			if ( _i_SCH_CLUSTER_Get_User_OrderIn( i , j ) == 0 ) {
																				if ( _i_SCH_CLUSTER_Get_User_SlotOut( i , j ) == 0 ) {
																					if ( _i_SCH_CLUSTER_Get_Stock( i , j ) == 0 ) { // 2011.04.30
																						continue;
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			//=======================================================
			if ( lg == 0 ) { // 2014.10.28
				lg = 1;
				f_enc_printf( fpt , "	SCH_Data\n" ); // 2014.10.28
			}
			//=======================================================
//			f_enc_printf( fpt , "		SLOT=%2d,INDEX=%2d" , _i_SCH_CLUSTER_Get_SlotIn( i , j ) , j + 1 ); // 2013.07.29
			f_enc_printf( fpt , "		SLOT=%2d,PNT=%2d" , _i_SCH_CLUSTER_Get_SlotIn( i , j ) , j ); // 2013.07.29
			//
			if ( lotlog == -1 ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) == SCHEDULER_CLUSTER_DefaultUse_Data[i][j] ) { // 2017.09.08
					f_enc_printf( fpt , ",PathRange=%2d" , _i_SCH_CLUSTER_Get_PathRange( i , j ) );
				}
				else { // 2017.09.08
					if ( SCHEDULER_CLUSTER_DefaultUse_Data[i][j] == -9999 ) {
						f_enc_printf( fpt , ",PathRange=%2d(X)" , _i_SCH_CLUSTER_Get_PathRange( i , j ) );
					}
					else {
						f_enc_printf( fpt , ",PathRange=%2d(%d)" , _i_SCH_CLUSTER_Get_PathRange( i , j ) , SCHEDULER_CLUSTER_DefaultUse_Data[i][j] );
					}
				}
				//
				f_enc_printf( fpt , ",PathIn=%2d" , _i_SCH_CLUSTER_Get_PathIn( i , j ) );
				f_enc_printf( fpt , ",PathOut=%2d" , _i_SCH_CLUSTER_Get_PathOut( i , j ) );
				f_enc_printf( fpt , ",SlotIn=%2d" , _i_SCH_CLUSTER_Get_SlotIn( i , j ) );
				f_enc_printf( fpt , ",SlotOut=%2d" , _i_SCH_CLUSTER_Get_SlotOut( i , j ) );
			}
			else { // 2015.06.25
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) == SCHEDULER_CLUSTER_DefaultUse_Data[i][j] ) { // 2017.09.08
					f_enc_printf( fpt , ",Range=%2d" , _i_SCH_CLUSTER_Get_PathRange( i , j ) );
				}
				else { // 2017.09.08
					if ( SCHEDULER_CLUSTER_DefaultUse_Data[i][j] == -9999 ) {
						f_enc_printf( fpt , ",Range=%2d(X)" , _i_SCH_CLUSTER_Get_PathRange( i , j ) );
					}
					else {
						f_enc_printf( fpt , ",Range=%2d(%d)" , _i_SCH_CLUSTER_Get_PathRange( i , j ) , SCHEDULER_CLUSTER_DefaultUse_Data[i][j] );
					}
				}
				//
				f_enc_printf( fpt , ",Path=[%2d,%2d]->[%2d,%2d]" , _i_SCH_CLUSTER_Get_PathIn( i , j ) , _i_SCH_CLUSTER_Get_SlotIn( i , j ) , _i_SCH_CLUSTER_Get_PathOut( i , j ) , _i_SCH_CLUSTER_Get_SlotOut( i , j ) );
			}
			//
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_PathDo( i , j ) != 0 ) ) f_enc_printf( fpt , ",PathDo=%2d" , _i_SCH_CLUSTER_Get_PathDo( i , j ) );
			//
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) != SCHEDULER_READY ) ) {
				switch( _i_SCH_CLUSTER_Get_PathStatus( i , j ) ) {
				case SCHEDULER_READY			:	f_enc_printf( fpt , ",PathSts=%2d(READY)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_SUPPLY			:	f_enc_printf( fpt , ",PathSts=%2d(SUPLY)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_STARTING			:	f_enc_printf( fpt , ",PathSts=%2d(START)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_RUNNING			:	f_enc_printf( fpt , ",PathSts=%2d(RUNNG)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_RUNNINGW			:	f_enc_printf( fpt , ",PathSts=%2d(RUNNW)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_RUNNING2			:	f_enc_printf( fpt , ",PathSts=%2d(RUNN2)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_WAITING			:	f_enc_printf( fpt , ",PathSts=%2d(WAITG)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_BM_END			:	f_enc_printf( fpt , ",PathSts=%2d(BMEND)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_CM_END			:	f_enc_printf( fpt , ",PathSts=%2d(CMEND)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				case SCHEDULER_RETURN_REQUEST	:	f_enc_printf( fpt , ",PathSts=%2d(RETRQ)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				default							:	f_enc_printf( fpt , ",PathSts=%2d(?????)" , _i_SCH_CLUSTER_Get_PathStatus( i , j ) );	break;
				}
			}
			//
			f_enc_printf( fpt , ",ClusterIndex=%2d" , _i_SCH_CLUSTER_Get_ClusterIndex( i , j ) );
			//
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_SupplyID( i , j ) != 0 ) )		f_enc_printf( fpt , ",SupplyID=%2d" , _i_SCH_CLUSTER_Get_SupplyID( i , j ) ); // 2007.09.05
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_SwitchInOut( i , j ) != 0 ) )		f_enc_printf( fpt , ",SwitchInOut=%2d" , _i_SCH_CLUSTER_Get_SwitchInOut( i , j ) );
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_FailOut( i , j ) != 0 ) )			f_enc_printf( fpt , ",FailOut=%2d" , _i_SCH_CLUSTER_Get_FailOut( i , j ) );
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Extra( i , j ) != 0 ) )			f_enc_printf( fpt , ",Extra=%2d" , _i_SCH_CLUSTER_Get_Extra( i , j ) );
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_PathHSide( i , j ) != 0 ) )		f_enc_printf( fpt , ",PathHSide=%2d" , _i_SCH_CLUSTER_Get_PathHSide( i , j ) );
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_CPJOB_CONTROL( i , j ) != -1 ) )	f_enc_printf( fpt , ",CPJOB_CONTROL=%2d" , _i_SCH_CLUSTER_Get_CPJOB_CONTROL( i , j ) );
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_CPJOB_PROCESS( i , j ) != -1 ) )	f_enc_printf( fpt , ",CPJOB_PROCESS=%2d" , _i_SCH_CLUSTER_Get_CPJOB_PROCESS( i , j ) );
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Buffer( i , j ) != 0 ) )			f_enc_printf( fpt , ",Buffer=%2d" , _i_SCH_CLUSTER_Get_Buffer( i , j ) );
//			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Optimize( i , j ) != 0 ) )		f_enc_printf( fpt , ",Optimize=%2d" , _i_SCH_CLUSTER_Get_Optimize( i , j ) ); // 2015.06.25
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Optimize( i , j ) != -1 ) )		f_enc_printf( fpt , ",Optimize=%2d" , _i_SCH_CLUSTER_Get_Optimize( i , j ) ); // 2015.06.25
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Stock( i , j ) != 0 ) )			f_enc_printf( fpt , ",Stock=%2d" , _i_SCH_CLUSTER_Get_Stock( i , j ) );
			//
			if ( ( lotlog == -1 ) || ( logpos == 1 ) ) { // 2015.06.25
				//
				f_enc_printf( fpt , ",UserPathIn=%2d" , _i_SCH_CLUSTER_Get_User_PathIn( i , j ) );
				f_enc_printf( fpt , ",UserPathOut=%2d" , _i_SCH_CLUSTER_Get_User_PathOut( i , j ) );
				f_enc_printf( fpt , ",UserOderIn=%2d" , _i_SCH_CLUSTER_Get_User_OrderIn( i , j ) );
				f_enc_printf( fpt , ",UserSlotOut=%2d" , _i_SCH_CLUSTER_Get_User_SlotOut( i , j ) );
				//
			}
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) { // 2008.04.23
				//
				pmc = _i_SCH_CLUSTER_Get_PathRun( i , j , 0 , 2 );
				//
				if ( pmc > 1 ) f_enc_printf( fpt , ",MultiRunning=%d" , pmc );
				//
				if (
					( _i_SCH_CLUSTER_Get_PathRun( i , j ,  0 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j ,  8 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j ,  9 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) == 0 ) &&
					( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 4 ) == 0 )
					) {
					f_enc_printf( fpt , "\n" );
				}
				else {
					if ( lotlog == -1 ) {
						f_enc_printf( fpt , ",[" );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  0 , 2 ) != 0 ) f_enc_printf( fpt , "RunPickedCount(0.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 0 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  8 , 2 ) != 0 ) f_enc_printf( fpt , ":DeleteClusterDepth(8.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 8 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  9 , 2 ) != 0 ) f_enc_printf( fpt , ":DllCallChecking(9.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 9 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) != 0 ) f_enc_printf( fpt , ":TotalLoopCount(10.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) != 0 ) f_enc_printf( fpt , ":NextGoPointer(11.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) != 0 ) f_enc_printf( fpt , ":BeforePickWaitControl(12.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) != 0 ) f_enc_printf( fpt , ":AfterPickWaitControl(13.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) != 0 ) f_enc_printf( fpt , ":LotPrePostUse(14.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) != 0 ) f_enc_printf( fpt , ":LotPrePostStyle(15.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) != 0 ) f_enc_printf( fpt , ":WaferFlowControlDetailStyle(16.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) != 0 ) f_enc_printf( fpt , ":ReferenceNormalWaferPointer(Post Only)(17.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) != 0 ) f_enc_printf( fpt , ":WaitingReferencePointer(Post Only)(18.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) );
						if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) != 0 ) ) f_enc_printf( fpt , ":WaitingReferenceModule(Post Only)(19.2)=%02d/WaitingReferenceOrder(19.3)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) );
						if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) != 0 ) ) f_enc_printf( fpt , ":DetailRetuningInfoCount(20.2)=%02d/(20.3)=%02d/(20.4)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 4 ) );
						f_enc_printf( fpt , "]\n" );
					}
					else { // 2015.06.25
						f_enc_printf( fpt , ",[" );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  0 , 2 ) != 0 ) f_enc_printf( fpt , "(0.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 0 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  8 , 2 ) != 0 ) f_enc_printf( fpt , ":(8.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 8 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  9 , 2 ) != 0 ) f_enc_printf( fpt , ":(9.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 9 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) != 0 ) f_enc_printf( fpt , ":(10.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) != 0 ) f_enc_printf( fpt , ":(11.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) != 0 ) f_enc_printf( fpt , ":(12.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) != 0 ) f_enc_printf( fpt , ":(13.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) != 0 ) f_enc_printf( fpt , ":(14.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) != 0 ) f_enc_printf( fpt , ":(15.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) != 0 ) f_enc_printf( fpt , ":(16.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) != 0 ) f_enc_printf( fpt , ":(17.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) );
						if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) != 0 ) f_enc_printf( fpt , ":(18.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) );
						if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) != 0 ) ) f_enc_printf( fpt , ":(19.2)=%02d/(19.3)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) );
						if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) != 0 ) ) f_enc_printf( fpt , ":(20.2)=%02d/(20.3)=%02d/(20.4)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 4 ) );
						f_enc_printf( fpt , "]\n" );
					}
				}
				//
				continue;
			}
			//
//			if ( lotlog == -1 )	f_enc_printf( fpt , ",TimeSE=%d:%d" , _i_SCH_CLUSTER_Get_StartTime( i , j ) , _i_SCH_CLUSTER_Get_EndTime( i , j ) ); // 2009.08.05 // 2014.02.03
			//
			if ( lotlog == -1 )	{ // 2014.02.03
				//
				f_enc_printf( fpt , ",TimeSE=" );
				//
				SYSTEM_PRGS_TIME_GET( i , _i_SCH_CLUSTER_Get_StartTime( i , j ) , Revinfo );	f_enc_printf( fpt , "%s:" , Revinfo );
				SYSTEM_PRGS_TIME_GET( i , _i_SCH_CLUSTER_Get_EndTime( i , j ) , Revinfo );		f_enc_printf( fpt , "%s" , Revinfo );
				//
			}
			//
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_LotSpecial( i , j ) != 0 ) )	f_enc_printf( fpt , ",LotSpecial=%2d" , _i_SCH_CLUSTER_Get_LotSpecial( i , j ) );
			//
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_ClusterSpecial( i , j ) != 0 ) )	f_enc_printf( fpt , ",ClusterSpecial=%2d" , _i_SCH_CLUSTER_Get_ClusterSpecial( i , j ) ); // 2014.06.23
			//
			if ( _i_SCH_CLUSTER_Get_Ex_EILInfo( i , j ) != NULL ) {
				f_enc_printf( fpt , ",EILInfo=%s" , _i_SCH_CLUSTER_Get_Ex_EILInfo( i , j ) );
			}
			//
			if ( _i_SCH_CLUSTER_Get_LotUserSpecial( i , j ) == NULL ) {
				if ( lotlog == -1 )	f_enc_printf( fpt , ",LotUserSpecial=NULL" );
			}
			else {
				f_enc_printf( fpt , ",LotUserSpecial=%s" , _i_SCH_CLUSTER_Get_LotUserSpecial( i , j ) );
			}
			//
			if ( _i_SCH_CLUSTER_Get_ClusterUserSpecial( i , j ) == NULL ) {
				if ( lotlog == -1 )	f_enc_printf( fpt , ",ClusterUserSpecial=NULL" );
			}
			else {
				f_enc_printf( fpt , ",ClusterUserSpecial=%s" , _i_SCH_CLUSTER_Get_ClusterUserSpecial( i , j ) );
			}
			//
			if ( _i_SCH_CLUSTER_Get_ClusterTuneData( i , j ) == NULL ) {
				if ( lotlog == -1 )	f_enc_printf( fpt , ",ClusterTuneData=NULL" );
			}
			else {
				f_enc_printf( fpt , ",ClusterTuneData=%s" , _i_SCH_CLUSTER_Get_ClusterTuneData( i , j ) );
			}
			//
//			f_enc_printf( fpt , ",DisableSkip=%d,PrcsID=%d" , _i_SCH_CLUSTER_Get_Ex_DisableSkip( i , j ) , _i_SCH_CLUSTER_Get_Ex_PrcsID( i , j ) ); // 2012.04.20 // 2015.06.25
			//
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Ex_DisableSkip( i , j ) != 0 ) )	f_enc_printf( fpt , ",DisableSkip=%d" , _i_SCH_CLUSTER_Get_Ex_DisableSkip( i , j ) ); // 2012.04.20 // 2015.06.25
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Ex_UserDummy( i , j ) != 0 ) )	f_enc_printf( fpt , ",UserDummy=%d" , _i_SCH_CLUSTER_Get_Ex_UserDummy( i , j ) ); // 2015.10.12
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Ex_PrcsID( i , j ) != 0 ) )		f_enc_printf( fpt , ",PrcsID=%d" , _i_SCH_CLUSTER_Get_Ex_PrcsID( i , j ) ); // 2012.04.20 // 2015.06.25
			if ( ( lotlog == -1 ) || ( _i_SCH_CLUSTER_Get_Ex_OrderMode( i , j ) != 0 ) )	f_enc_printf( fpt , ",OrderMode=%d" , _i_SCH_CLUSTER_Get_Ex_OrderMode( i , j ) ); // 2016.08.25
			//
			// 2011.08.13
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) != 0 ) || ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( i , j ) != 0 ) ) { // 2016.11.02
				//
				f_enc_printf( fpt , ",Ex_MtlOut=%d,Ex_CarrierIndex=%d,%d,Ex_MtlOutWait=%d" ,  _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( i , j ) , _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( i , j ) , _i_SCH_CLUSTER_Get_Ex_MtlOutWait( i , j ) );
				//
				if ( _i_SCH_CLUSTER_Get_Ex_JobName( i , j ) == NULL ) {
					if ( lotlog == -1 )	f_enc_printf( fpt , ",Ex_JobName=NULL" );
				}
				else {
					f_enc_printf( fpt , ",Ex_JobName=%s" , _i_SCH_CLUSTER_Get_Ex_JobName( i , j ) );
				}
				//
				if ( _i_SCH_CLUSTER_Get_Ex_PPID( i , j ) == NULL ) {
					if ( lotlog == -1 )	f_enc_printf( fpt , ",Ex_PPID=NULL" );
				}
				else {
					f_enc_printf( fpt , ",Ex_PPID=%s" , _i_SCH_CLUSTER_Get_Ex_PPID( i , j ) );
				}
				//
				if ( _i_SCH_CLUSTER_Get_Ex_MaterialID( i , j ) == NULL ) {
					if ( lotlog == -1 )	f_enc_printf( fpt , ",Ex_MaterialID=NULL" );
				}
				else {
					f_enc_printf( fpt , ",Ex_MaterialID=%s" , _i_SCH_CLUSTER_Get_Ex_MaterialID( i , j ) );
				}
				//
				if ( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( i , j ) == NULL ) {
					if ( lotlog == -1 )	f_enc_printf( fpt , ",Ex_OutMaterialID=NULL" );
				}
				else {
					f_enc_printf( fpt , ",Ex_OutMaterialID=%s" , _i_SCH_CLUSTER_Get_Ex_OutMaterialID( i , j ) );
				}
				//
			}
			//
			if ( _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( i , j ) != 0 ) {
				//
				f_enc_printf( fpt , ",UserControl_Mode=%d,UserControl_Data=%s" ,  _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( i , j ) , _i_SCH_CLUSTER_Get_Ex_UserControl_Data( i , j ) );
				//
			}
			//
			// 2015.06.19
			//
			if ( _i_SCH_CLUSTER_Get_UserArea( i , j )  != NULL ) f_enc_printf( fpt , ",UserArea=%s" , _i_SCH_CLUSTER_Get_UserArea( i , j ) );
			if ( _i_SCH_CLUSTER_Get_UserArea2( i , j ) != NULL ) f_enc_printf( fpt , ",UserArea2=%s" , _i_SCH_CLUSTER_Get_UserArea2( i , j ) );
			if ( _i_SCH_CLUSTER_Get_UserArea3( i , j ) != NULL ) f_enc_printf( fpt , ",UserArea3=%s" , _i_SCH_CLUSTER_Get_UserArea3( i , j ) );
			if ( _i_SCH_CLUSTER_Get_UserArea4( i , j ) != NULL ) f_enc_printf( fpt , ",UserArea4=%s" , _i_SCH_CLUSTER_Get_UserArea4( i , j ) );
			//
			//
			pmc = _i_SCH_CLUSTER_Get_PathRun( i , j , 0 , 2 );
			//
			if ( pmc > 1 ) f_enc_printf( fpt , ",MultiRunning=%d" , pmc );
			//
			if (
				( _i_SCH_CLUSTER_Get_PathRun( i , j ,  0 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j ,  8 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j ,  9 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) == 0 ) &&
				( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 4 ) == 0 )
				) {
				f_enc_printf( fpt , "\n" );
			}
			else {
				if ( lotlog == -1 ) {
					f_enc_printf( fpt , ",[" );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  0 , 2 ) != 0 ) f_enc_printf( fpt , "RunPickedCount(0.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 0 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  8 , 2 ) != 0 ) f_enc_printf( fpt , ":DeleteClusterDepth(8.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 8 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  9 , 2 ) != 0 ) f_enc_printf( fpt , ":DllCallChecking(9.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 9 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) != 0 ) f_enc_printf( fpt , ":TotalLoopCount(10.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) != 0 ) f_enc_printf( fpt , ":NextGoPointer(11.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) != 0 ) f_enc_printf( fpt , ":BeforePickWaitControl(12.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) != 0 ) f_enc_printf( fpt , ":AfterPickWaitControl(13.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) != 0 ) f_enc_printf( fpt , ":LotPrePostUse(14.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) != 0 ) f_enc_printf( fpt , ":LotPrePostStyle(15.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) != 0 ) f_enc_printf( fpt , ":WaferFlowControlDetailStyle(16.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) != 0 ) f_enc_printf( fpt , ":ReferenceNormalWaferPointer(Post Only)(17.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) != 0 ) f_enc_printf( fpt , ":WaitingReferencePointer(Post Only)(18.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) );
					if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) != 0 ) ) f_enc_printf( fpt , ":WaitingReferenceModule(Post Only)(19.2)=%02d/WaitingReferenceOrder(19.3)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) );
					if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) != 0 ) ) f_enc_printf( fpt , ":DetailRetuningInfoCount(20.2)=%02d/(20.3)=%02d/(20.4)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 4 ) );
					f_enc_printf( fpt , "]\n" );
				}
				else { // 2015.06.25
					f_enc_printf( fpt , ",[" );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  0 , 2 ) != 0 ) f_enc_printf( fpt , "(0.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 0 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  8 , 2 ) != 0 ) f_enc_printf( fpt , ":(8.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 8 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j ,  9 , 2 ) != 0 ) f_enc_printf( fpt , ":(9.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 9 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) != 0 ) f_enc_printf( fpt , ":(10.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 10 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) != 0 ) f_enc_printf( fpt , ":(11.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 11 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) != 0 ) f_enc_printf( fpt , ":(12.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 12 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) != 0 ) f_enc_printf( fpt , ":(13.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 13 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) != 0 ) f_enc_printf( fpt , ":(14.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 14 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) != 0 ) f_enc_printf( fpt , ":(15.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 15 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) != 0 ) f_enc_printf( fpt , ":(16.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 16 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) != 0 ) f_enc_printf( fpt , ":(17.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 17 , 2 ) );
					if ( _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) != 0 ) f_enc_printf( fpt , ":(18.2)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 18 , 2 ) );
					if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) != 0 ) ) f_enc_printf( fpt , ":(19.2)=%02d/(19.3)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 19 , 3 ) );
					if ( ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) != 0 ) || ( _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) != 0 ) ) f_enc_printf( fpt , ":(20.2)=%02d/(20.3)=%02d/(20.4)=%02d" , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 2 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 3 ) , _i_SCH_CLUSTER_Get_PathRun( i , j , 20 , 4 ) );
					f_enc_printf( fpt , "]\n" );
				}
			}
			//=======================================================================================
			for ( k = 0 ; k < MAX_CLUSTER_DEPTH ; k++ ) {
				if ( ( _i_SCH_CLUSTER_Get_PathDo( i , j ) - 1 ) == k ) {
					c = '>';
				}
				else {
					c = ' ';
				}
				for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
					//
					pmc = _i_SCH_CLUSTER_Get_PathProcessChamber( i , j , k , l );
					//
					if ( pmc > 0 ) {
						//=======================================================================================
						if ( lotlog == -1 ) {
							if ( _i_SCH_CLUSTER_Get_PathRun( i , j , k , 0 ) == pmc ) {
								f_enc_printf( fpt , "			%c [E]    ClusterDepth=%2d,PMDepth=%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
							else {
								f_enc_printf( fpt , "			%c    [e] ClusterDepth=%2d,PMDepth=%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
						}
						else { // 2015.06.25
							if ( _i_SCH_CLUSTER_Get_PathRun( i , j , k , 0 ) == pmc ) {
								f_enc_printf( fpt , "			%c E    %2d,%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
							else {
								f_enc_printf( fpt , "			%c    e %2d,%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
						}
						//=======================================================================================
						if ( _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k ) != NULL ) { // 2007.02.21
//							if      ( _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[pmc - PM1] >= 100 ) { // 2007.07.29 // 2015.04.10
							if      ( _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , (pmc - PM1) ) >= 100 ) { // 2007.07.29 // 2015.04.10
//								f_enc_printf( fpt , "\t<$%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[pmc - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<$%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , (pmc - PM1) ) ); // 2015.04.10
							}
//							else if ( _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[pmc - PM1] < 0 ) { // 2015.04.10
							else if ( _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , (pmc - PM1) ) < 0 ) { // 2015.04.10
//								f_enc_printf( fpt , "\t<*%d>" , -_i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[pmc - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<*%d>" , -_i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , (pmc - PM1) ) ); // 2015.04.10
							}
							else {
//								f_enc_printf( fpt , "\t<%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[pmc - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , (pmc - PM1) ) ); // 2015.04.10
							}
						}
						if ( _i_SCH_CLUSTER_Get_PathProcessDepth( i , j , k ) != NULL ) { // 2014.01.28
//							if ( _i_SCH_CLUSTER_Get_PathProcessDepth( i , j , k )[pmc - PM1] > 0 ) { // 2015.04.10
							if ( _i_SCH_CLUSTER_Get_PathProcessDepth2( i , j , k , ( pmc - PM1 ) ) > 0 ) { // 2015.04.10
//								f_enc_printf( fpt , "\t<#:%d>" , _i_SCH_CLUSTER_Get_PathProcessDepth( i , j , k )[pmc - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<#:%d>" , _i_SCH_CLUSTER_Get_PathProcessDepth2( i , j , k , (pmc - PM1) ) ); // 2015.04.10
							}
						}
						//=======================================================================================
						f_enc_printf( fpt , "\n" );
						//=======================================================================================
					}
					else if ( pmc < 0 ) { // 2003.10.23
						//=======================================================================================
						if ( lotlog == -1 ) {
							if ( _i_SCH_CLUSTER_Get_PathRun( i , j , k , 0 ) == -pmc ) {
								f_enc_printf( fpt , "			%c [D]    ClusterDepth=%2d,PMDepth=%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( -pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
							else {
								f_enc_printf( fpt , "			%c    [d] ClusterDepth=%2d,PMDepth=%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( -pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
						}
						else { // 2015.06.25
							if ( _i_SCH_CLUSTER_Get_PathRun( i , j , k , 0 ) == -pmc ) {
								f_enc_printf( fpt , "			%c D    %2d,%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( -pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
							else {
								f_enc_printf( fpt , "			%c    d %2d,%2d => %s[%s][%s][%s]" , c , k , l , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( -pmc ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 0 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 1 ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( i , j , k , l , 2 ) );
							}
						}
						//=======================================================================================
						if ( _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k ) != NULL ) { // 2007.02.21
//							if      ( _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[(-pmc) - PM1] >= 100 ) { // 2007.07.29 // 2015.04.10
							if      ( _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , ((-pmc) - PM1) ) >= 100 ) { // 2007.07.29 // 2015.04.10
//								f_enc_printf( fpt , "\t<$%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[(-pmc) - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<$%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , ((-pmc) - PM1) ) ); // 2015.04.10
							}
//							else if ( _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[(-pmc) - PM1] < 0 ) { // 2015.04.10
							else if ( _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , ((-pmc) - PM1) ) < 0 ) { // 2015.04.10
//								f_enc_printf( fpt , "\t<*%d>" , -_i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[(-pmc) - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<*%d>" , -_i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , ((-pmc) - PM1) ) ); // 2015.04.10
							}
							else {
//								f_enc_printf( fpt , "\t<%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel( i , j , k )[(-pmc) - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<%d>" , _i_SCH_CLUSTER_Get_PathProcessParallel2( i , j , k , ((-pmc) - PM1) ) ); // 2015.04.10
							}
						}
						if ( _i_SCH_CLUSTER_Get_PathProcessDepth( i , j , k ) != NULL ) { // 2014.01.28
//							if ( _i_SCH_CLUSTER_Get_PathProcessDepth( i , j , k )[(-pmc) - PM1] > 0 ) { // 2015.04.10
							if ( _i_SCH_CLUSTER_Get_PathProcessDepth2( i , j , k , ((-pmc) - PM1) ) > 0 ) { // 2015.04.10
//								f_enc_printf( fpt , "\t<#:%d>" , _i_SCH_CLUSTER_Get_PathProcessDepth( i , j , k )[(-pmc) - PM1] ); // 2015.04.10
								f_enc_printf( fpt , "\t<#:%d>" , _i_SCH_CLUSTER_Get_PathProcessDepth2( i , j , k , ((-pmc) - PM1) ) ); // 2015.04.10
							}
						}
						//=======================================================================================
						f_enc_printf( fpt , "\n" );
						//=======================================================================================
					}
				}
				//=======================================================================================
				pmc = _i_SCH_CLUSTER_Get_PathRun( i , j , k , 1 ); // 2008.01.11
				//
				if ( lotlog == -1 ) {
//					if ( pmc > 0 ) f_enc_printf( fpt , "			     [U] ClusterDepth=%2d,Updated=%2d\n" , l , pmc ); // 2015.07.02
					if ( pmc > 0 ) f_enc_printf( fpt , "			     [U] ClusterDepth=%2d,Updated=%2d\n" , k , pmc ); // 2015.07.02
				}
				else { // 2015.06.25
//					if ( pmc > 0 ) f_enc_printf( fpt , "			     [U] %2d,Updated=%2d\n" , l , pmc ); // 2015.07.02
					if ( pmc > 0 ) f_enc_printf( fpt , "			     [U] %2d,Updated=%2d\n" , k , pmc ); // 2015.07.02
				}
				//=======================================================================================
			}
		}
		//======================================================
		if ( lotlog == -1 ) _i_SCH_PRM_SET_LOG_ENCRIPTION( encryp_backup );
		//=================================================================================================================================================
		//=================================================================================================================================================
		//=================================================================================================================================================
		if ( lotlog == -1 ) { // 2006.10.19
			fprintf( fpt , "	THD_Check_Flag = %d , RUN_TAG = %d , TM_COUNT = %d , Sleeping_Count = %d\n" , SYSTEM_THDCHECK_GET( i ) , _i_SCH_SYSTEM_RUN_TAG_GET( i ) , _Trace_TM_Count[i] , _Trace_Sleeping_Count[i] );
			//
			fprintf( fpt , "	TM_Use_Flag =" );
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) fprintf( fpt , "[%d]" , _i_SCH_SYSTEM_TM_GET( j , i ) );
			fprintf( fpt , "\n" );
			//
			/*
			// 2014.03.12
			fprintf( fpt , "	FM_Flow_Flag=%d , TM_Flow_Flag=" , _Trace_FEM_Flow[i] );
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) fprintf( fpt , "[%d]" , _Trace_TM_Flow[i][j] );
			fprintf( fpt , "\n" );
			*/
			//
			// 2014.03.12
			fprintf( fpt , "	FM_Flow_Flag=%d/%d , TM_Flow_Flag=" , _Trace_FEM_Flow[i] , _Trace_FM_Flow_In[i] );
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) fprintf( fpt , "[%d/%d/%d]" , _Trace_TM_Flow[i][j] , _Trace_TM_Flow_In[i][j] , _Trace_TM_Flow_Kill[i][j] );
			//
			fprintf( fpt , " UR_Tag=" );
			//
			for ( j = 0 ; j < USER_RECIPE_MAX_TAG ; j++ ) {
				//
				if ( ( USER_RECIPE_START_TAG[j][i] != 0 ) || ( USER_RECIPE_START_END[j][i] != 0 ) ) {
					fprintf( fpt , "[%d:%d,%d]" , j , USER_RECIPE_START_TAG[j][i] , USER_RECIPE_START_END[j][i] ); // 2015.07.09
				}
				//
			}
			//
			fprintf( fpt , "\n" );
			//
			// 2015.08.26
			fprintf( fpt , "	Load,Unload_Flag =" );
			fprintf( fpt , "L_C=%d,LS=%d,LQ=%d," , FA_LOAD_CONTROLLER_RUN[i] , FA_LOADING_SIGNAL[i] , FA_LOADREQ_SIGNAL[i] );
			fprintf( fpt , " M=%d,MC=%d," , FA_MAPPING_SIGNAL[i] , FA_MAPPING_CANCEL[i] );
			fprintf( fpt , " U_C=%d,US=%d" , FA_UNLOAD_CONTROLLER_RUN[i] , FA_UNLOADING_SIGNAL[i] );
			fprintf( fpt , "\n" );
			//
		}
		//=================================================================================================================================================
		//=================================================================================================================================================
		//=================================================================================================================================================
	}
	//==============
	if ( lotlog != -1 ) { // 2006.10.19
		fclose(fpt);
		//
		if ( filename[0] == 0 ) {
			_SCH_COMMON_SYSTEM_DATA_LOG( SCHEDULER_DEBUG_DATA_FILE , lotlog );
		}
		else {
			_SCH_COMMON_SYSTEM_DATA_LOG( filename , lotlog ); // 2008.04.17
		}
		return;
	}
	//==============
	//========================================================================================================
	fprintf( fpt , " ---------------------\n" );
//	fprintf( fpt , " RUNCHECK=%2d\n" , _i_SCH_SYSTEM_RUNCHECK_GET() ); // 2015.06.18
	fprintf( fpt , " RUNCHECK=%2d,%d\n" , _i_SCH_SYSTEM_RUNCHECK_GET() , _i_SCH_SYSTEM_RUNCHECK_FM_GET() ); // 2015.06.18
	fprintf( fpt , " ---------------------\n\n" );
	//
	//========================================================================================================
	fprintf( fpt , "<MODULE DATA AREA>\n" );
	//
	if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) != 0 ) { // 2015.06.18
		fprintf( fpt , " FM_DUAL_EXTEND_STYLE=%d\n" , _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
	}
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		if ( _i_SCH_MODULE_Get_FM_WAFER(i) == 0 ) {
			fprintf( fpt , " FM(F%2d)[H%d],WFR=----,SID=%2d,PNT=%2d,MODE=%d,IO_WFR=%4d,IO_SRC=%2d\n" , i+1 , _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE(i) , _i_SCH_MODULE_Get_FM_SIDE(i) , _i_SCH_MODULE_Get_FM_POINTER(i) , _i_SCH_MODULE_Get_FM_MODE(i) , WAFER_STATUS_IN_FM(0,i) , WAFER_SOURCE_IN_FM(0,i) );
		}
		else {
			fprintf( fpt , " FM(F%2d)[H%d],WFR=%4d,SID=%2d,PNT=%2d,MODE=%d,IO_WFR=%4d,IO_SRC=%2d\n" , i+1 , _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE(i) , _i_SCH_MODULE_Get_FM_WAFER(i) , _i_SCH_MODULE_Get_FM_SIDE(i) , _i_SCH_MODULE_Get_FM_POINTER(i) , _i_SCH_MODULE_Get_FM_MODE(i) , WAFER_STATUS_IN_FM(0,i) , WAFER_SOURCE_IN_FM(0,i) );
		}
	}
	//
//	fprintf( fpt , " FAL,    IO_WFR=%4d,%4d,W,S,P=(%4d,%2d,%2d)(%4d,%2d,%2d)\n" , _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 ) , _i_SCH_IO_GET_MODULE_STATUS( 1001 , 0 ) , _i_SCH_MODULE_Get_MFAL_WAFER() , _i_SCH_MODULE_Get_MFAL_SIDE() , _i_SCH_MODULE_Get_MFAL_POINTER() , _i_SCH_MODULE_Get_MFALS_WAFER(2) , _i_SCH_MODULE_Get_MFALS_SIDE(2) , _i_SCH_MODULE_Get_MFALS_POINTER(2) ); // 2015.02.11
	fprintf( fpt , " FAL,    IO_WFR=%4d,%4d,W,S,P=(%4d,%2d,%2d)(%4d,%2d,%2d) ALBM=(%d,%d)\n" , _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 ) , _i_SCH_IO_GET_MODULE_STATUS( 1001 , 0 ) , _i_SCH_MODULE_Get_MFAL_WAFER() , _i_SCH_MODULE_Get_MFAL_SIDE() , _i_SCH_MODULE_Get_MFAL_POINTER() , _i_SCH_MODULE_Get_MFALS_WAFER(2) , _i_SCH_MODULE_Get_MFALS_SIDE(2) , _i_SCH_MODULE_Get_MFALS_POINTER(2) , _i_SCH_MODULE_Get_MFALS_BM(1) , _i_SCH_MODULE_Get_MFALS_BM(2) ); // 2015.02.11
	//
	fprintf( fpt , " FIC,    IO_WFR=%4d,M[%d](W,S,P[TRG/TC])= " , _i_SCH_IO_GET_MODULE_STATUS( F_IC , 1 ) , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) );
	//
	//
	//
	// 2014.11.14
	//
	/*
	for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) { // 2006.10.19 // 2008.01.30
		//
		if ( _i_SCH_MODULE_Get_MFIC_WAFER(i+1) <= 0 ) { // 2011.05.12
			fprintf( fpt , "(-,%d,%d)" , _i_SCH_MODULE_Get_MFIC_SIDE(i+1) , _i_SCH_MODULE_Get_MFIC_POINTER(i+1) );
		}
		else {
			if ( _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i+1) != 0 ) {
				fprintf( fpt , "(%d,%d,%d[%d/%d])" , _i_SCH_MODULE_Get_MFIC_WAFER(i+1) , _i_SCH_MODULE_Get_MFIC_SIDE(i+1) , _i_SCH_MODULE_Get_MFIC_POINTER(i+1) , KPLT_GET_MFIC_TRG_TIMER(i+1) , _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i+1) );
			}
			else {
				fprintf( fpt , "(%d,%d,%d[%d])" , _i_SCH_MODULE_Get_MFIC_WAFER(i+1) , _i_SCH_MODULE_Get_MFIC_SIDE(i+1) , _i_SCH_MODULE_Get_MFIC_POINTER(i+1) , KPLT_GET_MFIC_TRG_TIMER(i+1) );
			}
		}
		//
	}
	fprintf( fpt , "\n" );
	*/
	//
	lg = -1;
	//
	for ( i = (MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE - 1) ; i >= 1 ; i-- ) {
		if (
			( _i_SCH_MODULE_Get_MFIC_WAFER(i) != 0 ) ||
			( _i_SCH_MODULE_Get_MFIC_SIDE(i) != 0 ) ||
			( _i_SCH_MODULE_Get_MFIC_POINTER(i) != 0 ) ||
			( KPLT_GET_MFIC_TRG_TIMER(i) != 0 ) ||
			( _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i) != 0 )
			) {
				lg = i+1;
				break;
		}
	}
	//
	if ( lg == -1 ) lg = 1;
	//
	for ( i = 0 ; i < lg ; i++ ) {
		//
		if ( i == 0 ) {
			if (
				( _i_SCH_MODULE_Get_MFIC_WAFER(i) == 0 ) &&
				( _i_SCH_MODULE_Get_MFIC_SIDE(i) == 0 ) &&
				( _i_SCH_MODULE_Get_MFIC_POINTER(i) == 0 )
				) {
				fprintf( fpt , ">" );
			}
			else {
				fprintf( fpt , "(?%02d,%02d,%02d)" , _i_SCH_MODULE_Get_MFIC_WAFER(i) , _i_SCH_MODULE_Get_MFIC_SIDE(i) , _i_SCH_MODULE_Get_MFIC_POINTER(i) );
			}
		}
		else {
			if (
				( _i_SCH_MODULE_Get_MFIC_WAFER(i) == 0 ) &&
				( _i_SCH_MODULE_Get_MFIC_SIDE(i) == 0 ) &&
				( _i_SCH_MODULE_Get_MFIC_POINTER(i) == 0 ) &&
				( KPLT_GET_MFIC_TRG_TIMER(i) == 0 ) &&
				( _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i) == 0 )
				) {
				fprintf( fpt , "()" );
			}
			else {
				if ( ( KPLT_GET_MFIC_TRG_TIMER(i) == 0 ) && ( _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i) == 0 ) ) {
					//
					if ( _i_SCH_MODULE_Get_MFIC_WAFER(i) == 0 ) {
						fprintf( fpt , "(--,%02d,%02d)" , _i_SCH_MODULE_Get_MFIC_SIDE(i) , _i_SCH_MODULE_Get_MFIC_POINTER(i) );
					}
					else {
						fprintf( fpt , "(%02d,%02d,%02d)" , _i_SCH_MODULE_Get_MFIC_WAFER(i) , _i_SCH_MODULE_Get_MFIC_SIDE(i) , _i_SCH_MODULE_Get_MFIC_POINTER(i) );
					}
					//
				}
				else {
					//
					if ( _i_SCH_MODULE_Get_MFIC_WAFER(i) == 0 ) {
						fprintf( fpt , "(--,%02d,%02d[%d/%d])" , _i_SCH_MODULE_Get_MFIC_SIDE(i) , _i_SCH_MODULE_Get_MFIC_POINTER(i) , KPLT_GET_MFIC_TRG_TIMER(i) , _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i) );
					}
					else {
						fprintf( fpt , "(%02d,%02d,%02d[%d/%d])" , _i_SCH_MODULE_Get_MFIC_WAFER(i) , _i_SCH_MODULE_Get_MFIC_SIDE(i) , _i_SCH_MODULE_Get_MFIC_POINTER(i) , KPLT_GET_MFIC_TRG_TIMER(i) , _i_SCH_MODULE_Get_MFIC_TIMECONTROL(i)  );
					}
					//
				}
			}
		}
		//
	}
	fprintf( fpt , "\n" );
	fprintf( fpt , " ---------------------\n" );
	for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE(j + TM) == 0 ) {
			if ( _i_SCH_MODULE_Get_TM_Switch_Signal(j) == -1 ) {
				if ( _i_SCH_MODULE_Get_TM_WAFER(j,0) == 0 ) {
					if ( _i_SCH_MODULE_Get_TM_WAFER(j,1) == 0 ) {
						if ( _i_SCH_MODULE_Get_TM_SIDE(j,0) == 0 ) {
							if ( _i_SCH_MODULE_Get_TM_SIDE(j,1) == 0 ) {
								if ( _i_SCH_MODULE_Get_TM_POINTER(j,0) == 0 ) {
									if ( _i_SCH_MODULE_Get_TM_POINTER(j,1) == 0 ) {
										if ( _i_SCH_MODULE_Get_TM_SIDE2(j,0) == 0 ) {
											if ( _i_SCH_MODULE_Get_TM_SIDE2(j,1) == 0 ) {
												if ( _i_SCH_MODULE_Get_TM_POINTER2(j,0) == 0 ) {
													if ( _i_SCH_MODULE_Get_TM_POINTER2(j,1) == 0 ) {
														if ( _i_SCH_MODULE_Get_TM_PATHORDER(j,0) == 0 ) {
															if ( _i_SCH_MODULE_Get_TM_PATHORDER(j,1) == 0 ) {
																if ( _i_SCH_MODULE_Get_TM_TYPE(j,0) == 0 ) {
																	if ( _i_SCH_MODULE_Get_TM_TYPE(j,1) == 0 ) {
																		if ( _i_SCH_MODULE_Get_TM_OUTCH(j,0) == 0 ) {
																			if ( _i_SCH_MODULE_Get_TM_OUTCH(j,1) == 0 ) {
																				if ( WAFER_STATUS_IN_TM(j,0) == 0 ) {
																					if ( WAFER_STATUS_IN_TM(j,1) == 0 ) {
																						if ( WAFER_SOURCE_IN_TM(j,0) == 0 ) {
																							if ( WAFER_SOURCE_IN_TM(j,1) == 0 ) {

																								if ( EQUIP_TM_RUNNING_FLAG[j] == 0 ) {
																									if ( EQUIP_TM_ROBOT_FLAG[j] == 0 ) {

																										continue;

																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		//
//		fprintf( fpt , " TM%d[SS=%02d]\n" , j + 1 , _i_SCH_MODULE_Get_TM_Switch_Signal( j ) ); // 2015.06.18
		fprintf( fpt , " TM%d[SS=%02d][SR=%02d] [RUNNING=%d,ROBOT=%d]\n" , j + 1 , _i_SCH_MODULE_Get_TM_Switch_Signal( j ) , _i_SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( j ) , EQUIP_TM_RUNNING_FLAG[j] , EQUIP_TM_ROBOT_FLAG[j] ); // 2015.06.18
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			if ( _i_SCH_MODULE_Get_TM_WAFER(j,i) == 0 ) {
				fprintf( fpt , "   (F%2d)[H%d],WFR=----,SID=%2d,PNT=%2d,SID2=%2d,PNT2=%2d,POR=%2d,TYP=%2d,OUTCH=%2d,IO_WFR=%4d,IO_SRC=%2d\n" , i+1 , _i_SCH_ROBOT_GET_FINGER_HW_USABLE( j , i ) , _i_SCH_MODULE_Get_TM_SIDE(j,i) , _i_SCH_MODULE_Get_TM_POINTER(j,i) , _i_SCH_MODULE_Get_TM_SIDE2(j,i) , _i_SCH_MODULE_Get_TM_POINTER2(j,i) , _i_SCH_MODULE_Get_TM_PATHORDER(j,i) , _i_SCH_MODULE_Get_TM_TYPE(j,i) , _i_SCH_MODULE_Get_TM_OUTCH(j,i) , WAFER_STATUS_IN_TM(j , i) , WAFER_SOURCE_IN_TM(j , i) );
			}
			else {
				fprintf( fpt , "   (F%2d)[H%d],WFR=%4d,SID=%2d,PNT=%2d,SID2=%2d,PNT2=%2d,POR=%2d,TYP=%2d,OUTCH=%2d,IO_WFR=%4d,IO_SRC=%2d\n" , i+1 , _i_SCH_ROBOT_GET_FINGER_HW_USABLE( j , i ) , _i_SCH_MODULE_Get_TM_WAFER(j,i) , _i_SCH_MODULE_Get_TM_SIDE(j,i) , _i_SCH_MODULE_Get_TM_POINTER(j,i) , _i_SCH_MODULE_Get_TM_SIDE2(j,i) , _i_SCH_MODULE_Get_TM_POINTER2(j,i) , _i_SCH_MODULE_Get_TM_PATHORDER(j,i) , _i_SCH_MODULE_Get_TM_TYPE(j,i) , _i_SCH_MODULE_Get_TM_OUTCH(j,i) , WAFER_STATUS_IN_TM(j , i) , WAFER_SOURCE_IN_TM(j , i) );
			}
		}
	}
	fprintf( fpt , " ---------------------\n" );
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		//
		// 2014.10.28
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SLOT_SIZE-1) ; j >= 1 ; j-- ) {
			if (
				( _i_SCH_MODULE_Get_BM_WAFER(i,j) != 0 ) ||
				( _i_SCH_MODULE_Get_BM_SIDE(i,j) != 0 ) ||
				( _i_SCH_MODULE_Get_BM_POINTER(i,j) != 0 ) ||
				( _i_SCH_MODULE_Get_BM_STATUS(i,j) != 0 )
				) {
					lg = j+1;
					break;
			}
		}
		//
		if ( lg == -1 ) {
			if ( _i_SCH_MODULE_Get_BM_FULL_MODE(i) != 0 ) {
				lg = 1;
			}
			if ( _i_SCH_PRM_GET_MODULE_MODE(i) == 1 ) {
				lg = 1;
			}
		}
		//
		if ( lg == -1 ) continue;
		//
		/*
		// 2015.06.18
		switch( _i_SCH_MODULE_Get_BM_FULL_MODE(i) ) {
		case BUFFER_WAIT_STATION :
//			fprintf( fpt , " %s,FULL_MODE=%2d(W)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			fprintf( fpt , " %s,FULL_MODE=%2d(W) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			break;
		case BUFFER_FM_STATION :
//			fprintf( fpt , " %s,FULL_MODE=%2d(F)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			fprintf( fpt , " %s,FULL_MODE=%2d(F) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			break;
		case BUFFER_TM_STATION :
//			fprintf( fpt , " %s,FULL_MODE=%2d(T)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			fprintf( fpt , " %s,FULL_MODE=%2d(T) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			break;
		default :
//			fprintf( fpt , " %s,FULL_MODE=%2d(?)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			fprintf( fpt , " %s,FULL_MODE=%2d(?) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
			break;
		}
		*/
		//
		// 2015.06.18
		if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) ) {
			switch( _i_SCH_MODULE_Get_BM_FULL_MODE(i) ) {
			case BUFFER_WAIT_STATION :
				fprintf( fpt , " %s,F:%d:%d:%d MODE=%2d(W) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  									
			case BUFFER_FM_STATION :																																																				  									
				fprintf( fpt , " %s,F:%d:%d:%d MODE=%2d(F) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  
			case BUFFER_TM_STATION :																																																				  
				fprintf( fpt , " %s,F:%d:%d:%d MODE=%2d(T) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  
			default :																																																								  
				fprintf( fpt , " %s,F:%d:%d:%d MODE=%2d(?) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  
			}																																																										  
		}																																																											  
		else {																																																										  
			switch( _i_SCH_MODULE_Get_BM_FULL_MODE(i) ) {																																															  
			case BUFFER_WAIT_STATION :																																																				  
				fprintf( fpt , " %s,D:%d:%d:%d MODE=%2d(W) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  
			case BUFFER_FM_STATION :																																																				  
				fprintf( fpt , " %s,D:%d:%d:%d MODE=%2d(F) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  
			case BUFFER_TM_STATION :																																																				  
				fprintf( fpt , " %s,D:%d:%d:%d MODE=%2d(T) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;																																																								  
			default :																																																								  
				fprintf( fpt , " %s,D:%d:%d:%d MODE=%2d(?) [" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i) , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i) , _i_SCH_MODULE_Get_BM_FULL_MODE(i) ); // 2012.08.08
				break;
			}
		}
		//
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) fprintf( fpt , "(%d:%d)" , j , _i_SCH_PRM_GET_MODULE_BUFFER_MODE( j , i ) ); // 2012.08.08
		fprintf( fpt , "] " ); // 2012.08.08
		//
		fprintf( fpt , "WFR,SID,PNT,STS= " );
		//
//		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) { 2014.10.28
		for ( j = 0 ; j < lg ; j++ ) { // 2014.10.28
			if ( j == 0 ) {
				if (
					( _i_SCH_MODULE_Get_BM_WAFER(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_BM_SIDE(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_BM_POINTER(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_BM_STATUS(i,j) == 0 )
					) {
					fprintf( fpt , ">" );
				}
				else {
					fprintf( fpt , "(?%02d,%02d,%02d,%02d)" , _i_SCH_MODULE_Get_BM_WAFER(i,j) , _i_SCH_MODULE_Get_BM_SIDE(i,j) , _i_SCH_MODULE_Get_BM_POINTER(i,j) , _i_SCH_MODULE_Get_BM_STATUS(i,j) );
				}
			}
			else {
				if (
					( _i_SCH_MODULE_Get_BM_WAFER(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_BM_SIDE(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_BM_POINTER(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_BM_STATUS(i,j) == 0 )
					) {
					fprintf( fpt , "()" );
				}
				else {
					if ( _i_SCH_MODULE_Get_BM_WAFER(i,j) == 0 ) {
						fprintf( fpt , "(--,%02d,%02d," , _i_SCH_MODULE_Get_BM_SIDE(i,j) , _i_SCH_MODULE_Get_BM_POINTER(i,j) );
					}
					else {
						fprintf( fpt , "(%02d,%02d,%02d," , _i_SCH_MODULE_Get_BM_WAFER(i,j) , _i_SCH_MODULE_Get_BM_SIDE(i,j) , _i_SCH_MODULE_Get_BM_POINTER(i,j) );
					}
					switch( _i_SCH_MODULE_Get_BM_STATUS(i,j) ) {
					case BUFFER_READY_STATUS :		fprintf( fpt , "0R)" ); break;
					case BUFFER_INWAIT_STATUS :		fprintf( fpt , "1I)" ); break;
					case BUFFER_OUTWAIT_STATUS :	fprintf( fpt , "2O)" ); break;
					default						:	fprintf( fpt , "%02d)" , _i_SCH_MODULE_Get_BM_STATUS(i,j) ); break;
					}
				}
			}
		}
		fprintf( fpt , "\n" );
	}
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		//
		// 2014.10.28
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SLOT_SIZE-1) ; j >= 0 ; j-- ) {
			if (
				( _i_SCH_IO_GET_MODULE_STATUS(i,j+1) != 0 ) ||
				( _i_SCH_IO_GET_MODULE_SOURCE(i,j+1) != 0 )
				) {
					lg = j+1;
					break;
			}
		}
		//
		if ( lg == -1 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE(i) == 1 ) {
				lg = 1;
			}
		}
		//
		if ( lg == -1 ) continue;
		//
		fprintf( fpt , " %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
		//
		fprintf( fpt , "	" );
		fprintf( fpt , "IO_WFR=(" );
		for ( j = 0 ; j < lg ; j++ ) {
			if ( j == 0 ) {
				fprintf( fpt , "%2d" , _i_SCH_IO_GET_MODULE_STATUS(i,j+1) );
			}
			else {
				fprintf( fpt , ",%2d" , _i_SCH_IO_GET_MODULE_STATUS(i,j+1) );
			}
		}
		fprintf( fpt , ")\n" );
		//
		fprintf( fpt , "	" );
		fprintf( fpt , "IO_SRC=(" );
		for ( j = 0 ; j < lg ; j++ ) {
			if ( j == 0 ) {
				fprintf( fpt , "%2d" , _i_SCH_IO_GET_MODULE_SOURCE(i,j+1) );
			}
			else {
				fprintf( fpt , ",%2d" , _i_SCH_IO_GET_MODULE_SOURCE(i,j+1) );
			}
		}
		fprintf( fpt , ")\n" );
		//
		if ( EXTEND_SOURCE_USE != 0 ) { // 2018.09.05
			//
			fprintf( fpt , "	" );
			fprintf( fpt , "IOSRCE=(" );
			for ( j = 0 ; j < lg ; j++ ) {
				if ( j == 0 ) {
					fprintf( fpt , "%2d" , _i_SCH_IO_GET_MODULE_SOURCE_E(i,j+1) );
				}
				else {
					fprintf( fpt , ",%2d" , _i_SCH_IO_GET_MODULE_SOURCE_E(i,j+1) );
				}
			}
			fprintf( fpt , ")\n" );
			//
		}
		//
	}
	fprintf( fpt , " ---------------------\n" );
	/*
	// 2013.08.02
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		fprintf( fpt , " %4s," , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
		//
		fprintf( fpt , "WFR=" );	for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ )	fprintf( fpt , "%4d," ,	_i_SCH_MODULE_Get_PM_WAFER(i,j) );
		//
		fprintf( fpt , " SIDE=" );	for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_MODULE_Get_PM_SIDE(i,j) );
		//
		fprintf( fpt , " POINT=" );	for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_MODULE_Get_PM_POINTER(i,j) );
		//
		fprintf( fpt , " STS=" );	for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_MODULE_Get_PM_STATUS(i,j) );
		//
		fprintf( fpt , " IO_SRC=" );	for ( j = 0 ; j < (MAX_PM_SLOT_DEPTH-1) ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_IO_GET_MODULE_SOURCE(i,j+1) );
		//
		fprintf( fpt , " IO_WFR=" );	for ( j = 0 ; j < (MAX_PM_SLOT_DEPTH-1) ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_IO_GET_MODULE_STATUS(i,j+1) );
		//
		fprintf( fpt , " AbortWaitflag=%2d,MacroFlag=%d\n" , SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag(i) , SCH_MACRO_PROCESS_STS[i] );
		//
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) > 0 ) { // 2008.08.12
			fprintf( fpt , " \tINF=%4s\n" ,	_i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) ) );

			for ( j = 0 ; j < MAX_INTERFACE_COUNT ; j++ ) {
				PROCESS_INTERFACE_MODULE_GET_INFO( i , j , &k , &m , &l , &pmc );
				if ( k >= 0 ) {
					fprintf( fpt , " \t\t[%2d]\tM=%2d,W=%2d,S=%2d,P=%2d\n" , j , m , pmc , k , l );
				}
			}
		}
		//
	}
	*/
	// 2013.08.02
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		//
		// 2014.10.28
		//
		lg = -1;
		//
		for ( j = (MAX_PM_SLOT_DEPTH-1) ; j >= 0 ; j-- ) {
			if (
				( _i_SCH_MODULE_Get_PM_WAFER(i,j) != 0 ) ||
				( _i_SCH_MODULE_Get_PM_SIDE(i,j) != 0 ) ||
				( _i_SCH_MODULE_Get_PM_POINTER(i,j) != 0 ) ||
				( _i_SCH_MODULE_Get_PM_STATUS(i,j) != 0 )
				) {
					lg = j+1;
					break;
			}
		}
		//
		if ( lg == -1 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE(i) == 1 ) {
				lg = 1;
			}
		}
		//
		if ( lg != -1 ) {
			//
			fprintf( fpt , " %4s WFR,SID,PNT,STS,LCK= " , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
			//
			for ( j = 0 ; j < lg ; j++ )	{
				if (
					( _i_SCH_MODULE_Get_PM_WAFER(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_PM_SIDE(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_PM_POINTER(i,j) == 0 ) &&
					( _i_SCH_MODULE_Get_PM_STATUS(i,j) == 0 ) ) {
						fprintf( fpt , "()" );
				}
				else {
					if ( _i_SCH_MODULE_Get_PM_WAFER(i,j) == 0 ) {
						fprintf( fpt , "(--,%02d,%02d,%02d,%02d)" , _i_SCH_MODULE_Get_PM_SIDE(i,j)  , _i_SCH_MODULE_Get_PM_POINTER(i,j)  , _i_SCH_MODULE_Get_PM_STATUS(i,j) , _i_SCH_MODULE_Get_PM_PICKLOCK(i,j) );
					}
					else {
						fprintf( fpt , "(%02d,%02d,%02d,%02d,%02d)" , _i_SCH_MODULE_Get_PM_WAFER(i,j)  , _i_SCH_MODULE_Get_PM_SIDE(i,j)  , _i_SCH_MODULE_Get_PM_POINTER(i,j)  , _i_SCH_MODULE_Get_PM_STATUS(i,j) , _i_SCH_MODULE_Get_PM_PICKLOCK(i,j) );
					}
				}
			}
			fprintf( fpt , "\n" );
			//
		}
	}	
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		//
		//
		// 2014.10.28
		//
		lg = -1;
		//
		for ( j = (MAX_PM_SLOT_DEPTH-2) ; j >= 0 ; j-- ) {
			if (
				( _i_SCH_IO_GET_MODULE_STATUS(i,j+1) != 0 ) ||
				( _i_SCH_IO_GET_MODULE_SOURCE(i,j+1) != 0 )
				) {
					lg = j+1;
					break;
			}
		}
		//
		if ( lg == -1 ) {
			if ( ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag(i) != 0 ) || ( SCH_MACRO_PROCESS_STS[i] != 0 ) ) {
				lg = 1;
			}
			if ( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) > 0 ) {
				lg = 1;
			}
			if ( _i_SCH_PRM_GET_MODULE_MODE(i) == 1 ) {
				lg = 1;
			}
		}
		//
		if ( lg == -1 ) continue;
		//
		fprintf( fpt , " %s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
		//
		fprintf( fpt , "	" );
		fprintf( fpt , "IO_WFR=(" );	for ( j = 0 ; j < lg ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_IO_GET_MODULE_STATUS(i,j+1) );	fprintf( fpt , ")\n" );
		//
		fprintf( fpt , "	" );
		fprintf( fpt , "IO_SRC=(" );	for ( j = 0 ; j < lg ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_IO_GET_MODULE_SOURCE(i,j+1) );	fprintf( fpt , ")" );
		//
		//
		if ( EXTEND_SOURCE_USE != 0 ) { // 2018.09.05
			fprintf( fpt , "	" );
			fprintf( fpt , "IOSRCE=(" );	for ( j = 0 ; j < lg ; j++ )	fprintf( fpt , "%2d," ,	_i_SCH_IO_GET_MODULE_SOURCE_E(i,j+1) );	fprintf( fpt , ")" );
		}
		//
		//
		if ( ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag(i) == 0 ) && ( SCH_MACRO_PROCESS_STS[i] == 0 ) ) {
			fprintf( fpt , "\n" );
		}
		else {
			fprintf( fpt , " AbortWaitflag=%2d,MacroFlag=%d\n" , SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag(i) , SCH_MACRO_PROCESS_STS[i] );
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) > 0 ) { // 2008.08.12
			//
			fprintf( fpt , " \tINF=%4s [%d] TIME(MS)=%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) ) , PM_INTERFACE_MONITOR_COUNT[i] , PROCESS_INTERFACE_MODULE_GET_TIME( i ) );

			for ( j = 0 ; j < MAX_INTERFACE_COUNT ; j++ ) {
				//
				if ( PROCESS_INTERFACE_MODULE_GET_INFO( i , j , &k , &m , &l , &pmc ) ) {
					//
					fprintf( fpt , " \t\t[%2d]\tM=%2d,W=%2d,S=%2d,P=%2d\n" , j , m , pmc , k , l );
					//
				}
			}
		}
		//
	}
	//
	// 2017.09.22
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( STRCMP_L( _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST( i ) , "" ) ) {
			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( i ) == 0 ) {
				continue;
			}
		}
		//
		fprintf( fpt , " %s PRE_LAST = [%d] %s (%d)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST(i) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST(i) , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST(i) );
		//
	}
	//
	// 2017.10.16
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( STRCMP_L( _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_OLD_LAST( i ) , "" ) ) {
			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_OLD_LAST( i ) == 0 ) {
				continue;
			}
		}
		//
		fprintf( fpt , " %s PRE_OLD_LAST = [%d] %s (%d)\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_OLD_LAST(i) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_OLD_LAST(i) , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_OLD_LAST(i) );
		//
	}
	//
	fprintf( fpt , " ---------------------\n" );

	for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
		//
		fprintf( fpt , " %5s_STS :" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SLOT_SIZE-1) ; j >= 0 ; j-- ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( i , j+1 ) != 0 ) {
				lg = j+1;
				break;
			}
		}
		//
		for ( j = 0 ; j < lg ; j++ ) { // 2014.10.28
			fprintf( fpt , " %2d" , _i_SCH_IO_GET_MODULE_STATUS( i , j+1 ) );
		}	
		fprintf( fpt , "\n" );
	}

	fprintf( fpt , " ---------------------\n" );

	for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
		fprintf( fpt , " %5s_RES :" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SLOT_SIZE-1) ; j >= 0 ; j-- ) {
			if ( _i_SCH_CASSETTE_LAST_RESULT_GET( i , j+1 ) != -1 ) {
				lg = j+1;
				break;
			}
		}
		//
		for ( j = 0 ; j < lg ; j++ ) {
			fprintf( fpt , " %2d" , _i_SCH_CASSETTE_LAST_RESULT_GET( i , j+1 ) );
		}	
		fprintf( fpt , "\n" );
	}
	//

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		fprintf( fpt , " LOT%2d_RES :" , i + 1 );
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SLOT_SIZE-1) ; j >= 0 ; j-- ) {
			if ( _i_SCH_CASSETTE_LAST_RESULT2_GET( i , j ) != -1 ) {
				lg = j+1;
				break;
			}
		}
		//
		for ( j = 0 ; j < lg ; j++ ) {
			fprintf( fpt , " %2d" , _i_SCH_CASSETTE_LAST_RESULT2_GET( i , j ) );
		}	
		fprintf( fpt , "\n" );
	}
	//

	fprintf( fpt , "\n<ETC DATA AREA>\n" );

	fprintf( fpt , " -All Mdl Run Flag--------------------\n\t" );
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag(i) == 0 ) {
			fprintf( fpt , "[ ]" );
		}
		else {
			fprintf( fpt , "[%d]" , _i_SCH_MODULE_Get_Mdl_Run_Flag(i) );
		}
	}

	fprintf( fpt , "\n" );

	fprintf( fpt , " -All Last Prcs Run Flag--[Last_CPName Side:Status,<MSG_READYEND>]------\n\t" );

	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = (PM1 + MAX_PM_CHAMBER_DEPTH-1) ; i >= PM1 ; i-- ) {
		if ( PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( i ) != NULL ) {
			lg = i+1;
			break;
		}
		if ( PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) != -1 ) {
			lg = i+1;
			break;
		}
		if ( MSG_READYEND[i] != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	for ( i = PM1 ; i < lg ; i++ ) {
		if ( MSG_READYEND[i] == 0 ) {
			if ( PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( i ) == NULL ) {
				if ( PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) == -1 ) {
					fprintf( fpt , "[]" );
				}
				else {
					fprintf( fpt , "[%d:NULL,<%d>]" , PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) , MSG_READYEND[i] );
				}
			}
			else {
				fprintf( fpt , "[%d:%s,<%d>]" , PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) , PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( i ) , MSG_READYEND[i] );
			}
		}
		else {
			if ( PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( i ) == NULL ) {
				if ( PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) == -1 ) {
					fprintf( fpt , "[-,<%d>]" , MSG_READYEND[i] );
				}
				else {
					fprintf( fpt , "[%d:NULL,<%d>]" , PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) , MSG_READYEND[i] );
				}
			}
			else {
				fprintf( fpt , "[%d:%s,<%d>]" , PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( i ) , PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( i ) , MSG_READYEND[i] );
			}
		}
	}

	fprintf( fpt , "\n" );

	fprintf( fpt , " -PROCESS Last Status for Ordering--(MOVE,CHECK_SKIP_STS)--\n" ); // 2012.11.09

	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = (PM1 + MAX_PM_CHAMBER_DEPTH-1) ; i >= PM1 ; i-- ) {
		if ( SCH_MACRO_PROCESS_MOVE[ i ] != 0 ) {
			lg = i+1;
			break;
		}
		if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[ i ] != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	for ( i = PM1 ; i < lg ; i++ ) {
		if ( i == PM1 ) fprintf( fpt , "\tPM:" );
		//
		if ( ( SCH_MACRO_PROCESS_MOVE[i] == 0 ) && ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] == 0 ) ) {
			fprintf( fpt , "(-)" );
		}
		else {
			fprintf( fpt , "(%d:%d)" , SCH_MACRO_PROCESS_MOVE[i] , SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] );
		}
	}
	//
	if ( lg != -1 ) fprintf( fpt , "\n" );
	//
	lg = -1;
	//
	for ( i = (TM-1) ; i >= BM1 ; i-- ) {
		if ( SCH_MACRO_PROCESS_MOVE[ i ] != 0 ) {
			lg = i+1;
			break;
		}
		if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[ i ] != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	for ( i = BM1 ; i < lg ; i++ ) {
		if ( i == BM1 ) fprintf( fpt , "\tBM:" );
		//
		if ( ( SCH_MACRO_PROCESS_MOVE[i] == 0 ) && ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] == 0 ) ) {
			fprintf( fpt , "(-)" );
		}
		else {
			fprintf( fpt , "(%d:%d)" , SCH_MACRO_PROCESS_MOVE[i] , SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] );
		}
	}
	//
	fprintf( fpt , "\n" );
	//

	fprintf( fpt , " -All Mdl Equip Flag--------------------\n" );

	for ( i = CM1 ; i <= FM ; i++ ) {
		if ( i == CM1 ) fprintf( fpt , "\t" );
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		switch( EQUIP_READ_FUNCTION_FOR_LOG( i ) ) {
		case -1				:	fprintf( fpt , "[ ]" );			break;
		case SYS_ERROR		:	fprintf( fpt , "[E]" );			break;
		case SYS_ABORTED	:	fprintf( fpt , "[A]" );			break;
		case SYS_RUNNING	:	fprintf( fpt , "[R]" );			break;
		case SYS_SUCCESS	:	fprintf( fpt , "[S]" );			break;
		default				:	fprintf( fpt , "[%d]" , EQUIP_READ_FUNCTION_FOR_LOG( i ) );		break;
		}
	}
	fprintf( fpt , "\n\n" );

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		fprintf( fpt , "\t" );
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (AL-1) ; j >= PM1 ; j-- ) {
			if ( EQUIP_STATUS_PROCESS_FOR_LOG( i , j ) != -1 ) {
				lg = j+1;
				break;
			}
		}
		//
		for ( j = PM1 ; j < lg ; j++ ) {
			switch( EQUIP_STATUS_PROCESS_FOR_LOG( i , j ) ) {
			case -1				:	fprintf( fpt , "[ ]" );			break;
			case SYS_ERROR		:	fprintf( fpt , "[E]" );			break;
			case SYS_ABORTED	:	fprintf( fpt , "[A]" );			break;
			case SYS_RUNNING	:	fprintf( fpt , "[R]" );			break;
			case SYS_SUCCESS	:	fprintf( fpt , "[S]" );			break;
			default				:	fprintf( fpt , "[%d]" , EQUIP_STATUS_PROCESS_FOR_LOG( i , j ) );		break;
			}
		}
		fprintf( fpt , "\n" );
	}

	fprintf( fpt , " -TIMER-[ELAPSED(START),ELAPSED(END):RUNPLAN,REMAIN]-\n" );

	fprintf( fpt , "\tMAIN " );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = FM ; i >= CM1 ; i-- ) {
		if ( ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 0 , i , TRUE ) != 0 ) || ( _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , i ) != 0 ) ) {
			lg = i;
			break;
		}
	}
	//
	for ( i = CM1 ; i <= lg ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		if ( ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 0 , i , TRUE ) == 0 ) && ( _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , i ) == 0 ) ) {
			fprintf( fpt , "[-]" );
		}
		else {
			if ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 0 , i , TRUE ) == 0 ) {
				fprintf( fpt , "[-,%d:%d,-]" ,
					_i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( 0 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , i )
					);
			}
			else {
				fprintf( fpt , "[%d,%d:%d,%d]" ,
					_i_SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( 0 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , i )
					);
			}
		}
	}
	fprintf( fpt , "\n" );
	//
	fprintf( fpt , "\tPOST " );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = FM ; i >= CM1 ; i-- ) {
		if ( ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 1 , i , TRUE ) != 0 ) || ( _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , i ) != 0 ) ) {
			lg = i;
			break;
		}
	}
	//
	for ( i = CM1 ; i <= lg ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		if ( ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 1 , i , TRUE ) == 0 ) && ( _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , i ) == 0 ) ) {
			fprintf( fpt , "[-]" );
		}
		else {
			if ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 1 , i , TRUE ) == 0 ) {
				fprintf( fpt , "[-,%d:%d,-]" ,
					_i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( 1 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , i )
					);
			}
			else {
				fprintf( fpt , "[%d,%d:%d,%d]" ,
					_i_SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 1 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( 1 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , i )
					);
			}
		}
	}
	fprintf( fpt , "\n" );
	//
	fprintf( fpt , "\tPRE  " );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = FM ; i >= CM1 ; i-- ) {
		if ( ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 2 , i , TRUE ) != 0 ) || ( _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 2 , i ) != 0 ) ) {
			lg = i;
			break;
		}
	}
	//
	for ( i = CM1 ; i <= lg ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		if ( ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 2 , i , TRUE ) == 0 ) && ( _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 2 , i ) == 0 ) ) {
			fprintf( fpt , "[-]" );
		}
		else {
			if ( _i_SCH_TIMER_GET_PROCESS_TIME_START( 2 , i , TRUE ) == 0 ) {
				fprintf( fpt , "[-,%d:%d,-]" ,
					_i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( 2 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 2 , i )
					);
			}
			else {
				fprintf( fpt , "[%d,%d:%d,%d]" ,
					_i_SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 2 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( 2 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 2 , i ) ,
					_i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , i )
					);
			}
		}
	}
	fprintf( fpt , "\n" );
	//
	fprintf( fpt , " -SDM--------------------\n" );

	fprintf( fpt , "\tSDM_EVENT_INCREASE = %d\n" , SDM_EVENT_INCREASE ); // 2016.11.10
	//
	fprintf( fpt , "\tFM_WFR_SUPPLY = (%d,%d)\n" , _i_SCH_MODULE_Get_FM_MODE(0) , _i_SCH_MODULE_Get_FM_MODE(1) );
	
	fprintf( fpt , "\tRUN_FM_LAST_SELECTED_PM = %d\n" , _i_SCH_ONESELECT_Get_LastSelectPM() ); // 2006.03.10

	fprintf( fpt , "\tLOTFIRST_CHAMBER " );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = (MAX_CHAMBER-1) ; i >= 0 ; i-- ) {
		if ( _i_SCH_SDM_Get_LOTFIRST_OPTION( i ) != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	for ( i = 0 ; i < lg ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		if ( _i_SCH_SDM_Get_LOTFIRST_OPTION(i) == 0 ) {
			fprintf( fpt , "[-]" );
		}
		else {
			fprintf( fpt , "[%d]" , _i_SCH_SDM_Get_LOTFIRST_OPTION(i) );
		}
	}
	fprintf( fpt , "\n" );
	//
	fprintf( fpt , "\tLOTEND_CHAMBER   " );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = (MAX_CHAMBER-1) ; i >= 0 ; i-- ) {
		if ( _i_SCH_SDM_Get_LOTEND_OPTION( i ) != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	for ( i = 0 ; i < lg ; i++ ) {
		if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
		if ( _i_SCH_SDM_Get_LOTEND_OPTION(i) == 0 ) {
			fprintf( fpt , "[-]" );
		}
		else {
			fprintf( fpt , "[%d]" , _i_SCH_SDM_Get_LOTEND_OPTION(i) );
		}
	}
	fprintf( fpt , "\n" );

	fprintf( fpt , "\tRUN_CHAMBER      " );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = (MAX_CASSETTE_SLOT_SIZE-1) ; i >= 0 ; i-- ) {
		if ( _i_SCH_SDM_Get_RUN_CHAMBER( i ) != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	for ( i = 0 ; i < lg ; i++ ) {
		if ( _i_SCH_SDM_Get_RUN_CHAMBER(i) == 0 ) {
			fprintf( fpt , "[]" );
		}
		else {
			fprintf( fpt , "[%02d]" , _i_SCH_SDM_Get_RUN_CHAMBER(i) );
		}
	}
	fprintf( fpt , "\n" );


	/*
	// 2014.10.28
	fprintf( fpt , " ------------------------------------------------------------------\n" ); // 2007.10.30
	fprintf( fpt , " FORCE_MAKE_FMSIDE " );
	j = _i_SCH_SUB_Get_FORCE_FMSIDE_DATA( &i );
	fprintf( fpt , "[TAG=%d,DATA=%d]\n" , i , j );
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	*/
	//
	// 2014.10.28
	j = _i_SCH_SUB_Get_FORCE_FMSIDE_DATA( &i );
	//
	if ( ( i != 0 ) || ( j != 0 ) ) {
		fprintf( fpt , " ------------------------------------------------------------------\n" ); // 2007.10.30
		fprintf( fpt , " FORCE_MAKE_FMSIDE " );
		fprintf( fpt , "[TAG=%d,DATA=%d]\n" , i , j );
		fprintf( fpt , " ------------------------------------------------------------------\n" );
	}
	//
//	fprintf( fpt , " PM_INTERFACE_MONITOR\n" ); // 2014.10.28
	//
	//
	//
	/*
	//
	// 2018.06.05
	//
	lg = 0; // 2014.10.28
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		if ( PM_INTERFACE_MONITOR_COUNT[i] <= 0 ) continue;
		//
		if ( lg == 0 ) {
			lg = 1;
			fprintf( fpt , " PM_INTERFACE_MONITOR[SPWM]\n" );
		}
		//
		fprintf( fpt , "   %s:%d - " , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , PM_INTERFACE_MONITOR_COUNT[i] );
		//
		for ( j = 0 ; j < MAX_INTERFACE_COUNT ; j++ ) {
			//
			if ( PM_INTERFACE_MONITOR_SIDE[i][j] < 0 ) continue;
			//
			fprintf( fpt , "(%d:%d:%d:%d:%d)" , j , PM_INTERFACE_MONITOR_SIDE[i][j] , PM_INTERFACE_MONITOR_POINTER[i][j] , PM_INTERFACE_MONITOR_WAFER[i][j] , PM_INTERFACE_MONITOR_MODE[i][j] );
			//
		}
		//
		fprintf( fpt , "\n" );
	}
	//
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	//
	*/
	//
	//
	if ( _i_SCH_PRM_GET_DFIX_IOINTLKS_USE() ) { // 2009.04.24
		fprintf( fpt , " ------------------------------------------------------------------\n" );
		fprintf( fpt , " IOINTLKS_USE\n" );
		fprintf( fpt , "    PICK : " );
		//
		if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( FM , 9999 ) ) fprintf( fpt , "FM " );
		//
		for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( i , 9999 ) ) fprintf( fpt , "CM%d " , i - CM1 + 1 );
		}
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( i , 9999 ) ) fprintf( fpt , "PM%d " , i - PM1 + 1 );
		}
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( i , 9999 ) ) fprintf( fpt , "BM%d " , i - BM1 + 1 );
		}
		for ( i = TM ; i < ( TM + MAX_TM_CHAMBER_DEPTH ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PICK_POSSIBLE( i , 9999 ) ) fprintf( fpt , "TM%d " , i - TM + 1 );
		}
		//
		fprintf( fpt , "\n    PLACE : " );
		//
		if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( FM , -1 ) ) fprintf( fpt , "FM " );
		//
		for ( i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( i , 9999 ) ) fprintf( fpt , "CM%d " , i - CM1 + 1 );
		}
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( i , 9999 ) ) fprintf( fpt , "PM%d " , i - PM1 + 1 );
		}
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( i , 9999 ) ) fprintf( fpt , "BM%d " , i - BM1 + 1 );
		}
		for ( i = TM ; i < ( TM + MAX_TM_CHAMBER_DEPTH ) ; i++ ) {
			if ( !_SCH_IO_GET_INLTKS_FOR_PLACE_POSSIBLE( i , 9999 ) ) fprintf( fpt , "TM%d " , i - TM + 1 );
		}
		fprintf( fpt , "\n ------------------------------------------------------------------\n\n" );
	}
	else {
		fprintf( fpt , "\n" );
	}
	//
	fprintf( fpt , " ------------------------------------------------------------------\n" );
	//
	fprintf( fpt , " PRE_PRCS\n" );
	//
	//
	// 2014.11.14
	//
	lg = -1;
	//
	for ( i = (MAX_CHAMBER-1) ; i >= 0 ; i-- ) {
		if ( Global_Chamber_Pre_NotUse_Flag[i] != 0 ) {
			lg = i+1;
			break;
		}
	}
	//
	if ( lg != -1 ) {
		//
		fprintf( fpt , "  - Pre_NotUse            : " );
		//
		for ( i = 0 ; i < lg ; i++ ) { // 2014.11.14
			//
			if ( ( i == PM1 ) || ( i == AL ) || ( i == BM1 ) || ( i == TM ) || ( i == FM ) ) fprintf( fpt , " " );
			//
			if ( Global_Chamber_Pre_NotUse_Flag[i] == 0 ) { // 2014.11.14
				fprintf( fpt , " " );
			}
			else {
				fprintf( fpt , "%d" , Global_Chamber_Pre_NotUse_Flag[i] );
			}
		}
		//
		fprintf( fpt , "\n" );
		//
	}
	//
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CHAMBER-1) ; j >= 0 ; j-- ) {
			if ( Global_Chamber_FirstPre_Use_Flag[i][j] != 0 ) {
				lg = j+1;
				break;
			}
		}
		//
		if ( lg != -1 ) {
			//
			fprintf( fpt , "  - FirstPre_Use     [%2d] : " , i );
			//
			for ( j = 0 ; j < lg ; j++ ) { // 2014.11.14
				if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) fprintf( fpt , " " );
				//
				if ( Global_Chamber_FirstPre_Use_Flag[i][j] == 0 ) { // 2014.11.14
					fprintf( fpt , "-" );
				}
				else {
					fprintf( fpt , "%d" , Global_Chamber_FirstPre_Use_Flag[i][j] );
				}
			}
			//
			fprintf( fpt , "\n" );
			//
		}
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CHAMBER-1) ; j >= 0 ; j-- ) {
			if ( Global_Chamber_FirstPre_Done_Flag[i][j] != 0 ) {
				lg = j+1;
				break;
			}
		}
		//
		if ( lg != -1 ) {
			//
			fprintf( fpt , "  - FirstPre_Done    [%2d] : " , i );
			//
			for ( j = 0 ; j < lg ; j++ ) { // 2014.11.14
				if ( ( j == PM1 ) || ( j == AL ) || ( j == BM1 ) || ( j == TM ) || ( j == FM ) ) fprintf( fpt , " " );
				//
				if ( Global_Chamber_FirstPre_Done_Flag[i][j] == 0 ) { // 2014.11.14
					fprintf( fpt , "-" );
				}
				else {
					fprintf( fpt , "%d" , Global_Chamber_FirstPre_Done_Flag[i][j] );
				}
			}
			//
			fprintf( fpt , "\n" );
		}
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SIDE-1) ; j >= 0 ; j-- ) {
			if ( Global_Chamber_FirstPre_NotUse_Flag[i][j] != 0 ) {
				lg = j+1;
				break;
			}
		}
		//
		if ( lg != -1 ) {
			//
			fprintf( fpt , "  - FirstPre_NotUse  [%2d] : " , i );
			//
			for ( j = 0 ; j < lg ; j++ ) { // 2014.11.14
				//
				if ( Global_Chamber_FirstPre_NotUse_Flag[i][j] == 0 ) { // 2014.11.14
					fprintf( fpt , "-" );
				}
				else {
					fprintf( fpt , "%d" , Global_Chamber_FirstPre_NotUse_Flag[i][j] );
				}
			}
			//
			fprintf( fpt , "\n" );
		}
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		// 2014.11.14
		//
		lg = -1;
		//
		for ( j = (MAX_CASSETTE_SIDE-1) ; j >= 0 ; j-- ) {
			if ( Global_Chamber_FirstPre_NotUse2_Flag[i][j] != 0 ) {
				lg = j+1;
				break;
			}
		}
		//
		if ( lg != -1 ) {
			//
			fprintf( fpt , "  - FirstPre_NotUse2 [%2d] : " , i );
			//
			for ( j = 0 ; j < lg ; j++ ) { // 2014.11.14
				//
				if ( Global_Chamber_FirstPre_NotUse2_Flag[i][j] == 0 ) { // 2014.11.14
					fprintf( fpt , "-" );
				}
				else {
					fprintf( fpt , "%d" , Global_Chamber_FirstPre_NotUse2_Flag[i][j] );
				}
			}
			fprintf( fpt , "\n" );
			//
		}
	}
	//
	// 2016.03.18
	//
	//
	lg = 0;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		if ( _TM_PICKPLACING_CHAMBER[ i ] != 0 ) {
			//
			if ( lg == 0 ) {
				//
				lg = 1;
				//
				fprintf( fpt , "  - TM_PICKPLACING : [MAX_TM=%d][MAX_ARM=%d]\n" , _TM_WILLPLACING_MAX_TM , _TM_WILLPLACING_MAX_ARM );
				//
			//
			}
			//
			fprintf( fpt , "    _TM_PICKPLACING_CHAMBER : [CH=%d] [%d]\n" , i , _TM_PICKPLACING_CHAMBER[i] );
			//
		}
		//
		for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
			//
			for ( k = 0 ; k < MAX_FINGER_TM ; k++ ) {
				//
				if ( _TM_WILLPLACING_CHAMBER[j][k][ i ] != 0 ) {
					//
					if ( lg == 0 ) {
						//
						lg = 1;
						//
						fprintf( fpt , "  - TM_PICKPLACING : [MAX_TM=%d][MAX_ARM=%d]\n" , _TM_WILLPLACING_MAX_TM , _TM_WILLPLACING_MAX_ARM );
						//
					//
					}
					//
					fprintf( fpt , "    _TM_WILLPLACING_CHAMBER : [TM=%d][ARM=%d][CH=%d] [%d]\n" , j , k , i , _TM_WILLPLACING_CHAMBER[j][k][ i ] );
					//
				}
				//
			}
		}
	}
	//
	//
	//
	// 2016.12.09
	//
	//
	lg = 0;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		if ( ( _i_SCH_PRM_GET_SIM_TIME1( i ) > 0 ) || ( _i_SCH_PRM_GET_SIM_TIME2( i ) > 0 ) || ( _i_SCH_PRM_GET_SIM_TIME3( i ) > 0 ) ) {
			//
			if ( lg == 0 ) {
				//
				lg = 1;
				//
				fprintf( fpt , "  - SIM_TIME :\n" );
				//
			//
			}
			//
			fprintf( fpt , "    [%4s] : %8d , %8d , %8d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_SIM_TIME1( i ) , _i_SCH_PRM_GET_SIM_TIME2( i ) , _i_SCH_PRM_GET_SIM_TIME3( i ) );
			//
		}
		//
	}
	//
	// 2017.04.17
	//
	//
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		lg = 0;
		//
		for ( j = 1 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( !_i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue;
			//
			if ( lg == 0 ) fprintf( fpt , "SLOT_NOTUSE = [%s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
			//
			fprintf( fpt , "(%d)" , j );
			//
			lg = 1;
			//
		}
		//
		if ( lg == 1 ) fprintf( fpt , "\n" );
		//
	}
	//
	// 2018.12.06
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		lg = 0;
		//
		if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , 0 ) != 0 ) {
			//
			fprintf( fpt , "SLOT_USE = [%s] [MODE=%d]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) , _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , 0 ) );
			//
			lg = 1;
			//
		}
		//
		for ( j = 1 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i , j ) ) continue;
			if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( i , j ) == 0 ) continue;
			//
			if ( lg == 0 ) fprintf( fpt , "SLOT_USE = [%s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(i) );
			//
			fprintf( fpt , "(%d:%d)" , j , _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE_DATA( i , j ) );
			//
			lg = 1;
			//
		}
		//
		if ( lg == 1 ) fprintf( fpt , "\n" );
		//
	}
	//
	//
	//
	fclose(fpt);
	//=======================================================================================================================================
	if ( filename[0] == 0 ) {
		_SCH_COMMON_SYSTEM_DATA_LOG( SCHEDULER_DEBUG_DATA_FILE , lotlog );
	}
	else {
		_SCH_COMMON_SYSTEM_DATA_LOG( filename , lotlog );
	}
	//=======================================================================================================================================
}

//=======================================================================================================================================



