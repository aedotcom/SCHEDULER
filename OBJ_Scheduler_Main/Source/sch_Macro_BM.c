#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "Equip_Handling.h"
#include "User_Parameter.h"
#include "iolog.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "sch_processmonitor.h"
#include "sch_prm_FBTPM.h"
#include "sch_EIL.h"
#include "FA_Handling.h"

#include "sch_estimate.h" // 2016.10.22

//================================================================================
extern int  _SCH_TAG_BM_LAST_PLACE;
//================================================================================
int _SCH_MACRO_inside_BM_HAS_COUNT( int ch ) {
	int i , k = 0;
	for ( i = _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
		if ( _i_SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) k++;
	}
	return k;
}

int _SCH_MACRO_inside_BM_FIRST_WAFER( int ch , int option ) { // 2009.12.10
	int i;
	//
	if ( option >= 0 ) return option;
	//
	for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _i_SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) return _i_SCH_MODULE_Get_BM_WAFER( ch , i );
	}
	return 0;
}

void _SCH_MACRO_SPAWN_WAITING_BM_OPERATION( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata ) {
	//-----------------------------------
	int xside , xp , i , Wfr;
	char elsestr[32]; // 2011.05.17
	char MsgSltchar[16]; /* 2013.05.23 */
	//-----------------------------------

//	char Buffer[256];
//	sprintf( Buffer , "[BM%d] [SIDE=%d] [mode=%d] [option=%d] [modeset=%d] [logwhere=%d][logdata=%d]\n" , ch - BM1 + 1 , side , mode , option , modeset , logwhere , logdata );
//	_IO_MSGEVENT_PRINTF( "BM_MC_SPAWN" , Buffer );

	//
	Wfr = -1;
	//
//	if ( _i_SCH_PRM_GET_EIL_INTERFACE() == 0 ) { // 2011.05.17 // 2011.07.27
//		xside = side; // 2011.07.27
//		strcpy( elsestr , "" ); // 2011.07.27
//	} // 2011.07.27
//	else { // 2011.05.17 // 2011.07.27
		//
		xside = -1;
		xp = -1; // 2016.11.17
		//
		for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _i_SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				//
				xside = _i_SCH_MODULE_Get_BM_SIDE( ch , i );
				xp    = _i_SCH_MODULE_Get_BM_POINTER( ch , i );
				Wfr   = _i_SCH_MODULE_Get_BM_WAFER( ch , i );
				//
				FA_SIDE_TO_SLOT_GET_L( xside , xp , MsgSltchar );
				//
				sprintf( elsestr , "%c%d%s" , 9 , Wfr , MsgSltchar );
				if ( side == xside ) break;
				//
			}
		}
		//
		if ( xside == -1 ) {
			/*
			// 2012.07.20
			i = EIL_Get_One_Blank_Wafer_Slot( side , ( mode == BUFFER_TM_STATION ) );
			if ( i == -1 ) {
				xside = side;
				strcpy( elsestr , "" );
			}
			else {
				xside = side;
				Wfr   = i;
				sprintf( elsestr , "%c%d" , 9 , Wfr );
			}
			*/
			// 2012.07.20
			i = EIL_Get_One_Blank_Wafer_Slot( side , &xside , ( mode == BUFFER_TM_STATION ) );
			if ( i == -1 ) {
				strcpy( elsestr , "" );
			}
			else {
				Wfr   = i;
				sprintf( elsestr , "%c%d" , 9 , Wfr );
			}
		}
		//
//	} // 2011.07.27


	//
	//===========================================================
	//
	// 2016.11.17
	//
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( xside , xp ) ) return;
	//
	//===========================================================
	//

	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.10
		//
		i = _EIL_FMMODE_MACRO_SPAWN_WAITING_BM_OPERATION( mode , xside , ch , option , modeset , logwhere , logdata , elsestr );
		//
		if ( i != -1 ) return;
	}
	//
	if      ( logwhere >= 20000 ) { // 'B'
		if      ( ( logwhere - 20000 ) == 0 ) { // fm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "FM Handling %s Goto FMSIDE(B)%cWHFMGOFMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "FM Handling %s Goto TMSIDE(B)%cWHFMGOTMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
		}
		else if ( ( logwhere - 20000 ) == 1 ) { // tm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM Handling %s Goto FMSIDE(B)%cWHTMGOFMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM Handling %s Goto TMSIDE(B)%cWHTMGOTMSIDE BM%d:%d,B%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
		}
		else if ( ( logwhere - 20000 ) >= 2 ) { // tm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM%d Handling %s Goto FMSIDE(B)%cWHTM%dGOFMSIDE BM%d:%d,B%s\n" , ( logwhere - 20000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 20000 ) , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM%d Handling %s Goto TMSIDE(B)%cWHTM%dGOTMSIDE BM%d:%d,B%s\n" , ( logwhere - 20000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 20000 ) , ch - BM1 + 1 , logdata , elsestr );
		}
	}
	else if ( logwhere >= 10000 ) { // 'S'
		if      ( ( logwhere - 10000 ) == 0 ) { // fm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "FM Handling %s Goto FMSIDE(S)%cWHFMGOFMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "FM Handling %s Goto TMSIDE(S)%cWHFMGOTMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
		}
		else if ( ( logwhere - 10000 ) == 1 ) { // tm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM Handling %s Goto FMSIDE(S)%cWHTMGOFMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM Handling %s Goto TMSIDE(S)%cWHTMGOTMSIDE BM%d:%d,S%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
		}
		else if ( ( logwhere - 10000 ) >= 2 ) { // tm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM%d Handling %s Goto FMSIDE(S)%cWHTM%dGOFMSIDE BM%d:%d,S%s\n" , ( logwhere - 10000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 10000 ) , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM%d Handling %s Goto TMSIDE(S)%cWHTM%dGOTMSIDE BM%d:%d,S%s\n" , ( logwhere - 10000 ) , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ( logwhere - 10000 ) , ch - BM1 + 1 , logdata , elsestr );
		}
	}
	else {
		if      ( logwhere == 0 ) { // fm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "FM Handling %s Goto FMSIDE%cWHFMGOFMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "FM Handling %s Goto TMSIDE%cWHFMGOTMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
		}
		else if ( logwhere == 1 ) { // tm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM Handling %s Goto FMSIDE%cWHTMGOFMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM Handling %s Goto TMSIDE%cWHTMGOTMSIDE BM%d:%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata , elsestr );
		}
		else if ( logwhere >= 2 ) { // tm
			if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM%d Handling %s Goto FMSIDE%cWHTM%dGOFMSIDE BM%d:%d%s\n" , logwhere , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , logwhere , ch - BM1 + 1 , logdata , elsestr );
			else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( xside , "TM%d Handling %s Goto TMSIDE%cWHTM%dGOTMSIDE BM%d:%d%s\n" , logwhere , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , logwhere , ch - BM1 + 1 , logdata , elsestr );
		}
	}
	//-----------------------------------
	/*
	// 2012.07.20
	if ( mode == BUFFER_FM_STATION ) {
		EQUIP_SPAWN_WAITING_BM( side , ch , BUFFER_RUN_WAITING_FOR_FM , _SCH_MACRO_inside_BM_FIRST_WAFER( ch , option ) , _SCH_MACRO_inside_BM_HAS_COUNT( ch ) );
		_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
		PROCESS_MONITOR_Set_Last( side , ch , BUFFER_RUN_WAITING_FOR_FM );
//		PROCESS_MONITOR_Set_Pointer( side , ch , -1 , -1 , -1 ); // 2007.12.28
//		PROCESS_MONITOR_Set_Pointer( side , ch , -1 , -1 , logwhere % 10000 ); // 2007.12.28 // 2011.06.15
		PROCESS_MONITOR_Set_Pointer( side , ch , -1 , Wfr , logwhere % 10000 ); // 2007.12.28 // 2011.06.15
		PROCESS_MONITOR_Set_Status( side , ch , 1 , FALSE , -1 );
	}
	else if ( mode == BUFFER_TM_STATION ) {
		//---------------------------------------------------
		// 2008.03.25
		//---------------------------------------------------
		if ( ( logwhere % 10000 ) == 0 ) { // fm
			if ( _SCH_TAG_BM_LAST_PLACE == ch ) { // 2008.03.25
				_SCH_TAG_BM_LAST_PLACE = 0;
			}
		}
		//---------------------------------------------------
		EQUIP_SPAWN_WAITING_BM( side , ch , BUFFER_RUN_WAITING_FOR_TM , _SCH_MACRO_inside_BM_FIRST_WAFER( ch , option ) , _SCH_MACRO_inside_BM_HAS_COUNT( ch ) );
		_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch );
		PROCESS_MONITOR_Set_Last( side , ch , BUFFER_RUN_WAITING_FOR_TM );
//		PROCESS_MONITOR_Set_Pointer( side , ch , -1 , -1 , -1 ); // 2007.12.28
//		PROCESS_MONITOR_Set_Pointer( side , ch , -1 , -1 , ( logdata * 100 ) + ( logwhere % 10000 ) ); // 2007.12.28 // 2011.06.15
		PROCESS_MONITOR_Set_Pointer( side , ch , -1 , Wfr , ( logdata * 100 ) + ( logwhere % 10000 ) ); // 2007.12.28 // 2011.06.15
		PROCESS_MONITOR_Set_Status( side , ch , 1 , FALSE , -1 );
	}
	else {
		return;
	}
	//-----------------------------------
	if ( modeset ) _i_SCH_MODULE_Set_BM_FULL_MODE( ch , mode );
	//-----------------------------------
	*/
	// 2012.07.20
	//-----------------------------------
//	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) xside = side; // 2014.01.02
	if ( ( xside < 0 ) || ( xside >= MAX_CASSETTE_SIDE ) ) { // 2014.01.02
		if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) xside = side;
	}
	//-----------------------------------
	if ( mode == BUFFER_FM_STATION ) {
		//
		EST_BM_FM_SIDE_START( ch ); // 2016.10.22
		//
		EQUIP_SPAWN_WAITING_BM( xside , ch , BUFFER_RUN_WAITING_FOR_FM , _SCH_MACRO_inside_BM_FIRST_WAFER( ch , option ) , _SCH_MACRO_inside_BM_HAS_COUNT( ch ) );
		_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
		PROCESS_MONITOR_Set_Last( xside , ch , BUFFER_RUN_WAITING_FOR_FM );
		PROCESS_MONITOR_Set_Pointer( xside , ch , -1 , Wfr , logwhere % 10000 , 0 , 0 ); // 2007.12.28 // 2011.06.15 // 2016.04.26
		PROCESS_MONITOR_Set_Status( xside , ch , 1 , FALSE , -1 );
	}
	else if ( mode == BUFFER_TM_STATION ) {
		//
		EST_BM_TM_SIDE_START( ch ); // 2016.10.22
		//
		//---------------------------------------------------
		// 2008.03.25
		//---------------------------------------------------
		if ( ( logwhere % 10000 ) == 0 ) { // fm
			if ( _SCH_TAG_BM_LAST_PLACE == ch ) { // 2008.03.25
				_SCH_TAG_BM_LAST_PLACE = 0;
			}
		}
		//---------------------------------------------------
		EQUIP_SPAWN_WAITING_BM( xside , ch , BUFFER_RUN_WAITING_FOR_TM , _SCH_MACRO_inside_BM_FIRST_WAFER( ch , option ) , _SCH_MACRO_inside_BM_HAS_COUNT( ch ) );
		_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch );
		PROCESS_MONITOR_Set_Last( xside , ch , BUFFER_RUN_WAITING_FOR_TM );
		PROCESS_MONITOR_Set_Pointer( xside , ch , -1 , Wfr , ( logdata * 100 ) + ( logwhere % 10000 ) , 0 , 0 ); // 2007.12.28 // 2011.06.15 // 2016.04.26
		PROCESS_MONITOR_Set_Status( xside , ch , 1 , FALSE , -1 );
	}
	else {
		return;
	}
	//-----------------------------------
	if ( modeset ) _i_SCH_MODULE_Set_BM_FULL_MODE( ch , mode );
	//-----------------------------------
}


int _SCH_MACRO_RUN_WAITING_BM_OPERATION( int mode , int side , int ch , int option , BOOL modeset , int logwhere , int logdata ) {
	int res;
	//-----------------------------------
//	char Buffer[256];
//	sprintf( Buffer , "[BM%d] [SIDE=%d] [mode=%d] [option=%d] [modeset=%d] [logwhere=%d][logdata=%d]\n" , ch - BM1 + 1 , side , mode , option , modeset , logwhere , logdata );
//	_IO_MSGEVENT_PRINTF( "BM_MC_RUN" , Buffer );
	//

	//
	//===========================================================
	//
	// 2016.11.17
	//
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , -1 ) ) return SYS_ABORTED;
	//
	//===========================================================
	//

	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.10
		//
		return _EIL_FMMODE_MACRO_RUN_WAITING_BM_OPERATION( mode , side , ch , option , modeset , logwhere , logdata );
		//
	}
	//
	if      ( logwhere == 0 ) { // fm
		if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Run FMSIDE%cWHFMGOFMSIDEW BM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata );
		else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( side , "FM Handling %s Run TMSIDE%cWHFMGOTMSIDEW BM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata );
	}
	else if ( logwhere == 1 ) { // tm
		if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Run FMSIDE%cWHTMGOFMSIDEW BM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata );
		else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( side , "TM Handling %s Run TMSIDE%cWHTMGOTMSIDEW BM%d:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , ch - BM1 + 1 , logdata );
	}
	else if ( logwhere >= 2 ) { // tm
		if      ( mode == BUFFER_FM_STATION ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Run FMSIDE%cWHTM%dGOFMSIDEW BM%d:%d\n" , logwhere , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , logwhere , ch - BM1 + 1 , logdata );
		else if ( mode == BUFFER_TM_STATION ) _i_SCH_LOG_LOT_PRINTF( side , "TM%d Handling %s Run TMSIDE%cWHTM%dGOTMSIDEW BM%d:%d\n" , logwhere , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , 9 , logwhere , ch - BM1 + 1 , logdata );
	}
	if ( mode == BUFFER_FM_STATION ) {
		//
		EST_BM_FM_SIDE_START( ch ); // 2016.10.22
		//
		res = EQUIP_RUN_WAITING_BM( side , ch , BUFFER_RUN_WAITING_FOR_FM , option , _SCH_MACRO_inside_BM_HAS_COUNT( ch ) , FALSE );
	}
	else if ( mode == BUFFER_TM_STATION ) {
		//
		EST_BM_TM_SIDE_START( ch ); // 2016.10.22
		//
		//---------------------------------------------------
		// 2008.03.25
		//---------------------------------------------------
		if ( logwhere == 0 ) { // fm
			if ( _SCH_TAG_BM_LAST_PLACE == ch ) { // 2008.03.25
				_SCH_TAG_BM_LAST_PLACE = 0;
			}
		}
		//---------------------------------------------------
		//
		res = EQUIP_RUN_WAITING_BM( side , ch , BUFFER_RUN_WAITING_FOR_TM , option , _SCH_MACRO_inside_BM_HAS_COUNT( ch ) , FALSE );
	}
	else {
		res = SYS_ABORTED;
	}
	//
	if ( res != SYS_ABORTED ) {
		//
		if ( mode == BUFFER_FM_STATION ) { // 2016.10.22
			EST_BM_FM_SIDE_END( ch ); // 2016.10.22
		}
		else if ( mode == BUFFER_TM_STATION ) { // 2016.10.22
			EST_BM_TM_SIDE_END( ch ); // 2016.10.22
		}
		//
		if ( modeset ) _i_SCH_MODULE_Set_BM_FULL_MODE( ch , mode );
	}
	return res;
}


