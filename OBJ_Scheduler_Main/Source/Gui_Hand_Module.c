#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "sch_sdm.h"
#include "sch_CommonUser.h"
#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "system_tag.h"
#include "utility.h"
#include "resource.h"
//------------------------------------------------------------------------------------------
BOOL MODULE_SUB_RESULT = FALSE;
BOOL MODULE_SUB_ROTANIM;
char MODULE_SUB_PRCSNAME[64];
int  MODULE_SUB_DUMMY_MODE;
int  MODULE_SUB_DUMMY_DATA;
int  MODULE_SUB_DUMMY_DATA2;
int  MODULE_SUB_DUMMY_DATA3;
int  MODULE_SUB_DUMMY_DATA4;
int  MODULE_SUB_DUMMY_DATA5;
int  MODULE_SUB_DUMMY_DATA6; // 2006.07.12
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_MODULE_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	char buffer[64];
	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MODULE_SUB_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );

//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_ROTANIM , "FALSE|TRUE" , "FALSE" ); // 2007.07.24 // 2010.01.27
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_ROTANIM , "FALSE|TRUE|FALSE(A)" , "FALSE" ); // 2007.07.24 // 2010.01.27

		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , MODULE_SUB_PRCSNAME );

		switch( MODULE_SUB_DUMMY_MODE ) {
		case 0 :
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			break;
		case 1 :
			switch( MODULE_SUB_DUMMY_DATA ) {
			case DM_NOT_USE :		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_NOT_USE" ); break;
			case DM_DEFAULT :		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_DEFAULT" ); break;
			case DM_OPTION1 :		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_OPTION1" ); break;
			case DM_OPTION2 :		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_OPTION2" ); break;
			case DM_OPTION3	:		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_OPTION3" ); break;
			case DM_DEFAULT_DMFIX :	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_DEFAULT_DMFIX" ); break;
			case DM_OPTION1_DMFIX :	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_OPTION1_DMFIX" ); break;
			case DM_OPTION2_DMFIX :	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_OPTION2_DMFIX" ); break;
			case DM_OPTION3_DMFIX :	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "DM_OPTION3_DMFIX" ); break;
			}
			break;
		case 2 :
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , MODULE_SUB_DUMMY_DATA , "Depend|Not Depend(x)" , "Depend" ); // 2007.07.24
			break;
		}
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , MODULE_SUB_DUMMY_DATA2 , "Not Use|Main Recipe Set to Pre Recipe" , "Not Use" ); // 2007.07.24
		//---------------------------------------------------------------
		_SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 5 , 0 , hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA3 );
		//---------------------------------------------------------------
		_SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 6 , 0 , hdlg , IDC_READY_USE_B8 , MODULE_SUB_DUMMY_DATA6 );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B6 , MODULE_SUB_DUMMY_DATA4 , "Always Use|Just Pre Use|Just Post Use|Not Use" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B7 , MODULE_SUB_DUMMY_DATA5 , "Multi lot Single Dummy|Multi lot Multi Dummy|Multi lot Multi Dummy2" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
//			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_ROTANIM ); // 2010.01.27
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &MODULE_SUB_ROTANIM ); // 2010.01.27
			return TRUE;
		case IDC_READY_USE_B2 :
			strcpy( buffer , MODULE_SUB_PRCSNAME );
			if ( MODAL_STRING_BOX1( hdlg , "Select Process Name" , "Select Process Name" , buffer ) ) {
				strncpy( MODULE_SUB_PRCSNAME , buffer , 63 );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			return TRUE;
		case IDC_READY_USE_B3 :
			switch( MODULE_SUB_DUMMY_MODE ) {
			case 0 :
				break;
			case 1 :
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , DM_OPTION3_DMFIX , &MODULE_SUB_DUMMY_DATA );
				break;
			case 2 :
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_DATA );
			}
			return TRUE;
		case IDC_READY_USE_B4 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_DATA2 );
			return TRUE;
		case IDC_READY_USE_B5 :
			//
			_SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( SYSTEM_GUI_ALG_STYLE_GET() , 5 , 0 , hdlg , &MODULE_SUB_DUMMY_DATA3 );
			//
			return TRUE;
		case IDC_READY_USE_B6 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA4 );
			return TRUE;
		case IDC_READY_USE_B7 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &MODULE_SUB_DUMMY_DATA5 );
			return TRUE;
		case IDC_READY_USE_B8 : // 2006.07.10
			//
			_SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( SYSTEM_GUI_ALG_STYLE_GET() , 6 , 0 , hdlg , &MODULE_SUB_DUMMY_DATA6 );
			//
			return TRUE;
		case IDOK :		MODULE_SUB_RESULT = TRUE;	EndDialog( hdlg , 0 );		return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL APIENTRY Gui_IDD_MODULE_SUB_PAD_BM_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MODULE_SUB_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );

//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_ROTANIM , "FALSE|TRUE" , "FALSE" ); // 2007.07.24 // 2010.01.27
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_ROTANIM , "FALSE|TRUE|FALSE(A)" , "FALSE" ); // 2007.07.24 // 2010.01.27

		switch( MODULE_SUB_DUMMY_DATA ) {
		case 0 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Always" );
			break;
		case 1 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free" );
			break;
		case 2 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free" );
			break;
		case 3 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free" );
			break;
		case 4 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Out Process Pick Deny" );
			break;
		case 5 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Out Process Pick+Move Deny" );
			break;
		case 6 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Out Process Only Move Deny" );
			break;
		case 7 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(ALL)" );
			break;
		case 8 : // 2005.03.29
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free(LX)" );
			break;
		case 9 : // 2005.03.29
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free(LX)" );
			break;
		case 10 : // 2005.03.29
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(LX)" );
			break;
		case 11 : // 2006.01.26
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free(A1)" );
			break;
		case 12 : // 2006.01.26
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free(A1)" );
			break;
		case 13 : // 2006.01.26
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(A1)" );
			break;
		case 14 : // 2006.02.08
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free(A1.G0)" );
			break;
		case 15 : // 2006.02.08
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free(A1.G0)" );
			break;
		case 16 : // 2006.02.08
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(A1.G0)" );
			break;
		case 17 : // 2006.07.13
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Count Free" );
			break;
		case 18 : // 2007.12.12
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Direct Switchable" );
			break;
		case 101 : // 2006.04.15
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "User Define 001" );
			break;
		case 102 : // 2006.04.15
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "User Define 002" );
			break;
		}
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B8 , MODULE_SUB_DUMMY_DATA6 , "Always|Process Free|Wafer Free|Process+Wafer Free" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
//			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_ROTANIM ); // 2010.01.27
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &MODULE_SUB_ROTANIM ); // 2010.01.27
			return TRUE;
		case IDC_READY_USE_B2 :
			if      ( MODULE_SUB_DUMMY_DATA == 0 ) MODULE_SUB_DUMMY_DATA = 1;
			else if ( MODULE_SUB_DUMMY_DATA == 1 ) MODULE_SUB_DUMMY_DATA = 2;
			else if ( MODULE_SUB_DUMMY_DATA == 2 ) MODULE_SUB_DUMMY_DATA = 3;
			else if ( MODULE_SUB_DUMMY_DATA == 3 ) MODULE_SUB_DUMMY_DATA = 4;
			else if ( MODULE_SUB_DUMMY_DATA == 4 ) MODULE_SUB_DUMMY_DATA = 5;
			else if ( MODULE_SUB_DUMMY_DATA == 5 ) MODULE_SUB_DUMMY_DATA = 6;
			else if ( MODULE_SUB_DUMMY_DATA == 6 ) MODULE_SUB_DUMMY_DATA = 7;
			else if ( MODULE_SUB_DUMMY_DATA == 7 ) MODULE_SUB_DUMMY_DATA = 8;
			else if ( MODULE_SUB_DUMMY_DATA == 8 ) MODULE_SUB_DUMMY_DATA = 9; // 2005.03.29
			else if ( MODULE_SUB_DUMMY_DATA == 9 ) MODULE_SUB_DUMMY_DATA = 10; // 2005.03.29
			else if ( MODULE_SUB_DUMMY_DATA ==10 ) MODULE_SUB_DUMMY_DATA = 11; // 2005.03.29
			else if ( MODULE_SUB_DUMMY_DATA ==11 ) MODULE_SUB_DUMMY_DATA = 12; // 2006.01.26
			else if ( MODULE_SUB_DUMMY_DATA ==12 ) MODULE_SUB_DUMMY_DATA = 13; // 2006.01.26
			else if ( MODULE_SUB_DUMMY_DATA ==13 ) MODULE_SUB_DUMMY_DATA = 14; // 2006.01.26
			else if ( MODULE_SUB_DUMMY_DATA ==14 ) MODULE_SUB_DUMMY_DATA = 15; // 2006.02.08
			else if ( MODULE_SUB_DUMMY_DATA ==15 ) MODULE_SUB_DUMMY_DATA = 16; // 2006.02.08
			else if ( MODULE_SUB_DUMMY_DATA ==16 ) MODULE_SUB_DUMMY_DATA = 17; // 2006.04.15
			else if ( MODULE_SUB_DUMMY_DATA ==17 ) MODULE_SUB_DUMMY_DATA = 18; // 2007.12.12
			else if ( MODULE_SUB_DUMMY_DATA ==18 ) MODULE_SUB_DUMMY_DATA = 101; // 2006.07.13
			else if ( MODULE_SUB_DUMMY_DATA ==101) MODULE_SUB_DUMMY_DATA = 102; // 2006.04.15
			else if ( MODULE_SUB_DUMMY_DATA ==102) MODULE_SUB_DUMMY_DATA = 0; // 2006.04.15
			InvalidateRect( hdlg , NULL , TRUE );
			return TRUE;
		case IDC_READY_USE_B8 : // 2006.07.10
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA6 );
			return TRUE;
		case IDOK :		MODULE_SUB_RESULT = TRUE;	EndDialog( hdlg , 0 );		return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL APIENTRY Gui_IDD_MODULE_SUB_PAD_CM_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int Res;
	static int ebm;
	char Buffer[64];
	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MODULE_SUB_RESULT = FALSE;
		ebm = FALSE;
		for ( Res = 0 ; Res < MAX_BM_CHAMBER_DEPTH ; Res++ ) {
			if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Res + BM1 ) == BUFFER_OUT_CASSETTE ) {
				ebm = TRUE;
				break;
			}
		}
		if ( !ebm ) {
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B5 );
		}
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B6 , "Pick Possible When Next PM Free" );
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );

		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_ROTANIM , "FALSE|TRUE" , "FALSE" ); // 2007.07.24 // 2010.01.27
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_ROTANIM , "FALSE|TRUE|FALSE(A)" , "FALSE" ); // 2007.07.24 // 2010.01.27
		//---------------------------------------------------------------

		switch( MODULE_SUB_DUMMY_DATA ) {
		case 0 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Always" );
			break;
		case 1 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free" );
			break;
		case 2 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free" );
			break;
		case 3 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free" );
			break;
		case 4 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Out Process Pick Deny" );
			break;
		case 5 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Out Process Pick+Move Deny" );
			break;
		case 6 :
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Out Process Only Move Deny" );
			break;
		case 7 : // 2004.12.14
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(ALL)" );
			break;
		case 8 : // 2005.03.29
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free(LX)" );
			break;
		case 9 : // 2005.03.29
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free(LX)" );
			break;
		case 10 : // 2005.03.29
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(LX)" );
			break;
		case 11 : // 2006.01.26
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free(A1)" );
			break;
		case 12 : // 2006.01.26
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free(A1)" );
			break;
		case 13 : // 2006.01.26
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(A1)" );
			break;
		case 14 : // 2006.02.08
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process Free(A1.G0)" );
			break;
		case 15 : // 2006.02.08
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Wafer Free(A1.G0)" );
			break;
		case 16 : // 2006.02.08
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Process+Wafer Free(A1.G0)" );
			break;
		case 17 : // 2006.07.13
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Count Free" );
			break;
		case 18 : // 2007.12.12
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Direct Switchable" );
			break;
		case 101 : // 2006.04.15
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "User Define 001" );
			break;
		case 102 : // 2006.04.15
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "User Define 002" );
			break;
		}
		//
		if ( ebm ) {
			if ( MODULE_SUB_DUMMY_DATA2 == 0 ) {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "-" );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "" );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B5 , "" );
			}
			else {
				sprintf( Buffer , "BM%d" , MODULE_SUB_DUMMY_DATA2 - BM1 + 1 );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , Buffer );
				//---------------------------------------------------------------
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , MODULE_SUB_DUMMY_DATA3 , "NOTUSE|DIRECT.ERROR|DIRECT.CM|ORDER.ERROR|ORDER.CM|SP1" , "NOTUSE" ); // 2007.07.24
				//---------------------------------------------------------------
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA4 , "ALL|IO|SIGNAL" , "ALL" ); // 2007.07.24
				//---------------------------------------------------------------
			}
		}
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
//			if ( !MODULE_SUB_ROTANIM )	MODULE_SUB_ROTANIM = TRUE; // 2010.01.27
//			else						MODULE_SUB_ROTANIM = FALSE; // 2010.01.27
//			InvalidateRect( hdlg , NULL , TRUE ); // 2010.01.27
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &MODULE_SUB_ROTANIM ); // 2010.01.27
			return TRUE;
		case IDC_READY_USE_B2 :
			if      ( MODULE_SUB_DUMMY_DATA == 0 ) MODULE_SUB_DUMMY_DATA = 1;
			else if ( MODULE_SUB_DUMMY_DATA == 1 ) MODULE_SUB_DUMMY_DATA = 2;
			else if ( MODULE_SUB_DUMMY_DATA == 2 ) MODULE_SUB_DUMMY_DATA = 3;
			else if ( MODULE_SUB_DUMMY_DATA == 3 ) MODULE_SUB_DUMMY_DATA = 4;
			else if ( MODULE_SUB_DUMMY_DATA == 4 ) MODULE_SUB_DUMMY_DATA = 5;
			else if ( MODULE_SUB_DUMMY_DATA == 5 ) MODULE_SUB_DUMMY_DATA = 6;
			else if ( MODULE_SUB_DUMMY_DATA == 6 ) MODULE_SUB_DUMMY_DATA = 7;
			else if ( MODULE_SUB_DUMMY_DATA == 7 ) MODULE_SUB_DUMMY_DATA = 8; // 2004.12.14
			else if ( MODULE_SUB_DUMMY_DATA == 8 ) MODULE_SUB_DUMMY_DATA = 9; // 2005.03.29
			else if ( MODULE_SUB_DUMMY_DATA == 9 ) MODULE_SUB_DUMMY_DATA = 10; // 2005.03.29
			else if ( MODULE_SUB_DUMMY_DATA ==10 ) MODULE_SUB_DUMMY_DATA = 11; // 2005.03.29
			else if ( MODULE_SUB_DUMMY_DATA ==11 ) MODULE_SUB_DUMMY_DATA = 12; // 2006.01.26
			else if ( MODULE_SUB_DUMMY_DATA ==12 ) MODULE_SUB_DUMMY_DATA = 13; // 2006.01.26
			else if ( MODULE_SUB_DUMMY_DATA ==13 ) MODULE_SUB_DUMMY_DATA = 14; // 2006.01.26
			else if ( MODULE_SUB_DUMMY_DATA ==14 ) MODULE_SUB_DUMMY_DATA = 15; // 2006.02.08
			else if ( MODULE_SUB_DUMMY_DATA ==15 ) MODULE_SUB_DUMMY_DATA = 16; // 2006.02.08
			else if ( MODULE_SUB_DUMMY_DATA ==16 ) MODULE_SUB_DUMMY_DATA = 17; // 2006.04.15
			else if ( MODULE_SUB_DUMMY_DATA ==17 ) MODULE_SUB_DUMMY_DATA = 18; // 2007.12.12
			else if ( MODULE_SUB_DUMMY_DATA ==18 ) MODULE_SUB_DUMMY_DATA = 101; // 2006.07.13
			else if ( MODULE_SUB_DUMMY_DATA ==101) MODULE_SUB_DUMMY_DATA = 102; // 2006.04.15
			else if ( MODULE_SUB_DUMMY_DATA ==102) MODULE_SUB_DUMMY_DATA = 0; // 2006.04.15
			InvalidateRect( hdlg , NULL , TRUE );
			return TRUE;
		case IDC_READY_USE_B3 :
			if ( !ebm ) return TRUE;
			if ( MODULE_SUB_DUMMY_DATA2 == 0 ) {
				Res = 0;
			}
			else {
				Res = MODULE_SUB_DUMMY_DATA2 - BM1 + 1;
			}
			if ( MODAL_DIGITAL_BOX2( hdlg , "BM for Cassette Out" , "Select" , 0 , MAX_BM_CHAMBER_DEPTH , &Res ) ) {
				if ( Res == 0 ) {
					MODULE_SUB_DUMMY_DATA2 = 0;
					InvalidateRect( hdlg , NULL , TRUE );
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , Res + BM1 - 1 ) == BUFFER_OUT_CASSETTE ) {
						MODULE_SUB_DUMMY_DATA2 = Res + BM1 - 1;
						InvalidateRect( hdlg , NULL , TRUE );
					}
					else {
						sprintf( Buffer , "BM%d is not Cassete mode" , Res );
						MessageBox( hdlg , Buffer , "Error", MB_ICONQUESTION );
					}
				}
			}
			return TRUE;
		case IDC_READY_USE_B4 :
			KWIN_GUI_SELECT_DATA_INCREASE( !ebm , hdlg , 4 , &MODULE_SUB_DUMMY_DATA3 );
			return TRUE;
		case IDC_READY_USE_B5 :
			KWIN_GUI_SELECT_DATA_INCREASE( !ebm , hdlg , 2 , &MODULE_SUB_DUMMY_DATA4 );
			return TRUE;
		case IDOK :		MODULE_SUB_RESULT = TRUE;	EndDialog( hdlg , 0 );		return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_MODULE_SUB_PAD4_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01
		//
		MODULE_SUB_RESULT = FALSE;
		//
		if      ( MODULE_SUB_ROTANIM == 1 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_B6 ); // 2016.12.05 FM
		else if ( MODULE_SUB_ROTANIM == 2 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_B6 ); // 2016.12.05 BM
		//
		return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		if ( MODULE_SUB_ROTANIM == 1 ) { // FM
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_DUMMY_MODE  , "Use|Not Use(FM)|Not Use(BM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , MODULE_SUB_DUMMY_DATA  , "Use|Not Use(FM)|Not Use(BM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , MODULE_SUB_DUMMY_DATA2 , "Use|Not Use(FM)|Not Use(BM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , MODULE_SUB_DUMMY_DATA3 , "Use|Not Use(FM)|Not Use(BM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA4 , "Use|Not Use(FM)|Not Use(BM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
		}
		else if ( MODULE_SUB_ROTANIM == 2 ) { // BM
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_DUMMY_MODE  , "Use|Not Use(TM)|Not Use(FM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , MODULE_SUB_DUMMY_DATA  , "Use|Not Use(TM)|Not Use(FM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , MODULE_SUB_DUMMY_DATA2 , "Use|Not Use(TM)|Not Use(FM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , MODULE_SUB_DUMMY_DATA3 , "Use|Not Use(TM)|Not Use(FM)|Not Use(ALL)" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
//			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA4 , "Use|Not Use" , "Use" ); // 2007.07.24 // 2009.06.15
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA4 , "Use|Not Use|Use2" , "Use" ); // 2007.07.24 // 2009.06.15
			//---------------------------------------------------------------
		}
		else {
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , MODULE_SUB_DUMMY_MODE  , "Use|Not Use" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , MODULE_SUB_DUMMY_DATA  , "Use|Not Use" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , MODULE_SUB_DUMMY_DATA2 , "Use|Not Use" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , MODULE_SUB_DUMMY_DATA3 , "Use|Not Use" , "Use" ); // 2007.07.24
			//---------------------------------------------------------------
//			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA4 , "Use|Not Use" , "Use" ); // 2007.07.24 // 2009.06.15
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , MODULE_SUB_DUMMY_DATA4 , "Use|Not Use|Use2" , "Use" ); // 2007.07.24 // 2009.06.15
			//---------------------------------------------------------------
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B6 , MODULE_SUB_DUMMY_DATA5 , "-|Not Use|Use" , "-" ); // 2016.11.24
			//---------------------------------------------------------------
		}
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			if      ( MODULE_SUB_ROTANIM == 1 ) { // FM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_MODE );
			}
			else if ( MODULE_SUB_ROTANIM == 2 ) { // BM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_MODE );
			}
			else {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_MODE );
			}
			return TRUE;
		case IDC_READY_USE_B2 :
			if      ( MODULE_SUB_ROTANIM == 1 ) { // FM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA );
			}
			else if ( MODULE_SUB_ROTANIM == 2 ) { // BM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA );
			}
			else {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_DATA );
			}
			return TRUE;
		case IDC_READY_USE_B3 :
			if      ( MODULE_SUB_ROTANIM == 1 ) { // FM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA2 );
			}
			else if ( MODULE_SUB_ROTANIM == 2 ) { // BM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA2 );
			}
			else {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_DATA2 );
			}
			return TRUE;
		case IDC_READY_USE_B4 :
			if      ( MODULE_SUB_ROTANIM == 1 ) { // FM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA3 );
			}
			else if ( MODULE_SUB_ROTANIM == 2 ) { // BM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA3 );
			}
			else {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_DATA3 );
			}
			return TRUE;
		case IDC_READY_USE_B5 :
			if ( MODULE_SUB_ROTANIM == 1 ) { // FM
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &MODULE_SUB_DUMMY_DATA4 );
			}
			else {
//				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &MODULE_SUB_DUMMY_DATA4 ); // 2009.06.15
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &MODULE_SUB_DUMMY_DATA4 ); // 2009.06.15
			}
			return TRUE;

		case IDC_READY_USE_B6 : // 2016.11.24
			//
			if      ( MODULE_SUB_ROTANIM == 1 ) return TRUE; // 2016.12.05 FM
			else if ( MODULE_SUB_ROTANIM == 2 ) return TRUE; // 2016.12.05 BM
			//
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &MODULE_SUB_DUMMY_DATA5 );
			//
			return TRUE;

		case IDOK :		MODULE_SUB_RESULT = TRUE;	EndDialog( hdlg , 0 );		return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_MODULE_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i;
	static int  TEMP_READY_USE[MAX_CHAMBER];
	static int  TEMP_READY_USE2[MAX_CHAMBER];
	static int  TEMP_READY_USE3;
	static int  TEMP_READY_USE32;
	static int  TEMP_READY_USE33;
	static int  TEMP_READY_USE4[MAX_CHAMBER];
	static int  TEMP_READY_USE5[MAX_CHAMBER];
	static int  TEMP_READY_USE52[MAX_CHAMBER]; // 2006.07.12
	static int  TEMP_READY_USE6[MAX_CHAMBER];
	static int  TEMP_READY_USE7[MAX_CHAMBER];
	static int  TEMP_READY_USE8[MAX_CHAMBER];
	static int  TEMP_READY_USE_PrRv[MAX_CHAMBER];
	static int  TEMP_READY_USE_PrSn[MAX_CHAMBER];
	static int  TEMP_READY_USE_PoRv[MAX_CHAMBER];
	static int  TEMP_READY_USE_PoSn[MAX_CHAMBER];
	static int  TEMP_READY_USE_Gate[MAX_CHAMBER];
	static int  TEMP_READY_USE_Cancel[MAX_CHAMBER]; // 2016.11.24
	static char TEMP_READY_RECIPE[MAX_CHAMBER][64];
	static BOOL Control;
	static int  Modify_Sdm;
	char Buffer[256];
	char *szString[] = { "PM" , "RotAnim" , "Process Name" , "DummyRun" , "SDM" , "Pre" , "FrPk" };
	int    szSize[7] = {  60  ,        68 ,            151 ,        160 ,    73 ,    73 ,    73  };
	char *szString2[] = { "M" , "RotAnim" , "FrPk" , "ErrorOut/Order" };
	int    szSize2[4] = {  45  ,       68  ,   43  ,        200 };
	char *szString3[] = { "No" , "RunPM" , "DumPM" , "Count" , "Time(sec)" , "Mode" , "Recipe" };
	int    szSize3[7] = {  30  ,      65 ,      66 ,      80 ,         120 ,     70 ,   200 };
	char *szString3_2[] = { "No" , "SelPM" , "PreRecipe" , "RunRecipe" , "PreRecipeF" , "RunRecipeF" , "PreRecipeE" , "RunRecipeE" , "RunCount" };
	int    szSize3_2[9] = {  30  ,      65 ,        200  ,        200  ,         200  ,         200  ,         200  ,         200  ,        100 };
	char *szString4[] = { "M" , "PrRv" , "PrSn" , "PoRv" , "PoSn" , "Gat." , "Can" }; // 2016.11.24
	int    szSize4[7] = {  45  ,    45 ,    45  ,    48  ,    48  ,     41 ,    41 }; // 2016.11.24
	HWND hSWnd;
	char bufferdir[256];
	int  Res;

	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.01
		//
		return TRUE;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.01
		//
		//------------------------------------------------------------
		Control = (BOOL) lParam;
		//------------------------------------------------------------
		Modify_Sdm = FALSE;
		MoveCenterWindow( hdlg );
		for ( i = PM1 ; i < TM ; i++ ) {
			strncpy( TEMP_READY_RECIPE[ i ] , _i_SCH_PRM_GET_MODULE_PROCESS_NAME(i) , 63 );
			TEMP_READY_USE2[ i ] = _i_SCH_PRM_GET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER(i);
			TEMP_READY_USE4[ i ] = _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE(i);
		}
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			TEMP_READY_USE[ i ] = _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE(i);
			TEMP_READY_USE5[ i ]  = _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE(i);
			TEMP_READY_USE52[ i ] = _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE(i); // 2006.07.12
			TEMP_READY_USE6[ i ]  = _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE(i);
			TEMP_READY_USE7[ i ]  = _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA(i);
			TEMP_READY_USE8[ i ]  = _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK(i);
			//
			TEMP_READY_USE_PrRv[ i ] = _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV(i);
			TEMP_READY_USE_PrSn[ i ] = _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND(i);
			TEMP_READY_USE_PoRv[ i ] = _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV(i);
			TEMP_READY_USE_PoSn[ i ] = _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND(i);
			TEMP_READY_USE_Gate[ i ] = _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE(i);
			//
			TEMP_READY_USE_Cancel[ i ] = _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL(i); // 2016.11.24
		}
		TEMP_READY_USE3 = _i_SCH_PRM_GET_DUMMY_PROCESS_MODE();
		TEMP_READY_USE32 = _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER();
		TEMP_READY_USE33 = _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS(); // 2003.02.08

		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 7 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 4 , szString2 , szSize2 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , 9 , szString3_2 , szSize3_2 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == 0 ) {
			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , 0 );
			KWIN_Item_String_Display( hdlg , IDYES , "NOT USED" );
		}
		else {
			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , _i_SCH_PRM_GET_MODULE_SIZE( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) );
			sprintf( Buffer , "RUN BM%d" , _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() - BM1 + 1 );
			KWIN_Item_String_Display( hdlg , IDYES , Buffer );
		}
		//======================================================================
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , 7 , szString4 , szSize4 ); // 2016.11.24
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , MAX_BM_CHAMBER_DEPTH + MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + 1 );
		//
		return TRUE;
	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i , j;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO:	{
					LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
					TCHAR szString[ 256 ];
					TCHAR szString2[ 256 ];
					//
					szString[0] = 0; // 2016.02.02
					//
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if      ( TEMP_READY_USE[i+PM1] == 1 )	sprintf( szString , "TRUE" );
										else if ( TEMP_READY_USE[i+PM1] == 2 )	sprintf( szString , "FALSE(A)" ); // 2010.01.27
										else									sprintf( szString , "FALSE" );
									}
									else {
										sprintf( szString , "(x)" );
									}
									break;
								case 2 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) )	sprintf( szString , "%s" , TEMP_READY_RECIPE[i+PM1] );
									else								sprintf( szString , "" );
									break;
								case 3 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) )	{
										if ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() > 0 ) {
											if ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() == ( i + PM1 ) ) {
												if ( TEMP_READY_USE33 == 0 ) {
													switch( TEMP_READY_USE3 ) {
													case DM_NOT_USE :		sprintf( szString , "D.S(%d):[NOT USE]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_DEFAULT :		sprintf( szString , "D.S(%d):[DEFAULT]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION1 :		sprintf( szString , "D.S(%d):[OPTION1]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION2 :		sprintf( szString , "D.S(%d):[OPTION2]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION3	:		sprintf( szString , "D.S(%d):[OPTION3]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_DEFAULT_DMFIX :	sprintf( szString , "D.S(%d):[DEF_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION1_DMFIX :	sprintf( szString , "D.S(%d):[OP1_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION2_DMFIX :	sprintf( szString , "D.S(%d):[OP2_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION3_DMFIX :	sprintf( szString , "D.S(%d):[OP3_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													}
												}
												else if ( TEMP_READY_USE33 == 1 ) {
													switch( TEMP_READY_USE3 ) {
													case DM_NOT_USE :		sprintf( szString , "D.M(%d):[NOT USE]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_DEFAULT :		sprintf( szString , "D.M(%d):[DEFAULT]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION1 :		sprintf( szString , "D.M(%d):[OPTION1]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION2 :		sprintf( szString , "D.M(%d):[OPTION2]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION3	:		sprintf( szString , "D.M(%d):[OPTION3]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_DEFAULT_DMFIX :	sprintf( szString , "D.M(%d):[DEF_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION1_DMFIX :	sprintf( szString , "D.M(%d):[OP1_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION2_DMFIX :	sprintf( szString , "D.M(%d):[OP2_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION3_DMFIX :	sprintf( szString , "D.M(%d):[OP3_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													}
												}
												else {
													switch( TEMP_READY_USE3 ) {
													case DM_NOT_USE :		sprintf( szString , "D.m(%d):[NOT USE]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_DEFAULT :		sprintf( szString , "D.m(%d):[DEFAULT]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION1 :		sprintf( szString , "D.m(%d):[OPTION1]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION2 :		sprintf( szString , "D.m(%d):[OPTION2]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION3	:		sprintf( szString , "D.m(%d):[OPTION3]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_DEFAULT_DMFIX :	sprintf( szString , "D.m(%d):[DEF_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION1_DMFIX :	sprintf( szString , "D.m(%d):[OP1_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION2_DMFIX :	sprintf( szString , "D.m(%d):[OP2_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													case DM_OPTION3_DMFIX :	sprintf( szString , "D.m(%d):[OP3_FIX]:%d" , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , TEMP_READY_USE32 ); break;
													}
												}
											}
											else {
												if  ( TEMP_READY_USE2[ i+PM1 ] )	sprintf( szString , "x" );
												else								sprintf( szString , "" );
											}
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										sprintf( szString , "" );
									}
									break;
								case 4 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) )	{
										for ( j = 0 ; j < MAX_SDUMMY_DEPTH ; j++ ) {
											if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( j ) == i + PM1 ) {
												sprintf( szString , "%d" , _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( j ) );
												break;
											}
										}
										if ( j == MAX_SDUMMY_DEPTH ) sprintf( szString , "" );
									}
									else {
										sprintf( szString , "" );
									}
									break;
								case 5 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) )	{
										if ( TEMP_READY_USE4[ i+PM1 ] ) {
											sprintf( szString , "SET" );
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										sprintf( szString , "" );
									}
									break;
								case 6 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) )	{
										//
										_SCH_COMMON_GUI_INTERFACE_DATA_GET( SYSTEM_GUI_ALG_STYLE_GET() , 5 , i , TEMP_READY_USE5[ i+PM1 ] , szString );
										_SCH_COMMON_GUI_INTERFACE_DATA_GET( SYSTEM_GUI_ALG_STYLE_GET() , 6 , i , TEMP_READY_USE52[ i+PM1 ] , szString2 );

										if ( strlen( szString ) <= 0 ) {
											if ( strlen( szString2 ) > 0 ) {
												sprintf( szString , "-|%s" , szString2 );
											}
										}
										else {
											if ( strlen( szString2 ) > 0 ) {
												strcat( szString , "|" );
												strcat( szString , szString2 );
											}
										}
										//
									}
									else {
										sprintf( szString , "" );
									}
									break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if ( i < MAX_CASSETTE_SIDE ) {
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
											if      ( TEMP_READY_USE[i+CM1] == 1 )	sprintf( szString , "TRUE" );
											else if ( TEMP_READY_USE[i+CM1] == 2 )	sprintf( szString , "FALSE(A)" ); // 2010.01.27
											else									sprintf( szString , "FALSE" );
										}
										else {
											sprintf( szString , "(x)" );
										}
									}
									else {
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i-MAX_CASSETTE_SIDE+BM1) ) {
											if      ( TEMP_READY_USE[i-MAX_CASSETTE_SIDE+BM1] == 1 )	sprintf( szString , "TRUE" );
											else if ( TEMP_READY_USE[i-MAX_CASSETTE_SIDE+BM1] == 2 )	sprintf( szString , "FALSE(A)" ); // 2010.01.27
											else														sprintf( szString , "FALSE" );
										}
										else {
											sprintf( szString , "(x)" );
										}
									}
									break;
								case 2 :
									if ( i < MAX_CASSETTE_SIDE ) {
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) )	{
											switch ( TEMP_READY_USE5[ i+CM1 ] ) {
											case 0 : sprintf( szString , "" ); break;
											case 1 : sprintf( szString , "PF" ); break;
											case 2 : sprintf( szString , "WF" ); break;
											case 3 : sprintf( szString , "AF" ); break;
											case 4 : sprintf( szString , "OPF" ); break;
											case 5 : sprintf( szString , "OAF" ); break;
											case 6 : sprintf( szString , "OMF" ); break;
											case 7 : sprintf( szString , "FF" ); break; // 2004.12.14
											case 8 : sprintf( szString , "PF.lx" ); break; // 2005.03.29
											case 9 : sprintf( szString , "WF.lx" ); break; // 2005.03.29
											case 10 : sprintf( szString , "AF.lx" ); break; // 2005.03.29
											case 11 : sprintf( szString , "PF.A1" ); break; // 2006.01.26
											case 12 : sprintf( szString , "WF.A1" ); break; // 2006.01.26
											case 13 : sprintf( szString , "AF.A1" ); break; // 2006.01.26
											case 14 : sprintf( szString , "PF.A1G0" ); break; // 2006.02.08
											case 15 : sprintf( szString , "WF.A1G0" ); break; // 2006.02.08
											case 16 : sprintf( szString , "AF.A1G0" ); break; // 2006.02.08
											case 17 : sprintf( szString , "CF" ); break; // 2006.07.13
											case 18 : sprintf( szString , "DSWITCH" ); break; // 2007.12.12
											case 101: sprintf( szString , "UD001" ); break; // 2006.04.15
											case 102: sprintf( szString , "UD002" ); break; // 2006.04.15
											}
											//--------------------------------------------------------------------
											// 2006.07.12
											//--------------------------------------------------------------------
											switch ( TEMP_READY_USE52[ i+CM1 ] ) {
											case 1 :
												if ( TEMP_READY_USE5[ i+CM1 ] == 0 ) sprintf( szString , "-" );
												strcat( szString , "|PF" );
												break;
											case 2 :
												if ( TEMP_READY_USE5[ i+CM1 ] == 0 ) sprintf( szString , "-" );
												strcat( szString , "|WF" );
												break;
											case 3 :
												if ( TEMP_READY_USE5[ i+CM1 ] == 0 ) sprintf( szString , "-" );
												strcat( szString , "|AF" );
												break;
											}
											//--------------------------------------------------------------------
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i-MAX_CASSETTE_SIDE+BM1) )	{
											switch ( TEMP_READY_USE5[ i-MAX_CASSETTE_SIDE+BM1 ] ) {
											case 0 : sprintf( szString , "" ); break;
											case 1 : sprintf( szString , "PF" ); break;
											case 2 : sprintf( szString , "WF" ); break;
											case 3 : sprintf( szString , "AF" ); break;
											case 4 : sprintf( szString , "OPF" ); break;
											case 5 : sprintf( szString , "OAF" ); break;
											case 6 : sprintf( szString , "OMF" ); break;
											case 7 : sprintf( szString , "FF" ); break; // 2004.12.14
											case 8 : sprintf( szString , "PF.lx" ); break; // 2005.03.29
											case 9 : sprintf( szString , "WF.lx" ); break; // 2005.03.29
											case 10 : sprintf( szString , "AF.lx" ); break; // 2005.03.29
											case 11 : sprintf( szString , "PF.A1" ); break; // 2006.01.26
											case 12 : sprintf( szString , "WF.A1" ); break; // 2006.01.26
											case 13 : sprintf( szString , "AF.A1" ); break; // 2006.01.26
											case 14 : sprintf( szString , "PF.A1G0" ); break; // 2006.02.08
											case 15 : sprintf( szString , "WF.A1G0" ); break; // 2006.02.08
											case 16 : sprintf( szString , "AF.A1G0" ); break; // 2006.02.08
											case 17 : sprintf( szString , "CF" ); break; // 2006.07.13
											case 18 : sprintf( szString , "DSWITCH" ); break; // 2007.12.12
											case 101: sprintf( szString , "UD001" ); break; // 2006.04.15
											case 102: sprintf( szString , "UD002" ); break; // 2006.04.15
											}
											//--------------------------------------------------------------------
											// 2006.07.12
											//--------------------------------------------------------------------
											switch ( TEMP_READY_USE52[ i-MAX_CASSETTE_SIDE+BM1 ] ) {
											case 1 :
												if ( TEMP_READY_USE5[ i-MAX_CASSETTE_SIDE+BM1 ] == 0 ) sprintf( szString , "-" );
												strcat( szString , "|PF" );
												break;
											case 2 :
												if ( TEMP_READY_USE5[ i-MAX_CASSETTE_SIDE+BM1 ] == 0 ) sprintf( szString , "-" );
												strcat( szString , "|WF" );
												break;
											case 3 :
												if ( TEMP_READY_USE5[ i-MAX_CASSETTE_SIDE+BM1 ] == 0 ) sprintf( szString , "-" );
												strcat( szString , "|AF" );
												break;
											}
											//--------------------------------------------------------------------
										}
										else {
											sprintf( szString , "" );
										}
									}
									break;
								case 3 :
									if ( i < MAX_CASSETTE_SIDE ) {
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) )	{
											if ( TEMP_READY_USE6[ i+CM1 ] == 0 ) {
												sprintf( szString , "-" );
											}
											else {
												sprintf( szString , "BM%d" , TEMP_READY_USE6[ i+CM1 ] - BM1 + 1 );
												switch( TEMP_READY_USE7[ i+CM1 ] ) {
												case 1 :	strcat( szString , ":DIRECT.ERROR" );	break;
												case 2 :	strcat( szString , ":DIRECT.CM" );		break;
												case 3 :	strcat( szString , ":ORDER.ERROR" );	break;
												case 4 :	strcat( szString , ":ORDER.CM" );		break;
												default :	strcat( szString , ":NOTUSE" );			break;
												}
												switch( TEMP_READY_USE8[ i+CM1 ] ) {
												case 1 :	strcat( szString , ":IO" );		break;
												case 2 :	strcat( szString , ":SIGNAL" );	break;
												default :	strcat( szString , ":ALL" );	break;
												}
											}
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										sprintf( szString , "" );
									}
									break;
								}
								//
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								//
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								if ( lpdi->item.iItem < MAX_CASSETTE_SIDE ) {
									wsprintf( szString, "CM%d", lpdi->item.iItem + 1 );
									//
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
									//
								}
								else {
									wsprintf( szString, "BM%d", lpdi->item.iItem - MAX_CASSETTE_SIDE + 1 );
									//
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
									//
								}
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
										j = _i_SCH_SDM_Get_RUN_CHAMBER(i);
										if ( j == 0 ) {
											sprintf( szString , "-" );
										}
										else {
											sprintf( szString , "PM%d" , j - PM1 + 1 );
										}
										break;
								case 2 :
										strncpy( szString , _i_SCH_SDM_Get_RECIPE( 2 , i%100 , SDM_DEFAULT_STYLE , 0 ) , 255 );
										break;
								case 3 :
										strncpy( szString , _i_SCH_SDM_Get_RECIPE( 0 , i%100 , SDM_DEFAULT_STYLE , 0 ) , 255 );
										break;
								case 4 :
										strncpy( szString , _i_SCH_SDM_Get_RECIPE( 2 , i%100 , SDM_DEFAULT_STYLE , 1 ) , 255 );
										break;
								case 5 :
										strncpy( szString , _i_SCH_SDM_Get_RECIPE( 0 , i%100 , SDM_DEFAULT_STYLE , 1 ) , 255 );
										break;
								case 6 :
										strncpy( szString , _i_SCH_SDM_Get_RECIPE( 2 , i%100 , SDM_DEFAULT_STYLE , 2 ) , 255 );
										break;
								case 7 :
										strncpy( szString , _i_SCH_SDM_Get_RECIPE( 0 , i%100 , SDM_DEFAULT_STYLE , 2 ) , 255 );
										break;
								case 8 :
										sprintf( szString , "%d" , _i_SCH_SDM_Get_RUN_USECOUNT( i%100 ) );
										break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "%d" , lpdi->item.iItem + 1 );
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;							
								wsprintf( szString, "" );
								switch( lpdi->item.iSubItem ) {
								case 1 :
										if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
											if      ( TEMP_READY_USE_PrRv[ FM ] == 1 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PrRv[ FM ] == 2 ) wsprintf( szString, "B(X)" );
											else if ( TEMP_READY_USE_PrRv[ FM ] == 3 ) wsprintf( szString, "A(X)" );
											else                                       wsprintf( szString, "" );
										}
										else if ( i < MAX_TM_CHAMBER_DEPTH ) {
											if ( TEMP_READY_USE_PrRv[ i + TM ] ) wsprintf( szString, "X" );
										}
										else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
											if ( TEMP_READY_USE_PrRv[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] ) wsprintf( szString, "X" );
										}
										else {
											if      ( TEMP_READY_USE_PrRv[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 1 ) wsprintf( szString, "T(X)" );
											else if ( TEMP_READY_USE_PrRv[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 2 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PrRv[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 3 ) wsprintf( szString, "A(X)" );
										}
										break;
								case 2 :
										if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
											if      ( TEMP_READY_USE_PoRv[ FM ] == 1 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PoRv[ FM ] == 2 ) wsprintf( szString, "B(X)" );
											else if ( TEMP_READY_USE_PoRv[ FM ] == 3 ) wsprintf( szString, "A(X)" );
											else                                       wsprintf( szString, "" );
										}
										else if ( i < MAX_TM_CHAMBER_DEPTH ) {
											if ( TEMP_READY_USE_PoRv[ i + TM ] ) wsprintf( szString, "X" );
										}
										else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
											if ( TEMP_READY_USE_PoRv[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] ) wsprintf( szString, "X" );
										}
										else {
											if      ( TEMP_READY_USE_PoRv[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 1 ) wsprintf( szString, "T(X)" );
											else if ( TEMP_READY_USE_PoRv[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 2 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PoRv[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 3 ) wsprintf( szString, "A(X)" );
										}
										break;
								case 3 :
										if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
											if      ( TEMP_READY_USE_PrSn[ FM ] == 1 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PrSn[ FM ] == 2 ) wsprintf( szString, "B(X)" );
											else if ( TEMP_READY_USE_PrSn[ FM ] == 3 ) wsprintf( szString, "A(X)" );
											else                                       wsprintf( szString, "" );
										}
										else if ( i < MAX_TM_CHAMBER_DEPTH ) {
											if ( TEMP_READY_USE_PrSn[ i + TM ] ) wsprintf( szString, "X" );
										}
										else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
											if ( TEMP_READY_USE_PrSn[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] ) wsprintf( szString, "X" );
										}
										else {
											if      ( TEMP_READY_USE_PrSn[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 1 ) wsprintf( szString, "T(X)" );
											else if ( TEMP_READY_USE_PrSn[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 2 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PrSn[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 3 ) wsprintf( szString, "A(X)" );
										}
										break;
								case 4 :
										if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
											if      ( TEMP_READY_USE_PoSn[ FM ] == 1 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PoSn[ FM ] == 2 ) wsprintf( szString, "B(X)" );
											else if ( TEMP_READY_USE_PoSn[ FM ] == 3 ) wsprintf( szString, "A(X)" );
											else                                       wsprintf( szString, "" );
										}
										else if ( i < MAX_TM_CHAMBER_DEPTH ) {
											if ( TEMP_READY_USE_PoSn[ i + TM ] ) wsprintf( szString, "X" );
										}
										else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
											if ( TEMP_READY_USE_PoSn[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] ) wsprintf( szString, "X" );
										}
										else {
											if      ( TEMP_READY_USE_PoSn[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 1 ) wsprintf( szString, "T(X)" );
											else if ( TEMP_READY_USE_PoSn[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 2 ) wsprintf( szString, "F(X)" );
											else if ( TEMP_READY_USE_PoSn[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 3 ) wsprintf( szString, "A(X)" );
										}
										break;
								case 5 :
//										if ( _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) { // 2006.11.01
//											wsprintf( szString, "x" ); // 2006.11.01
//										} // 2006.11.01
//										else { // 2006.11.01
											if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
												if      ( TEMP_READY_USE_Gate[ FM ] == 1 ) wsprintf( szString, "F(X)" );
												else if ( TEMP_READY_USE_Gate[ FM ] == 2 ) wsprintf( szString, "B(X)" );
												else if ( TEMP_READY_USE_Gate[ FM ] == 3 ) wsprintf( szString, "A(X)" );
												else                                       wsprintf( szString, "" );
											}
											else if ( i < MAX_TM_CHAMBER_DEPTH ) {
												if      ( TEMP_READY_USE_Gate[ i + TM ] == 1 ) wsprintf( szString, "X" );
												else if ( TEMP_READY_USE_Gate[ i + TM ] == 2 ) wsprintf( szString, "O2" );
												else                                           wsprintf( szString, "" );
											}
											else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
												if      ( TEMP_READY_USE_Gate[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] == 1 ) wsprintf( szString, "X" );
												else if ( TEMP_READY_USE_Gate[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] == 2 ) wsprintf( szString, "O2" );
												else																	   wsprintf( szString, "" );
											}
											else {
												if      ( TEMP_READY_USE_Gate[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 1 ) wsprintf( szString, "X" );
												else if ( TEMP_READY_USE_Gate[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 2 ) wsprintf( szString, "O2" );
												else																								  wsprintf( szString, "" );
											}
//										} // 2006.11.01
										break;
										//
								case 6 : // 2016.11.24
										if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) {
											if      ( TEMP_READY_USE_Cancel[ FM ] == 1 ) wsprintf( szString, "X" );
											else if ( TEMP_READY_USE_Cancel[ FM ] == 2 ) wsprintf( szString, "O" );
											else									     wsprintf( szString, "" );
										}
										else if ( i < MAX_TM_CHAMBER_DEPTH ) {
											if      ( TEMP_READY_USE_Cancel[ i + TM ] == 1 ) wsprintf( szString, "X" );
											else if ( TEMP_READY_USE_Cancel[ i + TM ] == 2 ) wsprintf( szString, "O" );
											else                                             wsprintf( szString, "" );
										}
										else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
											if      ( TEMP_READY_USE_Cancel[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] == 1 ) wsprintf( szString, "X" );
											else if ( TEMP_READY_USE_Cancel[ ( i - MAX_TM_CHAMBER_DEPTH ) + PM1 ] == 2 ) wsprintf( szString, "O" );
											else																	     wsprintf( szString, "" );
										}
										else {
											if      ( TEMP_READY_USE_Cancel[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 1 ) wsprintf( szString, "X" );
											else if ( TEMP_READY_USE_Cancel[ ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] == 2 ) wsprintf( szString, "O" );
											else																								    wsprintf( szString, "" );
										}
										break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								if ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
									sprintf( szString , "FM" );
								}
								else if ( i < MAX_TM_CHAMBER_DEPTH ) {
									sprintf( szString , "TM%d" , i + 1 );
								}
								else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
									sprintf( szString , "PM%d" , ( i - MAX_TM_CHAMBER_DEPTH ) + 1 );
								}
								else {
									sprintf( szString , "BM%d" , ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + 1 );
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					//
					strcpy( lpdi->item.pszText , szString ); // 2016.02.02
					//
				}
				return 0;

			case NM_DBLCLK :
				if ( Control ) return TRUE;
				if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
						MODULE_SUB_RESULT  = FALSE;
						MODULE_SUB_ROTANIM = TEMP_READY_USE[i+PM1];
						if ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() > 0 ) { 
							if ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() == ( i + PM1 ) ) {
								MODULE_SUB_DUMMY_MODE = 1;
								MODULE_SUB_DUMMY_DATA = TEMP_READY_USE3;
							}
							else {
								MODULE_SUB_DUMMY_MODE = 2;
								MODULE_SUB_DUMMY_DATA = TEMP_READY_USE2[i+PM1];
							}
						}
						else {
							MODULE_SUB_DUMMY_MODE = 0;
						}
						MODULE_SUB_DUMMY_DATA2 = TEMP_READY_USE4[i+PM1];
						MODULE_SUB_DUMMY_DATA3 = TEMP_READY_USE5[i+PM1];
						MODULE_SUB_DUMMY_DATA4 = TEMP_READY_USE32;
						MODULE_SUB_DUMMY_DATA5 = TEMP_READY_USE33;
						MODULE_SUB_DUMMY_DATA6 = TEMP_READY_USE52[i+PM1]; // 2006.07.12
						strncpy( MODULE_SUB_PRCSNAME , TEMP_READY_RECIPE[i+PM1] , 63 );
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_MODULE_SUB_PAD ) , hSWnd , Gui_IDD_MODULE_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( MODULE_SUB_RESULT ) {
							TEMP_READY_USE[i+PM1] = MODULE_SUB_ROTANIM;
							switch ( MODULE_SUB_DUMMY_MODE ) {
							case 1 : TEMP_READY_USE3 = MODULE_SUB_DUMMY_DATA; break;
							case 2 : TEMP_READY_USE2[i+PM1] = MODULE_SUB_DUMMY_DATA; break;
							}
							TEMP_READY_USE4[i+PM1]  = MODULE_SUB_DUMMY_DATA2;
							TEMP_READY_USE5[i+PM1]  = MODULE_SUB_DUMMY_DATA3;
							TEMP_READY_USE32        = MODULE_SUB_DUMMY_DATA4;
							TEMP_READY_USE33        = MODULE_SUB_DUMMY_DATA5;
							TEMP_READY_USE52[i+PM1] = MODULE_SUB_DUMMY_DATA6; // 2006.07.12
							strncpy( TEMP_READY_RECIPE[i+PM1] , MODULE_SUB_PRCSNAME , 63 );
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if ( i < MAX_CASSETTE_SIDE ) {
						if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
							MODULE_SUB_RESULT  = FALSE;
							MODULE_SUB_ROTANIM = TEMP_READY_USE[i+CM1];
							MODULE_SUB_DUMMY_DATA  = TEMP_READY_USE5[i+CM1];
							MODULE_SUB_DUMMY_DATA2 = TEMP_READY_USE6[i+CM1];
							MODULE_SUB_DUMMY_DATA3 = TEMP_READY_USE7[i+CM1];
							MODULE_SUB_DUMMY_DATA4 = TEMP_READY_USE8[i+CM1];
							MODULE_SUB_DUMMY_DATA6 = TEMP_READY_USE52[i+CM1]; // 2006.07.12
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_MODULE_SUB_PAD3 ) , hSWnd , Gui_IDD_MODULE_SUB_PAD_CM_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							if ( MODULE_SUB_RESULT ) {
								TEMP_READY_USE[i+CM1] = MODULE_SUB_ROTANIM;
								TEMP_READY_USE5[i+CM1] = MODULE_SUB_DUMMY_DATA;
								TEMP_READY_USE6[i+CM1] = MODULE_SUB_DUMMY_DATA2;
								TEMP_READY_USE7[i+CM1] = MODULE_SUB_DUMMY_DATA3;
								TEMP_READY_USE8[i+CM1] = MODULE_SUB_DUMMY_DATA4;
								TEMP_READY_USE52[i+CM1] = MODULE_SUB_DUMMY_DATA6; // 2006.07.12
								InvalidateRect( hdlg , NULL , FALSE );
							}
						}
					}
					else {
						if  ( _i_SCH_PRM_GET_MODULE_MODE(i-MAX_CASSETTE_SIDE+BM1) ) {
							MODULE_SUB_RESULT  = FALSE;
							MODULE_SUB_ROTANIM = TEMP_READY_USE[i-MAX_CASSETTE_SIDE+BM1];
							MODULE_SUB_DUMMY_DATA = TEMP_READY_USE5[i-MAX_CASSETTE_SIDE+BM1];
							MODULE_SUB_DUMMY_DATA6 = TEMP_READY_USE52[i-MAX_CASSETTE_SIDE+BM1]; // 2006.07.12
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_MODULE_SUB_PAD2 ) , hSWnd , Gui_IDD_MODULE_SUB_PAD_BM_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							if ( MODULE_SUB_RESULT ) {
								TEMP_READY_USE[i-MAX_CASSETTE_SIDE+BM1] = MODULE_SUB_ROTANIM;
								TEMP_READY_USE5[i-MAX_CASSETTE_SIDE+BM1] = MODULE_SUB_DUMMY_DATA;
								TEMP_READY_USE52[i-MAX_CASSETTE_SIDE+BM1] = MODULE_SUB_DUMMY_DATA6; // 2006.07.12
								InvalidateRect( hdlg , NULL , FALSE );
							}
						}
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == 0 ) return 0;
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					j = -1;
					if ( !MODAL_DIGITAL_BOX1( hdlg , "Select Action?" , "Select" , "Change Control or Chamber|Change Pre Recipe|Change Run Recipe|Change Pre Recipe(First)|Change Run Recipe(First)|Change Pre Recipe(End)|Change Run Recipe(End)|Reset Run Count" , &j ) ) return 0;
					switch( j ) {
					case 0 :
						Res = 0;
						if ( MODAL_DIGITAL_BOX2( hdlg , "Path Process" , "Select" , 0 , MAX_PM_CHAMBER_DEPTH , &Res ) ) {
							if ( Res != 0 ) {
								Res = Res + PM1 - 1;
							}
							SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_MODULE( i , Res );
							InvalidateRect( hdlg , NULL , TRUE );
						}
						break;
					case 1 :
						Res = _i_SCH_SDM_Get_RUN_CHAMBER( i );
						if ( Res != 0 ) {
							if ( GUI_GROUP_SELECT_GET() == 0 )
								sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							else
								sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							//
							strncpy( Buffer , _i_SCH_SDM_Get_RECIPE( 2 , i%100 , SDM_DEFAULT_STYLE , 0 ) , 255 );
							//
							if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Pre Recipe Name" , "*.*" , "" , Buffer ) ) {
								if ( GUI_GROUP_SELECT_GET() == 2 ) {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
									UTIL_MAKE_DYNAMIC_GROUP_FILE( bufferdir , Buffer , 64 );
								}
								else {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
								}
								SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( i , SDM_DEFAULT_STYLE , 0 , bufferdir );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						break;
					case 2 :
						Res = _i_SCH_SDM_Get_RUN_CHAMBER( i );
						if ( Res != 0 ) {
							if ( GUI_GROUP_SELECT_GET() == 0 )
								sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							else
								sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							//
							strncpy( Buffer , _i_SCH_SDM_Get_RECIPE( 0 , i%100 , SDM_DEFAULT_STYLE , 0 ) , 255 );
							//
							if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Run Recipe Name" , "*.*" , "" , Buffer ) ) {
								if ( GUI_GROUP_SELECT_GET() == 2 ) {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
									UTIL_MAKE_DYNAMIC_GROUP_FILE( bufferdir , Buffer , 64 );
								}
								else {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
								}
								SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( i , SDM_DEFAULT_STYLE , 0 , bufferdir );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						break;
					case 3 :
						Res = _i_SCH_SDM_Get_RUN_CHAMBER( i );
						if ( Res != 0 ) {
							if ( GUI_GROUP_SELECT_GET() == 0 )
								sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							else
								sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							//
							strncpy( Buffer , _i_SCH_SDM_Get_RECIPE( 2 , i%100 , SDM_DEFAULT_STYLE , 1 ) , 255 );
							//
							if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Pre Recipe(First) Name" , "*.*" , "" , Buffer ) ) {
								if ( GUI_GROUP_SELECT_GET() == 2 ) {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
									UTIL_MAKE_DYNAMIC_GROUP_FILE( bufferdir , Buffer , 64 );
								}
								else {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
								}
								SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( i , SDM_DEFAULT_STYLE , 1 , bufferdir );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						break;
					case 4 :
						Res = _i_SCH_SDM_Get_RUN_CHAMBER( i );
						if ( Res != 0 ) {
							if ( GUI_GROUP_SELECT_GET() == 0 )
								sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							else
								sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							//
							strncpy( Buffer , _i_SCH_SDM_Get_RECIPE( 0 , i%100 , SDM_DEFAULT_STYLE , 1 ) , 255 );
							//
							if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Run Recipe(First) Name" , "*.*" , "" , Buffer ) ) {
								if ( GUI_GROUP_SELECT_GET() == 2 ) {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
									UTIL_MAKE_DYNAMIC_GROUP_FILE( bufferdir , Buffer , 64 );
								}
								else {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
								}
								SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( i , SDM_DEFAULT_STYLE , 1 , bufferdir );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						break;
					case 5 :
						Res = _i_SCH_SDM_Get_RUN_CHAMBER( i );
						if ( Res != 0 ) {
							if ( GUI_GROUP_SELECT_GET() == 0 )
								sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							else
								sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							//
							strncpy( Buffer , _i_SCH_SDM_Get_RECIPE( 2 , i%100 , SDM_DEFAULT_STYLE , 2 ) , 255 );
							//
							if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Pre Recipe(End) Name" , "*.*" , "" , Buffer ) ) {
								if ( GUI_GROUP_SELECT_GET() == 2 ) {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
									UTIL_MAKE_DYNAMIC_GROUP_FILE( bufferdir , Buffer , 64 );
								}
								else {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
								}
								SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_PRE_RECIPE( i , SDM_DEFAULT_STYLE , 2 , bufferdir );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						break;
					case 6 :
						Res = _i_SCH_SDM_Get_RUN_CHAMBER( i );
						if ( Res != 0 ) {
							if ( GUI_GROUP_SELECT_GET() == 0 )
								sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							else
								sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( Res ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( Res ) );
							//
							strncpy( Buffer , _i_SCH_SDM_Get_RECIPE( 0 , i%100 , SDM_DEFAULT_STYLE , 2 ) , 255 );
							//
							if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Run Recipe(End) Name" , "*.*" , "" , Buffer ) ) {
								if ( GUI_GROUP_SELECT_GET() == 2 ) {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
									UTIL_MAKE_DYNAMIC_GROUP_FILE( bufferdir , Buffer , 64 );
								}
								else {
									Ext_Name_From_FullFile( bufferdir , Buffer , 255 );
								}
								SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RUN_RECIPE( i , SDM_DEFAULT_STYLE , 2 , bufferdir );
								InvalidateRect( hdlg , NULL , TRUE );
							}
						}
						break;
					case 7 :
						_i_SCH_SDM_Set_CHAMBER_RESET_COUNT( i ); // 2012.02.15
//						SCHEDULER_CONTROL_Set_SDM_ins_CHAMER_RESET_COUNT( i ); // 2012.02.15
						InvalidateRect( hdlg , NULL , TRUE );
						break;
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;

					if      ( i == ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) { // 2005.12.27
						i = FM;
						MODULE_SUB_ROTANIM = 1; // 2005.12.27
					}
					else if ( i < MAX_TM_CHAMBER_DEPTH ) {
						i = i + TM;
						MODULE_SUB_ROTANIM = 0; // 2005.12.27
					}
					else if ( i < ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) {
						i = ( i - MAX_TM_CHAMBER_DEPTH ) + PM1;
						MODULE_SUB_ROTANIM = 0; // 2005.12.27
					}
					else {
						i = ( i - ( MAX_TM_CHAMBER_DEPTH + MAX_PM_CHAMBER_DEPTH ) ) + BM1;
						MODULE_SUB_ROTANIM = 2; // 2007.06.22
					}
					if ( _i_SCH_PRM_GET_MODULE_MODE(i) ) {
						MODULE_SUB_RESULT  = FALSE;
						MODULE_SUB_DUMMY_MODE = TEMP_READY_USE_PrRv[i];
						MODULE_SUB_DUMMY_DATA = TEMP_READY_USE_PoRv[i];
						MODULE_SUB_DUMMY_DATA2 = TEMP_READY_USE_PrSn[i];
						MODULE_SUB_DUMMY_DATA3 = TEMP_READY_USE_PoSn[i];
						MODULE_SUB_DUMMY_DATA4 = TEMP_READY_USE_Gate[i];
						MODULE_SUB_DUMMY_DATA5 = TEMP_READY_USE_Cancel[i]; // 2016.11.24
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_MODULE_SUB_PAD4 ) , hSWnd , Gui_IDD_MODULE_SUB_PAD4_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( MODULE_SUB_RESULT ) {
							TEMP_READY_USE_PrRv[i] = MODULE_SUB_DUMMY_MODE;
							TEMP_READY_USE_PoRv[i] = MODULE_SUB_DUMMY_DATA;
							TEMP_READY_USE_PrSn[i] = MODULE_SUB_DUMMY_DATA2;
							TEMP_READY_USE_PoSn[i] = MODULE_SUB_DUMMY_DATA3;
							TEMP_READY_USE_Gate[i] = MODULE_SUB_DUMMY_DATA4;
							TEMP_READY_USE_Cancel[i] = MODULE_SUB_DUMMY_DATA5; // 2016.11.24
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
				}
				return 0;
			case LVN_ITEMCHANGED :
				if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
					InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , NULL , TRUE );
				}
				return 0;
			case LVN_KEYDOWN :
				if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
					InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , NULL , TRUE );
				}
				return 0;
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		if ( Modify_Sdm ) {
			KWIN_Item_Enable( hdlg , IDOK );
		}
		else {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) != TEMP_READY_USE5[ i ] ) break;
			}
			if ( i == MAX_CHAMBER ) {
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
					if ( _i_SCH_PRM_GET_ANIMATION_ROTATE_PREPARE( i ) != TEMP_READY_USE[ i ] ) break;
					if ( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE( i ) != TEMP_READY_USE6[ i ] ) break;
					if ( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( i ) != TEMP_READY_USE7[ i ] ) break;
					if ( _i_SCH_PRM_GET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( i ) != TEMP_READY_USE8[ i ] ) break;
					//
					if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( i ) != TEMP_READY_USE_PrRv[ i ] ) break;
					if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( i ) != TEMP_READY_USE_PrSn[ i ] ) break;
					if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( i ) != TEMP_READY_USE_PoRv[ i ] ) break;
					if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( i ) != TEMP_READY_USE_PoSn[ i ] ) break;
					if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( i ) != TEMP_READY_USE_Gate[ i ] ) break;
					//
					if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( i ) != TEMP_READY_USE_Cancel[ i ] ) break; // 2016.11.24
				}
				if ( i == MAX_CHAMBER ) {
					for ( i = PM1 ; i < TM ; i++ ) {
						if ( _i_SCH_PRM_GET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( i ) != TEMP_READY_USE2[ i ] ) break;
						if ( _i_SCH_PRM_GET_PRE_RECEIVE_WITH_PROCESS_RECIPE( i ) != TEMP_READY_USE4[ i ] ) break;
						if ( _i_SCH_PRM_GET_NEXT_FREE_PICK_POSSIBLE( i ) != TEMP_READY_USE5[ i ] ) break;
						if ( _i_SCH_PRM_GET_PREV_FREE_PICK_POSSIBLE( i ) != TEMP_READY_USE52[ i ] ) break; // 2006.07.12
						if ( !STRCMP_L( _i_SCH_PRM_GET_MODULE_PROCESS_NAME( i ) , TEMP_READY_RECIPE[ i ] ) ) break;
					}
					if ( i >= TM ) {
						if ( TEMP_READY_USE3 == _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() ) {
							if ( TEMP_READY_USE32 == _i_SCH_PRM_GET_DUMMY_PROCESS_FIXEDCLUSTER() ) {
								if ( TEMP_READY_USE33 == _i_SCH_PRM_GET_DUMMY_PROCESS_MULTI_LOT_ACCESS() ) { // 2003.02.08
									KWIN_Item_Disable( hdlg , IDOK );
								}
								else {
									KWIN_Item_Enable( hdlg , IDOK );
								}
							}
							else {
								KWIN_Item_Enable( hdlg , IDOK );
							}
						}
						else {
							KWIN_Item_Enable( hdlg , IDOK );
						}
					}
					else {
						KWIN_Item_Enable( hdlg , IDOK );
					}
				}
				else {
					KWIN_Item_Enable( hdlg , IDOK );
				}
			}
			else {
				KWIN_Item_Enable( hdlg , IDOK );
			}
		}
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDYES :
			if ( Control ) return TRUE;
			//---------------------------------------------------------------
			InvalidateRect( hdlg , NULL , TRUE );
			return TRUE;

	case IDOK :
			if ( Control ) return TRUE;
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				_i_SCH_PRM_SET_ANIMATION_ROTATE_PREPARE( i , TEMP_READY_USE[ i ] );
				_i_SCH_PRM_SET_NEXT_FREE_PICK_POSSIBLE( i , TEMP_READY_USE5[ i ] );
				_i_SCH_PRM_SET_PREV_FREE_PICK_POSSIBLE( i , TEMP_READY_USE52[ i ] ); // 2006.07.12
				_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE( i , TEMP_READY_USE6[ i ] );
				_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_DATA( i , TEMP_READY_USE7[ i ] );
				_i_SCH_PRM_SET_ERROR_OUT_CHAMBER_FOR_CASSETTE_CHECK( i , TEMP_READY_USE8[ i ] );
				//
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_PREPRECV( i , TEMP_READY_USE_PrRv[ i ] );
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_PREPSEND( i , TEMP_READY_USE_PrSn[ i ] );
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_POSTRECV( i , TEMP_READY_USE_PoRv[ i ] );
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_POSTSEND( i , TEMP_READY_USE_PoSn[ i ] );
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_GATE( i , TEMP_READY_USE_Gate[ i ] );
				//
				_i_SCH_PRM_SET_MODULE_MESSAGE_NOTUSE_CANCEL( i , TEMP_READY_USE_Cancel[ i ] ); // 2016.11.24
			}
			//
			for ( i = PM1 ; i < TM ; i++ ) {
				_i_SCH_PRM_SET_MODULE_PROCESS_NAME( i , TEMP_READY_RECIPE[ i ] );
				_i_SCH_PRM_SET_DUMMY_PROCESS_NOT_DEPAND_CHAMBER( i , TEMP_READY_USE2[ i ] );
				_i_SCH_PRM_SET_PRE_RECEIVE_WITH_PROCESS_RECIPE( i , TEMP_READY_USE4[ i ] );
			}
			//
			_i_SCH_PRM_SET_DUMMY_PROCESS_MODE( TEMP_READY_USE3 );
			_i_SCH_PRM_SET_DUMMY_PROCESS_FIXEDCLUSTER( TEMP_READY_USE32 );
			_i_SCH_PRM_SET_DUMMY_PROCESS_MULTI_LOT_ACCESS( TEMP_READY_USE33 ); // 2003.02.08
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			EndDialog( hdlg , 0 );
			return TRUE;
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
