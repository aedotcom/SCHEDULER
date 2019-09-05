#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL CLSSNR_RESULT = FALSE;
int  CLSSNR_DATA1;
int  CLSSNR_DATA2;
int  CLSSNR_DATA3;
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_CLSSNR_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
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
		CLSSNR_RESULT = FALSE;
		return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1		, CLSSNR_DATA1 , "X|Use|Skip" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		if      ( CLSSNR_DATA1 == 1 ) {
			KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B2 , CLSSNR_DATA2 );
		}
		else if ( CLSSNR_DATA1 == 2 ) { // 2006.04.27
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
		}
		else {
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
		}
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3		, CLSSNR_DATA3 , "X|Use" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &CLSSNR_DATA1 );
			return TRUE;
		case IDC_READY_USE_B2 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Clear Out Delay Time" , "Select" , 0 , 9999 , &CLSSNR_DATA2 );
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &CLSSNR_DATA3 );
			return TRUE;
		case IDOK :		CLSSNR_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL APIENTRY Gui_IDD_CLSSNR_SUB_PAD3_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
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
		CLSSNR_RESULT = FALSE;
		return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B2 , CLSSNR_DATA2 );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3		, CLSSNR_DATA3 , "X|Use" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B2 :
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Clear Out Delay Time" , "Select" , 0 , 9999 , &CLSSNR_DATA2 );
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &CLSSNR_DATA3 );
			return TRUE;
		case IDOK :		CLSSNR_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_CLSSNR_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i , c;
	static int  TEMP_USE[MAX_CHAMBER];
	static int  TEMP_MSG_USE[MAX_CHAMBER];
	static int  TEMP_TIME[MAX_CHAMBER];
	static int  TEMP_BM_SINGLE; // 2004.05.07
	static int  TEMP_MAINTINF_DB; // 2012.05.04
	static int  TEMP_SUB_AL , TEMP_SUB_IC; // 2005.12.19
	static BOOL Control;
	char *szString[]  = { "PM" , "LastOut" , "Delay" , "Clear(MSG)" };
	char *szString2[] = { "BM" , "LastOut" , "Delay" , "Clear(MSG)" };
	char *szString3[] = { "CM" , "Delay" , "Clear(MSG)" };
	int    szSize[4]  = {  50  ,        64 ,     51  ,           89 };
	int    szSize3[3] = {  114 ,     51  ,      89 };
	HWND hSWnd;
	char Buffer[32]; // 2005.12.19

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
		MoveCenterWindow( hdlg );
		for ( i = CM1 ; i <= FM ; i++ ) {
			TEMP_USE[ i ] = _i_SCH_PRM_GET_CLSOUT_USE(i);
			TEMP_TIME[ i ] = _i_SCH_PRM_GET_CLSOUT_DELAY(i);
			TEMP_MSG_USE[ i ] = _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE(i);
		}
		//
		TEMP_BM_SINGLE = _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY(); // 2004.05.07
		TEMP_MAINTINF_DB = _i_SCH_PRM_GET_MAINTINF_DB_USE(); // 2012.05.04
		//
		TEMP_SUB_AL = _i_SCH_PRM_GET_CLSOUT_AL_TO_PM(); // 2005.12.19
		TEMP_SUB_IC = _i_SCH_PRM_GET_CLSOUT_IC_TO_PM(); // 2005.12.19
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 4 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 4 , szString2 , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_BM_CHAMBER_DEPTH );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , 3 , szString3 , szSize3 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , MAX_CASSETTE_SIDE );
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
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if      ( TEMP_USE[i+PM1] == 1 )	sprintf( szString , "Use" );
										else if ( TEMP_USE[i+PM1] == 2 )	sprintf( szString , "Skip" ); // 2006.04.27
										else			  				    sprintf( szString , "" );
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 2 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_USE[i+PM1] == 1 ) {
											sprintf( szString , "%d" , TEMP_TIME[i+PM1] );
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 3 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_MSG_USE[i+PM1] )	sprintf( szString , "Use" );
										else						sprintf( szString , "" );
									}
									else {
										sprintf( szString , "-" );
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
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
										if ( TEMP_USE[i+BM1] )	sprintf( szString , "Use" );
										else					sprintf( szString , "" );
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 2 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
										if ( TEMP_USE[i+BM1] ) {
											sprintf( szString , "%d" , TEMP_TIME[i+BM1] );
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 3 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
										if ( TEMP_MSG_USE[i+BM1] )	sprintf( szString , "Use" );
										else						sprintf( szString , "" );
									}
									else {
										sprintf( szString , "-" );
									}
									break;
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
					else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
										if ( TEMP_USE[i+CM1] ) {
											sprintf( szString , "%d" , TEMP_TIME[i+CM1] );
										}
										else {
											sprintf( szString , "" );
										}
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 2 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
										if ( TEMP_MSG_USE[i+CM1] )	sprintf( szString , "Use" );
										else						sprintf( szString , "" );
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								wsprintf( szString, "Cassette%d", lpdi->item.iItem + 1 );
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
						CLSSNR_RESULT = FALSE;
						CLSSNR_DATA1  = TEMP_USE[i+PM1];
						CLSSNR_DATA2  = TEMP_TIME[i+PM1];
						CLSSNR_DATA3  = TEMP_MSG_USE[i+PM1];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CLSSNR_SUB_PAD ) , hSWnd , Gui_IDD_CLSSNR_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( CLSSNR_RESULT ) {
							TEMP_USE[i+PM1] = CLSSNR_DATA1;
							TEMP_TIME[i+PM1] = CLSSNR_DATA2;
							TEMP_MSG_USE[i+PM1] = CLSSNR_DATA3;
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
						CLSSNR_RESULT = FALSE;
						CLSSNR_DATA1  = TEMP_USE[i+BM1];
						CLSSNR_DATA2  = TEMP_TIME[i+BM1];
						CLSSNR_DATA3  = TEMP_MSG_USE[i+BM1];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CLSSNR_SUB_PAD ) , hSWnd , Gui_IDD_CLSSNR_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( CLSSNR_RESULT ) {
							TEMP_USE[i+BM1] = CLSSNR_DATA1;
							TEMP_TIME[i+BM1] = CLSSNR_DATA2;
							TEMP_MSG_USE[i+BM1] = CLSSNR_DATA3;
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
				}
				else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) ) {
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
						CLSSNR_RESULT = FALSE;
						CLSSNR_DATA2  = TEMP_TIME[i+CM1];
						CLSSNR_DATA3  = TEMP_MSG_USE[i+CM1];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX3 );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_CLSSNR_SUB_PAD3 ) , hSWnd , Gui_IDD_CLSSNR_SUB_PAD3_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( CLSSNR_RESULT ) {
							TEMP_TIME[i+CM1] = CLSSNR_DATA2;
							TEMP_MSG_USE[i+CM1] = CLSSNR_DATA3;
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
		//=============================================================================
		BeginPaint( hdlg, &ps );
		//=============================================================================
		for ( i = CM1 ; i <= FM ; i++ ) {
			if ( _i_SCH_PRM_GET_CLSOUT_USE( i )         != TEMP_USE[ i ] ) break;
			if ( _i_SCH_PRM_GET_CLSOUT_DELAY( i )       != TEMP_TIME[ i ] ) break;
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) != TEMP_MSG_USE[ i ] ) break;
		}
		if ( i > FM ) {
			if ( TEMP_MAINTINF_DB != _i_SCH_PRM_GET_MAINTINF_DB_USE() ) { // 2012.05.04
				KWIN_Item_Enable( hdlg , IDOK );
			}
			else {
				if ( TEMP_BM_SINGLE != _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() ) { // 2004.05.07
					KWIN_Item_Enable( hdlg , IDOK );
				}
				else {
					if ( TEMP_SUB_AL != _i_SCH_PRM_GET_CLSOUT_AL_TO_PM() ) { // 2005.12.19
						KWIN_Item_Enable( hdlg , IDOK );
					}
					else {
						if ( TEMP_SUB_IC != _i_SCH_PRM_GET_CLSOUT_IC_TO_PM() ) { // 2005.12.19
							KWIN_Item_Enable( hdlg , IDOK );
						}
						else {
							KWIN_Item_Disable( hdlg , IDOK );
						}
					}
				}
			}
		}
		else {
			KWIN_Item_Enable( hdlg , IDOK );
		}
		//=============================================================================
		//=============================================================================
		//=============================================================================
		for ( i = TM ; i <= FM ; i++ ) {
			switch( i ) {
			case TM :	c = IDC_CLEAR_USE_TM;	break;
			case FM :	c = IDC_CLEAR_USE_FM;	break;
			default : continue; break;
			}
			if  ( _i_SCH_PRM_GET_MODULE_MODE(i) ) {
				if ( TEMP_MSG_USE[i] )	KWIN_Item_String_Display( hdlg , c , "USE" );
				else					KWIN_Item_String_Display( hdlg , c , "-" );
			}
			else {
				KWIN_Item_Hide( hdlg , c );
			}
		}
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CLEAR_USE_BM		, TEMP_BM_SINGLE   , "-|USE|MULTI" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P1		, TEMP_MAINTINF_DB , "-|USE|NOTUSE(A)" , "????" ); // 2012.05.04
		//=============================================================================
		if ( TEMP_SUB_AL <= 0 ) { // 2005.12.19
			KWIN_Item_String_Display( hdlg , IDC_CLEAR_SUB_AL , "N/A" );
		}
		else {
			sprintf( Buffer , "PM%d" , TEMP_SUB_AL );
			KWIN_Item_String_Display( hdlg , IDC_CLEAR_SUB_AL , Buffer );
		}
		//====
		if ( TEMP_SUB_IC <= 0 ) { // 2005.12.19
			KWIN_Item_String_Display( hdlg , IDC_CLEAR_SUB_IC , "N/A" );
		}
		else {
			sprintf( Buffer , "PM%d" , TEMP_SUB_IC );
			KWIN_Item_String_Display( hdlg , IDC_CLEAR_SUB_IC , Buffer );
		}
		//=============================================================================
		EndPaint( hdlg , &ps );
		//=============================================================================
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_P1 : // 2012.05.04
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 2 , &TEMP_MAINTINF_DB );
			return TRUE;
		case IDC_CLEAR_USE_TM :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_MSG_USE[TM]) );
			return TRUE;
		case IDC_CLEAR_USE_FM :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_MSG_USE[FM]) );
			return TRUE;
		case IDC_CLEAR_USE_BM : // 2004.05.07
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 2 , &TEMP_BM_SINGLE );
			return TRUE;
		case IDC_CLEAR_SUB_AL : // 2005.12.19
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Clear/Move AL to PM Module Index" , "Select" , 0 , MAX_PM_CHAMBER_DEPTH , &TEMP_SUB_AL );
			return TRUE;

		case IDC_CLEAR_SUB_IC : // 2005.12.19
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Clear/Move IC to PM Module Index" , "Select" , 0 , MAX_PM_CHAMBER_DEPTH , &TEMP_SUB_IC );
			return TRUE;

		case IDOK :
			if ( Control ) return TRUE;
			for ( i = CM1 ; i <= FM ; i++ ) {
				 _i_SCH_PRM_SET_CLSOUT_USE( i , TEMP_USE[ i ] );
				 _i_SCH_PRM_SET_CLSOUT_DELAY( i , TEMP_TIME[ i ] );
				 _i_SCH_PRM_SET_CLSOUT_MESSAGE_USE( i , TEMP_MSG_USE[ i ] );
			}
			//
			_i_SCH_PRM_SET_CLSOUT_BM_SINGLE_ONLY( TEMP_BM_SINGLE ); // 2004.05.07
			//
			_i_SCH_PRM_SET_MAINTINF_DB_USE( TEMP_MAINTINF_DB ); // 2012.05.04
			//
			_i_SCH_PRM_SET_CLSOUT_AL_TO_PM( TEMP_SUB_AL ); // 2005.12.19
			_i_SCH_PRM_SET_CLSOUT_IC_TO_PM( TEMP_SUB_IC ); // 2005.12.19
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
