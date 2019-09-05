#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "utility.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
BOOL FIXEDRUN_RESULT = FALSE;
int  FIXEDRUN_DATA;
int  FIXEDRUN_DATA0;
int  FIXEDRUN_DATA1;
int  FIXEDRUN_DATA2;
char FIXEDRUN_DATA1_STR[64];
char FIXEDRUN_DATA2_STR[64];
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_FIXEDRUN_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	char bufferdir[256] , buffer[256];
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
		FIXEDRUN_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		if ( FIXEDRUN_DATA0 != 0 ) {

			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B0  , FIXEDRUN_DATA0 , "|Use when first not selected|Use when everytime" , "????" ); // 2007.07.24

			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1  , FIXEDRUN_DATA1 , "x|O" , "????" ); // 2007.07.24

			if ( FIXEDRUN_DATA1 ) {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , FIXEDRUN_DATA1_STR );
			}
			else {
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
			}

			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3  , FIXEDRUN_DATA2 , "x|O" , "????" ); // 2007.07.24

			if ( FIXEDRUN_DATA2 ) {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , FIXEDRUN_DATA2_STR );
			}
			else {
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
			}
		}
		else {
			KWIN_Item_String_Display( hdlg , IDC_READY_USE_B0 , "" );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B1 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B3 );
			KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
		}
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B0 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &FIXEDRUN_DATA0 );
			return TRUE;
		case IDC_READY_USE_B1 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &FIXEDRUN_DATA1 );
			return TRUE;
		case IDC_READY_USE_B2 :
			if ( GUI_GROUP_SELECT_GET() == 0 )
				sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( FIXEDRUN_DATA ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( FIXEDRUN_DATA ) );
			else
				sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( FIXEDRUN_DATA ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( FIXEDRUN_DATA ) );
			if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Recipe Name" , "*.*" , "" , buffer ) ) {
				if ( GUI_GROUP_SELECT_GET() == 2 ) { // 2002.05.10
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					UTIL_MAKE_DYNAMIC_GROUP_FILE( FIXEDRUN_DATA1_STR , bufferdir , 63 );
				}
				else {
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					strncpy( FIXEDRUN_DATA1_STR , bufferdir , 63 );
				}
				InvalidateRect( hdlg , NULL , TRUE );
			}
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 1 , &FIXEDRUN_DATA2 );
			return TRUE;
		case IDC_READY_USE_B4 :
			if ( GUI_GROUP_SELECT_GET() == 0 )
				sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( FIXEDRUN_DATA ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( FIXEDRUN_DATA ) );
			else
				sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( FIXEDRUN_DATA ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( FIXEDRUN_DATA ) );
			if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Recipe Name" , "*.*" , "" , buffer ) ) {
				if ( GUI_GROUP_SELECT_GET() == 2 ) { // 2002.05.10
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					UTIL_MAKE_DYNAMIC_GROUP_FILE( FIXEDRUN_DATA2_STR , bufferdir , 63 );
				}
				else {
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					strncpy( FIXEDRUN_DATA2_STR , bufferdir , 63 );
				}
				InvalidateRect( hdlg , NULL , TRUE );
			}
			return TRUE;
		case IDOK :		FIXEDRUN_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_FIXEDCLST_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i;
	static int  TEMP_READY_MODE[MAX_CHAMBER];
	static int  TEMP_READY_IN_USE[MAX_CHAMBER];
	static int  TEMP_READY_OUT_USE[MAX_CHAMBER];
	static char TEMP_READY_IN_RECIPE[MAX_CHAMBER][64];
	static char TEMP_READY_OUT_RECIPE[MAX_CHAMBER][64];
	static int  TEMP_END_MODE[MAX_CHAMBER];
	static int  TEMP_END_IN_USE[MAX_CHAMBER];
	static int  TEMP_END_OUT_USE[MAX_CHAMBER];
	static char TEMP_END_IN_RECIPE[MAX_CHAMBER][64];
	static char TEMP_END_OUT_RECIPE[MAX_CHAMBER][64];
	static BOOL Control;
	char *szString[] = { "PM" , "Use" , "In Recipe" , "Out Recipe" };
	int    szSize[4] = {  50  ,   190 ,        170  ,          170 };
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
		Control = (BOOL) lParam;
		//------------------------------------------------------------
		MoveCenterWindow( hdlg );
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			TEMP_READY_MODE[ i+PM1 ] = _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE(i+PM1);
			TEMP_READY_IN_USE[ i+PM1 ] = _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE(i+PM1);
			TEMP_READY_OUT_USE[ i+PM1 ] = _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE(i+PM1);
			strncpy( TEMP_READY_IN_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME(i+PM1) , 63 );
			strncpy( TEMP_READY_OUT_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME(i+PM1) , 63 );
			TEMP_END_MODE[ i+PM1 ] = _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE(i+PM1);
			TEMP_END_IN_USE[ i+PM1 ] = _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE(i+PM1);
			TEMP_END_OUT_USE[ i+PM1 ] = _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE(i+PM1);
			strncpy( TEMP_END_IN_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME(i+PM1) , 63 );
			strncpy( TEMP_END_OUT_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME(i+PM1) , 63 );
		}
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 4 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 4 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_PM_CHAMBER_DEPTH );
		//
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i;
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_READY_MODE[i+PM1] != 0 ) {
											if      ( TEMP_READY_MODE[i+PM1] == 1 ) sprintf( szString , "Use when first not selected" );
											else if ( TEMP_READY_MODE[i+PM1] == 2 ) sprintf( szString , "Use when everytime" );
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
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_READY_MODE[i+PM1] != 0 ) {
											if ( TEMP_READY_IN_USE[i+PM1] ) {
												sprintf( szString , "(O) %s" , TEMP_READY_IN_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "(x)" );
											}
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
										if ( TEMP_READY_MODE[i+PM1] != 0 ) {
											if ( TEMP_READY_OUT_USE[i+PM1] ) {
												sprintf( szString , "(O) %s" , TEMP_READY_OUT_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "(x)" );
											}
										}
										else {
											sprintf( szString , "" );
										}
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
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;

				case NM_DBLCLK :
					if ( Control ) return TRUE;
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
						FIXEDRUN_RESULT = FALSE;
						FIXEDRUN_DATA  = i+PM1;
						FIXEDRUN_DATA0 = TEMP_READY_MODE[i+PM1];
						FIXEDRUN_DATA1 = TEMP_READY_IN_USE[i+PM1];
						strncpy( FIXEDRUN_DATA1_STR , TEMP_READY_IN_RECIPE[i+PM1] , 63 );
						FIXEDRUN_DATA2 = TEMP_READY_OUT_USE[i+PM1];
						strncpy( FIXEDRUN_DATA2_STR , TEMP_READY_OUT_RECIPE[i+PM1] , 63 );
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_FIXEDRUN_SUB_PAD ) , hSWnd , Gui_IDD_FIXEDRUN_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( FIXEDRUN_RESULT ) {
							TEMP_READY_MODE[i+PM1] = FIXEDRUN_DATA0;
							TEMP_READY_IN_USE[i+PM1] = FIXEDRUN_DATA1;
							strncpy( TEMP_READY_IN_RECIPE[i+PM1] , FIXEDRUN_DATA1_STR , 63 );
							TEMP_READY_OUT_USE[i+PM1] = FIXEDRUN_DATA2;
							strncpy( TEMP_READY_OUT_RECIPE[i+PM1] , FIXEDRUN_DATA2_STR , 63 );
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
				}
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_END_MODE[i+PM1] != 0 ) {
											if      ( TEMP_END_MODE[i+PM1] == 1 ) sprintf( szString , "Use when first not selected" );
											else if ( TEMP_END_MODE[i+PM1] == 2 ) sprintf( szString , "Use when everytime" );
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
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_END_MODE[i+PM1] != 0 ) {
											if ( TEMP_END_IN_USE[i+PM1] ) {
												sprintf( szString , "(O) %s" , TEMP_END_IN_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "(x)" );
											}
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
										if ( TEMP_END_MODE[i+PM1] != 0 ) {
											if ( TEMP_END_OUT_USE[i+PM1] ) {
												sprintf( szString , "(O) %s" , TEMP_END_OUT_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "(x)" );
											}
										}
										else {
											sprintf( szString , "" );
										}
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
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					return 0;

				case NM_DBLCLK :
					if ( Control ) return TRUE;
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
						FIXEDRUN_RESULT = FALSE;
						FIXEDRUN_DATA  = i+PM1;
						FIXEDRUN_DATA0 = TEMP_END_MODE[i+PM1];
						FIXEDRUN_DATA1 = TEMP_END_IN_USE[i+PM1];
						strncpy( FIXEDRUN_DATA1_STR , TEMP_END_IN_RECIPE[i+PM1] , 63 );
						FIXEDRUN_DATA2 = TEMP_END_OUT_USE[i+PM1];
						strncpy( FIXEDRUN_DATA2_STR , TEMP_END_OUT_RECIPE[i+PM1] , 63 );
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_FIXEDRUN_SUB_PAD ) , hSWnd , Gui_IDD_FIXEDRUN_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( FIXEDRUN_RESULT ) {
							TEMP_END_MODE[i+PM1] = FIXEDRUN_DATA0;
							TEMP_END_IN_USE[i+PM1] = FIXEDRUN_DATA1;
							strncpy( TEMP_END_IN_RECIPE[i+PM1] , FIXEDRUN_DATA1_STR , 63 );
							TEMP_END_OUT_USE[i+PM1] = FIXEDRUN_DATA2;
							strncpy( TEMP_END_OUT_RECIPE[i+PM1] , FIXEDRUN_DATA2_STR , 63 );
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
				}
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );

		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_GROUP_CONTROL  , GUI_GROUP_SELECT_GET() , "Single Mode|Group Mode(Fixed)|Group Mode(Dynamic)" , "????" ); // 2007.07.24

		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_MODE( i+PM1 ) != TEMP_READY_MODE[ i+PM1 ] ) break;
			if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_USE( i+PM1 ) != TEMP_READY_IN_USE[ i+PM1 ] ) break;
			if ( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_USE( i+PM1 ) != TEMP_READY_OUT_USE[ i+PM1 ] ) break;
			if ( !STRCMP_L( _i_SCH_PRM_GET_FIXCLUSTER_PRE_IN_RECIPE_NAME( i+PM1 ) , TEMP_READY_IN_RECIPE[ i+PM1 ] ) ) break;
			if ( !STRCMP_L( _i_SCH_PRM_GET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( i+PM1 ) , TEMP_READY_OUT_RECIPE[ i+PM1 ] ) ) break;

			if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_MODE( i+PM1 ) != TEMP_END_MODE[ i+PM1 ] ) break;
			if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_USE( i+PM1 ) != TEMP_END_IN_USE[ i+PM1 ] ) break;
			if ( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_USE( i+PM1 ) != TEMP_END_OUT_USE[ i+PM1 ] ) break;
			if ( !STRCMP_L( _i_SCH_PRM_GET_FIXCLUSTER_POST_IN_RECIPE_NAME( i+PM1 ) , TEMP_END_IN_RECIPE[ i+PM1 ] ) ) break;
			if ( !STRCMP_L( _i_SCH_PRM_GET_FIXCLUSTER_POST_OUT_RECIPE_NAME( i+PM1 ) , TEMP_END_OUT_RECIPE[ i+PM1 ] ) ) break;
		}
		if ( i >= MAX_PM_CHAMBER_DEPTH )	KWIN_Item_Disable( hdlg , IDOK );
		else								KWIN_Item_Enable( hdlg , IDOK );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_GROUP_CONTROL :
			if ( Control ) return TRUE;
			switch( GUI_GROUP_SELECT_GET() ) { // 2002.05.10
			case 0 :
				GUI_GROUP_SELECT_SET( 1 );
				break;
			case 1 :
				GUI_GROUP_SELECT_SET( 2 );
				break;
			case 2 :
				GUI_GROUP_SELECT_SET( 0 );
				break;
			}
			InvalidateRect( hdlg , NULL , TRUE );
			return TRUE;

		case IDOK :
			if ( Control ) return TRUE;
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_MODE( i+PM1 , TEMP_READY_MODE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_USE( i+PM1 , TEMP_READY_IN_USE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_USE( i+PM1 , TEMP_READY_OUT_USE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_IN_RECIPE_NAME( i+PM1 , TEMP_READY_IN_RECIPE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_PRE_OUT_RECIPE_NAME( i+PM1 , TEMP_READY_OUT_RECIPE[ i+PM1 ] );

				_i_SCH_PRM_SET_FIXCLUSTER_POST_MODE( i+PM1 , TEMP_END_MODE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_USE( i+PM1 , TEMP_END_IN_USE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_USE( i+PM1 , TEMP_END_OUT_USE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_IN_RECIPE_NAME( i+PM1 , TEMP_END_IN_RECIPE[ i+PM1 ] );
				_i_SCH_PRM_SET_FIXCLUSTER_POST_OUT_RECIPE_NAME( i+PM1 , TEMP_END_OUT_RECIPE[ i+PM1 ] );
			}
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
