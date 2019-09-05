#include "default.h"

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <io.h>
#include <memory.h>
#include <winbase.h>
//================================================================================
#include "EQ_Enum.h"

#include "Equip_Handling.h"
#include "FA_Handling.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "sch_Cassmap.h"
#include "MR_Data.h"
#include "resource.h"
#include "File_ReadInit.h"
#include "system_Tag.h"

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int  FA_LOAD_CONTROLLER_RUN[ MAX_CASSETTE_SIDE ];

int  FA_MAPPING_SIGNAL[ MAX_CASSETTE_SIDE ];
int  FA_MAPPING_CANCEL[ MAX_CASSETTE_SIDE ]; // 2008.03.19
BOOL FA_LOADING_SIGNAL[ MAX_CASSETTE_SIDE ];
BOOL FA_LOADREQ_SIGNAL[ MAX_CASSETTE_SIDE ];

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_MAPPING_SIGNAL_SET( int Side , int data ) { FA_MAPPING_SIGNAL[ Side ] = data; }
int  FA_MAPPING_SIGNAL_GET( int Side ) { return FA_MAPPING_SIGNAL[ Side ]; }

void FA_MAPPING_CANCEL_SET( int Side , int data ) { FA_MAPPING_CANCEL[ Side ] = data; } // 2008.03.19
int  FA_MAPPING_CANCEL_GET( int Side ) { return FA_MAPPING_CANCEL[ Side ]; } // 2008.03.19

void FA_LOADING_SIGNAL_SET( int Side , BOOL data ) { FA_LOADING_SIGNAL[ Side ] = data; }
BOOL FA_LOADING_SIGNAL_GET( int Side ) { return FA_LOADING_SIGNAL[ Side ]; }

void FA_LOADREQ_SIGNAL_SET( int Side , BOOL data ) { FA_LOADREQ_SIGNAL[ Side ] = data; }
BOOL FA_LOADREQ_SIGNAL_GET( int Side ) { return FA_LOADREQ_SIGNAL[ Side ]; }

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_LOAD_CONTROLLER_RUN_STATUS_SET( int Side , int data ) { FA_LOAD_CONTROLLER_RUN[ Side ] = data; }
int  FA_LOAD_CONTROLLER_RUN_STATUS_GET( int Side ) { return FA_LOAD_CONTROLLER_RUN[ Side ]; }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_HANDLER_LOAD_INIT() {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		FA_LOAD_CONTROLLER_RUN[ i ] = 0;
		//
		FA_MAPPING_SIGNAL[ i ] = FALSE;
		FA_MAPPING_CANCEL[ i ] = FALSE; // 2008.03.19
		//
		FA_LOADING_SIGNAL[ i ] = FALSE;
		FA_LOADREQ_SIGNAL[ i ] = FALSE;
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_HandOff_Load_Proc_WM_INITDIALOG( HWND hdlg , int Cassette , int *mode , char *CassetteString ) {
	//
	sprintf( CassetteString , "Cassette%d/PORT%d" , Cassette - CM1 + 1 , Cassette - CM1 + 1 );
	//
	FA_HANDLER_WINDOW_POS( hdlg , Cassette - CM1 );
	FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MODULE , CassetteString );
	if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
		*mode = 0;
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Running Load Enter Function"*/ SCH_Get_String_Table( ST_INDEX_LOAD_ENTER ) );
		EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN_ENTER" );
		KWIN_Item_Disable( hdlg , IDCANCEL );
		//
		switch ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) {
		case 3 :	KWIN_Item_Hide( hdlg , IDOK );	break;
		default :	KWIN_Item_Disable( hdlg , IDOK );	break;
		}
		//--------------------------------------------------------------------------
		switch ( _i_SCH_PRM_GET_AUTO_HANDOFF( Cassette ) ) {
		case 0 : case 2 : case 4 : KWIN_Item_Disable( hdlg , IDYES ); break;
		case 1 : case 3 : case 5 : KWIN_Item_Hide( hdlg , IDYES ); break;
		}
	}
	else {
		*mode = 1;
		//
		if ( ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) / 4 ) == 1 ) {
			KWIN_Item_Disable( hdlg , IDCANCEL );
		}
		else {
			KWIN_Item_Enable( hdlg , IDCANCEL );
		}
		//
		switch ( _i_SCH_PRM_GET_AUTO_HANDOFF( Cassette ) ) {
		case 0 : case 2 : case 4 :
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If you need to run for Load Function ,\nPress [Load] Button.\nIf Load Operation has been done ,\nPress [Load Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_LOAD_LOAD ) );
			KWIN_Item_Enable( hdlg , IDYES );
			//
			switch( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) {
			case 2 :	KWIN_Item_Enable( hdlg , IDOK );	break;
			case 3 :	KWIN_Item_Hide( hdlg , IDOK );	break;
			default :	KWIN_Item_Disable( hdlg , IDOK );	break;
			}
			//
			break;
		case 1 : case 3 : case 5 :
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If Load Operation has been done ,\nPress [Load Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_LOAD_COMPLETE ) );
			KWIN_Item_Hide( hdlg , IDYES );
			KWIN_Item_Enable( hdlg , IDOK );
			break;
		}
	}
}

BOOL FA_HandOff_Load_Proc_WM_TIMER( HWND hdlg , int Cassette , int *mode ) {
	if ( *mode == 0 ) {
		switch( EQUIP_READ_FUNCTION( Cassette ) ) {
		case SYS_SUCCESS :
			*mode = 1;
			//
			if ( ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) / 4 ) == 1 ) {
				KWIN_Item_Disable( hdlg , IDCANCEL );
			}
			else {
				KWIN_Item_Enable( hdlg , IDCANCEL );
			}
			//
			switch ( _i_SCH_PRM_GET_AUTO_HANDOFF( Cassette ) ) {
			case 0 : case 2 : case 4 :
				KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If you need to run for Load Function ,\nPress [Load] Button.\nIf Load Operation has been done ,\nPress [Load Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_LOAD_LOAD ) );
				KWIN_Item_Enable( hdlg , IDYES );
				//
				switch( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) {
				case 2 :	KWIN_Item_Enable( hdlg , IDOK );	break;
				case 3 :	KWIN_Item_Hide( hdlg , IDOK );	break;
				default :	KWIN_Item_Disable( hdlg , IDOK );	break;
				}
				break;
			case 1 : case 3 : case 5 :
				KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If Load Operation has been done ,\nPress [Load Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_LOAD_COMPLETE ) );
				KWIN_Item_Hide( hdlg , IDYES );
				KWIN_Item_Enable( hdlg , IDOK );
				break;
			}
			break;
		case SYS_RUNNING :
			break;
		default :
			return FALSE;
			break;
		}
	}
	else if ( *mode == 2 ) {
		FA_HANDOFF_RES_SET( Cassette-CM1 , EQUIP_READ_FUNCTION( Cassette ) );
		switch( FA_HANDOFF_RES_GET( Cassette-CM1 ) ) {
		case SYS_RUNNING :
			break;
		case SYS_SUCCESS :
			if ( ( ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) == 0 ) || ( ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) == 3 ) ) { // 2004.12.15
				if ( *mode == 99 ) break;
				*mode = 99;
				FA_HANDOFF_RES_SET( Cassette-CM1 , SYS_SUCCESS );
				if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
					EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN_LEAVE" );
				}
				return FALSE;
			}
			else {
				KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If Load Operation has been done ,\nPress [Load Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_LOAD_COMPLETE ) );
				KWIN_Item_Enable( hdlg , IDOK );
				*mode = 3;
			}
			break;
		default :
			if ( *mode == 99 ) break;
			*mode = 99;
			FA_HANDOFF_RES_SET( Cassette-CM1 , SYS_ABORTED );
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN_LEAVE" );
			}
			return FALSE;
			break;
		}
	}
	return TRUE;
}

BOOL FA_HandOff_Load_Proc_WM_COMMAND( HWND hdlg , int wParam , int Cassette , int *mode , char *CassetteString ) {
	int Res;
	switch( wParam ) {
	case IDOK :
		if ( *mode == 99 ) return TRUE;
		if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) {
			Res = IDYES;
		}
		else {
			Res = MessageBox( hdlg , /*"Do you want to Set Load Complete?"*/ SCH_Get_String_Table( ST_INDEX_LOAD_CONFIRM_COMPLETE ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
		}
		if ( IDYES == Res ) {
			FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_SUCCESS );
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN_LEAVE" );
			}
			return FALSE;
		}
		break;
	case IDCANCEL :
		//
		if ( *mode == 99 ) return TRUE;
		//
		switch ( _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() ) { // 2018.12.20
		case 1 :
		case 3 :
			Res = IDYES;
			break;
		default :
			Res = MessageBox( hdlg , /*"Do you want to Abort or Cancel?"*/ SCH_Get_String_Table( ST_INDEX_LOAD_CONFIRM_ABORT ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
			break;
		}
		//
		if ( IDYES == Res ) {
			FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
			if ( *mode == 2 ) EQUIP_RUN_FUNCTION_ABORT( Cassette );
			*mode = 99;
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN_LEAVE" );
			}
			return FALSE;
		}
		break;
	case IDYES :
		if ( *mode == 99 ) return TRUE;
		if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) {
			Res = IDYES;
		}
		else {
			Res = MessageBox( hdlg , /*"Do you want to Start Load Function?"*/ SCH_Get_String_Table( ST_INDEX_LOAD_CONFIRM_START ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
		}
		if ( IDYES == Res ) {
			*mode = 2;
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Running Load Function\nWait Load Function Success\nIf you want to stop(abort) ,\nPress [Abort] Button"*/ SCH_Get_String_Table( ST_INDEX_LOAD_RUNNING ) );
			EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN" );
			KWIN_Item_Enable( hdlg , IDCANCEL );
			KWIN_Item_Disable( hdlg , IDYES );
			if ( ( _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) == 3 ) { // 2004.12.15
				KWIN_Item_Hide( hdlg , IDOK ); // 2004.12.15
			} // 2004.12.15
			else {
				KWIN_Item_Disable( hdlg , IDOK );
			}
		}
		break;
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#define MAKE_FA_HANDOFF_LOAD( ZZZ ) BOOL APIENTRY FA_HandOff_Load_##ZZZ##_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) { \
	static int mode , id , Cassette; \
	static char CassetteString[24]; \
	switch ( msg ) { \
	case WM_INITDIALOG: \
		Cassette = (int) lParam; \
		FA_HandOff_Load_Proc_WM_INITDIALOG( hdlg , Cassette , &mode , CassetteString ); \
		id = SetTimer( hdlg , 2000 + Cassette - CM1 , 200 , NULL ); \
		return TRUE; \
	case WM_TIMER : \
		if ( !FA_HandOff_Load_Proc_WM_TIMER( hdlg , Cassette , &mode ) ) EndDialog( hdlg , 0 ); \
		return TRUE; \
	case WM_COMMAND : \
		if ( !FA_HandOff_Load_Proc_WM_COMMAND( hdlg , wParam , Cassette , &mode , CassetteString ) ) EndDialog( hdlg , 0 ); \
		return TRUE; \
	case WM_DESTROY : \
		KillTimer( hdlg , id ); \
		return TRUE; \
	} \
	return FALSE; \
}

MAKE_FA_HANDOFF_LOAD( 1 )
MAKE_FA_HANDOFF_LOAD( 2 )
MAKE_FA_HANDOFF_LOAD( 3 )
MAKE_FA_HANDOFF_LOAD( 4 )


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_AGV_Load_Normal_Function( int Cassette , BOOL Disable ) {
	char AGVString[MAX_CASSETTE_SIDE][24];
	FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
	if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
		FA_HANDLER_ENTER_CR();
		if ( EQUIP_RUN_FUNCTION( Cassette , "HAND_OFF_IN_ENTER" ) == SYS_ABORTED ) {
			FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
			FA_HANDLER_LEAVE_CR();
			return;
		}
		FA_HANDLER_LEAVE_CR();
	}
	while( TRUE ) {
		FA_HANDLER_ENTER_CR();
		if ( _dREAD_FUNCTION( FA_AGV_FUNCTION_GET( Cassette - CM1 ) ) != SYS_RUNNING ) {
			//
			switch( FA_LOAD_CONTROLLER_RUN_STATUS_GET( Cassette - CM1 ) % 10 ) { // 2013.07.10
			case 2 :
				if ( Disable ) {
					sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d DISABLE REDO" , Cassette - CM1 + 1 );
				}
				else {
					switch( FA_FUNCTION_INTERFACE_GET() ) {
					case 0 : case 3 :	sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d ENABLE REDO"  , Cassette - CM1 + 1 );	break;
					case 1 : case 4 :	sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d ENABLE2 REDO" , Cassette - CM1 + 1 );	break;
					case 2 : case 5 :	sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d ENABLE3 REDO" , Cassette - CM1 + 1 );	break;
					}
				}
				break;
			default :
				if ( Disable ) {
					sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d DISABLE" , Cassette - CM1 + 1 );
				}
				else {
					switch( FA_FUNCTION_INTERFACE_GET() ) {
					case 0 : case 3 :	sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d ENABLE"  , Cassette - CM1 + 1 );	break;
					case 1 : case 4 :	sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d ENABLE2" , Cassette - CM1 + 1 );	break;
					case 2 : case 5 :	sprintf( AGVString[ Cassette - CM1 ] , "LOAD CM%d ENABLE3" , Cassette - CM1 + 1 );	break;
					}
				}
				break;
			}
			//
			_dRUN_SET_FUNCTION( FA_AGV_FUNCTION_GET( Cassette - CM1 ) , AGVString[ Cassette - CM1 ] );
			FA_HANDLER_LEAVE_CR();
			break;
		}
		FA_HANDLER_LEAVE_CR();
		Sleep(100);
	}
	while( TRUE ) {
		FA_HANDLER_ENTER_CR();
		FA_HANDOFF_RES_SET( Cassette - CM1 , _dREAD_FUNCTION( FA_AGV_FUNCTION_GET( Cassette - CM1 ) ) );
		if ( FA_HANDOFF_RES_GET( Cassette - CM1 ) != SYS_RUNNING ) {
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_IN_LEAVE" );
			}
			FA_HANDLER_LEAVE_CR();
			break;
		}
		FA_HANDLER_LEAVE_CR();
		Sleep(100);
	}
}
//---------------------------------------------------------------------------------------
BOOL APIENTRY FA_HandOff_Load_Again_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int mode;
	switch ( msg ) {
	case WM_INITDIALOG:
		mode = ( int ) lParam;
		FA_HANDLER_WINDOW_POS( hdlg , mode );
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Do you want to load operation again???\n(Already Load Completed)"*/ SCH_Get_String_Table( ST_INDEX_LOAD_AGAIN ) ); // 2004.12.15
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
			FA_HANDOFF_RES_SET( mode , SYS_ABORTED );
			EndDialog( hdlg , 0 );
			break;
		case IDOK :
			FA_HANDOFF_RES_SET( mode , SYS_SUCCESS );
			EndDialog( hdlg , 0 );
			break;
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
BOOL APIENTRY FA_HandOff_Load_Again2_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int mode;
	int Res;
	switch ( msg ) {
	case WM_INITDIALOG:
		mode = ( int ) lParam;
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Do you want to send Load/Request again???\n(Already Load Request)"*/ SCH_Get_String_Table( ST_INDEX_LOAD_AGAIN2 ) ); // 2004.12.15
		FA_HANDLER_WINDOW_POS( hdlg , mode );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
			//
			switch ( _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() ) { // 2018.12.20
			case 1 :
			case 3 :
				Res = IDYES;
				break;
			default :
				Res = MessageBox( hdlg , /*"Do you want to Abort or Cancel?"*/ SCH_Get_String_Table( ST_INDEX_LOAD_AGAIN2_CONF_ABORT ) , "Load" , MB_ICONQUESTION | MB_YESNO );
				break;
			}
			//
			if ( IDYES == Res ) {
				FA_HANDOFF_RES_SET( mode , SYS_ABORTED );
				EndDialog( hdlg , 0 );
			}
			break;
		case IDOK :
			if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) {
				Res = IDYES;
			}
			else {
				Res = MessageBox( hdlg , /*"No & Wait Complete?"*/ SCH_Get_String_Table( ST_INDEX_LOAD_AGAIN2_CONF_NO ) , "Load" , MB_ICONQUESTION | MB_YESNO );
			}
			if ( IDYES == Res ) {
				FA_HANDOFF_RES_SET( mode , SYS_SUCCESS );
				EndDialog( hdlg , 0 );
			}
			break;
		case IDYES :
			if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) {
				Res = IDYES;
			}
			else {
				Res = MessageBox( hdlg , /*"Yes & Wait Complete?"*/ SCH_Get_String_Table( ST_INDEX_LOAD_AGAIN2_CONF_YES ) , "Load" , MB_ICONQUESTION | MB_YESNO );
			}
			if ( IDYES == Res ) {
				FA_HANDOFF_RES_SET( mode , SYS_ERROR );
				EndDialog( hdlg , 0 );
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
extern int SIM_UNLOADLOAD_OPTION; // x noloop waiting noloop+waiting 2018.12.13
extern int SIM_WAITING_TAG[MAX_CASSETTE_SIDE]; // 0:wait 1:success 2:error 3:abort


//
BOOL SIM_Load_Normal_Function( int side ) {
	int c = 0;
	//
	if ( ( SIM_UNLOADLOAD_OPTION == 0 ) || ( SIM_UNLOADLOAD_OPTION == 1 ) ) return FALSE; 
	//
	SIM_WAITING_TAG[side] = 0;
	//
	while ( TRUE ) {
		//
		if ( SIM_WAITING_TAG[side] == 1 ) {
			FA_HANDOFF_RES_SET( side , SYS_SUCCESS );
			break;
		}
		if ( SIM_WAITING_TAG[side] == 2 ) {
			FA_HANDOFF_RES_SET( side , SYS_ERROR );
			break;
		}
		if ( SIM_WAITING_TAG[side] == 3 ) {
			FA_HANDOFF_RES_SET( side , SYS_ABORTED );
			break;
		}
		//
		Sleep(250);
		//
		c++;
		if ( ( c % 40 ) == 0 ) printf( "[L-CM%d]" , side + 1 );
	}
	//
	return TRUE;
	//
}
//

int FA_LOAD_HANDOFF_HANDLER( int CHECKING_SIDE ) {
	HWND hWnd;
	HANDLE hdle;
	int i , Res; // 2016.11.02
	FA_HANDOFF_RES_SET( CHECKING_SIDE , SYS_ABORTED );
	//
	if ( SIM_Load_Normal_Function( CHECKING_SIDE ) ) return FA_HANDOFF_RES_GET( CHECKING_SIDE ); // 2018.12.13
	//
	if ( FA_AGV_MODE_GET( CHECKING_SIDE ) == 1 ) {
		FA_AGV_Load_Normal_Function( CM1 + CHECKING_SIDE , FALSE );
	}
	else {
		if ( ( FA_AGV_MODE_GET( CHECKING_SIDE ) == 0 ) && ( FA_FUNCTION_INTERFACE_GET() >= 3 ) ) {
			FA_AGV_Load_Normal_Function( CM1 + CHECKING_SIDE , TRUE );
		}
		else {
			/*
			//
			// 2016.11.02
			//
			hWnd = GetTopWindow( NULL ); // 2002.10.30
			hdle = GetModuleHandle( NULL ); // 2002.10.30 // 2004.01.19 // 2004.08.10 // 2004.09.07
			//
			if      ( ( CHECKING_SIDE % 4 ) == 0 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_1_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
			else if ( ( CHECKING_SIDE % 4 ) == 1 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_2_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
			else if ( ( CHECKING_SIDE % 4 ) == 2 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_3_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
			else if ( ( CHECKING_SIDE % 4 ) == 3 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_4_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				//
			*/

			//
			// 2016.11.02
			//
			for ( i = 0 ; i < 3 ; i++ ) {
				//
				if      ( i == 0 ) {
					hWnd = GetTopWindow( NULL ); // 2002.10.30
				}
				else if ( i == 1 ) {
					hWnd = GetForegroundWindow();
				}
				else {
					hWnd = NULL;
				}
				//
				hdle = GETHINST( hWnd ); // 2002.10.30 // 2004.01.19 // 2004.08.10 // 2004.09.07 // 2016.11.02
				//
				if      ( ( CHECKING_SIDE % 4 ) == 0 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_1_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else if ( ( CHECKING_SIDE % 4 ) == 1 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_2_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else if ( ( CHECKING_SIDE % 4 ) == 2 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_3_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else if ( ( CHECKING_SIDE % 4 ) == 3 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_PAD ) , hWnd , FA_HandOff_Load_4_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else
					Res = -99;
				//
				if ( Res != -1 ) break;
				//
			}
			//
		}
	}
	return FA_HANDOFF_RES_GET( CHECKING_SIDE );
}
//=============================================================================
//=============================================================================
int FA_LOAD_HANDOFF_AGAIN_CHECK( int CHECKING_SIDE ) {
	HWND hWnd;
	FA_HANDOFF_RES_SET( CHECKING_SIDE , SYS_ABORTED );
	hWnd = GetForegroundWindow(); // GETHINST(NULL) // 2002.06.17
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_AGAIN_PAD ) , hWnd , FA_HandOff_Load_Again_Proc , (LPARAM) (int) CHECKING_SIDE ); // 2004.01.19 // 2004.08.10
	return FA_HANDOFF_RES_GET( CHECKING_SIDE );
}
//=============================================================================
//=============================================================================
int FA_LOAD_HANDOFF_AGAIN_CHECK2( int CHECKING_SIDE ) {
	HWND hWnd;
	FA_HANDOFF_RES_SET( CHECKING_SIDE , SYS_ABORTED );
	hWnd = GetForegroundWindow(); // GETHINST(NULL) // 2002.06.17
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_HANDOFF_LOAD_AGAIN2_PAD ) , hWnd , FA_HandOff_Load_Again2_Proc , (LPARAM) (int) CHECKING_SIDE ); // 2004.01.19 // 2004.08.10
	return FA_HANDOFF_RES_GET( CHECKING_SIDE );
}
//=============================================================================
//=============================================================================
BOOL USER_LOAD_CONDITION( int side ) { // 2013.07.11
	int address , c;
	//
	if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) return FALSE;
//	if ( _i_SCH_SYSTEM_CPJOB_GET( side ) != 1 ) return FALSE;
	//
//GEM_State	EQ-OFFL		ATTP-ONL	HOST-OFFL	LOCAL		REMOTE		
//GEM.Control_State		GEM_State
	//
	address = _FIND_FROM_STRING( _K_D_IO , "GEM.Control_State" );
	//
	if ( address < 0 ) return TRUE;
	//
	if ( _dREAD_DIGITAL( address , &c ) == 4 ) return TRUE;
	//
	return FALSE;
}

//=============================================================================
//=============================================================================
void FA_LOAD_CONTROLLER_TH( int CHECKING_SIDE ) {
//	BOOL force = FALSE; // 2013.06.13
	BOOL cioset; // 2013.06.13
	BOOL mrres , automap; // 2013.07.05
	//
	//----------------------------------------------------------------------
//	FA_LOAD_CONTROLLER_RUN[(CHECKING_SIDE%1000)] = 2; // 2013.07.10
	FA_LOAD_CONTROLLER_RUN[(CHECKING_SIDE%1000)] = 10 + FA_LOAD_CONTROLLER_RUN[(CHECKING_SIDE%1000)]; // 2013.07.10
	//----------------------------------------------------------------------
	//
	cioset = FA_MAIN_BUTTON_SET_CHECK( CHECKING_SIDE%1000 );
	//----------------------------------------------------------------------
	BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%1000) + CM1 , CTC_RUNNING );
	//----------------------------------------------------------------------
	if ( ( CHECKING_SIDE / 1000 ) == 1 ) {
		if ( FA_LOAD_HANDOFF_AGAIN_CHECK( CHECKING_SIDE % 1000 ) == SYS_ABORTED ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
			//
			//----------------------------------------------------------------------
			BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%1000) + CM1 , CTC_IDLE );
//			if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
			if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
			//----------------------------------------------------------------------
			FA_LOAD_CONTROLLER_RUN[(CHECKING_SIDE%1000)] = 0;
			_endthread();
			return;
		}
	}
	if ( FA_LOADREQ_SIGNAL_GET( CHECKING_SIDE % 1000 ) ) {
		switch( FA_LOAD_HANDOFF_AGAIN_CHECK2( CHECKING_SIDE % 1000 ) ) {
		case SYS_ABORTED :
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
			//
			//----------------------------------------------------------------------
			BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%1000) + CM1 , CTC_IDLE );
//			if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
			if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
			//----------------------------------------------------------------------
			FA_LOAD_CONTROLLER_RUN[(CHECKING_SIDE%1000)] = 0;
			_endthread();
			return;
			break;
		case SYS_SUCCESS :
			FA_LOADING_SIGNAL_SET( CHECKING_SIDE % 1000 , FALSE );
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE % 1000 , 0 );
			FA_LOADREQ_SIGNAL_SET( CHECKING_SIDE % 1000 , TRUE );
			break;
		case SYS_ERROR :
			FA_LOADING_SIGNAL_SET( CHECKING_SIDE % 1000 , FALSE );
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE % 1000 , 0 );
			FA_ACCEPT_MESSAGE( CHECKING_SIDE % 1000 , FA_LOAD_REQUEST , 0 );
			FA_LOADREQ_SIGNAL_SET( CHECKING_SIDE % 1000 , TRUE );
			break;
		}
	}
	else {
		FA_LOADING_SIGNAL_SET( CHECKING_SIDE % 1000 , FALSE );
		FA_MAPPING_SIGNAL_SET( CHECKING_SIDE % 1000 , 0 );
		FA_ACCEPT_MESSAGE( CHECKING_SIDE % 1000 , FA_LOAD_REQUEST , 0 );
		FA_LOADREQ_SIGNAL_SET( CHECKING_SIDE % 1000 , TRUE );
	}

	BUTTON_SET_FLOW_STATUS( CHECKING_SIDE % 1000 , _MS_1_LOADING );

	switch( FA_LOAD_HANDOFF_HANDLER( CHECKING_SIDE % 1000 ) ) {
	case SYS_SUCCESS :
		//
		mrres = SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_2_LOADED ); // 2013.07.05
		//
		automap = FALSE;
		//
		switch( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( CHECKING_SIDE % 1000 ) ) { // 2013.07.05
		case 3 :
			if ( !mrres ) {
				if ( USER_LOAD_CONDITION( CHECKING_SIDE % 1000 ) ) automap = TRUE; // 2013.07.11
			}
			break;
		case 2 :
			if ( mrres ) {
				if ( USER_LOAD_CONDITION( CHECKING_SIDE % 1000 ) ) automap = TRUE; // 2013.07.11
			}
			break;
		case 1 :
			automap = TRUE;
			break;
		}
		//
		BUTTON_SET_FLOW_STATUS( CHECKING_SIDE % 1000 , _MS_2_LOADED );
		FA_RESULT_MESSAGE( CHECKING_SIDE % 1000 , FA_LOAD_COMPLETE , 0 );
		FA_LOADING_SIGNAL_SET( CHECKING_SIDE % 1000 , TRUE );
		FA_LOADREQ_SIGNAL_SET( CHECKING_SIDE % 1000 , FALSE );
		//----------------------------------------------------------------------
		BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%1000) + CM1 , CTC_IDLE );
//		if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
		if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
		//----------------------------------------------------------------------
		//----- Auto Mapping
//		if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( CHECKING_SIDE % 1000 ) != 0 ) { // 2013.07.05
		if ( automap ) { // 2013.07.05
			//
			FA_MAPPING_CANCEL_SET( CHECKING_SIDE%1000 , 0 ); // 2008.03.19
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%1000 , 2 );
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_4_MAPPING ); // 2011.09.07
			//
			if ( _beginthread( (void *) Scheduler_Carrier_Mapping , 0 , (void *) ( (CHECKING_SIDE % 1000) + 100 ) ) == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Carrier_Mapping(1) %d\n" , (CHECKING_SIDE % 1000) + 100 );
				//
				FA_ACCEPT_MESSAGE( CHECKING_SIDE%1000 , FA_MAPPING , 0 );
				FA_REJECT_MESSAGE( CHECKING_SIDE%1000 , FA_MAPPING , 100 , "" ); // 2007.01.26
				FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%1000 , 0 );
				BUTTON_SET_FLOW_STATUS( CHECKING_SIDE%1000 , _MS_6_MAPFAIL );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		break;
	case SYS_ERROR :
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
		//
		BUTTON_SET_FLOW_STATUS( CHECKING_SIDE % 1000 , _MS_2_LOADED );
		FA_RESULT_MESSAGE( CHECKING_SIDE % 1000 , FA_LOAD_COMPLETE2 , 0 );
		FA_LOADING_SIGNAL_SET( CHECKING_SIDE % 1000 , TRUE );
		FA_LOADREQ_SIGNAL_SET( CHECKING_SIDE % 1000 , FALSE );
		//----------------------------------------------------------------------
		BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%1000) + CM1 , CTC_IDLE );
//		if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
		if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
		//----------------------------------------------------------------------
		//----- Auto Mapping
//		if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( CHECKING_SIDE % 1000 ) != 0 ) { // 2013.07.05
		if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( CHECKING_SIDE % 1000 ) == 1 ) { // 2013.07.05
			FA_MAPPING_CANCEL_SET( CHECKING_SIDE%1000 , 0 ); // 2008.03.19
			FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%1000 , 2 );
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_4_MAPPING ); // 2011.09.07
			//
			if ( _beginthread( (void *) Scheduler_Carrier_Mapping , 0 , (void *) ( (CHECKING_SIDE % 1000) + 100 ) ) == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_6_MAPFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Carrier_Mapping(2) %d\n" , (CHECKING_SIDE % 1000) + 100 );
				//
				FA_ACCEPT_MESSAGE( CHECKING_SIDE%1000 , FA_MAPPING , 0 );
				FA_REJECT_MESSAGE( CHECKING_SIDE%1000 , FA_MAPPING , 101 , "" ); // 2007.01.26
				FA_MAPPING_SIGNAL_SET( CHECKING_SIDE%1000 , 0 );
				BUTTON_SET_FLOW_STATUS( CHECKING_SIDE%1000 , _MS_6_MAPFAIL );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		break;
	default :
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( (CHECKING_SIDE%1000) + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
		//
		BUTTON_SET_FLOW_STATUS( CHECKING_SIDE % 1000 , _MS_3_LOADFAIL );
		FA_REJECT_MESSAGE( CHECKING_SIDE % 1000 , FA_LOAD_REQUEST , 0 , "" );
		FA_LOADREQ_SIGNAL_SET( CHECKING_SIDE % 1000 , FALSE );
		//----------------------------------------------------------------------
		BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%1000) + CM1 , CTC_IDLE );
//		if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
		if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%1000) , CTC_IDLE ); // 2013.06.13
		//----------------------------------------------------------------------
		break;
	}
	FA_LOAD_CONTROLLER_RUN[(CHECKING_SIDE%1000)] = 0;
	_endthread();
}
//=============================================================================
//=============================================================================
