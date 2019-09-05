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
#include "resource.h"
#include "File_ReadInit.h"
//================================================================================

void Scheduler_Run_Unload_FlagReset( int side , BOOL manual , BOOL cancel ); // 2012.04.03

//================================================================================

void Manual_HandOff_Unload_Proc_WM_INITDIALOG( HWND hdlg , int Cassette , int *mode , char *CassetteString ) {
	//--------------------------------------------------
	sprintf( CassetteString , "Cassette%d/PORT%d" , Cassette - CM1 + 1 , Cassette - CM1 + 1 );
	//--------------------------------------------------
	FA_HANDLER_WINDOW_POS( hdlg , Cassette - CM1 );
	KWIN_Item_String_Display( hdlg , IDC_STATIC_MODULE , CassetteString );
	if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
		*mode = 0;
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"Running Unload Enter Function"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_ENTER ) );
		EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_ENTER" );
		KWIN_Item_Disable( hdlg , IDCANCEL );
		KWIN_Item_Disable( hdlg , IDOK );
		KWIN_Item_Disable( hdlg , IDYES );
	}
	else {
		*mode = 1;
		KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If you need to run for Unload Function ,\nPress [Unload] Button."*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_UNLOADM ) );
		KWIN_Item_Disable( hdlg , IDCANCEL );
		KWIN_Item_Enable( hdlg , IDOK );
		KWIN_Item_Enable( hdlg , IDYES );
	}
}

BOOL Manual_HandOff_Unload_Proc_WM_TIMER( HWND hdlg , int Cassette , int *mode ) {
	if ( *mode == 0 ) {
		switch( EQUIP_READ_FUNCTION( Cassette ) ) {
		case SYS_SUCCESS :
			*mode = 1;
			KWIN_Item_String_Display( hdlg , IDC_STATIC_MESSAGE , /*"If you need to run for Unload Function ,\nPress [Unload] Button."*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_UNLOADM ) );
			KWIN_Item_Disable( hdlg , IDCANCEL );
			KWIN_Item_Enable( hdlg , IDOK );
			KWIN_Item_Enable( hdlg , IDYES );
			break;
		case SYS_RUNNING :
			break;
		default :
			return FALSE;
			break;
		}
	}
	else if ( *mode == 2 ) {
		switch( EQUIP_READ_FUNCTION( Cassette ) ) {
		case SYS_RUNNING :
			break;
		default :
			if ( *mode == 99 ) break;
			*mode = 99;
			if ( _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( Cassette ) ) {
				EQUIP_RUN_SET_FUNCTION( Cassette , "HAND_OFF_OUT_LEAVE" );
			}
			return FALSE;
			break;
		}
	}
	return TRUE;
}

BOOL Manual_HandOff_Unload_Proc_WM_COMMAND( HWND hdlg , int wParam , int Cassette , int *mode , char *CassetteString ) {
	int Res;
	switch( wParam ) {
	case IDOK :
		if ( *mode == 99 ) return TRUE;
		if ( _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) {
			Res = IDYES;
		}
		else {
			Res = MessageBox( hdlg , /*"Do you want to Unload later?"*/ SCH_Get_String_Table( ST_INDEX_UNLOAD_CONFIRM_LATER ) , CassetteString , MB_ICONQUESTION | MB_YESNO );
		}
		if ( IDYES == Res ) {
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
			KWIN_Item_Disable( hdlg , IDOK );
		}
		break;
	}
	return TRUE;
}
//=============================================================================
//=============================================================================
//=============================================================================
#define MAKE_MANUAL_HANDOFF_UNLOAD( ZZZ ) BOOL APIENTRY Manual_HandOff_Unload_##ZZZ##_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) { \
	static int mode , id , Cassette; \
	static char CassetteString[24]; \
	switch ( msg ) { \
	case WM_INITDIALOG: \
		Cassette = (int) lParam; \
		Manual_HandOff_Unload_Proc_WM_INITDIALOG( hdlg , Cassette , &mode , CassetteString ); \
		id = SetTimer( hdlg , 2040 + Cassette - CM1 , 200 , NULL ); \
		return TRUE; \
	case WM_TIMER : \
		if ( !Manual_HandOff_Unload_Proc_WM_TIMER( hdlg , Cassette , &mode ) ) EndDialog( hdlg , 0 ); \
		return TRUE; \
	case WM_COMMAND : \
		if ( !Manual_HandOff_Unload_Proc_WM_COMMAND( hdlg , wParam , Cassette , &mode , CassetteString ) ) EndDialog( hdlg , 0 ); \
		return TRUE; \
	case WM_DESTROY : \
		KillTimer( hdlg , id ); \
		return TRUE; \
	} \
	return FALSE; \
}

MAKE_MANUAL_HANDOFF_UNLOAD( 1 )
MAKE_MANUAL_HANDOFF_UNLOAD( 2 )
MAKE_MANUAL_HANDOFF_UNLOAD( 3 )
MAKE_MANUAL_HANDOFF_UNLOAD( 4 )
//=============================================================================
//=============================================================================
void MANUAL_UNLOAD_CONTROLLER_TH( int ch ) {
//	BOOL force = FALSE; // 2013.06.13
	BOOL cioset; // 2013.06.13
	HWND hWnd;
	HANDLE hdle;
	int i , Res; // 2016.11.02
	//----------------------------------------------------------------------
//	FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( (ch%10) - CM1 , 2 ); // 2012.04.03 // 2013.07.10
	FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( (ch%10) - CM1 , 10 + FA_UNLOAD_CONTROLLER_RUN_STATUS_GET( (ch%10) - CM1 ) ); // 2012.04.03 // 2013.07.10
	//----------------------------------------------------------------------
	// 2008.03.18
	//----------------------------------------------------------------------
	while ( TRUE ) {
		if ( FA_MAPPING_SIGNAL_GET( (ch%10) - CM1 ) < 3 ) break;
		Sleep(1000);
	}
	//----------------------------------------------------------------------
// 2013.06.13
	//
	cioset = FA_MAIN_BUTTON_SET_CHECK( (ch%10) - CM1 );
	//----------------------------------------------------------------------
	if ( ((ch/100000)%10) >= 1 ) Scheduler_Run_Unload_FlagReset( (ch%10) - CM1 , TRUE , ( ((ch/10000)%10) >= 1 ) ); // 2012.04.03 // 2013.05.27
	//----------------------------------------------------------------------
	BUTTON_SET_HANDOFF_CONTROL( (ch%10) , CTC_RUNNING );
	//----------------------------------------------------------------------
	if ( ((ch/100)%10) >= 1 ) {
		if ( FA_UNLOAD_HANDOFF_AGAIN_CHECK( (ch%10) - CM1 ) == SYS_ABORTED ) {
			//----------------------------------------------------------------------
			BUTTON_SET_HANDOFF_CONTROL( (ch%10) , CTC_IDLE );
//			if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (ch%10) - CM1 , CTC_IDLE ); // 2013.06.13
			if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (ch%10) - CM1 , CTC_IDLE ); // 2013.06.13
			//----------------------------------------------------------------------
			//----------------------------------------------------------------------
			FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( (ch%10) - CM1 , 0 ); // 2012.04.03
			//----------------------------------------------------------------------
			_endthread();
			return;
		}
	}
	//----------------------------------------------------------------------
	if ( ((ch/10000)%10) >= 1 ) {
//		BUTTON_SET_FLOW_STATUS( (ch%10) , _MS_18_UNLOADING_C ); // 2011.03.22
		BUTTON_SET_FLOW_STATUS( (ch%10) - CM1 , _MS_18_UNLOADING_C ); // 2011.03.22
	}
	else {
//		BUTTON_SET_FLOW_STATUS( (ch%10) , _MS_19_UNLOADING ); // 2011.03.22
		BUTTON_SET_FLOW_STATUS( (ch%10) - CM1 , _MS_19_UNLOADING ); // 2011.03.22
	}
	//----------------------------------------------------------------------
	if ( ((ch/1000)%10) >= 1 ) {
		if ( EQUIP_CARRIER_HANDOFF_OUT( (ch%10) ) == SYS_ABORTED ) {
			//----------------------------------------------------------------------
			BUTTON_SET_HANDOFF_CONTROL( (ch%10) , CTC_IDLE );
//			if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (ch%10) - CM1 , CTC_IDLE ); // 2013.06.13
			if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (ch%10) - CM1 , CTC_IDLE ); // 2013.06.13
			//----------------------------------------------------------------------
//			BUTTON_SET_FLOW_STATUS( (ch%10) , _MS_22_UNLOADFAIL ); // 2011.03.22
			BUTTON_SET_FLOW_STATUS( (ch%10) - CM1 , _MS_22_UNLOADFAIL ); // 2011.03.22
			//----------------------------------------------------------------------
			FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( (ch%10) - CM1 , 0 ); // 2012.04.03
			//----------------------------------------------------------------------
			_endthread();
			return;
		}
	}
	//----------------------------------------------------------------------
	FA_UNLOADING_SIGNAL_SET( (ch%10) - CM1 , TRUE );
	//
	//
	/*
	//
	// 2016.11.02
	//
	hWnd = GetTopWindow( NULL ); // 2002.10.30
	hdle = GetModuleHandle( NULL ); // 2002.10.30 // 2004.01.19 // 2004.08.10 // 2004.09.07
	//
	if      ( ( ( (ch%10) - CM1 ) % 4 ) == 0 )
		GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_1_Proc , (LPARAM) (int) (ch%10) );
	else if ( ( ( (ch%10) - CM1 ) % 4 ) == 1  )
		GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_2_Proc , (LPARAM) (int) (ch%10) );
	else if ( ( ( (ch%10) - CM1 ) % 4 ) == 2  )
		GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_3_Proc , (LPARAM) (int) (ch%10) );
	else if ( ( ( (ch%10) - CM1 ) % 4 ) == 3  )
		GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_4_Proc , (LPARAM) (int) (ch%10) );
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
		if      ( ( ( (ch%10) - CM1 ) % 4 ) == 0 )
			Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_1_Proc , (LPARAM) (int) (ch%10) );
		else if ( ( ( (ch%10) - CM1 ) % 4 ) == 1  )
			Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_2_Proc , (LPARAM) (int) (ch%10) );
		else if ( ( ( (ch%10) - CM1 ) % 4 ) == 2  )
			Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_3_Proc , (LPARAM) (int) (ch%10) );
		else if ( ( ( (ch%10) - CM1 ) % 4 ) == 3  )
			Res = GoModalDialogBoxParam( hdle , MAKEINTRESOURCE( IDD_MANUAL_UNLOAD_PAD ) , hWnd , Manual_HandOff_Unload_4_Proc , (LPARAM) (int) (ch%10) );
		else
			Res = -99;
		//
		if ( Res != -1 ) break;
		//
	}
	//
	//
	//
	FA_LOADREQ_SIGNAL_SET( (ch%10) - CM1 , FALSE );
	FA_UNLOADING_SIGNAL_SET( (ch%10) - CM1 , FALSE );
	//----------------------------------------------------------------------
	if ( ((ch/10000)%10) >= 1 ) {
//		BUTTON_SET_FLOW_STATUS( (ch%10) , _MS_20_UNLOADED_C ); // 2011.03.22
		BUTTON_SET_FLOW_STATUS( (ch%10) - CM1 , _MS_20_UNLOADED_C ); // 2011.03.22
	}
	else {
//		BUTTON_SET_FLOW_STATUS( (ch%10) , _MS_21_UNLOADED ); // 2011.03.22
		BUTTON_SET_FLOW_STATUS( (ch%10) - CM1 , _MS_21_UNLOADED ); // 2011.03.22
	}
	//
	BUTTON_SET_HANDOFF_CONTROL( (ch%10) , CTC_IDLE );
//	if ( !force ) _i_SCH_IO_SET_MAIN_BUTTON( (ch%10) - CM1 , CTC_IDLE ); // 2013.06.13
	if ( cioset ) _i_SCH_IO_SET_MAIN_BUTTON( (ch%10) - CM1 , CTC_IDLE ); // 2013.06.13
	//----------------------------------------------------------------------
//	if ( ((ch/100000)%10) >= 1 ) Scheduler_Run_Unload_FlagReset( (ch%10) - CM1 , TRUE , ( ((ch/10000)%10) >= 1 ) ); // 2012.04.03 // 2013.05.27
	//
	FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( (ch%10) - CM1 , 0 ); // 2012.04.03
	//----------------------------------------------------------------------
	_endthread();
}

//=============================================================================
//=============================================================================





//void MANUAL_UNLOAD_CONTROLLER( int ch1 , int ch2 , BOOL Manual , BOOL CancelCheck , BOOL CancelMsg , BOOL flagcheck ) { // 2012.07.10
void MANUAL_UNLOAD_CONTROLLER( int ch , BOOL Manual , BOOL CancelCheck , BOOL CancelMsg , BOOL flagcheck ) { // 2012.07.10
	int res = 0 , sts;

	if ( CancelCheck ) res += 100;
	if ( Manual )      res += 1000;
	if ( CancelMsg )   res += 10000;
	if ( flagcheck )   res += 100000; // 2012.04.03
	//
	//
	// 2012.07.10
	if ( _i_SCH_PRM_GET_AUTO_HANDOFF( ch ) >= 4 ) {
		//
		sts = _beginthread( (void *) MANUAL_UNLOAD_CONTROLLER_TH , 0 , (void *) ( ch + res ) );
		//
		if ( sts == -1 ) {
			//----------------------------------------------------------------------------------------------------------------
			// 2004.08.18
			//----------------------------------------------------------------------------------------------------------------
			FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( ch - CM1 , 0 ); // 2012.07.10
			//
			_IO_CIM_PRINTF( "THREAD FAIL MANUAL_UNLOAD_CONTROLLER_TH(1) %d\n" , ch + res );
			//
//			BUTTON_SET_FLOW_STATUS( ch , 20 ); // 2017.09.13
			BUTTON_SET_FLOW_STATUS( ch , _MS_22_UNLOADFAIL ); // 2017.09.13
			//----------------------------------------------------------------------------------------------------------------
		}
	}
	else {
		FA_UNLOAD_CONTROLLER_RUN_STATUS_SET( ch - CM1 , 0 ); // 2012.07.10
	}
	//
	/*
	// 2012.07.10
	if ( ch1 > 0 ) {
		if ( _i_SCH_PRM_GET_AUTO_HANDOFF( ch1 ) >= 4 ) {
			//
			sts = _beginthread( (void *) MANUAL_UNLOAD_CONTROLLER_TH , 0 , (void *) ( ch1 + res ) );
			//
			if ( sts == -1 ) {
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL MANUAL_UNLOAD_CONTROLLER_TH(1) %d\n" , ch1 + res );
				//
				BUTTON_SET_FLOW_STATUS( ch1 , 20 );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
	}
	if ( ( ch1 != ch2 ) && ( ch2 > 0 ) ) {
		if ( _i_SCH_PRM_GET_AUTO_HANDOFF( ch2 ) >= 4 ) {
			//
			sts = _beginthread( (void *) MANUAL_UNLOAD_CONTROLLER_TH , 0 , (void *) ( ch2 + res ) );
			//
			if ( sts == -1 ) {
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.18
				//----------------------------------------------------------------------------------------------------------------
				_IO_CIM_PRINTF( "THREAD FAIL MANUAL_UNLOAD_CONTROLLER_TH(2) %d\n" , ch2 + res );
				//
				BUTTON_SET_FLOW_STATUS( ch2 , 20 );
				//----------------------------------------------------------------------------------------------------------------
			}
		}
	}
	*/
}
