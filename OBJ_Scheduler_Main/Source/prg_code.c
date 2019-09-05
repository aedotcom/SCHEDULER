#include "default.h"
#include <commctrl.h>

#include "EQ_Enum.h"

#include "system_tag.h"
#include "ioLog.h"
#include "User_Parameter.h"
#include "Gui_Handling.h"
#include "sch_CassetteResult.h"
#include "sch_prm_log.h"
#include "sch_prm_FBTPM.h"
#include "sch_Multi_ALIC.h"
#include "sch_preprcs.h"
#include "sch_sdm.h"
#include "sch_prm.h"
#include "sch_main.h"
#include "sch_processmonitor.h"
#include "sch_util.h"
#include "sch_sub.h"
#include "sch_EIL.h"
#include "Robot_Handling.h"
#include "Robot_Animation.h"
#include "IO_Part_data.h"
#include "Equip_Handling.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "FA_Handling.h"
#include "Multireg.h"
#include "Multijob.h"
#include "File_ReadInit.h"
#include "DLL_Interface.h"
#include "Errorcode.h"
#include "Utility.h"
#include "Version.h"
#include "Sch_TransferMaintInf.h"
#include "sch_CommonUser.h"
#include "Alarm_Analysis.h"
#include "RcpFile_Handling.h"

#include "INF_sch_CommonUser.h"

#include "resource.h"

//---------------------------------------------------------------------------------------
int  USER_SCH_USE_TAG_GET();
//---------------------------------------------------------------------------------------
char COMMON_BLANK_CHAR[2] = { 0 , 0 }; // 2008.04.02
//---------------------------------------------------------------------------------------
CRITICAL_SECTION CR_MAIN;
CRITICAL_SECTION CR_WAIT;
CRITICAL_SECTION CR_MODULE; // 2010.05.27
CRITICAL_SECTION CR_TMRUN[MAX_TM_CHAMBER_DEPTH];
CRITICAL_SECTION CR_FEMRUN;
CRITICAL_SECTION CR_FEM_MAP_PICK_PLACE;
CRITICAL_SECTION CR_HMAPPING[ MAX_CASSETTE_SIDE ];
CRITICAL_SECTION CR_MAINT_INF_TRANSFER; // 2006.08.30
CRITICAL_SECTION CR_PRE_BEGIN_END; // 2006.12.01
CRITICAL_SECTION CR_PRE_BEGIN_ONLY; // 2010.05.21
CRITICAL_SECTION CR_FINISH_CHECK; // 2011.04.19
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};
//------------------------------------------------------------------------------------------
void Main_Thread_Communication_Check();
//------------------------------------------------------------------------------------------
HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//void xxx( int id ) {
//	int comm;
//	printf( "=[%d]===[%d]\n" , id , _dREAD_DIGITAL( 0 , &comm ) );
//}
//------------------------------------------------------------------------------------------
//===================================================================================================
int  SCHEDULER_SYSTEM_IO_Address = -2; // 2004.03.26
int  SCHEDULER_SYSTEM_STATUS = 0; // 2018.12.12

void SCHEDULER_SYSTEM_IO( int data ) { // 2004.03.26
	int CommStatus;
	//
	SCHEDULER_SYSTEM_STATUS = data; // 2018.12.12
	//
	if ( SCHEDULER_SYSTEM_IO_Address == -2 ) {
		SCHEDULER_SYSTEM_IO_Address   = _FIND_FROM_STRING( _K_D_IO , "CTC.SCHEDULER_STATUS" );
	}
	if ( SCHEDULER_SYSTEM_IO_Address != -1 ) {
		_dWRITE_DIGITAL( SCHEDULER_SYSTEM_IO_Address , data , &CommStatus );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GET_SYSTEM_FILE( char *retfilename , char *name1 , char *name2 ) { // 2017.03.12
	//
	char Buffer[256];
	char Buffer2[256];
	//
	strcpy( Buffer , "$REQUEST_SYSTEM$" );
	//
	_IO_GET_SYSTEM_FOLDER( Buffer , 255 );
	//
	if ( name2 == NULL ) {
		sprintf( retfilename , "%s\\%s" , Buffer , name1 );
	}
	else {
		sprintf( Buffer2 , name1 , name2 );
		sprintf( retfilename , "%s\\%s" , Buffer , Buffer2 );
	}
	//
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int KGUI_s_Get_WIN_SIZE_XS();
int KGUI_s_Get_WIN_SIZE_YS();

void KGUI_DRAW_StrBox25( HDC hDC , int x , int y , char *str , int xsize , int ysize , int xoffset , int charsize , int align , int lcolorU , int lcolorD , int tcolor , int bcolor , char *fontstr );

//
void GUI_Draw_Name( HDC hDC , int x0 , int y0 , char *str , int xsize0 , int ysize0 , int charsize , double GUI_Draw_Name_xp , double GUI_Draw_Name_yp ) {
	int x , y , xs , ys , cs;
	//
	x = (int) ( (double) x0 * GUI_Draw_Name_xp );
	y = (int) ( (double) y0 * GUI_Draw_Name_yp );
	xs = (int) ( (double) xsize0 * GUI_Draw_Name_xp );
	ys = (int) ( (double) ysize0 * GUI_Draw_Name_yp );
	cs = (int) ( (double) charsize * GUI_Draw_Name_xp );
	//
	KGUI_DRAW_StrBox25( hDC , x , y , str , xs , ys , 0 , cs , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank , "Arial" );
}

void GUI_Name_Drawing( HWND hWnd , HDC hDC , double xp , double yp ) {
	GUI_Draw_Name( hDC , 110 ,   6 , "Service"					,  84 , 20 , 16 , xp , yp );
	GUI_Draw_Name( hDC , 200 ,   6 , "GetPr"					,  50 , 20 , 16 , xp , yp );
	GUI_Draw_Name( hDC , 257 ,   6 , "PutPr"					,  50 , 20 , 16 , xp , yp );
	GUI_Draw_Name( hDC , 314 ,   6 , "PrcsTime"					,  57 , 20 , 15 , xp , yp );
	GUI_Draw_Name( hDC , 377 ,   6 , "SW Control"				,  87 , 20 , 16 , xp , yp );
	GUI_Draw_Name( hDC , 470 ,   6 , "Get Exchg"				,  64 , 20 , 16 , xp , yp );
	GUI_Draw_Name( hDC , 540 ,   6 , "Put Exchg"				,  64 , 20 , 16 , xp , yp );
	GUI_Draw_Name( hDC , 612 ,   6 , "CM:Map,AutoHO/PM:Process"	, 194 , 20 , 16 , xp , yp );
}


BOOL APIENTRY Gui_For_Main_Interface_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
//	static int sec_30 , sec_1; // 2003.07.14
//	static int OldRunStatus; // 2003.07.14
	int i;
	static double wxp , wyp;
	HBITMAP hBitmap;
	char Message[256];
	

	HFILE hFile;
	int  ReadCnt , posdata;
	BOOL FileEnd;
	int retindexs[256];
	int retindexsz[256];
	char NameBuffer[256];
	char Buffer[256];
	//
	int layoutxoffset , layoutyoffset , layoutxsoffset , layoutysoffset; // 2017.03.01

	switch ( msg ) {

		case WM_SYSKEYDOWN : // 2013.11.21
			if ( wParam == VK_F4 ) return FALSE;
			return TRUE;

		case WM_ERASEBKGND : // 2012.12.07
			return TRUE;

		case WM_USER:
			switch ( LOWORD( wParam ) ) {
			case WM_CLOSE:
				//<<-----------------------------------------------------------
				//>>-----------------------------------------------------------
				return TRUE;
			case WM_ACTIVATE:
				//>>-----------------------------------------------------------
				//<<-----------------------------------------------------------
				return TRUE;
			}
			return TRUE;

		case WM_DRAWITEM:
			if ( !KWIN_DRAW_USER_BUTTON_BITMAP( 0 , (LPDRAWITEMSTRUCT) lParam ) ) {
			}
			return TRUE;


		case WM_INITDIALOG:
			//
			layoutxoffset = layoutyoffset = layoutxsoffset = layoutysoffset = 0; // 2017.03.01

//			sec_30 = 0; // 2003.07.14
//			sec_1  = 0; // 2003.07.14
//			OldRunStatus = -1; // 2003.07.14

			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------


			//
			FileEnd = FALSE;
			//
			GET_SYSTEM_FILE( NameBuffer , "Scheduler(%s).cfg" , PROGRAM_FUNCTION_READ() ); // 2017.03.12
			//
			if ( ( hFile = _lopen( NameBuffer , OF_READ ) ) != -1 ) {
				FileEnd = TRUE;
			}
			else {
				//
				GET_SYSTEM_FILE( NameBuffer , "Layout\\Scheduler\\Layout.cfg" , NULL ); // 2017.03.12
				//
				if ( ( hFile = _lopen( NameBuffer , OF_READ ) ) != -1 ) {
					FileEnd = TRUE;
				}
			}
			//
			if ( FileEnd ) {
				//
				while( FileEnd ) {
					FileEnd = H_Get_Line_String_From_File2_Include_Index( hFile , Buffer , 255 , &ReadCnt , retindexs , retindexsz );
					if ( ReadCnt > 4 ) {
						if ( Get_Data_From_String_with_index( Buffer , NameBuffer , retindexs[0] , retindexsz[0] ) ) {
							//
							if      ( STRCMP_L( "BUTTON_OFFSET" , NameBuffer ) ) {
								//
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[1] , retindexsz[1] , NameBuffer , -999 , 999 , &posdata ) ) {
									//
									layoutxoffset = posdata;
									layoutxsoffset = - ( posdata * 2 );
									//
								}
								//
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[2] , retindexsz[2] , NameBuffer , -999 , 999 , &posdata ) ) {
									//
									layoutyoffset = posdata;
									layoutysoffset = - ( posdata * 2 );
									//
								}
								//
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[3] , retindexsz[3] , NameBuffer , -999 , 999 , &posdata ) ) {
									//
									if ( posdata != -999 ) layoutxsoffset = posdata;
									//
								}
								//
								if ( Get_Data_From_String_for_INT_with_index( Buffer , retindexs[4] , retindexsz[4] , NameBuffer , -999 , 999 , &posdata ) ) {
									//
									if ( posdata != -999 ) layoutysoffset = posdata;
									//
								}
								//
								continue;
								//
							}
							//
						}
					}
				}
				_lclose( hFile );
			}
			//
			//====================================================================================================================================================
			//
			GET_SYSTEM_FILE( NameBuffer , "Layout\\Scheduler" , NULL ); // 2017.03.12
			//
			KWIN_SET_USER_BUTTON_GROUP( 0 , NameBuffer , hdlg , "background.bmp" );

			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_UTILITY_BUTTON		) , "SysOption_Ena.bmp"	, "SysOption_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_RCPFILE_BUTTON		) , "RcpFile_Ena.bmp"	, "RcpFile_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_MODULE_BUTTON		) , "Module_Ena.bmp"	, "Module_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_RERCP_BUTTON		) , "PrePost_Ena.bmp"	, "PrePost_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_FAIL_BUTTON			) , "Fail_Ena.bmp"		, "Fail_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CLSSNR_BUTTON		) , "Clear_Ena.bmp"		, "Clear_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_FIXCLST_BUTTON		) , "Fixed_Ena.bmp"		, "Fixed_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_HANDLINGSIDE_BUTTON	) , "Handling_Ena.bmp"	, "Handling_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_FA_BUTTON			) , "FA_Ena.bmp"		, "FA_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_INTERLOCK_BUTTON	) , "Intlks_Ena.bmp"	, "Intlks_Dis.bmp"	);

			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_CPJOB_BUTTON		) , "Job_Ena.bmp"		, "Job_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_RBDATA_BUTTON		) , "RobotAnim_Ena.bmp"	, "RobotAnim_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_SCH_BUTTON			) , "System_Ena.bmp"	, "System_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_RB_BUTTON			) , "RobotCal_Ena.bmp"	, "RobotCal_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_RB2_BUTTON			) , "RobotCal2_Ena.bmp"	, "RobotCal2_Dis.bmp"	);
			KWIN_SET_USER_BUTTON_BITMAP( GetDlgItem( hdlg , IDC_USEROPTION_BUTTON	) , "User_Ena.bmp"		, "User_Dis.bmp"	);


			if ( KWIN_Item_Size_ReSetM_Check( KGUI_s_Get_WIN_SIZE_XS() + layoutxsoffset , KGUI_s_Get_WIN_SIZE_YS() + layoutysoffset , 0 , 0 , &wxp , &wyp ) ) {
				//
				KWIN_Item_Size_ReSetM_Init( hdlg , wxp , wyp , layoutxoffset , layoutyoffset , ( wxp == 1 ) ? 0 : -2 , ( wyp == 1 ) ? 0 : -2 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_DISPLAY );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_P6 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_AL );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_NAME_IC );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_REMLOC_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_GET_PRIORITY_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_PUT_PRIORITY_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_PM_GAP_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PM_GAP_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PM_GAP_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PM_GAP_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PM_GAP_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_PM_GAP_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_P6 );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_AL );
				KWIN_Item_Size_ReSetM_Add( IDC_SW_CONTROL_IC );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PICK_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_C4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_PLACE_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_CASS_MAP_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_CASS_MAP_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_CASS_MAP_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_CASS_MAP_C4 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_MODE_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_MODE_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_MODE_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_MODE_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_MODE_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_MODE_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_TIME_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_TIME_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_TIME_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_TIME_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_TIME_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_IN_TIME_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_AUTO_HANDOFF_C1 );
				KWIN_Item_Size_ReSetM_Add( IDC_AUTO_HANDOFF_C2 );
				KWIN_Item_Size_ReSetM_Add( IDC_AUTO_HANDOFF_C3 );
				KWIN_Item_Size_ReSetM_Add( IDC_AUTO_HANDOFF_C4 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_MODE_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_MODE_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_MODE_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_MODE_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_MODE_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_MODE_P6 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_TIME_P1 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_TIME_P2 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_TIME_P3 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_TIME_P4 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_TIME_P5 );
				KWIN_Item_Size_ReSetM_Add( IDC_PROCESS_OUT_TIME_P6 );
				//
				//
				KWIN_Item_Size_ReSetM_Add( IDC_MESSAGE_LIST	);
				KWIN_Item_Size_ReSetM_Add( IDC_MESSAGE_LIST2 );
				//
				KWIN_Item_Size_ReSetM_Add( IDC_UTILITY_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_RCPFILE_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_FIXCLST_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_FAIL_BUTTON		);
				KWIN_Item_Size_ReSetM_Add( IDC_INTERLOCK_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_CPJOB_BUTTON		);
				KWIN_Item_Size_ReSetM_Add( IDC_SCH_BUTTON		);
				//
				KWIN_Item_Size_ReSetM_Add( IDC_MODULE_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_RBDATA_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_RERCP_BUTTON		);
				KWIN_Item_Size_ReSetM_Add( IDC_CLSSNR_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_HANDLINGSIDE_BUTTON	);
				KWIN_Item_Size_ReSetM_Add( IDC_FA_BUTTON		);
				KWIN_Item_Size_ReSetM_Add( IDC_RB_BUTTON		);
				KWIN_Item_Size_ReSetM_Add( IDC_RB2_BUTTON	    ); // 2013.08.01
				KWIN_Item_Size_ReSetM_Add( IDC_USEROPTION_BUTTON); // 2013.08.01

				KWIN_Item_Size_ReSetM_Run( TRUE );
			}
			//
			for ( i = 1 ; i <= 255 ; i++ ) { // 2013.08.01
				if ( _i_SCH_PRM_GET_MFI_INTERFACE_FLOW_USER_OPTIONF( i ) > 0 ) {
					i = 0;
					break;
				}
			}
			//
			if ( i == 0 ) {
				KWIN_Item_Hide( hdlg , IDC_RB_BUTTON	);
			}
			else {
				KWIN_Item_Hide( hdlg , IDC_RB2_BUTTON	);
				KWIN_Item_Hide( hdlg , IDC_USEROPTION_BUTTON);
			}
			//----------------------------------
			PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
			//----------------------------------
			return TRUE;

		case WM_PAINT:
			//=============================================================================================
			// 2004.05.21
			//=============================================================================================
//			if ( !KGUI_STANDARD_Window_Signal_Mode() ) return TRUE; // 2005.02.28
			if ( !KGUI_STANDARD_Window_Signal_Mode() ) { // 2005.02.28
				BeginPaint( hdlg, &ps );
				EndPaint( hdlg , &ps );
				return TRUE;
			}
			//=============================================================================================
			BeginPaint( hdlg, &ps );
			//
			i = 0;
			//
			if ( !KWIN_DRAW_USER_BACK_BITMAP( 0 , ps.hdc , 0 , 0 ) ) {
				//
				if ( KGUI_Get_BACKGROUND() == NULL ) {
	//				KWIN_DRAW_BACK_PAINT( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) ); // 2012.03.10
					KWIN_DRAW_BACK_PAINT_RESET( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) , wxp , wyp ); // 2012.03.10
					//
					i = 1;
					//
				}
				else {
					hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUNDF" ); // 2012.03.10
					if ( hBitmap ) { // 2012.03.10
						KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
						DeleteObject( hBitmap );
					}
					else {
						hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUNDA" ); // 2012.03.10
						if ( hBitmap ) { // 2012.03.10
							KWIN_DRAW_BITMAP_RESET( ps.hdc , 0 , 0 , wxp , wyp , hBitmap , SRCCOPY );
							DeleteObject( hBitmap );
						}
						else {
							hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
							if ( hBitmap ) {
								KWIN_DRAW_BITMAP_RESET( ps.hdc , 0 , 0 , wxp , wyp , hBitmap , SRCCOPY );
								DeleteObject( hBitmap );
							}
							else {
								KWIN_DRAW_BACK_PAINT_RESET( GetModuleHandle(NULL) , ps , MAKEINTRESOURCE( IDB_BACKGROUND ) , wxp , wyp ); // 2012.03.10
								//
								i = 1;
							}
						}
					}
				}
			}
			//
			if ( i == 1 ) GUI_Name_Drawing( hdlg , ps.hdc , wxp , wyp );
			//
			GUI_Parameter_Drawing( hdlg , ps.hdc );
			//
			EndPaint( hdlg , &ps );
			return TRUE;

//		case WM_TIMER: // 2003.07.14
//			if ( sec_30 >= 120 ) {	sec_30 = 0;		_i_SCH_IO_MTL_SAVE();		}
//			sec_30++;
//			RUNTIME_MONITOR( ( sec_1 == 0 ) , &OldRunStatus );
//			if ( sec_1  >=  3 ) sec_1  = 0; else sec_1++;
//			return TRUE;

		case WM_COMMAND:
			if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_MESSAGE_LIST ) {
				//
				if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_DBLCLK ) { // 2010.12.01
					//
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_MESSAGE_LIST ) );
					//
					if ( i >= 0 ) {
						//
						ListBox_GetText( GetDlgItem( hdlg , IDC_MESSAGE_LIST ) , i , Message );
						//
						if ( !STRNCMP_L( Message , " >>REJECT: " , 11 ) ) {
							//
							if ( !STRCMP_L( Message + 9 , "" ) ) {
								//
								if ( IDYES == MessageBox( hdlg , "Do you want to Send Message to scheduler again?", Message + 9 , MB_ICONQUESTION | MB_YESNO ) ) {
									_dWRITE_FUNCTION_EVENT( -1 , Message + 9 );
								}
								//
							}
							//
						}
						//
					}
				}
				return TRUE;
			}
			if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_MESSAGE_LIST2 ) {
				return TRUE;
			}
			return( GUI_Parameter_Control( hdlg , LOWORD( wParam ) ) );
			break;

		case WM_RBUTTONDOWN:
			GUI_Parameter_Show( hdlg );
			return TRUE;

		case WM_DESTROY:
			KWIN_DELETE_USER_BUTTON_BITMAP( 0 );
			break;

	}
	return FALSE;
}

void RUNTIME_MONITOR_CHECK() { // 2008.10.02
	DWORD T_1 , T_30 , T_Curr;
	int OldRunStatus;

	T_1  = GetTickCount();
	T_30 = T_1;
	OldRunStatus = -1;
	//
	while ( TRUE ) {
		//
		T_Curr = GetTickCount();
		//
		if ( ( T_Curr - T_30 ) >= 30000 ) {
			T_30 = T_Curr;
			_i_SCH_IO_MTL_SAVE();
		}
		//
//		if ( ( T_Curr - T_1 ) >= 1000 ) { // 2012.03.09
		if ( ( T_Curr - T_1 ) >= 100 ) { // 2012.03.09
			//
			T_1 = T_Curr;
			//
			FA_RUN_STATUS_IO_SET( &OldRunStatus );
			//
			ALARM_INFO_MANAGEMENT(); // 2012.03.21
			//
			RECIPE_FILE_PROCESS_File_Locking_AutoDelete(); // 2013.09.13
			//
		}
		//
		if ( MANAGER_ABORT() ) {
			if ( !Get_RunPrm_RUNNING_CLUSTER() ) {
				MANAGER_ABORT_FALSE();
			}
		}
		//
		PROCESS_MONITOR_STATUS();
		if ( !Get_RunPrm_RUNNING_CLUSTER() ) _EIL_IO_NOTUSE_STATUS(); // 2012.09.14
		//
		Sleep(1);
		//
	}
	_endthread();
}
//
extern BOOL SLOTPROCESSING; // 2016.04.26

BOOL RUNTIME_MONITOR_INIT() { // 203.07.14
	int i;
	if ( SLOTPROCESSING ) { // 2016.04.26
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) && EQUIP_PROCESS_FUNCTION_CONNECT( i ) ) {
				if ( _beginthread( (void *) PROCESS_MONITOR_LOG , 0 , (void *) i ) == -1 ) {
					//--------------------------------------------------------------------------------------------------
					// 2004.08.18
					//--------------------------------------------------------------------------------------------------
					_IO_CIM_PRINTF( "THREAD FAIL PROCESS_MONITOR_LOG %d\n" , i );
					//--------------------------------------------------------------------------------------------------
					return FALSE;
				}
			}
		}
	}
	else {
		for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( i ) && EQUIP_INTERFACE_FUNCTION_CONNECT( i ) ) {
				if ( _beginthread( (void *) PROCESS_MONITOR_LOG , 0 , (void *) i ) == -1 ) {
					//--------------------------------------------------------------------------------------------------
					// 2004.08.18
					//--------------------------------------------------------------------------------------------------
					_IO_CIM_PRINTF( "THREAD FAIL PROCESS_MONITOR_LOG %d\n" , i );
					//--------------------------------------------------------------------------------------------------
					return FALSE;
				}
			}
		}
	}
	//========================================================
	// Auto Handler Check Message(2002.02.05)
	//========================================================
	if ( strlen( _i_SCH_PRM_GET_DFIX_CARR_AUTO_HANDLER() ) > 0 ) {
		i = _FIND_FROM_STRING( _K_F_IO , _i_SCH_PRM_GET_DFIX_CARR_AUTO_HANDLER() );
		if ( i >= 0 ) {
			_dWRITE_FUNCTION_EVENT( i , "REFRESH" );
		}
	}
	//
	if ( _beginthread( (void *) RUNTIME_MONITOR_CHECK , 0 , 0 ) == -1 ) {
		//--------------------------------------------------------------------------------------------------
		// 2004.08.18
		//--------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "THREAD FAIL RUNTIME_MONITOR_CHECK\n" );
		//--------------------------------------------------------------------------------------------------
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void RunTime_Value_Init_Before_Readfile() {
	//
	Scheduler_InfoInit(); // 2009.07.16
	//
	SYSTEM_TAG_DATA_INIT(); // 2006.11.16
	//
	IO_DEBUG_INIT();
	//
	SCHEDULING_DATA_STRUCTURE_INIT();
	//
/*
// 2007.11.28
	if ( !SCHMULTI_INIT() ) {
		//--------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "Scheduler MultiJob Interface Connet Fail\n" );
		//--------------------------------------------------------------------------------------------------
	}
*/
	//
	//---------------------------------------------------
	SCHEDULER_CONTROL_Data_Initialize_Part( 0 , -1 );
	//---------------------------------------------------
}
//---------------------------------------------------------------------------------------
void RunTime_Value_Init_Before_Readfile2() { // 2002.09.13
/*
// 2016.10.21
//		if ( !SCHMULTI_INIT() ) {
		//--------------------------------------------------------------------------------------------------
//		_IO_CIM_PRINTF( "Scheduler MultiJob Interface Connet Fail\n" ); // 2015.02.10
		_IO_CIM_PRINTF( "Scheduler MultiJob Interface Connection Fail\n" ); // 2015.02.10
		//--------------------------------------------------------------------------------------------------
	}
*/
	//
	IO_LOG_INIT();
	LOG_MTL_INIT();
	//
	IO_DATA_INIT_BEFORE_READFILE();
	EQUIP_INIT_BEFORE_READFILE();
}
//---------------------------------------------------------------------------------------
void RunTime_Value_Init_Before_Readfile3() { // 2016.10.21
	//
	if ( !SCHMULTI_INIT() ) {
		//--------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "Scheduler MultiJob Interface Connection Fail\n" );
		//--------------------------------------------------------------------------------------------------
	}
	//
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL RunTime_Value_Init_After_Readfile() {
	IO_DATA_INIT_AFTER_READFILE();
	EQUIP_INIT_AFTER_READFILE();
	//
	_SCH_COMMON_INITIALIZE_PART( 0 , -1 );
	//
	if ( _beginthread( (void *) Scheduler_Main_Other_End_Check_Monitor , 0 , 0 ) == -1 ) { // 2002.07.10
		//--------------------------------------------------------------------------------------------------
		// 2004.08.18
		//--------------------------------------------------------------------------------------------------
		_IO_CIM_PRINTF( "THREAD FAIL Scheduler_Main_Other_End_Check_Monitor\n" );
		//--------------------------------------------------------------------------------------------------
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	HWND hWnd;
	//----------------------------------
	InitCommonControls(); // 2004.05.19 (Move Here)
	//----------------------------------
	if ( !KWIN_DIALOG_DRIVER_LOADING() ) {
		MessageBeep(MB_ICONHAND); // 2004.05.19 (Move Here)
		_IO_CIM_PRINTF( "KWIN_DIALOG_DRIVER_LOADING Init Error\n" );
		return SYS_ABORTED;
	}
	//----------------------------------
	// RUNTIME_MONITOR_INIT(); // 2003.07.14 // 2004.08.18
	if ( !RUNTIME_MONITOR_INIT() ) { // 2003.07.14 // 2004.08.18
		_IO_CIM_PRINTF( "RUNTIME_MONITOR_INIT Init Error\n" );
		return SYS_ABORTED;
	}
	//----------------------------------
	//---------------------------------------------------
	BUTTON_HANDLER_REMOTE( "INIT" );
	//----------------------------------
	SCHEDULER_SYSTEM_IO( 2 ); // 2004.03.26
	//----------------------------------
	while( PROGRAM_GUI_SPAWNING == 0 ) { Sleep(100); } // 2003.07.12
	//----------------------------------
	SCHEDULER_SYSTEM_IO( 3 ); // 2004.03.26
	//----------------------------------
//	InitCommonControls(); // 2004.05.19 (Move Up)
	KGUI_Set_BACKGROUND( PROGRAM_PARAMETER_READ() );
	//>>-------------------------------------------------------------------------
//	if ( !KWIN_DIALOG_DRIVER_LOADING() ) MessageBeep(MB_ICONHAND); // 2004.05.19 (Move Up)
	//>>-------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------
//	GoModalDialogBoxParam( GETHINST(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , GET_MAIN_WINDOW() , Gui_For_Main_Interface_Proc , (LPARAM) NULL ); // 2002.06.17
	hWnd = GET_MAIN_WINDOW();
//	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Gui_For_Main_Interface_Proc , (LPARAM) NULL ); // 2004.01.19
//	GoModalDialogBoxParam( GetModuleHandle(NULL) , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Gui_For_Main_Interface_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
	GoModalDialogBoxParam( GETHINST(hWnd)  , MAKEINTRESOURCE( IDD_CONTROL_PAD ) , hWnd , Gui_For_Main_Interface_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10

	SCHEDULER_SYSTEM_IO( 4 ); // 2004.03.26

	SYSTEM_KILL_MYSELF();
	
	KWIN_DIALOG_DRIVER_UNLOADING();

	//----------------------------------
	PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
	//----------------------------------
	return SYS_SUCCESS;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	int i;
	if ( no == -1 ) {
		KGUI_STANDARD_Window_Kill_Signal();
		DeleteCriticalSection( &CR_MAIN );
		DeleteCriticalSection( &CR_WAIT );
		DeleteCriticalSection( &CR_FEMRUN );
		DeleteCriticalSection( &CR_FEM_MAP_PICK_PLACE );
//		DeleteCriticalSection( &CR_FEMRUN_SW_BATCH ); // 2004.05.03 // 2004.05.10
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) DeleteCriticalSection( &CR_TMRUN[ i ] );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) DeleteCriticalSection( &CR_HMAPPING[ i ] );
		DeleteCriticalSection( &CR_MAINT_INF_TRANSFER ); // 2006.08.30
		DeleteCriticalSection( &CR_PRE_BEGIN_END ); // 2006.12.01
		DeleteCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.05.21
		SCHEDULER_SYSTEM_IO( 4 ); // 2004.03.26
	}
	if ( ( no > 0 ) && ( PROGRAM_GUI_SPAWNING == 0 ) ) { // 2003.07.12
		PROGRAM_GUI_SPAWNING = 1;
		for ( i = 0 ; i < 200 ; i++ ) {
			Sleep(100);
			if ( PROGRAM_GUI_SPAWNING == 2 ) break;
		}
	}
	KGUI_STANDARD_Screen_Nav_Info( no );
}
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	int i;
//	char Else1Str[256] , Else2Str[256] , Else3Str[256] , Else4Str[256]; // 2017.10.30
	char Else1Str[256] , Else2Str[256] , Else3Str[256] , Else4Str[256] , Else5Str[256]; // 2017.10.30

	//getch();
	//
	i = _FILE2SM_INTERFACE_INITIALIZING( GET_SYSTEM_NAME() );	// 0:OK 1:AlreadyInit 2:CreateMutexFail 3:WaitObjectFail 4:OpenFileFail 5:CreateFileFail 6:MultiMutexFail
	//
	if ( i != 0 ) {
		//
		_IO_CIM_PRINTF( "_FILE2SM_INTERFACE_INITIALIZING Error (%d)\n" , i );
		//
	}
	//====================================================================================================================
	STR_SEPERATE_CHAR( PROGRAM_ARGUMENT_READ() , '|' , Else1Str , Else3Str , 255 );
	//


	/*
	//
	// 2017.10.30
	//
	STR_SEPERATE_CHAR( Else1Str , ':' , Else2Str , Else4Str , 255 ); // 2006.02.06
	//
	//====================================================================================================================
	_i_SCH_PRM_SET_DFIX_SYSTEM_PATH( Else2Str ); // 2006.02.06
	//====================================================================================================================
	if ( strlen( Else4Str ) > 0 ) {
		//==========================================================
		if ( strcmp( Else4Str , "$" ) == 0 ) { // 2006.02.16
			_IO_GET_LOG_FOLDER( Else4Str , 63 ); // 2006.02.16
		}
		//==========================================================
		_i_SCH_PRM_SET_DFIX_LOG_PATH( Else4Str ); // 2006.02.06
	}
	else {
		_i_SCH_PRM_SET_DFIX_LOG_PATH( Else2Str ); // 2006.02.06
	}
	//
	*/



	//
	// 2017.10.30
	//
	STR_SEPERATE_CHAR( Else1Str , ':' , Else2Str , Else5Str , 255 ); // 2006.02.06
	STR_SEPERATE_CHAR( Else5Str , '>' , Else4Str , Else1Str , 255 );
	//
	//====================================================================================================================
	_i_SCH_PRM_SET_DFIX_SYSTEM_PATH( Else2Str ); // 2006.02.06
	//====================================================================================================================
	if ( strlen( Else4Str ) > 0 ) {
		//==========================================================
		if ( strcmp( Else4Str , "$" ) == 0 ) { // 2006.02.16
			_IO_GET_LOG_FOLDER( Else4Str , 63 ); // 2006.02.16
		}
		//==========================================================
		_i_SCH_PRM_SET_DFIX_LOG_PATH( Else4Str ); // 2006.02.06
		//
		//
		if ( strlen( Else1Str ) > 0 ) {
			sprintf( Else5Str , "%s" , Else1Str );
		}
		else {
			sprintf( Else5Str , "%s" , Else4Str );
		}
		//
	}
	else {
		_i_SCH_PRM_SET_DFIX_LOG_PATH( Else2Str ); // 2006.02.06
		//
		//
		if ( strlen( Else1Str ) > 0 ) {
			sprintf( Else5Str , "%s" , Else1Str );
		}
		else {
			sprintf( Else5Str , "%s" , Else2Str );
		}
		//
	}
	//
	_i_SCH_PRM_SET_DFIX_TMP_PATH( Else5Str ); // 2017.10.30
	//




	//====================================================================================================================
	STR_SEPERATE_CHAR( Else3Str , '|' , Else1Str , Else2Str , 255 );
	//====================================================================================================================
	USER_RECIPE_DLL_LOAD( Else1Str , 0 );
	//====================================================================================================================
	STR_SEPERATE_CHAR( Else2Str , '|' , Else1Str , Else3Str , 255 );
	//====================================================================================================================
	USER_SCH_DLL_LOAD( Else1Str , Else3Str , 0 );
	//====================================================================================================================
	InitializeCriticalSection( &CR_MAIN );
	InitializeCriticalSection( &CR_WAIT );
	InitializeCriticalSection( &CR_FEMRUN );
	InitializeCriticalSection( &CR_FEM_MAP_PICK_PLACE );
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) InitializeCriticalSection( &CR_TMRUN[ i ] );
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) InitializeCriticalSection( &CR_HMAPPING[ i ] );
	InitializeCriticalSection( &CR_MAINT_INF_TRANSFER ); // 2006.08.30
	InitializeCriticalSection( &CR_PRE_BEGIN_END ); // 2006.12.01
	InitializeCriticalSection( &CR_PRE_BEGIN_ONLY ); // 2010.05.21
	InitializeCriticalSection( &CR_FINISH_CHECK ); // 2011.04.19
}
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	char Filename[256];
	int i , slinfo , userdllload = FALSE;
	//---------------------------------------------------
	//
	sprintf( Filename , "<BUILD>%s %s" , __DATE__ , __TIME__ );	REGIST_FILE_INFO( Filename );
	//---------------------------------------------------
	//---------------------------------------------------

	//getch();
	//-----------------------------------------------------------
	SCHEDULER_SYSTEM_IO( 1 ); // 2004.03.26
	//-----------------------------------------------------------
#ifdef _SCH_MIF
	if ( USER_SCH_USE_TAG_GET() == -1 ) {
		//
		sprintf( Filename , "%s/System.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
		slinfo = FILE_SYSTEM_SETUP_FOR_GET_ALG( Filename );
		//
		if ( slinfo >= 0 ) {
			switch( slinfo ) {
			case 0 :
#ifdef PM_CHAMBER_60
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User0_Scheduler60.dll" );
#else
	#ifdef PM_CHAMBER_30
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User0_Scheduler30.dll" );
	#else
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User0_Scheduler.dll" );
	#endif
#endif
				break;
			case 4 :
#ifdef PM_CHAMBER_60
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User4_Scheduler60.dll" );
#else
	#ifdef PM_CHAMBER_30
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User4_Scheduler30.dll" );
	#else
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User4_Scheduler.dll" );
	#endif
#endif
				break;
			case 6 :
#ifdef PM_CHAMBER_60
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User6_Scheduler60.dll" );
#else
	#ifdef PM_CHAMBER_30
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User6_Scheduler30.dll" );
	#else
				strcpy( Filename , "K:\\exec\\scheduler\\MFI\\User6_Scheduler.dll" );
	#endif
#endif
				break;
			default :
				strcpy( Filename , "" );
				break;
			}
			//
			if ( Filename[0] != 0 ) USER_SCH_DLL_LOAD( Filename , "" , 99 );
			//
			if ( USER_SCH_USE_TAG_GET() <= 0 ) {
				if ( UTIL_Get_OpenSave_File_Select( GET_MAIN_WINDOW() , FALSE , Filename , 255 , "" , "*.*" , "Select Macro Flow Interface DLL File" ) ) {
					if ( !USER_SCH_DLL_LOAD( Filename , "" , 99 ) ) {
						_IO_CIM_PRINTF( "USER_SCH_DLL_LOAD [%s] Loading Fail\n" , Filename );
					}
				}
				userdllload = TRUE;
			}
			//
		}
	}
	else {
		userdllload = TRUE;
	}
	//
	if ( USER_SCH_USE_TAG_GET() <= 0 ) {
		_IO_CIM_PRINTF( "[ERROR] MFI Dll can not find. Please regist the MFI DLL\n" );
		SCHEDULER_SYSTEM_IO( 6 ); // 2004.12.05
		return FALSE; // 2004.12.05
	}
	//
#endif
	//-----------------------------------------------------------
	//
	GUI_SAVE_PARAMETER_INIT(); // 2015.01.23
	//
	//-----------------------------------------------------------
	RunTime_Value_Init_Before_Readfile();
	//-----------------------------------------------------------
	sprintf( Filename , "%s/Message.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() ); // 2004.12.05
	if ( !FILE_MESSAGE_SETUP( Filename ) ) { // 2004.12.05
//		ERROR_HANDLER( ERROR_FILE_SYSTEM_CFG , "" ); // 2004.12.05 // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_SYSTEM_CFG , "Message.cfg File Reading Error." ); // 2004.12.05 // 2017.02.09
		SCHEDULER_SYSTEM_IO( 6 ); // 2004.12.05
		return FALSE; // 2004.12.05
	}
	//-----------------------------------------------------------
	sprintf( Filename , "%s/System.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	if ( !FILE_SYSTEM_SETUP( Filename ) ) {
//		ERROR_HANDLER( ERROR_FILE_SYSTEM_CFG , "" ); // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_SYSTEM_CFG , "System.cfg File Reading Error." ); // 2004.12.05 // 2017.02.09
		SCHEDULER_SYSTEM_IO( 6 ); // 2004.03.26
		return FALSE;
	}
	//-----------------------------------------------------------
	RunTime_Value_Init_Before_Readfile2();
	//-----------------------------------------------------------
	sprintf( Filename , "%s/Schedule.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	if ( !FILE_SCHEDULE_SETUP( Filename ) ) {
//		ERROR_HANDLER( ERROR_FILE_SCHEDULE_CFG , "" ); // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_SCHEDULE_CFG , "Schedule.cfg File Reading Error." ); // 2004.12.05 // 2017.02.09
		SCHEDULER_SYSTEM_IO( 7 ); // 2004.03.26
		return FALSE;
	}
	//-----------------------------------------------------------
	RunTime_Value_Init_Before_Readfile3(); // 2016.10.21
	//-----------------------------------------------------------
	sprintf( Filename , "%s/File.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	if ( !FILE_FILE_SETUP( Filename ) ) {
//		ERROR_HANDLER( ERROR_FILE_FILE_CFG , "" ); // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_FILE_CFG , "File.cfg File Reading Error." ); // 2004.12.05 // 2017.02.09
		SCHEDULER_SYSTEM_IO( 8 ); // 2004.03.26
		return FALSE;
	}
	sprintf( Filename , "%s/Robot.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	if ( !FILE_ROBOT_SETUP( Filename ) ) {
//		ERROR_HANDLER( ERROR_FILE_ROBOT_CFG , "" ); // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_ROBOT_CFG , "Robot.cfg File Reading Error." ); // 2004.12.05 // 2017.02.09
		SCHEDULER_SYSTEM_IO( 9 ); // 2004.03.26
		return FALSE;
	}
	sprintf( Filename , "%s/RobotSlot.cfg" , _i_SCH_PRM_GET_DFIX_SYSTEM_PATH() );
	if ( !FILE_ROBOT2_SETUP( Filename ) ) {
//		ERROR_HANDLER( ERROR_FILE_ROBOT2_CFG , "" ); // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_ROBOT2_CFG , "RobotSlot.cfg File Reading Error." ); // 2004.12.05 // 2017.02.09
		SCHEDULER_SYSTEM_IO( 10 ); // 2004.03.26
		return FALSE;
	}
	//-----------------------------------------------------------
	if ( !FILE_PARAM_SM_SETUP() ) { // 2013.08.22
//		ERROR_HANDLER( ERROR_FILE_SYSTEM_CFG , "" ); // 2017.02.09
		ERROR_HANDLER2( ERROR_FILE_SYSTEM_CFG , "FILE_PARAM_SM_SETUP Error" ); // 2017.02.09
		SCHEDULER_SYSTEM_IO( 10 );
		return FALSE;
	}
	//-----------------------------------------------------------
	if ( !_SCH_COMMON_CONFIG_INITIALIZE( SYSTEM_RUN_ALG_STYLE_GET() , SYSTEM_SUB_ALG_STYLE_GET() , SYSTEM_GUI_ALG_STYLE_GET() , MAX_PM_CHAMBER_DEPTH ) ) {
		_IO_CIM_PRINTF( "_SCH_COMMON_CONFIG_INITIALIZE Error\n" );
		SCHEDULER_SYSTEM_IO( 11 );
		return FALSE;
	}
	//-----------------------------------------------------------
	if ( !_SCH_COMMON_LOCKEKEY_CHECK( SYSTEM_RUN_ALG_STYLE_GET() , SYSTEM_SUB_ALG_STYLE_GET() , &slinfo , userdllload ) ) {
		SCHEDULER_SYSTEM_IO( 5 );
		return FALSE;
	}
	//-----------------------------------------------------------
	_SCH_COMMON_RECIPE_DLL_INTERFACE( USER_RECIPE_GET_DLL_INSTANCE() , FALSE );
	//-----------------------------------------------------------
	//RunTime_Value_Init_After_Readfile(); // 2004.08.18
	if ( !RunTime_Value_Init_After_Readfile() ) { // 2004.08.18
		_IO_CIM_PRINTF( "RunTime_Value_Init_After_Readfile Init Error\n" );
		SCHEDULER_SYSTEM_IO( 11 );
		return FALSE;
	}
	//-----------------------------------------------------------
	for ( i = CM1 ; i <= F_IC ; i++ ) FA_MDL_STATUS_IO_SET( i );
	//-----------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SIDE; i++ ) FA_AGV_STATUS_IO_SET( i );
	//-----------------------------------------------------------
	if ( _beginthread( (void *) Main_Thread_Communication_Check , 0 , NULL ) == -1 ) {
		_IO_CIM_PRINTF( "Main_Thread_Communication_Check Init Error\n" ); // 2004.08.18
		SCHEDULER_SYSTEM_IO( 12 );
		return FALSE;
	}
	//-----------------------------------------------------------
//	REGIST_FILE_INFO( REGIST_INFO_STRING ); // 2008.07.16
	sprintf( Filename , "\v%s(%s)" , REGIST_INFO_STRING , SCH_LASTUPDATE_DATE );
	REGIST_FILE_INFO( Filename ); // 2008.07.16
	//-----------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) { // 2003.11.09
		BUTTON_SET_FLOW_STATUS( i , _MS_0_IDLE );
		BUTTON_SET_HANDOFF_CONTROL( i + CM1 , CTC_IDLE ); // 2019.02.26
	}
	//-----------------------------------------------------------
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		ROBOT_ARM_SET_CONFIGURATION( i );
	}
	ROBOT_FM_ARM_SET_CONFIGURATION();
	//-----------------------------------------------------------
	Set_RunPrm_IO_Setting( -1 );
	Set_RunPrm_Config_Change_for_ChIntlks_IO(); // 2013.11.21
	//-----------------------------------------------------------
	ALARM_INFO_SET_EVENT_RECEIVE_OPTION( -1 );
	//-----------------------------------------------------------
	if ( !SCHMULTI_CONTROLLER_MONITORING( slinfo ) ) {
		SCHEDULER_SYSTEM_IO( 13 );
		return FALSE;
	}
	//-----------------------------------------------------------
	SCHEDULER_CONTROL_Read_SDM_ins_DATA();
	//-----------------------------------------------------------
	Transfer_Maint_Inf_Code_for_Init();
	//-----------------------------------------------------------
	return TRUE;
}
