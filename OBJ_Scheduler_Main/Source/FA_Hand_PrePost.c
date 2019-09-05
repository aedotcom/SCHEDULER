#include "default.h"

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <io.h>
#include <memory.h>
#include <winbase.h>
//================================================================================
#include "EQ_Enum.h"

#include "IO_Part_data.h"
#include "resource.h"
//================================================================================

HWND    FA_PrePost_Msg_Handle = NULL;
int		FA_PrePost_Msg_Mode = 0;

//=============================================================================
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL APIENTRY FA_PrePost_Message_Display_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int id , Count , Ce , ModeStart;
	char Buffer[256];
	switch ( msg ) {
	case WM_INITDIALOG:
		//--------------------------------------------------
		FA_PrePost_Msg_Handle = hdlg;
		//--------------------------------------------------
		MoveCenterWindow( hdlg );
		if ( FA_PrePost_Msg_Mode % 1000 )	ModeStart = TRUE;
		else								ModeStart = FALSE;
		if ( FA_PrePost_Msg_Mode / 1000 )	Ce = TRUE;
		else                                Ce = FALSE;
		if ( ModeStart ) {
			SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) -1 );
		}
		else {
			SendMessage( hdlg , WM_COMMAND , IDOK , (LPARAM)  -1 );
		}
		id = SetTimer( hdlg , 1030 , 1000 , NULL );
		Count = 0;
		return TRUE;

	case WM_TIMER :
		if ( GetForegroundWindow() != hdlg ) SetForegroundWindow( hdlg ); // 2002.05.17
		if ( Ce ) {
			Count++;
			if ( Count > 30 ) {
				FA_PrePost_Msg_Handle = NULL;
				EndDialog( hdlg , 0 );
				return TRUE;
			}
		} 
		else {
			Count = 0;
		}
		//------------------------------------------------------------
		if ( ModeStart ) {
			SendMessage( hdlg , WM_COMMAND , IDYES , (LPARAM) -1 );
		}
		else {
			SendMessage( hdlg , WM_COMMAND , IDOK , (LPARAM) -1 );
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDYES : // Start
			if ( (int) lParam >= 0 ) {
				Count = 0;
				ModeStart = TRUE;
				if ( (BOOL) lParam ) Ce = TRUE;
				else                 Ce = FALSE;
			}
			SetWindowText( hdlg , "Processing Lot Information Screen" );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MODULE , "Processing Lot Information Screen" );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE9 , "PPID" );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE10 , "LOTID" );
			_i_SCH_IO_GET_RECIPE_FILE( 0 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , Buffer );
			_i_SCH_IO_GET_RECIPE_FILE( 1 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE2 , Buffer );
			_i_SCH_IO_GET_RECIPE_FILE( 2 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE3 , Buffer );
			_i_SCH_IO_GET_RECIPE_FILE( 3 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE4 , Buffer );
			IO_GET_JID_NAME( 0 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE21 , Buffer );
			IO_GET_JID_NAME( 1 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE22 , Buffer );
			IO_GET_JID_NAME( 2 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE23 , Buffer );
			IO_GET_JID_NAME( 3 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE24 , Buffer );
			if ( MAX_CASSETTE_SIDE >= 6 ) {
				_i_SCH_IO_GET_RECIPE_FILE( 4 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE5 , Buffer );
				_i_SCH_IO_GET_RECIPE_FILE( 5 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE6 , Buffer );
				IO_GET_JID_NAME( 4 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE25 , Buffer );
				IO_GET_JID_NAME( 5 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE26 , Buffer );
				if ( MAX_CASSETTE_SIDE >= 8 ) {
					_i_SCH_IO_GET_RECIPE_FILE( 6 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE7 , Buffer );
					_i_SCH_IO_GET_RECIPE_FILE( 7 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE8 , Buffer );
					IO_GET_JID_NAME( 6 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE27 , Buffer );
					IO_GET_JID_NAME( 7 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE28 , Buffer );
				}
			}
			ShowWindow( hdlg , SW_SHOW );
			return TRUE;
		case IDOK : // End
			if ( (int) lParam >= 0 ) {
				Count = 0;
				ModeStart = FALSE;
				if ( (BOOL) lParam ) Ce = TRUE;
				else                 Ce = FALSE;
			}
			SetWindowText( hdlg , "Process End Lot Information Screen" );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MODULE , "Process End Lot Information Screen" );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE9 , "LOTID" );
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE10 , "NEXT-STEP" );
			IO_GET_JID_NAME( 0 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , Buffer );
			IO_GET_JID_NAME( 1 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE2 , Buffer );
			IO_GET_JID_NAME( 2 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE3 , Buffer );
			IO_GET_JID_NAME( 3 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE4 , Buffer );
			IO_GET_NID_NAME_FROM_READ( 0 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE21 , Buffer );
			IO_GET_NID_NAME_FROM_READ( 1 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE22 , Buffer );
			IO_GET_NID_NAME_FROM_READ( 2 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE23 , Buffer );
			IO_GET_NID_NAME_FROM_READ( 3 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE24 , Buffer );
			if ( MAX_CASSETTE_SIDE >= 6 ) {
				IO_GET_JID_NAME( 4 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE5 , Buffer );
				IO_GET_JID_NAME( 5 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE6 , Buffer );
				IO_GET_NID_NAME_FROM_READ( 4 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE25 , Buffer );
				IO_GET_NID_NAME_FROM_READ( 5 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE26 , Buffer );
				if ( MAX_CASSETTE_SIDE >= 8 ) {
					IO_GET_JID_NAME( 6 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE7 , Buffer );
					IO_GET_JID_NAME( 7 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE8 , Buffer );
					IO_GET_NID_NAME_FROM_READ( 6 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE27 , Buffer );
					IO_GET_NID_NAME_FROM_READ( 7 , Buffer );	KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE28 , Buffer );
				}
			}
			ShowWindow( hdlg , SW_SHOW );
			return TRUE;
		case IDCANCEL :
		case IDCLOSE :
			FA_PrePost_Msg_Handle = NULL;
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;
	case WM_DESTROY :
		KillTimer( hdlg , id );
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//=============================================================================
void FA_STARTEND_STATUS_CONTROLLER_TH() {
	HWND hWnd;
	hWnd = GetForegroundWindow();
	if      ( MAX_CASSETTE_SIDE >= 8 ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_PREPOST_INFO_PAD8 ) , hWnd , FA_PrePost_Message_Display_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	else if ( MAX_CASSETTE_SIDE >= 6 ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_PREPOST_INFO_PAD6 ) , hWnd , FA_PrePost_Message_Display_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	else {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_PREPOST_INFO_PAD ) , hWnd , FA_PrePost_Message_Display_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	}
	_endthread();
}
void FA_STARTEND_STATUS_CONTROLLER( BOOL Start , BOOL TimeOut ) {
	if ( FA_PrePost_Msg_Handle == NULL ) {
		if ( TimeOut )	FA_PrePost_Msg_Mode = Start + 1000;
		else			FA_PrePost_Msg_Mode = Start;
		if ( _beginthread( (void *) FA_STARTEND_STATUS_CONTROLLER_TH , 0 , 0 ) == -1 ) {
			//----------------------------------------------------------------------------------------------------------------
			_IO_CIM_PRINTF( "THREAD FAIL FA_STARTEND_STATUS_CONTROLLER_TH(1)\n" );
			//----------------------------------------------------------------------------------------------------------------
		}
	}
	else {
		if ( Start ) {
			SendMessage( FA_PrePost_Msg_Handle , WM_COMMAND , IDYES , (LPARAM) (BOOL) TimeOut );
		}
		else {
			SendMessage( FA_PrePost_Msg_Handle , WM_COMMAND , IDOK , (LPARAM) (BOOL) TimeOut );
		}
	}
}
//=============================================================================
//=============================================================================

