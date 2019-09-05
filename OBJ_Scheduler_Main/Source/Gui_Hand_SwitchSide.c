#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "system_tag.h"
#include "resource.h"

#include "sch_Commonuser.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL HANDLINGSIDE_RESULT = FALSE;
int  HANDLINGSIDE_DATA1;
int  HANDLINGSIDE_OFFSET;
//
BOOL SWITCHSIDE_RESULT = FALSE;
int  SWITCHSIDE_DATA1;
int  SWITCHSIDE_DATA2;
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_SWITCHSIDE_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i;
	char Buffer[1024];
	char Buffer2[32];
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
		SWITCHSIDE_RESULT = FALSE;
		return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		if ( SWITCHSIDE_DATA1 == 0 ) {
			sprintf( Buffer , "default" );
		}
		else {
			if ( SWITCHSIDE_DATA1 > MAX_BM_CHAMBER_DEPTH ) { // 2003.11.28
				sprintf( Buffer , "BM%d(C)" , SWITCHSIDE_DATA1 - MAX_BM_CHAMBER_DEPTH ); // 2003.11.28
			}
			else {
				sprintf( Buffer , "BM%d" , SWITCHSIDE_DATA1 );
			}
		}
		KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , Buffer );
		//--------------------------------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 , SWITCHSIDE_DATA2 , "Ordering|Overlapped|Ordering(F)|Overlapped(F)|Ordering(L)|Overlapped(L)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			strcpy( Buffer , "default" );
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
				sprintf( Buffer2 , "|BM%d" , i + 1 );
				strcat( Buffer , Buffer2 );
			}
			for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) { // 2003.11.28
				sprintf( Buffer2 , "|BM%d(C)" , i + 1 );
				strcat( Buffer , Buffer2 );
			}
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Default Switch Side" , "Select" , Buffer , &SWITCHSIDE_DATA1 );
			return TRUE;
		case IDC_READY_USE_B2 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Default Switch Access Mode" , "Select" , "Ordering|Overlapped|Ordering(F)|Overlapped(F)|Ordering(L)|Overlapped(L)" , &SWITCHSIDE_DATA2 );
			return TRUE;
		case IDOK :	SWITCHSIDE_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
		case IDCANCEL :
		case IDCLOSE :	EndDialog( hdlg , 0 );	return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_HANDLINGSIDE_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
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
		HANDLINGSIDE_RESULT = FALSE;
		return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		//
		_SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 4 , HANDLINGSIDE_OFFSET , hdlg , IDC_READY_USE_B1 , HANDLINGSIDE_DATA1 );
		//
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			//
			_SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( SYSTEM_GUI_ALG_STYLE_GET() , 4 , HANDLINGSIDE_OFFSET * 10 , hdlg , &HANDLINGSIDE_DATA1 );
			//
			return TRUE;
		case IDOK :	HANDLINGSIDE_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
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
BOOL APIENTRY Gui_IDD_HANDLINGSIDE_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int c , i , j;
	//
	static int  TEMP_READY_USE_S[MAX_CHAMBER];
	static int  TEMP_READY_USE2_S[MAX_CHAMBER];
	//
	static int  TEMP_READY_USE_H[MAX_CHAMBER];
	static int  TEMP_READY_USE2_H[4];
	//
	static int  TEMP_SUPP_INT; // 2006.09.05
	static int  TEMP_DIFF_SUPP; // 2007.07.05
	static int  TEMP_METHOD1_1BM; // 2018.03.21
	//
	static BOOL Control;
	//
	char *szString_S[] = { "PM" , "Switch Chamber" , "Access Type" };
	int    szSize_S[3]  = {  50  ,              150 ,           150 };
	char *szString_H[] = { "PM" , "Default Side" };
	int    szSize_H[2]  = {  50  ,            192 };
	//
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
		for ( i = 0 ; i < AL ; i++ ) { // 2010.07.22
			TEMP_READY_USE_H[ i ] = _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE(i);
		}
//		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2010.07.22
//			TEMP_READY_USE_H[ i+PM1 ] = _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE(i+PM1); // 2010.07.22
//		}
		TEMP_READY_USE2_H[ 0 ] = _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(0,0);
		TEMP_READY_USE2_H[ 1 ] = _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(0,1);
		TEMP_READY_USE2_H[ 2 ] = _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(1,0);
		TEMP_READY_USE2_H[ 3 ] = _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(1,1);
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			TEMP_READY_USE_S[ i ] = _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER(i);
			TEMP_READY_USE2_S[ i ] = _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE(i);
		}
		//
		TEMP_SUPP_INT = _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT(); // 2006.09.05
		TEMP_DIFF_SUPP = _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE(); // 2007.07.05
		TEMP_METHOD1_1BM = _i_SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE(); // 2018.03.21
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , 2 , szString_H , szSize_H );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
//		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , MAX_PM_CHAMBER_DEPTH ); // 2010.07.22
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , AL ); // 2010.07.22
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 3 , szString_S , szSize_S );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE );
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
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
										if ( TEMP_READY_USE_S[i] == 0 ) {
											sprintf( szString , "default" );
										}
										else {
											if ( TEMP_READY_USE_S[i] > MAX_BM_CHAMBER_DEPTH ) { // 2003.11.28
												sprintf( szString , "BM%d(C)" , TEMP_READY_USE_S[i] - MAX_BM_CHAMBER_DEPTH ); // 2003.11.28
											}
											else {
												sprintf( szString , "BM%d" , TEMP_READY_USE_S[i] );
											}
										}
									}
									else {
										sprintf( szString , "-" );
									}
									break;
								case 2 :
									if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
										switch( TEMP_READY_USE2_S[i] )	{
										case 0 :	sprintf( szString , "Ordering" ); break;
										case 1 :	sprintf( szString , "Overlapped" ); break;
										case 2 :	sprintf( szString , "Ordering(F)" ); break;
										case 3 :	sprintf( szString , "Overlapped(F)" ); break;
										case 4 :	sprintf( szString , "Ordering(L)" ); break;
										case 5 :	sprintf( szString , "Overlapped(L)" ); break;
										default :	sprintf( szString , "unknown" ); break;
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
								wsprintf( szString, "CM%d", lpdi->item.iItem + 1 );
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					break;

				case NM_DBLCLK :
					if ( Control ) return TRUE;
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
						SWITCHSIDE_RESULT = FALSE;
						SWITCHSIDE_DATA1  = TEMP_READY_USE_S[i];
						SWITCHSIDE_DATA2  = TEMP_READY_USE2_S[i];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_HANDLINGSIDE_SUB_PAD2 ) , hSWnd , Gui_IDD_SWITCHSIDE_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( SWITCHSIDE_RESULT ) {
							TEMP_READY_USE_S[i] = SWITCHSIDE_DATA1;
							TEMP_READY_USE2_S[i] = SWITCHSIDE_DATA2;
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					break;
				case LVN_ITEMCHANGED :
					return 0;
				case LVN_KEYDOWN :
					return 0;
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
								//
								_SCH_COMMON_GUI_INTERFACE_DATA_GET( SYSTEM_GUI_ALG_STYLE_GET() , 4 , i , TEMP_READY_USE_H[i] , szString );
								//
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
						}
						else {
							if ( lpdi->item.mask & LVIF_TEXT ) {
								/*
								// 2008.02.05
								switch( SYSTEM_GUIx_ALG_STYLE_GET() ) {
								case RUN_ALGORITHM_INLIGN_EXPRESS_V1 :
								case RUN_ALGORITHM_INLIGN_PM2_V1 :
									wsprintf( szString, "BM%d", lpdi->item.iItem + 1 ); // 2004.08.24
									break;
								default :
									wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
									break;
								}
								*/
//								wsprintf( szString, "B/PM%d", lpdi->item.iItem + 1 ); // 2010.07.22
								_SCH_COMMON_GUI_INTERFACE_ITEM_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 4 , lpdi->item.iItem , hdlg , -1 , szString ); // 2010.07.22
//								lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
							}
							if ( lpdi->item.mask & LVIF_IMAGE ) lpdi->item.iImage = 0;
						}
						//
						strcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					break;
				case NM_DBLCLK :
					if ( Control ) return TRUE;
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 ) , -1 , LVNI_SELECTED );
					if ( i < 0 ) return 0;
					//
					if ( i <= MAX_CASSETTE_SIDE ) {
						HANDLINGSIDE_RESULT = FALSE;
						HANDLINGSIDE_DATA1  = TEMP_READY_USE_H[i];
						HANDLINGSIDE_OFFSET = i; // 2010.07.22
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_HANDLINGSIDE_SUB_PAD ) , hSWnd , Gui_IDD_HANDLINGSIDE_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( HANDLINGSIDE_RESULT ) {
							TEMP_READY_USE_H[i] = HANDLINGSIDE_DATA1;
							InvalidateRect( hdlg , NULL , FALSE );
						}
					}
					else {
						if  ( _i_SCH_PRM_GET_MODULE_MODE(i) ) {
							HANDLINGSIDE_RESULT = FALSE;
							HANDLINGSIDE_DATA1  = TEMP_READY_USE_H[i];
							HANDLINGSIDE_OFFSET = i; // 2010.07.22
							hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX2 );
							GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_HANDLINGSIDE_SUB_PAD ) , hSWnd , Gui_IDD_HANDLINGSIDE_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
							if ( HANDLINGSIDE_RESULT ) {
								TEMP_READY_USE_H[i] = HANDLINGSIDE_DATA1;
								InvalidateRect( hdlg , NULL , FALSE );
							}
						}
					}
					break;
				case LVN_ITEMCHANGED :
					return 0;
				case LVN_KEYDOWN :
					return 0;
				}
			}
		}
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( i ) != TEMP_READY_USE_S[ i ] ) break;
			if ( _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( i ) != TEMP_READY_USE2_S[ i ] ) break;
		}
		if ( i >= MAX_CASSETTE_SIDE ) {
//			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2010.07.22
//				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i+PM1 ) != TEMP_READY_USE_H[ i+PM1 ] ) break; // 2010.07.22
//			} // 2010.07.22
//			if ( i >= MAX_PM_CHAMBER_DEPTH ) { // 2010.07.22
			for ( i = 0 ; i < AL ; i++ ) {
				if ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) != TEMP_READY_USE_H[ i ] ) break;
			}
			if ( i >= AL ) {
				if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(0,0) == TEMP_READY_USE2_H[ 0 ] ) {
					if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(0,1) == TEMP_READY_USE2_H[ 1 ] ) {
						if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(1,0) == TEMP_READY_USE2_H[ 2 ] ) {
							if ( _i_SCH_PRM_GET_INTERLOCK_FM_ROBOT_FINGER(1,1) == TEMP_READY_USE2_H[ 3 ] ) {
								if ( _i_SCH_PRM_GET_BATCH_SUPPLY_INTERRUPT() == TEMP_SUPP_INT ) { // 2006.09.05
									if ( _i_SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == TEMP_DIFF_SUPP ) { // 2007.07.05
										if ( _i_SCH_PRM_GET_METHOD1_TO_1BM_USING_MODE() == TEMP_METHOD1_1BM ) { // 2018.03.21
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
		//
		if ( TEMP_READY_USE2_H[ 0 ] == 0 ) KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "-" );
		else                               KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B1 , TEMP_READY_USE2_H[ 0 ] );
		if ( TEMP_READY_USE2_H[ 1 ] == 0 ) KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "-" );
		else                               KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B2 , TEMP_READY_USE2_H[ 1 ] );
		if ( TEMP_READY_USE2_H[ 2 ] == 0 ) KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "-" );
		else                               KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B3 , TEMP_READY_USE2_H[ 2 ] );
		if ( TEMP_READY_USE2_H[ 3 ] == 0 ) KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "-" );
		else                               KWIN_Item_Int_Display( hdlg , IDC_READY_USE_B4 , TEMP_READY_USE2_H[ 3 ] );
		//
		//--------------------------------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , TEMP_SUPP_INT  , "-|On|(Normal.End)|On(Normal.End)" , "????" ); // 2007.07.24 // 2008.01.15
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 , TEMP_SUPP_INT  , "-|On|(Normal.End)|On(Normal.End)|N/A(M-FM)|On(M-FM)|(Normal.End)(M-FM)|On(Normal.End)(M-FM)" , "????" ); // 2007.07.24 // 2008.01.15
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B6 , TEMP_DIFF_SUPP , "-|Separate|Separate(P)|Together(P)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B7 , TEMP_METHOD1_1BM  , "-|Skip" , "????" ); // 2018.03.21
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
		case IDC_READY_USE_B2 :
		case IDC_READY_USE_B3 :
		case IDC_READY_USE_B4 :
			if ( Control ) return TRUE;
			if      ( wParam == IDC_READY_USE_B1 ) c = 0;
			else if ( wParam == IDC_READY_USE_B2 ) c = 1;
			else if ( wParam == IDC_READY_USE_B3 ) c = 2;
			else if ( wParam == IDC_READY_USE_B4 ) c = 3;
			i = TEMP_READY_USE2_H[ c ];
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select FEM Robot Intlks Slot" , "Select" , "N/A|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30" , &i ) ) {
				if ( i != TEMP_READY_USE2_H[ c ] ) {
					if ( i != 0 ) {
						for ( j = 0 ; j < 4 ; j++ ) {
							if ( ( j != c ) && ( TEMP_READY_USE2_H[ j ] > 0 ) ) {
								if ( TEMP_READY_USE2_H[ j ] == i ) {
									MessageBox( hdlg , "Can not Set because overlapped slot!", "Error", MB_ICONQUESTION );
									return TRUE;
								}
							}
						}
					}
					TEMP_READY_USE2_H[ c ] = i;
					InvalidateRect( hdlg , NULL , TRUE );
				}
			}
			return TRUE;

		case IDC_READY_USE_B5 : // 2006.09.05
//			i = TEMP_SUPP_INT;
//			if ( MODAL_DIGITAL_BOX1( hdlg , "Select Batch Supply Interrupt" , "Select" , "N/A|On" , &i ) ) { // 2007.03.27
//				if ( i != TEMP_SUPP_INT ) {
//					TEMP_SUPP_INT = i;
//					InvalidateRect( hdlg , NULL , TRUE );
//				}
//			}
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Batch Supply Interrupt and (End Style)" , "Select" , "N/A|On|N/A(Normal End)|On(Normal End)" , &TEMP_SUPP_INT ); // 2007.03.27 // 2008.01.15
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Batch Supply Interrupt and (End Style)" , "Select" , "N/A|On|N/A(Normal End)|On(Normal End)|N/A(M-FM)|On(M-FM)|N/A(Normal End)(M-FM)|On(Normal End)(M-FM)" , &TEMP_SUPP_INT ); // 2007.03.27 // 2008.01.15
			return TRUE;

		case IDC_READY_USE_B6 : // 2007.07.05
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Different Lot Supply Mode" , "Select" , "N/A(Together)|Separate|Separate(Partial)|Together(Partial)" , &TEMP_DIFF_SUPP ); // 2007.07.05
			return TRUE;

		case IDC_READY_USE_B7 : // 2018.03.21
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Select Method 1 to 1 BM Mode" , "Select" , "-(Use)|Skip" , &TEMP_METHOD1_1BM );
			return TRUE;

		case IDOK :
			if ( Control ) return TRUE;
//			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) { // 2010.07.22
//				_i_SCH_PRM_SET_MODULE_DOUBLE_WHAT_SIDE( i+PM1 , TEMP_READY_USE_H[ i+PM1 ] );
//			}
			for ( i = 0 ; i < AL ; i++ ) { // 2010.07.22
				_i_SCH_PRM_SET_MODULE_DOUBLE_WHAT_SIDE( i , TEMP_READY_USE_H[ i ] );
			}
			_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 0 , 0 , TEMP_READY_USE2_H[0] );
			_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 0 , 1 , TEMP_READY_USE2_H[1] );
			_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 1 , 0 , TEMP_READY_USE2_H[2] );
			_i_SCH_PRM_SET_INTERLOCK_FM_ROBOT_FINGER( 1 , 1 , TEMP_READY_USE2_H[3] );
			//
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER( i , TEMP_READY_USE_S[ i ] );
				_i_SCH_PRM_SET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( i , TEMP_READY_USE2_S[ i ] );
			}
			//
			_i_SCH_PRM_SET_BATCH_SUPPLY_INTERRUPT( TEMP_SUPP_INT ); // 2006.09.05
			//
			_i_SCH_PRM_SET_DIFF_LOT_NOTSUP_MODE( TEMP_DIFF_SUPP ); // 2007.07.05
			//
			_i_SCH_PRM_SET_METHOD1_TO_1BM_USING_MODE( TEMP_METHOD1_1BM ); // 2018.03.21
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//---------------------------------
			Set_RunPrm_IO_Setting( 4 );
			Set_RunPrm_IO_Setting( 6 );
			Set_RunPrm_IO_Setting( 21 ); // 2008.10.23
			Set_RunPrm_IO_Setting( 28 ); // 2015.05.27
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
