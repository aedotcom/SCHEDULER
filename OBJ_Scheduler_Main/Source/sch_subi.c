#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "Dll_Interface.h"
#include "IOlog.h"
#include "IO_Part_data.h"
#include "Robot_Handling.h"
#include "FA_Handling.h"
#include "sch_prepost.h"
#include "sch_CassetteResult.h"
#include "sch_CassetteSupply.h"
#include "sch_prm.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_preprcs.h"
#include "sch_sub.h"
#include "sch_multi_alic.h"
#include "sch_FMArm_multi.h"
#include "sch_sdm.h"
#include "sch_util.h"
#include "sch_Cassmap.h"
#include "sch_OneSelect.h"
#include "sch_PrePost.h"
#include "sch_processmonitor.h"
#include "sch_CommonUser.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "Multireg.h"
#include "Multijob.h"
#include "Errorcode.h"
#include "Utility.h"

#include "INF_sch_macro.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
// 2016.12.09
int _SCH_Get_SIM_TIME1( int side , int ch ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return _i_SCH_PRM_GET_SIM_TIME1( ch );
	return _i_SCH_PRM_GET_inside_SIM_TIME1( side , ch );
}
int _SCH_Get_SIM_TIME2( int side , int ch ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return _i_SCH_PRM_GET_SIM_TIME2( ch );
	return _i_SCH_PRM_GET_inside_SIM_TIME2( side , ch );
}
int _SCH_Get_SIM_TIME3( int side , int ch ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return _i_SCH_PRM_GET_SIM_TIME3( ch );
	return _i_SCH_PRM_GET_inside_SIM_TIME3( side , ch );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SUB_Check_ClusterIndex_CPJob_Same( int side , int SchPointer , int side2 , int SchPointer2 ) {
	if ( _i_SCH_CLUSTER_Get_ClusterIndex( side , SchPointer ) != _i_SCH_CLUSTER_Get_ClusterIndex( side2 , SchPointer2 ) ) return FALSE;
//	if ( PROCESS_MONITOR_SameSide_and_CPName_is_Different( side , SchPointer , side2 , SchPointer2 ) ) return FALSE; // 2007.05.28 // 2014.02.03
	if ( PROCESS_MONITOR_CPName_is_Different( side , SchPointer , side2 , SchPointer2 ) ) return FALSE; // 2007.05.28 // 2014.02.03
	return TRUE;
}
//=======================================================================================

BOOL _i_SCH_SUB_Check_Last_And_Current_CPName_is_Different( int side , int SchPointer , int ch ) {
	return PROCESS_MONITOR_Last_And_Current_CPName_is_Different( side , SchPointer , ch );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Check Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULING_ins_CHECK_PLACE_CM_for_ERROROUT_BM_FREE( int ch , int *s1 , int orderoption ) {
	int i , k = 0;
	*s1 = 0;
	if ( orderoption == 2 ) {
		for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _i_SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				*s1 = i;
				k++;
			}
		}
	}
	else {
		for ( i = _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _i_SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				*s1 = i;
				k++;
			}
		}
	}
	return k;
}


int SCHEDULER_ins_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK_for_ERROROUT( int ch , int s1 ) {
	if ( s1 <= 0 ) return 0;
	if ( s1 > _i_SCH_PRM_GET_MODULE_SIZE( ch ) ) return 0;
	if ( _i_SCH_MODULE_Get_BM_WAFER( ch , s1 ) <= 0 ) {
		if ( !_i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , s1 ) ) { // 2018.04.03
			return 1;
		}
	}
	//
	return 0;
}

int  SCHEDULER_ins_CONTROL_Check_BM_ORDER_CONTROL_for_ERROROUT( int whatm , int option ) {
	if ( option == 0 ) return 0;
	if      ( whatm == CHECKORDER_FOR_FM_PICK ) { // fmside
		if ( ( option == 2 ) || ( option == 4 ) || ( option == 6 ) || ( option == 8 ) ) return 2;
	}
	else if ( whatm == CHECKORDER_FOR_FM_PLACE ) { // fmside
		if ( ( option == 2 ) || ( option == 4 ) || ( option == 6 ) || ( option == 8 ) ) return 2;
	}
	else if ( whatm == CHECKORDER_FOR_TM_PICK ) { // fmside
		if ( ( option == 2 ) || ( option == 3 ) || ( option == 6 ) || ( option == 7 ) ) return 2;
	}
	else if ( whatm == CHECKORDER_FOR_TM_PLACE ) { // fmside
		if ( ( option == 2 ) || ( option == 3 ) || ( option == 6 ) || ( option == 7 ) ) return 2;
	}
	else {
		return 0;
	}
	return 1;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SUB_PLACE_CM_for_ERROROUT( int CHECKING_SIDE1 , int Pointer , int *ch1 , int *sl1 , int CHECKING_SIDE2 , int Pointer2 , int *ch2 , int *sl2 ) {
	int FM_CM , FM_BM , FM_WF , s1 , fail;
	if ( *ch1 > 0 ) {
		FM_CM = _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE1 , Pointer );
		FM_WF = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE1 , Pointer );
		FM_BM = _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE( FM_CM );
		if ( FM_BM > 0 ) {
			fail = 0;
			switch( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( FM_CM ) ) {
			case 1 :
				if ( fail == 0 ) {
//					if ( _i_SCH_CASSETTE_LAST_RESULT_GET( FM_CM , FM_WF ) == 1 ) { // 2011.04.18
					if ( _i_SCH_CASSETTE_LAST_RESULT2_GET( CHECKING_SIDE1 , Pointer ) == 1 ) { // 2011.04.18
						fail = 1;
					}
				}
				break;
			case 2 :
				if ( fail == 0 ) {
					fail = _i_SCH_CLUSTER_Get_FailOut( CHECKING_SIDE1 , Pointer );
				}
				break;
			default :
				if ( fail == 0 ) {
//					if ( _i_SCH_CASSETTE_LAST_RESULT_GET( FM_CM , FM_WF ) == 1 ) { // 2011.04.18
					if ( _i_SCH_CASSETTE_LAST_RESULT2_GET( CHECKING_SIDE1 , Pointer ) == 1 ) { // 2011.04.18
						fail = 1;
					}
				}
				if ( fail == 0 ) {
					fail = _i_SCH_CLUSTER_Get_FailOut( CHECKING_SIDE1 , Pointer );
				}
				break;
			}
			if ( fail == 1 ) {
//				SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( CHECKING_SIDE1 , FM_WF , 1 ); // 2002.07.21 // 2011.12.14
				SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( CHECKING_SIDE1 , Pointer , FALSE , 1 ); // 2002.07.21 // 2011.12.14
				//
				switch( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( FM_CM ) ) {
				case 0 : // Not Use
					break;
				case 1 : // BM_DIRECT.ERROR
					if ( SCHEDULER_ins_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK_for_ERROROUT( FM_BM , *sl1 ) > 0 ) {
						*ch1 = FM_BM;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE1 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_WF );
					}
					else {
						return FALSE;
					}
					break;
				case 2 : // BM_DIRECT.CM
					if ( SCHEDULER_ins_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK_for_ERROROUT( FM_BM , *sl1 ) > 0 ) {
						*ch1 = FM_BM;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE1 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_WF );
					}
					break;
				case 3 : // BM_OPT.ERROR
					if ( SCHEDULING_ins_CHECK_PLACE_CM_for_ERROROUT_BM_FREE( FM_BM , &s1 , SCHEDULER_ins_CONTROL_Check_BM_ORDER_CONTROL_for_ERROROUT( CHECKORDER_FOR_FM_PLACE , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_BM ) ) ) > 0 ) {
						*ch1 = FM_BM;
						*sl1 = s1;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE1 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_WF );
					}
					else {
						return FALSE;
					}
					break;
				case 4 : // BM_OPT.CM
					if ( SCHEDULING_ins_CHECK_PLACE_CM_for_ERROROUT_BM_FREE( FM_BM , &s1 , SCHEDULER_ins_CONTROL_Check_BM_ORDER_CONTROL_for_ERROROUT( CHECKORDER_FOR_FM_PLACE , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_BM ) ) ) > 0 ) {
						*ch1 = FM_BM;
						*sl1 = s1;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE1 , Pointer , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE1 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl1 , FM_WF , 9 , FM_WF );
					}
					break;
				}
			}
		}
	}
	if ( *ch2 > 0 ) {
		FM_CM = _i_SCH_CLUSTER_Get_PathIn( CHECKING_SIDE2 , Pointer2 );
		FM_WF = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE2 , Pointer2 );
		FM_BM = _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE( FM_CM );
		if ( FM_BM > 0 ) {
			fail = 0;
			switch( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( FM_CM ) ) {
			case 1 :
				if ( fail == 0 ) {
//					if ( _i_SCH_CASSETTE_LAST_RESULT_GET( FM_CM , FM_WF ) == 1 ) { // 2011.04.18
					if ( _i_SCH_CASSETTE_LAST_RESULT2_GET( CHECKING_SIDE2 , Pointer2 ) == 1 ) { // 2011.04.18
						fail = 1;
					}
				}
				break;
			case 2 :
				if ( fail == 0 ) {
					fail = _i_SCH_CLUSTER_Get_FailOut( CHECKING_SIDE2 , Pointer2 );
				}
				break;
			default :
				if ( fail == 0 ) {
//					if ( _i_SCH_CASSETTE_LAST_RESULT_GET( FM_CM , FM_WF ) == 1 ) { // 2011.04.18
					if ( _i_SCH_CASSETTE_LAST_RESULT2_GET( CHECKING_SIDE2 , Pointer2 ) == 1 ) { // 2011.04.18
						fail = 1;
					}
				}
				if ( fail == 0 ) {
					fail = _i_SCH_CLUSTER_Get_FailOut( CHECKING_SIDE2 , Pointer2 );
				}
				break;
			}
			if ( fail == 1 ) {
//				SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( CHECKING_SIDE2 , FM_WF , 1 ); // 2002.07.21 // 2011.12.14
				SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( CHECKING_SIDE2 , Pointer2 , FALSE , 1 ); // 2002.07.21 // 2011.12.14
				//
				switch( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( FM_CM ) ) {
				case 0 : // Not Use
					break;
				case 1 : // BM_DIRECT.ERROR
					if ( SCHEDULER_ins_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK_for_ERROROUT( FM_BM , *sl2 ) > 0 ) {
						*ch2 = FM_BM;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE2 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_WF );
					}
					else {
						return FALSE;
					}
					break;
				case 2 : // BM_DIRECT.CM
					if ( SCHEDULER_ins_CONTROL_Chk_BM_HAS_OUT_FREE_CHECK_for_ERROROUT( FM_BM , *sl2 ) > 0 ) {
						*ch2 = FM_BM;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE2 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_WF );
					}
					break;
				case 3 : // BM_OPT.ERROR
					if ( SCHEDULING_ins_CHECK_PLACE_CM_for_ERROROUT_BM_FREE( FM_BM , &s1 , SCHEDULER_ins_CONTROL_Check_BM_ORDER_CONTROL_for_ERROROUT( CHECKORDER_FOR_FM_PLACE , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_BM ) ) ) > 0 ) {
						*ch2 = FM_BM;
						*sl2 = s1;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE2 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_WF );
					}
					else {
						return FALSE;
					}
					break;
				case 4 : // BM_OPT.CM
					if ( SCHEDULING_ins_CHECK_PLACE_CM_for_ERROROUT_BM_FREE( FM_BM , &s1 , SCHEDULER_ins_CONTROL_Check_BM_ORDER_CONTROL_for_ERROROUT( CHECKORDER_FOR_FM_PLACE , _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( FM_BM ) ) ) > 0 ) {
						*ch2 = FM_BM;
						*sl2 = s1;
//						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -1 ); // 2008.04.23
						_i_SCH_CLUSTER_Set_PathRange( CHECKING_SIDE2 , Pointer2 , -6 ); // 2008.04.23
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE2 , "FM Resetting Error Out BM%d:%d:%d%cERROROUT BM%d:%d:%d%c%d\n" , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_BM - BM1 + 1 , *sl2 , FM_WF , 9 , FM_WF );
					}
					break;
				}
			}
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL NOWAY_CONTROL = FALSE; // 2015.09.17

BOOL _i_SCH_SUB_FM_Current_No_Way_Supply( int side , int pt , int pmc ) { // 2008.07.28
	int j , m , p , pr;
	//
	if ( NOWAY_CONTROL ) return TRUE; // 2015.09.17
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2008.09.30
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return TRUE; // 2008.09.30
	//
	//=================================================================================
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) return FALSE; // 2011.07.07 // 2011.07.15
	//=================================================================================
	//
	pr = _i_SCH_CLUSTER_Get_PathRange( side , pt ); // 2010.12.08
	//
	if ( pr < 0 ) return TRUE;
	if ( pr == 0 ) return FALSE; // 2009.10.19
	//
	// 2015.10.12
	//
	if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( side , pt ) != 0 ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) < PM1 ) {
			//
			switch ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) ) ) {
			case CWM_PRESENT :
			case CWM_PROCESS :
			case CWM_FAILURE :
				break;
			default :
				return TRUE;
				break;
			}
			//
		}
		else if ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) >= BM1 ) {
			//
			if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , pt ) , _i_SCH_CLUSTER_Get_SlotIn( side , pt ) ) <= 0 ) {
				return TRUE;
			}
			//
		}
		//
	}
	//
//	if ( _i_SCH_CLUSTER_Get_PathStatus( side , pt ) != SCHEDULER_READY ) return TRUE; // 2014.11.07
	//
	if ( pmc != -2 ) pr = 1; // 2010.12.01
	//
	for ( p = 0 ; p < pr ; p++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
//			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , j ); // 2010.12.08
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , p , j ); // 2010.12.08
			if ( m > 0 ) {
	//			if ( ( pmc == -1 ) || ( pmc == m ) ) { // 2010.12.08
				if ( ( pmc < 0 ) || ( pmc == m ) ) { // 2010.12.08
					if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
//						return FALSE; // 2010.12.08
						break; // 2010.12.08
					}
				}
			}
		}
		//
		if ( j == MAX_PM_CHAMBER_DEPTH ) return TRUE; // 2010.12.08
		//
	}
//	return TRUE; // 2010.12.08
	return FALSE; // 2010.12.08
}
//=======================================================================================
BOOL _i_SCH_SUB_Run_Impossible_Condition( int side , int pt , int PlaceChamber ) { // 2009.04.29
	int i;
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2008.09.30
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return TRUE; // 2008.09.30
	//
	if ( ( PlaceChamber == -1 ) || ( PlaceChamber == -99 ) ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , pt ) < 0 ) return TRUE;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , pt ) == SCHEDULER_CM_END ) return TRUE;
		//
		if ( PlaceChamber == -99 ) { // 2009.04.29
			if ( _i_SCH_CLUSTER_Get_PathStatus( side , pt ) == SCHEDULER_SUPPLY ) {
				if ( ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) >= PM1 ) && ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) < AL ) ) return TRUE;
				return FALSE;
			}
		}
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , pt ) != SCHEDULER_READY ) {
			if ( ( _i_SCH_CLUSTER_Get_PathRun( side , pt , 13 , 2 ) >= 1 ) && ( _i_SCH_CLUSTER_Get_PathRun( side , pt , 13 , 2 ) <= 8 ) ) return FALSE; // 2009.02.13
			return TRUE;
		}
		else { // 2012.04.13
			//
			if ( _i_SCH_CLUSTER_Get_Ex_DisableSkip( side , pt ) ) {
				//
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , i ) > 0 ) break;
					//
				}
				//
				if ( i == MAX_PM_CHAMBER_DEPTH ) return TRUE;
				//
			}
			//
		}
		//
	}
	//
	if ( ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) >= PM1 ) && ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) < AL ) ) return TRUE;
	//
	if ( PlaceChamber >= 0 ) {
		if ( !_i_SCH_MODULE_Get_Use_Interlock_Run( side , PlaceChamber ) ) return TRUE;
	}
	return FALSE;
}
//=======================================================================================
BOOL _i_SCH_SUB_Simple_Supply_Possible_Sub( int side , BOOL usermodecheck ) { // 2006.11.30
	int i , fpt , Res;
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE; // 2008.09.30
	if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) return FALSE;
	//=================================================================================
	if ( _i_SCH_SYSTEM_MODE_END_GET( side ) != 0 ) return FALSE;
	//
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.28
		if ( _i_SCH_MODULE_Chk_FM_Finish_Status_Sub( side ) ) return FALSE;
	}
	else {
		if ( _i_SCH_MODULE_Chk_TM_Finish_Status( side ) ) return FALSE;
	}
	//
	// 2011.12.29
	//
	fpt = -1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue;
		//
		fpt = i;
		//
		break;
	}
	//
	if ( fpt != -1 ) {
		//
		Res = _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , fpt );
		//
		if ( usermodecheck ) {
			if ( ( Res % 10 ) == 0 ) return FALSE;
		}
		else {
			if ( ( Res != 0 ) && ( ( Res % 10 ) != 0 ) ) return FALSE;
		}
	}
	//
	return TRUE;
}
//=======================================================================================
BOOL _i_SCH_SUB_Simple_Supply_Possible_FDHC( int side ) { // 2006.11.30
	BOOL Res;
	//
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//
	Res = _i_SCH_SUB_Simple_Supply_Possible_Sub( side , FALSE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Res;
}
//=======================================================================================
//
void Scheduler_HSide_Remap( int side , int pt , int ); // 2018.11.22
//
BOOL _i_SCH_SUB_Remain_for_CM( int side , BOOL log ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE; // 2008.09.30
	if ( _i_SCH_MODULE_Get_TM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
	if ( log ) return TRUE;
	//
	while ( _i_SCH_SUB_Run_Impossible_Condition( side , _i_SCH_MODULE_Get_TM_DoPointer( side ) , _i_SCH_PRM_GET_MODULE_MODE( FM ) ? -99 : -1 ) ) {
		_i_SCH_MODULE_Inc_TM_DoPointer( side );
		if ( _i_SCH_MODULE_Get_TM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	}
	if ( _i_SCH_MODULE_Get_TM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
	Scheduler_HSide_Remap( side , _i_SCH_MODULE_Get_TM_DoPointer( side ) , -1 ); // 2018.11.22
	//
	return TRUE;
}
//=======================================================================================
/*
//
// 2017.07.20
//
BOOL _i_SCH_SUB_Remain_for_FM_Sub( int side ) {
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) return _i_SCH_SUB_Remain_for_CM( side ); // 2011.07.28
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE; // 2008.09.30
	//
	// 2011.10.25
/ *
	if ( _i_SCH_MODULE_Get_FM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	while ( _i_SCH_SUB_Run_Impossible_Condition( side , _i_SCH_MODULE_Get_FM_DoPointer( side ) , -1 ) ) {
		_i_SCH_MODULE_Inc_FM_DoPointer( side );
		if ( _i_SCH_MODULE_Get_FM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	}
	if ( _i_SCH_MODULE_Get_FM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
* /
	if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
	while ( _i_SCH_SUB_Run_Impossible_Condition( side , _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) , -1 ) ) {
		_i_SCH_MODULE_Inc_FM_DoPointer_Sub( side );
		if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	}
	if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
	return TRUE;

}
//
*/
//
//
// 2017.07.20
//
BOOL _i_SCH_SUB_Remain_for_FM_Sub( int side , BOOL log ) {
	int i , j;
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) return _i_SCH_SUB_Remain_for_CM( side , log ); // 2011.07.28
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE; // 2008.09.30
	//
	if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	//
	if ( log ) return TRUE;
	//
	if ( _i_SCH_PRM_GET_OPTIMIZE_MODE() == 1 ) {
		//
		for ( i = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_SUB_Run_Impossible_Condition( side , i , -1 ) ) continue;
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				//
				if ( 0 < _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , 0 , j ) ) {
					_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , i );
					//
					Scheduler_HSide_Remap( side , i , -1 ); // 2018.11.22
					//
					return TRUE;
				}
				//
			}
			//
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_SUB_Run_Impossible_Condition( side , i , -1 ) ) continue;
			//
			_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , i );
			//
			Scheduler_HSide_Remap( side , i , -1 ); // 2018.11.22
			//
			return TRUE;
			//
		}
		//
		_i_SCH_MODULE_Set_FM_DoPointer_Sub( side , MAX_CASSETTE_SLOT_SIZE );
		return FALSE;
		//
	}
	else {
		//
		while ( _i_SCH_SUB_Run_Impossible_Condition( side , _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) , -1 ) ) {
			_i_SCH_MODULE_Inc_FM_DoPointer_Sub( side );
			if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
		}
		if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
		//
		Scheduler_HSide_Remap( side , _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) , -1 ); // 2018.11.22
		//
	}
	//
	return TRUE;

}
//

//
BOOL _i_SCH_SUB_Remain_for_FM_FDHC( int side ) {
	BOOL Res;
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub();
	//
	Res = _i_SCH_SUB_Remain_for_FM_Sub( side , FALSE );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub();
	return Res;
}
//=======================================================================================
int _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( int side , int *cm , int *pt , int subchk , BOOL );
//
BOOL _i_SCH_SUB_FM_Current_No_More_Supply_FDHC( int side , int *pt , int pmc , int *retcm , int side2 , int pt2_or_clidx ) {
//	int i;
	int Res;
	//=================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return TRUE; // 2008.09.30
	//=================================================================================
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//=================================================================================
	if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM_SUB( side , retcm , pt , -1 , FALSE ) <= 0 ) {
		//
		_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
		//
		return TRUE;
	}
	//=================================================================================
	if ( side2 != -1 ) {
		if ( ( side2 < 0 ) || ( side2 >= MAX_CASSETTE_SIDE ) ) {
			//
			_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
			//
			return TRUE; // 2008.09.30
		}
		if ( ( pt2_or_clidx < 0 ) || ( pt2_or_clidx >= MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
			//
			return TRUE; // 2008.09.30
		}
		if ( !_i_SCH_SUB_Check_ClusterIndex_CPJob_Same( side , *pt , side2 , pt2_or_clidx ) ) {
			//
			_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
			//
			return TRUE; // 2007.05.28
		}
	}
	else {
		if ( pt2_or_clidx != -1 ) {
			if ( _i_SCH_CLUSTER_Get_ClusterIndex( side , *pt ) != pt2_or_clidx ) {
				//
				_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
				//
				return TRUE;
			}
		}
	}
	//
	Res = _i_SCH_SUB_FM_Current_No_Way_Supply( side , *pt , pmc );
	//
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//
	return Res;
	//
}
//=======================================================================================
BOOL _i_SCH_SUB_FM_Another_No_More_Supply( int side , int pmc , int side2 , int pt2_or_clidx ) { // 2006.11.27
	int i , dum_pt , dum_rcm;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
//			if ( pmc == -2 ) { // 2014.01.24
//				if ( _i_SCH_SUB_Simple_Supply_Possible_FDHC( i ) ) return FALSE; // 2014.01.24
//			} // 2014.01.24
//			else { // 2014.01.24
				if ( !_i_SCH_SUB_FM_Current_No_More_Supply_FDHC( i , &dum_pt , pmc , &dum_rcm , side2 , pt2_or_clidx ) ) return FALSE;
//			} // 2014.01.24
		}
	}
	return TRUE;
}

//=======================================================================================
int SCHEDULING_CHECK_Increase_for_FM( int side , int trgslot , int *pointer ) {
	int s; // 2003.12.01
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 0; // 2008.09.30
	//
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.28

	// 2011.10.25
/*
		if ( _i_SCH_MODULE_Get_FM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return 0;
		//
		*pointer = _i_SCH_MODULE_Get_FM_DoPointer( side );
		s = _i_SCH_CLUSTER_Get_SlotIn( side , *pointer ); // 2003.12.01
		if ( s != trgslot ) return 3; // 2003.12.01
		//
*/
		//
		if ( _i_SCH_MODULE_Get_FM_DoPointer_Sub( side ) >= MAX_CASSETTE_SLOT_SIZE ) return 0;
		//
		*pointer = _i_SCH_MODULE_Get_FM_DoPointer_Sub( side );
		s = _i_SCH_CLUSTER_Get_SlotIn( side , *pointer ); // 2003.12.01
		if ( s != trgslot ) return 3; // 2003.12.01
		//
		_i_SCH_MODULE_Inc_FM_DoPointer_Sub( side );
		//
		if ( !_i_SCH_SUB_Remain_for_FM_Sub( side , FALSE ) ) return 2; // 2006.11.28
	}
	else {
		if ( _i_SCH_MODULE_Get_TM_DoPointer( side ) >= MAX_CASSETTE_SLOT_SIZE ) return 0;
		//
		*pointer = _i_SCH_MODULE_Get_TM_DoPointer( side );
		s = _i_SCH_CLUSTER_Get_SlotIn( side , *pointer ); // 2003.12.01
		if ( s != trgslot ) return 3; // 2003.12.01
		//
		_i_SCH_MODULE_Inc_TM_DoPointer( side );

		if ( !_i_SCH_SUB_Remain_for_CM( side , FALSE ) ) return 2; // 2006.11.28
	}
	return 1;
}
//=======================================================================================
int _i_SCH_SUB_GET_OUT_CM_AND_SLOT_SUB( int FM_Side , int FM_Pointer , int FM_Slot , int *FM_CO , int *FM_OSlot , BOOL outchek , BOOL *fail ) {
	//
	BOOL return_impossible = FALSE;
	int Res;
	//
	*fail = FALSE;
	//
	if ( ( FM_Side < 0 ) || ( FM_Side >= MAX_CASSETTE_SIDE ) ) {
		*FM_CO = 0;
		*FM_OSlot = 0;
		return 0; // 2008.09.30
	}
	//
	if ( FM_Pointer >= MAX_CASSETTE_SLOT_SIZE ) {
		if ( FM_Slot == -1 ) {
			*FM_OSlot = ( FM_Pointer % 100 ) + 1;
		}
		else {
			*FM_OSlot = FM_Slot;
		}
		*FM_CO = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
		return 0;
	}
	//
	*FM_OSlot = _i_SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer );
	*FM_CO = _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer ); // 2013.04.02
	//
	if ( ( *FM_CO >= BM1 ) && ( *FM_CO < TM ) ) return 0; // 2013.04.02
	//
	if ( outchek ) {
		//
		Res = SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( FM_Side , FM_Pointer , FALSE , &return_impossible , TRUE ); // 2011.06.09
		//
		if ( Res < -100 ) { // 2011.06.09
			*FM_CO = 0;
			*FM_OSlot = 0;
			return -1;
		}
		if ( Res < 0 ) { // 2011.04.14
			*FM_CO = 0;
			*FM_OSlot = 0;
			*fail = TRUE;
			return 0;
		}
	}
	//
	if ( return_impossible ) { // 2011.04.14
		*FM_OSlot = _i_SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer );
		*FM_CO = _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
		return 0;
	}
	else {
		if ( ( _i_SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN ) || ( _i_SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == 0 ) ) {
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 0 ) { // 2016.07.28
				*FM_OSlot = _i_SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer );
				*FM_CO = _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
				return 2;
			}
			else {
				*FM_OSlot = _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer );
				*FM_CO = _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
				return 1;
			}
		}
		else {
			*FM_OSlot = _i_SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer );
			*FM_CO = _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
			return 0;
		}
	}
}

BOOL _i_SCH_SUB_GET_OUT_CM_AND_SLOT( int FM_Side , int FM_Pointer , int FM_Slot , int *FM_CO , int *FM_OSlot ) {
	BOOL fail;
	if ( _i_SCH_SUB_GET_OUT_CM_AND_SLOT_SUB( FM_Side , FM_Pointer , FM_Slot , FM_CO , FM_OSlot , FALSE , &fail ) == 1 ) return TRUE;
	return FALSE;
}

int _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( int FM_Side , int FM_Pointer , int FM_Slot , int *FM_CO , int *FM_OSlot , BOOL *fail ) {
	return _i_SCH_SUB_GET_OUT_CM_AND_SLOT_SUB( FM_Side , FM_Pointer , FM_Slot , FM_CO , FM_OSlot , TRUE , fail );
}
//=======================================================================================

void _i_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( int fms , int FM_Slot , int *FM_Slot_Real , int *FM_TSlot_Real ) {
	if ( !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
			*FM_TSlot_Real = ( ( FM_Slot - 1 ) % ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 ) );
			*FM_Slot_Real = FM_Slot - (*FM_TSlot_Real);
		}
		else {
			*FM_TSlot_Real = 0;
			*FM_Slot_Real = FM_Slot;
		}
	}
	else {
		*FM_TSlot_Real = 0;
		*FM_Slot_Real = FM_Slot;
	}
}

//============================================================================================================================
int _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( int Result , int mode ) { // 2007.07.25
	//
	// mode
	// 0 : PICK
	// 1 : PLACE
	// 2 : PICK(for SWAP) PM
	// 3 : PICK(for SWAP) BM
	//
	if ( ( Result < 1 ) || ( Result > 4 ) ) return 0; // 2007.10.12
	//
	switch( _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() ) { // "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|[E]Always Use|[E]Skip(But Gate Use)|[E]Skip All|[E]Skip(GateUse).X2|[E]Skip All.X2" 
	case 0 :
	case 5 : // 2007.10.12
				return 0;
				break;
	case 1 :
	case 6 : // 2007.10.12
//				return 1; // 2009.07.10
				if ( mode == 1 )	return 3; // 2009.07.10
				else				return 1; // 2009.07.10
				break;
	case 2 :
	case 7 : // 2007.10.12
//				return 2; // 2009.07.10
				if ( mode == 1 )	return 4; // 2009.07.10
				else				return 2; // 2009.07.10
				break;
	case 3 :
	case 8 : // 2007.10.12
				if ( Result >= 2 ) return 0; // 2007.10.12
//				return 1; // 2009.07.10
				if ( mode == 1 )	return 3; // 2009.07.10
				else				return 1; // 2009.07.10
				break;
	case 4 :
	case 9 : // 2007.10.12
				if ( Result >= 2 ) return 0; // 2007.10.12
//				return 2; // 2009.07.10
				if ( mode == 1 )	return 4; // 2009.07.10
				else				return 2; // 2009.07.10
				break;

	case 10 : // 2013.01.22
//				if ( Result == 1 ) return 10; // 2013.03.15
				if ( ( mode == 2 ) && ( Result == 1 ) ) return 10; // 2013.03.15
				break;
				//

	case 11 : // 2013.10.31
				if ( ( mode == 2 ) && ( Result == 1 ) ) return 10;
				if ( ( mode == 3 ) && ( Result == 1 ) ) return 10;
				break;
				//

	case 12 : // 2018.04.06 Switch Message X
				return 0;
				break;
				//
	}
	return 0;
}
//============================================================================================================================
BOOL _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( int Finger , int Slot ) {
	if ( Slot <= 0 ) return FALSE;
	if ( ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( Finger , 0 ) == Slot ) ||
	     ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER( Finger , 1 ) == Slot ) ) return TRUE;
	return FALSE;
}



//============================================================================================================================
int  Get_RunPrm_UTIL_START_PARALLEL_PRE_ONE_POSSIBLE( int CurrSide , int checkside , int ch , BOOL batch ) { // 2008.04.29
	int i;
	//
	if ( _i_SCH_SYSTEM_PMMODE_GET( CurrSide ) != _i_SCH_SYSTEM_PMMODE_GET( checkside ) ) return TRUE; // 2009.10.15
	if ( _i_SCH_SYSTEM_MOVEMODE_GET( CurrSide ) != _i_SCH_SYSTEM_MOVEMODE_GET( checkside ) ) return TRUE; // 2013.09.03
	//
	//================================================================================
//	EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
	EnterCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
	//================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( ( i != CurrSide ) && ( i != checkside ) ) {
//			if ( ( _i_SCH_SYSTEM_USING_GET( i ) >= 6 ) && ( SYSTEM_BEGIN_GET( i ) == 1 ) ) { // 2015.07.30
			if ( ( _i_SCH_SYSTEM_USING_GET( i ) >= 6 ) && ( SYSTEM_BEGIN_GET( i ) >= 2 ) ) { // 2015.07.30
//				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2009.10.15
//				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) <= MUF_99_USE_to_DISABLE ) ) { // 2009.10.15 // 2011.04.29
//				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_81_USE_to_PREND_RANDONLY ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2011.04.29 // 2011.06.24
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) >= MUF_81_USE_to_PREND_RANDONLY ) ) { // 2011.04.29 // 2011.06.24

					//
					if ( _i_SCH_SYSTEM_PMMODE_GET( CurrSide ) != _i_SCH_SYSTEM_PMMODE_GET( i ) ) {
						//================================================================================
//						LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
						LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
						//================================================================================
						return TRUE; // 2009.10.15
					}
					//
					if ( _i_SCH_SYSTEM_MOVEMODE_GET( CurrSide ) != _i_SCH_SYSTEM_MOVEMODE_GET( i ) ) { // 2013.09.03
						//================================================================================
						LeaveCriticalSection( &CR_PRE_BEGIN_ONLY );
						//================================================================================
						return TRUE;
					}
					//
					if ( ( _i_SCH_SYSTEM_FA_GET( i ) == 1 ) && _i_SCH_MULTIREG_HAS_REGIST_DATA( i ) ) continue;
					//
					if ( batch ) { // 2009.02.12
						if ( _i_SCH_MODULE_Get_TM_DoPointer( i ) <= _i_SCH_MODULE_Get_FM_LastOutPointer( i ) ) continue;
					}
					else {
						if ( !_i_SCH_SYSTEM_LASTING_GET( i ) ) continue;
					}
					//================================================================================
//					LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
					LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
					//================================================================================
					return TRUE;
				}
			}
		}
	}
	//================================================================================
//	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
	LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
	//================================================================================
	return FALSE;
}
//

//BOOL SCHEDULER_FIRST_SUPPLY_IMPOSSIBLE( int side , int ch ) { // 2008.07.24 // 2017.07.20
BOOL SCHEDULER_FIRST_SUPPLY_IMPOSSIBLE( int side , int ch , BOOL all ) { // 2008.07.24 // 2017.07.20
	int i , j , m;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , 0 , j );
			if ( m == ch ) return FALSE;
		}
//		break; // 2017.07.20
		if ( !all ) break; // 2017.07.20
	}
	return TRUE;
}
//
BOOL _i_SCH_SUB_START_PARALLEL_SKIP_CHECK( int runside , int checkside , int ranpos , BOOL precheck ) { // 2003.02.05
	int i;
	BOOL Checked; // 2016.01.13

//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\t[runside=%d][checkside=%d][ranpos=%d][precheck=%d][opt=%d]\n" , runside , checkside , ranpos , precheck , _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() );
//------------------------------------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------
	if ( runside == checkside ) return FALSE;
	//-----------------------------------------------------------------------
	if      ( checkside == -1 ) {
		Scheduler_Randomize_After_Waiting_Part( runside , ranpos );
		return TRUE;
	}
	else if ( checkside == -2 ) { // 2011.03.27
		SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( runside , ranpos );
		return TRUE;
	}
	//-----------------------------------------------------------------------
	switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) {
	case 0 : // 1234
		break;
	case 1 : // 1 234
		if ( runside   == 0 ) return TRUE;
		if ( checkside == 0 ) return TRUE;
		break;
	case 2 : // 2 134
		if ( runside   == 1 ) return TRUE;
		if ( checkside == 1 ) return TRUE;
		break;
	case 3 : // 3 124
		if ( runside   == 2 ) return TRUE;
		if ( checkside == 2 ) return TRUE;
		break;
	case 4 : // 4 123
		if ( runside   == 3 ) return TRUE;
		if ( checkside == 3 ) return TRUE;
		break;
	case 5 : // 12 34
		if ( ( runside == 0 ) && ( ( checkside == 2 ) || ( checkside == 3 ) ) ) return TRUE;
		if ( ( runside == 1 ) && ( ( checkside == 2 ) || ( checkside == 3 ) ) ) return TRUE;
		if ( ( runside == 2 ) && ( ( checkside == 0 ) || ( checkside == 1 ) ) ) return TRUE;
		if ( ( runside == 3 ) && ( ( checkside == 0 ) || ( checkside == 1 ) ) ) return TRUE;
		break;
	case 6 : // 13 24
		if ( ( runside == 0 ) && ( ( checkside == 1 ) || ( checkside == 3 ) ) ) return TRUE;
		if ( ( runside == 1 ) && ( ( checkside == 0 ) || ( checkside == 2 ) ) ) return TRUE;
		if ( ( runside == 2 ) && ( ( checkside == 1 ) || ( checkside == 3 ) ) ) return TRUE;
		if ( ( runside == 3 ) && ( ( checkside == 0 ) || ( checkside == 2 ) ) ) return TRUE;
		break;
	case 7 : // 14 23
		if ( ( runside == 0 ) && ( ( checkside == 1 ) || ( checkside == 2 ) ) ) return TRUE;
		if ( ( runside == 1 ) && ( ( checkside == 0 ) || ( checkside == 3 ) ) ) return TRUE;
		if ( ( runside == 2 ) && ( ( checkside == 0 ) || ( checkside == 3 ) ) ) return TRUE;
		if ( ( runside == 3 ) && ( ( checkside == 1 ) || ( checkside == 2 ) ) ) return TRUE;
		break;
/*
// 2008.04.29
	case 8 : // PM
		for ( i = PM1 ; i < AL ; i++ ) {
//			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) { // 2005.09.14
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2005.09.14
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) { // 2005.09.14
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2005.09.14
					return FALSE;
				}
			}
		}
		return TRUE;
		break;
	case 9 : // PM SuccessSkip(x) // 2005.04.20
		for ( i = PM1 ; i < AL ; i++ ) {
			if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_ALLSKIP ) {
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) { // 2005.09.14
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2005.09.14
//					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) { // 2005.09.14
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2005.09.14
						return FALSE;
					}
				}
			}
		}
		return TRUE;
		break;
	case 10 : // PM - Randomize // 2006.02.28
		for ( i = PM1 ; i < AL ; i++ ) {
			//
			if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue; // 2006.10.18
			//
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) < MUF_90_USE_to_XDEC_FROM ) ) {
				if ( ranpos ) { // 2006.06.22
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) {
	//					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 1 ) return TRUE;
					}
					else {
						if ( precheck ) { // 2006.06.27
							if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) return TRUE;
						}
						else {
							//=====================================================================================
							// 2007.05.04
							//=====================================================================================
							// if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 1 ) return TRUE; // 2007.05.04
							//=====================================================================================
							if ( Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( runside , i ) <= 0 ) return TRUE;
							//=====================================================================================
						}
					}
				}
				else { // 2006.06.22
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) {
						return FALSE;
					}
				}
			}
		}
		return FALSE;
		break;
	case 11 : // PM SuccessSkip(x) - Randomize  // 2006.02.28
		for ( i = PM1 ; i < AL ; i++ ) {
			//
			if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue; // 2006.10.18
			//
			if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_ALLSKIP ) {
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2005.09.14
					if ( ranpos ) { // 2006.06.22
						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2005.09.14
	//						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 1 ) return TRUE;
						}
						else {
							if ( precheck ) { // 2006.06.27
								if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) return TRUE;
							}
							else {
								//=====================================================================================
								// 2007.05.04
								//=====================================================================================
								// if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 1 ) return TRUE; // 2007.05.04
								//=====================================================================================
								if ( Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( runside , i ) <= 0 ) return TRUE;
								//=====================================================================================
							}
						}
					}
					else { // 2006.06.22
						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) {
							return FALSE;
						}
					}
				}
			}
		}
		return FALSE;
		break;
*/

// 2008.04.29
	case 8 : // PM
	case 9 : // PM SuccessSkip(x) // 2005.04.20
	case 10 : // PM - Randomize // 2006.02.28
	case 11 : // PM SuccessSkip(x) - Randomize  // 2006.02.28
		for ( i = PM1 ; i < AL ; i++ ) {
			//---------------------------------------------------------------------------------------------------------------------------
			if ( ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() == 10 ) || ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() == 11 ) ) {
//				if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue; // 2009.10.01
				if ( !Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , _i_SCH_SYSTEM_PMMODE_GET( runside ) ) ) continue; // 2009.10.01
			}
			//---------------------------------------------------------------------------------------------------------------------------
			if ( ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ==  9 ) || ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() == 11 ) ) {
				if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) == MRES_SUCCESS_LIKE_ALLSKIP ) continue;
			}
			//---------------------------------------------------------------------------------------------------------------------------
//			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2011.04.29
//			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) == MUF_01_USE ) { // 2011.04.29 // 2011.06.24
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) == MUF_01_USE ) || ( ( ranpos == 3 ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) ) ) { // 2011.04.29 // 2011.06.24

//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 1 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------

				if ( ( ranpos != 0 ) && ( ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() == 10 ) || ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() == 11 ) ) ) {

//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 2 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------

//					if ( !SCHEDULER_FIRST_SUPPLY_IMPOSSIBLE( runside , i ) ) { // 2008.07.25 // 2017.07.20
					if ( !SCHEDULER_FIRST_SUPPLY_IMPOSSIBLE( runside , i , _i_SCH_PRM_GET_OPTIMIZE_MODE() == 1 ) ) { // 2008.07.25 // 2017.07.20

//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 3 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
						//
//						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < 90 ) ) { // 2008.07.28
//						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) ) { // 2008.07.28 // 2011.04.29
//						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2008.07.28 // 2011.04.29 // 2011.06.24
						//
//						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_97_USE_to_SEQDIS_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2008.07.28 // 2011.04.29 // 2011.06.24 // 2016.01.13
//						} // 2016.01.13
//						else { // 2016.01.13

//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 4 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
						//
						// 2016.01.13
						//
						Checked = FALSE; // 2016.01.13
						//
						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_97_USE_to_SEQDIS_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2016.01.13
							//
							if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) { // 2016.01.13
								//
								if ( !_i_SCH_SYSTEM_USING_RUNNING( checkside ) ) {
									//
									if ( !Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , _i_SCH_SYSTEM_PMMODE_GET( checkside ) ) ) { // 2016.01.13
										//
										Checked = TRUE; // 2016.01.13
										//
									}
									//
								} // 2016.01.13
								//
							}
							// 2016.01.13
						} // 2016.01.13
						else { // 2016.01.13
							//
//							Checked = TRUE; // 2016.01.13 // 2016.05.13
							//
							// 2016.05.13
							//
							if ( ranpos == 3 ) {
								//
								switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) ) {
								case MUF_02_USE_to_END :
								case MUF_03_USE_to_END_DISABLE :
								case MUF_04_USE_to_PREND_EF_LP :
								case MUF_05_USE_to_PRENDNA_EF_LP :
								case MUF_07_USE_to_PREND_DECTAG :
								case MUF_08_USE_to_PRENDNA_DECTAG :
								case MUF_71_USE_to_PREND_EF_XLP :
									break;
								default :
									Checked = TRUE;
									break;
								}
								//
							}
							else {
								Checked = TRUE;
							}
						}
						//
						if ( Checked ) { // 2016.01.13
							if ( precheck ) {
								if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) return TRUE;
								if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) { // 2008.04.29
									if ( Get_RunPrm_UTIL_START_PARALLEL_PRE_ONE_POSSIBLE( runside , checkside , i , ( ranpos == 2 ) ) ) return TRUE; // 2008.04.29
								}
							}
							else {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 5 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
								if ( Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( runside , i , ( ranpos == 2 ) , ( _i_SCH_MODULE_Get_Mdl_Use_Flag( runside , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) ) <= 0 ) return TRUE;
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 6 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
							}
						}
					}
				}
				else {
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 7 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2008.07.28
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) < MUF_90_USE_to_XDEC_FROM ) ) ) { // 2008.07.28 // 2011.04.29
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2008.07.28 // 2011.04.29 // 2011.06.24
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2008.07.28 // 2011.04.29 // 2011.06.24
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_98_USE_to_DISABLE_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_97_USE_to_SEQDIS_RAND ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( checkside , i ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2008.07.28 // 2011.04.29 // 2011.06.24
						return FALSE;
					}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRCHECK=> 8 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
				}
			}
			//---------------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		if ( ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ==  8 ) || ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ==  9 ) ) {
			return TRUE;
		}
		else {
			return FALSE;
		}
		break;
	}
	return FALSE;
}









//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern CRITICAL_SECTION CR_CLUSTERDB;
//------------------------------------------------------------------------------------------
int _i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK( int ch , int sub ) { // 2015.06.01
	int j , s1 , s2 , p1 , p2 , d1 , d2 , f;
	//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK 0 [%d][%d]\n" , ch , sub );
	//
	if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() <= 0 ) return -1;

	EnterCriticalSection( &CR_CLUSTERDB );
	//
	f = 0;
	//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK A [%d][%d]\n" , ch , sub );
	if      ( ( ch >= TM ) && ( ch < FM ) ) {
		//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK B [%d][%d]\n" , ch , sub );
		s1 = -1;
		s2 = -1;
		//
		if ( ( ( _i_SCH_MODULE_Get_TM_WAFER( ch - TM , sub ) / 100 ) > 0 ) && ( ( _i_SCH_MODULE_Get_TM_WAFER( ch - TM , sub ) % 100 ) > 0 ) ) {
			//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK C [%d][%d]\n" , ch , sub );
			s1 = _i_SCH_MODULE_Get_TM_SIDE( ch - TM , sub );
			p1 = _i_SCH_MODULE_Get_TM_POINTER( ch - TM , sub );
			//
			s2 = _i_SCH_MODULE_Get_TM_SIDE2( ch - TM , sub );
			p2 = _i_SCH_MODULE_Get_TM_POINTER2( ch - TM , sub );
			//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK D [%d][%d] [%d,%d][%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 );
			if ( s1 == s2 ) s2 = -1;
			//
		}
		//
		if ( s2 != -1 ) {
			//
			d1 = _i_SCH_CLUSTER_Get_PathDo( s1 , p1 );
			d2 = _i_SCH_CLUSTER_Get_PathDo( s2 , p2 );
			//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK E [%d][%d] [%d,%d][%d,%d] [%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 , d1 , d2 );
			if      ( ( d1 == PATHDO_WAFER_RETURN ) && ( d2 != PATHDO_WAFER_RETURN ) ) {
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK E1 [%d][%d] [%d,%d][%d,%d] [%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 , d1 , d2 );
				_i_SCH_CLUSTER_Set_PathDo( s2 , p2 , PATHDO_WAFER_RETURN );
				_i_SCH_CLUSTER_Set_PathStatus( s2 , p2 , SCHEDULER_RETURN_REQUEST );
				//
				f = 1;
				//
			}
			else if ( ( d2 == PATHDO_WAFER_RETURN ) && ( d1 != PATHDO_WAFER_RETURN ) ) {
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK E2 [%d][%d] [%d,%d][%d,%d] [%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 , d1 , d2 );
				_i_SCH_CLUSTER_Set_PathDo( s1 , p1 , PATHDO_WAFER_RETURN );
				_i_SCH_CLUSTER_Set_PathStatus( s1 , p1 , SCHEDULER_RETURN_REQUEST );
				//
				f = 1;
				//
			}
			//
		}
		//
	}
	else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK b [%d][%d]\n" , ch , sub );
		for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; j++ ) {
			//
			s1 = -1;
			s2 = -1;
			//
			if ( ( _i_SCH_MODULE_Get_BM_WAFER( ch , j ) > 0 ) && ( _i_SCH_MODULE_Get_BM_WAFER( ch , j + 1 ) > 0 ) ) {
				//
				s1 = _i_SCH_MODULE_Get_BM_SIDE( ch , j );
				p1 = _i_SCH_MODULE_Get_BM_POINTER( ch , j );
				//
				s2 = _i_SCH_MODULE_Get_BM_SIDE( ch , j + 1 );
				p2 = _i_SCH_MODULE_Get_BM_POINTER( ch , j + 1 );
				//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK d [%d][%d] [%d,%d][%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 );
				if ( s1 == s2 ) s2 = -1;
				//
			}
			//
			j++;
			//
			if ( s2 != -1 ) {
				//
				d1 = _i_SCH_CLUSTER_Get_PathDo( s1 , p1 );
				d2 = _i_SCH_CLUSTER_Get_PathDo( s2 , p2 );
				//
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK e [%d][%d] [%d,%d][%d,%d] [%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 , d1 , d2 );
				if      ( ( d1 == PATHDO_WAFER_RETURN ) && ( d2 != PATHDO_WAFER_RETURN ) ) {
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK e1 [%d][%d] [%d,%d][%d,%d] [%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 , d1 , d2 );
					_i_SCH_CLUSTER_Set_PathDo( s2 , p2 , PATHDO_WAFER_RETURN );
					_i_SCH_CLUSTER_Set_PathStatus( s2 , p2 , SCHEDULER_RETURN_REQUEST );
					//
					f = 1;
					//
				}
				else if ( ( d2 == PATHDO_WAFER_RETURN ) && ( d1 != PATHDO_WAFER_RETURN ) ) {
//_IO_CIM_PRINTF( "_i_SCH_SUB_DOUBLE_DIFFSIDE_ONESTOP_CHECK e2 [%d][%d] [%d,%d][%d,%d] [%d,%d]\n" , ch , sub , s1 , p1 , s2 , p2 , d1 , d2 );
					_i_SCH_CLUSTER_Set_PathDo( s1 , p1 , PATHDO_WAFER_RETURN );
					_i_SCH_CLUSTER_Set_PathStatus( s1 , p1 , SCHEDULER_RETURN_REQUEST );
					//
					f = 1;
					//
				}
				//
			}
			//
		}
	}

	LeaveCriticalSection( &CR_CLUSTERDB );

	return f;
}
//=======================================================================================


int _i_SCH_SUB_REMAIN_FORCE_IN_DELETE_FDHC( int side , int cm ) { // 2016.07.15
	int i , c;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return -1;
	//
	EnterCriticalSection( &CR_CLUSTERDB );
	//
	c = 0;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue;
		//
		c++;
		//
		_i_SCH_CLUSTER_Set_PathRange( side , i , -7 );
		//
	}

	//
	LeaveCriticalSection( &CR_CLUSTERDB );
	//
	return c;
	//
}
