#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "FA_Handling.h"
#include "Timer_Handling.h"
#include "IO_Part_Log.h"
#include "Equip_Handling.h"
#include "sch_CassetteResult.h"
#include "sch_sub.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_prm.h"
#include "sch_EIL.h"
#include "RcpFile_Handling.h"
#include "multijob.h"
#include "iolog.h"
#include "Gui_Handling.h"
#include "Utility.h"

#include "sch_estimate.h" // 2016.10.22

#include "INF_sch_CommonUser.h"


//------------------------------------------------------------------------------------------
extern char *_lot_printf_sub_string_Dummy[];
//------------------------------------------------------------------------------------------
int PROCESSLOG_MONITOR_IO[ MAX_CHAMBER ];
//------------------------------------------------------------------------------------------
int				PROCESS_DATALOG_BUFFER_SIZE[ MAX_CHAMBER ];	// 2007.10.23
//------------------------------------------------------------------------------------------
int				PROCESS_PRJOB_End_Delay_Time = 0;	// 2005.04.06
//------------------------------------------------------------------------------------------
int				PROCESS_MONITORING_TIME = 250;
int				PROCESS_MONITORING_TIME_MINIMUM_LOG = 0; // 2002.03.06
//int				PROCESS_MONITORING_WITH_CPJOB = FALSE; // 2002.06.28 // 2019.04.05
int				PROCESS_MONITORING_WITH_CPJOB = 0; // 2002.06.28 // 2019.04.05
//int				PROCESS_MONITORING_AT_MODULE = FALSE; // 2004.04.06 // 2005.02.16
//------------------------------------------------------------------------------------------
int				PROCESS_MONITORING_RECIPE_DATA_SAVE_SKIP = FALSE; // 2004.04.27
//------------------------------------------------------------------------------------------
int				PM_PROCESS_MONITOR_ONE_BODY[ MAX_CHAMBER ]; // 2006.06.28
int				PM_PROCESS_SIDE_ONE_BODY[ MAX_CHAMBER ]; // 2006.06.28
int				PM_PROCESS_POINTER_ONE_BODY[ MAX_CHAMBER ]; // 2011.05.13
//
int				PM_PROCESS_MONITOR[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
int				PM_PROCESS_LAST[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
int				PM_PROCESS_POINTER[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
int				PM_PROCESS_NO1[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2002.03.25
int				PM_PROCESS_NO2[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2002.03.25
int				PM_PROCESS_PATHIN[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2010.05.12
int				PM_PROCESS_DUMMY[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.04.19
int				PM_PROCESS_FNOTUSE[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.07.25
int				PM_PROCESS_WFRNOTCHECK[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.10.11
int				PM_PROCESS_OPTION[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.11.07

int				PM_PROCESS_CHP[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2016.04.26
int				PM_PROCESS_CHS[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2016.04.26
//
int				PM_PROCESS_SIDE_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int				PM_PROCESS_POINTER_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int				PM_PROCESS_WAFER_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int				PM_PROCESS_DUMMY_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int				PM_PROCESS_MULTIUNUSE_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2014.02.10
//
int				PR_MON_Side_ex[MAX_PM_SLOT_DEPTH];
int				PR_MON_Pointer_ex[MAX_PM_SLOT_DEPTH];
int				PR_MON_Slot_ex[MAX_PM_SLOT_DEPTH];
int				PR_MON_Dummy_ex[MAX_PM_SLOT_DEPTH];
int				PR_MON_MultiUnuse_ex[MAX_PM_SLOT_DEPTH]; // 2014.02.10

//char			PM_PROCESS_RECIPE[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ][MAX_PROCESS_RECIPE_STRING_SIZE+1]; // 2007.01.26
char			*PM_PROCESS_RECIPE[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.01.26
int				PM_PROCESS_RECIPE_LENGTH[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.01.26
int				PM_PROCESS_RECIPE_LENGTH_CURR[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ]; // 2007.01.26 // 2007.10.02 // 2007.10.18
//char			PM_PROCESS_RECIPE_BLANK[2] = { 0 , 0 };
//------------------------------------------------------------------------------------------
int				PM_PROCESS_MONITOR_FINISH_CONFIRM[ MAX_CHAMBER ]; // 2007.06.25
//------------------------------------------------------------------------------------------
int				PM_LAST_PROCESS_JOB_SIDE[ MAX_CHAMBER ]; // 2007.08.27
char			*PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ MAX_CHAMBER ]; // 2007.02.22
//
int PROCESSLOG_MONITOR_SIDE_CHANGE[ MAX_CHAMBER ];

char	*_PROCESS_METHOD_FOLDER[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2012.02.18

//------------------------------------------------------------------------------------------
CRITICAL_SECTION CR_PM_INTERFACE;
CRITICAL_SECTION CR_PM_MULTI_PROCESS[MAX_CHAMBER]; // 2011.03.02
//------------------------------------------------------------------------------------------
CRITICAL_SECTION CR_PM_DATA_RESET1[MAX_CHAMBER]; // 2011.04.18
CRITICAL_SECTION CR_PM_DATA_RESET2[MAX_CHAMBER]; // 2011.04.18
//------------------------------------------------------------------------------------------
int PM_INTERFACE_MONITOR_SIDE[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
int PM_INTERFACE_MONITOR_POINTER[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
int PM_INTERFACE_MONITOR_WAFER[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
int PM_INTERFACE_MONITOR_MODE[MAX_CHAMBER][ MAX_INTERFACE_COUNT ];
int PM_INTERFACE_MONITOR_STARTCLOCK[MAX_CHAMBER][MAX_INTERFACE_COUNT]; // 2019.03.08
//
int PM_INTERFACE_MONITOR_SOURCE[MAX_CHAMBER][ MAX_INTERFACE_COUNT ]; // 2018.09.05
int PM_INTERFACE_MONITOR_SOURCE_E[MAX_CHAMBER][ MAX_INTERFACE_COUNT ]; // 2018.09.05
//
int PM_INTERFACE_MONITOR_COUNT[ MAX_CHAMBER ];
//
//
//long PM_INTERFACE_MONITOR_TIMER[ MAX_CHAMBER ][ MAX_INTERFACE_COUNT ]; // 2013.03.18
//time_t PM_INTERFACE_MONITOR_TIMER[ MAX_CHAMBER ][ MAX_INTERFACE_COUNT ]; // 2013.03.18 // 2018.06.07
time_t PM_INTERFACE_MONITOR_RTIMER[ MAX_CHAMBER ]; // 2013.03.18 // 2018.06.07
//------------------------------------------------------------------------------------------
BOOL Process_Monitor_Sts_Find[MAX_CASSETTE_SIDE];
int  Process_Monitor_Sts_Abort[MAX_CASSETTE_SIDE][MAX_CHAMBER];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2011.03.02
int Process_Append_Side[MAX_CHAMBER];
int Process_Append_Pointer[MAX_CHAMBER];
int Process_Append_StartChamber[MAX_CHAMBER];
int Process_Append_Slot[MAX_CHAMBER];
int Process_Append_Finger[MAX_CHAMBER];
char *Process_Append_Recipe[MAX_CHAMBER];
int Process_Append_mode[MAX_CHAMBER];
int Process_Append_PutTime[MAX_CHAMBER];
char *Process_Append_NextPM[MAX_CHAMBER];
int Process_Append_MinTime[MAX_CHAMBER];
int Process_Append_index[MAX_CHAMBER];
int Process_Append_status[MAX_CHAMBER];
int Process_Append_no1[MAX_CHAMBER];
int Process_Append_no2[MAX_CHAMBER];
int Process_Append_last[MAX_CHAMBER];
int Process_Append_append[MAX_CHAMBER];

//void PROCESS_MONITOR_Setting1( int ch , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , BOOL append ); // 2014.02.13
void PROCESS_MONITOR_Setting1( int ch , int chp , int chs , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , BOOL append , BOOL WfrNotCheck ); // 2014.02.13
BOOL PROCESS_MONITOR_Set_Last_ProcessJob_Data( int chamber , int side , int pointer ); // 2011.07.18


//------------------------------------------------------------------------------------------
// 2011.11.10
//------------------------------------------------------------------------------------------

/*
//
// 2015.04.09
//
int _sch_dRUN_FUNCTIONF( BOOL set , int ind , LPSTR list , ... ) {
	va_list va;
	int Ms;
	char	*TextBuffer;
	//
	TextBuffer  = calloc( 4096 + 1 , sizeof( char ) );
	if ( TextBuffer == NULL ) {
		_IO_CIM_PRINTF( "_sch_dRUN_FUNCTIONF(%d) Buffer Initial Error\n" , ind );
		return SYS_ABORTED;
	}
	//
	va_start( va , list );
	vsprintf( TextBuffer , list , (LPSTR) va );
	va_end( va );
	//
	if ( set ) {
		Ms = _dRUN_SET_FUNCTION( ind , TextBuffer );
	}
	else {
		Ms = _dRUN_FUNCTION( ind , TextBuffer );
	}
	//
	free( TextBuffer );
	return Ms;
}
*/

//
// 2015.04.09
//
int _sch_dRUN_FUNCTIONF( BOOL set , int ind , LPSTR list , ... ) {
	va_list va;
	int Ms;
	char	TextBuffer[4097];
	//
	va_start( va , list );
	vsprintf( TextBuffer , list , (LPSTR) va );
	va_end( va );
	//
	if ( set ) {
		Ms = _dRUN_SET_FUNCTION( ind , TextBuffer );
	}
	else {
		Ms = _dRUN_FUNCTION( ind , TextBuffer );
	}
	//
	return Ms;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void PROCESS_DATALOG_Init_BUFFER_SIZE() { // 2007.10.23
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) PROCESS_DATALOG_BUFFER_SIZE[ i ] = 0;	// 2007.10.23
}
//------------------------------------------------------------------------------------------
void PROCESS_DATALOG_Set_BUFFER_SIZE( int ch , int data ) { // 2007.10.23
	int i;
	if ( ch == -1 ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) PROCESS_DATALOG_BUFFER_SIZE[ i ] = data;	// 2007.10.23
	}
	else {
		PROCESS_DATALOG_BUFFER_SIZE[ch] = data;
	}
}
//------------------------------------------------------------------------------------------
int  PROCESS_DATALOG_Get_BUFFER_SIZE( int ch ) { // 2007.10.23
	return ( PROCESS_DATALOG_BUFFER_SIZE[ch] );
}
//------------------------------------------------------------------------------------------
void PROCESS_PRJOB_End_Set_Delay_Time( int timedata ) { // 2005.04.06
	PROCESS_PRJOB_End_Delay_Time = timedata;
}
//------------------------------------------------------------------------------------------
void PROCESS_MONITOR_Set_MONITORING_TIME( int timedata ) {
	PROCESS_MONITORING_TIME = timedata;
}
//------------------------------------------------------------------------------------------
void PROCESS_MONITOR_Set_MONITORING_TIME_MINIMUM_LOG( int data ) {
	//
	PROCESS_MONITORING_TIME_MINIMUM_LOG = data;
	//
	switch( GET_DATE_TIME_FORMAT() / 100 ) { // 2019.01.18
	case 1 : // %d y/m/d
	case 2 : // %0?d y/m/d
	case 3 : // %d y-m-d
	case 4 : // %0?d y-m-d
		//
		if ( ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) == 0 ) {
			PROCESS_MONITORING_TIME_MINIMUM_LOG = 2 + ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 );
		}
		//
		break;
	case 5 : // %d y/m/d
	case 6 : // %0?d y/m/d
	case 7 : // %d y-m-d
	case 8 : // %0?d y-m-d
		//
		if ( ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) == 1 ) {
			PROCESS_MONITORING_TIME_MINIMUM_LOG = PROCESS_MONITORING_TIME_MINIMUM_LOG % 2;
		}
		//
		break;
	}
	//
}
//------------------------------------------------------------------------------------------
// 2005.02.16
//void PROCESS_MONITOR_Set_MONITORING_WHERE_LOG_SIDE_AT_MODULE( int data ) { // 2004.04.06
//	if ( data == 2 ) {
//		PROCESS_MONITORING_AT_MODULE = TRUE;
//		if      ( PROCESS_MONITORING_TIME_MINIMUM_LOG == 0 )	PROCESS_MONITORING_TIME_MINIMUM_LOG = 2;
//		else if ( PROCESS_MONITORING_TIME_MINIMUM_LOG == 1 )	PROCESS_MONITORING_TIME_MINIMUM_LOG = 3;
//	}
//	else {
//		PROCESS_MONITORING_AT_MODULE = data;
//		if      ( PROCESS_MONITORING_TIME_MINIMUM_LOG == 2 )	PROCESS_MONITORING_TIME_MINIMUM_LOG = 0; // 2005.02.16
//		else if ( PROCESS_MONITORING_TIME_MINIMUM_LOG == 3 )	PROCESS_MONITORING_TIME_MINIMUM_LOG = 1; // 2005.02.16
//	}
//}
//------------------------------------------------------------------------------------------
//void PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( BOOL data ) { // 2019.04.05
void PROCESS_MONITOR_Set_MONITORING_WITH_CPJOB( int data ) { // 2019.04.05
	PROCESS_MONITORING_WITH_CPJOB = data;
}
//------------------------------------------------------------------------------------------
//BOOL PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() { // 2019.04.05
int PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() { // 2019.04.05
	return PROCESS_MONITORING_WITH_CPJOB;
}


char	*_PROCESS_METHOD_FOLDER[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2012.02.18

//=======================================================================================
void PROCESS_MAKE_SLOT_FOLDER( int side , int pt , char *data ) { // 2012.02.18
	STR_MEM_MAKE_COPY2( &(_PROCESS_METHOD_FOLDER[side][pt]) , data );
}
//=======================================================================================
char *PROCESS_Get_SLOT_FOLDER( int side , int pt ) { // 2012.02.18
	//
	if ( side < 0 ) return STR_MEM_GET_STR( NULL );
	if ( side >= MAX_CASSETTE_SIDE ) return STR_MEM_GET_STR( NULL );
	if ( pt < 0 ) return STR_MEM_GET_STR( NULL );
	if ( pt >= MAX_CASSETTE_SLOT_SIZE ) return STR_MEM_GET_STR( NULL );
	//
	return STR_MEM_GET_STR( _PROCESS_METHOD_FOLDER[side][pt] );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Process Monitor Operation
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_PROCESS_MONITOR_DATA( int apmode , int side ) {
	int i , j;
	if ( apmode == 0 ) {
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
				PM_PROCESS_MONITOR[ i ][ j ] = 0;
	//			strcpy( PM_PROCESS_RECIPE[ i ][ j ] , "" ); // 2007.01.26
				PM_PROCESS_RECIPE[ i ][ j ] = NULL; // 2007.01.26
				PM_PROCESS_RECIPE_LENGTH[ i ][ j ] = 0; // 2007.01.26
			}
		}
		//====================================================================
		// 2004.09.07
		//====================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			PROCESSLOG_MONITOR_IO[i] = -1;
			PM_LAST_PROCESS_JOB_SIDE[i] = -1;
			PM_LAST_PROCESS_JOB_CONTROL_PROCESS[i] = NULL;
		}
		//====================================================================
		// 2011.03.02
		//====================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			Process_Append_Side[i] = -1;
			Process_Append_Recipe[i] = NULL;
			Process_Append_NextPM[i] = NULL;
		}
		//====================================================================
		// 2012.02.18
		//====================================================================
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				_PROCESS_METHOD_FOLDER[ i ][ j ] = NULL;
			}
		}
		//====================================================================
	}
	else if ( apmode == 3 ) { // 2008.12.12
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
				if ( PM_PROCESS_MONITOR[ i ][ j ] < 0 ) {
					PM_PROCESS_MONITOR[ i ][ j ] = 0;
				}
			}
		}
	}
	//
	if ( ( apmode == 0 ) || ( apmode == 3 ) || ( apmode == 4 ) ) { // 2012.02.18
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
				STR_MEM_MAKE_COPY2( &(_PROCESS_METHOD_FOLDER[ i ][ j ]) , "" );
			}
		}
		//
	}
}
//===================================================================================================
/*
void PROCESS_MONITOR_ttt( int side , int chamber , int status , char *data ) {
	FILE *fpt;
	int xp;
	char filename[256];

	sprintf( filename , "S%d_P%d_%d.log" , side , chamber - PM1 + 1 , status );

	fpt = fopen( filename , "a" );

	fprintf( fpt , "data        = %s\n" , data );

	for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
		fprintf( fpt , "	[%02d] PM_PROCESS_SIDE_EX    = %d	%d\n" , xp , PM_PROCESS_SIDE_EX[ chamber ][xp] , _i_SCH_MODULE_Get_PM_SIDE( chamber , xp  ) );
		fprintf( fpt , "	[%02d] PM_PROCESS_POINTER_EX = %d	%d\n" , xp , PM_PROCESS_POINTER_EX[ chamber ][xp] , _i_SCH_MODULE_Get_PM_POINTER( chamber , xp  ) );
		fprintf( fpt , "	[%02d] PM_PROCESS_WAFER_EX   = %d	%d\n" , xp , PM_PROCESS_WAFER_EX[ chamber ][xp] , _i_SCH_MODULE_Get_PM_WAFER( chamber , xp  ) );
		fprintf( fpt , "	[%02d] PM_PROCESS_DUMMY_EX   = %d	%d\n" , xp , PM_PROCESS_DUMMY_EX[ chamber ][xp] );
	}
	fprintf( fpt , "    SET\n" );
	fprintf( fpt , "    	PM_PROCESS_OPTION      = %d\n" , PM_PROCESS_OPTION[ side ][ chamber ] );
	fprintf( fpt , "    	PM_PROCESS_FNOTUSE     = %d\n" , PM_PROCESS_FNOTUSE[ side ][ chamber ] );
	fprintf( fpt , "    	PM_PROCESS_PATHIN      = %d\n" , PM_PROCESS_PATHIN[ side ][ chamber ] );
	fprintf( fpt , "    	PM_PROCESS_DUMMY       = %d\n" , PM_PROCESS_DUMMY[ side ][ chamber ] );
	fprintf( fpt , "    	PM_PROCESS_WFRNOTCHECK = %d\n" , PM_PROCESS_WFRNOTCHECK[ side ][ chamber ] );

	fclose( fpt );
}
*/

void PROCESS_MONITOR_Set_Status_Sub( int side , int chamber , int chp , int chs , int status , int dummy , BOOL notuse , int wfrNotCheck , int pathin , 
									int s2 , int p2 , int w2 , BOOL dummy2 , BOOL append ,
									int option
									) {
	int xp , off;

//	PM_PROCESS_MONITOR[ side ][ chamber ] = status; // 2007.04.19
	if ( ( status > 0 ) && ( chamber < BM1 ) ) {

/*
		{
			FILE *fpt;
			char strdata[256];
			sprintf( strdata , "S%d_P%d_%d.log" , side , chamber - PM1 + 1 , status );
			fpt = fopen( strdata , "a" );

			fprintf( fpt , "side        = %d\n" , side );
			fprintf( fpt , "chamber     = PM%d\n" , chamber - PM1 + 1 );
			fprintf( fpt , "status      = %d\n" , status );
			fprintf( fpt , "dummy       = %d\n" , dummy );
			fprintf( fpt , "wfrNotCheck = %d\n" , wfrNotCheck );
			fprintf( fpt , "pathin      = %d\n" , pathin );
			fprintf( fpt , "s2          = %d\n" , s2 );
			fprintf( fpt , "p2          = %d\n" , pathin );
			fprintf( fpt , "w2          = %d\n" , w2 );
			fprintf( fpt , "dummy2      = %d\n" , dummy2 );
			fprintf( fpt , "append      = %d\n" , append );
			fprintf( fpt , "option      = %d\n" , option );

			fclose( fpt );
		}
*/
//PROCESS_MONITOR_ttt( side , chamber , status , "1" );

		//-------------------------------------------------------------------------------------------
		if ( _i_SCH_SYSTEM_USING_GET( side ) <= 0 ) Process_Monitor_Sts_Find[side] = TRUE; // 2010.04.02
		//-------------------------------------------------------------------------------------------
		PM_PROCESS_PATHIN[ side ][ chamber ] = pathin; // 2010.05.12
		PM_PROCESS_DUMMY[ side ][ chamber ] = dummy; // 2007.05.02
		PM_PROCESS_WFRNOTCHECK[ side ][ chamber ] = ( wfrNotCheck == 1 ) || ( wfrNotCheck == 3 ); // 2007.10.11
		//-------------------------------------------------------------------------------------------
		if ( s2 == 9999 ) {
			//
			for ( xp = 0 ; xp < ( MAX_PM_SLOT_DEPTH - 1 ) ; xp++ ) {
				if ( _i_SCH_MODULE_Get_PM_WAFER( chamber , xp + 1 ) > 0 ) {
					PM_PROCESS_SIDE_EX[ chamber ][xp] = _i_SCH_MODULE_Get_PM_SIDE( chamber , xp+1 );
					PM_PROCESS_POINTER_EX[ chamber ][xp] = _i_SCH_MODULE_Get_PM_POINTER( chamber , xp+1 );
					PM_PROCESS_WAFER_EX[ chamber ][xp] = _i_SCH_MODULE_Get_PM_WAFER( chamber , xp+1 );
					PM_PROCESS_DUMMY_EX[ chamber ][xp] = SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( _i_SCH_MODULE_Get_PM_SIDE( chamber , xp+1 ) , _i_SCH_MODULE_Get_PM_POINTER( chamber , xp+1 ) ) , 1 );
				}
				else {
					PM_PROCESS_SIDE_EX[ chamber ][xp] = -1;
					PM_PROCESS_POINTER_EX[ chamber ][xp] = -1;
					PM_PROCESS_WAFER_EX[ chamber ][xp] = -1;
					PM_PROCESS_DUMMY_EX[ chamber ][xp] = FALSE;
				}
				//
				PM_PROCESS_MULTIUNUSE_EX[ chamber ][xp] = FALSE; // 2014.02.10
				//
			}
			//
		}
		else {
			PM_PROCESS_SIDE_EX[ chamber ][0] = s2;
			PM_PROCESS_POINTER_EX[ chamber ][0] = p2;
			PM_PROCESS_WAFER_EX[ chamber ][0] = w2;
			PM_PROCESS_DUMMY_EX[ chamber ][0] = dummy2;
			//
			PM_PROCESS_MULTIUNUSE_EX[ chamber ][0] = TRUE; // 2014.02.10
			//
			xp = 1;
		}
		//
		for ( ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
			PM_PROCESS_SIDE_EX[ chamber ][xp] = -1;
			PM_PROCESS_POINTER_EX[ chamber ][xp] = -1;
			PM_PROCESS_WAFER_EX[ chamber ][xp] = -1;
			PM_PROCESS_DUMMY_EX[ chamber ][xp] = FALSE;
			//
			PM_PROCESS_MULTIUNUSE_EX[ chamber ][xp] = FALSE; // 2014.02.10
			//
		}
		//
//PROCESS_MONITOR_ttt( side , chamber , status , "2" );
		//-------------------------------------------------------------------------------------------
		PM_PROCESS_FNOTUSE[ side ][ chamber ] = notuse; // 2007.07.25
		//-------------------------------------------------------------------------------------------
		if ( !append ) { // 2010.04.05
			for ( xp = 0 ; xp < MAX_CASSETTE_SIDE ; xp++ ) { // 2009.08.21
				if ( xp != side ) {
					if ( PM_PROCESS_MONITOR[ xp ][ chamber ] < 0 ) PM_PROCESS_MONITOR[ xp ][ chamber ] = 0;
				}
			}
		}
		//
		PM_PROCESS_MONITOR[ side ][ chamber ] = status; // 2007.04.19
		//
		if ( append ) { // 2010.04.05
			for ( xp = 0 ; xp < MAX_CASSETTE_SIDE ; xp++ ) {
				if ( xp != side ) {
					PM_PROCESS_MONITOR[ xp ][ chamber ] = 0;
				}
			}
		}
//PROCESS_MONITOR_ttt( side , chamber , status , "3" );
		//-------------------------------------------------------------------------------------------
		PM_PROCESS_OPTION[ side ][ chamber ] = option; // 2007.11.07
		//-------------------------------------------------------------------------------------------
		if ( chs > 0 ) { // 2016.04.26
			SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , chp , chs , -2 , -1 , -1 );
		}
		else {
			SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , chamber , status , -1 , -1 , -1 );
		}
		//========================================================================================
		// 2006.07.04
		//========================================================================================
		if ( ( PM_PROCESS_POINTER[ side ][ chamber ] >= 0 ) && ( PM_PROCESS_POINTER[ side ][ chamber ] < MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			if ( !PROCESS_MONITOR_Set_Last_ProcessJob_Data( chamber , side , PM_PROCESS_POINTER[ side ][ chamber ] ) ) { // 2011.07.18
				_IO_CIM_PRINTF( "PROCESS_MONITOR_Set_Status_Sub Memory Allocate Error[%d,%d,%d,%d]\n" , side , PM_PROCESS_POINTER[ side ][ chamber ] , chamber , status );
			}
			//
			//
			/*
			PM_LAST_PROCESS_JOB_SIDE[ chamber ] = side; // 2007.08.27
			//
			if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] == NULL ) {
				PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] = calloc( 128 + 1 , sizeof( char ) );
			}
			//
			if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] != NULL ) {
				SCHMULTI_MESSAGE_GET_ALL( side , PM_PROCESS_POINTER[ side ][ chamber ] , PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] , 128 ); // 2007.02.22
			}
			else {
				_IO_CIM_PRINTF( "PROCESS_MONITOR_Set_Status_Sub Memory Allocate Error[%d,%d,%d]\n" , side , chamber , status ); // 2007.10.23
			}
			*/
			//---------------------------------------------------------------------------------------
			// 2006.07.13
			//---------------------------------------------------------------------------------------
			if ( ( status < PROCESS_INDICATOR_PRE ) && ( PM_PROCESS_NO1[ side ][ chamber ] >= 0 ) ) {
				//
				if ( _i_SCH_PRM_GET_MODULE_GROUP( chamber ) != -1 ) { // 2016.02.23
					//
					_i_SCH_CLUSTER_Set_PathRun( side , PM_PROCESS_POINTER[ side ][ chamber ] , PM_PROCESS_NO1[ side ][ chamber ] , 0 , chamber );
					//
				}
				//
				off = _i_SCH_CLUSTER_Get_PathRun( side , PM_PROCESS_POINTER[ side ][ chamber ] , 20 , 3 );
				//
				if ( off > 0 ) {
					if ( _i_SCH_CLUSTER_Get_PathRun( side , PM_PROCESS_POINTER[ side ][ chamber ] , 21 + off - 1 , 2 ) == 99 ) { // 2008.11.20
						xp = _i_SCH_CLUSTER_Get_PathRun( side , PM_PROCESS_POINTER[ side ][ chamber ] , 21 + off - 1 , 4 ) - 1;
						_i_SCH_CLUSTER_Set_PathRun( side , PM_PROCESS_POINTER[ side ][ chamber ] , 21 + off - 1 , 2 , 0 );
						//
						_i_SCH_CLUSTER_Disable_PathProcessRecipe_MultiData( side , PM_PROCESS_POINTER[ side ][ chamber ] , PM_PROCESS_NO1[ side ][ chamber ] , chamber , 0 , xp );
						//
					}
				}
				//
				//---------------------------------------------------------------------------------------
				// 2009.09.24
				//---------------------------------------------------------------------------------------
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PM_PROCESS_WAFER_EX[ chamber ][xp] <= 0 ) continue;
					if ( PM_PROCESS_SIDE_EX[ chamber ][xp] < 0 ) continue;
					if ( PM_PROCESS_SIDE_EX[ chamber ][xp] >= MAX_CASSETTE_SIDE ) continue;
					if ( PM_PROCESS_POINTER_EX[ chamber ][xp] < 0 ) continue;
					if ( PM_PROCESS_POINTER_EX[ chamber ][xp] >= MAX_CASSETTE_SLOT_SIZE ) continue;
					//
					//
					if ( _i_SCH_PRM_GET_MODULE_GROUP( chamber ) != -1 ) { // 2016.02.23
						//
						_i_SCH_CLUSTER_Set_PathRun( PM_PROCESS_SIDE_EX[ chamber ][xp] , PM_PROCESS_POINTER_EX[ chamber ][xp] , PM_PROCESS_NO1[ side ][ chamber ] , 0 , chamber );
						//
					}
				}
				//---------------------------------------------------------------------------------------
			}
		}
		//========================================================================================
		if ( wfrNotCheck >= 2 ) { // 2009.09.24
//			for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) PM_PROCESS_WAFER_EX[ chamber ][xp] = 0; // 2012.06.16
//			for ( xp = 1 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) PM_PROCESS_WAFER_EX[ chamber ][xp] = 0; // 2012.06.16 // 2014.01.27
//			for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) PM_PROCESS_WAFER_EX[ chamber ][xp] = 0; // 2012.06.16 // 2014.01.27 // 2014.02.10
			for ( xp = 1 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) PM_PROCESS_WAFER_EX[ chamber ][xp] = 0; // 2012.06.16 // 2014.01.27 // 2014.02.10
		}
		//========================================================================================
//PROCESS_MONITOR_ttt( side , chamber , status , "4" );
	}
	else {
		if ( status > 0 ) {
			//-------------------------------------------------------------------------------------------
			if ( _i_SCH_SYSTEM_USING_GET( side ) <= 0 ) Process_Monitor_Sts_Find[side] = TRUE; // 2010.04.02
			//-------------------------------------------------------------------------------------------
			PM_PROCESS_PATHIN[ side ][ chamber ] = 0; // 2010.05.12
			//-------------------------------------------------------------------------------------------
			PM_PROCESS_FNOTUSE[ side ][ chamber ] = FALSE; // 2007.07.25
			//-------------------------------------------------------------------------------------------
			PM_PROCESS_OPTION[ side ][ chamber ] = 0; // 2007.11.07
			//-------------------------------------------------------------------------------------------
			PM_PROCESS_DUMMY[ side ][ chamber ] = 0; // 2007.04.19
			//-------------------------------------------------------------------------------------------
			PM_PROCESS_WFRNOTCHECK[ side ][ chamber ] = FALSE; // 2007.10.11
			//-------------------------------------------------------------------------------------------
			for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
				PM_PROCESS_SIDE_EX[ chamber ][xp] = 0;
				PM_PROCESS_POINTER_EX[ chamber ][xp] = 0;
				PM_PROCESS_WAFER_EX[ chamber ][xp] = 0;
				PM_PROCESS_DUMMY_EX[ chamber ][xp] = FALSE;
				//
				PM_PROCESS_MULTIUNUSE_EX[ chamber ][xp] = FALSE; // 2014.02.10
				//
			}
			//-------------------------------------------------------------------------------------------
		}
		//
		PM_PROCESS_MONITOR[ side ][ chamber ] = status; // 2007.04.19
	}
	//=============================================================================
	// 2007.04.19
	//=============================================================================
	if ( status <= 0 ) {
		for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
			if ( PM_PROCESS_WAFER_EX[ chamber ][xp] > 0 ) {
				PM_PROCESS_MONITOR[ PM_PROCESS_SIDE_EX[ chamber ][xp] ][ chamber ] = status;
				//
				PM_PROCESS_SIDE_EX[ chamber ][xp] = 0;
				PM_PROCESS_POINTER_EX[ chamber ][xp] = 0;
				PM_PROCESS_WAFER_EX[ chamber ][xp] = 0;
				PM_PROCESS_DUMMY_EX[ chamber ][xp] = FALSE;
				//
				PM_PROCESS_MULTIUNUSE_EX[ chamber ][xp] = FALSE; // 2014.02.10
				//
			}
		}
		PM_PROCESS_MONITOR_FINISH_CONFIRM[ chamber ] = TRUE; // 2007.06.25
	}
	else {
		//=============================================================================
		// 2015.08.26
		//=============================================================================
//		SCH_TM_PICKPLACING_PRCS( chamber ); // 2016.03.22

//		SCH_TM_PICKPLACING_PRCS( chamber , ( status >= PROCESS_INDICATOR_POST ) ); // 2016.03.22 // 2017.02.02

		//
		// 2017.02.02
		//
		if ( status < PROCESS_INDICATOR_PRE ) {
			SCH_TM_PICKPLACING_PRCS( chamber , FALSE );
		}
		else if ( status >= PROCESS_INDICATOR_POST ) {
			SCH_TM_PICKPLACING_PRCS( chamber , TRUE );
		}
		//
		//=============================================================================
	}
}
//
void PROCESS_MONITOR_Set_Status( int side , int chamber , int status , BOOL append , int pathin ) {
	PROCESS_MONITOR_Set_Status_Sub( side , chamber , 0 , 0 , status , 0 , FALSE , 0 , pathin ,
		-1 , -1 , -1 , FALSE , append ,
		0 );
}


BOOL PROCESS_MONITOR_Set_Finish_Status( int side , int chamber , int status ) { // 2011.03.02
	//
	EnterCriticalSection( &CR_PM_MULTI_PROCESS[chamber] ); 
	//
	if      ( status == -99 ) {
		//
		Process_Append_Side[chamber] = -1;
		//
		PROCESS_MONITOR_Set_Status_Sub( side , chamber , 0 , 0 , 0 , 0 , FALSE , 0 , -1 ,
			-1 , -1 , -1 , FALSE , FALSE ,
			0 );
	}
	else if ( status == -100 ) {
		//
		if ( Process_Append_Side[chamber] != -1 ) {
			//
			//-----------------------------------------------------------------------------------------------------------------------------------
			if ( Process_Append_mode[chamber] == 0 ) {
//				_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Process Start at %s(S%d)[%s]%cPROCESS_START PM%d:D%d:%s:%d:311\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9001 ); // 2015.04.23
				_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Process Start at %s(S%d)[%s]%cPROCESS_START PM%d:0:D%d:%s:0:%d:311\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 90000 ); // 2015.04.23
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , chamber );
			}
			else {
				if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
					if ( ( Process_Append_mode[chamber] == 2 ) || ( Process_Append_mode[chamber] == 11 ) || ( Process_Append_mode[chamber] == 21 ) ) { // pre
						if ( Process_Append_Slot[chamber] <= 0 ) {
//							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM PRE Process Start at %s[%s]%cPROCESS_PRE_START PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9002 ); // 2015.04.23
							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM PRE Process Start at %s[%s]%cPROCESS_PRE_START PM%d:::%s:::401\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) ); // 2015.04.23
						}
						else {
//							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Pre Process Start at %s(S%d)[%s]%cPROCESS_PRE_START PM%d:S%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9003 ); // 2015.04.23
							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Pre Process Start at %s(S%d)[%s]%cPROCESS_PRE_START PM%d::S%d:%s:::402\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) ); // 2015.04.23
						}
					}
					else {
						if ( Process_Append_Slot[chamber] <= 0 ) {
//							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM POST Process Start at %s[%s]%cPROCESS_POST_START PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9004 ); // 2015.04.23
							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM POST Process Start at %s[%s]%cPROCESS_POST_START PM%d:::%s:::401\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) ); // 2015.04.23
						}
						else {
//							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Post Process Start at %s(S%d)[%s]%cPROCESS_POST_START PM%d:S%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9005 ); // 2015.04.23
							_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Post Process Start at %s(S%d)[%s]%cPROCESS_POST_START PM%d::S%d:%s:::402\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , Process_Append_Slot[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) ); // 2015.04.23
						}
					}
				}
				//-----------------------------------------------------------------------------------------------------------------------------------
				if ( ( Process_Append_mode[chamber] == 2 ) || ( Process_Append_mode[chamber] == 11 ) || ( Process_Append_mode[chamber] == 21 ) ) { // pre
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , chamber );
				}
				else {
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , chamber );
				}
				//-----------------------------------------------------------------------------------------------------------------------------------
			}
			//----------------------------------------------------------------------------
//			PROCESS_MONITOR_Setting1( chamber , Process_Append_status[chamber] , Process_Append_Side[chamber] , Process_Append_Pointer[chamber] , Process_Append_StartChamber[chamber] , Process_Append_no1[chamber] , Process_Append_no2[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , Process_Append_last[chamber] , Process_Append_append[chamber] ); // 2014.02.13
			PROCESS_MONITOR_Setting1( chamber , 0 , 0 , Process_Append_status[chamber] , Process_Append_Side[chamber] , Process_Append_Pointer[chamber] , Process_Append_StartChamber[chamber] , Process_Append_no1[chamber] , Process_Append_no2[chamber] , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , Process_Append_last[chamber] , Process_Append_append[chamber] , FALSE ); // 2014.02.13
			//----------------------------------------------------------------------------
			if ( side != Process_Append_Side[chamber] ) PM_PROCESS_MONITOR[ side ][ chamber ] = 0;
			//----------------------------------------------------------------------------
			PM_PROCESS_MONITOR_FINISH_CONFIRM[ chamber ] = TRUE;
			//----------------------------------------------------------------------------
			Process_Append_Side[chamber] = -1;
			//
		}
	}
	else if ( status == 0 ) {
		//
		if ( Process_Append_Side[chamber] == -1 ) {
			PROCESS_MONITOR_Set_Status_Sub( side , chamber , 0 , 0 , status , 0 , FALSE , 0 , -1 ,
				-1 , -1 , -1 , FALSE , FALSE ,
				0 );
			//
		}
		else {
			//
			if ( _i_SCH_EQ_SPAWN_PROCESS(
				Process_Append_Side[chamber] ,
				Process_Append_Pointer[chamber] ,
				chamber ,
				Process_Append_StartChamber[chamber] ,
				Process_Append_Slot[chamber] ,
				Process_Append_Finger[chamber] ,
				STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) ,
				Process_Append_mode[chamber] ,
				Process_Append_PutTime[chamber] ,
				STR_MEM_GET_STR( Process_Append_NextPM[chamber] ) ,
				Process_Append_MinTime[chamber] ,
				Process_Append_index[chamber] ) ) {
				//----------------------------------------------------------------------------
				LeaveCriticalSection( &CR_PM_MULTI_PROCESS[chamber] ); 
				//----------------------------------------------------------------------------
				return TRUE;
				//
			}
			else {
				if ( Process_Append_mode[chamber] == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM Process Fail at %s[%s]%cPROCESS_FAIL PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9013 );
				}
				else {
					if ( ( Process_Append_mode[chamber] == 2 ) || ( Process_Append_mode[chamber] == 11 ) || ( Process_Append_mode[chamber] == 21 ) ) { // pre
						_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM PRE Process Fail at %s[%s]%cPROCESS_PRE_FAIL PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9012 );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( Process_Append_Side[chamber] , "PM POST Process Fail at %s[%s]%cPROCESS_POST_FAIL PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chamber ) , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9 , chamber - PM1 + 1 , STR_MEM_GET_STR( Process_Append_Recipe[chamber] ) , 9013 );
					}
				}
				//----------------------------------------------------------------------------
				PROCESS_MONITOR_Set_Status_Sub( Process_Append_Side[chamber] , chamber , 0 , 0 , -1 , 0 , FALSE , 0 , -1 ,
					-1 , -1 , -1 , FALSE , FALSE ,
					0 );
				//----------------------------------------------------------------------------
				if ( side != Process_Append_Side[chamber] ) PM_PROCESS_MONITOR[ side ][ chamber ] = -1;
				//----------------------------------------------------------------------------
				//
				Process_Append_Side[chamber] = -1;
				//
			}
			//
		}
	}
	else {
		//
		Process_Append_Side[chamber] = -1;
		//
		PROCESS_MONITOR_Set_Status_Sub( side , chamber , 0 , 0 , status , 0 , FALSE , 0 , -1 ,
			-1 , -1 , -1 , FALSE , FALSE ,
			0 );
	}
	//
	LeaveCriticalSection( &CR_PM_MULTI_PROCESS[chamber] ); 
	//
	return FALSE;
}
//===================================================================================================
int  PROCESS_MONITOR_Get_Status( int side , int chamber ) {
	return PM_PROCESS_MONITOR[ side ][ chamber ];
}
//===================================================================================================
int  PROCESS_MONITOR_Get_Last_Cluster_CPName_Side( int chamber ) { // 2007.08.27
	return PM_LAST_PROCESS_JOB_SIDE[ chamber ];
}
//===================================================================================================
char *PROCESS_MONITOR_Get_Last_Cluster_CPName_Status( int chamber ) { // 2007.02.22
	return PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ];
}

//===================================================================================================
BOOL PROCESS_MONITOR_Set_Last_ProcessJob_Data( int chamber , int side , int pointer ) { // 2011.07.18
	PM_LAST_PROCESS_JOB_SIDE[ chamber ] = side;
	//
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] == NULL ) {
//		PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] = calloc( 128 + 1 , sizeof( char ) ); // 2014.01.30
		PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] = calloc( 256 + 1 , sizeof( char ) ); // 2014.01.30
	}
	//
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] != NULL ) {
//		SCHMULTI_MESSAGE_GET_ALL_SUB( side , pointer , PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] , 128 , 1 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL_SUB( side , pointer , PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] , 256 , 1 ); // 2014.01.30
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL PROCESS_MONITOR_Set_Last_ProcessJob_PPID( int chamber , int side , char *ppid ) { // 2011.07.18
	PM_LAST_PROCESS_JOB_SIDE[ chamber ] = side;
	//
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] == NULL ) {
//		PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] = calloc( 128 + 1 , sizeof( char ) ); // 2014.01.30
		PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] = calloc( 256 + 1 , sizeof( char ) ); // 2014.01.30
	}
	//
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] != NULL ) {
//		strncpy( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] , ppid , 128 ); // 2014.01.30
		strncpy( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] , ppid , 256 ); // 2014.01.30
	}
	else {
		return FALSE;
	}
	return TRUE;
}


void PROCESS_MONITOR_Reset_Last_ProcessJob_Data( int chamber ) { // 2011.07.18
	PM_LAST_PROCESS_JOB_SIDE[ chamber ] = -1;
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] != NULL ) {
		free( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] );
		PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] = NULL;
	}
}

//===================================================================================================
BOOL PROCESS_MONITOR_Last_And_Current_CPName_is_Different( int side , int pointer , int chamber ) { // 2007.05.28
//	char buffer[127+1]; // 2014.01.30
	char buffer[256+1]; // 2014.01.30
	if ( _i_SCH_PRM_GET_READY_MULTIJOB_MODE() == 0 ) return FALSE;
	if ( PM_LAST_PROCESS_JOB_SIDE[ chamber ] < 0 ) return FALSE; // 2007.08.27
	if ( PM_LAST_PROCESS_JOB_SIDE[ chamber ] != side ) return FALSE; // 2007.08.27
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] == NULL ) return FALSE;
	if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ][0] == 0 ) return FALSE;
	//
//	SCHMULTI_MESSAGE_GET_ALL( side , pointer , buffer , 127 ); // 2011.07.18
//	SCHMULTI_MESSAGE_GET_ALL_SUB( side , pointer , buffer , 127 , 1 ); // 2011.07.18 // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL_SUB( side , pointer , buffer , 256 , 1 ); // 2011.07.18 // 2014.01.30
	//
	if ( buffer[0] == 0 ) return FALSE;
	return !( STRCMP_L( buffer , PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ chamber ] ) );
}
//===================================================================================================
//BOOL PROCESS_MONITOR_SameSide_and_CPName_is_Different( int side1 , int pointer1 , int side2 , int pointer2 ) { // 2007.02.22 // 2014.02.03
BOOL PROCESS_MONITOR_CPName_is_Different( int side1 , int pointer1 , int side2 , int pointer2 ) { // 2007.02.22 // 2014.02.03
//	char buffer1[127+1]; // 2014.01.30
//	char buffer2[127+1]; // 2014.01.30
	char buffer1[256+1]; // 2014.01.30
	char buffer2[256+1]; // 2014.01.30
	if ( _i_SCH_PRM_GET_READY_MULTIJOB_MODE() == 0 ) return FALSE;
//	if ( side1 != side2 ) return FALSE; // 2014.02.03
//	SCHMULTI_MESSAGE_GET_ALL( side1 , pointer1 , buffer1 , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( side1 , pointer1 , buffer1 , 256 ); // 2014.01.30
	if ( buffer1[0] == 0 ) return FALSE;
//	SCHMULTI_MESSAGE_GET_ALL( side2 , pointer2 , buffer2 , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( side2 , pointer2 , buffer2 , 256 ); // 2014.01.30
	if ( buffer2[0] == 0 ) return FALSE;
	return !( STRCMP_L( buffer1 , buffer2 ) );
}
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================

void PROCESS_MONITOR_Set_Last( int side , int chamber , int status ) { PM_PROCESS_LAST[ side ][ chamber ] = status; }
int  PROCESS_MONITOR_Get_Last( int side , int chamber ) { return PM_PROCESS_LAST[ side ][ chamber ]; }

//void PROCESS_MONITOR_Set_Pointer( int side , int chamber , int pointer , int no , int no2 ) { // 2016.04.26
void PROCESS_MONITOR_Set_Pointer( int side , int chamber , int pointer , int no , int no2 , int chp , int chs ) { // 2016.04.26
	PM_PROCESS_POINTER[ side ][ chamber ] = pointer;
	PM_PROCESS_NO1[ side ][ chamber ] = no; // 2002.03.25
	PM_PROCESS_NO2[ side ][ chamber ] = no2; // 2002.03.25
	PM_PROCESS_CHP[ side ][ chamber ] = chp; // 2016.04.26
	PM_PROCESS_CHS[ side ][ chamber ] = chs; // 2016.04.26
}
int  PROCESS_MONITOR_Get_Pointer( int side , int chamber ) {
	return PM_PROCESS_POINTER[ side ][ chamber ];
}
int  PROCESS_MONITOR_Get_No1( int side , int chamber ) { // 2002.03.25
	return PM_PROCESS_NO1[ side ][ chamber ];
}
int  PROCESS_MONITOR_Get_No2( int side , int chamber ) { // 2002.03.25
	return PM_PROCESS_NO2[ side ][ chamber ];
}

int  PROCESS_MONITOR_Get_ChP( int side , int chamber ) { // 2016.04.26
	return PM_PROCESS_CHP[ side ][ chamber ];
}

int  PROCESS_MONITOR_Get_ChS( int side , int chamber ) { // 2016.04.26
	return PM_PROCESS_CHS[ side ][ chamber ];
}

void PROCESS_MONITOR_Set_Recipe( int side , int chamber , char *recipe ) {
// 2007.10.02 // 2007.10.18
	//
	PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] = strlen( recipe ); // 2007.01.26
	//
	if ( PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] <= 0 ) { // 2007.10.02
		if ( PM_PROCESS_RECIPE[ side ][ chamber ] == NULL ) {
			PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] = 0;
		}
		else {
			PM_PROCESS_RECIPE[ side ][ chamber ][0] = 0;
		}
	}
	else {
		if ( PM_PROCESS_RECIPE[ side ][ chamber ] == NULL ) { // 2007.01.26
			PM_PROCESS_RECIPE[ side ][ chamber ] = calloc( PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] + 1 , sizeof( char ) );
			if ( PM_PROCESS_RECIPE[ side ][ chamber ] == NULL ) {
				_IO_CIM_PRINTF( "PROCESS_MONITOR_Set_Recipe 1 Memory Allocate Error[%d,%d]\n" , side , chamber ); // 2007.10.23
				PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] = 0;
			}
			else {
				PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] = PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ];
	//			strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] ); // 2007.10.02
				strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] ); // 2007.10.02
	//			PM_PROCESS_RECIPE[ side ][ chamber ][PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ]] = 0;
				PM_PROCESS_RECIPE[ side ][ chamber ][PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ]] = 0;
			}
		}
		else {
			if ( PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] < PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] ) { // 2007.01.26
				free( PM_PROCESS_RECIPE[ side ][ chamber ] );
				PM_PROCESS_RECIPE[ side ][ chamber ] = calloc( PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] + 1 , sizeof( char ) );
				if ( PM_PROCESS_RECIPE[ side ][ chamber ] == NULL ) {
					_IO_CIM_PRINTF( "PROCESS_MONITOR_Set_Recipe 2 Memory Allocate Error[%d,%d]\n" , side , chamber ); // 2007.10.23
					PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] = 0;
				}
				else {
					PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] = PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ];
	//				strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] ); // 2007.10.02
					strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] ); // 2007.10.02
	//				PM_PROCESS_RECIPE[ side ][ chamber ][PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ]] = 0; // 2007.10.02
					PM_PROCESS_RECIPE[ side ][ chamber ][PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ]] = 0; // 2007.10.02
				}
			}
			else {
				strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ] );
				PM_PROCESS_RECIPE[ side ][ chamber ][PM_PROCESS_RECIPE_LENGTH_CURR[ side ][ chamber ]] = 0;
			}
		}
	}
//	strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , 64 ); // 2007.01.26

/*
// 2007.10.02 // 2007.10.18
	//
	PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] = strlen( recipe ); // 2007.01.26
	//
	if ( PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] <= 0 ) {
		if ( PM_PROCESS_RECIPE[ side ][ chamber ] != NULL ) {
			PM_PROCESS_RECIPE[ side ][ chamber ][0] = 0;
		}
	}
	else {
		if ( PM_PROCESS_RECIPE[ side ][ chamber ] == NULL ) { // 2007.01.26
			PM_PROCESS_RECIPE[ side ][ chamber ] = calloc( 128 + 1 , sizeof( char ) );
		}
		if ( PM_PROCESS_RECIPE[ side ][ chamber ] != NULL ) {
			if ( PM_PROCESS_RECIPE_LENGTH[ side ][ chamber ] > 128 ) {
				strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , 125 );
				PM_PROCESS_RECIPE[ side ][ chamber ][125] = 0;
				strcat( PM_PROCESS_RECIPE[ side ][ chamber ] , "..." );
				PM_PROCESS_RECIPE[ side ][ chamber ][128] = 0;
			}
			else {
				strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , 128 );
				PM_PROCESS_RECIPE[ side ][ chamber ][128] = 0;
			}
		}
	}
//	strncpy( PM_PROCESS_RECIPE[ side ][ chamber ] , recipe , 64 ); // 2007.01.26
*/
}

char *PROCESS_MONITOR_Get_Recipe( int side , int chamber ) {
	if ( PM_PROCESS_RECIPE[ side ][ chamber ] == NULL ) { // 2007.01.26
		return COMMON_BLANK_CHAR; // 2007.01.26
	} // 2007.01.26
	else { // 2007.01.26
		return( PM_PROCESS_RECIPE[ side ][ chamber ] ); // 2007.01.26
	} // 2007.01.26
//	return( PM_PROCESS_RECIPE[ side ][ chamber ] ); // 2007.01.26
}

int  PROCESS_MONITOR_Run_and_Get_Status( int chamber ) { // 2005.10.18
	int k , res = 0;
	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		if ( ( _i_SCH_SYSTEM_USING_GET( k ) > 0 ) || ( Process_Monitor_Sts_Find[k] ) ) {
			if      ( PM_PROCESS_MONITOR[ k ][ chamber ] < 0 ) {
				res = PM_PROCESS_MONITOR[ k ][ chamber ];
			}
			else if ( PM_PROCESS_MONITOR[ k ][ chamber ] > 0 ) {
				return PM_PROCESS_MONITOR[ k ][ chamber ];
			}
		}
	}
	return res;
}

int  PROCESS_MONITOR_Reset_Status( int chamber ) { // 2018.10.31
	int k , res = 0;
	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		if ( PM_PROCESS_MONITOR[ k ][ chamber ] < 0 ) {
			//
			res = PM_PROCESS_MONITOR[ k ][ chamber ];
			//
			PM_PROCESS_MONITOR[ k ][ chamber ] = 0;
			//
		}
	}
	return res;
}

void PROCESS_MONITOR_ReSet_For_Move( int tside , int tpointer , int sside , int spointer ) { // 2011.04.18
	int j , xp;
	//
	for ( j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) {
		//
		if ( PM_PROCESS_MONITOR[ sside ][ j ] <= 0 ) continue;
		if ( PM_PROCESS_POINTER[ sside ][ j ] != spointer ) continue;
		//
		EnterCriticalSection( &CR_PM_DATA_RESET1[j] );
		EnterCriticalSection( &CR_PM_DATA_RESET2[j] );
		//
		PM_PROCESS_POINTER[ tside ][ j ]		= tpointer;
		PM_PROCESS_NO1[ tside ][ j ]			= PM_PROCESS_NO1[ sside ][ j ];
		PM_PROCESS_NO2[ tside ][ j ]			= PM_PROCESS_NO2[ sside ][ j ];
		PM_PROCESS_LAST[ tside ][ j ]			= PM_PROCESS_LAST[ sside ][ j ];
		PROCESS_MONITOR_Set_Recipe( tside , j , PROCESS_MONITOR_Get_Recipe( sside , j ) );
		//
		PM_PROCESS_PATHIN[ tside ][ j ]			= PM_PROCESS_PATHIN[ sside ][ j ];
		PM_PROCESS_DUMMY[ tside ][ j ]			= PM_PROCESS_DUMMY[ sside ][ j ];
		PM_PROCESS_WFRNOTCHECK[ tside ][ j ]	= PM_PROCESS_WFRNOTCHECK[ sside ][ j ];

		for ( xp = 0 ; xp < ( MAX_PM_SLOT_DEPTH - 1 ) ; xp++ ) {
			if ( _i_SCH_MODULE_Get_PM_WAFER( j , xp + 1 ) > 0 ) {
				PM_PROCESS_SIDE_EX[ j ][xp]    = _i_SCH_MODULE_Get_PM_SIDE( j , xp+1 );
				PM_PROCESS_POINTER_EX[ j ][xp] = _i_SCH_MODULE_Get_PM_POINTER( j , xp+1 );
			}
		}
		//
		PM_PROCESS_FNOTUSE[ tside ][ j ]		= PM_PROCESS_FNOTUSE[ sside ][ j ];
		PM_PROCESS_OPTION[ tside ][ j ]			= PM_PROCESS_OPTION[ sside ][ j ];

		PM_PROCESS_MONITOR[ tside ][ j ]		= PM_PROCESS_MONITOR[ sside ][ j ];
		//
		PM_PROCESS_MONITOR[ sside ][ j ]		= 0;
		//
		PROCESSLOG_MONITOR_SIDE_CHANGE[ j ]	= tside;
		//
//		PROCESS_MAKE_SLOT_FOLDER( tside , tpointer , PROCESS_Get_SLOT_FOLDER( sside , spointer ) ); // 2012.02.18 // 2012.03.16
		PROCESS_MAKE_SLOT_FOLDER( sside , spointer , "" ); // 2012.02.18
		//
		LeaveCriticalSection( &CR_PM_DATA_RESET2[j] );
		LeaveCriticalSection( &CR_PM_DATA_RESET1[j] );
		//
	}
	//
}




//==========================================================================================================================
BOOL PROCESS_MONITOR_Get_Remain_Predict_Time( int ch , int wafer , int *tdata , int willch ) {
	int k;
	//-----------------------------------------------------------------------------------------
	if ( ch == willch ) { // 2006.03.27
		*tdata = _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) + _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , ch );
		return TRUE;
	}
	//-----------------------------------------------------------------------------------------
	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		if ( ( _i_SCH_SYSTEM_USING_GET( k ) > 0 ) || ( Process_Monitor_Sts_Find[k] ) ) {
			if ( PM_PROCESS_MONITOR[ k ][ ch ] > 0 ) {
				if ( PM_PROCESS_MONITOR[ k ][ ch ] < PROCESS_INDICATOR_PRE ) {
					*tdata = _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( 0 , ch ) + _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , ch );
				}
				else if ( ( PM_PROCESS_MONITOR[ k ][ ch ] >= PROCESS_INDICATOR_PRE ) && ( PM_PROCESS_MONITOR[ k ][ ch ] < PROCESS_INDICATOR_POST ) ) {
					*tdata = _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( 2 , ch );
				}
				else if ( PM_PROCESS_MONITOR[ k ][ ch ] >= PROCESS_INDICATOR_POST ) {
					*tdata = _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN( 1 , ch );
				}
				return TRUE;
			}
		}
	}
	if ( wafer ) {
		*tdata = _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , ch );
		return TRUE;
	}
	else {
		*tdata = 0;
	}
	return FALSE;
}
//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
//void PROCESS_MONITOR_Setting1( int ch , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , BOOL append ) { // 2006.01.12 // 2014.02.13
void PROCESS_MONITOR_Setting1( int ch , int chp , int chs , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , BOOL append , BOOL wfrnotcheck ) { // 2006.01.12 // 2014.02.13
	PROCESS_MONITOR_Set_Recipe( side , ch , recipe );
	PROCESS_MONITOR_Set_Last( side , ch , last );
	PROCESS_MONITOR_Set_Pointer( side , ch , pointer , no1 , no2 , chp , chs ); // 2016.04.26
//	PROCESS_MONITOR_Set_Status( side , ch , status , append , pathin ); // 2014.02.13
	PROCESS_MONITOR_Set_Status_Sub( side , ch , chp , chs , status , 0 , FALSE , wfrnotcheck ? 1 : 0 , pathin ,  // 2014.02.13
		-1 , -1 , -1 , FALSE , append ,
		0 );
}

//==========================================================================================================================
void PROCESS_MONITOR_Setting_with_WfrNotCheck( int ch , int chp , int chs , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last ) { // 2007.10.11
	PROCESS_MONITOR_Set_Recipe( side , ch , recipe );
	PROCESS_MONITOR_Set_Last( side , ch , last );
	PROCESS_MONITOR_Set_Pointer( side , ch , pointer , no1 , no2 , chp , chs ); // 2016.04.26
	PROCESS_MONITOR_Set_Status_Sub( side , ch , chp , chs , status , 0 , FALSE , 1 , pathin ,
		-1 , -1 , -1 , FALSE , FALSE ,
		0 );
}
//==========================================================================================================================
void PROCESS_MONITOR_Setting_with_Dummy( int ch , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , BOOL wfrnotcheck , int dummyonly ) { // 2006.01.12
	PROCESS_MONITOR_Set_Recipe( side , ch , recipe );
	PROCESS_MONITOR_Set_Last( side , ch , last );
	PROCESS_MONITOR_Set_Pointer( side , ch , pointer , no1 , no2 , 0 , 0 ); // 2016.04.26
	PROCESS_MONITOR_Set_Status_Sub( side , ch , 0 , 0 , status , ( dummyonly <= 0 ) ? 1 : 2 + ( ( dummyonly - 1 ) * 10 ) , FALSE , 1 , pathin ,
		-1 , -1 , -1 , FALSE , FALSE ,
		1 );
}
//==========================================================================================================================

void PROCESS_MONITOR_Setting2( int ch , int chp , int chs , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last ,
							  int side2 , int pointer2 , BOOL wfrnotcheck ) { // 2007.04.19
	PROCESS_MONITOR_Set_Recipe( side , ch , recipe );
	PROCESS_MONITOR_Set_Last( side , ch , last );
	PROCESS_MONITOR_Set_Pointer( side , ch , pointer , no1 , no2 , chp , chs ); // 2016.04.26
	PROCESS_MONITOR_Set_Status_Sub( side , ch , chp , chs , status % 100 , 0 , FALSE , wfrnotcheck ? 1 : 0 , pathin ,
		side2 , pointer2 , status / 100 , FALSE , FALSE ,
		2 );
}
//==========================================================================================================================
void PROCESS_MONITOR_Setting2_One( int ch , int chp , int chs , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last ,
							  int side2 , int pointer2 , BOOL wfrnotcheck ) { // 2009.09.24
	PROCESS_MONITOR_Set_Recipe( side , ch , recipe );
	PROCESS_MONITOR_Set_Last( side , ch , last );
	PROCESS_MONITOR_Set_Pointer( side , ch , pointer , no1 , no2 , chp , chs ); // 2016.04.26
	PROCESS_MONITOR_Set_Status_Sub( side , ch , chp , chs , status , 0 , FALSE , wfrnotcheck ? 3 : 2 , pathin ,
		side2 , pointer2 , status , FALSE , FALSE ,
		0 );
}
//==========================================================================================================================
//void PROCESS_MONITOR_SettingM( int ch , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , int firstnotuse ) { // 2007.05.02 // 2014.02.13
void PROCESS_MONITOR_SettingM( int ch , int status , int side , int pointer , int pathin , int no1 , int no2 , char *recipe , int last , int firstnotuse , BOOL wfrnotcheck ) { // 2007.05.02 // 2014.02.13
	PROCESS_MONITOR_Set_Recipe( side , ch , recipe );
	PROCESS_MONITOR_Set_Last( side , ch , last );
	PROCESS_MONITOR_Set_Pointer( side , ch , pointer , no1 , no2 , 0 , 0 ); // 2016.04.26
//	PROCESS_MONITOR_Set_Status_Sub( side , ch , status , ( firstnotuse == 2 ) ? 2 : 0 , ( firstnotuse == 2 ) ? 0 : firstnotuse , 0 , pathin , // 2014.02.13
	PROCESS_MONITOR_Set_Status_Sub( side , ch , 0 , 0 , status , ( firstnotuse == 2 ) ? 2 : 0 , ( firstnotuse == 2 ) ? 0 : firstnotuse , wfrnotcheck ? 1 : 0 , pathin , // 2014.02.13
		9999 , 0 , 0 , FALSE , FALSE ,
		3 );
}
//==========================================================================================================================
BOOL PROCESS_MONITOR_HAS_APPENDING( int ch ) { // 2017.09.19
	//
	EnterCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
	//
	if ( Process_Append_Side[ch] != -1 ) {
		//
		LeaveCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
		//
		return TRUE;
	}
	//
	LeaveCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
	//
	return FALSE;
	//
}

BOOL PROCESS_MONITOR_SPAWN_APPENDING( int side , int pt , int ch , int pathin , int slot , int arm , char *recipe , int mode , int puttime , char *NextPM , int MinTime , int index , int status , int no1 , int no2 , int last , BOOL append ) { // 2011.03.02
	//
	EnterCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
	//
	if ( PM_PROCESS_MONITOR[ side ][ ch ] <= 0 ) {
		//
		LeaveCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
		//
		return FALSE;
	}
	//
	if ( Process_Append_Side[ch] != -1 ) {
		//
		LeaveCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
		//
		return FALSE;
	}
	//
	Process_Append_Side[ch] = side;
	Process_Append_Pointer[ch] = pt;
	Process_Append_StartChamber[ch] = pathin;
	Process_Append_Slot[ch] = slot;
	Process_Append_Finger[ch] = arm;
	STR_MEM_MAKE_COPY2( &(Process_Append_Recipe[ch]) , recipe );
	Process_Append_mode[ch] = mode;
	Process_Append_PutTime[ch] = puttime;
	STR_MEM_MAKE_COPY2( &(Process_Append_NextPM[ch]) , NextPM );
	Process_Append_MinTime[ch] = MinTime;
	Process_Append_index[ch] = index;
	Process_Append_status[ch] = status;
	Process_Append_no1[ch] = no1;
	Process_Append_no2[ch] = no2;
	Process_Append_last[ch] = last;
	Process_Append_append[ch] = append;
	//
	LeaveCriticalSection( &CR_PM_MULTI_PROCESS[ch] ); 
	//
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PROCESS_SUB_END_OPERATION( int side , BOOL expcneck ) { // 2007.07.24
	int xp;
	if ( _i_SCH_SYSTEM_USING_RUNNING( side ) ) {
		if ( _i_SCH_SYSTEM_MODE_END_GET( side ) == 0 ) {
			_i_SCH_IO_SET_END_BUTTON( side , CTCE_IDLE );
			_i_SCH_SYSTEM_MODE_END_SET( side , 1 );
		}
	}
	if ( expcneck ) {
		for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
			if ( PR_MON_Slot_ex[xp] > 0 ) {
				if ( PR_MON_Side_ex[xp] != side ) {
					if ( _i_SCH_SYSTEM_USING_RUNNING( PR_MON_Side_ex[xp] ) ) {
						if ( _i_SCH_SYSTEM_MODE_END_GET( PR_MON_Side_ex[xp] ) == 0 ) {
							_i_SCH_IO_SET_END_BUTTON( PR_MON_Side_ex[xp] , CTCE_IDLE );
							_i_SCH_SYSTEM_MODE_END_SET( PR_MON_Side_ex[xp] , 1 );
						}
					}
				}
			}
		}
	}
}


void PROCESS_SUB_ABORT_STOP_SCENARIO_OPERATION( int side , int ch , BOOL succmode , int *finish_result ) { // 2007.07.24
	switch( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( ch ) ) {
	case STOP_LIKE_NONE :
	case STOP_LIKE_PM_ABORT_GO_NEXT :
	case STOP_LIKE_NONE_ALLABORT_REMAINPRCS : // 2012.02.08
	case STOP_LIKE_PM_ABORT_GO_NEXT_ALLABORT_REMAINPRCS : // 2012.02.08
//		PROCESS_MONITOR_Set_Finish_xStatus( side , ch , succmode ? 0 : -2 ); // 2011.03.02
//		PROCESS_MONITOR_Set_Finish_xStatus( side , ch , succmode ? -99 : -2 ); // 2011.03.02 // 2013.06.19
		*finish_result = succmode ? -99 : -2; // 2013.06.19
		break;
	case STOP_LIKE_PM_NONE_GO_OUT :
	case STOP_LIKE_PM_ABORT_GO_OUT :
	case STOP_LIKE_PM_NONE_GO_OUT_ALLABORT_REMAINPRCS : // 2012.02.08
	case STOP_LIKE_PM_ABORT_GO_OUT_ALLABORT_REMAINPRCS : // 2012.02.08
//		PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -4 ); // 2013.06.19
		*finish_result = -4; // 2013.06.19
		break;
	}
}

void PROCESS_SUB_REMAIN_RESET( int ch , int mode ) { // 2012.08.24
	int i;
//	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) { // 2013.04.29
	for ( i = 1 ; i <= (MAX_PM_SLOT_DEPTH-1) ; i++ ) { // 2013.04.29
		if ( _i_SCH_IO_GET_MODULE_STATUS( ch , i ) > 0 ) { // 2013.04.29
			if ( _i_SCH_IO_GET_MODULE_RESULT( ch , i ) == WAFERRESULT_PROCESSING ) {
				_i_SCH_IO_SET_MODULE_RESULT( ch , i , ( mode < 0 ) ? 0 : mode );
			}
		}
	}
}

void PROCESS_SUB_FAIL_RESULT_OPERATION( int side , int pointer , int slot , int ch , int chp , int chs , int result , BOOL expcneck , BOOL firstnotuse , int PathIn ) { // 2007.07.24 // 2010.05.09
	int xp;
	if ( chs > 0 ) {
		switch( result ) {
		case MRES_LIKE_FAIL		:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , 1 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , 1 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 1 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 1 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 2 ); // 2012.08.24
			//
			break;
		case MRES_LIKE_SUCCESS	:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , 0 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , 0 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 0 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 0 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 1 ); // 2012.08.24
			//
			break;
		case MRES_LIKE_NOTRUN	:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , -2 , TRUE ); // 2003.10.21 // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , -2 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -2 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -2 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 0 ); // 2012.08.24
			//
			break;
		case MRES_LIKE_SKIP		:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , -99 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , -99 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -99 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -99 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , -1 ); // 2012.08.24
			//
			break;
		}
	}
	else {
		switch( result ) {
		case MRES_LIKE_FAIL		:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , 1 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , 1 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , 1 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , 1 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 1 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 1 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 2 ); // 2012.08.24
			//
			break;
		case MRES_LIKE_SUCCESS	:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , 0 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , 0 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , 0 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , 0 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 0 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 0 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 1 ); // 2012.08.24
			//
			break;
		case MRES_LIKE_NOTRUN	:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , -2 , TRUE ); // 2003.10.21 // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , -2 , TRUE ); // 2003.10.21 // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , -2 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , -2 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -2 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -2 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 0 ); // 2012.08.24
			//
			break;
		case MRES_LIKE_SKIP		:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , -99 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , -99 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , -99 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , -99 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -99 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -99 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , -1 ); // 2012.08.24
			//
			break;
		}
	}
}

void PROCESS_SUB_FAIL_SCENARIO_LEVEL1_OPERATION( int side , int result , BOOL expcneck ) { // 2007.07.24
	switch( result ) {
	case 	FAIL_LIKE_ABORT :
	case	FAIL_LIKE_END_GO_USE :
	case	FAIL_LIKE_END_GO_DISABLE :
	case	FAIL_LIKE_END_OUT_USE :
	case	FAIL_LIKE_END_OUT_DISABLE :
	case	FAIL_LIKE_END_OUT_DISABLEHW : // 2006.06.28
	// 2003.08.09
	case	FAIL_LIKE_END_STOP_DISABLE :
	case	FAIL_LIKE_END_GO_DISABLESA :
	case	FAIL_LIKE_END_OUT_DISABLESA :
	case	FAIL_LIKE_END_STOP_DISABLESA :
	case	FAIL_LIKE_END_STOP_DISABLEHW :
	case	FAIL_LIKE_END_STOP_DISABLEHWSA :
	case	FAIL_LIKE_END_GO_DISABLE_ABORT :
	case	FAIL_LIKE_END_OUT_DISABLE_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLE_ABORT :
	case	FAIL_LIKE_END_GO_DISABLESA_ABORT :
	case	FAIL_LIKE_END_OUT_DISABLESA_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLESA_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLEHW_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLEHWSA_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLE_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLESA_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLEHW_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLEHWSA_ABORT :
			//
			PROCESS_SUB_END_OPERATION( side , TRUE );
			break;
	}
}

void PROCESS_SUB_FAIL_SCENARIO_LEVEL2_OPERATION( int side , int ch , int result , BOOL normal , int *finish_result ) { // 2007.07.24
	int i , data1 , data2 , data3 , data4 , data5 , res;
	switch( result ) {
	case 	FAIL_LIKE_ABORT :
			//---------------------------------------------------------
			if ( ( _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO() / 2 ) == 1 ) {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_ABORT , 0 );
			}
			*finish_result = -1; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_GO_USE :
	case	FAIL_LIKE_END_GO_USE :
			//---------------------------------------------------------
			*finish_result = -2; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -2 ); // 2013.06.19
			//---------------------------------------------------------
			break;

	case	FAIL_LIKE_CONT_GO_DISABLE :
	case	FAIL_LIKE_END_GO_DISABLE :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
			*finish_result = -3; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_OUT_USE :
	case	FAIL_LIKE_END_OUT_USE :
			//---------------------------------------------------------
			*finish_result = -4; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -4 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_OUT_DISABLE :
	case	FAIL_LIKE_END_OUT_DISABLE :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -5; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLE :
	case	FAIL_LIKE_END_STOP_DISABLE :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , normal ? ABORTWAIT_FLAG_WAIT : ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -3; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_GO_DISABLESA :
	case	FAIL_LIKE_END_GO_DISABLESA :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , normal ? ABORTWAIT_FLAG_ABORT : ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -3; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_OUT_DISABLESA :
	case	FAIL_LIKE_END_OUT_DISABLESA :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , normal ? ABORTWAIT_FLAG_ABORT : ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -5; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLESA :
	case	FAIL_LIKE_END_STOP_DISABLESA :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , normal ? ABORTWAIT_FLAG_ABORTWAIT : ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -3; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLEHW :
	case	FAIL_LIKE_END_STOP_DISABLEHW :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 0 , normal ? ABORTWAIT_FLAG_WAIT : ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -3; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLEHWSA :
	case	FAIL_LIKE_END_STOP_DISABLEHWSA :
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 0 , normal ? ABORTWAIT_FLAG_ABORTWAIT : ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -3; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_GO_DISABLE_ABORT :
	case	FAIL_LIKE_END_GO_DISABLE_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 3 , ABORTWAIT_FLAG_NONE , 0 ) ) {
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -3; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_OUT_DISABLE_ABORT :
	case	FAIL_LIKE_END_OUT_DISABLE_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 3 , ABORTWAIT_FLAG_NONE , 0 ) ) {
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -5; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -5; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLE_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLE_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 3 , ABORTWAIT_FLAG_WAIT , 0 ) ) {
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -3; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_GO_DISABLESA_ABORT :
	case	FAIL_LIKE_END_GO_DISABLESA_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 3 , ABORTWAIT_FLAG_ABORT , 0 ) ) {
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -3; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_OUT_DISABLESA_ABORT :
	case	FAIL_LIKE_END_OUT_DISABLESA_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 3 , ABORTWAIT_FLAG_ABORT , 0 ) ) {
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -5; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -5; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLESA_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLESA_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 3 , ABORTWAIT_FLAG_ABORTWAIT , 0 ) ) {
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_ABORT , 0 );
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -3; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLEHW_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLEHW_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 3 , ABORTWAIT_FLAG_WAIT , 0 ) ) {
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -3; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_STOP_DISABLEHWSA_ABORT :
	case	FAIL_LIKE_END_STOP_DISABLEHWSA_ABORT :
			//---------------------------------------------------------
			if ( normal ) {
				if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 3 , ABORTWAIT_FLAG_ABORTWAIT , 0 ) ) {
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_ABORT , 0 );
					*finish_result = -1; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
				}
				else {
					*finish_result = -3; // 2013.06.19
//					PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
				}
			}
			else {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_NOTUSE_DISABLE_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLE_ABORT :
			//---------------------------------------------------------
			if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 4 , ABORTWAIT_FLAG_WAIT , 0 ) ) {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				if ( normal ) *finish_result = -1; // 2013.06.19
				else          *finish_result = -3; // 2013.06.19
//				if ( normal ) PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
//				else          PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			else {
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_NOTUSE_DISABLESA_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLESA_ABORT :
			//---------------------------------------------------------
			if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 0 , 4 , ABORTWAIT_FLAG_ABORTWAIT , 0 ) ) {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_ABORT , 0 );
				if ( normal ) *finish_result = -1; // 2013.06.19
				else          *finish_result = -3; // 2013.06.19
//				if ( normal ) PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
//				else          PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			else {
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_NOTUSE_DISABLEHW_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLEHW_ABORT :
			//---------------------------------------------------------
			if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 4 , ABORTWAIT_FLAG_WAIT , 0 ) ) {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
				if ( normal ) *finish_result = -1; // 2013.06.19
				else          *finish_result = -3; // 2013.06.19
//				if ( normal ) PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
//				else          PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			else {
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_NOTUSE_DISABLEHWSA_ABORT :
	case	FAIL_LIKE_END_NOTUSE_DISABLEHWSA_ABORT :
			//---------------------------------------------------------
			if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 4 , ABORTWAIT_FLAG_ABORTWAIT , 0 ) ) {
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , -1 , 0 , ABORTWAIT_FLAG_ABORT , 0 );
				if ( normal ) *finish_result = -1; // 2013.06.19
				else          *finish_result = -3; // 2013.06.19
//				if ( normal ) PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -1 ); // 2013.06.19
//				else          PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			else {
				*finish_result = -3; // 2013.06.19
//				PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -3 ); // 2013.06.19
			}
			//---------------------------------------------------------
			break;
	case	FAIL_LIKE_CONT_OUT_DISABLEHW : // 2006.06.28
	case	FAIL_LIKE_END_OUT_DISABLEHW : // 2006.06.28
			//---------------------------------------------------------
			SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , 1 , 0 , ABORTWAIT_FLAG_NONE , 0 );
			//---------------------------------------------------------
			*finish_result = -5; // 2013.06.19
//			PROCESS_MONITOR_Set_Finish_xStatus( side , ch , -5 ); // 2013.06.19
			//---------------------------------------------------------
			break;

	//==============================================================================================================================
	// User Area
	//==============================================================================================================================
	case	FAIL_LIKE_USER_STYLE_1 : // 2006.02.03
			res = FALSE;
			for ( i = 0 ; i < 5 ; i++ ) {
				data1 = 0;
				data2 = 0;
				data3 = 0;
				data4 = 0;
				data5 = FALSE;
				switch ( _SCH_COMMON_FAIL_SCENARIO_OPERATION( side , ch , normal , i , res , &data1 , &data2 , &data3 , &data4 , &data5 ) ) {
				case 1 : // Dis+prset
					res = SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , data1 , data2 , data3 , 0 );
//					if ( data4 <= 0 ) PROCESS_MONITOR_Set_Finish_xStatus( side , ch , data4 ); // 2013.06.19
					if ( data4 <= 0 ) *finish_result = data4; // 2013.06.19
					break;
				case 2 : // Dis
					res = SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( ch , data1 , data2 , data3 , 0 );
					break;
				case 3 : // prset
					res = FALSE;
//					if ( data4 <= 0 ) PROCESS_MONITOR_Set_Finish_xStatus( side , ch , data4 ); // 2013.06.19
					if ( data4 <= 0 ) *finish_result = data4; // 2013.06.19
					break;
				default :
					res = FALSE;
					break;
				}
				if ( !data5 ) break;
			}
			break;
	}
}

void PROCESS_SUB_SUCCESS_RESULT_OPERATION( int side , int pointer , int slot , int ch , int chp , int chs , int result , BOOL expcneck , BOOL firstnotuse , int PathIn ) { // 2007.07.24 // 2010.05.09
	int xp;
	if ( chs > 0 ) { // 2016.04.26
		switch( result ) {
		case MRES_SUCCESS_LIKE_SUCCESS	:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , 0 , FALSE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , 0 , FALSE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 0 , FALSE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 0 , FALSE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 1 ); // 2012.08.24
			//
			break;
		case MRES_SUCCESS_LIKE_NOTRUN	:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , -2 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , -2 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -2 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -2 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 0 ); // 2012.08.24
			//
			break;
		case MRES_SUCCESS_LIKE_SKIP		:
		case MRES_SUCCESS_LIKE_ALLSKIP	:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , -99 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , -99 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -99 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -99 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , -1 ); // 2012.08.24
			//
			break;
		case MRES_SUCCESS_LIKE_FAIL		:
			if ( !firstnotuse ) {
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , chp , chs , 1 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , chp , chs , 1 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 1 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 1 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 2 ); // 2012.08.24
			//
			break;
		}
	}
	else {
		switch( result ) {
		case MRES_SUCCESS_LIKE_SUCCESS	:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , 0 , FALSE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , 0 , FALSE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , 0 , FALSE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , 0 , FALSE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 0 , FALSE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 0 , FALSE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 1 ); // 2012.08.24
			//
			break;
		case MRES_SUCCESS_LIKE_NOTRUN	:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , -2 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , -2 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , -2 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , -2 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -2 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -2 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 0 ); // 2012.08.24
			//
			break;
		case MRES_SUCCESS_LIKE_SKIP		:
		case MRES_SUCCESS_LIKE_ALLSKIP	:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , -99 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , -99 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , -99 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , -99 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , -99 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , -99 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , -1 ); // 2012.08.24
			//
			break;
		case MRES_SUCCESS_LIKE_FAIL		:
			if ( !firstnotuse ) {
	//			SCHEDULER_CASSETTE_LAST_RESULT_SET( side , PathIn , slot , ch , 1 , 1 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT_SET( side , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , _i_SCH_CLUSTER_Get_SlotIn( side , pointer ) , ch , 1 , 1 , TRUE ); // 2015.04.29
				SCHEDULER_CASSETTE_LAST_RESULT2_SET( side , pointer , ch , 1 , 1 , TRUE ); // 2011.04.27
			}
			if ( expcneck ) {
				for ( xp = 0 ; xp < (MAX_PM_SLOT_DEPTH-1) ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
	//					SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , PR_MON_Slot_ex[xp] , ch , xp + 2 , 1 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT_SET( PR_MON_Side_ex[xp] , _i_SCH_CLUSTER_Get_PathIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , _i_SCH_CLUSTER_Get_SlotIn( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] ) , ch , xp + 2 , 1 , TRUE ); // 2015.04.29
						SCHEDULER_CASSETTE_LAST_RESULT2_SET( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , ch , xp + 2 , 1 , TRUE ); // 2011.04.27
					}
				}
			}
			//
			PROCESS_SUB_REMAIN_RESET( ch , 2 ); // 2012.08.24
			//
			break;
		}
	}
}

void PROCESS_SUB_UPLOAD_STATUS_OPERATION( int mode , int rm , int k , int j , int *fail_scenario , int *success_result , int *fail_result , int *abort_result , BOOL expcneck , BOOL firstnotuse ) { // 2007.07.24
	int xp , fo , finishout , dispr , pre_time , prcs_time , post_time;
	int l;
	switch( EQUIP_STATUS_PROCESS_OF_OPTION( k , rm , &fo , &finishout , &dispr , fail_scenario , success_result , fail_result , abort_result , &pre_time , &prcs_time , &post_time ) ) {
	case PRCS_RET_OPTION_NONE :
		break;
	case PRCS_RET_OPTION_POSTSKIP :
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			//=======================================================================================
			if ( !firstnotuse ) _i_SCH_CLUSTER_Set_PathProcessData_SkipPost( k , PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_No1( k , j ) , PROCESS_MONITOR_Get_No2( k , j ) );
			//=======================================================================================
			if ( expcneck ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						_i_SCH_CLUSTER_Set_PathProcessData_SkipPost( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , PROCESS_MONITOR_Get_No1( k , j ) , PROCESS_MONITOR_Get_No2( k , j ) );
					}
				}
			}
			//=======================================================================================
			if ( !firstnotuse ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
					//-------------------------------------------------------------------------------------
					if ( ( ( j - PM1 ) % 2 ) == 0 )
						xp = j + 1;
					else
						xp = j - 1;
					//-------------------------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathProcessData_SkipPost2( k , PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_No1( k , j ) , xp );
				}
			}
			//=======================================================================================
		}
		break;
	case PRCS_RET_OPTION_PRESKIP :
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			//=================================================================================
			if ( !firstnotuse ) _i_SCH_CLUSTER_Set_PathProcessData_SkipPre( k , j );
			//=================================================================================
			if ( expcneck ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						if ( PR_MON_Side_ex[xp] != k ) {
							_i_SCH_CLUSTER_Set_PathProcessData_SkipPre( PR_MON_Side_ex[xp] , j );
						}
					}
				}
			}
			//====================================================================================================================================
		}
		break;
	case PRCS_RET_OPTION_POSTPRESKIP :
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			//=================================================================================
			if ( !firstnotuse ) _i_SCH_CLUSTER_Set_PathProcessData_SkipPost( k , PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_No1( k , j ) , PROCESS_MONITOR_Get_No2( k , j ) );
			//=================================================================================
			if ( expcneck ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						_i_SCH_CLUSTER_Set_PathProcessData_SkipPost( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , PROCESS_MONITOR_Get_No1( k , j ) , PROCESS_MONITOR_Get_No2( k , j ) );
					}
				}
			}
			//=======================================================================================
			if ( !firstnotuse ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
					//-------------------------------------------------------------------------------------
					if ( ( ( j - PM1 ) % 2 ) == 0 )
						xp = j + 1;
					else
						xp = j - 1;
					//-------------------------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathProcessData_SkipPost2( k , PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_No1( k , j ) , xp );
				}
			}
			//=======================================================================================
		}
		if ( ( mode == 0 ) || ( mode == 2 ) ) {
			//=======================================================================================
			if ( !firstnotuse ) _i_SCH_CLUSTER_Set_PathProcessData_SkipPre( k , j );
			//=================================================================================
			if ( expcneck ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						if ( PR_MON_Side_ex[xp] != k ) {
							_i_SCH_CLUSTER_Set_PathProcessData_SkipPre( PR_MON_Side_ex[xp] , j );
						}
					}
				}
			}
			//====================================================================================================================================
		}
		break;
	}
	//====================================================================================
	if ( mode == 0 ) {
		if ( fo ) {
			if ( !firstnotuse ) _i_SCH_CLUSTER_Set_FailOut( k , PROCESS_MONITOR_Get_Pointer( k , j ) , 1 );
			//=================================================================================
			if ( expcneck ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						_i_SCH_CLUSTER_Set_FailOut( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , 1 );
					}
				}
			}
			//====================================================================================================================================
		}
		//====================================================================================
		if ( ( finishout >= 1 ) && ( finishout <= 4 ) ) { // All,Out,ResetAll,Reset
			if ( !firstnotuse ) _i_SCH_CLUSTER_Set_FailOut( k , PROCESS_MONITOR_Get_Pointer( k , j ) , finishout + 1 );
			//=================================================================================
			if ( expcneck ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
					if ( PR_MON_Slot_ex[xp] > 0 ) {
						_i_SCH_CLUSTER_Set_FailOut( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , finishout + 1 );
					}
				}
			}
		}
		//====================================================================================
		if ( !firstnotuse ) {
			if ( dispr ) {
				/*
				// 2014.01.10
				if ( _i_SCH_MODULE_Get_PM_WAFER( j , 0 ) > 0 ) {
					//
					_i_SCH_IO_SET_MODULE_STATUS( j , 1 , 0 );
					//
					_i_SCH_MODULE_Set_PM_WAFER( j , 0 , 0 );
					//
					if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
						_i_SCH_MODULE_Inc_FM_InCount( k );
					}
					//
					_i_SCH_MODULE_Inc_TM_InCount( k );
					//=============================================================================
					SCHEDULER_CONTROL_Set_Change_Disappear_Status( _i_SCH_MODULE_Get_PM_SIDE( j , 0 ) , _i_SCH_MODULE_Get_PM_POINTER( j , 0 ) );
					//=============================================================================
					//-----------------------------------------------------------
					_i_SCH_LOG_LOT_PRINTF( k , "PM%d Disappear at %d%cDISAPPEAR PM%d:%d%c%d\n" , j - PM1 + 1 , _i_SCH_MODULE_Get_PM_WAFER( j , 0 ) , 9 , j - PM1 + 1 , _i_SCH_MODULE_Get_PM_WAFER( j , 0 ) , 9 , _i_SCH_MODULE_Get_PM_WAFER( j , 0 ) );
					//-----------------------------------------------------------
//					_i_SCH_LOG_TIMER_PRINTF( k , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( k , PROCESS_MONITOR_Get_Pointer( k , j ) ) , _i_SCH_CLUSTER_Get_PathIn( k , PROCESS_MONITOR_Get_Pointer( k , j ) ) , -1 , -1 , -1 , "" , "" ); // 2012.02.18
					_i_SCH_LOG_TIMER_PRINTF( k , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( k , PROCESS_MONITOR_Get_Pointer( k , j ) ) , _i_SCH_CLUSTER_Get_PathIn( k , PROCESS_MONITOR_Get_Pointer( k , j ) ) , PROCESS_MONITOR_Get_Pointer( k , j ) , -1 , -1 , "" , "" ); // 2012.02.18
				}
				*/
				//
				// 2014.01.10
				//
				for ( l = 0 ; l < _i_SCH_PRM_GET_MODULE_SIZE( j ) ; l++ ) {
					if ( _i_SCH_MODULE_Get_PM_WAFER( j , l ) > 0 ) {
						//
						_i_SCH_IO_SET_MODULE_STATUS( j , l+1 , 0 );
						//
						_i_SCH_MODULE_Set_PM_WAFER( j , l , 0 );
						//
						if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
							_i_SCH_MODULE_Inc_FM_InCount( k );
						}
						//
						_i_SCH_MODULE_Inc_TM_InCount( k );
						//=============================================================================
						SCHEDULER_CONTROL_Set_Change_Disappear_Status( _i_SCH_MODULE_Get_PM_SIDE( j , l ) , _i_SCH_MODULE_Get_PM_POINTER( j , l ) );
						//=============================================================================
						//-----------------------------------------------------------
						_i_SCH_LOG_LOT_PRINTF( k , "PM%d Disappear at %d%cDISAPPEAR PM%d:%d%c%d\n" , j - PM1 + 1 , _i_SCH_MODULE_Get_PM_WAFER( j , l ) , 9 , j - PM1 + 1 , _i_SCH_MODULE_Get_PM_WAFER( j , l ) , 9 , _i_SCH_MODULE_Get_PM_WAFER( j , l ) );
						//-----------------------------------------------------------
						_i_SCH_LOG_TIMER_PRINTF( _i_SCH_MODULE_Get_PM_SIDE( j , l ) , TIMER_CM_END , _i_SCH_CLUSTER_Get_SlotIn( _i_SCH_MODULE_Get_PM_SIDE( j , l ) , _i_SCH_MODULE_Get_PM_POINTER( j , l ) ) , _i_SCH_CLUSTER_Get_PathIn( _i_SCH_MODULE_Get_PM_SIDE( j , l ) , _i_SCH_MODULE_Get_PM_POINTER( j , l ) ) , _i_SCH_MODULE_Get_PM_POINTER( j , l ) , -1 , -1 , "" , "" ); // 2012.02.18
						//
					}
				}
				//
			}
		}
		//====================================================================================
	}
	if ( pre_time >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Time Target Change at Main %s[%d]%cPROCESS_TIME PM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , pre_time , 9 , j - PM1 + 1 , pre_time );
		_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 2 , j , pre_time );
	}
	if ( prcs_time >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( k , "PM Process Time Target Change at Main %s[%d]%cPROCESS_TIME PM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , prcs_time , 9 , j - PM1 + 1 , prcs_time );
		_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 0 , j , prcs_time );
	}
	if ( post_time >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Time Target Change at Main %s[%d]%cPROCESS_TIME PM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , post_time , 9 , j - PM1 + 1 , post_time );
		_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 1 , j , post_time );
	}
}

void PROCESS_SUB_ADD_RUN_COUNT( BOOL succes , int ch , int Slot , BOOL expcneck , BOOL firstnotuse ) { // 2007.07.24
	int xp;
	if ( ( ( Slot % 100 ) > 0 ) && ( ( Slot / 100 ) > 0 ) ) {
		if ( !firstnotuse ) {
			if ( succes )	LOG_MTL_ADD_PROCESS_COUNT( ch , 2 );
			else			LOG_MTL_ADD_FAIL_COUNT( ch , 2 );
		}
	}
	else {
		if ( !firstnotuse ) {
			if ( succes )	LOG_MTL_ADD_PROCESS_COUNT( ch , 1 );
			else			LOG_MTL_ADD_FAIL_COUNT( ch , 1 );
		}
		if ( expcneck ) {
			for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
				if ( PR_MON_Slot_ex[xp] > 0 ) {
					if ( succes )	LOG_MTL_ADD_PROCESS_COUNT( ch , 1 );
					else			LOG_MTL_ADD_FAIL_COUNT( ch , 1 );
				}
			}
		}
	}
}

void PROCESS_SUB_NORMAL_BM_END_MESSAGE( int side , int ch , int Result ) { // 2007.12.28
	int wfr;
	char Buffer[16];
	//
	wfr = PROCESS_MONITOR_Get_No1( side , ch ); // 2011.06.15
	//
	if ( wfr <= 0 ) {
		sprintf( Buffer , "" );
	}
	else {
		sprintf( Buffer , "\t%d" , wfr );
	}
	//
	if      ( ( PROCESS_MONITOR_Get_No2( side , ch ) % 100 ) == 0 ) { // fm
		switch( PROCESS_MONITOR_Get_Last( side , ch ) ) {
		case BUFFER_RUN_WAITING_FOR_FM :
			switch( Result ) {
			case SYS_SUCCESS :
				//
				EST_BM_FM_SIDE_END( ch ); // 2016.10.22
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , TRUE );
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s End FMSIDE%cWHFMGOFMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ERROR :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Error FMSIDE%cWHFMGOFMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ABORTED :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Abort FMSIDE%cWHFMGOFMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			default :
				return;
				break;
			}
			break;
		case BUFFER_RUN_WAITING_FOR_TM :
			switch( Result ) {
			case SYS_SUCCESS :
				//
				EST_BM_TM_SIDE_END( ch ); // 2016.10.22
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , TRUE );
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s End TMSIDE%cWHFMGOTMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ERROR :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Error TMSIDE%cWHFMGOTMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ABORTED :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Abort TMSIDE%cWHFMGOTMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			default :
				return;
				break;
			}
			break;
		}
	}
	else if ( ( PROCESS_MONITOR_Get_No2( side , ch ) % 100 ) == 1 ) { // tm
		switch( PROCESS_MONITOR_Get_Last( side , ch ) ) {
		case BUFFER_RUN_WAITING_FOR_FM :
			switch( Result ) {
			case SYS_SUCCESS :
				//
				EST_BM_FM_SIDE_END( ch ); // 2016.10.22
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , TRUE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s End FMSIDE%cWHTMGOFMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ERROR :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Error FMSIDE%cWHTMGOFMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ABORTED :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Abort FMSIDE%cWHTMGOFMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			default :
				return;
				break;
			}
			break;
		case BUFFER_RUN_WAITING_FOR_TM :
			switch( Result ) {
			case SYS_SUCCESS :
				//
				EST_BM_TM_SIDE_END( ch ); // 2016.10.22
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , TRUE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s End TMSIDE%cWHTMGOTMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ERROR :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Error TMSIDE%cWHTMGOTMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ABORTED :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Abort TMSIDE%cWHTMGOTMSIDE_END BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			default :
				return;
				break;
			}
			break;
		}
	}
	else if ( ( PROCESS_MONITOR_Get_No2( side , ch ) % 100 ) >= 2 ) { // tm
		switch( PROCESS_MONITOR_Get_Last( side , ch ) ) {
		case BUFFER_RUN_WAITING_FOR_FM :
			switch( Result ) {
			case SYS_SUCCESS :
				//
				EST_BM_FM_SIDE_END( ch ); // 2016.10.22
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , TRUE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s End FMSIDE%cWHTM%dGOFMSIDE_END BM%d:%d%s\n" , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ERROR :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Error FMSIDE%cWHTM%dGOFMSIDE_END BM%d:%d%s\n" , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ABORTED :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Abort FMSIDE%cWHTM%dGOFMSIDE_END BM%d:%d%s\n" , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			default :
				return;
				break;
			}
			break;
		case BUFFER_RUN_WAITING_FOR_TM :
			switch( Result ) {
			case SYS_SUCCESS :
				//
				EST_BM_TM_SIDE_END( ch ); // 2016.10.22
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , TRUE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s End TMSIDE%cWHTM%dGOTMSIDE_END BM%d:%d%s\n" , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ERROR :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Error TMSIDE%cWHTM%dGOTMSIDE_END BM%d:%d%s\n" , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			case SYS_ABORTED :
				_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , ch , FALSE );
				_i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Abort TMSIDE%cWHTM%dGOTMSIDE_END BM%d:%d%s\n" , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , PROCESS_MONITOR_Get_No2( side , ch ) % 100 , ch - BM1 + 1 , PROCESS_MONITOR_Get_No2( side , ch ) / 100 , Buffer );
				break;
			default :
				return;
				break;
			}
			break;
		}
	}
}

/*
// 2013.05.23
void PROCESS_SUB_NORMAL_PM_END_MESSAGE( BOOL Fnotuse , int Dummy , BOOL wfrnotcheck , int side , int pt , int ch , int Slot , int Result , int nextres ) {
	if ( Fnotuse ) return;
	if ( !wfrnotcheck ) { // 2007.10.11
		if ( _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) return;
	}
	switch( Result ) {
	case SYS_SUCCESS :
		if      ( ( Dummy % 10 ) == 1 ) {
			_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:%d:%s:%d:51%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot % 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26
			_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d)[%s][%d]%cPROCESS_END PM%d:D%d:%s:%d:52%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26
		}
		else if ( ( Dummy % 10 ) == 2 ) {
//			_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d)[%s][%d]%cPROCESS_END PM%d:D%d:%s:50%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , 9 , Slot / 100 ); // 2009.03.09
			//
			if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
			//
			if ( ( Dummy / 10 ) == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d)[%s][%d]%cPROCESS_END PM%d:D%d:%s:%d:53%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2009.03.09 // 2012.09.26
			}
			else { // 2009.03.09
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d-%d)[%s][%d]%cPROCESS_END PM%d:D%d-%d:%s:%d:54%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , ( Dummy / 10 ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , ( Dummy / 10 ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , ( Dummy / 10 ) , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2009.03.09 // 2012.09.26
			}
		}
		else {
//			_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:%d:%s:%d:56%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2013.05.06
			//
			// 2013.05.06
			//
			if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:%d:%s:%d:55%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch + 1 - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
			}
			else {
				if ( ( Slot / 100 ) > 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:%d:%s:%d:56%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
				}
				if ( ( Slot % 100 ) > 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:%d:%s:%d:57%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot % 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
				}
			}
		}
		break;
	case SYS_ERROR :
		if      ( ( Dummy % 10 ) == 1 ) {
			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s:%d:61%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot % 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26
			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d)[%s][%d]%cPROCESS_ERROR PM%d:D%d:%s:%d:62%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26
		}
		else if ( ( Dummy % 10 ) == 2 ) {
//			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d)[%s][%d]%cPROCESS_ERROR PM%d:D%d:%s:50%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , 9 , Slot / 100 ); // 2009.03.09
			//
			if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
			//
			if ( ( Dummy / 10 ) == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d)[%s][%d]%cPROCESS_ERROR PM%d:D%d:%s:%d:63%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2009.03.09 // 2012.09.26
			}
			else { // 2009.03.09
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d-%d)[%s][%d]%cPROCESS_ERROR PM%d:D%d-%d:%s:%d:64%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , ( Dummy / 10 ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , ( Dummy / 10 ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , ( Dummy / 10 ) , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2009.03.09 // 2012.09.26
			}
		}
		else {
//			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s:%d:66%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26 // 2013.05.06
			//
			// 2013.05.06
			//
			if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s:%d:65%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch + 1 - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
			}
			else {
				if ( ( Slot / 100 ) > 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s:%d:66%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
				}
				if ( ( Slot % 100 ) > 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s:%d:67%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot % 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
				}
			}
		}
		break;
	case SYS_ABORTED :
		if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 0 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 4 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 8 ) ) {
			if      ( ( Dummy % 10 ) == 1 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s:%d:71%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot % 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d)[%s][%d]%cPROCESS_ABORT PM%d:D%d:%s:%d:72%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26
			}
			else if ( ( Dummy % 10 ) == 2 ) {
//				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d)[%s][%d]%cPROCESS_ABORT PM%d:D%d:%s:50%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , 9 , Slot / 100 ); // 2009.03.09
				//
				if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
				//
				if ( ( Dummy / 10 ) == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d)[%s][%d]%cPROCESS_ABORT PM%d:D%d:%s:%d:73%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2009.03.09
				}
				else { // 2009.03.09
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d-%d)[%s][%d]%cPROCESS_ABORT PM%d:D%d-%d:%s:%d:74%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , ( Dummy / 10 ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , ( Dummy / 10 ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , ( Dummy / 10 ) , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2009.03.09 // 2012.09.26
				}
			}
			else {
//				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s:%d:76%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot , FA_SIDE_TO_SLOT_GETxS( side , pt ) ); // 2012.09.26 // 2013.05.06
				//
				// 2013.05.06
				//
				if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s:%d:75%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch + 1 - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
				}
				else {
					if ( ( Slot / 100 ) > 0 ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s:%d:76%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot / 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
					}
					if ( ( Slot % 100 ) > 0 ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s:%d:77%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Recipe(side,ch) , nextres , 9 , Slot % 100 , FA_SIDE_TO_SLOT_GETxS( side , pt ) );
					}
				}
			}
		}
		break;
	default :
		return;
		break;
	}

// 2007.11.07
//	if ( Dummy != 0 ) { // 2009.03.18
	if ( ( Dummy % 10 ) != 0 ) { // 2009.03.18
//		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? ( Dummy*1000 ) + 0 : ( Dummy*1000 ) + 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" ); // 2009.03.18
//		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? ( ( Dummy % 10 )*1000 ) + 0 : ( ( Dummy % 10 )*1000 ) + 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" ); // 2009.03.18 // 2010.11.30
		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? ( Dummy * 1000 ) + 0 : ( Dummy * 1000 ) + 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" ); // 2009.03.18 // 2010.11.30
	}
	else {
//		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? 0 : 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" ); // 2013.05.06
		if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch + 1 - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? 0 : 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" );
		}
		else {
			if ( ( Slot / 100 ) > 0 ) {
				_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot / 100 , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? 0 : 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" );
			}
			if ( ( Slot % 100 ) > 0 ) {
				_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot % 100 , PROCESS_MONITOR_Get_Pointer(side,ch) , ( Result == SYS_SUCCESS ) ? 0 : 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" );
			}
		}
	}
}
*/


// 2013.05.23
//void PROCESS_SUB_NORMAL_PM_END_MESSAGE( BOOL Fnotuse , int Dummy , BOOL wfrnotcheck , int side , int pt , int ch , int Slot , char *recipe , int Last , int Result , int nextres ) { // 2016.04.26
void PROCESS_SUB_NORMAL_PM_END_MESSAGE( BOOL Fnotuse , int Dummy , BOOL wfrnotcheck , int side , int pt , int ch , int chp , int chs , int Slot , char *recipe , int Last , int Result , int nextres ) { // 2016.04.26
	char MsgSltchar[16]; /* 2013.05.23 */
	if ( Fnotuse ) return;
	if ( !wfrnotcheck ) { // 2007.10.11
		if ( chs > 0 ) { // 2016.04.26
			if ( _i_SCH_MODULE_Get_PM_WAFER( chp , chs ) <= 0 ) return;
		}
		else {
			if ( _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) return;
		}
	}
	//
	FA_SIDE_TO_SLOT_GET_L( side , pt , MsgSltchar );
	//
	if ( chs > 0 ) { // 2016.04.26
		switch( Result ) {
		case SYS_SUCCESS :
			//
			EST_PM_PROCESS_END( ch  ); // 2016.10.22
			//
			if      ( ( Dummy % 10 ) == 1 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:51%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar ); // 2012.09.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(D%d)[%s][%d]%cPROCESS_END PM%d:%d:D%d:%s:%d:%d:52%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar ); // 2012.09.26
			}
			else if ( ( Dummy % 10 ) == 2 ) {
				//
				if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
				//
				if ( ( Dummy / 10 ) == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(D%d)[%s][%d]%cPROCESS_END PM%d:%d:D%d:%s:%d:%d:53%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot , recipe , pt , nextres , 9 , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
				else { // 2009.03.09
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(D%d-%d)[%s][%d]%cPROCESS_END PM%d:%d:D%d-%d:%s:%d:%d:54%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , ( Dummy / 10 ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , ( Dummy / 10 ) , Slot , recipe , pt , nextres , 9 , ( Dummy / 10 ) , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
			}
			else {
				//
				// 2013.05.06
				//
				if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
					if ( side == PR_MON_Side_ex[0] ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:58%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , pt , nextres , 9 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( side              , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:59%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot              , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot              , recipe , pt , nextres , 9 , Slot              , MsgSltchar );
						_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[0] , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:59%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , PR_MON_Slot_ex[0] , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , PR_MON_Slot_ex[0] , recipe , pt , nextres , 9 , PR_MON_Slot_ex[0] , MsgSltchar );
					}
				}
				else {
					if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:55%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp + 1 ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp + 1 - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
					}
					else {
						if ( ( Slot / 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:56%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
						}
						if ( ( Slot % 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s:%d(%d)[%s][%d]%cPROCESS_END PM%d:%d:%d:%s:%d:%d:57%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar );
						}
					}
				}
				//
			}
			break;
		case SYS_ERROR :
			if      ( ( Dummy % 10 ) == 1 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:61%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar ); // 2012.09.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(D%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:D%d:%s:%d:%d:62%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar ); // 2012.09.26
			}
			else if ( ( Dummy % 10 ) == 2 ) {
				//
				if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
				//
				if ( ( Dummy / 10 ) == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(D%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:D%d:%s:%d:%d:63%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot , recipe , pt , nextres , 9 , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
				else { // 2009.03.09
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(D%d-%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:D%d-%d:%s:%d:%d:64%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , ( Dummy / 10 ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , ( Dummy / 10 ) , Slot , recipe , pt , nextres , 9 , ( Dummy / 10 ) , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
			}
			else {
				//
				// 2013.05.06
				//
				//
				if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
					if ( side == PR_MON_Side_ex[0] ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:68%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , pt , nextres , 9 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( side              , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot              , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot              , recipe , pt , nextres , 9 , Slot              , MsgSltchar );
						_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[0] , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , PR_MON_Slot_ex[0] , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , PR_MON_Slot_ex[0] , recipe , pt , nextres , 9 , PR_MON_Slot_ex[0] , MsgSltchar );
					}
				}
				else {
					if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:65%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp + 1 ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp + 1 - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
					}
					else {
						if ( ( Slot / 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:66%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
						}
						if ( ( Slot % 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s:%d(%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%d:%s:%d:%d:67%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar );
						}
					}
				}
			}
			break;
		case SYS_ABORTED :
			if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 0 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 4 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 8 ) ) {
				if      ( ( Dummy % 10 ) == 1 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:71%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar ); // 2012.09.26
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(D%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:D%d:%s:%d:%d:72%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar ); // 2012.09.26
				}
				else if ( ( Dummy % 10 ) == 2 ) {
					//
					if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
					//
					if ( ( Dummy / 10 ) == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(D%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:D%d:%s:%d:%d:73%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot , recipe , pt , nextres , 9 , Slot , MsgSltchar ); // 2009.03.09
					}
					else { // 2009.03.09
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(D%d-%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:D%d-%d:%s:%d:%d:74%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , ( Dummy / 10 ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , ( Dummy / 10 ) , Slot , recipe , pt , nextres , 9 , ( Dummy / 10 ) , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
					}
				}
				else {
					//
					// 2013.05.06
					//
					if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
						if ( side == PR_MON_Side_ex[0] ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:68%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , pt , nextres , 9 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( side              , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot              , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot              , recipe , pt , nextres , 9 , Slot              , MsgSltchar );
							_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[0] , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , PR_MON_Slot_ex[0] , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , PR_MON_Slot_ex[0] , recipe , pt , nextres , 9 , PR_MON_Slot_ex[0] , MsgSltchar );
						}
					}
					else {
						if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:75%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp + 1 ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp + 1 - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
						}
						else {
							if ( ( Slot / 100 ) > 0 ) {
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:76%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
							}
							if ( ( Slot % 100 ) > 0 ) {
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s:%d(%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%d:%s:%d:%d:77%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , chp - PM1 + 1 , chs , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar );
							}
						}
					}
				}
			}
			break;
		default :
			return;
			break;
		}
	}
	else {
		//
		switch( Result ) {
		case SYS_SUCCESS :
			//
			EST_PM_PROCESS_END( ch  ); // 2016.10.22
			//
			if      ( ( Dummy % 10 ) == 1 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:51%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar ); // 2012.09.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d)[%s][%d]%cPROCESS_END PM%d:1:D%d:%s:%d:%d:52%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar ); // 2012.09.26
			}
			else if ( ( Dummy % 10 ) == 2 ) {
				//
				if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
				//
				if ( ( Dummy / 10 ) == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d)[%s][%d]%cPROCESS_END PM%d:1:D%d:%s:%d:%d:53%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , recipe , pt , nextres , 9 , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
				else { // 2009.03.09
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(D%d-%d)[%s][%d]%cPROCESS_END PM%d:1:D%d-%d:%s:%d:%d:54%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , ( Dummy / 10 ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , ( Dummy / 10 ) , Slot , recipe , pt , nextres , 9 , ( Dummy / 10 ) , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
			}
			else {
				//
				// 2013.05.06
				//
				if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
					if ( side == PR_MON_Side_ex[0] ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:58%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , pt , nextres , 9 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( side              , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:59%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot              , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot              , recipe , pt , nextres , 9 , Slot              , MsgSltchar );
						_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[0] , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:59%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , PR_MON_Slot_ex[0] , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , PR_MON_Slot_ex[0] , recipe , pt , nextres , 9 , PR_MON_Slot_ex[0] , MsgSltchar );
					}
				}
				else {
					if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:55%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch + 1 - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
					}
					else {
						if ( ( Slot / 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:56%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
						}
						if ( ( Slot % 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:57%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar );
						}
					}
				}
				//
			}
			break;
		case SYS_ERROR :
			if      ( ( Dummy % 10 ) == 1 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:61%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar ); // 2012.09.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d)[%s][%d]%cPROCESS_ERROR PM%d:1:D%d:%s:%d:%d:62%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar ); // 2012.09.26
			}
			else if ( ( Dummy % 10 ) == 2 ) {
				//
				if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
				//
				if ( ( Dummy / 10 ) == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d)[%s][%d]%cPROCESS_ERROR PM%d:1:D%d:%s:%d:%d:63%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , recipe , pt , nextres , 9 , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
				else { // 2009.03.09
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(D%d-%d)[%s][%d]%cPROCESS_ERROR PM%d:1:D%d-%d:%s:%d:%d:64%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , ( Dummy / 10 ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , ( Dummy / 10 ) , Slot , recipe , pt , nextres , 9 , ( Dummy / 10 ) , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
				}
			}
			else {
				//
				// 2013.05.06
				//
				//
				if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
					if ( side == PR_MON_Side_ex[0] ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:68%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , pt , nextres , 9 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , MsgSltchar );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( side              , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot              , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot              , recipe , pt , nextres , 9 , Slot              , MsgSltchar );
						_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[0] , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , PR_MON_Slot_ex[0] , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , PR_MON_Slot_ex[0] , recipe , pt , nextres , 9 , PR_MON_Slot_ex[0] , MsgSltchar );
					}
				}
				else {
					if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:65%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch + 1 - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
					}
					else {
						if ( ( Slot / 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:66%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
						}
						if ( ( Slot % 100 ) > 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:67%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar );
						}
					}
				}
			}
			break;
		case SYS_ABORTED :
			if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 0 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 4 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 8 ) ) {
				if      ( ( Dummy % 10 ) == 1 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:71%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar ); // 2012.09.26
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d)[%s][%d]%cPROCESS_ABORT PM%d:1:D%d:%s:%d:%d:72%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar ); // 2012.09.26
				}
				else if ( ( Dummy % 10 ) == 2 ) {
					//
					if ( ( Slot / 100 ) > 0 ) Slot = Slot / 100; // 2013.03.11
					//
					if ( ( Dummy / 10 ) == 0 ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d)[%s][%d]%cPROCESS_ABORT PM%d:1:D%d:%s:%d:%d:73%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , ch ) , 9 , ch - PM1 + 1 , Slot , recipe , pt , nextres , 9 , Slot , MsgSltchar ); // 2009.03.09
					}
					else { // 2009.03.09
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(D%d-%d)[%s][%d]%cPROCESS_ABORT PM%d:1:D%d-%d:%s:%d:%d:74%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , ( Dummy / 10 ) , Slot , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , ( Dummy / 10 ) , Slot , recipe , pt , nextres , 9 , ( Dummy / 10 ) , Slot , MsgSltchar ); // 2009.03.09 // 2012.09.26
					}
				}
				else {
					//
					// 2013.05.06
					//
					if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
						if ( side == PR_MON_Side_ex[0] ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:68%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , recipe , pt , nextres , 9 , Slot + ( PR_MON_Slot_ex[0] * 100 ) , MsgSltchar );
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( side              , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot              , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot              , recipe , pt , nextres , 9 , Slot              , MsgSltchar );
							_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[0] , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:69%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , PR_MON_Slot_ex[0] , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , PR_MON_Slot_ex[0] , recipe , pt , nextres , 9 , PR_MON_Slot_ex[0] , MsgSltchar );
						}
					}
					else {
						if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:75%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch + 1 - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
						}
						else {
							if ( ( Slot / 100 ) > 0 ) {
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:76%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot / 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot / 100 , recipe , pt , nextres , 9 , Slot / 100 , MsgSltchar );
							}
							if ( ( Slot % 100 ) > 0 ) {
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:77%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Slot % 100 , recipe , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , Slot % 100 , recipe , pt , nextres , 9 , Slot % 100 , MsgSltchar );
							}
						}
					}
				}
			}
			break;
		default :
			return;
			break;
		}
	}

	if ( ( Dummy % 10 ) != 0 ) { // 2009.03.18
		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot , pt , ( Result == SYS_SUCCESS ) ? ( Dummy * 1000 ) + 0 : ( Dummy * 1000 ) + 1 , Last , recipe , "" ); // 2009.03.18 // 2010.11.30
	}
	else {
		if ( ( ( Slot / 100 ) <= 0 ) && ( PR_MON_MultiUnuse_ex[0] )  && ( PR_MON_Slot_ex[0] > 0 ) ) { // 2014.07.25
			//
			_i_SCH_LOG_TIMER_PRINTF( side              , TIMER_PM_END , ch - PM1 + 1 , Slot              , pt                   , ( Result == SYS_SUCCESS ) ? 0 : 1 , Last , recipe , "" );
			_i_SCH_LOG_TIMER_PRINTF( PR_MON_Side_ex[0] , TIMER_PM_END , ch - PM1 + 1 , PR_MON_Slot_ex[0] , PR_MON_Pointer_ex[0] , ( Result == SYS_SUCCESS ) ? 0 : 1 , Last , recipe , "" );
			//
		}
		else {
			if ( ( ( Slot / 100 ) > 0 ) && ( ( Slot % 100 ) <= 0 ) && ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) ) { // 2013.05.06
				_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch + 1 - PM1 + 1 , Slot / 100 , pt , ( Result == SYS_SUCCESS ) ? 0 : 1 , Last , recipe , "" );
			}
			else {
				if ( ( Slot / 100 ) > 0 ) {
					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot / 100 , pt , ( Result == SYS_SUCCESS ) ? 0 : 1 , Last , recipe , "" );
				}
				if ( ( Slot % 100 ) > 0 ) {
					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , Slot % 100 , pt , ( Result == SYS_SUCCESS ) ? 0 : 1 , Last , recipe , "" );
				}
			}
		}
	}
}


void PROCESS_SUB_EXTEND_PM_END_MESSAGE( int side , int ch , int Result , int nextres ) {
	int xp;
	char MsgSltchar[16]; /* 2013.05.23 */
	//
	if ( ( _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() / 2 ) != 0 ) return; // 2017.01.11
	//
	for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH ; xp++ ) {
		//
		if ( PR_MON_MultiUnuse_ex[xp] ) continue; // 2014.02.10
		//
		if ( PR_MON_Slot_ex[xp] > 0 ) {
			//
			FA_SIDE_TO_SLOT_GET_L( PR_MON_Side_ex[xp] , PR_MON_Pointer_ex[xp] , MsgSltchar );
			//
			switch( Result ) {
			case SYS_SUCCESS :
//				_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[xp] , "PM Process End at %s(%d:%s%d)[%s][%d]%cPROCESS_END PM%d:%d:%s%d:%s:%d:81%c%s%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xp+1 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , xp+1 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , xp+1 , 9 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , MsgSltchar ); // 2012.09.26 // 2015.04.30
				_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[xp] , "PM Process End at %s(%d:%s%d)[%s][%d]%cPROCESS_END PM%d:%d:%s%d:%s:%d:%d:81%c%s%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xp+2 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , xp+2 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , PR_MON_Pointer_ex[xp] , nextres , 9 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , MsgSltchar ); // 2012.09.26 // 2015.04.30
				break;
			case SYS_ERROR :
//				_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[xp] , "PM Process Error at %s(%d:%s%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s%d:%s:%d:82%c%s%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xp+1 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , xp+1 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , xp+1 , 9 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , MsgSltchar ); // 2012.09.26 // 2015.04.30
				_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[xp] , "PM Process Error at %s(%d:%s%d)[%s][%d]%cPROCESS_ERROR PM%d:%d:%s%d:%s:%d:%d:82%c%s%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xp+2 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , xp+2 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , PR_MON_Pointer_ex[xp] , nextres , 9 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , MsgSltchar ); // 2012.09.26 // 2015.04.30
				break;
			case SYS_ABORTED :
//				if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 0 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 4 ) && ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() != 8 ) ) { // 2015.04.30
//					_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[xp] , "PM Process Abort at %s(%d:%s%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s%d:%s:%d:83%c%s%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xp+1 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , xp+1 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , xp+1 , 9 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , MsgSltchar ); // 2012.09.26 // 2015.04.30
					_i_SCH_LOG_LOT_PRINTF( PR_MON_Side_ex[xp] , "PM Process Abort at %s(%d:%s%d)[%s][%d]%cPROCESS_ABORT PM%d:%d:%s%d:%s:%d:%d:83%c%s%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , xp+2 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , ch ) , 9 , ch - PM1 + 1 , xp+2 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , PROCESS_MONITOR_Get_Recipe(side,ch) , PR_MON_Pointer_ex[xp] , nextres , 9 , _lot_printf_sub_string_Dummy[PR_MON_Dummy_ex[xp]] , PR_MON_Slot_ex[xp] , MsgSltchar ); // 2012.09.26 // 2015.04.30
//				} // 2015.04.30
				break;
			default :
				return;
				break;
			}
// 2007.11.07
			if ( PR_MON_Dummy_ex[xp] ) {
				_i_SCH_LOG_TIMER_PRINTF( PR_MON_Side_ex[xp] , TIMER_PM_END , ch - PM1 + 1 , PR_MON_Slot_ex[xp] , PR_MON_Pointer_ex[xp] , ( Result == SYS_SUCCESS ) ? 2000 : 2001 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" ); // 2008.07.15
			}
			else {
				_i_SCH_LOG_TIMER_PRINTF( PR_MON_Side_ex[xp] , TIMER_PM_END , ch - PM1 + 1 , PR_MON_Slot_ex[xp] , PR_MON_Pointer_ex[xp] , ( Result == SYS_SUCCESS ) ? 0 : 1 , PROCESS_MONITOR_Get_Last(side,ch) , PROCESS_MONITOR_Get_Recipe(side,ch) , "" ); // 2008.07.15
			}
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_PROCESS_INTERFACE_MODULE_DATA( int apmode , int side ) {
	int i , j;
	if ( apmode == 0 ) {
		//
		InitializeCriticalSection( &CR_PM_INTERFACE );
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			InitializeCriticalSection( &CR_PM_MULTI_PROCESS[i] ); // 2011.03.02
			//
			InitializeCriticalSection( &CR_PM_DATA_RESET1[i] ); // 2011.04.18
			InitializeCriticalSection( &CR_PM_DATA_RESET2[i] ); // 2011.04.18
			//
		}
		//
	}
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		EnterCriticalSection( &CR_PM_INTERFACE );
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			for ( j = 0 ; j < MAX_INTERFACE_COUNT ; j++ ) {
				PM_INTERFACE_MONITOR_SIDE[i][j] = -1;
			}
			PM_INTERFACE_MONITOR_COUNT[i] = 0;
		}
		//========================================================
		// 2006.07.04
		//========================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			PM_LAST_PROCESS_JOB_SIDE[ i ] = -1;
			if ( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ i ] != NULL ) { // 2007.03.15
				free( PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ i ] );
				PM_LAST_PROCESS_JOB_CONTROL_PROCESS[ i ] = NULL;
			}
		}
		//========================================================
		LeaveCriticalSection( &CR_PM_INTERFACE );
	}
}
//
BOOL PROCESS_INTERFACE_MODULE_GET_INFO( int ch , int index , int *s , int *m , int *p , int *w ) {
	//
	*s = PM_INTERFACE_MONITOR_SIDE[ch][index];
	*m = PM_INTERFACE_MONITOR_MODE[ch][index];
	*p = PM_INTERFACE_MONITOR_POINTER[ch][index];
	*w = PM_INTERFACE_MONITOR_WAFER[ch][index];
	//
	if ( *s >= 0 ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
	//
}
//

void PROCESS_INTERFACE_MODULE_CLEAR_INFO( int side , int ch ) {
	int i;
	int max; // 2018,06.07
	//
	EnterCriticalSection( &CR_PM_INTERFACE );
	if ( side != TR_CHECKING_SIDE ) { // 2002.07.16
		_i_SCH_MODULE_Set_PM_WAFER( ch , 0 , 0 );
	}
//	if ( _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( _i_SCH_PRM_GET_MODULE_GROUP( ch ) ) ) { // 2002.02.19
		_i_SCH_IO_SET_MODULE_STATUS( ch , 1 , 0 );
//	} // 2002.02.19
	//
	//
	// 2018.06.07
	//
	max = 0;
	//
	for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
		//
		if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] >= 0 ) && ( PM_INTERFACE_MONITOR_MODE[ch][ i ] >= 1 ) ) {
			//
			if ( PM_INTERFACE_MONITOR_MODE[ch][ i ] > max ) max = PM_INTERFACE_MONITOR_MODE[ch][ i ];
			//
		}
		//
	}
	//
	for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
//		if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] == side ) && ( PM_INTERFACE_MONITOR_MODE[ch][ i ] == 0 ) ) { // 2018.06.05
		if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] >= 0 ) && ( PM_INTERFACE_MONITOR_MODE[ch][ i ] == 0 ) ) { // 2018.06.05
			//
//			time( &(PM_INTERFACE_MONITOR_TIMER[ch][ i ]) ); // 2018.06.07
			//
			PM_INTERFACE_MONITOR_MODE[ch][ i ] = max + 1; // 2018.06.07
			//
			break;
		}
		//
	}
	LeaveCriticalSection( &CR_PM_INTERFACE );
}
//
void PROCESS_INTERFACE_MODULE_APPEND_DATA( int ch , int side , int pt , int w ) {
	int i;
	EnterCriticalSection( &CR_PM_INTERFACE );
	for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
		if ( PM_INTERFACE_MONITOR_SIDE[ch][i] < 0 ) break;
	}
	if ( i < MAX_INTERFACE_COUNT ) {
		//
		PM_INTERFACE_MONITOR_MODE[ch][ i ] = 0;
		//
//		if ( side != TR_CHECKING_SIDE ) {
//			PM_INTERFACE_MONITOR_POINTER[ch][ i ] = pt;
//			PM_INTERFACE_MONITOR_WAFER[ch][ i ] = _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ); // 2008.08.12
//		}
//		else { // 2002.07.16
//			PM_INTERFACE_MONITOR_POINTER[ch][ i ] = pt / 100;
//			PM_INTERFACE_MONITOR_WAFER[ch][ i ] = pt % 100;
//		}
		//
		PM_INTERFACE_MONITOR_POINTER[ch][ i ] = pt; // 2008.08.12
		PM_INTERFACE_MONITOR_WAFER[ch][ i ] = w; // 2008.08.12
		//
		PM_INTERFACE_MONITOR_SIDE[ch][i] = side;
		//
		PM_INTERFACE_MONITOR_STARTCLOCK[ch][i] = GetTickCount(); // 2019.03.08
		//
		PM_INTERFACE_MONITOR_SOURCE[ch][ i ] = _i_SCH_IO_GET_MODULE_SOURCE( ch , 1 ); // 2018.09.05
		PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] = _i_SCH_IO_GET_MODULE_SOURCE_E( ch , 1 ); // 2018.09.05
		//
		PM_INTERFACE_MONITOR_COUNT[ch]++;
	}
	LeaveCriticalSection( &CR_PM_INTERFACE );
}
//


int PROCESS_INTERFACE_MODULE_GET_TIME( int ch ) { // 2018.06.07
	int i;
	time_t finish;
	//
	if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) <= 0 ) return -2;
	//
	for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
		//
		if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] >= 0 ) && ( PM_INTERFACE_MONITOR_MODE[ch][ i ] >= 1 ) ) {
			//
			time( &finish );
			return (int) difftime( finish , PM_INTERFACE_MONITOR_RTIMER[_i_SCH_PRM_GET_MODULE_INTERFACE( ch )] );
			//
		}
		//
	}
	//
	return -1;
	//
}

//
void PROCESS_INTERFACE_MODULE_TIMER_RESET( int xch ) { // 2018.06.07
	time( &(PM_INTERFACE_MONITOR_RTIMER[xch]) );
}
//
//
int PROCESS_INTERFACE_MODULE_RECOVER_DATA( int ch , BOOL Mode , char *data , int *rwafer ) {
	int i , l , len , side , wafer , type , j , k;
	BOOL find;
	long tvalue;
	time_t finish;
	int Result; // , pres;
	int Finish_Result; // 2013.06.19
	int Elapsed_Sec; // 2019.03.08

	EnterCriticalSection( &CR_PM_INTERFACE );
	Result = 0;
	if ( Mode ) {
		find = FALSE;
		side = 0;
		wafer = 0;
		type = SYS_SUCCESS;
		len = strlen( data );
		for ( l = 0 ; l < len ; l++ ) {
			switch( data[l] ) {
			case '|' :	side++; wafer = 0;	break;
			case '0' : // Ready
				wafer++;
				break;		
			case '1' : // Picking
				wafer++;
				break;		
			case '2' : // Running
				wafer++;
				break;
			case '3' : // Success
				wafer++;
				for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
					if ( PM_INTERFACE_MONITOR_MODE[ch][i] >= 1 ) {
						if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] == side ) && ( PM_INTERFACE_MONITOR_WAFER[ch][i] == wafer ) ) {
							find = TRUE;
							type = SYS_SUCCESS;
							l = 99999;
							break;
						}
					}
				}
				break;
			case '4' : // Fail
				wafer++;
				for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) { // 2002.03.25
					if ( PM_INTERFACE_MONITOR_MODE[ch][i] >= 1 ) {
						if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] == side ) && ( PM_INTERFACE_MONITOR_WAFER[ch][i] == wafer ) ) {
							find = TRUE;
							type = SYS_ERROR;
							l = 99999;
							break;
						}
					}
				}
				break;
			case '5' : // Lost
				wafer++;
				for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) { // 2002.03.25
					if ( PM_INTERFACE_MONITOR_MODE[ch][i] >= 1 ) {
						if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] == side ) && ( PM_INTERFACE_MONITOR_WAFER[ch][i] == wafer ) ) {
							find = TRUE;
							type = SYS_WAITING;
							l = 99999;
							break;
						}
					}
				}
				break;
			case '6' : // Aborted
				wafer++;
				for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) { // 2002.03.25
					if ( PM_INTERFACE_MONITOR_MODE[ch][i] >= 1 ) {
						if ( ( PM_INTERFACE_MONITOR_SIDE[ch][i] == side ) && ( PM_INTERFACE_MONITOR_WAFER[ch][i] == wafer ) ) {
							find = TRUE;
							type = SYS_ABORTED;
							l = 99999;
							break;
						}
					}
				}
				break;
			}
		}
		if ( !find ) i = MAX_INTERFACE_COUNT;
	}
	else {
		//
		type = SYS_SUCCESS;
		//
		// 2018.06.07
		//
		/*
		//
		for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
			if ( PM_INTERFACE_MONITOR_MODE[ch][i] == 1 ) {
				if ( PM_INTERFACE_MONITOR_SIDE[ch][i] >= 0 ) {
					time( &finish );
					tvalue = (long) difftime( finish , PM_INTERFACE_MONITOR_TIMER[ch][ i ] );
					if ( tvalue >= _i_SCH_PRM_GET_Process_Run_In_Time( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) ) ) break;
				}
			}
		}
		//
		*/
		//
		//
		find = -1;
		//
		for ( i = 0 ; i < MAX_INTERFACE_COUNT ; i++ ) {
			if ( PM_INTERFACE_MONITOR_MODE[ch][i] >= 1 ) {
				if ( PM_INTERFACE_MONITOR_SIDE[ch][i] >= 0 ) {
					//
					if ( find == -1 ) {
						find = i;
					}
					else {
						if ( PM_INTERFACE_MONITOR_MODE[ch][i] < PM_INTERFACE_MONITOR_MODE[ch][find] ) {
							find = i;
						}
					}
				}
			}
		}
		//
		if ( find != -1 ) {
			//
			time( &finish );
			tvalue = (long) difftime( finish , PM_INTERFACE_MONITOR_RTIMER[_i_SCH_PRM_GET_MODULE_INTERFACE( ch )] );
			//
			if ( tvalue >= _i_SCH_PRM_GET_Process_Run_In_Time( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) ) ) {
				//
				i = find;
				//
				PROCESS_INTERFACE_MODULE_TIMER_RESET(_i_SCH_PRM_GET_MODULE_INTERFACE( ch ));
				//
			}
			else {
				i = MAX_INTERFACE_COUNT;
			}
			//
		}
		else {
			i = MAX_INTERFACE_COUNT;
		}
		//
	}
	if ( i < MAX_INTERFACE_COUNT ) {
		if ( PM_INTERFACE_MONITOR_SIDE[ch][ i ] == TR_CHECKING_SIDE ) { // 2002.07.16
			switch( type ) {
			case SYS_SUCCESS :
			case SYS_ERROR :
				IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , -1 , -1 ); // 2007.07.25
				*rwafer = PM_INTERFACE_MONITOR_WAFER[ch][ i ];
				Result = 1;
				break;
			case SYS_ABORTED :
				IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , -1 , -1 ); // 2007.07.25
				*rwafer = PM_INTERFACE_MONITOR_WAFER[ch][ i ];
				Result = 2;
				break;
			case SYS_WAITING :
				*rwafer = PM_INTERFACE_MONITOR_WAFER[ch][ i ];
				Result = 3;
				break;
			}
			PM_INTERFACE_MONITOR_SIDE[ch][i] = -1;
			PM_INTERFACE_MONITOR_COUNT[ch]--;
		}
		else {
			//===================================================
			// 2002.03.25 //Append Result Check
			//===================================================
			//
			Elapsed_Sec = ( GetTickCount() - PM_INTERFACE_MONITOR_STARTCLOCK[ch][i] ) / 1000; // 2019.03.08
			//
			switch( type ) {
			case SYS_SUCCESS :
				//=======================================================================================================================
				//----Anim
				if ( _i_SCH_SUB_Run_Impossible_Condition( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , -2 ) ) { // 2007.03.20
//					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , MAX_CASSETTE_SIDE , PM_INTERFACE_MONITOR_SIDE[ch][ i ] + 1 , -1 , -1 ); // 2007.07.25
					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SOURCE[ch][ i ] , PM_INTERFACE_MONITOR_SIDE[ch][ i ] + 1 , WAFERRESULT_SUCCESS , PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] ); // 2007.07.25
				}
				else {
//					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , -1 ); // 2007.07.25
					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SOURCE[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , WAFERRESULT_SUCCESS , PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] ); // 2007.07.25
				}
				//----Anim
				//=======================================================================================================================
				//============================================================
//				pres = PROCESS_MONITOR_Set_Finish_xStatus( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 ); // 2013.06.19
				//============================================================
				PROCESS_SUB_SUCCESS_RESULT_OPERATION( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 , 0 , _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO(_i_SCH_PRM_GET_MODULE_INTERFACE( ch )) , FALSE , FALSE , -1 ); // 2007.07.24
				//============================================================
				PROCESS_SUB_ADD_RUN_COUNT( TRUE , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 1 , FALSE , FALSE ); // 2007.07.24
				//============================================================
//				_i_SCH_LOG_TIMER_PRINTF( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , TIMER_PM_END , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) - PM1 + 1 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , 0 , FALSE , "EXTERNAL" , "" ); // 2019.03.08
				//
				_i_SCH_LOG_LOT_PRINTF(PM_INTERFACE_MONITOR_SIDE[ch][i], "PM Process End at %s(%d)[%s][%d]%cPROCESS_END PM%d:1:%d:%s:%d:%d:951%c%d\n", _i_SCH_SYSTEM_GET_MODULE_NAME(ch), PM_INTERFACE_MONITOR_WAFER[ch][i], "EXTERNAL", Elapsed_Sec, 9, ch - PM1 + 1, PM_INTERFACE_MONITOR_WAFER[ch][i], "EXTERNAL", PM_INTERFACE_MONITOR_POINTER[ch][i], 0, 9, PM_INTERFACE_MONITOR_WAFER[ch][i]); // 2019.03.08
				_i_SCH_LOG_TIMER_PRINTF(PM_INTERFACE_MONITOR_SIDE[ch][i], TIMER_PM_END, ch - PM1 + 1, PM_INTERFACE_MONITOR_WAFER[ch][i], PM_INTERFACE_MONITOR_POINTER[ch][i], 0, FALSE, "EXTERNAL", "");
				//=======================================================================================================================
				_i_SCH_MODULE_Set_PM_SIDE_POINTER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , 0 , 0 );
				_i_SCH_MODULE_Set_PM_WAFER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] );
				//=======================================================================================================================
//				if ( pres ) PROCESS_MONITOR_Set_Finish_xStatus( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -100 ); // 2011.03.02 // 2013.06.19
				//=======================================================================================================================
				if ( PROCESS_MONITOR_Set_Finish_Status( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 ) ) { // 2013.06.19
					PROCESS_MONITOR_Set_Finish_Status( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -100 ); // 2013.06.19
				}
				//=======================================================================================================================
				break;
			case SYS_ERROR :
				//=======================================================================================================================
				Finish_Result = -1; // 2013.06.19
				//=======================================================================================================================
				//----Anim
				if ( _i_SCH_SUB_Run_Impossible_Condition( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , -2 ) ) { // 2007.03.20
//					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , MAX_CASSETTE_SIDE , PM_INTERFACE_MONITOR_SIDE[ch][ i ] + 1 , -1 , -1 ); // 2007.07.25
					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SOURCE[ch][ i ] , PM_INTERFACE_MONITOR_SIDE[ch][ i ] + 1 , WAFERRESULT_FAILURE , PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] ); // 2007.07.25
				}
				else {
//					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , -1 ); // 2007.07.25
					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SOURCE[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , WAFERRESULT_FAILURE , PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] ); // 2007.07.25
				}
				//----Anim
				//=======================================================================================================================
				j = _i_SCH_PRM_GET_MODULE_INTERFACE( ch );
				k = PM_INTERFACE_MONITOR_SIDE[ch][ i ];
				//=======================================================================================================================
				PROCESS_SUB_FAIL_SCENARIO_LEVEL1_OPERATION( k , _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( j ) , FALSE ); // 2007.07.24
				//=======================================================================================================================
				PROCESS_SUB_FAIL_SCENARIO_LEVEL2_OPERATION( k , j , _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( j ) , TRUE , &Finish_Result ); // 2007.07.24
				//=======================================================================================================================
//				PROCESS_SUB_FAIL_RESULT_OPERATION( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , _i_SCH_PRM_GET_MRES_FAIL_SCENARIO(_i_SCH_PRM_GET_MODULE_INTERFACE( ch )) , FALSE , FALSE , -1 ); // 2007.07.24 // 2015.04.30
				PROCESS_SUB_FAIL_RESULT_OPERATION( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 , 0 , _i_SCH_PRM_GET_MRES_FAIL_SCENARIO(_i_SCH_PRM_GET_MODULE_INTERFACE( ch )) % 4 , FALSE , FALSE , -1 ); // 2007.07.24 // 2015.04.30
				//============================================================================================================
				PROCESS_SUB_ADD_RUN_COUNT( FALSE , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 1 , FALSE , FALSE ); // 2007.07.24
				//============================================================================================================
//				_i_SCH_LOG_TIMER_PRINTF( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , TIMER_PM_END , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) - PM1 + 1 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , 1 , FALSE , "EXTERNAL" , "" ); // 2019.03.08
//
				_i_SCH_LOG_LOT_PRINTF(PM_INTERFACE_MONITOR_SIDE[ch][i], "PM Process Error at %s(%d)[%s][%d]%cPROCESS_ERROR PM%d:1:%d:%s:%d:%d:951%c%d\n", _i_SCH_SYSTEM_GET_MODULE_NAME(ch), PM_INTERFACE_MONITOR_WAFER[ch][i], "EXTERNAL", Elapsed_Sec, 9, ch - PM1 + 1, PM_INTERFACE_MONITOR_WAFER[ch][i], "EXTERNAL", PM_INTERFACE_MONITOR_POINTER[ch][i], 1, 9, PM_INTERFACE_MONITOR_WAFER[ch][i]); // 2019.03.08
				_i_SCH_LOG_TIMER_PRINTF(PM_INTERFACE_MONITOR_SIDE[ch][i], TIMER_PM_END, ch - PM1 + 1, PM_INTERFACE_MONITOR_WAFER[ch][i], PM_INTERFACE_MONITOR_POINTER[ch][i], 1, FALSE, "EXTERNAL", ""); // 2019.03.08
				//=======================================================================================================================
				_i_SCH_MODULE_Set_PM_SIDE_POINTER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , 0 , 0 );
				_i_SCH_MODULE_Set_PM_WAFER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] );
				//=======================================================================================================================
				PROCESS_MONITOR_Set_Finish_Status( k , j , Finish_Result ); // 2013.06.19
				//=======================================================================================================================
				break;
			case SYS_ABORTED :
				//=======================================================================================================================
				//----Anim
				if ( _i_SCH_SUB_Run_Impossible_Condition( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , -2 ) ) { // 2007.03.20
//					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , MAX_CASSETTE_SIDE , PM_INTERFACE_MONITOR_SIDE[ch][ i ] + 1 , -1 , -1 ); // 2007.07.25
					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SOURCE[ch][ i ] , PM_INTERFACE_MONITOR_SIDE[ch][ i ] + 1 , WAFERRESULT_FAILURE , PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] ); // 2007.07.25
				}
				else {
//					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , -1 ); // 2007.07.25
					IO_WAFER_DATA_SET_TO_CHAMBER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 , PM_INTERFACE_MONITOR_SOURCE[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , WAFERRESULT_FAILURE , PM_INTERFACE_MONITOR_SOURCE_E[ch][ i ] ); // 2007.07.25
				}
				//----Anim
				//=======================================================================================================================
				//============================================================================================================
				PROCESS_SUB_END_OPERATION( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , FALSE );
				//============================================================================================================
//				PROCESS_MONITOR_Set_Finish_xStatus( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 ); // 2013.06.19
				//============================================================================================================
				PROCESS_SUB_FAIL_RESULT_OPERATION( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 , 0 , _i_SCH_PRM_GET_MRES_ABORT_SCENARIO(_i_SCH_PRM_GET_MODULE_INTERFACE( ch )) , FALSE , FALSE , -1 ); // 2007.07.24
				//============================================================================================================
				PROCESS_SUB_ADD_RUN_COUNT( FALSE , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 1 , FALSE , FALSE ); // 2007.07.24
				//============================================================================================================
//				_i_SCH_LOG_TIMER_PRINTF( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , TIMER_PM_END , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) - PM1 + 1 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , 1 , FALSE , "EXTERNAL" , "" ); // 2019.03.08
//
				_i_SCH_LOG_LOT_PRINTF(PM_INTERFACE_MONITOR_SIDE[ch][i], "PM Process Abort at %s(%d)[%s][%d]%cPROCESS_ABORT PM%d:1:%d:%s:%d:%d:951%c%d\n", _i_SCH_SYSTEM_GET_MODULE_NAME(ch), PM_INTERFACE_MONITOR_WAFER[ch][i], "EXTERNAL", Elapsed_Sec, 9, ch - PM1 + 1, PM_INTERFACE_MONITOR_WAFER[ch][i], "EXTERNAL", PM_INTERFACE_MONITOR_POINTER[ch][i], 1, 9, PM_INTERFACE_MONITOR_WAFER[ch][i]); // 2019.03.08
				_i_SCH_LOG_TIMER_PRINTF(PM_INTERFACE_MONITOR_SIDE[ch][i], TIMER_PM_END, ch - PM1 + 1, PM_INTERFACE_MONITOR_WAFER[ch][i], PM_INTERFACE_MONITOR_POINTER[ch][i], 1, FALSE, "EXTERNAL", ""); // 2019.03.08
				//=======================================================================================================================
				_i_SCH_MODULE_Set_PM_SIDE_POINTER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , PM_INTERFACE_MONITOR_SIDE[ch][ i ] , PM_INTERFACE_MONITOR_POINTER[ch][ i ] , 0 , 0 );
				_i_SCH_MODULE_Set_PM_WAFER( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , 0 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] );
				//=======================================================================================================================
				PROCESS_MONITOR_Set_Finish_Status( PM_INTERFACE_MONITOR_SIDE[ch][ i ] , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) , -1 ); // 2013.06.19
				//============================================================================================================
				break;
			case SYS_WAITING :
				if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2011.07.27
					_i_SCH_MODULE_Inc_FM_InCount( PM_INTERFACE_MONITOR_SIDE[ch][ i ] );
				}
				//
				_i_SCH_MODULE_Inc_TM_InCount( PM_INTERFACE_MONITOR_SIDE[ch][ i ] );
				//
				_i_SCH_LOG_LOT_PRINTF( side , "PM%d Disappear at %d%cDISAPPEAR PM%d:%d%c%d\n" , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) - PM1 + 1 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , 9 , _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) - PM1 + 1 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] , 9 , PM_INTERFACE_MONITOR_WAFER[ch][ i ] );
				break;
			}
			PM_INTERFACE_MONITOR_SIDE[ch][i] = -1;
			PM_INTERFACE_MONITOR_COUNT[ch]--;
		}
	}
	LeaveCriticalSection( &CR_PM_INTERFACE );
	return Result;
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Process Log Data Monitoring
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//#define	MAX_PM_LOG_STEP_COUNT	MAX_PM_STEP_COUNT
//#define	MAX_PM_LOG_STEP_COUNT	25 // 2013.04.17
#define	MAX_PM_LOG_STEP_COUNT	MAX_PM_STEP_COUNT // 2013.04.17
//------------------------------------------------------------------------------------------
int PROCESSLOG_I[ MAX_CHAMBER ];
int PROCESSLOG_J[ MAX_CHAMBER ];
int PROCESSLOG_SIDE[ MAX_CHAMBER ];
int PROCESSLOG_STEPAUTOREMAP[ MAX_CHAMBER ]; // 2013.06.04
//int PROCESSLOG_L[ MAX_CHAMBER ]; // 2007.11.07
//int PROCESSLOG_E[ MAX_CHAMBER ]; // 2004.02.27 // 2007.11.07
int PROCESSLOG_XP[ MAX_CHAMBER ]; // 2007.05.02
long PROCESSLOG_CLOCK_START[ MAX_CHAMBER ];
long PROCESSLOG_CLOCK_OVERTIME[ MAX_CHAMBER ];
long PROCESSLOG_CLOCK_FLOW[ MAX_CHAMBER ];
char *PROCESSLOG_IMSI[ MAX_CHAMBER ];
char *PROCESSLOG_FILENAME[ MAX_CHAMBER ]; // 2004.04.06
char *PROCESSLOG_READDATA[ MAX_CHAMBER ];
FILE *PROCESSLOG_FPT[ MAX_CHAMBER ];

//
int  PROCESSLOG_STEP_LAST_READ[ MAX_CHAMBER ];
int  PROCESSLOG_STEP_CURRENT_READ[ MAX_CHAMBER ];
//char PROCESSLOG_STEP_NAME[ MAX_CHAMBER ][MAX_PM_LOG_STEP_COUNT][65]; // 2013.04.17
char *PROCESSLOG_STEP_NAME[ MAX_CHAMBER ][MAX_PM_LOG_STEP_COUNT]; // 2013.04.17
int  PROCESSLOG_CH0[ MAX_CHAMBER ];
int  PROCESSLOG_DOING[ MAX_CHAMBER ]; // 2006.02.22
//
int PROCESSLOG_DUMMY[ MAX_CHAMBER ]; // 2007.04.19
int PROCESSLOG_FNOTUSE[ MAX_CHAMBER ]; // 2007.07.25
int PROCESSLOG_WFRNOTCHECK[ MAX_CHAMBER ]; // 2007.10.11
int PROCESSLOG_OPTION[ MAX_CHAMBER ]; // 2007.07.25
//
int PROCESSLOG_SIDE_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int PROCESSLOG_POINTER_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int PROCESSLOG_WAFER_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
int PROCESSLOG_DUMMY_EX[ MAX_CHAMBER ][MAX_PM_SLOT_DEPTH]; // 2007.04.19
//
//
/*
// 2007.06.08
char PROCESSLOG_TUNE_INFO_FOR_LOG[ MAX_CHAMBER ][256];
BOOL PROCESSLOG_TUNE_INFO_FOR_LOG_USE[ MAX_CHAMBER ];
*/
//
/*
// 2007.06.08
void PROCESS_MONITOR_TUNE_DATA_SET_FOR_LOG( int Ch , BOOL Use , char *data ) {
	strncpy( PROCESSLOG_TUNE_INFO_FOR_LOG[ Ch ] , data , 255 );
	PROCESSLOG_TUNE_INFO_FOR_LOG_USE[ Ch ] = Use;
}
*/
//
void PROCESS_MONITOR_LOG_READY( int side , int Ch , int Option ) {
//	PROCESSLOG_TUNE_INFO_FOR_LOG_USE[ Ch ] = FALSE; // 2007.06.08
//	SCHMULTI_TUNE_INFO_DATA_RESET( Ch ); // 2007.06.08 // 2007.10.19
//	if ( !SCHMULTI_CTJOB_USE_GET() || !PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ) { // 2002.05.20 // 2002.06.28 // 2019.04.05
	if ( !SCHMULTI_CTJOB_USE_GET() || ( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() % 2 ) == 0 ) ) { // 2002.05.20 // 2002.06.28 // 2019.04.05
		//
		if ( PROCESSLOG_MONITOR_IO[ Ch ] < 0 ) return;
		//
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) <= 0 ) {
			_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] , "READY %d|%d" , side , Option );
		}
		else { // 2012.02.10
			_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] , "READY %d|%d" , side , Option );
		}
	}
	else {
		//
		if ( PROCESSLOG_MONITOR_IO[ Ch ] < 0 ) return;
		//
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) > 0 ) { // 2012.02.10
			_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] , "READY %d|%d" , side , Option );
		}
		//
	}
}
//
void PROCESS_MONITOR_LOG_READY_for_PRJOB( char *CPJobName , char *PRJobName ) { // 2002.05.20
	int i;
//	if ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ) { // 2002.06.28 // 2019.04.05
	if ( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() % 2 ) == 1 ) { // 2002.06.28 // 2019.04.05
		for ( i = PM1 ; i < AL ; i++ ) {
//			if ( PROCESSLOG_MONITOR_IO[ i ] < 0 ) return; // 2005.03.12
			if ( PROCESSLOG_MONITOR_IO[ i ] < 0 ) continue; // 2005.03.12
			if ( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) <= 0 ) {
				_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ i ] , "READY -|-|%s_%s" , CPJobName , PRJobName );
//xprintf( "============================================================\n" );
//xprintf( "READY PM%d - %s,%s\n" , i - PM1 + 1 , CPJobName , PRJobName );
//xprintf( "============================================================\n" );
			}
		}
	}
}
//
void PROCESS_MONITOR_LOG_END( int side , int Ch , BOOL OnlyOne ) {
	int l; // 2005.03.12
//	2001.12.20
	// PROCESSLOG_TUNE_INFO_FOR_LOG_USE[ Ch ] = FALSE; // 2007.06.08
//	SCHMULTI_TUNE_INFO_DATA_RESET( Ch ); // 2007.06.08 // 2007.10.19
//	if ( !SCHMULTI_CTJOB_USE_GET() || !PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ) { // 2002.05.20 // 2002.06.28 // 2019.04.05
	if ( !SCHMULTI_CTJOB_USE_GET() || ( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() % 2 ) == 0 ) ) { // 2002.05.20 // 2002.06.28 // 2019.04.05
		//
		if ( PROCESSLOG_MONITOR_IO[ Ch ] < 0 ) return;
		//
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) <= 0 ) {
			//
			_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] , "END %d|%d" , side , OnlyOne );
			//
			l = 0; // 2005.03.12
			do {
				if ( l >= 100 ) { // 2005.03.12 // 10 sec
					_IO_CIM_PRINTF( "==========================================================================================================\n" );
					_IO_CIM_PRINTF( "PROCESS LOG MONITOR FUNCTION(PM%d) END MESSAGE FINISH CHECK Timeout[C,%d,%d]\n" , Ch - PM1 + 1 , side , OnlyOne );
					_IO_CIM_PRINTF( "==========================================================================================================\n" );
					break;
				}
//				Sleep(100); // 2004.10.14
//				Sleep(10); // 2005.03.12
				Sleep(100); // 2005.03.12
				l++; // 2005.03.12
			}
			while( _dREAD_FUNCTION_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] ) == SYS_RUNNING );
		}
		else { // 2012.02.10
			//
			_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] , "END %d|%d" , side , OnlyOne );
			//
		}
	}
	else {
		//
		if ( PROCESSLOG_MONITOR_IO[ Ch ] < 0 ) return;
		//
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) > 0 ) { // 2012.02.10
			//
			_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ Ch ] , "END %d|%d" , side , OnlyOne );
			//
		}
	}
}
//
//
void PROCESS_MONITOR_LOG_END_for_PRJOB( int id , char *CPJobName , char *PRJobName ) { // 2002.05.20
	int i;
	int l; // 2005.03.12
//	if ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() ) { // 2002.06.28 // 2019.04.05
	if ( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() % 2 ) == 1 ) { // 2002.06.28 // 2019.04.05
		for ( i = PM1 ; i < AL ; i++ ) {
//			if ( PROCESSLOG_MONITOR_IO[ i ] < 0 ) return; // 2005.03.12
			if ( PROCESSLOG_MONITOR_IO[ i ] < 0 ) continue; // 2005.03.12
			if ( _i_SCH_PRM_GET_MODULE_INTERFACE( i ) <= 0 ) {
				_dWRITE_FUNCTIONF_EVENT( PROCESSLOG_MONITOR_IO[ i ] , "END -|%d|%s_%s" , id , CPJobName , PRJobName );
//xprintf( "============================================================\n" );
//xprintf( "END   PM%d - %s,%s (%d)\n" , i - PM1 + 1 , CPJobName , PRJobName , id );
//xprintf( "============================================================\n" );
				l = 0; // 2005.03.12
				do {
					if ( l >= 100 ) { // 2005.03.12 // 10 sec
						_IO_CIM_PRINTF( "==========================================================================================================\n" );
						_IO_CIM_PRINTF( "PROCESS LOG MONITOR FUNCTION(PM%d) END MESSAGE FINISH CHECK Timeout[J,%s,%s,%d]\n" , i - PM1 + 1 , CPJobName , PRJobName , id );
						_IO_CIM_PRINTF( "==========================================================================================================\n" );
						break;
					}
//					Sleep(100); // 2004.10.14
//					Sleep(10); // 2005.03.12
					Sleep(100); // 2005.03.12
					l++; // 2005.03.12
				}
				while( _dREAD_FUNCTION_EVENT( PROCESSLOG_MONITOR_IO[ i ] ) == SYS_RUNNING );
			}
		}
		//=================================================================================================
		// 2005.04.06
		//=================================================================================================
		if ( PROCESS_PRJOB_End_Delay_Time > 0 ) {
			Sleep( PROCESS_PRJOB_End_Delay_Time );
		}
		//=================================================================================================
	}
}

BOOL PROCESS_MONITOR_GET_FILENAME( int prcs , int ch , int side , int pt , int i , int j , int e , char *filename , BOOL dummy , int mode , BOOL notuse , BOOL highappend ) {
	//
	BOOL filenotuse = FALSE; // 2012.11.29
	char foldername[256]; // 2011.05.13
	//
	//===========================================================================================================================================================================================================
	if ( _SCH_COMMON_GET_PROCESS_FILENAME( _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , prcs , ch , side , i , j , e , filename , dummy , mode , notuse , highappend ) ) return TRUE;
	//===========================================================================================================================================================================================================
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.05.13
		if ( STR_MEM_SIZE( EIL_Get_SLOT_FOLDER( side , pt ) ) > 0 ) {
			strncpy( foldername , EIL_Get_SLOT_FOLDER( side , pt ) , 255 );
		}
		else {
			strncpy( foldername , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , 255 );
		}
	}
	else {
		if ( STR_MEM_SIZE( PROCESS_Get_SLOT_FOLDER( side , pt ) ) > 0 ) { // 2012.02.18
			strncpy( foldername , PROCESS_Get_SLOT_FOLDER( side , pt ) , 255 );
		}
		else {
			strncpy( foldername , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , 255 );
		}
	}
	//===========================================================================================================================================================================================================
	if ( _SCH_COMMON_GET_PROCESS_FILENAME2( _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , prcs , ch , side , pt , &i , &j , &e , filename , foldername , &dummy , &mode , &notuse , &highappend , &filenotuse ) ) {
		return !filenotuse; // 2012.11.29
	}
	//===========================================================================================================================================================================================================
	if ( filenotuse ) return FALSE;
	//===========================================================================================================================================================================================================
	// 2007.06.04
	//====================================================================================================================================
	if ( mode > 3 ) {
		if ( prcs > 0 ) {
			sprintf( filename , "%s/LOTLOG/%s/PROCESS_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , ch - PM1 + 1 );
		}
		else {
			sprintf( filename , "%s/LOTLOG/%s/RECIPE_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , ch - PM1 + 1 );
		}
		if      ( mode == 4 ) strcat( filename , "_LOTPRE" );
		else if ( mode == 5 ) strcat( filename , "_LOTPOST" );
		//================================================================================================================================================
		if      ( notuse ) strcat( filename , "_TEMP" );
		//================================================================================================================================================
		return TRUE;
	}
	//
	//====================================================================================================================================
	//
	if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( side , pt ) != 0 ) dummy = TRUE; // 2015.10.12
	//
	//====================================================================================================================================
	if ( prcs > 0 ) {
		if ( e > 0 ) {
			sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_%02d_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , j , e , ch - PM1 + 1 );
		}
		else {
			if ( ( i > 0 ) && ( j > 0 ) ) {
				if ( dummy ) { // 2007.04.25
					sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_D%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , j , ch - PM1 + 1 );
				}
				else {
					sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , j , ch - PM1 + 1 );
				}
			}
			else if ( i > 0 ) {
				if ( highappend ) { // 2007.11.07
					if ( dummy ) { // 2007.04.25
						sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_D%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , PROCESSLOG_CH0[ ch ] % 100 , i , ch - PM1 + 1 );
					}
					else {
						sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , PROCESSLOG_CH0[ ch ] % 100 , i , ch - PM1 + 1 );
					}
				}
				else {
					if ( dummy ) { // 2007.04.25
						sprintf( filename , "%s/LOTLOG/%s/PROCESS_D%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , ch - PM1 + 1 );
					}
					else {
						sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , ch - PM1 + 1 );
					}
				}
			}
			else if ( j > 0 ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2014.07.11
					sprintf( filename , "%s/LOTLOG/%s/PROCESS_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , j , ch - PM1 + 1 + 1 );
				}
				else {
					sprintf( filename , "%s/LOTLOG/%s/PROCESS_00_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , j , ch - PM1 + 1 );
				}
			}
		}
	}
	else {
		if ( e > 0 ) {
			sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_%02d_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , j , e , ch - PM1 + 1 );
		}
		else {
			if ( ( i > 0 ) && ( j > 0 ) ) {
				if ( dummy ) { // 2007.04.25
					sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_D%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , j , ch - PM1 + 1 );
				}
				else {
					sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , j , ch - PM1 + 1 );
				}
			}
			else if ( i > 0 ) {
				if ( highappend ) { // 2007.11.07
					if ( dummy ) { // 2007.04.25
						sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_D%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , PROCESSLOG_CH0[ ch ] % 100 , i , ch - PM1 + 1 );
					}
					else {
						sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , PROCESSLOG_CH0[ ch ] % 100 , i , ch - PM1 + 1 );
					}
				}
				else {
					if ( dummy ) { // 2007.04.25
						sprintf( filename , "%s/LOTLOG/%s/RECIPE_D%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , ch - PM1 + 1 );
					}
					else {
						sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , i , ch - PM1 + 1 );
					}
				}
			}
			else if ( j > 0 ) {
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2014.07.11
					sprintf( filename , "%s/LOTLOG/%s/RECIPE_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , j , ch - PM1 + 1 + 1 );
				}
				else {
					sprintf( filename , "%s/LOTLOG/%s/RECIPE_00_%02d_PM%d" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , foldername , j , ch - PM1 + 1 );
				}
			}
		}
	}
	if      ( mode == 2 ) strcat( filename , "_PRE" );
	else if ( mode == 3 ) strcat( filename , "_POST" );
	//================================================================================================================================================
	if      ( notuse ) strcat( filename , "_TEMP" );
	//================================================================================================================================================
	return TRUE;
}

BOOL PROCESS_MONITOR_ONEBODY_CHECK( int Ch ) {
	//======================================================================================================================
	// 2006.06.28
	//======================================================================================================================
	if ( PM_PROCESS_MONITOR_ONE_BODY[ Ch ] == 0 ) {
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
			if ( ( ( Ch - PM1 ) % 2 ) == 0 ) {
				if ( PROCESSLOG_MONITOR_IO[ Ch + 1 ] < 0 ) {
					for ( PROCESSLOG_SIDE[Ch+1] = 0 ; PROCESSLOG_SIDE[Ch+1] < MAX_CASSETTE_SIDE ; PROCESSLOG_SIDE[Ch+1]++ ) {
						if ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch+1] ][ Ch + 1 ] > 0 ) {
							PM_PROCESS_SIDE_ONE_BODY[ Ch ] = PROCESSLOG_SIDE[Ch+1];
							PM_PROCESS_MONITOR_ONE_BODY[ Ch ] = PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch+1] ][ Ch + 1 ];
							PM_PROCESS_POINTER_ONE_BODY[ Ch ] = PM_PROCESS_POINTER[ PROCESSLOG_SIDE[Ch+1] ][ Ch + 1 ]; // 2011.05.13
							return TRUE;
						}
					}
				}
			}
		}
	}
	//======================================================================================================================
	return FALSE;
}

void PROCESS_FILE_COPY_OPERATION( char *srcfilename , char *trgfilename ) {
	char BufferS[256];
	char BufferT[256];
	//
	CopyFile( srcfilename , trgfilename , TRUE );
	//
	sprintf( BufferS , "%s_ALL" , srcfilename );
	sprintf( BufferT , "%s_ALL" , trgfilename );
	//
	CopyFile( BufferS , BufferT , TRUE );
}

/*
// 2015.04.09
int PROCESS_MAKE_STEP_NAME( int ch , int step , char *data , int l ) { // 2013.04.17
	if ( PROCESSLOG_STEP_NAME[ch][step] != NULL ) free( PROCESSLOG_STEP_NAME[ch][step] );
	if ( ( data == NULL ) || ( l <= 0 ) ) {
		PROCESSLOG_STEP_NAME[ch][step] = NULL;
		return 0;
	}
	PROCESSLOG_STEP_NAME[ch][step] = calloc( l + 1 , sizeof( char ) );
	if ( PROCESSLOG_STEP_NAME[ch][step] == NULL ) {
		_IO_CIM_PRINTF( "================================================\n" );
		_IO_CIM_PRINTF( "Memory calloc Fail in PROCESS_MAKE_STEP_NAME <= NULL\n" );
		_IO_CIM_PRINTF( "================================================\n" );
		return -1;
	}
	//
	memcpy( PROCESSLOG_STEP_NAME[ch][step] , data , l );
	PROCESSLOG_STEP_NAME[ch][step][l] = 0;
	//
	return 1;
	//
}
*/

// 2015.04.09


int PROCESS_MAKE_STEP_NAME( int ch , int step , char *data , int l ) { // 2013.04.17
	if ( ( data == NULL ) || ( l <= 0 ) ) {
		if ( PROCESSLOG_STEP_NAME[ch][step] != NULL ) strcpy( PROCESSLOG_STEP_NAME[ch][step] , "" );
		return 0;
	}
	//
	if ( PROCESSLOG_STEP_NAME[ch][step] != NULL ) {
		//
		if ( (unsigned) l <= (_msize( PROCESSLOG_STEP_NAME[ch][step] )-1) ) {
			strncpy( PROCESSLOG_STEP_NAME[ch][step] , data , l );
			PROCESSLOG_STEP_NAME[ch][step][l] = 0;
			return 1;
		}
		//
		free( PROCESSLOG_STEP_NAME[ch][step] );
		//
	}
	//
	PROCESSLOG_STEP_NAME[ch][step] = calloc( l + 1 , sizeof( char ) );
	//
	if ( PROCESSLOG_STEP_NAME[ch][step] == NULL ) {
		_IO_CIM_PRINTF( "================================================\n" );
		_IO_CIM_PRINTF( "Memory calloc Fail in PROCESS_MAKE_STEP_NAME <= NULL\n" );
		_IO_CIM_PRINTF( "================================================\n" );
		return -1;
	}
	//
	strncpy( PROCESSLOG_STEP_NAME[ch][step] , data , l );
	PROCESSLOG_STEP_NAME[ch][step][l] = 0;
	//
	return 1;
	//
}


BOOL PROCESS_MONITOR_ONEBODY_REBUILD( int Ch ) {
	//======================================================================================================================
	// 2006.06.28
	//======================================================================================================================
	if ( PM_PROCESS_MONITOR_ONE_BODY[ Ch ] != 0 ) {
//		PROCESS_MONITOR_GET_FILENAME( TRUE , Ch + 1 , PM_PROCESS_SIDE_ONE_BODY[ Ch ] , PM_PROCESS_MONITOR_ONE_BODY[ Ch ] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , FALSE ); // 2007.06.04
//		PROCESS_MONITOR_GET_FILENAME( TRUE , Ch + 1 , PM_PROCESS_SIDE_ONE_BODY[ Ch ] , PM_PROCESS_MONITOR_ONE_BODY[ Ch ] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , FALSE , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13
//		PROCESS_MONITOR_GET_FILENAME( TRUE , Ch + 1 , PM_PROCESS_SIDE_ONE_BODY[ Ch ] , PM_PROCESS_POINTER_ONE_BODY[ Ch ] , PM_PROCESS_MONITOR_ONE_BODY[ Ch ] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , FALSE , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13 // 2012.11.29
//		if      ( PROCESSLOG_DOING[ Ch ] == 2 ) strcat( PROCESSLOG_IMSI[ Ch ] , "_PRE" ); // 2007.06.04
//		else if ( PROCESSLOG_DOING[ Ch ] == 3 ) strcat( PROCESSLOG_IMSI[ Ch ] , "_POST" ); // 2007.06.04

		if ( PROCESS_MONITOR_GET_FILENAME( 2 , Ch + 1 , PM_PROCESS_SIDE_ONE_BODY[ Ch ] , PM_PROCESS_POINTER_ONE_BODY[ Ch ] , PM_PROCESS_MONITOR_ONE_BODY[ Ch ] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , FALSE , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ) ) { // 2012.11.29
			PROCESS_FILE_COPY_OPERATION( PROCESSLOG_FILENAME[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
		}
		return TRUE;
	}
	//======================================================================================================================
	return FALSE;
}

void PROCESS_TIME_to_STR( char *SettimedataStr ) { // 2014.09.24
	time_t Settimedata;
	time( &Settimedata );
//	sprintf(SettimedataStr, "%ld", Settimedata); // 2019.03.08
	sprintf( SettimedataStr , "%ld" , (long) Settimedata ); // 2019.03.08
}


void PROCESS_DATA_WRITE( FILE *fpt , char *data , BOOL overtime ) { // 2019.01.18
	//
	int i;
	//
	SYSTEMTIME		SysTime;
	//
	GetLocalTime( &SysTime );
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		switch( GET_DATE_TIME_FORMAT() / 100 ) { // 2019.01.18
		case 1 : // %d y/m/d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			break;
		case 2 : // %0?d y/m/d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%02d:%02d:%02d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%04d/%02d/%02d %02d:%02d:%02d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			break;
		case 3 : // %d y-m-d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%d-%d-%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			break;
		case 4 : // %0?d y-m-d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%02d:%02d:%02d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%04d-%02d-%02d %02d:%02d:%02d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
			}
			break;
		case 5 : // %d y/m/d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			break;
		case 6 : // %0?d y/m/d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%02d:%02d:%02d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%04d/%02d/%02d %02d:%02d:%02d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			break;
		case 7 : // %d y-m-d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%d-%d-%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			break;
		case 8 : // %0?d y-m-d
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				f_enc_printf( fpt , "%02d:%02d:%02d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			else {
				f_enc_printf( fpt , "%04d-%02d-%02d %02d:%02d:%02d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
					SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
			}
			break;
		default :
			if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
				if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
					f_enc_printf( fpt , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
				}
				else {
					f_enc_printf( fpt , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
				}
			}
			else {
				if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
					f_enc_printf( fpt , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
						SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , data ); // 2006.10.02
				}
				else {
					f_enc_printf( fpt , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
						SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , data ); // 2006.10.02
				}
			}
			break;
		}
		//
		if ( !overtime ) break;
		//
	}
}

void PROCESS_MONITOR_LOG( int Ch ) {
	int dummy , Style;
//	int startstep; // 2016.11.02
//	SYSTEMTIME		SysTime; // 2006.10.02 // 2019.01.18

//	time_t Settimedata; // 2004.04.06
	char SettimedataStr[32]; // 2014.09.24
	BOOL PROCESSLOG_MONITOR_HAS_DATA; // 2017.06.29
	char MsgSltchar[16]; /* 2018.11.13 */

	//
	GUI_PROCESS_LOG_MONITOR2_SET( Ch , 1 , 0 , 0 , "" );
	//
	PROCESSLOG_MONITOR_IO[ Ch ] = _FIND_FROM_STRING( _K_F_IO , _i_SCH_PRM_GET_DFIX_PROCESS_LOG_FUNCTION_NAME( Ch - PM1 ) );
	if ( PROCESSLOG_MONITOR_IO[ Ch ] < 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) <= 0 ) { // 2002.02.27
			_endthread();
			return;
		}
	}
	PROCESSLOG_FILENAME[ Ch ]  = calloc( 128 + 1 , sizeof( char ) ); // 2004.04.06
	if ( PROCESSLOG_FILENAME[ Ch ] == NULL ) { // 2004.04.06
		_IO_CIM_PRINTF( "SCHEDULER Log Buffer Init Error 1 - Less Memory(PM%d)\n" , Ch - PM1 + 1 );
		_endthread();
		return;
	}
//	PROCESSLOG_IMSI[ Ch ]  = calloc( 128 + 1 , sizeof( char ) ); // 2007.11.07
	PROCESSLOG_IMSI[ Ch ]  = calloc( 256 + 1 , sizeof( char ) ); // 2007.11.07
	if ( PROCESSLOG_IMSI[ Ch ] == NULL ) {
		_IO_CIM_PRINTF( "SCHEDULER Log Buffer Init Error 2 - Less Memory(PM%d)\n" , Ch - PM1 + 1 );
		free( PROCESSLOG_FILENAME[ Ch ] );
		_endthread();
		return;
	}
	//
	if ( PROCESS_DATALOG_BUFFER_SIZE[ Ch ] < 512 ) PROCESS_DATALOG_BUFFER_SIZE[ Ch ] = 1024; // 2007.10.23
	//
	PROCESSLOG_READDATA[ Ch ]  = calloc( PROCESS_DATALOG_BUFFER_SIZE[ Ch ] + 1 , sizeof( char ) ); // 2007.10.23
	if ( PROCESSLOG_READDATA[ Ch ] == NULL ) {
		_IO_CIM_PRINTF( "SCHEDULER Log Buffer Init Error 3 - Less Memory(PM%d)\n" , Ch - PM1 + 1 );
		free( PROCESSLOG_FILENAME[ Ch ] );
		free( PROCESSLOG_IMSI[ Ch ] );
		_endthread();
		return;
	}
	//
	for ( PROCESSLOG_I[Ch] = 0 ; PROCESSLOG_I[Ch] < MAX_PM_LOG_STEP_COUNT ; PROCESSLOG_I[Ch]++ ) PROCESSLOG_STEP_NAME[ Ch ][ PROCESSLOG_I[Ch] ] = NULL; // 2013.04.17
	//
	if ( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) > 0 ) {
		while ( TRUE ) {
			Sleep(500);
			//
			if ( 
				( _i_SCH_MODULE_Get_PM_WAFER( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) , 0 ) <= 0 ) &&
				( PM_INTERFACE_MONITOR_COUNT[Ch] > 0 ) ) {
				if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Ch) || ( PROCESSLOG_MONITOR_IO[ Ch ] < 0 ) ) {
					GUI_PROCESS_LOG_MONITOR2_SET( Ch , 4 , MAX_CASSETTE_SIDE , 0 , "{Inf-L}" );
					PROCESS_INTERFACE_MODULE_RECOVER_DATA( Ch , FALSE , "" , &dummy );
				}
				else {
					GUI_PROCESS_LOG_MONITOR2_SET( Ch , 4 , MAX_CASSETTE_SIDE , 0 , "{Inf-R}" );
					_dREAD_UPLOAD_MESSAGE( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] );
					if ( strlen( PROCESSLOG_READDATA[ Ch ] ) > 0 ) {
						PROCESS_INTERFACE_MODULE_RECOVER_DATA( Ch , TRUE , PROCESSLOG_READDATA[ Ch ] , &dummy );
					}
				}
			}
			else {
				//
				PROCESS_INTERFACE_MODULE_TIMER_RESET( _i_SCH_PRM_GET_MODULE_INTERFACE( Ch ) ); // 2018.06.07
				//
				GUI_PROCESS_LOG_MONITOR2_SET( Ch , 4 , MAX_CASSETTE_SIDE , 0 , "{Inf-W}" );
			}
			//
		}
	}
	else {
		GUI_PROCESS_LOG_MONITOR2_SET( Ch , 2 , 0 , 0 , "" );
		//
		while ( TRUE ) {
			//
			EnterCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
			//
			for ( PROCESSLOG_SIDE[Ch] = 0 ; PROCESSLOG_SIDE[Ch] < MAX_CASSETTE_SIDE ; PROCESSLOG_SIDE[Ch]++ ) {
				//====================================================================================================
				// 2006.02.22
				//====================================================================================================
				//PROCESSLOG_CH0[ Ch ] = PROCESS_MONITOR_Get_Status( PROCESSLOG_SIDE[Ch] , Ch ); // 2006.02.22
				//====================================================================================================
				PROCESSLOG_DUMMY[ Ch ]   = PM_PROCESS_DUMMY[ PROCESSLOG_SIDE[Ch] ][ Ch ];
				PROCESSLOG_FNOTUSE[ Ch ] = PM_PROCESS_FNOTUSE[ PROCESSLOG_SIDE[Ch] ][ Ch ]; // 2007.07.25
				PROCESSLOG_OPTION[ Ch ]  = PM_PROCESS_OPTION[ PROCESSLOG_SIDE[Ch] ][ Ch ]; // 2007.07.25
				PROCESSLOG_WFRNOTCHECK[ Ch ] = PM_PROCESS_WFRNOTCHECK[ PROCESSLOG_SIDE[Ch] ][ Ch ]; // 2007.10.11
				//====================================================================================================
				for ( PROCESSLOG_XP[Ch] = 0 ; PROCESSLOG_XP[Ch] < MAX_PM_SLOT_DEPTH ; PROCESSLOG_XP[Ch]++ ) {
					PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]]    = PM_PROCESS_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]];
					PROCESSLOG_POINTER_EX[ Ch ][PROCESSLOG_XP[Ch]] = PM_PROCESS_POINTER_EX[ Ch ][PROCESSLOG_XP[Ch]];
					PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]]   = PM_PROCESS_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]];
					PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]]   = PM_PROCESS_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]];
				}
				//====================================================================================================
				Style = -1; // 2012.04.12
				//
				if ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] <= 0 ) {
					PROCESSLOG_DOING[ Ch ] = 0;
				}
				else if ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] < PROCESS_INDICATOR_PRE ) {
					//
					Style = 0; // 2012.04.12
					//
					if ( PROCESSLOG_WFRNOTCHECK[ Ch ] ) { // 2007.10.11
						PROCESSLOG_DOING[ Ch ] = 1;
						PROCESSLOG_CH0[ Ch ] = PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ];
					}
					else {
						if ( _i_SCH_MODULE_Get_PM_WAFER( Ch , 0 ) <= 0 ) { // 2007.09.03
							if ( PROCESSLOG_FNOTUSE[ Ch ] ) { // 2012.12.05
								PROCESSLOG_DOING[ Ch ] = 1;
								PROCESSLOG_CH0[ Ch ] = PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ];
							}
							else {
								PROCESSLOG_DOING[ Ch ] = 0; // 2007.09.03
							}
						} // 2007.09.03
						else {
							PROCESSLOG_DOING[ Ch ] = 1;
							PROCESSLOG_CH0[ Ch ] = PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ];
						}
					}
				}
				//====================================================================================================
				else if ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] == PROCESS_INDICATOR_PRE ) { // 2007.06.04
					if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 5 ) {
						if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
							PROCESSLOG_DOING[ Ch ] = 4;
							PROCESSLOG_CH0[ Ch ] = 98;
						}
						else {
							PROCESSLOG_DOING[ Ch ] = 0;
						}
					}
					else {
						PROCESSLOG_DOING[ Ch ] = 0;
					}
				}
				//====================================================================================================
				else if ( ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] > PROCESS_INDICATOR_PRE ) && ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] < PROCESS_INDICATOR_POST ) ) {
					//
					Style = 2; // 2012.04.12
					//
					if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 5 ) {
						if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
							PROCESSLOG_DOING[ Ch ] = 2;
							PROCESSLOG_CH0[ Ch ] = PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] - PROCESS_INDICATOR_PRE;
						}
						else {
							PROCESSLOG_DOING[ Ch ] = 0;
						}
					}
					else {
						PROCESSLOG_DOING[ Ch ] = 0;
					}
				}
				//====================================================================================================
				else if ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] == PROCESS_INDICATOR_POST ) { // 2007.06.04
					if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 5 ) {
						if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) {
							PROCESSLOG_DOING[ Ch ] = 5;
							PROCESSLOG_CH0[ Ch ] = 99;
						}
						else {
							PROCESSLOG_DOING[ Ch ] = 0;
						}
					}
					else {
						PROCESSLOG_DOING[ Ch ] = 0;
					}
				}
				//====================================================================================================
				else if ( PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] > PROCESS_INDICATOR_POST ) {
					//
					Style = 1; // 2012.04.12
					//
					if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 5 ) {
						if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) {
							PROCESSLOG_DOING[ Ch ] = 3;
							PROCESSLOG_CH0[ Ch ] = PM_PROCESS_MONITOR[ PROCESSLOG_SIDE[Ch] ][ Ch ] - PROCESS_INDICATOR_POST;
						}
						else {
							PROCESSLOG_DOING[ Ch ] = 0;
						}
					}
					else {
						PROCESSLOG_DOING[ Ch ] = 0;
					}
				}
				//====================================================================================================
				else { // 2007.06.04
					PROCESSLOG_DOING[ Ch ] = 0;
				}
				//====================================================================================================
				if ( ( IO_LOT_LOG_STATUS(PROCESSLOG_SIDE[Ch]) > 0 ) && ( PROCESSLOG_DOING[ Ch ] > 0 ) ) { // 2005.09.13 // 2006.02.22
					//======================================================================================================================
					// 2007.06.25
					//======================================================================================================================
					PM_PROCESS_MONITOR_FINISH_CONFIRM[ Ch ] = FALSE;
					//======================================================================================================================
					// 2006.06.28
					//======================================================================================================================
					PM_PROCESS_MONITOR_ONE_BODY[ Ch ] = 0; // 2006.06.28
					PM_PROCESS_SIDE_ONE_BODY[ Ch ] = 0; // 2006.06.28
					PM_PROCESS_POINTER_ONE_BODY[ Ch ] = 0; // 2011.05.13
					//======================================================================================================================
//					for ( PROCESSLOG_I[Ch] = 0 ; PROCESSLOG_I[Ch] < MAX_PM_LOG_STEP_COUNT ; PROCESSLOG_I[Ch]++ ) strcpy( PROCESSLOG_STEP_NAME[ Ch ][ PROCESSLOG_I[Ch] ] , "" ); // 2013.04.17
					for ( PROCESSLOG_I[Ch] = 0 ; PROCESSLOG_I[Ch] < MAX_PM_LOG_STEP_COUNT ; PROCESSLOG_I[Ch]++ ) PROCESS_MAKE_STEP_NAME( Ch , PROCESSLOG_I[Ch] , NULL , 0 ); // 2013.04.17
					//
//					PROCESSLOG_STEP_LAST_READ[ Ch ] = 0; // 2013.07.18
					PROCESSLOG_STEP_LAST_READ[ Ch ] = -1;
					PROCESSLOG_STEP_CURRENT_READ[ Ch ] = 0;
//					PROCESSLOG_E[ Ch ] = PROCESSLOG_CH0[ Ch ] / 10000; // 2004.02.27 // 2007.11.07
//					PROCESSLOG_I[ Ch ] = PROCESSLOG_CH0[ Ch ] % 100; // 2007.11.07
//					PROCESSLOG_J[ Ch ] = ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100; // 2004.02.27 // 2007.11.07
					//
					GUI_PROCESS_LOG_MONITOR2_SET( Ch , 3 , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESS_MONITOR_Get_Recipe( PROCESSLOG_SIDE[Ch] , Ch ) );
					//
					//---------------------------------------------------------------------------------
					if ( !PROCESS_MONITORING_RECIPE_DATA_SAVE_SKIP ) { // 2004.04.27
						//======================================================================================
						if ( !PROCESSLOG_FNOTUSE[ Ch ] ) { // 2007.07.25
//							PROCESS_MONITOR_GET_FILENAME( FALSE , Ch , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] % 100 , ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100 , PROCESSLOG_CH0[ Ch ] / 10000 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY[ Ch ] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13
//							PROCESS_MONITOR_GET_FILENAME( FALSE , Ch , PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , PROCESSLOG_CH0[ Ch ] % 100 , ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100 , PROCESSLOG_CH0[ Ch ] / 10000 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY[ Ch ] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13 // 2012.11.29
							//
							if ( PROCESS_MONITOR_GET_FILENAME( 0 , Ch , PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , PROCESSLOG_CH0[ Ch ] % 100 , ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100 , PROCESSLOG_CH0[ Ch ] / 10000 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY[ Ch ] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ) ) { // 2012.11.29
//								if ( RECIPE_FILE_PROCESS_DATA_READ_And_File_Save( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , PROCESSLOG_SIDE[Ch] , Ch , PROCESS_MONITOR_Get_Recipe( PROCESSLOG_SIDE[Ch] , Ch ) , Style , PROCESSLOG_CH0[ Ch ] % 100 , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , SCHMULTI_TUNE_INFO_DATA_GET( Ch ) ) ) {} // 2014.02.08
								if ( RECIPE_FILE_PROCESS_DATA_READ_And_File_Save( SCHEDULER_GET_RECIPE_LOCKING( PROCESSLOG_SIDE[Ch] ) , PROCESSLOG_SIDE[Ch] , Ch , PROCESS_MONITOR_Get_Recipe( PROCESSLOG_SIDE[Ch] , Ch ) , Style , PROCESSLOG_CH0[ Ch ] % 100 , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , SCHMULTI_TUNE_INFO_DATA_GET( Ch ) ) ) {} // 2014.02.08
							}
						}
						//====================================================================================================
						// 2007.05.02
						//====================================================================================================
						for ( PROCESSLOG_XP[Ch] = 0 ; PROCESSLOG_XP[Ch] < MAX_PM_SLOT_DEPTH ; PROCESSLOG_XP[Ch]++ ) {
							if ( PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] > 0 ) {
								//
								if ( PROCESSLOG_OPTION[ Ch ] != 3 ) { // 2013.03.11
									if ( ( PROCESSLOG_XP[Ch] == 0 ) && ( ( PROCESSLOG_CH0[ Ch ] / 100 ) > 0 ) ) continue; // 2012.06.16
								}
								//
//								PROCESS_MONITOR_GET_FILENAME( FALSE , Ch , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13
//								PROCESS_MONITOR_GET_FILENAME( FALSE , Ch , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_POINTER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13 // 2012.11.29
								//
								if ( PROCESS_MONITOR_GET_FILENAME( 0 , Ch , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_POINTER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ) ) { // 2012.11.29
//									if ( RECIPE_FILE_PROCESS_DATA_READ_And_File_Save( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , Ch , PROCESS_MONITOR_Get_Recipe( PROCESSLOG_SIDE[Ch] , Ch ) , Style , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_IMSI[ Ch ] , SCHMULTI_TUNE_INFO_DATA_GET( Ch ) ) ) {} // 2014.02.08
									if ( RECIPE_FILE_PROCESS_DATA_READ_And_File_Save( SCHEDULER_GET_RECIPE_LOCKING( PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] ) , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , Ch , PROCESS_MONITOR_Get_Recipe( PROCESSLOG_SIDE[Ch] , Ch ) , Style , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_IMSI[ Ch ] , SCHMULTI_TUNE_INFO_DATA_GET( Ch ) ) ) {} // 2014.02.08
								}
								//
							}
						}
						//====================================================================================================
					}
					//---------------------------------------------------------------------------------
					for ( PROCESSLOG_XP[ Ch ] = 0 ; PROCESSLOG_XP[ Ch ] < 3 ; PROCESSLOG_XP[ Ch ]++ ) {
//						PROCESS_MONITOR_GET_FILENAME( TRUE , Ch , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] % 100 , ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100 , PROCESSLOG_CH0[ Ch ] / 10000 , PROCESSLOG_FILENAME[ Ch ] , PROCESSLOG_DUMMY[ Ch ] , PROCESSLOG_DOING[ Ch ] , PROCESSLOG_FNOTUSE[ Ch ] , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13
//						PROCESS_MONITOR_GET_FILENAME( TRUE , Ch , PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , PROCESSLOG_CH0[ Ch ] % 100 , ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100 , PROCESSLOG_CH0[ Ch ] / 10000 , PROCESSLOG_FILENAME[ Ch ] , PROCESSLOG_DUMMY[ Ch ] , PROCESSLOG_DOING[ Ch ] , PROCESSLOG_FNOTUSE[ Ch ] , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13 // 2012.11.29
						//
						if ( !PROCESS_MONITOR_GET_FILENAME( 1 , Ch , PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , PROCESSLOG_CH0[ Ch ] % 100 , ( PROCESSLOG_CH0[ Ch ] % 10000 ) / 100 , PROCESSLOG_CH0[ Ch ] / 10000 , PROCESSLOG_FILENAME[ Ch ] , PROCESSLOG_DUMMY[ Ch ] , PROCESSLOG_DOING[ Ch ] , PROCESSLOG_FNOTUSE[ Ch ] , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ) ) { // 2012.11.29
							PROCESSLOG_FPT[Ch] = NULL;
							break;
						}
						//======================================================================================
//						if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) { // 2005.02.16 // 2007.11.28
						if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) >= 1 ) { // 2005.02.16 // 2007.11.28
							break;
						}
						else {
							if ( !SYSTEM_LOGSKIP_STATUS() ) { // 2004.05.21
								PROCESSLOG_FPT[Ch] = fopen( PROCESSLOG_FILENAME[ Ch ] , "a" );
								if ( PROCESSLOG_FPT[Ch] != NULL ) break;
							}
							else { // 2004.05.21
								PROCESSLOG_FPT[Ch] = NULL;
								break;
							}
						}
					}
					//---------------------------------------------------------------------------------
//					if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) { // 2005.02.16 // 2007.11.27
					if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) >= 1 ) { // 2005.02.16 // 2007.11.27
						_dRUN_FUNCTION_ABORT( PROCESSLOG_MONITOR_IO[ Ch ] );
//						if ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() && SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 128 , TRUE ) ) { // 2007.11.07
						if (
//							PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() && // 2019.04.05
							( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() % 2 ) == 1 ) && // 2019.04.05
/*
// 2014.02.28
							SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) ,
								( PROCESSLOG_WAFER_EX[ Ch ][0] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][0] : -1 , PROCESSLOG_POINTER_EX[ Ch ][0] ,
								( PROCESSLOG_WAFER_EX[ Ch ][1] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][1] : -1 , PROCESSLOG_POINTER_EX[ Ch ][1] ,
								( PROCESSLOG_WAFER_EX[ Ch ][2] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][2] : -1 , PROCESSLOG_POINTER_EX[ Ch ][2] ,
								( PROCESSLOG_WAFER_EX[ Ch ][3] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][3] : -1 , PROCESSLOG_POINTER_EX[ Ch ][3] ,
								( PROCESSLOG_WAFER_EX[ Ch ][4] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][4] : -1 , PROCESSLOG_POINTER_EX[ Ch ][4] ,
								PROCESSLOG_IMSI[ Ch ] , 255 , TRUE )
*/
// 2014.02.28
							SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) ,
//								1 , PROCESSLOG_WAFER_EX[ Ch ] , PROCESSLOG_SIDE_EX[ Ch ] , PROCESSLOG_POINTER_EX[ Ch ] , PROCESSLOG_IMSI[ Ch ] , 255 , TRUE ) // 2014.09.03
								Ch , 0 , PROCESSLOG_WAFER_EX[ Ch ] , PROCESSLOG_SIDE_EX[ Ch ] , PROCESSLOG_POINTER_EX[ Ch ] , PROCESSLOG_IMSI[ Ch ] , 255 , TRUE ) // 2014.09.03
							) { // 2007.11.07

							//
							/*
							//
							//
							//
							// 2019.01.18
							//

							if ( _i_SCH_PRM_GET_LOG_ENCRIPTION() == 0 ) { // 2012.06.11
								if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) == 2 ) { // 2007.11.27
									//
									//time( &Settimedata ); // 2007.11.27 // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE_F %d|%d|%s|%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() ); // 2007.11.27
								}
								else {
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM_AT_MODULE %d|%d|%s|%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY_AT_MODULE %d|%d|%s|%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE %d|%d|%s|%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								}
							}
							else { // 2012.06.11
								if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) == 2 ) { // 2007.11.27
									//
									//time( &Settimedata ); // 2007.11.27 // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE_F %d|%d|%s|%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() ); // 2007.11.27
								}
								else {
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM_AT_MODULE %d|%d|%s|%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY_AT_MODULE %d|%d|%s|%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE %d|%d|%s|%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								}
							}
							//
							*/
							//
							//
							// 2019.01.18
							//
							//
							if ( _i_SCH_PRM_GET_LOG_ENCRIPTION() == 0 ) { // 2012.06.11
								strcpy( MsgSltchar , "" );
							}
							else {
								strcpy( MsgSltchar , ":1" );
							}
							//
							switch( GET_DATE_TIME_FORMAT() / 100 ) { // 2019.01.18
							case 1 :	strcat( MsgSltchar , ":A" );	break;
							case 2 :	strcat( MsgSltchar , ":B" );	break;
							case 3 :	strcat( MsgSltchar , ":C" );	break;
							case 4 :	strcat( MsgSltchar , ":D" );	break;
							case 5 :	strcat( MsgSltchar , ":E" );	break;
							case 6 :	strcat( MsgSltchar , ":F" );	break;
							case 7 :	strcat( MsgSltchar , ":G" );	break;
							case 8 :	strcat( MsgSltchar , ":H" );	break;
							case 9 :	strcat( MsgSltchar , ":I" );	break;
							}
							//
							if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) == 2 ) { // 2007.11.27
								//
								//time( &Settimedata ); // 2007.11.27 // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE_F %d|%d|%s|%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() ); // 2007.11.27
							}
							else {
								//
								//time( &Settimedata ); // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM_AT_MODULE %d|%d|%s|%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								//
								//time( &Settimedata ); // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY_AT_MODULE %d|%d|%s|%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								//
								//time( &Settimedata ); // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE %d|%d|%s|%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
							}
							//
						}
						else {
							//
							/*
							//
							//
							//
							// 2019.01.18
							//
							if ( _i_SCH_PRM_GET_LOG_ENCRIPTION() == 0 ) { // 2012.06.11
								if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) == 2 ) { // 2007.12.15
									//
									//time( &Settimedata ); // 2007.11.27 // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE_F %d|%d||||%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								}
								else {
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM_AT_MODULE %d|%d||||%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY_AT_MODULE %d|%d||||%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE %d|%d||||%s|%d|%d|%d|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								}
							}
							else { // 2012.06.11
								if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) == 2 ) { // 2007.12.15
									//
									//time( &Settimedata ); // 2007.11.27 // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE_F %d|%d||||%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								}
								else {
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM_AT_MODULE %d|%d||||%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY_AT_MODULE %d|%d||||%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
									//
									//time( &Settimedata ); // 2014.09.24
									PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
									//
									_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE %d|%d||||%s|%d|%d|%d:1|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								}
							}
							//
							*/
							//
							//
							//
							// 2019.01.18
							//
							//
							if ( _i_SCH_PRM_GET_LOG_ENCRIPTION() == 0 ) { // 2012.06.11
								strcpy( MsgSltchar , "" );
							}
							else {
								strcpy( MsgSltchar , ":1" );
							}
							//
							switch( GET_DATE_TIME_FORMAT() / 100 ) { // 2019.01.18
							case 1 :	strcat( MsgSltchar , ":A" );	break;
							case 2 :	strcat( MsgSltchar , ":B" );	break;
							case 3 :	strcat( MsgSltchar , ":C" );	break;
							case 4 :	strcat( MsgSltchar , ":D" );	break;
							case 5 :	strcat( MsgSltchar , ":E" );	break;
							case 6 :	strcat( MsgSltchar , ":F" );	break;
							case 7 :	strcat( MsgSltchar , ":G" );	break;
							case 8 :	strcat( MsgSltchar , ":H" );	break;
							case 9 :	strcat( MsgSltchar , ":I" );	break;
							}
							//
							if ( ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() / 2 ) == 2 ) { // 2007.12.15
								//
								//time( &Settimedata ); // 2007.11.27 // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE_F %d|%d||||%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
							}
							else {
								//
								//time( &Settimedata ); // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM_AT_MODULE %d|%d||||%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								//
								//time( &Settimedata ); // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( FALSE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY_AT_MODULE %d|%d||||%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
								//
								//time( &Settimedata ); // 2014.09.24
								PROCESS_TIME_to_STR( SettimedataStr ); // 2014.09.24
								//
								_sch_dRUN_FUNCTIONF( TRUE , PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_AT_MODULE %d|%d||||%s|%d|%d|%d%s|%s|%d|%s|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , PROCESS_MONITORING_TIME , _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() , PROCESS_MONITORING_TIME_MINIMUM_LOG , MsgSltchar , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , Ch - PM1 + 1 , SettimedataStr , PROGRAM_FUNCTION_READ() );
							}
							//
						}
						//
						PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] = -1; // 2011.04.18
						//
						while ( ( PROCESS_MONITOR_Get_Status( PROCESSLOG_SIDE[Ch] , Ch ) > 0 ) && ( !PM_PROCESS_MONITOR_FINISH_CONFIRM[ Ch ] ) ) { // 2005.09.13 // 2006.03.09 // 2007.06.25
							//
							GUI_PROCESS_LOG_MONITOR2_RUN( Ch );
							//======================================================================================================================
							if ( !Process_Monitor_Sts_Find[PROCESSLOG_SIDE[Ch]] ) Process_Monitor_Sts_Find[PROCESSLOG_SIDE[Ch]] = TRUE;
							//======================================================================================================================
							// 2006.06.28
							//======================================================================================================================
							PROCESS_MONITOR_ONEBODY_CHECK( Ch );
							//======================================================================================================================
							//
							LeaveCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
							//
//							Sleep( PROCESS_MONITORING_TIME ); // 2007.11.27
							Sleep( 1 ); // 2007.11.27
							//======================================================================================================================
							//
							EnterCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
							//
							if ( PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] != -1 ) { // 2011.04.18
								PROCESSLOG_SIDE[Ch] = PROCESSLOG_MONITOR_SIDE_CHANGE[Ch];
								PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] = -1;
							}
							//
						}
						//
						_dRUN_FUNCTION_ABORT( PROCESSLOG_MONITOR_IO[ Ch ] );
					}
					//---------------------------------------------------------------------------------
					else {
						if ( PROCESSLOG_FPT[Ch] != NULL ) {
							_dRUN_FUNCTION_ABORT( PROCESSLOG_MONITOR_IO[ Ch ] );
//							if ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() && SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 128 , FALSE ) ) { // 2002.05.20 // 2002.06.28 // 2007.11.07
							if (
//								PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() && // 2019.04.05
								( ( PROCESS_MONITOR_Get_MONITORING_WITH_CPJOB() % 2 ) == 1 ) && // 2019.04.05
/*
// 2014.02.28
								SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) ,
									( PROCESSLOG_WAFER_EX[ Ch ][0] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][0] : -1 , PROCESSLOG_POINTER_EX[ Ch ][0] ,
									( PROCESSLOG_WAFER_EX[ Ch ][1] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][1] : -1 , PROCESSLOG_POINTER_EX[ Ch ][1] ,
									( PROCESSLOG_WAFER_EX[ Ch ][2] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][2] : -1 , PROCESSLOG_POINTER_EX[ Ch ][2] ,
									( PROCESSLOG_WAFER_EX[ Ch ][3] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][3] : -1 , PROCESSLOG_POINTER_EX[ Ch ][3] ,
									( PROCESSLOG_WAFER_EX[ Ch ][4] > 0 ) ? PROCESSLOG_SIDE_EX[ Ch ][4] : -1 , PROCESSLOG_POINTER_EX[ Ch ][4] ,
									PROCESSLOG_IMSI[ Ch ] , 255 , FALSE )
*/
// 2014.02.28
								SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) ,
//									1 , PROCESSLOG_WAFER_EX[ Ch ] , PROCESSLOG_SIDE_EX[ Ch ] , PROCESSLOG_POINTER_EX[ Ch ] , PROCESSLOG_IMSI[ Ch ] , 255 , FALSE ) // 2014.09.03
									Ch , 0 , PROCESSLOG_WAFER_EX[ Ch ] , PROCESSLOG_SIDE_EX[ Ch ] , PROCESSLOG_POINTER_EX[ Ch ] , PROCESSLOG_IMSI[ Ch ] , 255 , FALSE ) // 2014.09.03
								) { // 2002.05.20 // 2002.06.28 // 2007.11.07
								if ( _dRUN_FUNCTIONF( PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM %d|%d|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] ) == SYS_SUCCESS ) {
//									_dREAD_UPLOAD_MESSAGE( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] ); // 2013.05.07
									_dREAD_UPLOAD_MESSAGE2( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] - 1 ); // 2013.05.07
//									if ( strlen( PROCESSLOG_READDATA[ Ch ] ) > 0 ) { // 2007.10.24
									PROCESSLOG_I[ Ch ] = strlen( PROCESSLOG_READDATA[ Ch ] ); // 2007.10.24
									if ( PROCESSLOG_I[ Ch ] > 0 ) { // 2007.10.24
										if ( PROCESSLOG_I[ Ch ] > PROCESS_DATALOG_BUFFER_SIZE[ Ch ] ) { // 2007.10.24
											_IO_CIM_PRINTF( "SCHEDULER Will be terminated with Data Overflow 1 in (PM%d)[ReadCount=%d:BufferSize=%d]\n" , Ch - PM1 + 1 , PROCESSLOG_I[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] );
											_IO_CIM_PRINTF( "SCHEDULER Will be terminated with Data Overflow 1 in (PM%d)[ReadCount=%d:BufferSize=%d]\n" , Ch - PM1 + 1 , PROCESSLOG_I[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] );
										}

										/*
										//
										GetLocalTime( &SysTime ); // 2006.10.02
										if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
											else {
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
										}
										else {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
													SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
											else {
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
													SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
										}
										*/
										//
										PROCESS_DATA_WRITE( PROCESSLOG_FPT[Ch] , PROCESSLOG_READDATA[ Ch ] , FALSE ); // 2019.01.18
										//
									}
								}
								_dRUN_FUNCTIONF( PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY %d|%d|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
								_dRUN_SET_FUNCTIONF( PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL %d|%d|%s" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
							}
							else {
								if ( _dRUN_FUNCTIONF( PROCESSLOG_MONITOR_IO[ Ch ] , "ITEM %d|%d" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] ) == SYS_SUCCESS ) {
//									_dREAD_UPLOAD_MESSAGE( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] ); // 2013.05.07
									_dREAD_UPLOAD_MESSAGE2( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] - 1 ); // 2013.05.07
//									if ( strlen( PROCESSLOG_READDATA[ Ch ] ) > 0 ) { // 2007.10.24
									PROCESSLOG_I[ Ch ] = strlen( PROCESSLOG_READDATA[ Ch ] ); // 2007.10.24
									if ( PROCESSLOG_I[ Ch ] > 0 ) { // 2007.10.24
										if ( PROCESSLOG_I[ Ch ] > PROCESS_DATALOG_BUFFER_SIZE[ Ch ] ) { // 2007.10.24
											_IO_CIM_PRINTF( "SCHEDULER Will be terminated with Data Overflow 2 in (PM%d)[ReadCount=%d:BufferSize=%d]\n" , Ch - PM1 + 1 , PROCESSLOG_I[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] );
											_IO_CIM_PRINTF( "SCHEDULER Will be terminated with Data Overflow 2 in (PM%d)[ReadCount=%d:BufferSize=%d]\n" , Ch - PM1 + 1 , PROCESSLOG_I[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] );
										}
										//
										/*
										//
										GetLocalTime( &SysTime ); // 2006.10.02
										if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
											else {
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
										}
										else {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
													SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.SysTime
											}
											else {
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
													SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.SysTime
											}
										}
										//
										*/
										//
										//
										PROCESS_DATA_WRITE( PROCESSLOG_FPT[Ch] , PROCESSLOG_READDATA[ Ch ] , FALSE ); // 2019.01.18
										//
									}
								}
								_dRUN_FUNCTIONF( PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL_READY %d|%d" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] );
								_dRUN_SET_FUNCTIONF( PROCESSLOG_MONITOR_IO[ Ch ] , "DATAPOLL %d|%d" , PROCESSLOG_SIDE[Ch] , PROCESSLOG_CH0[ Ch ] );
							}
						}
						//
						PROCESSLOG_CLOCK_OVERTIME[Ch] = 0;
						PROCESSLOG_CLOCK_START[Ch] = GetTickCount(); // 2003.10.09
						//
						PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] = -1; // 2011.04.18
						//
						PROCESSLOG_MONITOR_HAS_DATA = FALSE; // 2017.06.29
						//
						while ( ( PROCESS_MONITOR_Get_Status( PROCESSLOG_SIDE[Ch] , Ch ) > 0 ) && ( !PM_PROCESS_MONITOR_FINISH_CONFIRM[ Ch ] ) ) { // 2005.09.13 // 2006.03.09 // 2007.06.25
							//======================================================================================================================
							// 2006.06.28
							//======================================================================================================================
							PROCESS_MONITOR_ONEBODY_CHECK( Ch );
							//======================================================================================================================
							GUI_PROCESS_LOG_MONITOR2_RUN( Ch );
							//======================================================================================================================
							if ( !Process_Monitor_Sts_Find[PROCESSLOG_SIDE[Ch]] ) Process_Monitor_Sts_Find[PROCESSLOG_SIDE[Ch]] = TRUE; // 2003.11.05
							//======================================================================================================================
							if ( PROCESSLOG_FPT[Ch] != NULL ) {
//								_dREAD_UPLOAD_MESSAGE( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] ); // 2013.05.07
								_dREAD_UPLOAD_MESSAGE2( PROCESSLOG_MONITOR_IO[ Ch ] , PROCESSLOG_READDATA[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] - 1 ); // 2013.05.07
								PROCESSLOG_I[Ch] = strlen( PROCESSLOG_READDATA[ Ch ] );
								if ( PROCESSLOG_I[Ch] > 0 ) {
									//
									PROCESSLOG_MONITOR_HAS_DATA = TRUE; // 2017.06.29
									//
									if ( PROCESSLOG_I[ Ch ] > PROCESS_DATALOG_BUFFER_SIZE[ Ch ] ) { // 2007.10.24
										_IO_CIM_PRINTF( "SCHEDULER Will be terminated with Data Overflow 3 in (PM%d)[ReadCount=%d:BufferSize=%d]\n" , Ch - PM1 + 1 , PROCESSLOG_I[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] );
										_IO_CIM_PRINTF( "SCHEDULER Will be terminated with Data Overflow 3 in (PM%d)[ReadCount=%d:BufferSize=%d]\n" , Ch - PM1 + 1 , PROCESSLOG_I[ Ch ] , PROCESS_DATALOG_BUFFER_SIZE[ Ch ] );
									}
									//
//		STEP_CHANGE_EVENT	LOG
//
//	&	RECV_ONLY			RECV_ONLY		0
//	$	AUTOMAP				AUTOMAP			1
//	%	AUTOMAP				RECV_ONLY		2	X
//	@	RECV_ONLY			AUTOMAP			3	X

//									if ( ( PROCESSLOG_I[Ch] > 8 ) && ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) ) { // 2013.06.04
//									if ( ( PROCESSLOG_I[Ch] > 8 ) && ( ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '&' ) ) ) { // 2013.06.04 // 2016.01.22
//									if ( ( PROCESSLOG_I[Ch] > 8 ) && ( ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '&' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '%' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '@' ) ) ) { // 2013.06.04 // 2016.01.22 // 2016.03.10
									if ( ( PROCESSLOG_I[Ch] > 8 ) && ( ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '&' ) ) ) { // 2013.06.04 // 2016.01.22 // 2016.03.10
										//
										if ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) { // 2013.06.04
//											PROCESSLOG_STEPAUTOREMAP[ Ch ] = TRUE; // 2016.01.22
											PROCESSLOG_STEPAUTOREMAP[ Ch ] = 1; // 2016.01.22
											//
//											if ( PROCESSLOG_STEP_LAST_READ[ Ch ] == -1 ) PROCESSLOG_STEP_LAST_READ[ Ch ] = 0; // 2013.07.18 // 2016.01.22
											//
										}
//										else if ( PROCESSLOG_READDATA[ Ch ][0] == '%' ) { // 2016.01.22 // 2016.03.10
//											PROCESSLOG_STEPAUTOREMAP[ Ch ] = 2; // 2016.03.10
//										} // 2016.03.10
//										else if ( PROCESSLOG_READDATA[ Ch ][0] == '@' ) { // 2016.01.22 // 2016.03.10
//											PROCESSLOG_STEPAUTOREMAP[ Ch ] = 3; // 2016.03.10
//										} // 2016.03.10
										else {
//											PROCESSLOG_STEPAUTOREMAP[ Ch ] = FALSE; // 2016.01.22
											PROCESSLOG_STEPAUTOREMAP[ Ch ] = 0; // 2016.01.22
										}
										//
										memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 1 , 3 );
										PROCESSLOG_IMSI[ Ch ][3] = 0;
										PROCESSLOG_STEP_CURRENT_READ[ Ch ] = atoi( PROCESSLOG_IMSI[ Ch ] );
										//
										if ( PROCESSLOG_STEP_LAST_READ[ Ch ] != PROCESSLOG_STEP_CURRENT_READ[ Ch ] ) {
											memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 4 , 2 );
											PROCESSLOG_IMSI[ Ch ][2] = 0;
											PROCESSLOG_I[Ch] = atoi( PROCESSLOG_IMSI[ Ch ] );
											//
											if ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] < MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
//												memcpy( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ]] , PROCESSLOG_READDATA[ Ch ] + 6 , PROCESSLOG_I[Ch] ); // 2013.04.17
//												PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ]][PROCESSLOG_I[Ch]] = 0; // 2013.04.17
												PROCESS_MAKE_STEP_NAME( Ch , PROCESSLOG_STEP_CURRENT_READ[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 6 , PROCESSLOG_I[Ch] ); // 2013.04.17
											}
											//
											memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 6 + PROCESSLOG_I[Ch] , 2 );
											PROCESSLOG_IMSI[ Ch ][2] = 0;
											PROCESSLOG_J[Ch] = atoi( PROCESSLOG_IMSI[ Ch ] );
											//
											if ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] > 0 ) { // 2013.07.18
												if ( ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1 ) < MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
	//												memcpy( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1] , PROCESSLOG_READDATA[ Ch ] + 8 + PROCESSLOG_I[Ch] , PROCESSLOG_J[Ch] ); // 2013.04.17
	//												PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1][PROCESSLOG_J[Ch]] = 0; // 2013.04.17
													PROCESS_MAKE_STEP_NAME( Ch , PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1 , PROCESSLOG_READDATA[ Ch ] + 8 + PROCESSLOG_I[Ch] , PROCESSLOG_J[Ch] ); // 2013.04.17
												}
											}
											//
											PROCESSLOG_J[Ch] = 8 + PROCESSLOG_I[Ch] + PROCESSLOG_J[Ch];
										}
										else if ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] > 0 ) {
											memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 4 , 2 );
											PROCESSLOG_IMSI[ Ch ][2] = 0;
											PROCESSLOG_I[Ch] = atoi( PROCESSLOG_IMSI[ Ch ] );
											if ( PROCESSLOG_I[Ch] > 0 ) {
												//
												if ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] < MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
//													memcpy( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ]] , PROCESSLOG_READDATA[ Ch ] + 6 , PROCESSLOG_I[Ch] ); // 2013.04.17
//													PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ]][PROCESSLOG_I[Ch]] = 0; // 2013.04.17
													PROCESS_MAKE_STEP_NAME( Ch , PROCESSLOG_STEP_CURRENT_READ[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 6 , PROCESSLOG_I[Ch] ); // 2013.04.17
												}
												//
											}
											//
											memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 6 + PROCESSLOG_I[Ch] , 2 );
											PROCESSLOG_IMSI[ Ch ][2] = 0;
											PROCESSLOG_J[Ch] = atoi( PROCESSLOG_IMSI[ Ch ] );
											if ( PROCESSLOG_J[Ch] > 0 ) {
												//
												if ( ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1 ) < MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
//													memcpy( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1] , PROCESSLOG_READDATA[ Ch ] + 8 + PROCESSLOG_I[Ch] , PROCESSLOG_J[Ch] ); // 2013.04.17
//													PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1][PROCESSLOG_J[Ch]] = 0; // 2013.04.17
													PROCESS_MAKE_STEP_NAME( Ch , PROCESSLOG_STEP_CURRENT_READ[ Ch ] - 1 , PROCESSLOG_READDATA[ Ch ] + 8 + PROCESSLOG_I[Ch] , PROCESSLOG_J[Ch] ); // 2013.04.17
												}
												//
											}
											PROCESSLOG_J[Ch] = 8 + PROCESSLOG_I[Ch] + PROCESSLOG_J[Ch];
										}
										else { // 2008.02.12
											//
//											PROCESSLOG_J[Ch] = 8; // 2013.07.21
											//
											// 2013.07.21
											memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 4 , 2 );
											PROCESSLOG_IMSI[ Ch ][2] = 0;
											PROCESSLOG_I[Ch] = atoi( PROCESSLOG_IMSI[ Ch ] );
											//
											memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + 6 + PROCESSLOG_I[Ch] , 2 );
											PROCESSLOG_IMSI[ Ch ][2] = 0;
											PROCESSLOG_J[Ch] = atoi( PROCESSLOG_IMSI[ Ch ] );
											//
											PROCESSLOG_J[Ch] = 8 + PROCESSLOG_I[Ch] + PROCESSLOG_J[Ch];
										}
										//
										/*
										// 2013.06.03
										if ( PROCESSLOG_STEP_LAST_READ[ Ch ] != PROCESSLOG_STEP_CURRENT_READ[ Ch ] ) {
											for ( PROCESSLOG_I[Ch] = PROCESSLOG_STEP_LAST_READ[ Ch ] + 1 ; PROCESSLOG_I[Ch] <= PROCESSLOG_STEP_CURRENT_READ[ Ch ] ; PROCESSLOG_I[Ch]++ ) {
												//
												if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d|Step%d\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] );
												}
												else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
//													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d|%s\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ); // 2013.04.17
													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d|%s\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) ); // 2013.04.17
												}
												else {
//													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|%s|%s\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] , PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ); // 2013.04.17
													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|%s|%s\n" , PROCESSLOG_I[Ch] + 1 , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) ); // 2013.04.17
												}
												//
											}
										}
										*/
										//
										// 2013.06.03
										//
										if ( PROCESSLOG_STEP_LAST_READ[ Ch ] != PROCESSLOG_STEP_CURRENT_READ[ Ch ] ) {
											//
//											if ( PROCESSLOG_STEPAUTOREMAP[ Ch ] ) { // 2016.01.22
//											if ( ( PROCESSLOG_STEPAUTOREMAP[ Ch ] == 1 ) || ( PROCESSLOG_STEPAUTOREMAP[ Ch ] == 3 ) ) { // 2016.01.22 // 2016.03.10
											if ( PROCESSLOG_STEPAUTOREMAP[ Ch ] == 1 ) { // 2016.01.22 // 2016.03.10
												//
//												startstep = ( PROCESSLOG_STEP_LAST_READ[ Ch ] < 0 ) ? 0 :PROCESSLOG_STEP_LAST_READ[ Ch ]; // 2016.01.22 // 2016.11.02
												//
												for ( PROCESSLOG_I[Ch] = PROCESSLOG_STEP_LAST_READ[ Ch ] + 1 ; PROCESSLOG_I[Ch] <= PROCESSLOG_STEP_CURRENT_READ[ Ch ] ; PROCESSLOG_I[Ch]++ ) { // 2016.01.22 // 2016.11.02
//												for ( PROCESSLOG_I[Ch] = startstep + 1 ; PROCESSLOG_I[Ch] <= PROCESSLOG_STEP_CURRENT_READ[ Ch ] ; PROCESSLOG_I[Ch]++ ) { // 2016.01.22 // 2016.11.02
													//
													if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
														f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d|Step%d\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] );
													}
													else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
														f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d|%s\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) ); // 2013.04.17
													}
													else {
														f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|%s|%s\n" , PROCESSLOG_I[Ch] + 1 , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) ); // 2013.04.17
													}
													//
												}
											}
											else { // 2013.06.03
												//
												PROCESSLOG_I[Ch] = PROCESSLOG_STEP_CURRENT_READ[ Ch ];
												//
												if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 );
												}
												else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|Step%d\n" , PROCESSLOG_I[Ch] + 1 , PROCESSLOG_I[Ch] + 1 );
												}
												else {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "$STEP %d|%s\n" , PROCESSLOG_I[Ch] + 1 , STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) );
												}
												//
											}
										}
										//
										if ( PROCESSLOG_READDATA[ Ch ][PROCESSLOG_J[Ch]] != 0 ) { // 2008.02.11
											//
											//
											/*
											//
											GetLocalTime( &SysTime ); // 2006.10.02
											//
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
												if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
												}
												else {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
												}
											}
											else {
												if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
														SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
												}
												else {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
														SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
												}
											}
											if ( PROCESSLOG_CLOCK_OVERTIME[Ch] > 0 ) {
												if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
													if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
														f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
													}
													else {
														f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
													}
												}
												else {
													if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
														f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
															SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
													}
													else {
														f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
															SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] ); // 2006.10.02
													}
												}
											}
											//
											*/
											//
											//
											PROCESS_DATA_WRITE( PROCESSLOG_FPT[Ch] , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_J[Ch] , PROCESSLOG_CLOCK_OVERTIME[Ch] > 0 ); // 2019.01.18
											//
										}
										//
										/*
										// 2013.06.03
										if ( PROCESSLOG_STEP_LAST_READ[ Ch ] != PROCESSLOG_STEP_CURRENT_READ[ Ch ] ) {
											if ( ( PROCESSLOG_DOING[ Ch ] == 1 ) || ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 9 ) ) { // 2006.02.22
												for ( PROCESSLOG_I[Ch] = PROCESSLOG_STEP_LAST_READ[ Ch ] + 1 ; PROCESSLOG_I[Ch] <= PROCESSLOG_STEP_CURRENT_READ[ Ch ] ; PROCESSLOG_I[Ch]++ ) {
													if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() % 2 ) {
														//
//
// 2011.11.11
														if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|Step%d|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch] ,
																PROCESSLOG_I[Ch] );
														}
														else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|%s|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ,
																PROCESSLOG_I[Ch] );
														}
														else {
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|%s|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) ,
																PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ,
																PROCESSLOG_I[Ch] );
														}
//
// 2011.11.11
														UTIL_Copy_Multi_Recipe_Valid_String_Count( PROCESSLOG_IMSI[ Ch ] , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , 255 );
														//
														if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|Step%d|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESSLOG_IMSI[ Ch ] ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch] ,
																PROCESSLOG_I[Ch] );
														}
														else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|%s|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESSLOG_IMSI[ Ch ] ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch]+1 ,
//																PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] , // 2013.04.17
																STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) , // 2013.04.17
																PROCESSLOG_I[Ch] );
														}
														else {
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|%s|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESSLOG_IMSI[ Ch ] ,
//																PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] , // 2013.04.17
																STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) , // 2013.04.17
																PROCESSLOG_I[Ch]+1 ,
//																PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] , // 2013.04.17
																STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) , // 2013.04.17
																PROCESSLOG_I[Ch] );
														}
														//
//														if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 128 ) ) { // 2007.11.07
														if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
															strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
														}
														//
														FA_SEND_MESSAGE_TO_SERVER( PROCESSLOG_READDATA[ Ch ] );
													}
												}
											}
											//
											PROCESSLOG_STEP_LAST_READ[ Ch ] = PROCESSLOG_STEP_CURRENT_READ[ Ch ];
										}
										*/
										//
										// 2013.06.03
										if ( PROCESSLOG_STEP_LAST_READ[ Ch ] != PROCESSLOG_STEP_CURRENT_READ[ Ch ] ) {
											if ( ( PROCESSLOG_DOING[ Ch ] == 1 ) || ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 9 ) ) { // 2006.02.22
												//
//												if ( PROCESSLOG_STEPAUTOREMAP[ Ch ] ) { // 2016.01.22
//												if ( ( PROCESSLOG_STEPAUTOREMAP[ Ch ] == 1 ) || ( PROCESSLOG_STEPAUTOREMAP[ Ch ] == 2 ) ) { // 2016.01.22 // 2016.03.10
												if ( PROCESSLOG_STEPAUTOREMAP[ Ch ] == 1 ) { // 2016.01.22 // 2016.03.10
													//
//													startstep = ( PROCESSLOG_STEP_LAST_READ[ Ch ] < 0 ) ? 0 :PROCESSLOG_STEP_LAST_READ[ Ch ]; // 2016.01.22 // 2016.11.02
													//
													for ( PROCESSLOG_I[Ch] = PROCESSLOG_STEP_LAST_READ[ Ch ] + 1 ; PROCESSLOG_I[Ch] <= PROCESSLOG_STEP_CURRENT_READ[ Ch ] ; PROCESSLOG_I[Ch]++ ) { // 2016.01.22 // 2016.11.02
//													for ( PROCESSLOG_I[Ch] = startstep + 1 ; PROCESSLOG_I[Ch] <= PROCESSLOG_STEP_CURRENT_READ[ Ch ] ; PROCESSLOG_I[Ch]++ ) { // 2016.01.22 // 2016.11.02
														//
														if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() % 2 ) {
															//
															UTIL_Copy_Multi_Recipe_Valid_String_Count( PROCESSLOG_IMSI[ Ch ] , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , 255 );
															//
															FA_SIDE_TO_PORT_GETS( PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , MsgSltchar ); // 2018.11.13
															//
															if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
//																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|Step%d|%d" , // 2018.11.13
																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|Step%d|%d|Step%d|%d" , // 2018.11.13
																	Ch-PM1+1 ,
//																	PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																	MsgSltchar , // 2018.11.13
																	PROCESSLOG_CH0[Ch] ,
																	PROCESSLOG_IMSI[ Ch ] ,
																	PROCESSLOG_I[Ch]+1 ,
																	PROCESSLOG_I[Ch]+1 ,
																	PROCESSLOG_I[Ch] ,
																	PROCESSLOG_I[Ch] );
															}
															else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
//																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|%s|%d" , // 2018.11.13
																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|Step%d|%d|%s|%d" , // 2018.11.13
																	Ch-PM1+1 ,
//																	PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																	MsgSltchar , // 2018.11.13
																	PROCESSLOG_CH0[Ch] ,
																	PROCESSLOG_IMSI[ Ch ] ,
																	PROCESSLOG_I[Ch]+1 ,
																	PROCESSLOG_I[Ch]+1 ,
																	STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) , // 2013.04.17
																	PROCESSLOG_I[Ch] );
															}
															else {
//																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|%s|%d" , // 2018.11.13
																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|%s|%d|%s|%d" , // 2018.11.13
																	Ch-PM1+1 ,
//																	PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																	MsgSltchar , // 2018.11.13
																	PROCESSLOG_CH0[Ch] ,
																	PROCESSLOG_IMSI[ Ch ] ,
																	STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) , // 2013.04.17
																	PROCESSLOG_I[Ch]+1 ,
																	STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) , // 2013.04.17
																	PROCESSLOG_I[Ch] );
															}
															//
															if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
															}
															//
															FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
														}
													}
												}
												else { // 2013.06.03
													//
													PROCESSLOG_J[Ch] = PROCESSLOG_STEP_LAST_READ[ Ch ]; // 2014.10.10
													PROCESSLOG_I[Ch] = PROCESSLOG_STEP_CURRENT_READ[ Ch ];
													//
													if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() % 2 ) {
														//
														UTIL_Copy_Multi_Recipe_Valid_String_Count( PROCESSLOG_IMSI[ Ch ] , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , 255 );
														//
														FA_SIDE_TO_PORT_GETS( PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , MsgSltchar ); // 2018.11.13
														//
														/*
														// 2014.10.10
														//
														if      ( ( PROCESSLOG_I[ Ch ] - 1 ) >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|Step%d|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESSLOG_IMSI[ Ch ] ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch] ,
																PROCESSLOG_I[Ch] );
															//
															if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
															}
															//
															FA_SEND_MESSAGE_TO_SERVER( PROCESSLOG_READDATA[ Ch ] );
															//
														}
														else if ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
															sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|%s|%d" ,
																Ch-PM1+1 ,
																PROCESSLOG_SIDE[Ch]+1 ,
																PROCESSLOG_CH0[Ch] ,
																PROCESSLOG_IMSI[ Ch ] ,
																PROCESSLOG_I[Ch]+1 ,
																PROCESSLOG_I[Ch]+1 ,
																STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) , // 2013.04.17
																PROCESSLOG_I[Ch] );
															//
															if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
															}
															//
															FA_SEND_MESSAGE_TO_SERVER( PROCESSLOG_READDATA[ Ch ] );
															//
														}
														else {
															if ( PROCESSLOG_I[Ch] > 0 ) { // 203.07.18
																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|%s|%d" ,
																	Ch-PM1+1 ,
																	PROCESSLOG_SIDE[Ch]+1 ,
																	PROCESSLOG_CH0[Ch] ,
																	PROCESSLOG_IMSI[ Ch ] ,
																	STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) , // 2013.04.17
																	PROCESSLOG_I[Ch]+1 ,
																	STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]-1] ) , // 2013.04.17
																	PROCESSLOG_I[Ch] );
																//
																if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																	strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																}
																//
																FA_SEND_MESSAGE_TO_SERVER( PROCESSLOG_READDATA[ Ch ] );
															}
														}
														//
														*/
														//
														//
														// 2014.10.10
														//
														if      ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
															//
															if      ( PROCESSLOG_J[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
//																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|Step%d|%d" , // 2018.11.13
																sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|Step%d|%d|Step%d|%d" , // 2018.11.13
																	Ch-PM1+1 ,
//																	PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																	MsgSltchar , // 2018.11.13
																	PROCESSLOG_CH0[Ch] ,
																	PROCESSLOG_IMSI[ Ch ] ,
																	PROCESSLOG_I[Ch]+1 ,
																	PROCESSLOG_I[Ch]+1 ,
																	PROCESSLOG_J[Ch]+1 ,
																	PROCESSLOG_J[Ch]+1 );
																//
																if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																	strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																}
																//
																FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																//
															}
															else {
																if ( PROCESSLOG_J[Ch] >= 0 ) {
//																	sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|%s|%d" , // 2018.11.13
																	sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|Step%d|%d|%s|%d" , // 2018.11.13
																		Ch-PM1+1 ,
//																		PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																		MsgSltchar , // 2018.11.13
																		PROCESSLOG_CH0[Ch] ,
																		PROCESSLOG_IMSI[ Ch ] ,
																		PROCESSLOG_I[Ch]+1 ,
																		PROCESSLOG_I[Ch]+1 ,
																		STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_J[Ch]] ) ,
																		PROCESSLOG_J[Ch]+1 );
																	//
																	if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																		strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																	}
																	//
																	FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																}
															}
															//
														}
														else {
															if ( PROCESSLOG_I[Ch] > 0 ) {
																//
																if      ( PROCESSLOG_J[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
																	//
//																	sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|Step%d|%d" , // 2018.11.13
																	sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|%s|%d|Step%d|%d" , // 2018.11.13
																		Ch-PM1+1 ,
//																		PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																		MsgSltchar , // 2018.11.13
																		PROCESSLOG_CH0[Ch] ,
																		PROCESSLOG_IMSI[ Ch ] ,
																		STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) ,
																		PROCESSLOG_I[Ch]+1 ,
																		PROCESSLOG_J[Ch]+1 ,
																		PROCESSLOG_J[Ch]+1 );
																	//
																	if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																		strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																	}
																	//
																	FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																}
																else {
																	if ( PROCESSLOG_J[Ch] >= 0 ) {
//																		sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|%s|%d" , // 2018.11.13
																		sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|%s|%d|%s|%d" , // 2018.11.13
																			Ch-PM1+1 ,
//																			PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																			MsgSltchar , // 2018.11.13
																			PROCESSLOG_CH0[Ch] ,
																			PROCESSLOG_IMSI[ Ch ] ,
																			STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) ,
																			PROCESSLOG_I[Ch]+1 ,
																			STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_J[Ch]] ) ,
																			PROCESSLOG_J[Ch]+1 );
																		//
																		if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																			strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																		}
																		//
																		FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																	}
																}
															}
														}
														//
														//
													}
												}
											}
											//
											PROCESSLOG_STEP_LAST_READ[ Ch ] = PROCESSLOG_STEP_CURRENT_READ[ Ch ];
										}
									}
//									else if ( ( PROCESSLOG_I[Ch] == 8 ) && ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) ) { // 2006.05.17 // 2013.06.04
//									else if ( ( PROCESSLOG_I[Ch] == 8 ) && ( ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '&' ) ) ) { // 2006.05.17 // 2013.06.04 // 2016.01.22
//									else if ( ( PROCESSLOG_I[Ch] == 8 ) && ( ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '&' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '%' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '@' ) ) ) { // 2006.05.17 // 2013.06.04 // 2016.01.22 // 2016.03.10
									else if ( ( PROCESSLOG_I[Ch] == 8 ) && ( ( PROCESSLOG_READDATA[ Ch ][0] == '$' ) || ( PROCESSLOG_READDATA[ Ch ][0] == '&' ) ) ) { // 2006.05.17 // 2013.06.04 // 2016.01.22 // 2016.03.10
										// Skip 2006.05.17 Step String is nothing
									}
									else if ( PROCESSLOG_READDATA[ Ch ][0] == '#' ) {
										PROCESSLOG_STEP_CURRENT_READ[Ch] = 0;
										PROCESSLOG_I[Ch] = 1;
										while( TRUE ) {
											if ( PROCESSLOG_READDATA[ Ch ][PROCESSLOG_I[Ch]] == 0 ) break;
											//
											if ( PROCESSLOG_READDATA[ Ch ][PROCESSLOG_I[Ch]] == '&' ) { // 2016.03.10
												//
												memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_I[Ch] + 1 , 3 ); PROCESSLOG_IMSI[ Ch ][3] = 0;
												PROCESSLOG_STEP_CURRENT_READ[ Ch ] = atoi( PROCESSLOG_IMSI[ Ch ] );
												//
												if ( PROCESSLOG_STEP_LAST_READ[ Ch ] != PROCESSLOG_STEP_CURRENT_READ[ Ch ] ) {
													if ( ( PROCESSLOG_DOING[ Ch ] == 1 ) || ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() >= 9 ) ) {
														PROCESSLOG_J[Ch] = PROCESSLOG_STEP_LAST_READ[ Ch ]; // 2014.10.10
														PROCESSLOG_I[Ch] = PROCESSLOG_STEP_CURRENT_READ[ Ch ];
														//
														if ( _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() % 2 ) {
															//
															UTIL_Copy_Multi_Recipe_Valid_String_Count( PROCESSLOG_IMSI[ Ch ] , PROCESS_MONITOR_Get_Recipe(PROCESSLOG_SIDE[Ch],Ch) , 255 );
															//
															FA_SIDE_TO_PORT_GETS( PROCESSLOG_SIDE[Ch] , PM_PROCESS_POINTER[PROCESSLOG_SIDE[Ch]][Ch] , MsgSltchar ); // 2018.11.13
															//
															if      ( PROCESSLOG_I[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
																//
																if      ( PROCESSLOG_J[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
//																	sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|Step%d|%d" , // 2018.11.13
																	sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|Step%d|%d|Step%d|%d" , // 2018.11.13
																		Ch-PM1+1 ,
//																		PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																		MsgSltchar , // 2018.11.13
																		PROCESSLOG_CH0[Ch] ,
																		PROCESSLOG_IMSI[ Ch ] ,
																		PROCESSLOG_I[Ch]+1 ,
																		PROCESSLOG_I[Ch]+1 ,
																		PROCESSLOG_J[Ch]+1 ,
																		PROCESSLOG_J[Ch]+1 );
																	//
																	if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																		strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																	}
																	//
																	FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																	//
																}
																else {
																	if ( PROCESSLOG_J[Ch] >= 0 ) {
//																		sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|Step%d|%d|%s|%d" , // 2018.11.13
																		sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|Step%d|%d|%s|%d" , // 2018.11.13
																			Ch-PM1+1 ,
//																			PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																			MsgSltchar , // 2018.11.13
																			PROCESSLOG_CH0[Ch] ,
																			PROCESSLOG_IMSI[ Ch ] ,
																			PROCESSLOG_I[Ch]+1 ,
																			PROCESSLOG_I[Ch]+1 ,
																			STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_J[Ch]] ) ,
																			PROCESSLOG_J[Ch]+1 );
																		//
																		if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																			strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																		}
																		//
																		FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																	}
																}
																//
															}
															else {
																if ( PROCESSLOG_I[Ch] > 0 ) {
																	//
																	if      ( PROCESSLOG_J[ Ch ] >= MAX_PM_LOG_STEP_COUNT ) {
																		//
//																		sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|Step%d|%d" , // 2018.11.13
																		sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|%s|%d|Step%d|%d" , // 2018.11.13
																			Ch-PM1+1 ,
//																			PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																			MsgSltchar , // 2018.11.13
																			PROCESSLOG_CH0[Ch] ,
																			PROCESSLOG_IMSI[ Ch ] ,
																			STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) ,
																			PROCESSLOG_I[Ch]+1 ,
																			PROCESSLOG_J[Ch]+1 ,
																			PROCESSLOG_J[Ch]+1 );
																		//
																		if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																			strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																		}
																		//
																		FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																	}
																	else {
																		if ( PROCESSLOG_J[Ch] >= 0 ) {
//																			sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|PORT%d|%d|%s|%s|%d|%s|%d" , // 2018.11.13
																			sprintf( PROCESSLOG_READDATA[ Ch ] , "PROCESS_STEP_CHANGE PM%d|%s|%d|%s|%s|%d|%s|%d" , // 2018.11.13
																				Ch-PM1+1 ,
//																				PROCESSLOG_SIDE[Ch]+1 , // 2018.11.13
																				MsgSltchar , // 2018.11.13
																				PROCESSLOG_CH0[Ch] ,
																				PROCESSLOG_IMSI[ Ch ] ,
																				STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_I[Ch]] ) ,
																				PROCESSLOG_I[Ch]+1 ,
																				STR_MEM_GET_STR( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_J[Ch]] ) ,
																				PROCESSLOG_J[Ch]+1 );
																			//
																			if ( SCHMULTI_MESSAGE_GET_ALL( PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_IMSI[ Ch ] , 255 ) ) { // 2007.11.07
																				strcat( PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
																			}
																			//
																			FA_SEND_MESSAGE_TO_SERVER( 31 , PROCESSLOG_SIDE[Ch] , PROCESS_MONITOR_Get_Pointer(PROCESSLOG_SIDE[Ch],Ch) , PROCESSLOG_READDATA[ Ch ] );
																		}
																	}
																}
															}
															//
														}
													}
													//
													PROCESSLOG_STEP_LAST_READ[ Ch ] = PROCESSLOG_STEP_CURRENT_READ[ Ch ];
													//
												}
												//
												break;
											}
											else {
												//
												memcpy( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_I[Ch] , 2 ); PROCESSLOG_IMSI[ Ch ][2] = 0;
												PROCESSLOG_J[ Ch ] = atoi( PROCESSLOG_IMSI[ Ch ] );
												//
												if ( ( PROCESSLOG_J[Ch] > 0 ) && ( PROCESSLOG_READDATA[ Ch ][PROCESSLOG_I[Ch] + 2] == 0 ) ) break;
												//
												if ( PROCESSLOG_STEP_CURRENT_READ[ Ch ] < MAX_PM_LOG_STEP_COUNT ) { // 2007.10.25
	//												memcpy( PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[Ch]] , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_I[Ch] + 2 , PROCESSLOG_J[Ch] ); // 2013.04.17
	//												PROCESSLOG_STEP_NAME[Ch][PROCESSLOG_STEP_CURRENT_READ[Ch]][PROCESSLOG_J[Ch]] = 0; // 2013.04.17
													PROCESS_MAKE_STEP_NAME( Ch , PROCESSLOG_STEP_CURRENT_READ[ Ch ] , PROCESSLOG_READDATA[ Ch ] + PROCESSLOG_I[Ch] + 2 , PROCESSLOG_J[Ch] ); // 2013.04.17
												}
												//
												PROCESSLOG_I[Ch] = PROCESSLOG_I[Ch] + 2 + PROCESSLOG_J[Ch];
												PROCESSLOG_STEP_CURRENT_READ[Ch]++;
												//
											}
											//
										}
										//============================
										PROCESSLOG_CLOCK_OVERTIME[Ch] = 0;
										//============================
									}
									else {
										//
										//
										/*
										//
										GetLocalTime( &SysTime ); // 2006.10.02
										if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
											else {
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
										}
										else {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
													SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
											else {
												f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
													SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
											}
										}
										//
										if ( PROCESSLOG_CLOCK_OVERTIME[Ch] > 0 ) {
											if ( PROCESS_MONITORING_TIME_MINIMUM_LOG % 2 ) {
												if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d.%03d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
												}
												else {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d:%d:%d%c%s\n" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
												}
											}
											else {
												if ( PROCESS_MONITORING_TIME_MINIMUM_LOG / 2 ) { // 2009.02.02
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d.%03d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
														SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
												}
												else {
													f_enc_printf( PROCESSLOG_FPT[Ch] , "%d/%d/%d %d:%d:%d%c%s\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , // 2006.10.02
														SysTime.wHour , SysTime.wMinute , SysTime.wSecond , 9 , PROCESSLOG_READDATA[ Ch ] ); // 2006.10.02
												}
											}
										}
										//
										*/
										//
										//
										//
										//
										PROCESS_DATA_WRITE( PROCESSLOG_FPT[Ch] , PROCESSLOG_READDATA[ Ch ] , PROCESSLOG_CLOCK_OVERTIME[Ch] > 0 ); // 2019.01.18
										//
										//
									}
								}
								else {
									PROCESSLOG_CLOCK_OVERTIME[Ch] = 0;
								}
								//
								LeaveCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
								//
								if ( PROCESSLOG_CLOCK_OVERTIME[Ch] > 0 ) {
									PROCESSLOG_CLOCK_FLOW[Ch] = PROCESSLOG_CLOCK_OVERTIME[Ch] + GetTickCount() - ( PROCESSLOG_CLOCK_START[Ch] + PROCESS_MONITORING_TIME ); // 2003.10.09
								}
								else {
									PROCESSLOG_CLOCK_FLOW[Ch] = GetTickCount() - PROCESSLOG_CLOCK_START[Ch]; // 2003.10.09
								}

								if ( PROCESSLOG_CLOCK_FLOW[Ch] >= PROCESS_MONITORING_TIME ) {
									Sleep(1);
									PROCESSLOG_CLOCK_OVERTIME[Ch] = ( PROCESSLOG_CLOCK_FLOW[Ch] + 10 ) - PROCESS_MONITORING_TIME;
								}
								else {
									Sleep( PROCESS_MONITORING_TIME - PROCESSLOG_CLOCK_FLOW[Ch] );
									PROCESSLOG_CLOCK_OVERTIME[Ch] = 0;
								}
								PROCESSLOG_CLOCK_START[Ch] = GetTickCount(); // 2003.10.09
								//
								EnterCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
								//
								if ( PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] != -1 ) { // 2011.04.18
									PROCESSLOG_SIDE[Ch] = PROCESSLOG_MONITOR_SIDE_CHANGE[Ch];
									PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] = -1;
								}
								//
							}
							else {
								//
								LeaveCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
								//
								Sleep( PROCESS_MONITORING_TIME );
								//
								EnterCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
								//
								if ( PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] != -1 ) { // 2011.04.18
									PROCESSLOG_SIDE[Ch] = PROCESSLOG_MONITOR_SIDE_CHANGE[Ch];
									PROCESSLOG_MONITOR_SIDE_CHANGE[Ch] = -1;
								}
								//
							}
						}
						//
						if ( PROCESSLOG_FPT[Ch] != NULL ) {
							_dRUN_FUNCTION_ABORT( PROCESSLOG_MONITOR_IO[ Ch ] );
							fclose( PROCESSLOG_FPT[Ch] );
							//
							if ( !PROCESSLOG_MONITOR_HAS_DATA ) { // 2017.06.29
								//
								sprintf( PROCESSLOG_IMSI[ Ch ] , "%s\\Process_Sim.dat" , _i_SCH_PRM_GET_DFIX_LOG_PATH() );
								//
								CopyFile( PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_FILENAME[ Ch ] , FALSE );
								//
							}
							//
						}
						//-------------------------------
					}
					//====================================================================================================
					// 2007.05.02
					//====================================================================================================
					for ( PROCESSLOG_XP[Ch] = 0 ; PROCESSLOG_XP[Ch] < MAX_PM_SLOT_DEPTH ; PROCESSLOG_XP[Ch]++ ) {
						if ( PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] > 0 ) {
							//
							if ( PROCESSLOG_OPTION[ Ch ] != 3 ) { // 2013.03.11
								if ( ( PROCESSLOG_XP[Ch] == 0 ) && ( ( PROCESSLOG_CH0[ Ch ] / 100 ) > 0 ) ) continue; // 2012.06.16
							}
							//
//							PROCESS_MONITOR_GET_FILENAME( TRUE , Ch , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13
//							PROCESS_MONITOR_GET_FILENAME( TRUE , Ch , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_POINTER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ); // 2007.06.04 // 2011.05.13 // 2012.11.29

							if ( PROCESS_MONITOR_GET_FILENAME( 2 , Ch , PROCESSLOG_SIDE_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_POINTER_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_WAFER_EX[ Ch ][PROCESSLOG_XP[Ch]] , 0 , 0 , PROCESSLOG_IMSI[ Ch ] , PROCESSLOG_DUMMY_EX[ Ch ][PROCESSLOG_XP[Ch]] , PROCESSLOG_DOING[ Ch ] , FALSE , ( PROCESSLOG_OPTION[ Ch ] == 3 ) ) ) { // 2012.11.29
								PROCESS_FILE_COPY_OPERATION( PROCESSLOG_FILENAME[ Ch ] , PROCESSLOG_IMSI[ Ch ] );
							}
						}
					}
					//====================================================================================================
					if ( PROCESSLOG_FNOTUSE[ Ch ] ) DeleteFile( PROCESSLOG_FILENAME[ Ch ] ); // 2007.07.25
					//====================================================================================================
					GUI_PROCESS_LOG_MONITOR2_SET( Ch , 2 , 0 , 0 , "" );
					//======================================================================================================================
					// 2006.06.28
					//======================================================================================================================
					PROCESS_MONITOR_ONEBODY_REBUILD( Ch );
					//======================================================================================================================
				}
				else {
					//
					LeaveCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
					//
//					Sleep(250); // 2013.02.27
					Sleep(25); // 2013.02.27
					//
					EnterCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
					//
				}
			}
			//
			LeaveCriticalSection( &CR_PM_DATA_RESET1[Ch] ); // 2011.04.18
			//
		}
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduler Process Monitor
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void PROCESS_MONITOR_STATUS_Abort_Signal_Set( int side , BOOL Always ) {
	int j;
	for ( j = PM1 ; j < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; j++ ) { // Except BM?
		if ( Always ) {
			Process_Monitor_Sts_Abort[side][j] = 1;
		}
		else {
			switch( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( j ) ) {
			case STOP_LIKE_NONE :
			case STOP_LIKE_NONE_ALLABORT_REMAINPRCS : // 2012.02.08
				break;
			case STOP_LIKE_PM_NONE_GO_OUT :
			case STOP_LIKE_PM_NONE_GO_OUT_ALLABORT_REMAINPRCS : // 2012.02.08
				Process_Monitor_Sts_Abort[side][j] = 2;
				break;
			case STOP_LIKE_PM_ABORT_GO_NEXT :
			case STOP_LIKE_PM_ABORT_GO_NEXT_ALLABORT_REMAINPRCS : // 2012.02.08
				Process_Monitor_Sts_Abort[side][j] = 1;
				break;
			case STOP_LIKE_PM_ABORT_GO_OUT :
			case STOP_LIKE_PM_ABORT_GO_OUT_ALLABORT_REMAINPRCS : // 2012.02.08
				Process_Monitor_Sts_Abort[side][j] = 1;
				break;
			}
		}
	}
}
//
void PROCESS_MONITOR_STATUS_Abort_Signal_Reset( int side ) {
	int j;
	for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
		Process_Monitor_Sts_Abort[side][j] = 0;
	}
}
//
void PROCESS_MONITOR_STATUS() {
	int i , j , k , Slot , f = 0 , rm;
	int ioa , CommStatus; // 2016.02.15
	int fail_scenario , success_result , fail_result , abort_result;
	BOOL Find , Abort , Fnotuse; // , pres;
	int Dummy , WfrNotCheck; // 2007.07.24
	int PathIn; // 2010.05.12
	int Finish_Result; // 2013.06.19
	//
	for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
		//
		if ( ( _i_SCH_SYSTEM_USING_GET( k ) <= 0 ) && ( !Process_Monitor_Sts_Find[k] ) ) continue;
		//
		Find = FALSE;
		//
		for ( j = PM1 ; j < TM ; j++ ) {
			if ( j == AL ) continue;
			if ( j == IC ) continue;
			//
			if ( !_i_SCH_PRM_GET_MODULE_MODE( j ) ) continue; // 2003.08.01
			//
			//if ( !_SCH_MODULE_GET_USE_ENABLE( j , TRUE ) ) continue; // 2003.08.01
			//=================================================================================
			Abort = FALSE;
			//
			EnterCriticalSection( &CR_PM_DATA_RESET2[j] ); // 2011.04.18
			//
			Slot = PROCESS_MONITOR_Get_Status( k , j );
			Dummy = PM_PROCESS_DUMMY[k][j]; // 2007.05.15
			PathIn = PM_PROCESS_PATHIN[k][j]; // 2010.05.12
			Fnotuse = PM_PROCESS_FNOTUSE[k][j]; // 2007.05.15
			WfrNotCheck = PM_PROCESS_WFRNOTCHECK[k][j]; // 2007.10.11
			//=================================================================================
			// 2007.05.02
			//=================================================================================
			for ( i = 0 ; i < MAX_PM_SLOT_DEPTH ; i++ ) {
				PR_MON_Side_ex[i]    = PM_PROCESS_SIDE_EX[j][i];
				PR_MON_Pointer_ex[i] = PM_PROCESS_POINTER_EX[j][i];
				PR_MON_Slot_ex[i]    = PM_PROCESS_WAFER_EX[j][i];
				PR_MON_Dummy_ex[i]   = PM_PROCESS_DUMMY_EX[j][i];
				PR_MON_MultiUnuse_ex[i]   = PM_PROCESS_MULTIUNUSE_EX[j][i]; // 2014.02.10
			}
			//=================================================================================
			if ( Slot > 0 ) {
				if ( Slot < PROCESS_INDICATOR_PRE ) { // 2005.09.13
					if ( Process_Monitor_Sts_Abort[k][j] == 1 ) {
						Abort = TRUE;
						EQUIP_RUN_PROCESS_ABORT( k , j );
						Process_Monitor_Sts_Abort[k][j] = 2;
					}
					else if ( Process_Monitor_Sts_Abort[k][j] == 2 ) {
						Abort = TRUE;
					}
				}
			}
			//
			if ( Slot > 0 ) {
				Find = TRUE; f++;
				//-----------------------------------------------------------
				fail_scenario = -1; // 2003.10.15
				success_result = -1; // 2003.10.15
				fail_result = -1; // 2003.10.15
				abort_result = -1; // 2003.10.15
				//-----------------------------------------------------------
				if ( j >= BM1 ) {
					if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( j ) ) {
						i = EQUIP_STATUS_PROCESS( k , j );
					}
					else {
//						if ( KPLT_CHECK_BM_RUN_TIMER( j ) ) i = SYS_SUCCESS; // 2016.12.09
						if ( KPLT_CHECK_BM_RUN_MS_TIMER( j ) ) i = SYS_SUCCESS; // 2016.12.09
						else                                   i = SYS_RUNNING;
					}
				}
				else {
					//
					if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
						if ( ( ( j - PM1 ) % 2 ) == 0 ) {
							rm = j;
						}
						else {
							rm = j - 1;
						}
					}
					else {
						rm = j;
					}
					//
					i = EQUIP_STATUS_PROCESS( k , rm );
					//
					//---------------------------------------
					// 2016.02.15
					//---------------------------------------
					if ( i == SYS_ABORTED ) {
						//
						switch ( _i_SCH_PRM_GET_MODULE_COMMSTATUS2_MODE() ) {
						case 4 :
						case 5 :
							//
							ioa = _FIND_FROM_STRING( _K_D_IO , _i_SCH_PRM_GET_MODULE_COMMSTATUS_IO_NAME( rm ) );
							//
							if ( ioa >= 0 ) {
								if ( 0 == _dREAD_DIGITAL( ioa , &CommStatus ) ) {
									i = SYS_ERROR;
								}
							}
							//
							break;
						}
						//
					}
					//---------------------------------------
					// 2002.03.25
					//---------------------------------------
					if ( i != SYS_RUNNING ) { // 2007.10.02
						//=====================================================================================================
						// 2007.11.28
						//=====================================================================================================
						if ( PROCESSLOG_MONITOR_IO[ j ] >= 0 ) _dRUN_FUNCTION_ABORT( PROCESSLOG_MONITOR_IO[ j ] );
						//=====================================================================================================
//						if ( ( Slot < PROCESS_INDICATOR_PRE ) && ( i != SYS_RUNNING ) ) { // 2005.09.13 // 2007.10.02
						if ( Slot < PROCESS_INDICATOR_PRE ) { // 2005.09.13 // 2007.10.02
							if ( ( PROCESS_MONITOR_Get_Pointer( k , j ) >= 0 ) &&
								( PROCESS_MONITOR_Get_No1( k , j ) >= 0 ) &&
								( PROCESS_MONITOR_Get_No2( k , j ) >= 0 ) ) {
//								if ( PROCESS_MONITOR_Get_Pointer( k , j ) < 100 ) { // 2013.05.23
								if ( PROCESS_MONITOR_Get_Pointer( k , j ) < MAX_CASSETTE_SLOT_SIZE ) { // 2013.05.23
									//==============================================================================================
									PROCESS_SUB_UPLOAD_STATUS_OPERATION( 0 , rm , k , j , &fail_scenario , &success_result , &fail_result , &abort_result , TRUE , Fnotuse ); // 2007.07.24
									//==============================================================================================
								}
								else { // 2002.05.09
									//==============================================================================================
									PROCESS_SUB_UPLOAD_STATUS_OPERATION( 2 , rm , k , j , &fail_scenario , &success_result , &fail_result , &abort_result , TRUE , Fnotuse ); // 2007.07.24
									//==============================================================================================
								}
							}
						}
						//---------------------------------------
						// 2006.02.13
						//---------------------------------------
//						else if ( ( Slot >= PROCESS_INDICATOR_PRE ) && ( Slot < PROCESS_INDICATOR_POST ) && ( i != SYS_RUNNING ) ) { // 2006.02.13 // 2007.10.02
						else if ( ( Slot >= PROCESS_INDICATOR_PRE ) && ( Slot < PROCESS_INDICATOR_POST ) ) { // 2006.02.13 // 2007.10.02
							if ( ( PROCESS_MONITOR_Get_Pointer( k , j ) >= 0 ) &&
								( PROCESS_MONITOR_Get_No1( k , j ) >= 0 ) &&
								( PROCESS_MONITOR_Get_No2( k , j ) >= 0 ) ) {
//								if ( PROCESS_MONITOR_Get_Pointer( k , j ) < 100 ) { // 2013.05.23
								if ( PROCESS_MONITOR_Get_Pointer( k , j ) < MAX_CASSETTE_SLOT_SIZE ) { // 2013.05.23
									//==============================================================================================
									PROCESS_SUB_UPLOAD_STATUS_OPERATION( 1 , rm , k , j , &fail_scenario , &success_result , &fail_result , &abort_result , TRUE , Fnotuse ); // 2007.07.24
									//==============================================================================================
								}
							}
						}
//						else if ( ( Slot >= PROCESS_INDICATOR_POST ) && ( i != SYS_RUNNING ) ) { // 2006.02.13 // 2007.10.02
						else if ( Slot >= PROCESS_INDICATOR_POST ) { // 2006.02.13 // 2007.10.02
							if ( ( PROCESS_MONITOR_Get_Pointer( k , j ) >= 0 ) &&
								( PROCESS_MONITOR_Get_No1( k , j ) >= 0 ) &&
								( PROCESS_MONITOR_Get_No2( k , j ) >= 0 ) ) {
//								if ( PROCESS_MONITOR_Get_Pointer( k , j ) < 100 ) { // 2013.05.23
								if ( PROCESS_MONITOR_Get_Pointer( k , j ) < MAX_CASSETTE_SLOT_SIZE ) { // 2013.05.23
									//==============================================================================================
									PROCESS_SUB_UPLOAD_STATUS_OPERATION( 2 , rm , k , j , &fail_scenario , &success_result , &fail_result , &abort_result , TRUE , Fnotuse ); // 2007.07.24
									//==============================================================================================
								}
							}
						}
					}
					//---------------------------------------
				}
				switch( i ) {
				case SYS_SUCCESS :
					//-----------------------------------------------------
					if ( j < BM1 ) { // 2003.09.08
						if ( _i_SCH_PRM_GET_MODULE_INTERFACE( j ) > 0 ) {
							PROCESS_INTERFACE_MODULE_CLEAR_INFO( k , j );
						}
					}
					//-----------------------------------------------------
					if ( Abort ) {
						//=======================================================================================================================
						Finish_Result = -1; // 2013.06.19
						//=======================================================================================================================
						PROCESS_SUB_ABORT_STOP_SCENARIO_OPERATION( k , j , TRUE , &Finish_Result ); // 2007.07.24
						//=======================================================================================================================
//						pres = FALSE; // 2011.03.02 // 2013.06.19
					}
					else {
						//=======================================================================================================================
						Finish_Result = 0; // 2013.06.19
						//=======================================================================================================================
//						pres = PROCESS_MONITOR_Set_Finish_xStatus( k , j , 0 ); // 2013.06.19
						//=======================================================================================================================
					}
					if ( Slot < PROCESS_INDICATOR_PRE ) { // 2005.09.13
						Process_Monitor_Sts_Abort[k][j] = 0;
						if ( j < BM1 ) {
							//=======================================================================================================================
							_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , j , TRUE );
							//=======================================================================================================================
							if ( ( success_result < 0 ) || ( success_result > MRES_SUCCESS_LIKE_FAIL ) ) success_result = _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( j );
							//=======================================================================================================================
//							PROCESS_SUB_SUCCESS_RESULT_OPERATION( ( PROCESS_MONITOR_Get_Pointer( k , j ) < 100 ) ? k : PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_Pointer( k , j ) , Slot , j , success_result , TRUE , Fnotuse , PathIn ); // 2007.07.24 // 2015.04.30
							PROCESS_SUB_SUCCESS_RESULT_OPERATION( ( PROCESS_MONITOR_Get_Pointer( k , j ) < MAX_CASSETTE_SLOT_SIZE ) ? k : PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_Pointer( k , j ) , Slot , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , success_result , TRUE , Fnotuse , PathIn ); // 2007.07.24 // 2015.04.30
							//=======================================================================================================================
							PROCESS_SUB_ADD_RUN_COUNT( TRUE , j , Slot , TRUE , Fnotuse ); // 2007.07.24
							//=======================================================================================================================
							//PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , Slot , i , success_result ); // 2007.07.25 // 2013.05.23
//							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , success_result ); // 2007.07.25 // 2013.05.23 // 2015.04.30 // 2016.04.26
							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , success_result ); // 2007.07.25 // 2013.05.23 // 2015.04.30 // 2016.04.26
							//=======================================================================================================================
							PROCESS_SUB_EXTEND_PM_END_MESSAGE( k , j , i , success_result ); // 2007.07.25
							//=======================================================================================================================
						}
						else { // 2006.07.20
							PROCESS_SUB_NORMAL_BM_END_MESSAGE( k , j , i ); // 2007.12.28
						}
					}
					else if ( ( Slot >= PROCESS_INDICATOR_PRE ) && ( Slot < PROCESS_INDICATOR_POST ) ) { // 2002.04.08 // Pre // 2005.09.13
						_i_SCH_TIMER_SET_PROCESS_TIME_END( 2 , j , ( Slot > PROCESS_INDICATOR_PRE ) ); // 2006.02.09
						//-----------------------------------------------------------------------------------------------------------------------------------
						if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { // 2004.05.11
							if ( Slot == PROCESS_INDICATOR_PRE ) { // 2005.09.13
								_i_SCH_LOG_LOT_PRINTF( k , "PM PRE Process End at %s[%s]%cPROCESS_PRE_END PM%d::%s:52\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , PROCESS_MONITOR_Get_Recipe(k,j) );
							}
							else { // 2005.09.13
								//
								EST_PM_PRE_PROCESS_END( j ); // 2016.10.22
								//
								if ( ( Dummy % 10 ) == 2 ) { // 2011.09.30
									if ( ( Dummy / 10 ) == 0 ) {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process End at %s(D%d)[%s]%cPROCESS_PRE_END PM%d:D%d:%s:52%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_PRE );
									}
									else {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process End at %s(D%d-%d)[%s]%cPROCESS_PRE_END PM%d:D%d-%d:%s:52%cD%d-%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE );
									}
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process End at %s(%d)[%s]%cPROCESS_PRE_END PM%d:%d:%s:52%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_PRE );
								}
								//
							}
						}
					}
					else if ( Slot >= PROCESS_INDICATOR_POST ) { // 2002.04.08 // Post // 2005.09.13
						_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , j , ( Slot > PROCESS_INDICATOR_POST ) ); // 2006.02.09
						//-----------------------------------------------------------------------------------------------------------------------------------
						if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { // 2004.05.11
							if ( Slot == PROCESS_INDICATOR_POST ) { // 2005.09.13
								_i_SCH_LOG_LOT_PRINTF( k , "PM POST Process End at %s[%s]%cPROCESS_POST_END PM%d::%s:53\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , PROCESS_MONITOR_Get_Recipe(k,j) );
							}
							else { // 2005.09.13
								//
								EST_PM_POST_PROCESS_END( j ); // 2016.10.22
								//
								if ( ( Dummy % 10 ) == 2 ) { // 2011.09.30
									if ( ( Dummy / 10 ) == 0 ) {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process End at %s(D%d)[%s]%cPROCESS_POST_END PM%d:D%d:%s:53%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_POST );
									}
									else {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process End at %s(D%d-%d)[%s]%cPROCESS_POST_END PM%d:D%d-%d:%s:53%cD%d-%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST );
									}
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process End at %s(%d)[%s]%cPROCESS_POST_END PM%d:%d:%s:53%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_POST );
								}
							}
						}
					}
					//-------------------------------------------------------------------
//					if ( pres ) PROCESS_MONITOR_Set_Finish_Status( k , j , -100 ); // 2011.03.02 // 2013.06.19
					//
					// 2013.06.19
					if ( Finish_Result == 0 ) {
						if ( PROCESS_MONITOR_Set_Finish_Status( k , j , 0 ) ) {
							PROCESS_MONITOR_Set_Finish_Status( k , j , -100 );
						}
					}
					else {
						PROCESS_MONITOR_Set_Finish_Status( k , j , Finish_Result );
					}
					//-------------------------------------------------------------------
					break;
				case SYS_ERROR :
					//=======================================================================================================================
					Finish_Result = -1; // 2013.06.19
					//=======================================================================================================================
					if ( Abort ) {
						PROCESS_SUB_ABORT_STOP_SCENARIO_OPERATION( k , j , FALSE , &Finish_Result ); // 2007.07.24
					}
					else {
						if ( j < BM1 ) { // 2003.09.08
							//=======================================================================================================================
							if      ( ( fail_scenario >=    0 ) && ( fail_scenario <= FAIL_LIKE_DEFAULT_LAST_ITEM ) ) { // 2006.02.03
								// N/A
							} // 2006.02.03
							else if ( ( fail_scenario >= 1000 ) && ( fail_scenario <= FAIL_LIKE_USER_LAST_ITEM    ) ) { // 2006.02.03
								// N/A
							} // 2006.02.03
							else { // 2006.02.03
								fail_scenario = _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( j ); // 2006.02.03
							} // 2006.02.03
							//=======================================================================================================================
							PROCESS_SUB_FAIL_SCENARIO_LEVEL1_OPERATION( k , fail_scenario , TRUE ); // 2007.07.24
							//=======================================================================================================================
							PROCESS_SUB_FAIL_SCENARIO_LEVEL2_OPERATION( k , j , fail_scenario , ( Slot < PROCESS_INDICATOR_PRE ) , &Finish_Result ); // 2007.07.24
							//=======================================================================================================================
						}
						else { // 2003.09.08
//							PROCESS_MONITOR_Set_Finish_xStatus( k , j , -1 ); // 2013.06.19
						}
					}
					if ( Slot < PROCESS_INDICATOR_PRE ) { // 2005.09.13
						Process_Monitor_Sts_Abort[k][j] = 0;
						if ( j < BM1 ) {
							//=======================================================================================================================
							_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , j , FALSE );
							//=======================================================================================================================
//							if ( ( fail_result < 0 ) || ( fail_result > MRES_LIKE_SKIP ) ) fail_result = _i_SCH_PRM_GET_MRES_FAIL_SCENARIO( j ); // 2015.04.30
							if ( ( fail_result < 0 ) || ( fail_result > MRES_LIKE_SKIP ) ) fail_result = _i_SCH_PRM_GET_MRES_FAIL_SCENARIO( j ) % 4; // 2015.04.30
							//=======================================================================================================================
//							PROCESS_SUB_FAIL_RESULT_OPERATION( ( PROCESS_MONITOR_Get_Pointer( k , j ) < 100 ) ? k : PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_Pointer( k , j ) , Slot , j , fail_result , TRUE , Fnotuse , PathIn ); // 2007.07.24 // 2015.04.30
							PROCESS_SUB_FAIL_RESULT_OPERATION( ( PROCESS_MONITOR_Get_Pointer( k , j ) < MAX_CASSETTE_SLOT_SIZE ) ? k : PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_Pointer( k , j ) , Slot , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , fail_result , TRUE , Fnotuse , PathIn ); // 2007.07.24 // 2015.04.30
							//=======================================================================================================================
							PROCESS_SUB_ADD_RUN_COUNT( FALSE , j , Slot , TRUE , Fnotuse ); // 2007.07.24
							//=======================================================================================================================
							//PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , Slot , i , fail_result ); // 2007.07.25 // 2013.05.23
//							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , fail_result ); // 2007.07.25 // 2013.05.23 // 2016.04.26
//							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , fail_result ); // 2007.07.25 // 2013.05.23 // 2016.04.26 // 2016.04.27
							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , ( fail_scenario >= 0 ) ? ( fail_scenario * 10 ) + fail_result : fail_result ); // 2007.07.25 // 2013.05.23 // 2016.04.26 // 2016.04.27
							//=========================================================================================================================
							PROCESS_SUB_EXTEND_PM_END_MESSAGE( k , j , i , fail_result ); // 2007.07.25
							//=======================================================================================================================
						}
						else { // 2006.07.20
							PROCESS_SUB_NORMAL_BM_END_MESSAGE( k , j , i ); // 2007.12.28
						}
					}
					else if ( ( Slot >= PROCESS_INDICATOR_PRE ) && ( Slot < PROCESS_INDICATOR_POST ) ) { // 2002.04.08 // Pre // 2005.09.13
						_i_SCH_TIMER_SET_PROCESS_TIME_END( 2 , j , FALSE );
						//-----------------------------------------------------------------------------------------------------------------------------------
						if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { // 2004.05.11
							if ( Slot == PROCESS_INDICATOR_PRE ) { // 2005.09.13
								_i_SCH_LOG_LOT_PRINTF( k , "PM PRE Process Error at %s[%s:%d]%cPROCESS_PRE_ERROR PM%d::%s:%d:55\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario ); // 2006.03.02
							}
							else { // 2005.09.13
								//
								if ( ( Dummy % 10 ) == 2 ) { // 2011.09.30
									if ( ( Dummy / 10 ) == 0 ) {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Error at %s(D%d)[%s:%d]%cPROCESS_PRE_ERROR PM%d:D%d:%s:%d:55%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , Slot - PROCESS_INDICATOR_PRE); // 2006.03.02
									}
									else {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Error at %s(D%d-%d)[%s:%d]%cPROCESS_PRE_ERROR PM%d:D%d-%d:%s:%d:55%cD%d-%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE); // 2006.03.02
									}
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Error at %s(%d)[%s:%d]%cPROCESS_PRE_ERROR PM%d:%d:%s:%d:55%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , Slot - PROCESS_INDICATOR_PRE); // 2006.03.02
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
					else if ( Slot >= PROCESS_INDICATOR_POST ) { // 2002.04.08 // Post // 2005.09.13
						_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , j , FALSE );
						//-----------------------------------------------------------------------------------------------------------------------------------
						if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { // 2004.05.11
							if ( Slot == PROCESS_INDICATOR_POST ) { // 2005.09.13
								_i_SCH_LOG_LOT_PRINTF( k , "PM POST Process Error at %s[%s:%d]%cPROCESS_POST_ERROR PM%d::%s:%d:56\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario ); // 2006.03.02
							}
							else { // 2005.09.13
								//
								if ( ( Dummy % 10 ) == 2 ) { // 2011.09.30
									if ( ( Dummy / 10 ) == 0 ) {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Error at %s(D%d)[%s:%d]%cPROCESS_POST_ERROR PM%d:D%d:%s:%d:56%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , Slot - PROCESS_INDICATOR_POST ); // 2006.03.02
									}
									else {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Error at %s(D%d-%d)[%s:%d]%cPROCESS_POST_ERROR PM%d:D%d-%d:%s:%d:56%cD%d-%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST ); // 2006.03.02
									}
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Error at %s(%d)[%s:%d]%cPROCESS_POST_ERROR PM%d:%d:%s:%d:56%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , fail_scenario , 9 , Slot - PROCESS_INDICATOR_POST ); // 2006.03.02
								}
								//
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
					//=======================================================================================================================
					PROCESS_MONITOR_Set_Finish_Status( k , j , Finish_Result ); // 2013.06.19
					//=======================================================================================================================
					break;
				case SYS_ABORTED :
					//---------------------------------------------------------
					if ( j < BM1 ) { // 2003.09.08
						if ( ( _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO() / 2 ) == 1 ) { // 2003.09.03
//							SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( j , 0 , 1 , ABORTWAIT_FLAG_ABORTWAIT , 0 ); // 2003.09.03 // 2015.07.02
							SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( j , 0 , 1 , ( Slot < PROCESS_INDICATOR_PRE ) ? ABORTWAIT_FLAG_ABORTWAIT : ABORTWAIT_FLAG_NONE , 0 ); // 2003.09.03 // 2015.07.02
						}
						else { // 2003.09.03
							//-----------------------------------------------------------------------------
//							SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( j , ABORTWAIT_FLAG_ABORTWAIT ); // 2003.06.13 // 2015.07.02
							SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( j , ( Slot < PROCESS_INDICATOR_PRE ) ? ABORTWAIT_FLAG_ABORTWAIT : ABORTWAIT_FLAG_NONE ); // 2003.06.13 // 2015.07.02
							//-----------------------------------------------------------------------------
						}
					}
					//============================================================================================================
					PROCESS_SUB_END_OPERATION( k , TRUE );
					//=======================================================================================================================
					Finish_Result = -1; // 2013.06.19
					//=======================================================================================================================
					if ( Abort ) {
						//=======================================================================================================================
						PROCESS_SUB_ABORT_STOP_SCENARIO_OPERATION( k , j , FALSE , &Finish_Result ); // 2007.07.24
						//=======================================================================================================================
					}
//					else {
//						PROCESS_MONITOR_Set_Finish_xStatus( k , j , -1 ); // 2013.06.19
//					}
					//============================================================================================================
					if ( Slot < PROCESS_INDICATOR_PRE ) { // 2005.09.13
						Process_Monitor_Sts_Abort[k][j] = 0;
						if ( j < BM1 ) {
							//=======================================================================================================================
							_i_SCH_TIMER_SET_PROCESS_TIME_END( 0 , j , FALSE );
							//=======================================================================================================================
							if ( ( abort_result < 0 ) || ( abort_result > MRES_LIKE_SKIP ) ) abort_result = _i_SCH_PRM_GET_MRES_ABORT_SCENARIO( j );
							//=======================================================================================================================
//							PROCESS_SUB_FAIL_RESULT_OPERATION( ( PROCESS_MONITOR_Get_Pointer( k , j ) < 100 ) ? k : PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_Pointer( k , j ) , Slot , j , abort_result , TRUE , Fnotuse , PathIn ); // 2007.07.24 // 2015.04.30
							PROCESS_SUB_FAIL_RESULT_OPERATION( ( PROCESS_MONITOR_Get_Pointer( k , j ) < MAX_CASSETTE_SLOT_SIZE ) ? k : PROCESS_MONITOR_Get_Pointer( k , j ) , PROCESS_MONITOR_Get_Pointer( k , j ) , Slot , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , abort_result , TRUE , Fnotuse , PathIn ); // 2007.07.24 // 2015.04.30
							//=======================================================================================================================
							PROCESS_SUB_ADD_RUN_COUNT( FALSE , j , Slot , TRUE , Fnotuse ); // 2007.07.24
							//=======================================================================================================================
							//PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , Slot , i , abort_result ); // 2007.07.25 // 2013.05.23
//							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , abort_result ); // 2007.07.25 // 2013.05.23 // 2016.04.26
							PROCESS_SUB_NORMAL_PM_END_MESSAGE( Fnotuse , Dummy , WfrNotCheck , k , PROCESS_MONITOR_Get_Pointer( k , j ) , j , PROCESS_MONITOR_Get_ChP(k,j) , PROCESS_MONITOR_Get_ChS(k,j) , Slot , PROCESS_MONITOR_Get_Recipe(k,j) , PROCESS_MONITOR_Get_Last(k,j) , i , abort_result ); // 2007.07.25 // 2013.05.23 // 2016.04.26
							//=========================================================================================================================
							PROCESS_SUB_EXTEND_PM_END_MESSAGE( k , j , i , abort_result ); // 2007.07.25
							//=======================================================================================================================
						}
						else { // 2006.07.20
							PROCESS_SUB_NORMAL_BM_END_MESSAGE( k , j , i ); // 2007.12.28
						}
					}
					else if ( ( Slot >= PROCESS_INDICATOR_PRE ) && ( Slot < PROCESS_INDICATOR_POST ) ) { // 2002.04.08 // Pre // 2005.09.13
						_i_SCH_TIMER_SET_PROCESS_TIME_END( 2 , j , FALSE );
						//-----------------------------------------------------------------------------------------------------------------------------------
						if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) { // 2004.05.11
							if ( Slot == PROCESS_INDICATOR_PRE ) { // 2005.09.13
								_i_SCH_LOG_LOT_PRINTF( k , "PM PRE Process Abort at %s[%s]%cPROCESS_PRE_ABORT PM%d::%s:58\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , PROCESS_MONITOR_Get_Recipe(k,j) );
							}
							else { // 2005.09.13
								//
								if ( ( Dummy % 10 ) == 2 ) { // 2011.09.30
									if ( ( Dummy / 10 ) == 0 ) {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Abort at %s(D%d)[%s]%cPROCESS_PRE_ABORT PM%d:D%d:%s:58%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_PRE );
									}
									else {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Abort at %s(D%d-%d)[%s]%cPROCESS_PRE_ABORT PM%d:D%d-%d:%s:58%cD%d-%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_PRE );
									}
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( k , "PM Pre Process Abort at %s(%d)[%s]%cPROCESS_PRE_ABORT PM%d:%d:%s:58%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_PRE , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_PRE );
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
					else if ( Slot >= PROCESS_INDICATOR_POST ) { // 2002.04.08 // Post // 2005.09.13
						_i_SCH_TIMER_SET_PROCESS_TIME_END( 1 , j , FALSE );
						//-----------------------------------------------------------------------------------------------------------------------------------
						if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) { // 2004.05.11
							if ( Slot == PROCESS_INDICATOR_POST ) { // 2005.09.13
								_i_SCH_LOG_LOT_PRINTF( k , "PM POST Process Abort at %s[%s]%cPROCESS_POST_ABORT PM%d::%s:59\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , PROCESS_MONITOR_Get_Recipe(k,j) );
							}
							else { // 2005.09.13
								//
								if ( ( Dummy % 10 ) == 2 ) { // 2011.09.30
									if ( ( Dummy / 10 ) == 0 ) {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Abort at %s(D%d)[%s]%cPROCESS_POST_ABORT PM%d:D%d:%s:59%cD%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_POST );
									}
									else {
										_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Abort at %s(D%d-%d)[%s]%cPROCESS_POST_ABORT PM%d:D%d-%d:%s:59%cD%d-%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , ( Dummy / 10 ) , Slot - PROCESS_INDICATOR_POST );
									}
								}
								else {
									_i_SCH_LOG_LOT_PRINTF( k , "PM Post Process Abort at %s(%d)[%s]%cPROCESS_POST_ABORT PM%d:%d:%s:59%c%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( j ) , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , j - PM1 + 1 , Slot - PROCESS_INDICATOR_POST , PROCESS_MONITOR_Get_Recipe(k,j) , 9 , Slot - PROCESS_INDICATOR_POST );
								}
								//
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
					//=======================================================================================================================
					PROCESS_MONITOR_Set_Finish_Status( k , j , Finish_Result ); // 2013.06.19
					//=======================================================================================================================
					break;
				}
			}
			//
			LeaveCriticalSection( &CR_PM_DATA_RESET2[j] ); // 2011.04.18
			//
		}
		if ( Find ) Process_Monitor_Sts_Find[k] = TRUE;
		else        Process_Monitor_Sts_Find[k] = FALSE;
	}
	if ( f > 0 )
		GUI_PROCESS_LOG_MONITOR_RUN();
	else
		GUI_PROCESS_LOG_MONITOR_RESET();
}
//
BOOL PROCESS_MONITOR_STATUS_for_Abort_Remain( int side ) { // 2003.12.01
	int c;
	for ( c = 0 ; c < MAX_PM_CHAMBER_DEPTH ; c++ ) {
		if ( ( PROCESS_MONITOR_Get_Status( side , c ) > 0 ) && ( PROCESS_MONITOR_Get_Status( side , c ) < PROCESS_INDICATOR_PRE ) ) return TRUE; // 2005.09.13
	}
	return FALSE;
}
