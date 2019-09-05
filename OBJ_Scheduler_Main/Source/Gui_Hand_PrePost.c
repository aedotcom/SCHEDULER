#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "utility.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL RERCP_RESULT = FALSE;
int  RERCP_DATA0;
int  RERCP_DATA1;
int  RERCP_DATA2;
//
int  RERCP_DATA3;
int  RERCP_DATA4;
//
char RERCP_DATA1_STR[64];
char RERCP_DATA2_STR[64];
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_RERCP_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
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
		RERCP_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		if ( ( RERCP_DATA0 >= PM1 ) && ( RERCP_DATA0 < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) { // 2002.07.30
			switch( RERCP_DATA1 ) {
			case 1 :
			case 2 :
			case 3 : // 2005.02.17
			case 4 : // 2005.02.17
			case 5 : // 2005.02.17
			case 6 : // 2005.02.17
			case 7 : // 2005.02.17
			case 8 : // 2005.02.17
			case 9 : // 2005.02.17
			case 10 : // 2007.04.02
			case 11 : // 2007.04.02
			case 12 : // 2017.09.07
			case 13 : // 2017.09.15
				if ( RERCP_DATA1 == 1 )	sprintf( buffer , "USE" );
				else					sprintf( buffer , "USE%d" , RERCP_DATA1 );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , buffer );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , RERCP_DATA1_STR );
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B5 , RERCP_DATA3 );
				break;
			default :
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "-" );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B5 );
				break;
			}
			switch( RERCP_DATA2 ) {
			case 1 :
			case 2 :
			case 3 : // 2004.10.08
			case 4 : // 2019.02.13
				if ( RERCP_DATA2 == 1 )	sprintf( buffer , "USE" );
				else					sprintf( buffer , "USE%d" , RERCP_DATA2 );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , buffer );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , RERCP_DATA2_STR );
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B6 , RERCP_DATA4 );
				break;
			default :
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "-" );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B6 );
				break;
			}
		}
		else {
			if ( RERCP_DATA1 == 1 ) {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "USE" );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , RERCP_DATA1_STR );
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B5 , RERCP_DATA3 );
			}
			else if ( RERCP_DATA1 == 2 ) { // 2006.07.03
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "USE2" );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "[Info]" );
				KWIN_Item_Disable( hdlg , IDC_READY_USE_B2 );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B5 );
			}
			else {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "-" );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B2 );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B5 );
			}
			if ( RERCP_DATA2 == 1 ) {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "USE" );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , RERCP_DATA2_STR );
				KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B6 , RERCP_DATA4 );
			}
			else if ( RERCP_DATA2 == 2 ) { // 2006.07.03
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "USE2" );
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "[Info]" );
				KWIN_Item_Disable( hdlg , IDC_READY_USE_B4 );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B6 );
			}
			else {
				KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "-" );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B4 );
				KWIN_Item_Hide( hdlg , IDC_READY_USE_B6 );
			}
		}
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			if ( ( RERCP_DATA0 >= PM1 ) && ( RERCP_DATA0 < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) { // 2002.08.27
//				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 9 , &RERCP_DATA1 ); // 2007.04.02
//				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 11 , &RERCP_DATA1 ); // 2007.04.02 // 2017.09.07
//				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 12 , &RERCP_DATA1 ); // 2007.04.02 // 2017.09.07 // 2017.09.15
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 13 , &RERCP_DATA1 ); // 2007.04.02 // 2017.09.07 // 2017.09.15
			}
			else {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &RERCP_DATA1 );
			}
			return TRUE;
		case IDC_READY_USE_B2 :
			if ( GUI_GROUP_SELECT_GET() == 0 )
				sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( RERCP_DATA0 ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( RERCP_DATA0 ) );
			else
				sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( RERCP_DATA0 ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( RERCP_DATA0 ) );
			if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Pre Recipe Name" , "*.*" , "" , buffer ) ) {
				if ( GUI_GROUP_SELECT_GET() == 2 ) { // 2002.05.10
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					UTIL_MAKE_DYNAMIC_GROUP_FILE( RERCP_DATA1_STR , bufferdir , 63 );
				}
				else {
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					strncpy( RERCP_DATA1_STR , bufferdir , 63 );
				}
				InvalidateRect( hdlg , NULL , TRUE );
			}
			return TRUE;
		case IDC_READY_USE_B3 :
			if ( ( RERCP_DATA0 >= PM1 ) && ( RERCP_DATA0 < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) { // 2002.07.30
//				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 3 , &RERCP_DATA2 ); // 2019.02.13
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 4 , &RERCP_DATA2 ); // 2019.02.13
			}
			else {
				KWIN_GUI_SELECT_DATA_INCREASE( FALSE , hdlg , 2 , &RERCP_DATA2 );
			}
			return TRUE;
		case IDC_READY_USE_B4 :
			if ( GUI_GROUP_SELECT_GET() == 0 )
				sprintf( bufferdir , "%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( RERCP_DATA0 ) , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( RERCP_DATA0 ) );
			else
				sprintf( bufferdir , "%s/%s/%s" , _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATH( RERCP_DATA0 ) , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATH( RERCP_DATA0 ) );
			if ( MODAL_FILE_OPEN_BOX( hdlg , bufferdir , "Select Post Recipe Name" , "*.*" , "" , buffer ) ) {
				if ( GUI_GROUP_SELECT_GET() == 2 ) { // 2002.05.10
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					UTIL_MAKE_DYNAMIC_GROUP_FILE( RERCP_DATA2_STR , bufferdir , 63 );
				}
				else {
					Ext_Name_From_FullFile( bufferdir , buffer , 99 );
					strncpy( RERCP_DATA2_STR , bufferdir , 63 );
				}
				InvalidateRect( hdlg , NULL , TRUE );
			}
			return TRUE;
		case IDC_READY_USE_B5 : // 2005.08.18
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Minimum Process Time" , "Select" , 0 , 9999 , &RERCP_DATA3 );
			return TRUE;
		case IDC_READY_USE_B6 : // 2005.08.18
			KWIN_GUI_MODAL_DIGITAL_BOX2( FALSE , hdlg , "Minimum Process Time" , "Select" , 0 , 9999 , &RERCP_DATA4 );
			return TRUE;
		case IDOK :		RERCP_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_RERCP_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i;
	static int	TEMP_DATA;
	static int  TEMP_READY_USE[MAX_CHAMBER];
	static int  TEMP_READY_MINTIME[MAX_CHAMBER];
//	static char TEMP_READY_RECIPE[MAX_CHAMBER][64]; // 2007.01.04
	static char TEMP_READY_RECIPE[MAX_CHAMBER][513]; // 2007.01.04
	static int  TEMP_END_USE[MAX_CHAMBER];
	static int  TEMP_END_MINTIME[MAX_CHAMBER];
//	static char TEMP_END_RECIPE[MAX_CHAMBER][64]; // 2007.01.04
	static char TEMP_END_RECIPE[MAX_CHAMBER][513]; // 2007.01.04
	static BOOL Control;
	char *szString[] = { "PM" , "Pre Recipe" , "Post Recipe" };
	int    szSize[3] = {  50  ,         231  ,           231 };
	char *szString2[] = { "BM" , "TM Side Recipe" , "FM Side Recipe" };
	int    szSize2[3] = {  50  ,         231      ,              231 };
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
		//
		TEMP_DATA  = _i_SCH_PRM_GET_READY_MULTIJOB_MODE(); // 2007.03.15
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			TEMP_READY_USE[ i+PM1 ] = _i_SCH_PRM_GET_READY_RECIPE_USE(i+PM1);
			TEMP_READY_MINTIME[ i+PM1 ] = _i_SCH_PRM_GET_READY_RECIPE_MINTIME(i+PM1);
//			strncpy( TEMP_READY_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_READY_RECIPE_NAME(i+PM1) , 63 ); // 2007.01.04
			strncpy( TEMP_READY_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_READY_RECIPE_NAME(i+PM1) , 512 ); // 2007.01.04
			TEMP_END_USE[ i+PM1 ] = _i_SCH_PRM_GET_END_RECIPE_USE(i+PM1);
			TEMP_END_MINTIME[ i+PM1 ] = _i_SCH_PRM_GET_END_RECIPE_MINTIME(i+PM1);
//			strncpy( TEMP_END_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_END_RECIPE_NAME(i+PM1) , 63 ); // 2007.01.04
			strncpy( TEMP_END_RECIPE[ i+PM1 ] , _i_SCH_PRM_GET_END_RECIPE_NAME(i+PM1) , 512 ); // 2007.01.04
		}
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			TEMP_READY_USE[ i+BM1 ] = _i_SCH_PRM_GET_READY_RECIPE_USE(i+BM1);
			TEMP_READY_MINTIME[ i+BM1 ] = _i_SCH_PRM_GET_READY_RECIPE_MINTIME(i+BM1);
//			strncpy( TEMP_READY_RECIPE[ i+BM1 ] , _i_SCH_PRM_GET_READY_RECIPE_NAME(i+BM1) , 63 ); // 2007.01.04
			strncpy( TEMP_READY_RECIPE[ i+BM1 ] , _i_SCH_PRM_GET_READY_RECIPE_NAME(i+BM1) , 512 ); // 2007.01.04
			TEMP_END_USE[ i+BM1 ] = _i_SCH_PRM_GET_END_RECIPE_USE(i+BM1);
			TEMP_END_MINTIME[ i+BM1 ] = _i_SCH_PRM_GET_END_RECIPE_MINTIME(i+BM1);
//			strncpy( TEMP_END_RECIPE[ i+BM1 ] , _i_SCH_PRM_GET_END_RECIPE_NAME(i+BM1) , 63 ); // 2007.01.04
			strncpy( TEMP_END_RECIPE[ i+BM1 ] , _i_SCH_PRM_GET_END_RECIPE_NAME(i+BM1) , 512 ); // 2007.01.04
		}
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 3 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 3 , szString2 , szSize2 );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_BM_CHAMBER_DEPTH );
		//
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i;
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
				switch( lpnmh->code ) {
				case LVN_GETDISPINFO:	{
						LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
//						TCHAR szString[ 256 ]; // 2007.01.04
						TCHAR szString[ 540 ]; // 2007.01.04
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_READY_USE[i+PM1] == 0 ) {
											sprintf( szString , "" );
										}
										else if ( TEMP_READY_USE[i+PM1] == 1 ) {
											if ( TEMP_READY_MINTIME[i+PM1] > 0 ) {
												sprintf( szString , "[Use:%d] %s" , TEMP_READY_MINTIME[i+PM1] , TEMP_READY_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "[Use] %s" , TEMP_READY_RECIPE[i+PM1] );
											}
										}
										else {
											if ( TEMP_READY_MINTIME[i+PM1] > 0 ) {
												sprintf( szString , "[Use%d:%d] %s" , TEMP_READY_USE[i+PM1] , TEMP_READY_MINTIME[i+PM1] , TEMP_READY_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "[Use%d] %s" , TEMP_READY_USE[i+PM1] , TEMP_READY_RECIPE[i+PM1] );
											}
										}
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 2 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
										if ( TEMP_END_USE[i+PM1] == 0 ) {
											sprintf( szString , "" );
										}
										else if ( TEMP_END_USE[i+PM1] == 1 ) {
											if ( TEMP_END_MINTIME[i+PM1] > 0 ) {
												sprintf( szString , "[Use:%d] %s" , TEMP_END_MINTIME[i+PM1] , TEMP_END_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "[Use] %s" , TEMP_END_RECIPE[i+PM1] );
											}
										}
										else {
											if ( TEMP_END_MINTIME[i+PM1] > 0 ) {
												sprintf( szString , "[Use%d:%d] %s" , TEMP_END_USE[i+PM1] , TEMP_END_MINTIME[i+PM1] , TEMP_END_RECIPE[i+PM1] );
											}
											else {
												sprintf( szString , "[Use%d] %s" , TEMP_END_USE[i+PM1] , TEMP_END_RECIPE[i+PM1] );
											}
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
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
						i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
						if ( i < 0 ) return 0;
						if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
							RERCP_RESULT = FALSE;
							RERCP_DATA0 = i + PM1;
							RERCP_DATA1 = TEMP_READY_USE[i+PM1];
							strncpy( RERCP_DATA1_STR , TEMP_READY_RECIPE[i+PM1] , 63 );
							RERCP_DATA2 = TEMP_END_USE[i+PM1];
							strncpy( RERCP_DATA2_STR , TEMP_END_RECIPE[i+PM1] , 63 );
							//
							RERCP_DATA3 = TEMP_READY_MINTIME[i+PM1];
							RERCP_DATA4 = TEMP_END_MINTIME[i+PM1];
							//
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
//							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_RERCP_SUB_PAD ) , hSWnd , Gui_IDD_RERCP_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19
//							GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_RERCP_SUB_PAD ) , hSWnd , Gui_IDD_RERCP_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_RERCP_SUB_PAD ) , hSWnd , Gui_IDD_RERCP_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							if ( RERCP_RESULT ) {
								TEMP_READY_USE[i+PM1] = RERCP_DATA1;
								strncpy( TEMP_READY_RECIPE[i+PM1] , RERCP_DATA1_STR , 63 );
								TEMP_END_USE[i+PM1] = RERCP_DATA2;
								strncpy( TEMP_END_RECIPE[i+PM1] , RERCP_DATA2_STR , 63 );
								//
								TEMP_READY_MINTIME[i+PM1] = RERCP_DATA3;
								TEMP_END_MINTIME[i+PM1] = RERCP_DATA4;
								InvalidateRect( hdlg , NULL , FALSE );
							}
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
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
										if ( TEMP_READY_USE[i+BM1] == 1 ) {
											sprintf( szString , "[Use] %s" , TEMP_READY_RECIPE[i+BM1] );
										}
										else if ( TEMP_READY_USE[i+BM1] == 2 ) { // 2006.07.03
											sprintf( szString , "[Use2] Info" );
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
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
										if ( TEMP_END_USE[i+BM1] == 1 )	{
											sprintf( szString , "[Use] %s" , TEMP_END_RECIPE[i+BM1] );
										}
										else if ( TEMP_END_USE[i+BM1] == 2 ) { // 2006.07.03
											sprintf( szString , "[Use2] Info" );
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
								wsprintf( szString, "BM%d", lpdi->item.iItem + 1 );
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
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) ) {
						i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
						if ( i < 0 ) return 0;
						if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) ) {
							RERCP_RESULT = FALSE;
							RERCP_DATA0 = i + BM1;
							RERCP_DATA1 = TEMP_READY_USE[i+BM1];
							strncpy( RERCP_DATA1_STR , TEMP_READY_RECIPE[i+BM1] , 63 );
							RERCP_DATA2 = TEMP_END_USE[i+BM1];
							strncpy( RERCP_DATA2_STR , TEMP_END_RECIPE[i+BM1] , 63 );
							//
							RERCP_DATA3 = TEMP_READY_MINTIME[i+BM1];
							RERCP_DATA4 = TEMP_END_MINTIME[i+BM1];
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
//							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_RERCP_SUB2_PAD ) , hSWnd , Gui_IDD_RERCP_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19
//							GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_RERCP_SUB2_PAD ) , hSWnd , Gui_IDD_RERCP_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_RERCP_SUB2_PAD ) , hSWnd , Gui_IDD_RERCP_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							if ( RERCP_RESULT ) {
								TEMP_READY_USE[i+BM1] = RERCP_DATA1;
								strncpy( TEMP_READY_RECIPE[i+BM1] , RERCP_DATA1_STR , 63 );
								TEMP_END_USE[i+BM1] = RERCP_DATA2;
								strncpy( TEMP_END_RECIPE[i+BM1] , RERCP_DATA2_STR , 63 );
								//
								TEMP_READY_MINTIME[i+BM1] = RERCP_DATA3;
								TEMP_END_MINTIME[i+BM1] = RERCP_DATA4;
								InvalidateRect( hdlg , NULL , FALSE );
							}
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
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_GROUP_CONTROL , GUI_GROUP_SELECT_GET() , "Single Mode|Group Mode(Fixed)|Group Mode(Dynamic)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDYES , TEMP_DATA , "N/A|On" , "N/A" ); // 2007.07.24
		//---------------------------------------------------------------
		//
		if ( TEMP_DATA != _i_SCH_PRM_GET_READY_MULTIJOB_MODE() ) { // 2007.03.15
			KWIN_Item_Enable( hdlg , IDOK );
		}
		else {
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				if ( _i_SCH_PRM_GET_READY_RECIPE_USE( i+PM1 ) != TEMP_READY_USE[ i+PM1 ] ) break;
				if ( _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i+PM1 ) != TEMP_READY_MINTIME[ i+PM1 ] ) break;
				if ( !STRCMP_L( _i_SCH_PRM_GET_READY_RECIPE_NAME( i+PM1 ) , TEMP_READY_RECIPE[ i+PM1 ] ) ) break;
				//
				if ( _i_SCH_PRM_GET_END_RECIPE_USE( i+PM1 ) != TEMP_END_USE[ i+PM1 ] ) break;
				if ( _i_SCH_PRM_GET_END_RECIPE_MINTIME( i+PM1 ) != TEMP_END_MINTIME[ i+PM1 ] ) break;
				if ( !STRCMP_L( _i_SCH_PRM_GET_END_RECIPE_NAME( i+PM1 ) , TEMP_END_RECIPE[ i+PM1 ] ) ) break;
			}
			if ( i >= MAX_PM_CHAMBER_DEPTH )	{
				for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
					if ( _i_SCH_PRM_GET_READY_RECIPE_USE( i+BM1 ) != TEMP_READY_USE[ i+BM1 ] ) break;
					if ( _i_SCH_PRM_GET_READY_RECIPE_MINTIME( i+BM1 ) != TEMP_READY_MINTIME[ i+BM1 ] ) break;
					if ( !STRCMP_L( _i_SCH_PRM_GET_READY_RECIPE_NAME( i+BM1 ) , TEMP_READY_RECIPE[ i+BM1 ] ) ) break;
					//
					if ( _i_SCH_PRM_GET_END_RECIPE_USE( i+BM1 ) != TEMP_END_USE[ i+BM1 ] ) break;
					if ( _i_SCH_PRM_GET_END_RECIPE_MINTIME( i+BM1 ) != TEMP_END_MINTIME[ i+BM1 ] ) break;
					if ( !STRCMP_L( _i_SCH_PRM_GET_END_RECIPE_NAME( i+BM1 ) , TEMP_END_RECIPE[ i+BM1 ] ) ) break;
				}
				if ( i >= MAX_BM_CHAMBER_DEPTH )	{
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
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_GROUP_CONTROL :
			if ( Control ) return TRUE;
			switch ( GUI_GROUP_SELECT_GET() ) {
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

		case IDYES :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &TEMP_DATA );
			return TRUE;

		case IDOK :
			if ( Control ) return TRUE;
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_READY_RECIPE_MINTIME( i+PM1 , TEMP_READY_MINTIME[ i+PM1 ] );
				_i_SCH_PRM_SET_READY_RECIPE_NAME( i+PM1 , TEMP_READY_RECIPE[ i+PM1 ] );
				_i_SCH_PRM_SET_READY_RECIPE_USE( i+PM1 , TEMP_READY_USE[ i+PM1 ] );
				//
				_i_SCH_PRM_SET_END_RECIPE_MINTIME( i+PM1 , TEMP_END_MINTIME[ i+PM1 ] );
				_i_SCH_PRM_SET_END_RECIPE_NAME( i+PM1 , TEMP_END_RECIPE[ i+PM1 ] );
				_i_SCH_PRM_SET_END_RECIPE_USE( i+PM1 , TEMP_END_USE[ i+PM1 ] );
			}
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				_i_SCH_PRM_SET_READY_RECIPE_MINTIME( i+BM1 , TEMP_READY_MINTIME[ i+BM1 ] );
				_i_SCH_PRM_SET_READY_RECIPE_NAME( i+BM1 , TEMP_READY_RECIPE[ i+BM1 ] );
				_i_SCH_PRM_SET_READY_RECIPE_USE( i+BM1 , TEMP_READY_USE[ i+BM1 ] );
				//
				_i_SCH_PRM_SET_END_RECIPE_MINTIME( i+BM1 , TEMP_END_MINTIME[ i+BM1 ] );
				_i_SCH_PRM_SET_END_RECIPE_NAME( i+BM1 , TEMP_END_RECIPE[ i+BM1 ] );
				_i_SCH_PRM_SET_END_RECIPE_USE( i+BM1 , TEMP_END_USE[ i+BM1 ] );
			}
			//
			_i_SCH_PRM_SET_READY_MULTIJOB_MODE( TEMP_DATA ); // 2007.03.15
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
