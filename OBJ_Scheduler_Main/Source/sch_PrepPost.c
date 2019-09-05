#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "Equip_Handling.h"
#include "DLL_Interface.h"
#include "sch_CassetteSupply.h"
#include "sch_sub.h"
#include "sch_sdm.h"
#include "sch_processmonitor.h"
#include "sch_prm.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_preprcs.h"
#include "iolog.h"
#include "Errorcode.h"
#include "RcpFile_Handling.h"
#include "Utility.h"

#include "INF_sch_CommonUser.h"
#include "INF_sch_macro.h"

int  _in_LOTPRE_CLUSTER_DIFFER_MODE2[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2017.09.22

int _i_SCH_MODULE_GET_USE_ENABLE_Sub( int Chamber , BOOL Always , int side );
void _SCH_CASSETTE_Set_Count_Action( int s , BOOL ); // 2018.10.01
//------------------------------------------------------------------------------------------
//   DEFAULT_RECIPE_CHANGE
//			READY|PM?|{RecipeFileName}	: RUN READY_PROCESS(If PROCESS Runnig then run as READY9): Spawn       : SameLotSkip
//			READY2						: If PROCESS READY then RUN READY_PROCESS else Skip      : Spawn       : SameLotSkip
//			READY3						: RUN READY_PROCESS(If PROCESS Runnig then run as READY9): Wait Finish : SameLotSkip
//			READY4						: If PROCESS READY then RUN READY_PROCESS else Skip      : Wait Finish : SameLotSkip
//			READY5						: RUN READY_PROCESS(If PROCESS Runnig then run as READY9): Spawn       : LotAlways(Force)
//			READY6						: If PROCESS READY then RUN READY_PROCESS else Skip      : Spawn       : LotAlways(Force)
//			READY7						: RUN READY_PROCESS(If PROCESS Runnig then run as READY9): Wait Finish : LotAlways(Force)
//			READY8						: If PROCESS READY then RUN READY_PROCESS else Skip      : Wait Finish : LotAlways(Force)
//			READY9						: RUN READY_PROCESS BEFORE FIRST WAFER PLACE             : x           : SameLot/LotAlways(Force)
//			READY10						: RUN READY_PROCESS(If PROCESS Runnig then run as READY9): Spawn       : SameLotSkip But If PROCESS Runnig LotAlways(Force)
//			READY11						: RUN READY_PROCESS(If PROCESS Runnig then run as READY9): Wait Finish : SameLotSkip But If PROCESS Runnig LotAlways(Force)
//
//			READY12						: RUN READY_PROCESS BEFORE FIRST WAFER PLACE             : x           : SameLotSkip/LotAlways(User)							// 2017.09.07
//			READY13						: RUN READY_PROCESS BEFORE FIRST WAFER PLACE             : x           : SameLotSkip/LotAlways(User)							// 2017.09.07
//
//			END							: RUN END_PROCESS
//			END2						: If PROCESS READY then RUN END_PROCESS else Skip
//			END3						: If PROCESS READY then RUN END_PROCESS else RUN_EVENT END_PROCESS
//------------------------------------------------------------------------------------------
//extern int SDM_4_LOTFIRST_SIGNAL[ MAX_CHAMBER ];
//extern int sch4_RERUN_FST_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
//extern int sch4_RERUN_FST_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
//extern int sch4_RERUN_END_S1_TAG[ MAX_CHAMBER ]; // 2005.07.29
//extern int sch4_RERUN_END_S2_TAG[ MAX_CHAMBER ]; // 2005.07.29
//extern int sch4_RERUN_END_S3_TAG[ MAX_CHAMBER ]; // 2005.11.11
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  xinside_global_BM_Return[MAX_CASSETTE_SIDE]; // 2007.06.08

int  xinside_global_Run_Tag[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2002.08.27 // 2005.08.18
int  xinside_global_Run2_Tag[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2002.08.27 // 2005.08.18
int  xinside_global_PrcsRunCheck[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2005.02.17

	// 0 : Not Use
	// 1 : Lot Pre Running / Need Finish Check
	// 2 : Lot Pre Running / NoNeed Finish Check
	// 3 : Lot Pre Running / Checked Finish Check


//int	 xinside_READY_RECIPE_STEP2[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
//
//int  xinside_READY_RECIPE_USE[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
//char xinside_READY_RECIPE_NAME[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ][512+1]; // 2007.01.04
//
//int  xinside_READY_RECIPE_MINTIME[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
//
//int  xinside_END_RECIPE_USE[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
//char xinside_END_RECIPE_NAME[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ][512+1]; // 2007.01.04
//
//int  xinside_END_RECIPE_MINTIME[ MAX_CASSETTE_SIDE ][ MAX_CHAMBER ];
//
//int	  _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( int side , int ch ) { return xinside_READY_RECIPE_STEP2[side][ch]; }
//void  _i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( int side , int ch , int data ) { xinside_READY_RECIPE_STEP2[side][ch] = data; }

//int   _i_SCH_PRM_GET_inside_READY_RECIPE_USE( int side , int ch )	{ return xinside_READY_RECIPE_USE[side][ch]; }
//int   _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME( int side , int ch )	{ return xinside_READY_RECIPE_MINTIME[side][ch]; }
//char *_i_SCH_PRM_GET_inside_READY_RECIPE_NAME( int side , int ch )	{ return xinside_READY_RECIPE_NAME[side][ch]; }

//int   _i_SCH_PRM_GET_inside_END_RECIPE_USE( int side , int ch )	{ return xinside_END_RECIPE_USE[side][ch]; }
//int   _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( int side , int ch )	{ return xinside_END_RECIPE_MINTIME[side][ch]; }
//char *_i_SCH_PRM_GET_inside_END_RECIPE_NAME( int side , int ch )	{ return xinside_END_RECIPE_NAME[side][ch]; }

MAKE_DEFAULT_INT_PARAMETER3(  inside_READY_RECIPE_STEP2   , MAX_CASSETTE_SIDE , MAX_CHAMBER )

MAKE_DEFAULT_INT_PARAMETER3(  inside_READY_RECIPE_USE     , MAX_CASSETTE_SIDE , MAX_CHAMBER )
MAKE_DEFAULT_CHAR_PARAMETER3( inside_READY_RECIPE_NAME    , MAX_CASSETTE_SIDE , MAX_CHAMBER )
MAKE_DEFAULT_INT_PARAMETER3(  inside_READY_RECIPE_MINTIME , MAX_CASSETTE_SIDE , MAX_CHAMBER )

MAKE_DEFAULT_INT_PARAMETER3(  inside_READY_RECIPE_TYPE    , MAX_CASSETTE_SIDE , MAX_CHAMBER ) // 2017.10.16

MAKE_DEFAULT_CHAR_PARAMETER2( inside_READY_RECIPE_NAME_LAST , MAX_CHAMBER ) // 2017.09.07
MAKE_DEFAULT_INT_PARAMETER2( inside_READY_RECIPE_TYPE_LAST , MAX_CHAMBER ) // 2017.10.16
MAKE_DEFAULT_INT_PARAMETER2( inside_READY_RECIPE_INDX_LAST , MAX_CHAMBER ) // 2017.10.16

MAKE_DEFAULT_CHAR_PARAMETER2( inside_READY_RECIPE_NAME_OLD_LAST , MAX_CHAMBER ) // 2017.10.16
MAKE_DEFAULT_INT_PARAMETER2( inside_READY_RECIPE_TYPE_OLD_LAST , MAX_CHAMBER ) // 2017.10.16
MAKE_DEFAULT_INT_PARAMETER2( inside_READY_RECIPE_INDX_OLD_LAST , MAX_CHAMBER ) // 2017.10.16

MAKE_DEFAULT_INT_PARAMETER3(  inside_END_RECIPE_USE       , MAX_CASSETTE_SIDE , MAX_CHAMBER )
MAKE_DEFAULT_CHAR_PARAMETER3( inside_END_RECIPE_NAME      , MAX_CASSETTE_SIDE , MAX_CHAMBER )
MAKE_DEFAULT_INT_PARAMETER3(  inside_END_RECIPE_MINTIME   , MAX_CASSETTE_SIDE , MAX_CHAMBER )

//
// 2016.12.09
MAKE_DEFAULT_INT_PARAMETER3( inside_SIM_TIME1 , MAX_CASSETTE_SIDE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER3( inside_SIM_TIME2 , MAX_CASSETTE_SIDE , MAX_CHAMBER );
MAKE_DEFAULT_INT_PARAMETER3( inside_SIM_TIME3 , MAX_CASSETTE_SIDE , MAX_CHAMBER );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//CRITICAL_SECTION CR_LOTEND_DELAY; // 2003.08.04 // 2009.04.13
CRITICAL_SECTION CR_LOTEND_DELAY[2]; // 2003.08.04 // 2009.04.13
//------------------------------------------------------------------------------------------
//DWORD			End_Set_Delay_Time = 5000; // 2003.08.04 // 2009.04.13
//DWORD			End_Set_Delay_Time_BUFFER = 0; // 2003.08.04 // 2009.04.13
DWORD			End_Set_Delay_Time = 3000; // 2003.08.04 // 2009.04.13
DWORD			End_Set_Delay_Time_BUFFER[2] = { 0 , 0 }; // 2003.08.04 // 2009.04.13

int  xinside_global_restart_prcs[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE]; // 2011.12.14
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void INIT_SCHEDULER_CONTROL_PREPOST_DATA( int apmode , int side ) {
	int i;
	if ( apmode == 0 ) {
//		InitializeCriticalSection( &CR_LOTEND_DELAY ); // 2009.04.13
		InitializeCriticalSection( &CR_LOTEND_DELAY[0] ); // 2009.04.13
		InitializeCriticalSection( &CR_LOTEND_DELAY[1] ); // 2009.04.13
	}
	else { // 2011.12.14
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) xinside_global_restart_prcs[side][i] = 0;
	}
	//
	if ( apmode == 3 ) { // 2017.10.16
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_OLD_LAST( i , "" );
			_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_OLD_LAST( i , 0 );
			_i_SCH_PRM_SET_inside_READY_RECIPE_INDX_OLD_LAST( i , 0 );
			//
			_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_LAST( i , "" );
			_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_LAST( i , 0 );
			_i_SCH_PRM_SET_inside_READY_RECIPE_INDX_LAST( i , 0 );
		}
		//
	}
	//
}

void Scheduler_Restart_Process_Set( int side , int pt , int ch ) { // 2011.12.15
	xinside_global_restart_prcs[side][pt] = ch;
}

void Scheduler_Restart_Process_Part( int side ) { // 2011.12.15
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( xinside_global_restart_prcs[side][i] != 0 ) {
			//
			if ( _SCH_MACRO_CHECK_PROCESSING( xinside_global_restart_prcs[side][i] ) <= 0 ) {
				//
				_SCH_MACRO_MAIN_PROCESS_PART_SIMPLE( side , i , -1 , xinside_global_restart_prcs[side][i] , 0 , 1036 );
				//
				xinside_global_restart_prcs[side][i] = 0;
			}
			//
		}
	}
	//
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_End_Set_Delay_Time( DWORD timedata ) { // 2003.08.04
	End_Set_Delay_Time = timedata;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Module Check Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_Module_Use_Reference_Data_Refresh( int side , int mode ) { // 2003.01.25
	int i;
	for ( i = CM1 ; i <= FM ; i++ ) {
		if ( ( i != AL ) && ( i != IC ) ) {
//			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_01_USE ) { // 2003.10.25
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2003.10.25
				if ( mode == 0 ) {
					_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
					//=========================================================================
					// 2005.11.10
					//=========================================================================
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
						_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
						_i_SCH_LOG_LOT_PRINTF( side , "Module %s Run Flag Status Error 1%cMDLSTSERROR 1:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
					}
					//=========================================================================
				}
				else if ( mode == 1 ) {
					if ( i < PM1 ) {
						_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
						//=========================================================================
						// 2005.11.10
						//=========================================================================
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
							_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
							_i_SCH_LOG_LOT_PRINTF( side , "Module %s Run Flag Status Error 2%cMDLSTSERROR 2:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
						}
						//=========================================================================
					}
				}
			}
			_i_SCH_MODULE_Set_Mdl_Use_Flag( side , i , MUF_00_NOTUSE );
		}
	}
}


void Scheduler_LotPre_Data_Set( BOOL normal , int side , int ch , char *processname , int processtype ) { // 2017.10.18
	int x , ch2;
	int i;
	//
	if ( normal ) {
		if ( ( processtype / 100 ) > 0 ) return;
	}
	else {
		if ( ( processtype / 100 ) <= 0 ) return;
	}
	//
	x = _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) / 10;
	//
	_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_OLD_LAST( ch , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST( ch ) );
	_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_OLD_LAST( ch , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( ch ) );
	_i_SCH_PRM_SET_inside_READY_RECIPE_INDX_OLD_LAST( ch , _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) );
	//
	_i_SCH_PRM_SET_inside_READY_RECIPE_INDX_LAST( ch , ( ( x+1 ) * 10 ) + side );
	//
	switch( processtype / 100 ) {
	case 1 : // P12 P34
	case 2 : // P13 P24
	case 3 : // P14 P23
	case 4 :
		_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_LAST( ch , "" );
		_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_LAST( ch , 0 );
		break;
		//
	default :
		_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_LAST( ch , processname );
		_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_LAST( ch , processtype );
		break;
	}
	//
	ch2 = 0;
	//
	switch( processtype / 100 ) {
	case 1 : // P12 P34
	case 5 : // P12 P34
		//
		if      ( ch == ( PM1 + 1 - 1 ) ) {
			ch2 = PM1 + 2 - 1;
		}
		else if ( ch == ( PM1 + 2 - 1 ) ) {
			ch2 = PM1 + 1 - 1;
		}
		else if ( ch == ( PM1 + 3 - 1 ) ) {
			ch2 = PM1 + 4 - 1;
		}
		else if ( ch == ( PM1 + 4 - 1 ) ) {
			ch2 = PM1 + 3 - 1;
		}
		break;
		//
	case 2 : // P13 P24
	case 6 : // P13 P24
		//
		if      ( ch == ( PM1 + 1 - 1 ) ) {
			ch2 = PM1 + 3 - 1;
		}
		else if ( ch == ( PM1 + 2 - 1 ) ) {
			ch2 = PM1 + 4 - 1;
		}
		else if ( ch == ( PM1 + 3 - 1 ) ) {
			ch2 = PM1 + 1 - 1;
		}
		else if ( ch == ( PM1 + 4 - 1 ) ) {
			ch2 = PM1 + 2 - 1;
		}
		break;
		//
	case 3 : // P14 P23
	case 7 : // P14 P23
		//
		if      ( ch == ( PM1 + 1 - 1 ) ) {
			ch2 = PM1 + 4 - 1;
		}
		else if ( ch == ( PM1 + 2 - 1 ) ) {
			ch2 = PM1 + 3 - 1;
		}
		else if ( ch == ( PM1 + 3 - 1 ) ) {
			ch2 = PM1 + 2 - 1;
		}
		else if ( ch == ( PM1 + 4 - 1 ) ) {
			ch2 = PM1 + 1 - 1;
		}
		break;
		//
	}
	//
	if ( ch2 == 0 ) return;
	//
	x = _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch2 ) / 10;
	//
	_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_OLD_LAST( ch2 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST( ch2 ) );
	_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_OLD_LAST( ch2 , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( ch2 ) );
	_i_SCH_PRM_SET_inside_READY_RECIPE_INDX_OLD_LAST( ch2 , _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch2 ) );
	//
	switch( processtype / 100 ) {
	case 1 : // P12 P34
	case 2 : // P13 P24
	case 3 : // P14 P23
	case 4 :
		_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_LAST( ch2 , "" );
		_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_LAST( ch2 , 0 );
		break;
	default :
		_i_SCH_PRM_SET_inside_READY_RECIPE_NAME_LAST( ch2 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( side , ch2 ) );
		_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE_LAST( ch2 , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( side , ch2 ) );
		break;
	}
	//
	_i_SCH_PRM_SET_inside_READY_RECIPE_INDX_LAST( ch2 , ( ( x+1 ) * 10 ) + side );
	//
	// 2017.11.01
	//
	if ( normal ) return;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( i == side ) continue;
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if      ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch2 ) == MUF_98_USE_to_DISABLE_RAND ) ) {
		}
		else if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_98_USE_to_DISABLE_RAND ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch2 ) == MUF_01_USE ) ) {
		}
		else if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_98_USE_to_DISABLE_RAND ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch2 ) == MUF_98_USE_to_DISABLE_RAND ) ) {
		}
		else {
			continue;
		}
		//
		if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , ch ) == 100 /* 처음 시작시 이전 Lot이 사용중이었음 설정 되었을때 */ ) {
			//
			_in_LOTPRE_CLUSTER_DIFFER_MODE2[i][ch] = 9000 + side;
			//
			_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( i , ch ); /* Chamber 앞에서 Pre 예약 */
			//
		}
		//
		if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( i , ch2 ) == 100 /* 처음 시작시 이전 Lot이 사용중이었음 설정 되었을때 */ ) {
			//
			_in_LOTPRE_CLUSTER_DIFFER_MODE2[i][ch2] = 9000 + side;
			//
			_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( i , ch2 ); /* Chamber 앞에서 Pre 예약 */
			//
		}
		//
	}
	//
}



void Scheduler_LotPre_Data_Supply( int side , int pointer ) { // 2017.10.19
	int pr , i , j , m;
	//
	pr = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
	//
	for ( i = 0 ; i < pr ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pointer , i , j );
			//
			if ( m > 0 ) {
				//
				Scheduler_LotPre_Data_Set( FALSE , side , m , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( side , m ) , _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( side , m ) );
				//
			}
		}
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Regist_Lot_PrePost_Recipe Part // 2005.02.17
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


// int mode
//
//	>= 1000	File 이름만 가져오기
//
//		1000,1001,1002	Wafer Type
//		else			Lot Type
//
//		return
//			0	Locking 0 File Name
//			1	Locking 1 File Name
//			2	Locking 2 File Name
//			9	미지정 파일
//
//	else	Locking File 복사하기
//
//		return
//			-1	Locking File Copy Fail
//			0	Locking 0 : Not Need to Copy
//			1	Not Need to Copy : Not FIle Type
//			2	Locking File Copy OK
//			9	미지정 파일
//

int _i_SCH_SUB_COPY_LOCKING_FILE_WHILE_RUNNING( int side , int pt , int ch , int wfr , char *filename , int mode , int etc ) { // 2018.01.05
	//
	char RunGroup[255+1];
	char RunFile[255+1];
	char BufferN[512];
	int Recipe_SHPT;
	//
//	if ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 0 ) return -1; // 2019.02.13
	//
	if ( mode >= 1000 ) { // 2018.03.19
		//
		Recipe_SHPT = 0;
		//
		if ( ( filename[0] == '+' ) && ( ( filename[1] == '\\' ) || ( filename[1] == '/' ) ) ) Recipe_SHPT = 2;
		//
		if ( strlen( filename ) <= 0 ) return 9;
		if ( filename[0] == '?' ) return 9;
		if ( strlen( filename + Recipe_SHPT ) <= 0 ) return 9;
		if ( filename[Recipe_SHPT] == '?' ) return 9;
		//
		UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(side) , filename + Recipe_SHPT , RunGroup , 255 , RunFile , 255 );
		//
		if ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 0 ) { // 2019.02.13
			//
			_snprintf( filename , 511 , "%s/%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( ch ) , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ch ) , RunFile );
			//
			return 0;
			//
		}
		else {
			if ( SCHEDULER_GET_RECIPE_LOCKING( side ) >= 2 ) {
				//
				if ( ( ( mode % 1000 ) == 0 ) || ( ( mode % 1000 ) == 1 ) || ( ( mode % 1000 ) == 2 ) ) {
					_snprintf( BufferN , 511 , "%d$%d$%s$%s$%s" , side , wfr , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , RunFile );
				}
				else {
					_snprintf( BufferN , 511 , "%d$%s$%s$%s" , side , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , RunFile );
				}
				//
			}
			else {
				_snprintf( BufferN , 511 , "%d$%s$%s$%s" , side , RunGroup , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ch ) , RunFile );
				//
			}
			//
			Change_Dirsep_to_Dollar( BufferN );
			//
			_snprintf( filename , 511 , "%s/tmp/%s" , _i_SCH_PRM_GET_DFIX_TMP_PATH() , BufferN );
			//
			return SCHEDULER_GET_RECIPE_LOCKING( side );
			//
		}
	}
	else {
		//
		if ( SCHEDULER_GET_RECIPE_LOCKING( side ) == 0 ) return 0; // 2019.02.13
		//
		if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( ch ) != 0 ) return 1; // 2019.02.13
		//
		Recipe_SHPT = RECIPE_FILE_PROCESS_File_Check( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , 1 , side , pt , ch , wfr , filename , mode , etc );
		//
		switch( Recipe_SHPT ) {
		case 0 : // ok
			return 2;
			break;
		case 1 : // fail
			return -1;
			break;
		case 2 : // null
			return 9;
			break;
		}
		//
	}
	//
	return 0;
	//
}


void Scheduler_Regist_Lot_PrePost_Recipe( int side ) {
	int i;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
/*
		xinside_READY_RECIPE_STEP2[side][i] = 0;
		//
		xinside_READY_RECIPE_USE[side][i] = _i_SCH_PRM_GET_READY_RECIPE_USE( i );
		xinside_READY_RECIPE_MINTIME[side][i] = _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i );
		//
//		strncpy( xinside_READY_RECIPE_NAME[side][i] , _i_SCH_PRM_GET_READY_RECIPE_NAME( i ) , 64 ); // 2007.01.04
		strncpy( xinside_READY_RECIPE_NAME[side][i] , _i_SCH_PRM_GET_READY_RECIPE_NAME( i ) , 512 ); // 2007.01.04
		//
		xinside_END_RECIPE_USE[side][i] = _i_SCH_PRM_GET_END_RECIPE_USE( i );
		xinside_END_RECIPE_MINTIME[side][i] = _i_SCH_PRM_GET_END_RECIPE_MINTIME( i );
//		strncpy( xinside_END_RECIPE_NAME[side][i] , _i_SCH_PRM_GET_END_RECIPE_NAME( i ) , 64 ); // 2007.01.04
		strncpy( xinside_END_RECIPE_NAME[side][i] , _i_SCH_PRM_GET_END_RECIPE_NAME( i ) , 512 ); // 2007.01.04
*/
		//
		// 2016.12.09
		_i_SCH_PRM_SET_inside_SIM_TIME1( side , i , _i_SCH_PRM_GET_SIM_TIME1( i ) );
		_i_SCH_PRM_SET_inside_SIM_TIME2( side , i , _i_SCH_PRM_GET_SIM_TIME2( i ) );
		_i_SCH_PRM_SET_inside_SIM_TIME3( side , i , _i_SCH_PRM_GET_SIM_TIME3( i ) );
		//
		_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( side , i , 0 /* 사용안함 설정 */ );
		//
		_i_SCH_PRM_SET_inside_READY_RECIPE_USE( side , i , _i_SCH_PRM_GET_READY_RECIPE_USE( i ) );
		_i_SCH_PRM_SET_inside_READY_RECIPE_MINTIME( side , i , _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i ) );
		//
		_i_SCH_PRM_SET_inside_READY_RECIPE_TYPE( side , i , _i_SCH_PRM_GET_READY_RECIPE_TYPE( i ) ); // 2017.10.16
		//
		_i_SCH_PRM_SET_inside_READY_RECIPE_NAME( side ,i , _i_SCH_PRM_GET_READY_RECIPE_NAME( i ) );
		//
		_i_SCH_PRM_SET_inside_END_RECIPE_USE( side , i , _i_SCH_PRM_GET_END_RECIPE_USE( i ) );
		_i_SCH_PRM_SET_inside_END_RECIPE_MINTIME( side , i , _i_SCH_PRM_GET_END_RECIPE_MINTIME( i ) );
		_i_SCH_PRM_SET_inside_END_RECIPE_NAME( side , i , _i_SCH_PRM_GET_END_RECIPE_NAME( i ) );
		//
		/*
		//
		// 2017.09.22
		//
		if ( !STRCMP_L( _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST( i ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( side , i ) ) ) {
			_in_LOTPRE_CLUSTER_DIFFER_MODE2[side][i] = 1;
		}
		else {
			_in_LOTPRE_CLUSTER_DIFFER_MODE2[side][i] = 0;
		}
		//
		*/
		//
		_in_LOTPRE_CLUSTER_DIFFER_MODE2[side][i] = 0; // 2017.10.16
		//
//		if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 0 ) { // 2010.09.28 // 2014.02.08
		if ( SCHEDULER_GET_RECIPE_LOCKING( side ) != 0 ) { // 2010.09.28 // 2014.02.08
			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( side , i ) != 0 ) {
//				RECIPE_FILE_PROCESS_File_Check( 1 , side , -1 , i , 0 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(side,i) , 11 , 0 ); // 2014.02.08
				RECIPE_FILE_PROCESS_File_Check( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , 1 , side , -1 , i , 0 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(side,i) , 11 , 0 ); // 2014.02.08
			}
			if ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( side , i ) != 0 ) {
//				RECIPE_FILE_PROCESS_File_Check( 1 , side , -1 , i , 0 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME(side,i) , 12 , 0 ); // 2014.02.08
				RECIPE_FILE_PROCESS_File_Check( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , 1 , side , -1 , i , 0 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME(side,i) , 12 , 0 ); // 2014.02.08
			}
		}
		//
	}
}


void Scheduler_Make_Mdl_String( int side , char *strbuffer ) {
	int i;
	strcpy( strbuffer , "" );
	for ( i = PM1 ; i < TM ; i++ ) {
		if ( ( i == AL ) || ( i == IC ) ) continue; // 2005.12.20
		if ( i == BM1 ) strcat( strbuffer , ":" ); // 2005.12.20
		if ( i != PM1 ) strcat( strbuffer , ":" );
		switch( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) ) {
		case MUF_00_NOTUSE					: strcat( strbuffer , "-" ); break;	//							0
		case MUF_01_USE						: strcat( strbuffer , "1" ); break;	//	INC		CTC_InUse		1
		case MUF_02_USE_to_END				: strcat( strbuffer , "2" ); break;	//	DEC						2
		case MUF_03_USE_to_END_DISABLE		: strcat( strbuffer , "3" ); break;	//	DEC						3
		case MUF_04_USE_to_PREND_EF_LP		: strcat( strbuffer , "4" ); break;	//	DEC						4
		case MUF_05_USE_to_PRENDNA_EF_LP	: strcat( strbuffer , "5" ); break;	//	DEC						5
		case 6								: strcat( strbuffer , "6" ); break;
		case MUF_07_USE_to_PREND_DECTAG		: strcat( strbuffer , "7" ); break;	//	DEC						7
		case MUF_08_USE_to_PRENDNA_DECTAG	: strcat( strbuffer , "8" ); break;	//	DEC						8
		case 9								: strcat( strbuffer , "9" ); break;
		case 10								: strcat( strbuffer , "A" ); break;
		case 11								: strcat( strbuffer , "B" ); break;
		case MUF_31_USE_to_NOTUSE_SEQMODE	: strcat( strbuffer , "S" ); break;	//	DEC						S
		case MUF_71_USE_to_PREND_EF_XLP		: strcat( strbuffer , "P" ); break;	//	DEC		CTC_InUse		P
		case MUF_81_USE_to_PREND_RANDONLY	: strcat( strbuffer , "o" ); break;	//	XDEC	CTC_InUse		o
		case MUF_90_USE_to_XDEC_FROM		: strcat( strbuffer , "X" ); break;	//	N/A						X
		case MUF_97_USE_to_SEQDIS_RAND		: strcat( strbuffer , "s" ); break;	//	XDEC					s
		case MUF_98_USE_to_DISABLE_RAND		: strcat( strbuffer , "r" ); break;	//	XDEC					r
		case MUF_99_USE_to_DISABLE			: strcat( strbuffer , "d" ); break;	//	XDEC					d
		default								: strcat( strbuffer , "?" ); break;
		}
		switch( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) ) {
		case 0  : strcat( strbuffer , "-" ); break;
		case 1  : strcat( strbuffer , "1" ); break;
		case 2  : strcat( strbuffer , "2" ); break;
		case 3  : strcat( strbuffer , "3" ); break;
		case 4  : strcat( strbuffer , "4" ); break;
		case 5  : strcat( strbuffer , "5" ); break;
		case 6  : strcat( strbuffer , "6" ); break;
		case 7  : strcat( strbuffer , "7" ); break;
		case 8  : strcat( strbuffer , "8" ); break;
		case 9  : strcat( strbuffer , "9" ); break;
		case 10 : strcat( strbuffer , "A" ); break;
		case 11 : strcat( strbuffer , "B" ); break;
		case 12 : strcat( strbuffer , "C" ); break;
		case 13 : strcat( strbuffer , "D" ); break;
		case 14 : strcat( strbuffer , "E" ); break;
		case 15 : strcat( strbuffer , "F" ); break;
		default : strcat( strbuffer , "?" ); break;
		}
	}
}
//
//void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch ) { // 2018.06.22
void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch , char *recipe , int mode ) { // 2018.06.22
	if ( PROCESS_MONITOR_Run_and_Get_Status( ch ) <= 0 ) {
		if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , ch ) == SYS_RUNNING ) {
//			PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , "Manual" , 1 , FALSE ); // 2007.03.16 // 2014.02.13
			//
			if ( mode > 0 ) { // 2018.06.22
				PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_PRE + mode , CHECKING_SIDE , -1 , -1 , -1 , -1 , ( recipe == NULL ) ? "Manual" : recipe , 0 , FALSE , 1 );
			}
			else {
//				PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , "Manual" , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13 // 2018.06.22
				PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , ( recipe == NULL ) ? "Manual" : recipe , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13 // 2018.06.22
			}
			//
			if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
				if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
//					PROCESS_MONITOR_Setting1( ch + 1 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , "Manual" , 1 , FALSE ); // 2007.03.16 // 2014.02.13
					//
					if ( mode > 0 ) { // 2018.06.22
						PROCESS_MONITOR_Setting_with_Dummy( ch + 1 , PROCESS_INDICATOR_PRE + mode , CHECKING_SIDE , -1 , -1 , -1 , -1 , ( recipe == NULL ) ? "Manual" : recipe , 0 , FALSE , 1 );
					}
					else {
//						PROCESS_MONITOR_Setting1( ch + 1 , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , "Manual" , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13 // 2018.06.22
						PROCESS_MONITOR_Setting1( ch + 1 , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , ( recipe == NULL ) ? "Manual" : recipe , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13 // 2018.06.22
					}
				}
			}
		}
	}
}
//


int Scheduler_LotPre_Check_Setting_Part( BOOL rand , int CHECKING_SIDE , int ch ) { // 2017.10.19
	int opt;
	//
	opt = rand ? 10 : 0;
	//
	if ( rand ) {
		if ( 12 != _i_SCH_PRM_GET_inside_READY_RECIPE_USE( CHECKING_SIDE , ch ) ) return 1;
	}
	//
	if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch ) == 100 /* 처음 시작시 이전 Lot이 사용중이었음 설정 되었을때 */ ) {
		//
		if ( !STRCMP_L( _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST( ch ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( CHECKING_SIDE , ch ) ) ) {
			//
			if ( ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( ch ) % 100 ) != ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( CHECKING_SIDE , ch ) % 100 ) ) {
				//
				_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = ( _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) * 100 ) + 3 + opt;
				//
			}
			else {
				//
				_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = ( _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) * 100 ) + 2 + opt;
				//
			}
			//
			_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 예약 */
			//
		}
		else {
			//
			if ( ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( ch ) % 100 ) != ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( CHECKING_SIDE , ch ) % 100 ) ) {
				//
				_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = ( _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) * 100 ) + 1 + opt;
				//
				_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 예약 */
				//
			}
			else {
				//
				_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = 0;
				//
				if ( rand ) { // 2017.10.25 에 확인 필요(OK)
					//
					_i_SCH_PREPRCS_FirstRunPre_Clear_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 취소 */
					//
				}
				//
			}
			//
		}
		//
	}
	else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch ) > 100 /* 처음 시작시 이전 Lot이 사용중이었음 설정 되지 않았을때 */ ) {
		//
		if ( !rand ) {
			//
			_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 예약 */
			//
		}
		//
	}
	//
	return 0;
	//
}
//

//BOOL Scheduler_LotPre_Check_Start_Part( int CHECKING_SIDE , int ch , int firstNormalmode , BOOL *maintag , BOOL *mintag , BOOL *plustag , BOOL *chinctag , int logdata ) { // 2015.05.27
BOOL Scheduler_LotPre_Check_Start_Part( int CHECKING_SIDE , int ch , BOOL otherlotcheck , int firstNormalmode , BOOL *maintag , BOOL *mintag , BOOL *plustag , BOOL *chinctag , int logdata ) { // 2015.05.27
	int mi , Res , mode;
	BOOL prerun;
	int i , dirrun; // 2015.05.27
	//
	//
	// otherlotcheck
	//
	//	FALSE	EIL
	//	TRUE	NORMAL
	//
	// firstNormalmode
	//
	//	0	AFTER_BEGIN_and_WAIT
	//	1	AFTER_BEGIN
	//	2	EIL1
	//	3	EIL2
	//
	//
	prerun = FALSE;
	//
	*maintag  = FALSE;
	*mintag   = FALSE;
	*plustag  = FALSE;
	*chinctag = FALSE;
	//
	mode = _i_SCH_PRM_GET_inside_READY_RECIPE_USE( CHECKING_SIDE,ch );
	//
	if ( firstNormalmode >= 2 /* EIL1,EIL2 */ ) { // 2011.05.12
		//
//		if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( CHECKING_SIDE,ch ) != 0 ) { // 2011.05.15 // 2017.09.07
//		if ( ( mode != 0 ) && ( mode != 12 ) ) { // 2011.05.15 // 2017.09.07 // 2017.09.15
		if ( ( mode != 0 ) && ( mode != 12 ) && ( mode != 13 ) ) { // 2011.05.15 // 2017.09.07 // 2017.09.15
			//
			for ( mi = 0 ; mi < MAX_PM_SLOT_DEPTH ; mi++ ) {
				//
				if ( _i_SCH_MODULE_Get_PM_WAFER( ch , mi ) > 0 ) {
					//
					mode = 9;
					//
					break;
					//
				}
				//
			}
			//
		}
		//
	}
	//
	switch( mode ) { // 2005.02.17
	case 1 : // 2007.03.16
	case 2 : // 2007.03.16
	case 3 : // 2007.03.16
	case 4 : // 2007.03.16
	case 5 : // 2007.03.16
	case 6 : // 2007.03.16
	case 7 : // 2007.03.16
	case 8 : // 2007.03.16
	//
	case 10 : // 2007.04.02
	case 11 : // 2007.04.02
	//
	case 12 : // 2017.09.06
	case 13 : // 2017.09.15
	//
		if ( firstNormalmode == 3 /* EIL2 */ ) { // 2005.02.21
			//
			_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 예약 */
			//
		}
		else if ( ( firstNormalmode != 0 /* AFTER_BEGIN 일때 */ ) && ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() / 3 ) / 2 ) == 1 /* LotThread */ ) ) { // 2005.02.21 // AFTER_BEGIN 이고 LotThread 일때
			//
//			_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 1 ); // 2015.05.31
			//
			switch( mode ) { // 2015.05.31
			case 5 :	// LotAlways(Force)
			case 6 :
			case 7 :
			case 8 :
				//
//				_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
				_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 101 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
				//
				break;
			default : // SameLotSkip
				//
				if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch ) != 2 /* 처음 시작시 이전 Lot이 사용중이었음 설정 되지 않았을때 */ ) {
					//
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 102 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
					//
				}
				//
				break;
			}
			//
		}
		else { // AFTER_BEGIN_and_WAIT 이거나 LotThread 가 아닌경우
			//=========================================================================================================
			mi = ch;
			if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
				if ( ( ( ch - PM1 ) % 2 ) != 0 ) mi = ch - 1;
			}
			//=========================================================================================================
			// 2015.05.27
			//
//			if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , mi ) != SYS_RUNNING ) { // 2002.08.27 // 2010.12.17
//			if ( ( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() >= 4 ) || ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , mi ) != SYS_RUNNING ) ) { // 2002.08.27 // 2010.12.17 // 2015.05.27
			//
			//
			dirrun = 0;
			//
			//
//			if ( mode != 12 ) { // 2017.09.07 // 2017.09.15
			if ( ( mode != 12 ) && ( mode != 13 ) ) { // 2017.09.07 // 2017.09.15
				//
				if ( otherlotcheck /* NORMAL(Not EIL) */ ) {
					//
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) > 1 ) {
						//
	//					dirrun = 2; // 2015.06.02
						//
						// 2015.06.02
						//
						//
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							//
							if ( i != CHECKING_SIDE ) {
								//
								if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
									//
									if ( _i_SCH_SYSTEM_RUNORDER_GET( i ) < _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) ) {
										//
										if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) {
											//
											dirrun = 2;
											//
											break;
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
						//
					}
				}
				//
				if ( dirrun == 0 ) {
					//
					if ( ( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() >= 4 ) || ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , mi ) != SYS_RUNNING ) ) {
						dirrun = 1;
					}
					//
				}
				//
			}
			//
			if ( dirrun == 1 ) { // 2015.05.27
				//
				//=============================================================================================
				// 2007.04.02
				//=============================================================================================
				switch( mode ) {
				case 10 : // 2007.04.02
				case 11 : // 2007.04.02
					return FALSE;
					break;
				}
				//=============================================================================================

				//=============================================================================================================
//				if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return FALSE;

				prerun = TRUE; // 2008.03.07
				//
				Res = _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
								 mi ,
//								 0 , 0 , 0 , // 2010.03.10
								 CHECKING_SIDE + 1 , 0 , 0 , // 2010.03.10
								 _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,ch) ,
								 11 ,
								 0 , "" , _i_SCH_PRM_GET_inside_READY_RECIPE_MINTIME(CHECKING_SIDE,ch) ,
//								 1 , 101 ) == 1 ) { // 2010.07.08
								 1 , logdata + ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( CHECKING_SIDE,ch ) * 10000 ) ); // 2010.07.08 // 2010.12.17
				//
				if (
					( ( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() >= 4 ) && ( Res >= 1 /* Success + Success(Append) */ ) ) ||
					( ( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() <  4 ) && ( Res == 1 /* Success */ ) )
					) { // 2010.12.17
					//
					switch( mode ) { // 2005.02.17
					case 3 : // Wait Finish
					case 4 :
					case 7 :
					case 8 :
						*maintag = TRUE;
						break;
					}
					//----------------------------------------------------------------------------
					//=========================================================================================================
					// 2005.12.12
					//=========================================================================================================
					if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
						if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
							_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch+1 );
//							PROCESS_MONITOR_Setting1( ch+1 , PROCESS_INDICATOR_PRE , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,ch) , 1 , FALSE ); // 2007.03.16 // 2014.02.13
							PROCESS_MONITOR_Setting1( ch+1 , 0 , 0 , PROCESS_INDICATOR_PRE , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,ch) , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13
//										PROCESS_MONITOR_Set_Recipe( CHECKING_SIDE , i+1 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,i) );
//										PROCESS_MONITOR_Set_Last( CHECKING_SIDE , i+1 , 1 );
//										PROCESS_MONITOR_Set_Pointer( CHECKING_SIDE , i+1 , -1 , -1 , -1 );
//										PROCESS_MONITOR_Set_Status( CHECKING_SIDE , i+1 , PROCESS_INDICATOR_PRE );
							//----------------------------------------------------------------------------
							switch( mode ) {
							case 3 : // Wait Finish
							case 4 :
							case 7 :
							case 8 :
								*plustag = TRUE;
								break;
							}
							*chinctag = TRUE;
						}
						else {
							_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch-1 );
//							PROCESS_MONITOR_Setting1( ch-1 , PROCESS_INDICATOR_PRE , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,ch) , 1 , FALSE ); // 2007.03.16 // 2014.02.13
							PROCESS_MONITOR_Setting1( ch-1 , 0 , 0 , PROCESS_INDICATOR_PRE , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,ch) , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13
//										PROCESS_MONITOR_Set_Recipe( CHECKING_SIDE , i-1 , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME(CHECKING_SIDE,i) );
//										PROCESS_MONITOR_Set_Last( CHECKING_SIDE , i-1 , 1 );
//										PROCESS_MONITOR_Set_Pointer( CHECKING_SIDE , i-1 , -1 , -1 , -1 );
//										PROCESS_MONITOR_Set_Status( CHECKING_SIDE , i-1 , PROCESS_INDICATOR_PRE );
							//----------------------------------------------------------------------------
							switch( mode ) {
							case 3 : // Wait Finish
							case 4 :
							case 7 :
							case 8 :
								*mintag = TRUE;
								break;
							}
						}
					}
					//=========================================================================================================
				}
			}
			else {
				//====================================================================
				// 2007.03.16
				//====================================================================
				switch( mode ) { // 2005.02.17
				case 1 : // (If PROCESS Runnig then run as READY9)
				case 3 :
				case 5 :
				case 7 :
				case 10 : // 2007.04.02
				case 11 : // 2007.04.02
//				case 12 : // 2017.09.20 // 2017.10.16

				case 13 :
					//
					if ( firstNormalmode != 0 /* AFTER_BEGIN 일때 */ ) { // 2007.04.13
						//
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2007.04.13 // 2017.09.22
						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 103 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2007.04.13 // 2017.09.22
						//
					}
					else {
						//
						_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 예약 */
						//
					}
					//
					break;
					//

				case 12 : // 2017.09.20 // 2017.10.16
					//
					if ( firstNormalmode != 0 /* AFTER_BEGIN 일때 */ ) { // 2007.04.13
						//
						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 104 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2007.04.13 // 2017.09.22
						//
					}
					else {
						//
						/*
						//
						// 2017.10.19
						//
						if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch ) == 100 / * 처음 시작시 이전 Lot이 사용중이었음 설정 되었을때 * / ) {
							//
							if ( !STRCMP_L( _i_SCH_PRM_GET_inside_READY_RECIPE_NAME_LAST( ch ) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( CHECKING_SIDE , ch ) ) ) {
								//
								if ( ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( ch ) % 100 ) != ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( CHECKING_SIDE , ch ) % 100 ) ) {
									//
									_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = ( _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) * 10 ) + 3;
									//
								}
								else {
									//
									_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = ( _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) * 10 ) + 2;
									//
								}
								//
								_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); / * Chamber 앞에서 Pre 예약 * /
								//
							}
							else {
								//
								if ( ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE_LAST( ch ) % 100 ) != ( _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( CHECKING_SIDE , ch ) % 100 ) ) {
									//
									_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = ( _i_SCH_PRM_GET_inside_READY_RECIPE_INDX_LAST( ch ) * 10 ) + 1;
									//
									_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); / * Chamber 앞에서 Pre 예약 * /
									//
								}
								else {
									//
									_in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][ch] = 0;
									//
									_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch , 41 );
									//
								}
								//
							}
							//
						}
						else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , ch ) > 100 / * 처음 시작시 이전 Lot이 사용중이었음 설정 되지 않았을때 * / ) {
							//
							_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); / * Chamber 앞에서 Pre 예약 * /
							//
						}
						//
						*/
						//
						Scheduler_LotPre_Check_Setting_Part( FALSE , CHECKING_SIDE , ch ); // 2017.10.19
						//
					}
					//
					break;
					//

				}
				//====================================================================
			}
		}
		break;
		//
	case 9 :
		//
		if ( ( firstNormalmode != 0 /* AFTER_BEGIN_and_WAIT */ ) || ( firstNormalmode == 3 ) ) {
			//
			_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , ch ); /* Chamber 앞에서 Pre 예약 */
			//
		}
		//
		break;
	}
	//
	return prerun; // 2008.03.07
}
//
BOOL Scheduler_Has_Multi_Path( int CHECKING_SIDE ) { // 2014.09.17
	int i;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) <= 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) > 1 ) return TRUE;
		//
		return FALSE;
		//
	}
	//
	return FALSE;
	//
}
//
BOOL Scheduler_LotPre_Finished_Check_Part( int CHECKING_SIDE , int mode , char *errorstring ) { // 2008.03.10
	int i , RunningFindCount , AdaptProgressiveOK;
	//
	while( TRUE ) {
		//========================================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return TRUE; // 2010.10.28
		//========================================================================================================
		RunningFindCount = 0;
		AdaptProgressiveOK = FALSE;
		//========================================================================================================
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( xinside_global_PrcsRunCheck[CHECKING_SIDE][i] == 1 ) { // Wait
				switch( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) ) {
				case SYS_RUNNING :
					RunningFindCount++;
					break;
				case SYS_SUCCESS :
					xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 3;
					break;
				//
				case SYS_ERROR : // 2012.04.20
					//
					xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 3;
					//
					switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) ) {
					case -1 : // All Stop
						//
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
						//
						return FALSE;
						break;
					case -2 : // Cluster - Continue , Process - Continue : Post OK , Next Out
						break;
					case -3 : // Cluster - Continue , Process - Disable : Post Skip
						break;
					case -4 : // Cluster - Out , Process - Continue : Post OK , Next Out
						break;
					case -5 : // Cluster - Out , Process - Disable : Post Skip , Next Out
						break;
					}
					break;

				default :
					//
					strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2008.04.14
					//
					return FALSE;
					break;
				}
			}
			else if ( xinside_global_PrcsRunCheck[CHECKING_SIDE][i] == 2 ) { // Spawn
				switch( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) ) {
				case SYS_RUNNING :
					break;
				case SYS_SUCCESS :
					xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 3;
					break;
				//
				case SYS_ERROR : // 2012.04.20
					//
					xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 3;
					//
					switch ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) ) {
					case -1 : // All Stop
						//
						strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) );
						//
						return FALSE;
						break;
					case -2 : // Cluster - Continue , Process - Continue : Post OK , Next Out
						break;
					case -3 : // Cluster - Continue , Process - Disable : Post Skip
						break;
					case -4 : // Cluster - Out , Process - Continue : Post OK , Next Out
						break;
					case -5 : // Cluster - Out , Process - Disable : Post Skip , Next Out
						break;
					}
					break;

				default :
					//
					strcpy( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2008.04.14
					//
					return FALSE;
					break;
				}
			}
			else { // 2008.03.10
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2008.03.10
					//
					switch( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) ) { // 2008.03.13
					case SYS_RUNNING :
						//
//						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) < PROCESS_INDICATOR_POST ) { // 2011.07.22 // 2011.08.30
						if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) >= PROCESS_INDICATOR_POST ) { // 2011.07.22 // 2011.08.30
							AdaptProgressiveOK = TRUE;
						}
						//
						break;
					default :
						AdaptProgressiveOK = TRUE;
						break;
					}
				}
			}
		}
		//========================================================================================================
		if ( RunningFindCount != 0 ) {
			Sleep(1);
			continue;
		}
		//========================================================================================================
		if ( Scheduler_Has_Multi_Path( CHECKING_SIDE ) ) { // 2014.09.17
			//
			switch( _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() ) {
			case 1 :
			case 3 :
			case 5 : // 2010.12.17
			case 7 : // 2010.12.17
				if ( _i_SCH_SYSTEM_USING_ANOTHER_RUNNING( CHECKING_SIDE ) ) {
					if ( !AdaptProgressiveOK ) {
						Sleep(1);
						continue;
					}
				}
				break;
			default :
				break;
			}
			//
		}
		//========================================================================================================
		break;
		//========================================================================================================
	}
	return TRUE;
}

/*
// 2008.03.10
BOOL Scheduler_LotPre_Finished_Check_Part( int CHECKING_SIDE ) {
	int i , j , k;
	while( TRUE ) {
		j = 0;
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( xinside_global_PrcsRunCheck[CHECKING_SIDE][i] ) {
				k = EQUIP_STATUS_PROCESS( CHECKING_SIDE , i );
				if ( k == SYS_RUNNING ) {
					j++;
				}
				else {
					if ( k == SYS_SUCCESS ) xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = FALSE;
					else                    return FALSE;
				}
			}
		}
		if ( j == 0 ) break;
		Sleep(10);
	}
	return TRUE;
}
*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Begin Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Scheduler_Ready_Possible_Check( int side , int ch ) { // 2012.11.12
	//
//	if ( _i_SCH_SYSTEM_EQUIP_RUNNING_GET( ch ) ) return FALSE;
	//
	if ( !_i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) return TRUE;
	if ( EQUIP_STATUS_PROCESS( side , ch ) != SYS_RUNNING ) return TRUE;
	//
	return FALSE;
}

void Scheduler_ProcessTime_Set( int ch ) { // 2016.12.14
	int t1 , t2;
	//
	if ( _i_SCH_PRM_GET_SIM_TIME1( ch ) <= 0 ) {
		t1 = _i_SCH_PRM_GET_Process_Run_In_Time( ch );
	}
	else {
		t1 = _i_SCH_PRM_GET_SIM_TIME1( ch ) / 1000;
	}
	//
	if ( _i_SCH_PRM_GET_SIM_TIME2( ch ) <= 0 ) {
		t2 = _i_SCH_PRM_GET_Process_Run_Out_Time( ch );
	}
	else {
		t2 = _i_SCH_PRM_GET_SIM_TIME2( ch ) / 1000;
	}
	//
	KPLT_PROCESS_TIME_INIT( ch , t1 , t2 );
	//
}

Module_Status Scheduler_Begin_Part( int CHECKING_SIDE , char *errorstring ) {
	int i , j , k , l , f;
	BOOL maintag , mintag , plustag , inctag;
//	int Run[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2002.08.27 // 2005.08.18
//	int Run2[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2003.06.20 // 2005.08.18

	int xinside0_BeforeLotUsed[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2005.02.17

//	char RunRecipe[65]; // 2007.01.04
	char RunRecipe[513]; // 2007.01.04
//	char strbuffer[256]; // 2012.02.01
	//
	char strbuffer[(MAX_CHAMBER)*4]; // 2012.02.01
	//
	int startmdl; // 2011.05.30
/*
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
	int pt , sdres;
	int xinside0_TFM_Tag_for_A4[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2005.02.17
//###################################################################################################
#endif
//###################################################################################################
*/
	//=============================================================================================================
	// 2008.10.10
	//=============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) xinside0_BeforeLotUsed[CHECKING_SIDE][i] = 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) <= 0 ) continue;
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					l = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
					//
					if      ( l > 0 ) {
						xinside0_BeforeLotUsed[CHECKING_SIDE][l] = 1;
					}
					else if ( l < 0 ) {
						if ( xinside0_BeforeLotUsed[CHECKING_SIDE][-l] == 0 ) xinside0_BeforeLotUsed[CHECKING_SIDE][-l] = 2;
					}
					//
				}
			}
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( ( i >= PM1 ) && ( i < AL ) ) {
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) { // 2009.10.14
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_99_USE_to_DISABLE ) ) { // 2009.10.14
					//
					if ( xinside0_BeforeLotUsed[CHECKING_SIDE][i] == 0 ) {
						//
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Select Rejected at %s%cPROCESS_REJECT PM%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i - PM1 + 1 );
						//
					}
				}
			}
			//
			xinside0_BeforeLotUsed[CHECKING_SIDE][i] = 0;
		}
		//
	}

	//=============================================================================================================
	// 2007.06.08
	//=============================================================================================================
	xinside_global_BM_Return[CHECKING_SIDE] = FALSE; // 2007.06.08
	//=============================================================================================================

	//=============================================================================================================
	// 2007.01.26
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2008.04.03
	//=============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
		//===================================================================================
		// 2005.11.10
		//===================================================================================
		//_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part Start%cBEGINSTART\n" , 9 );
		//===================================================================================
		Scheduler_Make_Mdl_String( CHECKING_SIDE , strbuffer );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part Start%cBEGINSTART %s\n" , 9 , strbuffer );
	}
	//=============================================================================================
	// 2007.03.29
	//=============================================================================================
	if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 3 ) {
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , CHECKING_SIDE ) ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i + BM1 ) == MUF_00_NOTUSE ) {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i + BM1 , MUF_01_USE );
				}
			}
		}
	}
	//=============================================================================================
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2007.05.16
	//================================================================================
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.05.21
	//================================================================================
	//==========================================================================================================================
//	SYSTEM_BEGIN_SET( CHECKING_SIDE , 1 ); // 2010.04.23 // 2015.07.30
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 2 ); // 2010.04.23 // 2015.07.30
	//==========================================================================================================================
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.30
		startmdl = CM1;
	}
	else {
		startmdl = PM1;
	}
	//------------------------------------------------------------------------------------
	for ( i = startmdl ; i <= FM ; i++ ) {  // 2011.05.21
//	for ( i = CM1 ; i <= FM ; i++ ) {  // 2011.05.21

		//==============================================================================================
		xinside_global_Run_Tag[CHECKING_SIDE][i] = 0; // 2002.08.27
		//==============================================================================================
		xinside0_BeforeLotUsed[CHECKING_SIDE][i] = FALSE; // 2005.02.17
		//==============================================================================================
		if ( ( i != AL ) && ( i != IC ) ) {
			//---------------------
			// 2002.03.25
			//---------------------
			if ( ( i >= PM1 ) && ( i < AL ) ) PROCESS_MONITOR_LOG_READY( CHECKING_SIDE , i , TRUE );
			//---------------------
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
				//---------------------------------------------------------------------------------
//				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( i , ABORTWAIT_FLAG_NONE ); // 2003.06.13 // 2007.05.09
				//---------------------------------------------------------------------------------
				if ( i < PM1 ) {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) {
						Scheduler_ProcessTime_Set( i );
						xinside_global_Run_Tag[CHECKING_SIDE][i] = 1; // 2002.08.27
						_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , FALSE );
					}
				}
				else {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
						_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
						Scheduler_ProcessTime_Set( i );
						xinside_global_Run_Tag[CHECKING_SIDE][i] = 1; // 2002.08.27

						if ( ( i >= BM1 ) && ( i < TM ) ) {
							//
							strcpy( RunRecipe , "" );
							//
							_SCH_COMMON_BM_2MODULE_END_APPEND_STRING( CHECKING_SIDE , i , RunRecipe );
							//
							_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , RunRecipe , FALSE ); // 2003.05.29
							//
						}
						else {
							if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) { // 2005.09.07
								/*
								// 2012.11.12
								if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) { // 2006.03.28
									if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) == SYS_RUNNING ) {
										xinside_global_Run_Tag[CHECKING_SIDE][i] = 3;
									}
									else {
										_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 ); // 2005.09.07
									}
								}
								else {
									_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 ); // 2005.09.07
								}
								*/
								//
								// 2012.11.12
								if ( Scheduler_Ready_Possible_Check( CHECKING_SIDE , i ) ) {
									_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 );
								}
								else {
									xinside_global_Run_Tag[CHECKING_SIDE][i] = 3;
								}
								//
							}
							else {
								/*
								// 2012.11.12
//								if ( ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2003.10.24 // 2010.05.27
								if ( ( _i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2003.10.24 // 2010.05.27
									if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) { // 2006.03.28
										if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) == SYS_RUNNING ) {
											xinside_global_Run_Tag[CHECKING_SIDE][i] = 3;
										}
										else {
//											_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ); // 2003.10.08 // 2010.05.27
											_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ); // 2003.10.08 // 2010.05.27
										}
									}
									else {
//										_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ); // 2003.10.08 // 2010.05.27
										_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ); // 2003.10.08 // 2010.05.27
									}
								}
								else { // 2003.10.24
									xinside_global_Run_Tag[CHECKING_SIDE][i] = 0; // 2003.10.24
								}
								*/
								//
								// 2012.11.12
								if ( ( _i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2003.10.24 // 2010.05.27
									if ( Scheduler_Ready_Possible_Check( CHECKING_SIDE , i ) ) {
										_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) );
									}
									else {
										xinside_global_Run_Tag[CHECKING_SIDE][i] = 3;
									}
								}
								else {
									xinside_global_Run_Tag[CHECKING_SIDE][i] = 0;
								}
								//
							}
						}
					}
					else {
						//================================================================================================================
						// 2005.09.10
						//================================================================================================================
						k = 1;
						f = 2; // 2011.04.01
						//================================================================================================================
						if ( ( i >= PM1 ) && ( i < AL ) ) {
							if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) {
								k = _i_SCH_MODULE_Get_Mdl_Run_Flag( i );
								for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
									if ( j != CHECKING_SIDE ) {
										if ( ( _i_SCH_SYSTEM_USING_GET( j ) > 0 ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) >= MUF_90_USE_to_XDEC_FROM ) ) k--;
									}
								}
							}
						}
						//================================================================================================================
						if ( k != 0 ) { // 2011.03.31
							//================================================================================================================
							k = 1;
							f = 0; // 2011.04.01
							//================================================================================================================
							if ( ( i >= PM1 ) && ( i < AL ) ) {
								if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , FALSE ) ) {
									k = _i_SCH_MODULE_Get_Mdl_Run_Flag( i );
									for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
										if ( j != CHECKING_SIDE ) {
											if ( ( _i_SCH_SYSTEM_USING_GET( j ) > 0 ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_99_USE_to_DISABLE ) ) k--;
										}
									}
								}
							}
							//================================================================================================================
						}
						//================================================================================================================
						//================================================================================================================
						if ( k == 0 ) {
							//================================================================================================================
							// 2005.09.10
							//================================================================================================================
							_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
							Scheduler_ProcessTime_Set( i );
							xinside_global_Run_Tag[CHECKING_SIDE][i] = 1;
							//
							if ( ( i >= PM1 ) && ( i < AL ) ) { // 2006.03.28
								/*
								// 2012.11.12
								if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) { // 2006.03.28
									if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) == SYS_RUNNING ) {
										xinside_global_Run_Tag[CHECKING_SIDE][i] = 3;
									}
									else {
//										_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 ); // 2011.04.01
										_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , f ); // 2011.04.01
									}
								}
								else {
//									_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 ); // 2011.04.01
									_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , f ); // 2011.04.01
								}
								*/
								//
								// 2012.11.12
								if ( Scheduler_Ready_Possible_Check( CHECKING_SIDE , i ) ) {
									_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , f );
								}
								else {
									xinside_global_Run_Tag[CHECKING_SIDE][i] = 3;
								}
								//
							}
							else {
//								_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 ); // 2011.04.01
								_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , f ); // 2011.04.01
							}
							//================================================================================================================
						}
						else {
							//if ( i < AL ) PROCESS_MONITOR_LOG_READY( CHECKING_SIDE , i , FALSE ); // 2002.03.25
							_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
							//==============================================================================================
							xinside0_BeforeLotUsed[CHECKING_SIDE][i] = TRUE; // 2005.02.17
							//==============================================================================================
						}
					}
					//
//					if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) { // 2010.04.10 // 2010.05.27
					if ( !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) { // 2010.04.10 // 2010.05.27
						_SCH_CLUSTER_Parallel_Check_Curr_DisEna( CHECKING_SIDE , -1 , i , TRUE ); // 2010.04.10
					}
					//
				}
			}
		}
	}
	//
	// 2011.05.19
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) <= MUF_00_NOTUSE ) continue;
		if ( xinside0_BeforeLotUsed[CHECKING_SIDE][i] ) continue;
		//
		for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
			//
			if ( k == CHECKING_SIDE ) continue;
			//
			if ( _i_SCH_SYSTEM_USING_GET( k ) <= 0 ) continue;
			if ( _i_SCH_SYSTEM_MODE_GET( k ) < 2 ) continue;
			//
			//
			if ( _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) > _i_SCH_SYSTEM_RUNORDER_GET( k ) ) { // 2016.01.26
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( k , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) {
					//
					xinside0_BeforeLotUsed[CHECKING_SIDE][i] = TRUE;
					break;
					//
				}
				//
			}
			//
		}
	}
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.05.21
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 3 ); // 2017.07.03
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2007.12.31
	//================================================================================
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.30
		startmdl = CM1;
	}
	else {
		startmdl = PM1;
	}
	//------------------------------------------------------------------------------------
	do {
		//------------------------------
		Sleep(1); // Append 2002.03.23
		//------------------------------
		k = 0;
		f = 0;
		//
		strcpy( errorstring , "" ); // 2008.04.14
		//
//		for ( i = CM1 ; i <= FM ; i++ ) { // 2011.05.21
		for ( i = startmdl ; i <= FM ; i++ ) { // 2011.05.21
			if ( ( i != AL ) && ( i != IC ) ) {
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) { // 2002.07.11
//				if ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 1 ) { // 2002.07.11 // 2006.03.28
				if ( ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 1 ) || ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 3 ) ) { // 2002.07.11 // 2006.03.28
					if ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 1 ) { // 2006.03.28
						l = EQUIP_BEGIN_END_STATUS( i );
						if ( ( l == SYS_ABORTED ) || ( l == SYS_ERROR ) ) {
							//
							if ( errorstring[0] != 0 ) strcat( errorstring , "|" ); // 2008.04.14
							strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2008.04.14
							//
							f++;
						}
						if ( l == SYS_RUNNING ) k++;
					}
					else { // 2006.03.28
						l = SYS_SUCCESS;
					}
					if ( l == SYS_SUCCESS ) { // 2002.08.27
						xinside_global_Run_Tag[CHECKING_SIDE][i] = 2; // 2002.08.27
						//----------------------------------------------------------------------
						// Pre Check for Process Running // 2002.08.27
						//----------------------------------------------------------------------
						if ( ( i >= PM1 ) && ( i < AL ) ) {
							//-----------------------------------------------------------------------------
//							Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i ); // 2007.03.16 // 2018.06.22
							Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i , NULL , 0 ); // 2007.03.16 // 2018.06.22
							//-----------------------------------------------------------------------------
						}
					} // 2002.08.27
				}
			}
		}
	}
	while ( k != 0 );
	//================================================================================
	if ( f != 0 ) {
//		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part Fail%cBEGINFAIL\n" , 9 ); // 2010.10.28
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part Fail(%s)%cBEGINFAIL\n" , errorstring , 9 ); // 2010.10.28
		return SYS_ABORTED;
	}
	//=================================================================================
	strcpy( errorstring , "" ); // 2008.04.14
	//=================================================================================
	// 2006.07.12
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
		//===================================================================================
		Scheduler_Make_Mdl_String( CHECKING_SIDE , strbuffer );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part Success%cBEGINSUCCESS %s\n" , 9 , strbuffer );
	}
	//
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 4 ); // 2017.07.03
	//================================================================================
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
		return SYS_SUCCESS;
	}
	//===============================================================================================
	// 2005.10.27
	//===============================================================================================
	EnterCriticalSection( &CR_MAIN );
	//
	if ( !USER_RECIPE_ACTION_AFTER_READY( CHECKING_SIDE ) ) { // 2005.10.27
		LeaveCriticalSection( &CR_MAIN );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part Fail2%cBEGINFAIL\n" , 9 );
		return SYS_ABORTED;
	}
	//===============================================================================================
	Scheduler_Regist_Lot_PrePost_Recipe( CHECKING_SIDE ); // 2005.07.18
	//===============================================================================================
	LeaveCriticalSection( &CR_MAIN );
	//=============================================================================================================
	// 2006.07.12
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//-----------------------------
		// Append 2003.01.20 // 2003.06.20
		//-----------------------------
		xinside_global_Run2_Tag[CHECKING_SIDE][i] = xinside_global_Run_Tag[CHECKING_SIDE][i];
		//-----------------------------
		xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 0; // 2005.02.17
		//-----------------------------
/*
// 2008.10.15
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) {
			for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
				if ( j != CHECKING_SIDE ) {
					if ( _i_SCH_SYSTEM_USING_GET( j ) > 0 ) {
						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_04_USE_to_PREND_EF_LP ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_05_USE_to_PRENDNA_EF_LP ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_71_USE_to_PREND_EF_XLP ) ) { // 2006.01.17 // 2007.04.12
							xinside_global_Run2_Tag[CHECKING_SIDE][i] = 0; // 2003.06.20
						}
					}
				}
			}
		}
*/
		//--------------------------------------------------------------------------
		if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) { // 2003.10.25
		//--------------------------------------------------------------------------
			if ( xinside0_BeforeLotUsed[CHECKING_SIDE][i] ) {
				//
				_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 2 /* 처음 시작시 이전 Lot이 사용중이었음 설정 */ );
				//
				xinside_global_Run2_Tag[CHECKING_SIDE][i] = 0; // 2016.01.26
				//
			}
			else { // first
				//================================================================================
				// 2007.04.02
				//================================================================================
				switch( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( CHECKING_SIDE,i ) ) {
				case 10 :
					_i_SCH_PRM_SET_inside_READY_RECIPE_USE( CHECKING_SIDE , i , 1 );
					break;
				case 11 :
					_i_SCH_PRM_SET_inside_READY_RECIPE_USE( CHECKING_SIDE , i , 3 );
					break;
				}
				//================================================================================
			}
			//=====================================================================================================
			if ( ( xinside_global_Run2_Tag[CHECKING_SIDE][i] != 0 ) && ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) != 0 ) ) { // 2003.06.20 // 2005.02.17
				//============================================================================================================
//				if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , 1 , &maintag , &mintag , &plustag , &inctag , 101 ) ) { // 2015.05.27
				if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , TRUE , 1 , &maintag , &mintag , &plustag , &inctag , 101 ) ) { // 2015.05.27
					if ( maintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 1;
					else			xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 2;
					//
					if ( mintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i-1] = 1;
					if ( plustag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i+1] = 1;
					//
					if ( inctag )	i++;
				}
				//============================================================================================================
			}
		}
		else {
			//=====================================================================================================
			// 2007.08.17 // 2009.09.24
			//=====================================================================================================
//			if ( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() != 0 ) {
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) {
//					if ( !_i_SCH_MODULE_GET_USE_ENABLE( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) , CHECKING_SIDE ) ) {
//						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_99_USE_to_DISABLE );
//					}
//				}
//			}
			//=====================================================================================================
		}
	}
	//=============================================================================================================
	// 2005.02.17
	//=============================================================================================================
	strcpy( errorstring , "" );
	if ( !Scheduler_LotPre_Finished_Check_Part( CHECKING_SIDE , 0 , errorstring ) ) return SYS_ABORTED;
	//-------------------------------------------
	//
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 5 ); // 2017.07.03
	//================================================================================
	//
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return SYS_SUCCESS; // 2010.10.28
	//-------------------------------------------
	strcpy( errorstring , "" );
	//=============================================================================================================
	//=============================================================================================================
	// 2006.07.12
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	//=============================================================================================================
	strcpy( errorstring , "" ); // 2008.04.14
	if ( !_SCH_COMMON_PRE_PART_OPERATION( CHECKING_SIDE , 0 , xinside_global_Run_Tag[CHECKING_SIDE] , xinside_global_Run2_Tag[CHECKING_SIDE] , xinside_global_PrcsRunCheck[CHECKING_SIDE] , errorstring ) ) return SYS_ABORTED;
	strcpy( errorstring , "" ); // 2008.04.14
	//=============================================================================================================
	//
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 6 ); // 2017.07.03
	//================================================================================
	//
	//=============================================================================
	// 2005.11.11
	//=============================================================================
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Scheduler_Begin_Before_Wait_Part( int CHECKING_SIDE , char *errorstring ) { // 2005.08.19
	int i , j , k , l , m , n;
	//=============================================================================================================
	// 2006.07.12
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return SYS_SUCCESS; // 2010.10.28
	//-------------------------------------------
	strcpy( errorstring , "" ); // 2008.04.14
	if ( !_SCH_COMMON_PRE_PART_OPERATION( CHECKING_SIDE , 1 , xinside_global_Run_Tag[CHECKING_SIDE] , xinside_global_Run2_Tag[CHECKING_SIDE] , xinside_global_PrcsRunCheck[CHECKING_SIDE] , errorstring ) ) return SYS_ABORTED;
	strcpy( errorstring , "" ); // 2008.04.14
	//=============================================================================================================
	//=================================================================
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( xinside_global_Run_Tag[CHECKING_SIDE][i] != 0 ) {
			_SCH_COMMON_RUN_PRE_BM_CONDITION( CHECKING_SIDE , i , _i_SCH_PRM_GET_MODULE_BUFFER_MODE( _i_SCH_PRM_GET_MODULE_GROUP( i ) , i ) );
		}
	}
	//=================================================================
	// 2005.08.19
	//=================================================================
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) == 2 /* 처음 시작시 이전 Lot이 사용중이었음 설정 되었을때 */) {
			//
			if ( _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) >= 2 ) { // Sequential
				//
				if      ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) == 12 ) { // 2017.09.15
					//
//					if ( _in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][i] != 0 ) { // 2017.09.22 // 2017.10.16
						//
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 121 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.10.16
						//
//					} // 2017.10.16
//					else { // 2017.10.16
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 12 /* 사용안함 설정 */ ); // 2017.09.22 // 2017.10.16
//					} // 2017.10.16
					//
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 100 /* AFTER_BEGIN_and_WAIT 에 CHECK 하도록 예약 */ ); // 2017.10.16
					//
				}
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) == 13 ) { // 2017.09.15
					//
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 122 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ );
					//
				}
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) >= 5 /* LotAlways(Force) */ ) {
					//
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 123 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
					//
				}
				else {
					//
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 0 /* 사용안함 설정 */ ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 13 /* 사용안함 설정 */ ); // 2017.09.22
					//
				}
				//======================================
				_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
				//======================================

				//=========================================================================
				// 2006.05.18
				//=========================================================================
//				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) { // 2009.10.14
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_99_USE_to_DISABLE ) ) { // 2009.10.14
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_31_USE_to_NOTUSE_SEQMODE );
				}
				//=========================================================================
				// 2005.11.10
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
					_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 3%cMDLSTSERROR 3:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
				}
				//=========================================================================
			}
			else {
				//================================================================================
				// 2007.04.02
				//================================================================================
				if      ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) == 12 ) { // 2017.09.15
					//
//					if ( _in_LOTPRE_CLUSTER_DIFFER_MODE2[CHECKING_SIDE][i] != 0 ) { // 2017.09.22 // 2017.10.16
						//
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 124 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.10.16
						//
//					}
//					else {
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 14 /* 사용안함 설정 */ ); // 2017.09.22
//					}
					//
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 100 /* AFTER_BEGIN_and_WAIT 에 CHECK 하도록 예약 */ ); // 2017.10.16
					//
				}
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) == 13 ) { // 2017.09.15
					//
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 125 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ );
					//
				}
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) >= 10 /* SameLotSkip But If PROCESS Runnig LotAlways(Force) */ ) {
					//
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 122 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
					//
				}
				//================================================================================
				else if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE(CHECKING_SIDE,i) >= 5 /* LotAlways(Force) */ ) {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) { // 2005.12.20
						//
//						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
						_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 123 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ); // 2017.09.22
						//
					}
					else {
						_i_SCH_PREPRCS_FirstRunPre_Doing_PathProcessData( CHECKING_SIDE , i ); /* Chamber 앞에서 Pre 예약 */
					}
				}
				else {
					//
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 0 /* 사용안함 설정 */ ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 15 /* 사용안함 설정 */ ); // 2017.09.22
					//
				}
			}
		}
		//=========================================================================================================
		// 2006.12.01
		//=========================================================================================================
//		if ( !_SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 0 ) { // 2008.07.19
			//===================================================================================
			// 2005.11.24
			//===================================================================================
//			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) == 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ) { // 2017.09.22
			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) >= 100 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ) { // 2017.09.22
				n = FALSE;
				for ( j = 0 ; ( j < MAX_CASSETTE_SLOT_SIZE ) && (!n) ; j++ ) {
					if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) >= 0 ) {
						for ( k = 0 ; ( k < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , j ) ) && (!n) ; k++ ) {
							for ( l = 0 ; ( l < MAX_PM_CHAMBER_DEPTH ) && (!n) ; l++ ) {
								m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , j , k , l );
								if ( m == i ) {
									n = TRUE;
									break;
								}
							}
						}
					}
				}
				if ( !n ) {
					//
//					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 0 /* 사용안함 설정 */ ); // 2017.09.22
					_i_SCH_PRM_SET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i , 30 /* 사용안함 설정 */ ); // 2017.09.22
					//
					_i_SCH_PREPRCS_FirstRunPre_Clear_PathProcessData( CHECKING_SIDE , i ); // 2005.12.20
					//
//					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) { // 2009.10.14
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_99_USE_to_DISABLE ) ) { // 2009.10.14
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
						//======================================
						_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
						//======================================
						//=========================================================================
						if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
							_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 4%cMDLSTSERROR 4:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i );
						}
						//=========================================================================
					}
					else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) { // 2006.06.11
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
					}
				}
			}
			//===================================================================================
		}
	}
	//=================================================================
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Scheduler_Begin_After_Wait_Part( int CHECKING_SIDE , char *errorstring , BOOL seqrerun ) {
	int i , j , k , f , l;
	BOOL maintag , mintag , plustag , inctag;
	int xinside2_SeqRun[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2005.03.15
	int xinside2_SeqRun2[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2005.03.15
	//
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return SYS_SUCCESS; // 2010.10.28
	//-------------------------------------------

	if ( _i_SCH_SYSTEM_MODE_GET( CHECKING_SIDE ) >= 2 ) { // 2005.03.15
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			xinside2_SeqRun[CHECKING_SIDE][i] = 0;
			//
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_97_USE_to_SEQDIS_RAND ) { // 2011.06.23
				if ( !seqrerun ) _i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
			}
			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
				//=========================================================================
				if ( seqrerun ) {
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_31_USE_to_NOTUSE_SEQMODE ) continue;
				}
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
					Scheduler_ProcessTime_Set( i ); // 2006.02.09
					if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) { // 2005.09.07
						_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 ); // 2005.09.07
						xinside2_SeqRun[CHECKING_SIDE][i] = 1; // 2005.09.07
					}
					else {
						if ( ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) {
							_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) );
							xinside2_SeqRun[CHECKING_SIDE][i] = 1;
						}
					}
				}
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) {
//					if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) { // 2009.10.14 // 2016.05.09
//					if ( _i_SCH_MODULE_GET_USE_ENABLE( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) > 0 ) , CHECKING_SIDE ) ) { // 2009.10.14 // 2016.05.09 // 2018.12.20
					if ( _i_SCH_MODULE_GET_USE_ENABLE( i , ( _i_SCH_MODULE_Get_PM_WAFER( i , -1 ) > 0 ) , CHECKING_SIDE + 1000 ) ) { // 2009.10.14 // 2016.05.09 // 2018.12.20
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
					}
					else { // 2009.10.14
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_99_USE_to_DISABLE );
					}
					_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
				}
				//=========================================================================
			}
		}
		do {
			//------------------------------
			Sleep(1);
			//------------------------------
			k = 0;
			f = 0;
			//
			strcpy( errorstring , "" ); // 2008.04.14
			//
			for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
				if ( xinside2_SeqRun[CHECKING_SIDE][i] == 1 ) {
					l = EQUIP_BEGIN_END_STATUS( i );
					if ( ( l == SYS_ABORTED ) || ( l == SYS_ERROR ) ) {
						//
						if ( errorstring[0] != 0 ) strcat( errorstring , "|" ); // 2008.04.14
						strcat( errorstring , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) ); // 2008.04.14
						//
						f++;
					}
					if ( l == SYS_RUNNING ) k++;
					if ( l == SYS_SUCCESS ) {
						xinside2_SeqRun[CHECKING_SIDE][i] = 2;
						//----------------------------------------------------------------------
						// Pre Check for Process Running
						//----------------------------------------------------------------------
//						Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i ); // 2007.03.16 // 2018.06.22
						Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i , NULL , 0 ); // 2007.03.16 // 2018.06.22
						//-----------------------------------------------------------------------------
					}
				}
			}
		}
		while ( k != 0 );
		if ( f != 0 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Part 2 Fail%cBEGINFAIL\n" , 9 );
			return SYS_ABORTED;
		}
	}
	else { // 2005.08.16
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			xinside2_SeqRun[CHECKING_SIDE][i] = 0;
		}
	}
	//=============================================================================================================
	strcpy( errorstring , "" );
	//=============================================================================================================
	//
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	//
	//=============================================================================================================
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_97_USE_to_SEQDIS_RAND ) continue; // 2011.06.23
		//
		xinside2_SeqRun2[CHECKING_SIDE][i] = xinside2_SeqRun[CHECKING_SIDE][i];
		//-----------------------------
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) {
			for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
				if ( j != CHECKING_SIDE ) {
					if ( _i_SCH_SYSTEM_USING_GET( j ) > 0 ) {
						if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_02_USE_to_END ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_03_USE_to_END_DISABLE ) ) { // 2008.09.22
							xinside2_SeqRun2[CHECKING_SIDE][i] = 0;
						}
					}
				}
			}
		}
		//-----------------------------
		if ( xinside_global_PrcsRunCheck[CHECKING_SIDE][i] == 0 ) { // 2008.03.07
			//--------------------------------------------------------------------------
			if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) {
			//--------------------------------------------------------------------------
//				if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) == 1 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ) { // 2017.09.22
				if ( _i_SCH_PRM_GET_inside_READY_RECIPE_STEP2( CHECKING_SIDE , i ) >= 100 /* AFTER_BEGIN_and_WAIT 에 하도록 예약 */ ) { // 2017.09.22
					//============================================================================================================
//					if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , 0 , &maintag , &mintag , &plustag , &inctag , 102 ) ) { // 2015.05.27
					if ( Scheduler_LotPre_Check_Start_Part( CHECKING_SIDE , i , TRUE , 0 , &maintag , &mintag , &plustag , &inctag , 102 ) ) { // 2015.05.27
						//
						if ( maintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 1;
						else			xinside_global_PrcsRunCheck[CHECKING_SIDE][i]	= 2;
						if ( mintag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i-1]	= 1;
						if ( plustag )	xinside_global_PrcsRunCheck[CHECKING_SIDE][i+1]	= 1;
						//
						if ( inctag )	i++;
						//
					}
					//============================================================================================================
				}
			}
		}
		//
	}
	//
	//=============================================================================================================
	strcpy( errorstring , "" );
	if ( !Scheduler_LotPre_Finished_Check_Part( CHECKING_SIDE , 1 , errorstring ) ) return SYS_ABORTED;
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return SYS_SUCCESS; // 2010.10.28
	//-------------------------------------------
	strcpy( errorstring , "" );
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	strcpy( errorstring , "" ); // 2008.04.14
	if ( !_SCH_COMMON_PRE_PART_OPERATION( CHECKING_SIDE , 2 , xinside_global_Run_Tag[CHECKING_SIDE] , xinside2_SeqRun[CHECKING_SIDE] , xinside2_SeqRun2[CHECKING_SIDE] , errorstring ) ) return SYS_ABORTED;
	strcpy( errorstring , "" ); // 2008.04.14
	return SYS_SUCCESS;
}

//----------------------------------------------------------------------------
void Scheduler_Randomize_After_Waiting_Part( int CHECKING_SIDE , int pralskip ) { // 2006.02.28
	int pmd[MAX_PM_CHAMBER_DEPTH];
	int i , j , k , m;
//	int c; // 2018.09.28

//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRANDWAIT=> [Side=%d][Parl=%d]\n" , CHECKING_SIDE , pralskip );
//------------------------------------------------------------------------------------------------------------------

	if ( pralskip == 0 ) return;

//	c = 0; // 2006.06.22 // 2018.09.28

	switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) {
	case 10 :
/*
// 2007.04.17
		for ( i = PM1 ; i < AL ; i++ ) {
			pmd[i - PM1] = FALSE;
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) < MUF_90_USE_to_XDEC_FROM ) ) {
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) > 1 ) {
					if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
						if ( _i_SCH_MODULE_Get_Mdl_Spd_Flag( CHECKING_SIDE , i ) == 0 ) { // 2006.11.24
							c++;
						}
					}
					pmd[i - PM1] = TRUE;
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_98_USE_to_DISABLE_RAND );
				}
			}
		}
		break;
*/
	case 11 :
		for ( i = PM1 ; i < AL ; i++ ) {
			pmd[i - PM1] = FALSE;
			if ( ( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() == 10 ) || ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) {
//				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) < MUF_90_USE_to_XDEC_FROM ) ) { // 2009.10.14
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) <= MUF_99_USE_to_DISABLE ) ) { // 2009.10.14
					//
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_81_USE_to_PREND_RANDONLY ) ) continue; // 2009.03.25 // 2009.10.14
//					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_81_USE_to_PREND_RANDONLY ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_99_USE_to_DISABLE ) ) continue; // 2009.03.25 // 2009.10.14 // 2011.06.24
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_31_USE_to_NOTUSE_SEQMODE ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_81_USE_to_PREND_RANDONLY ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_99_USE_to_DISABLE ) ) continue; // 2009.03.25 // 2009.10.14 // 2011.06.24
					//
					//=========================================================================================
					// 2007.05.04
					//=========================================================================================
//					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) > 1 ) { // 2007.05.04
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRANDWAIT => 1 [PM%d]\n" , i - PM1 + 1 );
//------------------------------------------------------------------------------------------------------------------
//					m = Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( CHECKING_SIDE , i , ( pralskip == 2 ) ); // 2011.03.28
					m = Get_RunPrm_UTIL_START_PARALLEL_USE_COUNT( CHECKING_SIDE , i , ( pralskip == 2 ) || ( pralskip == 4 ) , ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) ); // 2011.03.28
					//
					if ( m >= 1 ) { // 2007.05.04
					//=========================================================================================
//------------------------------------------------------------------------------------------------------------------
//_IO_CONSOLE_PRINTF( "PARLSKIP\tRANDWAIT => 2 [PM%d][m=%d]\n" , i - PM1 + 1 , m );
//------------------------------------------------------------------------------------------------------------------
						/*
						//
						// 2018.09.28
						//
						if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
							if ( _i_SCH_MODULE_Get_Mdl_Spd_Flag( CHECKING_SIDE , i ) == 0 ) { // 2006.11.24
								//
								if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) { // 2009.08.05
									c++;
								}
								//
							}
						}
						*/
						//
						pmd[i - PM1] = TRUE;
						//
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_31_USE_to_NOTUSE_SEQMODE ) { // 2011.06.24
							_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_98_USE_to_DISABLE_RAND );
						}
						else {
							_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_97_USE_to_SEQDIS_RAND );
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
	//========================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) >= 0 ) {
			//
//			if ( pralskip == 2 ) { // 2009.02.12 // 2011.03.28
			if ( ( pralskip == 2 ) || ( pralskip == 4 ) ) { // 2009.02.12 // 2011.03.28
				if ( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) > SCHEDULER_SUPPLY ) continue;
			}
			else {
				if ( _i_SCH_CLUSTER_Get_PathStatus( CHECKING_SIDE , i ) != SCHEDULER_READY ) continue;
			}
			//
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
					if ( m > 0 ) {
						if ( pmd[m-PM1] ) {
							_i_SCH_CLUSTER_Set_PathProcessChamber_Disable( CHECKING_SIDE , i , j , k );
						}
					}
				}
			}
			//
		}
	}
	//
	/*
	//
	// 2018.09.28
	//
	//========================================================================
	// 2006.06.22
	//========================================================================
	if ( ( pralskip == 3 ) || ( pralskip == 4 ) ) { // 2011.03.28
		_SCH_CASSETTE_Set_Count_Action( CHECKING_SIDE );
	}
	else {
		if ( c > 0 ) { // 2006.06.26
			i = _i_SCH_CASSETTE_Get_Side_Monitor_Cluster_Count( CHECKING_SIDE ) - c;
	//_IO_CIM_PRINTF( "=======================================\n" );
	//_IO_CIM_PRINTF( "=[A]====================[%d]===========\n" , i );
	//_IO_CIM_PRINTF( "=======================================\n" );
			//
			SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( CHECKING_SIDE , i );
		}
	}
	//========================================================================
	*/
	//
	// 2018.09.28
	//
	_SCH_CASSETTE_Set_Count_Action( CHECKING_SIDE , TRUE ); // 2018.10.01
	//
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling End Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
//int _i_SCH_PREPOST_Randomize_End_Part( int side , int Chamber ) { // 2006.02.28 // 2007.04.17
int _i_SCH_PREPOST_Randomize_End_Part2( int side , int Chamber , BOOL All ) { // 2006.02.28 // 2007.04.17
	int s , i , j , k , m , flg;
	//int c; // 2007.04.17
	int ch_buffer[ MAX_PM_CHAMBER_DEPTH ]; // 2006.11.22
	char Buffer[64];

	//==========================================================================================
	// 2007.04.17
	//==========================================================================================
//	if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) > 1 ) return 1; // 2007.04.17
	//==========================================================================================
	//
	s = -1;
	//
//printf( "[END][%d] [PM%d] START[%d]\n" , side , Chamber - PM1 + 1 , All );

	for ( m = 0 ; m < MAX_CASSETTE_SIDE ; m++ ) {
		if ( m == side ) continue;
		//
/*
//
2011.06.24
		flg = _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber );
		//
		if ( _i_SCH_SYSTEM_USING_RUNNING( m ) ) {
			//
printf( "[END][%d] [PM%d] FIND 1 [M=%d][F=%d]\n" , side , Chamber - PM1 + 1 , m , flg );
			//
			if ( flg != MUF_31_USE_to_NOTUSE_SEQMODE ) {
				if ( flg != MUF_97_USE_to_SEQDIS_RAND ) {
					if ( flg != MUF_98_USE_to_DISABLE_RAND ) {
						continue;
					}
				}
			}
		}
		else { // 2011.06.24
			//
printf( "[END][%d] [PM%d] FIND 2 [M=%d][F=%d]\n" , side , Chamber - PM1 + 1 , m , flg );

			if ( flg != MUF_31_USE_to_NOTUSE_SEQMODE ) {
				continue;
			}
			else {
				if ( !_i_SCH_SYSTEM_USING_STARTING( m ) ) continue;
			}
		}
*/
		if ( _i_SCH_SYSTEM_USING_GET( m ) <= 0 ) continue; // 2011.06.24
		//
		//====
		// 2011.10.07
		//
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_00_NOTUSE ) continue;
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_02_USE_to_END ) continue;
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_03_USE_to_END_DISABLE ) continue;
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_04_USE_to_PREND_EF_LP ) continue;
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_05_USE_to_PRENDNA_EF_LP ) continue;
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_07_USE_to_PREND_DECTAG ) continue;
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_08_USE_to_PRENDNA_DECTAG ) continue;
		//
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_71_USE_to_PREND_EF_XLP   ) continue; // 2015.10.15
		if (  _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_81_USE_to_PREND_RANDONLY ) continue; // 2015.10.15
		//
		//====
		//
		if ( s == -1 ) {
			s = m;

//printf( "[END][%d] [PM%d] SEL 1 [S=%d]\n" , side , Chamber - PM1 + 1 , s );
		
		}
		else {
			if ( _i_SCH_SYSTEM_RUNORDER_GET( s ) > _i_SCH_SYSTEM_RUNORDER_GET( m ) ) {
				s = m;
//printf( "[END][%d] [PM%d] SEL 2 [S=%d]\n" , side , Chamber - PM1 + 1 , s );
			}
		}
	}
	//==========================================================================================
	// 2007.08.20
	//==========================================================================================
	if ( s != -1 ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) return TRUE;
		//
		flg = _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber );
		//
//printf( "[END][%d] [PM%d] RUN 1 [S=%d][F=%d]\n" , side , Chamber - PM1 + 1 , s , flg );
		//
//		if ( flg == MUF_31_USE_to_NOTUSE_SEQMODE ) return TRUE; // 2011.06.24
		//
		if ( !All ) {
			if ( flg != MUF_97_USE_to_SEQDIS_RAND ) { // 2018.11.01
				if ( flg != MUF_98_USE_to_DISABLE_RAND ) return TRUE; // 2011.06.24
			}
		}
		else {
			if ( flg != MUF_97_USE_to_SEQDIS_RAND ) {
				if ( flg != MUF_98_USE_to_DISABLE_RAND ) {
					return TRUE; // 2011.06.24
				}
			}
		}
		//
		for ( m = 0 ; m < MAX_CASSETTE_SIDE ; m++ ) {
			if ( m == side ) continue;
			if ( m == s ) continue; // 2009.10.28
			if ( !_i_SCH_SYSTEM_USING_RUNNING( m ) ) continue;
//			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_01_USE ) { // 2009.10.28
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( m , Chamber ) == MUF_99_USE_to_DISABLE ) ) { // 2009.10.28
				if ( _i_SCH_SYSTEM_RUNORDER_GET( s ) > _i_SCH_SYSTEM_RUNORDER_GET( m ) ) {
					return TRUE;
				}
			}
		}
	}
	//==========================================================================================
/*
// 2007.04.17
	switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) {
	case 10 :
		break;
	case 11 :
		if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber ) == MRES_SUCCESS_LIKE_ALLSKIP ) return 2;
		break;
	default :
		return 3;
		break;
	}
*/
	//==========================================================================================
//	c = 0; // 2007.04.17
//	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) { // 2007.04.17
//		if ( s == side ) continue; // 2007.04.17
//		if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue; // 2007.04.17
//		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) != MUF_98_USE_to_DISABLE_RAND ) continue; // 2007.04.17
		//
	if ( s != -1 ) { // 2007.04.23
//printf( "[END][%d] [PM%d] RUN 2 [S=%d][F=%d]\n" , side , Chamber - PM1 + 1 , s , flg );
		//-----------------------------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) ch_buffer[ i ] = FALSE; // 2006.11.22
		//-----------------------------------------------------------------------------------------------------------
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _i_SCH_CLUSTER_Get_PathRange( s , i ) <= 0 ) continue;
			if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) >= SCHEDULER_BM_END ) {
//				continue; // 2008.09.19
				if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) != SCHEDULER_RETURN_REQUEST ) continue; // 2008.09.19
			}
			//
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( s , i ) ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , i , j , k );
					//
					if ( m == -Chamber ) {
						//
						//------------------------------------
						switch( _i_SCH_SUB_Get_ENABLE_TO_ENABLEN() ) { // 2015.04.14
						case 1 :
						case 2 :
							break;
						case 3 :
							if ( _i_SCH_CLUSTER_Get_PathStatus( s , i ) == SCHEDULER_READY ) {
								if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , s ) ) { // 2009.10.28
									_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , i , j , k );
								}
							}
							break;
						default :
							if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , s ) ) { // 2009.10.28
								_i_SCH_CLUSTER_Set_PathProcessChamber_Enable( s , i , j , k );
							}
							break;
						}
						//------------------------------------
//						c++; // 2007.04.17
						//==========================================================
						// 2006.11.22
						//==========================================================
						m = -m; // 2006.11.22
						//==========================================================
					}
					//==========================================================
					// 2006.11.22
					//==========================================================
					if ( m > 0 ) {
						if ( _i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , s ) ) {
							if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( m ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) {
								ch_buffer[ m - PM1 ] = TRUE;
							}
						}
					}
					//==========================================================
				}
			}
		}
		//=============================================================================
		// 2006.11.22
		//=============================================================================
		j = 0;
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( ch_buffer[ i ] ) j++;
		}
		if ( j <= 0 ) j = 1;
//_IO_CIM_PRINTF( "=======================================\n" );
//_IO_CIM_PRINTF( "=[B]====================[%d]===========\n" , j );
//_IO_CIM_PRINTF( "=======================================\n" );
		SCHEDULER_CONTROL_Set_Side_Monitor_Cluster_Count( s , j ); // 2009.04.02
		//====================================================================================================================================
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_97_USE_to_SEQDIS_RAND ) { // 2011.06.24
//printf( "[END][%d] [PM%d] RUN 3 [S=%d][F=%d]\n" , side , Chamber - PM1 + 1 , s , _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) );
			_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_31_USE_to_NOTUSE_SEQMODE );
			_i_SCH_MODULE_Dec_Mdl_Run_Flag( Chamber );
			if ( Scheduler_Begin_After_Wait_Part( s , Buffer , TRUE ) == SYS_ABORTED ) return FALSE;
		}
		else {
//printf( "[END][%d] [PM%d] RUN 4 [S=%d][F=%d]\n" , side , Chamber - PM1 + 1 , s , _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) );
//			if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , s ) ) { // 2009.10.28 // 2016.05.09
//			if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , -1 ) > 0 ) , s ) ) { // 2009.10.28 // 2016.05.09 // 2018.12.20
			if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , -1 ) > 0 ) , s + 1000 ) ) { // 2009.10.28 // 2016.05.09 // 2018.12.20
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) == MUF_98_USE_to_DISABLE_RAND ) { // 2017.10.19
					Scheduler_LotPre_Check_Setting_Part( TRUE , s , Chamber ); // 2017.10.19
				}
				//
				_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_01_USE );
			}
			else { // 2009.10.28
				_i_SCH_MODULE_Set_Mdl_Use_Flag( s , Chamber , MUF_99_USE_to_DISABLE );
			}
		}
//printf( "[END][%d] [PM%d] RUN END [S=%d][F=%d]\n" , side , Chamber - PM1 + 1 , s , _i_SCH_MODULE_Get_Mdl_Use_Flag( s , Chamber ) );
		//====================================================================================================================================
	} // 2007.04.23
//	} // 2007.04.17
/*
// 2009.04.02
	if ( ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber ) != MRES_SUCCESS_LIKE_SKIP ) && ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( Chamber ) != MRES_SUCCESS_LIKE_ALLSKIP ) ) { // 2006.06.26
		//========================================================================
		// 2006.06.22
		//========================================================================
		if ( _i_SCH_MODULE_Get_Mdl_Spd_Flag( side , Chamber ) == 0 ) { // 2006.11.24
//_IO_CIM_PRINTF( "=======================================\n" );
//_IO_CIM_PRINTF( "=[B2]===============================\n" );
//_IO_CIM_PRINTF( "=======================================\n" );
			//
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , Chamber ) == MUF_81_USE_to_PREND_RANDONLY ) ) { // 2009.03.25
				SCHEDULER_CONTROL_Dec_Side_Monitor_Cluster_Count( side );
			}
			//
		}
		//========================================================================
	}
*/
//	if ( c == 0 ) return 4; // 2007.04.17
//	return 0; // 2007.04.17
	return TRUE;
}

void _i_SCH_PREPOST_Randomize_End_Part( int side , int Chamber ) { // 2011.06.24
	_i_SCH_PREPOST_Randomize_End_Part2( side , Chamber , TRUE );
}

//============================================================================================================================
//
void Scheduler_LotPost_Check_Start_Part( int CHECKING_SIDE , int ch , BOOL premode , BOOL *chinctag ) {
	int mi;
	//
	*chinctag = FALSE;
	//
	//========================================================================================
	if ( premode ) {
		while( TRUE ) {
			mi = EQUIP_BEGIN_END_STATUS( ch );
			if ( mi == SYS_ABORTED ) return;
			if ( mi == SYS_ERROR   ) return;
			if ( mi == SYS_SUCCESS ) break;
			//------------------------------
			Sleep(1);
			//------------------------------
		}
	}
	//========================================================================================
	switch ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , ch ) ) {
	case 1 :
	case 2 :
	case 3 :
	case 4 : // 2019.02.13
		mi = ch;
		if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
			if ( ( ( ch - PM1 ) % 2 ) != 0 ) mi = ch - 1;
		}
		//=========================================================================================================
		if ( ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , ch ) == 1 ) || ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , mi ) != SYS_RUNNING ) ) {
			//
			if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch , 0 );
			//
			if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
							 mi ,
//							 0 , 0 , 0 , // 2010.03.10
							 CHECKING_SIDE + 1 , 0 , 0 , // 2010.03.10
							 _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) ,
							 12 ,
							 0 , "" , _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( CHECKING_SIDE , ch ) ,
//							 1 , 105 ) == 1 ) { // 2010.07.08
							 1 , 105 + ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE,ch ) * 10000 ) ) == 1 ) { // 2010.07.08
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
					if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
						_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch+1 );
//						PROCESS_MONITOR_Setting1( ch+1 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE ); // 2014.02.13
						PROCESS_MONITOR_Setting1( ch+1 , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE , FALSE ); // 2014.02.13
						*chinctag = TRUE;
						//
						if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch + 1 , 0 );
						//
					}
					else {
						_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch-1 );
//						PROCESS_MONITOR_Setting1( ch-1 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE ); // 2014.02.13
						PROCESS_MONITOR_Setting1( ch-1 , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE , FALSE ); // 2014.02.13
						//
						if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch - 1 , 0 );
						//
					}
				}
				//=========================================================================================================
			}
		}
		else {
			//=========================================================================================================
//			if ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , ch ) != 3 ) break; // 2019.02.13
			if ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , ch ) < 3 ) break; // 2019.02.13
			//=========================================================================================================
			mi = ch;
			if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
				if ( ( ( ch - PM1 ) % 2 ) != 0 ) mi = ch - 1;
			}
			//=========================================================================================================
			if ( !_i_SCH_EQ_SPAWN_EVENT_PROCESS( CHECKING_SIDE , -1 , mi , 0 , 0 , 0 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 12 , 0 , "" , _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( CHECKING_SIDE , ch ) , 0 ) ) {
				if ( _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( CHECKING_SIDE ,
						 mi ,
//						 0 , 0 , 0 , // 2010.03.10
						 CHECKING_SIDE + 1 , 0 , 0 , // 2010.03.10
						 _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) ,
						 12 ,
						 0 , "" , _i_SCH_PRM_GET_inside_END_RECIPE_MINTIME( CHECKING_SIDE , ch ) ,
//						 1 , 106 ) == 1 ) { // 2010.07.08
						 1 , 106 + ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE,ch ) * 10000 ) ) == 1 ) { // 2010.07.08
					//=========================================================================================================
					if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
						if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
							_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch+1 );
//							PROCESS_MONITOR_Setting1( ch+1 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE ); // 2007.03.16 // 2014.02.13
							PROCESS_MONITOR_Setting1( ch+1 , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13
							*chinctag = TRUE;
							//
							if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch + 1 , 0 );
							//
						}
						else {
							_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch-1 );
//							PROCESS_MONITOR_Setting1( ch-1 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE ); // 2007.03.16 // 2014.02.13
							PROCESS_MONITOR_Setting1( ch-1 , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , FALSE , FALSE ); // 2007.03.16 // 2014.02.13
							//
							if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch - 1 , 0 );
							//
						}
					}
					//=========================================================================================================
				}
				//--------------------------------------------------------------------------
			}
			else {
				//=========================================================================================================
				// 2007.06.11
				//=========================================================================================================
				if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Post Process Append at %s[%s]%cPROCESS_POST_APPEND PM%d::%s:9\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( mi ) , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 9 , mi - PM1 + 1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) );
				}
//				PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , TRUE ); // 2014.02.13
				PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_POST , CHECKING_SIDE , -1 , -1 , -1 , -1 , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( CHECKING_SIDE , ch ) , 1 , TRUE , FALSE ); // 2014.02.13
				//=========================================================================================================
				//=========================================================================================================
				if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch , 0 ); // 2007.04.19
				//=========================================================================================================
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
					if ( ( ( ch - PM1 ) % 2 ) == 0 ) {
						*chinctag = TRUE;
						//=========================================================================================================
						if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch + 1 , 0 ); // 2007.04.19
						//=========================================================================================================
					}
					else {
						//=========================================================================================================
						if ( premode ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( CHECKING_SIDE , ch - 1 , 0 ); // 2007.04.19
						//=========================================================================================================
					}
				}
				//=========================================================================================================
			}
		}
		break;
	}
}
//

void _i_SCH_PREPOST_LotPost_Part_for_All_BM_Return( int CHECKING_SIDE ) { // 2007.04.06
	int i , inctag;
	//=======================================================================================================================
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
//		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_04_USE_to_PREND_EF_LP ) { // 2007.04.12
		if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_04_USE_to_PREND_EF_LP ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_71_USE_to_PREND_EF_XLP ) ) { // 2007.04.12
			if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) {
				if ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , i ) != 0 ) {
					Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , i , TRUE , &inctag );
					if ( inctag ) i++;
				}
			}
		}
	}
	//=======================================================================================================================
	xinside_global_BM_Return[CHECKING_SIDE] = TRUE; // 2007.06.08
	//=======================================================================================================================
}


//============================================================================================================================
//----2001.05.31 for Pre End

/*
// 2010.09.20
BOOL _ins_SCH_PREPOST_Pre_End_Next_Random_Check() { // 2010.01.21
	//
	if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) return FALSE;
	//
	switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) {
	case 10 :
	case 11 :
		return TRUE;
		break;
	}
	//
	return FALSE;
}
*/
// 2010.09.20
BOOL _ins_SCH_PREPOST_Pre_End_Next_Random_Check( BOOL force ) { // 2010.01.21
	//
	if ( force ) return FALSE;
	//
	if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) return FALSE;
	//
//	switch( _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) { // 2011.01.06
//	case 10 :
//	case 11 :
//		return FALSE;
//		break;
//	}
	//
	return TRUE;
}

//BOOL _i_SCH_PREPOST_Pre_End_Sub_Part( int CHECKING_SIDE , int Chamber , int force ) { // 2018.11.27
BOOL _i_SCH_PREPOST_Pre_End_Sub_Part( int CHECKING_SIDE , int Chamber0 , int force ) { // 2018.11.27
	int i , inctag;
	int s1p;
	int Chamber; // 2018.11.27
	BOOL rand , randmize; // 2018.11.27
	//
	//================================================================================
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() != 0 ) return FALSE; // 2010.10.12
	//================================================================================
	//
	// 2018.11.27
	//
	if ( Chamber0 >= 1000 ) {
		Chamber = Chamber0 % 1000;
		rand = TRUE;
	}
	else {
		Chamber = Chamber0;
		rand = FALSE;
	}
	//
	if ( Chamber < BM1 ) { // 2008.04.25
		if ( ( CHECKING_SIDE >= 0 ) && ( CHECKING_SIDE < MAX_CASSETTE_SIDE ) ) { // 2009.07.28
			if ( _i_SCH_SYSTEM_MODE_LOOP_GET( CHECKING_SIDE ) == 0 ) return FALSE; // 2003.12.04
		}
	}
	//================================================================================
	if ( ( CHECKING_SIDE >= 0 ) && ( CHECKING_SIDE < MAX_CASSETTE_SIDE ) ) { // 2009.07.28
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) { // 2008.11.22
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) < 0 ) continue;
			if ( _i_SCH_CLUSTER_Get_PathRun( CHECKING_SIDE , i , 11 , 2 ) != 0 ) return FALSE;
		}
	}
	//
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2006.12.01
	//================================================================================
//	if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 2001 ) ) { // 2011.04.29
	if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 2000 + Chamber ) ) { // 2011.04.29
		LeaveCriticalSection( &CR_PRE_BEGIN_END );
		return FALSE;
	}
	//================================================================================
//	if ( Chamber >= BM1 ) { // 2008.04.21 // 2011.04.14
	if ( ( Chamber < PM1 ) || ( Chamber >= BM1 ) ) { // 2008.04.21 // 2011.04.14
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( ( CHECKING_SIDE != -1 ) && ( i != CHECKING_SIDE ) ) continue;
			if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , Chamber ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2003.10.25
				//=========================================================================
//				_i_SCH_MODULE_Set_Mdl_Use_Flag( i , Chamber , MUF_00_NOTUSE ); // 2009.08.22
				_i_SCH_MODULE_Set_Mdl_Use_Flag( i , Chamber , MUF_02_USE_to_END ); // 2009.08.22
				//=========================================================================
				_i_SCH_MODULE_Dec_Mdl_Run_Flag( Chamber );
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) < 0 ) {
					_i_SCH_MODULE_Set_Mdl_Run_Flag( Chamber , 0 );
//					_i_SCH_LOG_LOT_PRINTF( i , "Module %s Run Flag Status Error 55%cMDLSTSERROR 55:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber ); // 2011.06.10
					_i_SCH_LOG_LOT_PRINTF( i , "Module %s Run Flag Status Error 5%cMDLSTSERROR 5:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber ); // 2011.06.10
					continue;
				}
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) <= 0 ) {
					if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , CHECKING_SIDE ) ) {
						_i_SCH_EQ_BEGIN_END_RUN( Chamber , TRUE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , CHECKING_SIDE ) );
					}
					//_i_SCH_MODULE_Set_BM_FULL_MODE( Chamber , BUFFER_WAIT_STATION ); // 2009.04.08 // 2009.08.22 -> Move to Common_End
				}
				//=========================================================================
			}
		}
		//
	}
	else {
		//
//		if ( !_ins_SCH_PREPOST_Pre_End_Next_Random_Check() ) { // 2010.09.02
//			LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2010.09.02
//			return FALSE; // 2010.02.10 // 2010.09.02
//		} // 2010.09.02
		//
		if ( ( CHECKING_SIDE >= 0 ) && ( CHECKING_SIDE < MAX_CASSETTE_SIDE ) ) { // 2009.07.28
		//	if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) == MUF_01_USE ) { // 2003.10.25
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , Chamber ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2003.10.25
				if ( _i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , CHECKING_SIDE ) ) { // 2003.06.12 // 2003.10.25
					//
					// 2018.11.27
					//
					if ( rand ) {
						randmize = FALSE;
					}
					else {
						randmize = TRUE;
					}
					//
					//=========================================================================
					// 2005.11.11
					//=========================================================================
//					if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) { // 2008.07.29 // 2010.09.02
					if ( !_ins_SCH_PREPOST_Pre_End_Next_Random_Check( force != 0 ) ) { // 2010.01.21 // 2010.09.02
						if      ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) <= 0 ) {
//							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 4%cMDLSTSERROR 4:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber ); // 2011.06.10
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 6%cMDLSTSERROR 6:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber ); // 2011.06.10
							_i_SCH_MODULE_Set_Mdl_Run_Flag( Chamber , 0 );
			//				_SCH_EQ_BEGIN_END_RUN( Chamber , TRUE , "" , FALSE ); // 2006.01.17
						}
						else if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( Chamber ) == 1 ) {
							//
							if ( !rand ) { // 2018.11.27
								//
				//				_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_02_USE_to_???? ); // 2006.01.17
								//===============================================================================================
								if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 3 ) { // 2007.04.12
									if ( xinside_global_BM_Return[ CHECKING_SIDE ] ) {
										//===============================================================================================
										_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_04_USE_to_PREND_EF_LP ); // 2006.01.17
										//===============================================================================================
										_i_SCH_MODULE_Set_Mdl_Run_Flag( Chamber , 0 );
										//
										_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start%cPREENDSTART PM%d:1:4\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber - PM1 + 1 ); // 2006.11.23
										//
										_i_SCH_EQ_BEGIN_END_RUN( Chamber , TRUE , "" , FALSE );
										//===============================================================================================
										Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , Chamber , TRUE , &inctag );
									}
									else {
										//===============================================================================================
										_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_71_USE_to_PREND_EF_XLP ); // 2006.01.17
										//===============================================================================================
										_i_SCH_MODULE_Set_Mdl_Run_Flag( Chamber , 0 );
										//
										_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start%cPREENDSTART PM%d:2:11\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber - PM1 + 1 ); // 2006.11.23
										//
										_i_SCH_EQ_BEGIN_END_RUN( Chamber , TRUE , "" , FALSE );
										//===============================================================================================
									}
									//===============================================================================================
								}
								else {
									//===============================================================================================
									_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_04_USE_to_PREND_EF_LP ); // 2006.01.17
									//===============================================================================================
									_i_SCH_MODULE_Set_Mdl_Run_Flag( Chamber , 0 );
									//
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start%cPREENDSTART PM%d:3:4\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber - PM1 + 1 ); // 2006.11.23
									//
									_i_SCH_EQ_BEGIN_END_RUN( Chamber , TRUE , "" , FALSE );
									//=======================================================================================================================
									// 2007.04.02
									//=======================================================================================================================
									if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 2 ) {
										Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , Chamber , TRUE , &inctag );
									}
									//=======================================================================================================================
								}
								//
							}
							//
						}
						else {
							_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_07_USE_to_PREND_DECTAG ); // 2006.01.17
							_i_SCH_MODULE_Dec_Mdl_Run_Flag( Chamber );
							//
							randmize = TRUE; // 2018.11.27
							//
						}
					}
					else { // 2008.09.19
						//
						//
						if ( !rand ) { // 2018.11.27
							//
							_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Called%cPREENDCALL PM%d:81:0\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( Chamber ) , 9 , Chamber - PM1 + 1 ); // 2009.06.04
							//
							_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , Chamber , MUF_81_USE_to_PREND_RANDONLY ); // 2008.09.19
							//
						}
						//
					}
					//=============================================================================================
					if ( randmize ) { // 2018.11.27
						//
						if ( !_i_SCH_PREPOST_Randomize_End_Part2( CHECKING_SIDE , Chamber , FALSE ) ) { // 2006.02.28
							LeaveCriticalSection( &CR_PRE_BEGIN_END );
							return FALSE;
						}
						//
					}
					//=============================================================================================
				}
				//===================================================================================================
				if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 1 ) { // 2007.10.07
					//-------------------------------------------
					if ( Chamber >= BM1 ) {
						if ( ( ( Chamber - BM1 ) % 2 ) == 0 )
							s1p = Chamber + 1;
						else
							s1p = Chamber - 1;
					}
					else {
						if ( ( ( Chamber - PM1 ) % 2 ) == 0 )
							s1p = Chamber + 1;
						else
							s1p = Chamber - 1;
					}
					//-------------------------------------------
					if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , s1p ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , s1p ) >= MUF_90_USE_to_XDEC_FROM ) ) {
						if ( _i_SCH_MODULE_GET_USE_ENABLE( s1p , FALSE , CHECKING_SIDE ) ) {
							//
							// 2018.11.27
							//
							if ( rand ) {
								randmize = FALSE;
							}
							else {
								randmize = TRUE;
							}
							//
//							if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() >= 1 ) { // 2008.07.29 // 2010.09.02
							if ( !_ins_SCH_PREPOST_Pre_End_Next_Random_Check( force != 0 ) ) { // 2010.01.21 // 2010.09.02
								if      ( _i_SCH_MODULE_Get_Mdl_Run_Flag( s1p ) <= 0 ) {
//									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 4%cMDLSTSERROR 4:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( s1p ) , 9 , s1p ); // 2011.06.10
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 7%cMDLSTSERROR 7:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( s1p ) , 9 , s1p ); // 2011.06.10
									_i_SCH_MODULE_Set_Mdl_Run_Flag( s1p , 0 );
								}
								else if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( s1p ) == 1 ) {

									/*

									// 2008.09.22

									_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , s1p , MUF_04_USE_to_PREND_EF_LP );
									_i_SCH_MODULE_Set_Mdl_Run_Flag( s1p , 0 );
									//
									_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start 2%cPREENDSTART PM%d:2:4\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( s1p ) , 9 , s1p - PM1 + 1 ); // 2006.11.23
									//
									_i_SCH_EQ_BEGIN_END_RUN( s1p , TRUE , "" , FALSE );
									//=======================================================================================================================
									// 2007.04.02
									//=======================================================================================================================
									if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 2 ) {
										Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , s1p , TRUE , &inctag );
									}
									//=======================================================================================================================
									*/

									//
									//
									if ( !rand ) { // 2018.11.27
										//
										//----
										// 2008.09.22
										if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 3 ) {
											if ( xinside_global_BM_Return[ CHECKING_SIDE ] ) {
												//===============================================================================================
												_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , s1p , MUF_04_USE_to_PREND_EF_LP );
												//===============================================================================================
												_i_SCH_MODULE_Set_Mdl_Run_Flag( s1p , 0 );
												//
												_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start 2%cPREENDSTART PM%d:12:4\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( s1p ) , 9 , s1p - PM1 + 1 );
												//
												_i_SCH_EQ_BEGIN_END_RUN( s1p , TRUE , "" , FALSE );
												//===============================================================================================
												Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , s1p , TRUE , &inctag );
											}
											else {
												//===============================================================================================
												_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , s1p , MUF_71_USE_to_PREND_EF_XLP );
												//===============================================================================================
												_i_SCH_MODULE_Set_Mdl_Run_Flag( s1p , 0 );
												//
												_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start 2%cPREENDSTART PM%d:13:11\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( s1p ) , 9 , s1p - PM1 + 1 );
												//
												_i_SCH_EQ_BEGIN_END_RUN( s1p , TRUE , "" , FALSE );
												//===============================================================================================
											}
											//===============================================================================================
										}
										else {
											//===============================================================================================
											_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , s1p , MUF_04_USE_to_PREND_EF_LP );
											//===============================================================================================
											_i_SCH_MODULE_Set_Mdl_Run_Flag( s1p , 0 );
											//
											_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM %s Pre End Part Start 2%cPREENDSTART PM%d:14:4\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( s1p ) , 9 , s1p - PM1 + 1 );
											//
											_i_SCH_EQ_BEGIN_END_RUN( s1p , TRUE , "" , FALSE );
											//=======================================================================================================================
											// 2007.04.02
											//=======================================================================================================================
											if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 2 ) {
												Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , s1p , TRUE , &inctag );
											}
											//=======================================================================================================================
										}
										//----
									}
									//
								}
								else {
									_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , s1p , MUF_07_USE_to_PREND_DECTAG );
									_i_SCH_MODULE_Dec_Mdl_Run_Flag( s1p );
									//
									randmize = TRUE; // 2018.11.27
									//
								}
							}
							else { // 2008.09.19
								//
								//
								if ( !rand ) { // 2018.11.27
									//
									_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , s1p , MUF_81_USE_to_PREND_RANDONLY ); // 2008.09.19
									//
								}
								//
							}
							//=============================================================================================
							if ( randmize ) { // 2018.11.27
								//
								if ( !_i_SCH_PREPOST_Randomize_End_Part2( CHECKING_SIDE , s1p , FALSE ) ) {
									LeaveCriticalSection( &CR_PRE_BEGIN_END );
									return FALSE;
								}
								//
							}
							//=============================================================================================
						}
					}
				}
				//===============================================================================================
			}
		}
	}
	//================================================================================
//	_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 2999 ); // 2011.04.29
	_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 3000 + Chamber ); // 2011.04.29
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2006.12.01
	//================================================================================
	return TRUE;
}
//
//
BOOL _i_SCH_PREPOST_Pre_End_Part( int CHECKING_SIDE , int Chamber ) {
	return _i_SCH_PREPOST_Pre_End_Sub_Part( CHECKING_SIDE , Chamber , 0 );
}

BOOL _i_SCH_PREPOST_Pre_End_Force_Part( int CHECKING_SIDE , int Chamber ) {
	return _i_SCH_PREPOST_Pre_End_Sub_Part( CHECKING_SIDE , Chamber , 1 );
}
//
BOOL Scheduler_End_Part_Append( int side , int ch ) { // 2005.12.20
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_00_NOTUSE ) {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch ) <= 0 ) {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( i , ch , MUF_01_USE );
						_i_SCH_MODULE_Inc_Mdl_Run_Flag( ch );
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//
//
Module_Status Scheduler_End_Part_for_sub( int CHECKING_SIDE ) {
	int i , k , l , f , inctag;
	int Run[MAX_CASSETTE_SIDE][MAX_CHAMBER]; // 2002.08.27
//	char strbuffer[256]; // 2005.11.10 // 2012.02.01
	//
	char strbuffer[(MAX_CHAMBER)*4]; // 2012.02.01
	//
	int startmdl; // 2011.05.30

	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
		//=======================================================================================================================
		if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 0 ) ) return SYS_ABORTED;
		//=======================================================================================================================
		Scheduler_Make_Mdl_String( CHECKING_SIDE , strbuffer );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "End Part Start%cENDSTART %s\n" , 9 , strbuffer );
		//===================================================================================
	}
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_END ); // 2007.05.16
	//================================================================================
	if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 1001 ) ) {
		LeaveCriticalSection( &CR_PRE_BEGIN_END );
		return SYS_ABORTED;
	}
	//
	//------------------------------------------------------------------------------------
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.05.30
		startmdl = CM1;
	}
	else {
		startmdl = PM1;
	}
	//------------------------------------------------------------------------------------
	for ( i = startmdl ; i <= FM ; i++ ) { // 2011.05.21
//	for ( i = CM1 ; i <= FM ; i++ ) { // 2011.05.21
		//---------------------
		// 2002.03.25
		//---------------------
//		if ( ( i >= PM1 ) && ( i < AL ) ) { // 2009.07.15
//			f = _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ); // 2003.10.25 // 2009.07.15
//			PROCESS_MONITOR_LOG_END( CHECKING_SIDE , i , f ); // 2009.07.15
//		} // 2009.07.15
		//---------------------
		Run[CHECKING_SIDE][i] = 0; // 2002.08.27
		//
		if ( ( i != AL ) && ( i != IC ) ) {
			//----2001.05.31 for Pre End
//			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2003.10.25 // 2008.09.19
			if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_81_USE_to_PREND_RANDONLY ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) >= MUF_90_USE_to_XDEC_FROM ) ) { // 2003.10.25 // 2008.09.19
				//===============================================================================
				f = ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_81_USE_to_PREND_RANDONLY );
				//===============================================================================
				if ( ( i >= PM1 ) && ( i < AL ) ) {
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_02_USE_to_END ); // 2005.11.11
					}
					else {
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_03_USE_to_END_DISABLE ); // 2008.09.22
					}
				}
				else if ( ( i >= BM1 ) && ( i < TM ) ) { // 2009.08.22
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_02_USE_to_END );
				}
				else { // 2011.04.14
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_02_USE_to_END );
				}
				//===============================================================================
				_i_SCH_MODULE_Dec_Mdl_Run_Flag( i );
				//=========================================================================
				// 2005.11.10
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) < 0 ) {
					_i_SCH_MODULE_Set_Mdl_Run_Flag( i , 0 );
//					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 5%cMDLSTSERROR 5:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i ); // 2011.06.10
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Module %s Run Flag Status Error 8%cMDLSTSERROR 8:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i ); // 2011.06.10
				}
				//=========================================================================
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
					Run[CHECKING_SIDE][i] = 1; // 2002.08.27
					//if ( ( i >= PM1 ) && ( i < AL ) ) { // 2002.03.25
					//	PROCESS_MONITOR_LOG_END( CHECKING_SIDE , i , TRUE );
					//}
					if ( ( i >= BM1 ) && ( i < TM ) ) { // 2004.02.19
						if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) {
							switch ( _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() ) {
							case 1 :
							case 4 : // 2008.04.14
								if ( EQUIP_READ_FUNCTION( i ) != SYS_RUNNING ) {
									_i_SCH_EQ_BEGIN_END_RUN( i , TRUE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) );
								}
								else {
									if ( !Scheduler_End_Part_Append( CHECKING_SIDE , i ) ) { // 2005.12.20
										//
										_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "End Part Skip at Module %s with Running%cENDSKIP BM%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i - BM1 + 1 ); // 2005.12.20
										//
									}
									//
									Run[CHECKING_SIDE][i] = 0;
								}
								break;
							case 2 : // 2006.10.24
							case 5 : // 2008.04.14
								if ( EQUIP_READ_FUNCTION( i ) != SYS_RUNNING ) {
									_i_SCH_EQ_BEGIN_END_RUN( i , TRUE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) );
								}
								else {
									EQUIP_BM_END_RUN_SUB_CALL( i , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) );
								}
								break;
							default :
								_i_SCH_EQ_BEGIN_END_RUN( i , TRUE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) );
								break;
							}
						}
						else {
							Run[CHECKING_SIDE][i] = 0;
						}
						//=========================================================================================================================
						// 2008.04.14
						//=========================================================================================================================
//						switch ( _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() ) { // 2009.03.03
//						case 3 : // 2009.03.03
//						case 4 : // 2009.03.03
//						case 5 : // 2009.03.03
//							_i_SCH_MODULE_Set_BM_FULL_MODE( i , BUFFER_WAIT_STATION ); // 2009.08.22 -> Move to Common_End
//							break; // 2009.03.03
//						} // 2009.03.03
						//=========================================================================================================================
					}
					else {
						if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) { // 2005.09.07
							_i_SCH_EQ_BEGIN_END_RUN( i , TRUE , "" , 2 ); // 2005.09.07
						}
						else {
							if ( ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2003.10.25
								_i_SCH_EQ_BEGIN_END_RUN( i , TRUE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ); // 2003.10.25
							}
							else { // 2003.10.25
								Run[CHECKING_SIDE][i] = 0; // 2003.10.25
							}
						}
					}
//					if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE ) ) { // 2003.06.12 // 2003.10.25
//						_SCH_EQ_BEGIN_END_RUN( i , TRUE , "" , FALSE );
//					}
				}
				else {
					//if ( ( i >= PM1 ) && ( i < AL ) ) { // 2002.03.25
					//	PROCESS_MONITOR_LOG_END( CHECKING_SIDE , i , FALSE );
					//}
				}
				//=============================================================================================
				// _i_SCH_PREPOST_Randomize_End_Part( CHECKING_SIDE ,  i ); // 2006.02.28 // 2008.09.19
				if ( !f ) {
					if ( !_i_SCH_PREPOST_Randomize_End_Part2( CHECKING_SIDE ,  i , TRUE ) ) { // 2006.02.28 // 2008.09.19
						LeaveCriticalSection( &CR_PRE_BEGIN_END );
						return SYS_ABORTED;
					}
				}
				//=============================================================================================
			}
//			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_04_USE_to_PREND_EF_LP ) { // 2006.01.17
			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_04_USE_to_PREND_EF_LP ) {
				//if ( ( i >= PM1 ) && ( i < AL ) ) { // 2002.03.25
				//	PROCESS_MONITOR_LOG_END( CHECKING_SIDE , i , TRUE );
				//}
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) { // 2015.06.05
					Run[CHECKING_SIDE][i] = 1; // 2002.08.27
				}
			}
			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_71_USE_to_PREND_EF_XLP ) { // 2007.04.12
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) { // 2015.06.05
					Run[CHECKING_SIDE][i] = 1;
				}
			}
			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_05_USE_to_PRENDNA_EF_LP ) { // 2006.01.17
				if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) { // 2015.06.05
					Run[CHECKING_SIDE][i] = 1;
				}
			}
			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_07_USE_to_PREND_DECTAG ) { // 2006.01.17
			}
			else if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_08_USE_to_PRENDNA_DECTAG ) { // 2006.01.17
			}
			else {
			}
		}
	}
	//--------------------------------------------------------------------------
	if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 1998 ) ) {
		LeaveCriticalSection( &CR_PRE_BEGIN_END );
		return SYS_ABORTED;
	}
	//--------------------------------------------------------------------------
	do {
		//------------------------------
		Sleep(1); // Append 2002.03.23
		//------------------------------
		k = 0;
		f = 0;
		for ( i = CM1 ; i <= FM ; i++ ) {
			if ( ( i != AL ) && ( i != IC ) ) {
				if ( Run[CHECKING_SIDE][i] == 1 ) { // 2002.07.11 // 2002.08.27
					l = EQUIP_BEGIN_END_STATUS( i ); // 2002.07.11
					if ( l == SYS_ABORTED ) f++; // 2002.07.11
					if ( l == SYS_ERROR   ) f++; // 2006.08.30
					if ( l == SYS_RUNNING ) k++; // 2002.07.11
					if ( l == SYS_SUCCESS ) { // 2002.08.27
						 Run[CHECKING_SIDE][i] = 2; // 2002.08.27
					} // 2002.08.27
				} // 2002.07.11
			}
		}
	}
	while ( k != 0 );
	//================================================================================
	if ( f == 0 ) {
		if ( !_SCH_COMMON_CHECK_END_PART( CHECKING_SIDE , 1999 ) ) {
			LeaveCriticalSection( &CR_PRE_BEGIN_END );
			return SYS_ABORTED;
		}
	}
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2007.05.16
	//================================================================================
	if ( f != 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "End Part Fail%cENDFAIL\n" , 9 );
		return SYS_ABORTED;
	}
	//=======================================================================================================================
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//-----------------------------
		// Append 2003.01.20
		//-----------------------------
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) > 0 ) Run[CHECKING_SIDE][i] = 0;
		//-----------------------------
		//--------------------------------------------------------------------------
		if ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) { // 2003.10.25
			//=======================================================================================================================
			// 2007.02.09
			//=======================================================================================================================
			//--------------------------------------------------------------------------
//			if ( ( Run[CHECKING_SIDE][i] != 0 ) && ( _i_SCH_PRM_GET_END_RECIPE_USE(i) != 0 ) ) { // 2002.08.27 // 2005.03.24
//			if ( ( Run[CHECKING_SIDE][i] != 0 ) && ( inside_END_RECIPE_USE[CHECKING_SIDE][i] != 0 ) ) { // 2002.08.27 // 2005.03.24 // 2005.08.18
//			if ( ( Run[CHECKING_SIDE][i] != 0 ) && ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , i ) != 0 ) ) { // 2002.08.27 // 2005.03.24 // 2005.08.18 // 2019.02.13
			if ( ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , i ) == 4 ) || ( ( Run[CHECKING_SIDE][i] != 0 ) && ( _i_SCH_PRM_GET_inside_END_RECIPE_USE( CHECKING_SIDE , i ) != 0 ) ) ) { // 2002.08.27 // 2005.03.24 // 2005.08.18 // 2019.02.13
				//==============================================================================================================
				// 2007.04.02
				//==============================================================================================================
				Scheduler_LotPost_Check_Start_Part( CHECKING_SIDE , i , FALSE , &inctag );
				if ( inctag ) i++;
				//==============================================================================================================
			}
		}
	}
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) { // 2005.09.22
		//===================================================================================
		// 2005.11.10
		//===================================================================================
		//_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "End Part Success%cENDSUCCESS\n" , 9 );
		//===================================================================================
		Scheduler_Make_Mdl_String( CHECKING_SIDE , strbuffer );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "End Part Success%cENDSUCCESS %s\n" , 9 , strbuffer );
	}
	//=======================================================================================================================
	return SYS_SUCCESS;
}

//

Module_Status Scheduler_End_Part_Post_for_sub( int CHECKING_SIDE ) { // 2009.07.15
	int i , f;
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_END );
	//================================================================================
	for ( i = CM1 ; i <= FM ; i++ ) {
		if ( ( i >= PM1 ) && ( i < AL ) ) {
			f = _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE );
			PROCESS_MONITOR_LOG_END( CHECKING_SIDE , i , f );
		}
	}
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_END ); // 2007.05.16
	//================================================================================
	return SYS_SUCCESS;
}

//

void Scheduler_End_Part_Lock_Make( int mode ) { // 2009.04.13
//	DWORD i , j; // 2009.05.27
	//-------------------------------------------------------------------------
	EnterCriticalSection( &CR_LOTEND_DELAY[mode] );
	//-------------------------------------------------------------------------
	while( TRUE ) {
/*
// 2009.05.27
//		i = clock(); // 2003.10.09
		i = GetTickCount(); // 2003.10.09
		j = i - End_Set_Delay_Time_BUFFER[mode];
		if ( j < 0 ) {
			End_Set_Delay_Time_BUFFER[mode] = i;
		}
		else if ( j >= End_Set_Delay_Time ) {
			End_Set_Delay_Time_BUFFER[mode] = i;
			break;
		}
*/
// 2009.05.27
		if ( ( GetTickCount() - End_Set_Delay_Time_BUFFER[mode] ) >= End_Set_Delay_Time ) {
			End_Set_Delay_Time_BUFFER[mode] = GetTickCount();
			break;
		}
		Sleep(100);
	}
}

//------------------------------------------------------------------------------------------
void Scheduler_End_Part_Lock_Clear( int mode ) { // 2004.03.11
	End_Set_Delay_Time_BUFFER[mode] = GetTickCount(); // 2004.03.11
	LeaveCriticalSection( &CR_LOTEND_DELAY[mode] ); // 2004.03.11
}
//------------------------------------------------------------------------------------------

Module_Status Scheduler_End_Part( int CHECKING_SIDE ) {
	int Res;
	//-------------------------------------------------------------------------
//	Scheduler_End_Part_Lock_Make( 0 ); // 2009.04.13 // 2009.07.15
	//-------------------------------------------------------------------------
	Res = Scheduler_End_Part_for_sub( CHECKING_SIDE );
	//-------------------------------------------------------------------------
//	End_Set_Delay_Time_BUFFER = clock(); // 2003.10.09
//	End_Set_Delay_Time_BUFFER = GetTickCount(); // 2003.10.09 // 2004.03.11
	//-------------------------------------------------------------------------
//	LeaveCriticalSection( &CR_LOTEND_DELAY ); // 2004.03.11
	//-------------------------------------------------------------------------
//	Scheduler_End_Part_Lock_Clear( 0 ); // 2009.04.13 // 2009.07.15
	//-------------------------------------------------------------------------
	return Res;
}
//
Module_Status Scheduler_End_Post_Part( int CHECKING_SIDE ) { // 2009.07.15
	int Res;
	//-------------------------------------------------------------------------
	Scheduler_End_Part_Lock_Make( 0 );
	//-------------------------------------------------------------------------
	Res = Scheduler_End_Part_Post_for_sub( CHECKING_SIDE );
	//-------------------------------------------------------------------------
	Scheduler_End_Part_Lock_Clear( 0 );
	//-------------------------------------------------------------------------
	return Res;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling Abort Part(2003.06.03)
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Scheduler_All_Abort_Part( BOOL running ) {
	int i , j , k;
	int Run[MAX_CASSETTE_SIDE][MAX_CHAMBER];

	if ( running ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
				Run[i][j] = FALSE;
			}
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
				for ( j = CM1 ; j <= FM ; j++ ) {
					if ( ( j != AL ) && ( j != IC ) ) {
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , j ) > MUF_00_NOTUSE ) {
							Run[i][j] = TRUE;
						}
					}
				}
			}
		}
		for ( j = CM1 ; j <= FM ; j++ ) {
			k = 0;
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( Run[i][j] ) {
					k++;
					break;
				}
			}
			if ( k > 0 ) {
				EQUIP_RUN_FUNCTION_ABORT( j );
				//
				if ( ( j >= PM1 ) && ( j < AL ) ) {
					//
					if ( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( j ) < STOP_LIKE_NONE_ALLABORT_REMAINPRCS ) { // 2012.02.08
						//
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							if ( Run[i][j] ) {
								EQUIP_RUN_PROCESS_ABORT( i , j );
							}
						}
						//
					}
					//
				}
			}
		}
	}
	else {
		for ( j = CM1 ; j <= FM ; j++ ) {
			if ( ( j != AL ) && ( j != IC ) ) {
				EQUIP_RUN_FUNCTION_ABORT( j );
			}
		}
	}
}


//------------------------------------------------------------------------------------------
int  _i_SCH_PREPOST_Get_inside_READY_RECIPE_USE( int data , int data2 ) {
	return _i_SCH_PRM_GET_inside_READY_RECIPE_USE( data , data2 );
}

char *_i_SCH_PREPOST_Get_inside_READY_RECIPE_NAME( int data0 , int data ) {
	return _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( data0 , data );
}
//
int  _i_SCH_PREPOST_Get_inside_END_RECIPE_USE( int data , int data2 ) {
	return _i_SCH_PRM_GET_inside_END_RECIPE_USE( data , data2 );
}

char *_i_SCH_PREPOST_Get_inside_END_RECIPE_NAME( int data0 , int data ) {
	return _i_SCH_PRM_GET_inside_END_RECIPE_NAME( data0 , data );
}

int  _i_SCH_PREPOST_Get_inside_READY_RECIPE_TYPE( int data , int data2 ) {
	return _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( data , data2 );
}

//------------------------------------------------------------------------------------------



Module_Status Scheduler_Begin_One( int CHECKING_SIDE , int i ) {
	if ( ( i < PM1 ) || ( i >= AL ) ) return SYS_ERROR;
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_END );
	//================================================================================
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.05.21
	//================================================================================
	PROCESS_MONITOR_LOG_READY( CHECKING_SIDE , i , TRUE );
	//
	if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {

		_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );

		Scheduler_ProcessTime_Set( i );

		if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) {
			//
			/*
			// 2012.11.12
			if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) {
				if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) != SYS_RUNNING ) {
					_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 );
				}
			}
			else {
				_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 );
			}
			*/
			//
			// 2012.11.12
			if ( Scheduler_Ready_Possible_Check( CHECKING_SIDE , i ) ) {
				_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , 2 );
			}
			//
		}
		else {
			/*
			// 2012.11.12
//			if ( ( _i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2010.05.27
			if ( ( _i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2010.05.27
				if ( _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) {
					if ( EQUIP_STATUS_PROCESS( CHECKING_SIDE , i ) != SYS_RUNNING ) {
//						_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ); // 2010.05.27
						_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ); // 2010.05.27
					}
				}
				else {
//					_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE( i , FALSE , CHECKING_SIDE ) ); // 2010.05.27
					_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ); // 2010.05.27
				}
			}
			*/
			//
			// 2012.11.12
			if ( ( _i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) { // 2010.05.27
				if ( Scheduler_Ready_Possible_Check( CHECKING_SIDE , i ) ) {
					_i_SCH_EQ_BEGIN_END_RUN( i , FALSE , "" , !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) );
				}
			}
			//
		}
	}
	else {
		_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
	}
	//
	_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_01_USE );
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.05.21
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_END );
	//================================================================================
	return SYS_SUCCESS;
}



int Scheduler_Begin_Restart_Part( int CHECKING_SIDE , char *errorstring ) {
	int i , j , k , l , f;
	int xinside0_BeforeLotUsed[MAX_CASSETTE_SIDE][MAX_CHAMBER];
	char strbuffer[(MAX_CHAMBER)*4];
	//
	int startmdl;

	//=============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) {
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) xinside0_BeforeLotUsed[CHECKING_SIDE][i] = 0;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) <= 0 ) continue;
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( CHECKING_SIDE , i ) ; j++ ) {
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					//
					l = _i_SCH_CLUSTER_Get_PathProcessChamber( CHECKING_SIDE , i , j , k );
					//
					if      ( l > 0 ) {
						xinside0_BeforeLotUsed[CHECKING_SIDE][l] = 1;
					}
					else if ( l < 0 ) {
						if ( xinside0_BeforeLotUsed[CHECKING_SIDE][-l] == 0 ) xinside0_BeforeLotUsed[CHECKING_SIDE][-l] = 2;
					}
					//
				}
			}
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( ( i >= PM1 ) && ( i < AL ) ) {
				if ( ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_01_USE ) || ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) == MUF_99_USE_to_DISABLE ) ) {
					//
					if ( xinside0_BeforeLotUsed[CHECKING_SIDE][i] == 0 ) {
						//
						_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i , MUF_00_NOTUSE );
						//
						_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "PM Select Rejected at %s%cPROCESS_REJECT PM%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , 9 , i - PM1 + 1 );
						//
					}
				}
			}
			//
			xinside0_BeforeLotUsed[CHECKING_SIDE][i] = 0;
		}
		//
	}
	//=============================================================================================================
	xinside_global_BM_Return[CHECKING_SIDE] = FALSE;
	//=============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) {
		Scheduler_Make_Mdl_String( CHECKING_SIDE , strbuffer );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Restart Part Start%cBEGINSTART %s\n" , 9 , strbuffer );
	}
	//=============================================================================================
	if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 3 ) {
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_MODULE_GET_USE_ENABLE( i + BM1 , FALSE , CHECKING_SIDE ) ) {
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i + BM1 ) == MUF_00_NOTUSE ) {
					_i_SCH_MODULE_Set_Mdl_Use_Flag( CHECKING_SIDE , i + BM1 , MUF_01_USE );
				}
			}
		}
	}
	//=============================================================================================
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_END );
	//================================================================================
	//================================================================================
	EnterCriticalSection( &CR_PRE_BEGIN_ONLY );
	//================================================================================
	//==========================================================================================================================
//	SYSTEM_BEGIN_SET( CHECKING_SIDE , 1 ); // 2015.07.30
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 2 ); // 2015.07.30
	//==========================================================================================================================
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
		startmdl = CM1;
	}
	else {
		startmdl = PM1;
	}
	//------------------------------------------------------------------------------------
	for ( i = startmdl ; i <= FM ; i++ ) {
		//==============================================================================================
		xinside_global_Run_Tag[CHECKING_SIDE][i] = 0;
		//==============================================================================================
		xinside0_BeforeLotUsed[CHECKING_SIDE][i] = FALSE;
		//==============================================================================================
		if ( ( i != AL ) && ( i != IC ) ) {
			//---------------------
			if ( ( i >= PM1 ) && ( i < AL ) ) PROCESS_MONITOR_LOG_READY( CHECKING_SIDE , i , TRUE );
			//---------------------
			if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) > MUF_00_NOTUSE ) {
				//---------------------------------------------------------------------------------
				if ( i < PM1 ) {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 1 ) {
						Scheduler_ProcessTime_Set( i );
						xinside_global_Run_Tag[CHECKING_SIDE][i] = 1;
						/* */
					}
				}
				else {
					if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) <= 0 ) {
						_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
						Scheduler_ProcessTime_Set( i );
						xinside_global_Run_Tag[CHECKING_SIDE][i] = 1;

						if ( ( i >= BM1 ) && ( i < TM ) ) {
							/* */
						}
						else {
							if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) { // 2005.09.07
								/* */
							}
							else {
								if ( ( _i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) || ( _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() < 7 ) ) {
									/* */
								}
								else {
									xinside_global_Run_Tag[CHECKING_SIDE][i] = 0;
								}
								//
							}
						}
					}
					else {
						//================================================================================================================
						k = 1;
						f = 2; // 2011.04.01
						//================================================================================================================
						if ( ( i >= PM1 ) && ( i < AL ) ) {
							if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , TRUE ) ) {
								k = _i_SCH_MODULE_Get_Mdl_Run_Flag( i );
								for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
									if ( j != CHECKING_SIDE ) {
										if ( ( _i_SCH_SYSTEM_USING_GET( j ) > 0 ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) >= MUF_90_USE_to_XDEC_FROM ) ) k--;
									}
								}
							}
						}
						//================================================================================================================
						if ( k != 0 ) {
							//================================================================================================================
							k = 1;
							f = 0;
							//================================================================================================================
							if ( ( i >= PM1 ) && ( i < AL ) ) {
								if ( Get_RunPrm_MODULE_CHECK_ENABLE_SIMPLE( i , FALSE ) ) {
									k = _i_SCH_MODULE_Get_Mdl_Run_Flag( i );
									for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
										if ( j != CHECKING_SIDE ) {
											if ( ( _i_SCH_SYSTEM_USING_GET( j ) > 0 ) && ( _i_SCH_MODULE_Get_Mdl_Use_Flag( j , i ) == MUF_99_USE_to_DISABLE ) ) k--;
										}
									}
								}
							}
							//================================================================================================================
						}
						//================================================================================================================
						if ( k == 0 ) {
							//================================================================================================================
							_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
							Scheduler_ProcessTime_Set( i );
							xinside_global_Run_Tag[CHECKING_SIDE][i] = 1;
							/* */
							//================================================================================================================
						}
						else {
							_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
							//==============================================================================================
							xinside0_BeforeLotUsed[CHECKING_SIDE][i] = TRUE;
							//==============================================================================================
						}
					}
					//
					if ( !_i_SCH_MODULE_GET_USE_ENABLE_Sub( i , FALSE , CHECKING_SIDE ) ) {
						_SCH_CLUSTER_Parallel_Check_Curr_DisEna( CHECKING_SIDE , -1 , i , TRUE );
					}
					//
				}
			}
		}
	}
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) <= MUF_00_NOTUSE ) continue;
		if ( xinside0_BeforeLotUsed[CHECKING_SIDE][i] ) continue;
		//
		for ( k = 0 ; k < MAX_CASSETTE_SIDE ; k++ ) {
			//
			if ( k == CHECKING_SIDE ) continue;
			//
			if ( _i_SCH_SYSTEM_USING_GET( k ) <= 0 ) continue;
			if ( _i_SCH_SYSTEM_MODE_GET( k ) < 2 ) continue;
			//
			if ( _i_SCH_SYSTEM_RUNORDER_GET( CHECKING_SIDE ) > _i_SCH_SYSTEM_RUNORDER_GET( k ) ) { // 2016.01.26
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( k , i ) == MUF_31_USE_to_NOTUSE_SEQMODE ) {
					//
					xinside0_BeforeLotUsed[CHECKING_SIDE][i] = TRUE;
					break;
					//
				}
				//
			}
			//
		}
	}
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_ONLY );
	//================================================================================
	//
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 3 ); // 2017.07.03
	//================================================================================
	//
	//================================================================================
	LeaveCriticalSection( &CR_PRE_BEGIN_END );
	//================================================================================
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
		startmdl = CM1;
	}
	else {
		startmdl = PM1;
	}
	//------------------------------------------------------------------------------------
	do {
		//------------------------------
		k = 0;
		f = 0;
		//
		strcpy( errorstring , "" );
		//
		for ( i = startmdl ; i <= FM ; i++ ) {
			if ( ( i != AL ) && ( i != IC ) ) {
				if ( ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 1 ) || ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 3 ) ) {
					if ( xinside_global_Run_Tag[CHECKING_SIDE][i] == 1 ) {
						l = SYS_SUCCESS;
					}
					else {
						l = SYS_SUCCESS;
					}
					if ( l == SYS_SUCCESS ) {
						xinside_global_Run_Tag[CHECKING_SIDE][i] = 2;
						//----------------------------------------------------------------------
						if ( ( i >= PM1 ) && ( i < AL ) ) {
							//-----------------------------------------------------------------------------
//							Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i ); // 2018.06.22
							Scheduler_PreCheck_for_ProcessRunning_Part( CHECKING_SIDE , i , NULL , 0 ); // 2018.06.22
							//-----------------------------------------------------------------------------
						}
					}
				}
			}
		}
	}
	while ( k != 0 );
	//================================================================================
	if ( f != 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Restart Part Fail(%s)%cBEGINFAIL\n" , errorstring , 9 );
		return SYS_ABORTED;
	}
	//=================================================================================
	strcpy( errorstring , "" );
	//=================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	if ( _i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) {
		//===================================================================================
		Scheduler_Make_Mdl_String( CHECKING_SIDE , strbuffer );
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Begin Restart Part Success%cBEGINSUCCESS %s\n" , 9 , strbuffer );
	}
	//
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 4 ); // 2017.07.03
	//================================================================================
	//
	if ( !_i_SCH_PRM_GET_MODULE_MODE( CHECKING_SIDE + CM1 ) ) {
		return SYS_SUCCESS;
	}
	//===============================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//-----------------------------
		xinside_global_Run2_Tag[CHECKING_SIDE][i] = xinside_global_Run_Tag[CHECKING_SIDE][i];
		//-----------------------------
		xinside_global_PrcsRunCheck[CHECKING_SIDE][i] = 0;
		//-----------------------------
	}
	//-------------------------------------------
	if ( _i_SCH_SYSTEM_USING_GET( CHECKING_SIDE ) >= 100 ) return SYS_SUCCESS;
	//-------------------------------------------
	strcpy( errorstring , "" );
	//=============================================================================================================
//	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) ) return SYS_ABORTED; // 2016.01.12
	if ( SIGNAL_MODE_ABORT_GET( CHECKING_SIDE ) > 0 ) return SYS_ABORTED; // 2016.01.12
	//=============================================================================================================
	strcpy( errorstring , "" );
	if ( !_SCH_COMMON_PRE_PART_OPERATION( CHECKING_SIDE , 0 , xinside_global_Run_Tag[CHECKING_SIDE] , xinside_global_Run2_Tag[CHECKING_SIDE] , xinside_global_PrcsRunCheck[CHECKING_SIDE] , errorstring ) ) return SYS_ABORTED;
	strcpy( errorstring , "" );
	//=============================================================================================================
	//
	//================================================================================
	SYSTEM_BEGIN_SET( CHECKING_SIDE , 6 ); // 2017.07.03
	//================================================================================
	//
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _i_SCH_SUB_CHECK_PRE_END( int side , int tmside , int ch , BOOL lastcheckskip ) { // 2018.11.29
	int i , j , k , r , pmc;
	int Sch_Ch_End_Chk[MAX_PM_CHAMBER_DEPTH];
	//
	if ( !lastcheckskip ) {
		if ( !_i_SCH_SUB_Get_Last_Status( side ) ) return 1;
	}
	//
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , side ) ) return 2; // 2018.12.10
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( ( ch > 0 ) && ( ch != i ) ) {
			Sch_Ch_End_Chk[i-PM1] = 9;
		}
		else {
			//
			if      ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
				Sch_Ch_End_Chk[i-PM1] = 4;
			}
			else if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) >= 2 ) {
				Sch_Ch_End_Chk[i-PM1] = 5;
			}
			else {
				if ( tmside >= 0 ) {
					if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( tmside , i ) ) {
						if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_01_USE ) {
							Sch_Ch_End_Chk[i-PM1] = 0;
						}
						else {
							Sch_Ch_End_Chk[i-PM1] = 1;
						}
					}
					else {
						Sch_Ch_End_Chk[i-PM1] = 2;
					}
				}
				else {
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_01_USE ) {
						Sch_Ch_End_Chk[i-PM1] = 0;
					}
					else {
						Sch_Ch_End_Chk[i-PM1] = 3;
					}
				}
			}
		}
		//
	}
	//
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		//
		r = _i_SCH_CLUSTER_Get_PathRange( side , i );
		//
		if ( r <= 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_CM_END ) continue;
		//
		for ( j = 0 ; j < r ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRun( side , i , j , 0 ) != 0 ) continue;
			//
			for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
				//
				pmc = _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
				//
				if ( ( pmc >= PM1 ) && ( pmc < AL ) ) Sch_Ch_End_Chk[pmc-PM1] = 6;
			}
		}
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( Sch_Ch_End_Chk[i] != 0 ) continue;
		//
		r = FALSE;
		//
		for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i + PM1 ) ; j++ ) {
			//
			if ( _i_SCH_MODULE_Get_PM_WAFER( i + PM1 , j ) > 0 ) {
				r = FALSE;
				break;
			}
			//
			if ( _i_SCH_PRM_GET_DFIM_SLOT_NOTUSE( i + PM1 , j + 1 ) ) continue;
			//
			r = TRUE;
			//
		}
		//
		if ( r ) {
			if ( _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() == 3 ) {
				_i_SCH_PREPOST_Pre_End_Part( side , i + PM1 );
			}
			else {
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i + PM1 ) <= 0 ) {
					_i_SCH_PREPOST_Pre_End_Part( side , i + PM1 );
				}
			}
		}
		else {
			_i_SCH_PREPOST_Pre_End_Part( side , ( i + PM1 ) + 1000 );
		}
		//
	}
	//
	return 0;
	//
}