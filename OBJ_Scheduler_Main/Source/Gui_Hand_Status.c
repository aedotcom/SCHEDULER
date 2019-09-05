#include "default.h"

#include "EQ_Enum.h"

#include "Equip_Handling.h"
#include "GUI_Handling.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "sch_prm_log.h"
#include "sch_processmonitor.h"
#include "DLL_Interface.h"
#include "iolog.h"
#include "Multijob.h"
#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "system_tag.h"
#include "resource.h"
#include "version.h"

#include "INF_sch_CommonUser.h"

extern BOOL EVENT_MESSAGE_LOG; // 2015.04.09
extern int EVENT_FLAG_LOG; // 2015.05.26

extern int PROCESSLOG_MONITOR_IO[ MAX_CHAMBER ];
extern int IO_DEBUG_MONITOR_DATAVIEW; // 2008.09.08
extern char *DRV_DLL_SCH_FILENAME;

//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
BOOL Gui_Open_Explorer( int side ) {
	int i , len;
	char Buffer[256];
	//
	if ( strlen( _i_SCH_PRM_GET_DFIX_LOG_PATH() ) <= 0 ) return FALSE;
	if ( strlen( _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) ) <= 0 ) return FALSE;
	//
	_snprintf( Buffer , 255 , "explorer %c%s\\lotlog\\%s%c" , '"' , _i_SCH_PRM_GET_DFIX_LOG_PATH() , _i_SCH_SYSTEM_GET_LOTLOGDIRECTORY( side ) , '"' );
	//
	len = strlen( Buffer );
	//
	for ( i = 0 ; i < len ; i++ ) {
		if( Buffer[i] == '/' ) Buffer[i] = '\\';
	}
	//
	system( Buffer ); 
	//
	return TRUE;
}

BOOL APIENTRY Gui_IDD_STATUS_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int id , SLS;	
	int i , c , Res , ret;
	char Buffer[256];
	char Buffer2[256];
	char *szString[] = { "Ch" , "Status" , "Sd,Wid" , "Main" , "Post" , "Pre" };
	int    szSize[6] = {  40  ,      135 ,       65 ,     50 ,     50 ,    50 };
	switch ( msg ) {
	case WM_DESTROY:
		//
		KGUI_STANDARD_Set_User_POPUP_Close( 0 ); // 2015.04.01
		//
		IO_DEBUG_DISABLE( FALSE ); // 2015.06.11
		//
		KillTimer( hdlg , id );
		break;

	case WM_INITDIALOG:
		//
		KGUI_STANDARD_Set_User_POPUP_Open( 0 , hdlg , 1 ); // 2015.04.01
		//
		MoveCenterWindow( hdlg );
		//===============================================================
//		strcpy( Buffer2 , "" ); // 2017.01.05
		_SCH_COMMON_REVISION_HISTORY( 0 , Buffer2 , 255 );
		if ( strlen( Buffer2 ) <= 0 ) {
			sprintf( Buffer , "Scheduler Status [%s](%s %s)" , SCH_LASTUPDATE_DATE , __DATE__ , __TIME__ );
		}
		else {
			sprintf( Buffer , "Scheduler Status [%s][%s](%s %s)" , SCH_LASTUPDATE_DATE , Buffer2 , __DATE__ , __TIME__ );
		}
		//
		SetWindowText( hdlg , Buffer );
		//===============================================================
		SLS = _i_SCH_PRM_GET_SYSTEM_LOG_STYLE(); // 2006.02.07
		//===============================================================
		sprintf( Buffer , "%d.%d (%d:%d)" ,
			SYSTEM_RUN_ALG_STYLE_GET() ,
			SYSTEM_SUB_ALG_STYLE_GET() ,
			( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL()        * 10 ) + _i_SCH_PRM_GET_MODULE_MODE( FM ) ,
			_i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING()
			);
		KWIN_Item_String_Display( hdlg , IDC_USER_ALGORITHM , Buffer );
		//---------------------------------------------------------------
		if ( ( USER_RECIPE_USE_TAG_GET() == -1 ) && ( USER_SCH_USE_TAG_GET() == -1 ) ) {
			KWIN_Item_Hide( hdlg , IDC_USER_DLL );
		}
		else {
			strcpy( Buffer , "" );
			switch ( USER_RECIPE_USE_TAG_GET() ) {
			case -1 :	strcat( Buffer , "N/A" );	break;
			case 0 :	strcat( Buffer , "Unload" );	break;
			case 1 :	strcat( Buffer , "Loaded" );	break;
			case 2 :	strcat( Buffer , "Loaded(Disable)" );	break;
			default :	strcat( Buffer , "Unknown" );	break;
			}
			if ( USER_SCH_USE_TAG_GET() != -1 ) {
				strcat( Buffer , "|" );
				switch ( USER_SCH_USE_TAG_GET() ) {
				case -1 :	strcat( Buffer , "N/A" );	break;
				case 0 :	strcat( Buffer , "Unload" );	break;
				case 1 :	strcat( Buffer , "Loaded" );	break;
				case 2 :	strcat( Buffer , "Loaded(Disable)" );	break;
				default :	strcat( Buffer , "Unknown" );	break;
				}
			}
		}
		KWIN_Item_String_Display( hdlg , IDC_USER_DLL , Buffer );
		//---------------------------------------------------------------
		switch ( USER_SCH_USE_TAG_GET() ) {
		case 0 :
			if ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME ) > 0 ) {
				sprintf( Buffer , "NotLoad-[%s]" , DRV_DLL_SCH_FILENAME );
			}
			else {
				sprintf( Buffer , "NotLoad" );
			}
			KWIN_Item_String_Display( hdlg , IDC_USER_MFI , Buffer );
			break;
		case 1 :
			if ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME ) > 0 ) {
				sprintf( Buffer , "Load-[%s]" , DRV_DLL_SCH_FILENAME );
			}
			else {
				sprintf( Buffer , "Load" );
			}
			KWIN_Item_String_Display( hdlg , IDC_USER_MFI , Buffer );
			break;
		case 2 :
			if ( STR_MEM_SIZE( DRV_DLL_SCH_FILENAME ) > 0 ) {
				sprintf( Buffer , "Disable-[%s]" , DRV_DLL_SCH_FILENAME );
			}
			else {
				sprintf( Buffer , "Disable" );
			}
			KWIN_Item_String_Display( hdlg , IDC_USER_MFI , Buffer );
			break;
		default :
			KWIN_Item_String_Display( hdlg , IDC_USER_MFI , "N/A" );
			break;
		}
		//---------------------------------------------------------------
		KWIN_Item_String_Display( hdlg , IDYES , "Debug Not Log" );
		//--------------------------------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDNO , SLS , "N/A|SysName|SysLog|SysName+Log|MessageLog|SysName+Msg|SysLog+Msg|Name+Log+Msg" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
//		id = SetTimer( hdlg , 1010 , 250 , NULL ); // 2008.04.18
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if      ( i == 0 ) c = IDC_LOG_STATUS1;
			else if ( i == 1 ) c = IDC_LOG_STATUS2;
			else if ( i == 2 ) c = IDC_LOG_STATUS3;
			else if ( i == 3 ) c = IDC_LOG_STATUS4;
			if      ( IO_LOT_LOG_STATUS( i ) == 2 )
				KWIN_Item_String_Display( hdlg , c , "File" );
			else if ( IO_LOT_LOG_STATUS( i ) == 1 )
				KWIN_Item_String_Display( hdlg , c , "Run" );
			else
				KWIN_Item_String_Display( hdlg , c , "N/A" );
		}
		LOG_MTL_STATUS( 0 , Buffer ); KWIN_Item_String_Display( hdlg , IDC_LOG_STATUS0 , Buffer );
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 6 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH );
		//
		id = SetTimer( hdlg , 1010 , 250 , NULL ); // 2008.04.18
		//
		return TRUE;

	case WM_NOTIFY: {
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO: {
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
								switch( GUI_PROCESS_LOG_MONITOR2_GET( i + PM1 ) ) {
								case 0 : sprintf( szString , "-" ); break;
								case 1 : sprintf( szString , "N/A" ); break;
								case 2 : sprintf( szString , "Wait" ); break;
								case 3 : sprintf( szString , "Start" ); break;
								case 4 : sprintf( szString , "[%s]" , GUI_PROCESS_LOG_MONITOR2_GRCP( i + PM1 ) ); break;
								case 5 : sprintf( szString , "*[%s]" , GUI_PROCESS_LOG_MONITOR2_GRCP( i + PM1 ) ); break;
								default : sprintf( szString , "X" ); break;
								}
								break;
							case 2 : // 2007.01.31
								switch( GUI_PROCESS_LOG_MONITOR2_GET( i + PM1 ) ) {
								case 0 : sprintf( szString , "" ); break;
								case 1 : sprintf( szString , "" ); break;
								case 2 : sprintf( szString , "" ); break;
								default : sprintf( szString , "%c:%d" , GUI_PROCESS_LOG_MONITOR2_GSD( i + PM1 ) + 'A' , GUI_PROCESS_LOG_MONITOR2_WID( i + PM1 ) ); break;
								}
								break;
							case 3 : // 2006.02.09
								switch( GUI_PROCESS_LOG_MONITOR2_GET( i + PM1 ) ) {
								case 4 :
								case 5 :
									sprintf( szString , "%d:%d" , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , i + PM1 ) , _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN_for_PE( 0 , i + PM1 ) ); // 2012.09,26
									break;
								default :
									sprintf( szString , "%d" , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 0 , i + PM1 ) ); // 2012.09,26
									break;
								}
								break;
							case 4 : // 2006.02.09
								switch( GUI_PROCESS_LOG_MONITOR2_GET( i + PM1 ) ) {
								case 4 :
								case 5 :
									sprintf( szString , "%d:%d" , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 2 , i + PM1 ) , _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN_for_PE( 2 , i + PM1 ) ); // 2012.09,26
									break;
								default :
									sprintf( szString , "%d" , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 2 , i + PM1 ) ); // 2012.09,26
									break;
								}
								break;
							case 5 : // 2006.02.09
								switch( GUI_PROCESS_LOG_MONITOR2_GET( i + PM1 ) ) {
								case 4 :
								case 5 :
									sprintf( szString , "%d:%d" , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 1 , i + PM1 ) , _i_SCH_TIMER_GET_PROCESS_TIME_REMAIN_for_PE( 1 , i + PM1 ) ); // 2012.09,26
									break;
								default :
									sprintf( szString , "%d" , _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN_for_PE( 1 , i + PM1 ) ); // 2012.09,26
									break;
								}
								break;
							}
//							lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
						}
					}
					else {
						if ( lpdi->item.mask & LVIF_TEXT ) {
							if ( ( PROCESSLOG_MONITOR_IO[ lpdi->item.iItem + PM1 ] >= 0 ) && ( PROCESS_DATALOG_Get_BUFFER_SIZE( lpdi->item.iItem + PM1 ) > 0 ) ) { // 2007.10.23
								wsprintf( szString, "PM%d:%d", lpdi->item.iItem + 1 , PROCESS_DATALOG_Get_BUFFER_SIZE( lpdi->item.iItem + PM1 ) ); // 2007.10.23
							}
							else {
								wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
							}
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
				if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , NULL , FALSE );
				return 0;
			case LVN_ITEMCHANGED :	return 0;
			case LVN_KEYDOWN :		return 0;
			}
		}
		return TRUE;

	case WM_TIMER:
		for ( i = 0 ; i <= MAX_CASSETTE_SIDE ; i++ ) {
			if      ( i == 0 ) c = IDC_SCH_STATUS1;
			else if ( i == 1 ) c = IDC_SCH_STATUS2;
			else if ( i == 2 ) c = IDC_SCH_STATUS3;
			else if ( i == 3 ) c = IDC_SCH_STATUS4;
			else if ( i == 4 ) c = IDC_SCH_STATUSTR;
			switch( _i_SCH_SYSTEM_USING_GET( i ) ) {
			case 0   : KWIN_Item_String_Display( hdlg , c , "- Ready -" ); break;
			case 1   : KWIN_Item_String_Display( hdlg , c , "(1) Start" ); break;
			case 2   : KWIN_Item_String_Display( hdlg , c , "(2) Sys - Pre Info Check" ); break;
			case 3   : KWIN_Item_String_Display( hdlg , c , "(3) Sys - Pre Data Set" ); break;
			case 4   : KWIN_Item_String_Display( hdlg , c , "(4) Sys - Recipe Data Check" ); break;
			case 5   : KWIN_Item_String_Display( hdlg , c , "(5) Run - HandOff In Part" ); break;
			case 6   : KWIN_Item_String_Display( hdlg , c , "(6) Run - Begin Part" ); break;
			case 7   : KWIN_Item_String_Display( hdlg , c , "(7) Run - Mapping Part" ); break;
			case 8   : KWIN_Item_String_Display( hdlg , c , "(8) Sys - Waiting for Scheduling" ); break;
			case 9   : KWIN_Item_String_Display( hdlg , c , "(9) Sys - Waiting for Scheduling2" ); break;
			case 10  : KWIN_Item_String_Display( hdlg , c , "(10) Sys - Scheduling Start" ); break;
			case 11  : KWIN_Item_String_Display( hdlg , c , "(11) Sch - Checking" ); break;
			case 12  : KWIN_Item_String_Display( hdlg , c , "(12) Sch - Wait Situation Matching" ); break;
			case 13  : KWIN_Item_String_Display( hdlg , c , "(13) Sch/Run - Pick from PM" ); break;
			case 14  : KWIN_Item_String_Display( hdlg , c , "(14) Sch/Run - Pick from CM" ); break;
			case 15  : KWIN_Item_String_Display( hdlg , c , "(15) Sch/Run - Place to CM/PM" ); break;
			case 16  : KWIN_Item_String_Display( hdlg , c , "(16) Sch/Run - Pick from BM" ); break;
			case 17  : KWIN_Item_String_Display( hdlg , c , "(17) Sch/Run - Place to BM/PM" ); break;
			case 18  : KWIN_Item_String_Display( hdlg , c , "(18) Sch/Run - Move PM" ); break;
			case 19  : KWIN_Item_String_Display( hdlg , c , "(19) Sch/Run - Move BM for Get" ); break;
			case 20  : KWIN_Item_String_Display( hdlg , c , "(20) Sch/Run - Move BM for Put" ); break;
			case 99  : KWIN_Item_String_Display( hdlg , c , "(99) Sch - Complete" ); break;
			case 100 : KWIN_Item_String_Display( hdlg , c , "(100) Sch - Cancel" ); break;
			case 107 : KWIN_Item_String_Display( hdlg , c , "(107) Run - Mapping Part(Rerun)" ); break;
			case 110 : KWIN_Item_String_Display( hdlg , c , "(110) Run - End Part" ); break;
			case 111 : KWIN_Item_String_Display( hdlg , c , "(111) Run - HandOff Out Part" ); break;
			case 120 : KWIN_Item_String_Display( hdlg , c , "(120) Sys - Post Data Set 1" ); break;
			case 121 : KWIN_Item_String_Display( hdlg , c , "(121) Run - Unload Part" ); break;
			case 122 : KWIN_Item_String_Display( hdlg , c , "(122) Sys - Post Data Set 2" ); break;
			default  : KWIN_Item_String_Display( hdlg , c , "-" ); break;
			}
		}

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if      ( i == 0 ) c = IDC_SCH2_STATUS1;
			else if ( i == 1 ) c = IDC_SCH2_STATUS2;
			else if ( i == 2 ) c = IDC_SCH2_STATUS3;
			else if ( i == 3 ) c = IDC_SCH2_STATUS4;
			//
			switch( _i_SCH_SYSTEM_USING2_GET( i ) ) {
			case 0   : KWIN_Item_String_Display( hdlg , c , "- Ready -" ); break;
			case 1   : KWIN_Item_String_Display( hdlg , c , "Start" ); break;
			case 2   : KWIN_Item_String_Display( hdlg , c , "Enter 0" ); break;
			case 3   : KWIN_Item_String_Display( hdlg , c , "Enter 1" ); break;
			case 4   : KWIN_Item_String_Display( hdlg , c , "Enter 2" ); break;
			case 5   : KWIN_Item_String_Display( hdlg , c , "Running" ); break;
				//
//			case 6   : KWIN_Item_String_Display( hdlg , c , "6 Wait Situation Matching" ); break;
//			case 7   : KWIN_Item_String_Display( hdlg , c , "7 Wait Situation Matching" ); break;
//			case 8   : KWIN_Item_String_Display( hdlg , c , "8 Wait Situation Matching" ); break;
				//
//			case 11  : KWIN_Item_String_Display( hdlg , c , "Run - Pick from CM" ); break;
//			case 12  : KWIN_Item_String_Display( hdlg , c , "Wait Place to BM" ); break;
//			case 13  : KWIN_Item_String_Display( hdlg , c , "Run - Place to BM" ); break;
//			case 21  : KWIN_Item_String_Display( hdlg , c , "Run - Pick from BM" ); break;
//			case 22  : KWIN_Item_String_Display( hdlg , c , "Run - Place to CM" ); break;
			default  : KWIN_Item_Int_Display( hdlg , c , _i_SCH_SYSTEM_USING2_GET( i ) ); break;
			}
		}
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if      ( i == 0 ) c = IDC_SCH3_STATUS1;
			else if ( i == 1 ) c = IDC_SCH3_STATUS2;
			else if ( i == 2 ) c = IDC_SCH3_STATUS3;
			else if ( i == 3 ) c = IDC_SCH3_STATUS4;
			KWIN_Item_Int_Display( hdlg , c , _i_SCH_SYSTEM_FA_GET( i ) );
		}

		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if      ( i == 0 ) c = IDC_SCH4_STATUS1;
			else if ( i == 1 ) c = IDC_SCH4_STATUS2;
			else if ( i == 2 ) c = IDC_SCH4_STATUS3;
			else if ( i == 3 ) c = IDC_SCH4_STATUS4;
			switch( BUTTON_GET_FLOW_STATUS_VAR( 100 + i ) ) {
			case 0   : KWIN_Item_String_Display( hdlg , c , "" ); break;
			case 1   : KWIN_Item_String_Display( hdlg , c , "(1) Load Request & Run" ); break;
			case 2   : KWIN_Item_String_Display( hdlg , c , "(2) Load Complete & Wait" ); break;
			case 3   : KWIN_Item_String_Display( hdlg , c , "(3) Load Request & Reject" ); break;
			case 4   : KWIN_Item_String_Display( hdlg , c , "(4) Map Request & Run" ); break;
			case 5   : KWIN_Item_String_Display( hdlg , c , "(5) Map Complete & Wait" ); break;
			case 6   : KWIN_Item_String_Display( hdlg , c , "(6) Map Request & Reject" ); break;
			case 7   : KWIN_Item_String_Display( hdlg , c , "(7) Run Start" ); break;
			case 8   : KWIN_Item_String_Display( hdlg , c , "(8) Run + HandOff In Start" ); break;
			case 9   : KWIN_Item_String_Display( hdlg , c , "(9) Run + Begin Start" ); break;
			case 10  : KWIN_Item_String_Display( hdlg , c , "(10) Run + Mapping In Start" ); break;
			case 11  : KWIN_Item_String_Display( hdlg , c , "(11) Run + Wait Signal" ); break;
			case 12  : KWIN_Item_String_Display( hdlg , c , "(12) Run + Looping" ); break;
			case 13  : KWIN_Item_String_Display( hdlg , c , "(13) Run + Mapping Out Start" ); break;
			case 14  : KWIN_Item_String_Display( hdlg , c , "(14) Run + HandOff Out Start" ); break;
			case 15  : KWIN_Item_String_Display( hdlg , c , "(15) RunEnd with Success" ); break;
			case 16  : KWIN_Item_String_Display( hdlg , c , "(16) RunEnd with Cancel" ); break;
			case 17  : KWIN_Item_String_Display( hdlg , c , "(17) RunEnd with Abort" ); break;
			case 18  : KWIN_Item_String_Display( hdlg , c , "(18) Unload Request & Run by Cancel" ); break;
			case 19  : KWIN_Item_String_Display( hdlg , c , "(19) Unload Request & Run" ); break;
			case 20  : KWIN_Item_String_Display( hdlg , c , "(20) Unload Complete & Wait by Cancel" ); break;
			case 21  : KWIN_Item_String_Display( hdlg , c , "(21) Unload Complete & Wait" ); break;
			case 22  : KWIN_Item_String_Display( hdlg , c , "(22) Unload Request & Reject" ); break;
			default  : KWIN_Item_Int_Display( hdlg , c , BUTTON_GET_FLOW_STATUS_VAR( 100 + i ) ); break;
			}
		}
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_PRCSMON_STATUS0 , GUI_PROCESS_LOG_MONITOR_GET() , "Wait||*" , "????" ); // 2007.07.24
		//---------------------------------------------------------------

		EQUIP_RUNNING_STRING_INFO( IO_DEBUG_RUNNING() , Buffer ); // 2008.04.18
		KWIN_Item_String_Display( hdlg , IDC_LOG_STATUS5 , Buffer ); // 2008.04.18
		//
		if ( IO_DEBUG_MONITOR_DATAVIEW == 0 ) { // 2008.09.08
			KWIN_Item_String_Display( hdlg , IDRETRY , "DV Off" );
		}
		else {
			KWIN_Item_String_Display( hdlg , IDRETRY , "DV On" );
		}
		//
		InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , NULL , FALSE );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDC_STATIC_MESSAGE : // 2014.07.10
			Gui_Open_Explorer( 0 );
			break;
		case IDC_STATIC_MESSAGE2 : // 2014.07.10
			Gui_Open_Explorer( 1 );
			break;
		case IDC_STATIC_MESSAGE3 : // 2014.07.10
			Gui_Open_Explorer( 2 );
			break;
		case IDC_STATIC_MESSAGE4 : // 2014.07.10
			Gui_Open_Explorer( 3 );
			break;
			//
		case IDC_USER_DLL :
			//================================================================================================
			if ( ( USER_RECIPE_USE_TAG_GET() == -1 ) && ( USER_SCH_USE_TAG_GET() == -1 ) ) {
				MessageBox( hdlg , "Analysis(MFI) DLL is not Defined", "Error", MB_ICONQUESTION );
			}
			else {
				//
				if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
					MessageBox( hdlg , "Analysis(MFI) DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
					break;
				}
				//
				if ( ( USER_RECIPE_USE_TAG_GET() != -1 ) && ( USER_SCH_USE_TAG_GET() != -1 ) ) {
					i = -1;
//					if ( !MODAL_DIGITAL_BOX1( hdlg , "DLL Mode" , "Select" , "Analysis DLL|MFI DLL" , &i ) ) i = -1; // 2013.10.31
					if ( !MODAL_DIGITAL_BOX1( hdlg , "DLL Mode" , "Select" , "Analysis DLL|MFI DLL" , &i ) ) break; // 2013.10.31
				}
				else {
					if ( USER_SCH_USE_TAG_GET() == -1 ) {
						i = 0;
					}
					else if ( USER_RECIPE_USE_TAG_GET() == -1 ) {
						i = 1;
					}
				}
				//
				while ( TRUE ) {
					//
					ret = FALSE;
					//
					if ( i == 0 ) {
						//
						switch( USER_RECIPE_USE_TAG_GET() ) {
						case 0 : // Not Loaded
							Res = -1;
							if ( MODAL_DIGITAL_BOX1( hdlg , "Analysis DLL Mode" , "Select" , "Loading|Loading with Disable|Cancel" , &Res ) ) {
								if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
									MessageBox( hdlg , "Analysis DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
									ret = TRUE;
								}
								else {
									if ( Res == 2 ) {
										ret = TRUE;
									}
									else {
										if ( !USER_RECIPE_DLL_LOAD( "" , ( Res == 0 ) ? 4 : 5 ) ) {
											MessageBox( hdlg , "Analysis DLL Operation Fail", "Error", MB_ICONQUESTION );
										}
									}
								}
							}
							else {
								ret = TRUE;
							}
							break;
						case 1 : // Loaded Enable
							Res = -1;
							if ( MODAL_DIGITAL_BOX1( hdlg , "Analysis DLL Mode" , "Select" , "Change Disable Mode|Unloading|Cancel" , &Res ) ) {
								if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
									MessageBox( hdlg , "Analysis DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
									ret = TRUE;
								}
								else {
									if ( Res == 2 ) {
										ret = TRUE;
									}
									else {
										if ( !USER_RECIPE_DLL_LOAD( "" , ( Res == 0 ) ? 1 : 3 ) ) {
											MessageBox( hdlg , "Analysis DLL Operation Fail", "Error", MB_ICONQUESTION );
										}
									}
								}
							}
							else {
								ret = TRUE;
							}
							break;
						case 2 : // Loaded Disable
							Res = -1;
							if ( MODAL_DIGITAL_BOX1( hdlg , "Analysis DLL Mode" , "Select" , "Change Enable Mode|Unloading|Cancel" , &Res ) ) {
								if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
									MessageBox( hdlg , "Analysis DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
									ret = TRUE;
								}
								else {
									if ( Res == 2 ) {
										ret = TRUE;
									}
									else {
										if ( !USER_RECIPE_DLL_LOAD( "" , ( Res == 0 ) ? 2 : 3 ) ) {
											MessageBox( hdlg , "Analysis DLL Operation Fail", "Error", MB_ICONQUESTION );
										}
									}
								}
							}
							else {
								ret = TRUE;
							}
							break;
						default : // 2013.10.31
							ret = TRUE;
							break;
						}
					}
					else if ( i == 1 ) {
						//
						switch( USER_SCH_USE_TAG_GET() ) {
						case 0 : // Not Loaded
							Res = -1;
							if ( MODAL_DIGITAL_BOX1( hdlg , "MFI DLL Mode" , "Select" , "Loading|Loading with Disable|Cancel" , &Res ) ) {
								if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
									MessageBox( hdlg , "MFI DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
									ret = TRUE;
								}
								else {
									if ( Res == 2 ) {
										ret = TRUE;
									}
									else {
										if ( !USER_SCH_DLL_LOAD( "" , "" , ( Res == 0 ) ? 4 : 5 ) ) {
											MessageBox( hdlg , "MFI DLL Operation Fail", "Error", MB_ICONQUESTION );
										}
									}
								}
							}
							else {
								ret = TRUE;
							}
							break;
						case 1 : // Loaded Enable
							Res = -1;
							if ( MODAL_DIGITAL_BOX1( hdlg , "MFI DLL Mode" , "Select" , "Change Disable Mode|Unloading|Cancel" , &Res ) ) {
								if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
									MessageBox( hdlg , "MFI DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
									ret = TRUE;
								}
								else {
									if ( Res == 2 ) {
										ret = TRUE;
									}
									else {
										if ( !USER_SCH_DLL_LOAD( "" , "" , ( Res == 0 ) ? 1 : 3 ) ) {
											MessageBox( hdlg , "MFI DLL Operation Fail", "Error", MB_ICONQUESTION );
										}
									}
								}
							}
							else {
								ret = TRUE;
							}
							break;
						case 2 : // Loaded Disable
							Res = -1;
							if ( MODAL_DIGITAL_BOX1( hdlg , "MFI DLL Mode" , "Select" , "Change Enable Mode|Unloading|Cancel" , &Res ) ) {
								if ( Get_RunPrm_RUNNING_CLUSTER() || Get_RunPrm_RUNNING_TRANSFER() ) {
									MessageBox( hdlg , "MFI DLL Operation Fail => System is Running", "Error", MB_ICONQUESTION );
									ret = TRUE;
								}
								else {
									if ( Res == 2 ) {
										ret = TRUE;
									}
									else {
										if ( !USER_SCH_DLL_LOAD( "" , "" , ( Res == 0 ) ? 2 : 3 ) ) {
											MessageBox( hdlg , "MFI DLL Operation Fail", "Error", MB_ICONQUESTION );
										}
									}
								}
							}
							else {
								ret = TRUE;
							}
							break;
						default : // 2013.10.31
							ret = TRUE;
							break;
						}
					}
					//---------------------------------------------------------------
					if ( ret ) break;
					//---------------------------------------------------------------
					if ( ( USER_RECIPE_USE_TAG_GET() == -1 ) && ( USER_SCH_USE_TAG_GET() == -1 ) ) {
						KWIN_Item_Hide( hdlg , IDC_USER_DLL );
					}
					else {
						strcpy( Buffer , "" );
						switch ( USER_RECIPE_USE_TAG_GET() ) {
						case -1 :	strcat( Buffer , "N/A" );	break;
						case 0 :	strcat( Buffer , "Unload" );	break;
						case 1 :	strcat( Buffer , "Loaded" );	break;
						case 2 :	strcat( Buffer , "Loaded(Disable)" );	break;
						default :	strcat( Buffer , "Unknown" );	break;
						}
						if ( USER_SCH_USE_TAG_GET() != -1 ) {
							strcat( Buffer , "|" );
							switch ( USER_SCH_USE_TAG_GET() ) {
							case -1 :	strcat( Buffer , "N/A" );	break;
							case 0 :	strcat( Buffer , "Unload" );	break;
							case 1 :	strcat( Buffer , "Loaded" );	break;
							case 2 :	strcat( Buffer , "Loaded(Disable)" );	break;
							default :	strcat( Buffer , "Unknown" );	break;
							}
						}
					}
					KWIN_Item_String_Display( hdlg , IDC_USER_DLL , Buffer );
					//---------------------------------------------------------------
				}
			}
			return TRUE;

		case IDRETRY : // 2008.09.08
			if ( IO_DEBUG_MONITOR_DATAVIEW == 0 ) {
				IO_DEBUG_MONITOR_DATAVIEW = 1;
				KWIN_Item_String_Display( hdlg , IDRETRY , "DV On" );
			}
			else {
				IO_DEBUG_MONITOR_DATAVIEW = 0;
				KWIN_Item_String_Display( hdlg , IDRETRY , "DV Off" );
			}
			return TRUE;

		case IDYES :
			Res = -1;
			if ( MODAL_DIGITAL_BOX1( hdlg , "Debug Mode" , "Select" , "Message Flow Log(1 Sec)|Message Flow Log(3 Sec)|Data Status Log|Data Status Log(View)|Message & Status Log|Message(FM Only) & Status Log|Message(TM Only) & Status Log|Open Data File|Open Message File|Kill Scheduler System" , &Res ) ) {
				if ( Res == 9 ) {
					if ( IDYES == MessageBox( hdlg , "Do you want to exit scheduler system?", "exit Confirmation", MB_ICONQUESTION | MB_YESNO ) ) {
						SYSTEM_KILL_MYSELF();
					}
				}
				else {
					//===========================================================
					switch( Res ) {
					case 2 :
					case 3 :
					case 4 :
					case 5 :
					case 6 :
//						SCHEDULING_DATA_STRUCTURE_LOG( "" , "" , -1 , 0 ); // 2013.02.20
//						SCHEDULING_DATA_STRUCTURE_JOB_LOG( "" , "" ); // 2004.10.06 // 2013.02.20
						//
						_i_SCH_SYSTEM_INFO_LOG( "" , "" ); // 2013.02.20
						break;
					}
					//===========================================================
					switch( Res ) {
					case 0 :
					case 1 :
					case 4 :
						if ( !IO_DEBUG_ENABLE( -1 ) ) {
							MessageBox( hdlg , "Message Debug Running Already", "Error", MB_ICONQUESTION );
							return TRUE;
						}
						KWIN_Item_String_Display( hdlg , IDYES , "Debug Log" );
						break;
					case 5 :
						if ( !IO_DEBUG_ENABLE( 0 ) ) {
							MessageBox( hdlg , "Message Debug Running Already", "Error", MB_ICONQUESTION );
							return TRUE;
						}
						KWIN_Item_String_Display( hdlg , IDYES , "Debug Log" );
						break;
					case 6 :
						if ( !IO_DEBUG_ENABLE( 1 ) ) {
							MessageBox( hdlg , "Message Debug Running Already", "Error", MB_ICONQUESTION );
							return TRUE;
						}
						KWIN_Item_String_Display( hdlg , IDYES , "Debug Log" );
						break;
					}
					//===========================================================
					switch( Res ) {
					case 1 :
						Sleep( 3000 );
						break;
					case 0 :
					case 4 :
					case 5 :
					case 6 :
						Sleep( 1000 );
						break;
					}
					//===========================================================
					switch( Res ) {
					case 3 :
					case 7 :
						if ( !File_Check_For_Exist( TRUE , SCHEDULER_DEBUG_DATA_FILE ) ) {
							MessageBox( hdlg , "Data File can not find !!", SCHEDULER_DEBUG_DATA_FILE , MB_ICONQUESTION );
						}
						else {
							_spawnlp( _P_NOWAIT , "NotePad" , "NotePad" , SCHEDULER_DEBUG_DATA_FILE , NULL );
						}
						break;
					case 8 :
						if ( !File_Check_For_Exist( TRUE , SCHEDULER_DEBUG_FLOW_FILE ) ) {
							MessageBox( hdlg , "Message File can not find !!", SCHEDULER_DEBUG_FLOW_FILE , MB_ICONQUESTION );
						}
						else {
							_spawnlp( _P_NOWAIT , "NotePad" , "NotePad" , SCHEDULER_DEBUG_FLOW_FILE , NULL );
						}
						break;
					case 0 :
					case 1 :
					case 4 :
					case 5 :
					case 6 :
						//
						IO_DEBUG_DISABLE( FALSE ); // 2015.06.11
						//
						KWIN_Item_String_Display( hdlg , IDYES , "Debug Not Log" );
						break;
					}
					//===========================================================
				}
			}
			return TRUE;

		case IDNO : // 2006.02.07
			//
			Res = SLS;
			//
//			if ( MODAL_DIGITAL_BOX1( hdlg , "System Log Mode" , "Select" , "N/A|Separate with System Name|Include System Inside Data|Separate with System Name and Include System Inside Data|Message Log|Separate with System Name and Message Log|Include System Inside Data and Message Log|Separate with System Name and Include System Inside Data and Message Log" , &Res ) ) { // 2015.04.09
//			if ( MODAL_DIGITAL_BOX1( hdlg , "System Log Mode" , "Select" , "N/A|Separate with System Name|Include System Inside Data|Separate with System Name and Include System Inside Data|Message Log|Separate with System Name and Message Log|Include System Inside Data and Message Log|Separate with System Name and Include System Inside Data and Message Log|Message Log 1Time(Off)|Message Log 1Time(On)" , &Res ) ) { // 2015.04.09 // 2015.05.26
			if ( MODAL_DIGITAL_BOX1( hdlg , "System Log Mode" , "Select" , "N/A|Separate with System Name|Include System Inside Data|Separate with System Name and Include System Inside Data|Message Log|Separate with System Name and Message Log|Include System Inside Data and Message Log|Separate with System Name and Include System Inside Data and Message Log|Message Log 1Time(Off)|Message Log 1Time(On)|Flag Log(Off)|Flag Log(On)" , &Res ) ) { // 2015.04.09 // 2015.05.26
				if      ( Res == 8 ) { // off
					EVENT_MESSAGE_LOG = FALSE; // 2015.04.09
					printf( "========================\n" );
					printf( "========================\n" );
					printf( "EVENT_MESSAGE_LOG is OFF\n" );
					printf( "========================\n" );
					printf( "========================\n" );
				}
				else if ( Res == 9 ) { // on
					EVENT_MESSAGE_LOG = TRUE; // 2015.04.09
					printf( "========================\n" );
					printf( "========================\n" );
					printf( "EVENT_MESSAGE_LOG is ON\n" );
					printf( "========================\n" );
					printf( "========================\n" );
				}
				else if ( Res == 10 ) { // off
					EVENT_FLAG_LOG = 0; // 2015.05.26
					printf( "========================\n" );
					printf( "========================\n" );
					printf( "EVENT_FLAG_LOG is OFF\n" );
					printf( "========================\n" );
					printf( "========================\n" );
				}
				else if ( Res == 11 ) { // on
					if      ( EVENT_FLAG_LOG == 0 ) { // 2016.06.09
						EVENT_FLAG_LOG = 1;
					}
					else if ( EVENT_FLAG_LOG == 1 ) { // 2016.06.09
						EVENT_FLAG_LOG = 2;
					}
					else if ( EVENT_FLAG_LOG == 2 ) { // 2016.06.09
						EVENT_FLAG_LOG = 3;
					}
					else {
						EVENT_FLAG_LOG = 1;
					}
					//
//					EVENT_FLAG_LOG = TRUE; // 2015.05.26 // 2016.06.09
					//
					printf( "========================\n" );
					printf( "========================\n" );
					//
					switch( EVENT_FLAG_LOG ) { // 2016.06.09
					case 1 :	printf( "EVENT_FLAG_LOG is ON\n" );	break;
					case 2 :	printf( "EVENT_FLAG_LOG is ON(Only-2)\n" );	break;
					case 3 :	printf( "EVENT_FLAG_LOG is ON(Only-3)\n" );	break;
					default :	printf( "EVENT_FLAG_LOG is OFF\n" );	break;
					break;
					}
					//
					printf( "========================\n" );
					printf( "========================\n" );
				}
				else {
					if ( Res != SLS ) {
						SLS = Res;
						_i_SCH_PRM_SET_SYSTEM_LOG_STYLE( SLS );
						//--------------------------------------------------------------------------------------
						KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDNO , SLS , "N/A|SysName|SysLog|SysName+Log|MessageLog|SysName+Msg|SysLog+Msg|Name+Log+Msg" , "????" ); // 2007.07.24
						//---------------------------------------------------------------
						GUI_SAVE_PARAMETER_DATA( 1 );
					}
				}
			}
			return TRUE;

		case IDOK :
		case IDCANCEL :
		case IDCLOSE :
			//
			IO_DEBUG_DISABLE( FALSE ); // 2015.06.11
			//
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;

	case WM_RBUTTONDOWN:
		GUI_LotData_Show( hdlg );
		return TRUE;

	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
