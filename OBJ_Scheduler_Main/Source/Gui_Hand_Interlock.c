#include "default.h"

#include "EQ_Enum.h"

#include "System_Tag.h"
#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL CHINTERLOCK_RESULT = FALSE;
int  CHINTERLOCK_DATA[MAX_CASSETTE_SIDE];
int  CHINTERLOCK_DATA2[MAX_TM_CHAMBER_DEPTH];
int  CHINTERLOCK_DATA3; // 2012.09.21
//------------------------------------------------------------------------------------------
/*
// 2012.09.21
BOOL APIENTRY Gui_IDD_CHINTERLOCK_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int mode;
	PAINTSTRUCT ps;
	switch ( msg ) {
	case WM_INITDIALOG :
		mode = (int) lParam;
		if ( mode ) {
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
		}
		CHINTERLOCK_RESULT = FALSE;
		if ( MAX_TM_CHAMBER_DEPTH < 1 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T1 );
		if ( MAX_TM_CHAMBER_DEPTH < 2 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T2 );
		if ( MAX_TM_CHAMBER_DEPTH < 3 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T3 );
		if ( MAX_TM_CHAMBER_DEPTH < 4 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T4 );
		if ( MAX_TM_CHAMBER_DEPTH < 5 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T5 );
		if ( MAX_TM_CHAMBER_DEPTH < 6 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T6 );
		if ( MAX_TM_CHAMBER_DEPTH < 7 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T7 );
		if ( MAX_TM_CHAMBER_DEPTH < 8 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T8 );
		if ( MAX_TM_CHAMBER_DEPTH < 9 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T9 );
		return TRUE;
	case WM_PAINT :
		BeginPaint( hdlg, &ps );
		if ( !mode ) {
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] , "Enable|Disable" , "Enable" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , CHINTERLOCK_DATA[1] , "Enable|Disable" , "Enable" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , CHINTERLOCK_DATA[2] , "Enable|Disable" , "Enable" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , CHINTERLOCK_DATA[3] , "Enable|Disable" , "Enable" ); // 2007.07.24
		}
		//
		if ( MAX_TM_CHAMBER_DEPTH >= 1 ) {
			if ( CHINTERLOCK_DATA2[0] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T1 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T1 , CHINTERLOCK_DATA2[0] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 2 ) {
			if ( CHINTERLOCK_DATA2[1] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T2 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T2 , CHINTERLOCK_DATA2[1] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 3 ) {
			if ( CHINTERLOCK_DATA2[2] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T3 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T3 , CHINTERLOCK_DATA2[2] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 4 ) {
			if ( CHINTERLOCK_DATA2[3] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T4 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T4 , CHINTERLOCK_DATA2[3] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 5 ) {
			if ( CHINTERLOCK_DATA2[4] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T5 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T5 , CHINTERLOCK_DATA2[4] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 6 ) {
			if ( CHINTERLOCK_DATA2[5] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T6 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T6 , CHINTERLOCK_DATA2[5] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 7 ) {
			if ( CHINTERLOCK_DATA2[6] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T7 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T7 , CHINTERLOCK_DATA2[6] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 8 ) {
			if ( CHINTERLOCK_DATA2[7] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T8 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T8 , CHINTERLOCK_DATA2[7] );
		}
		if ( MAX_TM_CHAMBER_DEPTH >= 9 ) {
			if ( CHINTERLOCK_DATA2[8] <= 0 )
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_T9 , "" );
			else
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T9 , CHINTERLOCK_DATA2[8] );
		}
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			KWIN_GUI_SELECT_DATA_INCREASE( mode , hdlg , 1 , &(CHINTERLOCK_DATA[0]) );
			return TRUE;
		case IDC_READY_USE_B2 :
			KWIN_GUI_SELECT_DATA_INCREASE( mode , hdlg , 1 , &(CHINTERLOCK_DATA[1]) );
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_SELECT_DATA_INCREASE( mode , hdlg , 1 , &(CHINTERLOCK_DATA[2]) );
			return TRUE;
		case IDC_READY_USE_B4 :
			KWIN_GUI_SELECT_DATA_INCREASE( mode , hdlg , 1 , &(CHINTERLOCK_DATA[3]) );
			return TRUE;

		case IDC_READY_USE_T1 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[0]) );
			return TRUE;
		case IDC_READY_USE_T2 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM2 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[1]) );
			return TRUE;
		case IDC_READY_USE_T3 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM3 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[2]) );
			return TRUE;
		case IDC_READY_USE_T4 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM4 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[3]) );
			return TRUE;
		case IDC_READY_USE_T5 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM5 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[4]) );
			return TRUE;
		case IDC_READY_USE_T6 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM6 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[5]) );
			return TRUE;
		case IDC_READY_USE_T7 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM7 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[6]) );
			return TRUE;
		case IDC_READY_USE_T8 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM8 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[7]) );
			return TRUE;
		case IDC_READY_USE_T9 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM9 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[8]) );
			return TRUE;

		case IDOK :
			CHINTERLOCK_RESULT = TRUE;
			EndDialog( hdlg , 0 );
			return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
*/

// 2012.09.21
BOOL APIENTRY Gui_IDD_CHINTERLOCK_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int mode;
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
		mode = (int) lParam;
		switch( mode ) {
		case 1 : // bm
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
			//
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T4 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T5 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T6 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T7 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T8 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_T9 );
			break;
		case 2 :
		case 3 :
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );

			KWIN_Item_Hide( hdlg , IDC_READY_USE_P1 );
			break;
		}
		//
		if ( mode != 1 ) {
			if ( MAX_TM_CHAMBER_DEPTH < 1 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T1 );
			if ( MAX_TM_CHAMBER_DEPTH < 2 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T2 );
			if ( MAX_TM_CHAMBER_DEPTH < 3 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T3 );
			if ( MAX_TM_CHAMBER_DEPTH < 4 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T4 );
			if ( MAX_TM_CHAMBER_DEPTH < 5 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T5 );
			if ( MAX_TM_CHAMBER_DEPTH < 6 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T6 );
			if ( MAX_TM_CHAMBER_DEPTH < 7 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T7 );
			if ( MAX_TM_CHAMBER_DEPTH < 8 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T8 );
			if ( MAX_TM_CHAMBER_DEPTH < 9 ) KWIN_Item_Hide( hdlg , IDC_READY_USE_T9 );
		}
		//
		CHINTERLOCK_RESULT = FALSE;
		//
		return TRUE;
	case WM_PAINT :
		BeginPaint( hdlg, &ps );
		//
		if ( mode == 0 ) {
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] , "Enable|Disable" , "Enable" );
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , CHINTERLOCK_DATA[1] , "Enable|Disable" , "Enable" );
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , CHINTERLOCK_DATA[2] , "Enable|Disable" , "Enable" );
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 , CHINTERLOCK_DATA[3] , "Enable|Disable" , "Enable" );
		}
		//
		if ( mode == 3 ) {
			if ( MAX_TM_CHAMBER_DEPTH >= 1 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T1 , CHINTERLOCK_DATA2[0] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 2 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T2 , CHINTERLOCK_DATA2[1] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 3 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T3 , CHINTERLOCK_DATA2[2] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 4 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T4 , CHINTERLOCK_DATA2[3] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 5 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T5 , CHINTERLOCK_DATA2[4] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 6 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T6 , CHINTERLOCK_DATA2[5] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 7 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T7 , CHINTERLOCK_DATA2[6] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 8 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T8 , CHINTERLOCK_DATA2[7] , "NotUse|Last|First" , "NotUse" );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 9 ) {
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_T9 , CHINTERLOCK_DATA2[8] , "NotUse|Last|First" , "NotUse" );
			}
		}
		else if ( mode != 1 ) {
			if ( MAX_TM_CHAMBER_DEPTH >= 1 ) {
				if ( CHINTERLOCK_DATA2[0] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T1 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T1 , CHINTERLOCK_DATA2[0] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 2 ) {
				if ( CHINTERLOCK_DATA2[1] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T2 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T2 , CHINTERLOCK_DATA2[1] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 3 ) {
				if ( CHINTERLOCK_DATA2[2] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T3 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T3 , CHINTERLOCK_DATA2[2] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 4 ) {
				if ( CHINTERLOCK_DATA2[3] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T4 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T4 , CHINTERLOCK_DATA2[3] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 5 ) {
				if ( CHINTERLOCK_DATA2[4] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T5 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T5 , CHINTERLOCK_DATA2[4] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 6 ) {
				if ( CHINTERLOCK_DATA2[5] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T6 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T6 , CHINTERLOCK_DATA2[5] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 7 ) {
				if ( CHINTERLOCK_DATA2[6] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T7 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T7 , CHINTERLOCK_DATA2[6] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 8 ) {
				if ( CHINTERLOCK_DATA2[7] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T8 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T8 , CHINTERLOCK_DATA2[7] );
			}
			if ( MAX_TM_CHAMBER_DEPTH >= 9 ) {
				if ( CHINTERLOCK_DATA2[8] <= 0 )
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T9 , "" );
				else
					KWIN_Item_Int_Display( hdlg , IDC_READY_USE_T9 , CHINTERLOCK_DATA2[8] );
			}
		}
		//
		if ( ( mode == 0 ) || ( mode == 1 ) ) {
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P1 , CHINTERLOCK_DATA3 , "N/A|Skip" , "N/A" );
		}
		//
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &(CHINTERLOCK_DATA[0]) );
			return TRUE;
		case IDC_READY_USE_B2 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &(CHINTERLOCK_DATA[1]) );
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &(CHINTERLOCK_DATA[2]) );
			return TRUE;
		case IDC_READY_USE_B4 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &(CHINTERLOCK_DATA[3]) );
			return TRUE;

		case IDC_READY_USE_T1 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[0]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[0]) );
			}
			return TRUE;
		case IDC_READY_USE_T2 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[1]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM2 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[1]) );
			}
			return TRUE;
		case IDC_READY_USE_T3 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[2]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM3 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[2]) );
			}
			return TRUE;
		case IDC_READY_USE_T4 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[3]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM4 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[3]) );
			}
			return TRUE;
		case IDC_READY_USE_T5 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[4]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM5 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[4]) );
			}
			return TRUE;
		case IDC_READY_USE_T6 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[5]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM6 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[5]) );
			}
			return TRUE;
		case IDC_READY_USE_T7 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[6]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM7 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[6]) );
			}
			return TRUE;
		case IDC_READY_USE_T8 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[7]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM8 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[7]) );
			}
			return TRUE;
		case IDC_READY_USE_T9 :
			if ( mode == 3 ) {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &(CHINTERLOCK_DATA2[8]) );
			}
			else {
				KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select TM9 Interlocak No" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA2[8]) );
			}
			return TRUE;

		case IDC_READY_USE_P1 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &CHINTERLOCK_DATA3 );
			return TRUE;

		case IDOK :
			CHINTERLOCK_RESULT = TRUE;
			EndDialog( hdlg , 0 );
			return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}


BOOL APIENTRY Gui_IDD_CHINTERLOCK_SUB_PAD2_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
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
		CHINTERLOCK_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B2 , CHINTERLOCK_DATA[1] );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B3 , CHINTERLOCK_DATA[2] );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B4 , CHINTERLOCK_DATA[3] );
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Offeset" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA[0]) );
			return TRUE;
		case IDC_READY_USE_B2 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Offeset" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA[1]) );
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Offeset" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA[2]) );
			return TRUE;
		case IDC_READY_USE_B4 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Offeset" , "Select" , 0 , 99 , &(CHINTERLOCK_DATA[3]) );
			return TRUE;
		case IDOK :	CHINTERLOCK_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL APIENTRY Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	char Buffer[32];
	static int mode;
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
		mode = (int) lParam;
		CHINTERLOCK_RESULT = FALSE;
		return TRUE;
	case WM_PAINT:
		//
		BeginPaint( hdlg, &ps );
		//
		if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() > 2 ) { // 2015.03.30
			//
			if ( ( mode == 2 ) || ( mode == 3 ) ) {

				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] , "AB|A|B|AB(1)" , "????" );

				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , CHINTERLOCK_DATA[1] , "AB|A|B|AB(1)" , "????" );

				KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			}
			else {

				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] , "AB|A|B|C|D|AB|AC|AD|BC|BD|CD|ABC|ABD|ACD|BCD" , "????" );

				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , CHINTERLOCK_DATA[1] , "AB|A|B|C|D|AB|AC|AD|BC|BD|CD|ABC|ABD|ACD|BCD" , "????" );

				KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );

			}
			//
		}
		else {
			//
//			if ( ( mode == 1 ) || ( mode == 2 ) ) { // 2015.03.30
			if ( ( mode == 1 ) || ( mode == 2 ) || ( mode == 3 ) ) { // 2015.03.30

				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] , "AB|A|B|AB(1)" , "????" ); // 2007.07.24

				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , CHINTERLOCK_DATA[1] , "AB|A|B|AB(1)" , "????" ); // 2007.07.24

				KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 ); // 2004.01.30
			}
			else {
				if ( CHINTERLOCK_DATA[0] <= 0 ) {
	//				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "N/A" ); // 2014.11.28
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "AB" ); // 2014.11.28
				}
				else {
					//
					if      ( CHINTERLOCK_DATA[0] == 1 ) { // 2014.11.28
						sprintf( Buffer , "A.PM%d" , CHINTERLOCK_DATA[0] );
					}
					else if ( CHINTERLOCK_DATA[0] == 2 ) { // 2014.11.28
						sprintf( Buffer , "B.PM%d" , CHINTERLOCK_DATA[0] );
					}
					else {
						sprintf( Buffer , "PM%d" , CHINTERLOCK_DATA[0] );
					}
					//
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , Buffer );
				}
				if ( CHINTERLOCK_DATA[1] <= 0 ) {
	//				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "N/A" ); // 2014.11.28
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "AB" ); // 2014.11.28
				}
				else {
					//
					if      ( CHINTERLOCK_DATA[1] == 1 ) {
						sprintf( Buffer , "A.PM%d" , CHINTERLOCK_DATA[1] );
					}
					else if ( CHINTERLOCK_DATA[1] == 2 ) {
						sprintf( Buffer , "B.PM%d" , CHINTERLOCK_DATA[1] );
					}
					else {
						sprintf( Buffer , "PM%d" , CHINTERLOCK_DATA[1] );
					}
					//
					KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , Buffer );
				}
				//
				KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 , CHINTERLOCK_DATA[3] ,
					"N/A|Pick|Place|All|Pick(TR)|Place(TR)|All(TR)|All-Deny|All-Deny:Swap-PM1|All-Deny:Swap-PM2|All-Deny:Swap-PM3|All-Deny:Swap-PM4|All-Deny:Swap-PM5|All-Deny:Swap-PM6|All-Deny:Swap-PM7|All-Deny:Swap-PM8|All-Deny:Swap-PM9|All-Deny:Swap-PM10|All-Deny:Swap-PM11|All-Deny:Swap-PM12" , "Unknown" ); // 2007.07.24
				//
			}
			//
		}
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			if      ( mode == 1 ) {
				if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() > 2 ) { // 2015.03.30
					KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B|C|D|AB|AC|AD|BC|BD|CD|ABC|ABD|ACD|BCD" , &(CHINTERLOCK_DATA[0]) );
				}
				else {
					KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B" , &(CHINTERLOCK_DATA[0]) );
				}
			}
			else if ( mode == 2 ) {
				KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B|AB(1)" , &(CHINTERLOCK_DATA[0]) );
			}
			else if ( mode == 3 ) { // 2015.03.30
				KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B" , &(CHINTERLOCK_DATA[0]) );
			}
			else {
				if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() > 2 ) { // 2015.03.30
					KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B|C|D|AB|AC|AD|BC|BD|CD|ABC|ABD|ACD|BCD" , &(CHINTERLOCK_DATA[0]) );
				}
				else {
					KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Chamber Index" , "Chamber" , 0 , MAX_PM_CHAMBER_DEPTH , &(CHINTERLOCK_DATA[0]) );
				}
			}
			return TRUE;
		case IDC_READY_USE_B2 :
			if      ( mode == 1 ) {
				if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() > 2 ) { // 2015.03.30
					KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B|C|D|AB|AC|AD|BC|BD|CD|ABC|ABD|ACD|BCD" , &(CHINTERLOCK_DATA[1]) );
				}
				else {
					KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B" , &(CHINTERLOCK_DATA[1]) );
				}
			}
			else if ( mode == 2 ) { // 2007.06.28
				KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B|AB(1)" , &(CHINTERLOCK_DATA[1]) ); // 2007.06.28
			}
			else if ( mode == 3 ) { // 2015.03.30
				KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B" , &(CHINTERLOCK_DATA[1]) );
			}
			else {
				if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() > 2 ) { // 2015.03.30
					KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm" , "Arm Finger" , "AB|A|B|C|D|AB|AC|AD|BC|BD|CD|ABC|ABD|ACD|BCD" , &(CHINTERLOCK_DATA[1]) );
				}
				else {
					KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Chamber Index" , "Chamber" , 0 , MAX_PM_CHAMBER_DEPTH , &(CHINTERLOCK_DATA[1]) );
				}
			}
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Arm (Full) Deny Mode" , "Deny Mode" , "N/A|When Pick|When Place|Every Time|When Pick(+Transfer)|When Place(+Transfer)|Every Time(+Transfer)|All Deny(Single Run)|All Deny(Single Run):Swap-PM1|All Deny(Single Run):Swap-PM2|All Deny(Single Run):Swap-PM3|All Deny(Single Run):Swap-PM4|All Deny(Single Run):Swap-PM5|All Deny(Single Run):Swap-PM6|All Deny(Single Run):Swap-PM7|All Deny(Single Run):Swap-PM8|All Deny(Single Run):Swap-PM9|All Deny(Single Run):Swap-PM10|All Deny(Single Run):Swap-PM11|All Deny(Single Run):Swap-PM12" , &(CHINTERLOCK_DATA[2]) );
			return TRUE;
		case IDOK :	CHINTERLOCK_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL APIENTRY Gui_IDD_CHINTERLOCK_SUB_PAD5_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) { // 2002.08.27
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
		CHINTERLOCK_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B1 , CHINTERLOCK_DATA[0] );
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Select Offset Index" , "Offset" , 0 , 9999 , &(CHINTERLOCK_DATA[0]) );
			return TRUE;
		case IDOK :	CHINTERLOCK_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
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
BOOL APIENTRY Gui_IDD_CHINTERLOCK_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	static int i , j;
	static int  TEMP_READY_USE_A[MAX_CASSETTE_SIDE][MAX_CHAMBER];
	static int  TEMP_READY_USE_B[MAX_CASSETTE_SIDE][MAX_CHAMBER];
	static int  TEMP_TM_INTERLOCK_NO[MAX_TM_CHAMBER_DEPTH][MAX_CHAMBER]; // 2002.04.27
	static int  TEMP_PICK_DENY[MAX_CHAMBER];
	static int  TEMP_PLACE_DENY[MAX_CHAMBER];
	static int  TEMP_MULTI_DENY[MAX_CHAMBER]; // 2004.01.30
	static int  TEMP_SLOT_OFFSET[MAX_CHAMBER]; // 2002.08.27
	static int  TEMP_FM_PICK_DENY[MAX_CHAMBER]; // 2006.09.26
	static int  TEMP_FM_PLACE_DENY[MAX_CHAMBER]; // 2006.09.26
	//
	static int  TEMP_TM_INTERLOCK_S[MAX_TM_CHAMBER_DEPTH]; // 2007.02.08
	static int  TEMP_INTERLOCK_FM_BM_PLACE_SEPARATE; // 2007.11.15
	static int  TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP; // 2013.04.25
	//
	static int  TEMP_PICK_ORDERING[MAX_TM_CHAMBER_DEPTH]; // 2012.09.21
	static int  TEMP_PICK_ORDER_SKIP[MAX_CHAMBER]; // 2012.09.21
	//
	static BOOL Control;
	BOOL modify;
	//
	static int intlks_mtm; // 2013.05.28
	static int intlks_tms; // 2013.05.28
	//
//	char szString[MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 1][4];    // <- 선언시 값 저장 삭제 // 2008.03.13 // 2012.09.21
	char szString[MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 2][6];    // <- 선언시 값 저장 삭제 // 2008.03.13 // 2012.09.21
	//
	char *szString2[] = { "AL/IC" , "CM1" , "CM2" , "CM3" , "CM4" , "CM5" , "CM6" , "CM7" , "CM8" , "CM9" , "CM10" , "CM11" , "CM12" , "CM13" , "CM14" , "CM15" , "CM16" , "CM17" , "CM18" , "CM19" , "CM20" };
	char *szString3[] = { "BM"    , "CM1" , "CM2" , "CM3" , "CM4" , "CM5" , "CM6" , "CM7" , "CM8" , "CM9" , "CM10" , "CM11" , "CM12" , "CM13" , "CM14" , "CM15" , "CM16" , "CM17" , "CM18" , "CM19" , "CM20" };
	char *szString4[] = { "ML"    , "PICK" , "PLACE" };
	char *szString5[] = { "BM"    , "Offset" };
	//
//	char *szTemp[ MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 1 ];      // KWIN_LView_Init_Header에서 szString사용시 warning 발생하여 새로운 포인터배열 생성 // 2008.03.13 // 2012.09.21
	char *szTemp[ MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 2 ];      // KWIN_LView_Init_Header에서 szString사용시 warning 발생하여 새로운 포인터배열 생성 // 2008.03.13 // 2012.09.21
	//
	int    szSize[21]  = {  50     ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 };
	int    szSize2[21] = {  70     ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,    45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 ,     45 };
	int    szSize4[3]  = {  50     ,    115 ,    115 };
	int    szSize5[2]  = {  50     ,    87 };

	HWND hSWnd;

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
		intlks_mtm = 0; // 2013.05.28
		intlks_tms = 0; // 2013.05.28
		//
		for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( TM + i ) ) {
				intlks_mtm = 1;
				break;
			}
		}
		//
		if ( intlks_mtm == 0 ) {
			KWIN_Item_Hide( hdlg , IDYES );
		}
		else {
			KWIN_Item_String_Display( hdlg , IDYES , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( TM + intlks_tms ) );
		}
		//
		//------------------------------------------------------------
		Control = (BOOL) lParam;
		//------------------------------------------------------------
		MoveCenterWindow( hdlg );

		TEMP_INTERLOCK_FM_BM_PLACE_SEPARATE = _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE(); // 2007.11.15
		TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP = _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP(); // 2013.04.25

		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			//
			TEMP_TM_INTERLOCK_S[ i ] = _i_SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( i ); // 2007.02.08
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				TEMP_TM_INTERLOCK_NO[i][ j+PM1 ]  = _i_SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL( i , j+PM1 );
			}
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
//				TEMP_READY_USE_A[i][ j+PM1 ]  = _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( i , j+PM1 ); // 2004.12.10
				TEMP_READY_USE_A[i][ j+PM1 ]  = _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM( i+CM1 , j+PM1 ); // 2004.12.10
			}
			TEMP_READY_USE_B[i][ AL ] = _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,AL);
			TEMP_READY_USE_B[i][ IC ] = _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,IC);
			TEMP_READY_USE_B[i][ F_AL ] = _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,F_AL);
			TEMP_READY_USE_B[i][ F_IC ] = _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,F_IC);
			//
			for ( j = 0 ; j < MAX_BM_CHAMBER_DEPTH ; j++ ) {
				TEMP_READY_USE_B[i][ j+BM1 ] = _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,j+BM1);
			}
//			TEMP_PICK_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+CM1 ); // 2014.11.28
//			TEMP_PLACE_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+CM1 ); // 2014.11.28
			//
			TEMP_MULTI_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i+CM1 ); // 2004.01.30
			//
			TEMP_FM_PICK_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+CM1 ); // 2006.09.26
			TEMP_FM_PLACE_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+CM1 ); // 2006.09.26
		}
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2002.08.27
			TEMP_SLOT_OFFSET[ i+BM1 ] = _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL(i+BM1);
		}
		//
		//-----------------------------------------------------------------------------------------------------------
		// 2008.03.13
		//-----------------------------------------------------------------------------------------------------------
/*
// 2008.03.13
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.04.27
			if ( i == 0 ) {
				sprintf( szString[MAX_CASSETTE_SIDE + 1 + i ] , "TM" );
			}
			else {
				sprintf( szString[MAX_CASSETTE_SIDE + 1 + i ] , "TM%d" , i + 1 );
			}
		}
*/
		/*
		// 2012.09.21
		sprintf( szString[0] , "PM" );
		for ( i = 1 ; i <= MAX_CASSETTE_SIDE ; i++ ){
			sprintf( szString[i], "CM%d", i );
		}
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.04.27
			if ( i == 0 ) {
				sprintf( szString[MAX_CASSETTE_SIDE + 1 + i ] , "TM" );
			}
			else {
				sprintf( szString[MAX_CASSETTE_SIDE + 1 + i ] , "TM%d" , i + 1 );
			}
		}
		//
		for( i = 0 ; i < ( MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 1 ) ; i++ )
			szTemp[i] = szString[i];                                // KWIN_LView_Init_Header에서 warning을 막고자 szTemp에 szString저장
		*/
		//
		// 2012.09.21
		sprintf( szString[0] , "MDL" );
		for ( i = 1 ; i <= MAX_CASSETTE_SIDE ; i++ ){
			sprintf( szString[i], "CM%d", i );
		}
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.04.27
			if ( i == 0 ) {
				sprintf( szString[MAX_CASSETTE_SIDE + 1 + i ] , "TM" );
			}
			else {
				sprintf( szString[MAX_CASSETTE_SIDE + 1 + i ] , "TM%d" , i + 1 );
			}
		}
		//
		strcpy( szString[MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 1 ] , "POSK" );
		//
		for( i = 0 ; i < ( MAX_CASSETTE_SIDE + MAX_TM_CHAMBER_DEPTH + 2 ) ; i++ ) szTemp[i] = szString[i];	// KWIN_LView_Init_Header에서 warning을 막고자 szTemp에 szString저장
		//
		//-----------------------------------------------------------------------------------------------------------
		/*
		// 2013.05.28
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.05
			TEMP_PICK_DENY[i+PM1] = _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+PM1 ); // 2003.02.05
			TEMP_PLACE_DENY[i+PM1] = _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+PM1 ); // 2003.02.05
		}
//		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29 // 2006.09.26
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29 // 2006.09.26
			TEMP_PICK_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+BM1 );
			TEMP_PLACE_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+BM1 );
			//
			TEMP_FM_PICK_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+BM1 ); // 2006.09.26
			TEMP_FM_PLACE_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+BM1 ); // 2006.09.26
		}
		*/

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2014.11.28
			TEMP_PICK_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+CM1 );
			TEMP_PLACE_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+CM1 );
		}
		// 2013.05.28
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			TEMP_PICK_DENY[i+PM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+PM1 );
			TEMP_PLACE_DENY[i+PM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+PM1 );
		}
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			TEMP_PICK_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+BM1 );
			TEMP_PLACE_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+BM1 );
		}
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			TEMP_FM_PICK_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+BM1 ); // 2006.09.26
			TEMP_FM_PLACE_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+BM1 ); // 2006.09.26
		}
		//
		TEMP_FM_PICK_DENY[F_AL] = _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ); // 2006.09.26
		TEMP_FM_PLACE_DENY[F_AL] = _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ); // 2006.09.26
		TEMP_FM_PICK_DENY[F_IC] = _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ); // 2006.09.26
		TEMP_FM_PLACE_DENY[F_IC] = _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ); // 2006.09.26
		//
		// 2012.09.21
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			TEMP_PICK_ORDERING[ i ] = _i_SCH_PRM_GET_MODULE_PICK_ORDERING( i );
		}
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			TEMP_PICK_ORDER_SKIP[ i ] = _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i );
		}
		//
		//-----------------------------------------------------------------------------------------------------------
		// 2008.03.13
		//-----------------------------------------------------------------------------------------------------------
//		//KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE , szString , szSize );
//		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE + 1 + MAX_TM_CHAMBER_DEPTH , szString , szSize ); // 2002.04.27
//		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

//		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE + 1 + MAX_TM_CHAMBER_DEPTH , szTemp , szSize ); // 2012.09.21
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE + 2 + MAX_TM_CHAMBER_DEPTH , szTemp , szSize ); // 2012.09.21
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//-----------------------------------------------------------------------------------------------------------
		//
//		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH + 1 ); // 2012.09.21
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 2 ); // 2012.09.21
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_CASSETTE_SIDE + 1 , szString2 , szSize2 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 4 );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , MAX_CASSETTE_SIDE + 1 , szString3 , szSize2 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , MAX_BM_CHAMBER_DEPTH );
		//
//		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2003.02.05
//			ShowWindow( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , SW_HIDE );
//		}
//		else { // 2003.02.05
			KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , 3 , szString4 , szSize4 );
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
//			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , MAX_CASSETTE_SIDE );
//			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ); // 2006.09.26
			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 2 ); // 2006.09.26
//		}
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 ) , 2 , szString5 , szSize5 ); // 2002.08.27
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ); // 2002.08.27
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 ) , MAX_BM_CHAMBER_DEPTH ); // 2002.08.27
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO:	{
					LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
					TCHAR szString[ 256 ];
					//
					szString[0] = 0; // 2016.02.02
					//
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
						/*
						// 2012.09.21
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								if ( i >= MAX_PM_CHAMBER_DEPTH ) { // 2007.02.28
									if ( ( lpdi->item.iSubItem - 1 ) >= MAX_CASSETTE_SIDE ) {
										sprintf( szString , "%d" , TEMP_TM_INTERLOCK_S[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE] );
									}
									else {
										sprintf( szString , "-" );
									}
								}
								else {
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( ( lpdi->item.iSubItem - 1 ) >= MAX_CASSETTE_SIDE ) {
											if ( TEMP_TM_INTERLOCK_NO[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE][ i+PM1 ] == 0 )
												sprintf( szString , "" );
											else
												sprintf( szString , "%d" , TEMP_TM_INTERLOCK_NO[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE][ i+PM1 ] );
										}
										else {
											switch( TEMP_READY_USE_A[lpdi->item.iSubItem-1][i+PM1] )	{
											case 1 :	sprintf( szString , "X" ); break;
											default :	sprintf( szString , "" ); break;
											}
										}
									}
									else {
										sprintf( szString , "-" );
									}
								}
								lstrcpy( lpdi->item.pxszText , szString );
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								if ( lpdi->item.iItem >= MAX_PM_CHAMBER_DEPTH ) {
									wsprintf( szString, "TMS" );
									lstrcpy( lpdi->item.pxszText , szString );
								}
								else {
									wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
									lstrcpy( lpdi->item.pxszText , szString );
								}
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
						*/
						//
						// 2012.09.21
						if ( lpdi->item.iSubItem == 0 ) { // first
							//
							if ( lpdi->item.mask & LVIF_TEXT ) {
								//
								if      ( lpdi->item.iItem == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 ) ) { // Pick Order
									wsprintf( szString, "PORD" );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								else if ( lpdi->item.iItem == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 0 ) ) { // tms
									wsprintf( szString, "TMS" );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								else if ( lpdi->item.iItem >= MAX_PM_CHAMBER_DEPTH ) { // bm
									wsprintf( szString, "BM%d", lpdi->item.iItem - MAX_PM_CHAMBER_DEPTH + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								else {
									wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								//
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
						else if ( lpdi->item.iSubItem == ( MAX_CASSETTE_SIDE + 1 + MAX_TM_CHAMBER_DEPTH ) ) { // pickorder skip
							//
							if ( lpdi->item.mask & LVIF_TEXT ) {
								//
								if      ( lpdi->item.iItem == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 ) ) { // Pick Order
									wsprintf( szString, "-" );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								else if ( lpdi->item.iItem == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 0 ) ) { // tms
									wsprintf( szString, "-" );
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								else if ( lpdi->item.iItem >= MAX_PM_CHAMBER_DEPTH ) { // bm
									if  ( !_i_SCH_PRM_GET_MODULE_MODE( lpdi->item.iItem - MAX_PM_CHAMBER_DEPTH + BM1 ) ) {
										wsprintf( szString, "-" );
									}
									else {
										if ( TEMP_PICK_ORDER_SKIP[ lpdi->item.iItem - MAX_PM_CHAMBER_DEPTH + BM1 ] ) {
											wsprintf( szString, "X" );
										}
										else {
											wsprintf( szString, "" );
										}
									}
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								else {
									if  ( !_i_SCH_PRM_GET_MODULE_MODE( lpdi->item.iItem + PM1 ) ) {
										wsprintf( szString, "-" );
									}
									else {
										if ( TEMP_PICK_ORDER_SKIP[ lpdi->item.iItem + PM1 ] ) {
											wsprintf( szString, "X" );
										}
										else {
											wsprintf( szString, "" );
										}
									}
//									lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								}
								//
							}
							//
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								//
								i = lpdi->item.iItem;
								//
								if      ( i == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 ) ) { // Pick Order
									if ( ( lpdi->item.iSubItem - 1 ) >= MAX_CASSETTE_SIDE ) {
										switch ( TEMP_PICK_ORDERING[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE] ) {
										case 1 :	sprintf( szString , "L" );	break;
										case 2 :	sprintf( szString , "F" );	break;
										default :	sprintf( szString , "" );	break;
										}
									}
									else {
										sprintf( szString , "-" );
									}
								}
								else if ( i == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 0 ) ) { // tms
									if ( ( lpdi->item.iSubItem - 1 ) >= MAX_CASSETTE_SIDE ) {
										sprintf( szString , "%d" , TEMP_TM_INTERLOCK_S[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE] );
									}
									else {
										sprintf( szString , "-" );
									}
								}
								else if ( i >= MAX_PM_CHAMBER_DEPTH ) { // bm
									wsprintf( szString, "-" );
								}
								else {
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( ( lpdi->item.iSubItem - 1 ) >= MAX_CASSETTE_SIDE ) {
											if ( TEMP_TM_INTERLOCK_NO[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE][ i+PM1 ] == 0 )
												sprintf( szString , "" );
											else
												sprintf( szString , "%d" , TEMP_TM_INTERLOCK_NO[ lpdi->item.iSubItem - 1 - MAX_CASSETTE_SIDE][ i+PM1 ] );
										}
										else {
											switch( TEMP_READY_USE_A[lpdi->item.iSubItem-1][i+PM1] )	{
											case 1 :	sprintf( szString , "X" ); break;
											default :	sprintf( szString , "" ); break;
											}
										}
									}
									else {
										sprintf( szString , "-" );
									}
								}
								//
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
								//
							}
						}
						//
						//
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								switch( lpdi->item.iItem ) {
								case 0 :
									wsprintf( szString , "%d" , TEMP_READY_USE_B[lpdi->item.iSubItem-1][F_AL] );
									break;
								case 1 :
									wsprintf( szString , "%d" , TEMP_READY_USE_B[lpdi->item.iSubItem-1][AL] );
									break;
								case 2 :
									wsprintf( szString , "%d" , TEMP_READY_USE_B[lpdi->item.iSubItem-1][F_IC] );
									break;
								case 3 :
									wsprintf( szString , "%d" , TEMP_READY_USE_B[lpdi->item.iSubItem-1][IC] );
									break;
								default :
									wsprintf( szString , "" );
									break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								switch( lpdi->item.iItem ) {
								case 0 :
									wsprintf( szString , "AL/FEM" );
									break;
								case 1 :
									wsprintf( szString , "AL/TM" );
									break;
								case 2 :
									wsprintf( szString , "IC/FEM" );
									break;
								case 3 :
									wsprintf( szString , "IC/TM" );
									break;
								default :
									wsprintf( szString , "" );
									break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString , "%d" , TEMP_READY_USE_B[lpdi->item.iSubItem-1][lpdi->item.iItem + BM1] );
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "BM%d", lpdi->item.iItem + 1 );
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if ( lpdi->item.iItem < MAX_CASSETTE_SIDE ) {
										//
										if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
											//
											if ( TEMP_PICK_DENY[lpdi->item.iItem + CM1] <= 0 ) {
												//
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "AB" ); break;
												case 1 : wsprintf( szString , "AB.(M-Deny)" ); break;
												case 2 : wsprintf( szString , "AB" ); break;
												case 3 : wsprintf( szString , "AB.(M-Deny)" ); break;
												case 4 : wsprintf( szString , "AB.(M-Deny:T)" ); break;
												case 5 : wsprintf( szString , "AB" ); break;
												case 6 : wsprintf( szString , "AB.(M-Deny:T)" ); break;
												case 7 : wsprintf( szString , "AB.(All-Deny)" ); break; // 2006.04.27
												default : wsprintf( szString , "AB.(All-Deny)" ); break; // 2006.04.27
												}
											}
											else if ( TEMP_PICK_DENY[lpdi->item.iItem + CM1] == 1 ) { // 2014.11.28
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "A.PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 1 : wsprintf( szString , "A.PM%d(M-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 2 : wsprintf( szString , "A.PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 3 : wsprintf( szString , "A.PM%d(M-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 4 : wsprintf( szString , "A.PM%d(M-Deny:T)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 5 : wsprintf( szString , "A.PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 6 : wsprintf( szString , "A.PM%d(M-Deny:T)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 7 : wsprintf( szString , "A.PM%d(All-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												default : wsprintf( szString , "A.PM%d(AllDeny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												}
											}
											else if ( TEMP_PICK_DENY[lpdi->item.iItem + CM1] == 2 ) { // 2014.11.28
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "B.PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 1 : wsprintf( szString , "B.PM%d(M-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 2 : wsprintf( szString , "B.PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 3 : wsprintf( szString , "B.PM%d(M-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 4 : wsprintf( szString , "B.PM%d(M-Deny:T)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 5 : wsprintf( szString , "B.PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 6 : wsprintf( szString , "B.PM%d(M-Deny:T)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 7 : wsprintf( szString , "B.PM%d(All-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												default : wsprintf( szString , "B.PM%d(AllDeny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												}
											}
											else {
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 1 : wsprintf( szString , "PM%d(M-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 2 : wsprintf( szString , "PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 3 : wsprintf( szString , "PM%d(M-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 4 : wsprintf( szString , "PM%d(M-Deny:T)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 5 : wsprintf( szString , "PM%d" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 6 : wsprintf( szString , "PM%d(M-Deny:T)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break;
												case 7 : wsprintf( szString , "PM%d(All-Deny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												default : wsprintf( szString , "PM%d(AllDeny)" , TEMP_PICK_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												}
											}
										}
										else { // 2015.03.30
											switch( TEMP_PICK_DENY[lpdi->item.iItem + CM1] ) {
											case 0 : wsprintf( szString , "ALL" ); break;
											case 1 : wsprintf( szString , "A" ); break;
											case 2 : wsprintf( szString , "B" ); break;
											case 3 : wsprintf( szString , "C" ); break;
											case 4 : wsprintf( szString , "D" ); break;
											case 5 : wsprintf( szString , "AB" ); break;
											case 6 : wsprintf( szString , "AC" ); break;
											case 7 : wsprintf( szString , "AD" ); break;
											case 8 : wsprintf( szString , "BC" ); break;
											case 9 : wsprintf( szString , "BD" ); break;
											case 10 : wsprintf( szString , "CD" ); break;
											case 11 : wsprintf( szString , "ABC" ); break;
											case 12 : wsprintf( szString , "ABD" ); break;
											case 13 : wsprintf( szString , "ACD" ); break;
											case 14 : wsprintf( szString , "BCD" ); break;
											default : wsprintf( szString , "?" ); break;
											}
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) )  {
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - MAX_CASSETTE_SIDE ) + PM1 ) ) {
											switch( TEMP_PICK_DENY[ ( lpdi->item.iItem - MAX_CASSETTE_SIDE ) + PM1 ] ) {
											case 0 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB" );
												}
												else { // 2015.03.30
													wsprintf( szString, "ALL" );
												}
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB(1)" );
												}
												else { // 2015.03.30
													wsprintf( szString, "C" );
												}
												break;
												// 2015.03.30
											case 4 : wsprintf( szString , "D" ); break;
											case 5 : wsprintf( szString , "AB" ); break;
											case 6 : wsprintf( szString , "AC" ); break;
											case 7 : wsprintf( szString , "AD" ); break;
											case 8 : wsprintf( szString , "BC" ); break;
											case 9 : wsprintf( szString , "BD" ); break;
											case 10 : wsprintf( szString , "CD" ); break;
											case 11 : wsprintf( szString , "ABC" ); break;
											case 12 : wsprintf( szString , "ABD" ); break;
											case 13 : wsprintf( szString , "ACD" ); break;
											case 14 : wsprintf( szString , "BCD" ); break;
											default : wsprintf( szString , "?" ); break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
//									else { // 2006.09.26
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ) ) {
											switch( TEMP_PICK_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] ) {
											case 0 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB" );
												}
												else { // 2015.03.30
													wsprintf( szString, "ALL" );
												}
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB(1)" );
												}
												else { // 2015.03.30
													wsprintf( szString, "C" );
												}
												break;
												// 2015.03.30
											case 4 : wsprintf( szString , "D" ); break;
											case 5 : wsprintf( szString , "AB" ); break;
											case 6 : wsprintf( szString , "AC" ); break;
											case 7 : wsprintf( szString , "AD" ); break;
											case 8 : wsprintf( szString , "BC" ); break;
											case 9 : wsprintf( szString , "BD" ); break;
											case 10 : wsprintf( szString , "CD" ); break;
											case 11 : wsprintf( szString , "ABC" ); break;
											case 12 : wsprintf( szString , "ABD" ); break;
											case 13 : wsprintf( szString , "ACD" ); break;
											case 14 : wsprintf( szString , "BCD" ); break;
											default : wsprintf( szString , "?" ); break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1 ) ) {
											switch( TEMP_FM_PICK_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1 ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1 ) ) {
											switch( TEMP_FM_PICK_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1 ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL ) ) {
											switch( TEMP_FM_PICK_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 + 1 ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC ) ) {
											switch( TEMP_FM_PICK_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									break;
								case 2 :
									if ( lpdi->item.iItem < MAX_CASSETTE_SIDE ) {
										//
										if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
											//
											if ( TEMP_PLACE_DENY[lpdi->item.iItem + CM1] <= 0 ) {
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "AB" ); break;
												case 1 : wsprintf( szString , "AB" ); break;
												case 2 : wsprintf( szString , "AB.(M-Deny)" ); break;
												case 3 : wsprintf( szString , "AB.(M-Deny)" ); break;
												case 4 : wsprintf( szString , "AB" ); break;
												case 5 : wsprintf( szString , "AB.(M-Deny:T)" ); break;
												case 6 : wsprintf( szString , "AB.(M-Deny:T)" ); break;
												case 7 : wsprintf( szString , "AB.(All-Deny)" ); break; // 2006.04.27
												default : wsprintf( szString , "AB.(Swap:P%d)" , TEMP_MULTI_DENY[lpdi->item.iItem + CM1] - 7 );	break; // 2006.04.27
												}
											}
											else if ( TEMP_PLACE_DENY[lpdi->item.iItem + CM1] == 1 ) { // 2014.11.28
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "A.PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 1 : wsprintf( szString , "A.PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 2 : wsprintf( szString , "A.PM%d(M-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 3 : wsprintf( szString , "A.PM%d(M-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 4 : wsprintf( szString , "A.PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 5 : wsprintf( szString , "A.PM%d(M-Deny:T)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 6 : wsprintf( szString , "A.PM%d(M-Deny:T)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 7 : wsprintf( szString , "A.PM%d(All-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												default : wsprintf( szString , "A.PM%d(Swap:P%d)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] , TEMP_MULTI_DENY[lpdi->item.iItem + CM1] - 7 );	break; // 2006.04.27
												}
											}
											else if ( TEMP_PLACE_DENY[lpdi->item.iItem + CM1] == 2 ) { // 2014.11.28
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "B.PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 1 : wsprintf( szString , "B.PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 2 : wsprintf( szString , "B.PM%d(M-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 3 : wsprintf( szString , "B.PM%d(M-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 4 : wsprintf( szString , "B.PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 5 : wsprintf( szString , "B.PM%d(M-Deny:T)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 6 : wsprintf( szString , "B.PM%d(M-Deny:T)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 7 : wsprintf( szString , "B.PM%d(All-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												default : wsprintf( szString , "B.PM%d(Swap:P%d)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] , TEMP_MULTI_DENY[lpdi->item.iItem + CM1] - 7 );	break; // 2006.04.27
												}
											}
											else {
												switch( TEMP_MULTI_DENY[lpdi->item.iItem + CM1] ) {
												case 0 : wsprintf( szString , "PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 1 : wsprintf( szString , "PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 2 : wsprintf( szString , "PM%d(M-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 3 : wsprintf( szString , "PM%d(M-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 4 : wsprintf( szString , "PM%d" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 5 : wsprintf( szString , "PM%d(M-Deny:T)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 6 : wsprintf( szString , "PM%d(M-Deny:T)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break;
												case 7 : wsprintf( szString , "PM%d(All-Deny)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] );	break; // 2006.04.27
												default : wsprintf( szString , "PM%d(Swap:P%d)" , TEMP_PLACE_DENY[lpdi->item.iItem + CM1] , TEMP_MULTI_DENY[lpdi->item.iItem + CM1] - 7 );	break; // 2006.04.27
												}
											}
										}
										else { // 2015.03.30
											switch( TEMP_PLACE_DENY[lpdi->item.iItem + CM1] ) {
											case 0 : wsprintf( szString , "ALL" ); break;
											case 1 : wsprintf( szString , "A" ); break;
											case 2 : wsprintf( szString , "B" ); break;
											case 3 : wsprintf( szString , "C" ); break;
											case 4 : wsprintf( szString , "D" ); break;
											case 5 : wsprintf( szString , "AB" ); break;
											case 6 : wsprintf( szString , "AC" ); break;
											case 7 : wsprintf( szString , "AD" ); break;
											case 8 : wsprintf( szString , "BC" ); break;
											case 9 : wsprintf( szString , "BD" ); break;
											case 10 : wsprintf( szString , "CD" ); break;
											case 11 : wsprintf( szString , "ABC" ); break;
											case 12 : wsprintf( szString , "ABD" ); break;
											case 13 : wsprintf( szString , "ACD" ); break;
											case 14 : wsprintf( szString , "BCD" ); break;
											default : wsprintf( szString , "?" ); break;
											}
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) )  {
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - MAX_CASSETTE_SIDE ) + PM1 ) ) {
											switch( TEMP_PLACE_DENY[ ( lpdi->item.iItem - MAX_CASSETTE_SIDE ) + PM1 ] ) {
											case 0 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB" );
												}
												else { // 2015.03.30
													wsprintf( szString, "ALL" );
												}
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB(1)" );
												}
												else { // 2015.03.30
													wsprintf( szString, "C" );
												}
												break;
												// 2015.03.30
											case 4 : wsprintf( szString , "D" ); break;
											case 5 : wsprintf( szString , "AB" ); break;
											case 6 : wsprintf( szString , "AC" ); break;
											case 7 : wsprintf( szString , "AD" ); break;
											case 8 : wsprintf( szString , "BC" ); break;
											case 9 : wsprintf( szString , "BD" ); break;
											case 10 : wsprintf( szString , "CD" ); break;
											case 11 : wsprintf( szString , "ABC" ); break;
											case 12 : wsprintf( szString , "ABD" ); break;
											case 13 : wsprintf( szString , "ACD" ); break;
											case 14 : wsprintf( szString , "BCD" ); break;
											default : wsprintf( szString , "?" ); break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
//									else { // 2006.09.26
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ) ) {
											switch( TEMP_PLACE_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1 ] ) {
											case 0 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB" );
												}
												else { // 2015.03.30
													wsprintf( szString, "ALL" );
												}
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 :
												if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) { // 2015.03.30
													wsprintf( szString, "AB(1)" );
												}
												else { // 2015.03.30
													wsprintf( szString, "C" );
												}
												break;
												// 2015.03.30
											case 4 : wsprintf( szString , "D" ); break;
											case 5 : wsprintf( szString , "AB" ); break;
											case 6 : wsprintf( szString , "AC" ); break;
											case 7 : wsprintf( szString , "AD" ); break;
											case 8 : wsprintf( szString , "BC" ); break;
											case 9 : wsprintf( szString , "BD" ); break;
											case 10 : wsprintf( szString , "CD" ); break;
											case 11 : wsprintf( szString , "ABC" ); break;
											case 12 : wsprintf( szString , "ABD" ); break;
											case 13 : wsprintf( szString , "ACD" ); break;
											case 14 : wsprintf( szString , "BCD" ); break;
											default : wsprintf( szString , "?" ); break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1 ) ) {
											switch( TEMP_FM_PLACE_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1 ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 : // 2007.06.28
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1 ) ) {
											switch( TEMP_FM_PLACE_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1 ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 : // 2009.10.13
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL ) ) {
											switch( TEMP_FM_PLACE_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 : // 2009.10.13
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 + 1 ) )  { // 2006.09.26
										if ( _i_SCH_PRM_GET_MODULE_MODE( ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC ) ) {
											switch( TEMP_FM_PLACE_DENY[ ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC ] ) {
											case 0 :
												wsprintf( szString, "AB" );
												break;
											case 1 :
												wsprintf( szString, "A" );
												break;
											case 2 :
												wsprintf( szString, "B" );
												break;
											case 3 : // 2009.10.13
												wsprintf( szString, "AB(1)" );
												break;
											}
										}
										else {
											wsprintf( szString, "" );
										}
									}
									break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								if ( lpdi->item.iItem < MAX_CASSETTE_SIDE ) {
									wsprintf( szString, "CM%d", lpdi->item.iItem + 1 );
								}
								else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) {
									wsprintf( szString, "PM%d", ( lpdi->item.iItem - MAX_CASSETTE_SIDE ) + 1 );
								}
//								else { // 2006.09.26
								else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) )  { // 2006.09.26
									wsprintf( szString, "BM%d", ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + 1 );
								}
								else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) )  { // 2006.09.26
									wsprintf( szString, "F-CM%d", ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + 1 );
								}
								else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) )  { // 2006.09.26
									wsprintf( szString, "F-BM%d", ( lpdi->item.iItem - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + 1 );
								}
								else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) )  { // 2006.09.26
									wsprintf( szString, "F-AL" );
								}
								else if ( lpdi->item.iItem < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 + 1 ) )  { // 2006.09.26
									wsprintf( szString, "F-IC" );
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
					}
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 ) ) { // 2002.08.27
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								if  ( _i_SCH_PRM_GET_MODULE_MODE( lpdi->item.iItem + BM1 ) ) {
									wsprintf( szString , "%d" , TEMP_SLOT_OFFSET[lpdi->item.iItem + BM1] );
								}
								else {
									wsprintf( szString , "" );
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "BM%d", lpdi->item.iItem + 1 );
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
					/*
					// 2012.09.21
					if ( i >= MAX_PM_CHAMBER_DEPTH ) { // 2007.02.28
						CHINTERLOCK_RESULT = FALSE;
						for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
							CHINTERLOCK_DATA2[j]  = TEMP_TM_INTERLOCK_S[j];
						}
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD_Proc , (LPARAM) TRUE );
						if ( CHINTERLOCK_RESULT ) {
							for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
								TEMP_TM_INTERLOCK_S[j] = CHINTERLOCK_DATA2[j];
							}
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
							CHINTERLOCK_RESULT = FALSE;
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								CHINTERLOCK_DATA[j]  = TEMP_READY_USE_A[j][i+PM1];
							}
							for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) { // 2002.04.27
								CHINTERLOCK_DATA2[j]  = TEMP_TM_INTERLOCK_NO[j][i+PM1];
							}
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD_Proc , (LPARAM) FALSE ); // 2002.04.27 // 2004.01.19 // 2004.08.10
							if ( CHINTERLOCK_RESULT ) {
								for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {	// 2002.04.27
									TEMP_TM_INTERLOCK_NO[j][i+PM1] = CHINTERLOCK_DATA2[j];
								}
								for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
									TEMP_READY_USE_A[j][i+PM1] = CHINTERLOCK_DATA[j];
								}
								InvalidateRect( hdlg , NULL , FALSE );
							}
						}
					}
					*/

					// 2012.09.21
					if      ( i == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 1 ) ) { // Pick Order
						CHINTERLOCK_RESULT = FALSE;
						for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
							CHINTERLOCK_DATA2[j]  = TEMP_PICK_ORDERING[j];
						}
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD_Proc , (LPARAM) 3 );
						if ( CHINTERLOCK_RESULT ) {
							for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
								TEMP_PICK_ORDERING[j] = CHINTERLOCK_DATA2[j];
							}
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					else if ( i == ( MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 0 ) ) { // tms
						CHINTERLOCK_RESULT = FALSE;
						for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
							CHINTERLOCK_DATA2[j]  = TEMP_TM_INTERLOCK_S[j];
						}
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD_Proc , (LPARAM) 2 );
						if ( CHINTERLOCK_RESULT ) {
							for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
								TEMP_TM_INTERLOCK_S[j] = CHINTERLOCK_DATA2[j];
							}
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					else if ( i >= MAX_PM_CHAMBER_DEPTH ) { // bm
						if ( _i_SCH_PRM_GET_MODULE_MODE(i-MAX_PM_CHAMBER_DEPTH + BM1) ) {
							CHINTERLOCK_RESULT = FALSE;
							CHINTERLOCK_DATA3 = TEMP_PICK_ORDER_SKIP[i-MAX_PM_CHAMBER_DEPTH + BM1];
							//
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD_Proc , (LPARAM) 1 );
							if ( CHINTERLOCK_RESULT ) {
								TEMP_PICK_ORDER_SKIP[i-MAX_PM_CHAMBER_DEPTH + BM1] = CHINTERLOCK_DATA3;
								//
								InvalidateRect( hdlg , NULL , FALSE );
							}
						}
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
							CHINTERLOCK_RESULT = FALSE;
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								CHINTERLOCK_DATA[j]  = TEMP_READY_USE_A[j][i+PM1];
							}
							for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
								CHINTERLOCK_DATA2[j]  = TEMP_TM_INTERLOCK_NO[j][i+PM1];
							}
							CHINTERLOCK_DATA3 = TEMP_PICK_ORDER_SKIP[i+PM1];
							//
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD_Proc , (LPARAM) 0 );
							if ( CHINTERLOCK_RESULT ) {
								for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
									TEMP_TM_INTERLOCK_NO[j][i+PM1] = CHINTERLOCK_DATA2[j];
								}
								for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
									TEMP_READY_USE_A[j][i+PM1] = CHINTERLOCK_DATA[j];
								}
								TEMP_PICK_ORDER_SKIP[i+PM1] = CHINTERLOCK_DATA3;
								//
								InvalidateRect( hdlg , NULL , FALSE );
							}
						}
					}
					//
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					CHINTERLOCK_RESULT = FALSE;
					switch( i ) {
					case 0 :
						for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
							CHINTERLOCK_DATA[j]  = TEMP_READY_USE_B[j][F_AL];
						}
						break;
					case 1 :
						for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
							CHINTERLOCK_DATA[j]  = TEMP_READY_USE_B[j][AL];
						}
						break;
					case 2 :
						for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
							CHINTERLOCK_DATA[j]  = TEMP_READY_USE_B[j][F_IC];
						}
						break;
					case 3 :
						for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
							CHINTERLOCK_DATA[j]  = TEMP_READY_USE_B[j][IC];
						}
						break;
					}
					hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
					GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB2_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD2_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
					if ( CHINTERLOCK_RESULT ) {
						switch( i ) {
						case 0 :
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								TEMP_READY_USE_B[j][F_AL] = CHINTERLOCK_DATA[j];
							}
							break;
						case 1 :
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								TEMP_READY_USE_B[j][AL] = CHINTERLOCK_DATA[j];
							}
							break;
						case 2 :
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								TEMP_READY_USE_B[j][F_IC] = CHINTERLOCK_DATA[j];
							}
							break;
						case 3 :
							for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
								TEMP_READY_USE_B[j][IC] = CHINTERLOCK_DATA[j];
							}
							break;
						}
						InvalidateRect( hdlg , NULL , FALSE );
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					CHINTERLOCK_RESULT = FALSE;
					for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
						CHINTERLOCK_DATA[j]  = TEMP_READY_USE_B[j][i + BM1];
					}
					hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 );
					GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB2_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD2_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
					if ( CHINTERLOCK_RESULT ) {
						for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
							TEMP_READY_USE_B[j][i + BM1] = CHINTERLOCK_DATA[j];
						}
						InvalidateRect( hdlg , NULL , FALSE );
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					CHINTERLOCK_RESULT = FALSE;
					if ( i < MAX_CASSETTE_SIDE ) {
						if ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
							CHINTERLOCK_DATA[0]  = TEMP_PICK_DENY[i + CM1];
							CHINTERLOCK_DATA[1]  = TEMP_PLACE_DENY[i + CM1];
							CHINTERLOCK_DATA[2]  = TEMP_MULTI_DENY[i + CM1]; // 2004.01.30
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 0 ); // 2004.01.19 // 2004.08.10
						}
					}
					else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) {
						if ( _i_SCH_PRM_GET_MODULE_MODE(( i - MAX_CASSETTE_SIDE ) + PM1) ) {
							CHINTERLOCK_DATA[0]  = TEMP_PICK_DENY[( i - MAX_CASSETTE_SIDE ) + PM1];
							CHINTERLOCK_DATA[1]  = TEMP_PLACE_DENY[( i - MAX_CASSETTE_SIDE ) + PM1];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10
						}
					}
//					else { // 2006.09.26
					else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) {
						if ( _i_SCH_PRM_GET_MODULE_MODE(( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1) ) {
							CHINTERLOCK_DATA[0]  = TEMP_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1];
							CHINTERLOCK_DATA[1]  = TEMP_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10
						}
					}
					else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) { // 2006.09.26
						if ( _i_SCH_PRM_GET_MODULE_MODE(( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1) ) {
							CHINTERLOCK_DATA[0]  = TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1];
							CHINTERLOCK_DATA[1]  = TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 2 ); // 2004.01.19 // 2004.08.10
						}
					}
					else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) { // 2006.09.26
						if ( _i_SCH_PRM_GET_MODULE_MODE(( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1) ) {
							CHINTERLOCK_DATA[0]  = TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1];
							CHINTERLOCK_DATA[1]  = TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
//							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10 // 2009.10.13
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 2 ); // 2004.01.19 // 2004.08.10 // 2009.10.13
						}
					}
					else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) { // 2006.09.26
						if ( _i_SCH_PRM_GET_MODULE_MODE(( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL) ) {
							CHINTERLOCK_DATA[0]  = TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL];
							CHINTERLOCK_DATA[1]  = TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
//							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10 // 2015.03.30
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 3 ); // 2004.01.19 // 2004.08.10 // 2015.03.30
						}
					}
					else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 + 1 ) ) { // 2006.09.26
						if ( _i_SCH_PRM_GET_MODULE_MODE(( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC) ) {
							CHINTERLOCK_DATA[0]  = TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC];
							CHINTERLOCK_DATA[1]  = TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 );
//							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 1 ); // 2004.01.19 // 2004.08.10 // 2015.03.30
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB3_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD3_Proc , (LPARAM) 3 ); // 2004.01.19 // 2004.08.10 // 2015.03.30
						}
					}
					if ( CHINTERLOCK_RESULT ) {
						if ( i < MAX_CASSETTE_SIDE ) {
							TEMP_PICK_DENY[i + CM1] = CHINTERLOCK_DATA[0];
							TEMP_PLACE_DENY[i + CM1] = CHINTERLOCK_DATA[1];
							TEMP_MULTI_DENY[i + CM1] = CHINTERLOCK_DATA[2]; // 2004.01.30
						}
						else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) {
							TEMP_PICK_DENY[( i - MAX_CASSETTE_SIDE ) + PM1] = CHINTERLOCK_DATA[0];
							TEMP_PLACE_DENY[( i - MAX_CASSETTE_SIDE ) + PM1] = CHINTERLOCK_DATA[1];
						}
//						else { // 2006.09.26
						else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) {
							TEMP_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1] = CHINTERLOCK_DATA[0];
							TEMP_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH ) ) + BM1] = CHINTERLOCK_DATA[1];
						}
						else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) { // 2006.09.26
							TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1] = CHINTERLOCK_DATA[0];
							TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ) ) + CM1] = CHINTERLOCK_DATA[1];
						}
						else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) { // 2006.09.26
							TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1] = CHINTERLOCK_DATA[0];
							TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) ) + BM1] = CHINTERLOCK_DATA[1];
						}
						else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) { // 2006.09.26
							TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL] = CHINTERLOCK_DATA[0];
							TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH ) ) + F_AL] = CHINTERLOCK_DATA[1];
						}
						else if ( i < ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 2 ) ) { // 2006.09.26
							TEMP_FM_PICK_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC] = CHINTERLOCK_DATA[0];
							TEMP_FM_PLACE_DENY[( i - ( MAX_CASSETTE_SIDE + MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + MAX_BM_CHAMBER_DEPTH + 1 ) ) + F_IC] = CHINTERLOCK_DATA[1];
						}
						InvalidateRect( hdlg , NULL , FALSE );
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 ) ) { // 2002.08.27
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
						CHINTERLOCK_RESULT = FALSE;
						CHINTERLOCK_DATA[0]  = TEMP_SLOT_OFFSET[i + BM1];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX5 );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_SUB5_PAD ) , hSWnd , Gui_IDD_CHINTERLOCK_SUB_PAD5_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( CHINTERLOCK_RESULT ) {
							TEMP_SLOT_OFFSET[i + BM1] = CHINTERLOCK_DATA[0];
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
				}
				return 0;
			case LVN_ITEMCHANGED :	return 0;
			case LVN_KEYDOWN :		return 0;
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		modify = FALSE;

		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CLEAR_USE_BM		, TEMP_INTERLOCK_FM_BM_PLACE_SEPARATE , "-|On" , "????" ); // 2007.11.15
		//=============================================================================
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CLEAR_USE_TM		, TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP , "-|On" , "????" ); // 2013.04.24 // 2017.07.13
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CLEAR_USE_TM		, TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP , "-|On|On+EvSwp" , "????" ); // 2013.04.24 // 2017.07.13
		//=============================================================================

		if ( TEMP_INTERLOCK_FM_BM_PLACE_SEPARATE != _i_SCH_PRM_GET_INTERLOCK_FM_BM_PLACE_SEPARATE() ) modify = TRUE; // 2007.11.15
		if ( TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP != _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() ) modify = TRUE; // 2013.04.24

		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.04.27
			//
			if ( TEMP_TM_INTERLOCK_S[ i ] != _i_SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( i ) ) modify = TRUE; // 2007.02.08
			//
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( TEMP_TM_INTERLOCK_NO[i][ j+PM1 ] != _i_SCH_PRM_GET_INTERLOCK_TM_RUN_FOR_ALL(i,j+PM1) ) modify = TRUE;
			}
		}			
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
				if ( TEMP_READY_USE_A[i][ j+PM1 ] != _i_SCH_PRM_GET_INTERLOCK_PM_RUN_FOR_CM(i+CM1,j+PM1) ) modify = TRUE;
			}
			if ( TEMP_READY_USE_B[i][ AL ] != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,AL) ) modify = TRUE;
			if ( TEMP_READY_USE_B[i][ IC ] != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,IC) ) modify = TRUE;
			if ( TEMP_READY_USE_B[i][ F_AL ] != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,F_AL) ) modify = TRUE;
			if ( TEMP_READY_USE_B[i][ F_IC ] != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,F_IC) ) modify = TRUE;
			//
			for ( j = 0 ; j < MAX_BM_CHAMBER_DEPTH ; j++ ) {
				if ( TEMP_READY_USE_B[i][ j+BM1 ] != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM(i+CM1,j+BM1) ) modify = TRUE;
			}
//			if ( TEMP_PICK_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+CM1 ) ) modify = TRUE; // 2014.11.28
//			if ( TEMP_PLACE_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+CM1 ) ) modify = TRUE; // 2014.11.28
			//
			if ( TEMP_MULTI_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i+CM1 ) ) modify = TRUE; // 2004.01.30
			if ( TEMP_FM_PICK_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+CM1 ) ) modify = TRUE; // 2006.09.26
			if ( TEMP_FM_PLACE_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+CM1 ) ) modify = TRUE; // 2006.09.26
		}
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2002.08.27
			if ( TEMP_SLOT_OFFSET[ i+BM1 ] != _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL(i+BM1) ) modify = TRUE;
		}
		/*
		// 2013.05.28
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.05
			if ( TEMP_PICK_DENY[i+PM1] != _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+PM1 ) ) modify = TRUE;
			if ( TEMP_PLACE_DENY[i+PM1] != _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+PM1 ) ) modify = TRUE;
		}
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29
			if ( TEMP_PICK_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+BM1 ) ) modify = TRUE;
			if ( TEMP_PLACE_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+BM1 ) ) modify = TRUE;
			if ( TEMP_FM_PICK_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+BM1 ) ) modify = TRUE; // 2006.09.26
			if ( TEMP_FM_PLACE_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+BM1 ) ) modify = TRUE; // 2006.09.26
		}
		*/
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2014.11.28
			if ( TEMP_PICK_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+CM1 ) ) modify = TRUE;
			if ( TEMP_PLACE_DENY[i+CM1] != _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+CM1 ) ) modify = TRUE;
		}
		//
		// 2013.05.28
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.05
			if ( TEMP_PICK_DENY[i+PM1] != _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+PM1 ) ) modify = TRUE;
			if ( TEMP_PLACE_DENY[i+PM1] != _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+PM1 ) ) modify = TRUE;
		}
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29
			if ( TEMP_PICK_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+BM1 ) ) modify = TRUE;
			if ( TEMP_PLACE_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+BM1 ) ) modify = TRUE;
		}
		//
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.29
			if ( TEMP_FM_PICK_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+BM1 ) ) modify = TRUE; // 2006.09.26
			if ( TEMP_FM_PLACE_DENY[i+BM1] != _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+BM1 ) ) modify = TRUE; // 2006.09.26
		}
		//
		if ( TEMP_FM_PICK_DENY[F_AL] != _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL ) ) modify = TRUE; // 2006.09.26
		if ( TEMP_FM_PLACE_DENY[F_AL] != _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL ) ) modify = TRUE; // 2006.09.26
		if ( TEMP_FM_PICK_DENY[F_IC] != _i_SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC ) ) modify = TRUE; // 2006.09.26
		if ( TEMP_FM_PLACE_DENY[F_IC] != _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC ) ) modify = TRUE; // 2006.09.26
		//
		// 2012.09.21
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			if ( TEMP_PICK_ORDERING[ i ] != _i_SCH_PRM_GET_MODULE_PICK_ORDERING( i ) ) modify = TRUE;
		}
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( TEMP_PICK_ORDER_SKIP[ i ] != _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( i ) ) modify = TRUE;
		}
		//
		if ( modify ) {
			//
			if ( intlks_mtm == 0 ) {
				KWIN_Item_Hide( hdlg , IDRETRY );
				KWIN_Item_Enable( hdlg , IDOK );
			}
			else {
				KWIN_Item_Hide( hdlg , IDOK );
				KWIN_Item_Enable( hdlg , IDRETRY );
			}
			//
			if ( intlks_mtm == 1 ) intlks_mtm = 2;
			//
			if ( intlks_mtm != 0 ) KWIN_Item_Disable( hdlg , IDYES );
			//
		}
		else {
			//
			if ( intlks_mtm == 0 ) {
				KWIN_Item_Hide( hdlg , IDRETRY );
				KWIN_Item_Disable( hdlg , IDOK );
			}
			else {
				KWIN_Item_Hide( hdlg , IDOK );
				KWIN_Item_Disable( hdlg , IDRETRY );
			}
			//
			if ( intlks_mtm == 2 ) intlks_mtm = 1;
			//
			if ( intlks_mtm != 0 ) KWIN_Item_Enable( hdlg , IDYES );
			//
		}
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_CLEAR_USE_BM :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &TEMP_INTERLOCK_FM_BM_PLACE_SEPARATE );
			return TRUE;

		case IDC_CLEAR_USE_TM : // 2013.04.25
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP ); // 2017.07.13
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 2 , &TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP ); // 2017.07.13
			return TRUE;

		case IDYES : // 2013.05.23
			//
			if ( intlks_mtm != 1 ) return TRUE;
			//
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , MAX_TM_CHAMBER_DEPTH -1 , &intlks_tms );
			//
			KWIN_Item_String_Display( hdlg , IDYES , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( TM + intlks_tms ) );
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2014.11.28
				TEMP_PICK_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+CM1 );
				TEMP_PLACE_DENY[i+CM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+CM1 );
			}
			//
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2016.01.21
				TEMP_PICK_DENY[i+PM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+PM1 );
				TEMP_PLACE_DENY[i+PM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+PM1 );
			}
			//
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				TEMP_PICK_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+BM1 );
				TEMP_PLACE_DENY[i+BM1] = _i_SCH_PRM_GET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+BM1 );
			}
			//
			InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX4 ) , NULL , FALSE );
			//
			return TRUE;

		case IDOK :
		case IDRETRY : // 2013.05.28
			if ( Control ) return TRUE;

			_i_SCH_PRM_SET_INTERLOCK_FM_BM_PLACE_SEPARATE( TEMP_INTERLOCK_FM_BM_PLACE_SEPARATE );
			_i_SCH_PRM_SET_INTERLOCK_TM_BM_OTHERGROUP_SWAP( TEMP_INTERLOCK_TM_BM_OTHERGROUP_SWAP ); // 2013.04.25

			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2002.04.27
				//
				_i_SCH_PRM_SET_INTERLOCK_TM_SINGLE_RUN( i , TEMP_TM_INTERLOCK_S[ i ] ); // 2007.02.08
				//
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					_i_SCH_PRM_SET_INTERLOCK_TM_RUN_FOR_ALL( i , j+PM1 , TEMP_TM_INTERLOCK_NO[i][ j+PM1 ] );
				}
			}			
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					_i_SCH_PRM_SET_INTERLOCK_PM_RUN_FOR_CM( i + CM1 , j+PM1 , TEMP_READY_USE_A[i][ j+PM1 ] );
				}
				//
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( i + CM1 , AL , TEMP_READY_USE_B[i][ AL ] );
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( i + CM1 , IC , TEMP_READY_USE_B[i][ IC ] );
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( i + CM1 , F_AL , TEMP_READY_USE_B[i][ F_AL ] );
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( i + CM1 , F_IC , TEMP_READY_USE_B[i][ F_IC ] );
				for ( j = 0 ; j < MAX_BM_CHAMBER_DEPTH ; j++ ) {
					_i_SCH_PRM_SET_OFFSET_SLOT_FROM_CM( i + CM1 , j + BM1 , TEMP_READY_USE_B[i][ j+BM1 ] );
				}
//				_i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+CM1 , TEMP_PICK_DENY[i+CM1] ); // 2014.11.28
//				_i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+CM1 , TEMP_PLACE_DENY[i+CM1] ); // 2014.11.28
				//
				_i_SCH_PRM_SET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( i+CM1 , TEMP_MULTI_DENY[i+CM1] ); // 2004.01.30
				_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+CM1 , TEMP_FM_PICK_DENY[i+CM1] ); // 2006.09.26
				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+CM1 , TEMP_FM_PLACE_DENY[i+CM1] ); // 2006.09.26
			}
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2002.08.27
				_i_SCH_PRM_SET_OFFSET_SLOT_FROM_ALL( i + BM1 , TEMP_SLOT_OFFSET[ i+BM1 ] );
			}
			/*
			// 2013.05.28
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2003.02.05
				_i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+PM1 , TEMP_PICK_DENY[i+PM1] );
				_i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+PM1 , TEMP_PLACE_DENY[i+PM1] );
			}
//			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2004.06.28 // 2006.09.26
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2004.06.28 // 2006.09.26
				_i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+BM1 , TEMP_PICK_DENY[i+BM1] );
				_i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+BM1 , TEMP_PLACE_DENY[i+BM1] );
				_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+BM1 , TEMP_FM_PICK_DENY[i+BM1] ); // 2006.09.26
				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+BM1 , TEMP_FM_PLACE_DENY[i+BM1] ); // 2006.09.26
			}
			*/

			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2014.11.28
				_i_SCH_PRM_SET_INTERLOCK_PM_PICK_DENY_FOR_MDL( i+CM1 , TEMP_PICK_DENY[i+CM1] );
				_i_SCH_PRM_SET_INTERLOCK_PM_PLACE_DENY_FOR_MDL( i+CM1 , TEMP_PLACE_DENY[i+CM1] );
				//
			}

			// 2013.05.28
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+PM1 , TEMP_PICK_DENY[i+PM1] );
				_i_SCH_PRM_SET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+PM1 , TEMP_PLACE_DENY[i+PM1] );
			}
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_INTERLOCK_PM_M_PICK_DENY_FOR_MDL( intlks_tms , i+BM1 , TEMP_PICK_DENY[i+BM1] );
				_i_SCH_PRM_SET_INTERLOCK_PM_M_PLACE_DENY_FOR_MDL( intlks_tms , i+BM1 , TEMP_PLACE_DENY[i+BM1] );
			}
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( i+BM1 , TEMP_FM_PICK_DENY[i+BM1] ); // 2006.09.26
				_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i+BM1 , TEMP_FM_PLACE_DENY[i+BM1] ); // 2006.09.26
			}
			//
			_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_AL , TEMP_FM_PICK_DENY[F_AL] ); // 2006.09.26
			_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_AL , TEMP_FM_PLACE_DENY[F_AL] ); // 2006.09.26
			//
			_i_SCH_PRM_SET_INTERLOCK_FM_PICK_DENY_FOR_MDL( F_IC , TEMP_FM_PICK_DENY[F_IC] ); // 2006.09.26
			_i_SCH_PRM_SET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( F_IC , TEMP_FM_PLACE_DENY[F_IC] ); // 2006.09.26
			//
			// 2012.09.21
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_MODULE_PICK_ORDERING( i , TEMP_PICK_ORDERING[ i ] );
			}
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				_i_SCH_PRM_SET_MODULE_PICK_ORDERING_SKIP( i , TEMP_PICK_ORDER_SKIP[ i ] );
			}
			//
			Set_RunPrm_Config_Change_for_ChIntlks_IO(); // 2013.11.21
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//---------------------------------
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
