#include "default.h"

#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "Timer_Handling.h"

//=================================================================================
int _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( int ch , int * );
//=================================================================================
//time_t Process_Progress_Run_Timer[3][ MAX_CHAMBER ]; // 2013.03.18
long   Process_Progress_Run_Timer[3][ MAX_CHAMBER ]; // 2013.03.18
time_t Process_Progress_Start_Timer[3][ MAX_CHAMBER ];
time_t Process_Progress_End_Timer[3][ MAX_CHAMBER ];

//time_t Robot_Progress_Run_Timer_FM[ MAX_FINGER_FM + 1 ]; // 2013.03.18
long   Robot_Progress_Run_Timer_FM[ MAX_FINGER_FM + 1 ]; // 2013.03.18
time_t Robot_Progress_Start_Timer_FM[ MAX_FINGER_FM + 1 ];
time_t Robot_Progress_End_Timer_FM[ MAX_FINGER_FM + 1 ];
//
//time_t Robot_Progress_Run_Timer_TM[MAX_TM_CHAMBER_DEPTH][ MAX_FINGER_TM + 1 ]; // 2013.03.18
long   Robot_Progress_Run_Timer_TM[MAX_TM_CHAMBER_DEPTH][ MAX_FINGER_TM + 1 ]; // 2013.03.18
time_t Robot_Progress_Start_Timer_TM[MAX_TM_CHAMBER_DEPTH][ MAX_FINGER_TM + 1 ];
time_t Robot_Progress_End_Timer_TM[MAX_TM_CHAMBER_DEPTH][ MAX_FINGER_TM + 1 ];
//
//
//time_t MFIC_Progress_Timer[ MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.22
//int    MFIC_Progress_Target[ MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.22
time_t MFIC_Progress_Timer[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.22
int    MFIC_Progress_Target[ MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ]; // 2007.06.22

time_t MFAL_Progress_Timer;

//time_t BM_Progress_Timer[ MAX_CHAMBER ]; // 2016.12.09
//time_t BM_Target_Timer[ MAX_CHAMBER ]; // 2016.12.09

DWORD BM_Start_Timer[ MAX_CHAMBER ]; // 2016.12.09
int   BM_Target_Timer[ MAX_CHAMBER ]; // 2016.12.09

//=================================================================================
// 2006.07.20
//=================================================================================
//time_t Robot_Progress_Run_Timer_FM_PICK[MAX_FM_CHAMBER_DEPTH][2]; // 2013.03.18
long   Robot_Progress_Run_Timer_FM_PICK[MAX_FM_CHAMBER_DEPTH][2]; // 2013.03.18
time_t Robot_Progress_Start_Timer_FM_PICK[MAX_FM_CHAMBER_DEPTH][2];
//
//time_t Robot_Progress_Run_Timer_FM_PLACE[MAX_FM_CHAMBER_DEPTH][2]; // 2013.03.18
long   Robot_Progress_Run_Timer_FM_PLACE[MAX_FM_CHAMBER_DEPTH][2]; // 2013.03.18
time_t Robot_Progress_Start_Timer_FM_PLACE[MAX_FM_CHAMBER_DEPTH][2];
//
//time_t Robot_Progress_Run_Timer_TM_PICK[MAX_TM_CHAMBER_DEPTH][2]; // 2013.03.18
long   Robot_Progress_Run_Timer_TM_PICK[MAX_TM_CHAMBER_DEPTH][2]; // 2013.03.18
time_t Robot_Progress_Start_Timer_TM_PICK[MAX_TM_CHAMBER_DEPTH][2];
//
//time_t Robot_Progress_Run_Timer_TM_PLACE[MAX_TM_CHAMBER_DEPTH][2]; // 2013.03.18
long   Robot_Progress_Run_Timer_TM_PLACE[MAX_TM_CHAMBER_DEPTH][2]; // 2013.03.18
time_t Robot_Progress_Start_Timer_TM_PLACE[MAX_TM_CHAMBER_DEPTH][2];
//=================================================================================
DWORD Process_Equip_Timer[ MAX_CHAMBER ]; // 2008.09.27
//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
long _i_SCH_TIMER_GET_PROCESS_TIME_START( int mode , int chamber , BOOL real ) {
	int d;
	//
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	//
	if ( ( mode == 0 ) && ( _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( chamber , &d ) > 0 ) ) { // 2012.03.21 // 2012.08.21
		return 999999999;
	}
	//
	if ( ( _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START() == 2 ) && !real ) return 0;
	//
	return (long) Process_Progress_Start_Timer[mode][ chamber ]; // 2013.03.18
}
//
long _i_SCH_TIMER_GET_PROCESS_TIME_END( int mode , int chamber , BOOL real ) {
	if ( ( mode < 0 ) || ( mode > 2 ) || ( ( _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START() == 2 ) && !real ) ) return 0;
	return (long) Process_Progress_End_Timer[mode][ chamber ]; // 2013.03.18
}
//
long _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( int mode , int chamber ) {
	int d;
	long l;
	time_t finish;
	//
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	//
	if ( ( mode == 0 ) && ( _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( chamber , &d ) > 0 ) ) { // 2012.03.21 // 2012.08.21
		return 999999999;
	}
	//
	time( &finish );
	l = Process_Progress_Run_Timer[mode][chamber] - (long) difftime( finish , Process_Progress_Start_Timer[mode][chamber] );
	if ( l < 0 ) l = 0;
	return l;
}
//
long _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( int mode , int chamber ) {
	int d;
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	//
	if ( ( mode == 0 ) && ( _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( chamber , &d ) > 0 ) ) { // 2012.03.21 // 2012.08.21
		return 999999999;
	}
	//
	return Process_Progress_Run_Timer[mode][chamber];
}
//
long _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN_for_PE( int mode , int chamber ) { // 2013.04.01
	long l;
	time_t finish;
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	time( &finish );
	l = Process_Progress_Run_Timer[mode][chamber] - (long) difftime( finish , Process_Progress_Start_Timer[mode][chamber] );
	if ( l < 0 ) l = 0;
	return l;
}

long _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( int mode , int chamber ) { // 2012.09.26
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	return Process_Progress_Run_Timer[mode][chamber];
}
//
long _i_SCH_TIMER_GET_PROCESS_TIME_ELAPSED( int mode , int chamber ) {
	long l;
	time_t finish;
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	time( &finish );
	l = (long) difftime( finish , Process_Progress_Start_Timer[mode][chamber] );
	if ( l < 0 ) l = 0;
	return l;
}
//
void _i_SCH_TIMER_SET_PROCESS_TIME_START( int mode , int chamber ) {
	if ( ( mode < 0 ) || ( mode > 2 ) ) return;
	time( &Process_Progress_Start_Timer[mode][chamber] );
}
//
void _i_SCH_TIMER_SET_PROCESS_TIME_END( int mode , int chamber , BOOL Success ) {
	if ( ( mode < 0 ) || ( mode > 2 ) ) return;
	time( &Process_Progress_End_Timer[mode][chamber] );
	if ( Success ) {
		if ( Process_Progress_Start_Timer[mode][chamber] != 0 ) { // 2006.06.27
			Process_Progress_Run_Timer[mode][chamber] = (long) difftime( Process_Progress_End_Timer[mode][chamber] , Process_Progress_Start_Timer[mode][chamber] );
		}
	}
}
//
long _i_SCH_TIMER_GET_PROCESS_TIME_END_ELAPSED( int mode , int chamber ) { // 2014.11.14
	long l;
	time_t finish;
	if ( ( mode < 0 ) || ( mode > 2 ) ) return 0;
	time( &finish );
	l = (long) difftime( finish , Process_Progress_End_Timer[mode][chamber] );
	if ( l < 0 ) l = 0;
	return l;
}
//
//
/*
// 2012.09.26
void _i_SCH_TIMER_SET_PROCESS_TIME_SKIP( int mode , int chamber ) { // 2006.02.10
	if ( ( mode < 0 ) || ( mode > 2 ) ) return;
	time( &Process_Progress_Start_Timer[mode][chamber] );
	Process_Progress_End_Timer[mode][chamber] = Process_Progress_Start_Timer[mode][chamber];
	Process_Progress_Run_Timer[mode][chamber] = 0;
}
*/

// 2012.09.26
void _iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( int ch , int data );

void _i_SCH_TIMER_SET_PROCESS_TIME_SKIP( int mode , int chamber ) { // 2006.02.10
	//
	if      ( mode == 10 ) {
		_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( chamber , FALSE );
		return;
	}
	else if ( mode == 11 ) {
		_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( chamber , TRUE );
		return;
	}
	/*
	// 2012.11.09
	else if ( mode == 12 ) {
		_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( -1 , FALSE );
		return;
	}
	else if ( mode == 13 ) {
		_iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( -1 , TRUE );
		return;
	}
	*/
	//
	if ( ( mode < 0 ) || ( mode > 2 ) ) return;
	time( &Process_Progress_Start_Timer[mode][chamber] );
	Process_Progress_End_Timer[mode][chamber] = Process_Progress_Start_Timer[mode][chamber];
	Process_Progress_Run_Timer[mode][chamber] = 0;
}


//
void _i_SCH_TIMER_SET_PROCESS_TIME_TARGET( int mode , int chamber , int trgtime ) { // 2006.02.09
	if ( ( mode < 0 ) || ( mode > 2 ) ) return;
	if ( trgtime >= 0 ) Process_Progress_Run_Timer[mode][chamber] = trgtime;
}
//-------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void _i_SCH_TIMER_SET_ROBOT_TIME_START( int TMATM , int finger ) {
	if ( TMATM == -1 ) {
		time( &Robot_Progress_Start_Timer_FM[finger] );
		//
		time( &Robot_Progress_Start_Timer_FM[MAX_FINGER_FM] ); // 2009.09.28
	}
	else {
		time( &Robot_Progress_Start_Timer_TM[TMATM][finger] );
		//
		time( &Robot_Progress_Start_Timer_TM[TMATM][MAX_FINGER_TM] ); // 2009.09.28
	}
}
//
void _i_SCH_TIMER_SET_ROBOT_TIME_END( int TMATM , int finger ) {
	if ( TMATM == -1 ) {
		time( &Robot_Progress_End_Timer_FM[finger] );
		//
		time( &Robot_Progress_End_Timer_FM[MAX_FINGER_FM] ); // 2009.09.28
		//
		Robot_Progress_Run_Timer_FM[finger] = (long) difftime( Robot_Progress_End_Timer_FM[finger] , Robot_Progress_Start_Timer_FM[finger] );
		//
		Robot_Progress_Run_Timer_FM[MAX_FINGER_FM] = (long) difftime( Robot_Progress_End_Timer_FM[MAX_FINGER_FM] , Robot_Progress_Start_Timer_FM[MAX_FINGER_FM] ); // 2009.09.28
	}
	else {
		time( &Robot_Progress_End_Timer_TM[TMATM][finger] );
		//
		time( &Robot_Progress_End_Timer_TM[TMATM][MAX_FINGER_TM] ); // 2009.09.28
		//
		Robot_Progress_Run_Timer_TM[TMATM][finger] = (long) difftime( Robot_Progress_End_Timer_TM[TMATM][finger] , Robot_Progress_Start_Timer_TM[TMATM][finger] );
		//
		Robot_Progress_Run_Timer_TM[TMATM][MAX_FINGER_TM] = (long) difftime( Robot_Progress_End_Timer_TM[TMATM][MAX_FINGER_TM] , Robot_Progress_Start_Timer_TM[TMATM][MAX_FINGER_TM] ); // 2009.09.28
	}
}
//
long _i_SCH_TIMER_GET_ROBOT_TIME_REMAIN( int TMATM , int finger ) {
	long l;
	time_t finish;
	time( &finish );
	if ( TMATM == -1 ) {
		if ( finger < 0 ) { // 2009.09.28
			l = Robot_Progress_Run_Timer_FM[MAX_FINGER_FM] - (long) difftime( finish , Robot_Progress_Start_Timer_FM[MAX_FINGER_FM] );
		}
		else {
			l = Robot_Progress_Run_Timer_FM[finger] - (long) difftime( finish , Robot_Progress_Start_Timer_FM[finger] );
		}
	}
	else {
		if ( finger < 0 ) { // 2009.09.28
			l = Robot_Progress_Run_Timer_TM[TMATM][MAX_FINGER_TM] - (long) difftime( finish , Robot_Progress_Start_Timer_TM[TMATM][MAX_FINGER_TM] );
		}
		else {
			l = Robot_Progress_Run_Timer_TM[TMATM][finger] - (long) difftime( finish , Robot_Progress_Start_Timer_TM[TMATM][finger] );
		}
	}
	if ( l < 0 ) l = 0;
	return l;
}
//
long _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( int TMATM , int finger ) {
	long l;
	time_t finish;
	time( &finish );
	if ( TMATM == -1 ) {
		if ( finger < 0 ) { // 2009.09.28
			l = (long) difftime( finish , Robot_Progress_Start_Timer_FM[MAX_FINGER_FM] );
		}
		else {
			l = (long) difftime( finish , Robot_Progress_Start_Timer_FM[finger] );
		}
	}
	else {
		if ( finger < 0 ) { // 2009.09.28
			l = (long) difftime( finish , Robot_Progress_Start_Timer_TM[TMATM][MAX_FINGER_TM] );
		}
		else {
			l = (long) difftime( finish , Robot_Progress_Start_Timer_TM[TMATM][finger] );
		}
	}
	if ( l < 0 ) l = 0;
	return l;
}
//----------------------------------------------------------------------------------------------------------------
//
int  KPLT_GET_MFIC_TRG_TIMER( int slot ) {
	return MFIC_Progress_Target[ slot - 1 ];
}
//
void KPLT_RESET_MFIC_RUN_TIMER( int slot1 , int slot2 , int targettime , BOOL targetsetonly ) {
	time_t finish;
	time( &finish );
//	if ( ( slot1 > 0 ) && ( slot1 <= MAX_CASSETTE_SLOT_SIZE ) ) { // 2007.06.22
	if ( ( slot1 > 0 ) && ( slot1 <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2007.06.22
		MFIC_Progress_Target[ slot1 - 1 ] = targettime;
		if ( !targetsetonly ) MFIC_Progress_Timer[ slot1 - 1 ] = finish;
	}
	//
//	if ( ( slot2 > 0 ) && ( slot2 <= MAX_CASSETTE_SLOT_SIZE ) ) { // 2007.06.22
	if ( ( slot2 > 0 ) && ( slot2 <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2007.06.22
		MFIC_Progress_Target[ slot2 - 1 ] = targettime;
		if ( !targetsetonly ) MFIC_Progress_Timer[ slot2 - 1 ] = finish;
	}
}
//
void KPLT_RESET_MFIC_CURRTRG_TIMER( int slot1 , BOOL curr ) { // 2007.07.03
	int i;
	if ( slot1 == -1 ) {
		for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) {
			if ( curr ) {
//				MFIC_Progress_Timer[ i ] = 0; // 2007.09.27
				MFIC_Progress_Target[ i ] = 0; // 2007.09.27
			}
			else {
				if ( MFIC_Progress_Target[ i ] >= 0 ) {
					MFIC_Progress_Target[ i ] = MFIC_Progress_Target[ i ] + 30;
				}
			}
		}
	}
	else {
		if ( ( slot1 > 0 ) && ( slot1 <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) {
			if ( curr ) {
//				MFIC_Progress_Timer[ slot1 - 1 ] = 0; // 2007.09.27
				MFIC_Progress_Target[ slot1 - 1 ] = 0; // 2007.09.27
			}
			else {
				if ( MFIC_Progress_Target[ slot1 - 1 ] >= 0 ) {
					MFIC_Progress_Target[ slot1 - 1 ] = MFIC_Progress_Target[ slot1 - 1 ] + 30;
				}
			}
		}
	}
}
//
BOOL KPLT_CHECK_MFIC_RUN_TIMER1( int slot , time_t finish ) {
	long l;
	//
	//=======================================================================
	if ( MFIC_Progress_Timer[ slot - 1 ] == 0 ) return TRUE;
	if ( MFIC_Progress_Target[ slot - 1 ] == 0 ) return TRUE;
	//=======================================================================
	if ( MFIC_Progress_Target[ slot - 1 ] < 0 ) return FALSE; // 2007.07.03
	//=======================================================================
	l = (long) difftime( finish , MFIC_Progress_Timer[slot-1] );
	if ( l < 0 ) return FALSE;
	if ( l >= MFIC_Progress_Target[ slot - 1 ] ) return TRUE;
	return FALSE;
}
//
int KPLT_CHECK_MFIC_RUN_TIMER2( int slot , time_t finish , int *target ) { // 2005.12.01
	if ( MFIC_Progress_Timer[slot-1] == 0 ) {
		*target = 0;
		return 0;
	}
	*target = MFIC_Progress_Target[ slot - 1 ];
	return ( (int) difftime( finish , MFIC_Progress_Timer[ slot - 1 ] ) );
}
//
int KPLT_CHECK_MFIC_HOWLONG_TIMER( int slot1 , int slot2 , time_t finish ) { // 2005.11.28
	double l1 , l2;
	//=========================================================
	if ( ( MFIC_Progress_Timer[slot1-1] == 0 ) && ( MFIC_Progress_Timer[slot2-1] == 0 ) ) {
		return 0;
	}
	else if ( MFIC_Progress_Timer[slot1-1] == 0 ) {
		return 1;
	}
	else if ( MFIC_Progress_Timer[slot2-1] == 0 ) {
		return -1;
	}
	//=========================================================
	l1 = difftime( finish , MFIC_Progress_Timer[slot1-1] );
	l2 = difftime( finish , MFIC_Progress_Timer[slot2-1] );
	//=========================================================
	// within COOLING_GAP_SAME_TIME_ZONE sec same time
	if      ( l1 > l2 ) {
		if ( ( l1 - l2 ) <= COOLING_GAP_SAME_TIME_ZONE ) return 0;
		return 1;
	}
	else if ( l1 < l2 ) {
		if ( ( l2 - l1 ) <= COOLING_GAP_SAME_TIME_ZONE ) return 0;
		return -1;
	}
	return 0;
}
//
void KPLT_CHECK_MFIC_START_TIME( int slot , char *target ) { // 2007.07.04
	struct tm *time_data2;
	if ( MFIC_Progress_Timer[slot-1] == 0 ) {
		strcpy( target , "" );
	}
	else {
		time_data2 = localtime( &(MFIC_Progress_Timer[slot-1]) );
		sprintf( target , "%04d/%02d/%02d %02d:%02d:%02d" , time_data2->tm_year+1900 , time_data2->tm_mon + 1 , time_data2->tm_mday , time_data2->tm_hour , time_data2->tm_min , time_data2->tm_sec );
	}
}
//
void KPLT_RESET_MFAL_RUN_TIMER() {
	time( &MFAL_Progress_Timer );
}
//
BOOL KPLT_CHECK_MFAL_RUN_TIMER( long timevalue ) {
	time_t finish;
	long l;
	if ( MFAL_Progress_Timer == 0 ) return TRUE;
	if ( timevalue == 0 ) return TRUE;
	time( &finish );
	l = (long) difftime( finish , MFAL_Progress_Timer );
	if ( l < 0 ) return FALSE;
	if ( l >= timevalue ) return TRUE;
	return FALSE;
}
//


/*
//
// 2016.12.09
//
void KPLT_RESET_BM_RUN_TIMER( int ch , long timevalue ) {
	time( &BM_Progress_Timer[ch] );
	BM_Target_Timer[ch] = timevalue;
}
//
BOOL KPLT_CHECK_BM_RUN_TIMER( int ch ) {
	time_t finish;
	long l;
	if ( BM_Progress_Timer[ch] == 0 ) return TRUE;
	if ( BM_Target_Timer[ch] == 0 ) return TRUE;
	time( &finish );
	l = (long) difftime( finish , BM_Progress_Timer[ch] );
	if ( l < 0 ) return FALSE;
	if ( l >= BM_Target_Timer[ch] ) return TRUE;
	return FALSE;
}
//
*/
//
//

DWORD BM_Start_Timer[ MAX_CHAMBER ]; // 2016.12.09
int   BM_Target_Timer[ MAX_CHAMBER ]; // 2016.12.09

// 2016.12.09
//
void KPLT_RESET_BM_RUN_MS_TIMER( int ch , int targetvalue ) {
	BM_Target_Timer[ch] = targetvalue;
	BM_Start_Timer[ch] = GetTickCount();
}
//
BOOL KPLT_CHECK_BM_RUN_MS_TIMER( int ch ) {
	DWORD t;
	t = GetTickCount() - BM_Start_Timer[ ch ];
	if ( t >= (DWORD) BM_Target_Timer[ch] ) return TRUE;
	return FALSE;
}
//
//=================================================================================
//=================================================================================
//=================================================================================
void KPLT_Current_Time( char *buffer ) {
//	struct tm		Pres_Time; // 2006.10.02
//	_get-systime( &Pres_Time ); // 2006.10.02
//	sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d" , Pres_Time.tm_year+1900 , Pres_Time.tm_mon + 1 , Pres_Time.tm_mday , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec ); // 2006.10.02
	SYSTEMTIME		SysTime; // 2006.10.02
	GetLocalTime( &SysTime ); // 2006.10.02
	//
	switch( GET_DATE_TIME_FORMAT() / 100 ) { // 2019.01.18
	case 1 :
		sprintf( buffer , "%d/%d/%d %d:%d:%d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ); // 2006.10.02
		break;
	case 2 :
		sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ); // 2006.10.02
		break;
	case 3 :
		sprintf( buffer , "%d-%d-%d %d:%d:%d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ); // 2006.10.02
		break;
	case 4 :
		sprintf( buffer , "%04d-%02d-%02d %02d:%02d:%02d.%03d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ); // 2006.10.02
		break;
	case 5 :
		sprintf( buffer , "%d/%d/%d %d:%d:%d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // 2006.10.02
		break;
	case 6 :
		sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // 2006.10.02
		break;
	case 7 :
		sprintf( buffer , "%d-%d-%d %d:%d:%d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // 2006.10.02
		break;
	case 8 :
		sprintf( buffer , "%04d-%02d-%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // 2006.10.02
		break;
	default :
		sprintf( buffer , "%04d/%02d/%02d %02d:%02d:%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // 2006.10.02
		break;
	}
}
//

void KPLT_Local_Time_for_File( BOOL sysmode , char *buffer , int Sep ) {
	SYSTEMTIME		SysTime;
	GetLocalTime( &SysTime );
	if ( sysmode ) {
		if      ( Sep == 1 )
			sprintf( buffer , "%s/%04d%02d%02d/%02d%02d%02d" , GET_SYSTEM_NAME() , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
		else if ( Sep == 2 )
			sprintf( buffer , "%s/%04d/%02d/%02d/%02d%02d%02d" , GET_SYSTEM_NAME() , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
		else
			sprintf( buffer , "%s/%04d%02d%02d_%02d%02d%02d" , GET_SYSTEM_NAME() , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	}
	else {
		if      ( Sep == 1 )
			sprintf( buffer , "%04d%02d%02d/%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
		else if ( Sep == 2 )
			sprintf( buffer , "%04d/%02d/%02d/%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
		else
			sprintf( buffer , "%04d%02d%02d_%02d%02d%02d" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	}
}
//=================================================================================================================
//=================================================================================================================
//=================================================================================================================
void KPLT_START_FM_PICK_ROBOT_TIME( int fms , int mode ) {
	time( &Robot_Progress_Start_Timer_FM_PICK[fms][mode] );
}
//
void KPLT_END_FM_PICK_ROBOT_TIME( int fms , int mode ) {
	time_t finish;
	time( &finish );
	Robot_Progress_Run_Timer_FM_PICK[fms][mode] = (long) difftime( Robot_Progress_Start_Timer_FM_PICK[fms][mode] , finish );
}
//
long KPLT_GET_FM_PICK_ROBOT_TIME( int fms , int mode ) {
	return Robot_Progress_Run_Timer_FM_PICK[fms][mode];
}
//===========
void KPLT_START_FM_PLACE_ROBOT_TIME( int fms , int mode ) {
	time( &Robot_Progress_Start_Timer_FM_PLACE[fms][mode] );
}
//
void KPLT_END_FM_PLACE_ROBOT_TIME( int fms , int mode ) {
	time_t finish;
	time( &finish );
	Robot_Progress_Run_Timer_FM_PLACE[fms][mode] = (long) difftime( Robot_Progress_Start_Timer_FM_PLACE[fms][mode] , finish );
}
//
long KPLT_GET_FM_PLACE_ROBOT_TIME( int fms , int mode ) {
	return Robot_Progress_Run_Timer_FM_PLACE[fms][mode];
}
//=================================================================================================================
void KPLT_START_TM_PICK_ROBOT_TIME( int tms , int mode ) {
	time( &Robot_Progress_Start_Timer_TM_PICK[tms][mode] );
}
//
void KPLT_END_TM_PICK_ROBOT_TIME( int tms , int mode ) {
	time_t finish;
	time( &finish );
	Robot_Progress_Run_Timer_TM_PICK[tms][mode] = (long) difftime( Robot_Progress_Start_Timer_TM_PICK[tms][mode] , finish );
}
//
long KPLT_GET_TM_PICK_ROBOT_TIME( int tms , int mode ) {
	return Robot_Progress_Run_Timer_TM_PICK[tms][mode];
}
//===========
void KPLT_START_TM_PLACE_ROBOT_TIME( int tms , int mode ) {
	time( &Robot_Progress_Start_Timer_TM_PLACE[tms][mode] );
}
//
void KPLT_END_TM_PLACE_ROBOT_TIME( int tms , int mode ) {
	time_t finish;
	time( &finish );
	Robot_Progress_Run_Timer_TM_PLACE[tms][mode] = (long) difftime( Robot_Progress_Start_Timer_TM_PLACE[tms][mode] , finish );
}
//
long KPLT_GET_TM_PLACE_ROBOT_TIME( int tms , int mode ) {
	return Robot_Progress_Run_Timer_TM_PLACE[tms][mode];
}

//=================================================================================================================
//=================================================================================================================

void KPLT_SET_LOT_START_TIME() { // 2008.09.27
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		Process_Equip_Timer[ i ] = GetTickCount();
	}
}

//===========

unsigned long KPLT_GET_LOT_PROGRESS_TIME( int ch ) { // 2008.09.27
	DWORD l = GetTickCount() , t;
	t = l - Process_Equip_Timer[ ch ];
	Process_Equip_Timer[ ch ] = l;
	return t;
}

//=================================================================================================================
//=================================================================================================================
//=================================================================================================================
void KPLT_PROCESS_TIME_INIT( int chamber , int t1 , int t2 ) {
	int i , j , k;
	if ( chamber < 0 ) {
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
//			if ( chamber == -1 ) { // 2008.04.18 // 2008.07.13
				for ( k = 0 ; k < 3 ; k++ ) {
					Process_Progress_Start_Timer[k][ i ] = 0;
					//Process_Progress_End_Timer[k][ i ] = 0; // 2010.11.12
					time( &Process_Progress_End_Timer[k][i] ); // 2010.11.12
					Process_Progress_Run_Timer[k][ i ] = ( ( k == 0 ) ? t1 : t2 ); // sec // 2006.02.09
				}
//			} // 2008.07.13
			//
//			BM_Progress_Timer[ i ] = 0; // 2016.12.09
			BM_Target_Timer[ i ] = 0; // 2016.12.09
			//
		}
		MFAL_Progress_Timer = 0;
		for ( i = 0 ; i < ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; i++ ) { // 2007.06.22 
			MFIC_Progress_Timer[ i ] = 0;
			MFIC_Progress_Target[ i ] = 0;
		}
		//
		for ( i = 0 ; i < ( MAX_FINGER_FM + 1 ) ; i++ ) {
			Robot_Progress_Start_Timer_FM[ i ] = 0;
			Robot_Progress_End_Timer_FM[ i ] = 0;
			Robot_Progress_Run_Timer_FM[ i ] = 0;
		}
		//
		for ( i = 0 ; i < ( MAX_FINGER_TM + 1 ) ; i++ ) {
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				Robot_Progress_Start_Timer_TM[ j ][i] = 0;
				Robot_Progress_End_Timer_TM[ j ][i] = 0;
				Robot_Progress_Run_Timer_TM[ j ][i] = 0;
			}
		}
		//==============================================================
		// 2006.07.20
		//==============================================================
		for ( i = 0 ; i < 2 ; i++ ) {
			//-------------------------
			for ( j = 0 ; j < MAX_FM_CHAMBER_DEPTH ; j++ ) {
				Robot_Progress_Run_Timer_FM_PICK[j][i] = 0;
				Robot_Progress_Start_Timer_FM_PICK[j][i] = 0;
				//
				Robot_Progress_Run_Timer_FM_PLACE[j][i] = 0;
				Robot_Progress_Start_Timer_FM_PLACE[j][i] = 0;
			}
			//-------------------------
			for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
				Robot_Progress_Run_Timer_TM_PICK[j][i] = 0;
				Robot_Progress_Start_Timer_TM_PICK[j][i] = 0;
				//
				Robot_Progress_Run_Timer_TM_PLACE[j][i] = 0;
				Robot_Progress_Start_Timer_TM_PLACE[j][i] = 0;
			}
			//-------------------------
		}
	}
	else {
		for ( k = 0 ; k < 3 ; k++ ) {
			if ( Process_Progress_Run_Timer[k][ chamber ] <= 0 ) {
				if ( k == 0 ) {
					if ( t1 < 0 ) {
						Process_Progress_Start_Timer[k][ chamber ] = 0;
						//Process_Progress_End_Timer[k][ chamber ] = 0; // 2010.11.12
						time( &Process_Progress_End_Timer[k][chamber] ); // 2010.11.12
						Process_Progress_Run_Timer[k][ chamber ] = 0;
					}
					else {
						Process_Progress_Run_Timer[k][ chamber ] = t1; // sec // 2006.02.09
					}
				}
				else {
					if ( t2 < 0 ) {
						Process_Progress_Start_Timer[k][ chamber ] = 0;
						//Process_Progress_End_Timer[k][ chamber ] = 0; // 2010.11.12
						time( &Process_Progress_End_Timer[k][chamber] ); // 2010.11.12
						Process_Progress_Run_Timer[k][ chamber ] = 0;
					}
					else {
						Process_Progress_Run_Timer[k][ chamber ] = t2; // sec // 2006.02.09
					}
				}
			}
		}
	}
}
//=================================================================================================================
void INIT_TIMER_KPLT_DATA( int apmode , int side ) {
	if ( apmode == 0 ) {
		KPLT_PROCESS_TIME_INIT( -1 , 0 , 0 );
	}
	else if ( apmode == 3 ) { // 2008.04.17
		KPLT_SET_LOT_START_TIME(); // 2008.09.27
		if ( _i_SCH_PRM_GET_TIMER_RESET_WHEN_NEW_START() == 1 ) {
			KPLT_PROCESS_TIME_INIT( -1 , 0 , 0 );
		}
	}
}
