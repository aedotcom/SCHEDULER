//===========================================================================================================
#include "INF_default.h"
//===========================================================================================================
#include "INF_CimSeqnc.h"
#include "INF_EQ_Enum.h"
#include "INF_User_Parameter.h"
//===========================================================================================================
#include "INF_sch_CommonUser.h"
//===========================================================================================================
#include "DLL_Interface.h"
//===========================================================================================================
//===========================================================================================================
BOOL CHECK_USER_KEY_CODE2( int , char * , int * );
//===========================================================================================================
//===========================================================================================================
BOOL _SCH_COMMON_LOCKEKEY_CHECK( int alg , int algsub , int *slinfo , BOOL userdllload ) {
//	char retkeyid[32]; // 2016.11.05
	char retkeyid[128]; // 2016.11.05
	//====================================================================================================================
	//====================================================================================================================
	if ( !CHECK_USER_KEY_CODE2( 1 /* CTC */ , retkeyid , slinfo ) ) { // 2008.07.31
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program [%d][%s]\n" , *slinfo , retkeyid );
		return FALSE;
	}
	//====================================================================================================================
	switch ( *slinfo ) {
	case 1 : // (001) Default- TM Style(All)
	case 2 : // (002) Default- TM Style(200)
#ifdef _SCH_MIF
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default Style])\n" , *slinfo );
		return FALSE;
#else
		if ( ( alg != 0 ) || _SCH_PRM_GET_MODULE_MODE( FM ) ) {
			MessageBeep( MB_ICONHAND );
			_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default- TM Style])\n" , *slinfo );
			return FALSE;
		}
#endif
		break;
	case 3 : // (003) Default- FM+TM Style(All)
	case 4 : // (004) Default- FM+TM Style(200)
#ifdef _SCH_MIF
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default Style])\n" , *slinfo );
		return FALSE;
#else
		if ( ( alg != 0 ) || !_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2006.04.13
			MessageBeep( MB_ICONHAND );
			_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default - FM+TM Style])\n" , *slinfo );
			return FALSE;
		}
		else { // 2006.04.13
			if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) { // 2006.12.21 // 2008.02.28
				MessageBeep( MB_ICONHAND );
				_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default - FM+TM Style])\n" , *slinfo );
				return FALSE;
			}
		}
#endif
		break;
	case 5 : // (005) FM+TM(Twin) Style(All)
	case 6 : // (006) FM+TM(Twin) Style(200)
#ifdef _SCH_MIF
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default Style])\n" , *slinfo );
		return FALSE;
#else
		if ( ( alg != 0 ) || ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) ) {
			MessageBeep( MB_ICONHAND );
			_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) FM+TM(Twin) Style])\n" , *slinfo );
			return FALSE;
		}
#endif
		break;
	case 7 : // (007) FM+TM(Inlign) Style(All)
	case 8 : // (008) FM+TM(Inlign) Style(200)
#ifdef _SCH_MIF
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default Style])\n" , *slinfo );
		return FALSE;
#else
		if ( alg != 4 ) {
			MessageBeep( MB_ICONHAND );
			_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) FM+TM(Inlign) Style])\n" , *slinfo );
			return FALSE;
		}
#endif
		break;
	case 9 :  // (009) FM+TM(Double) Style(All)
	case 10 : // (010) FM+TM(Double) Style(200)
#ifdef _SCH_MIF
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) Default Style])\n" , *slinfo );
		return FALSE;
#else
		if ( alg != 6 ) {
			MessageBeep( MB_ICONHAND );
			_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for [(%03d) FM+TM(Double) Style])\n" , *slinfo );
			return FALSE;
		}
#endif
		break;
	case 15 : // (015) Customize
#ifdef _SCH_MIF
		if ( !userdllload ) {
			MessageBeep( MB_ICONHAND );
			_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for Customize Style)\n" );
			return FALSE;
		}
#else
		MessageBeep( MB_ICONHAND );
		_IO_CIM_PRINTF( "=> Not Licensed Scheduler Program(Just Licensed for Customize Style)\n" );
		return FALSE;
#endif
		break;
	}
	//====================================================================================================================
	return TRUE;
}
//===========================================================================================================

void _SCH_COMMON_GUI_INTERFACE_ITEM_DISPLAY( int alg , int mode , int offset , HWND hdlg , int guiindex , char *Message ) {
	char list[256];
	int  x;
	// 0	:	N/A
	// 1	:	Put Priority Check
	// 2	:	Get Priority Check
	// 3	:	BM Scheduling Factor
	// 4	:	Switching Side
	// 5	:	Next Pick
	// 6	:	Prev Pick
	//
	strcpy( Message , "" );
	//
	switch ( _SCH_COMMON_GUI_INTERFACE( 0 /* Item(0) / Data(1) / control(2) / dataget(3) */ , alg , mode , offset , Message , list , &x ) ) {
	case -1 :
		if ( guiindex >= 0 ) KWIN_Item_Hide( hdlg , guiindex );
		break;
	case 0 :
		break;
	case 1 :
		if ( guiindex >= 0 ) KWIN_Item_String_Display( hdlg , guiindex , Message );
		break;
	default :
		if ( guiindex >= 0 ) KWIN_Item_String_Display( hdlg , guiindex , "" );
		break;
	}
}

void _SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( int alg , int mode , int offset , HWND hdlg , int guiindex , int currdata ) {
	char Message[256];
	char list[512];
	int  x;
	// 0	:	N/A
	// 1	:	Put Priority Check
	// 2	:	Get Priority Check
	// 3	:	BM Scheduling Factor
	// 4	:	Switching Side
	// 5	:	Next Pick
	// 6	:	Prev Pick
	//
	x = offset; // 2010.07.22
	//
	switch ( _SCH_COMMON_GUI_INTERFACE( 1 /* Item(0) / Data(1) / control(2) / dataget(3) */ , alg , mode , currdata , Message , list , &x ) ) {
	case -1 :
		KWIN_Item_Hide( hdlg , guiindex );
		break;
	case 0 :
		break;
	case 1 :
		KWIN_GUI_INT_STRING_DISPLAY( hdlg , guiindex , currdata , list , Message );
		break;
	case 2 : // 2008.05.23
		KWIN_Item_String_Display( hdlg , guiindex , Message );
		break;
	default :
		KWIN_Item_String_Display( hdlg , guiindex , "" );
		break;
	}
}

//===========================================================================================================
//===========================================================================================================

void _SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( int alg , int mode , int offset_controlpass , HWND hdlg , int *currdata ) {
	int  RetData;
	char Message[256];
	char list[512];
	//
	RetData = *currdata; // 2008.05.23
	//
	switch ( _SCH_COMMON_GUI_INTERFACE( 2 /* Item(0) / Data(1) / control(2) / dataget(3) */ , alg , mode , offset_controlpass , Message , list , &RetData ) ) {
	case -1 :
		break;
		//
	case 0 :
		break;
		//
	case 1 :
		KWIN_GUI_MODAL_DIGITAL_BOX1( offset_controlpass % 10 , hdlg , Message , "Select" , list , currdata );
		break;
		//
	case 2 :
		*currdata = RetData;
		InvalidateRect( hdlg , NULL , TRUE );
		break;
		//
	case 3 :
		KWIN_GUI_SELECT_DATA_INCREASE( offset_controlpass % 10 , hdlg , RetData , currdata );
		break;
		//
	case 4 :
		if ( offset_controlpass % 10 ) return;
		if ( MODAL_DIGITAL_BOX1( hdlg , Message , "Select" , list , &RetData ) ) {
//			if ( _SCH_INF_DLL_CONTROL_GUI_INTERFACE( 4 /* user */ , alg , mode , offset_controlpass , Message , list , &RetData ) ) { // 2015.03.03
			if ( _SCH_COMMON_GUI_INTERFACE( 4 /* user */ , alg , mode , offset_controlpass , Message , list , &RetData ) ) { // 2015.03.03
				if ( RetData != *currdata ) {
					*currdata = RetData;
					InvalidateRect( hdlg , NULL , TRUE );
				}
			}
		}
		break;

	default :
		break;
	}
}

void _SCH_COMMON_GUI_INTERFACE_DATA_GET( int alg , int mode , int offset , int currdata , char *data ) {
	char Message[256];
	char list[256];
	int  x;
	// 0	:	N/A
	// 1	:	Put Priority Check : N/U
	// 2	:	Get Priority Check : N/U
	// 3	:	BM Scheduling Factor : N/U
	// 4	:	Switching Side
	// 5	:	Next Pick
	// 6	:	Precv Pick

	x = offset; // 2010.07.22
	//
	strcpy( data , "" );
	//
	switch ( _SCH_COMMON_GUI_INTERFACE( 3 /* Item(0) / Data(1) / control(2) / dataget(3) */ , alg , mode , currdata , Message , list , &x ) ) {
	case -1 :
		break;
	case 0 :
		break;
	case 1 :
		strcpy( data , Message );
		break;
	default :
		break;
	}
}







