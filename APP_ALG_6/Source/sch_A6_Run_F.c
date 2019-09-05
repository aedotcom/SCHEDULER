//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Equip_Handling.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_multi_alic.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_MultiJob.h"
#include "INF_User_Parameter.h"
//================================================================================
#include "sch_A6_subBM.h"
#include "sch_A6_sub.h"
#include "sch_A6_dll.h"
#include "sch_A6_Init.h"
//================================================================================

//_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 )
//
// DiffSide				: FM Part	:	PM Seq/Balance로 Arm 별로 다른 Side Wafer Pick 해야 할 경우 적용
// TM-BM_Pick_Order무시	: TM Part	:
//
//	Data	DiffSide	TM-BM_Pick_Order무시
//	0		X			X
//	1		O			X
//	2		X			O
//	3		O			O

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL FM_Pick_Running_Blocking_Style_6; // 2006.08.02
int  FM_Pick_Running_Blocking_Style_6_Side; // 2011.05.04
int  FM_Pick_Running_Blocking_Style_6_Pointer; // 2011.05.04
//------------------------------------------------------------------------------------------
void SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( int tms , int ch ); // 2013.11.26

BOOL SCHEDULER_COOLING_SKIP_AL6( int side , int pt ) { // 2014.08.26
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) == 2 ) return TRUE;
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) == 6 ) return TRUE; // 2014.08.26
	return FALSE;
}


int SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_6( int side , int bmc , BOOL placecmcheck , int option , int Dummy_Max , int Dummy_S , int Dummy_P , int *dummy_count ) {
	int i , j , ssd , ssp , ss , es , RemainCount , bmslot , bmslot2 , ch , dummyslot , dummyslot2 , dmpt , dmpt2 , uc , Result;
	int FM_CM , FM_CO1 , FM_CO2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_OSlot , FM_OSlot2 , FM_TSlot , FM_TSlot2;
	int dumsel , gidx;
	//
	*dummy_count = 0; // 2013.01.25
	//
	Result = 1;
	//
	if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) return Result;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FALSE , -1 ) ) return Result;


//printf( "=========================================================\n" );
//printf( "=<DUMMY:%d>==============================================\n" , option );
//printf( "=========================================================\n" );
	//=====================================================================================
	// Dll Interface
	//=====================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 1 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
	switch( USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE( bmc - BM1 + 1 , 0 ) ) { // 2010.07.27
	case 0 :
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 2 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
		return Result;
		break;
	case 1 :
		break;
	default :
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 3 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
		return -99;
		break;
	}
	//=====================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 4 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
	//============================================================================================================================
	// 2007.05.02
	//============================================================================================================================
	if ( !placecmcheck ) {
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return -1;
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return -2;
	}
	else {
		FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
		FM_Side2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
		//
		FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		//
		FM_TSlot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
		FM_TSlot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
		//-------------------------------------------------------------------------
		if ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) {
			//-------------------------------------------------------------------------
			Result = 0;
			//-------------------------------------------------------------------------
			if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
				FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
			}
			else if ( FM_TSlot > 0 ) {
				FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
			}
			else if ( FM_TSlot2 > 0 ) {
				FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 );
			}
			//-------------------------------------------------------------------------
			if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , side , IC , FM_TSlot , FM_TSlot2 , FM_CM , -1 , TRUE , FM_TSlot , FM_TSlot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
				_SCH_LOG_LOT_PRINTF( side , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , FM_TSlot , FM_TSlot2 , 9 , FM_TSlot , FM_TSlot2 , FM_TSlot2 * 100 + FM_TSlot );
				return -3;
			}
			//-------------------------------------------------------------------------
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return -4;
			//-------------------------------------------------------------------------
			if ( FM_TSlot > 0 ) {
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side , FM_Pointer , FM_TSlot , &FM_CO1 , &FM_OSlot ); // 2007.07.11
			}
			else {
				FM_OSlot = 0;
				FM_CO1 = 0;
			}
			if ( FM_TSlot2 > 0 ) {
				_SCH_SUB_GET_OUT_CM_AND_SLOT( FM_Side2 , FM_Pointer2 , FM_TSlot2 , &FM_CO2 , &FM_OSlot2 ); // 2007.07.11
			}
			else {
				FM_OSlot2 = 0;
				FM_CO2 = 0;
			}
			//-------------------------------------------------------------------------
/*
// 2007.12.03
			switch( _SCH_MACRO_FMx_OPERATION( 0 , MACRO_PLACE , FM_CO1 , FM_TSlot , FM_OSlot , FM_Side , FM_Pointer , FM_CO2 , FM_TSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 , -1 ) ) {
			case -1 :
				return -5;
				break;
			}
			//-------------------------------------------------------------------------
			//====================================================================================================================================================
			_SCH_MACRO_PLACE_TO_CMx_POST_PART_with_FM( FM_TSlot , FM_CO1 , FM_OSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_CO2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 , TRUE );
			//====================================================================================================================================================
*/
			//
			if ( FM_TSlot  > 0 ) SCH_CYCLON2_GROUP_RETURN( FM_Side , FM_Pointer ); // 2013.02.15
			if ( FM_TSlot2 > 0 ) SCH_CYCLON2_GROUP_RETURN( FM_Side2 , FM_Pointer2 ); // 2013.02.15
			//
			switch( _SCH_MACRO_FM_PLACE_TO_CM( side , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FALSE , FALSE , 0 ) ) {
			case -1 :
				return -5;
				break;
			}
		}
	}
	//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 5 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
	if ( Dummy_S < 0 ) {
		//
//		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_FOR_6( CHECKORDER_FOR_FM_PLACE , bmc , &ss , &es ); // 2005.01.24 // 2014.12.24
		SCHEDULER_CONTROL_Chk_GET_SLOT_FOR_BM_2_FOR_6( CHECKORDER_FOR_FM_PLACE , bmc , &ss , &es ); // 2005.01.24 // 2014.12.24
		//
		ssd = -1;
		for ( i = ss ; i <= es ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_SIDE( bmc , i ) == side ) {
					ssd = _SCH_MODULE_Get_BM_SIDE( bmc , i );
					ssp = _SCH_MODULE_Get_BM_POINTER( bmc , i );
					break;
				}
			}
		}
		if ( ssd == -1 ) {
			for ( i = ss ; i <= es ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
					ssd = _SCH_MODULE_Get_BM_SIDE( bmc , i );
					ssp = _SCH_MODULE_Get_BM_POINTER( bmc , i );
					break;
				}
			}
			if ( ssd == -1 ) return Result;
		}
	}
	else { // 2013.01.29
		ssd = Dummy_S;
		ssp = Dummy_P;
	}
	//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 6 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
	while ( TRUE ) {
		//
		RemainCount = SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_Free_Count_FOR_6( bmc , &bmslot , &bmslot2 , FALSE );
		//
		if ( RemainCount <= 0 ) return Result;
		//
		ch = bmc - BM1 + PM1;
		//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 7 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
		if ( !SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_6( ssd , &ch , &dummyslot , &uc , -1 ) ) {
			//=====================================================================================
			// Dll Interface
			//=====================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 8 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
			switch( USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE( bmc - BM1 + 1 , bmslot ) ) {
			case 0 :
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 9 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
				return Result;
				break;
			case 1 :
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 10 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
				continue;
				break;
			default :
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 11 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
				return -6;
				break;
			}
			//=====================================================================================
		}
		//===========================================================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 12 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
		if ( bmslot2 > 0 ) {
			//
			if ( ( Dummy_Max > 0 ) && ( ( *dummy_count + 1 ) >= Dummy_Max ) ) {
				//
				bmslot2 = 0; // 2013.01.29
				//
			}
			//
		}
		//===========================================================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 13 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( bmslot2 > 0 ) {
				if ( !SCHEDULER_CONTROL_Get_SDM_4_DUMMY_WAFER_SLOT_STYLE_6( ssd , &ch , &dummyslot2 , &uc , dummyslot ) ) {
					bmslot2 = 0;
					dummyslot2 = 0;
				}
			}
			else {
				dummyslot2 = 0;
			}
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
			bmslot2 = 0;
			dummyslot2 = 0;
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			bmslot2 = bmslot;
			bmslot = 0;
			dummyslot2 = dummyslot;
			dummyslot = 0;
		}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][dummyslot=%d][dummyslot2=%d]\n" , bmc , placecmcheck , option , Dummy_Max , dummyslot , dummyslot2 );
//----------------------------------------------------------------------
		//===========================================================================================================================
		//		if ( SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( bmc ) ) return 1; // 2010.07.27 // 2010.09.15
		if ( SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( bmc ) ) {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-2a = [dummyslot=%d][ssp=%d][ssp=%d][RemainCount=%d][bmslot=%d][bmslot2=%d]\n" , dummyslot , ssp , ssp , RemainCount , bmslot , bmslot2 );
//----------------------------------------------------------------------
			return Result; // 2010.07.27 // 2010.09.15
		}
		//===========================================================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-2 = [dummyslot=%d][ssp=%d][ssp=%d][RemainCount=%d][bmslot=%d][bmslot2=%d]\n" , dummyslot , ssp , ssp , RemainCount , bmslot , bmslot2 );
//----------------------------------------------------------------------
		//===============
		// Pick From Dummy dummyslot,dummyslot2 & Place to bmc bmslot,bmslot2
		//===============
		if ( dummyslot <= 0 ) {
			RemainCount = 1;
		}
		else {
			RemainCount = 0;
		}
//		for ( j = 0 ; j < 2 ; j++ ) { // 2010.09.29
		for ( j = 0 ; j < 3 ; j++ ) { // 2010.09.29
			//
//			for ( i = ssp + 1 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2010.09.28
			for ( i = ( ( j == 2 ) ? 0 : ssp + 1 ) ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2010.09.28
				//------------------------------------------------------------
				// 2007.05.15
				//------------------------------------------------------------
				dumsel = FALSE;
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-3 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
				if ( _SCH_CLUSTER_Get_PathRange( ssd , i ) < 0 ) {
					dumsel = TRUE;
				}
				else {
					if ( j != 0 ) {
						if ( _SCH_CLUSTER_Get_PathIn( ssd , i ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
							if ( _SCH_CLUSTER_Get_PathStatus( ssd , i ) == SCHEDULER_CM_END ) {
//								if ( _SCH_CLUSTER_Get_PathDo( ssd , i ) != PATHDO_WAFER_RETURN ) { // 2013.05.02
									if ( _SCH_CLUSTER_Get_PathDo( ssd , i ) > _SCH_CLUSTER_Get_PathRange( ssd , i ) ) {
										dumsel = TRUE;
									}
//								} // 2013.05.02
							}
						}
					}
				}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-4 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
				//------------------------------------------------------------
				if ( dumsel ) {
					//==========================================================================================================
					// (dummy,dummyslot)
					//==========================================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-5 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
					_SCH_CLUSTER_Copy_Cluster_Data( ssd , i , ssd , ssp , 0 );
					//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-6 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
					_SCH_CLUSTER_Set_PathIn( ssd , i , _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() );
					_SCH_CLUSTER_Set_PathOut( ssd , i , _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() );
					_SCH_CLUSTER_Set_SlotIn( ssd , i , ( RemainCount == 0 ) ? dummyslot : dummyslot2 );
					_SCH_CLUSTER_Set_SlotOut( ssd , i , ( RemainCount == 0 ) ? dummyslot : dummyslot2 );
					_SCH_CLUSTER_Set_PathStatus( ssd , i , SCHEDULER_READY );
					_SCH_CLUSTER_Set_PathDo( ssd , i , 0 ); // 2013.02.15
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-7 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
					//
					_SCH_CLUSTER_Set_CPJOB_CONTROL( ssd , i , -1 ); // 2010.09.13
					_SCH_CLUSTER_Set_CPJOB_PROCESS( ssd , i , -1 ); // 2010.09.13
					//
					_SCH_CLUSTER_Set_SwitchInOut( ssd , i , 1 ); // 2010.09.13
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-8 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
					//==========================================================================================================
					if ( RemainCount == 0 ) {
						dmpt = i;
						if ( dummyslot2 > 0 ) {
							RemainCount = 1;
						}
						else {
							RemainCount = 99;
							break;
						}
					}
					else {
						dmpt2 = i;
						RemainCount = 99;
						break;
					}
					//==========================================================================================================
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-9 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
				}
			}
			//===============
			if ( RemainCount == 99 ) break;
			//===============
		}
		//===============
		if ( RemainCount != 99 ) {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 14-9a = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
			return -7;
		}
		//===============
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 15 = [dummyslot=%d][ssp=%d][ssd=%d][RemainCount=%d][dumsel=%d][i=%d][j=%d]\n" , dummyslot , ssp , ssd , RemainCount , dumsel , i , j );
//----------------------------------------------------------------------
		//=============================================================================================
		// Pick From Dummy dummyslot,dummyslot2 & Place to bmc bmslot,bmslot2	ssd,dmpt  ssd,dmpt2
		//=============================================================================================
		if ( dummyslot > 0 ) {
			//=============================================================================================
			_SCH_SDM_Set_CHAMBER_INC_COUNT( dummyslot - 1 );
			//=============================================================================================
			_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot , MAX_CASSETTE_SIDE + 1 );
			_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot , 0 );
			_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot , dummyslot );
			//=============================================================================================
			_SCH_MODULE_Inc_FM_OutCount( ssd );
			//=============================================================================================
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( ssd , dmpt , 0 , -1 , -1 ); // 2007.05.18
			//=============================================================================================
		}
		if ( dummyslot2 > 0 ) {
			//=============================================================================================
			_SCH_SDM_Set_CHAMBER_INC_COUNT( dummyslot2 - 1 );
			//=============================================================================================
			_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot2 , MAX_CASSETTE_SIDE + 1 );
			_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot2 , 0 );
			_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , dummyslot2 , dummyslot2 );
			//=============================================================================================
			_SCH_MODULE_Inc_FM_OutCount( ssd );
			//=============================================================================================
			_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( ssd , dmpt2 , 0 , -1 , -1 ); // 2007.05.18
			//=============================================================================================
		}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP DUMMY_OPERATION_PUMP 16 = [bmc=%d][placecmcheck=%d][option=%d][Dummy_Max=%d][Dummy_S=%d][Dummy_P=%d]\n" , bmc , placecmcheck , option , Dummy_Max , Dummy_S , Dummy_P );
//----------------------------------------------------------------------
		//=============================================================================================
		if ( dummyslot > 0 ) _SCH_CLUSTER_Make_SupplyID( ssd , dmpt ); // 2013.01.29
		if ( dummyslot2 > 0 ) _SCH_CLUSTER_Make_SupplyID( ssd , dmpt2 ); // 2013.01.29
		//=============================================================================================
		switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 200 , -1 , dummyslot , dummyslot , ssd , dmpt , -1 , dummyslot2 , dummyslot2 , ssd , dmpt2 , -1 ) ) {
		case -1 :
			return -8;
			break;
		}
		//=============================================================================================
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( ssd ) ) || ( _SCH_SYSTEM_USING_GET( ssd ) <= 0 ) ) return -9;
		//=============================================================================================
		if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , ssd , AL , dummyslot , dummyslot2 , _SCH_CLUSTER_Get_PathIn( ssd , ( dummyslot > 0 ) ? dmpt : dmpt2 ) , bmc , TRUE , dummyslot , dummyslot2 , ssd , ssd , dmpt , dmpt2 ) == SYS_ABORTED ) {
			_SCH_LOG_LOT_PRINTF( ssd , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d%d\n" , dummyslot , dummyslot2 , 9 , dummyslot , dummyslot2 , 9 , dummyslot , dummyslot2 );
			return -10;
		}
		//=============================================================================================
		while ( TRUE ) {
			//
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( ssd ) ) || ( _SCH_SYSTEM_USING_GET( ssd ) <= 0 ) ) return -11;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) == 0 ) {
				//
				break;
			}
			//
			Sleep(1);
		}
		//=============================================================================================
		switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 210 , bmc , dummyslot , bmslot , ssd , dmpt , bmc , dummyslot2 , bmslot2 , ssd , dmpt2 , -1 ) ) {
		case -1 :
			return -12;
			break;
		}
		//=============================================================================================
		if ( dummyslot > 0 ) {
			_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , bmslot , ssd , dmpt );
			_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , bmslot , dummyslot , BUFFER_INWAIT_STATUS );
		}
		if ( dummyslot2 > 0 ) {
			_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , bmslot2 , ssd , dmpt2 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , bmslot2 , dummyslot2 , BUFFER_INWAIT_STATUS );
		}
		//=============================================================================================
		//
		if ( dummyslot > 0 ) {
			(*dummy_count)++; // 2013.01.25
			//
			SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bmc ) , &gidx , ssd , dmpt ); // 2013.02.15
			//
			_SCH_CLUSTER_Set_Stock( ssd , dmpt , gidx ); // 2013.02.15
			//
		}
		if ( dummyslot2 > 0 ) {
			(*dummy_count)++; // 2013.01.25
			//
			SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bmc ) , &gidx , ssd , dmpt2 ); // 2013.02.15
			//
			_SCH_CLUSTER_Set_Stock( ssd , dmpt2 , gidx ); // 2013.02.15
			//
		}
		//
		if ( ( Dummy_Max > 0 ) && ( *dummy_count >= Dummy_Max ) ) break; // 2013.01.25
		//
		//=============================================================================================
	}
	return Result;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FEM Scheduling
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_6( int CHECKING_SIDE , int mode ) {
	int k , Dummy_Result , Dummy_count , Dummy_Max , Dummy_S , Dummy_P , x , bm;
	if ( mode == 2 ) {
		//----------------------------------------------------------------------
		FM_Pick_Running_Blocking_Style_6 = FALSE; // 2006.08.02
		//---------------------------------------------------------------------
		if ( _SCH_MODULE_Chk_FM_Finish_Status( CHECKING_SIDE ) ) {
			//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_1 = TRUE\n" );
			//
			if ( _SCH_MODULE_Get_FM_OutCount( CHECKING_SIDE ) != _SCH_MODULE_Get_FM_InCount( CHECKING_SIDE ) ) {
				//
				for ( k = BM1 ; k < TM ; k++ ) { // 2004.02.25
					//
					if ( _SCH_PRM_GET_MODULE_MODE( k - BM1 + TM ) ) { // 2004.02.25
						//
						for ( x = 0 ; x < 2 ; x++ ) { // 2013.01.28
							//
							bm = k + (x*2);
							//
							if ( !_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) continue; // 2013.11.22
							//
							if ( SCHEDULING_ThisIs_BM_OtherChamber6( bm , 0 ) ) continue; // 2014.07.10
							//
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) == BUFFER_FM_STATION ) ) {
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) <= 0 ) {
									//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_1-1 = %d\n" , bm );
									if ( SCHEDULER_CONTROL_Chk_BM_has_just_one_for_supply_FOR_6( CHECKING_SIDE , bm ) ) {
										//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_1-2 = %d\n" , bm );
										if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bm , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) {
											//
											switch ( SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( bm , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) ) { // 2008.01.16
											case -1 :
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling PM Fail at %s%cWHBM%dFAIL 1%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( bm ) , 9 , bm - BM1 + 1 , 9 );
												return SYS_ABORTED;
												break;
											case 1 :
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_1-3 = %d\n" , bm );
												//===============================================================================================
												if ( !SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( bm ) ) { // 2010.07.27
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_1-3 = %d\n" , bm );
													//
													Dummy_count = 0;
													//
													if ( SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( bm , &Dummy_Max , &Dummy_S , &Dummy_P ) > 0 ) { // 2013.01.25
														//
														Dummy_Result = SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_6( CHECKING_SIDE , bm , FALSE , 1 , Dummy_Max , Dummy_S , Dummy_P , &Dummy_count );
														if ( Dummy_Result < 0 ) {
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Dummy Fail(%d) at %s%cWHBM%dFAIL 1%c\n" , Dummy_Result , _SCH_SYSTEM_GET_MODULE_NAME( bm ) , 9 , bm - BM1 + 1 , 9 );
															return SYS_ABORTED;
														}
													}
													//
													SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bm ) , &Dummy_count , -1 , -1 );
													//
													SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( bm ) , bm ); // 2013.11.26
													//
													_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bm , -1 , TRUE , 0 , 1 );
												}
												break;
											}
										}
									}
								}
							}
							//
//							if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) break; // 2013.01.28 // 2014.07.10
							if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 1 ) break; // 2013.01.28 // 2014.07.10
							//
						}
					}
				}
			}
		}
		//===================================================================================================
		// 2010.04.26
		//===================================================================================================
		if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) {
			//
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
				//
				for ( k = BM1 ; k < TM ; k++ ) {
					//
					if ( ( k - BM1 ) >= MAX_TM_CHAMBER_DEPTH ) continue; // 2010.05.21
					//
					if ( _SCH_PRM_GET_MODULE_MODE( k - BM1 + TM ) ) {
						//
						for ( x = 0 ; x < 2 ; x++ ) { // 2013.01.28
							//
							bm = k + (x*2);
							//
							if ( !_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) continue; // 2013.11.22
							//
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) == BUFFER_FM_STATION ) ) {
								//
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) <= 0 ) {
									//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-1 = %d\n" , bm );
									if ( SCHEDULER_CONTROL_Chk_BM_TM_PM_HAS_WAFER_CYCLON_STYLE_6( bm , TRUE ) ) {
										//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-2 = %d\n" , bm );
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bm , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
											//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-3 = %d\n" , bm );
											//
											Dummy_Max = 0; // 2015.01.12
											//
											if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bm , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) Dummy_Max = 1; // 2015.01.12
											//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-31 = %d , %d\n" , bm , Dummy_Max );
											//
											if ( Dummy_Max == 0 ) { // 2015.01.12
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_SUPPLY_and_DUMMY_STYLE_6( bm ) ) Dummy_Max = 1; // 2015.01.12
												//
											}
											//
											if ( Dummy_Max != 0 ) { // 2015.01.12
//											if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( bm , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) { // 2015.01.12
												//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-4 = %d\n" , bm );
												switch ( SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( bm , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) ) { // 2008.01.16
												case -1 :
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling BM Fail at %s%cWHBM%dFAIL 1%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( bm ) , 9 , bm - BM1 + 1 , 9 );
													return SYS_ABORTED;
													break;
												case 1 :
													//
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-5 = %d\n" , bm );
													if ( !SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( bm ) ) { // 2010.07.27
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 FINISH_CHECK_2-6 = %d\n" , bm );
														//
														Dummy_count = 0;
														//
														if ( SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( bm , &Dummy_Max , &Dummy_S , &Dummy_P ) > 0 ) { // 2013.01.25
															Dummy_Result = SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_6( CHECKING_SIDE , bm , FALSE , 2 , Dummy_Max , Dummy_S , Dummy_P , &Dummy_count );
															if ( Dummy_Result < 0 ) {
																_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Dummy Fail(%d) at %s%cWHBM%dFAIL 11%c\n" , Dummy_Result , _SCH_SYSTEM_GET_MODULE_NAME( bm ) , 9 , bm - BM1 + 1 , 9 );
																return SYS_ABORTED;
															}
														}
														//
														SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bm ) , &Dummy_count , -1 , -1 );
														//
														SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( bm ) , bm ); // 2013.11.26
														//
														_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bm , -1 , TRUE , 0 , 11 );
													}
													break;
												}
											}
										}
									}
								}
							}
							//
							if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() != 2 ) break; // 2013.01.28
							//
						}
					}
				}
			}
		}
	}
	return -1;
}



BOOL SCHEDULER_CONTROL_Chk_Pick_CM_Dummy_Possible_FOR_6( int side , int pointer ) { // 2010.09.17
	int i;
	for ( i = pointer ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//======================================================================================================================================
		if ( _SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//======================================================================================================================================
		if ( _SCH_CLUSTER_Get_PathIn( side , i ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
			if ( _SCH_IO_GET_MODULE_STATUS( _SCH_CLUSTER_Get_PathIn( side , i ) , _SCH_CLUSTER_Get_SlotIn( side , i ) ) <= 0 ) {
				return FALSE;
			}
		}
		else {
			return TRUE;
		}
		//======================================================================================================================================
	}
	//
	return TRUE;
}


int SCHEDULER_CONTROL_Chk_FM_DUMMY_SUPPLY( int CHECKING_SIDE ) {
	int i , t , next_BM , Justoneuse , bm , Slotmode;
	int Dummy_Result , Dummy_Max , Dummy_count , Dummy_S , Dummy_P;
	//
	if ( !SCH_CYCLON2_GROUP_GET_NEED_DUMMY( -1 , &t ) ) return 0;
	//
	next_BM = 1;
	//
	for ( i = 0 ; i < t ; i++ ) {
		next_BM = next_BM * 10;
	}
	//
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		Slotmode = 0;
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		Slotmode = 1;
	}
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		Slotmode = 2;
	}
	//
	if ( SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_PRE_FOR_6( CHECKING_SIDE , next_BM , Slotmode , 0 , &bm , &Justoneuse , FALSE ) <= 0 ) return 0;
	//
	if ( SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( bm ) ) return 0;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_FM_STATION ) return 0;
	//
//	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) != 0 ) return 0;
	//
//printf( "[DUMMY] [1] [next_BM=%d][Slotmode=%d][bm=%d]\n" , next_BM , Slotmode , bm );

	Dummy_count = 0;
	//
	if ( SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( bm , &Dummy_Max , &Dummy_S , &Dummy_P ) > 0 ) {
		//
//printf( "[DUMMY] [2] [next_BM=%d][Slotmode=%d][bm=%d]\n" , next_BM , Slotmode , bm );
		//
		Dummy_Result = SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_6( CHECKING_SIDE , bm , FALSE , 1 , Dummy_Max , Dummy_S , Dummy_P , &Dummy_count );
		//
		if ( Dummy_Result < 0 ) {
			//
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Dummy Fail(%d) at %s%cWHBM%dFAIL 22%c\n" , Dummy_Result , _SCH_SYSTEM_GET_MODULE_NAME( bm ) , 9 , bm - BM1 + 1 , 9 );
			//
			return -1;
			//
		}
	}
	//
//printf( "[DUMMY] [3] [next_BM=%d][Slotmode=%d][bm=%d][Dummy_Max=%d][Dummy_count=%d]\n" , next_BM , Slotmode , bm , Dummy_Max , Dummy_count );
	//
	SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( bm ) , &Dummy_count , -1 , -1 );
	//
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( bm , -1 ) > 0 ) {
		//
		while ( TRUE ) {
			//
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) return -1;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) == 0 ) {
				//
				SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( bm ) , bm ); // 2013.11.26
				//
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bm , -1 , TRUE , 0 , 1 );
				//
				break;
			}
			//
			Sleep(1);
		}
		//
	}
	//
	return 1;
	//
}

BOOL SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( int tms );
BOOL SCHEDULER_CONTROL_Chk_PM_WAFER_NONE( int tms );
BOOL SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( int tms , BOOL pmdischeck , BOOL wfrcheck , int chkbm );

void SCHEDULER_CONTROL_Chk_PM_NONE_AND_STOCKBM_GO_TMSIDE( int side ) {
	int k , x , bm;
	//
	for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( k + TM ) ) continue;
		//
		if ( !SCHEDULER_CONTROL_Chk_PM_WAFER_NONE( k ) ) continue;
		//
		if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( k , FALSE , FALSE , -1 ) ) continue;
		//
		for ( x = 0 ; x < 2 ; x++ ) { // 2013.01.28
			//
			bm = k + BM1 + (x*2);
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) continue;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_FM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) > 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm , -1 ) > 0 ) continue;
			//
			SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( k , bm ); // 2013.11.26
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bm , -1 , TRUE , 0 , 8302 );
			//
		}
		//
	}
	//

}


BOOL SCHEDULER_CONTROL_Chk_PM_DISABLE_AND_BM_GO_TMSIDE( int side ) {
	int k , bm , x , s , p , w , s2 , w2 , p2;
	//
	k = FALSE;
	//
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		//
		s = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
		p = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		//
		if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( _SCH_CLUSTER_Get_Stock( s , p ) % 100 ) ) {
			k = TRUE;
		}
		else {
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
				k = TRUE;
			}
		}
		//
	}
	//
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		//
		s = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
		p = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		//
		if ( SCHEDULER_CONTROL_Chk_PM_DISABLE_CHECK( _SCH_CLUSTER_Get_Stock( s , p ) % 100 ) ) {
			k = TRUE;
		}
		else {
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
				k = TRUE;
			}
		}
		//
	}
	//
	if ( k ) {
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
			//
			s = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
			p = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
			w = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
			//
			if ( _SCH_CLUSTER_Get_SwitchInOut( s , p ) < 4 ) {
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( s , p , -1 );
				SCH_CYCLON2_GROUP_SUPPLY_STOP( s , p );
			}
			//
		}
		else {
			w = 0;
		}
		//
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
			//
			s2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
			p2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
			w2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
			//
			if ( _SCH_CLUSTER_Get_SwitchInOut( s2 , p2 ) < 4 ) {
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( s2 , p2 , -1 );
				SCH_CYCLON2_GROUP_SUPPLY_STOP( s2 , p2 );
			}
			//
		}
		else {
			w2 = 0;
		}
		//=============================================================================================
		if ( w  > 0 ) SCH_CYCLON2_GROUP_RETURN( s , p );
		if ( w2 > 0 ) SCH_CYCLON2_GROUP_RETURN( s2 , p2 );
		//=============================================================================================
		switch( _SCH_MACRO_FM_PLACE_TO_CM( side , w , s , p , w2 , s2 , p2 , FALSE , FALSE , 0 ) ) {
		case -1 :
			return FALSE;
			break;
		}
		//=============================================================================================
	}
	//
	for ( k = 0 ; k < MAX_TM_CHAMBER_DEPTH ; k++ ) {
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( k + TM ) ) continue;
		//
		if ( !SCHEDULER_CONTROL_Chk_STOCK_BM_HAS_RETURN( k , TRUE , FALSE , -1 ) ) continue;
		//
		for ( x = 0 ; x < 2 ; x++ ) { // 2013.01.28
			//
			bm = k + BM1 + (x*2);
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( bm , FALSE , -1 ) ) continue;
			//
			if ( _SCH_MODULE_Get_BM_FULL_MODE( bm ) != BUFFER_FM_STATION ) continue;
			//
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bm ) > 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_6( bm , -1 ) > 0 ) continue;
			//
			SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( k , bm ); // 2013.11.26
			//
			_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bm , -1 , TRUE , 0 , 8301 );
			//
		}
		//
	}
	//
	return TRUE;
}


int USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_6( int CHECKING_SIDE ) {
	int	FingerCount;
	int	GetCount , GetSlot , GetSlot2;
	int	FM_Slot , FM_Slot2;
	int	FM_TSlot , FM_TSlot2;
	int	FM_OSlot , FM_OSlot2;
	int	FM_Pointer , FM_Pointer2;
	int k;
	int FM_Buffer , FM_Buffer2;
	int FM_CM , FM_CO1 , FM_CO2;
	int Slotmode , next_FM_sDoPointer , next_FM_DoPointer , next_BM , Justoneuse;
	int FM_Side , FM_Side2 , retdata;
	int runing_action; // 2007.05.03
	int Dummy_Result;
	int ALsts , ICsts , ICsts2 , ICfinger , coolplace;
	int FM_crs_act; // 2009.03.10
	int gidx , Dummy_Max , Dummy_count , Dummy_S , Dummy_P; // 2013.01.25
	int forcediv , hasinvalid;
	BOOL Pick_Other_Waiting; // 2013.11.27
	int SupplyCount; // 2014.01.07
	int	Get99 , GetBMPickOnly , GetCountB , GetSlotB , GetSlot2B; // 2014.10.03

	runing_action = FALSE; // 2007.05.03

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PICK FROM CM
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	FingerCount = 0;
	//
	if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) FingerCount++;
	if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) FingerCount++;
	//
	//=======================================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.29
		if ( FingerCount <= 0 ) {
			switch( SCHEDULER_CONTROL_Chk_FM_DUMMY_SUPPLY( CHECKING_SIDE ) ) {
			case 1 :
				return 1;
			case -1 :
				return 0;
			}
		}
		//
		SCHEDULER_CONTROL_Chk_PM_NONE_AND_STOCKBM_GO_TMSIDE( CHECKING_SIDE );
		//
		if ( !SCHEDULER_CONTROL_Chk_PM_DISABLE_AND_BM_GO_TMSIDE( CHECKING_SIDE ) ) return 0;
		//
	}
	//----------------------------------------------------------------------
	FM_Pick_Running_Blocking_Style_6_Side = -1; // 2011.05.04
	FM_Pick_Running_Blocking_Style_6_Pointer = -1; // 2011.05.04
	FM_Pick_Running_Blocking_Style_6 = TRUE; // 2006.08.02
	//---------------------------------------------------------------------
	//
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-1 RESULT = [FingerCount=%d][%d]\n" , FingerCount , _SCH_MODULE_Get_FM_MidCount() ); // 2012.07.13
	//=======================================================================================================
	// 2006.11.16
	//=======================================================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 0 ) { // 2007.05.03
		if ( FingerCount <= 0 ) {
			if ( SCHEDULER_CONTROL_Chk_FM_BMOUT_DIRECT_RETURN_FOR_6( CHECKING_SIDE ) ) {
				_SCH_MODULE_Set_FM_MidCount( 0 );
//				FingerCount++; // 2012.07.13
				FingerCount = 11; // 2012.07.13
			}
		}
	}
	//=======================================================================================================
	if ( ( FingerCount <= 0 ) && ( _SCH_MODULE_Get_FM_MidCount() == 0 ) ) { // 2003.10.23
		if ( !_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.08.26
//			FingerCount = 1; // 2012.07.13
			FingerCount = 12; // 2012.07.13
		}
	}
	//=======================================================================================================
	// 2007.07.04
	//=======================================================================================================
	if ( FingerCount <= 0 ) {
		if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
//			if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() < 1 ) FingerCount = 1; // 2012.07.13
			if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() < 1 ) FingerCount = 13; // 2012.07.13
		}
	}
	//=======================================================================================================
	//----------------------------------------------------------------------
//	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 RESULT = [FingerCount=%d]\n" , FingerCount ); // 2012.07.13
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-2 RESULT = [FingerCount=%d]\n" , FingerCount ); // 2012.07.13
	//----------------------------------------------------------------------
	Pick_Other_Waiting = FALSE; // 2013.11.27
	//
	if ( ( FingerCount <= 0 ) && ( _SCH_MODULE_Get_FM_MidCount() == 0 ) ) {
		if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) {
			//
			GetCount = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_6( 0 , CHECKING_SIDE ,
				&FM_Slot , &FM_Slot2 ,
				&FM_Pointer , &FM_Pointer2 ,
				&Slotmode , &next_FM_sDoPointer , &next_FM_DoPointer ,
				&next_BM , FALSE , &forcediv , &hasinvalid );
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 RESULT = [FingerCount=%d][FM_Buffer=?][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
			//----------------------------------------------------------------------
			if ( GetCount >= 0 ) {
				//====================================================================================================
				// 2007.08.24
				//====================================================================================================
				if ( _SCH_MODULE_Need_Do_Multi_FIC() && _SCH_MODULE_Chk_MFIC_Yes_for_Get( -1 ) ) ALsts = TRUE;
				else                                                                             ALsts = FALSE;
				//====================================================================================================
				if ( !SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_MIC_FOR_6( FM_Slot , CHECKING_SIDE , FM_Pointer , FM_Slot2 , CHECKING_SIDE , FM_Pointer2 ) ) { // 2007.11.06
					//
//					GetCount = -1; // 2014.05.26
					GetCount = -101; // 2014.05.25
					//
				}
				else {
					/*
					// 2014.05.26
					if ( SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_PRE_FOR_6( CHECKING_SIDE , next_BM , Slotmode , &FM_Buffer , &Justoneuse , ALsts ) <= 0 ) {
						//
						GetCount = -2; // 2004.05.10
						//
					} // 2004.05.10
					else {
					*/
					//
					// 2014.05.26
					//
					GetCount = SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_PRE_FOR_6( CHECKING_SIDE , next_BM , Slotmode , Slotmode , &FM_Buffer , &Justoneuse , ALsts );
					//
					if ( GetCount <= 0 ) {
						//
						if ( ALsts ) {
							GetCount = GetCount - 200;
						}
						else {
							GetCount = GetCount - 300;
						}
						//
					}
					else {
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d][Justoneuse=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount , Justoneuse );
						//----------------------------------------------------------------------
						switch ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Cyclon_FOR_6( FM_Buffer , 2 , CHECKING_SIDE , FM_Pointer , FM_Slot , CHECKING_SIDE , FM_Pointer2 , FM_Slot2 ) ) { // 2007.05.01
						case 0 :
							break;
						case 1 :
//							GetCount = -3; // 2014.05.26
							GetCount = -102; // 2014.05.26
							break;
						case 2 :
							//
							Justoneuse = TRUE;
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
							break;
						}
						//
						if ( GetCount >= 0 ) { // 2013.11.27
							//
							if ( SCHEDULER_CONTROL_Chk_Place_BM_Force_Waiting_FOR_6( FM_Buffer , CHECKING_SIDE , FM_Pointer , FM_Slot , FM_Pointer2 , FM_Slot2 , &Pick_Other_Waiting ) ) { // 2013.11.27
//								GetCount = -4; // 2014.05.26
								GetCount = -103; // 2014.05.26
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d][Justoneuse=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount , Justoneuse );
							//----------------------------------------------------------------------
						}
					}
					//
					if ( GetCount > 0 ) { // 2015.12.15
						//
						k = SCH_BM_SUPPLY_LOCK_GET_FOR_AL6( 99 ); // 2015.12.15
						//
						if ( ( k >= 0 ) && ( k < MAX_CASSETTE_SIDE ) ) {
							//
							if ( k != CHECKING_SIDE ) {
								//
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 );
								//
								if ( !_SCH_SUB_Remain_for_FM( k ) ) SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , -1 , 0 );
								//
								GetCount = -122;
								//
							}
							//
						}
						//
					}
					//
					if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) { // 2010.09.17
						//
						if ( GetCount >= 0 ) { // 2010.09.15
							//
							if ( !SCHEDULER_CONTROL_Chk_Pick_CM_Dummy_Possible_FOR_6( CHECKING_SIDE , FM_Pointer ) ) { // 2010.09.17
								//
//								GetCount = -4; // 2014.05.26
								GetCount = -104; // 2014.05.26
								//
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_6( FM_Buffer ) ) {
									//
									if ( SCHEDULER_CONTROL_Chk_BM_TM_PM_HAS_WAFER_CYCLON_STYLE_6( FM_Buffer , FALSE ) ) {
										//
										SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , FM_Buffer ); // 2013.11.26
										//
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 111 );
										//
									}
								}
								//
							}
						}
					}
					//
				}
				//===============================================================================================================
				// 2004.05.10
				//===============================================================================================================
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 03 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d][Justoneuse=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount , Justoneuse );
				//----------------------------------------------------------------------
				//
				SupplyCount = ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ? 2 : 1; // 2014.01.07
				//
				if ( GetCount >= 0 ) {
					switch( Slotmode ) {
					case 0 : // 0:AB
						if ( Justoneuse ) {
							// pick from CM ( A , FM_Slot , 0 );
							Slotmode = 1;
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
							next_FM_DoPointer = next_FM_sDoPointer;
						}
						else {
							//
							// 2014.01.07
							//
							if ( SupplyCount > 1 ) {
								//
								_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , 1 );
								//
								SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , -1 , 0 ); // 2015.12.15
								//
								SupplyCount--;
								//
//								if ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) { // PM Seq/Balance로 Arm 별로 다른 Side Wafer Pick 해야 할 경우 적용 // 2014.05.23
								if ( ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 1 ) || ( _SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( 1 ) == 3 ) ) { // PM Seq/Balance로 Arm 별로 다른 Side Wafer Pick 해야 할 경우 적용 // 2014.05.23
									//
									if ( !_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2014.01.07
										//
										Justoneuse = TRUE;
										Slotmode = 1;
										FM_Slot2 = 0;
										FM_Pointer2 = 0;
										next_FM_DoPointer = next_FM_sDoPointer;
										//
										SupplyCount--;
										//
									}
									//
								}
								//
							}
							//
						}
						break;
					case 1 : // 1:A(1)
						// pick from CM ( A , FM_Slot , 0 );
						break;
					case 2 : // 2:B(1)
						// pick from CM ( B , 0 , FM_Slot2 );
						break;
					}
					if ( ( GetCount >= 0 ) && ( FM_Slot > 0 ) ) {
						if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , FM_Pointer ) ) {
//							GetCount = -101; // 2014.05.26
							GetCount = -111; // 2014.05.26
						}
					}
					if ( ( GetCount >= 0 ) && ( FM_Slot2 > 0 ) ) {
						if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , FM_Pointer2 ) ) {
//							GetCount = -102; // 2014.05.26
							GetCount = -112; // 2014.05.26
						}
					}
				}
				//===============================================================================================================
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 04 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
				//----------------------------------------------------------------------
				if ( GetCount >= 0 ) {
					switch( Slotmode ) {
					case 0 : // 0:AB
						if ( Justoneuse ) {
							// pick from CM ( A , FM_Slot , 0 );
							_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
//								Slotmode = 1; // 2004.05.10
//								FM_Slot2 = 0; // 2004.05.10
//								FM_Pointer2 = 0; // 2004.05.10
//								next_FM_DoPointer = next_FM_sDoPointer; // 2004.05.10
						}
						else {
							// pick from CM ( AB , FM_Slot , FM_Slot2 );
							_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
							_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
						}
						break;
					case 1 : // 1:A(1)
						// pick from CM ( A , FM_Slot , 0 );
						_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
						break;
					case 2 : // 2:B(1)
						// pick from CM ( B , 0 , FM_Slot2 );
						_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
						break;
					}
					//----------------------------------------------------------------------
					// Code for Pick from CM
					// FM_Slot
					//----------------------------------------------------------------------
					//----------------------------------------------------------------------------
//						_SCH_CASSETTE_Reset_Side_Monitor( CHECKING_SIDE , 0 ); // 2003.10.23 // 2007.11.27
//					_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ? 2 : 1 ); // 2003.10.23
					//
					if ( SupplyCount > 0 ) _SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , SupplyCount ); // 2014.01.07
					//
					SCH_BM_SUPPLY_LOCK_SET2_FOR_AL6( 99 , -1 , 0 ); // 2015.12.15
					//
					//----------------------------------------------------------------------------
					//----------------------------------------------------------------------------
					if ( FM_Slot > 0 ) {
						//
						FM_Pick_Running_Blocking_Style_6_Side = CHECKING_SIDE; // 2011.05.04
						FM_Pick_Running_Blocking_Style_6_Pointer = FM_Pointer; // 2011.05.04
						//
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , FM_Pointer , 0 , 0 , 0 ); // 2006.11.27
						//
						SCH_CYCLON2_GROUP_SET_SUPPLY( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , CHECKING_SIDE , FM_Pointer , &gidx , forcediv ); // 2013.01.25
						//
						_SCH_CLUSTER_Set_Stock( CHECKING_SIDE , FM_Pointer , gidx ); // 2013.01.25
						//
					}
					if ( FM_Slot2 > 0 ) {
						//
						FM_Pick_Running_Blocking_Style_6_Side = CHECKING_SIDE; // 2011.05.04
						FM_Pick_Running_Blocking_Style_6_Pointer = FM_Pointer2; // 2011.05.04
						//
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27
						//
						SCH_CYCLON2_GROUP_SET_SUPPLY( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , CHECKING_SIDE , FM_Pointer2 , &gidx , forcediv ); // 2013.01.25
						//
						_SCH_CLUSTER_Set_Stock( CHECKING_SIDE , FM_Pointer2 , gidx ); // 2013.01.25
						//
					}
					//=================================================================================================
					// 2007.05.02
					//=================================================================================================
					if ( FM_Slot > 0 ) {
						if ( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
							//
							_SCH_SDM_Set_CHAMBER_INC_COUNT( FM_Slot - 1 ); // 2010.08.30
							//
							_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot , MAX_CASSETTE_SIDE + 1 );
							_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot , 0 );
							_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot , FM_Slot );
						}
					}
					if ( FM_Slot2 > 0 ) {
						if ( _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
							//
							_SCH_SDM_Set_CHAMBER_INC_COUNT( FM_Slot2 - 1 ); // 2010.08.30
							//
							_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot2 , MAX_CASSETTE_SIDE + 1 );
							_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot2 , 0 );
							_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot2 , FM_Slot2 );
						}
					}
					//=================================================================================================
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 100 , -1 , FM_Slot , FM_Slot , CHECKING_SIDE , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , CHECKING_SIDE , FM_Pointer2 , -1 ) ) { // 2007.03.21
					case -1 :
						return FALSE;
						break;
					}
					//=================================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , FM_Slot , FM_Slot , CHECKING_SIDE , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , CHECKING_SIDE , FM_Pointer2 , -1 , -1 ); // 2007.11.26
					//=================================================================================================
					//----------------------------------------------------------------------
					_SCH_CLUSTER_Set_SwitchInOut( CHECKING_SIDE , FM_Pointer , 0 ); // 2013.08.01
					//----------------------------------------------------------------------
					_SCH_MODULE_Set_FM_DoPointer( CHECKING_SIDE , next_FM_DoPointer );
					_SCH_MODULE_Set_FM_MidCount( FM_Buffer ); // go BM Set
					//----------------------------------------------------------------------

					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
					//----------------------------------------------------------------------
					//==============================================================================================
					// 2006.03.02
					//==============================================================================================
					//----------------------------------------------------------------------
					//
					// Code Aligning Before Place to BI
					//
					//----------------------------------------------------------------------
					if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
						FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
					}
					else if ( FM_Slot > 0 ) {
						FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
					}
					else if ( FM_Slot2 > 0 ) {
						FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 );
					}
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_Buffer , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
						return FALSE;
					}
					//==============================================================================================

					//==============================================================================================
					// 2005.08.09
					//==============================================================================================
					if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 5 ) ||
						 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 6 ) ) {
						if ( FM_Slot > 0 ) {
							_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , FM_Pointer , PATHDO_WAFER_RETURN );
						}
						if ( FM_Slot2 > 0 ) {
							_SCH_CLUSTER_Set_PathDo( CHECKING_SIDE , FM_Pointer2 , PATHDO_WAFER_RETURN );
						}
					}
					//==============================================================================================
					//----------------------------------------------------------------------
					//----------------------------------------------------------------------
					if ( Slotmode == 0 ) {
						if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
							GetCount = 0;
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_WAIT_STATION ) && ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
									GetCount = 1;
								}
							}
							if ( GetCount == 0 ) {
								if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , FM_Slot , FM_Slot2 ) == SYS_ABORTED ) {
									return FALSE;
								}
							}
						}
					}
					else if ( Slotmode == 1 ) { // 2003.06.10
						//===============================================================================================================================
						if ( !forcediv ) { // 2013.04.01
							if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 0 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) ) { // 2007.07.05
							//===============================================================================================================================
								if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
									//----------------------------------------------------------------------
									//==========================================================================
									Sleep(250);
									//==========================================================================
									Pick_Other_Waiting = FALSE; // 2013.11.27
									//
									for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
										//==========================================================================================
										if ( _SCH_SYSTEM_MODE_END_GET( k ) != 0 ) continue; // 2011.07.21
										if ( CHECKING_SIDE == k ) continue;
										//==========================================================================================
										_SCH_MACRO_OTHER_LOT_LOAD_WAIT( k , 5 ); // 2011.07.21
										//==========================================================================================
										/*
										// 2011.07.21
										if ( _SCH_SYSTEM_MODE_END_GET( k ) != 0 ) continue;
										if ( CHECKING_SIDE == k ) continue;
										if ( _SCH_SYSTEM_USING_GET( k ) == 8 ) {
											if ( ( _SCH_SYSTEM_MODE_GET( k ) == 1 ) || ( _SCH_SYSTEM_MODE_GET( k ) == 3 ) ) {
												//==========================================================================
												Sleep(500);
												//==========================================================================
											}
										}
										if ( _SCH_SYSTEM_USING_GET( k ) < 9 ) continue;
										//
										if ( _SCH_SYSTEM_ABORT_PAUSE_CHECK( k ) == 1 ) continue; // 2008.07.01
										//
										*/
										//================================================================================================
										if ( !_SCH_SYSTEM_USING_RUNNING( k ) ) continue; // 2010.06.10
										//================================================================================================
										GetCount = SCHEDULING_CHECK_Enable_PICK_from_FM_ArmBOnly_for_STYLE_6( 0 , k , &next_BM ,
													&FM_Slot2 , 
													&FM_Pointer2 ,
													&next_FM_DoPointer , &hasinvalid );
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
										//----------------------------------------------------------------------
										if ( GetCount >= 0 ) {
											//
											if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) { // 2010.09.17
												if ( !SCHEDULER_CONTROL_Chk_Pick_CM_Dummy_Possible_FOR_6( k , FM_Pointer2 ) ) { // 2010.09.17
//													GetCount = -1; // 2014.05.26
													GetCount = -501; // 2014.05.26
												}
											}
											//
											if ( GetCount >= 0 ) {
												//
												if ( !SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_MIC_FOR_6( FM_Slot2 , k , FM_Pointer2 , -1 , -1 , -1 ) ) { // 2007.11.06
//													GetCount = -1; // 2014.05.26
													GetCount = -502; // 2014.05.26
												}
												else {
													if ( SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_PRE_FOR_6( k , next_BM , 2 , 2 , &FM_Buffer2 , &Justoneuse , FALSE ) <= 0 ) {
//														GetCount = -1; // 2004.05.10 // 2014.05.26
														GetCount = -503; // 2004.05.10 // 2014.05.26
													} // 2004.05.10
													else { // 2006.04.04
														if ( FM_Buffer != FM_Buffer2 ) { // 2006.04.04
															//----------------------------------------------------------------------
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
															//----------------------------------------------------------------------
//															GetCount = -1; // 2006.04.04 // 2014.05.26
															GetCount = -504; // 2006.04.04 // 2014.05.26
															//====================================================================================================
															// 2006.04.04
															//====================================================================================================
															_SCH_CLUSTER_Set_Buffer( CHECKING_SIDE , FM_Pointer , 1 );
															//====================================================================================================
														}
														else {
															//----------------------------------------------------------------------
															_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06c RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
															//----------------------------------------------------------------------
															switch ( SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Cyclon_FOR_6( FM_Buffer , 1 , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , k , FM_Pointer2 , FM_Slot2 ) ) { // 2007.05.01
															case 0 :
																break;
															case 1 :
															case 2 :
//																GetCount = -1; // 2014.05.26
																GetCount = -505; // 2014.05.26
																_SCH_CLUSTER_Set_Buffer( CHECKING_SIDE , FM_Pointer , 1 );
																break;
															}
														}
													}
												}
											}
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06d RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
											//----------------------------------------------------------------------
											//==============================================================================================
											// 2005.09.14
											//==============================================================================================
											if ( GetCount >= 0 ) {
	//											if ( SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_Free_Count_FOR_6( FM_Buffer2 ) <= 1 ) { // 2007.05.02
	//											if ( SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_Free_Count_FOR_6( FM_Buffer2 , &ALsts , &ALsts ) <= 1 ) { // 2007.05.02 // 2009.03.09
												if ( SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_Free_Count_FOR_6( FM_Buffer2 , &ALsts , &ALsts , ( FM_Buffer == FM_Buffer2 ) ) <= 1 ) { // 2007.05.02 // 2009.03.09
//													GetCount = -132; // 2014.05.26
													GetCount = -511; // 2014.05.26
												}
												else { // 2006.04.04
													if ( !SCHEDULER_CONTROL_Chk_BM_for_FM_PLACE_TM_ArmCheck_FOR_6( FM_Buffer2 , CHECKING_SIDE , FM_Pointer , k , FM_Pointer2 ) ) { // 2006.04.04
//														GetCount = -133; // 2006.04.04 // 2014.05.26
														GetCount = -512; // 2006.04.04 // 2014.05.26
														//====================================================================================================
														// 2006.04.04
														//====================================================================================================
														_SCH_CLUSTER_Set_Buffer( CHECKING_SIDE , FM_Pointer , 1 );
														//====================================================================================================
													} // 2006.04.04
												}
											}
											//==============================================================================================
											// 2004.05.10
											//==============================================================================================
											if ( GetCount >= 0 ) {
												if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( k , FM_Pointer2 ) ) {
//													GetCount = -101; // 2014.05.26
													GetCount = -513; // 2014.05.26
												}
											}
											//==============================================================================================
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06e RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
											//----------------------------------------------------------------------
											if ( GetCount >= 0 ) { // 2004.05.10
												// pick from CM ( B , 0 , FM_Slot2 );
												_SCH_MODULE_Inc_FM_OutCount( k );
												FM_Slot = 0;
												FM_Pointer = 0;
												//----------------------------------------------------------------------
												// Code for Pick from CM
												// FM_Slot2
												//----------------------------------------------------------------------
												if ( _SCH_CLUSTER_Get_PathIn( k , FM_Pointer2 ) == _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2010.08.30
													//
													_SCH_SDM_Set_CHAMBER_INC_COUNT( FM_Slot2 - 1 ); // 2010.08.30
													//
												}
												//----------------------------------------------------------------------
												_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( k , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27
												//
												SCH_CYCLON2_GROUP_SET_SUPPLY( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer2 ) , k , FM_Pointer2 , &gidx , FALSE ); // 2013.01.25
												//
												_SCH_CLUSTER_Set_Stock( k , FM_Pointer2 , gidx ); // 2013.01.25
												//

												//----------------------------------------------------------------------
												switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 110 , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , k , FM_Pointer2 , -1 ) ) { // 2007.03.21
												case -1 :
													return FALSE;
													break;
												}
												//=================================================================================================
												_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , -1 , 0 , 0 , 0 , 0 , -1 , FM_Slot2 , FM_Slot2 , k , FM_Pointer2 , -1 , -1 ); // 2007.11.26
												//=================================================================================================
												//----------------------------------------------------------------------
												_SCH_CLUSTER_Set_SwitchInOut( k , FM_Pointer2 , 0 ); // 2013.08.01
												//----------------------------------------------------------------------
	/*
	// 2007.11.26
												SCHEDULER_CONTROL_Set_FMx_POINTER( TB_STATION , FM_Pointer2 );
												SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , k , FM_Slot2 );
	*/			
												//----------------------------------------------------------------------
												_SCH_MODULE_Set_FM_DoPointer( k , next_FM_DoPointer );
												//----------------------------------------------------------------------
												//==============================================================================================
												// 2006.03.02
												//==============================================================================================
												//----------------------------------------------------------------------
												//
												// Code Aligning Before Place to BI
												//
												//----------------------------------------------------------------------
												FM_CM = _SCH_CLUSTER_Get_PathIn( k , FM_Pointer2 );
												if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , k , AL , 0 , FM_Slot2 , FM_CM , FM_Buffer2 , TRUE , 0 , FM_Slot2 , 0 , k , 0 , FM_Pointer2 ) == SYS_ABORTED ) {
													_SCH_LOG_LOT_PRINTF( k , "FM Handling Fail at AL(B:%d)%cWHFMALFAIL B:%d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
													return FALSE;
												}
												//==============================================================================================
												break;
											}
										}
										else { // 2013.05.27
											//
											if ( ( GetCount == -99 ) && ( hasinvalid >= 0 ) ) {
												SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_NORMALFULL( hasinvalid , CHECKING_SIDE , TRUE );
											}
											//
										}
									}
								}
							}
						}
					}
					//
					if ( Pick_Other_Waiting ) { // 2013.11.27
						//
						Pick_Other_Waiting = 0;
						//
						for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
							//==========================================================================================
							if ( _SCH_SYSTEM_MODE_END_GET( k ) != 0 ) continue;
							if ( CHECKING_SIDE == k ) continue;
							//==========================================================================================
							_SCH_MACRO_OTHER_LOT_LOAD_WAIT( k , 3 );
							//==========================================================================================
							if ( _SCH_SYSTEM_USING_GET( k ) > 0 ) {
								//================================================================================================
								if ( !_SCH_SYSTEM_USING_RUNNING( k ) ) {
									//
									if ( Pick_Other_Waiting < 3 ) {
										//
										Pick_Other_Waiting++;
										k--;
										continue;
									}
									else {
										Pick_Other_Waiting = 0;
									}
								}
								//================================================================================================
							}	
							//==========================================================================================
						}
						//
					}
					//
				}
			}
			else { // 2013.05.27
				if ( ( GetCount == -99 ) && ( hasinvalid >= 0 ) ) {
					SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_NORMALFULL( hasinvalid , CHECKING_SIDE , TRUE );
				}
			}
		}
	}
	//----------------------------------------------------------------------
	FM_Pick_Running_Blocking_Style_6 = FALSE; // 2006.08.02
	//---------------------------------------------------------------------
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 2 from FEM%cWHFMFAIL 2\n" , 9 );
		return FALSE;
	}
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	// PLACE TO BM
	//
	// PICK FROM BM
	// PLACE TO CM
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	GetBMPickOnly = FALSE; // 2014.10.04
	//
	FingerCount = 0;
	FM_Buffer = -1;
	if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) FingerCount++;
	if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) FingerCount++;
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 08 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
	//----------------------------------------------------------------------
	if ( FingerCount > 0 ) {
		FingerCount = 0;
		if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) FingerCount++;
		if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) FingerCount++;
		if ( FingerCount > 0 ) {
			//
			FM_Buffer = _SCH_MODULE_Get_FM_MidCount();
			//
			if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
					//----------------------------------------------------------------------
					if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) {
						Slotmode = 0;
					}
					else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
						Slotmode = 1;
					}
					else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
						Slotmode = 2;
					}
					GetCount = SCHEDULER_CONTROL_Chk_Place_BM_Possible_for_CM_Pick_FOR_6( CHECKING_SIDE , FM_Buffer , Slotmode , &GetSlot );
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Pointer=%d][FM_Pointer2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Pointer , FM_Pointer2 , GetCount );
					//----------------------------------------------------------------------
					if ( GetCount > 0 ) {
						FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2003.06.10
						FM_Side2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2003.06.10
						//
						FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
						FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
						//
						FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
						FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
						switch( Slotmode ) {
						case 0 : // Arm AB
							FM_TSlot = GetCount;
//								FM_TSlot2 = FM_TSlot + 1; // 2004.11.25
							FM_TSlot2 = GetSlot; // 2004.11.25
							break;
						case 1 : // Arm A
							FM_TSlot = GetCount;
							FM_TSlot2 = 0;
							break;
						case 2 : // Arm B
							FM_TSlot = 0;
							FM_TSlot2 = GetCount;
							break;
						}
						//==========================================================================================
						// Code for Place to BI
						// FM_Slot
						// FM_Slot2
						//==========================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) { // 2003.09.09
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
							return FALSE;
						}
						//==========================================================================================
						runing_action = TRUE; // 2007.05.03
						//=================================================================================================================================================
						// 2005.05.17
						//=================================================================================================================================================
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) && ( FM_Side == FM_Side2 ) && SCHEDULER_CONTROL_Chk_NEED_SEPARATE_PLACE_FOR_6( FM_Buffer ) ) { // 2005.05.17
							//=============================================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 120 , FM_Buffer , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSlot , FM_Side2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
							case -1 :
								return FALSE;
								break;
							}
							//=================================================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , 0 , 0 , 0 , 0 , FM_Buffer , FM_Slot2 , FM_TSlot , FM_Side2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
							//=================================================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 130 , FM_Buffer , FM_Slot , FM_TSlot2 , FM_Side , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 , -1 ) ) { // 2007.03.21
							case -1 :
								return FALSE;
								break;
							}
							//=================================================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot2 , FM_Side , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
							//=================================================================================================
						}
						else {
							//=============================================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 140 , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
							case -1 :
								return FALSE;
								break;
							}
							//=============================================================================================
							_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , FM_Buffer , FM_Slot , FM_TSlot , FM_Side , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , BUFFER_INWAIT_STATUS , BUFFER_INWAIT_STATUS ); // 2007.11.26
							//=================================================================================================
						}
						//=================================================================================================================================================
					}
				}
			}
		}
		else {
//			Sleep(1); // 2008.05.19
			return TRUE;
		}
	}
	else {
		GetBMPickOnly = TRUE; // 2014.10.04
	}
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 10 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
		return FALSE;
	}
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	FingerCount = 0;
	GetCount = 0;
	GetSlot = 0;
	FM_Buffer = -1;
	if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) FingerCount++;
	if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) FingerCount++;
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
	//----------------------------------------------------------------------
	if ( FingerCount <= 0 ) {
		if ( _SCH_MODULE_Get_FM_MidCount() == 0 ) {
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
			//----------------------------------------------------------------------
			if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) { // 2007.05.18
				FM_Side = -1;
				for ( k = BM1 ; k < TM ; k++ ) {
					//
					if ( _SCH_PRM_GET_MODULE_MODE( k - BM1 + TM ) ) {
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2013.11.22
							//
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
									GetCount = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_CYCLON_FOR_6( CHECKING_SIDE , k , &FM_TSlot , &FM_TSlot2 , &FM_CM , &FM_Side2 );
									if ( GetCount > 0 ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( k , BUFFER_INWAIT_STATUS ) > 0 ) {
											FM_Side = FM_Side2;
											FM_Buffer2 = k;
											FM_OSlot = FM_TSlot;
											FM_OSlot2 = FM_TSlot2;
											break;
										}
										else {
											if ( FM_Side == -1 ) {
												FM_Side = FM_Side2;
												FM_Buffer2 = k;
												FM_OSlot = FM_TSlot;
												FM_OSlot2 = FM_TSlot2;
												FM_CO1 = FM_CM;
											}
											else {
												if ( FM_CO1 > FM_CM ) {
													FM_Side = FM_Side2;
													FM_Buffer2 = k;
													FM_OSlot = FM_TSlot;
													FM_OSlot2 = FM_TSlot2;
													FM_CO1 = FM_CM;
												}
											}
										}
									}
								}
							}
						}
						//
						if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25
							//
							if ( _SCH_MODULE_GET_USE_ENABLE( k+2 , FALSE , -1 ) ) { // 2013.11.22
								//
								if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k+2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k+2 ) == BUFFER_FM_STATION ) ) {
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k+2 ) <= 0 ) {
										GetCount = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_CYCLON_FOR_6( CHECKING_SIDE , k+2 , &FM_TSlot , &FM_TSlot2 , &FM_CM , &FM_Side2 );
										if ( GetCount > 0 ) {
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT2_STYLE_6( k+2 , BUFFER_INWAIT_STATUS ) > 0 ) {
												FM_Side = FM_Side2;
												FM_Buffer2 = k+2;
												FM_OSlot = FM_TSlot;
												FM_OSlot2 = FM_TSlot2;
												break;
											}
											else {
												if ( FM_Side == -1 ) {
													FM_Side = FM_Side2;
													FM_Buffer2 = k+2;
													FM_OSlot = FM_TSlot;
													FM_OSlot2 = FM_TSlot2;
													FM_CO1 = FM_CM;
												}
												else {
													if ( FM_CO1 > FM_CM ) {
														FM_Side = FM_Side2;
														FM_Buffer2 = k+2;
														FM_OSlot = FM_TSlot;
														FM_OSlot2 = FM_TSlot2;
														FM_CO1 = FM_CM;
													}
												}
											}
										}
									}
								}
							}
							//
						}
						//
					}
				}
				//==========================================================
				if ( FM_Side != -1 ) {
					if ( FM_Side == CHECKING_SIDE ) {
						FM_Buffer = FM_Buffer2;
						GetSlot = FM_OSlot;
						GetSlot2 = FM_OSlot2;
						if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							/*
							//
							// 2014.09.16
							//
							if ( GetSlot2 > 0 ) {
								GetCount = 3;
							}
							else {
								GetCount = 1;
							}
							//
							*/
							//
							//
							// 2014.09.16
							//
							switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( FM_Buffer ) ) {
							case 1 :
								GetCount = 1;
								break;
							case 2 :
								GetCount = 2;
								break;
							default :
								if ( GetSlot2 > 0 ) {
									GetCount = 3;
								}
								else {
									GetCount = 1;
								}
								break;
							}
							//
						}
						else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							GetCount = 1;
						}
						else {
							GetCount = 2;
						}
					}
					else {
						FM_Buffer = -99;
					}
				}
				//==========================================================
			}
			else {
				//
				Get99 = FALSE; // 2014.10.04
				//
				for ( k = BM1 ; k < TM ; k++ ) { // 2004.02.25
					//
					if ( _SCH_PRM_GET_MODULE_MODE( k - BM1 + TM ) ) { // 2004.02.25
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) { // 2013.11.22
							//
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k ) <= 0 ) {
									GetCountB = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( CHECKING_SIDE , k , &GetSlotB , &GetSlot2B );
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a1 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d,%d,%d][%d,%d,%d][%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 , GetCountB , GetSlotB , GetSlot2B , Get99 , GetBMPickOnly );
			//----------------------------------------------------------------------
									if ( GetCountB == -99 ) { // the other side has out wafer
										//========================================================================================================
										// 2007.07.03
										//========================================================================================================
										if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
											GetCountB = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( -1 , k , &GetSlotB , &GetSlot2B );
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a2 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d,%d,%d][%d,%d,%d][%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 , GetCountB , GetSlotB , GetSlot2B , Get99 , GetBMPickOnly );
			//----------------------------------------------------------------------
											switch( GetCountB ) {
											case 1 : // A
											case 2 : // B
												if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( k , GetSlotB ) , _SCH_MODULE_Get_BM_POINTER( k , GetSlotB ) , -1 , -1 ) < 1 ) {
													GetCountB = 0;
												}
												else {
													GetCountB = -99;
												}
												break;
											case 3 : // AB
												if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( k , GetSlotB ) , _SCH_MODULE_Get_BM_POINTER( k , GetSlotB ) , _SCH_MODULE_Get_BM_SIDE( k , GetSlot2B ) , _SCH_MODULE_Get_BM_POINTER( k , GetSlot2B ) ) < 2 ) {
													GetCountB = 0;
												}
												else {
													GetCountB = -99;
												}
												break;
											default :
												GetCountB = -99;
												break;
											}
										}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a3 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d,%d,%d][%d,%d,%d][%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 , GetCountB , GetSlotB , GetSlot2B , Get99 , GetBMPickOnly );
			//----------------------------------------------------------------------
										//========================================================================================================
										if ( GetCountB == -99 ) { // the other side has out wafer
											Get99 = TRUE; // 2014.10.04
											continue; // 2014.10.04
//											FM_Buffer = -99; // 2014.10.04
//											break; // 2014.10.04
										}
										//========================================================================================================
									}
									//
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a4 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d,%d,%d][%d,%d,%d][%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 , GetCountB , GetSlotB , GetSlot2B , Get99 , GetBMPickOnly );
			//----------------------------------------------------------------------
									if ( GetCountB > 0 ) { // 2014.10.04
										if ( GetBMPickOnly ) {
											//
											if ( !SCHEDULER_CONTROL_Chk_BM_has_no_place_space_from_CM_FOR_6( k ) ) { // 2015.02.27
												if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( k , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) {
													GetCountB = 0;
												}
											}
											//
										}
									}
									//
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a5 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d,%d,%d][%d,%d,%d][%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 , GetCountB , GetSlotB , GetSlot2B , Get99 , GetBMPickOnly );
			//----------------------------------------------------------------------
									if ( GetCountB > 0 ) {
										//
//										FM_Buffer = k; // 2014.10.03
										//
										// 2014.10.03
										//
										if ( FM_Buffer >= BM1 ) {
											if ( SCHEDULING_CHECK_BM_HAS_OUT_SUPPLYID( k , FM_Buffer ) ) {
												FM_Buffer = k;
												GetCount  = GetCountB;
												GetSlot   = GetSlotB;
	//											GetSlot2B = GetSlot2B; // 2015.02.27
												GetSlot2  = GetSlot2B; // 2015.02.27
											}
										}
										else {
											FM_Buffer = k;
											GetCount  = GetCountB;
											GetSlot   = GetSlotB;
//											GetSlot2B = GetSlot2B; // 2015.02.27
											GetSlot2  = GetSlot2B; // 2015.02.27
										}

//										break; // 2014.10.03
									}
								}
							}
						}
						//
//						if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() == 2 ) { // 2013.01.25 // 2014.07.08
						//
						if ( !SCHEDULING_ThisIs_BM_OtherChamber6( k+2 , 0 ) ) { // 2014.07.08

							if ( _SCH_MODULE_GET_USE_ENABLE( k+2 , FALSE , -1 ) ) { // 2013.11.22
								//
								if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k+2 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k+2 ) == BUFFER_FM_STATION ) ) {
									if ( _SCH_MACRO_CHECK_PROCESSING_INFO( k+2 ) <= 0 ) {
										GetCountB = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( CHECKING_SIDE , k+2 , &GetSlotB , &GetSlot2B );
										if ( GetCountB == -99 ) { // the other side has out wafer
											//========================================================================================================
											// 2007.07.03
											//========================================================================================================
											if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
												GetCountB = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( -1 , k+2 , &GetSlotB , &GetSlot2B );
												switch( GetCountB ) {
												case 1 : // A
												case 2 : // B
													if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( k+2 , GetSlotB ) , _SCH_MODULE_Get_BM_POINTER( k+2 , GetSlotB ) , -1 , -1 ) < 1 ) {
														GetCountB = 0;
													}
													else {
														GetCountB = -99;
													}
													break;
												case 3 : // AB
													if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( k+2 , GetSlotB ) , _SCH_MODULE_Get_BM_POINTER( k+2 , GetSlotB ) , _SCH_MODULE_Get_BM_SIDE( k+2 , GetSlot2B ) , _SCH_MODULE_Get_BM_POINTER( k+2 , GetSlot2B ) ) < 2 ) {
														GetCountB = 0;
													}
													else {
														GetCountB = -99;
													}
													break;
												default :
													GetCountB = -99;
													break;
												}
											}
											//========================================================================================================
											if ( GetCountB == -99 ) { // the other side has out wafer
												Get99 = TRUE; // 2014.10.04
												continue; // 2014.10.04
//												FM_Buffer = -99; // 2014.10.04
//												break; // 2014.10.04
											}
											//========================================================================================================
										}
										//
										if ( GetCountB > 0 ) { // 2014.10.04
											if ( GetBMPickOnly ) {
												//
												if ( !SCHEDULER_CONTROL_Chk_BM_has_no_place_space_from_CM_FOR_6( k+2 ) ) { // 2015.02.27
													if ( !SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( k+2 , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) {
														GetCountB = 0;
													}
												}
											}
										}
										//
										if ( GetCountB > 0 ) {
											//
//											FM_Buffer = k+2; // 2014.10.03
											//
											// 2014.10.03
											//
											if ( FM_Buffer >= BM1 ) {
												if ( SCHEDULING_CHECK_BM_HAS_OUT_SUPPLYID( k+2 , FM_Buffer ) ) {
													FM_Buffer = k+2;
													GetCount  = GetCountB;
													GetSlot   = GetSlotB;
//													GetSlot2B = GetSlot2B; // 2015.02.27
													GetSlot2  = GetSlot2B; // 2015.02.27
												}
											}
											else {
												FM_Buffer = k+2;
												GetCount  = GetCountB;
												GetSlot   = GetSlotB;
//												GetSlot2B = GetSlot2B;
												GetSlot2  = GetSlot2B; // 2015.02.27
											}

//											break; // 2014.10.03
										}
									}
								}
								//
							}
						}
						//
					}
				}
				//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11a6 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d,%d,%d][%d,%d,%d][%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 , GetCountB , GetSlotB , GetSlot2B , Get99 , GetBMPickOnly );
//----------------------------------------------------------------------
				if ( Get99 ) { // 2014.10.04
					if ( FM_Buffer == -1 ) {
						FM_Buffer = -99;
					}
				}
				//
			}
		}
		else {
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
			//----------------------------------------------------------------------
			FM_Buffer = _SCH_MODULE_Get_FM_MidCount();
			if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_FM_STATION ) ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) <= 0 ) {
					GetCount = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( CHECKING_SIDE , FM_Buffer , &GetSlot , &GetSlot2 );
					if ( GetCount == -99 ) {
						//========================================================================================================
						// 2007.07.03
						//========================================================================================================
						if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
							GetCount = SCHEDULER_CONTROL_Chk_FM_BMOUT_COUNT_FOR_6( -1 , FM_Buffer , &GetSlot , &GetSlot2 );
							switch( GetCount ) {
							case 1 : // A
							case 2 : // B
								if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , GetSlot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , GetSlot ) , -1 , -1 ) < 1 ) {
									GetCount = 0;
									FM_Buffer = -1; // 2007.10.29
								}
								else {
									GetCount = -99;
								}
								break;
							case 3 : // AB
								if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , GetSlot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , GetSlot ) , _SCH_MODULE_Get_BM_SIDE( FM_Buffer , GetSlot2 ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , GetSlot2 ) ) < 2 ) {
									GetCount = 0;
									FM_Buffer = -1; // 2007.10.29
								}
								else {
									GetCount = -99;
								}
								break;
							default :
								GetCount = -99;
								break;
							}
						}
						//========================================================================================================
						if ( GetCount == -99 ) FM_Buffer = -99;
						//========================================================================================================
					}
				}
			}
			else {
				FM_Buffer = -1;
			}
		}
		//====================================================================================================================
		//
		if ( ( GetBMPickOnly ) && ( FM_Buffer <= 0 ) ) { // 2014.10.04
		}
		else {
		}
		//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
		if ( FM_Buffer == -99 ) {
			//
			runing_action = FALSE; // 2007.05.03
			//
//			Sleep(1); // 2004.05.17 // 2008.05.19
			return TRUE;
		}
		else {

//=============================================================================================
// 2007.07.02
//=============================================================================================
pickfromficpart :	// 2007.07.02
//=============================================================================================
			FM_crs_act = 0; // 2009.03.10
//	
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				if ( ( FM_Buffer != -1 ) && ( FM_Buffer != F_IC ) ) {
					//========================================================================================================================================
					// 2007.07.03
					//========================================================================================================================================
					switch( GetCount ) {
					case 1 : // A
					case 2 : // B
						if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , GetSlot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , GetSlot ) , -1 , -1 ) < 1 ) FM_Buffer = -1;
						break;
					case 3 : // AB
						if ( _SCH_MODULE_Chk_MFIC_FREE2_COUNT( _SCH_MODULE_Get_BM_SIDE( FM_Buffer , GetSlot ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , GetSlot ) , _SCH_MODULE_Get_BM_SIDE( FM_Buffer , GetSlot2 ) , _SCH_MODULE_Get_BM_POINTER( FM_Buffer , GetSlot2 ) ) < 2 ) FM_Buffer = -1;
						break;
					}
					//========================================================================================================================================
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
				//----------------------------------------------------------------------
				if ( FM_Buffer == -1 ) {
					//========================================================================================================================================
					// 2007.07.02
					//========================================================================================================================================
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						FM_Side = CHECKING_SIDE;
//							ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( FM_Side , &FM_Side2 , &ICsts , &ICsts2 ); // 2007.11.06
						ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &FM_Side2 , &ICsts , &ICsts2 ); // 2007.11.06
						if ( ALsts == SYS_ABORTED ) {
							_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
							return FALSE;
						}
						else if ( ALsts == SYS_SUCCESS ) {
							FM_Buffer = F_IC;
							if ( ( ICsts > 0 ) && ( ICsts2 > 0 ) ) {
								GetCount = 3;
							}
							else {
								GetCount = 1;
							}
						}
					}
					//========================================================================================================================================
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
			//----------------------------------------------------------------------
			if ( FM_Buffer != -1 ) {
				//
				SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( FM_Buffer ); // 2010.07.28
				//
				if ( GetCount > 0 ) {
// 2007.07.02
					if ( FM_Buffer == F_IC ) {
						//========================================================================================================================================
						// 2007.07.02
						//========================================================================================================================================
						retdata = 0;
						//========================================================================================================================================
					}
					else {
						switch( GetCount ) {
						case 1 : // A
							FM_Slot  = GetSlot;
							FM_Slot2 = 0;
							break;
						case 2 : // B
							FM_Slot  = 0;
							FM_Slot2 = GetSlot;
							break;
						case 3 : // AB
							FM_Slot  = GetSlot;
							FM_Slot2 = GetSlot2; // 2004.11.25
							break;
						}
						//====================================================
						if ( FM_Slot > 0 ) {
							FM_Side    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot );
							FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
							FM_TSlot   = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot );
						}
						else {
							FM_Side = 0;
							FM_Pointer = 0;
							FM_TSlot = 0;
						}
						//====================================================
						if ( FM_Slot2 > 0 ) {
							FM_Side2    = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 );
							FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 );
							FM_TSlot2   = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot2 );
						}
						else {
							FM_Side2 = 0;
							FM_Pointer2 = 0;
							FM_TSlot2 = 0;
						}
						//====================================================
						if      ( FM_Slot > 0 ) {
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
						}
						else if ( FM_Slot2 > 0 ) {
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 );
						}
						else {
							FM_CM = 0;
						}
						//====================================================
						//==========================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) { // 2003.09.09
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
							return FALSE;
						}
						//==========================================================================
						// 2007.07.02
						//==========================================================================
						if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
							runing_action = FALSE;
//							Sleep(1); // 2008.05.19
							return TRUE;
						}

						//==========================================================================
						// 2009.03.04
						//==========================================================================
						if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2008.02.20
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								if ( ( _SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN ) || ( SCHEDULER_COOLING_SKIP_AL6( FM_Side , FM_Pointer ) ) ) {
									if ( ( _SCH_CLUSTER_Get_PathDo( FM_Side2 , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) || ( SCHEDULER_COOLING_SKIP_AL6( FM_Side2 , FM_Pointer2 ) ) ) {
										// OK
									}
									else {
										FM_crs_act = ( FM_Buffer * 100 ) + FM_Slot2; // 2009.03.10
										GetCount = 1;
										FM_TSlot2 = 0;
										FM_Slot2 = 0;
									}
								}
								else {
									if ( ( _SCH_CLUSTER_Get_PathDo( FM_Side2 , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) || ( SCHEDULER_COOLING_SKIP_AL6( FM_Side2 , FM_Pointer2 ) ) ) {
										FM_crs_act = ( FM_Buffer * 100 ) + FM_Slot2; // 2009.03.10
										GetCount = 1;
										FM_TSlot2 = 0;
										FM_Slot2 = 0;
									}
									else {
										// OK
									}
								}
							}
						}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12b-2 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
						//==========================================================================
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 150 , FM_Buffer , FM_TSlot , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
						case -1 :
							return FALSE;
							break;
						}
						//==========================================================================
						_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_TSlot , FM_Slot , FM_Side , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
						//==========================================================================
						//==========================================================================
						//==========================================================================
						// 2004.02.10
						//==========================================================================
						//==========================================================================
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 4 from FEM%cWHFMFAIL 4\n" , 9 );
							return FALSE;
						}
						//-----------------------------------------------
						_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 22 );
						//-----------------------------------------------
						retdata = 0; // 2007.05.02
						//-------------------------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_exist_run_wafer_at_TMPMBMIN_FOR_6( FM_Buffer , TRUE ) ) { // 2003.03.27
							//
							if ( SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( FM_Buffer ) ) {
								//
								if ( SCHEDULER_CONTROL_Chk_BM_has_no_place_space_from_CM_FOR_6( FM_Buffer ) ) { // 2003.06.05
									//
									SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_NORMALFULL( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , CHECKING_SIDE , SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( FM_Buffer , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , FALSE ) ); // 2013.01.25
									//
									SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , FM_Buffer ); // 2013.11.26
									//
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 2 );
									//
								}
								else {
									//
									if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( FM_Buffer , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) { // 2003.06.05
										//
										switch ( SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( FM_Buffer , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) ) { // 2008.01.16
										case -1 :
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling PM Fail at %s%cWHBM%dFAIL 1%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 , 9 );
											return FALSE;
											break;
										case 1 :
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12b-3 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
											//===============================================================================================
											//
											if ( !SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( FM_Buffer ) ) { // 2010.07.27
												//
												Dummy_count = 0;
												//
												if ( SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( FM_Buffer , &Dummy_Max , &Dummy_S , &Dummy_P ) > 0 ) { // 2013.01.25
													//
													Dummy_Result = SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_6( CHECKING_SIDE , FM_Buffer , TRUE , 3 , Dummy_Max , Dummy_S , Dummy_P , &Dummy_count );
													if ( Dummy_Result < 0 ) {
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Dummy Fail(%d) at %s%cWHBM%dFAIL 2%c\n" , Dummy_Result , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 , 9 );
														return FALSE;
													}
													else if ( Dummy_Result == 0 ) {
														retdata = 1;
													}
												}
												//
												SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , &Dummy_count , -1 , -1 );
												//
												SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , FM_Buffer ); // 2013.11.26
												//
												_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 3 );
											}
											break;
										}
									}
								}
							}
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12c RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata , FM_crs_act );
					//----------------------------------------------------------------------
					//-----------------------------------------------------------------------------------------
					//-----------------------------------------------------------------------------------------
					if ( retdata == 0 ) { // 2007.05.02
						//
						coolplace = 0;
						//
						if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2008.02.20
							//
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
								if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) coolplace = 2;
								if ( SCHEDULER_COOLING_SKIP_AL6( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) coolplace = 2;
							}
							//
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
								if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) coolplace = 2;
								if ( SCHEDULER_COOLING_SKIP_AL6( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) coolplace = 2;
							}
						}
						else {
							coolplace = 1;
						}
						//
						if ( coolplace != 0 ) {
							//-----------------------------------------------------------------------------------------
							//-----------------------------------------------------------------------------------------
							// 2004.12.01
							//-----------------------------------------------------------------------------------------
							//-----------------------------------------------------------------------------------------
							if ( coolplace == 1 ) {
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , IC , FM_TSlot , FM_TSlot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , FM_Slot2 * 100 + FM_Slot );
									return FALSE;
								}
							}
							else { // 2008.05.21
								FM_Buffer = F_IC; // 2008.05.21
							} // 2008.05.21
							//
							if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 5 from FEM%cWHFMFAIL 5\n" , 9 );
								return FALSE;
							}
							//-----------------------------------------------------------------------------------------
							//-----------------------------------------------------------------------------------------
						}
						else {
							//==================================================================================================================
							// 2007.07.02
							//==================================================================================================================
							if ( FM_Buffer != F_IC ) {
								//==================================================================================================================
								if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
									FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_Side2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_Pointer2  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								}
								else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									FM_Side = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_Side2 = -1;
								}
								else {
									FM_Side = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FM_Side2 = -1;
								}
								//==================================================================================================================
								ALsts = _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( FM_Side , FM_Pointer , FM_Side2 , FM_Pointer2 , &ICsts , &ICsts2 , 0 );
								if ( ALsts <= 0 ) {
									_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail 2 at IC%cWHFMICFAIL%c\n" , 9 , 9 );
									return FALSE;
								}
								//==================================================================================================================
								if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
									if ( ALsts >= 2 ) {
										ICfinger = 0;
									}
									else {
										ICfinger = 1;
									}
								}
								else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									ICfinger = 1;
								}
								else {
									ICfinger = 2;
								}
								//==================================================================================================================
								if      ( ICfinger == 0 ) {
									FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									//
									FM_Side2    = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								}
								else if ( ICfinger == 1 ) {
									FM_Side     = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_TSlot2   = 0;
								}
								else if ( ICfinger == 2 ) {
									FM_Side     = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FM_TSlot2   = 0;
								}
								FM_CM = _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
								//==================================================================================================================
								if ( ICfinger == 0 ) {
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_ALL , CHECKING_SIDE , IC , ICsts , ICsts2 , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot2 , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
										return FALSE;
									}
									//==========================================================================
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , FM_TSlot , ICsts , FM_Side , FM_Pointer , IC , FM_TSlot2 , ICsts2 , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
									//==========================================================================
									//============================================================================
									_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ICsts , 0 , FM_TSlot , 0 , FM_CM );
									//============================================================================
									_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side2 , ICsts2 , 0 , FM_TSlot2 , 0 , FM_CM );
									//============================================================================
								}
								else {
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_Side , IC , ( ICfinger == 1 ) ? ICsts : 0 , ( ICfinger == 1 ) ? 0 : ICsts , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot , FM_Side , FM_Side , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
										return FALSE;
									}
									//==========================================================================
									_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PLACE , IC , ( ICfinger == 1 ) ? FM_TSlot : 0 , ICsts , FM_Side , FM_Pointer , IC , ( ICfinger == 1 ) ? 0 : FM_TSlot , ICsts , FM_Side , FM_Pointer , -1 , -1 ); // 2007.11.26
									//==========================================================================
									//============================================================================
									_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ( ICfinger == 1 ) ? ICsts : 0 , ( ICfinger == 1 ) ? 0 : ICsts , ( ICfinger == 1 ) ? FM_TSlot : 0 , ( ICfinger == 1 ) ? 0 : FM_TSlot , FM_CM );
									//============================================================================
								}
								//==================================================================================================================
								// 2007.07.02
								//=============================================================================================
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12i-3 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata , FM_crs_act );
								//----------------------------------------------------------------------
								if ( FM_crs_act != 0 ) { // 2009.03.10
									GetCount = 2;
									FM_Buffer = FM_crs_act / 100;
									GetSlot = FM_crs_act % 100;
									FM_crs_act = 0;
								}
								else {
									FM_Buffer = -1;
									_SCH_MODULE_Set_FM_MidCount( 0 ); // Reset for Next Use
								}
								goto pickfromficpart;	// 2007.07.02
								//=============================================================================================
							}
							else {
								FM_Side = CHECKING_SIDE;
//								ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( FM_Side , &FM_Side2 , &ICsts , &ICsts2 ); // 2007.11.06
								ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( -1 , &FM_Side2 , &ICsts , &ICsts2 ); // 2007.11.06
								if ( ALsts == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
									return FALSE;
								}
								else if ( ALsts == SYS_SUCCESS ) {
									FM_Buffer = F_IC;
									if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( ICsts > 0 ) && ( ICsts2 > 0 ) ) {
										//
										FM_Side  = _SCH_MODULE_Get_MFIC_SIDE( ICsts ); // 2007.11.06
										FM_Side2 = _SCH_MODULE_Get_MFIC_SIDE( ICsts2 ); // 2007.11.06
										//
										_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Success at IC(%d:%d)%cWHFMICSUCCESS %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
										FM_CM = _SCH_CLUSTER_Get_PathOut( FM_Side , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) );
										//
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , FM_Side , IC , ICsts , ICsts2 , FM_CM , -1 , TRUE , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , _SCH_MODULE_Get_MFIC_SIDE( ICsts ) , _SCH_MODULE_Get_MFIC_SIDE( ICsts2 ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( FM_Side , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
											return FALSE;
										}
										//==========================================================================
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , ICsts , FM_Side , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , IC , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , ICsts2 , FM_Side2 , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) , -1 , -1 ); // 2007.11.26
										//==========================================================================
										//====================================================================================
										FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										//====================================================================================
										GetCount = 3;
										//====================================================================================
									}
									//===============================================================================================
									else {
										if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
												ICfinger = TA_STATION;
											}
											else {
												ICfinger = TB_STATION;
											}
										}
										else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
											ICfinger = TA_STATION;
										}
										else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
											ICfinger = TB_STATION;
										}
										if ( ICfinger == TA_STATION ) {
											FM_Side     = _SCH_MODULE_Get_MFIC_SIDE( ICsts ); // 2007.11.06
											FM_TSlot	= _SCH_MODULE_Get_MFIC_WAFER( ICsts );
											FM_Pointer	= _SCH_MODULE_Get_MFIC_POINTER( ICsts );
											//
											FM_Pointer2	= 0;
											FM_TSlot2	= 0;
											//
											FM_CM		= _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
											//====================================================================================
											GetCount = 1;
											//====================================================================================
										}
										else {
//												FM_Side2	= FM_Side; // 2007.11.06
											FM_Side2    = _SCH_MODULE_Get_MFIC_SIDE( ICsts ); // 2007.11.06
											FM_TSlot2	= _SCH_MODULE_Get_MFIC_WAFER( ICsts );
											FM_Pointer2	= _SCH_MODULE_Get_MFIC_POINTER( ICsts );
											//
											FM_Pointer	= 0;
											FM_TSlot	= 0;
											FM_CM		= _SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 );
											//====================================================================================
											GetCount = 2;
											//====================================================================================
										}
										//---------------------------------------------------------------------------------------
										if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , ( FM_TSlot > 0 ? FM_Side : FM_Side2 ) , IC , ( FM_TSlot > 0 ? ICsts : 0 ) , ( FM_TSlot2 > 0 ? ICsts : 0 ) , FM_CM , -1 , TRUE , ( FM_TSlot > 0 ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FM_TSlot2 > 0 ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FM_TSlot > 0 ? _SCH_MODULE_Get_MFIC_SIDE( ICsts ) : 0 ) , ( FM_TSlot2 > 0 ? _SCH_MODULE_Get_MFIC_SIDE( ICsts ) : 0 ) , ( FM_TSlot > 0 ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) , ( FM_TSlot2 > 0 ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) ) == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( ( FM_TSlot > 0 ? FM_Side : FM_Side2 ) , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
											return FALSE;
										}
										//==========================================================================
										_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK , IC , ( ICfinger == TA_STATION ) ? FM_TSlot : 0 , ICsts , FM_Side , FM_Pointer , IC , ( ICfinger == TA_STATION ) ? 0 : FM_TSlot2 , ICsts , FM_Side2 , FM_Pointer2 , -1 , -1 ); // 2007.11.26
										//==========================================================================
									}
								}
							}
							//==================================================================================================================
						}
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12d RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata );
						//----------------------------------------------------------------------
						//==============================================================================================
						//==============================================================================================
						//==============================================================================================
						if ( ( FM_Buffer != -1 ) && ( ( FM_Buffer == F_IC ) || ( !_SCH_MODULE_Need_Do_Multi_FIC() ) ) ) {
							//===============================================================================================================
							// 2006.03.22
							//===============================================================================================================
							retdata = 0;
							//===============================================================================================================
							switch( GetCount ) {
							case 1 : // A
								if ( _SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN ) { // 2005.01.26
									FM_OSlot  = _SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer );
									FM_OSlot2 = 0;
									//
									FM_CO1 = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
									FM_CO2 = 0;
									//----------------------------------------------
									retdata = 2; // 2006.03.22
									//----------------------------------------------
								}
								else {
									FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer );
									FM_OSlot2 = 0;
									//
									FM_CO1 = _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
									FM_CO2 = 0;
								}
								//====================================================================================
								break;
							case 2 : // B
								if ( _SCH_CLUSTER_Get_PathDo( FM_Side2 , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) { // 2005.01.26
									FM_OSlot  = 0;
									FM_OSlot2 = _SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 );
									//
									FM_CO1 = 0;
									FM_CO2 = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 );
									//----------------------------------------------
									retdata = 2; // 2006.03.22
									//----------------------------------------------
								}
								else {
									FM_OSlot  = 0;
									FM_OSlot2 = _SCH_CLUSTER_Get_SlotOut( FM_Side2 , FM_Pointer2 );
									//
									FM_CO1 = 0;
									FM_CO2 = _SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 );
								}
								//====================================================================================
								break;
							case 3 : // AB
								if ( _SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN ) { // 2005.01.26
									FM_OSlot  = _SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer );
									//
									FM_CO1 = _SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer );
									//----------------------------------------------
									retdata = 2; // 2006.03.22
									//----------------------------------------------
								}
								else {
									FM_OSlot  = _SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer );
									//
									FM_CO1 = _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
								}
								//
								if ( _SCH_CLUSTER_Get_PathDo( FM_Side2 , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) { // 2005.01.26
									FM_OSlot2 = _SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 );
									//
									FM_CO2 = _SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 );
									//----------------------------------------------
									retdata = 2; // 2006.03.22
									//----------------------------------------------
								}
								else {
									FM_OSlot2 = _SCH_CLUSTER_Get_SlotOut( FM_Side2 , FM_Pointer2 );
									//
									FM_CO2 = _SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 );
								}
								break;
							}
							//
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12e RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata );
							//----------------------------------------------------------------------
							//=============================================================================================
							if ( FM_TSlot  > 0 ) SCH_CYCLON2_GROUP_RETURN( FM_Side , FM_Pointer ); // 2013.02.15
							if ( FM_TSlot2 > 0 ) SCH_CYCLON2_GROUP_RETURN( FM_Side2 , FM_Pointer2 ); // 2013.02.15
							//=============================================================================================
							switch( _SCH_MACRO_FM_PLACE_TO_CM( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FALSE , FALSE , 0 ) ) {
							case -1 :
								return FALSE;
								break;
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12g RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata );
							//----------------------------------------------------------------------
							//==================================================================================================================
							// 2007.07.05
							//==================================================================================================================
//								if ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() < 0 ) { // 2007.08.27
//								if ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() < -1 ) { // 2007.08.27 // 2007.09.18
							if ( _SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() == -2 ) { // 2007.08.27 // 2007.09.18
								FM_Buffer = -1;
								_SCH_MODULE_Set_FM_MidCount( 0 ); // Reset for Next Use
								goto pickfromficpart;
							}
							//==================================================================================================================
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12i RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata );
							//----------------------------------------------------------------------
						} // 2007.05.02
					}
					//=============================================
					// 2009.03.10
					//=============================================
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12i-2 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata , FM_crs_act );
					//----------------------------------------------------------------------
					if ( FM_crs_act != 0 ) {
						GetCount = 2;
						FM_Buffer = FM_crs_act / 100;
						GetSlot = FM_crs_act % 100;
						FM_crs_act = 0;
						goto pickfromficpart;
					}
					//=============================================
				}
				else {
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata );
					//----------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_Chk_BM_has_no_place_space_from_CM_FOR_6( FM_Buffer ) ) {
						//
						if ( SCHEDULER_CONTROL_Chk_no_more_getout_wafer_from_BM_FOR_6( FM_Buffer ) ) { // 2007.07.04
							//
							SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_NORMALFULL( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , CHECKING_SIDE , SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( FM_Buffer , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , FALSE ) ); // 2013.01.25
							//
							SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , FM_Buffer ); // 2013.11.26
							//
							_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 4 );
							//
						}
					}
					else {
						//
						if ( SCHEDULER_CONTROL_Chk_no_more_supply_wafer_from_CM_FOR_6( FM_Buffer , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 , FALSE , TRUE ) ) { // 2003.03.28
							//
							switch ( SCHEDULER_CONTROL_Chk_BM_to_PM_Process_Fail_FOR_6( FM_Buffer , SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) ) { // 2008.01.16
							case -1 :
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling PM Fail at %s%cWHBM%dFAIL 1%c\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 , 9 );
								return FALSE;
								break;
							case 1 :
								//===============================================================================================
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-2 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
								if ( !SCHEDULER_CONTROL_Chk_exist_return_Wafer_in_BM_FOR_6( FM_Buffer ) ) { // 2010.07.27
									//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-3 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
									Dummy_count = 0;
									//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-4 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
									if ( SCH_CYCLON2_GROUP_SUPPLY_DUMMY_POSSIBLE( FM_Buffer , &Dummy_Max , &Dummy_S , &Dummy_P ) > 0 ) { // 2013.01.25
										//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-5 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
										Dummy_Result = SCHEDULER_CONTROL_DUMMY_OPERATION_BEFORE_PUMPING_for_6( CHECKING_SIDE , FM_Buffer , FALSE , 4 , Dummy_Max , Dummy_S , Dummy_P , &Dummy_count );
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-6 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
										if ( Dummy_Result < 0 ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Dummy Fail(%d) at %s%cWHBM%dFAIL 3%c\n" , Dummy_Result , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 , 9 );
											return FALSE;
										}
									}
									//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-7 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
									SCH_CYCLON2_GROUP_MAKE_TMSIDE_AFTER_DUMMY( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , &Dummy_count , -1 , -1 );
									//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-8 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
									SCH_CYCLON2_WAFER_ORDERING_REMAP_BEFORE_PUMPING( _SCH_PRM_GET_MODULE_GROUP( FM_Buffer ) , FM_Buffer ); // 2013.11.26
									//
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12j-9 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][GetSlot=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , GetSlot , GetSlot2 );
		//----------------------------------------------------------------------
									_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , FM_Buffer , -1 , TRUE , 0 , 5 );
								}
								break;
							}
						}
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12k RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d][retdata=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount , retdata );
				//----------------------------------------------------------------------
				_SCH_MODULE_Set_FM_MidCount( 0 ); // Reset for Next Use
				//==============================================================================
				// 2007.05.03
				//==============================================================================
				if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) { // 2007.05.03
					if ( !runing_action ) {
//						Sleep(1); // 2004.05.17 // 2008.05.19
						return TRUE;
					}
					else {
						return -1;
					}
				}
				else {
//					Sleep(1); // 2004.05.17 // 2008.05.19
					return TRUE;
				}
			}
		}
	}
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 25 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][next_BM=%d][Slotmode=%d][FM_Side=%d][FM_Side2=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][GetCount=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , next_BM , Slotmode , FM_Side , FM_Side2 , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , GetCount );
	//----------------------------------------------------------------------
	//==============================================================================
	// 2007.05.03
	//==============================================================================
	if ( SCHEDULER_CONTROL_Chk_CYCLON_MODE_FOR_6() > 0 ) { // 2007.05.03
		if ( !runing_action ) {
//			Sleep(10); // 2004.05.17 // 2008.05.19
			return TRUE;
		}
		else {
//			Sleep(10); // 2004.05.17 // 2008.05.19
			return -1;
		}
	}
	else {
//		Sleep(10); // 2004.05.17 // 2008.05.19
		return TRUE;
	}
	//==============================================================================
	//Sleep(1); // 2004.05.14
	Sleep(1); // 2004.05.14
	//==============================================================================
	return TRUE;
}

