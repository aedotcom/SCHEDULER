#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "sch_prepost.h"
#include "Equip_Handling.h"
#include "Utility.h"
#include "sch_estimate.h"

#include "sch_prm_Cluster.h"
#include "sch_prm_FBTPM.h"
#include "sch_sub.h"
#include "IO_Part_Data.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip BM Processing Part
//------------------------------------------------------------------------------------------
BOOL EQUIP_GET_DUMMY_WAFER_IN_BM( int ch ) {
	int i;
	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _i_SCH_IO_GET_MODULE_STATUS( ch , i ) <= 0 ) continue;
		if ( _i_SCH_IO_GET_MODULE_SOURCE( ch , i ) < MAX_CASSETTE_SIDE ) continue;
		return TRUE;
	}
	return FALSE;
}

void EQUIP_GET_SPAWN_MESSAGE_BM( int Side , int Chamber , int mode , int slot , int bmwafercount , char *Buffer , int size , BOOL MaintInfUse ) {
//	int i; // 2007.11.27
	BOOL dummy = FALSE;
	char RunName[256];
	char DirName[256];
	char MsgAppchar[2]; /* 2013.04.26 */

	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */

	if      ( mode == BUFFER_RUN_WAITING_FOR_TM ) {
		if      ( _i_SCH_PRM_GET_READY_RECIPE_USE( Chamber ) == 0 ) {
			_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE %d" , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) );
			//EQUIP_INTERFACE_STRING_APPEND_TRANSFER( Side , Buffer ); // 2007.07.20
		}
		else if ( _i_SCH_PRM_GET_READY_RECIPE_USE( Chamber ) == 2 ) {
			if ( Side != TR_CHECKING_SIDE ) dummy = EQUIP_GET_DUMMY_WAFER_IN_BM( Chamber ); // 2009.02.02
			//
			UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(Side) , "????" , DirName , 255 , RunName , 255 );
			if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 0 ) { // 2017.02.02
				if ( dummy ) {
//					_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s %d 2 %d D%d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2017.10.30
					_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s %d 2 %d D%d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_TMP_PATH() ); // 2017.10.30
				}
				else {
//					_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s %d 2 %d %d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2017.10.30
					_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s %d 2 %d %d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_TMP_PATH() ); // 2017.10.30
				}
			}
			else {
				if ( dummy ) {
					_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s %d 2 %d D%d %s/%s/%s" , MsgAppchar , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) ); // 2006.12.12
				}
				else {
					_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s %d 2 %d %d %s/%s/%s" , MsgAppchar , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) ); // 2006.12.12
				}
			}
			//EQUIP_INTERFACE_STRING_APPEND_TRANSFER( Side , Buffer ); // 2007.07.20
		}
		else {
			if ( Side != TR_CHECKING_SIDE ) {
				UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(Side) , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( Side , Chamber ) , DirName , 255 , RunName , 255 ); // 2002.05.10 // 2007.07.20
				_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s PROCESS_OUT/B %d:%d:%d:%d %s/%s/%s/%s" , MsgAppchar , bmwafercount , 0 , 0 , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName ); // 2002.05.10 // 2007.11.27
			}
			else {
				_snprintf( Buffer , size , "WAITING_FOR_TM_SIDE%s PROCESS_OUT/B %d:%d:%d:%d" , MsgAppchar , -1 , 0 , 0 , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) );
			}
		}
	}
	else if ( mode == BUFFER_RUN_WAITING_FOR_FM ) {
		if      ( _i_SCH_PRM_GET_END_RECIPE_USE( Chamber ) == 0 ) {
			_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d" , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) );
			//EQUIP_INTERFACE_STRING_APPEND_TRANSFER( Side , Buffer ); // 2007.07.20
		}
		else if ( _i_SCH_PRM_GET_END_RECIPE_USE( Chamber ) == 2 ) { // 2006.07.03
			if ( Side != TR_CHECKING_SIDE ) dummy = EQUIP_GET_DUMMY_WAFER_IN_BM( Chamber ); // 2009.02.02
			//
			UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(Side) , "????" , DirName , 255 , RunName , 255 );
			if ( _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() != 0 ) { // 2017.02.02
				if ( dummy ) {
//					_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d 2 %d D%d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2017.10.30
					_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d 2 %d D%d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_TMP_PATH() ); // 2017.10.30
				}
				else {
//					_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d 2 %d %d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_LOG_PATH() ); // 2017.10.30
					_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d 2 %d %d %s/tmp/." , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_TMP_PATH() ); // 2017.10.30
				}
			}
			else {
				if ( dummy ) {
					_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d 2 %d D%d %s/%s/%s" , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) ); // 2006.12.12
				}
				else {
					_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s %d 2 %d %d %s/%s/%s" , MsgAppchar , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , Side + 1 , slot , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) ); // 2006.12.12
				}
			}
			//EQUIP_INTERFACE_STRING_APPEND_TRANSFER( Side , Buffer ); // 2007.07.20
		}
		else {
			if ( Side != TR_CHECKING_SIDE ) {
//				UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(Side) , _i_SCH_PRM_GET_END_RECIPE_NAME( Chamber ) , DirName , 255 , RunName , 255 ); // 2002.05.10 // 2007.07.20
				UTIL_EXTRACT_GROUP_FILE( _i_SCH_PRM_GET_DFIX_GROUP_RECIPE_PATH(Side) , _i_SCH_PRM_GET_inside_END_RECIPE_NAME( Side , Chamber ) , DirName , 255 , RunName , 255 ); // 2002.05.10 // 2007.07.20
				_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s PROCESS_OUT/E %d:%d:%d:%d %s/%s/%s/%s" , MsgAppchar , bmwafercount , 0 , 0 , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHM( Chamber ) , DirName , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Chamber ) , RunName ); // 2002.05.10 // 2007.11.27
			}
			else {
				_snprintf( Buffer , size , "WAITING_FOR_FM_SIDE%s PROCESS_OUT/E %d:%d:%d:%d" , MsgAppchar , -1 , 0 , 0 , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) );
			}
		}
	}
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER( Side , Buffer ); // 2007.07.20
}

//------------------------------------------------------------------------------------------
//#include "iolog.h"
void EQUIP_SPAWN_WAITING_BM( int Side , int Chamber , int mode , int slot , int bmfirstslot ) {
	//---------------------------------------------
	// 2002.03.27 // Modify for BM Processing
	//---------------------------------------------
	char Buffer[265+1];
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] Chamber=%d,mode=%d,slot=%d\n" , Side + 1 , "EQUIP_SPAWN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
	}
	//======================================================================================================================================================

//	sprintf( Buffer , "[BM%d] [SIDE=%d] [mode=%d] [Slot=%d] [bmf=%d]\n" , Chamber - BM1 + 1 , Side , mode , slot , bmfirstslot );
//	_IO_MSGEVENT_PRINTF( "BM_EQ_SPAWN" , Buffer );


//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) ) { // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( GET_RUN_LD_CONTROL(7) > 0 ) ) { // 2014.01.09
		if      ( mode == BUFFER_RUN_WAITING_FOR_TM ) {
			if ( _SCH_Get_SIM_TIME1( Side , Chamber ) > 0 ) {
				KPLT_RESET_BM_RUN_MS_TIMER( Chamber , _SCH_Get_SIM_TIME1( Side , Chamber ) ); // 2016.12.09
			}
			else {
//				KPLT_RESET_BM_RUN_TIMER( Chamber , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) ); // 2016.12.09
				KPLT_RESET_BM_RUN_MS_TIMER( Chamber , _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) * 1000 ); // 2016.12.09
			}
		}
		else if ( mode == BUFFER_RUN_WAITING_FOR_FM ) {
			if ( _SCH_Get_SIM_TIME2( Side , Chamber ) > 0 ) {
				KPLT_RESET_BM_RUN_MS_TIMER( Chamber , _SCH_Get_SIM_TIME2( Side , Chamber ) ); // 2016.12.09
			}
			else {
//				KPLT_RESET_BM_RUN_TIMER( Chamber , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) ); // 2016.12.09
				KPLT_RESET_BM_RUN_MS_TIMER( Chamber , _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) * 1000 ); // 2016.12.09
			}
		}
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,mode=%d, slot=%d [1]\n" , Side + 1 , "EQUIP_SPAWN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
		}
		//======================================================================================================================================================
		return;
	}
	//======================================================================================================================================================
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) {
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,mode=%d, slot=%d [2]\n" , Side + 1 , "EQUIP_SPAWN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
		}
		//======================================================================================================================================================
		return;
	}
	//======================================================================================================================================================
	EQUIP_GET_SPAWN_MESSAGE_BM( Side , Chamber , mode , slot , bmfirstslot , Buffer , 255 , FALSE );
	//======================================================================================================================================================
	_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , Buffer );
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,mode=%d, slot=%d\n" , Side + 1 , "EQUIP_SPAWN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
	}
	//======================================================================================================================================================
}
//
int EQUIP_RUN_WAITING_BM( int Side , int Chamber , int mode , int slot , int bmfirstslot , BOOL MaintInfUse ) {
	//---------------------------------------------
	// 2002.03.27 // Modify for BM Processing
	//---------------------------------------------
	char Buffer[265+1];
	int Res;

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] Chamber=%d,mode=%d,slot=%d\n" , Side + 1 , "EQUIP_RUN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
	}

//	sprintf( Buffer , "[BM%d] [SIDE=%d] [mode=%d] [Slot=%d] [bmf=%d]\n" , Chamber - BM1 + 1 , Side , mode , slot , bmfirstslot );
//	_IO_MSGEVENT_PRINTF( "BM_EQ_RUN" , Buffer );

	//======================================================================================================================================================
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) ) { // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( GET_RUN_LD_CONTROL(7) > 0 ) ) { // 2014.01.09
		//
		switch( GET_RUN_LD_CONTROL(0) ) {
		case 1 :
			break;
		case 2 :
			if      ( mode == BUFFER_RUN_WAITING_FOR_TM ) {
				Sleep( _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) * 250 );
			}
			else if ( mode == BUFFER_RUN_WAITING_FOR_FM ) {
				Sleep( _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) * 250 );
			}
			break;
		default :
			if      ( mode == BUFFER_RUN_WAITING_FOR_TM ) {
				if ( _SCH_Get_SIM_TIME1( Side , Chamber ) > 0 ) {
					Sleep( _SCH_Get_SIM_TIME1( Side , Chamber ) );
				}
				else {
					Sleep( _i_SCH_PRM_GET_Process_Run_In_Time( Chamber ) * 1000 );
				}
			}
			else if ( mode == BUFFER_RUN_WAITING_FOR_FM ) {
				if ( _SCH_Get_SIM_TIME2( Side , Chamber ) > 0 ) {
					Sleep( _SCH_Get_SIM_TIME2( Side , Chamber ) );
				}
				else {
					Sleep( _i_SCH_PRM_GET_Process_Run_Out_Time( Chamber ) * 1000 );
				}
			}
			break;
		}
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,mode=%d,slot=%d [1]\n" , Side + 1 , "EQUIP_RUN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
		}
		//======================================================================================================================================================
		return SYS_SUCCESS;
	}
	//======================================================================================================================================================
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) {
		//======================================================================================================================================================
		// 2008.09.27
		//======================================================================================================================================================
		if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
			_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,mode=%d,slot=%d [2]\n" , Side + 1 , "EQUIP_RUN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot );
		}
		//======================================================================================================================================================
		return SYS_ABORTED;
	}
	//======================================================================================================================================================
	EQUIP_GET_SPAWN_MESSAGE_BM( Side , Chamber , mode , slot , bmfirstslot , Buffer , 255 , MaintInfUse );
	//======================================================================================================================================================
	Res = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , Buffer );
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] Chamber=%d,mode=%d,slot=%d,Res=%d\n" , Side + 1 , "EQUIP_RUN_WAITING_BM" , KPLT_GET_LOT_PROGRESS_TIME( Chamber ) , Chamber , mode , slot , Res );
	}
	//======================================================================================================================================================
	return Res;
}
//

