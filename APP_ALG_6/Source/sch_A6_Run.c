//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_preprcs.h"
#include "INF_sch_prepost.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A6_subBM.h"
#include "sch_A6_sub.h"
#include "sch_A6_Init.h"
//================================================================================


//------------------------------------------------------------------------------------------
extern BOOL FM_Pick_Running_Blocking_Style_6; // 2006.08.02
//------------------------------------------------------------------------------------------


int SCHEDULING_MOVE_OPERATION( int tms , int mode , int side , int pt , int mdl , int arm , int wfr , int slot , int switching , int swmode ); // 2013.11.07

// 2004.08.16 // 2014.07.17
//int SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( int tms , int side , int pm , int runcheckmode , int *order , int *ptr ) { // 2014.08.21
int SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( int tms , int side0 , int pm , int runcheckmode , int *order , int *ptr ) { // 2014.08.21
	int r1 , r2 , r3 , p1 , p2 , o1 , o2 , m , k;
	int s1 , s2 , e1 , e2; // 2014.08.21
	//
	*ptr = 0;
	e1 = 0;
	e2 = 0;
	//
	if ( runcheckmode == 0 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pm , 0 ) > 0 ) {
			*ptr = 1;
			return -1;
		}
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pm ) > 0 ) {
			*ptr = 2;
			return -1;
		}
	}
	else if ( runcheckmode == 1 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pm , 0 ) <= 0 ) {
			*ptr = 3;
			return -1;
		}
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pm ) <= 0 ) {
			return -99;
		}
		if ( ( _SCH_MODULE_Get_TM_WAFER( tms , 0 ) > 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tms , 1 ) > 0 ) ) {
			*ptr = 4;
			return -1;
		}
	}
	else if ( runcheckmode == 2 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( pm , 0 ) > 0 ) {
			*ptr = 5;
			return -1;
		}
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pm ) <= 0 ) {
			return -99;
		}
	}
	else {
		*ptr = 7;
		return -1;
	}
	//
	r1 = FALSE;
	r2 = FALSE;
	r3 = FALSE; /* 2005.05.11 */
	//
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) {
		//
//		if ( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( pm ) != 2 ) { /* 2007.07.26 */ // 2014.07.18
		if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms , pm ) != 2 ) { /* 2007.07.26 */ // 2014.07.18
			//
//			if ( _SCH_MODULE_Get_TM_SIDE( tms , TA_STATION ) == side ) { // 2014.08.21
			//
			s1 = _SCH_MODULE_Get_TM_SIDE( tms , TA_STATION ); // 2014.08.21
			//
//			if ( ( side0 < 0 ) || ( side0 == s1 ) ) { // 2014.08.21 // 2016.03.23
				//
				p1 = _SCH_MODULE_Get_TM_POINTER( tms , TA_STATION );
				//
//				if ( _SCH_CLUSTER_Get_PathDo( side , p1 ) <= _SCH_CLUSTER_Get_PathRange( side , p1 ) ) { // 2014.08.21
				if ( _SCH_CLUSTER_Get_PathDo( s1 , p1 ) <= _SCH_CLUSTER_Get_PathRange( s1 , p1 ) ) { // 2014.08.21
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
//						m = _SCH_CLUSTER_Get_PathProcessChamber( side , p1 , _SCH_CLUSTER_Get_PathDo( side , p1 ) - 1 , k ); // 2014.08.21
						m = _SCH_CLUSTER_Get_PathProcessChamber( s1 , p1 , _SCH_CLUSTER_Get_PathDo( s1 , p1 ) - 1 , k ); // 2014.08.21
						//
						if ( m == pm ) {
							//
//							if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( side , p1 , _SCH_CLUSTER_Get_PathDo( side , p1 ) - 1 , k ) ) { /* 2005.05.11 */ // 2014.08.21
							if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( s1 , p1 , _SCH_CLUSTER_Get_PathDo( s1 , p1 ) - 1 , k ) ) { /* 2005.05.11 */ // 2014.08.21
								//
								r3 = TRUE; /* 2005.05.11 */
								r2 = FALSE;
							}  /* 2005.05.11 */
							//
							r1 = TRUE;
							o1 = k;
							//
							break;
						}
					}
				}
				else {
					e1 = 4;
				}
//			} // 2016.03.23
//			else { // 2016.03.23
//				e1 = 3; // 2016.03.23
//			} // 2016.03.23
		}
		else {
			e1 = 2;
		}
	}
	else {
		e1 = 1;
	}
	//
	if ( !r3 ) { /* 2005.05.11 */
		if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) {
			//
//			if ( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( pm ) != 1 ) { /* 2007.07.26 */
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms , pm ) != 1 ) { /* 2007.07.26 */ // 2014.07.18
				//
//				if ( _SCH_MODULE_Get_TM_SIDE( tms , TB_STATION ) == side ) { // 2014.08.21
				//
				s2 = _SCH_MODULE_Get_TM_SIDE( tms , TB_STATION ); // 2014.08.21
				//
//				if ( ( side0 < 0 ) || ( side0 == s2 ) ) { // 2014.08.21 // 2016.03.23
					//
					p2 = _SCH_MODULE_Get_TM_POINTER( tms , TB_STATION );
					//
//					if ( _SCH_CLUSTER_Get_PathDo( side , p2 ) <= _SCH_CLUSTER_Get_PathRange( side , p2 ) ) { // 2014.08.21
					if ( _SCH_CLUSTER_Get_PathDo( s2 , p2 ) <= _SCH_CLUSTER_Get_PathRange( s2 , p2 ) ) { // 2014.08.21
						//
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							//
//							m = _SCH_CLUSTER_Get_PathProcessChamber( side , p2 , _SCH_CLUSTER_Get_PathDo( side , p2 ) - 1 , k ); // 2014.08.21
							m = _SCH_CLUSTER_Get_PathProcessChamber( s2 , p2 , _SCH_CLUSTER_Get_PathDo( s2 , p2 ) - 1 , k ); // 2014.08.21
							//
							if ( m == pm ) {
								//
//								if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( side , p2 , _SCH_CLUSTER_Get_PathDo( side , p2 ) - 1 , k ) ) { /* 2005.05.11 */ // 2014.08.21
								if ( _SCH_CLUSTER_Check_Already_Run_UsedPre( s2 , p2 , _SCH_CLUSTER_Get_PathDo( s2 , p2 ) - 1 , k ) ) { /* 2005.05.11 */ // 2014.08.21
									//
									r3 = TRUE; /* 2005.05.11 */
									r1 = FALSE;
								}  /* 2005.05.11 */
								//
								r2 = TRUE;
								o2 = k;
								break;
							}
						}
					}
					else {
						e2 = 4;
					}
//				} // 2016.03.23
//				else { // 2016.03.23
//					e2 = 3; // 2016.03.23
//				} // 2016.03.23
			}
			else {
				e2 = 2;
			}
		}
		else {
			e2 = 1;
		}
	}
	else {
		e2 = 5;
	}
	//
	if ( !r1 && !r2 ) {
		*ptr = ( e1 * 100 ) + ( e2 * 10 ) + 8;
		return -1;
	}
	//
	if ( r1 && r2 ) {
//		if ( p1 < p2 ) { // 2016.03.28
		if ( _SCH_CLUSTER_Get_SupplyID( s1,p1 ) < _SCH_CLUSTER_Get_SupplyID( s2,p2 ) ) { // 2016.03.28
			*order = o1;
			*ptr = p1; /* 2005.05.11 */
			//
			if ( ( side0 >= 0 ) && ( side0 != s1 ) ) { // 2016.03.23
				*ptr = ( e1 * 100 ) + ( e2 * 10 ) + 7;
				return -1;
			}
			//
			return 0;
		}
		else {
			*order = o2;
			*ptr = p2; /* 2005.05.11 */
			//
			if ( ( side0 >= 0 ) && ( side0 != s2 ) ) { // 2016.03.23
				*ptr = ( e1 * 100 ) + ( e2 * 10 ) + 6;
				return -1;
			}
			//
			return 1;
		}
	}
	else if ( r1 ) {
		*order = o1;
		*ptr = p1; /* 2005.05.11 */
		//
		if ( ( side0 >= 0 ) && ( side0 != s1 ) ) { // 2016.03.23
			*ptr = ( e1 * 100 ) + ( e2 * 10 ) + 5;
			return -1;
		}
		//
		return 0;
	}
	else if ( r2 ) {
		*order = o2;
		*ptr = p2; /* 2005.05.11 */
		//
		if ( ( side0 >= 0 ) && ( side0 != s2 ) ) { // 2016.03.23
			*ptr = ( e1 * 100 ) + ( e2 * 10 ) + 4;
			return -1;
		}
		//
		return 1;
	}
	//
	*ptr = ( e1 * 100 ) + ( e2 * 10 ) + 9;
	//
	return -1;
}


int SCHEDULER_CONTROL_Chk_RUN_HOLD_WAIT_FOR_6( int tms , int side ) {
	int i;
	if ( FM_Pick_Running_Blocking_Style_6 ) return FALSE; /* 2006.08.02 */
	if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + tms ) != BUFFER_FM_STATION ) return FALSE;
	if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_6( BM1 + tms ) ) return FALSE;
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + tms ) > 0 ) return FALSE;
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( FM , FALSE ) == SYS_RUNNING ) return FALSE;
	if ( _SCH_EQ_INTERFACE_FUNCTION_STATUS( BM1 + tms , FALSE ) == SYS_RUNNING ) return FALSE;
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return FALSE;
	if ( FM_Pick_Running_Blocking_Style_6 ) return FALSE; /* 2006.08.02 */
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) {
		if ( FM_Pick_Running_Blocking_Style_6 ) return FALSE; /* 2006.08.02 */
		return TRUE;
	}
	if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) {
		if ( FM_Pick_Running_Blocking_Style_6 ) return FALSE; /* 2006.08.02 */
		return TRUE;
	}
	for ( i = PM1 ; i < AL ; i++ ) {
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , side ) ) {
				if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) {
					if ( FM_Pick_Running_Blocking_Style_6 ) return FALSE; /* 2006.08.02 */
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int SCHEDULER_Get_ArmIntlks_PLACE_BM_FOR_6( int tms ) { // 2014.07.18
	//
	switch( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms , BM1 + tms ) ) {
	case 1 :
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + tms + 2 , FALSE , -1 ) && !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + tms + 2 , 0 ) ) {
			//
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms , BM1 + tms + 2 ) == 1 ) return 1;
			//
			return 0;
		}
		else {
			return 1;
		}
		//
		break;
	case 2 :
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( BM1 + tms + 2 , FALSE , -1 ) && !SCHEDULING_ThisIs_BM_OtherChamber6( BM1 + tms + 2 , 0 ) ) {
			//
			if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( tms , BM1 + tms + 2 ) == 2 ) return 2;
			//
			return 0;
		}
		else {
			return 2;
		}
		//
		break;
	}
	//
	return 0;
	//
}

BOOL SCHEDULER_CONTROL_Check_ArmIntlks_TM_PICK_FROM_PM_FOR_6( int tms , int pmc , int *arm ) {
	int armmode;
	//
//	switch( SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( tms , BM1 + tms , _SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( pmc ) , _SCH_MODULE_Get_PM_SIDE( pmc , 0 ) , _SCH_MODULE_Get_PM_POINTER( pmc , 0 ) , &armmode ) ) { // 2014.07.18
//	switch( SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( tms , _SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( pmc ) , _SCH_MODULE_Get_PM_SIDE( pmc , 0 ) , _SCH_MODULE_Get_PM_POINTER( pmc , 0 ) , &armmode ) ) { // 2014.07.18 // 2014.07.31
	switch( SCHEDULER_CONTROL_Chk_Next_PM_is_ArmIntlks_OK_FOR_6( tms , _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms , pmc ) , _SCH_MODULE_Get_PM_SIDE( pmc , 0 ) , _SCH_MODULE_Get_PM_POINTER( pmc , 0 ) , &armmode ) ) { // 2014.07.18 // 2014.07.31
	case -1 :
/*
		// 2014.07.18
		//
		switch( _SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( pmc ) ) {
		case 0 :
			switch( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( BM1 + tms ) ) {
			case 0 :
				*arm = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , pmc );
				break;
			case 1 :
				if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return FALSE;
				*arm = TA_STATION;
				break;
			case 2 :
				if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return FALSE;
				*arm = TB_STATION;
				break;
			}
			return TRUE;
			break;
		case 1 :
			if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return FALSE;
			switch( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( BM1 + tms ) ) {
			case 0 :
			case 1 :
				*arm = TA_STATION;
				break;
			case 2 :
				return FALSE;
				break;
			}
			return TRUE;
			break;
		case 2 :
			if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return FALSE;
			switch( _SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( BM1 + tms ) ) {
			case 0 :
			case 2 :
				*arm = TB_STATION;
				break;
			case 1 :
				return FALSE;
				break;
			}
			return TRUE;
			break;
		}
		*/
		//
		// 2014.07.18
		//
//		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( pmc , tms ) ) { // 2014.07.31
		switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tms , pmc ) ) { // 2014.07.31
		case 0 :
			switch( SCHEDULER_Get_ArmIntlks_PLACE_BM_FOR_6( tms ) ) {
			case 0 :
				*arm = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , pmc );
				break;
			case 1 :
				if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return FALSE;
				*arm = TA_STATION;
				break;
			case 2 :
				if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return FALSE;
				*arm = TB_STATION;
				break;
			}
			return TRUE;
			break;
		case 1 :
			//
			if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return FALSE;
			//
			switch( SCHEDULER_Get_ArmIntlks_PLACE_BM_FOR_6( tms ) ) {
			case 0 :
			case 1 :
				*arm = TA_STATION;
				break;
			case 2 :
				return FALSE;
				break;
			}
			return TRUE;
			break;
		case 2 :
			//
			if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return FALSE;
			//
			switch( SCHEDULER_Get_ArmIntlks_PLACE_BM_FOR_6( tms ) ) {
			case 0 :
			case 2 :
				*arm = TB_STATION;
				break;
			case 1 :
				return FALSE;
				break;
			}
			return TRUE;
			break;
		}
		//

		break;

	case 1 :
		if      ( armmode == 0 ) {
			*arm = _SCH_ROBOT_GET_FAST_PICK_FINGER( tms , pmc );
		}
		else if ( armmode == 1 ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tms , TA_STATION ) > 0 ) return FALSE;
			*arm = TA_STATION;
		}
		else if ( armmode == 2 ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tms , TB_STATION ) > 0 ) return FALSE;
			*arm = TB_STATION;
		}
		return TRUE;
		break;
	case 0 :
		return FALSE;
		break;
	}
	return FALSE;
}



int SCHEDULER_CONTROL_Check_TM_AND_PM_WAITING_FOR_NOWAFER_PRCS( int side , int tmside , int bmch , BOOL *hasin ) { // 2007.10.02
	int rc , ch;
	BOOL Alltmout; //2013.11.15

	*hasin = FALSE; // 2013.10.30
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return FALSE;
	//
	if ( ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) <= 0 ) ) return FALSE;
	//
	if ( !SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( bmch , TRUE ) ) return FALSE;
	//
	if ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() != 2 ) { // 2013.10.30
		if ( !_SCH_MODULE_Chk_FM_Finish_Status( side ) ) return FALSE; // 2013.10.30
	}
	//
//	else { // 2013.10.30 // 2013.11.15
		if ( !SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_Lock_FOR_6( tmside , &Alltmout ) ) {
			//
			if ( Alltmout ) { // 2013.11.15
				if ( SCHEDULER_CONTROL_Chk_BM_has_all_Out_FOR_6( bmch ) ) return TRUE; // 2013.11.15
			}
			//
//			return FALSE; // 2013.11.15
			//
			if ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() == 2 ) return FALSE; // 2013.11.15
			//
		}
//	} // 2013.11.15
	//
//	if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_curr_out_last_one_FOR_6( side , bmch ) ) return FALSE; // 2013.10.30
	if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_curr_out_last_one_FOR_6( side , bmch , hasin ) ) return FALSE; // 2013.10.30
	//
//	if ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) {
//		if ( _SCH_MODULE_Get_TM_SIDE( tmside , TA_STATION ) != side ) return FALSE;
//	}
//	if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) {
//		if ( _SCH_MODULE_Get_TM_SIDE( tmside , TB_STATION ) != side ) return FALSE;
//	}
	//
	rc = 0;
	//
	for ( ch = PM1 ; ch < AL ; ch++ ) {
		if ( _SCH_PRM_GET_MODULE_GROUP( ch ) != tmside ) continue;
		if ( !_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , side ) ) continue;
		if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) > 0 ) { // 2008.01.21
			if ( _SCH_MODULE_Get_PM_SIDE( ch , 0 ) == side ) return FALSE; // 2008.01.21
		} // 2008.01.21
		else { // 2008.01.21
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 ) rc++; // 2008.01.21
		} // 2008.01.21
	}
	if ( rc == 0 ) return FALSE;
	//
	return TRUE;
}


//int SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( int tms , int side , int *SCH_Chamber , int *SCH_No ) { // 2014.10.04
int SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( int tms , int *r_side , int *SCH_Chamber , int *SCH_No ) { // 2014.10.04
	int armcount , ch , arm;
	int x; // 2015.02.17
	//
	armcount = _SCH_ROBOT_GET_FINGER_FREE_COUNT( tms );
	if ( armcount <= 0 ) return 1;
	//
	*SCH_Chamber = 0;
	//
	for ( x = 0 ; x < 2 ; x++ ) {
		//
		for ( ch = PM1 ; ch < AL ; ch++ ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( ch ) != tms ) continue;
	//		if ( !_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , side ) ) continue; // 2014.10.04
			if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) continue;
	//		if ( _SCH_MODULE_Get_PM_SIDE( ch , 0 ) != side ) continue; // 2014.10.04
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , _SCH_MODULE_Get_PM_SIDE( ch , 0 ) ) ) continue; // 2014.10.04
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) > 0 )  continue;
			//
			if ( x == 0 ) { // 2015.02.17
				if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , _SCH_MODULE_Get_PM_SIDE( ch , 0 ) ) ) continue;
			}
			//
//			if ( !SCHEDULER_CONTROL_Chk_Next_PM_Go_Impossible_FOR_6( tms , ch , armcount ) ) { // 2015.02.17
			if ( !SCHEDULER_CONTROL_Chk_Next_PM_Go_Impossible_FOR_6( tms , ch , armcount , x ) ) { // 2015.02.17
				//
				if ( SCHEDULER_CONTROL_Check_ArmIntlks_TM_PICK_FROM_PM_FOR_6( tms , ch , &arm ) ) {
					if ( *SCH_Chamber == 0 ) {
						*SCH_Chamber = ch;
						*SCH_No = arm;
						*r_side = _SCH_MODULE_Get_PM_SIDE( ch , 0 ); // 2014.10.04
					}
					else {
						if      ( _SCH_PRM_GET_Getting_Priority( *SCH_Chamber ) > _SCH_PRM_GET_Getting_Priority( ch ) ) {
							*SCH_Chamber = ch;
							*SCH_No = arm;
							*r_side = _SCH_MODULE_Get_PM_SIDE( ch , 0 ); // 2014.10.04
						}
						else if ( _SCH_PRM_GET_Getting_Priority( *SCH_Chamber ) == _SCH_PRM_GET_Getting_Priority( ch ) ) {
	//						if ( _SCH_MODULE_Get_PM_POINTER( *SCH_Chamber , 0 ) > _SCH_MODULE_Get_PM_POINTER( ch , 0 ) ) { // 2014.10.04
							if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_PM_SIDE( *SCH_Chamber , 0 ) , _SCH_MODULE_Get_PM_POINTER( *SCH_Chamber , 0 ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_PM_SIDE( ch , 0 ) , _SCH_MODULE_Get_PM_POINTER( ch , 0 ) ) ) { // 2014.10.04
								*SCH_Chamber = ch;
								*SCH_No = arm;
								*r_side = _SCH_MODULE_Get_PM_SIDE( ch , 0 ); // 2014.10.04
							}
						}
					}
				}
			}
		}
		//
		if ( *SCH_Chamber != 0 ) break; // 2015.02.17
		//
	}
	//
	if ( *SCH_Chamber == 0 ) return 2;
	//
	return 0;
}

//int SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_SUB( int tmside , int side , int bmindex , int *SCH_Chamber , int *SCH_Slot , int skipch , int skipslot , int *cf , int *swtag , BOOL sideCheck ) { // 2014.08.22
int SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_SUB( int tmside , int *rside , int bmindex , int *SCH_Chamber , int *SCH_Slot , int skipch , int skipslot , int *cf , int *swtag , BOOL sideCheck ) { // 2014.08.22
	int chkcount , ech , sch , SCH_Ptr , SCH_NotDis , intarm;
	BOOL dualfree; // 2014.07.25
	//
	*SCH_Chamber = BM1 + tmside + bmindex;

	*SCH_Slot = 0; /* 2014.05.23 */

	if ( !_SCH_MODULE_GET_USE_ENABLE( *SCH_Chamber , FALSE , -1 ) ) return 21; // 2014.07.08
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( *SCH_Chamber , 0 ) ) return 22; // 2014.07.08

	if ( !SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( *SCH_Chamber , TRUE ) ) return 1;
	/* *SCH_Slot = SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( side , *SCH_Chamber , TRUE ); */ /* 2014.05.23 */
//	*SCH_Slot = SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( side , *SCH_Chamber , skipch , skipslot , sideCheck ); /* 2014.05.23 */ // 2014.08.22
	*SCH_Slot = SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( rside , *SCH_Chamber , skipch , skipslot , sideCheck ); /* 2014.05.23 */ // 2014.08.22
	if ( *SCH_Slot <= 0 ) return 2;

	if ( SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_OtherSlot_first_FOR_6( tmside , *SCH_Chamber , *SCH_Slot ) ) return 22; /* 2014.06.18 */

//	if ( SCHEDULER_CONTROL_Chk_Will_Do_PM_POST_FOR_6( side , tmside , *SCH_Chamber , *SCH_Slot ) ) return 3; // 2014.08.22
	if ( SCHEDULER_CONTROL_Chk_Will_Do_PM_POST_FOR_6( *rside , tmside , *SCH_Chamber , *SCH_Slot ) ) return 3; // 2014.08.22

	chkcount = _SCH_ROBOT_GET_FINGER_FREE_COUNT( tmside );
	if ( chkcount <= 0 ) return 4; /* 2007.07.26 */
	//
	*swtag = 0;
	//
	dualfree = ( chkcount >= 2 ); // 2014.07.25
	//
	if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TB_STATION ) ) {
		//
		if ( !SCHEDULER_CONTROL_Chk_BMs_Target_PM_is_ArmIntlks_OK_FOR_6( tmside , *SCH_Chamber , *SCH_Slot , &intarm ) ) return 5; /* 2007.07.26 */
		//
		if      ( intarm == 1 ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) return 6;
			chkcount = 0;
		}
		else if ( intarm == 2 ) {
			if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) return 7;
			chkcount = 1;
		}
		else {
			//
			switch( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( tmside , *SCH_Chamber ) ) { // 2014.07.18
			case 1 :
				if ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) return 6;
				chkcount = 0;
				break;
			case 2 :
				if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) return 7;
				chkcount = 1;
				break;
			default :
				//
				if ( chkcount >= 2 ) {
					//
					chkcount = 2;
					//
					if ( SCHEDULER_CONTROL_Chk_exist_run_wafer_at_PM_FOR_6( *SCH_Chamber ) ) *swtag = 1;
					//
				}
				else if ( chkcount == 1 ) {
					if ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) {
						chkcount = 1;
					}
					else if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) {
						chkcount = 0;
					}
				}
				break;
			}
			//
		}
		//
		if ( chkcount < 2 ) {
			if ( !SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( *SCH_Chamber ) ) {
				if ( _SCH_MODULE_Get_TM_WAFER( tmside , TA_STATION ) > 0 ) {
					ech = _SCH_MODULE_Get_TM_SIDE( tmside , TA_STATION );
					sch = _SCH_MODULE_Get_TM_POINTER( tmside , TA_STATION );
					if ( _SCH_CLUSTER_Get_PathDo( ech , sch ) > _SCH_CLUSTER_Get_PathRange( ech , sch ) ) {
						*swtag = 1;
					}
					else {
						if ( !SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( *SCH_Chamber ) ) return 8;
					}
				}
				if ( _SCH_MODULE_Get_TM_WAFER( tmside , TB_STATION ) > 0 ) {
					ech = _SCH_MODULE_Get_TM_SIDE( tmside , TB_STATION );
					sch = _SCH_MODULE_Get_TM_POINTER( tmside , TB_STATION );
					if ( _SCH_CLUSTER_Get_PathDo( ech , sch ) > _SCH_CLUSTER_Get_PathRange( ech , sch ) ) {
						*swtag = 1;
					}
					else {
						if ( !SCHEDULER_CONTROL_Chk_BALANCE_BM_SLOT_MODE_FOR_6( *SCH_Chamber ) ) return 9;
					}
				}
			}
		}
	}
	else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TA_STATION ) ) {
		chkcount = 0;
	}
	else if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TB_STATION ) ) {
		chkcount = 1;
	}
	else {
		return 10;
	}
	//
	if ( chkcount >= 2 ) {
		*cf = _SCH_ROBOT_GET_FAST_PICK_FINGER( tmside , *SCH_Chamber );
		return 0;
	}
	else { // 2014.07.25
		if ( dualfree ) {
			*cf = chkcount;
			return 0;
		}
	}
	//
	if ( !SCHEDULER_CONTROL_Chk_BMs_Target_PM_is_free_FOR_6( *SCH_Chamber , *SCH_Slot , &SCH_Ptr , &SCH_NotDis ) ) {
		if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( tmside , TB_STATION ) ) {
			if ( chkcount == 0 ) {
				ech = _SCH_MODULE_Get_TM_SIDE( tmside , TB_STATION );
				sch = _SCH_MODULE_Get_TM_POINTER( tmside , TB_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( ech , sch ) > _SCH_CLUSTER_Get_PathRange( ech , sch ) ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_one_out_full_FOR_6( *SCH_Chamber ) ) {
						return 11;
					}
					else {
						*swtag = 0;
					}
				}
				else {
					if ( SCH_NotDis ) return 12;
				}
			}
			else {
				ech = _SCH_MODULE_Get_TM_SIDE( tmside , TA_STATION );
				sch = _SCH_MODULE_Get_TM_POINTER( tmside , TA_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( ech , sch ) > _SCH_CLUSTER_Get_PathRange( ech , sch ) ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_has_a_supply_one_out_full_FOR_6( *SCH_Chamber ) ) {
						return 13;
					}
					else {
						*swtag = 0;
					}
				}
				else {
					if ( SCH_NotDis ) return 14;
				}
			}
		}
		else {
			if ( SCH_NotDis || SCHEDULER_CONTROL_Chk_full_getout_wafer_from_BM_FOR_6( *SCH_Chamber ) ) return 15;
		}
	}
	else {
		if ( SCH_NotDis ) {
			if ( !SCHEDULER_CONTROL_Chk_TMs_Target_PM_is_free_FOR_6( tmside , SCH_Ptr ) ) return 16;
		}
	}
	*cf = chkcount;
	return 0;
}

//int SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( int tmside , int side , int *SCH_Chamber , int *SCH_Slot , int skipch , int skipslot , int *cf , int *swtag , BOOL sideCheck ) { // 2014.07.08 // 2014.08.22
int SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( int tmside , int *rside , int *SCH_Chamber , int *SCH_Slot , int skipch , int skipslot , int *cf , int *swtag , BOOL sideCheck ) { // 2014.07.08 // 2014.08.22
	//
	int Res1 , SCH_Chamber1 , SCH_Slot1 , cf1 , swtag1 , side1;
	int Res2 , SCH_Chamber2 , SCH_Slot2 , cf2 , swtag2 , side2;
	//
//	Res1 = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_SUB( tmside , side , 0 , &SCH_Chamber1 , &SCH_Slot1 , skipch , skipslot , &cf1 , &swtag1 , sideCheck ); // 2014.08.22
//	Res2 = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_SUB( tmside , side , 2 , &SCH_Chamber2 , &SCH_Slot2 , skipch , skipslot , &cf2 , &swtag2 , sideCheck ); // 2014.08.22
	//
	side1 = *rside;
	side2 = *rside;
	//
	Res1 = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_SUB( tmside , &side1 , 0 , &SCH_Chamber1 , &SCH_Slot1 , skipch , skipslot , &cf1 , &swtag1 , sideCheck ); // 2014.08.22
	Res2 = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_SUB( tmside , &side2 , 2 , &SCH_Chamber2 , &SCH_Slot2 , skipch , skipslot , &cf2 , &swtag2 , sideCheck ); // 2014.08.22
	//
	if ( ( Res1 == 0 ) && ( Res2 == 0 ) ) {
		//
		if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( SCH_Chamber1 , SCH_Slot1 ) , _SCH_MODULE_Get_BM_POINTER( SCH_Chamber1 , SCH_Slot1 ) ) > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( SCH_Chamber2 , SCH_Slot2 ) , _SCH_MODULE_Get_BM_POINTER( SCH_Chamber2 , SCH_Slot2 ) ) ) {
			//
			Res1 = 31;
			//
		}
		//
	}
	//
	if ( Res1 == 0 ) {
		*rside = side1; // 2014.08.22
		*SCH_Chamber = SCH_Chamber1;
		*SCH_Slot = SCH_Slot1;
		*cf = cf1;
		*swtag = swtag1;
		return 0;
	}
	//
	if ( Res2 == 0 ) {
		*rside = side2; // 2014.08.22
		*SCH_Chamber = SCH_Chamber2;
		*SCH_Slot = SCH_Slot2;
		*cf = cf2;
		*swtag = swtag2;
		return 0;
	}
	//
	// 2014.09.26
	//
	*rside = side1 + ( side2 * 1000 );
	*SCH_Chamber = SCH_Chamber1 + ( SCH_Chamber2 * 1000 );
	*SCH_Slot = SCH_Slot1 + ( SCH_Slot2 * 1000 );
	*cf = cf1 + ( cf2 * 1000 );
	*swtag = swtag1 + ( swtag2 * 1000 );
	//
	return Res1 + ( Res2 * 1000 );
}

/*
// 2014.08.21
int SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( int tms , int side , int *SCH_Chamber , int *SCH_No , int *SCH_Order ) {
	int cf , ch , ord , pt , SCH_Ptr;
	*SCH_Chamber = -1;
	//
	for ( ch = PM1 ; ch < AL ; ch++ ) {
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( ch ) == tms ) {
			//
			if ( ( _SCH_MODULE_Get_TM_Switch_Signal( tms ) == -1 ) ||
				( _SCH_MODULE_Get_TM_Switch_Signal( tms ) == ch ) ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , side ) ) {
					//
					cf = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( tms , side , ch , 0 , &ord , &pt );
					//
					if ( cf < 0 ) continue;
					//
					if ( _SCH_MODULE_Get_TM_SIDE( tms , cf ) != side ) {
						continue;
					}
					else {
						if ( _SCH_CLUSTER_Get_PathDo( side , _SCH_MODULE_Get_TM_POINTER( tms , cf ) ) > _SCH_CLUSTER_Get_PathRange( side , _SCH_MODULE_Get_TM_POINTER( tms , cf ) ) ) {
							continue;
						}
					}
					if ( *SCH_Chamber == -1 ) {
						*SCH_Chamber = ch;
						*SCH_No = cf;
						*SCH_Order = ord;
						SCH_Ptr = pt;
					}
					else {
						if ( pt < SCH_Ptr ) {
							*SCH_Chamber = ch;
							*SCH_No = cf;
							*SCH_Order = ord;
							SCH_Ptr = pt;
						}
					}
				}
			}
		}
	}
	if ( *SCH_Chamber != -1 ) return 0;
	return 1;
}
*/

// 2014.08.21
int SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( int tms , int *SCH_SIDE , int *SCH_Chamber , int *SCH_No , int *SCH_Order ) {
	int cf , ch , ord , pt , SCH_Ptr , s;
	int sel1 , sel2 , sel3 , sk;
	//
	*SCH_Chamber = -1;
	//
	sel1 = -1;
	sel2 = -1;
	sel3 = -1;
	//
	for ( ch = PM1 ; ch < AL ; ch++ ) {
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( ch ) == tms ) {
			//
			if ( ( _SCH_MODULE_Get_TM_Switch_Signal( tms ) == -1 ) ||
				( _SCH_MODULE_Get_TM_Switch_Signal( tms ) == ch ) ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( ch , FALSE , -1 ) ) {
					//
					cf = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( tms , -1 , ch , 0 , &ord , &pt );
					//
					if ( cf < 0 ) {
						//
						if      ( sel1 == -1 ) {
							sel1 = 100000 + pt;
						}
						else if ( sel2 == -1 ) {
							sel2 = 100000 + pt;
						}
						else if ( sel3 == -1 ) {
							sel3 = 100000 + pt;
						}
						//
						continue;
					}
					//
					sk = 0;
					//
					s = _SCH_MODULE_Get_TM_SIDE( tms , cf );
					//
					if ( !_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , s ) ) {
						sk = 1;
					}
					else {
						if ( _SCH_CLUSTER_Get_PathDo( s , _SCH_MODULE_Get_TM_POINTER( tms , cf ) ) > _SCH_CLUSTER_Get_PathRange( s , _SCH_MODULE_Get_TM_POINTER( tms , cf ) ) ) {
							sk = 2;
						}
					}
					//
					if ( sk != 0 ) {
						//
						if      ( sel1 == -1 ) {
							sel1 = ( sk * 1000 ) + ( ch * 10 ) + cf;
						}
						else if ( sel2 == -1 ) {
							sel2 = ( sk * 1000 ) + ( ch * 10 ) + cf;
						}
						else if ( sel3 == -1 ) {
							sel3 = ( sk * 1000 ) + ( ch * 10 ) + cf;
						}
						//
						continue;
					}
					//
					if ( *SCH_Chamber == -1 ) {
						*SCH_SIDE = s;
						*SCH_Chamber = ch;
						*SCH_No = cf;
						*SCH_Order = ord;
						SCH_Ptr = pt;
					}
					else {
						if ( pt < SCH_Ptr ) {
							*SCH_SIDE = s;
							*SCH_Chamber = ch;
							*SCH_No = cf;
							*SCH_Order = ord;
							SCH_Ptr = pt;
						}
					}
				}
			}
		}
	}
	if ( *SCH_Chamber != -1 ) return 0;
	//
	*SCH_SIDE    = _SCH_MODULE_Get_TM_Switch_Signal( tms );
	*SCH_Chamber = sel1;
	*SCH_No      = sel2;
	*SCH_Order   = sel3;
	//
	return 1;
}

void SCHEDULER_CONTROL_Check_BEFORE_PLACE_TO_BM_PRE_END_FOR_6( int tms , int CHECKING_SIDE , int pmc , int arm0 ) {
	int arm , ch;
	int s; // 2014.08.22
	/* if ( _SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() < 1 ) return; */ /* 2008.07.29 */
	if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) < 2 ) return;
	//
//	if ( SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( CHECKING_SIDE , tms + BM1 , 0 , 0 , FALSE ) > 0 ) return; // 2014.08.22
	//
	s = CHECKING_SIDE;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_FOR_6( &s , tms + BM1 , 0 , 0 , FALSE ) > 0 ) return; // 2014.08.22
	//
	if ( s != CHECKING_SIDE ) return; // 2014.08.22
	//
	if ( arm0 == TA_STATION ) arm = TB_STATION;
	else                      arm = TA_STATION;
	if ( pmc != -1 ) {
		if ( ( _SCH_MODULE_Get_TM_SIDE( tms , arm0 ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_TM_WAFER( tms , arm0 ) > 0 ) ) {
			if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_TM_POINTER( tms , arm0 ) ) <= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , _SCH_MODULE_Get_TM_POINTER( tms , arm0 ) ) ) {
				return;
			}
		}
	}
	if ( ( _SCH_MODULE_Get_TM_SIDE( tms , arm ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_TM_WAFER( tms , arm ) > 0 ) ) {
		if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_TM_POINTER( tms , arm ) ) <= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , _SCH_MODULE_Get_TM_POINTER( tms , arm ) ) ) {
			return;
		}
	}
	for ( ch = PM1 ; ch < AL ; ch++ ) {
		if ( ( pmc == -1 ) || ( pmc == ch ) ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( ch ) == tms ) {
				if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , ch ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , ch ) >= MUF_90_USE_to_XDEC_FROM ) ) {
					if ( ( _SCH_MODULE_Get_Mdl_Run_Flag( ch ) > 1 ) || ( ( _SCH_MODULE_Get_Mdl_Run_Flag( ch ) <= 1 ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) <= 0 ) ) ) {
						if ( _SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) {
							if ( _SCH_PREPOST_Pre_End_Part( CHECKING_SIDE , ch ) ) {
							}
						}
					}
				}
			}
		}
	}
}


/*
// 2014.07.17
int SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( int tms , int side , int *SCH_Chamber , int *SCH_Slot , int *SCH_No , int *SCH_Return ) {
	*SCH_Chamber = BM1 + tms;
	if ( !SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( *SCH_Chamber , TRUE ) ) return 1;
	*SCH_No = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( side , tms , *SCH_Chamber , SCH_Slot , SCH_Return , FALSE );
	if ( *SCH_No >= 0 ) {
		return 0;
	}
	return 2;
}
*/

// 2014.07.17
int SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( BOOL in , int ch ) {
	//
	int i , supid = -1;
	//
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 )  continue;
		//
		if ( ( in ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_INWAIT_STATUS ) ) continue;
		if ( ( !in ) && ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_OUTWAIT_STATUS ) ) continue;
		//
		if ( supid == -1 ) {
			//
			supid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
			//
		}
		else {
			//
			if ( supid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) ) {
				//
				supid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) );
				//
			}
		}
	}
	return supid;
}

/*
//
// 2015.02.13
//
// 2014.07.17
int SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( int tms , int side , int *SCH_Chamber , int *SCH_Slot , int *SCH_No , int *SCH_Return ) {
	int bm1 , bm2;
	int slot1 , slot2;
	int Arm1 , Arm2;
	int r1 , r2;
	int ic1 , oc1 , ic2 , oc2;
	int sid1 , sid2;
	//
	bm1 = BM1 + tms + 0;
	bm2 = BM1 + tms + 2;
	Arm1 = -1;
	Arm2 = -1;
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( bm1 , FALSE , -1 ) ) {
		if ( !SCHEDULING_ThisIs_BM_OtherChamber6( bm1 , 0 ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( bm1 , TRUE ) ) {
				Arm1 = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( side , tms , bm1 , &slot1 , &r1, FALSE );
			}
		}
	}
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( bm2 , FALSE , -1 ) ) {
		if ( !SCHEDULING_ThisIs_BM_OtherChamber6( bm2 , 0 ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( bm2 , TRUE ) ) {
				Arm2 = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm_FOR_6( side , tms , bm2 , &slot2 , &r2, FALSE );
			}
		}
	}
	//
	if ( ( Arm1 >= 0 ) && ( Arm2 >= 0 ) ) {
		//
		ic1 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm1 , BUFFER_INWAIT_STATUS );
		oc1 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm1 , BUFFER_OUTWAIT_STATUS );
		//
		ic2 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm2 , BUFFER_INWAIT_STATUS );
		oc2 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm2 , BUFFER_OUTWAIT_STATUS );
		//
		if      ( oc1 > oc2 ) {
			Arm2 = -1;
		}
		else if ( oc1 < oc2 ) {
			Arm1 = -1;
		}
		else {
			if      ( ( ic1 > 0 ) && ( ic2 == 0 ) ) {
				Arm2 = -1;
			}
			else if ( ( ic1 == 0 ) && ( ic2 > 0 ) ) {
				Arm1 = -1;
			}
			else if ( ( ic1 == 0 ) && ( ic2 == 0 ) ) {
				if ( oc1 > 0 ) {
					//
					sid1 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( FALSE , bm1 );
					sid2 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( FALSE , bm2 );
					//
					if ( sid1 > sid2 ) Arm1 = -1;
					//
				}
			}
			else {
				//
				sid1 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( TRUE, bm1 );
				sid2 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( TRUE, bm2 );
				//
				if ( sid1 > sid2 ) Arm1 = -1;
				//
			}
		}
		//
	}
	//
	if ( Arm1 >= 0 ) {
		*SCH_Chamber = bm1;
		*SCH_Slot = slot1;
		*SCH_No = Arm1;
		*SCH_Return = r1;
		return 0;
	}
	//
	if ( Arm2 >= 0 ) {
		*SCH_Chamber = bm2;
		*SCH_Slot = slot2;
		*SCH_No = Arm2;
		*SCH_Return = r2;
		return 0;
	}
	return 2;
	//
}
*/
//

//
// 2015.02.13
//
int SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( int tms , int *SCH_SIDE , int *SCH_Chamber , int *SCH_Slot , int *SCH_No , int *SCH_Return ) {
	int bm1 , bm2;
	int slot1 , slot2;
	int Arm1 , Arm2;
	int r1 , r2;
	int ic1 , oc1 , ic2 , oc2;
	int supid1 , supid2;
	int side1 , side2;
	//
	bm1 = BM1 + tms + 0;
	bm2 = BM1 + tms + 2;
	Arm1 = -1;
	Arm2 = -1;
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( bm1 , FALSE , -1 ) ) {
		if ( !SCHEDULING_ThisIs_BM_OtherChamber6( bm1 , 0 ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( bm1 , TRUE ) ) {
				Arm1 = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm2_FOR_6( &side1 , tms , bm1 , &slot1 , &r1, FALSE );
			}
		}
	}
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( bm2 , FALSE , -1 ) ) {
		if ( !SCHEDULING_ThisIs_BM_OtherChamber6( bm2 , 0 ) ) {
			if ( SCHEDULER_CONTROL_Chk_BM_is_Pump_Status_for_FOR_6( bm2 , TRUE ) ) {
				Arm2 = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_bm2_FOR_6( &side2 , tms , bm2 , &slot2 , &r2, FALSE );
			}
		}
	}
	//
	if ( ( Arm1 >= 0 ) && ( Arm2 >= 0 ) ) {
		//
		ic1 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm1 , BUFFER_INWAIT_STATUS );
		oc1 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm1 , BUFFER_OUTWAIT_STATUS );
		//
		ic2 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm2 , BUFFER_INWAIT_STATUS );
		oc2 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm2 , BUFFER_OUTWAIT_STATUS );
		//
		if      ( oc1 > oc2 ) {
			Arm2 = -1;
		}
		else if ( oc1 < oc2 ) {
			Arm1 = -1;
		}
		else {
			if      ( ( ic1 > 0 ) && ( ic2 == 0 ) ) {
				Arm2 = -1;
			}
			else if ( ( ic1 == 0 ) && ( ic2 > 0 ) ) {
				Arm1 = -1;
			}
			else if ( ( ic1 == 0 ) && ( ic2 == 0 ) ) {
				if ( oc1 > 0 ) {
					//
					supid1 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( FALSE , bm1 );
					supid2 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( FALSE , bm2 );
					//
					if ( supid1 > supid2 ) Arm1 = -1;
					//
				}
			}
			else {
				//
				supid1 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( TRUE, bm1 );
				supid2 = SCHEDULER_CONTROL_Check_BM_INOUT_SUPPLYID_FOR_6( TRUE, bm2 );
				//
				if ( supid1 > supid2 ) Arm1 = -1;
				//
			}
		}
		//
	}
	//
	if ( Arm1 >= 0 ) {
		*SCH_SIDE = side1;
		*SCH_Chamber = bm1;
		*SCH_Slot = slot1;
		*SCH_No = Arm1;
		*SCH_Return = r1;
		return 0;
	}
	//
	if ( Arm2 >= 0 ) {
		*SCH_SIDE = side2;
		*SCH_Chamber = bm2;
		*SCH_Slot = slot2;
		*SCH_No = Arm2;
		*SCH_Return = r2;
		return 0;
	}
	return 2;
	//
}
//
//=====================================================================================================================================================================
//=====================================================================================================================================================================
// LOCK 1 :
//
//		CONDITION	:	BM이 Enable이고 TMSIDE이고 PROCESSING이 아닐때
//
//						BM에 OUT Wafer가 있을때
//
//						TM이 Wafer를 하나도 가지고 있지 않고 BM에 IN이 있을 경우 제외(2015.03.20)
//
//						TM이 회수할 Wafer를 가지고 있지 않을때
//
//						TM이 공급할 Wafer를 가지고 있을때 이 Wafer가 갈 PM이 [Check Time] 이상으로 Process 시간이 남아 있을때
//
//							또는 TM이 공급할 Wafer를 가지고 있지 않을때 해당 TM의 모든 PM이 [Check Time] 이상으로 Process 시간이 남아 있을때
//
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//int SCHEDULER_MAKE_LOCK_FREE_for_6( int side , int tms ) { // 2014.01.27
int SCHEDULER_MAKE_LOCK_FREE_for_6_Sub( int side , int tms , int index ) { // 2014.01.27
	int i , s , p , k , m , f;
	int bmc;
	int Mode;
	int checktimesec , runtime;
	BOOL hasin;
	int rside , rpt; // 2014.06.11
	BOOL hastmwfr; // 2015.03.20
	//
	int hastmspace , bs , bp , tmgopm[MAX_CHAMBER]; // 2015.11.13
	int prerunside , ss , es; // 2015.12.15
	BOOL hasinspace; // 2015.12.15
	//
//	bmc = tms + BM1; // 2014.07.10
	bmc = tms + BM1 + index; // 2014.07.10
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return 1;
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc , FALSE , -1 ) ) return 2;
	if ( _SCH_MACRO_CHECK_PROCESSING( bmc ) > 0 ) return 3;
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( bmc , 0 ) ) return 4; // 2014.07.10
	//
	hasin = ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bmc , BUFFER_INWAIT_STATUS ) > 0 );
	//
	hasinspace = FALSE; // 2015.12.15
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bmc , BUFFER_OUTWAIT_STATUS ) <= 0 ) return 5;
	//
	// 2014.06.19
	//
	rside = -1;
	rpt = -1;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) {
				rside = _SCH_MODULE_Get_BM_SIDE( bmc , i );
				rpt = _SCH_MODULE_Get_BM_POINTER( bmc , i );
				break;
			}
		}
	}
	//
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) tmgopm[i] = 0; // 2015.11.13
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmc , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) {
			hasinspace = TRUE;
			break;
		}
	}
	//
	f = 0;
	//
	hastmspace = FALSE; // 2015.11.13
	//
	hastmwfr = FALSE; // 2015.03.20
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) <= 0 ) {
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,i ) ) hastmspace = TRUE; // 2015.11.13
			//
			continue;
		}
		//
		hastmwfr = TRUE; // 2015.03.20
		//
		s = _SCH_MODULE_Get_TM_SIDE( tms , i );
		p = _SCH_MODULE_Get_TM_POINTER( tms , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) > _SCH_CLUSTER_Get_PathRange( s , p ) ) return 11;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , k );
			//
			if ( m > 0 ) {
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) continue;
				//
				if ( !SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmc , m ) ) continue;
				//
				tmgopm[m] = 1; // 2015.11.13
				//
				checktimesec = _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( m );
				//
				if ( checktimesec <= 0 ) continue;
				//
				Mode = _SCH_MACRO_CHECK_PROCESSING_INFO( m );
				//
				if ( Mode <= 0 ) return 20;
				//
				f = 1;
				//
				if      ( Mode >= PROCESS_INDICATOR_POST ) {
					runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 1 , m );
//_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 21 [%d][%d][%d]\n" , m , runtime , checktimesec );
					if ( runtime < checktimesec ) return 21;
				}
				else if ( Mode >= PROCESS_INDICATOR_PRE ) {
					runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 2 , m );
//_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 22 [%d][%d][%d]\n" , m , runtime , checktimesec );
					if ( runtime < checktimesec ) return 22;
				}
				else {
					runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , m );
//_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 23 [%d][%d][%d]\n" , m , runtime , checktimesec );
					if ( runtime < checktimesec ) return 23;
				}
				//
			}
		}
	}
	//
	if ( !hastmwfr && hasin ) return 25; // 2015.03.20
	//
//	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 H [%d]\n" , f );
	//
	if ( f == 0 ) {
		//
		for ( i = PM1 ; i < AL ; i++ ) {
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != tms ) continue;
			if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
			//
			checktimesec = _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( i );
			//
			if ( checktimesec <= 0 ) continue;
			//
			Mode = _SCH_MACRO_CHECK_PROCESSING_INFO( i );
			//
			if ( Mode <= 0 ) continue;
			//
			f = 1;
			//
			if      ( Mode >= PROCESS_INDICATOR_POST ) {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 1 , i );
//_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 31 [%d][%d][%d]\n" , i , runtime , checktimesec );
				if ( runtime < checktimesec ) return 31;
			}
			else if ( Mode >= PROCESS_INDICATOR_PRE ) {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 2 , i );
//_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 32 [%d][%d][%d]\n" , i , runtime , checktimesec );
				if ( runtime < checktimesec ) return 32;
			}
			else {
				runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , i );
//_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 33 [%d][%d][%d]\n" , i , runtime , checktimesec );
				if ( runtime < checktimesec ) return 33;
			}
		}
	}
	//
	if ( f == 1 ) { // 2015.11.13
		//
		if ( hastmspace ) {
			//
			for ( i = _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i >= 1 ; i-- ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) continue;
				if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) continue;
				//
				bs = _SCH_MODULE_Get_BM_SIDE( bmc , i );
				bp = _SCH_MODULE_Get_BM_POINTER( bmc , i );
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					m = _SCH_CLUSTER_Get_PathProcessChamber( bs , bp , 0 , k );
					//
					if ( m > 0 ) {
						//
						if ( tmgopm[m] == 0 ) {
							//
							if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) {
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( m ) <= 0 ) {
									f = 0;
									break;
								}
								//
							}
							//
						}
						//
					}
					//
				}
				//
			}
			//
		}
	}
	//
	if ( f == 1 ) {
		//
//		if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( tms , bmc ); // 2014.01.28 // 2014.06.11
		if ( hasin ) {
			//
			if ( hasinspace ) { // 2015.12.15
				//
				m = SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_Other_CM_FOR_6( bmc , &prerunside );
				//
				if ( m == 0 ) {
					//
					if ( prerunside >= 0 ) { // 1771,1772,1773,1774,1775
						//
//						SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , prerunside , 0 ); // 2016.02.26
						SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , prerunside , bmc ); // 2016.02.26
						//
						Mode = 1771 + prerunside;
						//
					}
					else {
						//
						Mode = 1770;
						//
					}
					//
				}
				else {
					//
					hasinspace = FALSE;
					//
					Mode = 1780 + m;
					//
				}
			}
			else {
				//
				Mode = 1788;
				//
			}
			//--------------------------------------------------------------------------------------------------------------
			if ( !hasinspace ) { // 2015.12.15
				//
				if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) {
	//				SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( tms , bmc , rside , rpt ); // 2014.01.28 // 2014.06.11 // 2014.07.10
					SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( bmc , rside , rpt ); // 2014.01.28 // 2014.06.11 // 2014.07.10
				}
				else {
	//				SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( tms , bmc ); // 2014.01.28 // 2014.06.11 // 2014.07.10
					SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( bmc ); // 2014.01.28 // 2014.06.11 // 2014.07.10
				}
				//
			}
			//--------------------------------------------------------------------------------------------------------------
		}
		else {
			//
			Mode = 1789;
			//
		}
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , 1 , Mode );
	}
	//
	return 0;
}

int SCHEDULER_MAKE_LOCK_FREE_for_6( int side , int tms ) { // 2014.07.10
	if ( SCHEDULER_MAKE_LOCK_FREE_for_6_Sub( side , tms , 0 ) != 0 ) {
		return SCHEDULER_MAKE_LOCK_FREE_for_6_Sub( side , tms , 2 );
	}
	return 0;
}

//=====================================================================================================================================================================
//=====================================================================================================================================================================
// LOCK 2 :
//
//		CONDITION	:	BM이 Enable이고 TMSIDE이고 PROCESSING이 아닐때
//
//						BM에 OUT Wafer가 모두 차 있을때
//
//						BM이 공급할 Wafer가 갈 PM이 모두 차 있을때
//
//						TM이 회수할 Wafer만 가지고 있을때
//
//=====================================================================================================================================================================
//=====================================================================================================================================================================
//int SCHEDULER_MAKE_LOCK_FREE2_for_6( int side , int tms ) { // 2014.04.25 // 2014.07.10
int SCHEDULER_MAKE_LOCK_FREE2_for_6_Sub( int side , int tms , int index ) { // 2014.04.25 // 2014.07.10
	int i , s , p , k , m;
	int bmc;
	BOOL hasin , hasimp , poss;
	int ss , es;
	int rside , rpt; // 2014.06.11
	//
//	bmc = tms + BM1; // 2014.07.10
	bmc = tms + BM1 + index; // 2014.07.10
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return 1;
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc , FALSE , -1 ) ) return 2;
	if ( _SCH_MACRO_CHECK_PROCESSING( bmc ) > 0 ) return 3;
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( bmc , 0 ) ) return 4; // 2014.07.10
	//
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PLACE , bmc , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) return 4;
	}
	//
	// 2014.06.19
	//
	rside = -1;
	rpt = -1;
	//
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) {
				rside = _SCH_MODULE_Get_BM_SIDE( bmc , i );
				rpt = _SCH_MODULE_Get_BM_POINTER( bmc , i );
				break;
			}
		}
	}
	//
	//
	hasin = FALSE;
	hasimp = FALSE;
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmc , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) continue;
		//
		hasin = TRUE;
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmc , i );
		p = _SCH_MODULE_Get_BM_POINTER( bmc , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) != 0 ) continue;
		//
		poss = FALSE;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , k );
			//
			if ( m > 0 ) {
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) continue;
				//
				if ( !SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmc , m ) ) continue;
				//
				if ( _SCH_MODULE_Get_PM_WAFER( m , 0 ) <= 0 ) {
					poss = TRUE;
					break;
				}
				//
			}
		}
		//
		if ( !poss ) hasimp = TRUE;
		//
	}
	//
	if ( !hasimp ) return 5;
	//
	poss = FALSE;
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) <= 0 ) continue;
		//
		s = _SCH_MODULE_Get_TM_SIDE( tms , i );
		p = _SCH_MODULE_Get_TM_POINTER( tms , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= _SCH_CLUSTER_Get_PathRange( s , p ) ) return 6;
		//
		poss = TRUE;
		//
	}
	//
	if ( !poss ) return 7;
	//
//	if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( tms , bmc ); // 2014.06.11
	if ( hasin ) {
		if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) {
//			SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( tms , bmc , rside , rpt ); // 2014.06.11 // 2014.07.10
			SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( bmc , rside , rpt ); // 2014.06.11 // 2014.07.10
		}
		else {
//			SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( tms , bmc ); // 2014.06.11 // 2014.07.10
			SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( bmc ); // 2014.06.11 // 2014.07.10
		}
	}
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , 1 , 1768 );
	//
	return 0;
}

int SCHEDULER_MAKE_LOCK_FREE2_for_6( int side , int tms ) { // 2014.07.10
	if ( SCHEDULER_MAKE_LOCK_FREE2_for_6_Sub( side , tms , 0 ) != 0 ) {
		return SCHEDULER_MAKE_LOCK_FREE2_for_6_Sub( side , tms , 2 );
	}
	return 0;
}




int SCHEDULER_MAKE_LOCK_FREE3_for_6( int side , int tms ) { // 2015.03.20
	int i;
	int bmc1 , bmc2 , selbmc , hascount;
	//
	bmc1 = tms + BM1 + 0;
	bmc2 = tms + BM1 + 2;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc1 ) != BUFFER_TM_STATION ) return 1;
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc1 , FALSE , -1 ) ) return 2;
	if ( _SCH_MACRO_CHECK_PROCESSING( bmc1 ) > 0 ) return 3;
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( bmc1 , 0 ) ) return 4;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc2 ) != BUFFER_TM_STATION ) return 11;
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc2 , FALSE , -1 ) ) return 12;
	if ( _SCH_MACRO_CHECK_PROCESSING( bmc2 ) > 0 ) return 13;
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( bmc2 , 0 ) ) return 14;
	//
	hascount = 0;
	//
	for ( i = PM1 ; i < AL ; i++ ) {
		//
		if ( _SCH_PRM_GET_MODULE_GROUP( i ) != tms ) continue;
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) hascount++;
		//
	}
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) > 0 ) hascount++;
		//
	}
	//
	if ( hascount > 2 ) return 15;
	//
	selbmc = 0;
	//
	if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_6( bmc1 ) ) {
		if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bmc1 , FALSE , FALSE , TRUE ) ) {
			selbmc = bmc1;
		}
	}
	//
	if ( selbmc == 0 ) {
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_6( bmc2 ) ) {
			if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bmc2 , FALSE , FALSE , TRUE ) ) {
				selbmc = bmc2;
			}
		}
	}
	//
	if ( selbmc == 0 ) return 31;
	//
	_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , selbmc , -1 , TRUE , 1 , 1769 );
	//
	return 0;
}

//int SCHEDULER_MAKE_LOCK_FREE_for_6( int side , int tms ) { // 2014.01.27
//	int Res;
//	Res = SCHEDULER_MAKE_LOCK_FREE_for_6_x( side , tms );
	//----------------------------------------------------------------------
//	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM STEP SCHEDULER_MAKE_LOCK_FREE_for_6 [%d]\n" , Res );
	//----------------------------------------------------------------------
//	return Res;
//}



//=====================================================================================================================================================================
//=====================================================================================================================================================================
// LOCK 4 :
//
//		CONDITION	:	BM이 Enable이고 TMSIDE이고 PROCESSING이 아니고 BM이 한개만 있을때
//
//						TM이 공급할 Wafer를 가지고 있고 TM이 회수할 Wafer를 가지고 있지 않을때
//
//						TM이 공급할 Wafer가 갈 PM이 [Check Time] 이상으로 Process 시간이 남아 있을때
//
//						BM에 IN Wafer가 있고 IN Wafer를 위한 공간이 있을때
//
//						TM이 공급할 Wafer가 BM에 IN Wafer와 같은 Cluster Index 일때
//
//						다른 Side가 공급 가능 할때 그 Wafer가 갈 PM이 비어있고 공정도 안할때
//
//=====================================================================================================================================================================
//=====================================================================================================================================================================

int SCHEDULER_MAKE_LOCK_FREE4_for_6( int side , int tms ) { // 2015.12.01
	int i , k , m;
	int bmc , bmc1 , bmc2 , ss , es , bs , bp , ts , tp;
	int Mode;
	int checktimesec , runtime;
	BOOL hastmspace , hastmInwfr;
	int prerunside;
	//
	bmc1 = tms + BM1 + 0;
	bmc2 = tms + BM1 + 2;
	//
	if ( _SCH_MODULE_GET_USE_ENABLE( bmc1 , FALSE , -1 ) ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( bmc2 , FALSE , -1 ) ) {
			return 1;
		}
		else {
			bmc = bmc1;
		}
	}
	else {
		if ( _SCH_MODULE_GET_USE_ENABLE( bmc2 , FALSE , -1 ) ) {
			bmc = bmc2;
		}
		else {
			return 2;
		}
	}
	//
	if ( SCHEDULING_ThisIs_BM_OtherChamber6( bmc , 0 ) ) return 3;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) return 4;
	if ( _SCH_MACRO_CHECK_PROCESSING( bmc ) > 0 ) return 5;
	//
	//==============================================================================================================
	//
	hastmspace = FALSE;
	//
	hastmInwfr = FALSE;
	//
	SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_TM_PICK , bmc , &ss , &es );
	//
	for ( i = ss ; i <= es ; i++ ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) <= 0 ) {
			hastmspace = TRUE;
			continue;
		}
		//
		hastmInwfr = TRUE;
		//
		bs = _SCH_MODULE_Get_BM_SIDE( bmc , i );
		bp = _SCH_MODULE_Get_BM_POINTER( bmc , i );
		//
		if ( _SCH_CLUSTER_Get_PathDo( bs , bp ) != 0 ) return 11;
		//
	}
	//
	if ( !hastmInwfr || !hastmspace ) return 12;
	//
	hastmspace = FALSE;
	//
	hastmInwfr = FALSE;
	//
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_WAFER( tms , i ) <= 0 ) {
			//
			if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( tms,i ) ) hastmspace = TRUE;
			//
			continue;
		}
		//
		ts = _SCH_MODULE_Get_TM_SIDE( tms , i );
		tp = _SCH_MODULE_Get_TM_POINTER( tms , i );
		//
		if ( _SCH_CLUSTER_Get_ClusterIndex( bs , bp ) != _SCH_CLUSTER_Get_ClusterIndex( ts , tp ) ) return 20;
		//
		if ( _SCH_CLUSTER_Get_PathDo( ts , tp ) > _SCH_CLUSTER_Get_PathRange( ts , tp ) ) return 21;
		//
		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			//
			m = _SCH_CLUSTER_Get_PathProcessChamber( ts , tp , _SCH_CLUSTER_Get_PathDo( ts , tp ) - 1 , k );
			//
			if ( m > 0 ) {
				//
				if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , ts ) ) continue;
				//
				if ( !SCHEDULER_CONTROL_Chk_Same_Group_FOR_6( bmc , m ) ) continue;
				//
				checktimesec = _SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( m );
				//
				if ( checktimesec <= 0 ) continue;
				//
				Mode = _SCH_MACRO_CHECK_PROCESSING_INFO( m );
				//
				if ( Mode <= 0 ) return 22;
				//
				if      ( Mode >= PROCESS_INDICATOR_POST ) {
					runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 1 , m );
					if ( runtime < checktimesec ) return 31;
				}
				else if ( Mode >= PROCESS_INDICATOR_PRE ) {
					runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 2 , m );
					if ( runtime < checktimesec ) return 32;
				}
				else {
					runtime = _SCH_TIMER_GET_PROCESS_TIME_ELAPSED( 0 , m );
					if ( runtime < checktimesec ) return 33;
				}
				//
				hastmInwfr = TRUE;
				//
			}
		}
	}
	//
	if ( !hastmInwfr || !hastmspace ) return 41;
	//
	if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_Other_CM_FOR_6( bmc , &prerunside ) > 0 ) return 42;
	//
	if ( prerunside >= 0 ) { // 1761,1762,1763,1764,1765
		//
//		SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , prerunside , 0 ); // 2016.02.26
		SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , prerunside , bmc ); // 2016.02.26
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , 1 , 1761 + prerunside );
		//
	}
	else { // 1760
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , bmc , -1 , TRUE , 1 , 1760 );
		//
	}
	return 0;
}














//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


int USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_6( int CHECKING_SIDE , int mode ) {
	return -1;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int USER_DLL_SCH_INF_RUN_CONTROL_TM_1_STYLE_6( int CHECKING_SIDE , int MAXTMATM ) {
	int j;
	int wsa , wsb , pt , cf;
	int SCH_No = -1 , SCH_Order = 0 , SCH_Chamber = 0 , SCH_Slot = 0 , SCH_TMATM = 0 , SCH_Return = 0 , SCH_Ptr = 0 , SCH_SELMODE = 0 , SCH_SELPR = 0;
	int r_SIDE; // 2014.08.21
	int r_SCH_Chamber = 0 , r_SCH_Slot = 0 , r_SCH_No = 0 , r_SCH_Order = 0 , r_SCH_Return = 0;
	int Result , timebuf;
	int SCH_Out_Not_Run = 0;
	int FM_CM;
	int swmode;
	int prcs_time , post_time;
	int Function_Result , Check_Result = 0 , Sav_Int1 , Sav_Int2;
	BOOL hasin; // 2013.10.30
	//
	//--------------------------------------------------
	//-- for All
	//--------------------------------------------------
	SCH_TMATM = 0;
	//--------------------------------------------------
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() >= 1 ) { // 2007.04.30 // 2013.01.19
		//===============================================================================================================
		// 2007.04.30
		//===============================================================================================================
		if ( Scheduler_TM_Running_CYCLON_0_for_Style_6( CHECKING_SIDE ) == SYS_ABORTED ) {
			return FALSE;
		}
		//===============================================================================================================
	}
	else {
		//
		SCHEDULER_MAKE_LOCK_FREE_for_6( CHECKING_SIDE , SCH_TMATM ); // 2014.01.27
		SCHEDULER_MAKE_LOCK_FREE2_for_6( CHECKING_SIDE , SCH_TMATM ); // 2014.04.25
		SCHEDULER_MAKE_LOCK_FREE3_for_6( CHECKING_SIDE , SCH_TMATM ); // 2015.03.20
		SCHEDULER_MAKE_LOCK_FREE4_for_6( CHECKING_SIDE , SCH_TMATM ); // 2015.12.01
		//
		if ( !SCHEDULER_CONTROL_Chk_TM_PICKPLACE_DENY_FOR_6( SCH_TMATM ) ) { // 2006.06.28
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 1 CHECK START\n" );
			//----------------------------------------------------------------------
			SCH_SELMODE = -1;
			while ( TRUE ) {
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) {
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 1 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
					//----------------------------------------------------------------------
					/* Pick from PMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( 0 , CHECKING_SIDE , &SCH_Chamber , &SCH_No ); // 2014.10.04
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( 0 , &r_SIDE , &SCH_Chamber , &SCH_No ); // 2014.10.04
					if ( Check_Result == 0 ) {
						if ( CHECKING_SIDE == r_SIDE ) { // 2014.10.04
							SCH_SELMODE = 3;
							break;
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 2 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
					//----------------------------------------------------------------------
					/* Place to BMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( 0 , CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , &SCH_No , &SCH_Return ); // 2015.02.13
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( 0 , &r_SIDE , &SCH_Chamber , &SCH_Slot , &SCH_No , &SCH_Return ); // 2015.02.13
					if ( Check_Result == 0 ) {
						if ( CHECKING_SIDE == r_SIDE ) { // 2015.02.13
							SCH_SELMODE = 2;
							break;
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 3 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
					//----------------------------------------------------------------------
					/* Pick from BMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_0( CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , &SCH_No , &SCH_Return ); // 2014.05.23
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , 0 , 0 , &SCH_No , &SCH_Return , TRUE ); // 2014.05.23 // 2014.08.22
					r_SIDE = CHECKING_SIDE;
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , &r_SIDE , &SCH_Chamber , &SCH_Slot , 0 , 0 , &SCH_No , &SCH_Return , TRUE ); // 2014.05.23 // 2014.08.22
					if ( Check_Result == 0 ) {
						if ( CHECKING_SIDE == r_SIDE ) { // 2014.08.21
							SCH_SELMODE = 0;
							break;
						}
					}
					else { // 2014.05.23
						//
						if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) {
							//
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 4a CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
							//----------------------------------------------------------------------
//							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , SCH_Chamber , SCH_Slot , &SCH_No , &SCH_Return , FALSE ); // 2014.08.22
							r_SIDE = CHECKING_SIDE;
							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , &r_SIDE , &SCH_Chamber , &SCH_Slot , SCH_Chamber , SCH_Slot , &SCH_No , &SCH_Return , FALSE ); // 2014.08.22
							if ( Check_Result == 0 ) {
								if ( CHECKING_SIDE == r_SIDE ) { // 2014.08.21
									SCH_SELMODE = 0;
									break;
								}
							}
							//
						}
						//
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 4 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
					//----------------------------------------------------------------------
					/* Place to PMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( 0 , CHECKING_SIDE , &SCH_Chamber , &SCH_No , &SCH_Order ); // 2014.08.21
					//
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( 0 , &r_SIDE , &SCH_Chamber , &SCH_No , &SCH_Order ); // 2014.08.21
					//
					if ( Check_Result == 0 ) {
						if ( CHECKING_SIDE == r_SIDE ) { // 2014.08.21
							SCH_SELMODE = 1;
							break;
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 5 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
					//----------------------------------------------------------------------
				}
				else {
					//----------------------------------------------------------------------
					r_SIDE = r_SCH_Chamber = r_SCH_Slot = r_SCH_No = r_SCH_Order = r_SCH_Return = 0; // 2014.08.22
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 11 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return );
					//----------------------------------------------------------------------
					/* Pick from BMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_0( CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , &r_SCH_No , &r_SCH_Return ); // 2014.05.23
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , 0 , 0 , &r_SCH_No , &r_SCH_Return , TRUE ); // 2014.05.23 // 2014.08.22
					r_SIDE = CHECKING_SIDE;
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , &r_SIDE , &r_SCH_Chamber , &r_SCH_Slot , 0 , 0 , &r_SCH_No , &r_SCH_Return , TRUE ); // 2014.05.23 // 2014.08.22
					if ( Check_Result == 0 ) {
//						SCH_SELMODE = 0; // 2014.08.21
						SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 0 : 100;
						SCH_Chamber = r_SCH_Chamber;
						SCH_Slot    = r_SCH_Slot;
						SCH_No      = r_SCH_No;
						SCH_Order   = r_SCH_Order;
						SCH_Return  = r_SCH_Return;
						SCH_SELPR	= _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber );
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break;
					}
					else { // 2014.05.23
						//
						if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) {
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 12a CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return );
							//----------------------------------------------------------------------
							//
//							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , r_SCH_Chamber , r_SCH_Slot , &r_SCH_No , &r_SCH_Return , FALSE ); // 2014.08.22
							r_SIDE = CHECKING_SIDE;
							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( 0 , &r_SIDE , &r_SCH_Chamber , &r_SCH_Slot , r_SCH_Chamber , r_SCH_Slot , &r_SCH_No , &r_SCH_Return , FALSE ); // 2014.08.22
							if ( Check_Result == 0 ) {
//								SCH_SELMODE = 0; // 2014.08.21
								SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 0 : 100;
								SCH_Chamber = r_SCH_Chamber;
								SCH_Slot    = r_SCH_Slot;
								SCH_No      = r_SCH_No;
								SCH_Order   = r_SCH_Order;
								SCH_Return  = r_SCH_Return;
								SCH_SELPR	= _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber );
								if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break;
							}
							//
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 12 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return );
					//----------------------------------------------------------------------
					/* Place to PMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( 0 , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_No , &r_SCH_Order ); // 2014.08.21
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( 0 , &r_SIDE , &r_SCH_Chamber , &r_SCH_No , &r_SCH_Order ); // 2014.08.21
					//
					if ( Check_Result == 0 ) {
						if ( ( SCH_SELMODE == -1 ) || ( SCH_SELPR > _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber ) ) ) {
							//
//							SCH_SELMODE = 1; // 2014.08.21
							SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 1 : 101;
							SCH_Chamber = r_SCH_Chamber;
							SCH_Slot    = r_SCH_Slot;
							SCH_No      = r_SCH_No;
							SCH_Order   = r_SCH_Order;
							SCH_Return  = r_SCH_Return;
							SCH_SELPR	= _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber );
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break;
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 13 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return );
					//----------------------------------------------------------------------
					/* Place to BMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( 0 , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , &r_SCH_No , &r_SCH_Return ); // 2015.02.13
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( 0 , &r_SIDE , &r_SCH_Chamber , &r_SCH_Slot , &r_SCH_No , &r_SCH_Return ); // 2015.02.13
					if ( Check_Result == 0 ) {
						if ( ( SCH_SELMODE == -1 ) || ( SCH_SELPR > _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber ) ) ) {
//							SCH_SELMODE = 2; // 2015.02.13
							SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 2 : 102; // 2015.02.13
							SCH_Chamber = r_SCH_Chamber;
							SCH_Slot    = r_SCH_Slot;
							SCH_No      = r_SCH_No;
							SCH_Order   = r_SCH_Order;
							SCH_Return  = r_SCH_Return;
							SCH_SELPR	= _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber );
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break;
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 14 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return );
					//----------------------------------------------------------------------
					/* Pick from PMx */
//					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( 0 , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_No ); // 2014.10.04
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( 0 , &r_SIDE , &r_SCH_Chamber , &r_SCH_No ); // 2014.10.04
					if ( Check_Result == 0 ) {
						if ( ( SCH_SELMODE == -1 ) || ( SCH_SELPR > _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber ) ) ) {
//							SCH_SELMODE = 3; // 2014.10.04
							SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 3 : 103; // 2014.10.04
							SCH_Chamber = r_SCH_Chamber;
							SCH_Slot    = r_SCH_Slot;
							SCH_No      = r_SCH_No;
							SCH_Order   = r_SCH_Order;
							SCH_Return  = r_SCH_Return;
							SCH_SELPR	= _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber );
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break;
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 15 , "TM STEP 15 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return );
					//----------------------------------------------------------------------
				}
				break;
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM STEP 21 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order );
			//----------------------------------------------------------------------
			if ( SCH_SELMODE >= 100 ) return TRUE; // 2014.08.21
			//==============================================================================================
			// Pick from BMx
			//==============================================================================================
			if ( SCH_SELMODE == 0 ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < 0 ) { // 2003.09.09
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - BM1 + 1 );
					return FALSE;
				}
				//
//				SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , 0 ); /* 2013.10.30 */ // 2014.07.10
				SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( SCH_Chamber ); /* 2013.10.30 */ // 2014.07.10
				//
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 );
				//-------------------------------------------
				pt = _SCH_MODULE_Get_BM_POINTER( SCH_Chamber , SCH_Slot );
				cf = SCH_No;
				Result = SCH_Return;
				//-----------------------------------------------------------------------
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); // 2006.05.15
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_STARTING );
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
				//-----------------------------------------------------------------------
				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE , pt );
				wsb = SCH_Slot;
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt );
				//-----------------------------------------------------------------------
				swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 ); // 2007.07.25
				//-----------------------------------------------------------------------
				Sav_Int2 = _SCH_SUB_Get_Last_Status( CHECKING_SIDE ); // 2006.05.15
				if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) {
					_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 2 );
				}
//				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , -1 , FALSE , swmode , -1 , -1 ); // 2007.07.13 // 2013.11.07
				Function_Result = SCHEDULING_MOVE_OPERATION( 0 , MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , swmode ); // 2007.07.13 // 2013.11.07
				if ( Function_Result == SYS_ABORTED ) { // 2006.05.15
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) { // 2006.05.15
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 );
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
					_SCH_SUB_Set_Last_Status( CHECKING_SIDE , Sav_Int2 );
				}
				if ( Function_Result == SYS_SUCCESS ) { // 2006.05.15
					_SCH_MODULE_Set_TM_PATHORDER( SCH_TMATM , cf , 0 );
					_SCH_MODULE_Set_TM_TYPE( SCH_TMATM , cf , SCHEDULER_MOVING_IN );
					_SCH_MODULE_Set_TM_SIDE_POINTER( SCH_TMATM , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
					_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , cf , _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot ) );
					_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , 0 , BUFFER_READY_STATUS );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , -1 );
					//-----------------------------------------------------------------------
//					if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( CHECKING_SIDE , SCH_TMATM ) ) { // 2014.07.10
					if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( CHECKING_SIDE , SCH_TMATM , SCH_Chamber ) ) { // 2014.07.10
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , -1 , TRUE , 1 , 1 );
					}
				}
//				Sleep(1); // 2008.05.19
				return TRUE;
			}
			//==============================================================================================
			// Place to PMx
			//==============================================================================================
			if ( SCH_SELMODE == 1 ) {
				swmode = 0;
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 );
				//-------------------------------------------
				SCH_Slot = 1;
				//-------------------------------------------
				pt = _SCH_MODULE_Get_TM_POINTER( SCH_TMATM , SCH_No );
				//-------------------------------------------
				wsa = _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No );
				wsb = SCH_Slot;
				//-----------------------------------------------------------------------
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) ) {
				case -1 :
//				case -3 : // 2006.06.26
//				case -5 : // 2006.06.26
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - PM1 + 1 );
					return FALSE;
					break;
				}
				//=====================================================================================================
				// 2007.02.22
				//=====================================================================================================
				if ( _SCH_SUB_Check_Last_And_Current_CPName_is_Different( CHECKING_SIDE , pt , SCH_Chamber ) ) {
					if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( CHECKING_SIDE , SCH_Chamber ) == 0 ) {
						_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( CHECKING_SIDE * 1000 ) + SCH_Chamber , 3 );
					}
					else {
						_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( CHECKING_SIDE * 1000 ) + SCH_Chamber , 4 );
					}
				}
				//=====================================================================================================
				//-----------------------------------------------------------------------
				// Pre Process Part Begin
				//-----------------------------------------------------------------------
				j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , SCH_No , SCH_Chamber );
				if      ( j == 0 ) {
					_SCH_CLUSTER_Set_PathProcessChamber_Select_Other_Disable( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Chamber ); // 2004.03.18
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 1 ); // 2005.01.26
					swmode = -1; // 2005.05.11
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); // 2005.05.20
					//-----------------------------------------------------------------------
				}
				else if ( j == 1 ) {
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 1 ); // 2005.01.26
					swmode = -1; // 2005.05.11
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); // 2005.05.20
					//-----------------------------------------------------------------------
				}
				else if ( j == -1 ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Place Fail to %s(%d)[F%c]%cWHTMPLACEFAIL PM%d:%d:%d:%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , pt + 1 , SCH_No + 'A' , 9 , SCH_Chamber - PM1 + 1 , pt + 1 , pt + 1 , SCH_No + 'A' );
					return FALSE;
				}
				//
				if ( swmode != -1 ) { // 2005.05.11
					Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); // 2006.05.15
					//-----------------------------------------------------------------------
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING );
					//-----------------------------------------------------------------------
//					Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , 0 , -1 , FALSE , 0 , -1 , -1 ); // 2013.11.07
					Function_Result = SCHEDULING_MOVE_OPERATION( 0 , MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , 0 , 0 ); // 2013.11.07
					if ( Function_Result == SYS_ABORTED ) { // 2006.05.15
						return FALSE;
					}
					else if ( Function_Result == SYS_ERROR ) { // 2006.05.15
						_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 ); // 2006.05.15
					}
					if ( Function_Result == SYS_SUCCESS ) { // 2006.05.15
						_SCH_MODULE_Set_PM_SIDE_POINTER( SCH_Chamber , CHECKING_SIDE , pt , 0 , 0 );
						_SCH_MODULE_Set_PM_PATHORDER( SCH_Chamber , 0 , SCH_Order );
						_SCH_MODULE_Set_PM_WAFER( SCH_Chamber , 0 , _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No ) );
						_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , SCH_No , 0 );
						//-----------------------------------------------------------------------
						_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 );
						//-----------------------------------------------------------------------
						if ( _SCH_CLUSTER_Check_HasProcessData_Post( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order ) ) {
							if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( SCH_Chamber , &prcs_time , &post_time ) % 10 ) == 1 ) {
								_SCH_CLUSTER_Set_PathProcessData_SkipPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order );
							}
						}
						//-----------------------------------------------------------------------
						_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING2 );
						//-----------------------------------------------------------------------
						if ( _SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt ,
							  SCH_Chamber , -1 ,
							  wsa , -1 , SCH_No , -1 ,
							  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order , 0 ) ,
							  0 , "" , PROCESS_DEFAULT_MINTIME ,
							  _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order , ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) ,
							  -1 , -1 , -1 , 0 ,
							  0 , 601 ) == -1 ) {
						}
						//-----------------------------------------------------------------------
						// 2007.02.06
						//-----------------------------------------------------------------------
//						if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( CHECKING_SIDE , SCH_TMATM ) ) { // 2014.07.10
						if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( CHECKING_SIDE , SCH_TMATM , BM1 + SCH_TMATM ) ) { // 2014.07.10
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , BM1 + SCH_TMATM , -1 , TRUE , 1 , 1 );
						}
						if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( CHECKING_SIDE , SCH_TMATM , BM1 + SCH_TMATM + 2 ) ) { // 2014.07.10
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , BM1 + SCH_TMATM + 2 , -1 , TRUE , 1 , 1 );
						}
						//swmode = 1; // 2005.05.11
						//break; // 2005.05.11
					}
//					Sleep(1); // 2005.05.11 // 2008.05.19
					return TRUE; // 2005.05.11
				}
			}
			//==============================================================================================
			// Place to BMx
			//==============================================================================================
			if ( SCH_SELMODE == 2 ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < 0 ) { // 2003.09.09
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - BM1 + 1 );
					return FALSE;
				}
				//
//				SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , 0 ); /* 2013.10.30 */ // 2014.07.10
				SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( SCH_Chamber ); /* 2013.10.30 */ // 2014.07.10
				//
				SCHEDULER_CONTROL_Check_BEFORE_PLACE_TO_BM_PRE_END_FOR_6( 0 , CHECKING_SIDE , -1 , SCH_No ); // 2007.05.03
				//
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 16 );
				//-------------------------------------------
				pt = _SCH_MODULE_Get_TM_POINTER( SCH_TMATM , SCH_No );
				//-----------------------------------------------------------------------
				if ( SCH_Return ) { // 2005.01.26
					if ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) == 1 ) {
						_SCH_CLUSTER_Set_PathProcessData_UsedPre_Restore( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 );
						_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 0 );
					}
				}
				//-----------------------------------------------------------------------
				wsa = _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No );
				wsb = SCH_Slot;
				//-----------------------------------------------------------------------
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_BM_END );
				//-----------------------------------------------------------------------
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt );
				//-----------------------------------------------------------------------
				Result = SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( CHECKING_SIDE , SCH_TMATM , SCH_Chamber , TRUE );
				//-----------------------------------------------------------------------
//				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , Result , -1 , FALSE , 0 , -1 , -1 ); // 2013.11.07
				Function_Result = SCHEDULING_MOVE_OPERATION( 0 , MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , Result , 0 ); // 2013.11.07
				if ( Function_Result == SYS_ABORTED ) { // 2006.05.15
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) { // 2006.05.15
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 );
				}
				if ( Function_Result == SYS_SUCCESS ) { // 2006.05.15
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_BM_SIDE_POINTER( SCH_Chamber , SCH_Slot , CHECKING_SIDE , pt );
					_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No ) , BUFFER_OUTWAIT_STATUS );
					_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , SCH_No , 0 );
					_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 );
					//-----------------------------------------------------------------------
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , 1 ); // 2008.01.15
					//-----------------------------------------------------------------------
//					if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( CHECKING_SIDE , SCH_TMATM ) ) { // 2014.07.10
					if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( CHECKING_SIDE , SCH_TMATM , SCH_Chamber ) ) { // 2014.07.10
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , -1 , TRUE , 1 , 2 );
					}
				}
//				Sleep(1); // 2008.05.19
				return TRUE;
			}
			//==============================================================================================
			// Pick from PMx
			//==============================================================================================
			if ( SCH_SELMODE == 3 ) {
				//-------------------------------------------
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 15 );
				//-------------------------------------------
				SCH_Out_Not_Run = 0;
				//
				pt = _SCH_MODULE_Get_PM_POINTER( SCH_Chamber , 0 );
				SCH_Order = _SCH_MODULE_Get_PM_PATHORDER( SCH_Chamber , 0 );
				//
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) ) {
				case -1 : // All Stop
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - PM1 + 1 );
					return FALSE;
					break;
				case -2 : // Cluster - Continue , Process - Continue
					break;
				case -3 : // // Cluster - Continue , Process - Disable
					SCH_Out_Not_Run = 1;
					break;
				case -4 : // Cluster - Out , Process - Continue
					break;
				case -5 : // Cluster - Out , Process - Disable
					SCH_Out_Not_Run = 1;
					break;
				}
				//
				SCH_Slot = 1;
				wsa = _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 );
				wsb = SCH_Slot;
				cf  = SCH_No;
				//-----------------------------------------------------------------------
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt );
				//-----------------------------------------------------------------------
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_WAITING );
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt );
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt );
				//-----------------------------------------------------------------------
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) { // 2005.02.22
					Result = 0;
				}
				else {
					if ( ( _SCH_MODULE_Get_TM_Switch_Signal( SCH_TMATM ) == -1 ) ||
						( _SCH_MODULE_Get_TM_Switch_Signal( SCH_TMATM ) == SCH_Chamber ) ) { // 2005.03.15
//						if ( ( SCH_Out_Not_Run == 0 ) && _SCH_CLUSTER_Check_Possible_UsedPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order ) ) { // 2006.01.06
//						if ( ( SCH_Out_Not_Run == 0 ) && ( _SCH_CLUSTER_Check_Possible_UsedPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order ) || _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , SCH_Chamber ) ) ) { // 2006.01.06 // 2006.02.02
						if ( ( SCH_Out_Not_Run != 0 ) || ( _SCH_CLUSTER_Check_Possible_UsedPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order ) || _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , SCH_Chamber ) ) ) { // 2006.01.06 // 2006.02.02
							Result = 0;
						}
						else {
							Result = SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_FOR_6( SCH_TMATM , CHECKING_SIDE , cf , SCH_Chamber );
						}
					}
					else { // 2005.03.15
						Result = 0;
					}
				}
				//==================================================================================================================
				//-----------------------------------------------------------------------
				swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 ); // 2007.07.25
				//-----------------------------------------------------------------------
//				Function_Result = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , -1 , FALSE , swmode , -1 , -1 ); // 2007.07.13 // 2013.11.07
				Function_Result = SCHEDULING_MOVE_OPERATION( 0 , MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , swmode ); // 2007.07.13 // 2013.11.07
				if ( Function_Result == SYS_ABORTED ) { // 2006.05.15
					return FALSE;
				}
				else if ( Function_Result == SYS_ERROR ) { // 2006.05.15
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 );
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt );
				}
				if ( Function_Result == SYS_SUCCESS ) { // 2006.05.15
					_SCH_MODULE_Set_TM_SIDE_POINTER( SCH_TMATM , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 );
					_SCH_MODULE_Set_TM_PATHORDER( SCH_TMATM , cf , SCH_Order );
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , cf , _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 ) );
					_SCH_MODULE_Set_PM_WAFER( SCH_Chamber , 0 , 0 );
					//-----------------------------------------------------------------------
					if ( Result > 0 ) {
						_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , SCH_Chamber );
						//=========================================================================================================================
						SCHEDULING_CHECK_Switch_PLACE_From_Fixed_TM_and_OtherDisable_FOR_6( SCH_TMATM , CHECKING_SIDE , cf , SCH_Chamber ); // 2006.02.02
						//=========================================================================================================================
					}
					//-----------------------------------------------------------------------
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 );
					//-----------------------------------------------------------------------
					if ( SCH_Out_Not_Run == 0 ) {
						_SCH_MACRO_POST_PROCESS_OPERATION( CHECKING_SIDE , pt , cf , SCH_Chamber );
					}
					//-----------------------------------------------------------------------
				}
				//
				SCHEDULER_CONTROL_Check_BEFORE_PLACE_TO_BM_PRE_END_FOR_6( 0 , CHECKING_SIDE , SCH_Chamber , cf ); // 2007.05.03
				//
//				Sleep(1); // 2008.05.19
				return TRUE;
			}
			//==============================================================================================
			// Force Vent
			//==============================================================================================
			if ( SCH_SELMODE == -1 ) { // 2007.10.02
				if ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() != 0 ) {
					if ( SCHEDULER_CONTROL_Check_TM_AND_PM_WAITING_FOR_NOWAFER_PRCS( CHECKING_SIDE , SCH_TMATM , BM1 + SCH_TMATM , &hasin ) ) { // 2007.10.02
//						if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , BM1 + SCH_TMATM ); /* 2013.10.30 */ // 2014.01.28 // 2014.07.10
						if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( BM1 + SCH_TMATM ); /* 2013.10.30 */ // 2014.01.28 // 2014.07.10
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , BM1 + SCH_TMATM , -1 , TRUE , 1 , 111 );
//						if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , TRUE ); /* 2013.10.30 */ // 2014.01.28
					}
				}
			}
			//==============================================================================================
		}
		//==============================================================================================
		// Move to PM1 or PM2
		//==============================================================================================
		if ( _SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() ) {
			if ( _SCH_MODULE_Get_TM_Move_Signal( SCH_TMATM ) == -1 ) {
				swmode = -1;
//				sch = PM1 + ( 2 * SCH_TMATM ); // 2004.02.25
//				ech = PM1 + ( ( 2 * SCH_TMATM ) + 1 ); // 2004.02.25
//				for ( SCH_Chamber = sch ; SCH_Chamber <= ech ; SCH_Chamber++ ) { // 2004.02.25
				for ( SCH_Chamber = PM1 ; SCH_Chamber < AL ; SCH_Chamber++ ) { // 2004.02.25
					if ( _SCH_PRM_GET_MODULE_GROUP( SCH_Chamber ) == SCH_TMATM ) { // 2004.02.25
						if ( _SCH_MODULE_GET_USE_ENABLE( SCH_Chamber , FALSE , CHECKING_SIDE ) ) { // 2003.08.05
							SCH_No = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( 0 , CHECKING_SIDE , SCH_Chamber , 1 , &SCH_Order , &SCH_Ptr ); // pick
						}
						else { // 2003.08.05
							SCH_No = -1;
						}
						if ( SCH_No == -99 ) {
							swmode = -1;
							break;
						}
						if ( SCH_No >= 0 ) {
							if ( swmode == -1 ) {
								pt       = 0;
								if ( SCH_No == 0 ) {
									cf = 1;
								}
								else {
									cf = 0;
								}
								swmode = SCH_Chamber;
								Result   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , SCH_Chamber );
							}
							else {
								if ( _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , SCH_Chamber ) < Result ) {
									pt       = 0;
									if ( SCH_No == 0 ) {
										cf = 1;
									}
									else {
										cf = 0;
									}
									swmode = SCH_Chamber;
									Result   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , SCH_Chamber );
								}
							}
						}
						SCH_No = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( 0 , CHECKING_SIDE , SCH_Chamber , 2 , &SCH_Order , &SCH_Ptr ); // place
						if ( SCH_No == -99 ) {
							swmode = -1;
							break;
						}
						if ( SCH_No >= 0 ) {
							if ( swmode == -1 ) {
								pt       = 1;
								cf       = SCH_No;
								swmode = SCH_Chamber;
								if      ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_PRE ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < PROCESS_INDICATOR_POST ) )
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , SCH_Chamber );
								else if   ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_POST )
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , SCH_Chamber );
								else
									timebuf = 0;
								Result   = timebuf;
							}
							else {
								if      ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_PRE ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < PROCESS_INDICATOR_POST ) )
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , SCH_Chamber );
								else if   ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_POST )
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , SCH_Chamber );
								else
									timebuf = 0;
								if ( timebuf < Result ) {
									pt       = 1;
									cf       = SCH_No;
									swmode = SCH_Chamber;
									Result   = timebuf;
								}
							}
						}
					}
				}
				if ( swmode != -1 ) {
					SCH_Chamber = swmode;
					if ( pt == 0 ) {
						wsa = _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 );
					}
					else {
						wsa = _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , cf );
					}
					//-----------------------------------------------------------------------
					if ( _SCH_MACRO_TM_MOVE_OPERATION_0( CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , 1 ) == SYS_ABORTED ) {
						return FALSE;
					}
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , SCH_Chamber );
					_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , -1 );
//					Sleep(1); // 2008.05.19
					return TRUE;
				}
			}
		}
		//=============================================================================================
		// 2005.10.19
		//=============================================================================================
		if ( SCHEDULER_CONTROL_Chk_RUN_HOLD_WAIT_FOR_6( 0 , CHECKING_SIDE ) ) {
			if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( BM1 , FALSE , FALSE , TRUE ) ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , BM1 , -1 , TRUE , 1 , 101 );
			}
		}
		//=============================================================================================
	} // 2007.04.30
	//==============================================================================================
	_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , -1 );
	//==============================================================================================
//		Sleep(5); // 2004.05.17
//	Sleep(10); // 2004.05.17 // 2008.05.19
	//
	return TRUE;
}



#define MAKE_SCHEDULER_MAIN_SUB_TM_for_6( ZZZ , ZZZ2 ) int USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ2##_STYLE_6( int CHECKING_SIDE ) { \
	int j; \
	int wsa , wsb , pt , cf; \
	int SCH_No = -1 , SCH_Order = 0 , SCH_Chamber = 0 , SCH_Slot = 0 , SCH_TMATM = 0 , SCH_Return = 0 , SCH_Ptr = 0 , SCH_SELMODE = 0 , SCH_SELPR = 0; \
	int r_SIDE; /* 2014.08.21 */ \
	int r_SCH_Chamber = 0 , r_SCH_Slot , r_SCH_No , r_SCH_Order = 0 , r_SCH_Return; \
	int Result , timebuf; \
	int SCH_Out_Not_Run = 0; \
	int FM_CM; \
	int swmode; \
	int prcs_time , post_time; \
	int Function_Result , Check_Result = 0 , Sav_Int1 , Sav_Int2; \
	BOOL hasin; /* 2013.10.30 */ \
\
	SCH_TMATM = ##ZZZ##; \
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() >= 1 ) { \
		if ( Scheduler_TM_Running_CYCLON_##ZZZ##_for_Style_6( CHECKING_SIDE ) == SYS_ABORTED ) { \
			return FALSE; \
		} \
	} \
	else { \
\
		SCHEDULER_MAKE_LOCK_FREE_for_6( CHECKING_SIDE , SCH_TMATM ); /* 2014.01.27 */ \
		SCHEDULER_MAKE_LOCK_FREE2_for_6( CHECKING_SIDE , SCH_TMATM ); /* 2014.04.25 */ \
		SCHEDULER_MAKE_LOCK_FREE3_for_6( CHECKING_SIDE , SCH_TMATM ); /* 2015.03.20 */ \
		SCHEDULER_MAKE_LOCK_FREE4_for_6( CHECKING_SIDE , SCH_TMATM ); /* 2015.12.01 */ \
\
		if ( !SCHEDULER_CONTROL_Chk_TM_PICKPLACE_DENY_FOR_6( SCH_TMATM ) ) { /* 2006.06.28 */ \
			SCH_SELMODE = -1; \
			while ( TRUE ) { \
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) { \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 1 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
					/* Pick from PMx */ \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( ##ZZZ## , &r_SIDE /* CHECKING_SIDE */ , &SCH_Chamber , &SCH_No ); \
					if ( Check_Result == 0 ) { \
						if ( CHECKING_SIDE == r_SIDE ) { /* 2014.10.04 */ \
							SCH_SELMODE = 3; \
						} \
						break; \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 2 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
					/* Place to BMx */ \
					/* Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( ##ZZZ## , CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , &SCH_No , &SCH_Return ); */ /* 2015.02.13 */ \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( ##ZZZ## , &r_SIDE , &SCH_Chamber , &SCH_Slot , &SCH_No , &SCH_Return ); /* 2015.02.13 */ \
					if ( Check_Result == 0 ) { \
						if ( CHECKING_SIDE == r_SIDE ) { /* 2015.02.13 */ \
							SCH_SELMODE = 2; \
							break; \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 3 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
					/* Pick from BMx */ \
/*					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_##ZZZ##( CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , &SCH_No , &SCH_Return ); */ /* 2014.05.23 */ \
/*					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , 0 , 0 , &SCH_No , &SCH_Return , TRUE ); */ /* 2014.05.23 */ /* 2014.08.22 */ \
					r_SIDE = CHECKING_SIDE; \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , &r_SIDE , &SCH_Chamber , &SCH_Slot , 0 , 0 , &SCH_No , &SCH_Return , TRUE ); /* 2014.05.23 */ /* 2014.08.22 */ \
					if ( Check_Result == 0 ) { \
						if ( CHECKING_SIDE == r_SIDE ) { /* 2014.08.21 */ \
							SCH_SELMODE = 0; \
							break; \
						} \
					} \
					else { /* 2014.05.23 */ \
						if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) { \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 4a CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
/*							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , CHECKING_SIDE , &SCH_Chamber , &SCH_Slot , SCH_Chamber , SCH_Slot , &SCH_No , &SCH_Return , FALSE ); */ /* 2014.08.22 */ \
							r_SIDE = CHECKING_SIDE; \
							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , &r_SIDE , &SCH_Chamber , &SCH_Slot , SCH_Chamber , SCH_Slot , &SCH_No , &SCH_Return , FALSE ); /* 2014.08.22 */ \
							if ( Check_Result == 0 ) { \
								if ( CHECKING_SIDE == r_SIDE ) { /* 2014.08.21 */ \
									SCH_SELMODE = 0; \
									break; \
								} \
							} \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 4 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
					/* Place to PMx */ \
					/* Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( ##ZZZ## , CHECKING_SIDE , &SCH_Chamber , &SCH_No , &SCH_Order ); */ /* 2014.08.21 */ \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( ##ZZZ## , &r_SIDE , &SCH_Chamber , &SCH_No , &SCH_Order ); /* 2014.08.21 */ \
					if ( Check_Result == 0 ) { \
						if ( CHECKING_SIDE == r_SIDE ) { /* 2014.08.21 */ \
							SCH_SELMODE = 1; \
							break; \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 5 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
				} \
				else { \
\
					r_SIDE = r_SCH_Chamber = r_SCH_Slot = r_SCH_No = r_SCH_Order = r_SCH_Return = 0; /* 2014.08.22 */ \
\
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 11 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return ); \
					/* Pick from BMx */ \
/*					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6_##ZZZ##( CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , &r_SCH_No , &r_SCH_Return ); */ /* 2014.05.23 */ \
/*					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , 0 , 0 , &r_SCH_No , &r_SCH_Return , TRUE ); */ /* 2014.05.23 */ /* 2014.08.22 */ \
					r_SIDE = CHECKING_SIDE; \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , &r_SIDE , &r_SCH_Chamber , &r_SCH_Slot , 0 , 0 , &r_SCH_No , &r_SCH_Return , TRUE ); /* 2014.05.23 */ /* 2014.08.22 */ \
					if ( Check_Result == 0 ) { \
/*						SCH_SELMODE = 0; */ /* 2014.08.22 */ \
						SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 0 : 100; /* 2014.08.22 */ \
						SCH_Chamber = r_SCH_Chamber; \
						SCH_Slot    = r_SCH_Slot; \
						SCH_No      = r_SCH_No; \
						SCH_Order   = r_SCH_Order; \
						SCH_Return  = r_SCH_Return; \
						SCH_SELPR	= _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber ); \
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break; \
					} \
					else { /* 2014.05.23 */ \
						if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 2 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) { \
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 12a CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return ); \
/*							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , r_SCH_Chamber , r_SCH_Slot , &r_SCH_No , &r_SCH_Return , FALSE ); */ /* 2014.08.22 */ \
							r_SIDE = CHECKING_SIDE; \
							Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_BM_FOR_6( ##ZZZ## , &r_SIDE , &r_SCH_Chamber , &r_SCH_Slot , r_SCH_Chamber , r_SCH_Slot , &r_SCH_No , &r_SCH_Return , FALSE ); /* 2014.08.22 */ \
							if ( Check_Result == 0 ) { \
/*								SCH_SELMODE = 0; */ /* 2014.08.22 */ \
								SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 0 : 100; /* 2014.08.22 */ \
								SCH_Chamber = r_SCH_Chamber; \
								SCH_Slot    = r_SCH_Slot; \
								SCH_No      = r_SCH_No; \
								SCH_Order   = r_SCH_Order; \
								SCH_Return  = r_SCH_Return; \
								SCH_SELPR	= _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber ); \
								if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break; \
							} \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 12 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return ); \
					/* Place to PMx */ \
					/* Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( ##ZZZ## , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_No , &r_SCH_Order ); */ /* 2014.08.21 */ \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_PM_FOR_6( ##ZZZ## , &r_SIDE , &r_SCH_Chamber , &r_SCH_No , &r_SCH_Order ); /* 2014.08.21 */ \
					if ( Check_Result == 0 ) { \
						if ( ( SCH_SELMODE == -1 ) || ( SCH_SELPR > _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber ) ) ) { \
							/* SCH_SELMODE = 1; */ /* 2014.08.21 */ \
							SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 1 : 101; /* 2014.08.21 */ \
							SCH_Chamber = r_SCH_Chamber; \
							SCH_Slot    = r_SCH_Slot; \
							SCH_No      = r_SCH_No; \
							SCH_Order   = r_SCH_Order; \
							SCH_Return  = r_SCH_Return; \
							SCH_SELPR	= _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber ); \
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break; \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 13 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return ); \
					/* Place to BMx */ \
					/* Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( ##ZZZ## , CHECKING_SIDE , &r_SCH_Chamber , &r_SCH_Slot , &r_SCH_No , &r_SCH_Return ); */ /* 2015.02.13 */ \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PLACE_TO_BM_FOR_6( ##ZZZ## , &r_SIDE , &r_SCH_Chamber , &r_SCH_Slot , &r_SCH_No , &r_SCH_Return ); /* 2015.02.13 */ \
					if ( Check_Result == 0 ) { \
						if ( ( SCH_SELMODE == -1 ) || ( SCH_SELPR > _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber ) ) ) { \
							/* SCH_SELMODE = 2; */ /* 2015.02.13 */ \
							SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 2 : 102; /* 2015.02.13 */ \
							SCH_Chamber = r_SCH_Chamber; \
							SCH_Slot    = r_SCH_Slot; \
							SCH_No      = r_SCH_No; \
							SCH_Order   = r_SCH_Order; \
							SCH_Return  = r_SCH_Return; \
							SCH_SELPR	= _SCH_PRM_GET_Putting_Priority( r_SCH_Chamber ); \
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break; \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 14 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return ); \
					/* Pick from PMx */ \
					Check_Result = SCHEDULER_CONTROL_Check_TM_PICK_FROM_PM_FOR_6( ##ZZZ## , &r_SIDE /* CHECKING_SIDE */ , &r_SCH_Chamber , &r_SCH_No ); \
					if ( Check_Result == 0 ) { \
						if ( ( SCH_SELMODE == -1 ) || ( SCH_SELPR > _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber ) ) ) { \
							/* SCH_SELMODE = 3; */ /* 2014.10.04 */ \
							SCH_SELMODE = ( r_SIDE == CHECKING_SIDE ) ? 3 : 103; /* 2014.10.04 */ \
							SCH_Chamber = r_SCH_Chamber; \
							SCH_Slot    = r_SCH_Slot; \
							SCH_No      = r_SCH_No; \
							SCH_Order   = r_SCH_Order; \
							SCH_Return  = r_SCH_Return; \
							SCH_SELPR	= _SCH_PRM_GET_Getting_Priority( r_SCH_Chamber ); \
							if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 0 ) break; \
						} \
					} \
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 15 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d(%d,%d,%d,%d,%d,%d)\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order , r_SIDE , r_SCH_Chamber , r_SCH_Slot , r_SCH_No , r_SCH_Order , r_SCH_Return ); \
				} \
				break; \
			} \
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d STEP 21 CHECK DATA=%d,%d , SCH_Chamber=%d,SCH_Slot=%d,SCH_No=%d,SCH_Return=%d,SCH_Order=%d\n" , ##ZZZ## + 1, SCH_SELMODE , Check_Result , SCH_Chamber , SCH_Slot , SCH_No , SCH_Return , SCH_Order ); \
			if ( SCH_SELMODE >= 100 ) return TRUE; /* 2014.08.21 */ \
			/* Pick from BMx */ \
			if ( SCH_SELMODE == 0 ) { \
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < 0 ) { \
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - BM1 + 1 ); \
					return FALSE; \
				} \
				/* SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , 0 ); */ /* 2013.10.30 */ /* 2014.07.10 */ \
				SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( SCH_Chamber ); /* 2013.10.30 */ /* 2014.07.10 */ \
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 ); \
				pt = _SCH_MODULE_Get_BM_POINTER( SCH_Chamber , SCH_Slot ); \
				cf = SCH_No; \
				Result = SCH_Return; \
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); \
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_STARTING ); \
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt ); \
				wsa = _SCH_CLUSTER_Get_SlotIn(CHECKING_SIDE , pt ); \
				wsb = SCH_Slot; \
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ); \
				swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 ); \
				Sav_Int2 = _SCH_SUB_Get_Last_Status( CHECKING_SIDE ); \
				if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , pt ) ) { \
					_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 2 ); \
				} \
				/* Function_Result = _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , -1 , FALSE , swmode , -1 , -1 ); */ /* 2013.11.07 */ \
				Function_Result = SCHEDULING_MOVE_OPERATION( ##ZZZ## , MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , swmode ); /* 2013.11.07 */ \
				if ( Function_Result == SYS_ABORTED ) { \
					return FALSE; \
				} \
				else if ( Function_Result == SYS_ERROR ) { \
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 ); \
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt ); \
					_SCH_SUB_Set_Last_Status( CHECKING_SIDE , Sav_Int2 ); \
				} \
				if ( Function_Result == SYS_SUCCESS ) { \
					_SCH_MODULE_Set_TM_PATHORDER( SCH_TMATM , cf , 0 ); \
					_SCH_MODULE_Set_TM_TYPE( SCH_TMATM , cf , SCHEDULER_MOVING_IN ); \
					_SCH_MODULE_Set_TM_SIDE_POINTER( SCH_TMATM , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
					_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , cf , _SCH_MODULE_Get_BM_WAFER( SCH_Chamber , SCH_Slot ) ); \
					_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , 0 , BUFFER_READY_STATUS ); \
					_SCH_MODULE_Inc_TM_OutCount( CHECKING_SIDE ); \
					_SCH_MODULE_Inc_TM_DoPointer( CHECKING_SIDE ); \
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); \
					_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , -1 ); \
					/* if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( CHECKING_SIDE , SCH_TMATM ) ) { */ /* 2014.07.10 */ \
					if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PICK_FROM_BM_FOR_6( CHECKING_SIDE , SCH_TMATM , SCH_Chamber ) ) { /* 2014.07.10 */ \
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , -1 , TRUE , ##ZZZ## + 1 , 3 ); \
					} \
				} \
				/* Sleep(1); */ /* 2008.05.19 */ \
				return TRUE; \
			} \
			/* Place to PMx */ \
			if ( SCH_SELMODE == 1 ) { \
				swmode = 0; \
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 17 ); \
				SCH_Slot = 1; \
				pt = _SCH_MODULE_Get_TM_POINTER( SCH_TMATM , SCH_No ); \
				wsa = _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No ); \
				wsb = SCH_Slot; \
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) ) { \
				case -1 : \
				/* case -3 : // 2006.06.26 */ \
				/* case -5 : // 2006.06.26 */ \
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - PM1 + 1 ); \
					return FALSE; \
					break; \
				} \
				if ( _SCH_SUB_Check_Last_And_Current_CPName_is_Different( CHECKING_SIDE , pt , SCH_Chamber ) ) { /* 2007.02.22 */ \
					if ( _SCH_PREPOST_Get_inside_READY_RECIPE_USE( CHECKING_SIDE , SCH_Chamber ) == 0 ) { \
						_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( CHECKING_SIDE * 1000 ) + SCH_Chamber , 3 ); /* 2007.02.22 */ \
					} \
					else { \
						_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( CHECKING_SIDE * 1000 ) + SCH_Chamber , 4 ); /* 2007.02.22 */ \
					} \
				} /* 2007.02.22 */ \
				j = _SCH_MACRO_PRE_PROCESS_OPERATION( CHECKING_SIDE , pt , ( ##ZZZ## * 100 ) + SCH_No , SCH_Chamber ); \
				if      ( j == 0 ) { \
					_SCH_CLUSTER_Set_PathProcessChamber_Select_Other_Disable( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Chamber ); \
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 1 ); \
					swmode = -1; \
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); \
				} \
				else if ( j == 1 ) { \
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 1 ); \
					swmode = -1; \
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); \
				} \
				else if ( j == -1 ) { \
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Handling Place Fail to %s(%d)[F%c]%cWHTM%dPLACEFAIL PM%d:%d:%d:%c\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , pt + 1 , SCH_No + 'A' , 9 , ##ZZZ## + 1 , SCH_Chamber - PM1 + 1 , pt + 1 , pt + 1 , SCH_No + 'A' ); \
					return FALSE; \
				} \
				if ( swmode != -1 ) { \
					Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); \
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING ); \
					/* Function_Result = _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , 0 , -1 , FALSE , 0 , -1 , -1 ); */ /* 2013.11.07 */ \
					Function_Result = SCHEDULING_MOVE_OPERATION( ##ZZZ## , MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , 0 , 0 ); /* 2013.11.07 */ \
					if ( Function_Result == SYS_ABORTED ) { \
						return FALSE; \
					} \
					else if ( Function_Result == SYS_ERROR ) { \
						_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 ); \
					} \
					if ( Function_Result == SYS_SUCCESS ) { \
						_SCH_MODULE_Set_PM_SIDE_POINTER( SCH_Chamber , CHECKING_SIDE , pt , 0 , 0 ); \
						_SCH_MODULE_Set_PM_PATHORDER( SCH_Chamber , 0 , SCH_Order ); \
						_SCH_MODULE_Set_PM_WAFER( SCH_Chamber , 0 , _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No ) ); \
						_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , SCH_No , 0 ); \
						_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); \
						if ( _SCH_CLUSTER_Check_HasProcessData_Post( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order ) ) { \
							if ( ( _SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( SCH_Chamber , &prcs_time , &post_time ) % 10 ) == 1 ) { \
								_SCH_CLUSTER_Set_PathProcessData_SkipPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order ); \
							} \
						} \
						_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_RUNNING2 ); \
						if ( _SCH_MACRO_MAIN_PROCESS( CHECKING_SIDE , pt , \
							  SCH_Chamber , -1 , \
							  wsa , -1 , SCH_No , -1 , \
							  _SCH_CLUSTER_Get_PathProcessRecipe( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order , 0 ) , \
							  0 , "" , PROCESS_DEFAULT_MINTIME , \
							  _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 , SCH_Order , ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) >= _SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , pt ) ) , \
							  -1 , -1 , -1 , 0 , \
							  0 , 602 ) == -1 ) { \
						} \
						/* if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( CHECKING_SIDE , SCH_TMATM ) ) { */ /* 2007.02.06 */ /* 2014.07.10 */ \
						if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( CHECKING_SIDE , SCH_TMATM , BM1 + SCH_TMATM ) ) { /* 2007.02.06 */ /* 2014.07.10 */ \
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , BM1 + SCH_TMATM , -1 , TRUE , ##ZZZ## + 1 , 3 ); \
						} \
						if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_PM_FOR_6( CHECKING_SIDE , SCH_TMATM , BM1 + SCH_TMATM + 2 ) ) { /* 2014.07.10 */ \
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , BM1 + SCH_TMATM + 2 , -1 , TRUE , ##ZZZ## + 1 , 3 ); \
						} \
					} \
					/* Sleep(1); */ /* 2008.05.19 */ \
					return TRUE; \
				} \
			} \
			/* Place to BMx */ \
			if ( SCH_SELMODE == 2 ) { \
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < 0 ) { \
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - BM1 + 1 ); \
					return FALSE; \
				} \
				/* SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , 0 ); */ /* 2013.10.30 */ /* 2014.07.10 */ \
				SCH_BM_SUPPLY_LOCK_RESET_FOR_AL6( SCH_Chamber ); /* 2013.10.30 */ /* 2014.07.10 */ \
				SCHEDULER_CONTROL_Check_BEFORE_PLACE_TO_BM_PRE_END_FOR_6( ##ZZZ## , CHECKING_SIDE , -1 , SCH_No ); /* 2007.05.03 */ \
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 16 ); \
				pt = _SCH_MODULE_Get_TM_POINTER( SCH_TMATM , SCH_No ); \
				if ( SCH_Return ) { \
					if ( _SCH_CLUSTER_Get_SwitchInOut( CHECKING_SIDE , pt ) == 1 ) { \
						_SCH_CLUSTER_Set_PathProcessData_UsedPre_Restore( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 1 ); \
						_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , pt , 0 ); \
					} \
				} \
				wsa = _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No ); \
				wsb = SCH_Slot; \
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); \
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_BM_END ); \
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ); \
				Result = SCHEDULER_CONTROL_Chk_BM_has_a_supply_wafer_to_TM_exist_FOR_6( CHECKING_SIDE , SCH_TMATM , SCH_Chamber , TRUE ); \
				/* Function_Result = _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , Result , -1 , FALSE , 0 , -1 , -1 ); */ /* 2013.11.07 */ \
				Function_Result = SCHEDULING_MOVE_OPERATION( ##ZZZ## , MACRO_PLACE , CHECKING_SIDE , pt , SCH_Chamber , SCH_No , wsa , wsb , Result , 0 ); /* 2013.11.07 */ \
				if ( Function_Result == SYS_ABORTED ) { \
					return FALSE; \
				} \
				else if ( Function_Result == SYS_ERROR ) { \
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 ); \
				} \
				if ( Function_Result == SYS_SUCCESS ) { \
					_SCH_MODULE_Set_BM_SIDE_POINTER( SCH_Chamber , SCH_Slot , CHECKING_SIDE , pt ); \
					_SCH_MODULE_Set_BM_WAFER_STATUS( SCH_Chamber , SCH_Slot , _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , SCH_No ) , BUFFER_OUTWAIT_STATUS ); \
					_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , SCH_No , 0 ); \
					_SCH_MODULE_Inc_TM_InCount( CHECKING_SIDE ); \
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); \
					_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , pt , 1 ); /* 2008.01.15 */ \
					/* if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( CHECKING_SIDE , SCH_TMATM ) ) { */ /* 2014.07.10 */ \
					if ( SCHEDULER_CONTROL_Chk_VENTING_AFTER_PLACE_TO_BM_FOR_6( CHECKING_SIDE , SCH_TMATM , SCH_Chamber ) ) { /* 2014.07.10 */ \
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , SCH_Chamber , -1 , TRUE , ##ZZZ## + 1 , 4 ); \
					} \
				} \
				/* Sleep(1); */ /* 2008.05.19 */ \
				return TRUE; \
			} \
			/* Pick from PMx */ \
			if ( SCH_SELMODE == 3 ) { \
				_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 15 ); \
				SCH_Out_Not_Run = 0; \
				pt = _SCH_MODULE_Get_PM_POINTER( SCH_Chamber , 0 ); \
				SCH_Order = _SCH_MODULE_Get_PM_PATHORDER( SCH_Chamber , 0 ); \
				switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) ) { \
				case -1 : \
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d Scheduling Aborted Because %s Abort%cWHPM%dFAIL\n" , ##ZZZ## + 1 , _SCH_SYSTEM_GET_MODULE_NAME( SCH_Chamber ) , 9 , SCH_Chamber - PM1 + 1 ); \
					return FALSE; \
					break; \
				case -2 : \
					break; \
				case -3 : \
					SCH_Out_Not_Run = 1; \
					break; \
				case -4 : \
					break; \
				case -5 : \
					SCH_Out_Not_Run = 1; \
					break; \
				} \
				SCH_Slot = 1; \
				wsa = _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 ); \
				wsb = SCH_Slot; \
				cf  = SCH_No; \
				Sav_Int1 = _SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , pt ); \
				_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , SCHEDULER_WAITING ); \
				_SCH_CLUSTER_Inc_PathDo( CHECKING_SIDE , pt ); \
				FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , pt ); \
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) { \
					Result = 0; /* 무조건 SWITCH 사용 안함 */ \
				} \
				else { \
					if ( ( _SCH_MODULE_Get_TM_Switch_Signal( SCH_TMATM ) == -1 ) || \
						( _SCH_MODULE_Get_TM_Switch_Signal( SCH_TMATM ) == SCH_Chamber ) ) { \
						if ( ( SCH_Out_Not_Run != 0 ) || ( _SCH_CLUSTER_Check_Possible_UsedPost( CHECKING_SIDE , pt , _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , pt ) - 2 , SCH_Order ) || _SCH_EQ_PROCESS_SKIPFORCE_RUN( 2 , SCH_Chamber ) ) ) { /* 2006.01.06 2006.02.02 */ \
							Result = 0; \
						} \
						else { \
							Result = SCHEDULING_CHECK_Switch_PLACE_From_Extend_TM_FOR_6( SCH_TMATM , CHECKING_SIDE , cf , SCH_Chamber ); \
						} \
					} \
					else { \
						Result = 0; \
					} \
				} \
				swmode = _SCH_SUB_SWMODE_FROM_SWITCH_OPTION( Result , 0 ); \
				/* Function_Result = _SCH_MACRO_TM_OPERATION_##ZZZ##( MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , -1 , FALSE , swmode , -1 , -1 ); */ /* 2013.11.07 */ \
				Function_Result = SCHEDULING_MOVE_OPERATION( ##ZZZ## , MACRO_PICK , CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , wsb , Result , swmode ); /* 2013.11.07 */ \
				if ( Function_Result == SYS_ABORTED ) { \
					return FALSE; \
				} \
				else if ( Function_Result == SYS_ERROR ) { \
					_SCH_CLUSTER_Set_PathStatus( CHECKING_SIDE , pt , Sav_Int1 ); \
					_SCH_CLUSTER_Dec_PathDo( CHECKING_SIDE , pt ); \
				} \
				if ( Function_Result == SYS_SUCCESS ) { \
					_SCH_MODULE_Set_TM_SIDE_POINTER( SCH_TMATM , cf , CHECKING_SIDE , pt , CHECKING_SIDE , 0 ); \
					_SCH_MODULE_Set_TM_PATHORDER( SCH_TMATM , cf , SCH_Order ); \
					_SCH_MODULE_Set_TM_WAFER( SCH_TMATM , cf , _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 ) ); \
					_SCH_MODULE_Set_PM_WAFER( SCH_Chamber , 0 , 0 ); \
					if ( Result > 0 ) { \
						_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , SCH_Chamber ); \
						SCHEDULING_CHECK_Switch_PLACE_From_Fixed_TM_and_OtherDisable_FOR_6( SCH_TMATM , CHECKING_SIDE , cf , SCH_Chamber ); /* 2006.02.02 */ \
					} \
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , -1 ); \
					if ( SCH_Out_Not_Run == 0 ) { \
						_SCH_MACRO_POST_PROCESS_OPERATION( CHECKING_SIDE , pt , ( ##ZZZ## * 100 ) + cf , SCH_Chamber ); \
					} \
				} \
				SCHEDULER_CONTROL_Check_BEFORE_PLACE_TO_BM_PRE_END_FOR_6( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , cf ); /* 2007.05.03 */ \
				/* Sleep(1); */ /* 2008.05.19 */ \
				return TRUE; \
			} \
			/* Force Vent */ \
			if ( SCH_SELMODE == -1 ) { /* 2007.10.02 */ \
				if ( _SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() != 0 ) { \
					if ( SCHEDULER_CONTROL_Check_TM_AND_PM_WAITING_FOR_NOWAFER_PRCS( CHECKING_SIDE , SCH_TMATM , BM1 + SCH_TMATM , &hasin ) ) { \
						/* if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , BM1 + SCH_TMATM ); */ /* 2013.10.30 2014.01.28 2014.07.10 */ \
						if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( BM1 + SCH_TMATM ); /* 2013.10.30 2014.01.28 2014.07.10 */ \
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , BM1 + SCH_TMATM , -1 , TRUE , 1 , 111 ); \
						/*if ( hasin ) SCH_BM_SUPPLY_LOCK_SET_FOR_AL6( SCH_TMATM , TRUE ); */ /* 2013.10.30 2014.01.28 */ \
					} \
				} \
			} \
		} \
		if ( _SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() ) { \
			if ( _SCH_MODULE_Get_TM_Move_Signal( SCH_TMATM ) == -1 ) { \
				swmode = -1; \
				for ( SCH_Chamber = PM1 ; SCH_Chamber < AL ; SCH_Chamber++ ) { \
					if ( _SCH_PRM_GET_MODULE_GROUP( SCH_Chamber ) == SCH_TMATM ) { \
						if ( _SCH_MODULE_GET_USE_ENABLE( SCH_Chamber , FALSE , CHECKING_SIDE ) ) { \
							SCH_No = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , 1 , &SCH_Order , &SCH_Ptr ); \
						} \
						else { \
							SCH_No = -1; \
						} \
						if ( SCH_No == -99 ) { \
							swmode = -1; \
							break; \
						} \
						if ( SCH_No >= 0 ) { \
							if ( swmode == -1 ) { \
								pt       = 0; \
								if ( SCH_No == 0 ) { \
									cf = 1; \
								} \
								else { \
									cf = 0; \
								} \
								swmode = SCH_Chamber; \
								Result   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , SCH_Chamber ); \
							} \
							else { \
								if ( _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , SCH_Chamber ) < Result ) { \
									pt       = 0; \
									if ( SCH_No == 0 ) { \
										cf = 1; \
									} \
									else { \
										cf = 0; \
									} \
									swmode = SCH_Chamber; \
									Result   = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , SCH_Chamber ); \
								} \
							} \
						} \
						SCH_No = SCHEDULER_CONTROL_Chk_finger_has_wafer_to_place_pm_FOR_6( ##ZZZ## , CHECKING_SIDE , SCH_Chamber , 2 , &SCH_Order , &SCH_Ptr ); \
						if ( SCH_No == -99 ) { \
							swmode = -1; \
							break; \
						} \
						if ( SCH_No >= 0 ) { \
							if ( swmode == -1 ) { \
								pt       = 1; \
								cf       = SCH_No; \
								swmode = SCH_Chamber; \
								if      ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_PRE ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < PROCESS_INDICATOR_POST ) ) \
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , SCH_Chamber ); \
								else if   ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_POST ) \
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , SCH_Chamber ); \
								else \
									timebuf = 0; \
								Result   = timebuf; \
							} \
							else { \
								if      ( ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_PRE ) && ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) < PROCESS_INDICATOR_POST ) ) \
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , SCH_Chamber ); \
								else if   ( _SCH_MACRO_CHECK_PROCESSING_INFO( SCH_Chamber ) >= PROCESS_INDICATOR_POST ) \
									timebuf = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , SCH_Chamber ); \
								else \
									timebuf = 0; \
								if ( timebuf < Result ) { \
									pt       = 1; \
									cf       = SCH_No; \
									swmode = SCH_Chamber; \
									Result   = timebuf; \
								} \
							} \
						} \
					} \
				} \
				if ( swmode != -1 ) { \
					SCH_Chamber = swmode; \
					if ( pt == 0 ) { \
						wsa = _SCH_MODULE_Get_PM_WAFER( SCH_Chamber , 0 ); \
					} \
					else { \
						wsa = _SCH_MODULE_Get_TM_WAFER( SCH_TMATM , cf ); \
					} \
					if ( _SCH_MACRO_TM_MOVE_OPERATION_##ZZZ##( CHECKING_SIDE , pt , SCH_Chamber , cf , wsa , 1 ) == SYS_ABORTED ) { \
						return FALSE; \
					} \
					_SCH_MODULE_Set_TM_Move_Signal( SCH_TMATM , SCH_Chamber ); \
					_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , -1 ); \
					/* Sleep(1); */ /* 2008.05.19 */ \
					return TRUE; \
				} \
			} \
		} \
		if ( SCHEDULER_CONTROL_Chk_RUN_HOLD_WAIT_FOR_6( ##ZZZ## , CHECKING_SIDE ) ) { \
			if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( BM1 + ##ZZZ## , FALSE , FALSE , TRUE ) ) { \
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , BM1 + ##ZZZ## , -1 , TRUE , ##ZZZ## + 1 , 101 ); \
			} \
		} \
	} \
	_SCH_MODULE_Set_TM_Switch_Signal( SCH_TMATM , -1 ); \
	/* Sleep(10); */ /* 2008.05.19 */ \
	return TRUE; \
}



MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 1 , 2 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 2 , 3 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 3 , 4 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 4 , 5 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 5 , 6 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 6 , 7 );
MAKE_SCHEDULER_MAIN_SUB_TM_for_6( 7 , 8 );
#endif
#endif


