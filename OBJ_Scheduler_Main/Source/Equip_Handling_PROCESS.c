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
#include "sch_prm.h"
#include "sch_prepost.h"
#include "sch_multi_alic.h"
#include "multijob.h"
#include "iolog.h"
#include "sch_processmonitor.h"
#include "Equip_Handling.h"
#include "FA_Handling.h"
#include "RcpFile_Handling.h"
#include "Utility.h"
#include "sch_sub.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_estimate.h"

#include "INF_sch_commonuser.h"

//
int EQUIP_STATUS_PROCESS( int Side , int Chamber ) {
	if ( Chamber >= BM1 ) {
		if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return SYS_SUCCESS;
		return _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
	}
	else {
		if ( Address_FUNCTION_PROCESS[Side][Chamber] < 0 ) return SYS_SUCCESS;
		return _dREAD_FUNCTION( Address_FUNCTION_PROCESS[Side][Chamber] );
	}
	return SYS_SUCCESS;
}
//
int EQUIP_WAIT_PROCESS( int Side , int Chamber ) {
	int res;
	int l_c; // 2008.09.14
	l_c = 0; // 2008.09.14
	while( TRUE ) {
		res = EQUIP_STATUS_PROCESS( Side , Chamber );
		if ( res != SYS_RUNNING ) return res;
		// Sleep(1); // 2008.09.14
		if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
		l_c++; // 2008.09.14
	}
}
//
int EQUIP_STATUS_PROCESS_OF_OPTION( int Side , int Chamber , int *failout , int *finishout , int *disappear , int *fail_scenario , int *success_result , int *fail_result , int *abort_result , int *pre_time , int *prcs_time , int *post_time ) {
//	char RetBuffer[256]; // 2011.03.18
	char RetBuffer[1024]; // 2011.03.18
//	char ImsiBuffer[32]; // 2006.04.14
	char ImsiBuffer[256]; // 2006.04.14
	int  n , z1 , z2;
	*failout = FALSE;
	*disappear = FALSE;
	*finishout = 0; // 2006.03.28

	*pre_time = -1; // 2006.02.13
	*prcs_time = -1; // 2006.02.13
	*post_time = -1; // 2006.02.13

	if ( Chamber >= BM1 ) {
		return PRCS_RET_OPTION_NONE;
	}
	else {
//		if ( Address_FUNCTION_PROCESS[Side][Chamber] < 0 ) return PRCS_RET_OPTION_NONE; // 2017.01.03
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] Chamber=%d\n" , Side + 1 , "EQUIP_STATUS_PROCESS_OF_OPTION" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber );
		}
		//======================================================================================================================================================

		if ( Address_FUNCTION_PROCESS[Side][Chamber] < 0 ) { // 2017.01.03
			//
			if ( !_SCH_COMMON_GET_UPLOAD_MESSAGE( 11 , Chamber , RetBuffer ) ) { // 2017.01.03
				return PRCS_RET_OPTION_NONE;
			}
			//
		}
		else {
			//
			if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) { // 2017.01.03
				if ( !_SCH_COMMON_GET_UPLOAD_MESSAGE( 21 , Chamber , RetBuffer ) ) { // 2017.01.03
					_dREAD_UPLOAD_MESSAGE( Address_FUNCTION_PROCESS[Side][Chamber] , RetBuffer );
				}
			}
			else {
				if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) ) { // 2017.04.01
					if ( !_SCH_COMMON_GET_UPLOAD_MESSAGE( 31 , Chamber , RetBuffer ) ) {
						_dREAD_UPLOAD_MESSAGE( Address_FUNCTION_PROCESS[Side][Chamber] , RetBuffer );
					}
				}
				else {
					if ( !_SCH_COMMON_GET_UPLOAD_MESSAGE( 1 , Chamber , RetBuffer ) ) { // 2017.01.03
						_dREAD_UPLOAD_MESSAGE( Address_FUNCTION_PROCESS[Side][Chamber] , RetBuffer );
					}
				}
			}
			//
		}
		if ( strlen( RetBuffer ) <= 0 ) {
			//======================================================================================================================================================
			// 2008.09.27
			//======================================================================================================================================================
			if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
				_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d Res=%d\n" , Side + 1 , "EQUIP_STATUS_PROCESS_OF_OPTION" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , PRCS_RET_OPTION_NONE );
			}
			//======================================================================================================================================================
			return PRCS_RET_OPTION_NONE;
		}
		z1 = 0;
		n = PRCS_RET_OPTION_NONE;
		while( TRUE ) {
			z2 = STR_SEPERATE_CHAR_WITH_POINTER( RetBuffer , '|' , ImsiBuffer , z1 , 31 );
			if ( z1 == z2 ) break;
			if      ( STRCMP_L( ImsiBuffer , "PRESKIP" ) ) {
				switch( n ) {
				case PRCS_RET_OPTION_NONE		: n = PRCS_RET_OPTION_PRESKIP; break;
				case PRCS_RET_OPTION_POSTSKIP	: n = PRCS_RET_OPTION_POSTPRESKIP; break;
				}
			}
			else if ( STRCMP_L( ImsiBuffer , "POSTSKIP" ) ) {
				switch( n ) {
				case PRCS_RET_OPTION_NONE		: n = PRCS_RET_OPTION_POSTSKIP; break;
				case PRCS_RET_OPTION_PRESKIP	: n = PRCS_RET_OPTION_POSTPRESKIP; break;
				}
			}
			else if ( STRCMP_L( ImsiBuffer , "FAILOUT" ) ) {
				*failout = TRUE;
			}
			else if ( STRCMP_L( ImsiBuffer , "FINISHALL" ) ) { // 2006.03.28
				*finishout = 1;
			}
			else if ( STRCMP_L( ImsiBuffer , "FINISHOUT" ) ) { // 2006.03.28
				*finishout = 2;
			}
			else if ( STRCMP_L( ImsiBuffer , "RESETALL" ) ) { // 2006.03.28
				*finishout = 3;
			}
			else if ( STRCMP_L( ImsiBuffer , "RESET" ) ) { // 2006.03.28
				*finishout = 4;
			}
			else if ( STRCMP_L( ImsiBuffer , "DUMMYRUN" ) ) {
				*failout = TRUE;
			}
			else if ( STRCMP_L( ImsiBuffer , "DISAPPEAR" ) ) {
				*disappear = TRUE;
			}
			else if ( STRNCMP_L( ImsiBuffer , "FAILSCENARIO" , 12 ) ) { // 2003.10.15
				if ( strlen( ImsiBuffer + 12 ) >= 1 ) {
					*fail_scenario = atoi( ImsiBuffer + 12 ); 
				}
			}
			else if ( STRNCMP_L( ImsiBuffer , "SUCCESSRESULT" , 13 ) ) { // 2003.10.15
				if ( strlen( ImsiBuffer + 13 ) >= 1 ) {
					*success_result = atoi( ImsiBuffer + 13 ); 
				}
			}
			else if ( STRNCMP_L( ImsiBuffer , "FAILRESULT" , 10 ) ) { // 2003.10.15
				if ( strlen( ImsiBuffer + 10 ) >= 1 ) {
					*fail_result = atoi( ImsiBuffer + 10 ); 
				}
			}
			else if ( STRNCMP_L( ImsiBuffer , "ABORTRESULT" , 11 ) ) { // 2003.10.15
				if ( strlen( ImsiBuffer + 11 ) >= 1 ) {
					*abort_result = atoi( ImsiBuffer + 11 ); 
				}
			}
			else if ( STRNCMP_L( ImsiBuffer , "PRETIME" , 7 ) ) { // 2006.02.13
				if ( strlen( ImsiBuffer + 7 ) >= 1 ) {
					*pre_time = atoi( ImsiBuffer + 7 ); 
				}
			}
			else if ( STRNCMP_L( ImsiBuffer , "PRCSTIME" , 8 ) ) { // 2006.02.13
				if ( strlen( ImsiBuffer + 8 ) >= 1 ) {
					*prcs_time = atoi( ImsiBuffer + 8 ); 
				}
			}
			else if ( STRNCMP_L( ImsiBuffer , "POSTTIME" , 8 ) ) { // 2006.02.13
				if ( strlen( ImsiBuffer + 8 ) >= 1 ) {
					*post_time = atoi( ImsiBuffer + 8 ); 
				}
			}
			z1 = z2;
		}
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,failout=%d,finishout=%d,disappear=%d,fail_scenario=%d,success_result=%d,fail_result=%d,abort_result=%d,pre_time=%d,prcs_time=%d,post_time=%d,Res=%d\n" , Side + 1 , "EQUIP_STATUS_PROCESS_OF_OPTION" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , *failout , *finishout , *disappear , *fail_scenario , *success_result , *fail_result , *abort_result , *pre_time , *prcs_time , *post_time , n );
		}
		//======================================================================================================================================================
		return n;
	}
}
//===================================================================================================================================================
BOOL EQUIP_MAIN_PROCESS_SKIP_RUN( int Chamber ) { // 2006.01.06
	if ( _i_SCH_PRM_GET_Process_Run_In_Mode( Chamber ) == 4 ) return TRUE;
	return FALSE;
}
//===================================================================================================================================================
BOOL EQUIP_POST_PROCESS_SKIP_RUN( int Chamber ) { // 2006.01.06
	if ( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) == 4 ) return TRUE;
	return FALSE;
}
//===================================================================================================================================================
BOOL EQUIP_POST_PROCESS_FORCE_RUN( int Chamber ) { // 2006.01.06
	switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
	case 1 : // runa
	case 3 : // sima
		return TRUE;
		break;
	}
	return FALSE;
}

//===================================================================================================================================================
BOOL EQUIP_NORMAL_CONDITION( int Chamber , char *filename ) { // 2016.12.10
	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) {
		if ( ( filename[0] != 'T' ) || ( filename[1] != '*' ) ) return TRUE;
	}
	return FALSE;
}

void EQUIP_TIME_STRING( int CHECKING_SIDE , int Chamber , int mode , int simmode , BOOL inmode , char *filename , char *header ) { // 2016.12.10
	int trg;
	//
	switch( simmode ) {
	case 2 :
		trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , Chamber );
		break;
	case 3 :
		trg = _SCH_Get_SIM_TIME3( CHECKING_SIDE , Chamber );
		break;
	default :
		trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , Chamber );
		break;
	}
	//
	if ( trg > 0 ) {
		sprintf( header , "TIME %.3f:%d" , (double) ( (double) trg / (double) 1000 ) , mode );
	}
	else {
		//
		if ( ( filename[0] == 'T' ) && ( filename[1] == '*' ) ) {
			//
			trg = atoi( filename + 2 );
			//
			if ( trg > 0 ) {
				sprintf( header , "TIME %d:%d" , trg , mode );
				return;
			}
		}
		//
		if ( inmode ) {
			sprintf( header , "TIME %d:%d" , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , mode );
		}
		else {
			sprintf( header , "TIME %d:%d" , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , mode );
		}
	}
	//
}

//===================================================================================================================================================
//BOOL EQUIP_SPAWN_PROCESS_GET_HEADER( int Chamber , char *RunName , int mode , char *header ) { // 2005.10.16 // 2015.04.23
BOOL EQUIP_SPAWN_PROCESS_GET_HEADER( int CHECKING_SIDE , int Chamber , char *RunName , int mode , char *header , int *rid ) { // 2005.10.16 // 2015.04.23
	if      ( mode == 0 ) { // Main Process
		switch( _i_SCH_PRM_GET_Process_Run_In_Mode( Chamber ) ) {
		case 0 : // run
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_IN" );
					*rid = 0; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 1 : 2; // 2015.04.23
				}
			}
			else {
				*rid = 2; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 : // runa
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_IN" );
				*rid = 3; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 4 : 5; // 2015.04.23
			}
			break;
		case 2 : // sim
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
				//
				*rid = 6; // 2015.04.23
			}
			else {
				*rid = 6; // 2015.04.23
				return FALSE;
			}
			break;
		case 3 : // sima
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
			*rid = 7; // 2015.04.23
			break;
		default :
			*rid = 9; // 2015.04.23
			return FALSE;
			break;
		}
	}
	else if ( mode == 1 ) { // Post Process
		switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
		case 0 : // run
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_OUT" );
					*rid = 10; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 11 : 12; // 2015.04.23
				}
			}
			else {
				*rid = 12; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 : // runa
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_OUT" );
				*rid = 13; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 14 : 15; // 2015.04.23
			}
			break;
		case 2 : // sim
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = 16; // 2015.04.23
			}
			else {
				*rid = 16; // 2015.04.23
				return FALSE;
			}
			break;
		case 3 : // sima
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
			//
			*rid = 17; // 2015.04.23
			break;
		default :
			*rid = 19; // 2015.04.23
			return FALSE;
			break;
		}
	}
	else if ( mode == 2 ) { // Pre Process
		switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
		case 0 : // run
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_PRE" );
					*rid = 20; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 21 : 22; // 2015.04.23
				}
			}
			else {
				*rid = 22; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 : // runa
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_PRE" );
				*rid = 23; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 24 : 25; // 2015.04.23
			}
			break;
		case 2 : // sim
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = 26; // 2015.04.23
			}
			else {
				*rid = 26; // 2015.04.23
				return FALSE;
			}
			break;
		case 3 : // sima
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
			//
			*rid = 27; // 2015.04.23
			break;
		default :
			*rid = 29; // 2015.04.23
			return FALSE;
			break;
		}
	}
	else if ( mode == 11 ) { // Ready
		switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
		case 0 : // run
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_OUT/B" );
					*rid = 30; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 31 : 32; // 2015.04.23
				}
			}
			else {
				*rid = 32; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 : // sim
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_OUT/B" );
				*rid = 33; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 34 : 35; // 2015.04.23
			}
			break;
		case 2 : // runa
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = 36; // 2015.04.23
			}
			else {
				*rid = 36; // 2015.04.23
				return FALSE;
			}
			break;
		default :
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 3 , FALSE , RunName , header ); // 2016.12.10
			//
			*rid = 39; // 2015.04.23
			break;
		}
	}
	else if ( mode == 12 ) { // End
		switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
		case 0 :
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_OUT/E" );
					*rid = 40; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 41 : 42; // 2015.04.23
				}
			}
			else {
				*rid = 42; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 :
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_OUT/E" );
				*rid = 43; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 44 : 45; // 2015.04.23
			}
			break;
		case 2 :
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = 46; // 2015.04.23
			}
			else {
				*rid = 46; // 2015.04.23
				return FALSE;
			}
			break;
		default :
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
			//
			*rid = 49; // 2015.04.23
			break;
		}
	}
	else if ( mode == 13 ) { // SDummy at Transfer
		switch( _i_SCH_PRM_GET_Process_Run_In_Mode( Chamber ) ) {
		case 0 :
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_IN/D" );
					*rid = 50; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 51 : 52; // 2015.04.23
				}
			}
			else {
				*rid = 52; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 : // runa
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_IN/D" );
				*rid = 53; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 54 : 55; // 2015.04.23
			}
			break;
		default :
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 1 , TRUE , RunName , header ); // 2016.12.10
			//
			*rid = 59; // 2015.04.23
			break;
		}
	}
	else if ( mode == 14 ) { // SDummy at Transfer // 2002.05.28
		switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
		case 0 :
			if ( strlen( RunName ) > 0 ) {
//				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
				if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
					strcpy( header , "PROCESS_OUT/D" );
					*rid = 60; // 2015.04.23
				}
				else {
					EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
					//
					*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 61 : 62; // 2015.04.23
				}
			}
			else {
				*rid = 62; // 2015.04.23
				return FALSE;
			}
			break;
		case 1 :
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				strcpy( header , "PROCESS_OUT/D" );
				*rid = 63; // 2015.04.23
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 64 : 65; // 2015.04.23
			}
			break;
		case 2 :
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = 66; // 2015.04.23
			}
			else {
				*rid = 66; // 2015.04.23
				return FALSE;
			}
			break;
		default :
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
			//
			*rid = 69; // 2015.04.23
			break;
		}
	}
	else if ( mode == 99 ) { // END Append
		switch( _i_SCH_PRM_GET_Process_Run_Out_Mode( Chamber ) ) {
		case 0 :
		case 1 :
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) )	strcpy( header , "PROCESS_OUT/FN" ); // 2005.12.30 (Bug - Old Version Miss)
//			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) { // 2016.12.10
			if ( EQUIP_NORMAL_CONDITION( Chamber , RunName ) ) { // 2016.12.10
				if ( ( strlen( RunName ) <= 0 ) || ( strcmp( RunName , "?" ) == 0 ) ) {
					strcpy( header , "PROCESS_OUT/FN" ); // 2005.12.30 (Bug - Old Version Miss)
					*rid = 90; // 2015.04.23
				}
				else {
					strcpy( header , "PROCESS_OUT/FY" ); // 2005.12.30 (Bug - Old Version Miss)
					*rid = 91; // 2015.04.23
				}
			}
			else {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = ( GET_RUN_LD_CONTROL(8) <= 0 ) ? 92 : 93; // 2015.04.23
			}
			break;
		case 2 :
			if ( strlen( RunName ) > 0 ) {
				EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
				//
				*rid = 94; // 2015.04.23
			}
			else {
				*rid = 94; // 2015.04.23
				return FALSE;
			}
			break;
		default :
			EQUIP_TIME_STRING( CHECKING_SIDE , Chamber , mode , 2 , FALSE , RunName , header ); // 2016.12.10
			//
			*rid = 99; // 2015.04.23
			break;
		}
	}
	else { // 2015.04.23
		*rid = 98; // 2015.04.23
		return FALSE; // 2015.04.23
	}
	return TRUE;
}

//void EQUIP_SPAWN_PROCESS_GET_INFO( int side , int pointer , int Slot , int Chamber , char *data ) { // 2006.04.06 // 2016.04.26
void EQUIP_SPAWN_PROCESS_GET_INFO( int side , int pointer , int Slot , int Chamber , int ChamberS , char *data ) { // 2006.04.06 // 2016.04.26
	//=================================================================================
	char buffer[512];
	int i , j; // 2010.08.30
	int s , p;
	//=================================================================================
	switch( _i_SCH_PRM_GET_PROCESS_MESSAGE_STYLE() ) { // 2010.09.02
	case 1 :
		sprintf( data , "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%s/%d/%d/%d/%d/%d/%d/" ,
			_i_SCH_CLUSTER_Get_PathIn( side , pointer ) ,
			_i_SCH_CLUSTER_Get_PathOut( side , pointer ) ,
			_i_SCH_CLUSTER_Get_SlotIn( side , pointer ) ,
			_i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ,
			_i_SCH_CLUSTER_Get_PathRange( side , pointer ) ,
			_i_SCH_CLUSTER_Get_PathDo( side , pointer ) ,
//			_i_SCH_CLUSTER_Get_PathHSide( side , pointer ) , // 2018.11.22
			_i_SCH_CLUSTER_Get_PathHSide( side , pointer ) % 1000000 , // 2018.11.22
			_i_SCH_CLUSTER_Get_SwitchInOut( side , pointer ) ,
			_i_SCH_CLUSTER_Get_FailOut( side , pointer ) ,
			_i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , // 2010.07.15
			_i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , side ) , // 2010.09.21
			_i_SCH_CLUSTER_Get_Buffer( side , pointer ) ,
			_i_SCH_CLUSTER_Get_Stock( side , pointer ) ,
			_i_SCH_CLUSTER_Get_Extra( side , pointer ) ,
			_i_SCH_CLUSTER_Get_Optimize( side , pointer ) ,
			_i_SCH_CLUSTER_Get_LotSpecial( side , pointer )
			);
		//=================================================================================
		// 2010.08.30
		//=================================================================================
		if ( ( Slot / 100 ) <= 0 ) { // 2012.02.18
			//
			j = SCHMULTI_MESSAGE_GET_PPID( side , pointer , 1 , 0 , Slot , FALSE , Chamber , buffer , 255 ); // 2012.02.18
			//
			UTIL_CHANGE_FILE_VALID_STYLE( buffer );
			//
			strcat( data , buffer );
			//
			for ( i = 0 ; i < ( MAX_PM_SLOT_DEPTH - 1 ) ; i++ ) {
				if ( _i_SCH_MODULE_Get_PM_WAFER( Chamber , i + 1 ) > 0 ) {
					//
					s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , i+1 );
					p = _i_SCH_MODULE_Get_PM_POINTER( Chamber , i+1 );
					//
					if ( !SCHMULTI_MESSAGE_GET_PPID( s , p , 1 , i + 1 , _i_SCH_CLUSTER_Get_SlotIn( s , p ) , FALSE , Chamber , buffer , 255 ) ) continue;
					//
					UTIL_CHANGE_FILE_VALID_STYLE( buffer );
					//
					if ( j ) strcat( data , "|" );
					//
					strcat( data , buffer );
					//
					j = TRUE;
				}
			}
		}
		else { // 2012.04.06
			//
			s = _i_SCH_MODULE_Get_PM_SIDE( Chamber , 1 );
			p = _i_SCH_MODULE_Get_PM_POINTER( Chamber , 1 );
			//
			if ( side == s ) {
				j = SCHMULTI_MESSAGE_GET_PPID( side , pointer , 1 , 0 , Slot , TRUE , Chamber , buffer , 255 ); // 2012.02.18
			}
			else {
				j = SCHMULTI_MESSAGE_GET_PPID( side , pointer , 1 , 0 , Slot % 100 , TRUE , Chamber , buffer , 255 ); // 2012.02.18
			}
			//
			UTIL_CHANGE_FILE_VALID_STYLE( buffer );
			//
			strcat( data , buffer );
			//
			if ( side == s ) {
				i = SCHMULTI_MESSAGE_GET_PPID( s , p , 1 , 1 , Slot , TRUE , Chamber , buffer , 255 );
			}
			else {
				i = SCHMULTI_MESSAGE_GET_PPID( s , p , 1 , 1 , Slot / 100 , TRUE , Chamber , buffer , 255 );
			}
			//
			if ( i ) {
				//
				UTIL_CHANGE_FILE_VALID_STYLE( buffer );
				//
				if ( j ) strcat( data , "|" );
				//
				strcat( data , buffer );
				//
			}
			//
		}
		//=================================================================================
		strcat( data , "/" );
		//=================================================================================
		break;

	default :

		sprintf( data , "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/" ,
			_i_SCH_CLUSTER_Get_PathIn( side , pointer ) ,
			_i_SCH_CLUSTER_Get_PathOut( side , pointer ) ,
			_i_SCH_CLUSTER_Get_SlotIn( side , pointer ) ,
			_i_SCH_CLUSTER_Get_SlotOut( side , pointer ) ,
			_i_SCH_CLUSTER_Get_PathRange( side , pointer ) ,
			_i_SCH_CLUSTER_Get_PathDo( side , pointer ) ,
//			_i_SCH_CLUSTER_Get_PathHSide( side , pointer ) , // 2018.11.22
			_i_SCH_CLUSTER_Get_PathHSide( side , pointer ) % 1000000 , // 2018.11.22
			_i_SCH_CLUSTER_Get_SwitchInOut( side , pointer ) ,
			_i_SCH_CLUSTER_Get_FailOut( side , pointer ) ,
			0 ,
			_i_SCH_MODULE_GET_USE_ENABLE( Chamber , FALSE , side ) , // 2010.09.21
			_i_SCH_CLUSTER_Get_Buffer( side , pointer ) ,
			_i_SCH_CLUSTER_Get_Stock( side , pointer ) ,
			_i_SCH_CLUSTER_Get_Extra( side , pointer ) ,
			_i_SCH_CLUSTER_Get_Optimize( side , pointer ) ,
			_i_SCH_CLUSTER_Get_LotSpecial( side , pointer )
			);
		//=================================================================================
		// 2007.08.14
		//=================================================================================
		SCHMULTI_MESSAGE_GET_PPID( side , pointer , 0 , -1 , 0 , FALSE , Chamber , buffer , 255 );
		UTIL_CHANGE_FILE_VALID_STYLE( buffer );
		//
		strcat( data , buffer );
		//
		//=================================================================================
		strcat( data , "/" );
		//=================================================================================
		break;
	}
}

//BOOL EQUIP_SPAWN_PROCESS_SUB_MSG( int CHECKING_SIDE , int pointer , int Chamber , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int Mindex , char *Buffer ) { // 2014.11.09
//BOOL EQUIP_SPAWN_PROCESS_SUB_MSG( int CHECKING_SIDE , int pointer , int Chamber , int ChamberP , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int Mindex , char *Buffer ) { // 2014.11.09 // 2015.04.23
//BOOL EQUIP_SPAWN_PROCESS_SUB_MSG( int CHECKING_SIDE , int pointer , int Chamber , int ChamberP , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int Mindex , char *Buffer , int *rid ) { // 2014.11.09 // 2015.04.23 // 2016.04.26
BOOL EQUIP_SPAWN_PROCESS_SUB_MSG( int CHECKING_SIDE , int pointer , int Chamber , int ChamberP , int ChamberS , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int Mindex , char *Buffer , int *rid ) { // 2014.11.09 // 2015.04.23 // 2016.04.26
	//
	//	LOCKSC-[0-C][10-C][1-C][11-C][2-C][12-C][3-C][13-C]
	//
	char Imsi[256];
	char RunName[256];
	char DirName[256];
	char Header[256];
//	char Header2[256]; // 2010.08.30
	char Header2[1024]; // 2010.08.30
	//
	int  AllRecipe_SHPT; // 2015.07.20
	//
	int locking , LckRecipe_SHPT; // 2016.11.03
	//
	//
	// 2016.11.03
	//
	if ( ( AllRecipe[0] == '-' ) && ( ( AllRecipe[1] == '\\' ) || ( AllRecipe[1] == '/' ) ) ) {
		LckRecipe_SHPT = 2;
		locking = 0;
	}
	else {
		LckRecipe_SHPT = 0;
		locking = SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE );
	}
	//
	//======================================================================================================================================================
	AllRecipe_SHPT = 0; // 2015.07.20
	if ( ( AllRecipe[0] == '+' ) && ( ( AllRecipe[1] == '\\' ) || ( AllRecipe[1] == '/' ) ) )  AllRecipe_SHPT = 2; // 2015.07.20
	//======================================================================================================================================================
	if ( ( mode == 13 ) || ( mode == 14 ) ) {
//		UTIL_EXTRACT_GROUP_FILE( "" , AllRecipe + AllRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
		UTIL_EXTRACT_GROUP_FILE( "" , AllRecipe + AllRecipe_SHPT + LckRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
	}
	else {
//		UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , AllRecipe + AllRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
		UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , AllRecipe + AllRecipe_SHPT + LckRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
	}
	//======================================================================================================================================================
//	if ( !EQUIP_SPAWN_PROCESS_GET_HEADER( Chamber , RunName , mode , Header ) ) return FALSE; // 2015.04.23
//	if ( !EQUIP_SPAWN_PROCESS_GET_HEADER( Chamber , RunName , mode , Header , rid ) ) return FALSE; // 2015.04.23 // 2016.04.26
	if ( !EQUIP_SPAWN_PROCESS_GET_HEADER( CHECKING_SIDE , ChamberP , RunName , mode , Header , rid ) ) return FALSE; // 2015.04.23 // 2016.04.26
//	EQUIP_SPAWN_PROCESS_GET_INFO( CHECKING_SIDE , pointer , Slot , Chamber , Header2 ); // 2006.04.06 // 2016.04.26
	EQUIP_SPAWN_PROCESS_GET_INFO( CHECKING_SIDE , pointer , Slot , ChamberP , ChamberS , Header2 ); // 2006.04.06 // 2016.04.26
	//======================================================================================================================================================
	if ( ( strlen( RunName ) <= 0 ) || ( strcmp( RunName , "?" ) == 0 ) ) {
		sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d ?" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex );
	}
	else {
//		if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) != 2 ) { // 2014.11.09
		if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( ChamberP ) != 2 ) { // 2014.11.09
//			if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() ) { // 2009.10.30
//			if ( ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) == 0 ) ) { // 2009.10.30 // 2014.02.08
//			if ( ( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) == 0 ) ) { // 2009.10.30 // 2014.02.08 // 2014.11.09
//			if ( ( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( ChamberP ) == 0 ) ) { // 2009.10.30 // 2014.02.08 // 2014.11.09 // 2016.11.03
			if ( ( locking != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( ChamberP ) == 0 ) ) { // 2009.10.30 // 2014.02.08 // 2014.11.09 // 2016.11.03
//				if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 2 ) { // 2012.04.12 // 2015.07.20
				if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() >= 2 ) { // 2012.04.12 // 2015.07.20
					if ( ( Slot % 100 ) > 0 ) { // 2012.06.12
						/*
						// 2012.07.26
						if      ( mode == 0 ) { // Main
							sprintf( Imsi , "%d$0$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 1 ) { // Post
							sprintf( Imsi , "%d$1$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 2 ) { // Pre
							sprintf( Imsi , "%d$2$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else {
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						*/
						//
						if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) {
//							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
						else {
//							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
					}
					else if ( ( Slot / 100 ) > 0 ) { // 2012.06.12
						/*
						// 2012.07.26
						if      ( mode == 0 ) { // Main
							sprintf( Imsi , "%d$0$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 1 ) { // Post
							sprintf( Imsi , "%d$1$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 2 ) { // Pre
							sprintf( Imsi , "%d$2$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else {
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						*/
						if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) {
//							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
						else {
//							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
					}
					else { // 2012.06.12
//						sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
						sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
					}
				}
				else {
//					sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
					sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
				}
				Change_Dirsep_to_Dollar( Imsi );
				//
				if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() >= 3 ) { // 2015.07.20
//					sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d +/%s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Imsi ); // 2017.10.30
					sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d +/%s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_TMP_PATH() , Imsi ); // 2017.10.30
				}
				else {
//					sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d %s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Imsi ); // 2017.10.30
					sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d %s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_TMP_PATH() , Imsi ); // 2017.10.30
				}
				//
			}
			else {
//				sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d %s/%s/%s/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName ); // 2014.11.09
				if ( AllRecipe_SHPT > 0 ) { // 2015.07.20
					sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d +/%s/%s/%s/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ChamberP ) , RunName ); // 2014.11.09
				}
				else {
					sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d %s/%s/%s/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ChamberP ) , RunName ); // 2014.11.09
				}
			}
		}
		else { // 2007.03.07
			sprintf( Buffer , "%s %d:%d:%d:%d:%s:%d%s:%d ?%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , RunName );
		}
	}
	return TRUE;
}
//
//BOOL _i_SCH_EQ_SPAWN_EVENT_PROCESS( int CHECKING_SIDE , int Pointer , int Chamber , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int Mindex ) { // 2004.10.08 // 2014.11.09
BOOL _i_SCH_EQ_SPAWN_EVENT_PROCESS( int CHECKING_SIDE , int Pointer , int Chamber0 , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int Mindex ) { // 2004.10.08 // 2014.11.09
	//
	//	LOCKSC-[0-C][10-C][1-C][11-C][2-C][12-C][3-C][13-C]
	//
	int res;
//	char Buffer[256]; // 2010.09.02
	char Buffer[1024]; // 2010.09.02
	char RunName[256];
	char DirName[256];
	char Imsi[256];
	char Header[256];
//	char Header2[256]; // 2010.08.30
//	char Header2[10244]; // 2010.08.30 // 2011.03.02
	char Header2[1024]; // 2010.08.30 // 2011.03.02
	int l_c;
	int Chamber , ChamberP; // 2014.11.09
	//
	int rid; // 2015.04.23
	//
	int  AllRecipe_SHPT; // 2015.07.20
	//
	int locking , LckRecipe_SHPT; // 2016.11.03
	//
	rid = 0; // 2015.04.23
	//
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( Chamber0 >= 1000 ) { // 2014.11.09
		Chamber  = Chamber0 % 1000;
		ChamberP = Chamber0 / 1000;
	}
	else {
		Chamber  = Chamber0;
		ChamberP = Chamber0;
	}
	//
	//========================
	//
	if ( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] < 0 ) return TRUE;
	//
	//========================
	//
	//======================================================================================================================================================
	// 2008.09.27
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d,Mindex=%d\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_EVENT_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime , Mindex );
	}
	//
	//
	// 2016.11.03
	//
	if ( ( AllRecipe[0] == '-' ) && ( ( AllRecipe[1] == '\\' ) || ( AllRecipe[1] == '/' ) ) ) {
		LckRecipe_SHPT = 2;
		locking = 0;
	}
	else {
		LckRecipe_SHPT = 0;
		locking = SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE );
	}
	//
	//======================================================================================================================================================
	AllRecipe_SHPT = 0; // 2015.07.20
	if ( ( AllRecipe[0] == '+' ) && ( ( AllRecipe[1] == '\\' ) || ( AllRecipe[1] == '/' ) ) )  AllRecipe_SHPT = 2; // 2015.07.20
	//======================================================================================================================================================
	if ( ( mode == 13 ) || ( mode == 14 ) ) {
//		UTIL_EXTRACT_GROUP_FILE( "" , AllRecipe + AllRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
		UTIL_EXTRACT_GROUP_FILE( "" , AllRecipe + AllRecipe_SHPT + LckRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
	}
	else {
//		UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , AllRecipe + AllRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
		UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH( CHECKING_SIDE ) , AllRecipe + AllRecipe_SHPT + LckRecipe_SHPT , DirName , 255 , RunName , 255 ); // 2016.11.03
	}
	//======================================================================================================================================================
//	if ( !EQUIP_SPAWN_PROCESS_GET_HEADER( Chamber , RunName , mode , Header ) ) { // 2015.04.23
	if ( !EQUIP_SPAWN_PROCESS_GET_HEADER( CHECKING_SIDE , Chamber , RunName , mode , Header , &rid ) ) { // 2015.04.23
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d,Mindex=%d [1]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_EVENT_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime , Mindex );
		}
		//======================================================================================================================================================
		return FALSE; // 2005.10.16
	}
	EQUIP_SPAWN_PROCESS_GET_INFO( CHECKING_SIDE , Pointer , Slot , Chamber , 0 , Header2 ); // 2006.04.06
	//======================================================================================================================================================
	if ( ( strlen( RunName ) <= 0 ) || ( strcmp( RunName , "?" ) == 0 ) ) {
		sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d ?" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex );
	}
	else {
		if ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) != 2 ) {
//			if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() ) { // 2009.10.30
//			if ( ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) == 0 ) ) { // 2009.10.30 // 2014.02.08
//			if ( ( SCHEDULER_GET_RECIPE_LOCKING( CHECKING_SIDE ) != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) == 0 ) ) { // 2009.10.30 // 2014.02.08 // 2016.11.03
			if ( ( locking != 0 ) && ( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( Chamber ) == 0 ) ) { // 2009.10.30 // 2014.02.08 // 2016.11.03
//				if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() == 2 ) { // 2012.04.12 // 2015.07.20
				if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() >= 2 ) { // 2012.04.12 // 2015.07.20
					if ( ( Slot % 100 ) > 0 ) { // 2012.06.12
						/*
						// 2012.07.26
						if      ( mode == 0 ) { // Main
							sprintf( Imsi , "%d$0$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 1 ) { // Post
							sprintf( Imsi , "%d$1$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 2 ) { // Pre
							sprintf( Imsi , "%d$2$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else {
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						*/
						if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) {
//							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot % 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
						else {
//							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
					}
					else if ( ( Slot / 100 ) > 0 ) { // 2012.06.12
						/*
						// 2012.07.26
						if      ( mode == 0 ) { // Main
							sprintf( Imsi , "%d$0$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 1 ) { // Post
							sprintf( Imsi , "%d$1$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else if ( mode == 2 ) { // Pre
							sprintf( Imsi , "%d$2$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						else {
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName );
						}
						*/
						if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) {
//							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%d$%s$%s$%s" , CHECKING_SIDE , ( Slot / 100 ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
						else {
//							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
							sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
						}
					}
					else { // 2012.06.12
//						sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
						sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
					}
				}
				else {
//					sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( Chamber ) , RunName ); // 2014.11.09
					sprintf( Imsi , "%d$%s$%s$%s" , CHECKING_SIDE , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( ChamberP ) , RunName ); // 2014.11.09
				}
				Change_Dirsep_to_Dollar( Imsi );
				//
				if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() >= 3 ) { // 2015.07.20
//					sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d +/%s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Imsi ); // 2017.10.30
					sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d +/%s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_TMP_PATH() , Imsi ); // 2017.10.30
				}
				else {
//					sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d %s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_LOG_PATH() , Imsi ); // 2017.10.30
					sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d %s/tmp/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_TMP_PATH() , Imsi ); // 2017.10.30
				}
				//
			}
			else {
//				sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d %s/%s/%s/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName ); // 2014.11.09
				//
				if ( AllRecipe_SHPT > 0 ) { // 2015.07.20
					sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d +/%s/%s/%s/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ChamberP ) , RunName ); // 2014.11.09
				}
				else {
					sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d %s/%s/%s/%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( ChamberP ) , RunName ); // 2014.11.09
				}
				//
			}
		}
		else { // 2007.03.07
			sprintf( Buffer , "APPEND %s %d:%d:%d:%d:%s:%d%s:%d ?%s" , Header , Slot , StartChamber , Finger , PutTime , NextPM , MinTime , Header2 , Mindex , RunName );
		}
	}
	//======================================================================================================================================================
	_dWRITE_FUNCTION_EVENT( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] , Buffer );
	//======================================================================================================================================================
	l_c = 0; // 2008.09.14
	while( TRUE ) {
		res = _dREAD_FUNCTION_EVENT( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] );
		if ( res != SYS_RUNNING ) break;
		// Sleep(1); // 2008.09.14
		if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
		l_c++; // 2008.09.14
	}
	//======================================================================================================================================================
	// 2008.09.27
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d,Mindex=%d,res=%d\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_EVENT_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime , Mindex , res );
	}
	//======================================================================================================================================================
	if ( res == SYS_SUCCESS ) return TRUE;
	return FALSE;
}
//
void EQUIP_RUN_PROCESS_ABORT( int Side , int Chamber ) {
	if ( Address_FUNCTION_PROCESS[Side][Chamber] < 0 ) return;
	_dRUN_FUNCTION_ABORT( Address_FUNCTION_PROCESS[Side][Chamber] );
}
//
int EQUIP_PROCESS_ITEM_CHANGE_EVENT( int Side , int Chamber , int Slot , int Step , char *Item , char *Data , char *Else ) {
	char Buffer[256];
	int i;
	int l_c; // 2008.09.14
	if ( Address_FUNCTION_PROCESS[Side][Chamber] < 0 ) return SYS_ABORTED;
	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) && ( GET_RUN_LD_CONTROL(8) <= 0 ) ) {
		sprintf( Buffer , "ITEM_CHANGE %d|%d|%s|%s|%s" , Slot , Step , Item , Data , Else );
		_dWRITE_FUNCTION_EVENT( Address_FUNCTION_PROCESS[Side][Chamber] , Buffer );
		l_c = 0; // 2008.09.14
		while( TRUE ) {
			i = _dREAD_FUNCTION_EVENT( Address_FUNCTION_PROCESS[Side][Chamber] );
			if ( i != SYS_RUNNING ) return i;
			// Sleep(1); // 2008.09.14
			if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
			l_c++; // 2008.09.14
		}
	}
	return SYS_SUCCESS;
}
//===================================================================
//BOOL _i_SCH_EQ_SPAWN_PROCESS( int CHECKING_SIDE , int Pointer , int Chamber , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int cutxindex ) { // 2005.10.16 // 2014.11.07
//BOOL _i_SCH_EQ_SPAWN_PROCESS( int CHECKING_SIDE , int Pointer , int Chamber0 , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int cutxindex ) { // 2005.10.16 // 2014.11.07 // 2015.04.23
BOOL _i_SCH_EQ_SPAWN_PROCESS_SUB( int CHECKING_SIDE , int Pointer , int Chamber0 , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int cutxindex , int *rid ) { // 2005.10.16 // 2014.11.07 // 2015.04.23
	int z1 , z2 , x , d , rmode , nomore , xoffset , res;
	char Retdata[256];
//	char GetStr[256]; // 2007.06.19
//	char GetStr[512]; // 2007.06.19 // 2010.09.02
	char GetStr[1024]; // 2007.06.19 // 2010.09.02
//	char TotStr[2048]; // 2007.06.15

	char TotStr[MAX_PRCS_SPAWN_STRING_SIZE]; // 2007.06.15
	char *TotStr2; // 2007.06.19
	char *TotStr2_Temp; // 2007.06.19
	int  TotStrMode; // 2007.06.19
	int  TotStrCnt; // 2007.06.19
	int  TotStrMC; // 2007.06.19
	int  TotStrLen; // 2007.06.19
//	int  Chamber , ChamberP; // 2014.11.08 // 2016.04.26
	int  Chamber , ChamberP , ChamberS; // 2014.11.08 // 2016.04.26
	//
//	char TunInfo[1024]; // 2007.06.08
	//
	//========================
	//
	*rid = 0; // 2015.04.23
	//
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( Chamber0 >= 1000000 ) { // 2016.04.26
		ChamberS = Chamber0 / 1000000;
		ChamberP = ( Chamber0 % 1000000 ) / 1000;
		Chamber =    Chamber0 % 1000;
		if ( ChamberP == 0 ) ChamberP = Chamber;
	}
	else {
		//
		ChamberS = 0; // 2016.04.26
		//
		if ( Chamber0 >= 1000 ) { // 2014.11.09
			Chamber  = Chamber0 % 1000;
			ChamberP = Chamber0 / 1000;
		}
		else {
			Chamber  = Chamber0;
			ChamberP = Chamber0;
		}
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
	}
	//===========================================================================================================================
	// 2005.01.06
	//===========================================================================================================================
//	if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) { // 2008.02.28
//		SCHEDULER_CONTROL_Set_LotSpecial_Item_PM_LAST_PROCESS_DATA_for_STYLE_4( CHECKING_SIDE , Pointer , Chamber ); // 2008.02.28
//	} // 2008.02.28
	//===========================================================================================================================
//	PROCESS_MONITOR_TUNE_DATA_SET_FOR_LOG( Chamber , FALSE , "" ); // 2007.06.08
	SCHMULTI_TUNE_INFO_DATA_RESET( Chamber );
	//
	if ( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] < 0 ) {
		//======================================================================================================================================================
		// 2008.09.26
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d [1]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
		}
		//======================================================================================================================================================
		//
		*rid = *rid + 1000; // 2015.04.23
		//
		return TRUE;
	}
	//
	//======================================================================================================================================================
//	if ( mode == 0 ) { // Main Process // 2007.06.04
	if ( ( mode == 0 ) || ( mode == 1 ) || ( mode == 2 ) ) { // 2007.06.04
//		if ( SCHMULTI_RUNJOB_GET_TUNING_INFO( mode , CHECKING_SIDE , Pointer , Chamber - PM1 + 1 , TunInfo ) ) { // 2007.06.08

		if ( SCHMULTI_RUNJOB_GET_TUNING_INFO( mode , CHECKING_SIDE , Pointer , Chamber , Slot , AllRecipe ) ) { // 2007.06.08
// 			PROCESS_MONITOR_TUNE_DATA_SET_FOR_LOG( Chamber , TRUE , TunInfo + 1 ); // 2007.06.08
			_dWRITE_FUNCTION_EVENT( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] , SCHMULTI_TUNE_INFO_DATA_GET(Chamber) );
			//
			while( TRUE ) { // 2010.01.27
				res = _dREAD_FUNCTION_EVENT( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] );
				if ( res != SYS_RUNNING ) break;
				Sleep(1);
			}
			//
		}
	}
	//======================================================================================================================================================
	// 2006.03.23
	//======================================================================================================================================================
	TotStrMode = 0; // 2007.06.19
	strcpy( TotStr , "" );
	TotStr2 = NULL; // 2007.06.19
	TotStrCnt = 0; // 2007.06.19
	TotStrMC = MAX_PRCS_SPAWN_STRING_SIZE; // 2007.06.19
	//
	d = 0;
	x = 0;
	xoffset = 0; // 2007.06.11
	z1 = 0;
	z2 = STR_SEPERATE_CHAR_WITH_POINTER( AllRecipe , '|' , Retdata , z1 , 255 );
	if ( z1 == z2 ) {
		//======================================================================================================================================================
		// 2008.09.26
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d [2]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
		}
		//======================================================================================================================================================
		//
		*rid = *rid + 2000; // 2015.04.23
		//
		return FALSE;
	}
	//=======================================================================================================================
	do {
		//=======================================================================================================================
		nomore = FALSE; // 2007.06.11
		//=======================================================================================================================
		// 2007.02.14
		//=======================================================================================================================
		rmode = mode;
		//=======================================================================================================================
		_SCH_COMMON_GET_PROCESS_INDEX_DATA( mode , x , &rmode , &nomore , &xoffset );
		//=======================================================================================================================
		if ( ( cutxindex >= 0 ) && ( cutxindex <= x ) ) nomore = TRUE; // 2008.11.20
		//=======================================================================================================================
//		if ( strlen( Retdata ) > 0 ) { // 2008.05.20
		if ( ( strlen( Retdata ) > 0 ) && ( Retdata[0] != '*' ) ) { // 2008.05.20
//			if ( !EQUIP_SPAWN_PROCESS_SUB_MSG( CHECKING_SIDE , Pointer , Chamber , StartChamber , Slot , Finger , Retdata , mode , PutTime , NextPM , MinTime , x , GetStr ) ) { // 2006.07.10
//			if ( !EQUIP_SPAWN_PROCESS_SUB_MSG( CHECKING_SIDE , Pointer , Chamber , StartChamber , Slot , Finger , Retdata , rmode , PutTime , NextPM , MinTime , x , GetStr ) ) { // 2006.07.10 // 2007.06.11
//			if ( !EQUIP_SPAWN_PROCESS_SUB_MSG( CHECKING_SIDE , Pointer , Chamber , StartChamber , Slot , Finger , Retdata , rmode , PutTime , NextPM , MinTime , x + xoffset , GetStr ) ) { // 2006.07.10 // 2007.06.11 // 2014.11.09
//			if ( !EQUIP_SPAWN_PROCESS_SUB_MSG( CHECKING_SIDE , Pointer , Chamber , ChamberP , StartChamber , Slot , Finger , Retdata , rmode , PutTime , NextPM , MinTime , x + xoffset , GetStr ) ) { // 2006.07.10 // 2007.06.11 // 2014.11.09 // 2015.04.23
//			if ( !EQUIP_SPAWN_PROCESS_SUB_MSG( CHECKING_SIDE , Pointer , Chamber , ChamberP , StartChamber , Slot , Finger , Retdata , rmode , PutTime , NextPM , MinTime , x + xoffset , GetStr , rid ) ) { // 2006.07.10 // 2007.06.11 // 2014.11.09 // 2015.04.23 // 2016.04.26
			if ( !EQUIP_SPAWN_PROCESS_SUB_MSG( CHECKING_SIDE , Pointer , Chamber , ChamberP , ChamberS , StartChamber , Slot , Finger , Retdata , rmode , PutTime , NextPM , MinTime , x + xoffset , GetStr , rid ) ) { // 2006.07.10 // 2007.06.11 // 2014.11.09 // 2015.04.23 // 2016.04.26
				if ( TotStrMode != 0 ) free( TotStr2 ); // 2007.06.19
				//======================================================================================================================================================
				// 2008.09.26
				//======================================================================================================================================================
				if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
					_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d [3]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
				}
				//======================================================================================================================================================
				//
				*rid = *rid + 3000; // 2015.04.23
				//
				return FALSE;
			}
			//============================================================================================
			TotStrLen = strlen( GetStr ); // 2007.06.19
			//============================================================================================
			if ( d == 0 ) {
				d = 1;
				TotStrMode = 0; // 2007.06.19
				TotStrCnt = TotStrLen; // 2007.06.19
				strcat( TotStr , GetStr ); // 2007.06.19
			}
			else {
				if ( TotStrMode == 0 ) {
					if ( ( TotStrCnt + TotStrLen + 1 ) >= TotStrMC ) {
						TotStrMC = TotStrMC + MAX_PRCS_SPAWN_STRING_SIZE; // 2007.06.19
						//
						TotStr2 = calloc( TotStrMC + 1 , sizeof( char ) );
						if ( TotStr2 == NULL ) {
							//
							_IO_CIM_PRINTF( "EQUIP_SPAWN_PROCESS Memory Allocate Error 1\n" );
							//
							*rid = ( *rid % 100 ) + 100; // 2015.04.23
							//
							break;
						}
						//
						TotStrMode = 1; // 2007.06.19
						//
						strcpy( TotStr2 , TotStr );
					}
				}
				else {
					if ( ( TotStrCnt + TotStrLen + 1 ) >= TotStrMC ) {
						TotStr2_Temp = calloc( TotStrMC + 1 , sizeof( char ) );
						if ( TotStr2_Temp == NULL ) {
							_IO_CIM_PRINTF( "EQUIP_SPAWN_PROCESS Memory Allocate Error 2\n" );
							//
							*rid = ( *rid % 100 ) + 200; // 2015.04.23
							//
							break;
						}
						strcpy( TotStr2_Temp , TotStr2 );
						//
						TotStrMC = TotStrMC + MAX_PRCS_SPAWN_STRING_SIZE; // 2007.06.19
						//
						TotStr2 = calloc( TotStrMC + 1 , sizeof( char ) );
						if ( TotStr2 == NULL ) {
							free( TotStr2_Temp );
							_IO_CIM_PRINTF( "EQUIP_SPAWN_PROCESS Memory Allocate Error 3\n" );
							//
							*rid = ( *rid % 100 ) + 300; // 2015.04.23
							//
							break;
						}
						//
						strcpy( TotStr2 , TotStr2_Temp );
						//
						free( TotStr2_Temp );
					}
				}
				//===============================================================================
				TotStrCnt = TotStrCnt + TotStrLen + 1; // 2007.06.19
				//===============================================================================
				if ( TotStrMode == 0 ) {
					strcat( TotStr , ">" );
					strcat( TotStr , GetStr ); // 2007.06.19
				}
				else {
					strcat( TotStr2 , ">" );
					strcat( TotStr2 , GetStr ); // 2007.06.19
				}
				//===============================================================================
			}
//			strcat( TotStr , GetStr ); // 2007.06.19
		}
		//=======================================================================================================================
		if ( nomore ) break; // 2007.06.11
		//=======================================================================================================================
		x++;
		z1 = z2;
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( AllRecipe , '|' , Retdata , z1 , 255 );
		if ( z1 == z2 ) break;
	}
	while( TRUE );
	//
	if ( d == 0 ) {
		//======================================================================================================================================================
		// 2008.09.26
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d [4]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
		}
		//======================================================================================================================================================
		//
		*rid = *rid + 4000; // 2015.04.23
		//
		return FALSE;
	}
	//======================================================================================================================================================
	// 2006.03.23
	//======================================================================================================================================================
	if ( TotStrMode == 0 ) {
//		_dRUN_SET_FUNCTION( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] , TotStr ); // 2008.04.03
		if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] , TotStr ) ) {
			//======================================================================================================================================================
			// 2008.09.26
			//======================================================================================================================================================
			if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
				_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d [5]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
			}
			//======================================================================================================================================================
			//
			*rid = *rid + 5000; // 2015.04.23
			//
			return FALSE; // 2008.04.03
		}
	}
	else {
//		_dRUN_SET_FUNCTION( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] , TotStr2 ); // 2008.04.03
		if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_PROCESS[CHECKING_SIDE][Chamber] , TotStr2 ) ) { // 2008.04.03
			free( TotStr2 );
			//======================================================================================================================================================
			// 2008.09.26
			//======================================================================================================================================================
			if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
				_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d [6]\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
			}
			//======================================================================================================================================================
			//
			*rid = *rid + 6000; // 2015.04.23
			//
			return FALSE;
		}
		free( TotStr2 );
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,Pointer=%d,mode=%d,StartChamber=%d,Slot=%d,Finger=%d,PutTime=%d,AllRecipe=%s,NextPM=%s,MinTime=%d\n" , CHECKING_SIDE + 1 , "EQUIP_SPAWN_PROCESS" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , Pointer , mode , StartChamber , Slot , Finger , PutTime , AllRecipe , NextPM , MinTime );
	}
	//======================================================================================================================================================
	return TRUE;
}




BOOL _i_SCH_EQ_SPAWN_PROCESS( int CHECKING_SIDE , int Pointer , int Chamber0 , int StartChamber , int Slot , int Finger , char *AllRecipe , int mode , int PutTime , char *NextPM , int MinTime , int cutxindex ) { // 2015.04.23
	int rid;
	return _i_SCH_EQ_SPAWN_PROCESS_SUB( CHECKING_SIDE , Pointer , Chamber0 , StartChamber , Slot , Finger , AllRecipe , mode , PutTime , NextPM , MinTime , cutxindex , &rid );
}
