#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "sch_Multi_ALIC.h"
#include "system_tag.h"
#include "resource.h"

#include "sch_Commonuser.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_SYSTEM_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
	int Res;
//	char buffer[32]; // 2007.09.17 // 2010.07.21
	char buffer[256]; // 2007.09.17 // 2010.07.21
//	static int  TEMP_USE[31]; // 2011.04.27
	static int  TEMP_USE[32]; // 2011.04.27
	static BOOL Control;

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
		TEMP_USE[ 0 ] = _i_SCH_PRM_GET_UTIL_CHECK_WAFER_SYNCH();
		TEMP_USE[ 1 ] = _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE();
		TEMP_USE[ 2 ] = _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL();
		TEMP_USE[ 3 ] = _i_SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL();
		TEMP_USE[ 4 ] = _i_SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME();
		TEMP_USE[ 5 ] = _i_SCH_PRM_GET_UTIL_SCH_START_OPTION();
		TEMP_USE[ 6 ] = _i_SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL();
		TEMP_USE[ 7 ] = _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS();
		TEMP_USE[ 8 ] = _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT();
		TEMP_USE[ 9 ] = _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN();
		TEMP_USE[ 10 ] = _i_SCH_PRM_GET_UTIL_ABORT_MESSAGE_USE_POINT();
		TEMP_USE[ 11 ] = _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT();
		TEMP_USE[ 12 ] = _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT();
		TEMP_USE[ 13 ] = _i_SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME();
		TEMP_USE[ 14 ] = _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK();
		TEMP_USE[ 15 ] = _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME();
		TEMP_USE[ 16 ] = _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME();
		TEMP_USE[ 17 ] = _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK();
		TEMP_USE[ 18 ] = _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH();
		TEMP_USE[ 19 ] = _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE();
		TEMP_USE[ 20 ] = _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND(); // 2003.01.08
		TEMP_USE[ 21 ] = _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP(); // 2003.01.11
		TEMP_USE[ 22 ] = _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE(); // 2003.06.11
		TEMP_USE[ 23 ] = _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE(); // 2003.10.09
		TEMP_USE[ 24 ] = _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING(); // 2004.02.19
		TEMP_USE[ 25 ] = _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS(); // 2004.05.11
		TEMP_USE[ 26 ] = _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER(); // 2004.06.24
		TEMP_USE[ 27 ] = _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR(); // 2004.08.13
		TEMP_USE[ 28 ] = _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE(); // 2005.12.01
		TEMP_USE[ 29 ] = _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING(); // 2006.03.28
		TEMP_USE[ 30 ] = _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING(); // 2008.02.28
		//
		TEMP_USE[ 31 ] = _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2(); // 2011.04.27
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SW_CONTROL , _i_SCH_PRM_GET_DFIX_MODULE_SW_CONTROL() , "FALSE|TRUE|TRUE(E)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_Item_Int_Display( hdlg , IDC_MAX_CASS_SLOT  , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() );
		//
		_SCH_COMMON_GUI_INTERFACE_ITEM_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 1 , 0 , hdlg , IDC_CHAMBER_PUT_PR_ITEM , buffer );
		_SCH_COMMON_GUI_INTERFACE_ITEM_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 2 , 0 , hdlg , IDC_CHAMBER_GET_PR_ITEM , buffer );
		_SCH_COMMON_GUI_INTERFACE_ITEM_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 3 , 0 , hdlg , IDC_SW_SCHEDULING_FACTOR_ITEM , buffer );
		//
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		Res = 0;
		if ( TEMP_USE[ 0 ] != _i_SCH_PRM_GET_UTIL_CHECK_WAFER_SYNCH() ) Res = 1;
		if ( TEMP_USE[ 1 ] != _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() ) Res = 1;
		if ( TEMP_USE[ 2 ] != _i_SCH_PRM_GET_UTIL_TM_GATE_SKIP_CONTROL() ) Res = 1;
		if ( TEMP_USE[ 3 ] != _i_SCH_PRM_GET_UTIL_TM_MOVE_SR_CONTROL() ) Res = 1;
		if ( TEMP_USE[ 4 ] != _i_SCH_PRM_GET_UTIL_FIRST_MODULE_PUT_DELAY_TIME() ) Res = 1;
		if ( TEMP_USE[ 5 ] != _i_SCH_PRM_GET_UTIL_SCH_START_OPTION() ) Res = 1;
		if ( TEMP_USE[ 6 ] != _i_SCH_PRM_GET_UTIL_FM_MOVE_SR_CONTROL() ) Res = 1;
		if ( TEMP_USE[ 7 ] != _i_SCH_PRM_GET_UTIL_LOOP_MAP_ALWAYS() ) Res = 1;
		if ( TEMP_USE[ 8 ] != _i_SCH_PRM_GET_UTIL_SCH_LOG_DIRECTORY_FORMAT() ) Res = 1;
		if ( TEMP_USE[ 9 ] != _i_SCH_PRM_GET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN() ) Res = 1;
		if ( TEMP_USE[ 10 ] != _i_SCH_PRM_GET_UTIL_ABORT_MESSAGE_USE_POINT() ) Res = 1;
		if ( TEMP_USE[ 11 ] != _i_SCH_PRM_GET_UTIL_CANCEL_MESSAGE_USE_POINT() ) Res = 1;
		if ( TEMP_USE[ 12 ] != _i_SCH_PRM_GET_UTIL_END_MESSAGE_SEND_POINT() ) Res = 1;
		if ( TEMP_USE[ 13 ] != _i_SCH_PRM_GET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME() ) Res = 1;
		if ( TEMP_USE[ 14 ] != _i_SCH_PRM_GET_UTIL_CHAMBER_PUT_PR_CHECK() ) Res = 1;
		if ( TEMP_USE[ 15 ] != _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME() ) Res = 1;
		if ( TEMP_USE[ 16 ] != _i_SCH_PRM_GET_UTIL_FAL_MULTI_WAITTIME() ) Res = 1;
		if ( TEMP_USE[ 17 ] != _i_SCH_PRM_GET_UTIL_CHAMBER_GET_PR_CHECK() ) Res = 1;
		if ( TEMP_USE[ 18 ] != _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_SWITCH() ) Res = 1;
		if ( TEMP_USE[ 19 ] != _i_SCH_PRM_GET_UTIL_CM_SUPPLY_MODE() ) Res = 1;
		if ( TEMP_USE[ 20 ] != _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() ) Res = 1; // 2003.01.08
		if ( TEMP_USE[ 21 ] != _i_SCH_PRM_GET_UTIL_START_PARALLEL_CHECK_SKIP() ) Res = 1; // 2003.01.11
		if ( TEMP_USE[ 22 ] != _i_SCH_PRM_GET_UTIL_WAFER_SUPPLY_MODE() ) Res = 1; // 2003.06.11
		if ( TEMP_USE[ 23 ] != _i_SCH_PRM_GET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE() ) Res = 1; // 2003.10.09
		if ( TEMP_USE[ 24 ] != _i_SCH_PRM_GET_UTIL_BMEND_SKIP_WHEN_RUNNING() ) Res = 1; // 2004.02.19
		if ( TEMP_USE[ 25 ] != _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() ) Res = 1; // 2004.05.11
		if ( TEMP_USE[ 26 ] != _i_SCH_PRM_GET_UTIL_MESSAGE_USE_WHEN_ORDER() ) Res = 1; // 2004.06.24
		if ( TEMP_USE[ 27 ] != _i_SCH_PRM_GET_UTIL_SW_BM_SCHEDULING_FACTOR() ) Res = 1; // 2004.08.13
		if ( TEMP_USE[ 28 ] != _i_SCH_PRM_GET_UTIL_FIC_MULTI_TIMEMODE() ) Res = 1; // 2005.12.01
		if ( TEMP_USE[ 29 ] != _i_SCH_PRM_GET_UTIL_PMREADY_SKIP_WHEN_RUNNING() ) Res = 1; // 2006.03.28
		if ( TEMP_USE[ 30 ] != _i_SCH_PRM_GET_UTIL_ADAPTIVE_PROGRESSING() ) Res = 1; // 2008.02.28
		//
		if ( TEMP_USE[ 31 ] != _i_SCH_PRM_GET_UTIL_FIC_MULTI_WAITTIME2() ) Res = 1; // 2008.02.28

		if ( Res == 0 ) KWIN_Item_Disable( hdlg , IDOK );
		else            KWIN_Item_Enable( hdlg , IDOK );

		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_WAFER_SYNCH , TEMP_USE[ 0 ] , "Disable|Enable|Enable_Detail" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			//---------------------------------------------------------------
//			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_MAP_TMFREE , TEMP_USE[ 1 ] , "Multiple|Single+TM FREE|Single|Multi+TM FREE|Single+TMfree(A)|Multi+TMfree(A)" , "????" ); // 2007.07.24 // 2008.03.19
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_MAP_TMFREE , TEMP_USE[ 1 ] , "Multiple|Single+TM FREE|Single|Multi+TM FREE|Single+TMfree(A)|Multi+TMfree(A)|M/S+TM FREE|Multi+Single|M/S+TMfree(A)|Single+BM XPLC|Multi+BM XPLC|M/S+BM XPLC" , "????" ); // 2007.07.24 // 2008.03.19
			//---------------------------------------------------------------
		}
		else {
			//---------------------------------------------------------------
//			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_MAP_TMFREE , TEMP_USE[ 1 ] , "Single|Single+FM FREE|Multiple|Multi+FM FREE|Single+FMfree(A)|Multi+FMfree(A)" , "????" ); // 2007.07.24 // 2008.03.19
			KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_MAP_TMFREE , TEMP_USE[ 1 ] , "Single|Single+FM FREE|Multiple|Multi+FM FREE|Single+FMfree(A)|Multi+FMfree(A)|M/S+FM FREE|Multi+Single|M/S+FMfree(A)|Single+BM XPLC|Multi+BM XPLC|M/S+BM XPLC" , "????" ); // 2007.07.24 // 2008.03.19
			//---------------------------------------------------------------
		}
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_GATE_TM_CONTROL		, TEMP_USE[  2 ] , "Use|Skip|Close-Skip|Open-Skip|CloseBM-Skip|Skip(A)|Close-Skip(A)|Open-Skip(A)|CloseBM-Skip(A)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_MOVE_SR_TM_CONTROL	, TEMP_USE[  3 ] , "N/A|Use" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_Item_Int_Display( hdlg , IDC_FIRST_PUT_DELAY		, TEMP_USE[  4 ] );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_START_MSG_STYLE		, TEMP_USE[  5 ] , "Original|Always Concurrent|Always Continue|Always Sequential|Always Sequential2" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_MOVE_SR_FEM_CONTROL	, TEMP_USE[  6 ] , "N/A|Use" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_LOOP_MAP_ALWAYS		, TEMP_USE[  7 ] , "N/A|Use" , "????" ); // 2007.07.24 // 2012.04.12
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_LOOP_MAP_ALWAYS		, TEMP_USE[  7 ] , "N/A|MapUse|LotLogSeparate|MapUse+LotLogSeparate" , "????" ); // 2007.07.24 // 2012.04.12
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_LOG_APPEND		, TEMP_USE[  8 ] , "Time|Time_JID|Time_LOT|Time_MID|Time_JID_LOT|Time_JID_MID|Time_LOT_JID|Time_LOT_MID|Time_MID_JID|Time_MID_LOT|Time_JID_LOT_MID|Time_JID_MID_LOT|Time_LOT_JID_MID|Time_LOT_MID_JID|Time_MID_JID_LOT|Time_MID_LOT_JID|Time(S)|Time_JID(S)|Time_LOT(S)|Time_MID(S)|Time_JID_LOT(S)|Time_JID_MID(S)|Time_LOT_JID(S)|Time_LOT_MID(S)|Time_MID_JID(S)|Time_MID_LOT(S)|Time_JID_L_M(S)|Time_JID_M_L(S)|Time_LOT_J_M(S)|Time_LOT_M_J(S)|Time_MID_J_L(S)|Time_MID_L_J(S)|Time(D)|Time_JID(D)|Time_LOT(D)|Time_MID(D)|Time_JID_LOT(D)|Time_JID_MID(D)|Time_LOT_JID(D)|Time_LOT_MID(D)|Time_MID_JID(D)|Time_MID_LOT(D)|Time_JID_L_M(D)|Time_JID_M_L(D)|Time_LOT_J_M(D)|Time_LOT_M_J(D)|Time_MID_J_L(D)|Time_MID_L_J(D)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_CHANGE_TO_RUN	, TEMP_USE[  9 ] , "N/A|Change Run Method" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_ABORT_MESSAGE	, TEMP_USE[ 10 ] , "Always|Wait Confirm" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_CANCEL_MESSAGE	, TEMP_USE[ 11 ] , "Always|Run + Wait(Free)|Wait(All) Only|Wait(Free) Only" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_END_MESSAGE		, TEMP_USE[ 12 ] , "Lot Finish|Prcs Finish|Prcs Finish(LP)|Prcs Finish(LPB)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_Item_Int_Display( hdlg , IDC_CASSETTE_DEV_DELAY , TEMP_USE[ 13 ] );
		//---------------------------------------------------------------
		_SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 1 , 0 , hdlg , IDC_CHAMBER_PUT_PR , TEMP_USE[ 14 ] );
		//---------------------------------------------------------------
		if ( ( TEMP_USE[ 28 ] >= 5 ) && ( TEMP_USE[ 31 ] > 0 ) ) {
			if      ( TEMP_USE[ 15 ] < -20 ) {
				sprintf( buffer , "I.?|%d" , TEMP_USE[ 31 ] );
			}
			else if ( TEMP_USE[ 15 ] < -10 ) {
				sprintf( buffer , "IS.%c|%d" , -11 - TEMP_USE[ 15 ] + 'A' , TEMP_USE[ 31 ] );
			}
			else if ( TEMP_USE[ 15 ] < 0 ) {
				sprintf( buffer , "I.%c|%d" , -1 - TEMP_USE[ 15 ] + 'A' , TEMP_USE[ 31 ] );
			}
			else {
				sprintf( buffer , "%d|%d" , TEMP_USE[ 15 ] , TEMP_USE[ 31 ] );
			}
			KWIN_Item_String_Display( hdlg , IDC_SCH_MFIC_WAITTIME , buffer );
		}
		else {
			if      ( TEMP_USE[ 15 ] < -20 ) {
				KWIN_Item_String_Display( hdlg , IDC_SCH_MFIC_WAITTIME , "INF.?" );
			}
			else if ( TEMP_USE[ 15 ] < -10 ) {
				sprintf( buffer , "INF.S.%c" , -11 - TEMP_USE[ 15 ] + 'A' );
				KWIN_Item_String_Display( hdlg , IDC_SCH_MFIC_WAITTIME , buffer );
			}
			else if ( TEMP_USE[ 15 ] < 0 ) {
				sprintf( buffer , "INF.%c" , -1 - TEMP_USE[ 15 ] + 'A' );
				KWIN_Item_String_Display( hdlg , IDC_SCH_MFIC_WAITTIME , buffer );
			}
			else {
				KWIN_Item_Int_Display( hdlg , IDC_SCH_MFIC_WAITTIME , TEMP_USE[ 15 ] );
			}
		}
		//
		KWIN_Item_Int_Display( hdlg , IDC_SCH_MFAL_WAITTIME , TEMP_USE[ 16 ] );
		//---------------------------------------------------------------
		_SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 2 , 0 , hdlg , IDC_CHAMBER_GET_PR , TEMP_USE[ 17 ] );
		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_SWITCH_MESSAGE	, TEMP_USE[ 18 ] , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|[E]Always Use|[E]Skip(But Gate Use)|[E]Skip All|[E]Skip(GateUse).X2|[E]Skip All.X2" , "????" ); // 2007.07.24 // 2013.01.22
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_SWITCH_MESSAGE	, TEMP_USE[ 18 ] , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|[E]Always Use|[E]Skip(But Gate Use)|[E]Skip All|[E]Skip(GateUse).X2|[E]Skip All.X2|SWITCH" , "????" ); // 2007.07.24 // 2013.01.22 // 2013.10.31
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_SWITCH_MESSAGE	, TEMP_USE[ 18 ] , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|[E]Always Use|[E]Skip(But Gate Use)|[E]Skip All|[E]Skip(GateUse).X2|[E]Skip All.X2|SWITCH|SWITCH(A)" , "????" ); // 2007.07.24 // 2013.01.22 // 2013.10.31 // 2018.04.06
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_SWITCH_MESSAGE	, TEMP_USE[ 18 ] , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|[E]Always Use|[E]Skip(But Gate Use)|[E]Skip All|[E]Skip(GateUse).X2|[E]Skip All.X2|SWITCH|SWITCH(A)|SW.MSG(X)" , "????" ); // 2007.07.24 // 2013.01.22 // 2013.10.31 // 2018.04.06
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CASSETTE_SUP_DELAY	, TEMP_USE[ 19 ] , "Delay.Random|NotDelay.Random|Delay.Sequence|NotDelay.Sequenc|Delay.Balance|NotDelay.Balance|Delay.Balance(R)|NotDelay.Balance(R)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_PRE_PROCESS_DEPAND	, TEMP_USE[ 20 ] , "Thread.Normal|Depend.Normal|Thread(R).Normal|Thread.Balance|Depend.Balance|Thread(R).Balance|Thread.Normal.LT|Depend.Normal.LT|Thread(R).Norml.LT|Thread.Balance.LT|Depend.Balance.LT|Thread(R).Balan.LT" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_START_PARALLEL_CHECK_SKIP	, TEMP_USE[ 21 ] , "N/A|1 / 234|2 / 134|3 / 124|4 / 123|12 / 34|13 / 24|14 / 23|PM|PM[Succ-Skip(X)]|PM(R)|PM(R)[Succ-Skip(X)]" , "????" ); // 2007.07.24
		//---------------------------------------------------------------

//		A->	A	A->	B
//------------------------
//		1	1	1	1
//		1	1	1	25
//		1	1	25	1
//		1	1	25	25
//		25	25	1	1
//		25	25	1	25
//		25	25	25	1
//		25	25	25	25

		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_WAFER_SUP_MODE			, TEMP_USE[ 22 ] , "First:First|First:First.Rev|First:Last.Rev|First:Last|Last:First|Last:First.Rev|Last:Last.Rev|Last:Last" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_CLUSTER_INCLUDE_DISABLE	, TEMP_USE[ 23 ] , "N/A|+(Dis)|+(DisHW)|+(Dis,HW)|+(Dis)(W)|+(DisHW)(W)|+(Dis,HW)(W)|+(Dis)(REX)|+(DisHW)(REX)|+(Dis,HW)(REX)|+(Dis)(W)(REX)|+(DisHW)(W)(REX)|+(Dis,HW)(W)(REX)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_BMEND_MESSAGE		, TEMP_USE[ 24 ] , "Always|Skip|MulSub" , "????" ); // 2007.07.24 // 2008.04.14
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_BMEND_MESSAGE		, TEMP_USE[ 24 ] , "Always|Skip|MulSub|Always(Reset)|Skip(Reset)|MulSub(Reset)" , "????" ); // 2007.07.24 // 2008.04.14
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_LOT_PREPOST_LOG		, TEMP_USE[ 25 ] , "N/A|Pre|Post|All|DataLog|Pre+Data|Post+Data|All+Data|DataLog+S|Pre+Data+S|Post+Data+S|All+Data+S" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_ORDER_MESSAGE		, TEMP_USE[ 26 ] , "N/A|Use1|Use2|Use3|Use4|Use5|Use6|Use7|Use8|Use9" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		_SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( SYSTEM_GUI_ALG_STYLE_GET() , 3 , 0 , hdlg , IDC_SW_SCHEDULING_FACTOR , TEMP_USE[ 27 ] );
		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_MFIC_TIMEMODE		, TEMP_USE[ 28 ] , "N/A|Dec|Inc|Dec(%)|Inc(%)" , "????" ); // 2007.07.24 // 2011.04.27
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_MFIC_TIMEMODE		, TEMP_USE[ 28 ] , "N/A|Dec|Inc|Dec(%)|Inc(%)|N/A(M)|Dec(M)|Inc(M)|D(%)(M)|I(%)(M)" , "????" ); // 2007.07.24 // 2011.04.27
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_PMREADY_MESSAGE		, TEMP_USE[ 29 ] , "Always|Skip" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
//		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_ADAPT_PROGRESS		, TEMP_USE[ 30 ] , "Off|On|On(S)|On(L)" , "????" ); // 2008.02.29 // 2010.12.17
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , IDC_SCH_ADAPT_PROGRESS		, TEMP_USE[ 30 ] , "Off|On|On(S)|On(L)|Off(LPre-AP)|On(LPre-AP)|On(S)(LPre-AP)|On(L)(LPre-AP)" , "????" ); // 2008.02.29 // 2010.12.17
		//---------------------------------------------------------------
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDRETRY :
			if ( _i_SCH_MODULE_Need_Do_Multi_FIC() ) {
				GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE( IDD_MCOOL_PAD ) , hdlg , Gui_IDD_MCOOL_PAD_Proc , (LPARAM) NULL ); // 2007.07.02
			}
			break;

		case IDC_WAFER_SYNCH :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Wafer Synch Check" , "Select" , "Disable|Enable|Enable_Detail" , &(TEMP_USE[ 0 ]) );
			break;

		case IDC_MAP_TMFREE :
			if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
//				KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cassette Mapping Time" , "Select" , "Multiple|With Single & When TM Free|With Single Only|Multiple & When TM Free|With Single & (Auto) TM Free|Multiple & (Auto) TM Free" , &(TEMP_USE[ 1 ]) ); // 2008.03.19
				KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cassette Mapping Time" , "Select" , "Multiple|With Single & When TM Free|With Single Only|Multiple & When TM Free|With Single & (Auto) TM Free|Multiple & (Auto) TM Free|Multi+Single & When TM Free|Multi+Single Only|Multi+Single & (Auto) TM Free|With Single & When BM Not Placed|Multiple & When BM Not Placed|Multi+Single & When BM Not Placed" , &(TEMP_USE[ 1 ]) ); // 2008.03.19
			}
			else {
//				KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cassette Mapping Time" , "Select" , "With Single Only|With Single & When FM Free|Multiple|Multiple & When FM Free|With Single & (Auto) FM Free|Multiple & (Auto) FM Free" , &(TEMP_USE[ 1 ]) ); // 2008.03.19
				KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cassette Mapping Time" , "Select" , "With Single Only|With Single & When FM Free|Multiple|Multiple & When FM Free|With Single & (Auto) FM Free|Multiple & (Auto) FM Free|Multi+Single & When FM Free|Multi+Single Only|Multi+Single & (Auto) FM Free|With Single & When BM Not Placed|Multiple & When BM Not Placed|Multi+Single & When BM Not Placed" , &(TEMP_USE[ 1 ]) ); // 2008.03.19
			}
			break;

		case IDC_GATE_TM_CONTROL :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( hdlg , "TM Gate Control" , "Select" , "Use|Skip" , &(TEMP_USE[ 2 ]) ); // 2006.10.31
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "TM Gate Control" , "Select" , "Use|Skip|Close-Skip|Open-Skip|CloseBM-Skip|Skip(A)|Close-Skip(A)|Open-Skip(A)|CloseBM-Skip(A)" , &(TEMP_USE[ 2 ]) ); // 2006.10.31
			break;

		case IDC_MOVE_SR_TM_CONTROL :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "TM Move Send/Recv Control" , "Select" , "N/A|Use" , &(TEMP_USE[ 3 ]) );
			break;

		case IDC_FIRST_PUT_DELAY :
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "First Module Put Delay Time" , "Select(sec)" , 0 , 9999 , &(TEMP_USE[ 4 ]) );
			break;

		case IDC_START_MSG_STYLE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Start Message Style" , "Select" , "Original|Always Concurrent|Always Continue|Always Sequential|Always Sequential2" , &(TEMP_USE[ 5 ]) );
			break;

		case IDC_MOVE_SR_FEM_CONTROL :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "FM Move Send/Recv Control" , "Select" , "N/A|Use" , &(TEMP_USE[ 6 ]) );
			break;

		case IDC_LOOP_MAP_ALWAYS :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "When Loop test , Map Always?" , "Select" , "N/A|Use" , &(TEMP_USE[ 7 ]) ); // 2012.04.12
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "When Loop test , Map Always(Log)?" , "Select" , "N/A|MapUse|LotLogSeparate|MapUse+LotLogSeparate" , &(TEMP_USE[ 7 ]) ); // 2012.04.12
			break;

		case IDC_SCH_LOG_APPEND :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg ,
				"Select Log Directory Type?" ,
				"Select" ,
				"Time|Time_JID|Time_LOT|Time_MID|Time_JID_LOT|Time_JID_MID|Time_LOT_JID|Time_LOT_MID|Time_MID_JID|Time_MID_LOT|Time_JID_LOT_MID|Time_JID_MID_LOT|Time_LOT_JID_MID|Time_LOT_MID_JID|Time_MID_JID_LOT|Time_MID_LOT_JID|Time(S)|Time_JID(S)|Time_LOT(S)|Time_MID(S)|Time_JID_LOT(S)|Time_JID_MID(S)|Time_LOT_JID(S)|Time_LOT_MID(S)|Time_MID_JID(S)|Time_MID_LOT(S)|Time_JID_LOT_MID(S)|Time_JID_MID_LOT(S)|Time_LOT_JID_MID(S)|Time_LOT_MID_JID(S)|Time_MID_JID_LOT(S)|Time_MID_LOT_JID(S)|Time(D)|Time_JID(D)|Time_LOT(D)|Time_MID(D)|Time_JID_LOT(D)|Time_JID_MID(D)|Time_LOT_JID(D)|Time_LOT_MID(D)|Time_MID_JID(D)|Time_MID_LOT(D)|Time_JID_LOT_MID(D)|Time_JID_MID_LOT(D)|Time_LOT_JID_MID(D)|Time_LOT_MID_JID(D)|Time_MID_JID_LOT(D)|Time_MID_LOT_JID(D)" ,
				&(TEMP_USE[ 8 ]) );
			break;

		case IDC_SCH_CHANGE_TO_RUN :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Processed Wafer Change for Run Method?" , "Select" , "N/A|Change Run Method" , &(TEMP_USE[ 9 ]) );
			break;

		case IDC_SCH_ABORT_MESSAGE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Abort Message Use Point?" , "Select" , "Always|When Wait Confirm with Stop or End" , &(TEMP_USE[ 10 ]) );
			break;

		case IDC_SCH_CANCEL_MESSAGE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cancel Message Use Point?" , "Select" , "Always|Running + Wait When All Free|Wait Always|Wait When All Free" , &(TEMP_USE[ 11 ]) );
			break;

		case IDC_SCH_END_MESSAGE :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "End Message Send Point?" , "Select" , "After Lot Finish|After Process Not Remain" , &(TEMP_USE[ 12 ]) ); // 2007.04.02
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "End Message Send Point?" , "Select" , "After Lot Finish|After Process Not Remain|After Process Not Remain and Lot Post Run|After Process Not Remain and Lot Post Run(Return BM)" , &(TEMP_USE[ 12 ]) ); // 2007.04.02
			break;

		case IDC_CASSETTE_DEV_DELAY :
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Cassette Supply Deviation Time" , "Select(sec)" , 0 , 9999 , &(TEMP_USE[ 13 ]) );
			break;

		case IDC_CHAMBER_PUT_PR :
			//
			_SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( SYSTEM_GUI_ALG_STYLE_GET() , 1 , Control , hdlg , &(TEMP_USE[ 14 ])  );
			//
			break;

		case IDC_CHAMBER_GET_PR :
			//
			_SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( SYSTEM_GUI_ALG_STYLE_GET() , 2 , Control , hdlg , &(TEMP_USE[ 17 ]) );
			//
			break;

		case IDC_SCH_MFIC_WAITTIME :
//			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Multi Cooler Waiting Time" , "Select(sec)" , 0 , 9999 , &(TEMP_USE[ 15 ]) ); // 2007.07.03
//			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Multi Cooler Waiting Time" , "Select(sec)" , -2 , 99999 , &(TEMP_USE[ 15 ]) ); // 2007.07.03 // 2007.09.17
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Multi Cooler Waiting Time" , "Select(sec)" , -20 , 99999 , &(TEMP_USE[ 15 ]) ); // 2007.07.03 // 2007.09.17
			//
			if ( TEMP_USE[ 28 ] >= 5 ) { // 2011.04.27
				KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Multi Cooler Waiting Time(2)" , "Select(sec)" , 0 , 99999 , &(TEMP_USE[ 31 ]) ); // 2011.04.27
			}
			//
			break;

		case IDC_SCH_MFAL_WAITTIME :
			KWIN_GUI_MODAL_DIGITAL_BOX2( Control , hdlg , "Multi Aligner Waiting Time(Simulation)" , "Select(sec)" , 0 , 9999 , &(TEMP_USE[ 16 ]) );
			break;

		case IDC_SCH_SWITCH_MESSAGE :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Post/Pre Message Use When Switch?" , "Select" , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|Extend:Always Use|Extend:Skip(But Gate Use)|Extend:Skip All|Extend:Skip(GateUse).X2|Extend:Skip All.X2" , &(TEMP_USE[ 18 ]) ); // 2013.01.12
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Post/Pre Message Use When Switch?" , "Select" , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|Extend:Always Use|Extend:Skip(But Gate Use)|Extend:Skip All|Extend:Skip(GateUse).X2|Extend:Skip All.X2|SWITCH" , &(TEMP_USE[ 18 ]) ); // 2013.01.12 // 2013.10.31
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Post/Pre Message Use When Switch?" , "Select" , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|Extend:Always Use|Extend:Skip(But Gate Use)|Extend:Skip All|Extend:Skip(GateUse).X2|Extend:Skip All.X2|SWITCH|SWITCH(A)" , &(TEMP_USE[ 18 ]) ); // 2013.01.12 // 2013.10.31 // 2018.04.06
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Post/Pre Message Use When Switch?" , "Select" , "Always Use|Skip(But Gate Use)|Skip All|Skip(GateUse).X2|Skip All.X2|Extend:Always Use|Extend:Skip(But Gate Use)|Extend:Skip All|Extend:Skip(GateUse).X2|Extend:Skip All.X2|SWITCH|SWITCH(A)|SWITCH.MESSAGE(X)" , &(TEMP_USE[ 18 ]) ); // 2013.01.12 // 2013.10.31 // 2018.04.06
			break;

		case IDC_CASSETTE_SUP_DELAY :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cassette supply mode?" , "Select" , "Delay.Random|Not Delay.Random|Delay.Sequence|Not Delay.Sequence|Delay.Balance|Not Delay.Balance|Delay.Balance(R)|Not Delay.Balance(R)" , &(TEMP_USE[ 19 ]) );
			break;

		case IDC_PRE_PROCESS_DEPAND :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Pre.Post Process Run Type?" , "Select" , "Thread.Normal|Depend at Main.Normal|Thread(Remain module Info).Normal|Thread.Balance|Depend at Main.Balance|Thread(Remain module Info).Balance|Thread.Normal.LotThread|Depend at Main.Normal.LotThread|Thread(Remain module Info).Normal.LotThread|Thread.Balance.LotThread|Depend at Main.Balance.LotThread|Thread(Remain module Info).Balance.LotThread" , &(TEMP_USE[ 20 ]) );
			break;

		case IDC_START_PARALLEL_CHECK_SKIP :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "When Continue Start , Check Skip side style?" , "Select" , "N/A|1 / 234|2 / 134|3 / 124|4 / 123|12 / 34|13 / 24|14 / 23|PM(Alone)|PM(Success-Result-Skip is (x))|PM(R)(Alone)|PM(R)(Success-Result-Skip is (x))" , &(TEMP_USE[ 21 ]) );
			break;

		case IDC_WAFER_SUP_MODE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Wafer Supply Mode?" , "Select" , "First:First|First:First.Rev|First:Last.Rev|First:Last|Last:First|Last:First.Rev|Last:Last.Rev|Last:Last" , &(TEMP_USE[ 22 ]) );
			break;

		case IDC_CLUSTER_INCLUDE_DISABLE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Cluster Include when Disable Mode?" , "Select" , "N/A|Include Disable|Include DisableHW|Include Disable,DisableHW|Include Disable(No.Wait)|Include DisableHW(No.Wait)|Include Disable,DisableHW(No.Wait)|Include Disable(ReadyEndX)|Include DisableHW(ReadyEndX)|Include Disable,DisableHW(ReadyEndX)|Include Disable(No.Wait)(ReadyEndX)|Include DisableHW(No.Wait)(ReadyEndX)|Include Disable,DisableHW(No.Wait)(ReadyEndX)" , &(TEMP_USE[ 23 ]) );
			break;

		case IDC_SCH_BMEND_MESSAGE :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( hdlg , "BM END Message Send when running already?" , "Select" , "Always|Skip" , &(TEMP_USE[ 24 ]) ); // 2006.10.24
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "BM END Message Send when running already?" , "Select" , "Always|Skip|Multiple SubCall" , &(TEMP_USE[ 24 ]) ); // 2006.10.24 // 2008.04.14
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "BM END Message Send when running already?" , "Select" , "Always|Skip|Multiple SubCall|Always(Reset)|Skip(Reset)|Multiple SubCall(Reset)" , &(TEMP_USE[ 24 ]) ); // 2006.10.24 // 2008.04.14
			break;

		case IDC_SCH_LOT_PREPOST_LOG :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Lot Pre/Post Process Data Log?" , "Select" , "N/A|Pre|Post|All(Pre/Post)|DataLog|Pre+DataLog|Post+DataLog|All(Pre/Post)+DataLog|DataLog+StepChange|Pre+DataLog+StepChange|Post+DataLog+StepChange|All(Pre/Post)+DataLog+StepChange" , &(TEMP_USE[ 25 ]) );
			break;

		case IDC_SCH_ORDER_MESSAGE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Order Message Use When Operate?" , "Select" , "Not Use|Use1|Use2|Use3|Use4|Use5|Use6|Use7|Use8|Use9" , &(TEMP_USE[ 26 ]) );
			break;

		case IDC_SW_SCHEDULING_FACTOR :
			//
			_SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( SYSTEM_GUI_ALG_STYLE_GET() , 3 , Control , hdlg , &(TEMP_USE[ 27 ]) );
			//
			break;

		case IDC_SCH_MFIC_TIMEMODE :
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Multi Cooling Timer Mode?" , "Select" , "N/A|Decrease|Increase|Decrease(Percent)|Increase(Percent)" , &(TEMP_USE[ 28 ]) ); // 2011.04.27
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Multi Cooling Timer Mode?" , "Select" , "N/A|Decrease|Increase|Decrease(Percent)|Increase(Percent)|N/A(M)|Decrease(M)|Increase(M)|Decrease(Percent)(M)|Increase(Percent)(M)" , &(TEMP_USE[ 28 ]) ); // 2011.04.27
			break;

		case IDC_SCH_PMREADY_MESSAGE :
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "PM READY Message Send when running already?" , "Select" , "Always|Skip" , &(TEMP_USE[ 29 ]) );
			break;

		case IDC_SCH_ADAPT_PROGRESS : // 2008.02.29
//			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Use Adaptive Progressing?" , "Select" , "Off|On|On(Start Part Only)|On(LotPre Only)" , &(TEMP_USE[ 30 ]) ); // 2011.01.18
			KWIN_GUI_MODAL_DIGITAL_BOX1( Control , hdlg , "Use Adaptive Progressing?" , "Select" , "Off|On|On(S)|On(L)|Off(LPre-AP)|On(LPre-AP)|On(S)(LPre-AP)|On(L)(LPre-AP)" , &(TEMP_USE[ 30 ]) ); // 2011.01.18
			break;

		case IDOK :
			if ( Control ) return TRUE;
			_i_SCH_PRM_SET_UTIL_CHECK_WAFER_SYNCH( TEMP_USE[ 0 ] );
			_i_SCH_PRM_SET_UTIL_MAPPING_WHEN_TMFREE( TEMP_USE[ 1 ] );
			_i_SCH_PRM_SET_UTIL_TM_GATE_SKIP_CONTROL( TEMP_USE[ 2 ] );
			_i_SCH_PRM_SET_UTIL_TM_MOVE_SR_CONTROL( TEMP_USE[ 3 ] );
			_i_SCH_PRM_SET_UTIL_FIRST_MODULE_PUT_DELAY_TIME( TEMP_USE[ 4 ] );
			_i_SCH_PRM_SET_UTIL_SCH_START_OPTION( TEMP_USE[ 5 ] );
			_i_SCH_PRM_SET_UTIL_FM_MOVE_SR_CONTROL( TEMP_USE[ 6 ] );
			_i_SCH_PRM_SET_UTIL_LOOP_MAP_ALWAYS( TEMP_USE[ 7 ] );
			_i_SCH_PRM_SET_UTIL_SCH_LOG_DIRECTORY_FORMAT( TEMP_USE[ 8 ] );
			_i_SCH_PRM_SET_UTIL_SCH_CHANGE_PROCESS_WAFER_TO_RUN( TEMP_USE[ 9 ] );
			_i_SCH_PRM_SET_UTIL_ABORT_MESSAGE_USE_POINT( TEMP_USE[ 10 ] );
			_i_SCH_PRM_SET_UTIL_CANCEL_MESSAGE_USE_POINT( TEMP_USE[ 11 ] );
			_i_SCH_PRM_SET_UTIL_END_MESSAGE_SEND_POINT( TEMP_USE[ 12 ] );
			_i_SCH_PRM_SET_UTIL_CASSETTE_SUPPLY_DEVIATION_TIME( TEMP_USE[ 13 ] );
			_i_SCH_PRM_SET_UTIL_CHAMBER_PUT_PR_CHECK( TEMP_USE[ 14 ] );
			_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME( TEMP_USE[ 15 ] );
			_i_SCH_PRM_SET_UTIL_FAL_MULTI_WAITTIME( TEMP_USE[ 16 ] );
			_i_SCH_PRM_SET_UTIL_CHAMBER_GET_PR_CHECK( TEMP_USE[ 17 ] );
			_i_SCH_PRM_SET_UTIL_MESSAGE_USE_WHEN_SWITCH( TEMP_USE[ 18 ] );
			_i_SCH_PRM_SET_UTIL_CM_SUPPLY_MODE( TEMP_USE[ 19 ] );
			_i_SCH_PRM_SET_UTIL_PREPOST_PROCESS_DEPAND( TEMP_USE[ 20 ] ); // 2003.01.08
			_i_SCH_PRM_SET_UTIL_START_PARALLEL_CHECK_SKIP( TEMP_USE[ 21 ] ); // 2003.01.11
			_i_SCH_PRM_SET_UTIL_WAFER_SUPPLY_MODE( TEMP_USE[ 22 ] ); // 2003.06.11
			_i_SCH_PRM_SET_UTIL_CLUSTER_INCLUDE_FOR_DISABLE( TEMP_USE[ 23 ] ); // 2003.10.09
			_i_SCH_PRM_SET_UTIL_BMEND_SKIP_WHEN_RUNNING( TEMP_USE[ 24 ] ); // 2004.02.19
			_i_SCH_PRM_SET_UTIL_LOT_LOG_PRE_POST_PROCESS( TEMP_USE[ 25 ] ); // 2004.05.11
			_i_SCH_PRM_SET_UTIL_MESSAGE_USE_WHEN_ORDER( TEMP_USE[ 26 ] ); // 2004.06.24
			_i_SCH_PRM_SET_UTIL_SW_BM_SCHEDULING_FACTOR( TEMP_USE[ 27 ] ); // 2004.08.13
			_i_SCH_PRM_SET_UTIL_FIC_MULTI_TIMEMODE( TEMP_USE[ 28 ] ); // 2005.12.01
			_i_SCH_PRM_SET_UTIL_PMREADY_SKIP_WHEN_RUNNING( TEMP_USE[ 29 ] ); // 2006.03.28
			_i_SCH_PRM_SET_UTIL_ADAPTIVE_PROGRESSING( TEMP_USE[ 30 ] ); // 2008.02.28
			//
			_i_SCH_PRM_SET_UTIL_FIC_MULTI_WAITTIME2( TEMP_USE[ 31 ] ); // 2011.04.27
			//
			GUI_SAVE_PARAMETER_DATA( 1 );
			//---------------------------------
			//
			Set_RunPrm_IO_Setting( 5 );
			Set_RunPrm_IO_Setting( 10 ); // 2003.02.05
			Set_RunPrm_IO_Setting( 12 ); // 2006.07.21
			Set_RunPrm_IO_Setting( 13 ); // 2005.08.16
			Set_RunPrm_IO_Setting( 14 ); // 2005.11.29
			Set_RunPrm_IO_Setting( 25 ); // 2015.03.25
			//
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
