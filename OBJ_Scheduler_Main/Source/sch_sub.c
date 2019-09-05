#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "Dll_Interface.h"
#include "IOlog.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "Robot_Handling.h"
#include "FA_Handling.h"
#include "Equip_Handling.h"
#include "sch_prepost.h"
#include "sch_CassetteResult.h"
#include "sch_CassetteSupply.h"
#include "sch_prm.h"
#include "sch_preprcs.h"
#include "sch_main.h"
#include "sch_EIL.h"
#include "sch_sub.h"
#include "sch_multi_alic.h"
#include "sch_FMArm_multi.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_sdm.h"
#include "sch_util.h"
#include "sch_Cassmap.h"
#include "sch_OneSelect.h"
#include "sch_processmonitor.h"
#include "sch_restart.h"
#include "sch_CommonUser.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "Multireg.h"
#include "Multijob.h"
#include "Errorcode.h"
#include "Utility.h"

#include "INF_sch_CommonUser.h"
#include "INF_sch_macro.h"


//------------------------------------------------------------------------------------------
int         _in_HSide_Last_Pointer[MAX_CASSETTE_SIDE]; // 2018.11.22
//------------------------------------------------------------------------------------------
extern int  _in_HOT_LOT_CHECKING[MAX_CASSETTE_SIDE]; // 2011.06.13
//
extern int  _in_HOT_LOT_ORDERING_INVALID[MAX_CASSETTE_SIDE]; // 2011.06.13

extern int  _in_NOCHECK_RUNNING_TYPE[MAX_CASSETTE_SIDE]; // 2017.10.10
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _insdie_run_Tag[MAX_CASSETTE_SIDE]; // 2010.03.26
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// void SCHEDULER_CONTROL_Make_Clear_Method_Data( int side ); // 2012.04.25

void INIT_RUN_COMMON( int apmode , int side ); // 2008.09.26

void INIT_SCHEDULER_CONTROL_RECIPE_SETTING_DATA( int mode , int );
//
void INIT_SCHEDULER_MACRO_DATA( int mode , int );
//------------------------------------------------------------------------------------------
//void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch ); // 2010.01.26 // 2018.06.22
void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch , char *recipe , int mode ); // 2010.01.26 // 2018.06.22
//------------------------------------------------------------------------------------------
BOOL _MSG_READYEND_READY_ACTION_BEFORE_ENABLE( int ); // 2010.09.21
//------------------------------------------------------------------------------------------

BOOL _SCH_SUB_NOCHECK_RUNNING( int side ) { // 2017.10.10
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) return TRUE;
	if ( side < 0 || side >= MAX_CASSETTE_SIDE ) return TRUE;
	//
	return _in_NOCHECK_RUNNING_TYPE[ side ];
}

int SCHEDULER_GET_RECIPE_LOCKING( int side ) { // 2014.02.08
	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
		if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING_SKIP( side ) ) return 0;
	}
	//
	return _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING();
	//
}

/*
//
// 2019.03.04
//
int BUTTON_SET_FLOW_MTLOUT_STATUS( int side , int cm , BOOL check , int data ) { // 2013.06.08
	int i , m , Res;
	//
	m = _i_SCH_PRM_GET_MTLOUT_INTERFACE();
	//
	if ( cm == -1 ) {
		//
		if ( m <= 1 ) {
			BUTTON_SET_FLOW_STATUS( side , data );
			return 1;
		}
		//
		switch ( data ) {
		case _MS_16_CANCEL : // 2013.07.02
		case _MS_17_ABORTED :
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != ( side + CM1 ) ) continue;
				//
				BUTTON_SET_FLOW_STATUS( side , data );
				//
				return 1;
			}
			break;
		case _MS_7_START :
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_User_PathIn( side , i ) == ( side + CM1 ) ) {
					//
					BUTTON_SET_FLOW_STATUS( side , data );
					//
					return 1;
				}
				//
			}
			break;
		default :
			//
			if ( ( m == 3 ) && ( data == _MS_15_COMPLETE ) ) {
				if ( ( SYSTEM_IN_MODULE_GET(side) >= 0 ) || ( SYSTEM_OUT_MODULE_GET(side) >= 0 ) ) { // 2013.10.08
					return 0; // 2013.06.19
				}
				//
				Res = _MS_5_MAPPED; // 2013.10.08
				//
			}
			else {
				//
				Res = data; // 2013.10.08
				//
			}
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
				//
				if ( ( data >= _MS_14_HANDOFFOUT ) && ( data <= _MS_15_COMPLETE ) ) {
				}
				else {
					if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
				}
				//
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , i ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) continue;
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , i ) % 100 ) == _MOTAG_13_NEED_IN ) continue; // 2013.07.02
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != ( side + CM1 ) ) continue;
				//
//				BUTTON_SET_FLOW_STATUS( side , data ); // 2013.10.08
				BUTTON_SET_FLOW_STATUS( side , Res ); // 2013.10.08
				//
				return 1;
			}
			break;
		}
	}
	else {
		//
		//
		if ( m <= 1 ) {
			//
			if ( check ) {
				if ( _i_SCH_SYSTEM_USING_GET( cm ) <= 0 ) return 0;
			}
			//
			BUTTON_SET_FLOW_STATUS( cm , data );
			return 1;
		}
		//
		if ( _i_SCH_SYSTEM_USING_GET( cm ) <= 0 ) return 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( cm , i ) < 0 ) continue;
			//
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( cm , i ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) continue;
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( cm , i ) % 100 ) == _MOTAG_13_NEED_IN ) continue; // 2013.07.02
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( cm , i ) != ( cm + CM1 ) ) continue;
			//
			if ( m > 2 ) { // 2013.06.12
				BUTTON_SET_FLOW_STATUS( cm , data );
			}
			else {
				if ( _i_SCH_SYSTEM_USING_RUNNING( cm ) ) {
					if ( ( data == _MS_14_HANDOFFOUT ) || ( data == _MS_15_COMPLETE ) ) {
						BUTTON_SET_FLOW_STATUS( cm , _MS_12_RUNNING );
					}
					else {
						BUTTON_SET_FLOW_STATUS( cm , data );
					}
				}
				else {
					BUTTON_SET_FLOW_STATUS( cm , data );
				}
			}
			//
			return 1;
		}
		//
	}
	//
	return 0;
}
//
*/
//
//
// 2019.03.04
//
int BUTTON_SET_FLOW_MTLOUT_STATUS( int side , int cm , BOOL check , int data ) { // 2013.06.08
	int i , m , Res , cc , UseCM[MAX_CHAMBER];
	//
	m = _i_SCH_PRM_GET_MTLOUT_INTERFACE();
	//
	if ( cm == -1 ) {
		//
		if ( m <= 1 ) {
			BUTTON_SET_FLOW_STATUS( side , data );
			return 1;
		}
		//
		switch ( data ) {
		case _MS_16_CANCEL : // 2013.07.02
		case _MS_17_ABORTED :
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
				//
				if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != ( side + CM1 ) ) continue;
				//
				BUTTON_SET_FLOW_STATUS( side , data );
				//
				return 1;
			}
			break;
		case _MS_7_START :
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_User_PathIn( side , i ) == ( side + CM1 ) ) {
					//
					BUTTON_SET_FLOW_STATUS( side , data );
					//
					return 1;
				}
				//
			}
			break;
		default :
			//
			if ( ( m == 3 ) && ( data == _MS_15_COMPLETE ) ) {
				if ( ( SYSTEM_IN_MODULE_GET(side) >= 0 ) || ( SYSTEM_OUT_MODULE_GET(side) >= 0 ) ) { // 2013.10.08
					return 0; // 2013.06.19
				}
				//
				Res = _MS_5_MAPPED; // 2013.10.08
				//
			}
			else {
				//
				Res = data; // 2013.10.08
				//
			}
			//
			if ( m < 2 ) {
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
					//
					if ( ( data >= _MS_14_HANDOFFOUT ) && ( data <= _MS_15_COMPLETE ) ) {
					}
					else {
						if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
					}
					//
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , i ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) continue;
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , i ) % 100 ) == _MOTAG_13_NEED_IN ) continue; // 2013.07.02
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != ( side + CM1 ) ) continue;
					//
					BUTTON_SET_FLOW_STATUS( side , Res ); // 2013.10.08
					//
					return 1;
				}
			}
			else {
				//
				for ( i = CM1 ; i < PM1 ; i++ ) UseCM[i] = FALSE;
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
					//
					if ( ( data >= _MS_14_HANDOFFOUT ) && ( data <= _MS_15_COMPLETE ) ) {
					}
					else {
						if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
					}
					//
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , i ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) continue;
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , i ) % 100 ) == _MOTAG_13_NEED_IN ) continue; // 2013.07.02
					//
					cc = _i_SCH_CLUSTER_Get_PathIn( side , i );
					//
					if ( ( cc >= CM1 ) && ( cc < PM1 ) ) {
						//
						if ( !UseCM[cc] ) {
							UseCM[cc] = TRUE;
							BUTTON_SET_FLOW_STATUS( cc - CM1 , Res );
						}
						//
					}
					//
				}
			}
			//
			break;
		}
	}
	else {
		//
		//
		if ( m <= 1 ) {
			//
			if ( check ) {
				if ( _i_SCH_SYSTEM_USING_GET( cm ) <= 0 ) return 0;
			}
			//
			BUTTON_SET_FLOW_STATUS( cm , data );
			return 1;
		}
		//
		BUTTON_SET_FLOW_STATUS( cm , data );
		//
	}
	//
	return 0;
}
//

BOOL SCH_Inside_ThisIs_BM_OtherChamber( int ch , int mode ) { // 2009.01.21
	if ( ( ch >= BM1 ) && ( ch < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) { // 2009.09.22
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			if ( ch == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) return TRUE;
		}
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			if ( ch == _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) return TRUE;
		}
//	if ( ( ch >= BM1 ) && ( ch < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) { // 2009.09.22
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE ) return TRUE;
		}
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , ch ) == BUFFER_OTHER_STYLE2 ) return TRUE;
		}
	}
	return FALSE;
}

int SCH_Inside_ThisIs_BM_OtherChamberD( int ch , int mode ) { // 2009.09.23
	int i , l = 0;
	if ( SCH_Inside_ThisIs_BM_OtherChamber( ch , mode ) ) {
		for ( i = BM1 ; i < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ; i++ ) {
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2009.09.23
			//
			if ( ( mode == 0 ) || ( mode == 1 ) ) {
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , i ) == BUFFER_OTHER_STYLE ) l++;
			}
			if ( ( mode == 0 ) || ( mode == 2 ) ) {
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , i ) == BUFFER_OTHER_STYLE2 ) l++;
			}
		}
		//
		if ( l <= 1 ) return 0;
		//
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == 0 ) { // 2009.09.23
				return ( ch - BM1 + 1 );
			}
			else {
				if ( ch < _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2013.12.05
					return ( ch - BM1 + 11 );
				}
				else {
					return ( ch - _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() + 1 );
				}
			}
		}
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			if ( _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == 0 ) { // 2009.09.23
				return ( ch - BM1 + 1 );
			}
			else {
				if ( ch < _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() ) { // 2013.12.05
					return ( ch - BM1 + 11 );
				}
				else {
					return ( ch - _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() + 1 );
				}
			}
		}
		return 0;
	}
	else {
		return -1;
	}
}



int  SCH_Inside_ThisIs_DummyMethod( int ch , int mode , int wh , int s , int p ) { // 2017.09.11
	int GetIndex;
	//
	if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) && ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
		//
		GetIndex = -1;
		//
		if ( _SCH_COMMON_GET_DUMMY_METHOD_INDEX( ch , mode , wh , s , p , &GetIndex ) ) return GetIndex;
		//
	}
	//
	return SCH_Inside_ThisIs_BM_OtherChamberD( ch , mode );
	//
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( int tms , int side , BOOL EndTh ) {
	if ( tms != 0 ) _i_SCH_SYSTEM_TM_SET( tms , side , -1 );
	LeaveCriticalSection( &CR_TMRUN[tms] );
	if ( EndTh ) _endthread();
	return SYS_ABORTED;
}
//------------------------------------------------------------------------------------------
int _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( int tms , int side , BOOL EndTh ) {
	if ( tms != 0 ) _i_SCH_SYSTEM_TM_SET( tms , side , 0 );
	LeaveCriticalSection( &CR_TMRUN[tms] );
	if ( EndTh ) _endthread();
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
void _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( int side , BOOL EndTh ) {
	_i_SCH_MODULE_Set_FM_Runinig_Flag( side , -1 );
	_i_SCH_SYSTEM_USING2_SET( side , 0 );
	LeaveCriticalSection( &CR_FEMRUN );
	if ( EndTh ) _endthread();
}

void _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( int side , BOOL EndTh ) {
	_i_SCH_MODULE_Set_FM_Runinig_Flag( side , 0 );
	_i_SCH_SYSTEM_USING2_SET( side , 0 );
	LeaveCriticalSection( &CR_FEMRUN );
	if ( EndTh ) _endthread();
}

int _i_SCH_SUB_PRECHECK_WAITING_FOR_THREAD_TM( int side , int tmside ) {
	_i_SCH_SYSTEM_TM_SET( tmside , side , 2 );
	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) return SYS_RUNNING;
	while( TRUE ) {
		if ( _i_SCH_SYSTEM_USING2_GET( side ) >= 2 ) break;
		if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( side ) < 0 ) {
			_i_SCH_SYSTEM_TM_SET( tmside , side , -1 );
			_endthread();
			return SYS_ABORTED;
		}
		if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( side ) == 0 ) {
			_i_SCH_SYSTEM_TM_SET( tmside , side , 0 );
			_endthread();
			return SYS_SUCCESS;
		}
		Sleep(1);
	}
	return SYS_RUNNING;
}


//================================================================================================================
//================================================================================================================
//================================================================================================================
//================================================================================================================
//=======================================================================================
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Check Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL _i_SCH_SUB_Check_Complete_Return( int side ) { // 2004.12.03
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) { // 2004.12.06
			if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_RETURN_REQUEST ) return FALSE;
			if ( ( _i_SCH_CLUSTER_Get_PathRun( side , i , 13 , 2 ) >= 5 ) && ( _i_SCH_CLUSTER_Get_PathRun( side , i , 13 , 2 ) <= 8 ) ) return FALSE; // 2008.10.31
		}
	}
	return TRUE;
}
//-------------------------------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Make_Side_Monitor_Cluster_Count( int side ) { // 2009.04.02
	int k , c = 0;
	for ( k = PM1 ; k < PM1 + MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , k ) == MUF_01_USE ) {
			if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( k ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( k ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) {
				c++;
			}
		}
	}
	return c;
}


BOOL SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable_Sub( int Chamber , int HW , int Check , int abflag , int multipm ) {
	int s , i , j , k , l , m , parl , find , c , p , p2 , Chamber2;
//	int z0 , zx;
	int MC[MAX_CASSETTE_SIDE]; // 2004.05.17
	BOOL ok = TRUE;
	BOOL buffering; // 2008.05.28
	int rck; // 2011.09.10
	int repoint; // 2012.04.15
	//------------------------------------
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_DISAB-01 \t[Chamber=%d][HW=%d][Check=%d][abflag=%d][multipm=%d]\n" , Chamber , HW , Check , abflag , multipm );
//------------------------------------------------------------------------------------------------------------------
	if ( Chamber < 0 ) {
		for ( i = CM1 ; i <= F_IC ; i++ ) {
			if ( abflag >= 0 ) {
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( i , abflag );
			}
			if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
				if ( HW == 1 )
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( i , 2 );
				else
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( i , 0 );
				//
				FA_MDL_STATUS_IO_SET( i ); // 2007.07.23
			}
		}
		return TRUE;
	}
	//===========================================================================
	if ( ( Chamber == AL ) || ( Chamber == IC ) || ( ( Chamber >= TM ) && ( Chamber <= FM ) ) ) { // 2011.09.07
		if ( ( Chamber >= TM ) && ( Chamber <= FM ) ) {
			//
			if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
				if ( HW == 1 )
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 2 );
				else
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 0 );
				//
				FA_MDL_STATUS_IO_SET( Chamber );
			}
			//
		}
		//
		return TRUE;
	}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_DISAB-02 \t[Chamber=%d][HW=%d][Check=%d][abflag=%d][multipm=%d]\n" , Chamber , HW , Check , abflag , multipm );
//------------------------------------------------------------------------------------------------------------------
	//===========================================================================
	// 2007.03.22
	//===========================================================================
	if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
//		if ( _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL() ) { // 2008.10.15
		if ( multipm || _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL() ) { // 2008.10.15
			//===========================================================================
			if ( ( ( Chamber - PM1 ) % 2 ) == 0 )
				Chamber2 = Chamber + 1;
			else
				Chamber2 = Chamber - 1;
			//===========================================================================
		}
		else {
			Chamber2 = -99999;
		}
	}
	else {
		Chamber2 = -99999;
	}
	//===========================================================================
	if ( _i_SCH_SYSTEM_USING_ANOTHER_RUNNING( -1 ) ) { // 2011.07.15
		//
		if ( ( !HW ) && ( abflag != ABORTWAIT_FLAG_WAIT ) && ( abflag != ABORTWAIT_FLAG_ABORTWAIT ) ) {
			//
			if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
				//
				for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
					//------------------------------------
					if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , j ) <= 0 ) continue;
					//------------------------------------
					s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , j );
					p = _i_SCH_MODULE_Get_PM_POINTER( Chamber , j );
					//
					if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return FALSE;
					if ( p < 0 ) return FALSE;
					//
				}
				//
			}
			//
			if ( ( Chamber2 >= PM1 ) && ( Chamber2 < AL ) ) {
				//
				for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
					//------------------------------------
					if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber2 , j ) <= 0 ) continue;
					//------------------------------------
					s = _i_SCH_MODULE_Get_PM_SIDE( Chamber2 , j );
					p = _i_SCH_MODULE_Get_PM_POINTER( Chamber2 , j );
					//
					if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return FALSE;
					if ( p < 0 ) return FALSE;
					//
				}
				//
			}
			//
		}
	}
	//===========================================================================
	//------------------------------------
//	if ( abflag >= 0 ) { // 2006.02.03
	if ( ( abflag >= ABORTWAIT_FLAG_NONE ) && ( abflag < ABORTWAIT_FLAG_MODULE_SET_ONLY ) ) { // 2006.02.03
		SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( Chamber , abflag );
		//===========================================================================
		// 2007.03.22
		//===========================================================================
		if ( Chamber2 != -99999 ) SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( Chamber2 , abflag );
		//===========================================================================
	}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_DISAB-03 \t[Chamber=%d][HW=%d][Check=%d][abflag=%d][multipm=%d]\n" , Chamber , HW , Check , abflag , multipm );
//------------------------------------------------------------------------------------------------------------------
	//------------------------------------
	if ( HW < 0 ) return TRUE;
	//------------------------------------
	if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
		if ( HW == 1 ) {
			_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 2 );
			if ( Chamber2 != -99999 ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber2 , 2 );
		}
		else {
			_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 0 );
			if ( Chamber2 != -99999 ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber2 , 0 );
		}
		FA_MDL_STATUS_IO_SET( Chamber ); // 2007.07.23
		//===========================================================================
		// 2007.03.22
		//===========================================================================
		if ( Chamber2 != -99999 ) FA_MDL_STATUS_IO_SET( Chamber2 ); // 2007.07.23
		//===========================================================================
	}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_DISAB-04 \t[Chamber=%d][HW=%d][Check=%d][abflag=%d][multipm=%d]\n" , Chamber , HW , Check , abflag , multipm );
//------------------------------------------------------------------------------------------------------------------
	//------------------------------------
	if ( abflag == ABORTWAIT_FLAG_MODULE_SET_ONLY ) return TRUE; // 2006.02.03
	//------------------------------------
	if ( ( Chamber >= BM1 ) && ( ( Chamber - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) ) return TRUE; // 2004.08.14
	//------------------------------------
	//------------------------------------
	if ( ( Chamber == AL ) || ( Chamber == IC ) || ( Chamber == F_AL ) || ( Chamber == F_IC ) ) return TRUE;
	//------------------------------------
	if ( ( HW == 1 ) && ( Check == 4 ) ) { // 2003.12.10 Not Use
		/*
		// 2014.01.10
		if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , 0 ) > 0 ) {
			s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , 0 );
			_i_SCH_MODULE_Set_PM_WAFER( Chamber , 0 , 0 );
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
				_i_SCH_MODULE_Inc_FM_InCount( s );
			}
			//
			_i_SCH_MODULE_Inc_TM_InCount( s );
			SCHEDULER_CONTROL_Set_Disable_Disappear( s , TRUE );
		}
		//===========================================================================
		// 2007.03.22
		//===========================================================================
		if ( Chamber2 != -99999 ) {
			if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber2 , 0 ) > 0 ) {
				s = _i_SCH_MODULE_Get_PM_SIDE( Chamber2 , 0 );
				_i_SCH_MODULE_Set_PM_WAFER( Chamber2 , 0 , 0 );
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					_i_SCH_MODULE_Inc_FM_InCount( s );
				}
				//
				_i_SCH_MODULE_Inc_TM_InCount( s );
				SCHEDULER_CONTROL_Set_Disable_Disappear( s , TRUE );
			}
		}
		*/
		//===========================================================================
		// 2014.01.10
		//
		for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
			//
			if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , j ) > 0 ) {
				s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , j );
				_i_SCH_MODULE_Set_PM_WAFER( Chamber , j , 0 );
				//
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					_i_SCH_MODULE_Inc_FM_InCount( s );
				}
				//
				_i_SCH_MODULE_Inc_TM_InCount( s );
				SCHEDULER_CONTROL_Set_Disable_Disappear( s , TRUE );
			}
			if ( Chamber2 != -99999 ) {
				if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber2 , j ) > 0 ) {
					s = _i_SCH_MODULE_Get_PM_SIDE( Chamber2 , j );
					_i_SCH_MODULE_Set_PM_WAFER( Chamber2 , j , 0 );
					//
					if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
						_i_SCH_MODULE_Inc_FM_InCount( s );
					}
					//
					_i_SCH_MODULE_Inc_TM_InCount( s );
					SCHEDULER_CONTROL_Set_Disable_Disappear( s , TRUE );
				}
			}
		}
		//
	}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_DISAB-06 \t[Chamber=%d][HW=%d][Check=%d][abflag=%d][multipm=%d]\n" , Chamber , HW , Check , abflag , multipm );
//------------------------------------------------------------------------------------------------------------------
	//------------------------------------
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) { // 2003.08.08
		//--------------------------------------
		MC[s] = TRUE; // 2004.05.17
		//--------------------------------------
		if ( _i_SCH_SYSTEM_USING_GET( s ) <= 0 ) continue;
		//-------------------------------
//		c = 1; // 2003.10.10 // 2009.04.02
		c = 0; // 2003.10.10 // 2009.04.02
		//-------------------------------
//		z0 = 0; // 2007.09.07
//		zx = 0; // 2007.09.07
		//-------------------------------
		repoint = FALSE; // 2012.04.15
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			rck = 0; // 2011.09.10
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) >= 0 ) {
				switch( _i_SCH_CLUSTER_Get_PathStatus( s , i ) ) {
				case SCHEDULER_READY	:
					//
					if ( _i_SCH_CLUSTER_Get_Ex_DisableSkip( s , i ) ) repoint = TRUE; // 2012.04.14
					//
					l = 0;
					break;

				case SCHEDULER_SUPPLY	: l = 0;		break;
				case SCHEDULER_STARTING	: l = 0;		break;
//				case SCHEDULER_RUNNING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2019.01.22
				case SCHEDULER_RUNNING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2019.01.22
//				case SCHEDULER_RUNNINGW	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2019.01.22
				case SCHEDULER_RUNNINGW	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2019.01.22
//				case SCHEDULER_RUNNING2	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2019.01.22
				case SCHEDULER_RUNNING2	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2019.01.22
//				case SCHEDULER_WAITING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2003.10.09
				case SCHEDULER_WAITING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2003.10.09
//				case SCHEDULER_BM_END	: l = 99999;			break; // 2011.09.10
				case SCHEDULER_BM_END	: l = 0;	rck = 1;	break; // 2011.09.10
//				case SCHEDULER_CM_END	: l = 99999;			break;
//				case SCHEDULER_CM_END	: l = 0;	rck = 1;	break; // 2011.09.10 // 2013.04.11
//				case SCHEDULER_CM_END	: l = 0;	rck = 2;	break; // 2011.09.10 // 2013.04.11 // 2019.01.22
				case SCHEDULER_CM_END	: l = 99999;	break; // 2011.09.10 // 2013.04.11 // 2019.01.22
//				case SCHEDULER_RETURN_REQUEST	: l = 99999;	break; // 2007.05.15
//				case SCHEDULER_RETURN_REQUEST	: l = 0;	rck = 1;	break; // 2007.05.15 // 2019.01.22
				case SCHEDULER_RETURN_REQUEST	: l = 0;	rck = 3;	break; // 2007.05.15 // 2019.01.22
				}

				if ( l < 0 ) l = 0; // 2003.10.09
				//
//if ( i == 0 ) printf( "[s=%d][p=%d][Start][l=%d][ps=%d][PM%d]\n" , s , i , l , _i_SCH_CLUSTER_Get_PathStatus( s , i ) , Chamber - PM1 + 1 );

				if ( _i_SCH_CLUSTER_Get_PathDo( s , i ) == 0 ) { // 2010.04.20
					if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_STARTING ) {

//if ( i == 0 ) printf( "[s=%d][p=%d][Curr_DisEna][PM%d]\n" , s , i , Chamber - PM1 + 1 );

						_SCH_CLUSTER_Parallel_Check_Curr_DisEna( s , i , Chamber , TRUE );
					}
				}
				//
				for ( j = l ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) {
					//
					find = FALSE;
					parl = FALSE;
					//-------------------------------
					p = 0; // 2003.10.10
					//-------------------------------
					p2 = 0; // 2006.06.29
					//-------------------------------
					// 2008.05.28
					//-------------------------------
					if ( j == 0 ) {
						if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) < MAX_CLUSTER_DEPTH ) { // 2010.04.10
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								if ( _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , MAX_CLUSTER_DEPTH - 1 , k ) > 0 ) break;
							}
							if ( k == MAX_PM_CHAMBER_DEPTH ) buffering = FALSE;
							else							 buffering = TRUE;
						}
						else { // 2010.04.10
							buffering = FALSE;
						}
					}
					else {
						buffering = FALSE;
					}
					//
					//-------------------------------
					//
					// 2019.01.22
					//
					if ( rck != 3 ) {
						if ( _i_SCH_CLUSTER_Get_PathRun( s , i , j , 0 ) != 0 ) continue;
					}
					//
					//
					//-------------------------------
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
						//-------------------------------------------------------------------------------
						// 2008.05.28
						//-------------------------------------------------------------------------------
						//
						if ( rck == 1 ) { // 2011.09.10
							if ( m > 0 ) {
								if ( _i_SCH_CLUSTER_Get_PathRun( s , i , j , 0 ) == m ) {
									continue;
								}
							}
						}
//						if ( rck == 2 ) continue; // 2013.04.11 // 2019.01.22
						//
						if ( buffering && ( m == 0 ) ) {
							if ( _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , MAX_CLUSTER_DEPTH - 1 , k ) > 0 ) {
								m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , MAX_CLUSTER_DEPTH - 1 , k );
								if ( m > 0 ) {
									if ( !_i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) m = 0;
								}
							}
						}
						//-------------------------------------------------------------------------------
//						if ( m == Chamber ) { // 2006.11.15
						if ( ( m == Chamber ) || ( m == Chamber2 ) ) { // 2006.11.15
							if ( ( Check == 2 ) || ( Check == 4 ) ) { // 2003.08.13
//if ( i == 0 ) printf( "[s=%d][p=%d][Delete][l=%d][j=%d][k=%d][PM%d]\n" , s , i , l , j , k , Chamber - PM1 + 1 );
								_i_SCH_CLUSTER_Set_PathProcessData( s , i , j , k , 0 , "" , "" , "" );
							}
							else {
//if ( i == 0 ) printf( "[s=%d][p=%d][Disable][l=%d][j=%d][k=%d][PM%d]\n" , s , i , l , j , k , Chamber - PM1 + 1 );
								_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , i , j , k );
							}
							find = TRUE;
						}
//						else if ( ( m != Chamber ) && ( m > 0 ) ) { // 2006.11.15
						else if ( ( m != Chamber ) && ( m != Chamber2 ) && ( m > 0 ) ) { // 2006.11.15
//if ( i == 0 ) printf( "[s=%d][p=%d][skip][l=%d][j=%d][k=%d][PM%d]\n" , s , i , l , j , k , Chamber - PM1 + 1 );
							if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
								//-------------------------------
								p++; // 2003.10.10
								//-------------------------------
							}
							else {
								p2++; // 2006.06.29
							}
							parl = TRUE;
						}
					}
					if ( find && !parl ) {
						ok = FALSE;
						//--------------------------------------
						MC[s] = FALSE; // 2004.05.17
						//--------------------------------------
					}
					//-------------------------------
//					if ( p <= 0 ) { // 2006.06.29
					if ( ( p <= 0 ) && ( p2 <= 0 ) ) { // 2006.06.29
//						c = 0; // 2007.09.07 // 2009.04.03
//						z0++; // 2007.09.07
					}
					else {
//						zx++; // 2007.09.07
//						if ( ( p > c ) && ( c != 0 ) ) c = p; // 2003.10.10 // 2009.04.02
						if ( p > c ) c = p; // 2003.10.10 // 2009.04.02
					}
					//-------------------------------
				}
				//
			}
		}
		//
		if ( repoint ) { // 2012.04.15
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				_i_SCH_MODULE_Set_TM_DoPointer( s , 0 );
			}
			else {
				_i_SCH_MODULE_Set_FM_DoPointer_FDHC( s , 0 );
			}
			//
		}
		//-------------------------------
		// 2007.09.07
		//-------------------------------
//		if ( ( z0 > 0 ) && ( z0 > zx ) ) c = 0;
		//-------------------------------
//_IO_CIM_PRINTF( "=======================================\n" );
//_IO_CIM_PRINTF( "=[D]====================[%d]===========\n" , c );
//_IO_CIM_PRINTF( "=======================================\n" );
		SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( s , c );	// 2003.10.10
		//-------------------------------
	}
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "TEST_DISAB-10 \t[Chamber=%d][HW=%d][Check=%d][abflag=%d][multipm=%d][ok=%d]\n" , Chamber , HW , Check , abflag , multipm , ok );
//------------------------------------------------------------------------------------------------------------------

	/*
	//
	// 2017.12.07
	//
	if ( Check != 0 ) {
		if ( ( Check == 3 ) || ( Check == 4 ) ) {
			if ( !ok ) {
				ERROR_HANDLER( ERROR_DISABLE_NOT_GO_MODULE , "" );
				//-----------------------------------------------------------------------
				// 2004.05.14
				//-----------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) {
					for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
						if ( !MC[s] ) {
							//-------------------------------
							_i_SCH_IO_SET_END_BUTTON( s , CTCE_ENDING );
							//-------------------------------
							PROCESS_MONITOR_STATUS_Abort_Signal_Set( s , FALSE );
							_i_SCH_SYSTEM_MODE_END_SET( s , 4 );
						}
					}
				}
				//-----------------------------------------------------------------------
			}
		}
		return ok;
	}
	else {
		if ( !ok ) {
			ERROR_HANDLER( ERROR_DISABLE_NOT_GO_MODULE , "" );
			//-----------------------------------------------------------------------
			// 2004.05.14
			//-----------------------------------------------------------------------
			if ( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) {
				for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
					if ( !MC[s] ) {
						//-------------------------------
						_i_SCH_IO_SET_END_BUTTON( s , CTCE_ENDING );
						//-------------------------------
						PROCESS_MONITOR_STATUS_Abort_Signal_Set( s , FALSE );
						_i_SCH_SYSTEM_MODE_END_SET( s , 4 );
					}
				}
			}
			//-----------------------------------------------------------------------
		}
		return TRUE;
	}
	//
	*/
	//

	//
	// 2017.12.07
	//
	if ( Check != 0 ) {
		if ( ( Check == 3 ) || ( Check == 4 ) ) {
			if ( !ok ) {
				//
				switch( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) {
				case 0 :
				case 1 :
					ERROR_HANDLER( ERROR_DISABLE_NOT_GO_MODULE , "" );
					break;
				}
				//
				//-----------------------------------------------------------------------
				// 2004.05.14
				//-----------------------------------------------------------------------
				switch( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) {
				case 1 :
				case 3 :
					for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
						if ( !MC[s] ) {
							//-------------------------------
							_i_SCH_IO_SET_END_BUTTON( s , CTCE_ENDING );
							//-------------------------------
							PROCESS_MONITOR_STATUS_Abort_Signal_Set( s , FALSE );
							_i_SCH_SYSTEM_MODE_END_SET( s , 4 );
						}
					}
					break;
				}
				//-----------------------------------------------------------------------
			}
		}
		return ok;
	}
	else {
		if ( !ok ) {
			//
			switch( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) {
			case 0 :
			case 1 :
				ERROR_HANDLER( ERROR_DISABLE_NOT_GO_MODULE , "" );
				break;
			}
			//
			//-----------------------------------------------------------------------
			// 2004.05.14
			//-----------------------------------------------------------------------
			switch( _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) {
			case 1 :
			case 3 :
				for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
					if ( !MC[s] ) {
						//-------------------------------
						_i_SCH_IO_SET_END_BUTTON( s , CTCE_ENDING );
						//-------------------------------
						PROCESS_MONITOR_STATUS_Abort_Signal_Set( s , FALSE );
						_i_SCH_SYSTEM_MODE_END_SET( s , 4 );
					}
				}
				break;
			}
			//-----------------------------------------------------------------------
		}
		return TRUE;
	}
	//
}
//=======================================================================================
void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable_Sub( int Chamber , BOOL PMMode , int NextEnable , int abflag , int multipm ) { // 2003.05.26
	int s , i , j , k , m , y , c , p , p2 , Chamber2 , l;
	BOOL EnableReadyOnly = FALSE;
	BOOL NoMore; // 2018.12.10
	int rck; // 2019.01.22
	//------------------------------------
	if ( NextEnable == 0 ) { // 2015.04.03
		switch( _i_SCH_SUB_Get_ENABLE_TO_ENABLEN() ) {
		case 1 :
			NextEnable = 1;
			break;
		case 2 :
			NextEnable = 2;
			break;
		case 3 :
			EnableReadyOnly = TRUE;
			break;
		}
	}
	//------------------------------------
	if ( Chamber < 0 ) {
		for ( i = CM1 ; i <= F_IC ; i++ ) {
//			if ( abflag >= 0 ) { // 2006.02.03
			if ( ( abflag >= 0 ) && ( abflag < ABORTWAIT_FLAG_MODULE_SET_ONLY ) ) { // 2006.07.10
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( i , abflag );
			}
			if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
				if ( PMMode ) {
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( i , 3 );
				}
				else {
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( i , 1 );
				}
				FA_MDL_STATUS_IO_SET( i ); // 2007.07.23
			}
		}
		//
		EIL_CHECK_ENABLE_CONTROL( -1 , PMMode , NextEnable ); // 2011.07.25
		//
		return;
	}
	//===========================================================================
	if ( ( Chamber == AL ) || ( Chamber == IC ) || ( ( Chamber >= TM ) && ( Chamber <= FM ) ) ) { // 2011.09.07
		if ( ( Chamber >= TM ) && ( Chamber <= FM ) ) {
			//
			if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
				if ( PMMode ) {
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 3 );
				}
				else {
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 1 );
				}
				FA_MDL_STATUS_IO_SET( Chamber );
			}
			//
		}
		//
		return;
	}
	//===========================================================================
	// 2007.03.22
	//===========================================================================
	if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
//		if ( _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL() ) { // 2008.10.15
		if ( multipm || _SCH_COMMON_PM_MULTI_ENABLE_DISABLE_CONTROL() ) { // 2008.10.15
			//===========================================================================
			if ( ( ( Chamber - PM1 ) % 2 ) == 0 )
				Chamber2 = Chamber + 1;
			else
				Chamber2 = Chamber - 1;
			//===========================================================================
		}
		else {
			Chamber2 = -99999;
		}
	}
	else {
		Chamber2 = -99999;
	}
	//===========================================================================
	if ( _i_SCH_SYSTEM_USING_ANOTHER_RUNNING( -1 ) ) { // 2011.07.15
		//
		if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
			//
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				//------------------------------------
				if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , j ) <= 0 ) continue;
				//------------------------------------
				s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , j );
				p = _i_SCH_MODULE_Get_PM_POINTER( Chamber , j );
				//
				if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return;
				if ( p < 0 ) return;
				//
			}
			//
		}
		//
		if ( ( Chamber2 >= PM1 ) && ( Chamber2 < AL ) ) {
			//
			for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
				//------------------------------------
				if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber2 , j ) <= 0 ) continue;
				//------------------------------------
				s = _i_SCH_MODULE_Get_PM_SIDE( Chamber2 , j );
				p = _i_SCH_MODULE_Get_PM_POINTER( Chamber2 , j );
				//
				if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) return;
				if ( p < 0 ) return;
				//
			}
			//
		}
		//
	}
	//===========================================================================
	//------------------------------------
//	if ( abflag >= 0 ) { // 2006.02.03
	if ( ( abflag >= 0 ) && ( abflag < ABORTWAIT_FLAG_MODULE_SET_ONLY ) ) { // 2006.02.03
		SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( Chamber , abflag );
		//===========================================================================
		// 2007.03.22
		//===========================================================================
		if ( Chamber2 != -99999 ) SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( Chamber2 , abflag );
		//===========================================================================
	}
	else if ( abflag == ABORTWAIT_FLAG_MODULE_NEXT_SKIP ) { // 2015.03.26
		SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( Chamber , ABORTWAIT_FLAG_NONE );
		if ( Chamber2 != -99999 ) SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( Chamber2 , ABORTWAIT_FLAG_NONE );
	}
	//------------------------------------
	if ( abflag != ABORTWAIT_FLAG_MODULE_SET_ONLY ) { // 2006.02.03
		if ( ( abflag >= 0 ) && ( abflag < ABORTWAIT_FLAG_MODULE_SET_ONLY ) ) { // 2006.07.10
			for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
				if ( _i_SCH_SYSTEM_USING_GET( s ) <= 0 ) continue;
				//============================================================================
				if ( PMMode && !_i_SCH_SYSTEM_PMMODE_GET( s ) ) continue; // 2005.07.18
				if ( !PMMode && _i_SCH_SYSTEM_PMMODE_GET( s ) ) continue; // 2005.07.18
				//============================================================================
				switch( PROCESS_MONITOR_Get_Status( s , Chamber ) ) {
				case -1 :
					break;
				case -2 :
					break;
				case -3 :
					PROCESS_MONITOR_Set_Finish_Status( s , Chamber , -2 );
					break;
				case -4 :
					break;
				case -5 :
					PROCESS_MONITOR_Set_Finish_Status( s , Chamber , -4 );
					break;
				}
				//===========================================================================
				// 2007.03.22
				//===========================================================================
				if ( Chamber2 != -99999 ) {
					switch( PROCESS_MONITOR_Get_Status( s , Chamber2 ) ) {
					case -1 :
						break;
					case -2 :
						break;
					case -3 :
						PROCESS_MONITOR_Set_Finish_Status( s , Chamber2 , -2 );
						break;
					case -4 :
						break;
					case -5 :
						PROCESS_MONITOR_Set_Finish_Status( s , Chamber2 , -4 );
						break;
					}
				}
				//===========================================================================
			}
		}
		else if ( abflag == ABORTWAIT_FLAG_MODULE_NEXT_SKIP ) { // 2006.07.10
			//==========================================================
			for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
				if ( _i_SCH_SYSTEM_USING_GET( s ) <= 0 ) continue;
				//============================================================================
				if ( PMMode && !_i_SCH_SYSTEM_PMMODE_GET( s ) ) continue;
				if ( !PMMode && _i_SCH_SYSTEM_PMMODE_GET( s ) ) continue;
				//============================================================================
//				if ( PROCESS_MONITOR_Get_Status( s , Chamber ) < 0 ) { // 2006.07.20
				if ( PROCESS_MONITOR_Get_Status( s , Chamber ) <= 0 ) { // 2006.07.20
					PROCESS_MONITOR_Set_Finish_Status( s , Chamber , -4 );
				}
				//============================================================================
				//===========================================================================
				// 2007.03.22
				//===========================================================================
				if ( Chamber2 != -99999 ) {
					if ( PROCESS_MONITOR_Get_Status( s , Chamber2 ) <= 0 ) { // 2006.07.20
						PROCESS_MONITOR_Set_Finish_Status( s , Chamber2 , -4 );
					}
				}
				//===========================================================================
			}
			//==========================================================
			// 2007.03.12
			//==========================================================
			SCHEDULER_CONTROL_PM_Next_Place_Impossible( Chamber );
			//===========================================================================
			// 2007.03.22
			//===========================================================================
			if ( Chamber2 != -99999 ) {
				SCHEDULER_CONTROL_PM_Next_Place_Impossible( Chamber2 );
			}
			//===========================================================================
		}
	}
	//------------------------------------
	//
	// 2018.12.10
	//
	/*
	if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
		if ( PMMode ) {
			_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 3 );
			if ( Chamber2 != -99999 ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber2 , 3 );
			//
		}
		else {
			_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 1 );
			if ( Chamber2 != -99999 ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber2 , 1 );
			//
		}
		//
		EIL_CHECK_ENABLE_CONTROL( Chamber , PMMode , NextEnable ); // 2011.07.25
		if ( Chamber2 != -99999 ) EIL_CHECK_ENABLE_CONTROL( Chamber2 , PMMode , NextEnable ); // 2011.07.25
		//
		FA_MDL_STATUS_IO_SET( Chamber ); // 2007.07.23
		//===========================================================================
		// 2007.03.22
		//===========================================================================
		if ( Chamber2 != -99999 ) FA_MDL_STATUS_IO_SET( Chamber2 ); // 2007.07.23
		//===========================================================================
	}
	*/
	//
	//
	//
	//
	NoMore = FALSE; // 2018.12.10
	//
	//------------------------------------
//	if ( abflag == ABORTWAIT_FLAG_MODULE_SET_ONLY ) return; // 2006.02.03 // 2018.12.10
	if ( abflag == ABORTWAIT_FLAG_MODULE_SET_ONLY ) NoMore = TRUE; // 2006.02.03 // 2018.12.10
	//------------------------------------
//	if ( NextEnable == 1 ) return; // 2003.07.21 // 2018.12.10
	if ( NextEnable == 1 ) NoMore = TRUE; // 2003.07.21 // 2018.12.10
	//------------------------------------
	//------------------------------------
//	if ( ( Chamber >= BM1 ) && ( ( Chamber - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) ) return; // 2004.08.14 // 2018.12.10
	if ( ( Chamber >= BM1 ) && ( ( Chamber - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) ) NoMore = TRUE; // 2004.08.14 // 2018.12.10
	//------------------------------------
	//------------------------------------
//	if ( ( Chamber == AL ) || ( Chamber == IC ) || ( Chamber == F_AL ) || ( Chamber == F_IC ) ) return; // 2018.12.10
	if ( ( Chamber == AL ) || ( Chamber == IC ) || ( Chamber == F_AL ) || ( Chamber == F_IC ) ) NoMore = TRUE; // 2018.12.10
	//------------------------------------
	//------------------------------------
	// 2007.08.13
	//------------------------------------
//	if ( NextEnable == 2 ) { // 2018.12.10
	if ( !NoMore && ( NextEnable == 2 ) ) { // 2018.12.10
		//================================================================================
//		EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
		EnterCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
		//================================================================================
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//-----------------------
//			if ( _i_SCH_SYSTEM_USING_GET( s ) <= 0 ) continue; // 2010.04.23
			//-----------------------
//			if ( ( _i_SCH_SYSTEM_USING_GET( s ) < 6 ) || ( SYSTEM_BEGIN_GET( s ) == 0 ) ) continue; // 2010.04.23 // 2015.07.30
			if ( ( _i_SCH_SYSTEM_USING_GET( s ) < 6 ) || ( SYSTEM_BEGIN_GET( s ) <= 1 ) ) continue; // 2010.04.23 // 2015.07.30
			//-----------------------
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) {
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
							if ( ( -(m) == Chamber ) || ( m == Chamber ) ) {
								_i_SCH_CLUSTER_Set_PathProcessData( s , i , j , k , 0 , "" , "" , "" );
							}
						}
					}
				}
				//=========================================================================
				_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_00_NOTUSE );
				_i_SCH_MODULE_Dec_Mdl_Run_Flag( Chamber );
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) < 0 ) {
					_i_SCH_LOG_LOT_PRINTF( s , "Module %s Run Flag Status Error 11%cMDLSTSERROR 11:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber );
				}
				//=========================================================================
			}
			//-------------------------------
			if ( Chamber2 != -99999 ) {
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber2 ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber2 ) >= MUF_90_USE_to_XDEC_FROM ) ) {
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) {
							for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
								m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
								if ( ( -(m) == Chamber2 ) || ( m == Chamber2 ) ) {
									_i_SCH_CLUSTER_Set_PathProcessData( s , i , j , k , 0 , "" , "" , "" );
								}
							}
						}
					}
					//=========================================================================
					_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber2 , MUF_00_NOTUSE );
					_i_SCH_MODULE_Dec_Mdl_Run_Flag( Chamber2 );
					//=========================================================================
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber2 ) < 0 ) {
						_i_SCH_LOG_LOT_PRINTF( s , "Module %s Run Flag Status Error 12%cMDLSTSERROR 12:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber2 ) , 9 , Chamber2 );
					}
					//=========================================================================
				}
			}
			//-------------------------------
		}
		//================================================================================
//		LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2010.04.23 // 2010.05.21
		LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.04.23 // 2010.05.21
		//================================================================================
		//
//		return; // 2018.12.10
		NoMore = TRUE; // 2018.12.10
		//
	}
	//------------------------------------
	if ( !NoMore ) { // 2018.12.10
		//
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			if ( _i_SCH_SYSTEM_USING_GET( s ) <= 0 ) continue;
			//============================================================================
			if ( PMMode && !_i_SCH_SYSTEM_PMMODE_GET( s ) ) continue; // 2005.07.18
			if ( !PMMode && _i_SCH_SYSTEM_PMMODE_GET( s ) ) continue; // 2005.07.18
			//============================================================================
			y = 0;
			//-----------------------
	//		c = 1; // 2003.10.10 // 2009.04.02
			c = 0; // 2003.10.10 // 2009.04.02
			//-----------------------
	//		q = _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( s );	// 2003.10.21 // 2009.04.02
			//-----------------------
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) < 0 ) continue; // 2009.04.02
				//
				rck = 0; // 2019.01.22
				//
				switch( _i_SCH_CLUSTER_Get_PathStatus( s , i ) ) { // 2009.04.02
				case SCHEDULER_READY	: l = 0;		break;
				case SCHEDULER_SUPPLY	: l = 0;		break;
				case SCHEDULER_STARTING	: l = 0;		break;
//				case SCHEDULER_RUNNING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2019.01.22
				case SCHEDULER_RUNNING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2019.01.22
//				case SCHEDULER_RUNNINGW	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2019.01.22
				case SCHEDULER_RUNNINGW	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2019.01.22
//				case SCHEDULER_RUNNING2	: l = _i_SCH_CLUSTER_Get_PathDo( s , i );		break; // 2019.01.22
				case SCHEDULER_RUNNING2	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break; // 2019.01.22
				case SCHEDULER_WAITING	: l = _i_SCH_CLUSTER_Get_PathDo( s , i ) - 1;	break;
				case SCHEDULER_BM_END	: l = 99999;		break;
				case SCHEDULER_CM_END	: l = 99999;		break;
//				case SCHEDULER_RETURN_REQUEST	: l = 0;		break; // 2019.01.22
				case SCHEDULER_RETURN_REQUEST	: l = 0;	rck = 3;	break; // 2019.01.22
				}
				if ( l < 0 ) l = 0; // 2009.04.02
				//
	//			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) { // 2009.04.02
				for ( j = l ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) { // 2009.04.02
					//
					//-----------------------
					//
					// 2019.01.22
					//
					if ( rck != 3 ) {
						if ( _i_SCH_CLUSTER_Get_PathRun( s , i , j , 0 ) != 0 ) continue;
					}
					//
					//-----------------------
					//
					p = 0; // 2003.10.10
					//-----------------------
					p2 = 0; // 2007.03.28
					//-----------------------
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
	//					if ( -(m) == Chamber ) { // 2007.03.28
						if ( ( -(m) == Chamber ) || ( -(m) == Chamber2 ) ) { // 2007.03.28
							//=====================================================================================================================
							if ( _SCH_COMMON_PM_MULTI_ENABLE_SKIP_CONTROL( s , i , m ) ) continue;
							//=====================================================================================================================
							//
							if ( EnableReadyOnly ) { // 2015.04.03
								if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_READY ) continue;
							}
							//
	//						if ( !_i_SCH_CLUSTER_Chk_Parallel_Used_Disable( s , i , j , Chamber ) ) { // 2007.02.21 // 2007.03.28
							if ( !_i_SCH_CLUSTER_Chk_Parallel_Used_Disable( s , i , j , -(m) ) ) { // 2007.02.21 // 2007.03.28
								if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , -(m) ) != MUF_97_USE_to_SEQDIS_RAND ) {
									if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , -(m) ) != MUF_98_USE_to_DISABLE_RAND ) { // 2006.12.06
										y++;
										_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , i , j , k );
		//								if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26 // 2007.03.28
										if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( -(m) ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( -(m) ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26 // 2007.03.28
											//-------------------------------
											p++; // 2003.10.10
											//-------------------------------
										}
										else {
											//-------------------------------
											p2++; // 2007.03.28
											//-------------------------------
										}
									}
								}
							}
						}
	/*
	// 2007.03.28
						else if ( -(m) == Chamber2 ) { // 2006.11.15
							if ( !_i_SCH_CLUSTER_Chk_Parallel_Used_Disable( s , i , j , Chamber2 ) ) { // 2007.02.21
								if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , -(m) ) != MUF_98_USE_to_DISABLE_RAND ) { // 2006.12.06
									y++;
									_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , i , j , k );
									if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber2 ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber2 ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
										//-------------------------------
										p++; // 2003.10.10
										//-------------------------------
									}
								}
							}
						}
	*/
						else if ( m > 0 ) {
							if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
								//-------------------------------
								p++; // 2003.10.10
								//-------------------------------
							}
							else {
								//-------------------------------
								p2++; // 2007.03.28
								//-------------------------------
							}
						}
					}
					//-------------------------------
	//				if ( p <= 0 ) { // 2007.03.28
					if ( ( p <= 0 ) && ( p2 <= 0 ) ) { // 2007.03.28
	//					c = 0; // 2009.04.03
					}
					else {
	//					if ( ( p > c ) && ( c != 0 ) ) c = p; // 2003.10.10 // 2009.04.02
						if ( p > c ) c = p; // 2003.10.10 // 2009.04.02
					}
					//-------------------------------
				}
				//
				if ( _i_SCH_CLUSTER_Get_PathDo( s , i ) == 0 ) { // 2010.04.20
					if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_STARTING ) {
						_SCH_CLUSTER_Parallel_Check_Curr_DisEna( s , i , Chamber , FALSE );
					}
				}
			}
			//----------------------------------------------------------
	//_IO_CIM_PRINTF( "=======================================\n" );
	//_IO_CIM_PRINTF( "=[E]====================[%d]===========\n" , c );
	//_IO_CIM_PRINTF( "=======================================\n" );

			SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( s , c );	// 2003.10.10
			//----------------------------------------------------------
			//----------------------------------------------------------
	/*
	// 2011.11.17
			if ( y != 0 ) {
	//			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_00_NOTUSE ) { // 2003.07.31 // 2003.10.25
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_00_NOTUSE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_99_USE_to_DISABLE ) ) { // 2003.07.31 // 2003.10.25
					_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_01_USE );
				}
				//============================================================================================
				// 2006.11.15
				//============================================================================================
				if ( Chamber2 != -99999 ) {
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber2 ) == MUF_00_NOTUSE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber2 ) == MUF_99_USE_to_DISABLE ) ) { // 2003.07.31 // 2003.10.25
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber2 , MUF_01_USE );
					}
				}
				//============================================================================================
			}
	*/
			//----------------------------------------------------------
			// 2011.11.17
			if ( y != 0 ) {
				if      ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_00_NOTUSE ) {
					_i_SCH_MODULE_Inc_Mdl_Run_Flag( Chamber );
					_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_01_USE );
				}
				else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_99_USE_to_DISABLE ) {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_01_USE );
				}
				//============================================================================================
				if ( Chamber2 != -99999 ) {
					if      ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber2 ) == MUF_00_NOTUSE ) {
						_i_SCH_MODULE_Inc_Mdl_Run_Flag( Chamber2 );
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber2 , MUF_01_USE );
					}
					else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber2 ) == MUF_99_USE_to_DISABLE ) {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber2 , MUF_01_USE );
					}
				}
				//============================================================================================
			}
			//----------------------------------------------------------
		}
	} // 2018.12.10
	//
	//
	// 2018.12.10
	//
	if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() )	{
		if ( PMMode ) {
			_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 3 );
			if ( Chamber2 != -99999 ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber2 , 3 );
			//
		}
		else {
			_i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber , 1 );
			if ( Chamber2 != -99999 ) _i_SCH_PRM_SET_MODULE_MODE_for_SW( Chamber2 , 1 );
			//
		}
		//
		EIL_CHECK_ENABLE_CONTROL( Chamber , PMMode , NextEnable ); // 2011.07.25
		if ( Chamber2 != -99999 ) EIL_CHECK_ENABLE_CONTROL( Chamber2 , PMMode , NextEnable ); // 2011.07.25
		//
		FA_MDL_STATUS_IO_SET( Chamber ); // 2007.07.23
		//===========================================================================
		// 2007.03.22
		//===========================================================================
		if ( Chamber2 != -99999 ) FA_MDL_STATUS_IO_SET( Chamber2 ); // 2007.07.23
		//===========================================================================
	}
	//
	//
	//
}
//-------------------------------------------------------------------------------------------------------------------
//void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove_Sub( int Chamber , int multipm , BOOL wfrcls ) { // 2017.02.17
void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove_Sub( int Chamber , int multipm , BOOL wfrcls , BOOL force ) { // 2017.02.17
	int j , s , p;
	//------------------------------------
	if ( Chamber < 0 ) return;
	//------------------------------------
	if ( !force ) { // 2017.02.17
		if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) return;
		if ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Chamber ) != 2 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Chamber ) != 0 ) {
				return;
			}
			if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) != ABORTWAIT_FLAG_WAIT ) {
				if ( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( Chamber ) != ABORTWAIT_FLAG_ABORTWAIT ) {
					return;
				}
			}
		}
	}
	//
	for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
		//------------------------------------
		if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , j ) <= 0 ) continue;
		//------------------------------------
		s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , j );
		p = _i_SCH_MODULE_Get_PM_POINTER( Chamber , j );
		//
		_i_SCH_MODULE_Set_PM_WAFER( Chamber , j , 0 );
		//
		_i_SCH_MODULE_Set_PM_SIDE( Chamber , j , 9999 );
		//
	//	_i_SCH_CLUSTER_Set_PathRange( s , p , -1 ); // 2008.04.23
		_i_SCH_CLUSTER_Set_PathRange( s , p , -4 ); // 2008.04.23
		_i_SCH_CLUSTER_Set_PathStatus( s , p , SCHEDULER_READY );
		_i_SCH_CLUSTER_Set_PathDo( s , p , 0 );
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
			_i_SCH_MODULE_Inc_FM_InCount( s );
		}
		//
		_i_SCH_MODULE_Inc_TM_InCount( s );
		//
		if ( wfrcls ) _i_SCH_IO_SET_MODULE_STATUS( Chamber , j + 1 , 0 );
		//
	}
}
//=======================================================================================
//int SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot_Sub( int side , int PreChecking ) { // 2011.06.13 // 2013.03.14
BOOL SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot_Sub( int side , int mode ) { // 2011.06.13 // 2013.03.14
	//================================================================================
	int s , i , j , k , m , c , Chamber , lowrunorder , fg;
	//================================================================================
//	if ( PreChecking == 1 ) return 1; // 2013.03.14
	if ( mode == 1 ) return TRUE; // 2013.03.14
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_ONLY );
	//================================================================================
	//
	lowrunorder = _i_SCH_SYSTEM_RUNORDER_GET( side );
	//
	fg = -1; // 2011.07.28
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) continue;
		//
		if ( _i_SCH_SYSTEM_USING_GET( i ) <= 0 ) continue;
		//
		if ( mode >= 2 ) { // 2013.03.14
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				//
				if ( mode >= 4 ) { // 2013.05.30
					if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < lowrunorder ) lowrunorder = _i_SCH_SYSTEM_RUNORDER_GET( i );
				}
				//
				continue;
			}
		}
		//
		if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < lowrunorder ) lowrunorder = _i_SCH_SYSTEM_RUNORDER_GET( i );
		//
		if ( SYSTEM_MODE_FIRST_GET( i ) ) fg = i; // 2011.07.28
	}
	//
	if ( lowrunorder != _i_SCH_SYSTEM_RUNORDER_GET( side ) ) _i_SCH_SYSTEM_RUNORDER_SET( side , ( lowrunorder / 2 ) + ( lowrunorder / 4 ) );
	//
	if ( mode >= 3 ) { // 2013.03.14
		//================================================================================
		LeaveCriticalSection( &CR_PRE_BEGIN_ONLY );
		//================================================================================
		return FALSE;
	}
	//
	if ( fg != -1 ) { // 2011.07.28
		SYSTEM_MODE_FIRST_SET( fg , FALSE );
		_i_SCH_SYSTEM_MODE_SET( fg , SYSTEM_MODE_FIRST_GET_BUFFER( fg ) );
		//
		SYSTEM_MODE_FIRST_SET( side , TRUE );
		SYSTEM_MODE_FIRST_SET_BUFFER( side , _i_SCH_SYSTEM_MODE_GET( side ) );
		_i_SCH_SYSTEM_MODE_SET( side , 0 );
	}
	//
	//
//printf( "==[%d]=> 1\n" , side );
	for ( Chamber = PM1 ; Chamber < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; Chamber++ ) {
		//
//printf( "==[%d]=> 2 [PM%d] {%d}\n" , side , Chamber - PM1 + 1 , _i_SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) );
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) != MUF_97_USE_to_SEQDIS_RAND ) {
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) != MUF_98_USE_to_DISABLE_RAND ) {
				continue;
			}
		}
		//
//printf( "==[%d]=> 3 [PM%d]\n" , side , Chamber - PM1 + 1 );
		//
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			//-----------------------
			if ( s == side ) continue;
			//-----------------------
//printf( "==[%d]=> 4 [PM%d] [%d]\n" , side , Chamber - PM1 + 1 , s );
//			if ( ( _i_SCH_SYSTEM_USING_GET( s ) < 6 ) || ( SYSTEM_BEGIN_GET( s ) == 0 ) ) continue; // 2015.07.30
			if ( ( _i_SCH_SYSTEM_USING_GET( s ) < 6 ) || ( SYSTEM_BEGIN_GET( s ) <= 1 ) ) continue; // 2015.07.30
//printf( "==[%d]=> 5 [PM%d] [%d]\n" , side , Chamber - PM1 + 1 , s );
			//-----------------------
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) != MUF_01_USE ) continue;
			//-----------------------
//printf( "==[%d]=> 6 [PM%d] [%d]\n" , side , Chamber - PM1 + 1 , s );
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				c = _i_SCH_CLUSTER_Get_PathRange( s , i );
				//
				if ( c < 0 ) continue;
				//
				k = _i_SCH_CLUSTER_Get_PathStatus( s , i );
				//
				if ( ( k != SCHEDULER_READY ) && ( k != SCHEDULER_RETURN_REQUEST ) ) continue;
				//
				for ( j = 0 ; j < c ; j++ ) {
					//
					for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
						//
						m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
						//
						if ( ( -(m) == Chamber ) || ( m == Chamber ) ) {
							//
							_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( s , i , j , k );
							//
						}
					}
				}
			}
			//
//printf( "==[%d]=> 7 [PM%d]\n" , side , Chamber - PM1 + 1 );
			//=========================================================================
			if ( _in_HOT_LOT_ORDERING_INVALID[s] == 0 ) {
				//=========================================================================
				_in_HOT_LOT_ORDERING_INVALID[s] = 1;
				//=========================================================================
				_SCH_IO_SET_MAIN_BUTTON_MC( s , CTC_WAITING );
				//
				BUTTON_SET_FLOW_MTLOUT_STATUS( s , -1 , FALSE , _MS_11_WAITING ); // 2013.06.08
				//
				//=========================================================================
			}
			//=========================================================================
			_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_98_USE_to_DISABLE_RAND );
			//=========================================================================
			c = _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( s ) - 1;	if ( c < 0 ) c = 0;
			SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( s , c );
			//=========================================================================
		}
		//=========================================================================
//printf( "==[%d]=> 8 [PM%d]\n" , side , Chamber - PM1 + 1 );
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
				//
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
					//
					if ( ( -(m) == Chamber ) || ( m == Chamber ) ) {
						//
						_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( side , i , j , k );
						//
					}
				}
			}
		}
		//=========================================================================
		_i_SCH_MODULE_Set_Mdl_Use_Flag( side , Chamber , MUF_01_USE );
		//=========================================================================
		c = _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( side ) + 1;
		SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( side , c );
		//=========================================================================
	}
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_ONLY );
	//================================================================================
//	return 0; // 2013.03.14
	return TRUE; // 2013.03.14
}

//=======================================================================================
//
extern BOOL PRE_RUN_CHECK_RESET; // 2016.03.28
//


BOOL SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( int side , BOOL PreChecking , int *mode ) { // 2011.06.13
	BOOL Res; // 2013.03.14
	//
	*mode = -1; // 2013.03.14
	//------------------------------------------------------------------------------------------
	if ( _in_HOT_LOT_CHECKING[side] == 0 ) return FALSE; // 2011.06.13
	//------------------------------------------------------------------------------------------
	if ( !PreChecking ) EnterCriticalSection( &CR_WAIT );
	//
	if ( PreChecking ) { // 2013.03.14
		if      ( _in_HOT_LOT_CHECKING[side] == 1 )
			*mode = 1;
		else if ( _in_HOT_LOT_CHECKING[side] == 2 )
			*mode = 2;
		else if ( _in_HOT_LOT_CHECKING[side] == 3 )
			*mode = 3;
		else // 2013.05.30
			*mode = 4;
	}
	else {
		*mode = 0;
	}
	//
//	SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot_Sub( side , PreChecking ); // 2013.03.14
	Res = SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot_Sub( side , *mode ); // 2013.03.14
	//
//	if ( !PreChecking ) _in_HOT_LOT_CHECKING[side] = 0; // 2011.06.13 // 2013.03.14
	if ( *mode >= 2 ) _in_HOT_LOT_CHECKING[side] = 0; // 2011.06.13 // 2013.03.14

	if ( !PreChecking ) LeaveCriticalSection( &CR_WAIT );
	//------------------------------------------------------------------------------------------
//	return TRUE; // 2013.03.14
	return Res; // 2013.03.14
}

//void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( int Chamber , int multipm , BOOL wfrcls ) { // 2009.06.23 // 2017.02.17
void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( int Chamber , int multipm , BOOL wfrcls , BOOL force ) { // 2009.06.23 // 2017.02.17
	EnterCriticalSection( &CR_WAIT );
	//
//	SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove_Sub( Chamber , multipm , wfrcls ); // 2017.02.17
	SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove_Sub( Chamber , multipm , wfrcls , force ); // 2017.02.17
	//
	PRE_RUN_CHECK_RESET = TRUE; // 2016.03.28
	//
	LeaveCriticalSection( &CR_WAIT );
}

BOOL SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( int Chamber , int HW , int Check , int abflag , int multipm ) { // 2009.06.23
	BOOL Res;
	EnterCriticalSection( &CR_WAIT );
	Res = SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable_Sub( Chamber , HW , Check , abflag , multipm );
	//
	PRE_RUN_CHECK_RESET = TRUE; // 2016.03.28
	//
	LeaveCriticalSection( &CR_WAIT );
	return Res;
}

void SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( int Chamber , BOOL PMMode , int NextEnable , int abflag , int multipm ) { // 2009.06.23
	//
	if ( NextEnable == 0 ) { // 2010.09.21
		if ( !_MSG_READYEND_READY_ACTION_BEFORE_ENABLE( Chamber ) ) { // 2010.09.21
			//
			return;
		}
	}
	//
	EnterCriticalSection( &CR_WAIT );
	//
	SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable_Sub( Chamber , PMMode , NextEnable , abflag , multipm );
	//
	PRE_RUN_CHECK_RESET = TRUE; // 2016.03.28
	//
	LeaveCriticalSection( &CR_WAIT );
}

//=======================================================================================
void SCHEDULER_CONTROL_Data_Initialize_Part( int apmode , int side ) {
	int i;
	//============================================================================================================
	// apmode = 0 : Init
	// apmode = 1 : Start
	// apmode = 2 : End
	// apmode = 3 : Start(only)
	// apmode = 4 : End(only)
	//============================================================================================================
	switch ( apmode ) { // 2010.03.26
	case 0 :
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			_insdie_run_Tag[i] = 0;
			_in_HOT_LOT_CHECKING[i] = 0; // 2011.06.13
		}
		break;
	case 1 :
	case 3 :
		_insdie_run_Tag[side]++;
		break;
	case 2 :
	case 4 :
		_in_HOT_LOT_CHECKING[side] = 0; // 2011.06.13
		break;
	}
	//============================================================================================================
	INIT_PROCESS_MONITOR_DATA( apmode , side );
	//============================================================================================================
	INIT_RUN_COMMON( apmode , side ); // 2008.09.26
	//============================================================================================================
	INIT_SCHEDULER_CASSETTE_RESULT_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_CONTROL_RECIPE_SETTING_DATA( apmode , side );
	//============================================================================================================
	INIT_FA_HANDLER_SETTING_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_PRM_FBTPM_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_PRM_CLUSTER_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_MFALIC_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_CONTROL_FIRSTRUNPRE_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_CONTROL_PREPOST_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_CONTROL_ONESELECT_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_FM_ARM_MULTI_DATA( apmode , side );
	//============================================================================================================
	INIT_PROCESS_INTERFACE_MODULE_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_REG_RESET_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_MACRO_DATA( apmode , side );
	//============================================================================================================
	INIT_TIMER_KPLT_DATA( apmode , side );
	//============================================================================================================
	INIT_SIGNAL_MODE_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_PRM_DATA( apmode , side );
	//============================================================================================================
	INIT_SCHEDULER_EIL_DATA( apmode , side );
	//============================================================================================================
	INIT_SCH_RESTART_CONTROL( apmode , side );
	//============================================================================================================
	if ( apmode != 0 ) _SCH_COMMON_INITIALIZE_PART( apmode , side );
	//============================================================================================================
	//============================================================================================================
	//============================================================================================================
	//============================================================================================================
	if ( apmode == 0 ) { // 2003.06.13
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2006.03.29
			//
			_i_SCH_SUB_Reset_Scheduler_Data( i );
			//
		}
		//
		ROBOT_LAST_ACTION_CONTROL( -1 , -1 , -1 , -1 ); // 2013.03.26
		//
	}
	//================================================================================================
	if ( apmode == 3 ) {
		//
		IO_SET_PAUSE_STATUS( 0 ); // 2004.06.29
		//
		ROBOT_LAST_ACTION_CONTROL( -1 , -1 , -1 , -1 ); // 2013.03.26
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2016.12.13
			//
			_SCH_IO_SET_MODULE_FLAG_DATA( i + 1 , 0 ); // 2016.12.13
			//
			_in_HSide_Last_Pointer[i] = -1; // 2018.11.22
			//
		}
		//
	}
	//================================================================================================
//	if ( ( apmode == 2 ) || ( apmode == 4 ) ) SCHEDULER_CONTROL_Make_Clear_Method_Data( side ); // 2011.04.28 // 2012.04.25
	//================================================================================================
}
//=======================================================================================
void _i_SCH_SUB_Wait_Finish_Complete( int CHECKING_SIDE , int mode ) { // 2003.05.22
	Scheduler_Main_Wait_Finish_Complete( CHECKING_SIDE , mode ); // 2003.05.22
}
//=======================================================================================
void _i_SCH_SUB_Reset_Scheduler_Data( int side ) {
	_i_SCH_MODULE_Init_FBTPMData( side );
	//
	_i_SCH_CLUSTER_Init_ClusterData( side , FALSE ); // 2007.11.28
	//
	SCHEDULER_CONTROL_Set_Disable_Disappear( side , FALSE ); // 2003.12.10
	//
	_i_SCH_SUB_Set_Last_Status( side , 0 ); // 2004.09.07
	//
	_i_SCH_CASSETTE_Reset_Side_Monitor( side , 1 ); // 2004.09.07
	//
	PROCESS_MONITOR_STATUS_Abort_Signal_Reset( side ); // 2004.09.07
	//
	SCHEDULER_CONTROL_Set_CASSETTE_SKIP( side , -1 , FALSE ); // 2005.07.19
	//
//	SCHEDULER_CONTROL_Set_GLOBAL_STOP( -1 , 0 , 0 , FALSE ); // 2007.11.08 // 2013.11.28
	SCHEDULER_CONTROL_Set_GLOBAL_STOP( -1 , 0 , 0 , 0 ); // 2007.11.08 // 2013.11.28
	//
	_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( side , -1 ); // 2007.09.03
	//
}
//=======================================================================================
void SCHEDULER_CONTROL_Switch_Scheduler_Data( int side ) {
	//
	_i_SCH_MODULE_Init_FBTPMData( side );
	//
	_i_SCH_CLUSTER_Init_ClusterData( side , TRUE ); // 2007.11.28
	//
	_i_SCH_SUB_Set_Last_Status( side , 0 ); // 2018.09.28
	//
	SCHEDULER_CONTROL_Set_CASSETTE_SKIP( side , -1 , FALSE ); // 2018.09.28
	//
}
//=======================================================================================
int SCHEDULER_CONTROL_Check_Scheduler_Data_for_Run( int side , int RunAll , int starts , int ends , int CPJOB , int ch1 , int ch2 ) {
	int i , j , Temp = 0 , c , si , so;
//	int wc[MAX_CASSETTE_SIDE]; // 2010.01.19
//	int ws[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2010.01.19
	int wc[MAX_CHAMBER]; // 2010.01.19
	int ws[MAX_CHAMBER][MAX_CASSETTE_SLOT_SIZE]; // 2010.01.19
	char String_Read_Buffer[256];
	BOOL f , checkall , Res;

	if ( RunAll == -1 ) { // 2011.09.19
		checkall = FALSE;
	}
	else {
//		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13 // 2017.10.10
		if ( !_SCH_SUB_NOCHECK_RUNNING( side ) ) { // 2010.09.13 // 2017.10.10
			checkall = _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 1 , side , ch1 , ch2 , 0 );
		}
		else {
			checkall = FALSE;
		}
	}

//	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2002.03.27 // 2010.01.19
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2002.03.27 // 2010.01.19
		wc[i] = 0;
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			ws[i][j] = 0;
		}
	}
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2002.03.27
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {
//			if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < PM1 ) { // 2010.01.19
//				wc[_i_SCH_CLUSTER_Get_PathIn( side , i )-CM1]++; // 2010.01.19
				if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) >= CM1 ) { // 2011.03.30
					wc[_i_SCH_CLUSTER_Get_PathIn( side , i )]++; // 2010.01.19
				} // 2011.03.30
//			} // 2010.01.19
//			if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) < PM1 ) { // 2010.01.19
//				wc[_i_SCH_CLUSTER_Get_PathOut( side , i )-CM1]++; // 2010.01.19
				if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) { // 2011.03.30
					wc[_i_SCH_CLUSTER_Get_PathOut( side , i )]++; // 2010.01.19
				}
//			} // 2010.01.19
		}
	}
//	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2002.03.27 // 2010.01.19
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2002.03.27 // 2010.01.19
		if ( wc[i] > 0 ) {
			if ( i < PM1 ) { 
				//
				if ( RunAll == -1 ) { // 2011.09.19
					ws[ i ][ j - 1 ] = CWM_PRESENT;
				}
				else {
	//				if ( !WAFER_STATUS_IN_ALL_CASSETTE( i + CM1 , 0 , String_Read_Buffer ) ) return FALSE; // 2010.01.19
					if ( !WAFER_STATUS_IN_ALL_CASSETTE( i , 0 , String_Read_Buffer ) ) return 1; // 2010.01.19
					for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
						ws[i][j] = String_Read_Buffer[j] - '0';
					}
				}
				//
			}
			else if ( i >= BM1 ) {
				for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( RunAll != -1 ) { // 2011.09.19
						ws[ i ][ j - 1 ] = CWM_PRESENT;
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) > 0 ) {
							switch( _i_SCH_IO_GET_MODULE_RESULT( i , j ) ) {
//							case WAFERRESULT_SUCCESS : // 2016.08.19
							case WAFERRESULT_PRESENT : // 2016.08.19
								ws[ i ][ j - 1 ] = CWM_PRESENT;
								break;
							case WAFERRESULT_FAILURE :
								ws[ i ][ j - 1 ] = CWM_FAILURE;
								break;
							default :
								ws[ i ][ j - 1 ] = CWM_PROCESS;
								break;
							}
						}
						else {
							ws[ i ][ j - 1 ] = CWM_ABSENT;
						}
					}
				}
			}
		}
	}
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < CM1 ) continue; // 2011.04.14
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < PM1 ) {
			//
			if ( RunAll == -1 ) continue; // 2011.09.19
			//
			si = _i_SCH_CLUSTER_Get_SlotIn( side , i );
			so = _i_SCH_CLUSTER_Get_SlotOut( side , i );
			//
			if ( _i_SCH_SYSTEM_MOVEMODE_GET( side ) == 2 ) { // 2013.09.03
				f = TRUE;
			}
			else {
				c = ws[ _i_SCH_CLUSTER_Get_PathIn( side , i )][ si - 1 ]; // 2002.03.27 // 2010.01.19
				f = FALSE;
				if ( ( RunAll == 1 ) || _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN() ) {
					//
//					if ( ( c == CWM_PROCESS ) || ( c == CWM_FAILURE ) || ( c == CWM_PRESENT ) ) { // 2016.08.19
					if ( ( c == CWM_PROCESS ) || ( c == CWM_PRESENT ) ) { // 2016.08.19
						//
//						if ( ( c == CWM_PROCESS ) || ( c == CWM_FAILURE ) ) { // 2016.08.19
						if ( c == CWM_PROCESS ) { // 2016.08.19
							//
							_i_SCH_IO_SET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , i ) , si , CWM_PRESENT );
						}
						f = TRUE;
					}
				}
				else {
					if ( c == CWM_PRESENT ) f = TRUE;
				}
			}
		}
		else if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) >= BM1 ) { // 2010.01.19
			//
			si = _i_SCH_CLUSTER_Get_SlotIn( side , i );
			so = _i_SCH_CLUSTER_Get_SlotOut( side , i );
			//
			if ( RunAll != -1 ) { // 2011.09.19
				f = TRUE;
			}
			else {
				//
				c = ws[ _i_SCH_CLUSTER_Get_PathIn( side , i )][ si - 1 ]; // 2002.03.27 // 2010.01.19
				f = FALSE;
				if ( ( RunAll == 1 ) || _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN() ) {
					if ( ( c == CWM_PROCESS ) || ( c == CWM_FAILURE ) || ( c == CWM_PRESENT ) ) {
						if ( ( c == CWM_PROCESS ) || ( c == CWM_FAILURE ) ) {
							_i_SCH_IO_SET_MODULE_RESULT( _i_SCH_CLUSTER_Get_PathIn( side , i ) , si , WAFERRESULT_PRESENT );
						}
						//
	//					_i_SCH_IO_SET_MODULE_STATUS( _i_SCH_CLUSTER_Get_PathIn( side , i ) , si , si ); // 2011.05.31
	//					_i_SCH_IO_SET_MODULE_SOURCE( _i_SCH_CLUSTER_Get_PathIn( side , i ) , si , _i_SCH_CLUSTER_Get_PathIn( side , i ) - CM1 ); // 2011.05.31
						//
						f = TRUE;
					}
				}
				else {
					if ( c == CWM_PRESENT ) f = TRUE;
				}
			}
			//
		}
		else {
			continue;
		}
		//
		if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( side , i ) != 0 ) f = TRUE; // 2015.10.12
		//
		if ( RunAll == -1 ) { // 2011.09.19
			//
			if ( !f ) _i_SCH_CLUSTER_Set_PathRange( side , i , -2 );
			//
			continue;
		}
		//
		if ( !checkall ) continue;// 2008.05.08
		//
		if ( f ) {
			//
			Temp++;
			//
//			if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13 // 2017.10.10
			if ( !_SCH_SUB_NOCHECK_RUNNING( side ) ) { // 2010.09.13 // 2017.10.10
				Res = _SCH_COMMON_CASSETTE_CHECK_INVALID_INFO( 2 , side , ch1 , ch2 , 0 );
			}
			else {
				Res = FALSE;
			}
			//
			if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( side , i ) != 0 ) Res = FALSE; // 2015.10.12
			//
			if ( Res ) { // 2008.04.29
				//
				if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) { // 2011.03.30
					//
					if ( _i_SCH_SYSTEM_MOVEMODE_GET( side ) == 2 ) { // 2013.09.03
						//
						if ( ws[ _i_SCH_CLUSTER_Get_PathOut( side , i )][ so - 1 ] != CWM_ABSENT )
							return 3;
						//
					}
					else {
						if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) == _i_SCH_CLUSTER_Get_PathOut( side , i ) ) {
							if ( si != so ) {
								if ( ws[ _i_SCH_CLUSTER_Get_PathOut( side , i )][ so - 1 ] != CWM_ABSENT ) // 2002.03.27 // 2010.01.19
									return 2;
							}
						}
						else if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) != _i_SCH_CLUSTER_Get_PathOut( side , i ) ) {
							if ( ws[ _i_SCH_CLUSTER_Get_PathOut( side , i )][ so - 1 ] != CWM_ABSENT ) // 2002.03.27 // 2010.01.19
								return 3;
						}
					}
					//
					if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
						if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , si ) ) {
								return 4;
							}
							if ( _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , so ) ) {
								return 5;
							}
						}
						else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							if ( _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , si ) ) {
								return 6;
							}
							if ( _i_SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , so ) ) {
								return 7;
							}
						}
					}
				}
			}
			//================================================================
			_i_SCH_CASSETTE_LAST_RESET( _i_SCH_CLUSTER_Get_PathIn( side , i ) , si );
			//
			_i_SCH_CASSETTE_LAST_RESET2( side , i ); // 2011.04.21
			//================================================================
		}
		else {
			_i_SCH_CLUSTER_Set_PathRange( side , i , -2 ); // 2008.04.23
		}
	}
	//----------------------------------------------------------------------------------------
	if ( RunAll == -1 ) return 0; // 2011.09.19
	//----------------------------------------------------------------------------------------
	//
	// 2015.10.12
	//
	if ( checkall ) {
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) ws[i][j] = 0;
		}
		//
		for ( j = 0 ; j < 2 ; j++ ) {
			//
			for ( si = 0 ; si < MAX_CASSETTE_SIDE ; si++ ) {
				//
				if ( j == 0 ) {
					if ( si != side ) continue;
				}
				else {
					//
					if ( si == side ) continue;
					//
					if ( _i_SCH_SYSTEM_USING_GET( si ) < 4 ) continue;
					if ( SYSTEM_BEGIN_GET( si ) < 1 ) continue;
					//
				}
				//
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
					//
					if ( _i_SCH_CLUSTER_Get_PathRange( si , i ) < 0 ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( si , i ) == SCHEDULER_CM_END ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( si , i ) < CM1 ) continue;
					//
					if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( si , i ) != 0 ) continue; // 2015.10.12
					//
					if ( _i_SCH_CLUSTER_Get_PathStatus( si , i ) == SCHEDULER_READY ) { // 2019.02.28
						//
						if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( si , i ) % 100 ) != _MOTAG_12_NEED_IN_OUT ) { // 2019.02.28
							if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( si , i ) % 100 ) != _MOTAG_13_NEED_IN ) { // 2019.02.28
								//
								if ( _i_SCH_CLUSTER_Get_SlotIn( si , i ) > 0 ) {
									//
									if ( j == 0 ) {
										//
										ws[ _i_SCH_CLUSTER_Get_PathIn( si , i )][ _i_SCH_CLUSTER_Get_SlotIn( si , i ) - 1 ]++;
										//
										if ( ws[ _i_SCH_CLUSTER_Get_PathIn( si , i )][ _i_SCH_CLUSTER_Get_SlotIn( si , i ) - 1 ] > 1 ) return 11;
										//
									}
									else {
										//
										if ( _i_SCH_CLUSTER_Get_PathStatus( si , i ) == SCHEDULER_READY ) { // 2018.08.17
											//
											if ( ws[ _i_SCH_CLUSTER_Get_PathIn( si , i )][ _i_SCH_CLUSTER_Get_SlotIn( si , i ) - 1 ] > 0 ) return 12;
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
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( si , i ) == _i_SCH_CLUSTER_Get_PathOut( si , i ) ) {
						//
						if ( _i_SCH_CLUSTER_Get_SlotIn( si , i ) == _i_SCH_CLUSTER_Get_SlotOut( si , i ) ) continue;
						//
					}
					else {
						//
						if ( _i_SCH_CLUSTER_Get_PathOut( si , i ) < CM1 ) continue;
						//
					}
					//
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( si , i ) % 100 ) != _MOTAG_12_NEED_IN_OUT ) { // 2019.02.28
						if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( si , i ) % 100 ) != _MOTAG_14_NEED_OUT ) { // 2019.02.28
							//
							if ( _i_SCH_CLUSTER_Get_SlotOut( si , i ) > 0 ) {
								//
								if ( j == 0 ) {
									//
									ws[ _i_SCH_CLUSTER_Get_PathOut( si , i )][ _i_SCH_CLUSTER_Get_SlotOut( si , i ) - 1 ]++;
									//
									if ( ws[ _i_SCH_CLUSTER_Get_PathOut( si , i )][ _i_SCH_CLUSTER_Get_SlotOut( si , i ) - 1 ] > 1 ) return 13;
									//
								}
								else {
									//
									if ( ws[ _i_SCH_CLUSTER_Get_PathOut( si , i )][ _i_SCH_CLUSTER_Get_SlotOut( si , i ) - 1 ] > 0 ) return 14;
									//
								}
								//
							}
						}
					}
				}
				//
			}
			//
		}
		//
	}
	//
	//----------------------------------------------------------------------------------------
	for ( j = BM1 ; j < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ; j++ ) {
		//================================================================
		if ( !SCH_Inside_ThisIs_BM_OtherChamber( j , 1 ) ) continue; // 2009.01.21
		//================================================================
		_i_SCH_CASSETTE_LAST_RESET( j , -1 );
		//================================================================
	}
	//----------------------------------------------------------------------------------------
	LOG_MTL_SET_TARGET_WAFER_COUNT( side , Temp );
	//----------------------------------------------------------------------------------------
	if ( checkall ) { // 2008.05.08
		if ( Temp == 0 ) return 8;
	}
	//
	if ( CPJOB != 0 ) SCHMULTI_RUNJOB_SET_DATABASE_LINK( side ); // 2011.04.21
	//
	return 0;
}
//=======================================================================================
void SCHEDULER_CONTROL_MapData_Make_Verification_Info( int side , int cass1 , int cass2 , CASSETTEVerTemplate *MAPPING_INFO , int mapbufmode ) {
	int i;
	char String_Read_Buffer[256]; // 2002.03.27

	//=====================================================
	// 2002.03.27 // Change for Speed
	// 2010.03.10
	//=====================================================
	MAPPING_INFO->CASSETTE1 = ( cass1 > 0 ) ? cass1 : 0;
	MAPPING_INFO->CASSETTE2 = ( cass2 > 0 ) ? cass2 : 0;
	//
	if ( ( cass1 > 0 ) && ( cass2 > 0 ) && ( cass1 < PM1 ) && ( cass2 < PM1 ) ) { // 2010.03.10
		if ( WAFER_STATUS_IN_ALL_CASSETTE( cass1 , cass2 , String_Read_Buffer ) ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				MAPPING_INFO->CASSETTE1_STATUS[i] = String_Read_Buffer[i] - '0';
				//------------------------------------------------------------------------------
				Scheduler_Mapping_Buffer_Data_Set( side , i , mapbufmode , String_Read_Buffer[i] - '0' ); // 2005.03.02
				//------------------------------------------------------------------------------
			}
			for ( i = MAX_CASSETTE_SLOT_SIZE ; i < (MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ); i++ ) {
				MAPPING_INFO->CASSETTE2_STATUS[i-MAX_CASSETTE_SLOT_SIZE] = String_Read_Buffer[i] - '0';
			}
		}
	}
	else {
		if ( cass1 > 0 ) {
			if ( cass1 < PM1 ) {
				if ( WAFER_STATUS_IN_ALL_CASSETTE( cass1 , 0 , String_Read_Buffer ) ) {
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						//------------------------------------------------------------------------------
						MAPPING_INFO->CASSETTE1_STATUS[i] = String_Read_Buffer[i] - '0';
						//------------------------------------------------------------------------------
						Scheduler_Mapping_Buffer_Data_Set( side , i , mapbufmode , String_Read_Buffer[i] - '0' ); // 2005.03.02
						//------------------------------------------------------------------------------
					}
				}
			}
			else {
				for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( cass1 ) ; i++ ) {
					//------------------------------------------------------------------------------
					MAPPING_INFO->CASSETTE1_STATUS[i] = ( _i_SCH_IO_GET_MODULE_STATUS( cass1 , i+1 ) > 0 ) ? 2 : 1;
					//------------------------------------------------------------------------------
					Scheduler_Mapping_Buffer_Data_Set( side , i , mapbufmode , MAPPING_INFO->CASSETTE1_STATUS[i] ); // 2005.03.02
					//------------------------------------------------------------------------------
				}
			}
		}
		//
		if ( cass2 > 0 ) {
			if ( cass2 < PM1 ) {
				if ( WAFER_STATUS_IN_ALL_CASSETTE( cass2 , 0 , String_Read_Buffer ) ) {
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						//------------------------------------------------------------------------------
						MAPPING_INFO->CASSETTE2_STATUS[i] = String_Read_Buffer[i] - '0';
						//------------------------------------------------------------------------------
						if ( cass1 <= 0 ) {
							Scheduler_Mapping_Buffer_Data_Set( side , i , mapbufmode , String_Read_Buffer[i] - '0' ); // 2005.03.02
						}
						//------------------------------------------------------------------------------
					}
				}
			}
			else {
				for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( cass2 ) ; i++ ) {
					//------------------------------------------------------------------------------
					MAPPING_INFO->CASSETTE2_STATUS[i] = ( _i_SCH_IO_GET_MODULE_STATUS( cass2 , i+1 ) > 0 ) ? 2 : 1;
					//------------------------------------------------------------------------------
					if ( cass1 <= 0 ) {
						Scheduler_Mapping_Buffer_Data_Set( side , i , mapbufmode , MAPPING_INFO->CASSETTE2_STATUS[i] ); // 2005.03.02
					}
					//------------------------------------------------------------------------------
				}
			}
		}
	}
	//=====================================================
}
//=======================================================================================
void SCHEDULER_CONTROL_RunData_Make_Verification_Info( int side , int id , CLUSTERStepTemplate *CLUSTER_INFO , CLUSTERStepTemplate4 *CLUSTER_INFO4 ) { // 2004.12.14
	int j , k , m;
	char DirName[255];
	char RunName[255];
	//
	if ( _i_SCH_CLUSTER_Get_PathRange( side , id ) >= 0 ) {
		CLUSTER_INFO->HANDLING_SIDE = _i_SCH_CLUSTER_Get_PathRange( side , id );
		CLUSTER_INFO->EXTRA_STATION = ( _i_SCH_CLUSTER_Get_PathIn( side , id ) * 100 ) + _i_SCH_CLUSTER_Get_SlotIn( side , id );
		//
		if ( _i_SCH_CLUSTER_Get_PathOut( side , id ) < CM1 ) { // 2011.05.25
			CLUSTER_INFO->EXTRA_TIME    = _i_SCH_CLUSTER_Get_SlotOut( side , id );
		}
		else {
			CLUSTER_INFO->EXTRA_TIME    = ( _i_SCH_CLUSTER_Get_PathOut( side , id ) * 100 ) + _i_SCH_CLUSTER_Get_SlotOut( side , id );
		}
		//
		for ( j = 0 ; j < CLUSTER_INFO->HANDLING_SIDE ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , id , j , k );
				CLUSTER_INFO->MODULE[j][k] = m;
				strcpy( CLUSTER_INFO->MODULE_IN_RECIPE[j][k] , "" );
				strcpy( CLUSTER_INFO->MODULE_OUT_RECIPE[j][k] , "" );
				strcpy( CLUSTER_INFO->MODULE_PR_RECIPE[j][k] , "" );
				if ( m > 0 ) {
					if ( STR_MEM_SIZE( _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 0 ) ) > 0 ) {
						UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 0 ) , DirName , 255 , RunName , 255 ); // 2005.08.03
						_snprintf( CLUSTER_INFO->MODULE_IN_RECIPE[j][k] , 64 , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( m ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( m ) , RunName );
					}
					if ( STR_MEM_SIZE( _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 1 ) ) > 0 ) {
						UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 1 ) , DirName , 255 , RunName , 255 ); // 2005.08.03
						_snprintf( CLUSTER_INFO->MODULE_OUT_RECIPE[j][k] , 64 , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( m ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( m ) , RunName );
					}
					if ( STR_MEM_SIZE( _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 2 ) ) > 0 ) {
						UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( side ) , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 2 ) , DirName , 255 , RunName , 255 ); // 2005.08.03
						_snprintf( CLUSTER_INFO->MODULE_PR_RECIPE[j][k] , 64 , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( m ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( m ) , RunName ); // 2007.01.26
					}
				}
			}
		}
		//====================================================================================
//		CLUSTER_INFO4->LOT_SPECIAL_DATA = _i_SCH_CLUSTER_Get_LotSpecial( side , id ); // 2014.06.23
		//
		// 2014.06.23
		//
		if ( _i_SCH_CLUSTER_Get_LotSpecial( side , id ) > 0 ) {
			CLUSTER_INFO4->LOT_SPECIAL_DATA = _i_SCH_CLUSTER_Get_LotSpecial( side , id );
		}
		else if ( _i_SCH_CLUSTER_Get_ClusterSpecial( side , id ) > 0 ) {
			CLUSTER_INFO4->LOT_SPECIAL_DATA = _i_SCH_CLUSTER_Get_ClusterSpecial( side , id );
		}
		else {
			CLUSTER_INFO4->LOT_SPECIAL_DATA = 0;
		}
		//
		//====================================================================================
		CLUSTER_INFO4->LOT_USER_DATA = _i_SCH_CLUSTER_Get_LotUserSpecial( side , id );
		//====================================================================================
		CLUSTER_INFO4->CLUSTER_USER_DATA = _i_SCH_CLUSTER_Get_ClusterUserSpecial( side , id );
		//====================================================================================
		CLUSTER_INFO4->RECIPE_TUNE_DATA = _i_SCH_CLUSTER_Get_ClusterTuneData( side , id );
		//====================================================================================
	}
	else {
		CLUSTER_INFO->HANDLING_SIDE = -1;
		//
		CLUSTER_INFO4->LOT_SPECIAL_DATA = 0;
		CLUSTER_INFO4->LOT_USER_DATA = NULL;
		CLUSTER_INFO4->CLUSTER_USER_DATA = NULL;
		CLUSTER_INFO4->RECIPE_TUNE_DATA = NULL;
	}
}
//=======================================================================================
void SCHEDULER_CONTROL_RunData_Make_Verification_Info2( int side , int id , CLUSTERStepTemplate2 *CLUSTER_INFO , CLUSTERStepTemplate4 *CLUSTER_INFO4 ) { // 2004.12.14
	int j , k , m;
	//
	if ( _i_SCH_CLUSTER_Get_PathRange( side , id ) >= 0 ) {
		CLUSTER_INFO->HANDLING_SIDE = _i_SCH_CLUSTER_Get_PathRange( side , id );
		CLUSTER_INFO->EXTRA_STATION = ( _i_SCH_CLUSTER_Get_PathIn( side , id ) * 100 ) + _i_SCH_CLUSTER_Get_SlotIn( side , id );
		if ( _i_SCH_CLUSTER_Get_PathOut( side , id ) < CM1 ) { // 2011.05.25
			CLUSTER_INFO->EXTRA_TIME    = _i_SCH_CLUSTER_Get_SlotOut( side , id );
		}
		else {
			CLUSTER_INFO->EXTRA_TIME    = ( _i_SCH_CLUSTER_Get_PathOut( side , id ) * 100 ) + _i_SCH_CLUSTER_Get_SlotOut( side , id );
		}
		//
		for ( j = 0 ; j < CLUSTER_INFO->HANDLING_SIDE ; j++ ) {
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , id , j , k );
				CLUSTER_INFO->MODULE[j][k] = m;
				if ( m > 0 ) {
					CLUSTER_INFO->MODULE_IN_RECIPE2[j][k]  = _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 0 );
					CLUSTER_INFO->MODULE_OUT_RECIPE2[j][k] = _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 1 );
					CLUSTER_INFO->MODULE_PR_RECIPE2[j][k]  = _i_SCH_CLUSTER_Get_PathProcessRecipe( side , id , j , k , 2 );
				}
				else {
					CLUSTER_INFO->MODULE_IN_RECIPE2[j][k]  = NULL;
					CLUSTER_INFO->MODULE_OUT_RECIPE2[j][k] = NULL;
					CLUSTER_INFO->MODULE_PR_RECIPE2[j][k]  = NULL;
				}
			}
		}
		//====================================================================================
//		CLUSTER_INFO4->LOT_SPECIAL_DATA = _i_SCH_CLUSTER_Get_LotSpecial( side , id ); // 2014.06.23
		//
		// 2014.06.23
		//
		if ( _i_SCH_CLUSTER_Get_LotSpecial( side , id ) > 0 ) {
			CLUSTER_INFO4->LOT_SPECIAL_DATA = _i_SCH_CLUSTER_Get_LotSpecial( side , id );
		}
		else if ( _i_SCH_CLUSTER_Get_ClusterSpecial( side , id ) > 0 ) {
			CLUSTER_INFO4->LOT_SPECIAL_DATA = _i_SCH_CLUSTER_Get_ClusterSpecial( side , id );
		}
		else {
			CLUSTER_INFO4->LOT_SPECIAL_DATA = 0;
		}
		//
		//====================================================================================
		CLUSTER_INFO4->LOT_USER_DATA = _i_SCH_CLUSTER_Get_LotUserSpecial( side , id );
		//====================================================================================
		CLUSTER_INFO4->CLUSTER_USER_DATA = _i_SCH_CLUSTER_Get_ClusterUserSpecial( side , id );
		//====================================================================================
		CLUSTER_INFO4->RECIPE_TUNE_DATA = _i_SCH_CLUSTER_Get_ClusterTuneData( side , id );
		//====================================================================================
	}
	else {
		CLUSTER_INFO->HANDLING_SIDE = -1;
		//
		CLUSTER_INFO4->LOT_SPECIAL_DATA = 0;
		CLUSTER_INFO4->LOT_USER_DATA = NULL;
		CLUSTER_INFO4->CLUSTER_USER_DATA = NULL;
		CLUSTER_INFO4->RECIPE_TUNE_DATA = NULL;
	}
}
//=======================================================================================
int SCHEDULER_CONTROL_Check_Scheduler_Wafer_Inform_Same( int side , int ch ) {
	int i , j , c;
//	int wc[MAX_CASSETTE_SIDE]; // 2010.01.17
//	int ws[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2010.01.17
	int wc[MAX_CHAMBER]; // 2010.01.17
	int ws[MAX_CHAMBER][MAX_CASSETTE_SLOT_SIZE]; // 2010.01.17
	char String_Read_Buffer[256];

//	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2002.03.27 // 2010.01.17
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2002.03.27 // 2010.01.17
		wc[i] = 0;
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			ws[i][j] = 0;
		}
	}
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2002.03.27
	//	if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) {// 2010.01.19
//			if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < PM1 ) { // 2010.01.19
//				wc[_i_SCH_CLUSTER_Get_PathIn( side , i )-CM1]++; // 2010.01.19
			if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) >= CM1 ) { // 2011.04.14
				wc[_i_SCH_CLUSTER_Get_PathIn( side , i )]++; // 2010.01.19
			} // 2011.04.14
//			} // 2010.01.19
//			if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) < PM1 ) { // 2010.01.19
//				wc[_i_SCH_CLUSTER_Get_PathOut( side , i )-CM1]++; // 2010.01.19
			if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) { // 2011.04.14
				wc[_i_SCH_CLUSTER_Get_PathOut( side , i )]++; // 2010.01.19
			} // 2011.04.14
//			} // 2010.01.19
//		}
	}
//	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2002.03.27// 2010.01.19
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2002.03.27// 2010.01.19
		if ( wc[i] > 0 ) {
			if ( i < PM1 ) {
//				if ( !WAFER_STATUS_IN_ALL_CASSETTE( i + CM1 , 0 , String_Read_Buffer ) ) return FALSE; // 2010.01.19
				if ( !WAFER_STATUS_IN_ALL_CASSETTE( i , 0 , String_Read_Buffer ) ) return 1; // 2010.01.19
				for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
					ws[i][j] = String_Read_Buffer[j] - '0';
				}
			}
			else if ( i >= BM1 ) { // 2010.01.19
				for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , j ) > 0 ) {
						switch( _i_SCH_IO_GET_MODULE_RESULT( i , j ) ) {
						case WAFERRESULT_SUCCESS :
							ws[ i ][ j - 1 ] = CWM_PRESENT;
							break;
						case WAFERRESULT_FAILURE :
							ws[ i ][ j - 1 ] = CWM_FAILURE;
							break;
						default :
							ws[ i ][ j - 1 ] = CWM_PROCESS;
							break;
						}
					}
					else {
						ws[ i ][ j - 1 ] = CWM_ABSENT;
					}
				}
			}
		}
	}

	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < CM1 ) continue; // 2011.04.14
		//
		if (
			( _i_SCH_CLUSTER_Get_PathDo( side , i ) == PATHDO_WAFER_RETURN ) ||
			( _i_SCH_CLUSTER_Get_PathStatus( side , i ) < SCHEDULER_STARTING ) ) {
			//
			if ( ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < PM1 ) || ( _i_SCH_CLUSTER_Get_PathIn( side , i ) >= BM1 ) ) { // 2009.01.19
				if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) == ch ) {
					c = ws[ _i_SCH_CLUSTER_Get_PathIn( side , i )][ _i_SCH_CLUSTER_Get_SlotIn( side , i ) - 1 ]; // 2002.03.27 // 2010.01.19
					if ( ( c != CWM_PROCESS ) && ( c != CWM_FAILURE ) && ( c != CWM_PRESENT ) ) {
						return 2;
					}
				}
				if ( ( _i_SCH_CLUSTER_Get_PathOut( side , i ) != _i_SCH_CLUSTER_Get_PathIn( side , i ) ) &&
					 ( _i_SCH_CLUSTER_Get_PathOut( side , i ) == ch ) ) {
					c = ws[ _i_SCH_CLUSTER_Get_PathOut( side , i )][ _i_SCH_CLUSTER_Get_SlotOut( side , i ) - 1]; // 2002.03.27 // 2010.01.19
					if ( ( c != CWM_ABSENT ) ) {
						return 3;
					}
				}
			}
		}
		else {
			//
			if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) < CM1 ) continue; // 2011.04.14
			//
			if ( ( _i_SCH_CLUSTER_Get_PathOut( side , i ) < PM1 ) || ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= BM1 ) ) { // 2009.01.19
				if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) == ch ) {
					c = ws[ _i_SCH_CLUSTER_Get_PathOut( side , i )][ _i_SCH_CLUSTER_Get_SlotOut( side , i ) - 1 ]; // 2002.03.27 // 2010.01.19
					if ( ( c != CWM_PROCESS ) && ( c != CWM_FAILURE ) && ( c != CWM_PRESENT ) ) {
						return 4;
					}
				}
				if ( ( _i_SCH_CLUSTER_Get_PathOut( side , i ) != _i_SCH_CLUSTER_Get_PathIn( side , i ) ) &&
					 ( _i_SCH_CLUSTER_Get_PathIn( side , i ) == ch ) ) {
					c = ws[ _i_SCH_CLUSTER_Get_PathIn( side , i )][ _i_SCH_CLUSTER_Get_SlotIn( side , i ) - 1 ]; // 2002.03.27 // 2010.01.19
					if ( ( c != CWM_ABSENT ) ) {
						return 5;
					}
				}
			}
		}
	}
	return 0;
}


//=======================================================================================
/*
// 2014.01.10
int SCHEDULER_CONTROL_PM_Next_Place_Impossible( int ch ) { // 2007.03.12
	int s , p , k;

	if ( _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) return -1;

	s = _i_SCH_MODULE_Get_PM_SIDE( ch , 0 );

	p = _i_SCH_MODULE_Get_PM_POINTER( ch , 0 );

	if ( _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) return -1;

	if ( ( p >= MAX_CASSETTE_SLOT_SIZE ) || ( p < 0 ) ) return -1;

	if ( _i_SCH_CLUSTER_Get_PathDo( s , p ) >= _i_SCH_CLUSTER_Get_PathRange( s , p ) ) return 3;

	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		if ( _i_SCH_CLUSTER_Get_PathProcessChamber( s , p , _i_SCH_CLUSTER_Get_PathDo( s , p ) , k ) > 0 ) return 0;
	}

	_i_SCH_CLUSTER_Inc_PathDo( s , p );
	return 2;
}
*/

// 2014.01.10
//
int SCHEDULER_CONTROL_PM_Next_Place_Impossible( int ch ) { // 2007.03.12
	int i , s , p , k , Res = -1;

	for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;

		s = _i_SCH_MODULE_Get_PM_SIDE( ch , i );

		p = _i_SCH_MODULE_Get_PM_POINTER( ch , i );

		if ( _i_SCH_MODULE_Get_PM_WAFER( ch , i ) <= 0 ) continue;

		if ( ( p >= MAX_CASSETTE_SLOT_SIZE ) || ( p < 0 ) ) continue;

		if ( _i_SCH_CLUSTER_Get_PathDo( s , p ) >= _i_SCH_CLUSTER_Get_PathRange( s , p ) ) Res = 3;

		for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
			if ( _i_SCH_CLUSTER_Get_PathProcessChamber( s , p , _i_SCH_CLUSTER_Get_PathDo( s , p ) , k ) > 0 ) Res = 0;
		}
		//
		_i_SCH_CLUSTER_Inc_PathDo( s , p );
		//
		Res = 2;
	}
	//
	return Res;
	//
}
//=======================================================================================
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Find/Enable/Possible Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//=======================================================================================
//=======================================================================================
int SCHEDULER_CONTROL_Set_Change_Disappear_Status( int side , int pointer ) { // 2006.03.03
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) return 2;
	//
	if ( _i_SCH_CLUSTER_Get_PathRange( side , pointer ) <= 0 ) return 3;
	//
//	_i_SCH_CLUSTER_Set_PathRange( side , pointer , -1 ); // 2008.04.23
	_i_SCH_CLUSTER_Set_PathRange( side , pointer , -5 ); // 2008.04.23
	_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_READY );
	_i_SCH_CLUSTER_Set_PathDo( side , pointer , 0 );
	//
	return 0;
}
//=======================================================================================
BOOL _i_SCH_SUB_DISAPPEAR_OPERATION( int grp , int option ) {
	int i , j , side , wafer , pointer , ar;
	BOOL Find = FALSE;
	int  iosts; // 2018.11.14
	BOOL Reject; // 2018.11.14
	//
	// option		PM/AL/IC/TFM				BMCHECK		BM_FM/TMSIDE CHECK	BM_PICK/PLACE_POSSIBLE(PP)	or GROUP INDEX(GI)
	//
	//		-4		PM/AL/IC CHECK ONLY + GI	NOTUSING	-					-
	//		-3		PM/AL/IC CHECK ONLY + PP	NOTUSING	-					-
	//		-2		ALL+GI						NOTUSING	-					-
	//		-1		ALL+PP						NOTUSING	-					-
	//
	//		0		ALL+PP						USING		O					PP
	//		1		ALL+GI						USING		O					GI
	//		2		ALL+PP						USING		X					PP
	//		3		ALL+GI						USING		X					GI
	//
	if ( SIGNAL_MODE_DISAPPEAR_GET( grp ) ) {
		//
		SIGNAL_MODE_DISAPPEAR_ENTER();
		//
		while ( TRUE ) {
			//
			Reject = FALSE; // 2018.11.14
			//
			if ( grp < 0 ) {
				//=========================================================================================================================================================
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					//=========================================================================================================================================================
					//
					if ( ( option != -3 ) && ( option != -4 ) ) { // 2018.10.25
						//
						for ( ar = 0 ; ar < MAX_FINGER_FM ; ar++ ) {
							//
							if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( 0 , ar ) ) continue;
							//
							side = _i_SCH_MODULE_Get_FM_SIDE( ar );
							wafer = _i_SCH_MODULE_Get_FM_WAFER( ar );
							pointer = _i_SCH_MODULE_Get_FM_POINTER( ar );
							//
							iosts = WAFER_STATUS_IN_FM( 0 , ar );
							//
							if ( wafer <= 0 ) continue;
							if ( iosts > 0 ) continue;
							//
							if ( side != _i_SCH_MODULE_Get_FM_SIDE( ar ) ) { Reject = TRUE; continue; }
							if ( wafer != _i_SCH_MODULE_Get_FM_WAFER( ar ) ) { Reject = TRUE; continue; }
							if ( pointer != _i_SCH_MODULE_Get_FM_POINTER( ar ) ) { Reject = TRUE; continue; }
							if ( iosts != WAFER_STATUS_IN_FM( 0 , ar ) ) { Reject = TRUE; continue; }
							//
							Find = TRUE; // 2002.05.17
							//
		//					_i_SCH_MODULE_Set_FM_SIDE_WAFER( ar , side , 0 ); // 2013.01.11
							_i_SCH_MODULE_Set_FM_WAFER( ar , 0 ); // 2013.01.11
							//
							_i_SCH_MODULE_Inc_FM_InCount( side );
							//=============================================================================
							SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
							//=============================================================================
							_i_SCH_LOG_LOT_PRINTF( side , "FM Disappear at %d[F%c]%cDISAPPEAR FM:%c:%d%c%d\n" , wafer , ar + 'A' , 9 , ar + 'A' , wafer , 9 , wafer );
							//=============================================================================
							// 2002.06.07
							if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
		//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
								_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
							}
						}
						//
					}
					//
					//=========================================================================================================================================================
					for ( i = 1 ; i <= 2 ; i++ ) {
						//
						side = _i_SCH_MODULE_Get_MFALS_SIDE( i );
						wafer = _i_SCH_MODULE_Get_MFALS_WAFER( i );
						pointer = _i_SCH_MODULE_Get_MFALS_POINTER( i );
						//
						iosts = _i_SCH_IO_GET_MODULE_STATUS( ( i == 1 ) ? F_AL : 1001 , 1 );
						//
						if ( wafer <= 0 ) continue;
						if ( iosts > 0 ) continue;
						//
						if ( side != _i_SCH_MODULE_Get_MFALS_SIDE( i ) ) { Reject = TRUE; continue; }
						if ( wafer != _i_SCH_MODULE_Get_MFALS_WAFER( i ) ) { Reject = TRUE; continue; }
						if ( pointer != _i_SCH_MODULE_Get_MFALS_POINTER( i ) ) { Reject = TRUE; continue; }
						if ( iosts != _i_SCH_IO_GET_MODULE_STATUS( ( i == 1 ) ? F_AL : 1001 , 1 ) ) { Reject = TRUE; continue; }
						//
						Find = TRUE;
						//
						_i_SCH_MODULE_Set_MFALS_WAFER( i , 0 );
						//
						_i_SCH_MODULE_Inc_FM_InCount( side );
						//=============================================================================
						SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
						//=============================================================================
						_i_SCH_LOG_LOT_PRINTF( side , "MFAL%d Disappear at %d%cDISAPPEAR MFAL%d:%d%c%d\n" , i , wafer , 9 , i , wafer , 9 , wafer );
						//=============================================================================
						if ( pointer < MAX_CASSETTE_SLOT_SIZE ) {
	//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
						}
					}
					//=========================================================================================================================================================
					if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() != 0 ) { // 2010.12.22
						for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) ; i++ ) {
							//
							side = _i_SCH_MODULE_Get_MFIC_SIDE( i );
							wafer = _i_SCH_MODULE_Get_MFIC_WAFER( i );
							pointer = _i_SCH_MODULE_Get_MFIC_POINTER( i );
							//
							iosts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , i );
							//
							if ( wafer <= 0 ) continue;
							if ( iosts > 0 ) continue;
							//
							if ( side != _i_SCH_MODULE_Get_MFIC_SIDE( i ) ) { Reject = TRUE; continue; }
							if ( wafer != _i_SCH_MODULE_Get_MFIC_WAFER( i ) ) { Reject = TRUE; continue; }
							if ( pointer != _i_SCH_MODULE_Get_MFIC_POINTER( i ) ) { Reject = TRUE; continue; }
							if ( iosts != _i_SCH_IO_GET_MODULE_STATUS( F_IC , i ) ) { Reject = TRUE; continue; }
							//
							Find = TRUE;
							//
							_i_SCH_MODULE_Set_MFIC_WAFER( i , 0 );
							//
							_i_SCH_MODULE_Inc_FM_InCount( side );
							//=============================================================================
							SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
							//=============================================================================
							_i_SCH_LOG_LOT_PRINTF( side , "MFIC%d Disappear at %d%cDISAPPEAR MFIC%d:%d%c%d\n" , i , wafer , 9 , i , wafer , 9 , wafer );
							//=============================================================================
							if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
	//							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
								_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
							}
						}
					}
					//=========================================================================================================================================================
					//
					if ( option >= 0 ) { // 2018.10.25
						//
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							//
							if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2009.05.21
							//
							// option		PM/AL/IC/TFM				BMCHECK		BM_FM/TMSIDE CHECK	BM_PICK/PLACE_POSSIBLE(PP)	or GROUP INDEX(GI)
							//
							//		-4		PM/AL/IC CHECK ONLY + GI	NOTUSING	-					-
							//		-3		PM/AL/IC CHECK ONLY + PP	NOTUSING	-					-
							//		-2		ALL+GI						NOTUSING	-					-
							//		-1		ALL+PP						NOTUSING	-					-
							//
							//		0		ALL+PP						USING		O					PP
							//		1		ALL+GI						USING		O					GI
							//		2		ALL+PP						USING		X					PP
							//		3		ALL+GI						USING		X					GI
							//
							if      ( option == 1 ) { // 2018.10.25
								//
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _i_SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
								//
							}
							else if ( option == 2 ) { // 2018.10.25
								//
								if ( !_i_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) && !_i_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2009.05.21 // 2018.10.25
								//
							}
							else if ( option == 3 ) { // 2018.10.25
								continue;
							}
							else { // 0:default
								//
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _i_SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) ) continue;
								if ( !_i_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) && !_i_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2009.05.21 // 2018.10.25
								//
							}
							//
			//				if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue; // 2009.05.21
	//						if ( !_i_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) && !_i_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PLACE , 0 ) ) continue; // 2009.05.21 // 2018.10.25
							//
							for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
								//
								side = _i_SCH_MODULE_Get_BM_SIDE( i , j );
								wafer = _i_SCH_MODULE_Get_BM_WAFER( i , j );
								pointer = _i_SCH_MODULE_Get_BM_POINTER( i , j );
								//
								iosts = _i_SCH_IO_GET_MODULE_STATUS( i , j );
								//
								if ( wafer <= 0 ) continue;
								if ( iosts > 0 ) continue;
								//
								if ( side != _i_SCH_MODULE_Get_BM_SIDE( i , j ) ) { Reject = TRUE; continue; }
								if ( wafer != _i_SCH_MODULE_Get_BM_WAFER( i , j ) ) { Reject = TRUE; continue; }
								if ( pointer != _i_SCH_MODULE_Get_BM_POINTER( i , j ) ) { Reject = TRUE; continue; }
								if ( iosts != _i_SCH_IO_GET_MODULE_STATUS( i , j ) ) { Reject = TRUE; continue; }
								//
								Find = TRUE;
								//
								_i_SCH_MODULE_Inc_FM_InCount( side );
								//==================================================================================
								_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , -1 );
								//=============================================================================
								SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer );
								//=============================================================================
								_i_SCH_LOG_LOT_PRINTF( side , "%s(%d) Disappear(FM) at %d%cDISAPPEAR %s:%d:%d%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , j , wafer , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , j , wafer , 9 , wafer );
								//=============================================================================
								if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
		//							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
									_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
								}
								//
							}
						}
						//
					}
					//
					//=========================================================================================================================================================
				}
			}
			else {
				//=========================================================================================================================================================
				if ( ( grp >= 0 ) && ( grp < MAX_TM_CHAMBER_DEPTH ) ) {
					//=========================================================================================================================================================
					if ( _i_SCH_PRM_GET_MODULE_MODE( TM + grp ) ) {
						//=========================================================================================================================================================
						if ( ( option != -3 ) && ( option != -4 ) ) { // 2018.10.25
							//
							for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
								//
								if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
								//
								if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( grp , i ) ) continue;
								//
								side = _i_SCH_MODULE_Get_TM_SIDE( grp , i );
								wafer = _i_SCH_MODULE_Get_TM_WAFER( grp , i );
								pointer = _i_SCH_MODULE_Get_TM_POINTER( grp , i );
								//
								iosts = WAFER_STATUS_IN_TM( grp , i );
								//
								if ( wafer <= 0 ) continue;
								if ( iosts > 0 ) continue;
								//
								if ( side != _i_SCH_MODULE_Get_TM_SIDE( grp , i ) ) { Reject = TRUE; continue; }
								if ( wafer != _i_SCH_MODULE_Get_TM_WAFER( grp , i ) ) { Reject = TRUE; continue; }
								if ( pointer != _i_SCH_MODULE_Get_TM_POINTER( grp , i ) ) { Reject = TRUE; continue; }
								if ( iosts != WAFER_STATUS_IN_TM( grp , i ) ) { Reject = TRUE; continue; }
								//
								Find = TRUE; // 2002.05.17
								//
								_i_SCH_MODULE_Set_TM_WAFER( grp , i , 0 );
								//
								if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_InCount( side ); // 2009.05.22
								//
								_i_SCH_MODULE_Inc_TM_InCount( side );
								//=============================================================================
								SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
								//=============================================================================
								if ( grp == 0 ) {
									_i_SCH_LOG_LOT_PRINTF( side , "TM Disappear at %d[F%c]%cDISAPPEAR TM:%c:%d%c%d\n" , wafer , i + 'A' , 9 , i + 'A' , wafer , 9 , wafer );
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( side , "TM%d Disappear at %d[F%c]%cDISAPPEAR TM%d:%c:%d%c%d\n" , grp + 1 , wafer , i + 'A' , 9 , grp + 1 , i + 'A' , wafer , 9 , wafer );
								}
								//=============================================================================
								// 2002.06.07
								if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
		//							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
									_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
								}
							}
							//
						}
						//=========================================================================================================================================================
						//
						// option		PM/AL/IC/TFM				BMCHECK		BM_FM/TMSIDE CHECK	BM_PICK/PLACE_POSSIBLE(PP)	or GROUP INDEX(GI)
						//
						//		-4		PM/AL/IC CHECK ONLY + GI	NOTUSING	-					-
						//		-3		PM/AL/IC CHECK ONLY + PP	NOTUSING	-					-
						//		-2		ALL+GI						NOTUSING	-					-
						//		-1		ALL+PP						NOTUSING	-					-
						//
						//		0		ALL+PP						USING		O					PP
						//		1		ALL+GI						USING		O					GI
						//		2		ALL+PP						USING		X					PP
						//		3		ALL+GI						USING		X					GI
						//
						for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
							//
							if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2009.05.21
							//
							switch( option ) { // 2018.10.25
							case 1 :
							case 3 :
							case -2 :
							case -4 :
								if ( _i_SCH_PRM_GET_MODULE_GROUP( i ) != grp ) continue;
								break;
							default :
	//							if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( grp , i , G_PICK ) && !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( grp , i , G_PLACE ) ) continue; // 2018.11.14
								if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE_EX( grp , i , G_PICK , FALSE ) ) continue; // 2018.11.14
								break;
							}
							//
							/*
							//
							// 2014.01.10
							//
							if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) <= 0 ) continue;
							if ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) continue;
							//
							Find = TRUE; // 2002.05.17
							side = _i_SCH_MODULE_Get_PM_SIDE( i , 0 );
							wafer = _i_SCH_MODULE_Get_PM_WAFER( i , 0 );
							pointer = _i_SCH_MODULE_Get_PM_POINTER( i , 0 );
							_i_SCH_MODULE_Set_PM_WAFER( i , 0 , 0 );
							//
							if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_InCount( side ); // 2009.05.22
							_i_SCH_MODULE_Inc_TM_InCount( side );
							//=============================================================================
							SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
							//=============================================================================
							_i_SCH_LOG_LOT_PRINTF( side , "%s Disappear at %d%cDISAPPEAR %s:%d%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , wafer , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , wafer , 9 , wafer );
							//=============================================================================
							// 2002.06.07
							if ( pointer < MAX_CASSETTE_SLOT_SIZE ) {
	//								_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
								_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
							}
							*/
							// 2014.01.10
							//
							for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
								//
								side = _i_SCH_MODULE_Get_PM_SIDE( i , j );
								wafer = _i_SCH_MODULE_Get_PM_WAFER( i , j );
								pointer = _i_SCH_MODULE_Get_PM_POINTER( i , j );
								//
								iosts = _i_SCH_IO_GET_MODULE_STATUS( i , j+1 );
								//
								if ( wafer <= 0 ) continue;
								if ( iosts > 0 ) continue;
								//
								if ( side != _i_SCH_MODULE_Get_PM_SIDE( i , j ) ) { Reject = TRUE; continue; }
								if ( wafer != _i_SCH_MODULE_Get_PM_WAFER( i , j ) ) { Reject = TRUE; continue; }
								if ( pointer != _i_SCH_MODULE_Get_PM_POINTER( i , j ) ) { Reject = TRUE; continue; }
								if ( iosts != _i_SCH_IO_GET_MODULE_STATUS( i , j+1 ) ) { Reject = TRUE; continue; }
								//
								Find = TRUE; // 2002.05.17
								//
								_i_SCH_MODULE_Set_PM_WAFER( i , j , 0 );
								//
								if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_InCount( side ); // 2009.05.22
								//
								_i_SCH_MODULE_Inc_TM_InCount( side );
								//=============================================================================
								SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
								//=============================================================================
								_i_SCH_LOG_LOT_PRINTF( side , "%s Disappear at %d%cDISAPPEAR %s:%d%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , wafer , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , wafer , 9 , wafer );
								//=============================================================================
								if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
									_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
								}
							}
							//
						}
						//=========================================================================================================================================================
						if ( option >= 0 ) { // 2018.10.25
							//
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								//
								if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2009.05.21
								//
								// option		PM/AL/IC/TFM				BMCHECK		BM_FM/TMSIDE CHECK	BM_PICK/PLACE_POSSIBLE(PP)	or GROUP INDEX(GI)
								//
								//		-4		PM/AL/IC CHECK ONLY + GI	NOTUSING	-					-
								//		-3		PM/AL/IC CHECK ONLY + PP	NOTUSING	-					-
								//		-2		ALL+GI						NOTUSING	-					-
								//		-1		ALL+PP						NOTUSING	-					-
								//
								//		0		ALL+PP						USING		O					PP
								//		1		ALL+GI						USING		O					GI
								//		2		ALL+PP						USING		X					PP
								//		3		ALL+GI						USING		X					GI
								//
								if      ( option == 1 ) { // 2018.10.25
									//
									if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _i_SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
									if ( _i_SCH_PRM_GET_MODULE_GROUP( i ) != grp ) continue;
									//
								}
								else if ( option == 2 ) { // 2018.10.25
									//
	//								if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( grp , i , G_PICK , G_MCHECK_ALL ) && !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( grp , i , G_PLACE , G_MCHECK_ALL ) ) continue; // 2018.11.14
									if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE_EX( grp , i , G_PICK , G_MCHECK_ALL , FALSE ) ) continue; // 2018.11.14
									//
								}
								else if ( option == 3 ) { // 2018.10.25
									//
									if ( _i_SCH_PRM_GET_MODULE_GROUP( i ) != grp ) continue;
									//
								}
								else { // 0:default
									//
									if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) && ( _i_SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_TM_STATION ) ) continue;
	//								if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( grp , i , G_PICK , G_MCHECK_ALL ) && !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( grp , i , G_PLACE , G_MCHECK_ALL ) ) continue; // 2018.11.14
									if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE_EX( grp , i , G_PICK , G_MCHECK_ALL , FALSE ) ) continue; // 2018.11.14
									//
								}
								//
								//
	//							if ( !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( grp , i , G_PICK , G_MCHECK_ALL ) && !_i_SCH_MODULE_GROUP_TBM_POSSIBLE( grp , i , G_PLACE , G_MCHECK_ALL ) ) continue; // 2018.10.25
								//
								for ( j = 1 ; j <= _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									//
									side = _i_SCH_MODULE_Get_BM_SIDE( i , j );
									wafer = _i_SCH_MODULE_Get_BM_WAFER( i , j );
									pointer = _i_SCH_MODULE_Get_BM_POINTER( i , j );
									//
									iosts = _i_SCH_IO_GET_MODULE_STATUS( i , j );
									//
									if ( wafer <= 0 ) continue;
									if ( iosts > 0 ) continue;
									//
									if ( side != _i_SCH_MODULE_Get_BM_SIDE( i , j ) ) { Reject = TRUE; continue; }
									if ( wafer != _i_SCH_MODULE_Get_BM_WAFER( i , j ) ) { Reject = TRUE; continue; }
									if ( pointer != _i_SCH_MODULE_Get_BM_POINTER( i , j ) ) { Reject = TRUE; continue; }
									if ( iosts != _i_SCH_IO_GET_MODULE_STATUS( i , j ) ) { Reject = TRUE; continue; }
									//
									Find = TRUE; // 2002.05.17
									//
									if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) _i_SCH_MODULE_Inc_FM_InCount( side ); // 2009.05.22
									//==================================================================================
									if ( grp != 0 ) _i_SCH_MODULE_Inc_TM_InCount( side );
									//==================================================================================
									_i_SCH_MODULE_Set_BM_WAFER_STATUS( i , j , 0 , -1 );
									//=============================================================================
									SCHEDULER_CONTROL_Set_Change_Disappear_Status( side , pointer ); // 2006.03.03
									//=============================================================================
									_i_SCH_LOG_LOT_PRINTF( side , "%s(%d) Disappear at %d%cDISAPPEAR %s:%d:%d%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , j , wafer , 9 , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , j , wafer , 9 , wafer );
									//=============================================================================
									if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
	//									_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
										_i_SCH_LOG_TIMER_PRINTF( side , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , pointer , -1 , -1 , "" , "" ); // 2012.02.18
									}
									//=============================================================================
								}
							}
						}
						//=========================================================================================================================================================
					}
				}
			}
			//
			if ( !Reject ) break; // 2018.11.14
			//
		} // while
		//
		SIGNAL_MODE_DISAPPEAR_CLEAR( grp );
		//
		return Find; // 2002.05.17
	}
	return FALSE;
}
//=======================================================================================
BOOL _i_SCH_SUB_FMTMSIDE_OPERATION( int side , int where ) {
	int i , fmtmdata , res = FALSE , s;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		fmtmdata = SIGNAL_MODE_FMTMSIDE_GET( FALSE , i );
		//
		if ( fmtmdata == -1 ) continue;
		//
		if ( where == 0 ) { // fm
			if ( fmtmdata == BUFFER_FM_STATION ) continue;
		}
		else { // tm
			if ( fmtmdata == BUFFER_TM_STATION ) continue;
		}
		//
		if ( _i_SCH_MODULE_Get_BM_FULL_MODE( i ) == fmtmdata ) continue;
		//
		if ( PROCESS_MONITOR_Run_and_Get_Status( i ) > 0 ) continue;
		//
		_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( fmtmdata , side , i , -1 , TRUE , where , 9999 );
		//
//		SIGNAL_MODE_FMTMSIDE_SET( i , -1 , NULL , -1 ); // 2018.06.22
		SIGNAL_MODE_FMTMSIDE_SET( i , -1 , NULL , -1 , 0 ); // 2018.06.22
		//
		res = TRUE;
	}
	//
/*
//2010.03.03
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
			if ( !_i_SCH_MODULE_Chk_TM_Free_Status( i ) ) break;
		}
	}
	if ( i == MAX_CASSETTE_SIDE ) { // free
		if ( where == 1 ) return res;
	}
	else {
		if ( where == 0 ) return res;
	}
*/
	if ( where == 0 ) return res; // 2010.03.06
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( where - 1 , i , G_PICK ) || !_i_SCH_MODULE_GROUP_TPM_POSSIBLE( where - 1 , i , G_PLACE ) ) continue; // 2016.11.03
		//
		fmtmdata = SIGNAL_MODE_FMTMSIDE_GET( FALSE , i );
		//
		if ( fmtmdata == -1 ) continue;
		//
//		if ( SIGNAL_MODE_FMTMSIDE_GET( TRUE , i ) == 0 ) {
			//
//			if ( PROCESS_MONITOR_Run_and_Get_Status( i ) > 0 ) continue; // 2012.08.22
			//
//			if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
			if ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) > 0 ) { // 2014.01.10
//				SIGNAL_MODE_FMTMSIDE_SET( i , 1001 , NULL );
				continue;
			}
			//
//			if ( PROCESS_MONITOR_Run_and_Get_Status( i ) > 0 ) continue; // 2012.08.22
			//
//			if ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2012.08.22
//				SIGNAL_MODE_FMTMSIDE_SET( i , 1001 , NULL ); // 2012.08.22
//				continue; // 2012.08.22
//			} // 2012.08.22
			//
			if ( SIGNAL_MODE_FMTMRECIPE_GET( i ) != NULL ) {
				//
				if ( _i_SCH_EQ_PROCESS_FUNCTION_STATUS( fmtmdata , i , FALSE ) == SYS_RUNNING ) {
					//
					// 2012.08.22
					//
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( fmtmdata , i ) != MUF_01_USE ) || ( _i_SCH_MODULE_Chk_TM_Free_Status( fmtmdata ) ) ) { // 2016.12.01
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( fmtmdata , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Chk_TM_Free_Status( fmtmdata ) ) ) { // 2016.12.01
						//
						_i_SCH_LOG_LOT_PRINTF( fmtmdata , "PM SEt Process Append at %s[%s]%cPROCESS_SET_START PM%d:::%s:::101\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , SIGNAL_MODE_FMTMRECIPE_GET( i ) , 9 , i - PM1 + 1 , SIGNAL_MODE_FMTMRECIPE_GET( i ) ); // 2016.11.03
						//
						if ( _i_SCH_EQ_SPAWN_EVENT_PROCESS( fmtmdata , -1 , i , 0 , 0 , 0 , SIGNAL_MODE_FMTMRECIPE_GET( i ) , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) {
							//
							s =	SIGNAL_MODE_FMTMPRESKIPSIDE_GET( i ); // 2014.12.18
							//
							if ( s != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( s , i ); // 2014.12.18
							//
//							Scheduler_PreCheck_for_ProcessRunning_Part( fmtmdata , i ); // 2018.06.22
							Scheduler_PreCheck_for_ProcessRunning_Part( fmtmdata , i , SIGNAL_MODE_FMTMRECIPE_GET( i ) , SIGNAL_MODE_FMTMEXTMODE_GET( i ) ); // 2018.06.22
							//
//							SIGNAL_MODE_FMTMSIDE_SET( i , -1 , NULL , -1 ); // 2018.06.22
							SIGNAL_MODE_FMTMSIDE_SET( i , -1 , NULL , -1 , 0 ); // 2018.06.22
							//
						}
					}
					//
					continue;
				}
				//
				_i_SCH_LOG_LOT_PRINTF( fmtmdata , "PM SEt Process Start at %s[%s]%cPROCESS_SET_START PM%d:::%s:::102\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , SIGNAL_MODE_FMTMRECIPE_GET( i ) , 9 , i - PM1 + 1 , SIGNAL_MODE_FMTMRECIPE_GET( i ) ); // 2016.11.03
				//
				if ( _i_SCH_EQ_SPAWN_PROCESS( fmtmdata , -1 , i , 0 , 0 , 0 , SIGNAL_MODE_FMTMRECIPE_GET( i ) , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) {
					//
					s =	SIGNAL_MODE_FMTMPRESKIPSIDE_GET( i ); // 2014.12.18
					//
					if ( s != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( s , i ); // 2014.12.18
					//
//					Scheduler_PreCheck_for_ProcessRunning_Part( fmtmdata , i ); // 2018.06.22
					Scheduler_PreCheck_for_ProcessRunning_Part( fmtmdata , i , SIGNAL_MODE_FMTMRECIPE_GET( i ) , SIGNAL_MODE_FMTMEXTMODE_GET( i ) ); // 2018.06.22
				}
			}
//		}
		//
//		SIGNAL_MODE_FMTMSIDE_SET( i , -1 , NULL , -1 ); // 2018.06.22
		SIGNAL_MODE_FMTMSIDE_SET( i , -1 , NULL , -1 , 0 ); // 2018.06.22
		//
		res = TRUE;
	}
	return res;
}
//=======================================================================================
//=======================================================================================
//=======================================================================================
int SCHEDULING_CHECK_Change_DataBase_PM_Enable_other_Disable( int CHECKING_SIDE , char *slotmodestring , char *ordermodestring , int pm , char *pmstr , int ReadyNotCheck ) { // 2005.11.25
	int i , j , k , l , len , len2 , len3 , s , m , crcheck , pmck[MAX_PM_CHAMBER_DEPTH];
	len = strlen( slotmodestring );
	len2 = strlen( ordermodestring );
	len3 = strlen( pmstr );
//printf( "=========================================================\n" );
//printf( "[SIDE=%d][Slot=%s(%d)][or=%s(%d)][pm=%s(%d)][pm=%d,rd=%d]\n" , CHECKING_SIDE , slotmodestring , len , ordermodestring , len2 , pmstr , len3 , pm , ReadyNotCheck );
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
			if ( ( ReadyNotCheck == 1 ) || ( ReadyNotCheck == 3 ) || ( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) == SCHEDULER_READY ) ) {
				if ( len == 0 ) {
					crcheck = TRUE;
				}
				else {
					crcheck = FALSE;
					j = _i_SCH_CLUSTER_Get_SlotIn( CHECKING_SIDE , i );
					for ( k = 0 ; k < len ; k++ ) {
						if      ( ( slotmodestring[k] >= '1' ) && ( slotmodestring[k] <= '9' ) ) {
							if ( ( slotmodestring[k] - '0' ) == j ) {
								crcheck = TRUE;
								break;
							}
						}
						else if ( ( slotmodestring[k] >= 'A' ) && ( slotmodestring[k] <= 'Z' ) ) {
							if ( ( slotmodestring[k] - 'A' + 10 ) == j ) {
								crcheck = TRUE;
								break;
							}
						}
						else if ( ( slotmodestring[k] >= 'a' ) && ( slotmodestring[k] <= 'z' ) ) {
							if ( ( slotmodestring[k] - 'a' + 10 ) == j ) {
								crcheck = TRUE;
								break;
							}
						}
					}
				}
				if ( crcheck ) {
//printf( "A.[i=%d][j=%d][k=%d]\n" , i , j , k );
					for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
						crcheck = FALSE;
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) { // 2006.02.08
							pmck[k] = FALSE;
						}
						for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
							m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
							if ( pm == 0 ) {
								//=================
								for ( l = 0 ; l < len3 ; l++ ) {
									if      ( ( pmstr[l] >= '1' ) && ( pmstr[l] <= '9' ) ) {
										s = pmstr[l] - '0' - 1;
									}
									else if ( ( pmstr[l] >= 'A' ) && ( pmstr[l] <= 'Z' ) ) {
										s = pmstr[l] - 'A' + 10 - 1;
									}
									else if ( ( pmstr[l] >= 'a' ) && ( pmstr[l] <= 'z' ) ) {
										s = pmstr[l] - 'a' + 10 - 1;
									}
									else { // 2007.07.02
										s = -1; // 2007.07.02
									} // 2007.07.02
									if ( ( s >= 0 ) && ( s < MAX_PM_CHAMBER_DEPTH ) ) {
										if ( ( m == ( s + PM1 ) ) || ( -m == ( s + PM1 ) ) ) {
//printf( "B.[i=%d][j=%d][k=%d]\n" , i , j , k );
											pmck[ s ] = TRUE;
											crcheck = TRUE;
										}
									}
								}
								//=================
							}
							else {
								if ( ( m == pm ) || ( -m == pm ) ) {
//printf( "C.[i=%d][j=%d][k=%d]\n" , i , j , k );
									pmck[ pm - PM1 ] = TRUE;
									crcheck = TRUE;
									break;
								}
							}
						}
						if ( crcheck ) {
//printf( "D.[i=%d][j=%d][k=%d]\n" , i , j , k );
							if ( len2 > 0 ) {
								crcheck = FALSE;
								//=================
								for ( k = 0 ; k < len2 ; k++ ) {
									if      ( ( ordermodestring[k] >= '1' ) && ( ordermodestring[k] <= '9' ) ) {
										if ( ( ordermodestring[k] - '0' ) == ( j + 1 ) ) {
											crcheck = TRUE;
											break;
										}
									}
									else if ( ( ordermodestring[k] >= 'A' ) && ( ordermodestring[k] <= 'Z' ) ) {
										if ( ( ordermodestring[k] - 'A' + 10 ) == ( j + 1 ) ) {
											crcheck = TRUE;
											break;
										}
									}
									else if ( ( ordermodestring[k] >= 'a' ) && ( ordermodestring[k] <= 'z' ) ) {
										if ( ( ordermodestring[k] - 'a' + 10 ) == ( j + 1 ) ) {
											crcheck = TRUE;
											break;
										}
									}
								}
								//================= 
							}
							if ( crcheck ) {
//printf( "E.[i=%d][j=%d][k=%d][%d,%d,%d,%d]\n" , i , j , k , pmck[0] , pmck[1] , pmck[2] , pmck[3] );
								for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
									m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
									//
//printf( "F.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
									if ( m < 0 ) {
										if ( pmck[ -m - PM1 ] ) {
//printf( "G.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
											_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( CHECKING_SIDE , i , j , k );
										}
										else {
//printf( "G2.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
											if ( ( ReadyNotCheck == 2 ) || ( ReadyNotCheck == 3 ) ) { // 2007.07.02
//printf( "G3.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
												_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( CHECKING_SIDE , i , j , k );
											}
										}
									}
									else {
										if ( !pmck[ m - PM1 ] ) {
//printf( "H.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
											if ( ( ReadyNotCheck == 2 ) || ( ReadyNotCheck == 3 ) ) { // 2007.07.02
//printf( "H2.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
												_i_SCH_CLUSTER_Set_PathProcessChamber_Delete( CHECKING_SIDE , i , j , k );
											}
											else {
//printf( "H3.[i=%d][j=%d][k=%d][m=%d]\n" , i , j , k , m );
												_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( CHECKING_SIDE , i , j , k );
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
//printf( "=========================================================\n" );
	return 0;
}





int _i_SCH_SUB_RECIPE_READ_OPERATION( int side ) {
	int Res;
	int filetype;
	int callstyle;
	char filename[64];

	if ( !SIGNAL_MODE_RECIPE_READ_GET( side , &filetype , &callstyle , filename ) ) return -1;

	SIGNAL_MODE_RECIPE_READ_CLEAR( side );

	//
	EnterCriticalSection( &CR_MAIN ); // 2011.03.28
	//
	if ( filetype == 0 ) { // lot
		Res = Scheduler_CONTROL_RECIPE_SETTING_FROM_LOT_RECIPE( callstyle , side , filename );
	}
	else { // cluster
		Res = Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( 0 , callstyle , 1 , side , -1 , -1 , filetype , filename , NULL );
	}
	//
	LeaveCriticalSection( &CR_MAIN ); // 2011.03.28
	//
	return Res;
}


int _i_SCH_SUB_RECIPE_READ_CLUSTER( int side , int pt , int mdlcheck , int incm , int slot , char *clusterRecipefile ) { // 2010.11.09
	int Res;
	//
	EnterCriticalSection( &CR_MAIN ); // 2011.03.28
	//
//	return Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( 1 , -1 , mdlcheck , side , pt , incm , slot , clusterRecipefile , NULL ); // 2011.03.28
	Res = Scheduler_CONTROL_RECIPE_SETTING_FROM_CLUSTER_RECIPE( 1 , -1 , mdlcheck , side , pt , incm , slot , clusterRecipefile , NULL ); // 2011.03.28
	//
	LeaveCriticalSection( &CR_MAIN ); // 2011.03.28
	//
	return Res;
}


void Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_REMAPPING_WITH_SELECT( int CHECKING_SIDE , int Pt , int Select_hSide , int org_hSide );

void Scheduler_HSide_Remap( int side , int pt , int hside_Aarm ) { // 2018.11.22
	int sts , hgd , iniv , mode;
	//
	sts = _i_SCH_CLUSTER_Get_PathHSide( side , pt ) % 1000000;
	//
	if ( sts != 9 ) return;
	//
	hgd = _i_SCH_CLUSTER_Get_PathHSide( side , pt ) / 1000000;
	//
	iniv = hgd / 10;
	mode = hgd % 10;
	//
	switch( mode ) {
	case 1 : // 1:MIX_AB
		//
		if      ( _in_HSide_Last_Pointer[side] == HANDLING_A_SIDE_ONLY ) { // a
			_in_HSide_Last_Pointer[side] = HANDLING_B_SIDE_ONLY;
			sts = HANDLING_B_SIDE_ONLY;
		}
		else if (_in_HSide_Last_Pointer[side] == HANDLING_B_SIDE_ONLY ) { // b
			_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
			sts = HANDLING_A_SIDE_ONLY;
		}
		else if (_in_HSide_Last_Pointer[side] == HANDLING_ALL_SIDE ) { // l1
			_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
			sts = HANDLING_A_SIDE_ONLY;
		}
		else if (_in_HSide_Last_Pointer[side] == 3 ) { // l2
			_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
			sts = HANDLING_A_SIDE_ONLY;
		}
		else {
			_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
			sts = HANDLING_A_SIDE_ONLY;
		}
		//		
		break;
	case 2 : // 2:MIX_AL
		//
		if ( hside_Aarm == HANDLING_ALL_SIDE ) {
			_in_HSide_Last_Pointer[side] = 3;
			sts = HANDLING_ALL_SIDE;
		}
		else {
			if      (_in_HSide_Last_Pointer[side] == HANDLING_A_SIDE_ONLY ) { // a
				_in_HSide_Last_Pointer[side] = HANDLING_ALL_SIDE;
				sts = HANDLING_ALL_SIDE;
			}
			else if (_in_HSide_Last_Pointer[side] == HANDLING_B_SIDE_ONLY ) { // b
				_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
				sts = HANDLING_A_SIDE_ONLY;
			}
			else if (_in_HSide_Last_Pointer[side] == HANDLING_ALL_SIDE ) { // l1
				_in_HSide_Last_Pointer[side] = 3;
				sts = HANDLING_ALL_SIDE;
			}
			else if (_in_HSide_Last_Pointer[side] == 3 ) { // l2
				_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
				sts = HANDLING_A_SIDE_ONLY;
			}
			else {
				_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
				sts = HANDLING_A_SIDE_ONLY;
			}
		}
		//		
		break;
	case 3 : // 3:MIX_BL
		//
		if ( hside_Aarm == HANDLING_ALL_SIDE ) {
			_in_HSide_Last_Pointer[side] = 3;
			sts = HANDLING_ALL_SIDE;
		}
		else {
			if      (_in_HSide_Last_Pointer[side] == HANDLING_A_SIDE_ONLY ) { // a
				_in_HSide_Last_Pointer[side] = HANDLING_B_SIDE_ONLY;
				sts = HANDLING_B_SIDE_ONLY;
			}
			else if (_in_HSide_Last_Pointer[side] == HANDLING_B_SIDE_ONLY ) { // b
				_in_HSide_Last_Pointer[side] = HANDLING_ALL_SIDE;
				sts = HANDLING_ALL_SIDE;
			}
			else if (_in_HSide_Last_Pointer[side] == HANDLING_ALL_SIDE ) { // l1
				_in_HSide_Last_Pointer[side] = 3;
				sts = HANDLING_ALL_SIDE;
			}
			else if (_in_HSide_Last_Pointer[side] == 3 ) { // l2
				_in_HSide_Last_Pointer[side] = HANDLING_B_SIDE_ONLY;
				sts = HANDLING_B_SIDE_ONLY;
			}
			else {
				_in_HSide_Last_Pointer[side] = HANDLING_B_SIDE_ONLY;
				sts = HANDLING_B_SIDE_ONLY;
			}
		}
		//		
		break;
	case 4 : // 4:MIX_ABL
		//
		if ( hside_Aarm == HANDLING_ALL_SIDE ) {
			_in_HSide_Last_Pointer[side] = 3;
			sts = HANDLING_ALL_SIDE;
		}
		else {
			if      (_in_HSide_Last_Pointer[side] == HANDLING_A_SIDE_ONLY ) { // a
				_in_HSide_Last_Pointer[side] = HANDLING_B_SIDE_ONLY;
				sts = HANDLING_B_SIDE_ONLY;
			}
			else if (_in_HSide_Last_Pointer[side] == HANDLING_B_SIDE_ONLY ) { // b
				_in_HSide_Last_Pointer[side] = HANDLING_ALL_SIDE;
				sts = HANDLING_ALL_SIDE;
			}
			else if (_in_HSide_Last_Pointer[side] == HANDLING_ALL_SIDE ) { // l1
				_in_HSide_Last_Pointer[side] = 3;
				sts = HANDLING_ALL_SIDE;
			}
			else if (_in_HSide_Last_Pointer[side] == 3 ) { // l2
				_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
				sts = HANDLING_A_SIDE_ONLY;
			}
			else {
				_in_HSide_Last_Pointer[side] = HANDLING_A_SIDE_ONLY;
				sts = HANDLING_A_SIDE_ONLY;
			}
		}
		//		
		break;
	default :
		_in_HSide_Last_Pointer[side] = -1;
		sts = HANDLING_ALL_SIDE;
		break;
	}
	//
	Scheduler_CONTROL_RECIPE_DOUBLE_SIDE_REMAPPING_WITH_SELECT( side , pt , sts , iniv );
	//
	_i_SCH_CLUSTER_Set_PathHSide( side , pt , ( hgd * 1000000 ) + sts );
	//	
}
