#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL FAILSNR_RESULT = FALSE;
int  FAILSNR_DATA1;
int  FAILSNR_DATA2;
int  FAILSNR_DATA3;
int  FAILSNR_DATA4;
int  FAILSNR_DATA5;
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_FAILSNR_SUB_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int Res;
	char Buffer[3000];
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
		FAILSNR_RESULT = FALSE;	return TRUE;
	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		switch( FAILSNR_DATA1 ) {
		case FAIL_LIKE_ABORT					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Abort Only" );	break;
		case FAIL_LIKE_CONT_GO_USE				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Go) Fail.PM(Use)" );	break;
		case FAIL_LIKE_CONT_GO_DISABLE			:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Go) Fail.PM(Disable)" );	break;
		case FAIL_LIKE_CONT_OUT_USE				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Skip) Fail.PM(Use)" );	break;
		case FAIL_LIKE_CONT_OUT_DISABLE			:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Skip) Fail.PM(Disable)" );	break;
		case FAIL_LIKE_END_GO_USE				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Go) Fail.PM(Use)" );	break;
		case FAIL_LIKE_END_GO_DISABLE			:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Go) Fail.PM(Disable)" );	break;
		case FAIL_LIKE_END_OUT_USE				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Skip) Fail.PM(Use)" );	break;
		case FAIL_LIKE_END_OUT_DISABLE			:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Skip) Fail.PM(Disable)" );	break;
		//
		case FAIL_LIKE_CONT_STOP_DISABLE		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(Disable)" );	break;
		case FAIL_LIKE_CONT_GO_DISABLESA		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Go) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_CONT_OUT_DISABLESA		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Skip) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLESA		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLEHW		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(DisableHW)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLEHWSA	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+DisableHW)" );	break;
		case FAIL_LIKE_CONT_GO_DISABLE_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Go) Fail.PM(Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_OUT_DISABLE_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Skip) Fail.PM(Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLE_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_GO_DISABLESA_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Go) Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_OUT_DISABLESA_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Skip) Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLESA_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLEHW_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(DisHW+Abort)" );	break;
		case FAIL_LIKE_CONT_STOP_DISABLEHWSA_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)" );	break;
		case FAIL_LIKE_CONT_NOTUSE_DISABLE_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont)Next.PM(NotUse)Fail.PM(Disable+Abort)" );	break;
		case FAIL_LIKE_CONT_NOTUSE_DISABLESA_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont)Next.PM(NotUse)Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_CONT_NOTUSE_DISABLEHW_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont)Next.PM(NotUse)Fail.PM(DisHW+Abort)" );	break;
		case FAIL_LIKE_CONT_NOTUSE_DISABLEHWSA_ABORT:KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 ,"Lot(Cont)Next.PM(NotUse)Fail.PM(SA+DisHW+Abort)" );	break;
		//
		case FAIL_LIKE_END_STOP_DISABLE			:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(Disable)" );	break;
		case FAIL_LIKE_END_GO_DISABLESA			:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Go) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_END_OUT_DISABLESA		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Skip) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_END_STOP_DISABLESA		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_END_STOP_DISABLEHW		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(DisableHW)" );	break;
		case FAIL_LIKE_END_STOP_DISABLEHWSA		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)" );	break;
		case FAIL_LIKE_END_GO_DISABLE_ABORT		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Go) Fail.PM(Dis+Abort)" );	break;
		case FAIL_LIKE_END_OUT_DISABLE_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Skip) Fail.PM(Dis+Abort)" );	break;
		case FAIL_LIKE_END_STOP_DISABLE_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(Dis+Abort)" );	break;
		case FAIL_LIKE_END_GO_DISABLESA_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Go) Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_END_OUT_DISABLESA_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Skip) Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_END_STOP_DISABLESA_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_END_STOP_DISABLEHW_ABORT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(DisHW+Abort)" );	break;
		case FAIL_LIKE_END_STOP_DISABLEHWSA_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)" );	break;
		case FAIL_LIKE_END_NOTUSE_DISABLE_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(NotUse)Fail.PM(Disable+Abort)" );	break;
		case FAIL_LIKE_END_NOTUSE_DISABLESA_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(NotUse)Fail.PM(SA+Dis+Abort)" );	break;
		case FAIL_LIKE_END_NOTUSE_DISABLEHW_ABORT:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(NotUse)Fail.PM(DisHW+Abort)" );	break;
		case FAIL_LIKE_END_NOTUSE_DISABLEHWSA_ABORT:KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End) Next.PM(NotUse)Fail.PM(SA+DisHW+Abort)" );	break;
		//
		case FAIL_LIKE_CONT_OUT_DISABLEHW		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(Cont) Next.PM(Skip) Fail.PM(DisableHW)" );	break; // 2006.06.28
		case FAIL_LIKE_END_OUT_DISABLEHW		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "Lot(End)  Next.PM(Skip) Fail.PM(DisableHW)" );	break; // 2006.06.28
		//
		case FAIL_LIKE_USER_STYLE_1				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , "User-Style-001" );	break; // 2006.02.03
		}
		switch( FAILSNR_DATA2 ) {
		case MRES_LIKE_FAIL		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Fail" );	break;
		case MRES_LIKE_SUCCESS	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Success" );	break;
		case MRES_LIKE_NOTRUN	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "NotRun" );	break;
		case MRES_LIKE_SKIP		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Skip" );	break;
			//
			// 2015.04.30
			//
		case 4					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Fail(TMLast)" );	break;
		case 5					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Success(TMLast)" );	break;
		case 6					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "NotRun(TMLast)" );	break;
		case 7					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Skip(TMLast)" );	break;
			//
			// 2018.09.27
			//
		case 8					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Fail(ReRun)" );	break;
		case 9					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Success(ReRun)" );	break;
		case 10					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "NotRun(ReRun)" );	break;
		case 11					:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B2 , "Skip(ReRun)" );	break;
		}
		switch( FAILSNR_DATA3 ) {
		case MRES_LIKE_FAIL		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "Fail" );	break;
		case MRES_LIKE_SUCCESS	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "Success" );	break;
		case MRES_LIKE_NOTRUN	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "NotRun" );	break;
		case MRES_LIKE_SKIP		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B3 , "Skip" );	break;
		}
		switch( FAILSNR_DATA4 ) {
		case STOP_LIKE_NONE				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "N/A" );	break;
		case STOP_LIKE_PM_NONE_GO_OUT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "PM(Run) NextPM.Skip" );	break;
		case STOP_LIKE_PM_ABORT_GO_NEXT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "PM(Abort) NextPM.Go" );	break;
		case STOP_LIKE_PM_ABORT_GO_OUT	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "PM(Abort) NextPM.Skip" );	break;
		case STOP_LIKE_NONE_ALLABORT_REMAINPRCS				:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "N/A(AllAbort PrcsRemain)" );	break; // 2012.02.08
		case STOP_LIKE_PM_NONE_GO_OUT_ALLABORT_REMAINPRCS	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "PM(Run) NextPM.Skip(AllAbort PrcsRemain)" );	break; // 2012.02.08
		case STOP_LIKE_PM_ABORT_GO_NEXT_ALLABORT_REMAINPRCS	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "PM(Abort) NextPM.Go(AllAbort PrcsRemain)" );	break; // 2012.02.08
		case STOP_LIKE_PM_ABORT_GO_OUT_ALLABORT_REMAINPRCS	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B4 , "PM(Abort) NextPM.Skip(AllAbort PrcsRemain)" );	break; // 2012.02.08
		}
		switch( FAILSNR_DATA5 ) {
		case MRES_SUCCESS_LIKE_SUCCESS	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B5 , "Success" );	break;
		case MRES_SUCCESS_LIKE_NOTRUN	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B5 , "NotRun" );	break;
		case MRES_SUCCESS_LIKE_SKIP		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B5 , "Skip" );	break;
		case MRES_SUCCESS_LIKE_FAIL		:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B5 , "Fail" );	break;
		case MRES_SUCCESS_LIKE_ALLSKIP	:	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B5 , "Skip(All)" );	break; // 2005.04.23
		}
		EndPaint( hdlg , &ps );
		return TRUE;
	case WM_COMMAND :
		switch( wParam ) {
		case IDC_READY_USE_B1 :
			Res = FAILSNR_DATA1;
			//=======================================================================================================
			// 2006.02.03
			//=======================================================================================================
			if ( Res >= 1000 ) {
				Res = Res - 1000 + FAIL_LIKE_DEFAULT_LAST_ITEM + 1;
			}
			//=======================================================================================================
/*
			strcpy( Buffer , "0:Abort Only\
|1: Lot(Con) Next.PM(Go)   Fail.PM(Use)\
|2: Lot(Con) Next.PM(Go)   Fail.PM(Disable)\
|3: Lot(Con) Next.PM(Skip) Fail.PM(Use)\
|4: Lot(Con) Next.PM(Skip) Fail.PM(Disable)\
|5: Lot(End) Next.PM(Go)   Fail.PM(Use)\
|6: Lot(End) Next.PM(Go)   Fail.PM(Disable)\
|7: Lot(End) Next.PM(Skip) Fail.PM(Use)\
|8: Lot(End) Next.PM(Skip) Fail.PM(Disable)\
|9: Lot(Con) Next.PM(Stop) Fail.PM(Disable)\
|10:Lot(Con) Next.PM(Go)   Fail.PM(SA+Disable)\
|11:Lot(Con) Next.PM(Skip) Fail.PM(SA+Disable)\
|12:Lot(Con) Next.PM(Stop) Fail.PM(SA+Disable)\
|13:Lot(Con) Next.PM(Stop) Fail.PM(DisableHW)\
|14:Lot(Con) Next.PM(Stop) Fail.PM(SA+DisableHW)\
|15:Lot(Con) Next.PM(Go)   Fail.PM(Dis+Abort)\
|16:Lot(Con) Next.PM(Skip) Fail.PM(Dis+Abort)\
|17:Lot(Con) Next.PM(Stop) Fail.PM(Dis+Abort)\
|18:Lot(Con) Next.PM(Go)   Fail.PM(SA+Dis+Abort)\
|19:Lot(Con) Next.PM(Skip) Fail.PM(SA+Dis+Abort)\
|20:Lot(Con) Next.PM(Stop) Fail.PM(SA+Dis+Abort)\
|21:Lot(Con) Next.PM(Stop) Fail.PM(DisHW+Abort)\
|22:Lot(Con) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)\
|23:Lot(Con) Next.PM(NotUse) Fail.PM(Disable+Abort)\
|24:Lot(Con) Next.PM(NotUse) Fail.PM(SA+Dis+Abort)\
|25:Lot(Con) Next.PM(NotUse) Fail.PM(DisHW+Abort)\
|26:Lot(Con) Next.PM(NotUse) Fail.PM(SA+DisHW+Abort)\
|27:Lot(End) Next.PM(Stop) Fail.PM(Disable)\
|28:Lot(End) Next.PM(Go)   Fail.PM(SA+Disable)\
|29:Lot(End) Next.PM(Skip) Fail.PM(SA+Disable)\
|30:Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)\
|31:Lot(End) Next.PM(Stop) Fail.PM(DisableHW)\
|32:Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)\
|33:Lot(End) Next.PM(Go)   Fail.PM(Dis+Abort)\
|34:Lot(End) Next.PM(Skip) Fail.PM(Dis+Abort)\
|35:Lot(End) Next.PM(Stop) Fail.PM(Dis+Abort)\
|36:Lot(End) Next.PM(Go)   Fail.PM(SA+Dis+Abort)\
|37:Lot(End) Next.PM(Skip) Fail.PM(SA+Dis+Abort)\
|38:Lot(End) Next.PM(Stop) Fail.PM(SA+Dis+Abort)\
|39:Lot(End) Next.PM(Stop) Fail.PM(DisHW+Abort)\
|40:Lot(End) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)\
|41:Lot(End) Next.PM(NotUse) Fail.PM(Disable+Abort)\
|42:Lot(End) Next.PM(NotUse) Fail.PM(SA+Dis+Abort)\
|43:Lot(End) Next.PM(NotUse) Fail.PM(DisHW+Abort)\
|44:Lot(End) Next.PM(NotUse) Fail.PM(SA+DisHW+Abort)\
|45:Lot(Con) Next.PM(Skip) Fail.PM(DisHW)\
|46:Lot(End) Next.PM(Skip) Fail.PM(DisHW)" );
*/

			strcpy( Buffer , "0:Abort Only" );
			strcat( Buffer , "|1: Lot(Con) Next.PM(Go)   Fail.PM(Use)" );
			strcat( Buffer , "|2: Lot(Con) Next.PM(Go)   Fail.PM(Disable)" );
			strcat( Buffer , "|3: Lot(Con) Next.PM(Skip) Fail.PM(Use)" );
			strcat( Buffer , "|4: Lot(Con) Next.PM(Skip) Fail.PM(Disable)" );
			strcat( Buffer , "|5: Lot(End) Next.PM(Go)   Fail.PM(Use)" );
			strcat( Buffer , "|6: Lot(End) Next.PM(Go)   Fail.PM(Disable)" );
			strcat( Buffer , "|7: Lot(End) Next.PM(Skip) Fail.PM(Use)" );
			strcat( Buffer , "|8: Lot(End) Next.PM(Skip) Fail.PM(Disable)" );
			strcat( Buffer , "|9: Lot(Con) Next.PM(Stop) Fail.PM(Disable)" );
			strcat( Buffer , "|10:Lot(Con) Next.PM(Go)   Fail.PM(SA+Disable)" );
			strcat( Buffer , "|11:Lot(Con) Next.PM(Skip) Fail.PM(SA+Disable)" );
			strcat( Buffer , "|12:Lot(Con) Next.PM(Stop) Fail.PM(SA+Disable)" );
			strcat( Buffer , "|13:Lot(Con) Next.PM(Stop) Fail.PM(DisableHW)" );
			strcat( Buffer , "|14:Lot(Con) Next.PM(Stop) Fail.PM(SA+DisableHW)" );
			strcat( Buffer , "|15:Lot(Con) Next.PM(Go)   Fail.PM(Dis+Abort)" );
			strcat( Buffer , "|16:Lot(Con) Next.PM(Skip) Fail.PM(Dis+Abort)" );
			strcat( Buffer , "|17:Lot(Con) Next.PM(Stop) Fail.PM(Dis+Abort)" );
			strcat( Buffer , "|18:Lot(Con) Next.PM(Go)   Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|19:Lot(Con) Next.PM(Skip) Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|20:Lot(Con) Next.PM(Stop) Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|21:Lot(Con) Next.PM(Stop) Fail.PM(DisHW+Abort)" );
			strcat( Buffer , "|22:Lot(Con) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)" );
			strcat( Buffer , "|23:Lot(Con) Next.PM(NotUse) Fail.PM(Disable+Abort)" );
			strcat( Buffer , "|24:Lot(Con) Next.PM(NotUse) Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|25:Lot(Con) Next.PM(NotUse) Fail.PM(DisHW+Abort)" );
			strcat( Buffer , "|26:Lot(Con) Next.PM(NotUse) Fail.PM(SA+DisHW+Abort)" );
			strcat( Buffer , "|27:Lot(End) Next.PM(Stop) Fail.PM(Disable)" );
			strcat( Buffer , "|28:Lot(End) Next.PM(Go)   Fail.PM(SA+Disable)" );
			strcat( Buffer , "|29:Lot(End) Next.PM(Skip) Fail.PM(SA+Disable)" );
			strcat( Buffer , "|30:Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)" );
			strcat( Buffer , "|31:Lot(End) Next.PM(Stop) Fail.PM(DisableHW)" );
			strcat( Buffer , "|32:Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)" );
			strcat( Buffer , "|33:Lot(End) Next.PM(Go)   Fail.PM(Dis+Abort)" );
			strcat( Buffer , "|34:Lot(End) Next.PM(Skip) Fail.PM(Dis+Abort)" );
			strcat( Buffer , "|35:Lot(End) Next.PM(Stop) Fail.PM(Dis+Abort)" );
			strcat( Buffer , "|36:Lot(End) Next.PM(Go)   Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|37:Lot(End) Next.PM(Skip) Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|38:Lot(End) Next.PM(Stop) Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|39:Lot(End) Next.PM(Stop) Fail.PM(DisHW+Abort)" );
			strcat( Buffer , "|40:Lot(End) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)" );
			strcat( Buffer , "|41:Lot(End) Next.PM(NotUse) Fail.PM(Disable+Abort)" );
			strcat( Buffer , "|42:Lot(End) Next.PM(NotUse) Fail.PM(SA+Dis+Abort)" );
			strcat( Buffer , "|43:Lot(End) Next.PM(NotUse) Fail.PM(DisHW+Abort)" );
			strcat( Buffer , "|44:Lot(End) Next.PM(NotUse) Fail.PM(SA+DisHW+Abort)" );
			strcat( Buffer , "|45:Lot(Con) Next.PM(Skip) Fail.PM(DisHW)" );	 // 2006.06.28
			strcat( Buffer , "|46:Lot(End) Next.PM(Skip) Fail.PM(DisHW)" );	 // 2006.06.28
			
			//---------------------
			// User Area
			//---------------------
			strcat( Buffer , "|1000:User-Style-001" );

			//=======================================================================================================
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select Fail Scenario" , "Select" , Buffer , &Res ) ) {
				//=======================================================================================================
				// 2006.02.03
				//=======================================================================================================
				if ( Res > FAIL_LIKE_DEFAULT_LAST_ITEM ) {
					Res = Res - FAIL_LIKE_DEFAULT_LAST_ITEM - 1 + 1000;
				}
				//=======================================================================================================
				if ( Res != FAILSNR_DATA1 ) {
					FAILSNR_DATA1 = Res;
					InvalidateRect( hdlg , NULL , TRUE );
				}
			}
			return TRUE;
		case IDC_READY_USE_B2 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Fail Result" , "Select" , "0:Fail|1:Success|2:NotRun|3:Skip" , &FAILSNR_DATA2 ); // 2015.04.30
//			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Fail Result" , "Select" , "0:Fail|1:Success|2:NotRun|3:Skip|0:Fail(TMLast)|1:Success(TMLast)|2:NotRun(TMLast)|3:Skip(TMLast)" , &FAILSNR_DATA2 ); // 2015.04.30 // 2018.09.27
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Fail Result" , "Select" , "0:Fail|1:Success|2:NotRun|3:Skip|0:Fail(TMLast)|1:Success(TMLast)|2:NotRun(TMLast)|3:Skip(TMLast)|0:Fail(ReRun)|1:Success(ReRun)|2:NotRun(ReRun)|3:Skip(ReRun)" , &FAILSNR_DATA2 ); // 2015.04.30 // 2018.09.27
			return TRUE;
		case IDC_READY_USE_B3 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Abort Result" , "Select" , "0:Fail|1:Success|2:NotRun|3:Skip" , &FAILSNR_DATA3 );
			return TRUE;
		case IDC_READY_USE_B4 :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Stop Scenario" , "Select" , "0:N/A|1:PM(Run) NextPM.Skip|2:PM(Abort) NextPM.Go|3:PM(Abort) NextPM.Skip" , &FAILSNR_DATA4 ); // 2012.02.09
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Stop Scenario" , "Select" , "0:N/A|1:PM(Run) NextPM.Skip|2:PM(Abort) NextPM.Go|3:PM(Abort) NextPM.Skip|4:N/A(AllAbort PrcsRemain)|5:PM(Run) NextPM.Skip(AllAbort PrcsRemain)|6:PM(Abort) NextPM.Go(AllAbort PrcsRemain)|7:PM(Abort) NextPM.Skip(AllAbort PrcsRemain)" , &FAILSNR_DATA4 ); // 2012.02.09
			return TRUE;
		case IDC_READY_USE_B5 :
			KWIN_GUI_MODAL_DIGITAL_BOX1( FALSE , hdlg , "Select Success Result" , "Select" , "0:Success|1:NotRun|2:Skip|3:Fail|4:Skip(All)" , &FAILSNR_DATA5 );
			return TRUE;
		case IDOK :		FAILSNR_RESULT = TRUE;	EndDialog( hdlg , 0 );	return TRUE;
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
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_FAILSNR_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int i;
	static int  TEMP_USE1[MAX_CHAMBER];
	static int  TEMP_USE2[MAX_CHAMBER];
	static int  TEMP_USE3[MAX_CHAMBER];
	static int  TEMP_USE4[MAX_CHAMBER];
	static int  TEMP_USE5;
	static int  TEMP_USE6[MAX_CHAMBER]; // 2003.09.25
	static int  TEMP_USE7; // 2004.05.14
	static BOOL Control;
	char  *szString[] = { "PM" , "Fail Go" , "Fail Res" , "Abort Res" , "Stop Res" , "Sucs Res" };
	int    szSize[6]  = {  50  ,       355 ,        90  ,          90 ,         90 ,         91 };
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
			TEMP_USE1[ i+PM1 ] = _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO(i+PM1);
			TEMP_USE2[ i+PM1 ] = _i_SCH_PRM_GET_MRES_FAIL_SCENARIO(i+PM1);
			TEMP_USE3[ i+PM1 ] = _i_SCH_PRM_GET_MRES_ABORT_SCENARIO(i+PM1);
			TEMP_USE4[ i+PM1 ] = _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO(i+PM1);
			TEMP_USE6[ i+PM1 ] = _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO(i+PM1);
		}
		TEMP_USE5 = _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO();
		//
		TEMP_USE7 = _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP(); // 2004.05.14
		//
		KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 6 , szString , szSize );
		ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		//
		KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_PM_CHAMBER_DEPTH );
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
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
									switch( TEMP_USE1[ i+PM1 ] ) {
									case FAIL_LIKE_ABORT					:	sprintf( szString , "Abort Only" );	break;
									case FAIL_LIKE_CONT_GO_USE				:	sprintf( szString , "Lot(Cont) Next.PM(Go) Fail.PM(Use)" );	break;
									case FAIL_LIKE_CONT_GO_DISABLE			:	sprintf( szString , "Lot(Cont) Next.PM(Go) Fail.PM(Disable)" );	break;
									case FAIL_LIKE_CONT_OUT_USE				:	sprintf( szString , "Lot(Cont) Next.PM(Skip) Fail.PM(Use)" );	break;
									case FAIL_LIKE_CONT_OUT_DISABLE			:	sprintf( szString , "Lot(Cont) Next.PM(Skip) Fail.PM(Disable)" );	break;
									case FAIL_LIKE_END_GO_USE				:	sprintf( szString , "Lot(End) Next.PM(Go) Fail.PM(Use)" );	break;
									case FAIL_LIKE_END_GO_DISABLE			:	sprintf( szString , "Lot(End) Next.PM(Go) Fail.PM(Disable)" );	break;
									case FAIL_LIKE_END_OUT_USE				:	sprintf( szString , "Lot(End) Next.PM(Skip) Fail.PM(Use)" );	break;
									case FAIL_LIKE_END_OUT_DISABLE			:	sprintf( szString , "Lot(End) Next.PM(Skip) Fail.PM(Disable)" );	break;
									//
									case FAIL_LIKE_CONT_STOP_DISABLE		:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(Disable)" );	break;
									case FAIL_LIKE_CONT_GO_DISABLESA		:	sprintf( szString , "Lot(Cont) Next.PM(Go) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_CONT_OUT_DISABLESA		:	sprintf( szString , "Lot(Cont) Next.PM(Skip) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLESA		:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLEHW		:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(DisableHW)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLEHWSA	:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+DisableHW)" );	break;
									case FAIL_LIKE_CONT_GO_DISABLE_ABORT	:	sprintf( szString , "Lot(Cont) Next.PM(Go) Fail.PM(Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_OUT_DISABLE_ABORT	:	sprintf( szString , "Lot(Cont) Next.PM(Skip) Fail.PM(Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLE_ABORT	:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_GO_DISABLESA_ABORT	:	sprintf( szString , "Lot(Cont) Next.PM(Go) Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_OUT_DISABLESA_ABORT	:	sprintf( szString , "Lot(Cont) Next.PM(Skip) Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLESA_ABORT:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLEHW_ABORT:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(DisHW+Abort)" );	break;
									case FAIL_LIKE_CONT_STOP_DISABLEHWSA_ABORT:	sprintf( szString , "Lot(Cont) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)" );	break;
									case FAIL_LIKE_CONT_NOTUSE_DISABLE_ABORT:	sprintf( szString , "Lot(Cont)Next.PM(NotUse)Fail.PM(Disable+Abort)" );	break;
									case FAIL_LIKE_CONT_NOTUSE_DISABLESA_ABORT:	sprintf( szString , "Lot(Cont)Next.PM(NotUse)Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_CONT_NOTUSE_DISABLEHW_ABORT:	sprintf( szString , "Lot(Cont)Next.PM(NotUse)Fail.PM(DisHW+Abort)" );	break;
									case FAIL_LIKE_CONT_NOTUSE_DISABLEHWSA_ABORT:sprintf( szString , "Lot(Cont)Next.PM(NotUse)Fail.PM(SA+DisHW+Abort)" );	break;
									//
									case FAIL_LIKE_END_STOP_DISABLE			:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(Disable)" );	break;
									case FAIL_LIKE_END_GO_DISABLESA			:	sprintf( szString , "Lot(End) Next.PM(Go) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_END_OUT_DISABLESA		:	sprintf( szString , "Lot(End) Next.PM(Skip) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_END_STOP_DISABLESA		:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_END_STOP_DISABLEHW		:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(DisableHW)" );	break;
									case FAIL_LIKE_END_STOP_DISABLEHWSA		:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(SA+Disable)" );	break;
									case FAIL_LIKE_END_GO_DISABLE_ABORT		:	sprintf( szString , "Lot(End) Next.PM(Go) Fail.PM(Dis+Abort)" );	break;
									case FAIL_LIKE_END_OUT_DISABLE_ABORT	:	sprintf( szString , "Lot(End) Next.PM(Skip) Fail.PM(Dis+Abort)" );	break;
									case FAIL_LIKE_END_STOP_DISABLE_ABORT	:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(Dis+Abort)" );	break;
									case FAIL_LIKE_END_GO_DISABLESA_ABORT	:	sprintf( szString , "Lot(End) Next.PM(Go) Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_END_OUT_DISABLESA_ABORT	:	sprintf( szString , "Lot(End) Next.PM(Skip) Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_END_STOP_DISABLESA_ABORT	:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_END_STOP_DISABLEHW_ABORT	:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(DisHW+Abort)" );	break;
									case FAIL_LIKE_END_STOP_DISABLEHWSA_ABORT:	sprintf( szString , "Lot(End) Next.PM(Stop) Fail.PM(SA+DisHW+Abort)" );	break;
									case FAIL_LIKE_END_NOTUSE_DISABLE_ABORT:	sprintf( szString , "Lot(End) Next.PM(NotUse)Fail.PM(Disable+Abort)" );	break;
									case FAIL_LIKE_END_NOTUSE_DISABLESA_ABORT:	sprintf( szString , "Lot(End) Next.PM(NotUse)Fail.PM(SA+Dis+Abort)" );	break;
									case FAIL_LIKE_END_NOTUSE_DISABLEHW_ABORT:	sprintf( szString , "Lot(End) Next.PM(NotUse)Fail.PM(DisHW+Abort)" );	break;
									case FAIL_LIKE_END_NOTUSE_DISABLEHWSA_ABORT:sprintf( szString , "Lot(End) Next.PM(NotUse)Fail.PM(SA+DisHW+Abort)" );	break;
									//
									case FAIL_LIKE_CONT_OUT_DISABLEHW		:	sprintf( szString , "Lot(Cont) Next.PM(Skip) Fail.PM(DisableHW)" );	break; // 2006.06.28
									case FAIL_LIKE_END_OUT_DISABLEHW		:	sprintf( szString , "Lot(End)  Next.PM(Skip) Fail.PM(DisableHW)" );	break; // 2006.06.28
									//
									case FAIL_LIKE_USER_STYLE_1				:	sprintf( szString , "User-Style-001" );	break; // 2006.02.03
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 2 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
									switch( TEMP_USE2[ i+PM1 ] ) {
									case MRES_LIKE_FAIL		:	sprintf( szString , "Fail" );	break;
									case MRES_LIKE_SUCCESS	:	sprintf( szString , "Success" );	break;
									case MRES_LIKE_NOTRUN	:	sprintf( szString , "NotRun" );	break;
									case MRES_LIKE_SKIP		:	sprintf( szString , "Skip" );	break;
										//
										// 2015.04.30
										//
									case 4					:	sprintf( szString , "Fail(TMLast)" );	break;
									case 5					:	sprintf( szString , "Success(TMLast)" );	break;
									case 6					:	sprintf( szString , "NotRun(TMLast)" );	break;
									case 7					:	sprintf( szString , "Skip(TMLast)" );	break;
										//
										//
										// 2018.09.27
										//
									case 8					:	sprintf( szString , "Fail(ReRun)" );	break;
									case 9					:	sprintf( szString , "Success(ReRun)" );	break;
									case 10					:	sprintf( szString , "NotRun(ReRun)" );	break;
									case 11					:	sprintf( szString , "Skip(ReRun)" );	break;
										//
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 3 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
									switch( TEMP_USE3[ i+PM1 ] ) {
									case MRES_LIKE_FAIL		:	sprintf( szString , "Fail" );	break;
									case MRES_LIKE_SUCCESS	:	sprintf( szString , "Success" );	break;
									case MRES_LIKE_NOTRUN	:	sprintf( szString , "NotRun" );	break;
									case MRES_LIKE_SKIP		:	sprintf( szString , "Skip" );	break;
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 4 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
									switch( TEMP_USE4[ i+PM1 ] ) {
									case STOP_LIKE_NONE				:	sprintf( szString , "N/A" );	break;
									case STOP_LIKE_PM_NONE_GO_OUT	:	sprintf( szString , "PM(Run) NextPM.Skip" );	break;
									case STOP_LIKE_PM_ABORT_GO_NEXT	:	sprintf( szString , "PM(Abort) NextPM.Go" );	break;
									case STOP_LIKE_PM_ABORT_GO_OUT	:	sprintf( szString , "PM(Abort) NextPM.Skip" );	break;
									case STOP_LIKE_NONE_ALLABORT_REMAINPRCS				:	sprintf( szString , "N/A(AllAbort PrcsRemain)" );	break; // 2012.02.08
									case STOP_LIKE_PM_NONE_GO_OUT_ALLABORT_REMAINPRCS	:	sprintf( szString , "PM(Run) NextPM.Skip(AllAbort PrcsRemain)" );	break; // 2012.02.08
									case STOP_LIKE_PM_ABORT_GO_NEXT_ALLABORT_REMAINPRCS	:	sprintf( szString , "PM(Abort) NextPM.Go(AllAbort PrcsRemain)" );	break; // 2012.02.08
									case STOP_LIKE_PM_ABORT_GO_OUT_ALLABORT_REMAINPRCS	:	sprintf( szString , "PM(Abort) NextPM.Skip(AllAbort PrcsRemain)" );	break; // 2012.02.08
									}
								}
								else {
									sprintf( szString , "-" );
								}
								break;
							case 5 :
								if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
									switch( TEMP_USE6[ i+PM1 ] ) {
									case MRES_SUCCESS_LIKE_SUCCESS	:	sprintf( szString , "" );	break;
									case MRES_SUCCESS_LIKE_NOTRUN	:	sprintf( szString , "NotRun" );	break;
									case MRES_SUCCESS_LIKE_SKIP		:	sprintf( szString , "Skip" );	break;
									case MRES_SUCCESS_LIKE_FAIL		:	sprintf( szString , "Fail" );	break;
									case MRES_SUCCESS_LIKE_ALLSKIP	:	sprintf( szString , "Skip(All)" );	break; // 2005.04.23
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
							wsprintf( szString, "PM%d", lpdi->item.iItem + 1 );
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
					if  ( _i_SCH_PRM_GET_MODULE_MODE(i+PM1) ) {
						FAILSNR_RESULT = FALSE;
						FAILSNR_DATA1  = TEMP_USE1[i+PM1];
						FAILSNR_DATA2  = TEMP_USE2[i+PM1];
						FAILSNR_DATA3  = TEMP_USE3[i+PM1];
						FAILSNR_DATA4  = TEMP_USE4[i+PM1];
						FAILSNR_DATA5  = TEMP_USE6[i+PM1];
						hSWnd = GetDlgItem( hdlg , IDC_LIST_COMMON_BOX );
						GoModalDialogBoxParam( GETHINST(hSWnd) , MAKEINTRESOURCE( IDD_FAILSNR_SUB_PAD ) , hSWnd , Gui_IDD_FAILSNR_SUB_PAD_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
						if ( FAILSNR_RESULT ) {
							TEMP_USE1[i+PM1] = FAILSNR_DATA1;
							TEMP_USE2[i+PM1] = FAILSNR_DATA2;
							TEMP_USE3[i+PM1] = FAILSNR_DATA3;
							TEMP_USE4[i+PM1] = FAILSNR_DATA4;
							TEMP_USE6[i+PM1] = FAILSNR_DATA5;
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
		//
		//----------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDYES  , TEMP_USE5 , "N/A|FAILURE|DISABLE|FAILURE.DISABLE" , "N/A" ); // 2007.07.24
		//----------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDNO   , TEMP_USE7 , "N/A|Auto STOP" , "N/A" ); // 2007.07.24 // 2017.12.07
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDNO   , TEMP_USE7 , "N/A|Auto STOP|ErrorX|Auto STOP+ErrorX" , "N/A" ); // 2007.07.24 // 2017.12.07
		//----------------------
		//
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( _i_SCH_PRM_GET_FAIL_PROCESS_SCENARIO( i+PM1 ) != TEMP_USE1[ i+PM1 ] ) break;
		}
		if ( i >= MAX_PM_CHAMBER_DEPTH ) {
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				if ( _i_SCH_PRM_GET_MRES_FAIL_SCENARIO( i+PM1 ) != TEMP_USE2[ i+PM1 ] ) break;
			}
			if ( i >= MAX_PM_CHAMBER_DEPTH ) {
				for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
					if ( _i_SCH_PRM_GET_MRES_ABORT_SCENARIO( i+PM1 ) != TEMP_USE3[ i+PM1 ] ) break;
				}
				if ( i >= MAX_PM_CHAMBER_DEPTH ) {
					for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
						if ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i+PM1 ) != TEMP_USE6[ i+PM1 ] ) break;
					}
					if ( i >= MAX_PM_CHAMBER_DEPTH ) {
						for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
							if ( _i_SCH_PRM_GET_STOP_PROCESS_SCENARIO( i+PM1 ) != TEMP_USE4[ i+PM1 ] ) break;
						}
						if ( i >= MAX_PM_CHAMBER_DEPTH ) {
							if ( TEMP_USE5 == _i_SCH_PRM_GET_MRES_ABORT_ALL_SCENARIO() ) {
								if ( TEMP_USE7 == _i_SCH_PRM_GET_DISABLE_MAKE_HOLE_GOTOSTOP() ) { // 2004.05.14
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
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDOK :
			if ( Control ) return TRUE;
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				 _i_SCH_PRM_SET_FAIL_PROCESS_SCENARIO( i+PM1 , TEMP_USE1[ i+PM1 ] );
				 _i_SCH_PRM_SET_MRES_FAIL_SCENARIO( i+PM1 , TEMP_USE2[ i+PM1 ] );
				 _i_SCH_PRM_SET_MRES_ABORT_SCENARIO( i+PM1 , TEMP_USE3[ i+PM1 ] );
				 _i_SCH_PRM_SET_MRES_SUCCESS_SCENARIO( i+PM1 , TEMP_USE6[ i+PM1 ] ); // 2003.09.25
				 _i_SCH_PRM_SET_STOP_PROCESS_SCENARIO( i+PM1 , TEMP_USE4[ i+PM1 ] );
			}
			_i_SCH_PRM_SET_MRES_ABORT_ALL_SCENARIO( TEMP_USE5 );
			_i_SCH_PRM_SET_DISABLE_MAKE_HOLE_GOTOSTOP( TEMP_USE7 ); // 2004.05.14
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			EndDialog( hdlg , 0 );
			return TRUE;

		case IDYES :
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &TEMP_USE5 );
			break;

		case IDNO :
//			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 1 , &TEMP_USE7 ); // 2017.12.07
			KWIN_GUI_SELECT_DATA_INCREASE( Control , hdlg , 3 , &TEMP_USE7 ); // 2017.12.07
			break;

		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
