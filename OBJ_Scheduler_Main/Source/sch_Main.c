#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "Equip_Handling.h"
#include "User_Parameter.h"
#include "sch_CassetteResult.h"
#include "sch_sub.h"
#include "sch_prm.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_cassmap.h"
#include "sch_prm_Log.h"
#include "MultiJob.h"
#include "iolog.h"
#include "FA_Handling.h"
#include "MultiReg.h"
#include "version.h"

#include "Errorcode.h" // 2018.09.04
#include "sch_util.h" // 2018.09.04

#include "INF_sch_CommonUser.h"
#include "INF_sch_macro.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Main Scheduling Run
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
CRITICAL_SECTION CR_EndSigCheck;

//------------------------------------------------------------------------------------------

int  _in_Main_locking_Count[MAX_CASSETTE_SIDE]; // 2017.07.26

CRITICAL_SECTION _CR_Main_Lock; // 2018.11.21

//------------------------------------------------------------------------------------------
extern int  _in_IMSI_PralSkip[MAX_CASSETTE_SIDE]; // 2009.08.05
//------------------------------------------------------------------------------------------
BOOL Scheduler_Main_Wait_Finish_Complete( int CHECKING_SIDE , int mode );
//------------------------------------------------------------------------------------------




// 2018.01.16



SYSTEMTIME _OPERATE_TIME_START[MAX_CASSETTE_SIDE];
SYSTEMTIME _OPERATE_TIME_RUN[MAX_CASSETTE_SIDE];



int _OPERATE_CHECK_YEAR;
int _OPERATE_CHECK_MONTH;
int _OPERATE_CHECK_DAY;

int _OPERATE_CHECKED[MAX_CASSETTE_SIDE];





void OPERATE_INIT() {
	int i;
	SYSTEMTIME		SysTime;

	GetLocalTime( &SysTime );

	_OPERATE_CHECK_YEAR = SysTime.wYear;
	_OPERATE_CHECK_MONTH = SysTime.wMonth;
	_OPERATE_CHECK_DAY = SysTime.wDay;

	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		_OPERATE_CHECKED[i] = FALSE;
	}


}


void OPERATE_CONTROL( int side , int mode ) {
	char str_title[16];
	char str_time[24];
	char str_folder[256];

	switch( mode ) {
	case 1 : // START
		//
		GetLocalTime( &_OPERATE_TIME_START[side] );
		//
		strcpy( str_title , "$LOT_START" );
		//
		sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_START[side].wYear , _OPERATE_TIME_START[side].wMonth , _OPERATE_TIME_START[side].wDay , _OPERATE_TIME_START[side].wHour , _OPERATE_TIME_START[side].wMinute , _OPERATE_TIME_START[side].wSecond );
		//
		break;

	case 2 : // RUN
		//
		GetLocalTime( &_OPERATE_TIME_RUN[side] );
		//
		strcpy( str_title , "$LOT_RUN" );
		//
		sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_START[side].wYear , _OPERATE_TIME_START[side].wMonth , _OPERATE_TIME_START[side].wDay , _OPERATE_TIME_START[side].wHour , _OPERATE_TIME_START[side].wMinute , _OPERATE_TIME_START[side].wSecond );
		//
		break;

	case 3 : // END
		//
		strcpy( str_title , "$LOT_END" );
		//
		sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_RUN[side].wYear , _OPERATE_TIME_RUN[side].wMonth , _OPERATE_TIME_RUN[side].wDay , _OPERATE_TIME_RUN[side].wHour , _OPERATE_TIME_RUN[side].wMinute , _OPERATE_TIME_RUN[side].wSecond );
		//
		break;

	case 4 : // ABORT
		//
		strcpy( str_title , "$LOT_ABORT" );
		//
		sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_RUN[side].wYear , _OPERATE_TIME_RUN[side].wMonth , _OPERATE_TIME_RUN[side].wDay , _OPERATE_TIME_RUN[side].wHour , _OPERATE_TIME_RUN[side].wMinute , _OPERATE_TIME_RUN[side].wSecond );
		//
		break;

	case 5 : // ERROR
		//
		strcpy( str_title , "$LOT_ERROR" );
		//
		sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_RUN[side].wYear , _OPERATE_TIME_RUN[side].wMonth , _OPERATE_TIME_RUN[side].wDay , _OPERATE_TIME_RUN[side].wHour , _OPERATE_TIME_RUN[side].wMinute , _OPERATE_TIME_RUN[side].wSecond );
		//
		break;
		
	default :
		return;
	}
	//
	sprintf( str_folder , "%s/LOTLOG/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) );
	//
	_IO_OPERATE_PRINTF( 0 , str_title , "%d|%s|%s\n" , side + 1 , str_time , str_folder );
	//
	_OPERATE_CHECKED[side] = TRUE;
}



void OPERATE_OLD_PRINTF( BOOL run , int old_Year , int old_Month , int old_Day , LPSTR list , ... ) {
	FILE *fpt;
	char operfolder[256];
	char FileName[64];
	va_list va;
	//
	_IO_GET_OPER_FOLDER( operfolder , 255 );
	//
	va_start( va , list );
	//
	sprintf( FileName , "%s\\OP%04d%02d%02d.log" , operfolder , old_Year , old_Month , old_Day );
	//
	fpt = fopen( FileName , "a" );
	//
	if ( fpt != NULL ) {
		if ( run ) {
			fprintf( fpt , "%d/%d/%d 23:59:59\t%s\t" , old_Year , old_Month , old_Day , "$LOT_RUNNED" );
		}
		else {
			fprintf( fpt , "%d/%d/%d 23:59:59\t%s\t" , old_Year , old_Month , old_Day , "$LOT_STARTED" );
		}

		vfprintf( fpt , list , (LPSTR) va );
		fclose( fpt );
	}
	//
    va_end( va );
}




void OPERATE_CHECK() {
	int i;
	SYSTEMTIME		SysTime;
	char str_time[32];
	char str_folder[256];
	//
	GetLocalTime( &SysTime );
	//
	if ( ( _OPERATE_CHECK_YEAR != SysTime.wYear ) || ( _OPERATE_CHECK_MONTH != SysTime.wMonth ) ||  ( _OPERATE_CHECK_DAY != SysTime.wDay ) ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( !_OPERATE_CHECKED[i] ) {
				//
				if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
					//
					sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_START[i].wYear , _OPERATE_TIME_START[i].wMonth , _OPERATE_TIME_START[i].wDay , _OPERATE_TIME_START[i].wHour , _OPERATE_TIME_START[i].wMinute , _OPERATE_TIME_START[i].wSecond );
					sprintf( str_folder , "%s/LOTLOG/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( i ) );
					//
					OPERATE_OLD_PRINTF( TRUE , _OPERATE_CHECK_YEAR , _OPERATE_CHECK_MONTH , _OPERATE_CHECK_DAY , "%d|%s|%s" , i + 1 , str_time , str_folder );
				}
				else if ( _i_SCH_SYSTEM_USING_STARTING( i ) ) {
					//
					sprintf( str_time , "%d/%d/%d %d:%d:%d" , _OPERATE_TIME_START[i].wYear , _OPERATE_TIME_START[i].wMonth , _OPERATE_TIME_START[i].wDay , _OPERATE_TIME_START[i].wHour , _OPERATE_TIME_START[i].wMinute , _OPERATE_TIME_START[i].wSecond );
					sprintf( str_folder , "%s/LOTLOG/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( i ) );
					//
					OPERATE_OLD_PRINTF( FALSE , _OPERATE_CHECK_YEAR , _OPERATE_CHECK_MONTH , _OPERATE_CHECK_DAY , "%d|%s|%s" , i + 1 , str_time , str_folder );
				}
				//
			}
			//
			_OPERATE_CHECKED[i] = FALSE;
			//
		}
		//
		_OPERATE_CHECK_YEAR = SysTime.wYear;
		_OPERATE_CHECK_MONTH = SysTime.wMonth;
		_OPERATE_CHECK_DAY = SysTime.wDay;
		//
	}
	//
}

//---------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Main Scheduling Wait
//------------------------------------------------------------------------------------------
BOOL Scheduler_Main_All_is_Disable( int CHECKING_SIDE ) { // 2007.05.17
	int i;
	int c;
	//
	c = 0;
	//
	for ( i = PM1 ; i < AL ; i++ ) {
		if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) < MUF_90_USE_to_XDEC_FROM ) ) {
			if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , _i_SCH_SYSTEM_PMMODE_GET( CHECKING_SIDE ) ) ) return FALSE;
			c++; // 2011.07.07
		}
		else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) >= MUF_97_USE_to_SEQDIS_RAND ) { // 2011.07.07
			c++;
		}
	}
	//
	if ( c == 0 ) return FALSE; // 2011.07.07
	//
	return TRUE;
}

//BOOL _SCH_inside_Ordering_Invalid_Check( int side ); // 2015.12.15 // 2016.01.08
BOOL _SCH_inside_Ordering_Invalid_Check( int side , BOOL wait_for_start ); // 2015.12.15 // 2016.01.08

BOOL Scheduler_Main_Supply_Impossible( int CHECKING_SIDE ) { // 2015.12.15
//	return _SCH_inside_Ordering_Invalid_Check( CHECKING_SIDE ); // 2016.01.18
	return _SCH_inside_Ordering_Invalid_Check( CHECKING_SIDE , TRUE ); // 2016.01.18
}


BOOL _SIDE_HAS_USERMODE_DATA( int rside , int side ) { // 2011.12.30
	int i , Res;
	//
	//=================================================================================
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	//
	if ( _i_SCH_SYSTEM_USING_GET( side ) <= 0 ) return FALSE;
	//
	if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) {
		//
		if ( _i_SCH_SYSTEM_RUNORDER_GET( rside ) < _i_SCH_SYSTEM_RUNORDER_GET( side ) ) {
			//
			return FALSE;
			//
		}
		//
	}
	//
	//=================================================================================
	//
	// 2016.11.25
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( rside , i ) < 0 ) continue;
		if ( _i_SCH_CLUSTER_Get_PathStatus( rside , i ) != SCHEDULER_READY ) continue;
		//
		Res = _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( rside , i );
		if ( Res == 0 ) break;
		if ( ( Res / 10 ) == 1 ) break;
		//
		return FALSE;
	}
	//
	//=================================================================================
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) != SCHEDULER_READY ) continue;
		//
		Res = _i_SCH_CLUSTER_Get_Ex_UserControl_Mode( side , i );
		if ( Res == 0 ) continue;
		if ( ( Res / 10 ) == 1 ) continue;
		//
		return TRUE;
	}
	//
	//=================================================================================
	//
	return FALSE;
}
//=======================================================================================

//------------------------------------------------------------------------------------------
int Scheduler_Main_Waiting( int CHECKING_SIDE , int PreChecking , int *pralskip , int lc , int *Result ) {
	int i , j , k , p , p2 , waitmode , waitmode2 , ok;
	char Buffer[MAX_CASSETTE_SLOT_SIZE+MAX_CASSETTE_SLOT_SIZE+2];
	int checkside[MAX_CASSETTE_SIDE][MAX_CASSETTE_SIDE]; // 2005.09.14
	int WAITING_MODE_Flag; // 2015.07.28

	//-------------------------------------------
	ok = FALSE; // 2011.11.08
	//-------------------------------------------
	*Result = 0;
	//-------------------------------------------
	*pralskip = FALSE; // 2006.03.02
	//-------------------------------------------
//	if ( !PreChecking ) { // 2004.06.16 // 2011.11.08
	if ( PreChecking != 1 ) { // 2004.06.16 // 2011.11.08
		if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) {
			//-------------------------------------------
			*Result = 101;
			//-------------------------------------------
			return SYS_ERROR;
		}
	}
	//
	if ( PreChecking == 0 ) { // 2011.11.08
		//-------------------------------------------
		// 2005.03.02
		//-------------------------------------------
		p2 = MAX_CASSETTE_SLOT_SIZE;
		for ( i = MAX_CASSETTE_SLOT_SIZE - 1 ; i >= 0 ; i-- ) {
			j = Scheduler_Mapping_Buffer_Data_Get1( CHECKING_SIDE , i );
			k = Scheduler_Mapping_Buffer_Data_Get2( CHECKING_SIDE , i );
			if ( ( j != 10 ) || ( k != 10 ) ) {
				p2 = i + 1;
				break;
			}
		}
		p = 0;
		for ( i = 0 ; i < p2 ; i++ ) {
			j = Scheduler_Mapping_Buffer_Data_Get1( CHECKING_SIDE , i );
			k = Scheduler_Mapping_Buffer_Data_Get2( CHECKING_SIDE , i );
			//
			if ( j != k ) p = 1;
			//
			if ( j >= 10 ) {
				Buffer[i] = 'A' + j - 10;
			}
			else {
				Buffer[i] = '0' + j;
			}
			//
			if ( k >= 10 ) {
				Buffer[i+p2+1] = 'A' + k - 10;
			}
			else {
				Buffer[i+p2+1] = '0' + k;
			}

		}
		if ( p == 1 ) {
			Buffer[p2] = '-';
			Buffer[p2+p2+1] = 0;
		}
		else {
			Buffer[p2] = 0;
		}
		//-------------------------------------------
		//-------------------------------------------
		//-------------------------------------------
//		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(%d)....%cSCHWAIT %d\n" , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) ); // 2005.03.02


#ifdef PM_CHAMBER_USER
		if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) { // 2009.03.25
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(F)....%cSCHWAIT F%d:%d:%s:U:%d\n" , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(%d)....%cSCHWAIT %d:%d:%s:U:%d\n" , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc );
		}
#else
	#ifdef PM_CHAMBER_60
		if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) { // 2005.03.10
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(F)....%cSCHWAIT F%d:%d:%s:60:%d\n" , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc ); // 2005.03.02
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(%d)....%cSCHWAIT %d:%d:%s:60:%d\n" , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc ); // 2005.03.02
		}
	#else
		#ifdef PM_CHAMBER_30
			if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) { // 2005.03.10
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(F)....%cSCHWAIT F%d:%d:%s:30:%d\n" , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc ); // 2005.03.02
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(%d)....%cSCHWAIT %d:%d:%s:30:%d\n" , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc ); // 2005.03.02
			}
		#else
			if ( SYSTEM_MODE_FIRST_GET( CHECKING_SIDE ) ) { // 2005.03.10
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(F)....%cSCHWAIT F%d:%d:%s:%d\n" , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc ); // 2005.03.02
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Wait for Scheduling(%d)....%cSCHWAIT %d:%d:%s:%d\n" , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , 9 , _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) , SYSTEM_MODE_BUFFER_GET( CHECKING_SIDE ) , Buffer , lc ); // 2005.03.02
			}
		#endif
	#endif
#endif
		
		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 8 );
		//-------------------------------------------
		_i_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , FALSE ); // 2003.05.23
		//-------------------------------------------
	}
	else if ( PreChecking == 2 ) { // 2011.11.08
		//-------------------------------------------
		_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 8 );
		//-------------------------------------------
		_i_SCH_SYSTEM_LASTING_SET( CHECKING_SIDE , FALSE ); // 2003.05.23
		//-------------------------------------------
	}

	//-----------------------------------------------------------------------------------
	// 2008.05.28
	//-----------------------------------------------------------------------------------
//	waitmode = _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ); // 2008.06.24
//	p = waitmode;
//	if ( _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , PreChecking , lc ) ) {
//		waitmode = p;
//	}
	//-----------------------------------------------------------------------------------
	while ( TRUE ) {
		//-------------------------------------------
//		*pralskip = FALSE; // 2008.07.31 // 2008.09.02
		//-------------------------------------------
		*Result = -11; // 2013.02.08
		//-------------------------------------------
		//if ( !PreChecking ) { // 2004.06.16
		if ( PreChecking != 1 ) { // 2004.06.16 // 2011.11.08
//			EnterCriticalSection( &CR_WAIT ); // 2009.07.09
			if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) { // Canceled
				//-------------------------------------------
				*Result = 102;
				//-------------------------------------------
//				LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
				return SYS_ERROR;
			}
			//-------------------------------------------
			if ( SCHMULTI_RUNJOB_STOPPED( CHECKING_SIDE ) ) { // 2016.09.19
				//-------------------------------------------
				_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 102 ); // 2018.11.01
				//-------------------------------------------
				*Result = 103;
				//-------------------------------------------
				return SYS_ERROR;
			}
			//-------------------------------------------
//			if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 9 ) { // 2011.11.08
			if ( ok ) { // 2011.11.08
				//-------------------------------------------
				*Result = 2;
				//-------------------------------------------
				_i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2011.11.08
				//
//				LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
				return SYS_SUCCESS;
			}
		}
		//
		WAITING_MODE_Flag = 0; // 2015.07.28
		//
		//-----------------------------------------------------------------------------------
		// 2008.06.24
		//-----------------------------------------------------------------------------------
		//
		waitmode = _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE );
		//
		p = waitmode;
		//
		SCHMULTI_CONTROLLER_LOGSET( 9980 , CHECKING_SIDE ); // 2016.01.12
		SCHMULTI_CONTROLLER_LOGSET( 9981 , PreChecking ); // 2016.01.12
		SCHMULTI_CONTROLLER_LOGSET( 9982 , waitmode ); // 2016.01.12
		//
		SCHMULTI_CONTROLLER_LOGSET( 9900 , waitmode ); // 2015.07.28
		//
		if ( _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 0 ) ) {
			//
			waitmode = p;
			//
			if ( PreChecking == 1 ) {
				WAITING_MODE_Flag = WAITING_MODE_Flag + 2; // 2015.07.28
			}
			else {
				WAITING_MODE_Flag = WAITING_MODE_Flag + 4; // 2015.07.28
			}
			//
		}
		else {
			//
			if ( PreChecking == 1 ) {
				WAITING_MODE_Flag = WAITING_MODE_Flag + 1; // 2015.07.28
			}
			else {
				WAITING_MODE_Flag = WAITING_MODE_Flag + 3; // 2015.07.28
			}
			//
		}
		//-----------------------------------------------------------------------------------
//		if ( PreChecking || ( SYSTEM_ENDCHECK_GET( CHECKING_SIDE ) != 1 ) ) { // 2002.07.10 // 2004.06.16 // 2011.11.08
		if ( ( PreChecking == 1 ) || ( SYSTEM_ENDCHECK_GET( CHECKING_SIDE ) != 1 ) ) { // 2002.07.10 // 2004.06.16 // 2011.11.08
			//
			if ( waitmode == 0 ) { // 2008.07.31
				if ( _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 1 ) ) {
					waitmode = 1;
					//
					WAITING_MODE_Flag = WAITING_MODE_Flag + 20; // 2015.07.28
					//
				}
				else {
					//
					WAITING_MODE_Flag = WAITING_MODE_Flag + 10; // 2015.07.28
					//
				}
			}
			//
			if ( waitmode != 0 ) { // 2008.07.31
				if ( _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 2 ) ) {
					//
					WAITING_MODE_Flag = WAITING_MODE_Flag + 200; // 2015.07.28
					//
					if      ( waitmode == 1 ) {
						waitmode = 11;
					}
					else if ( waitmode == 2 ) {
						waitmode = 12;
					}
					else if ( waitmode == 3 ) {
						waitmode = 13;
					}
					//
					if ( p == 99 ) { // 2009.03.06
						//
//						if ( PreChecking ) { // 2011.11.08
						if ( PreChecking == 1 ) { // 2011.11.08
							//-------------------------------------------
							*Result = -2;
							//-------------------------------------------
							SCHMULTI_CONTROLLER_LOGSET( 9901 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
							//-------------------------------------------
							return SYS_ABORTED;
						}
						//
						if ( !_i_SCH_SYSTEM_PAUSE_ABORT2_CHECK( CHECKING_SIDE ) ) {
							//-------------------------------------------
							*Result = -3;
							//-------------------------------------------
//							LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
							//-------------------------------------------
							SCHMULTI_CONTROLLER_LOGSET( 9902 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
							//-------------------------------------------
							return SYS_ABORTED;
						}
						//
//						LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
						Sleep(1);
						continue;
					}
					//
				}
				else {
					//
					WAITING_MODE_Flag = WAITING_MODE_Flag + 100; // 2015.07.28
					//
				}
			}
			//
			//-------------------------------------------
			*pralskip = FALSE; // 2008.09.02
			//-------------------------------------------
			waitmode2 = waitmode;
//			if ( ( waitmode2 != 2 ) && ( waitmode2 != 12 ) ) { // 2011.12.30 // 2015.07.22
			if ( ( waitmode2 != 2 ) && ( waitmode2 != 12 ) && ( waitmode2 != 3 ) && ( waitmode2 != 13 ) ) { // 2011.12.30 // 2015.07.22
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( i != CHECKING_SIDE ) {
						if ( _SIDE_HAS_USERMODE_DATA( CHECKING_SIDE , i ) ) {
							waitmode2 = 12;
							break;
						}
					}
				}
			}
			//-------------------------------------------
			*Result = -12; // 2013.02.08
			//-------------------------------------------
			//
			SCHMULTI_CONTROLLER_LOGSET( 9983 , waitmode2 ); // 2016.01.12
			//
			//-------------------------------------------
			//
			switch( waitmode2 ) {
			case 1 :	// Continue
//			case 3 :	// Sequential2 // 2015.07.22
			case 11 :	// Continue
//			case 13 :	// Sequential2 // 2015.07.22
				//======
//				if ( !PreChecking ) EnterCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				if ( PreChecking != 1 ) EnterCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				//======
				//=======================================================================================================
				// 2007.05.17
				//=======================================================================================================
				if ( Scheduler_Main_All_is_Disable( CHECKING_SIDE ) ) { // 2007.05.17
//					if ( !PreChecking ) LeaveCriticalSection( &CR_WAIT ); // 2009.08.05 // 2011.11.08
					if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT ); // 2009.08.05 // 2011.11.08
					//-------------------------------------------
					*Result = -21; // 2013.02.08
					//-------------------------------------------
					SCHMULTI_CONTROLLER_LOGSET( 9903 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
					//-------------------------------------------
					break;
				}
				//=======================================================================================================
				// 2015.12.15
				//=======================================================================================================
				if ( Scheduler_Main_Supply_Impossible( CHECKING_SIDE ) ) {
					if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT );
					//-------------------------------------------
					*Result = -22;
					//-------------------------------------------
					SCHMULTI_CONTROLLER_LOGSET( 9904 , ( waitmode * 10000 ) + WAITING_MODE_Flag );
					//-------------------------------------------
					break;
				}
				//=======================================================================================================
				// 2005.09.14
				//=======================================================================================================
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					checkside[CHECKING_SIDE][i] = FALSE;
					if ( i != CHECKING_SIDE ) {
						if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
							/*
							// 2011.06.24
							if ( waitmode >= 10 ) { // 2008.07.31
								if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
									checkside[CHECKING_SIDE][i] = TRUE;
								}
							}
							else {
								if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 1 , PreChecking ) ) { // 2003.01.11
									if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
										checkside[CHECKING_SIDE][i] = TRUE;
									}
								}
								else {
									*pralskip = TRUE; // 2006.03.02
								}
							}
							*/
							// 2011.06.24
							if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
								if ( waitmode >= 10 ) { // 2008.07.31
									checkside[CHECKING_SIDE][i] = TRUE;
								}
								else {
//									if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 1 , PreChecking ) ) { // 2003.01.11 // 2011.11.08
									if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 1 , ( PreChecking != 0 ) ) ) { // 2003.01.11 // 2011.11.08
										checkside[CHECKING_SIDE][i] = TRUE;
									}
									else {
										*pralskip = TRUE; // 2006.03.02
									}
								}
							}
							//
						}
					}
				}
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( checkside[CHECKING_SIDE][i] ) {
						if ( ( _i_SCH_SYSTEM_FA_GET( i ) == 1 ) && _i_SCH_MULTIREG_HAS_REGIST_DATA( i ) ) break;
						if ( ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) && ( !_i_SCH_SYSTEM_LASTING_GET( i ) ) ) break;
					}
				}
				//======
//				if ( !PreChecking ) LeaveCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				//======
				if ( i == MAX_CASSETTE_SIDE ) {
					//
					p = waitmode; // 2009.03.06
					if ( !_SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 3 ) ) {
						//
						WAITING_MODE_Flag = WAITING_MODE_Flag + 1000; // 2015.07.28
						//
						//
//						if ( PreChecking ) { // 2011.11.08
						if ( PreChecking == 1 ) { // 2011.11.08
							//-------------------------------------------
							*Result = 3;
							//-------------------------------------------
							SCHMULTI_CONTROLLER_LOGSET( 9905 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
							//-------------------------------------------
							return SYS_SUCCESS;
						}
						//
						// _i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2011.11.08
						ok = TRUE; // 2011.11.08
						//
					}
					else {
						//
						WAITING_MODE_Flag = WAITING_MODE_Flag + 2000; // 2015.07.28
						//
						//-------------------------------------------
						*Result = -23; // 2013.02.08
						//-------------------------------------------
					}
				}
				else {
					//-------------------------------------------
					*Result = -24; // 2013.02.08
					//-------------------------------------------
				}
				//-------------------------------------------
				if ( SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( CHECKING_SIDE , TRUE , &p ) ) { // 2011.06.13
					//
					if ( p != -1 ) _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 4 ); // 2013.03.14
					//
//					if ( PreChecking ) { // 2011.11.08
					if ( PreChecking == 1 ) { // 2011.11.08
						//-------------------------------------------
						*Result = 13;
						//-------------------------------------------
						SCHMULTI_CONTROLLER_LOGSET( 9906 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
						//-------------------------------------------
						return SYS_SUCCESS;
					}
					//
					// _i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2011.11.08
					ok = TRUE; // 2011.11.08
					//
				}
				else {
					//
					if ( p != -1 ) _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 4 ); // 2013.03.14
					//
					//-------------------------------------------
					if ( *Result == -23 )
						*Result = -25; // 2013.02.08
					else
						*Result = -26; // 2013.02.08
					//-------------------------------------------
				}
				//-------------------------------------------
				//=======================================================================================================
				break;

			case 2 :	// Sequential
			case 12 :	// Sequential
				//
			case 3 :	// Sequential2 // 2015.07.22
			case 13 :	// Sequential2 // 2015.07.22
				//======
//				if ( !PreChecking ) EnterCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				if ( PreChecking != 1 ) EnterCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				//======
				//=======================================================================================================
				// 2007.05.17
				//=======================================================================================================
				if ( Scheduler_Main_All_is_Disable( CHECKING_SIDE ) ) { // 2011.06.08
//					if ( !PreChecking ) LeaveCriticalSection( &CR_WAIT ); // 2011.11.08
					if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT ); // 2011.11.08
					//-------------------------------------------
					*Result = -31; // 2013.02.08
					//-------------------------------------------
					SCHMULTI_CONTROLLER_LOGSET( 9907 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
					//-------------------------------------------
					break;
				}
				//=======================================================================================================
				// 2015.12.15
				//=======================================================================================================
				if ( Scheduler_Main_Supply_Impossible( CHECKING_SIDE ) ) {
					if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT );
					//-------------------------------------------
					*Result = -32;
					//-------------------------------------------
					SCHMULTI_CONTROLLER_LOGSET( 9908 , ( waitmode * 10000 ) + WAITING_MODE_Flag );
					//-------------------------------------------
					break;
				}
				//=======================================================================================================
				// 2005.09.14
				//=======================================================================================================
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					checkside[CHECKING_SIDE][i] = FALSE;
					if ( i != CHECKING_SIDE ) {
						if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
							/*
							// 2011.06.24
							if ( waitmode >= 10 ) { // 2008.07.31
								if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
									checkside[CHECKING_SIDE][i] = TRUE;
								}
							}
							else {
								//
								// 2011.06.24
								//if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 0 , PreChecking ) ) { // 2003.01.11
								//	if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
								//		checkside[CHECKING_SIDE][i] = 1;
								//	}
								//}
								//
								// 2011.06.24
								if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 3 , PreChecking ) ) {
									if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
										checkside[CHECKING_SIDE][i] = TRUE;
									}
								}
								else {
									*pralskip = TRUE;
								}
							}
							*/
							// 2011.06.24
							if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
								if ( waitmode2 >= 10 ) { // 2008.07.31
									checkside[CHECKING_SIDE][i] = TRUE;
								}
								else {
									if ( waitmode2 == 3 ) { // 2015.07.22
										checkside[CHECKING_SIDE][i] = TRUE;
									}
									else {
	//									if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 3 , PreChecking ) ) { // 2011.11.08
										if ( !_i_SCH_SUB_START_PARALLEL_SKIP_CHECK( CHECKING_SIDE , i , 3 , ( PreChecking != 0 ) ) ) { // 2011.11.08
											checkside[CHECKING_SIDE][i] = TRUE;
										}
										else {
											*pralskip = TRUE;
										}
									}
								}
							}
							//
						}
					}
				}
				//
				for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
					if ( checkside[CHECKING_SIDE][i] ) {
						if ( ( _i_SCH_SYSTEM_FA_GET( i ) == 1 ) && _i_SCH_MULTIREG_HAS_REGIST_DATA( i ) ) break;
						if ( ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) && ( _i_SCH_SYSTEM_USING_GET( i ) < 110 ) ) break;
						//
					}
				}
				//======
//				if ( !PreChecking ) LeaveCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT ); // 2009.07.09 // 2011.11.08
				//======
				if ( i == MAX_CASSETTE_SIDE ) {
					//
					p = waitmode2; // 2009.03.06
					if ( !_SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 3 ) ) {
						//
						WAITING_MODE_Flag = WAITING_MODE_Flag + 1000; // 2015.07.28
						//
						//
//						if ( PreChecking ) { // 2011.11.08
						if ( PreChecking == 1 ) { // 2011.11.08
							//-------------------------------------------
							*Result = 4;
							//-------------------------------------------
							SCHMULTI_CONTROLLER_LOGSET( 9909 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
							//-------------------------------------------
							return SYS_SUCCESS;
						}
						//==============================================================================================
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							if ( checkside[CHECKING_SIDE][i] ) {
//								Scheduler_Main_Wait_Finish_Complete( i , 3 ); // 2010.07.29
								Scheduler_Main_Wait_Finish_Complete( i , 4 ); // 2010.07.29
							}
						}
						//==============================================================================================
						// _i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2011.11.08
						ok = TRUE; // 2011.11.08
						//
					}
					else {
						//
						WAITING_MODE_Flag = WAITING_MODE_Flag + 2000; // 2015.07.28
						//
						//-------------------------------------------
						*Result = -33; // 2013.02.08
						//-------------------------------------------
					}
				}
				else {
					//-------------------------------------------
					*Result = -34; // 2013.02.08
					//-------------------------------------------
				}
				//-------------------------------------------
				//
//				if ( SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( CHECKING_SIDE , TRUE , &p ) ) { // 2011.06.13 // 2015.07.22
				if ( ( waitmode2 != 13 ) && SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( CHECKING_SIDE , TRUE , &p ) ) { // 2011.06.13 // 2015.07.22
					//
					if ( p != -1 ) _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 4 ); // 2013.03.14
					//
//					if ( PreChecking ) { // 2011.11.08
					if ( PreChecking == 1 ) { // 2011.11.08
						//-------------------------------------------
						*Result = 14;
						//-------------------------------------------
						SCHMULTI_CONTROLLER_LOGSET( 9910 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
						//-------------------------------------------
						return SYS_SUCCESS;
					}
					//==============================================================================================
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
						if ( checkside[CHECKING_SIDE][i] ) {
							Scheduler_Main_Wait_Finish_Complete( i , 4 );
						}
					}
					//==============================================================================================
					//
					// _i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2011.11.08
					ok = TRUE; // 2011.11.08
					//
				}
				else {
					//
					if ( p != -1 ) _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 4 ); // 2013.03.14
					//
					//-------------------------------------------
					if ( *Result == -33 )
						*Result = -35; // 2013.02.08
					else
						*Result = -36; // 2013.02.08
					//-------------------------------------------
				}
				//-------------------------------------------
				//=======================================================================================================
				break;

			default :	// Concurrent
				//

				//=======================================================================================================
				// 2015.12.15
				//=======================================================================================================
				//======
				if ( PreChecking != 1 ) EnterCriticalSection( &CR_WAIT );
				//======
				if ( Scheduler_Main_All_is_Disable( CHECKING_SIDE ) ) {
					if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT );
					//-------------------------------------------
					*Result = -41;
					//-------------------------------------------
					SCHMULTI_CONTROLLER_LOGSET( 9911 , ( waitmode * 10000 ) + WAITING_MODE_Flag );
					//-------------------------------------------
					break;
				}
				if ( Scheduler_Main_Supply_Impossible( CHECKING_SIDE ) ) {
					if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT );
					//-------------------------------------------
					*Result = -42;
					//-------------------------------------------
					SCHMULTI_CONTROLLER_LOGSET( 9912 , ( waitmode * 10000 ) + WAITING_MODE_Flag );
					//-------------------------------------------
					break;
				}
				//======
				if ( PreChecking != 1 ) LeaveCriticalSection( &CR_WAIT );
				//======
				//
				//
				p = waitmode; // 2009.03.06
				if ( !_SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 3 ) ) {
					//
					WAITING_MODE_Flag = WAITING_MODE_Flag + 1000; // 2015.07.28
					//
					//
//					if ( PreChecking ) { // 2011.11.08
					if ( PreChecking == 1 ) { // 2011.11.08
						//-------------------------------------------
						*Result = 5;
						//-------------------------------------------
						SCHMULTI_CONTROLLER_LOGSET( 9913 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
						//-------------------------------------------
						return SYS_SUCCESS; // 2004.06.16
					}
					//
					// _i_SCH_SYSTEM_USING_SET( CHECKING_SIDE , 9 ); // 2011.11.08
					ok = TRUE; // 2011.11.08
					//
				}
				else {
					//
					WAITING_MODE_Flag = WAITING_MODE_Flag + 2000; // 2015.07.28
					//
					*Result = -43; // 2013.02.08
				}
				//-------------------------------------------
				// 2013.05.30
				//
				if ( SCHEDULER_CONTROL_Remapping_Multi_Chamber_Hot_Lot( CHECKING_SIDE , TRUE , &p ) ) {
					//
					if ( p != -1 ) _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 4 );
					//
					if ( PreChecking == 1 ) {
						//-------------------------------------------
						*Result = 15;
						//-------------------------------------------
						SCHMULTI_CONTROLLER_LOGSET( 9914 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
						//-------------------------------------------
						return SYS_SUCCESS;
					}
					//
					ok = TRUE;
					//
				}
				else {
					//
					if ( p != -1 ) _SCH_COMMON_CHECK_WAITING_MODE( CHECKING_SIDE , &p , ( lc * 10 ) + PreChecking , 4 );
					//
					//-------------------------------------------
					if ( *Result == -43 )
						*Result = -44;
					//-------------------------------------------
				}
				//-------------------------------------------
				//=======================================================================================================
				break;
			}
			//
		}
		else {
			//-------------------------------------------
			//
			SCHMULTI_CONTROLLER_LOGSET( 9984 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2016.01.12
			//
			//-------------------------------------------
		}
		//
//		if ( PreChecking ) { // 2011.11.08
		if ( PreChecking == 1 ) { // 2011.11.08
			//-------------------------------------------
//			*Result = -5; // 2013.02.08
			//-------------------------------------------
			SCHMULTI_CONTROLLER_LOGSET( 9915 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
			//-------------------------------------------
			return SYS_ABORTED; // 2004.06.16
		}
		else {
			if ( !_i_SCH_SYSTEM_PAUSE_ABORT2_CHECK( CHECKING_SIDE ) ) {
				//-------------------------------------------
				*Result = -6;
				//-------------------------------------------
				SCHMULTI_CONTROLLER_LOGSET( 9916 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
				//-------------------------------------------
//				LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
				return SYS_ABORTED;
			}
//			LeaveCriticalSection( &CR_WAIT ); // 2009.07.09
//			Sleep(5); // 2008.03.26
			Sleep(1); // 2008.03.26
		}
		//
		//-------------------------------------------
		SCHMULTI_CONTROLLER_LOGSET( 9921 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2016.01.12
		//-------------------------------------------
		//-------------------------------------------
		SCHMULTI_CONTROLLER_LOGSET( 9922 , *Result ); // 2016.01.12
		//-------------------------------------------
		//
	}
	//-------------------------------------------
	*Result = 1;
	//-------------------------------------------
	SCHMULTI_CONTROLLER_LOGSET( 9917 , ( waitmode * 10000 ) + WAITING_MODE_Flag ); // 2015.07.28
	//-------------------------------------------
	return SYS_SUCCESS;
}

//===================================================================================================
int _i_SCH_SUB_Main_End_Check_Finished( int side ) { // 2005.03.15
	int i;
	EnterCriticalSection( &CR_EndSigCheck ); // 2002.07.10
	if ( SYSTEM_ENDCHECK_GET( side ) == 1 ) { // 2002.07.11
		LeaveCriticalSection( &CR_EndSigCheck );
		return 1;
	}
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2002.07.10
		if ( i != side ) {
			if ( ( SYSTEM_ENDCHECK_GET( i ) == 1 ) && ( _i_SCH_SYSTEM_USING_GET( i ) >= 11 ) ) {
				break;
			}
		}
	}
	LeaveCriticalSection( &CR_EndSigCheck ); // 2002.07.10
	if ( i != MAX_CASSETTE_SIDE ) { // 2002.07.10
		return 2;
	}
	return 0;
}
//===================================================================================================
void Scheduler_Main_Other_End_Check_Monitor() { // 2002.07.10
	int i , s;
	//
	InitializeCriticalSection( &CR_EndSigCheck );
	//
	InitializeCriticalSection( &_CR_Main_Lock ); // 2018.11.21
	//
	//
	OPERATE_INIT(); // 2018.01.16
	//
	//
	while( TRUE ) {
		s = _i_SCH_SYSTEM_RUNCHECK_GET(); // Current Run Side
		if ( s > 0 ) {
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( ( s - 1 ) != i ) { // Find Not Current Run Side
					//
					if ( ( _i_SCH_SYSTEM_USING_GET( i ) >= 11 ) && ( SYSTEM_THDCHECK_GET( i ) ) ) { // 2005.10.06
						//
						if ( ( _i_SCH_SYSTEM_RUNCHECK_FM_GET() - 1 ) != i ) { // 2009.01.11
							if ( SYSTEM_ENDCHECK_GET( i ) == 0 ) {
								if ( _SCH_MACRO_FM_FINISHED_CHECK( i , TRUE ) ) {
									if ( ( _i_SCH_SYSTEM_RUNCHECK_FM_GET() - 1 ) != i ) { // 2009.01.11
										EnterCriticalSection( &CR_EndSigCheck );
										if ( SYSTEM_ENDCHECK_GET( i ) == 0 ) {
											SYSTEM_ENDCHECK_SET( i , 1 );
										}
										LeaveCriticalSection( &CR_EndSigCheck );
									}
								}
							}
						}
					}
				}
			}
		}
		//--------------------------------------------------------
		//
		OPERATE_CHECK(); // 2018.01.16
		//
		//--------------------------------------------------------
		Sleep(100);
		//--------------------------------------------------------
	}
	_endthread();
}
//

void Scheduler_Main_Lock_Init( int CHECKING_SIDE ) { // 2017.07.26
	//
	_in_Main_locking_Count[CHECKING_SIDE] = 0;
	//
}


BOOL Scheduler_Main_Lock_WAITING_FOR_MTLOUT() { // 2019.02.28
	int i , j;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING(i) ) continue;
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_CM_END ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathStatus( i , j ) == SCHEDULER_READY ) {
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) return TRUE;
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) == _MOTAG_13_NEED_IN ) return TRUE;
			}
			else {
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) == _MOTAG_12_NEED_IN_OUT ) return TRUE;
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( i , j ) % 100 ) == _MOTAG_14_NEED_OUT ) return TRUE;
			}
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) <= 0 ) return TRUE;
			if ( _i_SCH_CLUSTER_Get_PathOut( i , j ) <= 0 ) return TRUE;
			//
		}
	}
	//
	return FALSE;
	//
}


void Scheduler_Main_Locking( int CHECKING_SIDE ) { // 2008.04.18
	//
	char FileName_S[256];
	char FileName_A[256];
	char FileName_B[256];
	//
	BOOL notRun; // 2018.09.04
	//
	if ( CHECKING_SIDE == -1 ) { // finished
		//
//		EQUIP_RUNNING_CHECK( -1 ); // 2018.09.04
		//
		EQUIP_RUNNING_CHECK( -1 , NULL ); // 2018.09.04
		//
		IO_DEBUG_DISABLE( FALSE ); // 2015.06.11
		return;
	}
	if ( !IO_DEBUG_RUNNING() ) {
		//
		if ( _i_SCH_SYSTEM_PAUSE_SW_GET() == 0 ) { // 2009.07.10
			//
			if ( _i_SCH_SYSTEM_PAUSE_GET( CHECKING_SIDE ) == 0 ) { // 2009.07.10
				//
//				if ( !EQUIP_RUNNING_CHECK( 60 ) ) { // 2008.04.18 // 2018.09.04
				if ( !EQUIP_RUNNING_CHECK( 60 , &notRun ) ) { // 2008.04.18 // 2018.09.04
					//
					if ( Scheduler_Main_Lock_WAITING_FOR_MTLOUT() ) { // 2019.02.28
						_in_Main_locking_Count[CHECKING_SIDE] = 0;
					}
					else {
						//-------------------------------------------------
						// 2009.11.06
						//-------------------------------------------------
						sprintf( FileName_S , "%s/LOTLOG/%s/Deadlock" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( CHECKING_SIDE ) ); // 2009.11.06
						//-------------------------------------------------
						sprintf( FileName_A , "%s_2.cil" , FileName_S );
						sprintf( FileName_B , "%s_1.cil" , FileName_S );
						DeleteFile( FileName_A );
						rename( FileName_B , FileName_A );
						//
						sprintf( FileName_A , "%s.cil" , FileName_S );
						rename( FileName_A , FileName_B );
						//-------------------------------------------------
						sprintf( FileName_A , "%s_2.log" , FileName_S );
						sprintf( FileName_B , "%s_1.log" , FileName_S );
						DeleteFile( FileName_A );
						rename( FileName_B , FileName_A );
						//
						sprintf( FileName_A , "%s.log" , FileName_S );
						rename( FileName_A , FileName_B );
						//-------------------------------------------------
						sprintf( FileName_B , "%s.jpg" , FileName_S );
						//-------------------------------------------------
						SCHEDULING_DATA_STRUCTURE_LOG( FileName_A , "DEADLOCK_RESULT" , -1 , 0 , 0 );
						SCHEDULING_DATA_STRUCTURE_JOB_LOG( FileName_A , "DEADLOCK_RESULT" );
						//
						IO_DEBUG_FILE_ENABLE( -1 , FileName_A , 500 , FALSE ); // 2008.08.14
						//
						KWIN_SCREEN_CAPTURE( GET_WINDOW_X_REFERENCE_POSITION() , GET_WINDOW_Y_REFERENCE_POSITION() , GET_WINDOW_XS_REFERENCE_POSITION() , GET_WINDOW_YS_REFERENCE_POSITION() , FileName_B );
						sprintf( FileName_A , "%s.cil" , FileName_S );
						rename( FileName_B , FileName_A );
						//
						//
						// 2018.09.04
						//
						//
						if ( _in_Main_locking_Count[CHECKING_SIDE] == 0 ) {
							ERROR_HANDLER( ERROR_DEADLOCK_HAPPEN , "Deadlock Staus has been occured" );
						}
						//
						//
						//
						//
						//
						//
						_in_Main_locking_Count[CHECKING_SIDE]++; // 2017.07.26
						//
						//==========================================================================
						_SCH_COMMON_SYSTEM_CONTROL_FAIL( FALSE , CHECKING_SIDE , _in_Main_locking_Count[CHECKING_SIDE] , 0 , 0 ); // 2017.07.26
						//==========================================================================
						//
					}
					//
//					EQUIP_RUNNING_CHECK( 0 ); // 2018.09.04
					EQUIP_RUNNING_CHECK( 0 , NULL ); // 2018.09.04
					//
					//
				}
				else { // 2017.07.26
					//
//					_in_Main_locking_Count[CHECKING_SIDE] = 0; // 2018.09.04
					//
					if ( !notRun ) { // 2018.09.04
						_in_Main_locking_Count[CHECKING_SIDE] = 0;
					}
					//
				}
			}
		}
	}
	else {
//		EQUIP_RUNNING_CHECK( 0 ); // 2018.09.04
		EQUIP_RUNNING_CHECK( 0 , NULL ); // 2018.09.04
	}
}


void Scheduler_Main_Running_th_Sub( int data ) { // 2002.07.10
	int res;
	char Buffer2[256];
	//============================================================================================
	// 2007.11.30
	//============================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( ( data % 100 ) + CM1 ) ) { // 2005.09.22
		//
//		strcpy( Buffer2 , "" ); // 2017.01.05
		//
		_SCH_COMMON_REVISION_HISTORY( 1 , Buffer2 , 255 );
		//
		if ( strlen( Buffer2 ) <= 0 ) {
			_i_SCH_LOG_LOT_PRINTF( ( data % 100 ) , "Scheduling Start with(%s,%s)....%cSCHSTART %s:%d.%d(%d:%d):%s:%s:%s:%d:%d:%d:%d:%d:P%d:(%s %s)\n" , _i_SCH_SYSTEM_GET_JOB_ID( data % 100 ) , _i_SCH_SYSTEM_GET_MID_ID( data % 100 ) , 9 , SCH_LASTUPDATE_DATE , SYSTEM_RUN_ALG_STYLE_GET() , SYSTEM_SUB_ALG_STYLE_GET() , ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() * 10 ) + _i_SCH_PRM_GET_MODULE_MODE( FM ) , _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , _i_SCH_SYSTEM_GET_JOB_ID( data % 100 ) , _i_SCH_SYSTEM_GET_MID_ID( data % 100 ) , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( data % 100 ) , _i_SCH_SYSTEM_FA_GET( data % 100 ) , _i_SCH_SYSTEM_CPJOB_GET( data % 100 ) , _i_SCH_SYSTEM_CPJOB_ID_GET( data % 100 ) , _i_SCH_SYSTEM_RUNORDER_GET( data % 100 ) , _i_SCH_SYSTEM_PMMODE_GET( data % 100 ) , _in_IMSI_PralSkip[ data % 100 ] , __DATE__ , __TIME__ );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( ( data % 100 ) , "Scheduling Start with(%s,%s)....%cSCHSTART %s:%d.%d(%d:%d):%s:%s:%s:%d:%d:%d:%d:%d:P%d:%s:(%s %s)\n" , _i_SCH_SYSTEM_GET_JOB_ID( data % 100 ) , _i_SCH_SYSTEM_GET_MID_ID( data % 100 ) , 9 , SCH_LASTUPDATE_DATE , SYSTEM_RUN_ALG_STYLE_GET() , SYSTEM_SUB_ALG_STYLE_GET() , ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() * 10 ) + _i_SCH_PRM_GET_MODULE_MODE( FM ) , _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() , _i_SCH_SYSTEM_GET_JOB_ID( data % 100 ) , _i_SCH_SYSTEM_GET_MID_ID( data % 100 ) , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( data % 100 ) , _i_SCH_SYSTEM_FA_GET( data % 100 ) , _i_SCH_SYSTEM_CPJOB_GET( data % 100 ) , _i_SCH_SYSTEM_CPJOB_ID_GET( data % 100 ) , _i_SCH_SYSTEM_RUNORDER_GET( data % 100 ) , _i_SCH_SYSTEM_PMMODE_GET( data % 100 ) , _in_IMSI_PralSkip[ data % 100 ] , Buffer2 , __DATE__ , __TIME__ );
		}
		//
		OPERATE_CONTROL( ( data % 100 ) , 2 );
		//
	}
	//============================================================================================
	res = Scheduler_Main_Running_Sub( data % 100 , data / 100 );
	//============================================================================================
	SYSTEM_THDCHECK_SET( data % 100 , FALSE ); // 2005.10.06
	//----------------------------------------------------------------------
	switch( res ) {
	case SYS_SUCCESS :
		EnterCriticalSection( &CR_EndSigCheck );
		SYSTEM_ENDCHECK_SET( data % 100 , 2 );
		LeaveCriticalSection( &CR_EndSigCheck );
		break;
	case SYS_ERROR :
		EnterCriticalSection( &CR_EndSigCheck );
		SYSTEM_ENDCHECK_SET( data % 100 , 3 );
		LeaveCriticalSection( &CR_EndSigCheck );
		break;
	default :
		EnterCriticalSection( &CR_EndSigCheck );
		SYSTEM_ENDCHECK_SET( data % 100 , 4 );
		LeaveCriticalSection( &CR_EndSigCheck );
		break;
	}
//	SYSTEM_THDCHECK_SET( data % 100 , FALSE ); // 2005.10.06
	_endthread();
}
//
int Scheduler_Main_Running( int CHECKING_SIDE , int MAXTMATM ) { // 2002.07.10
	int i;

	EnterCriticalSection( &CR_EndSigCheck );
	//
	SYSTEM_ENDCHECK_SET( CHECKING_SIDE , 0 );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) _i_SCH_SYSTEM_TM_SET( i , CHECKING_SIDE , 0 ); // 2003.05.26
	SYSTEM_THDCHECK_SET( CHECKING_SIDE , TRUE );
	LeaveCriticalSection( &CR_EndSigCheck );
	if ( _beginthread( (void *) Scheduler_Main_Running_th_Sub , 0 , (void *) ( CHECKING_SIDE + (MAXTMATM*100) ) ) == -1 ) {
		//----------------------------------------------------------------------------------------------------------------
		// 2004.08.18
		//----------------------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Running_th_Sub %d\n" , CHECKING_SIDE + (MAXTMATM*100) );
		//
		SYSTEM_THDCHECK_SET( CHECKING_SIDE , FALSE );
		SYSTEM_ENDCHECK_SET( CHECKING_SIDE , 4 );
		//----------------------------------------------------------------------------------------------------------------
	}
	//
	Scheduler_Main_Lock_Init( CHECKING_SIDE ); // 2017.07.26
	//
	while( TRUE ) {
		//
		//
		//
		//
		EnterCriticalSection( &_CR_Main_Lock ); // 2018.11.21
		//
		Scheduler_Main_Locking( CHECKING_SIDE ); // 2018.11.21
		//
		LeaveCriticalSection( &_CR_Main_Lock ); // 2018.11.21
		//
		//
		//
		//
		//
		EnterCriticalSection( &CR_EndSigCheck );
		//================================================================================
		// 2008.04.17
		//================================================================================
//		Scheduler_Main_Locking( CHECKING_SIDE ); // 2018.11.21
		//================================================================================
		switch( SYSTEM_ENDCHECK_GET( CHECKING_SIDE ) ) {
		case 1 :
		case 2 :
			//================================================================================
			// 2008.04.17
			//================================================================================
			Scheduler_Main_Locking( -1 );
			//================================================================================
			LeaveCriticalSection( &CR_EndSigCheck );
			return SYS_SUCCESS;
			break;
		case 3 :
			//================================================================================
			// 2008.04.17
			//================================================================================
			Scheduler_Main_Locking( -1 );
			//================================================================================
			LeaveCriticalSection( &CR_EndSigCheck );
			return SYS_ERROR;
			break;
		case 4 :
			//================================================================================
			// 2008.04.17
			//================================================================================
			Scheduler_Main_Locking( -1 );
			//================================================================================
			LeaveCriticalSection( &CR_EndSigCheck );
			return SYS_ABORTED;
			break;
		}
		//================================================================================
//		Sleep(250); // 2008.04.18 // 2008.10.02
		//================================================================================
		LeaveCriticalSection( &CR_EndSigCheck );
		//================================================================================
//		Sleep(10); // 2008.04.18
		Sleep(250); // 2008.04.18 // 2008.10.02
		//================================================================================
	}
}
//

extern int _Trace_FM_Flow_Kill[MAX_CASSETTE_SIDE]; // 2012.01.29
extern int _Trace_TM_Flow_Kill[MAX_CASSETTE_SIDE][MAX_TM_CHAMBER_DEPTH];

BOOL Scheduler_Main_Wait_Finish_Complete( int CHECKING_SIDE , int mode ) { // 2003.05.22
	int i , j , t , k , x;
	char Buffer[256]; // 2004.08.16
	char Buffer2[32]; // 2004.08.16
//------------------------------------------
//
//	if ( mode == 0 ) { // 2004.02.16
//		t = 720000; // 72000 sec
//	}
//	else {
//		t = 60; // 6 sec
//	}
//
//------------------------------------------
// 2004.07.09
//------------------------------------------
//	if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) { // 2004.07.07 // 2005.12.14
//	if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) || ( mode == 4 ) || ( mode == 99 ) ) { // 2004.07.07 // 2005.12.14 // 2010.07.29
	if ( mode == -1 ) { // 2012.01.29
		t = 1;
	}
	else {
		if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) || ( mode == 9 ) ) { // 2004.07.07 // 2005.12.14 // 2010.07.29
			//t = 720000; // 72000 sec // 2004.08.16
			t = 360000; // 36000 sec // 2004.08.16
		}
		else {
			//t = 60; // 6 sec // 2004.08.16
			t = 300; // 30 sec // 2004.08.16
		}
	}
//------------------------------------------
	_Trace_FM_Flow_Kill[CHECKING_SIDE] = 1; // 2012.01.29
	for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) _Trace_TM_Flow_Kill[CHECKING_SIDE][j] = 1; // 2010.07.29
//------------------------------------------
//	for ( j = 0 ; j <= 60 ; j++ ) { // 2004.02.16
	for ( j = 0 , x = 0; j <= t ; j++ ) { // 2004.02.16
		//--------------------------------------------------------------------
		// 2004.08.16
		//--------------------------------------------------------------------
		if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) <= 0 ) {
			//
//			if ( mode >= 3 ) { // 2010.07.29 // 2012.01.29
			if ( ( mode == -1 ) || ( mode >= 3 ) ) { // 2010.07.29 // 2012.01.29
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					if ( _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) > 0 ) break;
				}
			}
			else { // 2010.07.29
				i = MAX_TM_CHAMBER_DEPTH;
			}
			//
			if ( i == MAX_TM_CHAMBER_DEPTH ) {
				if ( !SYSTEM_THDCHECK_GET( CHECKING_SIDE ) ) {
					if ( x != 0 ) { // 2005.08.26
						_IO_CIM_PRINTF( "SCHEDULING SIDE %d has a bad FM/TM Flag[%d] Cleared at %d secs\n" , CHECKING_SIDE , mode , j / 10 );
					}
					return TRUE;
				}
			}
		}
		//
		if ( mode == -1 ) return FALSE; // 2012.01.29
		//
		k = FALSE;
		if ( j > 150 ) { // 2006.01.17 over 15 sec
//			if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) || ( mode == 4 ) ) { // 2010.07.29
			if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) || ( mode == 9 ) ) { // 2010.07.29
	//			if ( ( j % 3000 ) == 0 ) k = TRUE; // 5 min // 2005.08.26
				if ( ( j % 1800 ) == 0 ) k = TRUE; // 3 min // 2005.08.26
			}
			else {
	//			if ( ( j % 1800 ) == 0 ) k = TRUE; //   3 min // 2005.08.26
				if ( ( j %  300 ) == 0 ) k = TRUE; // 0.5 min // 2005.08.26
			}
		}
		if ( k ) {
			x++;
			strcpy( Buffer , "" );
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				sprintf( Buffer2 , "[%d]" , _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) );
				strcat( Buffer , Buffer2 );
			}
			_IO_CIM_PRINTF( "SCHEDULING SIDE %d has a bad FM/TM Flag[%d][%d][{%d}%s] %d times\n" , CHECKING_SIDE , mode , SYSTEM_THDCHECK_GET( CHECKING_SIDE ) , _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) , Buffer , x );
		}
		//--------------------------------------------------------------------
/*
		//--------------------------------------------------------------------
		// 2004.07.09
		//--------------------------------------------------------------------
		if ( _i_SCH_MODULE_Get_FM_Runinig_Flag( CHECKING_SIDE ) <= 0 ) { // 2004.07.09
		//--------------------------------------------------------------------
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2003.05.26
				if ( _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) > 0 ) break;
			}
			if ( i == MAX_TM_CHAMBER_DEPTH ) {
				if ( !SYSTEM_THDCHECK_GET( CHECKING_SIDE ) ) return;
			}
			else { // 2003.06.05
				if ( ( j == 30 ) || ( j == 60 ) ) { // 2004.02.16
				if ( ( j % 30 ) == 0 ) { // 2004.02.16
					_IO_CIM_PRINTF( "SCHEDULING SIDE %d has a bad TM Flag[%d]" , CHECKING_SIDE , mode );
					for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
						_IO_CIM_PRINTF( "[%d]" , _i_SCH_SYSTEM_TM_GET( i , CHECKING_SIDE ) );
					}
					_IO_CIM_PRINTF( "\n" );
					------------------------------------------------------------------------------
					//==================================================================================
					if ( !SYSTEM_THDCHECK_GET( CHECKING_SIDE ) ) return;
					//==================================================================================
				}
			}
		}
*/
		Sleep(100);
	}
	_IO_CIM_PRINTF( "SCHEDULING SIDE %d has a bad FM/TM THD Flag[%d]\n" , CHECKING_SIDE , mode );
	//
	return TRUE;
}
//
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
//===============================================================================================================================
