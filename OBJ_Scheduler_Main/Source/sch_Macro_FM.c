#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "Equip_Handling.h"
#include "Robot_Handling.h"
#include "User_Parameter.h"
#include "Sch_Prm.h"
#include "sch_CassetteResult.h"
#include "sch_Sub.h"
#include "sch_EIL.h"
#include "Sch_Multi_ALIC.h"
#include "sch_FMARM_Multi.h"
#include "iolog.h"
#include "Multijob.h"
#include "MR_Data.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "FA_Handling.h"

#include "sch_estimate.h" // 2016.10.22

#include "INF_sch_macro.h"
#include "INF_sch_CommonUser.h"
//================================================================================
extern int  _SCH_TAG_BM_LAST_PLACE;
//================================================================================
extern char *_lot_printf_sub_string[][4];
extern char *_lot_printf_sub_string2[][4];
//================================================================================

int	Inf_FM_Result[MAX_FM_CHAMBER_DEPTH];

int	Inf_FM_Move[MAX_FM_CHAMBER_DEPTH];

int	Inf_FM_failres[MAX_FM_CHAMBER_DEPTH];

int	Inf_FM_Slot[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Side[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Pointer[MAX_FM_CHAMBER_DEPTH];

int	Inf_FM_Pick_Mdl[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Pick_MSlot[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Place_Mdl[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Place_MSlot[MAX_FM_CHAMBER_DEPTH];
//
int	Inf_FM_Slot2[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Side2[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Pointer2[MAX_FM_CHAMBER_DEPTH];

int	Inf_FM_Pick_Mdl2[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Pick_MSlot2[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Place_Mdl2[MAX_FM_CHAMBER_DEPTH];
int	Inf_FM_Place_MSlot2[MAX_FM_CHAMBER_DEPTH];

int	Inf_FM_fs[MAX_FM_CHAMBER_DEPTH];

BOOL SCHMULTI_CHECK_HANDOFF_OUT_CHECK( BOOL , int Side , int Pointer , int ch , int );
int Scheduler_Run_Unload_Code( int side , int cm , BOOL famode , BOOL Manual , int mode , int p , int w );

//================================================================================
void _SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( int fms , int mode0 , int sts , int ch , int slot1 , int mslot1 , int side1 , int pointer1 , int slot2 , int mslot2 , int side2 , int pointer2 ) {
	int pathin , xdm1 , xdm2;
	int mode , log; // 2010.09.15
	char MsgSltchar[16]; /* 2013.05.23 */

	mode = mode0 % 10; // 2010.09.15
	log  = ( mode0 / 10 ) * 10; // 2010.09.15

	if ( ( mode < MACRO_PICK ) || ( mode > MACRO_PLACE ) ) return;
	if ( ( sts < FA_SUBST_RUNNING ) || ( sts > FA_SUBST_REJECT ) ) return;
	//
	xdm1 = -1;
	xdm2 = -1;
	//
	if ( slot1 > 0 ) { // 2009.09.18
		if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
			pathin = _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 );
//			xdm1 = SCH_Inside_ThisIs_BM_OtherChamberD( pathin , 1 ); // 2017.09.11
			xdm1 = SCH_Inside_ThisIs_DummyMethod( pathin , 1 , 1 , side1 , pointer1 ); // 2017.09.11
		}
		else {
			xdm1 = 0;
		}
	}
	//
	if ( slot2 > 0 ) { // 2009.09.18
		if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
			pathin = _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 );
//			xdm2 = SCH_Inside_ThisIs_BM_OtherChamberD( pathin , 1 ); // 2017.09.11
			xdm2 = SCH_Inside_ThisIs_DummyMethod( pathin , 1 , 1 , side2 , pointer2 ); // 2017.09.11
		}
		else {
			xdm2 = 0;
		}
	}
	//
	// 2016.10.22
	//
	if      ( mode == MACRO_PICK ) {
		//
		// 2016.10.22
		//
		if ( sts == FA_SUBST_RUNNING ) {
			EST_FM_PICK_START();
			//
			if ( ( ch >= CM1 ) && ( ch < PM1 ) ) EST_CM_PICK_START( ch );
			//
		}
		else if ( sts == FA_SUBST_SUCCESS ) {
			EST_FM_PICK_END();
			//
			if ( ( ch >= CM1 ) && ( ch < PM1 ) ) EST_CM_PICK_END( ch );
			//
		}
		//
	}
	else if ( mode == MACRO_PLACE ) {
		//
		// 2016.10.22
		//
		if ( sts == FA_SUBST_RUNNING ) {
			EST_FM_PLACE_START();
			//
			if ( ( ch >= CM1 ) && ( ch < PM1 ) ) EST_CM_PLACE_START( ch );
			//
		}
		else if ( sts == FA_SUBST_SUCCESS ) {
			EST_FM_PLACE_END();
			//
			if ( ( ch >= CM1 ) && ( ch < PM1 ) ) EST_CM_PLACE_END( ch );
			//
		}
		//
	}
	//
	if ( ch < PM1 ) {
		if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
			if ( ( side1 == side2 ) && ( xdm1 == xdm2 ) ) {
				//
				FA_SIDE_TO_SLOT_GET_L( side1 , pointer1 , MsgSltchar );
				//
				if      ( xdm1 == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(%d,%d)%c%s %s:%d,%d:%d%c%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot1 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot1 , slot2 , log , 9 , (slot2*100)+slot1 , MsgSltchar );
				}
				else if ( xdm1 == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(D%d,D%d)%c%s %s:D%d,D%d:%d%cD%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot1 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot1 , slot2 , log , 9 , (slot2*100)+slot1 , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(D%d-%d,D%d-%d)%c%s %s:D%d,D%d:%d%cD%d-%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xdm1 , slot1 , xdm1 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot1 , slot2 , log , 9 , xdm1 , (slot2*100)+slot1 , MsgSltchar );
				}
				//
				if ( FA_SUBST_REJECT != sts ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side1 , pointer1 , mode + FA_SUBSTRATE_PICK , sts , ch , fms * 100 , 2 , (slot2*100)+slot1 , (slot2*100)+slot1 );
				return;
				//
			}
		}
		//
		if ( slot1 > 0 ) {
			//
			FA_SIDE_TO_SLOT_GET_L( side1 , pointer1 , MsgSltchar );
			//
			if      ( xdm1 == -1 ) {
				_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(A,%d)%c%s %s:%d,0:%d%c%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot1 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot1 , log , 9 , slot1 , MsgSltchar );
			}
			else if ( xdm1 == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(A,D%d)%c%s %s:D%d,0:%d%cD%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot1 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot1 , log , 9 , slot1 , MsgSltchar );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(A,D%d-%d)%c%s %s:D%d,0:%d%cD%d-%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xdm1 , slot1 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot1 , log , 9 , xdm1 , slot1 , MsgSltchar );
			}
			//
			if ( FA_SUBST_REJECT != sts ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side1 , pointer1 , mode + FA_SUBSTRATE_PICK , sts , ch , fms * 100 , 0 , slot1 , slot1 );
		}
		//
		if ( slot2 > 0 ) {
			//
			FA_SIDE_TO_SLOT_GET_L( side2 , pointer2 , MsgSltchar );
			//
			if      ( xdm2 == -1 ) {
				_i_SCH_LOG_LOT_PRINTF( side2 , "FM%s Handling %s %s(B,%d)%c%s %s:0,%d:%d%c%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot2 , log , 9 , slot2 , MsgSltchar );
			}
			else if ( xdm2 == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side2 , "FM%s Handling %s %s(B,D%d)%c%s %s:0,D%d:%d%cD%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot2 , log , 9 , slot2 , MsgSltchar );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side2 , "FM%s Handling %s %s(B,D%d-%d)%c%s %s:0,D%d:%d%cD%d-%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xdm2 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , slot2 , log , 9 , xdm2 , slot2 , MsgSltchar );
			}
			//
			if ( FA_SUBST_REJECT != sts ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side2 , pointer2 , mode + FA_SUBSTRATE_PICK , sts , ch , fms * 100 , 1 , slot2 , slot2 );
		}
	}
	else {
		if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
			if ( ( side1 == side2 ) && ( xdm1 == xdm2 ) ) {
				//
				FA_SIDE_TO_SLOT_GET_L( side1 , pointer1 , MsgSltchar );
				//
				if      ( xdm1 == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(%d:%d,%d:%d)%c%s %s:%d:%d,%d:%d:%d%c%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot1 , slot1 , mslot2 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot1 , slot1 , mslot2 , slot2 , log , 9 , (slot2*100)+slot1 , MsgSltchar );
				}
				else if ( xdm1 == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(%d:D%d,%d:D%d)%c%s %s:%d:D%d,%d:D%d:%d%cD%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot1 , slot1 , mslot2 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot1 , slot1 , mslot2 , slot2 , log , 9 , (slot2*100)+slot1 , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(%d:D%d-%d,%d:D%d-%d)%c%s %s:%d:D%d,%d:D%d:%d%cD%d-%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot1 , xdm1 , slot1 , mslot2 , xdm1 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot1 , slot1 , mslot2 , slot2 , log , 9 , xdm1 , (slot2*100)+slot1 , MsgSltchar );
				}
				//
				if ( FA_SUBST_REJECT != sts ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side1 , pointer1 , mode + FA_SUBSTRATE_PICK , sts , ch , fms * 100 , 2 , (slot2*100)+slot1 , (slot2*100)+slot1 );
				return;
				//
			}
		}
		//
		if ( slot1 > 0 ) {
			//
			FA_SIDE_TO_SLOT_GET_L( side1 , pointer1 , MsgSltchar );
			//
			if      ( xdm1 == -1 ) {
				_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(A,%d:%d)%c%s %s:%d:%d,0:0:%d%c%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot1 , slot1 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot1 , slot1 , log , 9 , slot1 , MsgSltchar );
			}
			else if ( xdm1 == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(A,%d:D%d)%c%s %s:%d:D%d,0:0:%d%cD%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot1 , slot1 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot1 , slot1 , log , 9 , slot1 , MsgSltchar );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side1 , "FM%s Handling %s %s(A,%d:D%d-%d)%c%s %s:%d:D%d,0:0:%d%cD%d-%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot1 , xdm1 , slot1 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot1 , slot1 , log , 9 , xdm1 , slot1 , MsgSltchar );
			}
			//
			if ( FA_SUBST_REJECT != sts ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side1 , pointer1 , mode + FA_SUBSTRATE_PICK , sts , ch , fms * 100 , 0 , slot1 , slot1 );
		}
		//
		if ( slot2 > 0 ) {
			//
			FA_SIDE_TO_SLOT_GET_L( side2 , pointer2 , MsgSltchar );
			//
			if      ( xdm2 == -1 ) {
				_i_SCH_LOG_LOT_PRINTF( side2 , "FM%s Handling %s %s(B,%d:%d)%c%s %s:0:0,%d:%d:%d%c%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot2 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot2 , slot2 , log , 9 , slot2 , MsgSltchar );
			}
			else if ( xdm2 == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side2 , "FM%s Handling %s %s(B,%d:D%d)%c%s %s:0:0,%d:D%d:%d%cD%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot2 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot2 , slot2 , log , 9 , slot2 , MsgSltchar );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side2 , "FM%s Handling %s %s(B,%d:D%d-%d)%c%s %s:0:0,%d:D%d:%d%cD%d-%d%s\n" , MULMDLSTR[fms] , _lot_printf_sub_string[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , mslot2 , xdm2 , slot2 , 9 , _lot_printf_sub_string2[mode][sts] , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mslot2 , slot2 , log , 9 , xdm2 , slot2 , MsgSltchar );
			}
			//
			if ( FA_SUBST_REJECT != sts ) _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( side2 , pointer2 , mode + FA_SUBSTRATE_PICK , sts , ch , fms * 100 , 1 , slot2 , slot2 );
		}
	}
}

/*
//
//
//
// 2018.06.14
//
//
int _SCH_MACRO_FM_OPERATION_SUB( int fms , int mode0 , int ch1 , int mch1 , int slot1 , int omslot1 , int side1 , int pointer1 , int ch2 , int mch2 , int slot2 , int omslot2 , int side2 , int pointer2 , int fs ) {
	int fmcm , dumc;
	int mslot1 , mslot2 , rslot1 , rslot2 , same;
	int mode , log; // 2010.09.15
	int es1 , es2;

	mode = mode0 % 10; // 2010.09.15
	log  = ( mode0 / 10 ) * 10; // 2010.09.15

	//-----------------------------------------------------------
	_SCH_TAG_BM_LAST_PLACE = 0; // 2008.03.24
	//-----------------------------------------------------------
	if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
		if ( ( ch1 == -1 ) && ( ch2 == -1 ) ) {
			if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
				if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 ) == _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 ) ) {
						same = TRUE;
					}
					else {
						same = FALSE;
					}
				}
				else {
					same = FALSE;
				}
			}
			else {
				same = FALSE;
			}
		}
		else {
			if ( ( ch1 == ch2 ) && ( ( ch1 < BM1 ) || !_i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() ) ) {
				same = TRUE;
			}
			else {
				same = FALSE;
			}
		}
	}
	else {
		same = FALSE;
	}
//
//	if ( ( slot1 > 0 ) && ( slot2 > 0 ) && ( ch1 == ch2 ) ) { // 2007.11.23
//	if ( ( slot1 > 0 ) && ( slot2 > 0 ) && ( ch1 == ch2 ) && ( ( ch1 < BM1 ) || !_i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() ) ) { // 2007.11.23 // 2007.11.29
	if ( same ) { // 2007.11.29
		if ( omslot1 > 10000 ) {
			rslot1 = omslot1 / 10000;
			mslot1 = omslot1 % 10000;
		}
		else {
			rslot1 = omslot1;
			mslot1 = omslot1;
		}
		if ( omslot2 > 10000 ) {
			rslot2 = omslot2 / 10000;
			mslot2 = omslot2 % 10000;
		}
		else {
			rslot2 = omslot2;
			mslot2 = omslot2;
		}
		//
		if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) )
			fmcm = _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 );
		else
			fmcm = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
		//
		if ( ch1 == -1 ) {
			ch1 = fmcm;
			//
			if ( ch1 >= PM1 ) { // 2008.11.22
				dumc = 0;
				if ( SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) dumc = ch1 - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); // 2009.02.12
				//
				_i_SCH_IO_SET_MODULE_SOURCE( ch1 , mslot1 , MAX_CASSETTE_SIDE + 1 + dumc );
				_i_SCH_IO_SET_MODULE_STATUS( ch1 , mslot1 , mslot1 );
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch1 , mslot1 , ch1 , mslot1 , -1 , side1 , pointer1 );
				//
				_i_SCH_IO_SET_MODULE_SOURCE( ch1 , mslot2 , MAX_CASSETTE_SIDE + 1 + dumc );
				_i_SCH_IO_SET_MODULE_STATUS( ch1 , mslot2 , mslot2 );
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch1 , mslot2 , ch1 , mslot2 , -1 , side2 , pointer2 ); // 2008.11.22
			}
			//
		}
		//
		if ( mch1 < 0 ) mch1 = fmcm;
		if ( mch2 < 0 ) mch2 = fmcm;
		//
		if ( mch1 != mch2 ) {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
		}
		else {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
		}
		if ( mode == MACRO_PICK ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2008.01.04
					if ( EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , mslot2 , fmcm , fmcm , ( mslot1 > 0 ) && ( mslot2 > 0 ) ? FAL_RUN_MODE_PICK_ALL : FAL_RUN_MODE_PICK , FALSE , slot1 , slot2 , side1 , side2 , pointer1 , pointer2 , FALSE ) == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
				}
				else {
					if ( EQUIP_PICK_FROM_FMBM( fms , side1 , ch1 , rslot1 , rslot2 , ( fs == -1 ) ? TRUE : fs , fmcm , 0 , FALSE ) == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
				}
			}
			else {
				//=======================================================================================
				// 2010.01.28
				//=======================================================================================
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
					return -1;
				}
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side2 , pointer2 , ch1 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
					return -1;
				}
				//=======================================================================================
			}
			//
			if ( ch1 < PM1 ) _i_SCH_IO_MTL_ADD_PICK_COUNT( ch1 , 2 );
			//
		}
		else if ( mode == MACRO_PLACE ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2008.01.04
					if ( EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , mslot2 , fmcm , fmcm , ( mslot1 > 0 ) && ( mslot2 > 0 ) ? FAL_RUN_MODE_PLACE_ALL : FAL_RUN_MODE_PLACE , FALSE , slot1 , slot2 , side1 , side2 , pointer1 , pointer2 , FALSE ) == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
				}
				else {
					if ( EQUIP_PLACE_TO_FMBM( fms , side1 , ch1 , rslot1 , rslot2 , ( fs == -1 ) ? TRUE : fs , fmcm , 0 , FALSE ) == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
				}
			}
			else {
				//=======================================================================================
				// 2010.01.28
				//=======================================================================================
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
					return -1;
				}
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side2 , pointer2 , ch1 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
					return -1;
				}
				//=======================================================================================
			}
			//
			if ( ch1 < PM1 ) {
				if ( side1 != side2 ) {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) != PATHDO_WAFER_RETURN ) , side1 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side1 , 1 );
					}
					if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) != PATHDO_WAFER_RETURN ) , side2 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side2 , 1 );
					}
				}
				else {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) != PATHDO_WAFER_RETURN ) , side1 , 2 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side1 , 2 );
					}
				}
				//====================================================================================================================================================
				_i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( ch1 );
				//====================================================================================================================================================
			}
			else if ( ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
				_SCH_TAG_BM_LAST_PLACE = ch1; // 2008.03.24
			}
		}
		else {
			return -1;
		}
		if ( mch1 != mch2 ) {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
		}
		else {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
		}
		//====================================================================================================================================================
		//====================================================================================================================================================
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.11
			//
			if ( ( ch1 >= BM1 ) && ( ch1 < TM ) && ( !SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) ) {
				es1 = slot1;
			}
			else {
				es1 = 0;
			}
			//
			if ( ( ch2 >= BM1 ) && ( ch2 < TM ) && ( !SCH_Inside_ThisIs_BM_OtherChamber( ch2 , 1 ) ) ) {
				es2 = slot2;
			}
			else {
				es2 = 0;
			}
			//
			if ( ( es1 > 0 ) || ( es2 > 0 ) ) {
				if ( !_EIL_FMMODE_MACRO_FM_OPERATION( fms , mode , ch1 , es1 , rslot1 , side1 , pointer1 , ch2 , es2 , rslot2 , side2 , pointer2 ) ) {
					return -1;
				}
			}
			//
		}
		//====================================================================================================================================================
		//====================================================================================================================================================
		if ( mode == MACRO_PLACE ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				if ( ch1 < PM1 ) {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) == PATHDO_WAFER_RETURN );
					}
					else {
						_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( FALSE );
					}
				}
			}
		}
		else { // 2011.04.14
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
					if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side1 , pointer1 , ch1 , slot1 ) ) return -1;
					if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side2 , pointer2 , ch1 , slot2 ) ) return -1;
				}
			}
			//
		}
		//====================================================================================================================================================
		return 2;
	}
	else {
		if ( slot1 > 0 ) {
			if ( omslot1 > 10000 ) {
				rslot1 = omslot1 / 10000;
				mslot1 = omslot1 % 10000;
			}
			else {
				rslot1 = omslot1;
				mslot1 = omslot1;
			}
			//
			if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) )
				fmcm = _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 );
			else
				fmcm = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
			//
			if ( ch1 == -1 ) {
				ch1 = fmcm;
				//
				if ( ch1 >= PM1 ) { // 2008.11.22
					dumc = 0;
					if ( SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) dumc = ch1 - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); // 2009.02.12
					//
					_i_SCH_IO_SET_MODULE_SOURCE( ch1 , mslot1 , MAX_CASSETTE_SIDE + 1 + dumc );
					_i_SCH_IO_SET_MODULE_STATUS( ch1 , mslot1 , mslot1 );
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch1 , mslot1 , ch1 , mslot1 , -1 , side1 , pointer1 );
				}
				//
			}
			if ( mch1 <  0 ) mch1 = fmcm;
			//
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			if ( mode == MACRO_PICK ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2009.01.16
						if ( EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , 0 , fmcm , fmcm , FAL_RUN_MODE_PICK , FALSE , slot1 , 0 , side1 , 0 , pointer1 , 0 , FALSE ) == SYS_ABORTED ) { // 2009.01.16
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
					}
					else {
						if ( EQUIP_PICK_FROM_FMBM( fms , side1 , ch1 , rslot1 , 0 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
						return -1;
					}
					//=======================================================================================
				}
				//
				if ( ch1 < PM1 ) _i_SCH_IO_MTL_ADD_PICK_COUNT( ch1 , 1 );
			}
			else if ( mode == MACRO_PLACE ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2009.01.16
						if ( EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , 0 , fmcm , fmcm , FAL_RUN_MODE_PLACE , FALSE , slot1 , 0 , side1 , 0 , pointer1 , 0 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
					}
					else {
						if ( EQUIP_PLACE_TO_FMBM( fms , side1 , ch1 , rslot1 , 0 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
						return -1;
					}
					//=======================================================================================
				}
				//
				if ( ch1 < PM1 ) {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) != PATHDO_WAFER_RETURN ) , side1 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side1 , 1 );
					}
					//====================================================================================================================================================
					_i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( ch1 );
					//====================================================================================================================================================
				}
				else if ( ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
					_SCH_TAG_BM_LAST_PLACE = ch1; // 2008.03.24
				}
			}
			else {
				return -1;
			}
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			//====================================================================================================================================================
			//====================================================================================================================================================
			//====================================================================================================================================================
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.11
				//
				if ( ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
					if ( !SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) {
						//
						if ( !_EIL_FMMODE_MACRO_FM_OPERATION( fms , mode , ch1 , slot1 , rslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 , 0 ) ) {
							return -1;
						}
						//
					}
				}
			}
			//====================================================================================================================================================
			//====================================================================================================================================================
			if ( mode == MACRO_PLACE ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ch1 < PM1 ) {
						if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
							_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) == PATHDO_WAFER_RETURN );
						}
						else {
							_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( FALSE );
						}
					}
				}
			}
			//====================================================================================================================================================
		}
		if ( slot2 > 0 ) {
			if ( omslot2 > 10000 ) {
				rslot2 = omslot2 / 10000;
				mslot2 = omslot2 % 10000;
			}
			else {
				rslot2 = omslot2;
				mslot2 = omslot2;
			}
			//
			if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) )
				fmcm = _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 );
			else
				fmcm = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
			//
			if ( ch2 == -1 ) {
				ch2 = fmcm;
				//
				if ( ch2 >= PM1 ) { // 2008.11.22
					dumc = 0;
					if ( SCH_Inside_ThisIs_BM_OtherChamber( ch2 , 1 ) ) dumc = ch2 - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); // 2009.02.12
					//
					_i_SCH_IO_SET_MODULE_SOURCE( ch2 , mslot2 , MAX_CASSETTE_SIDE + 1 + dumc );
					_i_SCH_IO_SET_MODULE_STATUS( ch2 , mslot2 , mslot2 );
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch2 , mslot2 , ch2 , mslot2 , -1 , side2 , pointer2 );
				}
				//
			}
			if ( mch2 <  0 ) mch2 = fmcm;
			//
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
			if ( mode == MACRO_PICK ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch2 == F_AL ) || ( ch2 == F_IC ) ) { // 2009.01.16
						if ( EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side2 , ( ch2 == F_AL ) ? AL : IC , 0 , mslot2 , fmcm , fmcm , FAL_RUN_MODE_PICK , FALSE , 0 , slot2 , 0 , side2 , 0 , pointer2 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
					}
					else {
						if ( EQUIP_PICK_FROM_FMBM( fms , side2 , ch2 , 0 , rslot2 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side2 , pointer2 , ch2 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
						return -1;
					}
					//=======================================================================================
				}
				//
				if ( ch2 < PM1 ) _i_SCH_IO_MTL_ADD_PICK_COUNT( ch2 , 1 );
			}
			else if ( mode == MACRO_PLACE ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch2 == F_AL ) || ( ch2 == F_IC ) ) { // 2009.01.16
						if ( EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side2 , ( ch2 == F_AL ) ? AL : IC , 0 , mslot2 , fmcm , fmcm , FAL_RUN_MODE_PLACE , FALSE , 0 , slot2 , 0 , side2 , 0 , pointer2 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
					}
					else {
						if ( EQUIP_PLACE_TO_FMBM( fms , side2 , ch2 , 0 , rslot2 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE ) == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side2 , pointer2 , ch2 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 ) == SYS_ABORTED ) {
						return -1;
					}
					//=======================================================================================
				}
				//
				if ( ch2 < PM1 ) {
					if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch2 , ( _i_SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) != PATHDO_WAFER_RETURN ) , side2 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch2 , TRUE , side2 , 1 );
					}
				}
				else if ( ( ch2 >= BM1 ) && ( ch2 < TM ) ) {
					_SCH_TAG_BM_LAST_PLACE = ch2; // 2008.03.24
				}
			}
			else {
				return -1;
			}
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
			//====================================================================================================================================================
			//====================================================================================================================================================
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.11
				//
				if ( ( ch2 >= BM1 ) && ( ch2 < TM ) ) {
					if ( !SCH_Inside_ThisIs_BM_OtherChamber( ch2 , 1 ) ) {
						if ( !_EIL_FMMODE_MACRO_FM_OPERATION( fms , mode , 0 , 0 , 0 , 0 , 0 , ch2 , slot2 , rslot2 , side2 , pointer2 ) ) {
							return -1;
						}
					}
				}
			}
			//====================================================================================================================================================
			//====================================================================================================================================================
		}
		//====================================================================================================================================================
		//====================================================================================================================================================
		if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
			//
			if ( mode == MACRO_PICK ) { // 2011.04.14
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side1 , pointer1 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side1 , pointer1 , ch1 , slot1 ) ) return -1;
					}
					if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side2 , pointer2 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side2 , pointer2 , ch2 , slot2 ) ) return -1;
					}
				}
			}
			//
			return 3;
		}
		if ( slot1 > 0 ) {
			//
			if ( mode == MACRO_PICK ) { // 2011.04.14
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side1 , pointer1 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side1 , pointer1 , ch1 , slot1 ) ) return -1;
					}
				}
			}
			//
			return 0;
		}
		if ( slot2 > 0 ) {
			//
			if ( mode == MACRO_PICK ) { // 2011.04.14
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side2 , pointer2 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side2 , pointer2 , ch2 , slot2 ) ) return -1;
					}
				}
			}
			//
			return 1;
		}
	}
	return -1;
}
//
*/

//
// 2018.06.14
//
//
int _SCH_MACRO_FM_OPERATION_SUB( int fms , int mode0 , int ch1 , int mch1 , int slot1 , int omslot1 , int side1 , int pointer1 , int ch2 , int mch2 , int slot2 , int omslot2 , int side2 , int pointer2 , int fs ) {
	int fmcm , dumc;
	int mslot1 , mslot2 , rslot1 , rslot2 , same;
	int mode , log; // 2010.09.15
	int es1 , es2;
	int Res; // 2018.06.14

	mode = mode0 % 10; // 2010.09.15
	log  = ( mode0 / 10 ) * 10; // 2010.09.15

	//-----------------------------------------------------------
	_SCH_TAG_BM_LAST_PLACE = 0; // 2008.03.24
	//-----------------------------------------------------------
	if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
		if ( ( ch1 == -1 ) && ( ch2 == -1 ) ) {
			if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
				if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
					if ( _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 ) == _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 ) ) {
						same = TRUE;
					}
					else {
						same = FALSE;
					}
				}
				else {
					same = FALSE;
				}
			}
			else {
				same = FALSE;
			}
		}
		else {
			if ( ( ch1 == ch2 ) && ( ( ch1 < BM1 ) || !_i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() ) ) {
				same = TRUE;
			}
			else {
				same = FALSE;
			}
		}
	}
	else {
		same = FALSE;
	}
//
	if ( same ) { // 2007.11.29
		if ( omslot1 > 10000 ) {
			rslot1 = omslot1 / 10000;
			mslot1 = omslot1 % 10000;
		}
		else {
			rslot1 = omslot1;
			mslot1 = omslot1;
		}
		if ( omslot2 > 10000 ) {
			rslot2 = omslot2 / 10000;
			mslot2 = omslot2 % 10000;
		}
		else {
			rslot2 = omslot2;
			mslot2 = omslot2;
		}
		//
		if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) )
			fmcm = _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 );
		else
			fmcm = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
		//
		if ( ch1 == -1 ) {
			ch1 = fmcm;
			//
			if ( ch1 >= PM1 ) { // 2008.11.22
				dumc = 0;
				if ( SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) dumc = ch1 - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); // 2009.02.12
				//
				_i_SCH_IO_SET_MODULE_SOURCE( ch1 , mslot1 , MAX_CASSETTE_SIDE + 1 + dumc );
				_i_SCH_IO_SET_MODULE_STATUS( ch1 , mslot1 , mslot1 );
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch1 , mslot1 , ch1 , mslot1 , -1 , side1 , pointer1 );
				//
				_i_SCH_IO_SET_MODULE_SOURCE( ch1 , mslot2 , MAX_CASSETTE_SIDE + 1 + dumc );
				_i_SCH_IO_SET_MODULE_STATUS( ch1 , mslot2 , mslot2 );
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch1 , mslot2 , ch1 , mslot2 , -1 , side2 , pointer2 ); // 2008.11.22
			}
			//
		}
		//
		if ( mch1 < 0 ) mch1 = fmcm;
		if ( mch2 < 0 ) mch2 = fmcm;
		//
		if ( mch1 != mch2 ) {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
		}
		else {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
		}
		if ( mode == MACRO_PICK ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2008.01.04
					//
					Res = EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , mslot2 , fmcm , fmcm , ( mslot1 > 0 ) && ( mslot2 > 0 ) ? FAL_RUN_MODE_PICK_ALL : FAL_RUN_MODE_PICK , FALSE , slot1 , slot2 , side1 , side2 , pointer1 , pointer2 , FALSE );
					//
					if ( Res == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return 4;
					}
				}
				else {
					//
					Res = EQUIP_PICK_FROM_FMBM( fms , side1 , ch1 , rslot1 , rslot2 , ( fs == -1 ) ? TRUE : fs , fmcm , 0 , FALSE );
					//
					if ( Res == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return 4;
					}
				}
			}
			else {
				//=======================================================================================
				// 2010.01.28
				//=======================================================================================
				Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 );
				//
				if ( Res == SYS_ABORTED ) {
					return -1;
				}
				//
				if ( Res == SYS_ERROR ) {
					return 4;
				}
				//
				Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side2 , pointer2 , ch1 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 );
				//
				if ( Res == SYS_ABORTED ) {
					return -1;
				}
				//
				if ( Res == SYS_ERROR ) {
					return 4;
				}
				//
				//=======================================================================================
			}
			//
			if ( ch1 < PM1 ) _i_SCH_IO_MTL_ADD_PICK_COUNT( ch1 , 2 );
			//
		}
		else if ( mode == MACRO_PLACE ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2008.01.04
					//
					Res = EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , mslot2 , fmcm , fmcm , ( mslot1 > 0 ) && ( mslot2 > 0 ) ? FAL_RUN_MODE_PLACE_ALL : FAL_RUN_MODE_PLACE , FALSE , slot1 , slot2 , side1 , side2 , pointer1 , pointer2 , FALSE );
					//
					if ( Res == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return 4;
					}
				}
				else {
					//
					Res = EQUIP_PLACE_TO_FMBM( fms , side1 , ch1 , rslot1 , rslot2 , ( fs == -1 ) ? TRUE : fs , fmcm , 0 , FALSE );
					//
					if ( Res == SYS_ABORTED ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						if ( mch1 != mch2 ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
						}
						else {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
						}
						return 4;
					}
				}
			}
			else {
				//=======================================================================================
				// 2010.01.28
				//=======================================================================================
				Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 );
				//
				if ( Res == SYS_ABORTED ) {
					return -1;
				}
				//
				if ( Res == SYS_ERROR ) {
					return 4;
				}
				//
				//
				Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side2 , pointer2 , ch1 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 );
				//
				if ( Res == SYS_ABORTED ) {
					return -1;
				}
				//
				if ( Res == SYS_ERROR ) {
					return 4;
				}
				//
				//=======================================================================================
			}
			//
			if ( ch1 < PM1 ) {
				if ( side1 != side2 ) {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) != PATHDO_WAFER_RETURN ) , side1 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side1 , 1 );
					}
					if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) != PATHDO_WAFER_RETURN ) , side2 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side2 , 1 );
					}
				}
				else {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) != PATHDO_WAFER_RETURN ) , side1 , 2 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side1 , 2 );
					}
				}
				//====================================================================================================================================================
				_i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( ch1 );
				//====================================================================================================================================================
			}
			else if ( ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
				_SCH_TAG_BM_LAST_PLACE = ch1; // 2008.03.24
			}
		}
		else {
			return -1;
		}
		if ( mch1 != mch2 ) {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
		}
		else {
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch1 , slot1 , mslot1 , side1 , pointer1 , slot2 , mslot2 , side2 , pointer2 );
		}
		//====================================================================================================================================================
		//====================================================================================================================================================
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.11
			//
			if ( ( ch1 >= BM1 ) && ( ch1 < TM ) && ( !SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) ) {
				es1 = slot1;
			}
			else {
				es1 = 0;
			}
			//
			if ( ( ch2 >= BM1 ) && ( ch2 < TM ) && ( !SCH_Inside_ThisIs_BM_OtherChamber( ch2 , 1 ) ) ) {
				es2 = slot2;
			}
			else {
				es2 = 0;
			}
			//
			if ( ( es1 > 0 ) || ( es2 > 0 ) ) {
				if ( !_EIL_FMMODE_MACRO_FM_OPERATION( fms , mode , ch1 , es1 , rslot1 , side1 , pointer1 , ch2 , es2 , rslot2 , side2 , pointer2 ) ) {
					return -1;
				}
			}
			//
		}
		//====================================================================================================================================================
		//====================================================================================================================================================
		if ( mode == MACRO_PLACE ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				if ( ch1 < PM1 ) {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) == PATHDO_WAFER_RETURN );
					}
					else {
						_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( FALSE );
					}
				}
			}
		}
		else { // 2011.04.14
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
					if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side1 , pointer1 , ch1 , slot1 ) ) return -1;
					if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side2 , pointer2 , ch1 , slot2 ) ) return -1;
				}
			}
			//
		}
		//====================================================================================================================================================
		return 2;
	}
	else {
		if ( slot1 > 0 ) {
			if ( omslot1 > 10000 ) {
				rslot1 = omslot1 / 10000;
				mslot1 = omslot1 % 10000;
			}
			else {
				rslot1 = omslot1;
				mslot1 = omslot1;
			}
			//
			if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) )
				fmcm = _i_SCH_CLUSTER_Get_PathIn( side1 , pointer1 );
			else
				fmcm = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
			//
			if ( ch1 == -1 ) {
				ch1 = fmcm;
				//
				if ( ch1 >= PM1 ) { // 2008.11.22
					dumc = 0;
					if ( SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) dumc = ch1 - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); // 2009.02.12
					//
					_i_SCH_IO_SET_MODULE_SOURCE( ch1 , mslot1 , MAX_CASSETTE_SIDE + 1 + dumc );
					_i_SCH_IO_SET_MODULE_STATUS( ch1 , mslot1 , mslot1 );
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch1 , mslot1 , ch1 , mslot1 , -1 , side1 , pointer1 );
				}
				//
			}
			if ( mch1 <  0 ) mch1 = fmcm;
			//
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			if ( mode == MACRO_PICK ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2009.01.16
						//
						Res = EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , 0 , fmcm , fmcm , FAL_RUN_MODE_PICK , FALSE , slot1 , 0 , side1 , 0 , pointer1 , 0 , FALSE );
						//
						if ( Res == SYS_ABORTED ) { // 2009.01.16
							//
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							//
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return 4;
						}
					}
					else {
						//
						Res = EQUIP_PICK_FROM_FMBM( fms , side1 , ch1 , rslot1 , 0 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return 4;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 );
					//
					if ( Res == SYS_ABORTED ) {
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						return 4;
					}
					//=======================================================================================
				}
				//
				if ( ch1 < PM1 ) _i_SCH_IO_MTL_ADD_PICK_COUNT( ch1 , 1 );
			}
			else if ( mode == MACRO_PLACE ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch1 == F_AL ) || ( ch1 == F_IC ) ) { // 2009.01.16
						//
						Res = EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side1 , ( ch1 == F_AL ) ? AL : IC , mslot1 , 0 , fmcm , fmcm , FAL_RUN_MODE_PLACE , FALSE , slot1 , 0 , side1 , 0 , pointer1 , 0 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return 4;
						}
					}
					else {
						//
						Res = EQUIP_PLACE_TO_FMBM( fms , side1 , ch1 , rslot1 , 0 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
							return 4;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side1 , pointer1 , ch1 , 0 , slot1 , rslot1 , 0 , fmcm , FALSE , 0 , -1 , -1 );
					//
					if ( Res == SYS_ABORTED ) {
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						return 4;
					}
					//=======================================================================================
				}
				//
				if ( ch1 < PM1 ) {
					if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , ( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) != PATHDO_WAFER_RETURN ) , side1 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch1 , TRUE , side1 , 1 );
					}
					//====================================================================================================================================================
					_i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( ch1 );
					//====================================================================================================================================================
				}
				else if ( ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
					_SCH_TAG_BM_LAST_PLACE = ch1; // 2008.03.24
				}
			}
			else {
				return -1;
			}
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch1 , slot1 , mslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 );
			//====================================================================================================================================================
			//====================================================================================================================================================
			//====================================================================================================================================================
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.11
				//
				if ( ( ch1 >= BM1 ) && ( ch1 < TM ) ) {
					if ( !SCH_Inside_ThisIs_BM_OtherChamber( ch1 , 1 ) ) {
						//
						if ( !_EIL_FMMODE_MACRO_FM_OPERATION( fms , mode , ch1 , slot1 , rslot1 , side1 , pointer1 , 0 , 0 , 0 , 0 , 0 ) ) {
							return -1;
						}
						//
					}
				}
			}
			//====================================================================================================================================================
			//====================================================================================================================================================
			if ( mode == MACRO_PLACE ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ch1 < PM1 ) {
						if ( ( ( side1 >= 0 ) && ( side1 < MAX_CASSETTE_SIDE ) && ( pointer1 >= 0 ) && ( pointer1 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
							_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( side1 , pointer1 ) == PATHDO_WAFER_RETURN );
						}
						else {
							_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( FALSE );
						}
					}
				}
			}
			//====================================================================================================================================================
		}
		if ( slot2 > 0 ) {
			if ( omslot2 > 10000 ) {
				rslot2 = omslot2 / 10000;
				mslot2 = omslot2 % 10000;
			}
			else {
				rslot2 = omslot2;
				mslot2 = omslot2;
			}
			//
			if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) )
				fmcm = _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 );
			else
				fmcm = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
			//
			if ( ch2 == -1 ) {
				ch2 = fmcm;
				//
				if ( ch2 >= PM1 ) { // 2008.11.22
					dumc = 0;
					if ( SCH_Inside_ThisIs_BM_OtherChamber( ch2 , 1 ) ) dumc = ch2 - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER(); // 2009.02.12
					//
					_i_SCH_IO_SET_MODULE_SOURCE( ch2 , mslot2 , MAX_CASSETTE_SIDE + 1 + dumc );
					_i_SCH_IO_SET_MODULE_STATUS( ch2 , mslot2 , mslot2 );
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( ch2 , mslot2 , ch2 , mslot2 , -1 , side2 , pointer2 );
				}
				//
			}
			if ( mch2 <  0 ) mch2 = fmcm;
			//
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_RUNNING , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
			if ( mode == MACRO_PICK ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch2 == F_AL ) || ( ch2 == F_IC ) ) { // 2009.01.16
						//
						Res = EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side2 , ( ch2 == F_AL ) ? AL : IC , 0 , mslot2 , fmcm , fmcm , FAL_RUN_MODE_PICK , FALSE , 0 , slot2 , 0 , side2 , 0 , pointer2 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return 4;
						}
					}
					else {
						//
						Res = EQUIP_PICK_FROM_FMBM( fms , side2 , ch2 , 0 , rslot2 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return 4;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PICK , side2 , pointer2 , ch2 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 );
					//
					if ( Res == SYS_ABORTED ) {
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						return 4;
					}
					//=======================================================================================
				}
				//
				if ( ch2 < PM1 ) _i_SCH_IO_MTL_ADD_PICK_COUNT( ch2 , 1 );
			}
			else if ( mode == MACRO_PLACE ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
					if ( ( ch2 == F_AL ) || ( ch2 == F_IC ) ) { // 2009.01.16
						//
						Res = EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side2 , ( ch2 == F_AL ) ? AL : IC , 0 , mslot2 , fmcm , fmcm , FAL_RUN_MODE_PLACE , FALSE , 0 , slot2 , 0 , side2 , 0 , pointer2 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return 4;
						}
					}
					else {
						//
						Res = EQUIP_PLACE_TO_FMBM( fms , side2 , ch2 , 0 , rslot2 , ( fs == -1 ) ? FALSE : fs , fmcm , 0 , FALSE );
						//
						if ( Res == SYS_ABORTED ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_FAIL , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return -1;
						}
						//
						if ( Res == SYS_ERROR ) {
							_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_REJECT , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
							return 4;
						}
					}
				}
				else { // 2010.01.28
					//=======================================================================================
					// 2010.01.28
					//=======================================================================================
					Res = _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , side2 , pointer2 , ch2 , 1 , slot2 , rslot2 , 0 , fmcm , FALSE , 0 , -1 , -1 );
					//
					if ( Res == SYS_ABORTED ) {
						return -1;
					}
					//
					if ( Res == SYS_ERROR ) {
						return 4;
					}
					//=======================================================================================
				}
				//
				if ( ch2 < PM1 ) {
					if ( ( ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) ) {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch2 , ( _i_SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) != PATHDO_WAFER_RETURN ) , side2 , 1 );
					}
					else {
						_i_SCH_IO_MTL_ADD_PLACE_COUNT( ch2 , TRUE , side2 , 1 );
					}
				}
				else if ( ( ch2 >= BM1 ) && ( ch2 < TM ) ) {
					_SCH_TAG_BM_LAST_PLACE = ch2; // 2008.03.24
				}
			}
			else {
				return -1;
			}
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( fms , mode0 , FA_SUBST_SUCCESS , mch2 , 0 , 0 , 0 , 0 , slot2 , mslot2 , side2 , pointer2 );
			//====================================================================================================================================================
			//====================================================================================================================================================
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.11
				//
				if ( ( ch2 >= BM1 ) && ( ch2 < TM ) ) {
					if ( !SCH_Inside_ThisIs_BM_OtherChamber( ch2 , 1 ) ) {
						if ( !_EIL_FMMODE_MACRO_FM_OPERATION( fms , mode , 0 , 0 , 0 , 0 , 0 , ch2 , slot2 , rslot2 , side2 , pointer2 ) ) {
							return -1;
						}
					}
				}
			}
			//====================================================================================================================================================
			//====================================================================================================================================================
		}
		//====================================================================================================================================================
		//====================================================================================================================================================
		if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
			//
			if ( mode == MACRO_PICK ) { // 2011.04.14
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side1 , pointer1 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side1 , pointer1 , ch1 , slot1 ) ) return -1;
					}
					if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side2 , pointer2 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side2 , pointer2 , ch2 , slot2 ) ) return -1;
					}
				}
			}
			//
			return 3;
		}
		if ( slot1 > 0 ) {
			//
			if ( mode == MACRO_PICK ) { // 2011.04.14
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side1 , pointer1 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side1 , pointer1 , ch1 , slot1 ) ) return -1;
					}
				}
			}
			//
			return 0;
		}
		if ( slot2 > 0 ) {
			//
			if ( mode == MACRO_PICK ) { // 2011.04.14
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) {
						//
						SCHMRDATA_Data_IN_Unload_Request_After_Pick( side2 , pointer2 , ch1 ); // 2016.11.02
						//
						if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , side2 , pointer2 , ch2 , slot2 ) ) return -1;
					}
				}
			}
			//
			return 1;
		}
	}
	return -1;
}

int _SCH_MACRO_FM_OPERATION( int fms , int mode , int och1 , int slot1 , int omslot1 , int side1 , int pointer1 , int och2 , int slot2 , int omslot2 , int side2 , int pointer2 , int fs ) {
	int Res;
	int ch1 , ch2 , mch1 , mch2;
	//
	//=================================================================================================
	if ( slot1 > 0 ) {
//		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side1 ) ) return -1; // 2012.07.12 // 2016.11.04
		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side1 , pointer1 ) ) return -1; // 2012.07.12 // 2016.11.04
	}
	if ( slot2 > 0 ) {
//		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side2 ) ) return -1; // 2012.07.12 // 2016.11.04
		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side2 , pointer2 ) ) return -1; // 2012.07.12 // 2016.11.04
	}
	//=================================================================================================
	//
	_SCH_COMMON_MESSAGE_CONTROL_FOR_USER( FALSE , 11 , fms , mode , och1 , slot1 , side1 , pointer1 , och2 , slot2 , side2 , pointer2 , 0.0 , 0.0 , 0.0 , NULL , NULL , NULL ); // 2016.07.28
	//
	//=================================================================================================
	if ( och1 > 10000 ) {
		mch1 = och1 / 10000;
		ch1 = och1 % 10000;
	}
	else {
		mch1 = och1;
		ch1 = och1;
	}
	if ( och2 > 10000 ) {
		mch2 = och2 / 10000;
		ch2 = och2 % 10000;
	}
	else {
		mch2 = och2;
		ch2 = och2;
	}
	//=================================================================================================
	// 2012.04.01
//	if ( mode == MACRO_PICK ) { // 2016.06.07
	if ( ( mode % 10 ) == MACRO_PICK ) { // 2016.06.07
		if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PICK( ch1 - CM1 , TRUE );
		if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PICK( ch2 - CM1 , TRUE );
	}
//	else if ( mode == MACRO_PLACE ) { // 2016.06.07
	else if ( ( mode % 10 ) == MACRO_PLACE ) { // 2016.06.07
		if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( ch1 - CM1 , TRUE );
		if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( ch2 - CM1 , TRUE );
	}
	//=================================================================================================
	//-----------------------------------------------------------
	EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
	//-----------------------------------------------------------
	Res = _SCH_MACRO_FM_OPERATION_SUB( fms , mode , ch1 , mch1 , slot1 , omslot1 , side1 , pointer1 , ch2 , mch2 , slot2 , omslot2 , side2 , pointer2 , fs );
	//-----------------------------------------------------------
	LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
	//-----------------------------------------------------------
	//=================================================================================================
	// 2012.04.01
//	if ( mode == MACRO_PICK ) { // 2016.06.07
	if ( ( mode % 10 ) == MACRO_PICK ) { // 2016.06.07
		if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PICK( ch1 - CM1 , FALSE );
		if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PICK( ch2 - CM1 , FALSE );
	}
//	else if ( mode == MACRO_PLACE ) { // 2016.06.07
	else if ( ( mode % 10 ) == MACRO_PLACE ) { // 2016.06.07
		if ( ( ch1 >= CM1 ) && ( ch1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( ch1 - CM1 , FALSE );
		if ( ( ch2 >= CM1 ) && ( ch2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( ch2 - CM1 , FALSE );
	}
	//=================================================================================================
	//
	_SCH_COMMON_MESSAGE_CONTROL_FOR_USER( TRUE , 11 , fms , mode , och1 , slot1 , side1 , pointer1 , och2 , slot2 , side2 , pointer2 , 0.0 , 0.0 , 0.0 , NULL , NULL , NULL ); // 2012.12.04
	//
	//=================================================================================================
	return Res;
}


BOOL _SCH_MACRO_FM_OPERATION_THREAD_WAITING( int fms ) {
	int l_c; // 2008.09.14
	l_c = 0; // 2008.09.14
	while( TRUE ) {
		if ( Inf_FM_Result[fms] == -1 ) return FALSE;
		if ( Inf_FM_Result[fms] ==  0 ) return TRUE;
		// Sleep(1); // 2008.09.14
		if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
		l_c++; // 2008.09.14
	}
	return TRUE;
}

int _SCH_MACRO_FM_OPERATION_THREAD_RESULT( int fms ) {
	return Inf_FM_Result[fms];
}

void _SCH_MACRO_FM_OPERATION_THREAD( int fms ) {
	int l_c; // 2008.09.14
	Inf_FM_Result[fms] = 1;
	//---------------------------------------------------------------------------------
	// 2008.01.15
	//---------------------------------------------------------------------------------
	if ( Inf_FM_Move[fms] <= 0 ) { // 2010.02.20
		if ( Inf_FM_Pick_Mdl[fms] > 0 ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				Inf_FM_failres[fms] = _SCH_MACRO_CHECK_PROCESSING( Inf_FM_Pick_Mdl[fms] );
				if ( Inf_FM_failres[fms] <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( Inf_FM_failres[fms] < 0 ) {
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
			}
		}
		if ( ( Inf_FM_Pick_Mdl2[fms] > 0 ) && ( Inf_FM_Pick_Mdl[fms] != Inf_FM_Pick_Mdl2[fms] ) ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				Inf_FM_failres[fms] = _SCH_MACRO_CHECK_PROCESSING( Inf_FM_Pick_Mdl2[fms] );
				if ( Inf_FM_failres[fms] <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( Inf_FM_failres[fms] < 0 ) {
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
			}
		}
	}
	//---------------------------------------------------------------------------------
	if ( ( Inf_FM_Pick_Mdl[fms] != 0 ) || ( Inf_FM_Pick_Mdl2[fms] != 0 ) ) { // 2008.06.27
		if ( Inf_FM_Move[fms] <= 0 ) { // 2010.02.20
			switch( _SCH_MACRO_FM_OPERATION_SUB( fms , MACRO_PICK , Inf_FM_Pick_Mdl[fms] , Inf_FM_Pick_Mdl[fms] , Inf_FM_Slot[fms] , Inf_FM_Pick_MSlot[fms] , Inf_FM_Side[fms] , Inf_FM_Pointer[fms] , Inf_FM_Pick_Mdl2[fms] , Inf_FM_Pick_Mdl2[fms] , Inf_FM_Slot2[fms] , Inf_FM_Pick_MSlot2[fms] , Inf_FM_Side2[fms] , Inf_FM_Pointer2[fms] , -1 ) ) {
			case -1 :
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
				break;
			}
			//
			if ( Inf_FM_Pick_Mdl[fms] < PM1 ) { // 2011.01.19
				if ( _SCH_MACRO_FM_ALIC_OPERATION( fms , FAL_RUN_MODE_PLACE_MDL_PICK , Inf_FM_Side[fms] , AL , Inf_FM_Slot[fms] , Inf_FM_Slot2[fms] , Inf_FM_Slot[fms] > 0 ? _i_SCH_CLUSTER_Get_PathIn( Inf_FM_Side[fms] , Inf_FM_Pointer[fms] ) : _i_SCH_CLUSTER_Get_PathIn( Inf_FM_Side2[fms] , Inf_FM_Pointer2[fms] ) , -1 , TRUE , Inf_FM_Slot[fms] , Inf_FM_Slot2[fms] , Inf_FM_Side[fms] , Inf_FM_Side2[fms] , Inf_FM_Pointer[fms] , Inf_FM_Pointer2[fms] ) == SYS_ABORTED ) {
					_i_SCH_LOG_LOT_PRINTF( Inf_FM_Side[fms] , "FM%d Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , fms , Inf_FM_Slot2[fms] * 100 + Inf_FM_Slot[fms] , 9 , Inf_FM_Slot2[fms] * 100 + Inf_FM_Slot[fms] , 9 , Inf_FM_Slot2[fms] * 100 + Inf_FM_Slot[fms] );
					Inf_FM_Result[fms] = -1;
					_endthread();
					return;
				}
			}
			//
		}
		else { // 2010.02.20
			switch( _SCH_MACRO_FM_MOVE_OPERATION( fms , Inf_FM_Side[fms] , 0 , Inf_FM_Pick_Mdl[fms] , Inf_FM_Slot[fms] , Inf_FM_Slot2[fms] ) ) {
			case SYS_ABORTED :
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
				break;
			}
		}
	}
	//---------------------------------------------------------------------------------
	Inf_FM_Result[fms] = 2;
	//---------------------------------------------------------------------------------
	// 2008.01.15
	//---------------------------------------------------------------------------------
	if ( Inf_FM_Move[fms] <= 0 ) { // 2010.02.20
		if ( Inf_FM_Place_Mdl[fms] > 0 ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				Inf_FM_failres[fms] = _SCH_MACRO_CHECK_PROCESSING( Inf_FM_Place_Mdl[fms] );
				if ( Inf_FM_failres[fms] <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( Inf_FM_failres[fms] < 0 ) {
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
			}
		}
		if ( ( Inf_FM_Place_Mdl2[fms] > 0 ) && ( Inf_FM_Place_Mdl[fms] != Inf_FM_Place_Mdl2[fms] ) ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				Inf_FM_failres[fms] = _SCH_MACRO_CHECK_PROCESSING( Inf_FM_Place_Mdl2[fms] );
				if ( Inf_FM_failres[fms] <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( Inf_FM_failres[fms] < 0 ) {
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
			}
		}
	}
	//---------------------------------------------------------------------------------
	if ( ( Inf_FM_Place_Mdl[fms] != 0 ) || ( Inf_FM_Place_Mdl2[fms] != 0 ) ) { // 2008.06.27
		if ( Inf_FM_Move[fms] <= 0 ) { // 2010.02.20
			switch( _SCH_MACRO_FM_OPERATION_SUB( fms , MACRO_PLACE , Inf_FM_Place_Mdl[fms] , Inf_FM_Place_Mdl[fms] , Inf_FM_Slot[fms] , Inf_FM_Place_MSlot[fms] , Inf_FM_Side[fms] , Inf_FM_Pointer[fms] , Inf_FM_Place_Mdl2[fms] , Inf_FM_Place_Mdl2[fms] , Inf_FM_Slot2[fms] , Inf_FM_Place_MSlot2[fms] , Inf_FM_Side2[fms] , Inf_FM_Pointer2[fms] , Inf_FM_fs[fms] ) ) {
			case -1 :
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
				break;
			}
		}
		else { // 2010.02.20
			switch( _SCH_MACRO_FM_MOVE_OPERATION( fms , Inf_FM_Side[fms] , 1 , Inf_FM_Pick_Mdl[fms] , Inf_FM_Slot[fms] , Inf_FM_Slot2[fms] ) ) {
			case SYS_ABORTED :
				Inf_FM_Result[fms] = -1;
				_endthread();
				return;
				break;
			}
		}
	}
	//---------------------------------------------------------------------------------
	Inf_FM_Result[fms] = 0;
	_endthread();
}

BOOL _SCH_MACRO_FM_OPERATION_MOVE_THREAD_STARTING( int fms0 , int slot , int side , int pointer , int pick_mdl , int pick_slot , int place_mdl , int place_slot ) {
	int fms;
	//
	fms = fms0 % 100; // 2010.02.18
	//
	Inf_FM_Result[fms] = 1;
	//
	Inf_FM_Move[fms] = fms0 / 100; // 2010.02.18
	//
	Inf_FM_Slot[fms] = slot % 100;
	Inf_FM_Side[fms] = side % 100;
	Inf_FM_Pointer[fms] = pointer % 100;
	Inf_FM_Pick_Mdl[fms] = ( pick_mdl < 0 ) ? -1 : pick_mdl % 100;
	Inf_FM_Pick_MSlot[fms] = pick_slot % 100;
	Inf_FM_Place_Mdl[fms] = ( place_mdl < 0 ) ? -1 : place_mdl % 100;
	Inf_FM_Place_MSlot[fms] = place_slot % 100;
	//
	Inf_FM_Slot2[fms] = slot / 100;
	Inf_FM_Side2[fms] = side / 100;
	Inf_FM_Pointer2[fms] = pointer / 100;
	Inf_FM_Pick_Mdl2[fms] = ( pick_mdl < 0 ) ? -1 : pick_mdl / 100;
	Inf_FM_Pick_MSlot2[fms] = pick_slot / 100;
	Inf_FM_Place_Mdl2[fms] = ( place_mdl < 0 ) ? -1 : place_mdl / 100;
	Inf_FM_Place_MSlot2[fms] = place_slot / 100;
	//-----------------------------------------------
	// 2008.10.13
	//-----------------------------------------------
	Inf_FM_fs[fms] = -1;
	//
	if ( Inf_FM_Move[fms] <= 0 ) { // 2010.02.20
		if ( ( Inf_FM_Slot[fms] > 0 ) && ( Inf_FM_Side[fms] >= 0 ) && ( Inf_FM_Side[fms] < MAX_CASSETTE_SIDE ) && ( Inf_FM_Pointer[fms] >= 0 ) && ( Inf_FM_Pointer[fms] < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( ( _i_SCH_CLUSTER_Get_PathDo( Inf_FM_Side[fms] , Inf_FM_Pointer[fms] ) == PATHDO_WAFER_RETURN ) || ( _i_SCH_CLUSTER_Get_PathDo( Inf_FM_Side[fms] , Inf_FM_Pointer[fms] ) == 0 ) ) {
				Inf_FM_fs[fms] = 2;
			}
		}
		if ( Inf_FM_fs[fms] == -1 ) {
			if ( ( Inf_FM_Slot2[fms] > 0 ) && ( Inf_FM_Side2[fms] >= 0 ) && ( Inf_FM_Side2[fms] < MAX_CASSETTE_SIDE ) && ( Inf_FM_Pointer2[fms] >= 0 ) && ( Inf_FM_Pointer2[fms] < MAX_CASSETTE_SLOT_SIZE ) ) {
				if ( ( _i_SCH_CLUSTER_Get_PathDo( Inf_FM_Side2[fms] , Inf_FM_Pointer2[fms] ) == PATHDO_WAFER_RETURN ) || ( _i_SCH_CLUSTER_Get_PathDo( Inf_FM_Side2[fms] , Inf_FM_Pointer2[fms] ) == 0 ) ) {
					Inf_FM_fs[fms] = 2;
				}
			}
		}
	}
	//-----------------------------------------------
	if ( _beginthread( (void *) _SCH_MACRO_FM_OPERATION_THREAD , 0 , (void *) fms ) == -1 ) {
		Inf_FM_Result[fms] = 0;
		return FALSE;
	}
	//-----------------------------------------------
	return TRUE;
}

BOOL _SCH_MACRO_FM_OPERATION_MOVE_STARTING( int fms0 , int slot0 , int side0 , int pointer0 , int pick_mdl0 , int pick_slot0 , int place_mdl0 , int place_slot0 ) {
	int mve;
	int fms;
	//
	int failres;
	//
	int slot;
	int side;
	int pointer;
	int pick_mdl;
	int pick_slot;
	int place_mdl;
	int place_slot;
	//
	int slot2;
	int side2;
	int pointer2;
	int pick_mdl2;
	int pick_slot2;
	int place_mdl2;
	int place_slot2;
	//
	int fs; // 2008.10.13
	//
	int l_c; // 2008.09.14
	//
	fms = fms0 % 100; // 2010.02.18
	mve = fms0 / 100; // 2010.02.18
	//
	slot = slot0 % 100;
	side = side0 % 100;
	pointer = pointer0 % 100;
	pick_mdl = ( pick_mdl0 < 0 ) ? -1 : pick_mdl0 % 100;
	pick_slot = pick_slot0 % 100;
	place_mdl = ( place_mdl0 < 0 ) ? -1 : place_mdl0 % 100;
	place_slot = place_slot0 % 100;
	//
	slot2 = slot0 / 100;
	side2 = side0 / 100;
	pointer2 = pointer0 / 100;
	pick_mdl2 = ( pick_mdl0 < 0 ) ? -1 : pick_mdl0 / 100;
	pick_slot2 = pick_slot0 / 100;
	place_mdl2 = ( place_mdl0 < 0 ) ? -1 : place_mdl0 / 100;
	place_slot2 = place_slot0 / 100;

	//-----------------------------------------------
	// 2008.10.13
	//-----------------------------------------------
	fs = -1;
	if ( mve <= 0 ) { // 2010.02.20
		if ( ( slot > 0 ) && ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) && ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) == PATHDO_WAFER_RETURN ) || ( _i_SCH_CLUSTER_Get_PathDo( side , pointer ) == 0 ) ) {
				fs = 2;
			}
		}
		if ( fs == -1 ) {
			if ( ( slot2 > 0 ) && ( side2 >= 0 ) && ( side2 < MAX_CASSETTE_SIDE ) && ( pointer2 >= 0 ) && ( pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) {
				if ( ( _i_SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) == PATHDO_WAFER_RETURN ) || ( _i_SCH_CLUSTER_Get_PathDo( side2 , pointer2 ) == 0 ) ) {
					fs = 2;
				}
			}
		}
		//---------------------------------------------------------------------------------
		// 2008.01.15
		//---------------------------------------------------------------------------------
		if ( pick_mdl > 0 ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				failres = _SCH_MACRO_CHECK_PROCESSING( pick_mdl );
				if ( failres <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( failres < 0 ) return FALSE;
		}
		if ( ( pick_mdl2 > 0 ) && ( pick_mdl != pick_mdl2 ) ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				failres = _SCH_MACRO_CHECK_PROCESSING( pick_mdl2 );
				if ( failres <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( failres < 0 ) return FALSE;
		}
	}
	//---------------------------------------------------------------------------------
	if ( ( pick_mdl != 0 ) || ( pick_mdl2 != 0 ) ) { // 2008.06.27
		if ( mve <= 0 ) { // 2010.02.20
			switch( _SCH_MACRO_FM_OPERATION_SUB( fms , MACRO_PICK , pick_mdl , pick_mdl , slot , pick_slot , side , pointer , pick_mdl2 , pick_mdl2 , slot2 , pick_slot2 , side2 , pointer2 , -1 ) ) {
			case -1 :
				return FALSE;
				break;
			}
			//
			if ( pick_mdl < PM1 ) { // 2011.01.19
				if ( _SCH_MACRO_FM_ALIC_OPERATION( fms , FAL_RUN_MODE_PLACE_MDL_PICK , side , AL , slot , slot2 , slot > 0 ? _i_SCH_CLUSTER_Get_PathIn( side , pointer ) : _i_SCH_CLUSTER_Get_PathIn( side2 , pointer2 ) , -1 , TRUE , slot , slot2 , side2 , side2 , pointer , pointer2 ) == SYS_ABORTED ) {
					_i_SCH_LOG_LOT_PRINTF( side , "FM%d Handling Fail at AL(%d)%cWHFMALFAIL %d%c%d\n" , fms + 1 , slot2 * 100 + slot , 9 , slot2 * 100 + slot , 9 , slot2 * 100 + slot );
					return FALSE;
				}
			}
			//
		}
		else { // 2010.02.20
			switch( _SCH_MACRO_FM_MOVE_OPERATION( fms , side , 0 , pick_mdl , slot , slot2 ) ) {
			case SYS_ABORTED :
				return FALSE;
				break;
			}
		}
	}
	//---------------------------------------------------------------------------------
	// 2008.01.15
	//---------------------------------------------------------------------------------
	if ( mve <= 0 ) { // 2010.02.20
		if ( place_mdl > 0 ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				failres = _SCH_MACRO_CHECK_PROCESSING( place_mdl );
				if ( failres <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( failres < 0 ) return FALSE;
		}
		if ( ( place_mdl2 > 0 ) && ( place_mdl != place_mdl2 ) ) { // 2008.06.27
			l_c = 0; // 2008.09.14
			while( TRUE ) {
				failres = _SCH_MACRO_CHECK_PROCESSING( place_mdl2 );
				if ( failres <= 0 ) break;
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			if ( failres < 0 ) return FALSE;
		}
	}
	//---------------------------------------------------------------------------------
	if ( ( place_mdl != 0 ) || ( place_mdl2 != 0 ) ) { // 2008.06.27
		if ( mve <= 0 ) { // 2010.02.20
			switch( _SCH_MACRO_FM_OPERATION_SUB( fms , MACRO_PLACE , place_mdl , place_mdl , slot , place_slot , side , pointer , place_mdl2 , place_mdl2 , slot2 , place_slot2 , side2 , pointer2 , fs ) ) {
			case -1 :
				return FALSE;
				break;
			}
		}
		else { // 2010.02.20
			switch( _SCH_MACRO_FM_MOVE_OPERATION( fms , side , 1 , place_mdl , slot , slot2 ) ) {
			case SYS_ABORTED :
				return FALSE;
				break;
			}
		}
	}
	//---------------------------------------------------------------------------------
	return TRUE;
}
//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================
//======================================================================================================================================================================

void _SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( int FM_TSlot , int FM_CO1 , int FM_OSlot , int FM_Side , int FM_Pointer , int FM_TSlot2 , int FM_CO2 , int FM_OSlot2 , int FM_Side2 , int FM_Pointer2 , BOOL NormalMode ) { // 2007.07.11
	int finc , fincm , finslot;
	int finc2; // 2017.06.20
	//====================================================================================================================================================
	if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
		if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) {
			//
			fincm   = _i_SCH_CLUSTER_Get_PathIn(FM_Side,FM_Pointer);
			finslot = _i_SCH_CLUSTER_Get_SlotIn(FM_Side,FM_Pointer);
			//
//			_i_SCH_IO_SET_MODULE_STATUS( FM_CO1 , FM_OSlot , _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( fincm , finslot ) ); // 2011.04.18
			_i_SCH_IO_SET_MODULE_STATUS( FM_CO1 , FM_OSlot , _i_SCH_CASSETTE_LAST_RESULT2_WITH_CASS_GET( FM_Side , FM_Pointer ) ); // 2011.04.18
			//
			if ( _i_SCH_PRM_GET_DFIX_CASSETTE_ABSENT_RUN_DATA() > 0 ) { // 2010.10.29
				if ( ( fincm >= CM1 ) && ( fincm < PM1 ) ) {
					if ( ( FM_CO1 != fincm ) || ( FM_OSlot != finslot ) ) {
						//
						if ( _i_SCH_IO_GET_MODULE_STATUS( fincm , finslot ) == CWM_ABSENT ) _i_SCH_IO_SET_MODULE_STATUS( fincm , finslot , CWM_ABSENT );
						//
					}
				}
			}
			//
		}
	}
	if ( ( FM_TSlot2 > 0 ) && ( FM_Pointer2 >= 0 ) && ( FM_Pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) {
		if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) {
			//
			fincm   = _i_SCH_CLUSTER_Get_PathIn(FM_Side2,FM_Pointer2);
			finslot = _i_SCH_CLUSTER_Get_SlotIn(FM_Side2,FM_Pointer2);
			//
//			_i_SCH_IO_SET_MODULE_STATUS( FM_CO2 , FM_OSlot2 , _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( fincm , finslot ) ); // 2011.04.18
			_i_SCH_IO_SET_MODULE_STATUS( FM_CO2 , FM_OSlot2 , _i_SCH_CASSETTE_LAST_RESULT2_WITH_CASS_GET( FM_Side2 , FM_Pointer2 ) ); // 2011.04.18
			//
			if ( _i_SCH_PRM_GET_DFIX_CASSETTE_ABSENT_RUN_DATA() > 0 ) { // 2010.10.29
				if ( ( fincm >= CM1 ) && ( fincm < PM1 ) ) {
					if ( ( FM_CO2 != fincm ) || ( FM_OSlot2 != finslot ) ) {
						//
						if ( _i_SCH_IO_GET_MODULE_STATUS( fincm , finslot ) == CWM_ABSENT ) _i_SCH_IO_SET_MODULE_STATUS( fincm , finslot , CWM_ABSENT );
						//
					}
				}
			}
			//
		}
	}
	//==================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
		if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			_i_SCH_FMARMMULTI_CASSETTE_SET_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN ); // 2007.07.11
		}
	}
	//==================================================================================================
	finc = 0;
	finc2 = 0; // 2017.06.20
	if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
		finc++;
//		if ( NormalMode ) _i_SCH_LOG_TIMER_PRINTF( FM_Side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
		if ( NormalMode ) _i_SCH_LOG_TIMER_PRINTF( FM_Side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , FM_Pointer , -1 , -1 , "" , "" ); // 2012.02.18
		_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_Side , FM_Pointer , FA_SUBSTRATE_EVERYOUT , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) , _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer ) , _i_SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) );
		if ( SCHEDULER_CONTROL_Chk_Last_Finish_Status_FM( FM_Side , FM_Pointer , finc ) ) {
			_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_Side , FM_Pointer , FA_SUBSTRATE_LASTOUT , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) , _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer ) , _i_SCH_CLUSTER_Get_SlotOut( FM_Side , FM_Pointer ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) );
		}
		//
		if ( NormalMode ) _i_SCH_MULTIJOB_PROCESSJOB_END_CHECK( FM_Side , FM_Pointer );
		//
		if ( ( FM_TSlot2 > 0 ) && ( FM_Side == FM_Side2 ) ) finc2 = finc; // 2017.06.20
		//
	}
	if ( ( FM_TSlot2 > 0 ) && ( FM_Pointer2 >= 0 ) && ( FM_Pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) {
//		finc++; // 2017.06.20
		finc2++; // 2017.06.20
//		if ( NormalMode ) _i_SCH_LOG_TIMER_PRINTF( FM_Side2 , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
		if ( NormalMode ) _i_SCH_LOG_TIMER_PRINTF( FM_Side2 , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , FM_Pointer2 , -1 , -1 , "" , "" ); // 2012.02.18
		_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_Side2 , FM_Pointer2 , FA_SUBSTRATE_EVERYOUT , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) , _i_SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 ) , _i_SCH_CLUSTER_Get_SlotOut( FM_Side2 , FM_Pointer2 ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) );
//		if ( SCHEDULER_CONTROL_Chk_Last_Finish_Status_FM( FM_Side2 , FM_Pointer2 , finc ) ) { // 2017.06.20
		if ( SCHEDULER_CONTROL_Chk_Last_Finish_Status_FM( FM_Side2 , FM_Pointer2 , finc2 ) ) { // 2017.06.20
			_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( FM_Side2 , FM_Pointer2 , FA_SUBSTRATE_LASTOUT , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) , _i_SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 ) , _i_SCH_CLUSTER_Get_SlotOut( FM_Side2 , FM_Pointer2 ) , 0 , 0 , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) );
		}
		//
		if ( NormalMode ) _i_SCH_MULTIJOB_PROCESSJOB_END_CHECK( FM_Side2 , FM_Pointer2 );
		//
		if ( ( FM_TSlot > 0 ) && ( FM_Side == FM_Side2 ) ) finc = finc2; // 2017.06.20
		//
	}
	//====================================================================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
		if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM_POST( &finc , NormalMode ); // 2007.07.11
			//
			_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( FM_Side , FM_Pointer );
		}
		//====================================================================================================================================================
		if ( ( FM_TSlot2 > 0 ) && ( FM_Pointer2 >= 0 ) && ( FM_Pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) {
			_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( FM_Side2 , FM_Pointer2 );
		}
		//====================================================================================================================================================
		if ( FM_TSlot > 0 )	 {
//			_i_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , FM_Side , 0 ); // 2013.01.11
			_i_SCH_MODULE_Set_FM_WAFER( TA_STATION , 0 ); // 2013.01.11
			_i_SCH_MODULE_Inc_FM_InCount( FM_Side );
			if ( FM_Pointer >= MAX_CASSETTE_SLOT_SIZE ) _i_SCH_MODULE_Dec_TM_DoubleCount( FM_Side );
		}
		if ( FM_TSlot2 > 0 ) {
//			_i_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , FM_Side2 , 0 ); // 2013.01.11
			_i_SCH_MODULE_Set_FM_WAFER( TB_STATION , 0 ); // 2013.01.11
			_i_SCH_MODULE_Inc_FM_InCount( FM_Side2 );
			if ( FM_Pointer2 >= MAX_CASSETTE_SLOT_SIZE ) _i_SCH_MODULE_Dec_TM_DoubleCount( FM_Pointer2 );
		}
		//==================================================================================================
		if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			_i_SCH_FMARMMULTI_LASTSET_AFTER_PLACE_CM(); // 2007.07.11
		}
		//====================================================================================================================================================
	}
	else {
		//====================================================================================================================================================
		if ( ( FM_TSlot > 0 ) && ( FM_Pointer >= 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( FM_Side , FM_Pointer );
		}
		if ( ( FM_TSlot2 > 0 ) && ( FM_Pointer2 >= 0 ) && ( FM_Pointer2 < MAX_CASSETTE_SLOT_SIZE ) ) {
			_i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_After_Place_CM_FDHC( FM_Side2 , FM_Pointer2 );
		}
		//====================================================================================================================================================
		if ( FM_TSlot > 0 )	 {
			_i_SCH_MODULE_Set_TM_WAFER( 0 , TA_STATION , 0 );
			_i_SCH_MODULE_Inc_TM_InCount( FM_Side );
		}
		if ( FM_TSlot2 > 0 ) {
			_i_SCH_MODULE_Set_TM_WAFER( 0 , TB_STATION , 0 );
			_i_SCH_MODULE_Inc_TM_InCount( FM_Side2 );
		}
		//====================================================================================================================================================
	}
}

/*
int _SCH_MACRO_FM_PLACE_TO_CM( int CHECKING_SIDE , int FM_TSlot0 , int FM_Side , int FM_Pointer , int FM_TSlot20 , int FM_Side2 , int FM_Pointer2 , int errorout , BOOL disapear , int realslotoffset ) { // 2007.07.11
	int FM_TSlot , FM_TSlot2;
	int FM_OSlot , FM_OSlot2;
	int FM_CO1 , FM_CO2;
	int ret1 , ret2;
	BOOL fail;
	//====================================================================================================================================================
	FM_TSlot = FM_TSlot0;
	FM_TSlot2 = FM_TSlot20;
	//
	FM_CO1 = 0; // 2011.04.14 (Testing)
	FM_CO2 = 0; // 2011.04.14 (Testing)
	//====================================================================================================================================================
	while ( TRUE ) { // 2011.04.14 (Testing)
		//
		if ( ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
			return -1;
		}
		//
		if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
			//
			if ( FM_CO1 <= 0 ) { // 2011.04.14 (Testing)
				//
				ret1 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side , FM_Pointer , FM_TSlot , &FM_CO1 , &FM_OSlot , &fail );
				//
				if ( fail ) return -1; // 2011.05.03
				//
				if ( ret1 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9;
					//
					ret1 = 0;
					//
				}
				//
			}
			//
			if ( FM_CO2 <= 0 ) { // 2011.04.14 (Testing)
				//
				ret2 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side2 , FM_Pointer2 , FM_TSlot2 , &FM_CO2 , &FM_OSlot2 , &fail );
				//
				if ( fail ) return -1; // 2011.05.03
				//
				if ( ret2 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9;
					//
					ret2 = 0;
					//
				}
				//
			}
			//
			if ( ( FM_CO1 <= 0 ) || ( FM_CO2 <= 0 ) ) {
				Sleep(1000);
				continue;
			}
			//
			if ( ( ( errorout == 1 ) || ( errorout == 3 ) ) && !ret1 && !ret2 ) _i_SCH_SUB_PLACE_CM_for_ERROROUT( FM_Side , FM_Pointer , &FM_CO1 , &FM_OSlot , FM_Side2 , FM_Pointer2 , &FM_CO2 , &FM_OSlot2 );
		}
		else if ( FM_TSlot > 0 ) {
			//
			if ( FM_CO1 <= 0 ) { // 2011.04.14 (Testing)
				//
				ret1 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side , FM_Pointer , FM_TSlot , &FM_CO1 , &FM_OSlot , &fail );
				//
				if ( fail ) return -1; // 2011.05.03
				//
				if ( ret1 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9;
					//
					ret1 = 0;
					//
				}
				//
			}
			//
			if ( FM_CO1 <= 0 ) { // 2011.04.14 (Testing)
				Sleep(1000);
				continue;
			}
			//
			FM_CO2 = 0;
			FM_OSlot2 = 0;
			if ( ( ( errorout == 1 ) || ( errorout == 3 ) ) && !ret1 ) _i_SCH_SUB_PLACE_CM_for_ERROROUT( FM_Side , FM_Pointer , &FM_CO1 , &FM_OSlot , FM_Side2 , FM_Pointer2 , &FM_CO2 , &FM_OSlot2 );
		}
		else if ( FM_TSlot2 > 0 ) {
			//
			if ( FM_CO2 <= 0 ) { // 2011.04.14 (Testing)
				ret2 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side2 , FM_Pointer2 , FM_TSlot2 , &FM_CO2 , &FM_OSlot2 , &fail );
				//
				if ( fail ) return -1; // 2011.05.03
				//
				if ( ret2 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9;
					//
					ret2 = 0;
					//
				}
				//
			}
			//
			if ( FM_CO2 <= 0 ) { // 2011.04.14 (Testing)
				Sleep(1000);
				continue;
			}
			//
			FM_CO1 = 0;
			FM_OSlot = 0;
			if ( ( ( errorout == 1 ) || ( errorout == 3 ) ) && !ret2 ) _i_SCH_SUB_PLACE_CM_for_ERROROUT( FM_Side , FM_Pointer , &FM_CO1 , &FM_OSlot , FM_Side2 , FM_Pointer2 , &FM_CO2 , &FM_OSlot2 );
		}
		//
		break; // 2011.04.14 (Testing)
		//
	}
	//====================================================================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2009.08.06
		//
		while( TRUE ) {
			if ( ( FM_CO1 > 0 ) && ( FM_CO2 > 0 ) ) {
				if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO1 , G_PLACE , 0 ) ) {
					if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO2 , G_PLACE , 0 ) ) {
						break;
					}
				}
			}
			else if ( FM_CO1 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO1 , G_PLACE , 0 ) ) {
					break;
				}
			}
			else if ( FM_CO2 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO2 , G_PLACE , 0 ) ) {
					break;
				}
			}
			if ( ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
				return -1;
			}
			//
			if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9; // 2011.09.28
			//
			Sleep(1);
		}
	}
	else { // 2010.09.01
		while( TRUE ) {
			if ( ( FM_CO1 > 0 ) && ( FM_CO2 > 0 ) ) {
				if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO1 , G_PLACE ) ) {
					if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO2 , G_PLACE ) ) {
						break;
					}
				}
			}
			else if ( FM_CO1 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO1 , G_PLACE ) ) {
					break;
				}
			}
			else if ( FM_CO2 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO2 , G_PLACE ) ) {
					break;
				}
			}
			if ( ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
				return -1;
			}
			//
			if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9; // 2011.09.28
			//
			Sleep(1);
		}
	}
	//====================================================================================================================================================
//	EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
	//-----------------------------------------------------------
	_SCH_TAG_BM_LAST_PLACE = 0; // 2008.03.24
	//====================================================================================================================================================
	if ( ( FM_CO1 > 0 ) && ( FM_CO2 > 0 ) ) {
//		if ( FM_CO1 == FM_CO2 ) { // 2008.04.11
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) && ( FM_CO1 == FM_CO2 ) ) { // 2008.04.11
			//------------------------------------------------------------
			if ( disapear ) {
				//###################################################################################
				// Disapear Check (2002.05.17) // 2002.06.07
				//###################################################################################
				if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
						FM_OSlot = 0;
						FM_TSlot = 0;
					}
					if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
						FM_OSlot2 = 0;
						FM_TSlot2 = 0;
					}
				}
				if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
					//====================================================================================================================================================
					LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
					//====================================================================================================================================================
					return 1;
				}
				//###################################################################################
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
			if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO1 , FM_OSlot + realslotoffset , FM_OSlot2 , ( ret1 || ret2 ) ? 2 : 1 , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , 0 , FALSE ) == SYS_ABORTED ) {
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
				//------------------------------------------------------------
				//====================================================================================================================================================
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
				//====================================================================================================================================================
				return -1;
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
				//------------------------------------------------------------
				if ( disapear ) {
					//###################################################################################
					// Disapear Check (2002.05.17) // 2002.06.07
					//###################################################################################
					if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
						if ( _i_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_OSlot = 0;
							FM_TSlot = 0;
						}
						if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							FM_OSlot2 = 0;
							FM_TSlot2 = 0;
						}
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return 2;
					}
					//###################################################################################
				}
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
				//------------------------------------------------------------
				if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO1 , FM_OSlot + realslotoffset , 0 , ret1 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , 0 , FALSE ) == SYS_ABORTED ) {
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
					//------------------------------------------------------------
					//====================================================================================================================================================
					LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
					//====================================================================================================================================================
					return -1;
				}
				if ( FM_TSlot > 0 ) {
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
					//------------------------------------------------------------
				}
				if ( FM_TSlot2 > 0 ) {
					//------------------------------------------------------------
					if ( disapear ) {
						//###################################################################################
						// Disapear Check (2002.05.17) // 2002.06.07
						//###################################################################################
						if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
							if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
								FM_OSlot2 = 0;
								FM_TSlot2 = 0;
							}
							if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
								//====================================================================================================================================================
								LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
								//====================================================================================================================================================
								return 3;
							}
						}
						//###################################################################################
					}
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
					//------------------------------------------------------------
					if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO2 , 0 , FM_OSlot2 , ret2 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , 0 , FALSE ) == SYS_ABORTED ) {
						//------------------------------------------------------------
						_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
						//------------------------------------------------------------
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return -1;
					}
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
					//------------------------------------------------------------
				}
			}
			else { // 2008.04.11
				//=======================================================================================
				// 2008.04.11
				//=======================================================================================
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side , FM_Pointer , FM_CO1 , 0 , FM_OSlot , FM_TSlot , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
					//====================================================================================================================================================
					// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
					//====================================================================================================================================================
					return -1;
				}
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side2 , FM_Pointer2 , FM_CO2 , 1 , FM_OSlot2 , FM_TSlot2 , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
					//====================================================================================================================================================
					// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
					//====================================================================================================================================================
					return -1;
				}
				//=======================================================================================
			}
		}
	}
	else if ( FM_CO1 > 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
			//------------------------------------------------------------
			if ( disapear ) {
				//###################################################################################
				// Disapear Check (2002.05.17) // 2002.06.07
				//###################################################################################
				if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
						FM_OSlot = 0;
						FM_TSlot = 0;
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return 4;
					}
				}
				//###################################################################################
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
			//------------------------------------------------------------
			if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO1 , FM_OSlot + realslotoffset , 0 , ret1 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , 0 , FALSE ) == SYS_ABORTED ) {
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
				//------------------------------------------------------------
				//====================================================================================================================================================
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
				//====================================================================================================================================================
				return -1;
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
			//------------------------------------------------------------
		}
		else {
			//=======================================================================================
			// 2008.04.11
			//=======================================================================================
			if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side , FM_Pointer , FM_CO1 , 0 , FM_OSlot , FM_TSlot , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
				//====================================================================================================================================================
				// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
				//====================================================================================================================================================
				return -1;
			}
			//=======================================================================================
		}
	}
	else if ( FM_CO2 > 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
			//------------------------------------------------------------
			if ( disapear ) {
				//###################################################################################
				// Disapear Check (2002.05.17) // 2002.06.07
				//###################################################################################
				if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
						FM_OSlot2 = 0;
						FM_TSlot2 = 0;
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return 5;
					}
				}
				//###################################################################################
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
			if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO2 , 0 , FM_OSlot2 , ret2 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , 0 , FALSE ) == SYS_ABORTED ) {
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
				//------------------------------------------------------------
				//====================================================================================================================================================
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
				//====================================================================================================================================================
				return -1;
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
		}
		else {
			//=======================================================================================
			// 2008.04.11
			//=======================================================================================
			if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side2 , FM_Pointer2 , FM_CO2 , 1 , FM_OSlot2 , FM_TSlot2 , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
				//====================================================================================================================================================
				// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
				//====================================================================================================================================================
				return -1;
			}
			//=======================================================================================
		}
	}
	//====================================================================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
		if ( FM_CO1 > 0 ) {
			_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN );
		}
		//====================================================================================================================================================
		if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
			if ( ( FM_CO1 == FM_CO2 ) && ( ret1 == ret2 ) && ( FM_Side == FM_Side2 ) ) {
				_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO1 , !ret1 , FM_Side , 2 );
			}
			else {
				_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO1 , !ret1 , FM_Side , 1 );
				_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO2 , !ret2 , FM_Side2 , 1 );
			}
		}
		else if ( FM_TSlot > 0 ) {
			_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO1 , !ret1 , FM_Side , 1 );
		}
		else if ( FM_TSlot2 > 0 ) {
			_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO2 , !ret2 , FM_Side2 , 1 );
		}
		//====================================================================================================================================================
		_i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( FM_CO1 ); // 2007.07.11
	}
	//====================================================================================================================================================
	_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_TSlot , FM_CO1 , FM_OSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_CO2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 , TRUE );
	//====================================================================================================================================================
//	LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
	//====================================================================================================================================================
	//
	// 2011.04.13
	//
	if ( FM_CO1 > 0 ) {
		if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) { // 2011.09.02
			if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( TRUE , FM_Side , FM_Pointer , FM_CO1 , FM_OSlot ) ) return -1;
		}
	}
	if ( FM_CO2 > 0 ) {
		if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) { // 2011.09.02
			if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( TRUE , FM_Side2 , FM_Pointer2 , FM_CO2 , FM_OSlot2 ) ) return -1;
		}
	}
	//
	return 0;
}
*/

int _SCH_MACRO_FM_PLACE_TO_CM_Sub( int CHECKING_SIDE , int FM_TSlot , int FM_Side , int FM_Pointer , int FM_CO1 , int FM_OSlot , int ret1 , int FM_TSlot2 , int FM_Side2 , int FM_Pointer2 , int FM_CO2 , int FM_OSlot2 , int ret2 , int errorout , BOOL disapear , int realslotoffset ) { // 2007.07.11
	//====================================================================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2009.08.06
		//
		while( TRUE ) {
			if ( ( FM_CO1 > 0 ) && ( FM_CO2 > 0 ) ) {
				if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO1 , G_PLACE , 0 ) ) {
					if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO2 , G_PLACE , 0 ) ) {
						break;
					}
				}
			}
			else if ( FM_CO1 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO1 , G_PLACE , 0 ) ) {
					break;
				}
			}
			else if ( FM_CO2 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_FM_POSSIBLE( FM_CO2 , G_PLACE , 0 ) ) {
					break;
				}
			}
			if ( ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
				return -1;
			}
			//
			if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9; // 2011.09.28
			//
			Sleep(1);
		}
	}
	else { // 2010.09.01
		while( TRUE ) {
			if ( ( FM_CO1 > 0 ) && ( FM_CO2 > 0 ) ) {
				if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO1 , G_PLACE ) ) {
					if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO2 , G_PLACE ) ) {
						break;
					}
				}
			}
			else if ( FM_CO1 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO1 , G_PLACE ) ) {
					break;
				}
			}
			else if ( FM_CO2 > 0 ) {
				if ( _i_SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , FM_CO2 , G_PLACE ) ) {
					break;
				}
			}
			if ( ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
				return -1;
			}
			//
			if ( ( errorout == 2 ) || ( errorout == 3 ) ) return 9; // 2011.09.28
			//
			Sleep(1);
		}
	}
	//====================================================================================================================================================
//	EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
	//-----------------------------------------------------------
	_SCH_TAG_BM_LAST_PLACE = 0; // 2008.03.24
	//====================================================================================================================================================
	if ( ( FM_CO1 > 0 ) && ( FM_CO2 > 0 ) ) {
//		if ( FM_CO1 == FM_CO2 ) { // 2008.04.11
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) && ( FM_CO1 == FM_CO2 ) ) { // 2008.04.11
			//------------------------------------------------------------
			if ( disapear ) {
				//###################################################################################
				// Disapear Check (2002.05.17) // 2002.06.07
				//###################################################################################
				if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
						FM_OSlot = 0;
						FM_TSlot = 0;
					}
					if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
						FM_OSlot2 = 0;
						FM_TSlot2 = 0;
					}
				}
				if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
					//====================================================================================================================================================
					LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
					//====================================================================================================================================================
					return 1;
				}
				//###################################################################################
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
			if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO1 , FM_OSlot + realslotoffset , FM_OSlot2 , ( ret1 || ret2 ) ? 2 : 1 , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , 0 , FALSE ) == SYS_ABORTED ) {
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
				//------------------------------------------------------------
				//====================================================================================================================================================
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
				//====================================================================================================================================================
				return -1;
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
				//------------------------------------------------------------
				if ( disapear ) {
					//###################################################################################
					// Disapear Check (2002.05.17) // 2002.06.07
					//###################################################################################
					if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
						if ( _i_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
							FM_OSlot = 0;
							FM_TSlot = 0;
						}
						if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
							FM_OSlot2 = 0;
							FM_TSlot2 = 0;
						}
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return 2;
					}
					//###################################################################################
				}
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
				//------------------------------------------------------------
				if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO1 , FM_OSlot + realslotoffset , 0 , ret1 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , 0 , FALSE ) == SYS_ABORTED ) {
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
					//------------------------------------------------------------
					//====================================================================================================================================================
					LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
					//====================================================================================================================================================
					return -1;
				}
				if ( FM_TSlot > 0 ) {
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
					//------------------------------------------------------------
				}
				if ( FM_TSlot2 > 0 ) {
					//------------------------------------------------------------
					if ( disapear ) {
						//###################################################################################
						// Disapear Check (2002.05.17) // 2002.06.07
						//###################################################################################
						if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
							if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
								FM_OSlot2 = 0;
								FM_TSlot2 = 0;
							}
							if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
								//====================================================================================================================================================
								LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
								//====================================================================================================================================================
								return 3;
							}
						}
						//###################################################################################
					}
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
					//------------------------------------------------------------
					if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO2 , 0 , FM_OSlot2 , ret2 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , 0 , FALSE ) == SYS_ABORTED ) {
						//------------------------------------------------------------
						_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
						//------------------------------------------------------------
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return -1;
					}
					//------------------------------------------------------------
					_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
					//------------------------------------------------------------
				}
			}
			else { // 2008.04.11
				//=======================================================================================
				// 2008.04.11
				//=======================================================================================
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side , FM_Pointer , FM_CO1 , 0 , FM_OSlot , FM_TSlot , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
					//====================================================================================================================================================
					// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
					//====================================================================================================================================================
					return -1;
				}
				if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side2 , FM_Pointer2 , FM_CO2 , 1 , FM_OSlot2 , FM_TSlot2 , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
					//====================================================================================================================================================
					// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
					//====================================================================================================================================================
					return -1;
				}
				//=======================================================================================
			}
		}
	}
	else if ( FM_CO1 > 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
			//------------------------------------------------------------
			if ( disapear ) {
				//###################################################################################
				// Disapear Check (2002.05.17) // 2002.06.07
				//###################################################################################
				if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) {
						FM_OSlot = 0;
						FM_TSlot = 0;
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return 4;
					}
				}
				//###################################################################################
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
			//------------------------------------------------------------
			if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO1 , FM_OSlot + realslotoffset , 0 , ret1 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , 0 , FALSE ) == SYS_ABORTED ) {
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
				//------------------------------------------------------------
				//====================================================================================================================================================
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
				//====================================================================================================================================================
				return -1;
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO1 , FM_OSlot , FM_OSlot , FM_Side , FM_Pointer , 0 , 0 , 0 , 0 );
			//------------------------------------------------------------
		}
		else {
			//=======================================================================================
			// 2008.04.11
			//=======================================================================================
			if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side , FM_Pointer , FM_CO1 , 0 , FM_OSlot , FM_TSlot , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side , FM_Pointer ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
				//====================================================================================================================================================
				// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
				//====================================================================================================================================================
				return -1;
			}
			//=======================================================================================
		}
	}
	else if ( FM_CO2 > 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
			//------------------------------------------------------------
			if ( disapear ) {
				//###################################################################################
				// Disapear Check (2002.05.17) // 2002.06.07
				//###################################################################################
				if ( _i_SCH_SUB_DISAPPEAR_OPERATION( -1 , 0 ) ) {
					if ( _i_SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) {
						FM_OSlot2 = 0;
						FM_TSlot2 = 0;
					}
					if ( ( FM_TSlot <= 0 ) && ( FM_TSlot2 <= 0 ) ) {
						//====================================================================================================================================================
						LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
						//====================================================================================================================================================
						return 5;
					}
				}
				//###################################################################################
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_RUNNING , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
			if ( EQUIP_PLACE_TO_FMBM( 0 , CHECKING_SIDE , FM_CO2 , 0 , FM_OSlot2 , ret2 ? 2 : 0 , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , 0 , FALSE ) == SYS_ABORTED ) {
				//------------------------------------------------------------
				_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_FAIL , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
				//------------------------------------------------------------
				//====================================================================================================================================================
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
				//====================================================================================================================================================
				return -1;
			}
			//------------------------------------------------------------
			_SCH_MACRO_LOT_PRINTF_SUBST_FOR_FM( 0 , MACRO_PLACE , FA_SUBST_SUCCESS , FM_CO2 , 0 , 0 , 0 , 0 , FM_OSlot2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 );
			//------------------------------------------------------------
		}
		else {
			//=======================================================================================
			// 2008.04.11
			//=======================================================================================
			if ( _SCH_MACRO_TM_OPERATION_0( MACRO_PLACE , FM_Side2 , FM_Pointer2 , FM_CO2 , 1 , FM_OSlot2 , FM_TSlot2 , realslotoffset , _i_SCH_CLUSTER_Get_PathIn( FM_Side2 , FM_Pointer2 ) , FALSE , _i_SCH_SUB_SWMODE_FROM_SWITCH_OPTION( realslotoffset , 0 ) , -1 , -1 ) == SYS_ABORTED ) {
				//====================================================================================================================================================
				// LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
				//====================================================================================================================================================
				return -1;
			}
			//=======================================================================================
		}
	}
	//====================================================================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2008.04.11
		if ( FM_CO1 > 0 ) {
			_i_SCH_FMARMMULTI_FA_SUBSTRATE_AFTER_PLACE_CM( _i_SCH_CLUSTER_Get_PathDo( FM_Side , FM_Pointer ) == PATHDO_WAFER_RETURN );
		}
		//====================================================================================================================================================
		if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
			if ( ( FM_CO1 == FM_CO2 ) && ( ret1 == ret2 ) && ( FM_Side == FM_Side2 ) ) {
				_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO1 , !ret1 , FM_Side , 2 );
			}
			else {
				_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO1 , !ret1 , FM_Side , 1 );
				_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO2 , !ret2 , FM_Side2 , 1 );
			}
		}
		else if ( FM_TSlot > 0 ) {
			_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO1 , !ret1 , FM_Side , 1 );
		}
		else if ( FM_TSlot2 > 0 ) {
			_i_SCH_IO_MTL_ADD_PLACE_COUNT( FM_CO2 , !ret2 , FM_Side2 , 1 );
		}
		//====================================================================================================================================================
		_i_SCH_FMARMMULTI_ADD_COUNT_AFTER_PLACE_CM( FM_CO1 ); // 2007.07.11
	}
	//====================================================================================================================================================
	_SCH_MACRO_PLACE_TO_CM_POST_PART_with_FM( FM_TSlot , FM_CO1 , FM_OSlot , FM_Side , FM_Pointer , FM_TSlot2 , FM_CO2 , FM_OSlot2 , FM_Side2 , FM_Pointer2 , TRUE );
	//====================================================================================================================================================
//	LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.25 // 2009.08.06
	//====================================================================================================================================================
	//
	// 2016.11.02
	//
	if ( FM_CO1 > 0 ) {
		//
		if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) {
			//
			if ( SCHMRDATA_Data_IN_Unload_Request_After_Place( FM_Side , FM_CO1 ) ) {
				//
				Scheduler_Run_Unload_Code( FM_Side , FM_CO1 , ( _i_SCH_SYSTEM_FA_GET( FM_Side ) == 1 ) , FALSE , 0 , FM_Pointer , _i_SCH_CLUSTER_Get_SlotIn( FM_Side , FM_Pointer ) );
				//
			}
			//
		}
		//
	}
	if ( FM_CO2 > 0 ) {
		//
		if ( ( FM_CO1 <= 0 ) || ( FM_CO1 != FM_CO2 ) ) {
			//
			if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) {
				//
				if ( SCHMRDATA_Data_IN_Unload_Request_After_Place( FM_Side2 , FM_CO2 ) ) {
					//
					Scheduler_Run_Unload_Code( FM_Side2 , FM_CO2 , ( _i_SCH_SYSTEM_FA_GET( FM_Side2 ) == 1 ) , FALSE , 0 , FM_Pointer2 , _i_SCH_CLUSTER_Get_SlotIn( FM_Side2 , FM_Pointer2 ) );
					//
				}
				//
			}
			//
		}
	}
	//
	//====================================================================================================================================================
	//
	// 2011.04.13
	//
	if ( FM_CO1 > 0 ) {
		if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) { // 2011.09.02
			if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( TRUE , FM_Side , FM_Pointer , FM_CO1 , FM_OSlot ) ) return -1;
		}
	}
	if ( FM_CO2 > 0 ) {
		if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) { // 2011.09.02
			if ( !SCHMULTI_CHECK_HANDOFF_OUT_CHECK( TRUE , FM_Side2 , FM_Pointer2 , FM_CO2 , FM_OSlot2 ) ) return -1;
		}
	}
	//
	return 0;
}



int _SCH_MACRO_FM_PLACE_TO_CM( int CHECKING_SIDE , int FM_TSlot0 , int FM_Side , int FM_Pointer , int FM_TSlot20 , int FM_Side2 , int FM_Pointer2 , int errorout , BOOL disapear , int realslotoffset ) { // 2007.07.11
	//
	int FM_TSlot , FM_TSlot2;
	int FM_OSlot , FM_OSlot2;
	int FM_CO1 , FM_CO2;
	int ret1 , ret2 , Res;
	BOOL fail , needlock;

	//====================================================================================================================================================

	if ( FM_TSlot0 > 0 ) {
//		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( FM_Side ) ) return -1; // 2012.07.12 // 2016.11.04
		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( FM_Side , FM_Pointer ) ) return -1; // 2012.07.12 // 2016.11.04
	}
	if ( FM_TSlot20 > 0 ) {
//		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( FM_Side2 ) ) return -1; // 2012.07.12 // 2016.11.04
		if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( FM_Side2 , FM_Pointer2 ) ) return -1; // 2012.07.12 // 2016.11.04
	}
	//====================================================================================================================================================
	//
	_SCH_COMMON_MESSAGE_CONTROL_FOR_USER( FALSE , 10 , 0 , MACRO_PLACE , -1 , FM_TSlot0 , FM_Side , FM_Pointer , -1 , FM_TSlot20 , FM_Side2 , FM_Pointer2 , 0.0 , 0.0 , 0.0 , NULL , NULL , NULL ); // 2016.07.28
	//
	//====================================================================================================================================================
	FM_TSlot = FM_TSlot0;
	FM_TSlot2 = FM_TSlot20;
	//
	FM_CO1 = 0; // 2011.04.14 (Testing)
	FM_CO2 = 0; // 2011.04.14 (Testing)
	//====================================================================================================================================================

	needlock = FALSE;
	if ( FM_TSlot > 0 ) {
		if ( ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( FM_Side , FM_Pointer ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( FM_Side , FM_Pointer ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) ) { // 2013.07.02
			if ( ( _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathOut( FM_Side , FM_Pointer ) < PM1 ) ) {
				needlock = TRUE;
			}
		}
	}
	if ( !needlock ) {
		if ( FM_TSlot2 > 0 ) {
			if ( ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( FM_Side2 , FM_Pointer2 ) % 100 ) == _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) || ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( FM_Side2 , FM_Pointer2 ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) ) { // 2013.07.02
				if ( ( _i_SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathOut( FM_Side2 , FM_Pointer2 ) < PM1 ) ) {
					needlock = TRUE;
				}
			}
		}
	}
	//------------------------------------------------------
	if ( needlock ) SCH_MR_UNLOAD_LOCK_START( -1 ); // 2012.04.01
	//------------------------------------------------------
	while ( TRUE ) { // 2011.04.14 (Testing)
		//
		if ( ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) || ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) <= 0 ) ) {
			//------------------------------------------------------
			if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
			//------------------------------------------------------
			return -1;
		}
		//
		if ( ( FM_TSlot > 0 ) && ( FM_TSlot2 > 0 ) ) {
			//
			if ( FM_CO1 <= 0 ) { // 2011.04.14 (Testing)
				//
				ret1 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side , FM_Pointer , FM_TSlot , &FM_CO1 , &FM_OSlot , &fail );
				//
				if ( fail ) {
					//------------------------------------------------------
					if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
					//------------------------------------------------------
					return -1; // 2011.05.03
				}
				//
				if ( ret1 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) {
						//------------------------------------------------------
						if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
						//------------------------------------------------------
						return 9;
					}
					//
					ret1 = 0;
					//
				}
				//
			}
			//
			if ( FM_CO2 <= 0 ) { // 2011.04.14 (Testing)
				//
				ret2 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side2 , FM_Pointer2 , FM_TSlot2 , &FM_CO2 , &FM_OSlot2 , &fail );
				//
				if ( fail ) {
					//------------------------------------------------------
					if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
					//------------------------------------------------------
					return -1; // 2011.05.03
				}
				//
				if ( ret2 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) {
						//------------------------------------------------------
						if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
						//------------------------------------------------------
						return 9;
					}
					//
					ret2 = 0;
					//
				}
				//
			}
			//
			if ( ( FM_CO1 <= 0 ) || ( FM_CO2 <= 0 ) ) {
				//------------------------------------------------------
				if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO1 - 1 , TRUE ); // 2012.04.01
				if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO2 - 1 , TRUE ); // 2012.04.01
				//------------------------------------------------------
				if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
				//------------------------------------------------------
				if      ( FM_CO1 > 0 ) {
					//
					Res = _SCH_MACRO_FM_PLACE_TO_CM_Sub( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_CO1 , FM_OSlot , ret1 , 0 , 0 , 0 , 0 , 0 , 0 , errorout , disapear , realslotoffset );
					//
					//------------------------------------------------------
					if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO1 - 1 , FALSE ); // 2012.04.01
					//------------------------------------------------------
					//
					if ( Res < 0 ) return Res;
					//
					FM_TSlot = 0;
					//
				}
				else if ( FM_CO2 > 0 ) {
					//
					Res = _SCH_MACRO_FM_PLACE_TO_CM_Sub( CHECKING_SIDE , 0 , 0 , 0 , 0 , 0 , 0 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FM_CO2 , FM_OSlot2 , ret2 , errorout , disapear , realslotoffset );
					//
					//------------------------------------------------------
					if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO2 - 1 , FALSE ); // 2012.04.01
					//------------------------------------------------------
					//
					if ( Res < 0 ) return Res;
					//
					FM_TSlot2 = 0;
					//
				}
				else {
					Sleep(500);
				}
				//------------------------------------------------------
				if ( needlock ) SCH_MR_UNLOAD_LOCK_START( -1 ); // 2012.04.01
				//------------------------------------------------------
				continue;
			}
			//
			if ( ( ( errorout == 1 ) || ( errorout == 3 ) ) && !ret1 && !ret2 ) _i_SCH_SUB_PLACE_CM_for_ERROROUT( FM_Side , FM_Pointer , &FM_CO1 , &FM_OSlot , FM_Side2 , FM_Pointer2 , &FM_CO2 , &FM_OSlot2 );
		}
		else if ( FM_TSlot > 0 ) {
			//
			if ( FM_CO1 <= 0 ) { // 2011.04.14 (Testing)
				//
				ret1 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side , FM_Pointer , FM_TSlot , &FM_CO1 , &FM_OSlot , &fail );
				//
				if ( fail ) {
					//------------------------------------------------------
					if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
					//------------------------------------------------------
					return -1; // 2011.05.03
				}
				//
				if ( ret1 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) {
						//------------------------------------------------------
						if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
						//------------------------------------------------------
						return 9;
					}
					//
					ret1 = 0;
					//
				}
				//
			}
			//
			if ( FM_CO1 <= 0 ) { // 2011.04.14 (Testing)
				//------------------------------------------------------
				if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
				//------------------------------------------------------
				Sleep(500);
				//------------------------------------------------------
				if ( needlock ) SCH_MR_UNLOAD_LOCK_START( -1 ); // 2012.04.01
				//------------------------------------------------------
				continue;
			}
			//
			FM_CO2 = 0;
			FM_OSlot2 = 0;
			if ( ( ( errorout == 1 ) || ( errorout == 3 ) ) && !ret1 ) _i_SCH_SUB_PLACE_CM_for_ERROROUT( FM_Side , FM_Pointer , &FM_CO1 , &FM_OSlot , FM_Side2 , FM_Pointer2 , &FM_CO2 , &FM_OSlot2 );
		}
		else if ( FM_TSlot2 > 0 ) {
			//
			if ( FM_CO2 <= 0 ) { // 2011.04.14 (Testing)
				ret2 = _i_SCH_SUB_GET_OUT_CM_AND_SLOT2( FM_Side2 , FM_Pointer2 , FM_TSlot2 , &FM_CO2 , &FM_OSlot2 , &fail );
				//
				if ( fail ) {
					//------------------------------------------------------
					if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
					//------------------------------------------------------
					return -1; // 2011.05.03
				}
				//
				if ( ret2 == -1 ) { // 2011.09.28
					//
					if ( ( errorout == 2 ) || ( errorout == 3 ) ) {
						//------------------------------------------------------
						if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
						//------------------------------------------------------
						return 9;
					}
					//
					ret2 = 0;
					//
				}
				//
			}
			//
			if ( FM_CO2 <= 0 ) { // 2011.04.14 (Testing)
				//------------------------------------------------------
				if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
				//------------------------------------------------------
				Sleep(500);
				//------------------------------------------------------
				if ( needlock ) SCH_MR_UNLOAD_LOCK_START( -1 ); // 2012.04.01
				//------------------------------------------------------
				continue;
			}
			//
			FM_CO1 = 0;
			FM_OSlot = 0;
			if ( ( ( errorout == 1 ) || ( errorout == 3 ) ) && !ret2 ) _i_SCH_SUB_PLACE_CM_for_ERROROUT( FM_Side , FM_Pointer , &FM_CO1 , &FM_OSlot , FM_Side2 , FM_Pointer2 , &FM_CO2 , &FM_OSlot2 );
		}
		//
		break; // 2011.04.14 (Testing)
		//
	}
	//------------------------------------------------------
	if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO1 - 1 , TRUE ); // 2012.04.01
	if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO2 - 1 , TRUE ); // 2012.04.01
	//------------------------------------------------------
	if ( needlock ) SCH_MR_UNLOAD_LOCK_END( -1 ); // 2012.04.01
	//------------------------------------------------------
	//
	Res = _SCH_MACRO_FM_PLACE_TO_CM_Sub( CHECKING_SIDE , FM_TSlot , FM_Side , FM_Pointer , FM_CO1 , FM_OSlot , ret1 , FM_TSlot2 , FM_Side2 , FM_Pointer2 , FM_CO2 , FM_OSlot2 , ret2 , errorout , disapear , realslotoffset );
	//
	//------------------------------------------------------
	if ( ( FM_CO1 >= CM1 ) && ( FM_CO1 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO1 - 1 , FALSE ); // 2012.04.01
	if ( ( FM_CO2 >= CM1 ) && ( FM_CO2 < PM1 ) ) SCH_MR_UNLOAD_LOCK_SET_PLACE( FM_CO2 - 1 , FALSE ); // 2012.04.01
	//------------------------------------------------------
	return Res;
	//====================================================================================================================================================
}

//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================

void _SCH_MACRO_FM_DATABASE_OPERATION( int fms , int mode , int och1 , int slot1 , int omslot1 , int side1 , int pointer1 , int och2 , int slot2 , int omslot2 , int side2 , int pointer2 , int mode1 , int mode2 ) {
//	if      ( mode == MACRO_PICK ) { // 2017.03.09
	if      ( ( mode % 10 ) == MACRO_PICK ) { // 2017.03.09
		if ( slot1 > 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				_i_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , pointer1 , mode1 );
				_i_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , side1 , slot1 );
			}
			else { // 2010.02.02
				_i_SCH_MODULE_Set_TM_PATHORDER( 0 , 0 , 0 );
				_i_SCH_MODULE_Set_TM_TYPE( 0 , 0 , mode1 );
				_i_SCH_MODULE_Set_TM_OUTCH( 0 , 0 , och1 );
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( 0 , 0 , side1 , pointer1 , 0 , 0 );
				_i_SCH_MODULE_Set_TM_WAFER( 0 , 0 , slot1 );
			}
			//
			if      ( ( och1 == IC ) || ( och1 == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_WAFER( omslot1 , 0 );
			}
			else if ( ( och1 == AL ) || ( och1 == F_AL ) ) {
//				_i_SCH_MODULE_Set_MFAL_WAFER( 0 ); // 2008.09.01
				_i_SCH_MODULE_Set_MFALS_WAFER( omslot1 , 0 ); // 2008.09.01
			}
			else if ( och1 >= BM1 ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och1 , omslot1 , 0 , -1 );
			}
			else if ( ( och1 >= PM1 ) && ( och1 < AL ) ) { // 2014.11.07
				_i_SCH_MODULE_Set_PM_WAFER( och1 , omslot1 - 1 , 0 );
			}
		}
		if ( slot2 > 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
				_i_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , pointer2 , mode2 );
				_i_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , side2 , slot2 );
			}
			else { // 2010.02.02
				_i_SCH_MODULE_Set_TM_PATHORDER( 0 , 1 , 0 );
				_i_SCH_MODULE_Set_TM_TYPE( 0 , 1 , mode2 );
				_i_SCH_MODULE_Set_TM_OUTCH( 0 , 1 , och2 );
				_i_SCH_MODULE_Set_TM_SIDE_POINTER( 0 , 1 , side2 , pointer2 , 0 , 0 );
				_i_SCH_MODULE_Set_TM_WAFER( 0 , 1 , slot2 );
			}
			//
			if      ( ( och2 == IC ) || ( och2 == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_WAFER( omslot2 , 0 );
			}
			else if ( ( och2 == AL ) || ( och2 == F_AL ) ) {
//				_i_SCH_MODULE_Set_MFAL_WAFER( 0 ); // 2008.09.01
				_i_SCH_MODULE_Set_MFALS_WAFER( omslot2 , 0 ); // 2008.09.01
			}
			else if ( och2 >= BM1 ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och2 , omslot2 , 0 , -1 );
			}
			else if ( ( och2 >= PM1 ) && ( och2 < AL ) ) { // 2014.11.07
				_i_SCH_MODULE_Set_PM_WAFER( och2 , omslot2 - 1 , 0 );
			}
		}
	}
//	else if ( mode == MACRO_PLACE ) { // 2017.03.09
	else if ( ( mode % 10 ) == MACRO_PLACE ) { // 2017.03.09
		if ( slot1 > 0 ) {
			if      ( ( och1 == IC ) || ( och1 == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( omslot1 , side1 , pointer1 );
				_i_SCH_MODULE_Set_MFIC_WAFER( omslot1 , slot1 );
			}
			else if ( ( och1 == AL ) || ( och1 == F_AL ) ) {
//				_i_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( side1 , pointer1 , mode1 ); // 2008.09.01
//				_i_SCH_MODULE_Set_MFAL_WAFER( slot1 ); // 2008.09.01
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( omslot1 , side1 , pointer1 , mode1 ); // 2008.09.01
				_i_SCH_MODULE_Set_MFALS_WAFER( omslot1 , slot1 ); // 2008.09.01
			}
			else if ( och1 >= BM1 ) {
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och1 , omslot1 , side1 , pointer1 );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och1 , omslot1 , slot1 , mode1 );
			}
			else if ( ( och1 >= PM1 ) && ( och1 < AL ) ) { // 2014.11.07
				_i_SCH_MODULE_Set_PM_SIDE( och1 , omslot1-1 , side1 );
				_i_SCH_MODULE_Set_PM_POINTER( och1 , omslot1-1 , pointer1 );
				_i_SCH_MODULE_Set_PM_WAFER( och1 , omslot1-1 , slot1 );
			}
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
//				_i_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , side1 , 0 ); // 2013.01.11
				_i_SCH_MODULE_Set_FM_WAFER( TA_STATION , 0 ); // 2013.01.11
			}
			else { // 2010.02.02
				_i_SCH_MODULE_Set_TM_WAFER( 0 , 0 , 0 );
			}
		}
		if ( slot2 > 0 ) {
			if      ( ( och2 == IC ) || ( och2 == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( omslot2 , side2 , pointer2 );
				_i_SCH_MODULE_Set_MFIC_WAFER( omslot2 , slot2 );
			}
			else if ( ( och2 == AL ) || ( och2 == F_AL ) ) {
//				_i_SCH_MODULE_Set_MFAL_SIDE_POINTER_BM( side2 , pointer2 , mode2 ); // 2008.09.01
//				_i_SCH_MODULE_Set_MFAL_WAFER( slot2 ); // 2008.09.01
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( omslot2 , side2 , pointer2 , mode2 ); // 2008.09.01
				_i_SCH_MODULE_Set_MFALS_WAFER( omslot2 , slot2 ); // 2008.09.01
			}
			else if ( och2 >= BM1 ) {
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och2 , omslot2 , side2 , pointer2 );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och2 , omslot2 , slot2 , mode2 );
			}
			else if ( ( och2 >= PM1 ) && ( och2 < AL ) ) { // 2014.11.07
				_i_SCH_MODULE_Set_PM_SIDE( och2 , omslot2-1 , side2 );
				_i_SCH_MODULE_Set_PM_POINTER( och2 , omslot2-1 , pointer2 );
				_i_SCH_MODULE_Set_PM_WAFER( och2 , omslot2-1 , slot2 );
			}
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.01.28
//				_i_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , side2 , 0 ); // 2013.01.11
				_i_SCH_MODULE_Set_FM_WAFER( TB_STATION , 0 ); // 2013.01.11
			}
			else {
				_i_SCH_MODULE_Set_TM_WAFER( 0 , 1 , 0 );
			}
		}
	}
}

int  _SCH_MACRO_DATABASE_CHECK_DBINDEX( int *side , int *pt , int *wafer ); // 2012.05.04
BOOL _SCH_MACRO_DATABASE_GET_DBINDEX( int ch , int slot , int *s , int *p );

BOOL _SCH_MACRO_FM_DATABASE_OPERATION_MAINT_INF( BOOL optioncheck , int fms , int mode , int och , int slot1 , int slot2 ) {
	int s , p , w;
	//
	if ( optioncheck ) { // 2012.11.05
		if ( _i_SCH_PRM_GET_MAINTINF_DB_USE() != 1 ) return TRUE; // 2012.05.04
	}
	//
//	if      ( mode == MACRO_PICK ) { // 2017.03.09
	if      ( ( mode % 10 ) == MACRO_PICK ) { // 2017.03.09
		if ( slot1 > 0 ) {
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_MFIC_SIDE( slot1 );
				p = _i_SCH_MODULE_Get_MFIC_POINTER( slot1 );
				w = _i_SCH_MODULE_Get_MFIC_WAFER( slot1 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_MFALS_SIDE( slot1 );
				p = _i_SCH_MODULE_Get_MFALS_POINTER( slot1 );
				w = _i_SCH_MODULE_Get_MFALS_WAFER( slot1 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( och >= BM1 ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_BM_SIDE( och , slot1 );
				p = _i_SCH_MODULE_Get_BM_POINTER( och , slot1 );
				w = _i_SCH_MODULE_Get_BM_WAFER( och , slot1 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				if ( !_SCH_MACRO_DATABASE_GET_DBINDEX( och , slot1 , &s , &p ) ) {
					s = 0; // 2012.01.21
					p = 0; // 2012.05.04
					w = 0; // 2012.05.04
				}
				else {
					//
					w = slot1;
					//
				}
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) { // 2014.11.07
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_PM_SLOT_DEPTH ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_PM_SIDE( och , slot1 - 1 );
				p = _i_SCH_MODULE_Get_PM_POINTER( och , slot1 - 1 );
				w = _i_SCH_MODULE_Get_PM_WAFER( och , slot1 - 1 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else {
				return FALSE;
			}
			//----------------------------------------------------------------
			if ( !optioncheck ) { // 2012.11.05
				if ( w <= 0 ) return FALSE;
			}
			//----------------------------------------------------------------
//			if ( s != -1 ) { // 2012.01.21 // 2012.05.04
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					_i_SCH_MODULE_Set_FM_POINTER_MODE( TA_STATION , p , 0 );
					_i_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , s , w );
				}
				else {
					_i_SCH_MODULE_Set_TM_PATHORDER( 0 , TA_STATION , 0 );
					_i_SCH_MODULE_Set_TM_TYPE( 0 , TA_STATION , 0 );
					_i_SCH_MODULE_Set_TM_OUTCH( 0 , TA_STATION , och );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( 0 , TA_STATION , s , p , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( 0 , TA_STATION , w );
				}
//			}
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_WAFER( slot1 , 0 );
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				_i_SCH_MODULE_Set_MFALS_WAFER( slot1 , 0 );
			}
			else if ( och >= BM1 ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot1 , 0 , -1 );
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) { // 2014.11.07
				_i_SCH_MODULE_Set_PM_WAFER( och , slot1 - 1 , 0 );
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) { // 2012.01.21
//				if ( s != -1 ) { // 2012.05.04
				if ( w > 0 ) { // 2012.05.04
					if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
						_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_SUPPLY );
						_i_SCH_MODULE_Inc_FM_OutCount( s );
					}
					else {
						_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_RUNNING );
						_i_SCH_MODULE_Inc_TM_OutCount( s );
					}
				}
			}
			//----------------------------------------------------------------
		}
		if ( slot2 > 0 ) {
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_MFIC_SIDE( slot2 );
				p = _i_SCH_MODULE_Get_MFIC_POINTER( slot2 );
				w = _i_SCH_MODULE_Get_MFIC_WAFER( slot2 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_MFALS_SIDE( slot2 );
				p = _i_SCH_MODULE_Get_MFALS_POINTER( slot2 );
				w = _i_SCH_MODULE_Get_MFALS_WAFER( slot2 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( och >= BM1 ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_BM_SIDE( och , slot2 );
				p = _i_SCH_MODULE_Get_BM_POINTER( och , slot2 );
				w = _i_SCH_MODULE_Get_BM_WAFER( och , slot2 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
				//
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) { // 2014.11.07
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_PM_SLOT_DEPTH ) ) return FALSE;
				//
				s = _i_SCH_MODULE_Get_PM_SIDE( och , slot2 - 1 );
				p = _i_SCH_MODULE_Get_PM_POINTER( och , slot2 - 1 );
				w = _i_SCH_MODULE_Get_PM_WAFER( och , slot2 - 1 );
				//
				_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w );
				//
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				if ( !_SCH_MACRO_DATABASE_GET_DBINDEX( och , slot2 , &s , &p ) ) {
					s = 0; // 2012.01.21
					p = 0; // 2012.05.04
					w = 0; // 2012.05.04
				}
				else {
					//
					w = slot2;
					//
				}
			}
			else {
				return FALSE;
			}
			//----------------------------------------------------------------
			if ( !optioncheck ) { // 2012.11.05
				if ( w <= 0 ) return FALSE;
			}
			//----------------------------------------------------------------
//			if ( s != -1 ) { // 2012.05.04
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					_i_SCH_MODULE_Set_FM_POINTER_MODE( TB_STATION , p , 0 );
					_i_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , s , w );
				}
				else {
					_i_SCH_MODULE_Set_TM_PATHORDER( 0 , TB_STATION , 0 );
					_i_SCH_MODULE_Set_TM_TYPE( 0 , TB_STATION , 0 );
					_i_SCH_MODULE_Set_TM_OUTCH( 0 , TB_STATION , och );
					_i_SCH_MODULE_Set_TM_SIDE_POINTER( 0 , TB_STATION , s , p , 0 , 0 );
					_i_SCH_MODULE_Set_TM_WAFER( 0 , TB_STATION , w );
				}
//			}
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				_i_SCH_MODULE_Set_MFIC_WAFER( slot2 , 0 );
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				_i_SCH_MODULE_Set_MFALS_WAFER( slot2 , 0 );
			}
			else if ( och >= BM1 ) {
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot2 , 0 , -1 );
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) { // 2014.11.07
				_i_SCH_MODULE_Set_PM_WAFER( och , slot2 - 1 , 0 );
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) { // 2012.01.21
//				if ( s != -1 ) { // 2012.05.04
				if ( w > 0 ) { // 2012.05.04
					if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
						_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_SUPPLY );
						_i_SCH_MODULE_Inc_FM_OutCount( s );
					}
					else {
						_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_RUNNING );
						_i_SCH_MODULE_Inc_TM_OutCount( s );
					}
				}
			}
			//----------------------------------------------------------------
		}
	}
//	else if ( mode == MACRO_PLACE ) { // 2017.03.09
	else if ( ( mode % 10 ) == MACRO_PLACE ) { // 2017.03.09
		if ( slot1 > 0 ) {
			//----------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				s = _i_SCH_MODULE_Get_FM_SIDE( TA_STATION );
				p = _i_SCH_MODULE_Get_FM_POINTER( TA_STATION );
				w = _i_SCH_MODULE_Get_FM_WAFER( TA_STATION );
			}
			else {
				s = _i_SCH_MODULE_Get_TM_SIDE( 0 , TA_STATION );
				p = _i_SCH_MODULE_Get_TM_POINTER( 0 , TA_STATION );
				w = _i_SCH_MODULE_Get_TM_WAFER( 0 , TA_STATION );
			}
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
			//----------------------------------------------------------------
			if ( !optioncheck ) { // 2012.11.05
				if ( w <= 0 ) return FALSE;
			}
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot1 , s , p );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot1 , w );
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( slot1 , s , p , 0 );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot1 , w );
			}
			else if ( och >= BM1 ) {
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , slot1 , s , p );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot1 , w , 0 );
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) { // 2014.11.07
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_PM_SLOT_DEPTH ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_PM_SIDE( och , slot1-1 , s );
				_i_SCH_MODULE_Set_PM_POINTER( och , slot1-1 , p );
				_i_SCH_MODULE_Set_PM_WAFER( och , slot1-1 , w );
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) { // 2012.01.21
				//
				if ( ( slot1 <= 0 ) || ( slot1 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				if ( w > 0 ) { // 2012.05.04
					_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
					if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
						_i_SCH_MODULE_Inc_FM_InCount( s );
					}
					else {
						_i_SCH_MODULE_Inc_TM_InCount( s );
					}
				}
				//
			}
			else {
				return FALSE;
			}
			//----------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
//				_i_SCH_MODULE_Set_FM_SIDE_WAFER( TA_STATION , s , 0 ); // 2013.01.11
				_i_SCH_MODULE_Set_FM_WAFER( TA_STATION , 0 ); // 2013.01.11
			}
			else {
				_i_SCH_MODULE_Set_TM_WAFER( 0 , TA_STATION , 0 );
			}
			//----------------------------------------------------------------
		}
		if ( slot2 > 0 ) {
			//----------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				s = _i_SCH_MODULE_Get_FM_SIDE( TB_STATION );
				p = _i_SCH_MODULE_Get_FM_POINTER( TB_STATION );
				w = _i_SCH_MODULE_Get_FM_WAFER( TB_STATION );
			}
			else {
				s = _i_SCH_MODULE_Get_TM_SIDE( 0 , TB_STATION );
				p = _i_SCH_MODULE_Get_TM_POINTER( 0 , TB_STATION );
				w = _i_SCH_MODULE_Get_TM_WAFER( 0 , TB_STATION );
			}
			//
			_SCH_MACRO_DATABASE_CHECK_DBINDEX( &s , &p , &w ); // 2012.05.04
			//
			//----------------------------------------------------------------
			if ( !optioncheck ) { // 2012.11.05
				if ( w <= 0 ) return FALSE;
			}
			//----------------------------------------------------------------
			if      ( ( och == IC ) || ( och == F_IC ) ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_MFIC_SIDE_POINTER( slot2 , s , p );
				_i_SCH_MODULE_Set_MFIC_WAFER( slot2 , w );
			}
			else if ( ( och == AL ) || ( och == F_AL ) ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_MFALS_SIDE_POINTER_BM( slot2 , s , p , 0 );
				_i_SCH_MODULE_Set_MFALS_WAFER( slot2 , w );
			}
			else if ( och >= BM1 ) {
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_BM_SIDE_POINTER( och , slot2 , s , p );
				_i_SCH_MODULE_Set_BM_WAFER_STATUS( och , slot2 , w , 0 );
			}
			else if ( ( och >= PM1 ) && ( och < AL ) ) { // 2014.11.07
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_PM_SLOT_DEPTH ) ) return FALSE;
				//
				_i_SCH_MODULE_Set_PM_SIDE( och , slot2-1 , s );
				_i_SCH_MODULE_Set_PM_POINTER( och , slot2-1 , p );
				_i_SCH_MODULE_Set_PM_WAFER( och , slot2-1 , w );
			}
			else if ( ( och >= CM1 ) && ( och < PM1 ) ) { // 2012.01.21
				//
				if ( ( slot2 <= 0 ) || ( slot2 > MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
				//
				if ( w > 0 ) { // 2012.05.04
					//
					_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_CM_END );
					if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
						_i_SCH_MODULE_Inc_FM_InCount( s );
					}
					else {
						_i_SCH_MODULE_Inc_TM_InCount( s );
					}
				}
				//
			}
			else {
				return FALSE;
			}
			//----------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
//				_i_SCH_MODULE_Set_FM_SIDE_WAFER( TB_STATION , s , 0 ); // 2013.01.11
				_i_SCH_MODULE_Set_FM_WAFER( TB_STATION , 0 ); // 2013.01.11
			}
			else {
				_i_SCH_MODULE_Set_TM_WAFER( 0 , TB_STATION , 0 );
			}
			//----------------------------------------------------------------
		}
	}
	else {
		return FALSE;
	}
	//
	return TRUE;
}



int _SCH_MACRO_TM_MOVE_OPERATION_0( int side , int mode , int ch , int arm , int slot , int depth );


int  _SCH_MACRO_FM_MOVE_OPERATION( int fms , int side , int mode , int ch , int slot1 , int slot2 ) {
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
		return _SCH_MACRO_TM_MOVE_OPERATION_0( side , mode , ch , ( slot1 > 0 ) ? 0 : 1 , ( slot1 > 0 ) ? slot1 : slot2 , ( slot1 > 0 ) ? slot1 : slot2 );
	}
	else {
		if ( EQUIP_ROBOT_FM_MOVE_CONTROL( fms , side , mode , ch , slot1 , slot2 , FALSE ) != SYS_SUCCESS ) {
			_i_SCH_LOG_LOT_PRINTF( side , "FM Handling Move Fail %s(%d,%d)%cWHFMMOVEFAIL %s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot1 , slot2 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( ch ) , mode );
			return SYS_ABORTED;
		}
		return SYS_SUCCESS;
	}
}

