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
#include "INF_sch_FMArm_multi.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_prepost.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_sdm.h"
#include "INF_sch_preprcs.h"
#include "INF_Multireg.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
//#include "INF_sch_CommonUser.h"
//================================================================================
#include "sch_A4_default.h"
#include "sch_A4_init.h"
#include "sch_A4_param.h"
#include "sch_A4_sub.h"
#include "sch_A4_sdm.h"
#include "sch_A4_subBM.h"
//================================================================================


BOOL SCHEDULER_COOLING_SKIP_AL4( int side , int pt ) { // 2014.08.26
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) == 2 ) return TRUE;
	if ( _SCH_CLUSTER_Get_Extra_Flag( side , pt , 1 ) == 6 ) return TRUE; // 2014.08.26
	return FALSE;
}

#define   PICK_FROM_BM_FM_MODE_4( SIDE , PNT )	_SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_CLUSTER_Get_PathDo( SIDE , PNT ) != PATHDO_WAFER_RETURN ) && ( !SCHEDULER_COOLING_SKIP_AL4( SIDE , PNT ) ) ? FMWFR_PICK_BM_NEED_IC : FMWFR_PICK_BM_DONE_IC

//------------------------------------------------------------------------------------------
BOOL FM_Pick_Running_Blocking_Style_4; // 2006.08.02
BOOL FM_Place_Running_Blocking_Style_4; // 2006.08.02
//------------------------------------------------------------------------------------------
BOOL FM_Has_Swap_Wafer_Style_4; // 2007.09.03
//------------------------------------------------------------------------------------------
BOOL FM_Last_Move_Recv_BM_4; // 2010.04.16
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
// Include Start
//###################################################################################################

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
//
int sch4_FM_WAFER_3LASTPICKBM_TB; // 2007.05.10
//
//int sch4_FM_WAFER_SUPPLY[2]; // 2004.08.11 // 2007.09.12
//Pick CM = 0 // 2005.09.06
//Pick BM = 1 // 2005.09.06
//Change
//Pick CM           = 0 // 2005.09.06
//Pick AL(Place BM) = 1 // 2005.09.06
//Pick BM           = 2 // 2005.09.06
//Pick IC(Place CM) = 3 // 2005.09.06

//int sch4_LAST_PLACE_BM = -1; // 2006.11.23 // 2007.09.12

int ALIGN_BUFFER_CONTROL_RUN = 0; // 2007.04.26

//=======================================================================================
int sch4_t3_logdata[MAX_BM_CHAMBER_DEPTH];
//=======================================================================================

int SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();


void SCHEDULER_CONTROL_Chk_SEPARATE_SUPPLY_for_STYLE_4( BOOL fullmode , int bms , int side , int Slot1 , int Pointer1 , int Slot2 , int Pointer2 ) {
	int i , s;
	//
	if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 0 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 3 ) ) return;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
		//==================================================================================
		if ( ( Slot1 > 0 ) && ( Slot2 > 0 ) ) return;
		//==================================================================================
		if ( fullmode ) {
			if ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) return;
		}
		else {
			if ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() != 2 ) return;

//			if ( Slot2 > 0 ) return;
//			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return; // 2007.09.19
//			if ( ( Slot1 <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) return; // 2007.09.19
//			if ( ( Slot2 <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return; // 2007.09.19
//			if ( Slot2 > 0 ) return; // 2007.09.19
			//
			if ( Slot2 > 0 ) {
				if ( bms != BM1 ) {
					if ( _SCH_SYSTEM_LASTING_GET( side ) ) {
						for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
							if ( s != side ) {
								if ( _SCH_SUB_Simple_Supply_Possible( s ) ) break;
							}
						}
						if ( s != MAX_CASSETTE_SIDE ) return;
					}
				}
			}
			//
		}
		//==================================================================================
		if ( _SCH_SYSTEM_LASTING_GET( side ) ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				_SCH_CLUSTER_Dec_ClusterIndex( side , i );
			}
		}
		//==================================================================================
	}
	//
}

void SCHEDULER_SWAPPING_3_1_REMAP_FM_ONEBODY3( int *rs , int *rp , int *rw ) { // 2007.09.03
	int bs , bp , bw , bm;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) return;
	//
	if ( !SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) return;
	//
	bs = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
	bp = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
	bw = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
	bm = _SCH_MODULE_Get_FM_MODE( TB_STATION );

	_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_MODE( TA_STATION ) );
	_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );

	_SCH_FMARMMULTI_DATA_SHIFTING_A_AND_APPEND( bs , bp , bw , bw , bm , rs , rp , rw );
	//
}

void SCHEDULER_SWAPPING_3_1_REMAP_BM_ONEBODY3( int bmc , int slot ) { // 2007.09.03
	int i , bs1 , bp1 , bw1 , bt1 , bs4 , bp4 , bw4 , bt4;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) return;
	//
	if ( !SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) return;
	//
	if ( bmc < BM1 ) {
		bs1 = _SCH_MODULE_Get_BM_SIDE(    BM1 , slot );
		bp1 = _SCH_MODULE_Get_BM_POINTER( BM1 , slot );
		bw1 = _SCH_MODULE_Get_BM_WAFER(   BM1 , slot );
		bt1 = _SCH_MODULE_Get_BM_STATUS(  BM1 , slot );
		//
		bs4 = _SCH_MODULE_Get_BM_SIDE(    BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , slot );
		bp4 = _SCH_MODULE_Get_BM_POINTER( BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , slot );
		bw4 = _SCH_MODULE_Get_BM_WAFER(   BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , slot );
		bt4 = _SCH_MODULE_Get_BM_STATUS(  BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , slot );
		//
		_SCH_MODULE_Set_BM_SIDE_POINTER( BM1 , slot , bs4 , bp4 );
		_SCH_MODULE_Set_BM_WAFER_STATUS( BM1 , slot , bw4 , bt4 );
		//
		for ( i = SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i > 2 ; i-- ) {
			bs4 = _SCH_MODULE_Get_BM_SIDE(    BM1 + i - 2 , slot );
			bp4 = _SCH_MODULE_Get_BM_POINTER( BM1 + i - 2 , slot );
			bw4 = _SCH_MODULE_Get_BM_WAFER(   BM1 + i - 2 , slot );
			bt4 = _SCH_MODULE_Get_BM_STATUS(  BM1 + i - 2 , slot );
			//
			_SCH_MODULE_Set_BM_SIDE_POINTER( BM1 + i - 1 , slot , bs4 , bp4 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( BM1 + i - 1 , slot , bw4 , bt4 );
		}
		//
		_SCH_MODULE_Set_BM_SIDE_POINTER( BM1 + i - 1 , slot , bs1 , bp1 );
		_SCH_MODULE_Set_BM_WAFER_STATUS( BM1 + i - 1 , slot , bw1 , bt1 );
	}
	else {
		bs1 = _SCH_MODULE_Get_BM_SIDE(    bmc , slot );
		bp1 = _SCH_MODULE_Get_BM_POINTER( bmc , slot );
		bw1 = _SCH_MODULE_Get_BM_WAFER(   bmc , slot );
		bt1 = _SCH_MODULE_Get_BM_STATUS(  bmc , slot );
		//
		bs4 = _SCH_MODULE_Get_BM_SIDE(    bmc , slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
		bp4 = _SCH_MODULE_Get_BM_POINTER( bmc , slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
		bw4 = _SCH_MODULE_Get_BM_WAFER(   bmc , slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
		bt4 = _SCH_MODULE_Get_BM_STATUS(  bmc , slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
		//
		_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , slot , bs4 , bp4 );
		_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , slot , bw4 , bt4 );
		//
		for ( i = SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i > 2 ; i-- ) {
			bs4 = _SCH_MODULE_Get_BM_SIDE(    bmc , slot + i - 2 );
			bp4 = _SCH_MODULE_Get_BM_POINTER( bmc , slot + i - 2 );
			bw4 = _SCH_MODULE_Get_BM_WAFER(   bmc , slot + i - 2 );
			bt4 = _SCH_MODULE_Get_BM_STATUS(  bmc , slot + i - 2 );
			//
			_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , slot + i - 1 , bs4 , bp4 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , slot + i - 1 , bw4 , bt4 );
		}
		//
		_SCH_MODULE_Set_BM_SIDE_POINTER( bmc , slot + i - 1 , bs1 , bp1 );
		_SCH_MODULE_Set_BM_WAFER_STATUS( bmc , slot + i - 1 , bw1 , bt1 );
	}
}

void SCHEDULER_SWAPPING_3_1_REMAP_FIC_ONEBODY3( int slot ) { // 2007.09.03
	int i , bs1 , bp1 , bw1 , bs4 , bp4 , bw4;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) return;
	//
	if ( !SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) return;
	//
	bs1 = _SCH_MODULE_Get_MFIC_SIDE(    slot );
	bp1 = _SCH_MODULE_Get_MFIC_POINTER( slot );
	bw1 = _SCH_MODULE_Get_MFIC_WAFER(   slot );
	//
	bs4 = _SCH_MODULE_Get_MFIC_SIDE(    slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
	bp4 = _SCH_MODULE_Get_MFIC_POINTER( slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
	bw4 = _SCH_MODULE_Get_MFIC_WAFER(   slot + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
	//
	_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot , bs4 , bp4 );
	_SCH_MODULE_Set_MFIC_WAFER( slot , bw4 );
	//
	for ( i = SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i > 2 ; i-- ) {
		bs4 = _SCH_MODULE_Get_MFIC_SIDE(    slot + i - 2 );
		bp4 = _SCH_MODULE_Get_MFIC_POINTER( slot + i - 2 );
		bw4 = _SCH_MODULE_Get_MFIC_WAFER(   slot + i - 2 );
		//
		_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot + i - 1 , bs4 , bp4 );
		_SCH_MODULE_Set_MFIC_WAFER( slot + i - 1 , bw4 );
	}
	//
	_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot + i - 1 , bs1 , bp1 );
	_SCH_MODULE_Set_MFIC_WAFER( slot + i - 1 , bw1 );
}

//=======================================================================================
BOOL SCHEDULER_CM_PLACE_SEPARATE_CHECK_ONEBODY3( int cm1 , int side1 , int oslot1 , int cm2 , int side2 , int oslot2 ) { // 2007.08.31
	if ( oslot1 <= 0 ) return FALSE;
	if ( oslot2 <= 0 ) return FALSE;
	//
	if (
		( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) &&
		( side1 == side2 ) &&
		( cm1 == cm2 ) &&
		( ( oslot1 + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) != oslot2 )
		) {
		return TRUE;
	}
	return FALSE;
}
//=======================================================================================
int SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( int ch ) { // 2007.10.30
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ) {
		return 0;
	}
	else {
		return _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2;
	}
}

BOOL SCHEDULER_ALIGN_BUFFER_CONTROL_PICK_FM_POSSIBLE( int ch ); // 2007.04.10
int  SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( int ch , BOOL highmode , int side );
//=========================================================================================================
void SCHEDULER_ALIGN_BUFFER_SET_RUN( int rundata ) {
	ALIGN_BUFFER_CONTROL_RUN = rundata;
}

int SCHEDULER_ALIGN_BUFFER_GET_RUN() {
	return ALIGN_BUFFER_CONTROL_RUN;
}
//=======================================================================================
//=======================================================================================
BOOL SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( BOOL bmmode ) {
	if ( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == 0 ) return FALSE;
	if ( !_SCH_MODULE_GET_USE_ENABLE( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , FALSE , -1 ) ) return FALSE;
	if ( bmmode ) {
		if ( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == F_IC ) return FALSE;
	}
	return TRUE;
}
//=======================================================================================
//=======================================================================================
int SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() {
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		return ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 );
	}
	else {
		return ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
	}
}
//=========================================================================================================
void SCHEDULER_BM_PROCESS_MONITOR_ONEBODY( int bmcnt ) {
	int i;
	for ( i = 0 ; i < bmcnt  ; i++ ) {
		SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( BM1 + i );
	}
}
//=========================================================================================================

//==============================================================================================================
BOOL SCHEDULER_CONTROL_Get_Free_OneBody_3_FIC_Slot( int *slot , int *slot2 ) {
	int i , j , l , f , fs; // , ICsts , ICsts2;
	fs = SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();

/*
// 2007.04.11
	if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts , &ICsts2 , -1 , -1 ) >= fs ) {
		if ( ( ( ICsts - 1 ) % fs ) == 0 ) {
			*slot = ICsts;
			*slot2 = ICsts2;
			return TRUE;
		}
	}
*/
	//========================================================
	l = _SCH_PRM_GET_MODULE_SIZE(F_IC) / fs;
	//========================================================
	for ( i = 0 ; i < l ; i++ ) {
		f = TRUE;
		for ( j = ( i * fs ) ; j < ( ( i + 1 ) * fs ) ; j++ ) {
			if ( _SCH_MODULE_Get_MFIC_WAFER( j + 1 ) > 0 ) {
				f = FALSE;
				break;
			}
		}
		if ( f ) {
			*slot = ( i * fs ) + 1;
			*slot2 = ( i * fs ) + 2;
			return TRUE;
		}
	}
	//========================================================
	return FALSE;
}

BOOL SCHEDULER_CONTROL_OneBody_3_FIC_Full( BOOL all , int slot2 ) {
	int s1 , s2 , fs , fs2;
	if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
		if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) return TRUE;
		if ( all ) {
			if ( !SCHEDULER_CONTROL_Get_Free_OneBody_3_FIC_Slot( &s1 , &s2 ) ) return TRUE;
		}
		else {
			//========================================================
			if ( slot2 <= 0 ) return FALSE;
			//========================================================
			fs = SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
			//========================================================
			fs2 = ( slot2 - 1 ) % SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
			//========================================================
			s2 = _SCH_PRM_GET_MODULE_SIZE(F_IC) / fs;
			//========================================================
			for ( s1 = 0 ; s1 < s2 ; s1++ ) {
				if ( _SCH_MODULE_Get_MFIC_WAFER( ( fs * s1 ) + fs2 + 1 ) <= 0 ) {
					return FALSE;
				}
			}
			//========================================================
			return TRUE;
			//========================================================
		}
	}
	//========================================================
	return FALSE;
	//========================================================
}

int SCHEDULER_CONTROL_FM_3_1_All_Multi_Mode() { // 2007.08.31
	//==================================================================================
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) return FALSE;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) return FALSE;
	if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != _SCH_MODULE_Get_FM_SIDE( TB_STATION ) ) return FALSE;
	//==================================================================================
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) + ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) != _SCH_MODULE_Get_FM_WAFER( TB_STATION ) ) return FALSE;
	//==================================================================================
	return TRUE;
}


int SCHEDULER_CONTROL_Pick_BM_3_1_Swapping_Multi_Mode( int bm , int s ) { // 2007.08.22
	int i , l , side , Pointer , offset;
	int oside , oslot , oc , os , csl;
	//----------------------------------------------------------------------------
	// return 0 : B1 + A3
	// return 1 : A3 + B1
	// return 2 : Random
	//----------------------------------------------------------------------------
	for ( l = 0 ; l < 2 ; l++ ) {
		//
		if ( l == 0 ) offset = 1;
		else          offset = 0;
		//
		oside = -1;
		//
		for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ; i++ ) {
			//==============================================================================================
			if ( _SCH_MODULE_Get_BM_WAFER( bm + i + offset , s ) > 0 ) {
				//==============================================================================================
				side    = _SCH_MODULE_Get_BM_SIDE( bm + i + offset, s );
				Pointer = _SCH_MODULE_Get_BM_POINTER( bm + i + offset, s );
				//==============================================================================================
				if ( oside == -1 ) {
					os = i;
					oside = side;
					_SCH_SUB_GET_OUT_CM_AND_SLOT( side , Pointer , -1 , &oc , &oslot ); // 2007.07.11
					csl = oslot;
				}
				else {
					if ( oside != side ) break;
					if ( ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == 0 ) ) {
						if ( oc != _SCH_CLUSTER_Get_PathIn( side , Pointer ) ) break;
						csl = _SCH_CLUSTER_Get_SlotIn( side , Pointer );
						if ( ( oslot + i + os ) != csl ) break;
					}
					else {
						if ( oc != _SCH_CLUSTER_Get_PathOut( side , Pointer ) ) break;
						csl = _SCH_CLUSTER_Get_SlotOut( side , Pointer );
						if ( ( oslot + i + os ) != csl ) break;
					}
				}
				//==============================================================================================
			}
			//==============================================================================================
		}
		if ( ( oside != -1 ) && ( i == ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ) ) return l;
	}
	return 2;
}

int Scheduler_MFIC_Pick_Multi_Mode( int s ) {
	int i , l , side , Pointer;
	int oside , oslot , oc , os , csl;

	//============================================================================================
	// 2007.08.23 imsi code for Cooler swapping (1/4)
	//============================================================================================
/*
	int sep;
	sep = FALSE;
	for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ; i++ ) {
		if ( _SCH_MODULE_Get_MFIC_WAFER( s + i ) > 0 ) {
			if ( _SCH_CLUSTER_Get_SwitchInOut( _SCH_MODULE_Get_MFIC_SIDE( s + i ) , _SCH_MODULE_Get_MFIC_POINTER( s + i ) ) != 0 ) {
				sep = TRUE;
				break;
			}
		}
	}
*/

	if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) { // 2007.09.04

		//----------------------------------------------------------------------------
		// return 2 : B1 + A3
		// return 3 : A3 + B1
		// return 1 : A3
		// return 0 : B
		//----------------------------------------------------------------------------

		for ( l = 0 ; l < 2 ; l++ ) {
			//
			oside = -1;
			//
			for ( i = l ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 + 1 ) ; i++ ) {
				if ( _SCH_MODULE_Get_MFIC_WAFER( s + i ) > 0 ) {
					side    = _SCH_MODULE_Get_MFIC_SIDE( s + i );
					Pointer = _SCH_MODULE_Get_MFIC_POINTER( s + i );
					//==============================================================================================
					if ( _SCH_MACRO_CHECK_FM_MCOOLING( side , s + i , FALSE ) != SYS_SUCCESS ) return 0;
					//==============================================================================================
					if ( oside == -1 ) {
						os = ( i - l );
						oside = side;
						_SCH_SUB_GET_OUT_CM_AND_SLOT( side , Pointer , -1 , &oc , &oslot ); // 2007.07.11
						csl = oslot;
					}
					else {
						if ( oside != side ) break;
						if ( ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == 0 ) ) {
							if ( oc != _SCH_CLUSTER_Get_PathIn( side , Pointer ) ) break;
							csl = _SCH_CLUSTER_Get_SlotIn( side , Pointer );
							if ( ( oslot + ( i - l ) + os ) != csl ) break;
						}
						else {
							if ( oc != _SCH_CLUSTER_Get_PathOut( side , Pointer ) ) break;
							csl = _SCH_CLUSTER_Get_SlotOut( side , Pointer );
							if ( ( oslot + ( i - l ) + os ) != csl ) break;
						}
					}
				}
				else {
					break;
				}
			}
			//
			if ( ( oside != -1 ) && ( i == ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 + 1 ) ) ) return ( l + 2 );
		}
	}
	//============================================================================================
	oside = -1; // 2007.09.17
	//
	for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ; i++ ) {
		if ( _SCH_MODULE_Get_MFIC_WAFER( s + i ) > 0 ) {
			side    = _SCH_MODULE_Get_MFIC_SIDE( s + i );
			Pointer = _SCH_MODULE_Get_MFIC_POINTER( s + i );
			//==============================================================================================
			// 2007.05.10
			//==============================================================================================
			if ( _SCH_MACRO_CHECK_FM_MCOOLING( side , s + i , FALSE ) != SYS_SUCCESS ) return 0;
			//==============================================================================================
			if ( oside == -1 ) {
				os = i;
				oside = side;
				_SCH_SUB_GET_OUT_CM_AND_SLOT( side , Pointer , -1 , &oc , &oslot ); // 2007.07.11
				csl = oslot;
			}
			else {
				if ( oside != side ) return 0;
				if ( ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == 0 ) ) {
					if ( oc != _SCH_CLUSTER_Get_PathIn( side , Pointer ) ) return 0;
					csl = _SCH_CLUSTER_Get_SlotIn( side , Pointer );
					if ( ( oslot + i + os ) != csl ) return 0;
				}
				else {
					if ( oc != _SCH_CLUSTER_Get_PathOut( side , Pointer ) ) return 0;
					csl = _SCH_CLUSTER_Get_SlotOut( side , Pointer );
					if ( ( oslot + i + os ) != csl ) return 0;
				}
			}
		}
		else {
			return 0;
		}
	}
	//=================================================
	// 2007.01.23
	//=================================================
	if ( oside == -1 ) return 0;
	//=================================================
	if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) { // 2007.09.04
		return 1;
	}
	//=================================================
	for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 ) ; i++ ) {
		if ( ( oslot + i + os ) > _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 0;
	}
	//===================================================================================================================================================
	// 2007.05.10
	//===================================================================================================================================================
	i = ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
	//
	if ( _SCH_MODULE_Get_MFIC_WAFER( s + i ) <= 0 ) return 1;
	//
	side    = _SCH_MODULE_Get_MFIC_SIDE( s + i );
	Pointer = _SCH_MODULE_Get_MFIC_POINTER( s + i );
	//
	if ( _SCH_MACRO_CHECK_FM_MCOOLING( side , s + i , FALSE ) != SYS_SUCCESS ) return 1;
	//
/*
	if ( sep ) return 3; // 2007.08.23 // imsi code for cooler swapping (2/4)
*/
	return 2; // 2007.08.22
/*
// 2007.08.22
	if ( oside != side ) {
//		if ( sep ) return 3; // 2007.08.02
		return 2;
	}
	//
	if ( ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) == 0 ) ) {
		if ( oc != _SCH_CLUSTER_Get_PathIn( side , Pointer ) ) {
//			if ( sep ) return 3; // 2007.08.02
			return 2;
		}
		csl = _SCH_CLUSTER_Get_SlotIn( side , Pointer );
		if ( ( oslot + i + os ) != csl ) return 1;
	}
	else {
		if ( oc != _SCH_CLUSTER_Get_PathOut( side , Pointer ) ) {
//			if ( sep ) return 3; // 2007.08.02
			return 2;
		}
		csl = _SCH_CLUSTER_Get_SlotOut( side , Pointer );
		if ( ( oslot + i + os ) != csl ) return 1;
	}
	//
	if ( ( oslot + i + os ) > _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 1;
	//===================================================================================================================================================
//	if ( sep ) return 3; // 2007.08.02
	return 2;
*/
}
//==============================================================================================================

int Scheduler_Place_to_CM_with_FM( int arm , BOOL retmode ) {
	int side1 , side2;
	int wafer1 , wafer2;
	int slot1 , slot2;
	int pointer1 , pointer2;
	int tc1 , tc2;
	int Res;
	int retdata; // 2007.07.09
	//----------------------------------------------------------------------
	side1    = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
	side2    = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
	wafer1   = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
	wafer2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
	pointer1 = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
	pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
	//==================================================================================
	if      ( arm == 1 ) wafer2 = 0;
	else if ( arm == 2 ) wafer1 = 0;
	//==================================================================================
	retdata = -1; // 2007.07.09
	//==================================================================================
	if ( retmode ) {
		retdata = 2; // 2007.07.09
		if ( wafer1 > 0 ) {
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( side1 , pointer1 , -1 );
		}
		if ( wafer2 > 0 ) {
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( side2 , pointer2 , -1 );
		}
	}
	//==================================================================================
	if ( ( wafer1 > 0 ) && ( wafer2 > 0 ) ) {
		if ( _SCH_SUB_GET_OUT_CM_AND_SLOT( side1 , pointer1 , wafer1 , &tc1 , &slot1 ) ) { // 2007.07.11
			retdata = 2; // 2007.07.09
		}
		if ( _SCH_SUB_GET_OUT_CM_AND_SLOT( side2 , pointer2 , wafer2 , &tc2 , &slot2 ) ) { // 2007.07.11
			retdata = 2; // 2007.07.09
		}
	}
	else if ( wafer1 > 0 ) {
		tc2 = 0;
		slot2 = 0;
		if ( _SCH_SUB_GET_OUT_CM_AND_SLOT( side1 , pointer1 , wafer1 , &tc1 , &slot1 ) ) { // 2007.07.11
			retdata = 2; // 2007.07.09
		}
	}
	else if ( wafer2 > 0 ) {
		tc1 = 0;
		slot1 = 0;
		if ( _SCH_SUB_GET_OUT_CM_AND_SLOT( side2 , pointer2 , wafer2 , &tc2 , &slot2 ) ) { // 2007.07.11
			retdata = 2; // 2007.07.09
		}
	}
	//==================================================================================
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		if ( wafer1 > 0 ) {
			if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( tc1 ) == 2 ) wafer1 = 0;
		}
		if ( wafer2 > 0 ) {
			if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( tc2 ) == 1 ) wafer2 = 0;
		}
	}
	//==================================================================================
	if ( ( wafer1 <= 0 ) && ( wafer2 <= 0 ) ) return 0;
	//==================================================================================
	if ( SCHEDULER_CM_PLACE_SEPARATE_CHECK_ONEBODY3( tc1 , side1 , slot1 , tc2 , side2 , slot2 ) ) { // 2007.08.31
		//========================================================================
		// 2007.09.03
		//========================================================================
		if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
			if ( FM_Has_Swap_Wafer_Style_4 ) return -1;
		}
		//========================================================================
		//-----------------------------------------------
		FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
		//-----------------------------------------------
		switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 110 , tc1 , wafer1 , slot1 , side1 , pointer1 , 0 , 0 , 0 , 0 , 0 , retdata ) ) { // 2007.03.21
		case -1 :
			return -1;
			break;
		case 0 :
			Res = 1;
			break;
		case 1 :
			Res = 2;
			break;
		case 2 :
			Res = 3;
			break;
		case 3 :
			Res = 4;
			break;
		}
		switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 120 , 0 , 0 , 0 , 0 , 0 , tc2 , wafer2 , slot2 , side2 , pointer2 , retdata ) ) { // 2007.03.21
		case -1 :
			return -1;
			break;
		case 0 :
			Res = 1;
			break;
		case 1 :
			Res = 2;
			break;
		case 2 :
			Res = 3;
			break;
		case 3 :
			Res = 4;
			break;
		}
	}
	else {
		//========================================================================
		// 2007.09.03
		//========================================================================
		if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
			if ( ( wafer1 > 0 ) && ( wafer2 > 0 ) ) {
				if ( side1 == side2 ) {
					if ( !FM_Has_Swap_Wafer_Style_4 ) return -1;
				}
				else {
					if ( FM_Has_Swap_Wafer_Style_4 ) return -1;
				}
			}
			else {
				if ( FM_Has_Swap_Wafer_Style_4 ) return -1;
			}
		}
		//========================================================================
		//-----------------------------------------------
		FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
		//-----------------------------------------------
		switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 130 , tc1 , wafer1 , slot1 , side1 , pointer1 , tc2 , wafer2 , slot2 , side2 , pointer2 , retdata ) ) { // 2007.03.21
		case -1 :
			return -1;
			break;
		case 0 :
			Res = 1;
			break;
		case 1 :
			Res = 2;
			break;
		case 2 :
			Res = 3;
			break;
		case 3 :
			Res = 4;
			break;
		}
	}
	//====================================================================================================================================================
	_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( wafer1 , tc1 , slot1 , side1 , pointer1 , wafer2 , tc2 , slot2 , side2 , pointer2 , TRUE );
	//====================================================================================================================================================
	return Res;
}

//=======================================================================================
int SCHEDULER_CONTROL_Chk_WILLDONE_PART( int rside , int rtmside , int rlog , int tmside , BOOL willmode , int pmcheck , int *side , int *pointer , int *order );
//=======================================================================================
int SCHEDULING_Possible_BM_for_STYLE_4( int side , int bbb , int Slot , int Slot2 , int Pointer , int Pointer2 , int *bmchamber , int *os , int *os2 , BOOL CMFirst , int mdlckeckskip );

BOOL SCHEDULER_TM_HAS_WAFER_SUPPLY( int bmc ) { // 2010.11.29
	int m , i , s , p , sbm , ebm;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 1 ) return FALSE;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) != 2 ) return FALSE;
	//
	if ( bmc == -1 ) {
		sbm = BM1;
		ebm = ( BM1 + MAX_BM_CHAMBER_DEPTH );
	}
	else {
		sbm = bmc;
		ebm = bmc + 1;
	}
	//
//	for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) { // 2011.03.23
	for ( m = sbm ; m < ebm ; m++ ) { // 2011.03.23
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( m - BM1 + PM1 , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( m ) != BUFFER_FM_STATION ) continue;
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			//
			if ( _SCH_MODULE_Get_TM_WAFER( m - BM1 , i ) <= 0 ) continue;
			//
			s = _SCH_MODULE_Get_TM_SIDE( m - BM1 , i );
			p = _SCH_MODULE_Get_TM_POINTER( m - BM1 , i );
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= 1 ) return TRUE;
			//
		}
	}
	return FALSE;
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( int side ) { // 2005.11.30
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( i + BM1 , BUFFER_INWAIT_STATUS ) <= 0 ) {
				if ( !SCHEDULER_TM_HAS_WAFER_SUPPLY( i + BM1 ) ) { // 2010.11.29
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_IN_YES_4( int ch ) { // 2005.11.30
	int j;
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( ch , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
	//====================================================================
	// 2006.11.17
	//====================================================================
	for ( j = TA_STATION ; j <= TB_STATION ; j++ ) {
		if ( _SCH_MODULE_Get_TM_WAFER( ch - BM1 , j ) > 0 ) {
			if ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_TM_SIDE( ch - BM1 , j ) , _SCH_MODULE_Get_TM_POINTER( ch - BM1 , j ) ) == 1 ) return TRUE;
		}
	}
	//====================================================================
	return FALSE;
}



BOOL SCHEDULING_CHECK_FM_sp_BM_Not_In( int side , int pt , int pmc ) { // 2006.11.17
	int j , m;
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , j );
		if ( m > 0 ) {
			if ( ( pmc == -1 ) || ( pmc == m ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( m - PM1 + BM1 , FALSE , side ) ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_IN_YES_4( m - PM1 + BM1 ) ) return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL SCHEDULING_CHECK_FM_sp_Supply_Pick_Impossible( int rcm , int side , int pt , int pmc ) { // 2006.10.19
	switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) ) {
	case 0 :
	case 3 :
	case 1 :
		if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) { // 2006.11.16
			if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) != 1 ) || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.16
				if ( !SCHEDULING_CHECK_FM_sp_BM_Not_In( side , pt , pmc ) ) return FALSE;
			}
		}
		break;
	}
	switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) ) {
	case 0 :
	case 3 :
	case 2 :
		if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) { // 2006.11.16
			if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) != 2 ) || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.16
				if ( !SCHEDULING_CHECK_FM_sp_BM_Not_In( side , pt , pmc ) ) return FALSE;
			}
		}
		break;
	}
	return TRUE;
}


BOOL SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( int side , int pmc ) { // 2006.10.19
	int pt , rcm;
	if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( side , &rcm , &pt , -1 ) ) {
		if ( !SCHEDULING_CHECK_FM_sp_Supply_Pick_Impossible( rcm , side , pt , pmc ) ) return FALSE;
	}
	return TRUE;
}

BOOL SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( int side , int pmc ) { // 2006.10.19
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
			if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( i , pmc ) ) return FALSE;
		}
	}
	return TRUE;
}

BOOL SCHEDULING_CHECK_FM_CurrAnother_Supply_Pick_Possible( int side , int pt , int currside ) { // 2006.11.17
	int m , j , m2 , ok , fc = FALSE;

	for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
			ok = FALSE;
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				m2 = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , j );
				if ( m2 > 0 ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
						if ( m2 - PM1 + BM1 == m ) {
							ok = TRUE;
							break;
						}
					}
				}
			}
			if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( currside , m - BM1 + PM1 ) ) {
				if ( !ok ) fc = TRUE;
				else       return FALSE;
			}
		}
	}
	return fc;
}

BOOL SCHEDULING_CHECK_FM_Cross_Place( int side1 , int pt1 , int side2 , int pt2 ) { // 2006.11.18
	int m1 , j1 , m2 , j2;

	for ( j1 = 0 ; j1 < MAX_PM_CHAMBER_DEPTH ; j1++ ) {
		m1 = _SCH_CLUSTER_Get_PathProcessChamber( side1 , pt1 , 0 , j1 );
		if ( m1 > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m1 , FALSE , side1 ) ) {
				for ( j2 = 0 ; j2 < MAX_PM_CHAMBER_DEPTH ; j2++ ) {
					m2 = _SCH_CLUSTER_Get_PathProcessChamber( side2 , pt2 , 0 , j2 );
					if ( m2 > 0 ) {
						if ( _SCH_MODULE_GET_USE_ENABLE( m2 , FALSE , side2 ) ) {
							if ( m1 == m2 ) return FALSE;
						}
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( int side , int intarm1 , int intarm2 , int *rcm ) { // 2006.11.24
	int rpt;
	if ( !_SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( side , rcm , &rpt , -1 ) ) return FALSE;
	if ( intarm1 != 0 ) {
		if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(*rcm) == intarm1 ) return FALSE;
	}
	if ( intarm2 != 0 ) {
		if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == intarm2 ) && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) return FALSE;
	}
	if ( ( intarm1 != 0 ) || ( intarm2 != 0 ) ) {
		if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) return FALSE;
	}
	return TRUE;
}

//=======================================================================================

BOOL SCHEDULER_CONTROL_Other_BM_Go_Pump_Possible_for_STYLE_4( int bch ) {
	int nch , bmchamber , os , os2 , mp;
	int xfm;

	nch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( bch );
	//
	xfm = FALSE; // 2007.04.24
	mp = 0;
	//
	while( TRUE ) {
	//	if ( _SCH_MODULE_GET_USE_ENABLE( nch - BM1 + PM1 , TRUE , -1 ) ) { // 2005.10.04 // 2006.01.13
	//		if ( _SCH_MODULE_Get_PM_WAFER( nch - BM1 + PM1 , 0 ) > 0 ) return TRUE; // 2006.01.13
	//	} // 2006.01.13
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( nch ) != BUFFER_TM_STATION ) { // 2007.04.13
			if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) {
				if ( SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , nch , _SCH_MODULE_Get_MFAL_WAFER() , 0 , _SCH_MODULE_Get_MFAL_POINTER() , 0 , &bmchamber , &os , &os2 , FALSE , 0 ) > 0 ) {
					if ( nch == bmchamber ) return TRUE;
				}
			}
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) {
				if ( SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , nch , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &bmchamber , &os , &os2 , FALSE , 0 ) > 0 ) {
					if ( nch == bmchamber ) return TRUE;
				}
			}
			if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) {
				if ( SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , nch , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , 0 , &bmchamber , &os , &os2 , FALSE , 0 ) > 0 ) {
					if ( nch == bmchamber ) return TRUE;
				}
			}
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( nch , BUFFER_OUTWAIT_STATUS ) > 0 ) return TRUE;
			xfm = TRUE; // 2007.04.24
		}
		//
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
			while( TRUE ) {
				if ( !SCHEDULER_CONTROL_Chk_ONEBODY_4Extother_nch( bch , mp , &nch ) ) {
					return xfm; // 2007.04.24
//					return FALSE; // 2007.04.24
				}
				mp++;
				if ( _SCH_MODULE_Get_BM_FULL_MODE( nch ) != BUFFER_TM_STATION ) {
					xfm = TRUE; // 2007.04.24
					break;
				}
			}
			return xfm; // 2007.04.24
		}
		else {
			break;
		}
		//
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Curr_BM_Go_Pump_Possible_for_STYLE_4( int nch ) { // 2005.10.05
	int bmchamber , os , os2;
	//
//	if ( _SCH_MODULE_GET_USE_ENABLE( nch - BM1 + PM1 , TRUE , -1 ) ) { // 2006.01.14
//		if ( _SCH_MODULE_Get_PM_WAFER( nch - BM1 + PM1 , 0 ) > 0 ) return TRUE; // 2006.01.14
//	} // 2006.01.14
	//
	if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) {
		if ( SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , nch , _SCH_MODULE_Get_MFAL_WAFER() , 0 , _SCH_MODULE_Get_MFAL_POINTER() , 0 , &bmchamber , &os , &os2 , FALSE , 0 ) > 0 ) {
			if ( nch == bmchamber ) return TRUE;
		}
	}
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) {
		if ( SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , nch , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &bmchamber , &os , &os2 , FALSE , 0 ) > 0 ) {
			if ( nch == bmchamber ) return TRUE;
		}
	}
	if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) {
		if ( SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , nch , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , 0 , &bmchamber , &os , &os2 , FALSE , 0 ) > 0 ) {
			if ( nch == bmchamber ) return TRUE;
		}
	}
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( nch , -1 ) > 0 ) return TRUE;
	return FALSE;
}

int SCHEDULER_CONTROL_BM_PUMPING_FM_PART( int side , BOOL fmside , int bch , int logoffset ) { // 2005.09.09
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) {
		EnterCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_All_Not_Running_for_STYLE_4( bch , FALSE ) ) {
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( bch , FALSE ) ) {
				_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , fmside ? 0 : 1 , logoffset );
				//
				SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , FALSE ); // 2007.05.11
				SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( bch , FALSE ); // 2005.10.05
				LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
				return 2;
			}
			else {
				if ( SCHEDULER_CONTROL_Other_BM_Go_Pump_Possible_for_STYLE_4( bch ) ) {
					if ( fmside ) {
						_SCH_LOG_LOT_PRINTF( side , "FM Scheduling %s Goto TMSIDE(R)%cWHFMGOTMSIDE BM%d:%d,R\n" , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , bch - BM1 + 1 , logoffset );
					}
					else {
						_SCH_LOG_LOT_PRINTF( side , "TM Scheduling %s Goto TMSIDE(R)%cWHTMGOTMSIDE BM%d:%d,R\n" , _SCH_SYSTEM_GET_MODULE_NAME( bch ) , 9 , bch - BM1 + 1 , logoffset );
					}
 					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_for_STYLE_4( bch , FALSE );
					_SCH_MODULE_Set_BM_FULL_MODE( bch , BUFFER_TM_STATION );
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
					return 3;
				}
				else {
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , fmside ? 10000 : 10001 , logoffset );
					//
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , FALSE ); // 2007.05.11
//					_SCH_MODULE_Set_BM_FULL_MODE( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_nch_for_STYLE_4( bch , FALSE ) , BUFFER_TM_STATION );
					SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Set_nch_for_STYLE_4( bch , FALSE );
					//
					LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
					return 4;
				}
			}
		}
		LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
	}
	else {
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , fmside ? 0 : 1 , logoffset );
		return 1;
	}
	return 0;
}


int SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( int side , BOOL fmside , int bch , int logoffset ) { // 2005.10.05
	int i; // 2006.11.22
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) {
		//=====================================================================================================
		// 2006.11.22
		//=====================================================================================================
		for ( i = 3 ; i >= 1 ; i-- ) { // 2006.11.22
		//=====================================================================================================
			EnterCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_All_Not_Running_for_STYLE_4( bch , FALSE ) ) {
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_Other_Wait_RealRun_for_STYLE_4( bch , FALSE ) ) {
					if ( !SCHEDULER_CONTROL_Curr_BM_Go_Pump_Possible_for_STYLE_4( bch ) ) {
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_for_STYLE_4( bch , FALSE ) ) {
							//=====================================================================================================
							// 2006.11.22
							//=====================================================================================================
							if ( _SCH_MODULE_Get_BM_FULL_MODE( bch ) == BUFFER_TM_STATION ) {
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
								return 4;
							}
							//=====================================================================================================
							if ( i == 1 ) {
								_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , bch , -1 , TRUE , fmside ? 20000 : 20001 , logoffset );
								//
								SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Current_Init_for_STYLE_4( bch , FALSE ); // 2007.05.11
								SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_RealRun_Conf_for_STYLE_4( bch , FALSE ); // 2005.10.05
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
								return 1;
							}
							else {
								LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
								//=====================================================================================================
								// 2006.11.22
								//=====================================================================================================
								Sleep(1000);
								//=====================================================================================================
								continue; // 2007.05.28
								//=====================================================================================================
							}
						}
						else {
							LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
							return 2;
						}
					}
					else {
						LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
						return 3;
					}
				}
			}
			LeaveCriticalSection( &CR_SINGLEBM_STYLE_4 ); // 2007.05.28
		}
	}
	return 0;
}

//


//=======================================================================================
//=======================================================================================
int SCHEDULING_Possible_BM_CHeck_Only_at_PM_for_STYLE_4( int bbb , int Slot , int Side , int Pointer , int Slot2 , int Side2 , int Pointer2 ) { // 2005.12.05
	int m , trg;
	if ( ( Slot > 0 ) && ( Pointer < 100 ) ) {
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			trg = _SCH_CLUSTER_Get_PathProcessChamber( Side , Pointer , 0 , m );
			if ( trg > 0 ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( trg , FALSE , Side ) ) trg = -1;
			}
			if ( trg > 0 ) {
				if ( ( trg - PM1 + BM1 ) == bbb ) return 1;
			}
		}
	}
	if ( ( Slot2 > 0 ) && ( Pointer2 < 100 ) ) {
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			trg = _SCH_CLUSTER_Get_PathProcessChamber( Side2 , Pointer2 , 0 , m );
			if ( trg > 0 ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( trg , FALSE , Side2 ) ) trg = -1;
			}
			if ( trg > 0 ) {
				if ( ( trg - PM1 + BM1 ) == bbb ) return 1;
			}
		}
	}
	return -1;
}


//=======================================================================================
//=======================================================================================


int SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( int side , int pch , BOOL check ) {
	int s , Result;
	int sp_old;
	int sp_new;
	int FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2;
	int FM_Buffer , FM_TSlot , FM_TSlot2;

	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		if ( s != side ) {
			if ( !_SCH_SUB_Simple_Supply_Possible( s ) ) continue; // 2006.11.30
//			if ( _SCH_SYSTEM_USING_RUNNING( s ) ) { // 2006.11.30
//				if ( _SCH_SYSTEM_MODE_END_GET( s ) == 0 ) { // 2006.11.30
			Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , s , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 0 , FALSE , 0 , 0 );
			if ( Result >= 0 ) {
				if ( check ) {
					Result = SCHEDULING_Possible_BM_CHeck_Only_at_PM_for_STYLE_4( pch - PM1 + BM1 , FM_Slot , s , FM_Pointer , FM_Slot2 , s , FM_Pointer2 ); // 2005.12.05
				}
				else {
					Result = SCHEDULING_Possible_BM_for_STYLE_4( s , pch - PM1 + BM1 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) , 0 );
				}
				if ( Result >= 0 ) {
					if ( check ) {
						if ( SCHEDULING_CONTROL_Check_LotSpecial_Item_Change_for_STYLE_4( s , FM_Pointer , pch , &sp_old , &sp_new ) ) {
							return TRUE;
						}
					}
					else {
						if ( ( pch - PM1 + BM1 ) == FM_Buffer ) {
							if ( SCHEDULING_CONTROL_Check_LotSpecial_Item_Change_for_STYLE_4( s , FM_Pointer , pch , &sp_old , &sp_new ) ) {
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4( int side , BOOL AllMode ) { // 2004.09.07
	int s , i , j , xside , m;
	Scheduling_Regist	Scheduler_Reg;

	xside = MAX_CASSETTE_SIDE - 1;
	//
	if ( AllMode ) {
		//--------------------------------------------------------------------------------
		if ( ( _SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() % 2 ) == 0 ) return 1;
		//--------------------------------------------------------------------------------
		if ( _SCH_PRM_GET_MODULE_MODE( xside + CM1 ) ) return 2;
		//
		if ( _SCH_MODULE_Get_TM_DoubleCount( side ) == 0 ) return 3;
		//
		if ( _SCH_MODULE_Get_FM_OutCount( side ) == 0 ) return 4;
		//
		if ( _SCH_MODULE_Get_FM_OutCount( side ) != ( _SCH_MODULE_Get_FM_InCount( side ) + _SCH_MODULE_Get_TM_DoubleCount( side ) ) ) return 5;
		//
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			if ( s != side ) {
//				if ( _SCH_SYSTEM_USING_GET( s ) > 0 ) return FALSE; // 2005.03.15
//				if ( _SCH_SYSTEM_USING_GET( s ) >= 9 ) return FALSE; // 2005.03.15
//				if ( ( _SCH_SYSTEM_USING_GET( s ) >= 9 ) || ( ( _SCH_SYSTEM_USING_GET( s ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( s ) < 2 ) ) ) return FALSE; // 2005.03.15 // 2005.07.26
//				if ( ( ( _SCH_SYSTEM_USING_GET( s ) <= 100 ) && ( _SCH_SYSTEM_USING_GET( s ) >= 9 ) ) || ( ( _SCH_SYSTEM_USING_GET( s ) <= 100 ) && ( _SCH_SYSTEM_USING_GET( s ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( s ) < 2 ) ) ) return FALSE; // 2005.03.15 // 2005.07.26 // 2005.07.29
//				if ( _SCH_SYSTEM_USING_RUNNING( s ) ) return 6; // 2005.07.29 // 2006.09.04
				if ( _SCH_SYSTEM_USING_RUNNING( s ) || ( ( _SCH_SYSTEM_USING_GET( s ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( s ) < 2 ) ) ) return 6; // 2006.09.04
			}
		}
		//
		if ( _SCH_SYSTEM_MODE_END_GET( side ) == 0 ) { // 2004.09.10
			if ( _SCH_SUB_Remain_for_FM( side ) ) return 7;
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_ENTER_TM_CRITICAL( i );
		//============================================================================================
		// 2006.03.28
		//============================================================================================
		s = 0;
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( PM1 + i , 0 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_POINTER( PM1 + i , 0 ) >= 100 ) {
					s = 1;
					break;
				}
			}
			for ( j = 0 ; j < 2 ; j++ ) {
				if ( _SCH_MODULE_Get_TM_WAFER( i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_TM_POINTER( i , j ) >= 100 ) {
						s = 1;
						break;
					}
				}
			}
			if ( s == 1 ) break;
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( BM1 + i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_POINTER( BM1 + i , j ) >= 100 ) {
						s = 1;
						break;
					}
				}
			}
			if ( s == 1 ) break;
		}
		if ( s == 0 ) {
			for ( j = 0 ; j < 2 ; j++ ) {
				if ( _SCH_MODULE_Get_FM_WAFER( j ) > 0 ) {
					if ( _SCH_MODULE_Get_FM_POINTER( j ) >= 100 ) {
						s = 1;
						break;
					}
				}
			}
		}
		if ( s == 0 ) {
			for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
			return 12;
		}
		//============================================================================================
		//------------------------------------------------------------
		//m = _SCH_SYSTEM_FA_GET( side ); // 2004.09.13
		m = 0; // 2004.09.13

		if ( m == 0 ) {
//			_SCH_IO_SET_IN_PATH( xside , xside ); // 2005.09.27
			_SCH_IO_SET_IN_PATH( xside , xside + 1 ); // 2005.09.27
//			_SCH_IO_SET_OUT_PATH( xside , xside ); // 2005.09.27
			_SCH_IO_SET_OUT_PATH( xside , xside + 1 ); // 2005.09.27
			_SCH_IO_SET_START_SLOT( xside , 1 );
			_SCH_IO_SET_END_SLOT( xside , 1 );
			_SCH_IO_SET_RECIPE_FILE( xside , "Dummy" );
			_SCH_IO_SET_LOOP_COUNT( xside , 1 );
			//
			_SCH_SYSTEM_FA_SET( xside , 0 );
			_SCH_SYSTEM_CPJOB_SET( xside , 0 );
		}
		else {
			Scheduler_Reg.RunIndex  = xside;
			Scheduler_Reg.CassIn    = xside;
			Scheduler_Reg.CassOut   = xside;
			Scheduler_Reg.SlotStart = 1;
			Scheduler_Reg.SlotEnd   = 1;
			Scheduler_Reg.LoopCount = 1;
			strcpy( Scheduler_Reg.JobName , "Dummy" );
			strcpy( Scheduler_Reg.LotName , "Dummy" );
			strcpy( Scheduler_Reg.MidName , "Dummy" );
			if ( !_SCH_MULTIREG_SET_REGIST_DATA( xside , &Scheduler_Reg ) ) {
				for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
				return 8;
			}
			else {
				_SCH_SYSTEM_FA_SET( xside , 1 );
				_SCH_SYSTEM_CPJOB_SET( xside , 0 );
			}
		}
		//--------------------------------------------------------------------------------------
		_SCH_SYSTEM_USING_SET( xside , 1 );
		//--------------------------------------------------------------------------------------
		_SCH_SUB_Reset_Scheduler_Data( xside );
		//--------------------------------------------------------------------------------------
		_SCH_PRM_SET_DFIX_GROUP_RECIPE_PATH( xside , _SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) ); // 2005.08.03
		//--------------------------------------------------------------------------------------
		if ( !_SCH_SUB_Main_Handling_Code_Spawn( xside ) ) {
			_IO_CIM_PRINTF( "THREAD FAIL Main_Code_Api_Proc_Style(4) %d\n" , xside );
			if ( m != 0 ) _SCH_MULTIREG_DATA_RESET( xside );
			for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
			return 9;
		}

		while( TRUE ) {
			Sleep(100);
			//-------------------------------------------
			if ( _SCH_SYSTEM_USING_GET( xside ) >= 10 ) break;
			//-------------------------------------------
			if ( MANAGER_ABORT() ) {
				if ( m != 0 ) _SCH_MULTIREG_DATA_RESET( xside );
				for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
				return 11;
			}
			//-------------------------------------------
			if ( _SCH_SYSTEM_USING_GET( xside ) <= 0 ) {
				_IO_CIM_PRINTF( "THREAD RUN FAIL Main_Code_Api_Proc_Style(4-2) %d\n" , xside );
				if ( m != 0 ) _SCH_MULTIREG_DATA_RESET( xside );
				for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
				return 10;
			}
			//-------------------------------------------
		}
		//
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( PM1 + i , 0 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_POINTER( PM1 + i , 0 ) >= 100 ) {
					if ( _SCH_MODULE_Get_PM_SIDE( PM1 + i , 0 ) != xside ) {
						_SCH_MODULE_Set_PM_SIDE( PM1 + i , 0 , xside );
						//
						_SCH_MODULE_Inc_FM_OutCount( xside );
						_SCH_MODULE_Inc_TM_OutCount( xside );
						//
						_SCH_MODULE_Inc_TM_DoubleCount( xside );
						//
						_SCH_MODULE_Inc_FM_InCount( side );
						_SCH_MODULE_Inc_TM_InCount( side );
						//
						_SCH_MODULE_Dec_TM_DoubleCount( side );
					}
				}
			}
			for ( j = 0 ; j < 2 ; j++ ) {
				if ( _SCH_MODULE_Get_TM_WAFER( i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_TM_POINTER( i , j ) >= 100 ) {
						if ( _SCH_MODULE_Get_TM_SIDE( i , j ) != xside ) {
							_SCH_MODULE_Set_TM_SIDE( i , j , xside );
							//
							_SCH_MODULE_Inc_FM_OutCount( xside );
							_SCH_MODULE_Inc_TM_OutCount( xside );
							//
							_SCH_MODULE_Inc_TM_DoubleCount( xside );
							//
							_SCH_MODULE_Inc_FM_InCount( side );
							_SCH_MODULE_Inc_TM_InCount( side );
							//
							_SCH_MODULE_Dec_TM_DoubleCount( side );
						}
					}
				}
			}
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( BM1 + i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_POINTER( BM1 + i , j ) >= 100 ) {
						if ( _SCH_MODULE_Get_BM_SIDE( BM1 + i , j ) != xside ) {
							_SCH_MODULE_Set_BM_SIDE( BM1 + i , j , xside );
							//
							_SCH_MODULE_Inc_FM_OutCount( xside );
							//
							_SCH_MODULE_Inc_TM_DoubleCount( xside );
							//
							_SCH_MODULE_Inc_FM_InCount( side );
							//
							_SCH_MODULE_Dec_TM_DoubleCount( side );
						}
					}
				}
			}
		}
		for ( j = 0 ; j < 2 ; j++ ) {
			if ( _SCH_MODULE_Get_FM_WAFER( j ) > 0 ) {
				if ( _SCH_MODULE_Get_FM_POINTER( j ) >= 100 ) {
					if ( _SCH_MODULE_Get_FM_SIDE( j ) != xside ) {
						_SCH_MODULE_Set_FM_SIDE( j , xside );
						//
						_SCH_MODULE_Inc_FM_OutCount( xside );
						//
						_SCH_MODULE_Inc_TM_DoubleCount( xside );
						//
						_SCH_MODULE_Inc_FM_InCount( side );
						//
						_SCH_MODULE_Dec_TM_DoubleCount( side );
					}
				}
			}
		}
		SCHEDULER_CONTROL_Change_SDM_4_Dummy_Side( -1 , xside );
		//
		_SCH_SUB_Set_Last_Status( xside , 2 ); // 2005.07.27
		//
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
	}
	else {
		if ( _SCH_MODULE_Get_TM_DoubleCount( side ) == 0 ) return 1;
		//
		if ( _SCH_MODULE_Get_FM_OutCount( side ) != ( _SCH_MODULE_Get_FM_InCount( side ) + _SCH_MODULE_Get_TM_DoubleCount( side ) ) ) return 2;
		//
		if ( !_SCH_PRM_GET_MODULE_MODE( xside + CM1 ) ) {
//			if ( _SCH_SYSTEM_USING_GET( xside ) > 0 ) return FALSE; // 2005.03.15
//			if ( ( _SCH_SYSTEM_USING_GET( xside ) >= 9 ) || ( ( _SCH_SYSTEM_USING_GET( xside ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( xside ) < 2 ) ) ) return FALSE; // 2005.03.15 // 2005.07.29
			if ( _SCH_SYSTEM_USING_RUNNING( xside ) ) return 3; // 2005.07.29
		}
		else {
			xside = -1;
		}
		//
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			if ( ( s != side ) && ( s != xside ) ) {
//				if ( _SCH_SYSTEM_USING_GET( s ) > 0 ) break; // 2005.03.15
//				if ( ( _SCH_SYSTEM_USING_GET( s ) >= 9 ) || ( ( _SCH_SYSTEM_USING_GET( s ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( s ) < 2 ) ) ) break; // 2005.03.15
//				if ( ( ( _SCH_SYSTEM_USING_GET( s ) <= 100 ) && ( _SCH_SYSTEM_USING_GET( s ) >= 9 ) ) || ( ( _SCH_SYSTEM_USING_GET( s ) <= 100 ) && ( _SCH_SYSTEM_USING_GET( s ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( s ) < 2 ) ) ) break; // 2005.03.15 // 2005.07.26 // 2005.07.29
				if ( _SCH_SYSTEM_USING_RUNNING( s ) ) break; // 2005.07.29
			}
		}
		if ( s == MAX_CASSETTE_SIDE ) return 4;
		//
		if ( _SCH_SYSTEM_MODE_END_GET( side ) == 0 ) { // 2004.09.10
			if ( _SCH_SUB_Remain_for_FM( side ) ) return 5;
		}
		//==========================================================================================================
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_ENTER_TM_CRITICAL( i );
		//==========================================================================================================
		// 2006.08.01
		//==========================================================================================================
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , PM1 + i ) == MUF_00_NOTUSE ) {
				if ( SCHEDULER_CONTROL_Wait_SDM_4_Dummy_Chamber( PM1 + i ) ) {
					_SCH_MODULE_Set_Mdl_Use_Flag( s , PM1 + i , MUF_04_USE_to_PREND_EF_LP );
				}
			}
		}
		//==========================================================================================================
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
			if ( _SCH_MODULE_Get_PM_WAFER( PM1 + i , 0 ) > 0 ) {
				if ( _SCH_MODULE_Get_PM_POINTER( PM1 + i , 0 ) >= 100 ) {
					if ( _SCH_MODULE_Get_PM_SIDE( PM1 + i , 0 ) == side ) { // 2005.07.26
						_SCH_MODULE_Set_PM_SIDE( PM1 + i , 0 , s );
						//
						_SCH_MODULE_Inc_FM_OutCount( s );
						_SCH_MODULE_Inc_TM_OutCount( s );
						//
						_SCH_MODULE_Inc_TM_DoubleCount( s );
						//
						_SCH_MODULE_Inc_FM_InCount( side );
						_SCH_MODULE_Inc_TM_InCount( side );
						//
						_SCH_MODULE_Dec_TM_DoubleCount( side );
					}
				}
			}
			for ( j = 0 ; j < 2 ; j++ ) {
				if ( _SCH_MODULE_Get_TM_WAFER( i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_TM_POINTER( i , j ) >= 100 ) {
						if ( _SCH_MODULE_Get_TM_SIDE( i , j ) == side ) { // 2005.07.26
//						if ( _SCH_MODULE_Get_TM_SIDE( i , j ) != s ) { // 2005.07.26
							_SCH_MODULE_Set_TM_SIDE( i , j , s );
							//
							_SCH_MODULE_Inc_FM_OutCount( s );
							_SCH_MODULE_Inc_TM_OutCount( s );
							//
							_SCH_MODULE_Inc_TM_DoubleCount( s );
							//
							_SCH_MODULE_Inc_FM_InCount( side );
							_SCH_MODULE_Inc_TM_InCount( side );
							//
							_SCH_MODULE_Dec_TM_DoubleCount( side );
						}
					}
				}
			}
			for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( BM1 + i ) ; j++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( BM1 + i , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_POINTER( BM1 + i , j ) >= 100 ) {
						if ( _SCH_MODULE_Get_BM_SIDE( BM1 + i , j ) == side ) { // 2005.07.26
//						if ( _SCH_MODULE_Get_BM_SIDE( BM1 + i , j ) != s ) { // 2005.07.26
							_SCH_MODULE_Set_BM_SIDE( BM1 + i , j , s );
							//
							_SCH_MODULE_Inc_FM_OutCount( s );
							//
							_SCH_MODULE_Inc_TM_DoubleCount( s );
							//
							_SCH_MODULE_Inc_FM_InCount( side );
							//
							_SCH_MODULE_Dec_TM_DoubleCount( side );
						}
					}
				}
			}
		}
		for ( j = 0 ; j < 2 ; j++ ) {
			if ( _SCH_MODULE_Get_FM_WAFER( j ) > 0 ) {
				if ( _SCH_MODULE_Get_FM_POINTER( j ) >= 100 ) {
					if ( _SCH_MODULE_Get_FM_SIDE( j ) == side ) { // 2005.07.26
//					if ( _SCH_MODULE_Get_FM_SIDE( j ) != s ) { // 2005.07.26
						_SCH_MODULE_Set_FM_SIDE( j , s );
						//
						_SCH_MODULE_Inc_FM_OutCount( s );
						//
						_SCH_MODULE_Inc_TM_DoubleCount( s );
						//
						_SCH_MODULE_Inc_FM_InCount( side );
						//
						_SCH_MODULE_Dec_TM_DoubleCount( side );
					}
				}
			}
		}
		SCHEDULER_CONTROL_Change_SDM_4_Dummy_Side( side , s );

		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) _SCH_SYSTEM_LEAVE_TM_CRITICAL( i );
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
BOOL SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( int side ) { // 2004.10.01
	int i , j , c , n , o;
	for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) {
			c = 0;
			n = 0;
			o = 0;
			for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
					c++;
					if      ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_OUTWAIT_STATUS ) o++;
					else if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_INWAIT_STATUS ) n++;
				}
			}
			//
			if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( i + BM1 ) ) n++; // 2010.11.29
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( i + PM1 , FALSE , -1 ) ) { // 2006.03.10
				if ( c == 0 ) return FALSE;
				if ( ( o > 0 ) && ( n > 0 ) ) return TRUE;
			}
			else { // 2006.03.10
				if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( i + BM1 ) ) {
					if ( o > 0 ) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//
BOOL SCHEDULER_CONTROL_Chk_BM_NEED_NO_SUPPLY_4( int side ) { // 2014.12.12
	int i , j , n , o;
	for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) {
			//
			n = 0;
			o = 0;
			//
			for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
					if      ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_OUTWAIT_STATUS ) o++;
					else if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_INWAIT_STATUS ) n++;
				}
			}
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( i + PM1 , FALSE , -1 ) ) {
				if ( ( o > 0 ) && ( n > 0 ) ) return TRUE;
			}
			else {
				if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( i + BM1 ) ) {
					if ( o > 0 ) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL SCHEDULER_CONTROL_Chk_BM_OUT_CMFIRST_FOR_4( int side , BOOL chkcm , int bmch , int *pt ) {
	int i , k = 0 , z = 0 , z2 = 0 , z3 = 0;
	*pt = 99999999;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) {
				if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
					if ( _SCH_MODULE_Get_BM_POINTER( bmch , i ) >= 100 ) {
						*pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
						k++;
					}
					else if ( *pt > _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) {
						if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( side , _SCH_MODULE_Get_BM_POINTER( bmch , i ) , 2 ) != 0 ) {
							*pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
							k++;
						}
					}
				}
				else { 
					z2 = 1;
				}
			}
			else {
				z3 = 1;
			}
		}
		else {
			z = 1;
		}
	}
	if ( k > 0 ) {
//		if ( chkcm && ( z == 0 ) && ( z2 == 1 ) && ( z3 == 1 ) ) return FALSE; // 2003.03.03
		if ( chkcm ) {
			if ( z  == 0 ) return TRUE; // 2003.03.03
			if ( z2 == 1 ) return TRUE; // 2003.03.03
			if ( z3 == 1 ) return TRUE; // 2003.03.03
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_OUT_OTHER_FOR_4( int side , int bmch ) { // 2004.11.04
	int i;
	BOOL otheryes = FALSE;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
		if ( ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
			if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) {
				return FALSE;
			}
			else {
				otheryes = TRUE;
			}
		}
	}
	return otheryes;
}

BOOL SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( int side , int bmch , int *FM_Slot , int *FM_Slot2 ) {
	int i , k = 0 , pt , oldpt1 = 9999999 , oldpt2 = 9999999;
	int s1 = 0 , s2 = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
//		if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) { // 2007.01.08
		if ( ( side < 0 ) || ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) == side ) ) { // 2007.01.08
			if ( ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
				pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
				if ( pt >= 100 ) {
					k++;
					s2 = s1;
					s1 = i;
					break;
				}
				else {
					if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( _SCH_MODULE_Get_BM_SIDE( bmch , i ) , pt , 2 ) != 0 ) { // 2007.01.08 // 2007.03.30
						if ( oldpt1 > pt ) {
							k++;
							oldpt2 = oldpt1;
							oldpt1 = pt;
							s2 = s1;
							s1 = i;
						}
						else if ( oldpt2 > pt ) {
							k++;
							oldpt2 = pt;
							s2 = i;
						}
					}
				}
			}
		}
	}
	if ( k > 0 ) {
		*FM_Slot = s1;
		*FM_Slot2 = s2;
		return TRUE;
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_OUT_OTHER_SIDE_FOR_4( int side , int bmch , int *FM_Slot , int *FM_Slot2 ) { // 2005.10.06
	int i , k = 0 , pt , oldpt1 = 9999999 , oldpt2 = 9999999 , sd = -1;
	int s1 = 0 , s2 = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_SIDE( bmch , i ) != side ) {
			if ( ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) && ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) ) {
				sd = _SCH_MODULE_Get_BM_SIDE( bmch , i );
				pt = _SCH_MODULE_Get_BM_POINTER( bmch , i );
				if ( pt >= 100 ) {
					k++;
					s2 = s1;
					s1 = i;
					break;
				}
				else {
					if ( _SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( sd , pt , 2 ) != 0 ) {
						if ( oldpt1 > pt ) {
							k++;
							oldpt2 = oldpt1;
							oldpt1 = pt;
							s2 = s1;
							s1 = i;
						}
						else if ( oldpt2 > pt ) {
							k++;
							oldpt2 = pt;
							s2 = i;
						}
					}
				}
			}
		}
	}
	if ( k > 0 ) {
		*FM_Slot = s1;
		*FM_Slot2 = s2;
		return TRUE;
	}
	return FALSE;
}



BOOL SCHEDULER_CONTROL_Chk_FM_OUT_HAS_FirstRunPre_FOR_4( int side , int pointer ) { // 2006.01.12
	int i , m;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
//				if ( _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , m ) ) return TRUE; // 2007.04.12
				if ( !_SCH_EQ_PROCESS_SKIPFORCE_RUN( 1 , m ) && _SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , m ) ) return TRUE; // 2007.04.12
			}
		}
	}
	return FALSE;
}



BOOL SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( int side , int bmch , BOOL TmmovePass , int nxmode ) {
	int k = 0;
	int s , p , k2 = 0; // 2010.11.10
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) return FALSE;
	if ( !TmmovePass ) {
		//
		s = -1;
		//
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) {
//			return FALSE; // 2010.11.10
			//
			s = _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ); // 2010.11.10
			p = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ); // 2010.11.10
			//
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) {  // 2010.11.10
				k2++;
			} // 2010.11.10
			else { // 2010.11.10
				return FALSE;
			}
		}
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ) > 0 ) {
//			return FALSE; // 2010.11.10
			//
			if ( s != -1 ) { // 2010.11.10
				if ( s != _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ) ) {
					s = -1;
				}
				else {
					if ( p != _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ) ) {
						s = -1;
					}
				}
			}
			//
			if ( s == -1 ) { // 2010.11.10
				//
				s = _SCH_MODULE_Get_TM_SIDE( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ); // 2010.11.10
				p = _SCH_MODULE_Get_TM_POINTER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ); // 2010.11.10
				//
				if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 1 ) {  // 2010.11.10
					k2++;
				} // 2010.11.10
				else { // 2010.11.10
					return FALSE;
				}
			}
			//
		}
	}
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_OUTWAIT_STATUS ) > 0 ) return FALSE;
	//
//	k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS ); // 2010.11.10
	k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS ) + k2; // 2010.11.10
	//
	if ( TmmovePass ) {
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_TM_PLACE ) ) > 0 ) k++;
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , SCHEDULER_CONTROL_Chk_ARM_WHAT( bmch - BM1 , CHECKORDER_FOR_FM_PLACE ) ) > 0 ) k++;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) return FALSE; // 2003.08.11
	if ( nxmode >= 2 ) { // Full In
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) k++;
		if ( _SCH_PRM_GET_MODULE_SIZE( bmch ) <= k ) return TRUE;
	}
	else { // Normal In
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) <= 0 ) {
			if ( _SCH_PRM_GET_MODULE_SIZE( bmch ) <= (k+1) ) return TRUE;
		}
		else {
			k++;
			if ( _SCH_PRM_GET_MODULE_SIZE( bmch ) <= k ) return TRUE;
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( int bmch , int fullcheck ) {
	int k = 0;
	if ( fullcheck == 1 ) {
		if ( SCHEDULER_CONTROL_Chk_BM_FULL_ALL_STYLE_4( bmch ) ) return TRUE;
	}
	//
	k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS );
	//
	if ( fullcheck == 2 ) { // 2008.12.08
		if ( k > 0 ) return TRUE;
	}
	//
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 0 ) > 0 ) k++;
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 1 ) > 0 ) k++; // 2005.06.28
	if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) k++;
	if ( _SCH_PRM_GET_MODULE_SIZE( bmch ) <= k ) return TRUE;
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_ALL_FREE_FOR_4( int bmch ) { // 2007.04.12
	int i;
	for ( i = 0 ; i < 2 ; i ++ ) {
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , -1 ) > 0 ) return FALSE;
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 0 ) > 0 ) return FALSE;
		if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 1 ) > 0 ) return FALSE;
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) return FALSE;
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_ALREADY_SUPPLIED_FOR_4( int bmch , BOOL all ) { // 2007.04.12
	if ( all ) {
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) > 0 ) return FALSE; // 2007.04.19
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) return TRUE;
	}
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
	//
	if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( bmch ) ) return TRUE; // 2010.11.29
	//
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( int bmch , int side , int pt ) { // 2007.04.12
	int i;
	if (
		( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( bmch , FALSE ) ) ||
		( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_TM_STATION )
	) {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) {
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side , pt , _SCH_MODULE_Get_BM_SIDE( bmch , i ) , _SCH_MODULE_Get_BM_POINTER( bmch , i ) ) ) { // 2007.05.28
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_HAS_SPACE_FOR_PALCE_WHEN_PRE_PICK_FOR_4( int bmch ) { // 2006.12.18
	int k1 , k2;
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_TM_STATION ) return FALSE;
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 0 ) > 0 ) return FALSE;
	if ( _SCH_MODULE_Get_TM_WAFER( bmch - BM1 , 1 ) > 0 ) return FALSE;
	k1 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS );
	k2 = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_OUTWAIT_STATUS );
	if ( _SCH_PRM_GET_MODULE_SIZE( bmch ) <= ( k1 + k2 ) ) return FALSE;
	if ( ( k1 > 0 ) && ( k2 <= 0 ) ) return TRUE;
	if ( ( k2 > 0 ) && ( k1 <= 0 ) ) return TRUE;
	return FALSE;
}

int SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( int bmch ) { // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
	int i , id = 0 , od = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if      ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) od++;
			else if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) id++;
		}
	}
	//
	if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( bmch ) ) id++; // 2010.11.29
	//
	if ( ( id == 0 ) && ( od == 0 ) ) return 0;
	if ( ( id >  0 ) && ( od == 0 ) ) return 1;
	if ( ( id == 0 ) && ( od >  0 ) ) return 2;
	return 3;
}

int SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK2_FOR_4( int bmch ) { // 2017.04.27 0:xx 1:Ix 2:xO 3:IO
	int i , id = 0 , od = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if      ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_OUTWAIT_STATUS ) od++;
			else if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) == BUFFER_INWAIT_STATUS ) id++;
		}
	}
	//
	if ( ( id == 0 ) && ( od == 0 ) ) return 0;
	if ( ( id >  0 ) && ( od == 0 ) ) return 1;
	if ( ( id == 0 ) && ( od >  0 ) ) return 2;
	return 3;
}

BOOL SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( int side ) { // 2004.08.20
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) return FALSE;
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_ONE_NOT_VACUUM( int side ) { // 2004.08.20
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) { // 2006.05.26
			if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( i + BM1 ) ) return TRUE; // 2006.05.26
		}
	}
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_IN_YES_OUT_NEED( int side , int bmc ) { // 2006.11.23
	int i , o , n;
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_FM_STATION ) return FALSE;
	o = 0;
	n = 0;
	for ( i = _SCH_PRM_GET_MODULE_SIZE( bmc ) ; i >= 1 ; i-- ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmc , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_OUTWAIT_STATUS ) {
				if ( _SCH_MODULE_Get_BM_SIDE( bmc , i ) == side ) {
					o++;
				}
			}
			else if ( _SCH_MODULE_Get_BM_STATUS( bmc , i ) == BUFFER_INWAIT_STATUS ) {
				n++;
			}
		}
	}
	//
	if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( bmc ) ) n++; // 2010.11.29
	//
	if ( ( o > 0 ) && ( n > 0 ) ) return TRUE;
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_FM_SIDE( int bmc ) { // 2015.02.05
	int s1p;
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_FM_STATION ) return FALSE;
	//
	switch( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) ) {
	case 0 :
	case 3 :
		return TRUE;
	}
	//
	s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( bmc );
	//
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc + s1p ) != BUFFER_FM_STATION ) return FALSE;
	//
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_ONLY_OUT_NEED( int side , int *bmc ) { // 2004.11.10
	int i , j , l , k;
//	int S , P , arm; // 2006.10.17
//	int fm_place_bm_impossible; // 2006.10.12 // 2006.10.17
	int bc; // 2006.11.04
	int bs , bp;

	//==============================================================================================
	// 2006.11.04
	//==============================================================================================
	bs = -1;
	if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) ) {
			bs = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
			bp = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		}
	}
	else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) ) {
			bs = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
			bp = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		}
	}
	//==============================================================================================
	// 2006.10.17
	//==============================================================================================
	for ( l = 0 ; l < 2 ; l++ ) { // 2007.03.15
		for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
			if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
			//=============================================================================================
			// 2006.11.03
			//=============================================================================================
			switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + BM1 ) ) {
			case 1 :
				if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) continue;
				bc = FALSE; // 2006.11.04
				break;
			case 2 :
				if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) continue;
				bc = FALSE; // 2006.11.04
				break;
			case 0 : // 2006.11.04
				bc = TRUE;
				break;
			}
			//=============================================================================================
//			if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) { // 2015.02.05
			if ( SCHEDULER_CONTROL_Chk_BM_FM_SIDE( i + BM1 ) ) { // 2015.02.05
				k = 0;
				for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
						if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_OUTWAIT_STATUS ) {
							//=================================================================
							// 2006.11.04
							//=================================================================
							if ( ( bc ) && ( bs != -1 ) ) {
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( i + BM1 , j ) , _SCH_MODULE_Get_BM_POINTER( i + BM1 , j ) , bs , bp ) ) {
									k++;
								}
								else {
									break;
								}
							}
							//=================================================================
							else {
								k++;
							}
						}
					}
					else {
						if ( l == 0 ) { // 2007.03.15
							k = 0;
							break;
						}
					}
				}
				if ( k > 0 ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_OUT_OTHER_FOR_4( side , i + BM1 ) ) { // 2007.03.15
						*bmc = i + BM1;
						return TRUE;
					}
				}
			}
		}
	}
	//==============================================================================================
	// 2006.10.12
	//==============================================================================================
	// 2006.10.17
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) return FALSE;
	}
	else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) return FALSE;
	}
/*
	// 2006.10.17
	arm = -1;
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) arm = TA_STATION;
	}
	else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) arm = TB_STATION;
	}
	if ( arm == -1 ) {
		fm_place_bm_impossible = FALSE;
	}
	else {
		S = _SCH_MODULE_Get_FM_SIDE( arm );
		P = _SCH_MODULE_Get_FM_POINTER( arm );
		if ( _SCH_CLUSTER_Get_PathDo( S , P ) != 0 ) {
			fm_place_bm_impossible = FALSE;
		}
		else {
			fm_place_bm_impossible = TRUE;
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				j = _SCH_CLUSTER_Get_PathProcessChamber( S , P , 0 , i );
				if ( j <= 0 ) continue;
				if ( _SCH_MODULE_GET_USE_ENABLE( j - PM1 + BM1 , FALSE , S ) ) {
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( j - PM1 + BM1 ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( j - PM1 + BM1 ) == BUFFER_FM_STATION ) ) {
						fm_place_bm_impossible = FALSE;
					}
				}
			}
		}
	}
*/
	//==============================================================================================
	for ( i = 0 ; i < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , -1 ) ) continue;
		//=============================================================================================
		// 2006.11.03
		//=============================================================================================
		switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i + BM1 ) ) {
		case 1 :
			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) continue;
			bc = FALSE; // 2006.11.04
			break;
		case 2 :
			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) continue;
			bc = FALSE; // 2006.11.04
			break;
		case 0 : // 2006.11.04
			bc = TRUE; // 2006.11.04
			break;
		}
		//=============================================================================================
//		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) { // 2015.02.05
		if ( SCHEDULER_CONTROL_Chk_BM_FM_SIDE( i + BM1 ) ) { // 2015.02.05
			k = 0;
			for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_OUTWAIT_STATUS ) {
						//=================================================================
						// 2006.11.04
						//=================================================================
						if ( ( bc ) && ( bs != -1 ) ) {
							if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( i + BM1 , j ) , _SCH_MODULE_Get_BM_POINTER( i + BM1 , j ) , bs , bp ) ) {
								k++;
							}
							else {
								break;
							}
						}
						//=================================================================
						else {
							k++;
						}
					}
				}
//				else { // 2006.10.17
//					if ( !fm_place_bm_impossible ) { // 2006.10.12 // 2006.10.17
//						k = 0; // 2006.10.17
//						break; // 2006.10.17
//					} // 2006.10.17
//				} // 2006.10.17
			}
			if ( k > 0 ) {
				*bmc = i + BM1;
				return TRUE;
			}
		}
	}
	return FALSE;
}


int SCHEDULING_Possible_BM_3_for_STYLE_4( int side , int bbb , int Slot1 , int Slot2 , int Pointer1 , int Pointer2 , int *bmchamber , int *os , int *os2 , BOOL CMFirst ) {
	int i;
	//--------------------------------------------------------------------
	*os = 0;
	*os2 = 0;
	*bmchamber = bbb;
	//--------------------------------------------------------------------
	if ( ( Slot1 > 0 ) && ( Slot2 > 0 ) ) {
//		if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bbb , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.01.18
		if ( SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( bbb , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.01.18
			*os = i;
			*os2 = 0;
//			if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.01.18
			if ( SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.01.18
				*os2 = i * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
			}
			else {
				return -111;
			}
		}
		else {
			return -112;
		}
	}
	else if ( ( Slot1 <= 0 ) && ( Slot2 > 0 ) ) {
		*os = 0;
		*os2 = 0;
//		if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.01.18
		if ( SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.01.18
			*os2 = i * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
		}
		else {
			return -113;
		}
	}
	else {
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bbb , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bbb - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
			*os = i;
			*os2 = 0;
		}
		else {
			return -114;
		}
	}
	return 1;
}


int SCHEDULING_Possible_BM_for_STYLE_4( int side , int bbb , int Slot1 , int Slot2 , int Pointer1 , int Pointer2 , int *bmchamber , int *os , int *os2 , BOOL CMFirst , int mdlcheckskip ) {
	int x3 , i , i2 , m , bmch , trg , f , ck , s1p , l;
	int RemainTime , op , pr;
	int Slot , Pointer;
	//
	int errres; // 2010.12.09
	//
	for ( x3 = 0 ; x3 < 2 ; x3++ ) { // 2007.04.04
		//--------------------------------------------------------------------
		// 2007.04.04
		//--------------------------------------------------------------------
		f = 0;
		RemainTime = -1;
		op = -1;
		//--------------------------------------------------------------------
		*bmchamber = -1; // 2006.01.23
		//--------------------------------------------------------------------
		*os = 0; // 2006.11.10
		*os2 = 0; // 2006.11.10
		//--------------------------------------------------------------------
		errres = 0; // 2010.12.09
		//
		for ( l = 0 ; l < 2 ; l++ ) {
			//
			if ( l == 0 ) {
				Slot = Slot1;
				Pointer = Pointer1;
			}
			else {
				Slot = Slot2;
				Pointer = Pointer2;
			}
			//=============================================================
			if ( Slot <= 0 ) continue;
			//=============================================================
			RemainTime = -1; // 2006.01.14
			op = -1; // 2006.01.14
			pr = -1; // 2006.01.24
			if ( Pointer < 100 ) {
				if ( _SCH_CLUSTER_Get_PathDo( side , Pointer ) != PATHDO_WAFER_RETURN ) { // 2006.09.18
					for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
						trg = _SCH_CLUSTER_Get_PathProcessChamber( side , Pointer , 0 , m );
						if ( trg > 0 ) {
							if ( !_SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side ) ) {
								trg = -1; // 2003.06.19
							}
							else {
								//=====================================================================================================================
								// 2007.04.04
								//=====================================================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( trg - PM1 + BM1 , FALSE ) ) {
										trg = -1;
									}
								}
								//=====================================================================================================================
							}
						}
						//==================================================================================
						// 2007.05.10
						//==================================================================================
						else if ( trg < 0 ) {
							if ( mdlcheckskip == 1 ) {
								if ( !_SCH_MODULE_GET_USE_ENABLE( -trg , FALSE , side ) ) {
									if ( ( -trg - PM1 + BM1 ) == bbb ) {
										trg = -trg;
									}
								}
							}
						}
						//==================================================================================
						if ( trg > 0 ) {
							bmch = trg - PM1 + BM1;


//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-1 RESULT = [x3=%d][l=%d] [%d,%d,%d] [f=%d]\n" , x3 , l , *bmchamber , bbb , trg , f );
//----------------------------------------------------------------------

		//					if ( (!CMFirst) || !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.05.26
		//						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_FM_STATION ) ) { // 2006.05.26
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_FM_STATION ) ) { // 2006.05.26
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-2 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
								if ( (!CMFirst) || !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.05.26
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-3 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
									if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( bmch , ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ? 1 : 2 ) ) {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-4 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
										if ( bmch != -bbb ) { // 2005.12.06
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-5 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
											// f = 1; // 2006.01.14
											if ( bmch == bbb ) {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-6 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
												f = 1; // 2006.01.14
												*bmchamber = bmch;
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-7 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
													if ( ( Slot1 > 0 ) && ( Slot2 > 0 ) && ( l == 0 ) ) {
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
															//===================================================================================================================================
															// 2007.05.10
															//===================================================================================================================================
															if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( bmch , &i2 , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
																return -121;
															}
															if ( i != i2 ) return -122;
															//===================================================================================================================================
															*os = i;
															*os2 = 0;
//															if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.04.13
															if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i2 , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) { // 2007.04.13
																*os2 = i * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
															}
															else {
																return -111;
															}
														}
														else {
															return -112;
														}
													}
													else if ( ( Slot1 > 0 ) && ( Slot2 <= 0 ) && ( l == 0 ) ) { // 2007.04.04
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
															//===================================================================================================================================
															// 2007.05.10
															//===================================================================================================================================
															if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( bmch , &i2 , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
																return -123;
															}
															if ( i != i2 ) return -124;
															//===================================================================================================================================
															*os = i;
															*os2 = 0;
														}
														else {
															return -113;
														}
													}
	/*
	// 2007.03.29
													else if ( ( Slot1 <= 0 ) && ( Slot2 > 0 ) && ( l == 1 ) ) {
														*os = 0;
														*os2 = 0;
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT( bmch + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
															*os2 = i * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
														}
														else {
															return -113;
														}
													}
	*/
													else {
														//=============================================================
														// 2007.05.03
														//=============================================================
														if ( ( Slot1 > 0 ) && ( l != 0 ) ) {
															return -114;
														}
														//=============================================================
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
															//===================================================================================================================================
															// 2007.05.10
															//===================================================================================================================================
															if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( bmch , &i2 , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
																return -125;
															}
															if ( i != i2 ) return -126;
															//===================================================================================================================================
															if ( l == 0 ) {
																*os = i;
																*os2 = 0;
															}
															else {
																*os2 = i;
																*os = 0;
															}
														}
													}
												}
												else {
													if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
														if ( l == 0 ) {
															*os = i;
															*os2 = 0;
														}
														else {
															*os2 = i;
															*os = 0;
														}
													}
												}
												return 1;
											}
											else {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-11 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
												ck = 0;
		//										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2005.12.22 // 2006.01.06
	//											if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.12.22 // 2006.01.06 // 2007.03.29
												if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2005.12.22 // 2006.01.06 // 2007.03.29
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-12 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
													//=======================================================================================
													// 2006.01.14
													//=======================================================================================
													s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( bmch );
													//
													if ( *bmchamber != ( bmch + s1p ) ) {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-13 RESULT = [x3=%d][l=%d] [%d]\n" , x3 , l , s1p );
//----------------------------------------------------------------------
														if ( ( bbb == ( bmch + s1p ) ) && !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2017.04.27
															//
															ck = 10;
															//
															errres = errres + 10;
															//
														}
														else {
															if ( ( pr != 0 ) && !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.01.24
																ck = 11;
																//
																errres = errres + 10; // 2010.12.09
																//
															}
		//													else if ( ( pr == 1 ) || !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.01.24 // 2006.11.10
															else if ( (!CMFirst) || ( pr == 1 ) || !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.01.24 // 2006.11.10
																if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch + s1p ) == BUFFER_TM_STATION ) {
																	if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( bmch + s1p , FALSE ) ) {
																		ck = 100;
																		//
																		errres = errres + 100; // 2010.12.09
																		//
																	}
																	else {
																		//
																		errres = errres + 1000; // 2010.12.09
																		//
																	}
																}
																else {
																	//
																	errres = errres + 10000; // 2010.12.09
																	//
																	switch( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( bmch ) ) { // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
																	case 0 :
																		switch( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( bmch + s1p ) ) { // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
																		case 0 : // [2][4]
																			if ( s1p == 1 ) {
																				if ( l == 0 ) {
																					if ( op < 2 ) ck = 2;
																				}
																				else {
																					if ( op < 4 ) ck = 4;
																				}
																			}
																			break;
																		case 1 : // [9]
																			if ( op < 9 ) ck = 9;
																			break;
																		case 3 : // [7]
																			if ( op < 7 ) ck = 7;
																			break;
																		}
																		break;
																	case 2 :
																		switch( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( bmch + s1p ) ) { // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
																		case 0 : // [6][8]
																			if ( s1p == 1 ) {
																				if ( l == 0 ) {
																					if ( op < 6 ) ck = 6;
																				}
																				else {
																					if ( op < 8 ) ck = 8;
																				}
																			}
																			break;
																		case 1 : // [5][6]
																			if ( l == 0 ) {
																				if ( op < 5 ) ck = 5;
																			}
																			else {
																				if ( op < 6 ) ck = 6;
																			}
																			break;
																		case 2 : // [8][x]
																			if ( l == 0 ) {
																				if ( s1p == 1 ) {
																					if ( op < 8 ) ck = 8;
																				}
																			}
																			break;
																		case 3 : // [4][5]
																			if ( l == 0 ) {
																				if ( op < 4 ) ck = 4;
																			}
																			else {
																				if ( op < 5 ) ck = 5;
																			}
																			break;
																		}
																		break;
																	}
																}
															}
														}
														//=======================================================================================
														if ( ck != 0 ) op = ck;
														//=======================================================================================
													}
												}
												else {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-14 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
													if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.04.04
														if ( Slot1 > 0 ) return -114; // 2007.04.04
														ck = 1;
													}
													else {
														if (
															( RemainTime < 0 ) ||
															( RemainTime > _SCH_TIMER_GET_PROCESS_TIME_START( 0 , trg ) ) ||
															( ( RemainTime == _SCH_TIMER_GET_PROCESS_TIME_START( 0 , trg ) ) && ( trg < ( *bmchamber - BM1 + PM1 ) ) )
															) {
															ck = 1;
														}
													}
												}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-14-2 RESULT = [x3=%d][l=%d] [ck=%d,%d] [f=%d]\n" , x3 , l , ck , f , op );
//----------------------------------------------------------------------
												if ( ck != 0 ) {
													//
													errres = errres + 10000; // 2010.12.09
													//
													if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) || ( f == 0 ) ) { // 2007.04.03
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-14-3 RESULT = [x3=%d][l=%d] [ck=%d] [f=%d]\n" , x3 , l , ck , f );
//----------------------------------------------------------------------
														f = 1; // 2006.01.14
														//
														RemainTime = _SCH_TIMER_GET_PROCESS_TIME_START( 0 , trg );
														//
														*bmchamber = bmch;
														pr = SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ); // 2006.01.23
														//
														if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
															if ( l == 0 ) {
																*os = i;
																*os2 = 0;
															}
															else {
																*os2 = i;
																*os = 0;
															}
														}
														else { // 2007.04.09
															if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
																f = 0; // 2007.04.09
																RemainTime = -1; // 2007.04.09
																*bmchamber = -1; // 2007.04.09
																pr = -1; // 2007.04.09
															}
														}
														if ( ck == 100 ) break;
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
			else {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-15 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
				if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_GO_WHERE( Pointer , &trg ) ) {
					if ( trg > 0 ) {
						bmch = trg - PM1 + BM1;
	//					if ( (!CMFirst) || !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.05.26
	//						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_FM_STATION ) ) { // 2006.05.26
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_FM_STATION ) ) { // 2006.05.26
							if ( (!CMFirst) || !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( bmch ) ) { // 2006.05.26
								if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( bmch , 1 ) ) {
									f = 1;
									*bmchamber = bmch;
									if ( SCHEDULER_CONTROL_Chk_BM_FREE_ONESLOT_for_STYLE_4_Part( bmch , &i , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( bmch - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
										if ( l == 0 ) {
											*os = i;
											*os2 = 0;
										}
										else {
											*os2 = i;
											*os = 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if ( f == 1 ) {
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-16 RESULT = [x3=%d][l=%d]\n" , x3 , l );
//----------------------------------------------------------------------
			//=============================================================
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.04.04
				if ( x3 == 0 ) {
					SCHEDULER_BM_PROCESS_MONITOR_ONEBODY( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() );
					continue; // 2007.04.04
				}
			} // 2007.04.04
			else if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2007.05.17
				//=============================================================
				// 2007.05.17
				//=============================================================
				if ( x3 == 0 ) {
					SCHEDULER_BM_PROCESS_MONITOR_ONEBODY( _SCH_PRM_GET_DFIM_MAX_PM_COUNT() );
					continue;
				}
				//=============================================================
			}
			//=============================================================
			return 1;
		}
		else { // 2007.04.04
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP 13-3-17 RESULT = [x3=%d][l=%d][%d,%d][%d,%d]\n" , x3 , l , Slot1 , Pointer1 , Slot2 , Pointer2 );
//----------------------------------------------------------------------
//			return -1; // 2007.04.04
			if ( errres <= 0 ) {
				return -2; // 2007.04.04 // 2010.12.09
			}
			else {
				return -errres; // 2007.04.04 // 2010.12.09
			}
		} // 2007.04.04
	}
	return -1;
}


int SCHEDULING_Possible_BM_for_STYLE2_4( int side , int Slot1 , int Slot2 , int Pointer1 , int Pointer2 ) {
	int m , trg , bmch , CK , s1p , l;
	int Slot , Pointer;
	//
	for ( l = 0 ; l < 2 ; l++ ) {
		if ( l == 0 ) {
			Slot = Slot1;
			Pointer = Pointer1;
		}
		else {
			Slot = Slot2;
			Pointer = Pointer2;
		}
		//=============================================================
		CK = FALSE;
		//=============================================================
		if ( Slot <= 0 ) continue;
		if ( Pointer >= 100 ) continue;
		//=============================================================
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			trg = _SCH_CLUSTER_Get_PathProcessChamber( side , Pointer , 0 , m );
			if ( trg > 0 ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side ) ) trg = -1;
			}
			if ( trg > 0 ) {
				bmch = trg - PM1 + BM1;
				if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_FM_STATION ) ) {
					if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( bmch , 1 ) ) {
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) {
//								s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( bmch + s1p ); // 2006.10.13
							s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( bmch ); // 2006.10.13
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( bmch + s1p ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch + s1p ) == BUFFER_FM_STATION ) ) {
								if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( bmch + s1p , 1 ) ) {
									CK = TRUE;
									break;
								}
							}
						}
						else {
							CK = TRUE;
							break;
						}
					}
				}
			}
		}
		if ( !CK ) return -1;
	}
	return 1;
}

int SCHEDULING_Possible_BM_for_STYLE3_4( int side , int Slot , int Slot2 , int Pointer , int Pointer2 ) {
	int i , j , m , pt , rcm , l , FM_Buffer , RemainTime , tdata , adata;
	int bc[MAX_CHAMBER];

	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) bc[i] = FALSE;

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &rcm , &pt , -1 ) ) {
			switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) ) {
			case 0 :
			case 1 :
			case 3 :
				if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) { // 2006.11.16
					if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) != 1 ) || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.16
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							m = _SCH_CLUSTER_Get_PathProcessChamber( i , pt , 0 , j );
							if ( m > 0 ) {
								if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , i ) ) {
									if ( _SCH_MODULE_GET_USE_ENABLE( m - PM1 + BM1 , FALSE , i ) ) {
										bc[m - PM1 + BM1] = TRUE;
									}
								}
							}
						}
					}
				}
				break;
			}
			//
			switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) ) {
			case 0 :
			case 2 :
			case 3 :
				if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) { // 2006.11.16
					if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) != 2 ) || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.16
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							m = _SCH_CLUSTER_Get_PathProcessChamber( i , pt , 0 , j );
							if ( m > 0 ) {
								if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , i ) ) {
									if ( _SCH_MODULE_GET_USE_ENABLE( m - PM1 + BM1 , FALSE , i ) ) {
										bc[m - PM1 + BM1] = TRUE;
									}
								}
							}
						}
					}
				}
				break;
			}
		}
	}

	RemainTime = 99999999;
	FM_Buffer = -1;

	for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
		if ( !bc[m] ) continue;
		if ( _SCH_MODULE_GET_USE_ENABLE( m - BM1 + PM1 , FALSE , side ) ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
				//======================================================================
				if ( ( _SCH_MODULE_Get_BM_FULL_MODE( m ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( m ) == BUFFER_FM_STATION ) ) { // 2006.11.17
					adata = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , m );
					if ( SCHEDULER_CONTROL_Chk_BM_IN_YES_4( m ) ) {
						//
						_SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( m - BM1 + PM1 , TRUE , &tdata , -1 );
						//
						if ( tdata > _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , m ) ) {
							adata = adata + tdata;
						}
						else {
							adata = adata + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , m );
						}
						adata = adata + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , m );
					}
				}
				//======================================================================
				else {
					if ( SCHEDULER_CONTROL_Chk_BM_IN_YES_4( m ) || SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( m ) ) {
						_SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( m - BM1 + PM1 , TRUE , &tdata , -1 );
						//
						if ( tdata > _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , m ) ) {
							adata = tdata;
						}
						else {
							adata = _SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , m );
						}
						//
						adata = adata + _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , m );
					}
					else {
						adata = _SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , m );
					}
				}
				//======================================================================
				if ( RemainTime > adata ) {
					RemainTime = adata;
					FM_Buffer  = m;
				}
				//======================================================================
			}
		}
	}

	if ( FM_Buffer == -1 ) return -1;

	if ( ( Slot > 0 ) && ( Pointer < 100 ) ) {
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			l = _SCH_CLUSTER_Get_PathProcessChamber( side , Pointer , 0 , m );
			if ( l > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( l , FALSE , side ) ) {
					if ( l - PM1 + BM1 == FM_Buffer ) return 1;
				}
			}
		}
	}
	if ( ( Slot2 > 0 ) && ( Pointer2 < 100 ) ) {
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			l = _SCH_CLUSTER_Get_PathProcessChamber( side , Pointer2 , 0 , m );
			if ( l > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( l , FALSE , side ) ) {
					if ( l - PM1 + BM1 == FM_Buffer ) return 2;
				}
			}
		}
	}
	return -1;
}

BOOL SCHEDULING_Possible_Pump_BM_for_STYLE_4( int side , int bmch ) {
	int s , w = 0 , chk;
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) == BUFFER_TM_STATION ) return FALSE;

	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_OUTWAIT_STATUS ) > 0 ) return FALSE;
	w = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( bmch , BUFFER_INWAIT_STATUS );
	//
	if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( bmch ) ) w++; // 2010.11.29
	//
	if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( bmch - BM1 + PM1 ) ) {
		return FALSE;
	}

	if ( ( sch4_RERUN_END_S2_TAG[ bmch - BM1 + PM1 ] != -1 ) && ( sch4_RERUN_FST_S2_TAG[ bmch - BM1 + PM1 ] != -1 ) ) { // 2005.07.30
		chk = TRUE; // 2006.11.16
	} // 2006.11.16
	else { // 2006.11.16
		if ( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() <= 0 ) chk = TRUE; // 2006.11.16
		else											  chk = FALSE; // 2006.11.16
	} // 2006.11.16
	if ( chk ) { // 2006.11.16
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			if ( !_SCH_SUB_Simple_Supply_Possible( s ) ) continue; // 2006.11.30
			if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( s , 0 , bmch , -1 , -1 ) ) return FALSE; // 2006.11.30
		}
	}
	if ( w == 0 ) {
		if ( _SCH_MODULE_Get_PM_WAFER( bmch - BM1 + PM1 , 0 ) <= 0 ) {
			return FALSE;
		}
		else {
			if ( !_SCH_MODULE_GET_USE_ENABLE( bmch - BM1 + PM1 , FALSE , side ) ) { // 2003.08.11
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL SCHEDULING_Possible_Pump_BM2_for_STYLE_4( int side , int *bmch ) {
	int l , s , w = 0;
	for ( l = 0 ; l < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; l++ ) {
		if ( _SCH_MODULE_Get_PM_WAFER( l + PM1 , 0 ) <= 0 ) continue;
		if ( _SCH_MODULE_Get_TM_WAFER( l , 0 ) > 0 ) continue;
		if ( _SCH_MODULE_Get_TM_WAFER( l , 1 ) > 0 ) continue; // 2005.06.28
		if ( _SCH_MODULE_Get_BM_FULL_MODE( l + BM1 ) == BUFFER_TM_STATION ) continue;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( l + BM1 , -1 ) > 0 ) continue;
		w = 0;
		if ( SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN( l + PM1 ) ) continue;
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			if ( !_SCH_SUB_Simple_Supply_Possible( s ) ) continue; // 2006.11.30
			if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( s , 0 , l + BM1 , -1 , -1 ) ) w = 1; // 2006.11.30
		}
		if ( w == 1 ) continue;
		*bmch = l + BM1;
		return TRUE;
	}
	return FALSE;
}

BOOL SCHEDULING_Possible_TargetSpace_for_STYLE_4( int side , int pointer , int arm , BOOL ALPICKMODE ) { // 2006.11.15
	int i , m , ok;
	//===============================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) return TRUE; // 2006.11.22
	//===============================================================
	if ( !ALPICKMODE ) {
		if ( _SCH_MODULE_Need_Do_Multi_FAL() ) return TRUE;
	}
	else {
		if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return TRUE;
	}
	//===============================================================
	ok = TRUE;
	//===============================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( m - PM1 + BM1 , FALSE , side ) ) {
					//=====================================================================
//					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( m - PM1 + BM1 ) == 0 ) continue; // 2006.11.16
					//=====================================================================
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( m - PM1 + BM1 , BUFFER_INWAIT_STATUS ) <= 0 ) {
						//
						if ( !SCHEDULER_TM_HAS_WAFER_SUPPLY( m - PM1 + BM1 ) ) { // 2010.11.29
							//
							return TRUE;
						}
						//
					}
					//=====================================================================
					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( m - PM1 + BM1 ) == ( arm + 1 ) ) {
						ok = FALSE;
					}
					else { // 2006.11.16
						if ( arm == TA_STATION ) {
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
								ok = FALSE;
							}
						}
						else {
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
								ok = FALSE;
							}
						}
					}
					//=====================================================================
				}
			}
		}
	}
	return ok;
}

BOOL SCHEDULING_Possible_TargetSpace2_for_STYLE_4( int side , int pointer ) { // 2006.11.24
	int i , m;
	//===============================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) return TRUE;
	//===============================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , 0 , i );
		if ( m > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( m , FALSE , side ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( m - PM1 + BM1 , FALSE , side ) ) {
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( m - PM1 + BM1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( m - PM1 + BM1 ) == BUFFER_FM_STATION ) ) {
						//=====================================================================
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( m - PM1 + BM1 , BUFFER_INWAIT_STATUS ) <= 0 ) {
							//
							if ( !SCHEDULER_TM_HAS_WAFER_SUPPLY( m - PM1 + BM1 ) ) { // 2010.11.29
								//
								return TRUE;
								//
							}
							//
						}
						//=====================================================================
					}
				}
			}
		}
	}
	return FALSE;
}


int SCHEDULER_CONTROL_Get_END_FST_S2_TAG( int side , int *index ) { // 2005.10.25
	int i;
	for ( i = PM1 ; i < PM1 + MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( sch4_RERUN_END_S2_TAG[i] != -1 ) {
			if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , ( sch4_RERUN_END_S2_TAG[i] % 100 ) + 1 ) > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , side ) ) {
					if ( ( _SCH_MODULE_Get_BM_FULL_MODE( i - PM1 + BM1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i - PM1 + BM1 ) == BUFFER_FM_STATION ) ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( i - PM1 + BM1 , 1 ) ) {
							*index = i;
							return 1;
						}
					}
				}
			}
		}
		if ( sch4_RERUN_FST_S2_TAG[i] != -1 ) {
			if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_END_FORCE_S2_EXIST( i ) ) { // 2005.09.21
				if ( _SCH_IO_GET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (sch4_RERUN_FST_S2_TAG[i]%100) + 1 ) > 0 ) {
					if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , side ) ) {
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( i - PM1 + BM1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i - PM1 + BM1 ) == BUFFER_FM_STATION ) ) {
							if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( i - PM1 + BM1 , 1 ) ) {
								*index = i;
								return 2;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================
//=================================================================================================================================
int Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody( int side1 , int pt1 , int side2 , int pt2 ) { // 2006.10.13
	// 0 : 갈데없음
	// 1 : OneBody의 같은 BM에만 갈수있음
	// 2 : OneBody의 다른 BM에만 갈수있음
	// 3 : 다른 Body에만 갈수있음
	// 4 : 기타
	int k , m1 , m2 , trg , bmch , xbmch , osm;
	//
	osm = 0;

	for ( k = 0 ; k < 2 ; k++ ) {
		for ( m1 = 0 ; m1 < MAX_PM_CHAMBER_DEPTH ; m1++ ) {
			trg = _SCH_CLUSTER_Get_PathProcessChamber( side1 , pt1 , 0 , m1 );
			if ( trg > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side1 ) ) {
					bmch = trg - PM1 + BM1;
					//
					if ( _SCH_MODULE_GET_USE_ENABLE( bmch , FALSE , side1 ) ) { // 2006.10.18
						//
						if ( ( k != 0 ) || ( _SCH_MODULE_Get_BM_FULL_MODE( bmch ) != BUFFER_TM_STATION ) ) { // 2006.10.18
							//
							xbmch = SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( bmch );
							//
							for ( m2 = 0 ; m2 < MAX_PM_CHAMBER_DEPTH ; m2++ ) {
								//
								trg = _SCH_CLUSTER_Get_PathProcessChamber( side2 , pt2 , 0 , m2 );
								//
								if ( trg > 0 ) {
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side2 ) ) {
										//
										if ( _SCH_MODULE_GET_USE_ENABLE( trg - PM1 + BM1 , FALSE , side2 ) ) { // 2006.10.18
											//
											if ( ( k != 0 ) || ( _SCH_MODULE_Get_BM_FULL_MODE( trg - PM1 + BM1 ) != BUFFER_TM_STATION ) ) { // 2006.10.18
												if      ( ( trg - PM1 + BM1 ) == bmch ) {
													switch( osm ) {
													case 0 :	osm = 1;	break;
													case 1 :				break;
													case 2 :	osm = 4;	break;
													case 3 :	osm = 4;	break;
													}
												}
												else if ( ( trg - PM1 + BM1 ) == xbmch ) {
													switch( osm ) {
													case 0 :	osm = 2;	break;
													case 1 :	osm = 4;	break;
													case 2 :				break;
													case 3 :	osm = 4;	break;
													}
												}
												else {
													switch( osm ) {
													case 0 :	osm = 3;	break;
													case 1 :	osm = 4;	break;
													case 2 :	osm = 4;	break;
													case 3 :				break;
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
		//================
		if ( k == 0 ) {
			if ( osm != 0 ) break;
		}
		//================
	}
	return osm;
}
//
BOOL Scheduling_Check_Waiting_for_BM_Action( int bmcheck ) {
	int i , o;
	if ( ( bmcheck == 2 ) || ( bmcheck == 3 ) ) {
		//=============================================================================================
		// 2007.04.27
		//=============================================================================================
		o = 0;
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + i , BUFFER_OUTWAIT_STATUS ) > 0 ) o++;
			if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + i ) != BUFFER_TM_STATION ) {
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , TRUE ) ) return FALSE;
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , FALSE ) ) return FALSE;
			}
		}
		//
		if ( bmcheck == 3 ) {
			return TRUE;
		}
		else {
			if ( o != 0 ) return TRUE;
		}
		//=============================================================================================
	}
	else {
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + i ) != BUFFER_TM_STATION ) {
				if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , TRUE ) ) {
	//				if ( SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( BM1 + i , 0 ) ) return TRUE;
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + i , BUFFER_OUTWAIT_STATUS ) > 0 ) {
						//=======================================================================================================
						if ( bmcheck == 0 )	return TRUE;
						//=======================================================================================================
						if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) return TRUE;
						if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) return TRUE;
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + i , BUFFER_INWAIT_STATUS ) > 0 ) return TRUE;
						//
						if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( BM1 + i ) ) return TRUE; // 2010.11.29
						//
						if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + i ) <= 0 ) return TRUE;
						//=======================================================================================================
					}
				}
			}
	//		else {
	//			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , FALSE ) ) return TRUE;
	//		}
		}
	}
	return FALSE;
}
//
BOOL Scheduling_Check_Possible_for_BM_Place( BOOL all ) { // 2007.04.26
	int i;
	if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) return FALSE;
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + i ) == BUFFER_TM_STATION ) return FALSE;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , FALSE ) ) return FALSE;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , TRUE ) ) return FALSE;
		if ( all ) {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + i , -1 ) > 0 ) return FALSE;
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + i , BUFFER_INWAIT_STATUS ) > 0 ) return FALSE;
			//
			if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( BM1 + i ) ) return FALSE; // 2010.11.29
			//
		}
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( BM1 + i ) ) return FALSE;
	}
	//========================================================================================================================
	if ( !all ) {
		if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , TRUE  , -1 ) != 0 ) return TRUE;
		if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , FALSE , -1 ) != 0 ) return TRUE;
		return FALSE;
	}
	//========================================================================================================================
	return TRUE;
}
//
BOOL Scheduling_Check_Possible_for_BM_Place2_NO_Align( int side , int pt ) { // 2007.08.22
	int i , m , trg;
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( !_SCH_MODULE_GET_USE_ENABLE( BM1 + i , FALSE , side ) ) continue;
		if ( !_SCH_MODULE_GET_USE_ENABLE( PM1 + i , FALSE , side ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + i ) == BUFFER_TM_STATION ) continue;
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( BM1 + i ) ) continue;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , FALSE ) ) continue;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , TRUE ) ) continue;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + i , BUFFER_INWAIT_STATUS ) > 0 ) continue;
		//
		if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( BM1 + i ) ) continue; // 2010.11.29
		//
		//========================================================================================================================
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			trg = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , m );
			if ( trg == ( PM1 + i ) ) return TRUE;
		}
		//========================================================================================================================
	}
	return FALSE;
}
//
BOOL Scheduling_Check_Two_is_Compatable_OneBody3_Sub( int side , int pt , int bmc , int all , BOOL mdlcheckskip ) { // 2007.04.05
	int m , trg , btr , skipchk;
	//
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		trg = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , m );
		//=====================================================================================
		// 2007.05.10
		//=====================================================================================
		skipchk = FALSE;
		if ( mdlcheckskip ) {
			if ( trg < 0 ) {
				if ( !_SCH_MODULE_GET_USE_ENABLE( -trg , FALSE , side ) ) {
					btr = -trg - PM1 + BM1;
					if ( _SCH_MODULE_GET_USE_ENABLE( btr , FALSE , side ) ) {
						if ( btr == bmc ) {
							trg = -trg;
							skipchk = TRUE;
						}
					}
				}
			}
		}
		//=====================================================================================
		if ( trg > 0 ) {
//			if ( _SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side ) ) { // 2007.05.10
			if ( _SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side ) || skipchk ) { // 2007.05.10
				btr = trg - PM1 + BM1;
				if ( _SCH_MODULE_GET_USE_ENABLE( btr , FALSE , side ) ) {
					if ( btr == bmc ) {
//						if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) { // 2007.04.19
						if ( ( all != 3 ) && SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) { // 2007.04.19
							if ( all == 2 ) {
								if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , bmc - BM1 + 1 ) <= 0 ) {
									return TRUE;
								}
								else {
									//=========================================================================================================
									// 2007.09.06
									//=========================================================================================================
									if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , bmc - BM1 + 1 + SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) <= 0 ) {
										if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
											return TRUE;
										}
									}
									//=========================================================================================================
								}
							}
							else {
								if ( SCHEDULER_ALIGN_BUFFER_CONTROL_PICK_FM_POSSIBLE( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
									return TRUE;
								}
								else {
									if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , bmc - BM1 + 1 ) <= 0 ) {
										return TRUE;
									}
									else {
										if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , bmc - BM1 + 1 + SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) <= 0 ) {
											if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
												return TRUE;
											}
										}
									}
								}
							}
						}
						else {
							//=============================================================================================
							if ( all == 1 ) {
								if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( btr , FALSE ) ) {
									if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_SUPPLIED_FOR_4( btr , TRUE ) ) {
										//=========================================================================
										// 2007.04.12
										//=========================================================================
										if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( btr ) , side , pt ) ) {
											return TRUE; // 2007.04.12
										}
										//=========================================================================
									}
								}
							}
							else if ( all == 3 ) { // 2007.04.19
								if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( btr , FALSE ) ) {
									if ( _SCH_MODULE_Get_BM_FULL_MODE( btr ) == BUFFER_TM_STATION ) {
										//=========================================================================
										// 2007.04.12
										//=========================================================================
										if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( btr ) , side , pt ) ) {
											return TRUE; // 2007.04.12
										}
										//=========================================================================
									}
									if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_SUPPLIED_FOR_4( btr , FALSE ) ) {
										//=========================================================================
										// 2007.04.12
										//=========================================================================
										if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( btr ) , side , pt ) ) {
											return TRUE; // 2007.04.12
										}
										//=========================================================================
									}
								}
							}
							else {
								if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( btr , FALSE ) ) {
									//=========================================================================
									// 2007.04.12
									//=========================================================================
									if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( btr ) , side , pt ) ) {
										return TRUE; // 2007.04.12
									}
									//=========================================================================
								}
/*
								if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( btr , FALSE ) ) {
									//=========================================================================
									// 2007.09.19
									//=========================================================================
									if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_SUPPLIED_FOR_4( btr , FALSE ) ) { // 2007.09.19
										//=========================================================================
										// 2007.04.12
										//=========================================================================
										if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( btr ) , side , pt ) ) {
											return TRUE; // 2007.04.12
										}
										//=========================================================================
									}
								}
								else { // 2007.09.19
									//=========================================================================
									// 2007.09.19
									//=========================================================================
									if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_SUPPLIED_FOR_4( btr , FALSE ) ) { // 2007.09.19
										//=========================================================================
										// 2007.04.12
										//=========================================================================
										if ( !SCHEDULER_CONTROL_Chk_BM_ALREADY_INVALID_SUPPLIED_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( btr ) , side , pt ) ) {
											return TRUE; // 2007.04.12
										}
										//=========================================================================
									}
								}
*/
							}
							//=============================================================================================
						}
					}
				}
			}
		}
	}
	return FALSE;
}

int Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody3( int side1 , int pt1 , int side2 , int slot2 , int pt2 , int cldxcheck ) { // 2007.03.29
	// 0 : OK
	// 1 : A만
	// 2 : B만
	// -1: xx
	int i , p , pl , sl;
	//
	//======================================================================================================
	p = pt1;
	pl = _SCH_CLUSTER_Get_PathIn( side1 , pt1 );
	sl = _SCH_CLUSTER_Get_SlotIn( side1 , pt1 );
	//======================================================================================================
	if ( cldxcheck != 5 ) {
		if ( cldxcheck != 0 ) {
			if ( slot2 > 0 ) {
				if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side1 , pt1 , side2 , pt2 ) ) return -1; // 2007.05.28
			}
		}
		//======================================================================================================
		if ( cldxcheck != 2 ) {
			for ( i = 0 ; i < ( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) ; i++ ) {
				//================================================================================
				while (
					( _SCH_CLUSTER_Get_PathRange( side1 , p ) < 0 ) ||
					( _SCH_CLUSTER_Get_PathStatus( side1 , p ) != SCHEDULER_READY ) ||
					( _SCH_CLUSTER_Get_PathIn( side1 , p ) >= PM1 )
					) {
					p++;
					if ( p >= MAX_CASSETTE_SLOT_SIZE ) break;
				}
				if ( p >= MAX_CASSETTE_SLOT_SIZE ) break;
				//================================================================================
				// 2007.04.02
				//================================================================================
				if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side1 , pt1 , side1 , p ) ) break; // 2007.05.28
				//================================================================================
				if ( pl != _SCH_CLUSTER_Get_PathIn( side1 , p ) ) break;
				//================================================================================
				if ( ( sl + i ) != _SCH_CLUSTER_Get_SlotIn( side1 , p ) ) break;
				//================================================================================
	//			if ( ( cldxcheck == 1 ) && SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) { // 2007.04.26 // 2007.04.26
	//				if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , TRUE , -1 ) != 0 ) break; // 2007.04.26 // 2007.04.26
	//			} // 2007.04.26
	//			else { // 2007.04.26
				if ( !Scheduling_Check_Two_is_Compatable_OneBody3_Sub( side1 , p , ( cldxcheck == 4 ) ? i + BM1 + 1 : i + BM1 , ( cldxcheck != 0 ) ? 2 : 0 , FALSE ) ) break;
	//			} // 2007.04.26
				//================================================================================
				p++;
			}
		}
		else { // 2007.04.26
			i = ( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
		}
		//======================================================================================================
		if ( i == ( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) ) {
			if ( slot2 > 0 ) {
				if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side1 , pt1 , side2 , pt2 ) ) { // 2007.05.28
					return 1; // A
				}
				else {
					//=================================================================================================
					// 2007.04.26
					//=================================================================================================
					if ( ( cldxcheck == 2 ) && SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
						return 0;
					}
					//=================================================================================================
					if ( cldxcheck == 4 ) { // 2008.02.11
						return 0; // AB
					}
					else {
						if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( side2 , pt2 , i + BM1 , ( cldxcheck != 0 ) ? 2 : 0 , FALSE ) ) return 0; // AB
					}
					//=================================================================================================
				}
			}
			return 1; // A
		}
		//======================================================================================================
		if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) && ( cldxcheck != 0 ) ) {
			for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
				if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( side1 , pt1 , BM1 + i , 2 , FALSE ) ) return 2; // B
			}
		}
		else {
			for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
				if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( side1 , pt1 , BM1 + i , 1 , FALSE ) ) return 2; // B
			}
		}
	}
	else { // 2008.05.07
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( side1 , pt1 , BM1 + i , 1 , FALSE ) ) return 2; // B
		}
	}
	//======================================================================================================
	return -1; // xx
	//======================================================================================================
}
//=========================================================================================================
BOOL Scheduling_Check_Single_Arm_BM_Explict_Check( int bm , int s , int p0 ) {
	int i , j , otherbm , p , Res;
	//
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return FALSE;
	//
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( FALSE ) != 2 ) return FALSE;
	//
	otherbm = ( ( ( bm - BM1 ) % 2 ) == 0 ) ? bm + 1 : bm - 1;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( s == -1 ) {
			if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		}
		else {
			if ( s != i ) continue;
		}
		//
		if ( p0 == -1 ) {
			//
			if ( !_SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &j , &p , -1 ) ) continue;
			//
		}
		else {
			p = p0;
		}
		//
		Res = FALSE;
		//
		for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( otherbm ) ; j++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( otherbm , j ) <= 0 ) continue;
			if ( _SCH_MODULE_Get_BM_STATUS( otherbm , j ) != BUFFER_INWAIT_STATUS ) continue;
			if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( i , p , _SCH_MODULE_Get_BM_SIDE( otherbm , j ) , _SCH_MODULE_Get_BM_POINTER( otherbm , j ) ) ) {
				Res = TRUE;
				break;
			}
		}
		//
		if ( !Res ) return FALSE;
		//
	}
	//
	return TRUE;
}
//=========================================================================================================

int SCHEDULER_ALIGN_BUFFER_PLACE_SLOT_CHECK_WITH_ARM_B( int side , int pt ) {
	int i;
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( side , pt , BM1 + i , 2 , FALSE ) ) return i+1; // 2007.04.24
	}
	return 0;
}

//=========================================================================================================

BOOL SCHEDULER_ALIGN_BUFFER_PLACE_IMPOSSIBLE( int side , int pt ) {
	int trg , m;
	//
	if ( SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) == 0 ) { // 2007.10.31
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) return TRUE;
	}
	else {
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) return FALSE;
	}
	//
	for ( m = 0 ; m < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ; m++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , m + 1 ) > 0 ) {
			if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side , pt , _SCH_MODULE_Get_BM_SIDE( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , m + 1 ) , _SCH_MODULE_Get_BM_POINTER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , m + 1 ) ) ) return TRUE; // 2007.05.28
		}
	}
	//
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		trg = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , m );
		if ( trg > 0 ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( trg , FALSE , side ) ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( trg - PM1 + BM1 , FALSE , side ) ) {
					if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , trg - PM1 + 1 ) <= 0 ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}

//=========================================================================================================
int SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( int ch , BOOL highmode , int side ) {
	int i , offset;
	//
	if ( highmode ) offset = SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( ch );
	else            offset = 0;
	//
	for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , offset + i + 1 ) > 0 ) {
			if ( side != -1 ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , offset + i + 1 ) != side ) return 2;
			}
			break;
		}
	}
	if ( i != ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ) return 1;
	return 0;
}
//=========================================================================================================
BOOL SCHEDULER_ALIGN_BUFFER_CONTROL_MOVE_LOW2HIGH( int ch ) {
	int i , src , sts;
	//============================================================================================
	// 2007.03.23
	//============================================================================================
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ch ) < 0 ) return FALSE;
	//============================================================================================
	if ( SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( ch ) != 0 ) { // 2007.10.30
		for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
				_SCH_MODULE_Set_BM_SIDE_POINTER( ch , ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) + i + 1 , _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) , _SCH_MODULE_Get_BM_POINTER( ch , i + 1 ) );
				_SCH_MODULE_Set_BM_WAFER_STATUS( ch , ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) + i + 1 , _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) , BUFFER_INWAIT_STATUS );
				//
				_SCH_MODULE_Set_BM_WAFER_STATUS( ch , i + 1 , 0 , BUFFER_READY_STATUS );
			}
			sts = _SCH_IO_GET_MODULE_STATUS( ch , i + 1 );
			if ( sts > 0 ) {
				src = _SCH_IO_GET_MODULE_SOURCE( ch , i + 1 );
				//
				_SCH_IO_SET_MODULE_SOURCE( ch , ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) + i + 1 , src );
				_SCH_IO_SET_MODULE_STATUS( ch , ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) + i + 1 , sts );
				//
				_SCH_IO_SET_MODULE_SOURCE( ch , i + 1 , 0 );
				_SCH_IO_SET_MODULE_STATUS( ch , i + 1 , 0 );
			}
		}
		//============================================================================================
		SCHEDULER_ALIGN_BUFFER_SET_RUN( 0 );
		//============================================================================================
	}
	else {
		//============================================================================================
		SCHEDULER_ALIGN_BUFFER_SET_RUN( 2 ); // 2007.11.28
		//============================================================================================
	}
	//============================================================================================
//	SCHEDULER_ALIGN_BUFFER_SET_RUN( 0 ); // 2007.11.28
	//============================================================================================
	return TRUE;
}
//=========================================================================================================
BOOL SCHEDULER_ALIGN_BUFFER_CONTROL_PICK_FM_POSSIBLE( int ch ) { // 2007.04.10
	int i , l , f , offset;
	//============================================================================================
	for ( l = 0 ; l < 2 ; l++ ) {
		if ( l == 0 ) offset = 0;
		else          offset = SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( ch );
		f = FALSE;
		for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 + offset ) > 0 ) {
				f = TRUE;
				break;
			}
		}
		if ( !f ) return TRUE;
	}
	//============================================================================================
	return FALSE;
}
//=========================================================================================================
BOOL SCHEDULER_ALIGN_BUFFER_CONTROL_MONITORING_ERROR( int dal_tc ) { // 2007.04.26
	if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() != 1 ) return FALSE;
	if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( dal_tc ) ) return FALSE;
	if ( SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( dal_tc ) != 0 ) { // 2007.10.30
		if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , TRUE , -1 ) != 0 ) return TRUE;
	}
	if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_MOVE_LOW2HIGH( dal_tc ) ) return TRUE;
	return FALSE;
}
//=========================================================================================================
int SCHEDULER_ALIGN_BUFFER_PICK_FROM_ALBF( int dal_tc , int side , BOOL chkmode , int *sd1 , int *pt1 , int *sl1 , int *sd2 , int *pt2 , int *sl2 , BOOL coolcheck ) { // 2007.01.23
	int i , FM_CM , Offset;
	int sel1 , sel2;
	//
	*sl1 = 0; // 2007.04.25
	*sl2 = 0; // 2007.04.25
	//
	//============================================================================================================
	// 2007.11.28
	//============================================================================================================
	if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 2 ) { // 2007.11.28
		if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_4( dal_tc ) ) {
			SCHEDULER_ALIGN_BUFFER_SET_RUN( 0 );
		}
	}
	//============================================================================================================
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) return 1;
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) return 2;
	if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_4( dal_tc ) ) return 3;
	//
	if ( chkmode ) {
		if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 1 ) { // 2007.04.26
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( dal_tc ) ) return 4;
			//============================================================================================
			// 2007.03.21
			//============================================================================================
			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , FALSE , side ) == 2 ) return 5;
			//============================================================================================
//			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , TRUE , -1 ) == 0 ) { // 2007.04.11 // 2007.04.26
//				if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_MOVE_LOW2HIGH( dal_tc ) ) return -1; // 2007.04.26
//			} // 2007.04.26
			//============================================================================================
			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( dal_tc ) != 0 ) { // 2007.10.30
				if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , TRUE , -1 ) != 0 ) return -1; // 2007.04.26
			}
			if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_MOVE_LOW2HIGH( dal_tc ) ) return -1; // 2007.04.26
			//============================================================================================
			return 15; // 2007.04.26
		}
	}
	//=============================================================================================================
	if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , TRUE , side ) != 1 ) return 6; // 2007.04.27
	//=========================================================================
	Offset = SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( dal_tc );
	sel1 = 0; // 2007.04.25
	sel2 = 0; // 2007.04.25
	//=========================================================================
	if ( _SCH_MODULE_Get_BM_WAFER( dal_tc , Offset + 1 ) > 0 ) {
		if ( !Scheduling_Check_Two_is_Compatable_OneBody3_Sub( _SCH_MODULE_Get_BM_SIDE( dal_tc , Offset + 1 ) , _SCH_MODULE_Get_BM_POINTER( dal_tc , Offset + 1 ) , BM1 , 3 , TRUE ) ) {
			return 7; // 2007.04.23
		}
		//=====================================================================================
		// 2007.04.19
		//=====================================================================================
		for ( i = 0 ; i < _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( dal_tc , Offset + 1 + i + 1 ) > 0 ) {
				if ( !Scheduling_Check_Two_is_Compatable_OneBody3_Sub( _SCH_MODULE_Get_BM_SIDE( dal_tc , Offset + 1 + i + 1 ) , _SCH_MODULE_Get_BM_POINTER( dal_tc , Offset + 1 + i + 1 ) , BM1 + i + 1 , 3 , TRUE ) ) {
					return 8; // 2007.04.19 // 2007.04.23
				}
			}
		}
		//=====================================================================================
		sel1 = Offset + 1; // 2007.04.25
		//=====================================================================================

	}
	//=========================================================================
	if ( sel1 == 0 ) { // 2007.04.25
//		for ( i = 0 ; i < Offset ; i++ ) { // 2007.11.28
		for ( i = 0 ; i < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ); i++ ) { // 2007.11.28
			if ( _SCH_MODULE_Get_BM_WAFER( dal_tc , Offset + i + 1 ) > 0 ) {
				if ( !Scheduling_Check_Two_is_Compatable_OneBody3_Sub( _SCH_MODULE_Get_BM_SIDE( dal_tc , Offset + i + 1 ) , _SCH_MODULE_Get_BM_POINTER( dal_tc , Offset + i + 1 ) , BM1 + i , 3 , TRUE ) ) {
					return 9; // 2007.04.19 // 2007.04.23
				}
				//
				if ( sel2 == 0 ) sel2 = Offset + i + 1; // 2007.04.25
				//
			}
		}
	}
	else {
		if ( _SCH_MODULE_Get_BM_WAFER( dal_tc , Offset + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) > 0 ) {
			if ( !Scheduling_Check_Two_is_Compatable_OneBody3_Sub( _SCH_MODULE_Get_BM_SIDE( dal_tc , Offset + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) , _SCH_MODULE_Get_BM_POINTER( dal_tc , Offset + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) , BM1 + 1 + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) , 3 , TRUE ) ) { // 2007.04.19 // 2007.04.23
				return 10; // 2007.04.19 // 2007.04.23
			}
			//
			sel2 = Offset + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2;
		}
	}
	//============================================================================================
	// 2007.03.23
	//============================================================================================
	if ( _SCH_MACRO_CHECK_PROCESSING_INFO( dal_tc ) < 0 ) return -1;
	//============================================================================================
	if ( ( sel1 == 0 ) && ( sel2 == 0 ) ) return 11; // 2007.04.25
	//============================================================================================
	// 2007.05.10
	//============================================================================================
	if ( coolcheck ) {
		if ( sel1 != 0 ) {
			if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( TRUE , 0 ) ) return 12;
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( BM1 + sel2 - ( Offset + 1 ) , BUFFER_INWAIT_STATUS ) > 0 ) { // 2007.06.26
				if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( FALSE , sel2 ) ) return 13;
			}
			else { // 2007.06.26
				if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 + sel2 - ( Offset + 1 ) ) == BUFFER_TM_STATION ) { // 2007.06.26
					if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( FALSE , sel2 ) ) return 14;
				}
			}
		}
	}
	//============================================================================================
	// 2007.04.25
	//============================================================================================
	if ( sel1 > 0 ) {
		*sd1 = _SCH_MODULE_Get_BM_SIDE( dal_tc , sel1 );
		*pt1 = _SCH_MODULE_Get_BM_POINTER( dal_tc , sel1 );
		*sl1 = _SCH_MODULE_Get_BM_WAFER( dal_tc , sel1 );
		//
		_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , *pt1 , FMWFR_PICK_CM_DONE_AL );
		_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , *sd1 , *sl1 );
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( *sd1 , *pt1 );
		//
		_SCH_MODULE_Set_BM_WAFER_STATUS( dal_tc , sel1 , 0 , BUFFER_READY_STATUS );
		//
		//======================================================================================================
		_SCH_FMARMMULTI_DATA_SET_FROM_BM( 0 , dal_tc , sel1 , 0 , 1 ); // 2007.08.01
		//======================================================================================================
		_SCH_FMARMMULTI_FA_SUBSTRATE( dal_tc , sel1 , FA_SUBST_RUNNING ); // 2007.07.11
		//======================================================================================================
	}
	//
	if ( sel2 > 0 ) {
		*sd2 = _SCH_MODULE_Get_BM_SIDE( dal_tc , sel2 );
		*pt2 = _SCH_MODULE_Get_BM_POINTER( dal_tc , sel2 );
		*sl2 = _SCH_MODULE_Get_BM_WAFER( dal_tc , sel2 );
		//
		FM_CM = _SCH_CLUSTER_Get_PathIn( *sd2 , *pt2 );
		//
		_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , *pt2 , FMWFR_PICK_CM_DONE_AL );
		_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , *sd2 , *sl2 );
		//
		_SCH_MODULE_Set_BM_WAFER_STATUS( dal_tc , sel2 , 0 , BUFFER_READY_STATUS );
	}
	//============================================================================================
	if ( ( *sl1 == 0 ) && ( *sl2 == 0 ) ) return 15; // 2007.04.25
	//============================================================================================
	//-----------------------------------------------
	FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
	//-----------------------------------------------
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 210 , dal_tc , *sl1 , sel1 , *sd1 , *pt1 , dal_tc , *sl2 , sel2 , *sd2 , *pt2 , -1 ) ) {
	case -1 :
		return -1;
		break;
	}
	//=========================================================================
	// 2007.09.03
	//=========================================================================
	if ( ( *sl1 > 0 ) && ( *sl2 > 0 ) ) {
		FM_Has_Swap_Wafer_Style_4 = TRUE;
	}
	else {
		FM_Has_Swap_Wafer_Style_4 = FALSE;
	}
	//=========================================================================
	// 2007.04.11
	//=========================================================================
	if ( chkmode ) {
		if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , FALSE , -1 ) == 1 ) {
			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( dal_tc , TRUE , -1 ) == 0 ) { // 2007.10.30
				if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 1 ) { // 2007.04.26
					return -1;
				}
				else {
					_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , side , dal_tc , -1 , TRUE , 0 , 1002 );
					SCHEDULER_ALIGN_BUFFER_SET_RUN( 1 ); // 2007.04.26
				}
			}
		}
	}
	//============================================================================================
	return 999;
}
//=========================================================================================================
int SCHEDULER_ALIGN_BUFFER_PLACE_TO_ALBF( int dal_tc , int curside , int mode , int reset , int *oFM_dbSide , int *oFM_Pointer , int *oFM_Slot , int *oFM_dbSide2 , int *oFM_Pointer2 , int *oFM_Slot2 , int setrslot , BOOL bmupdate ) {
	int FM_dbSide , FM_Pointer , FM_Slot , FM_dbSide2 , FM_Pointer2 , FM_Slot2 , FM_RSlot , FM_RSlot2;
	if ( ( mode == 0 ) || ( mode == 1 ) ) {
		FM_dbSide  = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
		FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
		FM_Slot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
		if ( setrslot <= 0 ) {
			if ( FM_Slot > 0 ) FM_RSlot = 1;
			else               FM_RSlot = 0;
		}
		else {
			if ( FM_Slot > 0 ) FM_RSlot = setrslot;
			else               FM_RSlot = 0;
		}
	}
	else {
		FM_Slot = 0;
		FM_RSlot = 0;
	}
	//
	if ( ( mode == 0 ) || ( mode == 2 ) ) {
		FM_dbSide2  = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
		FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
		FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
		//
		if ( setrslot <= 0 ) {
			if      ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
				FM_RSlot2 = _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 + 1;
			}
			else if ( FM_Slot2 > 0 ) {
				FM_RSlot2 = 1;
			}
			else {
				FM_RSlot2 = 0;
			}
		}
		else {
			if ( FM_Slot2 > 0 ) FM_RSlot2 = setrslot;
			else                FM_RSlot2 = 0;
		}
	}
	else {
		FM_Slot2 = 0;
		FM_RSlot2 = 0;
	}
	//=============================================================================================
	if ( ( reset == 0 ) || ( reset == 1 ) ) {
		*oFM_dbSide  = FM_dbSide;
		*oFM_Pointer = FM_Pointer;
		*oFM_Slot    = FM_Slot;
	}
	if ( ( reset == 0 ) || ( reset == 2 ) ) {
		*oFM_dbSide2  = FM_dbSide2;
		*oFM_Pointer2 = FM_Pointer2;
		*oFM_Slot2    = FM_Slot2;
	}
	//=============================================================================================
	if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) return 0;
	//=============================================================================================
	//============================================================================================
	// 2007.03.23
	//============================================================================================
	if ( dal_tc != F_IC ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( dal_tc ) < 0 ) return -1;
	}
	//============================================================================================
	//-----------------------------------------------
	FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
	//-----------------------------------------------
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 210 , dal_tc , FM_Slot , FM_RSlot , FM_dbSide , FM_Pointer , dal_tc , FM_Slot2 , FM_RSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
	case -1 :
		return -1;
		break;
	}
	//=============================================================================================
	if ( FM_Slot > 0 ) {
		if ( bmupdate ) {
			_SCH_MODULE_Set_BM_SIDE_POINTER( dal_tc , FM_RSlot , FM_dbSide , FM_Pointer );
			_SCH_MODULE_Set_BM_WAFER_STATUS( dal_tc , FM_RSlot , FM_Slot , BUFFER_INWAIT_STATUS );
			//
			_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( dal_tc , FM_RSlot , 0 );
		}
		//----------------------------------------------------------------------
		_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 0 );
		//----------------------------------------------------------------------
	}
	if ( FM_Slot2 > 0 ) {
		if ( bmupdate ) {
			_SCH_MODULE_Set_BM_SIDE_POINTER( dal_tc , FM_RSlot2 , FM_dbSide2 , FM_Pointer2 );
			_SCH_MODULE_Set_BM_WAFER_STATUS( dal_tc , FM_RSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
		}
		//----------------------------------------------------------------------
		_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , 0 , 0 );
		//----------------------------------------------------------------------
	}
	//========================================================================
	// 2007.09.03
	//========================================================================
	if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
		if ( bmupdate ) {
			if ( !FM_Has_Swap_Wafer_Style_4 ) {
				SCHEDULER_SWAPPING_3_1_REMAP_BM_ONEBODY3( dal_tc , FM_RSlot );
			}
		}
	}
	//========================================================================
	return 1;
}


BOOL SCHEDULER_CONTROL_CLIDX_CHECK_TO_ALBM( int ch , int slot ) { // 2007.04.26
	int i , j;
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_INWAIT_STATUS ) {
					if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + i ) > 0 ) return FALSE; // 2007.04.27
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_BM_SIDE( ch , slot ) , _SCH_MODULE_Get_BM_POINTER( ch , slot ) , _SCH_MODULE_Get_BM_SIDE( i + BM1 , j ) , _SCH_MODULE_Get_BM_POINTER( i + BM1 , j ) ) ) return FALSE; // 2007.05.28
				}
			}
		}
	}
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( int side , int bmc , int albm_ch ) {
	int i , j , cldx_side , cldx_pt , xs , pt , li , retcm;
	int bc[MAX_BM_CHAMBER_DEPTH];
	//
	cldx_side = -1;
	xs = FALSE;
	//
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		bc[i] = 0;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) {
			for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_INWAIT_STATUS ) {
						cldx_side = _SCH_MODULE_Get_BM_SIDE( i + BM1 , j );
						cldx_pt = _SCH_MODULE_Get_BM_POINTER( i + BM1 , j );
						if ( bc[i] == 0 ) bc[i] = 1;
					}
					else {
						bc[i] = 2;
						xs = TRUE;
					}
				}
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( i + BM1 , FALSE ) ) { // 2007.04.23
				bc[i] = 3;
				for ( j = _SCH_PRM_GET_MODULE_SIZE( i + BM1 ) ; j >= 1 ; j-- ) {
					if ( _SCH_MODULE_Get_BM_WAFER( i + BM1 , j ) > 0 ) {
						if ( _SCH_MODULE_Get_BM_STATUS( i + BM1 , j ) == BUFFER_INWAIT_STATUS ) {
							cldx_side = _SCH_MODULE_Get_BM_SIDE( i + BM1 , j );
							cldx_pt = _SCH_MODULE_Get_BM_POINTER( i + BM1 , j );
						}
					}
				}
			}
			else {
				bc[i] = 1;
			}
		}
	}
	//
	if ( bc[bmc-BM1] == 1 ) return 1;
	if ( bc[bmc-BM1] == 3 ) return 10;
	if ( bc[bmc-BM1] == 2 ) return 11;
	//
	if ( xs ) return 12;
	//
	i = bmc-BM1; // 2007.04.24

//	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) { // 2007.04.24
//		if ( bmc != ( i + BM1 ) ) { // 2007.04.24
//			if ( !bc[i] ) { // 2007.04.24
				if ( albm_ch == -1 ) {
					//===================================================================================================================
					pt = -1;
					if ( !_SCH_SUB_FM_Current_No_More_Supply( side , &pt , i + PM1 , &retcm , cldx_side , cldx_pt ) ) {
						return 13;
					}
					if ( pt != -1 ) {
						if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( side , pt + 1 , i + BM1 , cldx_side , cldx_pt ) ) {
							return 14;
						}
					}
					//===================================================================================================================
					for ( li = 0 ; li < MAX_CASSETTE_SIDE ; li++ ) {
						if ( li != side ) {
							pt = -1;
							if ( !_SCH_SUB_FM_Current_No_More_Supply( li , &pt , i + PM1 , &retcm , cldx_side , cldx_pt ) ) {
								return 15;
							}
							if ( pt != -1 ) {
								if ( SCHEDULER_CONTROL_Chk_FM_OUT_WILL_GO_BM_FOR_4( li , pt + 1 , i + BM1 , cldx_side , cldx_pt ) ) {
									return 16;
								}
							}
						}
					}
					//===================================================================================================================
				}
				else {
					if ( albm_ch != F_IC ) { // 2007.04.19
						//=============================================================================
						// 2007.04.11
						//=============================================================================
						if ( _SCH_MODULE_Get_BM_WAFER( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) > 0 ) {
							if ( cldx_side == -1 ) { // 2007.04.26
								if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( _SCH_MODULE_Get_BM_SIDE( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) , _SCH_MODULE_Get_BM_POINTER( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) , BM1 + i , 3 , TRUE ) ) { // 2007.04.19
									if ( SCHEDULER_CONTROL_CLIDX_CHECK_TO_ALBM( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + i ) ) { // 2007.04.27
										return 17;
									}
								}
							}
							else {
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( cldx_side , cldx_pt , _SCH_MODULE_Get_BM_SIDE( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) , _SCH_MODULE_Get_BM_POINTER( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) ) ) { // 2007.05.28
									if ( Scheduling_Check_Two_is_Compatable_OneBody3_Sub( _SCH_MODULE_Get_BM_SIDE( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) , _SCH_MODULE_Get_BM_POINTER( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + i + 1 ) , BM1 + i , 3 , TRUE ) ) { // 2007.04.19
										if ( SCHEDULER_CONTROL_CLIDX_CHECK_TO_ALBM( albm_ch , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + i ) ) { // 2007.04.27
											return 17;
										}
									}
								}
							}
						}
						//=============================================================================
					}
				}
//			} // 2007.04.24
//		} // 2007.04.24
//	} // 2007.04.24
	return 0;
}
//===============================================================================================================
int SCHEDULER_CONTROL_Chk_AFTER_PLACE_BM_AND_PUMP_FOR_4( int side , int lastpumpingbmc , int slot1 , int slot2 , int logoffset ) {
	int x , i , res = 0 , select;
	//==================================================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) {
		//==================================================================================
		return SCHEDULER_CONTROL_BM_PUMPING_FM_PART( side , TRUE , lastpumpingbmc , logoffset );
		//==================================================================================
	}
	//==================================================================================
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( lastpumpingbmc != ( i + BM1 ) ) {
			while( TRUE ) {
				if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return -1;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) <= 0 ) break;
				Sleep(1);
			}
		}
	}
	//==================================================================================
	// 2007.04.25
	//==================================================================================
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_ALL_FREE_FOR_4( BM1 + i ) ) break;
	}
	if ( i == SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ) return 0;
	//==================================================================================
	 for ( x = 0 ; x < 2 ; x++ ) {
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			if ( x == 0 ) {
				if ( ( i + BM1 ) != lastpumpingbmc ) continue;
			}
			else {
				if ( ( i + BM1 ) == lastpumpingbmc ) continue;
			}
			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( FALSE ) ) {
				if ( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == F_IC ) {
					select = SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , -1 );
				}
				else {
					select = SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() );
				}
			}
			else {
				select = SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , -1 );
			}
			//--------------------------------------------------------
			// 2007.04.27
			//--------------------------------------------------------
			sch4_t3_logdata[i] = ( sch4_t3_logdata[i] % 1000 ) + ( select * 1000 ); // 2007.04.27
//printf( "=[%d][PLACE]===> BM%d = %d\n" , x , i + 1 , select );
			//--------------------------------------------------------
			if ( select < 10 ) {
				//==================================================================================
				res = SCHEDULER_CONTROL_BM_PUMPING_FM_PART( side , TRUE , i + BM1 , logoffset + ( select * 1000 ) );
				//==================================================================================
			}
		}
	}

	//==================================================================================
	return res;
}

BOOL SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_RETURN_FOR_4( int maxbm , int *side , int *FM_Buffer , int *slot1 , int *slot2 ) { // 2007.05.10
	int s , p , i , k;
	for ( k = BM1 ; k < ( BM1 + maxbm ) ; k++ ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) continue;
		//===============================================================================
		for ( i = _SCH_PRM_GET_MODULE_SIZE( k ) ; i >= 1 ; i-- ) {
			s = _SCH_MODULE_Get_BM_SIDE( k , i );
			p = _SCH_MODULE_Get_BM_POINTER( k , i );
			if ( _SCH_MODULE_Get_BM_WAFER( k , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( k , i ) == BUFFER_OUTWAIT_STATUS ) {
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) == PATHDO_WAFER_RETURN ) {
						if ( *side == -1 ) {
							*side = s;
							*FM_Buffer = k;
							*slot1 = 0;
							*slot2 = i;
						}
						else if ( *side == s ) {
							*FM_Buffer = k;
							*slot1 = 0;
							*slot2 = i;
						}
						return TRUE;
					}
				}
			}
		}
		//===============================================================================
	}
	return FALSE;
}


int SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( int side , int chkbmc , int *FM_Buffer , int *slot1 , int *slot2 ) {
	int k , m , s , s2;
	m = 0;
	*FM_Buffer = 0;
	for ( k = BM1 ; k < ( BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ) ; k++ ) { // 2007.04.04
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) { // 2007.04.02
			if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( side , k , &s , &s2 ) ) {
				if ( chkbmc == k ) {
					*FM_Buffer = k;
					*slot1 = 0;
					*slot2 = s;
					return 100;
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( k , BUFFER_INWAIT_STATUS ) > 0 ) {
						if ( *FM_Buffer == 0 ) {
							m = 1;
							*FM_Buffer = k;
							*slot1 = 0;
							*slot2 = s;
						}
						else {
							if ( m == 0 ) {
								m = 1;
								*FM_Buffer = k;
								*slot1 = 0;
								*slot2 = s;
							}
						}
					}
					else {
						if ( *FM_Buffer == 0 ) {
							m = 0;
							*FM_Buffer = k;
							*slot1 = 0;
							*slot2 = s;
						}
					}
				}
			}
			else { // 2007.04.05
				if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , k , &s , &s2 ) ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( k , BUFFER_INWAIT_STATUS ) > 0 ) {
						if ( m == 0 ) {
							m = 1;
							*FM_Buffer = 0;
							*slot1 = 0;
							*slot2 = 0;
						}
					}
				}
			}
		}
	}
	return 0;
}
//===============================================================================================================

extern int	sch4_BM_PROCESS_MONITOR[ MAX_CHAMBER ];
extern int	sch4_BM_PROCESS_CHECKING[ MAX_CHAMBER ];
extern int	sch4_BM_PROCESS_MODE[ MAX_CHAMBER ];


int SCHEDULER_CONTROL_Chk_PICK_BM_FMSIDE_POSSIBLE_3MODE_FOR_4( int side ) { // 2016.06.07
	int k;

//_SCH_LOG_LOT_PRINTF( side , "FM Scheduling CHECK PRE [%d,%d,%d][%d,%d,%d][%d,%d,%d][%d,%d,%d]%c\n" ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 0 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 0 ] , sch4_BM_PROCESS_MODE[ BM1 + 0 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 1 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 1 ] , sch4_BM_PROCESS_MODE[ BM1 + 1 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 2 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 2 ] , sch4_BM_PROCESS_MODE[ BM1 + 2 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 3 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 3 ] , sch4_BM_PROCESS_MODE[ BM1 + 3 ] ,
//					9 );

	//============================================================================================================
//	for ( k = BM1 ; k < BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; k++ ) {
//		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {

//_SCH_LOG_LOT_PRINTF( side , "FM Scheduling CHECK ER1 [%d,%d,%d][%d,%d,%d][%d,%d,%d][%d,%d,%d][%d]%c\n" ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 0 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 0 ] , sch4_BM_PROCESS_MODE[ BM1 + 0 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 1 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 1 ] , sch4_BM_PROCESS_MODE[ BM1 + 1 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 2 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 2 ] , sch4_BM_PROCESS_MODE[ BM1 + 2 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 3 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 3 ] , sch4_BM_PROCESS_MODE[ BM1 + 3 ] ,
//					k ,
//					9 );
//
//			return ( k * 10 ) + 1;
//		}
//	}
	for ( k = BM1 ; k < BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; k++ ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) {

//_SCH_LOG_LOT_PRINTF( side , "FM Scheduling CHECK ER2 [%d,%d,%d][%d,%d,%d][%d,%d,%d][%d,%d,%d][%d]%c\n" ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 0 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 0 ] , sch4_BM_PROCESS_MODE[ BM1 + 0 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 1 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 1 ] , sch4_BM_PROCESS_MODE[ BM1 + 1 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 2 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 2 ] , sch4_BM_PROCESS_MODE[ BM1 + 2 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 3 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 3 ] , sch4_BM_PROCESS_MODE[ BM1 + 3 ] ,
//					k ,
//					9 );

				return ( k * 10 ) + 2;
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k , TRUE ) ) {

//_SCH_LOG_LOT_PRINTF( side , "FM Scheduling CHECK ER3 [%d,%d,%d][%d,%d,%d][%d,%d,%d][%d,%d,%d][%d]%c\n" ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 0 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 0 ] , sch4_BM_PROCESS_MODE[ BM1 + 0 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 1 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 1 ] , sch4_BM_PROCESS_MODE[ BM1 + 1 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 2 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 2 ] , sch4_BM_PROCESS_MODE[ BM1 + 2 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 3 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 3 ] , sch4_BM_PROCESS_MODE[ BM1 + 3 ] ,
//					k ,
//					9 );

				return ( k * 10 ) + 3;
			}
		}
	}

//_SCH_LOG_LOT_PRINTF( side , "FM Scheduling CHECK PST [%d,%d,%d][%d,%d,%d][%d,%d,%d][%d,%d,%d]%c\n" ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 0 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 0 ] , sch4_BM_PROCESS_MODE[ BM1 + 0 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 1 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 1 ] , sch4_BM_PROCESS_MODE[ BM1 + 1 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 2 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 2 ] , sch4_BM_PROCESS_MODE[ BM1 + 2 ] ,
//					sch4_BM_PROCESS_MONITOR[ BM1 + 3 ] , sch4_BM_PROCESS_CHECKING[ BM1 + 3 ] , sch4_BM_PROCESS_MODE[ BM1 + 3 ] ,
//					9 );


	return 0;
}

int SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_FOR_4( int side , int chkbmc , int *FM_Buffer , int *slot1 , int *slot2 , BOOL *forcepick ) {
	int k , ps , ms , s , s2;
	//============================================================================================================
	*FM_Buffer = 0;
	*forcepick = FALSE;
	//============================================================================================================
	ms = 0;
	for ( k = BM1 ; k < BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; k++ ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
			ms = 1;
		}
	}
	for ( k = BM1 ; k < BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; k++ ) { // 2007.04.03
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) { // 2007.04.02
			if ( ms == 0 ) {
				if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) return 1; // 2007.04.02
			}
			else {
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k , TRUE ) ) return 2; // 2007.04.02
			}
		}
	}
	//============================================================================================================
	// 2007.05.10
	//============================================================================================================
	ps = side;
	if ( SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_RETURN_FOR_4( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , &ps , FM_Buffer , slot1 , slot2 ) ) {
		*forcepick = TRUE;
		return 11;
	}
	//============================================================================================================
	// 2007.04.26
	//============================================================================================================
	if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
		if ( Scheduling_Check_Possible_for_BM_Place( FALSE ) ) return 3;
	}
	//============================================================================================================
/*
// 2007.05.10
	if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
		if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( TRUE ) ) return 4;
	}
*/
	//============================================================================================================
	if ( _SCH_MODULE_Get_BM_FULL_MODE( BM1 ) == BUFFER_TM_STATION ) { // 2007.04.02
		return ( 5 + SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( side , chkbmc , FM_Buffer , slot1 , slot2 ) );
	}
	else {
		if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( side , BM1 , &s , &s2 ) ) {
			*FM_Buffer = BM1;
			*slot1 = s;
			ps = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( BM1 , s ) , _SCH_MODULE_Get_BM_POINTER( BM1 , s ) );
			ms = _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( BM1 , s ) , _SCH_MODULE_Get_BM_POINTER( BM1 , s ) );
		}
		else {
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2007.04.13
				if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , BM1 , &s , &s2 ) ) {
					*FM_Buffer = 0;
					*slot1 = 0;
					*slot2 = 0;
					return 6;
				}
			}
			else {
				//==============================================================================================
				// 2007.04.26
				//==============================================================================================
				if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , BM1 , &s , &s2 ) ) {
					ps = _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( BM1 , s ) , _SCH_MODULE_Get_BM_POINTER( BM1 , s ) );
					ms = _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( BM1 , s ) , _SCH_MODULE_Get_BM_POINTER( BM1 , s ) );
					for ( k = BM1 + 1 ; k < ( BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) ; k++ ) {
						if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) {
							if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , k , &s , &s2 ) ) {
								if ( ( ms + ( k - BM1 ) ) != _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( k , s ) , _SCH_MODULE_Get_BM_POINTER( k , s ) ) ) {
									break;
								}
								if ( ps != _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( k , s ) , _SCH_MODULE_Get_BM_POINTER( k , s ) ) ) {
									break;
								}
							}
							else {
								break;
							}
						}
						else {
							break;
						}
					}
					if ( k == ( BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) ) {
						*FM_Buffer = 0;
						*slot1 = 0;
						*slot2 = 0;
						return 7;
					}
				}
				//==============================================================================================
			}
			return ( 8 + SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( side , chkbmc , FM_Buffer , slot1 , slot2 ) );
		}
		//
		if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) {
			for ( k = BM1 + 1 ; k < ( BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) ; k++ ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_TM_STATION ) { // 2007.04.02
					if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , k , &s , &s2 ) ) {
						if ( ( ms + ( k - BM1 ) ) != _SCH_CLUSTER_Get_SlotOut( _SCH_MODULE_Get_BM_SIDE( k , s ) , _SCH_MODULE_Get_BM_POINTER( k , s ) ) ) {
							return ( 9 + SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( side , chkbmc , FM_Buffer , slot1 , slot2 ) );
						}
						if ( ps != _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( k , s ) , _SCH_MODULE_Get_BM_POINTER( k , s ) ) ) {
							return ( 10 + SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( side , chkbmc , FM_Buffer , slot1 , slot2 ) );
						}
					}
					else { // 2007.04.05
						return ( 11 + SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( side , chkbmc , FM_Buffer , slot1 , slot2 ) );
					}
				}
				else { // 2007.04.05
					return ( 12 + SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_ONE_FOR_4( side , chkbmc , FM_Buffer , slot1 , slot2 ) );
				}
			}
			//
			if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , k , &s , &s2 ) ) {
				//===================================================================================
				// 2007.04.26
				//===================================================================================
//				if ( ps != _SCH_CLUSTER_Get_PathOut( _SCH_MODULE_Get_BM_SIDE( k , s ) , _SCH_MODULE_Get_BM_POINTER( k , s ) ) ) {
//					*slot2 = 0;
//				}
//				else {
//					*slot2 = s * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
//				}
				//===================================================================================
				*slot2 = s * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
				//===================================================================================
			}
			else {
				*slot2 = 0;
			}
		}
		else {
			k = ( BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 );
			if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , k , &s , &s2 ) ) {
				*slot2 = s * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
			}
			else {
				*slot2 = 0;
			}
		}
	}
	return 0;
}

BOOL SCHEDULER_CONTROL_PICK_IC_RUNNING_3( int side ) { // 2007.04.05
	int FM_Buffer , k , k2 , k3;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) return TRUE;
	//
	if ( !SCHEDULER_CONTROL_Get_Free_OneBody_3_FIC_Slot( &k , &k2 ) ) return TRUE;
	//
	for ( k = BM1 ; k < BM1 + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; k++ ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k , FALSE ) ) return FALSE;
		}
	}
	//
	SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_FOR_4( side , -1 , &FM_Buffer , &k , &k2 , &k3 );
	//==================================================================================================
	// 2007.05.10
	//==================================================================================================
	if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
		if ( FM_Buffer != 0 ) {
			if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( k > 0 , k2 ) ) {
				FM_Buffer = 0;
			}
		}
	}
	//==================================================================================================
	if ( FM_Buffer != 0 ) return FALSE;
	//
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_AFTER_PICK_BM_AND_PUMP_FOR_4( int side , BOOL xmode , int lastpumpingbmc , int slot1 , int slot2 , int logoffset ) {
	int x , i , select;
	//==================================================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) {
		if ( xmode ) {
			//==================================================================================
			SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( side , TRUE , lastpumpingbmc , logoffset );
			//==================================================================================
		}
		else {
			//==================================================================================
			SCHEDULER_CONTROL_BM_PUMPING_FM_PART( side , TRUE , lastpumpingbmc , logoffset );
			//==================================================================================
		}
		return 0;
	}
	//==================================================================================
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( ( BM1 + i ) == lastpumpingbmc ) continue;
		while( TRUE ) {
			if ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) return -1;
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) <= 0 ) break;
			Sleep(1);
		}
	}
	//==================================================================================
	// 2007.04.25
	//==================================================================================
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( !SCHEDULER_CONTROL_Chk_BM_ALL_FREE_FOR_4( BM1 + i ) ) break;
	}
	if ( i == SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ) return 0;
	//==================================================================================
	// 2008.04.15
	//==================================================================================
	if ( xmode ) {
		if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( FALSE ) ) {
			for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
				if ( SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , -1 ) >= 13 ) {
					return 0;
				}
			}
		}
	}
	//==================================================================================
	for ( x = 0 ; x < 2 ; x++ ) {
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			if ( x == 1 ) {
				if ( ( i + BM1 ) != lastpumpingbmc ) continue;
			}
			else {
				if ( ( i + BM1 ) == lastpumpingbmc ) continue;
			}
			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( FALSE ) ) {
				if ( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == F_IC ) {
					select = SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , -1 );
				}
				else {
					select = SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() );
				}
			}
			else {
				select = SCHEDULER_CONTROL_Chk_NEXT_PLACE_BM_POSSIBLE_FOR_4( side , i + BM1 , -1 );
			}
			//--------------------------------------------------------
			// 2007.04.27
			//--------------------------------------------------------
			sch4_t3_logdata[i] = ( sch4_t3_logdata[i] / 1000 ) + select; // 2007.04.27
//printf( "=[%d][PICK]===> BM%d = %d\n" , x , i + 1 , select );
			//--------------------------------------------------------
			if ( select < 10 ) {
				//==================================================================================
				SCHEDULER_CONTROL_BM_PUMPING_FM_PART( side , TRUE , i + BM1 , logoffset + ( select * 1000 ) );
				//==================================================================================
			}
		}
	}

	//==================================================================================
	return 1;
}

int SCHEDULER_CONTROL_Chk_IDLE_TIME_AUTO_PUMP_VENT_FOR_4( int side ) {
	int i , j , f , dum_pt , dum_rcm;
	//==================================================================================
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) return -1;
	//==================================================================================
	//==================================================================================
	// 2007.04.25
	//==================================================================================
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) > 0 ) break;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + i ) > 0 ) break; // 2007.05.17
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) break;
		if ( !SCHEDULER_CONTROL_Chk_BM_ALL_FREE_FOR_4( BM1 + i ) ) break;
	}
	//============================================================================================
	if ( i == SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ) {
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) == BUFFER_TM_STATION ) {
				SCHEDULER_CONTROL_BM_VENTING_PART_0( side , TRUE , i + BM1 , 81 );
			}
		}
		return 31;
	}
	//==================================================================================
	f = 0;
	//==================================================================================
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) > 0 ) return -2;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + i ) > 0 ) return -2; // 2007.05.17
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) == BUFFER_TM_STATION ) { // wait pump check
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( i + BM1 , FALSE ) ) {
				if ( f == 1 ) return -3;
				f = 2;
			}
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( i + BM1 , TRUE ) ) { // wait vent check
				if ( f == 2 ) return -4;
				f = 1;
			}
		}
	}
	//==================================================================================
	if      ( f == 1 ) { // wait vent check
		for ( j = 0 ; j < 2 ; j++ ) {
			for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) > 0 ) return -11;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + i ) > 0 ) return -11; // 2007.05.17
				if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) == BUFFER_TM_STATION ) { // wait pump check
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( i + BM1 , FALSE ) ) return -12;
					if ( !SCHEDULER_CONTROL_Chk_BM_ALL_FREE_FOR_4( i + BM1 ) ) return -13;
					if ( j == 1 ) SCHEDULER_CONTROL_BM_VENTING_PART_0( side , TRUE , i + BM1 , 82 );
				}
				else {
					if ( !SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( i + BM1 , TRUE ) ) return -14; // wait vent check
				}
			}
		}
		return -10;
	}
	else if ( f == 2 ) { // wait pump check
		for ( j = 0 ; j < 2 ; j++ ) {
			for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) > 0 ) return -21;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( PM1 + i ) > 0 ) return -21; // 2007.05.17
				if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) != BUFFER_TM_STATION ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( i + BM1 , -1 ) <= 0 ) {
						//============================================================================================
						// 2007.05.09
						//============================================================================================
						if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
							if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + i ) > 0 ) return -22;
						}
						//============================================================================================
						if ( _SCH_SUB_FM_Current_No_More_Supply( side , &dum_pt , i + PM1 , &dum_rcm , -1 , -1 ) ) {
							if ( _SCH_SUB_FM_Another_No_More_Supply( side , i + PM1 , -1 , -1 ) ) {
								if ( j == 1 ) SCHEDULER_CONTROL_BM_PUMPING_FM_PART( side , TRUE , i + BM1 , 81 );
							}
							else {
								return -23;
							}
						}
						else {
							return -24;
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( i + BM1 , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( i + BM1 , BUFFER_INWAIT_STATUS ) > 0 ) {
								if ( j == 1 ) SCHEDULER_CONTROL_BM_PUMPING_FM_PART( side , TRUE , i + BM1 , 82 );
							}
							else {
								return -25;
							}
						}
						else {
							return -26;
						}
					}
				}
			}
		}
		return -20;
	}
	//==================================================================================
	return 0;
}



int SCHEDULER_CONTROL_Chk_Result_for_Place_to_BM( int side , int FM_Slot , int FM_Slot2 , int FM_Buffer ) {
	int sbres , i , k;
	int dum_s , dum_p , dum_cs , dum_bs;
	//-----------------------------------------------------------------------------------------------------------
	k = SYS_SUCCESS;
	//-----------------------------------------------------------------------------------------------------------
	if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && ( FM_Slot > 0 ) ) {
		for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
			//-----------------------------------------------------------------------------------------------------------
			if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( BM1 + i ) ) {
				sbres = SYS_RUNNING;
			}
			else {
				sbres = SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Res_for_STYLE_4( side , BM1 + i );
			}
			//-----------------------------------------------------------------------------------------------------------
			if ( sbres == SYS_RUNNING ) {
				if ( i == ( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) ) {
					if ( FM_Slot2 <= 0 ) {
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , FALSE ) ) {
							continue;
						}
					}
				}
				else {
					if ( i != 0 ) {
						if ( !_SCH_FMARMMULTI_DATA_Exist( i - 1 , &dum_s , &dum_p , &dum_cs , &dum_bs ) ) { // 2007.07.11
							if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( BM1 + i , FALSE ) ) {
								continue;
							}
						}
					}
				}
			}
			//-----------------------------------------------------------------------------------------------------------
			if ( ( sbres == SYS_ABORTED ) || ( sbres == SYS_ERROR ) ) {
				k = SYS_ABORTED;
				break;
			}
			else if ( sbres != SYS_SUCCESS ) {
				k = SYS_RUNNING;
			}
			//-----------------------------------------------------------------------------------------------------------
		}
	}
	else {
		if ( SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( FM_Buffer ) ) {
			sbres = SYS_RUNNING;
		}
		else {
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) {
				sbres = SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Res_for_STYLE_4( side , FM_Buffer );
			}
			else {
				sbres = _SCH_EQ_PROCESS_FUNCTION_STATUS( side , FM_Buffer , FALSE );
			}
		}
		if ( ( sbres == SYS_ABORTED ) || ( sbres == SYS_ERROR ) ) {
			k = SYS_ABORTED;
		}
		else if ( sbres != SYS_SUCCESS ) {
			k = SYS_RUNNING;
		}
	}
	//-----------------------------------------------------------------------------------------------------------
	return k;
	//-----------------------------------------------------------------------------------------------------------
}


BOOL SCHEDULING_OneBody3_Return_Check_for_STYLE_4( int *FM_Slot , int *FM_Pointer , int *FM_Slot2 ) { // 2007.05.11
	int i , side , FM_Buffer , pointer , slot , wafer , rs;
	//
	for ( i = 0 ; i < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ; i++ ) {
		//======================================================================================================
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( BM1 + i ) > 0 ) return TRUE;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i + BM1 ) == BUFFER_TM_STATION ) return TRUE;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( i + BM1 , TRUE ) ) return TRUE;
		//======================================================================================================
	}
	//==============
	i = 0;
	while( TRUE ) {
		side = -1;
		if ( !SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_RETURN_FOR_4( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , &side , &FM_Buffer , &wafer , &slot ) ) return TRUE;
		//
		if ( i == 0 ) {
			i = 1;
			*FM_Slot2 = 0;
			//===========================================================================
			// 2007.09.03
			//===========================================================================
			if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
				if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
					if ( FM_Has_Swap_Wafer_Style_4 ) {
						SCHEDULER_SWAPPING_3_1_REMAP_FM_ONEBODY3( &rs , FM_Pointer , FM_Slot ); // 2007.09.03
						FM_Has_Swap_Wafer_Style_4 = FALSE;
					}
				}
			}
			//===========================================================================
		}
		else {
			//===========================================
			// pick Arm B from FM_Buffer Slot
			//===========================================
			if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) return FALSE; // 2008.01.15
			//===================================================================================================
			pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , slot );
			wafer = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , slot );
			//===================================================================================================
			//-----------------------------------------------
			FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
			//-----------------------------------------------
			switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 220 , 0 , 0 , 0 , 0 , 0 , FM_Buffer , wafer , slot , side , pointer , FALSE ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//===================================================================================================
			// 2007.09.03
			//=========================================================================
			FM_Has_Swap_Wafer_Style_4 = FALSE;
			//=========================================================================
			_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 230 , 0 , 0 , 0 , 0 , 0 , FM_Buffer , wafer , slot , side , pointer , -1 , -1 );
/*
// 2007.11.26
			SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( TB_STATION , pointer , -1 );
			SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( TB_STATION , side , wafer );
			SCHEDULER_CONTROL_Set_BMx_WAFER_STATUS( FM_Buffer , slot , 0 , BUFFER_READY_STATUS );
*/
			//===================================================================================================
			if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
			//===================================================================================================
			_SCH_CLUSTER_Check_and_Make_Return_Wafer( side , pointer , 0 );
			//===================================================================================================
		}
		//===========================================
		// FM Arm B to CM
		//===========================================
		if ( Scheduler_Place_to_CM_with_FM( 2 , TRUE ) == -1 ) return FALSE;
		//===========================================
		//===================================================================================================
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//===================================================================================================
	}
	//==============
	return TRUE;
	//==============
}


BOOL SCHEDULING_Normal_Return_Check_for_STYLE_4() { // 2007.05.15
	int arm , side , pointer , slot , wafer , FM_Buffer;
	//
	//===========================================
	if ( Scheduler_Place_to_CM_with_FM( 0 , TRUE ) == -1 ) return FALSE;
	//===========================================
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
		arm = TA_STATION;
	}
	else {
		arm = TB_STATION;
	}
	//===========================================
	while( TRUE ) {
		side = -1;
		if ( !SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_RETURN_FOR_4( _SCH_PRM_GET_DFIM_MAX_PM_COUNT() , &side , &FM_Buffer , &wafer , &slot ) ) return TRUE;
		//
		//===========================================
		// pick Arm ? from FM_Buffer Slot
		//===========================================
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) return FALSE; // 2008.01.15
		//===================================================================================================
		pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , slot );
		wafer = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , slot );
		//===================================================================================================
		//-----------------------------------------------
		FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
		//-----------------------------------------------
		switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 310 , FM_Buffer , ( arm == TA_STATION ) ? wafer : 0 , slot , side , pointer , FM_Buffer , ( arm == TB_STATION ) ? wafer : 0 , slot , side , pointer , FALSE ) ) {
		case -1 :
			return FALSE;
			break;
		}
		//===================================================================================================
		// 2007.09.03
		//=========================================================================
		FM_Has_Swap_Wafer_Style_4 = FALSE;
		//=========================================================================
		_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 320 , FM_Buffer , ( arm == TA_STATION ) ? wafer : 0 , slot , side , pointer , FM_Buffer , ( arm == TB_STATION ) ? wafer : 0 , slot , side , pointer , -1 , -1 );
/*
// 2007.11.26
		SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( arm , pointer , -1 );
		SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( arm , side , wafer );
		SCHEDULER_CONTROL_Set_BMx_WAFER_STATUS( FM_Buffer , slot , 0 , BUFFER_READY_STATUS );
*/
		//===================================================================================================
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//===================================================================================================
		_SCH_CLUSTER_Check_and_Make_Return_Wafer( side , pointer , 0 );
		//===================================================================================================
		//===========================================
		// FM Arm ? to CM
		//===========================================
		if ( Scheduler_Place_to_CM_with_FM( arm + 1 , TRUE ) == -1 ) return FALSE;
		//===========================================
		//===================================================================================================
		if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
		//===================================================================================================
	}
	//==============
	return TRUE;
	//==============
}






































































//=========================================================================================================
//=========================================================================================================
int Scheduling_Other_Side_Pick_From_FM_for_4( BOOL currck , int curside , BOOL chkmode , int *retcode , BOOL seqmode , BOOL extcheck ) { // 2006.10.10
	int side , pickarm , chckarm , Result;
	int FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , FM_Side , FM_Side2;
	int FM_Slot0 , FM_Pointer0;
	int cm , pt;
	int FM_Slot_Real , k;
	//
//	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) return 0; // 2007.01.16
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		*retcode = 0;
		return 0;
	}
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
		*retcode = 1;
		return 0;
	}
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
		*retcode = 2;
		return 0; // 2006.10.14
	}
	//
//	if ( _SCH_SUB_Remain_for_FM( curside ) )  return SYS_SUCCESS;
	//
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_DONE_AL ) {
			if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_DONE_AL_GOBM ) {
				if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_NEED_AL ) { // 2006.10.14
					*retcode = 3;
					return 0;
				}
			}
		}
		if ( !currck ) { // 2007.01.08
			if ( curside != -1 ) { // 2006.10.18
				if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != curside ) {
					*retcode = 4;
					return 0;
				}
			}
		}
		chckarm = TA_STATION;
		pickarm = TB_STATION;
	}
	else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_DONE_AL ) {
			if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_DONE_AL_GOBM ) {
				if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_NEED_AL ) { // 2006.10.14
					*retcode = 5;
					return 0;
				}
			}
		}
		if ( !currck ) { // 2007.01.08
			if ( curside != -1 ) { // 2006.10.18
				if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) != curside ) {
					*retcode = 6;
					return 0;
				}
			}
		}
		chckarm = TB_STATION;
		pickarm = TA_STATION;
	}
	else {
		*retcode = 7;
		return 0;
	}
	//
	for ( side = 0 ; side < MAX_CASSETTE_SIDE ; side++ ) {
//		if ( side != curside ) { // 2007.01.08
		if ( ( !currck && ( side != curside ) ) || ( currck && ( side == curside ) ) ) { // 2007.01.08
			//==========================================================================================
			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( side , 3 ); // 2007.09.06
			//==========================================================================================
			//===========================================
			if ( !_SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( side , &cm , &pt , -1 ) ) continue;
			//===========================================
			cm = 0;
			if ( SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , side , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , ( pickarm == TA_STATION ) ? 2 : 3 , TRUE , _SCH_MODULE_Get_FM_SIDE( chckarm ) , _SCH_MODULE_Get_FM_POINTER( chckarm ) ) <= 0 ) {
				if ( SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , side , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , ( pickarm == TA_STATION ) ? 2 : 3 , FALSE , _SCH_MODULE_Get_FM_SIDE( chckarm ) , _SCH_MODULE_Get_FM_POINTER( chckarm ) ) <= 0 ) {
					continue;
				}
				else {
					cm = 1;
				}
			}
			//
			if ( FM_Slot > 0 ) {
				FM_Slot0 = FM_Slot;
				FM_Pointer0 = FM_Pointer;
			}
			else if ( FM_Slot2 > 0 ) {
				FM_Slot0 = FM_Slot2;
				FM_Pointer0 = FM_Pointer2;
			}
			//===========================================
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) {
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2007.01.17
					switch ( Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody( _SCH_MODULE_Get_FM_SIDE( chckarm ) , _SCH_MODULE_Get_FM_POINTER( chckarm ) , side , FM_Pointer0 ) ) {
					case 0 :	// 0 : 갈데없음
					case 1 :	// 1 : OneBody의 같은 BM에만 갈수있음
						continue;
						break;
					case 2 :	// 2 : OneBody의 다른 BM에만 갈수있음
						if ( cm ) continue;
						break;
					case 3 :	// 3 : 다른 Body에만 갈수있음
						break;
					case 4 :	// 4 : 기타
						if ( cm ) continue;
						break;
					}
					//===========================================
					if ( SCHEDULER_CONTROL_Chk_FM_OUT_HAS_FirstRunPre_FOR_4( side , FM_Pointer0 ) ) { 
						continue;
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( _SCH_MODULE_Get_FM_SIDE( chckarm ) , _SCH_MODULE_Get_FM_POINTER( chckarm ) , side , FM_Pointer0 ) ) {
							// 위의 "3 : 다른 Body에만 갈수있음" 있다 해도 여기서 다시 Conflict Check 하여 "3 : 다른 Body에만 갈수있음" 의 의미가 없어짐
							continue;
						}
					}
					//===========================================
				}
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) {
					if ( SCHEDULING_Possible_BM_for_STYLE2_4( side , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 ) <= 0 ) continue;
				}
				else if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 3 ) {
					if ( SCHEDULING_Possible_BM_for_STYLE2_4( side , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 ) <= 0 ) {
						if ( SCHEDULING_Possible_BM_for_STYLE3_4( side , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 ) <= 0 ) continue;
					}
				}
				//===========================================
				if ( !SCHEDULING_Possible_Process_for_STYLE_4( side , FM_Pointer0 , -1 ) ) continue;
				//===========================================
				if ( !SCHEDULING_Possible_TargetSpace_for_STYLE_4( side , FM_Pointer0 , pickarm , FALSE ) ) continue; // 2006.11.15
				//===========================================
				if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( side , FM_Pointer0 ) ) continue;
				//===========================================
				if ( pickarm == TA_STATION ) { // 2006.11.15
					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( cm ) == 1 ) continue; // 2006.11.15
				} // 2006.11.15
				else if ( pickarm == TB_STATION ) { // 2006.11.15
					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( cm ) == 2 ) continue;
				}
			}
			//===========================================================================================
			// 2007.04.04
			//===========================================================================================
			else {
				//===========================================================================================
				// 2007.11.28
				//===========================================================================================
				if ( extcheck ) { // 2007.11.28
					if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
//						if ( pickarm != TA_STATION ) { // 2008.02.11
							if ( SCHEDULER_ALIGN_BUFFER_PLACE_SLOT_CHECK_WITH_ARM_B( side , FM_Pointer0 ) == 0 ) {
								continue;
							}
//						}
					}
				}
				//===========================================================================================
				if ( pickarm == TA_STATION ) {
					Result = Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody3( side , FM_Pointer0 , _SCH_MODULE_Get_FM_SIDE( chckarm ) , _SCH_MODULE_Get_FM_WAFER( chckarm ) , _SCH_MODULE_Get_FM_POINTER( chckarm ) , seqmode ? 1 : ( extcheck ? 4 : 3 ) );
					switch ( Result ) {
					case 0 :	// AB All
						if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
							FM_Slot = FM_Slot2;
							FM_Pointer = FM_Pointer2;
						}
						FM_Slot2 = 0;
						FM_Pointer2 = 0;
						break;
					case 1 :	// 1 : A만 됨
						//
						if ( extcheck ) continue; // 2008.02.11
						//
						if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
							FM_Slot = FM_Slot2;
							FM_Pointer = FM_Pointer2;
						}
						FM_Slot2 = 0;
						FM_Pointer2 = 0;
						break;
					case 2 :	// 2 : B만 됨
					case -1 :	// -1: xx
						continue;
						break;
					}
				}
				else {
					Result = Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody3( _SCH_MODULE_Get_FM_SIDE( chckarm ) , _SCH_MODULE_Get_FM_POINTER( chckarm ) , side , FM_Slot0 , FM_Pointer0 , seqmode ? 2 : 3 );
					switch ( Result ) {
					case 0 :	// AB All
					case 2 :	// 2 : B만 됨
						if ( FM_Slot > 0 ) {
							FM_Slot2 = FM_Slot;
							FM_Pointer2 = FM_Pointer;
						}
						FM_Slot = 0;
						FM_Pointer = 0;
						break;
					case 1 :	// 1 : A만 됨
					case -1 :	// -1: xx
						continue;
						break;
					}
				}
				//
			}
			//===========================================================================================
			break;
			//===========================================
		}
	}
	//=======================================================
	if ( side == MAX_CASSETTE_SIDE ) {
		*retcode = 8;
		return 0;
	}
	//================================================================================================
	if ( !_SCH_SYSTEM_USING_RUNNING( side ) ) { // 2010.08.23
		*retcode = 9;
		return 0;
	}
	//================================================================================================
	//=======================================================
	*retcode = 0;
	//=======================================================
	if ( chkmode ) return 1; // 2007.01.22
	//================================================================================================
//	if ( !_SCH_SYSTEM_USING_RUNNING( side ) ) return 1; // 2010.06.09 // 2010.08.23
	//================================================================================================
	if ( _SCH_SUB_Get_Last_Status( side ) == 0 ) {
		if ( _SCH_SUB_Chk_Last_Out_Status( side , FM_Pointer0 ) ) {
			_SCH_MODULE_Set_BM_MidCount( side + 1 );
			_SCH_SUB_Set_Last_Status( side , 1 );
		}
	}
	_SCH_MODULE_Inc_FM_DoPointer( side );
	//
	_SCH_SUB_Remain_for_FM( side );
	//================================================================================================
//	_SCH_CASSETTE_Reset_Side_Monitor( side , 0 ); // 2007.11.27
	_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1 );
	//----------------------------------------------------------------------------
//	_SCH_SYSTEM_USING2_SET( side , 11 ); // 2008.01.14
	//-----------------------------------------------
	_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
	//----------------------------------------------
	_SCH_MODULE_Inc_FM_OutCount( side );
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	// Code for Pick from CM
	// FM_Slot
	//----------------------------------------------------------------------
	_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( side , FM_Pointer0 , 0 , 0 , 0 ); // 2006.11.27
	//----------------------------------------------------------------------
	// 2007.01.08
	//----------------------------------------------------------------------
	if ( FM_Slot > 0 ) {
		//===============================================================================================================
		_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , FM_Slot , &FM_Slot_Real , &k );
		//======================================================================================================
		_SCH_FMARMMULTI_DATA_SET_FROM_CM( side , 0 , FM_Slot_Real , k ); // 2007.08.01
		//===============================================================================================================
		_SCH_FMARMMULTI_PICK_FROM_CM_POST( 0 , 0 ); // 2007.07.11
		//===============================================================================================================
	}
	else {
		FM_Slot_Real = 0;
	}
	//----------------------------------------------------------------------
	//-----------------------------------------------
	FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
	//-----------------------------------------------
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 330 , -1 , FM_Slot , FM_Slot_Real , side , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , side , FM_Pointer2 , -1 ) ) { // 2007.03.21
	case -1 :
		return 2;
		break;
	}
	//----------------------------------------------------------------------
	SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
	//----------------------------------------------------------------------
	//===================================================================================================
	// 2007.09.03
	//=========================================================================
	FM_Has_Swap_Wafer_Style_4 = FALSE;
	//=========================================================================
	_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 340 , -1 , FM_Slot , FM_Slot_Real , side , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , side , FM_Pointer2 , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL );
/*
// 2007.11.26
	SCHEDULER_CONTROL_Set_FMx_POINTER_MODE( pickarm , FM_Pointer0 , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL );
	SCHEDULER_CONTROL_Set_FMx_SIDE_WAFER( pickarm , side , FM_Slot0 );
*/
	//----------------------------------------------------------------------
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return 2;
	//----------------------------------------------------------------------
	//===============================================================================================================
	// 2006.11.10
	//===============================================================================================================
//	if ( _SCH_MODULE_Get_FM_MODE( ckarm ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( ckarm , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
	//===============================================================================================================
	return 1;
}





//===============================================================================================================
//===============================================================================================================
//===============================================================================================================
int Scheduling_FEM_Pick_FAL_First( int side , int pickarmmode ) { // 2006.11.28
	int pickarm , carm , ALsts;
	if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.11.17
		if ( _SCH_MODULE_Get_MFAL_SIDE() == side ) {
			ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( side , FALSE );
			if ( ALsts == SYS_ABORTED ) {
				return -1;
			}
			if ( ALsts == SYS_SUCCESS ) {
				if ( pickarmmode == 0 ) {
					switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) ) { // 2007.03.15
					case 0 :
						if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TA_STATION , TRUE ) ) {
							return 1;
						}
						else if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TB_STATION , TRUE ) ) {
							return 2;
						}
						break;
					case 1 :
						if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TA_STATION , TRUE ) ) {
							return 1;
						}
						break;
					case 2 :
						if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TB_STATION , TRUE ) ) {
							return 2;
						}
						break;
					}
				}
				else {
					if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 0 ) { // 2007.03.15
						if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != pickarmmode ) { // 2007.03.15
							return 0;
						}
					}
					if ( pickarmmode == 1 ) {
						pickarm = TA_STATION;
						carm = TB_STATION;
					}
					else {
						pickarm = TB_STATION;
						carm = TA_STATION;
					}
					if ( ( _SCH_MODULE_Get_FM_MODE(carm) != FMWFR_PICK_CM_DONE_AL ) && ( _SCH_MODULE_Get_FM_MODE(carm) != FMWFR_PICK_CM_DONE_AL_GOBM ) ) { // 2006.11.17
						if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , pickarm , TRUE ) ) {
							return 1;
						}
					}
					else { // 2006.11.18
						if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , pickarm , TRUE ) ) {
							if ( SCHEDULING_CHECK_FM_Cross_Place( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , _SCH_MODULE_Get_FM_SIDE( carm ) , _SCH_MODULE_Get_FM_POINTER( carm ) ) ) { // 2006.11.18
								return 1;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}



BOOL SCHEDULER_CONTROL_MOVE_RECV_CHECK( int CHECKING_SIDE ) { // 2010.04.16
	int bmc , bmcside , k , i;
	int slot1 , slot2 , supid;
	//==================================================================================================================================================================
	if ( !_SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) return TRUE;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 1 ) return TRUE;
	//==================================================================================================================================================================
	if ( !_SCH_SUB_FM_Another_No_More_Supply( -1 , -1 , -1 , -1 ) ) return TRUE;
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return TRUE;
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) return TRUE;
	if ( _SCH_MODULE_Chk_MFIC_HAS_COUNT() > 0 ) return TRUE;
	//==================================================================================================================================================================
	bmc     = 0;
	bmcside = 0;
	supid   = 0;
	slot1 = 0;
	slot2 = 0;
	//
	for ( k = BM1 ; k < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; k++ ) {
		if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) <= MUF_00_NOTUSE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) >= MUF_90_USE_to_XDEC_FROM ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) continue;
		//
		//
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( k ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( k , i ) <= 0 ) continue;
			if ( _SCH_MODULE_Get_BM_STATUS( k , i ) != BUFFER_OUTWAIT_STATUS ) continue;
			if ( _SCH_MODULE_Get_BM_POINTER( k , i ) >= 100 ) continue;
			//
			if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) return TRUE;
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k , TRUE ) ) return TRUE;
			//
			if      ( bmc == 0 ) {
				bmc     = k;
				bmcside = _SCH_MODULE_Get_BM_SIDE( k , i );
				supid   = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( k , i ) , _SCH_MODULE_Get_BM_POINTER( k , i ) );
				slot1 = i;
				slot2 = 0;
			}
			else if ( bmc == k ) {
				if ( supid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( k , i ) , _SCH_MODULE_Get_BM_POINTER( k , i ) ) ) {
					//
					bmcside = _SCH_MODULE_Get_BM_SIDE( k , i );
					supid   = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( k , i ) , _SCH_MODULE_Get_BM_POINTER( k , i ) );
					slot2 = slot1;
					slot1 = i;
					//
				}
				else {
					if      ( slot1 == 0 ) slot1 = i;
					else if ( slot2 == 0 ) slot2 = i;
				}
			}
			else if ( bmc != k ) {
				if ( supid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( k , i ) , _SCH_MODULE_Get_BM_POINTER( k , i ) ) ) {
					//
					bmc     = k;
					bmcside = _SCH_MODULE_Get_BM_SIDE( k , i );
					supid   = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( k , i ) , _SCH_MODULE_Get_BM_POINTER( k , i ) );
					slot1 = i;
					slot2 = 0;
					//
				}
			}
		}
	}
	//==================================================================================================================================================================
	if ( bmc == 0 ) return TRUE;
	if ( bmcside != CHECKING_SIDE ) return TRUE;
	//==================================================================================================================================================================
	if ( FM_Last_Move_Recv_BM_4 == bmc ) return TRUE;
	//
	FM_Last_Move_Recv_BM_4 = bmc;
	//
	_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Start %s(%d,%d)%cWHFMMOVESTART BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( bmc ) , slot1 , slot2 , 9 , bmc - BM1 + 1 , slot1 , slot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );
	if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 0 , bmc , slot1 , slot2 ) == SYS_ABORTED ) {
		return FALSE;
	}
	_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Success %s(%d,%d)%cWHFMMOVESUCCESS BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( bmc ) , slot1 , slot2 , 9 , bmc - BM1 + 1 , slot1 , slot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );
	return TRUE;
}







int USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_4( int CHECKING_SIDE , int mode ) {
	int FM_CM , k;

	if ( mode == 0 ) {
		//-----------------------------------------------
		FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
		//-----------------------------------------------
		SCHEDULER_Set_FM_WILL_GO_BM( CHECKING_SIDE , 0 );
		//-----------------------------------------------
		return -1;
	}
	else {
		//----------------------------------------------------------------------
		FM_Pick_Running_Blocking_Style_4 = FALSE; // 2006.12.07
		FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
		//---------------------------------------------------------------------
		if ( mode == 2 ) {
			//###################################################################################
			// Cassette Skip Check (2005.07.19)
			//###################################################################################
			if ( _SCH_CASSETTE_Chk_SKIP( CHECKING_SIDE ) != 0 ) {
			}
			//###################################################################################

			if ( _SCH_MACRO_INTERRUPT_PART_CHECK( CHECKING_SIDE , FALSE , 0 ) == 99 ) {
				//===============================================================================
				for ( FM_CM = 0 , k = 0 ; k < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; k++ ) {
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + k ) > MUF_00_NOTUSE ) { // 2005.07.29
						if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( PM1 + k , 1 ) ) {
							if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + k ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Run_Flag( PM1 + k ) <= 1 ) ) { 
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , PM1 + k , -1 , 2 , 101 );
							}
							else {
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , PM1 + k , -1 , 1 , 102 );
							}
						}
						else {
							FM_CM = 1;
						}
					}
				}
				if ( FM_CM == 0 ) {
					//================================================================================================
					// 2005.07.27
					//================================================================================================
					_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 2 );
					//================================================================================================
				}
				//===============================================================================
			}
			//=====================================================================================================
			//-------------------------------------------------------------------------------------
			// 2004.09.07
			//-------------------------------------------------------------------------------------
			if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
				k = SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4( CHECKING_SIDE , TRUE );
			}
			//-------------------------------------------------------------------------------------
			//=====================================================================================================
			if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
				if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) { // 2004.09.07
//					if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN_SIDE( CHECKING_SIDE ) ) { // 2003.06.26 // 2009.08.04
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 FINISH_CHECK_2 = TRUE\n" );
						if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 2 for FEM Module%cWHFMSUCCESS 2\n" , 9 );
						}
						return SYS_SUCCESS;
//					} // 2009.08.04
				}
			}
			//=====================================================================================================
			return 0;
		}
	}
	return -1;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FM_SINGLE_BMOUT_FIRST_FOR_4() { // 2010.09.03
	int m;
	//
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return FALSE;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 1 ) return FALSE;

	for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m = m + 2 ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( m ) != BUFFER_TM_STATION ) continue;
		if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( m ) ) continue;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( m , BUFFER_OUTWAIT_STATUS ) > 0 ) continue;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( m , BUFFER_INWAIT_STATUS ) <= 0 ) {
			if ( !SCHEDULER_TM_HAS_WAFER_SUPPLY( m ) ) { // 2010.11.29
				continue;
			}
		}
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( m+1 , FALSE , -1 ) ) continue;
		if ( _SCH_MODULE_Get_BM_FULL_MODE( m+1 ) != BUFFER_FM_STATION ) continue;
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( m+1 , TRUE ) ) continue;
		//
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( m+1 , BUFFER_OUTWAIT_STATUS ) <= 0 ) continue;
		if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( m+1 , BUFFER_INWAIT_STATUS ) <= 0 ) {
			if ( !SCHEDULER_TM_HAS_WAFER_SUPPLY( m + 1 ) ) { // 2010.11.29
				continue;
			}
		}
		//
		return TRUE;
	}
	return FALSE;
	//
}

BOOL SCHEDULER_CONTROL_Chk_BM_FM_SINGLE_FORCE_MOVE_FOR_4( int orgbm , int slot ) { // 2010.09.03
	int BMC , arm;
	int os , op , i;
	int side , pt , w , cm;

	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return TRUE;
	if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 1 ) return TRUE;
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) return TRUE;
	//
	os = _SCH_MODULE_Get_BM_SIDE( orgbm , slot );
	op = _SCH_MODULE_Get_BM_POINTER( orgbm , slot );
	//
	if ( ( ( orgbm - BM1 ) % 2 ) == 0 ) {
		BMC = orgbm + 1;
	}
	else {
		BMC = orgbm - 1;
	}
	//
	if ( _SCH_MODULE_Get_BM_WAFER( BMC , slot ) > 0 ) return TRUE;
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		for ( side = 0 ; side < MAX_CASSETTE_SIDE ; side++ ) {
			//==========================================================================================
			if ( i == 0 ) {
				if ( side != os ) continue;
			}
			else {
				if ( side == os ) continue;
			}
			//==========================================================================================
			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( side , 3 );
			//==========================================================================================
			if ( !_SCH_SYSTEM_USING_RUNNING( side ) ) continue;
			//==========================================================================================
			if ( !_SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( side , &cm , &pt , -1 ) ) continue;
			//===========================================================================================
			if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( side , pt , os , op ) ) continue;
			//===========================================================================================
			break;
			//===========================================================================================
		}
		if ( side != MAX_CASSETTE_SIDE ) break;
	}
	//================================================================================================
	if ( side == MAX_CASSETTE_SIDE ) return TRUE;
	//================================================================================================
	if ( !_SCH_SYSTEM_USING_RUNNING( side ) ) return TRUE;
	//================================================================================================
	_SCH_MODULE_Set_FM_DoPointer( side , pt );
	//
	_SCH_SUB_Remain_for_FM( side );
	//================================================================================================
	_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( side , 1 );
	//================================================================================================
	_SCH_MODULE_Inc_FM_OutCount( side );
	//================================================================================================
	// Code for Pick from CM
	// FM_Slot
	//================================================================================================
	_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( side , pt , 0 , 0 , 0 );
	//----------------------------------------------------------------------
	if      ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) arm = TA_STATION;
	else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) arm = TB_STATION;
	//----------------------------------------------------------------------
	w = _SCH_CLUSTER_Get_SlotIn( side , pt );
	//----------------------------------------------------------------------
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 350 , -1 , ( arm == TA_STATION ) ? w : 0 , ( arm == TA_STATION ) ? w : 0 , side , pt , -1 , ( arm == TB_STATION ) ? w : 0 , ( arm == TB_STATION ) ? w : 0 , side , pt , -1 ) ) {
	case -1 :
		return FALSE;
		break;
	}
	//===============================================================================================================
	if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , side , side , AL , ( arm == TA_STATION ) ? w : 0 , ( arm == TB_STATION ) ? w : 0 , cm , BMC , TRUE , ( arm == TA_STATION ) ? w : 0 , ( arm == TB_STATION ) ? w : 0 , side , side , pt , pt ) == SYS_ABORTED ) { // 2006.02.14
		return FALSE;
	}
	//===============================================================================================================
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
	//===============================================================================================================
	switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 220 , BMC , ( arm == TA_STATION ) ? w : 0 , slot , side , pt , BMC , ( arm == TB_STATION ) ? w : 0 , slot , side , pt , -1 ) ) {
	case -1 :
		return FALSE;
		break;
	}
	//===============================================================================================================
	if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( side ) ) || ( _SCH_SYSTEM_USING_GET( side ) <= 0 ) ) return FALSE;
	//===============================================================================================================
	_SCH_MODULE_Set_BM_SIDE_POINTER( BMC , slot , side , pt );
	_SCH_MODULE_Set_BM_WAFER_STATUS( BMC , slot , w , BUFFER_INWAIT_STATUS );
	//===============================================================================================================
	return TRUE;
}


int SCHEDULING_Possible_Pre_BM_4( int side , int bx ) { // 2017.04.27
	int k , bch , Res;

//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP PRE-BM RESULT 1 = [bx=%d]\n" , bx );
//----------------------------------------------------------------------

	if ( bx > 0 ) return bx;
	//
	bch = bx;
	//
	for ( k = BM1 ; k < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; k++ ) {
		//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP PRE-BM RESULT 11 = [k=%d][bch=%d]\n" , k , bch );
//----------------------------------------------------------------------
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) != BUFFER_FM_STATION ) continue;
		//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP PRE-BM RESULT 12 = [k=%d][bch=%d]\n" , k , bch );
//----------------------------------------------------------------------
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) continue;
		//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP PRE-BM RESULT 13 = [k=%d][bch=%d]\n" , k , bch );
//----------------------------------------------------------------------
		//
		Res = SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK2_FOR_4( k ); // 0:xx 1:Ix 2:xO 3:IO
		//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP PRE-BM RESULT 14 = [k=%d][bch=%d] [%d]\n" , k , bch , Res );
//----------------------------------------------------------------------
		//
		if ( 3 == Res ) { // 0:xx 1:Ix 2:xO 3:IO
			//
			if ( bch > 0 ) return bx;
			//
			bch = k;
			//
		}
		//
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( side , 0 , "FEM STEP PRE-BM RESULT 15 = [k=%d][bch=%d]\n" , k , bch );
//----------------------------------------------------------------------
		//
	}
	//
	return bch;
	//
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FEM Scheduling
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_4( int CHECKING_SIDE ) {
	int	FingerCount;
	int	SignalCount;
	int	SignalBM;
	int	FM_Slot;
	int	FM_Slot2;
	int	FM_TSlot , FM_OSlot , FM_CO1 , FM_CO2;
	int	FM_TSlot2 , FM_OSlot2;
	int	FM_Pointer;
	int	FM_Pointer2;
	int	FM_Side;
	int	FM_Side2;
	int	DM_Side;
	int k;
	int FM_Buffer , FM_Bufferx , FM_TSlotx , FM_TSlot2x , FM_TSlotxx , FM_dbSide , FM_dbSide2;
	int ALsts , ICsts , ICsts1 , ICsts2;
	int Result;
	int FM_CM;
	int dummode;
	int doubleok , doubleok4isdouble;
	int sbres;
	int sp_data , sp_old , sp_new , sp_pointer , sp_pointer2 , sp_start , sp_index , s1p , splogdata;
	BOOL FMPickForce; // 2005.09.07
	BOOL FMICPickReject; // 2005.09.07
	BOOL FMALBMPickReject; // 2007.04.27
	int rcm; // 2006.09.27
	int osp; // 2006.10.31
	int olddoubleok , mvbuffer , mvslot1 , mvslot2; // 2006.11.10
	int SignalMove; // 2006.11.10
	int clscheckskip; // 2006.11.18

	int FM_Ext_Loop , FM_Slot_Real , FM_TSlot_Real; // 2006.12.25
	int retdata; // 2007.07.09

//	int BM_AlingTag; // 2007.01.22

//	char RunRecipe[65]; // 2005.12.21 // 2007.01.04
	char RunRecipe[4096+1]; // 2005.12.21 // 2007.01.04

	int dal_s[32] = { -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 };
	int dal_p[32];
	int dal_w[32] = {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 };
	int dal_xl , dal_sldata , dal_s2;

	int garbage , dum_pt , dum_rcm;


	SignalMove = 0; // 2006.11.10
	SignalCount = 0;
	SignalBM = -1;
	//
	FingerCount = 0;
	FM_Buffer = 0;
	FM_Slot = 0;
	FM_Slot2 = 0;
	FM_Pointer = 0;
	FM_Pointer2 = 0;
	FM_TSlot = 0;
	FM_TSlot2 = 0;
	//
	//----------------------------------------------------------------------
	_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 00 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
	//----------------------------------------------------------------------
	if ( !SCHEDULER_CONTROL_MOVE_RECV_CHECK( CHECKING_SIDE ) ) { // 2010.04.16
		return 0;
	}
	//----------------------------------------------------------------------
	while ( TRUE ) {
		//=====================================================================================================
		// 2007.04.26
		//=====================================================================================================
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
			if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
				if ( SCHEDULER_ALIGN_BUFFER_CONTROL_MONITORING_ERROR( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) { // 2007.04.26
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , 9 , _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() - BM1 + 1 );
					return 0;
				}
			}
		}
		//=====================================================================================================
		// 2007.06.26
		//=====================================================================================================
		if ( MANAGER_ABORT() ) {
			_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because Abort%cWHFAIL\n" , 9 );
			return 0;
		}
		//=====================================================================================================
		//=====================================================================================================
		FM_Pick_Running_Blocking_Style_4 = FALSE; // 2006.12.07
		FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
		//=====================================================================================================
		//-------------------------------------------------------------------------------------
		FMPickForce = FALSE; // 2005.09.07
		FMICPickReject = FALSE; // 2005.09.07
		FMALBMPickReject = FALSE; // 2007.04.27
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		// PICK FROM BM & PLACE TO CM PART
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		FM_Buffer = 0;
		FingerCount = 0;
//			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.06.24 // 2007.01.06
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.06.24 // 2007.01.06
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
				FingerCount++;
			}
			else {
				if ( ( SignalCount != 0 ) && ( SignalBM == -1 ) ) { // 2004.11.10
					SCHEDULER_CONTROL_Chk_BM_ONLY_OUT_NEED( CHECKING_SIDE , &SignalBM ); // 2004.10.10
				}
				//===================================================================================================
				// 2006.11.23
				//===================================================================================================
				if ( ( SignalCount == 0 ) && ( SCHEDULER_Get_BM_WhereLastPlaced() != -1 ) && ( SignalBM == -1 ) ) {
					if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) {
							if ( SCHEDULER_CONTROL_Chk_BM_IN_YES_OUT_NEED( CHECKING_SIDE , SCHEDULER_Get_BM_WhereLastPlaced() ) ) {
								SignalBM = SCHEDULER_Get_BM_WhereLastPlaced();
							}
						}
					}
					else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
						if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) {
							if ( SCHEDULER_CONTROL_Chk_BM_IN_YES_OUT_NEED( CHECKING_SIDE , SCHEDULER_Get_BM_WhereLastPlaced() ) ) {
								SignalBM = SCHEDULER_Get_BM_WhereLastPlaced();
							}
						}
					}
				}
				//===================================================================================================
//					if ( SignalBM == -1 ) { // 2004.11.02
				if ( ( SignalCount == 0 ) || ( SignalBM == -1 ) ) { // 2004.11.02
					if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) { // 2004.11.04 // 2004.12.22
//						if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) { // 2004.11.04 // 2004.12.22
//							if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 0 ) FingerCount++; // 2005.09.06
						if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) {
							//========================================================================================================
							// 2006.11.24
							//========================================================================================================
//								FingerCount++; // 2005.09.06
							//========================================================================================================
							if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) {
								FingerCount++;
							}
							else {
								if ( SCHEDULING_Possible_TargetSpace2_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) ) {
									FingerCount++;
								}
							}
							//========================================================================================================
						}
					}
					else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) { // 2004.11.04 // 2004.12.22
//						else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) { // 2004.11.04 // 2004.12.22
//							if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 0 ) FingerCount++; // 2005.09.06
						if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) {
							//========================================================================================================
							// 2006.11.24
							//========================================================================================================
//								FingerCount++; // 2005.09.06
							//========================================================================================================
							if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) {
								FingerCount++;
							}
							else {
								if ( SCHEDULING_Possible_TargetSpace2_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) {
									FingerCount++;
								}
							}
							//========================================================================================================
						}
					}
				}
				//==================================================================================================================================
				// 2004.11.02
				//==================================================================================================================================
				if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
//						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) { // 2004.11.04
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) { // 2004.11.04
						//if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 1(?) ) FingerCount++;
						//
						if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) { // 2007.03.16
							if ( SCHEDULING_Possible_TargetSpace2_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() ) ) { // 2007.03.20
								FingerCount++;
							}
						}
					}
//						else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) { // 2004.11.04
					else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) { // 2004.11.04
						//if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 1(?) ) FingerCount++;
						if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) { // 2007.03.16
							if ( SCHEDULING_Possible_TargetSpace2_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() ) ) { // 2007.03.20
								FingerCount++;
							}
						}
					}
				}
				//==================================================================================================================================
				//==================================================================================================================================
				// 2005.09.08
				//==================================================================================================================================
				if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2005.09.06
					if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
						if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) FingerCount++;
						else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) FingerCount++;
						//===================================================================================================================
						if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.11.17
						//===================================================================================================================
							if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
								if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) == 0 ) { // 2007.03.16
									FingerCount++;
								}
								else { // 2007.03.16
									if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) { // 2007.03.16
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) { // 2007.03.16
											FingerCount++; // 2007.03.16
										} // 2007.03.16
									} // 2007.03.16
									else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) { // 2007.03.16
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) { // 2007.03.16
											FingerCount++; // 2007.03.16
										} // 2007.03.16
									} // 2007.03.16
								} // 2007.03.16
							}
						}
					}
				}
				//==================================================================================================================================
			}
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) FingerCount++;
			//============================================================================================
			// 2006.12.18
			//============================================================================================
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
					FingerCount++;
				}
			}
			//============================================================================================
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) FingerCount++;
			//============================================================================================
			// 2006.12.18
			//============================================================================================
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
					FingerCount++;
				}
			}
			//============================================================================================
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][sch4_LAST_PLACE_BM=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , SCHEDULER_Get_BM_WhereLastPlaced() );
		//----------------------------------------------------------------------
		//===================================================================================================
		// 2006.12.15
		//===================================================================================================
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
//				SignalBM = BM1; // 2007.03.30
			//=============================================================================================
			SCHEDULER_BM_PROCESS_MONITOR_ONEBODY( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() );
			//=============================================================================================
			if ( SignalCount != 0 ) {
				if ( FingerCount == 0 ) {
					Result = SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_FOR_4( CHECKING_SIDE , SignalBM , &FM_Buffer , &FM_Slot , &FM_Slot2 , &dal_xl );
					//==================================================================================================
					// 2007.05.10
					//==================================================================================================
					if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
						if ( FM_Buffer != 0 ) {
							if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( FM_Slot > 0 , FM_Slot2 ) ) {
								FM_Buffer = 0;
							}
						}
					}
					//==================================================================================================
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][sch4_LAST_PLACE_BM=%d][Result=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , SCHEDULER_Get_BM_WhereLastPlaced() , Result );
				//----------------------------------------------------------------------
//					if ( FM_Buffer == 0 ) { // 2007.04.26
				if ( ( FM_Buffer == 0 ) && ( SignalBM == -1 ) ) { // 2007.04.26
					Result = SCHEDULER_CONTROL_Chk_IDLE_TIME_AUTO_PUMP_VENT_FOR_4( CHECKING_SIDE );
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][sch4_LAST_PLACE_BM=%d][Result=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , SCHEDULER_Get_BM_WhereLastPlaced() , Result );
				//----------------------------------------------------------------------
				//=======================================================================================================
				// 2007.04.11
				//=======================================================================================================
				if ( FM_Buffer != 0 ) {
					if ( FM_Slot <= 0 ) {
						if ( !dal_xl ) {
							if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
								if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
									if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( FM_Buffer , BUFFER_INWAIT_STATUS ) <= 0 ) {
										if ( _SCH_MODULE_Get_BM_WAFER( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + FM_Buffer - BM1 ) > 0 ) {
											if ( SCHEDULER_CONTROL_CLIDX_CHECK_TO_ALBM( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) + 1 + FM_Buffer - BM1 ) ) { // 2007.04.26
												FM_Buffer = 0;
											}
										}
									}
								}
							}
						}
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01c RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][sch4_LAST_PLACE_BM=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , SCHEDULER_Get_BM_WhereLastPlaced() );
				//----------------------------------------------------------------------
				//=======================================================================================================
			}
			//=============================================================================================
		}
		else { // 2007.03.30
			//----------------------------------------------------------------------
			// 2006.01.24
			//----------------------------------------------------------------------
			//----------------------------------------------------------------------
			if ( FingerCount == 0 ) {
//				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2006.12.18
				if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2006.12.18
					if (
						( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) ||
						( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) )
						) {
						for ( k = BM1 ; k < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; k++ ) {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) {
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k , FALSE ) ) {
									//=================================================================================================================
									// 2006.10.10
									//=================================================================================================================
									if ( SCHEDULING_Possible_BM_CHeck_Only_at_PM_for_STYLE_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( k ) , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) > 0 ) {
									//=================================================================================================================
										FingerCount = 1;
										break;
									//=================================================================================================================
									}
									//=================================================================================================================
								}
							}
						}
					}
					else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						//===============================================================================================================
						retdata = 0; // 2014.01.15
						FM_dbSide2 = 1;
						for ( k = BM1 ; k < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; k++ ) {
							if ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_TM_STATION ) continue;
							if ( !_SCH_MODULE_GET_USE_ENABLE( k , FALSE , -1 ) ) continue;
							FM_dbSide = 0;
							DM_Side = SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k ); // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
							if ( _SCH_MODULE_GET_USE_ENABLE( k - BM1 + PM1 , FALSE , -1 ) ) {
								if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT_STYLE_4( k ) <= 0 ) { // 2007.08.27
									FM_dbSide2 = 1;
									break;
								}
								else {
									if ( ( DM_Side == 1 ) || ( DM_Side == 3 ) ) {
										FM_dbSide2 = 1;
										break;
									}
								}
							}
							else {
								if ( ( DM_Side == 2 ) || ( DM_Side == 3 ) ) {
									FM_dbSide2 = 1;
									break;
								}
							}
							/*
							// 2014.01.15
							if ( FM_dbSide == 0 ) {
								if ( !_SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , k - BM1 + PM1 , &dum_rcm , -1 , -1 ) ) {
									FM_dbSide2 = 0;
								}
								else if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , k - BM1 + PM1 , -1 , -1 ) ) {
									FM_dbSide2 = 0;
								}
							}
							*/
							//
							// 2014.01.15
							if ( FM_dbSide == 0 ) {
								if ( !_SCH_SUB_FM_Another_No_More_Supply( -1 , k - BM1 + PM1 , -1 , -1 ) ) {
									//
									if ( !Scheduling_Check_Single_Arm_BM_Explict_Check( k , -1 , -1 ) ) {
										//
										FM_dbSide2 = 0;
										//
									}
									else {
										retdata = k; // 2014.01.15
									}
									//
								}
							}
						}
						//
						if ( retdata <= 0 ) { // 2014.01.15
							if ( FM_dbSide2 == 0 ) FingerCount = 1;
						}
						//===============================================================================================================
					}
				}
			}
			//----------------------------------------------------------------------
			//----------------------------------------------------------------------
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 0a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
			//----------------------------------------------------------------------
			//----------------------------------------------------------------------
//			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 1 ) && ( FingerCount == 0 ) ) { // 2005.01.24
			if ( ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) && ( FingerCount == 0 ) ) { // 2005.01.24
//				if ( SignalCount == 0 ) { // 2006.11.23
				if ( ( SignalCount == 0 ) && ( SignalBM == -1 ) ) { // 2006.11.23
					//
					FM_Buffer = 0;
					FM_Slot = 0;
					FM_Slot2 = 0;
					FM_dbSide = 0;
					//
					for ( k = BM1 ; k < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; k++ ) {
//						if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) <= MUF_00_NOTUSE ) continue; // 2003.10.25
						if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) <= MUF_00_NOTUSE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , k ) >= MUF_90_USE_to_XDEC_FROM ) ) continue; // 2003.10.25
						if ( SCHEDULER_CONTROL_Chk_BM_OUT_CMFIRST_FOR_4( CHECKING_SIDE , FALSE , k , &FM_Slot ) ) {
							//===============================================================
							// 2006.10.13
							//===============================================================
							if      ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 1 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) continue;
								//
								if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2006.10.19
									if ( FM_Slot < 100 ) { // 2006.10.19
										if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , FM_Slot ) ) {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) continue;
											if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 1 ) {
												if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
													continue;
												}
											}
										}
									}
									else {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) continue;
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 1 ) {
											if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
												continue;
											}
										}
									}
								}
								//
							}
							else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 2 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) continue;
								//
								if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2006.10.19
									if ( FM_Slot < 100 ) { // 2006.10.19
										if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , FM_Slot ) ) {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) continue;
											if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 2 ) {
												if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
													continue;
												}
											}
										}
									}
									else {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) continue;
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 2 ) {
											if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
												continue;
											}
										}
									}
								}
								//
							}
							//===============================================================
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) {
								if ( SignalBM == -1 ) {
									SignalBM = k;
									FM_Buffer = SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k );
									FM_Slot2 = FM_Slot;
									FM_dbSide = SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k ); // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2006.01.14
										//-----------------------------------------------------------------------
										DM_Side = SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k ); // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
										s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( k );
										//-----------------------------------------------------------------------
										if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) {
											if ( _SCH_MODULE_Get_BM_FULL_MODE( k + s1p ) == BUFFER_FM_STATION ) {
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k + s1p , TRUE ) ) {
													SignalBM = k;
													FM_Buffer = FALSE;
													FM_Slot2 = FM_Slot;
													FM_dbSide = 4;
													break;
												}
											}
											if ( DM_Side == 3 ) {
												if ( FM_dbSide < 3 ) {
													SignalBM = k;
													FM_Buffer = FALSE;
													FM_Slot2 = FM_Slot;
													FM_dbSide = 3;
												}
											}
											else if ( DM_Side == 2 ) {
												if ( FM_dbSide < 2 ) {
													SignalBM = k;
													FM_Buffer = FALSE;
													FM_Slot2 = FM_Slot;
													FM_dbSide = 2;
												}
											}
										}
										else {
											if ( FM_Buffer ) {
												if ( _SCH_MODULE_Get_BM_FULL_MODE( k + s1p ) == BUFFER_FM_STATION ) {
													if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k + s1p , TRUE ) ) {
														SignalBM = k;
														FM_Buffer = TRUE;
														FM_Slot2 = FM_Slot;
														FM_dbSide = 4;
													}
												}
												if ( DM_Side == 3 ) {
													if ( FM_dbSide < 3 ) {
														SignalBM = k;
														FM_Buffer = TRUE;
														FM_Slot2 = FM_Slot;
														FM_dbSide = 3;
													}
												}
												else if ( DM_Side == 2 ) {
													if ( FM_dbSide < 2 ) {
														SignalBM = k;
														FM_Buffer = TRUE;
														FM_Slot2 = FM_Slot;
														FM_dbSide = 2;
													}
												}
											}
										}
									}
									else {
										if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) {
											if ( FM_Buffer ) {
												SignalBM = k;
												FM_Buffer = FALSE;
												FM_Slot2 = FM_Slot;
											}
											else {
												if ( FM_Slot >= 100 ) {
													SignalBM = k;
													FM_Buffer = FALSE;
													FM_Slot2 = FM_Slot;
												}
												else {
													if ( FM_Slot2 < 100 ) {
														if ( FM_Slot < FM_Slot2 ) {
															SignalBM = k;
															FM_Buffer = FALSE;
															FM_Slot2 = FM_Slot;
														}
													}
												}
											}
										}
										else {
											if ( FM_Buffer ) {
												if ( FM_Slot >= 100 ) {
													SignalBM = k;
													FM_Buffer = TRUE;
													FM_Slot2 = FM_Slot;
												}
												else {
													if ( FM_Slot2 < 100 ) {
														if ( FM_Slot < FM_Slot2 ) {
															SignalBM = k;
															FM_Buffer = TRUE;
															FM_Slot2 = FM_Slot;
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
					FM_Buffer = 0;
					FingerCount = 1;
					//
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
			//----------------------------------------------------------------------
			if ( FingerCount == 0 ) {
				if ( ( SignalCount != 0 ) && ( SignalBM != -1 ) ) {
					//===============================================================================================================================
//					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.11.05 // 2007.01.06
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.05 // 2007.01.06
						if ( SCHEDULER_CONTROL_Chk_BM_OUT_OTHER_FOR_4( CHECKING_SIDE , SignalBM ) ) break;
					}
					//===============================================================================================================================
					if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) || ( SignalBM == BM1 ) ) { // 2007.01.08
						k = SignalBM;
						if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( CHECKING_SIDE , k , &FM_Slot , &FM_Slot2 ) ) { // 2006.10.19
							//===============================================================
							// 2006.10.19
							//===============================================================
							if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 1 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) k = -1;
								else {
									//
									if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2006.10.19
										if ( _SCH_MODULE_Get_BM_POINTER( k , FM_Slot ) < 100 ) {
											if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( k , FM_Slot ) ) ) {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) k = -1;
												else {
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 1 ) {
														if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
															k = -1;
														}
													}
												}
											}
										}
										else {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) k = -1;
											else {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 1 ) {
													if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
														k = -1;
													}
												}
											}
										}
									}
									//
								}
							}
							else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 2 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) k = -1;
								else {
									//
									if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2006.10.19
										if ( _SCH_MODULE_Get_BM_POINTER( k , FM_Slot ) < 100 ) {
											if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( k , FM_Slot ) ) ) {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) k = -1;
												else {
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 2 ) {
														if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
															k = -1;
														}
													}
												}
											}
										}
										else {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) k = -1;
											else {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 2 ) {
													if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
														k = -1;
													}
												}
											}
										}
									}
									//
								}
							}
							//=========================================================================================================
							if ( k != -1 ) {
		//						if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) { // 2006.05.26
		//							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2006.05.26
								if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2006.05.26
									if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) { // 2006.05.26
										FM_Buffer = k;
										//============================================================================================================================
										// 2007.01.08
										//============================================================================================================================
										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.08
											if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) {
												if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( CHECKING_SIDE , FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
													FM_Slot2 = FM_TSlotx * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
												}
												else {
													if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
														FM_Slot2 = FM_TSlotx * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
													}
												}
											}
										}
										//============================================================================================================================
										else {
											if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
												if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
												}
												else {
													FM_Slot2 = FM_Slot;
													FM_Slot = 0;
													//
													if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2006.01.14
														if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2006.02.15
															if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
																if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( k ) ) == 2 ) {
																	FM_Slot2 = 0;
																	FM_Slot = 0;
																	SignalCount = 1;
																	FM_Buffer = 0;
																}
															}
														}
													}
												}
											}
											else if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
												FM_Slot2 = 0;
												//
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2006.01.14
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.02.15
														if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
															if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( SCHEDULER_CONTROL_Chk_ONEBODY_nch_Style_4( k ) ) == 2 ) {
																FM_Slot2 = 0;
																FM_Slot = 0;
																SignalCount = 1;
																FM_Buffer = 0;
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
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 03 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
				//----------------------------------------------------------------------
				if ( FM_Buffer == 0 ) {
					FM_dbSide = 0; // 2006.01.14
					for ( k = BM1 ; k < ( BM1 + _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; k++ ) {
						//===================================================================================================
						// 2006.12.15
						//===================================================================================================
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
							if ( k != BM1 ) continue;
						}
						//===================================================================================================
						if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( CHECKING_SIDE , k , &FM_TSlotx , &FM_TSlot2x ) ) {
							//===============================================================
							// 2006.10.19
							//===============================================================
							if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 1 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) continue;
								else {
									//
									if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
										if ( _SCH_MODULE_Get_BM_POINTER( k , FM_TSlotx ) < 100 ) {
											if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( k , FM_TSlotx ) ) ) {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) continue;
												else {
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 1 ) {
														if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
															continue;
														}
													}
												}
											}
										}
										else {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) continue;
											else {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 1 ) {
													if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
														continue;
													}
												}
											}
										}
									}
									//
								}
							}
							else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( k ) == 2 ) {
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) continue;
								else {
									//
									if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
										if ( _SCH_MODULE_Get_BM_POINTER( k , FM_TSlotx ) < 100 ) {
											if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , _SCH_MODULE_Get_BM_POINTER( k , FM_TSlotx ) ) ) {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) continue;
												else {
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 2 ) {
														if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
															continue;
														}
													}
												}
											}
										}
										else {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) continue;
											else {
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) == 2 ) {
													if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
														continue;
													}
												}
											}
										}
									}
									//
								}
							}
							//=========================================================================================================
	//							if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) { // 2006.05.26
//								if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAITx_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2006.05.26
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k ) == BUFFER_FM_STATION ) ) { // 2006.05.26
								if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( k ) ) { // 2006.05.26
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2006.12.18
										FM_Buffer = k;
										FM_Slot = FM_TSlotx;
										FM_Slot2 = FM_TSlot2x;
										//============================================================================================================================
										// 2007.01.08
										//============================================================================================================================
										if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) ) { // 2007.01.08
											if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( CHECKING_SIDE , FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
												FM_Slot2 = FM_TSlotx * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
											}
											else {
												if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( -1 , FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
													FM_Slot2 = FM_TSlotx * SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT();
												}
											}
										}
										//============================================================================================================================
									}
									else if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2006.01.14 // 2006.12.18
//									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2006.01.14 // 2006.12.18
										//-----------------------------------------------------------------------
										DM_Side = SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k ); // 2006.01.14 0:xx 1:Ix 2:xO 3:IO
										s1p = SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( k );
										//-----------------------------------------------------------------------
										if ( _SCH_MODULE_Get_BM_FULL_MODE( k + s1p ) == BUFFER_FM_STATION ) {
											if ( SCHEDULER_CONTROL_Chk_ONEBODY_BM_MONITOR_Wait_for_STYLE_4( k + s1p , TRUE ) ) {
												FM_Buffer = k;
												FM_Slot = FM_TSlotx;
												FM_Slot2 = FM_TSlot2x;
												if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
													}
													else {
														if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2006.02.15
															if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
																if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k + s1p ) == 2 ) {
																	FM_Slot2 = 0;
																	FM_Slot = 0;
																	FM_Buffer = 0;
																	SignalCount = 1;
																	break;
																}
															}
														}
														FM_Slot2 = FM_Slot;
														FM_Slot = 0;
													}
												}
												else if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.02.15
														if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
															if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k + s1p ) == 2 ) {
																FM_Slot2 = 0;
																FM_Slot = 0;
																FM_Buffer = 0;
																SignalCount = 1;
																break;
															}
														}
													}
													FM_Slot2 = 0;
												}
												break;
											}
										}
										if ( DM_Side == 3 ) {
											if ( FM_dbSide != 3 ) {
												FM_Buffer = k;
												FM_Slot = FM_TSlotx;
												FM_Slot2 = FM_TSlot2x;
												if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
													}
													else {
														if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2006.02.15
															if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
																if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k + s1p ) == 2 ) {
																	FM_Slot2 = 0;
																	FM_Slot = 0;
																	FM_Buffer = 0;
																	SignalCount = 1;
																	break;
																}
															}
														}
														FM_Slot2 = FM_Slot;
														FM_Slot = 0;
													}
												}
												else if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.02.15
														if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
															if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k + s1p ) == 2 ) {
																FM_Slot2 = 0;
																FM_Slot = 0;
																FM_Buffer = 0;
																SignalCount = 1;
																break;
															}
														}
													}
													FM_Slot2 = 0;
												}
												FM_dbSide = 3;
											}
										}
										else if ( DM_Side == 2 ) {
											if ( FM_dbSide < 2 ) {
												FM_Buffer = k;
												FM_Slot = FM_TSlotx;
												FM_Slot2 = FM_TSlot2x;
												if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
													}
													else {
														if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2006.02.15
															if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
																if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k + s1p ) == 2 ) {
																	FM_Slot2 = 0;
																	FM_Slot = 0;
																	FM_Buffer = 0;
																	SignalCount = 1;
																	break;
																}
															}
														}
														FM_Slot2 = FM_Slot;
														FM_Slot = 0;
													}
												}
												else if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
													if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2006.02.15
														if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) {
															if ( SCHEDULER_CONTROL_Chk_BM_IN_OUT_CHECK_FOR_4( k + s1p ) == 2 ) {
																FM_Slot2 = 0;
																FM_Slot = 0;
																FM_Buffer = 0;
																SignalCount = 1;
																break;
															}
														}
													}
													FM_Slot2 = 0;
												}
												FM_dbSide = 3;
											}
										}
									}
									else {
										FM_Buffer = k;
										FM_Slot = FM_TSlotx;
										FM_Slot2 = FM_TSlot2x;
										if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
											if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
											}
											else {
												FM_Slot2 = FM_Slot;
												FM_Slot = 0;
											}
										}
										else if ( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
											FM_Slot2 = 0;
										}
										break;
									}
								}
							}
						}
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 04 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
			//----------------------------------------------------------------------
			//=========================================================================================================================
			// 2006.11.17
			//=========================================================================================================================
			if ( FM_Buffer != 0 ) {
//				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2007.01.06
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.05 // 2007.01.06
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) {
						if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
							if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
								if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , FM_Buffer - BM1 + PM1 ) ) {
									FM_Buffer = 0;
									FMICPickReject = TRUE;
								}
								else {
									if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , FM_Buffer - BM1 + PM1 ) ) {
										FM_Buffer = 0;
										FMICPickReject = TRUE;
									}
								}
							}
						}
					}
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 04a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][Result=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , Result );
		//----------------------------------------------------------------------
		if ( FM_Buffer == 0 ) {
//				if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.08.24 // 2007.01.06
			if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) ) { // 2004.08.24 // 2007.01.06
				if ( SignalCount != 0 ) break;
			}
			//--------------------------------------------------
			FingerCount = 1; // 2004.08.13
			//--------------------------------------------------
		}
		else {
			SignalBM = FM_Buffer;
			//============================================================================================
			if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( SignalBM - BM1 + PM1 , 1 ) ) {
				if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SignalBM - BM1 + PM1 ) == MUF_04_USE_to_PREND_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , SignalBM - BM1 + PM1 ) == MUF_05_USE_to_PRENDNA_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Run_Flag( SignalBM - BM1 + PM1 ) <= MUF_00_NOTUSE ) ) {  // 2006.01.17
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SignalBM - BM1 + PM1 , -1 , 2 , 103 );
				}
				else {
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , SignalBM - BM1 + PM1 , -1 , 1 , 104 );
				}
			}
			//============================================================================================
			doubleok4isdouble = 0; // 2005.10.06
			//============================================================================================
//				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2005.10.06 // 2006.01.06
//				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.10.06 // 2006.01.06 // 2007.01.08
			if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2005.10.06 // 2006.01.06 // 2007.01.08
//					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2006.12.15 // 2007.01.06
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.05 // 2007.01.06
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						if ( !_SCH_MODULE_Need_Do_Multi_FIC() || ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() > 0 ) ) {
							if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( CHECKING_SIDE , FM_Buffer + 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
									if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer + 1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer + 1 ) == BUFFER_FM_STATION ) ) {
										if ( FM_TSlotx == FM_Slot ) {
											if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( FM_Buffer + 1 ) ) {
												doubleok4isdouble = 3;
												FM_Slot2 = FM_Slot;
											}
											else {
												FM_Slot = 0;
												FM_Slot2 = 0;
											}
										}
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_OUT_OTHER_SIDE_FOR_4( CHECKING_SIDE , FM_Buffer + 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
										if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer + 1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer + 1 ) == BUFFER_FM_STATION ) ) {
											if ( FM_TSlotx == FM_Slot ) {
												if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( FM_Buffer + 1 ) ) {
													doubleok4isdouble = 3;
													FM_Slot2 = FM_Slot;
												}
												else {
													FM_Slot = 0;
													FM_Slot2 = 0;
												}
											}
										}
									}
								}
							}
							else {
								if ( SCHEDULER_CONTROL_Chk_BM_OUT_FOR_4( CHECKING_SIDE , FM_Buffer - 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
									if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer - 1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer - 1 ) == BUFFER_FM_STATION ) ) {
										if ( FM_TSlotx == FM_Slot ) {
											if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( FM_Buffer - 1 ) ) {
												doubleok4isdouble = 3;
												FM_Slot2 = FM_Slot;
												FM_Buffer = FM_Buffer - 1;
											}
											else {
												FM_Slot = 0;
												FM_Slot2 = 0;
											}
										}
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_OUT_OTHER_SIDE_FOR_4( CHECKING_SIDE , FM_Buffer - 1 , &FM_TSlotx , &FM_TSlot2x ) ) {
										if ( ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer - 1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer - 1 ) == BUFFER_FM_STATION ) ) {
											if ( FM_TSlotx == FM_Slot ) {
												if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( FM_Buffer - 1 ) ) {
													doubleok4isdouble = 3;
													FM_Slot2 = FM_Slot;
													FM_Buffer = FM_Buffer - 1;
												}
												else {
													FM_Slot = 0;
													FM_Slot2 = 0;
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
			//============================================================================================
			if ( ( doubleok4isdouble == 3 ) && ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) { // 2005.10.06
				FM_dbSide = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot );
				FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
				FM_TSlot = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot );

				FM_dbSide2 = _SCH_MODULE_Get_BM_SIDE( FM_Buffer + 1 , FM_Slot2 );
				FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer + 1 , FM_Slot2 );
				FM_TSlot2 = _SCH_MODULE_Get_BM_WAFER( FM_Buffer + 1 , FM_Slot2 );
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				//----
				// Code for Pick from BO
				// FM_Slot
				//----------------------------------------------------------------------
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) { // 2008.01.15
					_SCH_LOG_LOT_PRINTF( FM_dbSide , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
					return 0;
				}
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer + 1 ) < 0 ) { // 2008.01.15
					_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
					return 0;
				}
				//-----------------------------------------------
				FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
				//-----------------------------------------------
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2006.01.06
					//----------------------------------------------------------------------
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 410 , FM_Buffer , FM_TSlot , FM_Slot , FM_dbSide , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 + _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) , FM_dbSide2 , FM_Pointer2 , ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( FM_Buffer , -1 ) - 1 ) <= 0 ) ) ) { // 2007.03.21
					case -1 :
						return 0;
						break;
					}
					//----------------------------------------------------------------------
					//========================================================================================
					SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
					//========================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 420 , FM_Buffer , FM_TSlot , FM_Slot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , PICK_FROM_BM_FM_MODE_4( FM_dbSide , FM_Pointer ) , -1 );
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 430 , 0 , 0 , 0 , 0 , 0 , FM_Buffer + 1 , FM_TSlot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , -1 , PICK_FROM_BM_FM_MODE_4( FM_dbSide2 , FM_Pointer2 ) );
				}
				else { // 2006.01.06
					//----------------------------------------------------------------------
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 440 , FM_Buffer , FM_TSlot , FM_Slot , FM_dbSide , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 , ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( FM_Buffer , -1 ) - 1 ) <= 0 ) ) ) { // 2007.03.21
					case -1 :
						return 0;
						break;
					}
					//----------------------------------------------------------------------
					//========================================================================================
					SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
					//========================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 450 , FM_Buffer , FM_TSlot , FM_Slot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , PICK_FROM_BM_FM_MODE_4( FM_dbSide , FM_Pointer ) , -1 );
					//----------------------------------------------------------------------
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 460 , FM_Buffer + 1 , 0 , 0 , 0 , 0 , FM_Buffer + 1 , FM_TSlot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , ( ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( FM_Buffer , -1 ) - 1 ) <= 0 ) ) ) { // 2007.03.21
					case -1 :
						return 0;
						break;
					}
					//----------------------------------------------------------------------
					//========================================================================================
					SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
					//========================================================================================
					_SCH_MACRO_FM_DATABASE_OPERATION( 0 , MACRO_PICK + 470 , 0 , 0 , 0 , 0 , 0 , FM_Buffer + 1 , FM_TSlot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , -1 , PICK_FROM_BM_FM_MODE_4( FM_dbSide2 , FM_Pointer2 ) );
				}
				//----------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1 from FEM%cWHFMFAIL 1\n" , 9 );
					return 0;
				}
				//-----------------------------------------------
				_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 22 );
				//-----------------------------------------------
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide  , FM_Pointer , 0 );
				_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide2 , FM_Pointer2 , 0 );
				//-----------------------------------------------
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) {
					if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( FM_dbSide , FM_Buffer , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( FM_Buffer - BM1 + PM1 ) ) ) {
						//=================================================================================================================
						// 2005.10.18
						//=================================================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 2 from FEM%cWHFMFAIL 2\n" , 9 );
							return 0;
						}
						//=================================================================================================================
						//==================================================================================
						SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 1 );
						//==================================================================================
					}
					else {
						if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( FM_dbSide , FM_Buffer ) ) {
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 3 from FEM%cWHFMFAIL 3\n" , 9 );
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 2 );
							//==================================================================================
						}
						else {
							//==================================================================================
							SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 11 );
							//==================================================================================
						}
					}
				}
				else {
					if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( FM_dbSide , FM_Buffer ) ) {
						//=================================================================================================================
						// 2005.10.18
						//=================================================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 4 from FEM%cWHFMFAIL 4\n" , 9 );
							return 0;
						}
						//=================================================================================================================
						//==================================================================================
						SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 3 );
						//==================================================================================
					}
					else {
						//==================================================================================
						SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 12 );
						//==================================================================================
					}
				}
				//
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) {
					if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( FM_dbSide2 , FM_Buffer + 1 , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( ( FM_Buffer + 1 ) - BM1 + PM1 ) ) ) {
						//=================================================================================================================
						// 2005.10.18
						//=================================================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ( FM_Buffer + 1 ) - BM1 + PM1 ) == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 5 from FEM%cWHFMFAIL 5\n" , 9 );
							return 0;
						}
						//=================================================================================================================
						//==================================================================================
						SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 1 );
						//==================================================================================
					}
					else {
						if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( FM_dbSide2 , FM_Buffer + 1 ) ) {
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ( FM_Buffer + 1 ) - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 6 from FEM%cWHFMFAIL 6\n" , 9 );
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 2 );
							//==================================================================================
						}
						else {
							//==================================================================================
							SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 11 );
							//==================================================================================
						}
					}
				}
				else {
					if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( FM_dbSide2 , FM_Buffer + 1 ) ) {
						//=================================================================================================================
						// 2005.10.18
						//=================================================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ( FM_Buffer + 1 ) - BM1 + PM1 ) == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 7 from FEM%cWHFMFAIL 7\n" , 9 );
							return 0;
						}
						//=================================================================================================================
						//==================================================================================
						SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 3 );
						//==================================================================================
					}
					else {
						//==================================================================================
						SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 12 );
						//==================================================================================
					}
				}
			}
			else if ( ( FM_Slot > 0 ) || ( FM_Slot2 > 0 ) ) { // 2005.10.06
				if ( FM_Slot > 0 ) {
					FM_dbSide = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot );
					FM_Pointer = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot );
					FM_TSlot = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot );
				}
				else {
					FM_TSlot = 0;
					FM_Pointer = 0;
				}
				if ( FM_Slot2 > 0 ) {
					if ( ( FM_Slot > 0 ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) ) { // 2007.01.08
						FM_dbSide2 = _SCH_MODULE_Get_BM_SIDE( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_Slot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() );
						FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_Slot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() );
						FM_TSlot2   = _SCH_MODULE_Get_BM_WAFER( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_Slot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() );
					}
					else {
						FM_dbSide2 = _SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot2 );
						FM_Pointer2 = _SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot2 );
						FM_TSlot2 = _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot2 );
					}
				}
				else {
					FM_TSlot2 = 0;
					FM_Pointer2 = 0;
				}
				//==================================================================================
				// 2006.09.27
				//==================================================================================
//					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2007.01.06
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.05 // 2007.01.06
					switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( FM_Buffer ) ) {
					case 0 :
						if ( FM_Slot > 0 ) {
							if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
								if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , FM_Pointer ) ) {
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
								}
								else { // 2006.10.19
									//==========================================================
									_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer , 1 , &FM_CM , &garbage ); // 2007.07.11
									//==========================================================
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 2 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
									//==========================================================
								}
							}
							else { // 2006.10.19
								//==========================================================
								_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer , 1 , &FM_CM , &garbage ); // 2007.07.11
								//==========================================================
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 2 ) {
									FM_Slot = 0;
									FM_TSlot = 0;
								}
								//==========================================================
							}
						}
						if ( FM_Slot2 > 0 ) {
							if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
								if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , FM_Pointer2 ) ) {
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
								}
								else { // 2006.10.19
									//==========================================================
									_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer2 , 1 , &FM_CM , &garbage ); // 2007.07.11
									//==========================================================
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 1 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
									//==========================================================
								}
							}
							else { // 2006.10.19
								//==========================================================
								_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer2 , 1 , &FM_CM , &garbage ); // 2007.07.11
								//==========================================================
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 1 ) {
									FM_Slot2 = 0;
									FM_TSlot2 = 0;
								}
								//==========================================================
							}
						}
						break;
					case 1 : // arm A
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) { // 2006.10.17
							FM_Slot2 = 0;
							FM_TSlot2 = 0;
						}
						else if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) { // 2006.10.17
							if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) { // 2006.10.17
								FM_Slot = FM_Slot2;
								FM_TSlot = FM_TSlot2;
								FM_Pointer = FM_Pointer2;
							}
							FM_Slot2 = 0;
							FM_TSlot2 = 0;
						}
						//
						if ( FM_Slot > 0 ) {
							if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
								if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , FM_Pointer ) ) {
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
								}
								else { // 2006.10.19
									//==========================================================
									_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer , 1 , &FM_CM , &garbage ); // 2007.07.11
									//==========================================================
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 2 ) {
										FM_Slot = 0;
										FM_TSlot = 0;
									}
									//==========================================================
								}
							}
							else { // 2006.10.19
								//==========================================================
								_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer , 1 , &FM_CM , &garbage ); // 2007.07.11
								//==========================================================
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 2 ) {
									FM_Slot = 0;
									FM_TSlot = 0;
								}
								//==========================================================
							}
						}
						break;
					case 2 : // arm B
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) { // 2006.10.17
							FM_Slot = 0;
							FM_TSlot = 0;
						}
						else if ( ( FM_Slot2 <= 0 ) && ( FM_Slot > 0 ) ) { // 2006.10.17
							if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) { // 2006.10.17
								FM_Slot2 = FM_Slot;
								FM_TSlot2 = FM_TSlot;
								FM_Pointer2 = FM_Pointer;
							}
							//
							FM_Slot = 0;
							FM_TSlot = 0;
						}
						//
						if ( FM_Slot2 > 0 ) {
							if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
								if ( !SCHEDULER_COOLING_SKIP_AL4( CHECKING_SIDE , FM_Pointer2 ) ) {
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
								}
								else { // 2006.10.19
									//==========================================================
									_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer2 , 1 , &FM_CM , &garbage ); // 2007.07.11
									//==========================================================
									if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 1 ) {
										FM_Slot2 = 0;
										FM_TSlot2 = 0;
									}
									//==========================================================
								}
							}
							else { // 2006.10.19
								//==========================================================
								_SCH_SUB_GET_OUT_CM_AND_SLOT( CHECKING_SIDE , FM_Pointer2 , 1 , &FM_CM , &garbage ); // 2007.07.11
								//==========================================================
								if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CM ) == 1 ) {
									FM_Slot2 = 0;
									FM_TSlot2 = 0;
								}
								//==========================================================
							}
						}
						break;
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 05 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
				//----------------------------------------------------------------------
				if ( ( FM_Slot > 0 ) || ( FM_Slot2 > 0 ) ) { // 2006.09.27
					//----------------------------------------------------------------------
					//----
					// Code for Pick from BO
					// FM_Slot
					//----------------------------------------------------------------------
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) { // 2008.01.15
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
						return 0;
					}
					//--------
					//============================================================================================
					//--------
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2016.06.07
						//

//		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling CHECK 1 [%d,%d]%c\n" , FM_Slot , FM_Slot2 , 9 );

						k = 0;

						while( TRUE ) {
							//----------------------------------------------------------------------
							//
							if ( 0 == SCHEDULER_CONTROL_Chk_PICK_BM_FMSIDE_POSSIBLE_3MODE_FOR_4( CHECKING_SIDE ) ) break;
							//
							if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
								return 0;
							}
							//
							k++;
							//
							//----------------------------------------------------------------------
							Sleep(1);
							//----------------------------------------------------------------------
						}

//		_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling CHECK 2 [%d,%d] [%d]%c\n" , FM_Slot , FM_Slot2 , k , 9 );

					}
					//--------
					//============================================================================================
					//--------
					k = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT_STYLE_4( FM_Buffer , -1 );
					if ( FM_Slot > 0 ) k = k - 1;
					if ( FM_Slot2 > 0 ) k = k - 1;
					//--------
					//======================================================================================================
					ALsts = 0; // 2007.08.02
// 2007.08.21
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
						if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) { // 2007.08.21
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								ALsts = SCHEDULER_CONTROL_Pick_BM_3_1_Swapping_Multi_Mode( FM_Buffer , FM_Slot ); // 2007.08.22
							}
						}
					}
					//======================================================================================================
					//======================================================================================================
					// 2006.12.15
					//======================================================================================================
					_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
					//======================================================================================================
					FM_TSlot_Real = 0;
					if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								//======================================================================================================
								_SCH_FMARMMULTI_DATA_SET_FROM_BM( 0 , FM_Buffer , FM_Slot , 0 , 2 ); // 2007.08.01
								//======================================================================================================
								_SCH_FMARMMULTI_FA_SUBSTRATE( FM_Buffer , FM_Slot + 10000 , FA_SUBST_RUNNING ); // 2007.07.11
								//======================================================================================================
								FM_TSlot_Real = 0;
								FM_Slot_Real = FM_Slot;
								//======================================================================================================
							}
							else {
								FM_Slot_Real = FM_Slot;
							}
						}
						else {
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								//===============================================================================================================
								_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , FM_Slot , &FM_Slot_Real , &FM_TSlot_Real );
								//======================================================================================================
								_SCH_FMARMMULTI_DATA_SET_FROM_BM( 0 , FM_Buffer , FM_Slot_Real , FM_TSlot_Real , 0 ); // 2007.08.01
								//======================================================================================================
								_SCH_FMARMMULTI_FA_SUBSTRATE( FM_Buffer , FM_Slot_Real , FA_SUBST_RUNNING ); // 2007.07.11
								//======================================================================================================
							}
							else {
								FM_Slot_Real = FM_Slot;
							}
						}
					}
					else {
						FM_Slot_Real = FM_Slot;
					}
					//======================================================================================================
					//-----------------------------------------------
					FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
					//-----------------------------------------------
					if ( ( ALsts == 1 ) && ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 510 , FM_Buffer , FM_TSlot , ( FM_Slot_Real * 10000 ) + FM_Slot, CHECKING_SIDE , FM_Pointer , 0 , 0 , 0 , 0 , 0 , ( k <= 0 ) ) ) { // 2007.03.21
						case -1 :
							return 0;
							break;
						}
						//
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 520 , 0 , 0 , 0 , 0 , 0 , FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_TSlot2 , FM_Slot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , FM_dbSide2 , FM_Pointer2 , ( k <= 0 ) ) ) { // 2007.03.21
						case -1 :
							return 0;
							break;
						}
					}
					else {
						//===================================================================================================
//						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK , FM_Buffer , FM_TSlot , ( FM_Slot_Real * 10000 ) + FM_Slot, CHECKING_SIDE , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , CHECKING_SIDE , FM_Pointer2 , ( k <= 0 ) ) ) { // 2007.03.21 // 2007.08.02
						switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 530 , FM_Buffer , FM_TSlot , ( FM_Slot_Real * 10000 ) + FM_Slot, CHECKING_SIDE , FM_Pointer , FM_Buffer , FM_TSlot2 , FM_Slot2 , FM_dbSide2 , FM_Pointer2 , ( k <= 0 ) ) ) { // 2007.03.21
						case -1 :
							return 0;
							break;
						}
						//=========================================================================
					}
					//=============================================================================================================================================
					SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
					//=============================================================================================================================================
					//=============================================================================================================================================
					//
					FM_dbSide = CHECKING_SIDE; // 2007.08.21
					//
					if ( FM_Slot > 0 ) {
						//========================================================================================
						_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , PICK_FROM_BM_FM_MODE_4( FM_dbSide , FM_Pointer ) ); // 2004.08.11 // 2005.09.06
						_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , FM_TSlot ); // 2004.08.13
						//===============================================================================================================
						// 2006.11.10
						//===============================================================================================================
//							if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
						//===============================================================================================================
						_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_Slot , 0 , BUFFER_READY_STATUS );
					}
					if ( FM_Slot2 > 0 ) {
						//========================================================================================
						_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , PICK_FROM_BM_FM_MODE_4( FM_dbSide2 , FM_Pointer2 ) ); // 2004.08.11 // 2005.09.06
						_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , FM_TSlot2 );// 2004.08.13
						//========================================================================================
						if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) {
							if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
								if ( FM_Slot > 0 ) {
									//======================================================================================================
									sch4_FM_WAFER_3LASTPICKBM_TB = ( ( FM_Slot2 - 1 ) / 2 ) + 1;
									//======================================================================================================
								}
								else {
									//======================================================================================================
									sch4_FM_WAFER_3LASTPICKBM_TB = FM_Buffer - BM1 + 1;
									//======================================================================================================
								}
							}
						}
						//===============================================================================================================
						// 2006.11.10
						//===============================================================================================================
//							if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
						//===============================================================================================================
						if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && ( FM_Slot > 0 ) ) { // 2007.01.06
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_Slot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , 0 , BUFFER_READY_STATUS );
						}
						else {
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_Slot2 , 0 , BUFFER_READY_STATUS );
						}
					}
					//==========================================================================
					// 2006.12.15
					//==========================================================================
					_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( FM_Buffer , 0 , ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) ? 12 : 11 ); // 2007.07.11
					//==========================================================================
					//======================================================================================================
					// 2007.09.03
					//======================================================================================================
					if ( ( ALsts != 1 ) && ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
						//===================================================================================================
						// 2007.09.03
						//=========================================================================
						FM_Has_Swap_Wafer_Style_4 = TRUE;
						//=========================================================================
					}
					else {
						//===================================================================================================
						// 2007.09.03
						//=========================================================================
						FM_Has_Swap_Wafer_Style_4 = FALSE;
						//=========================================================================
					}
					//======================================================================================================
					//----------------------------------------------------------------------
					//----------------------------------------------------------------------
					if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 8 from FEM%cWHFMFAIL 8\n" , 9 );
						return 0;
					}
					//-----------------------------------------------
					_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 22 );
					//-----------------------------------------------
					if ( FM_Slot > 0 ) {
	//					_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , FM_Pointer , 0 , FM_Pointer ); // 2003.08.11 // 2005.01.26
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide , FM_Pointer , 0 ); // 2003.08.11 // 2005.01.26
					}
					if ( FM_Slot2 > 0 ) {
	//					_SCH_CLUSTER_Check_and_Make_Return_Wafer( CHECKING_SIDE , FM_Pointer2 , 0 , FM_Pointer2 ); // 2003.08.11 // 2005.01.26
						_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide2 , FM_Pointer2 , 0 ); // 2003.08.11 // 2005.01.26
					}
					//-------------------------------------------------------------------------------------
	//				if ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 1 ) { // 2005.01.24
					if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2005.01.24
						if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( CHECKING_SIDE , FM_Buffer , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( FM_Buffer - BM1 + PM1 ) ) ) {
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 9 from FEM%cWHFMFAIL 9\n" , 9 );
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							// 2007.03.22
							//==================================================================================
							if ( SCHEDULER_CONTROL_Chk_AFTER_PICK_BM_AND_PUMP_FOR_4( CHECKING_SIDE , FALSE , FM_Buffer , FM_Slot , FM_Slot2 , 1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 101 from FEM%cWHFMFAIL 101\n" , 9 );
								return 0;
							}
							//==================================================================================
						}
						else {
							if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer ) ) {
								//=================================================================================================================
								// 2005.10.18
								//=================================================================================================================
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 10 from FEM%cWHFMFAIL 10\n" , 9 );
									return 0;
								}
								//=================================================================================================================
								//==================================================================================
								// 2007.03.22
								//==================================================================================
								if ( SCHEDULER_CONTROL_Chk_AFTER_PICK_BM_AND_PUMP_FOR_4( CHECKING_SIDE , FALSE , FM_Buffer , FM_Slot , FM_Slot2 , 2 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 101 from FEM%cWHFMFAIL 101\n" , 9 );
									return 0;
								}
								//==================================================================================
							}
							else { // 2005.10.05
								//==================================================================================
								// 2007.03.22
								//==================================================================================
								if ( SCHEDULER_CONTROL_Chk_AFTER_PICK_BM_AND_PUMP_FOR_4( CHECKING_SIDE , TRUE , FM_Buffer , FM_Slot , FM_Slot2 , 11 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 101 from FEM%cWHFMFAIL 101\n" , 9 );
									return 0;
								}
								//==================================================================================
							}
						}
					}
					else {
						if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer ) ) {
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 11 from FEM%cWHFMFAIL 11\n" , 9 );
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							SCHEDULER_CONTROL_BM_PUMPING_FM_PART( CHECKING_SIDE , TRUE , FM_Buffer , 3 ); // 2005.09.09
							//==================================================================================
						}
						else { // 2005.10.05
							//==================================================================================
							SCHEDULER_CONTROL_BMX_PUMPING_FM_PART( CHECKING_SIDE , TRUE , FM_Buffer , 12 );
							//==================================================================================
						}
					}
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 06 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
		//----------------------------------------------------------------------
		//===================================================================================================================
		// 2004.08.13
		//===================================================================================================================
//			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2007.01.08
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2007.01.08
			sbres = 0;
			//=========================================================================================================
			// 2005.09.06
			//=========================================================================================================
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				//=========================================================================================================
				// 2005.11.28
				//=========================================================================================================
				if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
					//===========================================================================================
					if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 0 ) { // 2006.09.27
					//===========================================================================================
						if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts1 , &ICsts2 , -1 , -1 ) >= 2 ) {
//								if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2008.02.11
							if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) != PATHDO_WAFER_RETURN ) ) { // 2008.02.11
//									if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) { // 2008.02.11
								if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) != PATHDO_WAFER_RETURN ) ) { // 2008.02.11
									sbres = 3;
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FingerCount = 1;
									FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
								}
							}
						}
					}
				}
				//=========================================================================================================
				if ( sbres == 0 ) {
					//===========================================================================================
//						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) { // 2006.09.27
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) != 2 ) ) { // 2006.09.27
						if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts1 , &ICsts2 , -1 , -1 ) > 0 ) {
//								if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2008.02.11
							if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) != PATHDO_WAFER_RETURN ) ) { // 2008.02.11
								if ( SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( CHECKING_SIDE ) ) {
									sbres       = 2;
									ICsts       = ICsts1;
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FM_TSlot2   = 0;
									FM_Pointer2 = 0;
									FingerCount = 1;
									FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
								}
							}
						}
					}
//						else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) { // 2006.09.27
					else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) != 1 ) ) { // 2006.09.27
						if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts1 , &ICsts2 , -1 , -1 ) > 0 ) {
//								if ( ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2008.02.11
							if ( ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) != PATHDO_WAFER_RETURN ) ) { // 2008.02.11
								if ( SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( CHECKING_SIDE ) ) {
									sbres       = 2;
									ICsts       = ICsts1;
									FM_TSlot    = 0;
									FM_Pointer  = 0;
									FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FingerCount = 1;
									FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer2 );
								}
							}
						}
					}
				}
			}
			//=========================================================================================================
			if ( sbres == 0 ) {
				//=========================================================================================================
				// 2004.11.02
				//=========================================================================================================
				if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
					if (
//						( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 1 ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) && // 2005.09.06
//						( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 1 ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) // 2005.09.06
						( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) && // 2005.09.06
						( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) // 2005.09.06
						) {
						if      ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) {
							sbres       = 1;
							FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
							FingerCount = 0;
						}
						else if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) != CHECKING_SIDE ) ) {
							sbres       = 1;
							FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_TSlot2   = 0;
							FM_Pointer2 = 0;
							FingerCount = 0;
						}
						else if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) {
							sbres       = 1;
							FM_TSlot    = 0;
							FM_Pointer  = 0;
							FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
							FingerCount = 0;
						}
					}
				}
				else if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
					if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
//						if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 1 ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) ) { // 2005.09.06
						if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) ) { // 2005.09.06
							if ( SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( CHECKING_SIDE ) ) {
								sbres       = 1;
								FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								FM_TSlot2   = 0;
								FM_Pointer2 = 0;
								FingerCount = 0;
							}
						}
					}
					else { // 2005.09.06
						if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
							if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) ) { // 2005.09.06
								sbres       = 1;
								FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								FM_TSlot2   = 0;
								FM_Pointer2 = 0;
								FingerCount = 0;
							}
						}
					}
				}
				else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
					if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) {
//						if ( ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 1 ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) ) { // 2005.09.06
						if ( ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) ) { // 2005.09.06
							if ( SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( CHECKING_SIDE ) ) {
								sbres       = 1;
								FM_TSlot    = 0;
								FM_Pointer  = 0;
								FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								FingerCount = 0;
							}
						}
					}
					else { // 2005.09.06
						if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
							if ( ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) ) { // 2005.09.06
								sbres       = 1;
								FM_TSlot    = 0;
								FM_Pointer  = 0;
								FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								FingerCount = 0;
							}
						}
					}
				}
			}
			//=========================================================================================================
			if ( sbres == 0 ) { // 2004.11.02
				if ( SignalCount == 0 ) { // 2004.08.24
					FingerCount = 1;
				}
				else {
					if ( FingerCount == 0 ) {
						FingerCount = 1;
					}
					else {
						//========================================================================================================
						// 2006.11.16
						//========================================================================================================
//							if (
//								_SCH_MODULE_Need_Do_Multi_FAL() &&
//								( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) &&
//								( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) )
//								) {
//								FingerCount = 1; // 2004.08.17
//							}
						//========================================================================================================
						if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) {
							if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) ) {
								if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TA_STATION , TRUE ) ) {
									FingerCount = 0;
								}
							}
							if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) ) {
								if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TB_STATION , TRUE ) ) {
									FingerCount = 0;
								}
							}
						}
						//========================================================================================================
						// 2006.11.16
						//========================================================================================================
						if ( FingerCount == 0 ) {
							FingerCount = 1;
						}
						//========================================================================================================
						else {
//							if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 1 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) { // 2004.10.01
							if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) { // 2004.10.01
//									if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 1 ) || ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) { // 2004.10.01 // 2005.09.06
								if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) == PATHDO_WAFER_RETURN ) ) { // 2004.10.01 // 2005.09.06
									FM_TSlot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									FingerCount = 0;
								}
								else if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) { // 2005.09.06
									if ( ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) { // 2005.09.14
										FM_TSlot = 0;
										FM_Pointer = 0;
									}
									else {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 2 ) { // 2006.09.27
											FM_TSlot = 0;
											FM_Pointer = 0;
										}
										else {
											if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts1 , &ICsts2 , -1 , -1 ) > 0 ) {
												sbres       = 2;
												ICsts		= ICsts1;
												FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
												FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
												FM_TSlot2   = 0;
												FM_Pointer2 = 0;
												FingerCount = 1;
												FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
											}
											else {
												FM_TSlot = 0;
												FM_Pointer = 0;
											}
										}
									}
								}
								else { // 2004.10.01
									FM_TSlot = 0;
									FM_Pointer = 0;
								}
							}
							else {
								FM_TSlot = 0;
								FM_Pointer = 0;
							}
							if ( sbres == 0 ) {
	//							if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 1 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) { // 2004.10.01
								if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) { // 2004.10.01
//									if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 1 ) || ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) { // 2004.10.01 // 2005.09.06
									if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) || ( _SCH_CLUSTER_Get_PathDo( CHECKING_SIDE , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) == PATHDO_WAFER_RETURN ) ) { // 2004.10.01 // 2005.09.06
										FM_TSlot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										FingerCount = 0;
									}
									else if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) { // 2005.09.06
										if ( ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) { // 2005.09.14
											FM_TSlot2 = 0;
											FM_Pointer2 = 0;
										}
										else {
											if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) == 1 ) { // 2006.09.27
												FM_TSlot2 = 0;
												FM_Pointer2 = 0;
											}
											else {
												if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts1 , &ICsts2 , -1 , -1 ) > 0 ) {
													sbres       = 2;
													ICsts		= ICsts1;
													FM_TSlot    = 0;
													FM_Pointer  = 0;
													FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
													FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
													FingerCount = 1;
													FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer2 );
												}
												else {
													FM_TSlot2 = 0;
													FM_Pointer2 = 0;
												}
											}
										}
									}
									else { // 2004.10.01
										FM_TSlot2 = 0;
										FM_Pointer2 = 0;
									}
								}
								else {
									FM_TSlot2 = 0;
									FM_Pointer2 = 0;
								}
							}
							//======================================================
							if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) { // 2005.09.06
								FingerCount = 1;
							}
							//======================================================
						}
					}
				}
			}
			//
			if ( sbres == 2 ) { // 2005.09.06
				//-----------------------------------------------
				FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
				//-----------------------------------------------
				FingerCount = 1;
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , ( FM_TSlot > 0 ? ICsts : 0 ), ( FM_TSlot2 > 0 ? ICsts : 0 ) , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot2 * 100 + FM_TSlot , 9 , FM_TSlot2 * 100 + FM_TSlot , 9 , FM_TSlot2 * 100 + FM_TSlot );
					return 0;
				}
				//========================================================================================
				SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
				//========================================================================================
				if ( FM_TSlot > 0 ) {
//						_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
					//
					_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , CHECKING_SIDE , FM_Pointer );
					_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_TSlot );
					//
					_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
					//============================================================================
					_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , ICsts , 0 , FM_TSlot , FM_TSlot2 , FM_CM );
					//============================================================================
				}
				else {
//						_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
					//
					_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , CHECKING_SIDE , FM_Pointer2 );
					_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_TSlot2 );
					//
					_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
					//============================================================================
					_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , 0 , ICsts , FM_TSlot , FM_TSlot2 , FM_CM );
					//============================================================================
				}
			}
			else if ( sbres == 3 ) { // 2005.11.28
				//-----------------------------------------------
				FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
				//-----------------------------------------------
				FingerCount = 1;
				if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_ALL , CHECKING_SIDE , IC , ICsts1 , ICsts2 , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot2 , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot2 * 100 + FM_TSlot , 9 , FM_TSlot2 * 100 + FM_TSlot , 9 , FM_TSlot2 * 100 + FM_TSlot );
					return 0;
				}
				//========================================================================================
				SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
				//========================================================================================
				//
				_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts1 , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , FM_Pointer );
				//
//					_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
				//
				_SCH_MODULE_Set_MFIC_WAFER( ICsts1 , FM_TSlot );
				//
				_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
				//
				_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts2 , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Pointer2 );
				//
//					_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
				//
				_SCH_MODULE_Set_MFIC_WAFER( ICsts2 , FM_TSlot2 );
				//
				_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
				//============================================================================
				_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , ICsts1 , ICsts2 , FM_TSlot , FM_TSlot2 , FM_CM );
				//============================================================================
				ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( CHECKING_SIDE , &FM_Side2 , &ICsts , &ICsts2 );
				if ( ALsts == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
					return 0;
				}
				else if ( ALsts == SYS_SUCCESS ) {
					if ( ( ICsts > 0 ) && ( ICsts2 > 0 ) ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at IC(%d:%d)%cWHFMICSUCCESS %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
						//-----------------------------------------------
						FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
						//-----------------------------------------------
						//---------------------------------------------------------------------------------------
						FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) );
						//---------------------------------------------------------------------------------------
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , CHECKING_SIDE , IC , ICsts , ICsts2 , FM_CM , -1 , TRUE , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , CHECKING_SIDE , FM_Side2 , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) ) == SYS_ABORTED ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
							return 0;
						}
						//========================================================================================
						SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
						//========================================================================================
						_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , FMWFR_PICK_BM_DONE_IC );
						_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
						//========================================================================================
						_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
						//========================================================================================
						_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) , FMWFR_PICK_BM_DONE_IC );
						_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_Side2 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) );
						//========================================================================================
						_SCH_MODULE_Set_MFIC_WAFER( ICsts2 , 0 );
						//========================================================================================
					}
				}
			}
		}
		//===================================================================================================================
		// 2006.12.18
		//===================================================================================================================
		else {
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				if ( 
//						( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) || // 2008.02.11
//						( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) // 2008.02.11
					( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ) != PATHDO_WAFER_RETURN ) ) || // 2008.02.11
					( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_CLUSTER_Get_PathDo( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) != PATHDO_WAFER_RETURN ) ) // 2008.02.11
				) {
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
						if ( SCHEDULER_CONTROL_Get_Free_OneBody_3_FIC_Slot( &ICsts , &ICsts2 ) ) { // 2007.01.08
							FM_dbSide   = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
							FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							//
							FM_dbSide2  = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
							FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );

							FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
							FingerCount = 1;
							//-----------------------------------------------
							FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
							//-----------------------------------------------
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_ALL , CHECKING_SIDE , IC , ICsts , ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
								return 0;
							}
							//========================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( -1 );
							//========================================================================================
							_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , FM_dbSide , FM_Pointer );
							_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_TSlot );
							//
							_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts ); // 2007.07.11
							//
							_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , FM_dbSide2 , FM_Pointer2 );
							_SCH_MODULE_Set_MFIC_WAFER( ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , FM_TSlot2 );
							//
							_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 );
							_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 );
							//============================================================================
							_SCH_MACRO_SPAWN_FM_MCOOLING( FM_dbSide , ICsts , 0 , FM_TSlot , 0 , FM_CM );
							//============================================================================
							_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts ); // 2007.07.11
							//============================================================================
							_SCH_MACRO_SPAWN_FM_MCOOLING( FM_dbSide2 , ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , 0 , FM_TSlot2 , 0 , FM_CM );
							//============================================================================
							//============================================================================
							// 2007.09.03
							//============================================================================
							if ( !FM_Has_Swap_Wafer_Style_4 ) {
								SCHEDULER_SWAPPING_3_1_REMAP_FIC_ONEBODY3( ICsts ); // 2007.09.03
							}
							//============================================================================
						}
					}
					else {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) { // 2007.05.10
							if ( SCHEDULER_CONTROL_Get_Free_OneBody_3_FIC_Slot( &ICsts , &ICsts2 ) ) { // 2007.01.08
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) { // 2007.04.02
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								}
								else {
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								}
								FM_TSlot2   = 0;
								FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
								FingerCount = 1;
								//
								//============================================================================
								// 2007.09.03
								//============================================================================
								if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
									if ( FM_Has_Swap_Wafer_Style_4 ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail(101) at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
										return 0;
									}
								}
								//============================================================================
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ? ICsts : 0 , ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ? 0 : ICsts , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
									return 0;
								}
								//============================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 );
								//========================================================================================
								_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , CHECKING_SIDE , FM_Pointer );
								_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_TSlot );
								//
								_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( ICsts ); // 2007.07.11
								//
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 );
									//============================================================================
									_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , ICsts , 0 , FM_TSlot , 0 , FM_CM );
									//============================================================================
								}
								else {
									_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 );
									//============================================================================
									_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , 0 , ICsts , 0 , FM_TSlot , FM_CM );
									//============================================================================
								}
								//============================================================================
								_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( ICsts ); // 2007.07.11
								//============================================================================
							}
						}
						else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) { // 2007.05.10
							//=========================================================================================================
							// 2007.05.10
							//=========================================================================================================
							if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE2_SLOT( sch4_FM_WAFER_3LASTPICKBM_TB , 4 , &ICsts , &ICsts2 ) > 0 ) {
								FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
								FingerCount = 1;
								//
								//============================================================================
								// 2007.09.03
								//============================================================================
								if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
									if ( FM_Has_Swap_Wafer_Style_4 ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail(102) at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
										return 0;
									}
								}
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								//============================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , 0 , ICsts , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 );
								//========================================================================================
								_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , CHECKING_SIDE , FM_Pointer );
								_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_TSlot );
								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 );
								//============================================================================
								_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , 0 , ICsts , 0 , FM_TSlot , FM_CM );
								//============================================================================
							}
							else {
								//=========================================================================================================
								if ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts , &ICsts2 , -1 , -1 ) > 0 ) {
									FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
									FingerCount = 1;
									//============================================================================
									// 2007.09.03
									//============================================================================
									if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
										if ( FM_Has_Swap_Wafer_Style_4 ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail(103) at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
											return 0;
										}
									}
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
									//============================================================================
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , IC , 0 , ICsts , FM_CM , FM_CM , TRUE , FM_TSlot , FM_TSlot , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_TSlot , 9 , FM_TSlot , 9 , FM_TSlot );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , CHECKING_SIDE , FM_Pointer );
									_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_TSlot );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 );
									//============================================================================
									_SCH_MACRO_SPAWN_FM_MCOOLING( CHECKING_SIDE , 0 , ICsts , 0 , FM_TSlot , FM_CM );
									//============================================================================
								}
								//=========================================================================================================
							}
						}
					}
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 07 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][sbres=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , sbres );
		//----------------------------------------------------------------------
		//=========================================================================================================
		// 2006.10.17
		//=========================================================================================================
		if ( FingerCount != 0 ) {
			if ( !_SCH_MODULE_Need_Do_Multi_FAL() || ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) {
				if ( !_SCH_MODULE_Need_Do_Multi_FIC() || ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() > 0 ) ) {
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.08
						if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) {
							FingerCount = 0;
							FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
						}
						else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) {
							FingerCount = 0;
							FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_TSlot2   = 0;
							FM_Pointer2 = 0;
						}
						else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) {
							FingerCount = 0;
							FM_TSlot    = 0;
							FM_Pointer  = 0;
							FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
						}
					}
					else {
						if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) && ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) ) {
							FingerCount = 0;
							FM_TSlot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_TSlot2   = 0;
							FM_Pointer2 = 0;
						}
						else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) {
							FingerCount = 0;
							FM_TSlot    = 0;
							FM_Pointer  = 0;
							FM_TSlot2   = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
						}
					}
				}
			}
		}
		else {
			//=========================================================================================================
			// 2006.11.17
			//=========================================================================================================
			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2007.01.08
				if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) {
					if ( SYS_SUCCESS == _SCH_MACRO_CHECK_FM_MALIGNING( _SCH_MODULE_Get_MFAL_SIDE() , FALSE ) ) {
						if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) {
								if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TA_STATION , TRUE ) ) {
									FingerCount = 1;
								}
							}
						}
						if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) {
								if ( SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TB_STATION , TRUE ) ) {
									FingerCount = 1;
								}
							}
						}
					}
				}
			}
		}
		//===================================================================================================================
		if ( FingerCount == 0 ) { // 2004.08.13
			//-----------------------------------------------
			_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
			//----------------------------------------------------------------------
			if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
				FM_dbSide  = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
				FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
			}
			else {
				FM_dbSide  = CHECKING_SIDE;
				FM_dbSide2 = CHECKING_SIDE;
			}
			//----------------------------------------------------------------------
			retdata = -1; // 2007.07.09
			//----------------------------------------------------------------------
			if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
				if ( FM_Pointer >= 100 ) {
					FM_CO1 = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
					FM_OSlot = FM_TSlot;
				}
				else {
					if ( ( _SCH_CLUSTER_Get_PathDo( FM_dbSide , FM_Pointer ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( FM_dbSide , FM_Pointer ) == 0 ) ) {
						//----------------------------------------------------------------------
						retdata = 2; // 2007.07.09
						//----------------------------------------------------------------------
						FM_CO1 = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
						FM_OSlot = _SCH_CLUSTER_Get_SlotIn( FM_dbSide , FM_Pointer );
					}
					else {
						FM_CO1 = _SCH_CLUSTER_Get_PathOut( FM_dbSide , FM_Pointer );
						FM_OSlot = _SCH_CLUSTER_Get_SlotOut( FM_dbSide , FM_Pointer );
					}
				}
				if ( FM_Pointer2 >= 100 ) {
					FM_CO2 = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
					FM_OSlot2 = FM_TSlot2;
				}
				else {
					if ( ( _SCH_CLUSTER_Get_PathDo( FM_dbSide2 , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( FM_dbSide2 , FM_Pointer2 ) == 0 ) ) {
						//----------------------------------------------------------------------
						retdata = 2; // 2007.07.09
						//----------------------------------------------------------------------
						FM_CO2 = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 );
						FM_OSlot2 = _SCH_CLUSTER_Get_SlotIn( FM_dbSide2 , FM_Pointer2 );
					}
					else {
						FM_CO2 = _SCH_CLUSTER_Get_PathOut( FM_dbSide2 , FM_Pointer2 );
						FM_OSlot2 = _SCH_CLUSTER_Get_SlotOut( FM_dbSide2 , FM_Pointer2 );
					}
				}
			}
			else if ( FM_TSlot > 0 ) {
				FM_CO2 = 0;
				FM_OSlot2 = 0;
				if ( FM_Pointer >= 100 ) {
					FM_CO1 = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
					FM_OSlot = FM_TSlot;
				}
				else {
					if ( ( _SCH_CLUSTER_Get_PathDo( FM_dbSide , FM_Pointer ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( FM_dbSide , FM_Pointer ) == 0 ) ) {
						//----------------------------------------------------------------------
						retdata = 2; // 2007.07.09
						//----------------------------------------------------------------------
						FM_CO1 = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer );
						FM_OSlot = _SCH_CLUSTER_Get_SlotIn( FM_dbSide , FM_Pointer );
					}
					else {
						FM_CO1 = _SCH_CLUSTER_Get_PathOut( FM_dbSide , FM_Pointer );
						FM_OSlot = _SCH_CLUSTER_Get_SlotOut( FM_dbSide , FM_Pointer );
					}
				}
			}
			else if ( FM_TSlot2 > 0 ) {
				FM_CO1 = 0;
				FM_OSlot = 0;
				if ( FM_Pointer2 >= 100 ) {
					FM_CO2 = _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
					FM_OSlot2 = FM_TSlot2;
				}
				else {
					if ( ( _SCH_CLUSTER_Get_PathDo( FM_dbSide2 , FM_Pointer2 ) == PATHDO_WAFER_RETURN ) || ( _SCH_CLUSTER_Get_PathDo( FM_dbSide2 , FM_Pointer2 ) == 0 ) ) {
						//----------------------------------------------------------------------
						retdata = 2; // 2007.07.09
						//----------------------------------------------------------------------
						FM_CO2 = _SCH_CLUSTER_Get_PathIn( FM_dbSide2 , FM_Pointer2 );
						FM_OSlot2 = _SCH_CLUSTER_Get_SlotIn( FM_dbSide2 , FM_Pointer2 );
					}
					else {
						FM_CO2 = _SCH_CLUSTER_Get_PathOut( FM_dbSide2 , FM_Pointer2 );
						FM_OSlot2 = _SCH_CLUSTER_Get_SlotOut( FM_dbSide2 , FM_Pointer2 );
					}
				}
			}
			//==================================================================================
			// 2006.09.27
			//==================================================================================
			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
				if ( FM_TSlot > 0 ) {
					if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CO1 ) == 2 ) FM_TSlot = 0;
				}
				if ( FM_TSlot2 > 0 ) {
					if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( FM_CO2 ) == 1 ) FM_TSlot2 = 0;
				}
			}
			//==================================================================================
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 08 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 );
			//----------------------------------------------------------------------
			//===============================================================
			if ( ( FM_TSlot > 0 ) || ( FM_TSlot2 > 0 ) ) { // 2006.09.27
			//===============================================================
				if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 100 ) ) {
					if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( FM_Pointer ) == 0 ) {
						//============================================================================================
						if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( FM_Buffer - BM1 + PM1 , 1 ) ) {
							if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( FM_dbSide , FM_Buffer - BM1 + PM1 ) == MUF_04_USE_to_PREND_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( FM_dbSide , FM_Buffer - BM1 + PM1 ) == MUF_05_USE_to_PRENDNA_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( FM_dbSide , FM_Buffer - BM1 + PM1 ) == MUF_71_USE_to_PREND_EF_XLP ) || ( _SCH_MODULE_Get_Mdl_Run_Flag( FM_Buffer - BM1 + PM1 ) <= MUF_00_NOTUSE ) ) { // 2006.01.17 // 2007.04.12
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( FM_dbSide , FM_Buffer - BM1 + PM1 , FM_Pointer , 0 , 105 );
							}
							else {
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( FM_dbSide , FM_Buffer - BM1 + PM1 , FM_Pointer , 0 , 106 );
							}
						}
						//============================================================================================
					}
				}
				if ( ( FM_TSlot2 > 0 ) && ( FM_Pointer2 >= 100 ) ) {
					if ( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_READY( FM_Pointer2 ) == 0 ) {
						//============================================================================================
						if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( FM_Buffer - BM1 + PM1 , 1 ) ) {
							if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( FM_dbSide2 , FM_Buffer - BM1 + PM1 ) == MUF_04_USE_to_PREND_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( FM_dbSide2 , FM_Buffer - BM1 + PM1 ) == MUF_05_USE_to_PRENDNA_EF_LP ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( FM_dbSide2 , FM_Buffer - BM1 + PM1 ) == MUF_71_USE_to_PREND_EF_XLP ) || ( _SCH_MODULE_Get_Mdl_Run_Flag( FM_Buffer - BM1 + PM1 ) <= MUF_00_NOTUSE ) ) { // 2006.01.17 // 2007.04.12
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( FM_dbSide2 , FM_Buffer - BM1 + PM1 , FM_Pointer2 , 0 , 107 );
							}
							else {
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( FM_dbSide2 , FM_Buffer - BM1 + PM1 , FM_Pointer2 , 0 , 108 );
							}
						}
						//============================================================================================
					}
				}
				//----------------------------------------------------------------------
				if ( !_SCH_MODULE_Need_Do_Multi_FIC() ) { // 2005.09.16 // 2006.12.18
					//-----------------------------------------------
					FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
					//-----------------------------------------------
					if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , IC , FM_TSlot , FM_TSlot2 , FM_CM , ( FM_TSlot > 0 ) ? FM_CO1 : FM_CO2 , TRUE , FM_TSlot , FM_TSlot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , FM_TSlot , FM_TSlot2 , 9 , FM_TSlot , FM_TSlot2 , 9 , FM_TSlot2 * 100 + FM_TSlot );
						return 0;
					}
					//========================================================================================
					SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
					//========================================================================================
				}
				//===============================================================================================================
				// 2005.09.06
				//===============================================================================================================
				if ( FM_TSlot > 0 ) {
					_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_BM_DONE_IC );
					//===============================================================================================================
					// 2006.11.10
					//===============================================================================================================
//						if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
					//===============================================================================================================
				}
				if ( FM_TSlot2 > 0 ) {
					_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_BM_DONE_IC );
					//===============================================================================================================
					// 2006.11.10
					//===============================================================================================================
//						if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
					//===============================================================================================================
				}
				//===============================================================================================================
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 13 from FEM%cWHFMFAIL 13\n" , 9 );
					return 0;
				}
				//=============================================================================================
				if ( SCHEDULER_CM_PLACE_SEPARATE_CHECK_ONEBODY3( FM_CO1 , FM_dbSide , FM_OSlot , FM_CO2 , FM_dbSide2 , FM_OSlot2 ) ) { // 2007.08.31
					//========================================================================
					// 2007.09.03
					//========================================================================
					if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
						if ( FM_Has_Swap_Wafer_Style_4 ) {
							return 0;
						}
					}
					//-----------------------------------------------
					FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
					//-----------------------------------------------
					//========================================================================
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 230 , FM_CO1 , FM_TSlot , FM_OSlot , FM_dbSide , FM_Pointer , 0 , 0 , 0 , 0 , 0 , retdata ) ) { // 2007.03.21 // 2007.07.09
					case -1 :
						return 0;
						break;
					}
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 240 , 0 , 0 , 0 , 0 , 0 , FM_CO2 , FM_TSlot2 , FM_OSlot2 , FM_dbSide2 , FM_Pointer2 , retdata ) ) { // 2007.03.21 // 2007.07.09
					case -1 :
						return 0;
						break;
					}
				}
				else {
					//========================================================================
					// 2007.09.03
					//========================================================================
					if ( SCHEDULER_CONTROL_Chk_FM_ARM_APLUSB_SWAPPING_for_STYLE_4() ) {
						if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
							if ( FM_dbSide == FM_dbSide2 ) {
								if ( !FM_Has_Swap_Wafer_Style_4 ) {
									return 0;
								}
							}
							else {
								if ( FM_Has_Swap_Wafer_Style_4 ) {
									return 0;
								}
							}
						}
						else {
							if ( FM_Has_Swap_Wafer_Style_4 ) {
								return 0;
							}
						}
					}
					//-----------------------------------------------
					FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
					//-----------------------------------------------
					//========================================================================
					switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 250 , FM_CO1 , FM_TSlot , FM_OSlot , FM_dbSide , FM_Pointer , FM_CO2 , FM_TSlot2 , FM_OSlot2 , FM_dbSide2 , FM_Pointer2 , retdata ) ) { // 2007.03.21 // 2007.07.09
					case -1 :
						return 0;
						break;
					}
				}
				//=============================================================================================
				_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_TSlot , FM_CO1 , FM_OSlot , FM_dbSide , FM_Pointer , FM_TSlot2 , FM_CO2 , FM_OSlot2 , FM_dbSide2 , FM_Pointer2 , TRUE ); // 2007.07.11
				//==================================================================================================
				//==================================================================================================
				// 2007.04.27
				//==================================================================================================
				//========================================================================================
				SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
				//========================================================================================
				FMALBMPickReject = TRUE;
				//==================================================================================================
				//========================================================================================================
				if ( _SCH_MODULE_Chk_FM_Free_Status( CHECKING_SIDE ) ) { // 2004.09.10
					if ( ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 1 ) ||
						 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 3 ) ||
						 ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 5 ) ) {
						_SCH_MODULE_Set_FM_End_Status( CHECKING_SIDE );
						_SCH_SYSTEM_MODE_END_SET( CHECKING_SIDE , 0 );
						_SCH_SYSTEM_MODE_LOOP_SET( CHECKING_SIDE , 2 );
						_SCH_SYSTEM_MODE_WAITR_SET( CHECKING_SIDE , FALSE );
						//===============================================================================
						for ( FM_CM = 0 , k = 0 ; k < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; k++ ) {
							if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + k ) > MUF_00_NOTUSE ) { // 2005.07.29
								if ( !SCHEDULING_More_Supply_Check_for_STYLE_4( PM1 + k , 1 ) ) {
									if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , PM1 + k ) == MUF_01_USE ) || ( _SCH_MODULE_Get_Mdl_Run_Flag( PM1 + k ) <= 1 ) ) { 
										SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , PM1 + k , -1 , 2 , 109 );
									}
									else {
										SCHEDULER_CONTROL_Set_SDM_4_CHAMER_LOTEND_AT_LAST( CHECKING_SIDE , PM1 + k , -1 , 1 , 110 );
									}
								}
								else {
									FM_CM = 1;
								}
							}
						}
						if ( FM_CM == 0 ) {
							//================================================================================================
							// 2005.07.27
							//================================================================================================
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 2 );
							//================================================================================================
						}
						//===============================================================================
					}
				}
				//========================================================================================================
				if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2004.09.07
					k = 0;
					if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
						if ( ( FM_Pointer < 100 ) || ( FM_Pointer2 < 100 ) ) {
							k = SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4( CHECKING_SIDE , FALSE );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4 NOT 1 = %d\n" , k );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
						}
					}
					else if ( FM_TSlot > 0 ) {
						if ( FM_Pointer < 100 ) {
							k = SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4( CHECKING_SIDE , FALSE );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4 NOT 2 = %d\n" , k );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
						}
					}
					else if ( FM_TSlot2 > 0 ) {
						if ( FM_Pointer2 < 100 ) {
							k = SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4( CHECKING_SIDE , FALSE );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "SCHEDULER_CONTROL_Chk_OTHER_RUN_WAFER_NOT_EXIT_4 NOT 3 = %d\n" , k );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
//printf( "============================================================================\n" );
						}
					}
					//=====================================================================================================
					if ( k == 0 ) {
//						if ( !SIGNAL_MODE_WAITR_GET( CHECKING_SIDE ) ) { // 2005.07.25 // 2005.09.15
						if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
							if ( _SCH_MODULE_Chk_TM_Free_Status( CHECKING_SIDE ) ) { // 2004.09.07
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 01-2 FINISH_CHECK_1 = TRUE\n" );
//								if ( !SCHEDULER_CONTROL_Get_SDM_4_CHAMER_WAIT_RUN_SIDE( CHECKING_SIDE ) ) { // 2003.06.26 // 2009.08.04
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 02-1 FINISH_CHECK_2 = TRUE\n" );
									if ( _SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Finish 3 for FEM Module%cWHFMSUCCESS 3\n" , 9 );
									}
									return SYS_SUCCESS;
//								} // 2009.08.04
							}
						}
					}
					//=====================================================================================================
				}
				//========================================================================================================
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 09 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
		//----------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.11.05 // 2007.01.06 // 2007.04.04
			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2005.01.24
				if ( SignalCount != 0 ) break;
			}
		}
		else { // 2004.10.01
			if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2005.01.24
				if ( SignalCount != 0 ) {
					//break; // 2006.11.10
					if ( SignalMove == 0 ) break; // 2006.11.10
				}
			}
		}
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		// PICK FROM CM & PLACE TO BM PART
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		//=====================================================================================================
		FM_Pick_Running_Blocking_Style_4 = TRUE; // 2006.12.07
		//=====================================================================================================
		SignalMove = 0; // 2006.11.10
		FM_Buffer = 0;
		FingerCount = 0;
		doubleok = 0;
		doubleok4isdouble = 0; // 2005.09.30
		clscheckskip = FALSE; // 2006.11.18
		//
		//------+------------------------------------------------------------------------------------------
		//   0  |
		//------+------------------------------------------------------------------------------------------
		//   1  | PICK CMx with Armx | PLACE AL with Armx |
		//------+------------------------------------------------------------------------------------------
		//   2  |                                         | PLACE BM with ArmA
		//------+------------------------------------------------------------------------------------------
		//   3  |                                         | PLACE BM with ArmB
		//------+------------------------------------------------------------------------------------------
		//   4  |                                         | PLACE BM with ArmAB
		//------+------------------------------------------------------------------------------------------
		//  11  |                    | PICK  AL with ArmA | PLACE BM with ArmA
		//------+------------------------------------------------------------------------------------------
		//  12  |                    | PICK  AL with ArmB | PLACE BM with ArmB
		//------+------------------------------------------------------------------------------------------
		//  21  | PICK CMx with ArmA | PLACE AL with ArmA |
		//------+------------------------------------------------------------------------------------------
		//  22  | PICK CMx with ArmB | PLACE AL with ArmB |
		//------+------------------------------------------------------------------------------------------
		//  31  |                    | PICK  AL with ArmA
		//------+------------------------------------------------------------------------------------------
		//  32  |                    | PICK  AL with ArmB
		//-------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------------------
		// PICK  CM	:	O
		// PICK  AL	:	O
		// PICK  IC	:	O
		// PICK  BM	:	O
		//-------------------------------------------------------------------------------------------------
		// PLACE CM	:	O
		// PLACE AL	:	O
		// PLACE IC	:	O
		// PLACE BM	:	x
		//-------------------------------------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 10 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
		//----------------------------------------------------------------------
//		{
//			int x;
//
//			if ( CHECKING_SIDE == 0 ) {
//				x = 0;
//			}
//			else if ( CHECKING_SIDE == 1 ) {
//				x = 1;
//			}
//		}
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
// WAFER (O,O)
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
//			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.06.24 // 2007.01.06
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.05 // 2007.01.06
			if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) >  0 ) ) {
//					if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 0 ) ) { // 2004.08.11 // 2005.09.06
				if ( ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) { // 2004.08.11 // 2005.09.06
					if ( ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) ) { // 2004.07.06
						doubleok = 4; // (4) PLACE BM with ArmAB





					}
					else if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) { // 2004.07.06
						doubleok = 2; // 2004.07.06 // (2) PLACE BM with ArmA




					}
					else if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) { // 2004.07.06
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) {
							//=====================================================================================
							// 2007.11.14
							//=====================================================================================
							FingerCount = 1;
							//=====================================================================================
						}
						else {
							doubleok = 3; // 2004.07.06 // (3) PLACE BM with ArmB
						}
					}
					else { // 2004.07.06
						FingerCount = 1; // 2004.07.06
					}
				}
//					else if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 1 ) ) { // 2004.08.11 // 2005.09.06
				else if ( ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_DONE_AL ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) != FMWFR_PICK_CM_DONE_AL_GOBM ) ) { // 2004.08.11 // 2005.09.06
					if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) { // 2004.07.06
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) { // 2005.09.08
							doubleok = 2; // (2) PLACE BM with ArmA




						}
						else { // 2005.09.08
							if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) {
								if ( SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 ) > 0 ) {
									doubleok = 2; // (2) PLACE BM with ArmA




								}
								else {
									FingerCount = 1; // 2004.07.06
								}
							}
							else {
								doubleok = 2; // (2) PLACE BM with ArmA




							}
						}
					}
					else { // 2004.07.06
						FingerCount = 1; // 2004.07.06
					}
				}
//					else if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == 1 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == 0 ) ) { // 2004.08.11 // 2005.09.06
				else if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_DONE_AL ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) != FMWFR_PICK_CM_DONE_AL_GOBM ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) { // 2004.08.11 // 2005.09.06
					if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) { // 2004.07.06
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) { // 2005.09.08
							doubleok = 3; // (3) PLACE BM with ArmB




						}
						else { // 2005.09.08
							if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) {
								if ( SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , 0 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 ) > 0 ) {
									doubleok = 3; // (3) PLACE BM with ArmB




								}
								else {
									FingerCount = 1; // 2004.07.06
								}
							}
							else {
								doubleok = 3; // (3) PLACE BM with ArmB




							}
						}
					}
					else { // 2004.07.06
						FingerCount = 1; // 2004.07.06
					}
				}
				else { // 2004.08.11
					FingerCount = 1; // 2004.08.11
				}
			}
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
// WAFER (X,O)
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
			else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) >  0 ) ) {
				//===========================================================================================================
				switch ( Scheduling_FEM_Pick_FAL_First( CHECKING_SIDE , 1 ) ) { // 2006.11.28
				case -1 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
					return 0;
					break;
				case 1 :
					doubleok = 11; // (11) PICK AL with ArmA | PLACE BM with ArmA
					FMICPickReject = TRUE;
					break;
				}
				//===========================================================================================================
				if ( doubleok == 0 ) {
					if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) ) { // 2004.08.11 // 2005.09.06
						if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2005.09.06
							if ( _SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
								if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) {  // 2006.11.27
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) {
										if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
											if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2006.11.15
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
									}
									else { // 2006.11.23
										if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
											if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2006.11.15
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
									}
								}
								FingerCount = 1;
							}
							else {
								//=============================================================================================
								// 2006.11.18
								//=============================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
//										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.28
									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.28
										if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											FMICPickReject = TRUE;
										}
										else {
											if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
												FMICPickReject = TRUE;
											}
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
								else {
									FingerCount = 1;
								}
							}
						}
						else {
//							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.08.17 // 2006.09.27
							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) ) { // 2004.08.17 // 2006.09.27
								if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2005.09.06
									FingerCount = 1;
								}
								else {
									if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
										ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
										if ( ALsts == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
											return 0;
										}
										if ( ALsts == SYS_SUCCESS ) {
											if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2005.09.08
												if ( SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , _SCH_MODULE_Get_MFAL_WAFER() , 0 , _SCH_MODULE_Get_MFAL_POINTER() , 0 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 ) > 0 ) {
													doubleok = 11; // (11) PICK AL with ArmA | PLACE BM with ArmA




												}
												else {
													doubleok = 31; // (31) PICK AL with ArmA
												}
											}
											else {
												doubleok = 11; // (11) PICK AL with ArmA | PLACE BM with ArmA




											}
										}
										else {
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
							}
							else {
								//=============================================================================================
								// 2006.11.18
								//=============================================================================================
								//FingerCount = 1; // 2004.08.11
								//=============================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
//										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.28
									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.28
										if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											FMICPickReject = TRUE;
										}
										else {
											if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
												FMICPickReject = TRUE;
											}
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
								else {
									FingerCount = 1;
								}
								//=============================================================================================
							}
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( CHECKING_SIDE ) ) { // 2004.08.20
//							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.08.20 // 2006.09.27
							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) ) { // 2004.08.17 // 2006.09.27
								//----------------------------------------------
								// 2004.09.02
								//----------------------------------------------
								if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
									ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
									if ( ALsts == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
										return 0;
									}
									if ( ALsts == SYS_SUCCESS ) {
										doubleok = 31; // 2004.08.20 // (31) PICK AL with ArmA
									}
									else {
										FingerCount = 1; // 2004.09.03
									}
								}
								else {
									FingerCount = 1; // 2004.09.03
								}
							}
							else {
								if ( _SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
									if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) { // 2006.11.27
										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
											if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
										else {
											if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
									}
									FingerCount = 1;
								}
								else {
									if (
										SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ||
										( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) // 2006.11.16
										) {
										FingerCount = 1;
									}
									else {
										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_AL) == 2 ) ) { // 2006.09.27
											FingerCount = 1;
										}
										else {
											if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 2 , 1 , &rcm ) ) { // 2006.11.24
												FingerCount = 1;
											}
											else {
												//==============================================================================
												// 2006.11.18
												//==============================================================================
												//doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												//FMICPickReject = TRUE; // 2005.09.07
												//==============================================================================
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
													if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) != CHECKING_SIDE ) {
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
														FMICPickReject = TRUE; // 2005.09.07
													}
													else {
														if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) { // 2006.11.18
															if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
																doubleok = 3; // (3) PLACE BM with ArmB
																FMICPickReject = TRUE; // 2005.09.07
															}
															else {
																if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
																	doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
																	FMICPickReject = TRUE; // 2005.09.07
																}
																else {
																	doubleok = 3; // (3) PLACE BM with ArmB
																	FMICPickReject = TRUE; // 2005.09.07
																}
															}
														}
														else {
															doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															FMICPickReject = TRUE; // 2005.09.07
														}
													}
												}
												else { // 2006.11.23
													doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
													FMICPickReject = TRUE; // 2005.09.07
												}
												//==============================================================================
											}
										}
									}
								}
							}
						}
						else {
//							if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) { // 2004.07.06 // 2005.09.06
							if ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) { // 2004.07.06 // 2005.09.06
//								if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2004.09.02 // 2004.09.30
//									doubleok = 3; // 2005.09.06
//								} // 2004.09.30
//								else { // 2004.09.30
//									FingerCount = 1; // 2004.09.03 // 2004.09.30
//								} // 2004.09.30
								//===================================================================================================================
								// 2005.09.06
								//===================================================================================================================
								if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) { // 2005.09.08
//									if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.09.27
									if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) ) { // 2004.08.17 // 2006.09.27
										if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
											ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
											if ( ALsts == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
												return 0;
											}
											if ( ALsts == SYS_SUCCESS ) {
												doubleok = 31; // (31) PICK AL with ArmA
											}
											else {
												FingerCount = 1;
											}
										}
										else {
											FingerCount = 1;
										}
									}
									else {
										if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) {
											doubleok = 3; // (3) PLACE BM with ArmB
										}
										else {
											FingerCount = 1;
										}
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_ONE_NOT_VACUUM( CHECKING_SIDE ) ) { // 2005.09.06
										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.11.28
//											if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.09.27
											if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) ) { // 2004.08.17 // 2006.09.27
												if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
													ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
													if ( ALsts == SYS_ABORTED ) {
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
														return 0;
													}
													if ( ALsts == SYS_SUCCESS ) {
														doubleok = 31; // (31) PICK AL with ArmA
													}
													else {
														FingerCount = 1;
													}
												}
												else {
													FingerCount = 1;
												}
											}
											else {
												if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) { // 2006.11.16
													doubleok = 3; // (3) PLACE BM with ArmB
												}
												else {
													if ( !_SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
														if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2005.11.30
															FingerCount = 1;
														}
														else {
															if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_AL) == 2 ) ) { // 2006.09.27
																FingerCount = 1;
															}
															else {
																if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 2 , 1 , &rcm ) ) { // 2006.11.24
																	FingerCount = 1;
																}
																else {
																	doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
																}
															}
														}
													}
													else {
														FingerCount = 1;
													}
												}
											}
										}
										else {
											if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) {
												doubleok = 3; // (3) PLACE BM with ArmB
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
													if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.11.18
														FMICPickReject = TRUE; // 2005.11.30
													}
												}
												else {
													FMICPickReject = TRUE; // 2005.11.30
												}
											}
											else {
												//=========================================================================================================================
												// 2006.11.18
												//=========================================================================================================================
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
//														if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.16 // 2006.11.28
													if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.16 // 2006.11.28
														if ( SCHEDULING_CHECK_FM_CurrAnother_Supply_Pick_Possible( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , CHECKING_SIDE ) ) { // 2006.11.17
															doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															clscheckskip = TRUE;
														}
														else {
															FingerCount = 1;
														}
													}
													else {
													//=========================================================================================================================
														FingerCount = 1;
													}
												}
												else {
													FingerCount = 1;
												}
											}
										}
									}
									else {
//										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.09.27
										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) ) { // 2004.08.17 // 2006.09.27
											if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
												ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
												if ( ALsts == SYS_ABORTED ) {
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
													return 0;
												}
												if ( ALsts == SYS_SUCCESS ) {
													doubleok = 31; // (31) PICK AL with ArmA
												}
												else {
													FingerCount = 1;
												}
											}
											else {
												FingerCount = 1;
											}
										}
										else {
											if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) == CHECKING_SIDE ) {
												doubleok = 3; // (3) PLACE BM with ArmB
											}
											else {
												if ( !_SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
													if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2005.11.30
														FingerCount = 1;
													}
													else {
														if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_AL) == 2 ) ) { // 2006.09.27
															FingerCount = 1;
														}
														else {
															if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 2 , 1 , &rcm ) ) { // 2006.11.24
																FingerCount = 1;
															}
															else {
																doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															}
														}
													}
												}
												else {
													FingerCount = 1;
												}
											}
										}
									}
								}
								//===================================================================================================================
							}
							else { // 2004.07.06
								FingerCount = 1; // 2004.07.06
							}
						}
					}
				}
			}
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
// WAFER (O,X)
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
			else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) >  0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
				//===========================================================================================================
				switch ( Scheduling_FEM_Pick_FAL_First( CHECKING_SIDE , 2 ) ) { // 2006.11.28
				case -1 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
					return 0;
					break;
				case 1 :
					doubleok = 12; // (12) PICK AL with ArmB | PLACE BM with ArmB
					FMICPickReject = TRUE;
					break;
				}
				//===========================================================================================================
				if ( doubleok == 0 ) {
					if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) ) { // 2004.08.11 // 2005.09.06
						if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2005.09.06
							if ( _SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
								if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) { // 2006.11.27
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
										if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
											if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2006.11.15
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
									}
									else {
										if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
											if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2006.11.15
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
									}
								}
								FingerCount = 1;
							}
							else {
								//=============================================================================================
								// 2006.11.18
								//=============================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.28
										if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
											doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
											FMICPickReject = TRUE;
										}
										else {
											if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
												FMICPickReject = TRUE;
											}
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
								else {
									FingerCount = 1;
								}
								//=============================================================================================
							}
						}
						else {
//							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.08.17 // 2006.09.27
							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) ) { // 2004.08.17 // 2006.09.27
								if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2005.09.06
									FingerCount = 1;
								}
								else {
									if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
										ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
										if ( ALsts == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
											return 0;
										}
										if ( ALsts == SYS_SUCCESS ) {
											if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) ) { // 2005.09.08
												if ( SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , _SCH_MODULE_Get_MFAL_WAFER() , 0 , _SCH_MODULE_Get_MFAL_POINTER() , 0 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 ) > 0 ) {
													doubleok = 12; // (12) PICK AL with ArmB | PLACE BM with ArmB




												}
												else {
													doubleok = 32; // (32) PICK AL with ArmB
												}
											}
											else {
												doubleok = 12; // (12) PICK AL with ArmB | PLACE BM with ArmB




											}
										}
										else {
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
							}
							else {
								//=============================================================================================
								// 2006.11.16
								//=============================================================================================
								//FingerCount = 1; // 2004.08.11
								//=============================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
//										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.28
									if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.28
										if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
											doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
											FMICPickReject = TRUE;
										}
										else {
											if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
												FMICPickReject = TRUE;
											}
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
								else {
									FingerCount = 1;
								}
								//=============================================================================================
							}
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_BM_ALL_VACUUM( CHECKING_SIDE ) ) { // 2004.08.20
//							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.08.20 // 2006.09.27
							if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) ) { // 2004.08.17 // 2006.09.27
								//----------------------------------------------
								// 2004.09.02
								//----------------------------------------------
								if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
									ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
									if ( ALsts == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
										return 0;
									}
									if ( ALsts == SYS_SUCCESS ) {
										doubleok = 32; // 2004.08.20 // (32) PICK AL with ArmB
									}
									else {
										FingerCount = 1; // 2004.09.03
									}
								}
								else {
									FingerCount = 1; // 2004.09.03
								}
							}
							else {
								if ( _SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
									if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) { // 2006.11.27
										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
											if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
										else {
											if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
												FMICPickReject = TRUE; // 2005.11.30
											}
										}
									}
									FingerCount = 1;
								}
								else {
									if (
										SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ||
										( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) // 2006.11.16
										) { // 2005.11.30
										FingerCount = 1;
									}
									else {
										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_AL) == 1 ) ) { // 2006.09.27
											FingerCount = 1;
										}
										else {
											if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 1 , 2 , &rcm ) ) { // 2006.11.24
												FingerCount = 1;
											}
											else {
												//==============================================================================
												// 2006.11.18
												//==============================================================================
												//doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
												//FMICPickReject = TRUE; // 2005.09.07
												//==============================================================================
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
													if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != CHECKING_SIDE ) {
														doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
														FMICPickReject = TRUE; // 2005.09.07
													}
													else {
														if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) { // 2006.11.18
															if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
																doubleok = 2; // (2) PLACE BM with ArmA
																FMICPickReject = TRUE; // 2005.09.07
															}
															else {
																if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
																	doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
																	FMICPickReject = TRUE; // 2005.09.07
																}
																else {
																	doubleok = 2; // (2) PLACE BM with ArmA
																	FMICPickReject = TRUE; // 2005.09.07
																}
															}
														}
														else {
															doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															FMICPickReject = TRUE; // 2005.09.07
														}
													}
												}
												else {
													doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
													FMICPickReject = TRUE; // 2005.09.07
												}
												//==============================================================================
											}
										}
									}
								}
							}
						}
						else {
//							if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) { // 2004.07.06 // 2005.09.06
							if ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) { // 2004.07.06 // 2005.09.06
//								if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2004.09.02 // 2004.09.30
//									doubleok = 2; // 2005.09.06
//								} // 2004.09.30
//								else { // 2004.09.30
//									FingerCount = 1; // 2004.09.03 // 2004.09.30
//								} // 2004.09.30
								//===================================================================================================================
								// 2005.09.06
								//===================================================================================================================
								if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) == 1 ) { // 2005.09.08
//									if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.09.27
									if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) ) { // 2004.08.17 // 2006.09.27
										if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
											ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
											if ( ALsts == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
												return 0;
											}
											if ( ALsts == SYS_SUCCESS ) {
												doubleok = 32; // (32) PICK AL with ArmB
											}
											else {
												FingerCount = 1;
											}
										}
										else {
											FingerCount = 1;
										}
									}
									else {
										if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
											doubleok = 2; // (2) PLACE BM with ArmA
										}
										else {
											FingerCount = 1;
										}
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_ONE_NOT_VACUUM( CHECKING_SIDE ) ) { // 2005.09.06
										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.11.28
//											if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.09.27
											if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) ) { // 2004.08.17 // 2006.09.27
												if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
													ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
													if ( ALsts == SYS_ABORTED ) {
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
														return 0;
													}
													if ( ALsts == SYS_SUCCESS ) {
														doubleok = 32; // (32) PICK AL with ArmB
													}
													else {
														FingerCount = 1;
													}
												}
												else {
													FingerCount = 1;
												}
											}
											else {
												if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
													doubleok = 2; // (2) PLACE BM with ArmA




												}
												else {
													if ( !_SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
														if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2005.11.30
															FingerCount = 1;
														}
														else {
															if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_AL) == 1 ) ) { // 2006.09.27
																FingerCount = 1;
															}
															else {
																if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 1 , 2 , &rcm ) ) { // 2006.11.24
																	FingerCount = 1;
																}
																else {
																	doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
																}
															}
														}
													}
													else {
														FingerCount = 1;
													}
												}
											}
										}
										else {
											if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
												doubleok = 2; // (2) PLACE BM with ArmA
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
													if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.11.18
														FMICPickReject = TRUE; // 2005.11.30
													}
												}
												else {
													FMICPickReject = TRUE; // 2005.11.30
												}
											}
											else {
												//=========================================================================================================================
												// 2006.11.18
												//=========================================================================================================================
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
													if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) { // 2006.11.16
														if ( SCHEDULING_CHECK_FM_CurrAnother_Supply_Pick_Possible( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , CHECKING_SIDE ) ) { // 2006.11.17
															doubleok = 22; // (22) PICK CMx with ArmA | PLACE AL with ArmB
															clscheckskip = TRUE;
														}
														else {
															FingerCount = 1;
														}
													}
													else {
													//=========================================================================================================================
														FingerCount = 1;
													}
												}
												else {
													FingerCount = 1;
												}
											}
										}
									}
									else {
//										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2006.09.27
										if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 1 ) ) { // 2004.08.17 // 2006.09.27
											if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
												ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
												if ( ALsts == SYS_ABORTED ) {
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
													return 0;
												}
												if ( ALsts == SYS_SUCCESS ) {
													doubleok = 32; // (32) PICK AL with ArmB
												}
												else {
													FingerCount = 1;
												}
											}
											else {
												FingerCount = 1;
											}
										}
										else {
											if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == CHECKING_SIDE ) {
												doubleok = 2; // (2) PLACE BM with ArmA
											}
											else {
												if ( !_SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
													if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2005.11.30
														FingerCount = 1;
													}
													else {
														if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_AL) == 1 ) ) { // 2006.09.27
															FingerCount = 1;
														}
														else {
															if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 1 , 2 , &rcm ) ) { // 2006.11.24
																FingerCount = 1;
															}
															else {
																doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															}
														}
													}
												}
												else {
													FingerCount = 1;
												}
											}
										}
									}
								}
								//===================================================================================================================
							}
							else { // 2004.07.06
								FingerCount = 1; // 2004.07.06
							}
						}
					}
				}
			}
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
// WAFER (X,X)
//===============================================================================================================================================================
//===============================================================================================================================================================
//===============================================================================================================================================================
			else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
				//===========================================================================================================
				switch ( Scheduling_FEM_Pick_FAL_First( CHECKING_SIDE , 0 ) ) { // 2006.11.28
				case -1 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
					return 0;
					break;
				case 1 :
					doubleok = 11; // (11) PICK AL with ArmA | PLACE BM with ArmA
					FMICPickReject = TRUE;
					break;
				case 2 :
					doubleok = 12; // (12) PICK AL with ArmB | PLACE BM with ArmB
					FMICPickReject = TRUE;
					break;
				}
				if ( doubleok == 0 ) {
					if ( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) ) { // 2004.08.17
						if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) ) { // 2005.09.06
							if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
								if ( _SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
									//------------------------------------------
									ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
									if ( ALsts == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
										return 0;
									}
									//------------------------------------------
									if ( ALsts == SYS_SUCCESS ) {
//										doubleok = 31; // (31) PICK AL with ArmA // 2006.09.27
										switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
										case 0 : doubleok = 31; break; // (31) PICK AL with ArmA
										case 1 : doubleok = 31; break; // (31) PICK AL with ArmA
										case 2 : doubleok = 32; break; // (32) PICK AL with ArmB
										}
									}
									else {
										if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) { // 2006.11.27
											// FMICPickReject = TRUE; // 2005.09.07 // 2005.11.30
											if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
//											if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
												if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
													FMICPickReject = TRUE; // 2005.11.30
												}
//											}
											}
											else {
												if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
													FMICPickReject = TRUE; // 2005.11.30
												}
											}
										}
										FingerCount = 1;
									}
								}
								else {
									if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2005.11.30
										FingerCount = 1;
									}
									else {
//										doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB // 2006.09.27
										if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 0 , 0 , &rcm ) ) { // 2006.11.24
											FingerCount = 1;
										}
										else {
//											FMPickForce = TRUE; // 2005.09.07 // 2006.09.27
											//==============================================================
											// 2006.09.27
											//==============================================================
											switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
											case 0 :
												switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
												case 0 :
												case 3 :
													if      ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
														doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
														FMPickForce = TRUE;
													}
													else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
														FMPickForce = TRUE;
													}
													else {
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
														FMPickForce = TRUE;
													}
													break;
												case 1 :
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
														//------------------------------------------
														// 2006.10.19
														//------------------------------------------
														ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
														if ( ALsts == SYS_ABORTED ) {
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
															return 0;
														}
														//------------------------------------------
														if ( ALsts == SYS_SUCCESS ) {
															doubleok = 31;	// (31) PICK AL with ArmA
														}
														else {
															FingerCount = 1;
														}
														//------------------------------------------
													}
													else {
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
														FMPickForce = TRUE;
													}
													break;
												case 2 :
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
														//------------------------------------------
														// 2006.10.19
														//------------------------------------------
														ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
														if ( ALsts == SYS_ABORTED ) {
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
															return 0;
														}
														//------------------------------------------
														if ( ALsts == SYS_SUCCESS ) {
															doubleok = 32;	// (32) PICK AL with ArmB
														}
														else {
															FingerCount = 1;
														}
														//------------------------------------------
													}
													else {
														doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
														FMPickForce = TRUE;
													}
													break;
												}
												break;
											case 1 :
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 2 ) {
													FingerCount = 1;
												}
												else {
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
														//------------------------------------------
														// 2006.10.19
														//------------------------------------------
														ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
														if ( ALsts == SYS_ABORTED ) {
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
															return 0;
														}
														//------------------------------------------
														if ( ALsts == SYS_SUCCESS ) {
															doubleok = 31;	// (31) PICK AL with ArmA
														}
														else {
															FingerCount = 1;
														}
														//------------------------------------------
													}
													else {
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
														FMPickForce = TRUE;
													}
												}
												break;
											case 2 :
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 1 ) {
													FingerCount = 1;
												}
												else {
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
														//------------------------------------------
														// 2006.10.19
														//------------------------------------------
														ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
														if ( ALsts == SYS_ABORTED ) {
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
															return 0;
														}
														//------------------------------------------
														if ( ALsts == SYS_SUCCESS ) {
															doubleok = 32;	// (32) PICK AL with ArmB
														}
														else {
															FingerCount = 1;
														}
														//------------------------------------------
													}
													else {
														doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
														FMPickForce = TRUE;
													}
												}
												break;
											}
											//==============================================================
										}
									}
								}
							}
							else {
								if ( !_SCH_SUB_FM_Current_No_More_Supply( _SCH_MODULE_Get_MFAL_SIDE() , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
									// FMICPickReject = TRUE; // 2005.11.30
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
										if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
											FMICPickReject = TRUE; // 2005.11.30
										}
									}
									else {
										if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
											FMICPickReject = TRUE; // 2005.11.30
										}
									}
									FingerCount = 1;
								}
								else {
									if ( _SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
										//------------------------------------------
										ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( _SCH_MODULE_Get_MFAL_SIDE() , FALSE );
										if ( ALsts == SYS_ABORTED ) {
											_SCH_LOG_LOT_PRINTF( _SCH_MODULE_Get_MFAL_SIDE() , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
											return 0;
										}
										//------------------------------------------
										if ( ALsts == SYS_SUCCESS ) {
											if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
												if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
													FMICPickReject = TRUE; // 2005.11.30
												}
											}
											else {
												if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
													FMICPickReject = TRUE; // 2005.11.30
												}
											}
											FingerCount = 1;
										}
										else {
											if ( !_SCH_SUB_FM_Another_No_More_Supply( CHECKING_SIDE , -1 , -1 , -1 ) ) { // 2006.11.27
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
													if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) { // 2006.10.19
														FMICPickReject = TRUE; // 2005.11.30
													}
												}
												else {
													if ( SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) { // 2006.11.15
														FMICPickReject = TRUE; // 2005.11.30
													}
												}
											}
											FingerCount = 1;
										}
									}
									else {
										if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2005.11.30
											FingerCount = 1;
										}
										else {
//											doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB // 2006.09.27
											if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 0 , 0 , &rcm ) ) { // 2006.11.24
												FingerCount = 1;
											}
											else {
//												FMPickForce = TRUE; // 2005.09.07 // 2006.09.27
												//==============================================================
												// 2006.09.27
												//==============================================================
												switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
												case 0 :
													switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
													case 0 :
													case 3 :
														if      ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
															doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															FMPickForce = TRUE;
														}
														else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
															doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															FMPickForce = TRUE;
														}
														else {
															doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															FMPickForce = TRUE;
														}
														break;
													case 1 :
														if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
															FingerCount = 1;
														}
														else {
															doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															FMPickForce = TRUE;
														}
														break;
													case 2 :
														if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
															FingerCount = 1;
														}
														else {
															doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															FMPickForce = TRUE;
														}
														break;
													}
													break;
												case 1 :
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 2 ) {
														FingerCount = 1;
													}
													else {
														if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
															FingerCount = 1;
														}
														else {
															doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															FMPickForce = TRUE;
														}
													}
													break;
												case 2 :
													if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 1 ) {
														FingerCount = 1;
													}
													else {
														if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
															FingerCount = 1;
														}
														else {
															doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															FMPickForce = TRUE;
														}
													}
													break;
												}
												//==============================================================
											}
										}
									}
								}
							}
						}
						else {
							if ( _SCH_MODULE_Get_MFAL_SIDE() == CHECKING_SIDE ) {
								ALsts = _SCH_MACRO_CHECK_FM_MALIGNING( CHECKING_SIDE , FALSE );
								if ( ALsts == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
									return 0;
								}
								if ( ALsts == SYS_SUCCESS ) {
//									doubleok = 11; // (11) PICK AL with ArmA | PLACE BM with ArmA // 2006.09.27
									switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
									case 0 : doubleok = 11; break; // (11) PICK AL with ArmA | PLACE BM with ArmA
									case 1 : doubleok = 11; break; // (11) PICK AL with ArmA | PLACE BM with ArmA
									case 2 : doubleok = 12; break; // (12) PICK AL with ArmB | PLACE BM with ArmB
									}




								}
								else {

									FingerCount = 1; // 2006.11.16

									/*
									//==============================================================
									// 2006.11.16
									//==============================================================
									if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
										switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
										case 0 :
											switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
											case 0 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											case 1 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											case 2 :
												doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
												break;
											case 3 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											}
											break;
										case 1 :
											if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 2 ) {
												FingerCount = 1;
											}
											else {
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											}
											break;
										case 2 :
											if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 1 ) {
												FingerCount = 1;
											}
											else {
												doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
											}
											break;
										}
									}
									else {
										switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
										case 0 :
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											break;
										case 1 :
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											break;
										case 2 :
											doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
											break;
										case 3 :
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											break;
										}
									}
									//==============================================================
*/


								}
							}
							else {
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.11.30
									if ( !_SCH_SUB_FM_Current_No_More_Supply( CHECKING_SIDE , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
										if ( _SCH_SUB_FM_Current_No_More_Supply( _SCH_MODULE_Get_MFAL_SIDE() , &dum_pt , -1 , &dum_rcm , -1 , -1 ) ) {
											if ( SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) {
												FingerCount = 1;
											}
											else {
//												doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB // 2006.09.27
												if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 0 , 0 , &rcm ) ) { // 2006.11.24
													FingerCount = 1;
												}
												else {
													//==============================================================
													// 2006.09.27
													//==============================================================
													switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
													case 0 :
														switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
														case 0 :
														case 3 :
															if      ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
																doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															}
															else if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
																doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															}
															else {
																doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															}
															break;
														case 1 :
															if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
																FingerCount = 1;
															}
															else {
																doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															}
															break;
														case 2 :
															if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
																FingerCount = 1;
															}
															else {
																doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															}
															break;
														}
														break;
													case 1 :
														if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 2 ) {
															FingerCount = 1;
														}
														else {
															if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 1 ) { // 2006.10.19
																FingerCount = 1;
															}
															else {
																doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
															}
														}
														break;
													case 2 :
														if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 1 ) {
															FingerCount = 1;
														}
														else {
															if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_AL) == 2 ) { // 2006.10.19
																FingerCount = 1;
															}
															else {
																doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
															}
														}
														break;
													}
													//==============================================================
												}
											}
										}
										else {
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
								else {
									FingerCount = 1;
								}
							}
						}
					}
					else {
						if ( _SCH_MODULE_Need_Do_Multi_FIC() && ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) ) { // 2005.09.06
							// FingerCount = 1; // 2005.11.30
							//===================================================================
							// 2005.11.30
							//===================================================================
//							if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2006.11.16 // 2006.11.23
							if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) || !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ) { // 2006.11.16 // 2006.11.23
								if (
//									SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) || // 2006.11.16 // 2006.11.23
									( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && SCHEDULER_CONTROL_Chk_BM_NEED_IN_4( CHECKING_SIDE ) ) || // 2006.11.16 // 2006.11.23
//										!SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) || // 2006.11.16 // 2006.11.23
									( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) && !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) || // 2006.11.16 // 2006.11.23

									( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) // 2006.11.16

									) {

//									doubleok = 1; // (1) PICK CMx with Armx | PLACE AL with Armx // 2006.09.27
									if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 0 , 0 , &rcm ) ) { // 2006.11.24
										FingerCount = 1;
									}
									else {
										//==============================================================
										// 2006.09.27
										//==============================================================
										if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
											switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
											case 0 :
												switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
												case 0 :
													doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
													break;
												case 1 :
													doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
													break;
												case 2 :
													doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
													break;
												case 3 :
													doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
													break;
												}
												break;
											case 1 :
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 2 ) {
													FingerCount = 1;
												}
												else {
													doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												}
												break;
											case 2 :
												if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 1 ) {
													FingerCount = 1;
												}
												else {
													doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
												}
												break;
											}
										}
										else {
											switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
											case 0 :
												doubleok =  1; // (1)  PICK CMx with Armx | PLACE AL with Armx
												break;
											case 1 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											case 2 :
												doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
												break;
											case 3 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											}
										}
										//==============================================================
									}
								}
								else {
									FingerCount = 1;
								}
								//==================================================================
								// 2006.11.16
								//==================================================================
								if ( FingerCount == 1 ) {
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2006.11.23
										if ( !SCHEDULING_CHECK_FM_Another_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
											FMICPickReject = TRUE;
										}
									}
								}
								//==================================================================


							} // 2006.11.16 // 2006.11.23
							else { // 2006.11.16 // 2006.11.23
								FingerCount = 1; // 2006.11.16 // 2006.11.23
							} // 2006.11.16 // 2006.11.23
							//===================================================================
						}
						else {
/*
// 2006.11.29
							if (
								!SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ||

								( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) // 2006.11.16 // 2006.11.17 // 2006.11.22
//									( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) // 2006.11.16 // 2006.11.17 // 2006.11.22

								) { // 2004.09.02
//								doubleok = 1; // (1) PICK CMx with Armx | PLACE AL with Armx // 2006.09.27
*/
							//======================================================================================
							// 2006.11.29
							//======================================================================================
							if (
								!SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) ||
								( _SCH_MODULE_Need_Do_Multi_FAL() && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) )
								) {
								// check
							}
							else { // 2006.11.29
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) {
									if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) {
										if ( !SCHEDULING_CHECK_FM_Side_Supply_Pick_Impossible( CHECKING_SIDE , -1 ) ) {
											// check
										}
										else {
											FingerCount = 1;
										}
									}
									else {
										FingerCount = 1;
									}
								}
								else {
									FingerCount = 1;
								}
							}
							//============================================================================
							if ( FingerCount != 1 ) { // 2006.11.29
								if ( !SCHEDULING_CHECK_FM_PICK_from_CM_Possible_with_IntCheck( CHECKING_SIDE , 0 , 0 , &rcm ) ) { // 2006.11.24
									FingerCount = 1;
								}
								else {
									//==============================================================
									// 2006.09.27
									//==============================================================
									if ( _SCH_MODULE_Need_Do_Multi_FAL() ) {
										switch( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) { // 2006.09.27
										case 0 :
											switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
											case 0 :
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 0 ) { // 2007.01.16
													if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.16
														doubleok =  1; // (1)  PICK CMx with Armx | PLACE AL with Armx
													}
													else {
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
													}
												}
												else {
													if ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) { // 2006.11.16
														doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
													}
													else {
														doubleok =  1; // (1)  PICK CMx with Armx | PLACE AL with Armx
													}
												}
												break;
											case 1 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											case 2 :
												doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
												break;
											case 3 :
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
												break;
											}
											break;
										case 1 :
											if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 2 ) {
												FingerCount = 1;
											}
											else {
												doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											}
											break;
										case 2 :
											if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(rcm) == 1 ) {
												FingerCount = 1;
											}
											else {
												doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
											}
											break;
										}
									}
									else {
										switch( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( rcm ) ) { // 2006.09.27
										case 0 :
											doubleok =  1; // (1)  PICK CMx with Armx | PLACE AL with Armx
											break;
										case 1 :
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											break;
										case 2 :
											doubleok = 22; // (22) PICK CMx with ArmB | PLACE AL with ArmB
											break;
										case 3 :
											doubleok = 21; // (21) PICK CMx with ArmA | PLACE AL with ArmA
											break;
										}
									}
									//==============================================================
								}
							}
							// else { // 2006.11.29
								// FingerCount = 1;// 2006.11.29
							// } // 2006.11.29
						}
					}
				}
			}
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
			if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) FingerCount++;
			//==============================================================================================
			// 2006.12.18
			//==============================================================================================
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				if ( _SCH_MODULE_Get_MFIC_Completed_Wafer( CHECKING_SIDE , &FM_Side2 , &ICsts , &ICsts2 ) == SYS_SUCCESS ) {
					if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
						FMICPickReject = FALSE;
						FingerCount++;
					}
				}
			}
			//==============================================================================================
			//
			if ( FingerCount == 0 ) { // 2014.12.12
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
					if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_NEED_NO_SUPPLY_4( CHECKING_SIDE ) ) {
							FingerCount++;
						}
					}
				}
			}
			//
			//==============================================================================================
		}
		else if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) FingerCount++;
			//==============================================================================================
			// 2006.12.18
			//==============================================================================================
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				if ( _SCH_MODULE_Get_MFIC_Completed_Wafer( CHECKING_SIDE , &FM_Side2 , &ICsts , &ICsts2 ) == SYS_SUCCESS ) {
					if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
						FMICPickReject = FALSE;
						FingerCount++;
					}
				}
			}
			//==============================================================================================
			//
			if ( FingerCount == 0 ) { // 2014.12.12
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
					if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
						if ( SCHEDULER_CONTROL_Chk_BM_NEED_NO_SUPPLY_4( CHECKING_SIDE ) ) {
							FingerCount++;
						}
					}
				}
			}
			//
			//==============================================================================================
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 11 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
		//----------------------------------------------------------------------
//			if ( ( FingerCount == 0 ) && ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 1 ) ) { // 2005.01.24
		if ( ( FingerCount == 0 ) && ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) ) { // 2005.01.24
			//========================================================================================================================
			// 2006.03.10
			//========================================================================================================================
//				if ( FingerCount == 1 ) { // 2006.05.02
				if ( ( doubleok == 2 ) || ( doubleok == 4 ) ) {
					if ( SCHEDULING_CHECK_Curr_Data_Target_Full_All_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , FingerCount == 1 , -1 ) ) {

						//_IO_CIM_PRINTF( "SCHEDULING_CHECK_Curr_Data_Target_Full_All_for_STYLE_4 A Return %d\n" , CHECKING_SIDE );

						_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , -1 );
						//
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ); // 2006.03.23
					}
				}
				if ( ( doubleok == 3 ) || ( doubleok == 4 ) ) {
					if ( SCHEDULING_CHECK_Curr_Data_Target_Full_All_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , FingerCount == 1 , -1 ) ) {

						//_IO_CIM_PRINTF( "SCHEDULING_CHECK_Curr_Data_Target_Full_All_for_STYLE_4 B Return %d\n" , CHECKING_SIDE );

						_SCH_CLUSTER_Check_and_Make_Return_Wafer( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , -1 );
						//
						_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ); // 2006.03.23
					}
				}
//				} // 2006.05.02
			//========================================================================================================================
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 12 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
		//----------------------------------------------------------------------
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 13 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
		//----------------------------------------------------------------------
		if ( FingerCount == 0 ) {
			Result = -1;
			// (11) PICK AL with ArmA | PLACE BM with ArmA
			// (12) PICK AL with ArmB | PLACE BM with ArmB
			// (31) PICK AL with ArmA
			// (32) PICK AL with ArmB
			if ( ( doubleok == 11 ) || ( doubleok == 12 ) || ( doubleok == 31 ) || ( doubleok == 32 ) ) { // 2004.08.17
				Result = -1;
			}
			else if ( ( doubleok != 1 ) && ( doubleok != 21 ) && ( doubleok != 22 ) ) {
				//===================================================================================================
				// 2004.12.22
				//===================================================================================================
				if ( doubleok == 2 ) { // (2) PLACE BM with ArmA
//						if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2005.10.06 // 2006.01.14
					if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) { // 2005.10.06 // 2006.01.14
						//
						FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
						FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
						FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
						FM_Slot2 = 0;
						FM_Pointer2 = 0;
						Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
						if ( Result > 0 ) {
							if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) { // 2006.09.13
								if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 );
									if ( Result > 0 ) {
										if ( ( FM_Buffer + 1 ) == FM_Bufferx ) {
											doubleok = 4; // (4) PLACE BM with ArmAB
											//
											FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
											FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
											FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
											FM_TSlot2 = FM_TSlot2x;
											//
											doubleok4isdouble = 3;
										}
									}
									else {
										Result = 1;
									}
								}
								else {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , FM_Buffer - 1 , FM_Slot , 0 , FM_Pointer , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 );
									if ( Result > 0 ) {
										if ( ( FM_Buffer - 1 ) == FM_Bufferx ) {
											Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 );
											if ( Result > 0 ) {
												if ( FM_Buffer == FM_Bufferx ) {
													FM_Buffer = FM_Buffer - 1;
													//
													doubleok = 4; // (4) PLACE BM with ArmAB
													//
													FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
													FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
													FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
													FM_TSlot = FM_TSlotx;
													//
													FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
													FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
													FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
													FM_TSlot2 = FM_TSlot2x;
													//
													doubleok4isdouble = 3;
												}
											}
											else {
												Result = 1;
											}
										}
									}
									else {
										Result = 1;
									}
								}
							}
						}
					}
					else {
						FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
						FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
						FM_Slot2 = 0;
						FM_Pointer2 = 0;
//						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2017.04.27
						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SCHEDULING_Possible_Pre_BM_4( CHECKING_SIDE , SignalBM ) , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2017.04.27
					}
				}
				else if ( doubleok == 3 ) { // (3) PLACE BM with ArmB
//						if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2005.10.06
					if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) { // 2005.10.06 // 2006.01.14
						FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
						FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
						FM_Slot = 0;
						FM_Pointer = 0;
						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
						if ( Result > 0 ) {
							if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) { // 2006.09.13
								if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , FM_Buffer , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 );
									if ( Result > 0 ) {
										if ( FM_Buffer == FM_Bufferx ) {
											Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 );
											if ( Result > 0 ) {
												if ( ( FM_Buffer + 1 ) == FM_Bufferx ) {
													//
													doubleok = 4; // (4) PLACE BM with ArmAB
													//
													FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
													FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
													FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
													FM_TSlot = FM_TSlotx;
													//
													FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
													FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
													FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
													FM_TSlot = FM_TSlot2x;
													//
													doubleok4isdouble = 3;
													//
												}
											}
											else {
												Result = 1;
											}
										}
									}
									else {
										Result = 1;
									}
								}
								else {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , FM_Buffer - 1 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 );
									if ( Result > 0 ) {
										if ( ( FM_Buffer - 1 ) == FM_Bufferx ) {
											FM_Buffer = FM_Buffer - 1;
											//
											doubleok = 4; // (4) PLACE BM with ArmAB
											//
											FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
											FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
											FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
											FM_TSlot = FM_TSlotx;
											//
											FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
											FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
											FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
											//
											doubleok4isdouble = 3;
										}
									}
									else {
										Result = 1;
									}
								}
							}
						}
					}
					else {
						FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
						FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
						FM_Slot = 0;
						FM_Pointer = 0;
//						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2017.04.27
						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SCHEDULING_Possible_Pre_BM_4( CHECKING_SIDE , SignalBM ) , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2017.04.27
					}
				}
				else if ( doubleok == 4 ) { // (4) PLACE BM with ArmAB
//						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2005.09.30 // 2006.01.06
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.09.30 // 2006.01.06
						if ( ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) || ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != _SCH_MODULE_Get_FM_SIDE( TB_STATION ) ) ) {
							FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
							FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
							//
							Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							//
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 13-2 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][dbs=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , doubleok4isdouble );
				//----------------------------------------------------------------------
							//
							if ( Result < 0 ) {
								FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
								FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								FM_Slot = 0;
								FM_Pointer = 0;
								Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide2 , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
								if ( Result > 0 ) {
									Result = 11;
									doubleok4isdouble = 2; // 2005.10.05
								}
							}
							else {
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) { // 2006.09.13
									if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
										Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , FALSE , 0 );
										//
										if ( ( Result >= 0 ) && ( ( FM_Buffer + 1 ) == FM_Bufferx ) ) {
											FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
											FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
											FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
											FM_TSlot2 = FM_TSlot2x;
											//
											Result = 21;
											doubleok4isdouble = 3; // 2005.09.30
											//
										}
										else {
											Result = 22;
											doubleok4isdouble = 1; // 2005.10.05
										}
									}
									else { // 2005.10.05
										Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , FM_Buffer - 1 , FM_Slot , 0 , FM_Pointer , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlotxx , FALSE , 0 );
										//
										if ( ( Result >= 0 ) && ( ( FM_Buffer - 1 ) == FM_Bufferx ) ) {
											Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , FALSE , 0 );
											if ( ( Result >= 0 ) && ( FM_Buffer == FM_Bufferx ) ) {
												FM_Buffer = FM_Buffer - 1;
												//
												FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
												FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
												FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
												FM_TSlot = FM_TSlotx;
												//
												FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
												FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
												FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
												FM_TSlot2 = FM_TSlot2x;
												//
												Result = 31;
												doubleok4isdouble = 3;
												//
											}
											else {
												Result = 32;
												doubleok4isdouble = 1; // 2005.10.05
											}
										}
										else {
											Result = 33;
											doubleok4isdouble = 1; // 2005.10.05
										}
									}
								}
								else {
									doubleok4isdouble = 1; // 2006.09.13
								}
							}
						}
						else {
							FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
							FM_Slot = 0;
							FM_Pointer = 0;
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							//
							if ( Result < 0 ) {
								FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								FM_Slot2 = 0;
								FM_Pointer2 = 0;
								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							}
						}
					}
					else {
						if ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) {
							FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
							FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							//
							if ( Result < 0 ) {
								FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								FM_Slot = 0;
								FM_Pointer = 0;
								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							}
						}
						else {
							FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
							FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
							FM_Slot = 0;
							FM_Pointer = 0;
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							//
							if ( Result < 0 ) {
								FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								FM_Slot2 = 0;
								FM_Pointer2 = 0;
								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							}
						}
					}
				}
				//===================================================================================================
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 14 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][dbs=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , doubleok4isdouble );
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				if ( Result < 0 ) {
					if ( SignalBM >= 0 ) {
						if ( ( _SCH_MODULE_Get_BM_FULL_MODE( SignalBM ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( SignalBM ) == BUFFER_FM_STATION ) ) {
							if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( SignalBM , 0 ) ) {
								dummode = SCHEDULER_CONTROL_Get_SDM_4_SUPPLY_WAFER_SLOT( CHECKING_SIDE , SignalBM - BM1 + PM1 , &FM_Pointer , &FM_Slot ); // 1:first 2:last 3:cycle
								if ( dummode != -1 ) {
									if ( dummode == 2 ) { // last
										FM_Slot2 = 0;
										FM_Pointer2 = 0;
										Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) , 0 ); // 2005.01.24
									}
									else {
										FM_Slot2 = 0;
										FM_Pointer2 = 0;
//											Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() == 1 ) , 0 ); // 2005.01.24
										Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) , 0 ); // 2005.01.24
									}
								}
							}
						}
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 15 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
					//----------------------------------------------------------------------
					if ( Result < 0 ) {
						for ( k = 0 ; k < _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; k++ ) {
							DM_Side = -99;
							if ( ( k + BM1 ) == SignalBM ) continue;
							if ( ( _SCH_MODULE_Get_BM_FULL_MODE( k + BM1 ) == BUFFER_WAIT_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( k + BM1 ) == BUFFER_FM_STATION ) ) {
								if ( !SCHEDULER_CONTROL_Chk_BM_FULL_FOR_4( k + BM1 , 0 ) ) {
									dummode = SCHEDULER_CONTROL_Get_SDM_4_SUPPLY_WAFER_SLOT( CHECKING_SIDE , k + PM1 , &FM_Pointer , &FM_Slot ); // 1:first 2:last 3:cycle
									if ( dummode != -1 ) {
										if ( dummode == 2 ) { // last
											FM_Slot2 = 0;
											FM_Pointer2 = 0;
											Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) , 0 ); // 2005.01.24
											if ( Result >= 0 ) break;
										}
										else {
											FM_Slot2 = 0;
											FM_Pointer2 = 0;
//												Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() == 1 ) , 0 ); // 2005.01.24
											Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) , 0 ); // 2005.01.24
											if ( Result >= 0 ) break;
										}
									}
								}
							}
						}
						if ( ( Result >= 0 ) && ( DM_Side != -99 ) && ( CHECKING_SIDE != DM_Side ) ) break;
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 16 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
					//----------------------------------------------------------------------
					if ( Result >= 0 ) {
	//					if ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , FM_Buffer ) <= MUF_00_NOTUSE ) Result = -1; // 2003.10.25
						if ( ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , FM_Buffer ) <= MUF_00_NOTUSE ) || ( _SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , FM_Buffer ) >= MUF_90_USE_to_XDEC_FROM ) ) Result = -1; // 2003.10.25
					}
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 17 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
					//----------------------------------------------------------------------
					if ( Result < 0 ) {
						if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) {
							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 0 , FALSE , 0 , 0 );
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 17b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
							if ( Result >= 0 ) {
//									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() == 1 ) , 0 ); // 2005.01.24
								while ( TRUE ) { // 2005.12.06
									//========================================================================================
									// 2007.04.03
									//========================================================================================
//										if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) ) { // 2007.04.03 // 2007.04.06
									if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && ( FM_Slot > 0 ) ) { // 2007.04.03 // 2007.04.06
										switch ( Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody3( CHECKING_SIDE , FM_Pointer , CHECKING_SIDE , FM_Slot2 , FM_Pointer2 , 0 ) ) {
										case 1 :	// 1 : A만 됨
											FM_Slot2 = 0;
											FM_Pointer2 = 0;
											break;
										case 2 :	// 2 : B만 됨
											FM_Slot2 = FM_Slot;
											FM_Pointer2 = FM_Pointer;
											FM_Slot = 0;
											FM_Pointer = 0;
											break;
										case -1 : // -1 : xx
											FM_Slot = 0;
											FM_Slot2 = 0;
											break;
										}
									}
									//========================================================================================
									// 2007.04.05
									//========================================================================================
									if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
										Result = -15;
										break;
									}
									//========================================================================================
									// 2007.04.03
									//========================================================================================
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
										//========================================================================
										// 2007.04.27
										//========================================================================
										if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( TRUE , 0 ) ) {
											if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
												if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
													//=================================================================================
													// 2007.05.10
													//=================================================================================
//														FM_Slot = 0;
//														FM_Slot2 = 0;
//														Result = -16;
//														break;
													//=================================================================================
													if ( FM_Slot > 0 ) {
														FM_Slot = 0;
														FM_Slot2 = 0;
														Result = -16;
														break;
													}
													else if ( FM_Slot2 > 0 ) {
														if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( FALSE , FM_Slot2 ) ) {
															FM_Slot = 0;
															FM_Slot2 = 0;
															Result = -16;
															break;
														}
														else {
															//======================================================================
															// 2007.11.28
															//======================================================================
															if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 2 ) { // 2007.04.26
																FM_Slot = 0;
																FM_Slot2 = 0;
																Result = -16;
																break;
															}
															//======================================================================
														}
													}
													//=================================================================================
												}
											}
											else {
												if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( FALSE ) ) { // 2007.04.27
													//=================================================================================
													// 2007.05.10
													//=================================================================================
//														FM_Slot = 0;
//														FM_Slot2 = 0;
//														Result = -16;
//														break;
													//=================================================================================
													if ( FM_Slot > 0 ) {
														FM_Slot = 0;
														FM_Slot2 = 0;
														Result = -16;
														break;
													}
													else if ( FM_Slot2 > 0 ) {
														if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( FALSE , FM_Slot2 ) ) {
															FM_Slot = 0;
															FM_Slot2 = 0;
															Result = -16;
															break;
														}
													}
													//=================================================================================
												}
												else {
													if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
														if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) { // 2007.08.22
															if ( !Scheduling_Check_Possible_for_BM_Place2_NO_Align( CHECKING_SIDE , FM_Pointer2 ) ) { // 2007.08.22
																FM_Slot = 0;
																FM_Slot2 = 0;
																Result = -16;
																break;
															}
														}
														else {
															//=================================================================================
															// 2007.06.15
															//=================================================================================
															FM_Slot = 0;
															FM_Slot2 = 0;
															Result = -16;
															break;
															//=================================================================================
														}
													}
													else {
														if ( !Scheduling_Check_Waiting_for_BM_Action( 3 ) ) { // 2007.04.27
															//=================================================================================
															// 2007.05.10
															//=================================================================================
	//														FM_Slot = 0;
	//														FM_Slot2 = 0;
	//														Result = -16;
	//														break;
															//=================================================================================
															if ( FM_Slot > 0 ) {
																FM_Slot = 0;
																FM_Slot2 = 0;
																Result = -16;
																break;
															}
															else if ( FM_Slot2 > 0 ) {
																if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( FALSE , FM_Slot2 ) ) {
																	FM_Slot = 0;
																	FM_Slot2 = 0;
																	Result = -16;
																	break;
																}
															}
															//=================================================================================
														}
														else {
															//=================================================================================
															// 2007.06.15
															//=================================================================================
															if ( FM_Slot > 0 ) {
																FM_Slot = 0;
																FM_Slot2 = 0;
																Result = -16;
																break;
															}
															//=================================================================================
														}
													}
												}
											}
										}
										//========================================================================
										if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
											//======================================================================
											// 2007.11.28
											//======================================================================
											if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 2 ) { // 2007.04.26
												FM_Slot = 0;
												FM_Slot2 = 0;
												Result = -17;
												break;
											}
											//======================================================================
											//============================================================================
											// 2007.04.27
											//============================================================================
											if ( Scheduling_Check_Waiting_for_BM_Action( 2 ) ) {
												FM_Slot = 0;
												FM_Slot2 = 0;
												Result = -17;
												break;
											}
											//============================================================================
											if ( Scheduling_Check_Waiting_for_BM_Action( 0 ) ) {
												if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , FALSE , -1 ) != 0 ) { // 2007.04.27
													FM_Slot = 0;
													FM_Slot2 = 0;
													Result = -18;
													break;
												}
											}
											else {
												//========================================================================
												// 2007.04.24
												//========================================================================
												if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
													if ( SCHEDULER_ALIGN_BUFFER_PLACE_IMPOSSIBLE( CHECKING_SIDE , FM_Pointer2 ) ) {
														FM_Slot = 0;
														FM_Slot2 = 0;
														Result = -19;
														break;
													}
												}
												//========================================================================
											}
										}
									}
									//========================================================================================
									// 2006.12.15
									//========================================================================================
//										if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.04.03
									if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && ( FM_Slot > 0 ) ) { // 2007.04.03
										SignalBM = BM1;
//											if ( SCHEDULER_CONTROL_OneBody_3_FIC_Full( TRUE ) ) { // 2007.04.27
//												Result = -1; // 2007.04.27
//											} // 2007.04.27
//											else { // 2007.04.27
											if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) { // 2007.04.10
												Result = 101;
												FM_Buffer = BM1;
												FM_TSlot = FM_TSlot2 = 1;
											}
											else {
												//========================================================================================
												Result = SCHEDULING_Possible_BM_3_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) ); // 2007.01.08
												//=====================================================================================================================
											}
//											} // 2007.04.27
										if ( ( Result > 0 ) && ( FM_Buffer != BM1 ) ) Result = -11; // 2007.04.03
									}
									else {
										if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) { // 2007.04.10
											Result = 101;
											FM_Buffer = BM1;
											FM_TSlot = FM_TSlot2 = 1;
										}
										else {
											//========================================================================================
											Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 1 ) , 0 ); // 2005.01.24
										}
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 17c RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
										//=====================================================================================================================
										// 2006.12.18
										//=====================================================================================================================
										if ( ( Result < 0 ) && ( SignalBM != -1 ) ) {
											if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) {
												if ( SCHEDULER_CONTROL_Chk_BM_HAS_SPACE_FOR_PALCE_WHEN_PRE_PICK_FOR_4( SignalBM ) ) {
													FM_Buffer = SignalBM;
													Result = 11;
												}
											}
										}
									}
									//=====================================================================================================================
									if ( Result >= 0 ) {
										k = SCHEDULER_CONTROL_Set_SDM_4_CHAMER_NORMAL_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , &DM_Side , 0 , 0 , &dummode , FALSE );
										if ( dummode ) {
											Result = -99;
											//============================================================================================================
											// 2005.12.21
											//============================================================================================================
											if ( k >= 0 ) {
												SCHEDULER_CONTROL_Get_SDM_4_CHAMER_PRE_RECIPE( DM_Side , SCHEDULER_CONTROL_Get_LotSpecial_Item_PM_DUMMY_LAST_PROCESS_DATA_for_STYLE_4( SDM4_PTYPE_LOTFIRST0 , CHECKING_SIDE , FM_Buffer - BM1 + PM1 ) , SDM4_PTYPE_LOTFIRST , RunRecipe , 64 );
												//
												_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
														 FM_Buffer - BM1 + PM1 ,
														 _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , (DM_Side % 100) + 1 , 0 ,
														 RunRecipe ,
														 2 ,
														 1 , "F" , PROCESS_DEFAULT_MINTIME ,
														 0 , 401 );
											}
											//============================================================================================================
										}
										//==================================================================================================================
										// 2005.12.06
										//==================================================================================================================
										if ( ( SignalBM == -1 ) && ( Result >= 0 ) ) {
											if ( sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] == -1 ) {
												if ( sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] == -1 ) {
													if ( sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] == -1 ) {
														if ( sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] == -1 ) {
															break;
														}
													}
												}
											}
											//-------------------------
											SignalBM = -FM_Buffer;
											//-------------------------
										}
										else {
											if ( SignalBM < -1 ) SignalBM = -1;
											break;
										}
										//==================================================================================================================
									}
									else { // 2005.12.06
										break;
									} // 2005.12.06
								} // 2005.12.06
								//
								//========================================================================================
								// 2014.01.15
								//========================================================================================
								if ( Result >= 0 ) {
									//
									if ( FM_Slot > 0 ) {
										if ( Scheduling_Check_Single_Arm_BM_Explict_Check( FM_Buffer , CHECKING_SIDE , FM_Pointer ) ) {
											FM_Slot = 0;
										}
									}
									if ( FM_Slot2 > 0 ) {
										if ( Scheduling_Check_Single_Arm_BM_Explict_Check( FM_Buffer , CHECKING_SIDE , FM_Pointer2 ) ) {
											FM_Slot2 = 0;
										}
									}
									//
									if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) {
										Result = -98;
									}
									//
								}
								//
							}
						}
					}
				}
				//=============================================================================================================================
				// 2006.01.24
				//=============================================================================================================================
				if ( doubleok != 0 ) {
					if ( Result > 0 ) {
						Result = -500 - Result;
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 18 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
				//----------------------------------------------------------------------
			}
			else { // 2004.06.25
				if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) == 0 ) {
					if      ( doubleok == 1 ) { // (1) PICK CMx with Armx | PLACE AL with Armx
						Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 1 , TRUE , 0 , 0 );
					}
					else if ( doubleok == 21 ) { // (21) PICK CMx with ArmA | PLACE AL with ArmA
//							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 2 , ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ); // 2006.11.03
						if ( clscheckskip ) { // 2006.11.18
							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 2 , FALSE , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ); // 2006.11.03
						}
						else {
							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 2 , ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ); // 2006.11.03
						}
						//===========================================
						if ( Result > 0 ) { // 2006.01.12
							if ( FM_Slot > 0 ) {
								if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) != CHECKING_SIDE ) {
									if ( SCHEDULER_CONTROL_Chk_FM_OUT_HAS_FirstRunPre_FOR_4( CHECKING_SIDE , FM_Pointer ) ) { 
										Result = -151;
									}
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) != CHECKING_SIDE ) {
									if ( SCHEDULER_CONTROL_Chk_FM_OUT_HAS_FirstRunPre_FOR_4( CHECKING_SIDE , FM_Pointer2 ) ) { 
										Result = -152;
									}
								}
							}
						}
						//===========================================
						if ( Result > 0 ) { // 2006.02.18
							if ( FM_Slot > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , CHECKING_SIDE , FM_Pointer ) ) {
									Result = -155;
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , CHECKING_SIDE , FM_Pointer2 ) ) {
									Result = -156;
								}
							}
						}
						//===========================================
					}
					else if ( doubleok == 22 ) { // (22) PICK CMx with ArmB | PLACE AL with ArmB
//							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 3 , ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ); // 2006.11.03
						if ( clscheckskip ) { // 2006.11.18
							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 3 , FALSE , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ); // 2006.11.03
						}
						else {
							Result = SCHEDULING_CHECK_Enable_PICK_from_FM_for_STYLE_4( 0 , CHECKING_SIDE , &FM_Slot , &FM_Slot2 , &FM_Pointer , &FM_Pointer2 , &FM_Side , &FM_Side2 , 3 , ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) , _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) ); // 2006.11.03
						}
						//===========================================
						if ( Result > 0 ) { // 2006.01.12
							if ( FM_Slot > 0 ) {
								if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != CHECKING_SIDE ) {
									if ( SCHEDULER_CONTROL_Chk_FM_OUT_HAS_FirstRunPre_FOR_4( CHECKING_SIDE , FM_Pointer ) ) { 
										Result = -153;
									}
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != CHECKING_SIDE ) {
									if ( SCHEDULER_CONTROL_Chk_FM_OUT_HAS_FirstRunPre_FOR_4( CHECKING_SIDE , FM_Pointer2 ) ) { 
										Result = -154;
									}
								}
							}
						}
						//===========================================
						if ( Result > 0 ) { // 2006.02.18
							if ( FM_Slot > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , CHECKING_SIDE , FM_Pointer ) ) {
									Result = -157;
								}
							}
							if ( FM_Slot2 > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_FM_OUT_AND_NEW_OUT_CONFLICT_FOR_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , CHECKING_SIDE , FM_Pointer2 ) ) {
									Result = -158;
								}
							}
						}
						//===========================================
					}
					doubleok = 1;
					//===========================================================================================================================
					// 2006.06.14
					//===========================================================================================================================
					if ( Result > 0 ) {
						if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 1 ) {
							Result = SCHEDULING_Possible_BM_for_STYLE2_4( CHECKING_SIDE , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 );
						}
						else if ( ( _SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() / 2 ) == 3 ) {
							Result = SCHEDULING_Possible_BM_for_STYLE2_4( CHECKING_SIDE , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 );
							if ( Result < 0 ) { 
								Result = SCHEDULING_Possible_BM_for_STYLE3_4( CHECKING_SIDE , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 );
							}
						}
					}
					//===========================================================================================================================
					//----------------------------------------------------------------------
					_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 19 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
					//----------------------------------------------------------------------
					// 2006.05.04
					//----------------------------------------------------------------------
					if ( Result > 0 ) { // 2006.05.04
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
							if ( !SCHEDULING_Possible_Process_for_STYLE_4( CHECKING_SIDE , FM_Pointer , -1 ) ) {
								if ( !SCHEDULING_Possible_Process_for_STYLE_4( CHECKING_SIDE , FM_Pointer2 , -1 ) ) {
									Result = -159;
								}
							}
						}
						else if ( FM_Slot > 0 ) {
							if ( !SCHEDULING_Possible_Process_for_STYLE_4( CHECKING_SIDE , FM_Pointer , -1 ) ) {
								Result = -159;
							}
						}
						else if ( FM_Slot2 > 0 ) {
							if ( !SCHEDULING_Possible_Process_for_STYLE_4( CHECKING_SIDE , FM_Pointer2 , -1 ) ) {
								Result = -159;
							}
						}
					}
					//----------------------------------------------------------------------
					// 2006.11.15
					//----------------------------------------------------------------------
					if ( Result > 0 ) {
						if ( FM_Slot > 0 ) {
							if ( !SCHEDULING_Possible_TargetSpace_for_STYLE_4( CHECKING_SIDE , FM_Pointer , TA_STATION , FALSE ) ) {
								Result = -181;
							}
						}
						if ( FM_Slot2 > 0 ) {
							if ( !SCHEDULING_Possible_TargetSpace_for_STYLE_4( CHECKING_SIDE , FM_Pointer2 , TB_STATION , FALSE ) ) {
								Result = -182;
							}
						}
					}
					//----------------------------------------------------------------------
					//----------------------------------------------------------------------
					//----------------------------------------------------------------------
					if ( Result < 0 ) { // 2005.09.07
						FMICPickReject = FALSE; // 2005.09.07
					} // 2005.09.07
				}
				else {
					FMICPickReject = FALSE; // 2005.09.07
				}
			}
			//=================================================================================================================================
			if ( Result >= 0 ) { // 2005.08.03
				if ( SCHEDULER_TM_HAS_WAFER_SUPPLY( -1 ) ) { // 2010.11.29
					Result = -901;
				}
			}
			//=================================================================================================================================
			//----------------------------------------------------------------------
			// 2005.07.20 Check Product type Change (Lot Special)
			//----------------------------------------------------------------------
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 101 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
//----------------------------------------------------------------------
			sp_data = 0;
			sp_start = 0;
			//
//				if ( Result >= 0 ) { // 2005.09.06
			if ( ( Result >= 0 ) && ( FM_Buffer >= BM1 ) ) { // 2005.09.06
				if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
					if ( sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -301;
					}
					if ( sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -302;
					}
					if ( sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -303;
					}
					if ( sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -304;
					}
				}
				if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
					if ( sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -305;
					}
					if ( sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -306;
					}
					if ( sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -307;
					}
					if ( sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] != -1 ) {
						Result = -308;
					}
				}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 102 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
//----------------------------------------------------------------------
				//=================================================================================================================================
				if ( Result >= 0 ) { // 2005.08.03
					if ( ( FM_Slot > 0 ) && ( FM_Pointer >= 100 ) ) {
						if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2( CHECKING_SIDE , FM_Pointer ) ) {
							if ( !_SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) {
								// 다른 Lot이 continue 가 있고 그 Lot의 첫번째가 Product Change 일때

								if ( SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , FALSE ) ) {

//printf( "======================================================================================================================\n" );
//printf( "======================================================================================================================\n" );
//printf( "TEST 1 => CYCLE SKIP\n" );
//printf( "======================================================================================================================\n" );
//printf( "======================================================================================================================\n" );
									SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2_RESET( CHECKING_SIDE , FM_Pointer );
									Result = -401;
								}

							}
						}
					}
					else if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 >= 100 ) ) {
						if ( SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2( CHECKING_SIDE , FM_Pointer2 ) ) {
							if ( !_SCH_SUB_Remain_for_FM( CHECKING_SIDE ) ) {
								if ( SCHEDULER_CONTROL_Chk_Other_LotSpecialChange( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , FALSE ) ) {

//printf( "======================================================================================================================\n" );
//printf( "======================================================================================================================\n" );
//printf( "TEST 2 => CYCLE SKIP\n" );
//printf( "======================================================================================================================\n" );
//printf( "======================================================================================================================\n" );

									SCHEDULER_CONTROL_Get_SDM_4_HAS_LOTCYCLE_WAFER2_RESET( CHECKING_SIDE , FM_Pointer2 );
									Result = -402;
								}
							}
						}
					}
				}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 103 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
//----------------------------------------------------------------------
				//=================================================================================================================================
				if ( Result >= 0 ) {
					if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
						if ( SCHEDULING_CONTROL_Check_LotSpecial_Item_Change_for_STYLE_4( CHECKING_SIDE , FM_Pointer , FM_Buffer - BM1 + PM1 , &sp_old , &sp_new ) ) {
							sp_data = 1;
						}
					}
				}
				if ( Result >= 0 ) {
					if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
						if ( SCHEDULING_CONTROL_Check_LotSpecial_Item_Change_for_STYLE_4( CHECKING_SIDE , FM_Pointer2 , FM_Buffer - BM1 + PM1 , &sp_old , &sp_new ) ) {
							if ( FM_Slot > 0 ) {
								FM_Slot2 = 0;
							}
							else {
								sp_data = 2;
							}
						}
					}
				}
			}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 104 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
//----------------------------------------------------------------------
			if ( Result < 0 ) { // 2005.08.01
				switch( SCHEDULER_CONTROL_Get_END_FST_S2_TAG( CHECKING_SIDE , &sp_index ) ) { // 2005.10.25
				case 1 :
					Result = 301;
					FM_Slot = (sch4_RERUN_END_S2_TAG[sp_index] % 100 ) + 1;
					FM_Pointer = sch4_RERUN_END_S2_TAG[sp_index];
					FM_Slot2 = 0;
					FM_Pointer2 = 0;
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sch4_RERUN_END_S2_TAG[sp_index] , TRUE ); // 2005.08.01
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( sp_index ); // 2005.08.03
					sch4_RERUN_END_S2_TAG[sp_index] = -1;
					sp_start = 1;
					break;
				case 2 :
					Result = 302;
					FM_Slot = (sch4_RERUN_FST_S2_TAG[sp_index] % 100 ) + 1;
					FM_Pointer = sch4_RERUN_FST_S2_TAG[sp_index];
					FM_Slot2 = 0;
					FM_Pointer2 = 0;
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sch4_RERUN_FST_S2_TAG[sp_index] , FALSE ); // 2005.08.01
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( sp_index ); // 2005.08.03
					sch4_RERUN_FST_S2_TAG[sp_index] = -1;
					sp_start = 2;
					break;
				}
			}
//----------------------------------------------------------------------
_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 105 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][sp_data=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , sp_data );
//----------------------------------------------------------------------
			//=========================================================================================================================
			if ( sp_data != 0 ) {

//----------------------------------------------------------------------------------------------------------------
_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM OLD Data1 PM%d [%d] [E/S1=%d,%d][E/S2=%d,%d][F/S1=%d,%d][F/S2=%d,%d]%d\n" , FM_Buffer - BM1 + 1 , sp_data ,
					sch4_RERUN_END_S1_ETC[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] ,
					sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] );
//----------------------------------------------------------------------------------------------------------------

				//----------------------------------------------------------------------------------------------------------------
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Lot-Special Data Change(%d->%d) for PM%d%cWHFMLOTSPECIAL %d:%d:PM%d\n" , sp_old , sp_new , FM_Buffer - BM1 + 1 , 9 , sp_old , sp_new , FM_Buffer - BM1 + 1 );
				//----------------------------------------------------------------------------------------------------------------
				if ( sp_data == 1 ) SCHEDULING_CONTROL_Check_LotSpecial_Item_Set_for_STYLE_4( CHECKING_SIDE , FM_Pointer , FM_Buffer - BM1 + PM1 );
				if ( sp_data == 2 ) SCHEDULING_CONTROL_Check_LotSpecial_Item_Set_for_STYLE_4( CHECKING_SIDE , FM_Pointer2 , FM_Buffer - BM1 + PM1 );
				//----------------------------------------------------------------------------------------------------------------
				splogdata = sp_data;
				//----------------------------------------------------------------------------------------------------------------
				switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTEND( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , &sp_pointer , sp_old ) ) {
				case 0 : // End Nothing
					//---------------------------------------------------------------
					// 2006.08.30
					//---------------------------------------------------------------
					if ( sp_pointer == 0 ) {
						sp_data = 0;
					}
					//---------------------------------------------------------------
					switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , 2 , sp_pointer , &sp_pointer2 , sp_new ) ) {
					case 0 : // First Nothing : OK
						// Need Reset Count in PM
						// Need Reset Count in PM
						// Need Reset Count in PM
						// Need Reset Count in PM
						break;
					case 1 : // First S1:Spawn : OK
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						//---------------------------------------------------------------
						break;
					case 2 : // First S1:Spawn + S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 200;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						if ( sp_data == 0 ) { // 2010.10.20
							sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
							sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						}
						else {
							sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
							sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
							//
							if ( sp_data == 1 ) {
								FM_Slot = ( sp_pointer2 % 100 ) + 1;
								FM_Pointer = sp_pointer2;
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
								FM_Slot2 = 0;
								FM_Pointer2 = 0;
							}
							if ( sp_data == 2 ) {
								FM_Slot = 0;
								FM_Pointer = 0;
								FM_Slot2 = ( sp_pointer2 % 100 ) + 1;
								FM_Pointer2 = sp_pointer2;
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
							}
							sp_start = 2;
						}
						//---------------------------------------------------------------
						break;
					case 3 : // First S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 300;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer2 % 100 ) + 1;
							FM_Pointer = sp_pointer2;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer2 % 100 ) + 1;
							FM_Pointer2 = sp_pointer2;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
						}
						sp_start = 2;
						//---------------------------------------------------------------
						break;
					case 4 : // First S1:Spawn + S2:Pick:Wait
						//---------------------------------------------------------------
						splogdata = splogdata + 400;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sp_data = 0;
						//---------------------------------------------------------------
						break;
					case 5 : // First S2:Pick:Wait
						//---------------------------------------------------------------
						splogdata = splogdata + 500;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sp_data = 0;
						//---------------------------------------------------------------
						break;
					}
					break;
				case 1 : // End S1:Spawn
					//---------------------------------------------------------------
					splogdata = splogdata + 10;
					//---------------------------------------------------------------
					//---------------------------------------------------------------
					sch4_RERUN_END_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_old;
					sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer;
					//
					sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
					sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
					//---------------------------------------------------------------
					switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , 0 , sp_pointer , &sp_pointer2 , sp_new ) ) {
					case 0 : // First Nothing : OK
						break;
					case 1 : // First S1:Spawn : OK
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						//---------------------------------------------------------------
						break;
					case 2 : // First S1:Spawn + S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 200;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						//
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer2 % 100 ) + 1;
							FM_Pointer = sp_pointer2;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer2 % 100 ) + 1;
							FM_Pointer2 = sp_pointer2;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
						}
						sp_start = 2;
						//---------------------------------------------------------------
						break;
					case 3 : // First S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 300;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer2 % 100 ) + 1;
							FM_Pointer = sp_pointer2;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer2 % 100 ) + 1;
							FM_Pointer2 = sp_pointer2;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer2 , FALSE );
						}
						sp_start = 2;
						//---------------------------------------------------------------
						break;
					case 4 : // First S1:Spawn + S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 400;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//---------------------------------------------------------------
						sp_data = 0;
						break;
					case 5 : // First S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 500;
						//---------------------------------------------------------------
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//---------------------------------------------------------------
						sp_data = 0;
						break;
					}
					//
					break;
				case 2 : // End S1:Spawn + S2:Pick
					//---------------------------------------------------------------
					splogdata = splogdata + 20;
					//---------------------------------------------------------------
					//---------------------------------------------------------------
					sch4_RERUN_END_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_old;
					sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer;
					//
					sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
					sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
					//---------------------------------------------------------------
					sp_start = 1;
					//---------------------------------------------------------------
					switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , 1 , sp_pointer , &sp_pointer2 , sp_new ) ) {
					case 0 : // First Nothing : OK
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						break;
					case 1 : // First S1:Spawn : OK
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						//
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						break;
					case 2 : // First S1:Spawn + S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 4 : // First S1:Spawn + S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 400;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 3 : // First S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 300;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 5 : // First S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 500;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					}
					//
					break;
				case 3 : // End            S2:Pick
					//---------------------------------------------------------------
					splogdata = splogdata + 30;
					//---------------------------------------------------------------
					//---------------------------------------------------------------
					sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
					sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
					//---------------------------------------------------------------
					sp_start = 1;
					//---------------------------------------------------------------
					switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , 1 , sp_pointer , &sp_pointer2 , sp_new ) ) {
					case 0 : // First Nothing : OK
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						break;
					case 1 : // First S1:Spawn : OK
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						break;
					case 2 : // First S1:Spawn + S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 200;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 4 : // First S1:Spawn + S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 400;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 3 : // First S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 300;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 5 : // First S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 500;
						//---------------------------------------------------------------
						if ( sp_data == 1 ) {
							FM_Slot = ( sp_pointer % 100 ) + 1;
							FM_Pointer = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
						}
						if ( sp_data == 2 ) {
							FM_Slot = 0;
							FM_Pointer = 0;
							FM_Slot2 = ( sp_pointer % 100 ) + 1;
							FM_Pointer2 = sp_pointer;
							SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_S2_WHAT( sp_pointer , TRUE );
						}
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					}
					//
					break;
				case 4 : // End S1:Spawn + S2:Pick:Wait
					//---------------------------------------------------------------
					splogdata = splogdata + 40;
					//---------------------------------------------------------------
					//---------------------------------------------------------------
					sch4_RERUN_END_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_old;
					sch4_RERUN_END_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer;
					//
					sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_old;
					sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer;
					//---------------------------------------------------------------
					sp_data = 0;
					//---------------------------------------------------------------
					switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , 1 , sp_pointer , &sp_pointer2 , sp_new ) ) {
					case 0 : // First Nothing : OK
						break;
					case 1 : // First S1:Spawn : OK
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						break;
					case 2 : // First S1:Spawn + S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 200;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 4 : // First S1:Spawn + S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 400;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 3 : // First S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 300;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 5 : // First S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 500;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					}
					//
					break;
				case 5 : // End            S2:Pick:Wait
					//---------------------------------------------------------------
					splogdata = splogdata + 50;
					//---------------------------------------------------------------
					//---------------------------------------------------------------
//						sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new; // 2005.10.25
					sch4_RERUN_END_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_old; // 2005.10.25
					sch4_RERUN_END_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer;
					//---------------------------------------------------------------
					sp_data = 0;
					//---------------------------------------------------------------
					switch( SCHEDULER_CONTROL_Set_SDM_4_CHAMER_AT_SPD_CHANGE_LOTFIRST( CHECKING_SIDE , FM_Buffer - BM1 + PM1 , 1 , sp_pointer , &sp_pointer2 , sp_new ) ) {
					case 0 : // First Nothing : OK
						break;
					case 1 : // First S1:Spawn : OK
						//---------------------------------------------------------------
						splogdata = splogdata + 100;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = -1;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = -1;
						break;
					case 2 : // First S1:Spawn + S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 200;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 4 : // First S1:Spawn + S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 400;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S1_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S1_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						//
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 3 : // First S2:Pick : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 300;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					case 5 : // First S2:Pick:Wait : ?
						//---------------------------------------------------------------
						splogdata = splogdata + 500;
						//---------------------------------------------------------------
						sch4_RERUN_FST_S2_ETC[ FM_Buffer - BM1 + PM1 ] = sp_new;
						sch4_RERUN_FST_S2_TAG[ FM_Buffer - BM1 + PM1 ] = sp_pointer2;
						break;
					}
					break;
				}
				//-----------------------------------------------------------------------------------------
				if ( sp_data == 0 ) { // Wait
					Result = -201;
					SCHEDULER_CONTROL_Reset_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 ); // 2006.09.03
				}
				else {
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_FORCE_CLEAR_RUN( FM_Buffer - BM1 + PM1 ); // 2005.08.03
				}
				//----------------------------------------------------------------------------------------------------------------
				// 2006.07.10
				//----------------------------------------------------------------------------------------------------------------
				_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Lot-Special PM%d Management Data is(%d)(%d,%d)(%d,%d)(%d,%d)%cWHFMLOTSPECIAL2 PM%d:%d:%d,%d:%d,%d:%d,%d\n" , FM_Buffer - BM1 + 1 , splogdata , sp_old , sp_pointer , sp_new ,  sp_pointer2 , sp_data , Result , 9 , FM_Buffer - BM1 + 1 , splogdata , sp_old , sp_pointer , sp_new ,  sp_pointer2 , sp_data , Result );
				//----------------------------------------------------------------------------------------------------------------
			}
			//----------------------------------------------------------------------
			// 2005.03.17 Move Here & Modify
			//----------------------------------------------------------------------
			if ( Result >= 0 ) {
				if ( ( ( FM_Slot <= 0 ) || ( FM_Pointer < 100 ) ) && ( ( FM_Slot2 <= 0 ) || ( FM_Pointer2 < 100 ) ) ) {
//						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.11.10
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.10 // 2007.01.06
						if ( ( doubleok == 1 ) || ( doubleok == 21 ) || ( doubleok == 22 ) ) {
							if ( !FMPickForce && ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 2 ) ) { // 2005.09.07
//								if ( _SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() >= 2 ) { // 2005.09.07
								if ( !_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) {
									Result = -81;
								}
							}
						}
					}
					else {
						if ( !_SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( CHECKING_SIDE , TRUE , 1 ) ) { // 2004.08.26
							Result = -82;
						}
					}
				}
			}
			//==============================================================================================
			// 2006.10.13
			//==============================================================================================
			if ( Result >= 0 ) {
//					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) { // 2007.01.08 // 2007.01.16
				if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2007.01.08 // 2007.01.16
					if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) && ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) {
							switch ( Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody( CHECKING_SIDE , FM_Pointer , CHECKING_SIDE , FM_Pointer2 ) ) {
							case 0 :	// 0 : 갈데없음
							case 1 :	// 1 : OneBody의 같은 BM에만 갈수있음
								FM_Slot2 = 0;
								FM_Pointer2 = 0;
								break;
							case 2 :	// 2 : OneBody의 다른 BM에만 갈수있음
								break;
							case 3 :	// 3 : 다른 Body에만 갈수있음
								break;
							case 4 :	// 4 : 기타
								break;
							}
						}
					}
				}
				//=============================================================================================================================================
				// 2007.03.29
				//=============================================================================================================================================
				else if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
					if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
						switch ( Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody3( CHECKING_SIDE , FM_Pointer , CHECKING_SIDE , FM_Slot2 , FM_Pointer2 , 0 ) ) {
						case 1 :	// 1 : A만 됨
							FM_Slot2 = 0;
							FM_Pointer2 = 0;
							break;
						case 2 :	// 2 : B만 됨
							FM_Slot2 = FM_Slot;
							FM_Pointer2 = FM_Pointer;
							FM_Slot = 0;
							FM_Pointer = 0;
							break;
						case -1 :	// xx
							FM_Slot = 0;
							FM_Slot2 = 0;
							Result = -81;
							break;
						}
					}
					//-----------------------------------------------------------------------------
					// 2008.05.07
					//-----------------------------------------------------------------------------
					else if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
						switch ( Scheduling_Check_Two_is_Not_Explict_when_BM_OneBody3( CHECKING_SIDE , FM_Pointer2 , 0 , 0 , 0 , 5 ) ) {
						case -1 :	// xx
							FM_Slot = 0;
							FM_Slot2 = 0;
							Result = -82;
							break;
						}
					}
					//-----------------------------------------------------------------------------
				}
				//=============================================================================================================================================
			}
			//==============================================================================================
			// 2004.05.10
			//==============================================================================================
			if ( Result >= 0 ) {
				if ( ( Result >= 0 ) && ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
					if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , FM_Pointer ) ) {
						Result = -101;
					}
				}
				if ( ( Result >= 0 ) && ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
					if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( CHECKING_SIDE , FM_Pointer2 ) ) {
						Result = -101;
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 20 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
			//----------------------------------------------------------------------
			//==============================================================================================
			// 2006.09.02
			//==============================================================================================
			if ( Result >= 0 ) {
				if ( FM_Buffer >= BM1 ) {
					if ( FM_Slot > 0 ) {
						if ( FM_Pointer < 100 ) {
							if ( SCHEDULER_CONTROL_Run_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 ) ) {
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 );
								Result = -901;
							}
						}
						else {
							SCHEDULER_CONTROL_Reset_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 );
						}
					}
					if ( FM_Slot2 > 0 ) {
						if ( FM_Pointer2 < 100 ) {
							if ( SCHEDULER_CONTROL_Run_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 ) ) {
								SCHEDULER_CONTROL_Set_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 );
								Result = -902;
							}
						}
						else {
							SCHEDULER_CONTROL_Reset_SDM_4_CHAMER_ENDFINISH( FM_Buffer - BM1 + PM1 );
						}
					}
				}
			}
			//==============================================================================================
			if ( Result >= 0 ) { // 2010.09.03
				if ( SCHEDULER_CONTROL_Chk_BM_FM_SINGLE_BMOUT_FIRST_FOR_4() ) Result = -1201;
			}
			//==============================================================================================
			if ( Result >= 0 ) {
//					if ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 1 ) { // 2003.06.20 // 2005.01.24
				if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2003.06.20 // 2005.01.24
					if ( FM_Pointer < 100 ) {
						if ( FM_Buffer >= BM1 ) { // 2006.01.06
							if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( FM_Buffer ) ) {
								SCHEDULER_Set_FM_WILL_GO_BM( CHECKING_SIDE , FM_Buffer );
							}
						}
					}
				}
				if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
					if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , FM_Pointer ) ) {
							_SCH_MODULE_Set_BM_MidCount( CHECKING_SIDE + 1 );
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
					}
					_SCH_MODULE_Inc_FM_DoPointer( CHECKING_SIDE ); // ***********
					//======================================================================================
					_SCH_SUB_Remain_for_FM( CHECKING_SIDE ); // 2003.06.05
					//======================================================================================
					if ( FM_Buffer >= BM1 ) { // 2006.01.06
						SCHEDULING_CONTROL_Check_LotSpecial_Item_Set_for_STYLE_4( CHECKING_SIDE , FM_Pointer , FM_Buffer - BM1 + PM1 ); // 2005.07.27
					}
					//======================================================================================
				}
				if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
					if ( _SCH_SUB_Get_Last_Status( CHECKING_SIDE ) == 0 ) {
						if ( _SCH_SUB_Chk_Last_Out_Status( CHECKING_SIDE , FM_Pointer2 ) ) {
							_SCH_MODULE_Set_BM_MidCount( CHECKING_SIDE + 1 );
							_SCH_SUB_Set_Last_Status( CHECKING_SIDE , 1 );
						}
					}
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) { // 2007.01.06
						_SCH_MODULE_Inc_FM_DoPointer( CHECKING_SIDE ); // ***********
					}
					//======================================================================================
					_SCH_SUB_Remain_for_FM( CHECKING_SIDE ); // 2003.06.05
					//======================================================================================
					if ( FM_Buffer >= BM1 ) { // 2006.01.06
						SCHEDULING_CONTROL_Check_LotSpecial_Item_Set_for_STYLE_4( CHECKING_SIDE , FM_Pointer2 , FM_Buffer - BM1 + PM1 ); // 2005.07.27
					}
					//======================================================================================
				}
				//=======================================================================================================
				FMICPickReject = FALSE; // 2005.09.07
				FMPickForce = FALSE; // 2005.09.07
				//=======================================================================================================
				if ( ( FM_Slot > 0 ) && ( FM_Pointer >= 100 ) ) {
					_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot , MAX_CASSETTE_SIDE + 1 );
					_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot , 0 );
					_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot , FM_Slot );
					//
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_RUN( FM_Pointer );
				}
				if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 >= 100 ) ) {
					_SCH_IO_SET_MODULE_SOURCE( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot2 , MAX_CASSETTE_SIDE + 1 );
					_SCH_IO_SET_MODULE_RESULT( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot2 , 0 );
					_SCH_IO_SET_MODULE_STATUS( _SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , FM_Slot2 , FM_Slot2 );
					//
					SCHEDULER_CONTROL_Set_SDM_4_CHAMER_GO_RUN( FM_Pointer2 );
				}
				//----------------------------------------------------------------------------
				//----------------------------------------------------------------------------
				if ( ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) || ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) ) { // 2005.03.16
//						_SCH_CASSETTE_Reset_Side_Monitor( CHECKING_SIDE , FALSE ); // 2007.11.27
					_SCH_CASSETTE_Set_Side_Monitor_SupplyDone( CHECKING_SIDE , ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ? 2 : 1 ); // 2003.10.10
				}
				//----------------------------------------------------------------------------
				//----------------------------------------------------------------------------
				_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 11 );
				//-----------------------------------------------
				//----------------------------------------------
				_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
				//----------------------------------------------
				if ( FM_Slot  > 0 ) {
					//======================================================================
					_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
					//======================================================================
					if ( FM_Pointer >= 100 ) _SCH_MODULE_Inc_TM_DoubleCount( CHECKING_SIDE ); // 2004.09.07
					//======================================================================
				}
				if ( FM_Slot2 > 0 ) {
					//======================================================================
					_SCH_MODULE_Inc_FM_OutCount( CHECKING_SIDE );
					//======================================================================
					if ( FM_Pointer2 >= 100 ) _SCH_MODULE_Inc_TM_DoubleCount( CHECKING_SIDE ); // 2004.09.07
					//======================================================================
				}
				//----------------------------------------------------------------------
				// Code for Pick from CM
				// FM_Slot
				//----------------------------------------------------------------------
				//
				//======================================================================================================
				_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
				//======================================================================================================
				FM_Slot_Real = 0;
				//
				if ( FM_Slot > 0 ) { // 2007.01.08
					if ( FM_Pointer < 100 ) {
						//===============================================================================================================
						_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , FM_Pointer , 0 , 0 , 0 );
						//===============================================================================================================
						_SCH_SUB_GET_REAL_SLOT_FOR_MULTI_A_ARM( 0 , FM_Slot , &FM_Slot_Real , &k );
						//===============================================================================================================
						_SCH_FMARMMULTI_DATA_SET_FROM_CM( CHECKING_SIDE , 0 , FM_Slot_Real , k ); // 2007.08.01
						//===============================================================================================================
						_SCH_FMARMMULTI_PICK_FROM_CM_POST( 0 , 0 ); // 2007.07.11
						//===============================================================================================================
					}
					else {
						FM_Slot_Real = FM_Slot;
					}
				} // 2007.01.08
				if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
//						SCHMULTI_MESSAGE_PROCESSJOB_START_CHECK( CHECKING_SIDE , FM_Pointer2 ); // 2002.06.24 // 2004.05.10
					_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( CHECKING_SIDE , FM_Pointer2 , 0 , 0 , 0 ); // 2006.11.27
					//----------------------------------------------------------------------------------------------------------------------------
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
						_SCH_MODULE_Inc_FM_DoPointer( CHECKING_SIDE ); // ***********
					}
					//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				}
				//-----------------------------------------------
				FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
				//-----------------------------------------------
				//=========================================================================
				switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PICK + 540 , -1 , FM_Slot , FM_Slot_Real , CHECKING_SIDE , FM_Pointer , -1 , FM_Slot2 , FM_Slot2 , CHECKING_SIDE , FM_Pointer2 , -1 ) ) { // 2007.03.21
				case -1 :
					return 0;
					break;
				}
				//=========================================================================
				//========================================================================================
				SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
				//========================================================================================
				//----------------------------------------------------------------------
				if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
					//========================================================================================
					_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL ); // 2004.08.11
					_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot );
					//========================================================================================
					_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL ); // 2004.08.11
					_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 );
					//========================================================================================
					// 2007.09.03
					//======================================================================================================
					FM_Has_Swap_Wafer_Style_4 = TRUE;
					//=========================================================================
				}
				else if ( FM_Slot > 0 ) {
					_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL ); // 2004.08.11
					_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot );
					//===============================================================================================================
					// 2006.11.10
					//===============================================================================================================
//						if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
					//===============================================================================================================
					//========================================================================================
//						_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 );
					//========================================================================================
					// 2007.09.03
					//======================================================================================================
					FM_Has_Swap_Wafer_Style_4 = FALSE;
					//=========================================================================
				}
				else if ( FM_Slot2 > 0 ) {
					_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , _SCH_MODULE_Need_Do_Multi_FAL() ? FMWFR_PICK_CM_NEED_AL : FMWFR_PICK_CM_DONE_AL ); // 2004.08.11
					_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 ); // bug 2004.07.07
					//===============================================================================================================
					// 2006.11.10
					//===============================================================================================================
//						if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
					//===============================================================================================================
					//========================================================================================
//						_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // bug 2004.07.07
////					_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 ); // bug 2004.07.07
					//========================================================================================
					// 2007.09.03
					//======================================================================================================
					FM_Has_Swap_Wafer_Style_4 = FALSE;
					//=========================================================================
				}
				//----------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 14 from FEM%cWHFMFAIL 14\n" , 9 );
					return 0;
				}
				//====================================================================================================================
				// 2007.09.17
				//====================================================================================================================
				SCHEDULER_CONTROL_Chk_SEPARATE_SUPPLY_for_STYLE_4( TRUE , -1 , CHECKING_SIDE , FM_Slot , FM_Pointer , FM_Slot2 , FM_Pointer2 );
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 20a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
				//----------------------------------------------------------------------
				//====================================================================================================================
				// 2006.10.14
				//====================================================================================================================
//					if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( doubleok == 1 ) ) {
				switch ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , CHECKING_SIDE , FALSE , &sp_start , TRUE , FALSE ) ) {
				case 2 :
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
					return 0;
					break;
				case 1 : // 2006.10.31
					osp = TRUE; // 2006.10.31
					break; // 2006.10.31
				default : // 2006.10.31
					osp = FALSE; // 2006.10.31
					break; // 2006.10.31
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 20b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
				//----------------------------------------------------------------------
				//====================================================================================================================
				// 2007.09.17
				//====================================================================================================================
				if ( !osp ) {
					SCHEDULER_CONTROL_Chk_SEPARATE_SUPPLY_for_STYLE_4( FALSE , FM_Buffer , CHECKING_SIDE , FM_Slot , FM_Pointer , FM_Slot2 , FM_Pointer2 );
				}
				//====================================================================================================================
				//====================================================================================================================
				//----------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 16 from FEM%cWHFMFAIL 16\n" , 9 );
					return 0;
				}
				//==================================================================================================
				// 2007.01.08
				//==================================================================================================
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.08

// 2007.08.23 imsi Code for cooler swapping (3/4)
/*
					if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
						if ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < 100 ) {
							_SCH_CLUSTER_Set_SwitchInOut( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 1 );
						}
					}
					else if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
						if ( _SCH_MODULE_Get_FM_POINTER( TB_STATION ) < 100 ) {
							_SCH_CLUSTER_Set_SwitchInOut( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , 1 );
						}
					}
*/
					//====================================================================================================================
					if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( FALSE ) ) {
						if ( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == F_IC ) {
							if ( ( FM_Slot <= 0 ) || ( FM_Slot2 <= 0 ) ) {
								if ( !osp ) {
									if ( FM_Slot2 > 0 ) {
										if ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , -1 , TRUE , &sp_start , FALSE , FALSE ) ) { // 2007.01.22
											if ( SCHEDULER_CONTROL_Get_Free_OneBody_3_FIC_Slot( &ICsts , &ICsts2 ) ) { // 2007.01.08
												//-------------------------------------------------------
												dal_sldata = 0;
												for ( dal_xl = 0 ; dal_xl < 32 ; dal_xl++ ) {
													dal_s[dal_xl] = -1;
													dal_w[dal_xl] = 0;
												}
												//-------------------------------------------------------
												_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , 0 , FMWFR_PICK_CM_DONE_AL );
												_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 99 );
												//-------------------------------------------------------
												while( TRUE ) {
													//----------------------------------------------------------------------
													// place albm(IC) armB slot s1+1
													//----------------------------------------------------------------------
													if ( SCHEDULER_ALIGN_BUFFER_PLACE_TO_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , 2 , 2 , &FM_dbSide , &FM_Pointer , &FM_Slot , &(dal_s[dal_sldata]) , &(dal_p[dal_sldata]) , &(dal_w[dal_sldata]) , ICsts + dal_sldata , FALSE ) == -1 ) {
														return 0;
													}
													//----------------------------------------------------------------------
													dal_sldata++;
													if ( dal_sldata >= ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ) {
														break;
													}
													//----------------------------------------------------------------------
													osp = FALSE;
													switch ( Scheduling_Other_Side_Pick_From_FM_for_4( TRUE , CHECKING_SIDE , FALSE , &sp_start , FALSE , FALSE ) ) {
													case 2 :
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
														return 0;
														break;
													case 1 :
														osp = TRUE;
														break;
													default :
														switch ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , -1 , FALSE , &sp_start , FALSE , FALSE ) ) {
														case 2 :
															_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
															return 0;
															break;
														case 1 :
															osp = TRUE;
															break;
														default :
															osp = FALSE;
															break;
														}
														break;
													}
													if ( !osp ) break;
												}
//													_SCH_MODULE_Set_FM_POINTER( TA_STATION , 0 ); // 2007.09.12
												_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 0 );
												//====================================================
												// pick albm	All
												//====================================================
												if ( dal_sldata == ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ) {
													_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , dal_p[dal_sldata-1] , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
													_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , dal_s[dal_sldata-1] , dal_w[dal_sldata-1] );
													//
													dal_s2 = ICsts + dal_sldata - 1;
													//
													FM_Slot2 = dal_w[dal_sldata-1];
													FM_Pointer2 = dal_p[dal_sldata-1];
													//
													dal_sldata--;
													//
												}
												else {
													_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , 0 , 0 );
													dal_s2 = 0;
													FM_Slot2 = 0;
												}
												FM_Slot = 0;
												if ( dal_sldata > 0 ) {
													_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , dal_p[0] , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
													_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , dal_s[0] , dal_w[0] );
													FM_Slot = dal_w[0];
													FM_Pointer = dal_p[0];
													//
													_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
													for ( dal_xl = 1 ; dal_xl < dal_sldata ; dal_xl++ ) {
														_SCH_FMARMMULTI_DATA_Append( dal_s[dal_xl] , dal_p[dal_xl] , dal_w[dal_xl] , dal_w[dal_xl] ); // 2007.07.11
													}
													//==================================================
													//
													//==================================================
													//-----------------------------------------------
													FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
													//-----------------------------------------------
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%d:%d,%d:%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , dal_w[0] , 1 , dal_w[3] , dal_s2 , 9 , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , dal_w[0] , 1 , dal_w[3] , dal_s2 , 9 , ( dal_s2 * 100 ) + 1 );
													if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , ( dal_s2 == 0 ) ? FAL_RUN_MODE_PICK : FAL_RUN_MODE_PICK_ALL , CHECKING_SIDE , IC , ICsts , dal_s2 , -1 , -1 , FALSE , dal_w[0] , dal_w[3] , dal_s[0] , dal_s[3] , dal_p[0] , dal_p[3] ) == SYS_ABORTED ) {
														_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Fail from %s(%d:%d,%d:%d)%cWHFMPICKFAIL %s:%d:%d,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , dal_w[0] , 1 , dal_w[3] , dal_s2 , 9 , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , dal_w[0] , 1 , dal_w[3] , dal_s2 , 9 , ( dal_s2 * 100 ) + 1 );
														return 0;
													}
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%d:%d,%d:%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , dal_w[0] , 1 , dal_w[3] , dal_s2 , 9 , _SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , dal_w[0] , 1 , dal_w[3] , dal_s2 , 9 , ( dal_s2 * 100 ) + 1 );
	//												Result = -999;
//														_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
//														_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
													//======================================================
												}
												//====================================================
											}
										}
									}
								}
							}
						}
						else { // Align Buffer Use
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1000 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
							if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 1 ) { // 2007.04.26
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1001 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
								//----------------------------------------------------------------------
								//============================================================================================
								// 2007.01.22
								//============================================================================================
								if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
									while( TRUE ) {
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP WHILE 1 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
										if ( !SCHEDULER_CONTROL_BM_PROCESS_MONITOR_Run_Sts_for_STYLE_4( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) break;
										if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
											return 0;
										}
										Sleep(1);
									}
								}
								//============================================================================================
								if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_MOVE_LOW2HIGH( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) {
									return 0;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1002 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
							//=============================================================================================================
//								BM_AlingTag = 0;
							//=============================================================================================================
//								if ( ( FM_Slot <= 0 ) || ( FM_Slot2 <= 0 ) ) {
//									if ( !osp ) {
//										if ( FM_Slot2 > 0 ) {
//											if ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , -1 , TRUE , &sp_start ) ) {
//												BM_AlingTag = 1;
//											}
//										}
//									}
//								}
							//=============================================================================================================
							// 2007.12.15
							//=============================================================================================================
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 <= 0 ) && !osp ) {
								switch ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , -1 , FALSE , &sp_start , FALSE , TRUE ) ) {
								case 2 :
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
									return 0;
									break;
								case 1 :
									FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_dbSide2  = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									break;
								default :
									break;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1003 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
							//=============================================================================================================
//								if ( BM_AlingTag != 0 ) { // arm B만 일때
							if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) && !osp ) {
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1004 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
								//----------------------------------------------------------------------
								//
								dal_sldata = SCHEDULER_ALIGN_BUFFER_PLACE_SLOT_CHECK_WITH_ARM_B( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) );
								//
								if ( dal_sldata != 0 ) {
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1005 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , dal_sldata );
									//----------------------------------------------------------------------
//										_SCH_MODULE_Set_FM_POINTER( TA_STATION , 0 ); // 2007.04.19
//										_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 99 ); // 2007.04.19
									//-------------------------------------------------------------------------------------------------
									// 2008.02.11
									//-------------------------------------------------------------------------------------------------
//										_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer2 , FMWFR_PICK_CM_DONE_AL ); // 2008.02.11
//										_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot2 ); // 2007.04.19 // 2008.02.11
									//-------------------------------------------------------------------------------------------------
									dal_s[0] = CHECKING_SIDE;
									dal_w[0] = FM_Slot2;
									dal_p[0] = FM_Pointer2;
									//-------------------------------------------------------------------------------------------------
									while ( TRUE ) {
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1006 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
										//----------------------------------------------------------------------
										// place albm arm? slot dal_sldata
										//----------------------------------------------------------------------
//											if ( SCHEDULER_ALIGN_BUFFER_PLACE_TO_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , 2 , -1 , &FM_dbSide , &FM_Pointer , &FM_Slot , &FM_dbSide2 , &FM_Pointer2 , &FM_Slot2 , dal_sldata , TRUE ) == -1 ) { // 2008.02.11
										if ( SCHEDULER_ALIGN_BUFFER_PLACE_TO_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , 0 , -1 , &FM_dbSide , &FM_Pointer , &FM_Slot , &FM_dbSide2 , &FM_Pointer2 , &FM_Slot2 , dal_sldata , TRUE ) == -1 ) { // 2008.02.11
											return 0;
										}
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1007 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
										// 2008.02.11
										//----------------------------------------------------------------------
										for ( dal_xl = 0 ; dal_xl < 2 ; dal_xl++ ) {
											if ( dal_xl == 0 ) {
												_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , dal_p[0] , FMWFR_PICK_CM_DONE_AL );
												_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , dal_s[0] , dal_w[0] );
												//
												_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 0 );
											}
											else {
												_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , dal_p[0] , FMWFR_PICK_CM_DONE_AL );
												_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , dal_s[0] , dal_w[0] );
												//
												_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , 0 , 0 );
											}
											//----------------------------------------------------------------------
											osp = FALSE;
											switch ( Scheduling_Other_Side_Pick_From_FM_for_4( TRUE , CHECKING_SIDE , FALSE , &sp_start , FALSE , TRUE ) ) {
											case 2 :
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
												return 0;
												break;
											case 1 :
												osp = TRUE;
												break;
											default :
												switch ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , -1 , FALSE , &sp_start , FALSE , TRUE ) ) {
												case 2 :
													_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 15 from FEM%cWHFMFAIL 15\n" , 9 );
													return 0;
													break;
												case 1 :
													osp = TRUE;
													break;
												default :
													osp = FALSE;
													break;
												}
												break;
											}
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1008 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
											//----------------------------------------------------------------------
											if ( osp ) break; // 2008.02.11
											//----------------------------------------------------------------------
										}
										//
										if ( !osp ) break;
										//
										if ( dal_xl == 0 ) { // 2008.02.11
											_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , 0 , 0 ); // 2008.02.11
											dal_sldata = SCHEDULER_ALIGN_BUFFER_PLACE_SLOT_CHECK_WITH_ARM_B( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) );
										}
										else {
											_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 0 ); // 2008.02.11
											dal_sldata = SCHEDULER_ALIGN_BUFFER_PLACE_SLOT_CHECK_WITH_ARM_B( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) );
										}
										//
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1009 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , dal_sldata );
										//----------------------------------------------------------------------
										if ( dal_sldata == 0 ) {
											return 0;
										}
									}
//										_SCH_MODULE_Set_FM_POINTER( TA_STATION , 0 ); // 2007.09.12
									_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , 0 , 0 );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , 0 , 0 ); // 2008.02.11
								}
							}
							//=============================================================================================================
							else {
								if ( SCHEDULER_ALIGN_BUFFER_PLACE_TO_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , 0 , 0 , &FM_dbSide , &FM_Pointer , &FM_Slot , &FM_dbSide2 , &FM_Pointer2 , &FM_Slot2 , 0 , TRUE ) == -1 ) {
									return 0;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1010 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
							//=============================================================================================================
							//=============================================================================================================
							//=============================================================================================================
							//=============================================================================================================
							if ( SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) == 0 ) { // 2007.10.30
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1011 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
								//----------------------------------------------------------------------
								FM_Slot = 0;
								FM_Slot2 = 0;
								Result = -999;
							}
							else {
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1012 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
								//----------------------------------------------------------------------
								if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , TRUE , -1 ) == 1 ) {
									if ( SCHEDULER_ALIGN_BUFFER_PICK_FROM_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , FALSE , &FM_dbSide , &FM_Pointer , &FM_Slot , &FM_dbSide2 , &FM_Pointer2 , &FM_Slot2 , FALSE ) == -1 ) {
										return 0;
									}
								}
								else {
									FM_Slot = 0;
									FM_Slot2 = 0;
									Result = -999;
								}
							}
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1013 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
							//============================================================================================
							if ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , FALSE , -1 ) == 1 ) {
								if ( ( SCHEDULER_ALIGN_BUFFER_CONTROL_HIGHER_INDEX( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) == 0 ) || ( SCHEDULER_ALIGN_BUFFER_CONTROL_WAFER_EXIST( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , TRUE , -1 ) != 1 ) ) { // 2007.10.30
									if ( SCHEDULER_ALIGN_BUFFER_GET_RUN() == 1 ) { // 2007.04.26
										return 0;
									}
									else {
										_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , -1 , TRUE , 0 , 1001 );
										SCHEDULER_ALIGN_BUFFER_SET_RUN( 1 ); // 2007.04.26
									}
								}
							}
							//============================================================================================
							//----------------------------------------------------------------------
							_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 1014 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
							//----------------------------------------------------------------------
						}
					}
				}
				//==================================================================================================
				//----------------------------------------------------------------------
				//----------------------------------------------------------------------
				//
				// Code Aligning Before Place to BI
				//
				//----------------------------------------------------------------------
				if ( !SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( FALSE ) ) {
//					if ( _SCH_MODULE_Need_Do_Multi_FAL() && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.06.24
					if ( _SCH_MODULE_Need_Do_Multi_FAL() && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.06.24
						//======================================================================
						FM_dbSide = CHECKING_SIDE; // 2006.10.14
						FM_dbSide2 = CHECKING_SIDE; // 2006.10.14
						//==================================================================================================
						// 2006.10.14
						//==================================================================================================
						if ( ( FM_Slot <= 0 ) || ( FM_Slot2 <= 0 ) ) {
							if (
								( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) &&
								( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) &&
								( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) &&
								( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) &&
								( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL )
								) {
								if ( FM_Slot <= 0 ) {
									FM_dbSide  = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
									FM_Slot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								}
								else {
									FM_dbSide2  = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
									FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
								}
							}
						}
						//======================================================================
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
							if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) != 2 ) { // 2007.03.15
								//====================================================================================================================
								// 2006.02.14
								//====================================================================================================================
								if ( FM_Buffer < BM1 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
								}
								else {
									FM_Bufferx = FM_Buffer;
								}
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								//====================================================================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSide , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2006.02.14
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
								//========================================================================================
								//============================================================================
	//							_SCH_MACRO_SPAWN_FM_MALIGNING( CHECKING_SIDE , FM_Slot , 0 , FM_CM ); // 2006.02.08
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSide , FM_Slot , 0 , FM_CM , FM_Bufferx ); // 2006.02.08
								//============================================================================
								if ( _SCH_MACRO_CHECK_FM_MALIGNING( FM_dbSide , TRUE ) != SYS_SUCCESS ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
									return 0;
								}
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								//----------------------------------
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSide , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
								//========================================================================================
								//===============================================================================================================
								// 2005.09.06
								//===============================================================================================================
								_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );
								//===============================================================================================================
								//===============================================================================================================
								// 2006.11.10
								//===============================================================================================================
//									if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
								//===============================================================================================================
								//====================================================================================================================
								// 2006.02.14
								//====================================================================================================================
								if ( FM_Buffer < BM1 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
								}
								else {
									FM_Bufferx = FM_Buffer;
								}
								//====================================================================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2006.02.14
									_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
								//========================================================================================
	//							_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
								//
								_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( FM_dbSide2 , FM_Pointer2 , -1 );
								_SCH_MODULE_Set_MFAL_WAFER( FM_Slot2 );
								//
								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , 0 ); // 2006.09.21
								//============================================================================
								// Aligning
								//============================================================================
	//							_SCH_MACRO_SPAWN_FM_MALIGNING( CHECKING_SIDE , 0 , FM_Slot2 , FM_CM ); // 2006.02.08
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSide2 , 0 , FM_Slot2 , FM_CM , FM_Bufferx ); // 2006.02.08
								//============================================================================
							}
							else { // 2007.03.15
								if ( FM_Buffer < BM1 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
								}
								else {
									FM_Bufferx = FM_Buffer;
								}
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								//====================================================================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) { // 2006.02.14
									_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
								//============================================================================
								// Aligning
								//============================================================================
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSide2 , 0 , FM_Slot2 , FM_CM , FM_Bufferx );
								//============================================================================
								if ( _SCH_MACRO_CHECK_FM_MALIGNING( FM_dbSide2 , TRUE ) != SYS_SUCCESS ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
									return 0;
								}
								//----------------------------------
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot2 , 9 , FM_Slot2 , 9 , FM_Slot2 );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 );
								//========================================================================================
								_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );
								//===============================================================================================================
//									if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
								//===============================================================================================================
								if ( FM_Buffer < BM1 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
								}
								else {
									FM_Bufferx = FM_Buffer;
								}
								//====================================================================================================================
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_dbSide , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) { // 2006.02.14
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 );
								//============================================================================
								_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( FM_dbSide , FM_Pointer , -1 );
								_SCH_MODULE_Set_MFAL_WAFER( FM_Slot );
								//
								_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , 0 );
								//========================================================================================
								_SCH_MACRO_SPAWN_FM_MALIGNING( FM_dbSide , FM_Slot , 0 , FM_CM , FM_Bufferx ); // 2006.02.08
								//====================================================================================================================
							}
						}
						else {
							//============================================================================================================
							// 2005.09.06
							//============================================================================================================
							if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								_SCH_LOG_LOT_PRINTF( _SCH_MODULE_Get_MFAL_SIDE() , "FM Handling Success at AL(%c:%d)%cWHFMALSUCCESS %c:%d%c%d\n" , ( FM_Slot > 0 ) ? 'B' : 'A' , _SCH_MODULE_Get_MFAL_WAFER() , 9 , ( FM_Slot > 0 ) ? 'B' : 'A' , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , _SCH_MODULE_Get_MFAL_SIDE() , AL , ( FM_Slot > 0 ) ? 0 : _SCH_MODULE_Get_MFAL_WAFER() , ( FM_Slot > 0 ) ? _SCH_MODULE_Get_MFAL_WAFER() : 0 , _SCH_CLUSTER_Get_PathIn( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() ) , -1 , TRUE , ( FM_Slot > 0 ) ? 0 : _SCH_MODULE_Get_MFAL_WAFER() , ( FM_Slot > 0 ) ? _SCH_MODULE_Get_MFAL_WAFER() : 0 , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , _SCH_MODULE_Get_MFAL_POINTER() ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( _SCH_MODULE_Get_MFAL_SIDE() , "FM Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
									return 0;
								}
								//========================================================================================
								_SCH_MODULE_Set_FM_POINTER_MODE( ( FM_Slot > 0 ) ? TB_STATION : TA_STATION , _SCH_MODULE_Get_MFAL_POINTER() , FMWFR_PICK_CM_DONE_AL );
								_SCH_MODULE_Set_FM_SIDE_WAFER( ( FM_Slot > 0 ) ? TB_STATION : TA_STATION , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_WAFER() );
								//========================================================================================
								_SCH_MODULE_Set_MFAL_WAFER( 0 );
								//========================================================================================
								// 2006.11.10
								//===============================================================================================================
//									if ( _SCH_MODULE_Get_FM_MODE( ( FM_Slot > 0 ) ? TA_STATION : TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( ( FM_Slot > 0 ) ? TA_STATION : TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
								//===============================================================================================================
							}
							//============================================================================================================
							//====================================================================================================================
							// 2006.02.14
							//====================================================================================================================
							if ( FM_Buffer < BM1 ) {
								if ( FM_Slot > 0 ) {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , CHECKING_SIDE , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
								}
								else {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , CHECKING_SIDE , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
								}
							}
							else {
								FM_Bufferx = FM_Buffer;
							}
							//-----------------------------------------------
							FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
							//-----------------------------------------------
							//====================================================================================================================
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2006.02.14
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
								return 0;
							}
							//========================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
							//========================================================================================
							if ( FM_Slot > 0 ) {
//								_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
								//
								_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( CHECKING_SIDE , FM_Pointer , -1 );
								_SCH_MODULE_Set_MFAL_WAFER( FM_Slot );
								//
								_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
								//============================================================================
								// Aligning
								//============================================================================
//								_SCH_MACRO_SPAWN_FM_MALIGNING( CHECKING_SIDE , FM_Slot , 0 , FM_CM ); // 2006.02.08
								_SCH_MACRO_SPAWN_FM_MALIGNING( CHECKING_SIDE , FM_Slot , 0 , FM_CM , FM_Bufferx ); // 2006.02.08
								//============================================================================
							}
							else if ( FM_Slot2 > 0 ) {
//								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
								//
								_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( CHECKING_SIDE , FM_Pointer2 , -1 );
								_SCH_MODULE_Set_MFAL_WAFER( FM_Slot2 );
								//
								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , 0 ); // 2006.09.21
								//============================================================================
								// Aligning
								//============================================================================
//								_SCH_MACRO_SPAWN_FM_MALIGNING( CHECKING_SIDE , 0 , FM_Slot2 , FM_CM ); // 2006.02.08
								_SCH_MACRO_SPAWN_FM_MALIGNING( CHECKING_SIDE , 0 , FM_Slot2 , FM_CM , FM_Bufferx ); // 2006.02.08
								//============================================================================
							}
							//================================================================================
							//================================================================================
							//================================================================================
						}
						Result = -1;
					}
					else {
						//======================================================================
						FM_dbSide = CHECKING_SIDE; // 2006.10.31
						FM_dbSide2 = CHECKING_SIDE; // 2006.10.31
						//==================================================================================================
						//====================================================================================================================
						// 2006.02.14
						//====================================================================================================================
						if ( FM_Buffer < BM1 ) {
							if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , CHECKING_SIDE , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) {
									FM_Bufferx = -1;
								}
								else {
									if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , CHECKING_SIDE , FM_Pointer2 , FALSE , TRUE , &FM_TSlotx ) != 1 ) {
										FM_Bufferx = -1;
									}
									else {
										FM_Bufferx = FM_Bufferx + ( FM_TSlotx * 10000 );
									}
								}
							}
							else if ( FM_Slot > 0 ) {
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , CHECKING_SIDE , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
							}
							else if ( FM_Slot2 > 0 ) {
								if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , CHECKING_SIDE , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
							}
							else {
								FM_Bufferx = -1;
							}
						}
						else {
							FM_Bufferx = FM_Buffer;
						}
						//-----------------------------------------------
						FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
						//-----------------------------------------------
						//====================================================================================================================
						if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , FM_Slot , FM_Slot2 , FM_dbSide , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2006.02.14
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
							return 0;
						}
						//========================================================================================
						SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
						//========================================================================================
						//===============================================================================================================
						// 2005.09.06
						//===============================================================================================================
						if ( FM_Slot > 0 ) {
							_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );
							//===============================================================================================================
							// 2006.11.10
							//===============================================================================================================
//								if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
							//===============================================================================================================
						}
						if ( FM_Slot2 > 0 ) {
							_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );
							//===============================================================================================================
							// 2006.11.10
							//===============================================================================================================
//								if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
							//===============================================================================================================
						}
						//===============================================================================================================
						//====================================================================================================================
						// 2006.10.31
						//====================================================================================================================
						if ( osp ) {
							if      ( ( FM_Slot  <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) {
								FM_dbSide  = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
								if ( CHECKING_SIDE != FM_dbSide ) {
									FM_Slot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
									//
									if ( FM_Buffer < BM1 ) {
										if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
									}
									else {
										FM_Bufferx = FM_Buffer;
									}
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide , AL , FM_Slot , 0 , FM_CM , FM_Bufferx , TRUE , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer , 0 ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , 0 , 9 , FM_Slot , 0 , 9 , FM_Slot );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
									//========================================================================================
									_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL );
									//===============================================================================================================
									// 2006.11.10
									//===============================================================================================================
//										if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
									//===============================================================================================================
								}
							}
							else if ( ( FM_Slot2 <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) {
								FM_dbSide2  = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
								if ( CHECKING_SIDE != FM_dbSide2 ) {
									FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									//
									if ( FM_Buffer < BM1 ) {
										if ( _SCH_ONESELECT_Get_First_One_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 , FALSE , TRUE , &FM_Bufferx ) != 1 ) FM_Bufferx = -1;
									}
									else {
										FM_Bufferx = FM_Buffer;
									}
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE_MDL_PICK , FM_dbSide2 , AL , 0 , FM_Slot2 , FM_CM , FM_Bufferx , TRUE , 0 , FM_Slot2 , 0 , FM_dbSide2 , 0 , FM_Pointer2 ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , 0 , FM_Slot2 , 9 , 0 , FM_Slot2 , 9 , FM_Slot2 * 100 );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
									//========================================================================================
									_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL );
									//===============================================================================================================
									// 2006.11.10
									//===============================================================================================================
//										if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
									//===============================================================================================================
								}
							}
						}
						//====================================================================================================================
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 21 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
			//----------------------------------------------------------------------
			if ( Result < 0 ) { // 2007.01.23
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.08
					if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
						if ( !FMALBMPickReject ) { // 2007.04.27
							if ( !SCHEDULER_CONTROL_Chk_PICK_BM_OUT_3MODE_BUT_RETURN_FOR_4( SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , &CHECKING_SIDE , &dal_xl , &dal_xl , &dal_xl ) ) { // 2007.05.10
								if ( !Scheduling_Check_Waiting_for_BM_Action( 1 ) ) {
									Result = SCHEDULER_ALIGN_BUFFER_PICK_FROM_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , TRUE , &FM_dbSide , &FM_Pointer , &FM_Slot , &FM_dbSide2 , &FM_Pointer2 , &FM_Slot2 , TRUE );
									//
									if ( Result == -1 ) {
										return 0;
									}
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 21b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
									//----------------------------------------------------------------------
									if ( Result != 999 ) Result = -100;
									//----------------------------------------------------------------------
								}
								else { // 2007.04.26
									if ( Scheduling_Check_Possible_for_BM_Place( FALSE ) ) { // 2007.04.26
										Result = SCHEDULER_ALIGN_BUFFER_PICK_FROM_ALBF( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() , CHECKING_SIDE , TRUE , &FM_dbSide , &FM_Pointer , &FM_Slot , &FM_dbSide2 , &FM_Pointer2 , &FM_Slot2 , TRUE );
										//
										if ( Result == -1 ) {
											return 0;
										}
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 21c RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
										if ( Result != 999 ) Result = -100;
										//----------------------------------------------------------------------
									}
								}
							}
							//==================================================================================
						}
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 21d RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
			//----------------------------------------------------------------------
			//==============================================================================================================================================
			//==============================================================================================================================================
			// Place BM
			//==============================================================================================================================================
			//==============================================================================================================================================
			if ( Result < 0 ) { // 2004.06.25
//					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.06.24 // 2007.01.06
				if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.11.05 // 2007.01.06
					//==============================================================
					// 2006.11.10
					//==============================================================
					olddoubleok = doubleok;
					//==============================================================
					// (11) PICK AL with ArmA | PLACE BM with ArmA
					// (31) PICK AL with ArmA
					if      ( ( doubleok == 11 ) || ( doubleok == 31 ) ) { // 2004.08.17
						//===========================================================================================================================
						// 2006.11.15
						//===========================================================================================================================
						if ( !SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TA_STATION , TRUE ) ) {
							doubleok = 0;
							FMICPickReject = FALSE;
						}
						//===========================================================================================================================
						if ( doubleok != 0 ) { // 2006.11.13
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at AL(A:%d)%cWHFMALSUCCESS A:%d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
							FM_Slot = _SCH_MODULE_Get_MFAL_WAFER();
							FM_Slot2 = 0;
							FM_Pointer = _SCH_MODULE_Get_MFAL_POINTER();
							FM_Pointer2 = 0;
							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer );
							//-----------------------------------------------
							FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
							//-----------------------------------------------
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_SIDE() , FM_Pointer , FM_Pointer ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
								return 0;
							}
							//========================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
							//========================================================================================
							_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , FMWFR_PICK_CM_DONE_AL ); // 2005.09.06
							_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot );
							//========================================================================================
							_SCH_MODULE_Set_MFAL_WAFER( 0 );
							//========================================================================================
							//===============================================================================================================
							// 2006.11.10
							//===============================================================================================================
//								if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
							//===============================================================================================================
							//---------------------------------------------------------------------------------------
							// (2) PLACE BM with ArmA
							// (11) PICK AL with ArmA | PLACE BM with ArmA
//								if ( doubleok == 11 ) doubleok = 2; // 2006.11.16
							//===============================================================================================================
							// 2006.11.17
							//===============================================================================================================
//								doubleok = 2; // 2006.11.16 // 2006.11.17
							if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) || ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) {
								if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == _SCH_MODULE_Get_FM_SIDE( TB_STATION ) ) {
									if ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) {
										doubleok = 2;
									}
									else {
										doubleok = 3;
									}
								}
								else {
									doubleok = 3;
								}
							}
							else {
								doubleok = 2;
							}
							//===============================================================================================================
							//---------------------------------------------------------------------------------------
						}
					}
					// (12) PICK AL with ArmB | PLACE BM with ArmB
					// (32) PICK AL with ArmB
					else if ( ( doubleok == 12 ) || ( doubleok == 32 ) ) { // 2004.08.17
						//===========================================================================================================================
						// 2006.11.15
						//===========================================================================================================================
						if ( !SCHEDULING_Possible_TargetSpace_for_STYLE_4( _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_POINTER() , TB_STATION , TRUE ) ) {
							doubleok = 0;
							FMICPickReject = FALSE;
						}
						//===========================================================================================================================
						if ( doubleok != 0 ) { // 2006.11.13
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at AL(B:%d)%cWHFMALSUCCESS B:%d%c%d\n" , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() , 9 , _SCH_MODULE_Get_MFAL_WAFER() );
							FM_Slot = 0;
							FM_Slot2 = _SCH_MODULE_Get_MFAL_WAFER();
							FM_Pointer = 0;
							FM_Pointer2 = _SCH_MODULE_Get_MFAL_POINTER();
//							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer ); // 2006.09.20
							FM_CM = _SCH_CLUSTER_Get_PathIn( CHECKING_SIDE , FM_Pointer2 ); // 2006.09.20
							//-----------------------------------------------
							FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
							//-----------------------------------------------
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , AL , FM_Slot , FM_Slot2 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , _SCH_MODULE_Get_MFAL_SIDE() , _SCH_MODULE_Get_MFAL_SIDE() , FM_Pointer2 , FM_Pointer2 ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at AL(%d:%d)%cWHFMALFAIL %d:%d%c%d\n" , FM_Slot , FM_Slot2 , 9 , FM_Slot , FM_Slot2 , 9 , FM_Slot2 * 100 + FM_Slot );
								return 0;
							}
							//========================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
							//========================================================================================
							_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , FMWFR_PICK_CM_DONE_AL ); // 2005.09.06
							_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 );
							//========================================================================================
							_SCH_MODULE_Set_MFAL_WAFER( 0 );
							//========================================================================================
							//===============================================================================================================
							// 2006.11.10
							//===============================================================================================================
//								if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
							//===============================================================================================================
							//---------------------------------------------------------------------------------------
							// (3) PLACE BM with ArmB
							// (12) PICK AL with ArmB | PLACE BM with ArmB
//								if ( doubleok == 12 ) doubleok = 3; // 2006.11.16
							//===============================================================================================================
							// 2006.11.17
							//===============================================================================================================
//								doubleok = 3; // 2006.11.16 // 2006.11.17
							if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) || ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) ) ) {
								if ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) == _SCH_MODULE_Get_FM_SIDE( TB_STATION ) ) {
									if ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) {
										doubleok = 2;
									}
									else {
										doubleok = 3;
									}
								}
								else {
									doubleok = 2;
								}
							}
							else {
								doubleok = 3;
							}
							//===============================================================================================================
							//---------------------------------------------------------------------------------------
						}
					}
					//=======================================================================================================================================
					//=======================================================================================================================================
					//=======================================================================================================================================
					if      ( doubleok == 2 ) { // (2) PLACE BM with ArmA
						FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
						FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );

						FM_Slot2 = 0;
						FM_Pointer2 = 0;

//							if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2005.10.06 // 2006.01.06
						if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2005.10.06 // 2006.01.06
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 ); // 2005.12.22
						}
						else {
//							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
//							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09 // 2017.04.27
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SCHEDULING_Possible_Pre_BM_4( CHECKING_SIDE , SignalBM ) , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09 // 2017.04.27
						}
						//=====================================================================================================================
						// 2005.12.21
						//=====================================================================================================================
						if ( Result > 0 ) {
							if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
								if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) < 0 ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer - 1 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , TRUE , 0 );
									if ( ( Result > 0 ) && ( ( FM_Buffer - 1 ) == FM_Bufferx ) ) {
										FM_Buffer = FM_Bufferx;
										FM_TSlot = FM_TSlotx;
										FM_TSlot2 = FM_TSlot2x;
									}
									else {
										Result = 2;
									}
								}
							}
						}
						//=====================================================================================================================
						// 2006.11.10
						//=====================================================================================================================
						if ( ( Result < 0 ) && ( doubleok != 0 ) ) {
							if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
								if ( ( olddoubleok == 2 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , -1 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &mvbuffer , &mvslot1 , &mvslot2 , FALSE , 0 );
									if ( Result > 0 ) {
										SignalMove = 1;
										if ( SignalCount != 0 ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Start %s(%d,%d)%cWHFMMOVESTART BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( mvbuffer ) , mvslot1 , mvslot2 , 9 , mvbuffer - BM1 + 1 , mvslot1 , mvslot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );
											//-----------------------------------------------
											FM_Last_Move_Recv_BM_4 = mvbuffer; // 2010.04.16
											//-----------------------------------------------
											if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , mvbuffer , mvslot1 , mvslot2 ) == SYS_ABORTED ) {
												return 0;
											}
											//========================================================================================
											SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
											//========================================================================================
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Success %s(%d,%d)%cWHFMMOVESUCCESS BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( mvbuffer ) , mvslot1 , mvslot2 , 9 , mvbuffer - BM1 + 1 , mvslot1 , mvslot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );
											//========================================================================================
											_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL_GOBM );
											//========================================================================================
//												if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
										}
										Result = -2002;
									}
								}
							}
						}
						//=====================================================================================================================
					}
					else if ( doubleok == 3 ) { // (3) PLACE BM with ArmB
						FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
						FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );

						FM_Slot = 0;
						FM_Pointer = 0;

//							if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2005.10.06 // 2006.01.06
						if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) ) { // 2005.10.06 // 2006.01.06
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 );
						}
						else {
//							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
//							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09 // 2017.04.27
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SCHEDULING_Possible_Pre_BM_4( CHECKING_SIDE , SignalBM ) , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09 // 2017.04.27
						}
						//=====================================================================================================================
						// 2005.12.21
						//=====================================================================================================================
						if ( Result > 0 ) {
							if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) ) {
								if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) < 0 ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer - 1 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , TRUE , 0 );
									if ( ( Result > 0 ) && ( ( FM_Buffer - 1 ) == FM_Bufferx ) ) {
										FM_Buffer = FM_Bufferx;
										FM_TSlot = FM_TSlotx;
										FM_TSlot2 = FM_TSlot2x;
									}
									else {
										Result = 3;
									}
								}
							}
						}
						//=====================================================================================================================
						// 2006.11.10
						//=====================================================================================================================
						if ( ( Result < 0 ) && ( doubleok != 0 ) ) {
							if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
								if ( ( olddoubleok == 3 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , -1 , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &mvbuffer , &mvslot1 , &mvslot2 , FALSE , 0 );
									if ( Result > 0 ) {
										SignalMove = 1;
										if ( SignalCount != 0 ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Start %s(%d,%d)%cWHFMMOVESTART BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( mvbuffer ) , mvslot1 , mvslot2 , 9 , mvbuffer - BM1 + 1 , mvslot1 , mvslot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) );
											//-----------------------------------------------
											FM_Last_Move_Recv_BM_4 = mvbuffer; // 2010.04.16
											//-----------------------------------------------
											if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , mvbuffer , mvslot1 , mvslot2 ) == SYS_ABORTED ) {
												return 0;
											}
											//========================================================================================
											SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
											//========================================================================================
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Success %s(%d,%d)%cWHFMMOVESUCCESS BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( mvbuffer ) , mvslot1 , mvslot2 , 9 , mvbuffer - BM1 + 1 , mvslot1 , mvslot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) );
											//========================================================================================
											_SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL_GOBM );
											//========================================================================================
//												if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
										}
										Result = -2003;
									}
								}
							}
						}
						//=====================================================================================================================
					}
					else if ( doubleok == 4 ) { // (4) PLACE BM with ArmAB
//							if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2005.09.30 // 2006.01.06
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) { // 2005.09.30 // 2006.01.06
							if ( ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) || ( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) != _SCH_MODULE_Get_FM_SIDE( TB_STATION ) ) ) {
								FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
								FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );

								FM_Slot2 = 0;
								FM_Pointer2 = 0;

								Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 );

								if ( Result < 0 ) {
									if ( doubleok4isdouble == 2 ) {
										FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );

										FM_Slot = 0;
										FM_Pointer = 0;

										Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide2 , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 );
									}
								}
								else {
									if ( doubleok4isdouble == 3 ) {
										if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) { // 2006.09.13
											if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
												if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2006.01.09
													Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , TRUE , 0 );
													//
													if ( Result >= 0 ) {
														if ( ( FM_Buffer + 1 ) != FM_Bufferx ) {
															doubleok4isdouble = 1;
														}
														else {
															FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
															FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
															FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
															FM_TSlot2 = FM_TSlot2x;
															//
															doubleok4isdouble = 3;
														}
													}
												}
												else { // 2006.01.09
													doubleok4isdouble = 1;
												}
											}
											else { // 2005.10.05
												Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , FM_Buffer - 1 , FM_Slot , 0 , FM_Pointer , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlotxx , TRUE , 0 );
												//
												if ( Result >= 0 ) {
													if ( ( FM_Buffer - 1 ) == FM_Bufferx ) {
														if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2006.01.09
															Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , TRUE , 0 );
															if ( Result >= 0 ) {
																if ( FM_Buffer == FM_Bufferx ) {
																	FM_Buffer = FM_Buffer - 1;
																	//
																	FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
																	FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
																	FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
																	FM_TSlot = FM_TSlotx;
																	//
																	FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
																	FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
																	FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
																	FM_TSlot2 = FM_TSlot2x;
																	//
																	//
																	doubleok4isdouble = 3;
																}
																else {
																	doubleok4isdouble = 1;
																}
															}
														}
														else { // 2006.01.09
															FM_Buffer = FM_Buffer - 1;
															doubleok4isdouble = 1;
														}
													}
													else {
														doubleok4isdouble = 1;
													}
												}
											}
										}
										else {
											Result = -1; // 2006.09.13
										}
									}
									else if ( doubleok4isdouble == 1 ) { // A
										if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) ) { // 2006.09.13
											if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
												Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , FALSE , 0 );
												if ( Result > 0 ) {
													if ( ( FM_Buffer + 1 ) == FM_Bufferx ) {
														Result = -1;
													}
													else {
													}
												}
												else {
													Result = 1;
												}
											}
											else {
												Result = SCHEDULING_Possible_BM_for_STYLE_4( FM_dbSide , FM_Buffer - 1 , FM_Slot , 0 , FM_Pointer , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlotxx , FALSE , 0 );
												//
												if ( Result >= 0 ) {
													if ( ( FM_Buffer - 1 ) == FM_Bufferx ) {
														Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer , 0 , _SCH_MODULE_Get_FM_WAFER( TB_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TB_STATION ) , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , FALSE , 0 );
														if ( Result >= 0 ) {
															Result = -1;
														}
														else {
															Result = 1;
														}
													}
													else {
														Result = 1;
													}
												}
												else {
													Result = 1;
												}
											}
										}
									}
									else if ( doubleok4isdouble == 2 ) { // B
										Result = -1;
									}
								}
							}
							else {
								FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );

								FM_Slot = 0;
								FM_Pointer = 0;

//								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09

								if ( Result < 0 ) {
									FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );

									FM_Slot2 = 0;
									FM_Pointer2 = 0;

//									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09
								}
							}
						}
						else {
							if ( _SCH_MODULE_Get_FM_POINTER( TA_STATION ) < _SCH_MODULE_Get_FM_POINTER( TB_STATION ) ) {
								FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );

								FM_Slot2 = 0;
								FM_Pointer2 = 0;

//								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09

								//===================================================================================
								// 2006.11.17
								//===================================================================================
								if ( Result >= 0 ) {
									if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( FM_Buffer ) == 2 ) Result = -100;
								}
								//===================================================================================
								if ( Result < 0 ) {
									FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );

									FM_Slot = 0;
									FM_Pointer = 0;

//									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09
									//===================================================================================
									// 2006.11.17
									//===================================================================================
									if ( Result >= 0 ) {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( FM_Buffer ) == 1 ) Result = -100;
									}
									//===================================================================================
								}
							}
							else {
								FM_Slot2 = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
								FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );

								FM_Slot = 0;
								FM_Pointer = 0;

//								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
								Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09

								//===================================================================================
								// 2006.11.17
								//===================================================================================
								if ( Result >= 0 ) {
									if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( FM_Buffer ) == 1 ) Result = -100;
								}
								//===================================================================================
								if ( Result < 0 ) {
									FM_Slot = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
									FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );

									FM_Slot2 = 0;
									FM_Pointer2 = 0;

//									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09
									//===================================================================================
									// 2006.11.17
									//===================================================================================
									if ( Result >= 0 ) {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( FM_Buffer ) == 2 ) Result = -100;
									}
									//===================================================================================
								}
							}
						}
						//=====================================================================================================================
						// 2006.11.10
						//=====================================================================================================================
						if ( ( Result < 0 ) && ( doubleok != 0 ) ) {
							if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
								if ( ( olddoubleok == 4 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
									Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , -1 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) , 0 , _SCH_MODULE_Get_FM_POINTER( TA_STATION ) , 0 , &mvbuffer , &mvslot1 , &mvslot2 , FALSE , 0 );
									if ( Result > 0 ) {
										SignalMove = 1;
										if ( SignalCount != 0 ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Start %s(%d,%d)%cWHFMMOVESTART BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( mvbuffer ) , mvslot1 , mvslot2 , 9 , mvbuffer - BM1 + 1 , mvslot1 , mvslot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );
											//-----------------------------------------------
											FM_Last_Move_Recv_BM_4 = mvbuffer; // 2010.04.16
											//-----------------------------------------------
											if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , mvbuffer , mvslot1 , mvslot2 ) == SYS_ABORTED ) {
												return 0;
											}
											//========================================================================================
											SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
											//========================================================================================
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Success %s(%d,%d)%cWHFMMOVESUCCESS BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( mvbuffer ) , mvslot1 , mvslot2 , 9 , mvbuffer - BM1 + 1 , mvslot1 , mvslot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) );
											//========================================================================================
											_SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL_GOBM );
											//========================================================================================
//												if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
										}
										Result = -2004;
									}
								}
							}
						}
						//=====================================================================================================================
					}
					else {
						Result = -1;
					}
				}
			}
			else {
//					if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) && ( doubleok == 1 ) ) { // 2005.10.05 // 2006.01.06
				if ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 0 ) && ( doubleok == 1 ) ) { // 2005.10.05 // 2006.01.06
					//============================================================================================
					Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , 0 , FM_Pointer , 0 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 );
					if ( Result > 0 ) {
						if ( SCHEDULER_CONTROL_Get_ONEBODY_nch_is_MainMdl_Style_4( FM_Buffer ) > 0 ) {
							if ( FM_Slot2 <= 0 ) { // 2006.01.06
								Result = 11;
								doubleok4isdouble = 0;
								FM_Slot2 = 0;
							}
							else {
//									if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( CHECKING_SIDE , FM_Pointer , CHECKING_SIDE , FM_Pointer2 ) ) { // 2006.09.13 // 2006.11.03
								if ( _SCH_SUB_Check_ClusterIndex_CPJob_Same( CHECKING_SIDE , FM_Pointer , _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Pointer2 ) ) { // 2006.09.13 // 2006.11.03
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2006.01.09
//											Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer + 1 , 0 , FM_Slot2 , 0 , FM_Pointer2 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2 , TRUE , 0 ); // 2006.11.02
										Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , FM_Slot2 , 0 , FM_Pointer2 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2 , TRUE , 0 ); // 2006.11.02
										//
										if ( Result >= 0 ) {
											if ( ( FM_Buffer + 1 ) != FM_Bufferx ) {
												Result = 12;
												doubleok4isdouble = 0;
												FM_Slot2 = 0;
											}
											else {
//													FM_dbSide = CHECKING_SIDE; // 2006.11.02
//													FM_dbSide2 = CHECKING_SIDE; // 2006.11.02
												FM_dbSide = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2006.11.02
												FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2006.11.02
												//
												doubleok4isdouble = 3;
											}
										}
										else {
//												Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer + 1 , 0 , FM_Slot2 , 0 , FM_Pointer2 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 ); // 2006.11.02
											Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer + 1 , 0 , FM_Slot2 , 0 , FM_Pointer2 , &FM_Bufferx , &FM_TSlotx , &FM_TSlot2x , FALSE , 0 ); // 2006.11.02
											if ( Result > 0 ) {
												if ( ( FM_Buffer + 1 ) != FM_Bufferx ) {
													Result = 13;
													doubleok4isdouble = 0;
													FM_Slot2 = 0;
												}
												else {
													Result = -401;
												}
											}
											else {
												Result = -402;
											}
										}
									}
									else { // 2006.01.09
										Result = 14;
										doubleok4isdouble = 0;
										FM_Slot2 = 0;
									}
								}
								else { // 2006.09.13
									Result = 15;
									doubleok4isdouble = 0;
									FM_Slot2 = 0;
								}
							}
						}
						else {
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer - 1 , FM_Slot , 0 , FM_Pointer , 0 , &FM_Bufferx , &FM_TSlotx , &FM_TSlotxx , FALSE , 0 );
							//
							if ( Result >= 0 ) {
								if ( ( FM_Buffer - 1 ) == FM_Bufferx ) {
									if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2006.01.09
										if ( FM_Slot2 <= 0 ) { // 2006.01.06
											FM_Buffer = FM_Buffer - 1;
											Result = 21;
											doubleok4isdouble = 0;
											FM_Slot2 = 0;
										}
										else {
//												Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer , 0 , FM_Slot2 , 0 , FM_Pointer2 , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , FALSE , 0 ); // 2006.11.02
											Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , FM_Buffer , 0 , FM_Slot2 , 0 , FM_Pointer2 , &FM_Bufferx , &FM_TSlotxx , &FM_TSlot2x , FALSE , 0 ); // 2006.11.02
											if ( Result >= 0 ) {
												Result = -403;
											}
											else {
												Result = 22;
												doubleok4isdouble = 0;
												FM_Slot2 = 0;
											}
										}
									}
									else { // 2006.01.09
										Result = 23;
										FM_Buffer = FM_Buffer - 1;
										doubleok4isdouble = 0;
										FM_Slot2 = 0;
									}
								}
								else {
									Result = 24;
									doubleok4isdouble = 0;
									FM_Slot2 = 0;
								}
							}
							else {
								Result = 25;
								doubleok4isdouble = 0;
								FM_Slot2 = 0;
							}
						}
					}
				}
				else {
					if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) { // 2007.01.06
						if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2015.01.23
							FM_Slot2 = 0;
						}
					}
//						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.06.24 // 2007.01.06
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.06.24 // 2007.01.06
//						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 ); // 2005.09.09
						Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() / 2 ) != 1 , 0 ); // 2005.09.09
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
			//----------------------------------------------------------------------
			if ( Result >= 0 ) {
//printf( "=================================================================\n" );
//printf( "=================================================================\n" );
//printf( "[Result = %d][doubleok = %d][doubleok4isdouble = %d]\n" , Result , doubleok , doubleok4isdouble );
//printf( "=================================================================\n" );
//printf( "=================================================================\n" );
//					if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2004.06.24
				if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) ) { // 2004.06.24 // 2007.01.06
					//=================================================================================
					FMICPickReject = TRUE; // 2006.12.18
					//=================================================================================
					while ( TRUE ) {
						//=================================================================================
						SignalBM = FM_Buffer; // 2005.12.20
						//=================================================================================
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2006.12.15
// 								SignalBM = BM1; // 2007.03.29
							if ( FM_Slot > 0 ) {
								if ( FM_Slot2 <= 0 ) {
									//
									FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
									FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
									//
								}
								//===
								if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) SignalBM = -1; // 2007.03.29
								else									    SignalBM = BM1; // 2007.03.29
								//===
								Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
								//===
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
								//----------------------------------------------------------------------
								if ( Result < 0 ) {
									//===
									Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TA_STATION ) , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 1 );
									//===
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22c RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
									//----------------------------------------------------------------------
									//==================================================================================================================================================================
									// 2007.05.11
									//==================================================================================================================================================================
									if ( Result < 0 ) {
										if ( !SCHEDULING_OneBody3_Return_Check_for_STYLE_4( &FM_Slot , &FM_Pointer , &FM_Slot2 ) ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1101 from FEM%cWHFMFAIL 1101\n" , 9 );
											return 0;
										}
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22d RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
									}
									//==================================================================================================================================================================
								}
							}
							else {
								//
								FM_Slot    = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
								FM_Pointer = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
								//
								//===
								if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) SignalBM = -1; // 2007.03.29
								else									    SignalBM = BM1; // 2007.03.29
								//===
								Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
								//----------------------------------------------------------------------
								_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22e RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
								//----------------------------------------------------------------------
								if ( Result < 0 ) {
									//===
									Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 1 );
									//===
									//----------------------------------------------------------------------
									_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22f RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
									//----------------------------------------------------------------------
									//==================================================================================================================================================================
									// 2007.05.11
									//==================================================================================================================================================================
									if ( Result < 0 ) {
										if ( !SCHEDULING_OneBody3_Return_Check_for_STYLE_4( &FM_Slot , &FM_Pointer , &FM_Slot2 ) ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1102 from FEM%cWHFMFAIL 1102\n" , 9 );
											return 0;
										}
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 22g RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
									}
									//==================================================================================================================================================================
								}
							}
							if ( Result >= 0 ) {
//									if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) { // 2007.03.29 // 2007.04.09
								if ( FM_Slot > 0 ) { // 2007.03.29 // 2007.04.09
									if ( FM_Buffer == BM1 ) break;
								}
								else { // 2007.03.29
									break; // 2007.03.29
								}
							}
							else {
								//==================================================================================
								if ( ( FM_Slot <= 0 ) && ( FM_Slot2 <= 0 ) ) break; // 2007.04.24
								//==================================================================================
							}
						}
						else {
							Result = SCHEDULING_Possible_BM_for_STYLE_4( CHECKING_SIDE , SignalBM , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , FALSE , 0 );
							if ( Result >= 0 ) {
								break;
							}
						}
						Sleep(1);
						if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 18 from FEM%cWHFMFAIL 18\n" , 9 );
							return 0;
						}
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 23 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
				//----------------------------------------------------------------------
				//==========================================================================================
				if ( Result >= 0 ) {
//						if ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() != 1 ) { // 2005.01.24
					if ( ( _SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() % 2 ) == 0 ) { // 2005.01.24
						if ( FM_Pointer < 100 ) SCHEDULER_Set_FM_WILL_GO_BM( CHECKING_SIDE , FM_Buffer ); // 2003.06.20
						//-----------------------------------------------------------------------------------------------------------
						// 2006.12.15 // 2007.04.30
						//-----------------------------------------------------------------------------------------------------------
						k = SCHEDULER_CONTROL_Chk_Result_for_Place_to_BM( CHECKING_SIDE , FM_Slot , FM_Slot2 , FM_Buffer );
						//-----------------------------------------------------------------------------------------------------------
						if ( k != SYS_SUCCESS ) {
							//=====================================================================================================================
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling %s Wait FMSIDE(%d,%d)%cWHFMWAITFMSIDE BM%d:1\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_Slot , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 );
							//=====================================================================================================================
							if ( _SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) {
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = FM_Buffer; // 2010.04.16
								//-----------------------------------------------
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Start %s(%d,%d)%cWHFMMOVESTART BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_Slot , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 , FM_Slot , FM_Slot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ? _SCH_MODULE_Get_FM_WAFER( TA_STATION ) : _SCH_MODULE_Get_FM_WAFER( TB_STATION ) );
//								if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , FM_Slot , FM_Slot2 ) == SYS_ABORTED ) { // 2010.04.23
								if ( _SCH_MACRO_FM_MOVE_OPERATION( 0 , CHECKING_SIDE , 1 , FM_Buffer , FM_TSlot , FM_TSlot2 ) == SYS_ABORTED ) { // 2010.04.23
									return 0;
								}
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Move Success %s(%d,%d)%cWHFMMOVESUCCESS BM%d:%d,%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_Slot , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 , FM_Slot , FM_Slot2 , 9 , _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ? _SCH_MODULE_Get_FM_WAFER( TA_STATION ) : _SCH_MODULE_Get_FM_WAFER( TB_STATION ) );
							}
							//=====================================================================================================================
							do {
								if ( ( k == SYS_ABORTED ) || ( k == SYS_ERROR ) ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling %s Wait FMSIDE Fail%cWHFMWAITFMSIDEFAIL BM%d:1\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
									return 0;
								}
								if ( k == SYS_SUCCESS ) break;
								//
								if ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) break; // 2005.10.05
								//
								Sleep(1);

								//===================================================================================================================
								// 2007.04.27
								//===================================================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
									if ( SCHEDULER_ALIGN_BUFFER_CONTROL_Chk_USE( TRUE ) ) {
										if ( SCHEDULER_ALIGN_BUFFER_CONTROL_MONITORING_ERROR( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) ) { // 2007.04.26
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) , 9 , _SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() - BM1 + 1 );
											return 0;
										}
									}
								}
								//===================================================================================================================
								// 2007.04.04
								//===================================================================================================================
								if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2006.12.15
									if ( ( FM_Slot <= 0 ) && ( FM_Slot2 > 0 ) ) {
										//===
										FM_Bufferx = FM_Buffer;
										//===
										Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , -1 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 );
										//----------------------------------------------------------------------
										_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 23b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
										//----------------------------------------------------------------------
										if ( Result < 0 ) {
											FM_Buffer = FM_Bufferx; // 2008.04.15
											continue;
										}
										else { // 2008.04.15
											//----------------------------------------------------------------------
											SCHEDULER_BM_PROCESS_MONITOR_ONEBODY( _SCH_PRM_GET_DFIM_MAX_PM_COUNT() ); // 2008.04.15
											//----------------------------------------------------------------------
											Result = SCHEDULING_Possible_BM_for_STYLE_4( _SCH_MODULE_Get_FM_SIDE( TB_STATION ) , -1 , FM_Slot , FM_Slot2 , FM_Pointer , FM_Pointer2 , &FM_Buffer , &FM_TSlot , &FM_TSlot2 , TRUE , 0 );
											//----------------------------------------------------------------------
											_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 23b-2 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
											//----------------------------------------------------------------------
											if ( Result < 0 ) {
												FM_Buffer = FM_Bufferx; // 2008.04.15
												continue;
											}
										}
										//----------------------------------------------------------------------
										if ( FM_Bufferx != FM_Buffer ) {
											_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling %s Wait2 FMSIDE(%d,%d)%cWHFMWAIT2FMSIDE BM%d:1\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_Slot , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 );
										}
										//----------------------------------------------------------------------
										//===
									}
								}
								//===================================================================================================================
								// 2007.04.30
								//===================================================================================================================
								k = SCHEDULER_CONTROL_Chk_Result_for_Place_to_BM( CHECKING_SIDE , FM_Slot , FM_Slot2 , FM_Buffer );
								//===================================================================================================================
							}
							while( TRUE );
						}
					}
				}
				//==========================================================================================
				// 2007.05.11
				//==========================================================================================
				if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
					if ( Result >= 0 ) { // 2007.05.15
						for ( dal_xl = 0 ; dal_xl < 2 ; dal_xl++ ) {
							FM_CM = 0;
							if ( FM_Slot > 0 ) {
								for ( k = 0 ; k < SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ; k++ ) {
									if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , FM_TSlot ) > 0 ) {
										FM_CM = 1;
									}
								}
								if ( FM_Slot2 > 0 ) {
									if ( ( ( FM_TSlot2 - 1 ) / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ) != ( ( FM_TSlot - 1 ) / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() ) ) {
										FM_CM = 2;
									}
									else {
										if ( _SCH_MODULE_Get_BM_WAFER( BM1 + k , FM_TSlot ) > 0 ) {
											FM_CM = 3;
										}
									}
								}
							}
							else {
								if ( FM_Slot2 > 0 ) {
									if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_TSlot2 ) > 0 ) {
										FM_CM = 4;
									}
								}
							}
							//=========================================================================================
							if ( FM_CM == 0 ) break;
							//=========================================================================================
							if ( dal_xl == 0 ) {
								if ( !SCHEDULING_OneBody3_Return_Check_for_STYLE_4( &FM_Slot , &FM_Pointer , &FM_Slot2 ) ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1103 from FEM%cWHFMFAIL 1103\n" , 9 );
									return 0;
								}
							}
							else {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort %d from FEM%cWHFMFAIL %d\n" , FM_CM + 2000 , 9 , FM_CM + 2000 );
								return 0;
							}
						}
					}
				}
				else {
					//=========================================================================================
					// 2007.05.15
					//=========================================================================================
					if ( Result >= 0 ) {
						if ( _SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 2 ) {
							FM_CM = 0;
							if ( ( doubleok4isdouble == 3 ) && ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( FM_Buffer , &dal_xl , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( FM_Buffer - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
									FM_CM = 1;
								}
								else {
									if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , dal_xl ) > 0 ) {
										FM_CM = 1;
									}
									else {
										if ( dal_xl != FM_TSlot ) {
											FM_CM = 1;
										}
										else {
											if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( FM_Buffer + 1 , &dal_xl , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( FM_Buffer - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
												FM_CM = 1;
											}
											else {
												if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer + 1 , dal_xl ) > 0 ) {
													FM_CM = 1;
												}
												else {
													if ( dal_xl != FM_TSlot2 ) {
														FM_CM = 1;
													}
												}
											}
										}
									}
								}
							}
							else {
								if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
								}
								else {
									if ( FM_Slot > 0 ) {
										if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( FM_Buffer , &dal_xl , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( FM_Buffer - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
											FM_CM = 1;
										}
										else {
											if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , dal_xl ) > 0 ) {
												FM_CM = 1;
											}
											else {
												if ( dal_xl != FM_TSlot ) {
													FM_CM = 1;
												}
											}
										}
									}
									if ( FM_Slot2 > 0 ) {
										if ( !SCHEDULER_CONTROL_Chk_BM_SELECT_ONESLOT_for_STYLE_4_Part( FM_Buffer , &dal_xl , ( _SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( FM_Buffer - BM1 + PM1 ) <= 1 ) ? 2 : 1 ) ) {
											FM_CM = 1;
										}
										else {
											if ( _SCH_MODULE_Get_BM_WAFER( FM_Buffer , dal_xl ) > 0 ) {
												FM_CM = 1;
											}
											else {
												if ( dal_xl != FM_TSlot2 ) {
													FM_CM = 1;
												}
											}
										}
									}
								}
							}
							//
							if ( FM_CM != 0 ) {
								if ( SCHEDULING_Normal_Return_Check_for_STYLE_4() == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 1201 from FEM%cWHFMFAIL 1201\n" , 9 );
									return 0;
								}
								FM_Slot = 0;
								FM_Slot2 = 0;
								Result = -1200;
								SignalCount = 1; // 2007.07.31
							}
						}
					}
					//=========================================================================================
				}
				//===============================================================================================
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 24 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
				//----------------------------------------------------------------------
				if ( ( !_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 19 from FEM%cWHFMFAIL 19\n" , 9 );
					return 0;
				}
				//==========================================================================================
				if ( Result >= 0 ) {
					//==========================================================================================
					FM_Place_Running_Blocking_Style_4 = TRUE; // 2007.05.30
					//==========================================================================================
					// Code for Place to BI
					// FM_Slot
					// FM_Slot2
					//==========================================================================================
					if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer ) < 0 ) { // 2008.01.15
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , 9 , FM_Buffer - BM1 + 1 );
						return 0;
					}
					//
					Result = 3; // 2006.10.18
					//
					if ( ( doubleok4isdouble == 3 ) && ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) { // 2005.09.30
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 2 ) { // 2006.01.06
							//
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer + 1 ) < 0 ) { // 2008.01.15
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , 9 , FM_Buffer + 1 - BM1 + 1 );
								return 0;
							}
							//
							FM_CM = _SCH_CLUSTER_Get_PathIn( FM_dbSide , FM_Pointer ); // 2006.11.03
							//
							if ( FM_dbSide == FM_dbSide2 ) {
								_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Place Start to %s,%s(%d:%d,%d:%d)%cWHFMPLACESTART BM%d,BM%d:%d:%d,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , FM_TSlot , FM_Slot , FM_TSlot2 , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 , ( FM_Buffer + 1 ) - BM1 + 1 , FM_TSlot , FM_Slot , FM_TSlot2 , FM_Slot2 , 9 , ( FM_Slot2 * 100 ) + FM_Slot );
							}
							else {
								_SCH_LOG_LOT_PRINTF( FM_dbSide  , "FM Handling Place Start to %s(%d:%d)%cWHFMPLACESTART BM%d:%d:%d,0:0%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_TSlot , FM_Slot , 9 , FM_Buffer - BM1 + 1 , FM_TSlot , FM_Slot , 9 , FM_Slot );
								_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Place Start to %s(%d:%d)%cWHFMPLACESTART BM%d:0:0,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , FM_TSlot2 , FM_Slot2 , 9 , ( FM_Buffer + 1 ) - BM1 + 1 , FM_TSlot2 , FM_Slot2 , 9 , FM_Slot2 );
							}
							//
							_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_dbSide , FM_Pointer  , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , FM_Buffer     , 0 , 0 , FM_TSlot  , FM_Slot );
							_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_dbSide2 , FM_Pointer2 , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , FM_Buffer + 1 , 0 , 1 , FM_TSlot2 , FM_Slot2 );
							//
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2006.03.10
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2006.03.10
							//
							if ( _SCH_EQ_PLACE_TO_FMBM( 0 , FM_dbSide , FM_Buffer , FM_TSlot , FM_TSlot2 + _SCH_PRM_GET_MODULE_SIZE( FM_Buffer ) , FALSE , FM_CM , 0 ) == SYS_ABORTED ) {
								if ( FM_dbSide == FM_dbSide2 ) {
									_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Place Fail to %s,%s(%d:%d,%d:%d)%cWHFMPLACEFAIL BM%d,BM%d:%d:%d,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , FM_TSlot , FM_Slot , FM_TSlot2 , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 , ( FM_Buffer + 1 ) - BM1 + 1 , FM_TSlot , FM_Slot , FM_TSlot2 , FM_Slot2 , 9 , ( FM_Slot2 * 100 ) + FM_Slot );
								}
								else {
									_SCH_LOG_LOT_PRINTF( FM_dbSide  , "FM Handling Place Fail to %s(%d:%d)%cWHFMPLACEFAIL BM%d:%d:%d,0:0%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_TSlot , FM_Slot , 9 , FM_Buffer - BM1 + 1 , FM_TSlot , FM_Slot , 9 , FM_Slot );
									_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Place Fail to %s(%d:%d)%cWHFMPLACEFAIL BM%d:0:0,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , FM_TSlot2 , FM_Slot2 , 9 , ( FM_Buffer + 1 ) - BM1 + 1 , FM_TSlot2 , FM_Slot2 , 9 , FM_Slot2 );
								}
								//
								_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_dbSide , FM_Pointer  , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , FM_Buffer     , 0 , 0 , FM_TSlot  , FM_Slot );
								_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_dbSide2 , FM_Pointer2 , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , FM_Buffer + 1 , 0 , 1 , FM_TSlot2 , FM_Slot2 );
								//
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
							}
							//========================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( FM_Buffer ); // 2006.11.23
							//========================================================================================
							//----------------------------------------------------------------------
//								_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , 0 ); // 2006.09.21
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot , FM_dbSide , FM_Pointer );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , 0 ); // 2006.09.21
							//
//								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , 0 ); // 2006.09.21
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer + 1 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer + 1 , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , 0 ); // 2006.09.21
							//-------------------------------------------------------------------------------------------
							SCHEDULER_Set_FM_WILL_GO_BM( FM_dbSide , 0 );
							SCHEDULER_Set_FM_WILL_GO_BM( FM_dbSide2 , 0 );
							//-------------------------------------------------------------------------------------------
							if ( FM_dbSide == FM_dbSide2 ) {
								_SCH_LOG_LOT_PRINTF( FM_dbSide , "FM Handling Place Success to %s,%s(%d:%d,%d:%d)%cWHFMPLACESUCCESS BM%d,BM%d:%d:%d,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , FM_TSlot , FM_Slot , FM_TSlot2 , FM_Slot2 , 9 , FM_Buffer - BM1 + 1 , ( FM_Buffer + 1 ) - BM1 + 1 , FM_TSlot , FM_Slot , FM_TSlot2 , FM_Slot2 , 9 , ( FM_Slot2 * 100 ) + FM_Slot );
							}
							else {
								_SCH_LOG_LOT_PRINTF( FM_dbSide  , "FM Handling Place Success to %s(%d:%d)%cWHFMPLACESUCCESS BM%d:%d:%d,0:0%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer ) , FM_TSlot , FM_Slot , 9 , FM_Buffer - BM1 + 1 , FM_TSlot , FM_Slot , 9 , FM_Slot );
								_SCH_LOG_LOT_PRINTF( FM_dbSide2 , "FM Handling Place Success to %s(%d:%d)%cWHFMPLACESUCCESS BM%d:0:0,%d:%d%c%d\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , FM_TSlot2 , FM_Slot2 , 9 , ( FM_Buffer + 1 ) - BM1 + 1 , FM_TSlot2 , FM_Slot2 , 9 , FM_Slot2 );
							}
							//
							_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_dbSide  , FM_Pointer  , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , FM_Buffer     , 0 , 0 , FM_TSlot  , FM_Slot );
							_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_dbSide2 , FM_Pointer2 , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , FM_Buffer + 1 , 0 , 1 , FM_TSlot2 , FM_Slot2 );
							//----------------------------------------------------------------------
						}
						else { // 2006.01.06
							//
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer + 1 ) < 0 ) { // 2008.01.15
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Scheduling Aborted Because %s Abort%cWHBM%dFAIL\n" , _SCH_SYSTEM_GET_MODULE_NAME( FM_Buffer + 1 ) , 9 , FM_Buffer + 1 - BM1 + 1 );
								return 0;
							}
							//
							//-----------------------------------------------
							FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
							//-----------------------------------------------
							//=============================================================================================
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2006.03.10
							//=============================================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 310 , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Buffer , 0 , 0 , 0 , 0 , -1 ) ) { // 2007.03.21
							case -1 :
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
								break;
							}
							//=============================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( FM_Buffer ); // 2006.11.23
							//========================================================================================
//								_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , 0 ); // 2006.09.21
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot , FM_dbSide , FM_Pointer );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , 0 ); // 2006.09.21
							//-------------------------------------------------------------------------------------------
							SCHEDULER_Set_FM_WILL_GO_BM( FM_dbSide , 0 );
							//-------------------------------------------------------------------------------------------
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2006.03.10
							//=============================================================================================
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 320 , FM_Buffer + 1 , 0 , 0 , 0 , 0 , FM_Buffer + 1 , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
							case -1 :
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
								break;
							}
							//=============================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( FM_Buffer + 1 ); // 2006.11.23
							//========================================================================================
//								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , 0 ); // 2006.09.21
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer + 1 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer + 1 , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , 0 ); // 2006.09.21
							//-------------------------------------------------------------------------------------------
							SCHEDULER_Set_FM_WILL_GO_BM( FM_dbSide2 , 0 );
						}
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( FM_dbSide , FM_Buffer , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( FM_Buffer - BM1 + PM1 ) ) ) {
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 20 from FEM%cWHFMFAIL 20\n" , 9 );
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 4 ); // 2005.09.09
							//==================================================================================
						}
						else {
							if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( FM_dbSide , FM_Buffer ) ) {
								//=================================================================================================================
								// 2005.10.18
								//=================================================================================================================
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 21 from FEM%cWHFMFAIL 21\n" , 9 );
									//==========================================================================================
									FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
									//==========================================================================================
									return 0;
								}
								//=================================================================================================================
								//==================================================================================
								SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide , TRUE , FM_Buffer , 5 ); // 2005.09.09
								//==================================================================================
							}
						}
						//----------------------------------------------------------------------
						//----------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( FM_dbSide2 , FM_Buffer + 1 , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( ( FM_Buffer + 1 ) - BM1 + PM1 ) ) ) {
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ( FM_Buffer + 1 ) - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 22 from FEM%cWHFMFAIL 22\n" , 9 );
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 4 ); // 2005.09.09
							//==================================================================================
						}
						else {
							if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( FM_dbSide2 , FM_Buffer + 1 ) ) {
								//=================================================================================================================
								// 2005.10.18
								//=================================================================================================================
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( ( FM_Buffer + 1 ) - BM1 + PM1 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 23 from FEM%cWHFMFAIL 23\n" , 9 );
									//==========================================================================================
									FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
									//==========================================================================================
									return 0;
								}
								//=================================================================================================================
								//==================================================================================
								SCHEDULER_CONTROL_BM_PUMPING_FM_PART( FM_dbSide2 , TRUE , FM_Buffer + 1 , 5 ); // 2005.09.09
								//==================================================================================
							}
						}
						//----------------------------------------------------------------------
					}
					else {
						//
						if ( _SCH_MODULE_Get_BM_FULL_MODE( FM_Buffer ) == BUFFER_WAIT_STATION ) { // 2009.08.05
							if ( _SCH_MACRO_RUN_WAITING_BM_OPERATION( BUFFER_FM_STATION , CHECKING_SIDE , FM_Buffer , 1 , TRUE , 0 , 1 ) == SYS_ABORTED ) {
								return 0;
							}
						}
						//
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
							//==========================================================================
							FM_dbSide  = _SCH_MODULE_Get_FM_SIDE( TA_STATION ); // 2007.01.08
							FM_dbSide2 = _SCH_MODULE_Get_FM_SIDE( TB_STATION ); // 2007.01.08
							//==========================================================================
						}
						else {
							FM_dbSide  = CHECKING_SIDE; // 2007.01.08
							FM_dbSide2 = CHECKING_SIDE; // 2007.01.08
						}
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2006.03.10
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2006.03.10
							//
						}
						else if ( FM_Slot > 0 ) {
							FM_TSlot2 = 0; // 2006.08.31
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide , FM_Pointer ); // 2006.03.10
						}
						else if ( FM_Slot2 > 0 ) {
							FM_TSlot = 0; // 2006.08.31
							_SCH_ONESELECT_Reset_First_What( ( _SCH_PRM_GET_MULTI_GROUP_AUTO_ARRANGE() == 1 ) , FM_dbSide2 , FM_Pointer2 ); // 2006.03.10
						}
						//=============================================================================================
						//-----------------------------------------------
						FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
						//-----------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2017.04.21
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 330 , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , ( ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) && ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) ? FM_Buffer : FM_Buffer + ( ( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 ) * 10000 )  , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
							case -1 :
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
								break;
							}
						}
						else { // 2017.04.21
							switch( _SCH_MACRO_FM_OPERATION( 0 , MACRO_PLACE + 330 , FM_Buffer , FM_Slot , FM_TSlot , FM_dbSide , FM_Pointer , FM_Buffer , FM_Slot2 , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 , -1 ) ) { // 2007.03.21
							case -1 :
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
								break;
							}
						}
						//========================================================================================
						SCHEDULER_Set_BM_WhereLastPlaced( FM_Buffer ); // 2006.11.23
						//========================================================================================
						if ( FM_Slot > 0 ) {
							//=============================================================================================================================================
							// 2007.05.10
							//=============================================================================================================================================
							_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot , FM_dbSide , FM_Pointer );
							_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot , FM_Slot , BUFFER_INWAIT_STATUS );
							_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , 0 ); // 2006.09.21
							//=============================================================================================================================================
							// 2007.05.10
							//=============================================================================================================================================
							if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
								if ( !_SCH_MODULE_GET_USE_ENABLE( FM_Buffer - BM1 + PM1 , FALSE , FM_dbSide ) ) {
									_SCH_MODULE_Set_BM_STATUS( FM_Buffer , FM_TSlot , BUFFER_OUTWAIT_STATUS );
									_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide , FM_Pointer , -1 );
								}
							}
							//=============================================================================================================================================
						}
						if ( FM_Slot2 > 0 ) {
							//===============================================================================================================
							if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) { // 2007.01.06
								if ( FM_Slot > 0 ) { // 2007.03.29
									_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_TSlot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , FM_dbSide2 , FM_Pointer2 );
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_TSlot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , FM_Slot2 , BUFFER_INWAIT_STATUS );
									//=============================================================================================================================================
									// 2007.05.10
									//=============================================================================================================================================
									if ( !_SCH_MODULE_GET_USE_ENABLE( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 - BM1 + PM1 , FALSE , FM_dbSide2 ) ) {
										_SCH_MODULE_Set_BM_STATUS( FM_Buffer + SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() - 1 , FM_TSlot2 / SCHEDULER_CONTROL_Get_ONEBODY_3_BMCOUNT() , BUFFER_OUTWAIT_STATUS );
										_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide2 , FM_Pointer2 , -1 );
									}
									//=============================================================================================================================================
								}
								else { // 2007.03.29
									_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 ); // 2007.03.29
									_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS ); // 2007.03.29
									//=============================================================================================================================================
									// 2007.05.10
									//=============================================================================================================================================
									if ( !_SCH_MODULE_GET_USE_ENABLE( FM_Buffer - BM1 + PM1 , FALSE , FM_dbSide2 ) ) {
										_SCH_MODULE_Set_BM_STATUS( FM_Buffer , FM_TSlot2 , BUFFER_OUTWAIT_STATUS );
										_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_dbSide2 , FM_Pointer2 , -1 );
									}
									//=============================================================================================================================================
								}
							}
							else {
								_SCH_MODULE_Set_BM_SIDE_POINTER( FM_Buffer , FM_TSlot2 , FM_dbSide2 , FM_Pointer2 );
								_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_TSlot2 , FM_Slot2 , BUFFER_INWAIT_STATUS );
							}
							//===============================================================================================================
							_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , 0 ); // 2006.09.21
						}
						//----------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
							//=================================================================
							FMICPickReject = TRUE; // 2007.01.08
							//=================================================================
							if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
								_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( FM_Buffer , FM_TSlot , 2 ); // 2007.07.11
							}
							FM_Bufferx = FM_Buffer;
						}
						else {
							if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
								//==============================================================
								_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( FM_Buffer , FM_TSlot , 1 ); // 2007.07.11
								//==============================================================
							}
							FM_Bufferx = FM_Buffer;
						}
						//-------------------------------------------------------------------------------------------
						// 2007.09.03
						//-------------------------------------------------------------------------------------------
						if ( ( FM_Slot > 0 ) && ( FM_Slot2 > 0 ) ) {
							if ( !FM_Has_Swap_Wafer_Style_4 ) {
								SCHEDULER_SWAPPING_3_1_REMAP_BM_ONEBODY3( -1 , FM_TSlot );
							}
						}
						//-------------------------------------------------------------------------------------------
						SCHEDULER_Set_FM_WILL_GO_BM( CHECKING_SIDE , 0 ); // 2003.06.20
						//-------------------------------------------------------------------------------------------
//						if ( !SCHEDULER_CONTROL_Chk_BM_FM_SINGLE_FORCE_MOVE_FOR_4( FM_Buffer , FM_TSlot ) ) return 0; // 2010.09.03 // 2015.01.23
						if ( !SCHEDULER_CONTROL_Chk_BM_FM_SINGLE_FORCE_MOVE_FOR_4( FM_Buffer , ( FM_Slot > 0 ) ? FM_TSlot : FM_TSlot2 ) ) return 0; // 2010.09.03 // 2015.01.23
						//----------------------------------------------------------------------
						if ( SCHEDULER_CONTROL_Chk_BM_CHANGE_TO_PUMP_FOR_4( CHECKING_SIDE , FM_Bufferx , FALSE , _SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( FM_Bufferx - BM1 + PM1 ) ) ) { // 2006.12.15
							//=================================================================================================================
							// 2005.10.18
							//=================================================================================================================
							if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 24 from FEM%cWHFMFAIL 24\n" , 9 );
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
							}
							//=================================================================================================================
							//==================================================================================
							// 2006.12.15
							//==================================================================================
							if ( SCHEDULER_CONTROL_Chk_AFTER_PLACE_BM_AND_PUMP_FOR_4( CHECKING_SIDE , FM_Buffer , FM_Slot , FM_Slot2 , 4 ) == -1 ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 101 from FEM%cWHFMFAIL 101\n" , 9 );
								//==========================================================================================
								FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
								//==========================================================================================
								return 0;
							}
							//==================================================================================
						}
						else {
							if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( CHECKING_SIDE , FM_Buffer ) ) {
								//=================================================================================================================
								// 2005.10.18
								//=================================================================================================================
								if ( _SCH_MACRO_CHECK_PROCESSING_INFO( FM_Buffer - BM1 + PM1 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 25 from FEM%cWHFMFAIL 25\n" , 9 );
									//==========================================================================================
									FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
									//==========================================================================================
									return 0;
								}
								//=================================================================================================================
								//==================================================================================
								// 2006.12.15
								//==================================================================================
								if ( SCHEDULER_CONTROL_Chk_AFTER_PLACE_BM_AND_PUMP_FOR_4( CHECKING_SIDE , FM_Buffer , FM_Slot , FM_Slot2 , 5 ) == -1 ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 101 from FEM%cWHFMFAIL 101\n" , 9 );
									//==========================================================================================
									FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
									//==========================================================================================
									return 0;
								}
								//==================================================================================
							}
						}
						//----------------------------------------------------------------------
					}
					//==========================================================================================
					FM_Place_Running_Blocking_Style_4 = FALSE; // 2007.05.30
					//==========================================================================================
					//----------------------------------------------------------------------
					_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
					//----------------------------------------------------------------------
					// 2004.10.14
					//----------------------------------------------------------------------
					if ( ( FM_Slot > 0 ) && ( FM_Pointer < 100 ) ) {
						_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , FM_Pointer , 0 );
					}
					if ( ( FM_Slot2 > 0 ) && ( FM_Pointer2 < 100 ) ) {
						_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( CHECKING_SIDE , FM_Pointer2 , 0 );
					}
					//-------------------------------------------------------------------------------------
					// 2006.10.18
					//-------------------------------------------------------------------------------------
					if ( ( doubleok == 4 ) && ( Result != 3 ) && ( ( FM_Slot <= 0 ) || ( FM_Slot2 <= 0 ) ) ) {
						if ( Scheduling_Other_Side_Pick_From_FM_for_4( FALSE , -1 , FALSE , &sp_start , FALSE , FALSE ) == 2 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 26 from FEM%cWHFMFAIL 26\n" , 9 );
							return 0;
						}
					}
					//-------------------------------------------------------------------------------------
//						if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2004.08.20
					if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.08.20 // 2007.01.06
						SignalBM = FM_Buffer;
						SignalCount = 0; // 2004.08.24
					}
					//-------------------------------------------------------------------------------------
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 25 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][FMICPickReject=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , FMICPickReject );
		//----------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		//=====================================================================================================
		FM_Pick_Running_Blocking_Style_4 = FALSE; // 2006.12.07
		//=====================================================================================================
		//-------------------------------------------------------------------------------------
		// PICK FROM IC // 2005.09.06
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
//			if ( !FMICPickReject && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2007.01.06
		if ( !FMICPickReject && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2007.01.06
			if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
				ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( CHECKING_SIDE , &FM_Side2 , &ICsts , &ICsts2 );
				if ( ALsts == SYS_ABORTED ) {
					_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
					return 0;
				}
				else if ( ALsts == SYS_SUCCESS ) {
					//==================================================================================
					// 2006.10.19
					//==================================================================================
					switch ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) ) {
					case 1 :	ICsts2 = 0;	break;
					case 2 :	ICsts2 = 0;	break;
					}
					//===============================================================================================
					// 2005.11.29
					//===============================================================================================
					if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) && ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( ICsts > 0 ) && ( ICsts2 > 0 ) && ( _SCH_MODULE_Get_MFAL_WAFER() <= 0 ) ) {
						//==================================================================================
						if ( ( ICsts > 0 ) || ( ICsts2 > 0 ) ) { // 2006.09.27
							//=======================================================
							// 2006.11.10
							//=======================================================
							SignalMove = 0;
							//=======================================================
							//==================================================================================
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at IC(%d:%d)%cWHFMICSUCCESS %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
							//---------------------------------------------------------------------------------------
							if ( ICsts > 0 ) { // 2006.09.27
								FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) );
							} // 2006.09.27
							else { // 2006.09.27
								FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) ); // 2006.09.27
							} // 2006.09.27
							//-----------------------------------------------
							FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
							//-----------------------------------------------
							//---------------------------------------------------------------------------------------
							if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , CHECKING_SIDE , IC , ICsts , ICsts2 , FM_CM , -1 , TRUE , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , _SCH_MODULE_Get_MFIC_SIDE( ICsts ) , _SCH_MODULE_Get_MFIC_SIDE( ICsts2 ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) ) == SYS_ABORTED ) {
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d:%d)%cWHFMICFAIL %d:%d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) * 100 + _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
								return 0;
							}
							//========================================================================================
							SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
							//========================================================================================
							if ( ICsts > 0 ) { // 2006.09.27
								//========================================================================================
								_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , _SCH_MODULE_Get_MFIC_POINTER( ICsts ) , FMWFR_PICK_BM_DONE_IC );
								_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
								//========================================================================================
								_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
								//===============================================================================================================
								// 2006.11.10
								//===============================================================================================================
//									if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
								//===============================================================================================================
							}
							//========================================================================================
							if ( ICsts2 > 0 ) { // 2006.09.27
								//========================================================================================
								_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , _SCH_MODULE_Get_MFIC_POINTER( ICsts2 ) , FMWFR_PICK_BM_DONE_IC );
								_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_Side2 , _SCH_MODULE_Get_MFIC_WAFER( ICsts2 ) );
								//========================================================================================
								_SCH_MODULE_Set_MFIC_WAFER( ICsts2 , 0 );
								//===============================================================================================================
								// 2006.11.10
								//===============================================================================================================
//									if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
								//===============================================================================================================
							}
							//========================================================================================
						}
					}
					//===============================================================================================
					else {
//							if ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) || ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) { // 2005.11.29
						//
						if ( ICsts2 > 0 ) { // 2005.11.30
							if ( _SCH_MODULE_Chk_MFIC_FREE_COUNT() <= 0 ) {
								if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
									ICsts2 = 0;
								}
							}
						}
						//
						if ( ( ICsts2 == 0 ) && ( !SCHEDULER_CONTROL_Chk_BM_NEED_OUT_4( CHECKING_SIDE ) || ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) ) { // 2005.11.29
							FM_Buffer = FALSE;
//								if      ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) { // 2006.09.27
							if      ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_IC) != 2 ) ) { // 2006.09.27
								FingerCount = TA_STATION;
								if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
									if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_DONE_IC ) {
										FingerCount = -1;
									}
									else if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_BM_NEED_IC ) {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_IC) != 1 ) { // 2006.09.27
											FM_Buffer = TRUE;
										}
									}
									else {
										if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_NEED_AL ) ) {
											FingerCount = -1;
										}
									}
								}
							}
//								else if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) { // 2006.09.27
							else if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) && ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL(F_IC) != 1 ) ) { // 2006.09.27
								FingerCount = TB_STATION;
								if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
									if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_DONE_IC ) {
										FingerCount = -1;
									}
									else if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_BM_NEED_IC ) {
										if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL(F_IC) != 2 ) { // 2006.09.27
											FM_Buffer = TRUE;
										}
									}
									else {
										if ( ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_NEED_AL ) ) {
											FingerCount = -1;
										}
									}
								}
							}
							else {
								FingerCount = -1;
							}
							if ( FingerCount != -1 ) {
								//=======================================================
								// 2006.11.10
								//=======================================================
								SignalMove = 0;
								//=======================================================
								_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at IC(%c:%d)%cWHFMICSUCCESS %c:%d%c%d\n" , FingerCount + 'A' , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , FingerCount + 'A' , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
								if ( FingerCount == TA_STATION ) {
									FM_Slot = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
									FM_Pointer = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
									FM_Slot2 = 0;
									FM_Pointer2 = 0;
									FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
								}
								else {
									FM_Slot2 = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
									FM_Pointer2 = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
									FM_Slot = 0;
									FM_Pointer = 0;
									FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer2 );
								}
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								//---------------------------------------------------------------------------------------
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , IC , ( FM_Slot > 0 ? ICsts : 0 ) , ( FM_Slot2 > 0 ? ICsts : 0 ) , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , CHECKING_SIDE , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2005.11.28
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
								//========================================================================================
								if ( FingerCount == TA_STATION ) {
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
									//===============================================================================================================
									// 2006.11.10
									//===============================================================================================================
//										if ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TB_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
									//===============================================================================================================
								}
								else {
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , CHECKING_SIDE , FM_Slot2 );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
									//===============================================================================================================
									// 2006.11.10
									//===============================================================================================================
//										if ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL_GOBM ) _SCH_MODULE_Set_FM_MODE( TA_STATION , FMWFR_PICK_CM_DONE_AL ); // 2007.09.12
									//===============================================================================================================
								}
								//
								if ( FM_Buffer && ( _SCH_MODULE_Chk_MFIC_FREE_SLOT( &ICsts , &FM_Slot , -1 , -1 ) > 0 ) ) {
									if ( FingerCount == TA_STATION ) {
										FM_Side		= _SCH_MODULE_Get_FM_SIDE( TB_STATION );
										FM_Slot     = 0;
										FM_Pointer  = 0;
										FM_Slot2    = _SCH_MODULE_Get_FM_WAFER( TB_STATION );
										FM_Pointer2 = _SCH_MODULE_Get_FM_POINTER( TB_STATION );
										FM_CM       = _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer2 );
									}
									else {
										FM_Side		= _SCH_MODULE_Get_FM_SIDE( TA_STATION );
										FM_Slot     = _SCH_MODULE_Get_FM_WAFER( TA_STATION );
										FM_Pointer  = _SCH_MODULE_Get_FM_POINTER( TA_STATION );
										FM_Slot2    = 0;
										FM_Pointer2 = 0;
										FM_CM       = _SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer );
									}
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PLACE , FM_Side , IC , ( FM_Slot > 0 ? ICsts : 0 ) , ( FM_Slot2 > 0 ? ICsts : 0 ) , FM_CM , FM_CM , TRUE , FM_Slot , FM_Slot2 , FM_Side , FM_Side , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) { // 2006.10.19
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 ); // 2006.11.23
									//========================================================================================
									if ( FingerCount == TA_STATION ) {
//											_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_Side , 0 ); // 2006.09.21
										//
										_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , FM_Side , FM_Pointer2 );
										_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_Slot2 );
										//
										_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_Side , 0 ); // 2006.09.21
										//============================================================================
										_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , 0 , ICsts , FM_Slot , FM_Slot2 , FM_CM );
										//============================================================================
									}
									else {
//											_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_Side , 0 ); // 2006.09.21
										//
										_SCH_MODULE_Set_MFIC_SIDE_POINTER( ICsts , FM_Side , FM_Pointer );
										_SCH_MODULE_Set_MFIC_WAFER( ICsts , FM_Slot );
										//
										_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_Side , 0 ); // 2006.09.21
										//============================================================================
										_SCH_MACRO_SPAWN_FM_MCOOLING( FM_Side , ICsts , 0 , FM_Slot , FM_Slot2 , FM_CM );
										//============================================================================
									}
								}
							}
						}
					}
				}
				//----------------------------------------------------------------------
				_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 25a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][FMICPickReject=%d][ALsts=%d][ICsts=%d][ICsts2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , FMICPickReject , ALsts , ICsts , ICsts2 );
				//----------------------------------------------------------------------
			}
		}
		//===================================================================================================================
		// 2006.12.18
		//===================================================================================================================
		//===================================================================================================================
		// 2006.12.18
		//===================================================================================================================
		else if (
			!FMICPickReject &&
			( ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ||
			  ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ||
			  ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) ) // 2007.01.06
			) {
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) { // 2007.01.08
				if ( _SCH_MODULE_Need_Do_Multi_FIC() ) {
					ALsts = _SCH_MODULE_Get_MFIC_Completed_Wafer( CHECKING_SIDE , &FM_Side2 , &ICsts , &ICsts2 );
					if ( ALsts == SYS_ABORTED ) {
						_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
						return 0;
					}
					else if ( ALsts == SYS_SUCCESS ) {
						if ( SCHEDULER_CONTROL_PICK_IC_RUNNING_3( CHECKING_SIDE ) ) {
							if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) )	FingerCount = TA_STATION;
							else													FingerCount = TB_STATION;
							//
							SignalMove = 0;
							//
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at IC(%c:%d)%cWHFMICSUCCESS %c:%d%c%d\n" , FingerCount + 'A' , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , FingerCount + 'A' , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
							//
							FM_Slot     = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
							FM_Pointer  = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
							//---------------------------------------------------------------------------------------
							FM_CM = _SCH_CLUSTER_Get_PathOut( CHECKING_SIDE , FM_Pointer );
							//---------------------------------------------------------------------------------------
							if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) == 3 ) {
								switch ( Scheduler_MFIC_Pick_Multi_Mode( ICsts ) ) {
								case 3 : // Arm AB(SWAP) // 2007.09.04
									//==============================================================================================================
									FM_dbSide   = _SCH_MODULE_Get_MFIC_SIDE( ICsts + 1 );
									FM_Slot     = _SCH_MODULE_Get_MFIC_WAFER( ICsts + 1 );
									FM_Pointer  = _SCH_MODULE_Get_MFIC_POINTER( ICsts + 1 );
									//==============================================================================================================
									FM_dbSide2  = _SCH_MODULE_Get_MFIC_SIDE( ICsts );
									FM_Slot2    = _SCH_MODULE_Get_MFIC_WAFER( ICsts );
									FM_Pointer2 = _SCH_MODULE_Get_MFIC_POINTER( ICsts );
									//==============================================================================================================
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , CHECKING_SIDE , IC , ICsts , ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 );
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_dbSide , FM_Slot );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts + 1 , 0 );
									//========================================================================================
									_SCH_FMARMMULTI_DATA_SET_FROM_FIC( 0 , ICsts + 1 , 0 );
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , FM_Slot2 );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
									//========================================================================================
									// 2007.09.03
									//========================================================================================
									FM_Has_Swap_Wafer_Style_4 = FALSE;
									//========================================================================================
									break;

								case 2 : // Arm AB
									//==============================================================================================================
									FM_dbSide2  = _SCH_MODULE_Get_MFIC_SIDE( ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
									FM_Slot2    = _SCH_MODULE_Get_MFIC_WAFER( ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
									FM_Pointer2 = _SCH_MODULE_Get_MFIC_POINTER( ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 );
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK_ALL , CHECKING_SIDE , IC , ICsts , ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , FM_CM , -1 , TRUE , FM_Slot , FM_Slot2 , CHECKING_SIDE , FM_dbSide2 , FM_Pointer , FM_Pointer2 ) == SYS_ABORTED ) {
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 );
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , FM_Pointer , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , CHECKING_SIDE , FM_Slot );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
									//========================================================================================
									_SCH_FMARMMULTI_DATA_SET_FROM_FIC( 0 , ICsts , 0 );
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer2 , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide2 , FM_Slot2 );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts + _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 1 , 0 );
									//========================================================================================
									// 2007.09.03
									//========================================================================================
									FM_Has_Swap_Wafer_Style_4 = TRUE;
									//========================================================================================
									break;
								case 1 : // Arm A
									FM_Slot2    = 0;
									FM_Pointer2 = 0;
									//---------------------------------------------------------------------------------------
									//-----------------------------------------------
									FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
									//-----------------------------------------------
//									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , IC , ( FingerCount == TA_STATION ? ICsts : 0 ) , ( FingerCount == TA_STATION ? ICsts : 0 ) , FM_CM , -1 , TRUE , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , CHECKING_SIDE , CHECKING_SIDE , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) ) == SYS_ABORTED ) { // 2011.12.13
									if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , IC , ( FingerCount == TA_STATION ? ICsts : 0 ) , ( FingerCount == TB_STATION ? ICsts : 0 ) , FM_CM , -1 , TRUE , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FingerCount == TB_STATION ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , CHECKING_SIDE , CHECKING_SIDE , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) , ( FingerCount == TB_STATION ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) ) == SYS_ABORTED ) { // 2011.12.13
										_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
										return 0;
									}
									//========================================================================================
									SCHEDULER_Set_BM_WhereLastPlaced( -1 );
									//========================================================================================
									_SCH_MODULE_Set_FM_POINTER_MODE( FingerCount , FM_Pointer , FMWFR_PICK_BM_DONE_IC );
									_SCH_MODULE_Set_FM_SIDE_WAFER( FingerCount , CHECKING_SIDE , FM_Slot );
									//========================================================================================
									_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
									//========================================================================================
									_SCH_FMARMMULTI_DATA_SET_FROM_FIC( 0 , ICsts , 0 );
									//========================================================================================
									//========================================================================================
									// 2007.09.03
									//========================================================================================
									FM_Has_Swap_Wafer_Style_4 = FALSE;
									//========================================================================================
									break;
								case 0 : // Arm B
									for ( FM_Ext_Loop = 0 ; FM_Ext_Loop < ( _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + 2 ) ; FM_Ext_Loop++ ) {
										FM_dbSide   = _SCH_MODULE_Get_MFIC_SIDE( ICsts + FM_Ext_Loop );
										FM_Slot     = _SCH_MODULE_Get_MFIC_WAFER( ICsts + FM_Ext_Loop );
										FM_Pointer  = _SCH_MODULE_Get_MFIC_POINTER( ICsts + FM_Ext_Loop );
										if ( FM_Slot > 0 ) {
											//==============================================================================================
											// 2007.05.10
											//==============================================================================================
											ALsts = _SCH_MACRO_CHECK_FM_MCOOLING( FM_dbSide , ICsts + FM_Ext_Loop , FALSE );
											if ( ALsts == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
												return 0;
											}
											else if ( ALsts != SYS_SUCCESS ) {
												break;
											}
											//==============================================================================================
											FM_CM = _SCH_CLUSTER_Get_PathOut( FM_dbSide , FM_Pointer );
											//========================================================================================
											//-----------------------------------------------
											FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
											//-----------------------------------------------
											if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , FM_dbSide , IC , 0 , ICsts + FM_Ext_Loop , FM_CM , -1 , TRUE , 0 , FM_Slot , 0 , FM_dbSide , 0 , FM_Pointer ) == SYS_ABORTED ) {
												_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , FM_Slot , 9 , FM_Slot , 9 , FM_Slot );
												return 0;
											}
											//========================================================================================
											_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , FM_Pointer , -1 );
											_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_dbSide , FM_Slot );
											//
											_SCH_MODULE_Set_MFIC_WAFER( ICsts + FM_Ext_Loop , 0 );
											//========================================================================================
											//========================================================================================
											// 2007.09.03
											//========================================================================================
											FM_Has_Swap_Wafer_Style_4 = FALSE;
											//========================================================================================
											if ( Scheduler_Place_to_CM_with_FM( 2 , FALSE ) == -1 ) {
												return 0;
											}
											//========================================================================================
										}
									}
									break;
								}
							}
							else {
								FM_Slot2    = 0;
								FM_Pointer2 = 0;
								//-----------------------------------------------
								FM_Last_Move_Recv_BM_4 = 0; // 2010.04.16
								//-----------------------------------------------
								//---------------------------------------------------------------------------------------
								if ( _SCH_MACRO_FM_ALIC_OPERATION( 0 , FAL_RUN_MODE_PICK , CHECKING_SIDE , IC , ( FingerCount == TA_STATION ? ICsts : 0 ) , ( FingerCount == TA_STATION ? ICsts : 0 ) , FM_CM , -1 , TRUE , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_WAFER( ICsts ) : 0 ) , CHECKING_SIDE , CHECKING_SIDE , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) , ( FingerCount == TA_STATION ? _SCH_MODULE_Get_MFIC_POINTER( ICsts ) : 0 ) ) == SYS_ABORTED ) {
									_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Fail at IC(%d)%cWHFMICFAIL %d%c%d\n" , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) , 9 , _SCH_MODULE_Get_MFIC_WAFER( ICsts ) );
									return 0;
								}
								//========================================================================================
								SCHEDULER_Set_BM_WhereLastPlaced( -1 );
								//========================================================================================
								_SCH_MODULE_Set_FM_POINTER_MODE( FingerCount , FM_Pointer , FMWFR_PICK_BM_DONE_IC );
								_SCH_MODULE_Set_FM_SIDE_WAFER( FingerCount , CHECKING_SIDE , FM_Slot );
								//========================================================================================
								_SCH_MODULE_Set_MFIC_WAFER( ICsts , 0 );
								//========================================================================================
								if ( FingerCount == TA_STATION ) {
									//========================================================================================
									_SCH_FMARMMULTI_DATA_SET_FROM_FIC( 0 , ICsts , 0 );
									//========================================================================================
								}
								else {
									//========================================================================================
									_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
									//========================================================================================
								}
							}
						}
					}
				}
			}
			//----------------------------------------------------------------------
			_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 25b RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d][FMICPickReject=%d][ALsts=%d][ICsts=%d][ICsts2=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) , FMICPickReject , ALsts , ICsts , ICsts2 );
			//----------------------------------------------------------------------
		}
		//----------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		//----------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		//----------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
		if ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) { // 2008.02.20
			if ( _SCH_SYSTEM_MODE_END_GET( CHECKING_SIDE ) != 0 ) {
				if ( SignalBM > 0 ) {
					if ( SCHEDULING_Possible_Pump_BM_for_STYLE_4( CHECKING_SIDE , SignalBM ) ) {
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 26a RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result );
						//----------------------------------------------------------------------
						//=================================================================================================================
						// 2005.10.18
						//=================================================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SignalBM - BM1 + PM1 ) == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 27 from FEM%cWHFMFAIL 27\n" , 9 );
							return 0;
						}
						//==================================================================================
						SCHEDULER_CONTROL_BM_PUMPING_FM_PART( CHECKING_SIDE , TRUE , SignalBM , 6 ); // 2005.09.09
						//==================================================================================
					}
				}
				else {
					if ( SCHEDULING_Possible_Pump_BM2_for_STYLE_4( CHECKING_SIDE , &SignalBM ) ) {
						//----------------------------------------------------------------------
						_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 27 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result );
						//----------------------------------------------------------------------
						//=================================================================================================================
						// 2005.10.18
						//=================================================================================================================
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( SignalBM - BM1 + PM1 ) == -1 ) {
							_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Scheduling Abort 28 from FEM%cWHFMFAIL 28\n" , 9 );
							return 0;
						}
						//==================================================================================
						SCHEDULER_CONTROL_BM_PUMPING_FM_PART( CHECKING_SIDE , TRUE , SignalBM , 7 ); // 2005.09.09
						//==================================================================================
					}
				}
			}
		}
		//----------------------------------------------------------------------
		_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM STEP 26 RESULT = [FingerCount=%d][FM_Buffer=%d][FM_Slot=%d][FM_Slot2=%d][SignalBM=%d][SignalCount=%d][FM_Pointer=%d][FM_Pointer2=%d][FM_TSlot=%d][FM_TSlot2=%d][doubleok=%d][Result=%d][WAFER_SUPPLY_MODE=%d,%d]\n" , FingerCount , FM_Buffer , FM_Slot , FM_Slot2 , SignalBM , SignalCount , FM_Pointer , FM_Pointer2 , FM_TSlot , FM_TSlot2 , doubleok , Result , _SCH_MODULE_Get_FM_MODE(0) , _SCH_MODULE_Get_FM_MODE(1) );
		//----------------------------------------------------------------------
		//-------------------------------------------------------------------------------------
//			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2004.08.24
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( SCHEDULER_CONTROL_Chk_ONEBODY_MODE_for_STYLE_4( TRUE ) != 3 ) ) { // 2004.08.24 // 2007.01.06
			if ( SignalCount != 0 ) break;
		}
		//-------------------------------------------------------------------------------------
		SignalCount++;
	}
	//
	_SCH_MODULE_Set_FM_MidCount( 0 );
	return 1;
}



//###################################################################################################
// Include End
//###################################################################################################
#endif
//###################################################################################################
