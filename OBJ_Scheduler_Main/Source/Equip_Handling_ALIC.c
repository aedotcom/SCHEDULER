#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "IO_Part_log.h"
#include "Robot_Handling.h"
#include "Robot_Animation.h"
#include "Timer_Handling.h"
#include "sch_multi_alic.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_sub.h"
#include "iolog.h"
#include "Equip_Handling.h"
#include "FA_Handling.h"
#include "sch_estimate.h"


/*

FIC_MULTI_CONTROL	0	S
FIC_MULTI_CONTROL	1	T	SLOT = 1	=> Function
					SLOT > 1	=> Timer
FIC_MULTI_CONTROL	2	S	IC to AL (for OUT)			<= FAL_MULTI_CONTROL Must 0,1,2
FIC_MULTI_CONTROL	3	T	IC to AL (for OUT)			<= FAL_MULTI_CONTROL Must 3




FAL_MULTI_CONTROL	0	S
FAL_MULTI_CONTROL	1	S	Double
FAL_MULTI_CONTROL	2	S	Can Change 3(T)
FAL_MULTI_CONTROL	3	T	Can Change 2(T)
*/


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Place and Pick with FALIC
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_FM_IC_HASNOT_A_WAFER( int CHECKING_SIDE , int *slot , int *slot2 ) {
	int i , k = 0;
	*slot  = 0;
	*slot2 = 0;
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) )	return 0;
	for ( i = _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i >= 1 ; i-- ) {
		if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , i ) <= 0 ) {
			*slot2 = *slot;
			*slot = i;
			k++;
		}
	}
	return k;
}
//
//
// 
/*
//
//
// 2018.06.14
//
//
int EQUIP_PLACE_AND_PICK_WITH_FALIC_Sub( int fms , int CHECKING_SIDE , int Chamber0 , int RSlot1 , int RSlot2 , int SrcCas , int swch , int PickPlaceMode , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int sideA , int sideB , int PointerA , int PointerB , BOOL MaintInfUse ) {
	int timex = 100;
	char ParamF[256];
	char Buffer[32];
	int i , Mf , Mr , OSlot1 , OSlot2 , Slot1 , Slot2 , ACSlot , ACSlot2 , MrMode , MrRes , swch2 , swch3 , AC_Off , AC1_Off = 0 , AC2_Off = 0;
	BOOL FM_Run , Rb_Run , Rb_RunOrg;
	long Mon_Goal;
	BOOL update;
	int sourcewaf;
	int slw0 , slw1 , swaf1 , slw2 , swaf2;
	int aborted , sd0 , sp0;
	int l_c; // 2008.09.14
	int xdm1 , xdm2; // 2009.09.23
	int Chamber , AL_swap; // 2010.12.22
	char MsgAppchar[2]; / * 2013.04.26 * /
	char MsgSltchar[16]; / * 2013.05.23 * /
	//
	//===============================================================
	//
	long Sim_Goal; // 2016.12.09
	int  Sim_Trg; // 2016.12.09
	//
	//===============================================================
	//

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
//	if ( Chamber == AL ) { // 2008.01.04
	if ( ( Chamber0 == F_AL ) || ( Chamber0 == AL ) ) { // 2008.01.04
		//
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) )	return SYS_SUCCESS;
		//
		Chamber = Chamber0; // 2010.12.22
		AL_swap = FALSE; // 2010.12.22
	}
	else {
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) )	return SYS_SUCCESS;
		//
		if ( ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) || ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) ) { // 2010.12.22
			//
			Chamber = AL; // 2010.12.22
			AL_swap = TRUE; // 2010.12.22
			//
		}
		else {
			//
			Chamber = Chamber0; // 2010.12.22
			AL_swap = FALSE; // 2010.12.22
			//
		}
		//
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] FMS=%d,Chamber=%d,RSlot1=%d,RSlot2=%d,SrcCas=%d,swch=%d,PickPlaceMode=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_AND_PICK_WITH_FALIC" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , RSlot1 , RSlot2 , SrcCas , swch , PickPlaceMode );
	}
	//======================================================================================================================================================

	Rb_RunOrg = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms );
//	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( EQUIP_ADDRESS_FM(fms) >= 0 ) ) // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( EQUIP_ADDRESS_FM(fms) < 0 ) || ( GET_RUN_LD_CONTROL(4) > 0 ) ) { // 2014.01.09
		FM_Run = FALSE;
	}
	else {
		FM_Run = TRUE;
	}
	//
//	if ( Chamber == AL ) { // 2008.01.04
	if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
//		OSlot1 = RSlot1; // 2008.08.29
//		OSlot2 = RSlot2; // 2008.08.29
		//
		OSlot1 = RSlot1 % 10000; // 2008.08.29
		OSlot2 = RSlot2 % 10000; // 2008.08.29
		//
		AC1_Off = RSlot1 / 10000; // 2008.08.29
		AC2_Off = RSlot2 / 10000; // 2008.08.29
	}
	else {
		if ( ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) && ( CHECKING_SIDE >= TR_CHECKING_SIDE ) && ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) ) { // 2010.12.22
			ACSlot = EQUIP_FM_IC_HASNOT_A_WAFER( CHECKING_SIDE , &Slot1 , &Slot2 );
			if      ( ACSlot == 0 ) {
				return SYS_ABORTED;
			}
			else if ( ACSlot == 1 ) {
				if ( RSlot1 > 0 ) OSlot1 = Slot1;
				else              OSlot1 = 0;
				if ( RSlot2 > 0 ) OSlot2 = Slot1;
				else              OSlot2 = 0;
			}
			else {
				if ( RSlot1 > 0 ) OSlot1 = Slot1;
				else              OSlot1 = 0;
				if ( RSlot2 > 0 ) OSlot2 = Slot2;
				else              OSlot2 = 0;
			}
		}
		else {
			OSlot1 = RSlot1;
			OSlot2 = RSlot2;
		}
	}
	//
	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); / * 2013.04.26 * /
	//
	//========================================================================================================================================================
	// 2005.11.28
	//========================================================================================================================================================
	if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_ALL ) || ( PickPlaceMode == FAL_RUN_MODE_PICK_ALL ) ) {
//		if ( Chamber == AL ) { // 2008.01.04
		if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
			return SYS_ABORTED; // IC only
		}
		//=========================================
		Slot1 = OSlot1;
		Slot2 = OSlot2;
		//
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
			ACSlot  = Slot1;
			ACSlot2 = Slot2;
		}
		else {
			ACSlot  = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
			ACSlot2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
		}
		//=========================================
		// 2009.09.23
		//=========================================
		if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
//			xdm1 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( sideA , PointerA ) , 1 ); // 2017.09.11
			xdm1 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( sideA , PointerA ) , 1 , 11 , sideA , PointerA ); // 2017.09.11
		}
		else {
			xdm1 = -1;
		}
		if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
//			xdm2 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( sideB , PointerB ) , 1 ); // 2017.09.11
			xdm2 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( sideB , PointerB ) , 1 , 11 , sideB , PointerB ); // 2017.09.11
		}
		else {
			xdm2 = -1;
		}
		//=========================================
		if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
			if ( PickPlaceMode == FAL_RUN_MODE_PICK_ALL ) {
				//
				if ( xdm1 != xdm2 ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(A:%d,%d)%cWHFMPICKSTART IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(A:%d,D%d)%cWHFMPICKSTART IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(A:%d,D%d-%d)%cWHFMPICKSTART IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
					}
					//
					FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
					//
					if      ( xdm2 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(B:%d,%d)%cWHFMPICKSTART IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else if ( xdm2 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(B:%d,D%d)%cWHFMPICKSTART IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(B:%d,D%d-%d)%cWHFMPICKSTART IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
					}
				}
				else {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(%d,%d:%d,%d)%cWHFMPICKSTART IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(%d,D%d:%d,D%d)%cWHFMPICKSTART IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPICKSTART IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
				}
				//
				//==========================================================================================
				// 2007.03.19
				//==========================================================================================
				if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
					if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
					}
					if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
					}
				}
				//==========================================================================================
			}
			else {
				//
				if ( xdm1 != xdm2 ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(A:%d,%d)%cWHFMPLACESTART IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(A:%d,D%d)%cWHFMPLACESTART IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(A:%d,D%d-%d)%cWHFMPLACESTART IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
					}
					//
					FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
					//
					if      ( xdm2 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(B:%d,%d)%cWHFMPLACESTART IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else if ( xdm2 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(B:%d,D%d)%cWHFMPLACESTART IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(B:%d,D%d-%d)%cWHFMPLACESTART IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
					}
				}
				else {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(%d,%d:%d,%d)%cWHFMPLACESTART IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(%d,D%d:%d,D%d)%cWHFMPLACESTART IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPLACESTART IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
				}
				//==========================================================================================
				// 2007.03.19
				//==========================================================================================
				if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
					if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
					}
					if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
					}
				}
				//==========================================================================================
			}
		}
		//=========================================
		//
		switch( PickPlaceMode ) {
		case FAL_RUN_MODE_PLACE_ALL :
			sprintf( ParamF , "PLACE_WAFER%s IC %d %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot , ACSlot2 );
			MrMode = RB_MODE_PLACE;
			MrRes = RB_PLACEDONE;
			break;
		case FAL_RUN_MODE_PICK_ALL :
			sprintf( ParamF , "PICK_WAFER%s IC %d %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot , ACSlot2 );
			MrMode = RB_MODE_PICK;
			MrRes = RB_PICKDONE;
			break;
			//
		default :
			break;
			//
		}
		//
		if ( !FM_Run ) { // 2016.12.09
			//
			switch( PickPlaceMode ) {
			case FAL_RUN_MODE_PLACE_ALL :
				Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
				break;
			case FAL_RUN_MODE_PICK_ALL :
				Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
				break;
				//
			default :
				//
				Sim_Trg = 0;
				break;
				//
			}
			//
			if ( Sim_Trg > 0 ) {
				Sim_Goal = Clock_Goal_Get_Start_Time();
				timex = Sim_Trg / 20;
				if ( timex <= 0 ) timex = 1;
			}
			else {
				timex = 100;
			}
			//
		}
		else {
			timex = 100;
		}
		//
		EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
		//
		Rb_Run = Rb_RunOrg;
		//
		if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , MrMode , Chamber , ACSlot , ACSlot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
		//
		aborted = FALSE;
		//
		if ( Rb_Run == 1 ) {
			if ( FM_Run ) {
				//_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
				if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
					Mf = SYS_RUNNING; // 2008.04.03
				}
				else {
					Mf = SYS_ABORTED; // 2008.04.03
				}
			}
			else {
				Mf = SYS_RUNNING; // 2008.04.03
			}
			//
//			Mf = SYS_RUNNING; // 2008.04.03
			while ( TRUE ) {
				//========================
				_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
				//========================
				Mon_Goal = Clock_Goal_Get_Start_Time();
				if ( FM_Run ) {
					if ( Mf == SYS_RUNNING ) {
						Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
					}
					if ( Mf == SYS_ABORTED ) {
						aborted = TRUE;
						break;
					}
					if ( Rb_Run == 1 ) {
						Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == MrRes ) {
							if ( Mf != SYS_RUNNING ) break;
							Rb_Run = 0;
						}
					}
					else {
						if ( Mf != SYS_RUNNING ) break;
					}
				}
				else {
					//
					//===============================================================
					//
					if ( Sim_Trg > 0 ) {
						if ( Mf == SYS_RUNNING ) {
							if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
								Mf = SYS_SUCCESS;
							}
						}
						if ( Rb_Run == 1 ) {
							Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
							if ( update ) _i_SCH_IO_MTL_SAVE();
							if ( Mr == MrRes ) {
								if ( Mf != SYS_RUNNING ) break;
								Rb_Run = 0;
							}
						}
						else {
							if ( Mf != SYS_RUNNING ) break;
						}
					}

					//
					//===============================================================
					//
					else {
	//					if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) { // 2007.12.28
						if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == MrRes ) { // 2007.12.28
							if ( update ) _i_SCH_IO_MTL_SAVE();
							break;
						}
						if ( update ) _i_SCH_IO_MTL_SAVE();
					}
				}
				l_c = 0; // 2008.09.14
				do {
					if ( MANAGER_ABORT() ) {
						aborted = TRUE;
						break;
					}
					//===================================================================
					// 2005.10.26
					//===================================================================
					if ( FM_Run ) {
						if ( Mf == SYS_RUNNING ) {
							if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
								break;
							}
						}
					}
					//===================================================================
					//
					switch( GET_RUN_LD_CONTROL( 0 ) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
					//===================================================================
					//Sleep(2); // 2005.10.26
					// Sleep(1); // 2008.09.14
					if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
					l_c++; // 2008.09.14
				}
				while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
				//===================================================================
				if ( aborted ) break;
				//===================================================================
			}
		}
		else {
			//
			if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( MrRes == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
				}
			}
			//
			if ( FM_Run ) {
				if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) aborted = TRUE;
			}
			if ( !aborted ) {
				//
				if ( Rb_Run == 4 ) { // 2015.12.16
					ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , MrMode , Chamber , ACSlot , ACSlot2 , -1 , MaintInfUse , TRUE );
				}
				//==================================================================================================
				// 2006.11.09
				//==================================================================================================
				//
				if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
					while ( TRUE ) {
						if ( MrRes == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
					}
				}
				//
				_i_SCH_IO_MTL_SAVE();
				//==================================================================================================
			}
		}
		//======================================================================================================================================================
		if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
			if ( PickPlaceMode == FAL_RUN_MODE_PICK_ALL ) {
				if ( aborted ) {
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
				else {
					//
					if ( xdm1 != xdm2 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(A:%d,%d)%cWHFMPICKSUCCESS IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(A:%d,D%d)%cWHFMPICKSUCCESS IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(A:%d,D%d-%d)%cWHFMPICKSUCCESS IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
						}
						//
						FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
						//
						if      ( xdm2 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(B:%d,%d)%cWHFMPICKSUCCESS IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else if ( xdm2 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(B:%d,D%d)%cWHFMPICKSUCCESS IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(B:%d,D%d-%d)%cWHFMPICKSUCCESS IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
						}
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(%d,%d:%d,%d)%cWHFMPICKSUCCESS IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(%d,D%d:%d,D%d)%cWHFMPICKSUCCESS IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPICKSUCCESS IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
					}
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
			}
			else {
				if ( aborted ) {
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
				else {
					//
					if ( xdm1 != xdm2 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(A:%d,%d)%cWHFMPLACESUCCESS IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(A:%d,D%d)%cWHFMPLACESUCCESS IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(A:%d,D%d-%d)%cWHFMPLACESUCCESS IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
						}
						//
						FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
						//
						if      ( xdm2 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(B:%d,%d)%cWHFMPLACESUCCESS IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else if ( xdm2 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(B:%d,D%d)%cWHFMPLACESUCCESS IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(B:%d,D%d-%d)%cWHFMPLACESUCCESS IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
						}
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(%d,%d:%d,%d)%cWHFMPLACESUCCESS IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(%d,D%d:%d,D%d)%cWHFMPLACESUCCESS IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPLACESUCCESS IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
					}
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
			}
		}
		//======================================================================================================================================================
		if ( aborted ) return SYS_ABORTED;
		//======================================================================================================================================================
	}
	//========================================================================================================================================================
	else {
		//=============================================================================================
		// 2007.10.17
		//=============================================================================================
//		if ( ( Chamber == AL ) && ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) && ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) && ( OSlot1 > 0 ) && ( OSlot2 > 0 ) ) { // 2008.09.10
		if ( ( Chamber == AL ) && ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE ) ) && ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) && ( OSlot1 > 0 ) && ( OSlot2 > 0 ) ) { // 2008.09.10
			//
			if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE ) ) { // 2008.09.10
				//------------------------------------------------------------------------------------
				//-- Place Part
				//------------------------------------------------------------------------------------
				//
				if ( !FM_Run ) { // 2016.12.09
					//
					Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
					//
					if ( Sim_Trg > 0 ) {
						Sim_Goal = Clock_Goal_Get_Start_Time();
						timex = Sim_Trg / 20;
						if ( timex <= 0 ) timex = 1;
					}
					else {
						timex = 100;
					}
					//
				}
				else {
					Sim_Trg = 0; // 2017.03.24
					timex = 100;
				}
				//
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%d,%d:%d,%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//
				Rb_Run = Rb_RunOrg;
				//
				sprintf( ParamF , "PLACE_WAFER%s AL %d %d %d %d" , MsgAppchar , OSlot1 , OSlot2 , 1 , 2 );
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 1 , 2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
				//
				aborted = FALSE;
				//
				if ( Rb_Run == 1 ) {
					if ( FM_Run ) {
	//					_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
						if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
							Mf = SYS_RUNNING; // 2008.04.03
						}
						else {
							Mf = SYS_ABORTED; // 2008.04.03
						}
					}
					else {
						Mf = SYS_RUNNING; // 2008.04.03
					}
					//
	//				Mf = SYS_RUNNING; // 2008.04.03
					while ( TRUE ) {
						//========================
						_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
						//========================
						Mon_Goal = Clock_Goal_Get_Start_Time();
						if ( FM_Run ) {
							if ( Mf == SYS_RUNNING ) {
								Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
							}
							if ( Mf == SYS_ABORTED ) {
								aborted = TRUE;
								break;
							}
							if ( Rb_Run == 1 ) {
								Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
								if ( update ) _i_SCH_IO_MTL_SAVE();
								if ( Mr == RB_PLACEDONE ) {
									if ( Mf != SYS_RUNNING ) break;
									Rb_Run = 0;
								}
							}
							else {
								if ( Mf != SYS_RUNNING ) break;
							}
						}
						else {
							//
							//===============================================================
							//
							if ( Sim_Trg > 0 ) { // 2016.12.09
								if ( Mf == SYS_RUNNING ) {
									if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
										Mf = SYS_SUCCESS;
									}
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PLACEDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							//
							//===============================================================
							//
							else {
								if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) {
									if ( update ) _i_SCH_IO_MTL_SAVE();
									break;
								}
								if ( update ) _i_SCH_IO_MTL_SAVE();
							}
							//
						}
						l_c = 0; // 2008.09.14
						do {
							if ( MANAGER_ABORT() ) {
								aborted = TRUE;
								break;
							}
							//===================================================================
							// 2005.10.26
							//===================================================================
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
										break;
									}
								}
							}
							//===================================================================
							//
							switch( GET_RUN_LD_CONTROL( 0 ) ) {
							case 1 :
								timex = 0;
								break;
							case 2 :
								timex = 20;
								break;
							}
							//
							//===================================================================
							//Sleep(2); // 2005.10.26
							// Sleep(1); // 2008.09.14
							if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
							l_c++; // 2008.09.14
						}
						while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
						//
						if ( aborted ) break;
					}
				}
				else {
					//
					if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
						while ( TRUE ) {
							if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
						}
					}
					//
					if ( FM_Run ) {
						if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) aborted = TRUE;
					}
					//
					if ( !aborted ) {
						//
						if ( Rb_Run == 4 ) { // 2015.12.16
							ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 1 , 2 , -1 , MaintInfUse , TRUE );
						}
						//==================================================================================================
						// 2006.11.09
						//==================================================================================================
						//
						if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							//
							while ( TRUE ) {
								if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
							}
							//
						}
						_i_SCH_IO_MTL_SAVE();
						//==================================================================================================
					}
				}
				//======================================================================================================================================================
				//======================================================================================================================================================
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					if ( aborted ) {
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%d,%d:%d,%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
						//
					}
				}
				//======================================================================================================================================================
				if ( aborted ) return SYS_ABORTED;
				//
			}
			//
			if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) ) { // 2008.09.10
				//------------------------------------------------------------------------------------
				//-- Run Part
				//------------------------------------------------------------------------------------
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%d:%d)%cWHFM%sRUN %d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//
				if ( FM_Run ) {
					//===================================================
					sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d %d" , MsgAppchar , OSlot1 , OSlot2 , SrcCas - CM1 + 1 , 1 , 2 );
					if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
					//===================================================
					EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					//
					if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) return SYS_ABORTED;
				}
				else {
					if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
				}
				//======================================================================================================================================================
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%d:%d)%cWHFM%sSUCCESS %d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//======================================================================================================================================================
			}
			if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) ) { // 2008.09.10
				//
				//------------------------------------------------------------------------------------
				//-- Pick Part
				//------------------------------------------------------------------------------------
				//
				if ( !FM_Run ) { // 2016.12.09
					//
					Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
					//
					if ( Sim_Trg > 0 ) {
						Sim_Goal = Clock_Goal_Get_Start_Time();
						timex = Sim_Trg / 20;
						if ( timex <= 0 ) timex = 1;
					}
					else {
						timex = 100;
					}
					//
				}
				else {
					Sim_Trg = 0; // 2017.03.24
					timex = 100;
				}
				//
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%d,%d:%d,%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//
				Rb_Run = Rb_RunOrg;
				//
				sprintf( ParamF , "PICK_WAFER%s AL %d %d %d %d" , MsgAppchar , OSlot1 , OSlot2 , 1 , 2 );
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 1 , 2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
				//
				aborted = FALSE;
				//
				if ( Rb_Run == 1 ) {
					if ( FM_Run ) {
	//					_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
						if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
							Mf = SYS_RUNNING; // 2008.04.03
						}
						else {
							Mf = SYS_ABORTED; // 2008.04.03
						}
					}
					else {
						Mf = SYS_RUNNING; // 2008.04.03
					}
					//
	//				Mf = SYS_RUNNING; // 2008.04.03
					while ( TRUE ) {
						//========================
						_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
						//========================
						Mon_Goal = Clock_Goal_Get_Start_Time();
						if ( FM_Run ) {
							if ( Mf == SYS_RUNNING ) {
								Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
							}
							if ( Mf == SYS_ABORTED ) {
								aborted = TRUE;
								break;
							}
							if ( Rb_Run == 1 ) {
								Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
								if ( update ) _i_SCH_IO_MTL_SAVE();
								if ( Mr == RB_PICKDONE ) {
									if ( Mf != SYS_RUNNING ) break;
									Rb_Run = 0;
								}
							}
							else {
								if ( Mf != SYS_RUNNING ) break;
							}
						}
						else {
							//
							//===============================================================
							//
							if ( Sim_Trg > 0 ) { // 2016.12.09
								if ( Mf == SYS_RUNNING ) {
									if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
										Mf = SYS_SUCCESS;
									}
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PICKDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							//
							//===============================================================
							//
							else {
								if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PICKDONE ) {
									if ( update ) _i_SCH_IO_MTL_SAVE();
									break;
								}
								if ( update ) _i_SCH_IO_MTL_SAVE();
							}
						}
						l_c = 0; // 2008.09.14
						do {
							if ( MANAGER_ABORT() ) {
								aborted = TRUE;
								break;
							}
							//===================================================================
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
										break;
									}
								}
							}
							//===================================================================
							//
							switch( GET_RUN_LD_CONTROL( 0 ) ) {
							case 1 :
								timex = 0;
								break;
							case 2 :
								timex = 20;
								break;
							}
							//
							//
							//===================================================================
							//Sleep(2); // 2005.10.26
							// Sleep(1); // 2008.09.14
							if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
							l_c++; // 2008.09.14
						}
						while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
						//
						if ( aborted ) break;
					}
				}
				else {
					//
					if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
						while ( TRUE ) {
							if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
						}
					}
					//
					if ( FM_Run ) {
						if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) aborted = TRUE;
					}
					if ( !aborted ) {
						//
						if ( Rb_Run == 4 ) { // 2015.12.16
							ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 1 , 2 , -1 , MaintInfUse , TRUE );
						}
						//==================================================================================================
						// 2006.11.09
						//==================================================================================================
						//
						if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							while ( TRUE ) {
								if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
							}
						}
						//
						_i_SCH_IO_MTL_SAVE();
						//==================================================================================================
					}
				}
				//======================================================================================================================================================
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					if ( aborted ) {
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%d,%d:%d,%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
						//
					}
				}
				//======================================================================================================================================================
				if ( aborted ) return SYS_ABORTED;
				//======================================================================================================================================================
				//
			}
		}
		//======================================================================================================================================================
		else {
			for ( i = 0 ; i < 2 ; i++ ) {
				if ( i == 0 ) {
					if ( OSlot1 > 0 ) {
						Slot1 = OSlot1;
						Slot2 = 0;
						//
						sourcewaf = sourcewaferidA;
						//
						slw0 = Slot1;
						slw1 = Slot1;
						swaf1 = sourcewaf;
						slw2 = 0;
						swaf2 = 0;
						//
						sd0 = sideA;
						sp0 = PointerA;
						//
						AC_Off = AC1_Off; // 2008.08.29
					}
					else {
						continue;
					}
				}
				else {
					if ( OSlot2 > 0 ) {
						Slot1 = 0;
						Slot2 = OSlot2;
						//
						sourcewaf = sourcewaferidB;
						//
						slw0 = Slot2;
						slw1 = 0;
						swaf1 = 0;
						slw2 = Slot2;
						swaf2 = sourcewaf;
						//
						sd0 = sideB;
						sp0 = PointerB;
						//
						AC_Off = AC2_Off; // 2008.08.29
					}
					else {
						continue;
					}
				}
				//
				//=========================================
				// 2009.09.23
				//=========================================
				if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
//					xdm1 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( sd0 , sp0 ) , 1 ); // 2017.09.11
					xdm1 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( sd0 , sp0 ) , 1 , 11 , sd0 , sp0 ); // 2017.09.11
				}
				else {
					xdm1 = -1;
				}
				//=========================================
				//
				if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE ) ) {
					//
					//------------------------------------------------------------------------------------
					//-- Place Part
					//------------------------------------------------------------------------------------
					//
					if ( !FM_Run ) { // 2016.12.09
						//
						Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
						//
						if ( Sim_Trg > 0 ) {
							Sim_Goal = Clock_Goal_Get_Start_Time();
							timex = Sim_Trg / 20;
							if ( timex <= 0 ) timex = 1;
						}
						else {
							timex = 100;
						}
						//
					}
					else {
						Sim_Trg = 0; // 2017.03.24
						timex = 100;
					}
					//
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%c:%d,%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%c:%d,D%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%c:%d,D%d-%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
						}
						//
					}
					Rb_Run = Rb_RunOrg;

//					if ( Chamber == AL ) { // 2008.01.04
					if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
						if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
							ACSlot = 1 + AC_Off;
						}
						else {
							ACSlot = 1 + AC_Off + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_AL );
						}
						sprintf( ParamF , "PLACE_WAFER%s AL %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
						//
						EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					}
					else {
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							if ( i == 0 ) {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot1;
								}
								else {
									ACSlot = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
							else {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot2;
								}
								else {
									ACSlot = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
						}
						else {
							if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
								ACSlot = 1;
							}
							else {
								ACSlot = 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
							}
						}
						sprintf( ParamF , "PLACE_WAFER%s IC %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
						//
						EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					}
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
							if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
							}
						}
					}
					//==========================================================================================
					if ( i == 0 ) {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , ACSlot , 0 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					else {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 0 , ACSlot , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					//
					aborted = FALSE;
					//
					if ( Rb_Run == 1 ) {
						if ( FM_Run ) {
//							_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
							if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
								Mf = SYS_RUNNING; // 2008.04.03
							}
							else {
								Mf = SYS_ABORTED; // 2008.04.03
							}
						}
						else {
							Mf = SYS_RUNNING; // 2008.04.03
						}
						//
//						Mf = SYS_RUNNING; // 2008.04.03
						while ( TRUE ) {
							//========================
							_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
							//========================
							Mon_Goal = Clock_Goal_Get_Start_Time();
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
								}
								if ( Mf == SYS_ABORTED ) {
									aborted = TRUE;
									break;
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PLACEDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							else {
								//
								//===============================================================
								//
								if ( Sim_Trg > 0 ) { // 2016.12.09
									if ( Mf == SYS_RUNNING ) {
										if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
											Mf = SYS_SUCCESS;
										}
									}
									if ( Rb_Run == 1 ) {
										Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
										if ( update ) _i_SCH_IO_MTL_SAVE();
										if ( Mr == RB_PLACEDONE ) {
											if ( Mf != SYS_RUNNING ) break;
											Rb_Run = 0;
										}
									}
									else {
										if ( Mf != SYS_RUNNING ) break;
									}
								}
								//
								//===============================================================
								//
								else {
									if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) {
										if ( update ) _i_SCH_IO_MTL_SAVE();
										break;
									}
									if ( update ) _i_SCH_IO_MTL_SAVE();
								}
								//
							}
							l_c = 0; // 2008.09.14
							do {
								if ( MANAGER_ABORT() ) {
									aborted = TRUE;
									break;
								}
								//===================================================================
								// 2005.10.26
								//===================================================================
								if ( FM_Run ) {
									if ( Mf == SYS_RUNNING ) {
										if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
											break;
										}
									}
								}
								//===================================================================
								//
								switch( GET_RUN_LD_CONTROL( 0 ) ) {
								case 1 :
									timex = 0;
									break;
								case 2 :
									timex = 20;
									break;
								}
								//
								//===================================================================
								// Sleep(1); // 2008.09.14
								if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
								l_c++; // 2008.09.14
							}
							while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
							//
							if ( aborted ) break;
						}
					}
					else {
						//
						if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							while ( TRUE ) {
								if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
							}
						}
						//
						if ( FM_Run ) {
							if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) aborted = TRUE;
						}
						if ( !aborted ) {
							//
							if ( Rb_Run == 4 ) { // 2015.12.16
								if ( i == 0 ) {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , ACSlot , 0 , -1 , MaintInfUse , TRUE );
								}
								else {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 0 , ACSlot , -1 , MaintInfUse , TRUE );
								}
							}
							//==================================================================================================
							// 2006.11.09
							//==================================================================================================
							//
							if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
								while ( TRUE ) {
									if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
								}
							}
							//
							_i_SCH_IO_MTL_SAVE();
							//==================================================================================================
						}
					}
					//======================================================================================================================================================
					// 2006.09.21
					//======================================================================================================================================================
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						if ( aborted ) {
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
								}
							}
							//==========================================================================================
						}
						else {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%c:%d,%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%c:%d,D%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%c:%d,D%d-%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
								}
							}
							//==========================================================================================
						}
					}
					//======================================================================================================================================================
					if ( aborted ) return SYS_ABORTED;
					//------------------------------------------------------------------------------------
					//-- Run Part
					//------------------------------------------------------------------------------------
					if ( PickPlaceMode != FAL_RUN_MODE_PLACE ) {
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							if ( swch <= 0 ) {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
								}
								//
							}
							else {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , xdm1 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								//
							}
						}
						if ( FM_Run ) {
							//===================================================
							// 2006.02.01 // 2007.01.29
							//===================================================
							if ( swch <= 0 ) {
								swch2 = 0;
								swch3 = 0;
							}
							else {
								//
								if ( i == 0 ) {
									swch2 = ( swch % 10000 ) % 100;
									swch3 = ( swch % 10000 ) / 100;
								}
								else {
									//
									// 2016.10.13
									//
//									swch2 = ( swch / 10000 ) % 100;
//									swch3 = ( swch / 10000 ) / 100;
									//
									if ( OSlot1 > 0 ) {
										swch2 = ( swch / 10000 ) % 100;
										swch3 = ( swch / 10000 ) / 100;
									}
									else {
										if ( ( swch / 10000 ) > 0 ) {
											swch2 = ( swch / 10000 ) % 100;
											swch3 = ( swch / 10000 ) / 100;
										}
										else {
											swch2 = ( swch % 10000 ) % 100;
											swch3 = ( swch % 10000 ) / 100;
										}
									}
								}
								//
							}
							//===================================================
//							if ( Chamber == AL ) { // 2008.01.04
							if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
									if ( AL_swap ) strcat( ParamF , " - OUT" ); // 2010.12.22
								}
							}
							else {
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
								}
								else {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
								}
							}
							//===================================================
							if ( ( swch2 > 0 ) && ( swch3 > 0 ) ) { // 2007.01.29
								if ( ( swch3 >= CM1 ) && ( swch3 < PM1 ) ) {
									sprintf( Buffer , " CM%d" , swch3 - CM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= PM1 ) && ( swch3 < AL ) ) {
									sprintf( Buffer , " PM%d" , swch3 - PM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= BM1 ) && ( swch3 < TM ) ) {
									sprintf( Buffer , " BM%d" , swch3 - BM1 + 1 );	strcat( ParamF , Buffer );
								}
							}
							//===================================================
							EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
							//
							if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) return SYS_ABORTED;
						}
						else {
							if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
						}
						//======================================================================================================================================================
						// 2006.09.21
						//======================================================================================================================================================
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:%d)%cWHFM%sSUCCESS %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d)%cWHFM%sSUCCESS %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d-%d)%cWHFM%sSUCCESS %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
						}
						//======================================================================================================================================================
					}
				}
				else {
//					if ( Chamber == AL ) { // 2008.01.04
					if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
						if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
							ACSlot = 1 + AC_Off;
						}
						else {
							ACSlot = 1 + AC_Off + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_AL );
						}
					}
					else {
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							if ( i == 0 ) {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot1;
								}
								else {
									ACSlot = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
							else {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot2;
								}
								else {
									ACSlot = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
						}
						else {
							if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
								ACSlot = 1;
							}
							else {
								ACSlot = 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
							}
						}
					}
					if ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) {
						//------------------------------------------------------------------------------------
						//-- Run Part
						//------------------------------------------------------------------------------------
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							if ( swch <= 0 ) {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
								}
								//
							}
							else {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , xdm1 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								//
							}
						}
						if ( FM_Run ) {
							//===================================================
							// 2006.02.01
							//===================================================
							if ( swch <= 0 ) {
								swch2 = 0;
								swch3 = 0;
							}
							else {
								if ( i == 0 ) {
									swch2 = ( swch % 10000 ) % 100;
									swch3 = ( swch % 10000 ) / 100;
								}
								else {
									//
									// 2016.10.13
									//
//									swch2 = ( swch / 10000 ) % 100;
//									swch3 = ( swch / 10000 ) / 100;
									//
									if ( OSlot1 > 0 ) {
										swch2 = ( swch / 10000 ) % 100;
										swch3 = ( swch / 10000 ) / 100;
									}
									else {
										if ( ( swch / 10000 ) > 0 ) {
											swch2 = ( swch / 10000 ) % 100;
											swch3 = ( swch / 10000 ) / 100;
										}
										else {
											swch2 = ( swch % 10000 ) % 100;
											swch3 = ( swch % 10000 ) / 100;
										}
									}
								}
								//
							}
							//===================================================
//							if ( Chamber == AL ) { // 2008.01.04
							if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
									if ( AL_swap ) strcat( ParamF , " - OUT" ); // 2010.12.22
								}
							}
							else {
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
								}
								else {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
								}
							}
							//===================================================
							if ( ( swch2 > 0 ) && ( swch3 > 0 ) ) { // 2007.01.29
								if ( ( swch3 >= CM1 ) && ( swch3 < PM1 ) ) {
									sprintf( Buffer , " CM%d" , swch3 - CM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= PM1 ) && ( swch3 < AL ) ) {
									sprintf( Buffer , " PM%d" , swch3 - PM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= BM1 ) && ( swch3 < TM ) ) {
									sprintf( Buffer , " BM%d" , swch3 - BM1 + 1 );	strcat( ParamF , Buffer );
								}
							}
							//===================================================
							EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
							//
							if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) return SYS_ABORTED;
						}
						else {
							if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
						}
						//======================================================================================================================================================
						// 2006.09.21
						//======================================================================================================================================================
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:%d)%cWHFM%sSUCCESS %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d)%cWHFM%sSUCCESS %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d-%d)%cWHFM%sSUCCESS %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
						}
						//======================================================================================================================================================
					}
				}
				//------------------------------------------------------------------------------------
				//-- Pick Part
				//------------------------------------------------------------------------------------
				if ( ( PickPlaceMode != FAL_RUN_MODE_PLACE_MDL ) && ( PickPlaceMode != FAL_RUN_MODE_PLACE ) ) {
					//
					if ( !FM_Run ) { // 2016.12.09
						//
						Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
						//
						if ( Sim_Trg > 0 ) {
							Sim_Goal = Clock_Goal_Get_Start_Time();
							timex = Sim_Trg / 20;
							if ( timex <= 0 ) timex = 1;
						}
						else {
							timex = 100;
						}
						//
					}
					else {
						Sim_Trg = 0; // 2017.03.24
						timex = 100;
					}
					//
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%c:%d,%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%c:%d,D%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%c:%d,D%d-%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
						}
						//==========================================================================================
						// 2007.03.19
						//==========================================================================================
						if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
							if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
							}
						}
						//==========================================================================================
					}
					Rb_Run = Rb_RunOrg;
//					if ( Chamber == AL ) // 2008.01.04
					if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) // 2008.01.04
						sprintf( ParamF , "PICK_WAFER%s AL %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
					else
						sprintf( ParamF , "PICK_WAFER%s IC %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
					//
					EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					//
					if ( i == 0 ) {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , ACSlot , 0 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					else {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 0 , ACSlot , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					//
					aborted = FALSE;
					//
					if ( Rb_Run == 1 ) {
						if ( FM_Run ) {
//							_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
							if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
								Mf = SYS_RUNNING; // 2008.04.03
							}
							else {
								Mf = SYS_ABORTED; // 2008.04.03
							}
						}
						else {
							Mf = SYS_RUNNING; // 2008.04.03
						}
						//
//						Mf = SYS_RUNNING; // 2008.04.03
						while ( TRUE ) {
							//========================
							_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
							//========================
							Mon_Goal = Clock_Goal_Get_Start_Time();
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
								}
								if ( Mf == SYS_ABORTED ) {
									aborted = TRUE;
									break;
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PICKDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							else {
								//
								//===============================================================
								//
								if ( Sim_Trg > 0 ) { // 2016.12.09
									if ( Mf == SYS_RUNNING ) {
										if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
											Mf = SYS_SUCCESS;
										}
									}
									if ( Rb_Run == 1 ) {
										Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
										if ( update ) _i_SCH_IO_MTL_SAVE();
										if ( Mr == RB_PICKDONE ) {
											if ( Mf != SYS_RUNNING ) break;
											Rb_Run = 0;
										}
									}
									else {
										if ( Mf != SYS_RUNNING ) break;
									}
								}
								//
								//===============================================================
								//
								else {
									if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PICKDONE ) {
										if ( update ) _i_SCH_IO_MTL_SAVE();
										break;
									}
									if ( update ) _i_SCH_IO_MTL_SAVE();
								}
								//
							}
							l_c = 0; // 2008.09.14
							do {
								if ( MANAGER_ABORT() ) {
									aborted = TRUE;
									break;
								}
								//===================================================================
								// 2005.10.26
								//===================================================================
								if ( FM_Run ) {
									if ( Mf == SYS_RUNNING ) {
										if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
											break;
										}
									}
								}
								//===================================================================
								//
								switch( GET_RUN_LD_CONTROL( 0 ) ) {
								case 1 :
									timex = 0;
									break;
								case 2 :
									timex = 20;
									break;
								}
								//
								//===================================================================
								// Sleep(1); // 2008.09.14
								if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
								l_c++; // 2008.09.14
							}
							while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
							//
							if ( aborted ) break;
						}
					}
					else {
						//
						if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							while ( TRUE ) {
								if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
							}
						}
						//
						if ( FM_Run ) {
							if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) aborted = TRUE;
						}
						if ( !aborted ) {
							//
							if ( Rb_Run == 4 ) { // 2015.12.16
								if ( i == 0 ) {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , ACSlot , 0 , -1 , MaintInfUse , TRUE );
								}
								else {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 0 , ACSlot , -1 , MaintInfUse , TRUE );
								}
							}
							//==================================================================================================
							// 2006.11.09
							//==================================================================================================
							//
							if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
								while ( TRUE ) {
									if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
								}
							}
							//
							_i_SCH_IO_MTL_SAVE();
							//==================================================================================================
						}
					}
					//======================================================================================================================================================
					// 2006.09.21
					//======================================================================================================================================================
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						if ( aborted ) {
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
								}
							}
							//==========================================================================================
						}
						else {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%c:%d,%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%c:%d,D%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%c:%d,D%d-%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
								}
							}
							//==========================================================================================
						}
					}
					//======================================================================================================================================================
					if ( aborted ) return SYS_ABORTED;
					//======================================================================================================================================================
				}
				//======================================================================================================================================================
				if ( PickPlaceMode != FAL_RUN_MODE_PLACE_MDL_PICK ) {
					if ( PickPlaceMode != FAL_RUN_MODE_PICK ) { // 2010.09.28
						if ( PickPlaceMode != FAL_RUN_MODE_PLACE ) { // 2010.09.28
							break;
						}
					}
				}
				//======================================================================================================================================================
			}
		}
	}
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] FMS=%d,Chamber=%d,RSlot1=%d,RSlot2=%d,SrcCas=%d,swch=%d,PickPlaceMode=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_AND_PICK_WITH_FALIC" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , RSlot1 , RSlot2 , SrcCas , swch , PickPlaceMode );
	}
	//======================================================================================================================================================
	return SYS_SUCCESS;
}
//
//
*/









//
// 2018.06.14
//



int EQUIP_PLACE_AND_PICK_WITH_FALIC_Sub( int fms , int CHECKING_SIDE , int Chamber0 , int RSlot1 , int RSlot2 , int SrcCas , int swch , int PickPlaceMode , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int sideA , int sideB , int PointerA , int PointerB , BOOL MaintInfUse ) {
	int timex = 100;
	char ParamF[256];
	char Buffer[32];
	int i , Mf , Mr , OSlot1 , OSlot2 , Slot1 , Slot2 , ACSlot , ACSlot2 , MrMode , MrRes , swch2 , swch3 , AC_Off , AC1_Off = 0 , AC2_Off = 0;
	BOOL FM_Run , Rb_Run , Rb_RunOrg;
	long Mon_Goal;
	BOOL update;
	int sourcewaf;
	int slw0 , slw1 , swaf1 , slw2 , swaf2;
	int abortedError , sd0 , sp0;
	int l_c; // 2008.09.14
	int xdm1 , xdm2; // 2009.09.23
	int Chamber , AL_swap; // 2010.12.22
	char MsgAppchar[2]; /* 2013.04.26 */
	char MsgSltchar[16]; /* 2013.05.23 */
	//
	//===============================================================
	//
	long Sim_Goal; // 2016.12.09
	int  Sim_Trg; // 2016.12.09
	//
	//===============================================================
	//

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
//	if ( Chamber == AL ) { // 2008.01.04
	if ( ( Chamber0 == F_AL ) || ( Chamber0 == AL ) ) { // 2008.01.04
		//
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) )	return SYS_SUCCESS;
		//
		Chamber = Chamber0; // 2010.12.22
		AL_swap = FALSE; // 2010.12.22
	}
	else {
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) )	return SYS_SUCCESS;
		//
		if ( ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) || ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) ) { // 2010.12.22
			//
			Chamber = AL; // 2010.12.22
			AL_swap = TRUE; // 2010.12.22
			//
		}
		else {
			//
			Chamber = Chamber0; // 2010.12.22
			AL_swap = FALSE; // 2010.12.22
			//
		}
		//
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] FMS=%d,Chamber=%d,RSlot1=%d,RSlot2=%d,SrcCas=%d,swch=%d,PickPlaceMode=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_AND_PICK_WITH_FALIC" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , RSlot1 , RSlot2 , SrcCas , swch , PickPlaceMode );
	}
	//======================================================================================================================================================

	Rb_RunOrg = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms );
//	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( EQUIP_ADDRESS_FM(fms) >= 0 ) ) // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( EQUIP_ADDRESS_FM(fms) < 0 ) || ( GET_RUN_LD_CONTROL(4) > 0 ) ) { // 2014.01.09
		FM_Run = FALSE;
	}
	else {
		FM_Run = TRUE;
	}
	//
//	if ( Chamber == AL ) { // 2008.01.04
	if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
//		OSlot1 = RSlot1; // 2008.08.29
//		OSlot2 = RSlot2; // 2008.08.29
		//
		OSlot1 = RSlot1 % 10000; // 2008.08.29
		OSlot2 = RSlot2 % 10000; // 2008.08.29
		//
		AC1_Off = RSlot1 / 10000; // 2008.08.29
		AC2_Off = RSlot2 / 10000; // 2008.08.29
	}
	else {
		if ( ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) && ( CHECKING_SIDE >= TR_CHECKING_SIDE ) && ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) ) { // 2010.12.22
			ACSlot = EQUIP_FM_IC_HASNOT_A_WAFER( CHECKING_SIDE , &Slot1 , &Slot2 );
			if      ( ACSlot == 0 ) {
				return SYS_ABORTED;
			}
			else if ( ACSlot == 1 ) {
				if ( RSlot1 > 0 ) OSlot1 = Slot1;
				else              OSlot1 = 0;
				if ( RSlot2 > 0 ) OSlot2 = Slot1;
				else              OSlot2 = 0;
			}
			else {
				if ( RSlot1 > 0 ) OSlot1 = Slot1;
				else              OSlot1 = 0;
				if ( RSlot2 > 0 ) OSlot2 = Slot2;
				else              OSlot2 = 0;
			}
		}
		else {
			OSlot1 = RSlot1;
			OSlot2 = RSlot2;
		}
	}
	//
	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */
	//
	//========================================================================================================================================================
	// 2005.11.28
	//========================================================================================================================================================
	if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_ALL ) || ( PickPlaceMode == FAL_RUN_MODE_PICK_ALL ) ) {
//		if ( Chamber == AL ) { // 2008.01.04
		if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
			return SYS_ABORTED; // IC only
		}
		//=========================================
		Slot1 = OSlot1;
		Slot2 = OSlot2;
		//
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
			ACSlot  = Slot1;
			ACSlot2 = Slot2;
		}
		else {
			ACSlot  = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
			ACSlot2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
		}
		//=========================================
		// 2009.09.23
		//=========================================
		if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
//			xdm1 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( sideA , PointerA ) , 1 ); // 2017.09.11
			xdm1 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( sideA , PointerA ) , 1 , 11 , sideA , PointerA ); // 2017.09.11
		}
		else {
			xdm1 = -1;
		}
		if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
//			xdm2 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( sideB , PointerB ) , 1 ); // 2017.09.11
			xdm2 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( sideB , PointerB ) , 1 , 11 , sideB , PointerB ); // 2017.09.11
		}
		else {
			xdm2 = -1;
		}
		//=========================================
		if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
			if ( PickPlaceMode == FAL_RUN_MODE_PICK_ALL ) {
				//
				if ( xdm1 != xdm2 ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(A:%d,%d)%cWHFMPICKSTART IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(A:%d,D%d)%cWHFMPICKSTART IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(A:%d,D%d-%d)%cWHFMPICKSTART IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
					}
					//
					FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
					//
					if      ( xdm2 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(B:%d,%d)%cWHFMPICKSTART IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else if ( xdm2 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(B:%d,D%d)%cWHFMPICKSTART IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(B:%d,D%d-%d)%cWHFMPICKSTART IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
					}
				}
				else {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(%d,%d:%d,%d)%cWHFMPICKSTART IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(%d,D%d:%d,D%d)%cWHFMPICKSTART IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPICKSTART IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
				}
				//
				//==========================================================================================
				// 2007.03.19
				//==========================================================================================
				if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
					if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
					}
					if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
					}
				}
				//==========================================================================================
			}
			else {
				//
				if ( xdm1 != xdm2 ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(A:%d,%d)%cWHFMPLACESTART IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(A:%d,D%d)%cWHFMPLACESTART IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(A:%d,D%d-%d)%cWHFMPLACESTART IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
					}
					//
					FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
					//
					if      ( xdm2 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(B:%d,%d)%cWHFMPLACESTART IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else if ( xdm2 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(B:%d,D%d)%cWHFMPLACESTART IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(B:%d,D%d-%d)%cWHFMPLACESTART IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
					}
				}
				else {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					if      ( xdm1 == -1 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(%d,%d:%d,%d)%cWHFMPLACESTART IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else if ( xdm1 == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(%d,D%d:%d,D%d)%cWHFMPLACESTART IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPLACESTART IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
					}
				}
				//==========================================================================================
				// 2007.03.19
				//==========================================================================================
				if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
					if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
					}
					if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
					}
				}
				//==========================================================================================
			}
		}
		//=========================================
		//
		switch( PickPlaceMode ) {
		case FAL_RUN_MODE_PLACE_ALL :
			sprintf( ParamF , "PLACE_WAFER%s IC %d %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot , ACSlot2 );
			MrMode = RB_MODE_PLACE;
			MrRes = RB_PLACEDONE;
			break;
		case FAL_RUN_MODE_PICK_ALL :
			sprintf( ParamF , "PICK_WAFER%s IC %d %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot , ACSlot2 );
			MrMode = RB_MODE_PICK;
			MrRes = RB_PICKDONE;
			break;
			//
		default :
			break;
			//
		}
		//
		if ( !FM_Run ) { // 2016.12.09
			//
			switch( PickPlaceMode ) {
			case FAL_RUN_MODE_PLACE_ALL :
				Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
				break;
			case FAL_RUN_MODE_PICK_ALL :
				Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
				break;
				//
			default :
				//
				Sim_Trg = 0;
				break;
				//
			}
			//
			if ( Sim_Trg > 0 ) {
				Sim_Goal = Clock_Goal_Get_Start_Time();
				timex = Sim_Trg / 20;
				if ( timex <= 0 ) timex = 1;
			}
			else {
				timex = 100;
			}
			//
		}
		else {
			timex = 100;
		}
		//
		EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
		//
		Rb_Run = Rb_RunOrg;
		//
		if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , MrMode , Chamber , ACSlot , ACSlot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
		//
		abortedError = 0;
		//
		if ( Rb_Run == 1 ) {
			if ( FM_Run ) {
				//_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
				if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
					Mf = SYS_RUNNING; // 2008.04.03
				}
				else {
					Mf = SYS_ABORTED; // 2008.04.03
				}
			}
			else {
				Mf = SYS_RUNNING; // 2008.04.03
			}
			//
//			Mf = SYS_RUNNING; // 2008.04.03
			while ( TRUE ) {
				//========================
				_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
				//========================
				Mon_Goal = Clock_Goal_Get_Start_Time();
				if ( FM_Run ) {
					if ( Mf == SYS_RUNNING ) {
						Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
					}
					if ( Mf == SYS_ABORTED ) {
						abortedError = 1;
						break;
					}
					if ( Mf == SYS_ERROR ) {
						abortedError = 2;
						break;
					}
					if ( Rb_Run == 1 ) {
						Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == MrRes ) {
							if ( Mf != SYS_RUNNING ) break;
							Rb_Run = 0;
						}
					}
					else {
						if ( Mf != SYS_RUNNING ) break;
					}
				}
				else {
					//
					//===============================================================
					//
					if ( Sim_Trg > 0 ) {
						if ( Mf == SYS_RUNNING ) {
							if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
								Mf = SYS_SUCCESS;
							}
						}
						if ( Rb_Run == 1 ) {
							Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
							if ( update ) _i_SCH_IO_MTL_SAVE();
							if ( Mr == MrRes ) {
								if ( Mf != SYS_RUNNING ) break;
								Rb_Run = 0;
							}
						}
						else {
							if ( Mf != SYS_RUNNING ) break;
						}
					}

					//
					//===============================================================
					//
					else {
	//					if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) { // 2007.12.28
						if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == MrRes ) { // 2007.12.28
							if ( update ) _i_SCH_IO_MTL_SAVE();
							break;
						}
						if ( update ) _i_SCH_IO_MTL_SAVE();
					}
				}
				l_c = 0; // 2008.09.14
				do {
					if ( MANAGER_ABORT() ) {
						abortedError = 1;
						break;
					}
					//===================================================================
					// 2005.10.26
					//===================================================================
					if ( FM_Run ) {
						if ( Mf == SYS_RUNNING ) {
							if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
								break;
							}
						}
					}
					//===================================================================
					//
					switch( GET_RUN_LD_CONTROL( 0 ) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
					//===================================================================
					//Sleep(2); // 2005.10.26
					// Sleep(1); // 2008.09.14
					if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
					l_c++; // 2008.09.14
				}
				while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
				//===================================================================
				if ( abortedError != 0 ) break;
				//===================================================================
			}
		}
		else {
			//
			if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( MrRes == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
				}
			}
			//
			if ( FM_Run ) {
				switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
				case SYS_ABORTED :
					abortedError = 1;
					break;
				case SYS_ERROR :
					abortedError = 3;
					break;
				}
			}
			if ( abortedError == 0 ) {
				//
				if ( Rb_Run == 4 ) { // 2015.12.16
					ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , MrMode , Chamber , ACSlot , ACSlot2 , -1 , MaintInfUse , TRUE );
				}
				//==================================================================================================
				// 2006.11.09
				//==================================================================================================
				//
				if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
					while ( TRUE ) {
						if ( MrRes == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
					}
				}
				//
				_i_SCH_IO_MTL_SAVE();
				//==================================================================================================
			}
		}
		//======================================================================================================================================================
		if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
			if ( PickPlaceMode == FAL_RUN_MODE_PICK_ALL ) {
				if ( abortedError == 1 ) {
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
				else if ( ( abortedError == 2 ) || ( abortedError == 3 ) ) {
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_REJECT , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_REJECT , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
				else {
					//
					if ( xdm1 != xdm2 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(A:%d,%d)%cWHFMPICKSUCCESS IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(A:%d,D%d)%cWHFMPICKSUCCESS IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(A:%d,D%d-%d)%cWHFMPICKSUCCESS IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
						}
						//
						FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
						//
						if      ( xdm2 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(B:%d,%d)%cWHFMPICKSUCCESS IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else if ( xdm2 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(B:%d,D%d)%cWHFMPICKSUCCESS IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(B:%d,D%d-%d)%cWHFMPICKSUCCESS IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
						}
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(%d,%d:%d,%d)%cWHFMPICKSUCCESS IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(%d,D%d:%d,D%d)%cWHFMPICKSUCCESS IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPICKSUCCESS IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
					}
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
			}
			else {
				if ( abortedError == 1 ) {
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
				else if ( ( abortedError == 2 ) || ( abortedError == 3 ) ) {
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_REJECT , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_REJECT , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
				else {
					//
					if ( xdm1 != xdm2 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(A:%d,%d)%cWHFMPLACESUCCESS IC:A,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(A:%d,D%d)%cWHFMPLACESUCCESS IC:A,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(A:%d,D%d-%d)%cWHFMPLACESUCCESS IC:A,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , 9 , Slot1 , sourcewaferidA , 9 , xdm1 , sourcewaferidA , MsgSltchar );
						}
						//
						FA_SIDE_TO_SLOT_GET_L( sideB , PointerB , MsgSltchar );
						//
						if      ( xdm2 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(B:%d,%d)%cWHFMPLACESUCCESS IC:B,%d:%d%c%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else if ( xdm2 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(B:%d,D%d)%cWHFMPLACESUCCESS IC:B,%d:%d%cD%d%s\n" , Slot2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , sourcewaferidB , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(B:%d,D%d-%d)%cWHFMPLACESUCCESS IC:B,%d:%d%cD%d-%d%s\n" , Slot2 , xdm2 , sourcewaferidB , 9 , Slot2 , sourcewaferidB , 9 , xdm2 , sourcewaferidB , MsgSltchar );
						}
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(%d,%d:%d,%d)%cWHFMPLACESUCCESS IC:%d:%d,%d:%d%c%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(%d,D%d:%d,D%d)%cWHFMPLACESUCCESS IC:%d:%d,%d:%d%cD%d%s\n" , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to IC(%d,D%d-%d:%d,D%d-%d)%cWHFMPLACESUCCESS IC:%d:%d,%d:%d%cD%d-%d%s\n" , Slot1 , xdm1 , sourcewaferidA , Slot2 , xdm1 , sourcewaferidB , 9 , Slot1 , sourcewaferidA , Slot2 , sourcewaferidB , 9 , xdm1 , sourcewaferidB * 100 + sourcewaferidA , MsgSltchar );
						}
					}
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
						if ( ( sideA >= 0 ) && ( sideA < MAX_CASSETTE_SIDE ) && ( PointerA >= 0 ) && ( PointerA < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideA , PointerA , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , F_IC , fms * 100 , 0 , ACSlot , sourcewaferidA );
						}
						if ( ( sideB >= 0 ) && ( sideB < MAX_CASSETTE_SIDE ) && ( PointerB >= 0 ) && ( PointerB < MAX_CASSETTE_SLOT_SIZE ) ) {
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sideB , PointerB , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , F_IC , fms * 100 , 1 , ACSlot2 , sourcewaferidB );
						}
					}
					//==========================================================================================
				}
			}
		}
		//======================================================================================================================================================
		if ( abortedError == 1 ) return SYS_ABORTED;
		//
		if ( abortedError == 2 ) {
			//
			ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , MrMode , Chamber , ACSlot , ACSlot2 );
			//
			return SYS_ERROR;
		}
		//
		if ( abortedError == 3 ) return SYS_ERROR;
		//======================================================================================================================================================
	}
	//========================================================================================================================================================
	else {
		//=============================================================================================
		// 2007.10.17
		//=============================================================================================
//		if ( ( Chamber == AL ) && ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) && ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) && ( OSlot1 > 0 ) && ( OSlot2 > 0 ) ) { // 2008.09.10
		if ( ( Chamber == AL ) && ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE ) ) && ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) && ( OSlot1 > 0 ) && ( OSlot2 > 0 ) ) { // 2008.09.10
			//
			if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE ) ) { // 2008.09.10
				//------------------------------------------------------------------------------------
				//-- Place Part
				//------------------------------------------------------------------------------------
				//
				if ( !FM_Run ) { // 2016.12.09
					//
					Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
					//
					if ( Sim_Trg > 0 ) {
						Sim_Goal = Clock_Goal_Get_Start_Time();
						timex = Sim_Trg / 20;
						if ( timex <= 0 ) timex = 1;
					}
					else {
						timex = 100;
					}
					//
				}
				else {
					Sim_Trg = 0; // 2017.03.24
					timex = 100;
				}
				//
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%d,%d:%d,%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//
				Rb_Run = Rb_RunOrg;
				//
				sprintf( ParamF , "PLACE_WAFER%s AL %d %d %d %d" , MsgAppchar , OSlot1 , OSlot2 , 1 , 2 );
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 1 , 2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
				//
				abortedError = 0;
				//
				if ( Rb_Run == 1 ) {
					if ( FM_Run ) {
	//					_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
						if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
							Mf = SYS_RUNNING; // 2008.04.03
						}
						else {
							Mf = SYS_ABORTED; // 2008.04.03
						}
					}
					else {
						Mf = SYS_RUNNING; // 2008.04.03
					}
					//
	//				Mf = SYS_RUNNING; // 2008.04.03
					while ( TRUE ) {
						//========================
						_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
						//========================
						Mon_Goal = Clock_Goal_Get_Start_Time();
						if ( FM_Run ) {
							if ( Mf == SYS_RUNNING ) {
								Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
							}
							if ( Mf == SYS_ABORTED ) {
								abortedError = 1;
								break;
							}
							if ( Mf == SYS_ERROR ) {
								abortedError = 2;
								break;
							}
							if ( Rb_Run == 1 ) {
								Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
								if ( update ) _i_SCH_IO_MTL_SAVE();
								if ( Mr == RB_PLACEDONE ) {
									if ( Mf != SYS_RUNNING ) break;
									Rb_Run = 0;
								}
							}
							else {
								if ( Mf != SYS_RUNNING ) break;
							}
						}
						else {
							//
							//===============================================================
							//
							if ( Sim_Trg > 0 ) { // 2016.12.09
								if ( Mf == SYS_RUNNING ) {
									if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
										Mf = SYS_SUCCESS;
									}
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PLACEDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							//
							//===============================================================
							//
							else {
								if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) {
									if ( update ) _i_SCH_IO_MTL_SAVE();
									break;
								}
								if ( update ) _i_SCH_IO_MTL_SAVE();
							}
							//
						}
						l_c = 0; // 2008.09.14
						do {
							if ( MANAGER_ABORT() ) {
								abortedError = 1;
								break;
							}
							//===================================================================
							// 2005.10.26
							//===================================================================
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
										break;
									}
								}
							}
							//===================================================================
							//
							switch( GET_RUN_LD_CONTROL( 0 ) ) {
							case 1 :
								timex = 0;
								break;
							case 2 :
								timex = 20;
								break;
							}
							//
							//===================================================================
							//Sleep(2); // 2005.10.26
							// Sleep(1); // 2008.09.14
							if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
							l_c++; // 2008.09.14
						}
						while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
						//
						if ( abortedError != 0 ) break;
					}
				}
				else {
					//
					if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
						while ( TRUE ) {
							if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
						}
					}
					//
					if ( FM_Run ) {
						switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
						case SYS_ABORTED :
							abortedError = 1;
							break;
						case SYS_ERROR :
							abortedError = 3;
							break;
						}
					}
					//
					if ( abortedError == 0 ) {
						//
						if ( Rb_Run == 4 ) { // 2015.12.16
							ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 1 , 2 , -1 , MaintInfUse , TRUE );
						}
						//==================================================================================================
						// 2006.11.09
						//==================================================================================================
						//
						if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							//
							while ( TRUE ) {
								if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
							}
							//
						}
						_i_SCH_IO_MTL_SAVE();
						//==================================================================================================
					}
				}
				//======================================================================================================================================================
				//======================================================================================================================================================
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					if ( abortedError != 0 ) {
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%d,%d:%d,%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
						//
					}
				}
				//======================================================================================================================================================
				if ( abortedError == 1 ) return SYS_ABORTED;
				//
				if ( abortedError == 2 ) {
					//
					ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PLACE , Chamber , 1 , 2 );
					//
					return SYS_ERROR;
				}
				//
				if ( abortedError == 3 ) return SYS_ERROR;
				//
			}
			//
			if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) ) { // 2008.09.10
				//------------------------------------------------------------------------------------
				//-- Run Part
				//------------------------------------------------------------------------------------
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%d:%d)%cWHFM%sRUN %d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//
				if ( FM_Run ) {
					//===================================================
					sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d %d" , MsgAppchar , OSlot1 , OSlot2 , SrcCas - CM1 + 1 , 1 , 2 );
					if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
					//===================================================
					EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					//
					if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) return SYS_ABORTED;
				}
				else {
					if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
				}
				//======================================================================================================================================================
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%d:%d)%cWHFM%sSUCCESS %d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , OSlot2 , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//======================================================================================================================================================
			}
			if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) ) { // 2008.09.10
				//
				//------------------------------------------------------------------------------------
				//-- Pick Part
				//------------------------------------------------------------------------------------
				//
				if ( !FM_Run ) { // 2016.12.09
					//
					Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
					//
					if ( Sim_Trg > 0 ) {
						Sim_Goal = Clock_Goal_Get_Start_Time();
						timex = Sim_Trg / 20;
						if ( timex <= 0 ) timex = 1;
					}
					else {
						timex = 100;
					}
					//
				}
				else {
					Sim_Trg = 0; // 2017.03.24
					timex = 100;
				}
				//
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					//
					FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
					//
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%d,%d:%d,%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
					//
				}
				//
				Rb_Run = Rb_RunOrg;
				//
				sprintf( ParamF , "PICK_WAFER%s AL %d %d %d %d" , MsgAppchar , OSlot1 , OSlot2 , 1 , 2 );
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 1 , 2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
				//
				abortedError = 0;
				//
				if ( Rb_Run == 1 ) {
					if ( FM_Run ) {
	//					_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
						if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
							Mf = SYS_RUNNING; // 2008.04.03
						}
						else {
							Mf = SYS_ABORTED; // 2008.04.03
						}
					}
					else {
						Mf = SYS_RUNNING; // 2008.04.03
					}
					//
	//				Mf = SYS_RUNNING; // 2008.04.03
					while ( TRUE ) {
						//========================
						_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
						//========================
						Mon_Goal = Clock_Goal_Get_Start_Time();
						if ( FM_Run ) {
							if ( Mf == SYS_RUNNING ) {
								Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
							}
							if ( Mf == SYS_ABORTED ) {
								abortedError = 1;
								break;
							}
							if ( Mf == SYS_ERROR ) {
								abortedError = 2;
								break;
							}
							if ( Rb_Run == 1 ) {
								Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
								if ( update ) _i_SCH_IO_MTL_SAVE();
								if ( Mr == RB_PICKDONE ) {
									if ( Mf != SYS_RUNNING ) break;
									Rb_Run = 0;
								}
							}
							else {
								if ( Mf != SYS_RUNNING ) break;
							}
						}
						else {
							//
							//===============================================================
							//
							if ( Sim_Trg > 0 ) { // 2016.12.09
								if ( Mf == SYS_RUNNING ) {
									if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
										Mf = SYS_SUCCESS;
									}
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PICKDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							//
							//===============================================================
							//
							else {
								if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PICKDONE ) {
									if ( update ) _i_SCH_IO_MTL_SAVE();
									break;
								}
								if ( update ) _i_SCH_IO_MTL_SAVE();
							}
						}
						l_c = 0; // 2008.09.14
						do {
							if ( MANAGER_ABORT() ) {
								abortedError = 1;
								break;
							}
							//===================================================================
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
										break;
									}
								}
							}
							//===================================================================
							//
							switch( GET_RUN_LD_CONTROL( 0 ) ) {
							case 1 :
								timex = 0;
								break;
							case 2 :
								timex = 20;
								break;
							}
							//
							//
							//===================================================================
							//Sleep(2); // 2005.10.26
							// Sleep(1); // 2008.09.14
							if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
							l_c++; // 2008.09.14
						}
						while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
						//
						if ( abortedError != 0 ) break;
					}
				}
				else {
					//
					if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
						while ( TRUE ) {
							if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
						}
					}
					//
					if ( FM_Run ) {
						switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
						case SYS_ABORTED :
							abortedError = 1;
							break;
						case SYS_ERROR :
							abortedError = 3;
							break;
						}
						//
					}
					if ( abortedError == 0 ) {
						//
						if ( Rb_Run == 4 ) { // 2015.12.16
							ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 1 , 2 , -1 , MaintInfUse , TRUE );
						}
						//==================================================================================================
						// 2006.11.09
						//==================================================================================================
						//
						if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							while ( TRUE ) {
								if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
							}
						}
						//
						_i_SCH_IO_MTL_SAVE();
						//==================================================================================================
					}
				}
				//======================================================================================================================================================
				if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
					if ( abortedError != 0 ) {
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( sideA , PointerA , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%d,%d:%d,%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , OSlot1 , sourcewaferidA , OSlot2 , sourcewaferidB , 9 , sourcewaferidA + ( sourcewaferidB * 100 ) , MsgSltchar );
						//
					}
				}
				//======================================================================================================================================================
				if ( abortedError == 1 ) return SYS_ABORTED;
				if ( abortedError == 2 ) {
					//
					ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PICK , Chamber , 1 , 2 );
					//
					return SYS_ERROR;
				}
				if ( abortedError == 3 ) return SYS_ERROR;
				//======================================================================================================================================================
				//
			}
		}
		//======================================================================================================================================================
		else {
			for ( i = 0 ; i < 2 ; i++ ) {
				if ( i == 0 ) {
					if ( OSlot1 > 0 ) {
						Slot1 = OSlot1;
						Slot2 = 0;
						//
						sourcewaf = sourcewaferidA;
						//
						slw0 = Slot1;
						slw1 = Slot1;
						swaf1 = sourcewaf;
						slw2 = 0;
						swaf2 = 0;
						//
						sd0 = sideA;
						sp0 = PointerA;
						//
						AC_Off = AC1_Off; // 2008.08.29
					}
					else {
						continue;
					}
				}
				else {
					if ( OSlot2 > 0 ) {
						Slot1 = 0;
						Slot2 = OSlot2;
						//
						sourcewaf = sourcewaferidB;
						//
						slw0 = Slot2;
						slw1 = 0;
						swaf1 = 0;
						slw2 = Slot2;
						swaf2 = sourcewaf;
						//
						sd0 = sideB;
						sp0 = PointerB;
						//
						AC_Off = AC2_Off; // 2008.08.29
					}
					else {
						continue;
					}
				}
				//
				//=========================================
				// 2009.09.23
				//=========================================
				if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
//					xdm1 = SCH_Inside_ThisIs_BM_OtherChamberD( _i_SCH_CLUSTER_Get_PathIn( sd0 , sp0 ) , 1 ); // 2017.09.11
					xdm1 = SCH_Inside_ThisIs_DummyMethod( _i_SCH_CLUSTER_Get_PathIn( sd0 , sp0 ) , 1 , 11 , sd0 , sp0 ); // 2017.09.11
				}
				else {
					xdm1 = -1;
				}
				//=========================================
				//
				if ( ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL_PICK ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE_MDL ) || ( PickPlaceMode == FAL_RUN_MODE_PLACE ) ) {
					//
					//------------------------------------------------------------------------------------
					//-- Place Part
					//------------------------------------------------------------------------------------
					//
					if ( !FM_Run ) { // 2016.12.09
						//
						Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
						//
						if ( Sim_Trg > 0 ) {
							Sim_Goal = Clock_Goal_Get_Start_Time();
							timex = Sim_Trg / 20;
							if ( timex <= 0 ) timex = 1;
						}
						else {
							timex = 100;
						}
						//
					}
					else {
						Sim_Trg = 0; // 2017.03.24
						timex = 100;
					}
					//
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%c:%d,%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%c:%d,D%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Start to %s(%c:%d,D%d-%d)%cWHFMPLACESTART %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
						}
						//
					}
					Rb_Run = Rb_RunOrg;

//					if ( Chamber == AL ) { // 2008.01.04
					if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
						if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
							ACSlot = 1 + AC_Off;
						}
						else {
							ACSlot = 1 + AC_Off + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_AL );
						}
						sprintf( ParamF , "PLACE_WAFER%s AL %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
						//
						EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					}
					else {
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							if ( i == 0 ) {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot1;
								}
								else {
									ACSlot = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
							else {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot2;
								}
								else {
									ACSlot = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
						}
						else {
							if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
								ACSlot = 1;
							}
							else {
								ACSlot = 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
							}
						}
						sprintf( ParamF , "PLACE_WAFER%s IC %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
						//
						EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					}
					//==========================================================================================
					// 2007.03.19
					//==========================================================================================
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
							if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_RUNNING , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
							}
						}
					}
					//==========================================================================================
					if ( i == 0 ) {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , ACSlot , 0 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					else {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 0 , ACSlot , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					//
					abortedError = 0;
					//
					if ( Rb_Run == 1 ) {
						if ( FM_Run ) {
//							_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
							if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
								Mf = SYS_RUNNING; // 2008.04.03
							}
							else {
								Mf = SYS_ABORTED; // 2008.04.03
							}
						}
						else {
							Mf = SYS_RUNNING; // 2008.04.03
						}
						//
//						Mf = SYS_RUNNING; // 2008.04.03
						while ( TRUE ) {
							//========================
							_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
							//========================
							Mon_Goal = Clock_Goal_Get_Start_Time();
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
								}
								if ( Mf == SYS_ABORTED ) {
									abortedError = 1;
									break;
								}
								if ( Mf == SYS_ERROR ) {
									abortedError = 2;
									break;
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PLACEDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							else {
								//
								//===============================================================
								//
								if ( Sim_Trg > 0 ) { // 2016.12.09
									if ( Mf == SYS_RUNNING ) {
										if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
											Mf = SYS_SUCCESS;
										}
									}
									if ( Rb_Run == 1 ) {
										Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
										if ( update ) _i_SCH_IO_MTL_SAVE();
										if ( Mr == RB_PLACEDONE ) {
											if ( Mf != SYS_RUNNING ) break;
											Rb_Run = 0;
										}
									}
									else {
										if ( Mf != SYS_RUNNING ) break;
									}
								}
								//
								//===============================================================
								//
								else {
									if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) {
										if ( update ) _i_SCH_IO_MTL_SAVE();
										break;
									}
									if ( update ) _i_SCH_IO_MTL_SAVE();
								}
								//
							}
							l_c = 0; // 2008.09.14
							do {
								if ( MANAGER_ABORT() ) {
									abortedError = 1;
									break;
								}
								//===================================================================
								// 2005.10.26
								//===================================================================
								if ( FM_Run ) {
									if ( Mf == SYS_RUNNING ) {
										if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
											break;
										}
									}
								}
								//===================================================================
								//
								switch( GET_RUN_LD_CONTROL( 0 ) ) {
								case 1 :
									timex = 0;
									break;
								case 2 :
									timex = 20;
									break;
								}
								//
								//===================================================================
								// Sleep(1); // 2008.09.14
								if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
								l_c++; // 2008.09.14
							}
							while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
							//
							if ( abortedError != 0 ) break;
						}
					}
					else {
						//
						if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							while ( TRUE ) {
								if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
							}
						}
						//
						if ( FM_Run ) {
							switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
							case SYS_ABORTED :
								abortedError = 1;
								break;
							case SYS_ERROR :
								abortedError = 3;
								break;
							}
						}
						if ( abortedError == 0 ) {
							//
							if ( Rb_Run == 4 ) { // 2015.12.16
								if ( i == 0 ) {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , ACSlot , 0 , -1 , MaintInfUse , TRUE );
								}
								else {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , 0 , ACSlot , -1 , MaintInfUse , TRUE );
								}
							}
							//==================================================================================================
							// 2006.11.09
							//==================================================================================================
							//
							if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
								while ( TRUE ) {
									if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
								}
							}
							//
							_i_SCH_IO_MTL_SAVE();
							//==================================================================================================
						}
					}
					//======================================================================================================================================================
					// 2006.09.21
					//======================================================================================================================================================
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						if ( abortedError == 1 ) {
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_FAIL , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
								}
							}
							//==========================================================================================
						}
						else if ( ( abortedError == 2 ) || ( abortedError == 3 ) ) {
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_REJECT , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
								}
							}
							//==========================================================================================
						}
						else {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%c:%d,%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%c:%d,D%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Place Success to %s(%c:%d,D%d-%d)%cWHFMPLACESUCCESS %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PLACE , FA_SUBST_SUCCESS , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , ACSlot , sourcewaf );
								}
							}
							//==========================================================================================
						}
					}
					//======================================================================================================================================================
					if ( abortedError == 1 ) return SYS_ABORTED;
					//
					if ( abortedError == 2 ) {
						//
						if ( i == 0 ) {
							ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PLACE , Chamber , ACSlot , 0 );
						}
						else {
							ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PLACE , Chamber , 0 , ACSlot );
						}
						//
						return SYS_ERROR;
					}
					//
					if ( abortedError == 3 ) return SYS_ERROR;
					//------------------------------------------------------------------------------------
					//-- Run Part
					//------------------------------------------------------------------------------------
					if ( PickPlaceMode != FAL_RUN_MODE_PLACE ) {
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							if ( swch <= 0 ) {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
								}
								//
							}
							else {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , xdm1 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								//
							}
						}
						if ( FM_Run ) {
							//===================================================
							// 2006.02.01 // 2007.01.29
							//===================================================
							if ( swch <= 0 ) {
								swch2 = 0;
								swch3 = 0;
							}
							else {
								//
								if ( i == 0 ) {
									swch2 = ( swch % 10000 ) % 100;
									swch3 = ( swch % 10000 ) / 100;
								}
								else {
									//
									// 2016.10.13
									//
//									swch2 = ( swch / 10000 ) % 100;
//									swch3 = ( swch / 10000 ) / 100;
									//
									if ( OSlot1 > 0 ) {
										swch2 = ( swch / 10000 ) % 100;
										swch3 = ( swch / 10000 ) / 100;
									}
									else {
										if ( ( swch / 10000 ) > 0 ) {
											swch2 = ( swch / 10000 ) % 100;
											swch3 = ( swch / 10000 ) / 100;
										}
										else {
											swch2 = ( swch % 10000 ) % 100;
											swch3 = ( swch % 10000 ) / 100;
										}
									}
								}
								//
							}
							//===================================================
//							if ( Chamber == AL ) { // 2008.01.04
							if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
									if ( AL_swap ) strcat( ParamF , " - OUT" ); // 2010.12.22
								}
							}
							else {
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
								}
								else {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
								}
							}
							//===================================================
							if ( ( swch2 > 0 ) && ( swch3 > 0 ) ) { // 2007.01.29
								if ( ( swch3 >= CM1 ) && ( swch3 < PM1 ) ) {
									sprintf( Buffer , " CM%d" , swch3 - CM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= PM1 ) && ( swch3 < AL ) ) {
									sprintf( Buffer , " PM%d" , swch3 - PM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= BM1 ) && ( swch3 < TM ) ) {
									sprintf( Buffer , " BM%d" , swch3 - BM1 + 1 );	strcat( ParamF , Buffer );
								}
							}
							//===================================================
							EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
							//
							if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) return SYS_ABORTED;
						}
						else {
							if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
						}
						//======================================================================================================================================================
						// 2006.09.21
						//======================================================================================================================================================
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:%d)%cWHFM%sSUCCESS %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d)%cWHFM%sSUCCESS %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d-%d)%cWHFM%sSUCCESS %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
						}
						//======================================================================================================================================================
					}
				}
				else {
//					if ( Chamber == AL ) { // 2008.01.04
					if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
						if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
							ACSlot = 1 + AC_Off;
						}
						else {
							ACSlot = 1 + AC_Off + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_AL );
						}
					}
					else {
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							if ( i == 0 ) {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot1;
								}
								else {
									ACSlot = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
							else {
								if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
									ACSlot = Slot2;
								}
								else {
									ACSlot = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
								}
							}
						}
						else {
							if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
								ACSlot = 1;
							}
							else {
								ACSlot = 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
							}
						}
					}
					if ( PickPlaceMode == FAL_RUN_MODE_MDL_PICK ) {
						//------------------------------------------------------------------------------------
						//-- Run Part
						//------------------------------------------------------------------------------------
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							if ( swch <= 0 ) {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
								}
								//
							}
							else {
								//
								FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
								//
								if      ( xdm1 == -1 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:%d)%cWHFM%sRUN %c:%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else if ( xdm1 == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d)%cWHFM%sRUN %c:%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at %s(%c:D%d-%d)%cWHFM%sRUN %c:%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , swch , 9 , xdm1 , sourcewaf , MsgSltchar ); // 2006.12.22
								}
								//
							}
						}
						if ( FM_Run ) {
							//===================================================
							// 2006.02.01
							//===================================================
							if ( swch <= 0 ) {
								swch2 = 0;
								swch3 = 0;
							}
							else {
								if ( i == 0 ) {
									swch2 = ( swch % 10000 ) % 100;
									swch3 = ( swch % 10000 ) / 100;
								}
								else {
									//
									// 2016.10.13
									//
//									swch2 = ( swch / 10000 ) % 100;
//									swch3 = ( swch / 10000 ) / 100;
									//
									if ( OSlot1 > 0 ) {
										swch2 = ( swch / 10000 ) % 100;
										swch3 = ( swch / 10000 ) / 100;
									}
									else {
										if ( ( swch / 10000 ) > 0 ) {
											swch2 = ( swch / 10000 ) % 100;
											swch3 = ( swch / 10000 ) / 100;
										}
										else {
											swch2 = ( swch % 10000 ) % 100;
											swch3 = ( swch % 10000 ) / 100;
										}
									}
								}
								//
							}
							//===================================================
//							if ( Chamber == AL ) { // 2008.01.04
							if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) { // 2008.01.04
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
									if ( AL_swap ) strcat( ParamF , " OUT" ); // 2010.12.22
								}
								else {
									sprintf( ParamF , "ALIGN_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
									if ( AL_swap ) strcat( ParamF , " - OUT" ); // 2010.12.22
								}
							}
							else {
								if ( ( swch2 >= CM1 ) && ( swch2 < PM1 ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d CM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - CM1 + 1 );
								}
								else if ( ( swch2 >= PM1 ) && ( swch2 < AL ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d PM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - PM1 + 1 );
								}
								else if ( ( swch2 >= BM1 ) && ( swch2 < TM ) ) {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d BM%d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch2 - BM1 + 1 );
								}
								else {
									sprintf( ParamF , "COOL_WAFER%s %d %d CM%d %d" , MsgAppchar , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
								}
							}
							//===================================================
							if ( ( swch2 > 0 ) && ( swch3 > 0 ) ) { // 2007.01.29
								if ( ( swch3 >= CM1 ) && ( swch3 < PM1 ) ) {
									sprintf( Buffer , " CM%d" , swch3 - CM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= PM1 ) && ( swch3 < AL ) ) {
									sprintf( Buffer , " PM%d" , swch3 - PM1 + 1 );	strcat( ParamF , Buffer );
								}
								else if ( ( swch3 >= BM1 ) && ( swch3 < TM ) ) {
									sprintf( Buffer , " BM%d" , swch3 - BM1 + 1 );	strcat( ParamF , Buffer );
								}
							}
							//===================================================
							EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
							//
							if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) return SYS_ABORTED;
						}
						else {
							if ( !WAIT_SECONDS( 0.5 ) ) return SYS_ABORTED;
						}
						//======================================================================================================================================================
						// 2006.09.21
						//======================================================================================================================================================
						if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:%d)%cWHFM%sSUCCESS %c:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d)%cWHFM%sSUCCESS %c:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Success at %s(%c:D%d-%d)%cWHFM%sSUCCESS %c:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , sourcewaf , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
						}
						//======================================================================================================================================================
					}
				}
				//------------------------------------------------------------------------------------
				//-- Pick Part
				//------------------------------------------------------------------------------------
				if ( ( PickPlaceMode != FAL_RUN_MODE_PLACE_MDL ) && ( PickPlaceMode != FAL_RUN_MODE_PLACE ) ) {
					//
					if ( !FM_Run ) { // 2016.12.09
						//
						Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
						//
						if ( Sim_Trg > 0 ) {
							Sim_Goal = Clock_Goal_Get_Start_Time();
							timex = Sim_Trg / 20;
							if ( timex <= 0 ) timex = 1;
						}
						else {
							timex = 100;
						}
						//
					}
					else {
						Sim_Trg = 0; // 2017.03.24
						timex = 100;
					}
					//
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						//
						FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
						//
						if      ( xdm1 == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%c:%d,%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else if ( xdm1 == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%c:%d,D%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Start from %s(%c:%d,D%d-%d)%cWHFMPICKSTART %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
						}
						//==========================================================================================
						// 2007.03.19
						//==========================================================================================
						if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
							if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_RUNNING , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
							}
						}
						//==========================================================================================
					}
					Rb_Run = Rb_RunOrg;
//					if ( Chamber == AL ) // 2008.01.04
					if ( ( Chamber == F_AL ) || ( Chamber == AL ) ) // 2008.01.04
						sprintf( ParamF , "PICK_WAFER%s AL %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
					else
						sprintf( ParamF , "PICK_WAFER%s IC %d %d %d" , MsgAppchar , Slot1 , Slot2 , ACSlot );
					//
					EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
					//
					if ( i == 0 ) {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , ACSlot , 0 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					else {
						if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 0 , ACSlot , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) return SYS_ABORTED;
					}
					//
					abortedError = 0;
					//
					if ( Rb_Run == 1 ) {
						if ( FM_Run ) {
//							_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
							if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
								Mf = SYS_RUNNING; // 2008.04.03
							}
							else {
								Mf = SYS_ABORTED; // 2008.04.03
							}
						}
						else {
							Mf = SYS_RUNNING; // 2008.04.03
						}
						//
//						Mf = SYS_RUNNING; // 2008.04.03
						while ( TRUE ) {
							//========================
							_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
							//========================
							Mon_Goal = Clock_Goal_Get_Start_Time();
							if ( FM_Run ) {
								if ( Mf == SYS_RUNNING ) {
									Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
								}
								if ( Mf == SYS_ABORTED ) {
									abortedError = 1;
									break;
								}
								if ( Mf == SYS_ERROR ) {
									abortedError = 2;
									break;
								}
								if ( Rb_Run == 1 ) {
									Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
									if ( update ) _i_SCH_IO_MTL_SAVE();
									if ( Mr == RB_PICKDONE ) {
										if ( Mf != SYS_RUNNING ) break;
										Rb_Run = 0;
									}
								}
								else {
									if ( Mf != SYS_RUNNING ) break;
								}
							}
							else {
								//
								//===============================================================
								//
								if ( Sim_Trg > 0 ) { // 2016.12.09
									if ( Mf == SYS_RUNNING ) {
										if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
											Mf = SYS_SUCCESS;
										}
									}
									if ( Rb_Run == 1 ) {
										Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
										if ( update ) _i_SCH_IO_MTL_SAVE();
										if ( Mr == RB_PICKDONE ) {
											if ( Mf != SYS_RUNNING ) break;
											Rb_Run = 0;
										}
									}
									else {
										if ( Mf != SYS_RUNNING ) break;
									}
								}
								//
								//===============================================================
								//
								else {
									if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PICKDONE ) {
										if ( update ) _i_SCH_IO_MTL_SAVE();
										break;
									}
									if ( update ) _i_SCH_IO_MTL_SAVE();
								}
								//
							}
							l_c = 0; // 2008.09.14
							do {
								if ( MANAGER_ABORT() ) {
									abortedError = 1;
									break;
								}
								//===================================================================
								// 2005.10.26
								//===================================================================
								if ( FM_Run ) {
									if ( Mf == SYS_RUNNING ) {
										if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
											break;
										}
									}
								}
								//===================================================================
								//
								switch( GET_RUN_LD_CONTROL( 0 ) ) {
								case 1 :
									timex = 0;
									break;
								case 2 :
									timex = 20;
									break;
								}
								//
								//===================================================================
								// Sleep(1); // 2008.09.14
								if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
								l_c++; // 2008.09.14
							}
							while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
							//
							if ( abortedError != 0 ) break;
						}
					}
					else {
						//
						if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
							while ( TRUE ) {
								if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
							}
						}
						//
						if ( FM_Run ) {
							switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
							case SYS_ABORTED :
								abortedError = 1;
								break;
							case SYS_ERROR :
								abortedError = 3;
								break;
							}
						}
						if ( abortedError == 0 ) {
							//
							if ( Rb_Run == 4 ) { // 2015.12.16
								if ( i == 0 ) {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , ACSlot , 0 , -1 , MaintInfUse , TRUE );
								}
								else {
									ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , 0 , ACSlot , -1 , MaintInfUse , TRUE );
								}
							}
							//==================================================================================================
							// 2006.11.09
							//==================================================================================================
							//
							if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
								while ( TRUE ) {
									if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
								}
							}
							//
							_i_SCH_IO_MTL_SAVE();
							//==================================================================================================
						}
					}
					//======================================================================================================================================================
					// 2006.09.21
					//======================================================================================================================================================
					if ( ( CHECKING_SIDE < TR_CHECKING_SIDE ) && insidelog ) {
						if ( abortedError == 1 ) {
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
								}
							}
							//==========================================================================================
						}
						else if ( ( abortedError == 2 ) || ( abortedError == 3 ) ) {
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_FAIL , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
								}
							}
							//==========================================================================================
						}
						else {
							//
							FA_SIDE_TO_SLOT_GET_L( sd0 , sp0 , MsgSltchar );
							//
							if      ( xdm1 == -1 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%c:%d,%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else if ( xdm1 == 0 ) {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%c:%d,D%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , sourcewaf , MsgSltchar );
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Pick Success from %s(%c:%d,D%d-%d)%cWHFMPICKSUCCESS %s:%d:%d,%d:%d%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , i + 'A' , slw0 , xdm1 , sourcewaf , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , slw1 , swaf1 , slw2 , swaf2 , 9 , xdm1 , sourcewaf , MsgSltchar );
							}
							//
							//==========================================================================================
							// 2007.03.19
							//==========================================================================================
							if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
								if ( ( sd0 >= 0 ) && ( sd0 < MAX_CASSETTE_SIDE ) && ( sp0 >= 0 ) && ( sp0 < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( sd0 , sp0 , FA_SUBSTRATE_PICK , FA_SUBST_SUCCESS , ( Chamber == AL ) || ( Chamber == F_AL ) ? F_AL : F_IC , fms * 100 , i , slw0 , sourcewaf );
								}
							}
							//==========================================================================================
						}
					}
					//======================================================================================================================================================
					if ( abortedError == 1 ) return SYS_ABORTED;
					//
					if ( abortedError == 2 ) {
						//
						if ( i == 0 ) {
							ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PICK , Chamber , ACSlot , 0 );
						}
						else {
							ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PICK , Chamber , 0 , ACSlot );
						}
						//
						return SYS_ERROR;
					}
					//
					if ( abortedError == 3 ) return SYS_ERROR;
					//======================================================================================================================================================
				}
				//======================================================================================================================================================
				if ( PickPlaceMode != FAL_RUN_MODE_PLACE_MDL_PICK ) {
					if ( PickPlaceMode != FAL_RUN_MODE_PICK ) { // 2010.09.28
						if ( PickPlaceMode != FAL_RUN_MODE_PLACE ) { // 2010.09.28
							break;
						}
					}
				}
				//======================================================================================================================================================
			}
		}
	}
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] FMS=%d,Chamber=%d,RSlot1=%d,RSlot2=%d,SrcCas=%d,swch=%d,PickPlaceMode=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_AND_PICK_WITH_FALIC" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , RSlot1 , RSlot2 , SrcCas , swch , PickPlaceMode );
	}
	//======================================================================================================================================================
	return SYS_SUCCESS;
}
//




int EQUIP_PLACE_AND_PICK_WITH_FALIC( int fms , int CHECKING_SIDE , int Chamber0 , int RSlot1 , int RSlot2 , int SrcCas , int swch , int PickPlaceMode , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int sideA , int sideB , int PointerA , int PointerB , BOOL MaintInfUse ) {
	int Res;
	//
//	_EQUIP_RUNNING2_TAG[FM] = TRUE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , TRUE ); // 2012.10.31
	if ( ( Chamber0 == F_AL ) || ( Chamber0 == AL ) ) {
//		_EQUIP_RUNNING2_TAG[F_AL] = TRUE; // 2012.03.23 // 2012.10.31
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( F_AL , TRUE ); // 2012.10.31
	}
	else {
//		_EQUIP_RUNNING2_TAG[F_IC] = TRUE; // 2012.03.23 // 2012.10.31
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( F_IC , TRUE ); // 2012.10.31
	}
	//
	Res = EQUIP_PLACE_AND_PICK_WITH_FALIC_Sub( fms , CHECKING_SIDE , Chamber0 , RSlot1 , RSlot2 , SrcCas , swch , PickPlaceMode , insidelog , sourcewaferidA , sourcewaferidB , sideA , sideB , PointerA , PointerB , MaintInfUse );
	//
//	_EQUIP_RUNNING2_TAG[FM] = FALSE; // 2012.03.23 // 2012.10.31
	if ( ( Chamber0 == F_AL ) || ( Chamber0 == AL ) ) {
//		_EQUIP_RUNNING2_TAG[F_AL] = FALSE; // 2012.03.23 // 2012.10.31
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( F_AL , FALSE ); // 2012.10.31
	}
	else {
//		_EQUIP_RUNNING2_TAG[F_IC] = FALSE; // 2012.03.23 // 2012.10.31
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( F_IC , FALSE ); // 2012.10.31
	}
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
	//
	return Res;
}
//	
int EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( int Chamber , int RSlot1 , int RSlot2 , int SrcCas , int swch , int PickPlaceMode ) {
	return EQUIP_PLACE_AND_PICK_WITH_FALIC( 0 , TR_CHECKING_SIDE , Chamber , RSlot1 , RSlot2 , SrcCas , swch , PickPlaceMode , FALSE , 0 , 0 , -1 , -1 , -1 , -1 , FALSE );
}
//

DWORD _in_EQUIP_ALIGNING_CHECK_TIMER;
int   _in_EQUIP_ALIGNING_CHECK_LAST;
// 
//int EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( int CHECKING_SIDE , int Slot1 , int Slot2 , int SrcCas ) { // 2006.02.08
//int EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( int CHECKING_SIDE , int RSlot1 , int RSlot2 , int SrcCas , int swch , int outmode ) { // 2006.02.08 // 2008.09.01 // 2010.12.22 // 2017.09.11
int EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( int CHECKING_SIDE , int RSlot1 , int RSlot2 , int SrcCas , int swch , int outmode , int s1 , int p1 , int s2 , int p2 ) { // 2006.02.08 // 2008.09.01 // 2010.12.22 // 2017.09.11
	char ParamF[256];
	int ACSlot;
	int ACOffset;
	int Slot1;
	int Slot2;
	int xdm;
	char MsgSltchar[16]; /* 2013.05.23 */
	//
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) ) return SYS_SUCCESS;
	//
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\tRSlot1=%d,RSlot2=%d,SrcCas=%d,swch=%d\n" , CHECKING_SIDE + 1 , "EQUIP_ALIGNING_SPAWN_FOR_FM_MAL" , RSlot1 , RSlot2 , SrcCas , swch );
	}
	//
	if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() != 3 ) return SYS_ABORTED; // 2005.12.01
	//
	// 2008.09.01
	//
	Slot1 = RSlot1 % 10000;
	Slot2 = RSlot2 % 10000;
	//
	ACOffset = 0;
	if ( ( RSlot1 / 10000 ) > 0 ) ACOffset = ( RSlot1 / 10000 );
	if ( ( RSlot2 / 10000 ) > 0 ) ACOffset = ( RSlot2 / 10000 );
	//
	if ( CHECKING_SIDE < TR_CHECKING_SIDE ) {
		//=========================================
		// 2009.09.23
		//=========================================
//		xdm = SCH_Inside_ThisIs_BM_OtherChamberD( SrcCas , 1 ); // 2017.09.11
		//=========================================
		//
		FA_SIDE_TO_SLOT_GET_L( CHECKING_SIDE , -1 , MsgSltchar );
		//
		if ( swch <= 0 ) {
			if ( Slot1 > 0 ) {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 12 , s1 , p1 ); // 2017.09.11
				//
				if      ( xdm == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(A:%d)%cWHFMALRUN A:%d%c%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
				}
				else if ( xdm == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(A:D%d)%cWHFMALRUN A:%d%cD%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(A:D%d-%d)%cWHFMALRUN A:%d%cD%d-%d%s\n" , xdm , Slot1 , 9 , Slot1 , 9 , xdm , Slot1 , MsgSltchar );
				}
				//
			}
			else {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 12 , s2 , p2 ); // 2017.09.11
				//
				if      ( xdm == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(B:%d)%cWHFMALRUN B:%d%c%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
				}
				else if ( xdm == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(B:D%d)%cWHFMALRUN B:%d%cD%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(B:D%d-%d)%cWHFMALRUN B:%d%cD%d-%d%s\n" , xdm , Slot2 , 9 , Slot2 , 9 , xdm , Slot2 , MsgSltchar );
				}
				//
			}
		}
		else {
			if ( Slot1 > 0 ) {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 12 , s1 , p1 ); // 2017.09.11
				//
				if      ( xdm == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(A:%d)%cWHFMALRUN A:%d:%d%c%d%s\n" , Slot1 , 9 , Slot1 , swch , 9 , Slot1 , MsgSltchar ); // 2006.12.22
				}
				else if ( xdm == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(A:D%d)%cWHFMALRUN A:%d:%d%cD%d%s\n" , Slot1 , 9 , Slot1 , swch , 9 , Slot1 , MsgSltchar ); // 2006.12.22
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(A:D%d-%d)%cWHFMALRUN A:%d:%d%cD%d-%d%s\n" , xdm , Slot1 , 9 , Slot1 , swch , 9 , xdm , Slot1 , MsgSltchar ); // 2006.12.22
				}
				//
			}
			else {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 12 , s2 , p2 ); // 2017.09.11
				//
				if      ( xdm == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(B:%d)%cWHFMALRUN B:%d:%d%c%d%s\n" , Slot2 , 9 , Slot2 , swch , 9 , Slot2 , MsgSltchar ); // 2006.12.22
				}
				else if ( xdm == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(B:D%d)%cWHFMALRUN B:%d:%d%cD%d%s\n" , Slot2 , 9 , Slot2 , swch , 9 , Slot2 , MsgSltchar ); // 2006.12.22
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at AL(B:D%d-%d)%cWHFMALRUN B:%d:%d%cD%d-%d%s\n" , xdm , Slot2 , 9 , Slot2 , swch , 9 , xdm , Slot2 , MsgSltchar ); // 2006.12.22
				}
				//
			}
		}
	}
//	if ( !_i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( Address_FUNCTION_INTERFACE[FM] < 0 ) ) { // 2005.12.01
//	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( Address_FUNCTION_INTERFACE[FM] < 0 ) ) { // 2005.12.01 // 2011.07.26
//	if ( Address_FUNCTION_INTERFACE_FAL == -1 ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || ( GET_RUN_LD_CONTROL(5) > 0 ) ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
		KPLT_RESET_MFAL_RUN_TIMER();
	}
	else {
		_in_EQUIP_ALIGNING_CHECK_TIMER = -1;
		_in_EQUIP_ALIGNING_CHECK_LAST = SYS_RUNNING;
		//
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
			ACSlot = 1 + ACOffset; // 2008.09.01
		}
		else {
			ACSlot = 1 + ACOffset + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_AL ); // 2008.09.01
		}
		//
		if ( ( swch >= CM1 ) && ( swch < PM1 ) ) { // 2006.02.08
			sprintf( ParamF , "%d %d CM%d %d CM%d" , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch - CM1 + 1 );
			if ( outmode ) strcat( ParamF , " OUT" ); // 2010.12.22
		}
		else if ( ( swch >= PM1 ) && ( swch < AL ) ) { // 2006.02.08
			sprintf( ParamF , "%d %d CM%d %d PM%d" , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch - PM1 + 1 );
			if ( outmode ) strcat( ParamF , " OUT" ); // 2010.12.22
		}
		else if ( ( swch >= BM1 ) && ( swch < TM ) ) { // 2006.02.08
			sprintf( ParamF , "%d %d CM%d %d BM%d" , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , swch - BM1 + 1 );
			if ( outmode ) strcat( ParamF , " OUT" ); // 2010.12.22
		}
		else {
			sprintf( ParamF , "%d %d CM%d %d" , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot );
			if ( outmode ) strcat( ParamF , " - OUT" ); // 2010.12.22
		}
		//
//		sprintf( ParamF , "%d %d CM%d %d" , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot ); // 2006.02.08

		//_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_FAL , ParamF ); // 2008.04.03
		if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_FAL , ParamF ) ) return SYS_ABORTED; // 2008.04.03
	}
	//
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\tRSlot1=%d,RSlot2=%d,SrcCas=%d,swch=%d\n" , CHECKING_SIDE + 1 , "EQUIP_ALIGNING_SPAWN_FOR_FM_MAL" , RSlot1 , RSlot2 , SrcCas , swch );
	}
	//
	return SYS_SUCCESS;
}
//
int EQUIP_ALIGNING_STATUS_FOR_FM_MAL( int CHECKING_SIDE ) {
	DWORD cc;
	int altime;
	//
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) ) return SYS_SUCCESS;
	//
	if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() != 3 ) return SYS_ABORTED; // 2005.12.01
	//
//	if ( !_i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( Address_FUNCTION_INTERFACE[FM] < 0 ) ) { // 2005.12.01
//	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( Address_FUNCTION_INTERFACE[FM] < 0 ) ) { // 2005.12.01 // 2011.07.26
//	if ( Address_FUNCTION_INTERFACE_FAL == -1 ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || ( GET_RUN_LD_CONTROL(5) > 0 ) ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
//		if ( KPLT_CHECK_MFAL_RUN_TIMER( _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() ) ) return SYS_SUCCESS; // 2014.01.10
		//
		altime = _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() % 10; // 2014.01.10
		if ( altime <= 0 ) altime = 1; // 2014.01.10
		//
		if ( KPLT_CHECK_MFAL_RUN_TIMER( altime ) ) return SYS_SUCCESS; // 2014.01.10
		//
		return SYS_RUNNING;
	}
//	cc = clock(); // 2003.10.09
	cc = GetTickCount(); // 2003.10.09
	if ( _in_EQUIP_ALIGNING_CHECK_TIMER < 0 ) {
		_in_EQUIP_ALIGNING_CHECK_TIMER = cc;
		_in_EQUIP_ALIGNING_CHECK_LAST = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FAL );
	}
	else {
		if ( ( cc - _in_EQUIP_ALIGNING_CHECK_TIMER >= 100 ) || ( cc - _in_EQUIP_ALIGNING_CHECK_TIMER < 0 ) ) {
			_in_EQUIP_ALIGNING_CHECK_TIMER = cc;
			_in_EQUIP_ALIGNING_CHECK_LAST = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FAL );
		}
	}
	return _in_EQUIP_ALIGNING_CHECK_LAST;
}
//
int EQUIP_ALIGNING_SPAWN_FOR_TM_MAL( int CHECKING_SIDE , int Finger , int Slot , int SrcCas ) { // 2004.09.08
	char ParamF[256];
	int ACSlot , xdm;
	//
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) )	return SYS_SUCCESS;
	//
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\tFinger=%d,Slot=%d,SrcCas=%d\n" , CHECKING_SIDE + 1 , "EQUIP_ALIGNING_SPAWN_FOR_TM_MAL" , Finger , Slot , SrcCas );
	}
	//
	if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() != 3 ) return SYS_ABORTED; // 2005.12.01
	//
	if ( CHECKING_SIDE < TR_CHECKING_SIDE ) {
		//
		//=========================================
//		xdm = SCH_Inside_ThisIs_BM_OtherChamberD( SrcCas , 1 ); // 2017.09.11
		xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 13 , -1 , -1 ); // 2017.09.11
		//=========================================
		//
		if      ( xdm == -1 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Running at AL(%d)%cWHTMALRUN %d%c%d\n" , Slot , 9 , Slot , 9 , Slot );
		}
		else if ( xdm == 0 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Running at AL(D%d)%cWHTMALRUN %d%cD%d\n" , Slot , 9 , Slot , 9 , Slot );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM Handling Running at AL(D%d-%d)%cWHTMALRUN %d%cD%d-%d\n" , xdm , Slot , 9 , Slot , 9 , xdm , Slot );
		}
		//
	}
//	if ( !_i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM ) || ( Address_FUNCTION_INTERFACE[TM] < 0 ) ) { // 2005.12.01
//	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM ) || ( Address_FUNCTION_INTERFACE[TM] < 0 ) ) { // 2005.12.01 // 2011.07.26
//	if ( Address_FUNCTION_INTERFACE_FAL == -1 ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || ( GET_RUN_LD_CONTROL(5) > 0 ) ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
		KPLT_RESET_MFAL_RUN_TIMER();
	}
	else {
		_in_EQUIP_ALIGNING_CHECK_TIMER = -1;
		_in_EQUIP_ALIGNING_CHECK_LAST = SYS_RUNNING;
		//
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
			ACSlot = 1;
		}
		else {
			ACSlot = 1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , AL );
		}
		//
		sprintf( ParamF , "%c_AL %d CM%d %d" , Finger - TA_STATION + 'A' , Slot , SrcCas - CM1 + 1 , ACSlot ); \
//		_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_FAL , ParamF ); // 2008.04.03
		if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_FAL , ParamF ) ) return SYS_ABORTED; // 2008.04.03
	}
	//
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\tFinger=%d,Slot=%d,SrcCas=%d\n" , CHECKING_SIDE + 1 , "EQUIP_ALIGNING_SPAWN_FOR_TM_MAL" , Finger , Slot , SrcCas );
	}
	//
	return SYS_SUCCESS;
}

int EQUIP_ALIGNING_STATUS_FOR_TM_MAL( int CHECKING_SIDE ) { // 2004.09.08
	DWORD cc;
	int altime;
	//
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) )	return SYS_SUCCESS;
	if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() != 3 ) return SYS_ABORTED; // 2005.12.01
	//
//	if ( !_i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM ) || ( Address_FUNCTION_INTERFACE[TM] < 0 ) ) { // 2005.12.01
//	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM ) || ( Address_FUNCTION_INTERFACE[TM] < 0 ) ) { // 2005.12.01 // 2011.07.26
//	if ( Address_FUNCTION_INTERFACE_FAL == -1 ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
	if ( ( Address_FUNCTION_INTERFACE_FAL == -1 ) || ( GET_RUN_LD_CONTROL(5) > 0 ) ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
//		if ( KPLT_CHECK_MFAL_RUN_TIMER( _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() ) ) return SYS_SUCCESS; // 2014.01.10
		//
		altime = _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() % 10; // 2014.01.10
		if ( altime <= 0 ) altime = 1; // 2014.01.10
		//
		if ( KPLT_CHECK_MFAL_RUN_TIMER( altime ) ) return SYS_SUCCESS; // 2014.01.10
		//
		return SYS_RUNNING;
	}
	cc = GetTickCount();
	if ( _in_EQUIP_ALIGNING_CHECK_TIMER < 0 ) {
		_in_EQUIP_ALIGNING_CHECK_TIMER = cc;
		_in_EQUIP_ALIGNING_CHECK_LAST = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FAL );
	}
	else {
		if ( ( cc - _in_EQUIP_ALIGNING_CHECK_TIMER >= 100 ) || ( cc - _in_EQUIP_ALIGNING_CHECK_TIMER < 0 ) ) {
			_in_EQUIP_ALIGNING_CHECK_TIMER = cc;
			_in_EQUIP_ALIGNING_CHECK_LAST = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FAL );
		}
	}
	return _in_EQUIP_ALIGNING_CHECK_LAST;
}
//=================================================================================================================================================
//=================================================================================================================================================
//=================================================================================================================================================
//=================================================================================================================================================
//=================================================================================================================================================
//=================================================================================================================================================
//=================================================================================================================================================
//=================================================================================================================================================
int EQUIP_COOLING_SPAWN_FOR_FM_MIC( int CHECKING_SIDE , int icslot1 , int icslot2 , int Slot1 , int Slot2 , int SrcCas , int s1 , int p1 , int s2 , int p2 ) { // 2005.09.05
	char ParamF[256];
	int ACSlot , ACSlot2 , xdm;
	int xdm2; // 2017.09.11
	char MsgSltchar[16]; /* 2013.05.23 */
	//
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) return SYS_SUCCESS;
	//
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 0 ) return SYS_ABORTED; // 2005.12.01 // 2010.12.22
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) return SYS_ABORTED; // 2005.12.01 // 2010.12.22
	//
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) { // 2010.12.22
		//
//		return EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( CHECKING_SIDE , icslot1 , icslot2 , SrcCas , 0 , TRUE ); // 2010.12.22 // 2017.09.11
		return EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( CHECKING_SIDE , icslot1 , icslot2 , SrcCas , 0 , TRUE , s1 , p1 , s2 , p2 ); // 2010.12.22 // 2017.09.11
		//
	}
	else {
		//
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\ticslot1=%d,icslot2=%d,Slot1=%d,Slot2=%d,SrcCas=%d\n" , CHECKING_SIDE + 1 , "EQUIP_COOLING_SPAWN_FOR_FM_MIC" , icslot1 , icslot2 , Slot1 , Slot2 , SrcCas );
		}
		//
		if ( CHECKING_SIDE < TR_CHECKING_SIDE ) {
			//=========================================
//			xdm = SCH_Inside_ThisIs_BM_OtherChamberD( SrcCas , 1 ); // 2017.09.11
			//=========================================
			if ( ( icslot1 > 0 ) && ( icslot2 > 0 ) ) {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 14 , s1 , p1 ); // 2017.09.11
				xdm2 = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 14 , s2 , p2 ); // 2017.09.11
				//
				if ( xdm != xdm2 ) { // 2017.09.11
					//
					if      ( xdm == -1 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:%d)%cWHFMICRUN A:%d%c%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
						//
					}
					else if ( xdm == 0 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:D%d)%cWHFMICRUN A:%d%cD%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
						//
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:D%d-%d)%cWHFMICRUN A:%d%cD%d-%d%s\n" , xdm , Slot1 , 9 , Slot1 , 9 , xdm , Slot1 , MsgSltchar );
						//
					}
					//
					if      ( xdm2 == -1 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:%d)%cWHFMICRUN B:%d%c%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
					}
					else if ( xdm2 == 0 ) {
						//
						FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:D%d)%cWHFMICRUN B:%d%cD%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
					}
					else {
						//
						FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:D%d-%d)%cWHFMICRUN B:%d%cD%d-%d%s\n" , xdm2 , Slot2 , 9 , Slot2 , 9 , xdm2 , Slot2 , MsgSltchar );
					}
					//
				}
				else {
					//
					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
						if      ( xdm == -1 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(%d:%d)%cWHFMICRUN %d:%d%c%d\n" , Slot1 , Slot2 , 9 , Slot1 , Slot2 , 9 , Slot2 * 100 + Slot1 );
						}
						else if ( xdm == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(D%d:D%d)%cWHFMICRUN %d:%d%cD%d\n" , Slot1 , Slot2 , 9 , Slot1 , Slot2 , 9 , Slot2 * 100 + Slot1 );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(D%d-%d:D%d-%d)%cWHFMICRUN %d:%d%cD%d-%d\n" , xdm , Slot1 , xdm , Slot2 , 9 , Slot1 , Slot2 , 9 , xdm , Slot2 * 100 + Slot1 );
						}
					}
					else { // 2012.03.16
						if      ( xdm == -1 ) {
							//
							FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:%d)%cWHFMICRUN A:%d%c%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
							//
							FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:%d)%cWHFMICRUN B:%d%c%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
						}
						else if ( xdm == 0 ) {
							//
							FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:D%d)%cWHFMICRUN A:%d%cD%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
							//
							FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:D%d)%cWHFMICRUN B:%d%cD%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
						}
						else {
							//
							FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:D%d-%d)%cWHFMICRUN A:%d%cD%d-%d%s\n" , xdm , Slot1 , 9 , Slot1 , 9 , xdm , Slot1 , MsgSltchar );
							//
							FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:D%d-%d)%cWHFMICRUN B:%d%cD%d-%d%s\n" , xdm , Slot2 , 9 , Slot2 , 9 , xdm , Slot2 , MsgSltchar );
						}
					}
				}
				//
			}
			else if ( icslot1 > 0 ) {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 14 , s1 , p1 ); // 2017.09.11
				//
				FA_SIDE_TO_SLOT_GET_L( s1 , p1 , MsgSltchar );
				//
				if      ( xdm == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:%d)%cWHFMICRUN A:%d%c%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
				}
				else if ( xdm == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:D%d)%cWHFMICRUN A:%d%cD%d%s\n" , Slot1 , 9 , Slot1 , 9 , Slot1 , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(A:D%d-%d)%cWHFMICRUN A:%d%cD%d-%d%s\n" , xdm , Slot1 , 9 , Slot1 , 9 , xdm , Slot1 , MsgSltchar );
				}
				//
			}
			else if ( icslot2 > 0 ) {
				//
				xdm = SCH_Inside_ThisIs_DummyMethod( SrcCas , 1 , 14 , s2 , p2 ); // 2017.09.11
				//
				FA_SIDE_TO_SLOT_GET_L( s2 , p2 , MsgSltchar );
				//
				if      ( xdm == -1 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:%d)%cWHFMICRUN B:%d%c%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
				}
				else if ( xdm == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:D%d)%cWHFMICRUN B:%d%cD%d%s\n" , Slot2 , 9 , Slot2 , 9 , Slot2 , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM Handling Running at IC(B:D%d-%d)%cWHFMICRUN B:%d%cD%d-%d%s\n" , xdm , Slot2 , 9 , Slot2 , 9 , xdm , Slot2 , MsgSltchar );
				}
				//
			}
		}
//		if ( ( Address_FUNCTION_INTERFACE_FIC == -1 ) || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( Address_FUNCTION_INTERFACE[FM] < 0 ) ) { // 2005.12.01 // 2011.07.26
//		if ( Address_FUNCTION_INTERFACE_FIC == -1 ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
		if ( ( Address_FUNCTION_INTERFACE_FIC == -1 ) || ( GET_RUN_LD_CONTROL(6) > 0 ) ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
			KPLT_RESET_MFIC_RUN_TIMER( icslot1 , icslot2 , _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() , FALSE );
		}
		else {
			if ( ( icslot1 > 1 ) || ( icslot2 > 1 ) ) { // 2005.12.01
				//KPLT_RESET_MFIC_RUN_TIMER( icslot1 , icslot2 ); // 2005.12.01
				return SYS_ABORTED; // 2005.12.01
			}
			else {
				ACSlot  = 0;
				ACSlot2 = 0;
				//
				if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
					if ( Slot1 > 0 ) ACSlot  = Slot1;
					if ( Slot2 > 0 ) ACSlot2 = Slot2;
				}
				else {
					if ( Slot1 > 0 ) ACSlot  = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
					if ( Slot2 > 0 ) ACSlot2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , F_IC );
				}
				//
				sprintf( ParamF , "%d %d CM%d %d %d" , Slot1 , Slot2 , SrcCas - CM1 + 1 , ACSlot , ACSlot2 );
	//			_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_FIC , ParamF ); // 2008.04.03
				if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_FIC , ParamF ) ) return SYS_ABORTED; // 2008.04.03
			}
		}
		//
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\ticslot1=%d,icslot2=%d,Slot1=%d,Slot2=%d,SrcCas=%d\n" , CHECKING_SIDE + 1 , "EQUIP_COOLING_SPAWN_FOR_FM_MIC" , icslot1 , icslot2 , Slot1 , Slot2 , SrcCas );
		}
		//
	}
	//
	return SYS_SUCCESS;
}
//
int EQUIP_COOLING_STATUS_FOR_FM_MIC( int CHECKING_SIDE , int icslot ) {
	time_t finish;
	//
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) return SYS_SUCCESS;
	//
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 0 ) return SYS_ABORTED; // 2005.12.01 // 2010.12.22
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) return SYS_ABORTED; // 2005.12.01 // 2010.12.22
	//
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) { // 2010.12.22
		return EQUIP_ALIGNING_STATUS_FOR_FM_MAL( CHECKING_SIDE );
	}
	else {
//		if ( ( Address_FUNCTION_INTERFACE_FIC == -1 ) || !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( Address_FUNCTION_INTERFACE[FM] < 0 ) ) { // 2005.12.01 // 2011.07.26
//		if ( Address_FUNCTION_INTERFACE_FIC == -1 ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
		if ( ( Address_FUNCTION_INTERFACE_FIC == -1 ) || ( GET_RUN_LD_CONTROL(6) > 0 ) ) { // 2005.12.01 // 2011.07.26 // 2014.01.09
			time( &finish );
			if ( KPLT_CHECK_MFIC_RUN_TIMER1( icslot , finish ) ) return SYS_SUCCESS;
			return SYS_RUNNING;
		}
		if ( icslot > 1 ) { // 2005.12.01
			//if ( KPLT_CHECK_MFIC_RUN_TIMER1( icslot , finish ) ) return SYS_SUCCESS; // 2005.12.01
			//return SYS_RUNNING; // 2005.12.01
			return SYS_ABORTED; // 2005.12.01
		}
		else {
			return _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FIC );
		}
	}
	return SYS_SUCCESS;
}
//
void EQUIP_COOLING_TIMER_UPDATE_FOR_FM_MIC() { // 2005.12.01
	int i;
	time_t finish;
	//

//printf( "[A]" );
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) ) return;
	//
//printf( "[B]" );
	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() != 1 ) return; // 2010.12.22
	//
//	if ( ( Address_FUNCTION_INTERFACE_FIC != -1 ) && _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( Address_FUNCTION_INTERFACE[FM] >= 0 ) ) return; // 2011.07.26
//printf( "[C]" );
	if ( Address_FUNCTION_INTERFACE_FIC != -1 ) return; // 2011.07.26
	//
//printf( "[D]" );
	if ( GET_RUN_LD_CONTROL(0) > 0 ) return; // 2014.01.09
	//
//printf( "[E]" );
	if ( _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() == 0 ) return;
	//
//printf( "[F]" );
	time( &finish );
	//
	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
		WAFER_TIMER_SET_FIC( i , _i_SCH_MODULE_Chk_MFIC_RUN_TIMER( i , _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() , finish ) );
	}
}

