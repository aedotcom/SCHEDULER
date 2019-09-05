//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_sch_preprcs.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A4_default.h"
#include "sch_A4_param.h"
#include "sch_A4_sub.h"
#include "sch_A4_sdm.h"
#include "sch_A4_subBM.h"
//================================================================================



int SCHEDULER_MAKE_LOCK_FREE_for_4( int side , int TMSIDE , int waittm ); // 2010.11.10

//------------------------------------------------------------------------------------------
extern BOOL FM_Pick_Running_Blocking_Style_4; // 2006.08.02
extern BOOL FM_Place_Running_Blocking_Style_4; // 2006.08.02
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
// Include Start
//###################################################################################################

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int sch4_RERUN_END_S1_TAG[ MAX_CHAMBER ];
extern int sch4_RERUN_END_S1_ETC[ MAX_CHAMBER ];

extern int sch4_RERUN_END_S2_TAG[ MAX_CHAMBER ];
extern int sch4_RERUN_END_S2_ETC[ MAX_CHAMBER ];

extern int sch4_RERUN_FST_S1_TAG[ MAX_CHAMBER ];
extern int sch4_RERUN_FST_S1_ETC[ MAX_CHAMBER ];

extern int sch4_RERUN_FST_S2_TAG[ MAX_CHAMBER ];
extern int sch4_RERUN_FST_S2_ETC[ MAX_CHAMBER ];

extern int sch4_RERUN_END_S3_TAG[ MAX_CHAMBER ];
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//================================================================================================================
void SCHEDULER_CONTROL_UTIL_EXTRACT_UPFOLDER_for_4( char *folder , char *res ) {
	int i , l , c = 0;
	l = strlen( folder );
	for ( i = 0 ; i < l ; i++ ) {
		if      ( folder[i] == '/' ) c++;
		else if ( folder[i] == '\\' ) c++;
	}
	strcpy( res , ".." );
	for ( i = 0 ; i < c ; i++ ) {
		strcat( res , "\\.." );
	}
}

BOOL SCHEDULER_CONTROL_Chk_FM_Run_Locking() { // 2007.05.21
	if ( FM_Pick_Running_Blocking_Style_4 ) return TRUE;
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return TRUE;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return TRUE;
	if ( FM_Pick_Running_Blocking_Style_4 ) return TRUE;
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return TRUE;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return TRUE;
	return FALSE;
}
//=======================================================================================
//=======================================================================================
//==============================================================================================================
//=======================================================================================
int SCHEDULER_CONTROL_Chk_WILLDONE_PART( int rside , int rtmside , int rlog , int tmside , BOOL willmode , int pmcheck , int *side , int *pointer , int *order ) { // 2006.01.12
	int i , s , p , o , l;
	int again;

	while ( TRUE ) { // 2006.11.27
		//======================================
		again = FALSE; // 2006.11.27
		//======================================
		for ( l = 0 ; l < 2 ; l++ ) {
			if ( ( pmcheck == 1 ) || ( pmcheck == 3 ) ) {
	//printf( "[***A***]" );
				if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) {
	//printf( "[***B***]" );
					s = _SCH_MODULE_Get_PM_SIDE( PM1 + tmside , 0 );
					p = _SCH_MODULE_Get_PM_POINTER( PM1 + tmside , 0 );
					o = _SCH_MODULE_Get_PM_PATHORDER( PM1 + tmside , 0 );
					if ( willmode ) {
	//printf( "[***C%d,%d,%d,%d***]" , _SCH_CLUSTER_Get_PathStatus( s , p ) , s , p , o );
						if ( pmcheck == 3 ) {
							if ( _SCH_CLUSTER_Get_PathStatus( s , p ) == SCHEDULER_RUNNINGW ) {
		//printf( "[***D***]" );
								*side = s;
								*pointer = p;
								*order = o;
								if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) { // 2006.11.27
									return 4;
								}
								else { // 2006.11.27
									again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [1] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
								} // 2006.11.27
							}
						}
						else {
							if ( _SCH_CLUSTER_Get_PathStatus( s , p ) != SCHEDULER_RUNNING2 ) {
		//printf( "[***D***]" );
								*side = s;
								*pointer = p;
								*order = o;
								if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) { // 2006.11.27
									return 4;
								}
								else { // 2006.11.27
									again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [2] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
								} // 2006.11.27
							}
						}
					}
					else {
	//printf( "[***E%d,%d,%d,%d***]" , _SCH_CLUSTER_Get_PathStatus( s , p ) , s , p , o );
						if ( _SCH_CLUSTER_Get_PathStatus( s , p ) != SCHEDULER_RUNNING ) {
	//printf( "[***F***]" );
							*side = s;
							*pointer = p;
							*order = o;
							if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) { // 2006.11.27
								return 4;
							}
							else { // 2006.11.27
								again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [3] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
							} // 2006.11.27
						}
					}
				}
			}

			if ( pmcheck != 3 ) {
				for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//printf( "[***G***]" );
					if ( _SCH_MODULE_Get_TM_WAFER( tmside , i ) > 0 ) {
		//printf( "[***H***]" );
						s = _SCH_MODULE_Get_TM_SIDE( tmside , i );
						p = _SCH_MODULE_Get_TM_POINTER( tmside , i );
						o = _SCH_MODULE_Get_TM_PATHORDER( tmside , i );
		//printf( "[***I***]" );
						if ( willmode ) {
		//printf( "[***J***]" );
							if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) {
		//printf( "[***K***]" );
								*side = s;
								*pointer = p;
								*order = o;
								if ( _SCH_MODULE_Get_TM_WAFER( tmside , i ) > 0 ) { // 2006.11.27
									return i + 1;
								} // 2006.11.27
								else { // 2006.11.27
									again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [4] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
								} // 2006.11.27
							}
						}
						else {
		//printf( "[***L***]" );
							if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 1 ) {
		//printf( "[***M***]" );
								*side = s;
								*pointer = p;
								*order = o;
								if ( _SCH_MODULE_Get_TM_WAFER( tmside , i ) > 0 ) { // 2006.11.27
									return i + 1;
								}
								else { // 2006.11.27
									again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [5] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
								} // 2006.11.27
							}
						}
					}
				}
				for ( i = _SCH_PRM_GET_MODULE_SIZE( BM1 + tmside ) ; i >= 1 ; i-- ) {
		//printf( "[***N***]" );
					if ( _SCH_MODULE_Get_BM_WAFER( BM1 + tmside , i ) > 0 ) {
		//printf( "[***O***]" );
						s = _SCH_MODULE_Get_BM_SIDE( BM1 + tmside , i );
						p = _SCH_MODULE_Get_BM_POINTER( BM1 + tmside , i );
						if ( willmode ) {
		//printf( "[***P***]" );
							if ( _SCH_MODULE_Get_BM_STATUS( BM1 + tmside , i ) == BUFFER_INWAIT_STATUS ) {
		//printf( "[***Q***]" );
		//						if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 0 ) { // 2006.02.15
								if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) { // 2006.02.15
		//printf( "[***R***]" );
									*side = s;
									*pointer = p;
									*order = _SCH_CLUSTER_Get_PathOrder( s , p , 0 , PM1 + tmside );
									if ( _SCH_MODULE_Get_BM_WAFER( BM1 + tmside , i ) > 0 ) { // 2006.11.27
										return 3;
									}
									else { // 2006.11.27
										again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [6] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
									} // 2006.11.27
								}
							}
						}
						else {
		//printf( "[***S***]" );
							if ( _SCH_MODULE_Get_BM_STATUS( BM1 + tmside , i ) == BUFFER_OUTWAIT_STATUS ) {
		//printf( "[***T***]" );
								if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 0 ) {
		//printf( "[***U***]" );
									*side = s;
									*pointer = p;
									*order = _SCH_CLUSTER_Get_PathOrder( s , p , 0 , PM1 + tmside );
									if ( _SCH_MODULE_Get_BM_WAFER( BM1 + tmside , i ) > 0 ) { // 2006.11.27
										return 3;
									}
									else { // 2006.11.27
										again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [7] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
									} // 2006.11.27
								}
							}
						}
					}
				}
			}

			if ( pmcheck == 2 ) {
	//printf( "[***V***]" );
				if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) {
	//printf( "[***W***]" );
					s = _SCH_MODULE_Get_PM_SIDE( PM1 + tmside , 0 );
					p = _SCH_MODULE_Get_PM_POINTER( PM1 + tmside , 0 );
					o = _SCH_MODULE_Get_PM_PATHORDER( PM1 + tmside , 0 );
					if ( willmode ) {
	//printf( "[***X***]" );
						if ( _SCH_CLUSTER_Get_PathStatus( s , p ) != SCHEDULER_RUNNING2 ) {
	//printf( "[***Y***]" );
							*side = s;
							*pointer = p;
							*order = o;
							if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) { // 2006.11.27
								return 4;
							}
							else { // 2006.11.27
								again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [8] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
							} // 2006.11.27
						}
					}
					else {
	//printf( "[***Z***]" );
						if ( _SCH_CLUSTER_Get_PathStatus( s , p ) != SCHEDULER_RUNNING ) {
	//printf( "[***a***]" );
							*side = s;
							*pointer = p;
							*order = o;
							if ( _SCH_MODULE_Get_PM_WAFER( PM1 + tmside , 0 ) > 0 ) { // 2006.11.27
								return 4;
							}
							else { // 2006.11.27
								again = TRUE; // 2006.11.27
_SCH_LOG_DEBUG_PRINTF( rside , rtmside + 1 , "TM%d Chk_WILLDONE [9] rlog=%d,l=%d,willmode=%d,pmcheck=%d,s,p,o=%d,%d,%d\n" , rtmside + 1 , rlog , l , willmode , pmcheck , s , p , o ); // 2014.03.12
							} // 2006.11.27
						}
					}
				}
			}
		}
		//======================================
		if ( !again ) break; // 2006.11.27
		//======================================
	}
	return 0;
}
//=======================================================================================


BOOL SCHEDULER_CONTROL_Pre_End_Possible_Check( int side , int pmc , BOOL onebodychk ) { // 2006.09.13
	int i , j , k , s1p;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) < 2 ) return TRUE;
	if ( !onebodychk ) return FALSE; // 2006.11.06
	if ( _SCH_MODULE_Get_Mdl_Spd_Flag( side , pmc ) != 0 ) return FALSE; // 2006.11.07
	//======================================================================================================
	// 2007.06.26
	//======================================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			switch( _SCH_CLUSTER_Get_PathStatus( side , i ) ) {
			case SCHEDULER_READY			: s1p = 0;		break;
			case SCHEDULER_SUPPLY			: s1p = 0;		break;
			case SCHEDULER_STARTING			: s1p = 0;		break;
			case SCHEDULER_RUNNING			: s1p = -1;		break;
			case SCHEDULER_RUNNINGW			: s1p = -1;		break;
			case SCHEDULER_RUNNING2			: s1p = -1;		break;
			case SCHEDULER_WAITING			: s1p = -1;		break;
			case SCHEDULER_BM_END			: s1p = -1;		break;
			case SCHEDULER_CM_END			: s1p = -1;		break;
			case SCHEDULER_RETURN_REQUEST	: s1p = 0;		break;
			default							: s1p = -1;		break;
			}
			if ( s1p == -1 ) continue;
			for ( j = s1p ; j < _SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					if ( _SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k ) == pmc ) return FALSE;
				}
			}
		}
	}
	//======================================================================================================
	s1p = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( pmc );
	if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( side , s1p ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( side , s1p ) >= MUF_90_USE_to_XDEC_FROM ) ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( s1p ) <= 0 ) {
			if ( _SCH_MODULE_Get_PM_WAFER( s1p , 0 ) <= 0 ) {
				if ( ( _SCH_MODULE_Get_TM_SIDE( s1p - PM1 , 0 ) != side ) || ( _SCH_MODULE_Get_TM_WAFER( s1p - PM1 , 0 ) <= 0 ) ) { // 2006.11.17
					if ( ( _SCH_MODULE_Get_TM_SIDE( s1p - PM1 , 1 ) != side ) || ( _SCH_MODULE_Get_TM_WAFER( s1p - PM1 , 1 ) <= 0 ) ) { // 2006.11.17
						if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_for_STYLE_4( s1p - PM1 + BM1 , FALSE ) ) {
							return TRUE;
						}
					}
				}
			}
		}
		return FALSE;
	}
	else {
		return TRUE;
	}
}

//=======================================================================================


int SCHEDULER_CONTROL_Other_BM_Go_Vent_Possible_for_STYLE_4( int side , int tmside , int bch ) {
	int nch , mp , s , p;
	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( bch );
	mp = 0;
	while( TRUE ) {
		//
_SCH_LOG_DEBUG_PRINTF( side , tmside + 1 , "TM%d BM_Go_Vent_Possible [1] bch=%d\n" , tmside + 1 , bch ); // 2014.03.12
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
			if ( _SCH_MODULE_Get_BM_FULL_MODE( nch ) != BUFFER_FM_STATION ) { // 2007.04.13
				if ( _SCH_MODULE_GET_USE_ENABLE( nch - BM1 + PM1 , TRUE , -1 ) ) {
					if ( _SCH_MODULE_Get_PM_WAFER( nch - BM1 + PM1 , 0 ) > 0 ) {
						if ( _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( nch - BM1 + PM1 , 0 ) , _SCH_MODULE_Get_PM_POINTER( nch - BM1 + PM1 , 0 ) ) != SCHEDULER_RUNNINGW ) {
							return 1;
						}
					}
				}
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( nch , BUFFER_INWAIT_STATUS ) > 0 ) return 2;
				if ( _SCH_MODULE_Get_TM_WAFER( nch - BM1 , 0 ) > 0 ) {
					s = _SCH_MODULE_Get_TM_SIDE( nch - BM1 , 0 );
					p = _SCH_MODULE_Get_TM_POINTER( nch - BM1 , 0 );
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) {
						return 3;
					}
				}
				if ( _SCH_MODULE_Get_TM_WAFER( nch - BM1 , 1 ) > 0 ) {
					s = _SCH_MODULE_Get_TM_SIDE( nch - BM1 , 1 );
					p = _SCH_MODULE_Get_TM_POINTER( nch - BM1 , 1 );
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) {
						return 4;
					}
				}
			}
			//
			while( TRUE ) {
				if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( bch , mp , &nch ) ) return 0;
				mp++;
				if ( _SCH_MODULE_Get_BM_FULL_MODE( nch ) != BUFFER_FM_STATION ) break;
				//
_SCH_LOG_DEBUG_PRINTF( side , tmside + 1 , "TM%d BM_Go_Vent_Possible [11] bch=%d\n" , tmside + 1 , bch ); // 2014.03.12
				//
			}
		}
		else {
			if ( _SCH_MODULE_GET_USE_ENABLE( nch - BM1 + PM1 , TRUE , -1 ) ) { // 2006.01.13	// 2006.02.03
		//		if ( _SCH_MODULE_Get_PM_WAFER( nch - BM1 + PM1 , 0 ) > 0 ) return TRUE; // 2006.01.13	// 2006.02.03
				if ( _SCH_MODULE_Get_PM_WAFER( nch - BM1 + PM1 , 0 ) > 0 ) { // 2006.01.13	// 2006.02.03
		//			if ( _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( nch - BM1 + PM1 , 0 ) , _SCH_MODULE_Get_PM_POINTER( nch - BM1 + PM1 , 0 ) ) != SCHEDULER_RUNNING ) { // 2006.02.03 // 2006.09.05
					if ( _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( nch - BM1 + PM1 , 0 ) , _SCH_MODULE_Get_PM_POINTER( nch - BM1 + PM1 , 0 ) ) != SCHEDULER_RUNNING2 ) { // 2006.02.03 // 2006.09.05
						if ( _SCH_CLUSTER_Get_PathStatus( _SCH_MODULE_Get_PM_SIDE( nch - BM1 + PM1 , 0 ) , _SCH_MODULE_Get_PM_POINTER( nch - BM1 + PM1 , 0 ) ) != SCHEDULER_RUNNINGW ) { // 2006.11.30
							return 1;
						}
					}
				}
			} // 2006.01.13	// 2006.02.03
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( nch , -1 ) > 0 ) return 2;
			if ( _SCH_MODULE_Get_TM_WAFER( nch - BM1 , 0 ) > 0 )  return 3;
			if ( _SCH_MODULE_Get_TM_WAFER( nch - BM1 , 1 ) > 0 )  return 4;
			break;
		}
		//
	}
	return 0;
}



//------------------------------------------------------------------------------------
// 2005.09.09
//------------------------------------------------------------------------------------
#define MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( ZZZ ) int SCHEDULER_CONTROL_BM_PUMPING_TM_PART_##ZZZ##( int side , int bch , int logoffset ) { \
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { \
		EnterCriticalSection( &CR_SINGLEBM_STYLE_4 ); /* 2007.07.02 */ \
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_All_Not_Running_for_STYLE_4( bch , FALSE ) ) { \
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( bch , FALSE ) ) { \
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , ##ZZZ## + 1 , logoffset ); \
				SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , FALSE ); /* 2007.05.11 */ \
				SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( bch , FALSE ); \
				LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); /* 2007.07.02 */ \
				return 2; \
			} \
			else { \
				if ( SCHEDULER_CONTROL_Other_BM_Go_Pump_Possible_for_STYLE_4( bch ) ) { \
					if ( ##ZZZ## == 0 ) { \
						_SCH_LOG_LOT_PRINTF( side , "TM Scheduling %s Goto TMSIDE(R)%cWHTMGOTMSIDE BM%d:%d,R\n" , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , bch - BM1 + 1 , logoffset ); \
					} \
					else { \
						_SCH_LOG_LOT_PRINTF( side , "TM%d Scheduling %s Goto TMSIDE(R)%cWHTM%dGOTMSIDE BM%d:%d,R\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , ##ZZZ## + 1 , bch - BM1 + 1 , logoffset ); \
					} \
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( bch , FALSE ); \
					_SCH_MODULE_Set_BM_FULL_MODE( bch , BUFFER_TM_STATION ); \
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); /* 2007.07.02 */ \
					return 3; \
				} \
				else { \
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , 10000 + ( ##ZZZ## + 1 ) , logoffset ); \
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , FALSE ); /* 2007.05.11 */ \
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_nch_for_STYLE_4( bch , FALSE ); \
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); /* 2007.07.02 */ \
					return 4; \
				} \
			} \
		} \
		LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); /* 2007.07.02 */ \
	} \
	else { \
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , ##ZZZ## + 1 , logoffset ); \
		return 1; \
	} \
	return 0; \
}

MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_BM_PUMPING_TM_PART_FOR_4( 7 );
#endif
#endif


//------------------------------------------------------------------------------------
// 2005.09.09
//------------------------------------------------------------------------------------
#define MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( ZZZ ) int SCHEDULER_CONTROL_BM_VENTING_PART_##ZZZ##( int side , BOOL fmside , int bch , int logoffset ) { \
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { \
		EnterCriticalSection( &CR_SINGLEBM_STYLE_4 ); \
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_All_Not_Running_for_STYLE_4( bch , TRUE ) ) { \
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( bch , TRUE ) ) { \
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bch , -1 , TRUE , fmside ? 0 : ##ZZZ## + 1 , logoffset ); \
				SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , TRUE ); /* 2007.05.11 */ \
				SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( bch , TRUE ); \
				LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); \
				return 2; \
			} \
			else { \
				if ( SCHEDULER_CONTROL_Other_BM_Go_Vent_Possible_for_STYLE_4( side , ##ZZZ## , bch ) != 0 ) { \
					if ( fmside ) { \
						if ( ##ZZZ## == 0 ) { \
							_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Goto FMSIDE(R)%cWHFMGOFMSIDE BM%d:%d,R\n" , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , bch - BM1 + 1 , logoffset ); \
						} \
						else { \
							_SCH_LOG_LOT_PRINTF( side , "FM%d Handling %s Goto FMSIDE(R)%cWHFM%dGOFMSIDE BM%d:%d,R\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , ##ZZZ## + 1 , bch - BM1 + 1 , logoffset ); \
						} \
					} \
					else { \
						if ( ##ZZZ## == 0 ) { \
							_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Goto FMSIDE(R)%cWHTMGOFMSIDE BM%d:%d,R\n" , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , bch - BM1 + 1 , logoffset ); \
						} \
						else { \
							_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Goto FMSIDE(R)%cWHTM%dGOFMSIDE BM%d:%d,R\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , ##ZZZ## + 1 , bch - BM1 + 1 , logoffset ); \
						} \
					} \
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( bch , TRUE ); \
					_SCH_MODULE_Set_BM_FULL_MODE( bch , BUFFER_FM_STATION ); \
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); \
					return 3; \
				} \
				else { \
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bch , -1 , TRUE , fmside ? 10000 : 10000 + ( ##ZZZ## + 1 ) , logoffset ); \
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , TRUE ); /* 2007.05.11 */ \
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_nch_for_STYLE_4( bch , TRUE ); \
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); \
					return 4; \
				} \
			} \
		} \
		LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); \
	} \
	else { \
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bch , -1 , TRUE , fmside ? 0 : ##ZZZ## + 1 , logoffset ); \
		return 1; \
	} \
	return 0; \
}

MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_BM_VENTING_PART_FOR_4( 7 );
#endif
#endif

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


#define MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( ZZZ ) BOOL SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4_##ZZZ##( int side , int bmch , int *slot , int *order ) { \
	int i , m , pt , oldpt = 9999999 , k = 0; \
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { /* 2005.10.04 */ \
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) return FALSE; /* 2005.10.04 */ \
	} /* 2005.10.04 */ \
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , TRUE , side ) ) return FALSE; \
	if ( !SCHEDULER_CONTROL_Chk_ARM_DUAL_MODE( ZZZ ) ) { \
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) return FALSE; \
	} \
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PICK ) ) > 0 ) return FALSE; \
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_TM_STATION ) return FALSE; \
	/* if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { */ /* 2005.10.04 2007.03.02 */ \
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) return FALSE; \
	/* } */ /* 2005.10.04 2007.03.02 */ \
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i++ ) { \
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) { \
			if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) { \
				if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_OUTWAIT_STATUS ) { \
					pt = _SCH_MODULE_Get_BM_POINTER( bmch , i ); \
					if ( pt >= 100 ) { \
						if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( pt ) == 2 ) { \
							if ( oldpt > pt ) { \
								k++; \
								oldpt = pt; \
								*slot = i; \
								*order = 0; \
							} \
						} \
						else { \
							k++; \
							*slot = i; \
							*order = 0; \
							break; \
						} \
					} \
					if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , pt , 2 ) == 0 ) pt = -1; \
					if ( ( oldpt > pt ) && ( pt != -1 ) ) { \
						k++; \
						oldpt = pt; \
						*slot = i; \
						*order = 0; \
						for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) { \
							if ( ( bmch - BM1 + PM1 ) == _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , m ) ) { \
								*order = m; \
								break; \
							} \
						} \
					} \
				} \
			} \
		} \
	} \
	if ( k > 0 ) return TRUE; \
	return FALSE; \
}

MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4( 7 );
#endif
#endif

void SCHEDULER_CONTROL_Chk_MULTIJOB_PAUSE_FOR_4( int bmch ) { // 2004.10.14
	int i , s , p;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
			p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
			if ( p < 100 ) {
				_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( s , p , 0 );
			}
		}
	}
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PICK ) ) > 0 ) {
		s = _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PICK ) );
		p = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PICK ) );
		if ( p < 100 ) {
			_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( s , p , 0 );
		}
	}
}

#define MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( ZZZ ) int SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4_##ZZZ##( int side , int bmch , int *pickskip , BOOL *runact ) { \
	int i , pt , oldpt = 9999999 , k = -1 , slot; \
	if ( *pickskip ) return -1; \
/*	if ( !_SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , TRUE , side ) ) { */ /* 2007.03.15 */ \
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) { /* 2007.03.15 */ \
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i++ ) { \
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) { \
				if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) { \
					if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_OUTWAIT_STATUS ) { \
						pt = _SCH_MODULE_Get_BM_POINTER( bmch , i ); \
						if ( pt >= 100 ) { \
							if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( pt ) == 2 ) { \
								if ( oldpt > pt ) { \
									k = pt; \
									oldpt = pt; \
									slot = i; \
								} \
							} \
							else { \
								k = pt; \
								slot = i; \
								break; \
							} \
						} \
						if ( oldpt > pt ) { \
							k = pt; \
							oldpt = pt; \
							slot = i; \
						} \
					} \
					else { /* 2007.03.16 */ \
						if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_FM_STATION ) { /* 2007.03.16 */ \
							*runact = TRUE; /* 2007.03.16 */ \
						} /* 2007.03.16 */ \
					} /* 2007.03.16 */ \
				} \
			} \
		} \
	} \
	/* if ( k <= 0 ) { */ /* 2007.03.16 */ \
	if ( k < 0 ) { /* 2007.03.16 */ \
		return 0; \
	} \
	if ( k >= 100 ) { \
		/* _SCH_MODULE_Set_BM_STATUS( bmch , slot , BUFFER_OUTWAIT_STATUS ); */ /* 2007.03.16 */ \
		if ( _SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() <= 1 ) { /* 2007.03.16 */ \
			_SCH_MODULE_Set_BM_STATUS( bmch , slot , BUFFER_OUTWAIT_STATUS ); /* 2007.03.16 */ \
		} \
		*pickskip = TRUE; \
		return 2; \
	} \
	/* _SCH_MODULE_Set_BM_STATUS( bmch , slot , BUFFER_OUTWAIT_STATUS ); */ /* 2007.03.16 */ \
	if ( _SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() <= 1 ) { /* 2007.03.16 */ \
		_SCH_MODULE_Set_BM_STATUS( bmch , slot , BUFFER_OUTWAIT_STATUS ); /* 2007.03.16 */ \
		_SCH_CLUSTER_Check_and_Make_Return_Wafer( side , k , -1 ); /* 2007.03.16 */ \
	} \
	return 1; \
}

MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4( 7 );
#endif
#endif


#define MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( ZZZ ) BOOL SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4_##ZZZ##( int side , int pointer , int pmch , int *nextyes , int *nextpmyes ) { \
	int i , pt , z; \
	if ( _SCH_MODULE_Get_PM_WAFER( pmch , 0 ) <= 0 ) return FALSE; \
	if ( _SCH_MODULE_Get_PM_SIDE( pmch , 0 ) != side ) return FALSE; \
	if ( ( pointer < 100 ) && ( _SCH_SYSTEM_CPJOB_GET(side) != 0 ) ) { \
		if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , pointer , 2 ) == 0 ) return FALSE; \
	} \
	if ( !_SCH_MODULE_GET_USE_ENABLE( pmch , TRUE , side ) ) return FALSE; \
	if ( _SCH_MODULE_Get_BM_FULL_MODE( pmch - PM1 + BM1 ) != BUFFER_TM_STATION ) return FALSE; \
	if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( pmch - PM1 + BM1 ) ) return FALSE; \
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmch ) > 0 ) return FALSE; \
	if ( _SCH_MODULE_Get_TM_WAFER( pmch - PM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( pmch - PM1 , CHECKORDER_FOR_TM_PICK ) ) > 0 ) return FALSE; \
	if ( SCHEDULER_CONTROL_Chk_ARM_DUAL_MODE( pmch - PM1 ) ) { /* 2005.06.28 */ \
		if ( _SCH_MODULE_Get_TM_WAFER( pmch - PM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( pmch - PM1 , CHECKORDER_FOR_FM_PICK ) ) > 0 ) { \
			*nextyes = 0; \
			*nextpmyes = 0; \
			pt = _SCH_MODULE_Get_TM_POINTER( pmch - PM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( pmch - PM1 , CHECKORDER_FOR_FM_PICK ) ); \
			if ( pt >= 100 ) { \
				if ( *nextyes <= 1 ) { \
					*nextyes = pt; \
				} \
			} \
			else { \
				*nextpmyes = 1; \
				if ( *nextyes == 0 ) { \
					*nextyes = 1; \
				} \
			} \
			if ( ( *nextyes == 0 ) || ( *nextyes == 1 ) ) { \
				z = SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CYCLE_END_CHECK( side , pmch , TRUE , &pt ); \
				if ( z >= 1 ) { \
					*nextyes = 100 + pt; \
					*nextpmyes = 0; \
				} \
			} \
		} \
		else { \
			*nextyes = 0; \
			*nextpmyes = 0; \
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( pmch - PM1 + BM1 , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE; \
			if ( ( *nextyes == 0 ) || ( *nextyes == 1 ) ) { \
				z = SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CYCLE_END_CHECK( side , pmch , TRUE , &pt ); \
				if ( z >= 1 ) { \
					*nextyes = 100 + pt; \
					*nextpmyes = 0; \
				} \
			} \
		} \
	} \
	else { \
		if ( !_SCH_MODULE_GET_USE_ENABLE( pmch , FALSE , side ) ) { /* 2006.12.07 */ \
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( pmch - PM1 + BM1 , -1 ) > 0 ) { \
				/* return FALSE; */ /* 2006.12.07 2007.03.21 */ \
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) return FALSE; /* 2007.03.21 */ \
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( pmch - PM1 + BM1 , BUFFER_OUTWAIT_STATUS ) > 0 ) return FALSE; /* 2007.03.21 */ \
			} \
		} /* 2006.12.07 */ \
		*nextyes = 0; \
		*nextpmyes = 0; \
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( pmch - PM1 + BM1 ) ; i++ ) { \
			if ( _SCH_MODULE_Get_BM_WAFER( pmch - PM1 + BM1 , i ) > 0 ) { \
				if ( _SCH_MODULE_Get_BM_STATUS( pmch - PM1 + BM1 , i ) != BUFFER_OUTWAIT_STATUS ) { \
					pt = _SCH_MODULE_Get_BM_POINTER( pmch - PM1 + BM1 , i ); \
					if ( pt >= 100 ) { \
						if ( *nextyes <= 1 ) { \
							*nextyes = pt; \
						} \
					} \
					else { \
						*nextpmyes = 1; \
						if ( *nextyes == 0 ) { \
							*nextyes = 1; \
						} \
					} \
				} \
			} \
		} \
		if ( ( *nextyes == 0 ) || ( *nextyes == 1 ) ) { \
			z = SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CYCLE_END_CHECK( side , pmch , TRUE , &pt ); \
			if ( z >= 1 ) { \
				*nextyes = 100 + pt; \
				*nextpmyes = 0; \
			} \
		} \
	} \
	if ( !_SCH_MODULE_GET_USE_ENABLE( pmch , FALSE , side ) ) { /* 2007.05.11 */ \
		*nextyes = 0; /* 2007.05.11 */ \
	} /* 2007.05.11 */ \
	return TRUE; \
}

MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4( 7 );
#endif
#endif


#define MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( ZZZ ) BOOL SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4_##ZZZ##( int side , int pointer , int *arm , int bmch , int *slot ) { \
	int pl; \
	if ( SCHEDULER_CONTROL_Chk_ARM_DUAL_MODE( bmch - BM1 ) ) { /* 2007.06.15 */ \
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , *arm ) <= 0 ) { \
			if ( *arm == TA_STATION ) { \
				if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , TB_STATION ) <= 0 ) return FALSE; /* 2007.06.15*/ \
				if ( _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , TB_STATION ) != side ) return FALSE; /* 2007.06.15*/ \
				if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) return FALSE; /* 2007.06.15*/ \
				*arm = TB_STATION; /* 2007.06.15 */ \
				pointer = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , TB_STATION ); \
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( side , pointer , -1 ); /* 2007.06.15 */ \
			} \
			else { \
				if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , TA_STATION ) <= 0 ) return FALSE; /* 2007.06.15*/ \
				if ( _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , TA_STATION ) != side ) return FALSE; /* 2007.06.15*/ \
				if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) return FALSE; /* 2007.06.15*/ \
				*arm = TA_STATION; /* 2007.06.15 */ \
				pointer = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , TA_STATION ); \
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( side , pointer , -1 ); /* 2007.06.15 */ \
			} \
		} \
		else { \
			if ( _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , *arm ) != side ) return FALSE; \
		} \
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) { \
			if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) <= 0 ) { \
				/* if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , TRUE , side ) ) { */ /* 2007.06.15 */ \
				if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) { /* 2007.06.15 */ \
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch - BM1 + PM1 ) <= 0 ) { \
						return FALSE; \
					} \
				} \
			} \
		} \
	} /* 2007.06.15 */ \
	else { /* 2007.06.15 */ \
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , *arm ) <= 0 ) return FALSE; \
		if ( _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , *arm ) != side ) return FALSE; \
	} /* 2007.06.15 */ \
	if ( ( pointer < 100 ) && ( _SCH_SYSTEM_CPJOB_GET(side) != 0 ) ) { \
		if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , pointer , 2 ) == 0 ) return FALSE; \
	} \
	pl = FALSE; \
	if ( _SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() <= 1 ) { /* 2006.05.15 */ \
		switch( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) ) { \
		case 1 : \
		case 3 : \
			pl = TRUE; \
			break; \
		} \
	} \
	else { \
		if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) != 2 ) { /* 2006.05.15 */ \
			switch( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) ) { \
			case 2 : \
			case 3 : \
				pl = TRUE; \
				break; \
			} \
		} \
		else { \
			switch( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) ) { \
			case 1 : \
			case 3 : \
				pl = TRUE; \
				break; \
			} \
		} \
	} \
	if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , slot , pl ? 1 : 2 ) ) return TRUE; \
	return FALSE; \
}

MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4( 7 );
#endif
#endif

#define MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( ZZZ ) int SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4_##ZZZ##( int side , int bmch , BOOL TmmovePass , BOOL FullOut , int nxmode ) { \
	int s , i , j , m , f , k = 0; \
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_TM_STATION ) return 1; \
	if ( !TmmovePass ) { \
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) return 2; \
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ) > 0 ) return 3; \
	} \
	if ( FullOut ) { \
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) return 4; \
	} \
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS ) > 0 ) return 5; \
	k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_OUTWAIT_STATUS ); \
	/* if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_LOTFIRST( bmch - BM1 + PM1 ) ) { */ /* 2006.07.12 */ \
	/* 	return 6; */ /* 2006.07.12 */ \
	/* } */ /* 2006.07.12 */ \
	if ( !FullOut ) { /* 2005.12.19 */ \
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) { \
			if ( sch4_RERUN_END_S2_TAG[ bmch - BM1 + PM1 ] == -1 ) { \
				if ( sch4_RERUN_FST_S2_TAG[ bmch - BM1 + PM1 ] != -1 ) { \
					if ( k == 0 ) { /* 2006.01.10 */ \
						if ( !SCHEDULER_CONTROL_Get_SDM_4_READY_WAFER_SLOT0( side , bmch - BM1 + PM1 ) ) return 11; \
					} \
				} \
				else if ( sch4_RERUN_FST_S1_TAG[ bmch - BM1 + PM1 ] != -1 ) { \
					if ( k == 0 ) { /* 2006.01.10 */ \
						return 12; \
					} \
				} \
			} \
		} \
	} \
	f = 0; \
	if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( bmch - BM1 + PM1 ) ) { \
		f = 1; \
	} \
	else { \
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) { \
			if ( !_SCH_SUB_Simple_Supply_Possible( s ) ) continue; \
			if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( s , 0 , bmch , -1 , -1 ) ) { \
				f = 1; \
				break; \
			} \
		} \
	} \
	/* if ( ( k == 0 ) && ( f == 0 ) ) return FALSE; */ /* 2004.11.04 */ \
	if ( ( ( nxmode == 1 ) || ( nxmode == 3 ) ) && ( f == 0 ) ) { \
		if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , TRUE , side ) ) { \
			if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) { \
				if ( _SCH_PRM_GET_MODULE_SIZE( bmch ) > k ) return 7; \
			} \
		} \
	} \
	if ( ( k == 0 ) && ( f == 0 ) ) { /* 2004.11.04 */ \
		/* if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) ) return 8; */ /* 2004.11.24 */ \
		if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) return 8; /* 2004.11.24  2007.04.05*/ \
		i = -1; \
		/* if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { */ /* 2007.05.30 */ \
		if ( ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) && !FM_Place_Running_Blocking_Style_4 ) { /* 2007.05.30 */ \
			if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == side ) ) { \
				i = _SCH_MODULE_Get_FM_POINTER( TA_STATION ); \
			} \
			else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == side ) ) { \
				i = _SCH_MODULE_Get_FM_POINTER( TB_STATION ); \
			} \
		} \
		if ( i == -1 ) { \
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) return 0; /* 2007.04.11 */ \
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { /* 2006.09.05 */ \
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( bmch , TRUE ) ) { /* 2006.09.05 */ \
					return 0; /* 2006.09.05 */ \
				} \
			} \
			return 9; \
		} \
		else { \
			j = -1; \
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { \
				m = _SCH_CLUSTER_Get_PathProcessChamber( side , i , 0 , k ); \
				if ( m == ( bmch - BM1 + PM1 ) ) { \
					if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) { \
						j = k; \
						break; \
					} \
				} \
			} \
			if ( j == -1 ) return 10; \
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { /* 2006.11.29 */ \
				/* 2007.05.30 Begin */ \
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { \
					m = _SCH_CLUSTER_Get_PathProcessChamber( side , i , 0 , k ); \
					if ( m != ( bmch - BM1 + PM1 ) ) { \
						if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) { \
							if ( _SCH_MODULE_Get_BM_FULL_MODE( m - PM1 + BM1 ) != BUFFER_TM_STATION ) { \
								return 11; \
							} \
						} \
					} \
				} \
				/* 2007.05.30 End */ \
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { \
					if ( j != k ) { \
						_SCH_CLUSTER_Set_PathProcessChamber_Disable( side , i , 0 , k ); \
					} \
				} \
			} \
		} \
	} \
	return 0; \
}

MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4( 7 );
#endif
#endif

#define MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( ZZZ ) BOOL SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4_##ZZZ##( int side , int pmch , int *pt ) { \
	if ( !SCHEDULER_CONTROL_Chk_ARM_DUAL_MODE( pmch - PM1 ) ) return FALSE; \
	if ( _SCH_MODULE_Get_PM_WAFER( pmch , 0 ) > 0 ) return FALSE; \
	if ( _SCH_MODULE_Get_TM_WAFER( pmch - PM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( pmch - PM1 , CHECKORDER_FOR_TM_PLACE ) ) <= 0 ) return FALSE; \
	if ( _SCH_MODULE_Get_TM_SIDE( pmch - PM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( pmch - PM1 , CHECKORDER_FOR_TM_PLACE ) ) != side ) return FALSE; \
	/* if ( !_SCH_MODULE_GET_USE_ENABLE( pmch , TRUE , side ) ) return FALSE; */ /* 2006.05.04 */ \
	if ( !_SCH_MODULE_GET_USE_ENABLE( pmch , FALSE , side ) ) return FALSE; /* 2006.05.04 */ \
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pmch ) > 0 ) return FALSE; \
	*pt = _SCH_MODULE_Get_TM_POINTER( pmch - PM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( pmch - PM1 , CHECKORDER_FOR_TM_PLACE ) ); \
	if ( *pt < 100 ) { /* 2009.08.04 */ \
		if ( _SCH_CLUSTER_Get_PathDo( side , *pt ) == PATHDO_WAFER_RETURN ) return FALSE; /* 2006.05.22 */ \
	} \
	return TRUE; \
}

MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4( 7 );
#endif
#endif

#define CRITICAL_AND_SLEEP_FOR_4( ZZZ , TIME ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( ##ZZZ## ); /* 2007.07.23 */ \
	Sleep( TIME ); /* 2007.07.23 */ \
	_SCH_SYSTEM_ENTER_TM_CRITICAL( ##ZZZ## ); /* 2007.07.23 */ \



#define MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( ZZZ ) int SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( int side , int maincheck , int id ) { \
	int mainmode , s , p; \
	int tag2 = FALSE; \
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) == 0 ) return 0; \
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) tag2 = TRUE; \
	mainmode = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + ##ZZZ## ); \
	while( TRUE ) { \
		_SCH_LOG_DEBUG_PRINTF( side , ##ZZZ## + 1 , "TM%d STEP LOOP I id=%d,maincheck=%d,tag2=%d,mainmode=%d\n" , ##ZZZ## + 1 , id , maincheck , tag2 , mainmode ); \
		if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return -1; \
		if ( SCHEDULER_CONTROL_PROCESS_MONITOR_Run_with_Properly_Status( PM1 + ##ZZZ## + mainmode , maincheck , FALSE ) ) break; \
		CRITICAL_AND_SLEEP_FOR_4( ZZZ , 1 ) /* 2007.07.23 */ \
	} \
	while( TRUE ) { \
		_SCH_LOG_DEBUG_PRINTF( side , ##ZZZ## + 1 , "TM%d STEP LOOP 0 id=%d,maincheck=%d,tag2=%d,mainmode=%d\n" , ##ZZZ## + 1 , id , maincheck , tag2 , mainmode ); \
		if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return -1; \
		if ( maincheck == PROCESS_INDICATOR_MAIN ) { /* Main */ \
			if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( TM + ##ZZZ## + mainmode , FALSE ) != SYS_RUNNING ) { \
				if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( PM1 + ##ZZZ## + mainmode , FALSE ) != SYS_RUNNING ) { \
					if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + ##ZZZ## + mainmode ) == BUFFER_FM_STATION ) { \
						if ( !tag2 ) return 0; \
						if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) <= 0 ) { \
							if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + ##ZZZ## + mainmode ) == BUFFER_FM_STATION ) { /* 2007.04.13 */ \
								if ( mainmode == 1 ) { \
									return 1; \
								} \
								else { \
									return 2; \
								} \
							} \
						} \
						else { \
							s = _SCH_MODULE_Get_PM_SIDE( PM1 + ##ZZZ## + mainmode , 0 ); /* 2007.04.13 */ \
							p = _SCH_MODULE_Get_PM_POINTER( PM1 + ##ZZZ## + mainmode , 0 ); /* 2007.04.13 */ \
							if ( ( _SCH_CLUSTER_Get_PathStatus( s , p ) == SCHEDULER_RUNNING2 ) || ( _SCH_CLUSTER_Get_PathStatus( s , p ) == SCHEDULER_RUNNINGW ) ) { /* 2007.04.13 */ \
								if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) > 0 ) { /* 2007.04.13 */ \
									if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + ##ZZZ## + mainmode ) == BUFFER_FM_STATION ) { /* 2007.04.13 */ \
										if ( mainmode == 1 ) { \
											return 0; \
										} \
										else { \
											return 3; \
										} \
									} \
								} \
							} \
						} \
					} \
					else { \
						if ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## + mainmode , SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## + mainmode , CHECKORDER_FOR_TM_PLACE ) ) <= 0 ) { \
							if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_for_STYLE_4( BM1 + ##ZZZ## + mainmode , FALSE ) ) { \
								if ( !tag2 ) return 0; \
								if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) <= 0 ) { \
									if ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## + mainmode , SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## + mainmode , CHECKORDER_FOR_TM_PLACE ) ) <= 0 ) { /* 2007.04.13 */ \
										if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) <= 0 ) { /* 2007.04.13 */ \
											if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + ##ZZZ## + mainmode ) != BUFFER_FM_STATION ) { /* 2007.04.13 */ \
												if ( mainmode == 1 ) { \
													return 1; \
												} \
												else { \
													return 2; \
												} \
											} \
										} \
									} \
								} \
								else { \
									s = _SCH_MODULE_Get_PM_SIDE( PM1 + ##ZZZ## + mainmode , 0 ); /* 2007.04.13 */ \
									p = _SCH_MODULE_Get_PM_POINTER( PM1 + ##ZZZ## + mainmode , 0 ); /* 2007.04.13 */ \
									if ( ( _SCH_CLUSTER_Get_PathStatus( s , p ) == SCHEDULER_RUNNING2 ) || ( _SCH_CLUSTER_Get_PathStatus( s , p ) == SCHEDULER_RUNNINGW ) ) { /* 2007.04.13 */ \
										if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) > 0 ) { /* 2007.04.13 */ \
											if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + ##ZZZ## + mainmode ) != BUFFER_FM_STATION ) { /* 2007.04.13 */ \
												if ( mainmode == 1 ) { \
													return 0; \
												} \
												else { \
													return 3; \
												} \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
			} \
		} \
		else if ( maincheck == PROCESS_INDICATOR_POST ) { /* Post */ \
			if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) <= 0 ) { \
				if ( mainmode == 1 ) { \
					return 0; \
				} \
				else { \
					return 3; \
				} \
			} \
			else { /* 2006.11.29 */ \
				if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , ##ZZZ## , 1001 , ##ZZZ## + mainmode , TRUE , 3 , &s , &s , &s ) > 0 ) { \
					if ( mainmode == 1 ) { \
						return 0; \
					} \
					else { \
						return 4; \
					} \
				} \
			} \
		} \
		else { /* Pre */ \
			if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( TM + ##ZZZ## + mainmode , FALSE ) != SYS_RUNNING ) { \
				if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( PM1 + ##ZZZ## + mainmode , FALSE ) != SYS_RUNNING ) { \
					if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## + mainmode , 0 ) <= 0 ) { \
						if ( mainmode == 1 ) { \
							return 0; \
						} \
						else { \
							return 3; \
						} \
					} \
					else { /* 2006.11.29 */ \
						if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , ##ZZZ## , 1002 , ##ZZZ## + mainmode , TRUE , 3 , &s , &s , &s ) > 0 ) { \
							if ( mainmode == 1 ) { \
								return 0; \
							} \
							else { \
								return 4; \
							} \
						} \
					} \
				} \
			} \
		} \
		CRITICAL_AND_SLEEP_FOR_4( ZZZ , 1 ) /* 2007.07.23 */ \
	} \
	return 0; \
}

MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 0 );
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 1 );
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 2 );
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 3 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 4 );
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 5 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 6 );
MAKE_SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4( 7 );
#endif
#endif


void SCHEDULER_CONTROL_PRCSWAITING_10SEC( int pmc , int mode ) { // 2006.11.23
	int i , m;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) < 2 ) return;
	m = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( pmc );
	for ( i = 0 ; i < 100 ; i++ ) {
		if ( mode == -1 ) { // 2007.04.03
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) return;
		}
		else { // 2007.04.03
			if ( SCHEDULER_CONTROL_PROCESS_MONITOR_Run_with_Properly_Status( m , mode , TRUE ) ) return;
		}
		Sleep(100);
	}
}



BOOL SCHEDULER_CONTROL_Chk_OTHER_WILL_GO_THIS_PM( int side , int tmatm ) { // 2005.03.08
	int i , j , m , S , P;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( tmatm + BM1 ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( tmatm + BM1 , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( tmatm + BM1 , i ) == BUFFER_INWAIT_STATUS ) {
				S = _SCH_MODULE_Get_BM_SIDE( tmatm + BM1 , i );
				P = _SCH_MODULE_Get_BM_POINTER( tmatm + BM1 , i );
				if ( _SCH_CLUSTER_Get_PathDo( S , P ) == 0 ) {
					for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
						m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , 0 , j );
						if ( m == ( tmatm + PM1 ) ) {
							if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		if ( _SCH_MODULE_Get_TM_WAFER( tmatm , i ) > 0 ) {
			S = _SCH_MODULE_Get_TM_SIDE( tmatm , i );
			P = _SCH_MODULE_Get_TM_POINTER( tmatm , i );
			if ( _SCH_CLUSTER_Get_PathDo( S , P ) == 1 ) {
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , 0 , j );
					if ( m == ( tmatm + PM1 ) ) {
						if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}




BOOL SCHEDULING_Possible_RUNNING_for_STYLE_4() { // 2007.08.27
	int s;
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		if ( _SCH_SUB_Simple_Supply_Possible( s ) ) return TRUE;
	}
	return FALSE;
}

BOOL SCHEDULING_Possible_Pump_Force_BM_for_STYLE_4( int side , int bmch ) {
	int s;
	//---------------------------------------------------------------------------------------
	if ( FM_Pick_Running_Blocking_Style_4 ) return FALSE; // 2007.05.07
	//---------------------------------------------------------------------------------------
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) return FALSE;

	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , -1 ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) return FALSE; // 2004.08.17
	if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_LOTFIRST( bmch - BM1 + PM1 ) ) {
		//---------------------------------------------------------------------------------------
		if ( FM_Pick_Running_Blocking_Style_4 ) return FALSE; // 2007.05.07
		//---------------------------------------------------------------------------------------
		return TRUE;
	}
	if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( bmch - BM1 + PM1 ) ) {
		return FALSE;
	}
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		if ( !_SCH_SUB_Simple_Supply_Possible( s ) ) continue; // 2006.11.30
		if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( s , 0 , bmch , -1 , -1 ) ) return FALSE; // 2006.11.30
	}
	//---------------------------------------------------------------------------------------
	if ( FM_Pick_Running_Blocking_Style_4 ) return FALSE; // 2007.05.07
	//---------------------------------------------------------------------------------------
	return TRUE;
}

int SCHEDULING_Possible_Pump_Disable_Force_BM_for_STYLE_4( int side , int bmch ) { // 2006.05.02
	int i , y = 0 , s , p;
	int v;

	if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) return -1;
	//
	if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) return -2;
	//
	if ( FM_Pick_Running_Blocking_Style_4 ) return -3; // 2006.12.07
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) {
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) {
			y = 1;
		}
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PICK ) ) > 0 ) {
			y = 1;
		}
//		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , -1 ) > 0 ) y = 1; // 2007.04.03
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , -1 ) > 0 ) y = 2; // 2007.04.03
//		if ( y == 1 ) { // 2007.04.03
		if ( ( y == 1 ) || ( y == 2 ) ) { // 2007.04.03
			//=============================================================================================================
			// 2007.03.21 // 2007.04.03
			//=============================================================================================================
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2007.03.21
				if ( y == 2 ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( bmch ) , -1 ) <= 0 ) {
						return -7;
					}
				}
			}
			//=============================================================================================================
			if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) {
				s = _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) );
				p = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) );

				if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= 1 ) {
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
					_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , s , p );
				}
			}
			if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PICK ) ) > 0 ) {
				s = _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PICK ) );
				p = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PICK ) );

				if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= 1 ) {
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
					_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , s , p );
				}
			}
			for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
					s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
					p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= 1 ) {
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , s , p );
						//
						_SCH_MODULE_Set_BM_STATUS( bmch , i , BUFFER_OUTWAIT_STATUS );
					}
				}
			}
			//
			// 2017.03.22
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , TRUE , side ) ) {
				if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch - BM1 + PM1 ) <= 0 ) {
						return -7;
					}
				}
			}
			//
			return 1; // go vent
		}
	}
	else {
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) return -4;
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PICK ) ) > 0 ) return -5;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , -1 ) > 0 ) {
			//===================================================================================================
			// 2006.12.06
			//===================================================================================================
//			return -1; // 2006.12.06
			//===================================================================================================
			v = FALSE;
			for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
					s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
					p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= 1 ) {
						v = TRUE;
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , s , p );
						//
						_SCH_MODULE_Set_BM_STATUS( bmch , i , BUFFER_OUTWAIT_STATUS );
					}
					else if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
						v = TRUE;
					}
				}
			}
//			if ( ( v ) || (	SCHEDULER_CONTROL_Chk_ONEBODY_BM_Other_Wait_RealRun_for_STYLE_4( bmch , TRUE ) ) ) { // 2006.12.07
			if ( ( v ) && (	SCHEDULER_CONTROL_Chk_ONEBODY_BM_Other_Wait_RealRun_for_STYLE_4( bmch , TRUE ) ) ) { // 2006.12.07
				return 1; // go vent
			}
			//===================================================================================================
		}
		else {
			if ( _SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , TRUE , side ) ) {
				if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch - BM1 + PM1 ) <= 0 ) {
						return 0;
					}
				}
			}
		}
	}
	return -6;
}

int SCHEDULING_Possible_Pump_LotFirst_Lock_Force_BM_for_STYLE_4( int side , int bmch ) { // 2006.07.21
	int j;

	for ( j = 0 ; j < 2 ; j++ ) {
		//
		if ( ( sch4_RERUN_END_S2_TAG[ bmch - BM1 + PM1] == -1 ) && ( sch4_RERUN_FST_S2_TAG[ bmch - BM1 + PM1] == -1 ) ) return -1;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) return -2;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) return -3;
		//
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) return -4;
		//
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PICK ) ) > 0 ) return -5;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , -1 ) > 0 ) return -6;
		//
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch ) > 0 ) return -7;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch - BM1 + PM1 ) > 0 ) return -8;
	}
	//
	return ( SCHEDULER_CONTROL_Get_SDM_4_NOTHING_WAFER_SLOT( side , bmch - BM1 + PM1 , _SCH_MODULE_Get_PM_POINTER( bmch - BM1 + PM1 , 0 ) ) );
}

//=======================================================================================================================
//=======================================================================================================================
//=======================================================================================================================

BOOL SCHEDULER_CONTROL_PM_PART_RUNNING( int ch ) { // 2006.11.16
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( ch - PM1 + TM , FALSE ) == SYS_RUNNING ) return TRUE;
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( ch - PM1 + BM1 , FALSE ) == SYS_RUNNING ) return TRUE;
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( ch , FALSE ) == SYS_RUNNING ) return TRUE;
	return FALSE;
}

//=======================================================================================================================
//=======================================================================================================================
//=======================================================================================================================


BOOL SCHEDULER_CONTROL_Chk_OtherPM_Has_More_PRE( int tmside0 , int ch0 ) { // 2015.06.25
	//
	int i;
	//
	int tmside , ch;
	int s , p;
	//
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) < 2 ) return FALSE;
	//
	//---------------------------------------------------------------------------
	//
	if ( ( tmside0 % 2 ) == 0 )
		tmside = tmside0 + 1;
	else
		tmside = tmside0 - 1;
	//
	ch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch0 );
	//
	//---------------------------------------------------------------------------
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tmside , i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( tmside , i );
		p = _SCH_MODULE_Get_TM_POINTER( tmside , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 1 ) continue;
		//
		if ( _SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) || !_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( s , ch ) ) continue;
		//
		if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( s , ch ) == 0 ) continue;
		//
		return TRUE;
		//
	}
	//
	return FALSE;
}

int SCHEDULER_CONTROL_Chk_PRE_BEFORE_PLACE_PART( int tmside , int side , int pointer , int ch , int order , char *RunRecipe , int *forcespawn , int id , int *subresult ) { // 2006.01.12
	int j , s1p , pds;
	int res , side2 , pointer2 , order2;
	char imsi[64];
	//
	*forcespawn = FALSE;
	*subresult = 0;
	//
	//=====================================================================================================================
	// 2007.06.29
	//=====================================================================================================================
	if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_SUB_Check_Last_And_Current_CPName_is_Different( side , pointer , ch ) ) { // 2007.06.29
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) < 2 ) {
			if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch ) == 0 ) {
				_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 3 );
				*subresult = 1;
			}
			else {
				_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 4 );
				*subresult = 2;
			}
		}
		else {
			if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch ) == 0 ) {
				s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside );
				if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch + s1p ) != 0 ) {
					_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 4 );
					*subresult = 3;
				}
				else {
					_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 3 );
					*subresult = 4;
				}
			}
			else {
				_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 4 );
				*subresult = 5;
			}
		}
	}
	//=====================================================================================================================
	//
//	if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , ch ) ) { // 2007.04.12
	if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , ch ) ) { // 2007.04.12
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[1][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
		//
		*subresult = *subresult + 100;
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) < 2 ) {
			//
//			_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( side , ch ); // 2009.09.15
			//
			if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch ) != 0 ) {
				strcpy( RunRecipe , _SCH_PREPOST_Get_inside_READY_RECIPE_NAME( side , ch ) );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[2][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
				*subresult = *subresult + 10;
			}
			else {
				strcpy( RunRecipe , "?" );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[3][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
				*subresult = *subresult + 20;
			}
//			return 99; // 2009.09.15
			return 100; // 2009.09.15
		}
		else {
			//
			s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside ); // 2009.09.18
			res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , tmside , 2001 , tmside + s1p , TRUE , 0 , &side2 , &pointer2 , &order2 ); // 2009.09.18
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[4][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
			//
//			_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( side , ch ); // 2009.09.15
			//
			if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch ) != 0 ) {
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[5][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
				strcpy( RunRecipe , _SCH_PREPOST_Get_inside_READY_RECIPE_NAME( side , ch ) );
				//
				*subresult = *subresult + 30;
			}
			else {
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[6][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
//				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) { // 2006.11.28
//					s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside ); // 2009.09.18
					if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch + s1p ) != 0 ) {
						strcpy( RunRecipe , _SCH_PREPOST_Get_inside_READY_RECIPE_NAME( side , ch + s1p ) );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[7][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
						//
						*subresult = *subresult + 40;
					}
					else {
						strcpy( RunRecipe , "?" );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[8][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
						*subresult = *subresult + 50;
					}
//				} // 2006.11.28
//				else { // 2006.11.28
//					strcpy( RunRecipe , "?" ); // 2006.11.28
//				} // 2006.11.28
			}
//			return 99; // 2009.09.15

			//
			// 2009.09.18
			//
			if ( res != 0 ) { // 2014.03.14
				//
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch + s1p ) ) { // 2007.04.12
					if ( side != side2 ) return 102 + s1p; // 2009.09.18
				}
				//
			} //
			//
			return 100; // 2009.09.15
		}
	}
	else {
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[9][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) {
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[A][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
			s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside );
			res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , tmside , 2002 , tmside + s1p , TRUE , 0 , &side2 , &pointer2 , &order2 );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[B][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
			if ( res != 0 ) {
				//
				*subresult = *subresult + 200;
				//
				//===================================================================================================
				// Wait Until Process Finish // 2006.02.15
				//===================================================================================================
				while( TRUE ) { // 2006.09.13
					//
					j = SCHEDULER_MAKE_LOCK_FREE_for_4( side , tmside , 2 ); /* 2010.12.09 */
					if ( j < 0 ) return SYS_ABORTED;
					//
					_SCH_LOG_DEBUG_PRINTF( side , tmside + 1 , "TM%d STEP LOOP 100 (%d) ch=%d,res=%d,s1p=%d\n" , tmside + 1 , id , ch , res , s1p );
					//==============================================================================================================
					// 2007.02.23
					//==============================================================================================================
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + s1p ) > 0 ) {
						if ( SCHEDULER_CONTROL_PROCESS_MONITOR_Run_with_Properly_Status( ch + s1p , PROCESS_INDICATOR_POST , FALSE ) ) {
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return 0;
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side2 ) ) return 0;
							Sleep(1);
							continue;
						}
					}
					//==============================================================================================================
					if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( TM + s1p , FALSE ) != SYS_RUNNING ) { // 2006.09.13
						if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( PM1 + s1p , FALSE ) != SYS_RUNNING ) { // 2006.09.13
							break; // 2006.09.13
						} // 2006.09.13
					} // 2006.09.13
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return 0; // 2006.09.13
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side2 ) ) return 0; // 2006.09.13
					Sleep(1);
					//-------------------------------------------------------------------
					if ( _SCH_MODULE_Get_PM_WAFER( ch + s1p , 0 ) <= 0 ) { // 2006.10.19
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + s1p ) <= 0 ) break; // 2006.10.19
					} // 2006.10.19
					//-------------------------------------------------------------------
				} // 2006.09.13
				//===================================================================================================
//				if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch + s1p ) ) { // 2007.04.12
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch + s1p ) ) { // 2007.04.12
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[C][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
					if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side2 , ch + s1p ) != 0 ) {
						strcpy( RunRecipe , _SCH_PREPOST_Get_inside_READY_RECIPE_NAME( side2 , ch + s1p ) );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[D][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
						*subresult = *subresult + 10;
					}
					else {
						if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch ) != 0 ) {
							strcpy( RunRecipe , _SCH_PREPOST_Get_inside_READY_RECIPE_NAME( side , ch ) );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[E][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
							*subresult = *subresult + 20;
						}
						else {
							strcpy( RunRecipe , "?" );
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[F][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
							*subresult = *subresult + 30;
						}
					}
					return 99;
				}
				else {
					//
					*subresult = *subresult + 100;
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[G][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
					//====================================================================================
					// 2006.11.30
					//====================================================================================
					pds = _SCH_CLUSTER_Get_PathDo( side , pointer );
					if ( pds != PATHDO_WAFER_RETURN ) {
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , 0 , order , ch , TRUE ) ) { // 2007.04.12
						if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , 0 , order , ch , TRUE ) ) { // 2007.04.12
							strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , 0 , order , 2 ) );
							//
							*subresult = *subresult + 10;
							return 2;
						}
					}
					else {
						*subresult = *subresult + 100;
					}
					//====================================================================================
					pds = _SCH_CLUSTER_Get_PathDo( side2 , pointer2 ); // 2006.10.13
					if ( pds != PATHDO_WAFER_RETURN ) { // 2006.10.13
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[G2][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d][pds=%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 , pds );
//printf( "=====================================================================\n" , tmside );
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , _SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) - 1 , order2 , ch + s1p , TRUE ) ) { // 2006.10.13
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , pds - 1 , order2 , ch + s1p , TRUE ) ) { // 2006.10.13 // 2006.11.27
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch + s1p , TRUE ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
						if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch + s1p , TRUE ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[G3][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
//							strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pointer2 , _SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) - 1 , order2 , 2 ) ); // 2006.10.13
//							strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pointer2 , pds - 1 , order2 , 2 ) ); // 2006.10.13 // 2006.11.27
							strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pointer2 , 0 , order2 , 2 ) ); // 2006.10.13 // 2006.11.27
							//
							*subresult = *subresult + 20;
							return 2;
						}
						else {
							*subresult = *subresult + 30;
						}
					}
					else {
						*subresult = *subresult + 40;
					}
				}
			}
			else {
				//
				*subresult = *subresult + 500;
				//
				//===============================================================================================================================================
				// 2006.09.13
				//===============================================================================================================================================
				if ( s1p == -1 ) {
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[H2][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
					pds = _SCH_CLUSTER_Get_PathDo( side , pointer ); // 2006.10.13
					if ( pds != PATHDO_WAFER_RETURN ) { // 2006.10.13
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1 , order , ch , TRUE ) ) { // 2006.10.13
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , pds - 1 , order , ch , TRUE ) ) { // 2006.10.13 // 2006.11.27
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , 0 , order , ch , TRUE ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
						if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , 0 , order , ch , TRUE ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[H3][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
							SCHEDULER_CONTROL_UTIL_EXTRACT_UPFOLDER_for_4( _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch + s1p ) , imsi );
//							sprintf( RunRecipe , "%s\\%s\\%s" , imsi , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1 , order , 2 ) ); // 2006.10.13
//							sprintf( RunRecipe , "%s\\%s\\%s" , imsi , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , pds - 1 , order , 2 ) ); // 2006.10.13 // 2006.11.27
							sprintf( RunRecipe , "%s\\%s\\%s" , imsi , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , 0 , order , 2 ) ); // 2006.10.13 // 2006.11.27
							//
							*subresult = *subresult + 10;
							//
							*forcespawn = TRUE;
							return 2;
						}
						else {
							//
							*subresult = *subresult + 20;
							//
						}
					}
					else {
						//
						*subresult = *subresult + 30;
						//
					}
				}
				else {
					//========================================================================================
					// 2006.11.30
					//========================================================================================
					pds = _SCH_CLUSTER_Get_PathDo( side , pointer );
					if ( pds != PATHDO_WAFER_RETURN ) {
//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , 0 , order , ch , TRUE ) ) { // 2007.04.12
						if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , 0 , order , ch , TRUE ) ) { // 2007.04.12
							strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , 0 , order , 2 ) );
							//
							*subresult = *subresult + 40;
							//
							return 2;
						}
						else {
							//
							*subresult = *subresult + 50;
							//
						}
					}
					else {
						//
						*subresult = *subresult + 60;
						//
					}
					//========================================================================================
				}
				//===============================================================================================================================================
			}
		}
		else {
			//
			*subresult = *subresult + 700;
			//
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[I][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
			pds = _SCH_CLUSTER_Get_PathDo( side , pointer ); // 2006.10.13
			if ( pds != PATHDO_WAFER_RETURN ) { // 2006.10.13
//				if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1 , order , ch , TRUE ) ) { // 2006.10.13
//				if ( _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , pds - 1 , order , ch , TRUE ) ) { // 2006.10.13 // 2007.04.12
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_CLUSTER_Check_Possible_UsedPre( side , pointer , pds - 1 , order , ch , TRUE ) ) { // 2006.10.13 // 2007.04.12
//printf( "=====================================================================\n" , tmside );
//printf( "=PRE_BEFORE_PLACE=[J][tmside=%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
//					strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 1 , order , 2 ) ); // 2006.10.13
					strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , pds - 1 , order , 2 ) ); // 2006.10.13
					//
					*subresult = *subresult + 10;
					return 2;
				}
				else {
					//
					*subresult = *subresult + 20;
				}
			}
			else {
				//
				*subresult = *subresult + 30;
			}
		}
	}
	return 0;
}


int SCHEDULER_CONTROL_Chk_POST_BEFORE_PLACE_PART( int CHECKING_SIDE , int tmside , int ch , char *RunRecipe , int *sid , int *wid , int id ) { // 2006.01.12 // before Place
	int s1p;
	int res , side2 , pointer2 , order2;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) {
		res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , tmside , 3001 , tmside , FALSE , 0 , &side2 , &pointer2 , &order2 );
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PLACE=[1][tmside=%d][ch=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
		if ( res == 0 ) {
			s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside );
			res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , tmside , 3002 , tmside + s1p , FALSE , 1 , &side2 , &pointer2 , &order2 );
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PLACE=[2][tmside=%d][ch=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
			if ( res != 0 ) {
				//===================================================================================================
				// Wait Until Process Finish // 2006.02.15
				//===================================================================================================
				while( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + s1p ) > 0 ) {
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , tmside + 1 , "TM%d STEP LOOP 101 (%d) ch=%d,res=%d,s1p=%d\n" , tmside + 1 , id , ch , res , s1p );
					if ( _SCH_MODULE_Get_PM_WAFER( ch + s1p , 0 ) <= 0 ) break; // 2006.09.13
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side2 ) ) return -1;
					Sleep(1);
				}
				//===================================================================================================
//				if ( _SCH_CLUSTER_Check_Possible_UsedPost( side2 , pointer2 , 0 , order2 ) ) { // 2007.04.12
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_CLUSTER_Check_Possible_UsedPost( side2 , pointer2 , 0 , order2 ) ) { // 2007.04.12
					strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pointer2 , 0 , order2 , 1 ) );
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PLACE=[3][tmside=%d][ch=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
					*sid = side2; // 2006.11.30
					*wid = _SCH_CLUSTER_Get_SlotIn( side2 , pointer2 );
					return s1p;
				}
				else {
					if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_CLUSTER_Check_Possible_UsedPost2( side2 , pointer2 , ch , 0 , &order2 ) ) { // 2007.04.12
						strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pointer2 , 0 , order2 , 1 ) );
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PLACE=[4][tmside=%d][ch=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
						*sid = side2; // 2006.11.30
						*wid = _SCH_CLUSTER_Get_SlotIn( side2 , pointer2 );
						return s1p;
					}
					else {
						if ( _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , ch + s1p ) ) {
							strcpy( RunRecipe , "?" );
							*sid = side2; // 2006.11.30
							*wid = _SCH_CLUSTER_Get_SlotIn( side2 , pointer2 );
							return s1p;
						}
					}
				}
			}
		}
	}
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PLACE=[5][tmside=%d][ch=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , ch - PM1 + 1 , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
	return 0;
}

int SCHEDULER_CONTROL_Chk_POST_BEFORE_PICK_PART( int tmside , int side , int pointer , int ch , int order , char *RunRecipe , int *switchdata , int *forcespawn ) { // 2006.01.12 // after pick
	int pres , s1p , pds;
	int res , side2 , pointer2 , order2;
	char imsi[64];

	*forcespawn = FALSE;
	pres = FALSE;
	//=======================================================================================
	// 2006.12.04
	//=======================================================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) {
		s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside );
	}
	else {
		s1p = 0;
	}
	//=======================================================================================
	if ( *switchdata < 100 ) {
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) {
			//===================================================================================================
			// Wait Until Process Finish // 2006.02.15
			//===================================================================================================
//			s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside ); // 2006.12.04
			while( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + s1p ) > 0 ) {
				_SCH_LOG_DEBUG_PRINTF( side , tmside + 1 , "TM%d STEP LOOP 102 ch=%d,s1p=%d,pres=%d\n" , tmside + 1 , ch , s1p , pres );
				if ( _SCH_MODULE_Get_PM_WAFER( ch + s1p , 0 ) <= 0 ) break; // 2006.09.13
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return FALSE;
				Sleep(1);
			}
			//===================================================================================================
		}
		pds = _SCH_CLUSTER_Get_PathDo( side , pointer ); // 2006.10.13
		if ( pds != PATHDO_WAFER_RETURN ) { // 2006.10.13
//			if ( _SCH_CLUSTER_Check_Possible_UsedPost( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 2 , order ) ) { // 2006.10.13
//			if ( _SCH_CLUSTER_Check_Possible_UsedPost( side , pointer , pds - 2 , order ) ) { // 2006.10.13 // 2006.11.27
//			if ( _SCH_CLUSTER_Check_Possible_UsedPost( side , pointer , 0 , order ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
			if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_CLUSTER_Check_Possible_UsedPost( side , pointer , 0 , order ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
//				strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 2 , order , 1 ) ); // 2006.10.13
//				strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , pds - 2 , order , 1 ) ); // 2006.10.13 // 2006.11.27
				strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , 0 , order , 1 ) ); // 2006.10.13 // 2006.11.27
				pres = TRUE;
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[1][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) { // 2006.09.13
					if ( s1p == -1 ) {
						if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , ch + s1p ) ) {
							if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , tmside , 4001 , tmside + s1p , FALSE , 2 , &side2 , &pointer2 , &order2 ) == 0 ) {
								if ( !_SCH_CLUSTER_Check_Possible_UsedPost2( side , pointer , ch + s1p , 0 , &order2 ) ) { // 2006.10.13 // 2006.11.27
									SCHEDULER_CONTROL_UTIL_EXTRACT_UPFOLDER_for_4( _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch + s1p ) , imsi );
//									sprintf( RunRecipe , "%s\\%s\\%s" , imsi , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 2 , order , 1 ) ); // 2006.10.13
//									sprintf( RunRecipe , "%s\\%s\\%s" , imsi , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , pds - 2 , order , 1 ) ); // 2006.10.13 // 2006.11.27
									sprintf( RunRecipe , "%s\\%s\\%s" , imsi , _SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , 0 , order , 1 ) ); // 2006.10.13 // 2006.11.27
									*forcespawn = TRUE;
								}
							}
						}
					}
				}
			}
			else {
				if ( _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , ch ) ) {
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[2][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
					strcpy( RunRecipe , "?" );
					pres = TRUE;
				}
				else {
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[3][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) {
//						s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + tmside ); // 2006.12.04
						res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , tmside , 4002 , tmside + s1p , FALSE , 2 , &side2 , &pointer2 , &order2 );
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[4][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
						if ( res != 0 ) {
							//===================================================================================================
							// Wait Until Process Finish // 2006.02.15
							//===================================================================================================
							while( _SCH_MACRO_CHECK_PROCESSING_INFO( ch + s1p ) > 0 ) {
								_SCH_LOG_DEBUG_PRINTF( side , tmside + 1 , "TM%d STEP LOOP 103 ch=%d,res=%d,s1p=%d,pres=%d\n" , tmside + 1 , ch , res , s1p , pres );
								if ( _SCH_MODULE_Get_PM_WAFER( ch + s1p , 0 ) <= 0 ) break; // 2006.09.13
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return FALSE;
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side2 ) ) return FALSE;
								Sleep(1);
							}
							//===================================================================================================
//							if ( _SCH_CLUSTER_Check_Possible_UsedPost( side2 , pointer2 , 0 , order2 ) ) { // 2007.04.12
							if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_CLUSTER_Check_Possible_UsedPost( side2 , pointer2 , 0 , order2 ) ) { // 2007.04.12
								strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side2 , pointer2 , 0 , order2 , 1 ) );
								pres = TRUE;
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[5][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
							}
							else {
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[6][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
								if ( _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , ch + s1p ) ) {
									strcpy( RunRecipe , "?" );
									pres = TRUE;
								}
							}
						}
						else {
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[7][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
							if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_CLUSTER_Check_Possible_UsedPost2( side , pointer , ch + s1p , 0 , &order2 ) ) { // 2006.10.13 // 2006.11.27 // 2007.04.12
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[8][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
//								strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , _SCH_CLUSTER_Get_PathDo( side , pointer ) - 2 , order2 , 1 ) ); // 2006.10.13
//								strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , pds - 2 , order2 , 1 ) ); // 2006.10.13 // 2006.11.27
								strcpy( RunRecipe , _SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , 0 , order2 , 1 ) ); // 2006.10.13 // 2006.11.27
								pres = TRUE;
							}
							else {
	//printf( "=====================================================================\n" , tmside );
	//printf( "=POST_BEFORE_PICK=[9][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
	//printf( "=====================================================================\n" , tmside );
								if ( _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , ch + s1p ) ) {
									strcpy( RunRecipe , "?" );
									pres = TRUE;
								}
							}
						}
					}
				}
			}
		}
		if ( pres && ( *switchdata == 1 ) ) {
			*switchdata = 2;
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PICK=[A][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
		}
	}
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PICK=[B][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
	if ( *switchdata != 0 ) {
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) {
			res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , tmside , 4003 , tmside + s1p , TRUE , 0 , &side2 , &pointer2 , &order2 );
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PICK=[C][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
			if ( res != 0 ) {
//				if ( side2 != side ) { // 2006.11.27
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PICK=[D][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
//				if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch ) ) { // 2006.12.04
//				if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch + s1p ) ) { // 2006.12.04 // 2007.04.12
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch + s1p ) ) { // 2006.12.04 // 2007.04.12
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PICK=[E][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
						*switchdata = 0;
					}
//				} // 2006.11.27
				//==================================================================================
				// 2006.11.27
				//==================================================================================
					else {
						pds = _SCH_CLUSTER_Get_PathDo( side2 , pointer2 );
						if ( pds != PATHDO_WAFER_RETURN ) {
//							if ( _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch , FALSE ) ) *switchdata = 0; // 2006.12.04
//							if ( _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch + s1p , FALSE ) ) *switchdata = 0; // 2006.12.04 // 2007.04.12
							if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch + s1p ) && _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch + s1p , FALSE ) ) *switchdata = 0; // 2006.12.04 // 2007.04.12
						}
					}
				//==================================================================================
				//==================================================================================
			}
		}
		//==================================================================================
		// 2006.11.27
		//==================================================================================
		if ( *switchdata != 0 ) {
			res = SCHEDULER_CONTROL_Chk_WILLDONE_PART( side , tmside , 4004 , tmside , TRUE , 0 , &side2 , &pointer2 , &order2 );
			if ( res != 0 ) {
//				if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch ) ) { // 2007.04.12
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side2 , ch ) ) { // 2007.04.12
					*switchdata = 0;
				}
				else {
					//=====================================================================================================================
					// 2007.07.23
					//=====================================================================================================================
					if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_SUB_Check_Last_And_Current_CPName_is_Different( side2 , pointer2 , ch ) ) { // 2007.07.23
						*switchdata = 2;
					}
					//=====================================================================================================================
					else {
						pds = _SCH_CLUSTER_Get_PathDo( side2 , pointer2 );
						if ( pds != PATHDO_WAFER_RETURN ) {
	//						if ( _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch , FALSE ) ) *switchdata = 0; // 2007.04.12
							if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , ch ) && _SCH_CLUSTER_Check_Possible_UsedPre( side2 , pointer2 , 0 , order2 , ch , FALSE ) ) *switchdata = 0; // 2007.04.12
						}
					}
				}
			}
		}
		//==================================================================================
		//==================================================================================
	}
//printf( "=====================================================================\n" , tmside );
//printf( "=POST_BEFORE_PICK=[F][tmside=%d][side=%d,%d][ch=%d][order=%d][rcp=%s][s1p=%d][%d,%d,%d,%d]==\n" , tmside , side , pointer , ch - PM1 + 1 , order , RunRecipe , s1p , res , side2 , pointer2 , order2 );
//printf( "=====================================================================\n" , tmside );
	return pres;
}

//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================

//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================


#define MAKE_SCHEDULER_HEAD_PRE_END_DUMMY_for_4( ZZZ ) \
\
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY\n" , ##ZZZ## + 1 ); \
\
	if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) >= 2 ) { \
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 1\n" , ##ZZZ## + 1 ); \
\
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + ##ZZZ## ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + ##ZZZ## ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 2\n" , ##ZZZ## + 1 ); \
\
			if ( _SCH_MODULE_Get_Mdl_Run_Flag( PM1 + ##ZZZ## ) <= 1 ) { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 3\n" , ##ZZZ## + 1 ); \
\
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 4\n" , ##ZZZ## + 1 ); \
\
					if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) <= 0 ) { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 5\n" , ##ZZZ## + 1 ); \
\
						if ( ( _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , 0 ) != CHECKING_SIDE ) || ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , 0 ) <= 0 ) ) { \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 6\n" , ##ZZZ## + 1 ); \
\
							if ( ( _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , 1 ) != CHECKING_SIDE ) || ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , 1 ) <= 0 ) ) { \
\
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 7\n" , ##ZZZ## + 1 ); \
\
								if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_for_STYLE_4( BM1 + ##ZZZ## , TRUE ) ) { /* 2006.06.01 2006.06.13 */ \
\
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 8\n" , ##ZZZ## + 1 ); \
\
									if ( SCHEDULER_CONTROL_Pre_End_Possible_Check( CHECKING_SIDE , PM1 + ##ZZZ## , TRUE ) ) { /* 2006.09.13 */ \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 9\n" , ##ZZZ## + 1 ); \
\
										if ( _SCH_PREPOST_Pre_End_Force_Part( CHECKING_SIDE , PM1 + ##ZZZ## ) ) { \
\
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 10\n" , ##ZZZ## + 1 ); \
\
											if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_PM_ENDMONITOR( CHECKING_SIDE , PM1 + ##ZZZ## , &pt , 2 ) ) { \
\
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 11\n" , ##ZZZ## + 1 ); \
\
\
												if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
												SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( PM1 + ##ZZZ## , FALSE ); /* 2005.09.23 */ \
												SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , CHECKING_SIDE , PM1 + ##ZZZ## ) , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
												_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
														 PM1 + ##ZZZ## , \
														 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pt % 100) + 1 , 0 , \
														 RunRecipe , \
														 2 , \
														 2 , "E" , PROCESS_DEFAULT_MINTIME , \
														 0 , 402 ); \
												SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
					else { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 21\n" , ##ZZZ## + 1 ); \
\
						if ( SCHEDULING_Possible_Pump_Force_BM_for_STYLE_4( CHECKING_SIDE , BM1 + ##ZZZ## ) ) { \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 22\n" , ##ZZZ## + 1 ); \
\
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) == -1 ) { \
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted with Part 21 ....%cWHTM%dFAIL\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); \
								return SYS_ABORTED; \
							} \
\
\
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
							SCHEDULER_CONTROL_BM_PUMPING_TM_PART_##ZZZ##( CHECKING_SIDE , BM1 + ##ZZZ## , 1 ); \
						} \
					} \
				} \
			} \
			else { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 31\n" , ##ZZZ## + 1 ); \
\
				if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) > 0 ) { \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 32\n" , ##ZZZ## + 1 ); \
\
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 33\n" , ##ZZZ## + 1 ); \
\
						if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 34\n" , ##ZZZ## + 1 ); \
\
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 35\n" , ##ZZZ## + 1 ); \
\
								if ( SCHEDULING_Possible_Pump_Force_BM_for_STYLE_4( CHECKING_SIDE , BM1 + ##ZZZ## ) ) { \
\
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 36\n" , ##ZZZ## + 1 ); \
\
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) == -1 ) { \
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted with Part 22 ....%cWHTM%dFAIL\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); \
										return SYS_ABORTED; \
									} \
									if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 37\n" , ##ZZZ## + 1 ); \
\
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 38\n" , ##ZZZ## + 1 ); \
\
											if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) || ( !SCHEDULING_Possible_RUNNING_for_STYLE_4() ) ) { /* 2007.08.27 */ \
\
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 39\n" , ##ZZZ## + 1 ); \
\
\
\
												if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
												SCHEDULER_CONTROL_BM_PUMPING_TM_PART_##ZZZ##( CHECKING_SIDE , BM1 + ##ZZZ## , 2 ); \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
				else { /* 2006.11.22 */ \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 41\n" , ##ZZZ## + 1 ); \
\
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 42\n" , ##ZZZ## + 1 ); \
\
						if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) <= 0 ) { \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 43\n" , ##ZZZ## + 1 ); \
\
							if ( ( _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , 0 ) != CHECKING_SIDE ) || ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , 0 ) <= 0 ) ) { \
\
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 44\n" , ##ZZZ## + 1 ); \
\
								if ( ( _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , 1 ) != CHECKING_SIDE ) || ( _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , 1 ) <= 0 ) ) { \
\
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 45\n" , ##ZZZ## + 1 ); \
\
									if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_for_STYLE_4( BM1 + ##ZZZ## , TRUE ) ) { \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 46\n" , ##ZZZ## + 1 ); \
\
										if ( SCHEDULER_CONTROL_Pre_End_Possible_Check( CHECKING_SIDE , PM1 + ##ZZZ## , TRUE ) ) { \
\
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 47\n" , ##ZZZ## + 1 ); \
\
\
\
											if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
											if ( _SCH_PREPOST_Pre_End_Force_Part( CHECKING_SIDE , PM1 + ##ZZZ## ) ) { \
\
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 48\n" , ##ZZZ## + 1 ); \
\
											} \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
			} \
		} \
		else { \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 51\n" , ##ZZZ## + 1 ); \
\
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) { /* 2007.04.16 */ \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 52\n" , ##ZZZ## + 1 ); \
\
				if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 53\n" , ##ZZZ## + 1 ); \
\
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 54\n" , ##ZZZ## + 1 ); \
\
						if ( _SCH_MODULE_Get_Mdl_Run_Flag( PM1 + ##ZZZ## ) > 0 ) { \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 55\n" , ##ZZZ## + 1 ); \
\
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 56\n" , ##ZZZ## + 1 ); \
\
								if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) > 0 ) { \
\
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 57\n" , ##ZZZ## + 1 ); \
\
									if ( SCHEDULING_Possible_Pump_Force_BM_for_STYLE_4( CHECKING_SIDE , BM1 + ##ZZZ## ) ) { \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 58\n" , ##ZZZ## + 1 ); \
\
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) == -1 ) { \
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted with Part 23 ....%cWHTM%dFAIL\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); \
											return SYS_ABORTED; \
										} \
										if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 59\n" , ##ZZZ## + 1 ); \
\
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 60\n" , ##ZZZ## + 1 ); \
\
\
												if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
\
												SCHEDULER_CONTROL_BM_PUMPING_TM_PART_##ZZZ##( CHECKING_SIDE , BM1 + ##ZZZ## , 3 ); \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
				} \
			} \
		} \
	} \
	else { \
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 71\n" , ##ZZZ## + 1 ); \
\
		if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 72\n" , ##ZZZ## + 1 ); \
\
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 73\n" , ##ZZZ## + 1 ); \
\
				if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + ##ZZZ## ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + ##ZZZ## ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 74\n" , ##ZZZ## + 1 ); \
\
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 75\n" , ##ZZZ## + 1 ); \
\
						if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) > 0 ) { \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 76\n" , ##ZZZ## + 1 ); \
\
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) == -1 ) { \
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted with Part 24 ....%cWHTM%dFAIL\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); \
								return SYS_ABORTED; \
							} \
							else { /* 2006.05.02 */ \
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) { /* 2007.05.11 */ \
\
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 77\n" , ##ZZZ## + 1 ); \
\
\
\
									if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
									switch ( SCHEDULING_Possible_Pump_Disable_Force_BM_for_STYLE_4( CHECKING_SIDE , BM1 + ##ZZZ## ) ) { \
									case 0 : \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 78\n" , ##ZZZ## + 1 ); \
\
										if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
												SCHEDULER_CONTROL_BM_PUMPING_TM_PART_##ZZZ##( CHECKING_SIDE , BM1 + ##ZZZ## , 4 ); \
											} \
										} \
										break; \
									case 1 : \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 79\n" , ##ZZZ## + 1 ); \
\
										if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
												SCHEDULER_CONTROL_BM_VENTING_PART_##ZZZ##( CHECKING_SIDE , FALSE , BM1 + ##ZZZ## , 4 ); \
											} \
										} \
										break; \
									default : /* 2006.07.21 */ \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 80\n" , ##ZZZ## + 1 ); \
\
										if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 81\n" , ##ZZZ## + 1 ); \
\
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
												_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 82\n" , ##ZZZ## + 1 ); \
\
												if ( SCHEDULING_Possible_Pump_LotFirst_Lock_Force_BM_for_STYLE_4( CHECKING_SIDE , BM1 + ##ZZZ## ) == 0 ) { \
\
													_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 83\n" , ##ZZZ## + 1 ); \
\
													if ( !SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) { /* 2007.05.21 */ \
\
														_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 84\n" , ##ZZZ## + 1 ); \
\
														if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + ##ZZZ## ) <= 0 ) { /* 2007.05.21 */ \
\
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 85\n" , ##ZZZ## + 1 ); \
\
\
\
															if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
															SCHEDULER_CONTROL_BM_PUMPING_TM_PART_##ZZZ##( CHECKING_SIDE , BM1 + ##ZZZ## , 5 ); \
														} \
													} \
												} \
											} \
										} \
										break; \
									} \
								} \
							} \
						} \
					} \
				} \
			} \
		} \
	} \
\
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 99\n" , ##ZZZ## + 1 ); \
\
	if ( _SCH_MODULE_Get_Mdl_Run_Flag( PM1 + ##ZZZ## ) <= 0 ) { /* 2006.01.24 */ \
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 101\n" , ##ZZZ## + 1 ); \
\
		if ( ( sch4_RERUN_END_S1_TAG[ PM1 + ##ZZZ## ] == -1 ) && ( sch4_RERUN_END_S2_TAG[ PM1 + ##ZZZ## ] == -1 ) ) { \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 102\n" , ##ZZZ## + 1 ); \
\
			if ( ( sch4_RERUN_FST_S1_TAG[ PM1 + ##ZZZ## ] != -1 ) || ( sch4_RERUN_FST_S2_TAG[ PM1 + ##ZZZ## ] != -1 ) ) { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 103\n" , ##ZZZ## + 1 ); \
\
				sch4_RERUN_FST_S1_TAG[ PM1 + ##ZZZ## ] = -1; \
				sch4_RERUN_FST_S2_TAG[ PM1 + ##ZZZ## ] = -1; \
			} \
		} \
	} \
	if ( sch4_RERUN_END_S1_TAG[ PM1 + ##ZZZ## ] != -1 ) { \
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 104\n" , ##ZZZ## + 1 ); \
\
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 105\n" , ##ZZZ## + 1 ); \
\
			if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) <= 0 ) { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 106\n" , ##ZZZ## + 1 ); \
\
\
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
\
				pt = sch4_RERUN_END_S1_TAG[ PM1 + ##ZZZ## ]; \
				SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( PM1 + ##ZZZ## , FALSE ); /* 2005.09.23 */ \
				SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , sch4_RERUN_END_S1_ETC[ PM1 + ##ZZZ## ] , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
				_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
						 PM1 + ##ZZZ## , \
						 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pt % 100) + 1 , 0 , \
						 RunRecipe , \
						 2 , \
						 2 , "E" , PROCESS_DEFAULT_MINTIME , \
						 0 , 403 ); \
				SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( PM1 + ##ZZZ## ); \
				sch4_RERUN_END_S1_TAG[ PM1 + ##ZZZ## ] = -1; \
			} \
		} \
	} \
	else if ( ( sch4_RERUN_FST_S1_TAG[ PM1 + ##ZZZ## ] != -1 ) && ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( PM1 + ##ZZZ## ) ) ) { \
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 111\n" , ##ZZZ## + 1 ); \
\
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + ##ZZZ## ) <= 0 ) { \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 112\n" , ##ZZZ## + 1 ); \
\
			if ( _SCH_MODULE_Get_PM_WAFER( PM1 + ##ZZZ## , 0 ) <= 0 ) { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 113\n" , ##ZZZ## + 1 ); \
\
\
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
\
				pt = sch4_RERUN_FST_S1_TAG[ PM1 + ##ZZZ## ]; \
				SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( PM1 + ##ZZZ## , TRUE ); /* 2005.09.23 */ \
				SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , sch4_RERUN_FST_S1_ETC[ PM1 + ##ZZZ## ] , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
				SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( sch4_RERUN_FST_S1_ETC[ PM1 + ##ZZZ## ] , PM1 + ##ZZZ## ); \
				SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( PM1 + ##ZZZ## , sch4_RERUN_FST_S1_ETC[ PM1 + ##ZZZ## ] ); /* 2006.01.24 */ \
				_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
						 PM1 + ##ZZZ## , \
						 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (pt % 100) + 1 , 0 , \
						 RunRecipe , \
						 2 , \
						 1 , "F" , PROCESS_DEFAULT_MINTIME , \
						 0 , 404 ); \
				SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( PM1 + ##ZZZ## ); \
				sch4_RERUN_FST_S1_TAG[ PM1 + ##ZZZ## ] = -1; \
			} \
		} \
	} \
\
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PRE_END_DUMMY 999\n" , ##ZZZ## + 1 );

//=====================================================================================================================================================================
//=====================================================================================================================================================================
BOOL SCHEDULER_CONTROL_SDM_4_NO_MORE_WAFER( int side , int tms , int pmc ) { // 2010.09.02
//	return _SCH_SUB_FM_Another_No_More_Supply( side , pmc , -1 , -1 ); /* 2010.10.01 */
	return _SCH_SUB_FM_Another_No_More_Supply( -1 , pmc , -1 , -1 ); /* 2010.10.01 */
}
//=====================================================================================================================================================================
//=====================================================================================================================================================================
int SCHEDULER_MAKE_LOCK_FREE_for_4( int side , int TMSIDE , int waittm ) { // 2010.11.10
	int i , Mode1 , Mode2 , var;
	int checktimesec , runtime;
	//
//	if ( ( TMSIDE % 2 ) != 0 ) return 1;
	if ( ( TMSIDE % 2 ) == 0 ) {
		var = 1;
	}
	else {
		if ( waittm == 0 ) return 1;
		var = -1;
	}
	//
	if ( ( var == 1 ) && ( waittm != 2 ) ) {
		//
		checktimesec = _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( TMSIDE + PM1 );
		//
		if ( checktimesec <= 0 ) {
			checktimesec = _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( TMSIDE + PM1 + var );
			if ( checktimesec <= 0 ) {
				return 2;
			}
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 1 ) return 3;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) != 2 ) return 4;
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( TMSIDE + PM1 , TRUE , -1 ) ) return 11;
			if ( !_SCH_MODULE_GET_USE_ENABLE( TMSIDE + PM1 + var , TRUE , -1 ) ) return 12;
			//
			/*
			// 2011.04.22
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 ) ) return 21;
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 + var ) ) return 22;
			if ( _SCH_MODULE_Get_BM_FULL_MODE( TMSIDE + BM1 ) != BUFFER_TM_STATION ) return 31;
			if ( _SCH_MODULE_Get_BM_FULL_MODE( TMSIDE + BM1 + var ) != BUFFER_TM_STATION ) return 32;
			*/
			//----------------------------------------------
			// 2011.04.22
			//----------------------------------------------
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 ) ) {
				if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( TMSIDE + BM1 , TRUE ) ) return 21;
			}
			else {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( TMSIDE + BM1 ) != BUFFER_TM_STATION ) return 31;
			}
			//
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 + var ) ) {
				if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( TMSIDE + BM1 + var , TRUE ) ) return 22;
			}
			else {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( TMSIDE + BM1 + var ) != BUFFER_TM_STATION ) return 32;
			}
			//----------------------------------------------
			//
			if ( _SCH_MODULE_Get_PM_WAFER( TMSIDE + PM1 , 0 ) > 0 ) return 41;
			if ( _SCH_MODULE_Get_PM_WAFER( TMSIDE + PM1 + var , 0 ) > 0 ) return 42;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( TMSIDE + PM1 ) <= 0 ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( TMSIDE + PM1 + var ) <= 0 ) {
					return 51;
				}
			}
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( TMSIDE + BM1 , BUFFER_INWAIT_STATUS ) <= 0 ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( TMSIDE + BM1 + var , BUFFER_INWAIT_STATUS ) <= 0 ) {
					if ( _SCH_MODULE_Get_TM_WAFER( TMSIDE , SCHEDULER_CONTROL_Chk_ARM_WHAT( TMSIDE , CHECKORDER_FOR_FM_PICK ) ) <= 0 ) {
						if ( _SCH_MODULE_Get_TM_WAFER( TMSIDE + var , SCHEDULER_CONTROL_Chk_ARM_WHAT( TMSIDE + var , CHECKORDER_FOR_FM_PICK ) ) <= 0 ) {
							return 61;
						}
					}
				}
			}
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( TMSIDE + BM1 , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( TMSIDE + BM1 + var , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
					return 71;
				}
			}
			//
			if ( SCHEDULER_CONTROL_Chk_FM_Run_Locking() ) return 81;
			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) return 82;
	//		if ( !SCHEDULER_CONTROL_SDM_4_NO_MORE_WAFER( -1 , TMSIDE , -1 ) ) return 83; // 2010.11.29
			//
			if ( i == 0 ) Sleep(1);
			//
		}
		//
		Mode1 = _SCH_MACRO_CHECK_PROCESSING_INFO( TMSIDE + PM1 );
		Mode2 = _SCH_MACRO_CHECK_PROCESSING_INFO( TMSIDE + PM1 + var );
		//
		if      ( Mode1 > 0 ) {
			if      ( Mode1 >= PROCESS_INDICATOR_POST ) {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 1 , TMSIDE + PM1 );
				if ( runtime < checktimesec ) return 91;
			}
			else if ( Mode1 >= PROCESS_INDICATOR_PRE ) {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 2 , TMSIDE + PM1 );
				if ( runtime < checktimesec ) return 92;
			}
			else {
				return 93;
			}
		} 
		else if ( Mode2 > 0 ) {
			if      ( Mode2 >= PROCESS_INDICATOR_POST ) {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 1 , TMSIDE + PM1 + var );
				if ( runtime < checktimesec ) return 94;
			}
			else if ( Mode2 >= PROCESS_INDICATOR_PRE ) {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 2 , TMSIDE + PM1 + var );
				if ( runtime < checktimesec ) return 95;
			}
			else {
				return 96;
			}
		}
		else {
			return 97;
		}
		//
		if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return -1; /* 2017.04.11 */
		//
		if      ( TMSIDE == 0 ) {
			SCHEDULER_CONTROL_BM_VENTING_PART_0( side , FALSE , TMSIDE + BM1 , 101 );
			SCHEDULER_CONTROL_BM_VENTING_PART_1( side , FALSE , TMSIDE + BM1 + var , 101 );
		}
		else if ( TMSIDE == 2 ) {
			SCHEDULER_CONTROL_BM_VENTING_PART_2( side , FALSE , TMSIDE + BM1 , 101 );
			SCHEDULER_CONTROL_BM_VENTING_PART_3( side , FALSE , TMSIDE + BM1 + var , 101 );
		}
		else if ( TMSIDE == 4 ) {
	//		SCHEDULER_CONTROL_BM_VENTING_PART_4( side , FALSE , TMSIDE + BM1 , 101 );
	//		SCHEDULER_CONTROL_BM_VENTING_PART_5( side , FALSE , TMSIDE + BM1 + var , 101 );
		}
	}
	//
	if ( waittm != 0 ) {
		//
		while ( TRUE ) {
			//
			if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 ) ) {
				if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 + var ) ) {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( TMSIDE + BM1 ) == BUFFER_TM_STATION ) {
						if ( _SCH_MODULE_Get_BM_FULL_MODE( TMSIDE + BM1 + var ) == BUFFER_TM_STATION ) {
							if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 ) ) {
								if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( TMSIDE + BM1 + var ) ) {
									//
									break;
									//
								}
							}
						}
					}
				}
			}
			//
			if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) {
				return -1;
			}
			//
			Sleep(1);
			//
_SCH_LOG_DEBUG_PRINTF( side , TMSIDE + 1 , "TM%d MAKE_LOCK_FREE [1] waittm=%d\n" , TMSIDE + 1 , waittm ); // 2014.03.12
			//
		}
		//
	}
	//
	return 0;
}
//=====================================================================================================================================================================
//=====================================================================================================================================================================
#define MAKE_SCHEDULER_PICK_FROM_PM_PART_for_4( ZZZ ) \
		pickskip = FALSE; \
		runaction = FALSE; \
		SCH_No = PM1 + ##ZZZ##; \
\
\
\
		if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_FROM_PM SCH_No=%d\n" , ##ZZZ## + 1 , SCH_No ); \
\
		if ( SCHEDULER_CONTROL_Chk_PM_PICK_POSSIBLE_FOR_4_##ZZZ##( CHECKING_SIDE , _SCH_MODULE_Get_PM_POINTER( SCH_No , 0 ) , SCH_No , &NextYes , &NextPMYes ) ) { \
			if ( ##ZZZ## == 0 ) _SCH_SYSTEM_USING_SET( CHECKING_SIDE , 15 ); \
			runaction = TRUE; \
			dummypost = FALSE; \
			SCH_Out_Not_Run = 0; \
			pt = _SCH_MODULE_Get_PM_POINTER( SCH_No , 0 ); \
			SCH_Order = _SCH_MODULE_Get_PM_PATHORDER( SCH_No , 0 ); \
			switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) ) { \
			case -1 : \
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 ); \
				return SYS_ABORTED; \
				break; \
			case -2 : \
				break; \
			case -3 : \
				SCH_Out_Not_Run = 1; \
				break; \
			case -4 : \
				SCH_Type = SCHEDULER_MOVING_OUT; \
				break; \
			case -5 : \
				SCH_Type = SCHEDULER_MOVING_OUT; \
				SCH_Out_Not_Run = 1; \
				break; \
			} \
			SCH_Slot = 1; \
			wsa = _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ); \
			wsb = SCH_Slot; \
			if ( pt < 100 ) { \
				if ( _SCH_CLUSTER_Get_FailOut( CHECKING_SIDE , pt ) == 1 ) { \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( CHECKING_SIDE , SCH_No ); \
				} \
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_WAITING ); \
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt ); \
				FM_CM = _SCH_CLUSTER_Get_PathIn(CHECKING_SIDE,pt); \
			} \
			else { \
				FM_CM = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); \
			} \
			cf = SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## , CHECKORDER_FOR_TM_PICK ); \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_FROM_PM 1 SCH_No=%d,wsa=%d,pt=%d,cf=%d\n" , ##ZZZ## + 1 , SCH_No , wsa , pt , cf ); \
\
			if ( pt >= 100 ) { \
				switch( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( pt ) ) { \
				case 2 : \
				case 4 : \
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTEND ) ) { \
						NextYes = 0; \
						dummypost = TRUE; \
						sch4_RERUN_END_S3_TAG[ SCH_No ] = TRUE; /* 2005.11.11 */ \
					} \
					break; \
				case 1 : \
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTFIRST ) ) { \
						NextYes = 0; \
						dummypost = TRUE; \
						sch4_RERUN_END_S3_TAG[ SCH_No ] = TRUE; /* 2005.11.11 */ \
					} \
					break; \
				case 3 : \
					if ( SCHEDULER_CONTROL_Chk_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTRUN , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTRUN ) ) { \
						NextYes = 0; \
						dummypost = TRUE; \
						sch4_RERUN_END_S3_TAG[ SCH_No ] = TRUE; /* 2005.11.11 */ \
					} \
					break; \
				default : \
					break; \
				} \
			} \
			if ( sch4_RERUN_END_S1_TAG[ SCH_No ] != -1 ) { \
				NextYes = 0; \
			} \
			if ( ( sch4_RERUN_FST_S1_TAG[ SCH_No ] != -1 ) && ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( SCH_No ) ) ) { \
				NextYes = 0; \
			} \
			if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_LOTFIRST( SCH_No ) ) { /* 2006.08.01 */ \
				NextYes = 0; \
			} \
			if ( pt < 100 ) { \
				pres = SCHEDULER_CONTROL_Chk_POST_BEFORE_PICK_PART( ##ZZZ## , CHECKING_SIDE , pt , SCH_No , SCH_Order , RunRecipe , &NextYes , &forcespawn ); \
			} \
			swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( NextYes , 0 ); \
			if ( _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PICK + 110 , CHECKING_SIDE , pt , SCH_No , cf , wsa , wsb , NextYes , -1 , FALSE , swmode , -1 , -1 ) == SYS_ABORTED ) { /* 2005.12.05 */ \
				sch4_RERUN_END_S3_TAG[ SCH_No ] = FALSE; /* 2005.11.11 */ \
				return SYS_ABORTED; \
			} \
\
\
			if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
			if ( pt < 100 ) { \
				_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
				_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
				_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); \
				_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 ); \
				if ( SCH_Out_Not_Run == 0 ) { \
					if ( NextYes >= 100 ) { \
						if ( NextPMYes == 0 ) { \
							if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) >= 1 ) { \
								if ( SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( CHECKING_SIDE , SCH_No , TRUE ) ) { /* 2005.12.05 */ \
									if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER_DELETE( CHECKING_SIDE , NextYes ) ) { \
									} \
								} \
								if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) { \
									if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_No ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_No ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
										if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_No ) <= 1 ) { \
											if ( !SCHEDULER_CONTROL_Chk_OTHER_WILL_GO_THIS_PM( CHECKING_SIDE , ##ZZZ## ) ) { \
												if ( SCHEDULER_CONTROL_Pre_End_Possible_Check( CHECKING_SIDE , SCH_No , FALSE ) ) { /* 2006.09.13 */ \
													if ( _SCH_PREPOST_Pre_End_Force_Part( CHECKING_SIDE , SCH_No ) ) { \
														if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( CHECKING_SIDE , NextYes ) ) { \
														} \
													} \
												} \
											} \
										} \
									} \
								} \
								else { \
									if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) != ( SCH_No - PM1 + BM1 ) ) { \
										if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_No ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_No ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
											if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_No ) <= 1 ) { \
												if ( !SCHEDULER_CONTROL_Chk_OTHER_WILL_GO_THIS_PM( CHECKING_SIDE , ##ZZZ## ) ) { \
													if ( SCHEDULER_CONTROL_Pre_End_Possible_Check( CHECKING_SIDE , SCH_No , FALSE ) ) { /* 2006.09.13 */ \
														if ( _SCH_PREPOST_Pre_End_Force_Part( CHECKING_SIDE , SCH_No ) ) { \
															if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( CHECKING_SIDE , NextYes ) ) { \
															} \
														} \
													} \
												} \
											} \
										} \
									} \
								} \
							} \
							else { /* 2006.08.30 */ \
								if ( !SCHEDULER_CONTROL_Check_Chamber_Run_Wait_for_STYLE_4_Part( CHECKING_SIDE , SCH_No ) ) { \
									if ( SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( CHECKING_SIDE , SCH_No , TRUE ) ) { /* 2005.12.05 */ \
										if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER_DELETE( CHECKING_SIDE , NextYes ) ) { \
										} \
									} \
									if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) != ( SCH_No - PM1 + BM1 ) ) { \
										if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_No ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_No ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
											if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_No ) <= 1 ) { \
												if ( !SCHEDULER_CONTROL_Chk_OTHER_WILL_GO_THIS_PM( CHECKING_SIDE , ##ZZZ## ) ) { \
													if ( SCHEDULER_CONTROL_Pre_End_Possible_Check( CHECKING_SIDE , SCH_No , FALSE ) ) { /* 2006.09.13 */ \
														if ( _SCH_PREPOST_Pre_End_Force_Part( CHECKING_SIDE , SCH_No ) ) { \
															if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( CHECKING_SIDE , NextYes ) ) { \
															} \
														} \
													} \
												} \
											} \
										} \
									} \
								} \
							} \
						} \
						if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CHECK( NextYes , &dummode ) ) { \
							if ( dummode == 99 ) { \
								if ( SCHEDULING_More_Supply_Check_for_STYLE_4( SCH_No , 0 ) ) { \
									dummode = SDM4_PTYPE_LOTRUN; \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_END_RESET( SCH_No ); \
								} \
								else { \
									dummode = SDM4_PTYPE_LOTEND; \
								} \
							} \
\
\
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
							SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( NextYes , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( dummode , CHECKING_SIDE , SCH_No ) , dummode , RunRecipe , 64 ); \
							if ( RunRecipe[0] != 0 ) { \
								pres = FALSE; \
								switch( dummode ) { \
								case SDM4_PTYPE_LOTRUN : \
									if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
											 SCH_No , \
											 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( NextYes % 100 ) + 1 , cf , \
											 RunRecipe , \
											 2 , \
											 0 , "D" , PROCESS_DEFAULT_MINTIME , \
											 0 , 405 ) == 1 ) { \
										SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( NextYes ); \
									} \
									break; \
								case SDM4_PTYPE_LOTFIRST : \
									SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( dummode , CHECKING_SIDE , SCH_No ) , SCH_No ); \
									SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( SCH_No , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( dummode , CHECKING_SIDE , SCH_No ) ); /* 2010.09.03 */ \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_No , FALSE ); /* 2005.09.23 */ \
\
									if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
											 SCH_No , \
											 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( NextYes % 100 ) + 1 , cf , \
											 RunRecipe , \
											 2 , \
											 1 , "F" , PROCESS_DEFAULT_MINTIME , \
											 0 , 406 ) == 1 ) { \
										SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( NextYes ); \
									} \
									break; \
								case SDM4_PTYPE_LOTEND : \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_No , FALSE ); /* 2005.09.23 */ \
\
									if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
											 SCH_No , \
											 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( NextYes % 100 ) + 1 , cf , \
											 RunRecipe , \
											 2 , \
											 2 , "E" , PROCESS_DEFAULT_MINTIME , \
											 0 , 407 ) == 1 ) { \
										SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( NextYes ); \
									} \
									break; \
								} \
							} \
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_CLEAR( NextYes ); \
						} \
					} \
					else { \
\
\
						if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
						if ( pres ) { \
							outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( CHECKING_SIDE , PROCESS_INDICATOR_POST , 1 ); /* 1 : After Pick from PM */ \
\
\
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
							if ( outcheck == -1 ) { \
								return SYS_ABORTED; \
							} \
							else if ( ( outcheck == 3 ) || ( outcheck == 4 ) ) { /* 2006.11.29 */ \
								prsts = TRUE; /* 2007.04.04 */ \
								if ( forcespawn	&& ( outcheck == 3 ) ) { /* 2006.11.29 */ \
									SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( CHECKING_SIDE , pt , SCH_No - 1 ); /* 2008.02.28*/ \
									if ( _SCH_EQ_SPAWN_PROCESS( CHECKING_SIDE , pt , SCH_No - 1 , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ) , wsa , cf , RunRecipe , 1 , 0 , "" , PROCESS_DEFAULT_MINTIME ) ) { \
										if ( ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { \
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Start at %s(%d)[%s]%cPROCESS_POST_START PM%d:%d:%s:E039%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No - 1 ) , wsa , RunRecipe , 9 , SCH_No - 1 - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
										} \
										_SCH_TIMER_SET_PROCESS_TIME_START( 1 , SCH_No - 1 ); \
										_SCH_MACRO_PROCESS_PART_TAG_SETTING( SCH_No - 1 , PROCESS_INDICATOR_POST + wsa , CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order , RunRecipe , 0 ); \
									} \
									else { \
										/* _SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Fail at %s(%d)[%s]%cPROCESS_POST_FAIL PM%d:%d:%s:E0401%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No - 1 ) , wsa , RunRecipe , 9 , SCH_No - 1 - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); */ /* 2007.04.03 */ \
										prsts = FALSE; /* 2007.04.04 */ \
									} \
								} \
								s1p = 0; \
								tmo = 0; \
								while( TRUE ) { \
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 1 rckside=%d,SCH_No=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , SCH_No , outcheck , forcespawn , s1p , pres ); \
									if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
										return SYS_ABORTED; \
									} \
									if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , SCH_No - 1 , FALSE ) == SYS_RUNNING ) { \
										tmo = 1; \
										break; \
									} \
									CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*									Sleep(1); */ /* 2007.07.23 */ \
									if ( _SCH_MODULE_Get_PM_WAFER( SCH_No - 1 , 0 ) <= 0 ) { \
										s1p++; \
										if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) { \
											tmo = 11; \
											break; \
										} \
									} \
									else { /* 2006.11.29 */ \
										if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , ##ZZZ## , 5001 , SCH_No - 1 - PM1 , TRUE , 3 , &j , &j , &j ) > 0 ) { \
											if ( SCHEDULER_CONTROL_PM_PART_RUNNING( SCH_No - 1 ) ) { /* 2006.11.16 */ \
												s1p = 0; \
											} \
											else { \
												s1p++; \
											} \
											if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) { \
												tmo = 12; \
												break; \
											} \
										} \
										else { \
											s1p = 0; \
										} \
									} \
								} \
								if ( prsts ) { /* 2007.04.04 */ \
									if ( ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { \
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Start at %s(%d):[%s]%cPROCESS_POST_START PM%d:%d:%s:E0403-%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsa , RunRecipe , 9 , SCH_No - PM1 + 1 , wsa , RunRecipe , tmo , 9 , wsa ); \
									} \
									_SCH_TIMER_SET_PROCESS_TIME_START( 1 , SCH_No ); \
									_SCH_MACRO_PROCESS_PART_TAG_SETTING( SCH_No , PROCESS_INDICATOR_POST + wsa , CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order , RunRecipe , 0 ); \
								} \
							} \
							else { \
								if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , SCH_No ) ) { /* 2006.01.06 */ \
									SCHEDULER_CONTROL_PRCSWAITING_10SEC( SCH_No , -1 ); /* 2006.11.23 */ \
									SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( CHECKING_SIDE , pt , SCH_No ); /* 2008.02.28*/ \
									if ( _SCH_EQ_SPAWN_PROCESS( CHECKING_SIDE , pt , SCH_No , _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ) , wsa , cf , RunRecipe , 1 , 0 , "" , PROCESS_DEFAULT_MINTIME ) ) { \
										if ( ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { \
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Start at %s(%d)[%s]%cPROCESS_POST_START PM%d:%d:%s:E039-2%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsa , RunRecipe , 9 , SCH_No - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
										} \
										_SCH_TIMER_SET_PROCESS_TIME_START( 1 , SCH_No ); \
										_SCH_MACRO_PROCESS_PART_TAG_SETTING( SCH_No , PROCESS_INDICATOR_POST + wsa , CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order , RunRecipe , 0 ); \
									} \
									/* else { */ /* 2007.04.03 */ \
										/* _SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Fail at %s(%d)[%s]%cPROCESS_POST_FAIL PM%d:%d:%s:E0401%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , wsa , RunRecipe , 9 , SCH_No - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); */ /* 2007.04.03 */ \
									/* } */ /* 2007.04.03 */ \
								} \
							} \
						} \
					} \
				} \
			} \
			else { \
\
\
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
				switch( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( pt ) ) { \
				case 2 : \
				case 4 : \
					if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_LOTFIRST( SCH_No ) ) { \
						pickskip = TRUE; \
						NextPMYes = SCHEDULER_CONTROL_Get_SDM_4_CHAMER_SIDE_LOTFIRST( SCH_No ); \
						if ( NextPMYes != CHECKING_SIDE ) { \
/* _SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TEST LOG TM_InCount(%d) 1 \tTEST_LOG\t\n" , CHECKING_SIDE ); */ /* 2011.06.09 */ \
							_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); \
							_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE ); \
							_SCH_MODULE_Dec_TM_DoubleCount( CHECKING_SIDE ); \
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( pt ); \
							_SCH_MODULE_Inc_TM_OutCount( NextPMYes ); \
							_SCH_MODULE_Inc_FM_OutCount( NextPMYes ); \
							_SCH_MODULE_Inc_TM_DoubleCount( NextPMYes ); \
						} \
						else { \
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( pt ); \
						} \
						_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , NextPMYes , pt , NextPMYes , 0 ); \
						_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
						_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); \
						_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 ); \
						if ( dummypost ) { /* 2005.12.21 */ \
							SCHEDULER_CONTROL_Get_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
							_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
									 SCH_No , \
									 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , cf , \
									 RunRecipe , \
									 1 , \
									 2 , "E" , PROCESS_DEFAULT_MINTIME , \
									 0 , 408 ); \
						} \
						if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_LOTFIRST( NextPMYes , SCH_No , pt ) ) { \
							SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , NextPMYes , SCH_No ) , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_No , FALSE ); /* 2005.09.23 */ \
							SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , NextPMYes , SCH_No ) , SCH_No ); \
							SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( SCH_No , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , NextPMYes , SCH_No ) ); /* 2010.09.03 */ \
							\
							if ( RunRecipe[0] != 0 ) { /* 2005.12.21 */ \
								if ( dummypost ) { /* 2005.12.21 */ \
									while( TRUE ) { \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 301 NextPMYes=%d,SCH_No=%d,pt=%d\n" , ##ZZZ## + 1 , NextPMYes , SCH_No , pt ); /* 2014.03.12 */ \
\
										if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Fail at %s%cPROCESS_POST_FAIL PM%d:::E0441%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 , 9 ); \
											sch4_RERUN_END_S3_TAG[ SCH_No ] = FALSE; \
											return SYS_ABORTED; \
										} \
										pres = _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , SCH_No , FALSE ); \
										if ( pres == SYS_SUCCESS ) break; \
										if ( pres != SYS_RUNNING ) { \
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Fail at %s%cPROCESS_POST_FAIL PM%d:::E0442%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_No ) , 9 , SCH_No - PM1 + 1 , 9 ); \
											sch4_RERUN_END_S3_TAG[ SCH_No ] = FALSE; \
											return SYS_ABORTED; \
										} \
										Sleep(100); \
									} \
								} \
								if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( NextPMYes , \
										 SCH_No , \
										 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , cf , \
										 RunRecipe , \
										 2 , \
										 1 , "F" , PROCESS_DEFAULT_MINTIME , \
										 0 , 409 ) == 1 ) { \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
								} \
							} \
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_CLEAR( pt ); \
						} \
					} \
					else { \
						_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
						_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
						_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); \
						_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 ); \
						if ( dummypost ) { \
							SCHEDULER_CONTROL_Get_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
							_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
									 SCH_No , \
									 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , cf , \
									 RunRecipe , \
									 1 , \
									 2 , "E" , PROCESS_DEFAULT_MINTIME , \
									 0 , 410 ); \
						} \
					} \
					break; \
				case 1 : \
					_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
					_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
					_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); \
					_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 ); \
					if ( dummypost ) { \
						SCHEDULER_CONTROL_Get_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
						SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , CHECKING_SIDE , SCH_No ) , SCH_No ); \
						SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( SCH_No , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , CHECKING_SIDE , SCH_No ) ); /* 2010.09.03 */ \
						\
						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
								 SCH_No , \
								 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , cf , \
								 RunRecipe , \
								 1 , \
								 1 , "F" , PROCESS_DEFAULT_MINTIME , \
								 0 , 411 ); \
					} \
					break; \
				case 3 : \
					_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
					_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
					_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); \
					_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 ); \
					if ( dummypost ) { \
						SCHEDULER_CONTROL_Get_SDM_4_CHAMER_POST_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTRUN , CHECKING_SIDE , SCH_No ) , SDM4_PTYPE_LOTRUN , RunRecipe , 64 ); \
						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
								 SCH_No , \
								 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , cf , \
								 RunRecipe , \
								 1 , \
								 0 , "D" , PROCESS_DEFAULT_MINTIME , \
								 0 , 412 ); \
					} \
					break; \
				default : \
					_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
					_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
					_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_PM_WAFER( SCH_No , 0 ) ); \
					_SCH_MODULE_Set_PM_WAFER( SCH_No , 0 , 0 ); \
					break; \
				} \
				sch4_RERUN_END_S3_TAG[ SCH_No ] = FALSE; /* 2005.11.11 */ \
			} \
\
\
			if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
			if ( sch4_RERUN_END_S1_TAG[ SCH_No ] != -1 ) { \
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) <= 0 ) { \
					pt = sch4_RERUN_END_S1_TAG[ SCH_No ]; \
					SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , sch4_RERUN_END_S1_ETC[ SCH_No ] , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_No , FALSE ); /* 2005.09.23 */ \
					_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
							 SCH_No , \
							 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , 0 , \
							 RunRecipe , \
							 2 , \
							 2 , "E" , PROCESS_DEFAULT_MINTIME , \
							 0 , 413 ); \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( SCH_No ); \
					sch4_RERUN_END_S1_TAG[ SCH_No ] = -1; \
				} \
			} \
			else if ( ( sch4_RERUN_FST_S1_TAG[ SCH_No ] != -1 ) && ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( SCH_No ) ) ) { \
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_No ) <= 0 ) { \
\
					if ( ( NextYes == 0 ) && SCHEDULER_CONTROL_SDM_4_NO_MORE_WAFER( CHECKING_SIDE , ##ZZZ## , SCH_No ) ) { /* 2010.09.02 */ \
\
						sch4_RERUN_FST_S1_TAG[ SCH_No ] = -1; \
						sch4_RERUN_FST_S2_TAG[ SCH_No ] = -1; /* 2010.10.01 */ \
\
					} \
					else { \
						pt = sch4_RERUN_FST_S1_TAG[ SCH_No ]; \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_No , TRUE ); /* 2005.09.23 */ \
						SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , sch4_RERUN_FST_S1_ETC[ SCH_No ] , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
						SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( sch4_RERUN_FST_S1_ETC[ SCH_No ] , SCH_No ); \
						SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( SCH_No , sch4_RERUN_FST_S1_ETC[ SCH_No ] ); /* 2006.01.24 */ \
						\
						_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
								 SCH_No , \
								 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( pt % 100 ) + 1 , 0 , \
								 RunRecipe , \
								 2 , \
								 1 , "F" , PROCESS_DEFAULT_MINTIME , \
								 0 , 414 ); \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( SCH_No ); \
						sch4_RERUN_FST_S1_TAG[ SCH_No ] = -1; \
					} \
				} \
			} \
			_SCH_TIMER_SET_ROBOT_TIME_START( ##ZZZ## , cf ); \
		}
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
#define MAKE_SCHEDULER_PLACE_TO_BM_PART_for_4( ZZZ ) \
\
\
		if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
		SCH_No = SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## , CHECKORDER_FOR_FM_PLACE ); \
		SCH_Chamber = BM1 + ##ZZZ##; \
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_BM SCH_Chamber=%d,SCH_No=%d\n" , ##ZZZ## + 1 , SCH_Chamber , SCH_No ); \
		if ( !pickskip && SCHEDULER_CONTROL_Chk_BM_PLACE_POSSIBLE_FOR_4_##ZZZ##( CHECKING_SIDE , _SCH_MODULE_Get_TM_POINTER( ##ZZZ## , SCH_No ) , &SCH_No , SCH_Chamber , &SCH_Slot ) ) { \
			if ( ##ZZZ## == 0 ) _SCH_SYSTEM_USING_SET( CHECKING_SIDE , 16 ); \
			runaction = TRUE; \
			pt = _SCH_MODULE_Get_TM_POINTER( ##ZZZ## , SCH_No ); \
			ch = SCH_Chamber; \
			wsa = _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , SCH_No ); \
			wsb = SCH_Slot; \
			if ( pt < 100 ) { \
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_BM_END ); \
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ); \
			} \
			else { \
				FM_CM = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); \
			} \
			if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) swmode = 0; \
			else                                           swmode = 3; \
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_BM 1 SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,pt=%d\n" , ##ZZZ## + 1 , SCH_Chamber , SCH_Slot , SCH_No , pt ); \
			if ( _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PLACE , CHECKING_SIDE , pt , ch , SCH_No , wsa , wsb , 0 , -1 , FALSE , swmode , -1 , -1 ) == SYS_ABORTED ) { \
				return SYS_ABORTED; \
			} \
			_SCH_MODULE_Set_BM_SIDE_POINTER( SCH_Chamber , SCH_Slot , CHECKING_SIDE , pt ); \
			_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( ##ZZZ##,SCH_No ) , BUFFER_OUTWAIT_STATUS ); \
			_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , SCH_No , 0 ); \
/* _SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TEST LOG TM_InCount(%d) 2 \tTEST_LOG\t\n" , CHECKING_SIDE ); */ /* 2011.06.09 */ \
			_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); \
			if ( !SCHEDULER_CONTROL_Set_SDM_4_CHAMER_END_REJECT( pt ) ) { /* 2006.09.02 */ \
				if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( SCH_Chamber - BM1 + PM1 , 2 + SCH_Slot ) ) { \
					if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) == MUF_04_USE_to_PREND_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) == MUF_05_USE_to_PRENDNA_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) == MUF_71_USE_to_PREND_EF_XLP ) || ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_Chamber - BM1 + PM1 ) <= MUF_00_NOTUSE ) ) { \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , -1 , 2 , 201 ); \
					} \
					else { \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , -1 , 1 , 202 ); \
					} \
				} \
			} \
		}


//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
#define MAKE_SCHEDULER_PICK_FROM_BM_PART_for_4( ZZZ ) \
\
\
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
				if ( ##ZZZ## == 0 ) _SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 ); \
				runaction = TRUE; \
				pt = _SCH_MODULE_Get_BM_POINTER( SCH_Chamber , SCH_Slot ); \
				cf = SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## , CHECKORDER_FOR_FM_PICK ); \
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_FROM_BM SCH_Chamber=%d,SCH_Slot=%d,pt=%d,cf=%d\n" , ##ZZZ## + 1 , SCH_Chamber , SCH_Slot , pt , cf ); \
				if ( pt < 100 ) { \
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_STARTING ); \
					_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt ); \
					wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE , pt ); \
					wsb = SCH_Slot; \
					FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ); \
					if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) { \
						_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 2 ); \
					} \
					if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
						if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_Chamber - BM1 + PM1 ) <= 1 ) { \
							if ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( SCH_Chamber , BUFFER_INWAIT_STATUS ) - 1 ) <= 0 ) { \
								if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 2 ) { \
									if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , -1 , 2 , 203 ) == 0 ) { \
									} \
								} \
								else if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 1 ) { \
									if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) != SCH_Chamber ) { \
										if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , -1 , 2 , 204 ) == 0 ) { \
										} \
									} \
								} \
								else if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 0 ) { /* 2006.08.30 */ \
									if ( !SCHEDULER_CONTROL_Check_Chamber_Run_Wait_for_STYLE_4_Part( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) ) { \
										if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) != SCH_Chamber ) { \
											if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , -1 , 2 , 204 ) == 0 ) { \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
					if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( SCH_Chamber - BM1 + PM1 , 0 ) ) { \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , -1 , 1 , 205 ); \
					} \
				} \
				else { \
					wsa = _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot ); \
					wsb = SCH_Slot; \
					FM_CM = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); \
					if ( SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 , TRUE ) ) { /* 2005.12.05 */ \
						if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER_DELETE( CHECKING_SIDE , pt ) ) { \
							placeskip = 31; \
						} \
					} \
					if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_Chamber - BM1 + PM1 ) <= 0 ) { \
						if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( CHECKING_SIDE , pt ) ) { \
							placeskip = 1; \
						} \
					} \
					else { \
						if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 1 ) { \
							if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
								if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_Chamber - BM1 + PM1 ) <= 1 ) { \
									if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) != SCH_Chamber ) { \
										if ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( SCH_Chamber , BUFFER_INWAIT_STATUS ) - 1 ) <= 0 ) { \
											if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( CHECKING_SIDE , pt ) ) { \
												placeskip = 2; \
											} \
										} \
									} \
								} \
							} \
						} \
						else if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 0 ) { /* 2006.08.30 */ \
							if ( !SCHEDULER_CONTROL_Check_Chamber_Run_Wait_for_STYLE_4_Part( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) ) { \
								if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) >= MUF_90_USE_to_XDEC_FROM ) ) { \
									if ( _SCH_MODULE_Get_Mdl_Run_Flag( SCH_Chamber - BM1 + PM1 ) <= 1 ) { \
										if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) != SCH_Chamber ) { \
											if ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( SCH_Chamber , BUFFER_INWAIT_STATUS ) - 1 ) <= 0 ) { \
												if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_CHANGE_CYCLE_TO_LOTEND( CHECKING_SIDE , pt ) ) { \
													placeskip = 2; \
												} \
											} \
										} \
									} \
								} \
							} \
						} \
					} \
					if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PLACE_PM_SKIP_CHECK( pt ) ) { \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( pt ); \
						placeskip = 3; \
					} \
					if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_POSSIBLE_CHECK( pt , &dummode ) ) { \
						if ( dummode == 99 ) { \
							if ( SCHEDULING_More_Supply_Check_for_STYLE_4( SCH_Chamber , 0 ) ) { \
								dummode = SDM4_PTYPE_LOTRUN; \
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_END_RESET( SCH_Chamber ); \
							} \
							else { \
								dummode = SDM4_PTYPE_LOTEND; \
							} \
						} \
\
\
						if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
						SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( dummode , CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) , dummode , RunRecipe , 64 ); \
						if ( RunRecipe[0] != 0 ) { \
							switch ( dummode ) { \
							case SDM4_PTYPE_LOTRUN : \
								if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
										 SCH_Chamber - BM1 + PM1 , \
										 FM_CM , wsa , cf , \
										 RunRecipe , \
										 2 , \
										 0 , "D" , PROCESS_DEFAULT_MINTIME , \
										 0 , 415 ) == 1 ) { \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
								} \
								break; \
							case SDM4_PTYPE_LOTFIRST : \
								SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( dummode , CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) , SCH_Chamber - BM1 + PM1 ); \
								SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( SCH_Chamber - BM1 + PM1 , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( dummode , CHECKING_SIDE , SCH_Chamber - BM1 + PM1 ) ); /* 2010.09.03 */ \
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_Chamber - BM1 + PM1 , FALSE ); /* 2005.09.23 */ \
\
								if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
										 SCH_Chamber - BM1 + PM1 , \
										 FM_CM , wsa , cf , \
										 RunRecipe , \
										 2 , \
										 1 , "F" , PROCESS_DEFAULT_MINTIME , \
										 0 , 416 ) == 1 ) { \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
								} \
								break; \
							case SDM4_PTYPE_LOTEND : \
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( SCH_Chamber - BM1 + PM1 , FALSE ); /* 2005.09.23 */ \
\
								if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE , \
										 SCH_Chamber - BM1 + PM1 , \
										 FM_CM , wsa , cf , \
										 RunRecipe , \
										 2 , \
										 2 , "E" , PROCESS_DEFAULT_MINTIME , \
										 0 , 417 ) == 1 ) { \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
								} \
								break; \
							} \
						} \
						SCHEDULER_CONTROL_Set_SDM_4_CHAMER_PRE_CLEAR( pt ); \
					} \
				} \
				NextYes = SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_for_STYLE_4( ##ZZZ## , CHECKING_SIDE , cf , SCH_Chamber ); \
				if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) swmode = 0; \
				else                                           swmode = 3; \
				if ( _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PICK + 120 , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , NextYes , -1 , FALSE , swmode , -1 , -1 ) == SYS_ABORTED ) { /* 2005.12.05 */ \
					return SYS_ABORTED; \
				} \
				_SCH_MODULE_Set_TM_PATHORDER( ##ZZZ## , cf , SCH_Order ); \
				_SCH_MODULE_Set_TM_TYPE( ##ZZZ## , cf , SCHEDULER_MOVING_IN ); \
				_SCH_MODULE_Set_TM_SIDE_POINTER( ##ZZZ## , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
				_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , cf , _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot ) ); \
				_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , 0 , BUFFER_READY_STATUS ); \
				_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE ); \
				if ( pt < 100 ) { \
					_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE ); \
				} \
				_SCH_TIMER_SET_ROBOT_TIME_START( ##ZZZ## , cf );


//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
#define MAKE_SCHEDULER_PLACE_TO_PM_AFTER_CHECKED_PICKBM_PART_for_4( ZZZ ) \
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_PM_AFTER_CHECKED_PICKBM pickskip=%d,bmpickres=%d,pt=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres , pt ); \
\
\
			if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
			ch = SCH_Chamber - BM1 + PM1; \
			SCH_No = SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## , CHECKORDER_FOR_TM_PLACE ); \
			if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) { \
				placeskip = 4; \
			} \
			if ( pickskip ) { \
				if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PLACE_PM_SKIP_CHECK( pt ) ) { \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( pt ); \
					placeskip = 5; \
				} \
				rckside = NextPMYes; \
			} \
			else { \
				rckside = CHECKING_SIDE; \
			} \
			if ( placeskip == 0 ) { \
				if ( !SCHEDULING_Possible_Process_for_STYLE_4( rckside , pt , ch ) ) placeskip = 6; \
			} \
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_PM_AFTER_CHECKED_PICKBM 1 pickskip=%d,bmpickres=%d,placeskip=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres , placeskip ); \
			/* if ( pt >= 100 ) SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); // 2005.09.21 */ \
			if ( placeskip == 0 ) { \
				pres = SCHEDULER_CONTROL_Chk_POST_BEFORE_PLACE_PART( CHECKING_SIDE , ##ZZZ## , ch , RunRecipe , &SID_Get , &SCH_Slot , 2 ); \
				if ( pres != 0 ) { \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_PM_AFTER_CHECKED_PICKBM 2 pickskip=%d,bmpickres=%d,placeskip=%d,pres=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres , placeskip , pres ); \
					outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( CHECKING_SIDE , PROCESS_INDICATOR_POST , 2 ); /* 2 : After Pick from BM , Before Place to PM 1 */ \
\
\
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
					if ( outcheck == -1 ) { \
						return SYS_ABORTED; \
					} \
					else { \
						s1p = 0; \
						tmo = 0; \
						while( TRUE ) { \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 2 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
								return SYS_ABORTED; \
							} \
							if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , ch , FALSE ) == SYS_RUNNING ) { \
								tmo = 1; \
								break; \
							} \
							if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , ch + pres , FALSE ) == SYS_RUNNING ) { \
								tmo = 2; \
								break; \
							} \
							CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*							Sleep(1); */ /* 2007.07.23 */ \
							if ( _SCH_MODULE_Get_PM_WAFER( ch + pres , 0 ) <= 0 ) { \
								s1p++; \
								if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) { \
									tmo = 11; \
									break; \
								} \
							} \
							else { /* 2006.11.29 */ \
								if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , ##ZZZ## , 5002 , ch + pres - PM1 , TRUE , 3 , &j , &j , &j ) > 0 ) { \
									if ( SCHEDULER_CONTROL_PM_PART_RUNNING( ch + pres ) ) { /* 2006.11.16 */ \
										s1p = 0; \
									} \
									else { \
										s1p++; \
									} \
									if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) { \
										tmo = 12; \
										break; \
									} \
								} \
								else { \
									s1p = 0; \
								} \
							} \
						} \
						if ( ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { \
							if ( SID_Get == CHECKING_SIDE ) { /* 2006.11.30 */ \
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Start at %s(%d):[%s]%cPROCESS_POST_START PM%d:%d:%s:E007-%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , SCH_Slot , RunRecipe , 9 , ch - PM1 + 1 , SCH_Slot , RunRecipe , tmo , 9 , SCH_Slot ); \
							} \
						} \
						SCHEDULER_CONTROL_PRCSWAITING_10SEC( ch + pres , PROCESS_INDICATOR_POST ); /* 2007.04.03 */ \
						_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch ); \
						/* _SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_POST + SCH_Slot , CHECKING_SIDE , -1 , -1 , -1 , RunRecipe , 0 ); */ /* 2006.12.01 */ \
						_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_POST + SCH_Slot , SID_Get , -1 , -1 , -1 , RunRecipe , 0 ); /* 2006.12.01 */ \
					} \
				} \
				runaction = TRUE; \
				SCH_Slot = 1; \
				wsa = _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , SCH_No ); \
				wsb = SCH_Slot; \
				while( TRUE ) { \
\
\
\
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
					j = SCHEDULER_MAKE_LOCK_FREE_for_4( CHECKING_SIDE , ##ZZZ## , 1 ); /* 2010.11.29 */ \
\
					if ( j < 0 ) return SYS_ABORTED; \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 2-1 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d,j=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres , j ); \
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
						return SYS_ABORTED; \
					} \
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) { \
						j = _SCH_MACRO_CHECK_PROCESSING_INFO( ch ); \
						if ( ( j == -1 ) || ( j == -3 ) || ( j == -5 ) ) { \
							if ( pt >= 100 ) { /* // 2006.09.21 */ \
								_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(D%d)[%s]%cPROCESS_PRE_FAIL PM%d:D%d:%s:E053%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
								return SYS_ABORTED; \
							} \
							else { /* 2006.09.21 */ \
								if ( j == -1 ) { \
									return SYS_ABORTED; \
								} \
								break; \
							} \
						} \
						else if ( j <= 0 ) { \
							if ( pt >= 100 ) { /* // 2004.09.07 */ \
								if ( _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , SCH_No ) != CHECKING_SIDE ) { \
									rckside = _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , SCH_No ); \
								} \
							} \
							break; \
						} \
					} \
					if ( pt >= 100 ) { /* 2004.09.07 */ \
						_SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , 1 ); /* 2004.09.07 */ \
						_SCH_SYSTEM_LEAVE_TM_CRITICAL( ##ZZZ## ); /* 2004.09.07 */ \
						Sleep(1); /* 2009.08.04 */ \
					} /* 2004.09.07 */ \
					if ( pt < 100 ) { /* 2009.08.04 */ \
						CRITICAL_AND_SLEEP_FOR_4( ZZZ , 1 ) /* 2007.07.23 */ \
					} \
/*					Sleep(1); */ /* 2007.07.23 */ \
					if ( pt >= 100 ) { /* 2004.09.07 */ \
						_SCH_SYSTEM_ENTER_TM_CRITICAL( ##ZZZ## ); /* 2004.09.07 */ \
						_SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , 0 ); /* 2004.09.07 */ \
					} /* 2004.09.07 */ \
				} \
				\
\
\
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
				if ( ( sch4_RERUN_FST_S1_TAG[ ch ] != -1 ) && ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( ch ) ) ) { \
					s1p = sch4_RERUN_FST_S1_TAG[ ch ]; \
					SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( s1p , sch4_RERUN_FST_S1_ETC[ ch ] , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
					SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( sch4_RERUN_FST_S1_ETC[ ch ] , ch ); \
					SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( ch , sch4_RERUN_FST_S1_ETC[ ch ] ); /* 2006.01.24 */ \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( ch , TRUE ); /* 2005.09.23 */ \
					if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( rckside , \
							 ch , \
							 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (s1p % 100) + 1 , 0 , \
							 RunRecipe , \
							 2 , \
							 1 , "F" , PROCESS_DEFAULT_MINTIME , \
							 0 , 418 ) == 1 ) { \
						while( TRUE ) { \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 2-2 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
								return SYS_ABORTED; \
							} \
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) { \
								j = _SCH_MACRO_CHECK_PROCESSING_INFO( ch ); \
								if ( ( j == -1 ) || ( j == -3 ) || ( j == -5 ) ) { \
									_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(D%d)[%s]%cPROCESS_PRE_FAIL PM%d:D%d:%s:E0211%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
									return SYS_ABORTED; \
								} \
								else if ( j <= 0 ) { \
									break; \
								} \
							} \
							Sleep(1); \
						} \
					} \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( ch ); \
					sch4_RERUN_FST_S1_TAG[ ch ] = -1; \
				} \
				if ( pt >= 100 ) SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
\
\
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
				if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , rckside ) ) { \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_PM_AFTER_CHECKED_PICKBM 4 pt=%d , rckside=%d\n" , ##ZZZ## + 1 , pt , rckside ); \
\
					pres = 0; /* 2006.10.04 */ \
					if ( pt < 100 ) { /* 2006.01.11 */ \
						pres = SCHEDULER_CONTROL_Chk_PRE_BEFORE_PLACE_PART( ##ZZZ## , rckside , pt , ch , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ##,SCH_No ) , RunRecipe , &forcespawn , 3 , &subres ); \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_PM_AFTER_CHECKED_PICKBM 4-2 pt=%d , rckside=%d , pres=%d , subres = %d , forcespawn = %d\n" , ##ZZZ## + 1 , pt , rckside , pres , subres , forcespawn ); \
\
						if ( pres > 0 ) { \
							prsts = TRUE; /* 2007.04.04 */ \
							outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( rckside , PROCESS_INDICATOR_PRE , 3 ); /* 3 : After Pick from BM , Before Place to PM 2 */ \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PLACE_TO_PM_AFTER_CHECKED_PICKBM 4-3 pt=%d , rckside=%d , pres=%d , outcheck = %d\n" , ##ZZZ## + 1 , pt , rckside , pres , outcheck ); \
\
							if ( outcheck == -1 ) { \
								return SYS_ABORTED; \
							} \
							else if ( ( outcheck == 3 ) || ( outcheck == 4 ) ) { /* 2006.11.29 */ \
								if ( forcespawn && ( outcheck == 3 ) ) { /* 2006.09.13 2006.11.29 */ \
									if ( ( pres == 0 ) || ( pres == 1 ) || ( pres == 2 ) ) SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( rckside , pt , ch - 1 ); /* 2008.02.28*/ \
									if ( _SCH_EQ_SPAWN_PROCESS( rckside , pt , ch - 1 , _SCH_CLUSTER_Get_PathIn( rckside , pt ) , wsa , SCH_No , RunRecipe , /* 2009.09.15 pres */ ( pres >= 99 ) ? 99 : pres , 0 , "" , PROCESS_DEFAULT_MINTIME ) ) { \
										if ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { \
											_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Start at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:E0262-7%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch - 1 ) , wsa , RunRecipe , 9 , ch - 1 - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
										} \
										_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch - 1 ); \
										_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch - 1 , PROCESS_INDICATOR_PRE + wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 2 , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ## , SCH_No ) , RunRecipe , 0 ); \
									} \
									else { \
										prsts = FALSE; /* 2007.04.04 */ \
										/* _SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d:%d:%s:E008%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch - 1 ) , wsa , RunRecipe , 9 , ch - 1 - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); */ /* 2007.04.03*/ \
									} \
								} \
								s1p = 0; \
								tmo = 0; \
								while( TRUE ) { \
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 3 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
									if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
										return SYS_ABORTED; \
									} \
									if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( rckside , ch - 1 , FALSE ) == SYS_RUNNING ) { \
										tmo = 1; \
										break; \
									} \
									CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*									_sleep(1); */ /* 2007.07.23 */ \
									if ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) <= 0 ) { \
										if ( SCHEDULER_CONTROL_PM_PART_RUNNING( ch - 1 ) ) { /* 2006.11.16 */ \
											s1p = 0; \
										} \
										else { \
											s1p++; \
										} \
										if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT_PRE ) { \
											tmo = 11; \
											break; \
										} \
									} \
									else { /* 2006.11.29 */ \
										if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , ##ZZZ## , 5003 , ch - 1 - PM1 , TRUE , 3 , &j , &j , &j ) > 0 ) { \
											if ( SCHEDULER_CONTROL_PM_PART_RUNNING( ch - 1 ) ) { /* 2006.11.16 */ \
												s1p = 0; \
											} \
											else { \
												s1p++; \
											} \
											if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT_PRE ) { \
												tmo = 12; \
												break; \
											} \
										} \
										else { \
											s1p = 0; \
										} \
									} \
								} \
								if ( prsts ) { /* 2007.04.04 */ \
									if ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { \
										_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Start at %s(%d):[%s]%cPROCESS_PRE_START PM%d:%d:%s:E0262-8-%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , tmo , 9 , wsa ); \
									} \
									_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch ); \
									_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_PRE + wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 2 , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ## , SCH_No ) , RunRecipe , 0 ); \
								} \
							} \
							else { \
								SCHEDULER_CONTROL_PRCSWAITING_10SEC( ch , -1 ); /* 2006.11.23 */ \
								if ( ( pres == 0 ) || ( pres == 1 ) || ( pres == 2 ) ) SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( rckside , pt , ch ); /* 2008.02.28*/ \
								if ( _SCH_EQ_SPAWN_PROCESS( rckside , pt , ch , _SCH_CLUSTER_Get_PathIn( rckside , pt ) , wsa , SCH_No , RunRecipe , /* 2009.09.15 pres */ ( pres >= 99 ) ? 99 : pres , 0 , "" , PROCESS_DEFAULT_MINTIME ) ) { \
									if ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { \
										_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Start at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:E0262-9%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
									} \
									_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch ); \
									_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_PRE + wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 2 , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ## , SCH_No ) , RunRecipe , 0 ); \
								} \
								else { \
									prsts = FALSE; /* 2007.04.04 */ \
									/* _SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d:%d:%s:E008%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); */ /* 2007.04.03*/ \
								} \
							} \
\
							while ( TRUE ) { /* 2015.06.26 */ \
	\
\
\
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
								if ( prsts ) { /* 2007.04.04 */ \
									while( TRUE ) { \
	\
										j = SCHEDULER_MAKE_LOCK_FREE_for_4( CHECKING_SIDE , ##ZZZ## , 1 ); /* 2010.11.29 */ \
	\
										if ( j < 0 ) return SYS_ABORTED; \
	\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 3-1 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d,j=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres , j ); \
										if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
											return SYS_ABORTED; \
										} \
										if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( ##ZZZ## + BM1 ) ) { /* 2011.04.22 */ \
											if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) { \
	\
												j = _SCH_MACRO_CHECK_PROCESSING_INFO( ch ); \
												/* if ( ( j == -1 ) || ( j == -3 ) || ( j == -5 ) ) { */ /* 2007.05.17 */ \
												if ( j == -1 ) { /* 2007.05.17 */ \
													_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d:%d:%s:E027%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
													return SYS_ABORTED; \
												} \
												else if ( ( j == -3 ) || ( j == -5 ) ) { /* 2007.08.20 */ \
													_SCH_CLUSTER_Set_PathDo( rckside , pt , PATHDO_WAFER_RETURN ); \
													break; /* 2007.08.27 */ \
												} \
												else if ( j <= 0 ) { \
													break; \
												} \
	\
											} \
										} \
										CRITICAL_AND_SLEEP_FOR_4( ZZZ , 1 ) /* 2007.07.23 */ \
	/*									Sleep(1); */ /* 2007.07.23 */ \
									} \
								} \
	\
								if ( pres >= 100 ) _SCH_PREPRCS_FirstRunPre_Done_PathProcessData( rckside , ch ); /* 2009.09.15 */ \
								if ( pres == 101 ) _SCH_PREPRCS_FirstRunPre_Done_PathProcessData( rckside , ch - 1 ); /* 2009.09.18 */ \
								if ( pres == 103 ) _SCH_PREPRCS_FirstRunPre_Done_PathProcessData( rckside , ch + 1 ); /* 2009.09.18 */ \
\
								if ( prsts ) { /* 2015.06.26 */ \
									if ( !SCHEDULER_CONTROL_Chk_OtherPM_Has_More_PRE( ##ZZZ## , ch ) ) break; /* 2015.06.26 */ \
								} /* 2015.06.26 */ \
								else { /* 2015.06.26 */ \
									break; /* 2015.06.26 */ \
								} /* 2015.06.26 */ \
\
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 3-1-1 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d,j=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres , j ); \
\
								if ( pres >= 100 ) pres = 99; /* 2015.06.26 */ \
\
							} /* 2015.06.26 */ \
\
						} \
						if ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) != PATHDO_WAFER_RETURN ) { /* 2007.08.29 */ \
							if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , rckside ) ) { \
								_SCH_CLUSTER_Set_PathDo( rckside , pt , PATHDO_WAFER_RETURN ); \
							} \
						} \
						if ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) == PATHDO_WAFER_RETURN ) { /* 2006.10.04 */ \
							_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RETURN_REQUEST ); /* 2006.10.04 */ \
							pres = -999999; /* 2006.10.04 */ \
						} /* 2006.10.04 */ \
					} \
					if ( pres != -999999 ) { /* 2006.10.04 */ \
						if ( pt < 100 ) { \
							_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RUNNING ); \
						} \
						if ( _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PLACE , rckside , pt , ch , SCH_No , wsa , wsb , 0 , -1 , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) { \
							return SYS_ABORTED; \
						} \
						if ( pt < 100 ) { \
							if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( ch , &prcs_time , &post_time ) % 10 ) == 2 ) { \
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( rckside , ch ); \
							} \
						} \
						_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RUNNINGW ); /* 2006.09.05 */ /* 2006.11.29 */ \
						_SCH_MODULE_Set_PM_SIDE_POINTER( ch , rckside , pt , 0 , 0 ); \
						_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); \
						_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , SCH_No ) ); \
						_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , SCH_No , 0 ); \
\
\
						if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
						if ( pt < 100 ) { \
							if ( _SCH_CLUSTER_Check_HasProcessData_Post( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order ) ) { \
								if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( ch , &prcs_time , &post_time ) % 10 ) == 1 ) { \
									_SCH_CLUSTER_Set_PathProcessData_SkipPost( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order ); \
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) { /* 2006.01.12 */ \
										_SCH_CLUSTER_Set_PathProcessData_SkipPost2( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , ch + SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + ##ZZZ## ) ); \
									} \
								} \
							} \
							outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( rckside , PROCESS_INDICATOR_MAIN , 4 ); /* 4 : After to PM 1 */ \
\
\
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
							if ( outcheck == -1 ) { \
								return SYS_ABORTED; \
							} \
							else if ( outcheck == 3 ) { \
								s1p = 0; \
								while( TRUE ) { \
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 4 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
									if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( rckside ) ) { \
										return SYS_ABORTED; \
									} \
									if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( rckside , ch - 1 , FALSE ) == SYS_RUNNING ) { \
										break; \
									} \
									CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*									Sleep(1); */ /* 2007.07.23 */ \
									if ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) > 0 ) { \
										s1p++; \
										/* if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) break; */ /* 2007.10.19 */ \
										if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT_OT_MAIN ) break; /* 2007.10.19 */ \
									} \
								} \
								_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch ); \
								_SCH_LOG_LOT_PRINTF( rckside , "PM Process Start at %s(%d):[%s]%cPROCESS_START PM%d:%d:%s:E0223%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , 9 , ch - PM1 + 1 , wsa , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , 9 , wsa ); \
								_SCH_LOG_TIMER_PRINTF( rckside , TIMER_PM_START , wsa , ch - PM1 + 1 , pt , -1 , -1 , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , "" ); \
								_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) >= _SCH_CLUSTER_Get_PathRange( rckside , pt ) ) ); \
							} \
							else { \
								if ( outcheck == 2 ) { \
									rckside = _SCH_MODULE_Get_PM_SIDE( ch , 0 ); \
									wsa = _SCH_MODULE_Get_PM_WAFER( ch , 0 ); \
									pt = _SCH_MODULE_Get_PM_POINTER( ch , 0 ); \
									ch = ch - 1; \
									SCH_Order = -1; \
									for ( s1p = 0 ; s1p < MAX_PM_CHAMBER_DEPTH ; s1p++ ) { \
										if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == ch ) { \
											SCH_Order = s1p; \
											break; \
										} \
										if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == -ch ) { \
											SCH_Order = s1p; \
											break; \
										} \
									} \
									if ( SCH_Order == -1 ) { \
										for ( s1p = 0 ; s1p < MAX_PM_CHAMBER_DEPTH ; s1p++ ) { /* 2006.07.20 */ \
											if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == ( ch + 1 ) ) { \
												SCH_Order = s1p; \
												break; \
											} \
											if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == -( ch + 1 ) ) { \
												SCH_Order = s1p; \
												break; \
											} \
										} \
										if ( SCH_Order == -1 ) { \
											_SCH_LOG_LOT_PRINTF( rckside , "PM Process Recipe Fail at %s(%d)%cPROCESS_FAIL PM%d:%d::E113%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , 9 , ch - PM1 + 1 , wsa , 9 , wsa ); \
											return SYS_ABORTED; \
										} \
									} \
								} \
								sprintf( RunRecipe , "%d" , outcheck ); \
								SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( rckside , pt , ch ); /* 2008.02.28*/ \
								if ( outcheck != 0 ) SCHEDULER_CONTROL_PRCSWAITING_10SEC( ch , -1 ); \
								_SCH_MACRO_MAIN_PROCESS( rckside , pt , \
																  ch , -1 , \
																  wsa , -1 , SCH_No , -1 , \
																  _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , _SCH_TIMER_GET_ROBOT_TIME_RUNNING( ##ZZZ## , SCH_No ) , RunRecipe , PROCESS_DEFAULT_MINTIME , \
																  _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) >= _SCH_CLUSTER_Get_PathRange( rckside , pt ) ) , \
																  -1 , -1 , -1 , outcheck ,  \
																  0 , 401 ); \
							} \
							_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RUNNING2 ); /* 2006.09.05 2006.11.29 */ \
						} \
						else { \
							_SCH_SDM_Set_CHAMBER_INC_COUNT( pt ); \
							pres = FALSE; \
							switch ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( pt ) ) { \
							case 3 : \
								SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTRUN , rckside , ch ) , SDM4_PTYPE_LOTRUN , RunRecipe , 64 ); \
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( ch , FALSE ); /* 2005.09.23 */ \
								if ( RunRecipe[0] == 0 ) { /* 2006.09.03 */ \
									pres = FALSE; \
								} \
								else { \
									pres = _SCH_EQ_SPAWN_PROCESS( rckside , -1 , ch , FM_CM , wsa , SCH_No , RunRecipe , 0 , 0 , "D" , PROCESS_DEFAULT_MINTIME );\
								} \
								break; \
							case 1 : \
								SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , rckside , ch ) , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
								SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , rckside , ch ) , ch ); \
								SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( ch , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , rckside , ch ) ); /* 2010.09.03 */ \
								\
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( ch , FALSE ); /* 2005.09.23 */ \
								if ( RunRecipe[0] == 0 ) { /* 2006.09.03 */ \
									pres = FALSE; \
								} \
								else { \
									pres = _SCH_EQ_SPAWN_PROCESS( rckside , -1 , ch , FM_CM , wsa , SCH_No , RunRecipe , 0 , 1 , "F" , PROCESS_DEFAULT_MINTIME );\
								} \
								break; \
							case 4 : \
							case 2 : \
								if ( SCHEDULER_CONTROL_Pre_Force_Flag_End_for_STYLE_4_Part( rckside , ch ) == 0 ) { /* 2006.01.13 */ \
									SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( -1 , ch ); /* 2006.07.21 */ \
								} \
								SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , rckside , ch ) , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( ch , FALSE ); /* 2005.09.23 */ \
								if ( RunRecipe[0] == 0 ) { /* 2006.09.03 */ \
									pres = FALSE; \
								} \
								else { \
									pres = _SCH_EQ_SPAWN_PROCESS( rckside , -1 , ch , FM_CM , wsa , SCH_No , RunRecipe , 0 , 2 , "E" , PROCESS_DEFAULT_MINTIME );\
								} \
								break; \
							} \
							if ( RunRecipe[0] != 0 ) { /* 2006.09.03 */ \
								if ( pres ) { \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
									_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch ); \
									_SCH_LOG_LOT_PRINTF( rckside , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:D%d:%s:E057%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
									_SCH_LOG_TIMER_PRINTF( rckside , TIMER_PM_START , _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , ch - PM1 + 1 , -1 , -1 , -1 , RunRecipe , "" ); \
									_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , rckside , pt , -1 , -1 , RunRecipe , 1 ); \
								} \
								else { \
									_SCH_LOG_LOT_PRINTF( rckside , "PM Process Fail at %s(D%d)[%s]%cPROCESS_FAIL PM%d:D%d:%s:E058%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
								} \
							} \
						} \
					} \
				} \
				else { /* 2006.10.02 */ \
					if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , -1 ) ) { \
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( rckside , pt , -1 ); \
					} \
				} \
			} \
			else { \
				_SCH_LOG_LOT_PRINTF( rckside , "TM%d Handling Place %s Skip with %d%cWHTM%dPLACESKIP PM%d:%d\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , placeskip , 9 , ##ZZZ## + 1 , ch - PM1 + 1 , placeskip ); \
				if ( pt >= 100 ) { \
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
				} \
				else { /* 2006.03.11 */ \
					if ( !pickskip ) { \
						if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , -1 ) ) { \
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( rckside , pt , -1 ); \
						} \
					} \
				} \
			}

//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
#define MAKE_SCHEDULER_PICK_BM_X_PLACE_TO_PM_PART_for_4( ZZZ ) \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_BM_X_PLACE_TO_PM pickskip=%d , bmpickres=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres ); \
\
\
			if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
\
			ch = SCH_Chamber - BM1 + PM1; \
			placeskip = 0; \
			if ( SCHEDULER_CONTROL_Chk_PM_PLACE_POSSIBLE_FOR_4_##ZZZ##( CHECKING_SIDE , ch , &pt ) ) { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d BM_CHANGE_TO_VENT 1 runaction=%d,ch=%d,pt=%d\n" , ##ZZZ## + 1 , runaction , ch , pt ); \
\
				SCH_No = SCHEDULER_CONTROL_Chk_ARM_WHAT( ##ZZZ## , CHECKORDER_FOR_TM_PLACE ); \
				rckside = CHECKING_SIDE; \
				if ( !SCHEDULING_Possible_Process_for_STYLE_4( rckside , pt , ch ) ) placeskip = 7; \
				if ( pt >= 100 ) SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
				if ( placeskip == 0 ) { \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d BM_CHANGE_TO_VENT 2 runaction=%d,ch=%d,pt=%d\n" , ##ZZZ## + 1 , runaction , ch , pt ); \
\
					pres = SCHEDULER_CONTROL_Chk_POST_BEFORE_PLACE_PART( CHECKING_SIDE , ##ZZZ## , ch , RunRecipe , &SID_Get , &SCH_Slot , 5 ); \
					if ( pres != 0 ) { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d BM_CHANGE_TO_VENT 3 runaction=%d,pres=%d\n" , ##ZZZ## + 1 , runaction , pres ); \
\
						outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( CHECKING_SIDE , PROCESS_INDICATOR_POST , 5 ); /* 5 : Before Place to PM */ \
\
\
						if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
						if ( outcheck == -1 ) { \
							return SYS_ABORTED; \
						} \
						else { \
							s1p = 0; \
							while( TRUE ) { \
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 5 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
									return SYS_ABORTED; \
								} \
								if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , ch , FALSE ) == SYS_RUNNING ) { \
									break; \
								} \
								if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( CHECKING_SIDE , ch + pres , FALSE ) == SYS_RUNNING ) { \
									break; \
								} \
								CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*								Sleep(1); */ /* 2007.07.23 */ \
								if ( _SCH_MODULE_Get_PM_WAFER( ch + pres , 0 ) <= 0 ) { \
									s1p++; \
									if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) break; \
								} \
								else { /* 2006.11.29 */ \
									if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , ##ZZZ## , 5004 , ch + pres - PM1 , TRUE , 3 , &j , &j , &j ) > 0 ) { \
										if ( SCHEDULER_CONTROL_PM_PART_RUNNING( ch + pres ) ) { /* 2006.11.16 */ \
											s1p = 0; \
										} \
										else { \
											s1p++; \
										} \
										if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT ) break; \
									} \
									else { \
										s1p = 0; \
									} \
								} \
							} \
							if ( ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { \
								if ( SID_Get == CHECKING_SIDE ) { /* 2006.11.30 */ \
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Start at %s(%d):[%s]%cPROCESS_POST_START PM%d:%d:%s:E0072%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , SCH_Slot , RunRecipe , 9 , ch - PM1 + 1 , SCH_Slot , RunRecipe , 9 , SCH_Slot ); \
								} \
							} \
							SCHEDULER_CONTROL_PRCSWAITING_10SEC( ch + pres , PROCESS_INDICATOR_POST ); /* 2007.04.03 */ \
							_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch ); \
							/* _SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_POST + SCH_Slot , CHECKING_SIDE , -1 , -1 , -1 , RunRecipe , 0 ); */ /* 2006.12.01 */ \
							_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_POST + SCH_Slot , SID_Get , -1 , -1 , -1 , RunRecipe , 0 ); /* 2006.12.01 */ \
						} \
					} \
					if ( ##ZZZ## == 0 ) _SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 ); \
\
\
					if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
					runaction = TRUE; \
					SCH_Slot = 1; \
					SCH_Order = _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ##,SCH_No ); /* 2008.07.01*/ \
					wsa = _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , SCH_No ); \
					wsb = SCH_Slot; \
					while( TRUE ) { \
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 5-1 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
						if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
							return SYS_ABORTED; \
						} \
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) { \
							j = _SCH_MACRO_CHECK_PROCESSING_INFO( ch ); \
							if ( ( j == -1 ) || ( j == -3 ) || ( j == -5 ) ) { \
								if ( pt >= 100 ) { /* // 2006.09.21 */ \
									_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(D%d)[%s]%cPROCESS_PRE_FAIL PM%d:D%d:%s:E059%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
									return SYS_ABORTED; \
								} \
								else { \
									if ( j == -1 ) { \
										return SYS_ABORTED; \
									} \
									break; \
								} \
							} \
							else if ( j <= 0 ) { \
								if ( pt >= 100 ) { /* // 2004.09.07 */ \
									if ( _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , SCH_No ) != CHECKING_SIDE ) { \
										rckside = _SCH_MODULE_Get_TM_SIDE( ##ZZZ## , SCH_No ); \
									} \
								} \
								break; \
							} \
						} \
						if ( pt >= 100 ) { /* 2004.09.07 */ \
							_SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , 1 ); /* 2004.09.07 */ \
							_SCH_SYSTEM_LEAVE_TM_CRITICAL( ##ZZZ## ); /* 2004.09.07 */ \
							Sleep(1); /* 2009.08.04 */ \
						} /* 2004.09.07 */ \
						if ( pt < 100 ) { /* 2009.08.04 */ \
							CRITICAL_AND_SLEEP_FOR_4( ZZZ , 1 ) /* 2007.07.23 */ \
						} \
/*						Sleep(1); */ /* 2007.07.23 */ \
						if ( pt >= 100 ) { /* 2004.09.07 */ \
							_SCH_SYSTEM_ENTER_TM_CRITICAL( ##ZZZ## ); /* 2004.09.07 */ \
							_SCH_MODULE_Set_TM_Switch_Signal( ##ZZZ## , 0 ); /* 2004.09.07 */ \
						} /* 2004.09.07 */ \
					} \
					if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , rckside ) ) { \
\
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_BM_X_PLACE_TO_PM 4 pickskip=%d , bmpickres=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres ); \
\
						pres = 0; /* 2006.10.04 */ \
						if ( pt < 100 ) { /* 2006.01.11 */ \
							pres = SCHEDULER_CONTROL_Chk_PRE_BEFORE_PLACE_PART( ##ZZZ## , rckside , pt , ch , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ##,SCH_No ) , RunRecipe , &forcespawn , 6 , &subres ); \
\
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_BM_X_PLACE_TO_PM 4-1 pickskip=%d , bmpickres=%d , pres=%d , subres = %d , forcespawn = %d\n" , ##ZZZ## + 1 , pickskip , bmpickres , pres , subres , forcespawn ); \
\
							if ( pres > 0 ) { \
								outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( rckside , PROCESS_INDICATOR_PRE , 6 ); /* 6 : Before Place to PM 2 */ \
\
\
\
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_BM_X_PLACE_TO_PM 4-1 pickskip=%d , bmpickres=%d , pres=%d , outcheck=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres , pres , outcheck ); \
\
								prsts = TRUE; /* 2007.04.04 */ \
								if ( outcheck == -1 ) { \
									return SYS_ABORTED; \
								} \
								else if ( ( outcheck == 3 ) || ( outcheck == 4 ) ) { /* 2006.11.29 */ \
									if ( forcespawn && ( outcheck == 3 ) ) { /* 2006.09.13 2006.11.29 */ \
										if ( ( pres == 0 ) || ( pres == 1 ) || ( pres == 2 ) ) SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( rckside , pt , ch - 1 ); /* 2008.02.28*/ \
										if ( _SCH_EQ_SPAWN_PROCESS( rckside , pt , ch - 1 , _SCH_CLUSTER_Get_PathIn( rckside , pt ) , wsa , SCH_No , RunRecipe , /* 2009.09.15 pres */ ( pres >= 99 ) ? 99 : pres , 0 , "" , PROCESS_DEFAULT_MINTIME ) ) { \
											if ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { \
												_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Start at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:E0262-10%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch - 1 ) , wsa , RunRecipe , 9 , ch - 1 - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
											} \
											_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch - 1 ); \
											_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch - 1 , PROCESS_INDICATOR_PRE + wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 2 , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ## , SCH_No ) , RunRecipe , 0 ); \
										} \
										else { \
											prsts = FALSE; /* 2007.04.04 */ \
											/* _SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d:%d:%s:E0082%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch - 1 ) , wsa , RunRecipe , 9 , ch - 1 - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); */ /* 2007.04.03*/ \
										} \
									} \
									s1p = 0; \
									while( TRUE ) { \
\
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 6 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
\
										if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
											return SYS_ABORTED; \
										} \
										if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( rckside , ch - 1 , FALSE ) == SYS_RUNNING ) { \
											break; \
										} \
										CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*										Sleep(1); */ /* 2007.07.23 */ \
										if ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) <= 0 ) { \
											if ( SCHEDULER_CONTROL_PM_PART_RUNNING( ch - 1 ) ) { /* 2006.11.16 */ \
												s1p = 0; \
											} \
											else { \
												s1p++; \
											} \
											if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT_PRE ) break; \
										} \
										else { /* 2006.11.29 */ \
											if ( SCHEDULER_CONTROL_Chk_WILLDONE_PART( CHECKING_SIDE , ##ZZZ## , 5005 , ch - 1 - PM1 , TRUE , 3 , &j , &j , &j ) > 0 ) { \
												if ( SCHEDULER_CONTROL_PM_PART_RUNNING( ch - 1 ) ) { /* 2006.11.16 */ \
													s1p = 0; \
												} \
												else { \
													s1p++; \
												} \
												if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT_PRE ) break; \
											} \
											else { \
												s1p = 0; \
											} \
										} \
									} \
									if ( prsts ) { /* 2007.04.04 */ \
										if ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { \
											_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Start at %s(%d):[%s]%cPROCESS_PRE_START PM%d:%d:%s:E0262-11%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
										} \
										_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch ); \
										_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_PRE + wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 2 , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ## , SCH_No ) , RunRecipe , 0 ); \
									} \
								} \
								else { \
									SCHEDULER_CONTROL_PRCSWAITING_10SEC( ch , -1 ); /* 2006.11.23 */ \
									if ( ( pres == 0 ) || ( pres == 1 ) || ( pres == 2 ) ) SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( rckside , pt , ch ); /* 2008.02.28*/ \
									if ( _SCH_EQ_SPAWN_PROCESS( rckside , pt , ch , _SCH_CLUSTER_Get_PathIn( rckside , pt ) , wsa , SCH_No , RunRecipe , /* 2009.09.15 pres */ ( pres >= 99 ) ? 99 : pres , 0 , "" , PROCESS_DEFAULT_MINTIME ) ) { \
										if ( _SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { \
											_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Start at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:E0262-12%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
										} \
										_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch ); \
										_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , PROCESS_INDICATOR_PRE + wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 2 , _SCH_MODULE_Get_TM_PATHORDER( ##ZZZ## , SCH_No ) , RunRecipe , 0 ); \
									} \
									else { \
										prsts = FALSE; /* 2007.04.04 */ \
										/* _SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d:%d:%s:E0083%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); */ /* 2007.04.03*/ \
									} \
								} \
\
\
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
								if ( prsts ) { /* 2007.04.04 */ \
									while( TRUE ) { \
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 6-1 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
										if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { \
											return SYS_ABORTED; \
										} \
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) { \
											j = _SCH_MACRO_CHECK_PROCESSING_INFO( ch ); \
											/* if ( ( j == -1 ) || ( j == -3 ) || ( j == -5 ) ) { */ /* 2007.05.17 */ \
											if ( j == -1 ) { /* 2007.05.17 */ \
												_SCH_LOG_LOT_PRINTF( rckside , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d:%d:%s:E027%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
												return SYS_ABORTED; \
											} \
											else if ( j <= 0 ) { \
												break; \
											} \
										} \
										CRITICAL_AND_SLEEP_FOR_4( ZZZ , 1 ) /* 2007.07.23 */ \
/*										Sleep(1); */ /* 2007.07.23 */ \
									} \
								} \
\
								if ( pres >= 100 ) _SCH_PREPRCS_FirstRunPre_Done_PathProcessData( rckside , ch ); /* 2009.09.15 */ \
								if ( pres == 101 ) _SCH_PREPRCS_FirstRunPre_Done_PathProcessData( rckside , ch - 1 ); /* 2009.09.18 */ \
								if ( pres == 103 ) _SCH_PREPRCS_FirstRunPre_Done_PathProcessData( rckside , ch + 1 ); /* 2009.09.18 */ \
\
							} \
							if ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) == PATHDO_WAFER_RETURN ) { /* 2006.10.04 */ \
								_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RETURN_REQUEST ); /* 2006.10.04 */ \
								pres = -999999; /* 2006.10.04 */ \
							} /* 2006.10.04 */ \
						} \
						if ( pres != -999999 ) { /* 2006.10.04 */ \
							if ( pt < 100 ) { \
								_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RUNNING ); \
							} \
							if ( _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PLACE , rckside , pt , ch , SCH_No , wsa , wsb , 0 , -1 , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) { \
								return SYS_ABORTED; \
							} \
\
\
							if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
							if ( pt < 100 ) { \
								if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( ch , &prcs_time , &post_time ) % 10 ) == 2 ) { \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_CYCLE_HAVE_TO_RUN( rckside , ch ); \
								} \
							} \
							_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RUNNINGW ); /* 2006.09.05 */ /* 2006.11.29 */ \
							_SCH_MODULE_Set_PM_SIDE_POINTER( ch , rckside , pt , 0 , 0 ); \
							_SCH_MODULE_Set_PM_PATHORDER( ch , 0 , SCH_Order ); \
							_SCH_MODULE_Set_PM_WAFER( ch , 0 , _SCH_MODULE_Get_TM_WAFER( ##ZZZ## , SCH_No ) ); \
							_SCH_MODULE_Set_TM_WAFER( ##ZZZ## , SCH_No , 0 ); \
							if ( pt < 100 ) { \
								if ( _SCH_CLUSTER_Check_HasProcessData_Post( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order ) ) { \
									if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( ch , &prcs_time , &post_time ) % 10 ) == 1 ) { \
										_SCH_CLUSTER_Set_PathProcessData_SkipPost( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order ); \
										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) >= 2 ) { /* 2006.01.12 */ \
											_SCH_CLUSTER_Set_PathProcessData_SkipPost2( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , ch + SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( BM1 + ##ZZZ## ) ); \
										} \
									} \
								} \
								outcheck = SCHEDULER_CONTROL_Chk_ONE_BODY_PROCESS_WAIT_FOR_4_##ZZZ##( rckside , PROCESS_INDICATOR_MAIN , 7 ); /* 7 : After Place to PM */ \
\
\
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
								if ( outcheck == -1 ) { \
									return SYS_ABORTED; \
								} \
								else if ( outcheck == 3 ) { \
									s1p = 0; \
									while( TRUE ) { \
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP LOOP 7 rckside=%d,ch=%d,outcheck=%d,forcespawn=%d,s1p=%d,pres=%d\n" , ##ZZZ## + 1 , rckside , ch , outcheck , forcespawn , s1p , pres ); \
										if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( rckside ) ) { \
											return SYS_ABORTED; \
										} \
										if ( _SCH_EQ_PROCESS_FUNCTION_STATUS( rckside , ch - 1 , FALSE ) == SYS_RUNNING ) { \
											break; \
										} \
										CRITICAL_AND_SLEEP_FOR_4( ##ZZZ## , 1 ) /* 2007.07.23 */ \
/*										Sleep(1); */ /* 2007.07.23 */ \
										if ( _SCH_MODULE_Get_PM_WAFER( ch - 1 , 0 ) > 0 ) { \
											s1p++; \
											if ( s1p > ONEBODY_BM_TIME_CHECK_COUNT_OT_MAIN ) break; \
										} \
									} \
									_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch ); \
									_SCH_LOG_LOT_PRINTF( rckside , "PM Process Start at %s(%d):[%s]%cPROCESS_START PM%d:%d:%s:E0227%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , 9 , ch - PM1 + 1 , wsa , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , 9 , wsa ); \
									_SCH_LOG_TIMER_PRINTF( rckside , TIMER_PM_START , wsa , ch - PM1 + 1 , pt , -1 , -1 , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , "" ); \
									_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , wsa , rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) >= _SCH_CLUSTER_Get_PathRange( rckside , pt ) ) ); \
								} \
								else { \
									if ( outcheck == 2 ) { \
										rckside = _SCH_MODULE_Get_PM_SIDE( ch , 0 ); \
										wsa = _SCH_MODULE_Get_PM_WAFER( ch , 0 ); \
										pt = _SCH_MODULE_Get_PM_POINTER( ch , 0 ); \
										ch = ch - 1; \
										SCH_Order = -1; \
										for ( s1p = 0 ; s1p < MAX_PM_CHAMBER_DEPTH ; s1p++ ) { \
											if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == ch ) { \
												SCH_Order = s1p; \
												break; \
											} \
											if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == -ch ) { \
												SCH_Order = s1p; \
												break; \
											} \
										} \
										if ( SCH_Order == -1 ) { \
											for ( s1p = 0 ; s1p < MAX_PM_CHAMBER_DEPTH ; s1p++ ) { \
												if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == ( ch + 1 ) ) { \
													SCH_Order = s1p; \
													break; \
												} \
												if ( _SCH_CLUSTER_Get_PathProcessChamber( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , s1p ) == -( ch + 1 ) ) { \
													SCH_Order = s1p; \
													break; \
												} \
											} \
											if ( SCH_Order == -1 ) { \
												_SCH_LOG_LOT_PRINTF( rckside , "PM Process Recipe Fail at %s(%d)%cPROCESS_FAIL PM%d:%d::E114%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , 9 , ch - PM1 + 1 , wsa , 9 , wsa ); \
												return _SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , ( ##ZZZ## == 0 ) ? FALSE : TRUE ); \
											} \
										} \
									} \
									sprintf( RunRecipe , "%d" , outcheck ); \
									SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( rckside , pt , ch ); /* 2008.02.28*/ \
									if ( outcheck != 0 ) SCHEDULER_CONTROL_PRCSWAITING_10SEC( ch , -1 ); \
									_SCH_MACRO_MAIN_PROCESS( rckside , pt , \
																	  ch , -1 , \
																	  wsa , -1 , SCH_No , -1 , \
																	  _SCH_CLUSTER_Get_PathProcessRecipe( rckside , pt , _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , 0 ) , _SCH_TIMER_GET_ROBOT_TIME_RUNNING( ##ZZZ## , SCH_No ) , RunRecipe , PROCESS_DEFAULT_MINTIME , \
																	  _SCH_CLUSTER_Get_PathDo( rckside , pt ) - 1 , SCH_Order , ( _SCH_CLUSTER_Get_PathDo( rckside , pt ) >= _SCH_CLUSTER_Get_PathRange( rckside , pt ) ) , \
																	  -1 , -1 , -1 , outcheck ,  \
																	  0 , 402 ); \
								} \
								_SCH_CLUSTER_Set_PathStatus( rckside , pt , SCHEDULER_RUNNING2 ); /* 2006.09.05 2006.11.29 */ \
							} \
							else { \
\
\
								if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
								_SCH_SDM_Set_CHAMBER_INC_COUNT( pt ); \
								pres = FALSE; \
								switch ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_STYLE( pt ) ) { \
								case 3 : \
									SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTRUN , rckside , ch ) , SDM4_PTYPE_LOTRUN , RunRecipe , 64 ); \
									if ( RunRecipe[0] == 0 ) { /* 2006.09.03 */ \
										pres = FALSE; \
									} \
									else { \
										pres = _SCH_EQ_SPAWN_PROCESS( rckside , -1 , ch , FM_CM , wsa , SCH_No , RunRecipe , 0 , 0 , "D" , PROCESS_DEFAULT_MINTIME );\
									} \
									break; \
								case 1 : \
									SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , rckside , ch ) , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 ); \
									SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , rckside , ch ) , ch ); \
									SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( ch , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST , rckside , ch ) ); /* 2010.09.03 */ \
									\
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( ch , FALSE ); /* 2005.09.23 */ \
									if ( RunRecipe[0] == 0 ) { /* 2006.09.03 */ \
										pres = FALSE; \
									} \
									else { \
										pres = _SCH_EQ_SPAWN_PROCESS( rckside , -1 , ch , FM_CM , wsa , SCH_No , RunRecipe , 0 , 1 , "F" , PROCESS_DEFAULT_MINTIME );\
									} \
									break; \
								case 4 : \
								case 2 : \
									if ( SCHEDULER_CONTROL_Pre_Force_Flag_End_for_STYLE_4_Part( rckside , ch ) == 0 ) { /* 2006.01.13 */ \
										SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( -1 , ch ); /* 2006.07.21 */ \
									} \
									SCHEDULER_CONTROL_Get_SDM_4_CHAMER_RUN_RECIPE( pt , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTEND , rckside , ch ) , SDM4_PTYPE_LOTEND , RunRecipe , 64 ); \
									SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOT_CYCLE_FALSE( ch , FALSE ); /* 2005.09.23 */ \
									if ( RunRecipe[0] == 0 ) { /* 2006.09.03 */ \
										pres = FALSE; \
									} \
									else { \
										pres = _SCH_EQ_SPAWN_PROCESS( rckside , -1 , ch , FM_CM , wsa , SCH_No , RunRecipe , 0 , 2 , "E" , PROCESS_DEFAULT_MINTIME );\
									} \
									break; \
								} \
								if ( RunRecipe[0] != 0 ) { /* 2006.09.03 */ \
									if ( pres ) { \
										SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_OUT( pt ); \
										_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch ); \
										_SCH_LOG_LOT_PRINTF( rckside , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:D%d:%s:E063%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
										_SCH_LOG_TIMER_PRINTF( rckside , TIMER_PM_START , _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , ch - PM1 + 1 , -1 , -1 , -1 , RunRecipe , "" ); \
										_SCH_MACRO_PROCESS_PART_TAG_SETTING( ch , _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , rckside , -1 , -1 , -1 , RunRecipe , 1 ); \
									} \
									else { \
										_SCH_LOG_LOT_PRINTF( rckside , "PM Process Fail at %s(D%d)[%s]%cPROCESS_FAIL PM%d:D%d:%s:E064%cD%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , wsa , RunRecipe , 9 , ch - PM1 + 1 , wsa , RunRecipe , 9 , wsa ); \
									} \
								} \
							} \
						} \
					} \
				} \
				else { \
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Place %s Skip2 with %d%cWHTM%dPLACESKIP PM%d:%d\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( ch ) , placeskip , 9 , ##ZZZ## + 1 , ch - PM1 + 1 , placeskip ); \
				} \
			} \
			else { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d PICK_BM_X_PLACE_TO_PM 101 pickskip=%d , bmpickres=%d\n" , ##ZZZ## + 1 , pickskip , bmpickres ); \
\
				SCHEDULER_CONTROL_Chk_MULTIJOB_PAUSE_FOR_4( SCH_Chamber ); \
			}


//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//=====================================================================================================================================================================
#define MAKE_SCHEDULER_BM_CHANGE_TO_VENT_PART_for_4( ZZZ ) \
\
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d BM_CHANGE_TO_VENT runaction=%d\n" , ##ZZZ## + 1 , runaction ); \
\
\
\
		if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) return 0; /* 2017.04.11 */ \
\
\
		SCH_Chamber = BM1 + ##ZZZ##; \
		if ( runaction || SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( SCH_Chamber - BM1 + PM1 ) ) { \
\
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d BM_CHANGE_TO_VENT 1 runaction=%d\n" , ##ZZZ## + 1 , runaction ); \
\
			if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_VENT_FOR_4_##ZZZ##( CHECKING_SIDE , SCH_Chamber , FALSE , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( SCH_Chamber - BM1 + PM1 ) ) == 0 ) { \
\
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d BM_CHANGE_TO_VENT 2 runaction=%d\n" , ##ZZZ## + 1 , runaction ); \
\
				SCHEDULER_CONTROL_BM_VENTING_PART_##ZZZ##( CHECKING_SIDE , FALSE , SCH_Chamber , 1 ); \
			} \
		}




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_4( int CHECKING_SIDE , int mode ) {
	if      ( mode == 0 ) {
		return -1;
	}
	else if ( mode == 1 ) {
		return -1;
	}
	else if ( mode == 2 ) {
		if ( _SCH_MODULE_Get_TM_Switch_Signal( 0 ) > 0 ) { // 2004.09.07
			return SYS_RUNNING;
		}
		//----------------------------------------------------------------------
		_SCH_MACRO_INTERRUPT_PART_CHECK( CHECKING_SIDE , TRUE , -1 );
		//----------------------------------------------------------------------
		if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
			_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , TRUE );
			if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
				if ( _SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) {
					if ( _SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) == 0 ) {
						//_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Finished ....%cWHTMSUCCESS\n" , 9 ); // 2002.07.10
						return SYS_SUCCESS;
					}
				}
			}
		}
		//=====================================================================================
		// 2007.11.08
		//=====================================================================================
		_SCH_SUB_Chk_GLOBAL_STOP( -1 ); // 2007.11.08
		//=====================================================================================
		return 0;
	}
	return -1;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int USER_DLL_SCH_INF_RUN_CONTROL_TM_1_STYLE_4( int CHECKING_SIDE ) {
	int j;
	int wsa , wsb , pt , cf , ch , pres , subres , pickskip , placeskip , rckside = 0 , dummypost , bmpickres , s1p , outcheck;
	int SCH_No = -1 , SCH_Type , SCH_Order , SCH_Chamber , SCH_Slot;
	int SCH_Out_Not_Run = 0;
	int FM_CM;
	int NextYes , NextPMYes , swmode , dummode;
	int runaction;
//	char RunRecipe[65]; // 2007.01.04
	char RunRecipe[4096+1]; // 2007.01.04
	int prcs_time , post_time;
	int forcespawn;
	int SID_Get , prsts , tmo;
	//
	//--------------------------------------------------
	bmpickres = SCHEDULER_MAKE_LOCK_FREE_for_4( CHECKING_SIDE , 0 , 0 );
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 + 1 , "TM%d MAKE_LOCK_FREE %d\n" , 0 + 1 , bmpickres );
	if ( bmpickres < 0 ) return 0;
	//--------------------------------------------------
	MAKE_SCHEDULER_HEAD_PRE_END_DUMMY_for_4( 0 )
	//--------------------------------------------------
	MAKE_SCHEDULER_PICK_FROM_PM_PART_for_4( 0 )
	//--------------------------------------------------
	MAKE_SCHEDULER_PLACE_TO_BM_PART_for_4( 0 )
	//--------------------------------------------------
	SCH_Chamber = BM1 + 0;
	//----------------------------------------------------------------------
	SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4_0( CHECKING_SIDE , SCH_Chamber , &pickskip , &runaction );
	//---
	bmpickres = SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4_0( CHECKING_SIDE , SCH_Chamber , &SCH_Slot , &SCH_Order );
	if ( pickskip || bmpickres ) {
		placeskip = 0;
		if ( !pickskip ) {
			//--------------------------------------------------
			MAKE_SCHEDULER_PICK_FROM_BM_PART_for_4( 0 )
			//--------------------------------------------------
		}
		//--------------------------------------------------
		MAKE_SCHEDULER_PLACE_TO_PM_AFTER_CHECKED_PICKBM_PART_for_4( 0 )
		//--------------------------------------------------
	}
	else {
		//==================================================================================================
		MAKE_SCHEDULER_PICK_BM_X_PLACE_TO_PM_PART_for_4( 0 )
		//==================================================================================================
	}
	//==================================================================================================
	MAKE_SCHEDULER_BM_CHANGE_TO_VENT_PART_for_4( 0 )
	//==================================================================================================
	return 1;
}


#define MAKE_SCHEDULER_MAIN_SUB_TM_for_4( ZZZ , ZZZ2 ) int USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ2##_STYLE_4( int CHECKING_SIDE ) { \
	int j , wsa , wsb , pt , cf , ch , pres , subres , pickskip , placeskip , rckside = 0 , dummypost , bmpickres , s1p , outcheck; \
	int SCH_No = -1 , SCH_Type , SCH_Order , SCH_Chamber , SCH_Slot; \
	int SCH_Out_Not_Run = 0; \
	int FM_CM; \
	int NextYes , NextPMYes , swmode , dummode; \
	int runaction; \
	char RunRecipe[4096+1]; \
	int prcs_time , post_time; \
	int forcespawn; \
	int SID_Get , prsts , tmo; \
\
	if ( _SCH_MODULE_Get_TM_Switch_Signal( ##ZZZ## ) > 0 ) { /* 2004.09.07 */ \
		return 1; \
	} \
\
	bmpickres = SCHEDULER_MAKE_LOCK_FREE_for_4( CHECKING_SIDE , ##ZZZ## , 0 ); \
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d MAKE_LOCK_FREE %d\n" , ##ZZZ## + 1 , bmpickres ); \
	if ( bmpickres < 0 ) return 0; \
\
	MAKE_SCHEDULER_HEAD_PRE_END_DUMMY_for_4( ##ZZZ## ) \
\
	MAKE_SCHEDULER_PICK_FROM_PM_PART_for_4( ##ZZZ## ) \
\
	MAKE_SCHEDULER_PLACE_TO_BM_PART_for_4( ##ZZZ## ) \
\
	SCH_Chamber = BM1 + ##ZZZ##; \
\
	SCHEDULER_CONTROL_Chk_BM_PICK_DISABLE_FOR_4_##ZZZ##( CHECKING_SIDE , SCH_Chamber , &pickskip , &runaction ); \
\
	bmpickres = SCHEDULER_CONTROL_Chk_BM_PICK_POSSIBLE_FOR_4_##ZZZ##( CHECKING_SIDE , SCH_Chamber , &SCH_Slot , &SCH_Order ); \
	if ( pickskip || bmpickres ) { \
		placeskip = 0; \
		if ( !pickskip ) { \
\
			_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 11 ); /* 2007.07.05 */ \
\
			MAKE_SCHEDULER_PICK_FROM_BM_PART_for_4( ##ZZZ## ) \
\
		} \
\
		MAKE_SCHEDULER_PLACE_TO_PM_AFTER_CHECKED_PICKBM_PART_for_4( ##ZZZ## ) \
\
	} \
	else { \
\
		MAKE_SCHEDULER_PICK_BM_X_PLACE_TO_PM_PART_for_4( ##ZZZ## ) \
\
	} \
\
	MAKE_SCHEDULER_BM_CHANGE_TO_VENT_PART_for_4( ##ZZZ## ) \
\
	return 1; \
}

MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 1 , 2 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 2 , 3 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 3 , 4 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 4 , 5 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 5 , 6 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 6 , 7 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_4( 7 , 8 );
#endif
#endif

//###################################################################################################
// Include End
//###################################################################################################
#else
//###################################################################################################
int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_4( int CHECKING_SIDE , int mode ) { return -1; }
int USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_4( int CHECKING_SIDE ) { return 0; }

int USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_4( int CHECKING_SIDE , int mode ) { return -1; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_1_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_2_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_3_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_4_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_5_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_6_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_7_STYLE_4( int CHECKING_SIDE ) { return 0; }
int USER_DLL_SCH_INF_RUN_CONTROL_TM_8_STYLE_4( int CHECKING_SIDE ) { return 0; }


#endif
