#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "Equip_Handling.h"
#include "sch_CassetteResult.h"
#include "sch_sub.h"
#include "sch_prm.h"
#include "sch_prm_FBTPM.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Transfer_Enable_Check( int ch ) { // 2003.06.25
	//------------------------------------------------------
	if ( ( ch >= PM1 ) && ( ch < AL ) ) { // 2003.11.05
		if ( EQUIP_STATUS_PROCESS( 0 , ch ) == SYS_RUNNING ) return FALSE;
	}
	//
	if ( ( ch >= BM1 ) && ( ch < TM ) ) { // 2018.11.20
		if ( SCH_Inside_ThisIs_BM_OtherChamber( ch , 1 ) ) return FALSE; // 2018.11.20
	}
	//------------------------------------------------------
	//     DEFAULT(0)   PM_ENABLE(1)  BM_ENABLE(6) BPM_ENABLE(7) ALL_ENABLE(2)  TM_ENABLE(8)  TPM_ENABLE(9) TBM_ENABLE(10)
	//PM      D/E       E             D/E          E             E              D/E           E             D/E
	//BM      D/E       D/E           E            E             E              D/E           D/E           E
	//TM      D/E       D/E           D/E          D/E           E              E             E             E
	//------------------------------------------------------
	switch( _i_SCH_PRM_GET_MODULE_TRANSFER_CONTROL_MODE() ) {
	case 1 : // PM_ENABLE
	case 4 : // PM_ENABLE
		if ( ( ch >= PM1 ) && ( ch < AL ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else {
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
			}
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		}
		break;
	//
	case 2 : // ALL_ENABLE
	case 5 : // ALL_ENABLE
		return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		break;
	//========================================================================================================
	// 2005.10.10
	//========================================================================================================
	case 6 : // BM_ENABLE
	case 11 : // BM_ENABLE
		if      ( ( ch >= BM1 ) && ( ch < TM ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else {
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
			}
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		}
		break;
	//
	case 7 : // BPM_ENABLE
	case 12 : // BPM_ENABLE
		if      ( ( ch >= BM1 ) && ( ch < TM ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else if ( ( ch >= PM1 ) && ( ch < AL ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else {
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
			}
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		}
		break;
	//
	case 8 : // TM_ENABLE
	case 13 : // TM_ENABLE
		if      ( ( ch >= TM ) && ( ch < FM ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else {
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
			}
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		}
		break;
	//
	case 9 : // TPM_ENABLE
	case 14 : // TPM_ENABLE
		if      ( ( ch >= TM ) && ( ch < FM ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else if ( ( ch >= PM1 ) && ( ch < AL ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else {
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
			}
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		}
		break;
	//
	case 10 : // TBM_ENABLE
	case 15 : // TBM_ENABLE
		if      ( ( ch >= TM ) && ( ch < FM ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else if ( ( ch >= BM1 ) && ( ch < TM ) ) {
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , TR_CHECKING_SIDE );
		}
		else {
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
			}
			return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		}
		break;
	//========================================================================================================
	default :
		if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_WAIT ) {
			SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_NONE );
		}
		if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) == ABORTWAIT_FLAG_ABORTWAIT ) {
			SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORT );
		}
		return _i_SCH_MODULE_GET_USE_ENABLE( ch , TRUE , TR_CHECKING_SIDE );
		break;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Transfer_Enable_Check_until_Process_Finished() { // 2005.01.06
	int i;
	for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
		if ( EQUIP_STATUS_PROCESS( 0 , i ) == SYS_RUNNING ) {
			while( TRUE ) {
				if ( !WAIT_SECONDS( 0.1 ) ) return FALSE;
				if ( EQUIP_STATUS_PROCESS( 0 , i ) != SYS_RUNNING ) break;
			}
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Transfer_Enable_InterlockFinger_Check_for_Sub( int Mode , int tms , int Ch , int finger , int skippickch ) { // 2003.10.16
	int Data;

	switch( _i_SCH_PRM_GET_MODULE_TRANSFER_CONTROL_MODE() ) { // 2003.11.05
	case 3 :
	case 4 :
	case 5 :
		//
	case 11 : // 2005.10.10
	case 12 : // 2005.10.10
	case 13 : // 2005.10.10
	case 14 : // 2005.10.10
	case 15 : // 2005.10.10
		return TRUE;
		break;
	}
	if ( ( Ch >= CM1 ) && ( Ch < PM1 ) ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) return TRUE;
		if ( Mode == TR_PICK ) {
			Data = _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( Ch );
		}
		else {
			Data = _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( Ch );
		}
		if ( Data > 0 ) {
			if ( skippickch != ( Data + PM1 - 1 ) ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( Data + PM1 - 1 , 1 ) > 0 ) {
					return FALSE;
				}
			}
		}
	}
	else if ( ( Ch >= PM1 ) && ( Ch < AL ) ) {
		if ( Mode == TR_PICK ) {
			Data = _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( Ch );
			if ( Data == 1 ) {
				if ( WAFER_STATUS_IN_TM( tms , TA_STATION ) > 0 ) {
					return FALSE;
				}
				if ( finger == TB_STATION ) return FALSE;
			}
			else if ( Data == 2 ) {
				if ( WAFER_STATUS_IN_TM( tms , TB_STATION ) > 0 ) {
					return FALSE;
				}
				if ( finger == TA_STATION ) return FALSE;
			}
		}
		else {
			Data = _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( Ch );
			if ( Data == 1 ) {
				if ( skippickch == -1 ) {
					if ( WAFER_STATUS_IN_TM( tms , TA_STATION ) <= 0 ) {
						return FALSE;
					}
				}
				if ( finger == TB_STATION ) return FALSE;
			}
			else if ( Data == 2 ) {
				if ( skippickch == -1 ) {
					if ( WAFER_STATUS_IN_TM( tms , TB_STATION ) <= 0 ) {
						return FALSE;
					}
				}
				if ( finger == TA_STATION ) return FALSE;
			}
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------
void Transfer_Cass_Info_Operation_Before_Pick( int Ch , int Slot ) {
	if ( ( Ch < CM1 ) || ( Ch >= PM1 ) ) return;
	if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
	_i_SCH_CASSETTE_LAST_RESET( Ch , Slot );
	SCHEDULER_CASSETTE_LAST_RESULT_INIT_AFTER_CM_PICK( Ch , Slot , -1 , -1 ); // 2003.10.21
}
//------------------------------------------------------------------------------------------
void Transfer_Cass_Info_Operation_After_Place( int Ch , int Slot ) {
	if ( ( Ch < CM1 ) || ( Ch >= PM1 ) ) return;
	if ( ( Slot <= 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) return;
	switch( _i_SCH_CASSETTE_LAST_RESULT_GET( Ch , Slot ) ) {
	case 0 : _i_SCH_IO_SET_MODULE_STATUS( Ch , Slot , CWM_PROCESS ); break;
	case 1 : _i_SCH_IO_SET_MODULE_STATUS( Ch , Slot , CWM_FAILURE ); break;
	}
	_i_SCH_CASSETTE_LAST_RESET( Ch , Slot );
}
//===================================================================================================
//===================================================================================================
//===================================================================================================
