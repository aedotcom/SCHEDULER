#include "default.h"

#include "EQ_Enum.h"

#include "GUI_Handling.h"
#include "User_Parameter.h"
#include "system_tag.h"
#include "Robot_Handling.h"
#include "resource.h"

//------------------------------------------------------------------------------------------
#define	RB_CNT	_i_SCH_PRM_GET_DFIX_MAX_FINGER_TM()
//------------------------------------------------------------------------------------------
void GUI_TM_Multi4_Arm_String( int a , int b , int c , int d , int mode , char *retstr ) {
	if      (  a &&  b &&  c &&  d )	strcpy( retstr , "ABCD" );
	else if (  a &&  b &&  c && !d )	strcpy( retstr , "ABC" );
	else if (  a &&  b && !c &&  d )	strcpy( retstr , "ABD" );
	else if (  a &&  b && !c && !d )	strcpy( retstr , "AB" );
	else if (  a && !b &&  c &&  d )	strcpy( retstr , "ACD" );
	else if (  a && !b &&  c && !d )	strcpy( retstr , "AC" );
	else if (  a && !b && !c &&  d )	strcpy( retstr , "AD" );
	else if (  a && !b && !c && !d )	strcpy( retstr , "A" );
	else if ( !a &&  b &&  c &&  d )	strcpy( retstr , "BCD" );
	else if ( !a &&  b &&  c && !d )	strcpy( retstr , "BC" );
	else if ( !a &&  b && !c &&  d )	strcpy( retstr , "BD" );
	else if ( !a &&  b && !c && !d )	strcpy( retstr , "B" );
	else if ( !a && !b &&  c &&  d )	strcpy( retstr , "CD" );
	else if ( !a && !b &&  c && !d )	strcpy( retstr , "C" );
	else if ( !a && !b && !c &&  d )	strcpy( retstr , "D" );
	else								strcpy( retstr , "A" );
	//
	if      ( mode == 1 ) {
		if      (  a &&  b && !c && !d )	strcat( retstr , " All" );
		else if (  a && !b && !c && !d )	strcat( retstr , " Only" );
		else if ( !a &&  b && !c && !d )	strcat( retstr , " Only" );
		else if ( !a && !b &&  c && !d )	strcat( retstr , " Only" );
		else if ( !a && !b && !c &&  d )	strcat( retstr , " Only" );
		else if ( !a && !b && !c && !d )	strcat( retstr , " Only" );
	}
	else if ( mode == 2 ) {
		strcat( retstr , " -sep" );
	}
}
//------------------------------------------------------------------------------------------
void GUI_RobotData_Sub_Display( HWND hWnd , int mdl , int style , int a0 , int a1 , int a2 , int a3 , int a4 , int a5 , double pos ) {
	if ( mdl ) {
		if      ( style == 0 ) KWIN_Item_Double_Display( hWnd , a0  , pos , 2 );
		else if ( style == 1 ) KWIN_Item_Double_Display( hWnd , a1  , pos , 2 );
		else if ( style == 2 ) KWIN_Item_Double_Display( hWnd , a2  , pos , 2 );
		else if ( style == 3 ) KWIN_Item_Double_Display( hWnd , a3  , pos , 2 );
		else if ( style == 4 ) KWIN_Item_Double_Display( hWnd , a4  , pos , 2 );
		else if ( style == 5 ) KWIN_Item_Double_Display( hWnd , a5  , pos , 2 );
	}
	else {
		if      ( style == 0 ) KWIN_Item_Hide( hWnd , a0 );
		else if ( style == 1 ) KWIN_Item_Hide( hWnd , a1 );
		else if ( style == 2 ) KWIN_Item_Hide( hWnd , a2 );
		else if ( style == 3 ) KWIN_Item_Hide( hWnd , a3 );
		else if ( style == 4 ) KWIN_Item_Hide( hWnd , a4 );
		else if ( style == 5 ) KWIN_Item_Hide( hWnd , a5 );
	}
}
//
void GUI_RobotData_Display( HWND hWnd , HDC hDC ) {
	#define X31_START	16
	#define Y31_START	16
	int i , j , k , sp , ep , sc , ec , sb , eb , arm;
	int MDL[10],CHM[10],TD;
	char Buffer[32];
	char Buffer2[64];

	if ( GUI_RB_DISPLAY_GET() < 100 ) {
		TD = GUI_TM_DISPLAY_GET();
		sprintf( Buffer , "T%d" , TD + 1 );
		KWIN_Item_String_Display( hWnd , IDC_TM_DISPLAY , Buffer );
	}
	else { // 2007.06.22
		if ( _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) { // 2007.06.22
			sprintf( Buffer , "F%d" , GUI_FM_DISPLAY_GET() + 1 ); // 2007.06.22
			KWIN_Item_String_Display( hWnd , IDC_TM_DISPLAY , Buffer ); // 2007.06.22
		} // 2007.06.22
		else {
			KWIN_Item_Hide( hWnd , IDC_TM_DISPLAY ); // 2007.06.22
		}
	}
	if ( GUI_RB_DISPLAY_GET() < 100 ) {
		i = (GUI_RB_DISPLAY_GET() / RB_CNT);
		//
		sp = i * 6;
		ep = sp + 6;
		if ( sp >= MAX_PM_CHAMBER_DEPTH ) sp = 0;
		else                              sp = sp + PM1;
		if ( ep >= MAX_PM_CHAMBER_DEPTH ) ep = MAX_PM_CHAMBER_DEPTH;
		ep = ep + PM1 - 1;
		sb = i * 4;
		eb = sb + 4;
		if ( sb >= MAX_BM_CHAMBER_DEPTH ) sb = 0;
		else                              sb = sb + BM1;
		if ( eb >= MAX_BM_CHAMBER_DEPTH ) eb = MAX_BM_CHAMBER_DEPTH;
		eb = eb + BM1 - 1;
		//
		//
		/*
		//
		// 2018.12.14
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			sc = i * 4;
			ec = sc + 4;
			if ( sc >= MAX_CASSETTE_SIDE )	sc = 0;
			else							sc = sc + CM1;
			if ( ec >= MAX_CASSETTE_SIDE ) ec = MAX_CASSETTE_SIDE;
			ec = ec + CM1 - 1;
		}
		else {
			sc = i * 4;
			ec = sc + 4;
			if ( sc >= MAX_CASSETTE_SIDE ) {
				sc = BM1 + sc - MAX_CASSETTE_SIDE;
				ec = sc + 4;
				if ( ec >= ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ec = MAX_BM_CHAMBER_DEPTH + BM1;
				ec = ec - 1;
			}
			else {
				sc = sc + CM1;
				if ( ec >= MAX_CASSETTE_SIDE ) ec = MAX_CASSETTE_SIDE;
				ec = ec + CM1 - 1;
			}
		}
		*/
		//
		//
		// 2018.12.14
		//
		sc = i * 4;
		ec = sc + 4;
		if ( sc >= MAX_CASSETTE_SIDE )	sc = 0;
		else							sc = sc + CM1;
		if ( ec >= MAX_CASSETTE_SIDE ) ec = MAX_CASSETTE_SIDE;
		ec = ec + CM1 - 1;
		//
		//
		//
		arm = GUI_RB_DISPLAY_GET() % RB_CNT;
		//
		if ( sc == 0 ) ec = -1;
		if ( sp == 0 ) ep = -1;
		if ( sb == 0 ) eb = -1;
		if ( i == 0 ) {
			sprintf( Buffer , "Arm %c" , arm + 'A' );
		}
		else {
			if ( sp == 0 )
				sprintf( Buffer , "Arm %c(BM%d)" , arm + 'A' , sb - BM1 + 1 );
			else
				sprintf( Buffer , "Arm %c(PM%d)" , arm + 'A' , sp - PM1 + 1 );
		}
		KWIN_Item_String_Display( hWnd , IDC_ROBOT_DISPLAY , Buffer );
	}
	else {
		i = GUI_RB_DISPLAY_GET()-100;
		//
		sp = ep = 0;
		sb = i * 6;
		eb = sb + 6;
		if ( sb >= MAX_BM_CHAMBER_DEPTH ) sb = 0;
		else                              sb = sb + BM1;
		if ( eb >= MAX_BM_CHAMBER_DEPTH ) eb = MAX_BM_CHAMBER_DEPTH;
		eb = eb + BM1 - 1;
		sc = i * 4;
		ec = sc + 4;
		if ( sc >= MAX_CASSETTE_SIDE ) sc = 0;
		else                           sc = sc + CM1;
		if ( ec >= MAX_CASSETTE_SIDE ) ec = MAX_CASSETTE_SIDE;
		ec = ec + CM1 - 1;
		//
		if ( sc == 0 ) ec = -1;
		if ( sp == 0 ) ep = -1;
		if ( sb == 0 ) eb = -1;
		if ( i == 0 ) {
			sprintf( Buffer , "FEM" );
		}
		else {
			sprintf( Buffer , "FEM(%d)" , i + 1 );
		}
		KWIN_Item_String_Display( hWnd , IDC_ROBOT_DISPLAY , Buffer );
	}

	if ( GUI_RB_DISPLAY_GET() >= 100 ) {
		j = 1;
		for ( k = 0 , i = sc ; i <= ec ; i++ , j++ , k++ ) {
			sprintf( Buffer , "Cassette %d" , i - CM1 + 1 );
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , Buffer , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		for ( ; k < 4 ; j++ , k++ ) {
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		//
		for ( k = 0 , i = sb ; i <= eb ; i++ , j++ , k++ ) {
			sprintf( Buffer , "Buffer %d" , i - BM1 + 1 );
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , Buffer , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		for ( ; k < 6 ; j++ , k++ ) {
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "Aligner" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );		j++;
		KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "Cooler" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );		j++;
		KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "Speed" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
	}
	else {
		j = 1;
		//
		//
		/*
		//
		// 2018.12.14
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			for ( k = 0 , i = sb ; i <= eb ; i++ , j++ , k++ ) {
				sprintf( Buffer , "Buffer %d" , i - BM1 + 1 );
				KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , Buffer , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
			}
			for ( ; k < 4 ; j++ , k++ ) {
				KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
			}
		}
		else {
			if ( sc != 0 ) {
				for ( k = 0 , i = sc ; i <= ec ; i++ , j++ , k++ ) {
					if  ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) {
						sprintf( Buffer , "" );
					}
					else {
						if ( sc < BM1 ) {
							sprintf( Buffer , "Cassette %d" , i - CM1 + 1 );
						}
						else {
							sprintf( Buffer , "Buffer %d" , i - BM1 + 1 );
						}
					}
					KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , Buffer , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
				}
			}
			for ( ; k < 4 ; j++ , k++ ) {
				KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
			}
		}
		//
		*/
		//
		//
		//
		// 2018.12.14
		//
		for ( k = 0 , i = sb ; i <= eb ; i++ , j++ , k++ ) {
			sprintf( Buffer , "Buffer %d" , i - BM1 + 1 );
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , Buffer , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		for ( ; k < 4 ; j++ , k++ ) {
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		//
		//
		//
		for ( k = 0 , i = sp ; i <= ep ; i++ , j++ , k++ ) {
			sprintf( Buffer , "Process %d" , i - PM1 + 1 );
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , Buffer , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		for ( ; k < 6 ; j++ , k++ ) {
			KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
		}
		KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "Aligner" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );		j++;
		KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "Cooler" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );		j++;
		KGUI_DRAW_StrBox2( hDC , X31_START , Y31_START + ( 18 * j ) , "Speed" , 10 , 0 , COLOR_Blank , COLOR_Blank , COLOR_Black , COLOR_Blank );
	}

	if ( GUI_RB_DISPLAY_GET() >= 100 ) {
		for ( i = 0 ; i < 10 ; i++ ) MDL[i] = FALSE;
		for ( k = 0 , i = sc ; i <= ec ; i++ , k++ ) {
			if  ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) { MDL[k] = TRUE; CHM[k] = i; }
		}
		//
		for ( k = 4 , i = sb ; i <= eb ; i++ , k++ ) {
			if  ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) { MDL[k] = TRUE; CHM[k] = i; }
		}
		//
		for ( i = 0 ; i < 12 ; i++ ) {
			for ( j = 0 ; j < 6 ; j++ ) {
				if      ( i ==  0 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C1 , IDC_EXTEND_A_C1 , IDC_RETRACT_A_C1 , IDC_UP_A_C1 , IDC_DOWN_A_C1 , IDC_MOVE_A_C1 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  1 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C2 , IDC_EXTEND_A_C2 , IDC_RETRACT_A_C2 , IDC_UP_A_C2 , IDC_DOWN_A_C2 , IDC_MOVE_A_C2 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  2 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C3 , IDC_EXTEND_A_C3 , IDC_RETRACT_A_C3 , IDC_UP_A_C3 , IDC_DOWN_A_C3 , IDC_MOVE_A_C3 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  3 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C4 , IDC_EXTEND_A_C4 , IDC_RETRACT_A_C4 , IDC_UP_A_C4 , IDC_DOWN_A_C4 , IDC_MOVE_A_C4 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  4 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P1 , IDC_EXTEND_A_P1 , IDC_RETRACT_A_P1 , IDC_UP_A_P1 , IDC_DOWN_A_P1 , IDC_MOVE_A_P1 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  5 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P2 , IDC_EXTEND_A_P2 , IDC_RETRACT_A_P2 , IDC_UP_A_P2 , IDC_DOWN_A_P2 , IDC_MOVE_A_P2 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  6 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P3 , IDC_EXTEND_A_P3 , IDC_RETRACT_A_P3 , IDC_UP_A_P3 , IDC_DOWN_A_P3 , IDC_MOVE_A_P3 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  7 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P4 , IDC_EXTEND_A_P4 , IDC_RETRACT_A_P4 , IDC_UP_A_P4 , IDC_DOWN_A_P4 , IDC_MOVE_A_P4 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  8 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P5 , IDC_EXTEND_A_P5 , IDC_RETRACT_A_P5 , IDC_UP_A_P5 , IDC_DOWN_A_P5 , IDC_MOVE_A_P5 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i ==  9 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P6 , IDC_EXTEND_A_P6 , IDC_RETRACT_A_P6 , IDC_UP_A_P6 , IDC_DOWN_A_P6 , IDC_MOVE_A_P6 , MDL[i] ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , CHM[i] , 1 , j ) : 0 );
				else if ( i == 10 ) GUI_RobotData_Sub_Display( hWnd , _i_SCH_PRM_GET_MODULE_MODE( F_AL ) , j , IDC_ROTATE_A_AL , IDC_EXTEND_A_AL , IDC_RETRACT_A_AL , IDC_UP_A_AL , IDC_DOWN_A_AL , IDC_MOVE_A_AL , _i_SCH_PRM_GET_MODULE_MODE( F_AL ) ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , AL , 1 , j ) : 0 );
				else if ( i == 11 ) GUI_RobotData_Sub_Display( hWnd , _i_SCH_PRM_GET_MODULE_MODE( F_IC ) , j , IDC_ROTATE_A_IC , IDC_EXTEND_A_IC , IDC_RETRACT_A_IC , IDC_UP_A_IC , IDC_DOWN_A_IC , IDC_MOVE_A_IC , _i_SCH_PRM_GET_MODULE_MODE( F_IC ) ? _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , IC , 1 , j ) : 0 );
			}
		}
		//------------------------------------------------------------------
		KWIN_Item_Double_Display( hWnd , IDC_ROTATE_A_SPEED  , _i_SCH_PRM_GET_RB_FM_RNG(GUI_FM_DISPLAY_GET(),RB_ANIM_ROTATE ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_EXTEND_A_SPEED  , _i_SCH_PRM_GET_RB_FM_RNG(GUI_FM_DISPLAY_GET(),RB_ANIM_EXTEND ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_RETRACT_A_SPEED , _i_SCH_PRM_GET_RB_FM_RNG(GUI_FM_DISPLAY_GET(),RB_ANIM_RETRACT) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_UP_A_SPEED      , _i_SCH_PRM_GET_RB_FM_RNG(GUI_FM_DISPLAY_GET(),RB_ANIM_UP     ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_DOWN_A_SPEED    , _i_SCH_PRM_GET_RB_FM_RNG(GUI_FM_DISPLAY_GET(),RB_ANIM_DOWN   ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_MOVE_A_SPEED    , _i_SCH_PRM_GET_RB_FM_RNG(GUI_FM_DISPLAY_GET(),RB_ANIM_MOVE   ) , 2 );
		//------------------------------------------------------------------
		KWIN_Item_Double_Display( hWnd , IDC_HOME_POSITION , _i_SCH_PRM_GET_RB_FM_HOME_POSITION(GUI_FM_DISPLAY_GET()) , 2 );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ROBOT_SYNCH  , _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK(GUI_FM_DISPLAY_GET()) , "FALSE|TRUE|TRUE(S)|TRUE(A)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		switch( _i_SCH_PRM_GET_RB_FM_ROTATION_STYLE(GUI_FM_DISPLAY_GET()) ) {
		case ROTATION_STYLE_CW :		KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CW_ROTATION" );	break;
		case ROTATION_STYLE_CCW :		KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CCW_ROTATION" );	break;
		case ROTATION_STYLE_MINIMUM :	KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "MINIMUM" );		break;
		case ROTATION_STYLE_HOME_CW :	KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CW_HOMEBASED" );	break;
		default	:						KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CCW_HOMEBASED" );	break;
		}
		//---------------------------------------------------------------
		if      (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),1) )	strcpy( Buffer2 , "AB" );
		else if (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),0) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),1) )	strcpy( Buffer2 , "A" );
		else if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),1) )	strcpy( Buffer2 , "B" );
		else																																			strcpy( Buffer2 , "A" );
		//---------------------------------------------------------------
		if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE(GUI_FM_DISPLAY_GET()) != 0 ) { // 2003.10.06
			sprintf( Buffer , "(%d)" , _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE(GUI_FM_DISPLAY_GET()) + 1 );	strcat( Buffer2 , Buffer );
		}
		if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_SEPERATE(GUI_FM_DISPLAY_GET()) ) {
			strcpy( Buffer , "-Sep" );	strcat( Buffer2 , Buffer );
		}
		else {
			if      (  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),0) &&  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE(GUI_FM_DISPLAY_GET(),1) )	strcpy( Buffer , " All" );
			else																																			strcpy( Buffer , " Only" );
			//
			strcat( Buffer2 , Buffer );
		}
		KWIN_Item_String_Display( hWnd , IDC_ARM_USING , Buffer2 );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ARM_DISPLAY   , _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(GUI_FM_DISPLAY_GET(),0) + ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY(GUI_FM_DISPLAY_GET(),1) * 2 ) - 1 , "A Only|B Only|AB All" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ANIMATION_RUN , _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION(GUI_FM_DISPLAY_GET()) , "Off|On|Off(L)|Off(R)|Off(LR)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
	}
	else {
		for ( i = 0 ; i < 10 ; i++ ) MDL[i] = FALSE;
		//
		/*
		//
		// 2018.12.14
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			for ( k = 0 , i = sb ; i <= eb ; i++ , k++ ) {
				if  ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) { MDL[k] = TRUE; CHM[k] = i; }
			}
		}
		else {
			for ( k = 0 , i = sc ; i <= ec ; i++ , k++ ) {
				if  ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) { MDL[k] = TRUE; CHM[k] = i; }
			}
		}
		//
		*/
		//
		//
		// 2018.12.14
		//
		for ( k = 0 , i = sb ; i <= eb ; i++ , k++ ) {
			if  ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) { MDL[k] = TRUE; CHM[k] = i; }
		}
		//
		//
		for ( k = 4 , i = sp ; i <= ep ; i++ , k++ ) {
			if  ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) { MDL[k] = TRUE; CHM[k] = i; }
		}
		//
		for ( i = 0 ; i < 12 ; i++ ) {
			for ( j = 0 ; j < 6 ; j++ ) {
				if      ( i ==  0 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C1 , IDC_EXTEND_A_C1 , IDC_RETRACT_A_C1 , IDC_UP_A_C1 , IDC_DOWN_A_C1 , IDC_MOVE_A_C1 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  1 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C2 , IDC_EXTEND_A_C2 , IDC_RETRACT_A_C2 , IDC_UP_A_C2 , IDC_DOWN_A_C2 , IDC_MOVE_A_C2 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  2 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C3 , IDC_EXTEND_A_C3 , IDC_RETRACT_A_C3 , IDC_UP_A_C3 , IDC_DOWN_A_C3 , IDC_MOVE_A_C3 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  3 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_C4 , IDC_EXTEND_A_C4 , IDC_RETRACT_A_C4 , IDC_UP_A_C4 , IDC_DOWN_A_C4 , IDC_MOVE_A_C4 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  4 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P1 , IDC_EXTEND_A_P1 , IDC_RETRACT_A_P1 , IDC_UP_A_P1 , IDC_DOWN_A_P1 , IDC_MOVE_A_P1 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  5 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P2 , IDC_EXTEND_A_P2 , IDC_RETRACT_A_P2 , IDC_UP_A_P2 , IDC_DOWN_A_P2 , IDC_MOVE_A_P2 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  6 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P3 , IDC_EXTEND_A_P3 , IDC_RETRACT_A_P3 , IDC_UP_A_P3 , IDC_DOWN_A_P3 , IDC_MOVE_A_P3 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  7 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P4 , IDC_EXTEND_A_P4 , IDC_RETRACT_A_P4 , IDC_UP_A_P4 , IDC_DOWN_A_P4 , IDC_MOVE_A_P4 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  8 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P5 , IDC_EXTEND_A_P5 , IDC_RETRACT_A_P5 , IDC_UP_A_P5 , IDC_DOWN_A_P5 , IDC_MOVE_A_P5 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i ==  9 ) GUI_RobotData_Sub_Display( hWnd , MDL[i] , j , IDC_ROTATE_A_P6 , IDC_EXTEND_A_P6 , IDC_RETRACT_A_P6 , IDC_UP_A_P6 , IDC_DOWN_A_P6 , IDC_MOVE_A_P6 , MDL[i] ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , CHM[i] , j ) : 0 );
				else if ( i == 10 ) GUI_RobotData_Sub_Display( hWnd , _i_SCH_PRM_GET_MODULE_MODE( AL ) , j , IDC_ROTATE_A_AL , IDC_EXTEND_A_AL , IDC_RETRACT_A_AL , IDC_UP_A_AL , IDC_DOWN_A_AL , IDC_MOVE_A_AL , _i_SCH_PRM_GET_MODULE_MODE( AL ) ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , AL , j ) : 0 );
				else if ( i == 11 ) GUI_RobotData_Sub_Display( hWnd , _i_SCH_PRM_GET_MODULE_MODE( IC ) , j , IDC_ROTATE_A_IC , IDC_EXTEND_A_IC , IDC_RETRACT_A_IC , IDC_UP_A_IC , IDC_DOWN_A_IC , IDC_MOVE_A_IC , _i_SCH_PRM_GET_MODULE_MODE( IC ) ? _i_SCH_PRM_GET_RB_POSITION( TD , arm , IC , j ) : 0 );
			}
		}
		//------------------------------------------------------------------
		KWIN_Item_Double_Display( hWnd , IDC_ROTATE_A_SPEED  , _i_SCH_PRM_GET_RB_RNG( TD , arm , RB_ANIM_ROTATE  ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_EXTEND_A_SPEED  , _i_SCH_PRM_GET_RB_RNG( TD , arm , RB_ANIM_EXTEND  ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_RETRACT_A_SPEED , _i_SCH_PRM_GET_RB_RNG( TD , arm , RB_ANIM_RETRACT ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_UP_A_SPEED      , _i_SCH_PRM_GET_RB_RNG( TD , arm , RB_ANIM_UP      ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_DOWN_A_SPEED    , _i_SCH_PRM_GET_RB_RNG( TD , arm , RB_ANIM_DOWN    ) , 2 );
		KWIN_Item_Double_Display( hWnd , IDC_MOVE_A_SPEED    , _i_SCH_PRM_GET_RB_RNG( TD , arm , RB_ANIM_MOVE    ) , 2 );
		//------------------------------------------------------------------
		KWIN_Item_Double_Display( hWnd , IDC_HOME_POSITION , _i_SCH_PRM_GET_RB_HOME_POSITION( TD ) , 2 );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ROBOT_SYNCH , _i_SCH_PRM_GET_RB_SYNCH_CHECK( TD ) , "FALSE|TRUE" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
		switch( _i_SCH_PRM_GET_RB_ROTATION_STYLE( TD ) ) {
		case ROTATION_STYLE_CW :		KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CW_ROTATION" );	break;
		case ROTATION_STYLE_CCW :		KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CCW_ROTATION" );	break;
		case ROTATION_STYLE_MINIMUM :	KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "MINIMUM" );		break;
		case ROTATION_STYLE_HOME_CW :	KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CW_HOMEBASED" );	break;
		default	:						KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CCW_HOMEBASED" );	break;
		}
		//---------------------------------------------------------------
		GUI_TM_Multi4_Arm_String( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(TD,0) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(TD,1) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(TD,2) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(TD,3) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( TD ) ? 2 : 1 , Buffer2 );
		KWIN_Item_String_Display( hWnd , IDC_ARM_USING , Buffer2 );
		//---------------------------------------------------------------
		GUI_TM_Multi4_Arm_String( _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY(TD,0) , _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY(TD,1) , _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY(TD,2) , _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY(TD,3) , 1 , Buffer2 );
		KWIN_Item_String_Display( hWnd , IDC_ARM_DISPLAY , Buffer2 );
		//---------------------------------------------------------------
		KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ANIMATION_RUN , _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( TD ) , "Off|On|Off(L)|Off(R)|Off(LR)" , "????" ); // 2007.07.24
		//---------------------------------------------------------------
	}
}
//
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL GUI_RobotData_Control_Sub_Speed_Set( BOOL Control , HWND hWnd , int mode , char *str , int guimsg ) {
	double dRes;
	if ( Control ) return FALSE;
	if ( GUI_RB_DISPLAY_GET() >= 100 ) {
		dRes = _i_SCH_PRM_GET_RB_FM_RNG( GUI_FM_DISPLAY_GET() , mode );
		if ( MODAL_ANALOG_BOX1( hWnd , str , "Select" , 0 , 999 , 2 , &dRes ) ) {
			if ( dRes != _i_SCH_PRM_GET_RB_FM_RNG( GUI_FM_DISPLAY_GET() , mode ) ) {
				_i_SCH_PRM_SET_RB_FM_RNG( GUI_FM_DISPLAY_GET() , mode , dRes );
				KWIN_Item_Double_Display( hWnd , guimsg , dRes ,  2 );
				return TRUE;
			}
		}
	}
	else {
		dRes = _i_SCH_PRM_GET_RB_RNG( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , mode );
		if ( MODAL_ANALOG_BOX1( hWnd , "Rotate Speed" , "Select" , 0 , 999 , 2 , &dRes ) ) {
			if ( dRes != _i_SCH_PRM_GET_RB_RNG( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , mode ) ) {
				_i_SCH_PRM_SET_RB_RNG( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , mode , dRes );
				KWIN_Item_Double_Display( hWnd , guimsg , dRes , 2 );
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL GUI_RobotData_Control_Sub_Position_Set( BOOL Control , HWND hWnd , int mode , char *str , int guimsg , int msgidx , double min , double max ) {
	int c;
	double dRes;
	if ( Control ) return FALSE;

	if ( GUI_RB_DISPLAY_GET() >= 100 ) {
		if      ( msgidx == 1 ) c = CM1 + 0 + ( GUI_RB_DISPLAY_GET() - 100 ) * 4;
		else if ( msgidx == 2 ) c = CM1 + 1 + ( GUI_RB_DISPLAY_GET() - 100 ) * 4;
		else if ( msgidx == 3 ) c = CM1 + 2 + ( GUI_RB_DISPLAY_GET() - 100 ) * 4;
		else if ( msgidx == 4 ) c = CM1 + 3 + ( GUI_RB_DISPLAY_GET() - 100 ) * 4;
		//
		else if ( msgidx == 5 ) c = BM1 + 0 + ( GUI_RB_DISPLAY_GET() - 100 ) * 6;
		else if ( msgidx == 6 ) c = BM1 + 1 + ( GUI_RB_DISPLAY_GET() - 100 ) * 6;
		else if ( msgidx == 7 ) c = BM1 + 2 + ( GUI_RB_DISPLAY_GET() - 100 ) * 6;
		else if ( msgidx == 8 ) c = BM1 + 3 + ( GUI_RB_DISPLAY_GET() - 100 ) * 6;
		else if ( msgidx == 9 ) c = BM1 + 4 + ( GUI_RB_DISPLAY_GET() - 100 ) * 6;
		else if ( msgidx == 10 ) c = BM1 + 5 + ( GUI_RB_DISPLAY_GET() - 100 ) * 6;
		//
		else if ( msgidx == 11 ) c = AL;
		else if ( msgidx == 12 ) c = IC;
		dRes = _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , c , 1 , mode );
		if ( MODAL_ANALOG_BOX1( hWnd , str , "Select" , min , max , 2 , &dRes ) ) {
			if ( dRes != _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , c , 1 , mode ) ) {
				_i_SCH_PRM_SET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , c , 1 , mode , dRes );
				KWIN_Item_Double_Display( hWnd , guimsg , _i_SCH_PRM_GET_RB_FM_POSITION( GUI_FM_DISPLAY_GET() , c , 1 , mode ) , 2 );
				return TRUE;
			}
		}
	}
	else {
		//
		//
		/*
		// 2018.12.14
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if      ( msgidx == 1 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 0;
			else if ( msgidx == 2 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 1;
			else if ( msgidx == 3 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 2;
			else if ( msgidx == 4 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 3;
			//
			else if ( msgidx == 5 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 0;
			else if ( msgidx == 6 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 1;
			else if ( msgidx == 7 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 2;
			else if ( msgidx == 8 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 3;
			else if ( msgidx == 9 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 4;
			else if ( msgidx == 10 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 5;
			//
			else if ( msgidx == 11 ) c = AL;
			else if ( msgidx == 12 ) c = IC;
		}
		else {
			if      ( msgidx == 1 ) {
				c = CM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 0;
				if ( c >= MAX_CASSETTE_SIDE + CM1 ) c = BM1 + c - MAX_CASSETTE_SIDE - CM1;
			}
			else if ( msgidx == 2 ) {
				c = CM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 1;
				if ( c >= MAX_CASSETTE_SIDE + CM1 ) c = BM1 + c - MAX_CASSETTE_SIDE - CM1;
			}
			else if ( msgidx == 3 ) {
				c = CM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 2;
				if ( c >= MAX_CASSETTE_SIDE + CM1 ) c = BM1 + c - MAX_CASSETTE_SIDE - CM1;
			}
			else if ( msgidx == 4 ) {
				c = CM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 3;
				if ( c >= MAX_CASSETTE_SIDE + CM1 ) c = BM1 + c - MAX_CASSETTE_SIDE - CM1;
			}
			//
			else if ( msgidx == 5 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 0;
			else if ( msgidx == 6 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 1;
			else if ( msgidx == 7 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 2;
			else if ( msgidx == 8 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 3;
			else if ( msgidx == 9 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 4;
			else if ( msgidx == 10 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 5;
			//
			else if ( msgidx == 11 ) c = AL;
			else if ( msgidx == 12 ) c = IC;
		}
		//
		//
		*/
		//
		//
		//
		// 2018.12.14
		//
		if      ( msgidx == 1 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 0;
		else if ( msgidx == 2 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 1;
		else if ( msgidx == 3 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 2;
		else if ( msgidx == 4 ) c = BM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 4 ) + 3;
		//
		else if ( msgidx == 5 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 0;
		else if ( msgidx == 6 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 1;
		else if ( msgidx == 7 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 2;
		else if ( msgidx == 8 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 3;
		else if ( msgidx == 9 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 4;
		else if ( msgidx == 10 ) c = PM1 + ( ( GUI_RB_DISPLAY_GET() / RB_CNT ) * 6 ) + 5;
		//
		else if ( msgidx == 11 ) c = AL;
		else if ( msgidx == 12 ) c = IC;
		//
		//
		dRes = _i_SCH_PRM_GET_RB_POSITION( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , c , mode );
		if ( MODAL_ANALOG_BOX1( hWnd , str , "Select" , min , max , 2 , &dRes ) ) {
			if ( dRes != _i_SCH_PRM_GET_RB_POSITION( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , c , mode ) ) {
				_i_SCH_PRM_SET_RB_POSITION( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , c , mode , dRes );
				KWIN_Item_Double_Display( hWnd , guimsg , _i_SCH_PRM_GET_RB_POSITION( GUI_TM_DISPLAY_GET() , GUI_RB_DISPLAY_GET() % RB_CNT , c , mode ) , 2 );
				return TRUE;
			}
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL GUI_RobotData_Control( HWND hWnd , long msg , BOOL Control ) {
	int Res , c , c2 , c3 , i , j , k;
	double dRes;
	BOOL Change = FALSE;
	char Buffer[1024];
	char Buffer2[64];

	Change = FALSE;

	switch( msg ) {
	case IDC_ROBOT_DISPLAY :
		//
		strcpy( Buffer , "Arm A" );
		for ( k = 1 ; k < RB_CNT ; k++ ) {
			sprintf( Buffer2 , "|Arm %c" , k + 'A' );
			strcat( Buffer , Buffer2 );
		}
		//
		c2 = RB_CNT;
		//
		for ( c = 6 ; c < MAX_PM_CHAMBER_DEPTH ; ) {
			//
			for ( k = 0 ; k < RB_CNT ; k++ ) {
				sprintf( Buffer2 , "|Arm %c(PM%d)" , k + 'A' , c + 1 );
				strcat( Buffer , Buffer2 );
			}
			//
			c += 6;
			c2 = RB_CNT + c2;
		}
		c3 = 0;
		if ( ( MAX_BM_CHAMBER_DEPTH / 4 ) > ( MAX_PM_CHAMBER_DEPTH / 6 ) ) {
			j = ( MAX_BM_CHAMBER_DEPTH / 4 );
			if ( ( MAX_BM_CHAMBER_DEPTH % 4 ) != 0 ) j++;
			i = ( MAX_PM_CHAMBER_DEPTH / 6 );
			if ( ( MAX_PM_CHAMBER_DEPTH % 6 ) != 0 ) i++;
			//
			for ( c = 0 ; c < ( j - i ) ; c++ ) {
				//
				for ( k = 0 ; k < RB_CNT ; k++ ) {
					sprintf( Buffer2 , "|Arm %c(BM%d)" , k + 'A' , (i+c)*4+1 );
					strcat( Buffer , Buffer2 );
				}
				//
				c3 = RB_CNT + c3;
			}
		}
		//
		//
		/*
		//
		// 2018.12.14
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			if ( ( MAX_BM_CHAMBER_DEPTH / 6 ) > ( MAX_CASSETTE_SIDE / 4 ) ) {
				j = ( MAX_BM_CHAMBER_DEPTH / 6 );
				if ( ( MAX_BM_CHAMBER_DEPTH % 6 ) != 0 ) j++;
			}
			else {
				j = ( MAX_CASSETTE_SIDE / 4 );
				if ( ( MAX_CASSETTE_SIDE / 6 ) != 0 ) j++;
			}
			for ( i = 0 ; i < j ; i++ ) {
				strcat( Buffer , "|" );
				if ( i == 0 ) {
					sprintf( Buffer2 , "FEM" );
				}
				else {
					sprintf( Buffer2 , "FEM(%d)" , i + 1 );
				}
				strcat( Buffer , Buffer2 );
			}
		}
		//
		*/
		//
		//
		//
		// 2018.12.14
		//
		if ( ( MAX_BM_CHAMBER_DEPTH / 6 ) > ( MAX_CASSETTE_SIDE / 4 ) ) {
			j = ( MAX_BM_CHAMBER_DEPTH / 6 );
			if ( ( MAX_BM_CHAMBER_DEPTH % 6 ) != 0 ) j++;
		}
		else {
			j = ( MAX_CASSETTE_SIDE / 4 );
			if ( ( MAX_CASSETTE_SIDE / 6 ) != 0 ) j++;
		}
		for ( i = 0 ; i < j ; i++ ) {
			strcat( Buffer , "|" );
			if ( i == 0 ) {
				sprintf( Buffer2 , "FEM" );
			}
			else {
				sprintf( Buffer2 , "FEM(%d)" , i + 1 );
			}
			strcat( Buffer , Buffer2 );
		}
		//
		//
		//
		//

		Res = GUI_RB_DISPLAY_GET();
		//
		if ( Res >= 100 ) Res = c2 + c3 + ( Res - 100 );
		//
		if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Arm" , "Select" , Buffer , &Res ) ) {
			if ( ( Res >= (c2+c3) ) || ( Res != GUI_RB_DISPLAY_GET() ) ) {
				//
				if ( Res >= (c2+c3) )
					GUI_RB_DISPLAY_SET( 100 + ( Res - c2 - c3 ) );
				else
					GUI_RB_DISPLAY_SET( Res );
				//
				if ( GUI_RB_DISPLAY_GET() < 100 ) {
					c = (GUI_RB_DISPLAY_GET() / RB_CNT) * 6;
					if ( c >= MAX_PM_CHAMBER_DEPTH ) c = 0;
					else                             c = c + PM1;
					if ( c == 0 ) {
						c = (GUI_RB_DISPLAY_GET() / RB_CNT) * 4;
						if ( c >= MAX_BM_CHAMBER_DEPTH ) c = BM1;
						else                             c = c + BM1;
						sprintf( Buffer , "Arm %c(BM%d)" , (GUI_RB_DISPLAY_GET() % RB_CNT) + 'A' , c - BM1 + 1 );
					}
					else {
						if ( (GUI_RB_DISPLAY_GET() / RB_CNT) == 0 ) {
							sprintf( Buffer , "Arm %c" , (GUI_RB_DISPLAY_GET() % RB_CNT) + 'A' );
						}
						else {
							sprintf( Buffer , "Arm %c(PM%d)" , (GUI_RB_DISPLAY_GET() % RB_CNT) + 'A' , c - PM1 + 1 );
						}
					}
					KWIN_Item_String_Display( hWnd , IDC_ROBOT_DISPLAY , Buffer );
					//
					sprintf( Buffer , "T%d" , GUI_TM_DISPLAY_GET() + 1 ); // 2007.06.22
					KWIN_Item_String_Display( hWnd , IDC_TM_DISPLAY , Buffer ); // 2007.06.22
				}
				else {
					c = GUI_RB_DISPLAY_GET() - 100;
					//
					if ( c == 0 ) {
						sprintf( Buffer , "FEM" );
					}
					else {
						sprintf( Buffer , "FEM(%d)" , c + 1 );
					}
					KWIN_Item_String_Display( hWnd , IDC_ROBOT_DISPLAY , Buffer );

					if ( _i_SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) { // 2007.06.22
						sprintf( Buffer , "F%d" , GUI_FM_DISPLAY_GET() + 1 ); // 2007.06.22
						KWIN_Item_String_Display( hWnd , IDC_TM_DISPLAY , Buffer ); // 2007.06.22
					} // 2007.06.22
					else {
						KWIN_Item_Hide( hWnd , IDC_TM_DISPLAY ); // 2007.06.22
					}
				}
			}
			InvalidateRect( hWnd , NULL , TRUE );
		}
		break;

	case IDC_TM_DISPLAY :
		if ( GUI_RB_DISPLAY_GET() < 100 ) {
			strcpy( Buffer , "TM" );
			for ( c = 1 ; c < MAX_TM_CHAMBER_DEPTH ; c++ ) {
				sprintf( Buffer2 , "|TM%d" , c + 1 );
				strcat( Buffer , Buffer2 );
			}
			Res = GUI_TM_DISPLAY_GET();
			if ( MODAL_DIGITAL_BOX1( hWnd , "TM Group" , "Select" , Buffer , &Res ) ) {
				if ( Res != GUI_TM_DISPLAY_GET() ) {
					GUI_TM_DISPLAY_SET( Res );
					sprintf( Buffer , "T%d" , GUI_TM_DISPLAY_GET() + 1 );
					KWIN_Item_String_Display( hWnd , IDC_TM_DISPLAY , Buffer );
				}
				InvalidateRect( hWnd , NULL , TRUE );
			}
		}
		else {
			strcpy( Buffer , "FM|FM2" );
			Res = GUI_FM_DISPLAY_GET();
			if ( MODAL_DIGITAL_BOX1( hWnd , "FM Group" , "Select" , Buffer , &Res ) ) {
				if ( Res != GUI_FM_DISPLAY_GET() ) {
					GUI_FM_DISPLAY_SET( Res );
					sprintf( Buffer , "F%d" , GUI_FM_DISPLAY_GET() + 1 );
					KWIN_Item_String_Display( hWnd , IDC_TM_DISPLAY , Buffer );
				}
				InvalidateRect( hWnd , NULL , TRUE );
			}
		}
		break;

	case IDC_ROBOT_SYNCH :
		if ( Control ) return TRUE;
		if ( GUI_RB_DISPLAY_GET() >= 100 ) {
			Res = _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK( GUI_FM_DISPLAY_GET() );
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Synch Check" , "Select" , "FALSE|TRUE|TRUE(S)|TRUE(A)" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK( GUI_FM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_FM_SYNCH_CHECK( GUI_FM_DISPLAY_GET() , Res );
					//---------------------------------------------------------------
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ROBOT_SYNCH , _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK( GUI_FM_DISPLAY_GET() ) , "FALSE|TRUE|TRUE(S)|TRUE(A)" , "????" ); // 2007.07.24
					//---------------------------------------------------------------
				}
			}
		}
		else {
			Res = _i_SCH_PRM_GET_RB_SYNCH_CHECK( GUI_TM_DISPLAY_GET() );
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Synch Check" , "Select" , "FALSE|TRUE" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_RB_SYNCH_CHECK( GUI_TM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_SYNCH_CHECK( GUI_TM_DISPLAY_GET() , Res );
					//---------------------------------------------------------------
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ROBOT_SYNCH , _i_SCH_PRM_GET_RB_SYNCH_CHECK( GUI_TM_DISPLAY_GET() ) , "FALSE|TRUE" , "????" ); // 2007.07.24
					//---------------------------------------------------------------
				}
			}
		}
		break;

	case IDC_ARM_USING :
		if ( Control ) return TRUE;
		if ( GUI_RB_DISPLAY_GET() >= 100 ) {
			if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( GUI_FM_DISPLAY_GET() ) <= 0 ) {
				Res = _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 1 ) * 2 ) + ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_SEPERATE( GUI_FM_DISPLAY_GET() ) * 3 ) - 1;
			}
			else {
				Res = -1;
			}
			//
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Arm Using" , "Select" , "A Only|B Only|AB All|A Only(Seperate)|B Only(Seperate)|AB All(Seperate)|A(2)|A(3)|A(4)|A(5)|A(6)|A(7)|A(8)|A(9)|A(10)|AB(2)|AB(3)|AB(4)|AB(5)|AB(6)|AB(7)|AB(8)|AB(9)|AB(10)" , &Res ) ) {
				if ( ( Res >= 0 ) && ( Res <= 5 ) ) {
					Change = TRUE;
					//
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 0 ,   ( ( Res % 3 ) + 1 ) %  2       );
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 1 , ( ( ( Res % 3 ) + 1 ) %  4 ) / 2 );
					//
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( GUI_FM_DISPLAY_GET() , Res / 3 );
					_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( GUI_FM_DISPLAY_GET() , 0 );
					//
					if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_SEPERATE( GUI_FM_DISPLAY_GET() ) ) {
						//---------------------------------------------------------------
						KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ARM_USING ,  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 1 ) * 2 ) - 1 , "A -Sep|B -Sep|AB -Sp" , "????" ); // 2007.07.24
						//---------------------------------------------------------------
					}
					else {
						//---------------------------------------------------------------
						KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ARM_USING ,  _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 1 ) * 2 ) - 1 , "A Only|B Only|AB All" , "????" ); // 2007.07.24
						//---------------------------------------------------------------
					}
					//
					ROBOT_FM_ARM_SET_CONFIGURATION();
					//
				}
				else if ( ( Res >= 6 ) && ( Res <= 14 ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( GUI_FM_DISPLAY_GET() , 0 );
					//
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 0 , TRUE );
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 1 , FALSE );
					//
					_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( GUI_FM_DISPLAY_GET() , Res - 5 );
					//
					sprintf( Buffer , "A (%d)" , Res - 4 );
					KWIN_Item_String_Display( hWnd , IDC_ARM_USING , Buffer );
					//
					ROBOT_FM_ARM_SET_CONFIGURATION();
					//
				}
				else if ( ( Res >= 15 ) && ( Res <= 23 ) ) { // 2007.07.09
					Change = TRUE;
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_SEPERATE( GUI_FM_DISPLAY_GET() , 1 );
					//
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 0 , TRUE );
					_i_SCH_PRM_SET_RB_FM_FINGER_ARM_STYLE( GUI_FM_DISPLAY_GET() , 1 , TRUE );
					//
					_i_SCH_PRM_SET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( GUI_FM_DISPLAY_GET() , Res - 14 );
					//
					sprintf( Buffer , "AB(%d)-Sep" , Res - 13 );
					KWIN_Item_String_Display( hWnd , IDC_ARM_USING , Buffer );
					//
					ROBOT_FM_ARM_SET_CONFIGURATION();
					//
				}
			}
		}
		else {
			//
			if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() <= 2 ) {
				//
				Res = _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) * 3 ) - 1;
				//
				if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Arm Using" , "Select" , "A Only|B Only|AB All|A Only(Seperate)|B Only(Seperate)|AB All(Seperate)" , &Res ) ) {
					//
					if ( Res != ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) * 3 ) - 1 ) ) {
						//
						Change = TRUE;
						//
						_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 0 ,   ( ( Res % 3 ) + 1 ) %  2       );
						_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 1 , ( ( ( Res % 3 ) + 1 ) %  4 ) / 2 );
						//
						_i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() , Res / 3 );
						//
						//---------------------------------------------------------------
						GUI_TM_Multi4_Arm_String( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),0) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),1) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),2) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),3) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) ? 2 : 1 , Buffer );
						KWIN_Item_String_Display( hWnd , IDC_ARM_USING , Buffer );
						//---------------------------------------------------------------
						ROBOT_ARM_SET_CONFIGURATION( GUI_TM_DISPLAY_GET() );
						//---------------------------------------------------------------
					}
				}
			}
			else {
				//
				Res = _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 2 ) * 4 ) + + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 3 ) * 8 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) * 15 ) - 1;
				//
				if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Arm Using" , "Select" , "A Only|B Only|AB|C Only|AC|BC|ABC|D Only|AD|BD|ABD|CD|ACD|BCD|ABCD|A Only(Seperate)|B Only(Seperate)|AB(Seperate)|C Only(Seperate)|AC(Seperate)|BC(Seperate)|ABC(Seperate)|D Only(Seperate)|AD(Seperate)|BD(Seperate)|ABD(Seperate)|CD(Seperate)|ACD(Seperate)|BCD(Seperate)|ABCD(Seperate)" , &Res ) ) {
					//
					if ( Res != ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 2 ) * 4 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 3 ) * 8 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) * 15 ) - 1 ) ) {
						//
						Change = TRUE;
						//
						_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 0 ,   ( ( Res % 15 ) + 1 ) %  2       );
						_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 1 , ( ( ( Res % 15 ) + 1 ) %  4 ) / 2 );
						_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 2 , ( ( ( Res % 15 ) + 1 ) %  8 ) / 4 );
						_i_SCH_PRM_SET_RB_FINGER_ARM_STYLE( GUI_TM_DISPLAY_GET() , 3 , ( ( ( Res % 15 ) + 1 ) % 16 ) / 8 );
						//
						_i_SCH_PRM_SET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() , Res / 15 );
						//
						//---------------------------------------------------------------
						GUI_TM_Multi4_Arm_String( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),0) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),1) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),2) , _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE(GUI_TM_DISPLAY_GET(),3) , _i_SCH_PRM_GET_RB_FINGER_ARM_SEPERATE( GUI_TM_DISPLAY_GET() ) ? 2 : 1 , Buffer );
						KWIN_Item_String_Display( hWnd , IDC_ARM_USING , Buffer );
						//---------------------------------------------------------------
						ROBOT_ARM_SET_CONFIGURATION( GUI_TM_DISPLAY_GET() );
						//---------------------------------------------------------------
					}
				}
			}
		}
		break;

	case IDC_ARM_DISPLAY :
		//
		if ( Control ) return TRUE;
		//
		if ( GUI_RB_DISPLAY_GET() >= 100 ) {
			//
			Res = _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 0) + (_i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 1)*2) - 1;
			//
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Arm Display" , "Select" , "A Only|B Only|AB All" , &Res ) ) {
				//
				if ( Res != ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 0) + (_i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 1)*2) - 1 ) ) {
					Change = TRUE;
					//
					if      ( Res == 2 ) {
						_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 0 , TRUE );
						_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 1 , TRUE );
					}
					else if ( Res == 1 ) {
						_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 0 , FALSE );
						_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 1 , TRUE );
					}
					else {
						_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 0 , TRUE );
						_i_SCH_PRM_SET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 1 , FALSE );
					}
					//---------------------------------------------------------------
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ARM_DISPLAY , _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 0) + (_i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( GUI_FM_DISPLAY_GET() , 1)*2) - 1 , "A Only|B Only|AB All" , "????" ); // 2007.07.24
					//---------------------------------------------------------------
					ROBOT_FM_ARM_SET_CONFIGURATION();
					//
				}
			}
		}
		else {
			if ( _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() > 2 ) break;
			//
			Res = _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) - 1;
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Arm Display" , "Select" , "A Only|B Only|AB All" , &Res ) ) {
				//
				if ( Res != ( _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) - 1 ) ) {
					//
					Change = TRUE;
					//
					if      ( Res == 2 ) {
						_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 0 , TRUE );
						_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 1 , TRUE );
					}
					else if ( Res == 1 ) {
						_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 0 , FALSE );
						_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 1 , TRUE );
					}
					else {
						_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 0 , TRUE );
						_i_SCH_PRM_SET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 1 , FALSE );
					}
					//---------------------------------------------------------------
					KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ARM_DISPLAY , _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 0 ) + ( _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( GUI_TM_DISPLAY_GET() , 1 ) * 2 ) - 1 , "A Only|B Only|AB All" , "????" ); // 2007.07.24
					//---------------------------------------------------------------
					//
					ROBOT_ARM_SET_CONFIGURATION( GUI_TM_DISPLAY_GET() );
					//
				}
			}
		}
		break;

	case IDC_ANIMATION_RUN :
		if ( Control ) return TRUE;
		if ( GUI_RB_DISPLAY_GET() >= 100 ) {
			Res = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( GUI_FM_DISPLAY_GET() );
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Animation Run" , "Select" , "Off|On|Off(Last-Update)|Off(Result-Update)|Off(Last,Result-Update)" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( GUI_FM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_ROBOT_FM_ANIMATION( GUI_FM_DISPLAY_GET() , Res );
				}
			}
		}
		else {
			Res = _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( GUI_TM_DISPLAY_GET() );
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Animation Run" , "Select" , "Off|On|Off(Last-Update)|Off(Result-Update)|Off(Last,Result-Update)" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( GUI_TM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_ROBOT_ANIMATION( GUI_TM_DISPLAY_GET() , Res );
				}
			}
		}
		//
		if ( Change ) KWIN_GUI_INT_STRING_DISPLAY( hWnd , IDC_ANIMATION_RUN , Res , "Off|On|Off(L)|Off(R)|Off(LR)" , "????" ); // 2007.07.24
		break;

	case IDC_HOME_POSITION :
		if ( Control ) return TRUE;
		if ( GUI_RB_DISPLAY_GET() >= 100 ) {
			dRes = _i_SCH_PRM_GET_RB_FM_HOME_POSITION( GUI_FM_DISPLAY_GET() );
			if ( MODAL_ANALOG_BOX1( hWnd , "Robot Home Position" , "Select" , 0 , 360 , 2 , &dRes ) ) {
				if ( dRes != _i_SCH_PRM_GET_RB_FM_HOME_POSITION( GUI_FM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_FM_HOME_POSITION( GUI_FM_DISPLAY_GET() , dRes );
				}
			}
		}
		else {
			dRes = _i_SCH_PRM_GET_RB_HOME_POSITION( GUI_TM_DISPLAY_GET() );
			if ( MODAL_ANALOG_BOX1( hWnd , "Robot Home Position" , "Select" , 0 , 360 , 2 , &dRes ) ) {
				if ( dRes != _i_SCH_PRM_GET_RB_HOME_POSITION( GUI_TM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_HOME_POSITION( GUI_TM_DISPLAY_GET() , dRes );
				}
			}
		}
		//
		if ( Change ) KWIN_Item_Double_Display( hWnd , IDC_HOME_POSITION , dRes , 2 );
		break;

	case IDC_ROTATE_STYLE :
		if ( Control ) return TRUE;
		if ( GUI_RB_DISPLAY_GET() >= 100 ) {
			Res = _i_SCH_PRM_GET_RB_FM_ROTATION_STYLE( GUI_FM_DISPLAY_GET() );
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Rotate Style(FM)" , "Select" , "CW Home Based|CCW Home Based|CW ROTATION|CCW ROTATION|MINIMUM" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_RB_FM_ROTATION_STYLE( GUI_FM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_FM_ROTATION_STYLE( GUI_FM_DISPLAY_GET() , Res );
				}
			}
		}
		else {
			Res = _i_SCH_PRM_GET_RB_ROTATION_STYLE( GUI_TM_DISPLAY_GET() );
			if ( MODAL_DIGITAL_BOX1( hWnd , "Robot Rotate Style" , "Select" , "CW Home Based|CCW Home Based|CW ROTATION|CCW ROTATION|MINIMUM" , &Res ) ) {
				if ( Res != _i_SCH_PRM_GET_RB_ROTATION_STYLE( GUI_TM_DISPLAY_GET() ) ) {
					Change = TRUE;
					_i_SCH_PRM_SET_RB_ROTATION_STYLE( GUI_TM_DISPLAY_GET() , Res );
				}
			}
		}
		//
		if ( Change ) {
			switch( Res ) {
			case ROTATION_STYLE_CW :		KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CW_ROTATION" );	break;
			case ROTATION_STYLE_CCW :		KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CCW_ROTATION" );	break;
			case ROTATION_STYLE_MINIMUM :	KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "MINIMUM" );		break;
			case ROTATION_STYLE_HOME_CW :	KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CW_HOMEBASED" );	break;
			default	:						KWIN_Item_String_Display( hWnd , IDC_ROTATE_STYLE , "CCW_HOMEBASED" );	break;
			}
		}
		break;

	case IDC_ROTATE_A_C1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 1 , 0 , 360 );	break;
	case IDC_ROTATE_A_C2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 2 , 0 , 360 );	break;
	case IDC_ROTATE_A_C3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 3 , 0 , 360 );	break;
	case IDC_ROTATE_A_C4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 4 , 0 , 360 );	break;
	case IDC_ROTATE_A_P1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 5 , 0 , 360 );	break;
	case IDC_ROTATE_A_P2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 6 , 0 , 360 );	break;
	case IDC_ROTATE_A_P3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 7 , 0 , 360 );	break;
	case IDC_ROTATE_A_P4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 8 , 0 , 360 );	break;
	case IDC_ROTATE_A_P5 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 9 , 0 , 360 );	break;
	case IDC_ROTATE_A_P6 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 10 , 0 , 360 );	break;
	case IDC_ROTATE_A_AL :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 11 , 0 , 360 );	break;
	case IDC_ROTATE_A_IC :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Position" , msg , 12 , 0 , 360 );	break;
	//
	case IDC_EXTEND_A_C1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 1 , -100 , 100 );	break;
	case IDC_EXTEND_A_C2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 2 , -100 , 100 );	break;
	case IDC_EXTEND_A_C3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 3 , -100 , 100 );	break;
	case IDC_EXTEND_A_C4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 4 , -100 , 100 );	break;
	case IDC_EXTEND_A_P1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 5 , -100 , 100 );	break;
	case IDC_EXTEND_A_P2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 6 , -100 , 100 );	break;
	case IDC_EXTEND_A_P3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 7 , -100 , 100 );	break;
	case IDC_EXTEND_A_P4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 8 , -100 , 100 );	break;
	case IDC_EXTEND_A_P5 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 9 , -100 , 100 );	break;
	case IDC_EXTEND_A_P6 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 10 , -100 , 100 );	break;
	case IDC_EXTEND_A_AL :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 11 , -100 , 100 );	break;
	case IDC_EXTEND_A_IC :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_EXTEND , "EXTEND Position" , msg , 12 , -100 , 100 );	break;
	//
	case IDC_RETRACT_A_C1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 1 , -100 , 100 );	break;
	case IDC_RETRACT_A_C2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 2 , -100 , 100 );	break;
	case IDC_RETRACT_A_C3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 3 , -100 , 100 );	break;
	case IDC_RETRACT_A_C4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 4 , -100 , 100 );	break;
	case IDC_RETRACT_A_P1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 5 , -100 , 100 );	break;
	case IDC_RETRACT_A_P2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 6 , -100 , 100 );	break;
	case IDC_RETRACT_A_P3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 7 , -100 , 100 );	break;
	case IDC_RETRACT_A_P4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 8 , -100 , 100 );	break;
	case IDC_RETRACT_A_P5 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 9 , -100 , 100 );	break;
	case IDC_RETRACT_A_P6 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 10 , -100 , 100 );	break;
	case IDC_RETRACT_A_AL :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 11 , -100 , 100 );	break;
	case IDC_RETRACT_A_IC :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_RETRACT , "RETRACT Position" , msg , 12 , -100 , 100 );	break;
	//
	case IDC_UP_A_C1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 1 , 0 , 100 );	break;
	case IDC_UP_A_C2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 2 , 0 , 100 );	break;
	case IDC_UP_A_C3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 3 , 0 , 100 );	break;
	case IDC_UP_A_C4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 4 , 0 , 100 );	break;
	case IDC_UP_A_P1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 5 , 0 , 100 );	break;
	case IDC_UP_A_P2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 6 , 0 , 100 );	break;
	case IDC_UP_A_P3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 7 , 0 , 100 );	break;
	case IDC_UP_A_P4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 8 , 0 , 100 );	break;
	case IDC_UP_A_P5 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 9 , 0 , 100 );	break;
	case IDC_UP_A_P6 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 10 , 0 , 100 );	break;
	case IDC_UP_A_AL :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 11 , 0 , 100 );	break;
	case IDC_UP_A_IC :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_UP , "UP Position" , msg , 12 , 0 , 100 );	break;
	//
	case IDC_DOWN_A_C1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 1 , 0 , 100 );	break;
	case IDC_DOWN_A_C2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 2 , 0 , 100 );	break;
	case IDC_DOWN_A_C3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 3 , 0 , 100 );	break;
	case IDC_DOWN_A_C4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 4 , 0 , 100 );	break;
	case IDC_DOWN_A_P1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 5 , 0 , 100 );	break;
	case IDC_DOWN_A_P2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 6 , 0 , 100 );	break;
	case IDC_DOWN_A_P3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 7 , 0 , 100 );	break;
	case IDC_DOWN_A_P4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 8 , 0 , 100 );	break;
	case IDC_DOWN_A_P5 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 9 , 0 , 100 );	break;
	case IDC_DOWN_A_P6 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 10 , 0 , 100 );	break;
	case IDC_DOWN_A_AL :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 11 , 0 , 100 );	break;
	case IDC_DOWN_A_IC :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_DOWN , "DOWN Position" , msg , 12 , 0 , 100 );	break;
	//
	case IDC_MOVE_A_C1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 1 , -500 , 500 );	break;
	case IDC_MOVE_A_C2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 2 , -500 , 500 );	break;
	case IDC_MOVE_A_C3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 3 , -500 , 500 );	break;
	case IDC_MOVE_A_C4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 4 , -500 , 500 );	break;
	case IDC_MOVE_A_P1 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 5 , -500 , 500 );	break;
	case IDC_MOVE_A_P2 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 6 , -500 , 500 );	break;
	case IDC_MOVE_A_P3 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 7 , -500 , 500 );	break;
	case IDC_MOVE_A_P4 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 8 , -500 , 500 );	break;
	case IDC_MOVE_A_P5 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 9 , -500 , 500 );	break;
	case IDC_MOVE_A_P6 :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 10 , -500 , 500 );	break;
	case IDC_MOVE_A_AL :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 11 , -500 , 500 );	break;
	case IDC_MOVE_A_IC :	Change = GUI_RobotData_Control_Sub_Position_Set( Control , hWnd , RB_ANIM_MOVE , "MOVE Position" , msg , 12 , -500 , 500 );	break;

	case IDC_ROTATE_A_SPEED :	Change = GUI_RobotData_Control_Sub_Speed_Set( Control , hWnd , RB_ANIM_ROTATE , "Rotate Speed" , msg );		break;
	case IDC_EXTEND_A_SPEED :	Change = GUI_RobotData_Control_Sub_Speed_Set( Control , hWnd , RB_ANIM_EXTEND , "Extend Speed" , msg );		break;
	case IDC_RETRACT_A_SPEED :	Change = GUI_RobotData_Control_Sub_Speed_Set( Control , hWnd , RB_ANIM_RETRACT , "Retract Speed" , msg );	break;
	case IDC_UP_A_SPEED :		Change = GUI_RobotData_Control_Sub_Speed_Set( Control , hWnd , RB_ANIM_UP , "Up Speed" , msg );				break;
	case IDC_DOWN_A_SPEED :		Change = GUI_RobotData_Control_Sub_Speed_Set( Control , hWnd , RB_ANIM_DOWN , "Down Speed" , msg );			break;
	case IDC_MOVE_A_SPEED :		Change = GUI_RobotData_Control_Sub_Speed_Set( Control , hWnd , RB_ANIM_MOVE , "Move Speed" , msg );			break;
	}
	//
	if ( Change ) {
		Set_RunPrm_IO_Setting( 9 ); // 2003.01.24
		GUI_SAVE_PARAMETER_DATA( 2 );
	}
	return TRUE;
}
//
BOOL APIENTRY Gui_IDD_ROBOTDATA_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT ps;
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
		return TRUE;

	case WM_PAINT:
		BeginPaint( hdlg, &ps );
		GUI_RobotData_Display( hdlg , ps.hdc );
		EndPaint( hdlg , &ps );
		return TRUE;

	case WM_COMMAND :
		switch( wParam ) {
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		default :
			return( GUI_RobotData_Control( hdlg , LOWORD( wParam ) , Control ) );
		}
		return TRUE;

	}
	return FALSE;
}
