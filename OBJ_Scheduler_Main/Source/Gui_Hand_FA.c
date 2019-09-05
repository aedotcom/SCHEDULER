#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "FA_Handling.h"
#include "User_Parameter.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_FA_FUNCTION_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	char Buffer[256];
	int i , c , r;

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
		MoveCenterWindow( hdlg );
		switch ( FA_SERVER_MODE_GET() ) {
		case 1 :	sprintf( Buffer , "(ENABLE) %s" , FA_SERVER_FUNCTION_NAME_GET() );	break;
		case 0 :	sprintf( Buffer , "(DISABLE) %s" , FA_SERVER_FUNCTION_NAME_GET() );	break;
		default :	sprintf( Buffer , "N/A" );	break;
		}
		KWIN_Item_String_Display( hdlg , IDC_FA_SERVER_STATUS , Buffer );
		for ( i = 0 ; i < 4 ; i++ ) {
			if      ( i == 0 ) c = IDC_FA_AGV_STATUS;
			else if ( i == 1 ) c = IDC_FA_AGV_STATUS2;
			else if ( i == 2 ) c = IDC_FA_AGV_STATUS3;
			else if ( i == 3 ) c = IDC_FA_AGV_STATUS4;
			switch ( FA_AGV_MODE_GET(i) ) {
			case 1 :	sprintf( Buffer , "(ENABLE) %s" , FA_AGV_FUNCTION_NAME_GET(i) );	break;
			case 0 :	sprintf( Buffer , "(DISABLE) %s" , FA_AGV_FUNCTION_NAME_GET(i) );	break;
			default :	sprintf( Buffer , "N/A" );	break;
			}
			KWIN_Item_String_Display( hdlg , c , Buffer );
		}
		//
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FA_AGV_STATUS5 ,
							FA_FUNCTION_INTERFACE_GET() , "Default|D/Manaul,E/Function(2)|D/Manaul,E/Function(3)|D/Function,E/Function|D/Function,E/Function(2)|D/Function,E/Function(3)" , "????" ); // 2007.07.24
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_FA_SERVER_STATUS :
			switch ( FA_SERVER_MODE_GET() ) {
			case 1 :
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Disable|Change Server Function" , &r ) ) {
					if ( r == 0 ) {
						FA_SERVER_FUNCTION_SET( 4 , "" );
					}
					else {
						strcpy( Buffer , FA_SERVER_FUNCTION_NAME_GET() );
						if ( MODAL_STRING_BOX1( hdlg , "Select Server Function Name" , "Select Server Function Name(String IO)" , Buffer ) ) {
							FA_SERVER_FUNCTION_SET( 1 , Buffer );
						}
						else {
							r = -1;
						}
					}
				}
				else {
					r = -1;
				}
				break;
			case 0 :
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Enable|Change Server Function" , &r ) ) {
					if ( r == 0 ) {
						FA_SERVER_FUNCTION_SET( 3 , "" );
					}
					else {
						strcpy( Buffer , FA_SERVER_FUNCTION_NAME_GET() );
						if ( MODAL_STRING_BOX1( hdlg , "Select Server Function Name" , "Select Server Function Name(String IO)" , Buffer ) ) {
							FA_SERVER_FUNCTION_SET( 2 , Buffer );
						}
						else {
							r = -1;
						}
					}
				}
				else {
					r = -1;
				}
				break;
			default :
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Regist Server Function" , &r ) ) {
					if ( r == 0 ) {
						strcpy( Buffer , "" );
						if ( MODAL_STRING_BOX1( hdlg , "Select Server Function Name" , "Select Server Function Name(String IO)" , Buffer ) ) {
							FA_SERVER_FUNCTION_SET( 2 , Buffer );
						}
						else {
							r = -1;
						}
					}
				}
				else {
					r = -1;
				}
				break;
			}
			if ( r >= 0 ) {
				switch ( FA_SERVER_MODE_GET() ) {
				case 1 :	sprintf( Buffer , "(ENABLE) %s" , FA_SERVER_FUNCTION_NAME_GET() );	break;
				case 0 :	sprintf( Buffer , "(DISABLE) %s" , FA_SERVER_FUNCTION_NAME_GET() );	break;
				default :	sprintf( Buffer , "N/A" );	break;
				}
				KWIN_Item_String_Display( hdlg , IDC_FA_SERVER_STATUS , Buffer );
				GUI_SAVE_PARAMETER_DATA( 1 );
			}
			break;

		case IDC_FA_AGV_STATUS :
		case IDC_FA_AGV_STATUS2 :
		case IDC_FA_AGV_STATUS3 :
		case IDC_FA_AGV_STATUS4 :
			switch( wParam ) {
			case IDC_FA_AGV_STATUS  : i = 0; break;
			case IDC_FA_AGV_STATUS2 : i = 1; break;
			case IDC_FA_AGV_STATUS3 : i = 2; break;
			case IDC_FA_AGV_STATUS4 : i = 3; break;
			}
			switch ( FA_AGV_MODE_GET(i) ) {
			case 1 :
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Disable|Change Exchg Function" , &r ) ) {
					if ( r == 0 ) {
						FA_AGV_FUNCTION_SET( i , 4 , "" );
						//===================================================
						FA_AGV_AUTOHANDLER_SEND_MESSAGE( i );
						//===================================================
					}
					else {
						strcpy( Buffer , FA_AGV_FUNCTION_NAME_GET( i ) );
						if ( MODAL_STRING_BOX1( hdlg , "Select Exchg Function Name" , "Select Exchg Function Name" , Buffer ) ) {
							FA_AGV_FUNCTION_SET( i , 1 , Buffer );
							//===================================================
							FA_AGV_AUTOHANDLER_SEND_MESSAGE( i );
							//===================================================
						}
						else {
							r = -1;
						}
					}
				}
				else {
					r = -1;
				}
				break;
			case 0 :
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Enable|Change Exchg Function" , &r ) ) {
					if ( r == 0 ) {
						FA_AGV_FUNCTION_SET( i , 3 , "" );
						//===================================================
						FA_AGV_AUTOHANDLER_SEND_MESSAGE( i );
						//===================================================
					}
					else {
						strcpy( Buffer , FA_AGV_FUNCTION_NAME_GET( i ) );
						if ( MODAL_STRING_BOX1( hdlg , "Select Exchg Function Name" , "Select Exchg Function Name" , Buffer ) ) {
							FA_AGV_FUNCTION_SET( i , 2 , Buffer );
							//===================================================
							FA_AGV_AUTOHANDLER_SEND_MESSAGE( i );
							//===================================================
						}
						else {
							r = -1;
						}
					}
				}
				else {
					r = -1;
				}
				break;
			default :
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Regist Exchg Function" , &r ) ) {
					if ( r == 0 ) {
						strcpy( Buffer , "" );
						if ( MODAL_STRING_BOX1( hdlg , "Select Exchg Function Name" , "Select Exchg Function Name" , Buffer ) ) {
							FA_AGV_FUNCTION_SET( i , 2 , Buffer );
							//===================================================
							FA_AGV_AUTOHANDLER_SEND_MESSAGE( i );
							//===================================================
						}
						else {
							r = -1;
						}
					}
				}
				else {
					r = -1;
				}
				break;
			}
			if ( r >= 0 ) {
				switch ( FA_AGV_MODE_GET(i) ) {
				case 1 :	sprintf( Buffer , "(ENABLE) %s" , FA_AGV_FUNCTION_NAME_GET(i) );	break;
				case 0 :	sprintf( Buffer , "(DISABLE) %s" , FA_AGV_FUNCTION_NAME_GET(i) );	break;
				default :	sprintf( Buffer , "N/A" );											break;
				}
				KWIN_Item_String_Display( hdlg , wParam , Buffer );
				GUI_SAVE_PARAMETER_DATA( 1 );
				FA_AGV_STATUS_IO_SET( i );
			}
			break;

		case IDC_FA_AGV_STATUS5 :
			r = FA_FUNCTION_INTERFACE_GET();
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Select" , "Default|D/Manaul,E/Function(2)|D/Manaul,E/Function(3)|D/Function,E/Function|D/Function,E/Function(2)|D/Function,E/Function(3)" , &r ) ) {
				if ( r != FA_FUNCTION_INTERFACE_GET() ) {
					FA_FUNCTION_INTERFACE_SET( r );
					//-------------------------------------------------
					KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FA_AGV_STATUS5 ,
							r , "Default|D/Manaul,E/Function(2)|D/Manaul,E/Function(3)|D/Function,E/Function|D/Function,E/Function(2)|D/Function,E/Function(3)" , "????" ); // 2007.07.24
					//-------------------------------------------------
					GUI_SAVE_PARAMETER_DATA( 1 );
				}
			}
			break;

		case IDOK :
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL FAPAD_RESULT = FALSE;
int  FAPAD_DATA1;
int  FAPAD_DATA2;
int  FAPAD_DATA3;
int  FAPAD_DATA4;
int  FAPAD_DATA5;
int  FAPAD_DATA6;
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_FA_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
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
		FAPAD_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B1 ,
				FAPAD_DATA1 , "N/A|Use" , "????" ); // 2007.07.24
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B2 ,
//				FAPAD_DATA2 , "Auto|Manual|Manual2|Auto(H)" , "Auto" ); // 2007.07.24 // 2013.06.26
				FAPAD_DATA2 , "Auto|Manual|Manual2|Auto(H)|Auto.CX|Manual.CX|Manual2.CX|Auto(H).CX" , "Auto" ); // 2007.07.24 // 2013.06.26
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B3 ,
//				FAPAD_DATA3 , "Auto|Manual|Manual2|Auto(H)" , "Auto" ); // 2007.07.24 // 2013.06.26
				FAPAD_DATA3 , "Auto|Manual|Manual2|Auto(H)|Auto.CX|Manual.CX|Manual2.CX|Auto(H).CX" , "Auto" ); // 2007.07.24 // 2013.06.26
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B4 ,
//				FAPAD_DATA4 , "N/A|Do" , "????" ); // 2007.07.24 // 2013.07.05
				FAPAD_DATA4 , "N/A|Do|N/A(R.Do)|Do(R.N/A)" , "????" ); // 2007.07.24 // 2013.07.05
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B5 ,
				FAPAD_DATA5 , "Default|MsgBlank=>IO|Always=>IO|Current IO" , "Default" ); // 2007.07.24
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_B6 ,
				FAPAD_DATA6 , "With Message|With IO|WithIO AfterMap|Current IO" , "With Message" ); // 2007.07.24
		//-------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Extend hand Off Use" , "Select" , "N/A|Use" , &FAPAD_DATA1 );
			return TRUE;
		case IDC_READY_USE_B2 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Complete Message at Load" , "Select" , "Auto|Manual|Manual2|Auto(Hide)" , &FAPAD_DATA2 ); // 2013.06.26
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Complete Message at Load" , "Select" , "Auto|Manual|Manual2|Auto(Hide)|Auto.CX|Manual.CX|Manual2.CX|Auto(Hide).CX" , &FAPAD_DATA2 ); // 2013.06.26
			return TRUE;
		case IDC_READY_USE_B3 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Complete Message at Unload" , "Select" , "Auto|Manual|Manual2|Auto(Hide)" , &FAPAD_DATA3 ); // 2013.06.26
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Complete Message at Unload" , "Select" , "Auto|Manual|Manual2|Auto(Hide)|Auto.CX|Manual.CX|Manual2.CX|Auto(Hide).CX" , &FAPAD_DATA3 ); // 2013.06.26
			return TRUE;
		case IDC_READY_USE_B4 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Auto Mapping after load" , "Select" , "N/A|Do" , &FAPAD_DATA4 ); // 2013.07.05
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Auto Mapping after load" , "Select" , "N/A|Do|N/A(R.Do)|Do(R.N/A)" , &FAPAD_DATA4 ); // 2013.07.05
			return TRUE;
		case IDC_READY_USE_B5 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Job Name Read Point" , "Select" , "Default|MsgBlank=>IO|Always=>IO|Current IO" , &FAPAD_DATA5 );
			return TRUE;
		case IDC_READY_USE_B6 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select MID Name Read Point" , "Select" , "With Message|With IO|With IO After Map|Current IO" , &FAPAD_DATA6 );
			return TRUE;
		case IDOK :	FAPAD_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
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
BOOL APIENTRY Gui_IDD_FA_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	static int  TEMP_USE[22];
	static int  TEMP_USE2[MAX_CASSETTE_SIDE][6];
	static BOOL Control;
	int i;
	char *szString[] = { "Cass" , "EHO" , "Load" , "Unload" , "AfterMap" , "Job" , "Mid" };
	int    szSize[7]  = {    50  ,    74 ,     58 ,       58 ,         60 ,    99 ,   115 };
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
		if ( Control ) KWIN_Item_Hide( hdlg , IDYES );
		TEMP_USE[0] = _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP();
		TEMP_USE[1] = _i_SCH_PRM_GET_FA_MAPPING_SKIP();
		TEMP_USE[2] = _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN();
		TEMP_USE[3] = _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW();
		TEMP_USE[4] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST();
		TEMP_USE[5] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE();
		TEMP_USE[6] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REJECT();
		TEMP_USE[7] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST();
		TEMP_USE[8] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE();
		TEMP_USE[9] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT();
		TEMP_USE[10] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST();
		TEMP_USE[11] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE();
		TEMP_USE[12] = _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT();
		TEMP_USE[13] = _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP();
		TEMP_USE[14] = _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP();
		TEMP_USE[15] = _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP();
		TEMP_USE[16] = _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY();
		TEMP_USE[17] = _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE();
		TEMP_USE[18] = _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID();
		TEMP_USE[19] = _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY(); // 2004.06.16
		TEMP_USE[20] = _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE(); // 2007.08.29

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			TEMP_USE2[i][0] = _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( i + CM1 );
			TEMP_USE2[i][1] = _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE( i );
			TEMP_USE2[i][2] = _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE( i );
			TEMP_USE2[i][3] = _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i );
			TEMP_USE2[i][4] = _i_SCH_PRM_GET_FA_JID_READ_POINT( i );
			TEMP_USE2[i][5] = _i_SCH_PRM_GET_FA_MID_READ_POINT( i );
		}
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 7 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SIDE );
		//
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
					if ( lpdi->item.iSubItem ) {
						if ( lpdi->item.mask & LVIF_TEXT ) {
							i = lpdi->item.iItem;
							switch( lpdi->item.iSubItem ) {
							case 1 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
									if ( TEMP_USE2[i][0] )	sprintf( szString , "Use" );
									else					sprintf( szString , "N/A" );
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 2 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
									switch( TEMP_USE2[i][1] )	{
									case 0 : sprintf( szString , "Auto" ); break;
									case 1 : sprintf( szString , "Manual" ); break;
									case 2 : sprintf( szString , "Manual2" ); break;
									case 3 : sprintf( szString , "Auto(H)" ); break; // 2004.12.15
									case 4 : sprintf( szString , "Auto.CX" ); break; // 2013.06.26
									case 5 : sprintf( szString , "Manual.CX" ); break; // 2013.06.26
									case 6 : sprintf( szString , "Manual2.CX" ); break; // 2013.06.26
									case 7 : sprintf( szString , "Auto(H).CX" ); break; // 2013.06.26
									default: sprintf( szString , "Auto" ); break;
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 3 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
									switch( TEMP_USE2[i][2] )	{
									case 0 : sprintf( szString , "Auto" ); break;
									case 1 : sprintf( szString , "Manual" ); break;
									case 2 : sprintf( szString , "Manual2" ); break;
									case 3 : sprintf( szString , "Auto(H)" ); break; // 2004.12.15
									case 4 : sprintf( szString , "Auto.CX" ); break; // 2013.06.26
									case 5 : sprintf( szString , "Manual.CX" ); break; // 2013.06.26
									case 6 : sprintf( szString , "Manual2.CX" ); break; // 2013.06.26
									case 7 : sprintf( szString , "Auto(H).CX" ); break; // 2013.06.26
									default: sprintf( szString , "Auto" ); break;
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 4 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
									if      ( TEMP_USE2[i][3] == 3 )	sprintf( szString , "Do(R.N/A)" ); // 2013.07.05
									else if ( TEMP_USE2[i][3] == 2 )	sprintf( szString , "N/A(R.Do)" ); // 2013.07.05
									else if ( TEMP_USE2[i][3] == 1 )	sprintf( szString , "Do" );
									else								sprintf( szString , "N/A" );
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 5 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
									switch( TEMP_USE2[i][4] )	{
									case 0 : sprintf( szString , "Default" ); break;
									case 1 : sprintf( szString , "MsgBlank=>IO" ); break;
									case 2 : sprintf( szString , "Always=>IO" ); break;
									case 3 : sprintf( szString , "Current IO" ); break;
									default: sprintf( szString , "Default" ); break;
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 6 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
									switch( TEMP_USE2[i][5] )	{
									case 0 : sprintf( szString , "With Message" ); break;
									case 1 : sprintf( szString , "With IO" ); break;
									case 2 : sprintf( szString , "WithIO AfterMap" ); break;
									case 3 : sprintf( szString , "With Current IO" ); break;
									default: sprintf( szString , "With Message" ); break;
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							}
//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
						}
					}
					else {
						if ( lpdi->item.mask & LVIF_TEXT ) {
							wsprintf( szString, "CM%d", lpdi->item.iItem + 1 );
//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
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
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+CM1) ) {
						FAPAD_RESULT = FALSE;
						FAPAD_DATA1  = TEMP_USE2[i][0];
						FAPAD_DATA2  = TEMP_USE2[i][1];
						FAPAD_DATA3  = TEMP_USE2[i][2];
						FAPAD_DATA4  = TEMP_USE2[i][3];
						FAPAD_DATA5  = TEMP_USE2[i][4];
						FAPAD_DATA6  = TEMP_USE2[i][5];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_FA_SUB_PAD ) , hSWnd , Gui_IDD_FA_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( FAPAD_RESULT ) {
							TEMP_USE2[i][0] = FAPAD_DATA1;
							TEMP_USE2[i][1] = FAPAD_DATA2;
							TEMP_USE2[i][2] = FAPAD_DATA3;
							TEMP_USE2[i][3] = FAPAD_DATA4;
							TEMP_USE2[i][4] = FAPAD_DATA5;
							TEMP_USE2[i][5] = FAPAD_DATA6;
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

		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		KWIN_Item_Disable( hdlg , IDOK );
		if ( TEMP_USE[0] != _i_SCH_PRM_GET_FA_LOADUNLOAD_SKIP()       ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[1] != _i_SCH_PRM_GET_FA_MAPPING_SKIP()          ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[2] != _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[3] != _i_SCH_PRM_GET_FA_STARTEND_STATUS_SHOW() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[4] != _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[5] != _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[6] != _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_LOAD_REJECT() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[7] != _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[8] != _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[9] != _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[10]!= _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[11]!= _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[12]!= _i_SCH_PRM_GET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[13]!= _i_SCH_PRM_GET_FA_WINDOW_NORMAL_CHECK_SKIP() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[14]!= _i_SCH_PRM_GET_FA_WINDOW_ABORT_CHECK_SKIP() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[15]!= _i_SCH_PRM_GET_FA_PROCESS_STEPCHANGE_CHECK_SKIP() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[16]!= _i_SCH_PRM_GET_FA_SEND_MESSAGE_DISPLAY() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[17]!= _i_SCH_PRM_GET_FA_EXPAND_MESSAGE_USE() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[18]!= _i_SCH_PRM_GET_FA_SUBSTRATE_WAFER_ID() ) KWIN_Item_Enable( hdlg , IDOK );
		if ( TEMP_USE[19]!= _i_SCH_PRM_GET_FA_REJECTMESSAGE_DISPLAY() ) KWIN_Item_Enable( hdlg , IDOK ); // 2004.06.16
		if ( TEMP_USE[20]!= _i_SCH_PRM_GET_FA_NORMALUNLOAD_NOTUSE()       ) KWIN_Item_Enable( hdlg , IDOK ); // 2007.08.29

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( TEMP_USE2[i][0] != _i_SCH_PRM_GET_FA_EXTEND_HANDOFF( i + CM1 ) ) KWIN_Item_Enable( hdlg , IDOK );
			if ( TEMP_USE2[i][1] != _i_SCH_PRM_GET_FA_LOAD_COMPLETE_MESSAGE( i ) ) KWIN_Item_Enable( hdlg , IDOK );
			if ( TEMP_USE2[i][2] != _i_SCH_PRM_GET_FA_UNLOAD_COMPLETE_MESSAGE( i ) ) KWIN_Item_Enable( hdlg , IDOK );
			if ( TEMP_USE2[i][3] != _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( i ) ) KWIN_Item_Enable( hdlg , IDOK );
			if ( TEMP_USE2[i][4] != _i_SCH_PRM_GET_FA_JID_READ_POINT( i ) ) KWIN_Item_Enable( hdlg , IDOK );
			if ( TEMP_USE2[i][5] != _i_SCH_PRM_GET_FA_MID_READ_POINT( i ) ) KWIN_Item_Enable( hdlg , IDOK );
		}

		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P1 ,
//				TEMP_USE[0] , "Use|Skip|Use(Also Manual Use)|Use(Also Manual Use-Loop Type)|Use(Not Loop Type)|Use(Not Loop Type/Also Manual Use)" , "Use(Not Loop Type/Manual Use-Loop)" ); // 2007.07.24 // 2011.04.20
				TEMP_USE[0] , "Use:0|Skip:1|Use(Also Manual Use):2|Use(Also Manual Use-Loop Type):3|Use(Not Loop Type):4|Use(Not Loop Type/Also Manual Use):5|Use(Not Loop Type/Also Manual Use-Loop Type):6" , "????" ); // 2007.07.24 // 2011.04.20
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P2 ,
				TEMP_USE[1] , "Use|Skip" , "Use" ); // 2007.07.24
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_READY_USE_P5 ,
//				TEMP_USE[2] , "N/A|Use" , "N/A" ); // 2007.07.24 // 2017.01.11
				TEMP_USE[2] , "N/A|Use|N/A+PMSingle|Use+PMSingle" , "N/A" ); // 2007.07.24 // 2017.01.11
		//-------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FA_END_STATUS_SHOW ,
				TEMP_USE[3] , "N/A|Pre Part Show|Post Part Show|Always Show|Pre Part Show(A)|Post Part Show(A)" , "Always Show(A)" ); // 2007.07.24
		//-------------------------------------------------
		if ( TEMP_USE[0] == 1 ) {
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_1 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_2 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_3 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_4 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_5 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_6 );
		}
		else {
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_1 , TEMP_USE[4] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_2 , TEMP_USE[5] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_3 , TEMP_USE[6] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_4 , TEMP_USE[7] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_5 , TEMP_USE[8] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_6 , TEMP_USE[9] , "|N/A" , "N/A" ); // 2007.07.24
		}
		if ( TEMP_USE[1] == 1 ) {
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_7 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_8 );
			KWIN_Item_Hide( hdlg , IDC_FASYS_MESSAGE_9 );
		}
		else {
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_7 , TEMP_USE[10] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_8 , TEMP_USE[11] , "|N/A" , "N/A" ); // 2007.07.24
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_MESSAGE_9 , TEMP_USE[12] , "|N/A" , "N/A" ); // 2007.07.24
		}
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_NCHK_WINDOW , TEMP_USE[13] , "Use|Skip" , "Skip" ); // 2007.07.24
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_ACHK_WINDOW , TEMP_USE[14] , "Use|Skip" , "Skip" ); // 2007.07.24 // 2018.12.20
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_ACHK_WINDOW , TEMP_USE[14] , "Use|Skip|Use(EtoA)|Skip(EtoA)" , "Skip" ); // 2007.07.24 // 2018.12.20
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_STEPCHANGE  , TEMP_USE[15] , "N/A|Use|N/A[@Module]|Use[@Module]|N/A[@Module]|Use[@Module]" , "????" ); // 2007.07.24 // 2007.11.28
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_STEPCHANGE  , TEMP_USE[15] , "N/A|Use|N/A[@Module]|Use[@Module]|N/A[@ModuleF]|Use[@ModuleF]" , "????" ); // 2007.07.24 // 2007.11.28
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_SENDMESSAGE , TEMP_USE[16] , "Con|Alw" , "????" ); // 2007.07.24 // 2013.05.04
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_SENDMESSAGE , TEMP_USE[16] , "Con|Alw|Con(Prn)|Alw(Prn)" , "????" ); // 2007.07.24 // 2013.05.04
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_EXPANDMESSAGE  , TEMP_USE[17] , "N/A|Use|Use2" , "????" ); // 2007.07.24
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_SUBSTRATEWAFER , TEMP_USE[18] , "Src|Run" , "????" ); // 2007.07.24 // 2014.02.04
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_SUBSTRATEWAFER , TEMP_USE[18] , "Src|Run|Src(+PathDo)|Run(+PathDo)" , "????" ); // 2007.07.24 // 2014.02.04
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_REJECTDISPLAY  , TEMP_USE[19] , "N/A|Use" , "????" ); // 2007.07.24
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_FASYS_NORMALUNLOAD   , TEMP_USE[20] , "Use|AutoSkip|ManualSkip|AllSkip" , "????" ); // 2007.08.28
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_P1 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 6 , &(TEMP_USE[0]) );
			break;
		case IDC_READY_USE_P2 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[1]) );
			break;
		case IDC_READY_USE_P5 :
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[2]) ); // 2017.01.11
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &(TEMP_USE[2]) ); // 2017.01.11
			break;

		case IDC_FA_END_STATUS_SHOW :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 6 , &(TEMP_USE[3]) );
			break;

		case IDC_FASYS_MESSAGE_1 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[4]) );
			break;
		case IDC_FASYS_MESSAGE_2 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[5]) );
			break;
		case IDC_FASYS_MESSAGE_3 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[6]) );
			break;
		case IDC_FASYS_MESSAGE_4 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[7]) );
			break;
		case IDC_FASYS_MESSAGE_5 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[8]) );
			break;
		case IDC_FASYS_MESSAGE_6 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[9]) );
			break;
		case IDC_FASYS_MESSAGE_7 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[10]) );
			break;
		case IDC_FASYS_MESSAGE_8 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[11]) );
			break;
		case IDC_FASYS_MESSAGE_9 :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[12]) );
			break;

		case IDC_FASYS_NCHK_WINDOW :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[13]) );
			break;

		case IDC_FASYS_ACHK_WINDOW :
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[14]) ); // 2018.12.20
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &(TEMP_USE[14]) ); // 2018.12.20
			break;

		case IDC_FASYS_STEPCHANGE :
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &(TEMP_USE[15]) ); // 2007.12.03
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 5 , &(TEMP_USE[15]) ); // 2007.12.03
			break;

		case IDC_FASYS_SENDMESSAGE :
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[16]) ); // 2013.05.04
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &(TEMP_USE[16]) ); // 2013.05.04
			break;

		case IDC_FASYS_EXPANDMESSAGE :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 2 , &(TEMP_USE[17]) );
			break;

		case IDC_FASYS_SUBSTRATEWAFER :
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[18]) ); // 2014.02.04
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &(TEMP_USE[18]) ); // 2014.02.04
			break;

		case IDC_FASYS_REJECTDISPLAY :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &(TEMP_USE[19]) );
			break;

		case IDC_FASYS_NORMALUNLOAD : // 2007.08.29
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &(TEMP_USE[20]) );
			break;

		case IDYES :
			if ( Control ) return TRUE;
			GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_FA_FUNCTION_PAD ) , hdlg , Gui_IDD_FA_FUNCTION_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
			break;

		case IDOK :
			if ( Control ) return TRUE;
			_i_SCH_PRM_SET_FA_LOADUNLOAD_SKIP( TEMP_USE[0] );
			_i_SCH_PRM_SET_FA_MAPPING_SKIP( TEMP_USE[1] );
			_i_SCH_PRM_SET_FA_SINGLE_CASS_MULTI_RUN( TEMP_USE[2] );
			_i_SCH_PRM_SET_FA_STARTEND_STATUS_SHOW( TEMP_USE[3] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REQUEST( TEMP_USE[4] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_COMPLETE( TEMP_USE[5] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_LOAD_REJECT( TEMP_USE[6] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REQUEST( TEMP_USE[7] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_COMPLETE( TEMP_USE[8] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_UNLOAD_REJECT( TEMP_USE[9] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REQUEST( TEMP_USE[10] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_COMPLETE( TEMP_USE[11] );
			_i_SCH_PRM_SET_FA_SYSTEM_MSGSKIP_MAPPING_REJECT( TEMP_USE[12] );
			_i_SCH_PRM_SET_FA_WINDOW_NORMAL_CHECK_SKIP( TEMP_USE[13] );
			_i_SCH_PRM_SET_FA_WINDOW_ABORT_CHECK_SKIP( TEMP_USE[14] );
			_i_SCH_PRM_SET_FA_PROCESS_STEPCHANGE_CHECK_SKIP( TEMP_USE[15] );
			_i_SCH_PRM_SET_FA_SEND_MESSAGE_DISPLAY( TEMP_USE[16] );
			_i_SCH_PRM_SET_FA_EXPAND_MESSAGE_USE( TEMP_USE[17] );
			_i_SCH_PRM_SET_FA_SUBSTRATE_WAFER_ID( TEMP_USE[18] );
			_i_SCH_PRM_SET_FA_REJECTMESSAGE_DISPLAY( TEMP_USE[19] );
			_i_SCH_PRM_SET_FA_NORMALUNLOAD_NOTUSE( TEMP_USE[20] ); // 2007.08.29

			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				_i_SCH_PRM_SET_FA_EXTEND_HANDOFF( i + CM1 , TEMP_USE2[i][0] );
				_i_SCH_PRM_SET_FA_LOAD_COMPLETE_MESSAGE( i , TEMP_USE2[i][1] );
				_i_SCH_PRM_SET_FA_UNLOAD_COMPLETE_MESSAGE( i , TEMP_USE2[i][2] );
				_i_SCH_PRM_SET_FA_MAPPING_AFTERLOAD( i , TEMP_USE2[i][3] );
				_i_SCH_PRM_SET_FA_JID_READ_POINT( i , TEMP_USE2[i][4] );
				_i_SCH_PRM_SET_FA_MID_READ_POINT( i , TEMP_USE2[i][5] );
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
