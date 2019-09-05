#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "Robot_Handling.h"
#include "IO_Part_Log.h"
#include "Equip_Handling.h"
#include "sch_CassetteResult.h"
#include "sch_cassmap.h"
#include "sch_prm.h"
#include "sch_prm_FBTPM.h"
#include "sch_sub.h"
#include "sch_util.h"
#include "iolog.h"
#include "sch_main.h"
#include "sch_transfer.h"
#include "FA_Handling.h"
#include "Errorcode.h"
#include "sch_ProcessMonitor.h"
#include "sch_estimate.h"

#include "INF_sch_CommonUser.h"

//------------------------------------------------------------------------------------------
BOOL _SCH_MACRO_FM_DATABASE_OPERATION_MAINT_INF( BOOL optchk , int fms , int mode , int och , int slot1 , int slot2 );
BOOL _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( BOOL optchk , int tms , int mode , int och , int arm , int Slot , int Depth );
BOOL _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( BOOL optchk , int ch , int mode ); // 2012.11.21
//------------------------------------------------------------------------------------------
int TCfLL_RUNNING[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
//
int TCfLL_CHECK[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_TMATM[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_Chamber[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_Finger[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_Slot[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_Depth[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_ArmASlot[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_ArmBSlot[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_SrcCass[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_BMChamber[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_TransferID[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1];
int TCfLL_Order[MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1]; // 2004.06.23

//=============================================================================================
BOOL Transfer_Maint_Inf_Code_End( int c ) {
	int i;
	if ( c == -1 ) {
		for ( i = 0 ; i < (MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1 ) ; i++ ) {
			if ( TCfLL_RUNNING[i] ) return FALSE;
		}
	}
	else {
		if ( TCfLL_RUNNING[c] ) return FALSE;
	}
	return TRUE;
}
//=============================================================================================
void Transfer_Maint_Inf_Code_Start_IO_Set( int index ) {
	IO_TR_STATUS_SET( index , SYS_RUNNING );
	TCfLL_RUNNING[index] = TRUE;
}
//
void Transfer_Maint_Inf_Code_End_IO_Set( int index , int result ) {
	switch( result ) {
	case SYS_SUCCESS : IO_TR_STATUS_SET( index , SYS_SUCCESS ); break;
	case SYS_ERROR   : IO_TR_STATUS_SET( index , SYS_ERROR ); break;
	default          : IO_TR_STATUS_SET( index , SYS_ABORTED ); break;
	}
	TCfLL_RUNNING[index] = FALSE;
}
//
void Transfer_Maint_Inf_Code_Interface_Data_Setting( int check , int index , int TMATM , int Chamber , int Finger , int Slot , int Depth , int ArmASlot , int ArmBSlot , int SrcCass , int BMChamber , int TransferID , int order ) {
	TCfLL_CHECK[index]			= check; // 2006.11.10
	TCfLL_TMATM[index]			= TMATM;
	TCfLL_Chamber[index]		= Chamber;
	TCfLL_Finger[index]			= Finger;
	TCfLL_Slot[index]			= Slot;
	TCfLL_Depth[index]			= Depth;
	TCfLL_ArmASlot[index]		= ArmASlot;
	TCfLL_ArmBSlot[index]		= ArmBSlot;
	TCfLL_SrcCass[index]		= SrcCass;
	TCfLL_BMChamber[index]		= BMChamber;
//====================================================================
// 2006.11.09
//====================================================================
//	TCfLL_TransferID[index]		= TransferID; // 2006.11.09
	switch( TransferID ) {
	case 101 :	TCfLL_TransferID[index] = -3;	break;
	case 102 :	TCfLL_TransferID[index] = -2;	break;
	case 103 :	TCfLL_TransferID[index] = -1;	break;
	case 104 :	TCfLL_TransferID[index] = 0;	break;
	default :	TCfLL_TransferID[index] = TransferID;	break;
	}
//====================================================================
	TCfLL_Order[index]			= order; // 2004.06.23
}
//============================================================================================================
//============================================================================================================
//============================================================================================================
BOOL Transfer_Maint_Inf_Code_Check_TM( int tms , int mode , int ch , int arm , int slot , int depth , int type ) { // 2006.11.10
	BOOL exttrgyes = FALSE;
	//=======================================================================================
	if ( GET_RUN_LD_CONTROL(0) > 0 ) return TRUE; // 2014.09.04
	//=======================================================================================
	//=======================================================================================
	// Module Check
	//=======================================================================================
	if ( ch != -1 ) {
		if ( ( mode == 1 ) || ( mode == 3 ) ) {
			if ( !Transfer_Enable_Check( ch ) ) return FALSE;
		}
		if ( type == TR_PICK ) { // pick
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { // cm part
					switch( _i_SCH_IO_GET_MODULE_STATUS( ch , slot ) ) {
					case 2 : // Present
					case 3 : // Process
						break;
					default :
						return FALSE;
						break;
					}
				}
				else if ( ( ch >= PM1 ) && ( ch < AL ) ) { // pm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) <= 0 ) return FALSE;
				}
				else if ( ch == AL ) { // pm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) <= 0 ) return FALSE;
				}
				else if ( ch == IC ) { // pm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) <= 0 ) return FALSE;
				}
				else if ( ( ch >= BM1 ) && ( ch < TM ) ) { // bm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , depth ) <= 0 ) return FALSE;
				}
				else { // ? part
					return FALSE;
				}
			}
		}
		else if ( type == TR_PLACE ) { // place
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { // cm part
					switch( _i_SCH_IO_GET_MODULE_STATUS( ch , slot ) ) {
					case 1 : // Absent
						break;
					default :
						return FALSE;
						break;
					}
				}
				else if ( ( ch >= PM1 ) && ( ch < AL ) ) { // pm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) return FALSE;
				}
				else if ( ch == AL ) { // AL part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) return FALSE;
				}
				else if ( ch == IC ) { // IC part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) return FALSE;
				}
				else if ( ( ch >= BM1 ) && ( ch < TM ) ) { // bm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , depth ) > 0 ) return FALSE;
				}
				else { // ? part
					return FALSE;
				}
			}
		}
		else if ( type == TR_EXTEND ) { // extend
			exttrgyes = FALSE;
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { // cm part
					switch( _i_SCH_IO_GET_MODULE_STATUS( ch , slot ) ) {
					case 1 : // Absent
						break;
					default :
						exttrgyes = TRUE;
						break;
					}
				}
				else if ( ( ch >= PM1 ) && ( ch < AL ) ) { // pm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) exttrgyes = TRUE;
				}
				else if ( ch == AL ) { // AL part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) exttrgyes = TRUE;
				}
				else if ( ch == IC ) { // IC part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) exttrgyes = TRUE;
				}
				else if ( ( ch >= BM1 ) && ( ch < TM ) ) { // bm part
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , depth ) > 0 ) exttrgyes = TRUE;
				}
				else { // ? part
					return FALSE;
				}
			}
		}
	}
	//=======================================================================================
	// Robot Check
	//=======================================================================================
	if ( tms != -1 ) {
		if ( ( mode == 1 ) || ( mode == 3 ) ) {
			if ( !Transfer_Enable_Check( TM + tms ) ) return FALSE;
		}
		if ( arm != -1 ) {
			if      ( type == TR_PICK ) { // pick
				if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , arm ) ) return FALSE;
				if ( ( mode == 1 ) || ( mode == 2 ) ) {
					if ( WAFER_STATUS_IN_TM( tms , arm ) > 0 ) return FALSE;
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tms , ch , arm , -1 ) ) return FALSE;
				}
			}
			else if ( type == TR_PLACE ) { // place
				if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , arm ) ) return FALSE;
				if ( ( mode == 1 ) || ( mode == 2 ) ) {
					if ( WAFER_STATUS_IN_TM( tms , arm ) <= 0 ) return FALSE;
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tms , ch , arm , -1 ) ) return FALSE;
				}
			}
			else if ( type == TR_EXTEND ) { // extend
				if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , arm ) ) return FALSE;
				if ( ( mode == 1 ) || ( mode == 2 ) ) {
					if ( exttrgyes ) {
						if ( WAFER_STATUS_IN_TM( tms , arm ) > 0 ) return FALSE;
					}
				}
			}
			else if ( type == TR_MOVE ) { // Robot
				if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , arm ) ) return FALSE;
			}
		}
	}
	//=======================================================================================
	return TRUE;
}
//============================================================================================================
//============================================================================================================
//============================================================================================================
BOOL Transfer_Maint_Inf_Code_Check_FM( int fms , int mode , int ch , int arm1slot , int arm2slot , int type ) { // 2006.11.10
	//=======================================================================================
	if ( GET_RUN_LD_CONTROL(0) > 0 ) return TRUE; // 2014.09.04
	//=======================================================================================
	// Module Check
	//=======================================================================================
	if ( ch != -1 ) {
		if ( ( mode == 1 ) || ( mode == 3 ) ) {
			if ( ( ch == AL ) || ( ch == F_AL ) ) { // FAL part
				if ( !Transfer_Enable_Check( F_AL ) ) return FALSE;
			}
			else if ( ( ch == IC ) || ( ch == F_IC ) ) { // FIC part
				if ( !Transfer_Enable_Check( F_IC ) ) return FALSE;
			}
			else {
				if ( !Transfer_Enable_Check( ch ) ) return FALSE;
			}
			if ( arm1slot == arm2slot ) return FALSE;
		}
		if ( type == TR_PICK ) { // pick
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { // cm part
					if ( arm1slot > 0 ) {
						switch( _i_SCH_IO_GET_MODULE_STATUS( ch , arm1slot ) ) {
						case 2 : // Present
						case 3 : // Process
							break;
						default :
							return FALSE;
							break;
						}
					}
					if ( arm2slot > 0 ) {
						switch( _i_SCH_IO_GET_MODULE_STATUS( ch , arm2slot ) ) {
						case 2 : // Present
						case 3 : // Process
							break;
						default :
							return FALSE;
							break;
						}
					}
				}
				else if ( ( ch >= PM1 ) && ( ch < AL ) ) { // pm part
					if ( ( arm1slot > 0 ) && ( arm2slot > 0 ) ) return FALSE;
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) <= 0 ) return FALSE;
				}
				else if ( ( ch == AL ) || ( ch == F_AL ) ) { // FAL part
//					if ( ( arm1slot > 0 ) && ( arm2slot > 0 ) ) return FALSE; // 2008.09.10
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 ) <= 0 ) return FALSE;
				}
				else if ( ( ch == IC ) || ( ch == F_IC ) ) { // FIC part
					if ( arm1slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , arm1slot ) <= 0 ) return FALSE;
					}
					if ( arm2slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , arm2slot ) <= 0 ) return FALSE;
					}
				}
				else if ( ( ch >= BM1 ) && ( ch < TM ) ) { // bm part
					if ( arm1slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( ch , arm1slot ) <= 0 ) return FALSE;
					}
					if ( arm2slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( ch , arm2slot ) <= 0 ) return FALSE;
					}
				}
				else { // ? part
					return FALSE;
				}
			}
		}
		else if ( type == TR_PLACE ) { // place
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( ( ch >= CM1 ) && ( ch < PM1 ) ) { // cm part
					if ( arm1slot > 0 ) {
						switch( _i_SCH_IO_GET_MODULE_STATUS( ch , arm1slot ) ) {
						case 1 : // Absent
							break;
						default :
							return FALSE;
							break;
						}
					}
					if ( arm2slot > 0 ) {
						switch( _i_SCH_IO_GET_MODULE_STATUS( ch , arm2slot ) ) {
						case 1 : // Absent
							break;
						default :
							return FALSE;
							break;
						}
					}
				}
				else if ( ( ch >= PM1 ) && ( ch < AL ) ) { // pm part
					if ( ( arm1slot > 0 ) && ( arm2slot > 0 ) ) return FALSE;
					if ( _i_SCH_IO_GET_MODULE_STATUS( ch , 1 ) > 0 ) return FALSE;
				}
				else if ( ( ch == AL ) || ( ch == F_AL ) ) { // FAL part
//					if ( ( arm1slot > 0 ) && ( arm2slot > 0 ) ) return FALSE; // 2008.09.10
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 ) > 0 ) return FALSE;
				}
				else if ( ( ch == IC ) || ( ch == F_IC ) ) { // FIC part
					if ( arm1slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , arm1slot ) > 0 ) return FALSE;
					}
					if ( arm2slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , arm2slot ) > 0 ) return FALSE;
					}
				}
				else if ( ( ch >= BM1 ) && ( ch < TM ) ) { // bm part
					if ( arm1slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( ch , arm1slot ) > 0 ) return FALSE;
					}
					if ( arm2slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( ch , arm2slot ) > 0 ) return FALSE;
					}
				}
				else { // ? part
					return FALSE;
				}
			}
		}
	}
	//=======================================================================================
	// Robot Check
	//=======================================================================================
	if ( ( mode == 1 ) || ( mode == 3 ) ) {
		if ( !Transfer_Enable_Check( FM ) ) return FALSE;
	}
	if ( ( arm1slot > 0 ) || ( arm2slot > 0 ) ) {
		if      ( type == TR_PICK ) { // pick
			if ( arm1slot > 0 ) {
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) ) return FALSE;
			}
			if ( arm2slot > 0 ) {
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) return FALSE;
			}
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( arm1slot > 0 ) {
					if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) > 0 ) return FALSE;
				}
				if ( arm2slot > 0 ) {
					if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) > 0 ) return FALSE;
				}
			}
		}
		else if ( type == TR_PLACE ) { // place
			if ( arm1slot > 0 ) {
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) ) return FALSE;
			}
			if ( arm2slot > 0 ) {
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) return FALSE;
			}
			if ( ( mode == 1 ) || ( mode == 2 ) ) {
				if ( arm1slot > 0 ) {
					if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) <= 0 ) return FALSE;
				}
				if ( arm2slot > 0 ) {
					if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) <= 0 ) return FALSE;
				}
			}
		}
		else if ( type == TR_MOVE ) { // Robot
			if ( arm1slot > 0 ) {
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) ) return FALSE;
			}
			if ( arm2slot > 0 ) {
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) return FALSE;
			}
		}
	}
	//=======================================================================================
	return TRUE;
}
//============================================================================================================
//============================================================================================================
//============================================================================================================
void Transfer_Maint_Inf_Code_for_Low_Level( int SignalOrg ) {
	int i , Result = SYS_ABORTED;
	int Signal , Index , MaxSlot , maxunuse;

	//---------------------------------------------------
	EnterCriticalSection( &CR_MAINT_INF_TRANSFER ); // 2006.08.30
	//---------------------------------------------------
	Index  = SignalOrg / 100;
	Signal = SignalOrg % 100;
	//---------------------------------------------------
	_i_SCH_SYSTEM_FLAG_RESET( TR_CHECKING_SIDE ); // 2006.02.10
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2007.02.26
		if ( _i_SCH_SYSTEM_USING_GET(i) <= 0 ) _i_SCH_SYSTEM_FLAG_RESET( i ); // 2007.02.26
	} // 2007.02.26
	//----------------------------------
	BUTTON_SET_TR_CONTROL( CTC_RUNNING ); // 2003.10.28
	//----------------------------------
////	if ( Signal <= 0 ) { // 2006.08.29 // 2006.08.30
//	if ( SignalOrg <= 0 ) { // 2006.08.29 // 2006.08.30
//		//----------------------------------
//		IO_TR_STATUS_SET( SYS_ABORTED ); // 2006.08.30
//		//----------------------------------
//		_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 0 ); // 2006.08.30
//		//----------------------------------
//		BUTTON_SET_TR_CONTROL( CTC_IDLE ); // 2003.10.28 // 2006.08.30
//		//----------------------------------
//		_endthread(); // 2006.08.30
//		return; // 2006.08.30
//	} // 2006.08.30
	//----------------------------------
	Transfer_Maint_Inf_Code_Start_IO_Set( Index );
	//----------------------------------
//	_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 2 ); // 2012.07.28
	//
	if ( _i_SCH_SYSTEM_USING_GET( TR_CHECKING_SIDE ) < 3 ) _i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 2 ); // 2012.07.28
	//----------------------------------
	MANAGER_ABORT_FALSE(); // 2006.02.15
	//----------------------------------
	LeaveCriticalSection( &CR_MAINT_INF_TRANSFER ); // 2006.08.30

	_i_SCH_IO_MTL_SAVE();

	switch ( Signal ) {
	case 1 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_PICK ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_PICK , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 2 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_PLACE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_PLACE , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 3 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_RETPIC , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 4 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_RETPLC , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 5 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_EXTEND ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_EXTPIC , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 6 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_EXTEND ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_EXTPLC , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 7 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_ROTATE , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 8 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_EXTEND ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_EXTEND , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 9 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_RETRACT , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 10 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_UP , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 11 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_DOWN , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 12 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , -1 , -1 , 0 , 0 , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_CONTROL_MAINT_INF( TCfLL_TMATM[Index] , RB_MODE_HOME , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TCfLL_TransferID[Index] , FALSE , 0 , 0 , TCfLL_SrcCass[Index] , TCfLL_Order[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 13 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			if ( _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_TMATM[Index] , MACRO_PICK , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] ) ) { // 2012.11.05
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_PICK ) ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2008.02.15
					if ( ( TCfLL_Chamber[Index] < AL ) && ( TCfLL_Chamber[Index] >= PM1 ) && ( ( ( TCfLL_Chamber[Index] - PM1 ) % 2 ) == 0 ) ) {
						if      ( ( ( TCfLL_Depth[Index] / 100 ) > 0 ) && ( ( TCfLL_Depth[Index] % 100 ) > 0 ) ) {
							Result = EQUIP_PICK_FROM_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , TRUE );
						}
						else if ( ( ( TCfLL_Depth[Index] / 100 ) > 0 ) && ( ( TCfLL_Depth[Index] % 100 ) <= 0 ) ) {
							Result = EQUIP_PICK_FROM_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] + 1 , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
						}
						else {
							Result = EQUIP_PICK_FROM_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
						}
					}
					else {
						Result = EQUIP_PICK_FROM_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
					}
				}
				else {
					Result = EQUIP_PICK_FROM_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
				}
				//
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_TMATM[Index] , MACRO_PICK , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] ); // 2012.01.19
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 14 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			if ( _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_TMATM[Index] , MACRO_PLACE , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] ) ) { // 2012.11.05
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_PLACE ) ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2008.02.15
					if ( ( TCfLL_Chamber[Index] < AL ) && ( TCfLL_Chamber[Index] >= PM1 ) && ( ( ( TCfLL_Chamber[Index] - PM1 ) % 2 ) == 0 ) ) {
						if      ( ( ( TCfLL_Depth[Index] / 100 ) > 0 ) && ( ( TCfLL_Depth[Index] % 100 ) > 0 ) ) {
							Result = EQUIP_PLACE_TO_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , 0 , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , TRUE );
						}
						else if ( ( ( TCfLL_Depth[Index] / 100 ) > 0 ) && ( ( TCfLL_Depth[Index] % 100 ) <= 0 ) ) {
							Result = EQUIP_PLACE_TO_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] + 1 , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , 0 , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
						}
						else {
							Result = EQUIP_PLACE_TO_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , 0 , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
						}
					}
					else {
						Result = EQUIP_PLACE_TO_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , 0 , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
					}
				}
				else {
					Result = EQUIP_PLACE_TO_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , 0 , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
				}
				//
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_TMATM[Index] , MACRO_PLACE , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] ); // 2011.12.07
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 15 : // 2013.01.11
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			if ( _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_TMATM[Index] , MACRO_PICKPLACE , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] ) ) { // 2012.11.05
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , TR_PICK ) ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2008.02.15
					if ( ( TCfLL_Chamber[Index] < AL ) && ( TCfLL_Chamber[Index] >= PM1 ) && ( ( ( TCfLL_Chamber[Index] - PM1 ) % 2 ) == 0 ) ) {
						if      ( ( ( TCfLL_Depth[Index] / 100 ) > 0 ) && ( ( TCfLL_Depth[Index] % 100 ) > 0 ) ) {
							Result = EQUIP_SWAP_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , TRUE );
						}
						else if ( ( ( TCfLL_Depth[Index] / 100 ) > 0 ) && ( ( TCfLL_Depth[Index] % 100 ) <= 0 ) ) {
							Result = EQUIP_SWAP_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] + 1 , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
						}
						else {
							Result = EQUIP_SWAP_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
						}
					}
					else {
						Result = EQUIP_SWAP_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
					}
				}
				else {
					Result = EQUIP_SWAP_CHAMBER_MAINT_INF( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] , FALSE , TCfLL_SrcCass[Index] , FALSE , 0 , TCfLL_Order[Index] , FALSE );
				}
				//
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_TM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_TMATM[Index] , MACRO_PICKPLACE , TCfLL_Chamber[Index] , TCfLL_Finger[Index] , TCfLL_Slot[Index] , TCfLL_Depth[Index] ); // 2012.01.19
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	//===================================================================================================================================================
	case 21 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , -1 , 0 , 0 , TR_MOVE ) ) {
				Result = EQUIP_HOME_FROM_FMBM( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 22 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			if ( _SCH_MACRO_FM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_TMATM[Index] , MACRO_PICK , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] ) ) { // 2012.11.05
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PICK ) ) {
				Result = EQUIP_PICK_FROM_FMBM( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , FALSE , 0 , TCfLL_Order[Index] , TRUE );
				//
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_FM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_TMATM[Index] , MACRO_PICK , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] ); // 2011.12.07
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 23 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			if ( _SCH_MACRO_FM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_TMATM[Index] , MACRO_PLACE , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] ) ) { // 2011.12.07
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PLACE ) ) {
				Result = EQUIP_PLACE_TO_FMBM( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , FALSE , 0 , TCfLL_Order[Index] , TRUE );
				//
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_FM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_TMATM[Index] , MACRO_PLACE , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] ); // 2011.12.07
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 24 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_FM_MOVE_CONTROL( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , 2 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 25 : // 2011.04.11 extend
	case 26 : // 2011.04.11 retract
	case 27 : // 2011.04.11 up
	case 28 : // 2011.04.11 down
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , -1 , 0 , 0 , TR_MOVE ) ) {
				Result = EQUIP_ROBOT_FROM_FMBM( TCfLL_TMATM[Index] , Signal - 25 /* */ , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	//===================================================================================================================================================
	case 31 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PLACE ) ) {
				Result = EQUIP_PLACE_AND_PICK_WITH_FALIC( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TCfLL_SrcCass[Index] , TCfLL_BMChamber[Index] , FAL_RUN_MODE_PLACE_MDL_PICK , FALSE , 0 , 0 , -1 , -1 , -1 , -1 , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 32 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PICK ) ) {
				Result = EQUIP_PLACE_AND_PICK_WITH_FALIC( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TCfLL_SrcCass[Index] , TCfLL_BMChamber[Index] , FAL_RUN_MODE_PICK , FALSE , 0 , 0 , -1 , -1 , -1 , -1 , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 33 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PLACE ) ) {
				Result = EQUIP_PLACE_AND_PICK_WITH_FALIC( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TCfLL_SrcCass[Index] , TCfLL_BMChamber[Index] , FAL_RUN_MODE_PLACE_MDL , FALSE , 0 , 0 , -1 , -1 , -1 , -1 , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 34 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PICK ) ) {
				Result = EQUIP_PLACE_AND_PICK_WITH_FALIC( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TCfLL_SrcCass[Index] , TCfLL_BMChamber[Index] , FAL_RUN_MODE_MDL_PICK , FALSE , 0 , 0 , -1 , -1 , -1 , -1 , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 35 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_FM( TCfLL_TMATM[Index] , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TR_PLACE ) ) {
				Result = EQUIP_PLACE_AND_PICK_WITH_FALIC( TCfLL_TMATM[Index] , TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TCfLL_SrcCass[Index] , TCfLL_BMChamber[Index] , FAL_RUN_MODE_PLACE, FALSE , 0  , 0 , -1 , -1 , -1 , -1 , TRUE );
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	//===================================================================================================================================================
	case 50 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			if ( Transfer_Maint_Inf_Code_Check_TM( -1 , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] - 1 , 0 , 0 , 0 , TR_UNKNOWN ) ) {
				MaxSlot = Scheduler_Get_Max_Slot( TCfLL_Chamber[Index] - 1 , TCfLL_Chamber[Index] , TCfLL_Chamber[Index] , 41 , &maxunuse ); // 2010.12.17
				Result = EQUIP_CARRIER_MAPPING( TCfLL_Chamber[Index] - 1 , TRUE , FALSE , TRUE , FALSE , TCfLL_Chamber[Index] , TCfLL_Chamber[Index] , TRUE , TRUE , 0 , MaxSlot , 2 , &i ); // 2010.12.17
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	//===================================================================================================================================================
	case 51 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
//			Result = SYS_ERROR; // 2012.11.21
			if ( _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_Chamber[Index] , BUFFER_TM_STATION ) ) { // 2012.11.21
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
	//		Result = EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , TCfLL_Chamber[Index] , 0 , -1 , -1 ); // 2006.11.10
			if ( Transfer_Maint_Inf_Code_Check_TM( -1 , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , 0 , 0 , 0 , TR_UNKNOWN ) ) {
				Result = EQUIP_RUN_WAITING_BM( TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , TRUE ); // 2006.11.10
				//
//				if ( Result == SYS_SUCCESS ) _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_Chamber[Index] , BUFFER_TM_STATION ); // 2012.11.21 // 2013.01.29
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( TCfLL_SrcCass[Index] , TCfLL_Chamber[Index] , BUFFER_TM_STATION ); // 2012.11.21 // 2013.01.29
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	case 52 :
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
//			Result = SYS_ERROR; // 2012.11.21
			if ( _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( FALSE , TCfLL_Chamber[Index] , BUFFER_FM_STATION ) ) { // 2012.11.21
				Result = SYS_SUCCESS;
			}
			else {
				Result = SYS_ERROR;
			}
		}
		else {
	//		Result = EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , TCfLL_Chamber[Index] , 1 , -1 , -1 ); // 2006.11.10
			if ( Transfer_Maint_Inf_Code_Check_TM( -1 , TCfLL_CHECK[Index] , TCfLL_Chamber[Index] , 0 , 0 , 0 , TR_UNKNOWN ) ) {
				Result = EQUIP_RUN_WAITING_BM( TCfLL_TransferID[Index] + TR_CHECKING_SIDE - 1 , TCfLL_Chamber[Index] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , TRUE ); // 2006.11.10
				//
//				if ( Result == SYS_SUCCESS ) _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( TRUE , TCfLL_Chamber[Index] , BUFFER_FM_STATION ); // 2012.11.21 // 2013.01.29
				if ( Result == SYS_SUCCESS ) _SCH_MACRO_BM_DATABASE_OPERATION_MAINT_INF( TCfLL_SrcCass[Index] , TCfLL_Chamber[Index] , BUFFER_FM_STATION ); // 2012.11.21 // 2013.01.29
				//
			}
			else {
				Result = SYS_ERROR;
			}
		}
		break;
	//
	//===================================================================================================================================================
	case 53 : // 2003.10.28
		if ( TCfLL_CHECK[Index] == -1 ) { // 2012.11.05
			Result = SYS_ERROR;
		}
		else {
			TCfLL_ArmASlot[Index]	= WAFER_STATUS_IN_TM( 0 , TA_STATION );
			TCfLL_ArmBSlot[Index]	= WAFER_STATUS_IN_TM( 0 , TB_STATION );
			TCfLL_Depth[Index]		= _i_SCH_IO_GET_MODULE_STATUS( TCfLL_Chamber[Index] , 3 );
			TCfLL_SrcCass[Index]	= _i_SCH_IO_GET_MODULE_STATUS( TCfLL_Chamber[Index] , 2 );
			TCfLL_BMChamber[Index]	= _i_SCH_IO_GET_MODULE_STATUS( TCfLL_Chamber[Index] , 1 );
			//
			Result = EQUIP_PM_MOVE_CONTROL( TCfLL_Chamber[Index] , TCfLL_ArmASlot[Index] , TCfLL_ArmBSlot[Index] , TCfLL_Depth[Index] , TCfLL_SrcCass[Index] , TCfLL_BMChamber[Index] , 0 , 0 , 0 );
		}
		break;
	//===================================================================================================================================================

	}

	_i_SCH_IO_MTL_SAVE();
	//----------------------------------
	EnterCriticalSection( &CR_MAINT_INF_TRANSFER ); // 2006.08.30
	//----------------------------------
	//----------------------------------
	Transfer_Maint_Inf_Code_End_IO_Set( Index , Result );
	//----------------------------------
	if ( Transfer_Maint_Inf_Code_End( -1 ) ) {
		//----------------------------------
		_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 0 );
		//----------------------------------
		BUTTON_SET_TR_CONTROL( CTC_IDLE ); // 2003.10.28
	}
	//----------------------------------
	LeaveCriticalSection( &CR_MAINT_INF_TRANSFER ); // 2006.08.30
	//----------------------------------
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//============================================================================================================
void Transfer_Maint_Inf_Code_for_Init() {
	int i;
	for ( i = 0 ; i < ( MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+1 ) ; i++ ) {
		TCfLL_RUNNING[i] = FALSE;
	}
}
//============================================================================================================
