#include "default.h"

#include "EQ_Enum.h"

#include "File_ReadInit.h"
#include "Equip_Handling.h"
#include "GUI_Handling.h"
#include "IO_Part_data.h"
#include "FA_Handling.h"
#include "IO_Part_Log.h"
#include "DLL_Interface.h"
#include "iolog.h"
#include "Multijob.h"
#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "system_tag.h"
#include "Robot_Handling.h"
#include "utility.h"
#include "resource.h"
#include "version.h"
#include <commctrl.h>


BOOL EVENT_MESSAGE_LOG = FALSE; // 2015.04.09
int  EVENT_FLAG_LOG = 0; // 2015.05.26
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_CPJOB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
int GUI_PRCS_LOG_MON = 0;
int GUI_PRCS_LOG_MON2[MAX_CHAMBER];
int GUI_PRCS_LOG_MSD2[MAX_CHAMBER];
int GUI_PRCS_LOG_WID2[MAX_CHAMBER];
char GUI_PRCS_LOG_RCP2[MAX_CHAMBER][32+1];

//----------------------------------------------------------------------------
void GUI_PROCESS_LOG_MONITOR_RUN() { if ( GUI_PRCS_LOG_MON >= 2 ) GUI_PRCS_LOG_MON = 1; else GUI_PRCS_LOG_MON++; }
int  GUI_PROCESS_LOG_MONITOR_GET() {	return GUI_PRCS_LOG_MON; }
void GUI_PROCESS_LOG_MONITOR_RESET() {	GUI_PRCS_LOG_MON = 0; }
//----------------------------------------------------------------------------
void GUI_PROCESS_LOG_MONITOR2_SET( int ch , int data , int sd , int wid , char *rcp ) {
	if ( data == 2 ) {
		GUI_PRCS_LOG_MSD2[ch] = sd;
		GUI_PRCS_LOG_WID2[ch] = wid;
		strncpy( GUI_PRCS_LOG_RCP2[ch] , rcp , 32 );
		GUI_PRCS_LOG_RCP2[ch][32] = 0; // 2007.10.18
		GUI_PRCS_LOG_MON2[ch] = data;
	}
	else {
		GUI_PRCS_LOG_MON2[ch] = data;
		GUI_PRCS_LOG_MSD2[ch] = sd;
		GUI_PRCS_LOG_WID2[ch] = wid;
		strncpy( GUI_PRCS_LOG_RCP2[ch] , rcp , 32 );
		GUI_PRCS_LOG_RCP2[ch][32] = 0; // 2007.10.18
	}
}
void GUI_PROCESS_LOG_MONITOR2_RUN( int ch ) { if ( GUI_PRCS_LOG_MON2[ch] >= 5 ) GUI_PRCS_LOG_MON2[ch] = 4; else GUI_PRCS_LOG_MON2[ch]++; }
int  GUI_PROCESS_LOG_MONITOR2_GET( int ch ) { return GUI_PRCS_LOG_MON2[ch]; }
int  GUI_PROCESS_LOG_MONITOR2_GSD( int ch ) { return GUI_PRCS_LOG_MSD2[ch]; }
int  GUI_PROCESS_LOG_MONITOR2_WID( int ch ) { return GUI_PRCS_LOG_WID2[ch]; }
char *GUI_PROCESS_LOG_MONITOR2_GRCP( int ch ) { return GUI_PRCS_LOG_RCP2[ch]; }
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int GUI_GROUP_SELECT = 0;
int GUI_MD_DISPLAY = 0;
int GUI_RB_DISPLAY = 0;
int GUI_TM_DISPLAY = 0;
int GUI_FM_DISPLAY = 0;


void GUI_GROUP_SELECT_SET( int data ) { GUI_GROUP_SELECT = data; }
int  GUI_GROUP_SELECT_GET() { return GUI_GROUP_SELECT; }

void GUI_MD_DISPLAY_SET( int data ) { GUI_MD_DISPLAY = data; }
int  GUI_MD_DISPLAY_GET() { return GUI_MD_DISPLAY; }

void GUI_RB_DISPLAY_SET( int data ) { GUI_RB_DISPLAY = data; }
int  GUI_RB_DISPLAY_GET() { return GUI_RB_DISPLAY; }

void GUI_TM_DISPLAY_SET( int data ) { GUI_TM_DISPLAY = data; }
int  GUI_TM_DISPLAY_GET() { return GUI_TM_DISPLAY; }

void GUI_FM_DISPLAY_SET( int data ) { GUI_FM_DISPLAY = data; }
int  GUI_FM_DISPLAY_GET() { return GUI_FM_DISPLAY; }

//=================================================================================
//=================================================================================
CRITICAL_SECTION CR_GUI_SAVE_PARAMETER; // 2015.01.23

void GUI_SAVE_PARAMETER_INIT() { // 2015.01.23
	InitializeCriticalSection( &CR_GUI_SAVE_PARAMETER );
}

extern BOOL SCHFILE_USE_END_DELIMITER; // 2017.02.09

BOOL FILE_SCHEDULE_SETUP_HAS_END_DELIMETER( char *Filename_Org ); // 2017.11.02
extern int SCHFILE_USE_END_DELIMITER_ALARM; // 2017.11.02

void GUI_SAVE_PARAMETER_DATA( int mode ) {
	char Filename[128];
	char FilenameB[128]; // 2017.11.02
	char FileTemp[128]; // 2015.01.23
	//
	EnterCriticalSection( &CR_GUI_SAVE_PARAMETER ); // 2015.01.23
	//
	if ( ( mode == 0 ) || ( mode == 1 ) ) {
		//
		/*
		//
		// 2017.11.02
		//
		sprintf( Filename , "%s/Schedule.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
		//
		File_Buffering_For_SavePre( Filename , FileTemp ); // 2015.01.23
		//
//		FILE_SCHEDULE_SETUP_SAVE( Filename ); // 2015.01.23
//		FILE_SCHEDULE_SETUP_SAVE( FileTemp ); // 2015.01.23 // 2017.02.09
		FILE_SCHEDULE_SETUP_SAVE( FileTemp , TRUE ); // 2015.01.23 // 2017.02.09
		//
		File_Buffering_For_SavePost( Filename , FileTemp ); // 2015.01.23
		//
		if ( SCHFILE_USE_END_DELIMITER ) { // 2017.02.09
			//
			strcat( Filename , ".bak" );
			//
			File_Buffering_For_SavePre( Filename , FileTemp );
			//
//			FILE_SCHEDULE_SETUP_SAVE( FileTemp ); // 2017.02.09
			FILE_SCHEDULE_SETUP_SAVE( FileTemp , FALSE ); // 2017.02.09
			//
			File_Buffering_For_SavePost( Filename , FileTemp );
			//
		}
		//
		*/
		//
		// 2017.11.02
		//
		sprintf( Filename  , "%s/Schedule.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
		//
		File_Buffering_For_SavePre( Filename , FileTemp ); // 2015.01.23
		//
		FILE_SCHEDULE_SETUP_SAVE( FileTemp , TRUE ); // 2015.01.23 // 2017.02.09
		//
		File_Buffering_For_SavePost( Filename , FileTemp ); // 2015.01.23
		//
		if ( SCHFILE_USE_END_DELIMITER ) { // 2017.02.09
			//
			sprintf( FilenameB , "%s/Schedule.cfg.bak" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
			//
			File_Buffering_For_SavePre( FilenameB , FileTemp );
			//
			FILE_SCHEDULE_SETUP_SAVE( FileTemp , FALSE ); // 2017.02.09
			//
			File_Buffering_For_SavePost( FilenameB , FileTemp );
			//
			//
			if ( !FILE_SCHEDULE_SETUP_HAS_END_DELIMETER( Filename ) ) { // 2017.11.02
				//
				File_Buffering_For_SavePre( Filename , FileTemp );
				//
				FILE_SCHEDULE_SETUP_SAVE( FileTemp , FALSE );
				//
				File_Buffering_For_SavePost( Filename , FileTemp );
				//
				if ( SCHFILE_USE_END_DELIMITER_ALARM > 0 ) {
					if ( !FILE_SCHEDULE_SETUP_HAS_END_DELIMETER( Filename ) ) ALARM_POST( SCHFILE_USE_END_DELIMITER_ALARM );
				}
				//
			}
			//
			if ( !FILE_SCHEDULE_SETUP_HAS_END_DELIMETER( FilenameB ) ) { // 2017.11.02
				//
				File_Buffering_For_SavePre( FilenameB , FileTemp );
				//
				FILE_SCHEDULE_SETUP_SAVE( FileTemp , FALSE );
				//
				File_Buffering_For_SavePost( FilenameB , FileTemp );
				//
				if ( SCHFILE_USE_END_DELIMITER_ALARM > 0 ) {
					if ( !FILE_SCHEDULE_SETUP_HAS_END_DELIMETER( FilenameB ) ) ALARM_POST( SCHFILE_USE_END_DELIMITER_ALARM );
				}
				//
			}
			//
		}
		//
		//
		//
		//
	}
	if ( ( mode == 0 ) || ( mode == 2 ) ) {
		//
		sprintf( Filename , "%s/Robot.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
		//
		File_Buffering_For_SavePre( Filename , FileTemp ); // 2015.01.23
		//
//		FILE_ROBOT_SETUP_SAVE( Filename ); // 2015.01.23
		FILE_ROBOT_SETUP_SAVE( FileTemp ); // 2015.01.23
		//
		File_Buffering_For_SavePost( Filename , FileTemp ); // 2015.01.23
		//
	}
	//
	LeaveCriticalSection( &CR_GUI_SAVE_PARAMETER ); // 2015.01.23
	//
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GUI_Event_Data_Inserting( HWND hWnd , int mode , char *message ) {
	int i;
//	struct tm Pres_Time; // 2006.10.02
	SYSTEMTIME		SysTime; // 2006.10.02
	char SendBuffer[110];
	char SendBuffer2[101];

//		switch ( mode ) {
//		case 0 :	_IO_MSGEVENT_PRINTF( "RECEIVE" , message );	break;
//		case 1 :	_IO_MSGEVENT_PRINTF( "SEND" , message );	break;
//		case 2 :	_IO_MSGEVENT_PRINTF( "REJECT" , message );	break;
//		default :	_IO_MSGEVENT_PRINTF( "UNKNOWN" , message );	break;
//		}
	//===============================================================================
	// 2007.07.04
	//===============================================================================
//	if ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() >= 4 ) { // 2007.07.04 // 2015.04.09
	if ( EVENT_MESSAGE_LOG || ( _i_SCH_PRM_GET_SYSTEM_LOG_STYLE() >= 4 ) ) { // 2007.07.04 // 2015.04.09
		switch ( mode ) {
		case 0 :	_IO_MSGEVENT_PRINTF( "RECEIVE" , message );	break;
		case 1 :	_IO_MSGEVENT_PRINTF( "SEND" , message );	break;
		case 2 :	_IO_MSGEVENT_PRINTF( "REJECT" , message );	break;
		default :	_IO_MSGEVENT_PRINTF( "UNKNOWN" , message );	break;
		}
	}
	else { // 2018.09.25
		if ( mode == 0 ) SCHMULTI_SET_LOG_MESSAGE( message );
	}
	//===============================================================================
	if ( hWnd == NULL ) return;
	//===============================================================================
	strncpy( SendBuffer2 , message , 100 );
	SendBuffer2[100] = 0;
	if ( mode == 2 ) {
		sprintf( SendBuffer , " >>REJECT: %s" , SendBuffer2 );
	}
	else {
//		_get-systime( &Pres_Time ); // 2006.10.02
//		sprintf( SendBuffer , "%02d:%02d:%02d %s" , Pres_Time.tm_hour , Pres_Time.tm_min , Pres_Time.tm_sec , SendBuffer2 ); // 2006.10.02
		GetLocalTime( &SysTime ); // 2006.10.02
		sprintf( SendBuffer , "%02d:%02d:%02d %s" , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SendBuffer2 ); // 2006.10.02
	}
	i = ListBox_GetCount( hWnd );
	if ( i > MAX_LISTVIEW_DISPLAY_COUNT ) {
		ListBox_DeleteString( hWnd , 0 );
		i--;
	}
	ListBox_AddString( hWnd , SendBuffer );
	ListBox_SetCurSel( hWnd , i );
	ListBox_SetSel( hWnd , TRUE , i );
}
//
void GUI_Receive_Event_Inserting( char *message ) {
	GUI_Event_Data_Inserting( ( KGUI_STANDARD_Get_Screen_Handle() == NULL ) ? NULL : GetDlgItem( KGUI_STANDARD_Get_Screen_Handle() , IDC_MESSAGE_LIST ) , 0 , message );
}
//
void GUI_Reject_Event_Inserting( char *message ) {
	GUI_Event_Data_Inserting( ( KGUI_STANDARD_Get_Screen_Handle() == NULL ) ? NULL : GetDlgItem( KGUI_STANDARD_Get_Screen_Handle() , IDC_MESSAGE_LIST ) , 2 , message );
}
//
void GUI_Send_Event_Inserting( char *message ) {
	GUI_Event_Data_Inserting( ( KGUI_STANDARD_Get_Screen_Handle() == NULL ) ? NULL : GetDlgItem( KGUI_STANDARD_Get_Screen_Handle() , IDC_MESSAGE_LIST2 ) , 1 , message );
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GUI_Parameter_Drawing( HWND hWnd , HDC hDC ) {
	int i , j , c , c2 , sp , ep , sc , ec , Mode , fmd;
	char Buffer[32];

//	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) && !_i_SCH_PRM_GET_MODULE_BUFFER_USE() && ( GUI_MD_DISPLAY_GET() >= 100 ) ) GUI_MD_DISPLAY_SET( 0 ); // 2008.02.21

	Mode = GUI_MD_DISPLAY_GET();
	//=======================================================================================
	// Select Item Display & Claculate
	//=======================================================================================
	if      ( Mode == 0 ) { // CM1-4 / PM1-6 / ( AL & IC )
		c = 1;
		c2 = ( c + 6 ) - 1;
		if ( c2 >= MAX_PM_CHAMBER_DEPTH ) c2 = MAX_PM_CHAMBER_DEPTH;
		sp = c  - 1 + PM1;
		ep = c2 - 1 + PM1;
		KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , "Module" );
	}
	else if ( Mode >= 100 ) { // TM? / BM? / ( FAL & FIC )
		c = ( ( Mode - 100 ) * 6 ) + 1;
		c2 = ( c + 6 ) - 1;
		if ( c2 >= MAX_BM_CHAMBER_DEPTH ) c2 = MAX_BM_CHAMBER_DEPTH;
		sp = c  - 1 + BM1;
		ep = c2 - 1 + BM1;
		if ( Mode == 100 )
//			sprintf( Buffer , "Buffer" , c , c2 ); // 2019.03.07
			strcpy( Buffer , "Buffer" ); // 2019.03.07
		else
			sprintf( Buffer , "Buf(%d-%d)" , c , c2 );
		KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , Buffer );
	}
	else { // x / PM? / ( AL & IC )
		c = ( Mode * 6 ) + 1;
		c2 = ( c + 6 ) - 1;
		if ( c2 >= MAX_PM_CHAMBER_DEPTH ) c2 = MAX_PM_CHAMBER_DEPTH;
		sp = c  - 1 + PM1;
		ep = c2 - 1 + PM1;
		sprintf( Buffer , "Mdl(%d-%d)" , c , c2 );
		KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , Buffer );
	}
	//=======================================================================================
	fmd = FALSE;
	//
	if ( Mode < 100 ) {
		c = ( ( Mode ) * 4 ) + 1;
		c2 = ( c + 4 ) - 1;
		if ( c2 >= MAX_CASSETTE_SIDE ) c2 = MAX_CASSETTE_SIDE;
		sc = c  - 1 + CM1;
		ec = c2 - 1 + CM1;
		if ( sc >= MAX_CASSETTE_SIDE + CM1 ) {
//			sc = 0; // 2005.12.15
//			ec = -1; // 2005.12.15
			//================================================================
			// 2005.12.15
			//================================================================
			c = ( ( Mode - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 ) + 1;
			c2 = ( c + 3 ) - 1;
			if ( c2 >= MAX_TM_CHAMBER_DEPTH ) c2 = MAX_TM_CHAMBER_DEPTH;
			sc = c  - 1 + TM;
			ec = c2 - 1 + TM;
			if ( sc >= MAX_TM_CHAMBER_DEPTH + TM ) {
				sc = 0;
				ec = -1;
			}
			else {
				fmd = TRUE;
			}
			//================================================================
		}
	}
	else {
		c = ( ( Mode - 100 ) * 4 ) + 1;
		c2 = ( c + 4 ) - 1;
		if ( c2 >= MAX_TM_CHAMBER_DEPTH ) c2 = MAX_TM_CHAMBER_DEPTH;
		sc = c  - 1 + TM;
		ec = c2 - 1 + TM;
		if ( sc >= MAX_TM_CHAMBER_DEPTH + TM ) {
			sc = 0;
			ec = -1;
		}
	}
	j = 0;
	//===============================================================================================
	// Cassette/Transfer Status Display
	//===============================================================================================
	for ( i = sc ; i <= ec ; i++ , j++ ) {
		if      ( j == 0 ) c = IDC_MODULE_NAME_C1;
		else if ( j == 1 ) c = IDC_MODULE_NAME_C2;
		else if ( j == 2 ) c = IDC_MODULE_NAME_C3;
		else if ( j == 3 ) c = IDC_MODULE_NAME_C4;
		if ( i >= TM ) {
			if ( i == TM ) {
				sprintf( Buffer , "TM" );
			}
			else {
				sprintf( Buffer , "TM%d" , i - TM + 1 );
			}
		}
		else {
			sprintf( Buffer , "Cassette %d" , i - CM1 + 1 );
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )	{
			if ( EQUIP_INTERFACE_FUNCTION_CONNECT( i ) ) {
				KWIN_Item_String_Display( hWnd , c , Buffer );
			}
			else {
				KWIN_Item_String_Display( hWnd , c , Buffer );
				KWIN_Item_Disable( hWnd , c );
			}
		}
		else {
			KWIN_Item_Hide( hWnd , c );
		}
		//=====================================================================================
		if      ( j == 0 ) c = IDC_MODULE_REMLOC_C1;
		else if ( j == 1 ) c = IDC_MODULE_REMLOC_C2;
		else if ( j == 2 ) c = IDC_MODULE_REMLOC_C3;
		else if ( j == 3 ) c = IDC_MODULE_REMLOC_C4;
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )	{
			//=============================================================================
			KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_MODULE_SERVICE_MODE( i ) , "Local|Remote" , "????" ); // 2007.07.24
			//=============================================================================
		}
		else {
			KWIN_Item_Hide( hWnd , c );
		}
		//=====================================================================================
		if      ( j == 0 ) c = IDC_GET_PRIORITY_C1;
		else if ( j == 1 ) c = IDC_GET_PRIORITY_C2;
		else if ( j == 2 ) c = IDC_GET_PRIORITY_C3;
		else if ( j == 3 ) c = IDC_GET_PRIORITY_C4;
//		if ( i >= TM ) { // 2006.02.09
		if ( i >= FM ) { // 2006.02.09
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )
				KWIN_Item_Int_Display( hWnd , c , _i_SCH_PRM_GET_Getting_Priority( i ) );
			else
				KWIN_Item_Hide( hWnd , c );
		}
		//=====================================================================================
		if      ( j == 0 ) c = IDC_PUT_PRIORITY_C1;
		else if ( j == 1 ) c = IDC_PUT_PRIORITY_C2;
		else if ( j == 2 ) c = IDC_PUT_PRIORITY_C3;
		else if ( j == 3 ) c = IDC_PUT_PRIORITY_C4;
		if ( i >= TM ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )
				KWIN_Item_Int_Display( hWnd , c , _i_SCH_PRM_GET_Putting_Priority( i ) );
			else
				KWIN_Item_Hide( hWnd , c );
		}
		//=====================================================================================
		if      ( j == 0 ) c = IDC_SW_CONTROL_C1;
		else if ( j == 1 ) c = IDC_SW_CONTROL_C2;
		else if ( j == 2 ) c = IDC_SW_CONTROL_C3;
		else if ( j == 3 ) c = IDC_SW_CONTROL_C4;
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			//=============================================================================
			KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
			//=============================================================================
		}
		//=====================================================================================
		if      ( j == 0 ) c = IDC_MODULE_PICK_C1;
		else if ( j == 1 ) c = IDC_MODULE_PICK_C2;
		else if ( j == 2 ) c = IDC_MODULE_PICK_C3;
		else if ( j == 3 ) c = IDC_MODULE_PICK_C4;
		if ( i >= TM ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )	{
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_STATION_PICK_HANDLING( i ) , "Synch|Asynch" , "????" ); // 2007.07.24
				//=============================================================================
			}
			else {
				KWIN_Item_Hide( hWnd , c );
			}
		}
		//=====================================================================================
		if      ( j == 0 ) c = IDC_MODULE_PLACE_C1;
		else if ( j == 1 ) c = IDC_MODULE_PLACE_C2;
		else if ( j == 2 ) c = IDC_MODULE_PLACE_C3;
		else if ( j == 3 ) c = IDC_MODULE_PLACE_C4;
		if ( i >= TM ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) )	{
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_STATION_PLACE_HANDLING( i ) , "Synch|Asynch" , "????" ); // 2007.07.24
				//=============================================================================
			}
			else {
				KWIN_Item_Hide( hWnd , c );
			}
		}
		//=====================================================================================
		if      ( j == 0 ) { c = IDC_CASS_MAP_C1; c2 = IDC_AUTO_HANDOFF_C1; }
		else if ( j == 1 ) { c = IDC_CASS_MAP_C2; c2 = IDC_AUTO_HANDOFF_C2; }
		else if ( j == 2 ) { c = IDC_CASS_MAP_C3; c2 = IDC_AUTO_HANDOFF_C3; }
		else if ( j == 3 ) { c = IDC_CASS_MAP_C4; c2 = IDC_AUTO_HANDOFF_C4; }
		if ( i >= TM ) {
			KWIN_Item_Hide( hWnd , c );
			KWIN_Item_Hide( hWnd , c2 );
		}
		else {
			if      ( !_i_SCH_PRM_GET_MODULE_MODE( i ) )			{
				KWIN_Item_Hide( hWnd , c );
				KWIN_Item_Hide( hWnd , c2 );
			}
			else {
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c  , _i_SCH_PRM_GET_MAPPING_SKIP( i ) , "Mapping|Skip|MapAll|MapEndChk|MapAllChk|MapEnd|Skip(U.M)|MECk(U.M)|MEnd(U.M)" , "????" ); // 2007.07.24
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c2 , _i_SCH_PRM_GET_AUTO_HANDOFF( i ) , ".|Load|Unload|Load/Unload|Unld(Chk)|Ld/Unld(Chk)" , "????" ); // 2007.07.24
				//=============================================================================
			}
		}
	}
	//
	for ( ; j < 4 ; j++ ) {
		if ( fmd && ( j == 3 ) ) continue;
		if      ( j == 0 ) c = IDC_MODULE_NAME_C1;
		if      ( j == 1 ) c = IDC_MODULE_NAME_C2;
		else if ( j == 2 ) c = IDC_MODULE_NAME_C3;
		else if ( j == 3 ) c = IDC_MODULE_NAME_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) c = IDC_MODULE_REMLOC_C1;
		else if ( j == 1 ) c = IDC_MODULE_REMLOC_C2;
		else if ( j == 2 ) c = IDC_MODULE_REMLOC_C3;
		else if ( j == 3 ) c = IDC_MODULE_REMLOC_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) c = IDC_GET_PRIORITY_C1;
		else if ( j == 1 ) c = IDC_GET_PRIORITY_C2;
		else if ( j == 2 ) c = IDC_GET_PRIORITY_C3;
		else if ( j == 3 ) c = IDC_GET_PRIORITY_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) c = IDC_PUT_PRIORITY_C1;
		else if ( j == 1 ) c = IDC_PUT_PRIORITY_C2;
		else if ( j == 2 ) c = IDC_PUT_PRIORITY_C3;
		else if ( j == 3 ) c = IDC_PUT_PRIORITY_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) c = IDC_SW_CONTROL_C1;
		else if ( j == 1 ) c = IDC_SW_CONTROL_C2;
		else if ( j == 2 ) c = IDC_SW_CONTROL_C3;
		else if ( j == 3 ) c = IDC_SW_CONTROL_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) c = IDC_MODULE_PICK_C1;
		else if ( j == 1 ) c = IDC_MODULE_PICK_C2;
		else if ( j == 2 ) c = IDC_MODULE_PICK_C3;
		else if ( j == 3 ) c = IDC_MODULE_PICK_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) c = IDC_MODULE_PLACE_C1;
		else if ( j == 1 ) c = IDC_MODULE_PLACE_C2;
		else if ( j == 2 ) c = IDC_MODULE_PLACE_C3;
		else if ( j == 3 ) c = IDC_MODULE_PLACE_C4;
		KWIN_Item_Hide( hWnd , c );
		//
		if      ( j == 0 ) { c = IDC_CASS_MAP_C1; c2 = IDC_AUTO_HANDOFF_C1; }
		else if ( j == 1 ) { c = IDC_CASS_MAP_C2; c2 = IDC_AUTO_HANDOFF_C2; }
		else if ( j == 2 ) { c = IDC_CASS_MAP_C3; c2 = IDC_AUTO_HANDOFF_C3; }
		else if ( j == 3 ) { c = IDC_CASS_MAP_C4; c2 = IDC_AUTO_HANDOFF_C4; }
		KWIN_Item_Hide( hWnd , c );
		KWIN_Item_Hide( hWnd , c2 );
	}
	//=====================================================================================
	if ( fmd ) {
		/*
		//
		// 2018.12.14
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_C4 , "FM" );
			//=============================================================================
			KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_MODULE_REMLOC_C4 , _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) , "Local|Remote" , "????" ); // 2007.07.24
			//=============================================================================
			KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_SW_CONTROL_C4 , _i_SCH_PRM_GET_MODULE_MODE_for_SW( FM ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
			//=============================================================================
		}
		else {
			KWIN_Item_Hide( hWnd , IDC_MODULE_NAME_C4 );
			KWIN_Item_Hide( hWnd , IDC_SW_CONTROL_C4 );
			KWIN_Item_Hide( hWnd , IDC_MODULE_REMLOC_C4 );
		}
		//
		*/
		//
		//
		// 2018.12.14
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_C4 , "FM" );
		}
		else {
			KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_C4 , "FM[X]" );
		}
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_MODULE_REMLOC_C4 , _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) , "Local|Remote" , "????" ); // 2007.07.24
		//=============================================================================
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_SW_CONTROL_C4 , _i_SCH_PRM_GET_MODULE_MODE_for_SW( FM ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
		//=============================================================================
		//
		//
		//
		//
		KWIN_Item_Hide( hWnd , IDC_GET_PRIORITY_C4 );
		KWIN_Item_Hide( hWnd , IDC_PUT_PRIORITY_C4 );
		KWIN_Item_Hide( hWnd , IDC_MODULE_PICK_C4 );
		KWIN_Item_Hide( hWnd , IDC_MODULE_PLACE_C4 );
		KWIN_Item_Hide( hWnd , IDC_CASS_MAP_C4 );
		KWIN_Item_Hide( hWnd , IDC_AUTO_HANDOFF_C4 );
	}
	//=====================================================================================
	if ( Mode < 100 ) {
		if ( _i_SCH_PRM_GET_MODULE_MODE( AL ) ) {
			if ( EQUIP_INTERFACE_FUNCTION_CONNECT( TM ) ) {
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_AL , "Aligner" );
			}
			else {
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_AL , "Aligner" );
				KWIN_Item_Disable( hWnd , IDC_MODULE_NAME_AL );
			}
		}
		else {
			KWIN_Item_Hide( hWnd , IDC_MODULE_NAME_AL );
		}
		if ( _i_SCH_PRM_GET_MODULE_MODE( IC ) ) {
			if ( EQUIP_INTERFACE_FUNCTION_CONNECT( TM ) ) {
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , "Cooler" );
			}
			else {
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , "Cooler" );
				KWIN_Item_Disable( hWnd , IDC_MODULE_NAME_IC );
			}
		}
		else {
			KWIN_Item_Hide( hWnd , IDC_MODULE_NAME_IC );
		}
		//---------------------------------------------------------------------------------
		for ( i = AL ; i <= IC ; i++ ) {
			if      ( i == AL ) { c = IDC_SW_CONTROL_AL; }
			else if ( i == IC ) { c = IDC_SW_CONTROL_IC; }
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) {
				KWIN_Item_Hide( hWnd , c );
			}
			else {
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_MODULE_MODE_for_SW( i ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
				//=============================================================================
			}
		}
	}
	else {
		if ( _i_SCH_PRM_GET_MODULE_MODE( F_AL ) ) {
			if      ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2005.10.04
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_AL , "Aligner(F:2)" );
			}
			else if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 2 ) { // 2006.11.16
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_AL , "Aligner(F:X)" );
			}
			else if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) { // 2006.11.16
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_AL , "Aligner(F:T)" );
			}
			else {
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_AL , "Aligner(F)" );
			}
		}
		else {
			KWIN_Item_Hide( hWnd , IDC_SW_CONTROL_AL );
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( F_IC ) ) {
			if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2005.10.04 // 2010.12.22
				if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_FNAME()[0] != 0 ) {
					KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , "Cooler(F:T:F1)" );
				}
				else {
					sprintf( Buffer , "Cooler(F:T:M%d)" , _i_SCH_PRM_GET_MODULE_SIZE(F_IC) );
					KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , Buffer );
				}
			}
			else if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) { // 2010.12.22
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , "Cooler(FAL)" );
			}
			else if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) { // 2010.12.22
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , "Cooler(FAL:T)" );
			}
			else {
				KWIN_Item_String_Display( hWnd , IDC_MODULE_NAME_IC , "Cooler(F)" );
			}
		}
		else {
			KWIN_Item_Hide( hWnd , IDC_SW_CONTROL_IC );
		}
		//---------------------------------------------------------------------------------
		if ( !_i_SCH_PRM_GET_MODULE_MODE( F_AL ) ) {
			KWIN_Item_Hide( hWnd , IDC_SW_CONTROL_AL );
		}
		else {
			//=============================================================================
			KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_SW_CONTROL_AL , _i_SCH_PRM_GET_MODULE_MODE_for_SW( F_AL ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
			//=============================================================================
		}
		if ( !_i_SCH_PRM_GET_MODULE_MODE( F_IC ) ) {
			KWIN_Item_Hide( hWnd , IDC_SW_CONTROL_IC );
		}
		else {
			//=============================================================================
			KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_SW_CONTROL_IC , _i_SCH_PRM_GET_MODULE_MODE_for_SW( F_IC ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
			//=============================================================================
		}
	}

	for ( i = 0 ; i < 6 ; i++ ) {
		if      ( i == 0 ) c = IDC_MODULE_NAME_P1;
		else if ( i == 1 ) c = IDC_MODULE_NAME_P2;
		else if ( i == 2 ) c = IDC_MODULE_NAME_P3;
		else if ( i == 3 ) c = IDC_MODULE_NAME_P4;
		else if ( i == 4 ) c = IDC_MODULE_NAME_P5;
		else if ( i == 5 ) c = IDC_MODULE_NAME_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) ) {
				if ( sp >= BM1 ) {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( sp + i ) )
						sprintf( Buffer , "BM%d-(F/%d)" , sp + i - BM1 + 1 , _i_SCH_PRM_GET_MODULE_SIZE( sp + i ) );
					else
						sprintf( Buffer , "BM%d-(I/%d)" , sp + i - BM1 + 1 , _i_SCH_PRM_GET_MODULE_SIZE( sp + i ) );
					if ( EQUIP_INTERFACE_FUNCTION_CONNECT( sp + i ) ) {
						KWIN_Item_String_Display( hWnd , c , Buffer );
					}
					else {
						KWIN_Item_String_Display( hWnd , c , Buffer );
						KWIN_Item_Disable( hWnd , c );
					}
				}
				else {
					if ( strlen( _i_SCH_PRM_GET_MODULE_PROCESS_NAME( sp + i ) ) <= 0 ) {
						if ( _i_SCH_PRM_GET_MODULE_SIZE( sp + i ) > 1 )
							sprintf( Buffer , "PM%d(%d)" , sp + i - PM1 + 1 , _i_SCH_PRM_GET_MODULE_SIZE( sp + i ) );
						else
							sprintf( Buffer , "PM%d" , sp + i - PM1 + 1 );
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_SIZE( sp + i ) > 1 )
							sprintf( Buffer , "P%d:%s(%d)" , sp + i - PM1 + 1 , _i_SCH_PRM_GET_MODULE_PROCESS_NAME( sp + i ) , _i_SCH_PRM_GET_MODULE_SIZE( sp + i ) );
						else
							sprintf( Buffer , "P%d:%s" , sp + i - PM1 + 1 , _i_SCH_PRM_GET_MODULE_PROCESS_NAME( sp + i ) );
					}
					if ( EQUIP_INTERFACE_FUNCTION_CONNECT( sp + i ) ) {
						KWIN_Item_String_Display( hWnd , c , Buffer );
					}
					else {
						KWIN_Item_String_Display( hWnd , c , Buffer );
						KWIN_Item_Disable( hWnd , c );
					}
				}
			}
			else {
				KWIN_Item_Hide( hWnd , c );
			}
		}
		//
		if      ( i == 0 ) c = IDC_MODULE_REMLOC_P1;
		else if ( i == 1 ) c = IDC_MODULE_REMLOC_P2;
		else if ( i == 2 ) c = IDC_MODULE_REMLOC_P3;
		else if ( i == 3 ) c = IDC_MODULE_REMLOC_P4;
		else if ( i == 4 ) c = IDC_MODULE_REMLOC_P5;
		else if ( i == 5 ) c = IDC_MODULE_REMLOC_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) )	{
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_MODULE_SERVICE_MODE( sp + i ) , "Local|Remote" , "????" ); // 2007.07.24
				//=============================================================================
			}
			else {
				KWIN_Item_Hide( hWnd , c );
			}
		}
		//
		if      ( i == 0 ) c = IDC_GET_PRIORITY_P1;
		else if ( i == 1 ) c = IDC_GET_PRIORITY_P2;
		else if ( i == 2 ) c = IDC_GET_PRIORITY_P3;
		else if ( i == 3 ) c = IDC_GET_PRIORITY_P4;
		else if ( i == 4 ) c = IDC_GET_PRIORITY_P5;
		else if ( i == 5 ) c = IDC_GET_PRIORITY_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) )
				KWIN_Item_Int_Display( hWnd , c , _i_SCH_PRM_GET_Getting_Priority( sp + i ) );
			else
				KWIN_Item_Hide( hWnd , c );
		}
		//
		if      ( i == 0 ) c = IDC_PUT_PRIORITY_P1;
		else if ( i == 1 ) c = IDC_PUT_PRIORITY_P2;
		else if ( i == 2 ) c = IDC_PUT_PRIORITY_P3;
		else if ( i == 3 ) c = IDC_PUT_PRIORITY_P4;
		else if ( i == 4 ) c = IDC_PUT_PRIORITY_P5;
		else if ( i == 5 ) c = IDC_PUT_PRIORITY_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) )
				KWIN_Item_Int_Display( hWnd , c , _i_SCH_PRM_GET_Putting_Priority( sp + i ) );
			else
				KWIN_Item_Hide( hWnd , c );
		}
		//
		if      ( i == 0 ) c = IDC_SW_CONTROL_P1;
		else if ( i == 1 ) c = IDC_SW_CONTROL_P2;
		else if ( i == 2 ) c = IDC_SW_CONTROL_P3;
		else if ( i == 3 ) c = IDC_SW_CONTROL_P4;
		else if ( i == 4 ) c = IDC_SW_CONTROL_P5;
		else if ( i == 5 ) c = IDC_SW_CONTROL_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( sp + i ) ) {
				KWIN_Item_Hide( hWnd , c );
			}
			else {
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_MODULE_MODE_for_SW( sp + i ) , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
				//=============================================================================
			}
		}
		//
		if      ( i == 0 ) c = IDC_MODULE_PICK_P1;
		else if ( i == 1 ) c = IDC_MODULE_PICK_P2;
		else if ( i == 2 ) c = IDC_MODULE_PICK_P3;
		else if ( i == 3 ) c = IDC_MODULE_PICK_P4;
		else if ( i == 4 ) c = IDC_MODULE_PICK_P5;
		else if ( i == 5 ) c = IDC_MODULE_PICK_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) )	{
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_STATION_PICK_HANDLING( sp + i ) , "Synch|Asynch" , "????" ); // 2007.07.24
				//=============================================================================
			}
			else {
				KWIN_Item_Hide( hWnd , c );
			}
		}
		//
		if      ( i == 0 ) c = IDC_MODULE_PLACE_P1;
		else if ( i == 1 ) c = IDC_MODULE_PLACE_P2;
		else if ( i == 2 ) c = IDC_MODULE_PLACE_P3;
		else if ( i == 3 ) c = IDC_MODULE_PLACE_P4;
		else if ( i == 4 ) c = IDC_MODULE_PLACE_P5;
		else if ( i == 5 ) c = IDC_MODULE_PLACE_P6;
		if ( ( i + sp ) > ep ) {
			KWIN_Item_Hide( hWnd , c );
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) )	{
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_STATION_PLACE_HANDLING( sp + i ) , "Synch|Asynch" , "????" ); // 2007.07.24
				//=============================================================================
			}
			else {
				KWIN_Item_Hide( hWnd , c );
			}
		}
	}

	if ( Mode < 100 ) {
		for ( i = 0 ; i < 6 ; i++ ) {
			if      ( i == 0 ) c = IDC_PM_GAP_P1;
			else if ( i == 1 ) c = IDC_PM_GAP_P2;
			else if ( i == 2 ) c = IDC_PM_GAP_P3;
			else if ( i == 3 ) c = IDC_PM_GAP_P4;
			else if ( i == 4 ) c = IDC_PM_GAP_P5;
			else if ( i == 5 ) c = IDC_PM_GAP_P6;
			if ( ( i + sp ) > ep ) {
				KWIN_Item_Hide( hWnd , c );
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_MODE( sp + i ) ) {
					if ( ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 1 ) || ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 3 ) ) {
						if ( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( sp + i ) > 0 ) { // 2006.05.04
							sprintf( Buffer , "%d:%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( sp + i ) , _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( sp + i ) ); // 2006.05.04
						} // 2006.05.04
						else { // 2006.05.04
							sprintf( Buffer , "%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( sp + i ) ); // 2006.05.04
						} // 2006.05.04
						KWIN_Item_String_Display( hWnd , c , Buffer ); // 2006.05.04
					}
					else {
						KWIN_Item_Int_Display( hWnd , c , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( sp + i ) ); // 2006.05.04
					}
				}
				else {
					KWIN_Item_Hide( hWnd , c );
				}
			}
			//
			if      ( i == 0 ) { c = IDC_PROCESS_IN_MODE_P1; c2 = IDC_PROCESS_IN_TIME_P1; }
			else if ( i == 1 ) { c = IDC_PROCESS_IN_MODE_P2; c2 = IDC_PROCESS_IN_TIME_P2; }
			else if ( i == 2 ) { c = IDC_PROCESS_IN_MODE_P3; c2 = IDC_PROCESS_IN_TIME_P3; }
			else if ( i == 3 ) { c = IDC_PROCESS_IN_MODE_P4; c2 = IDC_PROCESS_IN_TIME_P4; }
			else if ( i == 4 ) { c = IDC_PROCESS_IN_MODE_P5; c2 = IDC_PROCESS_IN_TIME_P5; }
			else if ( i == 5 ) { c = IDC_PROCESS_IN_MODE_P6; c2 = IDC_PROCESS_IN_TIME_P6; }
			if ( ( i + sp ) > ep ) {
				KWIN_Item_Hide( hWnd , c );
				KWIN_Item_Hide( hWnd , c2 );
			}
			else {
				if      ( !_i_SCH_PRM_GET_MODULE_MODE( sp + i ) )			{
					KWIN_Item_Hide( hWnd , c );
					KWIN_Item_Hide( hWnd , c2 );
				}
				else {
					//=============================================================================
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_Process_Run_In_Mode( sp + i ) , "Run|Run(A)|Sim|Sim(A)|x" , "????" ); // 2007.07.24
					//=============================================================================
					KWIN_Item_Int_Display( hWnd , c2 , _i_SCH_PRM_GET_Process_Run_In_Time( sp + i ) );
				}
			}
			//
			if      ( i == 0 ) { c = IDC_PROCESS_OUT_MODE_P1; c2 = IDC_PROCESS_OUT_TIME_P1; }
			else if ( i == 1 ) { c = IDC_PROCESS_OUT_MODE_P2; c2 = IDC_PROCESS_OUT_TIME_P2; }
			else if ( i == 2 ) { c = IDC_PROCESS_OUT_MODE_P3; c2 = IDC_PROCESS_OUT_TIME_P3; }
			else if ( i == 3 ) { c = IDC_PROCESS_OUT_MODE_P4; c2 = IDC_PROCESS_OUT_TIME_P4; }
			else if ( i == 4 ) { c = IDC_PROCESS_OUT_MODE_P5; c2 = IDC_PROCESS_OUT_TIME_P5; }
			else if ( i == 5 ) { c = IDC_PROCESS_OUT_MODE_P6; c2 = IDC_PROCESS_OUT_TIME_P6; }
			if ( ( i + sp ) > ep ) {
				KWIN_Item_Hide( hWnd , c );
				KWIN_Item_Hide( hWnd , c2 );
			}
			else {
				if      ( !_i_SCH_PRM_GET_MODULE_MODE( sp + i ) )			{
					KWIN_Item_Hide( hWnd , c );
					KWIN_Item_Hide( hWnd , c2 );
				}
				else {
					//=============================================================================
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , c , _i_SCH_PRM_GET_Process_Run_Out_Mode( sp + i ) , "Run|Run(A)|Sim|Sim(A)|x" , "????" ); // 2007.07.24
					//=============================================================================
					KWIN_Item_Int_Display( hWnd , c2 , _i_SCH_PRM_GET_Process_Run_Out_Time( sp + i ) );
				}
			}
		}
	}
	else {
		KWIN_Item_Hide( hWnd , IDC_PM_GAP_P1 );
		KWIN_Item_Hide( hWnd , IDC_PM_GAP_P2 );
		KWIN_Item_Hide( hWnd , IDC_PM_GAP_P3 );
		KWIN_Item_Hide( hWnd , IDC_PM_GAP_P4 );
		KWIN_Item_Hide( hWnd , IDC_PM_GAP_P5 );
		KWIN_Item_Hide( hWnd , IDC_PM_GAP_P6 );
		//
		for ( i = 0 ; i < 6 ; i++ ) {
			if      ( i == 0 ) { c = IDC_PROCESS_IN_TIME_P1; c2 = IDC_PROCESS_OUT_TIME_P1; }
			else if ( i == 1 ) { c = IDC_PROCESS_IN_TIME_P2; c2 = IDC_PROCESS_OUT_TIME_P2; }
			else if ( i == 2 ) { c = IDC_PROCESS_IN_TIME_P3; c2 = IDC_PROCESS_OUT_TIME_P3; }
			else if ( i == 3 ) { c = IDC_PROCESS_IN_TIME_P4; c2 = IDC_PROCESS_OUT_TIME_P4; }
			else if ( i == 4 ) { c = IDC_PROCESS_IN_TIME_P5; c2 = IDC_PROCESS_OUT_TIME_P5; }
			else if ( i == 5 ) { c = IDC_PROCESS_IN_TIME_P6; c2 = IDC_PROCESS_OUT_TIME_P6; }
			if ( ( i + sp ) > ep ) {
				KWIN_Item_Hide( hWnd , c );
				KWIN_Item_Hide( hWnd , c2 );
			}
			else {
				if ( !_i_SCH_PRM_GET_MODULE_MODE( sp + i ) ) {
					KWIN_Item_Hide( hWnd , c );
					KWIN_Item_Hide( hWnd , c2 );
				}
				else {
					KWIN_Item_Int_Display( hWnd , c , _i_SCH_PRM_GET_Process_Run_In_Time( sp + i ) );
					KWIN_Item_Int_Display( hWnd , c2 , _i_SCH_PRM_GET_Process_Run_Out_Time( sp + i ) );
				}
			}
		}
		KWIN_Item_Hide( hWnd , IDC_PROCESS_IN_MODE_P1 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_IN_MODE_P2 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_IN_MODE_P3 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_IN_MODE_P4 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_IN_MODE_P5 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_IN_MODE_P6 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_OUT_MODE_P1 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_OUT_MODE_P2 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_OUT_MODE_P3 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_OUT_MODE_P4 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_OUT_MODE_P5 );
		KWIN_Item_Hide( hWnd , IDC_PROCESS_OUT_MODE_P6 );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GUI_Make_Temp_for_MFI_USER_OPTION( HWND hWnd ) { // 2014.06.13
	int i , j;
	//
	j = 0;
	//
	for ( i = 1 ; i <= 255 ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( i ) != 2 ) {
			//
			if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( i ) != 3 ) {
				//
				_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTIONF( i , 3 );
				//
				j = 1;
				//
			}
			//
		}
		//
	}
	//
	if ( j == 1 ) {
		KWIN_Item_Hide( hWnd , IDC_RB_BUTTON	);
		KWIN_Item_Show( hWnd , IDC_RB2_BUTTON	);
		KWIN_Item_Show( hWnd , IDC_USEROPTION_BUTTON);
	}
	//
	if ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 0 ) {
		_i_SCH_PRM_SET_PRCS_ZONE_RUN_TYPE( 1 );
	}
	//
}

BOOL APIENTRY Gui_IDD_MODULE_STATUS_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int i , l;
	char *szString[] = { "BM" , "Group" , "Type" , "Slot" , "Mode" , "HS" , "Else" };
	int    szSize[7] = {  37  ,      57 ,    100 ,     50 ,    100 ,   50 ,   300  };
	LPNMHDR lpnmh;
	LV_DISPINFO *lpdi;
	char Buffer[256];
	char Bufferx[12];

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
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 7 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_BM_CHAMBER_DEPTH );
		return TRUE;

	case WM_NOTIFY :
		lpnmh = (LPNMHDR) lParam;
		switch( lpnmh->code ) {
		case LVN_GETDISPINFO :
			lpdi = (LV_DISPINFO *) lParam;
			i = lpdi->item.iItem;
			//
			Buffer[0] = 0; // 2016.02.02
			//
			switch( lpdi->item.iSubItem ) {
			case 0 :
				sprintf( Buffer , "%d" , i+1 );
				break;
			case 1 :
				if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) )	{
					sprintf( Buffer , "%d" , _i_SCH_PRM_GET_MODULE_GROUP(i+BM1) );
				}
				else {
					sprintf( Buffer , "" );
				}
				break;
			case 2 :
				if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) )	{
					if      ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == i + BM1 ) {
						sprintf( Buffer , "[DUMMY]" );
					}
					else if ( _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == i + BM1 ) {
						sprintf( Buffer , "[ALBUFFER]" );
					}
					else {
						switch( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( _i_SCH_PRM_GET_MODULE_GROUP(i+BM1) , i+BM1 ) ) {
						case BUFFER_IN_MODE			:	sprintf( Buffer , "IN" ); break;
						case BUFFER_OUT_MODE		:	sprintf( Buffer , "OUT" ); break;
						case BUFFER_SWITCH_MODE		:	sprintf( Buffer , "SWITCH" ); break;
						case BUFFER_IN_S_MODE		:	sprintf( Buffer , "S:IN" ); break;
						case BUFFER_OUT_S_MODE		:	sprintf( Buffer , "S:OUT" ); break;
						case BUFFER_OUT_CASSETTE	:	sprintf( Buffer , "CASSETTE" ); break;
						default						:	sprintf( Buffer , "Unknown" ); break;
						}
					}
				}
				else {
					sprintf( Buffer , "" );
				}
				break;
			case 3 :
				if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) )	{
					sprintf( Buffer , "%d" , _i_SCH_PRM_GET_MODULE_SIZE(i+BM1) );
				}
				else {
					sprintf( Buffer , "" );
				}
				break;
			case 4 :
				if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) )	{
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN(i+BM1) ) {
						strcpy( Buffer , "" );
						for ( l = 0 ; l < MAX_TM_CHAMBER_DEPTH ; l++ ) {
							if ( l != 0 ) strcat( Buffer , "," );
							switch( _i_SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( l ) ) {
							case BUFFER_SWITCH_BATCH_ALL	:	sprintf( Bufferx , "Full" ); break;
							case BUFFER_SWITCH_BATCH_PART	:	sprintf( Bufferx , "Full(BP)" ); break;
							case BUFFER_SWITCH_SINGLESWAP	:	sprintf( Bufferx , "Full(1S)" ); break;
							case BUFFER_SWITCH_FULLSWAP		:	sprintf( Bufferx , "Full(FS)" ); break;
							case BUFFER_SWITCH_MIDDLESWAP	:	sprintf( Bufferx , "Full(MS)" ); break;
							default							:	sprintf( Bufferx , "%02d" , _i_SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( l ) ); break;
							}
							strcat( Buffer , Bufferx );
						}
					}
					else {
						sprintf( Buffer , "Immediately" );
					}
				}
				else {
					sprintf( Buffer , "" );
				}
				break;

			case 5 :
				if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) )	{
// 2008.04.07
//					switch( _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( i+BM1 ) ) {
//					case GROUP_HAS_SWITCH_NONE : sprintf( Buffer , "" ); break;
//					case GROUP_HAS_SWITCH_INS  : sprintf( Buffer , "I" ); break;
//					case GROUP_HAS_SWITCH_OUTS : sprintf( Buffer , "O" ); break;
//					case GROUP_HAS_SWITCH_ALL  : sprintf( Buffer , "A" ); break;
//					default					   : sprintf( Buffer , "?" ); break;
//					}
					if ( i < MAX_TM_CHAMBER_DEPTH ) { // 2008.04.07
						switch( _i_SCH_PRM_GET_MODULE_GROUP_HAS_SWITCH( i ) ) {
						case GROUP_HAS_SWITCH_NONE : sprintf( Buffer , "" ); break;
						case GROUP_HAS_SWITCH_INS  : sprintf( Buffer , "I" ); break;
						case GROUP_HAS_SWITCH_OUTS : sprintf( Buffer , "O" ); break;
						case GROUP_HAS_SWITCH_ALL  : sprintf( Buffer , "A" ); break;
						default					   : sprintf( Buffer , "?" ); break;
						}
					}
					else {
						sprintf( Buffer , "" );
					}
				}
				else {
					sprintf( Buffer , "" );
				}
				break;

			case 6 :
				if  ( _i_SCH_PRM_GET_MODULE_MODE(i+BM1) )	{
					strcpy( Buffer , "" );
					for ( l = 0 ; l < MAX_TM_CHAMBER_DEPTH ; l++ ) {
						if ( l != 0 ) strcat( Buffer , "," );
						switch( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( l , i+BM1 ) ) {
						case BUFFER_IN_MODE			:	sprintf( Bufferx , "IN" ); break;
						case BUFFER_OUT_MODE		:	sprintf( Bufferx , "OU" ); break;
						case BUFFER_SWITCH_MODE		:	sprintf( Bufferx , "SW" ); break;
						case BUFFER_IN_S_MODE		:	sprintf( Bufferx , "SI" ); break;
						case BUFFER_OUT_S_MODE		:	sprintf( Bufferx , "SO" ); break;
						case BUFFER_OUT_CASSETTE	:	sprintf( Bufferx , "CS" ); break;
						default						:	sprintf( Bufferx , "%02d" , _i_SCH_PRM_GET_MODULE_BUFFER_MODE( l , i+BM1 ) ); break;
						}
						strcat( Buffer , Bufferx );
					}
				}
				else {
					sprintf( Buffer , "" );
				}
				break;
			}
//			lstrcpy( lpdi->item.pszText , Buffer ); //
			//
			strcpy( lpdi->item.pszText , Buffer ); // 2016.02.02
			//
			return TRUE;

		case NM_DBLCLK :
			return TRUE;
		case LVN_ITEMCHANGED :
			return TRUE;
		case LVN_KEYDOWN :
			return TRUE;
		}
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
			//
		case IDOK : // 2014.06.13
			//
			if ( IDYES == MessageBox( hdlg , "Do you want to Show the MFI Option Dialog & Process Wait Multi Zone" , "MFI Option Dialog & Process Wait Multi Zone" , MB_ICONQUESTION | MB_YESNO ) ) {
				GUI_Make_Temp_for_MFI_USER_OPTION( GetParent(hdlg) );
			}
			//
			return TRUE;
			//
		}
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_USEROPTION_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i;
	//
	static int  TEMP_ADDR[256];
	static int  TEMP_DATA[256];
	static int  TEMP_DATA_COUNT;
	//
	static BOOL Control;
	BOOL modify;
	//
	char *szString[] = { "No" , "Name" , "Data" , "Fix" };
	//
	int    szSize[4] = {  50  ,    200 ,    50 ,    45 };

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
		//------------------------------------------------------------
		TEMP_DATA_COUNT = 0;
		//
		for ( i = 1 ; i <= 255 ; i++ ) {
			//
			if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( i ) == 0 ) continue;
			//
			TEMP_ADDR[TEMP_DATA_COUNT] = i;
			TEMP_DATA[TEMP_DATA_COUNT] = _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( i );
			TEMP_DATA_COUNT++;
			//
		}
		//------------------------------------------------------------
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 4 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//-----------------------------------------------------------------------------------------------------------
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , TEMP_DATA_COUNT );
		//
		return TRUE;

	case WM_NOTIFY:	{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			int i , Res;
			switch( lpnmh->code ) {
			case LVN_GETDISPINFO:	{
					LV_DISPINFO *lpdi = (LV_DISPINFO *) lParam;
					TCHAR szString[ 256 ];
					//
					szString[0] = 0; // 2016.02.02
					//
					if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
						//
						if ( lpdi->item.iSubItem == 0 ) { // no
							wsprintf( szString, "%d" , TEMP_ADDR[lpdi->item.iItem] );
						}
						else if ( lpdi->item.iSubItem == 1 ) { // Name
							if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONS( TEMP_ADDR[lpdi->item.iItem] ) == NULL ) {
								wsprintf( szString, "ITEM%d" , TEMP_ADDR[lpdi->item.iItem] );
							}
							else {
								wsprintf( szString, "%s" , _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONS( TEMP_ADDR[lpdi->item.iItem] ) );
							}
						}
						else if ( lpdi->item.iSubItem == 2 ) { // Data
							wsprintf( szString, "%d" , TEMP_DATA[lpdi->item.iItem] );
						}
						else if ( lpdi->item.iSubItem == 3 ) { // Fix
							if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( TEMP_ADDR[lpdi->item.iItem] ) == 1 ) {
								wsprintf( szString, "O" );
							}
							else if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( TEMP_ADDR[lpdi->item.iItem] ) == 3 ) { // 2014.06.13
								wsprintf( szString, "T" );
							}
							else {
								wsprintf( szString, "" );
							}
						}
						//
//						lstrcpy( lpdi->item.pszText , szString ); // 2016.02.02
						//
					}
					//
					strcpy( lpdi->item.pszText , szString ); // 2016.02.02
					//
				}
				return 0;

			case NM_DBLCLK :
				if ( Control ) return TRUE;
				if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) ) {
					//
					i = ListView_GetNextItem( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , -1 , LVNI_SELECTED );
					//
					if ( i < 0 ) return 0;
					//
					if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( TEMP_ADDR[i] ) != 2 ) {
						if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( TEMP_ADDR[i] ) != 3 ) { // 2014.06.13
							return 0;
						}
					}
					//
					Res = TEMP_DATA[i];
					//
					if ( MODAL_DIGITAL_BOX2( hdlg , "Select User Option Value" , "Select" , -9999 , 9999 , &Res ) ) {
						TEMP_DATA[i] = Res;
						InvalidateRect( hdlg , NULL , FALSE );
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
		//
		modify = FALSE;
		//
		for ( i = 0 ; i < TEMP_DATA_COUNT ; i++ ) {
			//
			if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTION( TEMP_ADDR[i] ) != TEMP_DATA[i] ) modify = TRUE;
			//
		}
		//
		if ( modify ) {
			KWIN_Item_Enable( hdlg , IDOK );
		}
		else {
			KWIN_Item_Hide( hdlg , IDOK );
		}
		//
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDOK :
			//
			if ( Control ) return TRUE;
			//
			for ( i = 0 ; i < TEMP_DATA_COUNT ; i++ ) {
				//
				_i_SCH_PRM_SET_MFI_INTERFACE_FLOW_USER_OPTION( TEMP_ADDR[i] , TEMP_DATA[i] );
				//
			}
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//
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

BOOL GUI_Parameter_Control( HWND hWnd , long msg ) {
	int Res , c , c2 , msg2 , mm , mc;
	BOOL Change = FALSE;
//	char Buffer[256]; // 2018.09.28
	char Buffer[1024]; // 2018.09.28
	char Buffer2[32];

	//=================================================================================
	//=================================================================================
	for ( c = 0 ; c < MAX_CASSETTE_SIDE + 1 ; c++ ) {
		if ( _i_SCH_SYSTEM_USING_GET( c ) > 0 ) Change = TRUE;
	}

	if ( msg == IDC_SCH_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_SCHEDULER_PAD ) , hWnd , Gui_IDD_STATUS_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_FIXCLST_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_FIXEDRUN_PAD ) , hWnd , Gui_IDD_FIXEDCLST_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_RERCP_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_RERCP_PAD ) , hWnd , Gui_IDD_RERCP_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_FAIL_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_FAILSNR_PAD ) , hWnd , Gui_IDD_FAILSNR_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_CLSSNR_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CLSSNR_PAD ) , hWnd , Gui_IDD_CLSSNR_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_UTILITY_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_UTILITY_PAD ) , hWnd , Gui_IDD_SYSTEM_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_RCPFILE_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_RCPFILE_PAD ) , hWnd , Gui_IDD_RCPFILE_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_MODULE_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_MODULE_PAD ) , hWnd , Gui_IDD_MODULE_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_HANDLINGSIDE_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_HANDLINGSIDE_PAD ) , hWnd , Gui_IDD_HANDLINGSIDE_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10 // 2006.02.21
		return TRUE;
	}
	if ( msg == IDC_INTERLOCK_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CHINTERLOCK_PAD ) , hWnd , Gui_IDD_CHINTERLOCK_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_FA_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_FA_PAD ) , hWnd , Gui_IDD_FA_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_CPJOB_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CPJOB_PAD ) , hWnd , Gui_IDD_CPJOB_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	if ( msg == IDC_RBDATA_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_ROBOTDATA_PAD ) , hWnd , Gui_IDD_ROBOTDATA_PAD_Proc , (LPARAM) (BOOL) Change ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}
	//
	if ( msg == IDC_USEROPTION_BUTTON ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_USEROPTION_PAD ) , hWnd , Gui_IDD_USEROPTION_PAD_Proc , (LPARAM) (BOOL) Change );
		return TRUE;
	}
	//
	if ( msg == IDC_MODULE_DISPLAY ) {
		strcpy( Buffer , "Module" );
		//================================================================
		mm = MAX_PM_CHAMBER_DEPTH;
		for ( c = MAX_PM_CHAMBER_DEPTH - 1 ; c >= 0 ; c-- ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( c + PM1 ) ) {
				mm = c + 1;
				break;
			}
		}
		//================================================================
		// 2010.10.07
		mc = MAX_CASSETTE_SIDE;
		for ( c = MAX_CASSETTE_SIDE - 1 ; c >= 0 ; c-- ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( c + CM1 ) ) {
				mc = c + 1;
				break;
			}
		}
		mc = ( ( ( mc - 1 ) / 4 ) + 1 ) * 6;
		if ( mc > mm ) mm = mc;
		//================================================================
		c2 = 1;
		for ( c = 6 ; c < mm ; ) {
			if ( ( c + 6 ) > mm )
				sprintf( Buffer2 , "Module(P%d-P%d)" , c + 1 , mm );
			else
				sprintf( Buffer2 , "Module(P%d-P%d)" , c + 1 , c + 6 );
			strcat( Buffer , "|" );
			strcat( Buffer , Buffer2 );
			c += 6;
			c2++;
		}
//		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || _i_SCH_PRM_GET_MODULE_BUFFER_USE() ) { // 2008.02.21
			//================================================================
			mm = MAX_BM_CHAMBER_DEPTH;
			for ( c = MAX_BM_CHAMBER_DEPTH - 1 ; c >= 0 ; c-- ) {
				if ( _i_SCH_PRM_GET_MODULE_MODE( c + BM1 ) ) {
					mm = c + 1;
					break;
				}
			}
			//================================================================
			for ( c = 0 ; c < mm ; ) {
				if ( ( c + 6 ) > mm ) {
					if ( c == 0 ) {
						sprintf( Buffer2 , "Buffer" );
					}
					else {
						sprintf( Buffer2 , "Buffer(B%d-B%d)" , c + 1 , mm );
					}
				}
				else {
					sprintf( Buffer2 , "Buffer(B%d-B%d)" , c + 1 , c + 6 );
				}
				strcat( Buffer , "|" );
				strcat( Buffer , Buffer2 );
				c += 6;
			}
//		} // 2008.02.21
		//
		Res = GUI_MD_DISPLAY_GET();
		if ( Res >= 100 ) Res = c2 + Res - 100;
		if ( MODAL_DIGITAL_BOX1( hWnd , "Module" , "Select" , Buffer , &Res ) ) {
			if ( ( Res >= c2 ) || ( Res != GUI_MD_DISPLAY_GET() ) ) {
				if ( Res >= c2 ) GUI_MD_DISPLAY_SET( Res - c2 + 100 );
				else             GUI_MD_DISPLAY_SET( Res );
				if ( GUI_MD_DISPLAY_GET() == 0 ) {
					KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , "Module" );
				}
				else if ( GUI_MD_DISPLAY_GET() == 100 ) {
					KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , "Buffer" );
				}
				else if ( GUI_MD_DISPLAY_GET() > 100 ) {
					c = ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 ) + 1;
					c2 = ( c + 6 ) - 1;
					if ( c2 >= MAX_BM_CHAMBER_DEPTH ) c2 = MAX_BM_CHAMBER_DEPTH;
					sprintf( Buffer , "Buf(%d-%d)" , c , c2 );
					KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , Buffer );
				}
				else {
					c = ( GUI_MD_DISPLAY_GET() * 6 ) + 1;
					c2 = ( c + 6 ) - 1;
					if ( c2 >= MAX_PM_CHAMBER_DEPTH ) c2 = MAX_PM_CHAMBER_DEPTH;
					sprintf( Buffer , "Mdl(%d-%d)" , c , c2 );
					KWIN_Item_String_Display( hWnd , IDC_MODULE_DISPLAY , Buffer );
				}
			}
			InvalidateRect( hWnd , NULL , TRUE );
		}
		return TRUE;
	}

	switch( msg ) {
	case IDC_PROCESS_IN_TIME_P1 :
	case IDC_PROCESS_IN_TIME_P2 :
	case IDC_PROCESS_IN_TIME_P3 :
	case IDC_PROCESS_IN_TIME_P4 :
	case IDC_PROCESS_IN_TIME_P5 :
	case IDC_PROCESS_IN_TIME_P6 :
		if ( GUI_MD_DISPLAY_GET() >= 100 ) {
			if      ( msg == IDC_PROCESS_IN_TIME_P1 ) c = BM1 + 0 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P2 ) c = BM1 + 1 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P3 ) c = BM1 + 2 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P4 ) c = BM1 + 3 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P5 ) c = BM1 + 4 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P6 ) c = BM1 + 5 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			//
			Res = _i_SCH_PRM_GET_Process_Run_In_Time( c );
			if ( MODAL_DIGITAL_BOX2( hWnd , "TMSIDE Time" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 9999 , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_Process_Run_In_Time( c ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Time( c , Res );
					KWIN_Item_Int_Display( hWnd , msg , Res );
					//---------------------------------
					Set_RunPrm_IO_Setting( 2 );
					//---------------------------------
					GUI_SAVE_PARAMETER_DATA( 1 );
				}
			}
			//
		}
		else {
			if      ( msg == IDC_PROCESS_IN_TIME_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_IN_TIME_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
			//
			Res = _i_SCH_PRM_GET_Process_Run_In_Time( c );
			if ( MODAL_DIGITAL_BOX2( hWnd , "Process In Time" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 9999 , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_Process_Run_In_Time( c ) ) {
					_i_SCH_PRM_SET_Process_Run_In_Time( c , Res );
					KWIN_Item_Int_Display( hWnd , msg , Res );
					//---------------------------------
					Set_RunPrm_IO_Setting( 2 );
					//---------------------------------
					GUI_SAVE_PARAMETER_DATA( 1 );
				}
			}
			//
		}
		return TRUE;
		break;
	case IDC_PROCESS_OUT_TIME_P1 :
	case IDC_PROCESS_OUT_TIME_P2 :
	case IDC_PROCESS_OUT_TIME_P3 :
	case IDC_PROCESS_OUT_TIME_P4 :
	case IDC_PROCESS_OUT_TIME_P5 :
	case IDC_PROCESS_OUT_TIME_P6 :
		if ( GUI_MD_DISPLAY_GET() >= 100 ) {
			if      ( msg == IDC_PROCESS_OUT_TIME_P1 ) c = BM1 + 0 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P2 ) c = BM1 + 1 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P3 ) c = BM1 + 2 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P4 ) c = BM1 + 3 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P5 ) c = BM1 + 4 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P6 ) c = BM1 + 5 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			//
			Res = _i_SCH_PRM_GET_Process_Run_Out_Time( c );
			if ( MODAL_DIGITAL_BOX2( hWnd , "FMSIDE Time" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 9999 , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_Process_Run_Out_Time( c ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Time( c , Res );
					KWIN_Item_Int_Display( hWnd , msg , Res );
					//---------------------------------
					Set_RunPrm_IO_Setting( 3 );
					//---------------------------------
					GUI_SAVE_PARAMETER_DATA( 1 );
				}
			}
			//
		}
		else {
			if      ( msg == IDC_PROCESS_OUT_TIME_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PROCESS_OUT_TIME_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
			//
			Res = _i_SCH_PRM_GET_Process_Run_Out_Time( c );
			if ( MODAL_DIGITAL_BOX2( hWnd , "Process Out Time" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 9999 , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_Process_Run_Out_Time( c ) ) {
					_i_SCH_PRM_SET_Process_Run_Out_Time( c , Res );
					KWIN_Item_Int_Display( hWnd , msg , Res );
					//---------------------------------
					Set_RunPrm_IO_Setting( 3 );
					//---------------------------------
					GUI_SAVE_PARAMETER_DATA( 1 );
				}
			}
			//
		}
		return TRUE;
		break;

	case IDC_GET_PRIORITY_C1 :
	case IDC_GET_PRIORITY_C2 :
	case IDC_GET_PRIORITY_C3 :
	case IDC_GET_PRIORITY_C4 :
	case IDC_GET_PRIORITY_P1 :
	case IDC_GET_PRIORITY_P2 :
	case IDC_GET_PRIORITY_P3 :
	case IDC_GET_PRIORITY_P4 :
	case IDC_GET_PRIORITY_P5 :
	case IDC_GET_PRIORITY_P6 :
		if ( GUI_MD_DISPLAY_GET() >= 100 ) {
			if      ( msg == IDC_GET_PRIORITY_C1 ) break;
			else if ( msg == IDC_GET_PRIORITY_C2 ) break;
			else if ( msg == IDC_GET_PRIORITY_C3 ) break;
			else if ( msg == IDC_GET_PRIORITY_C4 ) break;
			else if ( msg == IDC_GET_PRIORITY_P1 ) c = BM1 + 0 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_GET_PRIORITY_P2 ) c = BM1 + 1 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_GET_PRIORITY_P3 ) c = BM1 + 2 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_GET_PRIORITY_P4 ) c = BM1 + 3 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_GET_PRIORITY_P5 ) c = BM1 + 4 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_GET_PRIORITY_P6 ) c = BM1 + 5 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
		}
		else {
			if ( ( ( GUI_MD_DISPLAY_GET() ) * 4 ) >= MAX_CASSETTE_SIDE ) { // 2006.02.08
				if      ( msg == IDC_GET_PRIORITY_C1 ) c = TM + 0 + ( ( GUI_MD_DISPLAY_GET() - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 );
				else if ( msg == IDC_GET_PRIORITY_C2 ) c = TM + 1 + ( ( GUI_MD_DISPLAY_GET() - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 );
				else if ( msg == IDC_GET_PRIORITY_C3 ) c = TM + 2 + ( ( GUI_MD_DISPLAY_GET() - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 );
				else if ( msg == IDC_GET_PRIORITY_C4 ) c = TM + 3 + ( GUI_MD_DISPLAY_GET() * 4 );
				else if ( msg == IDC_GET_PRIORITY_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
			}
			else {
				if      ( msg == IDC_GET_PRIORITY_C1 ) c = CM1 + 0 + ( GUI_MD_DISPLAY_GET() * 4 );
				else if ( msg == IDC_GET_PRIORITY_C2 ) c = CM1 + 1 + ( GUI_MD_DISPLAY_GET() * 4 );
				else if ( msg == IDC_GET_PRIORITY_C3 ) c = CM1 + 2 + ( GUI_MD_DISPLAY_GET() * 4 );
				else if ( msg == IDC_GET_PRIORITY_C4 ) c = CM1 + 3 + ( GUI_MD_DISPLAY_GET() * 4 );
				else if ( msg == IDC_GET_PRIORITY_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
				else if ( msg == IDC_GET_PRIORITY_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
			}
		}
		Res = _i_SCH_PRM_GET_Getting_Priority( c );
//		if ( MODAL_DIGITAL_BOX2( hWnd , "Get Priority" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 99999 , &Res ) ) { // 2007.01.02
		if ( MODAL_DIGITAL_BOX2( hWnd , "Get Priority" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , -999 , 99999 , &Res ) ) { // 2007.01.02
			if ( Res != _i_SCH_PRM_GET_Getting_Priority( c ) ) {
				_i_SCH_PRM_SET_Getting_Priority( c , Res );
				KWIN_Item_Int_Display( hWnd , msg , _i_SCH_PRM_GET_Getting_Priority( c ) );
				//---------------------------------
				GUI_SAVE_PARAMETER_DATA( 1 );
				//---------------------------------
			}
		}
		return TRUE;
		break;

	case IDC_PUT_PRIORITY_C1 :
	case IDC_PUT_PRIORITY_C2 :
	case IDC_PUT_PRIORITY_C3 :
	case IDC_PUT_PRIORITY_C4 :
	case IDC_PUT_PRIORITY_P1 :
	case IDC_PUT_PRIORITY_P2 :
	case IDC_PUT_PRIORITY_P3 :
	case IDC_PUT_PRIORITY_P4 :
	case IDC_PUT_PRIORITY_P5 :
	case IDC_PUT_PRIORITY_P6 :
		if ( GUI_MD_DISPLAY_GET() >= 100 ) {
			if      ( msg == IDC_PUT_PRIORITY_C1 ) break;
			else if ( msg == IDC_PUT_PRIORITY_C2 ) break;
			else if ( msg == IDC_PUT_PRIORITY_C3 ) break;
			else if ( msg == IDC_PUT_PRIORITY_C4 ) break;
			else if ( msg == IDC_PUT_PRIORITY_P1 ) c = BM1 + 0 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P2 ) c = BM1 + 1 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P3 ) c = BM1 + 2 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P4 ) c = BM1 + 3 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P5 ) c = BM1 + 4 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P6 ) c = BM1 + 5 + ( (GUI_MD_DISPLAY_GET()-100) * 6 );
		}
		else {
			if      ( msg == IDC_PUT_PRIORITY_C1 ) c = CM1 + 0 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_PUT_PRIORITY_C2 ) c = CM1 + 1 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_PUT_PRIORITY_C3 ) c = CM1 + 2 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_PUT_PRIORITY_C4 ) c = CM1 + 3 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_PUT_PRIORITY_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_PUT_PRIORITY_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
		}
		Res = _i_SCH_PRM_GET_Putting_Priority( c );
//		if ( MODAL_DIGITAL_BOX2( hWnd , "Put Priority" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 99999 , &Res ) ) { // 2007.01.02
		if ( MODAL_DIGITAL_BOX2( hWnd , "Put Priority" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , -999 , 99999 , &Res ) ) { // 2007.01.02
			if ( Res != _i_SCH_PRM_GET_Putting_Priority( c ) ) {
				_i_SCH_PRM_SET_Putting_Priority( c , Res );
				KWIN_Item_Int_Display( hWnd , msg , _i_SCH_PRM_GET_Putting_Priority( c ) );
				//---------------------------------
				GUI_SAVE_PARAMETER_DATA( 1 );
				//---------------------------------
			}
		}
		return TRUE;
		break;

	case IDC_PM_GAP_P1 :
	case IDC_PM_GAP_P2 :
	case IDC_PM_GAP_P3 :
	case IDC_PM_GAP_P4 :
	case IDC_PM_GAP_P5 :
	case IDC_PM_GAP_P6 :
		if      ( msg == IDC_PM_GAP_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
		else if ( msg == IDC_PM_GAP_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
		else if ( msg == IDC_PM_GAP_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
		else if ( msg == IDC_PM_GAP_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
		else if ( msg == IDC_PM_GAP_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
		else if ( msg == IDC_PM_GAP_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
		Res = _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c );
		if ( MODAL_DIGITAL_BOX2( hWnd , "Process Run Inside Zone Time" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 99999 , &Res ) ) {
			if ( Res != _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c ) ) {
				_i_SCH_PRM_SET_PRCS_TIME_WAIT_RANGE( c , Res );
				if ( ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 1 ) || ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 3 ) ) {
					if ( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( c ) > 0 ) { // 2006.05.04
						sprintf( Buffer , "%d:%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c ) , _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( c ) ); // 2006.05.04
					} // 2006.05.04
					else { // 2006.05.04
						sprintf( Buffer , "%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c ) ); // 2006.05.04
					} // 2006.05.04
					KWIN_Item_String_Display( hWnd , msg , Buffer ); // 2006.05.04
				}
				else {
					KWIN_Item_Int_Display( hWnd , msg , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c ) ); // 2006.05.04
				}
				//---------------------------------
				GUI_SAVE_PARAMETER_DATA( 1 );
				//---------------------------------
			}
		}
		else { // 2006.05.04
			if ( ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 1 ) || ( _i_SCH_PRM_GET_PRCS_ZONE_RUN_TYPE() == 3 ) ) {
				Res = _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( c );
				if ( MODAL_DIGITAL_BOX2( hWnd , "Process Run Remain Zone Time" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , 0 , 99999 , &Res ) ) {
					if ( Res != _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( c ) ) {
						_i_SCH_PRM_SET_PRCS_TIME_REMAIN_RANGE( c , Res );
						if ( _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( c ) > 0 ) {
							sprintf( Buffer , "%d:%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c ) , _i_SCH_PRM_GET_PRCS_TIME_REMAIN_RANGE( c ) );
						}
						else {
							sprintf( Buffer , "%d" , _i_SCH_PRM_GET_PRCS_TIME_WAIT_RANGE( c ) );
						}
						KWIN_Item_String_Display( hWnd , msg , Buffer );
						//---------------------------------
						GUI_SAVE_PARAMETER_DATA( 1 );
						//---------------------------------
					}
				}
			}
		}
		return TRUE;
		break;
	}

	if ( Change ) {
		MessageBeep(MB_ICONHAND);
		return TRUE;
	}
	if ( ( msg == IDC_RB_BUTTON ) || ( msg == IDC_RB2_BUTTON ) ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_ROBOTCAL_PAD ) , hWnd , Gui_IDD_ROBOTCAL_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
		return TRUE;
	}

	Change = FALSE;

	switch( msg ) {
	case IDC_AUTO_HANDOFF_C1 :
	case IDC_AUTO_HANDOFF_C2 :
	case IDC_AUTO_HANDOFF_C3 :
	case IDC_AUTO_HANDOFF_C4 :
		if      ( msg == IDC_AUTO_HANDOFF_C1 ) c = CM1 + 0 + ( GUI_MD_DISPLAY_GET() * 4 );
		else if ( msg == IDC_AUTO_HANDOFF_C2 ) c = CM1 + 1 + ( GUI_MD_DISPLAY_GET() * 4 );
		else if ( msg == IDC_AUTO_HANDOFF_C3 ) c = CM1 + 2 + ( GUI_MD_DISPLAY_GET() * 4 );
		else if ( msg == IDC_AUTO_HANDOFF_C4 ) c = CM1 + 3 + ( GUI_MD_DISPLAY_GET() * 4 );
		Res = _i_SCH_PRM_GET_AUTO_HANDOFF( c );
		if ( MODAL_DIGITAL_BOX1( hWnd , "Cassette Auto Handoff Control" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , "None|Load Hadoff|Unload Handoff|Always(Load/Unload) Handoff|Unload(Check)|Load & Unload(Check)" , &Res ) ) {
			if ( Res != _i_SCH_PRM_GET_AUTO_HANDOFF( c ) ) {
				Change = TRUE;
				_i_SCH_PRM_SET_AUTO_HANDOFF( c , Res );
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , msg , _i_SCH_PRM_GET_AUTO_HANDOFF( c ) , ".|Load|Unload|Load/Unload|Unld(Chk)|Ld/Unld(Chk)" , "????" ); // 2007.07.24
				//=============================================================================
				//---------------------------------
				Set_RunPrm_IO_Setting( 1 );
				//---------------------------------
			}
		}
		break;

	case IDC_CASS_MAP_C1 :
	case IDC_CASS_MAP_C2 :
	case IDC_CASS_MAP_C3 :
	case IDC_CASS_MAP_C4 :
		if      ( msg == IDC_CASS_MAP_C1 ) c = CM1 + 0 + ( GUI_MD_DISPLAY_GET() * 4 );
		else if ( msg == IDC_CASS_MAP_C2 ) c = CM1 + 1 + ( GUI_MD_DISPLAY_GET() * 4 );
		else if ( msg == IDC_CASS_MAP_C3 ) c = CM1 + 2 + ( GUI_MD_DISPLAY_GET() * 4 );
		else if ( msg == IDC_CASS_MAP_C4 ) c = CM1 + 3 + ( GUI_MD_DISPLAY_GET() * 4 );
		Res = _i_SCH_PRM_GET_MAPPING_SKIP( c );
		if ( MODAL_DIGITAL_BOX1( hWnd , "Cassette Mapping Control" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , "Run(Mapping)|Skip|MapAll(In/Out)|MapEnd & Check|MapAll & Check|MapEnd Only|Skip(+User:MapIn)|MapEnd&Check(+User:MapIn)|MapEnd Only(+User:MapIn)" , &Res ) ) {
			if ( Res != _i_SCH_PRM_GET_MAPPING_SKIP( c ) ) {
				Change = TRUE;
				_i_SCH_PRM_SET_MAPPING_SKIP( c , Res );
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , msg  , _i_SCH_PRM_GET_MAPPING_SKIP( c ) , "Mapping|Skip|MapAll|MapEndChk|MapAllChk|MapEnd|Skip(U.M)|MECk(U.M)|MEnd(U.M)" , "????" ); // 2007.07.24
				//=============================================================================
				//---------------------------------
				Set_RunPrm_IO_Setting( 0 );
				//---------------------------------
			}
		}
		break;
	case IDC_PROCESS_IN_MODE_P1 :
	case IDC_PROCESS_IN_MODE_P2 :
	case IDC_PROCESS_IN_MODE_P3 :
	case IDC_PROCESS_IN_MODE_P4 :
	case IDC_PROCESS_IN_MODE_P5 :
	case IDC_PROCESS_IN_MODE_P6 :
		if      ( msg == IDC_PROCESS_IN_MODE_P1 ) { c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_IN_TIME_P1; }
		else if ( msg == IDC_PROCESS_IN_MODE_P2 ) { c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_IN_TIME_P2; }
		else if ( msg == IDC_PROCESS_IN_MODE_P3 ) { c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_IN_TIME_P3; }
		else if ( msg == IDC_PROCESS_IN_MODE_P4 ) { c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_IN_TIME_P4; }
		else if ( msg == IDC_PROCESS_IN_MODE_P5 ) { c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_IN_TIME_P5; }
		else if ( msg == IDC_PROCESS_IN_MODE_P6 ) { c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_IN_TIME_P6; }
		Res = _i_SCH_PRM_GET_Process_Run_In_Mode( c );
		if ( MODAL_DIGITAL_BOX1( hWnd , "Process In Mode" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , "Run|Run(Always)|Simulate|Simulate(Always)|NotUse" , &Res ) ) {
			if ( Res != _i_SCH_PRM_GET_Process_Run_In_Mode( c ) ) {
				Change = TRUE;
				_i_SCH_PRM_SET_Process_Run_In_Mode( c , Res );
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , msg , Res , "Run|Run(A)|Sim|Sim(A)|x" , "????" ); // 2007.07.24
				//=============================================================================
				KWIN_Item_Int_Display( hWnd , msg2 , _i_SCH_PRM_GET_Process_Run_In_Time(c) );
			}
		}
		break;

	case IDC_PROCESS_OUT_MODE_P1 :
	case IDC_PROCESS_OUT_MODE_P2 :
	case IDC_PROCESS_OUT_MODE_P3 :
	case IDC_PROCESS_OUT_MODE_P4 :
	case IDC_PROCESS_OUT_MODE_P5 :
	case IDC_PROCESS_OUT_MODE_P6 :
		if      ( msg == IDC_PROCESS_OUT_MODE_P1 ) { c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_OUT_TIME_P1; }
		else if ( msg == IDC_PROCESS_OUT_MODE_P2 ) { c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_OUT_TIME_P2; }
		else if ( msg == IDC_PROCESS_OUT_MODE_P3 ) { c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_OUT_TIME_P3; }
		else if ( msg == IDC_PROCESS_OUT_MODE_P4 ) { c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_OUT_TIME_P4; }
		else if ( msg == IDC_PROCESS_OUT_MODE_P5 ) { c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_OUT_TIME_P5; }
		else if ( msg == IDC_PROCESS_OUT_MODE_P6 ) { c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 ); msg2 = IDC_PROCESS_OUT_TIME_P6; }
		Res = _i_SCH_PRM_GET_Process_Run_Out_Mode( c );
		if ( MODAL_DIGITAL_BOX1( hWnd , "Process Out Mode" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , "Run|Run(Always)|Simulate|Simulate(Always)|NotUse" , &Res ) ) {
			if ( Res != _i_SCH_PRM_GET_Process_Run_Out_Mode( c ) ) {
				Change = TRUE;
				_i_SCH_PRM_SET_Process_Run_Out_Mode( c , Res );
				//=============================================================================
				KWIN_GUI_INT_STRING_DISPLAY( hWnd , msg , Res , "Run|Run(A)|Sim|Sim(A)|x" , "????" ); // 2007.07.24
				//=============================================================================
				KWIN_Item_Int_Display( hWnd , msg2 , _i_SCH_PRM_GET_Process_Run_Out_Time(c) );
			}
		}
		break;

	case IDC_SW_CONTROL_C1 :
	case IDC_SW_CONTROL_C2 :
	case IDC_SW_CONTROL_C3 :
	case IDC_SW_CONTROL_C4 :
	case IDC_SW_CONTROL_P1 :
	case IDC_SW_CONTROL_P2 :
	case IDC_SW_CONTROL_P3 :
	case IDC_SW_CONTROL_P4 :
	case IDC_SW_CONTROL_P5 :
	case IDC_SW_CONTROL_P6 :
	case IDC_SW_CONTROL_AL :
	case IDC_SW_CONTROL_IC :
		if ( GUI_MD_DISPLAY_GET() >= 100 ) {
			if      ( msg == IDC_SW_CONTROL_C1 ) c = TM  + 0 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 4 );
			else if ( msg == IDC_SW_CONTROL_C2 ) c = TM  + 1 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 4 );
			else if ( msg == IDC_SW_CONTROL_C3 ) c = TM  + 2 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 4 );
			else if ( msg == IDC_SW_CONTROL_C4 ) c = TM  + 3 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 4 );
			else if ( msg == IDC_SW_CONTROL_P1 ) c = BM1 + 0 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 );
			else if ( msg == IDC_SW_CONTROL_P2 ) c = BM1 + 1 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 );
			else if ( msg == IDC_SW_CONTROL_P3 ) c = BM1 + 2 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 );
			else if ( msg == IDC_SW_CONTROL_P4 ) c = BM1 + 3 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 );
			else if ( msg == IDC_SW_CONTROL_P5 ) c = BM1 + 4 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 );
			else if ( msg == IDC_SW_CONTROL_P6 ) c = BM1 + 5 + ( ( GUI_MD_DISPLAY_GET() - 100 ) * 6 );
			else if ( msg == IDC_SW_CONTROL_AL ) c = F_AL;
			else if ( msg == IDC_SW_CONTROL_IC ) c = F_IC;
		}
		else {
			if      ( msg == IDC_SW_CONTROL_C1 ) c = ( ( GUI_MD_DISPLAY_GET() * 4 ) >= MAX_CASSETTE_SIDE ) ? TM + 0 + ( ( GUI_MD_DISPLAY_GET() - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 ) : CM1 + 0 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_SW_CONTROL_C2 ) c = ( ( GUI_MD_DISPLAY_GET() * 4 ) >= MAX_CASSETTE_SIDE ) ? TM + 1 + ( ( GUI_MD_DISPLAY_GET() - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 ) : CM1 + 1 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_SW_CONTROL_C3 ) c = ( ( GUI_MD_DISPLAY_GET() * 4 ) >= MAX_CASSETTE_SIDE ) ? TM + 2 + ( ( GUI_MD_DISPLAY_GET() - ( MAX_CASSETTE_SIDE / 4 ) ) * 3 ) : CM1 + 2 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_SW_CONTROL_C4 ) c = ( ( GUI_MD_DISPLAY_GET() * 4 ) >= MAX_CASSETTE_SIDE ) ? FM																	   : CM1 + 3 + ( GUI_MD_DISPLAY_GET() * 4 );
			else if ( msg == IDC_SW_CONTROL_P1 ) c = PM1 + 0 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_SW_CONTROL_P2 ) c = PM1 + 1 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_SW_CONTROL_P3 ) c = PM1 + 2 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_SW_CONTROL_P4 ) c = PM1 + 3 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_SW_CONTROL_P5 ) c = PM1 + 4 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_SW_CONTROL_P6 ) c = PM1 + 5 + ( GUI_MD_DISPLAY_GET() * 6 );
			else if ( msg == IDC_SW_CONTROL_AL ) c = AL;
			else if ( msg == IDC_SW_CONTROL_IC ) c = IC;
		}
		switch( _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() ) {
		case 1 :
			Res = _i_SCH_PRM_GET_MODULE_MODE_for_SW( c );
			if ( MODAL_DIGITAL_BOX1( hWnd , "SW Module Control" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , "Disable|Enable" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_MODULE_MODE_for_SW( c ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( c , Res );
					//=============================================================================
					FA_MDL_STATUS_IO_SET( c );
					//=============================================================================
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , msg , Res , "Disable|Enable" , "????" ); // 2007.07.24
					//=============================================================================
				}
			}
			break;
		case 2 :
			Res = _i_SCH_PRM_GET_MODULE_MODE_for_SW( c );
			if ( MODAL_DIGITAL_BOX1( hWnd , "SW Module Control" , _i_SCH_SYSTEM_GET_MODULE_NAME( c ) , "Disable|Enable|Disable(HW)|Enable(PM)" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_MODULE_MODE_for_SW( c ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_MODULE_MODE_for_SW( c , Res );
					//=============================================================================
					FA_MDL_STATUS_IO_SET( c );
					//=============================================================================
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , msg , Res , "Disable|Enable|Disable(HW)|Enable(PM)" , "????" ); // 2007.07.24
					//=============================================================================
				}
			}
			break;
		default :
			MessageBeep(MB_ICONHAND);
			return TRUE;
		}
		break;

	}
	if ( Change ) GUI_SAVE_PARAMETER_DATA( 1 );
	return TRUE;
}
//
void GUI_Parameter_Show( HWND hWnd ) {
	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_MODULE_STATUS_PAD ) , hWnd , Gui_IDD_MODULE_STATUS_PAD_Proc , (LPARAM) NULL ); // 2004.01.19
}

