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
#include "MR_Data.h"
#include "resource.h"
#include "File_ReadInit.h"


//================================================================================

void Scheduler_Run_Unload_FlagReset( int side , BOOL manual , BOOL cancel ); // 2012.04.03

//================================================================================

int FA_UNLOAD_CONTROLLER_RUN[ MAX_CASSETTE_SIDE ];

BOOL FA_UNLOADING_SIGNAL[ MAX_CASSETTE_SIDE ];

//=============================================================================
void FA_UNLOADING_SIGNAL_SET( int Side , BOOL data ) { FA_UNLOADING_SIGNAL[ Side ] = data; }
BOOL FA_UNLOADING_SIGNAL_GET( int Side ) { return FA_UNLOADING_SIGNAL[ Side ]; }
//=============================================================================
//=============================================================================
void FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( int Side , int data ) { FA_UNLOAD_CONTROLLER_RUN[ Side ] = data; }
int  FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( int Side ) { return FA_UNLOAD_CONTROLLER_RUN[ Side ]; }
//=============================================================================
//=============================================================================
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_HANDLER_UNLOAD_INIT() {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		FA_UNLOAD_CONTROLLER_RUN[ i ] = 0;
		//
		FA_UNLOADING_SIGNAL[ i ] = FALSE;
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void FA_HandOff_Unload_Proc_WM_INITDIALOG( HWND hdlg , int Cassette , int *mode , char *CassetteString ) {
	//--------------------------------------------------
	sprintf( CassetteString , "Cassette%d/PORT%d" , Cassette - CM1 + 1 , Cassette - CM1 + 1 );
	//--------------------------------------------------
	FA_HANDLER_WINDOW_POS( hdlg , Cassette - CM1 );
	FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MODULE , CassetteString );
	if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
		*mode = 0;
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Running Unload Enter Function"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_ENTER ) );
		EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_ENTER" );
		KWIN_Item_Disable( hdlg , IDCANCEL );
		//--------------------------------------------------------------------------
		switch ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) {
		case 3 :	KWIN_Item_Hide( hdlg , IDOK );	break;
		default :	KWIN_Item_Disable( hdlg , IDOK );	break;
		}
		//--------------------------------------------------------------------------
		switch ( _i_SCH_PRM_GET_AUTO_HANDOFF( Cassette ) ) {
		case 0 : case 1 : case 4 : case 5 : KWIN_Item_Disable( hdlg , IDYES ); break;
		case 2 : case 3 : KWIN_Item_Hide( hdlg , IDYES ); break;
		}
	}
	else {
		*mode = 1;
		//
		if ( ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) / 4 ) == 1 ) {
			KWIN_Item_Disable( hdlg , IDCANCEL );
		}
		else {
			KWIN_Item_Enable( hdlg , IDCANCEL );
		}
		//
		switch ( _i_SCH_PRM_GET_AUTO_HANDOFF( Cassette ) ) {
		case 0 : case 1 : case 4 : case 5 :
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If you need to run for Unload Function ,\nPress [Unload] Button.\nIf Unload Operation has been done ,\nPress [Unload Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_UNLOAD ) );
			KWIN_Item_Enable( hdlg , IDYES );
			//
			switch( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) {
			case 2 :	KWIN_Item_Enable( hdlg , IDOK );	break;
			case 3 :	KWIN_Item_Hide( hdlg , IDOK );	break;
			default :	KWIN_Item_Disable( hdlg , IDOK );	break;
			}
			break;
		case 2 : case 3 :
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If Unload Operation has been done ,\nPress [Unload Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_COMPLETE ) );
			KWIN_Item_Hide( hdlg , IDYES );
			KWIN_Item_Enable( hdlg , IDOK );
			break;
		}
	}
}

BOOL FA_HandOff_Unload_Proc_WM_TIMER( HWND hdlg , int Cassette , int *mode ) {
	if ( *mode == 0 ) {
		switch( EQUIP_READ_FUNCTION( Cassette ) ) {
		case SYS_SUCCESS :
			*mode = 1;
			//
			if ( ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) / 4 ) == 1 ) {
				KWIN_Item_Disable( hdlg , IDCANCEL );
			}
			else {
				KWIN_Item_Enable( hdlg , IDCANCEL );
			}
			//
			switch ( _i_SCH_PRM_GET_AUTO_HANDOFF( Cassette ) ) {
			case 0 : case 1 : case 4 : case 5 :
				KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If you need to run for Unload Function ,\nPress [Unload] Button.\nIf Unload Operation has been done ,\nPress [Unload Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_UNLOAD ) );
				KWIN_Item_Enable( hdlg , IDYES );
				//
				switch( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) {
				case 2 :	KWIN_Item_Enable( hdlg , IDOK );	break;
				case 3 :	KWIN_Item_Hide( hdlg , IDOK );	break;
				default :	KWIN_Item_Disable( hdlg , IDOK );	break;
				}
				break;
			case 2 : case 3 :
				KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If Unload Operation has been done ,\nPress [Unload Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_COMPLETE ) );
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
			if ( ( ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) == 0 ) || ( ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) == 3 ) ) { // 2004.12.15
				if ( *mode == 99 ) break;
				*mode = 99;
				FA_HANDOFF_RES_SET( Cassette-CM1 , SYS_SUCCESS );
				if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
					EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_LEAVE" );
				}
				return FALSE;
			}
			else {
				KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If Unload Operation has been done ,\nPress [Unload Complete] Button !!"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_COMPLETE ) );
				KWIN_Item_Enable( hdlg , IDOK );
				*mode = 3;
			}
			break;
		default :
			if ( *mode == 99 ) break;
			*mode = 99;
			FA_HANDOFF_RES_SET( Cassette-CM1 , SYS_ABORTED );
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_LEAVE" );
			}
			return FALSE;
			break;
		}
	}
	return TRUE;
}

BOOL FA_HandOff_Unload_Proc_WM_COMMAND( HWND hdlg , int wParam , int Cassette , int *mode , char *CassetteString ) {
	int Res;
	switch( wParam ) {
	case IDOK :
		if ( *mode == 99 ) return TRUE;
		if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) {
			Res = IDYES;
		}
		else {
			Res = MessageBox( hdlg , /*"Do you want to Set Unload Complete?"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_CONFIRM_COMPLETE ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
		}
		if ( IDYES == Res ) {
			FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_SUCCESS );
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_LEAVE" );
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
			Res = MessageBox( hdlg , /*"Do you want to Abort or Cancel?"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_CONFIRM_ABORT ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
			break;
		}
		//
		if ( IDYES == Res ) {
			FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
			if ( *mode == 2 ) EQUIP_RUN_FUNCTION_ABORT( Cassette );
			*mode = 99;
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_LEAVE" );
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
			Res = MessageBox( hdlg , /*"Do you want to Start Unload Function?"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_CONFIRM_START ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
		}
		if ( IDYES == Res ) {
			*mode = 2;
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Running Unload Function\nWait Unload Function Success\nIf you want to stop(abort) ,\nPress [Abort] Button"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_RUNNING ) );
			//
			switch( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( Cassette - CM1 ) % 10 ) { // 2013.07.10
			case 2 :
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT REDO" );
				break;
			default :
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT" );
				break;
			}
			//
			KWIN_Item_Enable( hdlg , IDCANCEL );
			KWIN_Item_Disable( hdlg , IDYES );
			if ( ( _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE(Cassette-CM1) % 4 ) == 3 ) { // 2004.12.15
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
#define MAKE_FA_HANDOFF_UNLOAD( ZZZ ) BOOL APIENTRY FA_HandOff_Unload_##ZZZ##_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) { \
	static int mode , id , Cassette; \
	static char CassetteString[24]; \
	switch ( msg ) { \
	case WM_INITDIALOG: \
		Cassette = (int) lParam; \
		FA_HandOff_Unload_Proc_WM_INITDIALOG( hdlg , Cassette , &mode , CassetteString ); \
		id = SetTimer( hdlg , 2010 + Cassette - CM1 , 200 , NULL ); \
		return TRUE; \
	case WM_TIMER : \
		if ( !FA_HandOff_Unload_Proc_WM_TIMER( hdlg , Cassette , &mode ) ) EndDialog( hdlg , 0 ); \
		return TRUE; \
	case WM_COMMAND : \
		if ( !FA_HandOff_Unload_Proc_WM_COMMAND( hdlg , wParam , Cassette , &mode , CassetteString ) ) EndDialog( hdlg , 0 ); \
		return TRUE; \
	case WM_DESTROY : \
		KillTimer( hdlg , id ); \
		return TRUE; \
	} \
	return FALSE; \
}

MAKE_FA_HANDOFF_UNLOAD( 1 )
MAKE_FA_HANDOFF_UNLOAD( 2 )
MAKE_FA_HANDOFF_UNLOAD( 3 )
MAKE_FA_HANDOFF_UNLOAD( 4 )

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY FA_HandOff_Unload_Again_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int mode;
	switch ( msg ) {
	case WM_INITDIALOG:
		mode = ( int ) lParam;
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Do you want to unload operation ???\n(Not Properly Situation for Unload)"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_AGAIN ) ); // 2004.12.15
		FA_HANDLER_WINDOW_POS( hdlg , mode );
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

void FA_AGV_Unload_Normal_Function( int Cassette , BOOL Disable ) {
	char AGVString[MAX_CASSETTE_SIDE][24];
	FA_HANDOFF_RES_SET( Cassette - CM1 , SYS_ABORTED );
	if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
		FA_HANDLER_ENTER_CR();
		if ( EQUIP_RUN_FUNCTION( Cassette , "HAND_OFF_OUT_ENTER" ) == SYS_ABORTED ) {
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
			switch( FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( Cassette - CM1 ) % 10 ) { // 2013.07.10
			case 2 :
				//
				if ( Disable ) {
					sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d DISABLE REDO" , Cassette - CM1 + 1 );
				}
				else {
					switch( FA_FUNCTION_INTERFACE_GET() ) {
					case 0 : case 3 :	sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d ENABLE REDO"  , Cassette - CM1 + 1 );	break;
					case 1 : case 4 :	sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d ENABLE2 REDO" , Cassette - CM1 + 1 );	break;
					case 2 : case 5 :	sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d ENABLE3 REDO" , Cassette - CM1 + 1 );	break;
					}
				}
				//
				break;
			default :
				//
				if ( Disable ) {
					sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d DISABLE" , Cassette - CM1 + 1 );
				}
				else {
					switch( FA_FUNCTION_INTERFACE_GET() ) {
					case 0 : case 3 :	sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d ENABLE"  , Cassette - CM1 + 1 );	break;
					case 1 : case 4 :	sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d ENABLE2" , Cassette - CM1 + 1 );	break;
					case 2 : case 5 :	sprintf( AGVString[ Cassette - CM1 ] , "UNLOAD CM%d ENABLE3" , Cassette - CM1 + 1 );	break;
					}
				}
				//
				break;
			}
			//
			_dRUN_SET_FUNCTION( FA_AGV_FUNCTION_GET( Cassette - CM1 ) , AGVString[ Cassette - CM1 ] );
			//
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
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_LEAVE" );
			}
			FA_HANDLER_LEAVE_CR();
			break;
		}
		FA_HANDLER_LEAVE_CR();
		Sleep(100);
	}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//=============================================================================
//=============================================================================
int FA_UNLOAD_HANDOFF_AGAIN_CHECK( int CHECKING_SIDE ) {
	HWND hWnd;
	FA_HANDOFF_RES_SET( CHECKING_SIDE , SYS_ABORTED );
	hWnd = GetForegroundWindow(); // GETHINST(NULL) // 2002.06.17
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_AGAIN_PAD ) , hWnd , FA_HandOff_Unload_Again_Proc , (LPARAM) (int) CHECKING_SIDE ); // 2004.01.19 // 2004.08.10
	return FA_HANDOFF_RES_GET( CHECKING_SIDE );
}
//=============================================================================
//=============================================================================

int SIM_UNLOADLOAD_OPTION = 0; // x noloop waiting noloop+waiting 2018.12.13
int SIM_WAITING_TAG[MAX_CASSETTE_SIDE]; // 0:wait 1:success 2:error 3:abort

//
BOOL SIM_UnLoad_Normal_Function( int side ) {
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
		if ( ( c % 40 ) == 0 ) printf( "[U-CM%d]" , side + 1 );
	}
	//
	return TRUE;
	//
}
//

int FA_UNLOAD_HANDOFF_HANDLER( int CHECKING_SIDE ) {
	HWND hWnd;
	HANDLE hdle;
	int i , Res; // 2016.11.02
	//
	FA_HANDOFF_RES_SET( CHECKING_SIDE , SYS_ABORTED );
	//
	if ( SIM_UnLoad_Normal_Function( CHECKING_SIDE ) ) return FA_HANDOFF_RES_GET( CHECKING_SIDE ); // 2018.12.13
	//
	if ( FA_AGV_MODE_GET( CHECKING_SIDE ) == 1 ) {
		FA_AGV_Unload_Normal_Function( CM1 + CHECKING_SIDE , FALSE );
	}
	else {
		if ( ( FA_AGV_MODE_GET( CHECKING_SIDE ) == 0 ) && ( FA_FUNCTION_INTERFACE_GET() >= 3 ) ) {
			FA_AGV_Unload_Normal_Function( CM1 + CHECKING_SIDE , TRUE );
		}
		else {
			//
			/*
			//
			// 2016.11.02
			//
			hWnd = GetTopWindow( NULL ); // 2002.10.30
			hdle = GetModuleHandle( NULL ); // 2002.10.30 // 2004.01.19 // 2004.08.10 // 2004.09.07
			//
			if      ( ( CHECKING_SIDE % 4 ) == 0 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_1_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
			else if ( ( CHECKING_SIDE % 4 ) == 1 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_2_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
			else if ( ( CHECKING_SIDE % 4 ) == 2 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_3_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
			else if ( ( CHECKING_SIDE % 4 ) == 3 )
				GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_4_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
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
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_1_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else if ( ( CHECKING_SIDE % 4 ) == 1 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_2_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else if ( ( CHECKING_SIDE % 4 ) == 2 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_3_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else if ( ( CHECKING_SIDE % 4 ) == 3 )
					Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_HANDOFF_UNLOAD_PAD ) , hWnd , FA_HandOff_Unload_4_Proc , (LPARAM) (int) CM1 + CHECKING_SIDE );
				else
					Res = -99;
				//
				if ( Res != -1 ) break;
				//
			}
			//
			//
		}
	}
	return FA_HANDOFF_RES_GET( CHECKING_SIDE );
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

void FA_UNLOAD_CONTROLLER_TH( int CHECKING_SIDE ) {
//	BOOL force = FALSE; // 2013.06.13
	BOOL cioset; // 2013.06.13
	int i = 0 , sts;
	//----------------------------------------------------------------------
//	FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] = 2; // 2013.07.10
	FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] = 10 + FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)]; // 2013.07.10
	//----------------------------------------------------------------------
	// 2008.03.18
	//----------------------------------------------------------------------
	while ( TRUE ) {
		if ( FA_MAPPING_SIGNAL_GET( (CHECKING_SIDE%10) ) < 3 ) break;
		Sleep(1000);
	}
	//----------------------------------------------------------------------
	cioset = FA_MAIN_BUTTON_SET_CHECK( CHECKING_SIDE%10 );
	//
	//----------------------------------------------------------------------
	if ( ((CHECKING_SIDE/100000)%10) >= 1 ) Scheduler_Run_Unload_FlagReset( (CHECKING_SIDE%10) , FALSE , ( ((CHECKING_SIDE/10000)%10) >= 1 ) ); // 2012.04.03 // 2013.05.27
	//
	//----------------------------------------------------------------------
	BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%10) + CM1 , CTC_RUNNING );
	//----------------------------------------------------------------------
	if ( ((CHECKING_SIDE/10)%10) >= 1 ) {
		if ( FA_UNLOAD_HANDOFF_AGAIN_CHECK( CHECKING_SIDE%10 ) == SYS_ABORTED ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE%10 ) + CM1 , 12 ); // 2011.09.07
			//
			//----------------------------------------------------------------------
			BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%10) + CM1 , CTC_IDLE );
//			if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
			if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
			//----------------------------------------------------------------------
			FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] = 0;
			_endthread();
			return;
		}
		FA_ACCEPT_MESSAGE( CHECKING_SIDE%10 , FA_CANCEL , 0 );
		FA_RESULT_MESSAGE( CHECKING_SIDE%10 , FA_CANCELED , 0 );
	}
	//----------------------------------------------------------------------
	if ( ((CHECKING_SIDE/10000)%10) >= 1 ) {
		BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_18_UNLOADING_C );
	}
	else {
		BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_19_UNLOADING );
	}
	//----------------------------------------------------------------------
	if ( ((CHECKING_SIDE/100)%10) >= 1 ) {
		if ( EQUIP_CARRIER_HANDOFF_OUT( (CHECKING_SIDE%10) + CM1 ) == SYS_ABORTED ) {
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE%10 ) + CM1 , 12 ); // 2011.09.07
			//
			//----------------------------------------------------------------------
			BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%10) + CM1 , CTC_IDLE );
//			if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
			if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
			//----------------------------------------------------------------------
			BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_22_UNLOADFAIL );
			FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] = 0;
			_endthread();
			return;
		}
	}
	//
	FA_LOADREQ_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE ); // 2016.11.02
	//
	FA_UNLOADING_SIGNAL_SET( (CHECKING_SIDE%10) , TRUE );
	FA_ACCEPT_MESSAGE( (CHECKING_SIDE%10) , FA_UNLOAD_REQUEST , 0 );
	//
	switch( FA_UNLOAD_HANDOFF_HANDLER( CHECKING_SIDE%10 ) ) {
	case SYS_SUCCESS :
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE%10 ) + CM1 , 11 ); // 2011.09.07
		//
		sts = ( SIM_UNLOADLOAD_OPTION == 1 ) || ( SIM_UNLOADLOAD_OPTION == 3 ); // 2018.12.13
		//
		if ( sts || ( ((CHECKING_SIDE/1000)%10) < 1 ) ) {
			if ( ((CHECKING_SIDE/10000)%10) >= 1 ) {
				BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_20_UNLOADED_C );
			}
			else {
				BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_21_UNLOADED );
			}
		}
		FA_RESULT_MESSAGE( (CHECKING_SIDE%10) , FA_UNLOAD_COMPLETE , 0 );
		FA_UNLOADING_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		FA_LOADING_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		FA_MAPPING_SIGNAL_SET( (CHECKING_SIDE%10) , 0 );
		FA_LOADREQ_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		//----------------------------------------------------------------------
		BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%10) + CM1 , CTC_IDLE );
//		if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
		if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
		//----------------------------------------------------------------------
		if ( !sts && ( ((CHECKING_SIDE/1000)%10) >= 1 ) ) {
			//
//			FA_LOAD_CONTROLLER_RUN_STATUS_SET( CHECKING_SIDE % 10 , 1 ); // 2013.07.10
			FA_LOAD_CONTROLLER_RUN_STATUS_SET( CHECKING_SIDE % 10 , FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] % 10 ); // 2013.07.10
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE % 10 ) + CM1 , _MS_1_LOADING ); // 2011.09.07
			//
			sts = _beginthread( (void *) FA_LOAD_CONTROLLER_TH , 0 , (void *) (CHECKING_SIDE%10) );
			//
			if ( sts == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE % 10 ) + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL FA_LOAD_CONTROLLER_TH(1) %d\n" , CHECKING_SIDE % 10 );
				//
				BUTTON_SET_FLOW_STATUS( CHECKING_SIDE % 10 , _MS_3_LOADFAIL );
				FA_REJECT_MESSAGE( CHECKING_SIDE % 10 , FA_LOAD_REQUEST , 0 , "" );
				//
				FA_LOAD_CONTROLLER_RUN_STATUS_SET( CHECKING_SIDE % 10 , 0 );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		break;
	case SYS_ERROR :
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE%10 ) + CM1 , 11 ); // 2011.09.07
		//
//		if ( ((CHECKING_SIDE/1000)%10) < 1 ) { // 2018.12.13
			if ( ((CHECKING_SIDE/10000)%10) >= 1 ) {
				BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_20_UNLOADED_C );
			}
			else {
				BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_21_UNLOADED );
			}
//		} // 2018.12.13
		//
		FA_RESULT_MESSAGE( (CHECKING_SIDE%10) , FA_UNLOAD_COMPLETE2 , 0 );
		FA_UNLOADING_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		FA_LOADING_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		FA_MAPPING_SIGNAL_SET( (CHECKING_SIDE%10) , 0 );
		FA_LOADREQ_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		//----------------------------------------------------------------------
		BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%10) + CM1 , CTC_IDLE );
//		if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
		if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
		//----------------------------------------------------------------------
		/*
		//
		// 2018.12.13
		//
		if ( ((CHECKING_SIDE/1000)%10) >= 1 ) {
			//
//			FA_LOAD_CONTROLLER_RUN_STATUS_SET( CHECKING_SIDE % 10 , 1 ); // 2013.07.10
			FA_LOAD_CONTROLLER_RUN_STATUS_SET( CHECKING_SIDE % 10 , FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] % 10 ); // 2013.07.10
			//
			SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE % 10 ) + CM1 , _MS_1_LOADING ); // 2011.09.07
			//
			sts = _beginthread( (void *) FA_LOAD_CONTROLLER_TH , 0 , (void *) (CHECKING_SIDE%10) );
			//
			if ( sts == -1 ) {
				//
				SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE % 10 ) + CM1 , _MS_3_LOADFAIL ); // 2011.09.07
				//
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL FA_LOAD_CONTROLLER_TH(2) %d\n" , CHECKING_SIDE % 10 );
				//
				BUTTON_SET_FLOW_STATUS( CHECKING_SIDE % 10 , _MS_3_LOADFAIL );
				FA_REJECT_MESSAGE( CHECKING_SIDE % 10 , FA_LOAD_REQUEST , 0 , "" );
				//
				FA_LOAD_CONTROLLER_RUN_STATUS_SET( CHECKING_SIDE % 10 , 0 );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
		//
		*/
		//
		break;
	default :
		//
		SCHMRDATA_Data_Setting_for_LoadUnload( ( CHECKING_SIDE%10 ) + CM1 , 12 ); // 2011.09.07
		//
		BUTTON_SET_FLOW_STATUS( (CHECKING_SIDE%10) , _MS_22_UNLOADFAIL );
		FA_REJECT_MESSAGE( (CHECKING_SIDE%10) , FA_UNLOAD_REQUEST , 0 , "" );
		FA_UNLOADING_SIGNAL_SET( (CHECKING_SIDE%10) , FALSE );
		//----------------------------------------------------------------------
		BUTTON_SET_HANDOFF_CONTROL( (CHECKING_SIDE%10) + CM1 , CTC_IDLE );
//		if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
		if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (CHECKING_SIDE%10) , CTC_IDLE ); // 2013.06.13
		//----------------------------------------------------------------------
		break;
	}
	//
//	if ( ((CHECKING_SIDE/100000)%10) >= 1 ) Scheduler_Run_Unload_FlagReset( (CHECKING_SIDE%10) , FALSE , ( ((CHECKING_SIDE/10000)%10) >= 1 ) ); // 2012.04.03 // 2013.05.27
	//
	FA_UNLOAD_CONTROLLER_RUN[(CHECKING_SIDE%10)] = 0;
	_endthread();
}
//
//=============================================================================
//=============================================================================
void FA_UNLOAD_CONTROLLER( int CHECKING_SIDE , BOOL Loop , BOOL Manual , BOOL CancelCheck , BOOL CancelMsg , BOOL flagcheck ) {
	int Index , sts;
	//
	Index = CHECKING_SIDE;
	//
	if ( flagcheck )	Index = Index + 100000; // 2012.04.03
	if ( CancelMsg )	Index = Index + 10000;
	if ( Loop )			Index = Index + 1000;
	if ( Manual )		Index = Index + 100;
	if ( CancelCheck )	Index = Index + 10;
	//
	sts = _beginthread( (void *) FA_UNLOAD_CONTROLLER_TH , 0 , (void *) ( Index ) );
	//
	if ( sts == -1 ) {
		//----------------------------------------------------------------------------------------------------------------
		// 2004.08.18
		//----------------------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "THREAD FAIL FA_UNLOAD_CONTROLLER_TH(1) %d\n" , Index );
		//
		FA_UNLOAD_CONTROLLER_RUN[CHECKING_SIDE] = 0; // 2012.07.10
		//
		FA_ACCEPT_MESSAGE( CHECKING_SIDE , FA_UNLOAD_REQUEST , 0 );
		BUTTON_SET_FLOW_STATUS( CHECKING_SIDE , _MS_22_UNLOADFAIL );
		FA_REJECT_MESSAGE( CHECKING_SIDE , FA_UNLOAD_REQUEST , 0 , "" );
		//----------------------------------------------------------------------------------------------------------------
	}
}
//=============================================================================
//=============================================================================
