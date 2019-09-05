//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_iolog.h"
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
#include "INF_MultiJob.h"
#include "INF_User_Parameter.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_Timer_Handling.h"
//================================================================================
#include "sch_A6_Default.h"
#include "sch_A6_sub.h"
#include "sch_A6_subbm.h"
#include "sch_A6_init.h"
//================================================================================
extern BOOL FM_Pick_Running_Blocking_Style_6; // 2011.05.04
extern int  FM_Pick_Running_Blocking_Style_6_Side; // 2011.05.04
extern int  FM_Pick_Running_Blocking_Style_6_Pointer; // 2011.05.04

//=============================================================================================================
int SCHEDULER_CONTROL_ST6_GET_SLOT( int pmc , int side , int pt );
//=============================================================================================================

int SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( int tms , int bmc , int pmc );

//================================================================================
extern int  MODULE_MAX_SIZE_OF_TMPM[MAX_TM_CHAMBER_DEPTH]; // 2014.11.14
//================================================================================

int SIZE_OF_CYCLON_PM_DEPTH( int ch ) { // 2014.11.14
	int i;
	//
	if ( ( ch >= PM1 ) && ( ch < AL ) ) {
		i = _SCH_PRM_GET_MODULE_SIZE( ch );
		//
		if ( i <= 1 )
			return MAX_CYCLON_PM_DEPTH;
		else
			return i;
	}
	else if ( ( ch >= TM ) && ( ch < FM ) ) {
		return MODULE_MAX_SIZE_OF_TMPM[ch-TM];
	}
	else {
		return _SCH_PRM_GET_MODULE_SIZE( ch );
	}
	//
}

BOOL SCHEDULING_ThisIs_BM_OtherChamber6( int ch , int mode ) { // 2014.07.08
	if ( ( mode == 0 ) || ( mode == 1 ) ) {
		if ( ch == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return TRUE;
	}
	if ( ( mode == 0 ) || ( mode == 2 ) ) {
		if ( ch == _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return TRUE;
	}
	if ( ( ch >= BM1 ) && ( ch < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE ) return TRUE;
		}
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE2 ) return TRUE;
		}
	}
	return FALSE;
}


int Scheduler_BM_Get_First_for_CYCLON( int tms ) { // 2010.04.26
	int i;
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
		//
		for ( i = BM1 ; i < TM ; i++ ) {
			if ( i != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
				if ( ( tms + BM1 ) != i ) {
					if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
							return i;
						}
					}
				}
			}
		}
	}
	else { // 2013.01.25
		//
		for ( i = ( BM1 + 4 ) ; i < TM ; i++ ) {
			if ( i != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
				if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
						return i;
					}
				}
			}
		}
	}
	//
	return -1;
}

int Scheduler_PM_Get_First_for_CYCLON( int tms , BOOL All ) {
	int i;
	for ( i = PM1 ; i < AL ; i++ ) {
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
			//
			if ( All ) { // 2011.08.17
				if ( _SCH_PRM_GET_MODULE_MODE( i ) ) {
					return i;
				}
			}
			else {
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
					return i;
				}
			}
			//
		}
	}
	return -1;
}

BOOL SCHEDULER_CONTROL_Chk_BM_TM_PM_HAS_WAFER_CYCLON_STYLE_6( int bmc , BOOL all ) {
	int tms , ch , i;
	//
	if ( bmc == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return FALSE; // 2010.05.21
	//
	tms = _SCH_PRM_GET_MODULE_GROUP( bmc );
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2013.02.15
		//
		ch = Scheduler_BM_Get_First_for_CYCLON( tms );
		//
		if ( ch != -1 ) {
			//
			if ( ch == bmc ) return FALSE; // 2010.05.21
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( ch , -1 ) > 0 ) return TRUE;
		}
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bmc , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
	}
	else { // 2013.02.15
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bmc , -1 ) > 0 ) return FALSE;
	}
	//
//	ch = Scheduler_PM_Get_First_for_CYCLON( tms , FALSE ); // 2013.02.15
	ch = Scheduler_PM_Get_First_for_CYCLON( tms , all ); // 2013.02.15
	//
	if ( ch != -1 ) {
		for ( i = 0 ; i < SIZE_OF_CYCLON_PM_DEPTH(ch) ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_STATUS( ch , i ) == 100 ) continue; // temp // 2013.06.13
				return TRUE;
			}
		}
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return TRUE;
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return TRUE;
	//
	return FALSE;
}

/*
// 2013.11.26
int SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( int ch , int *pointer , int *s1 , int *realside ) {
	int i , k = 0 , no = -1;
	*realside = 0; // 2004.12.06
	*s1 = 0;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
			//
//			if ( ( *pointer < 0 ) || ( _SCH_MODULE_Get_BM_POINTER( ch , i ) == *pointer ) ) { // 2013.01.25
				if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) != PATHDO_WAFER_RETURN ) { // 2010.09.09
					no = _SCH_MODULE_Get_BM_POINTER( ch , i );
					*realside = _SCH_MODULE_Get_BM_SIDE( ch , i );
					*s1 = i;
					k++;
					break;
				}
//			} // 2013.01.25
		}
	}
	if ( k > 0 ) *pointer = no;
	return k;
}
*/

// 2013.11.26
int SCHEDULING_CHECK_BM_HAS_IN_SIDE_POINTER_for_CYCLON( int ch , int *pointer , int *s1 , int *realside ) {
	int i , k = 0 , no = -1;
	*realside = 0;
	*s1 = 0;
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) ) {
			//
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) != PATHDO_WAFER_RETURN ) {
				//
				if ( k == 0 ) {
					no = _SCH_MODULE_Get_BM_POINTER( ch , i );
					*realside = _SCH_MODULE_Get_BM_SIDE( ch , i );
					*s1 = i;
				}
				else {
					if ( _SCH_CLUSTER_Get_SupplyID( *realside , no ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
						no = _SCH_MODULE_Get_BM_POINTER( ch , i );
						*realside = _SCH_MODULE_Get_BM_SIDE( ch , i );
						*s1 = i;
					}
				}
				//
				k++;
				//
			}
		}
	}
	if ( k > 0 ) *pointer = no;
	return k;
}

int SCHEDULING_CHECK_BM_HAS_OUT_SUPPLYID( int ch1 , int ch2 ) { // 2014.10.03
	int i , ss , es;
	int sup1 , sup2;
	//
	sup1 = -1;
	sup2 = -1;
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , ch1 , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch1 , i ) <= 0 ) continue;
		//
		if ( ( _SCH_MODULE_Get_BM_STATUS( ch1 , i ) != BUFFER_OUTWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( ch1 , i ) ) continue;
		//
		sup1 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch1 , i ) , _SCH_MODULE_Get_BM_POINTER( ch1 , i ) );
		//
		break;
	}
	//
	if ( sup1 < 0 ) return FALSE;
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , ch2 , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch2 , i ) <= 0 ) continue;
		//
		if ( ( _SCH_MODULE_Get_BM_STATUS( ch2 , i ) != BUFFER_OUTWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( ch2 , i ) ) continue;
		//
		sup2 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch2 , i ) , _SCH_MODULE_Get_BM_POINTER( ch2 , i ) );
		//
		break;
	}
	//
	if ( sup2 < 0 ) return FALSE;
	//
	return ( sup1 < sup2 );
}


BOOL SCHEDULER_CONTROL_Chk_BM_FMWAIT_STYLE_6( int tms , int ch ); // 2018.10.02


int SCHEDULING_CHECK_BM_HAS_OUT_FREE_FIND_for_CYCLON( int tms , int ch , int *s1 , BOOL fmsidecheck ) {
	int i , k = 0;
	*s1 = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
			*s1 = i;
			k++;
		}
	}
	//
	if ( k <= 0 ) return k;
	//
	if ( fmsidecheck ) {
//		if ( SCHEDULER_CONTROL_Chk_BM_FMWAIT_STYLE_6( tms , ch ) ) return 0; // 2018.10.02 // 2018.10.31
	}
	//
	return k;
}

//=============================================================================================================

int SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_6( int TMATM , int side , int *FM_Slot , int *FM_Slot2 , int *FM_Pointer , int *FM_Pointer2 , int *FM_Mode , int *next_FM_sDoPointer , int *next_FM_DoPointer , int *next_BM , BOOL BOnly , BOOL *forcediv , int *HasInvalid ) {
	int i , j , dt , fp , fp2 , fs , fs2 , single , nbm;
	int chktm[MAX_TM_CHAMBER_DEPTH]; // 2004.02.26
	int hasinv;
	int pickcm; // 2014.09.16

	*forcediv = FALSE; // 2013.04.01
	*HasInvalid = -1; // 2013.05.27
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) { // 2008.11.12
//		if ( !_SCH_SUB_Check_Complete_Return( side ) ) return -3; // 2014.05.26
		if ( !_SCH_SUB_Check_Complete_Return( side ) ) return -11; // 2014.05.26
	}
//	if ( !_SCH_SUB_Remain_for_FM( side ) ) return -3; // 2014.05.26
	if ( !_SCH_SUB_Remain_for_FM( side ) ) return -12; // 2014.05.26
	//
	if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , _SCH_MODULE_Get_FM_DoPointer( side ) , 0 ) == 0 ) return -21;
	//
	_SCH_MODULE_Set_FM_HWait( side , TRUE );
	//
	// FM_Mode  => 0:AB 1:A(1) 2:B(1)
	//
	fp = _SCH_MODULE_Get_FM_DoPointer( side );
	fs = _SCH_CLUSTER_Get_SlotIn( side , fp );
	//
	pickcm = _SCH_CLUSTER_Get_PathIn( side , fp ); // 2014.09.16
	//
	//==================================================================================================
	// 2013.10.17
	if ( _SCH_SUB_FM_Current_No_Way_Supply( side , fp , -1 ) ) {
		_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1 );
//		return -4; // 2014.05.26
		return -13; // 2014.05.26
	}
	//==================================================================================================
	if ( BOnly ) { // 2014.09.16
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( pickcm ) ) {
			case 1 :
				return -14;
				break;
			}
		}
	}
	//==================================================================================================
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// 2004.02.26
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	j = 0;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		chktm[i] = FALSE;
	}
	if ( _SCH_CLUSTER_Get_PathRange( side , fp ) >= 1 ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			//
			dt = _SCH_CLUSTER_Get_PathProcessChamber( side , fp , 0 , i );
			//
			if ( dt > 0 ) {
				//
//				if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( _SCH_PRM_GET_MODULE_GROUP( dt ) ) > 0 ) continue; // 2013.10.30 // 2014.06.11
				//
				if ( SCH_BM_SUPPLY_LOCK_GET2_FOR_AL6( _SCH_PRM_GET_MODULE_GROUP( dt ) , side , fp ) ) continue; // 2014.06.11
				//
				if ( SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( dt ) , side , fp , TRUE , HasInvalid ) ) { // 2013.01.25
					if ( !SCH_CYCLON2_GROUP_UNMATCHED_COUNT_RUN_STATUS( _SCH_PRM_GET_MODULE_GROUP( dt ) ) ) { // 2013.05.28
						chktm[ _SCH_PRM_GET_MODULE_GROUP( dt ) ] = TRUE;
						j++;
					}
					else {
						*HasInvalid = _SCH_PRM_GET_MODULE_GROUP( dt );
					}
				}
			}
		}
	}
	//
	if ( j == 0 ) return -99;
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		if ( _SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
			if ( !chktm[ i ] ) break;
		}
	}
	//
	if ( !BOnly ) { // 2007.07.29
		*next_BM = 0; // All
		if ( i != MAX_TM_CHAMBER_DEPTH ) {
			j = 1;
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				if ( chktm[ i ] ) {
					*next_BM = *next_BM + j;
				}
				j = j * 10;
			}
		}
	}
	else { // 2007.07.29
		nbm = 0;
		if ( i != MAX_TM_CHAMBER_DEPTH ) {
			j = 1;
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				if ( chktm[ i ] ) {
					nbm = nbm + j;
				}
				j = j * 10;
			}
			if ( *next_BM == 0 ) {
				*next_BM = nbm;
			}
			else if ( *next_BM != nbm ) {
				return -98;
			}
		}
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//
	single = FALSE;
	//
	*next_FM_DoPointer = fp + 1;
	//
	if ( *next_FM_DoPointer >= MAX_CASSETTE_SLOT_SIZE ) {
		single = TRUE;
	}
	else {
		while ( _SCH_SUB_Run_Impossible_Condition( side , *next_FM_DoPointer , -1 ) ) {
			(*next_FM_DoPointer)++;
			if ( *next_FM_DoPointer >= MAX_CASSETTE_SLOT_SIZE ) break;
		}
		if ( *next_FM_DoPointer >= MAX_CASSETTE_SLOT_SIZE ) {
			single = TRUE;
		}
	}
	//===============================================================================================
	// 2007.07.29
	//===============================================================================================
	if ( BOnly ) { // 2007.07.29
		*FM_Pointer = fp;
		*FM_Slot    = fs;
		return 1;
	}
	//===============================================================================================
	*next_FM_sDoPointer = *next_FM_DoPointer;
	//
	//===============================================================================================
	// 2007.05.15
	//===============================================================================================
	if ( !single ) {
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
			if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side , fp , side , *next_FM_DoPointer ) ) { // 2007.07.29
				single = TRUE;
			}
		}
	}
	//===============================================================================================
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		if ( single ) {
			/*
			// 2014.09.16
			//
			*FM_Mode	 = 1;
			*FM_Pointer  = fp;
			*FM_Slot     = fs;
			*FM_Pointer2 = 0;
			*FM_Slot2    = 0;
			return 1;
			//
			*/
			//==================================================================================================
			switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( pickcm ) ) { // 2014.09.16
			case 2 :
				//
				*FM_Mode	 = 2;
				*FM_Pointer2 = fp;
				*FM_Slot2    = fs;
				*FM_Pointer  = 0;
				*FM_Slot     = 0;
				//
				return 1;
				//
				break;
			default :
				//
				*FM_Mode	 = 1;
				*FM_Pointer  = fp;
				*FM_Slot     = fs;
				*FM_Pointer2 = 0;
				*FM_Slot2    = 0;
				//
				return 1;
				//
				break;
			}
			//==================================================================================================
		}
		else { // 2014.09.16
			//==================================================================================================
			switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( pickcm ) ) {
			case 2 :
				//
				*FM_Mode	 = 2;
				*FM_Pointer2 = fp;
				*FM_Slot2    = fs;
				*FM_Pointer  = 0;
				*FM_Slot     = 0;
				//
				return 1;
				//
				break;
			case 1 :
				//
				*FM_Mode	 = 1;
				*FM_Pointer  = fp;
				*FM_Slot     = fs;
				*FM_Pointer2 = 0;
				*FM_Slot2    = 0;
				//
				return 1;
				//
				break;
			}
			//==================================================================================================
		}
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		*FM_Mode	 = 1;
		*FM_Pointer  = fp;
		*FM_Slot     = fs;
		*FM_Pointer2 = 0;
		*FM_Slot2    = 0;
		return 1;
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		*FM_Mode	 = 2;
		*FM_Pointer2 = fp;
		*FM_Slot2    = fs;
		*FM_Pointer  = 0;
		*FM_Slot     = 0;
		return 1;
	}

	fp2 = *next_FM_DoPointer;
	fs2 = _SCH_CLUSTER_Get_SlotIn( side , fp2 );
	//
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// 2004.02.26
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	j = 0;
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		chktm[i] = FALSE;
	}
	if ( _SCH_CLUSTER_Get_PathRange( side , fp2 ) >= 1 ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			dt = _SCH_CLUSTER_Get_PathProcessChamber( side , fp2 , 0 , i );
			if ( dt > 0 ) {
				if ( SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( dt ) , side , fp2 , TRUE , &hasinv ) ) { // 2013.01.25
					chktm[ _SCH_PRM_GET_MODULE_GROUP( dt ) ] = TRUE;
					j++;
				}
			}
		}
	}
	//
	if ( j == 0 ) {
		//
		*forcediv = TRUE; // 2013.04.01
		//
		single = TRUE;
	}
	else {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			if ( _SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
				if ( !chktm[ i ] ) break;
			}
		}
		nbm = 0; // All
		if ( i != MAX_TM_CHAMBER_DEPTH ) {
			j = 1;
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				if ( chktm[ i ] ) {
					nbm = nbm + j;
				}
				j = j * 10;
			}
		}
		if ( nbm == 0 ) {
			if ( *next_BM != 0 ) single = TRUE;
		}
		else {
			if ( *next_BM == 0 ) {
				*next_BM = nbm;
			}
			else {
				if ( *next_BM != nbm ) single = TRUE;
			}
		}
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	if ( single ) {
		*FM_Mode	 = 1;
		*FM_Pointer  = fp;
		*FM_Slot     = fs;
		*FM_Pointer2 = 0;
		*FM_Slot2    = 0;
		return 1;
	}
	//
	*FM_Mode	 = 0;
	*FM_Pointer  = fp;
	*FM_Slot     = fs;
	*FM_Pointer2 = fp2;
	*FM_Slot2    = fs2;
	( *next_FM_DoPointer ) ++;
	//======================================================================================
	// 2003.06.05
	//======================================================================================
	if ( *next_FM_DoPointer < MAX_CASSETTE_SLOT_SIZE ) {
		while ( _SCH_SUB_Run_Impossible_Condition( side , *next_FM_DoPointer , -1 ) ) {
			(*next_FM_DoPointer)++;
			if ( *next_FM_DoPointer >= MAX_CASSETTE_SLOT_SIZE ) break;
		}
	}
	//======================================================================================
	return 1;
}
//========
int SCHEDULING_CHECK_Enable_PICK_from_FM_ArmBOnly_for_STYLE_6( int TMATM , int side , int *next_BM , int *FM_Slot , int *FM_Pointer , int *next_FM_DoPointer , int *hasinv ) {
	int FM_Slot2 , FM_Pointer2 , FM_Mode , next_FM_sDoPointer , forcediv;
	return SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_6( TMATM , side , FM_Slot , &FM_Slot2 , FM_Pointer , &FM_Pointer2 , &FM_Mode , &next_FM_sDoPointer , next_FM_DoPointer , next_BM , TRUE , &forcediv , hasinv );
}
//=======================================================================================
int SCHEDULING_CHECK_Enable_PICK_from_FM_Properly_Group_for_STYLE_6( int TMATM , int side , int pt , int bm , int clichk ) { // 2005.09.14
	int i , fp , fs , dt , m , s , npsl , group;

	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) { // 2008.11.12
		if ( !_SCH_SUB_Check_Complete_Return( side ) ) return FALSE;
	}
	//
	if ( !_SCH_SUB_Remain_for_FM( side ) ) return FALSE;
	//
	/*
	// 2014.07.08
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.02.15
		//
		group = _SCH_PRM_GET_MODULE_GROUP( bm );
		//
	}
	else {
		group = bm - BM1;
	}
	*/
	//
	// 2014.07.08
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 1 ) { // 2013.02.15
		//
		group = _SCH_PRM_GET_MODULE_GROUP( bm );
		//
	}
	else {
		group = bm - BM1;
	}
	//
	if ( pt == -1 ) { // 2011.05.04
		fp = _SCH_MODULE_Get_FM_DoPointer( side );
	}
	else { // 2011.05.04
		fp = pt;
	}
	//
	fs = _SCH_CLUSTER_Get_SlotIn( side , fp );
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
		npsl = SCHEDULER_CONTROL_ST6_GET_SLOT( Scheduler_PM_Get_First_for_CYCLON( group , FALSE ) , side , fp );
	}
	else {
		npsl = 0;
	}
	//
	if ( clichk ) { // 2007.07.29
//		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bm , &m , &s ); // 2014.12.24
		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bm , &m , &s ); // 2014.12.24
		for ( i = m ; i <= s ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( bm , i ) == BUFFER_INWAIT_STATUS ) {
					if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bm , i ) ) { // 2010.09.09
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( bm , i ) , _SCH_MODULE_Get_BM_POINTER( bm , i ) , side , fp ) ) return FALSE;
						if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
							dt = SCHEDULER_CONTROL_ST6_GET_SLOT( Scheduler_PM_Get_First_for_CYCLON( group , FALSE ) , _SCH_MODULE_Get_BM_SIDE( bm , i ) , _SCH_MODULE_Get_BM_POINTER( bm , i ) );
							if ( ( dt > 0 ) && ( dt == npsl ) ) return FALSE;
						}
					}
				}
			}
		}
	}
	//
	if ( _SCH_CLUSTER_Get_PathRange( side , fp ) >= 1 ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			dt = _SCH_CLUSTER_Get_PathProcessChamber( side , fp , 0 , i );
			if ( dt > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( dt , FALSE , side ) ) {
					if ( _SCH_PRM_GET_MODULE_GROUP( dt ) == group ) return TRUE;
				}
			}
		}
	}
	return FALSE;
}




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() { // 2007.04.30
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() >= 5 ) {
		//
//		if ( _SCH_PRM_GET_MODULE_SIZE( BM1 ) != SIZE_OF_CYCLON_PM_DEPTH(PM1) ) return 2; // 2013.01.19 // 2014.12.24
		if ( _SCH_PRM_GET_MODULE_SIZE( BM1 ) < SIZE_OF_CYCLON_PM_DEPTH(PM1) ) return 2; // 2013.01.19 // 2014.12.24
		//
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_TM_PICKPLACE_DENY_FOR_6( int tms ) { // 2006.06.28
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() >= 1 ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( tms + BM1 ) != BUFFER_TM_STATION ) return TRUE;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( tms + BM1 ) > 0 ) return TRUE;
	}
	return FALSE;
}
//
BOOL SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( int tms , BOOL zeroreturn ) { // 2006.08.30
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 0 ) return zeroreturn;
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 1 ) return TRUE;
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 2 ) return TRUE;
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return FALSE;
	if ( !zeroreturn ) {
		if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 3 ) return FALSE;
		if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 4 ) return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( int bmc ) { // 2005.04.20
	if ( ( _SCH_PRM_GET_MODULE_SIZE( bmc ) % 2 ) == 0 ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_NEED_SEPARATE_PLACE_FOR_6( int bmc ) { // 2005.05.17
//(TM)(FM)
//Pick & Place (TM) Pick & Place (FM)

// TM PICK F and FM PLACE L
// TM PICK L and FM PLACE F

// 0 (TM=F/F)(FM=F/F)
// 1 (TM=F/L)(FM=F/F)
// 2 (TM=L/F)(FM=F/F)
// 3 (TM=L/L)(FM=F/F)
// 4 (TM=F/F)(FM=F/L)
// 5 (TM=F/L)(FM=F/L)
// 6 (TM=L/F)(FM=F/L)
// 7 (TM=L/L)(FM=F/L)
// 8 (TM=F/F)(FM=L/F)
// 9 (TM=F/L)(FM=L/F)
//10 (TM=L/F)(FM=L/F)
//11 (TM=L/L)(FM=L/F)
//12 (TM=F/F)(FM=L/L)
//13 (TM=F/L)(FM=L/L)
//14 (TM=L/F)(FM=L/L)
//15 (TM=L/L)(FM=L/L)

//  F/? , ?/L
//  L/? , ?/F
// 2,3,4,5,10,11,12,13
// 2 (TM=L/F)(FM=F/F)
// 3 (TM=L/L)(FM=F/F)
// 4 (TM=F/F)(FM=F/L)
// 5 (TM=F/L)(FM=F/L)
//10 (TM=L/F)(FM=L/F)
//11 (TM=L/L)(FM=L/F)
//12 (TM=F/F)(FM=L/L)
//13 (TM=F/L)(FM=L/L)
	switch ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmc - BM1 + PM1 ) ) {
	case 2 :
	case 3 :
	case 4 :
	case 5 :
	case 10 :
	case 11 :
	case 12 :
	case 13 :	return TRUE; break;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( int mode , int bmc , int *ss , int *es ) { // 2005.01.24
	//=========================================================================================
	// 2007.04.30
	//=========================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
		*ss = 1;
		*es = _SCH_PRM_GET_MODULE_SIZE( bmc );
		return;
	}
	//=========================================================================================
	if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 0 ) {
		switch ( mode ) {
		case CHECKORDER_FOR_FM_PICK : // 1,2
		case CHECKORDER_FOR_TM_PLACE :
			*ss = 1;
			*es = _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2;
			break;
		default : // 3,4
			*ss = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 ) + 1;
			*es = _SCH_PRM_GET_MODULE_SIZE( bmc );
			break;
		}
	}
	else {
		switch ( mode ) {
		case CHECKORDER_FOR_FM_PICK : // 3,4
		case CHECKORDER_FOR_TM_PLACE :
			*ss = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 ) + 1;
			*es = _SCH_PRM_GET_MODULE_SIZE( bmc );
			break;
		default : // 1,2
			*ss = 1;
			*es = _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2;
			break;
		}
	}
}
//------------------------------------------------------------------------------------------
void SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( int mode , int bmc , int *ss , int *es ) { // 2014.12.24
	int pmc , s;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
		//
		pmc = Scheduler_PM_Get_First_for_CYCLON( _SCH_PRM_GET_MODULE_GROUP(bmc) , TRUE );
		//
		if ( pmc <= 0 ) {
			*ss = 1;
			*es = _SCH_PRM_GET_MODULE_SIZE( bmc );
		}
		else {
			//
			s = SIZE_OF_CYCLON_PM_DEPTH( pmc );
			//
			if ( (s*2) < _SCH_PRM_GET_MODULE_SIZE( bmc ) ) {
				*ss = 1;
				*es = s*2;
			}
			else if ( s < _SCH_PRM_GET_MODULE_SIZE( bmc ) ) {
				*ss = 1;
				*es = s;
			}
			else {
				*ss = 1;
				*es = _SCH_PRM_GET_MODULE_SIZE( bmc );
			}
		}
		//
		return;
	}
	//=========================================================================================
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( mode , bmc , ss , es );
	//=========================================================================================
}
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( int mode , int bmc ) { // 2004.11.25
	//=========================================================================================
	// 2007.04.30
	//=========================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) return 0; // 2007.04.30
	//=========================================================================================
	if      ( mode == CHECKORDER_FOR_TM_PICK ) {
		return ( ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmc - BM1 + PM1 ) / 2 ) % 2 );
	}
	else if ( mode == CHECKORDER_FOR_TM_PLACE ) {
		return ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmc - BM1 + PM1 ) % 2 );
	}
	else if ( mode == CHECKORDER_FOR_FM_PICK ) {
		return ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmc - BM1 + PM1 ) / 8 );
	}
	else if ( mode == CHECKORDER_FOR_FM_PLACE ) {
		return ( ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmc - BM1 + PM1 ) / 4 ) % 2 );
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( int bmc , int pmc ) {
	if ( bmc <  BM1 ) return FALSE;
	if ( bmc >= TM  ) return FALSE;
	if ( pmc <  PM1 ) return FALSE;
	if ( pmc >= AL  ) return FALSE;
	//
//	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2014.07.10
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 1 ) { // 2014.07.10
		if ( _SCH_PRM_GET_MODULE_GROUP( pmc ) == ( bmc - BM1 ) ) return TRUE;
	}
	else { // 2013.01.28
		if ( _SCH_PRM_GET_MODULE_GROUP( pmc ) == _SCH_PRM_GET_MODULE_GROUP( bmc ) ) return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//int SCHEDULER_CONTROL_Chk_FM_BMOUT_DIRECT_RETURN_FOR_6() { // 2006.11.16 // 2014.11.13
int SCHEDULER_CONTROL_Chk_FM_BMOUT_DIRECT_RETURN_FOR_6( int side ) { // 2006.11.16 // 2014.11.13
	int b , i , ss , es;

	for ( b = BM1 ; b < TM ; b++ ) {
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( b ) ) continue; // 2012.07.13
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber6( b , 0 ) ) continue; // 2014.11.13
		//
//		if ( !_SCH_PRM_GET_MODULE_MODE( b - BM1 + TM ) ) continue; // 2014.11.13
		if ( !_SCH_PRM_GET_MODULE_MODE( _SCH_PRM_GET_MODULE_GROUP( b ) + TM ) ) continue; // 2014.11.13
		//
		if ( ( _SCH_MODULE_Get_BM_FULL_MODE( b ) != BUFFER_WAIT_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( b ) != BUFFER_FM_STATION ) ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( b ) > 0 ) continue;
		//
		for ( i = PM1 ; i < AL ; i++ ) {
			if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( b , i ) ) {
				if ( _SCH_MODULE_Get_Mdl_Run_Flag( i ) > 0 ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) break;
				}
			}
		}
		if ( i != AL ) continue;
		//
		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , b , &ss , &es );
		for ( i = ss ; i <= es ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( b , i ) > 0 ) return TRUE;
		}
	}
	return FALSE;
}

int SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_CYCLON_FOR_6( int side , int bmch , int *slotstart , int *slotend , int *c , int *s ) {
	int m , i , ss , es;

	*slotend = 0;

	m = 0;
	*c = 0;

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , bmch , &ss , &es );

	if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , bmch ) == 0 ) {
		for ( i = ss ; i <= es ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) { // 2010.09.09
					(*c)++;
					if ( m == 0 ) {
						*s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						*slotstart = i;
						if ( ( i + 1 ) <= es ) {
							if ( _SCH_MODULE_Get_BM_WAFER( bmch , i + 1 ) > 0 ) {
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i + 1 ) ) { // 2010.09.09
									*slotend = i + 1;
								}
							}
						}
						m = 1;
					}
				}
			}
		}
	}
	else {
		for ( i = es ; i >= ss ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) { // 2010.09.09
					(*c)++;
					if ( m == 0 ) {
						*s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						*slotstart = i;
						if ( ( i - 1 ) >= ss ) {
							if ( _SCH_MODULE_Get_BM_WAFER( bmch , i - 1 ) > 0 ) {
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i - 1 ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i - 1 ) ) { // 2010.09.09
									*slotend = i - 1;
								}
							}
						}
						m = 1;
					}
				}
			}
		}
	}
	return m;
}


int SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( int side , int bmch , int *slotstart , int *slotend ) {
	int P , k1 = FALSE , k2 = FALSE , s1 = FALSE , s2 = FALSE , ts1 , ts2 , po1 , po2 , fs , fs2;
	int i , ss , es; // 2004.02.24 // 2005.01.24
	int rcm , rslot , rcm2 , rslot2; // 2014.09.16

	*slotend = 0; // 2004.11.25

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , bmch , &ss , &es ); // 2005.01.24

	if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , bmch ) == 0 ) { // 2004.11.25
		for ( i = ss ; i <= es ; i++ ) { // 2005.01.24
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) { // 2010.09.09
					fs = i;
					k1 = TRUE;
					P = _SCH_MODULE_Get_BM_POINTER( bmch , i );
					ts1 = _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , P );
					po1 = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , P );
					if ( side == -1 ) { // 2007.07.03
						s1 = TRUE;
					}
					else {
						if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
							s1 = TRUE;
						}
					}
					ts2 = -1;
					po2 = -1;
					if ( ( i + 1 ) <= es ) { // 2005.01.24
						if ( _SCH_MODULE_Get_BM_WAFER( bmch , i + 1 ) > 0 ) {
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i + 1 ) ) { // 2010.09.09
								fs2 = i + 1; // 2004.11.25
								k2 = TRUE;
								P = _SCH_MODULE_Get_BM_POINTER( bmch , i + 1 );
								ts2 = _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( bmch , i + 1 ) , P );
								po2 = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( bmch , i + 1 ) , P );
								if ( side == -1 ) { // 2007.07.03
									s2 = TRUE;
								}
								else {
									if ( _SCH_MODULE_Get_BM_SIDE( bmch , i + 1 ) == side ) {
										s2 = TRUE;
									}
								}
							}
						}
					}
					break;
				}
			}
		}
	}
	else { // 2004.11.25
		for ( i = es ; i >= ss ; i-- ) { // 2005.01.24
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) { // 2010.09.09
					fs = i;
					k1 = TRUE;
					P = _SCH_MODULE_Get_BM_POINTER( bmch , i );
					ts1 = _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , P );
					po1 = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , P );
					if ( side == -1 ) { // 2007.07.03
						s1 = TRUE;
					}
					else {
						if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
							s1 = TRUE;
						}
					}
					ts2 = -1;
					po2 = -1;
					if ( ( i - 1 ) >= ss ) { // 2005.01.24
						if ( _SCH_MODULE_Get_BM_WAFER( bmch , i - 1 ) > 0 ) {
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i - 1 ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i - 1 ) ) { // 2010.09.09
								fs2 = i - 1; // 2004.11.25
								k2 = TRUE;
								P = _SCH_MODULE_Get_BM_POINTER( bmch , i - 1 );
								ts2 = _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( bmch , i - 1 ) , P );
								po2 = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( bmch , i - 1 ) , P );
								if ( side == -1 ) { // 2007.07.03
									s2 = TRUE;
								}
								else {
									if ( _SCH_MODULE_Get_BM_SIDE( bmch , i - 1 ) == side ) {
										s2 = TRUE;
									}
								}
							}
						}
					}
					break;
				}
			}
		}
	}
	//----------------------
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		/*
		//
		// 2014.09.16
		//
		//
		if ( k1 && k2 ) {
			if      (  s1 &&  s2 ) {
				*slotstart = fs;
				*slotend = fs2; // 2004.11.25
				return 3;
			}
			else if ( !s1 && !s2 ) {
				return -99;
			}
			else if (  s1 && !s2 ) {
				*slotstart = fs;
				*slotend = fs2; // 2004.11.25
				return 3;
			}
			else if ( !s1 &&  s2 ) {
				*slotstart = fs;
				*slotend = fs2; // 2004.11.25
				return 3;
			}
		}
		else if ( k1 ) {
			if ( s1 ) {
				*slotstart = fs;
				return 1;
			}
			return -99;
		}
		//
		*/
		//
		switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( bmch ) ) { // 2014.09.16
		case 1 :
			if ( k1 ) {
				if ( s1 ) {
					//
					_SCH_SUB_GET_OUT_CM_AND_SLOT( _SCH_MODULE_Get_BM_SIDE( bmch , fs ) , _SCH_MODULE_Get_BM_POINTER( bmch , fs ) , -1 , &rcm , &rslot );
					//
					if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm ) == 2 ) return 0;
					//
					*slotstart = fs;
					return 1;
				}
				return -99;
			}
			break;
		case 2 :
			if ( k1 ) {
				if ( s1 ) {
					//
					_SCH_SUB_GET_OUT_CM_AND_SLOT( _SCH_MODULE_Get_BM_SIDE( bmch , fs ) , _SCH_MODULE_Get_BM_POINTER( bmch , fs ) , -1 , &rcm , &rslot );
					//
					if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm ) == 1 ) return 0;
					//
					*slotstart = fs;
					return 2;
				}
				return -99;
			}
			break;
		default :
			if ( k1 && k2 ) {
				//
				_SCH_SUB_GET_OUT_CM_AND_SLOT( _SCH_MODULE_Get_BM_SIDE( bmch , fs ) , _SCH_MODULE_Get_BM_POINTER( bmch , fs ) , -1 , &rcm , &rslot );
				_SCH_SUB_GET_OUT_CM_AND_SLOT( _SCH_MODULE_Get_BM_SIDE( bmch , fs2 ) , _SCH_MODULE_Get_BM_POINTER( bmch , fs2 ) , -1 , &rcm2 , &rslot2 );
				//
				if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm ) == _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm2 ) ) {
					//
					switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm ) ) {
					case 1 :
						if ( k1 ) {
							if ( s1 ) {
								*slotstart = fs;
								return 1;
							}
							return -99;
						}
						break;
					case 2 :
						if ( k1 ) {
							if ( s1 ) {
								*slotstart = fs;
								return 2;
							}
							return -99;
						}
						break;
					default :
						//
						if      (  s1 &&  s2 ) {
							*slotstart = fs;
							*slotend = fs2; // 2004.11.25
							return 3;
						}
						else if ( !s1 && !s2 ) {
							return -99;
						}
						else if (  s1 && !s2 ) {
							*slotstart = fs;
							*slotend = fs2; // 2004.11.25
							return 3;
						}
						else if ( !s1 &&  s2 ) {
							*slotstart = fs;
							*slotend = fs2; // 2004.11.25
							return 3;
						}
						break;
					}
				}
				else {
					//
					switch ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm ) ) {
					case 2 :
						if ( k1 ) {
							if ( s1 ) {
								*slotstart = fs;
								return 2;
							}
							return -99;
						}
						break;
					default :
						if ( k1 ) {
							if ( s1 ) {
								*slotstart = fs;
								return 1;
							}
							return -99;
						}
						break;
					}
					//
				}
			}
			else if ( k1 ) {
				if ( s1 ) {
					//
					_SCH_SUB_GET_OUT_CM_AND_SLOT( _SCH_MODULE_Get_BM_SIDE( bmch , fs ) , _SCH_MODULE_Get_BM_POINTER( bmch , fs ) , -1 , &rcm , &rslot );
					//
					switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( rcm ) ) {
					case 2 :
						*slotstart = fs;
						return 2;
					default :
						*slotstart = fs;
						return 1;
					}
					//
				}
				return -99;
			}
			break;
		}
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		if ( k1 && k2 ) {
			if      (  s1 &&  s2 ) {
				*slotstart = fs;
				return 1;
			}
			else if ( !s1 && !s2 ) {
				return -99;
			}
			else if (  s1 && !s2 ) {
				*slotstart = fs;
				return 1;
			}
			else if ( !s1 &&  s2 ) {
//				*slotstart = fs + 1; // 2004.11.25
				*slotstart = fs2; // 2004.11.25
				return 1;
			}
		}
		else if ( k1 ) {
			if ( s1 ) {
				*slotstart = fs;
				return 1;
			}
			return -99;
		}
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		if ( k1 && k2 ) {
			if      (  s1 &&  s2 ) {
				*slotstart = fs;
				return 2;
			}
			else if ( !s1 && !s2 ) {
				return -99;
			}
			else if (  s1 && !s2 ) {
				*slotstart = fs;
				return 2;
			}
			else if ( !s1 &&  s2 ) {
//				*slotstart = fs + 1; // 2004.11.25
				*slotstart = fs2; // 2004.11.25
				return 2;
			}
		}
		else if ( k1 ) {
			if ( s1 ) {
				*slotstart = fs;
				return 2;
			}
			return -99;
		}
	}
	return 0;
}


BOOL SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( int bmch , int side , int pt , int offset ) { // 2004.02.10
	int k , m;
	if ( _SCH_CLUSTER_Get_PathRange( side , pt ) >= 0 ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) - offset , k ); // 2005.02.23
			if ( m > 0 ) { // 2008.01.21
				if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) { // 2004.02.25
					if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_Will_Do_PM_POST_FOR_6( int side , int tms , int bmch , int slot ) { // 2003.10.08
	int pt , od;
	int S , P , m , k , l;
	int p[MAX_PM_CHAMBER_DEPTH] , t[MAX_PM_CHAMBER_DEPTH] , b[MAX_PM_CHAMBER_DEPTH] , tc;
	//
	if ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() % 2 ) return FALSE; // 2004.01.06
	//
	//==================================================================================
	//==================================================================================
	// 2004.02.10
	//==================================================================================
	//==================================================================================
	if ( side != _SCH_MODULE_Get_BM_SIDE( bmch , slot ) ) return TRUE;
	//
	if ( _SCH_CLUSTER_Get_PathDo( side , _SCH_MODULE_Get_BM_POINTER( bmch , slot ) ) == PATHDO_WAFER_RETURN ) return FALSE; // 2008.01.21
	//
	if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bmch , _SCH_MODULE_Get_BM_SIDE( bmch , slot ) , _SCH_MODULE_Get_BM_POINTER( bmch , slot ) , 0 ) ) return FALSE;
	//==================================================================================
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		p[m] = FALSE;
		t[m] = FALSE;
		b[m] = FALSE;
	}
	//
	tc = FALSE;
	//----------
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m + PM1 ) ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m + PM1 , FALSE , side ) ) {
				if ( _SCH_MODULE_Get_PM_WAFER( m + PM1 , 0 ) > 0 ) {
					pt = _SCH_MODULE_Get_PM_POINTER( m + PM1 , 0 );
					od = _SCH_MODULE_Get_PM_PATHORDER( m + PM1 , 0 );
//					if ( !_SCH_CLUSTER_Check_HasProcessData_Post( side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) - 1 , od ) ) { // 2006.01.06
					if ( !_SCH_CLUSTER_Check_HasProcessData_Post( side , pt , _SCH_CLUSTER_Get_PathDo( side , pt ) - 1 , od ) && !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , m + PM1 ) ) { // 2006.01.06
						p[m] = TRUE;
					}
				}
				else {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m + PM1 ) <= 0 ) {
						p[m] = TRUE;
					}
				}
			}
		}
	}
	//
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		if ( p[m] ) break;
	}
	if ( m == MAX_PM_CHAMBER_DEPTH ) return TRUE;
	//----------
	for ( l = TA_STATION ; l <= TB_STATION ; l++ ) { 
		if ( _SCH_MODULE_Get_TM_WAFER( tms , l ) > 0 ) {
			S = _SCH_MODULE_Get_TM_SIDE( tms , l );
			P = _SCH_MODULE_Get_TM_POINTER( tms , l );
			if ( _SCH_CLUSTER_Get_PathDo( S , P ) <= _SCH_CLUSTER_Get_PathRange( S , P ) ) { // 2005.02.23
				tc = TRUE;
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) - 1 , k ); // 2005.02.23
					if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
						if ( p[m - PM1] ) t[m - PM1] = TRUE;
					}
				}
			}
		}
	}
	if ( tc ) {
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			if ( t[m] ) break;
		}
		if ( m == MAX_PM_CHAMBER_DEPTH ) return TRUE;
	}
	//----------
	P = _SCH_MODULE_Get_BM_POINTER( bmch , slot );
	if ( side != _SCH_MODULE_Get_BM_SIDE( bmch , slot ) ) return TRUE;
	if ( _SCH_CLUSTER_Get_PathRange( side , P ) >= 0 ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( side , P , 0 , k );
			if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
				if ( p[m - PM1] ) b[m - PM1] = TRUE;
			}
		}
	}
	//
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		if ( b[m] ) break;
	}
	if ( m == MAX_PM_CHAMBER_DEPTH ) return TRUE;
	//----------
	if ( tc ) {
		l = 0;
		k = 0;
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m + PM1 ) ) {
				if ( t[m] ) {
					l++;
				}
				else {
					k++;
				}
			}
		}
		//---
		if ( l == 0 ) {
			return TRUE;
		}
		else {
			if ( k == 0 ) {
				for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
					if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m + PM1 ) ) {
						if ( !b[m] ) break;
					}
				}
				if ( m == MAX_PM_CHAMBER_DEPTH ) return FALSE;
				return TRUE;
			}
			else {
				for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
					if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m + PM1 ) ) {
						if ( !t[m] && b[m] ) break;
					}
				}
				if ( m == MAX_PM_CHAMBER_DEPTH ) return TRUE;
				return FALSE;
			}
		}
	}
	else {
		return FALSE;
	}
	//----------
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_exist_run_wafer_at_PM_FOR_6( int bmch ) {
	int i;
	for ( i = PM1 ; i < AL ; i++ ) { // 2004.02.25
		if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , i ) ) { // 2004.02.25
			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) return TRUE; // 2004.02.25
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( int bmch ) {
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 0 ) > 0 ) return TRUE;
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 1 ) > 0 ) return TRUE;
	return SCHEDULER_CONTROL_Chk_exist_run_wafer_at_PM_FOR_6( bmch );
}

BOOL SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( int bmch ) { // 2010.07.27
	int i , ss , es , ok , x , allok , out;
	int s , p;

	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) return FALSE;

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es );
	//
	out = FALSE; // 2013.1.29
	allok = FALSE; // 2010.10.20
	//
	for ( x = 0 ; x < MAX_CASSETTE_SIDE ; x++ ) { // 2010.10.20
		//
		ok = FALSE;
		//
		for ( i = ss ; i <= es ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
				p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
				//
				if ( s != x ) continue; // 2010.10.20
				//
				if ( ( _SCH_SYSTEM_MODE_END_GET( s ) >= 3 ) && ( _SCH_SYSTEM_MODE_END_GET( s ) <= 6 ) ) { // 2012.11.12
					ok = TRUE;
					break;
				}
				else {
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
						if ( _SCH_CLUSTER_Get_PathIn( s , p ) != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
							ok = TRUE;
							break;
						}
					}
				}
				//
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) && ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) ) { // 2013.01.29
					out = TRUE;
				}
				//
			}
		}
		//
		if ( ok ) {
			//
			allok = TRUE; // 2010.10.20
			//
			for ( i = ss ; i <= es ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
					s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
					p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
					//
					if ( s != x ) continue; // 2010.10.20
					//
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
						if ( _SCH_CLUSTER_Get_PathIn( s , p ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
							_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
							SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
						}
					}
					//
				}
			}
		}
	}
	//
	if ( out ) return TRUE; // 2013.01.29
	//
//	return ok; // 2010.10.20
	return allok; // 2010.10.20
}


BOOL SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPMBMIN_FOR_6( int bmch , BOOL dmchk ) { // 2003.03.27
	int i , ss , es; // 2004.02.24 // 2005.01.24
	int s , p; // 2010.07.02

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es ); // 2005.01.24

	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) ) { // 2010.09.09
				//
				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) { // 2010.07.02
					if ( dmchk ) { // 2010.07.02
						s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
						//
						if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) return TRUE;
						//
					}
					else {
						return TRUE;
					}
				}
				else {
					return TRUE; // 2004.02.24
				}
				//
			}
		}
	}
	return SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( bmch );
}

BOOL SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( int bmch ) {
	int i , ss , es; // 2004.02.24 // 2005.01.24

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es ); // 2005.01.24
	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) { // 2010.09.09
				return FALSE; // 2004.02.24
			}
		}
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_Slot1_getout_wafer_from_BM_FOR_6( int bmch ) { // 2007.02.09
	int ss , es;
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es );
	if ( ss == es ) return TRUE;
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( int bmch ) {
	int i , ss , es; // 2004.02.24 // 2005.01.24

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es ); // 2005.01.24

	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) return FALSE; // 2004.02.24
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_has_no_place_space_from_CM_FOR_6( int bmch ) {
	int i , ss , es; // 2004.02.24 // 2005.01.24
	int c = 0 , x = 0; // 2006.04.04

//	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2005.01.24 // 2014.12.24
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2005.01.24 // 2014.12.24

	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) {
			c++; // 2004.02.24 // 2006.04.04
		}
		else {
			if ( _SCH_CLUSTER_Get_Buffer( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) != 0 ) x++;
		}
	}
	if ( c > 0 ) { // 2006.04.04
		//
//		if ( SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( _SCH_PRM_GET_MODULE_GROUP( bmch ) ) == 0 ) { // 2013.10.30 // 2014.07.10
		if ( !SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( bmch ) ) { // 2013.10.30 // 2014.07.10
			//
			if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) ) { // 2007.05.02
				//------------------------------------------
				if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( _SCH_PRM_GET_MODULE_GROUP( bmch ) , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( _SCH_PRM_GET_MODULE_GROUP( bmch ) , TB_STATION ) ) return FALSE; // 2006.04.04
				//------------------------------------------
				if ( x != 0 ) return TRUE; // 2006.04.04
				//------------------------------------------
			}
			return FALSE; // 2006.04.04
			//------------------------------------------
		}
	}
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_Free_Count_FOR_6( int bmch , int *fslot , int *fslot2 , BOOL modecheck ) { // 2005.09.14
	int i , j , ss , es;

	*fslot = 0; // 2007.05.03
	*fslot2 = 0; // 2007.05.03

//	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2005.01.24 // 2014.12.24
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2005.01.24 // 2014.12.24

	j = 0;
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) {
			if ( *fslot == 0 ) {
				*fslot = i; // 2007.05.03
			}
			else {
				if ( *fslot2 == 0 ) {
					*fslot2 = i; // 2007.05.03
				}
			}
			j++;
		}
	}
	//
	if ( modecheck && ( j <= 1 ) && ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) ) { // 2009.03.09
		//
		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , bmch , &ss , &es );
		//
		for ( i = ss ; i <= es ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) j++;
		}
	}
	//
	return j;
}

BOOL SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_TM_ArmCheck_FOR_6( int bmc , int s1 , int p1 , int s2 , int p2 ) { // 2006.04.04
	int i , m , g;
	int ch1 = 0;
	int ch2 = 0;

	//=============================================================================================================
	g = _SCH_PRM_GET_MODULE_GROUP( bmc );
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( g , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( g , TB_STATION ) ) return TRUE;
	//=============================================================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( s1 , p1 , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( m ) == g ) ch1++;
		}
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( s2 , p2 , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( m ) == g ) ch2++;
		}
	}
	//=============================================================================================================
	if ( ch1 > ch2 ) return FALSE;
	//=============================================================================================================
	return TRUE;
}

//=======================================================================================
int SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_FOR_6( int TMATM , int CHECKING_SIDE , int Fingerx , int PickChamber ) {
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
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , chkch , G_PLACE ) ) {
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
										rets = 3; // 2007.10.12
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

//=======================================================================================
int SCHEDULING_CHECK_Switch_PLACE_From_Fixed_TM_and_OtherDisable_FOR_6( int TMATM , int CHECKING_SIDE , int Fingerx , int PickChamber ) {
	int k , chkch , Pointer , Side , Finger;
	//
	if ( Fingerx == TA_STATION ) Finger = TB_STATION;
	else                         Finger = TA_STATION;
	//
	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( TMATM,Finger ) )								return 1;
	Side    = _SCH_MODULE_Get_TM_SIDE( TMATM,Finger );
	Pointer = _SCH_MODULE_Get_TM_POINTER( TMATM,Finger );
	if ( _SCH_MODULE_Get_TM_WAFER( TMATM,Finger ) <= 0 )					return 2;

	if ( Pointer < 100 ) {
		if ( ( PickChamber >= PM1 ) && ( PickChamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
			if ( ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_IN ) && ( _SCH_CLUSTER_Get_PathRange( Side , Pointer ) == 0 ) ) return 3;
			if ( _SCH_MODULE_Get_TM_TYPE( TMATM,Finger ) == SCHEDULER_MOVING_OUT ) return 4;
			if ( _SCH_CLUSTER_Get_PathDo( Side , Pointer ) <= _SCH_CLUSTER_Get_PathRange( Side , Pointer ) ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					chkch = _SCH_CLUSTER_Get_PathProcessChamber( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
					if ( chkch > 0 ) {
//						if ( _SCH_PRM_GET_MODULE_xGROUP( chkch ) == TMATM ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( TMATM , chkch , G_PLACE ) ) {
							if ( chkch != PickChamber ) {
								_SCH_CLUSTER_Set_PathProcessChamber_Disable( Side , Pointer , _SCH_CLUSTER_Get_PathDo( Side , Pointer ) - 1 , k );
							}
						}
					}
				}
			}
			return 0;
		}
	}
	return 4;
}


//int SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( int side , int bmch , BOOL sdcheck ) { // 2014.05.23
//int SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( int side , int bmch , int skipch , int skipslot , BOOL sdcheck ) { // 2014.05.23 // 2014.08.22
int SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( int *rside , int bmch , int skipch , int skipslot , BOOL sdcheck ) { // 2014.05.23 // 2014.08.22
	int i , ss , es; // 2004.02.24 // 2005.01.24
	int uses; // 2005.03.16
	int pt; // 2005.04.20
	BOOL chk; // 2014.09.30

	uses = 0;

	//
	// 2014.09.30
	//
	if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) {
		chk = FALSE;
	}
	else {
		if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( bmch ) ) {
			chk = TRUE;
		}
		else {
			chk = FALSE;
		}
	}
	//
	//
//	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( bmch ) ) { // 2005.04.20 // 2014.09.30
	//
	if ( chk ) { // 2014.09.30

		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es ); // 2005.01.24

		if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch ) == 0 ) { // 2004.11.25
			for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
				//
				if ( ( bmch == skipch ) && ( i == skipslot ) ) continue; // 2014.05.23
				//
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) { // 2004.02.24
					/*
					// 2014.08.22
					if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) { // 2004.02.24
						if ( uses == 0 ) uses = i;
						// return i; // 2004.02.24 // 2005.03.16
					}
					else { // 2005.03.16
						if ( sdcheck ) { // 2007.05.03
							if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( side ) ) { // 2005.03.16
								return 0; // 2005.03.16
							} // 2005.03.16
						}
					}
					*/
					//
					// 2014.08.22
					if ( uses == 0 ) {
						if ( ( sdcheck ) || ( *rside == _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) ) {
							uses = i;
							*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						}
					}
					else {
						if ( sdcheck ) {
							if ( *rside != _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) {
								if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( *rside ) ) {
									uses = i;
									*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
								}
							}
						}
					}
					//
				}
			}
		}
		else { // 2004.11.25
			for ( i = es ; i >= ss ; i-- ) { // 2005.01.24
				//
				if ( ( bmch == skipch ) && ( i == skipslot ) ) continue; // 2014.05.23
				//
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
					/*
					// 2014.08.22
					//
					if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
						if ( uses == 0 ) uses = i;
						// return i; // 2004.02.24 // 2005.03.16
					}
					else { // 2005.03.16
						if ( sdcheck ) { // 2007.05.03
							if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( side ) ) { // 2005.03.16
								return 0; // 2005.03.16
							} // 2005.03.16
						}
					}
					*/
					//
					//
					// 2014.08.22
					if ( uses == 0 ) {
						if ( ( sdcheck ) || ( *rside == _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) ) {
							uses = i;
							*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						}
					}
					else {
						if ( sdcheck ) {
							if ( *rside != _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) {
								if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( *rside ) ) {
									uses = i;
									*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
								}
							}
						}
					}
					//
				}
			}
		}
	}
	else { // 2005.04.20

		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es );

		if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch ) == 0 ) {
			for ( i = ss ; i <= es ; i++ ) {
				//
				if ( ( bmch == skipch ) && ( i == skipslot ) ) continue; // 2014.05.23
				//
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
					/*
					// 2014.08.22
					//
					if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
						if ( uses == 0 ) {
							uses = i;
							pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
						}
						else {
							if ( pt > _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) {
								uses = i;
								pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
							}
						}
					}
					else {
						if ( sdcheck ) { // 2007.05.03
							if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( side ) ) {
								return 0;
							}
						}
					}
					*/
					//
					//
					// 2014.08.22
					if ( uses == 0 ) {
						if ( ( sdcheck ) || ( *rside == _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) ) {
							uses = i;
							pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
							*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						}
					}
					else {
						if ( *rside != _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) {
							if ( sdcheck ) {
								if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( *rside ) ) {
									uses = i;
									pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
									*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
								}
							}
						}
						else {
							if ( pt > _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) {
								uses = i;
								pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
								*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
							}
						}
					}
					//
				}
			}
		}
		else {
			for ( i = es ; i >= ss ; i-- ) {
				//
				if ( ( bmch == skipch ) && ( i == skipslot ) ) continue; // 2014.05.23
				//
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
					/*
					// 2014.08.22
					//
					if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
						if ( uses == 0 ) {
							uses = i;
							pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
						}
						else {
							if ( pt > _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) {
								uses = i;
								pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
							}
						}
					}
					else {
						if ( sdcheck ) { // 2007.05.03
							if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( side ) ) {
								return 0;
							}
						}
					}
					*/
					//
					//
					// 2014.08.22
					if ( uses == 0 ) {
						if ( ( sdcheck ) || ( *rside == _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) ) {
							uses = i;
							pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
							*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
						}
					}
					else {
						if ( *rside != _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) {
							if ( sdcheck ) {
								if ( _SCH_SYSTEM_RUNORDER_GET( _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) < _SCH_SYSTEM_RUNORDER_GET( *rside ) ) {
									uses = i;
									pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
									*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
								}
							}
						}
						else {
							if ( pt > _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) {
								uses = i;
								pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
								*rside = _SCH_MODULE_Get_BM_SIDE( bmch , i );
							}
						}
					}
					//
				}
			}
		}



	}

	return uses; // 2005.03.15
}


BOOL SCHEDULER_CONTROL_Chk_Wafer_is_Differ_FOR_6( int tmside , int bmch , int cs , int cp , int os , int op ) { // 2015.12.07
	int i , m , pm[MAX_PM_CHAMBER_DEPTH] , t , ts , tp;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pm[i] = FALSE;
	//
	t = 0;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( cs , cp , 0 , i );
		//
		if ( m > 0 ) {
			//
			if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
				//
				pm[m-PM1] = TRUE;
				//
				if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
						//
						t = 1;
						//
					}
				}
			}
			//
		}
		//
	}
	//
	if ( t == 0 ) return FALSE;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( os , op , 0 , i );
		//
		if ( m > 0 ) {
			if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
				//
				if ( pm[m-PM1] ) return FALSE;
				//
			}
		}
		//
	}
	//
	for ( t = TA_STATION ; t <= TB_STATION ; t++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tmside , t ) > 0 ) {
			//
			ts = _SCH_MODULE_Get_TM_SIDE( tmside , t );
			tp = _SCH_MODULE_Get_TM_POINTER( tmside , t );
			//
			if ( _SCH_CLUSTER_Get_PathDo( ts , tp ) <= _SCH_CLUSTER_Get_PathRange( ts , tp ) ) {
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					m = _SCH_CLUSTER_Get_PathProcessChamber( ts , tp  , 0 , i );
					//
					if ( m > 0 ) {
						//
						if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
							//
							if ( pm[m-PM1] ) return FALSE;
							//
						}
					}
					//
				}
				//
			}
			//
		}
	}
	//
	return TRUE;
	//
}

BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_OtherSlot_first_FOR_6( int tmside , int bmch , int slot ) { // 2014.06.18
	int i , ss , es;
	int cs, cp , csup;

	if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) != 2 ) && ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) != 3 ) ) return FALSE;
	//
	if ( _SCH_MODULE_Get_BM_WAFER( bmch , slot ) <= 0 ) return FALSE;
	//
	cs = _SCH_MODULE_Get_BM_SIDE( bmch , slot );
	cp = _SCH_MODULE_Get_BM_POINTER( bmch , slot );
	csup = _SCH_CLUSTER_Get_SupplyID( cs , cp );
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		//
//		if ( ( bmch == i ) && ( i == slot ) ) continue; // 2014.09.30
		if ( i == slot ) continue; // 2014.09.26
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) continue;
		//
		if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == cs ) continue; // 2014.09.26
		//
//		if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) , cs , cp ) ) continue; // 2014.09.30
		//
//		if ( csup > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) ) return TRUE; // 2015.12.07
		//
		if ( csup > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) ) {
			//
			if ( !SCHEDULER_CONTROL_Chk_Wafer_is_Differ_FOR_6( tmside , bmch , cs , cp , _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) ) return TRUE; // 2015.12.07
			//
		}
		//
	}
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( int side , int tms , int bmch , BOOL SWCheck ) {
	int i , ss , es; // 2004.02.24 // 2005.01.24

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es ); // 2005.01.24

	if ( SWCheck ) {
		for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( SCHEDULER_CONTROL_Chk_Will_Do_PM_POST_FOR_6( side , tms , bmch , i ) ) { // 2003.10.08
					return FALSE;
				}
			}
		}
	}
	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) return TRUE; // 2004.02.24
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_has_just_one_for_supply_FOR_6( int side , int bmch ) {
	int i , c , c2 , s , ss , es; // 2004.02.24 // 2005.01.24

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es ); // 2005.01.24

	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) return FALSE; // 2004.02.24
	}
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es ); // 2005.01.24
	//
	c = 0;
	c2 = 0; // 2005.01.24
	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) { // 2004.02.24
			c++; // 2004.02.24
			s = i; // 2004.02.24
		}
		else { // 2005.01.24
			c2++; // 2005.01.24
		} // 2005.01.24
	}
	if ( ( c > 0 ) && ( c2 > 0 ) ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_SIDE( bmch , s ) == side ) return TRUE; // 2004.02.24
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_one_out_full_FOR_6( int bmch ) {
	int i , c , c2 , ss , es; // 2004.02.24 // 2005.01.24

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es ); // 2005.01.24

	//
	c = 0;
	c2 = 0; // 2005.01.24
	for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) c++; // 2004.02.24
		else                                                  c2++; // 2005.01.24
	}
	if ( ( c > 0 ) && ( c2 > 0 ) ) { // 2004.02.24 // 2005.01.24

		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es ); // 2005.01.24

		c = 0; // 2004.02.24
		c2 = 0; // 2005.01.24
		for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) c++; // 2004.02.24
			else                                                  c2++; // 2005.01.24
		}
		if ( c2 == 0 ) return TRUE; // 2004.02.24 // 2005.01.24
		return FALSE; // 2004.02.24
	} // 2004.02.24
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_has_all_Out_FOR_6( int bmch ) { // 2013.11.15
	int i , ss , es;

	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) return FALSE;
	}
	//
	return TRUE;
	//
}
//
//BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_curr_out_last_one_FOR_6( int side , int bmch ) { // 2007.10.02 // 2013.10.30
BOOL SCHEDULER_CONTROL_Chk_BM_has_a_supply_curr_out_last_one_FOR_6( int side , int bmch , BOOL *hasin ) { // 2007.10.02 // 2013.10.30
	int i , co , ss , es;

	//-------------------------------------------------------------------------------------------------
//	if ( !_SCH_MODULE_Chk_FM_Finish_Status( side ) ) return FALSE; // 2013.10.30
	//-------------------------------------------------------------------------------------------------
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmch , &ss , &es );
	//
	co = 0;
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( side == _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) {
				co++;
			}
		}
	}
	if ( co == 0 ) return FALSE;
	//-------------------------------------------------------------------------------------------------
/*
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es );
	//
	ci = 0;
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			ci++;
			if ( side != _SCH_MODULE_Get_BM_SIDE( bmch , i ) ) return FALSE;
		}
	}
	if ( ci == 0 ) return FALSE;
*/
	//-------------------------------------------------------------------------------------------------
	// 2013.10.30
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmch , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			*hasin = TRUE;
			break;
		}
	}
	//-------------------------------------------------------------------------------------------------

	return TRUE;
}


int SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( int bmch , BOOL cycl ) { // 2008.01.16
	int s , p , i , k , t , ss , es , f , rt , ps;
	//
	if ( !cycl ) return 1;
	//
	rt = FALSE;
	//
//	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2014.12.24
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2014.12.24
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) {
				s = _SCH_MODULE_Get_BM_SIDE( bmch , i );
				p = _SCH_MODULE_Get_BM_POINTER( bmch , i );
				//===============================================================================================================================
				f = 0;
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) < _SCH_CLUSTER_Get_PathRange( s , p ) ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							t = _SCH_CLUSTER_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , k );
							if ( t > 0 ) {
								if ( _SCH_MODULE_GET_USE_ENABLE( t , FALSE , s ) ) {
									if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , t ) ) {
										f = 1;
										//
//										if ( _SCH_MACRO_CHECK_PROCESSING( t ) < 0 ) return -1; // 2011.08.23
										//
										ps = _SCH_MACRO_CHECK_PROCESSING( t ); // 2011.08.23
										//
										if ( ps < 0 ) {
											if ( ( ps != -2 ) && ( ps != -4 ) ) return -1;
										}
										//
									}
								}
							}
						}
					}
					else {
						f = 1;
					}
				}
				//
				if ( f == 0 ) {
					rt = TRUE;
					_SCH_MODULE_Set_BM_STATUS( bmch , i , BUFFER_OUTWAIT_STATUS );
					if ( _SCH_CLUSTER_Get_PathIn( s , p ) != _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
						SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
					}
					else {
						_SCH_CLUSTER_Set_PathDo( s , p , 1 );
					}
				}
				//
				//===============================================================================================================================
			}
		}
	}
	if ( rt ) return 0;
	return 1;
}



BOOL SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( int bmch , BOOL clichk , BOOL blockcheck , BOOL allcheck ) {
	int i , k , m , s , p , clsd , hasinv;

	//===================================================================
	// 2007.05.03
	//===================================================================
	clsd = -1;

//	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &m , &s ); // 2014.12.24
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &m , &s ); // 2014.12.24

	for ( i = m ; i <= s ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) continue; // 2010.09.09
				//===============================================================================================================================
				// 2007.07.05
				//===============================================================================================================================
				if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) {
					if ( clsd == -1 ) {
						clsd = _SCH_MODULE_Get_BM_SIDE( bmch , i );
					}
				}
				//===============================================================================================================================
			}
		}
	}
	//===================================================================
	if ( SCH_CYCLON2_GROUP_UNMATCHED_COUNT_RUN_STATUS( _SCH_PRM_GET_MODULE_GROUP( bmch ) ) ) return TRUE; // 2013.05.28
	//===================================================================
	if ( allcheck ) {
		if ( SCH_CYCLON2_GROUP_GET_NEED_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bmch ) , &s ) ) return FALSE; // 2013.01.29
	}
	//===================================================================
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
//		if ( _SCH_SYSTEM_USING_GET( s ) < 9 ) continue; // 2003.06.10 // 2005.07.29
		//========================================================================================================================
		// 2007.07.05
		//========================================================================================================================
		if ( clsd != -1 ) {
			if ( clsd != s ) continue;
		}
		//========================================================================================================================
		if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue; // 2005.07.29
		if ( _SCH_SYSTEM_MODE_END_GET( s ) != 0 ) continue;
		if ( _SCH_MODULE_Chk_FM_Finish_Status( s ) ) continue;
		//========================================================================================================================
		if ( !SCHEDULING_CHECK_Enable_PICK_from_FM_Properly_Group_for_STYLE_6( 0 , s , -1 , bmch , clichk ) ) continue; // 2005.09.14 // 2013.02.15
		//========================================================================================================================
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _SCH_CLUSTER_Get_PathRange( s , i ) >= 0 ) {
				switch( _SCH_CLUSTER_Get_PathStatus( s , i ) ) {
				case SCHEDULER_READY	:
					//
					if ( SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( bmch ) , s , i , !allcheck , &hasinv ) ) {
						//
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							//
							m = _SCH_CLUSTER_Get_PathProcessChamber( s , i , 0 , k );
							//
							if ( m > 0 ) { // 2015.12.03
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) { // 2003.06.05
									//
									if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) return FALSE; // 2004.02.25
									//
								}
								//
							}
							//
						}
						//
					}
					//
					if ( !allcheck ) i = MAX_CASSETTE_SLOT_SIZE; // 2013.01.29
					//
					break;
				}
			}
		}
	}
	//
	// 2011.05.04
	if ( !blockcheck ) return TRUE;
	if ( !FM_Pick_Running_Blocking_Style_6 ) return TRUE;
	//
	for ( i = 0 ; i < 10 ; i++ ) {
		//
		if ( !FM_Pick_Running_Blocking_Style_6 ) return TRUE; // 2013.01.29
		//
		s = FM_Pick_Running_Blocking_Style_6_Side;
		p = FM_Pick_Running_Blocking_Style_6_Pointer;
		//
		if ( s != -1 ) {
			if ( p != -1 ) {
				break;
			}
		}
		//
		Sleep(250);
		//
	}
	//
	if ( i == 10 ) return TRUE;
	//
	if ( SCHEDULING_CHECK_Enable_PICK_from_FM_Properly_Group_for_STYLE_6( 0 , s , p , bmch , clichk ) ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , k );
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) {
				if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
					if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
						return FALSE;
					}
					else {
						if ( SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( bmch ) , s , p , TRUE , &hasinv ) ) return FALSE; // 2013.01.25
					}
				}
			}
		}
	}
	//
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_Other_CM_FOR_6( int bmch , int *prerunside ) { // 2015.12.01
	int i , k , m , s , es , clsd , hasinv , bs , bp;
	int bmwillgopm[MAX_CHAMBER]; // 2015.12.16
	//
	*prerunside = -1;
	//
	if ( FM_Pick_Running_Blocking_Style_6 ) return 1; // 2015.12.07
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return 2; // 2015.12.07
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return 3; // 2015.12.07
	//
	//===================================================================
	if ( SCH_CYCLON2_GROUP_UNMATCHED_COUNT_RUN_STATUS( _SCH_PRM_GET_MODULE_GROUP( bmch ) ) ) return 4;
	//===================================================================
	if ( SCH_CYCLON2_GROUP_GET_NEED_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bmch ) , &s ) ) return 5;
	//===================================================================
	//
	clsd = -1;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) bmwillgopm[i] = 0; // 2015.12.06
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &s , &es ); // 2014.12.24

	for ( i = s ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			//
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) continue;
				//
//				if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) { // 2015.12.16
					if ( clsd == -1 ) {
						clsd = _SCH_MODULE_Get_BM_SIDE( bmch , i );
					}
//				} // 2015.12.16
				//===============================================================================================================================
				//
				// 2015.12.16
				//
				bs = _SCH_MODULE_Get_BM_SIDE( bmch , i );
				bp = _SCH_MODULE_Get_BM_POINTER( bmch , i );
				//
				if ( _SCH_CLUSTER_Get_PathDo( bs , bp ) != 0 ) continue;
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					m = _SCH_CLUSTER_Get_PathProcessChamber( bs , bp , 0 , k );
					//
					if ( m > 0 ) {
						//
						if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , bs ) ) continue;
						//
						if ( !SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) continue;
						//
						bmwillgopm[m] = 1;
						//
					}
				}
				//
			}
		}
	}
	//
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//========================================================================================================================
		if ( clsd != -1 ) {
			if ( clsd == s ) continue;
		}
		//========================================================================================================================
		if ( !_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
		if ( _SCH_SYSTEM_MODE_END_GET( s ) != 0 ) continue;
		if ( _SCH_MODULE_Chk_FM_Finish_Status( s ) ) continue;
		//========================================================================================================================
		//
		if ( !SCHEDULING_CHECK_Enable_PICK_from_FM_Properly_Group_for_STYLE_6( 0 , s , -1 , bmch , FALSE ) ) continue;
		//
		//========================================================================================================================
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathRange( s , i ) >= 0 ) {
				switch( _SCH_CLUSTER_Get_PathStatus( s , i ) ) {
				case SCHEDULER_READY	:
					//
					if ( SCH_CYCLON2_GROUP_SUPPLY_POSSIBLE( _SCH_PRM_GET_MODULE_GROUP( bmch ) , s , i , FALSE , &hasinv ) ) {
						//
						bs = 0;
						//
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							//
							m = _SCH_CLUSTER_Get_PathProcessChamber( s , i , 0 , k );
							//
							if ( m > 0 ) { // 2015.12.03
								//
								if ( !bmwillgopm[m] ) { // 2015.12.03
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) {
										//
										if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) {
											//
											bs = 1;
											//
										}
										//
									}
									//
								}
								else {
									bs = 0;
									break;
								}
							}
							//
						}
						//
						if ( bs == 1 ) {
							*prerunside = s;
							return 0;
						}
						//
					}
					break;
				}
			}
		}
	}
	//
	return 6;
	//
}

BOOL SCHEDULER_CONTROL_Chk_FMs_Target_BM_is_Yes_FOR_6( int bmch ) { // 2007.02.06
	if ( bmch != _SCH_MODULE_Get_FM_MidCount() ) return FALSE;
	if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) return FALSE;
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_BMs_Target_PM_is_free_FOR_6( int bmch , int slot , int *pm , BOOL *notdissts ) {
	int S , P , m , k;
	*pm = -1;
	*notdissts = FALSE; // 2005.06.16
	//
	if ( _SCH_MODULE_Get_BM_WAFER( bmch , slot ) <= 0 ) return FALSE;
	P = _SCH_MODULE_Get_BM_POINTER( bmch , slot );
	S = _SCH_MODULE_Get_BM_SIDE( bmch , slot );
	//
	if ( _SCH_CLUSTER_Get_PathDo( S , P ) == PATHDO_WAFER_RETURN ) return FALSE; // 2008.01.21
	//
	if ( _SCH_CLUSTER_Get_PathRange( S , P ) >= 0 ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , 0 , k );
			if ( m > 0 ) { // 2005.01.25
				if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
	//				if ( ( m == ( ( ( bmch - BM1 ) * 2 ) + PM1 + 0 ) ) || ( m == ( ( ( bmch - BM1 ) * 2 ) + PM1 + 1 ) ) ) { // 2003.05.28 // 2004.02.25
					if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) { // 2004.02.25
						*notdissts = TRUE; // 2005.06.16
						if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) {
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
								*pm = m;
								break;
							}
						}
					}
				}
			}
		}
		if ( k == MAX_PM_CHAMBER_DEPTH ) return FALSE;
		return TRUE;
	}
	return FALSE;
}


//int SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( int tmside , int bmch , int barm , int S , int P , int *arm ) { // 2014.07.18
int SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( int tmside , int barm , int S , int P , int *arm ) { // 2014.07.18
	int m , k , alldis;
	//
	alldis = TRUE; // 2008.01.15
	//
	*arm = 0; // 2008.01.21
	//
	if ( _SCH_CLUSTER_Get_PathDo( S , P ) == PATHDO_WAFER_RETURN ) return 1; // 2008.01.21
	//
	if ( _SCH_CLUSTER_Get_PathRange( S , P ) <= _SCH_CLUSTER_Get_PathDo( S , P ) ) return -1;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) , k );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
				//
//				if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmch , m ) ) { // 2014.07.18
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) == tmside ) { // 2014.07.18
					//
					alldis = FALSE; // 2008.01.15
					//
//					switch( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( m ) ) { // 2014.07.18
					switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tmside , m ) ) { // 2014.07.18
					case 0 :
						if      ( barm == 0 ) {
							*arm = 0;
						}
						else if ( barm == 1 ) {
							*arm = 1;
						}
						else if ( barm == 2 ) {
							*arm = 2;
						}
						return 1;
						break;
					case 1 :
						if      ( barm == 0 ) {
							*arm = 1;
							return 1;
						}
						else if ( barm == 1 ) {
							*arm = 1;
							return 1;
						}
						break;
					case 2 :
						if      ( barm == 0 ) {
							*arm = 2;
							return 1;
						}
						else if ( barm == 2 ) {
							*arm = 2;
							return 1;
						}
						break;
					}
				}
			}
		}
	}
	if ( alldis ) return 1; // 2008.01.15
	return 0;
}


BOOL SCHEDULER_CONTROL_Chk_BMs_Target_PM_is_ArmIntlks_OK_FOR_6( int tmside , int bmch , int slot , int *arm ) {
//	switch( SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( tmside , bmch , _SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( bmch ) , _SCH_MODULE_Get_BM_SIDE( bmch , slot ) , _SCH_MODULE_Get_BM_POINTER( bmch , slot ) , arm ) ) { // 2014.07.18
//	switch( SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( tmside , _SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( bmch ) , _SCH_MODULE_Get_BM_SIDE( bmch , slot ) , _SCH_MODULE_Get_BM_POINTER( bmch , slot ) , arm ) ) { // 2014.07.18 // 2014.07.31
	switch( SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( tmside , _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tmside , bmch ) , _SCH_MODULE_Get_BM_SIDE( bmch , slot ) , _SCH_MODULE_Get_BM_POINTER( bmch , slot ) , arm ) ) { // 2014.07.18 // 2014.07.31
	case 1 :
		return TRUE;
	default :
		return FALSE;
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_TMs_Target_PM_is_free_FOR_6( int tms , int pm ) { // 2005.03.16
	int i , S , P , m , k , sm;

	if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TB_STATION ) ) return TRUE; // 2005.04.21

	sm = FALSE; // 2005.05.23

	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) <= 0 ) continue;
		S = _SCH_MODULE_Get_TM_SIDE( tms , i );
		P = _SCH_MODULE_Get_TM_POINTER( tms , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( S , P ) <= _SCH_CLUSTER_Get_PathRange( S , P ) ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , 0 , k );
				if ( m > 0 ) {
					//--------------------------------------------------------
					if ( ( m == pm ) || ( pm <= 0 ) ) sm = TRUE; // 2005.05.23
					//--------------------------------------------------------
					if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
						if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( tms + BM1 , m ) ) {
							if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
									if ( !_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( S , m ) ) {
										if ( !_SCH_CLUSTER_Check_Possible_UsedPre( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) - 1 , k , m , FALSE ) ) {
											return TRUE;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else {
			return TRUE;
		}
	}
	//--------------------------------------------------------
	if ( !sm ) return TRUE; // 2005.05.23
	//--------------------------------------------------------
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( int tch , int pch , int S , int P ) { // 2007.02.09
	int k , m;
	//
	if ( _SCH_CLUSTER_Get_PathDo( S , P ) > _SCH_CLUSTER_Get_PathRange( S , P ) ) return TRUE;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) - 1 , k );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
				if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( tch + BM1 , m ) ) {
					//
					if ( m == pch ) return FALSE;
					if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) {
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
							if ( _SCH_PRM_GET_Putting_Priority( m ) >= _SCH_PRM_GET_Getting_Priority( pch ) ) {
								return FALSE;
							}
						}
					}
				}
			}
		}
	}
	//
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_Next_PM_Go_Impossible_FOR_6( int tch , int ch , int armcnt , int dis ) { // 2005.02.23
	int S , P , k , m;
	BOOL imposs1 , imposs2 , imposs3; // 2015.02.17

	if ( armcnt > 1 ) return FALSE;

	P = _SCH_MODULE_Get_PM_POINTER( ch , 0 );
	S = _SCH_MODULE_Get_PM_SIDE( ch , 0 );

	if ( _SCH_CLUSTER_Get_PathDo( S , P ) < _SCH_CLUSTER_Get_PathRange( S , P ) ) {
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) , k );
			if ( m > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
					if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( tch + BM1 , m ) ) {
						if ( m == ch ) return FALSE;
						if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) return FALSE;
					}
				}
			}
		}
		//==================================================================================================
		// 2005.03.04
		//==================================================================================================
		if      ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tch , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tch , TB_STATION ) ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tch , 0 ) > 0 ) {
				S = _SCH_MODULE_Get_TM_SIDE( tch , 0 );
				P = _SCH_MODULE_Get_TM_POINTER( tch , 0 );
				if ( _SCH_CLUSTER_Get_PathDo( S , P ) > _SCH_CLUSTER_Get_PathRange( S , P ) ) return FALSE;
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) - 1 , k );
					if ( m > 0 ) {
						if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
							if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( tch + BM1 , m ) ) {
								if ( m == ch ) return FALSE;
							}
						}
					}
				}
			}
			if ( _SCH_MODULE_Get_TM_WAFER( tch , 1 ) > 0 ) {
				S = _SCH_MODULE_Get_TM_SIDE( tch , 1 );
				P = _SCH_MODULE_Get_TM_POINTER( tch , 1 );
				if ( _SCH_CLUSTER_Get_PathDo( S , P ) > _SCH_CLUSTER_Get_PathRange( S , P ) ) return FALSE;
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _SCH_CLUSTER_Get_PathProcessChamber( S , P , _SCH_CLUSTER_Get_PathDo( S , P ) - 1 , k );
					if ( m > 0 ) {
						if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , S ) ) {
							if ( SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( tch + BM1 , m ) ) {
								if ( m == ch ) return FALSE;
							}
						}
					}
				}
			}
		}
		//==================================================================================================
		return TRUE;
	}
	else {
		//=====================================================================================
		// 2007.02.02
		//=====================================================================================
		//
		imposs1 = FALSE; // 2015.02.17
		imposs2 = FALSE; // 2015.02.17
		imposs3 = TRUE; // 2015.02.17
		//
		if      ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tch , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tch , TB_STATION ) ) {
			//
			// 2015.02.17
			//
			/*
			//
			if (
				SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch ) ||
				SCHEDULER_CONTROL_Chk_Slot1_getout_wafer_from_BM_FOR_6( BM1 + tch )
				) {
				if ( _SCH_MODULE_Get_TM_WAFER( tch , 0 ) > 0 ) {
					if ( SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( tch , ch , _SCH_MODULE_Get_TM_SIDE( tch , 0 ) , _SCH_MODULE_Get_TM_POINTER( tch , 0 ) ) ) return TRUE; // 2007.02.09
				}
				if ( _SCH_MODULE_Get_TM_WAFER( tch , 1 ) > 0 ) {
					if ( SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( tch , ch , _SCH_MODULE_Get_TM_SIDE( tch , 1 ) , _SCH_MODULE_Get_TM_POINTER( tch , 1 ) ) ) return TRUE; // 2007.02.09
				}
			}
			*/
			//
			if (
				SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch ) ||
				SCHEDULER_CONTROL_Chk_Slot1_getout_wafer_from_BM_FOR_6( BM1 + tch )
				) {
				if ( _SCH_MODULE_Get_TM_WAFER( tch , 0 ) > 0 ) {
					if ( SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( tch , ch , _SCH_MODULE_Get_TM_SIDE( tch , 0 ) , _SCH_MODULE_Get_TM_POINTER( tch , 0 ) ) ) imposs1 = TRUE; // 2007.02.09
				}
				if ( _SCH_MODULE_Get_TM_WAFER( tch , 1 ) > 0 ) {
					if ( SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( tch , ch , _SCH_MODULE_Get_TM_SIDE( tch , 1 ) , _SCH_MODULE_Get_TM_POINTER( tch , 1 ) ) ) imposs1 = TRUE; // 2007.02.09
				}
			}
			//
			if ( dis != 0 ) {
				if ( !SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch ) ) imposs3 = FALSE;
			}
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + tch + 2 , FALSE , -1 ) ) {
				if (
					( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
					( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + tch + 2 , 0 ) ) )
					) {
						if (
							SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch + 2 ) ||
							SCHEDULER_CONTROL_Chk_Slot1_getout_wafer_from_BM_FOR_6( BM1 + tch + 2 )
							) {
							if ( _SCH_MODULE_Get_TM_WAFER( tch , 0 ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( tch , ch , _SCH_MODULE_Get_TM_SIDE( tch , 0 ) , _SCH_MODULE_Get_TM_POINTER( tch , 0 ) ) ) imposs2 = TRUE; // 2007.02.09
							}
							if ( _SCH_MODULE_Get_TM_WAFER( tch , 1 ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_Next_PM_Impossible_From_TMSP_FOR_6( tch , ch , _SCH_MODULE_Get_TM_SIDE( tch , 1 ) , _SCH_MODULE_Get_TM_POINTER( tch , 1 ) ) ) imposs2 = TRUE; // 2007.02.09
							}
						}
						//
						if ( dis != 0 ) {
							if ( !SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch + 2 ) ) imposs3 = FALSE;
						}
						//
				}
				else {
					imposs2  = TRUE;
				}
			}
			else {
				imposs2  = TRUE;
			}
			//
		}
		else {
			//
			// 2015.02.17
			//
			/*
			//
			if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch ) ) return TRUE;
			*/
			//
			if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch ) ) imposs1 = TRUE;
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + tch + 2 , FALSE , -1 ) ) {
				if (
					( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
					( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + tch + 2 , 0 ) ) )
					) {
						if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tch + 2 ) ) imposs2 = TRUE;
				}
				else {
					imposs2  = TRUE;
				}
			}
			else {
				imposs2  = TRUE;
			}
			//
		}
		//
		// 2015.02.17
		//
		if ( imposs1 && imposs2 && imposs3 ) return TRUE;
		//
		//=====================================================================================
	}
	return FALSE;
}




/*
// 2013.10.23
int SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( int side , int tmatm , int bm , int *slot , int *ret , BOOL slotonly ) {
	int r1 , r2 , p1 , p2 , bs = -1;
	int i , ss , es; // 2004.02.24 // 2005.01.24
	//
	*ret = FALSE; // 2005.01.25
	//===============================================================================================
	// 2004.11.25
	//===============================================================================================
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bm , &ss , &es ); // 2005.01.24

	if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bm ) == 0 ) {
		for ( i = ss ; i <= es ; i++ ) { // 2005.01.25
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				bs = i;
				break;
			}
		}
	}
	else {
		for ( i = es ; i >= ss ; i-- ) { // 2005.01.25
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				bs = i;
				break;
			}
		}
	}
	//===============================================================================================
	if ( bs == -1 ) return -1;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) > 0 ) return -1;
	//
	if ( slotonly ) { // 2007.04.30
		*slot = bs;
		return 1;
	}
	//
	r1 = FALSE;
	r2 = FALSE;

	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 0 ) > 0 ) {
		if ( _SCH_MODULE_Get_TM_SIDE( tmatm , 0 ) == side ) {
			p1 = _SCH_MODULE_Get_TM_POINTER( tmatm , 0 );
			if ( _SCH_CLUSTER_Get_PathDo( side , p1 ) > _SCH_CLUSTER_Get_PathRange( side , p1 ) ) { // 2005.02.23
				r1 = TRUE;
			}
			else { // 2004.02.10 // 2005.02.23
				if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bm , side , p1 , 1 ) ) { // 2004.02.10
					r1 = TRUE; // 2004.02.10
					*ret = TRUE; // 2005.01.25
				}
			}
		}
	}
	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 1 ) > 0 ) {
		if ( _SCH_MODULE_Get_TM_SIDE( tmatm , 1 ) == side ) {
			p2 = _SCH_MODULE_Get_TM_POINTER( tmatm , 1 );
			if ( _SCH_CLUSTER_Get_PathDo( side , p2 ) > _SCH_CLUSTER_Get_PathRange( side , p2 ) ) { // 2005.02.23
				r2 = TRUE;
			}
			else { // 2004.02.10 // 2005.02.23
				if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bm , side , p2 , 1 ) ) { // 2004.02.10
					r2 = TRUE; // 2004.02.10
					*ret = TRUE; // 2005.01.25
				}
			}
		}
	}
	if ( !r1 && !r2 ) return -1;
	//
	*slot = bs;
	//
	if ( r1 && r2 ) {
		if ( p1 < p2 ) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else if ( r1 ) {
		return 0;
	}
	else if ( r2 ) {
		return 1;
	}
	return -1;
}
*/



// 2013.10.23
int SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( int side , int tmatm , int bm , int *slot , int *ret , BOOL slotonly ) {
	int r1 , r2 , s1 , s2 , p1 , p2 , bs = -1;
	int i , ss , es; // 2004.02.24 // 2005.01.24
	//
	*ret = FALSE; // 2005.01.25
	//===============================================================================================
	// 2004.11.25
	//===============================================================================================
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bm , &ss , &es ); // 2005.01.24

	if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bm ) == 0 ) {
		for ( i = ss ; i <= es ; i++ ) { // 2005.01.25
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				bs = i;
				break;
			}
		}
	}
	else {
		for ( i = es ; i >= ss ; i-- ) { // 2005.01.25
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				bs = i;
				break;
			}
		}
	}
	//===============================================================================================
	if ( bs == -1 ) return -1;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) > 0 ) return -1;
	//
	if ( slotonly ) { // 2007.04.30
		*slot = bs;
		return 1;
	}
	//
	r1 = FALSE;
	r2 = FALSE;

	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 0 ) > 0 ) {
		//
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tmatm , bm ) != 2 ) { // 2014.07.18
			//
			s1 = _SCH_MODULE_Get_TM_SIDE( tmatm , 0 );
			p1 = _SCH_MODULE_Get_TM_POINTER( tmatm , 0 );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) > _SCH_CLUSTER_Get_PathRange( s1 , p1 ) ) { // 2005.02.23
				r1 = TRUE;
			}
			else { // 2004.02.10 // 2005.02.23
				if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bm , s1 , p1 , 1 ) ) { // 2004.02.10
					r1 = TRUE; // 2004.02.10
					*ret = TRUE; // 2005.01.25
				}
			}
			//
		}
		//
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 1 ) > 0 ) {
		//
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tmatm , bm ) != 1 ) { // 2014.07.18
			//
			s2 = _SCH_MODULE_Get_TM_SIDE( tmatm , 1 );
			p2 = _SCH_MODULE_Get_TM_POINTER( tmatm , 1 );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) > _SCH_CLUSTER_Get_PathRange( s2 , p2 ) ) { // 2005.02.23
				r2 = TRUE;
			}
			else { // 2004.02.10 // 2005.02.23
				if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bm , s2 , p2 , 1 ) ) { // 2004.02.10
					r2 = TRUE; // 2004.02.10
					*ret = TRUE; // 2005.01.25
				}
			}
			//
		}
		//
	}
	if ( !r1 && !r2 ) return -1;
	//
	*slot = bs;
	//
	if ( r1 && r2 ) {
		if ( _SCH_CLUSTER_Get_SupplyID( s1 , p1 ) < _SCH_CLUSTER_Get_SupplyID( s2 , p2 ) ) {
			if ( s1 != side ) return -1;
			return 0;
		}
		else {
			if ( s2 != side ) return -1;
			return 1;
		}
	}
	else if ( r1 ) {
		if ( s1 != side ) return -1;
		return 0;
	}
	else if ( r2 ) {
		if ( s2 != side ) return -1;
		return 1;
	}
	return -1;
}

// 2015.02.13
int SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm2_FOR_6( int *side , int tmatm , int bm , int *slot , int *ret , BOOL slotonly ) {
	int r1 , r2 , s1 , s2 , p1 , p2 , bs = -1;
	int i , ss , es; // 2004.02.24 // 2005.01.24
	//
	*ret = FALSE; // 2005.01.25
	//===============================================================================================
	// 2004.11.25
	//===============================================================================================
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bm , &ss , &es ); // 2005.01.24

	if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bm ) == 0 ) {
		for ( i = ss ; i <= es ; i++ ) { // 2005.01.25
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				bs = i;
				break;
			}
		}
	}
	else {
		for ( i = es ; i >= ss ; i-- ) { // 2005.01.25
			if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
				bs = i;
				break;
			}
		}
	}
	//===============================================================================================
	if ( bs == -1 ) return -1;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) > 0 ) return -1;
	//
	if ( slotonly ) { // 2007.04.30
		*slot = bs;
		return 1;
	}
	//
	r1 = FALSE;
	r2 = FALSE;

	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 0 ) > 0 ) {
		//
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tmatm , bm ) != 2 ) { // 2014.07.18
			//
			s1 = _SCH_MODULE_Get_TM_SIDE( tmatm , 0 );
			p1 = _SCH_MODULE_Get_TM_POINTER( tmatm , 0 );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) > _SCH_CLUSTER_Get_PathRange( s1 , p1 ) ) { // 2005.02.23
				r1 = TRUE;
			}
			else { // 2004.02.10 // 2005.02.23
				if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bm , s1 , p1 , 1 ) ) { // 2004.02.10
					r1 = TRUE; // 2004.02.10
					*ret = TRUE; // 2005.01.25
				}
			}
			//
		}
		//
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tmatm , 1 ) > 0 ) {
		//
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tmatm , bm ) != 1 ) { // 2014.07.18
			//
			s2 = _SCH_MODULE_Get_TM_SIDE( tmatm , 1 );
			p2 = _SCH_MODULE_Get_TM_POINTER( tmatm , 1 );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) > _SCH_CLUSTER_Get_PathRange( s2 , p2 ) ) { // 2005.02.23
				r2 = TRUE;
			}
			else { // 2004.02.10 // 2005.02.23
				if ( SCHEDULER_CONTROL_Chk_Disable_PM_With_BM_FOR_6( bm , s2 , p2 , 1 ) ) { // 2004.02.10
					r2 = TRUE; // 2004.02.10
					*ret = TRUE; // 2005.01.25
				}
			}
			//
		}
		//
	}
	if ( !r1 && !r2 ) return -1;
	//
	*slot = bs;
	//
	if ( r1 && r2 ) {
		if ( _SCH_CLUSTER_Get_SupplyID( s1 , p1 ) < _SCH_CLUSTER_Get_SupplyID( s2 , p2 ) ) {
			*side = s1;
			return 0;
		}
		else {
			*side = s2;
			return 1;
		}
	}
	else if ( r1 ) {
		*side = s1;
		return 0;
	}
	else if ( r2 ) {
		*side = s2;
		return 1;
	}
	return -1;
}


// 2013.10.30
BOOL SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_Lock_FOR_6( int tmatm , BOOL *alltmout ) {
	int f , s , p , d;
	int i , k , m;
	BOOL hasout;
	//
	*alltmout = TRUE; // 2013.11.15
	//
	hasout = FALSE; // 2013.11.15
	//
	f = FALSE;
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tmatm , i ) <= 0 ) {
			*alltmout = FALSE;
			continue;
		}
		//
		s = _SCH_MODULE_Get_TM_SIDE( tmatm , i );
		p = _SCH_MODULE_Get_TM_POINTER( tmatm , i );
		d = _SCH_CLUSTER_Get_PathDo( s , p );
		//
//		if ( d > _SCH_CLUSTER_Get_PathRange( s , p ) ) return FALSE; // 2013.11.15
		if ( d > _SCH_CLUSTER_Get_PathRange( s , p ) ) { // 2013.11.15
			hasout = TRUE;
			continue;
		}
		//
		*alltmout = FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , d - 1 , k );
			//
			if ( m > 0 ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) {
					//
					if ( _SCH_PRM_GET_MODULE_GROUP( m ) == tmatm ) {
						//
						if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) > 0 ) return FALSE;
						//
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) return FALSE;
						//
						f = TRUE;
						//
					}
				}
			}
		}
		//
	}
	//
	if ( hasout ) return FALSE; // 2013.11.15
	//
	if ( !f ) return FALSE;
	//
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( int bmch , BOOL runcheck ) {
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) { // 2006.05.26
		if ( runcheck ) { // 2006.05.26
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmch ) <= 0 ) return TRUE; // 2006.05.26
		} // 2006.05.26
		else { // 2006.05.26
			return TRUE; // 2006.05.26
		} // 2006.05.26
	} // 2006.05.26
	return FALSE;
}


int SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Cyclon_FOR_6( int bmch , int mode , int s1 , int p1 , int w1 , int s2 , int p2 , int w2 ) { // 2007.05.01
	int i , c , ss , es , wc , pmc;
	int pms[MAX_CASSETTE_SLOT_SIZE];
	//===============================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) return 0;
	//===============================================================================================
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) return 1;
	//===============================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) {
		pmc = Scheduler_PM_Get_First_for_CYCLON( bmch - BM1 , FALSE );
	}
	else { // 2013.01.25
		pmc = Scheduler_PM_Get_First_for_CYCLON( _SCH_PRM_GET_MODULE_GROUP( bmch ) , FALSE );
	}
	//===============================================================================================
//	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2014.12.24
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmch , &ss , &es ); // 2014.12.24
	//===============================================================================================
	c = 0;
	//===============================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) pms[i] = 0;
	//
	if ( mode == 1 ) { // 2010.12.02
		pms[SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s1 , p1 )] = 1;
	}
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) {
			c++;
		}
		else {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				if ( SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) continue; // 2010.09.09
				//
				pms[SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) )] = 1;
			}
		}
	}
	//===============================================================================================
	if ( c <= 0 ) return 1;
	//
	if ( SCHEDULER_CONTROL_SWAP_IMPOSSIBLE( _SCH_PRM_GET_MODULE_GROUP( bmch ) , -1 , -1 ) ) { // 2014.12.24
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bmch , BUFFER_INWAIT_STATUS ) <= 0 ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bmch , BUFFER_OUTWAIT_STATUS ) > 0 ) {
					if ( c <= 1 ) return 1;
				}
			}
		}
	}
	//
	//===============================================================================================
	wc = 0;
	//===============================================================================================
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) {
				//
				if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( bmch , i ) ) continue; // 2010.09.13
				//
				if ( w1 > 0 ) {
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) , s1 , p1 ) ) {
						return 1;
					}
				}
				if ( w2 > 0 ) {
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) , s2 , p2 ) ) {
						return 2;
					}
				}
				wc++;
			}
		}
	}
	if ( wc == 0 ) {
		if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
			if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , s2 , p2 ) ) { // 2007.07.29
				return 2;
			}
		}
	}
	//===============================================================================================
	if ( w1 > 0 ) {
		c = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s1 , p1 );
		if ( ( c > 0 ) && ( pms[c] == 1 ) ) return 1;
	}
	if ( w2 > 0 ) {
		c = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s2 , p2 );
		if ( ( c > 0 ) && ( pms[c] == 1 ) ) return 2;
	}
	if ( wc == 0 ) {
		if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
			c = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s1 , p1 );
			i = SCHEDULER_CONTROL_ST6_GET_SLOT( pmc , s2 , p2 );
			if ( ( c > 0 ) && ( i > 0 ) && ( c == i ) ) {
				return 2;
			}
		}
	}
	//===============================================================================================
	return 0;
}

//========================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_Place_BM_Force_Waiting_FOR_6( int bmch , int side , int p1 , int w1 , int p2 , int w2 , BOOL *PickOtherWait ) { // 2013.11.27
	int i ,p , pmc;
	//
	*PickOtherWait = FALSE;
	//
	//===============================================================================================
	if ( _SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() <= 0 ) return FALSE;
	//===============================================================================================
//	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) { // 2014.07.10
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 1 ) { // 2014.07.10
		pmc = Scheduler_PM_Get_First_for_CYCLON( bmch - BM1 , FALSE );
	}
	else {
		pmc = Scheduler_PM_Get_First_for_CYCLON( _SCH_PRM_GET_MODULE_GROUP( bmch ) , FALSE );
	}
	//
	if ( pmc <= 0 ) return FALSE;
	//
	if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
		//
		if ( p1 > p2 ) {
			p = p1+1;
		}
		else {
			p = p2+1;
		}
		//
	}
	else if ( w1 > 0 ) {
		//
		p = p1+1;
		//
	}
	else if ( w2 > 0 ) {
		//
		p = p2+1;
		//
	}
	else {
		//
		return FALSE;
		//
	}
	//
	for ( i = p ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_READY ) return FALSE;
		//
	}
	//
	if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 0 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) ) {
		//
		for ( p = 0 ; p < MAX_CASSETTE_SIDE ; p++ ) {
			//==========================================================================================
			if ( side == p ) continue;
			//==========================================================================================
			if ( _SCH_SYSTEM_MODE_END_GET( p ) != 0 ) continue;
			//==========================================================================================
			if ( _SCH_SYSTEM_USING_GET( p ) < 6 ) continue;
			//==========================================================================================
//			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( p , 5 );
//			if ( !_SCH_SYSTEM_USING_RUNNING( p ) ) continue;
			//================================================================================================
			if ( !_SCH_MODULE_Chk_FM_Finish_Status( p ) ) {
				//
				*PickOtherWait = TRUE;
				//
				return FALSE;
			}
			//================================================================================================
		}
	}
	else {
		return FALSE;
	}
	//
	if ( _SCH_MACRO_CHECK_PROCESSING( pmc ) <= 0 ) return FALSE;
	return TRUE;
	//
}

//========================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_MIC_FOR_6( int slot , int side , int pt , int slot2 , int side2 , int pt2 ) { // 2007.11.06
	int i , k , ss , es;
	if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) return TRUE;
	if ( ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() != -4 ) && ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() != -14 ) ) return TRUE;
	for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
		if ( _SCH_PRM_GET_MODULE_MODE( k + TM ) ) {
			SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k , &ss , &es );
			for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
				if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
					if ( _SCH_MODULE_Get_MFIC_WAFER( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) ) > 0 ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}

//========================================================================================================================

BOOL TM_PM_EnableCheck( int tms ) { // 2018.12.24
	int pmc;
	//
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() < 1 ) return TRUE;
	//
	pmc = Scheduler_PM_Get_First_for_CYCLON( tms , TRUE );
	//
	if ( pmc <= 0 ) return TRUE;
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( pmc , FALSE , -1 ) ) return FALSE;
	//
	return TRUE;
}


int SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_PRE_FOR_6( int side , int bmmodex , int Slotmode , int intlksmode , int *rbm , int *Justoneuse , BOOL runcheck ) {
	int i , k , ss , es , selk1 , selk2; // 2005.01.24
	int bmb;
	int bmuse[MAX_TM_CHAMBER_DEPTH];
	int bmc[MAX_TM_CHAMBER_DEPTH];
	int bmo[MAX_TM_CHAMBER_DEPTH];
	//========================================================================================================================
	int bmuse2[MAX_TM_CHAMBER_DEPTH]; // 2013.01.19
	int bmc2[MAX_TM_CHAMBER_DEPTH]; // 2013.01.19
	int bmo2[MAX_TM_CHAMBER_DEPTH]; // 2013.01.19
	int rmd1 , rmd2; // 2013.01.19
	int rbm1 , rbm2; // 2013.01.19
	int ret1 , ret2; // 2013.01.19
	int jone1 , jone2; // 2013.01.19
	int hasfree , hasslot; // 2014.07.10
	int rsup1 , rsup2; // 2014.10.03
	int hasout; // 2014.10.03
	//
//	int pmc; // 2018.10.31 // 2018.12.24
	//
	//========================================================================================================================
	if ( bmmodex == 0 ) { // 2004.02.25
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			bmc[i] = 0;
			bmc2[i] = 0; // 2013.01.19
			//
			//
			// 2018.10.31
			//
			if ( TM_PM_EnableCheck( i ) ) { // 2018.12.24
				//
				if ( _SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , -1 ) ) { // 2013.11.22
						//
						bmuse[i] = 1;
						//
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2014.09.16
							//
							switch( intlksmode ) {
							case 1 :
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i ) == 2 ) bmuse[i] = 0;
								break;
							case 2 :
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i ) == 1 ) bmuse[i] = 0;
								break;
							}
							//
						}
						//
					}
					else {
						bmuse[i] = 0;
					}
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i + 2 , FALSE , -1 ) ) { // 2013.11.22
						//
						if (
							( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
							( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + i + 2 , 0 ) ) )
							) { // 2014.11.13
							//
							bmuse2[i] = 1; // 2013.01.19
							//
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2014.09.16
								//
								switch( intlksmode ) {
								case 1 :
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i + 2 ) == 2 ) bmuse2[i] = 0;
									break;
								case 2 :
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i + 2 ) == 1 ) bmuse2[i] = 0;
									break;
								}
								//
							}
							//
						}
						else {
							bmuse2[i] = 0; // 2014.11.13
						}
					}
					else {
						bmuse2[i] = 0; // 2013.01.19
					}
				}
				else {
					bmuse[i] = 0;
					bmuse2[i] = 0; // 2013.01.19
				}
			}
			else {
				bmuse[i] = 0;
				bmuse2[i] = 0; // 2013.01.19
			}
		}
	}
	else {
		bmb = bmmodex;
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			bmc[i] = 0;
			bmc2[i] = 0; // 2013.01.19
			//
			//
			//
			// 2018.10.31
			//
			if ( TM_PM_EnableCheck( i ) ) { // 2018.12.24
				//
				if ( _SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , -1 ) ) { // 2013.11.22
						//
						bmuse[i] = bmb % 10;
						//
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2014.09.16
							//
							switch( intlksmode ) {
							case 1 :
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i ) == 2 ) bmuse[i] = 0;
								break;
							case 2 :
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i ) == 1 ) bmuse[i] = 0;
								break;
							}
							//
						}
						//
					}
					else {
						bmuse[i] = 0;
					}
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i + 2 , FALSE , -1 ) ) { // 2013.11.22
						//
						if (
							( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
							( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + i + 2 , 0 ) ) )
							) { // 2014.11.13
							//
							bmuse2[i] = bmb % 10; // 2013.01.19
							//
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2014.09.16
								//
								switch( intlksmode ) {
								case 1 :
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i + 2 ) == 2 ) bmuse2[i] = 0;
									break;
								case 2 :
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + i + 2 ) == 1 ) bmuse2[i] = 0;
									break;
								}
								//
							}
							//
						}
						else {
							bmuse2[i] = 0;
						}
					}
					else {
						bmuse2[i] = 0;
					}
				}
				else {
					bmuse[i] = 0;
					bmuse2[i] = 0; // 2013.01.19
				}
				//
			}
			else {
				bmuse[i] = 0;
				bmuse2[i] = 0;
			}
			//
			//
			bmb = bmb / 10;
		}
	}
	//========================================================================================================================
//	*Justoneuse	= FALSE; // 2013.01.25
	jone1 = FALSE; // 2013.01.25
	jone2 = FALSE; // 2013.01.25
	//========================================================================================================================
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[][bmmodex=%d][bmuse=%d,%d][bmc=%d,%d][bmuse2=%d,%d][bmc2=%d,%d]==\n" , bmmodex , bmuse[0] , bmuse[1] , bmc[0] , bmc[1] , bmuse2[0] , bmuse2[1] , bmc2[0] , bmc2[1] );
	if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) { // 2004.02.24 // 2005.01.24
		//
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( _SCH_PRM_GET_MODULE_MODE( k + TM ) ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
					//
					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k , &ss , &es ); // 2005.01.24
					//
					for ( i = ss ; i <= es ; i++ ) { // 2004.02.24 // 2005.01.24
						if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
							//
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) { // 2010.09.09
								bmc[k]++;
							}
							//
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) { // 2010.09.09
								bmc[k]++;
							}
						}
					}
					//
				}
				//
//				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
				if (
					( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
					( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
					)
				{ // 2013.01.19 // 2014.07.08
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
						//
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k + 2 , &ss , &es );
						//
						for ( i = ss ; i <= es ; i++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) > 0 ) {
								//
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
									bmc2[k]++;
								}
								//
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
									bmc2[k]++;
								}
							}
						}
						//
					}
					//
				}
				//
			}
		}
		//
		i = 0;
		//
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			if ( bmuse[k] ) {
				if ( bmc[k] != 0 ) {
					bmuse[k] = 0;
				}
				else {
					i++;
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( bmc2[k] != 0 ) {
						bmuse2[k] = 0;
					}
					else {
						i++;
					}
				}
			}
		}
		//
//		if ( i == 0 ) return -1; // 2014.05.26
		if ( i == 0 ) return -11; // 2014.05.26
//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) bmc[i] = 0;
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) bmc2[i] = 0; // 2013.01.19
		//
	}
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[][runcheck=%d][bmuse=%d,%d][bmc=%d,%d][bmuse2=%d,%d][bmc2=%d,%d]==\n" , runcheck , bmuse[0] , bmuse[1] , bmc[0] , bmc[1] , bmuse2[0] , bmuse2[1] , bmc2[0] , bmc2[1] );
	//========================================================================================================================
	// 2007.08.24
	//========================================================================================================================
	if ( runcheck ) {
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			if ( bmuse[k] ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) != BUFFER_WAIT_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) != BUFFER_FM_STATION ) ) {
						bmuse[k] = 0;
					}
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) != BUFFER_WAIT_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) != BUFFER_FM_STATION ) ) {
							bmuse2[k] = 0;
						}
					}
				}
			}
		}
	}
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[X1][Slotmode=%d][bmuse=%d,%d][bmc=%d,%d][bmuse2=%d,%d][bmc2=%d,%d]==\n" , Slotmode , bmuse[0] , bmuse[1] , bmc[0] , bmc[1] , bmuse2[0] , bmuse2[1] , bmc2[0] , bmc2[1] );
	//========================================================================================================================
	switch( Slotmode ) {
	case 0 : // AB
	case 3 : // AB2
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) { // 2004.02.25
			//
			if ( bmuse[k] ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
					//
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) == BUFFER_FM_STATION ) ) {
						//
//						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2005.01.24 // 2014.12.24
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2005.01.24 // 2014.12.24
						//
						for ( i = ss ; i <= es ; i++ ) { // 2005.01.24
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) <= 0 ) break;
						}
						if ( i > es ) { // 2005.01.24
							bmuse[k] = 0;
						}
						else if ( i == es ) { // 2005.01.24
							bmc[k] = i;
							//
	//						*Justoneuse	= TRUE; // 2013.01.25
							jone1 = TRUE; // 2013.01.25
//							break; // 2014.10.03
						}
						else {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i + 1 ) <= 0 ) {
								bmc[k] = i;
							}
							else {
								bmc[k] = i;
	//							*Justoneuse	= TRUE; // 2013.01.25
								jone1 = TRUE; // 2013.01.25
//								break; // 2014.10.03
							}
						}
					}
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
						//
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) == BUFFER_FM_STATION ) ) {
							//
//							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
							//
							for ( i = ss ; i <= es ; i++ ) {
								if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) <= 0 ) break;
							}
							if ( i > es ) {
								bmuse2[k] = 0;
							}
							else if ( i == es ) {
								bmc2[k] = i;
								jone2 = TRUE;
//								break; // 2014.10.03
							}
							else {
								if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i + 1 ) <= 0 ) {
									bmc2[k] = i;
								}
								else {
									bmc2[k] = i;
									jone2 = TRUE;
//									break; // 2014.10.03
								}
							}
						}
					}
				}
			}
		}
		break;
	case 1 : // A1
	case 2 : // B1
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) { // 2004.02.25
			//
			if ( bmuse[k] ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) == BUFFER_FM_STATION ) ) {
						//
//						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2005.01.24 // 2014.12.24
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2005.01.24 // 2014.12.24
						//
						for ( i = es ; i >= ss ; i-- ) { // 2005.01.24
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) <= 0 ) {
								bmc[k] = i;
							}
/*
//
// 2015.12.15
//
							else {
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) ) { // 2010.09.09
	//							if ( !SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_INWAIT_STATUS ) ) { // 2007.05.03
									if ( !_SCH_MODULE_Chk_FM_Finish_Status( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) ) ) {
										if ( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) != side ) {
											bmc[k] = 0;
											break;
										}
									}
								}
							}
*/
//
						}
					}
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) == BUFFER_FM_STATION ) ) {
							//
//							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
							//
							for ( i = es ; i >= ss ; i-- ) {
								if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) <= 0 ) {
									bmc2[k] = i;
								}
/*
//
// 2015.12.15
//
								else {
									if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) ) {
										if ( !_SCH_MODULE_Chk_FM_Finish_Status( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) ) ) {
											if ( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) != side ) {
												bmc2[k] = 0;
												break;
											}
										}
									}
								}
*/
//
//
							}
						}
					}
				}
			}
		}
		break;
	}
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[X2][Slotmode=%d][bmuse=%d,%d][bmc=%d,%d][bmuse2=%d,%d][bmc2=%d,%d]==\n" , Slotmode , bmuse[0] , bmuse[1] , bmc[0] , bmc[1] , bmuse2[0] , bmuse2[1] , bmc2[0] , bmc2[1] );
	//==================================================================================================================================================================================
	//==================================================================================================================================================================================
	//==================================================================================================================================================================================
	//==================================================================================================================================================================================
	//==================================================================================================================================================================================
	//==================================================================================================================================================================================
	rmd1 = 0;
	rmd2 = 0;
	rbm1 = 0;
	rbm2 = 0;
	ret1 = 0;
	ret2 = 0;
	//
	selk1 = -1; // 2010.08.05
	selk2 = -1; // 2010.08.05
	//
	for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) { // 2004.02.25
		//
		if ( bmuse[k] && ( bmc[k] != 0 ) ) {
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
				//
				// 2014.07.10
				//
				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) {
					//
					if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) == BUFFER_FM_STATION ) {
						//
						// 2014.10.03
						//
						hasout = 0;
						//
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k , &ss , &es );
						//
						for ( i = ss ; i <= es ; i++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
								//
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) ) {
									hasout = 1;
									break;
								}
							}
						}
						//
						//
//						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2014.12.24
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2014.12.24
						//
						hasfree = 0;
						hasslot = 0;
						//
						for ( i = ss ; i <= es ; i++ ) {
							//
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
								//
								if ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) {
									//
									hasslot = i;
									//
								}
								//
							}
							else {
								hasfree++;
							}
						}
						//
						if ( ( hasfree > 0 ) && ( hasslot > 0 ) ) {
							//
							if ( ( rmd1 >= 2 ) && ( rbm1 > 0 ) ) { // 2014.10.03
								//
								rbm1 = BM1 + k;
								ret1 = bmc[k];
								//
	//							rmd1 = 1; // 2014.10.03
								rmd1 = 2; // 2014.10.03
								//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A01] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
							}
							//
						}
						else {
							//
							if ( ( hasfree > 0 ) && ( hasout <= 0 ) ) { // 2014.10.03
								//
								if      ( ( rmd1 == 1 ) && ( rbm1 > 0 ) ) {
									if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , rbm1 ) > _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , BM1 + k ) ) {
										rbm1 = BM1 + k;
										ret1 = bmc[k];
										//
										rmd1 = 1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A02] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
									}
								}
								else if ( ( rmd2 == 1 ) && ( rbm2 > 0 ) ) {
									if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , rbm2 ) > _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , BM1 + k ) ) {
										//
										rbm2 = 0;
										rmd2 = 0;
										//
										rbm1 = BM1 + k;
										ret1 = bmc[k];
										//
										rmd1 = 1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A03] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
									}
								}
								else {
									//
									rbm1 = BM1 + k;
									ret1 = bmc[k];
									//
									rmd1 = 1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A04] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
								}
								//
							}
							//
						}
					}
					//
				}
				//
				SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k , &ss , &es ); // 2005.01.24
				//
				for ( i = ss ; i <= es ; i++ ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
						//
						if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) { // 2010.09.09
	//						*rbm = BM1 + k; // 2013.01.25
	//						return bmc[k]; // 2013.01.25
							//
							rbm1 = BM1 + k;
							ret1 = bmc[k];
							//
//							rmd1 = 2; // 2014.10.03
							rmd1 = 3; // 2014.10.03
							//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A05] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
							k = MAX_TM_CHAMBER_DEPTH;
							break;
							//
						}
						//
						if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) { // 2010.09.09 // 2014.10.03
							//
							if ( selk1 == -1 ) {
								selk1 = k; // 2010.08.05
							}
							//
//							if ( ( rmd1 == 1 ) && ( rbm1 == ( BM1 + k ) ) ) { // 2014.07.10 // 2014.10.03
							//
								if ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) {
									//
//									rmd1 = 0; // 2014.10.03
//									rbm1 = 0; // 2014.10.03
									//
									if      ( ( rmd1 == 4 ) && ( rbm1 > 0 ) ) {
										//
										if ( rsup1 > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k , i ) ) ) {
											//
											rmd1 = 4;
											rbm1 = BM1 + k;
											ret1 = bmc[k];
											//
											rsup1 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k , i ) );
											//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A06] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
										}
									}
									else if ( ( rmd2 == 4 ) && ( rbm2 > 0 ) ) {
										//
										if ( rsup2 > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k , i ) ) ) {
											//
											rmd2 = 0;
											rbm2 = 0;
											//
											rmd1 = 4;
											rbm1 = BM1 + k;
											ret1 = bmc[k];
											//
											rsup1 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k , i ) );
											//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A07] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
										}
									}
									else {
										//
										if ( rbm1 == 0 ) {
											//
											rmd1 = 4;
											rbm1 = BM1 + k;
											ret1 = bmc[k];
											//
											rsup1 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k , i ) );
											//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[A08] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
										}
										//
									}
								}
							//
							//
						}
						//
					}
				}
				//
			}
		}
		//
//		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
		if (
			( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
			( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
			)
		{ // 2013.01.19 // 2014.07.08
			if ( bmuse2[k] && ( bmc2[k] != 0 ) ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
					//
					// 2014.07.10
					//
					if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) {
						//
						if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) == BUFFER_FM_STATION ) {
							//
							// 2014.10.03
							//
							hasout = 0;
							//
							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k + 2 , &ss , &es );
							//
							for ( i = ss ; i <= es ; i++ ) {
								if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) > 0 ) {
									//
									if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) ) {
										hasout = 1;
										break;
									}
								}
							}
							//
							//
//							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
							SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
							//
							hasfree = 0;
							hasslot = 0;
							//
							for ( i = ss ; i <= es ; i++ ) {
								//
								if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) > 0 ) {
									//
									if ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
										//
										hasslot = i;
										//
									}
									//
								}
								else {
									hasfree++;
								}
							}
							//
							if ( ( hasfree > 0 ) && ( hasslot > 0 ) ) {
								//
								if ( ( rmd2 >= 2 ) && ( rbm2 > 0 ) ) { // 2014.10.03
									//
									rbm2 = BM1 + k + 2;
									ret2 = bmc2[k];
									//
	//								rmd2 = 1; // 2014.10.03
									rmd2 = 2; // 2014.10.03
									//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B01] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
								}
								//
							}
							else {
								//
								if ( ( hasfree > 0 ) && ( hasout <= 0 ) ) { // 2014.10.03
									//
									if      ( ( rmd1 == 1 ) && ( rbm1 > 0 ) ) {
										if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , rbm1 ) > _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , BM1 + k + 2 ) ) {
											//
											rbm1 = 0;
											rmd1 = 0;
											//
											rbm2 = BM1 + k + 2;
											ret2 = bmc2[k];
											//
											rmd2 = 1;
											//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B02] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
										}
									}
									else if ( ( rmd2 == 1 ) && ( rbm2 > 0 ) ) {
										if ( _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , rbm2 ) > _SCH_TIMER_GET_PROCESS_TIME_START( 0 /* BUFFER_FM_STATION */ , BM1 + k + 2 ) ) {
											//
											rbm2 = BM1 + k + 2;
											ret2 = bmc2[k];
											//
											rmd2 = 1;
											//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B03] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
										}
									}
									else {
										//
										rbm2 = BM1 + k + 2;
										ret2 = bmc2[k];
										//
										rmd2 = 1;
										//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B04] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
									}
									//
								}
								//
							}
						}
					}
					//
					//
					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k + 2 , &ss , &es );
					//
					for ( i = ss ; i <= es ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) > 0 ) {
							//
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_INWAIT_STATUS ) && !SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
								//
								rbm2 = BM1 + k + 2;
//								ret2 = bmc[k]; // 2014.01.10
								ret2 = bmc2[k]; // 2014.01.10
								//
//								rmd2 = 2; // 2014.10.03
								rmd2 = 3; // 2014.10.03
								//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B05] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
								k = MAX_TM_CHAMBER_DEPTH;
								break;
							}
							//
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
								if ( selk2 == -1 ) {
									selk2 = k;
								}
								//
//								if ( ( rmd2 == 1 ) && ( rbm2 == ( BM1 + k + 2 ) ) ) { // 2014.07.10 // 2014.10.03
									//
									if ( ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
										//
//										rmd2 = 0; // 2014.10.03
//										rbm2 = 0; // 2014.10.03
										//
										if      ( ( rmd1 == 4 ) && ( rbm1 > 0 ) ) {
											//
											if ( rsup1 > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k + 2 , i ) ) ) {
												//
												rmd1 = 0;
												rbm1 = 0;
												//
												rmd2 = 4;
												rbm2 = BM1 + k + 2;
//												ret2 = bmc[k]; // 2015.01.20
												ret2 = bmc2[k]; // 2015.01.20
												//
												rsup2 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k + 2 , i ) );
												//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B06] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
											}
										}
										else if ( ( rmd2 == 4 ) && ( rbm2 > 0 ) ) {
											//
											if ( rsup2 > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k + 2 , i ) ) ) {
												//
												rmd2 = 4;
												rbm2 = BM1 + k + 2;
												ret2 = bmc2[k];
												//
												rsup2 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k + 2 , i ) );
												//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B07] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
											}
										}
										else {
											//
											if ( rbm2 == 0 ) {
												//
												rmd2 = 4;
												rbm2 = BM1 + k + 2;
												ret2 = bmc2[k];
												//
												rsup2 = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( BM1 + k + 2 , i ) , _SCH_MODULE_Get_BM_POINTER( BM1 + k + 2 , i ) );
												//
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[B08] [%d,%d][%d,%d]\n" , rbm1 , rmd1 , rbm2 , rmd2 );
											}
										}


//									}
								}
								//
							}
						}
					}
				}
			}
		}
	}
	//
	if ( rbm1 == 0 ) {
		if ( selk1 != -1 ) { // 2010.08.05
//			*rbm = BM1 + selk; // 2010.08.05 // 2013.01.25
//			return bmc[selk]; // 2010.08.05 // 2013.01.25
			//
			rbm1 = BM1 + selk1; // 2013.01.25
			ret1 = bmc[selk1]; // 2013.01.25
			//
//			rmd1 = 3; // 2014.10.03
			rmd1 = 5; // 2014.10.03
			//
		}
		//
	}
	//
	if ( rbm2 == 0 ) {
		if ( selk2 != -1 ) {
			rbm2 = BM1 + selk2 + 2;
			ret2 = bmc2[selk2];
			//
//			rmd2 = 3; // 2014.10.03
			rmd2 = 5; // 2014.10.03
			//
		}
	}
	//
	if ( rbm1 == 0 ) {
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) { // 2004.02.25
			if ( bmuse[k] && ( bmc[k] != 0 ) ) {
	//			*rbm = BM1 + k; // 2013.01.25
	//			return bmc[k]; // 2013.01.25
				rbm1 = BM1 + k; // 2013.01.25
				ret1 = bmc[k]; // 2013.01.25
				//
//				rmd1 = 4; // 2014.10.03
				rmd1 = 6; // 2014.10.03
				//
				break;
			}
		}
	}
	//
	if ( rbm2 == 0 ) { // 2013.01.25
//		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2014.07.08
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 1 ) { // 2014.07.08
			for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
				if ( bmuse2[k] && ( bmc2[k] != 0 ) ) {
					rbm2 = BM1 + k + 2;
					ret2 = bmc2[k]; // 2013.01.25
					//
//					rmd2 = 4; // 2014.10.03
					rmd2 = 6; // 2014.10.03
					//
					break;
				}
				//
			}
		}
		//
	}
	//

//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =============================================\n" );

//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[bmuse=%d,%d][bmc=%d,%d][bmuse2=%d,%d][bmc2=%d,%d]==\n" , bmuse[0] , bmuse[1] , bmc[0] , bmc[1] , bmuse2[0] , bmuse2[1] , bmc2[0] , bmc2[1] );
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[rbm1=%d][rbm2=%d]=========================\n" , rbm1 , rbm2 );
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[rmd1=%d][rmd2=%d]=========================\n" , rmd1 , rmd2 );
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[jone1=%d][jone2=%d]=========================\n" , jone1 , jone2 );

//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =============================================\n" );
	if      ( ( rbm1 != 0 ) && ( rbm2 != 0 ) ) {
		//
		if ( ( _SCH_MODULE_Get_BM_FULL_MODE( rbm1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( rbm1 ) == BUFFER_FM_STATION ) ) {
			if ( ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_FM_STATION ) ) {
				if ( rmd1 <= rmd2 ) {
					*rbm = rbm1;
					*Justoneuse	= jone1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 1][%d][%d]=========================\n" , *rbm , ret1 );
					return ret1;
				}
				else {
					*rbm = rbm2;
					*Justoneuse	= jone2;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 2][%d][%d]=========================\n" , *rbm , ret2 );
					return ret2;
				}
			}
			else {
				*rbm = rbm1;
				*Justoneuse	= jone1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 3][%d][%d]=========================\n" , *rbm , ret1 );
				return ret1;
			}
		}
		else if ( ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_FM_STATION ) ) {
			*rbm = rbm2;
			*Justoneuse	= jone2;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 4][%d][%d]=========================\n" , *rbm , ret2 );
			return ret2;
		}
		else {
			if ( rmd1 <= rmd2 ) {
				*rbm = rbm1;
				*Justoneuse	= jone1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 5][%d][%d]=========================\n" , *rbm , ret1 );
				return ret1;
			}
			else {
				*rbm = rbm2;
				*Justoneuse	= jone2;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 6][%d][%d]=========================\n" , *rbm , ret2 );
				return ret2;
			}
		}
	}
	else if ( rbm1 != 0 ) {
		*rbm = rbm1;
		*Justoneuse	= jone1;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 7][%d][%d]=========================\n" , *rbm , ret1 );
		return ret1;
	}
	else if ( rbm2 != 0 ) {
		*rbm = rbm2;
		*Justoneuse	= jone2;
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL 8][%d][%d]=========================\n" , *rbm , ret2 );
		return ret2;
	}
//_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM =[SEL ??]================================\n" );
	//===========================================================================================================
	// 2006.08.30
	//===========================================================================================================
//	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 0 ) return -1; // 2014.05.26
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 0 ) return -21; // 2014.05.26
//	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 1 ) return -1; // 2014.05.26
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 1 ) return -22; // 2014.05.26
//	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 3 ) return -1; // 2014.05.26
	if ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() == 3 ) return -23; // 2014.05.26
	//===========================================================================================================
	if ( bmmodex == 0 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			bmc[i] = 0;
			bmc2[i] = 0; // 2013.01.19
			//
			//
			if ( TM_PM_EnableCheck( i ) ) { // 2018.12.24
				//
				if ( _SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , -1 ) ) { // 2013.11.22
						bmuse[i] = 1;
					}
					else {
						bmuse[i] = 0;
					}
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i + 2 , FALSE , -1 ) ) { // 2013.11.22
						bmuse2[i] = 1; // 2013.01.19
					}
					else {
						bmuse2[i] = 0; // 2013.01.19
					}
				}
				else {
					bmuse[i] = 0;
					bmuse2[i] = 0; // 2013.01.19
				}
			}
			else {
				bmuse[i] = 0;
				bmuse2[i] = 0;
			}
		}
	}
	else {
		bmb = bmmodex;
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			bmc[i] = 0;
			bmc2[i] = 0; // 2013.01.19
			//
			//
			if ( TM_PM_EnableCheck( i ) ) { // 2018.12.24
				//
				if ( _SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , -1 ) ) { // 2013.11.22
						bmuse[i] = bmb % 10;
					}
					else {
						bmuse[i] = 0;
					}
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + i + 2 , FALSE , -1 ) ) { // 2013.11.22
						bmuse2[i] = bmb % 10; // 2013.01.19
					}
					else {
						bmuse2[i] = 0; // 2013.01.19
					}
				}
				else {
					bmuse[i] = 0;
					bmuse2[i] = 0; // 2013.01.19
				}
				//
			}
			else {
				bmuse[i] = 0;
				bmuse2[i] = 0;
			}
			//
			bmb = bmb / 10;
		}
	}
	//========================================================================================================================
	// 2007.08.24
	//========================================================================================================================
	if ( runcheck ) {
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			//
			if ( bmuse[k] ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) != BUFFER_WAIT_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k ) != BUFFER_FM_STATION ) ) {
						bmuse[k] = 0;
					}
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) != BUFFER_WAIT_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + k + 2 ) != BUFFER_FM_STATION ) ) {
							bmuse2[k] = 0;
						}
					}
				}
			}
			//
		}
	}
	//========================================================================================================================
//	*Justoneuse	= FALSE;
	jone1 = FALSE;
	jone2 = FALSE;
	//========================================================================================================================
	switch( Slotmode ) {
	case 0 : // AB
	case 3 : // AB2
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( bmuse[k] ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
//					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2014.12.24
					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2014.12.24
					//
					bmc[k] = 0;
					for ( i = ss ; i <= es ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) <= 0 ) {
							bmc[k]++;
						}
					}
					//
					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k , &ss , &es );
					//
					bmo[k] = 0;
					for ( i = ss ; i <= es ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) { // 2010.09.09
								bmo[k]++;
							}
						}
					}
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
//						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
						//
						bmc2[k] = 0;
						for ( i = ss ; i <= es ; i++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) <= 0 ) {
								bmc2[k]++;
							}
						}
						//
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k + 2 , &ss , &es );
						//
						bmo2[k] = 0;
						for ( i = ss ; i <= es ; i++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) > 0 ) {
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
									bmo2[k]++;
								}
							}
						}
					}
				}
				//
			}
		}
		break;
	case 1 : // A1
	case 2 : // B1
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( bmuse[k] ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k , FALSE , -1 ) ) { // 2013.11.22
//					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2014.12.24
					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k , &ss , &es ); // 2014.12.24
					//
					bmc[k] = 0;
					for ( i = es ; i >= ss ; i-- ) {
						if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) <= 0 ) {
							bmc[k]++;
						}
					}
					//
					SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k , &ss , &es );
					//
					bmo[k] = 0;
					for ( i = ss ; i <= es ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , i ) > 0 ) {
							if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k , i ) ) { // 2010.09.09
								bmo[k]++;
							}
						}
					}
				}
			}
			//
//			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 2014.07.08
			if (
				( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) ||
				( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) && ( !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + k + 2 , 0 ) ) )
				)
			{ // 2013.01.19 // 2014.07.08
				if ( bmuse2[k] ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + k + 2 , FALSE , -1 ) ) { // 2013.11.22
//						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , BM1 + k + 2 , &ss , &es ); // 2014.12.24
						//
						bmc2[k] = 0;
						for ( i = es ; i >= ss ; i-- ) {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) <= 0 ) {
								bmc2[k]++;
							}
						}
						//
						SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PICK , BM1 + k + 2 , &ss , &es );
						//
						bmo2[k] = 0;
						for ( i = ss ; i <= es ; i++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k + 2 , i ) > 0 ) {
								if ( ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) || ( _SCH_MODULE_Get_BM_STATUS( BM1 + k + 2 , i ) == BUFFER_OUTWAIT_STATUS ) || SCHEDULER_CONTROL_Chk_BM_RETURN_STYLE_6( BM1 + k + 2 , i ) ) {
									bmo2[k]++;
								}
							}
						}
					}
				}
				//
			}
		}
		break;
	}
	//
//	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.19 // 204.07.08
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 1 ) { // 2013.01.19 // 204.07.08
		rmd1 = -1;
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( bmuse[k] ) {
				if ( rmd1 == -1 ) {
					rmd1 = k;
				}
				else {
					if ( bmc[k] > bmc[rmd1] ) {
						rmd1 = k;
					}
					else if ( bmc[k] == bmc[rmd1] ) {
						if ( bmo[k] > bmo[rmd1] ) {
							rmd1 = k;
						}
					}
				}
			}
		}
		//
		rmd2 = -1;
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( bmuse2[k] ) {
				if ( rmd2 == -1 ) {
					rmd2 = k;
				}
				else {
					if ( bmc2[k] > bmc2[rmd2] ) {
						rmd2 = k;
					}
					else if ( bmc2[k] == bmc2[rmd2] ) {
						if ( bmo2[k] > bmo2[rmd2] ) {
							rmd2 = k;
						}
					}
				}
			}
		}
		//
		if ( rmd1 >= 0 ) {
			rbm1 = BM1 + rmd1;
		}
		else {
			rbm1 = 0;
		}
		//
		if ( rmd2 >= 0 ) {
			rbm2 = BM1 + rmd2 + 2;
		}
		else {
			rbm2 = 0;
		}
		//
	}
	else {
		rmd1 = -1;
		for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
			//
			if ( bmuse[k] ) {
				if ( rmd1 == -1 ) {
					rmd1 = k;
				}
				else {
					if ( bmc[k] > bmc[rmd1] ) {
						rmd1 = k;
					}
					else if ( bmc[k] == bmc[rmd1] ) {
						if ( bmo[k] > bmo[rmd1] ) {
							rmd1 = k;
						}
					}
				}
			}
		}
		//
		if ( rmd1 >= 0 ) {
			rbm1 = BM1 + rmd1;
		}
		else {
			rbm1 = 0;
		}
		//
		rbm2 = 0;
		//
	}
	//
	if      ( ( rbm1 != 0 ) && ( rbm2 != 0 ) ) {
		//
		if ( ( _SCH_MODULE_Get_BM_FULL_MODE( rbm1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( rbm1 ) == BUFFER_FM_STATION ) ) {
			if ( ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_FM_STATION ) ) {
				//
				if ( bmc[rmd1] > bmc2[rmd2] ) {
					*rbm = rbm2;
					*Justoneuse	= jone2;
					return 1;
				}
				else if ( bmc[rmd1] == bmc2[rmd2] ) {
					if ( bmo[rmd1] > bmo2[rmd2] ) {
						*rbm = rbm2;
						*Justoneuse	= jone2;
						return 1;
					}
					else {
						*rbm = rbm1;
						*Justoneuse	= jone1;
						return 1;
					}
				}
				else {
					*rbm = rbm1;
					*Justoneuse	= jone1;
					return 1;
				}
			}
			else {
				*rbm = rbm1;
				*Justoneuse	= jone1;
				return 1;
			}
		}
		else if ( ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( rbm2 ) == BUFFER_FM_STATION ) ) {
			*rbm = rbm2;
			*Justoneuse	= jone2;
			return 1;
		}
		else {
			if ( rmd1 <= rmd2 ) {
				*rbm = rbm1;
				*Justoneuse	= jone1;
				return 1;
			}
			else {
				*rbm = rbm2;
				*Justoneuse	= jone2;
				return 1;
			}
		}
	}
	else if ( rbm1 != 0 ) {
		*rbm = rbm1;
		*Justoneuse	= jone1;
		return 1;
	}
	else if ( rbm2 != 0 ) {
		*rbm = rbm2;
		*Justoneuse	= jone2;
		return 1;
	}
	else {
//		return -1; // 2014.05.26
		return -31; // 2014.05.26
	}
}


//
int SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_FOR_6( int side , int bm , int Slotmode , int *slot2 ) {
	int bmyes = -1;
	int i , ss , es; // 2005.01.24
	//
	*slot2 = 0; // 2004.11.25
	//
	if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) == BUFFER_FM_STATION ) ) {
		// slotmode	 0:AB 1:A 2:B 3:B2
		switch( Slotmode ) {
		case 0 :
			//==============================================================================================================
			//==============================================================================================================
			// 2004.11.25
			//==============================================================================================================
			//==============================================================================================================
			//
//			SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bm , &ss , &es ); // 2005.01.24 // 2014.12.24
			SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bm , &ss , &es ); // 2005.01.24 // 2014.12.24
			//
			if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bm ) == 0 ) {
				for ( i = ss ; i <= es ; i++ ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) break;
				}
				if ( i < es ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( bm , i + 1 ) <= 0 ) {
						bmyes = i;
						*slot2 = bmyes + 1; // 2004.11.25
					}
				}
			}
			else { // 2004.11.25
				for ( i = es ; i >= ss ; i-- ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) break;
				}
				if ( i > ss ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( bm , i - 1 ) <= 0 ) {
						bmyes = i;
						*slot2 = bmyes - 1;
					}
				}
			}
			//==============================================================================================================
			//==============================================================================================================
			break;
		case 1 :
		case 2 :
			//
//			SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bm , &ss , &es ); // 2005.01.24 // 2014.12.24
			SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bm , &ss , &es ); // 2005.01.24 // 2014.12.24
			//
			if ( SCHEDULER_CONTROL_Chk_GET_ORDER_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bm ) == 0 ) { // 2004.11.25
				for ( i = es ; i >= ss ; i-- ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
						bmyes = i;
					}
/*
// 2008.07.13
					else {
						if ( !SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() ) { // 2007.05.15
							if ( !_SCH_MODULE_Chk_FM_Finish_Status( _SCH_MODULE_Get_BM_SIDE( bm , i ) ) ) {
								if ( _SCH_MODULE_Get_BM_SIDE( bm , i ) != side ) {
									bmyes = -1;
									break;
								}
							}
						}
					}
*/
				}
			}
			else { // 2004.11.25
				for ( i = ss ; i <= es ; i++ ) { // 2005.01.24
					if ( _SCH_MODULE_Get_BM_WAFER( bm , i ) <= 0 ) {
						bmyes = i;
					}
/*
// 2008.07.13
					else {
						if ( !SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() ) { // 2007.05.15
							if ( !_SCH_MODULE_Chk_FM_Finish_Status( _SCH_MODULE_Get_BM_SIDE( bm , i ) ) ) {
								if ( _SCH_MODULE_Get_BM_SIDE( bm , i ) != side ) {
									bmyes = -1;
									break;
								}
							}
						}
					}
*/
				}
			}
			break;
		}
	}
	return bmyes;
}


// 2014.07.10
/*
BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( int side , int tmside ) {
	int swmode;
	swmode = 0;
	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( BM1 + tmside ) ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( side , tmside , BM1 + tmside , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
				swmode = 1;
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( BM1 + tmside ) ) {
					swmode = 1;
				}
			}
		}
	}
	else {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TB_STATION ) ) {
			if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
				swmode = 1;
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( BM1 + tmside ) ) {
					swmode = 1;
				}
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( BM1 + tmside ) ) {
				if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
					swmode = 1;
				}
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPMBMIN_FOR_6( BM1 + tmside , FALSE ) ) {
					swmode = 1;
				}
			}
		}
	}
	if ( swmode == 0 ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( tmside , TRUE ) ) return FALSE;
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( int side , int tmside ) {
	int swmode;
	swmode = 0;
	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( BM1 + tmside ) ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( side , tmside , BM1 + tmside , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
				if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( BM1 + tmside , FALSE , FALSE , TRUE ) ) {
					swmode = 1;
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
					swmode = 1;
				}
			}
		}
	}
	else {
		if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
			swmode = 1;
		}
	}
	if ( swmode == 0 ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( tmside , TRUE ) ) return FALSE;
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( int side , int tmside ) {
	int swmode;
	swmode = 0;
	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( BM1 + tmside ) ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( side , tmside , BM1 + tmside , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
				if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( BM1 + tmside , FALSE , FALSE , TRUE ) ) {
					swmode = 1;
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_FMs_Target_BM_is_Yes_FOR_6( BM1 + tmside ) ) {
						swmode = 1;
					}
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( BM1 + tmside ) ) {
					swmode = 1;
				}
			}
		}
	}
	if ( swmode == 0 ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( tmside , FALSE ) ) return FALSE;
	return TRUE;
}
*/

// 2014.07.10
//
BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( int side , int tmside , int bmc ) {
	int swmode;
	swmode = 0;
	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( bmc ) ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( side , tmside , bmc , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( bmc ) ) {
				swmode = 1;
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( bmc ) ) {
					swmode = 1;
				}
			}
		}
	}
	else {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TB_STATION ) ) {
			if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( bmc ) ) {
				swmode = 1;
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( bmc ) ) {
					swmode = 1;
				}
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPM_FOR_6( bmc ) ) {
				if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( bmc ) ) {
					swmode = 1;
				}
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPMBMIN_FOR_6( bmc , FALSE ) ) {
					swmode = 1;
				}
			}
		}
	}
	if ( swmode == 0 ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( tmside , TRUE ) ) return FALSE;
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( int side , int tmside , int bmc ) {
	int swmode;
	swmode = 0;
	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( bmc ) ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( side , tmside , bmc , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( bmc ) ) {
				if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bmc , FALSE , FALSE , TRUE ) ) {
					swmode = 1;
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( bmc ) ) {
					swmode = 1;
				}
			}
		}
	}
	else {
		if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( bmc ) ) {
			swmode = 1;
		}
	}
	if ( swmode == 0 ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( tmside , TRUE ) ) return FALSE;
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( int side , int tmside , int bmc ) {
	int swmode;
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc , FALSE , -1 ) ) return FALSE; // 2014.07.10
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return FALSE; // 2014.07.10
	if ( _SCH_MACRO_CHECK_PROCESSING( bmc ) > 0 ) return FALSE; // 2014.07.10
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( bmc , 0 ) ) return FALSE; // 2014.07.10
	//
	swmode = 0;
	if ( SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( bmc ) ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( side , tmside , bmc , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( bmc ) ) {
				if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bmc , FALSE , FALSE , TRUE ) ) {
					swmode = 1;
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_FMs_Target_BM_is_Yes_FOR_6( bmc ) ) {
						swmode = 1;
					}
				}
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( bmc ) ) {
					swmode = 1;
				}
			}
		}
	}
	if ( swmode == 0 ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_TM_MAKE_FMSIDE_FOR_6( tmside , FALSE ) ) return FALSE;
	return TRUE;
}




BOOL SCHEDULER_CONTROL_Use_LOT_Check_4_DUMMY_WAFER_SLOT_STYLE_6( int mode , int bmc , int slot ) { // 2010.07.02
	int s , i;
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//========================================================================================================================
		if ( !_SCH_SYSTEM_USING_STARTING( s ) ) continue;
		//========================================================================================================================
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _SCH_CLUSTER_Get_PathRange( s , i ) < 0 ) continue;
			//
//			if ( _SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_CM_END ) continue; // 2010.07.27
			if ( _SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_CM_END ) continue; // 2010.07.27
			//
			if ( mode == 1 ) {
				if ( _SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_READY ) continue; // 2010.07.27
			}
			//
			if ( ( _SCH_CLUSTER_Get_PathIn( s , i ) == bmc ) && ( _SCH_CLUSTER_Get_SlotIn( s , i ) == slot ) ) return TRUE;
			//
		}
		//========================================================================================================================
	}
	//
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_6( int side , int *ch , int *slot , int *uc , int skipslot ) { // 2007.03.19
	int i , mx = -1 , s = -1 , c , l;
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , side ) ) return FALSE;
	//
	for ( l = 0 ; l < 2 ; l++ ) { // 2010.09.17
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ; i++ ) {
			//
			if ( ( i + 1 ) == skipslot ) continue;
			//
			if ( _SCH_SDM_Get_RUN_CHAMBER(i) == 0 ) continue; // 2008.09.03
			//
			if ( SCHEDULER_CONTROL_Use_LOT_Check_4_DUMMY_WAFER_SLOT_STYLE_6( l , _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) ) continue; // 2010.07.02
			//
			if ( *ch == -1 ) {
	//			if ( _SCH_SDM_Get_RUN_CHAMBER(i) == 0 ) continue; // 2007.05.04 // 2008.09.03
				if ( _SCH_PRM_GET_MODULE_MODE( _SCH_SDM_Get_RUN_CHAMBER(i) ) ) continue; // 2007.05.04
			}
			else {
				if ( _SCH_SDM_Get_RUN_CHAMBER(i) != *ch ) {
					if ( _SCH_PRM_GET_MODULE_MODE( _SCH_SDM_Get_RUN_CHAMBER(i) ) ) continue; // 2007.05.04 // 2008.09.03
					// continue; // 2007.05.04 // 2008.09.03
				}
			}
			if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , i + 1 ) <= 0 ) continue;
				if ( ( mx == -1 ) || ( mx > _SCH_SDM_Get_RUN_USECOUNT(i) ) ) {
					mx = _SCH_SDM_Get_RUN_USECOUNT(i);
					s = i;
					c = _SCH_SDM_Get_RUN_CHAMBER(i);
				}
	//		}
		}
		//
		if ( l == 0 ) { // 2010.09.17
			if ( s != -1 ) break;
		}
		//
	}
	if ( s == -1 ) return FALSE;
	*slot = s + 1;
	*ch = c;
	*uc = mx;
	return TRUE;
}
//



