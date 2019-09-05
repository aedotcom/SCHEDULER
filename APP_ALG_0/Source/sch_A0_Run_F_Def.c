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
#include "INF_sch_commonuser.h"
#include "INF_sch_sub.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A0_default.h"
#include "sch_A0_F.h"
#include "sch_A0_param.h"
#include "sch_A0_init.h"
#include "sch_A0_sub.h"
#include "sch_A0_subBM.h"
#include "sch_A0_sub_sp2.h" // 2005.10.07
#include "sch_A0_sub_sp3.h" // 2006.03.07
#include "sch_A0_sub_sp4.h" // 2006.03.07
#include "sch_A0_sub_F_sw.h"
#include "sch_A0_sub_F_dbl1.h"
//================================================================================
//BOOL Scheduler_Other_Arm_Cool_Return( int side , BOOL coolyes , BOOL *act ) ; // 2012.08.10 // 2015.03.25
//BOOL Scheduler_Other_Arm_Cool_Return( int side , int skipbm , BOOL coolyes , BOOL *act ) ; // 2012.08.10 // 2015.03.25 // 2016.05.02
//BOOL Scheduler_Other_Arm_Cool_Return( int side , int skipbm , BOOL coolyes , BOOL *act , int waitplacebm , int waitarm ) ; // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02
BOOL Scheduler_Other_Arm_Cool_Return( int side , int skipbm , BOOL coolyes , BOOL *act , int waitplacebm , int waitarm , BOOL noplacecm , BOOL *noplacedcm ) ; // 2012.08.10 // 2015.03.25 // 2016.05.02 // 2017.02.02

int SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( int side , int *rcm , int *rpt , int mode ); // 2015.06.25

//=========================================================================================================


BOOL SCHEDULER_COOLING_SKIP_AL0( int side , int pt ) { // 2014.08.26
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) == 2 ) return TRUE;
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) == 6 ) return TRUE; // 2014.08.26
	return FALSE;
}

BOOL SCHEDULER_ALIGN_SKIP_AL0( int side , int pt ) { // 2014.08.26
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) >= 5 ) return TRUE;
	return FALSE;
}

//=========================================================================================================
BOOL Scheduling_TM_Double( BOOL optonly ) { // 2010.07.10
	if ( optonly ) {
		if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) return TRUE;
	}
	else {
		if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
			if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) return TRUE;
		}
	}
	return FALSE;
}

//=========================================================================================================

BOOL FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28
int  FM_Place_Waiting_for_BM_style_0 = 0; // 2014.01.03


//=========================================================================================================
int Scheduling_Data_Mode_For_Pick_ArmIntlks( int ch , BOOL all ) { // 2007.08.30
	switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( ch ) ) {
	case 1 :
		return 1;
		break;
	case 2 :
		return 2;
		break;
	default :
		if ( !all ) return 3;
		break;
	}
	return 0;
}
//=========================================================================================================
//=========================================================================================================
int Scheduling_Other_Side_Pick_From_FM( int curside , int curpt , int curHSide , int *side2 , int *pointer2 , int *slot2 , int *HSide2 ) { // 2007.07.10
	int side , pt , ptx;
	int slot , slotx , HSide , HSidex;
	int cpreject , retpointer , retcm;
	//
	if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 1;
	//
	if ( ( Scheduling_TM_Double( FALSE ) ) ) {
		if ( _SCH_CLUSTER_Get_PathHSide( curside , curpt ) == HANDLING_A_SIDE_ONLY ) return 2;
		if ( _SCH_CLUSTER_Get_PathHSide( curside , curpt ) == HANDLING_B_SIDE_ONLY ) return 3;
	}
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return 4;
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return 5;
	//
	for ( side = 0 ; side < MAX_CASSETTE_SIDE ; side++ ) {
		//===========================================
		if ( side != curside ) {
			//===========================================
			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( side , 3 ); // 2008.07.16
			//===========================================
			if ( _SCH_SUB_FM_Current_No_More_Supply( side , &pt , -1 , &retcm , curside , curpt ) ) {
				continue;
			}
			//===========================================
			// 2007.08.28
			//===========================================
			if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( retcm ) == 1 ) {
				continue;
			}
			//===========================================
//			if ( SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , side , &slot , &slotx , &pt , &ptx , &HSide , &HSidex , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , 2 , &cpreject , &retpointer ) < 0 ) continue; // 2007.08.28
//			if ( SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , side , &slotx , &slot , &ptx , &pt , &HSidex , &HSide , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , 2 , &cpreject , &retpointer ) < 0 ) continue; // 2007.08.28 // 2008.11.01
			if ( _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , Scheduling_TM_Double( FALSE ) , 2 , side , &slotx , &slot , &ptx , &pt , &retpointer , &HSidex , &HSide , &cpreject ) < 0 ) continue; // 2007.08.28 // 2008.11.01
			//===========================================
			if ( Scheduling_TM_Double( FALSE ) ) {
				if ( _SCH_CLUSTER_Get_PathHSide( side , pt ) == HANDLING_A_SIDE_ONLY ) {
					_SCH_MODULE_Set_FM_DoPointer( side , retpointer );
					continue;
				}
				if ( _SCH_CLUSTER_Get_PathHSide( side , pt ) == HANDLING_B_SIDE_ONLY ) {
					_SCH_MODULE_Set_FM_DoPointer( side , retpointer );
					continue;
				}
			}
			//===========================================
			if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( side , pt ) ) == 3 ) {
				if ( SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT() <= 1 ) {
					_SCH_MODULE_Set_FM_DoPointer( side , retpointer );
					continue;
				}
			}
			//===========================================
			if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( side , pt ) ) {
				_SCH_MODULE_Set_FM_DoPointer( side , retpointer );
				continue;
			}
			//===========================================
			if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) { // 2008.06.10
				if ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( side , pt , 3 ) <= 0 ) { // 2008.04.17
					_SCH_MODULE_Set_FM_DoPointer( side , retpointer );
					continue;
				}
			}
			//===========================================
			*side2 = side;
			*pointer2 = pt;
			*slot2 = slot;
			*HSide2 = HSide;
			break;
		}
		//===========================================
	}
	//=======================================================
	if ( side == MAX_CASSETTE_SIDE ) return 6;
	//
	//----------------------------------------------------------------------------------------------------------------------------------------------
	// User Interface
	// 2016.05.10
	//----------------------------------------------------------------------------------------------------------------------------------------------
	switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PICK , 1 , 0 , 0 , 0 , side , pt , slot , 0 ) ) {
	case 0 :
		return -1;
		break;
	case 1 :
		return 11;
		break;
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------
	//
	//================================================================================================
	_SCH_SUB_Remain_for_FM( side );
	//================================================================================================
//	_SCH_CASSETTE_Reset_Side_Monitor( side , 0 ); // 2007.11.27
	_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1 );
	//----------------------------------------------------------------------------
	_SCH_SYSTEM_USING2_SET( side , 11 );
	//-----------------------------------------------
	_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
	//----------------------------------------------
	_SCH_MODULE_Inc_FM_OutCount( side );
	//----------------------------------------------
	SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( side , _SCH_CLUSTER_Get_ClusterIndex( side , pt ) ); // 2008.04.25
	//----------------------------------------------
	//----------------------------------------------------------------------
	// Code for Pick from CM
	// FM_Slot
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { // 2010.03.17
		_SCH_LOG_LOT_PRINTF( side , "FM Select Log with FIRSTLASTCHECK [P=%d,PR=(0)%d,(10)%d,(13)%d]\t\t\n" , pt , _SCH_CLUSTER_Get_PathRun( side , pt , 0 , 2 ) , _SCH_CLUSTER_Get_PathRun( side , pt , 10 , 2 ) , _SCH_CLUSTER_Get_PathRun( side , pt , 13 , 2 ) );
	}
	//----------------------------------------------------------------------
	_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( side , pt , 0 , 0 , 0 );
	//==========================================================================
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , slot , slot , side , pt , -1 ) ) {
	case -1 :
		return -1;
		break;
	}
	//----------------------------------------------------------------------
//	_SCH_MACRO_DATA_SET_FOR_ROBOT_FM( 0 , 0 , 0 , -1 , slot , side , pt , -1 , FALSE );
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , slot , slot , side , pt , -1 , -1 ); // 2007.11.26
	//----------------------------------------------------------------------
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return -1;
	//----------------------------------------------------------------------
	return 0;
}


// 2017.01.20
int Half_Placed_CM_SIDE;
int Half_Placed_CM_IC_SLOT1;
int Half_Placed_CM_IC_SLOT2;
int Half_Placed_CM_FM_CM;
int Half_Placed_CM_Slot1;
int Half_Placed_CM_Slot2;
int Half_Placed_CM_Side1;
int Half_Placed_CM_Side2;
int Half_Placed_CM_Pt1;
int Half_Placed_CM_Pt2;

int SCHEDULER_CONTROL_Run_Half_Placed_CM() { // 2017.01.20
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) {
		if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , Half_Placed_CM_SIDE , IC , Half_Placed_CM_IC_SLOT1 , Half_Placed_CM_IC_SLOT2 , Half_Placed_CM_FM_CM , -1 , TRUE , Half_Placed_CM_Slot1 , Half_Placed_CM_Slot2 , Half_Placed_CM_Side1 , Half_Placed_CM_Side2 , Half_Placed_CM_Pt1 , Half_Placed_CM_Pt2 ) == SYS_ABORTED ) {
			_SCH_LOG_LOT_PRINTF( Half_Placed_CM_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , Half_Placed_CM_Slot2 * 100 + Half_Placed_CM_Slot1 , 9 , Half_Placed_CM_Slot2 * 100 + Half_Placed_CM_Slot1 , 9 , Half_Placed_CM_Slot2 * 100 + Half_Placed_CM_Slot1 );
			return FALSE;
		}
	}
	//==========================================================================
	switch( _SCH_MACRO_FM_PLACE_TO_CM( Half_Placed_CM_SIDE , Half_Placed_CM_Slot1 , Half_Placed_CM_Side1 , Half_Placed_CM_Pt1 , Half_Placed_CM_Slot2 , Half_Placed_CM_Side2 , Half_Placed_CM_Pt2 , TRUE , TRUE , 0 ) ) {
	case -1 :
		return FALSE;
		break;
	}
	//==========================================================================
	return TRUE;
	//==========================================================================
}

//BOOL SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( int CHECKING_SIDE ) { // 2017.01.07
//int SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( int CHECKING_SIDE , BOOL outdo ) { // 2017.01.07 // 2017.01.20
int SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( int CHECKING_SIDE , BOOL noplacecm , BOOL *noplacedcm , BOOL outdo ) { // 2017.01.07 // 2017.01.20
	int k , pickarm , FM_Buffer , FM_BSide;
	int FM_Side , FM_Pointer , FM_TSlot , FM_Slot;
	int FM_Side2 , FM_Pointer2 , FM_TSlot2 , FM_Slot2;
	int FM_CM; // 2013.11.12
	//
//	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) return TRUE; // 2017.01.07
	if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) return -1; // 2017.01.07
	//
	if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.02.12
		if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) pickarm = TA_STATION;
		else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) pickarm = TB_STATION;
//		else return TRUE; // 2017.01.07
		else return -1; // 2017.01.07
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.02.12
		if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) pickarm = TA_STATION;
//		else return TRUE; // 2017.01.07
		else return -1; // 2017.01.07
	}
	else if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.02.12
		if      ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) pickarm = TB_STATION;
//		else return TRUE; // 2017.01.07
		else return -1; // 2017.01.07
	}
	//
	FM_Buffer = -1;
	//
	if ( outdo ) { // 2017.01.07
		//
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2009.02.18
				if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 1 ) ) {
					if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) continue;
					if ( _SCH_MODULE_Get_BM_WAFER( k , 1 ) <= 0 ) continue;
					//
					if ( FM_Buffer == -1 ) {
						FM_Buffer = k;
					}
					else {
						if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( k , 1 ) , _SCH_MODULE_Get_BM_POINTER( k , 1 ) ) < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , 1 ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , 1 ) ) ) {
							FM_Buffer = k;
						}
					}
				}
			}
			//
		}
		//
	}
	else { // 2017.01.07
		//
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
//				if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return TRUE; // 2017.01.07
				if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return -1; // 2017.01.07
//				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) return TRUE; // 2017.01.07
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) return -1; // 2017.01.07
			}
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2009.02.18
				if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 1 ) ) {
//					if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return TRUE; // 2017.01.07
					if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 1 ) return -1; // 2017.01.07
//					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) return TRUE; // 2017.01.07
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) <= 0 ) return -1; // 2017.01.07
					if ( FM_Buffer == -1 ) FM_Buffer = k;
				}
			}
		}
		//
	}
	//
//	if ( FM_Buffer == -1 ) return TRUE; // 2017.01.07
	if ( FM_Buffer == -1 ) return -1; // 2017.01.07
	//
	if ( pickarm == TA_STATION ) {
		FM_TSlot    = 1;
		FM_Side     = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot );
		FM_Pointer  = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot );
		FM_Slot     = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot );
		//
		FM_TSlot2   = 0;
		FM_Side2    = 0;
		FM_Pointer2 = 0;
		FM_Slot2    = 0;
		//
		FM_BSide	= FM_Side;
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ); // 2013.11.12
	}
	else {
		FM_TSlot    = 0;
		FM_Side     = 0;
		FM_Pointer  = 0;
		FM_Slot     = 0;
		//
		FM_TSlot2   = 1;
		FM_Side2    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_TSlot2 );
		FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_TSlot2 );
		FM_Slot2    = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 );
		//
		FM_BSide	= FM_Side2;
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ); // 2013.11.12
	}
	//==========================================================================
//	if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return FALSE; // 2009.04.14 // 2017.01.07
	if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return 0; // 2009.04.14 // 2017.01.07
	//==========================================================================
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , -1 ) ) {
	case -1 :
//		return FALSE;// 2017.01.07
		return 0;// 2017.01.07
		break;
	}
	//==========================================================================
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , -1 , -1 );
	//==========================================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , FM_BSide , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 1 );
	}
	//
	if ( noplacecm ) { // 2017.01.20
		*noplacedcm = TRUE;
		Half_Placed_CM_SIDE = FM_BSide;
		Half_Placed_CM_IC_SLOT1 = FM_TSlot;
		Half_Placed_CM_IC_SLOT2 = FM_TSlot2;
		Half_Placed_CM_FM_CM = FM_CM;
		Half_Placed_CM_Slot1 = FM_Slot;
		Half_Placed_CM_Slot2 = FM_Slot2;
		Half_Placed_CM_Side1 = FM_Side;
		Half_Placed_CM_Side2 = FM_Side2;
		Half_Placed_CM_Pt1 = FM_Pointer;
		Half_Placed_CM_Pt2 = FM_Pointer2;
		//
		return 1;
	}
	//
	//=============================================================================================
	// 2013.11.12
	//=============================================================================================
	if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_BSide , IC , FM_TSlot , FM_TSlot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.11
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
//		return FALSE;// 2017.01.07
		return 0;// 2017.01.07
	}
	//==========================================================================
	switch( _SCH_MACRO_FM_PLACE_TO_CM( FM_BSide , FM_Slot , FM_Side , FM_Pointer , FM_Slot2 , FM_Side2 , FM_Pointer2 , TRUE , TRUE , 0 ) ) {
	case -1 :
//		return FALSE;// 2017.01.07
		return 0;// 2017.01.07
		break;
	}
	//==========================================================================
//	return TRUE;// 2017.01.07
	return 1;// 2017.01.07
	//==========================================================================
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_DEFAULT_STYLE_0( int CHECKING_SIDE , int mode ) {
	int i; // 2016.12.13
	//
	if ( mode == 0 ) {
		//----------------------------------------------------------------------
		SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 0 );
		//----------------------------------------------------------------------
		return -1;
	}
	else {
		//----------------------------------------------------------------------
//		FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28 // 2008.09.25
		//---------------------------------------------------------------------
		//
		if ( mode == 2 ) { // 2016.12.13
			//
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
				//
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					//
					if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) continue;
					//
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_WAIT_STATION ) ) continue;
					//
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) != 0 ) continue;
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_IN_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_OUT_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_IN_S_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					else if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( _SCH_PRM_GET_MODULE_GROUP( i ) , i ) == BUFFER_OUT_S_MODE ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , i , -1 , TRUE , 0 , -1 );
					}
					//
				}
			}
			//
		}
		//
		return -1;
	}
	//
}


/*BOOL SCHEDULING_SIDE_ORDER_CHECK_FAST( int bmo , int bmo_slot1 , int bmo_slot2 , int bmn , int bmn_slot1 , int bmn_slot2 ) { // 2012.02.03
	int s , p , sp;
	//
	if ( bmo <= 0 ) return TRUE;
	if ( bmn <= 0 ) return TRUE;
	//
	if ( bmo_slot1 > 0 ) {
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmo , bmo_slot1 );
		p = _SCH_MODULE_Get_BM_POINTER( bmo , bmo_slot1 );
		//
		sp = _SCH_CLUSTER_Get_SupplyID( s , p );
		//
	}
	else if ( bmo_slot2 > 0 ) {
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmo , bmo_slot2 );
		p = _SCH_MODULE_Get_BM_POINTER( bmo , bmo_slot2 );
		//
		sp = _SCH_CLUSTER_Get_SupplyID( s , p );
		//
	}
	else {
		return TRUE;
	}
	//
	if ( bmn_slot1 > 0 ) {
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmn , bmn_slot1 );
		p = _SCH_MODULE_Get_BM_POINTER( bmn , bmn_slot1 );
		//
		if ( sp < _SCH_CLUSTER_Get_SupplyID( s , p ) ) return FALSE;
		//
	}
	else if ( bmn_slot2 > 0 ) {
		//
		s = _SCH_MODULE_Get_BM_SIDE( bmn , bmn_slot2 );
		p = _SCH_MODULE_Get_BM_POINTER( bmn , bmn_slot2 );
		//
		if ( sp < _SCH_CLUSTER_Get_SupplyID( s , p ) ) return FALSE;
		//
	}
	else {
		return TRUE;
	}
	//
	return TRUE;
}
*/




//
//========================================================================================
//
// START 2017.01.06
//
int SWAP_2FM_TH_AL( int sidex , int *FM_Slot , int *FM_Side , int *FM_Pointer , int *FM_Slot2 , int *FM_Side2 , int *FM_Pointer2 , int bm , int FM_XBuffer ) {
	//
	int Res , FM_CM = -1;
	int xFM_Slot , xFM_Side , xFM_Pointer , xFM_Slot2 , xFM_Side2 , xFM_Pointer2;
	int lside; // 2019.01.28
	//
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
		//
		// pick al
		//
		if ( *FM_Slot <= 0 ) {
			xFM_Slot = _SCH_MODULE_Get_MFAL_WAFER();
			xFM_Side = _SCH_MODULE_Get_MFAL_SIDE();
			xFM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
			xFM_Slot2 = 0;
			xFM_Side2 = 0;
			xFM_Pointer2 = 0;
			//
			lside = xFM_Side; // 2019.01.28
			//
		}
		else if ( *FM_Slot2 <= 0 ) {
			xFM_Slot2 = _SCH_MODULE_Get_MFAL_WAFER();
			xFM_Side2 = _SCH_MODULE_Get_MFAL_SIDE();
			xFM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER();
			xFM_Slot = 0;
			xFM_Side = 0;
			xFM_Pointer = 0;
			//
			lside = xFM_Side2; // 2019.01.28
			//
		}
		else {
			return 0;
		}
		//
		Res = _SCH_MACRO_CHECK_FM_MALIGNING( lside , TRUE );
		//
		if ( Res == SYS_ABORTED ) {
			//
			_SCH_LOG_LOT_PRINTF( lside , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
			//
			return 0;
		}
		//
		//==========================================================================================
		if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , lside , AL , xFM_Slot , xFM_Slot2 , FM_CM , -1 , TRUE , xFM_Slot , xFM_Slot2 , xFM_Side , xFM_Side2 , xFM_Pointer , xFM_Pointer2 ) == SYS_ABORTED ) {
			_SCH_LOG_LOT_PRINTF( lside , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , xFM_Slot2 * 100 + xFM_Slot , 9 , xFM_Slot2 * 100 + xFM_Slot , 9 , xFM_Slot2 * 100 + xFM_Slot );
			return 0;
		}
		//==========================================================================================
		_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , AL , xFM_Slot , 1 , xFM_Side , xFM_Pointer , AL , xFM_Slot2 , 1 , xFM_Side2 , xFM_Pointer2 , -1 , -1 );
		//
		Res = -1;
		//
	}
	else {
		//
		Res = 1;
		//
	}
	//
	// place al
	//
	if ( *FM_Slot > 0 ) {
		FM_CM = _SCH_CLUSTER_Get_PathIn( *FM_Side , *FM_Pointer ); // 2017.01.23
		//
		lside = *FM_Side; // 2019.01.28
		//
	}
	else if ( *FM_Slot2 > 0 ) {
		FM_CM = _SCH_CLUSTER_Get_PathIn( *FM_Side2 , *FM_Pointer2 ); // 2017.01.23
		//
		lside = *FM_Side2; // 2019.01.28
		//
	}
	else {
		//
		lside = sidex; // 2019.01.28
		//
	}
	//
	if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , lside , AL , *FM_Slot , *FM_Slot2 , FM_CM , -1 , TRUE , *FM_Slot , *FM_Slot2 , *FM_Side , *FM_Side2 , *FM_Pointer , *FM_Pointer2 ) == SYS_ABORTED ) {
		return 0;
	}
	//----------------------------------------------
	_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
	//-----------------------------------------------
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , *FM_Slot , 1 , *FM_Side , *FM_Pointer , AL , *FM_Slot2 , 1 , *FM_Side2 , *FM_Pointer2 , bm , bm );
	//
	// aligning
	//
	if ( *FM_Slot > 0 ) {
		//============================================================================
		// Aligning
		//============================================================================
		_SCH_MACRO_SPAWN_FM_MALIGNING( *FM_Side , *FM_Slot , 0 , FM_CM , FM_XBuffer );
		//============================================================================
	}
	else if ( *FM_Slot2 > 0 ) {
		//============================================================================
		// Aligning
		//============================================================================
		_SCH_MACRO_SPAWN_FM_MALIGNING( *FM_Side2 , 0 , *FM_Slot2 , FM_CM , FM_XBuffer );
		//============================================================================
	}
	//
	if ( Res == -1 ) {
		//
		*FM_Slot = xFM_Slot;
		*FM_Side = xFM_Side;
		*FM_Pointer = xFM_Pointer;
		*FM_Slot2 = xFM_Slot2;
		*FM_Side2 = xFM_Side2;
		*FM_Pointer2 = xFM_Pointer2;
		//
	}
	//
	return Res;
}


//
// END 2017.01.06
//
//========================================================================================
//


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FEM Scheduling
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//=====================================================================================================================
//
BOOL Scheduling_PT_and_1S_Same() { // 2016.11.01
	//
	if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 > 0 ) {
		//
		if ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) / 10 < 4 ) return FALSE;
		//
		return TRUE;
		//
	}
	//
	return FALSE;
	//
}
//
//=====================================================================================================================
//
BOOL Scheduling_Waiting_for_AL_Supply() { // 2019.01.29
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 1 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 3 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 5 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) ) { // not delay
		//
		if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) { // 2019.01.20
			//
			if ( SYS_SUCCESS == _SCH_MACRO_CHECK_FM_MALIGNING( _SCH_MODULE_Get_MFAL_SIDE() , FALSE ) ) {
				return TRUE;
			}
			//
		}
	}
	//
	return FALSE;
}

//=====================================================================================================================
//
int SUPPLY_CHECK_WAITING_TAG[MAX_CASSETTE_SIDE]; // 2018.08.14
//
BOOL Scheduler_Other_Side_Supply_Waiting_Cool_Cancel( int side ) { // 2018.08.14
	//
	if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 1 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 3 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 5 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) ) { // not delay
		//
		if ( Scheduling_Waiting_for_AL_Supply() ) { // 2019.01.30
			//
			SUPPLY_CHECK_WAITING_TAG[side] = 0;
			//
			return TRUE;
		}
		//
		if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) {
			//
			SUPPLY_CHECK_WAITING_TAG[side] = 0;
			//
			return FALSE;
		}
		//
		SUPPLY_CHECK_WAITING_TAG[side]++;
		//
		if ( SUPPLY_CHECK_WAITING_TAG[side] < 10 ) return TRUE;
		//
		SUPPLY_CHECK_WAITING_TAG[side] = 0;
		//
		return FALSE;
		//
	}
	//
	SUPPLY_CHECK_WAITING_TAG[side] = 0;
	//
	return FALSE;
}

//
//=====================================================================================================================
//
int USER_DLL_SCH_INF_RUN_CONTROL_FEM_DEFAULT_STYLE_0( int CHECKING_SIDE ) {
	int	FM_Slot;
	int	FM_Slot2;
	int	FM_TSlot , FM_OSlot , FM_CO1 , FM_CO2;
	int	FM_TSlot2 , FM_OSlot2;
	int	FM_Pointer;
	int	FM_Pointer2;
	//
	int	FM_XSlot;
	int	FM_XSlot2;
	int	FM_XTSlot;
	int	FM_XTSlot2;
	int	FM_XPointer;
	int	FM_XPointer2;
	int FM_XBuffer;
	//
	int	FM_Side;
	int	FM_Side2;
	int	FM_HSide;
	int	FM_HSide2;
	int	FM_TSX;
	int	FM_TSX2;
	int i , j , k , l;
	int FM_Buffer;
	int FM_Total_Out_Count;
	int FM_Total_In_Count;
	int FM_Total_Dbl_Count;
	int	RunMaxSize;
	int	RunBuffer;
	int	RemainTime;
	int Result;
	int Runmode;
	int FM_CM;
	BOOL Cont_True;
	BOOL Out_First;
	BOOL Referesh_True;
	BOOL CM_Place;
	BOOL Check_Flag;
	int retpointer;
	int cpreject;
	BOOL PlaceCheck; // 2004.09.09
	int ret_rcm , ret_pt; // 2007.02.01
	int FM_gpmc; // 2007.02.16
	int armbchk; // 2007.08.30
	int coolplace; // 2008.02.20
	int FM_AL_Buffer; // 2010.07.07
	//-----------------------------------------------
	int pmslot;
	//
	//========================================================================================
	//
	// START 2017.01.06
	//
	int FM_RejectBuffer , FM_TSlotBuf1 , FM_TSlotBuf2;
	//
	// END 2017.01.06
	//
	//========================================================================================
	//
	BOOL Half_CM_Placed; // 2017.01.20
	//
	//----------------------------------------------------------------------
	FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28 // 2008.09.25
	FM_Place_Waiting_for_BM_style_0 = 0; // 2014.01.03
	//---------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM BM & PLACE TO CM PART
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	Out_First = TRUE;

	//
	FM_CM = SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE );
	//
	if ( SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) == 0 ) {
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
				if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( k , &i , 0 ) > 0 ) {
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
							Out_First = FALSE;
						}
					}
					else { // 2008.10.17
						if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 1 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 3 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 5 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) ) { // not delay 2008.10.17
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2009.01.21
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) {
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) <= 0 ) ) {
										Out_First = FALSE;
									}
								}
							}
						}
					}
				}
			}
		}
		if ( !Out_First ) {
			SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 2 );
		}
	}
	else if ( SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) == 1 ) { // moverecv
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
				if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( k , &i , 0 ) > 0 ) {
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.07
							Out_First = FALSE;
						}
					}
				}
			}
		}
		if ( !Out_First ) {
			SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 3 );
		}
	}
	else if ( SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) == 2 ) {
		SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 0 );
	}
	else if ( SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) == 3 ) {
		SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 1 );
	}
	Cont_True = FALSE;
	Referesh_True = FALSE;
	CM_Place = FALSE;
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 CHECK_GETOUT_PART [Out_First=%d][%d][%d]\n" , Out_First , FM_CM , SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) );
	//----------------------------------------------------------------------
	//
	while( Out_First ) {
		//----------------------------------------------------------------------
//		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 CHECK_GETOUT_PART [Out_First=%d][%d][%d]\n" , Out_First , FM_CM , SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) );
		//----------------------------------------------------------------------
		FM_Buffer = -1;
		FM_CO1 = 0;
		FM_CO2 = 0;

		cpreject = 0; // 2007.10.15

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
				k = Get_Prm_MODULE_BUFFER_SINGLE_MODE();
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( k , i , &FM_CO1 , &FM_CO2 , 2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ) > 0 ) {
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							FM_Buffer = k;
							FM_Slot2  = FM_CO1;
							FM_Slot   = 0;
						}
					}
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_Buffer = k;
							FM_Slot   = FM_CO1;
							FM_Slot2  = 0;
						}
					}
				}
			}
			else {
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 1 ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( k , i , &FM_CO1 , &FM_CO2 , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ) > 0 ) {
								if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
										//==========================================================================
										// 2007.08.28
										//==========================================================================
										if      ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 1 ) {
											FM_Buffer = k;
											FM_Slot   = FM_CO1;
											FM_Slot2  = 0;
										}
										else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 2 ) {
											FM_Buffer = k;
											FM_Slot   = 0;
											FM_Slot2  = FM_CO1;
										}
										else {
											FM_Buffer = k;
											FM_Slot   = FM_CO1;
											FM_Slot2  = FM_CO2;
										}
										//==========================================================================
										break;
									}
									else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
										//==========================================================================
										// 2007.08.28
										//==========================================================================
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) != 2 ) {
											FM_Buffer = k;
											FM_Slot   = FM_CO1;
											FM_Slot2  = 0;
											break;
										}
										//==========================================================================
									}
									else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
										//==========================================================================
										// 2007.08.28
										//==========================================================================
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) != 1 ) {
											FM_Buffer = k;
											FM_Slot2  = FM_CO1;
											FM_Slot   = 0;
											break;
										}
										//==========================================================================
									}
								}
								else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
										//==========================================================================
										// 2007.08.28
										//==========================================================================
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) != 2 ) {
											FM_Buffer = k;
											FM_Slot   = FM_CO1;
											FM_Slot2  = 0;
											break;
										}
										//==========================================================================
									}
								}
								else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
										//==========================================================================
										// 2007.08.28
										//==========================================================================
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) != 1 ) {
											FM_Buffer = k;
											FM_Slot2  = FM_CO1;
											FM_Slot   = 0;
											break;
										}
										//==========================================================================
									}
								}
							}
						}
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 10-a CHECK_GET_OUT = (i=%d) FM_CM(k=%d,%d,%d) Slot(%d,%d) Buffer(%d)\n" , i , k , FM_CO1 , FM_CO2 , FM_Slot , FM_Slot2 , FM_Buffer );
			//----------------------------------------------------------------------
			if ( FM_Buffer >= 0 ) {
				if ( i == CHECKING_SIDE ) {
					break;
				}
				else {
					FM_Buffer = -1;
					cpreject = 1; // 2007.10.15
//						Cont_True = TRUE; // 2007.10.15
//						break; // 2007.10.15
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 10 CHECK_GET_OUT = Slot(%d,%d) Buffer(%d)\n" , FM_Slot , FM_Slot2 , FM_Buffer );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		// 2007.10.04
		//----------------------------------------------------------------------
		if ( FM_Buffer >= 0 ) {
			if ( Scheduling_TM_Double( FALSE ) ) {
				//===============================================================================================
				if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
//					if ( SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( FM_Buffer , FM_Slot , &FM_Slot2 ) == 2 ) { // 2018.06.18
					if ( SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( FM_Buffer , FM_Slot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_Buffer ) ) , &FM_Slot2 ) == 2 ) { // 2018.06.18
						FM_Buffer = -1;
						Referesh_True = TRUE;
					}
				}
				//===============================================================================================
			}
		}
		else {
			if ( cpreject == 1 ) { // 2007.10.15
				Cont_True = TRUE; // 2007.10.15
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 CHECK_GET_OUT = Slot(%d,%d) Buffer(%d)\n" , FM_Slot , FM_Slot2 , FM_Buffer );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
			if ( FM_Buffer >= 0 ) {
				i = _SCH_MODULE_Chk_MFIC_FREE_SLOT( &FM_CO1 , &FM_CO2 , -1 , -1 );
				if ( i > 0 ) {
					if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
						if ( i == 1 ) FM_Slot2 = 0;
					}
					else if ( FM_Slot > 0 ) {
					}
					else if ( FM_Slot2 > 0 ) {
					}
				}
				else {
					FM_Buffer = -1;
				}
			}
		}
		//----------------------------------------------------------------------
		if ( FM_Buffer >= 0 ) {
			//-----------------------------------------------
			_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 21 );
			//-----------------------------------------------
			_SCH_MODULE_Set_FM_MidCount( 2 );
			//-----------------------------------------------
			SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 0 );
			//-----------------------------------------------
			_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 1 );
			//----------------------------------------------------------------------------
			if ( Scheduling_TM_Double( FALSE ) ) {
//					//===============================================================================================
//					// 2007.07.11 // 2007.10.04
//					//===============================================================================================
//					if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
//						SCHEDULER_CONTROL_Chk_BM_TMDOUBLE_OTHERSIDE_OUT( FM_Buffer , FM_Slot , &FM_Slot2 );
//					}
				//===============================================================================================
				if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
					FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
					FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , FM_Pointer );
					if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_OSlot ) ) {
						FM_TSX   = FM_Slot;
						FM_Slot  = FM_Slot2;
						FM_Slot2 = FM_TSX;
						//
						FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
						FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , FM_Pointer );
						if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_OSlot ) ) {
							FM_Slot = 0;
						}
					}
					else {
						FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 );
						FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , FM_Pointer );
						if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , FM_OSlot ) ) {
							FM_Slot2 = 0;
						}
					}
				}
				else if ( FM_Slot > 0 ) {
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
						FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , FM_Pointer );
						if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_OSlot ) ) {
							FM_Slot2 = FM_Slot;
							FM_Slot  = 0;
						}
					}
				}
				else if ( FM_Slot2  > 0 ) {
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 );
						FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( CHECKING_SIDE , FM_Pointer );
						if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , FM_OSlot ) ) {
							FM_Slot  = FM_Slot2;
							FM_Slot2 = 0;
						}
					}
				}
			}
			//----------------------------------------------------------------------------
			//----------------------------------------------
			if ( FM_Slot  > 0 ) {
				FM_Side = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ); // 2007.07.11
				FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
				FM_TSlot = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot );
			}
			else {
				FM_TSlot = 0;
				FM_Pointer = 0;
			}
			if ( FM_Slot2  > 0 ) {
				FM_Side2 = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 ); // 2007.07.11
				FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 );
				FM_TSlot2 = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot2 );
			}
			else {
				FM_TSlot2 = 0;
				FM_Pointer2 = 0;
			}
			//
			// Testing
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2011.07.11
				//==================================================================================================================
				if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
					i = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( FM_Side , FM_Pointer , FM_Side2 , FM_Pointer2 , &FM_CO1 , &FM_CO2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
				}
				else {
					i = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , -1 , -1 , &FM_CO1 , &FM_CO2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) );
					if ( FM_Slot2 > 0 ) FM_CO2 = FM_CO1;
				}
				//==================================================================================================================
				if ( i <= 0 ) {
					FM_Buffer = -1;
				}
				else {
					if ( !_SCH_MODULE_Chk_MFIC_MULTI_FREE( FM_CO1 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) {
						FM_Buffer = -1;
					}
				}
				//==================================================================================================================
			}
			//
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 START_PICK_from_BM = [%d] Slot(%d,%d) Pointer(%d,%d)\n" , FM_Buffer , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		if ( FM_Buffer >= 0 ) { // 2011.07.11
			//----
			// Code for Pick from BO
			// FM_Slot
			//----------------------------------------------------------------------
			//
			k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 );
			if ( FM_Slot > 0 ) k = k - 1;
			if ( FM_Slot2 > 0 ) k = k - 1;
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
				if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
					if ( _SCH_MODULE_Get_BM_MidCount() != 1 ) {
						if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 1  ) == SYS_ABORTED ) {
							return 0;
						}
						_SCH_MODULE_Set_BM_MidCount( 1 );
					}
				}
			}
			//==========================================================================
			if ( _SCH_MACRO_CHECK_PROCESSING( FM_Buffer ) < 0 ) return 0; // 2009.04.14
			//==========================================================================
			switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_TSlot , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , ( k <= 0 ) ) ) { // 2007.03.21 // 2007.07.11
			case -1 :
				return 0;
				break;
			}
			//==========================================================================
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_TSlot , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , -1 );
			//==========================================================================
			if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( FM_Buffer ) ) {
					if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 1 );
					}
					else {
						_SCH_MODULE_Set_BM_FULL_MODE( FM_Buffer , BUFFER_TM_STATION ); // 2006.12.07
					}
				}
			}
			//----------------------------------------------------------------------
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1 from FEM%cWHFMFAIL 1\n" , 9 );
				return 0;
			}
			//----------------------------------------------------------------------
			//-----------------------------------------------
			_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 22 );
			//-----------------------------------------------
			_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
			//----------------------------------------------
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 START_PLACE_to_CM = Pointer(%d,%d)\n" , FM_Pointer , FM_Pointer2 );
			//----------------------------------------------------------------------
			//###################################################################################
			// Disapear Check (2001.11.28) // 2002.06.07
			//###################################################################################
			if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
				if ( FM_Slot > 0 ) {
					if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
						FM_Slot = 0;
						FM_TSlot = 0;
					}
				}
				if ( FM_Slot2 > 0 ) {
					if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
						FM_Slot2 = 0;
						FM_TSlot2 = 0;
					}
				}
			}
			if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
				Referesh_True = TRUE;
				break;
			}
			//###################################################################################

			//=========================================================================================================
			// 2006.05.27
			//=========================================================================================================
			if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
				if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) {
					FM_XBuffer = -1;
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						if ( k != FM_Buffer ) {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) {
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 1 ) ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( k , CHECKING_SIDE , &FM_CO1 , &FM_CO2 , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ) > 0 ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
											if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
												FM_XBuffer = k;
												FM_XSlot   = FM_CO1;
												FM_XSlot2  = 0;
												break;
											}
											else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
												FM_XBuffer = k;
												//================================================================================
												// 2006.12.08
												//================================================================================
//													FM_XSlot   = FM_CO1;
//													FM_XSlot2  = 0;
												//================================================================================
												FM_XSlot   = 0;
												FM_XSlot2  = FM_CO1;
												//================================================================================
												break;
											}
										}
									}
								}
							}
						}
					}
					if ( FM_XBuffer >= 0 ) {
						if ( FM_XSlot  > 0 ) {
							FM_XPointer = _SCH_MODULE_Get_BM_POINTER( FM_XBuffer , FM_XSlot );
							FM_XTSlot = _SCH_MODULE_Get_BM_WAFER( FM_XBuffer , FM_XSlot );
							//
							FM_XTSlot2 = 0;
							FM_XPointer2 = 0;
						}
						else if ( FM_XSlot2  > 0 ) {
							FM_XPointer2 = _SCH_MODULE_Get_BM_POINTER( FM_XBuffer , FM_XSlot2 );
							FM_XTSlot2 = _SCH_MODULE_Get_BM_WAFER( FM_XBuffer , FM_XSlot2 );
							//
							FM_XTSlot = 0;
							FM_XPointer = 0;
						}
						//----
						// Code for Pick from BO
						// FM_XSlot
						//----------------------------------------------------------------------
						k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_XBuffer , -1 );
						if ( FM_XSlot > 0 ) k = k - 1;
						if ( FM_XSlot2 > 0 ) k = k - 1;
						//
						//==========================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING( FM_XBuffer ) < 0 ) return 0; // 2009.04.14
						//==========================================================================
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , FM_XBuffer , FM_XTSlot , FM_XSlot , CHECKING_SIDE , FM_XPointer , FM_XBuffer , FM_XTSlot2 , FM_XSlot2 , CHECKING_SIDE , FM_XPointer2 , ( k <= 0 ) ) ) { // 2007.03.21
						case -1 :
							return 0;
							break;
						}
						//==========================================================================
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_XBuffer , FM_XTSlot , FM_XSlot , CHECKING_SIDE , FM_XPointer , FM_XBuffer , FM_XTSlot2 , FM_XSlot2 , CHECKING_SIDE , FM_XPointer2 , -1 , -1 );
						//----------------------------------------------------------------------
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_XBuffer ) ) {
							if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( FM_XBuffer ) ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_XBuffer , ( FM_XTSlot > 0 ) ? FM_XTSlot : FM_XTSlot2 , TRUE , 0 , 11 );
							}
						}
						//----------------------------------------------------------------------
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 2 from FEM%cWHFMFAIL 2\n" , 9 );
							return 0;
						}
						//----------------------------------------------------------------------
						if ( FM_XSlot > 0 ) {
							FM_Slot = FM_XSlot;
							FM_TSlot = FM_XTSlot;
							FM_Pointer = FM_XPointer;
						}
						else if ( FM_XSlot2 > 0 ) {
							FM_Slot2 = FM_XSlot2;
							FM_TSlot2 = FM_XTSlot2;
							FM_Pointer2 = FM_XPointer2;
						}
						//----------------------------------------------------------------------
					}
				}
			}
			//=========================================================================================================
			coolplace = 0;
			//
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2008.02.20
				//
				if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
					if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) coolplace = 2;
					if ( SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) coolplace = 2;
				}
				//
				if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
					if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) coolplace = 2;
					if ( SCHEDULER_COOLING_SKIP_AL0( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) coolplace = 2;
				}
			}
			else {
				coolplace = 1;
			}
			//
			if ( coolplace == 0 ) {
				//
				// Place Cool
				//
				if      ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
				}
				else if ( FM_TSlot > 0 ) {
					FM_CO2 = 0;
				}
				else if ( FM_TSlot2 > 0 ) {
					FM_CO2 = FM_CO1;
					FM_CO1 = 0;
				}
				//----------------------------------------------------------------------
				//=============================================================================================
				// 2006.11.14
				//=============================================================================================
				FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
				FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
				FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2007.07.11
				FM_Side2    = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2007.07.11
				FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION ); // 2007.07.11
				FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION ); // 2007.07.11
				FM_CM		= -1; // 2008.06.04
				//=============================================================================================
				if ( FM_TSlot > 0 ) {
//					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , FM_CO1 , 0 , FM_CM , -1 , TRUE , FM_Slot , 0 , FM_Side , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2007.07.11 // 2019.01.20
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_Side , IC , FM_CO1 , 0 , FM_CM , -1 , TRUE , FM_Slot , 0 , FM_Side , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2007.07.11 // 2019.01.20
						_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
						return 0;
					}
					//----------------------------------------------------------------------
/*
// 2007.11.26
//						SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2007.07.11
//						SCHEDULER_CONTROL_Set_MFICx_SIDE_POINTER( FM_CO1 , CHECKING_SIDE , FM_Pointer ); // 2007.07.11
					SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TA_STATION , FM_Side , 0 ); // 2007.07.11
					SCHEDULER_CONTROL_Set_MFICx_SIDE_POINTER( FM_CO1 , FM_Side , FM_Pointer ); // 2007.07.11
					SCHEDULER_CONTROL_Set_MFICx_WAFER( FM_CO1 , FM_Slot );
*/
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , FM_Slot , FM_CO1 , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 , -1 );
					//
					_SCH_MODULE_Set_MFIC_RUN_TIMER( FM_CO1 , 0 , _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , FALSE );
				}
				if ( FM_TSlot2 > 0 ) {
//					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , 0 , FM_CO2 , FM_CM , -1 , TRUE , 0 , FM_Slot2 , 0 , FM_Side2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.11 // 2019.01.20
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_Side2 , IC , 0 , FM_CO2 , FM_CM , -1 , TRUE , 0 , FM_Slot2 , 0 , FM_Side2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.11 // 2019.01.20
						_SCH_LOG_LOT_PRINTF( FM_Side2 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
						return 0;
					}
					//----------------------------------------------------------------------
/*
// 2007.11.26
//						SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2007.07.11
//						SCHEDULER_CONTROL_Set_MFICx_SIDE_POINTER( FM_CO2 , CHECKING_SIDE , FM_Pointer2 ); // 2007.07.11
					SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , FM_Side2 , 0 ); // 2007.07.11
					SCHEDULER_CONTROL_Set_MFICx_SIDE_POINTER( FM_CO2 , FM_Side2 , FM_Pointer2 ); // 2007.07.11
					SCHEDULER_CONTROL_Set_MFICx_WAFER( FM_CO2 , FM_Slot2 );
*/
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , IC , FM_Slot2 , FM_CO2 , FM_Side2 , FM_Pointer2 , -1 , -1 );
					//
					_SCH_MODULE_Set_MFIC_RUN_TIMER( FM_CO2 , 0 , _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , FALSE );
				}
				//----------------------------------------------------------------------
				//###################################################################################
				// Disapear Check (2001.11.28)
				//###################################################################################
				if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( FM_Slot > 0 ) {
						if ( _SCH_MODULE_Get_MFIC_WAFER( FM_CO1 ) <= 0 ) {
							FM_Slot = 0;
							FM_TSlot = 0;
						}
					}
					if ( FM_Slot2 > 0 ) {
						if ( _SCH_MODULE_Get_MFIC_WAFER( FM_CO2 ) <= 0 ) {
							FM_Slot2 = 0;
							FM_TSlot2 = 0;
						}
					}
					if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
						Referesh_True = TRUE;
						break;
					}
				}
				//###################################################################################
				//----------------------------------------------------------------------
			}
			else {
				//
				// Code Cooling Before Place to CM
				//
				//----------------------------------------------------------------------
				//=============================================================================================
				// 2006.11.14
				//=============================================================================================
				FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
				FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
				FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2007.07.11
				FM_Side2    = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2007.07.11
				FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION ); // 2007.07.11
				FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION ); // 2007.07.11
				//=============================================================================================
				if ( coolplace == 1 ) {
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , IC , FM_TSlot , FM_TSlot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.11
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
						return 0;
					}
				}
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
					return 0;
				}
				//----------------------------------------------------------------------
				//###################################################################################
				// Disapear Check (2001.11.28) // 2002.06.07
				//###################################################################################
				if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( FM_Slot > 0 ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_Slot = 0;
							FM_TSlot = 0;
						}
					}
					if ( FM_Slot2 > 0 ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							FM_Slot2 = 0;
							FM_TSlot2 = 0;
						}
					}
				}
				if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
					Referesh_True = TRUE;
					break;
				}
				//###################################################################################
				//----------------------------------------------------------------------
				//==========================================================================
				//----------------------------------------------------------------------
				//
				// Code for Place to CM
				// FM_Slot
				//----
				//----------------------------------------------------------------------
				Result = _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , TRUE , TRUE , 0 ); // 2007.07.11
				if      ( Result == -1 ) { // Abort
					return 0;
				}
				else if ( Result != 0 ) { // disappear
					Referesh_True = TRUE;
					break;
				}
				//=================================================================================
				CM_Place = TRUE;
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 13 END_PLACE_to_CM = TSlot(%d,%d) OutCass(%d,%d) OutSlot(%d)\n" , FM_TSlot , FM_TSlot2 , FM_CO1 , FM_CO2 , FM_OSlot );
				//----------------------------------------------------------------------
			}
			Cont_True = TRUE;
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) break;
			if ( Scheduling_TM_Double( FALSE ) ) break;
			//
			if ( Scheduling_Waiting_for_AL_Supply() ) { // 2019.01.29
				//
				Out_First = FALSE;
				break;
				//
			}
			//
			//
		}
		else {
			break;
		}
	}
	//================================================================================================================================================================
	if ( Referesh_True ) {
		return 1;
	}
	//================================================================================================================================================================
	if ( CM_Place ) { // 2002.07.11
//			if ( !_SCH_SYSTEM_MODE_WAITR_GET( CHECKING_SIDE ) ) { // 2005.07.25 // 2005.09.15
		if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 99 FINISH_CHECK_2 = TRUE\n" );
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 3 for FEM Module%cWHFMSUCCESS 3\n" , 9 );
			return SYS_SUCCESS;
		}
	}
	//================================================================================================================================================================
	if ( Cont_True && Out_First ) {
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		// PLACE TO CM from MIC
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
			//
			//
			if ( !Scheduler_Other_Side_Supply_Waiting_Cool_Cancel( CHECKING_SIDE ) ) { // 2018.09.04
				//
				//
				//Pick M_FIC & Place CM
	//			if ( _SCH_MODULE_Chk_MFIC_HAS_OUT_SIDE( FALSE , CHECKING_SIDE , &FM_Side2 , &FM_TSlot , &FM_TSlot2 ) > 0 ) { // 2011.07.11 (Testing)
				if ( _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &FM_Side2 , &FM_TSlot , &FM_TSlot2 ) == SYS_SUCCESS ) { // 2011.07.11 (Testing)
					if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
						//===========================================================================
						// 2006.11.14
						//===========================================================================
						if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
								FM_TSlot = 0;
								FM_TSlot2 = 0;
							}
							else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) {
								FM_TSlot2 = FM_TSlot;
								FM_TSlot = 0;
							}
							else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
								FM_TSlot2 = 0;
							}
						}
						else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) FM_TSlot = 0;
							FM_TSlot2 = 0;
						}
						else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
							if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) FM_TSlot2 = 0;
						}
						else {
							FM_TSlot = 0;
							FM_TSlot2 = 0;
						}
						//===========================================================================
						//if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
						//	FM_TSlot = 0;
						//}
						//if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						//	FM_TSlot2 = 0;
						//}
						//===========================================================================
					}
					else {
	//						if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2006.11.14
						if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2006.11.14
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
						}
	//						if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2006.11.14
						if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.11.14
							FM_TSlot2 = 0;
						}
					}
					//----------------------------------------------
					if ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) { // 2006.11.14
						if ( FM_TSlot > 0 ) {
	// Move Down 2004.07.06
	//						_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE );
							FM_Side    = _SCH_MODULE_Get_MFIC_SIDE( FM_TSlot ); // 2007.07.11
							FM_Pointer = _SCH_MODULE_Get_MFIC_POINTER( FM_TSlot );
							FM_Slot    = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot ); // 2005.11.29
						}
						else {
							FM_Side = 0; // 2007.07.11
							FM_Pointer = 0;
							FM_Slot = 0; // 2005.11.29
						}
						if ( FM_TSlot2 > 0 ) {
	// Move Down 2004.07.06
	//						_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE );
							FM_Side2    = _SCH_MODULE_Get_MFIC_SIDE( FM_TSlot2 ); // 2007.07.11
							FM_Pointer2 = _SCH_MODULE_Get_MFIC_POINTER( FM_TSlot2 );
							FM_Slot2    = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot2 ); // 2005.11.29
						}
						else {
							FM_Side2 = 0; // 2007.07.11
							FM_Pointer2 = 0;
							FM_Slot2 = 0; // 2005.11.29
						}
	//						if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) { // 2007.03.19
	//							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2007.03.19
	//						} // 2007.03.19
	//						else if ( FM_TSlot > 0 ) { // 2007.03.19
						if ( FM_TSlot > 0 ) {
	//							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2007.07.11
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ); // 2007.07.11
						}
						else if ( FM_TSlot2 > 0 ) {
	//							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 ); // 2007.07.11
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ); // 2007.07.11
						}
						//-----------------------------------------------
						_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 22 );
						//-----------------------------------------------
						_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
						//----------------------------------------------
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 START_PLACE_to_CM = Pointer(%d,%d)\n" , FM_Pointer , FM_Pointer2 );
						//----------------------------------------------------------------------
						//
						// Pick Cool
						//
						//----------------------------------------------------------------------
	/*
	2007.03.19
						//=============================================================================================
						// 2006.11.14
						//=============================================================================================
						if ( FM_TSlot > 0 ) {
							FM_Slot = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot );
						}
						else {
							FM_Slot = 0;
						}
						if ( FM_TSlot2 > 0 ) {
							FM_Slot2 = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot2 );
						}
						else {
							FM_Slot2 = 0;
						}
	*/
						//=============================================================================================
						if ( FM_TSlot > 0 ) {
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_Side , IC , FM_TSlot , 0 , FM_CM , -1 , TRUE , FM_Slot , 0 , FM_Side , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2007.07.11
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
								return 0;
							}
							//----------------------------------------------------------------------
	/*
	// 2007.11.26
							SCHEDULER_CONTROL_Set_MFICx_WAFER( FM_TSlot , 0 );
							//
							SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( TA_STATION , FM_Pointer , -1 );
							//
	//							SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot ); // 2007.07.11
							SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TA_STATION , FM_Side , FM_Slot ); // 2007.07.11
	*/
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 , -1 );
						}
						//----------------------------------------------------------------------
						if ( FM_TSlot2 > 0 ) {
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_Side2 , IC , 0 , FM_TSlot2 , FM_CM , -1 , TRUE , 0 , FM_Slot2 , 0 , FM_Side2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.11
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
								return 0;
							}
							//----------------------------------------------------------------------
	/*
	// 2007.11.26
							SCHEDULER_CONTROL_Set_MFICx_WAFER( FM_TSlot2 , 0 );
							//
							SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( TB_STATION , FM_Pointer2 , -1 );
							//
	//							SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 ); // 2007.07.11
							SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , FM_Side2 , FM_Slot2 ); // 2007.07.11
	*/
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , IC , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , -1 , -1 );
						}
						//----------------------------------------------------------------------
						//==========================================================================
						//----------------------------------------------------------------------
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 4 from FEM%cWHFMFAIL 4\n" , 9 );
							return 0;
						}
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//###################################################################################
						// Disapear Check (2001.11.28) // 2002.06.07
						//###################################################################################
						if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
							if ( FM_TSlot > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_OSlot = 0;
									FM_TSlot = 0;
								}
							}
							if ( FM_TSlot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_OSlot2 = 0;
									FM_TSlot2 = 0;
								}
							}
						}
						if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
							return 1;
						}
						//###################################################################################
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//
						// Code for Place to CM
						// FM_Slot
						//----
						//----------------------------------------------------------------------
						Result = _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , TRUE , TRUE , 0 ); // 2007.07.11
						if      ( Result == -1 ) { // Abort
							return 0;
						}
						else if ( Result != 0 ) { // disappear
							return 1;
						}
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 13 END_PLACE_to_CM = TSlot(%d,%d) OutCass(%d,%d) OutSlot(%d,%d)\n" , FM_TSlot , FM_TSlot2 , FM_CO1 , FM_CO2 , FM_OSlot , FM_OSlot2 );
						//----------------------------------------------------------------------
						if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 98 FINISH_CHECK_2 = TRUE\n" );
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 4 for FEM Module%cWHFMSUCCESS 4\n" , 9 );
							return SYS_SUCCESS;
						}
						return 1;
					}
				}
			}
		}
	}
	//================================================================================================================================================================
	//================================================================================================================================================================
	//================================================================================================================================================================
	//================================================================================================================================================================
	//================================================================================================================================================================
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM CM & PLACE TO BM PART
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//=============================================================================================
	PlaceCheck = FALSE; // 2004.09.09
	//=============================================================================================
	Check_Flag = TRUE;
//		if ( ( _SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2002.10.10 // 2006.11.28
	if ( ( _SCH_MODULE_Need_Do_Multi_FAL() ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2002.10.10 // 2006.11.28
		if (
			( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) &&
//			( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || // 2018.01.19
//			  ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) // 2018.01.19
			( ( (_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION )) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) || // 2018.01.19
			  ( ( (_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION )) && _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) ) // 2018.01.19
			) {
			if ( _SCH_MODULE_Get_MFAL_SIDE() != CHECKING_SIDE ) { // 2002.11.11
				Check_Flag = FALSE;
			}
			else {
				Check_Flag = FALSE;
				FM_Buffer = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
				if ( FM_Buffer == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
					return 0;
				}
				if ( FM_Buffer == SYS_SUCCESS ) {
//					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at AL(%d)%cWHFMALSUCCESS %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() ); // 2017.01.06
					FM_Buffer = -1;
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_MFAL_SIDE() , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
						//
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) {
									RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
									if ( RunBuffer > 0 ) {
										if ( !Scheduling_TM_Double( FALSE ) || ( RunBuffer >= 2 ) ) { // 2003.03.26
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
											}
											//
											FM_Buffer = k;
											break;
											//
										}
									}
								}
						}
					}
					if ( FM_Buffer >= 0 ) {
						//
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at AL(%d)%cWHFMALSUCCESS %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() ); // 2017.01.06
						//
						/*
						//
						// 2018.01.19
						//
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
							FM_Pointer2 = 0;
							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
							FM_Slot = _SCH_MODULE_Get_MFAL_WAFER();
							FM_Slot2 = 0;
							FM_TSlot2 = 0;
						}
						else {
							FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
							FM_Pointer2 = 0;
							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
							FM_Slot = 0;
							FM_Slot2 = _SCH_MODULE_Get_MFAL_WAFER();
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
						}
						//
						*/
						//
						//
						// 2018.01.19
						//
						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
								FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
								FM_Pointer2 = 0;
								FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
								FM_Slot = _SCH_MODULE_Get_MFAL_WAFER();
								FM_Slot2 = 0;
								FM_TSlot2 = 0;
							}
							else {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail[101] at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
								return 0;
							}
						}
						else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
								FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
								FM_Pointer2 = 0;
								FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
								FM_Slot = 0;
								FM_Slot2 = _SCH_MODULE_Get_MFAL_WAFER();
								FM_TSlot2 = FM_TSlot;
								FM_TSlot = 0;
							}
							else {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail[102] at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
								return 0;
							}
						}
						//
						//
						//==========================================================================================
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
							return 0;
						}
						//==========================================================================================
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , CHECKING_SIDE , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , CHECKING_SIDE , FM_Pointer2 , -1 ) ) {
						case -1 :
							return 0;
							break;
						}
						//==========================================================================================
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							//
							if ( FM_Slot > 0 ) { // 2019.01.21
								//
								_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot , CHECKING_SIDE , FM_Pointer );
								_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
								_SCH_MODULE_Set_MFAL_WAFER( 0 );
								//
							}
							//
						}
						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							//
							if ( FM_Slot2 > 0 ) { // 2019.01.21
								//
								_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot2 , CHECKING_SIDE , FM_Pointer );
								_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
								_SCH_MODULE_Set_MFAL_WAFER( 0 );
								//
							}
							//
						}
						//==========================================================================================
						if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) || _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
							//=====================================================================
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
								if ( _SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -2 , -1 , -1 ) ) { // 2003.01.13
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2 );
								}
								else { // 2019.01.21
									if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( FM_Buffer ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 201 );
									}
								}
							}
						}
						else {
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
								if ( Scheduling_TM_Double( FALSE ) ) { // 2003.03.28
									if ( !_SCH_MODULE_Get_FM_HWait( CHECKING_SIDE ) || SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 3 );
									}
								}
								else {
									if ( !_SCH_MODULE_Get_FM_HWait( CHECKING_SIDE ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL( FM_Buffer ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 4 );
									}
								}
							}
						}
					}
				}
			}
		}
		else {
			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
				Check_Flag = FALSE;
			}
			else {
				if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
					Check_Flag = FALSE;
				}
			}
		}
	}
	//
	//========================================================================================
	//
	// START 2017.01.06
	//
	else if ( ( _SCH_MODULE_Need_Do_Multi_FAL() ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
		//
		if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 5 ) == 1 ) { // 2017.01.07
			//
			while ( TRUE ) {
				//
//				k = SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( CHECKING_SIDE , TRUE ); // 2017.01.20
				k = SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( CHECKING_SIDE , FALSE , NULL , TRUE ); // 2017.01.20
				//
				if ( k == 0 ) return 0;
				//
				if ( k == -1 ) break;
				//
			}
			//
		}
		//
		if (
			( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) &&
			( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) && // 2017.01.23
			( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) &&
			  ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) )
			) {
			//
//			if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -2 , -1 , -1 ) ) { // 2017.01.20
			if ( _SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) { // 2017.01.20
				//
				FM_Buffer = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
				//
				if ( FM_Buffer == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
					return 0;
				}
				if ( FM_Buffer == SYS_SUCCESS ) {
					//
					/*
					//
					// 2017.01.23
					//
					FM_Buffer = -1;
					//
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( _SCH_MODULE_Get_MFAL_SIDE() , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
						//
						if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) {
									RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
									if ( RunBuffer > 0 ) {
										if ( !Scheduling_TM_Double( FALSE ) || ( RunBuffer >= 2 ) ) { // 2003.03.26
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
											}
											//
											FM_Buffer = k;
											break;
											//
										}
									}
								}
						}
					}
					*/
					//
					// 2017.01.23
					//
					FM_Buffer = -1;
					//
					if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
						FM_Side = CHECKING_SIDE;
						FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
						FM_Slot = _SCH_MODULE_Get_MFAL_WAFER();
						//
						FM_Side2 = CHECKING_SIDE;
						FM_Pointer2 = 0;
						FM_Slot2 = 0;
					}
					else {
						FM_Side = CHECKING_SIDE;
						FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
						FM_Slot = 0;
						//
						FM_Side2 = CHECKING_SIDE;
						FM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER();
						FM_Slot2 = _SCH_MODULE_Get_MFAL_WAFER();
					}
					//
					FM_AL_Buffer = _SCH_MODULE_Get_MFAL_BM();
					//
					if ( FM_AL_Buffer < BM1 ) FM_AL_Buffer = -1;
					//
					RemainTime = 999999;
					//
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue;
						//
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue;
						//
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) {
							//
							if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
								if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue;
							}
							//
							RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlotBuf1 , &FM_TSlotBuf2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
							//
							//
							if ( RunBuffer > 0 ) {
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
									if      ( FM_Slot  > 0 ) {
										if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) {
											RunBuffer = 0;
											continue;
										}
									}
									else if ( FM_Slot2 > 0 ) {
										if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) {
											RunBuffer = 0;
											continue;
										}
									}
								}
								//
								//=====================================================================================================================
								//
								if ( ( FM_AL_Buffer > 0 ) && Scheduling_PT_and_1S_Same() ) {
									//
									if ( FM_AL_Buffer == k ) {
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
											RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
											FM_Buffer = k;
											//
											FM_TSlot = FM_TSlotBuf1;
											FM_TSlot2 = FM_TSlotBuf2;
											//
											break;
										}
									}
									//
								}
								//
								//=====================================================================================================================
								//
								else {
									if ( !Scheduling_TM_Double( FALSE ) || ( RunBuffer >= 2 ) ) {
										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
											if ( FM_AL_Buffer == k ) {
												//
												FM_Buffer = k;
												//
												FM_TSlot = FM_TSlotBuf1;
												FM_TSlot2 = FM_TSlotBuf2;
												//
												RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												break;
											}
											else {
												if ( FM_Buffer == -1 ) {
													FM_Buffer = k;
													//
													FM_TSlot = FM_TSlotBuf1;
													FM_TSlot2 = FM_TSlotBuf2;
													//
													RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												}
												else {
													if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
														FM_Buffer  = k;
														//
														FM_TSlot = FM_TSlotBuf1;
														FM_TSlot2 = FM_TSlotBuf2;
														//
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
					//
					//
					//
					if ( FM_Buffer >= 0 ) {
						//
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at AL(%d)%cWHFMALSUCCESS %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
						//
						FM_Side = CHECKING_SIDE; // 2017.01.23
						FM_Side2 = CHECKING_SIDE; // 2017.01.23
						//
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
//							FM_Pointer2 = 0; // 2017.01.23
							FM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER(); // 2017.01.23
							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
							FM_Slot = _SCH_MODULE_Get_MFAL_WAFER();
							FM_Slot2 = 0;
							FM_TSlot2 = 0;
						}
						else {
							FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
//							FM_Pointer2 = 0; // 2017.01.23
							FM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER(); // 2017.01.23
							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
							FM_Slot = 0;
							FM_Slot2 = _SCH_MODULE_Get_MFAL_WAFER();
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
						}
						//==========================================================================================
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
							return 0;
						}
						//==========================================================================================
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , CHECKING_SIDE , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , CHECKING_SIDE , FM_Pointer2 , -1 ) ) {
						case -1 :
							return 0;
							break;
						}
						//==========================================================================================
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot , CHECKING_SIDE , FM_Pointer );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_MFAL_WAFER( 0 );
						}
//						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) { // 2017.01.23
						else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) { // 2017.01.23
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot2 , CHECKING_SIDE , FM_Pointer );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_MFAL_WAFER( 0 );
						}
						//==========================================================================================
						if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) || _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
							//=====================================================================
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
//								if ( _SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -2 , -1 , -1 ) ) { // 2003.01.13 // 2017.01.23
								if ( _SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) { // 2003.01.13 // 2017.01.23
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2 );
								}
							}
						}
						else {
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
								if ( Scheduling_TM_Double( FALSE ) ) { // 2003.03.28
									if ( !_SCH_MODULE_Get_FM_HWait( CHECKING_SIDE ) || SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 3 );
									}
								}
								else {
									if ( !_SCH_MODULE_Get_FM_HWait( CHECKING_SIDE ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL( FM_Buffer ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 4 );
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
	// END 2017.01.06
	//
	//========================================================================================
	//
	else {
		if ( SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) == 1 ) Check_Flag = FALSE;
	}

	if ( Check_Flag ) { // 2002.06.24
		if ( ( _SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) || ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() > 0 ) ) {
			FM_Total_Out_Count = 0;
			FM_Total_In_Count  = 0;
			FM_Total_Dbl_Count = 0;
			for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
				if ( _SCH_SYSTEM_USING_GET(k) > 0 ) {
					FM_Total_Out_Count += _SCH_MODULE_Get_FM_OutCount( k );
					FM_Total_In_Count  += _SCH_MODULE_Get_FM_InCount( k );
					FM_Total_Dbl_Count += _SCH_MODULE_Get_TM_DoubleCount(k);
				}
			}
			if ( _SCH_MODULE_Need_Do_Multi_FIC() )
				FM_Total_In_Count  += _SCH_MODULE_Chk_MFIC_HAS_COUNT();
			//
			Runmode    = 0; // 0=unused , 1=pick_place , 2=s_pick_only , 3=s_place_only , 4=a_pick , 5=a_pick
			RunMaxSize = 0;
			//
			if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
				RunMaxSize = 1;
				if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
					Runmode    = 2;
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						Runmode = 4;
					}
					else {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
							if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
								Runmode = 3;
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION ); // 2006.10.09
								FM_Slot  = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer2 = 0; // 2006.10.09
								FM_Slot2 = 0;
							}
							else {
								Runmode = 0;
							}
						}
						else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
							if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) {
								Runmode = 3;
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION ); // 2006.10.09
								FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer = 0; // 2006.10.09
								FM_Slot  = 0;
							}
							else {
								Runmode = 0;
							}
						}
					}
				}
				else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
					Runmode    = 2;
					if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
						Runmode = 0;
					}
				}
				else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					Runmode    = 2;
					if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
						Runmode = 0;
					}
				}
			}
			else {
				Runmode = 1;
				//==========================================
				// 2006.10.09
				//==========================================
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
							if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == 0 ) {
								Runmode = 3;
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								FM_Slot  = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer2 = 0;
								FM_Slot2 = 0;
							}
						}
					}
					else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
						if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) {
							if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == 0 ) {
								Runmode = 3;
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer = 0;
								FM_Slot  = 0;
							}
						}
					}
				}
				else {
					//
					// 2017.12.21
					//
					for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
						//
						if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
						//
						if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
							//
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) > 0 ) ) {
								//
								if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) ) {
									//
									if ( _SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( 0,k ) != 0 ) {
										//
										Runmode = 0;
										//
										break;
									}
									//
									if ( _SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( 0,k ) != 0 ) {
										//
										Runmode = 0;
										//
										break;
									}
									//
								}
							}
							//
						}
					}
					//
				}
				//==========================================
				if ( Runmode != 3 ) { // 2006.10.09
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
							Runmode = 0;
						}
					}
					else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
							Runmode = 0;
						}
					}
				}
				RunMaxSize = 0;
//					if ( Runmode != 0 ) { // 2006.10.09
				if ( Runmode == 1 ) { // 2006.10.09
					if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) ) {
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) {
//								if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_xGROUP( k ) == 0 ) ) {
//									if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_OUT_MODE ) {
									if ( Scheduling_TM_Double( TRUE ) ) {
										if ( RunMaxSize == 0 ) { // 2008.04.21
											RunMaxSize = RunMaxSize + 4 + _SCH_PRM_GET_MODULE_SIZE(k) - 2;
										}
										else { // 2008.04.21
											RunMaxSize = RunMaxSize + 2 + _SCH_PRM_GET_MODULE_SIZE(k) - 2;
										}
									}
									else {
										if ( RunMaxSize == 0 ) { // 2008.04.21
											RunMaxSize = RunMaxSize + 4 + _SCH_PRM_GET_MODULE_SIZE(k) - 1;
										}
										else { // 2008.04.21
											RunMaxSize = RunMaxSize + 2 + _SCH_PRM_GET_MODULE_SIZE(k) - 1;
										}
									}
									//======================================================================================================
									// 2006.09.29
									//======================================================================================================
									if ( ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( k ) == 2 ) || ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( k ) == 3 ) ) {
										RunMaxSize--;
									}
									//======================================================================================================
//									}
							}
						}
					}
					else {
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) {
//								if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_xGROUP( k ) == 0 ) ) {
//									if ( _SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , k ) == BUFFER_OUT_MODE ) {
									if ( Scheduling_TM_Double( TRUE ) ) {
										if ( RunMaxSize == 0 ) { // 2008.04.21
											RunMaxSize = RunMaxSize + 3 + _SCH_PRM_GET_MODULE_SIZE(k) - 2;
										}
										else { // 2008.04.21
											RunMaxSize = RunMaxSize + 1 + _SCH_PRM_GET_MODULE_SIZE(k) - 2;
										}
									}
									else {
										if ( RunMaxSize == 0 ) { // 2008.04.21
											RunMaxSize = RunMaxSize + 3 + _SCH_PRM_GET_MODULE_SIZE(k) - 1;
										}
										else { // 2008.04.21
											RunMaxSize = RunMaxSize + 1 + _SCH_PRM_GET_MODULE_SIZE(k) - 1;
										}
									}
									//======================================================================================================
									// 2006.09.29
									//======================================================================================================
									if ( ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( k ) == 2 ) || ( _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( k ) == 3 ) ) {
										RunMaxSize--;
									}
									//======================================================================================================
//								}
							}
						}
					}
				}
			}
			//
			if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 1 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 3 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 5 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) ) RunMaxSize = 9999999; // 2002.10.02
			//
			switch ( Runmode ) {
			case 1 : // pick
				//========================================================================================
				// 2006.05.27
				//========================================================================================
				j = 0;
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
						j++;
					}
				}
				//
				l = 0;
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
					if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
					if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) > 0 ) ) {
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) ) {
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2009.01.21
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) > 0 ) ) {
											if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_FM_STATION ) {
												if ( j > 1 ) {
													if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) {
														l++;
													}
												}
												else {
													l++;
												}
											}
										}
										else { // 2008.10.17
											if ( ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 1 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 3 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 5 ) || ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() == 7 ) ) { // not delay 2008.10.17
												if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , -1 ) <= 0 ) ) {
													l = 0;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( l != 0 ) Runmode = 0;
				//
				if ( Runmode != 0 ) {
					if ( j > 1 ) RunMaxSize = -99999;
					if ( ( FM_Total_Out_Count - FM_Total_In_Count - FM_Total_Dbl_Count ) > RunMaxSize ) { // delay mode
						l = 0;
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( k ) ) {
									l++;
								}
								else {
									if ( !Scheduling_TM_Double( FALSE ) ) { // 2003.03.27
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_FIND( k , &i , 0 ) > 0 ) {
//												if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
												l++;
											}
										}
									}
								}
							}
						}
						if ( l == 0 ) Runmode = 0;
					}
				}
				//========================================================================================
				break;
			case 2 : // s_pick
			case 3 : // s_place
				k = -1;
				if ( Runmode == 3 ) {
					if      ( FM_Slot  > 0 ) k = FM_Slot;
					else if ( FM_Slot2 > 0 ) k = FM_Slot2;
				}
				if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) { // 2006.10.09
					if ( SCHEDULING_CHECK_Enable_PICK_CM_or_PLACE_to_BM_at_SINGLE_MODE( 0 , CHECKING_SIDE , k ) < 0 ) {
						Runmode = 0;
					}
					if ( Runmode == 0 ) {
						if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TB_STATION ) ) {
							if ( ( FM_Total_Out_Count - FM_Total_In_Count - FM_Total_Dbl_Count ) <= 2 ) {
								Runmode = 5;
							}
						}
					}
				}
				break;
			}

			if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) && ( Runmode != 3 ) ) {
				Runmode = 0;
				Result = -99;
				_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 ); // 2003.05.28
			}

			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 04 RUN_BUFFER_CHECK = %d\n" , RunBuffer );

			if ( Runmode != 0 ) {
				//
				if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
					EnterCriticalSection( &CR_SINGLEBM_STYLE_0 );
					_SCH_MODULE_Set_FM_MidCount( 11 );
					if ( _SCH_MODULE_Get_BM_WAFER( Get_Prm_MODULE_BUFFER_SINGLE_MODE() , 1 ) > 0 ) {
						_SCH_MODULE_Set_FM_MidCount( 0 );
						LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
						return 1;
					}
					if ( Runmode == 2 ) {
						if ( SCHEDULING_CHECK_Enable_PICK_CM_or_PLACE_to_BM_at_SINGLE_MODE( 0 , CHECKING_SIDE , -1 ) < 0 ) {
							_SCH_MODULE_Set_FM_MidCount( 0 );
							LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							return 1;
						}
						if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TA_STATION ) ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( 0 , TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( 0 , TA_STATION ) == SCHEDULER_MOVING_OUT ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								return 1;
							}
						}
						if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TB_STATION ) ) {
							if ( ( _SCH_MODULE_Get_TM_WAFER( 0 , TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_TM_TYPE( 0 , TB_STATION ) == SCHEDULER_MOVING_OUT ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								return 1;
							}
						}
					}
				}
				//					//
				armbchk = TRUE; // 2007.08.30
				//
				if ( Runmode != 3 ) {
					l = 0;
					if ( !Scheduling_TM_Double( FALSE ) ) {
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) {
									i = SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k );
									if ( i == 1 ) {
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) ) {
											if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) { // 2007.12.08
												if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) { // 2008.12.12
													if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( _SCH_PRM_GET_MODULE_GROUP( k ) , TA_STATION ) && _SCH_ROBOT_GET_FINGER_HW_USABLE( _SCH_PRM_GET_MODULE_GROUP( k ) , TB_STATION ) ) { // 2006.05.26
														l++; // 2006.05.26
													}
												}
											}
										}
									}
									else if ( i >= 2 ) { // 2006.05.26
										l = l + i;
									}
							}
							else {
								//========================================================================================
								// 2007.08.30
								//========================================================================================
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
									i = SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k );
									if ( i >= 2 ) {
										l = l + i;
									}
								}
								//========================================================================================
							}
						}
						//====================================================================================
						// 2006.09.29
						//====================================================================================
						if ( l == 0 ) { // 2006.09.29
							for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
								if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
								if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
									i = SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( k );
									if ( i >= 3 ) {
										l = l + i;
									}
								}
							}
						}
						//====================================================================================
					}
					else { // 2006.05.26
						l = 2; // 2006.05.26
					}
					//----------------------------------------------------------------------
//					FM_Pick_Running_Blocking_style_0 = CM1; // 2006.04.28 // 2008.03.31 // 2008.09.25
					//---------------------------------------------------------------------
//					if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &ret_rcm , &ret_pt , 0 ) ) { // 2007.02.01 // 2015.06.25
					if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( CHECKING_SIDE , &ret_rcm , &ret_pt , 99 ) == 0 ) { // 2010.09.28 // 2015.06.25
						//----------------------------------------------------------------------
						FM_Pick_Running_Blocking_style_0 = ret_rcm; // 2006.04.28 // 2008.03.31 // 2008.09.25
						//---------------------------------------------------------------------
						if ( _SCH_SUB_FM_Current_No_Way_Supply( CHECKING_SIDE , ret_pt , -1 ) ) { // 2008.07.28
							_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 ); // 2008.07.28
							Result = -91; // 2008.07.28
						} // 2008.07.28
						else {
							if ( l >= 2 ) { // 2006.05.26
								//==========================================================================
								// 2007.08.28
								//==========================================================================
//								Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_HSide , &FM_HSide2 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , Scheduling_Data_Mode_For_Pick_ArmIntlks( ret_rcm , Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) , &cpreject , &retpointer ); // 2008.11.01
								Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , Scheduling_TM_Double( FALSE ) , Scheduling_Data_Mode_For_Pick_ArmIntlks( ret_rcm , Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_HSide , &FM_HSide2 , &cpreject ); // 2008.11.01
								//==========================================================================
							}
							else {
								armbchk = FALSE; // 2007.08.30
								//==========================================================================
								// 2007.08.28
								//==========================================================================
//								Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_HSide , &FM_HSide2 , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , Scheduling_Data_Mode_For_Pick_ArmIntlks( ret_rcm , ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) , &cpreject , &retpointer ); // 2007.07.10 // 2007.08.10 // 2008.11.01
								Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , Scheduling_TM_Double( FALSE ) , Scheduling_Data_Mode_For_Pick_ArmIntlks( ret_rcm , Scheduling_TM_Double( FALSE ) ) , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &retpointer , &FM_HSide , &FM_HSide2 , &cpreject ); // 2007.07.10 // 2007.08.10 // 2008.11.01
								//==========================================================================
							}
							//============================================================================================================
							// 2008.01.15
							//============================================================================================================
							if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
								if ( ( Result >= 0 ) && ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) && ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
									if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( CHECKING_SIDE , FM_Pointer , CHECKING_SIDE , FM_Pointer2 ) ) {
										FM_Slot2 = 0;
										_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , FM_Pointer2 );
									}
	//									else { // 2008.03.21 // 2008.05.23
	//										if ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( CHECKING_SIDE , FM_Pointer2 , 3 ) <= 0 ) { // 2008.05.23
	//											FM_Slot2 = 0; // 2008.05.23
	//											_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , FM_Pointer2 ); // 2008.05.23
	//										} // 2008.05.23
	//									} // 2008.05.23
								}
							}
							//============================================================================================================
							// 2006.12.05
							//============================================================================================================
							if ( ( Result >= 0 ) && ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
								if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ) ) == 3 ) {
									if ( SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT() <= 0 ) {
										_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , retpointer );
										Result = -102;
									}
								}
							}
							if ( ( Result >= 0 ) && ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
								if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 ) ) == 3 ) {
									if ( SCHEDULING_CHECK_PLACE_TO_BM_FREE_COUNT() <= 0 ) {
										_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , retpointer );
										Result = -103;
									}
								}
							}
							//============================================================================================================
						}
					}
					else {
						Result = -101;
					}
				}
				else {
					Result = 0;
				}
				if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
					if ( Result < 0 ) {
						_SCH_MODULE_Set_FM_MidCount( 0 );
						LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
					}
				}
				//
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05 CHECK_PICK_from_FM = %d(Run_Mode=%d)(ret_pt=%d) (TMS=%d)\n" , Result , Runmode , ret_pt , _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( 0 ) );
				//===============================================================================================================
				// 2004.05.10
				//===============================================================================================================
				if ( Result >= 0 ) {
					if ( Runmode != 3 ) {
						if ( ( FM_Slot > 0 ) && ( Result >= 0 ) ) {
							if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , FM_Pointer ) ) {
								_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , retpointer );
								Result = -101;
							}
						}
						if ( ( FM_Slot2 > 0 ) && ( Result >= 0 ) ) {
							if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , FM_Pointer2 ) ) {
								_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , retpointer );
								Result = -102;
							}
						}
					}
				}
				//===============================================================================================================
				FM_AL_Buffer = -1; // 2010.07.07
				//===============================================================================================================
				if ( Result < 0 ) {
					//----------------------------------------------------------------------
					FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28
					//----------------------------------------------------------------------
				}
				else {
					//----------------------------------------------------------------------------
					//----------------------------------------------------------------------------
//						_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ? 2 : 1 ); // 2003.04.23 // 2008.05.23
					//----------------------------------------------------------------------------
					//----------------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06 START_PICK_from_CM = Slot(%d,%d) Pointer(%d,%d) Side(%d,%d) HSide(%d,%d)\n" , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2 , FM_HSide , FM_HSide2 );
					//-----------------------------------------------
					_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 11 );
					//-----------------------------------------------
					FM_Side = CHECKING_SIDE; // 2007.07.10
					FM_Side2 = CHECKING_SIDE; // 2007.07.10
					//-----------------------------------------------
					if ( Runmode == 3 ) {
						//----------------------------------------------------------------------
						FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28
						//---------------------------------------------------------------------
					}
					else {
						//
						//========================================================================================
						//
						// START 2017.01.06
						//
						if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
							//
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
//								if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) { // 2017.01.24
									FM_Slot2 = 0;
									_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , FM_Pointer2 );
//								} // 2017.01.24
							}
						}
						//
						// END 2017.01.06
						//
						//========================================================================================
						//
						//----------------------------------------------------------------------------------------------------------------------------------------------
						// User Interface
						// 2016.05.10
						//----------------------------------------------------------------------------------------------------------------------------------------------
						switch( _SCH_COMMON_USER_FLOW_NOTIFICATION_MREQ( MACRO_PICK , 1 , FM_Side , FM_Pointer , FM_Slot , FM_Side2 , FM_Pointer2 , FM_Slot2 , 0 ) ) {
						case 0 :
							//-----------------------------------------------
							if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							}
							//----------------------------------------------
							return 0;
							break;
						case 1 :
							//-----------------------------------------------
							if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							}
							//----------------------------------------------
							return 1;
							break;
						}
						//----------------------------------------------------------------------------------------------------------------------------------------------
						//
						//----------------------------------------------
						_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
						//----------------------------------------------
						//=====================================================================================================
						// 2007.10.03
						//=====================================================================================================
						if ( _SCH_PRM_GET_DFIX_IOINTLKS_USE() && ( FM_Slot  > 0 ) && ( FM_Slot2 > 0 ) ) { // 2007.10.03
							//----------------------------------------------------------------------
							_SCH_MODULE_Inc_FM_OutCount( FM_Side );
							//----------------------------------------------------------------------
							SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_Side , _SCH_CLUSTER_Get_ClusterIndex( FM_Side , FM_Pointer ) ); // 2007.07.10
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { // 2010.03.17
								_SCH_LOG_LOT_PRINTF( FM_Side , "FM Select Log with FIRSTLASTCHECK [P=%d,PR=(0)%d,(10)%d,(13)%d]\t\t\n" , FM_Pointer , _SCH_CLUSTER_Get_PathRun( FM_Side , FM_Pointer , 0 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side , FM_Pointer , 10 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side , FM_Pointer , 13 , 2 ) );
							}
							//----------------------------------------------------------------------
							_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 );
							//----------------------------------------------------------------------
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_Side , FM_Pointer , -1 , 0 , 0 , 0 , 0 , -1 ) ) {
							case -1 :
								//-----------------------------------------------
								if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
									_SCH_MODULE_Set_FM_MidCount( 0 );
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								}
								//-----------------------------------------------
								return 0;
								break;
							}
							//==========================================================================================================================================
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , G_PICK , 0 ) ) {
								if ( ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) && ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( FM_Side2 , FM_Pointer2 , 3 ) <= 0 ) ) { // 2008.05.23 // 2008.06.10
									//----------------------------------------------------------------------
									_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 ); // 2003.04.23 // 2008.05.23
									//----------------------------------------------------------------------
									FM_Slot2 = 0; // 2008.05.23
									_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , FM_Pointer2 ); // 2008.05.23
								} // 2008.05.23
								else {
									//
									//=============================
									//
									if ( ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) || Scheduling_PT_and_1S_Same() ) { // 2017.10.20 // 2017.11.22 (fix)
										//
										//----------------------------------------------------------------------
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 );
										//----------------------------------------------------------------------
										FM_Slot2 = 0;
										//
										_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , FM_Pointer2 );
										//
									}
									//
									//=============================
									//
									else {
										//
										//----------------------------------------------------------------------
										_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 2 ); // 2003.04.23 // 2008.05.23
										//----------------------------------------------------------------------
										//----------------------------------------------------------------------
										_SCH_MODULE_Inc_FM_OutCount( FM_Side2 ); // 2007.07.10
										//----------------------------------------------------------------------
										SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_Side2 , _SCH_CLUSTER_Get_ClusterIndex( FM_Side2 , FM_Pointer2 ) ); // 2007.07.10
										//----------------------------------------------------------------------
										if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { // 2010.03.17
											_SCH_LOG_LOT_PRINTF( FM_Side2 , "FM Select Log with FIRSTLASTCHECK [P=%d,PR=(0)%d,(10)%d,(13)%d]\t\t\n" , FM_Pointer2 , _SCH_CLUSTER_Get_PathRun( FM_Side2 , FM_Pointer2 , 0 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side2 , FM_Pointer2 , 10 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side2 , FM_Pointer2 , 13 , 2 ) );
										}
										//----------------------------------------------------------------------
										_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27 // 2007.07.10
										//----------------------------------------------------------------------
										switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 ) ) { // 2007.03.21 // 2007.07.10
										case -1 :
											//-----------------------------------------------
											if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
												_SCH_MODULE_Set_FM_MidCount( 0 );
												LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
											}
											//-----------------------------------------------
											return 0;
											break;
										}
										//----------------------------------------------------------------------
										//
									}
									//
								}
							}
							else {
								//----------------------------------------------------------------------
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 ); // 2003.04.23 // 2008.05.23
								//----------------------------------------------------------------------
								FM_Slot2 = 0;
								_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , FM_Pointer2 );
							}
							//==========================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
							//==========================================================================
//							FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28 // 2008.09.25
							//----------------------------------------------------------------------
						}
						//=====================================================================================================
						else {
							//----------------------------------------------------------------------------
							// 2008.05.23
							//----------------------------------------------------------------------------
							if ( FM_Slot2 > 0 ) {
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
									//
									// 2017.11.22 (fix)
//									if ( SCHEDULER_CONTROL_PICK_FROM_FM_CHECK_A0SUB4( FM_Side2 , FM_Pointer2 , 3 ) <= 0 ) { // 2008.05.23
//										FM_Slot2 = 0; // 2008.05.23
//										_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , FM_Pointer2 ); // 2008.05.23
//									}
									//
									// 2017.11.22 (fix)
									//
									if ( FM_Slot > 0 ) { // 2017.12.21
										FM_Slot2 = 0; // 2008.05.23
										_SCH_MODULE_Set_FM_DoPointer( FM_Side2 , FM_Pointer2 ); // 2008.05.23
									}
									//
								}
							}
							//----------------------------------------------------------------------------
							_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ? 2 : 1 ); // 2003.04.23 // 2008.05.23
							//----------------------------------------------------------------------------
							//
							if ( FM_Slot  > 0 ) _SCH_MODULE_Inc_FM_OutCount( FM_Side ); // 2007.07.10
							if ( FM_Slot2 > 0 ) _SCH_MODULE_Inc_FM_OutCount( FM_Side2 ); // 2007.07.10
							//----------------------------------------------------------------------
							// Code for Pick from CM
							// FM_Slot
							//----------------------------------------------------------------------
							if ( FM_Slot > 0 ) {
								//===================================================================================================
								// 2007.02.09
								//===================================================================================================
								SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_Side , _SCH_CLUSTER_Get_ClusterIndex( FM_Side , FM_Pointer ) ); // 2007.07.10
								//===================================================================================================
								//----------------------------------------------------------------------
								if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { // 2010.03.17
									_SCH_LOG_LOT_PRINTF( FM_Side , "FM Select Log with FIRSTLASTCHECK [P=%d,PR=(0)%d,(10)%d,(13)%d]\t\t\n" , FM_Pointer , _SCH_CLUSTER_Get_PathRun( FM_Side , FM_Pointer , 0 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side , FM_Pointer , 10 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side , FM_Pointer , 13 , 2 ) );
								}
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side , FM_Pointer , 0 , 0 , 0 ); // 2006.11.27 // 2007.07.10
							}
							if ( FM_Slot2 > 0 ) {
								//===================================================================================================
								// 2007.02.09
								//===================================================================================================
								SCHEDULER_CONTROL_SET_Last_GlobalLot_Cluster_Index_When_Pick_A0SUB4( FM_Side2 , _SCH_CLUSTER_Get_ClusterIndex( FM_Side2 , FM_Pointer2 ) ); // 2007.07.10
								//===================================================================================================
								//----------------------------------------------------------------------
								if ( ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 6 ) || ( _SCH_PRM_GET_SYSTEM_LOG_STYLE() == 7 ) ) { // 2010.03.17
									_SCH_LOG_LOT_PRINTF( FM_Side2 , "FM Select Log with FIRSTLASTCHECK [P=%d,PR=(0)%d,(10)%d,(13)%d]\t\t\n" , FM_Pointer2 , _SCH_CLUSTER_Get_PathRun( FM_Side2 , FM_Pointer2 , 0 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side2 , FM_Pointer2 , 10 , 2 ) , _SCH_CLUSTER_Get_PathRun( FM_Side2 , FM_Pointer2 , 13 , 2 ) );
								}
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_Side2 , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27 // 2007.07.10
							}
							//==========================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 ) ) { // 2007.03.21 // 2007.07.10
							case -1 :
								//-----------------------------------------------
								if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
									_SCH_MODULE_Set_FM_MidCount( 0 );
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								}
								//-----------------------------------------------
								return 0;
								break;
							}
							//==========================================================================================================================================
							// 2007.07.10
							//==========================================================================================================================================
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
								//
								//========================================================================================
								//
								// START 2017.01.06
								//
								if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
									//
									if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
										armbchk = FALSE;
									}
									//
								}
								//
								// END 2017.01.06
								//
								//========================================================================================
								//
								if ( armbchk ) { // 2007.08.30
									switch ( Scheduling_Other_Side_Pick_From_FM( FM_Side , FM_Pointer , FM_HSide , &FM_Side2 , &FM_Pointer2 , &FM_Slot2 , &FM_HSide2 ) ) {
									case -1 :
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
										//-----------------------------------------------
										if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
											_SCH_MODULE_Set_FM_MidCount( 0 );
											LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										}
										//-----------------------------------------------
										return 0;
										break;
									}
								}
							}
							//==========================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , FM_Side , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
							//==========================================================================
//							FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28 // 2008.09.25
							//----------------------------------------------------------------------
						}
						//----------------------------------------------------------------------
						FM_Pick_Running_Blocking_style_0 = 0; // 2006.04.28 // 2008.09.25
						//----------------------------------------------------------------------
						//=====================================================================================================
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 5 from FEM%cWHFMFAIL 5\n" , 9 );
							//-----------------------------------------------
							if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							}
							return 0;
						}
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//###################################################################################
						// Disapear Check (2001.11.28) // 2002.06.07
						//###################################################################################
						if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
							if ( FM_Slot > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_Slot = 0;
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_Slot2 = 0;
								}
							}
						}
						if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
							//-----------------------------------------------
							if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							}
							//----------------------------------------------
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//-----------------------------------------------
							return 1;
						}
						//###################################################################################
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//
						// Code Aligning Before Place to BI
						//
						//----------------------------------------------------------------------
						if ( FM_Slot > 0 ) {
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ); // 2007.07.10
						}
						else if ( FM_Slot2 > 0 ) {
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ); // 2007.07.10
						}
						//
						if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2004.09.03
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) FM_Slot2 = 0;
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
								//-----------------------------------------------
								if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
									_SCH_MODULE_Set_FM_MidCount( 0 );
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								}
								return 0;
							}
							//----------------------------------------------
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//-----------------------------------------------
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , AL , FM_Slot , 1 , FM_Side , FM_Pointer , AL , FM_Slot2 , 1 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
							//
							if ( FM_Slot > 0 ) {
								//============================================================================
								// Aligning
								//============================================================================
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_Side , FM_Slot , 0 , FM_CM , -1 ); // 2007.07.10
								//============================================================================
							}
							else if ( FM_Slot2 > 0 ) {
								//============================================================================
								// Aligning
								//============================================================================
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_Side2 , 0 , FM_Slot2 , FM_CM , -1 ); // 2004.08.17 // 2007.07.10
								//============================================================================
							}
							return 1;
						}
						else {
							//
							//========================================================================================
							//
							// START 2017.01.06
							//
							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
								if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
									FM_RejectBuffer = _SCH_MODULE_Get_MFAL_BM();
									if ( FM_RejectBuffer < BM1 ) FM_RejectBuffer = -1;
								}
								else {
									FM_RejectBuffer = -1;
								}
							}
							else {
								FM_RejectBuffer = -2;
							}
							//
							// END 2017.01.06
							//
							//========================================================================================
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 77 Place_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) FM_RejectBuffer(%d) FM_XBuffer(%d) FM_AL_Buffer(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_RejectBuffer , FM_XBuffer , FM_AL_Buffer );
							//----------------------------------------------------------------------
							//====================================================================================================
							// 2007.01.15
							//====================================================================================================
							FM_XBuffer = -1;
							FM_Buffer = -1;
							if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
								//
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
									/*
									//
									// 2016.10.13
									//
									for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
										if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
										//
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
										//
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
											}
											//
											if ( FM_Buffer == -1 ) {
												FM_Buffer = k;
											}
											else {
												if ( FM_XBuffer == -1 ) {
													FM_XBuffer = k;
												}
											}
										}
									}
									if ( FM_Buffer  <= 0 ) FM_Buffer = 0;
									if ( FM_XBuffer <= 0 ) FM_XBuffer = 0;
									FM_XBuffer = FM_XBuffer * 1000 + FM_Buffer;
									//
									FM_AL_Buffer = FM_Buffer; // 2010.07.07
									*/
									//
									//-------------------------------------------------------------------------------
									//
									// 2016.10.13
									//
									RemainTime = 999999;
									for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
										if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
										//
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
										//
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
											}
											//
											if ( FM_XBuffer == -1 ) {
												RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												FM_XBuffer  = k;
											}
											else {
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_XBuffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_XBuffer ) == BUFFER_FM_STATION ) ) {
														if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_XBuffer ) == 0 ) {
														}
														else if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) == 0 ) {
															RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
															FM_XBuffer  = k;
														}
														else {
															if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
																RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
																FM_XBuffer  = k;
															}
														}
													}
													else {
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
														FM_XBuffer  = k;
													}
												}
												else {
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_XBuffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_XBuffer ) == BUFFER_FM_STATION ) ) {
													}
													else {
														if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
															RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
															FM_XBuffer  = k;
														}
													}
												}
											}
										}
									}
									//
									FM_AL_Buffer = FM_XBuffer;
									//
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( FM_XBuffer ) < 2 ) {
										//
										RemainTime = 999999;
										//
										FM_Buffer = -1;
										//
										for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
											//
											if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
											//
											if ( k == FM_XBuffer ) continue;
											//
											if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
											//
											if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
												//
												if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
													if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
												}
												//
												if ( FM_Buffer == -1 ) {
													RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
													FM_Buffer  = k;
												}
												else {
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) {
															if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) == 0 ) {
															}
															else if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) == 0 ) {
																RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
																FM_Buffer  = k;
															}
															else {
																if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
																	RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
																	FM_Buffer  = k;
																}
															}
														}
														else {
															RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
															FM_Buffer  = k;
														}
													}
													else {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) {
														}
														else {
															if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
																RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
																FM_Buffer  = k;
															}
														}
													}
												}
											}
										}
										//
										if ( FM_Buffer != -1 ) {
											//
											FM_XBuffer = FM_XBuffer + ( FM_Buffer * 10000 );
											//
										}
										//
									}
									//
									//-------------------------------------------------------------------------------
									//
								}
								else {
									//
									FM_XBuffer = -1; // 2016.10.13
									//
									RemainTime = 999999;
									//
									for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
										//
										//========================================================================================
										//
										// START 2017.01.06
										//
										if ( FM_RejectBuffer == k ) continue;
										//
										// END 2017.01.06
										//
										//========================================================================================
										//
										if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
										//
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
										//
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
											}
											//
											// 2016.10.13
											//
											/*
											if ( FM_Buffer == 1 ) {
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
													if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
														FM_XBuffer  = k;
													}
												}
											}
											else {
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
													FM_Buffer = 1;
													RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
													FM_XBuffer  = k;
												}
												else {
													if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
														FM_XBuffer  = k;
													}
												}
											}
											*/
											//
											// 2016.10.13
											//
											if ( FM_XBuffer == -1 ) {
												RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												FM_XBuffer  = k;
											}
											else {
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_XBuffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_XBuffer ) == BUFFER_FM_STATION ) ) {
														if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_XBuffer ) == 0 ) {
														}
														else if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) == 0 ) {
															RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
															FM_XBuffer  = k;
														}
														else {
															if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
																RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
																FM_XBuffer  = k;
															}
														}
													}
													else {
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
														FM_XBuffer  = k;
													}
												}
												else {
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_XBuffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_XBuffer ) == BUFFER_FM_STATION ) ) {
													}
													else {
														if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
															RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
															FM_XBuffer  = k;
														}
													}
												}
											}
											//
											//
										}
									}
									//
									//
									//========================================================================================
									//
									// START 2017.01.06
									//
									if ( FM_XBuffer == -1 ) {
										if ( FM_RejectBuffer >= BM1 ) {
											FM_XBuffer = FM_RejectBuffer;
										}
									}
									//
									// END 2017.01.06
									//
									//========================================================================================
									//
									FM_AL_Buffer = FM_XBuffer; // 2010.07.07
								}
							}
							else {
								//=========================================================================================================
								// 2007.02.16
								//=========================================================================================================
								for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
									//
									//========================================================================================
									//
									// START 2017.01.06
									//
									if ( FM_RejectBuffer == k ) continue;
									//
									// END 2017.01.06
									//
									//========================================================================================
									//
									if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
									//
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
									//
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
										//
										if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
											if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
										}
										//
										if ( FM_Buffer == -1 ) {
											FM_Buffer = k;
											break;
										}
									}
								}
								//
								//
								//========================================================================================
								//
								// START 2017.01.06
								//
								//
//								if ( FM_Buffer  <= 0 ) FM_Buffer = 0;
								//
								if ( FM_Buffer <= 0 ) {
									if ( FM_RejectBuffer >= BM1 ) {
										FM_Buffer = FM_RejectBuffer;
									}
									else {
										FM_Buffer = 0;
									}
								}
								//
								// END 2017.01.06
								//
								//========================================================================================
								//
								//
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_Side , FM_Pointer , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2007.07.10
										FM_XBuffer = ( FM_gpmc * 100 ) + FM_Buffer;
										if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_Side2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2007.07.10
											FM_XBuffer = FM_XBuffer + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
										} 
										else {
											FM_XBuffer = FM_XBuffer + ( FM_Buffer * 10000 );
										}
									}
									else {
										FM_XBuffer = FM_Buffer;
										if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_Side2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2007.07.10
											FM_XBuffer = FM_XBuffer + ( ( ( FM_gpmc * 100 ) + FM_Buffer ) * 10000 );
										}
										else {
											FM_XBuffer = FM_XBuffer + ( FM_Buffer * 10000 );
										}
									}
								}
								else if ( FM_Slot > 0 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_Side , FM_Pointer , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2007.07.10
										FM_XBuffer = ( FM_gpmc * 100 ) + FM_Buffer;
									}
									else {
										FM_XBuffer = FM_Buffer;
									}
								}
								else if ( FM_Slot2 > 0 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 11 ) , FM_Side2 , FM_Pointer2 , TRUE , FALSE , &FM_gpmc ) == 1 ) { // 2007.07.10
										FM_XBuffer = ( FM_gpmc * 100 ) + FM_Buffer;
									}
									else {
										FM_XBuffer = FM_Buffer;
									}
								}
								//
								FM_AL_Buffer = FM_Buffer; // 2010.07.07
							}
							//====================================================================================================
							//
							/*
							// 2014.08.26
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_XBuffer , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
								//-----------------------------------------------
								if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
									_SCH_MODULE_Set_FM_MidCount( 0 );
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								}
								return 0;
							}
							//
							*/
							//
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 77-2 Place_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) FM_RejectBuffer(%d) FM_XBuffer(%d) FM_AL_Buffer(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_RejectBuffer , FM_XBuffer , FM_AL_Buffer );
							//----------------------------------------------------------------------
							//========================================================================================
							//
							// START 2017.01.06
							//
							if ( FM_RejectBuffer == -2 ) {
							//
							// END 2017.01.06
							//
							//========================================================================================
								//
								// 2014.08.26
								//
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
									//
									if ( SCHEDULER_ALIGN_SKIP_AL0( FM_Side , FM_Pointer ) ) {
										if ( SCHEDULER_ALIGN_SKIP_AL0( FM_Side2 , FM_Pointer2 ) ) {
											//
										}
										else {
											//
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , 0 , FM_Slot2 , FM_CM , FM_XBuffer , TRUE , 0 , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + 0 , 9 , FM_Slot2 * 100 + 0 , 9 , FM_Slot2 * 100 + 0 );
												//-----------------------------------------------
												if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
													_SCH_MODULE_Set_FM_MidCount( 0 );
													LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
												}
												return 0;
											}
											//
										}
									}
									else {
										if ( SCHEDULER_ALIGN_SKIP_AL0( FM_Side2 , FM_Pointer2 ) ) {
											//
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , 0 , FM_CM , FM_XBuffer , TRUE , FM_Slot , 0 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , 0 * 100 + FM_Slot , 9 , 0 * 100 + FM_Slot , 9 , 0 * 100 + FM_Slot );
												//-----------------------------------------------
												if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
													_SCH_MODULE_Set_FM_MidCount( 0 );
													LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
												}
												return 0;
											}
											//
										}
										else {
											//
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_XBuffer , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
												//-----------------------------------------------
												if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
													_SCH_MODULE_Set_FM_MidCount( 0 );
													LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
												}
												return 0;
											}
											//
										}
									}
									//
								}
								else if ( FM_Slot > 0 ) {
									//
									if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_Side , FM_Pointer ) ) {
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_XBuffer , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
											//-----------------------------------------------
											if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
												_SCH_MODULE_Set_FM_MidCount( 0 );
												LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
											}
											return 0;
										}
										//
									}
									//
								}
								else if ( FM_Slot2 > 0 ) {
									//
									if ( !SCHEDULER_ALIGN_SKIP_AL0( FM_Side2 , FM_Pointer2 ) ) {
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_XBuffer , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2007.07.10
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
											//-----------------------------------------------
											if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
												_SCH_MODULE_Set_FM_MidCount( 0 );
												LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
											}
											return 0;
										}
										//
									}
									//
								}
								//===============================================
								FM_Buffer = -1; // 2007.01.15
								//===============================================
							//
							//========================================================================================
							//
							// START 2017.01.06
							//
							}
							else {
								//
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 77 SWAP_2FM_TH_AL = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) FM_RejectBuffer(%d) FM_XBuffer(%d) FM_AL_Buffer(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_RejectBuffer , FM_XBuffer , FM_AL_Buffer );
							//----------------------------------------------------------------------
								switch( SWAP_2FM_TH_AL( CHECKING_SIDE , &FM_Slot , &FM_Side , &FM_Pointer , &FM_Slot2 , &FM_Side2 , &FM_Pointer2 , FM_AL_Buffer , FM_XBuffer ) ) {
								case 1 :
									return 1;
								case 0 :
									//-----------------------------------------------
									if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
										_SCH_MODULE_Set_FM_MidCount( 0 );
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
									}
									return 0;
									break;
								}
								//
								FM_AL_Buffer = FM_RejectBuffer;
								//
								FM_Buffer = -1;
								//
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 77-3 Place_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) FM_RejectBuffer(%d) FM_XBuffer(%d) FM_AL_Buffer(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_RejectBuffer , FM_XBuffer , FM_AL_Buffer );
							//----------------------------------------------------------------------
							//
							// END 2017.01.06
							//
							//========================================================================================
						}
						//----------------------------------------------------------------------
						//###################################################################################
						// Disapear Check (2001.11.28) // 2002.06.07
						//###################################################################################
						if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
							if ( FM_Slot > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_Slot = 0;
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_Slot2 = 0;
								}
							}
						}
						if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
							//-----------------------------------------------
							if ( ( Runmode == 2 ) || ( Runmode == 5 ) ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							}
							//----------------------------------------------
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//-----------------------------------------------
							return 1;
						}
						//###################################################################################
						//----------------------------------------------------------------------
					}
					//----------------------------------------------------------------------
					if ( ( Runmode != 2 ) && ( Runmode != 5 ) ) {
						EnterCriticalSection( &CR_SINGLEBM_STYLE_0 );
						_SCH_MODULE_Set_FM_MidCount( 11 );
					}
					//----------------------------------------------------------------------
					if ( Runmode == 3 ) {
						_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
					}
					//----------------------------------------------------------------------
					FM_Buffer = -1;
					if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
						if ( ( Runmode != 2 ) && ( Runmode != 5 ) ) {
							if      ( FM_Slot  > 0 ) k = FM_Slot;
							else if ( FM_Slot2 > 0 ) k = FM_Slot2;
							if ( SCHEDULING_CHECK_Enable_PICK_CM_or_PLACE_to_BM_at_SINGLE_MODE( 0 , CHECKING_SIDE , k ) < 0 ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								//----------------------------------------------
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
								//----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//----------------------------------------------
								return 1;
							}
						}
						//----------------------------------------------------------------------
						k = Get_Prm_MODULE_BUFFER_SINGLE_MODE();
						RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
						if ( RunBuffer > 0 ) {
							FM_Buffer = k;
						}
					}
					else {
						//
						RemainTime = 999999; // 2010.07.05
						//
						for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
							if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
							//
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
							//
							if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) {
								//
								if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
									if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
								}
								//
								//
//								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2017.01.06
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlotBuf1 , &FM_TSlotBuf2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2017.01.06
								//
								//
								if ( RunBuffer > 0 ) {
									//=====================================================================================================================
									// 2006.07.10
									//=====================================================================================================================
									if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
										if      ( FM_Slot  > 0 ) {
											if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) {
												RunBuffer = 0;
												continue;
											}
										}
										else if ( FM_Slot2 > 0 ) {
											if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) {
												RunBuffer = 0;
												continue;
											}
										}
									}
									//
									//=====================================================================================================================
									//
									if ( ( FM_AL_Buffer > 0 ) && Scheduling_PT_and_1S_Same() ) { // 2016.11.01
										//
										if ( FM_AL_Buffer == k ) {
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
												RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												FM_Buffer = k;
												//
												FM_TSlot = FM_TSlotBuf1; // 2017.01.06
												FM_TSlot2 = FM_TSlotBuf2; // 2017.01.06
												//
												break;
											}
										}
										//
									}
									//
									//=====================================================================================================================
									//
									else {
										if ( !Scheduling_TM_Double( FALSE ) || ( RunBuffer >= 2 ) ) { // 2003.03.26
											if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.07
	//											FM_Buffer = k; // 2010.07.05
	//											break; // 2010.07.05
												if ( FM_AL_Buffer == k ) { // 2010.07.07
													FM_Buffer = k;
													//
													FM_TSlot = FM_TSlotBuf1; // 2017.01.06
													FM_TSlot2 = FM_TSlotBuf2; // 2017.01.06
													//
													RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
													break;
												}
												else {
													if ( FM_Buffer == -1 ) { // 2010.07.05
														FM_Buffer = k;
														//
														FM_TSlot = FM_TSlotBuf1; // 2017.01.06
														FM_TSlot2 = FM_TSlotBuf2; // 2017.01.06
														//
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
													}
													else { // 2010.07.05
														if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
															RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
															FM_Buffer  = k;
															//
															FM_TSlot = FM_TSlotBuf1; // 2017.01.06
															FM_TSlot2 = FM_TSlotBuf2; // 2017.01.06
															//
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
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 81 Place_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) FM_RejectBuffer(%d) FM_XBuffer(%d) FM_AL_Buffer(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_RejectBuffer , FM_XBuffer , FM_AL_Buffer );
					//----------------------------------------------------------------------
					//
					if ( FM_Buffer == -1 ) {
						if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
							if ( Runmode != 5 ) {
								_SCH_MODULE_Set_FM_MidCount( 0 );
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 END_PICK_from_CM = Slot(%d,%d)\n" , FM_Slot , FM_Slot2 );
								//----------------------------------------------------------------------
								_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
								//----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//----------------------------------------------
								return 1;
							}
						}
						else {
							if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
								RemainTime = 999999;
								for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
									if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
									//
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
									//
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
										//
										if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
											if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
										}
										//
										//=====================================================================================================================
										//
										if ( ( FM_AL_Buffer > 0 ) && Scheduling_PT_and_1S_Same() ) { // 2016.11.01
											//
											if ( FM_AL_Buffer == k ) {
												RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												FM_Buffer = k;
												break;
											}
											//
										}
										//
										//=====================================================================================================================
										//
										else {
											if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
												RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
												FM_Buffer  = k;
											}
										}
										//
									}
								}
								if ( FM_Buffer > 0 ) {
									FM_TSlot = 1;
									FM_TSlot2 = 0;
									if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
										FM_TSlot = 1;
										FM_TSlot2 = 2;
									}
									else if ( FM_Slot > 0 ) {
										if ( Scheduling_TM_Double( FALSE ) ) {
											if ( FM_HSide == HANDLING_B_SIDE_ONLY ) {
												FM_TSlot = 0;
												FM_TSlot2 = 2;
											}
										}
									}
									else if ( FM_Slot2 > 0 ) {
										FM_TSlot = 0; // 2008.02.12
										FM_TSlot2 = 1; // 2008.02.12
										if ( Scheduling_TM_Double( FALSE ) ) {
											if ( FM_HSide2 == HANDLING_B_SIDE_ONLY ) {
												FM_TSlot = 0;
												FM_TSlot2 = 2;
											}
										}
									}
									if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , FM_TSlot , FM_TSlot2 ) == SYS_ABORTED ) {
										//-----------------------------------------------
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										//-----------------------------------------------
										return 0;
									}
								}
								FM_Buffer = -1;
							}
						}
					}
					//
					Half_CM_Placed = FALSE; // 2017.01.20
					//
					Cont_True = FALSE;
					while ( TRUE ) {
						//-----------------------------------------------
						_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 12 );
						//-----------------------------------------------
						Cont_True = FALSE;
						while ( TRUE ) {
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck );
							//----------------------------------------------------------------------
							if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 6 from FEM%cWHFMFAIL 6\n" , 9 );
								//-----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//-----------------------------------------------
								return 0;
							}
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//###################################################################################
							// Disapear Check (2001.11.28) // 2002.06.07
							//###################################################################################
							if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
								if ( FM_Slot > 0 ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
								}
								if ( FM_Slot2 > 0 ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 WAIT_PLACE_to_BM[2] = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck );
							//----------------------------------------------------------------------
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
								Cont_True = TRUE;
								break;
							}
							//###################################################################################
							//----------------------------------------------------------------------
							if ( FM_Buffer > 0 ) break; // 2007.01.15
							//----------------------------------------------------------------------
							// 2004.09.09
							//----------------------------------------------------------------------
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
								if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) {
									PlaceCheck = TRUE;
									break;
								}
							}
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
								if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) {
									PlaceCheck = TRUE;
									break;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 WAIT_PLACE_to_BM[3] = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck );
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							Sleep(1);
							//----------------------------------------------------------------------
							//
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
								k = Get_Prm_MODULE_BUFFER_SINGLE_MODE();
								RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 WAIT_PLACE_to_BM[4] = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d) RunBuffer=%d\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer );
							//----------------------------------------------------------------------
								if ( RunBuffer > 0 ) {
									FM_Buffer = k;
									break;
								}
							}
							else {
								//=====================================================================
								l = 0; // 2006.10.09
								//=====================================================================
								for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
									if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
									//
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
									//
									if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 1 ) ) {
										//
										if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-21 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d) (%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer );
											//----------------------------------------------------------------------
											if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue; // 2009.08.26
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-22 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d) (%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer );
											//----------------------------------------------------------------------
										}
										//
										//
//										RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlot , &FM_TSlot2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2017.01.06
										RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlotBuf1 , &FM_TSlotBuf2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ); // 2017.01.06
										//
										//
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-23 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d) (%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer );
										//----------------------------------------------------------------------
										if ( RunBuffer > 0 ) {
											//=====================================================================================================================
											// 2006.07.10
											//=====================================================================================================================
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if      ( FM_Slot  > 0 ) {
													if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) {
														RunBuffer = 0;
														continue;
													}
												}
												else if ( FM_Slot2 > 0 ) {
													if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) {
														RunBuffer = 0;
														continue;
													}
												}
											}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-24 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d) (%d) (l=%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer , l );
							//----------------------------------------------------------------------
											//
											//=====================================================================================================================
											//
											if ( ( FM_AL_Buffer > 0 ) && Scheduling_PT_and_1S_Same() ) { // 2016.11.01
												//
												if ( FM_AL_Buffer == k ) {
													//
													if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 2 ) l = 1;
													//
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
														FM_Buffer = k;
														//
														FM_TSlot = FM_TSlotBuf1; // 2017.01.06
														FM_TSlot2 = FM_TSlotBuf2; // 2017.01.06
														//
														break;
													}
												}
												//
											}
											//
											//=====================================================================================================================
											//
											else {
												if ( !Scheduling_TM_Double( FALSE ) || ( RunBuffer >= 2 ) ) { // 2003.03.26
													//=====================================================================
													// 2006.10.09
													//=====================================================================
													if ( _SCH_PRM_GET_MODULE_SIZE( k ) > 2 ) l = 1;
													//=====================================================================
													if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2007.12.08
														FM_Buffer = k;
														//
														FM_TSlot = FM_TSlotBuf1; // 2017.01.06
														FM_TSlot2 = FM_TSlotBuf2; // 2017.01.06
														//
														break;
													}
												}
											}
											//=====================================================================================================================
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-25 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) Buffer(%d) TSlot(%d,%d) PlaceCheck(%d) (%d) (l=%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer , l );
							//----------------------------------------------------------------------
										}
									}
								}
								//=====================================================================
								// 2006.10.09
								//=====================================================================
								if ( l == 1 ) {
									if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
										if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
											for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
												if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
												if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
													if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 1 ) ) {
														if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( k , -1 ) > 0 ) {
															PlaceCheck = TRUE;
															break;
														}
													}
												}
											}
											if ( PlaceCheck ) break;
										}
									}
								}
							}
							//======================================================================================
							// 2007.12.08
							//======================================================================================
							//
							// START 2017.01.07
							//
//							FM_XBuffer = 0; // 2017.01.20
							//
							if ( FM_Buffer <= 0 ) {
								//
								/*
								//
								// 2017.01.20
								//
								//
								if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() <= 0 ) {
									//
									for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
										//
										if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
										//
										if ( _SCH_MODULE_Get_Mdl_Use_Flag( ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , k ) == MUF_00_NOTUSE ) continue; // 2009.11.05
										//
										if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PLACE , 0 ) ) {
											//
											if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
												if ( !SCHEDULER_CONTROL_Fixed_BM_Run_Check_A0SUB4( 0 , k , ( FM_Slot > 0 ) ? FM_Side : FM_Side2 , ( FM_Slot > 0 ) ? FM_Pointer : FM_Pointer2 , BUFFER_INWAIT_STATUS ) ) continue;
											}
											//
											RunBuffer = SCHEDULER_CONTROL_Chk_BM_FREE_SLOT( k , &FM_TSlotBuf1 , &FM_TSlotBuf2 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) );
											//
											if ( RunBuffer > 0 ) {
												if ( Get_Sub_Alg_Data_STYLE_0() == A0_SUB_ALGORITHM_NO4 ) {
													if      ( FM_Slot  > 0 ) {
														if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) {
															RunBuffer = 0;
															continue;
														}
													}
													else if ( FM_Slot2 > 0 ) {
														if ( !SCHEDULER_CONTROL_BMPLACE_POSSIBLE_A0SUB4( k , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) {
															RunBuffer = 0;
															continue;
														}
													}
												}
												//
												if ( ( FM_AL_Buffer > 0 ) && Scheduling_PT_and_1S_Same() ) {
													//
													if ( FM_AL_Buffer == k ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
															FM_XBuffer = k;
															break;
														}
													}
													//
												}
												//
												//=====================================================================================================================
												//
												else {
													if ( !Scheduling_TM_Double( FALSE ) || ( RunBuffer >= 2 ) ) {
														if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( k ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
															FM_XBuffer = k;
															break;
														}
													}
												}
											}
										}
									}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07-61 WAIT_PLACE_to_BM = Side(%d,%d) Slot(%d,%d) XBuffer(%d) TSlot(%d,%d) PlaceCheck(%d) (%d)\n" , FM_Side , FM_Side2 , FM_Slot , FM_Slot2 , FM_XBuffer , FM_TSlot , FM_TSlot2 , PlaceCheck , RunBuffer );
//----------------------------------------------------------------------
								}
								//
								// END 2017.01.07
								//
								//
								*/
								//
//								if ( FM_XBuffer <= 0 ) { // 2017.01.07 // 2017.01.20
								if ( !Half_CM_Placed ) { // 2017.01.07 // 2017.01.20
									//
	//								if ( !SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( CHECKING_SIDE ) ) { // 2017.01.07
//									if ( SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( CHECKING_SIDE , FALSE ) == 0 ) { // 2017.01.07 // 2017.01.20
									if ( SCHEDULER_CONTROL_Chk_BM_ONE_SLOT_AND_NO_MOVE_AFTER_PICK( CHECKING_SIDE , TRUE , &Half_CM_Placed , FALSE ) == 0 ) { // 2017.01.07 // 2017.01.20
										//-----------------------------------------------
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										//-----------------------------------------------
										return 0;
									}
									//
									if ( Half_CM_Placed ) continue; // 2017.01.20
									//
								}
								else { // 2017.01.20
									//
									Half_CM_Placed = FALSE;
									//
									if ( !SCHEDULER_CONTROL_Run_Half_Placed_CM() ) {
										//-----------------------------------------------
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										//-----------------------------------------------
										return 0;
									}
								}
								//
								//
							}
							//======================================================================================
//							if ( ( FM_Buffer <= 0 ) && ( FM_XBuffer <= 0 ) ) { // 2017.01.07
							if ( FM_Buffer <= 0 ) { // 2017.01.20
								//
								if ( !Half_CM_Placed ) { // 2017.01.07 // 2017.01.20
									//
		//							if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , FALSE , &k ) ) { // 2012.08.10 // 2015.03.25
//									if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , FALSE , &k , -1 , -1 ) ) { // 2012.08.10 // 2015.03.25 // 2017.01.20
									if ( !Scheduler_Other_Arm_Cool_Return( CHECKING_SIDE , -1 , FALSE , &k , -1 , -1 , TRUE , &Half_CM_Placed ) ) { // 2012.08.10 // 2015.03.25 // 2017.01.20
										//-----------------------------------------------
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										//-----------------------------------------------
										return 0;
									}
									//
									if ( Half_CM_Placed ) continue; // 2017.01.20
									//
								}
								else { // 2017.01.20
									//
									Half_CM_Placed = FALSE;
									//
									if ( !SCHEDULER_CONTROL_Run_Half_Placed_CM() ) {
										//-----------------------------------------------
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										//-----------------------------------------------
										return 0;
									}
								}
							}
							//======================================================================================
						}
						//===============================================================================================================
						// 2004.09.09
						//===============================================================================================================
//						if ( PlaceCheck ) break; // 2017.01.20
						//
						if ( PlaceCheck ) { // 2017.01.20
							if ( Half_CM_Placed ) {
								if ( !SCHEDULER_CONTROL_Run_Half_Placed_CM() ) {
									//-----------------------------------------------
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
									//-----------------------------------------------
									return 0;
								}
							}
							break;
						}
						//===============================================================================================================
						if ( Cont_True ) {
							_SCH_MODULE_Set_FM_MidCount( 0 );
							//----------------------------------------------
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//----------------------------------------------
							LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							//----------------------------------------------
							return 1;
							//----------------------------------------------
						}
						//
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 START_PLACE_to_BM = Slot(%d,%d) Buffer(%d) TSlot(%d,%d)\n" ,FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 );
						//----------------------------------------------------------------------
						_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 13 );
						//-----------------------------------------------
						//==========================================================================================
						// Code for Place to BI
						// FM_Slot
						// FM_Slot2
						//==========================================================================================
						if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
							if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) {
								if ( _SCH_MODULE_Get_BM_MidCount() != 1 ) {
									if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 2  ) == SYS_ABORTED ) {
										//----------------------------------------------
										LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
										//----------------------------------------------
										return 0;
									}
									_SCH_MODULE_Set_BM_MidCount( 1 );
								}
							}
						}
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
							if (
								( FM_HSide  == HANDLING_B_SIDE_ONLY ) ||
								( FM_HSide2 == HANDLING_A_SIDE_ONLY )
								) {
								k = FM_TSlot2;
								FM_TSlot2 = FM_TSlot;
								FM_TSlot = k;
							}
							if ( RunBuffer >= 2 ) {
								//==================================================================
								// 2007.11.23
								//==================================================================
								switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , ( ( RunBuffer - 2 ) == 0 ) ) ) { // 2007.03.21
								case -1 :
									//----------------------------------------------
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
									//----------------------------------------------
									return 0;
									break;
								}
								//==================================================================
								//###################################################################################
								// Disapear Check (2001.11.28) // 2002.06.07
								//###################################################################################
								if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
									if ( FM_Slot > 0 ) {
										if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
											FM_Slot = 0;
											FM_TSlot = 0;
										}
									}
									if ( FM_Slot2 > 0 ) {
										if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
											FM_Slot2 = 0;
											FM_TSlot2 = 0;
										}
									}
								}
								//###################################################################################
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
								//----------------------------------------------------------------------
								//
								if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
									Cont_True = TRUE;
									break;
								}
								break;
							}
							else if ( RunBuffer == 1 ) {
								//==================================================================
								// 2007.11.23
								//==================================================================
								switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 ) ) { // 2007.03.21
								case -1 :
									//----------------------------------------------
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
									//----------------------------------------------
									return 0;
									break;
								}
								//==================================================================
								//----------------------------------------------------------------------
								//###################################################################################
								// Disapear Check (2001.11.28) // 2002.06.07
								//###################################################################################
								if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
									if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
								}
								//###################################################################################
								//----------------------------------------------------------------------
								//----------------------------------------------------------------------
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , -1 , 1 , 0 , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 , -1 ); // 2007.11.26
								//----------------------------------------------------------------------
								//----------------------------------------------------------------------
								//
								if ( FM_Slot > 0 ) {
									//----------------------------------------------------------------------
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
									//----------------------------------------------------------------------
									FM_Slot = 0;
									//=====================================================================
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 5 );
										if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 2 );
									}
								}
								if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
									Cont_True = TRUE;
									break;
								}
								//=====================================================================
							}
						}
						else if ( FM_Slot > 0 ) {
							FM_TSX = FM_TSlot;
							FM_TSX2 = FM_Slot;
							if ( Scheduling_TM_Double( FALSE ) ) {
								if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
									if      ( FM_HSide == HANDLING_A_SIDE_ONLY ) {
										FM_TSX = FM_TSlot;
										//FM_TSX2 = FM_Slot; // 2008.04.10
									}
									else if ( FM_HSide == HANDLING_B_SIDE_ONLY ) {
										FM_TSX = FM_TSlot2;
										//FM_TSX2 = FM_Slot2; // 2008.04.10
									}
								}
							}

							//==================================================================
							// 2007.11.23
							//==================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_TSX2 , FM_TSX , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 ) ) { // 2007.03.21
							case -1 :
								//----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//----------------------------------------------
								return 0;
								break;
							}
							//==================================================================
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//###################################################################################
							// Disapear Check (2001.11.28) // 2002.06.07
							//###################################################################################
							if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
									FM_Slot = 0;
									FM_TSlot = 0;
								}
							}
							//###################################################################################
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , -1 , 1 , 0 , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 , -1 ); // 2007.11.26
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//
							if ( FM_Slot > 0 ) {
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSX , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
								//----------------------------------------------------------------------
							}
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
								Cont_True = TRUE;
								break;
							}
							break;
						}
						else if ( FM_Slot2 > 0 ) {
						//
						FM_TSX2 = 0;
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 START_PLACE_to_BM[6] = Slot(%d,%d) Buffer(%d) TSlot(%d,%d)\n" ,FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_TSX , FM_TSX2 );
						//----------------------------------------------------------------------
						//
							FM_TSX = FM_TSlot;
//								FM_TSX2 = FM_Slot; // 2007.12.08
							FM_TSX2 = FM_Slot2; // 2007.12.08
						//
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 START_PLACE_to_BM[7] = Slot(%d,%d) Buffer(%d) TSlot(%d,%d)\n" ,FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_TSX , FM_TSX2 );
						//----------------------------------------------------------------------
						//
							if ( Scheduling_TM_Double( FALSE ) ) {
								if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
									if      ( FM_HSide2 == HANDLING_A_SIDE_ONLY ) {
										FM_TSX = FM_TSlot;
										FM_TSX2 = FM_Slot;
									}
									else if ( FM_HSide2 == HANDLING_B_SIDE_ONLY ) {
										FM_TSX = FM_TSlot2;
										FM_TSX2 = FM_Slot2;
									}
								}
							}
						//
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 START_PLACE_to_BM[8] = Slot(%d,%d) Buffer(%d) TSlot(%d,%d)\n" ,FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 , FM_TSX , FM_TSX2 );
						//----------------------------------------------------------------------
						//
							//==================================================================
							// 2007.11.23
							//==================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , FM_TSX2 , FM_TSX , FM_Side2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
							case -1 :
								//----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//----------------------------------------------
								return 0;
								break;
							}
							//==================================================================
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//###################################################################################
							// Disapear Check (2001.11.28) // 2002.06.07
							//###################################################################################
							if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
									FM_Slot = 0;
									FM_TSlot = 0;
								}
							}
							//###################################################################################
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , -1 , 1 , 0 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
							//----------------------------------------------------------------------
							//----------------------------------------------------------------------
/*
// 2007.11.26
							SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 );
*/
							//----------------------------------------------------------------------
							//
							if ( FM_Slot2 > 0 ) {
/*
// 2007.11.23
								//================================================================================================================
								SCHEDULER_MACROx__SCH_LOG_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_Buffer , 0 , 0 , 0 , 0 , FM_TSX2 , FM_TSX , FM_Side2 , FM_Pointer2 );
								//================================================================================================================
*/
/*
// 2007.11.26
//									SCHEDULER_CONTROL_Set_BMx_SIDE_POINTER( FM_Buffer , FM_TSX , CHECKING_SIDE , FM_Pointer2 ); // 2007.07.10
								SCHEDULER_CONTROL_Set_BMx_SIDE_POINTER( FM_Buffer , FM_TSX , FM_Side2 , FM_Pointer2 ); // 2007.07.10
								SCHEDULER_CONTROL_Set_BMx_WAFER_STATUS( FM_Buffer , FM_TSX , FM_Slot2 , BUFFER_INWAIT_STATUS );
*/
								//----------------------------------------------------------------------
								_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , 0 , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSX , FM_Side2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
								//----------------------------------------------------------------------
							}
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
								Cont_True = TRUE;
								break;
							}
							//
							break;
						}
						FM_Buffer = -1;
					}
					//===============================================================================================================
					// 2004.09.09
					//===============================================================================================================
					if ( PlaceCheck ) {
						//
						if ( Half_CM_Placed ) { // 2017.01.20
							//
							if ( !SCHEDULER_CONTROL_Run_Half_Placed_CM() ) {
								//-----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//-----------------------------------------------
								return 0;
							}
							//
						}
						//
						LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
					}
					else {
						if ( Cont_True ) {
							//
							if ( Half_CM_Placed ) { // 2017.01.20
								//
								if ( !SCHEDULER_CONTROL_Run_Half_Placed_CM() ) {
									//-----------------------------------------------
									LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
									//-----------------------------------------------
									return 0;
								}
								//
							}
							//
							_SCH_MODULE_Set_FM_MidCount( 0 );
							//----------------------------------------------
							_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
							//----------------------------------------------
							LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
							//----------------------------------------------
							return 1;
						}
						if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) || _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
							//=====================================================================
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
								//==========================================================================
								EnterCriticalSection( &CR_FEM_TM_EX_STYLE_0 ); // 2006.06.26
								//==========================================================================
								if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) { // 2006.06.26
								//==========================================================================
									if ( _SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -2 , -1 , -1 ) ) { // 2003.01.13
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 6 );
										if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 2 );
									}
									else {
										if ( Scheduling_TM_Double( FALSE ) ) { // 2003.03.28
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) ) {
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 7 );
											}
										}
										else {
											if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL( FM_Buffer ) ) {
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 8 );
												if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 2 );
											}
										}
									}
								//==========================================================================
								}
								//==========================================================================
								LeaveCriticalSection( &CR_FEM_TM_EX_STYLE_0 ); // 2006.06.26
								//==========================================================================
							}
							//=====================================================================
						}
						else {
							//=====================================================================
							if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( FM_Buffer ) ) {
								if ( Scheduling_TM_Double( FALSE ) ) { // 2003.03.28
									if ( !_SCH_MODULE_Get_FM_HWait( CHECKING_SIDE ) || SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( FM_Buffer , -1 ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 9 );
									}
								}
								else {
									if ( !_SCH_MODULE_Get_FM_HWait( CHECKING_SIDE ) || SCHEDULER_CONTROL_Chk_BM_FULL_ALL( FM_Buffer ) ) {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , ( FM_Slot > 0 ) ? FM_Slot : FM_Slot2 , TRUE , 0 , 10 );
										if ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() > 0 ) _SCH_MODULE_Set_BM_MidCount( 2 );
									}
								}
							}
							//=====================================================================
						}
						//
						if ( Half_CM_Placed ) { // 2017.01.20
							//
							if ( !SCHEDULER_CONTROL_Run_Half_Placed_CM() ) {
								//-----------------------------------------------
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
								//-----------------------------------------------
								return 0;
							}
							//
						}
						//
						_SCH_MODULE_Set_FM_MidCount( 0 );
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 08 END_PLACE_to_BM = Slot(%d,%d) Buffer(%d) TSlot(%d,%d)\n" ,FM_Slot , FM_Slot2 , FM_Buffer , FM_TSlot , FM_TSlot2 );
						//----------------------------------------------
						_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
						//----------------------------------------------------------------------
						LeaveCriticalSection( &CR_SINGLEBM_STYLE_0 );
						//----------------------------------------------
						return 1;
					}
				}
			}
		}
	}
	//================================================================================================================================================================
	//================================================================================================================================================================
	//================================================================================================================================================================
	//=============================================================================================
	PlaceCheck = FALSE; // 2004.09.09
	//=============================================================================================
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PLACE TO CM for MIC
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
		//Pick M_FIC & Place CM
		//
		if ( !Scheduler_Other_Side_Supply_Waiting_Cool_Cancel( CHECKING_SIDE ) ) { // 2018.08.14
			//
	//		if ( _SCH_MODULE_Chk_MFIC_HAS_OUT_SIDE( FALSE , CHECKING_SIDE , &FM_Side2 , &FM_TSlot , &FM_TSlot2 ) > 0 ) { // 2011.07.11 (Testing)
			if ( _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &FM_Side2 , &FM_TSlot , &FM_TSlot2 ) == SYS_SUCCESS ) { // 2011.07.11 (Testing)
				if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
					//===========================================================================
					// 2006.11.14
					//===========================================================================
					if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
							FM_TSlot = 0;
							FM_TSlot2 = 0;
						}
						else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) {
							FM_TSlot2 = FM_TSlot;
							FM_TSlot = 0;
						}
						else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
							FM_TSlot2 = 0;
						}
					}
					else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
						if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) FM_TSlot = 0;
						FM_TSlot2 = 0;
					}
					else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
						FM_TSlot2 = FM_TSlot;
						FM_TSlot = 0;
						if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) FM_TSlot2 = 0;
					}
					else {
						FM_TSlot = 0;
						FM_TSlot2 = 0;
					}
					//===========================================================================
					//if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
					//	FM_TSlot = 0;
					//}
					//if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					//	FM_TSlot2 = 0;
					//}
					//===========================================================================
				}
				else {
	//					if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2006.11.14
					if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2006.11.14
						FM_TSlot2 = FM_TSlot;
						FM_TSlot = 0;
					}
	//					if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2006.11.14
					if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.11.14
						FM_TSlot2 = 0;
					}
				}
				//----------------------------------------------
				if ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) { // 2006.11.14
					if ( FM_TSlot > 0 ) {
						//_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE ); // 2004.09.09
						FM_Side    = _SCH_MODULE_Get_MFIC_SIDE( FM_TSlot ); // 2011.07.20
						FM_Pointer = _SCH_MODULE_Get_MFIC_POINTER( FM_TSlot );
						FM_Slot = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot ); // 2005.11.29
					}
					else {
						FM_Pointer = 0;
						FM_Slot = 0; // 2005.11.29
					}
					if ( FM_TSlot2 > 0 ) {
						//_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE ); // 2004.09.09
						FM_Side2    = _SCH_MODULE_Get_MFIC_SIDE( FM_TSlot2 ); // 2011.07.20
						FM_Pointer2 = _SCH_MODULE_Get_MFIC_POINTER( FM_TSlot2 );
						FM_Slot2 = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot2 ); // 2005.11.29
					}
					else {
						FM_Pointer2 = 0;
						FM_Slot2 = 0; // 2005.11.29
					}
	//					if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) { // 2007.03.19
	//						FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2007.03.19
	//					} // 2007.03.19
	//					else if ( FM_TSlot > 0 ) { // 2007.03.19
					if ( FM_TSlot > 0 ) {
	//					FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2011.07.21
						FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ); // 2011.07.21
					}
					else if ( FM_TSlot2 > 0 ) {
	//					FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 ); // 2011.07.21
						FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ); // 2011.07.21
					}
					//-----------------------------------------------
					_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 22 );
					//-----------------------------------------------
					_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
					//----------------------------------------------
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 START_PLACE_to_CM = Side(%d,%d) Pointer(%d,%d)\n" , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 );
					//----------------------------------------------------------------------
					//
					// Pick Cool
					//
					//----------------------------------------------------------------------
	/* 2007.03.19
					//=============================================================================================
					// 2006.11.14
					//=============================================================================================
					if ( FM_TSlot > 0 ) {
						FM_Slot = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot );
					}
					else {
						FM_Slot = 0;
					}
					if ( FM_TSlot2 > 0 ) {
						FM_Slot2 = _SCH_MODULE_Get_MFIC_WAFER( FM_TSlot2 );
					}
					else {
						FM_Slot2 = 0;
					}
	*/
					//=============================================================================================
					if ( FM_TSlot > 0 ) {
	//					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , IC , FM_TSlot , 0 , FM_CM , -1 , TRUE , FM_Slot , 0 , CHECKING_SIDE , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2011.07.21
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_Side , IC , FM_TSlot , 0 , FM_CM , -1 , TRUE , FM_Slot , 0 , FM_Side , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2011.07.21
							_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
							return 0;
						}
						//----------------------------------------------------------------------
	//					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , FM_Slot , FM_TSlot , CHECKING_SIDE , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 , -1 ); // 2007.11.26 // 2011.07.21
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 , 0 , -1 , -1 ); // 2007.11.26 // 2011.07.21
						//----------------------------------------------------------------------
	/*
	// 2007.11.26
						SCHEDULER_CONTROL_Set_MFICx_WAFER( FM_TSlot , 0 );
						//
						SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( TA_STATION , FM_Pointer , -1 );
						//
						SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot );
	*/
					}
					if ( FM_TSlot2 > 0 ) {
	//					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , IC , 0 , FM_TSlot2 , FM_CM , -1 , TRUE , 0 , FM_Slot2 , 0 , CHECKING_SIDE , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2011.07.21
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_Side2 , IC , 0 , FM_TSlot2 , FM_CM , -1 , TRUE , 0 , FM_Slot2 , 0 , FM_Side2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2011.07.21
							_SCH_LOG_LOT_PRINTF( FM_Side2 , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot , 9 , FM_Slot2 * 100 + FM_Slot );
							return 0;
						}
						//----------------------------------------------------------------------
	//					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , IC , FM_Slot2 , FM_TSlot2 , CHECKING_SIDE , FM_Pointer2 , -1 , -1 ); // 2007.11.26 // 2011.07.21
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , 0 , 0 , 0 , 0 , 0 , IC , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26 // 2011.07.21
						//----------------------------------------------------------------------
	/*
	// 2007.11.26
						SCHEDULER_CONTROL_Set_MFICc_WAFER( FM_TSlot2 , 0 );
						//
						SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( TB_STATION , FM_Pointer2 , -1 );
						//
						SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 );
	*/
					}
					//----------------------------------------------------------------------
					//==========================================================================
					//----------------------------------------------------------------------
					if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 7 from FEM%cWHFMFAIL 7\n" , 9 );
						return 0;
					}
					//----------------------------------------------------------------------
					//----------------------------------------------------------------------
					//###################################################################################
					// Disapear Check (2001.11.28) // 2002.06.07
					//###################################################################################
					if ( _SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
						if ( FM_TSlot > 0 ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
								FM_OSlot = 0;
								FM_TSlot = 0;
							}
						}
						if ( FM_TSlot2 > 0 ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
								FM_OSlot2 = 0;
								FM_TSlot2 = 0;
							}
						}
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//----------------------------------------------
						return 1;
						//----------------------------------------------
					}
					//###################################################################################
					//=============================================================================================
					PlaceCheck = TRUE; // 2004.09.09
					//=============================================================================================
				}
			}
		}
	}
	//====================================================================================
	// 2004.09.09
	//====================================================================================
	if ( !PlaceCheck ) {
		FM_TSlot = 0;
		FM_TSlot2 = 0;
		//
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
				FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , FM_Pointer ) == PATHDO_WAFER_RETURN ) {
					FM_TSlot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
					PlaceCheck = TRUE;
				}
			}
		}
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) {
				FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
				if ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) {
					FM_TSlot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
					PlaceCheck = TRUE;
				}
			}
		}
	}
	//====================================================================================
	if ( PlaceCheck ) { // 2004.09.09
		//==========================================================================
		//----------------------------------------------------------------------
		//
		// Code for Place to CM
		// FM_Slot
		//----
		//----------------------------------------------------------------------
		//=======================================================================
		// Move Down 2005.03.19
		//=======================================================================
//			if ( FM_TSlot > 0 ) { // 2004.09.09
//				_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE );
//			}
//			if ( FM_TSlot2 > 0 ) { // 2004.09.09
//				_SCH_MODULE_Inc_FM_InCount( CHECKING_SIDE );
//			}
		//=======================================================================
		//----------------------------------------------------------------------
		//
		// Code for Place to CM
		// FM_Slot
		//----
		//----------------------------------------------------------------------
//		FM_Side = CHECKING_SIDE; // 2007.07.11 // 2011.07.21
//		FM_Side2 = CHECKING_SIDE; // 2007.07.11 // 2011.07.21

		FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2011.07.21
		FM_Side2    = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2011.07.21
		FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION ); // 2011.07.21
		FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION ); // 2011.07.21
		FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION ); // 2011.07.21
		FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION ); // 2011.07.21
		//
		Result = _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , TRUE , TRUE , 0 ); // 2007.07.11
		//
		if      ( Result == -1 ) { // Abort
			return 0;
		}
		else if ( Result != 0 ) { // disappear
			return 1;
		}
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 13 END_PLACE_to_CM = TSlot(%d,%d) OutCass(%d,%d) OutSlot(%d,%d)\n" , FM_TSlot , FM_TSlot2 , FM_CO1 , FM_CO2 , FM_OSlot , FM_OSlot2 );
		//----------------------------------------------------------------------
		if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 97 FINISH_CHECK_2 = TRUE\n" );
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 5 for FEM Module%cWHFMSUCCESS 5\n" , 9 );
			return SYS_SUCCESS;
		}
		//----------------------------------------------
		return 1;
		//----------------------------------------------
	}
	//=======================================================================================================================================================================
	//=======================================================================================================================================================================
	//=======================================================================================================================================================================
	//=======================================================================================================================================================================
	//=======================================================================================================================================================================
	if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() && ( Get_Prm_MODULE_BUFFER_SINGLE_MODE() == 0 ) && ( SCHEDULER_Get_FM_OUT_FIRST( CHECKING_SIDE ) == 0 ) && !_SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) {
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 14 CHECK_MOVERECV_PART\n" );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 8 from FEM%cWHFMFAIL 8\n" , 9 );
			return 0;
		}
		//----------------------------------------------------------------------
		FM_Buffer = -1;
		RemainTime = 999999;
		for ( k = BM1 ; k < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ; k++ ) {
			if ( SCHEDULING_ThisIs_BM_OtherChamber( k , 0 ) ) continue; // 2009.01.21
			if ( _SCH_MODULE_GROUP_FM_POSSIBLE( k , G_PICK , 0 ) ) {
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) > 0 ) {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_SIDE( k , CHECKING_SIDE , &FM_TSlot , &FM_TSlot2 , 0 , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PICK , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ) > 0 ) {
							if ( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k ) ) {
								RemainTime = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , k );
								FM_Buffer = k;
								FM_Slot = FM_TSlot;
								FM_Slot2 = FM_TSlot2;
							}
						}
					}
					else {
						if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
							if ( RemainTime > 999990 ) {
								RemainTime = 999990;
								FM_Buffer = k;
								if ( Scheduling_TM_Double( FALSE ) ) {
									RunBuffer = 999999;
									FM_TSX = 1;
									for ( l = 0 ; l < MAX_PM_CHAMBER_DEPTH ; l++ ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , l+PM1 , G_PLACE ) ) {
//											if ( _SCH_MODULE_GET_USE_ENABLE( l+PM1 , FALSE , -1 ) && ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , 0 ) != 0 ) ) { // 2014.01.10
											if ( _SCH_MODULE_GET_USE_ENABLE( l+PM1 , FALSE , -1 ) && ( SCH_PM_HAS_PICKING( l+PM1 , &pmslot ) ) ) { // 2014.01.10
												//
												/*
												// 2014.01.10
												if (
													( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , 0 ) / 100 ) > 0 ) &&
													( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , 0 ) % 100 ) > 0 )
													) {
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , 0 ) ) { FM_TSX = 0; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , 0 ); }
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , 1 ) ) { FM_TSX = 0; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , 1 ); }
												}
												else if ( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , 0 ) / 100 ) > 0 ) {
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , 0 ) ) { FM_TSX = 2; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , 0 ); }
												}
												else if ( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , 0 ) % 100 ) > 0 ) {
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , 0 ) ) { FM_TSX = 1; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , 0 ); }
												}
												*/
												//
												// 2014.01.10
												if (
													( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , pmslot ) / 100 ) > 0 ) &&
													( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , pmslot ) % 100 ) > 0 )
													) {
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot ) ) { FM_TSX = 0; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot ); }
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot+1 ) ) { FM_TSX = 0; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot+1 ); }
												}
												else if ( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , pmslot ) / 100 ) > 0 ) {
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot ) ) { FM_TSX = 2; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot ); }
												}
												else if ( ( _SCH_MODULE_Get_PM_WAFER( l+PM1 , pmslot ) % 100 ) > 0 ) {
													if ( RunBuffer > _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot ) ) { FM_TSX = 1; RunBuffer = _SCH_MODULE_Get_PM_POINTER( l+PM1 , pmslot ); }
												}
												//
											}
										}
									}
									for ( l = TA_STATION ; l <= TB_STATION ; l++ ) {
										if ( _SCH_ROBOT_GET_FINGER_HW_USABLE( 0,l ) ) {
											if (
												( ( _SCH_MODULE_Get_TM_WAFER(0,l) / 100 ) > 0 ) &&
												( ( _SCH_MODULE_Get_TM_WAFER(0,l) % 100 ) > 0 )
												) {
												if ( RunBuffer > _SCH_MODULE_Get_TM_POINTER(0,l) ) { FM_TSX = 0; RunBuffer = _SCH_MODULE_Get_TM_POINTER(0,l); }
												if ( RunBuffer > _SCH_MODULE_Get_TM_POINTER2(0,l) ) { FM_TSX = 0; RunBuffer = _SCH_MODULE_Get_TM_POINTER2(0,l); }
											}
											else if ( ( _SCH_MODULE_Get_TM_WAFER(0,l) / 100 ) > 0 ) {
												if ( RunBuffer > _SCH_MODULE_Get_TM_POINTER(0,l) ) { FM_TSX = 2; RunBuffer = _SCH_MODULE_Get_TM_POINTER(0,l); }
											}
											else if ( ( _SCH_MODULE_Get_TM_WAFER(0,l) % 100 ) > 0 ) {
												if ( RunBuffer > _SCH_MODULE_Get_TM_POINTER(0,l) ) { FM_TSX = 1; RunBuffer = _SCH_MODULE_Get_TM_POINTER(0,l); }
											}
										}
									}
									if ( FM_TSX == 0 ) {
										FM_Slot = 1;
										FM_Slot2 = 2;
									}
									else if ( FM_TSX == 2 ) {
										FM_Slot = 0;
										FM_Slot2 = 2;
									}
									else {
										FM_Slot = 1;
										FM_Slot2 = 0;
									}
								}
								else {
									FM_Slot = 1;
									FM_Slot2 = 0;
								}
							}
						}
					}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 15 CHECK_MOVERECV = Slot(%d,%d) Buffer(%d)\n" , FM_Slot , FM_Slot2 , FM_Buffer );
		//----------------------------------------------------------------------
		if ( FM_Buffer > 0 ) {
			SCHEDULER_Set_FM_OUT_FIRST( CHECKING_SIDE , 1 );
			//
			if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.02.21
				if ( FM_Slot <= 0 ) FM_Slot = FM_Slot2;
				else                FM_Slot2 = 0;
			}
			else if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.02.21
				if ( FM_Slot2 <= 0 ) FM_Slot2 = FM_Slot;
				else                 FM_Slot = 0;
			}
			//
			if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 0 , FM_Buffer , FM_Slot , FM_Slot2 ) == SYS_ABORTED ) {
				return 0;
			}
		}
	}
	_SCH_MODULE_Set_FM_MidCount( 0 );


	return 1;
}
//=================================================================================================================================




