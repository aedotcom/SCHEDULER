#include "default.h"

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <io.h>
#include <memory.h>
#include <winbase.h>
//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "FA_Handling.h"
#include "Gui_Handling.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "sch_Cassmap.h"
#include "iolog.h"
#include "MR_Data.h"
#include "multijob.h"
#include "resource.h"
#include "File_ReadInit.h"
#include "sch_prm_cluster.h"

#include "Robot_Handling.h" // 2016.06.21
#include "sch_prm_FBTPM.h" // 2016.06.21


#include "INF_sch_CommonUser.h"
//================================================================================
extern int SYSTEM_WIN_UPDATE;
//================================================================================

CRITICAL_SECTION CR_LOADUNLOAD;

//int FA_SIDE_TO_PORT[ MAX_CASSETTE_SIDE ]; // 2006.03.09 // 2011.08.16

int  FA_SERVER_IO_MODE = 0; // 2008.06.26
int  FA_SERVER_IO = -1;
//
char *FA_SERVER_FUNCTION_NAME = NULL;
BOOL FA_SERVER_USE = FALSE;
//
int  FA_AGV_FUNCTION[ MAX_CASSETTE_SIDE ];
char *FA_AGV_FUNCTION_NAME[ MAX_CASSETTE_SIDE ];
BOOL FA_AGV_USE[ MAX_CASSETTE_SIDE ];
int  FA_FUNCTION_INTERFACE = 0;

int  FA_HANDOFF_RES[ MAX_CASSETTE_SIDE ];
//=============================================================================
// 2007.03.14
//=============================================================================
//int _LAST_FA_SUBST_PICKPLACE_SIDE[MAX_TM_CHAMBER_DEPTH+1]; // 2007.06.21
//int _LAST_FA_SUBST_PICKPLACE_PT[MAX_TM_CHAMBER_DEPTH+1]; // 2007.06.21
int _LAST_FA_SUBST_PICKPLACE_SIDE[MAX_TM_CHAMBER_DEPTH+2]; // 2007.06.21
int _LAST_FA_SUBST_PICKPLACE_PT[MAX_TM_CHAMBER_DEPTH+2]; // 2007.06.21
int _LAST_FA_SUBST_PICKPLACE_WID[MAX_TM_CHAMBER_DEPTH+2]; // 2009.04.08
int _LAST_FA_SUBST_PICKPLACE_DATA[MAX_TM_CHAMBER_DEPTH+2]; // 2007.06.21
//
BOOL _LAST_FA_SUBST_PICKPLACE_SWAP[MAX_TM_CHAMBER_DEPTH+2]; // 2016.06.21
//=============================================================================

extern char *_lot_printf_sub_string_Dummy[];

//=============================================================================

void INIT_FA_HANDLER_SETTING_DATA( int mode , int side ) {
	// apmode = 0 : Init
	// apmode = 1 : Start
	// apmode = 2 : End
	// apmode = 3 : Start(only)
	// apmode = 4 : End(only)
	int i;
	//
	if ( mode == 0 ) {
		InitializeCriticalSection( &CR_LOADUNLOAD );
		FA_SERVER_IO = -1;
		FA_SERVER_FUNCTION_NAME = NULL;
		FA_SERVER_USE = FALSE;
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
//			FA_SIDE_TO_PORT[ i ] = i + 1; // 2006.03.09 // 2011.08.16
			//
			FA_AGV_USE[ i ] = FALSE;
			FA_AGV_FUNCTION[ i ] = -1;
			FA_AGV_FUNCTION_NAME[ i ] = NULL;
		}
		FA_HANDLER_LOAD_INIT();
		FA_HANDLER_UNLOAD_INIT();
	}
	if ( ( mode == 0 ) || ( mode == 3 ) || ( mode == 4 ) ) { // 2007.11.28
		for ( i = 0 ; i < (MAX_TM_CHAMBER_DEPTH+2) ; i++ ) {
			//
			_LAST_FA_SUBST_PICKPLACE_SWAP[i] = FALSE; // 2016.06.21
			//
			_LAST_FA_SUBST_PICKPLACE_SIDE[i] = -1;
			_LAST_FA_SUBST_PICKPLACE_PT[i] = -1;
			_LAST_FA_SUBST_PICKPLACE_WID[i] = -1;
			_LAST_FA_SUBST_PICKPLACE_DATA[i] = -1;
		}
	}
}

//=============================================================================

void FA_HANDLER_ENTER_CR() {
	EnterCriticalSection( &CR_LOADUNLOAD );
}

void FA_HANDLER_LEAVE_CR() {
	LeaveCriticalSection( &CR_LOADUNLOAD );
}

//=============================================================================

void FA_HANDOFF_RES_SET( int id , int res ) {
	FA_HANDOFF_RES[ id ] = res;
}

int FA_HANDOFF_RES_GET( int id ) {
	return FA_HANDOFF_RES[ id ];
}
//=============================================================================
void FA_HANDLER_WINDOW_POS( HWND hdlg , int type ) {
	#define FA_HANDLER_WINDOW_POSINDEX_SIZE	80
	#define FA_HANDLER_WINDOW_POSINDEX_TERM	30
	int x = FA_HANDLER_WINDOW_POSINDEX_SIZE , y = FA_HANDLER_WINDOW_POSINDEX_SIZE , z;
	int wS;
	RECT    rect;
	//
	z = type / 4;
	switch ( type % 4 ) {
	case 1 :
		//---------------------------------------------------------------------------------------------------------
		GetWindowRect ( hdlg , &rect );
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_XS_REFERENCE_POSITION();
		//
		if ( wS < 640 ) {
			x = GetSystemMetrics( SM_CXSCREEN ) - ( rect.right - rect.left ) - FA_HANDLER_WINDOW_POSINDEX_SIZE;
		}
		else { // 2011.04.20
			x = wS - ( rect.right - rect.left ) - FA_HANDLER_WINDOW_POSINDEX_SIZE + GET_WINDOW_X_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_YS_REFERENCE_POSITION();
		//
		if ( wS >= 480 ) { // 2011.04.20
			y = y + GET_WINDOW_Y_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		break;
	case 2 :
		//---------------------------------------------------------------------------------------------------------
		GetWindowRect ( hdlg , &rect );
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_XS_REFERENCE_POSITION();
		//
		if ( wS >= 640 ) { // 2011.04.20
			x = x + GET_WINDOW_X_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_YS_REFERENCE_POSITION();
		//
		if ( wS < 480 ) {
			y = GetSystemMetrics( SM_CYSCREEN ) - ( rect.bottom - rect.top ) - FA_HANDLER_WINDOW_POSINDEX_SIZE;
		}
		else { // 2011.04.20
			y = wS - ( rect.bottom - rect.top ) - FA_HANDLER_WINDOW_POSINDEX_SIZE + GET_WINDOW_Y_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		break;
	case 3 :
		//---------------------------------------------------------------------------------------------------------
		GetWindowRect ( hdlg , &rect );
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_XS_REFERENCE_POSITION();
		//
		if ( wS < 640 ) {
			x = GetSystemMetrics( SM_CXSCREEN ) - ( rect.right - rect.left ) - FA_HANDLER_WINDOW_POSINDEX_SIZE;
		}
		else { // 2011.04.20
			x = wS - ( rect.right - rect.left ) - FA_HANDLER_WINDOW_POSINDEX_SIZE + GET_WINDOW_X_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_YS_REFERENCE_POSITION();
		//
		if ( wS < 480 ) {
			y = GetSystemMetrics( SM_CYSCREEN ) - ( rect.bottom - rect.top ) - FA_HANDLER_WINDOW_POSINDEX_SIZE;
		}
		else { // 2011.04.20
			y = wS - ( rect.bottom - rect.top ) - FA_HANDLER_WINDOW_POSINDEX_SIZE + GET_WINDOW_Y_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		break;
	default : // 2011.04.20
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_XS_REFERENCE_POSITION();
		//
		if ( wS >= 640 ) { // 2011.04.20
			x = x + GET_WINDOW_X_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		wS = GET_WINDOW_YS_REFERENCE_POSITION();
		//
		if ( wS >= 480 ) { // 2011.04.20
			y = y + GET_WINDOW_Y_REFERENCE_POSITION();
		}
		//---------------------------------------------------------------------------------------------------------
		break;
	}
	if ( z > 0 ) {
		switch ( type % 4 ) {
		case 0 :
			x = x + ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			y = y + ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			break;
		case 1 :
			x = x - ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			y = y + ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			break;
		case 2 :
			x = x + ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			y = y - ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			break;
		case 3 :
			x = x - ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			y = y - ( z * FA_HANDLER_WINDOW_POSINDEX_TERM );
			break;
		}
	}
	SetWindowPos( hdlg , NULL , x , y , 0 , 0 , SWP_NOSIZE | SWP_NOZORDER );
}
//=============================================================================
int FA_SERVER_MODE_GET() {
	if ( FA_SERVER_IO < 0 ) return -1;
	if ( FA_SERVER_USE ) return 1;
	return 0;
}

BOOL FA_SERVER_FUNCTION_SET( int data , char *name ) {
	//	1	On + Enable
	//	2	On + Disable
	//	3	Enable
	//	4	Disable
	//	0	Off
	if ( ( data != 3 ) && ( data != 4 ) ) {
		if ( FA_SERVER_FUNCTION_NAME != NULL ) free( FA_SERVER_FUNCTION_NAME );
		FA_SERVER_FUNCTION_NAME = NULL;
		FA_SERVER_USE = FALSE;
	}
	//
	if ( ( data == 1 ) || ( data == 2 ) ) {
		FA_SERVER_IO = -1; // 2008.06.26
		//
		if ( ( name[0] == 'e' ) && ( name[1] == ':' ) ) { // 2008.06.26
			FA_SERVER_IO_MODE = 1; // 2008.06.26
			FA_SERVER_IO = _FIND_FROM_STRING( _K_F_IO + 1 , name + 2 ); // 2008.06.26
		}
		else {
			FA_SERVER_IO_MODE = 1; // 2008.06.26
			FA_SERVER_IO = _FIND_FROM_STRING( _K_F_IO + 1 , name ); // 2008.06.26
		}
		if ( FA_SERVER_IO < 0 ) {
			FA_SERVER_IO_MODE = 0; // 2008.06.26
			FA_SERVER_IO = _FIND_FROM_STRING( _K_S_IO , name );
		}
	}
	//
	if ( data != 0 ) {
		if ( FA_SERVER_IO == -1 ) return FALSE;
	}
	else {
		FA_SERVER_IO = -1;
	}
	//
	if ( ( data == 1 ) || ( data == 2 ) ) {
		FA_SERVER_FUNCTION_NAME = calloc( strlen( name ) + 1 , sizeof( char ) );
		if ( FA_SERVER_FUNCTION_NAME == NULL ) {
			FA_SERVER_IO = -1;
			return FALSE;
		}
		strcpy( FA_SERVER_FUNCTION_NAME , name );
	}
	//
	if      ( ( data == 1 ) || ( data == 3 ) ) {
		FA_SERVER_USE = TRUE;
	}
	else if ( ( data == 2 ) || ( data == 4 ) ) {
		FA_SERVER_USE = FALSE;
	}
	//
	return TRUE;
}

char *FA_SERVER_FUNCTION_NAME_GET() { return FA_SERVER_FUNCTION_NAME; }
//=============================================================================
//=============================================================================
void FA_FUNCTION_INTERFACE_SET( int id ) { FA_FUNCTION_INTERFACE = id; }
int  FA_FUNCTION_INTERFACE_GET() { return FA_FUNCTION_INTERFACE; }
//=============================================================================
int FA_AGV_MODE_GET( int id ) {
	if ( FA_AGV_FUNCTION[id] < 0 ) return -1;
	if ( FA_AGV_USE[id] ) return 1;
	return 0;
}

int  FA_AGV_FUNCTION_GET( int id ) {
	return FA_AGV_FUNCTION[ id ];
}

BOOL FA_AGV_FUNCTION_SET( int id , int data , char *name ) {
	//	1	On + Enable
	//	2	On + Disable
	//	3	Enable
	//	4	Disable
	//	0	Off
	if ( ( data != 3 ) && ( data != 4 ) ) {
		if ( FA_AGV_FUNCTION_NAME[id] != NULL ) free( FA_AGV_FUNCTION_NAME[id] );
		FA_AGV_FUNCTION_NAME[id] = NULL;
		FA_AGV_USE[id] = FALSE;
	}
	//
	if ( ( data == 1 ) || ( data == 2 ) ) {
		FA_AGV_FUNCTION[id] = _FIND_FROM_STRING( _K_F_IO , name );
	}
	//
	if ( data != 0 ) {
		if ( FA_AGV_FUNCTION[id] == -1 ) return FALSE;
	}
	else {
		FA_AGV_FUNCTION[id] = -1;
	}
	//
	if ( ( data == 1 ) || ( data == 2 ) ) {
		FA_AGV_FUNCTION_NAME[id] = calloc( strlen( name ) + 1 , sizeof( char ) );
		if ( FA_AGV_FUNCTION_NAME[id] == NULL ) {
			FA_AGV_FUNCTION[id] = -1;
			return FALSE;
		}
		strcpy( FA_AGV_FUNCTION_NAME[id] , name );
	}
	//
	if      ( ( data == 1 ) || ( data == 3 ) ) {
		FA_AGV_USE[id] = TRUE;
	}
	else if ( ( data == 2 ) || ( data == 4 ) ) {
		FA_AGV_USE[id] = FALSE;
	}
	//
	return TRUE;
}

char *FA_AGV_FUNCTION_NAME_GET( int id ) { return FA_AGV_FUNCTION_NAME[id]; }

//=============================================================================
void FA_AGV_AUTOHANDLER_SEND_MESSAGE( int id ) {
	char Buffer[256];
	if ( FA_AGV_FUNCTION[ id ] < 0 ) return;
	if ( !FA_AGV_USE[id] ) {
		sprintf( Buffer , "DISABLE CM%d" , id + 1 );
		_dWRITE_FUNCTION_EVENT( FA_AGV_FUNCTION[ id ] , Buffer );
	}
	else {
		switch( FA_FUNCTION_INTERFACE_GET() ) {
		case 0 : case 3 :	sprintf( Buffer , "ENABLE CM%d"  , id + 1 );	break;
		case 1 : case 4 :	sprintf( Buffer , "ENABLE2 CM%d" , id + 1 );	break;
		case 2 : case 5 :	sprintf( Buffer , "ENABLE3 CM%d" , id + 1 );	break;
		}
		_dWRITE_FUNCTION_EVENT( FA_AGV_FUNCTION[ id ] , Buffer );
	}
}
//=============================================================================
//void FA_SIDE_TO_PORT_SET( int Side , int data ) { FA_SIDE_TO_PORT[ Side ] = data; } // 2006.03.09 // 2011.08.17
//int  FA_SIDE_TO_PORT_GET( int Side ) { return FA_SIDE_TO_PORT[ Side ]; } // 2006.03.09 // 2011.08.17
//=============================================================================
int SCHEDULER_Get_CARRIER_INDEX( int Side );

/*
// 2013.05.24
char   FA_SIDE_TO_PORT_STR[64]; // 2011.08.17
char *FA_SIDE_TO_PORT_GETS( int Side , int Pt ) { // 2011.08.17
	int c;
	//

	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
		sprintf( FA_SIDE_TO_PORT_STR , "PORT%d" , Side + 1 );
	}
	else {
		if ( ( Pt < 0 ) || ( Pt >= MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			c = SCHEDULER_Get_CARRIER_INDEX( Side );
			//
			if ( c < 0 ) {
				sprintf( FA_SIDE_TO_PORT_STR , "PORT%d" , Side + 1 );
			}
			else {
				sprintf( FA_SIDE_TO_PORT_STR , "PORT%d:I%d" , Side + 1 , c );
			}
		}
		else {
			if ( _i_SCH_CLUSTER_Get_Ex_MtlOut( Side , Pt ) <= 0 ) {
				sprintf( FA_SIDE_TO_PORT_STR , "PORT%d" , Side + 1 );
			}
			else {
				sprintf( FA_SIDE_TO_PORT_STR , "PORT%d:I%d" , Side + 1 , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( Side , Pt ) );
			}
		}
	}

	/ *
		if ( ( Pt < 0 ) || ( Pt >= MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			c = SCHEDULER_Get_CARRIER_INDEX( Side );
			//
			if ( c < 0 ) {
				sprintf( FA_SIDE_TO_PORT_STR , "PORT%d" , Side + 1 );
			}
			else {
				sprintf( FA_SIDE_TO_PORT_STR , "PORT%d:I%d" , Side + 1 , c );
			}
		}
		else {
			sprintf( FA_SIDE_TO_PORT_STR , "PORT%d:I%d" , Side + 1 , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( Side , Pt ) );
		}
		* /
	return FA_SIDE_TO_PORT_STR;
}
*/

// 2013.05.24
void FA_SIDE_TO_PORT_GETS( int Side , int Pt , char *data ) { // 2011.08.17
	int c;
	//

	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
		sprintf( data , "PORT%d" , Side + 1 );
	}
	else {
		if ( ( Pt < 0 ) || ( Pt >= MAX_CASSETTE_SLOT_SIZE ) ) {
			//
			c = SCHEDULER_Get_CARRIER_INDEX( Side );
			//
			if ( c < 0 ) {
				sprintf( data , "PORT%d" , Side + 1 );
			}
			else {
				sprintf( data , "PORT%d:I%d" , Side + 1 , c );
			}
		}
		else {
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( Side , Pt ) % 100 ) <= 0 ) {
				sprintf( data , "PORT%d" , Side + 1 );
			}
			else {
//				sprintf( data , "PORT%d:I%d" , Side + 1 , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( Side , Pt ) ); // 2018.06.01
				sprintf( data , "PORT%d:I%dS%d" , Side + 1 , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( Side , Pt ) , _i_SCH_CLUSTER_Get_SlotIn( Side , Pt ) ); // 2018.06.01
			}
		}
	}
}

//=============================================================================
// 2013.06.29
void FA_SIDE_TO_SLOT_GET_MTL( int Side , int Pt , char *data ) { // 2012.02.18
	//
	int pi;
	//
	if ( ( Side < 0 ) || ( Side >= MAX_CASSETTE_SIDE ) ) {
		sprintf( data , "" );
	}
	else {
		if ( ( Pt < 0 ) || ( Pt >= MAX_CASSETTE_SLOT_SIZE ) ) {
			sprintf( data , "" );
		}
		else {
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( Side , Pt ) % 100 ) <= 0 ) {
				//
				pi = _i_SCH_CLUSTER_Get_PathIn( Side , Pt );
				if ( ( pi >= BM1 ) && ( pi < TM ) ) { // 2012.08.14
					sprintf( data , ":BM%d" , pi - BM1 + 1 ); // 2013.03.11
				}
				else {
					sprintf( data , "" );
				}
				//
			}
			else {
				sprintf( data , ":%d" , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( Side , Pt ) );
			}
		}
	}
}
// 2013.05.23
//void FA_SIDE_TO_SLOT_GETS( int Side , int Pt , char *data ) { // 2012.02.18 // 2015.10.12
void FA_SIDE_TO_SLOT_GETS( int Side , int Pt , char *data , char *dataE ) { // 2012.02.18 // 2015.10.12
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) {
		sprintf( data , "" );
	}
	else {
		FA_SIDE_TO_SLOT_GET_MTL( Side , Pt , data );
	}
	//
	// 2015.10.12
	//
	if ( ( Side >= 0 ) && ( Side < MAX_CASSETTE_SIDE ) ) {
		if ( ( Pt >= 0 ) && ( Pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( _i_SCH_CLUSTER_Get_Ex_UserDummy( Side , Pt ) != 0 ) {
				sprintf( dataE , "E" );
			}
			else {
				sprintf( dataE , "" );
			}
		}
		else {
			sprintf( dataE , "" );
		}
	}
	else {
		sprintf( dataE , "" );
	}
	//
}


// 2013.06.29
void FA_SIDE_TO_SLOT_GET_L( int Side , int Pt , char *data ) {
	//
	switch( _i_SCH_PRM_GET_MTLOUT_INTERFACE() ) {
	case 1 :
	case 3 :
		FA_SIDE_TO_SLOT_GET_MTL( Side , Pt , data );
		break;
	default :
		sprintf( data , "" );
		break;
	}
}
//=============================================================================
//
#define MAX_dWRITE_FUNCTION_EVENT_TH_TRY2	32 // 2013.10.29
//
int EVENT_SEND_FA_SERVER_IO_ASNCH = 0; // 2013.10.29
//
void SCH_FA_SERVER_IO_dWRITE_FUNCTION_EVENT_TH( char *data ) { // 2013.10.29
	///
	int addr , sts , x;
	//
	if ( EVENT_SEND_FA_SERVER_IO_ASNCH == 0 ) {
		//
		addr = FA_SERVER_IO;
		//
		_dDIGITAL_SHAPE( -1010 , &sts , &x , &x , &addr , data );
		//
		if ( sts != 1 ) {
			//
			_dWRITE_FUNCTION_EVENT_TH( FA_SERVER_IO , data );
			//
			EVENT_SEND_FA_SERVER_IO_ASNCH++;
		}
		//
	}
	else {
		//
		_dWRITE_FUNCTION_EVENT_TH( FA_SERVER_IO , data );
		//
		EVENT_SEND_FA_SERVER_IO_ASNCH++;
		//
		if ( EVENT_SEND_FA_SERVER_IO_ASNCH > MAX_dWRITE_FUNCTION_EVENT_TH_TRY2 ) EVENT_SEND_FA_SERVER_IO_ASNCH = 0;
		//
	}
}

void FA_SEND_MESSAGE_TO_SERVER( int mode , int s , int p , char *data ) {
//	int CommStatus; // 2005.10.26
	//
	char SendBuffer[1024+1]; // 2018.06.18
	//
	if ( FA_SERVER_IO >= 0 ) {
		//
		if ( _SCH_COMMON_FA_SEND_MESSAGE( mode , s , p , data , SendBuffer , 1024 ) ) { // 2018.06.14
			//
			if ( FA_SERVER_IO_MODE == 0 ) { // 2008.06.26
				_dWRITE_STRING_TH( FA_SERVER_IO , SendBuffer ); // 2005.10.26
			}
			else { // 2008.06.26
				SCH_FA_SERVER_IO_dWRITE_FUNCTION_EVENT_TH( SendBuffer ); // 2013.10.29
			}
			//
			GUI_Send_Event_Inserting( SendBuffer );
			//
			if ( _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() >= 2 ) { // 2013.05.04
				_IO_CONSOLE_PRINTF( "FA-MESSAGE\t%s\n" , SendBuffer );
			}
			//
		}
		else {
			if ( FA_SERVER_IO_MODE == 0 ) { // 2008.06.26
				_dWRITE_STRING_TH( FA_SERVER_IO , data ); // 2005.10.26
			}
			else { // 2008.06.26
				SCH_FA_SERVER_IO_dWRITE_FUNCTION_EVENT_TH( data ); // 2013.10.29
			}
			//
			GUI_Send_Event_Inserting( data );
			//
			if ( _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() >= 2 ) { // 2013.05.04
				_IO_CONSOLE_PRINTF( "FA-MESSAGE\t%s\n" , data );
			}
			//
		}
		return;
	}
	//
	switch( _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() ) {
	case 0 : // Con
		break;
	case 1 : // Alw
		GUI_Send_Event_Inserting( data );
		break;
	case 2 : // Con-prn
		_IO_CONSOLE_PRINTF( "FA-MESSAGE\t%s\n" , data );
		break;
	case 3 : // Alw-prn
		GUI_Send_Event_Inserting( data );
		_IO_CONSOLE_PRINTF( "FA-MESSAGE\t%s\n" , data );
		break;
	}
}
//=============================================================================
//=============================================================================
//char FA_TIME_MESSAGE_BUF[128];
char FA_TIME_READ_STRING[12];

char *FA_TIME_READ() {
//	struct tm		Pres_Time; // 2006.10.02
//	_get-systime( &Pres_Time ); // 2006.10.02
//	sprintf( FA_TIME_READ_STRING , "%02d%02d%02d" , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec ); // 2006.10.02
	SYSTEMTIME		SysTime; // 2006.10.02
	GetLocalTime( &SysTime ); // 2006.10.02
	sprintf( FA_TIME_READ_STRING , "%02d%02d%02d" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ); // 2006.10.02
	return FA_TIME_READ_STRING;
}
//=============================================================================
void FA_TIME_START_MESSAGE( int Side ) {
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
	//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
//	sprintf( SendBuffer , "TIME_START PORT%d|%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() ); // 2011.08.16
	//
	FA_SIDE_TO_PORT_GETS( Side , -1 , MsgSltchar );
	//
	sprintf( SendBuffer , "TIME_START %s|%s" , MsgSltchar , FA_TIME_READ() ); // 2011.08.16
	FA_SEND_MESSAGE_TO_SERVER( 1 , Side , -1 , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

void FA_TIME_END2_MESSAGE( int Side , int pt , int id ) { // 2011.09.19
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
	//==================***************************************
	//
	FA_SIDE_TO_PORT_GETS( Side , pt , MsgSltchar );
	//
	if ( _SCH_COMMON_FA_ORDER_MESSAGE( 1 , Side , pt ) ) { // 2018.06.29
		//
		if ( pt != -1 ) {
			sprintf( SendBuffer , "COMPLETE_F %s|%d" , MsgSltchar , id );
			FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pt , SendBuffer );
		}
		//
		sprintf( SendBuffer , "TIME_COMPLETE %s|%s|%d" , MsgSltchar , FA_TIME_READ() , id );
		FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pt , SendBuffer );
		//
	}
	else {
		//
		sprintf( SendBuffer , "TIME_COMPLETE %s|%s|%d" , MsgSltchar , FA_TIME_READ() , id );
		FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pt , SendBuffer );
		//
		if ( pt != -1 ) {
			sprintf( SendBuffer , "COMPLETE_F %s|%d" , MsgSltchar , id );
			FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pt , SendBuffer );
		}
		//
	}
}

void FA_TIME_END_MESSAGE( int Side , int id ) {
	FA_TIME_END2_MESSAGE( Side , -1 , id );
}

/*
// 2012.09.25
void FA_TIME_FIRSTRUN_MESSAGE( int Side ) {
	char SendBuffer[128];
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//==================***************************************
//	sprintf( SendBuffer , "TIME_FIRSTRUN PORT%d|%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() ); // 2011.08.16
	sprintf( SendBuffer , "TIME_FIRSTRUN %s|%s" , FA_SIDE_TO_PORT_GETxS( Side , -1 ) , FA_TIME_READ() ); // 2011.08.16
	FA_SEND_MESSAGE_TO_SERVER( SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

void FA_TIME_LASTRUN_MESSAGE( int Side ) {
	char SendBuffer[128];
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//==================***************************************
//	sprintf( SendBuffer , "TIME_LASTRUN PORT%d|%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() ); // 2011.08.16
	sprintf( SendBuffer , "TIME_LASTRUN %s|%s" , FA_SIDE_TO_PORT_GETxS( Side , -1 ) , FA_TIME_READ() ); // 2011.08.16
	FA_SEND_MESSAGE_TO_SERVER( SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}
*/

// 2012.09.25
void FA_TIME_FIRSTRUN_MESSAGE( int Side , int Pt ) {
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//==================***************************************
//	sprintf( SendBuffer , "TIME_FIRSTRUN PORT%d|%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() ); // 2011.08.16
	//
	FA_SIDE_TO_PORT_GETS( Side , Pt , MsgSltchar );
	//
	sprintf( SendBuffer , "TIME_FIRSTRUN %s|%s" , MsgSltchar , FA_TIME_READ() ); // 2011.08.16
	FA_SEND_MESSAGE_TO_SERVER( 1 , Side , Pt , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

void FA_TIME_LASTRUN_MESSAGE( int Side , int Pt ) {
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//==================***************************************
//	sprintf( SendBuffer , "TIME_LASTRUN PORT%d|%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() ); // 2011.08.16
	//
	FA_SIDE_TO_PORT_GETS( Side , Pt , MsgSltchar );
	//
	sprintf( SendBuffer , "TIME_LASTRUN %s|%s" , MsgSltchar , FA_TIME_READ() ); // 2011.08.16
	FA_SEND_MESSAGE_TO_SERVER( 1 , Side , Pt , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

//void FA_TIME_PM_START_MESSAGE( int Side , int pointer , int module , int wfrid , char *recipe ) { // 2014.02.04
void FA_TIME_PM_START_MESSAGE( int Side , int pointer , int module , int wfrid , int PathDo , char *recipe ) { // 2014.02.04
//	char SendBuffer[127+1]; // 2007.11.28
//	char SendBuffer[2048+127+1]; // 2007.11.28 // 2008.02.13
	char SendBuffer[2555+127+1]; // 2007.11.28 // 2008.02.13
//	char Buffer[127+1]; // 2014.01.30
	char Buffer[256+1]; // 2014.01.30
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//==================***************************************
//	SCHMULTI_MESSAGE_GET_ALL_SUB( Side , pointer , Buffer , 127 , 2 ); // ppid // 2011.09.01 // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL_SUB( Side , pointer , Buffer , 256 , 2 ); // ppid // 2011.09.01 // 2014.01.30
	_i_SCH_IO_SET_RECIPE_PRCS_FILE( module - 1 + PM1 , Buffer ); // 2011.09.01
	//
//	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
	//
	FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
	//
//	_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START PORT%d|%s|PM%d|%d|%s%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , module , wfrid , recipe , Buffer ); // 2007.11.28 // 2008.02.13 // 2011.08.16 // 2011.08.16
//	_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|%d|%s%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Buffer ); // 2007.11.28 // 2008.02.13 // 2011.08.16 // 2011.08.16 // 2014.02.04
	//
	if ( _SCH_COMMON_FA_ORDER_MESSAGE( 2 , Side , pointer ) ) { // 2018.06.29
		//
		if ( SCHMULTI_PROCESSJOB_PROCESS_FIRST_START_CHECK( Side , pointer , wfrid ) ) {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( Side , pointer , Buffer , 127 ) ) {
				_snprintf( SendBuffer , 2555+127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe );
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FIRSTSTART , 0 , 0 , SendBuffer , Side , pointer );
			}
		}
		//
		if ( ( _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() / 2 ) == 0 )
			_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|%d|%s%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Buffer );
		else
			_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|%d|%s%s|%d" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Buffer , PathDo );
		//
		FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
	}
	else {
		//
		if ( ( _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() / 2 ) == 0 ) // 2014.02.04
			_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|%d|%s%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Buffer );
		else // 2014.02.04
			_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|%d|%s%s|%d" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Buffer , PathDo );
		//
		FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
		if ( SCHMULTI_PROCESSJOB_PROCESS_FIRST_START_CHECK( Side , pointer , wfrid ) ) {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( Side , pointer , Buffer , 127 ) ) {
	//			_snprintf( SendBuffer , 127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe ); // 2007.11.28
	//			_snprintf( SendBuffer , 2048+127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe ); // 2007.11.28 // 2008.02.13
				_snprintf( SendBuffer , 2555+127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe ); // 2007.11.28 // 2008.02.13
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_FIRSTSTART , 0 , 0 , SendBuffer , Side , pointer );
			}
		}
	}
//	LeaveCriticalSection( &CR_FA );
}

void FA_TIME_PM_END_MESSAGE( int Side , int pointer , int module , int wfrid , int Result , int Next , char *recipe ) {
//	char SendBuffer[127+1]; // 2007.11.28
//	char SendBuffer[2048+127+1]; // 2007.11.28 // 2008.02.13
	char SendBuffer[2555+127+1]; // 2007.11.28 // 2008.02.13
//	char Buffer[127+1]; // 2014.01.30
	char Buffer[256+1]; // 2014.01.30
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//==================***************************************
//	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
//	_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END PORT%d|%s|PM%d|%d|%s|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , module , wfrid , recipe , Result , Next , Buffer ); // 2007.11.28 // 2008.02.13 // 2011.08.16 // 2011.08.16
	//
	FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
	//
	if ( _SCH_COMMON_FA_ORDER_MESSAGE( 3 , Side , pointer ) ) { // 2018.06.29
		//
		if ( SCHMULTI_PROCESSJOB_PROCESS_LAST_END_CHECK( Side , pointer , wfrid , Next ) ) {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( Side , pointer , Buffer , 127 ) ) {
				_snprintf( SendBuffer , 2555+127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe );
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_LASTFINISH , 0 , 0 , SendBuffer , Side , pointer );
			}
		}
		//
		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END %s|%s|PM%d|%d|%s|%d|%d%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Result , Next , Buffer );
		FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
		//
	}
	else {
		//
		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END %s|%s|PM%d|%d|%s|%d|%d%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Result , Next , Buffer ); // 2007.11.28 // 2008.02.13 // 2011.08.16 // 2011.08.16
		FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
		if ( SCHMULTI_PROCESSJOB_PROCESS_LAST_END_CHECK( Side , pointer , wfrid , Next ) ) {
			if ( _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( Side , pointer , Buffer , 127 ) ) {
	//			_snprintf( SendBuffer , 127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe ); // 2007.11.28
	//			_snprintf( SendBuffer , 2048+127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe ); // 2007.11.28 // 2008.02.13
				_snprintf( SendBuffer , 2555+127 , "%s|PM%d|%d|%s" , Buffer , module , wfrid , recipe ); // 2007.11.28 // 2008.02.13
				SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( ENUM_NOTIFY_PROCESSJOB_LASTFINISH , 0 , 0 , SendBuffer , Side , pointer );
			}
		}
	}
//	LeaveCriticalSection( &CR_FA );
}

void FA_TIME_DUMMY_PM_START_MESSAGE( int Side , int pointer , int module , int srcoffset , int wfrid , char *recipe ) { // 2010.11.30
	char SendBuffer[2555+127+1];
//	char Buffer[127+1]; // 2014.01.30
	char Buffer[256+1]; // 2014.01.30
	char MsgSltchar[16]; /* 2013.05.23 */
	//==================***************************************
//	SCHMULTI_MESSAGE_GET_ALL_SUB( Side , pointer , Buffer , 127 , 2 ); // ppid // 2011.09.01 // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL_SUB( Side , pointer , Buffer , 256 , 2 ); // ppid // 2011.09.01 // 2014.01.30
	_i_SCH_IO_SET_RECIPE_PRCS_FILE( module - 1 + PM1 , Buffer ); // 2011.09.01
	//
//	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
	//
	FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
	//
	if ( srcoffset > 0 ) {
//		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START PORT%d|%s|PM%d|D%d-%d|%s%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , module , srcoffset , wfrid , recipe , Buffer ); // 2011.08.16
		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|D%d-%d|%s%s" , MsgSltchar , FA_TIME_READ() , module , srcoffset , wfrid , recipe , Buffer ); // 2011.08.16
	}
	else {
//		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START PORT%d|%s|PM%d|D%d|%s%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , module , wfrid , recipe , Buffer ); // 2011.08.16
		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_START %s|%s|PM%d|D%d|%s%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Buffer ); // 2011.08.16
	}
	FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
	//==================***************************************
}

void FA_TIME_DUMMY_PM_END_MESSAGE( int Side , int pointer , int module , int srcoffset , int wfrid , int Result , int Next , char *recipe ) {
	char SendBuffer[2555+127+1];
//	char Buffer[127+1]; // 2014.01.30
	char Buffer[256+1]; // 2014.01.30
	char MsgSltchar[16]; /* 2013.05.23 */
	//==================***************************************
//	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
	//
	FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
	//
	if ( srcoffset > 0 ) {
//		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END PORT%d|%s|PM%d|D%d-%d|%s|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , module , srcoffset , wfrid , recipe , Result , Next , Buffer ); // 2011.08.16
		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END %s|%s|PM%d|D%d-%d|%s|%d|%d%s" , MsgSltchar , FA_TIME_READ() , module , srcoffset , wfrid , recipe , Result , Next , Buffer ); // 2011.08.16
	}
	else {
//		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END PORT%d|%s|PM%d|D%d|%s|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , module , wfrid , recipe , Result , Next , Buffer ); // 2011.08.16
		_snprintf( SendBuffer , 2555+127 , "TIME_PROCESS_END %s|%s|PM%d|D%d|%s|%d|%d%s" , MsgSltchar , FA_TIME_READ() , module , wfrid , recipe , Result , Next , Buffer ); // 2011.08.16
	}
	FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
	//==================***************************************
}


void FA_TIME_WID_MESSAGE( int Side , int pointer , int wfrid , char *data ) { // 2003.01.15
	char SendBuffer[127+1];
//	char Buffer[127+1]; // 2014.01.30
	char Buffer[256+1]; // 2014.01.30
	char MsgSltchar[16]; /* 2013.05.23 */
//	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
	SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
//	_snprintf( SendBuffer , 127 , "TIME_WID_READ PORT%d|%s|%d|%s%s" , FA_SIDE_TO_PORT_GET( Side ) , FA_TIME_READ() , wfrid , data , Buffer ); // 2011.08.16
	//
	FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
	//
	_snprintf( SendBuffer , 127 , "TIME_WID_READ %s|%s|%d|%s%s" , MsgSltchar , FA_TIME_READ() , wfrid , data , Buffer ); // 2011.08.16
	FA_SEND_MESSAGE_TO_SERVER( 1 , Side , pointer , SendBuffer );
}

//=============================================================================
//=============================================================================
//void FA_EVENT_MESSAGE_SEND( char *data ) { // 2005.10.26
////	if ( !FA_SERVER_RUN_GET() ) return;
////	EnterCriticalSection( &CR_FA );
//	FA_SEND_MESSAGE_TO_SERVER( data );
////	LeaveCriticalSection( &CR_FA );
//}
//=============================================================================
//=============================================================================

void FA_ACCEPT_MESSAGE( int Side , int Type , int Data ) {
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//
	FA_SIDE_TO_PORT_GETS( Side , -1 , MsgSltchar );
	//
	switch ( Type ) {
	case FA_REGIST		:
//		sprintf( SendBuffer , "REGIST_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "REGIST_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_START		:
//		sprintf( SendBuffer , "START_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "START_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_ABORT		:
//		sprintf( SendBuffer , "ABORT_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "ABORT_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_PAUSE		:
//		sprintf( SendBuffer , "PAUSE_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "PAUSE_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_CONTINUE	:
//		sprintf( SendBuffer , "CONTINUE_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "CONTINUE_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_END			:
//		sprintf( SendBuffer , "END_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "END_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_END_CONTINUE	: // 2017.08.24
		sprintf( SendBuffer , "ENDC_F %s" , MsgSltchar );
		break;
	case FA_CANCEL		:
//		sprintf( SendBuffer , "CANCEL_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "CANCEL_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_DISAPPEAR	:
		sprintf( SendBuffer , "DISAPPEAR_F" );
		break;
	case FA_MAPPING		:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST() ) return;
//		sprintf( SendBuffer , "MAPPING_F PORT%d|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "MAPPING_F %s|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	//
	case FA_TRANSFER_PICK	:
		sprintf( SendBuffer , "TRANSFER_PICK_F" );
		break;
	case FA_TRANSFER_PLACE	:
		sprintf( SendBuffer , "TRANSFER_PLACE_F" );
		break;
	case FA_TRANSFER_MOVE	:
		sprintf( SendBuffer , "TRANSFER_MOVE_F" );
		break;
	case FA_TRANSFER_CLEAR	:
		sprintf( SendBuffer , "TRANSFER_CLEAR_F" );
		break;
	//
	case FA_TRANSFER_PAUSE	:
		sprintf( SendBuffer , "TRANSFER_PAUSE_F" );
		break;
	case FA_TRANSFER_RESUME	:
		sprintf( SendBuffer , "TRANSFER_CONTINUE_F" );
		break;
	case FA_TRANSFER_ABORT	:
		sprintf( SendBuffer , "TRANSFER_ABORT_F" );
		break;
	//
	case FA_LOAD_REQUEST   :
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST() ) return;
//		sprintf( SendBuffer , "LOAD_REQUEST_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "LOAD_REQUEST_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_UNLOAD_REQUEST :
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST() ) return;
//		sprintf( SendBuffer , "UNLOAD_REQUEST_F PORT%d" ,  FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "UNLOAD_REQUEST_F %s" ,  MsgSltchar ); // 2011.08.16
		break;
	//
	case FA_ITEM_CHANGE :
		sprintf( SendBuffer , "PROCESS_ITEM_CHANGE_F PM%d" , Data - PM1 + 1 );
		break;

	default : // 2014.03.14
		return;
		break;
	//
	}
	FA_SEND_MESSAGE_TO_SERVER( 2 , Side , Type , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

void FA_RESULT_MESSAGE( int Side , int Type , int Data ) {
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//
	FA_SIDE_TO_PORT_GETS( Side , -1 , MsgSltchar );
	//
	switch ( Type ) {
	case FA_COMPLETE	:
//		sprintf( SendBuffer , "COMPLETE_F PORT%d|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "COMPLETE_F %s|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_ABORTED		:
//		sprintf( SendBuffer , "ABORTED_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "ABORTED_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_CANCELED	:
//		sprintf( SendBuffer , "CANCELED_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "CANCELED_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_MAP_COMPLETE:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE() ) return;
//		sprintf( SendBuffer , "MAPPING_COMPLETE_F PORT%d|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "MAPPING_COMPLETE_F %s|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	//
	case FA_TRANSFER_PICK	:
		if      ( Data == SYS_ABORTED ) sprintf( SendBuffer , "ABORTED_F TRANSFER_PICK_F" );
		else if ( Data == SYS_ERROR   ) sprintf( SendBuffer , "ERROR_F TRANSFER_PICK_F" );
		else                            sprintf( SendBuffer , "COMPLETE_F TRANSFER_PICK_F" );
		break;
	case FA_TRANSFER_PLACE	:
		if      ( Data == SYS_ABORTED ) sprintf( SendBuffer , "ABORTED_F TRANSFER_PLACE_F" );
		else if ( Data == SYS_ERROR   ) sprintf( SendBuffer , "ERROR_F TRANSFER_PLACE_F" );
		else                            sprintf( SendBuffer , "COMPLETE_F TRANSFER_PLACE_F" );
		break;
	case FA_TRANSFER_MOVE	:
		if      ( Data == SYS_ABORTED ) sprintf( SendBuffer , "ABORTED_F TRANSFER_MOVE_F" );
		else if ( Data == SYS_ERROR   ) sprintf( SendBuffer , "ERROR_F TRANSFER_MOVE_F" );
		else                            sprintf( SendBuffer , "COMPLETE_F TRANSFER_MOVE_F" );
		break;
	case FA_TRANSFER_CLEAR	:
		if      ( Data == SYS_ABORTED ) sprintf( SendBuffer , "ABORTED_F TRANSFER_CLEAR_F" );
		else if ( Data == SYS_ERROR   ) sprintf( SendBuffer , "ERROR_F TRANSFER_CLEAR_F" );
		else                            sprintf( SendBuffer , "COMPLETE_F TRANSFER_CLEAR_F" );
		break;
	//
	case FA_LOAD_COMPLETE:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE() ) return;
//		sprintf( SendBuffer , "LOAD_COMPLETE_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "LOAD_COMPLETE_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_LOAD_COMPLETE2:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE() ) return;
//		sprintf( SendBuffer , "LOAD_COMPLETE2_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "LOAD_COMPLETE2_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_UNLOAD_COMPLETE:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE() ) return;
//		sprintf( SendBuffer , "UNLOAD_COMPLETE_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer , "UNLOAD_COMPLETE_F %s" , MsgSltchar ); // 2011.08.16
		break;
	case FA_UNLOAD_COMPLETE2:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE() ) return;
//		sprintf( SendBuffer, "UNLOAD_COMPLETE2_F PORT%d" , FA_SIDE_TO_PORT_GET( Side ) ); // 2011.08.16
		sprintf( SendBuffer, "UNLOAD_COMPLETE2_F %s" , MsgSltchar ); // 2011.08.16
		break;

	default : // 2014.03.14
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 3 , Side , Type , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

void FA_REJECT_MESSAGE( int Side , int Type , int Data , char *strdata ) {
	char SendBuffer[128];
	char MsgSltchar[16]; /* 2013.05.23 */
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	//
	FA_SIDE_TO_PORT_GETS( Side , -1 , MsgSltchar );
	//
	switch ( Type ) {
	case FA_REGIST		:
//		sprintf( SendBuffer , "REJECT_F PORT%d|REGIST_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|REGIST_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_START		:
//		sprintf( SendBuffer , "REJECT_F PORT%d|START_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|START_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_ABORT		:
//		sprintf( SendBuffer , "REJECT_F PORT%d|ABORT_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|ABORT_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_PAUSE		:
//		sprintf( SendBuffer , "REJECT_F PORT%d|PAUSE_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|PAUSE_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_CONTINUE	:
//		sprintf( SendBuffer , "REJECT_F PORT%d|CONTINUE_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|CONTINUE_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_END			:
//		sprintf( SendBuffer , "REJECT_F PORT%d|END_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|END_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_CANCEL		:
//		sprintf( SendBuffer , "REJECT_F PORT%d|CANCEL_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|CANCEL_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_DISAPPEAR	:
		sprintf( SendBuffer , "REJECT_F DISAPPERA_F" );
		break;
	case FA_MAPPING		:
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT() ) return;
//		sprintf( SendBuffer , "REJECT_F PORT%d|MAPPING_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|MAPPING_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	//
	case FA_TRANSFER_PICK	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_PICK_F" );
		break;
	case FA_TRANSFER_PLACE	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_PLACE_F" );
		break;
	case FA_TRANSFER_MOVE	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_MOVE_F" );
		break;
	case FA_TRANSFER_CLEAR	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_CLEAR_F" );
		break;
	//
	case FA_TRANSFER_PAUSE	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_PAUSE_F" );
		break;
	case FA_TRANSFER_RESUME	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_CONTINUE_F" );
		break;
	case FA_TRANSFER_ABORT	:
		sprintf( SendBuffer , "REJECT_F TRANSFER_ABORT_F" );
		break;
	//
	case FA_LOAD_REQUEST   :
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REJECT() ) return;
//		sprintf( SendBuffer , "REJECT_F PORT%d|LOAD_REQUEST_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|LOAD_REQUEST_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	case FA_UNLOAD_REQUEST :
		if ( _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT() ) return;
//		sprintf( SendBuffer , "REJECT_F PORT%d|UNLOAD_REQUEST_F|%d" , FA_SIDE_TO_PORT_GET( Side ) , Data ); // 2011.08.16
		sprintf( SendBuffer , "REJECT_F %s|UNLOAD_REQUEST_F|%d" , MsgSltchar , Data ); // 2011.08.16
		break;
	//
	case FA_ITEM_CHANGE :
		sprintf( SendBuffer , "REJECT_F PROCESS_ITEM_CHANGE|PM%d" , Data - PM1 + 1 );
		break;
		break;
	//
	case FA_INFO :
		if ( Side == 0 ) { // AGV
			sprintf( SendBuffer , "REJECT_INFO AGV %s %d" , strdata , Data );
			break;
		}
		else if ( Side == 1 ) { // MODULE
			sprintf( SendBuffer , "REJECT_INFO MODULE %s %d" , strdata , Data );
			break;
		}
	default : // 2014.03.14
		return;
		break;
	}
	FA_SEND_MESSAGE_TO_SERVER( 4 , Side , Type , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}





int index_for_dummy( int s , int p ) { // 2013.04.16
	int pi;
	//
	pi = _i_SCH_CLUSTER_Get_PathIn( s , p );
	if ( ( pi >= BM1 ) && ( pi < TM ) ) return 1;
	return 0;
}


//=============================================================================
void _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( int Side , int pointer , int Type , int mode , int module , int where0 , int arm , int runwaferid , int srcwaferid ) {
	char SendBuffer[512+1]; // 2014.01.30
//	char SendBuffer[127+1]; // 2014.01.30
	char Buffer[256+1]; // 2014.01.30
//	char Buffer[127+1]; // 2014.01.30
	int waferid;
	int where;
	int whereL;
	int flowctl = 0;
	int flowdata = -1;
	int specialdata = -1; // 2014.02.27
	char MsgSltchar[16]; /* 2013.05.23 */
	int pd;
	int fside , fpointer; // 2018.06.22
	//==============================================================================
	// 2009.02.02
	//==============================================================================
	if ( _i_SCH_CLUSTER_FlowControl( Side ) ) {
		flowctl = 1;
		if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			flowdata = _i_SCH_CLUSTER_Get_PathRun( Side , pointer , 16 , 2 );
			if ( flowdata >= 20 ) return;
		}
	}
	//==============================================================================
	// 2006.03.29
	//==============================================================================
	if ( where0 >= 100 ) {
		whereL = MAX_TM_CHAMBER_DEPTH+1;
		where  = 100;
	}
	else {
		whereL = where0;
		where  = where0;
	}
	//==============================================================================
	// 2006.03.29
	//==============================================================================
	switch ( Type ) {
	case FA_SUBSTRATE_EVERYIN	:
		if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) _i_SCH_CLUSTER_Do_StartTime( Side , pointer );
		break;
	case FA_SUBSTRATE_EVERYOUT	:
		if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) _i_SCH_CLUSTER_Do_EndTime( Side , pointer );
		break;
	}
	//==============================================================================
	if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 0 ) {
		if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) return; // 2011.09.07
	}
	//==============================================================================
//	if ( _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() == 0 ) // 2014.02.04
	if ( ( _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() % 2 ) == 0 ) // 2014.02.04
		waferid = srcwaferid;
	else
		waferid = runwaferid;
	//==============================================================================
	fside = Side; // 2018.06.22
	fpointer = pointer; // 2018.06.22
	//==============================================================================
	switch ( Type ) {
	case FA_SUBSTRATE_PICK	:
		flowctl++; // 2009.02.03
//		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
		switch( mode ) {
		case FA_SUBST_SUCCESS :
			//
			SYSTEM_WIN_UPDATE = 11; // 2013.03.11
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PICK , Side , pointer , where , arm , module , waferid );
			//
//			_snprintf( SendBuffer , 127 , "STS_PICKED_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PICKED_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( Side , pointer ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PICKED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PICKED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		case FA_SUBST_FAIL :
			//
			SYSTEM_WIN_UPDATE = 11; // 2013.03.11
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PICK , Side , pointer , where , arm , module , waferid );
			//
//			_snprintf( SendBuffer , 127 , "STS_PICKFAIL_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PICKFAIL_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( Side , pointer ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PICKFAIL_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PICKFAIL_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		default :
			//======================================================================================
			// 2007.03.14
			//======================================================================================
			//
			_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] = FALSE; // 2016.06.21
			//
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = Side;
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = pointer;
			//
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = waferid; // 2009.04.08
			//
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = ( flowctl == 2 ) ? flowdata : -1;
			//======================================================================================
//			_snprintf( SendBuffer , 127 , "STS_PICKING_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PICKING_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( Side , pointer ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PICKING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PICKING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		}
		break;
	case FA_SUBSTRATE_PLACE	:
		flowctl++; // 2009.02.03
//		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
		switch( mode ) {
		case FA_SUBST_SUCCESS :
			//
			SYSTEM_WIN_UPDATE = 12; // 2013.03.11
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PLACE , Side , pointer , where , arm , module , waferid );
			//
//			_snprintf( SendBuffer , 127 , "STS_PLACED_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PLACED_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( Side , pointer ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PLACED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			//
			pd = -1;
			if ( ( module >= PM1 ) && ( module < AL ) ) { // 2014.02.05
				if ( ( _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() / 2 ) != 0 ) {
					if ( ( Side >= 0 ) && ( Side < MAX_CASSETTE_SIDE ) ) {
						if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
							//
							pd = _i_SCH_CLUSTER_Get_PathDo( Side , pointer ) - 1;
							//
							if ( pd > _i_SCH_CLUSTER_Get_PathRange( Side , pointer ) ) pd = -1;
							//
						}
					}
				}
			}
			//
			if ( pd < 0 ) {
				_snprintf( SendBuffer , 512 , "STS_PLACED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			}
			else {
				_snprintf( SendBuffer , 512 , "STS_PLACED_F %s|%s|%d|%d|%s%d%s|%d" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer , pd );
			}
			break;
		case FA_SUBST_FAIL :
			//
			SYSTEM_WIN_UPDATE = 12; // 2013.03.11
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PLACE , Side , pointer , where , arm , module , waferid );
			//
//			_snprintf( SendBuffer , 127 , "STS_PLACEFAIL_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PLACEFAIL_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( Side , pointer ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PLACEFAIL_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PLACEFAIL_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		default :
			//======================================================================================
			// 2007.03.14
			//======================================================================================
			//
			_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] = FALSE; // 2016.06.21
			//
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = Side;
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = pointer;
			//
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = waferid; // 2009.04.08
			//
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = ( flowctl == 2 ) ? flowdata : -1;
			//======================================================================================
//			_snprintf( SendBuffer , 127 , "STS_PLACING_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PLACING_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( Side , pointer ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , waferid , Buffer ); // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PLACING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PLACING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer ); // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		}
		break;

/*
//
// 2016.06.21
//
	case FA_SUBSTRATE_PICK2	: // 2007.03.14
		flowctl++; // 2009.02.03
//		if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() != 2 ) return; // 2007.03.14 // 2013.03.11
		if ( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] == -1 ) return; // 2007.11.28
		if ( _LAST_FA_SUBST_PICKPLACE_PT[whereL] == -1 ) return; // 2007.11.28
		if ( _LAST_FA_SUBST_PICKPLACE_WID[whereL] == -1 ) return; // 2009.04.08
		//
		if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() != 2 ) {
			switch( mode ) {
			case FA_SUBST_SUCCESS :
			case FA_SUBST_FAIL :
				//
				SYSTEM_WIN_UPDATE = TRUE; // 2013.03.11
				//
				break;
			}
			return; // 2007.03.14 // 2013.03.11
		}
		//
//		SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 256 ); // 2014.01.30
		flowdata = _LAST_FA_SUBST_PICKPLACE_DATA[whereL];
		//
		FA_SIDE_TO_PORT_GETS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , MsgSltchar );
		//
		switch( mode ) {
		case FA_SUBST_SUCCESS :
			//
			SYSTEM_WIN_UPDATE = TRUE; // 2013.03.11
			//
//			_snprintf( SendBuffer , 127 , "STS_PICKED2_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PICKED2_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PICKED2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PICKED2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1; // 2007.11.28
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1; // 2007.11.28
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1; // 2009.04.08
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1; // 2009.02.12
			break;
		case FA_SUBST_FAIL :
			//
			SYSTEM_WIN_UPDATE = TRUE; // 2013.03.11
			//
//			_snprintf( SendBuffer , 127 , "STS_PICKFAIL2_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PICKFAIL2_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PICKFAIL2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PICKFAIL2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1; // 2007.11.28
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1; // 2007.11.28
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1; // 2009.04.08
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1; // 2009.02.12
			break;
		default :
//			_snprintf( SendBuffer , 127 , "STS_PICKING2_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PICKING2_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PICKING2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PICKING2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		}
		break;
*/



	case FA_SUBSTRATE_PLACE2	: // 2007.03.14
		flowctl++; // 2009.02.03
//		if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() != 2 ) return; // 2007.03.14 // 2013.03.11
		if ( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] == -1 ) return; // 2007.11.28
		if ( _LAST_FA_SUBST_PICKPLACE_PT[whereL] == -1 ) return; // 2007.11.28
		if ( _LAST_FA_SUBST_PICKPLACE_WID[whereL] == -1 ) return; // 2009.04.08
		//
		if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() != 2 ) {
			switch( mode ) {
			case FA_SUBST_SUCCESS :
			case FA_SUBST_FAIL :
				//
				SYSTEM_WIN_UPDATE = 2; // 2013.03.11
				//
				break;
			}
			return; // 2007.03.14 // 2013.03.11
		}
		//
		fside = _LAST_FA_SUBST_PICKPLACE_SIDE[whereL]; // 2018.06.22
		fpointer = _LAST_FA_SUBST_PICKPLACE_PT[whereL]; // 2018.06.22
		//
//		SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 256 ); // 2014.01.30
		flowdata = _LAST_FA_SUBST_PICKPLACE_DATA[whereL];
		//
		FA_SIDE_TO_PORT_GETS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , MsgSltchar );
		//
		switch( mode ) {
		case FA_SUBST_SUCCESS :
			//
			SYSTEM_WIN_UPDATE = 2; // 2013.03.11
			//
//			_snprintf( SendBuffer , 127 , "STS_PLACED2_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PLACED2_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PLACED2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PLACED2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			//
			if ( !_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] ) { // 2016.06.21
				//
				_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1; // 2007.11.28
				_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1; // 2007.11.28
				_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1; // 2009.04.08
				_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1; // 2009.02.12
				//
			}
			break;
		case FA_SUBST_FAIL :
			//
			SYSTEM_WIN_UPDATE = 2; // 2013.03.11
			//
//			_snprintf( SendBuffer , 127 , "STS_PLACEFAIL2_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PLACEFAIL2_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PLACEFAIL2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PLACEFAIL2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1; // 2007.11.28
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1; // 2007.11.28
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1; // 2009.04.08
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1; // 2009.02.12
			break;
		default :
//			_snprintf( SendBuffer , 127 , "STS_PLACING2_F PORT%d|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GET( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16
//			_snprintf( SendBuffer , 127 , "STS_PLACING2_F %s|%s|%d|%d|%d%s" , FA_SIDE_TO_PORT_GETxS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16
//			_snprintf( SendBuffer , 127 , "STS_PLACING2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			_snprintf( SendBuffer , 512 , "STS_PLACING2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer ); // 2009.04.08 // 2011.08.16 // 2013.04.16 // 2014.01.30
			break;
		}
		break;


//
// 2016.06.21
//
	case FA_SUBSTRATE_PICK2	: // 2016.06.21
		flowctl++;
		if ( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] == -1 ) return;
		if ( _LAST_FA_SUBST_PICKPLACE_PT[whereL] == -1 ) return;
		if ( _LAST_FA_SUBST_PICKPLACE_WID[whereL] == -1 ) return;
		//
		//
		fside = _LAST_FA_SUBST_PICKPLACE_SIDE[whereL]; // 2018.06.22
		fpointer = _LAST_FA_SUBST_PICKPLACE_PT[whereL]; // 2018.06.22
		//
		if ( !_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] ) {
			//
			if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() != 2 ) {
				switch( mode ) {
				case FA_SUBST_SUCCESS :
				case FA_SUBST_FAIL :
					//
					SYSTEM_WIN_UPDATE = 2;
					//
					break;
				}
				return;
			}
			//
			SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 256 );
			flowdata = _LAST_FA_SUBST_PICKPLACE_DATA[whereL];
			//
			FA_SIDE_TO_PORT_GETS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , MsgSltchar );
			//
			switch( mode ) {
			case FA_SUBST_SUCCESS :
				//
				SYSTEM_WIN_UPDATE = 2;
				//
				_snprintf( SendBuffer , 512 , "STS_PICKED2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
				//
				_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1;
				_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1;
				_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1;
				_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1;
				break;

			case FA_SUBST_FAIL :
				//
				SYSTEM_WIN_UPDATE = 2;
				//
				_snprintf( SendBuffer , 512 , "STS_PICKFAIL2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
				//
				_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1;
				_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1;
				_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1;
				_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1;
				break;

			default :
				_snprintf( SendBuffer , 512 , "STS_PICKING2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
				break;
			}
			//
		}
		else { // 2016.06.21
			//
			SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 256 );
			flowdata = _LAST_FA_SUBST_PICKPLACE_DATA[whereL];
			//
			FA_SIDE_TO_PORT_GETS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , MsgSltchar );
			//
			if ( _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() == 2 ) {
				//
				switch( mode ) {
				case FA_SUBST_SUCCESS :
					//
					SYSTEM_WIN_UPDATE = 2;
					//
					_snprintf( SendBuffer , 512 , "STS_PICKED2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
					//
					break;

				case FA_SUBST_FAIL :
					//
					SYSTEM_WIN_UPDATE = 2;
					//
					_snprintf( SendBuffer , 512 , "STS_PICKFAIL2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
					//
					break;

				default :
					_snprintf( SendBuffer , 512 , "STS_PICKING2_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
					break;
				}
				//
				FA_SEND_MESSAGE_TO_SERVER( 5 , _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , SendBuffer );
				//
			}
			//
			_snprintf( SendBuffer , 512 , "STS_PICKED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
			//
			FA_SEND_MESSAGE_TO_SERVER( 5 , _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , SendBuffer );
			//
			arm = _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( arm );
			//
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = _i_SCH_MODULE_Get_TM_SIDE( whereL-1 , arm );
			_LAST_FA_SUBST_PICKPLACE_PT[whereL]   = _i_SCH_MODULE_Get_TM_POINTER( whereL-1 , arm );
			_LAST_FA_SUBST_PICKPLACE_WID[whereL]  = _i_SCH_MODULE_Get_TM_WAFER( whereL-1 , arm );
			//
			SCHMULTI_MESSAGE_GET_ALL( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , Buffer , 256 );
			FA_SIDE_TO_PORT_GETS( _LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL] , MsgSltchar );
			//
			_snprintf( SendBuffer , 512 , "STS_PLACING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] , _LAST_FA_SUBST_PICKPLACE_PT[whereL]) ] , _LAST_FA_SUBST_PICKPLACE_WID[whereL] , Buffer );
			//
		}
		//
		break;

		/*
		//
		// 2016.06.21
		//
	case FA_SUBSTRATE_SWAP : // 2014.03.18

		flowctl++;
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 );
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
		switch( mode ) {
		case FA_SUBST_SUCCESS :
			//
			SYSTEM_WIN_UPDATE = TRUE;
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PICKPLACE , Side , pointer , where , arm , module , waferid );
			//
			_snprintf( SendBuffer , 512 , "STS_SWAPPED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			break;
		case FA_SUBST_FAIL :
			//
			SYSTEM_WIN_UPDATE = TRUE;
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PICKPLACE , Side , pointer , where , arm , module , waferid );
			//
			_snprintf( SendBuffer , 512 , "STS_SWAPFAIL_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			break;
		default :
			_snprintf( SendBuffer , 512 , "STS_SWAPPING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			break;
		}
		break;
		*/


	case FA_SUBSTRATE_SWAP	: // 2016.06.21
		//
		flowctl++;
		//
		if ( mode == FA_SUBST_SUCCESS ) {
			//
			Side    = _LAST_FA_SUBST_PICKPLACE_SIDE[whereL];
			pointer = _LAST_FA_SUBST_PICKPLACE_PT[whereL];
			waferid = _LAST_FA_SUBST_PICKPLACE_WID[whereL];
			arm     = _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( arm );
			//
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = -1;
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = -1;
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = -1;
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = -1;
			//
			fside = Side; // 2018.06.22
			fpointer = pointer; // 2018.06.22
			//
		}
		//
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 );
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
		switch( mode ) {
		case FA_SUBST_SUCCESS :
			//
			SYSTEM_WIN_UPDATE = 13;
			//
			_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] = FALSE;
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PLACE , Side , pointer , where , arm , module , waferid );
			//
			_snprintf( SendBuffer , 512 , "STS_PLACED_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			break;
		case FA_SUBST_FAIL :
			//
			SYSTEM_WIN_UPDATE = 13;
			//
			_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] = FALSE;
			//
			SCHMRDATA_Data_Setting_for_PickPlacing( MACRO_PICK , Side , pointer , where , arm , module , waferid );
			//
			_snprintf( SendBuffer , 512 , "STS_PICKFAIL_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			break;
		default :
			//
			_LAST_FA_SUBST_PICKPLACE_SWAP[whereL] = TRUE;
			//
			_LAST_FA_SUBST_PICKPLACE_SIDE[whereL] = Side;
			_LAST_FA_SUBST_PICKPLACE_PT[whereL] = pointer;
			//
			_LAST_FA_SUBST_PICKPLACE_WID[whereL] = waferid;
			//
			_LAST_FA_SUBST_PICKPLACE_DATA[whereL] = ( flowctl == 2 ) ? flowdata : -1;
			//
			_snprintf( SendBuffer , 512 , "STS_PICKING_F %s|%s|%d|%d|%s%d%s" , MsgSltchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , arm , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , waferid , Buffer );
			//
			break;
		}
		break;



	case FA_SUBSTRATE_FIRSTIN	:
//		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
//		_snprintf( SendBuffer , 127 , "STS_FIRSTIN_F PORT%d|%d|%s|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
//		_snprintf( SendBuffer , 127 , "STS_FIRSTIN_F %s|%d|%s|%d%s" , MsgSltchar , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20
//		_snprintf( SendBuffer , 127 , "STS_FIRSTIN_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		_snprintf( SendBuffer , 512 , "STS_FIRSTIN_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		break;
	case FA_SUBSTRATE_EVERYIN	:
		flowctl++; // 2009.02.03
//		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
//		_snprintf( SendBuffer , 127 , "STS_EVERYIN_F PORT%d|%d|%s|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
//		_snprintf( SendBuffer , 127 , "STS_EVERYIN_F %s|%d|%s|%d%s" , MsgSltchar , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20
//		_snprintf( SendBuffer , 127 , "STS_EVERYIN_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		_snprintf( SendBuffer , 512 , "STS_EVERYIN_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		break;
	case FA_SUBSTRATE_EVERYOUT	:
		flowctl++; // 2009.02.03
//		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
//		_snprintf( SendBuffer , 127 , "STS_EVERYOUT_F PORT%d|%d|%s|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
//		_snprintf( SendBuffer , 127 , "STS_EVERYOUT_F %s|%d|%s|%d%s" , MsgSltchar , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20
//		_snprintf( SendBuffer , 127 , "STS_EVERYOUT_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		_snprintf( SendBuffer , 512 , "STS_EVERYOUT_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30

		// 2014.02.26	:0:SUCCESS/1:ABORT/2:REJECT/3:?/4:LOST/5:STOP/6:SKIPPED
		if ( ( Side >= 0 ) && ( Side < MAX_CASSETTE_SIDE ) && ( pointer > 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( _i_SCH_CLUSTER_Get_PathDo( Side , pointer ) == PATHDO_WAFER_RETURN ) {
				specialdata = 5;
			}
			else {
				if ( SCHMULTI_RUNJOB_GET_STOPPED( Side , pointer ) ) { // 2015.03.27
					specialdata = 5;
				}
			}
		}
		//
		break;
	case FA_SUBSTRATE_LASTOUT	:
//		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 127 ); // 2014.01.30
		SCHMULTI_MESSAGE_GET_ALL( Side , pointer , Buffer , 256 ); // 2014.01.30
//		_snprintf( SendBuffer , 127 , "STS_LASTOUT_F PORT%d|%d|%s|%d%s" , FA_SIDE_TO_PORT_GET( Side ) , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16
		//
		FA_SIDE_TO_PORT_GETS( Side , pointer , MsgSltchar );
		//
//		_snprintf( SendBuffer , 127 , "STS_LASTOUT_F %s|%d|%s|%d%s" , MsgSltchar , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20
//		_snprintf( SendBuffer , 127 , "STS_LASTOUT_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		_snprintf( SendBuffer , 512 , "STS_LASTOUT_F %s|%s%d|%s|%d%s" , MsgSltchar , _lot_printf_sub_string_Dummy[ index_for_dummy(Side , pointer) ] , mode , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module ) , where , Buffer ); // 2011.08.16 // 2013.11.20 // 2014.01.30
		break;

	default : // 2014.03.14
		return;
		break;

	}
	//
	/*
	// 2014.02.27
	if ( flowctl == 2 ) { // 2009.02.02
		if ( flowdata != -1 ) {
			sprintf( Buffer , "|%d" , flowdata );
			strcat( SendBuffer , Buffer );
		}
	}
	*/

	// 2014.02.27
	if ( ( flowctl == 2 ) && ( flowdata != -1 ) ) {
		if ( specialdata != -1 ) {
			sprintf( Buffer , "|%d" , ( specialdata * 100 ) + flowdata );
			strcat( SendBuffer , Buffer );
		}
		else {
			sprintf( Buffer , "|%d" , flowdata );
			strcat( SendBuffer , Buffer );
		}
	}
	else {
		if ( specialdata != -1 ) {
			sprintf( Buffer , "|%d" , ( specialdata * 100 ) );
			strcat( SendBuffer , Buffer );
		}
	}
	//
	FA_SEND_MESSAGE_TO_SERVER( 5 , fside , fpointer , SendBuffer );
}

void FA_INFO_MESSAGE( int Type , int Data ) {
	int i;
	char SendBuffer[128];
	char Buffer[16];
//	if ( !FA_SERVER_RUN_GET() ) return;
//	EnterCriticalSection( &CR_FA );
	if ( Type == 0 ) {
		strcpy( SendBuffer , "INFO MODULE " );
		if ( Data == 0 ) {
			for ( i = CM1 ; i < TM ; i++ ) {
				if ( ( i == AL ) || ( i == IC ) ) continue;
				if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
					if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )	strcat( SendBuffer , "1" );
					else							strcat( SendBuffer , "0" );
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
						switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
						case 0 :	strcat( SendBuffer , "2" );	break;
						case 1 :	strcat( SendBuffer , "1" );	break;
						case 2 :	strcat( SendBuffer , "3" );	break;
						case 3 :	strcat( SendBuffer , "4" );	break;
						}
					}
					else {
						strcat( SendBuffer , "0" );
					}
				}
			}
		}
		else {
			if      ( ( Data >= CM1 ) && ( Data < ( CM1 + MAX_CASSETTE_SIDE ) ) ) {
				sprintf( Buffer , "CM%d " , Data - CM1 + 1 );
				strcat( SendBuffer , Buffer );
			}
			else if ( ( Data >= PM1 ) && ( Data < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
				sprintf( Buffer , "PM%d " , Data - PM1 + 1 );
				strcat( SendBuffer , Buffer );
			}
			else if ( ( Data >= BM1 ) && ( Data < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
				sprintf( Buffer , "BM%d " , Data - BM1 + 1 );
				strcat( SendBuffer , Buffer );
			}
			if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( Data ) )	strcat( SendBuffer , "1" );
				else								strcat( SendBuffer , "0" );
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_MODE( Data ) ) {
					switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( Data ) ) {
					case 0 :	strcat( SendBuffer , "2" );	break;
					case 1 :	strcat( SendBuffer , "1" );	break;
					case 2 :	strcat( SendBuffer , "3" );	break;
					case 3 :	strcat( SendBuffer , "4" );	break;
					}
				}
				else {
					strcat( SendBuffer , "0" );
				}
			}
		}
	}
	else if ( Type == 1 ) {
		strcpy( SendBuffer , "INFO AGV " );
		if ( Data < 100 ) {
			sprintf( Buffer , "CM%d " , Data + 1 );
			strcat( SendBuffer , Buffer );
			if ( FA_AGV_MODE_GET( Data ) == 1 )	strcat( SendBuffer , "1" );
			else								strcat( SendBuffer , "0" );
		}
		else if ( Data == 100 ) {
			strcat( SendBuffer , "0" );
		}
		else if ( Data == 101 ) {
			strcat( SendBuffer , "1" );
		}
	}
	else { // 2014.03.14
		return;
	}
	FA_SEND_MESSAGE_TO_SERVER( 99 , -1 , -1 , SendBuffer );
//	LeaveCriticalSection( &CR_FA );
}

//=============================================================================
//=============================================================================
//NotUse Enable Disable DisableHW EnablePM
void FA_MDL_STATUS_IO_SET( int ch ) {
	int i , s , e;
	if ( ch == 9999 ) {
		s = CM1;
		e = F_IC;
	}
	else {
		s = ch;
		e = ch;
	}
	for ( i = s ; i <= e ; i++ ) {
		if ( ( i >= CM1 ) && ( i <= F_IC ) ) {
			if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )
					IO_MDL_STATUS_SET( i , 1 );
				else
					IO_MDL_STATUS_SET( i , 0 );
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
					switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
					case 0 :	IO_MDL_STATUS_SET( i , 2 );	break;
					case 1 :	IO_MDL_STATUS_SET( i , 1 );	break;
					case 2 :	IO_MDL_STATUS_SET( i , 3 );	break;
					case 3 :	IO_MDL_STATUS_SET( i , 4 );	break;
					}
				}
				else {
					IO_MDL_STATUS_SET( i , 0 );
				}
			}
		}
	}
}

void FA_AGV_STATUS_IO_SET( int ch ) {
	int i , j;
	if ( ( ch >= 0 ) && ( ch < MAX_CASSETTE_SIDE ) ) {
		if ( FA_AGV_MODE_GET( ch ) == 1 )	IO_AGV_STATUS_SET( ch + CM1 , 1 );
		else								IO_AGV_STATUS_SET( ch + CM1 , 0 );
	}
	for ( j = 0 , i = CM1 ; i < ( CM1 + MAX_CASSETTE_SIDE ) ; i++ ) {
		if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				if ( FA_AGV_MODE_GET( i-1 ) != 1 ) {
					IO_AGV_STATUS_SET( 0 , 0 );
					return;
				}
				j++;
			}
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) ) {
				case 0 :
					break;
				case 1 :
					if ( FA_AGV_MODE_GET( i-1 ) != 1 ) {
						IO_AGV_STATUS_SET( 0 , 0 );
						return;
					}
					break;
				case 2 :
					break;
				case 3 :
					if ( FA_AGV_MODE_GET( i-1 ) != 1 ) {
						IO_AGV_STATUS_SET( 0 , 0 );
						return;
					}
					break;
				}
				j++;
			}
		}
	}
	if ( j > 0 ) IO_AGV_STATUS_SET( 0 , 1 );
}
//
void FA_RUN_STATUS_IO_SET( int *OldStatus ) {
	int i , j;
	//0=idle / 1=running / 5=disable
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _i_SCH_SYSTEM_USING_GET( i ) > 0 ) {
			if ( *OldStatus != 1 ) {
				*OldStatus = 1;
				IO_RUN_STATUS_SET( 1 );
			}
			return;
		}
	}
	j = 0;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( CM1 + i ) ) j++;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( CM1 + i ) ) {
				switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( CM1 + i ) ) {
				case 1 :
				case 3 :
					j++;
					break;
				}
			}
		}
	}
	if ( j == 0 ) {
		if ( *OldStatus != 5 ) {
			*OldStatus = 5;
			IO_RUN_STATUS_SET( 5 );
		}
		return;
	}
	j = 0;
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( PM1 + i ) ) j++;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( PM1 + i ) ) {
				switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( PM1 + i ) ) {
				case 1 :
				case 3 :
					j++;
					break;
				}
			}
		}
	}
	if ( j == 0 ) {
		if ( *OldStatus != 5 ) {
			*OldStatus = 5;
			IO_RUN_STATUS_SET( 5 );
		}
		return;
	}
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2006.07.25
		j = 0;
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( !_i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( BM1 + i ) ) j++;
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_MODE( BM1 + i ) ) {
					switch( _i_SCH_PRM_GET_MODULE_MODE_for_SW( BM1 + i ) ) {
					case 1 :
					case 3 :
						j++;
						break;
					}
				}
			}
		}
		if ( j == 0 ) {
			if ( *OldStatus != 5 ) {
				*OldStatus = 5;
				IO_RUN_STATUS_SET( 5 );
			}
			return;
		}
	}
	if ( *OldStatus != 0 ) {
		*OldStatus = 0;
		IO_RUN_STATUS_SET( 0 );
	}
}

BOOL FA_MAIN_BUTTON_SET_CHECK( int side ) {
	int i;
	//
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() < 3 ) { // 2013.10.02 // 2018.10.24
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() < 2 ) { // 2013.10.02 // 2018.10.24
		//
		for ( i = 0 ; i < 250 ; i++ ) {
			//
			if ( !_SCH_SYSTEM_SIDE_CLOSING_GET( side ) ) break;
			//
			Sleep(10);
			//
		}
		//
		if ( _i_SCH_IO_GET_MAIN_BUTTON( side ) == CTC_IDLE ) {
			_i_SCH_IO_SET_MAIN_BUTTON( side , CTC_WAIT_HAND_OFF );
			return TRUE;
		}
	}
	//
	return FALSE;
	//
}















/*

// 2015.03.26

int FA_TEST_MESSAGE_COUNT1;
int FA_TEST_MESSAGE_COUNT2;
int FA_TEST_MESSAGE_CONTROL = FALSE;

void FA_TEST_MESSAGE( int Side , int mode , int data , int data2 ) { // 2015.03.09
	char SendBuffer[127+1];
	char MsgSltchar[16];
	//
	FA_SIDE_TO_PORT_GETS( Side , -1 , MsgSltchar );
	//
	if ( mode == 0 ) {
		_snprintf( SendBuffer , 127 , "TIME_TEST1 %s %s %d %d" , MsgSltchar , FA_TIME_READ() , data , data2 );
	}
	else {
		_snprintf( SendBuffer , 127 , "TIME_TEST2 %s %s %d %d" , MsgSltchar , FA_TIME_READ() , data , data2 );
	}
	//
	FA_SEND_MESSAGE_TO_SERVER( SendBuffer );
	//
//	_IO_CIM_PRINTF( "%s\n" , SendBuffer );
	//
}

void FA_TEST_MESSAGE_Thread() { // 2015.03.09
	//
	while ( FA_TEST_MESSAGE_CONTROL ) {
		//
		FA_TEST_MESSAGE( 0 , 0 , FA_TEST_MESSAGE_COUNT1 , 0 ); // 2015.03.09
		//
		FA_TEST_MESSAGE_COUNT1++;
		//
		Sleep(100);
	}
	//
	_IO_CIM_PRINTF( "TIME_TEST1\tSTOP...\n" );
	//
	_endthread();
	//
}

void FA_TEST_SEND( int data ) { // 2015.03.09
	//
	FA_TEST_MESSAGE( 0 , 1 , FA_TEST_MESSAGE_COUNT2 , data ); // 2015.03.09
	//
	FA_TEST_MESSAGE_COUNT2++;
	//
}

void FA_TEST_STOP() { // 2015.03.09
	//
	FA_TEST_MESSAGE_CONTROL = FALSE;
	//
}

void FA_TEST_START() { // 2015.03.09
	//
	_IO_CIM_PRINTF( "TIME_TEST1\tSTART...\n" );
	//
	FA_TEST_MESSAGE_COUNT1 = 0;
	FA_TEST_MESSAGE_COUNT2 = 0;
	//
	if ( FA_TEST_MESSAGE_CONTROL ) {
		FA_TEST_MESSAGE_CONTROL = FALSE;
		Sleep(1000);
	}
	//
	FA_TEST_MESSAGE_CONTROL = TRUE;
	//
	if ( _beginthread( (void *) FA_TEST_MESSAGE_Thread , 0 , 0 ) == -1 ) {
		FA_TEST_MESSAGE_CONTROL = FALSE;
		_IO_CIM_PRINTF( "THREAD FAIL FA_TEST_MESSAGE_Thread\n" );
	}
	//
}


*/