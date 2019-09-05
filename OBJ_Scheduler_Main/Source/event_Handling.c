#include "default.h"

#include "EQ_Enum.h"

#include "system_tag.h"
#include "DLL_Interface.h"
#include "Equip_Handling.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "iolog.h"
#include "sch_main.h"
#include "sch_cassmap.h"
#include "sch_CassetteResult.h"
#include "sch_prepost.h"
#include "sch_preprcs.h"
#include "sch_processmonitor.h"
#include "sch_transfer.h"
#include "sch_transfermaintinf.h"
#include "sch_transferuser.h"
#include "sch_sub.h"
#include "sch_multi_alic.h"
#include "sch_prm.h"
#include "sch_prm_log.h"
#include "sch_sdm.h"
#include "sch_util.h"
#include "sch_EIL.h"
#include "DLL_Interface.h"
#include "FA_Handling.h"
#include "MR_Data.h"
#include "Multijob.h"
#include "Multireg.h"
#include "Gui_Handling.h"
#include "Timer_Handling.h"
#include "Errorcode.h"
#include "Utility.h"
#include "File_ReadInit.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"

#include "sch_HandOff.h"	// 2017.05.22

#include "sch_CassetteSupply.h"

#include "INF_sch_CommonUser.h"
#include "INF_sch_macro.h"


void _FLOW_MAP_CHART();


extern int SIM_UNLOADLOAD_OPTION; // x noloop waiting noloop+waiting 2018.12.13
extern int SIM_WAITING_TAG[MAX_CASSETTE_SIDE]; // 0:wait 1:success 2:error 3:abort


extern int  _SCH_MACRO_TM_ERROR_PICK_MODE[MAX_TM_CHAMBER_DEPTH]; // 2018.09.20	0:x 1:ERROR 2:+WaferReturn(+ReRun) 3:+WaferReturn 11:ERROR(onetime) 12:+WaferReturn(+ReRun)(onetime) 13:+WaferReturn(onetime)
extern int  _SCH_MACRO_TM_ERROR_PLACE_MODE[MAX_TM_CHAMBER_DEPTH]; // 2018.09.20 0:x 1:ERROR 2:+WaferReturn(+ReRun) 3:+WaferReturn 11:ERROR(onetime) 12:+WaferReturn(+ReRun)(onetime) 13:+WaferReturn(onetime)

int SCH_PATHOUT_RESET( int s , int cm ); // 2018.07.17

extern int USER_FIXED_JOB_OUT_CASSETTE; // 2018.07.17

void SCH_SW_BM_SCHEDULING_FACTOR_ORDER_SET( char *data ); // 2017.04.21

void EST_CONTROL_RUN( int mode , int pmtime , int bmtime , int cmtime );

/*

START기능

	기본

		(F)		FA MODE
		숫자	SIDE

	PART별

		1.SUPPLY	(JOB Part와 병행 사용 불가)

			''	CONCURRENT(Default)

			'P'	CONTINUE MODE
			'S'	SEQUENTIAL MODE
			'O'	SEQUENTIAL2 MODE

		2.RESTART	(JOB,MOVE Part와 병행 사용 불가)

			'R'	ABORT후 마지막 상태에서 RESTART 기능 수행(CM도 재시작)
			'T'	ABORT후 마지막 상태에서 RESTART 기능 수행(CM은 재시작 하지 않음)
			'A'	ABORT후 마지막 상태에서 RESTART 기능 수행(CM도 재시작)(ABORT된 SIDE와 같은 모든 SIDE에서 RESTART 진행)

		3.JOB	(SUPPLY,RESTART Part와 병행 사용 불가)

			'J'	JOB 형태로 START.
				(MAPPING정보가 현재 상태로 설정되고 MAPPING 완료 상태로 변경)

			'M'	JOB 형태로 START.
				(MAPPING상태가 변경되지 않음-실제 START가 되기 위하여 MAPPING 기능을 수행할 필요가 있음)

		4.MOVE	(RESTART Part와 병행 사용 불가)

			'V'	Recipe가 설정된 PM에 대해 Wafer를 공급하고 종료
			'Z'	Recipe가 설정된 PM에 대해 이미 공급된 Wafer를 회수하고 종료

		5.ETC

			'E'	EXPEDITE START
			'X'	HOTRUN START

			'N'	FIRST_MODULE_PUT_DELAY 기능 사용 안함(JOB,RESTART,MOVE Part와 병행 사용 불가)

*/


extern BOOL EVENT_MESSAGE_LOG; // 2015.04.09
extern BOOL NOWAY_CONTROL; // 2015.09.17

//---------------------------------------------------------------------------------------
extern int  _in_UNLOAD_USER_TYPE[MAX_CASSETTE_SIDE]; // 2010.04.26
extern int  _in_HOT_LOT_CHECKING[MAX_CASSETTE_SIDE]; // 2011.06.13

//---------------------------------------------------------------------------------------

BOOL SCHMULTI_CHECK_HANDOFF_OUT_CHECK( BOOL , int Side , int Pointer , int ch , int );

//---------------------------------------------------------------------------------------
//void SCHEDULER_Make_CARRIER_INDEX( int side ); // 2011.09.14
void _i_SCH_SUB_Make_FORCE_FMSIDE( int );
//---------------------------------------------------------------------------------------
//void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch ); // 2010.01.26 // 2018.06.22
void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch , char *recipe , int extmode ); // 2010.01.26 // 2018.06.22
void SCHEDULER_CONTROL_Make_Finish_Data( int side , int clear_wfr_Info ); // 2011.04.27
//---------------------------------------------------------------------------------------
int SCHEDULER_CM_LOCKED_for_HANDOFF( int cm ); // 2011.03.18
int SCHEDULER_CM_LOCKED_CHECK_FOR_UNLOAD( int cm ); // 2012.04.01
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
int SYSTEM_LOG_TAG = 0;
//
int SYSTEM_WIN_UPDATE = 0;
int SYSTEM_WIN_COUNT = 0;
int SYSTEM_WIN_TAG = 0;
BOOL SYSTEM_WIN_DATA = FALSE;
int SYSTEM_WIN_X = 0;
int SYSTEM_WIN_Y = 0;
int SYSTEM_WIN_XS = 0;
int SYSTEM_WIN_YS = 0;
char SYSTEM_WIN_FOLDER[256];
//
int SYSTEM_WIN_INT_DATA = 0; // 2016.07.15
char SYSTEM_WIN_STR_DATA[256]; // 2016.07.15

int SYSTEM_WIN_R_TAG = 0;
//---------------------------------------------------------------------------------------
void SYSTEM_LOG_WIN_MONITOR() { // 2013.03.11
	char FileName[256];
	BOOL dbgrun;
	//
	SYSTEMTIME SysTime;
	//
	dbgrun = FALSE;
	//
	while ( SYSTEM_WIN_TAG != 0 ) {
		//
		if ( SYSTEM_WIN_UPDATE > 0 ) {
			//
			SYSTEM_WIN_UPDATE = 0;
			//
			SYSTEM_WIN_COUNT++;
			//
			if ( SYSTEM_WIN_COUNT >= 1000 ) SYSTEM_WIN_COUNT = 1;
			//
			dbgrun = TRUE; // 2015.06.11
			//
			GetLocalTime( &SysTime );
			//
			sprintf( FileName , "%s/[%03d] %02d-%02d-%02d-%03d.dbg" , SYSTEM_WIN_FOLDER , SYSTEM_WIN_COUNT , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds ); // 2015.06.11
			//
			IO_DEBUG_FILE_ENABLE( -1 , FileName , 100 , TRUE ); // 2015.06.11
			//
			sprintf( FileName , "%s/[%03d] %02d-%02d-%02d-%03d.log" , SYSTEM_WIN_FOLDER , SYSTEM_WIN_COUNT , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			//
			_i_SCH_SYSTEM_INFO_LOG( FileName , FileName );
			//
			sprintf( FileName , "%s/[%03d] %02d-%02d-%02d-%03d.jpg" , SYSTEM_WIN_FOLDER , SYSTEM_WIN_COUNT , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds );
			//
			if ( SYSTEM_WIN_DATA ) {
				KWIN_SCREEN_CAPTURE_EX( SYSTEM_WIN_X , SYSTEM_WIN_Y ,
					SYSTEM_WIN_XS , SYSTEM_WIN_YS , FileName , 25 );
			}
			else {
				KWIN_SCREEN_CAPTURE_EX( GET_WINDOW_X_REFERENCE_POSITION() , GET_WINDOW_Y_REFERENCE_POSITION() ,
					GET_WINDOW_XS_REFERENCE_POSITION() , GET_WINDOW_YS_REFERENCE_POSITION() , FileName , 25 );
			}
			//
			if ( SYSTEM_WIN_UPDATE == 0 ) {
				//
				Sleep(1);
				//
				IO_DEBUG_DISABLE( TRUE ); // 2015.06.11
				//
				dbgrun = FALSE; // 2015.06.11
				//
			}
			//
		}
		else {
			//
			Sleep(10);
			//
			if ( dbgrun ) { // 2015.06.11
				//
				IO_DEBUG_DISABLE( TRUE );
				//
				dbgrun = FALSE;
				//
			}
			//
		}
	}
	//
	if ( dbgrun ) IO_DEBUG_DISABLE( TRUE ); // 2015.06.11
	//
	_endthread();
}
//
/*
//
// 2016.07.15
//
void SYSTEM_LOG_TAG_MONITOR( int data ) {
	IO_DEBUG_WAITING( TRUE ); // 2009.03.02
	//
	Sleep( data );
	//
	IO_DEBUG_WAITING( FALSE ); // 2009.03.02
	//
	IO_DEBUG_DISABLE( FALSE ); // 2015.06.11
	//
	SYSTEM_LOG_TAG = 0;
	_endthread();
}
*/
//
// 2016.07.15
//
void SYSTEM_LOG_TAG_MONITOR( int data ) {
	//
	char dispBuffer[256];
	int  dispcnt, i;
	DWORD timedata;
	//
	IO_DEBUG_WAITING( TRUE ); // 2009.03.02
	//
	if ( SYSTEM_WIN_INT_DATA  == 0 ) {
		//
		Sleep( data );
		//
	}
	else {
		//
		SYSTEM_WIN_UPDATE = 0;
		//
		timedata = GetTickCount();
		//
		dispcnt = 0;;
		//
		while ( TRUE ) {
			//
			for ( i = 0 ; i < 6 ; i++ ) {
				//
				Sleep( 500 );
				//
				if ( ( GetTickCount() - timedata ) >= (unsigned) data ) break;
				//
			}
			//
			if ( ( GetTickCount() - timedata ) >= (unsigned) data ) break;
			//
			if ( SYSTEM_WIN_UPDATE >= 10 ) {
				//
				dispcnt++;
				//
				sprintf( dispBuffer , "MONITORING[%d](%d)" , SYSTEM_WIN_UPDATE , dispcnt );
				//
				_i_SCH_SYSTEM_INFO_LOG( SYSTEM_WIN_STR_DATA , dispBuffer );
				//
				SYSTEM_WIN_UPDATE = 0;
				//
			}
			//
		}
		//
	}
	//
	IO_DEBUG_WAITING( FALSE ); // 2009.03.02
	//
	IO_DEBUG_DISABLE( FALSE ); // 2015.06.11
	//
	SYSTEM_LOG_TAG = 0;
	_endthread();
}

//---------------------------------------------------------------------------------------
int Get_LOT_ID_From_String( char *mdata , BOOL idalso ) {
	int i;
	if      ( STRNCMP_L( mdata , "PORT" , 4 ) ) {
		i = atoi( mdata + 4 ) - 1;
		if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) i = -1;
	}
	else if ( STRNCMP_L( mdata , "CM" , 2 ) ) {
		i = atoi( mdata + 2 ) - 1;
		if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) i = -1;
	}
	else {
		if ( idalso ) { // 2006.11.17
			if ( ( mdata[0] >= '1' ) && ( mdata[0] <= '9' ) ) {
				i = atoi( mdata ) - 1;
				if ( ( i < 0 ) || ( i >= MAX_CASSETTE_SIDE ) ) i = -1;
			}
			else {
				i = -1;
			}
		}
		else {
			i = -1;
		}
	}
	return i;
}
//
int Get_PMBM_ID_From_String( int mode , char *mdlstr , int badch , int *resch ) {
	// mode = 0 : PMBM , 1 = PM , 2 = BM
	int res;
	//
	if ( STRNCMP_L( mdlstr , "PM" , 2 ) ) {
		if ( mode == 2 ) {
			*resch = badch;
			return 0;
		}
		res = atoi( mdlstr + 2 ) - 1 + PM1;
		if ( res >= PM1 ) {
			if ( ( res - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) {
				*resch = res;
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( res , FALSE , -1 ) ) return 2;
				return 1;
			}
		}
	}
	else if ( STRNCMP_L( mdlstr , "P" , 1 ) ) {
		if ( mode == 2 ) {
			*resch = badch;
			return 0;
		}
		res = atoi( mdlstr + 1 ) - 1 + PM1;
		if ( res >= PM1 ) {
			if ( ( res - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) {
				*resch = res;
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( res , FALSE , -1 ) ) return 2;
				return 1;
			}
		}
	}
	else if ( STRNCMP_L( mdlstr , "BM" , 2 ) ) {
		if ( mode == 1 ) {
			*resch = badch;
			return 0;
		}
		res = atoi( mdlstr + 2 ) - 1 + BM1;
		if ( res >= BM1 ) {
			if ( ( res - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) {
				*resch = res;
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( res , FALSE , -1 ) ) return 2;
				return 1;
			}
		}
	}
	else if ( STRNCMP_L( mdlstr , "B" , 1 ) ) {
		if ( mode == 1 ) {
			*resch = badch;
			return 0;
		}
		res = atoi( mdlstr + 1 ) - 1 + BM1;
		if ( res >= BM1 ) {
			if ( ( res - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) {
				*resch = res;
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( res , FALSE , -1 ) ) return 2;
				return 1;
			}
		}
	}
	*resch = badch;
	return 0;
}
//
int Get_Module_ID_From_String( int mode , char *mdlstr , int badch ) {
	// 0 : All
	// 1 : All + CTC
	// 2 : C , P , B , IC , CTC
//	// 3 : B , IC // 2012.04.30
	// 3 : P , B , IC // 2012.04.30
	int res;
	//
	if ( mode == 3 ) {
		if ( STRCMP_L( mdlstr , "IC"  ) ) return IC;
	}
	else {
		if ( STRCMP_L( mdlstr , "CTC"  ) ) {
			if ( ( mode == 1 ) || ( mode == 2 ) ) return MAX_CHAMBER;
			return badch;
		}
//		if ( mode != 2 ) { // 2007.04.19
			if ( STRCMP_L( mdlstr , "AL"  ) ) return AL;
			if ( STRCMP_L( mdlstr , "IC"  ) ) return IC;
			if ( STRCMP_L( mdlstr , "FAL" ) ) return F_AL;
			if ( STRCMP_L( mdlstr , "FIC" ) ) return F_IC;
			if ( STRCMP_L( mdlstr , "TM"  ) ) return TM;
			if ( STRCMP_L( mdlstr , "FM"  ) ) return FM;
			if ( STRCMP_L( mdlstr , "F"   ) ) return FM; // 2007.06.29
//		} // 2007.04.19
	}
	//
	if ( mode != 3 ) {
		if ( STRNCMP_L( mdlstr , "PORT" , 4 ) ) {
			res = atoi( mdlstr + 4 ) - 1 + CM1;
			if ( res >= CM1 ) {
				if ( ( res - CM1 + 1 ) <= MAX_CASSETTE_SIDE ) return res;
			}
			return badch;
		}
		if ( STRNCMP_L( mdlstr , "CM" , 2 ) ) {
			res = atoi( mdlstr + 2 ) - 1 + CM1;
			if ( res >= CM1 ) {
				if ( ( res - CM1 + 1 ) <= MAX_CASSETTE_SIDE ) return res;
			}
			return badch;
		}
		if ( STRNCMP_L( mdlstr , "C" , 1 ) ) {
			res = atoi( mdlstr + 1 ) - 1 + CM1;
			if ( res >= CM1 ) {
				if ( ( res - CM1 + 1 ) <= MAX_CASSETTE_SIDE ) return res;
			}
			return badch;
		}
		/*
		// 2012.04.30
		if ( STRNCMP_L( mdlstr , "PM" , 2 ) ) {
			res = atoi( mdlstr + 2 ) - 1 + PM1;
			if ( res >= PM1 ) {
				if ( ( res - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) return res;
			}
			return badch;
		}
		if ( STRNCMP_L( mdlstr , "P" , 1 ) ) {
			res = atoi( mdlstr + 1 ) - 1 + PM1;
			if ( res >= PM1 ) {
				if ( ( res - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) return res;
			}
			return badch;
		}
		*/
	}
	// 2012.04.30
	if ( STRNCMP_L( mdlstr , "PM" , 2 ) ) {
		res = atoi( mdlstr + 2 ) - 1 + PM1;
		if ( res >= PM1 ) {
			if ( ( res - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) return res;
		}
		return badch;
	}
	if ( STRNCMP_L( mdlstr , "P" , 1 ) ) {
		res = atoi( mdlstr + 1 ) - 1 + PM1;
		if ( res >= PM1 ) {
			if ( ( res - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) return res;
		}
		return badch;
	}
	//
	if ( STRNCMP_L( mdlstr , "BM" , 2 ) ) {
		res = atoi( mdlstr + 2 ) - 1 + BM1;
		if ( res >= BM1 ) {
			if ( ( res - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) return res;
		}
		return badch;
	}
	if ( STRNCMP_L( mdlstr , "B" , 1 ) ) {
		res = atoi( mdlstr + 1 ) - 1 + BM1;
		if ( res >= BM1 ) {
			if ( ( res - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) return res;
		}
		return badch;
	}
	if ( mode != 3 ) {
		if ( mode != 2 ) {
			if ( STRNCMP_L( mdlstr , "TM" , 2 ) ) {
				res = atoi( mdlstr + 2 ) - 1 + TM;
				if ( res >= TM ) {
					if ( ( res - TM + 1 ) <= MAX_TM_CHAMBER_DEPTH ) return res;
				}
				return badch;
			}
			if ( STRNCMP_L( mdlstr , "T" , 1 ) ) {
				res = atoi( mdlstr + 1 ) - 1 + TM;
				if ( res >= TM ) {
					if ( ( res - TM + 1 ) <= MAX_TM_CHAMBER_DEPTH ) return res;
				}
				return badch;
			}
		}
	}
	return badch;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Event_Message_Reject( char *data ) {
	if ( ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 2 ) || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() == 3 ) ) { // 2006.02.02
		_IO_CIM_PRINTF( "SCHEDULER EVENT REJECT = [%s][%s]\n" , PROGRAM_EVENT_READ() , data );
	}
	else {
		printf( "SCHEDULER EVENT REJECT = [%s][%s]\n" , PROGRAM_EVENT_READ() , data );
	}
	GUI_Reject_Event_Inserting( data );
	MessageBeep(MB_ICONHAND);
}
//
BOOL Event_LOT_ID_From_String_And_Check( char *mdata , int *index ) {
	*index = Get_LOT_ID_From_String( mdata , TRUE );
	if ( ( *index < 0 ) || ( *index >= MAX_CASSETTE_SIDE ) ) {
		Event_Message_Reject( "PORT ANALYSIS ERROR" );
		return FALSE;
	}
	return TRUE;
}

//BOOL Event_CM_Run_Possible_Check( int index , BOOL mdlchk ) { // 2016.07.15
//BOOL Event_CM_Run_Possible_Check( int index , BOOL unload , BOOL mdlchk ) { // 2016.07.15 // 2016.10.05
BOOL Event_CM_Run_Possible_Check( int index , BOOL always , BOOL unload , BOOL mdlchk ) { // 2016.07.15 // 2016.10.05
	//
	if ( !always ) { // 2016.10.05
		//
		if ( Get_RunPrm_RUNNING_TRANSFER_EXCEPT_AUTOMAINT() ) { // 2012.07.28
			Event_Message_Reject( "TRANSFER SYSTEM IS RUNNING ALREADY" );
			return FALSE;
		}
		//
	} // 2016.10.05
	//
	switch( BUTTON_GET_FLOW_STATUS_VAR( index ) ) {
	case _MS_0_IDLE : // Init
	case _MS_2_LOADED : // Loading(Success)
	case _MS_3_LOADFAIL : // Loading(Fail)
	case _MS_5_MAPPED : // Mapping(Success)
	case _MS_6_MAPFAIL : // Mapping(Fail)
	case _MS_15_COMPLETE : // Running(Success)
	case _MS_16_CANCEL : // Running(Success with Cancel)
	case _MS_17_ABORTED : // Running(Aborted)
	case _MS_20_UNLOADED_C :
	case _MS_21_UNLOADED : // Unloading(Success)
	case _MS_22_UNLOADFAIL : // Unloading(Fail)
		break;
	default :
		//
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
//			Event_Message_Reject( "SYSTEM IS RUNNING ALREADY" ); // 2013.06.08
//			return FALSE; // 2013.06.08
		}
		else {
			if ( unload ) { // 2016.07.15
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 0 ) {
					if ( _i_SCH_SYSTEM_USING_GET( index ) > 0 ) {
						Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
						return FALSE;
					}
				}
			}
			else {
				if ( _i_SCH_SYSTEM_USING_GET( index ) > 0 ) {
					Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
					return FALSE;
				}
			}
		}
		break;
		//
	}
	//
	if ( mdlchk ) { // 2007.02.20
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( index + CM1 , FALSE , -1 ) ) { // 2006.11.09
			Event_Message_Reject( "DISABLE PORT" );
			return FALSE;
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int Event_Get_Side_Data( char *data , BOOL wchwck , int *famode , int *index , int *waitdata ) {
	int l; // 2012.07.23
	char buffer[64];
	//
	*index = -1;
	*waitdata = 0;
	/*
	// 2012.07.23
	if      ( STRCMP_L( data , "" ) )  { *famode = 0; *index = 0; }
	else if ( STRCMP_L( data , "1" ) ) { *famode = 0; *index = 0; }
	else if ( STRCMP_L( data , "2" ) ) { *famode = 0; *index = 1; }
	else if ( STRCMP_L( data , "3" ) ) { *famode = 0; *index = 2; }
	else if ( STRCMP_L( data , "4" ) ) { *famode = 0; *index = 3; }
	else if ( STRCMP_L( data , "5" ) ) { *famode = 0; *index = 4; }
	else if ( STRCMP_L( data , "6" ) ) { *famode = 0; *index = 5; }
	else if ( STRCMP_L( data , "7" ) ) { *famode = 0; *index = 6; }
	else if ( STRCMP_L( data , "8" ) ) { *famode = 0; *index = 7; }
	else if ( STRCMP_L( data , "9" ) ) { *famode = 0; *index = 8; }
	else if ( STRCMP_L( data , "10" ) ) { *famode = 0; *index = 9; }
	else if ( STRCMP_L( data , "11" ) ) { *famode = 0; *index = 10; }
	else if ( STRCMP_L( data , "12" ) ) { *famode = 0; *index = 11; }
	else if ( STRCMP_L( data , "13" ) ) { *famode = 0; *index = 12; }
	else if ( STRCMP_L( data , "14" ) ) { *famode = 0; *index = 13; }
	else if ( STRCMP_L( data , "15" ) ) { *famode = 0; *index = 14; }
	else if ( STRCMP_L( data , "16" ) ) { *famode = 0; *index = 15; }
	else if ( STRCMP_L( data , "17" ) ) { *famode = 0; *index = 16; }
	else if ( STRCMP_L( data , "18" ) ) { *famode = 0; *index = 17; }
	else if ( STRCMP_L( data , "19" ) ) { *famode = 0; *index = 18; }
	else if ( STRCMP_L( data , "20" ) ) { *famode = 0; *index = 19; }
	else if ( STRCMP_L( data , "(F)" ) )  { *famode = 2; *index = 0; }
	else if ( STRCMP_L( data , "1(F)" ) ) { *famode = 2; *index = 0; }
	else if ( STRCMP_L( data , "2(F)" ) ) { *famode = 2; *index = 1; }
	else if ( STRCMP_L( data , "3(F)" ) ) { *famode = 2; *index = 2; }
	else if ( STRCMP_L( data , "4(F)" ) ) { *famode = 2; *index = 3; }
	else if ( STRCMP_L( data , "5(F)" ) ) { *famode = 2; *index = 4; }
	else if ( STRCMP_L( data , "6(F)" ) ) { *famode = 2; *index = 5; }
	else if ( STRCMP_L( data , "7(F)" ) ) { *famode = 2; *index = 6; }
	else if ( STRCMP_L( data , "8(F)" ) ) { *famode = 2; *index = 7; }
	else if ( STRCMP_L( data , "9(F)" ) ) { *famode = 2; *index = 8; }
	else if ( STRCMP_L( data , "10(F)" ) ) { *famode = 2; *index = 9; }
	else if ( STRCMP_L( data , "11(F)" ) ) { *famode = 2; *index = 10; }
	else if ( STRCMP_L( data , "12(F)" ) ) { *famode = 2; *index = 11; }
	else if ( STRCMP_L( data , "13(F)" ) ) { *famode = 2; *index = 12; }
	else if ( STRCMP_L( data , "14(F)" ) ) { *famode = 2; *index = 13; }
	else if ( STRCMP_L( data , "15(F)" ) ) { *famode = 2; *index = 14; }
	else if ( STRCMP_L( data , "16(F)" ) ) { *famode = 2; *index = 15; }
	else if ( STRCMP_L( data , "17(F)" ) ) { *famode = 2; *index = 16; }
	else if ( STRCMP_L( data , "18(F)" ) ) { *famode = 2; *index = 17; }
	else if ( STRCMP_L( data , "19(F)" ) ) { *famode = 2; *index = 18; }
	else if ( STRCMP_L( data , "20(F)" ) ) { *famode = 2; *index = 19; }
	else if ( STRCMP_L( data , "W" ) )  { *famode = 0; *index = 0; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W1" ) ) { *famode = 0; *index = 0; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W2" ) ) { *famode = 0; *index = 1; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W3" ) ) { *famode = 0; *index = 2; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W4" ) ) { *famode = 0; *index = 3; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W5" ) ) { *famode = 0; *index = 4; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W6" ) ) { *famode = 0; *index = 5; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W7" ) ) { *famode = 0; *index = 6; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W8" ) ) { *famode = 0; *index = 7; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W9" ) ) { *famode = 0; *index = 8; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W10" ) ) { *famode = 0; *index = 9; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W11" ) ) { *famode = 0; *index = 10; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W12" ) ) { *famode = 0; *index = 11; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W13" ) ) { *famode = 0; *index = 12; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W14" ) ) { *famode = 0; *index = 13; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W15" ) ) { *famode = 0; *index = 14; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W16" ) ) { *famode = 0; *index = 15; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W17" ) ) { *famode = 0; *index = 16; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W18" ) ) { *famode = 0; *index = 17; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W19" ) ) { *famode = 0; *index = 18; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W20" ) ) { *famode = 0; *index = 19; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W(F)" ) )  { *famode = 2; *index = 0; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W1(F)" ) ) { *famode = 2; *index = 0; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W2(F)" ) ) { *famode = 2; *index = 1; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W3(F)" ) ) { *famode = 2; *index = 2; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W4(F)" ) ) { *famode = 2; *index = 3; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W5(F)" ) ) { *famode = 2; *index = 4; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W6(F)" ) ) { *famode = 2; *index = 5; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W7(F)" ) ) { *famode = 2; *index = 6; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W8(F)" ) ) { *famode = 2; *index = 7; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W9(F)" ) ) { *famode = 2; *index = 8; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W10(F)" ) ) { *famode = 2; *index = 9; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W11(F)" ) ) { *famode = 2; *index = 10; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W12(F)" ) ) { *famode = 2; *index = 11; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W13(F)" ) ) { *famode = 2; *index = 12; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W14(F)" ) ) { *famode = 2; *index = 13; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W15(F)" ) ) { *famode = 2; *index = 14; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W16(F)" ) ) { *famode = 2; *index = 15; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W17(F)" ) ) { *famode = 2; *index = 16; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W18(F)" ) ) { *famode = 2; *index = 17; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W19(F)" ) ) { *famode = 2; *index = 18; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "W20(F)" ) ) { *famode = 2; *index = 19; if ( wchwck ) *waitdata = 1; else *index = -1; }
	else if ( STRCMP_L( data , "_F" ) ) {
		*famode = 1;
		return 1;
	}
	else if ( STRCMP_L( data , "W_F" ) ) {
		*famode = 1;
		if ( wchwck ) {
			*waitdata = 1;
		}
		else {
			Event_Message_Reject( "PORT ANALYSIS ERROR" );
			return 0;
		}
		return 1;
	}
	*/

	// 2012.07.23
	if      ( STRCMP_L( data , "_F" ) ) {
		*famode = 1;
		return 1;
	}
	else if ( STRCMP_L( data , "W_F" ) ) {
		*famode = 1;
		if ( wchwck ) {
			*waitdata = 1;
		}
		else {
			Event_Message_Reject( "PORT ANALYSIS(WF) ERROR" );
			return 0;
		}
		return 1;
	}
	//==================================================================================================================
	else {
		//
		if ( tolower( data[0] ) == 'w' ) {
			if ( wchwck ) {
				*waitdata = 1;
				strcpy( buffer , data + 1 );
			}
			else {
				Event_Message_Reject( "PORT ANALYSIS(W) ERROR" );
				return 0;
			}
		}
		else {
			strcpy( buffer , data );
		}
		//
		*famode = 0;
		//
		l = strlen( buffer );
		//
		if ( l >= 3 ) {
			if ( STRCMP_L( buffer + l - 3 , "(F)" ) ) {
				*famode = 2;
				buffer[l-3] = 0;
			}
		}
		//
		if ( STRCMP_L( buffer , "" ) )  {
			*index = 0;
		}
		else {
			*index = atoi(buffer) - 1;
		}
		//
	}
	//==================================================================================================================
	if ( ( *index < 0 ) || ( *index >= MAX_CASSETTE_SIDE ) ) {
		Event_Message_Reject( "PORT ANALYSIS ERROR" );
		return 0;
	}
	return 2;
}



int Maint_Intf_Spawn( BOOL evt , char *RunStr , char *ElseStr ) {
	//==============================================================================================================================================================================
	//
	// MAINT_INTERFACEF PICK_FM|Trid|CH|Slot1|Slot2|Trorder
	// MAINT_INTERFACEF PLACE_FM|Trid|CH|Slot1|Slot2|Trorder
	// MAINT_INTERFACEF ROTATE_FM|Trid|CH|Slot1|Slot2|Trorder
	//
	// MAINT_INTERFACEA WAITING_FOR_FM|Trid|BM1(CH)|SrcCass
	// MAINT_INTERFACEA WAITING_FOR_TM|Trid|BM1(CH)|SrcCass
	//
	// MAINT_INTERFACE%d PICK_TM%d|Trid|CH|ARM|SLOT|DEPTH|Trorder|SrcCass
	// MAINT_INTERFACE%d PLACE_TM%d|Trid|CH|ARM|SLOT|DEPTH|Trorder|SrcCass
	// MAINT_INTERFACE%d ROTATE_TM%d|Trid|CH|ARM|SLOT|DEPTH|Trorder|SrcCass
	//
	//==============================================================================================================================================================================
	int	 Offset;
	char Else1Str[513] , Else2Str[513] , Else3Str[513];
	int AutoUse , Mode , Index , Check;
	//
	int tmatm;
	int Chamber;
	int Finger;
	int Slot;
	int Depth;
	int ArmASlot;
	int ArmBSlot;
	int SrcCass;
	int BMChamber;
	int TransferID;
	int Trorder;
	//

	//==============================================================================================================================================================================
	if ( STRNCMP_L( RunStr + 15 , "_A" , 2 ) ) { // 2008.09.04
		Offset = 2;
		AutoUse = 1;
	}
	else {
		Offset = 0;
		AutoUse = 0;
	}
	//==============================================================================================================================================================================
	Check = 0;
	if      ( RunStr[ 15 + Offset ] == 0 ) {
		Index = 0;
	}
	else if ( RunStr[ 15 + Offset ] == 'F' ) {
		Index = MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH;
		if      ( STRCMP_L( RunStr + 16 + Offset , "(C)" ) ) Check = 1; // Enable:Wafer
		else if ( STRCMP_L( RunStr + 16 + Offset , "(W)" ) ) Check = 2; // Wafer
		else if ( STRCMP_L( RunStr + 16 + Offset , "(M)" ) ) Check = 3; // Enable
		else if ( STRCMP_L( RunStr + 16 + Offset , "(D)" ) ) Check = -1; // DBSET_Only // 2012.11.07
	}
	else {
		if ( ( RunStr[ 15 + Offset ] >= '1' ) && ( RunStr[ 15 + Offset ] <= '9' ) ) {
			Index = RunStr[ 15 + Offset ] - '1';
			if ( Index >= MAX_TM_CHAMBER_DEPTH ) Index = 0;
			if      ( STRCMP_L( RunStr + 16 + Offset , "(C)" ) ) Check = 1;
			else if ( STRCMP_L( RunStr + 16 + Offset , "(W)" ) ) Check = 2;
			else if ( STRCMP_L( RunStr + 16 + Offset , "(M)" ) ) Check = 3;
			else if ( STRCMP_L( RunStr + 16 + Offset , "(D)" ) ) Check = -1; // 2012.11.07
		}
		else if ( ( RunStr[ 15 + Offset ] >= 'A' ) && ( RunStr[ 15 + Offset ] <= 'Z' ) ) {
			Index = RunStr[ 15 + Offset ] - 'A';
			if ( Index >= MAX_BM_CHAMBER_DEPTH ) {
				Index = 0;
			}
			else {
				Index = Index + MAX_TM_CHAMBER_DEPTH;
			}
			if      ( STRCMP_L( RunStr + 16 + Offset , "(C)" ) ) Check = 1;
			else if ( STRCMP_L( RunStr + 16 + Offset , "(W)" ) ) Check = 2;
			else if ( STRCMP_L( RunStr + 16 + Offset , "(M)" ) ) Check = 3;
			else if ( STRCMP_L( RunStr + 16 + Offset , "(D)" ) ) Check = -1; // 2012.11.07
		}
		else {
			Index = 0;
			if      ( STRCMP_L( RunStr + 15 + Offset , "(C)" ) ) Check = 1;
			else if ( STRCMP_L( RunStr + 15 + Offset , "(W)" ) ) Check = 2;
			else if ( STRCMP_L( RunStr + 15 + Offset , "(M)" ) ) Check = 3;
			else if ( STRCMP_L( RunStr + 15 + Offset , "(D)" ) ) Check = -1; // 2012.11.20
		}
	}
	//==============================================================================================================================================================================
	if ( !Transfer_Maint_Inf_Code_End( Index ) ) {
		if ( evt ) Event_Message_Reject( "TRANSFER SYSTEM is RUNNING ALREADY" );
		return 1;
	}
	//==============================================================================================================================================================================
	if ( Offset == 0 ) { // 2008.09.04
		//
		if ( Get_RunPrm_RUNNING_CLUSTER() ) { // 2006.08.30
			Transfer_Maint_Inf_Code_End_IO_Set( Index, SYS_ABORTED );
			if ( evt ) Event_Message_Reject( "AUTO SYSTEM is RUNNING ALREADY" );
			return 2;
		}
		//
	}
	//==============================================================================================================================================================================
	STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
	//
	if      ( STRNCMP_L( Else1Str , "PICK_ONLY_TM"	, 12 ) )	{ Mode = 1; Offset = 12; }
	else if ( STRNCMP_L( Else1Str , "PLACE_ONLY_TM"	, 13 ) )	{ Mode = 2; Offset = 13; }
	else if ( STRNCMP_L( Else1Str , "RETPICK_TM"	, 10 ) )	{ Mode = 3; Offset = 10; }
	else if ( STRNCMP_L( Else1Str , "RETPLACE_TM"	, 11 ) )	{ Mode = 4; Offset = 11; }
	else if ( STRNCMP_L( Else1Str , "EXTPICK_TM"	, 10 ) )	{ Mode = 5; Offset = 10; }
	else if ( STRNCMP_L( Else1Str , "EXTPLACE_TM"	, 11 ) )	{ Mode = 6; Offset = 11; }
	else if ( STRNCMP_L( Else1Str , "ROTATE_TM"		,  9 ) )	{ Mode = 7; Offset = 9; }
	else if ( STRNCMP_L( Else1Str , "EXTEND_TM"		,  9 ) )	{ Mode = 8; Offset = 9; }
	else if ( STRNCMP_L( Else1Str , "RETRACT_TM"	, 10 ) )	{ Mode = 9; Offset = 10; }
	else if ( STRNCMP_L( Else1Str , "UP_TM"			,  5 ) )	{ Mode = 10; Offset = 5; }
	else if ( STRNCMP_L( Else1Str , "DOWN_TM"		,  7 ) )	{ Mode = 11; Offset = 7; }
	else if ( STRNCMP_L( Else1Str , "HOME_TM"		,  7 ) )	{ Mode = 12; Offset = 7; }
	else if ( STRNCMP_L( Else1Str , "PICK_TM"		,  7 ) )	{ Mode = 13; Offset = 7; }
	else if ( STRNCMP_L( Else1Str , "PLACE_TM"		,  8 ) )	{ Mode = 14; Offset = 8; }
	else if ( STRNCMP_L( Else1Str , "SWAP_TM"		,  7 ) )	{ Mode = 15; Offset = 7; } // 2013.03.26
	//
	else if ( STRNCMP_L( Else1Str , "HOME_FM"       ,  7 ) )	{ Mode = 21; Offset = 7; }
	else if ( STRNCMP_L( Else1Str , "PICK_FM"       ,  7 ) )	{ Mode = 22; Offset = 7; }
	else if ( STRNCMP_L( Else1Str , "PLACE_FM"      ,  8 ) )	{ Mode = 23; Offset = 8; }
	else if ( STRNCMP_L( Else1Str , "ROTATE_FM"     ,  9 ) )	{ Mode = 24; Offset = 9; }

	//
	else if ( STRNCMP_L( Else1Str , "EXTEND_FM"		,  9 ) )	{ Mode = 25; Offset = 9; } // 2011.04.11
	else if ( STRNCMP_L( Else1Str , "RETRACT_FM"	, 10 ) )	{ Mode = 26; Offset = 10; } // 2011.04.11
	else if ( STRNCMP_L( Else1Str , "UP_FM"			,  5 ) )	{ Mode = 27; Offset = 5; } // 2011.04.11
	else if ( STRNCMP_L( Else1Str , "DOWN_FM"		,  7 ) )	{ Mode = 28; Offset = 7; } // 2011.04.11
	//

	else if ( STRNCMP_L( Else1Str , "PLCPICK_FALIC" , 13 ) )	{ Mode = 31; Offset = 13; }
	else if ( STRNCMP_L( Else1Str , "PICK_FALIC"    , 10 ) )	{ Mode = 32; Offset = 10; }
	else if ( STRNCMP_L( Else1Str , "PLACE_FALIC"   , 11 ) )	{ Mode = 33; Offset = 11; }
	else if ( STRNCMP_L( Else1Str , "PICKAL_FALIC"  , 12 ) )	{ Mode = 34; Offset = 12; }
	else if ( STRNCMP_L( Else1Str , "PLACEONLY_FALIC" , 15 ) )	{ Mode = 35; Offset = 15; }

	else if ( STRCMP_L( Else1Str , "MAPPING" ) )				{ Mode = 50; Offset = 7; }

	else if ( STRCMP_L( Else1Str , "WAITING_FOR_TM" ) )			{ Mode = 51; Offset = 14; }
	else if ( STRCMP_L( Else1Str , "WAITING_FOR_FM" ) )			{ Mode = 52; Offset = 14; }

	else if ( STRCMP_L( Else1Str , "PM_MOVE" ) )				{ Mode = 53; Offset = 7; }

	else														{ Mode = atoi( Else1Str ); Offset = 0; }
	//
	//==============================================================================================================================================================================
	//
	if ( ( Mode >= 1 ) && ( Mode <= 39 ) ) { // 2007.08.13 // 2011.04.11
		if ( Else1Str[Offset] == 0 ) {
			tmatm = 0;
		}
		else {
			tmatm = atoi( Else1Str + Offset ) - 1;
		}
	}
	else {
		tmatm = 0;
	}
	//
	//==============================================================================================================================================================================
	//
	STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
	TransferID = atoi( Else1Str );
	//
	//==============================================================================================================================================================================
	//
	STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
	Chamber = Get_Module_ID_From_String( 0 , Else1Str , -1 );
	if ( Chamber < 0 ) Mode = -1;
	//
	//==============================================================================================================================================================================
	//
	switch ( Mode ) {
	case 1 :
	case 2 :
	case 3 :
	case 4 :
	case 5 :
	case 6 :
	case 7 :
	case 8 :
	case 9 :
	case 10 :
	case 11 :
	case 12 :	// EQUIP_ROBOT_CONTROL_MAINT_INF
		//
	case 13 :	// EQUIP_PICK_FROM_CHAMBER_MAINT_INF
	case 14 :	// EQUIP_PLACE_TO_CHAMBER_MAINT_INF
	case 15 :	// EQUIP_SWAP_CHAMBER_MAINT_INF // 2013.01.11
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
		if      ( STRCMP_L( Else1Str , "A" ) )	Finger = 0;
		else if ( STRCMP_L( Else1Str , "B" ) )	Finger = 1;
		else if ( STRCMP_L( Else1Str , "C" ) )	Finger = 2;
		else if ( STRCMP_L( Else1Str , "D" ) )	Finger = 3;
		else									Mode = -1;
		//
		STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
		Slot = atoi( Else1Str );
		//
		if ( ( Slot < 0 ) || ( Slot > 9999 ) ) Mode = -1; // 2007.06.27
		//
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
		Depth = atoi( Else1Str );
		//
		if ( ( Depth < 0 ) || ( Depth > 9999 ) ) Mode = -1; // 2007.06.27
		//
		if ( ( Slot <= 0 ) && ( Depth <= 0 ) ) Mode = -1;
		//------------------------
		STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 ); // 2004.06.23
		Trorder = atoi( Else1Str );
		//------------------------
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 ); // 2006.11.16
		SrcCass = atoi( Else1Str ); // 2006.11.16
		break;
	case 21 :	// EQUIP_HOME_FROM_FMBM
	case 22 :	// EQUIP_PICK_FROM_FMBM
	case 23 :	// EQUIP_PLACE_TO_FMBM
	case 24 :	// EQUIP_ROBOT_FM_MOVE_CONTROL
		//
	case 25 :	// EXTEND
	case 26 :	// RETRACT
	case 27 :	// UP
	case 28 :	// DOWN
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
		ArmASlot = atoi( Else1Str );
		//
		if ( ( ArmASlot < 0 ) || ( ArmASlot > MAX_CASSETTE_SLOT_SIZE ) ) Mode = -1;
		//
		STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
		ArmBSlot = atoi( Else1Str );
		if ( ( ArmBSlot < 0 ) || ( ArmBSlot > MAX_CASSETTE_SLOT_SIZE ) ) Mode = -1;
		//
		if ( ( ArmASlot <= 0 ) && ( ArmBSlot <= 0 ) ) Mode = -1;
		//
//		if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) Mode = -1; // 2014.09.04
		//------------------------
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 ); // 2004.06.23
		Trorder = atoi( Else1Str );
		break;
		//
	case 31 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PLACE_MDL_PICK
	case 32 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PICK
	case 33 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PLACE_MDL
	case 34 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_MDL_PICK
	case 35 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PLACE
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
		ArmASlot = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
		ArmBSlot = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
		SrcCass = atoi( Else1Str );
		//
		if ( STRNCMP_L( Else3Str , "BM" , 2 ) ) {
			BMChamber = atoi( Else3Str + 2 ) - 1 + BM1;
			if ( ( BMChamber < BM1 ) || ( ( BMChamber - BM1 + 1 ) > MAX_BM_CHAMBER_DEPTH ) ) BMChamber = -1;
		}
		else {
			BMChamber = -1;
		}
		//
		if ( ( ArmASlot <= 0 ) && ( ArmBSlot <= 0 ) ) Mode = -1;
		//
		//================================================================
		// 2007.08.29
		//================================================================
		if ( ( ArmBSlot > 0 ) && ( BMChamber > 0 ) ) {
			BMChamber = ( BMChamber * 10000 ) + BMChamber;
		}
		//================================================================
		//
		if ( ( Chamber != AL ) && ( Chamber != IC ) ) Mode = -1;
		//
		if ( ( SrcCass < 0 ) && ( SrcCass > MAX_CASSETTE_SIDE ) ) Mode = -1;
		break;
		//-----------------------------------------------------------------------------------------------------------------------------------------
	case 50 :	// EQUIP_CARRIER_MAPPING
		if ( Chamber <  CM1 ) Mode = -1;
		if ( Chamber >= PM1 ) Mode = -1;
		break;
	case 51 :
	case 52 :	// EQUIP_RUN_WAITING_BM
		if ( Chamber <  BM1 ) Mode = -1;
		if ( Chamber >= TM  ) Mode = -1;

		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );	// 2013.01.29
		SrcCass = atoi( Else1Str );	// 2013.01.29

		break;
	case 53 :	// EQUIP_PM_MOVE_CONTROL
		// 2003.10.28
		break;
		//-----------------------------------------------------------------------------------------------------------------------------------------
	default :
		Mode = -1;
		break;
	}
	//
	//==============================================================================================================================================================================
	if ( Mode == -1 ) { // 2014.09.04
		Transfer_Maint_Inf_Code_End_IO_Set( Index, SYS_ABORTED );
		if ( evt ) Event_Message_Reject( "Data Failed" );
		return 3;
	}
	//==============================================================================================================================================================================
	//
	Transfer_Maint_Inf_Code_Interface_Data_Setting( Check , Index , tmatm , Chamber , Finger , Slot , Depth , ArmASlot , ArmBSlot , SrcCass , BMChamber , TransferID , Trorder );
	//---------------------------------------------------
	_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , AutoUse + 1 ); // 2012.07.28
	//---------------------------------------------------
	if ( _beginthread( (void *) Transfer_Maint_Inf_Code_for_Low_Level , 0 , (void *) ( ( Index * 100 ) + Mode ) ) == -1 ) {
		//----------------------------------------------------------------------------------------------------------------
		// 2004.08.18
		//----------------------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "THREAD FAIL Transfer_Maint_Inf_Code_for_Low_Level(3) %d\n" , -1 );
		//----------------------------------------------------------------------------------------------------------------
		Transfer_Maint_Inf_Code_End_IO_Set( Index , SYS_ABORTED ); // 2006.08.30
		if ( Transfer_Maint_Inf_Code_End( -1 ) ) _i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 0 ); // 2006.08.30
		//----------------------------------------------------------------------------------------------------------------
		return 3;
		//
	}
	//==============================================================================================================================================================================
	if ( !evt ) {
		for ( Offset = 0 ; Offset < 100 ; Offset++ ) {
			if ( !Transfer_Maint_Inf_Code_End( Index ) ) break;
			Sleep(1);
		}
	}
	//==============================================================================================================================================================================
	return 0;
}
//


//int _i_SCH_SUB_SET_PROCESSING( int Side0 , int ch , char *rcpname , int skiplotpreside ) { // 2017.09.19 // 2018.06.22
int _i_SCH_SUB_SET_PROCESSING( int Side0 , int ch , char *rcpname , int skiplotpreside , int mode ) { // 2017.09.19 // 2018.06.22
	//
	int Side , runside , i , Res;
//	char Recipe[256]; // 2019.05.30
	char Recipe[2048]; // 2019.05.30
	//
	Res = 0;
	//
	if ( ( Side0 >= 0 ) && ( Side0 < MAX_CASSETTE_SIDE ) ) {
		//
		if ( _i_SCH_SYSTEM_USING_RUNNING( Side0 ) ) {
			Side = Side0;
		}
		else {
			Side = -1;
		}
		//
	}
	else {
		Side = -1;
	}
	//
	if ( Side == -1 ) {
		//
		runside = -1;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) {
					//
					runside = i;
					//
					if ( !_i_SCH_MODULE_Chk_TM_Free_Status( i ) ) {
						//
						Side = i;
						//
						break;
					}
				}
			}
		}
	}
	//
	if ( ( rcpname == NULL ) || ( rcpname[0] == 0 ) ) {
		//
		Res = -1;
		//
		if ( ( Side >= 0 ) && ( Side < MAX_CASSETTE_SIDE ) ) {
			if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( Side , ch ) != 0 ) {
				//
				strcpy( Recipe , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( Side , ch ) );
				//
				Res = 0;
			}
		}
		//
		if ( Res == -1 ) {
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				//
				if ( _i_SCH_SYSTEM_USING_RUNNING( i ) ) {
					//
					if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( i , ch ) == MUF_01_USE ) {
						//
						if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( i , ch ) != 0 ) {
							//
							if ( Res < 0 ) {
								//
								Res = i;
								//
								strcpy( Recipe , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , ch ) );
								//
							}
							else {
								if ( _i_SCH_SYSTEM_RUNORDER_GET( Res ) < _i_SCH_SYSTEM_RUNORDER_GET( i ) ) {
									//
									Res = i;
									//
									strcpy( Recipe , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( i , ch ) );
									//
								}
							}
							//
						}
					}
				}
			}
			//
		}
		//
		if ( Res == -1 ) strcpy( Recipe , "?" );
		//
	}
	else {
		strcpy( Recipe , rcpname );
	}
	//
	Res = 0;
	//
	if ( Side == -1 ) {
		//
		Side = ( runside == -1 ) ? 0 : runside;
		//
		for ( i = 0 ; i < 2 ; i++ ) {
			//
			if ( EQUIP_STATUS_PROCESS( Side , ch ) == SYS_RUNNING ) {
				//
				_i_SCH_LOG_LOT_PRINTF( Side , "PM SET Process Append at %s[%s]%cPROCESS_SET_START PM%d:::%s:::103\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Recipe , 9 , ch - PM1 + 1 , Recipe );
				//
				if ( !_i_SCH_EQ_SPAWN_EVENT_PROCESS( Side , -1 , ch , 0 , 0 , 0 , Recipe , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) continue;
				//
				if ( skiplotpreside != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( skiplotpreside , ch );
				//
//				Scheduler_PreCheck_for_ProcessRunning_Part( Side , ch ); // 2018.06.22
				Scheduler_PreCheck_for_ProcessRunning_Part( Side , ch , Recipe , mode ); // 2018.06.22
				//
				Res = 1;
				//
				break;
			}
			else {
				//
				_i_SCH_LOG_LOT_PRINTF( Side , "PM SET Process Start at %s[%s]%cPROCESS_SET_START PM%d:::%s:::104\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Recipe , 9 , ch - PM1 + 1 , Recipe );
				//
				if ( !_i_SCH_EQ_SPAWN_PROCESS( Side , -1 , ch , 0 , 0 , 0 , Recipe , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) continue;
				//
				if ( skiplotpreside != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( skiplotpreside , ch );
				//
//				Scheduler_PreCheck_for_ProcessRunning_Part( Side , ch ); // 2018.06.22
				Scheduler_PreCheck_for_ProcessRunning_Part( Side , ch , Recipe, mode ); // 2018.06.22
				//
				Res = 2;
				//
				break;
			}
			//
		}
	}
	else {
		//
		_i_SCH_LOG_LOT_PRINTF( Side , "PM SET Process Regist at %s[%s]%cPROCESS_SET_REGIST PM%d:::%s:::%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , Recipe , 9 , ch - PM1 + 1 , Recipe , skiplotpreside );
		//
//		SIGNAL_MODE_FMTMSIDE_SET( ch , Side , Recipe , skiplotpreside ); // 2018.06.22
		SIGNAL_MODE_FMTMSIDE_SET( ch , Side , Recipe , skiplotpreside , mode ); // 2018.06.22
		//
	}
	//
	return 0;
	//
}


void Event_Message_Received() {
	int	 INDEX , id , ENDINDEX , SS , ES , LC , ResSC , REG_ID , EVSTR_POINTER , JOBRUN , MaxSlot , maxunuse , RESTARTMODE , MOVEMODE; // , msc;
//	char RunStr[256] , ElseStr[256] , Else1Str[256] , Else2Str[256] , JobStr[256] , LotStr[256] , MidStr[256] , DispStr[256]; // 2007.01.04
//	char RunStr[256] , ElseStr[513] , Else1Str[513] , Else2Str[513] , Else3Str[513] , JobStr[256] , LotStr[256] , MidStr[256] , DispStr[256]; // 2007.01.04
	char RunStr[256] , ElseStr[513] , Else1Str[513] , Else2Str[513] , JobStr[256] , LotStr[256] , MidStr[256] , DispStr[256]; // 2007.01.04
	int  FA_RUN;
	BOOL QA_RUN;
	int  QA_RUN2;
	Scheduling_Regist	Scheduler_Reg;
//	int tmatm;
	int Chamber;
//	int Finger;
	int Slot;
	int Depth;
//	int ArmASlot;
//	int ArmBSlot;
	int SrcCass;
//	int BMChamber;
	int TransferID;
//	int Trorder; // 2004.06.23
	long TimeCount; // 2013.06.21
	//
	int LOADUNLOAD_MODE; // 2013.07.10

	_IO_CONSOLE_PRINTF( "[EVENT]\t%s\n" , PROGRAM_EVENT_READ() ); // 2003.10.28

	GUI_Receive_Event_Inserting( PROGRAM_EVENT_READ() );

	STR_SEPERATE( PROGRAM_EVENT_READ() , RunStr , ElseStr , 255 );
	
	FA_RUN = 0;
	INDEX = -1;

	strcpy( Else1Str , "" ); // 2011.03.08
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	switch( _SCH_COMMON_EVENT_PRE_ANALYSIS( PROGRAM_EVENT_READ() , RunStr , ElseStr , Else1Str ) ) { // 2011.03.08
	case SYS_SUCCESS : 
		if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		return;
		break;
	case SYS_ABORTED : 
		if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
		PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
		return;
		break;
	case SYS_ERROR : 
		if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
		PROGRAM_EVENT_STATUS_SET( SYS_ERROR );
		return;
		break;
	case 1 :
		break;
	case 0 :
		if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
		return;
		break;
	default :
		Event_Message_Reject( "CAN NOT ANALYSIS MESSAGE" );
		return;
		break;
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	if      ( STRNCMP_L( RunStr , "EVENT_ON" , 8 ) ) {
		EVSTR_POINTER = strlen( RunStr );
		while ( TRUE ) {
			if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == 0   ) break;
			if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == ' ' ) EVSTR_POINTER++;
			else break;
		}
		FA_SEND_MESSAGE_TO_SERVER( 0 , -1 , -1 , PROGRAM_EVENT_READ() + EVSTR_POINTER );
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "PAUSE" , 5 ) ) {
//		if      ( STRCMP_L( RunStr + 5 , "ALL" ) ) { // 2011.11.14 // 2011.12.29
		if      ( STRNCMP_L( RunStr + 5 , "ALL" , 3 ) ) { // 2011.11.14 // 2011.12.29 // 2012.06.18
			//
			FA_RUN = 0; // 2012.06.18
			if      ( STRNCMP_L( RunStr + 8 , "_F" , 2 ) ) { // 2012.06.18
				FA_RUN = 1;
			}
			else if ( STRNCMP_L( RunStr + 8 , "(F)" , 3 ) ) { // 2012.06.18
				FA_RUN = 2;
			}
			//
			for ( INDEX = 0 ; INDEX < MAX_CASSETTE_SIDE ; INDEX++ ) {
				//
				if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) continue;
				//
				_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_PAUSING );
				//
				_i_SCH_SYSTEM_PAUSE_SET( INDEX , 2 );
				//
				if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_PAUSE , 0 ); // 2012.06.18
				//
				_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Pause(A)%cSCTLPAUSEA\n" , 9 ); // 2012.06.18
				//
			}
			//
			return; // 2012.01.17
		}
		else if ( STRCMP_L( RunStr + 5 , "SW" ) ) { // 2004.06.29
			if ( Get_RunPrm_RUNNING_CLUSTER() ) {
				IO_SET_PAUSE_STATUS( 1 );
				_i_SCH_SYSTEM_PAUSE_SW_SET( 1 );
			}
			return;
		}
		else if ( STRCMP_L( RunStr + 5 , "TR" ) ) {
			if ( Get_RunPrm_RUNNING_TRANSFER() ) {
				BUTTON_SET_TR_CONTROL( CTC_PAUSING );
				_i_SCH_SYSTEM_PAUSE_SET( TR_CHECKING_SIDE , 1 );
			}
			else {
				Event_Message_Reject( "TRANSFER SYSTEM IS NOT RUNNING" );
			}
			return;
		}
		else if ( STRCMP_L( RunStr + 5 , "TR_F" ) ) {
			if ( Get_RunPrm_RUNNING_TRANSFER() ) {
				BUTTON_SET_TR_CONTROL( CTC_PAUSING );
				_i_SCH_SYSTEM_PAUSE_SET( TR_CHECKING_SIDE , 1 );
				FA_ACCEPT_MESSAGE( TR_CHECKING_SIDE , FA_TRANSFER_PAUSE , 0 );
			}
			else {
				Event_Message_Reject( "TRANSFER SYSTEM IS NOT RUNNING" );
				FA_REJECT_MESSAGE( TR_CHECKING_SIDE , FA_TRANSFER_PAUSE , 0 , "" );
			}
			return;
		}
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 5 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_PAUSE , 0 , "" );
		}
		else {
			_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_PAUSING );
			_i_SCH_SYSTEM_PAUSE_SET( INDEX , 1 );
			_i_SCH_SYSTEM_MODE_RESUME_SET( INDEX , FALSE );
			if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_PAUSE , 0 );
			//
			_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Pause%cSCTLPAUSE\n" , 9 ); // 2004.03.17
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "CONTINUE" , 8 ) ) {
//		if      ( STRCMP_L( RunStr + 8 , "ALL" ) ) { // 2011.11.14
		if      ( STRNCMP_L( RunStr + 8 , "ALL" , 3 ) ) { // 2011.11.14 // 2012.06.18
			//
			FA_RUN = 0; // 2012.06.18
			if      ( STRNCMP_L( RunStr + 11 , "_F" , 2 ) ) { // 2012.06.18
				FA_RUN = 1;
			}
			else if ( STRNCMP_L( RunStr + 11 , "(F)" , 3 ) ) { // 2012.06.18
				FA_RUN = 2;
			}
			//
			for ( INDEX = 0 ; INDEX < MAX_CASSETTE_SIDE ; INDEX++ ) {
				//
				if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) continue;
				//
				_i_SCH_SYSTEM_PAUSE_SET( INDEX , 0 );
				//
				if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) {
					_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_RUNNING );
				}
				else {
					if ( _i_SCH_SYSTEM_RUN_TAG_GET( INDEX ) > 0 ) {
						_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_RUNNING );
					}
					else {
						_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_WAITING );
					}
					//--------------------------------
				}
				//
				if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 ); // 2012.06.18
				//
				_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Continue%cSCTLCONTINUE\n" , 9 ); // 2012.06.18
				//
			}
			//
			return; // 2012.01.17
		}
		else if ( STRCMP_L( RunStr + 8 , "SW" ) ) { // 2004.06.29
			IO_SET_PAUSE_STATUS( 0 );
			_i_SCH_SYSTEM_PAUSE_SW_SET( 0 );
			return;
		}
		else if ( STRCMP_L( RunStr + 8 , "TR" ) ) {
			if ( Get_RunPrm_RUNNING_TRANSFER() ) {
				_i_SCH_SYSTEM_PAUSE_SET( TR_CHECKING_SIDE , 0 );
			}
			else {
				Event_Message_Reject( "TRANSFER SYSTEM IS NOT RUNNING" );
			}
			return;
		}
		else if ( STRCMP_L( RunStr + 8 , "TR_F" ) ) {
			if ( Get_RunPrm_RUNNING_TRANSFER() ) {
				_i_SCH_SYSTEM_PAUSE_SET( TR_CHECKING_SIDE , 0 );
				FA_ACCEPT_MESSAGE( TR_CHECKING_SIDE , FA_TRANSFER_RESUME , 0 );
			}
			else {
				Event_Message_Reject( "TRANSFER SYSTEM IS NOT RUNNING" );
				FA_REJECT_MESSAGE( TR_CHECKING_SIDE , FA_TRANSFER_RESUME , 0 , "" );
			}
			return;
		}
		//===================================================================================
		switch( Event_Get_Side_Data( RunStr + 8 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_CONTINUE , 0 , "" );
		}
		else {
			_i_SCH_SYSTEM_PAUSE_SET( INDEX , 0 );
			//----------- Append 2001.10.12
//			if ( _i_SCH_SYSTEM_USING_GET( INDEX ) >= 10 ) { // 2005.07.29
			if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) { // 2005.07.29
				_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_RUNNING );
			}
			else {
				//--------------------------------
				// 2011.04.22
				//--------------------------------
				//_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_WAITING );
				//
				if ( _i_SCH_SYSTEM_RUN_TAG_GET( INDEX ) > 0 ) {
					_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_RUNNING );
				}
				else {
					_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_WAITING );
				}
				//--------------------------------
			}
			//-----------
			if ( _i_SCH_SYSTEM_LASTING_GET( INDEX ) ) { // 2007.09.18
				if ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 0 ) {
					//
					if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 );
					//
					_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Continue%cSCTLCONTINUE\n" , 9 ); // 2004.03.17
					//
				}
				else { // 2009.01.09
					if ( ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 2 ) || ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 4 ) || ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 6 ) ) {
						_i_SCH_IO_SET_END_BUTTON( INDEX , CTCE_RUNNING );
						_i_SCH_SYSTEM_MODE_RESUME_SET( INDEX , TRUE );
						//
						if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 );
						//
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Continue%cSCTLCONTINUE\n" , 9 ); // 2004.03.17
						//
					}
				}
			}
			else {
				if ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) != 0 ) {
					_i_SCH_IO_SET_END_BUTTON( INDEX , CTCE_RUNNING );
					_i_SCH_SYSTEM_MODE_RESUME_SET( INDEX , TRUE );
				}
				//
				if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 );
				//
				_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Continue%cSCTLCONTINUE\n" , 9 ); // 2004.03.17
				//
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "CANCEL" , 6 ) ) {
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 6 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//---------------------------
/*
// 2011.07.21
		if ( SCHEDULER_CM_LOCKED_for_HANDOFF( INDEX + CM1 ) > 0 ) {
			Event_Message_Reject( "CM USING Currently" );
			return; // 2011.03.18
		}
*/
		//---------------------------
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) < 10 ) ) {
			switch( _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT() ) {
			case 0 :
			case 1 :
				//=====================================================================
				//2002.02.05
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
				}
				else {
					SCHMULTI_CASSETTE_VERIFY_CLEAR( INDEX , FALSE );
				}
				//=====================================================================
				if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CANCEL , 0 );
				_i_SCH_SYSTEM_USING_SET( INDEX , 100 );
				//
				_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Cancel%cSCTLCANCEL\n" , 9 ); // 2004.03.17
				//
				break;
			default :
				Event_Message_Reject( "CAN NOT CACEL WHEN RUNNING" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
				return;
				break;
			}
			//
			LOADUNLOAD_MODE = 1; // 2013.07.10
			//
		}
//		else if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) { // 2013.06.08
		else { // 2013.06.08
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 1 ) { // 2013.06.08
				if ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) {
					Event_Message_Reject( "AUTO SYSTEM CAN NOT CANCEL" );
					if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
					return;
				}
				//
				switch( _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT() ) {
				case 1 :
				case 3 :
					if ( SYSTEM_CANCEL_DISABLE_GET( INDEX ) || Get_RunPrm_MODULE_HAS_WAFER() ) {
						Event_Message_Reject( "CAN NOT CACEL METHOD IS NOT FREE" );
						if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
						return;
					}
					break;
				}
				//
				LOADUNLOAD_MODE = 1;
				//
			}
			else {
				if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) { // 2013.07.10
					LOADUNLOAD_MODE = 1;
				}
				else {
					LOADUNLOAD_MODE = 1;
				}
			}
			//
			/*
			//
			// 2018.04.10
			//
			if ( FA_RUN == 1 ) {
				//---------------------------
				// Append 2001.12.17
				if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
					Event_Message_Reject( "LOADING IS RUNNING" );
					FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
					return;
				}
				if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
					Event_Message_Reject( "UNLOADING IS RUNNING" );
					FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
					return;
				}
				//---------------------------
				if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
					if ( FA_UNLOADING_SIGNAL_GET( INDEX ) ) {
						Event_Message_Reject( "UNLOAD PART RUNNING ALREADY" );
						FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
						return;
					}
					if ( !FA_LOADING_SIGNAL_GET( INDEX ) ) {
						Event_Message_Reject( "LOADING SIGNAL IS NOT PREPARED" );
						FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
						return;
					}
				}
			}
			else {
				//---------------------------
				// Append 2001.12.17
				if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
					Event_Message_Reject( "LOADING IS RUNNING" );
					if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
					return;
				}
				if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
					Event_Message_Reject( "UNLOADING IS RUNNING" );
					if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
					return;
				}
			}
			//
			*/
			//
			//
			// 2018.04.10
			//
			if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
				Event_Message_Reject( "UNLOADING IS RUNNING" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
				return;
			}
			//
			TimeCount = GetTickCount();
			//
			while ( TRUE ) {
				//
				if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) <= 0 ) {
					break;
				}
				//
				if ( ( GetTickCount() - TimeCount ) >= 5000 ) break;
				//
				Sleep(1);
				//
			}
			//
			if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
				Event_Message_Reject( "LOADING IS RUNNING" );
				if ( FA_RUN != 2 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
				return;
			}
			//
			if ( FA_RUN == 1 ) {
				//
				if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
					//
					if ( FA_UNLOADING_SIGNAL_GET( INDEX ) ) {
						Event_Message_Reject( "UNLOAD PART RUNNING ALREADY" );
						FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
						return;
					}
					if ( !FA_LOADING_SIGNAL_GET( INDEX ) ) {
						Event_Message_Reject( "LOADING SIGNAL IS NOT PREPARED" );
						FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
						return;
					}
					//
				}
				//
			}
			//
			//
			//
			//
			//
			//--------------------------------------------------------------
			if ( !SCH_MR_UNLOAD_LOCK_START( INDEX ) ) { // 2012.04.01
				Event_Message_Reject( "CAN NOT CACEL CM_LOCKED CURRENT_CR_LOCKING" );
				if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
			}
			//--------------------------------------------------------------
			if ( SCH_MR_UNLOAD_LOCK_GET_PICKPLACE( INDEX ) ) { // 2012.04.01
				Event_Message_Reject( "CAN NOT CACEL CM_LOCKED CURRENT_PICK_PLACING" );
				if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
			}
			else {
				if ( SCHEDULER_CM_LOCKED_CHECK_FOR_UNLOAD( INDEX + CM1 ) > 0 ) { // 2012.04.01
					Event_Message_Reject( "CAN NOT CACEL CM_LOCKED NOT FREE" );
					if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" );
				}
				else {
					if ( FA_RUN == 1 ) {
						//---------------------------
						if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
							//---------------------------
							// 2008.03.18
							//---------------------------
							//
							SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , 10 ); // 2011.09.07
							//
							FA_MAPPING_CANCEL_SET( INDEX , 1 );
							if ( FA_MAPPING_SIGNAL_GET( INDEX ) == 2 ) Sleep(100);
							//---------------------------
							//=====================================================================
							//2002.02.05
							SCHMULTI_CASSETTE_VERIFY_CLEAR( INDEX , FALSE );
							//=====================================================================
							FA_ACCEPT_MESSAGE( INDEX , FA_CANCEL , 0 );
							FA_RESULT_MESSAGE( INDEX , FA_CANCELED , 0 );
							//
//							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
							//
							switch( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ) { // 2004.02.04
							case 4 :
							case 5 :
							case 6 :
//								FA_UNLOAD_CONTROLLER( INDEX , FALSE , TRUE , FALSE , TRUE , TRUE ); // 2004.02.04 // 2016.10.31
								FA_UNLOAD_CONTROLLER( INDEX , FALSE , TRUE , FALSE , TRUE , 1 ); // 2004.02.04 // 2016.10.31
								break;
							default :
//								FA_UNLOAD_CONTROLLER( INDEX , TRUE , TRUE , FALSE , TRUE , TRUE ); // 2016.10.31
								FA_UNLOAD_CONTROLLER( INDEX , TRUE , TRUE , FALSE , TRUE , 1 ); // 2016.10.31
								break;
							}
						}
						else {
							//=====================================================================
							//2002.02.05
							SCHMULTI_CASSETTE_VERIFY_CLEAR( INDEX , FALSE );
							//=====================================================================
							FA_ACCEPT_MESSAGE( INDEX , FA_CANCEL , 0 );
							FA_RESULT_MESSAGE( INDEX , FA_CANCELED , 0 );
							//
						}
					}
					else {
						//---------------------------
						// 2008.03.18
						//---------------------------
						//
						SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , 10 ); // 2011.09.07
						//
						FA_MAPPING_CANCEL_SET( INDEX , 1 );
						//
						if ( FA_MAPPING_SIGNAL_GET( INDEX ) == 2 ) Sleep(100);
						//---------------------------
						//=====================================================================
						//2002.02.05
						SCHMULTI_CASSETTE_VERIFY_CLEAR( INDEX , FALSE );
						//=====================================================================
						QA_RUN = FALSE;
						//
						if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) { // 2013.06.08
							if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
								if ( FA_UNLOADING_SIGNAL_GET( INDEX ) ) {
									QA_RUN = TRUE;
								}
								if ( !FA_LOADING_SIGNAL_GET( INDEX ) ) {
									QA_RUN = TRUE;
								}
							}
						}
						//
						if ( FA_RUN == 2 ) {
							if ( !QA_RUN ) {
								FA_ACCEPT_MESSAGE( INDEX , FA_CANCEL , 0 );
								FA_RESULT_MESSAGE( INDEX , FA_CANCELED , 0 );
							}
						}
						switch( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() ) { // 2004.02.04
						case 2 :
						case 5 :
//							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
//							FA_UNLOAD_CONTROLLER( INDEX , FALSE , TRUE , QA_RUN , TRUE , TRUE ); // 2016.10.31
							FA_UNLOAD_CONTROLLER( INDEX , FALSE , TRUE , QA_RUN , TRUE , 1 ); // 2016.10.31
							break;
						case 3 :
						case 6 :
//							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
//							FA_UNLOAD_CONTROLLER( INDEX , TRUE , TRUE , QA_RUN , TRUE , TRUE ); // 2016.10.31
							FA_UNLOAD_CONTROLLER( INDEX , TRUE , TRUE , QA_RUN , TRUE , 1 ); // 2016.10.31
							break;
						default :
//							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2012.04.03 // 2013.07.10
							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2012.04.03 // 2013.07.10
//							MANUAL_UNLOAD_CONTROLLER( INDEX+1 , -1 , TRUE , QA_RUN , TRUE , TRUE ); // 2012.07.10
//							MANUAL_UNLOAD_CONTROLLER( INDEX+1 , TRUE , QA_RUN , TRUE , TRUE ); // 2012.07.10 // 2016.10.31
							MANUAL_UNLOAD_CONTROLLER( INDEX+1 , TRUE , QA_RUN , TRUE , 1 ); // 2012.07.10 // 2016.10.31
							break;
						}
					}
				}
			}
			//--------------------------------------------------------------
			SCH_MR_UNLOAD_LOCK_END( INDEX ); // 2012.04.01
			//--------------------------------------------------------------
		}
//		else { // 2013.06.08
//			Event_Message_Reject( "AUTO SYSTEM CAN NOT CANCEL" ); // 2013.06.08
//			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_CANCEL , 0 , "" ); // 2013.06.08
//		} // 2013.06.08
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "START" , 5 ) ) {
		if      ( STRNCMP_L( RunStr + 5 , "_TR_PICK" , 8 ) ) {
			INDEX = TR_PICK;		ENDINDEX = FA_TRANSFER_PICK;		LC = 13;
		}
		else if ( STRNCMP_L( RunStr + 5 , "_TR_PLACE" , 9 ) ) {
			INDEX = TR_PLACE;		ENDINDEX = FA_TRANSFER_PLACE;		LC = 14;
		}
		else if ( STRNCMP_L( RunStr + 5 , "_TR_MOVE" , 8 ) ) {
			INDEX = TR_MOVE;		ENDINDEX = FA_TRANSFER_MOVE;		LC = 13;
		}
		else if ( STRNCMP_L( RunStr + 5 , "_TR_CLEAR" , 9 ) ) {
			INDEX = TR_CLEAR;		ENDINDEX = FA_TRANSFER_CLEAR;		LC = 14;
		}
		else {
			INDEX = -1;
		}
		//=================================================================================
		if ( INDEX != -1 ) {
			if ( STRCMP_L( RunStr + LC , "_F" ) ) {
				FA_RUN = 1000;
			}
			else {
				FA_RUN = 0;
			}
			if ( !Get_RunPrm_RUNNING_CLUSTER() && !Get_RunPrm_RUNNING_TRANSFER() ) {
				//
				_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 1 );
				//
				if ( FA_RUN == 1000 ) FA_ACCEPT_MESSAGE( TR_CHECKING_SIDE , ENDINDEX , 0 );
				if ( _beginthread( (void *) Transfer_User_Code , 0 , (void *) ( INDEX + FA_RUN ) ) == -1 ) {
					//----------------------------------------------------------------------------------------------------------------
					// 2004.08.18
					//----------------------------------------------------------------------------------------------------------------
					_IO_CIM_PRINTF( "THREAD FAIL Transfer_User_Code(1) %d\n" , INDEX + FA_RUN );
					//
					_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 0 );
					if ( FA_RUN == 1000 ) FA_REJECT_MESSAGE( TR_CHECKING_SIDE , ENDINDEX , 0 , "" );
					//----------------------------------------------------------------------------------------------------------------
				}
			}
			else {
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				if ( FA_RUN == 1000 ) FA_REJECT_MESSAGE( TR_CHECKING_SIDE , ENDINDEX , 0 , "" );
			}
			return;
		}

		//============================================================================
		// 2006.06.27
		//============================================================================
		LC = strlen( RunStr + 5 );

		if ( STRCMP_L( RunStr + ( 5 + LC - 3 ) , "(F)" ) ) {
			FA_RUN = 2;
			LC = LC - 3;
		}
		else {
			FA_RUN = 0;
		}
		//
		INDEX = 0;
		ENDINDEX = 0;
		SS = 0;
		ES = 0;
		//
		JOBRUN = 0;
		//
		RESTARTMODE = 0; // 2011.09.23
		//
		MOVEMODE = 0; // 2013.09.03
		//
		for ( id = 0 ; id < LC ; id++ ) {
			/*
			// 2012.07.23
			if      ( tolower( RunStr[5+id] ) == '1' ) INDEX = 0;
			else if ( tolower( RunStr[5+id] ) == '2' ) INDEX = 1;
			else if ( tolower( RunStr[5+id] ) == '3' ) INDEX = 2;
			else if ( tolower( RunStr[5+id] ) == '4' ) INDEX = 3;
			else if ( tolower( RunStr[5+id] ) == '5' ) INDEX = 4;
			else if ( tolower( RunStr[5+id] ) == '6' ) INDEX = 5;
			else if ( tolower( RunStr[5+id] ) == '7' ) INDEX = 6;
			else if ( tolower( RunStr[5+id] ) == '8' ) INDEX = 7;
			*/
			//
			// 2012.07.23
			if      ( tolower( RunStr[5+id] ) == '1' ) INDEX = ( INDEX * 10 ) + 1;
			else if ( tolower( RunStr[5+id] ) == '2' ) INDEX = ( INDEX * 10 ) + 2;
			else if ( tolower( RunStr[5+id] ) == '3' ) INDEX = ( INDEX * 10 ) + 3;
			else if ( tolower( RunStr[5+id] ) == '4' ) INDEX = ( INDEX * 10 ) + 4;
			else if ( tolower( RunStr[5+id] ) == '5' ) INDEX = ( INDEX * 10 ) + 5;
			else if ( tolower( RunStr[5+id] ) == '6' ) INDEX = ( INDEX * 10 ) + 6;
			else if ( tolower( RunStr[5+id] ) == '7' ) INDEX = ( INDEX * 10 ) + 7;
			else if ( tolower( RunStr[5+id] ) == '8' ) INDEX = ( INDEX * 10 ) + 8;
			else if ( tolower( RunStr[5+id] ) == '9' ) INDEX = ( INDEX * 10 ) + 9;
			else if ( tolower( RunStr[5+id] ) == '0' ) {
				if ( INDEX <= 0 ) {
					INDEX = -1;
					break;
				}
				else {
					INDEX = ( INDEX * 10 ) + 0;
				}
			}
			//
			else if ( tolower( RunStr[5+id] ) == 'p' ) ENDINDEX = 100;
			else if ( tolower( RunStr[5+id] ) == 's' ) ENDINDEX = 200;
			else if ( tolower( RunStr[5+id] ) == 'o' ) ENDINDEX = 300;
			//
			else if ( tolower( RunStr[5+id] ) == 'j' ) JOBRUN = 1; // 2008.04.23
			else if ( tolower( RunStr[5+id] ) == 'm' ) JOBRUN = 2; // 2008.04.23
			//
			else if ( tolower( RunStr[5+id] ) == 'e' ) SS = 10;
			else if ( tolower( RunStr[5+id] ) == 'x' ) SS = 20; // 2008.12.17
			//
			else if ( tolower( RunStr[5+id] ) == 'n' ) ES = 1000;
			else if ( tolower( RunStr[5+id] ) == 'l' ) ES = 2000; // 2017.10.10
			//
			else if ( tolower( RunStr[5+id] ) == 'r' ) RESTARTMODE = 1;
			else if ( tolower( RunStr[5+id] ) == 't' ) RESTARTMODE = 2;
			else if ( tolower( RunStr[5+id] ) == 'a' ) RESTARTMODE = 3;
			//
			else if ( tolower( RunStr[5+id] ) == 'v' ) MOVEMODE = 1; // 2013.09.03
			else if ( tolower( RunStr[5+id] ) == 'z' ) MOVEMODE = 2; // 2013.09.03
			//
			else {
				INDEX = -1;
				break;
			}
		}
		//============================================================================
		if ( INDEX == 0 ) { // 2012.07.23
			INDEX = INDEX + ENDINDEX + SS + ES;
		}
		else if ( INDEX > 0 ) { // 2012.07.23
//		if ( INDEX != -1 ) { // 2012.07.23
//			INDEX = INDEX + ENDINDEX + SS + ES; // 2012.07.23
			INDEX = ( INDEX - 1 ) + ENDINDEX + SS + ES; // 2012.07.23
		}
		//============================================================================
		if (
			STRCMP_L( RunStr + 5 , "_F" ) ||
			STRCMP_L( RunStr + 5 , "P_F" ) ||
			STRCMP_L( RunStr + 5 , "S_F" ) ||
			STRCMP_L( RunStr + 5 , "O_F" ) ||
			STRCMP_L( RunStr + 5 , "PE_F" ) ||
			STRCMP_L( RunStr + 5 , "SE_F" ) ||
			STRCMP_L( RunStr + 5 , "OE_F" ) ||
			STRCMP_L( RunStr + 5 , "N_F" ) ||
			STRCMP_L( RunStr + 5 , "NP_F" ) ||
			STRCMP_L( RunStr + 5 , "NS_F" ) ||
			STRCMP_L( RunStr + 5 , "NO_F" ) ||
			STRCMP_L( RunStr + 5 , "NPE_F" ) ||
			STRCMP_L( RunStr + 5 , "NOE_F" ) ||
			STRCMP_L( RunStr + 5 , "NSE_F" ) ||
			STRCMP_L( RunStr + 5 , "L_F" ) || // 2017.10.10
			STRCMP_L( RunStr + 5 , "LP_F" ) ||
			STRCMP_L( RunStr + 5 , "LS_F" ) ||
			STRCMP_L( RunStr + 5 , "LO_F" ) ||
			STRCMP_L( RunStr + 5 , "LPE_F" ) ||
			STRCMP_L( RunStr + 5 , "LOE_F" ) ||
			STRCMP_L( RunStr + 5 , "LSE_F" )
			) {
			if      ( STRCMP_L( RunStr + 5 , "_F"   ) ) id = 0;
			else if ( STRCMP_L( RunStr + 5 , "P_F"  ) ) id = 100;
			else if ( STRCMP_L( RunStr + 5 , "S_F"  ) ) id = 200;
			else if ( STRCMP_L( RunStr + 5 , "O_F"  ) ) id = 300;
			else if ( STRCMP_L( RunStr + 5 , "PE_F" ) ) id = 110;
			else if ( STRCMP_L( RunStr + 5 , "SE_F" ) ) id = 210;
			else if ( STRCMP_L( RunStr + 5 , "OE_F" ) ) id = 310;
			else if ( STRCMP_L( RunStr + 5 , "N_F"  ) ) id = 1000;
			else if ( STRCMP_L( RunStr + 5 , "NP_F" ) ) id = 1100;
			else if ( STRCMP_L( RunStr + 5 , "NS_F" ) ) id = 1200;
			else if ( STRCMP_L( RunStr + 5 , "NO_F" ) ) id = 1300;
			else if ( STRCMP_L( RunStr + 5 , "NPE_F" ) ) id = 1110;
			else if ( STRCMP_L( RunStr + 5 , "NSE_F" ) ) id = 1210;
			else if ( STRCMP_L( RunStr + 5 , "NOE_F" ) ) id = 1310;
			else if ( STRCMP_L( RunStr + 5 , "L_F"  ) ) id = 2000; // 2017.10.10
			else if ( STRCMP_L( RunStr + 5 , "LP_F" ) ) id = 2100;
			else if ( STRCMP_L( RunStr + 5 , "LS_F" ) ) id = 2200;
			else if ( STRCMP_L( RunStr + 5 , "LO_F" ) ) id = 2300;
			else if ( STRCMP_L( RunStr + 5 , "LPE_F" ) ) id = 2110;
			else if ( STRCMP_L( RunStr + 5 , "LSE_F" ) ) id = 2210;
			else if ( STRCMP_L( RunStr + 5 , "LOE_F" ) ) id = 2310;
			FA_RUN = 1;
			//------------------------------------------------------------------------------------------
			strcpy( JobStr , "" );
			strcpy( LotStr , "" );
			strcpy( MidStr , "" );
			ResSC    = -1;
			ENDINDEX = -1;
			SS = -1;
			ES = -1;
			LC = -1;
			//------------------------------------------------------------------------------------------
			REG_ID = SCHEDULER_REG_GET_REGIST_INDEX();
			EVSTR_POINTER = strlen( RunStr );
			while ( TRUE ) {
				if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == 0   ) break;
				if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == ' ' ) EVSTR_POINTER++;
				else break;
			}
			//------------------------------------------------------------------------------------------
			if ( !USER_RECIPE_DEFAULT_DATA_READ( REG_ID , PROGRAM_EVENT_READ() + EVSTR_POINTER , &ResSC , &ENDINDEX , JobStr , LotStr , MidStr , &SS , &ES , &LC ) ) {
				if ( ( ResSC >= 0 ) && ( ResSC < MAX_CASSETTE_SIDE ) ) {
					FA_REJECT_MESSAGE( ResSC , FA_START , ERROR_JOB_PARAM_ERROR , "" );
				}
				Event_Message_Reject( "USER DATA ANALYSIS ERROR" );
				return;
			}
			//------------------------------------------------------------------------------------------
			INDEX = id + ResSC;
			if ( INDEX < 0 ) {
				Event_Message_Reject( "START PORT ANALYSIS ERROR" );
				return;
			}
			if ( ( INDEX % 10 ) >= MAX_CASSETTE_SIDE ) {
				Event_Message_Reject( "START PORT ANALYSIS ERROR" );
				return;
			}
			if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
				/*
				// 2012.09.25
				if ( !FA_LOADING_SIGNAL_GET( INDEX % 10 ) ) {
					Event_Message_Reject( "LOADING SIGNAL IS NOT PREPARED" );
					FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
					//----------------------------------------------------------------
					// 2006.08.25
					//----------------------------------------------------------------
					if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
						ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
					}
					//----------------------------------------------------------------
					return;
				}
				*/
				/*
				// 2013.06.21
				// 2012.09.25
				for ( msc = 20 ; msc >= 0 ; msc-- ) {
					//
					if ( !FA_LOADING_SIGNAL_GET( INDEX % 10 ) ) {
						//
						if ( msc == 0 ) {
							//
							Event_Message_Reject( "LOADING SIGNAL IS NOT PREPARED" );
							FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
							//----------------------------------------------------------------
							// 2006.08.25
							//----------------------------------------------------------------
							if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
								ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
							}
							//----------------------------------------------------------------
							return;
							//
						}
						//
						Sleep(100);
						//
					}
					else {
						break;
					}
					//
				}
				*/
				//=================================================================================
				// 2013.06.21
				//=================================================================================
				//
				TimeCount = GetTickCount();
				//
				while ( TRUE ) {
					//
					if ( !FA_LOADING_SIGNAL_GET( INDEX % 10 ) ) {
						//
						if ( ( GetTickCount() - TimeCount ) >= 2500 ) {
							//
							Event_Message_Reject( "LOADING SIGNAL IS NOT PREPARED" );
							FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
							//----------------------------------------------------------------
							// 2006.08.25
							//----------------------------------------------------------------
							if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
								ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
							}
							//----------------------------------------------------------------
							return;
						}
					}
					else {
						break;
					}
					//
					Sleep(1);
					//
				}
				//=================================================================================
				//
			}
			if ( !_i_SCH_PRM_GET_FA_MAPPING_SKIP() ) {
				/*
				// 2012.09.25
				if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) != 1 ) {
					Event_Message_Reject( "MAPPING SIGNAL IS NOT PREPARED" );
					FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
					//----------------------------------------------------------------
					// 2006.08.25
					//----------------------------------------------------------------
					if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
						ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
					}
					//----------------------------------------------------------------
					return;
				}
				*/
				/*
				// 2013.06.21
				// 2012.09.25
				for ( msc = 20 ; msc >= 0 ; msc-- ) {
					//
					if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) != 1 ) {
						//
						if ( msc == 0 ) {
							//
							Event_Message_Reject( "MAPPING SIGNAL IS NOT PREPARED" );
							FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
							//----------------------------------------------------------------
							// 2006.08.25
							//----------------------------------------------------------------
							if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
								ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
							}
							//----------------------------------------------------------------
							return;
						}
						//
						Sleep(100);
						//
					}
					else {
						break;
					}
					//
				}
				*/
				//
				//=================================================================================
				// 2013.06.21
				//=================================================================================
				//
				TimeCount = GetTickCount();
				//
				while ( TRUE ) {
					//
					if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) != 1 ) {
						//
						if ( ( GetTickCount() - TimeCount ) >= 2500 ) {
							//
							Event_Message_Reject( "MAPPING SIGNAL IS NOT PREPARED" );
							FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
							//----------------------------------------------------------------
							// 2006.08.25
							//----------------------------------------------------------------
							if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
								ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
							}
							//----------------------------------------------------------------
							return;
						}
					}
					else {
						break;
					}
					//
					Sleep(1);
					//
				}
				//=================================================================================
				//
			}
			else {
				/*
				// 2011.12.30
//				if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) == 2 ) { // 2008.03.19
				if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) >= 2 ) { // 2008.03.19
					Event_Message_Reject( "MAPPING SIGNAL IS RUNNING" );
					FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
					//----------------------------------------------------------------
					// 2006.08.25
					//----------------------------------------------------------------
					if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
						ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
					}
					//----------------------------------------------------------------
					return;
				}
				*/
				//
				/*
				// 2013.06.21
				// 2011.12.30
				for ( msc = 20 ; msc >= 0 ; msc-- ) {
					//
					if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) >= 2 ) {
						//
						if ( msc == 0 ) {
							//
							Event_Message_Reject( "MAPPING SIGNAL IS RUNNING" );
							FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
							//----------------------------------------------------------------
							// 2006.08.25
							//----------------------------------------------------------------
							if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
								ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
							}
							//----------------------------------------------------------------
							return;
						}
						//
						Sleep(100);
						//
					}
					else {
						break;
					}
					//
				}
				*/
				//
				//=================================================================================
				// 2013.06.21
				//=================================================================================
				//
				TimeCount = GetTickCount();
				//
				while ( TRUE ) {
					//
					if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) >= 2 ) {
						//
						if ( ( GetTickCount() - TimeCount ) >= 2500 ) {
							//
							Event_Message_Reject( "MAPPING SIGNAL IS RUNNING" );
							FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
							//----------------------------------------------------------------
							// 2006.08.25
							//----------------------------------------------------------------
							if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
								ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
							}
							//----------------------------------------------------------------
							return;
						}
					}
					else {
						break;
					}
					//
					Sleep(1);
					//
				}
				//=================================================================================
			}
			//======
			if ( Get_RunPrm_RUNNING_TRANSFER_EXCEPT_AUTOMAINT() ) { // 2012.07.28
				Event_Message_Reject( "TRANSFER SYSTEM IS RUNNING ALREADY" );
				FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
				return;
			}
			//======
			if ( ( ENDINDEX < 0 ) || ( ENDINDEX >= MAX_CASSETTE_SIDE ) ) {
				Event_Message_Reject( "END PORT ANALYSIS ERROR" );
				FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_PARAM_ERROR , "" );
				return;
			}
			if ( _i_SCH_SYSTEM_USING_GET( INDEX % 10 ) > 0 ) {
				if ( _i_SCH_SYSTEM_FA_GET( INDEX % 10 ) != 1 ) {
					Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
					FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
					return;
				}
				else {
//					if ( !_i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() ) { // 2017.01.11
					if ( ( _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() % 2 ) == 0 ) { // 2017.01.11
						Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
						FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
						return;
					}
				}
			}
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) { // 2011.10.11
				if ( ( INDEX % 10 ) != ENDINDEX ) {
					if ( _i_SCH_SYSTEM_USING_GET( ENDINDEX ) > 0 ) {
						Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY 2" );
						FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
						return;
					}
				}
			}
			//================================================================================
			if ( SCHEDULER_REG_GET_DISABLE( INDEX % 10 ) ) {
				Event_Message_Reject( "AUTO SYSTEM CAN NOT RUNNING" );
				FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
				return;
			}
			//================================================================================
			//================================================================================
			MaxSlot = Scheduler_Get_Max_Slot( INDEX % 10 , ( INDEX % 10 ) + 1 , ENDINDEX + 1 , 13 , &maxunuse ); // 2010.12.17
			//================================================================================
			if ( SS <= 0 ) SS = 1;
			if ( ( ES <= 0 ) || ( ES > MaxSlot ) ) ES = MaxSlot; // 2010.12.17
			//======
			if ( LC <= 0 ) LC = 1;
			//======
			id = Scheduler_InfoRegCheck( INDEX % 10 , ENDINDEX , LotStr , SS , ES , LC );
			if ( id != 0 ) {
				Event_Message_Reject( "AUTO SYSTEM INFORMATION CHECK ERROR" );
				FA_REJECT_MESSAGE( INDEX % 10 , FA_START , id , "" );
				return;
			}
			//================================================================================
			if ( SCHEDULER_REG_GET_DISABLE( INDEX % 10 ) ) {
				Event_Message_Reject( "AUTO SYSTEM CAN NOT RUNNING" );
				FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
				return;
			}
			//================================================================================
			Scheduler_Reg.RunIndex  = REG_ID;
			Scheduler_Reg.CassIn    = INDEX % 10;
			Scheduler_Reg.CassOut   = ENDINDEX;
			Scheduler_Reg.SlotStart = SS;
			Scheduler_Reg.SlotEnd   = ES;
			Scheduler_Reg.LoopCount = LC;
//			strncpy( Scheduler_Reg.JobName , JobStr , 128 ); // 2005.05.11
			strncpy( Scheduler_Reg.JobName , JobStr , 512 ); // 2005.05.11
			strncpy( Scheduler_Reg.LotName , LotStr , 128 );
			strncpy( Scheduler_Reg.MidName , MidStr , 128 );
			if ( !_i_SCH_MULTIREG_SET_REGIST_DATA( INDEX % 10 , &Scheduler_Reg ) ) {
				Event_Message_Reject( "AUTO SYSTEM INFORMATION REGISTER ERROR" );
				FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_PARAM_ERROR , "" );
				return;
			}
		}
		else {
			if ( INDEX >= 0 ) {
				/*
				// 2011.12.30
//				if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) == 2 ) { // 2008.03.19
				if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) >= 2 ) { // 2008.03.19
					Event_Message_Reject( "MAPPING SIGNAL IS RUNNING2" );
					if ( FA_RUN == 2 ) {
						FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
						//----------------------------------------------------------------
						// 2006.08.25
						//----------------------------------------------------------------
						if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
							ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
						}
						//----------------------------------------------------------------
					}
					return;
				}
				*/
				//
				/*
				// 2013.06.21
				// 2011.12.30
				for ( msc = 20 ; msc >= 0 ; msc-- ) {
					//
					if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) >= 2 ) {
						//
						if ( msc == 0 ) {
							Event_Message_Reject( "MAPPING SIGNAL IS RUNNING2" );
							if ( FA_RUN == 2 ) {
								FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
								//----------------------------------------------------------------
								// 2006.08.25
								//----------------------------------------------------------------
								if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
									ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
								}
								//----------------------------------------------------------------
							}
							return;
						}
						//
						Sleep(100);
						//
					}
					else {
						break;
					}
				}
				*/
				//
				//=================================================================================
				// 2013.06.21
				//=================================================================================
				//
				TimeCount = GetTickCount();
				//
				while ( TRUE ) {
					//
					if ( FA_MAPPING_SIGNAL_GET( INDEX % 10 ) >= 2 ) {
						//
						if ( ( GetTickCount() - TimeCount ) >= 2500 ) {
							//
							Event_Message_Reject( "MAPPING SIGNAL IS RUNNING2" );
							if ( FA_RUN == 2 ) {
								FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_NOT_PREPARE , "" );
								//----------------------------------------------------------------
								// 2006.08.25
								//----------------------------------------------------------------
								if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
									ERROR_HANDLER( ERROR_NOT_PREPARE + 1000 , "" );
								}
								//----------------------------------------------------------------
							}
							return;
						}
					}
					else {
						break;
					}
					//
					Sleep(1);
					//
				}
				//=================================================================================
				//------------------------------------------------------------------------------------------
				strcpy( JobStr , "" );
				strcpy( LotStr , "" );
				strcpy( MidStr , "" );
				ResSC    = -1;
				ENDINDEX = -1;
				SS = -1;
				ES = -1;
				LC = -1;
				//------------------------------------------------------------------------------------------
				EVSTR_POINTER = strlen( RunStr );
				while ( TRUE ) {
					if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == 0   ) break;
					if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == ' ' ) EVSTR_POINTER++;
					else break;
				}
				//------------------------------------------------------------------------------------------
				if ( !USER_RECIPE_MANUAL_DEFAULT_DATA_READ( INDEX % 10 , PROGRAM_EVENT_READ() + EVSTR_POINTER , &ResSC , &ENDINDEX , JobStr , LotStr , MidStr , &SS , &ES , &LC ) ) {
					Event_Message_Reject( "USER MANUAL DATA ANALYSIS ERROR" );
					return;
				}
				_i_SCH_SYSTEM_SET_JOB_ID( INDEX % 10 , JobStr );
				_i_SCH_SYSTEM_SET_MID_ID( INDEX % 10 , MidStr );
			}
		}

		if ( FA_RUN == 1 ) {
			//
//			SCHEDULER_Make_CARRIER_INDEX( INDEX % 10 ); // 2011.08.17 // 2011.09.14
			SCHMRDATA_Data_Setting_for_PreStarting( ( INDEX % 10 ) + CM1 , INDEX % 10 ); // 2011.09.07
			//
			_i_SCH_SYSTEM_CPJOB_ID_SET( INDEX % 10 , INDEX % 10 );
			_i_SCH_SYSTEM_CPJOB_SET( INDEX % 10 , 0 );
			FA_ACCEPT_MESSAGE( INDEX % 10 , FA_START , 0 );
			//
			if ( _i_SCH_SYSTEM_USING_GET( INDEX % 10 ) <= 0 ) {
				//
				_i_SCH_SYSTEM_FA_SET( INDEX % 10 , FA_RUN );
				_i_SCH_SYSTEM_BLANK_SET( INDEX % 10 , 0 ); // 2011.04.20
				_i_SCH_SYSTEM_RESTART_SET( INDEX % 10 , 0 ); // 2011.09.23
				_i_SCH_SYSTEM_MOVEMODE_SET( INDEX % 10 , MOVEMODE ); // 2013.09.03
				//
				if ( _beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) INDEX ) == -1 ) {
					//----------------------------------------------------------------------------------------------------------------
					// 2004.08.18
					//----------------------------------------------------------------------------------------------------------------
					_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Run_Main_Handling_Code(1) %d\n" , INDEX );
					//
					FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_THREAD_FAIL , "" );
					_i_SCH_MULTIREG_DATA_RESET( INDEX % 10 );
					//----------------------------------------------------------------------------------------------------------------
				}
			}
		}
		else {
			if ( ( ( INDEX % 10 ) < 0 ) || ( ( INDEX % 10 ) >= MAX_CASSETTE_SIDE ) ) {
				Event_Message_Reject( "START PORT ANALYSIS ERROR" );
				return;
			}
			//======
			if ( Get_RunPrm_RUNNING_TRANSFER_EXCEPT_AUTOMAINT() ) { // 2012.07.28
				if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
				Event_Message_Reject( "TRANSFER SYSTEM IS RUNNING ALREADY" );
			}
			else if ( _i_SCH_SYSTEM_USING_GET( INDEX % 10 ) > 0 ) {
				if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
				Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
			}
			else {
				//
				if ( JOBRUN == 0 ) { // 2008.04.23
					if ( SCHMULTI_CTJOB_USE_GET() ) {
						if ( SCHMULTI_CONTROLJOB_MANUAL_POSSIBLE() ) {
							_i_SCH_SYSTEM_CPJOB_ID_SET( INDEX % 10 , INDEX % 10 );
							_i_SCH_SYSTEM_CPJOB_SET( INDEX % 10 , 2 );
						}
						else {
							if ( FA_RUN == 2 ) {
								FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_RUN_ALREADY , "" );
								//----------------------------------------------------------------
								// 2006.08.25
								//----------------------------------------------------------------
								if ( _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) {
									ERROR_HANDLER( ERROR_JOB_RUN_ALREADY + 1000 , "" );
								}
								//----------------------------------------------------------------
							}
							else { // 2006.08.25
								ERROR_HANDLER( ERROR_JOB_RUN_ALREADY , "" );
							}
							return;
						}
					}
					else {
						_i_SCH_SYSTEM_CPJOB_ID_SET( INDEX % 10 , INDEX % 10 );
						_i_SCH_SYSTEM_CPJOB_SET( INDEX % 10 , 0 );
					}
				}
				//
//				SCHEDULER_Make_CARRIER_INDEX( INDEX % 10 ); // 2011.08.17 // 2011.09.14
				SCHMRDATA_Data_Setting_for_PreStarting( ( INDEX % 10 ) + CM1 , INDEX % 10 ); // 2011.09.07
				//
				if ( FA_RUN == 2 ) FA_ACCEPT_MESSAGE( INDEX % 10 , FA_START , 0 );
				//
				if ( JOBRUN == 0 ) {
					//
					_i_SCH_SYSTEM_FA_SET( INDEX % 10 , FA_RUN );
					_i_SCH_SYSTEM_BLANK_SET( INDEX % 10 , 0 ); // 2011.04.20
					_i_SCH_SYSTEM_RESTART_SET( INDEX % 10 , RESTARTMODE ); // 2011.09.23
					_i_SCH_SYSTEM_MOVEMODE_SET( INDEX % 10 , MOVEMODE ); // 2013.09.03
					//
					if ( _beginthread( (void *) Scheduler_Run_Main_Handling_Code , 0 , (void *) INDEX ) == -1 ) {
						//----------------------------------------------------------------------------------------------------------------
						// 2004.08.18
						//----------------------------------------------------------------------------------------------------------------
						_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Run_Main_Handling_Code(2) %d\n" , INDEX );
						//
						if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_THREAD_FAIL , "" );
						_i_SCH_MULTIREG_DATA_RESET( INDEX % 10 );
						//----------------------------------------------------------------------------------------------------------------
					}
					else { // 2012.07.21
						//
						if ( RESTARTMODE >= 2 ) {
							//
//							for ( SS = ( ( INDEX % 10 ) + 1 ) ; SS < MAX_CASSETTE_SIDE ; SS ) { // 2013.07.29
							for ( SS = ( ( INDEX % 10 ) + 1 ) ; SS < MAX_CASSETTE_SIDE ; SS++ ) { // 2013.07.29
								//
								if ( _i_SCH_IO_GET_MAIN_BUTTON( SS ) != CTC_IDLE ) continue;
								//
								sprintf( DispStr , "START%d%c" , ( SS + 1 ) , ( RESTARTMODE == 2 ) ? 'T' : 'A' );
								//
								_dWRITE_FUNCTION_EVENT_TH( -1 , DispStr );
								//
								break;
							}
						}
						//
					}
					//
				}
				else { // 2008.04.23
					ENDINDEX = Scheduler_Run_Gui_And_Job_Code( INDEX % 10 , ( JOBRUN == 1 ) , INDEX , MOVEMODE ); // 2008.04.23
					if ( ENDINDEX != ERROR_NONE ) {
						if ( FA_RUN == 2 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_START , ERROR_JOB_PARAM_ERROR , "" );
						Event_Message_Reject( "GUI JOB START ERROR" );
						_IO_CIM_PRINTF( "GUI JOB START ERROR %d (E=%d)\n" , INDEX , ENDINDEX );
					}
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "REGIST" , 6 ) ) {
		if      ( STRCMP_L( RunStr + 6 , "_F"   ) ) { FA_RUN = 1; QA_RUN = FALSE; }
		else if ( STRCMP_L( RunStr + 6 , "C_F"  ) ) { FA_RUN = 1; QA_RUN = TRUE; }
		else if ( STRCMP_L( RunStr + 6 , "C"    ) ) { FA_RUN = 0; QA_RUN = TRUE; }
		else if ( STRCMP_L( RunStr + 6 , "C(F)" ) ) { FA_RUN = 2; QA_RUN = TRUE; }
		else if ( STRCMP_L( RunStr + 6 , "(F)"  ) ) { FA_RUN = 2; QA_RUN = FALSE; }
		else                                        { FA_RUN = 0; QA_RUN = FALSE; }
		//------------------------------------------------------------------------------------------
		strcpy( JobStr , "" );
		strcpy( LotStr , "" );
		strcpy( MidStr , "" );
		strcpy( DispStr , "" );
		INDEX    = -1;
		ENDINDEX = -1;
		SS = -1;
		ES = -1;
		LC = -1;
		//------------------------------------------------------------------------------------------
		EVSTR_POINTER = strlen( RunStr );
		while ( TRUE ) {
			if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == 0   ) break;
			if ( PROGRAM_EVENT_READ()[EVSTR_POINTER] == ' ' ) EVSTR_POINTER++;
			else break;
		}
		if ( !USER_RECIPE_DEFAULT_DATA_REGIST( PROGRAM_EVENT_READ() + EVSTR_POINTER , &INDEX , &ENDINDEX , JobStr , LotStr , MidStr , &SS , &ES , &LC , DispStr ) ) {
			if ( ( INDEX >= 0 ) && ( INDEX < MAX_CASSETTE_SIDE ) ) {
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_REGIST , ERROR_JOB_PARAM_ERROR , "" );
			}
			Event_Message_Reject( "USER DATA ANALYSIS ERROR" );
			return;
		}
		//------------------------------------------------------------------------------------------
		if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SIDE ) ) {
			Event_Message_Reject( "START PORT ANALYSIS ERROR" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_REGIST , ERROR_JOB_PARAM_ERROR , "" );
			return;
		}
		//if ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) {
		//	Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
		//	if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_REGIST , ERROR_JOB_RUN_ALREADY , "" );
		//	return;
		//}
		//======
		if ( ( ENDINDEX < 0 ) || ( ENDINDEX >= MAX_CASSETTE_SIDE ) ) {
			Event_Message_Reject( "END PORT ANALYSIS ERROR" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_REGIST , ERROR_JOB_PARAM_ERROR , "" );
			return;
		}
		//================================================================================
		MaxSlot = Scheduler_Get_Max_Slot( INDEX , INDEX + 1 , ENDINDEX + 1 , 14 , &maxunuse ); // 2010.12.17
		//================================================================================
		//======
		if ( SS <= 0 ) SS = 1;
		if ( ( ES <= 0 ) || ( ES > MaxSlot ) ) ES = MaxSlot; // 2010.12.17
		//======
		if ( LC <= 0 ) LC = 1;
		//========================================
		id = Scheduler_InfoRegistCheck( INDEX , ENDINDEX , JobStr , LotStr , MidStr , SS , ES , LC , DispStr , QA_RUN , FA_RUN );
		if ( id != 0 ) {
			if ( FA_RUN != 0 ) {
				if ( id == 9999 )
					FA_ACCEPT_MESSAGE( INDEX , FA_REGIST , 0 );
				else
					FA_REJECT_MESSAGE( INDEX , FA_REGIST , id , "" );
			}
			return;
		}
	}
	//--------------------------------------------------------------------------------
	else if (
		( STRNCMP_L( RunStr , "END" , 3 ) ) ||
		( STRNCMP_L( RunStr , "STOP" , 4 ) ) ||
		( STRNCMP_L( RunStr , "ASTOP" , 5 ) ) ||
		( STRNCMP_L( RunStr , "STOPEND" , 7 ) ) ) { // 2003.12.01

/*
//
// 2017.07.17
		if      ( STRNCMP_L( RunStr , "END"      , 3 ) ) QA_RUN2 = 3;
		else if ( STRNCMP_L( RunStr , "STOPEND"  , 7 ) ) QA_RUN2 = 7; // 2003.12.01
		else if ( STRNCMP_L( RunStr , "STOP"     , 4 ) ) QA_RUN2 = 4;
		else if ( STRNCMP_L( RunStr , "ASTOP"    , 5 ) ) QA_RUN2 = 5;
*/

		//
		// 2017.07.17
		//
		JOBRUN = 0;
		RESTARTMODE = 0;
		//
		if      ( STRNCMP_L( RunStr , "ENDC"     , 4 ) ) { RESTARTMODE = 0; QA_RUN2 = 4; JOBRUN = 1; }
		else if ( STRNCMP_L( RunStr , "STOPENDC" , 8 ) ) { RESTARTMODE = 3; QA_RUN2 = 8; JOBRUN = 1; }
		else if ( STRNCMP_L( RunStr , "STOPC"    , 5 ) ) { RESTARTMODE = 1; QA_RUN2 = 5; JOBRUN = 1; }
		else if ( STRNCMP_L( RunStr , "ASTOPC"   , 6 ) ) { RESTARTMODE = 2; QA_RUN2 = 6; JOBRUN = 1; }
		else if ( STRNCMP_L( RunStr , "END"      , 3 ) ) { RESTARTMODE = 0; QA_RUN2 = 3; }
		else if ( STRNCMP_L( RunStr , "STOPEND"  , 7 ) ) { RESTARTMODE = 3; QA_RUN2 = 7; }
		else if ( STRNCMP_L( RunStr , "STOP"     , 4 ) ) { RESTARTMODE = 1; QA_RUN2 = 4; }
		else if ( STRNCMP_L( RunStr , "ASTOP"    , 5 ) ) { RESTARTMODE = 2; QA_RUN2 = 5; }
		//
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + QA_RUN2 , TRUE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_END , 0 , "" );
		}
//		else if ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) != 0 ) { // 2009.03.11
		else if ( ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) != 0 ) && ( ( !_i_SCH_SYSTEM_MODE_WAITR_GET( INDEX ) || ( _i_SCH_SYSTEM_PAUSE_GET( INDEX ) != 0 ) ) ) ) { // 2009.03.11
			Event_Message_Reject( "AUTO SYSTEM IS ADAPT END/STOP MESSAGE ALREADY" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_END , 0 , "" );
		}
		else if ( _i_SCH_SYSTEM_MODE_RESUME_GET( INDEX ) ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT ADAPT RESUME MESSAGE" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_END , 0 , "" );
		}
		else {
			//==================================================================================================
			if ( _i_SCH_SYSTEM_MODE_WAITR_GET( INDEX ) ) { // 2009.03.11
				_i_SCH_SYSTEM_MODE_WAITR_SET( INDEX , FALSE ); // 2009.03.11
			}
			//==================================================================================================
			_i_SCH_IO_SET_END_BUTTON( INDEX , CTCE_ENDING );
			//==================================================================================================
			if ( ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) < 10 ) ) { // 2002.11.15
//				_i_SCH_SYSTEM_USING_SET( INDEX , 100 ); // 2002.11.15 // 2018.11.01
				_i_SCH_SYSTEM_USING_SET( INDEX , 101 ); // 2002.11.15 // 2018.11.01
			} // 2002.11.15
			//
			//==================================================================================================
			//
			if ( ( JOBRUN == 1 ) && ( _i_SCH_SYSTEM_PAUSE_GET( INDEX ) != 0 ) ) { // 2017.07.17
				//
				JOBRUN = 2;
				//
				_i_SCH_SYSTEM_PAUSE_SET( INDEX , 0 );
				//
				if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) {
					_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_RUNNING );
				}
				else {
					if ( _i_SCH_SYSTEM_RUN_TAG_GET( INDEX ) > 0 ) {
						_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_RUNNING );
					}
					else {
						_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_WAITING );
					}
				}

				if ( _i_SCH_SYSTEM_LASTING_GET( INDEX ) ) {
					if ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 0 ) {
						//
//						if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 ); // 2017.08.24
						//
					}
					else {
						if ( ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 2 ) || ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 4 ) || ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) == 6 ) ) {
							_i_SCH_IO_SET_END_BUTTON( INDEX , CTCE_RUNNING );
							_i_SCH_SYSTEM_MODE_RESUME_SET( INDEX , TRUE );
							//
//							if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 ); // 2017.08.24
							//
						}
					}
				}
				else {
					if ( _i_SCH_SYSTEM_MODE_END_GET( INDEX ) != 0 ) {
						_i_SCH_IO_SET_END_BUTTON( INDEX , CTCE_RUNNING );
						_i_SCH_SYSTEM_MODE_RESUME_SET( INDEX , TRUE );
					}
					//
//					if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_CONTINUE , 0 ); // 2017.08.24
					//
				}
				//
			}
			//
			//==================================================================================================
			//
//			if ( QA_RUN2 == 3 ) { // End // 2017.07.17
			if ( RESTARTMODE == 0 ) { // End // 2017.07.17
				_i_SCH_SYSTEM_MODE_END_SET( INDEX , QA_RUN + 1 );
				if ( QA_RUN == 0 ) _i_SCH_SYSTEM_LASTING_SET( INDEX , TRUE ); // 2002.09.05
				//
				if ( JOBRUN == 2 ) { // 2017.07.17
					if ( QA_RUN == 1 ) {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control End+ContinueW%cSCTLENDWC\n" , 9 );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control End+Continue%cSCTLENDC\n" , 9 );
					}
				}
				else {
					if ( QA_RUN == 1 ) { // 2009.03.12
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control EndW%cSCTLENDW\n" , 9 );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control End%cSCTLEND\n" , 9 ); // 2004.03.17
					}
				}
				//
			}
//			else if ( QA_RUN2 == 4 ) { // Stop // 2017.07.17
			else if ( RESTARTMODE == 1 ) { // Stop // 2017.07.17
				PROCESS_MONITOR_STATUS_Abort_Signal_Set( INDEX , FALSE );
				_i_SCH_SYSTEM_MODE_END_SET( INDEX , QA_RUN + 1 + 2 );
				//
				if ( JOBRUN == 2 ) { // 2017.07.17
					if ( QA_RUN == 1 ) {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control StopW+Continue%cSCTLSTOPWC\n" , 9 );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Stop+Continue%cSCTLSTOPC\n" , 9 );
					}
				}
				else {
					if ( QA_RUN == 1 ) { // 2009.03.12
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control StopW%cSCTLSTOPW\n" , 9 );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Stop%cSCTLSTOP\n" , 9 ); // 2004.03.17
					}
				}
				//
			}
//			else if ( QA_RUN2 == 5 ) { // AStop // 2017.07.17
			else if ( RESTARTMODE == 2 ) { // AStop // 2017.07.17
				PROCESS_MONITOR_STATUS_Abort_Signal_Set( INDEX , TRUE );
				_i_SCH_SYSTEM_MODE_END_SET( INDEX , QA_RUN + 1 + 2 );
				//
				if ( JOBRUN == 2 ) { // 2017.07.17
					if ( QA_RUN == 1 ) {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control AStopW+Continue%cSCTLASTOPWC\n" , 9 );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control AStop+Continue%cSCTLASTOPC\n" , 9 );
					}
				}
				else {
					if ( QA_RUN == 1 ) { // 2009.03.12
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control AStopW%cSCTLASTOPW\n" , 9 ); // 2004.03.17
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control AStop%cSCTLASTOP\n" , 9 ); // 2004.03.17
					}
				}
				//
			}
//			else if ( QA_RUN2 == 7 ) { // StopEnd // 2003.12.01 // 2017.07.17
			else if ( RESTARTMODE == 3 ) { // StopEnd // 2003.12.01 // 2017.07.17
				_i_SCH_SYSTEM_MODE_END_SET( INDEX , 7 ); // 2003.12.01
				_i_SCH_SYSTEM_LASTING_SET( INDEX , TRUE ); // 2003.12.01
				//
				if ( JOBRUN == 2 ) { // 2017.07.17
					_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control StopEnd+Continue%cSCTLSTOPENDC\n" , 9 );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control StopEnd%cSCTLSTOPEND\n" , 9 ); // 2004.03.17
				}
				//
			} // 2003.12.01
			//
//			if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_END , 0 ); // 2017.08.24
			//
			if ( JOBRUN == 2 ) { // 2017.08.24
				if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_END_CONTINUE , 0 );
			}
			else {
				if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_END , 0 );
			}
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "ABORT" , 5 ) ) {
		if      ( STRCMP_L( RunStr + 5 , "TR" ) ) {
			if ( Get_RunPrm_RUNNING_TRANSFER() ) {
				BUTTON_SET_TR_CONTROL( CTC_ABORTING );
				_i_SCH_SYSTEM_MODE_ABORT_SET( TR_CHECKING_SIDE ); // 2004.02.10
				Scheduler_All_Abort_Part( FALSE ); // 2003.06.03
//				SYSTEM_ALL_ABORT(); // 2003.06.03
			}
			else {
				Event_Message_Reject( "TRANSFER SYSTEM IS NOT RUNNING" );
			}
			return;
		}
		else if ( STRCMP_L( RunStr + 5 , "TR_F" ) ) {
			if ( Get_RunPrm_RUNNING_TRANSFER() ) {
				BUTTON_SET_TR_CONTROL( CTC_ABORTING );
				FA_ACCEPT_MESSAGE( TR_CHECKING_SIDE , FA_TRANSFER_ABORT , 0 );
				_i_SCH_SYSTEM_MODE_ABORT_SET( TR_CHECKING_SIDE ); // 2004.02.10
				Scheduler_All_Abort_Part( FALSE ); // 2003.06.03
//				SYSTEM_ALL_ABORT(); // 2003.06.03
			}
			else {
				Event_Message_Reject( "TRANSFER SYSTEM IS NOT RUNNING" );
				FA_REJECT_MESSAGE( TR_CHECKING_SIDE , FA_TRANSFER_ABORT , 0 , "" );
			}
			return;
		}
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 5 , TRUE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_ABORT , 0 , "" );
		}
		else {
			if ( QA_RUN == 1 ) {
				if ( !_i_SCH_SYSTEM_MODE_WAITR_GET( INDEX ) ) {
					Event_Message_Reject( "AUTO SYSTEM IS NOT WAITING" );
					if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_ABORT , 0 , "" );
					return;
				}
			}
			switch( _i_SCH_PRM_GET_UTIL_ABORT_MESSAGE_USE_POINT() ) {
			case 1 :
				if ( ( _i_SCH_SYSTEM_PAUSE_GET( INDEX ) != 0 ) || !_i_SCH_SYSTEM_MODE_WAITR_GET( INDEX ) ) {
					Event_Message_Reject( "AUTO SYSTEM IS NOT WAITING" );
					if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_ABORT , 0 , "" );
					return;
				}
				break;
			}
			_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_ABORTING );
			if ( ( _i_SCH_SYSTEM_PAUSE_GET( INDEX ) == 0 ) && _i_SCH_SYSTEM_MODE_WAITR_GET( INDEX ) ) {
				_i_SCH_SYSTEM_MODE_RESUME_SET( INDEX , 0 );
				_i_SCH_SYSTEM_MODE_END_SET( INDEX , 1 );
				//
				_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Abort(W)%cSCTLABORTW\n" , 9 ); // 2004.03.17
				//
			}
			else {
				for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
				Scheduler_All_Abort_Part( TRUE ); // 2003.06.03
//				SYSTEM_ALL_ABORT(); // 2003.06.03
				//
				_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Abort%cSCTLABORT\n" , 9 ); // 2004.03.17
				//
			}
			if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_ABORT , 0 );
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "HALT" , 4 ) ) { // 2003.11.05
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 4 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_ABORT , 1 , "" );
		}
		else {
			_SCH_IO_SET_MAIN_BUTTON_MC( INDEX , CTC_ABORTING );
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
			if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( INDEX , FA_ABORT , 1 );
			//
			_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Halt%cSCTLHALT\n" , 9 ); // 2004.03.17
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "FINISH" , 6 ) ) { // 2011.04.22
		//===================================================================================
		// 2011.04.27
		ENDINDEX = 0;
		LC = strlen( RunStr );
		if ( LC >= 9 ) {
			if ( STRCMP_L( RunStr + ( LC - 3 ) , "(R)" ) ) {
				ENDINDEX = 1;
				RunStr[LC - 3] = 0;
			}
		}
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 6 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( ( FA_RUN == 0 ) && ( _i_SCH_SYSTEM_USING_GET( INDEX ) > 0 ) && ( _i_SCH_SYSTEM_FA_GET( INDEX ) != 0 ) ) FA_RUN = _i_SCH_SYSTEM_FA_GET( INDEX );
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_ABORT , 1 , "" );
		}
		else {
			SCHEDULER_CONTROL_Make_Finish_Data( INDEX , ENDINDEX ); // 2011.04.27
			//
			_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Control Finish%cSCTLFINISH\n" , 9 );
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "DISAPPEAR" , 9 ) ) {
		if      ( STRCMP_L( RunStr + 9 , ""    ) ) { FA_RUN = 0; }
		else if ( STRCMP_L( RunStr + 9 , "(F)" ) ) { FA_RUN = 2; }
		else if ( STRCMP_L( RunStr + 9 , "_F"  ) ) { FA_RUN = 1; }
		else {
			Event_Message_Reject( "MESSAGE ANALYSIS ERROR" );
			return;
		}

		if ( !Get_RunPrm_RUNNING_CLUSTER() ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 0 , FA_DISAPPEAR , 0 , "" );
			return;
		}
		if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( 0 , FA_DISAPPEAR , 0 );
		SIGNAL_MODE_DISAPPEAR_SET();
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "RECIPE_RELOAD" , 13 ) ) {
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 13 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			return;
		}
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		ENDINDEX = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		LC = atoi( Else1Str );
		//
		if ( !SIGNAL_MODE_RECIPE_READ_SET( INDEX , ENDINDEX , LC , ElseStr ) ) {
			Event_Message_Reject( "SIGNAL_MODE_RECIPE_READ_SET ERROR" );
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "UNLOAD_STYLE" , 12 ) ) { // 2010.04.26
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 12 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		if ( _i_SCH_SYSTEM_USING_GET( INDEX ) <= 0 ) {
			Event_Message_Reject( "AUTO SYSTEM IS NOT RUNNING" );
			return;
		}
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		ENDINDEX = atoi( Else1Str );
		//
//		if ( ( ENDINDEX < 0 ) || ( ENDINDEX > 4 ) ) { // 2017.05.22
//			Event_Message_Reject( "DATA IS NOT VALID" ); // 2017.05.22
//			return; // 2017.05.22
//		} // 2017.05.22
		//
		_in_UNLOAD_USER_TYPE[INDEX] = ENDINDEX;
		//
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "MAPPING" , 7 ) ) {
		//
		//===================================================================================
		switch ( Event_Get_Side_Data( RunStr + 7 , FALSE , &FA_RUN , &INDEX , &QA_RUN ) ) {
		case 1 :
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
			if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
			break;
		case 0 :
			return;
			break;
		}
		//===================================================================================
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_4_MAPPING ); // 2011.09.07
		//
//		if ( !Event_CM_Run_Possible_Check( INDEX , TRUE ) ) { // 2016.07.15
//		if ( !Event_CM_Run_Possible_Check( INDEX , FALSE , TRUE ) ) { // 2016.07.15 // 2016.10.05
		if ( !Event_CM_Run_Possible_Check( INDEX , TRUE , FALSE , TRUE ) ) { // 2016.07.15 // 2016.10.05
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
			//
//			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 0 , "" ); // 2007.01.26
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 200 , "" ); // 2007.01.26
			return;
		}
		//---------------------------
/*
// 2012.04.01
		if ( SCHEDULER_CM_LOCKED_for_HANDOFF( INDEX + CM1 ) > 0 ) { // 2011.03.18
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
			//
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 300 , "" );
			return;
		}
*/
		//---------------------------
// 2013.06.21
		//======================================================================================
		if ( FA_MAPPING_SIGNAL_GET( INDEX ) >= 2 ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL );
			//
			Event_Message_Reject( "PORT IS RUNNING OR DONE" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 204 , "" );
			return;
		}
		//===================================================================================
//		if ( FA_RUN == 1 ) { // 2013.06.19
			//---------------------------
			// Append 2001.12.17
			if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
				//
				Event_Message_Reject( "PORT IS NOT PREPARED" );
//				FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 0 , "" ); // 2007.01.26
//				FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 201 , "" ); // 2007.01.26 // 2013.06.19
				if ( FA_RUN == 1 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 201 , "" ); // 2007.01.26 // 2013.06.19
				return;
			}
			//---------------------------
			// 2007.01.26
			//---------------------------
			/*
			// 2013.06.21
			for ( SS = 0 ; SS < 100 ; SS++ ) {
				if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) <= 0 ) {
					if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() == 1 ) break;
					if ( FA_LOADING_SIGNAL_GET( INDEX ) ) break;
				}
				Sleep(100);
			}
			*/
			//
			//=================================================================================
			// 2013.06.21
			//=================================================================================
			//
			TimeCount = GetTickCount();
			//
			while ( TRUE ) {
				//
				if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) <= 0 ) {
					if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() == 1 ) break;
					if ( FA_LOADING_SIGNAL_GET( INDEX ) ) break;
				}
				//
				if ( ( GetTickCount() - TimeCount ) >= 10000 ) break;
				//
				Sleep(1);
				//
			}
			//=================================================================================
			//---------------------------
			if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
				//
				Event_Message_Reject( "PORT IS NOT PREPARED" );
//				FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 0 , "" ); // 2007.01.26
//				FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 202 , "" ); // 2007.01.26 // 2013.06.19
				if ( FA_RUN == 1 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 202 , "" ); // 2007.01.26 // 2013.06.19
				return;
			}
			//---------------------------
			if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
				if ( !FA_LOADING_SIGNAL_GET( INDEX ) ) {
					//
					SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
					//
					Event_Message_Reject( "PORT IS NOT PREPARED" );
//					FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 0 , "" ); // 2007.01.26
//					FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 203 , "" ); // 2007.01.26 // 2013.06.19
					if ( FA_RUN == 1 ) FA_REJECT_MESSAGE( INDEX % 10 , FA_MAPPING , 203 , "" ); // 2007.01.26 // 2013.06.19
					return;
				}
			}
//		} // 2013.06.19
//		if ( BUTTON_GET_HANDOFF_CONTROL( INDEX + 1 ) != CTC_IDLE ) {
//			Event_Message_Reject( "PORT CAN NOT USE" );
//			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 0 , "" );
//			return;
//		}
		//======================================================================================
//		if ( FA_MAPPING_SIGNAL_GET( INDEX ) != 0 ) { // 2007.03.27
		//======================================================================================
/*
// 2013.06.21
//		if ( FA_MAPPING_SIGNAL_GET( INDEX ) == 2 ) { // 2007.03.27 // 2008.03.19
		if ( FA_MAPPING_SIGNAL_GET( INDEX ) >= 2 ) { // 2007.03.27 // 2008.03.19
		//======================================================================================
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
			//
			Event_Message_Reject( "PORT IS RUNNING OR DONE" );
//			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 0 , "" ); // 2007.01.26
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 204 , "" ); // 2007.01.26
			return;
		}
*/
		//====================================================================
		SIGNAL_MODE_ABORT_RESET( INDEX ); // 2004.09.08 // 2008.03.19
		//====================================================================
		FA_MAPPING_CANCEL_SET( INDEX , 0 ); // 2008.03.19
		FA_MAPPING_SIGNAL_SET( INDEX , 2 );
		//====================================================================
//		SIGNAL_MODE_ABORT_RESET( INDEX ); // 2004.09.08 // 2008.03.19
		//====================================================================
		/*
		// 2013.06.19
		if ( FA_RUN != 0 ) {
			if ( _beginthread( (void *) Scheduler_Carrier_Mapping , 0 , (void *) (INDEX + 100) ) == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Carrier_Mapping(3) %d\n" , INDEX + 100 );
				//
				FA_ACCEPT_MESSAGE( INDEX , FA_MAPPING , 0 );
//				FA_REJECT_MESSAGE( INDEX , FA_MAPPING , -1 , "" ); // 2007.01.26
				FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 103 , "" ); // 2007.01.26
				FA_MAPPING_SIGNAL_SET( INDEX , 0 );
				BUTTON_SET_FLOW_STATUS( INDEX , _MS_6_MAPFAIL );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		else {
			if ( _beginthread( (void *) Scheduler_Carrier_Mapping , 0 , (void *) INDEX ) == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Carrier_Mapping(4) %d\n" , INDEX );
				//
				FA_MAPPING_SIGNAL_SET( INDEX , 0 );
				BUTTON_SET_FLOW_STATUS( INDEX , _MS_6_MAPFAIL );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		*/
		//
		// 2013.06.19
		//
		if ( _beginthread( (void *) Scheduler_Carrier_Mapping , 0 , (void *) (INDEX + ( ( FA_RUN != 0 ) ? 100 : 0 ) ) ) == -1 ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_6_MAPFAIL );
			//
			_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Carrier_Mapping(3) %d , %d\n" , INDEX , FA_RUN );
			//
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( INDEX , FA_MAPPING , 103 , "" );
			//
			FA_MAPPING_SIGNAL_SET( INDEX , 0 );
			BUTTON_SET_FLOW_STATUS( INDEX , _MS_6_MAPFAIL );
			//----------------------------------------------------------------------------------------------------------------
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "FA_SERVER_ON" ) ) {
		if ( !FA_SERVER_FUNCTION_SET( 1 , ElseStr ) ) {
			Event_Message_Reject( "FA SERVER ON FAILURE" );
		}
		//----------------------------------------------
		GUI_SAVE_PARAMETER_DATA( 1 );
		//----------------------------------------------
	}
	else if ( STRCMP_L( RunStr , "FA_SERVER_OFF" ) ) {
		FA_SERVER_FUNCTION_SET( 0 , "" );
		//----------------------------------------------
		GUI_SAVE_PARAMETER_DATA( 1 );
		//----------------------------------------------
	}
	//
	else if ( STRCMP_L( RunStr , "FA_SERVER_ENABLE" ) ) {
		if ( FA_SERVER_FUNCTION_SET( 3 , "" ) ) {
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else {
			Event_Message_Reject( "FA SERVER ENABLE FAILURE" );
		}
	}
	else if ( STRCMP_L( RunStr , "FA_SERVER_DISABLE" ) ) {
		if ( FA_SERVER_FUNCTION_SET( 4 , "" ) ) {
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else {
			Event_Message_Reject( "FA SERVER DISABLE FAILURE" );
		}
	}
	//
	else if ( STRCMP_L( RunStr , "FA_AGV_ON" ) ) {
//-- 2002.05.20 // for Always set/change AGV Status
//		for ( SS = 0 ; SS < ( MAX_CASSETTE_SIDE + 1 ) ; SS++ ) {
//			if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) break;
//		}
//		if ( SS != ( MAX_CASSETTE_SIDE + 1 ) ) {
//			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
//			FA_REJECT_MESSAGE( 0 , FA_INFO , 10 , ElseStr );
//			return;
//		}
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) {
			FA_REJECT_MESSAGE( 0 , FA_INFO , 11 , Else1Str );
			return;
		}
		//
		if ( FA_AGV_FUNCTION_SET( INDEX , 1 , Else2Str ) ) {
			//------------------------------------------------------
			FA_AGV_STATUS_IO_SET( INDEX );
			//------------------------------------------------------
			FA_INFO_MESSAGE( 1 , INDEX );
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else {
			//------------------------------------------------------
			FA_AGV_STATUS_IO_SET( INDEX );
			//------------------------------------------------------
			Event_Message_Reject( "FA AGV ON FAILURE" );
			FA_REJECT_MESSAGE( 0 , FA_INFO , 12 , Else1Str );
		}
		//===================================================
		FA_AGV_AUTOHANDLER_SEND_MESSAGE( INDEX );
		//===================================================
	}
	else if ( STRCMP_L( RunStr , "FA_AGV_OFF" ) ) {
//-- 2002.05.20 // for Always set/change AGV Status
//		for ( SS = 0 ; SS < ( MAX_CASSETTE_SIDE + 1 ) ; SS++ ) {
//			if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) break;
//		}
//		if ( SS != ( MAX_CASSETTE_SIDE + 1 ) ) {
//			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
//			FA_REJECT_MESSAGE( 0 , FA_INFO , 10 , ElseStr );
//			return;
//		}
		//
		if ( !Event_LOT_ID_From_String_And_Check( ElseStr , &INDEX ) ) {
			FA_REJECT_MESSAGE( 0 , FA_INFO , 11 , ElseStr );
			return;
		}
		//------------------------------------------------------
		FA_AGV_FUNCTION_SET( INDEX , 0 , "" );
		//------------------------------------------------------
		FA_AGV_STATUS_IO_SET( INDEX );
		//------------------------------------------------------
		FA_INFO_MESSAGE( 1 , INDEX );
		//----------------------------------------------
		GUI_SAVE_PARAMETER_DATA( 1 );
		//----------------------------------------------
		//===================================================
		FA_AGV_AUTOHANDLER_SEND_MESSAGE( INDEX );
		//===================================================
	}
	else if ( STRCMP_L( RunStr , "FA_AGV_ENABLE" ) ) {
//-- 2002.05.20 // for Always set/change AGV Status
//		for ( SS = 0 ; SS < ( MAX_CASSETTE_SIDE + 1 ) ; SS++ ) {
//			if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) break;
//		}
//		if ( SS != ( MAX_CASSETTE_SIDE + 1 ) ) {
//			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
//			FA_REJECT_MESSAGE( 0 , FA_INFO , 10 , ElseStr );
//			return;
//		}
		if ( strlen( ElseStr ) <= 0 ) {
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) {
				FA_AGV_FUNCTION_SET( SS , 3 , "" );
				FA_AGV_STATUS_IO_SET( SS );
				//===================================================
				FA_AGV_AUTOHANDLER_SEND_MESSAGE( INDEX );
				//===================================================
			}
			FA_INFO_MESSAGE( 1 , 101 );
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else {
			//
			if ( !Event_LOT_ID_From_String_And_Check( ElseStr , &INDEX ) ) {
				FA_REJECT_MESSAGE( 0 , FA_INFO , 11 , ElseStr );
				return;
			}
			//
			if ( FA_AGV_FUNCTION_SET( INDEX , 3 , "" ) ) {
				//------------------------------------------------------
				FA_AGV_STATUS_IO_SET( INDEX );
				//------------------------------------------------------
				FA_INFO_MESSAGE( 1 , INDEX );
				//----------------------------------------------
				GUI_SAVE_PARAMETER_DATA( 1 );
				//----------------------------------------------
				//===================================================
				FA_AGV_AUTOHANDLER_SEND_MESSAGE( INDEX );
				//===================================================
			}
			else {
				Event_Message_Reject( "FA AGV ENABLE FAILURE" );
				FA_REJECT_MESSAGE( 0 , FA_INFO , 12 , ElseStr );
			}
		}
	}
	else if ( STRCMP_L( RunStr , "FA_AGV_DISABLE" ) ) {
//-- 2002.05.20 // for Always set/change AGV Status
//		for ( SS = 0 ; SS < ( MAX_CASSETTE_SIDE + 1 ) ; SS++ ) {
//			if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) break;
//		}
//		if ( SS != ( MAX_CASSETTE_SIDE + 1 ) ) {
//			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
//			FA_REJECT_MESSAGE( 0 , FA_INFO , 10 , ElseStr );
//			return;
//		}
		if ( strlen( ElseStr ) <= 0 ) {
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) {
				FA_AGV_FUNCTION_SET( SS , 4 , "" );
				FA_AGV_STATUS_IO_SET( SS );
				//===================================================
				FA_AGV_AUTOHANDLER_SEND_MESSAGE( SS );
				//===================================================
			}
			FA_INFO_MESSAGE( 1 , 100 );
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else {
			//
			if ( !Event_LOT_ID_From_String_And_Check( ElseStr , &INDEX ) ) {
				FA_REJECT_MESSAGE( 0 , FA_INFO , 11 , ElseStr );
				return;
			}
			//
			//------------------------------------------------------
			FA_AGV_FUNCTION_SET( INDEX , 4 , "" );
			//------------------------------------------------------
			FA_AGV_STATUS_IO_SET( INDEX );
			//------------------------------------------------------
			FA_INFO_MESSAGE( 1 , INDEX );
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
			//===================================================
			FA_AGV_AUTOHANDLER_SEND_MESSAGE( INDEX );
			//===================================================
		}
	}
	else if ( STRCMP_L( RunStr , "FA_AGV_STATUS" ) ) {
		//
		if ( !Event_LOT_ID_From_String_And_Check( ElseStr , &INDEX ) ) {
			FA_REJECT_MESSAGE( 0 , FA_INFO , 11 , ElseStr );
			return;
		}
		//
		FA_INFO_MESSAGE( 1 , INDEX );
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "MAKE_UNLOAD_REQUEST_F" , 21 ) ) {
		//
		if ( STRCMP_L( ElseStr , "SIMOPTION0" ) ) { // 2018.12.13
			SIM_UNLOADLOAD_OPTION = 0;
			printf( "SET SIM_UNLOADLOAD_OPTION 0\n" );
			return;
		}
		else if ( STRCMP_L( ElseStr , "SIMOPTION1" ) ) { // 2018.12.13
			SIM_UNLOADLOAD_OPTION = 1;
			printf( "SET SIM_UNLOADLOAD_OPTION 1\n" );
			return;
		}
		else if ( STRCMP_L( ElseStr , "SIMOPTION2" ) ) { // 2018.12.13
			SIM_UNLOADLOAD_OPTION = 2;
			printf( "SET SIM_UNLOADLOAD_OPTION 2\n" );
			return;
		}
		else if ( STRCMP_L( ElseStr , "SIMOPTION3" ) ) { // 2018.12.13
			SIM_UNLOADLOAD_OPTION = 3;
			printf( "SET SIM_UNLOADLOAD_OPTION 3\n" );
			return;
		}
		//
		FA_RUN = 1;
		//===================================================================================
		if ( STRCMP_L( RunStr + 21 , "(M)" ) ) SS = FALSE;	else SS = TRUE; // 2007.02.20
		//===================================================================================
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//===================================================================================
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//===================================================================================

		if ( STRCMP_L( Else2Str , "SIMSTOP_SUCCESS" ) ) { // 2018.12.13
			SIM_WAITING_TAG[INDEX] = 1;
			return;
		}
		else if ( STRCMP_L( Else2Str , "SIMSTOP_ERROR" ) ) { // 2018.12.13
			SIM_WAITING_TAG[INDEX] = 2;
			return;
		}
		else if ( STRCMP_L( Else2Str , "SIMSTOP_ABORT" ) ) { // 2018.12.13
			SIM_WAITING_TAG[INDEX] = 3;
			return;
		}

		//===================================================================================
//		if ( !Event_CM_Run_Possible_Check( INDEX , SS ) ) return; // 2016.07.15
//		if ( !Event_CM_Run_Possible_Check( INDEX , TRUE , SS ) ) return; // 2016.07.15 // 2016.10.05
		if ( !Event_CM_Run_Possible_Check( INDEX , FALSE , TRUE , SS ) ) return; // 2016.07.15 // 2016.10.05
		//===================================================================================
/*
// 2011.08.01
		if ( SCHEDULER_CM_LOCKED_for_HANDOFF( INDEX + CM1 ) > 0 ) return; // 2011.03.18
*/
// 2012.04.01
		//
		//===================================================================================
		if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
			Event_Message_Reject( "LOAD CONTROL RUN & WAITING ALREADY" );
			return;
		}
		if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
			Event_Message_Reject( "UNLOAD CONTROL RUN & WAITING" );
			return;
		}
		//---------------------------
		// Append 2001.12.17
//		if ( FA_MAPPING_SIGNAL_GET( INDEX ) == 2 ) return; // 2008.03.19
		if ( FA_MAPPING_SIGNAL_GET( INDEX ) >= 2 ) {
			Event_Message_Reject( "MAPPING CONTROL RUN & WAITING" );
			return; // 2008.03.19
		}
		//--------------------------------------------------------------
		//---------------------------
		if ( !SCH_MR_UNLOAD_LOCK_START( INDEX ) ) { // 2012.04.01
			Event_Message_Reject( "CAN NOT CACEL CM_LOCKED CURRENT_CR_LOCKING" );
		}
		//---------------------------
		if ( SCH_MR_UNLOAD_LOCK_GET_PICKPLACE( INDEX ) ) { // 2012.04.01
			Event_Message_Reject( "CAN NOT CACEL CM_LOCKED CURRENT_PICKPLACING" );
		}
		else {
			if ( SCHEDULER_CM_LOCKED_CHECK_FOR_UNLOAD( INDEX + CM1 ) > 0 ) { // 2012.04.01
				Event_Message_Reject( "CAN NOT CACEL CM_LOCKED NOT FREE" );
			}
			else {
				//
				// 2016.07.15
				//
				if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() == 1 ) {
					//
					if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) {
						//
						if ( _i_SCH_SUB_REMAIN_FORCE_IN_DELETE_FDHC( INDEX , INDEX + CM1 ) > 0 ) {
							//
							_i_SCH_SYSTEM_LASTING_SET( INDEX , TRUE ); // 2016.07.22
							//
							SCHMULTI_CHECK_HANDOFF_OUT_CHECK( FALSE , INDEX , -1 , INDEX + CM1 , 0 );
							//
						}
						else {
						}
						//---------------------------
						SCH_MR_UNLOAD_LOCK_END( INDEX );
						//---------------------------
						return;
						//
					}
					//
				}
				//
				if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 1 ) {
					//
					if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) { // 2013.07.10
						//
						LOADUNLOAD_MODE = 1;
						//
					}
					else {
						LOADUNLOAD_MODE = 1;
					}
					//
					SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , 10 ); // 2011.09.07
					//
					if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() != 7 ) {
						if ( STRCMP_L( Else2Str , "NL" ) || STRCMP_L( Else2Str , "NOLOOP" ) ) {
//							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
							//
//							FA_UNLOAD_CONTROLLER( INDEX , FALSE , TRUE , FALSE , FALSE , TRUE ); // 2016.10.31
							FA_UNLOAD_CONTROLLER( INDEX , FALSE , TRUE , FALSE , FALSE , 1 ); // 2016.10.31
						}
						else {
//							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
							FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
							//
//							FA_UNLOAD_CONTROLLER( INDEX , TRUE , TRUE , FALSE , FALSE , TRUE ); // 2016.10.31
							FA_UNLOAD_CONTROLLER( INDEX , TRUE , TRUE , FALSE , FALSE , 1 ); // 2016.10.31
						}
					}
					else {
//						FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2012.04.03 // 2013.07.10
						FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
						//
//						MANUAL_UNLOAD_CONTROLLER( INDEX+1 , -1 , TRUE , FALSE , FALSE , TRUE ); // 2012.07.10
//						MANUAL_UNLOAD_CONTROLLER( INDEX+1 , TRUE , FALSE , FALSE , TRUE ); // 2012.07.10 // 2016.10.31
						MANUAL_UNLOAD_CONTROLLER( INDEX+1 , TRUE , FALSE , FALSE , 1 ); // 2012.07.10 // 2016.10.31
					}
				}
			}
		}
		//---------------------------
		SCH_MR_UNLOAD_LOCK_END( INDEX ); // 2012.04.01
		//---------------------------
	}
	else if ( STRNCMP_L( RunStr , "MAKE_LOAD_REQUEST_CF" , 20 ) ) {
		FA_RUN = 1;
		//===================================================================================
		if ( STRCMP_L( RunStr + 20 , "(M)" ) ) SS = FALSE;	else SS = TRUE; // 2007.02.20
		//===================================================================================
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//===================================================================================
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//===================================================================================
//		if ( !Event_CM_Run_Possible_Check( INDEX , SS ) ) return; // 2016.07.15
//		if ( !Event_CM_Run_Possible_Check( INDEX , FALSE , SS ) ) return; // 2016.07.15 // 2016.10.05
		if ( !Event_CM_Run_Possible_Check( INDEX , FALSE , FALSE , SS ) ) return; // 2016.07.15 // 2016.10.05
		//===================================================================================
/*
// 2011.08.01
		if ( SCHEDULER_CM_LOCKED_for_HANDOFF( INDEX + CM1 ) > 0 ) return; // 2011.03.18
*/
		//===================================================================================
		if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
			Event_Message_Reject( "LOAD CONTROL RUN & WAITING ALREADY" );
			return;
		}
		if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
			Event_Message_Reject( "UNLOAD CONTROL RUN & WAITING" );
			return;
		}

		if ( BUTTON_GET_HANDOFF_CONTROL( INDEX + 1 ) != CTC_IDLE ) {
			Event_Message_Reject( "PORT CAN NOT USE" );
			return;
		}
		if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() == 1 ) {
			Event_Message_Reject( "LOADING PARAMETER NOT USED" );
			return;
		}
		//
		if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) { // 2013.07.10
			LOADUNLOAD_MODE = 1;
		}
		else {
			LOADUNLOAD_MODE = 1;
		}
		//
//		FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
		FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_1_LOADING ); // 2011.09.07
		//
		if ( _beginthread( (void *) FA_LOAD_CONTROLLER_TH , 0 , (void *) INDEX ) == -1 ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
			//
			//----------------------------------------------------------------------------------------------------------------
			// 2004.08.18
			//----------------------------------------------------------------------------------------------------------------
			_IO_CIM_PRINTF( "THREAD FAIL FA_LOAD_CONTROLLER_TH(3) %d\n" , INDEX );
			//
			FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 0 );
			BUTTON_SET_FLOW_STATUS( INDEX , _MS_3_LOADFAIL );
			//----------------------------------------------------------------------------------------------------------------
		}
	}
	else if ( STRNCMP_L( RunStr , "MAKE_LOAD_REQUEST_F" , 19 ) ) {
		FA_RUN = 1;
		//===================================================================================
		if ( STRCMP_L( RunStr + 19 , "(M)" ) ) SS = FALSE;	else SS = TRUE; // 2007.02.20
		//===================================================================================
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//===================================================================================
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//===================================================================================
//		if ( !Event_CM_Run_Possible_Check( INDEX , SS ) ) return; // 2016.07.15
//		if ( !Event_CM_Run_Possible_Check( INDEX , FALSE , SS ) ) return; // 2016.07.15 // 2016.10.05
		if ( !Event_CM_Run_Possible_Check( INDEX , FALSE , FALSE , SS ) ) return; // 2016.07.15 // 2016.10.05
		//===================================================================================
/*
// 2011.08.01
		if ( SCHEDULER_CM_LOCKED_for_HANDOFF( INDEX + CM1 ) > 0 ) return; // 2011.03.18
*/
		//===================================================================================
		if ( FA_LOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
			Event_Message_Reject( "LOAD CONTROL RUN & WAITING ALREADY" );
			return;
		}
		if ( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( INDEX ) > 0 ) {
			Event_Message_Reject( "UNLOAD CONTROL RUN & WAITING" );
			return;
		}
		//---------------------------
		// Append 2001.12.17
//		if ( FA_MAPPING_SIGNAL_GET( INDEX ) == 2 ) { // 2008.03.19
		if ( FA_MAPPING_SIGNAL_GET( INDEX ) >= 2 ) { // 2008.03.19
			Event_Message_Reject( "MAPPING CONTROL RUN & WAITING" );
			return;
		}
		//---------------------------
		if ( BUTTON_GET_HANDOFF_CONTROL( INDEX + 1 ) != CTC_IDLE ) {
			Event_Message_Reject( "PORT CAN NOT USE" );
			return;
		}
		if ( _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP() == 1 ) {
			Event_Message_Reject( "LOADING PARAMETER NOT USED" );
			return;
		}

//		if ( FA_MAPPING_SIGNAL_GET( INDEX ) == 2 ) { // 2008.03.19
		if ( FA_MAPPING_SIGNAL_GET( INDEX ) >= 2 ) { // 2008.03.19
			Event_Message_Reject( "MAPPING CONTROL RUNNING" );
			return;
		}
		//
		if ( _i_SCH_SYSTEM_USING_RUNNING( INDEX ) ) { // 2013.07.10
			LOADUNLOAD_MODE = 1;
		}
		else {
			LOADUNLOAD_MODE = 1;
		}
		//
		if ( FA_LOADING_SIGNAL_GET( INDEX ) || ( FA_MAPPING_SIGNAL_GET( INDEX ) == 1 ) ) {
			//
//			FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
			FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_1_LOADING ); // 2011.09.07
			//
			if ( _beginthread( (void *) FA_LOAD_CONTROLLER_TH , 0 , (void *) (INDEX + 1000) ) == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL FA_LOAD_CONTROLLER_TH(4) %d\n" , INDEX + 1000 );
				//
				FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 0 );
				BUTTON_SET_FLOW_STATUS( INDEX , _MS_3_LOADFAIL );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		else {
			//
//			FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 1 ); // 2013.07.10
			FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , LOADUNLOAD_MODE ); // 2013.07.10
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_1_LOADING ); // 2011.09.07
			//
			if ( _beginthread( (void *) FA_LOAD_CONTROLLER_TH , 0 , (void *) INDEX ) == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( INDEX + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL FA_LOAD_CONTROLLER_TH(5) %d\n" , INDEX );
				//
				FA_LOAD_CONTROLLER_RUN_STATUS_SET( INDEX , 0 );
				BUTTON_SET_FLOW_STATUS( INDEX , _MS_3_LOADFAIL );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
	}
	else if ( STRCMP_L( RunStr , "GET_MODULE_INFO" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//====================================================================
		INDEX = Get_Module_ID_From_String( 0 , Else1Str , 0 );
		//====================================================================
		FA_INFO_MESSAGE( 0 , INDEX );
		// 0:NoUse 1:Use 2:Disable 3:DisableHW 4:EnablePM
	}
	else if ( STRNCMP_L( RunStr , "SET_MODULE_INFO" , 15 ) ) {
		if      ( STRCMP_L( RunStr + 15 , ""          ) ) { FA_RUN = 1; QA_RUN = TRUE; }
		else if ( STRCMP_L( RunStr + 15 , "_LOCAL"    ) ) { FA_RUN = 0; QA_RUN = FALSE; }
		else if ( STRCMP_L( RunStr + 15 , "_LOCAL(F)" ) ) { FA_RUN = 2; QA_RUN = FALSE; }
		else {
			Event_Message_Reject( "INVALIDE MESSAGE RECEIVED" );
			return;
		}
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//====================================================================
		// 2008.10.15
		//====================================================================
		LC = 0;
		SS = strlen( Else1Str );
		if ( SS > 3 ) {
			if ( STRCMP_L( Else1Str + ( SS - 3 ) , "(D)" ) ) {
				Else1Str[SS-3] = 0;
				LC = 1;
			}
		}
		//====================================================================
		INDEX = Get_Module_ID_From_String( 1 , Else1Str , 0 );
		//====================================================================
		if ( INDEX == 0 ) {
			Event_Message_Reject( "DATA INFORMATION ERROR" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 1 , ElseStr );
			return;
		}
		//====================================================================
		if      ( STRCMP_L( Else2Str , "DISABLE"					) ) ENDINDEX = 0;
		else if ( STRCMP_L( Else2Str , "ENABLE"						) ) ENDINDEX = 1;
		else if ( STRCMP_L( Else2Str , "DISABLEHW"					) ) ENDINDEX = 2;
		else if ( STRCMP_L( Else2Str , "ENABLEPM"					) ) ENDINDEX = 3;
		//
		else if ( STRCMP_L( Else2Str , "DISABLE.SA"					) ) ENDINDEX = 4;	// 2003.06.13
		else if ( STRCMP_L( Else2Str , "ENABLE.SA"					) ) ENDINDEX = 5;	// 2003.06.13
		else if ( STRCMP_L( Else2Str , "DISABLEHW.SA"				) ) ENDINDEX = 6;	// 2003.06.13
		else if ( STRCMP_L( Else2Str , "ENABLEPM.SA"				) ) ENDINDEX = 7;	// 2003.06.13
		//
		else if ( STRCMP_L( Else2Str , "ENABLEN"					) ) ENDINDEX = 8;	// 2003.07.21
		else if ( STRCMP_L( Else2Str , "ENABLEPMN"					) ) ENDINDEX = 9;	// 2003.07.21
		else if ( STRCMP_L( Else2Str , "ENABLEN.SA"					) ) ENDINDEX = 10;	// 2003.07.21
		else if ( STRCMP_L( Else2Str , "ENABLEPMN.SA"				) ) ENDINDEX = 11;	// 2003.07.21
		//
		else if ( STRCMP_L( Else2Str , "DISABLE.STOP"				) ) ENDINDEX = 12;	// 2003.08.07
		else if ( STRCMP_L( Else2Str , "DISABLE.SA.STOP"			) ) ENDINDEX = 13;	// 2003.08.07
		else if ( STRCMP_L( Else2Str , "DISABLE.STOP.ABORT"			) ) ENDINDEX = 14;	// 2003.08.09
		else if ( STRCMP_L( Else2Str , "DISABLE.SA.STOP.ABORT"		) ) ENDINDEX = 15;	// 2003.08.09
		else if ( STRCMP_L( Else2Str , "DISABLEHW.STOP.ABORT"		) ) ENDINDEX = 16;	// 2003.08.09
		else if ( STRCMP_L( Else2Str , "DISABLEHW.SA.STOP.ABORT"	) ) ENDINDEX = 17;	// 2003.08.09
		//
		else if ( STRCMP_L( Else2Str , "DISABLE.NOTUSE.ABORT"		) ) ENDINDEX = 18;	// 2003.08.13
		else if ( STRCMP_L( Else2Str , "DISABLE.SA.NOTUSE.ABORT"	) ) ENDINDEX = 19;	// 2003.08.13
		else if ( STRCMP_L( Else2Str , "DISABLEHW.NOTUSE.ABORT"		) ) ENDINDEX = 20;	// 2003.08.13
		else if ( STRCMP_L( Else2Str , "DISABLEHW.SA.NOTUSE.ABORT"	) ) ENDINDEX = 21;	// 2003.08.13
		//
		else if ( STRCMP_L( Else2Str , "DISABLE.ABORT"				) ) ENDINDEX = 22;	// 2004.05.19
		else if ( STRCMP_L( Else2Str , "DISABLEHW.ABORT"			) ) ENDINDEX = 23;	// 2004.05.19
		//
		else if ( STRCMP_L( Else2Str , "ENABLE.SKIP"				) ) ENDINDEX = 24;	// 2006.07.10
		else if ( STRCMP_L( Else2Str , "ENABLEPM.SKIP"				) ) ENDINDEX = 25;	// 2006.07.10
		//
		else if ( STRCMP_L( Else2Str , "ENABLEN.SKIP"				) ) ENDINDEX = 26;	// 2006.07.10
		else if ( STRCMP_L( Else2Str , "ENABLEPMN.SKIP"				) ) ENDINDEX = 27;	// 2006.07.10
		//
		else if ( STRCMP_L( Else2Str , "REMOVE"						) ) ENDINDEX = 28;	// 2006.09.06
		else if ( STRCMP_L( Else2Str , "REMOVEX"					) ) ENDINDEX = 29;	// 2011.07.15
		//
		else if ( STRCMP_L( Else2Str , "REMOVEF"					) ) ENDINDEX = 30;	// 2017.02.17
		//
		else if ( STRCMP_L( Else2Str , "ENABLENX"					) ) ENDINDEX = 31;	// 2007.08.13
		else if ( STRCMP_L( Else2Str , "ENABLEPMNX"					) ) ENDINDEX = 32;	// 2007.08.13
		else if ( STRCMP_L( Else2Str , "ENABLENX.SA"				) ) ENDINDEX = 33;	// 2007.08.13
		else if ( STRCMP_L( Else2Str , "ENABLEPMNX.SA"				) ) ENDINDEX = 34;	// 2007.08.13
		else if ( STRCMP_L( Else2Str , "ENABLENX.SKIP"				) ) ENDINDEX = 35;	// 2007.08.13
		else if ( STRCMP_L( Else2Str , "ENABLEPMNX.SKIP"			) ) ENDINDEX = 36;	// 2007.08.13
		//
		else {
			Event_Message_Reject( "DATA INFORMATION ERROR 2" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 2 , ElseStr );
			return;
		}
		if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
			Event_Message_Reject( "IMPOSSIBLE CHANGE MODULE INFORMATION(NOT SW CONTROL MODE)" );
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 3 , ElseStr );
			return;
		}
		//
		if ( ( ENDINDEX == 1 ) || ( ENDINDEX == 3 ) || ( ENDINDEX == 5 ) || ( ENDINDEX == 7 ) || ( ENDINDEX == 8 ) || ( ENDINDEX == 9 ) || ( ENDINDEX == 10 ) || ( ENDINDEX == 11 ) ) { // 2003.12.10
			if ( ( INDEX >= PM1 ) && ( ( INDEX - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) ) {
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					//
					/*
					// 2014.01.10
					//
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( INDEX , 1 ) > 0 ) && ( _i_SCH_MODULE_Get_PM_WAFER( INDEX , 0 ) <= 0 ) ) {
						//
						Event_Message_Reject( "IMPOSSIBLE CHANGE MODULE INFORMATION(METHOD EXIST AT MODULE)" );
						if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 6 , ElseStr );
						return;
						//
					}
					*/
					// 2014.01.10
					//
					for ( id = 0 ; id < _i_SCH_PRM_GET_MODULE_SIZE( INDEX ) ; id++ ) {
						//
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( INDEX , id+1 ) > 0 ) && ( _i_SCH_MODULE_Get_PM_WAFER( INDEX , id ) <= 0 ) ) {
							//
							Event_Message_Reject( "IMPOSSIBLE CHANGE MODULE INFORMATION(METHOD EXIST AT MODULE)" );
							if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 6 , ElseStr );
							return;
							//
						}
						//
					}
					//
				}
			}
		}
		//
		if ( INDEX == MAX_CHAMBER ) {
			switch( ENDINDEX ) {
			case 0 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 1 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 2 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 3 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 4 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 5 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 6 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 7 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 8 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 1 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 9 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 1 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 10 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 1 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 11 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 1 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 12 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 0 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 13 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 0 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 14 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 1 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 15 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 1 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 16 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 1 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 17 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 1 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 18 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 2 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 19 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 2 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 20 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 2 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 21 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 2 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
				//
			case 22 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 0 , 3 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 23 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( -1 , 1 , 3 , ABORTWAIT_FLAG_NONE , LC );
				break;
				//
			case 24 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 0 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 25 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 0 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 26 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 1 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 27 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 1 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 28 : // 2006.09.06
				break;
			case 29 : // 2011.07.15
				break;
			case 30 : // 2017.02.17
				break;

			case 31 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 2 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 32 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 2 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 33 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 2 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 34 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 2 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 35 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 2 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 36 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , TRUE , 2 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;


			}
			//----------------------------------------------
//			FA_MDL_STATUS_IO_SET( INDEX ); // 2007.07.23
			//----------------------------------------------
			if ( FA_RUN != 0 ) FA_INFO_MESSAGE( 0 , 0 );
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else if ( ( INDEX == AL ) || ( INDEX == IC ) || ( INDEX == F_AL ) || ( INDEX == F_IC ) ) {
			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 4 , ElseStr );
				return;
			}
			switch( ENDINDEX ) {
			case 0 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 1 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 2 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 3 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 0 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 4 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 5 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 6 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 7 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 0 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 8 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 1 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 9 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 1 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 10 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 1 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 11 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 1 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 12 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 13 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 14 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 1 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 15 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 1 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 16 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 1 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 17 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 1 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 18 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 2 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 19 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 2 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
			case 20 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 2 , ABORTWAIT_FLAG_WAIT , LC );
				break;
			case 21 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 2 , ABORTWAIT_FLAG_ABORTWAIT , LC );
				break;
				//
			case 22 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 3 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 23 :
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 3 , ABORTWAIT_FLAG_NONE , LC );
				break;
				//
			case 24 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 0 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 25 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 0 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 26 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 1 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 27 :	// 2006.07.10
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 1 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 28 : // 2006.09.06
				break;
			case 29 : // 2011.07.15
				break;
			case 30 : // 2017.02.17
				break;

			case 31 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 2 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 32 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 2 , ABORTWAIT_FLAG_NONE , LC );
				break;
			case 33 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 2 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 34 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 2 , ABORTWAIT_FLAG_ABORT , LC );
				break;
			case 35 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 2 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			case 36 :	// 2007.08.13
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 2 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
				break;
			}
			//----------------------------------------------
//			FA_MDL_STATUS_IO_SET( INDEX ); // 2007.07.23
			//----------------------------------------------
			if ( FA_RUN != 0 ) FA_INFO_MESSAGE( 0 , INDEX );
			//----------------------------------------------
			GUI_SAVE_PARAMETER_DATA( 1 );
			//----------------------------------------------
		}
		else {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( INDEX ) ) {
				Event_Message_Reject( "INVALID MODULE INFORMATION(NOT USABLE MODULE)" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 4 , ElseStr );
				return;
			}
			if ( QA_RUN && ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( INDEX ) == 2 ) ) { // Current Disable HW Status
				Event_Message_Reject( "INVALID MODULE STATUS(HW DISABLE MODE)" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 5 , ElseStr );
			}
			//else if ( QA_RUN && ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( INDEX ) == 3 ) ) { // Current Enable PM Status
			//	Event_Message_Reject( "INVALID MODULE STATUS(PM ENABLE MODE)" );
			//	if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 5 , ElseStr );
			//} 2002.02.27
			else if ( QA_RUN && ( ( ENDINDEX == 2 ) || ( ENDINDEX == 6 ) || ( ENDINDEX == 16 ) || ( ENDINDEX == 17 ) || ( ENDINDEX == 20 ) || ( ENDINDEX == 21 ) ) ) { // Set Disable HW Status
				Event_Message_Reject( "INVALID MODULE STATUS" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 5 , ElseStr );
			}
			else if ( QA_RUN && ( ( ENDINDEX == 3 ) || ( ENDINDEX == 7 ) || ( ENDINDEX == 9 ) || ( ENDINDEX == 11 ) ) ) { // Set Enable PM Status
				Event_Message_Reject( "INVALID MODULE STATUS" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 5 , ElseStr );
			}
			else if ( Get_RunPrm_RUNNING_CLUSTER() && ( ( ENDINDEX == 3 ) || ( ENDINDEX == 7 ) ) && ( _i_SCH_PRM_GET_MODULE_MODE_for_SW( INDEX ) == 1 ) ) { // Set Enable PM Status // 2005.07.18
				Event_Message_Reject( "INVALID CURRENT MODULE STATUS(ENABLE) for ENABLE PM MODE" );
				if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 1 , FA_INFO , 5 , ElseStr );
			}
			else {
				switch( ENDINDEX ) {
				case 0 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 1 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 0 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 2 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 0 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 3 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 0 , ABORTWAIT_FLAG_NONE , LC );
					break;
				//
				case 4 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				case 5 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 0 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				case 6 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 0 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				case 7 :
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 0 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				//
				case 8 :	// 2003.07.21
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 1 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 9 :	// 2003.07.21
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 1 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 10 :	// 2003.07.21
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 1 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				case 11 :	// 2003.07.21
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 1 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				//
				case 12 :	// 2003.08.07
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_WAIT , LC );
					break;
				case 13 :	// 2003.08.07
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 0 , ABORTWAIT_FLAG_ABORTWAIT , LC );
					break;
				//
				case 14 :	// 2003.08.09
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 3 , ABORTWAIT_FLAG_WAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				case 15 :	// 2003.08.09
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 3 , ABORTWAIT_FLAG_ABORTWAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				//
				case 16 :	// 2003.08.09
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 3 , ABORTWAIT_FLAG_WAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				case 17 :	// 2003.08.09
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 3 , ABORTWAIT_FLAG_ABORTWAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;

				case 18 :	// 2003.08.13
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 4 , ABORTWAIT_FLAG_WAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				case 19 :	// 2003.08.13
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 4 , ABORTWAIT_FLAG_ABORTWAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				//
				case 20 :	// 2003.08.13
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 4 , ABORTWAIT_FLAG_WAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				case 21 :	// 2003.08.13
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 4 , ABORTWAIT_FLAG_ABORTWAIT , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
					//
				case 22 : // 2004.05.19
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 0 , 3 , ABORTWAIT_FLAG_NONE , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
				case 23 : // 2004.05.19
					if ( !SCHEDULER_CONTROL_Remapping_Multi_Chamber_Disable( INDEX , 1 , 3 , ABORTWAIT_FLAG_NONE , LC ) ) {
						for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) _i_SCH_SYSTEM_MODE_ABORT_SET( SS );
					}
					break;
					//
				case 24 :	// 2006.07.10
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 0 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
					break;
				case 25 :	// 2006.07.10
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 0 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
					break;
				case 26 :	// 2006.07.10
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 1 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
					break;
				case 27 :	// 2006.07.10
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 1 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
					break;
				case 28 : // 2006.09.06
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( INDEX , LC , TRUE , FALSE );
					break;
				case 29 : // 2011.07.15
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( INDEX , LC , FALSE , FALSE );
					break;
				case 30 : // 2017.02.17
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Remove( INDEX , LC , TRUE , TRUE );
					break;

				case 31 :	// 2007.08.13
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 2 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 32 :	// 2007.08.13
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 2 , ABORTWAIT_FLAG_NONE , LC );
					break;
				case 33 :	// 2007.08.13
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 2 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				case 34 :	// 2007.08.13
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 2 , ABORTWAIT_FLAG_ABORT , LC );
					break;
				case 35 :	// 2007.08.13
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , FALSE , 2 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
					break;
				case 36 :	// 2007.08.13
					SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( INDEX , TRUE , 2 , ABORTWAIT_FLAG_MODULE_NEXT_SKIP , LC );
					break;
				//
				}
				//----------------------------------------------
//				FA_MDL_STATUS_IO_SET( INDEX ); // 2007.07.23
				//----------------------------------------------
				if ( FA_RUN != 0 ) FA_INFO_MESSAGE( 0 , INDEX );
				//----------------------------------------------
				GUI_SAVE_PARAMETER_DATA( 1 );
				//----------------------------------------------
			}
		}
		//-----------------------------------------------------------------------------------------------
//		if ( ENDINDEX != 28 ) { // 2006.09.06 // 2017.02.17
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) { // 2004.03.17
				if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) {
					if ( INDEX == 9999 ) {
						_i_SCH_LOG_LOT_PRINTF( SS , "Scheduler Module(all) Status Change to %s%cSCTLMDLSTS 0:%d:%s\n" , Else2Str , 9 , ENDINDEX , Else2Str );
					}
					else {
						_i_SCH_LOG_LOT_PRINTF( SS , "Scheduler Module(%s) Status Change to %s%cSCTLMDLSTS %d:%d:%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( INDEX ) , Else2Str , 9 , INDEX , ENDINDEX , Else2Str );
					}
				}
			}
//		} // 2017.02.17
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_PRIORITY" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		if      ( STRCMP_L( Else1Str , "PICK"  ) ) QA_RUN = 0;
		else if ( STRCMP_L( Else1Str , "PLACE" ) ) QA_RUN = 1;
		else if ( STRCMP_L( Else1Str , "GET"   ) ) QA_RUN = 0;
		else if ( STRCMP_L( Else1Str , "PUT"   ) ) QA_RUN = 1;
		else {
			Event_Message_Reject( "DATA INFORMATION ERROR" );
			return;
		}
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		INDEX = 0;
		if ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			if ( strlen( Else1Str + 4 ) <= 0 ) {
				ENDINDEX = 1;
			}
			else {
				ENDINDEX = 0;
				INDEX = atoi( Else1Str + 4 ) - 1 + CM1;
				if ( ( INDEX < CM1 ) || ( ( INDEX - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ) INDEX = -1;
			}
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			if ( strlen( Else1Str + 2 ) <= 0 ) {
				ENDINDEX = 1;
			}
			else {
				ENDINDEX = 0;
				INDEX = atoi( Else1Str + 2 ) - 1 + CM1;
				if ( ( INDEX < CM1 ) || ( ( INDEX - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ) INDEX = -1;
			}
		}
		else if ( STRNCMP_L( Else1Str , "PM" , 2 ) ) {
			if ( strlen( Else1Str + 2 ) <= 0 ) {
				ENDINDEX = 2;
			}
			else {
				ENDINDEX = 0;
				INDEX = atoi( Else1Str + 2 ) - 1 + PM1;
				if ( ( INDEX < PM1 ) || ( ( INDEX - PM1 + 1 ) > MAX_PM_CHAMBER_DEPTH ) ) INDEX = -1;
			}
		}
		else if ( STRNCMP_L( Else1Str , "BM" , 2 ) ) {
			if ( strlen( Else1Str + 2 ) <= 0 ) {
				ENDINDEX = 3;
			}
			else {
				ENDINDEX = 0;
				INDEX = atoi( Else1Str + 2 ) - 1 + BM1;
				if ( ( INDEX < BM1 ) || ( ( INDEX - BM1 + 1 ) > MAX_BM_CHAMBER_DEPTH ) ) INDEX = -1;
			}
		}
		else {
			INDEX = -1;
		}
		if ( INDEX < 0 ) {
			Event_Message_Reject( "DATA INFORMATION ERROR 2" );
			return;
		}
		switch( QA_RUN ) {
		case 0:
			switch( ENDINDEX ) {
			case 0 :
				STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
				if ( strlen( Else1Str ) > 0 ) {
					_i_SCH_PRM_SET_Getting_Priority( INDEX , atoi( Else1Str ) );
				}
				break;
			case 1 :
				SS = 0;
				while( strlen( ElseStr ) > 0 ) {
					STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
					if ( strlen( Else1Str ) > 0 ) {
						if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
							_i_SCH_PRM_SET_Getting_Priority( SS + CM1 , atoi( Else1Str ) );
						}
					}
					strcpy( ElseStr , Else2Str );
					SS++;
				}
				break;
			case 2 :
				SS = 0;
				while( strlen( ElseStr ) > 0 ) {
					STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
					if ( strlen( Else1Str ) > 0 ) {
						if ( ( SS >= 0 ) && ( SS < MAX_PM_CHAMBER_DEPTH ) ) {
							_i_SCH_PRM_SET_Getting_Priority( SS + PM1 , atoi( Else1Str ) );
						}
					}
					strcpy( ElseStr , Else2Str );
					SS++;
				}
				break;
			case 3 :
				SS = 0;
				while( strlen( ElseStr ) > 0 ) {
					STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
					if ( strlen( Else1Str ) > 0 ) {
						if ( ( SS >= 0 ) && ( SS < MAX_BM_CHAMBER_DEPTH ) ) {
							_i_SCH_PRM_SET_Getting_Priority( SS + BM1 , atoi( Else1Str ) );
						}
					}
					strcpy( ElseStr , Else2Str );
					SS++;
				}
				break;
			}
			break;
		case 1 :
			switch( ENDINDEX ) {
			case 0 :
				STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
				if ( strlen( Else1Str ) > 0 ) {
					_i_SCH_PRM_SET_Putting_Priority( INDEX , atoi( Else1Str ) );
				}
				break;
			case 1 :
				SS = 0;
				while( strlen( ElseStr ) > 0 ) {
					STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
					if ( strlen( Else1Str ) > 0 ) {
						if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
							_i_SCH_PRM_SET_Putting_Priority( SS + CM1 , atoi( Else1Str ) );
						}
					}
					strcpy( ElseStr , Else2Str );
					SS++;
				}
				break;
			case 2 :
				SS = 0;
				while( strlen( ElseStr ) > 0 ) {
					STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
					if ( strlen( Else1Str ) > 0 ) {
						if ( ( SS >= 0 ) && ( SS < MAX_PM_CHAMBER_DEPTH ) ) {
							_i_SCH_PRM_SET_Putting_Priority( SS + PM1 , atoi( Else1Str ) );
						}
					}
					strcpy( ElseStr , Else2Str );
					SS++;
				}
				break;
			case 3 :
				SS = 0;
				while( strlen( ElseStr ) > 0 ) {
					STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
					if ( strlen( Else1Str ) > 0 ) {
						if ( ( SS >= 0 ) && ( SS < MAX_BM_CHAMBER_DEPTH ) ) {
							_i_SCH_PRM_SET_Putting_Priority( SS + BM1 , atoi( Else1Str ) );
						}
					}
					strcpy( ElseStr , Else2Str );
					SS++;
				}
				break;
			}
			break;
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_PROCESS_ZONE" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		INDEX = 0;
		if ( STRNCMP_L( Else1Str , "PM" , 2 ) ) {
			if ( strlen( Else1Str + 2 ) <= 0 ) {
				ENDINDEX = 1;
			}
			else {
				ENDINDEX = 0;
				INDEX = atoi( Else1Str + 2 ) - 1 + PM1;
				if ( ( INDEX < PM1 ) || ( ( INDEX - PM1 + 1 ) > MAX_PM_CHAMBER_DEPTH ) ) INDEX = -1;
			}
		}
		else {
			INDEX = -1;
		}
		if ( INDEX < 0 ) {
			Event_Message_Reject( "DATA INFORMATION ERROR" );
			return;
		}
		switch( ENDINDEX ) {
		case 0 :
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
			if ( strlen( Else1Str ) > 0 ) {
				_i_SCH_PRM_SET_PRCS_TIME_WAIT_RANGE( INDEX , atoi( Else1Str ) );
			}
			break;
		case 1 :
			SS = 0;
			while( strlen( Else2Str ) > 0 ) {
				STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
				if ( strlen( Else1Str ) > 0 ) {
					if ( ( SS >= 0 ) && ( SS < MAX_PM_CHAMBER_DEPTH ) ) {
						_i_SCH_PRM_SET_PRCS_TIME_WAIT_RANGE( SS + PM1 , atoi( Else1Str ) );
					}
				}
				strcpy( Else2Str , ElseStr );
				SS++;
			}
			break;
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_PROCESSING" ) ) { // 2010.01.26	// SET_PROCESSING <LOTINDEX?>|<PM>|<RECIPE>|<LOTPRESKIPSIDE>
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
//		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) INDEX = -1;
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		//
		ENDINDEX = -1;
		if ( STRNCMP_L( Else1Str , "PM" , 2 ) ) {
			if ( strlen( Else1Str + 2 ) > 0 ) {
				ENDINDEX = atoi( Else1Str + 2 ) - 1 + PM1;
				if ( ( ENDINDEX < PM1 ) || ( ( ENDINDEX - PM1 + 1 ) > MAX_PM_CHAMBER_DEPTH ) ) ENDINDEX = -1;
			}
		}
		if ( ENDINDEX < 0 ) {
			Event_Message_Reject( "DATA INFORMATION ERROR" );
			return;
		}
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		if ( Else1Str[0] == 0 )	{
			//
			Event_Message_Reject( "RECIPE DATA INFORMATION ERROR" );
			//
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) { // 2016.12.07
				if ( _i_SCH_SYSTEM_USING_RUNNING( SS ) ) {
					//
					_i_SCH_LOG_LOT_PRINTF( SS , "PM SET Process Regist_Fail(1) at %s[?]%cPROCESS_SET_REGISTFAIL PM%d:::?:::1\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ENDINDEX ) , 9 , ENDINDEX - PM1 + 1 );
					//
				}
			}
			//
			return;
		}
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , DispStr , ElseStr , 255 ); // 2014.12.18
		//
		if ( DispStr[0] == 0 ) { // 2014.12.18
			QA_RUN = -1;
		}
		else { // 2014.12.18
			if ( !Event_LOT_ID_From_String_And_Check( DispStr , &QA_RUN ) ) QA_RUN = -1;
		}
		//
		//
		// 2018.06.22
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		//
		/*
		//
		// 2017.09.19
		//
		INDEX = -1;
		FA_RUN = -1; // 2016.12.07
		//
		for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) { // 2004.03.17
			if ( _i_SCH_SYSTEM_USING_RUNNING( SS ) ) {
				//
				if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( SS , ENDINDEX ) == MUF_01_USE ) {
					//
					FA_RUN = SS; // 2016.12.07
					//
					if ( !_i_SCH_MODULE_Chk_TM_Free_Status( SS ) ) { // 2010.05.06
						//
						INDEX = SS;
						//
						break;
					}
				}
			}
		}
		//
		if ( INDEX == -1 ) {
			//
			// INDEX = 0; // 2016.12.07
			INDEX = ( FA_RUN == -1 ) ? 0 : FA_RUN; // 2016.12.07
			//
			for ( SS = 0 ; SS < 2 ; SS++ ) {
				//
				if ( EQUIP_STATUS_PROCESS( INDEX , ENDINDEX ) == SYS_RUNNING ) {
					//
					_i_SCH_LOG_LOT_PRINTF( INDEX , "PM SET Process Append at %s[%s]%cPROCESS_SET_START PM%d:::%s:::103\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ENDINDEX ) , Else1Str , 9 , ENDINDEX - PM1 + 1 , Else1Str ); // 2016.11.03
					//
					if ( !_i_SCH_EQ_SPAWN_EVENT_PROCESS( INDEX , -1 , ENDINDEX , 0 , 0 , 0 , Else1Str , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) continue;
					//
					if ( QA_RUN != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( QA_RUN , ENDINDEX ); // 2014.12.18
					//
					Scheduler_PreCheck_for_ProcessRunning_Part( INDEX , ENDINDEX );
					//
					break;
				}
				else {
					//
					_i_SCH_LOG_LOT_PRINTF( INDEX , "PM SET Process Start at %s[%s]%cPROCESS_SET_START PM%d:::%s:::104\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ENDINDEX ) , Else1Str , 9 , ENDINDEX - PM1 + 1 , Else1Str ); // 2016.11.03
					//
					if ( !_i_SCH_EQ_SPAWN_PROCESS( INDEX , -1 , ENDINDEX , 0 , 0 , 0 , Else1Str , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) continue;
					//
					if ( QA_RUN != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( QA_RUN , ENDINDEX ); // 2014.12.18
					//
					Scheduler_PreCheck_for_ProcessRunning_Part( INDEX , ENDINDEX );
					//
					break;
				}
				//
			}
		}
		else {
			//
			_i_SCH_LOG_LOT_PRINTF( INDEX , "PM SET Process Regist at %s[%s]%cPROCESS_SET_REGIST PM%d:::%s:::%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ENDINDEX ) , Else1Str , 9 , ENDINDEX - PM1 + 1 , Else1Str , QA_RUN ); // 2016.12.07
			//
//			SIGNAL_MODE_FMTMSIDE_SET( ENDINDEX , INDEX , Else1Str ); // 2014.12.18
			SIGNAL_MODE_FMTMSIDE_SET( ENDINDEX , INDEX , Else1Str , QA_RUN ); // 2014.12.18
			//
		}
		//
		*/
		//
		// 2017.09.19
		//
		_i_SCH_SUB_SET_PROCESSING( -1 , ENDINDEX , Else1Str , QA_RUN , atoi( Else1Str ) ); // 2018.06.22
		//
		//
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "CASSETTE" , 8 ) ) {
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		//
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , JobStr , 255 );
		//
		if      ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
			QA_RUN = 0;
			INDEX = atoi( Else1Str + 4 ) - 1 + CM1;
			if ( ( INDEX < CM1 ) || ( ( INDEX - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ) INDEX = -1;
		}
		else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
			QA_RUN = 0;
			INDEX = atoi( Else1Str + 2 ) - 1 + CM1;
			if ( ( INDEX < CM1 ) || ( ( INDEX - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ) INDEX = -1;
		}
		else if ( STRCMP_L( Else1Str , "BMALL" ) ) {
			QA_RUN = 2;
			INDEX = 0;
		}
		else if ( STRNCMP_L( Else1Str , "BM" , 2 ) ) {
			QA_RUN = 1;
			INDEX = atoi( Else1Str + 2 ) - 1 + BM1;
			if ( ( INDEX < BM1 ) || ( ( INDEX - BM1 + 1 ) > MAX_BM_CHAMBER_DEPTH ) ) INDEX = -1;
		}
		else if ( STRCMP_L( Else1Str , "PMALL" ) ) {
			QA_RUN = 4;
			INDEX = 0;
		}
		else if ( STRNCMP_L( Else1Str , "PM" , 2 ) ) {
			QA_RUN = 3;
			INDEX = atoi( Else1Str + 2 ) - 1 + PM1;
			if ( ( INDEX < PM1 ) || ( ( INDEX - PM1 + 1 ) > MAX_PM_CHAMBER_DEPTH ) ) INDEX = -1;
		}
		else if ( STRCMP_L( Else1Str , "TMALL" ) ) {
			QA_RUN = 5;
			INDEX = 0;
		}
		else if ( STRCMP_L( Else1Str , "FMALL" ) ) {
			QA_RUN = 6;
			INDEX = 0;
		}
		else if ( STRCMP_L( Else1Str , "ALL" ) ) {
			QA_RUN = 7;
			INDEX = 0;
		}
		else if ( STRCMP_L( Else1Str , "SIM" ) ) {
			QA_RUN = 8;
			INDEX = 0;
		}
		else if ( STRCMP_L( Else1Str , "SIMA" ) ) {
			QA_RUN = 9;
			INDEX = 0;
		}
		else {
			QA_RUN = -1;
			INDEX = -1;
		}
		//===========================================================================
		if ( ( QA_RUN != 8 ) && ( QA_RUN != 9 ) ) {
			if      ( STRCMP_L( Else2Str , "SET"   ) ) ENDINDEX = 2;
			else if ( STRCMP_L( Else2Str , "RESET" ) ) ENDINDEX = 1;
			else if ( STRCMP_L( Else2Str , "SWAP"  ) ) ENDINDEX = 3;
			else                                       ENDINDEX = -1;
		}
		else {
			ENDINDEX = 0;
		}
		//===========================================================================
		if ( ( INDEX == -1 ) || ( ENDINDEX == -1 ) ) {
			Event_Message_Reject( "PORT/DATA ANALYSIS ERROR" );
			return;
		}
		//===========================================================================
		if ( QA_RUN != 0 ) {
			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				return;
			}
		}
		//===========================================================================
		if ( ( QA_RUN == 0 ) || ( QA_RUN == 1 ) || ( QA_RUN == 3 ) ) {
			if ( !_i_SCH_MODULE_GET_USE_ENABLE( INDEX , FALSE , -1 ) ) {
				Event_Message_Reject( "DISABLE PORT" );
				return;
			}
		}
		//===========================================================================
		if ( QA_RUN == 0 ) {
			//
			if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.01
			}
			else {
				if ( _i_SCH_SYSTEM_USING_GET( INDEX - 1 ) > 0 ) {
					Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
					return;
				}
			}
			//
			if ( BUTTON_GET_HANDOFF_CONTROL( INDEX ) != CTC_IDLE ) {
				Event_Message_Reject( "PORT CAN NOT USE" );
				return;
			}
			//
			if ( ENDINDEX == 3 ) { // swap // 2019.02.28
				//
				SS = -1;
				if      ( STRNCMP_L( JobStr , "PORT" , 4 ) ) {
					SS = atoi( JobStr + 4 ) - 1 + CM1;
					if ( ( SS < CM1 ) || ( ( SS - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ) SS = -1;
				}
				else if ( STRNCMP_L( JobStr , "CM" , 2 ) ) {
					SS = atoi( JobStr + 2 ) - 1 + CM1;
					if ( ( SS < CM1 ) || ( ( SS - CM1 + 1 ) > MAX_CASSETTE_SIDE ) ) SS = -1;
				}
				//
				if ( SS != -1 ) {
					//
					if ( INDEX == SS ) {
						Event_Message_Reject( "PORT SAME" );
						return;
					}
					if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) {
					}
					else {
						if ( _i_SCH_SYSTEM_USING_GET( SS - 1 ) > 0 ) {
							Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
							return;
						}
					}
					//
					if ( BUTTON_GET_HANDOFF_CONTROL( SS ) != CTC_IDLE ) {
						Event_Message_Reject( "PORT CAN NOT USE" );
						return;
					}
					//
					WAFER_MSWAP_CM( INDEX , SS );
					//
				}
				//
			}
			else {
				WAFER_MSET_CM( INDEX , ENDINDEX );
			}
			//
		}
		else if ( QA_RUN == 1 ) {
			WAFER_MSET_BM( INDEX , ENDINDEX-1 );
		}
		else if ( QA_RUN == 2 ) {
			WAFER_MSET_BM( -1 , ENDINDEX-1 );
		}
		else if ( QA_RUN == 3 ) {
			WAFER_MSET_PM( INDEX , ENDINDEX-1 );
		}
		else if ( QA_RUN == 4 ) {
			WAFER_MSET_PM( -1 , ENDINDEX-1 );
		}
		else if ( QA_RUN == 5 ) {
			WAFER_MSET_TM( ENDINDEX-1 );
		}
		else if ( QA_RUN == 6 ) {
			WAFER_MSET_FM( ENDINDEX-1 );
		}
		else if ( QA_RUN == 7 ) {	// 2004.08.13
			WAFER_MSET_FM( ENDINDEX-1 );
			WAFER_MSET_TM( ENDINDEX-1 );
			WAFER_MSET_PM( -1 , ENDINDEX-1 );
			WAFER_MSET_BM( -1 , ENDINDEX-1 );
		}
		else if ( ( QA_RUN == 8 ) || ( QA_RUN == 9 ) ) {	// 2005.09.07
			for ( INDEX = 0 ; INDEX < MAX_CASSETTE_SIDE ; INDEX++ ) {
				if ( _i_SCH_MODULE_GET_USE_ENABLE( INDEX + CM1 , FALSE , -1 ) ) {
					if ( BUTTON_GET_HANDOFF_CONTROL( INDEX + CM1 ) == CTC_IDLE ) {
						WAFER_MSET_CM( INDEX + CM1 , 2 );
					}
				}
			}
			//
			WAFER_MSET_FM( 0 );
			WAFER_MSET_TM( 0 );
			WAFER_MSET_PM( -1 , 0 );
			WAFER_MSET_BM( -1 , 0 );
			//
			if ( QA_RUN == 9 ) { // 2012.12.06
				SCHEDULER_CONTROL_Remapping_Multi_Chamber_Enable( -1 , FALSE , 0 , ABORTWAIT_FLAG_NONE , 0 );
			}
			//
			while ( TRUE ) { // 2016.04.28
				//
				strcpy( ElseStr , Else2Str );
				//
				if ( ElseStr[0] == 0 ) break;
				//
				STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
				//
				if      ( STRNCMP_L( Else1Str , "PORT" , 4 ) ) {
					INDEX = atoi( Else1Str + 4 ) - 1 + CM1;
					if ( ( INDEX >= CM1 ) && ( ( INDEX - CM1 + 1 ) <= MAX_CASSETTE_SIDE ) ) WAFER_MSET_CM( INDEX , 1 );
				}
				else if ( STRNCMP_L( Else1Str , "CM" , 2 ) ) {
					INDEX = atoi( Else1Str + 2 ) - 1 + CM1;
					if ( ( INDEX >= CM1 ) && ( ( INDEX - CM1 + 1 ) <= MAX_CASSETTE_SIDE ) ) WAFER_MSET_CM( INDEX , 1 );
				}
				else if ( STRNCMP_L( Else1Str , "BM" , 2 ) ) {
					INDEX = atoi( Else1Str + 2 ) - 1 + BM1;
					if ( ( INDEX >= BM1 ) && ( ( INDEX - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) ) WAFER_MSET_BM( INDEX , 1 );
				}
				else if ( STRNCMP_L( Else1Str , "PM" , 2 ) ) {
					INDEX = atoi( Else1Str + 2 ) - 1 + PM1;
					if ( ( INDEX >= PM1 ) && ( ( INDEX - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) ) WAFER_MSET_PM( INDEX , 1 );
				}
			}
		}
	}
	else if ( STRNCMP_L( RunStr , "ROBOT_HOME" , 10 ) ) {
		if ( STRCMP_L( ElseStr , "" ) ) {
			//
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND , 0 );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND2 , 0 );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_ROTATE , _i_SCH_PRM_GET_RB_FM_HOME_POSITION( 0 ) );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_UPDOWN , 0 );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_MOVE , 0 );
			//
			for ( SS = 0 ; SS < MAX_TM_CHAMBER_DEPTH ; SS++ ) {
				ROBOT_SET_EXTEND_POSITION( SS , 0 , 0 );
				ROBOT_SET_EXTEND_POSITION( SS , 1 , 0 );
				ROBOT_SET_ROTATE_POSITION( SS , _i_SCH_PRM_GET_RB_HOME_POSITION( SS ) ) ;
				ROBOT_SET_UPDOWN_POSITION( SS , 0 );
				ROBOT_SET_MOVE_POSITION( SS , 0 );
			}
		}
		else if ( STRNCMP_L( ElseStr , "TM" , 2 ) ) {
			SS = atoi( ElseStr + 2 );
			if ( ( SS >= 0 ) && ( SS < MAX_TM_CHAMBER_DEPTH ) ) {
				ROBOT_SET_EXTEND_POSITION( SS , 0 , 0 );
				ROBOT_SET_EXTEND_POSITION( SS , 1 , 0 );
				ROBOT_SET_ROTATE_POSITION( SS , _i_SCH_PRM_GET_RB_HOME_POSITION( SS ) );
				ROBOT_SET_UPDOWN_POSITION( SS , 0 );
				ROBOT_SET_MOVE_POSITION( SS , 0 );
			}
		}
		else if ( STRCMP_L( ElseStr , "FM" ) ) {
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND , 0 );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND2 , 0 );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_ROTATE , _i_SCH_PRM_GET_RB_FM_HOME_POSITION( 0 ) );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_UPDOWN , 0 );
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_MOVE , 0 );
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "PROCESS_ITEM_CHANGE" , 19 ) ) {
		FA_RUN = 0;
		if ( STRCMP_L( RunStr + 19 , "_F"  ) ) FA_RUN = 1;
		INDEX = -1;
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		switch( Get_PMBM_ID_From_String( 1 , Else1Str , 0 , &INDEX ) ) {
		case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
		case 2 :	Event_Message_Reject( "Disable Module Data" );	return;	break;
		}
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		ENDINDEX = atoi( Else1Str );
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		LC = atoi( Else1Str );
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
		STR_SEPERATE_CHAR( ElseStr , '|' , Else2Str , DispStr , 255 );
		//
		switch ( EQUIP_PROCESS_ITEM_CHANGE_EVENT( 0 , INDEX , ENDINDEX , LC , Else1Str , Else2Str , DispStr ) ) {
		case SYS_SUCCESS :
			if ( FA_RUN != 0 ) FA_ACCEPT_MESSAGE( 0 , FA_ITEM_CHANGE , INDEX );
			break;
		default :
			if ( FA_RUN != 0 ) FA_REJECT_MESSAGE( 0 , FA_ITEM_CHANGE , INDEX , "" );
			break;
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_CONFIG" ) ) { // SET_CONFIG {Module}|<STYLE}|<Data>
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , MidStr , 255 );
		STR_SEPERATE_CHAR( MidStr  , '|' , Else2Str , DispStr , 255 );
		//====================================================================
		INDEX = Get_Module_ID_From_String( 2 , Else1Str , 0 );
		//====================================================================
		if ( INDEX <= 0 ) {
			Event_Message_Reject( "DATA ANALYSIS ERROR" );
			return;
		}
		//====================================================================
		if      ( STRCMP_L( Else2Str , "MAPPING"         ) ) ENDINDEX = 0;
		else if ( STRCMP_L( Else2Str , "HANDOFF"         ) ) ENDINDEX = 1;
		else if ( STRCMP_L( Else2Str , "RUNTIME_IN"      ) ) ENDINDEX = 2;
		else if ( STRCMP_L( Else2Str , "RUNTIME_OUT"     ) ) ENDINDEX = 3;
		else if ( STRCMP_L( Else2Str , "DEFAULT_SIDE"    ) ) ENDINDEX = 4;
		else if ( STRCMP_L( Else2Str , "START_OPTION"    ) ) ENDINDEX = 5;
		else if ( STRCMP_L( Else2Str , "SWITCH_SIDE"     ) ) ENDINDEX = 6;
		else if ( STRCMP_L( Else2Str , "JOBLOGBASED"     ) ) ENDINDEX = 7; // 2002.12.30
		else if ( STRCMP_L( Else2Str , "CARRIER_GROUP"   ) ) ENDINDEX = 10; // 2003.02.05
		else if ( STRCMP_L( Else2Str , "DUMMY_MULTI"     ) ) ENDINDEX = 11; // 2003.02.14	=> No I/O
		else if ( STRCMP_L( Else2Str , "WAFER_SUPPLY"    ) ) ENDINDEX = 12; // 2003.06.12
		else if ( STRCMP_L( Else2Str , "CLUSTER_INCLUDE" ) ) ENDINDEX = 13; // 2005.08.02
		else if ( STRCMP_L( Else2Str , "COOLING_TIME"    ) ) ENDINDEX = 14; // 2005.11.29
		else if ( STRCMP_L( Else2Str , "COOLTIME_MODE"   ) ) ENDINDEX = 15; // 2005.12.01	=> No I/O
		else if ( STRCMP_L( Else2Str , "NEXTPICK_FREE"   ) ) ENDINDEX = 16; // 2006.07.21	=> No I/O
		else if ( STRCMP_L( Else2Str , "GET_PR_OPTION"   ) ) ENDINDEX = 17; // 2006.11.16	=> No I/O
		else if ( STRCMP_L( Else2Str , "PUT_PR_OPTION"   ) ) ENDINDEX = 18; // 2006.11.16	=> No I/O
		else if ( STRCMP_L( Else2Str , "CM_SUPPLY"       ) ) ENDINDEX = 19; // 2006.11.23	=> No I/O
		else if ( STRCMP_L( Else2Str , "ALIGN_BUFFER"    ) ) ENDINDEX = 20; // 2007.04.04	=> No I/O
		else if ( STRCMP_L( Else2Str , "LOT_SUPPLY"      ) ) ENDINDEX = 21; // 2007.10.15
		else if ( STRCMP_L( Else2Str , "CPJOB_MODE"      ) ) ENDINDEX = 22; // 2008.09.29
		else if ( STRCMP_L( Else2Str , "COOLING_TIME2"   ) ) ENDINDEX = 23; // 2011.04.27
		else if ( STRCMP_L( Else2Str , "MTLOUT"          ) ) ENDINDEX = 24; // 2013.04.17	=> No I/O
		else if ( STRCMP_L( Else2Str , "RECIPE_LOCKING"  ) ) ENDINDEX = 25; // 2013.09.04	=> No I/O
		else if ( STRCMP_L( Else2Str , "CHINTLKS_PICK"   ) ) ENDINDEX = 26; // 2013.11.21
		else if ( STRCMP_L( Else2Str , "CHINTLKS_PLACE"  ) ) ENDINDEX = 27; // 2013.11.21
		else if ( STRCMP_L( Else2Str , "DOUBLE_SIDE"     ) ) ENDINDEX = 28; // 2015.05.28
		else if ( STRCMP_L( Else2Str , "ANIM_CHECK"      ) ) ENDINDEX = 29; // 2016.12.09
		else {
			ENDINDEX = -1;
		}
		if ( ENDINDEX == -1 ) {
			Event_Message_Reject( "DATA ANALYSIS ERROR" );
			return;
		}
		if ( INDEX != MAX_CHAMBER ) {
//			if ( ENDINDEX != 20 ) { // 2007.04.04 // 2013.12.18
//			if ( ( ENDINDEX != 20 ) && ( ENDINDEX != 26 ) && ( ENDINDEX != 27 ) ) { // 2007.04.04 // 2013.12.18 // 2015.05.28
			if ( ( ENDINDEX != 20 ) && ( ENDINDEX != 26 ) && ( ENDINDEX != 27 ) && ( ENDINDEX != 28 ) ) { // 2007.04.04 // 2013.12.18 // 2015.05.28
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( INDEX , FALSE , -1 ) ) {
					Event_Message_Reject( "DISABLE PORT" );
					return;
				}
			}
		}
		if ( ENDINDEX == 29 ) { // 2016.12.09
			//
			_i_SCH_PRM_SET_WAFER_ANIM_STATUS_CHECK( atoi( DispStr ) );
			//
		}
		else if ( ENDINDEX == 24 ) { // 2013.04.17
			//
			_i_SCH_PRM_SET_MTLOUT_INTERFACE( atoi( DispStr ) );
			//
			FILE_PARAM_SM_SCHEDULER_SETTING2( 2 , 0 ); // 2013.09.04
			//
		}
		else if ( ENDINDEX == 25 ) { // 2013.09.04
			//
			_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING_MODE( TRUE ); // 2013.09.13
			_i_SCH_PRM_SET_DFIX_RECIPE_LOCKING( atoi( DispStr ) );
			//
			GUI_SAVE_PARAMETER_DATA( 1 ); // 2013.09.13
			//
		}
		else if ( ( ENDINDEX == 26 ) || ( ENDINDEX == 27 ) ) { // 2013.11.21
			//
			STR_SEPERATE_CHAR( DispStr , '|' , Else1Str , Else2Str , 255 );
			//
			Set_RunPrm_Config_Change_for_ChIntlks( ( ENDINDEX == 26 ) ? MACRO_PICK : MACRO_PLACE , atoi( Else1Str ) , INDEX , atoi( Else2Str ) );
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//
			Set_RunPrm_Config_Change_for_ChIntlks_IO();
			//
		}
		else {
	//		if ( ( ENDINDEX == 7 ) || ( ENDINDEX == 17 ) || ( ENDINDEX == 18 ) || ( ENDINDEX == 19 ) || ( ENDINDEX == 20 ) || ( ENDINDEX == 21 ) ) { // 2003.02.14 // 2008.06.24
			if ( ( ENDINDEX == 7 ) || ( ENDINDEX == 17 ) || ( ENDINDEX == 18 ) || ( ENDINDEX == 19 ) || ( ENDINDEX == 20 ) ) { // 2003.02.14 // 2008.06.24
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO SYSTEM RUNNING ALREADY" );
					return;
				}
			}
	//		if ( _i_SCH_SYSTEM_USING_GET( TR_CHECKING_SIDE ) > 0 ) {
	//			Event_Message_Reject( "TRANSFER SYSTEM IS RUNNING ALREADY" );
	//			return;
	//		}
	//		else {
	//			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) {
	//				if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) {
	//					Event_Message_Reject( "AUTO SYSTEM IS RUNNING ALREADY" );
	//					return;
	//				}
	//			}
	//		}
			Set_RunPrm_Config_Change( ENDINDEX , INDEX , atoi( DispStr ) );
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//
			Set_RunPrm_IO_Setting( ENDINDEX );
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "SET_TM_ARM_STYLE" , 16 ) ) { // 2014.11.20
		//
		if ( !STRCMP_L( RunStr + 16 , "_LOCAL" ) ) {
			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				return;
			}
		}
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , MidStr , 255 );
		//
		if ( MidStr[0] == 0 ) {
			INDEX = 0;
		}
		else {
			INDEX = atoi( MidStr ) - 1;
		}
		//
		Set_RunPrm_Config_Change( 24 , INDEX , atoi( Else1Str ) );
		//
		GUI_SAVE_PARAMETER_DATA( 2 );
		//
		Set_RunPrm_IO_Setting( 24 );
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "SET_FM_ARM_STYLE" , 16 ) ) {
		if ( !STRCMP_L( RunStr + 16 , "_LOCAL" ) ) {
			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				return;
			}
		}
		//
		Set_RunPrm_Config_Change( 9 , 0 , atoi( ElseStr ) );
		//
		GUI_SAVE_PARAMETER_DATA( 2 );
		//
		Set_RunPrm_IO_Setting( 9 );
	}
	else if ( STRCMP_L( RunStr , "SET_FM_INTERLOCK" ) ) { // 2006.11.16
		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
			return;
		}
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		if      ( STRCMP_L( Else1Str , "PICK"  ) ) QA_RUN = 0;
		else if ( STRCMP_L( Else1Str , "PLACE" ) ) QA_RUN = 1;
		else if ( STRCMP_L( Else1Str , "GET"   ) ) QA_RUN = 0;
		else if ( STRCMP_L( Else1Str , "PUT"   ) ) QA_RUN = 1;
		//
		SS = 0;
		while( strlen( Else2Str ) > 0 ) {
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );
			if ( strlen( Else1Str ) > 0 ) {
				INDEX = atoi( Else1Str );
				if ( ( INDEX < 0 ) || ( INDEX > 3 ) ) continue;
				switch( SS ) {
				case 0 : // CM
					for ( ENDINDEX = 0 ; ENDINDEX < MAX_CASSETTE_SIDE ; ENDINDEX++ ) {
						if ( QA_RUN == 0 )	_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( CM1 + ENDINDEX , INDEX );
						else				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( CM1 + ENDINDEX , INDEX );
					}
					break;
				case 1 : // BM
					for ( ENDINDEX = 0 ; ENDINDEX < MAX_BM_CHAMBER_DEPTH ; ENDINDEX++ ) {
						if ( QA_RUN == 0 )	_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( BM1 + ENDINDEX , INDEX );
						else				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( BM1 + ENDINDEX , INDEX );
					}
					break;
				case 2 : // AL
					if ( QA_RUN == 0 )	_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL , INDEX );
					else				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL , INDEX );
					break;
				case 3 : // IC
					if ( QA_RUN == 0 )	_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC , INDEX );
					else				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC , INDEX );
					break;
				}
			}
			strcpy( Else2Str , ElseStr );
			SS++;
		}
		//
		GUI_SAVE_PARAMETER_DATA( 1 );
	}
	else if ( STRCMP_L( RunStr , "SET_FM_ALIGNER" ) ) { // 2006.11.16
		/*
		// 2014.03.28
		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
			return;
		}
		*/
		//
		QA_RUN = -1;
		SS = 0;
		ENDINDEX = 0; // 2014.03.28
		//
		/*
		// 2014.03.27
		if      ( STRCMP_L( ElseStr , "THREAD" ) ) QA_RUN = 0;
		else if ( STRCMP_L( ElseStr , "DEPAND" ) ) QA_RUN = 1;
		//
		*/
		//
		// 2014.03.27
		//
		if ( STRCMP_L( ElseStr , "THREADA" ) ) { // 2014.03.28
			QA_RUN = 0;
			SS = 1;
			ENDINDEX = 1;
		}
		else if ( STRCMP_L( ElseStr , "DEPANDA" ) ) { // 2014.03.28
			QA_RUN = 1;
			SS = 1;
			ENDINDEX = 1;
		}
		else if ( STRNCMP_L( ElseStr , "THREADA" , 7 ) ) { // 2014.03.28
			QA_RUN = 0;
			SS = atoi( ElseStr + 7 );
			ENDINDEX = 1;
		}
		else if ( STRNCMP_L( ElseStr , "DEPANDA" , 7 ) ) { // 2014.03.28
			QA_RUN = 1;
			SS = atoi( ElseStr + 7 );
			ENDINDEX = 1;
		}
		//--------------------------------------------------------------------------
		else if ( STRCMP_L( ElseStr , "THREAD" ) ) {
			QA_RUN = 0;
			SS = 1;
		}
		else if ( STRCMP_L( ElseStr , "DEPAND" ) ) {
			QA_RUN = 1;
			SS = 1;
		}
		else if ( STRNCMP_L( ElseStr , "THREAD" , 6 ) ) {
			QA_RUN = 0;
			SS = atoi( ElseStr + 6 );
		}
		else if ( STRNCMP_L( ElseStr , "DEPAND" , 6 ) ) {
			QA_RUN = 1;
			SS = atoi( ElseStr + 6 );
		}
		//
		// 2014.03.28
		if ( ENDINDEX == 0 ) {
			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				return;
			}
		}
		//
		if ( ( SS <= 0 ) || ( SS > 2 ) ) {
			Event_Message_Reject( "MESSAGE ERROR" );
		}
		else {
			//
			if      ( QA_RUN == 0 ) {
				if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 2 ) {
					_i_SCH_PRM_SET_MODULE_SIZE( F_AL , SS ); // 2014.03.27
					_i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( 3 );
				}
			}
			else if ( QA_RUN == 1 ) {
				if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) {
					_i_SCH_PRM_SET_MODULE_SIZE( F_AL , SS ); // 2014.03.27
					_i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( 2 );
				}
			}
			else {
				Event_Message_Reject( "MESSAGE ERROR" );
			}
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "MAINT_INTERFACE" , 15 ) ) {
		//==========================================================
		Maint_Intf_Spawn( TRUE , RunStr , ElseStr );
		//==========================================================

/*
		//==========================================================
		if ( STRNCMP_L( RunStr + 15 , "_A" , 2 ) ) { // 2008.09.04
			SS = 2;
		}
		else {
			SS = 0;
		}
		//==========================================================
		ENDINDEX = 0;
		if      ( RunStr[ 15 + SS ] == 0 ) {
			LC = 0;
		}
		else if ( RunStr[ 15 + SS ] == 'F' ) {
			LC = MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH;
			if      ( STRCMP_L( RunStr + 16 + SS , "(C)" ) ) ENDINDEX = 1; // Enable:Wafer
			else if ( STRCMP_L( RunStr + 16 + SS , "(W)" ) ) ENDINDEX = 2; // Wafer
			else if ( STRCMP_L( RunStr + 16 + SS , "(M)" ) ) ENDINDEX = 3; // Enable
			else if ( STRCMP_L( RunStr + 16 + SS , "(D)" ) ) ENDINDEX = -1; // DBSET_Only // 2012.11.07
		}
		else {
			if ( ( RunStr[ 15 + SS ] >= '1' ) && ( RunStr[ 15 + SS ] <= '9' ) ) {
				LC = RunStr[ 15 + SS ] - '1';
				if ( LC >= MAX_TM_CHAMBER_DEPTH ) LC = 0;
				if      ( STRCMP_L( RunStr + 16 + SS , "(C)" ) ) ENDINDEX = 1;
				else if ( STRCMP_L( RunStr + 16 + SS , "(W)" ) ) ENDINDEX = 2;
				else if ( STRCMP_L( RunStr + 16 + SS , "(M)" ) ) ENDINDEX = 3;
				else if ( STRCMP_L( RunStr + 16 + SS , "(D)" ) ) ENDINDEX = -1; // 2012.11.07
			}
			else if ( ( RunStr[ 15 + SS ] >= 'A' ) && ( RunStr[ 15 + SS ] <= 'Z' ) ) {
				LC = RunStr[ 15 + SS ] - 'A';
				if ( LC >= MAX_BM_CHAMBER_DEPTH ) {
					LC = 0;
				}
				else {
					LC = LC + MAX_TM_CHAMBER_DEPTH;
				}
				if      ( STRCMP_L( RunStr + 16 + SS , "(C)" ) ) ENDINDEX = 1;
				else if ( STRCMP_L( RunStr + 16 + SS , "(W)" ) ) ENDINDEX = 2;
				else if ( STRCMP_L( RunStr + 16 + SS , "(M)" ) ) ENDINDEX = 3;
				else if ( STRCMP_L( RunStr + 16 + SS , "(D)" ) ) ENDINDEX = -1; // 2012.11.07
			}
			else {
				LC = 0;
				if      ( STRCMP_L( RunStr + 15 + SS , "(C)" ) ) ENDINDEX = 1;
				else if ( STRCMP_L( RunStr + 15 + SS , "(W)" ) ) ENDINDEX = 2;
				else if ( STRCMP_L( RunStr + 15 + SS , "(M)" ) ) ENDINDEX = 3;
				else if ( STRCMP_L( RunStr + 15 + SS , "(D)" ) ) ENDINDEX = -1; // 2012.11.20
			}
		}
		//==========================================================================
		// 2006.08.30
		//==========================================================================
		if ( !Transfer_Maint_Inf_Code_End( LC ) ) {
			Event_Message_Reject( "TRANSFER SYSTEM is RUNNING ALREADY" );
			return;
		}
		//==========================================================================
		//======
//		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) { // 2006.08.30
		if ( SS == 0 ) { // 2008.09.04
			if ( Get_RunPrm_RUNNING_CLUSTER() ) { // 2006.08.30
//======================================
// 2006.08.30
//======================================
//			if ( _beginthread( (void *) Transfer_Maint_Inf_Code_for_Low_Level , 0 , (void *) -1 ) == -1 ) {
//				//----------------------------------------------------------------------------------------------------------------
//				// 2004.08.18
//				//----------------------------------------------------------------------------------------------------------------
//				_IO_CIM_PRINTF( "THREAD FAIL Transfer_Maint_Inf_Code_for_Low_Level(2) %d\n" , -1 );
//				//----------------------------------------------------------------------------------------------------------------
//			}
				Transfer_Maint_Inf_Code_End_IO_Set( LC , SYS_ABORTED );
//======================================
				Event_Message_Reject( "AUTO SYSTEM is RUNNING ALREADY" );
				return;
			}
		}
		//
		QA_RUN = SS; // 2012.07.28
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		if      ( STRNCMP_L( Else1Str , "PICK_ONLY_TM"	, 12 ) )	{ INDEX = 1; SS = 12; }
		else if ( STRNCMP_L( Else1Str , "PLACE_ONLY_TM"	, 13 ) )	{ INDEX = 2; SS = 13; }
		else if ( STRNCMP_L( Else1Str , "RETPICK_TM"	, 10 ) )	{ INDEX = 3; SS = 10; }
		else if ( STRNCMP_L( Else1Str , "RETPLACE_TM"	, 11 ) )	{ INDEX = 4; SS = 11; }
		else if ( STRNCMP_L( Else1Str , "EXTPICK_TM"	, 10 ) )	{ INDEX = 5; SS = 10; }
		else if ( STRNCMP_L( Else1Str , "EXTPLACE_TM"	, 11 ) )	{ INDEX = 6; SS = 11; }
		else if ( STRNCMP_L( Else1Str , "ROTATE_TM"		,  9 ) )	{ INDEX = 7; SS = 9; }
		else if ( STRNCMP_L( Else1Str , "EXTEND_TM"		,  9 ) )	{ INDEX = 8; SS = 9; }
		else if ( STRNCMP_L( Else1Str , "RETRACT_TM"	, 10 ) )	{ INDEX = 9; SS = 10; }
		else if ( STRNCMP_L( Else1Str , "UP_TM"			,  5 ) )	{ INDEX = 10; SS = 5; }
		else if ( STRNCMP_L( Else1Str , "DOWN_TM"		,  7 ) )	{ INDEX = 11; SS = 7; }
		else if ( STRNCMP_L( Else1Str , "HOME_TM"		,  7 ) )	{ INDEX = 12; SS = 7; }
		else if ( STRNCMP_L( Else1Str , "PICK_TM"		,  7 ) )	{ INDEX = 13; SS = 7; }
		else if ( STRNCMP_L( Else1Str , "PLACE_TM"		,  8 ) )	{ INDEX = 14; SS = 8; }
		else if ( STRNCMP_L( Else1Str , "SWAP_TM"		,  7 ) )	{ INDEX = 15; SS = 7; } // 2013.03.26
		//
		else if ( STRNCMP_L( Else1Str , "HOME_FM"       ,  7 ) )	{ INDEX = 21; SS = 7; }
		else if ( STRNCMP_L( Else1Str , "PICK_FM"       ,  7 ) )	{ INDEX = 22; SS = 7; }
		else if ( STRNCMP_L( Else1Str , "PLACE_FM"      ,  8 ) )	{ INDEX = 23; SS = 8; }
		else if ( STRNCMP_L( Else1Str , "ROTATE_FM"     ,  9 ) )	{ INDEX = 24; SS = 9; }

		//
		else if ( STRNCMP_L( Else1Str , "EXTEND_FM"		,  9 ) )	{ INDEX = 25; SS = 9; } // 2011.04.11
		else if ( STRNCMP_L( Else1Str , "RETRACT_FM"	, 10 ) )	{ INDEX = 26; SS = 10; } // 2011.04.11
		else if ( STRNCMP_L( Else1Str , "UP_FM"			,  5 ) )	{ INDEX = 27; SS = 5; } // 2011.04.11
		else if ( STRNCMP_L( Else1Str , "DOWN_FM"		,  7 ) )	{ INDEX = 28; SS = 7; } // 2011.04.11
		//

		else if ( STRNCMP_L( Else1Str , "PLCPICK_FALIC" , 13 ) )	{ INDEX = 31; SS = 13; }
		else if ( STRNCMP_L( Else1Str , "PICK_FALIC"    , 10 ) )	{ INDEX = 32; SS = 10; }
		else if ( STRNCMP_L( Else1Str , "PLACE_FALIC"   , 11 ) )	{ INDEX = 33; SS = 11; }
		else if ( STRNCMP_L( Else1Str , "PICKAL_FALIC"  , 12 ) )	{ INDEX = 34; SS = 12; }
		else if ( STRNCMP_L( Else1Str , "PLACEONLY_FALIC" , 15 ) )	{ INDEX = 35; SS = 15; }

		else if ( STRCMP_L( Else1Str , "MAPPING" ) )			INDEX = 50;

		else if ( STRCMP_L( Else1Str , "WAITING_FOR_TM" ) )		INDEX = 51;
		else if ( STRCMP_L( Else1Str , "WAITING_FOR_FM" ) )		INDEX = 52;

		else if ( STRCMP_L( Else1Str , "PM_MOVE" ) )			INDEX = 53; // 2003.10.28

		else													INDEX = atoi( Else1Str );
		//
//		if ( ( INDEX >= 1 ) && ( INDEX <= 14 ) ) { // 2007.08.13
//		if ( ( INDEX >= 1 ) && ( INDEX <= 29 ) ) { // 2007.08.13 // 2011.04.11
		if ( ( INDEX >= 1 ) && ( INDEX <= 39 ) ) { // 2007.08.13 // 2011.04.11
			if ( Else1Str[SS] == 0 ) {
				tmatm = 0;
			}
			else {
				tmatm = atoi( Else1Str + SS ) - 1;
			}
		}
		else {
			tmatm = 0;
		}
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
		TransferID = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
		//
		Chamber = Get_Module_ID_From_String( 0 , Else1Str , -1 );
		//
		if ( Chamber < 0 ) INDEX = -1;
		//
		switch ( INDEX ) {
		case 1 :
		case 2 :
		case 3 :
		case 4 :
		case 5 :
		case 6 :
		case 7 :
		case 8 :
		case 9 :
		case 10 :
		case 11 :
		case 12 :	// EQUIP_ROBOT_CONTROL_MAINT_INF
			//
		case 13 :	// EQUIP_PICK_FROM_CHAMBER_MAINT_INF
		case 14 :	// EQUIP_PLACE_TO_CHAMBER_MAINT_INF
		case 15 :	// EQUIP_SWAP_CHAMBER_MAINT_INF // 2013.01.11
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
			if      ( STRCMP_L( Else1Str , "A" ) )	Finger = 0;
			else if ( STRCMP_L( Else1Str , "B" ) )	Finger = 1;
			else if ( STRCMP_L( Else1Str , "C" ) )	Finger = 2;
			else if ( STRCMP_L( Else1Str , "D" ) )	Finger = 3;
			else									INDEX = -1;
			//
			STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
			Slot = atoi( Else1Str );
			//
//			if ( ( Slot < 0 ) || ( Slot > MAX_CASSETTE_SLOT_SIZE ) ) INDEX = -1; // 2007.06.27
			if ( ( Slot < 0 ) || ( Slot > 9999 ) ) INDEX = -1; // 2007.06.27
			//
			//
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
			Depth = atoi( Else1Str );
			//
//			if ( ( Depth < 0 ) || ( Depth > MAX_CASSETTE_SLOT_SIZE ) ) INDEX = -1; // 2007.06.27
			if ( ( Depth < 0 ) || ( Depth > 9999 ) ) INDEX = -1; // 2007.06.27
			//
			if ( ( Slot <= 0 ) && ( Depth <= 0 ) ) INDEX = -1;
			//------------------------
			STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 ); // 2004.06.23
			Trorder = atoi( Else1Str );
			//------------------------
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 ); // 2006.11.16
			SrcCass = atoi( Else1Str ); // 2006.11.16
			break;
		case 21 :	// EQUIP_HOME_FROM_FMBM
		case 22 :	// EQUIP_PICK_FROM_FMBM
		case 23 :	// EQUIP_PLACE_TO_FMBM
		case 24 :	// EQUIP_ROBOT_FM_MOVE_CONTROL
			//
		case 25 :	// EXTEND
		case 26 :	// RETRACT
		case 27 :	// UP
		case 28 :	// DOWN
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
			ArmASlot = atoi( Else1Str );
			//
			if ( ( ArmASlot < 0 ) || ( ArmASlot > MAX_CASSETTE_SLOT_SIZE ) ) INDEX = -1;
			//
			STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
			ArmBSlot = atoi( Else1Str );
			if ( ( ArmBSlot < 0 ) || ( ArmBSlot > MAX_CASSETTE_SLOT_SIZE ) ) INDEX = -1;
			//
			if ( ( ArmASlot <= 0 ) && ( ArmBSlot <= 0 ) ) INDEX = -1;
			//
//			if ( ( Chamber >= PM1 ) && ( Chamber < BM1 ) ) INDEX = -1; // 2002.02.16
			if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) INDEX = -1;
			//------------------------
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 ); // 2004.06.23
			Trorder = atoi( Else1Str );
			break;
			//
		case 31 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PLACE_MDL_PICK
		case 32 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PICK
		case 33 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PLACE_MDL
		case 34 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_MDL_PICK
		case 35 :	// EQUIP_PLACE_AND_PICK_WITH_FALIC	FAL_RUN_MODE_PLACE
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
			ArmASlot = atoi( Else1Str );
			//
//			if ( ( ArmASlot < 0 ) || ( ArmASlot > MAX_CASSETTE_SLOT_SIZE ) ) INDEX = -1; // 2007.11.28
//			if ( ( ArmASlot < 0 ) || ( ArmASlot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) INDEX = -1; // 2007.11.28 // 2008.09.22
			//
			STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
			ArmBSlot = atoi( Else1Str );
//			if ( ( ArmBSlot < 0 ) || ( ArmBSlot > MAX_CASSETTE_SLOT_SIZE ) ) INDEX = -1; // 2007.11.28
//			if ( ( ArmBSlot < 0 ) || ( ArmBSlot > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) INDEX = -1; // 2007.11.28 // 2008.09.22
			//
			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
			SrcCass = atoi( Else1Str );
			//
			if ( STRNCMP_L( Else3Str , "BM" , 2 ) ) {
				BMChamber = atoi( Else3Str + 2 ) - 1 + BM1;
				if ( ( BMChamber < BM1 ) || ( ( BMChamber - BM1 + 1 ) > MAX_BM_CHAMBER_DEPTH ) ) BMChamber = -1;
			}
			else {
				BMChamber = -1;
			}
			//
			if ( ( ArmASlot <= 0 ) && ( ArmBSlot <= 0 ) ) {
				INDEX = -1;
			}
			//
			//================================================================
			// 2007.08.29
			//================================================================
			if ( ( ArmBSlot > 0 ) && ( BMChamber > 0 ) ) {
				BMChamber = ( BMChamber * 10000 ) + BMChamber;
			}
			//================================================================
			//
			if ( ( Chamber != AL ) && ( Chamber != IC ) ) INDEX = -1;
			//
			if ( ( SrcCass < 0 ) && ( SrcCass > MAX_CASSETTE_SIDE ) ) INDEX = -1;
			break;
			//-----------------------------------------------------------------------------------------------------------------------------------------
		case 50 :	// EQUIP_CARRIER_MAPPING
			if ( Chamber <  CM1 ) INDEX = -1;
			if ( Chamber >= PM1 ) INDEX = -1;
			break;
		case 51 :
		case 52 :	// EQUIP_RUN_WAITING_BM
			if ( Chamber <  BM1 ) INDEX = -1;
			if ( Chamber >= TM  ) INDEX = -1;

			STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );	// 2013.01.29
			SrcCass = atoi( Else1Str );	// 2013.01.29

			break;
		case 53 :	// EQUIP_PM_MOVE_CONTROL
			// 2003.10.28
			break;
			//-----------------------------------------------------------------------------------------------------------------------------------------
		default :
			INDEX = -1;
			break;
		}
		//
		Transfer_Maint_Inf_Code_Interface_Data_Setting( ENDINDEX , LC , tmatm , Chamber , Finger , Slot , Depth , ArmASlot , ArmBSlot , SrcCass , BMChamber , TransferID , Trorder );
		//---------------------------------------------------
//		_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 1 ); // 2012.07.28
		_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , QA_RUN + 1 ); // 2012.07.28
		//---------------------------------------------------
		if ( _beginthread( (void *) Transfer_Maint_Inf_Code_for_Low_Level , 0 , (void *) ( ( LC * 100 ) + INDEX ) ) == -1 ) {
			//----------------------------------------------------------------------------------------------------------------
			// 2004.08.18
			//----------------------------------------------------------------------------------------------------------------
			_IO_CIM_PRINTF( "THREAD FAIL Transfer_Maint_Inf_Code_for_Low_Level(3) %d\n" , -1 );
			//----------------------------------------------------------------------------------------------------------------
			Transfer_Maint_Inf_Code_End_IO_Set( LC , SYS_ABORTED ); // 2006.08.30
			if ( Transfer_Maint_Inf_Code_End( -1 ) ) _i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 0 ); // 2006.08.30
			//----------------------------------------------------------------------------------------------------------------
		}
	*/
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_REMLOC" ) ) { // 2017.04.04
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		switch( Get_PMBM_ID_From_String( 1 , Else1Str , 0 , &INDEX ) ) {
		case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
		}
		//
		if ( STRCMP_L( Else2Str , "REM" ) || STRCMP_L( Else2Str , "REMOTE" ) ) {
			_i_SCH_PRM_SET_MODULE_SERVICE_MODE( INDEX , TRUE );
		}
		else if ( STRCMP_L( Else2Str , "LOC" ) || STRCMP_L( Else2Str , "LOCAL" ) ) {
			_i_SCH_PRM_SET_MODULE_SERVICE_MODE( INDEX , FALSE );
		}
	}
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "WAFER_RESULT_SET" , 16 ) ) { // 2003.10.21
		if ( !STRCMP_L( RunStr + 16 , "(A)" ) ) {
			//======
			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
	//======================================
	// 2006.08.30
	//======================================
	//			if ( _beginthread( (void *) Transfer_Maint_Inf_Code_for_Low_Level , 0 , (void *) -1 ) == -1 ) {
	//				//----------------------------------------------------------------------------------------------------------------
	//				// 2004.08.18
	//				//----------------------------------------------------------------------------------------------------------------
	//				_IO_CIM_PRINTF( "THREAD FAIL Transfer_Maint_Inf_Code_for_Low_Level(5) %d\n" , -1 );
	//				//----------------------------------------------------------------------------------------------------------------
	//			}
	//======================================
				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
				//
				PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
				//
				return;
			}
		}
		//
//		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = atoi( Else1Str ); // 2005.05.29
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 0 , Else1Str , 0 ); // 2005.05.29
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Slot = atoi( Else1Str );
		//
//		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	Chamber = atoi( Else1Str ); // 2005.05.29
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	Chamber = Get_Module_ID_From_String( 0 , Else1Str , 0 ); // 2005.05.29
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Depth = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	TransferID = atoi( Else1Str );
		//
		SS = SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( SrcCass , Slot , Chamber , Depth , TransferID , -1 , -1 ); // 2003.10.21
		//
		if ( SS != 0 ) {
			sprintf( ElseStr , "WAFER_RESULT_SET FAIL = %d" , SS );
			Event_Message_Reject( ElseStr );
			//
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
			//
			return;
		}
		//
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		//
	}
	else if ( STRCMP_L( RunStr , "WAFER_RESTORE_SET" ) ) { // 2012.02.07
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 0 , Else1Str , 0 );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Slot = atoi( Else1Str );
		//
		SS = _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Event_FDHC( SrcCass , Slot , TRUE );
		//
		if ( SS != 0 ) {
			sprintf( ElseStr , "WAFER_RESTORE_SET FAIL = %d" , SS );
			Event_Message_Reject( ElseStr );
			//
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
			//
			return;
		}
		//
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		//
	}		
	else if ( STRCMP_L( RunStr , "WAFER_RESTORE_RESET" ) ) { // 2012.03.25
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 0 , Else1Str , 0 );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Slot = atoi( Else1Str );
		//
		SS = _i_SCH_CLUSTER_Check_and_Make_Restore_Wafer_From_Event_FDHC( SrcCass , Slot , FALSE );
		//
		if ( SS != 0 ) {
			sprintf( ElseStr , "WAFER_RESTORE_RESET FAIL = %d" , SS );
			Event_Message_Reject( ElseStr );
			//
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
			//
			return;
		}
		//
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		//
	}
	else if ( STRCMP_L( RunStr , "WAFER_REMOVE_SET" ) ) { // 2012.12.05
		//
//		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 0 , Else1Str , 0 ); // 2013.01.07
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 1 , Else1Str , 0 ); // 2013.01.07
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Slot = atoi( Else1Str );
		//
		SS = _i_SCH_CLUSTER_Check_and_Make_Remove_Wafer_From_Event_FDHC( SrcCass , Slot );
		//
		if ( SS != 0 ) {
			sprintf( ElseStr , "WAFER_REMOVE_SET FAIL = %d" , SS );
			Event_Message_Reject( ElseStr );
			//
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
			//
			return;
		}
		//
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		//
	}		
	else if ( STRCMP_L( RunStr , "WAFER_RETURN_SET" ) ) { // 2013.11.28
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 1 , Else1Str , 0 ); // 2013.01.07
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Slot = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 ); // 2013.12.13
		//
		if ( Else1Str[0] == 0 ) {
			SS = 1; // return
		}
		else if ( tolower(Else1Str[0]) == 'r' ) {
			SS = 2; // return & rerun
		}
		else {
			//
			SS = atoi( Else1Str );
			//
			if ( SS < 0 ) SS = 0; 
			if ( SS > 2 ) SS = 2; 
			//
		}
		//
		SCHEDULER_CONTROL_Set_GLOBAL_STOP( 1 , SrcCass , Slot , SS );
		//
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		//
	}		
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_EXTRA_FLAG" ) ) { // 2013.02.25
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	SrcCass = Get_Module_ID_From_String( 0 , Else1Str , 0 );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	Slot = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );	INDEX = atoi( Else1Str );
		//
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 );	ENDINDEX = atoi( Else1Str );
		//
		SS = _i_SCH_CLUSTER_Check_and_Set_Extra_Flag_From_Event_FDHC( SrcCass , Slot , INDEX , ENDINDEX );
		//
		if ( SS != 0 ) {
			sprintf( ElseStr , "SET_EXTRA_FLAG FAIL = %d" , SS );
			Event_Message_Reject( ElseStr );
			//
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
			//
			return;
		}
		//
		PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
		//
	}		
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "CONTROLJOB" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//--------------------------------------------------------------------------------
		if      ( STRCMP_L( Else1Str , "MAPVERIFY" ) ) {
			//---------------------------------------------------
			if ( !Event_LOT_ID_From_String_And_Check( Else2Str , &INDEX ) ) {
				//
				_IO_CIM_PRINTF( "%s [FAIL] = [A] %d\n" , PROGRAM_EVENT_READ() , -99 ); // 2015.03.27
				//
				return;
			}
			//---------------------------------------------------
			SS = SCHMULTI_CASSETTE_VERIFY_MAKE_OK( INDEX );
			//
			if ( SS != 0 ) {
				sprintf( ElseStr , "%s %s|%s FAIL = %d" , RunStr , Else1Str , Else2Str , SS );
				Event_Message_Reject( ElseStr );
				//
				_IO_CIM_PRINTF( "%s [FAIL] = [B] %d,%d\n" , PROGRAM_EVENT_READ() , INDEX , SS ); // 2015.03.27
				//
			}
			return;
		}
		//--------------------------------------------------------------------------------
		else if ( STRCMP_L( Else1Str , "FORCEVERIFY" ) ) { // 2019.04.12
			//---------------------------------------------------
			if ( !Event_LOT_ID_From_String_And_Check( Else2Str , &INDEX ) ) {
				//
				_IO_CIM_PRINTF( "%s [FAIL] = [F1] %d\n" , PROGRAM_EVENT_READ() , -99 );
				//
				return;
			}
			//---------------------------------------------------
			SS =  SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( INDEX , 0 );
			//
			if ( SS != 0 ) {
				sprintf( ElseStr , "%s %s|%s FAIL = %d" , RunStr , Else1Str , Else2Str , SS );
				Event_Message_Reject( ElseStr );
				//
				_IO_CIM_PRINTF( "%s [FAIL] = [F2] %d,%d\n" , PROGRAM_EVENT_READ() , INDEX , SS );
				//
			}
			return;
		}
		//--------------------------------------------------------------------------------
		else if ( STRCMP_L( Else1Str , "VERIFY" ) ) {
			INDEX = SCHMULTI_RUNJOB_SET_CONTROLJOB_POSSIBLE();
		}
		else if ( STRCMP_L( Else1Str , "INSERT" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_INSERT( FALSE , 0 );
		}
		else if ( STRCMP_L( Else1Str , "CHANGE" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_CHANGE();
		}
		else if ( STRCMP_L( Else1Str , "DELETE" ) ) {
			if ( Event_LOT_ID_From_String_And_Check( Else2Str , &INDEX ) ) { // 2011.04.11
				INDEX = SCHMULTI_CONTROLJOB_DELETE_CM( INDEX );
			}
			else {
				INDEX = SCHMULTI_CONTROLJOB_DELETE( -1 );
			}
		}
		else if ( STRCMP_L( Else1Str , "DELSELECT" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_DELSELECT( 0 , -1 , 0 , FALSE );
		}
		else if ( STRCMP_L( Else1Str , "HEADOFQ" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_HEADOFQ();
		}
		else if ( STRCMP_L( Else1Str , "CHGSELECT" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_CHGSELECT();
		}
		else if ( STRCMP_L( Else1Str , "PAUSE" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_PAUSE( Else2Str );
		}
		else if ( STRCMP_L( Else1Str , "RESUME" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_RESUME( Else2Str );
		}
		else if ( STRCMP_L( Else1Str , "STOP" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_STOP( Else2Str );
		}
		else if ( STRCMP_L( Else1Str , "ABORT" ) ) {
			INDEX = SCHMULTI_CONTROLJOB_ABORT( Else2Str );
		}

// TESTING
/*
		else if ( STRCMP_L( Else1Str , "TESTC" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 0 , Else2Str );

		}
		else if ( STRCMP_L( Else1Str , "TEST1" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 11 , Else2Str );

		}
		else if ( STRCMP_L( Else1Str , "TEST2" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 12 , Else2Str );

		}
		else if ( STRCMP_L( Else1Str , "TEST3" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 13 , Else2Str );

		}
		else if ( STRCMP_L( Else1Str , "TEST4" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 14 , Else2Str );

		}
		else if ( STRCMP_L( Else1Str , "TESTM" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 1 , Else2Str );
		}
		else if ( STRCMP_L( Else1Str , "TESTS" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 2 , Else2Str );
		}
		else if ( STRCMP_L( Else1Str , "TESTA" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 3 , Else2Str );
		}
		else if ( STRCMP_L( Else1Str , "TESTU" ) ) {

			void SCHMULTI_CONTROLJOB_TEST( int , char * );

			INDEX = 0;

			SCHMULTI_CONTROLJOB_TEST( 4 , Else2Str );
		}
*/
// TESTING

		else {
			INDEX = -1;
		}
		//
		if ( INDEX != 0 ) {
			sprintf( ElseStr , "%s %s FAIL = %d" , RunStr , Else1Str , INDEX );
			Event_Message_Reject( ElseStr );
			//
			SCHMULTI_GET_CONTROLJOB_ERRORDATA( Else1Str ); // 2015.03.27
			//
			_IO_CIM_PRINTF( "%s [FAIL] = %d , %s\n" , PROGRAM_EVENT_READ() , INDEX , Else1Str ); // 2015.03.27
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "PROCESSJOB" ) ) {
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//--------------------------------------------------------------------------------
		if      ( STRCMP_L( Else1Str , "VERIFY" ) ) {
			INDEX = SCHMULTI_RUNJOB_SET_PROCESSJOB_POSSIBLE();
		}
		else if ( STRCMP_L( Else1Str , "INSERT" ) ) {
			INDEX = SCHMULTI_PROCESSJOB_INSERT( -1 , FALSE ); // 2016.09.01
		}
		else if ( STRCMP_L( Else1Str , "INSERTG" ) ) { // 2016.09.01
			INDEX = SCHMULTI_PROCESSJOB_INSERT( -1 , TRUE );
		}
		else if ( STRCMP_L( Else1Str , "CHANGE" ) ) {
			INDEX = SCHMULTI_PROCESSJOB_CHANGE();
		}
		else if ( STRCMP_L( Else1Str , "UPDATE" ) ) { // 2007.12.09
			INDEX = SCHMULTI_PROCESSJOB_UPDATE();
		}
		else if ( STRCMP_L( Else1Str , "DELETE" ) ) { // Cancel
			INDEX = SCHMULTI_PROCESSJOB_DELETE( 2 , -1 );
		}
		else if ( STRCMP_L( Else1Str , "PAUSE" ) ) {
			INDEX = SCHMULTI_PROCESSJOB_PAUSE();
		}
		else if ( STRCMP_L( Else1Str , "RESUME" ) ) {
			INDEX = SCHMULTI_PROCESSJOB_RESUME();
		}
		else if ( STRCMP_L( Else1Str , "STOP" ) ) {
			INDEX = SCHMULTI_PROCESSJOB_STOP();
		}
		else if ( STRCMP_L( Else1Str , "ABORT" ) ) {
			INDEX = SCHMULTI_PROCESSJOB_ABORT();
		}
		else {
			INDEX = -1;
		}
		//
		if ( INDEX != 0 ) {
			sprintf( ElseStr , "%s %s FAIL = %d" , RunStr , Else1Str , INDEX );
			Event_Message_Reject( ElseStr );
			//
			SCHMULTI_GET_PROCESSJOB_ERRORDATA( Else1Str ); // 2015.03.27
			//
			_IO_CIM_PRINTF( "%s [FAIL] = %d , %s\n" , PROGRAM_EVENT_READ() , INDEX , Else1Str ); // 2015.03.27
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "CPJOBRESET" ) ) {
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		if ( STRCMP_L( Else1Str , "LOG" ) ) {
			SCHMULTI_CONTROLLER_LOGGING( atoi(Else2Str) );
		}
		else {
			INDEX = SCHMULTI_CPJOB_RESET();
			if ( INDEX != 0 ) {
				sprintf( ElseStr , "%s FAIL = %d" , RunStr , INDEX );
				Event_Message_Reject( ElseStr );
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "MODULE_SIZE_MODIFY" ) ) { // 2002.11.01
		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
			return;
		}
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//====================================================================
		INDEX = Get_Module_ID_From_String( 3 , Else1Str , 0 );
		//====================================================================
		if ( INDEX == 0 ) {
			Event_Message_Reject( "MESSAGE FORMAT ERROR" );
			return;
		}
		//====================================================================
		if ( INDEX == IC ) { // for inside // 2005.11.29
			if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_FNAME()[0] != 0 ) {
				Event_Message_Reject( "IC SLOT FIXED WITH 1" );
				return;
			}
			SS = atoi( Else2Str );
			if ( ( SS <= 0 ) || ( SS > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) {
				Event_Message_Reject( "IC SLOT DATA ERROR" );
				return;
			}
			if ( _i_SCH_PRM_GET_MODULE_SIZE( F_IC ) != SS ) {
				_i_SCH_PRM_SET_MODULE_SIZE( F_IC , SS );
				_i_SCH_PRM_SET_MODULE_SIZE_CHANGE( F_IC , TRUE ); // 2007.08.14
				//
				GUI_SAVE_PARAMETER_DATA( 1 );
				//
				Set_RunPrm_IO_Setting( 8 );
			}
		}
		else {
			SS = atoi( Else2Str );
			if ( ( SS <= 0 ) || ( SS > MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "MESSAGE FORMAT ERROR" );
				return;
			}
			if ( _i_SCH_PRM_GET_MODULE_SIZE( INDEX ) != SS ) {
				_i_SCH_PRM_SET_MODULE_SIZE( INDEX , SS );
				_i_SCH_PRM_SET_MODULE_SIZE_CHANGE( INDEX , TRUE );
				//
				GUI_SAVE_PARAMETER_DATA( 1 );
				//
				Set_RunPrm_IO_Setting( 8 );
			}
		}
		//====================================================================
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SAVEOPTION" ) ) { // 2002.05.31
		GUI_SAVE_PARAMETER_DATA( 1 );
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "TBM_SIZE_MODIFY" ) ) { // 2002.08.27 // for STL
		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
			return;
		}
		LC = -1;
		ResSC = -1;
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		STR_SEPERATE_CHAR( Else1Str , ':' , ElseStr , DispStr , 255 );
		//
		switch( Get_PMBM_ID_From_String( 2 , ElseStr , 0 , &INDEX ) ) {
		case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
		case 2 :	Event_Message_Reject( "Disable Module Data" );	return;	break;
		}
		//
		STR_SEPERATE_CHAR( DispStr , ':' , JobStr , LotStr , 255 );
		SS = atoi( JobStr );
		if ( ( SS <= 0 ) || ( SS > MAX_CASSETTE_SLOT_SIZE ) ) {
			Event_Message_Reject( "MESSAGE FORMAT ERROR" );
			return;
		}
		if ( strlen( LotStr ) > 0 ) LC = atoi( LotStr );
		if ( strlen( Else2Str ) <= 0 ) {
			_i_SCH_PRM_SET_MODULE_SIZE( INDEX , SS );
			if ( LC >= 0 ) _i_SCH_PRM_SET_OFFSET_SLOT_FROM_ALL( INDEX , LC );
		}
		else {
			STR_SEPERATE_CHAR( Else2Str , ':' , ElseStr , DispStr , 255 );
			//
			switch( Get_PMBM_ID_From_String( 2 , ElseStr , 0 , &ENDINDEX ) ) {
			case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
			case 2 :	Event_Message_Reject( "Disable Module Data" );	return;	break;
			}
			//
			STR_SEPERATE_CHAR( DispStr , ':' , JobStr , LotStr , 255 );
			ES = atoi( JobStr );
			if ( ( ES <= 0 ) || ( ES > MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "MESSAGE FORMAT ERROR" );
				return;
			}
			if ( strlen( LotStr ) > 0 ) ResSC = atoi( LotStr );
			_i_SCH_PRM_SET_MODULE_SIZE( INDEX , SS );
			if ( LC >= 0 ) _i_SCH_PRM_SET_OFFSET_SLOT_FROM_ALL( INDEX , LC );
			_i_SCH_PRM_SET_MODULE_SIZE( ENDINDEX , ES );
			if ( ResSC >= 0 ) _i_SCH_PRM_SET_OFFSET_SLOT_FROM_ALL( ENDINDEX , ResSC );
		}
	}
	//--------------------------------------------------------------------------------
//	else if ( STRCMP_L( RunStr , "DEFAULT_RECIPE_CHANGE" ) ) { // 2002.10.02 // 2011.11.16
	else if ( STRNCMP_L( RunStr , "DEFAULT_RECIPE_CHANGE" , 21 ) ) { // 2002.10.02 // 2011.11.16
		//
		SS = atoi(RunStr + 21) - 1; // 2011.11.16
		//
//		for ( SS = 0 ; SS < ( MAX_CASSETTE_SIDE + 1 ) ; SS++ ) { // 2003.02.12
//			if ( _i_SCH_SYSTEM_USING_GET( SS ) > 0 ) break; // 2003.02.12
//		} // 2003.02.12
//		if ( SS != ( MAX_CASSETTE_SIDE + 1 ) ) { // 2003.02.12
//			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" ); // 2003.02.12
//			return; // 2003.02.12
//		} // 2003.02.12
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		if      ( STRCMP_L( Else1Str , "READY0"  ) ) ENDINDEX = 0; // 2011.11.16
		else if ( STRCMP_L( Else1Str , "READY"   ) ) ENDINDEX = 1;
		else if ( STRCMP_L( Else1Str , "READY2"  ) ) ENDINDEX = 2; // 2004.06.11
		else if ( STRCMP_L( Else1Str , "READY3"  ) ) ENDINDEX = 3; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY4"  ) ) ENDINDEX = 4; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY5"  ) ) ENDINDEX = 5; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY6"  ) ) ENDINDEX = 6; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY7"  ) ) ENDINDEX = 7; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY8"  ) ) ENDINDEX = 8; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY9"  ) ) ENDINDEX = 9; // 2005.02.17
		else if ( STRCMP_L( Else1Str , "READY10" ) ) ENDINDEX = 10; // 2007.04.02
		else if ( STRCMP_L( Else1Str , "READY11" ) ) ENDINDEX = 11; // 2007.04.02
		else if ( STRCMP_L( Else1Str , "READY12" ) ) ENDINDEX = 12; // 2017.09.07
		else if ( STRCMP_L( Else1Str , "READY13" ) ) ENDINDEX = 13; // 2017.09.15
		//
		else if ( STRCMP_L( Else1Str , "END0"    ) ) ENDINDEX = 100; // 2011.11.16
		else if ( STRCMP_L( Else1Str , "END"     ) ) ENDINDEX = 101; // 2004.06.11
		else if ( STRCMP_L( Else1Str , "END2"    ) ) ENDINDEX = 102; // 2004.06.11
		else if ( STRCMP_L( Else1Str , "END3"    ) ) ENDINDEX = 103; // 2004.10.11
		else {
			Event_Message_Reject( "DATA INFORMATION ERROR" );
			return;
		}
//		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 255 ); // 2007.01.04
		STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , ElseStr , 512 ); // 2007.01.04
		//
		//
		//
		//
		// 2018.05.08
		//
//		switch( Get_PMBM_ID_From_String( 0 , Else1Str , 0 , &INDEX ) ) {
//		case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
///		case 2 :	Event_Message_Reject( "Disable Module Data" );	return;	break;
//		}
		switch( Get_PMBM_ID_From_String( 0 , Else1Str , 0 , &INDEX ) ) {
		case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
		}
		//
		//=========================================================================
		if ( ElseStr[0] == '"' ) {
			STR_SEPERATE_CHAR( ElseStr + 1 , '"' , Else1Str , Else2Str , 512 );
			if ( Else2Str[0] == '|' ) LC = 1;
			else                      LC = 0;
		}
		else {
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 512 ); // 2007.01.04
			LC = 0;
		}
		//
		if ( strlen( Else1Str ) <= 0 ) {
			if ( ENDINDEX < 100 ) {
				if ( SS < 0 ) { // 2011.11.16
					//
					STR_SEPERATE_CHAR( Else2Str + LC , '|' , LotStr , MidStr , 255 ); // 2017.11.09
					//
//					_i_SCH_PRM_SET_READY_RECIPE_MINTIME( INDEX , 0 ); // 2017.11.09
					_i_SCH_PRM_SET_READY_RECIPE_MINTIME( INDEX , atoi( LotStr ) ); // 2017.11.09

					_i_SCH_PRM_SET_READY_RECIPE_NAME( INDEX , "" );
					_i_SCH_PRM_SET_READY_RECIPE_USE( INDEX , 0 );
					//
//					_i_SCH_PRM_SET_READY_RECIPE_TYPE( INDEX , 0 ); // 2017.10.16 // 2017.11.09
					_i_SCH_PRM_SET_READY_RECIPE_TYPE( INDEX , atoi( MidStr ) ); // 2017.10.16 // 2017.11.09
				}
				else { // 2011.11.16
					if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
						_i_SCH_PRM_SET_inside_READY_RECIPE_NAME( SS , INDEX , "" );
						//
						if ( ENDINDEX != 0 ) _i_SCH_PRM_SET_inside_READY_RECIPE_USE( SS , INDEX , 0 );
					}
				}
			}
			else {
				if ( SS < 0 ) { // 2011.11.16
					_i_SCH_PRM_SET_END_RECIPE_MINTIME( INDEX , 0 );
					_i_SCH_PRM_SET_END_RECIPE_NAME( INDEX , "" );
					_i_SCH_PRM_SET_END_RECIPE_USE( INDEX , 0 );
				}
				else { // 2011.11.16
					if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
						_i_SCH_PRM_SET_inside_END_RECIPE_NAME( SS , INDEX , "" );
						//
						if ( ENDINDEX != 100 ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( SS , INDEX , 0 );
					}
				}
			}
		}
		else {
			if ( ENDINDEX < 100 ) {
				if ( SS < 0 ) { // 2011.11.16
					//
					STR_SEPERATE_CHAR( Else2Str + LC , '|' , LotStr , MidStr , 255 ); // 2017.10.16
					//
					_i_SCH_PRM_SET_READY_RECIPE_NAME( INDEX , Else1Str );
//					_i_SCH_PRM_SET_READY_RECIPE_MINTIME( INDEX , atoi( Else2Str + LC ) ); // 2005.08.18 // 2017.10.16
					_i_SCH_PRM_SET_READY_RECIPE_MINTIME( INDEX , atoi( LotStr ) ); // 2005.08.18 // 2017.10.16
					_i_SCH_PRM_SET_READY_RECIPE_USE( INDEX , ENDINDEX );
					//
					_i_SCH_PRM_SET_READY_RECIPE_TYPE( INDEX , atoi( MidStr ) ); // 2017.10.16
					//
				}
				else { // 2011.11.16
					if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
						_i_SCH_PRM_SET_inside_READY_RECIPE_NAME( SS , INDEX , Else1Str );
						//
						if ( ENDINDEX != 0 ) _i_SCH_PRM_SET_inside_READY_RECIPE_USE( SS , INDEX , ENDINDEX );
					}
				}
			}
			//
			else {
				if ( SS < 0 ) { // 2011.11.16
					//
					STR_SEPERATE_CHAR( Else2Str + LC , '|' , LotStr , MidStr , 255 ); // 2017.10.16
					//
					_i_SCH_PRM_SET_END_RECIPE_NAME( INDEX , Else1Str );
//					_i_SCH_PRM_SET_END_RECIPE_MINTIME( INDEX , atoi( Else2Str + LC ) ); // 2005.08.18 // 2017.10.16
					_i_SCH_PRM_SET_END_RECIPE_MINTIME( INDEX , atoi( LotStr ) ); // 2005.08.18 // 2017.10.16
					_i_SCH_PRM_SET_END_RECIPE_USE( INDEX , ENDINDEX - 100 );
					//
					_i_SCH_PRM_SET_READY_RECIPE_TYPE( INDEX , atoi( MidStr ) ); // 2017.10.16
					//
				}
				else { // 2011.11.16
					if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
						_i_SCH_PRM_SET_inside_END_RECIPE_NAME( SS , INDEX , Else1Str );
						//
						if ( ENDINDEX != 100 ) _i_SCH_PRM_SET_inside_END_RECIPE_USE( SS , INDEX , ENDINDEX - 100 );
					}
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_FORCE_PRE_FIRST" ) ) { // 2003.11.11
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( ElseStr , &INDEX ) ) return;
		//---------------------------------------------------
		_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( INDEX , 1 );
		//
		_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Event Received SET_FORCE_PRE_FIRST%cSCTLMSG\n" , 9 ); // 2012.03.23
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_FORCE_PRE_AGAIN" ) ) { // 2006.11.15
		//---------------------------------------------------
		if ( Get_PMBM_ID_From_String( 1 , ElseStr , 0 , &INDEX ) == 0 ) {
			Event_Message_Reject( "Invalid Module Data" );
			return;
		}
		//---------------------------------------------------
		_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( INDEX , 2 );
		//
//		_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Event Received SET_FORCE_PRE_AGAIN%cSCTLMSG\n" , 9 ); // 2012.03.23
		//
		// 2012.07.25
		for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) {
			if ( _i_SCH_SYSTEM_USING_RUNNING( SS ) ) {
				_i_SCH_LOG_LOT_PRINTF( SS , "Scheduler Event Received SET_FORCE_PRE_AGAIN %s%cSCTLMSG\n" , ElseStr , 9 );
			}
		}
		//
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "CHECK_FORCE_PRE_FIRST" ) ) { // 2005.10.27
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse( INDEX , Else2Str ); // 2005.10.27
		//
		_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Event Received CHECK_FORCE_PRE_FIRST %s%cSCTLMSG\n" , Else2Str , 9 ); // 2012.03.23
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "CHECK_LAST_LOT_FOR_PRE_SKIP" ) ) { // 2007.09.07
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData_NotUse2( INDEX , Else2Str );
		//
		_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Event Received CHECK_LAST_LOT_FOR_PRE_SKIP %s%cSCTLMSG\n" , Else2Str , 9 ); // 2012.03.23
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_SKIP_FOR_CASSETTE_SLOT" ) ) { // 2005.07.19
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		if ( !SCHEDULER_CONTROL_Set_CASSETTE_SKIP( INDEX , atoi(Else2Str) , TRUE ) ) {
			Event_Message_Reject( "CASSETTE_SKIP OPERATION ERROR" );
			return;
		}
		//
		_i_SCH_LOG_LOT_PRINTF( INDEX , "Scheduler Event Received SET_SKIP_FOR_CASSETTE_SLOT %s%cSCTLMSG\n" , Else2Str , 9 ); // 2012.03.23
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "PRE_WAIT_SET" ) ) { // 2016.11.02
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		//
		SCHMRDATA_Data_IN_Wait_Set( INDEX , -1 , atoi( Else2Str ) , FALSE );
		//
	}
	else if ( STRCMP_L( RunStr , "PRE_WAIT_SET(W)" ) ) { // 2016.11.02
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		//
		SCHMRDATA_Data_IN_Wait_Set( INDEX , -1 , atoi( Else2Str ) , TRUE );
		//
	}
	else if ( STRCMP_L( RunStr , "PRE_WAIT_RESET" ) ) { // 2016.11.02
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		//
		SCHMRDATA_Data_IN_Wait_Reset( INDEX , -1 );
		//
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_HOT_LOT" ) ) { // 2011.06.13
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//---------------------------------------------------
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//---------------------------------------------------
		_in_HOT_LOT_CHECKING[INDEX] = atoi( Else2Str );
		//---------------------------------------------------
	}
	//--------------------------------------------------------------------------------
	/*
	//
	// 2017.04.21
	//
	else if ( STRCMP_L( RunStr , "SET_CHANGE_BM12_TO_OTHER_MODE" ) ) { // 2004.08.14
		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
			return;
		}
		if      ( STRCMP_L( ElseStr , "DEFAULT"             ) ) INDEX = 0;
		else if ( STRCMP_L( ElseStr , "PATHTHRU(B)"         ) ) INDEX = 1;
		else if ( STRCMP_L( ElseStr , "PATHTHRU(F)"         ) ) INDEX = 2;
		else if ( STRCMP_L( ElseStr , "PATHTHRU(S)"         ) ) INDEX = 3;
		else if ( STRCMP_L( ElseStr , "BATCH"               ) ) INDEX = 4;
		else if ( STRCMP_L( ElseStr , "FULLSWAP"            ) ) INDEX = 5;
		else if ( STRCMP_L( ElseStr , "SINGLESWAP"          ) ) INDEX = 6;
		else if ( STRCMP_L( ElseStr , "PATHTHRU(M)"         ) ) INDEX = 7; // 2006.02.25
		else if ( STRCMP_L( ElseStr , "MIDDLESWAP"          ) ) INDEX = 8; // 2006.02.25
		else if ( STRCMP_L( ElseStr , "PATHTHRU1"           ) ) INDEX = 9; // 2015.03.25
		else if ( STRCMP_L( ElseStr , "PATHTHRU2"           ) ) INDEX = 10; // 2015.03.25
//		else if ( STRCMP_L( ElseStr , "DEFAULT(D)"          ) ) INDEX = 7; // 2006.02.25
//		else if ( STRCMP_L( ElseStr , "PATHTHRU(D)(B)"      ) ) INDEX = 8; // 2006.02.25
//		else if ( STRCMP_L( ElseStr , "PATHTHRU(D)(F)"      ) ) INDEX = 9; // 2006.02.25
//		else if ( STRCMP_L( ElseStr , "PATHTHRU(D)(S)"      ) ) INDEX = 10; // 2006.02.25
		//
		else if ( STRCMP_L( ElseStr , "THREAD"               ) ) INDEX = 1; // 2005.09.09
		else if ( STRCMP_L( ElseStr , "DEFAULT.ONEBODY"      ) ) INDEX = 2; // 2005.09.09
		else if ( STRCMP_L( ElseStr , "THREAD.ONEBODY"       ) ) INDEX = 3; // 2005.09.09
		else if ( STRCMP_L( ElseStr , "DEFAULT.ONEBODY(2)"   ) ) INDEX = 4; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "THREAD.ONEBODY(2)"    ) ) INDEX = 5; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "DEFAULT.ONEBODY(PM)"  ) ) INDEX = 6; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "THREAD.ONEBODY(PM)"   ) ) INDEX = 7; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "DEFAULT.ONEBODY(2PM)" ) ) INDEX = 8; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "THREAD.ONEBODY(2PM)"  ) ) INDEX = 9; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "DEFAULT.ONEBODY(PS)"  ) ) INDEX = 10; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "THREAD.ONEBODY(PS)"   ) ) INDEX = 11; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "DEFAULT.ONEBODY(2PS)" ) ) INDEX = 12; // 2005.10.11
		else if ( STRCMP_L( ElseStr , "THREAD.ONEBODY(2PS)"  ) ) INDEX = 13; // 2005.10.11
		//
		else {
//			Event_Message_Reject( "DATA ANALYSIS ERROR" ); // 2015.03.25
//			return; // 2015.03.25
			//
			// 2015.03.25
			//
			INDEX = atoi( ElseStr );
			//
			if ( ( INDEX < 0 ) || ( INDEX > 15 ) ) {
				Event_Message_Reject( "DATA ANALYSIS ERROR" );
				return;
			}
			//
		}
		//
//		_i_SCH_PRM_SET_UTIL_SW_BM_SCHEDULING_FACTOR( INDEX ); // 2015.03.25
		//
		if ( _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() != INDEX ) { // 2015.03.25
			//
			_i_SCH_PRM_SET_UTIL_SW_BM_SCHEDULING_FACTOR( INDEX );
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//
			Set_RunPrm_IO_Setting( 25 );
			//
		}
		//
	}
	//
	*/
	//
	//--------------------------------------------------------------------------------
	//
	// 2017.04.21
	//
	else if ( STRCMP_L( RunStr , "SET_CHANGE_BM12_TO_OTHER_MODE" ) ) { // 2004.08.14 // 2017.04.21
	/*
	Event Format
		SET_CHANGE_BM12_TO_OTHER_MODE	{Type}|{Order}

			{Type}

				DEFAULT
				PATHTHRU(B)
				PATHTHRU(F)
				PATHTHRU(S)
				BATCH
				FULLSWAP
				SINGLESWAP
				PATHTHRU(M)
				MIDDLESWAP
				PATHTHRU1
				PATHTHRU2

			{Order} - BM1부터 숫자 1 char 씩

				0	-	default
				1	-	(F)
				2	-	(L)
				3	-	(FL)
				4	-	(LF)
				5	-	(F2)
				6	-	(L2)
				7	-	(FL2)
				8	-	(LF2)
	*/

		if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
			Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
			return;
		}
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		if      ( STRCMP_L( Else1Str , "DEFAULT"             ) ) INDEX = 0;
		else if ( STRCMP_L( Else1Str , "PATHTHRU(B)"         ) ) INDEX = 1;
		else if ( STRCMP_L( Else1Str , "PATHTHRU(F)"         ) ) INDEX = 2;
		else if ( STRCMP_L( Else1Str , "PATHTHRU(S)"         ) ) INDEX = 3;
		else if ( STRCMP_L( Else1Str , "BATCH"               ) ) INDEX = 4;
		else if ( STRCMP_L( Else1Str , "FULLSWAP"            ) ) INDEX = 5;
		else if ( STRCMP_L( Else1Str , "SINGLESWAP"          ) ) INDEX = 6;
		else if ( STRCMP_L( Else1Str , "PATHTHRU(M)"         ) ) INDEX = 7; // 2006.02.25
		else if ( STRCMP_L( Else1Str , "MIDDLESWAP"          ) ) INDEX = 8; // 2006.02.25
		else if ( STRCMP_L( Else1Str , "PATHTHRU1"           ) ) INDEX = 9; // 2015.03.25
		else if ( STRCMP_L( Else1Str , "PATHTHRU2"           ) ) INDEX = 10; // 2015.03.25
		//
		else if ( STRCMP_L( Else1Str , "THREAD"               ) ) INDEX = 1; // 2005.09.09
		else if ( STRCMP_L( Else1Str , "DEFAULT.ONEBODY"      ) ) INDEX = 2; // 2005.09.09
		else if ( STRCMP_L( Else1Str , "THREAD.ONEBODY"       ) ) INDEX = 3; // 2005.09.09
		else if ( STRCMP_L( Else1Str , "DEFAULT.ONEBODY(2)"   ) ) INDEX = 4; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "THREAD.ONEBODY(2)"    ) ) INDEX = 5; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "DEFAULT.ONEBODY(PM)"  ) ) INDEX = 6; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "THREAD.ONEBODY(PM)"   ) ) INDEX = 7; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "DEFAULT.ONEBODY(2PM)" ) ) INDEX = 8; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "THREAD.ONEBODY(2PM)"  ) ) INDEX = 9; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "DEFAULT.ONEBODY(PS)"  ) ) INDEX = 10; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "THREAD.ONEBODY(PS)"   ) ) INDEX = 11; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "DEFAULT.ONEBODY(2PS)" ) ) INDEX = 12; // 2005.10.11
		else if ( STRCMP_L( Else1Str , "THREAD.ONEBODY(2PS)"  ) ) INDEX = 13; // 2005.10.11
		//
		else {
			//
			INDEX = atoi( Else1Str );
			//
			if ( ( INDEX < 0 ) || ( INDEX > 15 ) ) {
				Event_Message_Reject( "DATA ANALYSIS ERROR" );
				return;
			}
			//
		}
		//
		if ( Else2Str[0] != 0 ) { // 2017.04.21
			//
			LC = strlen( Else2Str );
			//
			for ( SS = 0 ; SS < LC ; SS++ ) {
				//
				if ( SS >= MAX_BM_CHAMBER_DEPTH ) break;
				//
				if ( ( Else2Str[SS] >= '0' ) && ( Else2Str[SS] <= '9' ) ) {
					_i_SCH_PRM_SET_MODULE_BUFFER_LASTORDER( BM1 + SS , Else2Str[SS] - '0' );
				}
				//
			}
			//
		}
		//
		if ( ( _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() != INDEX ) || ( Else2Str[0] != 0 ) ) {
			//
			if ( Else2Str[0] != 0 ) SCH_SW_BM_SCHEDULING_FACTOR_ORDER_SET( Else2Str );
			//
			_i_SCH_PRM_SET_UTIL_SW_BM_SCHEDULING_FACTOR( INDEX );
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//
			Set_RunPrm_IO_Setting( 25 );
			//
		}
		//
	}
	//
	//
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_PREPOST_PROCESS_DEPAND" ) ) { // 2005.02.28
		if ( strlen( ElseStr ) > 0 ) {
			INDEX = atoi( ElseStr );
			if ( ( INDEX >= 0 ) && ( INDEX <= 11 ) ) {
				_i_SCH_PRM_SET_UTIL_PREPOST_PROCESS_DEPAND( INDEX );
			}
			else {
				Event_Message_Reject( "DATA ANALYSIS ERROR" );
				return;
			}
		}
		else {
			Event_Message_Reject( "DATA ANALYSIS ERROR" );
			return;
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SLOT_MAP_NOTUSE" ) ) { // 2016.11.14
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , Else2Str , 255 );
		//
		INDEX = Get_Module_ID_From_String( 2 , Else1Str , 0 );
		//
		if ( INDEX == 0 ) {
			Event_Message_Reject( "MESSAGE FORMAT ERROR" );
			return;
		}
		//
		//
		// 2018.03.27
		//
		if ( ( Else2Str[0] >= 'A' ) && ( Else2Str[0] <= 'Z' ) ) {
			LC = 1;
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , 0 , Else2Str[0] - 'A' );
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , 0 , 0 );
		}
		else if ( ( Else2Str[0] >= 'a' ) && ( Else2Str[0] <= 'z' ) ) {
			LC = 1;
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , 0 , Else2Str[0] - 'a' );
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , 0 , 0 );
		}
		else {
			LC = 0;
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , 0 , 0 );
			_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , 0 , 0 );
		}
		//
		//
		//
		ENDINDEX = strlen( Else2Str + LC );
		//
		for ( SS = 0 ; SS < ENDINDEX ; SS++ ) {
			switch( Else2Str[SS+LC] ) {
			case '1' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 1 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 0 );
				break;
			case '2' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 1 );
				break;
			case '3' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 2 );
				break;
			case '4' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 3 );
				break;
			case '5' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 4 );
				break;
			case '6' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 5 );
				break;
			case '7' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 6 );
				break;
			case '8' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 7 );
				break;
			case '9' :
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE( INDEX , SS + 1 , 0 );
				_i_SCH_PRM_SET_DFIM_SLOT_NOTUSE_DATA( INDEX , SS + 1 , 8 );
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------
//	else if ( STRNCMP_L( RunStr , "INEXP_DUMMY" , 11 ) ) { // 2002.09.13 // 2018.02.08
	else if ( STRNCMP_L( RunStr , "INEXP_DUMMY" , 11 ) || STRNCMP_L( RunStr , "INEXA_DUMMY" , 11 ) ) { // 2002.09.13 // 2018.02.08
//###################################################################################################
#ifndef PM_CHAMBER_60
//###################################################################################################
		//
		SS = STRNCMP_L( RunStr , "INEXP_DUMMY" , 11 ); // 2018.02.08
		//
		//============================================================================
		// 2004.11.17
		//============================================================================
		ENDINDEX = 0;
		//============================================================================
//		if ( ( RunStr[11] >= '1' ) && ( RunStr[11] <= '9' ) ) { // 2006.09.19
		//============================================================================
		if      ( ( RunStr[11] >= '1' ) && ( RunStr[11] <= '9' ) && ( RunStr[12] == 0 ) ) {
			ENDINDEX = RunStr[11] - '0' - 1;
		}
		else if ( ( RunStr[11] >= '1' ) && ( RunStr[11] <= '9' ) && ( RunStr[12] >= '0' ) && ( RunStr[12] <= '9' ) && ( RunStr[13] == 0 ) ) {
			ENDINDEX = ( ( RunStr[11] - '0' ) * 10 ) + ( RunStr[12] - '0' ) - 1;
		}
		//============================================================================
		if ( ENDINDEX <  0             ) ENDINDEX = 0;
		if ( ENDINDEX >= MAX_SDM_STYLE ) ENDINDEX = 0;
		//============================================================================
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , ElseStr , Else2Str , 255 );

		INDEX = atoi(Else1Str) - 1;
		//
		if ( STRCMP_L( ElseStr , "EVENTINC" ) ) { // 2015.01.30 // "INEXP_DUMMY 0|EVENTINC|0 or 1"
			//
			_i_SCH_SDM_Set_MODE_INC_EVENT( atoi( Else2Str ) );
			//
		}
		else if ( STRCMP_L( ElseStr , "MODE" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
//==============================================================================================
// 2007.05.11
//==============================================================================================
//			if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
//				Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
//				return;
//			}
//==============================================================================================
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_MODULE( INDEX , atoi( Else2Str ) );
		}
		else if ( STRCMP_L( ElseStr , "PRE" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( INDEX , ENDINDEX , 0 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "RUN" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( INDEX , ENDINDEX , 0 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "POST" ) ) { // 2004.11.17
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_POST_RECIPE( INDEX , ENDINDEX , 0 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "PREF" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( INDEX , ENDINDEX , 1 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "RUNF" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( INDEX , ENDINDEX , 1 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "POSTF" ) ) { // 2004.11.17
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_POST_RECIPE( INDEX , ENDINDEX , 1 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "PREE" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( INDEX , ENDINDEX , 2 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "RUNE" ) ) {
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( INDEX , ENDINDEX , 2 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "POSTE" ) ) { // 2004.11.17
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_POST_RECIPE( INDEX , ENDINDEX , 2 , Else2Str ); // 2004.11.17
		}
		else if ( STRCMP_L( ElseStr , "RESET" ) ) {
//			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2013.02.15
			if ( ( INDEX < -1 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2013.02.15
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
			_i_SCH_SDM_Set_CHAMBER_RESET_COUNT( INDEX ); // 2012.02.15
//			SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RESET_COUNT( INDEX ); // 2012.02.15
		}
		else if ( STRCMP_L( ElseStr , "INC" ) ) { // 2009.01.12
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_CASSETTE_SLOT_SIZE ) ) {
				Event_Message_Reject( "Slot Data Error" );
				return;
			}
//			_i_SCH_SDM_Set_CHAMBER_INC_COUNT( INDEX ); // 2015.01.30
			_i_SCH_SDM_Set_CHAMBER_INC_COUNT_SUB( INDEX ); // 2015.01.30
		}
		else if ( STRCMP_L( ElseStr , "LOTFIRST" ) ) {
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_PM_CHAMBER_DEPTH ) ) {
				Event_Message_Reject( "PM Data Error" );
				return;
			}
			SCHEDULER_CONTROL_Set_SDM_ins_LOTFIRST_MODULE( INDEX + PM1 , atoi( Else2Str ) );
		}
		else if ( STRCMP_L( ElseStr , "LOTEND" ) ) {
			if ( SS ) { // 2018.02.08
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					Event_Message_Reject( "AUTO or TRANSFER SYSTEM RUNNING ALREADY" );
					return;
				}
			}
			if ( ( INDEX < 0 ) || ( INDEX >= MAX_PM_CHAMBER_DEPTH ) ) {
				Event_Message_Reject( "PM Data Error" );
				return;
			}
			SCHEDULER_CONTROL_Set_SDM_ins_LOTEND_MODULE( INDEX + PM1 , atoi( Else2Str ) );
		}
		else {
			Event_Message_Reject( "Invalid Data" );
		}
//###################################################################################################
#else
//###################################################################################################
		Event_Message_Reject( "Invalid Scheduler Version" );
//###################################################################################################
#endif
//###################################################################################################
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "MCOOL_FORCE_SET_TIME" ) ) { // 2005.12.01
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		INDEX = atoi( Else1Str );
//		if ( ( INDEX < 0 ) || ( INDEX > MAX_CASSETTE_SLOT_SIZE ) ) { // 2007.06.22
		if ( ( INDEX < 0 ) || ( INDEX > ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ) ) { // 2007.06.22
			Event_Message_Reject( "SLOT ANALYSIS ERROR" );
			return;
		}
		ENDINDEX = atoi( Else2Str );
		if ( ENDINDEX < 0 ) ENDINDEX = 0;
		if ( atoi( ElseStr ) == 0 ) {
			if ( INDEX == 0 ) {
//				for ( SS = 1 ; SS <= MAX_CASSETTE_SLOT_SIZE ; SS++ ) { // 2007.06.22
				for ( SS = 1 ; SS <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; SS++ ) { // 2007.06.22
					_i_SCH_MODULE_Set_MFIC_RUN_TIMER( SS , 0 , ENDINDEX , TRUE );
				}
			}
			else {
				_i_SCH_MODULE_Set_MFIC_RUN_TIMER( INDEX , 0 , ENDINDEX , TRUE );
			}
		}
		else { // 2006.04.05
			if ( INDEX == 0 ) {
//				for ( SS = 1 ; SS <= MAX_CASSETTE_SLOT_SIZE ; SS++ ) { // 2007.06.22
				for ( SS = 1 ; SS <= ( MAX_CASSETTE_SLOT_SIZE + MAX_CASSETTE_SLOT_SIZE ) ; SS++ ) { // 2007.06.22
					_i_SCH_MODULE_Set_MFIC_RUN_TIMER( SS , 0 , ENDINDEX , FALSE );
				}
			}
			else {
				_i_SCH_MODULE_Set_MFIC_RUN_TIMER( INDEX , 0 , ENDINDEX , FALSE );
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "SYSTEM_LOG" , 10 ) ) { // 2004.09.07 // 2005.11.30
		//=======================================================================
		// 2006.11.24
		//=======================================================================
		if      ( tolower( RunStr[10] ) == 'c' ) {
			if ( ElseStr[0] != 0 ) {
				KWIN_SCREEN_CAPTURE( GET_WINDOW_X_REFERENCE_POSITION() , GET_WINDOW_Y_REFERENCE_POSITION() ,
					GET_WINDOW_XS_REFERENCE_POSITION() , GET_WINDOW_YS_REFERENCE_POSITION() , ElseStr );
			}
			return;
		}
		//=======================================================================
		// 2013.03.11
		//=======================================================================
		if      ( tolower( RunStr[10] ) == 'w' ) {
			//
			if ( ElseStr[0] == 0 ) {
				SYSTEM_WIN_TAG = 0;
				return;
			}
			//
			if ( SYSTEM_WIN_TAG != 0 ) return;
			//
			SYSTEM_WIN_COUNT = 0;
			SYSTEM_WIN_UPDATE = 0;
			SYSTEM_WIN_DATA = FALSE;
			//
			STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );

			if ( Else1Str[0] == '*' ) { // 2016.12.12
				sprintf( SYSTEM_WIN_FOLDER , Else1Str + 1 , GET_SYSTEM_NAME() );
			}
			else {
				sprintf( SYSTEM_WIN_FOLDER , "%s/%s" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Else1Str );
			}
			//
			_mkdir( SYSTEM_WIN_FOLDER );
			//
			SYSTEM_WIN_TAG = 1;
			//
			if ( DispStr[0] != 0 ) {
				SYSTEM_WIN_DATA = TRUE;
				STR_SEPERATE_CHAR( DispStr , '|' , Else1Str , ElseStr , 255 );	SYSTEM_WIN_X = atoi( Else1Str );
				STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );	SYSTEM_WIN_Y = atoi( Else1Str );
				STR_SEPERATE_CHAR( DispStr , '|' , Else1Str , ElseStr , 255 );	SYSTEM_WIN_XS = atoi( Else1Str );
				STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );	SYSTEM_WIN_YS = atoi( Else1Str );
			}
			//
			if ( _beginthread( (void *) SYSTEM_LOG_WIN_MONITOR , 0 , 0 ) == -1 ) {
				SYSTEM_WIN_TAG = 0;
				Event_Message_Reject( "LOGGING SYSTEM RUNNING FAIL-Thread Fail" );
			}
			//
			return;
		}
		//=======================================================================
		// 2014.07.17
		//=======================================================================
		if      ( tolower( RunStr[10] ) == 'r' ) {
			//
			if ( SYSTEM_WIN_R_TAG == 0 )
				SYSTEM_WIN_R_TAG = 1;
			else
				SYSTEM_WIN_R_TAG = 0;
			//
			return;
		}
		//=======================================================================
		if ( SYSTEM_LOG_TAG != 0 ) {
			Event_Message_Reject( "LOGGING SYSTEM RUNNING ALREADY" );
			return;
		}
		//
		SS = -1;
		//
		SYSTEM_LOG_TAG = 1;
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		if      ( RunStr[10] == '1' ) {
			ENDINDEX = 1;
		}
		else if ( RunStr[10] == '2' ) {
			//====================================================================
			// 2007.04.27
			//====================================================================
			if      ( ( RunStr[11] == '(' ) && ( tolower( RunStr[12] ) == 'f' ) && ( RunStr[13] == ')' ) ) {
				SS = 0;
			}
			else if ( ( RunStr[11] == '(' ) && ( tolower( RunStr[12] ) == 't' ) && ( RunStr[13] == ')' ) ) {
				SS = 1;
			}
			//====================================================================
			INDEX = atoi( Else2Str );
			if ( INDEX < 1000 ) INDEX = 1000;
			//
			strcpy( Else2Str , Else1Str );
			//
			ENDINDEX = 2;
		}
		else if ( tolower( RunStr[10] ) == 't' ) {
			//====================================================================
			// 2007.04.27
			//====================================================================
			if      ( ( RunStr[11] == '(' ) && ( tolower( RunStr[12] ) == 'f' ) && ( RunStr[13] == ')' ) ) {
				SS = 0;
			}
			else if ( ( RunStr[11] == '(' ) && ( tolower( RunStr[12] ) == 't' ) && ( RunStr[13] == ')' ) ) {
				SS = 1;
			}
			//====================================================================
			INDEX = atoi( Else1Str );
			if ( INDEX < 1000 ) INDEX = 1000;
			//
			strcpy( Else1Str , "" );
			strcpy( Else2Str , "" );
			//
			ENDINDEX = 0;
		}
		else {
			//====================================================================
			// 2007.04.27
			//====================================================================
			if      ( ( RunStr[10] == '(' ) && ( tolower( RunStr[11] ) == 'f' ) && ( RunStr[12] == ')' ) ) {
				SS = 0;
			}
			else if ( ( RunStr[10] == '(' ) && ( tolower( RunStr[11] ) == 't' ) && ( RunStr[12] == ')' ) ) {
				SS = 1;
			}
			//====================================================================
			ENDINDEX = 0;
			//
			if      ( ( Else1Str[0] != 0 ) && ( Else2Str[0] == 0 ) ) {
				strcpy( Else2Str , Else1Str );
			}
			else if ( ( Else2Str[0] != 0 ) && ( Else1Str[0] == 0 ) ) {
				strcpy( Else1Str , Else2Str );
			}
			//
			INDEX = atoi( ElseStr );
			if ( INDEX < 1000 ) INDEX = 1000;
		}
		//
		if ( ( ENDINDEX == 0 ) || ( ENDINDEX == 1 ) ) {
//			SCHEDULING_DATA_STRUCTURE_LOG( Else1Str , Else1Str , -1 , 0 ); // 2013.02.20
//			SCHEDULING_DATA_STRUCTURE_JOB_LOG( Else1Str , Else1Str );
			//
			_i_SCH_SYSTEM_INFO_LOG( Else1Str , Else1Str ); // 2013.02.20
			//
			SYSTEM_WIN_INT_DATA = 1; // 2016.07.15
			strcpy( SYSTEM_WIN_STR_DATA , Else1Str ); // 2016.07.15
			//
		}
		else {
			//
			SYSTEM_WIN_INT_DATA = 0; // 2016.07.15
			strcpy( SYSTEM_WIN_STR_DATA , "" ); // 2016.07.15
			//
		}
		//
		if ( ( ENDINDEX == 0 ) || ( ENDINDEX == 2 ) ) {
			if ( IO_DEBUG_FILE_ENABLE( SS , Else2Str , 0 , FALSE ) ) {
				if ( _beginthread( (void *) SYSTEM_LOG_TAG_MONITOR , 0 , (void *) INDEX ) == -1 ) {
					Event_Message_Reject( "LOGGING SYSTEM RUNNING FAIL-Thread Fail" );
					SYSTEM_LOG_TAG = 0;
				}
			}
			else {
				Event_Message_Reject( "LOGGING SYSTEM RUNNING FAIL-Already Running" );
			}
		}
		else {
			SYSTEM_LOG_TAG = 0;
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "MAKE_PM_ENABLE_OTHER_DISABLE" ) ) { // 2006.01.02
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		STR_SEPERATE_CHAR( ElseStr , '|' , JobStr , LotStr , 255 );
		STR_SEPERATE_CHAR( LotStr  , '|' , ElseStr , DispStr , 255 ); // 2006.02.08
		//
		INDEX = atoi( Else1Str );

		Get_PMBM_ID_From_String( 1 , ElseStr , 0 , &ENDINDEX );
		//
		SCHEDULING_CHECK_Change_DataBase_PM_Enable_other_Disable( INDEX , Else2Str , JobStr , ENDINDEX , ElseStr , atoi( DispStr ) );
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_PROCESS_INSIDE_TARGET_TIME" ) ) { // 2006.02.09
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		switch( Get_PMBM_ID_From_String( 1 , Else1Str , 0 , &INDEX ) ) {
		case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
		case 2 :	Event_Message_Reject( "Disable Module Data" );	return;	break;
		}
		//
		if      ( STRCMP_L( Else2Str , "MAIN" ) ) {
			_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 0 , INDEX , atoi( ElseStr ) );
		}
		else if ( STRCMP_L( Else2Str , "PRE" ) ) {
			_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 2 , INDEX , atoi( ElseStr ) );
		}
		else if ( STRCMP_L( Else2Str , "POST" ) ) {
			_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 1 , INDEX , atoi( ElseStr ) );
		}
		else if ( STRCMP_L( Else2Str , "ALL" ) ) {
			_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 0 , INDEX , atoi( ElseStr ) );
			_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 2 , INDEX , atoi( ElseStr ) );
			_i_SCH_TIMER_SET_PROCESS_TIME_TARGET( 1 , INDEX , atoi( ElseStr ) );
		}
		else if ( STRCMP_L( Else2Str , "START" ) ) { // SET_PROCESS_INSIDE_TARGET_TIME PM1|START|0,1,2 // 2012.02.08
			_i_SCH_TIMER_SET_PROCESS_TIME_START( atoi( ElseStr ) , INDEX );
		}
		else if ( STRCMP_L( Else2Str , "END" ) ) { // SET_PROCESS_INSIDE_TARGET_TIME PM1|END|0,1,2 // 2012.02.08
			_i_SCH_TIMER_SET_PROCESS_TIME_END( atoi( ElseStr ) , INDEX , TRUE );
		}
		else {
			Event_Message_Reject( "Invalid Type Data" );
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "SET_FORCE_MAKE_FMSIDE" ) ) { // 2007.10.19
		ENDINDEX = Get_PMBM_ID_From_String( 2 , ElseStr , 0 , &INDEX );
		if ( ENDINDEX == 1 ) {
			_i_SCH_SUB_Make_FORCE_FMSIDE( INDEX );
		}
		else {
			if ( STRCMP_L( ElseStr , "ALL" ) ) {
				_i_SCH_SUB_Make_FORCE_FMSIDE( -1 );
			}
			else {
				Event_Message_Reject( "Invalid Module Data" );
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRNCMP_L( RunStr , "SET_FORCE_RUN_SIDE_" , 19 ) ) { // 2007.12.12
		if      ( STRCMP_L( RunStr + 19 , "FM" ) ) {
			ENDINDEX = BUFFER_FM_STATION;
			SS = 0; // 2016.11.17
		}
		else if ( STRCMP_L( RunStr + 19 , "TM" ) ) {
			ENDINDEX = BUFFER_TM_STATION;
			SS = 0; // 2016.11.17
		}
		else if ( STRCMP_L( RunStr + 19 , "FM(C)" ) ) {
			ENDINDEX = BUFFER_FM_STATION;
			SS = 1; // 2016.11.17
		}
		else if ( STRCMP_L( RunStr + 19 , "TM(C)" ) ) {
			ENDINDEX = BUFFER_TM_STATION;
			SS = 1; // 2016.11.17
		}
		else {
			return;
		}
		if ( Get_PMBM_ID_From_String( 2 , ElseStr , 0 , &INDEX ) == 1 ) {
//			if ( Get_RunPrm_RUNNING_CLUSTER() ) { // 2016.11.17
			if ( ( SS == 0 ) && Get_RunPrm_RUNNING_CLUSTER() ) { // 2016.11.17
//				SIGNAL_MODE_FMTMSIDE_SET( INDEX , ENDINDEX , NULL , -1 ); // 2018.06.22
				SIGNAL_MODE_FMTMSIDE_SET( INDEX , ENDINDEX , NULL , -1 , 0 ); // 2018.06.22
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( INDEX ) && ( _i_SCH_MODULE_Get_BM_FULL_MODE( INDEX ) != ENDINDEX ) ) {
					if ( PROCESS_MONITOR_Run_and_Get_Status( INDEX ) <= 0 ) {
//						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( ENDINDEX , 0 , INDEX , -1 , TRUE , -1 , -1 ); // 2012.07.11
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( ENDINDEX , 0 , INDEX , -1 , TRUE , (ENDINDEX == BUFFER_TM_STATION) ? 0 : 1 , -1 ); // 2012.07.11
					}
					else { // 2016.11.17
						sprintf( DispStr , "%s Fail - Already Running" , RunStr );
						Event_Message_Reject( DispStr );
					}
				}
				else { // 2016.11.17
					sprintf( DispStr , "%s Fail - Already Done" , RunStr );
					Event_Message_Reject( DispStr );
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "DLL_CONTROL" ) ) { // 2008.01.18
		//
//		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 ); // 2016.12.14
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else2Str , Else1Str , 255 ); // 2016.12.14
		STR_SEPERATE_CHAR( Else1Str , '|' , DispStr , Else2Str , 255 ); // 2016.12.14
		//
		if      ( STRCMP_L( ElseStr , "LOAD" ) ) {
			if ( !USER_RECIPE_DLL_LOAD( DispStr , 4 ) ) {
				Event_Message_Reject( "Invalid Operation for LOAD" );
			}
		}
		else if ( STRCMP_L( ElseStr , "LOAD_DISABLE" ) ) {
			if ( !USER_RECIPE_DLL_LOAD( DispStr , 5 ) ) {
				Event_Message_Reject( "Invalid Operation for LOAD_DISABLE" );
			}
		}
		else if ( STRCMP_L( ElseStr , "UNLOAD" ) ) {
			if ( !USER_RECIPE_DLL_LOAD( DispStr , 3 ) ) {
				Event_Message_Reject( "Invalid Operation for UNLOAD" );
			}
		}
		else if ( STRCMP_L( ElseStr , "ENABLE" ) ) {
			if ( !USER_RECIPE_DLL_LOAD( DispStr , 2 ) ) {
				Event_Message_Reject( "Invalid Operation for ENABLE" );
			}
		}
		else if ( STRCMP_L( ElseStr , "DISABLE" ) ) {
			if ( !USER_RECIPE_DLL_LOAD( DispStr , 1 ) ) {
				Event_Message_Reject( "Invalid Operation for DISABLE" );
			}
		}
		else if ( STRCMP_L( ElseStr , "SCH_LOAD" ) ) {
			if ( !USER_SCH_DLL_LOAD( DispStr , Else2Str , 4 ) ) {
				Event_Message_Reject( "Invalid Operation for LOAD" );
			}
		}
		else if ( STRCMP_L( ElseStr , "SCH_LOAD_DISABLE" ) ) {
			if ( !USER_SCH_DLL_LOAD( DispStr , Else2Str , 5 ) ) {
				Event_Message_Reject( "Invalid Operation for LOAD_DISABLE" );
			}
		}
		else if ( STRCMP_L( ElseStr , "SCH_UNLOAD" ) ) {
			if ( !USER_SCH_DLL_LOAD( DispStr , Else2Str , 3 ) ) {
				Event_Message_Reject( "Invalid Operation for UNLOAD" );
			}
		}
		else if ( STRCMP_L( ElseStr , "SCH_ENABLE" ) ) {
			if ( !USER_SCH_DLL_LOAD( DispStr , Else2Str , 2 ) ) {
				Event_Message_Reject( "Invalid Operation for ENABLE" );
			}
		}
		else if ( STRCMP_L( ElseStr , "SCH_DISABLE" ) ) {
			if ( !USER_SCH_DLL_LOAD( DispStr , Else2Str , 1 ) ) {
				Event_Message_Reject( "Invalid Operation for DISABLE" );
			}
		}
		else {
			Event_Message_Reject( "Invalid Operation Data" );
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "CONSOLE_CONTROL" ) ) { // 2009.01.23
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		//
		if      ( STRCMP_L( Else1Str , "ON" ) ) {
			INDEX = TRUE;
		}
		else if ( STRCMP_L( Else1Str , "OFF" ) ) {
			INDEX = FALSE;
		}
		else {
			Event_Message_Reject( "Invalid Operation Data" );
			return;
		}
		//
		STR_SEPERATE_CHAR( DispStr , '|' , Else1Str , Else2Str , 255 );
		//
		_IO_CONSOLE_OPERATION( INDEX , Else1Str , Else2Str );
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "EIL_LOG" ) ) { // 2010.09.15
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		if ( !Event_LOT_ID_From_String_And_Check( Else1Str , &INDEX ) ) return;
		//
		_EIL_LOG_LOT_PRINTF( INDEX , atoi(Else2Str) , ElseStr );
		//
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "USER_FIXED_JOB_OUT_CASSETTE" ) ) { // 2018.07.17
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		//
		if ( !STRCMP_L( DispStr , "" ) ) {
			//
			INDEX = Get_LOT_ID_From_String( Else1Str , TRUE );
			//
			ENDINDEX = Get_LOT_ID_From_String( DispStr , TRUE ) + CM1;
			//
			SS = SCH_PATHOUT_RESET( INDEX , ENDINDEX );
			//
			printf( "USER_FIXED_JOB_OUT_CASSETTE [2] = %d\n" , SS );
			//
		}
		else {
			if      ( STRCMP_L( Else1Str , "?" ) ) {
				USER_FIXED_JOB_OUT_CASSETTE = 0;
			}
			else if ( STRCMP_L( Else1Str , "-" ) ) {
				USER_FIXED_JOB_OUT_CASSETTE = -1;
			}
			else {
				USER_FIXED_JOB_OUT_CASSETTE = Get_LOT_ID_From_String( Else1Str , TRUE ) + CM1;
			}
			//
			printf( "USER_FIXED_JOB_OUT_CASSETTE [1] = %d\n" , USER_FIXED_JOB_OUT_CASSETTE );
			//
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "EIL_CONTROL" ) ) { // 2010.09.15
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		if      ( STRCMP_L( Else1Str , "BEGIN" ) || STRCMP_L( Else1Str , "END" ) || STRCMP_L( Else1Str , "STOP" ) || STRCMP_L( Else1Str , "LOTPREUPDATE" ) || STRCMP_L( Else1Str , "LOG" ) ) {
			//
			if ( !Event_LOT_ID_From_String_And_Check( Else2Str , &INDEX ) ) return;
			//
			ENDINDEX = 0;
			strcpy( DispStr , ElseStr );
			//
		}
		else if ( STRCMP_L( Else1Str , "OPTION" ) ) {
			//
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) return;
			//
			ENDINDEX = atoi( Else2Str );
			//
			if ( ( ENDINDEX > 0 ) && ( ENDINDEX < 256 ) ) _i_SCH_PRM_SET_EIL_INTERFACE_FLOW_USER_OPTION( ENDINDEX , atoi( ElseStr ) );
			//
			return;
		}
		else if ( STRCMP_L( Else1Str , "FINISH" ) ) {
		}
		else {
			switch( Get_PMBM_ID_From_String( 2 , Else2Str , 0 , &INDEX ) ) {
			case 0 :	Event_Message_Reject( "Invalid Module Data" );	return;	break;
			case 2 :	Event_Message_Reject( "Disable Module Data" );	return;	break;
			}
			//
			STR_SEPERATE_CHAR( ElseStr , '|' , Else2Str , DispStr , 255 );
			ENDINDEX = atoi( Else2Str );
		}
		//
		SS = _EIL_EVENT_RECEIVED_OPERATION( Else1Str , INDEX , ENDINDEX , DispStr , PROGRAM_EVENT_READ() );
		//
		if ( SS != 0 ) {
			sprintf( DispStr , "EIL_Fail [%d]" , SS );
			Event_Message_Reject( DispStr );
		}
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "MFI_CONTROL" ) ) { // 2012.12.05
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		if ( STRCMP_L( Else1Str , "OPTION" ) ) {
			//
			ENDINDEX = atoi( Else2Str );
			//
			if ( ( ENDINDEX > 0 ) && ( ENDINDEX < 256 ) ) _i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTION( ENDINDEX , atoi( ElseStr ) );
			//
			return;
		}
		//
	}
	//--------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "EVENT_MESSAGE_LOG" ) ) { // 2015.04.09
		//
		if      ( STRCMP_L( ElseStr , "ON" ) ) {
			EVENT_MESSAGE_LOG = TRUE; // 2015.04.09
			printf( "========================\n" );
			printf( "========================\n" );
			printf( "EVENT_MESSAGE_LOG is ON\n" );
			printf( "========================\n" );
			printf( "========================\n" );
		}
		else if ( STRCMP_L( ElseStr , "OFF" ) ) {
			EVENT_MESSAGE_LOG = FALSE; // 2015.04.09
			printf( "========================\n" );
			printf( "========================\n" );
			printf( "EVENT_MESSAGE_LOG is OFF\n" );
			printf( "========================\n" );
			printf( "========================\n" );
		}
		//
	}
	//------------------------------------------------------------------------------------------
	//
	/*
	//
	// 2015.03.26
	//
	else if ( STRCMP_L( RunStr , "FA_TEST_START" ) ) { // 2015.03.08
		//
void FA_TEST_START(); // 2015.03.09
		FA_TEST_START();
	}
	else if ( STRCMP_L( RunStr , "FA_TEST_STOP" ) ) { // 2015.03.08
		//
void FA_TEST_STOP(); // 2015.03.09
		FA_TEST_STOP();
	}
	else if ( STRCMP_L( RunStr , "FA_TEST_SEND" ) ) { // 2015.03.08
		//
void FA_TEST_SEND( int ); // 2015.03.09
		FA_TEST_SEND( atoi( ElseStr ) );
	}
	//
	else if ( STRCMP_L( RunStr , "XXX" ) ) { // 2015.06.01
		//
void SCHEDULER_CONTROL_Set_GLOBAL_STOP_TEST();
		//
		SCHEDULER_CONTROL_Set_GLOBAL_STOP_TEST();
		//
	}
	*/
	//
	else if ( STRCMP_L( RunStr , "NOWAY_ON" ) ) { // 2015.09.17
		//
		NOWAY_CONTROL = TRUE; // 2015.09.17
		//
		printf( "========================\n" );
		printf( "========================\n" );
		printf( "NOWAY_CONTROL is ON\n" );
		printf( "========================\n" );
		printf( "========================\n" );
		//
	}
	else if ( STRCMP_L( RunStr , "NOWAY_OFF" ) ) { // 2015.09.17
		//
		NOWAY_CONTROL = FALSE; // 2015.09.17
		//
		printf( "========================\n" );
		printf( "========================\n" );
		printf( "NOWAY_CONTROL is OFF\n" );
		printf( "========================\n" );
		printf( "========================\n" );
		//
	}
	//
	else if ( STRNCMP_L( RunStr , "NOPICKCM_ON" , 11 ) ) { // 2016.12.05
		//
		if ( RunStr[11] == 0 ) {
			printf( "========================\n" );
			printf( "========================\n" );
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) {
				_SCH_SYSTEM_NO_PICK_FROM_CM_SET( SS , TRUE );
				printf( "NO_PICK_FROM_CM(%d) is ON\n" , SS );
			}
			printf( "========================\n" );
			printf( "========================\n" );
		}
		else {
			SS = atoi( RunStr + 11 ) - 1;
			//
			if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
				_SCH_SYSTEM_NO_PICK_FROM_CM_SET( SS , TRUE );
				//
				printf( "========================\n" );
				printf( "========================\n" );
				printf( "NO_PICK_FROM_CM(%d) is ON\n" , SS );
				printf( "========================\n" );
				printf( "========================\n" );
				//
			}
		}
		//
	}
	else if ( STRNCMP_L( RunStr , "NOPICKCM_OFF" , 12 ) ) { // 2016.12.05
		//
		if ( RunStr[12] == 0 ) {
			printf( "========================\n" );
			printf( "========================\n" );
			for ( SS = 0 ; SS < MAX_CASSETTE_SIDE ; SS++ ) {
				_SCH_SYSTEM_NO_PICK_FROM_CM_SET( SS , FALSE );
				//
				printf( "NO_PICK_FROM_CM(%d) is OFF\n" , SS );
				//
			}
			printf( "========================\n" );
			printf( "========================\n" );
		}
		else {
			SS = atoi( RunStr + 12 ) - 1;
			//
			if ( ( SS >= 0 ) && ( SS < MAX_CASSETTE_SIDE ) ) {
				_SCH_SYSTEM_NO_PICK_FROM_CM_SET( SS , FALSE );
				//
				printf( "========================\n" );
				printf( "========================\n" );
				printf( "NO_PICK_FROM_CM(%d) is OFF\n" , SS );
				printf( "========================\n" );
				printf( "========================\n" );
				//
			}
		}
		//
	}
	else if ( STRCMP_L( RunStr , "MCC_REPORT" ) ) { // 2017.01.18
		STR_SEPERATE_CHAR( ElseStr  , '|' , DispStr , Else2Str , 255 );	SS = atoi( DispStr );		if ( DispStr[0] == '-' ) SS = -1;
		STR_SEPERATE_CHAR( Else2Str , '|' , DispStr , Else1Str , 255 );	ES = atoi( DispStr );		if ( DispStr[0] == '-' ) SS = -1;
		STR_SEPERATE_CHAR( Else1Str , '|' , DispStr , Else2Str , 255 );	LC = atoi( DispStr );		if ( DispStr[0] == '-' ) SS = -1;
		STR_SEPERATE_CHAR( Else2Str , '|' , DispStr , Else1Str , 255 );	INDEX = atoi( DispStr );	if ( DispStr[0] == '-' ) INDEX = -1;
		EST_CONTROL_RUN( SS , ES , LC , INDEX );
	}
	else if ( STRNCMP_L( RunStr , "SIMTIME" , 7 ) ) { // 2016.12.09
		//
		if ( RunStr[7] == 0 ) {
			//
			STR_SEPERATE( ElseStr , JobStr , Else1Str , 255 );
			STR_SEPERATE_CHAR( Else1Str , '|' , DispStr , Else2Str , 255 );	SS = atoi( DispStr );	if ( DispStr[0] == '-' ) SS = -1;
			STR_SEPERATE_CHAR( Else2Str , '|' , DispStr , Else1Str , 255 );	ES = atoi( DispStr );	if ( DispStr[0] == '-' ) ES = -1;
			STR_SEPERATE_CHAR( Else1Str , '|' , DispStr , Else2Str , 255 );	LC = atoi( DispStr );	if ( DispStr[0] == '-' ) LC = -1;
			//
			if      ( STRCMP_L( JobStr , "RESET" ) ) {
				_i_SCH_PRM_SET_SIM_TIME1( FM , 0 );
				_i_SCH_PRM_SET_SIM_TIME2( FM , 0 );
				_i_SCH_PRM_SET_SIM_TIME3( FM , 0 );
				for ( INDEX = BM1 ; INDEX < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; INDEX++ ) {
					_i_SCH_PRM_SET_SIM_TIME1( INDEX , 0 );
					_i_SCH_PRM_SET_SIM_TIME2( INDEX , 0 );
					_i_SCH_PRM_SET_SIM_TIME3( INDEX , 0 );
				}
				for ( INDEX = TM ; INDEX < ( TM + MAX_TM_CHAMBER_DEPTH ) ; INDEX++ ) {
					_i_SCH_PRM_SET_SIM_TIME1( INDEX , 0 );
					_i_SCH_PRM_SET_SIM_TIME2( INDEX , 0 );
					_i_SCH_PRM_SET_SIM_TIME3( INDEX , 0 );
				}
				for ( INDEX = PM1 ; INDEX < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; INDEX++ ) {
					_i_SCH_PRM_SET_SIM_TIME1( INDEX , 0 );
					_i_SCH_PRM_SET_SIM_TIME2( INDEX , 0 );
					_i_SCH_PRM_SET_SIM_TIME3( INDEX , 0 );
				}
			}
			else if ( STRCMP_L( JobStr , "SET" ) ) {
				if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( FM , SS );
				if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( FM , ES );
				if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( FM , LC );
				for ( INDEX = BM1 ; INDEX < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; INDEX++ ) {
					if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
					if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
					if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
				}
				for ( INDEX = TM ; INDEX < ( TM + MAX_TM_CHAMBER_DEPTH ) ; INDEX++ ) {
					if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
					if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
					if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
				}
				for ( INDEX = PM1 ; INDEX < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; INDEX++ ) {
					if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
					if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
					if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
				}
			}
			else if ( STRNCMP_L( JobStr , "FM" , 2 ) ) {
				if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( FM , SS );
				if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( FM , ES );
				if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( FM , LC );
			}
			else if ( STRNCMP_L( JobStr , "BM" , 2 ) ) {
				if ( JobStr[2] == 0 ) {
					for ( INDEX = BM1 ; INDEX < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; INDEX++ ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
					}
				}
				else {
					//
					INDEX = atoi( JobStr + 2 ) - 1 + BM1;
					//
					if ( ( INDEX >= BM1 ) && ( ( INDEX - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
					}
					//
				}
			}
			else if ( STRNCMP_L( JobStr , "TM" , 2 ) ) {
				if ( JobStr[2] == 0 ) {
					for ( INDEX = TM ; INDEX < ( TM + MAX_TM_CHAMBER_DEPTH ) ; INDEX++ ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
					}
				}
				else {
					//
					INDEX = atoi( JobStr + 2 ) - 1 + TM;
					//
					if ( ( INDEX >= TM ) && ( ( INDEX - TM + 1 ) <= MAX_TM_CHAMBER_DEPTH ) ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
					}
					//
				}
			}
			else if ( STRNCMP_L( JobStr , "PM" , 2 ) ) {
				if ( JobStr[2] == 0 ) {
					for ( INDEX = PM1 ; INDEX < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; INDEX++ ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
					}
				}
				else {
					//
					INDEX = atoi( JobStr + 2 ) - 1 + PM1;
					//
					if ( ( INDEX >= PM1 ) && ( ( INDEX - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_SIM_TIME1( INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_SIM_TIME2( INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_SIM_TIME3( INDEX , LC );
					}
					//
				}
			}
			//
		}
		else {
			//
			ENDINDEX = atoi( RunStr + 7 ) - 1;
			//
			if ( ( ENDINDEX >= 0 ) && ( ENDINDEX < MAX_CASSETTE_SIDE ) ) {
				//
				STR_SEPERATE( ElseStr , JobStr , Else1Str , 255 );
				STR_SEPERATE_CHAR( Else1Str , '|' , DispStr , Else2Str , 255 );	SS = atoi( DispStr );	if ( DispStr[0] == '-' ) SS = -1;
				STR_SEPERATE_CHAR( Else2Str , '|' , DispStr , Else1Str , 255 );	ES = atoi( DispStr );	if ( DispStr[0] == '-' ) ES = -1;
				STR_SEPERATE_CHAR( Else1Str , '|' , DispStr , Else2Str , 255 );	LC = atoi( DispStr );	if ( DispStr[0] == '-' ) LC = -1;
				//
				if      ( STRCMP_L( JobStr , "RESET" ) ) {
					_i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , FM , 0 );
					_i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , FM , 0 );
					_i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , FM , 0 );
					for ( INDEX = BM1 ; INDEX < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; INDEX++ ) {
						_i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , 0 );
						_i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , 0 );
						_i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , 0 );
					}
					for ( INDEX = TM ; INDEX < ( TM + MAX_TM_CHAMBER_DEPTH ) ; INDEX++ ) {
						_i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , 0 );
						_i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , 0 );
						_i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , 0 );
					}
					for ( INDEX = PM1 ; INDEX < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; INDEX++ ) {
						_i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , 0 );
						_i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , 0 );
						_i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , 0 );
					}
				}
				else if ( STRCMP_L( JobStr , "SET" ) ) {
					if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , FM , SS );
					if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , FM , ES );
					if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , FM , LC );
					for ( INDEX = BM1 ; INDEX < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; INDEX++ ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
					}
					for ( INDEX = TM ; INDEX < ( TM + MAX_TM_CHAMBER_DEPTH ) ; INDEX++ ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
					}
					for ( INDEX = PM1 ; INDEX < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; INDEX++ ) {
						if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
						if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
						if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
					}
				}
				else if ( STRNCMP_L( JobStr , "FM" , 2 ) ) {
					if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , FM , SS );
					if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , FM , ES );
					if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3(ENDINDEX ,  FM , LC );
				}
				else if ( STRNCMP_L( JobStr , "BM" , 2 ) ) {
					if ( JobStr[2] == 0 ) {
						for ( INDEX = BM1 ; INDEX < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; INDEX++ ) {
							if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
							if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
							if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
						}
					}
					else {
						//
						INDEX = atoi( JobStr + 2 ) - 1 + BM1;
						//
						if ( ( INDEX >= BM1 ) && ( ( INDEX - BM1 + 1 ) <= MAX_BM_CHAMBER_DEPTH ) ) {
							if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
							if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
							if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
						}
						//
					}
				}
				else if ( STRNCMP_L( JobStr , "TM" , 2 ) ) {
					if ( JobStr[2] == 0 ) {
						for ( INDEX = TM ; INDEX < ( TM + MAX_TM_CHAMBER_DEPTH ) ; INDEX++ ) {
							if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
							if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
							if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
						}
					}
					else {
						//
						INDEX = atoi( JobStr + 2 ) - 1 + TM;
						//
						if ( ( INDEX >= TM ) && ( ( INDEX - TM + 1 ) <= MAX_TM_CHAMBER_DEPTH ) ) {
							if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
							if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
							if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
						}
						//
					}
				}
				else if ( STRNCMP_L( JobStr , "PM" , 2 ) ) {
					if ( JobStr[2] == 0 ) {
						for ( INDEX = PM1 ; INDEX < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; INDEX++ ) {
							if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
							if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
							if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
						}
					}
					else {
						//
						INDEX = atoi( JobStr + 2 ) - 1 + PM1;
						//
						if ( ( INDEX >= PM1 ) && ( ( INDEX - PM1 + 1 ) <= MAX_PM_CHAMBER_DEPTH ) ) {
							if ( SS >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME1( ENDINDEX , INDEX , SS );
							if ( ES >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME2( ENDINDEX , INDEX , ES );
							if ( LC >= 0 ) _i_SCH_PRM_SET_inside_SIM_TIME3( ENDINDEX , INDEX , LC );
						}
						//
					}
				}
				//
			}
		}
	}
	//------------------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "ERROR_CONTROL" ) ) { // 2018.09.20 ERROR_CONTROL PICK|1|2
		//
		STR_SEPERATE_CHAR( ElseStr , '|' , Else1Str , DispStr , 255 );
		STR_SEPERATE_CHAR( DispStr , '|' , Else2Str , ElseStr , 255 );
		//
		INDEX = atoi( Else2Str ) - 1;
		//
		if ( ( INDEX < 0 ) || ( INDEX >= MAX_TM_CHAMBER_DEPTH ) ) return;

		if ( STRCMP_L( Else1Str , "PICK" ) ) {
			_SCH_MACRO_TM_ERROR_PICK_MODE[INDEX] = atoi(ElseStr);
		}
		else if ( STRCMP_L( Else1Str , "PLACE" ) ) {
			_SCH_MACRO_TM_ERROR_PLACE_MODE[INDEX] = atoi(ElseStr);
		}
		//

	}
	//------------------------------------------------------------------------------------------
	else if ( STRCMP_L( RunStr , "REPORT_BUTTON_BITMAP" ) ) { // 2017.02.22
		KWIN_REPORT_USER_BUTTON_BITMAP();
	}
	else if ( STRCMP_L( RunStr , "MODULE_FLOW_MAP_CHART" ) ) { // 2019.05.13
		_FLOW_MAP_CHART();
	}
	//------------------------------------------------------------------------------------------
	else {
		//
		switch( _SCH_COMMON_EVENT_ANALYSIS( PROGRAM_EVENT_READ() , RunStr , ElseStr , Else1Str ) ) {
		case SYS_SUCCESS : 
			if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
			PROGRAM_EVENT_STATUS_SET( SYS_SUCCESS );
			break;
		case SYS_ABORTED : 
			if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
			PROGRAM_EVENT_STATUS_SET( SYS_ABORTED );
			break;
		case SYS_ERROR : 
			if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
			PROGRAM_EVENT_STATUS_SET( SYS_ERROR );
			break;
		case 1 :
			break;
		case 0 :
			if ( strlen( Else1Str ) > 0 ) Event_Message_Reject( Else1Str );
			break;
		default :
			Event_Message_Reject( "CAN NOT ANALYSIS MESSAGE" );
			break;
		}
		//
	}
	//------------------------------------------------------------------------------------------
}

