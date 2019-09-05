#include "default.h"

#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "Gui_Handling.h"
#include "resource.h"
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_RCPFILE_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	char *szString[] = { "Chamber" , "Directory" , "Mode" };
	int    szSize[3] = {       80  ,         254 ,     80 };

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
		MoveCenterWindow( hdlg );
		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CONTROL_RCP_LOCK , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , "Off|Locking|Locking(A)" , "????" ); // 2007.07.24 // 2015.07.20
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CONTROL_RCP_LOCK , _i_SCH_PRM_GET_DFIX_RECIPE_LOCKING() , "Off|Locking|Locking(A)|Locking(S)" , "????" ); // 2007.07.24 // 2015.07.20
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CONTROL_RCP_TYPE , _i_SCH_PRM_GET_DFIX_CONTROL_RECIPE_TYPE() , "ALL_RECIPE|LOT_PROCESS_RECIPE|CLUSTER_RECIPE|PROCESS_RECIPE" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_Item_String_Display( hdlg , IDC_MAIN_RCP_PATH		, _i_SCH_PRM_GET_DFIX_MAIN_RECIPE_PATHF() );
		KWIN_Item_String_Display( hdlg , IDC_LOT_RCP_PATH		, _i_SCH_PRM_GET_DFIX_LOT_RECIPE_PATH() );
		KWIN_Item_String_Display( hdlg , IDC_CLUSTER_RCP_PATH	, _i_SCH_PRM_GET_DFIX_CLUSTER_RECIPE_PATH() );

		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 3 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
//		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH ); // 2002.03.27
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH ); // 2002.03.27
		//
		return TRUE;

	case WM_NOTIFY: {
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i;
			switch( lpnmh->code ) {
				case LVN_GETDISPINFO: {
						LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;
						TCHAR szString[ 256 ];
						//
						szString[0] = 0; // 2016.02.02
						//
						if ( lpdi->item.iSubItem ) {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								i = lpdi->item.iItem;
								switch( lpdi->item.iSubItem ) {
								case 1 :
									if ( i >= MAX_PM_CHAMBER_DEPTH ) { // 2002.03.27
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1-MAX_PM_CHAMBER_DEPTH) )
											sprintf( szString , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( i+BM1-MAX_PM_CHAMBER_DEPTH ) );
										else
											sprintf( szString , "(x)" );
									}
									else {
										if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) )
											sprintf( szString , "%s" , _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_PATHF( i + PM1 ) );
										else
											sprintf( szString , "(x)" );
									}
									break;
								case 2 : // 2007.03.15
									if ( i >= MAX_PM_CHAMBER_DEPTH ) {
										sprintf( szString , "" );
									}
									else {
										if ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
											switch( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_TYPE( i + PM1 ) ) {
											case 1 :
												switch( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( i + PM1 ) ) {
												case 1 :
													sprintf( szString , "Name:Text" );
													break;
												case 2 :
													sprintf( szString , "Name:Src" );
													break;
												default :
													sprintf( szString , "Name" );
													break;
												}
												break;
											case 2 :
												switch( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( i + PM1 ) ) {
												case 1 :
													sprintf( szString , "NameAll:Text" );
													break;
												case 2 :
													sprintf( szString , "NameAll:Src" );
													break;
												default :
													sprintf( szString , "NameAll" );
													break;
												}
												break;
											default :
												switch( _i_SCH_PRM_GET_DFIX_PROCESS_RECIPE_FILE( i + PM1 ) ) {
												case 1 :
													sprintf( szString , ":Text" );
													break;
												case 2 :
													sprintf( szString , ":Src" );
													break;
												default :
													sprintf( szString , "" );
													break;
												}
												break;
											}
										}
										else {
											sprintf( szString , "" );
										}
									}
									break;
								}
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								if ( lpdi->item.iItem >= MAX_PM_CHAMBER_DEPTH ) { // 2002.03.27
									wsprintf( szString, "BM%d", lpdi->item.iItem + 1 - MAX_PM_CHAMBER_DEPTH );
								}
								else {
									wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
								}
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
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , NULL , FALSE );
					return 0;
				case LVN_ITEMCHANGED :	return 0;
				case LVN_KEYDOWN :		return 0;
			}
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
