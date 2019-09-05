//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_system_tag.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_sch_preprcs.h"
#include "INF_sch_prepost.h"
#include "INF_MultiJob.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A4_default.h"
#include "sch_A4_param.h"
#include "sch_A4_sub.h"
#include "sch_A4_sdm.h"
#include "sch_A4_subBM.h"
//================================================================================


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern int SDM_4_LOTFIRST_SIGNAL[ MAX_CHAMBER ];
extern int sch4_RERUN_FST_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_FST_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_END_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_END_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
extern int sch4_RERUN_END_S3_TAG[ MAX_CHAMBER ]; // 2005.11.11

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int             SDM_4_PM_LAST_PROCESS_SPECIAL_DATA[MAX_CHAMBER]; // 2005.01.06

int				SDM_4_LOTSPECIAL_TAG[ MAX_CHAMBER ]; // 2005.07.20

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( int ch ) {
	if ( ch >= BM1 ) { // 2008.02.11
		if ( ( ( ch - BM1 ) % 2 ) == 0 )
			return ch + 1;
		else
			return ch - 1;
	}
	else {
		if ( ( ( ch - PM1 ) % 2 ) == 0 )
			return ch + 1;
		else
			return ch - 1;
	}
}


int SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( int ch ) { // 2006.01.12
	if ( ch >= BM1 ) { // 2008.02.11
		if ( ( ( ch - BM1 ) % 2 ) == 0 ) return 1;
	}
	else { // 2008.02.11
		if ( ( ( ch - PM1 ) % 2 ) == 0 ) return 1;
	}
	return -1;
}


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// 2005.07.20
//------------------------------------------------------------------------------------
BOOL SCHEDULING_CONTROL_Check_LotSpecial_Item_Change_for_STYLE_4( int currside , int currpt , int willch , int *sp_old , int *sp_new ) {
	*sp_old = SDM_4_LOTSPECIAL_TAG[ willch ];
	*sp_new = _SCH_CLUSTER_Get_LotSpecial( currside , currpt );
	//
	if ( SDM_4_LOTSPECIAL_TAG[ willch ] == -1 ) return FALSE;
	if ( SDM_4_LOTSPECIAL_TAG[ willch ] != _SCH_CLUSTER_Get_LotSpecial( currside , currpt ) ) return TRUE;
	return FALSE;
}

void SCHEDULING_CONTROL_Check_LotSpecial_Item_Set_for_STYLE_4( int currside , int currpt , int willch ) {
	SDM_4_LOTSPECIAL_TAG[ willch ] = _SCH_CLUSTER_Get_LotSpecial( currside , currpt );
}

void SCHEDULING_CONTROL_Check_LotSpecial_Item_ForceSet_for_STYLE_4( int data , int willch ) {
	SDM_4_LOTSPECIAL_TAG[ willch ] = data;
}

//------------------------------------------------------------------------------------------
// SDM4 DUMMY Special Get
// 2005.01.06
//------------------------------------------------------------------------------------------

void SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( int side , int pt , int ch ) {
	if ( side < 0 ) return;
	if ( pt < 0 ) return;
	if ( pt >= 100 ) return;
	SDM_4_PM_LAST_PROCESS_SPECIAL_DATA[ch] = _SCH_CLUSTER_Get_LotSpecial( side , pt );
}

void SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_FORCE_DATA_for_STYLE_4( int ch , int data ) {
	SDM_4_PM_LAST_PROCESS_SPECIAL_DATA[ch] = data;
}

int SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( int mode , int side , int ch ) {
	int i;
	if ( mode == SDM4_PTYPE_LOTFIRST0 ) { // 2005.10.26
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
				return _SCH_CLUSTER_Get_LotSpecial( side , i );
			}
		}
		return 0;
	}
	else if ( mode == SDM4_PTYPE_LOTFIRST ) {
		if ( SDM_4_LOTSPECIAL_TAG[ ch ] == -1 ) { // 2005.09.22
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
					return _SCH_CLUSTER_Get_LotSpecial( side , i );
				}
			}
			return 0;
		}
		else { // 2005.09.22
			return SDM_4_LOTSPECIAL_TAG[ ch ];
		}
	}
	else {
		if ( SDM_4_PM_LAST_PROCESS_SPECIAL_DATA[ch] < 0 ) return 0;
		return SDM_4_PM_LAST_PROCESS_SPECIAL_DATA[ch];
	}
}
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================


//=================================================================================================================
BOOL SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( int ch , int mp , int *n ) { // 2007.01.06
/*	switch( mc ) {
	case 0 :
		*n1 = 0;
		*n2 = 0;
		return FALSE;
		break;
	case 1 :
		*n1 = 0;
		*n2 = 0;
		return FALSE;
		break;
	case 2 :
		*n1 = 0;
		*n2 = 0;
		return FALSE;
		break;
	case 3 :
		*n1 = 0;
		*n2 = 0;
		return FALSE;
	case 4 :
*/
		if ( ( mp < 0 ) || ( mp > 1 ) ) return FALSE;
		//
		switch( ( ch - BM1 ) % 4 ) {
		case 0 : // BM1=3,4
			*n = ch + 2 + mp;
			break;
		case 1 : // BM2=3,4
			*n = ch + 1 + mp;
			break;
		case 2 : // BM3=1,2
			*n = ch - 2 + mp;
			break;
		case 3 : // BM4=1,2
			*n = ch - 3 + mp;
			break;
		}
		return TRUE;
/*	}
	return FALSE;
*/
}

//===============================================================================

BOOL SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() {
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 15 ) return TRUE;
	return FALSE;
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( int BMmode ) { // 2005.09.09
	//--------------------------------------------------------------------------------
//	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return 0; // 2006.12.15
	//--------------------------------------------------------------------------------
	if ( BMmode ) {
		switch( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() ) { // 2005.10.04
		case 2 : // Default.OneBody
		case 3 : // Thread.OneBody
		case 6 : // Default.OneBody(PM) // 2005.10.11
		case 7 : // Thread.OneBody(PM) // 2005.10.11
		case 10 : // Default.OneBody(PS) // 2005.10.11
		case 11 : // Thread.OneBody(PS) // 2005.10.11
			return 1;
		case 4 : // Default.OneBody(2)
		case 5 : // Thread.OneBody(2)
		case 8 : // Default.OneBody(2PM) // 2005.10.11
		case 9 : // Thread.OneBody(2PM) // 2005.10.11
		case 12 : // Default.OneBody(2PS) // 2005.10.11
		case 13 : // Thread.OneBody(2PS) // 2005.10.11
			return 2; // 2005.10.11
		case 14 : // Thread.OneBody[S](PS) // 2006.12.18
		case 15 : // Thread.OneBody[S](PSW) // 2007.08.21
			return 3; // 2006.12.18
		}
	}
	else {
		switch( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() ) { // 2005.10.11
		case 2 : // Default.OneBody
		case 3 : // Thread.OneBody
		case 4 : // Default.OneBody(2)
		case 5 : // Thread.OneBody(2)
			return 0;
		case 6 : // Default.OneBody(PM) // PM
		case 7 : // Thread.OneBody(PM)
		case 8 : // Default.OneBody(2PM)
		case 9 : // Thread.OneBody(2PM)
			return 1;
		case 10 : // Default.OneBody(PS) // PS
		case 11 : // Thread.OneBody(PS)
		case 12 : // Default.OneBody(2PS)
		case 13 : // Thread.OneBody(2PS)
			return 2;
		case 14 : // Thread.OneBody[S](PS) // 2006.12.18
		case 15 : // Thread.OneBody[S](PSW) // 2007.08.21
			return 3; // 2007.05.10
		}
	}
	//--------------------------------------------------------------------------------
	return 0;
}
//
int SCHEDULING_CHECK_Chk_All_Wafer_is_Nothing_for_STYLE_4( int pmc , int *pt ) { // 2006.01.17
	int i , l , y = FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return 0;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , -1 ) ) return 0;
	//
	for ( l = 0 ; l < 2 ; l++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pmc , 0 ) > 0 ) {
			*pt = _SCH_MODULE_Get_PM_POINTER( pmc , 0 );
			if ( *pt >= 100 ) return 2;
			y = TRUE;
		}
		if ( _SCH_MODULE_Get_TM_WAFER( pmc - PM1 , TA_STATION ) > 0 ) {
			*pt = _SCH_MODULE_Get_TM_POINTER( pmc - PM1 , TA_STATION );
			if ( *pt >= 100 ) return 2;
			y = TRUE;
		}
		if ( _SCH_MODULE_Get_TM_WAFER( pmc - PM1 , TB_STATION ) > 0 ) {
			*pt = _SCH_MODULE_Get_TM_POINTER( pmc - PM1 , TB_STATION );
			if ( *pt >= 100 ) return 2;
			y = TRUE;
		}
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( pmc - PM1 + BM1 ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( pmc - PM1 + BM1 , i ) > 0 ) {
				*pt = _SCH_MODULE_Get_BM_POINTER( pmc - PM1 + BM1 , i );
				if ( *pt >= 100 ) return 2;
				y = TRUE;
			}
		}
		//=========================================================================================
		// 2006.01.24
		//=========================================================================================
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			*pt = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
			if ( *pt >= 100 ) {
				if ( _SCH_SDM_Get_RUN_CHAMBER( *pt % 100 ) == pmc ) return 2;
			}
		}
		//
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			*pt = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
			if ( *pt >= 100 ) {
				if ( _SCH_SDM_Get_RUN_CHAMBER( *pt % 100 ) == pmc ) return 2;
			}
		}
		//=========================================================================================
	}
	if ( y ) return 1;
	return 0;
}





//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
// Include Start
//###################################################################################################

//==========================================================================================================================
BOOL SCHEDULER_CONTROL_PROCESS_MONITOR_Run_with_Properly_Status( int ch , int exppart , BOOL notruncheck ) { // 2006.11.28
	int status;
	//
	status = _SCH_MACRO_CHECK_PROCESSING_INFO( ch );
	//
	if ( status > 0 ) {
		if ( status < PROCESS_INDICATOR_PRE ) { // main
			if      ( exppart == PROCESS_INDICATOR_POST ) { // post
				return FALSE;
			}
			else if ( exppart == PROCESS_INDICATOR_PRE ) { // pre
				return FALSE;
			}
			else { // main
				if ( notruncheck ) return TRUE; // 2007.04.03
			}
		}
		else if ( ( status >= PROCESS_INDICATOR_PRE ) && ( status < PROCESS_INDICATOR_POST ) ) { // pre
			if      ( exppart == PROCESS_INDICATOR_POST ) { // post
				return FALSE;
			}
			else if ( exppart == PROCESS_INDICATOR_PRE ) { // pre
				if ( notruncheck ) return TRUE; // 2007.04.03
			}
			else { // main
				return FALSE;
			}
		}
		else if ( status >= PROCESS_INDICATOR_POST ) { // post
			if      ( exppart == PROCESS_INDICATOR_POST ) { // post
				if ( notruncheck ) return TRUE; // 2007.04.03
			}
			else if ( exppart == PROCESS_INDICATOR_PRE ) { // pre
				return FALSE;
			}
			else { // main
				return FALSE;
			}
		}
	}
	//
	if ( notruncheck ) return FALSE; // 2007.04.03
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_for_STYLE_4( int ch , BOOL freecheck ) {
	int i , x = 0;
	//===============================================================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) == 0 ) return TRUE; // 2006.07.10
	//===============================================================================================
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
//			x++; // 2006.11.15
		}
		else {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
			x++; // 2006.11.15
		}
	}
	if ( freecheck ) {
//		if ( x > 1 ) return FALSE; // 2006.11.15
		if ( x <= 0 ) return FALSE; // 2006.11.15
	}
	return TRUE;
}

//=================================================================================================================

int	sch4_BM_PROCESS_MONITOR[ MAX_CHAMBER ];
int	sch4_BM_PROCESS_CHECKING[ MAX_CHAMBER ];
int	sch4_BM_PROCESS_MODE[ MAX_CHAMBER ];


BOOL SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( int ch ) {
	int nch , mp;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
		EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
		//
		//=================================================================================================
		mp = 0;
		nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
		while( TRUE ) {
			//===================================================
			if ( sch4_BM_PROCESS_MONITOR[ ch ] && !sch4_BM_PROCESS_MONITOR[ nch ] ) { // 2006.10.26
				LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
				return TRUE;
			}
			//===================================================
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
			//===================================================
		}
		//=================================================================================================
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return TRUE;
		}
		if ( sch4_BM_PROCESS_MONITOR[ ch ] ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return TRUE;
		}
		//=================================================================================================
		mp = 0;
		nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
		while( TRUE ) {
			//===================================================
			if ( sch4_BM_PROCESS_MONITOR[ nch ] ) {
				if ( sch4_BM_PROCESS_CHECKING[ nch ] ) {
					sch4_BM_PROCESS_MONITOR[ nch ] = FALSE;
					sch4_BM_PROCESS_CHECKING[ nch ] = FALSE;
				}
			}
			//===================================================
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
			//===================================================
		}
		//=================================================================================================
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
		return FALSE;
	}
	else {
		EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
		//
		nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch ); // 2006.10.26
		//
	//	if ( sch4_BM_PROCESS_MONITOR[ ch ] ) { // 2006.10.26
		if ( sch4_BM_PROCESS_MONITOR[ ch ] && !sch4_BM_PROCESS_MONITOR[ nch ] ) { // 2006.10.26
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return TRUE;
		}
		//
	//	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch ); // 2006.10.26
		//
		if ( sch4_BM_PROCESS_MONITOR[ nch ] ) {
			if ( sch4_BM_PROCESS_CHECKING[ nch ] ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) {
					sch4_BM_PROCESS_MONITOR[ nch ] = FALSE;
					sch4_BM_PROCESS_CHECKING[ nch ] = FALSE;
					LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
					return FALSE;
				}
				else {
					LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
					return TRUE;
				}
			}
		}
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) {
			if ( sch4_BM_PROCESS_MONITOR[ ch ] ) {
				LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
				return TRUE;
			}
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return FALSE;
		}
		else {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return TRUE;
		}
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	}
	return FALSE;
}

int SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Res_for_STYLE_4( int side , int ch ) {
	int r;
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	if ( sch4_BM_PROCESS_MONITOR[ ch ] ) {
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
		return SYS_RUNNING;
	}
	r = _SCH_EQ_PROCESS_FUNCTION_STATUS( side , ch , FALSE );
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	return r;
}

void SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( int ch , BOOL ventmode ) {
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	sch4_BM_PROCESS_CHECKING[ ch ] = FALSE;
	sch4_BM_PROCESS_MODE[ ch ] = ventmode;
	sch4_BM_PROCESS_MONITOR[ ch ] = TRUE;
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
}

int SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_nch_for_STYLE_4( int ch , BOOL ventmode ) {
	int nch , mp;
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	//===================================================
	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
	mp = 0;
	while( TRUE ) {
		//===================================================
		sch4_BM_PROCESS_CHECKING[ nch ] = TRUE;
		sch4_BM_PROCESS_MODE[ nch ] = ventmode;
		sch4_BM_PROCESS_MONITOR[ nch ] = TRUE;
		//===================================================
		if ( ventmode ) {
			_SCH_MODULE_Set_BM_FULL_MODE( nch , BUFFER_FM_STATION );
		}
		else {
			_SCH_MODULE_Set_BM_FULL_MODE( nch , BUFFER_TM_STATION );
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
		}
		else {
			break;
		}
		//
	}
	//===================================================
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	return nch;
}

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( int ch , BOOL ventmode ) { // 2006.01.11
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	if ( sch4_BM_PROCESS_CHECKING[ ch ] ) {
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
		return FALSE;
	}
	if ( sch4_BM_PROCESS_MODE[ ch ] != ventmode ) {
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
		return FALSE;
	}
	if ( !sch4_BM_PROCESS_MONITOR[ ch ] ) {
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
		return FALSE;
	}
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( int ch , BOOL ventmode ) {
	int nch , mp;
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );

	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
	mp = 0;
	while( TRUE ) {
		if ( !sch4_BM_PROCESS_MONITOR[ nch ] ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return FALSE;
		}
		if ( sch4_BM_PROCESS_MODE[ nch ] != ventmode ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return FALSE;
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
		}
		else {
			break;
		}
		//
	}
//	sch4_BM_PROCESS_CHECKING[ nch ] = TRUE; // 2005.10.05
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	return TRUE;
}

void SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( int ch , BOOL ventmode ) {
	int nch , mp;
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );

	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
	mp = 0;
	while( TRUE ) {
		if ( ( sch4_BM_PROCESS_MONITOR[ nch ] ) && ( sch4_BM_PROCESS_MODE[ nch ] == ventmode ) ) {
			sch4_BM_PROCESS_CHECKING[ nch ] = TRUE;
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
		}
		else {
			break;
		}
		//
	}
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
}

void SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( int ch , BOOL ventmode ) { // 2007.05.11
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	sch4_BM_PROCESS_CHECKING[ ch ] = FALSE;
	sch4_BM_PROCESS_MODE[ ch ] = ventmode;
	sch4_BM_PROCESS_MONITOR[ ch ] = FALSE;
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
}

BOOL SCHEDULER_CONTROL_Chk_ONEBODY_BM_Other_Wait_RealRun_for_STYLE_4( int ch , BOOL ventmode ) { // 2005.10.05
	int nch , mp;
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
	mp = 0;
	while ( TRUE ) {
		if ( !sch4_BM_PROCESS_MONITOR[ nch ] ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return FALSE;
		}
		if ( sch4_BM_PROCESS_MODE[ nch ] != ventmode ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return FALSE;
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06 // More Check Need
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
		}
		else {
			break;
		}
		//
	}
	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_ONEBODY_All_Not_Running_for_STYLE_4( int ch , BOOL ventmode ) {
	int nch , mp;
	EnterCriticalSection( &CR_MULTI_BM_STYLE_4 );
	//

	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) {
		LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
		return FALSE;
	}

	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( ch );
	mp = 0;
	while( TRUE ) { // 2007.01.06
		
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( nch ) > 0 ) {
			LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
			return FALSE;
		}
		//
		if ( sch4_BM_PROCESS_MONITOR[ nch ] ) {
			if ( sch4_BM_PROCESS_MODE[ nch ] != ventmode ) {
				LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
				return FALSE;
			}
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
			if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( ch , mp , &nch ) ) break;
			mp++;
		}
		else {
			break;
		}
	}

	LeaveCriticalSection( &CR_MULTI_BM_STYLE_4 );
	return TRUE;
}

//=======================================================================================

int SCHEDULER_CONTROL_Chk_ARM_WHAT( int tms , int mode ) {
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TB_STATION ) ) {
		switch ( mode ) {
		case CHECKORDER_FOR_FM_PICK :
			return TA_STATION;
		case CHECKORDER_FOR_FM_PLACE :
			return TB_STATION;
		case CHECKORDER_FOR_TM_PICK :
			return TB_STATION;
		case CHECKORDER_FOR_TM_PLACE :
			return TA_STATION;
		}
	}
	else {
		return TA_STATION;
	}
	return TA_STATION;
}

BOOL SCHEDULER_CONTROL_Chk_ARM_DUAL_MODE( int tms ) { // 2005.06.28
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TB_STATION ) ) return TRUE;
	return FALSE;
}

//=======================================================================================
//=======================================================================================
BOOL SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( int side , int pt , int bmc , int cldx_side , int cldx_pt ) { // 2006.11.30
	int i , j , m;
	for ( i = pt ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			if ( _SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_READY ) {
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					m = _SCH_CLUSTER_Get_PathProcessChamber( side , i , 0 , j );
					if ( m == ( bmc - BM1 + PM1 ) ) {
						if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
							if ( cldx_side == -1 ) {
								return TRUE;
							}
							else {
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( cldx_side , cldx_pt , side , i ) ) { // 2007.05.28
									return TRUE;
								}
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL SCHEDULER_TM_HAS_WAFER_SUPPLY( int ch ); // 2011.01.28
//
BOOL SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( int ch , int *s1 , int orderoption ) {
	int i;
	*s1 = 0;
	//
	if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( ch ) ) { // 2011.01.28
		return FALSE;
	}
	//
	if ( orderoption == 2 ) { // 2003.06.02
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				*s1 = i;
				return TRUE;
			}
		}
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				*s1 = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( int ch , int *s1 , int orderoption ) { // 2007.01.17
	int i;
	*s1 = 0;
	if ( orderoption == 2 ) { // 2003.06.02
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			*s1 = i;
			return TRUE;
		}
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			*s1 = i;
			return TRUE;
		}
	}
	return FALSE;
}


//=======================================================================================
//=======================================================================================
BOOL SCHEDULING_CHECK_Curr_Data_Target_Full_All_for_STYLE_4( int side , int pointer , BOOL nccheck , int grpcheck ) { // 2006.03.10
	int i , m , c = 0;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
		if ( m > 0 ) {
			//===============================================================================================
			// 2006.11.16
			//===============================================================================================
			if ( grpcheck != -1 ) {
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) != grpcheck ) continue;
			}
			//===============================================================================================
			c++;
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( m - PM1 + BM1 ) != BUFFER_FM_STATION ) return FALSE;
				if ( !SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_4( m - PM1 + BM1 ) ) return FALSE;
			}
		}
	}
	if ( !nccheck ) {
		if ( c == 0 ) return TRUE;
		return FALSE;
	}
	return TRUE;
}


//=======================================================================================
//=======================================================================================
BOOL SCHEDULING_More_Supply_Check_for_STYLE_4( int ch , int pmcheck ) {
	int s , i;
// 2005.07.27
//	if ( pmcheck == 1 ) {
//		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) return TRUE;
//		if ( _SCH_MODULE_Get_TM_WAFER( ch - PM1 , 0 ) > 0 ) return TRUE;
//		if ( _SCH_MODULE_Get_TM_WAFER( ch - PM1 , 1 ) > 0 ) return TRUE; // 2005.06.28
//		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch - PM1 + BM1 ) ; i++ ) {
//			if ( _SCH_MODULE_Get_BM_WAFER( ch - PM1 + BM1 , i ) > 0 ) return TRUE;
//		}
//	}
	if ( pmcheck >= 1 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) return TRUE;
		if ( _SCH_MODULE_Get_TM_WAFER( ch - PM1 , 0 ) > 0 ) return TRUE;
		if ( _SCH_MODULE_Get_TM_WAFER( ch - PM1 , 1 ) > 0 ) return TRUE; // 2005.06.28
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch - PM1 + BM1 ) ; i++ ) {
			if ( i != ( pmcheck - 2 ) ) { // 2005.07.27
				if ( _SCH_MODULE_Get_BM_WAFER( ch - PM1 + BM1 , i ) > 0 ) return TRUE;
			}
		}
	}
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
//		if ( _SCH_SYSTEM_USING_GET( s ) <= 0 ) continue; // 2005.07.29
//		if ( ( _SCH_SYSTEM_USING_GET( s ) < 9 ) && ( _SCH_SYSTEM_MODE_GET( s ) == 2 ) ) continue; // 2005.07.29
		if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue; // 2005.07.29
		if ( _SCH_SYSTEM_MODE_END_GET( s ) != 0 ) continue;
		if ( _SCH_MODULE_Chk_FM_Finish_Status( s ) ) {
			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
				if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == s ) {
					if ( SCHEDULER_Get_FM_WILL_GO_BM( s ) == ch ) return TRUE;
				}
			}
			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
				if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == s ) {
					if ( SCHEDULER_Get_FM_WILL_GO_BM( s ) == ch ) return TRUE;
				}
			}
			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) { // 2004.08.17
				if ( _SCH_MODULE_Get_MFAL_SIDE() == s ) {
					if ( SCHEDULER_Get_FM_WILL_GO_BM( s ) == ch ) return TRUE;
				}
			}
			continue;
		}
		if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( s , 0 , ch - PM1 + BM1 , -1 , -1 ) ) return TRUE; // 2006.11.30
	}
	return FALSE;
}

//=======================================================================================
//=======================================================================================
int SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_Side , int *FM_Side2 , BOOL doublemode , BOOL clcheck , int s2 , int p2 ) {
	int nxp , xr , op , op2 , fh , lp;

	if ( !_SCH_SUB_Check_Complete_Return( side ) ) return -3;
	if ( !_SCH_SUB_Remain_for_FM( side ) ) return -3;

	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , _SCH_MODULE_Get_FM_DoPointer( side ) , 0 ) == 0 ) return -21;

	_SCH_MODULE_Set_FM_HWait( side , TRUE );
	*FM_Side  = 0;
	*FM_Side2 = 0;
	if ( doublemode == 0 ) { // 2004.08.20
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
			//===========================================================================================
			// 2006.12.15
			//===========================================================================================
			if ( ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
				if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
					*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
					*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
					*FM_Slot  = 0;
				}
				else {
					xr = 0;
					//
					lp = 0;
					//
					*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
					*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
					//
					fh			 = _SCH_CLUSTER_Get_PathIn( side , *FM_Pointer );
					//
					op = 0;
					op2 = *FM_Pointer + 1;
					//
					while( TRUE ) {
						while ( _SCH_SUB_Run_Impossible_Condition( side , op2 , -1 ) ) {
							op2++;
							if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) break;
						}
						if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) {
							if ( lp == 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return -21;
								xr = 2; // b
							}
							else {
								xr = 1; // a
							}
							break;
						}
						//
						if ( lp == 1 ) {
							*FM_Pointer2  = op2;
							*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , op2 );
						}
						//
						if ( _SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
							if ( lp == 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return -21;
								xr = 2; // b
							}
							else {
								xr = 1; // a
							}
							break;
						}
						if ( ( *FM_Slot + op + 1 ) != ( _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
							if ( lp == 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return -21;
								xr = 2; // b
							}
							else {
								xr = 1; // a
							}
							break;
						}
						op++;
						if ( lp == 0 ) {
							if ( op >= _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
									xr = 1; // a
									break;
								}
								lp = 1;
							}
						}
						else {
							if ( op >= ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) break;
						}
						op2++;
					}
					//
					if ( xr == 2 ) {
						*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
						*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
						*FM_Slot  = 0;
					}
					else if ( xr == 1 ) {
						*FM_Slot2 = 0;
					}
					//=========================================================================
				}
			}
			else {
				xr = 0;
				if ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) {
					*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
					*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
	//				*FM_Slot2 = 0;
					//
					fh			 = _SCH_CLUSTER_Get_PathIn( side , *FM_Pointer );
					//
					op = 0;
					op2 = *FM_Pointer + 1;
					//
					while( TRUE ) {
						while ( _SCH_SUB_Run_Impossible_Condition( side , op2 , -1 ) ) {
							op2++;
							if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) break;
						}
						if ( op2 >= MAX_CASSETTE_SLOT_SIZE ) {
							xr = 1;
							break;
						}
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
								xr = 2;
								break;
							}
							if ( ( *FM_Slot + op + 1 ) != ( _SCH_CLUSTER_Get_SlotIn( side , op2 ) ) ) {
								xr = 2;
								break;
							}
							op++;
							if ( op >= _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) break;
						}
						else {
							if ( ( _SCH_CLUSTER_Get_SlotIn( side , op2 ) > *FM_Slot ) &&
								( _SCH_CLUSTER_Get_SlotIn( side , op2 ) <= ( *FM_Slot + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) ) ) {
								if ( _SCH_CLUSTER_Get_PathIn( side , op2 ) != fh ) {
									xr = 2;
									break;
								}
							}
						}
						op2++;
					}
					//
					if ( xr != 0 ) {
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
							*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
							*FM_Slot  = 0;
						}
						else {
							if ( xr == 2 ) {
								return -21;
							}
							else {
								*FM_Slot2 = 0;
							}
						}
					}
					else {
						*FM_Slot2 = 0;
					}
					//
					xr = 1;
					//=========================================================================
				}
				//============================================================================
				else { // 2006.12.15
					*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
					*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
			//		_SCH_MODULE_Inc_FM_DoPointer( side );
					*FM_Slot2 = 0;
				}
			}
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
			*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
	//		_SCH_MODULE_Inc_FM_DoPointer( side );
			*FM_Slot  = 0;
		}
		else {
			return -4;
		}
	}
	else if ( doublemode == 1 ) { // 2004.06.25
		*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side ); // 2004.08.20
		*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer ); // 2004.08.20
		*FM_Slot2 = 0; // 2004.08.20
		//
		nxp = _SCH_MODULE_Get_FM_DoPointer( side ) + 1;
		if ( nxp < MAX_CASSETTE_SLOT_SIZE ) {
			while ( _SCH_SUB_Run_Impossible_Condition( side , nxp , -1 ) ) {
				nxp++;
				if ( nxp >= MAX_CASSETTE_SLOT_SIZE ) break;
			}
			if ( nxp < MAX_CASSETTE_SLOT_SIZE ) {
				if ( clcheck ) {
					if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( side , *FM_Pointer , side , nxp ) ) { // 2006.09.13
						*FM_Pointer2  = nxp;
						*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , nxp );
					}
				}
				else {
					*FM_Pointer2  = nxp;
					*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , nxp );
				}
			}
		}
	}
	else if ( doublemode == 2 ) { // 2004.08.20
		if ( clcheck ) { // 2006.09.13
			if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( s2 , p2 , side , _SCH_MODULE_Get_FM_DoPointer( side ) ) ) { // 2006.09.13
				*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
				*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
				*FM_Slot2 = 0;
			}
			else {
				return -51;
			}
		}
		else {
			*FM_Pointer  = _SCH_MODULE_Get_FM_DoPointer( side );
			*FM_Slot     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer );
			*FM_Slot2 = 0;
		}
	}
	else if ( doublemode == 3 ) { // 2004.08.20
		if ( clcheck ) { // 2006.09.13
			if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( s2 , p2 , side , _SCH_MODULE_Get_FM_DoPointer( side ) ) ) { // 2006.09.13
				*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
				*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
				*FM_Slot  = 0;
			}
			else {
				return -52;
			}
		}
		else {
			*FM_Pointer2  = _SCH_MODULE_Get_FM_DoPointer( side );
			*FM_Slot2     = _SCH_CLUSTER_Get_SlotIn( side , *FM_Pointer2 );
			*FM_Slot  = 0;
		}
	}
	return 1;
}



// 2002.03.25 // 0->NOT Switch 1->Switch 2->Switch but has Pre Recipe
int SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_for_STYLE_4( int TMATM , int CHECKING_SIDE , int Fingerx , int PickChamber ) {
	int k , chkch , Pointer , Side , Finger , rets;
	//
	if ( Fingerx == TA_STATION ) Finger = TB_STATION;
	else                         Finger = TA_STATION;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,Finger ) )								return 0;
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) <= 0 )			return 0;
	//
	Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,Finger );
	Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,Finger );
	//
	if ( ( PickChamber >= CM1 ) && ( PickChamber < ( CM1 + MAX_CASSETTE_SIDE ) ) ) {
		if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) {
			if ( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) == PickChamber ) return 1;
			return 0;
		}
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) {
			if ( _SCH_CLUSTER_Get_PathOut( Side , Pointer ) == PickChamber ) return 1;
			return 0;
		}
	}
	else if ( ( PickChamber >= PM1 ) && ( PickChamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
		if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) return 0;
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) return 0;
		if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) <= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
			rets = -1; // 2005.10.18
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				chkch = _SCH_CLUSTER_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
				if ( chkch > 0 ) {
					if ( chkch == PickChamber ) {
						//----------------------------------------------------------
						// 2002.03.25 // Check more for Pre Recipe
						//----------------------------------------------------------
						//return TRUE;
						if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( Side , chkch ) ) { // 2003.11.11
							rets = 2; // 2005.10.18
//									return 2; // 2005.10.18
						}
						else {
							if ( _SCH_CLUSTER_Check_Possible_UsedPre( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k , chkch , FALSE ) ) {
								if ( _SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() >= 5 ) { // 2007.10.12
									rets = 2; // 2007.10.12
								}
								else {
									rets = 2; // 2005.10.18
//											return 2; // 2005.10.18
								}
							}
							else {
								rets = 1; // 2005.10.18
//										return 1; // 2005.10.18
							}
						}
					}
					//=====================================================================================================================================
					else { // 2005.10.18
						if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k ) ) { // 2005.05.11
							return 0;
						}
					}
					//=====================================================================================================================================
				}
			}
			if ( rets != -1 ) {
				//==================================================================================================================
				// 2007.04.03
				//==================================================================================================================
				if ( rets == 1 ) {
					if ( _SCH_SUB_Check_Last_And_Current_CPName_is_Different( Side , Pointer , PickChamber ) ) rets = 2;
				}
				//==================================================================================================================
				return rets; // 2005.10.18
			}
		}
	}
	else if ( ( PickChamber >= BM1 ) && ( PickChamber < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
		if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) return 0;
		if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) return 0;
	}
	return 0;
}

BOOL SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( int s1 , int p1 , int s2 , int p2 ) {
	int i , m;
	int x1[MAX_PM_CHAMBER_DEPTH];
	int x2[MAX_PM_CHAMBER_DEPTH];

	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) return FALSE;

	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		x1[i] = FALSE;
		x2[i] = FALSE;
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( s1 , p1 , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , s1 ) ) {
				x1[m-PM1] = TRUE;
				x1[SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( m )-PM1] = TRUE;
			}
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( s2 , p2 , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , s2 ) ) {
				x2[m-PM1] = TRUE;
				x2[SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( m )-PM1] = TRUE;
			}
		}
	}
	//--------------------------------------------------------------
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( !x1[i] && x2[i] ) return TRUE;
		if ( x1[i] && !x2[i] ) return TRUE;
	}
	//--------------------------------------------------------------
	return FALSE;
}

BOOL SCHEDULING_Possible_Process_for_STYLE_4( int side , int pointer , int ch ) { // 2003.06.19
	int i , m;
	if ( ch != -1 ) { // 2006.05.04
		if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , side ) ) return FALSE;
	}
	if ( pointer < 100 ) {
		if ( ch != -1 ) { // 2006.05.04
			if ( _SCH_CLUSTER_Get_PathRange( side , pointer ) < 0 ) return FALSE;
			if ( _SCH_CLUSTER_Get_PathDo( side , pointer ) == PATHDO_WAFER_RETURN ) return FALSE;
		}
		//==========================================================
//		if ( _SCH_SYSTEM_CPJOB_GET(side) != 0 ) { // 2004.06.02
			//---------------------------------------
			// 2004.10.14
			//---------------------------------------
			while ( TRUE ) {
				if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , pointer , 2 ) != 0 ) break;
				Sleep(100);
			}
//		}
		//==========================================================
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
			if ( m > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
					if ( ch == -1 ) { // 2006.05.04
						if ( _SCH_MODULE_GET_USE_ENABLE( m - PM1 + BM1 , FALSE , side ) ) return TRUE; // 2006.05.04
					}
					else {
						if ( m == ch ) return TRUE;
					}
				}
			}
		}
		return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Check_Chamber_Run_Wait_for_STYLE_4_Part( int side , int Chamber ) { // 2006.08.30
	int i , j , k;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
			if ( _SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_READY ) {
				for ( j = 0 ; j < _SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						if ( _SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k ) == Chamber ) return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}


int SCHEDULER_CONTROL_Pre_Force_Flag_End_for_STYLE_4_Part( int CHECKING_SIDE , int Chamber ) { // 2006.01.14
	int res = 0; // 2006.07.21
	int i;
	//
//	if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2006.08.30
	if ( !_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) return 11; // 2006.08.30

//		if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) >= 2 ) { // 2006.02.04
//		if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) >= 1 ) { // 2006.02.04 // 2006.08.30
	if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) < 1 ) {
		if ( SCHEDULER_CONTROL_Check_Chamber_Run_Wait_for_STYLE_4_Part( CHECKING_SIDE , Chamber ) ) { // 2006.08.30
			return 12; // 2006.02.04 // 2006.08.30
		}
		else {
			if ( SCHEDULER_Get_FM_WILL_GO_BM( CHECKING_SIDE ) == ( Chamber - PM1 + BM1 ) ) {
				return 13; // 2006.02.04 // 2006.08.30
			}
		}
	}

//			if ( _SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , CHECKING_SIDE ) ) { // 2006.08.30
	if ( !_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , CHECKING_SIDE ) ) return 14; // 2006.08.30

//xprintf( "[A][S=%d][PM%d] " , CHECKING_SIDE , Chamber - PM1 + 1 );

//				if ( ( sch4_RERUN_END_S1_TAG[ Chamber ] == -1 ) && ( sch4_RERUN_END_S2_TAG[ Chamber ] == -1 ) && ( sch4_RERUN_FST_S1_TAG[ Chamber ] == -1 ) && ( sch4_RERUN_FST_S2_TAG[ Chamber ] == -1 ) ) { // 2006.08.30
	if ( ( sch4_RERUN_END_S1_TAG[ Chamber ] != -1 ) || ( sch4_RERUN_END_S2_TAG[ Chamber ] != -1 ) || ( sch4_RERUN_FST_S1_TAG[ Chamber ] != -1 ) || ( sch4_RERUN_FST_S2_TAG[ Chamber ] != -1 ) ) return 15; // 2006.08.30

//xprintf( "[B]" );
//					if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_ONLY_WAFER( CHECKING_SIDE , Chamber ) ) { // 2006.08.30
	if ( !SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTEND_ONLY_WAFER( CHECKING_SIDE , Chamber ) ) return 16; // 2006.08.30

//xprintf( "[C]" );
//						if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2006.08.30
	if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) != MUF_01_USE ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) < MUF_90_USE_to_XDEC_FROM ) ) return 17; // 2006.08.30

							if      ( _SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) == 1 ) {

//xprintf( ">>>>> [OK 1]" );

								_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_05_USE_to_PRENDNA_EF_LP ); // 2006.01.17
								_SCH_MODULE_Set_Mdl_Run_Flag( Chamber , 0 );
								res = 3; // 2006.08.30
							}
							else if ( _SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) > 1 ) {

//xprintf( ">>>>> [OK 2]" );

								_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_08_USE_to_PRENDNA_DECTAG ); // 2006.01.17
								_SCH_MODULE_Dec_Mdl_Run_Flag( Chamber );
								//=====================================================================
								// 2006.07.21
								//=====================================================================
								if ( _SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) == 1 ) {
									res = 0;
									for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
										if ( i != CHECKING_SIDE ) {
											if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
											if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , Chamber ) != MUF_01_USE ) continue;
											if ( _SCH_MODULE_Get_FM_OutCount( i ) != 0 ) continue;
											if ( _SCH_MODULE_Get_FM_InCount( i ) != 0 ) continue;
											if ( _SCH_MODULE_Get_TM_OutCount( i ) != 0 ) continue;
											if ( _SCH_MODULE_Get_TM_InCount( i ) != 0 ) continue;
											res = 1; // find
											SDM_4_LOTFIRST_SIGNAL[ Chamber ] = TRUE;
											break;
										}
									}
									if ( res == 1 ) res = 0; // 2006.08.30
									else            res = 1; // 2006.08.30
								}
								else { // 2006.08.30
									res = 2; // 2006.08.30
								}
							}
							else { // 2006.08.30
								res = 4; // 2006.08.30
							}
							//=============================================================================================
							_SCH_PREPOST_Randomize_End_Part( CHECKING_SIDE ,  Chamber ); // 2006.02.28
							//=============================================================================================
							return res; // 2006.08.30
						//}
					//}
				//}

//xprintf( "\n" );

			//}
		//}
	//}
//	return res; // 2006.08.30
}
//

//###################################################################################################
// Include End
//###################################################################################################
#endif
//###################################################################################################

