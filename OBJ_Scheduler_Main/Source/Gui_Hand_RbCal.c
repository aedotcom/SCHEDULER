#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "sch_prm_FBTPM.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
double Gui_For_Scheduler_RobotCal_Ext_Data( HWND hdlg , int Arm , int Station , double run , int inc ) {
	double data;
	if ( inc == 0 ) {
		if      ( Arm == 100 ) {
			data = ROBOT_FM_GET_POSITION( 0 , RB_ANIM_EXTEND );
		}
		else if ( Arm == 101 ) {
			if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_SEPERATE( 0 ) ) {
				data = ROBOT_FM_GET_POSITION( 0 , RB_ANIM_EXTEND );
			}
			else {
				data = ROBOT_FM_GET_POSITION( 0 , RB_ANIM_EXTEND2 );
			}
		}
		else {
			if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) ) {
				data = ROBOT_GET_EXTEND_POSITION( GUI_TM_DISPLAY_GET() , 0 );
			}
			else {
				data = ROBOT_GET_EXTEND_POSITION( GUI_TM_DISPLAY_GET() , Arm );
			}
		}
	}
	else {
		if      ( inc == 1 ) data = run + 0.01;
		else if ( inc == 2 ) data = run + 0.05;
		else if ( inc == 3 ) data = run + 0.1;
		else if ( inc == 4 ) data = run + 0.5;
		else if ( inc == 5 ) data = run + 1;
		else if ( inc == 6 ) data = run + 5;
		else if ( inc == 7 ) data = run + 10;
		else if ( inc == 99 ) data = run;
		if      ( inc == -1 ) data = run - 0.01;
		else if ( inc == -2 ) data = run - 0.05;
		else if ( inc == -3 ) data = run - 0.1;
		else if ( inc == -4 ) data = run - 0.5;
		else if ( inc == -5 ) data = run - 1;
		else if ( inc == -6 ) data = run - 5;
		else if ( inc == -7 ) data = run - 10;
		if ( data < -100 ) data = -100;  // 2003.01.14
		if ( data >  100 ) data =  100;  // 2003.01.14
		//
		if      ( Arm == 100 ) {
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND , data );
		}
		else if ( Arm == 101 ) {
			if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_SEPERATE( 0 ) ) {
				ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND , data );
			}
			else {
				ROBOT_FM_SET_POSITION( 0 , RB_ANIM_EXTEND2 , data );
			}
		}
		else {
			if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) ) {
				ROBOT_SET_EXTEND_POSITION( GUI_TM_DISPLAY_GET() , 0 , data );
			}
			else {
				ROBOT_SET_EXTEND_POSITION( GUI_TM_DISPLAY_GET() , Arm , data );
			}
		}
	}
	KWIN_Item_Double_Display( hdlg , IDC_EXT_VALUE  , data , 2 );
	return data;
}
//
double Gui_For_Scheduler_RobotCal_Rot_Data( HWND hdlg , int Arm , int Station , double run , int inc ) {
	double data;
	if ( inc == 0 ) {
		if ( Arm >= 100 ) {
			data = ROBOT_FM_GET_POSITION( 0 , RB_ANIM_ROTATE );
		}
		else {
			data = ROBOT_GET_ROTATE_POSITION( GUI_TM_DISPLAY_GET() );
		}
	}
	else {
		if      ( inc == 1 ) data = run + 0.01;
		else if ( inc == 2 ) data = run + 0.05;
		else if ( inc == 3 ) data = run + 0.1;
		else if ( inc == 4 ) data = run + 0.5;
		else if ( inc == 5 ) data = run + 1;
		else if ( inc == 6 ) data = run + 5;
		else if ( inc == 7 ) data = run + 10;
		else if ( inc == 99 ) data = run;
		if      ( inc == -1 ) data = run - 0.01;
		else if ( inc == -2 ) data = run - 0.05;
		else if ( inc == -3 ) data = run - 0.1;
		else if ( inc == -4 ) data = run - 0.5;
		else if ( inc == -5 ) data = run - 1;
		else if ( inc == -6 ) data = run - 5;
		else if ( inc == -7 ) data = run - 10;
		if ( data < 0 )    data = 359;  // 2003.01.14
		if ( data >= 360 ) data =   0;  // 2003.01.14
		//
		if ( Arm >= 100 ) {
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_ROTATE , data );
		}
		else {
			ROBOT_SET_ROTATE_POSITION( GUI_TM_DISPLAY_GET() , data );
		}
	}
	KWIN_Item_Double_Display( hdlg , IDC_ROT_VALUE  , data , 2 );
	return data;
}
//
double Gui_For_Scheduler_RobotCal_Move_Data( HWND hdlg , int Arm , int Station , double run , int inc ) {
	double data;
	if ( inc == 0 ) {
		if ( Arm >= 100 ) {
			data = ROBOT_FM_GET_POSITION( 0 , RB_ANIM_MOVE );
		}
		else {
			data = ROBOT_GET_MOVE_POSITION( GUI_TM_DISPLAY_GET() );
		}
	}
	else {
		if      ( inc == 1 ) data = run + 0.01;
		else if ( inc == 2 ) data = run + 0.05;
		else if ( inc == 3 ) data = run + 0.1;
		else if ( inc == 4 ) data = run + 0.5;
		else if ( inc == 5 ) data = run + 1;
		else if ( inc == 6 ) data = run + 5;
		else if ( inc == 7 ) data = run + 10;
		else if ( inc == 99 ) data = run;
		if      ( inc == -1 ) data = run - 0.01;
		else if ( inc == -2 ) data = run - 0.05;
		else if ( inc == -3 ) data = run - 0.1;
		else if ( inc == -4 ) data = run - 0.5;
		else if ( inc == -5 ) data = run - 1;
		else if ( inc == -6 ) data = run - 5;
		else if ( inc == -7 ) data = run - 10;
		if ( data < -500 )   data = -500;  // 2003.01.14
		if ( data > 500 ) data = 500;  // 2003.01.14
		//
		if ( Arm >= 100 ) {
			ROBOT_FM_SET_POSITION( 0 , RB_ANIM_MOVE , data );
		}
		else {
			ROBOT_SET_MOVE_POSITION( GUI_TM_DISPLAY_GET() , data );
		}
	}
	KWIN_Item_Double_Display( hdlg , IDC_MOVE_VALUE  , data , 2 );
	return data;
}
//
int Gui_For_Scheduler_RobotCal_Arm_Data( HWND hdlg , int Arm , int inc ) {
	//
	if      ( inc == 1 ) {
		//
		if ( ( Arm >= 0 ) && ( Arm <= ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() - 1 ) ) ) {
			if ( Arm == ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() - 1 ) )	Arm = 100;
			else												Arm++;
		}
		//
	}
	else if ( inc == -1 ) {
		//
		if ( ( Arm >= 1 ) && ( ( Arm == 100 ) || ( Arm <= ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() - 1 ) ) ) ) {
			if ( Arm == 100 )	Arm = ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() - 1 );
			else				Arm--;
		}
		//
	}
	//---------------------------------------------------------------
	if ( Arm <   0 ) Arm = 0;
	if ( ( Arm > 3 ) && ( Arm < 100 ) ) Arm = 0;
	if ( Arm > 100 ) Arm = 100;
	//---------------------------------------------------------------
	switch( Arm ) {
	case 0 :	KWIN_Item_String_Display( hdlg , IDC_ARM_VALUE  , "Arm A (TM)" );	break;
	case 1 :	KWIN_Item_String_Display( hdlg , IDC_ARM_VALUE  , "Arm B (TM)" );	break;
	case 2 :	KWIN_Item_String_Display( hdlg , IDC_ARM_VALUE  , "Arm C (TM)" );	break;
	case 3 :	KWIN_Item_String_Display( hdlg , IDC_ARM_VALUE  , "Arm D (TM)" );	break;
	case 100 :	KWIN_Item_String_Display( hdlg , IDC_ARM_VALUE  , "FEM" );	break;
	default :	KWIN_Item_String_Display( hdlg , IDC_ARM_VALUE  , "????" );	break;
	}
	//---------------------------------------------------------------
	return Arm;
}
//
int Gui_For_Scheduler_RobotCal_Station_Data( HWND hdlg , int Station , int inc ) {
	char Buffer[32];
	Station = Station + inc;
	if ( Station <                                   0 ) Station = ( BM1 + MAX_BM_CHAMBER_DEPTH - 1 ) - 1; // 2003.01.14
	if ( Station >= ( BM1 + MAX_BM_CHAMBER_DEPTH - 1 ) ) Station = 0; // 2003.01.14
	//
	// 2010.12.06
	/*
	switch( Station ) {
	case 0 :	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , "CM1" );	break;
	case 1 :	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , "CM2" );	break;
	case 2 :	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , "CM3" );	break;
	case 3 :	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , "CM4" );	break;
	case MAX_PM_CHAMBER_DEPTH + 4 :	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , "AL" );	break;
	case MAX_PM_CHAMBER_DEPTH + 5 :	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , "IC" );	break;
	default :
		if ( Station >= ( MAX_PM_CHAMBER_DEPTH + 6 ) ) {
			sprintf( Buffer , "BM%d" , Station - MAX_PM_CHAMBER_DEPTH - 6 + 1 );
			KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , Buffer );
		}
		else {
			sprintf( Buffer , "PM%d" , Station - 3 );
			KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , Buffer );
		}
		break;
	}
	*/
	if ( ( Station >= 0 ) && ( Station < MAX_CASSETTE_SIDE ) ) {
		sprintf( Buffer , "CM%d" , Station + 1 );
	}
	else if ( Station == MAX_PM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE ) {
		sprintf( Buffer , "AL" );
	}
	else if ( Station == MAX_PM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + 1 ) {
		sprintf( Buffer , "IC" );
	}
	else {
		if ( Station >= ( MAX_PM_CHAMBER_DEPTH + MAX_CASSETTE_SIDE + 2 ) ) {
			sprintf( Buffer , "BM%d" , Station - MAX_PM_CHAMBER_DEPTH - ( MAX_CASSETTE_SIDE + 2 ) + 1 );
		}
		else {
			sprintf( Buffer , "PM%d" , Station - ( MAX_CASSETTE_SIDE - 1 ) );
		}
	}
	//
	KWIN_Item_String_Display( hdlg , IDC_STATION_VALUE , Buffer );
	//
	return Station;
}
//
void Gui_For_Scheduler_RobotCal_Wafer_Data( HWND hdlg , BOOL tog , int Station , int Slot , int pos ) {
	int i , u , a , p , e;
	if ( Station < PM1 ) {
		u = 0;
		a = 0;
		p = 0;
		e = 0;
		for ( i = 0 ; i < _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ; i++ ) {
			switch( _i_SCH_IO_GET_MODULE_STATUS( Station , i + 1 ) ) {
			case 0 :
				u++;
				break;
			case 1 :
				a++;
				break;
			case 2 :
				p++;
				break;
			default :
				e++;
				break;
			}
		}
		if ( tog ) {
			if      ( a == p ) e = 2;
			else if ( a >  p ) e = 2;
			else if ( a <  p ) e = 1;
			else               e = 2;
			for ( i = 0 ; i < _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ; i++ ) {
				_i_SCH_IO_SET_MODULE_STATUS( Station , i + 1 , e );
			}
			if ( e == 1 ) {
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
		}
		else {
			if      ( ( u == 0 ) && ( a != 0 ) && ( p == 0 ) && ( e == 0 ) ) {
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
			else if ( ( u == 0 ) && ( a == 0 ) && ( p != 0 ) && ( e == 0 ) ) {
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "?" );
			}
		}
	}
	else {
		if ( _i_SCH_IO_GET_MODULE_STATUS( Station , Slot ) ) {
			if ( tog ) {
				_i_SCH_IO_SET_MODULE_STATUS( Station , Slot , 0 );
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
		}
		else {
			if ( tog ) {
				_i_SCH_IO_SET_MODULE_STATUS( Station , Slot , 1 );
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
		}
	}
}
//
void Gui_For_Scheduler_RobotCal_WaferTM_Data( HWND hdlg , BOOL tog , int Station , int Slot , int pos ) {
	if ( Slot == 1 ) {
		if ( WAFER_STATUS_IN_TM( GUI_TM_DISPLAY_GET() , Station ) ) {
			if ( tog ) {
				WAFER_STATUS_SET_TM( GUI_TM_DISPLAY_GET() , Station , 0 );
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
		}
		else {
			if ( tog ) {
				WAFER_STATUS_SET_TM( GUI_TM_DISPLAY_GET() , Station , 1 );
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
		}
	}
	else if ( Slot == 2 ) {
		if ( WAFER_STATUS_IN_TM2( GUI_TM_DISPLAY_GET() , Station ) ) {
			if ( tog ) {
				WAFER_STATUS_SET_TM2( GUI_TM_DISPLAY_GET() , Station , 0 );
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
		}
		else {
			if ( tog ) {
				WAFER_STATUS_SET_TM2( GUI_TM_DISPLAY_GET() , Station , 1 );
				KWIN_Item_String_Display( hdlg , pos , "#" );
			}
			else {
				KWIN_Item_String_Display( hdlg , pos , "" );
			}
		}
	}
}
//
void Gui_For_Scheduler_RobotCal_WaferFM_Data( HWND hdlg , BOOL tog , int Station , int pos ) {
	if ( WAFER_STATUS_IN_FM( 0,Station ) ) {
		if ( tog ) {
			WAFER_STATUS_SET_FM( 0 , Station , 0 );
			KWIN_Item_String_Display( hdlg , pos , "" );
		}
		else {
			KWIN_Item_String_Display( hdlg , pos , "#" );
		}
	}
	else {
		if ( tog ) {
			WAFER_STATUS_SET_FM( 0 , Station , 1 );
			KWIN_Item_String_Display( hdlg , pos , "#" );
		}
		else {
			KWIN_Item_String_Display( hdlg , pos , "" );
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_ROBOTCAL_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int stn = 0;	// C1 - C4 , P1 , B1 - B4
	static int arm = 0; // Arm A , Arm B , FEM;
	static double rot , ext , move;
	int i;
	char Buffer[256] , Buffer2[32];
	switch ( msg ) {
	case WM_INITDIALOG:
		MoveCenterWindow( hdlg );
		sprintf( Buffer , "T%d" , GUI_TM_DISPLAY_GET() + 1 );
		KWIN_Item_String_Display( hdlg , IDC_TM_DISPLAY , Buffer );

		arm  = Gui_For_Scheduler_RobotCal_Arm_Data( hdlg , arm , 0 );
		stn  = Gui_For_Scheduler_RobotCal_Station_Data( hdlg , stn , 0 );
		ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 0 );
		rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 0 );
		move = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 0 );

		SendMessage( hdlg , WM_COMMAND , IDABORT , (LPARAM) NULL );

		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDABORT : // 2006.07.13
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , CM1 , 1 , IDC_WFR_STS_C1 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , CM1+1 , 1 , IDC_WFR_STS_C2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , CM1+2 , 1 , IDC_WFR_STS_C3 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , CM1+3 , 1 , IDC_WFR_STS_C4 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1 , 1 , IDC_WFR_STS_P1 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+1 , 1 , IDC_WFR_STS_P2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+2 , 1 , IDC_WFR_STS_P3 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+3 , 1 , IDC_WFR_STS_P4 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+4 , 1 , IDC_WFR_STS_P5 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+5 , 1 , IDC_WFR_STS_P6 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+6 , 1 , IDC_WFR_STS_P7 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+7 , 1 , IDC_WFR_STS_P8 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+8 , 1 , IDC_WFR_STS_P9 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+9 , 1 , IDC_WFR_STS_P10 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+10 , 1 , IDC_WFR_STS_P11 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+11 , 1 , IDC_WFR_STS_P12 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , AL , 1 , IDC_WFR_STS_AL );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , IC , 1 , IDC_WFR_STS_IC );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1 , 1 , IDC_WFR_STS_B1 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1+1 , 1 , IDC_WFR_STS_B2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1+2 , 1 , IDC_WFR_STS_B3 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1+3 , 1 , IDC_WFR_STS_B4 );
			//
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1 , 2 , IDC_WFR_STS_P1_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+1 , 2 , IDC_WFR_STS_P2_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+2 , 2 , IDC_WFR_STS_P3_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+3 , 2 , IDC_WFR_STS_P4_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+4 , 2 , IDC_WFR_STS_P5_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+5 , 2 , IDC_WFR_STS_P6_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+6 , 2 , IDC_WFR_STS_P7_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+7 , 2 , IDC_WFR_STS_P8_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+8 , 2 , IDC_WFR_STS_P9_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+9 , 2 , IDC_WFR_STS_P10_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+10 , 2 , IDC_WFR_STS_P11_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , PM1+11 , 2 , IDC_WFR_STS_P12_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , F_AL , 1 , IDC_WFR_STS_AL2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , F_IC , 1 , IDC_WFR_STS_IC2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1 , 2 , IDC_WFR_STS_B1_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1+1 , 2 , IDC_WFR_STS_B2_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1+2 , 2 , IDC_WFR_STS_B3_2 );
			Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , FALSE , BM1+3 , 2 , IDC_WFR_STS_B4_2 );
			//
			Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , FALSE , 0 , 1 , IDC_WFR_STS_TA );
			Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , FALSE , 1 , 1 , IDC_WFR_STS_TB );
			Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , FALSE , 0 , 2 , IDC_WFR_STS_TA2 );
			Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , FALSE , 1 , 2 , IDC_WFR_STS_TB2 );
			Gui_For_Scheduler_RobotCal_WaferFM_Data( hdlg , FALSE , 0 , IDC_WFR_STS_FA );
			Gui_For_Scheduler_RobotCal_WaferFM_Data( hdlg , FALSE , 1 , IDC_WFR_STS_FB );
			break;
		case IDC_TM_DISPLAY :
			strcpy( Buffer , "TM" );
			for ( i = 1 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				sprintf( Buffer2 , "|TM%d" , i + 1 );
				strcat( Buffer , Buffer2 );
			}
			i = GUI_TM_DISPLAY_GET();
			if ( MODAL_DIGITAL_BOX1( hdlg , "TM Group" , "Select" , Buffer , &i ) ) {
				if ( i != GUI_TM_DISPLAY_GET() ) {
					GUI_TM_DISPLAY_SET( i );
					sprintf( Buffer , "T%d" , GUI_TM_DISPLAY_GET() + 1 );
					KWIN_Item_String_Display( hdlg , IDC_TM_DISPLAY , Buffer );
				}
				InvalidateRect( hdlg , NULL , TRUE );
				SendMessage( hdlg , WM_COMMAND , IDABORT , (LPARAM) NULL );
			}
			break;

		case IDC_WFR_STS_C1 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , CM1 , 1 , IDC_WFR_STS_C1 );	return TRUE;
		case IDC_WFR_STS_C2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , CM1+1 , 1 , IDC_WFR_STS_C2 );	return TRUE;
		case IDC_WFR_STS_C3 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , CM1+2 , 1 , IDC_WFR_STS_C3 );	return TRUE;
		case IDC_WFR_STS_C4 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , CM1+3 , 1 , IDC_WFR_STS_C4 );	return TRUE;
		case IDC_WFR_STS_P1 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1 , 1 , IDC_WFR_STS_P1 );	return TRUE;
		case IDC_WFR_STS_P2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+1 , 1 , IDC_WFR_STS_P2 );	return TRUE;
		case IDC_WFR_STS_P3 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+2 , 1 , IDC_WFR_STS_P3 );	return TRUE;
		case IDC_WFR_STS_P4 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+3 , 1 , IDC_WFR_STS_P4 );	return TRUE;
		case IDC_WFR_STS_P5 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+4 , 1 , IDC_WFR_STS_P5 );	return TRUE;
		case IDC_WFR_STS_P6 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+5 , 1 , IDC_WFR_STS_P6 );	return TRUE;
		case IDC_WFR_STS_P7 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+6 , 1 , IDC_WFR_STS_P7 );	return TRUE;
		case IDC_WFR_STS_P8 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+7 , 1 , IDC_WFR_STS_P8 );	return TRUE;
		case IDC_WFR_STS_P9 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+8 , 1 , IDC_WFR_STS_P9 );	return TRUE;
		case IDC_WFR_STS_P10 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+9 , 1 , IDC_WFR_STS_P10 );	return TRUE;
		case IDC_WFR_STS_P11 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+10 , 1 , IDC_WFR_STS_P11 );	return TRUE;
		case IDC_WFR_STS_P12 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+11 , 1 , IDC_WFR_STS_P12 );	return TRUE;
		case IDC_WFR_STS_AL :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , AL , 1 , IDC_WFR_STS_AL );	return TRUE;
		case IDC_WFR_STS_IC :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , IC , 1 , IDC_WFR_STS_IC );	return TRUE;
		case IDC_WFR_STS_B1 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1 , 1 , IDC_WFR_STS_B1 );	return TRUE;
		case IDC_WFR_STS_B2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1+1 , 1 , IDC_WFR_STS_B2 );	return TRUE;
		case IDC_WFR_STS_B3 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1+2 , 1 , IDC_WFR_STS_B3 );	return TRUE;
		case IDC_WFR_STS_B4 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1+3 , 1 , IDC_WFR_STS_B4 );	return TRUE;
		//
		case IDC_WFR_STS_P1_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1 , 2 , IDC_WFR_STS_P1_2 );	return TRUE;
		case IDC_WFR_STS_P2_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+1 , 2 , IDC_WFR_STS_P2_2 );	return TRUE;
		case IDC_WFR_STS_P3_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+2 , 2 , IDC_WFR_STS_P3_2 );	return TRUE;
		case IDC_WFR_STS_P4_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+3 , 2 , IDC_WFR_STS_P4_2 );	return TRUE;
		case IDC_WFR_STS_P5_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+4 , 2 , IDC_WFR_STS_P5_2 );	return TRUE;
		case IDC_WFR_STS_P6_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+5 , 2 , IDC_WFR_STS_P6_2 );	return TRUE;
		case IDC_WFR_STS_P7_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+6 , 2 , IDC_WFR_STS_P7_2 );	return TRUE;
		case IDC_WFR_STS_P8_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+7 , 2 , IDC_WFR_STS_P8_2 );	return TRUE;
		case IDC_WFR_STS_P9_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+8 , 2 , IDC_WFR_STS_P9_2 );	return TRUE;
		case IDC_WFR_STS_P10_2:	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+9 , 2 , IDC_WFR_STS_P10_2 );	return TRUE;
		case IDC_WFR_STS_P11_2:	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+10 , 2 , IDC_WFR_STS_P11_2 );	return TRUE;
		case IDC_WFR_STS_P12_2:	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , PM1+11 , 2 , IDC_WFR_STS_P12_2 );	return TRUE;
		case IDC_WFR_STS_AL2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , F_AL , 1 , IDC_WFR_STS_AL2 );	return TRUE;
		case IDC_WFR_STS_IC2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , F_IC , 1 , IDC_WFR_STS_IC2 );	return TRUE;
		case IDC_WFR_STS_B1_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1 , 2 , IDC_WFR_STS_B1_2 );	return TRUE;
		case IDC_WFR_STS_B2_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1+1 , 2 , IDC_WFR_STS_B2_2 );	return TRUE;
		case IDC_WFR_STS_B3_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1+2 , 2 , IDC_WFR_STS_B3_2 );	return TRUE;
		case IDC_WFR_STS_B4_2 :	Gui_For_Scheduler_RobotCal_Wafer_Data( hdlg , TRUE , BM1+3 , 2 , IDC_WFR_STS_B4_2 );	return TRUE;
		//
		case IDC_WFR_STS_TA :	Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , TRUE , 0 , 1 , IDC_WFR_STS_TA );	return TRUE;
		case IDC_WFR_STS_TB :	Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , TRUE , 1 , 1 , IDC_WFR_STS_TB );	return TRUE;
		case IDC_WFR_STS_TA2 :	Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , TRUE , 0 , 2 , IDC_WFR_STS_TA2 );	return TRUE;
		case IDC_WFR_STS_TB2 :	Gui_For_Scheduler_RobotCal_WaferTM_Data( hdlg , TRUE , 1 , 2 , IDC_WFR_STS_TB2 );	return TRUE;
		case IDC_WFR_STS_FA :	Gui_For_Scheduler_RobotCal_WaferFM_Data( hdlg , TRUE , 0 , IDC_WFR_STS_FA );	return TRUE;
		case IDC_WFR_STS_FB :	Gui_For_Scheduler_RobotCal_WaferFM_Data( hdlg , TRUE , 1 , IDC_WFR_STS_FB );	return TRUE;

		case IDRETRY :
			ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , 0 , 99 );
			rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , 0 , 99 );
			move = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , 0 , 99 );
			return TRUE;

		case IDYES :
			if ( arm >= 100 ) {
				ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , _i_SCH_PRM_GET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_EXTEND ) , 99 );
				rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , _i_SCH_PRM_GET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_ROTATE ) , 99 );
				move = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , _i_SCH_PRM_GET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_MOVE ) , 99 );
			}
			else {
				ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , _i_SCH_PRM_GET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_EXTEND ) , 99 );
				rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , _i_SCH_PRM_GET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_ROTATE ) , 99 );
				move = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , _i_SCH_PRM_GET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_MOVE ) , 99 );
			}
			return TRUE;

		case IDNO : // 2005.12.15
		case IDIGNORE : // 2007.04.02
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( _i_SCH_MODULE_GET_USE_ENABLE( i + CM1 , FALSE , -1 ) ) {
					WAFER_MSET_CM( i + CM1 , ( wParam == IDNO ) ? 1 : 2 );
				}
			}
			WAFER_MSET_FM( 0 );
			WAFER_MSET_TM( 0 );
			WAFER_MSET_PM( -1 , 0 );
			WAFER_MSET_BM( -1 , 0 );
			//
			SendMessage( hdlg , WM_COMMAND , IDABORT , (LPARAM) NULL );
			return TRUE;

		case IDOK :
			if ( IDYES == MessageBox( hdlg , "Do you want to regist this robot animation info ?", "Regist Confirmation", MB_ICONQUESTION | MB_YESNO ) ) {
				if ( arm >= 100 ) {
					_i_SCH_PRM_SET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_ROTATE  , rot );
					_i_SCH_PRM_SET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_EXTEND  , ext );
					_i_SCH_PRM_SET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_RETRACT , 0 );
					_i_SCH_PRM_SET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_UP      , 100 );
					_i_SCH_PRM_SET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_DOWN    , 0 );
					_i_SCH_PRM_SET_RB_FM_POSITION( 0 , stn + 1 , 1 , RB_ANIM_MOVE    , move );
				}
				else {
					_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_ROTATE  , rot );
					_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_EXTEND  , ext );
					_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_RETRACT , 0 );
					_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_UP      , 100 );
					_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_DOWN    , 0 );
					_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , arm , stn + 1 , RB_ANIM_MOVE    , move );
				}
				//
				GUI_SAVE_PARAMETER_DATA( 2 );
				InvalidateRect( KGUI_STANDARD_Get_Screen_Handle() , NULL , TRUE );
			}
			return TRUE;

		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;

		case IDC_RBC_ARM_DEC :		arm  = Gui_For_Scheduler_RobotCal_Arm_Data( hdlg , arm , -1 );	return TRUE;
		case IDC_RBC_ARM_INC :		arm  = Gui_For_Scheduler_RobotCal_Arm_Data( hdlg , arm , 1 );	return TRUE;
		case IDC_RBC_STATION_DEC :	stn  = Gui_For_Scheduler_RobotCal_Station_Data( hdlg , stn , -1 ); return TRUE;
		case IDC_RBC_STATION_INC :	stn  = Gui_For_Scheduler_RobotCal_Station_Data( hdlg , stn , 1 ); return TRUE;

		case IDC_EXT_DEC :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , -1 );	return TRUE;
		case IDC_EXT_DEC2 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , -2 );	return TRUE;
		case IDC_EXT_DEC3 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , -3 );	return TRUE;
		case IDC_EXT_DEC4 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , -4 );	return TRUE;
		case IDC_EXT_DEC5 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , -5 );	return TRUE;
		case IDC_EXT_DEC6 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , -6 );	return TRUE;
		case IDC_EXT_INC :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 1 );	return TRUE;
		case IDC_EXT_INC2 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 2 );	return TRUE;
		case IDC_EXT_INC3 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 3 );	return TRUE;
		case IDC_EXT_INC4 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 4 );	return TRUE;
		case IDC_EXT_INC5 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 5 );	return TRUE;
		case IDC_EXT_INC6 :	ext  = Gui_For_Scheduler_RobotCal_Ext_Data( hdlg , arm , stn , ext , 6 );	return TRUE;

		case IDC_ROT_DEC :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , -1 );	return TRUE;
		case IDC_ROT_DEC2 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , -2 );	return TRUE;
		case IDC_ROT_DEC3 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , -3 );	return TRUE;
		case IDC_ROT_DEC4 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , -4 );	return TRUE;
		case IDC_ROT_DEC5 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , -5 );	return TRUE;
		case IDC_ROT_DEC6 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , -6 );	return TRUE;
		case IDC_ROT_INC :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 1 );	return TRUE;
		case IDC_ROT_INC2 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 2 );	return TRUE;
		case IDC_ROT_INC3 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 3 );	return TRUE;
		case IDC_ROT_INC4 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 4 );	return TRUE;
		case IDC_ROT_INC5 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 5 );	return TRUE;
		case IDC_ROT_INC6 :	rot  = Gui_For_Scheduler_RobotCal_Rot_Data( hdlg , arm , stn , rot , 6 );	return TRUE;

		case IDC_MOVE_DEC :		move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , -1 );	return TRUE;
		case IDC_MOVE_DEC2 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , -2 );	return TRUE;
		case IDC_MOVE_DEC3 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , -3 );	return TRUE;
		case IDC_MOVE_DEC4 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , -4 );	return TRUE;
		case IDC_MOVE_DEC5 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , -5 );	return TRUE;
		case IDC_MOVE_DEC6 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , -6 );	return TRUE;
		case IDC_MOVE_INC :		move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 1 );	return TRUE;
		case IDC_MOVE_INC2 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 2 );	return TRUE;
		case IDC_MOVE_INC3 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 3 );	return TRUE;
		case IDC_MOVE_INC4 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 4 );	return TRUE;
		case IDC_MOVE_INC5 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 5 );	return TRUE;
		case IDC_MOVE_INC6 :	move  = Gui_For_Scheduler_RobotCal_Move_Data( hdlg , arm , stn , move , 6 );	return TRUE;

		}
		return TRUE;

	}
	return FALSE;
}
