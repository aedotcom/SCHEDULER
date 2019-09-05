//================================================================================
//
// CUSTOMIZE Project
//
//================================================================================
#include "default.h"

#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "sch_CassetteResult.h"
#include "sch_CassetteSupply.h"
#include "sch_sub.h"
#include "sch_EIL.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm.h"
#include "sch_ReStart.h"
#include "iolog.h"
#include "IO_Part_data.h"

#include "INF_sch_CommonUser.h"
#include "INF_sch_macro.h"

#include "Dll_Interface.h"

#ifdef _SCH_MIF
#else
	#include "sch_A0_Main.h"
	#include "sch_A4_Main.h"
	#include "sch_A6_Main.h"

	//===============================================================================
	int SYSTEM_RUN_ALG_STYLE_GET();
	//===============================================================================

#endif

//===============================================================================
extern int  IO_DEBUG_MONITOR_STATUS;
//===============================================================================

int _i_SCH_MODULE_GET_USE_ENABLE_Sub( int Chamber , BOOL Always , int side );

int _i_SCH_USER_POSSIBLE_PICK_FROM_FM( int side , int *pt ); // 2011.12.08
int _i_SCH_USER_POSSIBLE_PICK_FROM_TM( int side , int *pt ); // 2011.12.08

//===============================================================================
//===============================================================================
int _Trace_Sleeping_Count[MAX_CASSETTE_SIDE]; // 2012.02.07
int _Trace_TM_Count[MAX_CASSETTE_SIDE]; // 2012.02.07

int _Trace_FEM_Direct_Side;
int _Trace_TM_Direct_Side[MAX_TM_CHAMBER_DEPTH];
//
int _Trace_FEM_Other_RunPart_Abort_Check[MAX_CASSETTE_SIDE]; // 2010.07.02
//
//int _Trace_FEM_Wait_Last_Entering; // 2011.06.24 // 2011.10.11
//int _Trace_FEM_Wait_Entering[MAX_CASSETTE_SIDE]; // 2011.06.24 // 2011.10.11
//
int _Trace_FEM_Direct_Side_Mon[MAX_CASSETTE_SIDE];
int _Trace_TM_Direct_Side_Mon[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH];

int _Trace_FM_Flow_Kill[MAX_CASSETTE_SIDE]; // 2012.01.29
int _Trace_TM_Flow_Kill[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH];

//
int _Trace_FM_Flow_In[MAX_CASSETTE_SIDE]; // 2014.03.12
int _Trace_TM_Flow_In[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH]; // 2014.03.12

BOOL _Trace_FM_Flow_Usermode_Lock[MAX_CASSETTE_SIDE]; // 2017.02.27
//===============================================================================
//===============================================================================
void INIT_RUN_COMMON( int apmode , int side ) { // 2008.09.26
	int i;
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		_Trace_FEM_Direct_Side = -1;
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _Trace_TM_Direct_Side[i] = -1;
		//
//		_Trace_FEM_Wait_Last_Entering = -1; // 2011.10.11
//		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _Trace_FEM_Wait_Entering[i] = FALSE; // 2011.10.11
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _Trace_FM_Flow_Usermode_Lock[i] = FALSE; // 2017.02.27
		//
	}
	if ( ( apmode == 1 ) || ( apmode == 3 ) ) { // 2010.07.29
		_Trace_FEM_Other_RunPart_Abort_Check[side] = 0;
	}
}
//===============================================================================
int _Trace_Check_Direct_Side_Invalid( int mode , int side ) {
	// return 0 : Valid
	//        1 : Invalid
	//        2 : Invalid and Warning
	//        3 : Invalid But Warning so Valid
	if ( mode == -1 ) {
		if ( _Trace_FEM_Direct_Side == -1 ) return 0;
		if ( _Trace_FEM_Direct_Side != side ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( _Trace_FEM_Direct_Side ) ) { // 2008.12.12
				_Trace_FEM_Direct_Side = -1;
				return 0;
			}
			//
			if      ( ( _Trace_FEM_Direct_Side_Mon[_Trace_FEM_Direct_Side] - _Trace_FEM_Direct_Side_Mon[side] ) >= 30 ) {
				return 3;
			}
			else if ( ( _Trace_FEM_Direct_Side_Mon[_Trace_FEM_Direct_Side] - _Trace_FEM_Direct_Side_Mon[side] ) >= 15 ) {
				return 2;
			}
			return 1;
		}
	}
	else {
		if ( _Trace_TM_Direct_Side[mode] == -1 ) return 0;
		if ( _Trace_TM_Direct_Side[mode] != side ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( _Trace_TM_Direct_Side[mode] ) ) { // 2008.12.12
				_Trace_TM_Direct_Side[mode] = -1;
				return 0;
			}
			//
			if      ( ( _Trace_TM_Direct_Side_Mon[_Trace_TM_Direct_Side[mode]][mode] - _Trace_TM_Direct_Side_Mon[side][mode] ) >= 30 ) {
				return 3;
			}
			else if ( ( _Trace_TM_Direct_Side_Mon[_Trace_TM_Direct_Side[mode]][mode] - _Trace_TM_Direct_Side_Mon[side][mode] ) >= 15 ) {
				return 2;
			}
			return 1;
		}
	}
	return 0;
}
//
void _Trace_Set_Direct_Side( int mode , int side ) { // 2008.09.26
	int i;
	if ( mode == -1 ) {
		if ( _Trace_FEM_Direct_Side == -1 ) {
			if ( side != -1 ) {
				_Trace_FEM_Direct_Side = side;
				_Trace_FEM_Direct_Side_Mon[side]++;
			}
		}
		else {
			_Trace_FEM_Direct_Side = side;
			if ( side == -1 ) {
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					_Trace_FEM_Direct_Side_Mon[i] = 0;
				}
			}
			else {
				_Trace_FEM_Direct_Side_Mon[side]++;
			}
		}
	}
	else {
		if ( _Trace_TM_Direct_Side[mode] == -1 ) {
			if ( side != -1 ) {
				_Trace_TM_Direct_Side[mode] = side;
				_Trace_TM_Direct_Side_Mon[side][mode]++;
			}
		}
		else {
			_Trace_TM_Direct_Side[mode] = side;
			if ( side == -1 ) {
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					_Trace_TM_Direct_Side_Mon[i][mode] = 0;
				}
			}
			else {
				_Trace_TM_Direct_Side_Mon[side][mode]++;
			}
		}
	}
}
//===============================================================================
//===============================================================================
int _Trace_FEM_Flow[MAX_CASSETTE_SIDE];
int _Trace_TM_Flow[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH];
//
int _Trace_FEM_Restart_Checked[MAX_CASSETTE_SIDE]; // 2012.09.19
int _Trace_TM_Restart_Checked[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH]; // 2012.09.19
//===============================================================================
//===============================================================================
//===============================================================================
int Get_Inside_Value_for_Common_Thread_Status( int side , int where ) { // 2008.07.29
	if ( where < 0 ) return _Trace_FEM_Flow[side];
	else             return _Trace_TM_Flow[side][where];
}
//===============================================================================
//===============================================================================
//===============================================================================
/*
// 2013.11.21
#ifdef _SCH_MIF
	int _SCH_COMMON_ENTER_CONTROL_FEM( int side , int mode ) {
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
			return USER_DLL_SCH_INF_ENTER_CONTROL_FEM( side , mode );
		}
		else {
			return _EIL_COMMON_ENTER_CONTROL_FEM( side , mode );
		}
	}
#else
	int _SCH_COMMON_ENTER_CONTROL_FEM( int side , int mode ) {
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
			switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
			case 0 :
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_0( side , mode );
				break;
			case 4 :
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_4( side , mode );
				break;
			case 6 :
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_6( side , mode );
				break;
			default :
				return USER_DLL_SCH_INF_ENTER_CONTROL_FEM( side , mode );
				break;
			}
		}
		else {
			return _EIL_COMMON_ENTER_CONTROL_FEM( side , mode );
		}
		return SYS_ABORTED;
	}
#endif
*/
// 2013.11.21
int _SCH_COMMON_ENTER_CONTROL_FEM( int side , int mode ) {
	int Res;
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
		//
		Res = USER_DLL_SCH_INF_ENTER_CONTROL_FEM( side , mode );
		//
#ifndef _SCH_MIF
		if ( ( Res != SYS_ABORTED ) && ( Res != SYS_ERROR ) ) {
			//
			switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
			case 0 :
				Res = USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_0( side , mode );
				break;
			case 4 :
				Res = USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_4( side , mode );
				break;
			case 6 :
				Res = USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_6( side , mode );
				break;
			}
			//
		}
#endif
		//
	}
	else {
		Res = _EIL_COMMON_ENTER_CONTROL_FEM( side , mode );
	}
	return Res;
}
//
/*
// 2013.11.21
#ifdef _SCH_MIF
	int _SCH_COMMON_RUN_CONTROL_FEM( int side ) {
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
			return USER_DLL_SCH_INF_RUN_CONTROL_FEM( side );
		}
		else {
			return _EIL_COMMON_RUN_CONTROL_FEM( side );
		}
	}
#else
	int _SCH_COMMON_RUN_CONTROL_FEM( int side ) {
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
			switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
			case 0 :
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_0( side );
				break;
			case 4 :
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_4( side );
				break;
			case 6 :
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_6( side );
				break;
			default :
				return USER_DLL_SCH_INF_RUN_CONTROL_FEM( side );
				break;
			}
		}
		else {
			return _EIL_COMMON_RUN_CONTROL_FEM( side );
		}
		return SYS_ABORTED;
	}
#endif
*/
// 2013.11.21
int _SCH_COMMON_RUN_CONTROL_FEM( int side ) {
	int Res;
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.09.13
		//
		Res = USER_DLL_SCH_INF_RUN_CONTROL_FEM( side );
		//
#ifndef _SCH_MIF
		if (
			( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) ||
			( Res == -4 ) || ( Res == -3 ) || ( Res == -2 ) || ( Res == -1 ) || ( Res == SYS_RUNNING ) || ( Res == SYS_SUCCESS )
			) {
			//
			switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
			case 0 :
				Res = USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_0( side );
				break;
			case 4 :
				Res = USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_4( side );
				break;
			case 6 :
				Res = USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_6( side );
				break;
			}
			//
		}
#endif
		//
	}
	else {
		Res = _EIL_COMMON_RUN_CONTROL_FEM( side );
	}
	return Res;
}
//
/*
// 2013.11.21
#ifdef _SCH_MIF
	int _SCH_COMMON_ENTER_CONTROL_TM( int side , int mode ) {
		return USER_DLL_SCH_INF_ENTER_CONTROL_TM( side , mode );
	}
#else
	int _SCH_COMMON_ENTER_CONTROL_TM( int side , int mode ) {
		switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
		case 0 :
			return USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_0( side , mode );
			break;
		case 4 :
			return USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_4( side , mode );
			break;
		case 6 :
			return USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_6( side , mode );
			break;
		default :
			return USER_DLL_SCH_INF_ENTER_CONTROL_TM( side , mode );
			break;
		}
		return SYS_ABORTED;
	}
#endif
*/

BOOL FM_USER_MODE_LOCKED() { // 2017.02.27
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		if ( _Trace_FM_Flow_Usermode_Lock[i] ) return TRUE;
	}
	//
	return FALSE;
}



// 2013.11.21
int _SCH_COMMON_ENTER_CONTROL_TM( int side , int mode ) {
	int Res;
	//
	if ( FM_USER_MODE_LOCKED() ) return 0; // 2017.03.03
	//
	Res = USER_DLL_SCH_INF_ENTER_CONTROL_TM( side , mode );
	//
#ifndef _SCH_MIF
	if ( ( Res != SYS_ABORTED ) && ( Res != SYS_ERROR ) ) {
		//
		switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
		case 0 :
			Res = USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_0( side , mode );
			break;
		case 4 :
			Res = USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_4( side , mode );
			break;
		case 6 :
			Res = USER_DLL_SCH_INF_ENTER_CONTROL_TM_STYLE_6( side , mode );
			break;
		}
		//
	}
#endif
	return Res;
}
//
/*
// 2013.11.21
#ifdef _SCH_MIF
	#define MAKE_SCH_COMMON_RUN_CONTROL_TM( ZZZ ) int _SCH_COMMON_RUN_CONTROL_TM_##ZZZ##( int side ) { \
		return USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##( side ); \
	}
#else
	#define MAKE_SCH_COMMON_RUN_CONTROL_TM( ZZZ ) int _SCH_COMMON_RUN_CONTROL_TM_##ZZZ##( int side ) { \
		switch( SYSTEM_RUN_ALG_STYLE_GET() ) { \
		case 0 : \
			return USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##_STYLE_0( side ); \
			break; \
		case 4 : \
			return USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##_STYLE_4( side ); \
			break; \
		case 6 : \
			return USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##_STYLE_6( side ); \
			break; \
		default : \
			return USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##( side ); \
			break; \
		} \
		return SYS_ABORTED; \
	}
#endif
*/
// 2013.11.21
#ifdef _SCH_MIF
	#define MAKE_SCH_COMMON_RUN_CONTROL_TM( ZZZ ) int _SCH_COMMON_RUN_CONTROL_TM_##ZZZ##( int side ) { \
\
		if ( FM_USER_MODE_LOCKED() ) return SYS_RUNNING; /* 2017.02.27 */ \
\
		return USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##( side ); \
	}
#else
	#define MAKE_SCH_COMMON_RUN_CONTROL_TM( ZZZ ) int _SCH_COMMON_RUN_CONTROL_TM_##ZZZ##( int side ) { \
		int Res; \
\
		if ( FM_USER_MODE_LOCKED() ) return SYS_RUNNING; /* 2017.02.27 */ \
\
		Res = USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##( side ); \
		if ( \
			( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) || \
			( Res == -4 ) || ( Res == -3 ) || ( Res == -2 ) || ( Res == -1 ) || ( Res == SYS_RUNNING ) || ( Res == SYS_SUCCESS ) \
			) { \
			switch( SYSTEM_RUN_ALG_STYLE_GET() ) { \
			case 0 : \
				Res = USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##_STYLE_0( side ); \
				break; \
			case 4 : \
				Res = USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##_STYLE_4( side ); \
				break; \
			case 6 : \
				Res = USER_DLL_SCH_INF_RUN_CONTROL_TM_##ZZZ##_STYLE_6( side ); \
				break; \
			} \
		} \
		return Res; \
	}
#endif


MAKE_SCH_COMMON_RUN_CONTROL_TM( 1 );
MAKE_SCH_COMMON_RUN_CONTROL_TM( 2 );
MAKE_SCH_COMMON_RUN_CONTROL_TM( 3 );
MAKE_SCH_COMMON_RUN_CONTROL_TM( 4 );
#ifndef PM_CHAMBER_12
MAKE_SCH_COMMON_RUN_CONTROL_TM( 5 );
MAKE_SCH_COMMON_RUN_CONTROL_TM( 6 );
#ifndef PM_CHAMBER_30
MAKE_SCH_COMMON_RUN_CONTROL_TM( 7 );
MAKE_SCH_COMMON_RUN_CONTROL_TM( 8 );
#endif
#endif


//------------------------------------------------------------------------------------------
void _Sleep_LC( int *l_c , int Count ) {
	if ( ( *l_c % Count ) == 0 ) Sleep(1);
	(*l_c)++;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Move_Data_from_to( int side , int tm ); // 2011.04.18

//------------------------------------------------------------------------------------------
// FEM Scheduling
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_FEM_Running( int CHECKING_SIDE ) {
	int i;
	int Res;
	int l_c;
	int dside;
	int abtelse;
	int pt;
	//
	_Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = FALSE; // 2017.02.27
	//
	//============================================================================================================
	_Trace_FM_Flow_In[CHECKING_SIDE] = 0; // 2014.03.12
	//============================================================================================================
	_Trace_FM_Flow_Kill[CHECKING_SIDE] = 0; // 2012.01.29
	//============================================================================================================
	_Trace_FEM_Flow[CHECKING_SIDE] = 1;
	//============================================================================================================
	_Trace_FEM_Restart_Checked[CHECKING_SIDE] = 0; // 2012.09.19
	//============================================================================================================
	_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 1 );
	//============================================================================================================
	_i_SCH_MODULE_Set_FM_HWait( CHECKING_SIDE , FALSE );
	//============================================================================================================
	Res = 0;
	//============================================================================================================
	if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) START\n" );
	//============================================================================================================
	_Trace_FEM_Flow[CHECKING_SIDE] = 11;
	//============================================================================================================
	_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 2 );
	//============================================================================================================
	switch ( _SCH_COMMON_ENTER_CONTROL_FEM( CHECKING_SIDE , 0 ) ) {
	case SYS_SUCCESS :
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 12;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) RETURN with SYS_SUCCESS\n" );
		//============================================================================================================
		_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , 0 );
		_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
		_endthread();
		return;
		break;
	case SYS_ABORTED :
	case SYS_ERROR :
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 13;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) RETURN with SYS_ABORTED\n" );
		//============================================================================================================
		_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , -1 );
		_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
		_endthread();
		return;
		break;
	case SYS_RUNNING :
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 14;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) RETURN with SYS_RUNNING\n" );
		//============================================================================================================
		break;
	case -1 :
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 15;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) RETURN with -1\n" );
		//============================================================================================================
		break;
	case 0 :
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 16;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) RETURN with 0\n" );
		//============================================================================================================
		Res = 1;
		break;
	default :
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 17;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0) RETURN with default\n" );
		//============================================================================================================
		break;
	}
	//============================================================================================================
	_Trace_FEM_Flow[CHECKING_SIDE] = 21;
	//============================================================================================================
	if ( Res == 0 ) {
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 1022;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0-1) START\n" );
		//============================================================================================================
//		EnterCriticalSection( &CR_FEMRUN ); // 2009.03.27
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 1023;
		//============================================================================================================
		_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , 2 );
		//============================================================================================================
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12
			if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Start for FEM Module%cWHFMSTART\n" , 9 );
			}
		}
		//============================================================================================================
//		LeaveCriticalSection( &CR_FEMRUN ); // 2009.03.27
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 24;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(0-1) END\n" );
		//============================================================================================================
	}
	//============================================================================================================
	l_c = 0;
	//============================================================================================================
	Res = 0;
	//============================================================================================================
	while ( TRUE ) {
		//============================================================================================================
		_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 3 );
		//============================================================================================================
		if ( _Trace_FM_Flow_Kill[CHECKING_SIDE] != 0 ) { // 2012.01.29
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 30;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM FLAG_CHECK RETURN with SYS_SUCCESS\n" );
			//============================================================================================================
			if ( ( Res != -1 ) && ( Res != -3 ) ) {
				_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , 0 );
				_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
				_endthread();
			}
			else {
				_i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
			}
			return;
		}
		//============================================================================================================
		if ( ( Res != -1 ) && ( Res != -3 ) ) {
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 31;
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) START %d\n" , _Trace_FEM_Wait_Entering[CHECKING_SIDE] );// 2011.10.11
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) START\n" ); // 2011.10.11 // 2014.03.12
			//============================================================================================================
			if ( Res != -4 ) {
				//
				switch ( _SCH_COMMON_ENTER_CONTROL_FEM( CHECKING_SIDE , 1 ) ) {
				case SYS_RUNNING :
					//============================================================================================================
					_Trace_FEM_Flow[CHECKING_SIDE] = 33;
					//============================================================================================================
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) RETURN with SYS_RUNNING\n" );
					//============================================================================================================
					_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
					continue;
					break;
				case SYS_SUCCESS :
					//============================================================================================================
					_Trace_FEM_Flow[CHECKING_SIDE] = 34;
					//============================================================================================================
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) RETURN with SYS_SUCCESS\n" );
					//============================================================================================================
					_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , 0 );
					_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
					_endthread();
					return;
					break;
				case SYS_ABORTED :
				case SYS_ERROR :
					//============================================================================================================
					_Trace_FEM_Flow[CHECKING_SIDE] = 35;
					//============================================================================================================
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) RETURN with SYS_ABORTED\n" );
					//============================================================================================================
					_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , -1 );
					_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
					_endthread();
					return;
					break;
				default :
					//============================================================================================================
					_Trace_FEM_Flow[CHECKING_SIDE] = 36;
					//============================================================================================================
//					_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) RETURN with default\n" ); // 2014.03.12
					//============================================================================================================
					break;
				}
			}
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1041;
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_ENTER_FEM_CRITICAL 1 [%d]\n" , Res ); // 2014.03.12
			//============================================================================================================
//			_Trace_FEM_Other_RunPart_Abort_Check[CHECKING_SIDE] = 0; // 2010.07.02 // 2010.07.29
			//============================================================================================================
//			EnterCriticalSection( &CR_FEMRUN ); // 2011.05.14
			//
/*
			if ( !TryEnterCriticalSection( &CR_FEMRUN ) ) { // 2011.05.14
				_Sleep_LC( &l_c , 5 );
				continue;
			}
*/
			if ( !TryEnterCriticalSection( &CR_FEMRUN ) ) { // 2011.10.11
				//
				_Trace_FM_Flow_In[CHECKING_SIDE]++; // 2014.03.12
				//
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				continue;
			}
			//
			_Trace_FM_Flow_In[CHECKING_SIDE] = 0; // 2014.03.12
			//
/*
// 2011.10.11
			while ( TRUE ) { // 2011.06.24
				//
				if ( _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( CHECKING_SIDE ) == -1 ) {
					//============================================================================================================
					_Trace_FEM_Flow[CHECKING_SIDE] = 38;
					//============================================================================================================
					_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1) RETURN with SYS_ABORTED2\n" );
					//============================================================================================================
					_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , -1 );
					_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 0 );
					_endthread();
					return;
				}
				//
				if ( TryEnterCriticalSection( &CR_FEMRUN ) ) {
					if ( _Trace_FEM_Wait_Last_Entering == CHECKING_SIDE ) {
						//
						dside = 0;
						//
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							if ( i == CHECKING_SIDE ) continue;
							if ( _Trace_FEM_Wait_Entering[i] > 0 ) {
								_Trace_FEM_Wait_Entering[i]--;
								dside = 1;
							}
						}
						//
						if ( dside == 0 ) break;
						//============================================================================================================
						LeaveCriticalSection( &CR_FEMRUN );
						//============================================================================================================
					}
					else {
						//
						break;
						//
					}
				}
				else {
					//
					if ( _Trace_FEM_Wait_Entering[CHECKING_SIDE] < MAX_CASSETTE_SIDE ) _Trace_FEM_Wait_Entering[CHECKING_SIDE]++; // 2011.06.24
					//
				}
				//
				_Sleep_LC( &l_c , 5 );
				//
			}
			//============================================================================================================
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _Trace_FEM_Wait_Entering[i] = 0;
			_Trace_FEM_Wait_Last_Entering = CHECKING_SIDE;
*/
			//============================================================================================================
			if ( _Trace_FEM_Other_RunPart_Abort_Check[CHECKING_SIDE] != 0 ) { // 2010.07.02
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 37;
				//============================================================================================================
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12
//					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 3 for FEM Module%cWHFMSUCCESS 3\n" , 9 ); // 2012.01.19
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 11(OTHER_ABORT) for FEM Module%cWHFMERROR 11\n" , 9 ); // 2012.01.19
				}
				_i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
				//============================================================================================================
				return;
			}

			//============================================================================================================
			if ( _Trace_FM_Flow_Kill[CHECKING_SIDE] != 0 ) { // 2018.04.16
				_Trace_FEM_Flow[CHECKING_SIDE] = 38;
				_i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
				return;
			}
			//============================================================================================================


			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1042;
			//============================================================================================================
			switch ( _Trace_Check_Direct_Side_Invalid( -1 , CHECKING_SIDE ) ) {
			case 1 : // Invalid
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 44;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM INVALID SIDE 44\n" );
				//============================================================================================================
				continue;
				break;
			case 2 : // Invalid and Warning
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 44;
				//============================================================================================================
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12
//					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Invalid Check Warning%cWHFMINVALID 1\n" , 9 ); // 2012.11.14
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FM* Scheduling Invalid Check Warning\n" ); // 2012.11.14
				}
				//============================================================================================================
				continue;
				break;
			case 3 : // Invalid But Warning so Valid
				//============================================================================================================
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12
//					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Invalid Check Rejecting%cWHFMINVALID 2\n" , 9 ); // 2012.11.14
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FM* Scheduling Invalid Check Rejecting\n" ); // 2012.11.14
				}
				//============================================================================================================
				break;
			}
			//============================================================================================================
			_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 );
			//
			if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) {
				if ( !_SCH_COMMON_SIDE_SUPPLY( CHECKING_SIDE , FALSE , -1 , &Res ) ) {
					if ( !_i_SCH_CASSETTE_Check_Side_Monitor( CHECKING_SIDE ) ) {
						//============================================================================================================
						LeaveCriticalSection( &CR_FEMRUN );
						//============================================================================================================
						_Trace_FEM_Flow[CHECKING_SIDE] = 43;
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_LEAVE_FEM_CRITICAL 1\n" );
						//============================================================================================================
						continue;
					}
				}
			}
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1051;
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(1-1) END\n" ); // 2014.03.12
			//============================================================================================================
		}
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 1071;
		//============================================================================================================
		Res = 0;
		//============================================================================================================
		_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 4 );
		//============================================================================================================
		switch( _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( CHECKING_SIDE ) ) {
		case -1 :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 72;
			//============================================================================================================
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 2 for FEM Module%cWHFMSUCCESS 2\n" , 9 ); // 2005.09.27 // 2012.01.19
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 12(PAUSE_ABORT) for FEM Module%cWHFMERROR 12\n" , 9 ); // 2005.09.27 // 2012.01.19
			}
			_i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
			return;
			break;
		case 1 :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 73;
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_LEAVE_FEM_CRITICAL 2\n" ); // 2014.03.12
			//============================================================================================================
			LeaveCriticalSection( &CR_FEMRUN );
			_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
			continue;
			break;
		}
		//============================================================================================================
		_i_SCH_SUB_FMTMSIDE_OPERATION( CHECKING_SIDE , 0 ); // 2009.05.06 // 2010.03.06
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 1081;
		//============================================================================================================
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) START\n" );
		//============================================================================================================
		switch ( _SCH_COMMON_ENTER_CONTROL_FEM( CHECKING_SIDE , 2 ) ) {
		case SYS_SUCCESS :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 82;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with SYS_SUCCESS\n" );
			//============================================================================================================
			_i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
			return;
			break;
		case SYS_ABORTED :
		case SYS_ERROR :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 83;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with SYS_ABORTED\n" );
			//============================================================================================================
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 13(ENTER2_ABORT) for FEM Module%cWHFMERROR 13\n" , 9 ); // 2012.01.19
			//============================================================================================================
			_i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
			//============================================================================================================
			return;
			break;
		case SYS_RUNNING :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 84;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with SYS_RUNNING\n" );
			//============================================================================================================
			LeaveCriticalSection( &CR_FEMRUN );
			_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
			continue;
			break;
		case -1 :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1085;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with -1\n" );
			//============================================================================================================
			if ( _i_SCH_CASSETTE_Chk_SKIP( CHECKING_SIDE ) != 0 ) {
			}
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1086;
			//============================================================================================================
			_SCH_MACRO_INTERRUPT_PART_CHECK( CHECKING_SIDE , FALSE , 0 );
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1087;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with -1(FINISH1)\n" );
			//============================================================================================================
/*
// 2011.07.07
			if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 88;
				//============================================================================================================
				_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with -1(FINISH2)\n" );
				//============================================================================================================
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2010.10.12
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish for FEM Module%cWHFMSUCCESS 1\n" , 9 );
				}
				//============================================================================================================
				_i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
				//============================================================================================================
				return;
			}
*/
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1089;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with -1(FINISH3)\n" );
			//============================================================================================================
			break;
		default :
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1090;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with default\n" );
			//============================================================================================================
			break;
		}
// 2011.07.07
		//============================================================================================================
		if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , FALSE ) ) {
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 88;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM ENTER_CONTROL(2) RETURN with -1(FINISH2)\n" );
			//============================================================================================================
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() <= 0 ) { // 2010.10.12
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish for FEM Module%cWHFMSUCCESS 1\n" , 9 ); // 2012.01.19
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 1(FINISH1) for FEM Module%cWHFMSUCCESS 1\n" , 9 ); // 2012.01.19
			}
			//============================================================================================================
			_i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
			//============================================================================================================
			return;
		}
		//============================================================================================================

		//============================================================================================================
//		_i_SCH_SUB_FMTMSIDE_OPERATION( CHECKING_SIDE , 0 ); // 2009.05.06 // 2010.03.06
		//============================================================================================================
		_i_SCH_SYSTEM_USING2_SET( CHECKING_SIDE , 5 );
		//============================================================================================================
		_Trace_FEM_Flow[CHECKING_SIDE] = 1101;
		//============================================================================================================
		//============================================================================================================
		_i_SCH_SYSTEM_RUNCHECK_FM_SET( CHECKING_SIDE + 1 );
		//============================================================================================================
		//-------------------------------------------------------------------
		// 2012.09.19
		//-------------------------------------------------------------------
		Res = SYS_RUNNING;
		//
		if ( _Trace_FEM_Restart_Checked[CHECKING_SIDE] == 0 ) {
			//
			_Trace_FEM_Restart_Checked[CHECKING_SIDE] = 1;
			//
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				//
				if ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) != 0 ) {
					//
					if ( USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_FEM() ) {
						//============================================================================================================
						_Trace_FEM_Flow[CHECKING_SIDE] = 1102;
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RESTART_PRE_CONTROL START %d\n" , _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) );
						//============================================================================================================
						Res = USER_DLL_SCH_INF_RESTART_PRE_CONTROL_FEM( CHECKING_SIDE , _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) , 0 );
						//============================================================================================================
						if ( Res != SYS_ABORTED ) SCH_RESTART_DATA_REMAP_AFTER_USER_PART( CHECKING_SIDE , -1 ); // 2013.06.19
						//============================================================================================================
					}
				}
				//
			}
			//
		}
		//-------------------------------------------------------------------
		// 2011.12.08
		//-------------------------------------------------------------------
		if ( Res == SYS_RUNNING ) { // 2012.09.19
			//
	//		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2012.04.18 // 2012.07.24
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2012.04.18 // 2012.07.24
				//
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) {
					Res = _EIL_FMMODE_RUN_CONTROL_FM( CHECKING_SIDE ); // 2012.09.21
				}
				//
				if ( Res == SYS_RUNNING ) {
					//
					if ( USER_DLL_SCH_INF_HAS_USER_CONTROL_FEM() ) {
						//
						if ( _i_SCH_USER_POSSIBLE_PICK_FROM_FM( CHECKING_SIDE , &pt ) ) {
							//
							_Trace_FEM_Flow[CHECKING_SIDE] = 1111;

							//============================================================================================================
							if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM USER_CONTROL START %d[%d:%s]\n" , pt , _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( CHECKING_SIDE , pt ) , _i_SCH_CLUSTER_Get_Ex_UserControl_Data( CHECKING_SIDE , pt ) );
							//============================================================================================================
							//
							_Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = TRUE; // 2017.02.27
							//
							Res = USER_DLL_SCH_INF_USER_CONTROL_FEM(
								CHECKING_SIDE ,
								pt ,
								_i_SCH_CLUSTER_Get_Ex_UserControl_Mode( CHECKING_SIDE , pt ) ,
								_i_SCH_CLUSTER_Get_Ex_UserControl_Data( CHECKING_SIDE , pt ) );
							//
							if ( Res == SYS_RUNNING ) _Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = FALSE; // 2017.02.27
							//
							//============================================================================================================
							if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM USER_CONTROL END %d[Res=%d][%d:%s]\n" , pt , Res , _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( CHECKING_SIDE , pt ) , _i_SCH_CLUSTER_Get_Ex_UserControl_Data( CHECKING_SIDE , pt ) );
							//============================================================================================================
						}
						else {
							//
							_Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = FALSE; // 2017.02.27
							//
							//============================================================================================================
							if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM USER_CONTROL UNMATCHED %d\n" , pt );
							//============================================================================================================
						}
					}
					else {
						//
						_Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = FALSE; // 2017.02.27
						//
					}
				}
				else {
					//
					_Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = FALSE; // 2017.02.27
					//
				}
				//
			}
			else {
				//
				_Trace_FM_Flow_Usermode_Lock[CHECKING_SIDE] = FALSE; // 2017.02.27
				//
			}
			//
			if ( Res == SYS_RUNNING ) {
				//
				_Trace_FEM_Flow[CHECKING_SIDE] = 1112;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL START\n" );
				//============================================================================================================
				Res = _SCH_COMMON_RUN_CONTROL_FEM( CHECKING_SIDE );
			}
			else if ( Res == SYS_SUCCESS )	{
				Res = SYS_RUNNING;
			}
		}
		else if ( Res == SYS_SUCCESS )	{
			Res = SYS_RUNNING;
		}
		//============================================================================================================
		abtelse = 0;
		//============================================================================================================
		if (
			( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) ||
			( Res == -4 ) || ( Res == -3 ) || ( Res == -2 ) || ( Res == -1 ) || ( Res == SYS_RUNNING )
			) { // 2011.04.20
			//============================================================================================================
			dside = SCHEDULER_CONTROL_Move_Data_from_to( CHECKING_SIDE , -1 );
			//============================================================================================================
			if      ( dside >  1 ) {
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM Move_Data_from_to is %d\n" , dside );
			}
			else if ( dside == 1 ) {
			}
			else if ( dside == 0 ) {
				Res = SYS_RUNNING;
			}
			else {
				abtelse = -dside;
				Res = SYS_ABORTED;
			}
		}
		//============================================================================================================
		if ( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) { // nosleep , leave & Direct->dside
			//--------------------------------------------------
			dside = -100 - Res;
			//--------------------------------------------------
			//============================================================================================================
			_Trace_FEM_Flow[CHECKING_SIDE] = 1300 + dside;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with %d\n" , Res );
			//============================================================================================================
			//============================================================================================================
			_Trace_Set_Direct_Side( -1 , dside );
			//============================================================================================================
			if ( CHECKING_SIDE == dside ) {
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1500 + dside;
				//============================================================================================================
				Res = -3;
				//============================================================================================================
			}
			else {
				//============================================================================================================
				Res = -4;
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 500 + dside;
				//============================================================================================================
				_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_LEAVE_FEM_CRITICAL %d\n" , Res );
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
			}
		}
		else {
			switch( Res ) {
			case -4 : // nosleep , leave , NCEnter(1)Part_Skip
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1204;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with -4\n" );
				//============================================================================================================
				//============================================================================================================
				_Trace_Set_Direct_Side( -1 , -1 );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 304;
				//============================================================================================================
				_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_LEAVE_FEM_CRITICAL -4\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
				break;
			case -3 : // nosleep , noleave(same)
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1203;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with -3\n" );
				//============================================================================================================
				//============================================================================================================
				_Trace_Set_Direct_Side( -1 , -1 );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1303;
				//============================================================================================================
				break;
			case -2 : // nosleep , leave
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1202;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with -2\n" );
				//============================================================================================================
				//============================================================================================================
				_Trace_Set_Direct_Side( -1 , -1 );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 302;
				//============================================================================================================
				_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_LEAVE_FEM_CRITICAL -2\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
				break;
			case -1 : // sleep , noleave(same)
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1201;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with -1\n" );
				//============================================================================================================
				//============================================================================================================
				_Trace_Set_Direct_Side( -1 , -1 );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1301;
				//============================================================================================================
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				//============================================================================================================
				break;
			case SYS_RUNNING : // sleep , leave
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 1102;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with SYS_RUNNING\n" );
				//============================================================================================================
				//============================================================================================================
				_Trace_Set_Direct_Side( -1 , -1 );
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 105;
				//============================================================================================================
				_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM _SCH_SYSTEM_LEAVE_FEM_CRITICAL def\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				//============================================================================================================
				break;
			case SYS_SUCCESS :
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 103;
				//============================================================================================================
				_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 ); // 2010.06.24
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with SYS_SUCCESS\n" );
				//============================================================================================================
				_i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
				//============================================================================================================
				return;
				break;
			default :
				//============================================================================================================
				_Trace_FEM_Flow[CHECKING_SIDE] = 104 + ( abtelse * 1000 );
				//============================================================================================================
				_i_SCH_SYSTEM_RUNCHECK_FM_SET( 0 ); // 2010.06.24
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 0 , "FEM* SYSTEM RUN_CONTROL RETURN with default(%d)\n" , Res );
				//============================================================================================================
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) _Trace_FEM_Other_RunPart_Abort_Check[i] = 1; // 2010.07.02
				//============================================================================================================
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Finish 14(RUN_ABORT:%d) for FEM Module%cWHFMERROR 14\n" , Res , 9 ); // 2012.01.19
				//============================================================================================================
				_i_SCH_SUB_ERROR_FINISH_RETURN_FOR_FEM( CHECKING_SIDE , TRUE );
				//============================================================================================================
				return;
				break;
			}
		}
		//============================================================================================================
	}
	//============================================================================================================
	_Trace_FEM_Flow[CHECKING_SIDE] = 199;
	//============================================================================================================
	_endthread();
}


BOOL Scheduler_Other_TM_Aborted( int CHECKING_SIDE , int tmside ) { // 2017.02.04
	int i;
	//
	for ( i = 1 ; i <= _Trace_TM_Count[CHECKING_SIDE] ; i++ ) {
		//
		if ( _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) < 0 ) {
			//
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d* Scheduling Finish 13(TM_TAG_FAIL_ABORT:%d) for TM Module%cWHTM%dERROR 13\n" , tmside + 1 , i + 1 , 9 , tmside + 1 );
			//
			_i_SCH_SYSTEM_MODE_ABORT_SET( CHECKING_SIDE );
			//
			return TRUE;
		}
	}
	//
	return FALSE;
	//
}

//
/*
//
// 2019.03.04
//
//void Scheduler_Other_TM_Finished( int CHECKING_SIDE ) { // 2017.02.04 // 2018.04.16
void Scheduler_Other_TM_Finished( int CHECKING_SIDE , BOOL abort ) { // 2017.02.04 // 2018.04.16
	int i , f;
//	DWORD dc;
	//
	if ( _Trace_TM_Count[CHECKING_SIDE] < 1 ) return;
	//
//	_i_SCH_SYSTEM_MODE_ABORT_SET( CHECKING_SIDE ); // 2018.04.16
	if ( abort ) _i_SCH_SYSTEM_MODE_ABORT_SET( CHECKING_SIDE ); // 2018.04.16
	//
	_Trace_FM_Flow_Kill[CHECKING_SIDE] = 1; // 2018.04.16
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _Trace_TM_Flow_Kill[CHECKING_SIDE][i] = 1;
	//
//	dc = GetTickCount();
	//
	while ( TRUE ) {
		//
		f = 0;
		//
		for ( i = 1 ; i <= _Trace_TM_Count[CHECKING_SIDE] ; i++ ) {
			//
			if ( _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) > 0 ) {
				//
				f = 1;
				//
				break;
				//
			}
		}
		//
		if ( f == 0 ) return;
		//
//		if ( ( GetTickCount() - dc ) >= 600000 ) return;
		//
		Sleep(1);
		//
	}
	//
}
//
*/
//
//
//
// 2019.03.04
//
void Scheduler_Other_TM_Finished( int CHECKING_SIDE , BOOL abort ) { // 2017.02.04 // 2018.04.16
	int i , f;
	//
	if ( abort ) { // 2019.03.04
		//
		for ( f = 0 ; f < MAX_CASSETTE_SIDE ; f++ ) {
			//
			if ( !_i_SCH_SYSTEM_USING_RUNNING( f ) ) continue;
			//
			_i_SCH_SYSTEM_MODE_ABORT_SET( f ); // 2018.04.16
			//
			_Trace_FM_Flow_Kill[f] = 1; // 2018.04.16
			//
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _Trace_TM_Flow_Kill[f][i] = 1;
			//
		}
		//
		if ( _Trace_TM_Count[CHECKING_SIDE] < 1 ) return;
		//
	}
	else {
		if ( _Trace_TM_Count[CHECKING_SIDE] < 1 ) return;
		//
		_Trace_FM_Flow_Kill[CHECKING_SIDE] = 1; // 2018.04.16
		//
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _Trace_TM_Flow_Kill[CHECKING_SIDE][i] = 1;
		//
	}
	//
	while ( TRUE ) {
		//
		f = 0;
		//
		for ( i = 1 ; i <= _Trace_TM_Count[CHECKING_SIDE] ; i++ ) {
			//
			if ( _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) > 0 ) {
				//
				f = 1;
				//
				break;
				//
			}
		}
		//
		if ( f == 0 ) return;
		//
		Sleep(1);
		//
	}
	//
}
//

#define MAKE_SCHEDULER_MAIN_TM( ZZZ , ZZZ2 ) int Scheduler_Main_Running_TM_##ZZZ##( int CHECKING_SIDE ) { \
	int j; \
	int Res; \
	int l_c; \
	int dside; \
	int abtelse; \
\
	_Trace_TM_Flow_In[CHECKING_SIDE][ ##ZZZ## ] = 0; /* 2014.03.12 */ \
\
	_Trace_TM_Flow_Kill[CHECKING_SIDE][ ##ZZZ## ] = 0; /* 2010.07.29 */ \
\
	_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1; \
\
	_Trace_TM_Restart_Checked[CHECKING_SIDE][##ZZZ##] = 0; /* 2012.09.19 */ \
\
	j = _i_SCH_SUB_PRECHECK_WAITING_FOR_THREAD_TM( CHECKING_SIDE , ##ZZZ## ); \
\
	_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 2; \
\
	if ( j != SYS_RUNNING ) { \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 3; \
\
		return j; \
	} \
\
	_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 11; \
\
	l_c = 0; \
\
	Res = 0; \
\
	while ( TRUE ) { \
\
/*		if ( _i_SCH_SYSTEM_USING_GET(CHECKING_SIDE) < 9 ) { */ /* 2010.07.29 */ \
		if ( ( _i_SCH_SYSTEM_USING_GET(CHECKING_SIDE) < 9 ) || ( _Trace_TM_Flow_Kill[CHECKING_SIDE][ ##ZZZ## ] != 0 ) ) { /* 2010.07.29 */ \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 12; \
\
			_i_SCH_SYSTEM_TM_SET( ##ZZZ## , CHECKING_SIDE , 0 ); \
\
			if ( ( Res == -1 ) || ( Res == -3 ) ) LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); /* 2008.07.11 */ \
\
			_endthread(); \
\
			return SYS_SUCCESS; \
		} \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1041; \
\
		if ( ( Res != -1 ) && ( Res != -3 ) ) { \
\
/*			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM_ENTER_TM_CRITICAL 1 [%d]\n" , ##ZZZ## + 1 , Res ); */ /* 2014.03.12 */  \
\
/*			EnterCriticalSection( &CR_TMRUN[##ZZZ##] ); */ /* 2010.08.05 */ \
\
			if ( !TryEnterCriticalSection( &CR_TMRUN[##ZZZ##] ) ) { /* 2010.08.05 */ \
\
				_Trace_TM_Flow_In[CHECKING_SIDE][##ZZZ##]++; /* 2014.03.12 */ \
\
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] ); \
				continue;	\
\
			} \
\
			_Trace_TM_Flow_In[CHECKING_SIDE][##ZZZ##] = 0; /* 2014.03.12 */ \
\
			switch ( _Trace_Check_Direct_Side_Invalid( ##ZZZ## , CHECKING_SIDE ) ) { \
			case 1 : \
				LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 45; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM INVALID SIDE 45\n" , ##ZZZ## + 1 ); \
				continue; \
				break; \
			case 2 : \
				LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 45; \
/*				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d* Scheduling Invalid Check Warning%cWHTM%dINVALID 1\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); */ /* 2012.11.14 */ \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* Scheduling Invalid Check Warning\n" , ##ZZZ## + 1 ); /* 2012.11.14 */ \
				continue; \
				break; \
			case 3 : \
/*				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d* Scheduling Invalid Check Rejecting%cWHTM%dINVALID 2\n" , ##ZZZ## + 1 , 9 , ##ZZZ## + 1 ); */ /* 2012.11.14 */ \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* Scheduling Invalid Check Rejecting\n" , ##ZZZ## + 1 ); /* 2012.11.14 */ \
				break; \
			} \
\
		} \
\
		/* _Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1071; */ /* 2017.02.04 */ \
\
		Res = 0; \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1061; /* 2017.02.04 */ \
\
\
		if ( Scheduler_Other_TM_Aborted( CHECKING_SIDE , ##ZZZ## ) ) { /* 2017.02.04 */ \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 62; \
\
			return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , TRUE ); \
\
		} \
\
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1071; /* 2017.02.04 */ \
\
\
		switch( _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( CHECKING_SIDE ) ) { \
		case -1 : \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 72; \
\
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d* Scheduling Finish 11(PAUSE_ABORT) for TM Module%cWHTM%dERROR 11\n" , ##ZZZ## + 1 , 9 ,  ##ZZZ## + 1 ); /* 2012.01.19 */ \
\
			return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , TRUE ); \
			break; \
		case 1 : \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 73; \
\
			LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
\
/*			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL 2\n" , ##ZZZ## + 1 ); */ /* 2014.03.12 */ \
\
			_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] ); \
			continue; \
			break; \
		} \
\
		_i_SCH_SUB_FMTMSIDE_OPERATION( CHECKING_SIDE , ##ZZZ## + 1 ); /* 2016.11.03 */ \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1081; \
\
/*		if ( _i_SCH_SYSTEM_USING_GET(CHECKING_SIDE) >= 110 ) { */ /* 2010.07.29 */ \
		if ( ( _i_SCH_SYSTEM_USING_GET(CHECKING_SIDE) >= 110 ) || ( _Trace_TM_Flow_Kill[CHECKING_SIDE][ ##ZZZ## ] != 0 ) ) { /* 2010.07.29 */ \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 82; \
\
			return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , TRUE ); \
		} \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1101; \
\
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , TRUE ); /* 2012.09.10 */ \
\
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) _EIL_BEFORE_RUN_CONTROL_TM_##ZZZ2##( CHECKING_SIDE ); /* 2010.09.13 */ \
\
		/* 2012.09.19 */ \
\
		Res = SYS_RUNNING; \
\
		if ( _Trace_TM_Restart_Checked[CHECKING_SIDE][ ##ZZZ## ] == 0 ) { \
\
			_Trace_TM_Restart_Checked[CHECKING_SIDE][ ##ZZZ## ] = 1; \
\
			if ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) != 0 ) { \
\
				if ( USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_##ZZZ2##() ) { \
\
					_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1102; \
\
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RESTART_PRE_CONTROL START %d\n" , ##ZZZ## + 1 , _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) ); \
\
					Res = USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_##ZZZ2##( CHECKING_SIDE , _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) , 0 ); \
\
					if ( Res != SYS_ABORTED ) SCH_RESTART_DATA_REMAP_AFTER_USER_PART( CHECKING_SIDE , ##ZZZ## ); /* 2013.06.19 */ \
\
				} \
			} \
		} \
\
		if ( Res == SYS_RUNNING ) { \
\
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL START\n" , ##ZZZ## + 1 ); \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1103; /* 2017.02.08 */ \
\
			Res = _SCH_COMMON_RUN_CONTROL_TM_##ZZZ2##( CHECKING_SIDE ); \
\
		} \
		else if ( Res == SYS_SUCCESS ) { \
\
			Res = SYS_RUNNING; \
\
		} \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1104; /* 2017.02.08 */ \
\
		SCH_TM_PICKPLACING_SET( -1 , -1 , ##ZZZ## , -1 , CHECKING_SIDE , -1 ); /* 2015.08.26 */ \
\
		abtelse = 0; \
\
		if ( \
			( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) || \
			( Res == -4 ) || ( Res == -3 ) || ( Res == -2 ) || ( Res == -1 ) || ( Res == SYS_RUNNING ) \
			) { /* 2011.04.20 */ \
\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1105; /* 2017.02.08 */ \
\
			dside = SCHEDULER_CONTROL_Move_Data_from_to( CHECKING_SIDE , ##ZZZ## ); \
			if      ( dside >  1 ) { \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM Move_Data_from_to is %d\n" , ##ZZZ## + 1 , dside ); \
			} \
			else if ( dside == 1 ) { \
			} \
			else if ( dside == 0 ) { \
				Res = SYS_RUNNING; \
			} \
			else { \
				abtelse = -dside; \
				Res = SYS_ABORTED; \
			} \
		} \
\
		_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1106; /* 2017.02.08 */ \
\
		if ( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) { \
			\
			dside = -100 - Res; \
			\
			_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1300 + dside; \
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with %d\n" , ##ZZZ## + 1 , Res ); \
			\
			_Trace_Set_Direct_Side( ##ZZZ## , dside ); \
			if ( CHECKING_SIDE == dside ) { \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1500 + dside; \
				Res = -3; \
			} \
			else { \
				Res = -4; \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 500 + dside; \
				LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL %d\n" , ##ZZZ## + 1 , Res ); \
			} \
		} \
		else { \
			switch( Res ) { \
			case -4 : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1204; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with -4\n" , ##ZZZ## + 1 ); \
				\
				_Trace_Set_Direct_Side( ##ZZZ## , -1 ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 304; \
				LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL -4\n" , ##ZZZ## + 1 ); \
				break; \
			case -3 : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1203; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with -3\n" , ##ZZZ## + 1 ); \
				\
				_Trace_Set_Direct_Side( ##ZZZ## , -1 ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1303; \
				break; \
			case -2 : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1202; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with -2\n" , ##ZZZ## + 1 ); \
				\
				_Trace_Set_Direct_Side( ##ZZZ## , -1 ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 302; \
				LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL -2\n" , ##ZZZ## + 1 ); \
				break; \
			case -1 : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1201; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with -1\n" , ##ZZZ## + 1 ); \
				\
	 			_Trace_Set_Direct_Side( ##ZZZ## , -1 ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1301; \
				\
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] ); \
				break; \
			case SYS_RUNNING : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 1102; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with SYS_RUNNING\n" , ##ZZZ## + 1 ); \
				\
				_Trace_Set_Direct_Side( ##ZZZ## , -1 ); \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 105; \
				LeaveCriticalSection( &CR_TMRUN[##ZZZ##] ); \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL def\n" , ##ZZZ## + 1 ); \
				\
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] ); \
				break; \
			case SYS_SUCCESS : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 103; \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with SYS_SUCCESS\n" , ##ZZZ## + 1 ); \
				return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , TRUE ); \
				break; \
			default : \
				_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 104 + ( abtelse * 1000 ); \
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , ##ZZZ## + 1 , "TM%d* SYSTEM RUN_CONTROL RETURN with default\n" , ##ZZZ## + 1 ); \
\
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d* Scheduling Finish 12(RUN_ABORT:%d) for TM Module%cWHTM%dERROR 12\n" , ##ZZZ## + 1 , Res , 9 , ##ZZZ## + 1 ); /* 2012.01.19 */ \
\
				return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( ##ZZZ## , CHECKING_SIDE , TRUE ); \
				break; \
			} \
		} \
	} \
\
	_Trace_TM_Flow[CHECKING_SIDE][ ##ZZZ## ] = 199; \
\
	_i_SCH_SYSTEM_TM_SET( ##ZZZ## , CHECKING_SIDE , 0 ); \
	_endthread(); \
}



MAKE_SCHEDULER_MAIN_TM( 1 , 2 );
MAKE_SCHEDULER_MAIN_TM( 2 , 3 );
MAKE_SCHEDULER_MAIN_TM( 3 , 4 );
#ifndef PM_CHAMBER_12
MAKE_SCHEDULER_MAIN_TM( 4 , 5 );
MAKE_SCHEDULER_MAIN_TM( 5 , 6 );
#ifndef PM_CHAMBER_30
MAKE_SCHEDULER_MAIN_TM( 6 , 7 );
MAKE_SCHEDULER_MAIN_TM( 7 , 8 );
#endif
#endif


int Scheduler_Main_Running_Sub( int CHECKING_SIDE , int MAXTMATM0 ) {
	int i;
	int Res;
	int l_c;
	int dside;
	int abtelse;
	int MAXTMATM; // 2010.10.19
	int pt; // 2012.04.18
	//--------------------------------------------------
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.10
		MAXTMATM = 0;
	}
	else if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2010.10.18
		MAXTMATM = MAXTMATM0;
	}
	else {
		if ( ( _i_SCH_PRM_GET_EIL_INTERFACE() - 1 ) > MAXTMATM0 ) {
			MAXTMATM = _i_SCH_PRM_GET_EIL_INTERFACE() - 1;
		}
		else {
			MAXTMATM = MAXTMATM0;
		}
	}
	//
	// 2012.02.07
	_Trace_TM_Count[CHECKING_SIDE] = MAXTMATM;
	//
	if      ( MAXTMATM == 0 ) _Trace_Sleeping_Count[CHECKING_SIDE] = 5;
	else if ( MAXTMATM == 1 ) _Trace_Sleeping_Count[CHECKING_SIDE] = 4;
	else if ( MAXTMATM == 2 ) _Trace_Sleeping_Count[CHECKING_SIDE] = 3;
	else if ( MAXTMATM == 3 ) _Trace_Sleeping_Count[CHECKING_SIDE] = 2;
//	else if ( MAXTMATM == 4 ) _Trace_Sleeping_Count[CHECKING_SIDE] = 1; // 2018.06.01
//	else                      _Trace_Sleeping_Count[CHECKING_SIDE] = 0; // 2018.06.01
	else                      _Trace_Sleeping_Count[CHECKING_SIDE] = 1; // 2018.06.01
	//--------------------------------------------------
	_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1;
	//--------------------------------------------------
	_Trace_TM_Restart_Checked[CHECKING_SIDE][ 0 ] = 0; /* 2012.09.19 */
	//--------------------------------------------------
	_i_SCH_MODULE_Set_FM_SwWait( CHECKING_SIDE , 1 );
	//--------------------------------------------------
	// 2012.07.28
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i == CHECKING_SIDE ) continue;
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( _i_SCH_SYSTEM_PAUSE_GET( i ) == 2 ) break;
		//
	}
	if ( i != MAX_CASSETTE_SIDE ) {
		//
		_SCH_IO_SET_MAIN_BUTTON_MC( CHECKING_SIDE , CTC_PAUSING );
		//
		_i_SCH_SYSTEM_PAUSE_SET( CHECKING_SIDE , 2 );
		//
	}
	//
	//--------------------------------------------------
	//
//	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2012.04.09
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) ) { // 2012.04.09
		_i_SCH_MODULE_Set_FM_Runinig_Flag( CHECKING_SIDE , 1 );
		//
		if ( _beginthread( (void *) Scheduler_FEM_Running , 0 , (void *) CHECKING_SIDE ) == -1 ) {
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 2;
			//--------------------------------------------------
			_IO_CIM_PRINTF( "THREAD FAIL Scheduler_FEM_Running %d\n" , CHECKING_SIDE );
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* THREAD FAIL Scheduler_FEM_Running%cSCHTHREADFAIL\n" , 9 );
			//
			return SYS_ABORTED;
			//----------------------------------------------------------------------------------------------------------------
		}
	}
	//--------------------------------------------------
	_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 3;
	//--------------------------------------------------
	_Trace_TM_Flow_In[CHECKING_SIDE][0] = 0; // 2014.03.12
	//--------------------------------------------------
	_Trace_TM_Flow_Kill[CHECKING_SIDE][0] = 0; // 2010.07.29
	//--------------------------------------------------
	for ( i = 0 ; i < MAXTMATM ; i++ ) {
		//-------------
		_i_SCH_SYSTEM_TM_SET( i + 1 , CHECKING_SIDE , 1 );
		//-------------
		switch( i ) {
		case 0 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_1 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_1 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* THREAD FAIL Scheduler_Main_Running_TM_1%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
		case 1 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_2 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_2 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM2* THREAD FAIL Scheduler_Main_Running_TM_2%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
		case 2 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_3 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_3 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM3* THREAD FAIL Scheduler_Main_Running_TM_3%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
#ifndef PM_CHAMBER_12
		case 3 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_4 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_4 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM4* THREAD FAIL Scheduler_Main_Running_TM_4%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
		case 4 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_5 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_5 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM5* THREAD FAIL Scheduler_Main_Running_TM_5%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
#ifndef PM_CHAMBER_30
		case 5 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_6 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_6 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM6* THREAD FAIL Scheduler_Main_Running_TM_6%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
		case 6 :
			if ( _beginthread( (void *) Scheduler_Main_Running_TM_7 , 0 , (void *) CHECKING_SIDE ) == -1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 4;
				//--------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_TM_7 %d\n" , CHECKING_SIDE );
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM7* THREAD FAIL Scheduler_Main_Running_TM_7%cSCHTHREADFAIL\n" , 9 );
				return SYS_ABORTED;
			}
			break;
#endif
#endif
		}
		//-------------
	}
	//============================================================================================================
	//--------------------------------------------------
	_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 11;
	//--------------------------------------------------
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() >= 0 ) { // 2012.09.10
		//
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) _EIL_BEFORE_ENTER_CONTROL_TM( CHECKING_SIDE , 0 ); // 2010.09.13
		//--------------------------------------------------
		switch ( _SCH_COMMON_ENTER_CONTROL_TM( CHECKING_SIDE , 0 ) ) {
		case SYS_SUCCESS :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 12;
			//--------------------------------------------------
			return SYS_SUCCESS;
			break;
		case SYS_ABORTED :
		case SYS_ERROR :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 13;
			//--------------------------------------------------
			return SYS_ABORTED;
			break;
		case SYS_RUNNING :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 14;
			//--------------------------------------------------
			break;
		case -1 :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 15;
			//--------------------------------------------------
			break;
		default :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 16;
			//--------------------------------------------------
			break;
		}
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 21;
		//--------------------------------------------------
	}
	//============================================================================================================
	_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 10 );
	//============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 22;
		//--------------------------------------------------
		while( TRUE ) {
			if ( _i_SCH_SYSTEM_USING2_GET( CHECKING_SIDE ) >= 2 ) break;
			if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) <  0 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 23;
				//--------------------------------------------------
				return SYS_ABORTED;
			}
			if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) == 0 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 24;
				//--------------------------------------------------
				return SYS_SUCCESS;
			}
			Sleep(1);
		}
	}
	//============================================================================================================
	l_c = 0;
	//============================================================================================================
	Res = 0;
	//============================================================================================================
	while ( TRUE ) {
		//
		if ( _i_SCH_SYSTEM_USING_GET(CHECKING_SIDE) < 9 ) { // 2010.08.05
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 30;
			//--------------------------------------------------
			if ( ( Res == -1 ) || ( Res == -3 ) ) LeaveCriticalSection( &CR_TMRUN[0] );
			//--------------------------------------------------
			return SYS_SUCCESS;
		}
		//============================================================================================================
		if ( ( Res != -1 ) && ( Res != -3 ) ) {
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 31;
			//--------------------------------------------------
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(1) START\n" ); // 2014.03.12
			//============================================================================================================
			if ( Res != -4 ) {
				//
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() >= 0 ) { // 2012.09.10
					//
					if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) _EIL_BEFORE_ENTER_CONTROL_TM( CHECKING_SIDE , 1 ); // 2010.09.13
					//
					switch ( _SCH_COMMON_ENTER_CONTROL_TM( CHECKING_SIDE , 1 ) ) {
					case SYS_RUNNING :
						//--------------------------------------------------
						_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 33;
						//--------------------------------------------------
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(1) RETURN with SYS_RUNNING\n" );
						//============================================================================================================
						_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
						continue;
						break;
					case SYS_SUCCESS :
						//--------------------------------------------------
						_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 34;
						//--------------------------------------------------
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(1) RETURN with SYS_SUCCESS\n" );
						//============================================================================================================
						return SYS_SUCCESS;
						break;
					case SYS_ABORTED :
					case SYS_ERROR :
						//--------------------------------------------------
						_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 35;
						//--------------------------------------------------
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(1) RETURN with SYS_ABORTED\n" );
						//============================================================================================================
						return SYS_ABORTED;
						break;
					default :
						//--------------------------------------------------
						_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 36;
						//--------------------------------------------------
						//============================================================================================================
//						_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(1) RETURN with default\n" ); // 2014.03.12
						//============================================================================================================
						break;
					}
				}
				//
			}
			//=====================================================================================
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1041;
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_ENTER_TM_CRITICAL 1 [%d]\n" , Res ); // 2014.03.12
			//============================================================================================================
//			EnterCriticalSection( &CR_TMRUN[0] ); /* 2010.08.05 */
			if ( !TryEnterCriticalSection( &CR_TMRUN[0] ) ) { /* 2010.08.05 */
				//
				_Trace_TM_Flow_In[CHECKING_SIDE][0]++; // 2014.03.12
				//
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				continue;
			}
			//
			_Trace_TM_Flow_In[CHECKING_SIDE][0] = 0; // 2014.03.12
			//
			_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
			//
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1042;
			//--------------------------------------------------
			i = _i_SCH_SUB_Main_End_Check_Finished( CHECKING_SIDE );
			if ( i == 1 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 43;
				//--------------------------------------------------
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL END_CHECK RETURN with SUCCESS\n" );
				//============================================================================================================
				return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
			}
			if ( i == 2 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 44;
				//--------------------------------------------------
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL END_CHECK CONTINUE\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				continue;
			}
			//============================================================================================================
			//============================================================================================================
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1043;
			//--------------------------------------------------
			//============================================================================================================
			_i_SCH_SUB_RECIPE_READ_OPERATION( CHECKING_SIDE );
			//============================================================================================================
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( CHECKING_SIDE , FALSE , &i ); // 2011.06.13
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1044;
			//--------------------------------------------------
			//============================================================================================================
			//============================================================================================================
			switch ( _Trace_Check_Direct_Side_Invalid( 0 , CHECKING_SIDE ) ) {
			case 1 : // Invalid
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				//============================================================================================================
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 45;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM INVALID SIDE 45\n" );
				//============================================================================================================
				continue;
				break;
			case 2 : // Invalid and Warning
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				//============================================================================================================
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 45;
				//============================================================================================================
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Invalid Check Warning%cWHFMINVALID 1\n" , 9 ); // 2012.11.14
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* Scheduling Invalid Check Warning\n" ); // 2012.11.14
				//============================================================================================================
				continue;
				break;
			case 3 : // Invalid But Warning so Valid
				//============================================================================================================
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Invalid Check Rejecting%cWHFMINVALID 2\n" , 9 ); // 2012.11.14
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* Scheduling Invalid Check Rejecting\n" ); // 2012.11.14
				//============================================================================================================
				break;
			}
			//============================================================================================================
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1051;
			//--------------------------------------------------
			if ( _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() < 2 ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
					if ( !_SCH_COMMON_SIDE_SUPPLY( CHECKING_SIDE , FALSE , -2 , &i ) ) {
						if ( !_i_SCH_CASSETTE_Check_Side_Monitor( CHECKING_SIDE ) ) {
							//--------------------------------------------------
							_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 52;
							//--------------------------------------------------
							LeaveCriticalSection( &CR_TMRUN[0] );
							//============================================================================================================
							if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL 1\n" );
							//============================================================================================================
							continue;
						}
					}
				}
				else {
					if ( !_SCH_COMMON_SIDE_SUPPLY( CHECKING_SIDE , FALSE , -3 , &i ) ) {
						if ( !_i_SCH_CASSETTE_Check_Side_Monitor( CHECKING_SIDE ) ) {
							//--------------------------------------------------
							_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 53;
							//--------------------------------------------------
							LeaveCriticalSection( &CR_TMRUN[0] );
							//============================================================================================================
							if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL 1-1\n" );
							//============================================================================================================
							continue;
						}
					}
				}
			}
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1054;
			//--------------------------------------------------
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(1-1) END\n" ); // 2014.03.12
			//============================================================================================================
		}
		//--------------------------------------------------
		Res = 0;
		//--------------------------------------------------
		//============================================================================================================
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1061;
		//--------------------------------------------------

		/*
		//
		// 2017.02.04
		//
		for ( i = 0 ; i < MAXTMATM ; i++ ) {
			if ( _i_SCH_SYSTEM_TM_GET( i + 1 , CHECKING_SIDE ) < 0 ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 62;
				//--------------------------------------------------
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM%d* Scheduling Aborted ....%cWHTM%dFAIL\n" , i + 2 , 9 , i + 2 ); // 2012.01.19
				//
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 13(TM_TAG_FAIL_ABORT:%d) for TM Module%cWHTMERROR 13\n" , i+2 , 9 ); // 2012.01.19
				//
				return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
			}
		}
		*/
		//
		if ( Scheduler_Other_TM_Aborted( CHECKING_SIDE , 0 ) ) { // 2017.02.04
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 62;
			//--------------------------------------------------
			//
//			Scheduler_Other_TM_Finished( CHECKING_SIDE ); // 2017.04.11 // 2018.04.16
			Scheduler_Other_TM_Finished( CHECKING_SIDE , TRUE ); // 2017.04.11 // 2018.04.16
			//
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 63; // 2017.04.11
			//--------------------------------------------------
			//
			return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
		}
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1071;
		//--------------------------------------------------
		//============================================================================================================
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 11 );
		//============================================================================================================
		switch( _i_SCH_SYSTEM_ABORT_PAUSE_CHECK( CHECKING_SIDE ) ) {
		case -1 :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 72;
			//--------------------------------------------------
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 14(PAUSE_ABORT) for TM Module%cWHTMERROR 14\n" , 9 ); // 2012.01.19
			//
//			Scheduler_Other_TM_Finished( CHECKING_SIDE ); // 2017.04.11 // 2018.04.16
			Scheduler_Other_TM_Finished( CHECKING_SIDE , TRUE ); // 2017.04.11 // 2018.04.16
			//
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 74; // 2017.04.11
			//--------------------------------------------------
			//
			return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
			break;
		case 1 :
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 73;
			//--------------------------------------------------
			LeaveCriticalSection( &CR_TMRUN[0] );
			//============================================================================================================
//			_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL 2\n" ); // 2014.03.12
			//============================================================================================================
			_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
			continue;
			break;
		}
		//============================================================================================================
		_i_SCH_SUB_FMTMSIDE_OPERATION( CHECKING_SIDE , 1 ); // 2009.05.06 // 2010.03.06
		//============================================================================================================
		if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() != 0 ) { // 2010.06.14
			//
			EnterCriticalSection( &CR_WAIT ); // 2014.01.24
			//
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) {
//					if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) , CHECKING_SIDE ) ) { // 2011.12.06
//					if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) > 0 ) , CHECKING_SIDE ) ) { // 2011.12.06 // 2016.05.09
//					if ( !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) > 0 ) , CHECKING_SIDE ) ) { // 2011.12.06 // 2016.05.09 // 2018.12.20
					if ( !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) > 0 ) , CHECKING_SIDE + 1000 ) ) { // 2011.12.06 // 2016.05.09 // 2018.12.20
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_99_USE_to_DISABLE );
					}
				}
			}
			//
			LeaveCriticalSection( &CR_WAIT ); // 2014.01.24
			//
		}
		//============================================================================================================
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1081;
		//============================================================================================================
		if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(2) START\n" );
		//============================================================================================================
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() >= 0 ) { // 2012.09.10
			//
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) _EIL_BEFORE_ENTER_CONTROL_TM( CHECKING_SIDE , 2 ); // 2010.09.13
			//
			switch ( _SCH_COMMON_ENTER_CONTROL_TM( CHECKING_SIDE , 2 ) ) {
			case SYS_SUCCESS :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 82;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(2) RETURN with SYS_SUCCESS\n" );
				//============================================================================================================
				return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
				break;
			case SYS_ABORTED :
			case SYS_ERROR :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 83;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(2) RETURN with SYS_ABORTED\n" );
				//============================================================================================================
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 15(ENTER_ABORT) for TM Module%cWHTMERROR 15\n" , 9 ); // 2012.01.19
				//
				//--------------------------------------------------
				//
//				Scheduler_Other_TM_Finished( CHECKING_SIDE ); // 2017.04.11 // 2018.04.16
				Scheduler_Other_TM_Finished( CHECKING_SIDE , TRUE ); // 2017.04.11 // 2018.04.16
				//
				//--------------------------------------------------
				//
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 81; // 2017.04.11
				//
				//--------------------------------------------------
				return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
				break;
			case SYS_RUNNING :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 84;
				//--------------------------------------------------
				LeaveCriticalSection( &CR_TMRUN[0] );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(2) RETURN with SYS_RUNNING\n" );
				//============================================================================================================
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				continue;
				break;
			case -1 :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1085;
				//--------------------------------------------------
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(2) RETURN with -1\n" );
				//============================================================================================================
				//============================================================================================================
	//			_SCH_MACRO_INTERRUPT_PART_CHECK( CHECKING_SIDE , FALSE , 0 ); // 2008.07.28
				_SCH_MACRO_INTERRUPT_PART_CHECK( CHECKING_SIDE , FALSE , -1 ); // 2008.07.28
				//============================================================================================================
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1086;
				//--------------------------------------------------
	/*
	// 2011.07.07
				if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , ( _i_SCH_SYSTEM_RUNCHECK_FM_GET() - 1 ) ==  CHECKING_SIDE ) ) {
					//--------------------------------------------------
					_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 87;
					//--------------------------------------------------
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish for TM Module%cWHTMSUCCESS 1\n" , 9 );
					return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
				}
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1088;
				//--------------------------------------------------
	*/
				//============================================================================================================
				_i_SCH_SUB_Chk_GLOBAL_STOP_FDHC( -1 );
				//============================================================================================================
				break;
			default :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1089;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM ENTER_CONTROL(2) RETURN with default\n" );
				//============================================================================================================
				break;
			}
		}
		//=====================================================================================
		// 2011.07.07
		//=====================================================================================
		if ( TryEnterCriticalSection( &CR_FEMRUN ) ) { // 2011.09.03
			//
			if ( _SCH_MACRO_FM_FINISHED_CHECK( CHECKING_SIDE , ( _i_SCH_SYSTEM_RUNCHECK_FM_GET() - 1 ) ==  CHECKING_SIDE ) ) {
				//============================================================================================================
				LeaveCriticalSection( &CR_FEMRUN );
				//============================================================================================================
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 87;
				//--------------------------------------------------
				//
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish for TM Module%cWHTMSUCCESS 1\n" , 9 ); // 2012.01.19
//				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 2(FINISH2) for TM Module%cWHTMSUCCESS 2\n" , 9 ); // 2012.01.19 // 2017.04.12
//				return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE ); // 2017.04.12
				//
				//
//				Scheduler_Other_TM_Finished( CHECKING_SIDE ); // 2017.04.12 // 2018.04.16
				Scheduler_Other_TM_Finished( CHECKING_SIDE , FALSE ); // 2017.04.12 // 2018.04.16
				//
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 88; // 2017.04.12
				//--------------------------------------------------
				//
				if ( ( _Trace_FEM_Other_RunPart_Abort_Check[CHECKING_SIDE] != 0 ) || ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) < 0 ) ) { // 2017.04.12
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 3(FINISH3) for TM Module%cWHTMSUCCESS 3\n" , 9 ); // 2012.01.19 // 2017.04.12
					return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE ); // 2017.04.12
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 2(FINISH2) for TM Module%cWHTMSUCCESS 2\n" , 9 ); // 2012.01.19 // 2017.04.12
					return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE ); // 2017.04.12
				}
				//
			}
			//============================================================================================================
			LeaveCriticalSection( &CR_FEMRUN );
			//============================================================================================================
		}
		//=====================================================================================
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1091;
		//--------------------------------------------------
		//=====================================================================================
//		if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) < 0 ) { // 2017.04.12
		if ( ( _Trace_FEM_Other_RunPart_Abort_Check[CHECKING_SIDE] != 0 ) || ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) < 0 ) ) { // 2017.04.12
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 92;
			//--------------------------------------------------
//			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Aborted 2 ....%cWHTMFAIL 2\n" , 9 ); // 2012.01.19
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 16(FM_RUN_TAG_FAIL:%d) for TM Module%cWHTMERROR 16\n" , _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) , 9 ); // 2012.01.19
			//
//			Scheduler_Other_TM_Finished( CHECKING_SIDE ); // 2017.04.11 // 2018.04.16
			Scheduler_Other_TM_Finished( CHECKING_SIDE , TRUE ); // 2017.04.11 // 2018.04.16
			//
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 93; // 2017.04.11
			//--------------------------------------------------
			//
			return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
		}
		//--------------------------------------------------
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1093;
		//--------------------------------------------------
		//=====================================================================================
/*
		// 2010.06.14
		if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() != 0 ) {
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) {
					if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) , CHECKING_SIDE ) ) {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_99_USE_to_DISABLE );
					}
				}
			}
		}
*/
		//============================================================================================================
//		_i_SCH_SUB_FMTMSIDE_OPERATION( CHECKING_SIDE , 1 ); // 2009.05.06 // 2010.03.06
		//============================================================================================================
		_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1101;
		//============================================================================================================
//		_i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL START\n" );
		//============================================================================================================
		_i_SCH_SYSTEM_RUNCHECK_SET( CHECKING_SIDE + 1 );
		//============================================================================================================
		// 2012.09.19
		//
		Res = SYS_RUNNING;
		//
		if ( _Trace_TM_Restart_Checked[CHECKING_SIDE][0] == 0 ) {
			//
			_Trace_TM_Restart_Checked[CHECKING_SIDE][0] = 1;
			//
			if ( _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) != 0 ) {
				//
				if ( USER_DLL_SCH_INF_HAS_RESTART_PRE_CONTROL_TM_1() ) {
					//============================================================================================================
					_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1102;
					//============================================================================================================
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RESTART_PRE_CONTROL START %d\n" , _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) );
					//============================================================================================================
					Res = USER_DLL_SCH_INF_RESTART_PRE_CONTROL_TM_1( CHECKING_SIDE , _i_SCH_SYSTEM_RESTART_GET( CHECKING_SIDE ) , 0 );
					//============================================================================================================
					if ( Res != SYS_ABORTED ) SCH_RESTART_DATA_REMAP_AFTER_USER_PART( CHECKING_SIDE , 0 ); /* 2013.06.19 */
					//============================================================================================================
				}
			}
			//
		}
		//============================================================================================================
		if ( Res == SYS_RUNNING ) { // 2012.09.19
			//
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() >= 0 ) { // 2012.09.10
				//
				if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) _EIL_BEFORE_RUN_CONTROL_TM_1( CHECKING_SIDE ); // 2010.09.13
				//============================================================================================================
				//-------------------------------------------------------------------
				// 2012.04.18
				//-------------------------------------------------------------------
				Res = SYS_RUNNING;
				//
				if ( USER_DLL_SCH_INF_HAS_USER_CONTROL_TM() ) {
					if ( _i_SCH_USER_POSSIBLE_PICK_FROM_TM( CHECKING_SIDE , &pt ) ) {
						//
						_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1111;
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM USER_CONTROL START %d [%d:%s]\n" , pt , _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( CHECKING_SIDE , pt ) , _i_SCH_CLUSTER_Get_Ex_UserControl_Data( CHECKING_SIDE , pt ) );
						//============================================================================================================
						Res = USER_DLL_SCH_INF_USER_CONTROL_TM(
							CHECKING_SIDE ,
							pt ,
							_i_SCH_CLUSTER_Get_Ex_UserControl_Mode( CHECKING_SIDE , pt ) ,
							_i_SCH_CLUSTER_Get_Ex_UserControl_Data( CHECKING_SIDE , pt ) );
						//
					}
					else {
						//============================================================================================================
						if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM USER_CONTROL UNMATCHED %d\n" , pt );
						//============================================================================================================
					}
				}
				//
				if ( Res == SYS_RUNNING )	{
					//
					_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1112;
					//============================================================================================================
					if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL START\n" );
					//============================================================================================================
					Res = _SCH_COMMON_RUN_CONTROL_TM_1( CHECKING_SIDE );
				}
				else if ( Res == SYS_SUCCESS )	{
					Res = SYS_RUNNING;
				}
			}
			else { // 2012.09.10
				Res = _EIL_FMMODE_RUN_CONTROL_TM( CHECKING_SIDE );
			}
		}
		else if ( Res == SYS_SUCCESS )	{ // 2012.09.19
			Res = SYS_RUNNING;
		}
		//============================================================================================================
		// 2015.08.26
		SCH_TM_PICKPLACING_SET( -1 , -1 , 0 , -1 , CHECKING_SIDE , -1 );
		//============================================================================================================
		abtelse = 0;
		//============================================================================================================
		if (
			( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) ||
			( Res == -4 ) || ( Res == -3 ) || ( Res == -2 ) || ( Res == -1 ) || ( Res == SYS_RUNNING )
			) { /* 2011.04.20 */ \
			dside = SCHEDULER_CONTROL_Move_Data_from_to( CHECKING_SIDE , 0 );
			if      ( dside >  1 ) {
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM Move_Data_from_to is %d\n" , dside );
			}
			else if ( dside == 1 ) {
			}
			else if ( dside == 0 ) {
				Res = SYS_RUNNING;
			}
			else {
				abtelse = -dside;
				Res = SYS_ABORTED;
			}
		}
		//============================================================================================================
		if ( ( Res <= -100 ) && ( Res >= ( -100 - MAX_CASSETTE_SIDE + 1 ) ) ) { // nosleep , leave & Direct->dside
			//--------------------------------------------------
			dside = -100 - Res;
			//--------------------------------------------------
			_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1300 + dside;
			//============================================================================================================
			if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with %d\n" , Res );
			//============================================================================================================
			//--------------------------------------------------
			_Trace_Set_Direct_Side( 0 , dside );
			//--------------------------------------------------
			if ( CHECKING_SIDE == dside ) {
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1500 + dside;
				//--------------------------------------------------
				Res = -3;
				//--------------------------------------------------
			}
			else {
				//--------------------------------------------------
				Res = -4;
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 500 + dside;
				//--------------------------------------------------
				_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL %d\n" , Res );
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				//--------------------------------------------------
			}
		}
		else {
			switch( Res ) {
			case -4 : // nosleep , leave , NCEnter(1)Part_Skip
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1204;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with -4\n" );
				//============================================================================================================
				//--------------------------------------------------
				_Trace_Set_Direct_Side( 0 , -1 );
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 304;
				//--------------------------------------------------
				_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL -4\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				//--------------------------------------------------
				break;
			case -3 : // nosleep , noleave(same)
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1203;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with -3\n" );
				//============================================================================================================
				//--------------------------------------------------
				_Trace_Set_Direct_Side( 0 , -1 );
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1303;
				//--------------------------------------------------
				break;
			case -2 : // nosleep , leave
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1202;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with -2\n" );
				//============================================================================================================
				//--------------------------------------------------
				_Trace_Set_Direct_Side( 0 , -1 );
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 302;
				//--------------------------------------------------
				_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL -2\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				//--------------------------------------------------
				break;
			case -1 : // sleep , noleave(same)
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1201;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with -1\n" );
				//============================================================================================================
				//--------------------------------------------------
				_Trace_Set_Direct_Side( 0 , -1 );
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1301;
				//--------------------------------------------------
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				//--------------------------------------------------
				break;
			case SYS_RUNNING : // sleep , leave
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 1102;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with SYS_RUNNING\n" );
				//============================================================================================================
				//--------------------------------------------------
				_Trace_Set_Direct_Side( 0 , -1 );
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 105;
				//--------------------------------------------------
				_i_SCH_SYSTEM_RUNCHECK_SET( 0 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM _SCH_SYSTEM_LEAVE_TM_CRITICAL def\n" );
				//============================================================================================================
				LeaveCriticalSection( &CR_TMRUN[0] );
				//--------------------------------------------------
				_Sleep_LC( &l_c , _Trace_Sleeping_Count[CHECKING_SIDE] );
				//--------------------------------------------------
				break;
			case SYS_SUCCESS :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 103;
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with SYS_SUCCESS\n" );
				//============================================================================================================
				return _i_SCH_SUB_SUCCESS_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
				break;
			default :
				//--------------------------------------------------
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 104 + ( abtelse * 1000 );
				//============================================================================================================
				if ( IO_DEBUG_MONITOR_STATUS <= 10 ) _i_SCH_LOG_DEBUG_PRINTF( CHECKING_SIDE , 1 , "TM1* SYSTEM RUN_CONTROL RETURN with %d\n" , Res );
				//============================================================================================================
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "TM1* Scheduling Finish 17(RUN_ABORT:%d) for TM Module%cWHTMERROR 17\n" , Res , 9 ); // 2012.01.19
				//
//				Scheduler_Other_TM_Finished( CHECKING_SIDE ); // 2017.02.07 // 2018.04.16
				Scheduler_Other_TM_Finished( CHECKING_SIDE , TRUE ); // 2017.02.07 // 2018.04.16
				//
				_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 105 + ( abtelse * 1000 ); // 2017.02.08
				//
				return _i_SCH_SUB_ERROR_FINISH_RETURN_FOR_TM( 0 , CHECKING_SIDE , FALSE );
				break;
			}
		}
	}
	//--------------------------------------------------
	_Trace_TM_Flow[CHECKING_SIDE][ 0 ] = 199;
	//--------------------------------------------------
	return SYS_SUCCESS;
}





