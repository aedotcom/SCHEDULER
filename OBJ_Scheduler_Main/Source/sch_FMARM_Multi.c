#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "Equip_Handling.h"
#include "User_Parameter.h"
#include "IO_Part_Data.h"
#include "IO_Part_Log.h"
#include "sch_FMArm_Multi.h"
#include "FA_Handling.h"
#include "IOlog.h"
#include "sch_sub.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_CassetteResult.h"
#include "sch_prm.h"
#include "Multijob.h"
#include "sch_Multi_ALIC.h"

#include "INF_sch_macro.h"
//------------------------------------------------------------------------------------------
int				FM_ARM_Ext_Count; // 2007.01.08
int				FM_ARM_Ext_Side[MAX_CASSETTE_SLOT_SIZE]; // 2007.01.08
int				FM_ARM_Ext_Pointer[MAX_CASSETTE_SLOT_SIZE]; // 2007.01.08
int				FM_ARM_Ext_CMSlot[MAX_CASSETTE_SLOT_SIZE]; // 2007.01.08
int				FM_ARM_Ext_BMSlot[MAX_CASSETTE_SLOT_SIZE]; // 2007.01.08
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// FM ARM MULTI Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_FM_ARM_MULTI_DATA( int apmode , int side ) {
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		_i_SCH_FMARMMULTI_DATA_Init(); // 2007.07.11
	}
}
//
void _i_SCH_FMARMMULTI_DATA_Init() { // 2007.07.11
	FM_ARM_Ext_Count = 0;
}

BOOL _i_SCH_FMARMMULTI_DATA_Append( int side , int pointer , int slot , int bmslot ) { // 2007.07.11
	if ( FM_ARM_Ext_Count >= MAX_CASSETTE_SLOT_SIZE ) return FALSE;
	FM_ARM_Ext_Side[FM_ARM_Ext_Count] = side;
	FM_ARM_Ext_Pointer[FM_ARM_Ext_Count] = pointer;
	FM_ARM_Ext_CMSlot[FM_ARM_Ext_Count] = slot;
	FM_ARM_Ext_BMSlot[FM_ARM_Ext_Count] = bmslot;
	FM_ARM_Ext_Count++;
	return TRUE;
}

BOOL _i_SCH_FMARMMULTI_DATA_Exist( int index , int *s , int *p , int *cs , int *bs ) { // 2007.07.11
	*s = FM_ARM_Ext_Side[index];
	*p = FM_ARM_Ext_Pointer[index];
	*cs = FM_ARM_Ext_CMSlot[index];
	*bs = FM_ARM_Ext_BMSlot[index];
	if ( ( FM_ARM_Ext_Side[index] >= 0 ) && ( FM_ARM_Ext_Pointer[index] >= 0 ) && ( FM_ARM_Ext_CMSlot[index] > 0 ) ) return TRUE;
	return FALSE;
}



void _i_SCH_FMARMMULTI_DATA_SHIFTING_A_AND_APPEND( int side , int pointer , int slot , int bmslot , int mode , int *rs , int *rp , int *rw ) { // 2007.08.31
	int i;
	int as = -1 , ap , aw;
	for ( i = 0 ; i < ( FM_ARM_Ext_Count - 1 ) ; i++ ) {
		//==========================================
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			if ( as == -1 ) {
				as = FM_ARM_Ext_Side[i];
				ap = FM_ARM_Ext_Pointer[i];
				aw = FM_ARM_Ext_CMSlot[i];
			}
		}
		//==========================================
		FM_ARM_Ext_Side[i]    = FM_ARM_Ext_Side[i+1];
		FM_ARM_Ext_Pointer[i] = FM_ARM_Ext_Pointer[i+1];
		FM_ARM_Ext_CMSlot[i]  = FM_ARM_Ext_CMSlot[i+1];
		FM_ARM_Ext_BMSlot[i]  = FM_ARM_Ext_BMSlot[i+1];
		//==========================================
	}
	if ( as == -1 ) return;
	//
	*rs = as;
	*rp = ap;
	*rw = aw;
	//
	_i_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , ap , mode );
	_i_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , as , aw );
	//
	FM_ARM_Ext_Count--;
	//
	_i_SCH_FMARMMULTI_DATA_Append( side , pointer , slot , bmslot );
}

void _i_SCH_FMARMMULTI_FA_SUBSTRATE( int ch , int mslot , int Result ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			if ( ch == -1 ) {
//				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , FM_ARM_Ext_CMSlot[i] , FM_ARM_Ext_CMSlot[i] ); // 2008.04.21
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , Result , _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , FM_ARM_Ext_CMSlot[i] , FM_ARM_Ext_CMSlot[i] ); // 2008.04.21
			}
			else {
				if ( mslot >= 10000 ) {
//					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch + i + 1 , 0 , 0 , mslot - 10000 , FM_ARM_Ext_CMSlot[i] ); // 2008.04.21
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , Result , ch + i + 1 , 0 , 0 , mslot - 10000 , FM_ARM_Ext_CMSlot[i] ); // 2008.04.21
				}
				else {
//					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , 0 , 0 , mslot + i + 1 , FM_ARM_Ext_CMSlot[i] ); // 2008.04.21
					_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , Result , ch , 0 , 0 , mslot + i + 1 , FM_ARM_Ext_CMSlot[i] ); // 2008.04.21
				}
			}
		}
	}
}

void _i_SCH_FMARMMULTI_PICK_FROM_CM_POST( int mode , int optiondata ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				_i_SCH_MODULE_Inc_FM_OutCount( FM_ARM_Ext_Side[i] );
			}
			else {
				_i_SCH_MODULE_Inc_TM_OutCount( FM_ARM_Ext_Side[i] );
			}
			//
			_i_SCH_MODULE_Set_FM_LastOutPointer( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
			//
			if      ( mode == 0 ) {
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , 0 , 0 , 0 );
			}
			else if ( mode == 1 ) {
//				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , 23 , -1 , 2 ); // 2008.11.13
//				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , 20 , -1 , 2 ); // 2008.11.13 // 2009.03.05
				_SCH_MACRO_FM_SUPPLY_FIRSTLAST_CHECK( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , 20 , 20 , 2 ); // 2008.11.13 // 2009.03.05
			}
			//
			_i_SCH_IO_MTL_ADD_PICK_COUNT( _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 1 );
			//
			if ( mode == 1 ) {
				_i_SCH_CLUSTER_Set_Buffer( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , optiondata );
			}
			//
			_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
		}
	}
}


void _i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( BOOL retmode ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			if ( retmode ) {
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
			}
			else {
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , _i_SCH_CLUSTER_Get_PathOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_SlotOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
			}
		}
	}
}

void _i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM_POST( int *finc , BOOL endok ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			(*finc)++;
			_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_EVERYOUT , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_PathOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_SlotOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
			if ( SCHEDULER_CONTROL_Chk_Last_Finish_Status_FM( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , (*finc) ) ) {
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_LASTOUT , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_PathOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_SlotOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
			}
			if ( endok ) {
//				_i_SCH_LOG_TIMER_PRINTF( FM_ARM_Ext_Side[i] , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
				_i_SCH_LOG_TIMER_PRINTF( FM_ARM_Ext_Side[i] , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , _i_SCH_CLUSTER_Get_PathIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , FM_ARM_Ext_Pointer[i] , -1 , -1 , "" , "" ); // 2012.02.18
				_i_SCH_MULTIJOB_PROCESSJOB_END_CHECK( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
			}
		}
	}
}

void _i_SCH_FMARMMULTI_CASSETTE_SET_AFTER_PLACE_CM( BOOL retmode ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			if ( retmode ) {
//				_i_SCH_IO_SET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CLUSTER_Get_SlotIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( _i_SCH_CLUSTER_Get_PathIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CLUSTER_Get_SlotIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) ) ); // 2011.04.18
				_i_SCH_IO_SET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CLUSTER_Get_SlotIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CASSETTE_LAST_RESULT2_WITH_CASS_GET( FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i] ) ); // 2011.04.18
			}
			else {
//				_i_SCH_IO_SET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathOut(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CLUSTER_Get_SlotOut(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( _i_SCH_CLUSTER_Get_PathIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CLUSTER_Get_SlotIn(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) ) ); // 2011.04.18
				_i_SCH_IO_SET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathOut(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CLUSTER_Get_SlotOut(FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i]) , _i_SCH_CASSETTE_LAST_RESULT2_WITH_CASS_GET( FM_ARM_Ext_Side[i],FM_ARM_Ext_Pointer[i] ) ); // 2011.04.18
			}
		}
	}
}

void _i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( int cm ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			_i_SCH_IO_MTL_ADD_PLACE_COUNT( cm , ( _i_SCH_CLUSTER_Get_PathDo( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) != PATHDO_WAFER_RETURN ) , FM_ARM_Ext_Side[i] , 1 );
		}
	}
}

void _i_SCH_FMARMMULTI_LASTSET_AFTER_PLACE_CM() { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			//
			_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				_i_SCH_MODULE_Inc_FM_InCount( FM_ARM_Ext_Side[i] );
			}
			else {
				_i_SCH_MODULE_Inc_TM_InCount( FM_ARM_Ext_Side[i] );
			}
		}
	}
}














void _i_SCH_FMARMMULTI_DATA_Set_BM_SIDEWAFER( int ch , int slot , int mode ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			if ( mode == 2 ) { // set
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch + i + 1 , 0 , 0 , slot , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( ch + i + 1 , slot , FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( ch + i + 1 , slot , FM_ARM_Ext_CMSlot[i] , BUFFER_INWAIT_STATUS );
				//
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( ch + i + 1 - BM1 + PM1 , FALSE , FM_ARM_Ext_Side[i] ) ) {
					_i_SCH_MODULE_Set_BM_STATUS( ch + i + 1 , slot , BUFFER_OUTWAIT_STATUS );
					_i_SCH_CLUSTER_Check_and_Make_Return_Wafer( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , -1 );
				}
			}
			else if ( mode == 3 ) { // set : Place BM
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , 0 , 0 , slot + i + 1 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
				//
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) { // 2003.11.28
					_i_SCH_MODULE_Set_BM_SIDE( ch , slot + i + 1 , FM_ARM_Ext_Side[i] );
				}
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( ch , slot + i + 1 , FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( ch , slot + i + 1 , FM_ARM_Ext_CMSlot[i] , BUFFER_INWAIT_STATUS );
			}
			else if ( mode == 11 ) { // reset : Pick BM
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch , 0 , 0 , FM_ARM_Ext_BMSlot[i] , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
				//
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( ch , FM_ARM_Ext_BMSlot[i] , 0 , -1 );
			}
			else if ( mode == 12 ) { // reset
				_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ch + i + 1 , 0 , 0 , FM_ARM_Ext_BMSlot[i] , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
				//
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( ch + i + 1 , FM_ARM_Ext_BMSlot[i] , 0 , -1 );
			}
			else { // set : Place BM
				if ( mode == 1 ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ch , 0 , 0 , slot + i + 1 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( ch , slot + i + 1 , FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( ch , slot + i + 1 , FM_ARM_Ext_CMSlot[i] , BUFFER_INWAIT_STATUS );
			}
		}
	}
}

void _i_SCH_FMARMMULTI_DATA_Set_MFIC_SIDEWAFER( int slot ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot + i + 1 , FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] );
			_i_SCH_MODULE_Set_MFIC_WAFER( slot + i + 1 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) );
		}
	}
}

void _i_SCH_FMARMMULTI_MFIC_COOLING_SPAWN( int slot ) { // 2007.07.11
	int i;
	for ( i = 0 ; i < FM_ARM_Ext_Count ; i++ ) {
		if ( ( FM_ARM_Ext_Side[i] >= 0 ) && ( FM_ARM_Ext_Pointer[i] >= 0 ) && ( FM_ARM_Ext_CMSlot[i] > 0 ) ) {
			EQUIP_COOLING_SPAWN_FOR_FM_MIC( FM_ARM_Ext_Side[i] , slot + i + 1 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , 0 , _i_SCH_CLUSTER_Get_PathOut( FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] ) , FM_ARM_Ext_Side[i] , FM_ARM_Ext_Pointer[i] , -1 , -1 );
		}
	}
}

//=========================================================================================================================

void _i_SCH_FMARMMULTI_DATA_SET_FROM_FIC( int fms , int ICsts_Real , int k ) {
	int FM_Ext_Loop;
	//========================================================================================
	_i_SCH_FMARMMULTI_DATA_Init();
	//========================================================================================
// 2007.10.24
	for ( FM_Ext_Loop = k ; FM_Ext_Loop < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; FM_Ext_Loop++ ) {
		if ( _i_SCH_MODULE_Get_MFIC_WAFER( ICsts_Real + FM_Ext_Loop + 1 ) > 0 ) {
			_i_SCH_FMARMMULTI_DATA_Append( _i_SCH_MODULE_Get_MFIC_SIDE( ICsts_Real + FM_Ext_Loop + 1 ) , _i_SCH_MODULE_Get_MFIC_POINTER( ICsts_Real + FM_Ext_Loop + 1 ) , _i_SCH_MODULE_Get_MFIC_WAFER( ICsts_Real + FM_Ext_Loop + 1 ) , ICsts_Real + FM_Ext_Loop + 1 ); // 2007.07.11
			_i_SCH_MODULE_Set_MFIC_WAFER( ICsts_Real + FM_Ext_Loop + 1 , 0 );
		}
		else {
			_i_SCH_FMARMMULTI_DATA_Append( -1 , -1 , 0 , 0 ); // 2007.07.11
		}
	}
	//========================================================================================
}

void _i_SCH_FMARMMULTI_DATA_SET_FROM_CM_FDHC( int CHECKING_SIDE , int fms , int FM_Slot_Real , int k ) {
	int FM_Ext_Loop , FM_Ext_Data , FM_Ret_Pointer;
	//========================================================================================
	_i_SCH_MODULE_Enter_FM_DoPointer_Sub(); // 2011.10.25
	//========================================================================================
	_i_SCH_FMARMMULTI_DATA_Init();
	//========================================================================================
	for ( FM_Ext_Loop = k ; FM_Ext_Loop < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; FM_Ext_Loop++ ) {
		FM_Ext_Data = SCHEDULING_CHECK_Increase_for_FM( CHECKING_SIDE , FM_Slot_Real + FM_Ext_Loop + 1 , &FM_Ret_Pointer );
		if ( FM_Ext_Data == 0 ) break;
		if ( FM_Ext_Data == 3 ) {
			FM_Ret_Pointer = -1;
		}
		_i_SCH_FMARMMULTI_DATA_Append( CHECKING_SIDE , FM_Ret_Pointer , FM_Slot_Real + FM_Ext_Loop + 1 , FM_Slot_Real + FM_Ext_Loop + 1 );
		if ( FM_Ext_Data == 2 ) break;
	}
	//========================================================================================
	_i_SCH_MODULE_Leave_FM_DoPointer_Sub(); // 2011.10.25
	//========================================================================================
}

void _i_SCH_FMARMMULTI_DATA_SET_FROM_BM( int fms , int FM_Buffer0 , int FM_Slot_Real0 , int k , int mode ) {
	int FM_Ext_Loop , FM_Buffer , FM_Slot;
	//========================================================================================
	_i_SCH_FMARMMULTI_DATA_Init();
	//========================================================================================
	for ( FM_Ext_Loop = k ; FM_Ext_Loop < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; FM_Ext_Loop++ ) {
		if ( mode == 2 ) {
			FM_Buffer = FM_Buffer0 + FM_Ext_Loop + 1;
			FM_Slot = FM_Slot_Real0;
		}
		else {
			FM_Buffer = FM_Buffer0;
			FM_Slot = FM_Slot_Real0 + FM_Ext_Loop + 1;
		}
		//
		if ( _i_SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot ) <= 0 ) {
			_i_SCH_FMARMMULTI_DATA_Append( -1 , -1 , 0 , 0 );
		}
		else {
			_i_SCH_FMARMMULTI_DATA_Append( _i_SCH_MODULE_Get_BM_SIDE( FM_Buffer , FM_Slot ) , _i_SCH_MODULE_Get_BM_POINTER( FM_Buffer , FM_Slot ) , _i_SCH_MODULE_Get_BM_WAFER( FM_Buffer , FM_Slot ) , FM_Slot );
			if ( mode == 1 ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( FM_Buffer , FM_Slot , 0 , -1 );
			}
		}
	}
}
