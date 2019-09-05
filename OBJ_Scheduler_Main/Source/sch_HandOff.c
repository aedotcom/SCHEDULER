#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "Equip_Handling.h"
#include "sch_prm.h"
#include "iolog.h"
#include "Errorcode.h"
#include "Timer_Handling.h"
#include "User_Parameter.h"
#include "FA_Handling.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Scheduling HandOffIn/Out Part
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Scheduler_HandOffIn_Part( int CHECKING_SIDE , int ch1 , int ch2 ) {
	int xc1 , xc2;
	//
	if ( ( ch1 >= 0 ) && ( ch2 >= 0 ) ) {
		if ( ch1 != ch2 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff In Part Start%cHANDOFFINSTART %d:%d\n" , ch1 - CM1 + 1 , ch2 - CM1 + 1 , 9 , ch1 , ch2 );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Start%cHANDOFFINSTART %d:%d\n" , ch1 - CM1 + 1 , 9 , ch1 , ch2 );
		}
	}
	else if ( ch1 >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Start%cHANDOFFINSTART %d:%d\n" , ch1 - CM1 + 1 , 9 , ch1 , ch2 );
	}
	else if ( ch2 >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Start%cHANDOFFINSTART %d:%d\n" , ch2 - CM1 + 1 , 9 , ch1 , ch2 );
	}
	else {
		return SYS_SUCCESS; // 2010.01.19
	}
	//
	if ( ch1 >= 0 ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch1 ) <= 0 ) {
			xc1 = ch1;
		}
		else {
			xc1 = -1;
		}
		_i_SCH_MODULE_Inc_Mdl_Run_Flag( ch1 );
	}
	else {
		xc1 = -1;
	}
	if ( ( ch2 >= 0 ) && ( ch1 != ch2 ) ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( ch2 ) <= 0 ) {
			xc2 = ch2;
		}
		else {
			xc2 = -1;
		}
		_i_SCH_MODULE_Inc_Mdl_Run_Flag( ch2 );
	}
	else {
		xc2 = -1;
	}
	if ( EQUIP_CARRIER_HANDOFF( TRUE , xc1 , xc2 ) == SYS_ABORTED ) { // 2005.09.28
		if ( ( xc1 >= 0 ) && ( xc2 >= 0 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff In Part Fail%cHANDOFFINFAIL %d:%d\n" , xc1 - CM1 + 1 , xc2 - CM1 + 1 , 9 , xc1 , xc2 );
		}
		else if ( xc1 >= 0 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Fail%cHANDOFFINFAIL %d:%d\n" , xc1 - CM1 + 1 , 9 , xc1 , xc2 );
		}
		else if ( xc2 >= 0 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Fail%cHANDOFFINFAIL %d:%d\n" , xc2 - CM1 + 1 , 9 , xc1 , xc2 );
		}
		return SYS_ABORTED;
	}
	if ( ( xc1 >= 0 ) && ( xc2 >= 0 ) ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff In Part Success%cHANDOFFINSUCCESS %d:%d\n" , xc1 - CM1 + 1 , xc2 - CM1 + 1 , 9 , xc1 , xc2 );
	}
	else if ( xc1 >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Success%cHANDOFFINSUCCESS %d:%d\n" , xc1 - CM1 + 1 , 9 , xc1 , xc2 );
	}
	else if ( xc2 >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Success%cHANDOFFINSUCCESS %d:%d\n" , xc2 - CM1 + 1 , 9 , xc1 , xc2 );
	}
	return SYS_SUCCESS;
}


/* // 2011.06.09
int Scheduler_HandOffOut_Part( int CHECKING_SIDE , BOOL run , int *ch1 , int *ch2 ) {
	//
	if ( ( ( *ch1 < 0 ) || ( *ch1 >= PM1 ) ) && ( ( *ch2 < 0 ) || ( *ch2 >= PM1 ) ) ) {
		*ch1 = -1;
		*ch2 = -1;
		return SYS_SUCCESS; // 2010.01.19
	}
	//
	if ( run ) {
		if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) && ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
			if ( *ch1 != *ch2 ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch1 - CM1 + 1 , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
		}
		else if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		else if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
	}
	//
	if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( *ch1 ) > 0 ) {
			*ch1 = -1;
		}
	}
	else {
		*ch1 = -1;
	}
	if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) && ( *ch1 != *ch2 ) ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( *ch2 ) > 0 ) {
			*ch2 = -1;
		}
	}
	else {
		*ch2 = -1;
	}
	//
	if ( ( *ch1 == -1 ) && ( *ch2 == -1 ) ) return SYS_SUCCESS;
	//
	if ( run ) {
		if ( EQUIP_CARRIER_HANDOFF( FALSE , *ch1 , *ch2 ) == SYS_ABORTED ) {
			//
			if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) && ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch1 - CM1 + 1 , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
			else if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
			else if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
			//
			return SYS_ABORTED;
		}
		//
		if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) && ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch1 - CM1 + 1 , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		else if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		else if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		//
	}
	//
	return SYS_SUCCESS;
}
*/

int Scheduler_HandOffOut_Part( int CHECKING_SIDE , BOOL run , int *ch1 , int *ch2 ) {
	//
	if ( ( ( *ch1 < 0 ) || ( *ch1 >= PM1 ) ) && ( ( *ch2 < 0 ) || ( *ch2 >= PM1 ) ) ) {
		*ch1 = -1;
		*ch2 = -1;
		return SYS_SUCCESS; // 2010.01.19
	}
	//
	if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( *ch1 ) > 0 ) {
			*ch1 = -1;
		}
	}
	else {
		*ch1 = -1;
	}
	if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) && ( *ch1 != *ch2 ) ) {
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( *ch2 ) > 0 ) {
			*ch2 = -1;
		}
	}
	else {
		*ch2 = -1;
	}
	//
	if ( ( *ch1 == -1 ) && ( *ch2 == -1 ) ) return SYS_SUCCESS;
	//
	if ( !run ) return SYS_SUCCESS;
	//
	if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) && ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
		if ( *ch1 != *ch2 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch1 - CM1 + 1 , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
	}
	else if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
	}
	else if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
	}
	//
	if ( EQUIP_CARRIER_HANDOFF( FALSE , *ch1 , *ch2 ) == SYS_ABORTED ) {
		//
		if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) && ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
			if ( *ch1 != *ch2 ) {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch1 - CM1 + 1 , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
			}
		}
		else if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		else if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		//
		return SYS_ABORTED;
	}
	//
	if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) && ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
		if ( *ch1 != *ch2 ) {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch1 - CM1 + 1 , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
		}
	}
	else if ( ( *ch1 >= 0 ) && ( *ch1 < PM1 ) ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch1 - CM1 + 1 , 9 , *ch1 , *ch2 );
	}
	else if ( ( *ch2 >= 0 ) && ( *ch2 < PM1 ) ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , *ch2 - CM1 + 1 , 9 , *ch1 , *ch2 );
	}
	//
	return SYS_SUCCESS;
}

int  _In_HandOff_Out_Skip_Check[MAX_CHAMBER]; // 2011.06.01

int Scheduler_HandOffIn_Ready_Part( int CHECKING_SIDE , int ch ) { // 2011.05.21
	//
	int i , f , l , xc[MAX_CHAMBER] , xs[MAX_CHAMBER] , xccnt;
	//
	xccnt = 0;
	//
	for ( i = CM1 ; i < PM1 ; i++ ) {
		//
		if ( ( ch != -1 ) && ( ch != i ) ) continue;
		//
		if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( CHECKING_SIDE , i ) != MUF_01_USE ) continue;
		//
		if ( _i_SCH_MODULE_Get_Mdl_Run_Flag( i ) == 0 ) {
			//
			if ( !_In_HandOff_Out_Skip_Check[i] ) { // 2011.06.01
				//
				xc[xccnt] = i;	xs[xccnt] = 0;	xccnt++;
				//
			}
		}
		//
		_In_HandOff_Out_Skip_Check[i] = FALSE; // 2011.06.01
		//
		_i_SCH_MODULE_Inc_Mdl_Run_Flag( i );
		//
	}
	//
	// 2012.04.03
	/*
	if ( ( xc[0] >= 0 ) && ( xc[1] >= 0 ) ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff In Part Start%cHANDOFFINSTART %d:%d\n" , xc[0] - CM1 + 1 , xc[1] - CM1 + 1 , 9 , xc[0] , xc[1] );
	}
	else if ( xc[0] >= 0 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Start%cHANDOFFINSTART %d:%d\n" , xc[0] - CM1 + 1 , 9 , xc[0] , -2 );
	}
	else {
		if ( xccnt == 0 ) return SYS_SUCCESS;
	}
	*/
	//
	if ( xccnt <= 0 ) return SYS_SUCCESS;
	if ( xccnt == 1 ) {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Start%cHANDOFFINSTART %d:%d:%d\n" , xc[0] - CM1 + 1 , 9 , xc[0] , -2 , ch );
	}
	else {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d,%d) HandOff In Part Start%cHANDOFFINSTART %d:%d:%d\n" , xc[0] - CM1 + 1 , xc[1] - CM1 + 1 , 9 , xc[0] , xc[1] , ch );
	}
	//
	while( TRUE ) {
		//
		f = 0;
		//
		for ( i = 0 ; i < xccnt ; i++ ) {
			//
			if ( xs[i] == 0 ) { // HandOffIn
				//
				if ( EQUIP_CARRIER_HANDOFF( TRUE , xc[i] , -1 ) == SYS_ABORTED ) {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Fail%cHANDOFFINFAIL %d:%d\n" , xc[i] - CM1 + 1 , 9 , xc[i] , -2 );
					xs[i] = 9;
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff In Part Success%cHANDOFFINSUCCESS %d:%d\n" , xc[i] - CM1 + 1 , 9 , xc[i] , -2 );
					//
					xs[i] = 1;
					//
					f = 1;
				}
			}
			//
			if ( xs[i] == 1 ) { // Ready
				//
				KPLT_PROCESS_TIME_INIT( xc[i] , _i_SCH_PRM_GET_Process_Run_In_Time( xc[i] ) , _i_SCH_PRM_GET_Process_Run_Out_Time( xc[i] ) );
				//
				_i_SCH_EQ_BEGIN_END_RUN( xc[i] , FALSE , "" , FALSE );
				//
				xs[i] = 2;
				//
				f = 1;
			}
			//
			if ( xs[i] == 2 ) { // Ready CHeck
				//
				l = EQUIP_BEGIN_END_STATUS( xc[i] );
				//
				if ( ( l == SYS_ABORTED ) || ( l == SYS_ERROR ) ) {
					xs[i] = 9;
				}
				else if ( l == SYS_SUCCESS ) {
					//
					xs[i] = 3;
					//
				}
				else {
					//
					f = 1;
					//
				}
				//
			}
			//
			if ( xs[i] == 9 ) {
				if ( f == 0 ) f = 2;
			}
			//
		}
		//
		Sleep(1);
		//
		if ( f != 1 ) break;
		//
	}
	//
	if ( f != 0 ) return SYS_ABORTED;
	//
	return SYS_SUCCESS;
}


void Scheduler_HandOffOut_Clear() { // 2011.06.01
	int i;
	for ( i = CM1 ; i < PM1 ; i++ ) _In_HandOff_Out_Skip_Check[i] = FALSE;
}

void Scheduler_HandOffOut_Skip_Set( int ch ) { // 2011.06.01
	_In_HandOff_Out_Skip_Check[ch] = TRUE;
}
int Scheduler_HandOffOut_Skip_Get( int ch ) { // 2011.06.01
	return _In_HandOff_Out_Skip_Check[ch];
}

int Scheduler_HandOffOut_End_Part( int CHECKING_SIDE , int ch , int pointer , int wafer ) {
	//
	int l;
	char MsgSltchar[16]; /* 2013.05.23 */
	//
	if ( ( wafer > 0 ) && ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
		//
		FA_SIDE_TO_SLOT_GET_L( CHECKING_SIDE , pointer , MsgSltchar );
		//
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d%c%d%s\n" , ch - CM1 + 1 , 9 , ch , -2 , 9 , wafer , MsgSltchar );
	}
	else {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Start%cHANDOFFOUTSTART %d:%d\n" , ch - CM1 + 1 , 9 , ch , -2 );
	}
	//
	_In_HandOff_Out_Skip_Check[ch] = FALSE; // 2011.06.01
	//
	if ( EQUIP_CARRIER_HANDOFF( FALSE , ch , -1 ) == SYS_ABORTED ) {
		//
		if ( ( wafer > 0 ) && ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
			//
			FA_SIDE_TO_SLOT_GET_L( CHECKING_SIDE , pointer , MsgSltchar );
			//
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d%c%d%s\n" , ch - CM1 + 1 , 9 , ch , -2 , 9 , wafer , MsgSltchar );
		}
		else {
			_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Fail%cHANDOFFOUTFAIL %d:%d\n" , ch - CM1 + 1 , 9 , ch , -2 );
		}
		//
		return SYS_ABORTED;
	}
	//
	if ( ( wafer > 0 ) && ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
		//
		FA_SIDE_TO_SLOT_GET_L( CHECKING_SIDE , pointer , MsgSltchar );
		//
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d%c%d%s\n" , ch - CM1 + 1 , 9 , ch , -2 , 9 , wafer , MsgSltchar );
	}
	else {
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "Carrier(%d) HandOff Out Part Success%cHANDOFFOUTSUCCESS %d:%d\n" , ch - CM1 + 1 , 9 , ch , -2 );
	}
	//
	_i_SCH_EQ_BEGIN_END_RUN( ch , TRUE , "" , FALSE );
	//
	while ( TRUE ) {
		//
		l = EQUIP_BEGIN_END_STATUS( ch );
		//
		if ( ( l == SYS_ABORTED ) || ( l == SYS_ERROR ) ) {
			return SYS_ABORTED;
		}
		else if ( l == SYS_SUCCESS ) {
			return SYS_SUCCESS;
		}
		//
		Sleep(1);
		//
	}
	return SYS_SUCCESS;
}




