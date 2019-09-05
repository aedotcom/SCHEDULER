#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "Timer_Handling.h"
#include "Utility.h"
#include "sch_TransferMaintInf.h"
#include "resource.h"

//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================

//enum { _EST_MIN , _EST_MAX , _EST_SUM , _EST_SUM2 };

enum { _EST1_PRE_PROCES , _EST1_PLACE , _EST1_PROCESS , _EST1_PICK , _EST1_POST_PROCESS };
enum { _EST1_MODE_READY , _EST1_MODE_PRE_PROCESS , _EST1_MODE_PLACE , _EST1_MODE_PROCESS , _EST1_MODE_PICK , _EST1_MODE_POST_PROCESS };

enum { _EST2_FM_SIDE , _EST2_TM_SIDE , _EST2_PLACE , _EST2_PICK };
enum { _EST2_MODE_READY , _EST2_MODE_FM_SIDE , _EST2_MODE_TM_SIDE , _EST2_MODE_PLACE , _EST2_MODE_PICK };

enum { _EST3_PICK , _EST3_PLACE };
enum { _EST3_MODE_READY , _EST3_MODE_PICK , _EST3_MODE_PLACE };



//=======================================================================================================================================

typedef struct {
	//
	//----------------------------------
	//
//	int		COUNT[7];
//	double	DATA[7][4];
	//
	//----------------------------------
	//
	int		_i_MODE;
	int		_i_MODE2;

	int		_i_Timer[5];
	//
	//----------------------------------
	//
} EstimateBPMTemplate;

//=======================================================================================================================================

EstimateBPMTemplate EST_BPM[MAX_CHAMBER];

int	    EST_CONTROL    = 0;	// 0 : x , 1:All , 2 :timecheck(Only)
int		EST_LOG	= 0;

double	EST_RNG_PM_PRE    = 0;
double	EST_RNG_PM_PLACE  = 0;
double	EST_RNG_PM_PICK   = 0;
double	EST_RNG_PM_POST   = 0;

double	EST_RNG_BM_TMSIDE  = 0;
double	EST_RNG_BM_FMSIDE  = 0;
double	EST_RNG_BM_PICK    = 0;
double	EST_RNG_BM_PLACE   = 0;

double	EST_RNG_CM_PICK    = 0;
double	EST_RNG_CM_PLACE   = 0;

void EST_CONTROL_RUN( int mode , int pmtime , int bmtime , int cmtime ) {
	//
	// 0 : 0
	// 1 : 1 x-log
	// 2 : 2 x-log
	// 3 : 1 log
	// 4 : 2 log
	//
	switch( mode ) {
	case 1 : // nolog
		EST_CONTROL = 1;
		EST_LOG = 0;
		break;
	case 2 : // nolog
		EST_CONTROL = 2;
		EST_LOG = 0;
		break;
	case 3 : // log
		EST_CONTROL = 1;
		EST_LOG = 1;
		break;
	case 4 : // log
		EST_CONTROL = 2;
		EST_LOG = 1;
		break;
	default : // nolog
		EST_CONTROL = 0;
		EST_LOG = 0;
		break;
	}
	//
	if ( pmtime >= 0 ) {
		EST_RNG_PM_PRE    = (double) pmtime;
		EST_RNG_PM_PLACE  = (double) pmtime;
		EST_RNG_PM_PICK   = (double) pmtime;
		EST_RNG_PM_POST   = (double) pmtime;
	}

	if ( bmtime >= 0 ) {
		EST_RNG_BM_TMSIDE  = (double) bmtime;
		EST_RNG_BM_FMSIDE  = (double) bmtime;
		EST_RNG_BM_PICK    = (double) bmtime;
		EST_RNG_BM_PLACE   = (double) bmtime;
	}

	if ( cmtime >= 0 ) {
		EST_RNG_CM_PICK    = (double) cmtime;
		EST_RNG_CM_PLACE   = (double) cmtime;
	}
}
//=======================================================================================================================================
void _IO_RPT_PRINTF( char *SeparateSting , int x , BOOL appstr , LPSTR list , ... ) {
	FILE *fpt;
	char FileName[64];
	SYSTEMTIME		SysTime;
	va_list va;
	char TextBuffer[1024+1];

	if ( SYSTEM_LOGSKIP_STATUS() ) return;

	GetLocalTime( &SysTime );
	//
	sprintf( FileName , "%s\\LOTLOG\\REPORT-%04d%02d%02d.log" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth , SysTime.wDay );
	//
	fpt = fopen( FileName , "a" );
	if ( fpt != NULL ) {
		va_start( va , list );
		_vsnprintf( TextBuffer , 1024 , list , (LPSTR) va );
		va_end( va );
		if ( appstr ) {
			fprintf( fpt , "%04d/%02d/%02d %02d:%02d:%02d\t%s%d\t%s\t<CHECK>\n" ,
				SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ,
				SeparateSting , x , 
				TextBuffer );
		}
		else {
			fprintf( fpt , "%04d/%02d/%02d %02d:%02d:%02d\t%s%d\t%s\n" ,
				SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond ,
				SeparateSting , x , 
				TextBuffer );
		}
		fclose( fpt );
	}
	//
	if ( EST_LOG ) {
		sprintf( FileName , "%s\\LOTLOG\\REPORT-%04d%02d%02d.dbg" , _i_SCH_PRM_GET_DFIX_LOG_PATH() , SysTime.wYear , SysTime.wMonth , SysTime.wDay );
		_i_SCH_SYSTEM_INFO_LOG( FileName , FileName );
	}
	//
}

/*
double _i_EST_Get_Var_Value( double sum , double avr , int cnt ) {
	double var;
	if ( cnt <= 0 ) return 0;
	var = ( sum / (double) cnt ) - (avr*avr);
	if ( var <= 0 ) return 0;
	return var;
}


void _i_EST_Get_AVR_VAR_Value( int cnt , double sum , double sum2 , double *avr , double *var ) {
	//
	if ( cnt > 0 ) {
		//
		*avr = ( sum / (double) cnt );
		//
		*var = _i_EST_Get_Var_Value( sum2 , *avr , cnt );
		//
	}
	else {
		*avr = 0;
		*var = 0;
	}
	//
}
*/

double _i_EST_ELAPSED_TIME( int timer ) {
	return (double) ( (double) ( GetTickCount() - timer ) / (double) 1000 );
}

//=======================================================================================================================================

void _i_EST_BPM_SET_TIME( int pm , int index ) {
	EST_BPM[pm]._i_Timer[index] = GetTickCount();
}

/*
void _i_EST_BPM_CALCULATE( int pm , int index ) {
	double elapsed;
	//
	elapsed = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[index] );
	//
	if ( EST_BPM[pm].COUNT[index] <= 0 ) {
		EST_BPM[pm].COUNT[index]  = 1;
		EST_BPM[pm].DATA[index][_EST_MIN]  = elapsed;
		EST_BPM[pm].DATA[index][_EST_MAX]  = elapsed;
		EST_BPM[pm].DATA[index][_EST_SUM]  = elapsed;
		EST_BPM[pm].DATA[index][_EST_SUM2] = ( elapsed * elapsed );
	}
	else {
		EST_BPM[pm].COUNT[index]++;
		if ( EST_BPM[pm].DATA[index][_EST_MIN] > elapsed ) EST_BPM[pm].DATA[index][_EST_MIN] = elapsed;
		if ( EST_BPM[pm].DATA[index][_EST_MAX] < elapsed ) EST_BPM[pm].DATA[index][_EST_MAX] = elapsed;
		EST_BPM[pm].DATA[index][_EST_SUM]  = EST_BPM[pm].DATA[index][_EST_SUM] + elapsed;
		EST_BPM[pm].DATA[index][_EST_SUM2] = EST_BPM[pm].DATA[index][_EST_SUM2] + ( elapsed * elapsed );
	}
	//
}
*/

void EST_SETTING( int ch , int mode , int tm ) {
	if ( mode >= 0 ) EST_BPM[ch]._i_MODE = mode;
	_i_EST_BPM_SET_TIME( ch , tm );
}
void EST_SETTING2( int ch , int mode , int tm ) {
	if ( mode >= 0 ) EST_BPM[ch]._i_MODE2 = mode;
	_i_EST_BPM_SET_TIME( ch , tm );
}

int EST_CHECKING( double t , double checkt ) {
	//
	if ( t < 0 ) return -1;
	//
	if ( EST_CONTROL == 0 ) return -1;
	//
	if      ( EST_CONTROL == 1 ) { // log All
		if ( checkt > 0 ) {
			if ( t > checkt ) return 1;
		}
	}
	else if ( EST_CONTROL == 2 ) { // log checked
		if ( checkt <= 0 ) return -1;
		if ( t <= checkt ) return -1;
		return 1;
	}
	//
	return 0;
}
//
//=======================================================================================================================================

void EST_BPM_RUN_INIT() {
	int i , j;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		EST_BPM[i]._i_MODE = _EST1_MODE_READY;
		EST_BPM[i]._i_MODE2 = _EST1_MODE_READY;
		//
		for ( j = 0 ; j < 5 ; j++ ) {
			//
			EST_BPM[i]._i_Timer[j] = 0;
			//
		}
	}
	//
}

//=======================================================================================================================================

void EST_PM_BEGIN( int pm ) {
	//
	EST_BPM[pm]._i_MODE = _EST1_MODE_READY;
	//
}

void EST_PM_PRE_PROCESS_START( int pm ) {
	//
	int Res;
	double t;
	//
	switch ( EST_BPM[pm]._i_MODE ) {
	case _EST1_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_PICK] );
		//
		break;

	case _EST1_MODE_POST_PROCESS :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_POST_PROCESS] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_SETTING( pm , _EST1_MODE_PRE_PROCESS , _EST1_PRE_PROCES );
	//
	Res = EST_CHECKING( t , EST_RNG_PM_PRE );	if ( Res == -1 ) return;
	//
	_IO_RPT_PRINTF( "PM" , pm - PM1 + 1 , Res , "PRE	%.3f\n" , t );
	//
}

void EST_PM_PRE_PROCESS_END( int pm ) {
	//
	EST_SETTING( pm , -1 , _EST1_PRE_PROCES );
	//
}

void EST_PM_PLACE_START( int pm ) {
	//
	int Res;
	double t;
	//
	switch ( EST_BPM[pm]._i_MODE ) {
	case _EST1_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_PICK] );
		//
		break;

	case _EST1_MODE_POST_PROCESS :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_POST_PROCESS] );
		//
		break;

	case _EST1_MODE_PRE_PROCESS :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_PRE_PROCES] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_SETTING( pm , _EST1_MODE_PLACE , _EST1_PLACE );
	//
	Res = EST_CHECKING( t , EST_RNG_PM_PLACE );	if ( Res == -1 ) return;
	//
	_IO_RPT_PRINTF( "PM" , pm - PM1 + 1 , Res , "PLACE	%.3f\n" , t );
	//
}

void EST_PM_PLACE_END( int pm ) {
	EST_SETTING( pm , -1 , _EST1_PLACE );
}


void EST_PM_PROCESS_START( int pm ) {
	EST_SETTING( pm , _EST1_MODE_PROCESS , _EST1_PROCESS );
}

void EST_PM_PROCESS_END( int pm ) {
	EST_SETTING( pm , -1 , _EST1_PROCESS );
}

void EST_PM_PICK_START( int pm ) {
	//
	int Res;
	double t;
	//
	switch ( EST_BPM[pm]._i_MODE ) {
	case _EST1_MODE_PLACE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_PLACE] );
		//
		break;

	case _EST1_MODE_PROCESS :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_PROCESS] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_SETTING( pm , _EST1_MODE_PICK , _EST1_PICK );
	//
	Res = EST_CHECKING( t , EST_RNG_PM_PICK );	if ( Res == -1 ) return;
	//
	_IO_RPT_PRINTF( "PM" , pm - PM1 + 1 , Res , "PICK	%.3f\n" , t );
	//
}

void EST_PM_PICK_END( int pm ) {
	EST_SETTING( pm , -1 , _EST1_PICK );
}

void EST_PM_POST_PROCESS_START( int pm ) {
	//
	int Res;
	double t;
	//
	switch ( EST_BPM[pm]._i_MODE ) {
	case _EST1_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[pm]._i_Timer[_EST1_PICK] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_SETTING( pm , _EST1_MODE_POST_PROCESS , _EST1_POST_PROCESS );
	//
	Res = EST_CHECKING( t , EST_RNG_PM_POST );	if ( Res == -1 ) return;
	//
	_IO_RPT_PRINTF( "PM" , pm - PM1 + 1 , Res , "POST	%.3f\n" , t );
	//
}

void EST_PM_POST_PROCESS_END( int pm ) {
	EST_SETTING( pm , -1 , _EST1_POST_PROCESS );
}


void EST_PM_END( int pm ) {
	EST_BPM[pm]._i_MODE = _EST1_MODE_READY;
}

//=======================================================================================
//=======================================================================================

void EST_BM_BEGIN( int bm ) {
	//
	EST_BPM[bm]._i_MODE = _EST2_MODE_READY;
	//
}

void EST_BM_FM_SIDE_START( int bm ) {
	//
	double t;
	//
	switch ( EST_BPM[bm]._i_MODE ) {
	case _EST2_MODE_FM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_FM_SIDE] );
		//
		break;

	case _EST2_MODE_TM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_TM_SIDE] );
		//
		break;

	case _EST2_MODE_PLACE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PLACE] );
		//
		break;

	case _EST2_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PICK] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_BPM[bm]._i_MODE = _EST2_MODE_FM_SIDE;
	_i_EST_BPM_SET_TIME( bm , _EST2_FM_SIDE );
	//
	if ( t < 0 ) return;
	if ( EST_CONTROL == 0 ) return;
	//
	if ( EST_CONTROL == 2 ) {
		if ( EST_RNG_BM_FMSIDE <= 0 ) return;
		if ( t <= EST_RNG_BM_FMSIDE ) return;
	}
	//
//	_IO_RPT_PRINTF( "BM" , bm - BM1 + 1 , "FMSIDE	%.3f\n" , t );
	//
}

void EST_BM_FM_SIDE_END( int bm ) {
	_i_EST_BPM_SET_TIME( bm , _EST2_FM_SIDE );
}

void EST_BM_TM_SIDE_START( int bm ) {
	//
	double t;
	//
	switch ( EST_BPM[bm]._i_MODE ) {
	case _EST2_MODE_FM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_FM_SIDE] );
		//
		break;

	case _EST2_MODE_TM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_TM_SIDE] );
		//
		break;

	case _EST2_MODE_PLACE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PLACE] );
		//
		break;

	case _EST2_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PICK] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_BPM[bm]._i_MODE = _EST2_MODE_TM_SIDE;
	_i_EST_BPM_SET_TIME( bm , _EST2_TM_SIDE );
	//
	if ( t < 0 ) return;
	if ( EST_CONTROL == 0 ) return;
	//
	if ( EST_CONTROL == 2 ) {
		if ( EST_RNG_BM_TMSIDE <= 0 ) return;
		if ( t <= EST_RNG_BM_TMSIDE ) return;
	}
	//
//	_IO_RPT_PRINTF( "BM" , bm - BM1 + 1 , "TMSIDE	%.3f\n" , t );
	//
}

void EST_BM_TM_SIDE_END( int bm ) {
	_i_EST_BPM_SET_TIME( bm , _EST2_TM_SIDE );
}

void EST_BM_PLACE_START( int bm , int tms ) {
	//
	double t;
	//
	switch ( EST_BPM[bm]._i_MODE ) {
	case _EST2_MODE_FM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_FM_SIDE] );
		//
		break;

	case _EST2_MODE_TM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_TM_SIDE] );
		//
		break;

	case _EST2_MODE_PLACE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PLACE] );
		//
		break;

	case _EST2_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PICK] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_BPM[bm]._i_MODE = _EST2_MODE_PLACE;
	_i_EST_BPM_SET_TIME( bm , _EST2_PLACE );
	//
	if ( t < 0 ) return;
	if ( EST_CONTROL == 0 ) return;
	//
	if ( EST_CONTROL == 2 ) {
		if ( EST_RNG_BM_PLACE <= 0 ) return;
		if ( t <= EST_RNG_BM_PLACE ) return;
	}
	//
//	_IO_RPT_PRINTF( "BM" , bm - BM1 + 1 , "PLACE	%.3f\n" , t );
	//
}

void EST_BM_PLACE_END( int bm , int tms ) {
	_i_EST_BPM_SET_TIME( bm , _EST2_PLACE );
}

void EST_BM_PICK_START( int bm , int tms ) {
	//
	double t;
	//
	switch ( EST_BPM[bm]._i_MODE ) {
	case _EST2_MODE_FM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_FM_SIDE] );
		//
		break;

	case _EST2_MODE_TM_SIDE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_TM_SIDE] );
		//
		break;

	case _EST2_MODE_PLACE :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PLACE] );
		//
		break;

	case _EST2_MODE_PICK :
		//
		t = _i_EST_ELAPSED_TIME( EST_BPM[bm]._i_Timer[_EST2_PICK] );
		//
		break;

	default :
		t = -1;
		//
		break;
	}
	//
	EST_BPM[bm]._i_MODE = _EST2_MODE_PICK;
	_i_EST_BPM_SET_TIME( bm , _EST2_PICK );
	//
	if ( t < 0 ) return;
	if ( EST_CONTROL == 0 ) return;
	//
	if ( EST_CONTROL == 2 ) {
		if ( EST_RNG_BM_PICK <= 0 ) return;
		if ( t <= EST_RNG_BM_PICK ) return;
	}
	//
//	_IO_RPT_PRINTF( "BM" , bm - BM1 + 1 , "PICK	%.3f\n" , t );
	//
}

void EST_BM_PICK_END( int bm , int tms ) {
	_i_EST_BPM_SET_TIME( bm , _EST2_PICK );
}

//=======================================================================================
//=======================================================================================

void EST_CM_BEGIN( int cm ) {
	//
	EST_BPM[cm]._i_MODE = _EST3_MODE_READY;
	EST_BPM[cm]._i_MODE2 = _EST3_MODE_READY;
	//
}

void EST_CM_END( int cm ) {
	//
	EST_BPM[cm]._i_MODE = _EST3_MODE_READY;
	EST_BPM[cm]._i_MODE2 = _EST3_MODE_READY;
	//
}

void EST_CM_PLACE_START( int cm ) {
	//
	int Res;
	double t;
	//
	if ( EST_BPM[cm]._i_MODE2 == _EST3_MODE_READY ) {
		t = -1;
	}
	else {
		t = _i_EST_ELAPSED_TIME( EST_BPM[cm]._i_Timer[_EST3_PLACE] );
	}
	//
	EST_SETTING2( cm , _EST3_MODE_PLACE , _EST3_PLACE );
	//
	Res = EST_CHECKING( t , EST_RNG_CM_PLACE );	if ( Res == -1 ) return;
	//
	_IO_RPT_PRINTF( "CM" , cm - CM1 + 1 , Res , "PLACE	%.3f\n" , t );
	//
}

void EST_CM_PLACE_END( int cm ) {
	EST_SETTING2( cm , -1 , _EST3_PLACE );
}

void EST_CM_PICK_START( int cm ) {
	//
	int Res;
	double t;
	//
	if ( EST_BPM[cm]._i_MODE == _EST3_MODE_READY ) {
		t = -1;
	}
	else {
		t = _i_EST_ELAPSED_TIME( EST_BPM[cm]._i_Timer[_EST3_PICK] );
	}
	//
	EST_SETTING( cm , _EST3_MODE_PICK , _EST3_PICK );
	//
	Res = EST_CHECKING( t , EST_RNG_CM_PICK );	if ( Res == -1 ) return;
	//
	_IO_RPT_PRINTF( "CM" , cm - CM1 + 1 , Res , "PICK	%.3f\n" , t );
	//
}

void EST_CM_PICK_END( int cm ) {
	EST_SETTING( cm , -1 , _EST3_PICK );
}

//=======================================================================================
//=======================================================================================

void EST_TM_RUN_START() {
	int i;
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		EST_BPM[i+TM]._i_MODE = _EST3_MODE_READY;
		//
	}
}

void EST_TM_PLACE_START( int tmside ) {
	EST_BPM[tmside+TM]._i_MODE = _EST3_MODE_PLACE;
	_i_EST_BPM_SET_TIME( tmside+TM , _EST3_PLACE );
}

void EST_TM_PLACE_END( int tmside ) {
	_i_EST_BPM_SET_TIME( tmside+TM , _EST3_PLACE );
}

void EST_TM_PICK_START( int tmside ) {
	EST_BPM[tmside+TM]._i_MODE = _EST3_MODE_PICK;
	_i_EST_BPM_SET_TIME( tmside+TM , _EST3_PICK );
}

void EST_TM_PICK_END( int tmside ) {
	_i_EST_BPM_SET_TIME( tmside+TM , _EST3_PICK );
}


//=======================================================================================
//=======================================================================================

void EST_FM_RUN_START() {
	//
	EST_BPM[FM]._i_MODE = _EST3_MODE_READY;
	//
}

void EST_FM_PLACE_START() {
	EST_BPM[FM]._i_MODE = _EST3_MODE_PLACE;
	_i_EST_BPM_SET_TIME( FM , _EST3_PLACE );
}

void EST_FM_PLACE_END() {
	_i_EST_BPM_SET_TIME( FM , _EST3_PLACE );
}

void EST_FM_PICK_START() {
	EST_BPM[FM]._i_MODE = _EST3_MODE_PICK;
	_i_EST_BPM_SET_TIME( FM , _EST3_PICK );
}

void EST_FM_PICK_END() {
	_i_EST_BPM_SET_TIME( FM , _EST3_PICK );
}

//=======================================================================================
//=======================================================================================
/*
void EST_REPORT() {
	//
	int i;
	double avr,var;
	FILE *fpt;
	//
	fpt = fopen( "report.txt" , "w" );
	//
	if ( fpt == NULL ) return;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !EST_BPM[i+PM1]._i_START_HOLD_FIND ) continue;
		//
		fprintf( fpt , "[PM%d]		COUNT	MIN	MAX	AVR	STD\n" , i + 1 );
		//
		fprintf( fpt , "	START_HOLD	%.1f\n" , EST_BPM[i+PM1].START_HOLD_TIME );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_PRE_PROCES] , EST_BPM[i+PM1].DATA[_EST1_PRE_PROCES][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_PRE_PROCES][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PRE_PROCESS	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_PRE_PROCES] , EST_BPM[i+PM1].DATA[_EST1_PRE_PROCES][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_PRE_PROCES][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_PLACE_HOLD] , EST_BPM[i+PM1].DATA[_EST1_PLACE_HOLD][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_PLACE_HOLD][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PLACE_HOLD	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_PLACE_HOLD] , EST_BPM[i+PM1].DATA[_EST1_PLACE_HOLD][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_PLACE_HOLD][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_PLACE] , EST_BPM[i+PM1].DATA[_EST1_PLACE][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_PLACE][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PLACE	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_PLACE] , EST_BPM[i+PM1].DATA[_EST1_PLACE][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_PLACE][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_PROCESS] , EST_BPM[i+PM1].DATA[_EST1_PROCESS][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_PROCESS][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PROCESS	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_PROCESS] , EST_BPM[i+PM1].DATA[_EST1_PROCESS][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_PROCESS][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_PICK_HOLD] , EST_BPM[i+PM1].DATA[_EST1_PICK_HOLD][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_PICK_HOLD][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PICK_HOLD	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_PICK_HOLD] , EST_BPM[i+PM1].DATA[_EST1_PICK_HOLD][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_PICK_HOLD][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_PICK] , EST_BPM[i+PM1].DATA[_EST1_PICK][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_PICK][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PICK	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_PICK] , EST_BPM[i+PM1].DATA[_EST1_PICK][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_PICK][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+PM1].COUNT[_EST1_POST_PROCESS] , EST_BPM[i+PM1].DATA[_EST1_POST_PROCESS][_EST_SUM] , EST_BPM[i+PM1].DATA[_EST1_POST_PROCESS][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	POST_PROCESS	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+PM1].COUNT[_EST1_POST_PROCESS] , EST_BPM[i+PM1].DATA[_EST1_POST_PROCESS][_EST_MIN] , EST_BPM[i+PM1].DATA[_EST1_POST_PROCESS][_EST_MAX] , avr , var );
		//
		fprintf( fpt , "	END_HOLD	%.1f\n" , EST_BPM[i+PM1].END_HOLD_TIME );
		//
	}
	//
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !EST_BPM[i+BM1]._i_START_HOLD_FIND ) continue;
		//
		fprintf( fpt , "[BM%d]		COUNT	MIN	MAX	AVR	STD\n" , i + 1 );
		//
		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+BM1].COUNT[_EST2_FM_SIDE] , EST_BPM[i+BM1].DATA[_EST2_FM_SIDE][_EST_SUM] , EST_BPM[i+BM1].DATA[_EST2_FM_SIDE][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	FM_SIDE	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+BM1].COUNT[_EST2_FM_SIDE] , EST_BPM[i+BM1].DATA[_EST2_FM_SIDE][_EST_MIN] , EST_BPM[i+BM1].DATA[_EST2_FM_SIDE][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+BM1].COUNT[_EST2_TM_SIDE] , EST_BPM[i+BM1].DATA[_EST2_TM_SIDE][_EST_SUM] , EST_BPM[i+BM1].DATA[_EST2_TM_SIDE][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	TM_SIDE	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+BM1].COUNT[_EST2_TM_SIDE] , EST_BPM[i+BM1].DATA[_EST2_TM_SIDE][_EST_MIN] , EST_BPM[i+BM1].DATA[_EST2_TM_SIDE][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+BM1].COUNT[_EST2_PLACE] , EST_BPM[i+BM1].DATA[_EST2_PLACE][_EST_SUM] , EST_BPM[i+BM1].DATA[_EST2_PLACE][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PLACE	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+BM1].COUNT[_EST2_PLACE] , EST_BPM[i+BM1].DATA[_EST2_PLACE][_EST_MIN] , EST_BPM[i+BM1].DATA[_EST2_PLACE][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+BM1].COUNT[_EST2_PICK] , EST_BPM[i+BM1].DATA[_EST2_PICK][_EST_SUM] , EST_BPM[i+BM1].DATA[_EST2_PICK][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PICK	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+BM1].COUNT[_EST2_PICK] , EST_BPM[i+BM1].DATA[_EST2_PICK][_EST_MIN] , EST_BPM[i+BM1].DATA[_EST2_PICK][_EST_MAX] , avr , var );

		//
	}
	//
	for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !EST_BPM[i+TM]._i_START_HOLD_FIND ) continue;
		//
		fprintf( fpt , "[TM%d]		COUNT	MIN	MAX	AVR	STD\n" , i + 1 );
		//
		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+TM].COUNT[_EST3_PICK] , EST_BPM[i+TM].DATA[_EST3_PICK][_EST_SUM] , EST_BPM[i+TM].DATA[_EST3_PICK][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PICK	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+TM].COUNT[_EST3_PICK] , EST_BPM[i+TM].DATA[_EST3_PICK][_EST_MIN] , EST_BPM[i+TM].DATA[_EST3_PICK][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+TM].COUNT[_EST3_PLACE] , EST_BPM[i+TM].DATA[_EST3_PLACE][_EST_SUM] , EST_BPM[i+TM].DATA[_EST3_PLACE][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PLACE	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+TM].COUNT[_EST3_PLACE] , EST_BPM[i+TM].DATA[_EST3_PLACE][_EST_MIN] , EST_BPM[i+TM].DATA[_EST3_PLACE][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[i+TM].COUNT[_EST3_WAIT] , EST_BPM[i+TM].DATA[_EST3_WAIT][_EST_SUM] , EST_BPM[i+TM].DATA[_EST3_WAIT][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	WAIT	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[i+TM].COUNT[_EST3_WAIT] , EST_BPM[i+TM].DATA[_EST3_WAIT][_EST_MIN] , EST_BPM[i+TM].DATA[_EST3_WAIT][_EST_MAX] , avr , var );
		//
	}
	//
	if ( EST_BPM[FM]._i_START_HOLD_FIND ) {
		//
		fprintf( fpt , "[FM]		COUNT	MIN	MAX	AVR	STD\n" );
		//
		_i_EST_Get_AVR_VAR_Value( EST_BPM[FM].COUNT[_EST3_PICK] , EST_BPM[FM].DATA[_EST3_PICK][_EST_SUM] , EST_BPM[FM].DATA[_EST3_PICK][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PICK	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[FM].COUNT[_EST3_PICK] , EST_BPM[FM].DATA[_EST3_PICK][_EST_MIN] , EST_BPM[FM].DATA[_EST3_PICK][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[FM].COUNT[_EST3_PLACE] , EST_BPM[FM].DATA[_EST3_PLACE][_EST_SUM] , EST_BPM[FM].DATA[_EST3_PLACE][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	PLACE	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[FM].COUNT[_EST3_PLACE] , EST_BPM[FM].DATA[_EST3_PLACE][_EST_MIN] , EST_BPM[FM].DATA[_EST3_PLACE][_EST_MAX] , avr , var );

		_i_EST_Get_AVR_VAR_Value( EST_BPM[FM].COUNT[_EST3_WAIT] , EST_BPM[FM].DATA[_EST3_WAIT][_EST_SUM] , EST_BPM[FM].DATA[_EST3_WAIT][_EST_SUM2] , &avr , &var );
		//
		fprintf( fpt , "	WAIT	%d	%.1f	%1.f	%.1f	%.1f\n" , EST_BPM[FM].COUNT[_EST3_WAIT] , EST_BPM[FM].DATA[_EST3_WAIT][_EST_MIN] , EST_BPM[FM].DATA[_EST3_WAIT][_EST_MAX] , avr , var );
		//
	}
	//
	fclose( fpt );
	//
}
*/
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================================================================
#define LOT_DATA_GET_ESTIMATE_BUF_R	10
#define LOT_DATA_GET_ESTIMATE_BUF_P	4
#define LOT_DATA_GET_ESTIMATE_BUF_I	8
int LOT_DATA_EST_SUPPLY_INTERVAL[MAX_CASSETTE_SIDE] = { 0 , 0 , 0 , 0 };
int LOT_DATA_GET_ESTIMATE_TIME( int side , time_t *tdata , int *spint ) {
	int i , j , k , pmc , temp , exptime , notrun , currun , donrun , oldStartTime = 0 , PrcsTime = 0 , FirstPrgsTime = 0 , LastPrgsTime = 0;
	time_t t_temp;
	int RunningTime;
	int SupplyIntTime;
	int rbc = 0 , rbf[LOT_DATA_GET_ESTIMATE_BUF_R];
	int tbc = 0 , tbf[LOT_DATA_GET_ESTIMATE_BUF_P];
	int osc = 0 , osf[LOT_DATA_GET_ESTIMATE_BUF_I];

	//=================================================================
	if ( _i_SCH_SYSTEM_USING_GET( side ) <= 0 ) return 0;
	//=================================================================
//	if ( !_i_SCH_SYSTEM_USING_RUNNING( side ) ) return 0; // 2006.09.19
	if ( _i_SCH_SYSTEM_USING_GET( side ) < 6 ) return 0; // 2006.09.19
	//=================================================================
	*tdata = 0;
	//=================================================================
	notrun = 0;
	currun = 0;
	donrun = 0;
	//=================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
		if ( ( _i_SCH_CLUSTER_Get_StartTime( side, i ) == 0 ) && ( _i_SCH_CLUSTER_Get_EndTime( side, i ) == 0 ) ) {
			notrun++;
		}
		else if ( _i_SCH_CLUSTER_Get_EndTime( side, i ) == 0 ) {
			//--------------------------------------------------------------------------------------------------------------------------------------
			currun++;
			//--------------------------------------------------------------------------------------------------------------------------------------
			temp = ( GetTickCount() - _i_SCH_CLUSTER_Get_StartTime( side, i ) ) / 1000;
			//--------------------------------------------------------------------------------------------------------------------------------------
			if ( rbc < LOT_DATA_GET_ESTIMATE_BUF_R ) {
				rbf[rbc] = temp;
				rbc++;
			}
			else {
				for ( j = 0 ; j < ( LOT_DATA_GET_ESTIMATE_BUF_R - 1 ) ; j++ ) {
					rbf[j] = rbf[j+1];
				}
				rbf[j] = temp;
			}
			//--------------------------------------------------------------------------------------------------------------------------------------
			if ( ( FirstPrgsTime == 0 ) || ( FirstPrgsTime < temp ) ) FirstPrgsTime = temp;
			if ( ( LastPrgsTime  == 0 ) || ( LastPrgsTime  > temp ) ) LastPrgsTime = temp;
			//--------------------------------------------------------------------------------------------------------------------------------------
			if ( oldStartTime != 0 ) {
				temp = ( _i_SCH_CLUSTER_Get_StartTime( side, i ) - oldStartTime ) / 1000;
				//
				if ( temp > 0 ) {
					if ( osc < LOT_DATA_GET_ESTIMATE_BUF_I ) {
						osf[osc] = temp;
						osc++;
					}
					else {
						for ( j = 0 ; j < ( LOT_DATA_GET_ESTIMATE_BUF_I - 1 ) ; j++ ) {
							osf[j] = osf[j+1];
						}
						osf[j] = temp;
					}
				}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------
			oldStartTime = _i_SCH_CLUSTER_Get_StartTime( side, i );
			//--------------------------------------------------------------------------------------------------------------------------------------
		}
		else {
			//
			donrun++;
			//
			temp = ( _i_SCH_CLUSTER_Get_EndTime( side, i ) - _i_SCH_CLUSTER_Get_StartTime( side, i ) ) / 1000;
			//
			if ( tbc < LOT_DATA_GET_ESTIMATE_BUF_P ) {
				tbf[tbc] = temp;
				tbc++;
			}
			else {
				for ( j = 0 ; j < ( LOT_DATA_GET_ESTIMATE_BUF_P - 1 ) ; j++ ) {
					tbf[j] = tbf[j+1];
				}
				tbf[j] = temp;
			}
		}
	}
	//=================================================================
	if ( ( notrun == 0 ) && ( currun == 0 ) ) {
		return 0;
	}
	//=================================================================
	if ( tbc <= 0 ) {
		RunningTime = 0;
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
			//
			exptime = KPLT_GET_FM_PICK_ROBOT_TIME( 0 , 0 ) + KPLT_GET_FM_PLACE_ROBOT_TIME( 0 , 0 ) +
				KPLT_GET_TM_PICK_ROBOT_TIME( 0 , TRUE ) + KPLT_GET_TM_PLACE_ROBOT_TIME( 0 , TRUE );
			//
			for ( j = 0 ; j < _i_SCH_CLUSTER_Get_PathRange( side , i ) ; j++ ) {
				PrcsTime = 0;
				for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
					pmc = _i_SCH_CLUSTER_Get_PathProcessChamber( side , i , j , k );
					if ( pmc > 0 ) {
						oldStartTime = _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 0 , pmc ) + _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 2 , pmc ) + _i_SCH_TIMER_GET_PROCESS_TIME_RUNPLAN( 1 , pmc );
						if ( oldStartTime > PrcsTime ) PrcsTime = oldStartTime;
					}
				}
				//==============================
				exptime = exptime + PrcsTime + KPLT_GET_TM_PICK_ROBOT_TIME( 0 , FALSE ) + KPLT_GET_TM_PLACE_ROBOT_TIME( 0 , FALSE );
				//==============================
			}
			//
			if ( exptime > RunningTime ) RunningTime = exptime;
		}
		if ( RunningTime < 100 ) RunningTime = 100;
		if ( RunningTime < FirstPrgsTime ) RunningTime = FirstPrgsTime;
		if ( RunningTime < LastPrgsTime  ) RunningTime = LastPrgsTime;
	}
	else {
		//===============================================
		for ( i = 0 ; i < rbc ; i ++ ) {
			for ( j = 0 ; j < tbc ; j++ ) {
				if ( rbf[i] <= tbf[j] ) break;
			}
			if ( j == tbc ) {
				if ( tbc >= LOT_DATA_GET_ESTIMATE_BUF_P ) {
					for ( j = 0 ; j < ( LOT_DATA_GET_ESTIMATE_BUF_P - 1 ) ; j++ ) {
						tbf[j] = tbf[j+1];
					}
					tbf[j] = rbf[i];
				}
				else {
					tbf[tbc] = rbf[i];
					tbc++;
				}
			}
		}
		//===============================================
		RunningTime = 0;
		for ( i = 0 ; i < tbc ; i ++ ) RunningTime += tbf[i];
		RunningTime = RunningTime / tbc;
	}
	//----
	if ( osc <= 0 ) {
		if ( *spint <= 0 ) { // 2006.08.29
			if ( FirstPrgsTime <= 0 ) {
				SupplyIntTime = 30;
			}
			else {
				SupplyIntTime = FirstPrgsTime;
			}
		}
		else {
			if ( FirstPrgsTime > *spint ) {
				SupplyIntTime = FirstPrgsTime;
			}
			else {
				SupplyIntTime = *spint;
			}
		}
	}
	else {
		SupplyIntTime = 0;
		for ( i = 0 ; i < osc ; i ++ ) SupplyIntTime += osf[i];
		SupplyIntTime = SupplyIntTime / osc;
	}
	*spint = SupplyIntTime;
	//=================================================================
	if ( notrun <= 0 ) {
		if ( RunningTime > LastPrgsTime ) {
			exptime = RunningTime - LastPrgsTime;
		}
		else {
//			if ( currun > 1 ) { // 2006.09.25
			if ( currun >= 1 ) { // 2006.09.25
				exptime = 30;
			}
			else {
				exptime = 0;
			}
		}
	}
	else {
		if ( SupplyIntTime > LastPrgsTime ) {
			exptime = SupplyIntTime - LastPrgsTime;
		}
		else {
			exptime = LastPrgsTime - SupplyIntTime;
		}
		exptime = exptime + ( ( notrun - 1 ) * SupplyIntTime ) + RunningTime;
	}
	//==================================================================
	time( &t_temp );
	*tdata = t_temp + exptime;
	//==================================================================
	return exptime;
}
//
int LD_MODE = 0;
//
int LD_ALL_DISP = 0;
int LD_PAD_INDEX = 0;
//
//#define MAX_LD_DATA_COUNT	8192

//void *LD_DATA[MAX_LD_DATA_COUNT];
int   LD_MAX_TABLE = 0;
void *LD_TABLE;
//
int   LD_DATA_COUNT = 0;
//
int LD_RUN_CONTROL = 0; // stop , run , pause
int LD_RUN_MODE = 0; // Check , IO , Sim , Check(Until) , IO(Until) , Sim(Until) 
int LD_RUN_INDEX = 0;
int LD_RUN_BREAK = 0;
int LD_RUN_THREAD = 0;
int LD_RUN_UPDATE = 0;
int LD_RUN_UPDATE2 = 0;
int LD_RUN_STEP = 0;
//
BOOL LD_RUN_MSG_TM = FALSE;
BOOL LD_RUN_MSG_PM = FALSE;
BOOL LD_RUN_MSG_BM = FALSE;
BOOL LD_RUN_MSG_FM = FALSE;
BOOL LD_RUN_MSG_AL = FALSE;
BOOL LD_RUN_MSG_IC = FALSE;
BOOL LD_RUN_MSG_BMSIDE = FALSE;
BOOL LD_RUN_MSG_PMPRCS = FALSE;
//
BOOL LD_RUN_IO_MAINTAIN = FALSE;

#define COUNTER_CHAR		sizeof(char)	//1
#define COUNTER_INT			sizeof(int)		//4
#define COUNTER_LONG		sizeof(long)	//4
#define COUNTER_FLOAT		sizeof(float)	//4
#define COUNTER_DOUBLE		sizeof(double)	//8
#define COUNTER_CLOCK		sizeof(long)
#define COUNTER_BOOL		sizeof(char)

// IO Table
#define		xIO_TBL_ADDR( xxx , ind )		*(int *)((char *) xxx + ( COUNTER_INT * (ind) ) )
#define		IO_TBL_ADDR( ind )				xIO_TBL_ADDR( LD_TABLE , ind )

#define		IO_TIME_YEAR_POINTER			0
#define		IO_TIME_MON_POINTER				IO_TIME_YEAR_POINTER+COUNTER_INT
#define		IO_TIME_DAY_POINTER				IO_TIME_MON_POINTER+COUNTER_INT
#define		IO_TIME_HOUR_POINTER			IO_TIME_DAY_POINTER+COUNTER_INT
#define		IO_TIME_MIN_POINTER				IO_TIME_HOUR_POINTER+COUNTER_INT
#define		IO_TIME_SEC_POINTER				IO_TIME_MIN_POINTER+COUNTER_INT
#define		IO_TIME_MSEC_POINTER			IO_TIME_SEC_POINTER+COUNTER_INT
#define		IO_LOT_POINTER					IO_TIME_MSEC_POINTER+COUNTER_INT
#define		IO_LOG_POINTER					IO_LOT_POINTER+COUNTER_INT
#define		IO_LOG2_POINTER					IO_LOG_POINTER+512+1
#define		IO_LOG3_POINTER					IO_LOG2_POINTER+64+1
#define		IO_FTM_POINTER					IO_LOG3_POINTER+64+1
#define		IO_ACT_POINTER					IO_FTM_POINTER+COUNTER_INT
#define		IO_STS_POINTER					IO_ACT_POINTER+COUNTER_INT
#define		IO_CHAMBER_POINTER				IO_STS_POINTER+COUNTER_INT
#define		IO_ARM_POINTER					IO_CHAMBER_POINTER+COUNTER_INT
#define		IO_SLOT1_POINTER				IO_ARM_POINTER+COUNTER_INT
#define		IO_SLOT2_POINTER				IO_SLOT1_POINTER+COUNTER_INT
#define		IO_SWITCH_POINTER				IO_SLOT2_POINTER+COUNTER_INT
#define		IO_DUMMY_POINTER				IO_SWITCH_POINTER+COUNTER_INT
#define		IO_ETC_POINTER					IO_DUMMY_POINTER+COUNTER_INT
#define		IO_RECIPE_POINTER				IO_ETC_POINTER+COUNTER_INT
#define		IO_TOTAL						IO_RECIPE_POINTER+256+1

#define		xIO_TIME_YEAR( xxx )			*(int     *)((char *) xxx + IO_TIME_YEAR_POINTER)
#define		xIO_TIME_MON( xxx )				*(int     *)((char *) xxx + IO_TIME_MON_POINTER)
#define		xIO_TIME_DAY( xxx )				*(int     *)((char *) xxx + IO_TIME_DAY_POINTER)
#define		xIO_TIME_HOUR( xxx )			*(int     *)((char *) xxx + IO_TIME_HOUR_POINTER)
#define		xIO_TIME_MIN( xxx )				*(int     *)((char *) xxx + IO_TIME_MIN_POINTER)
#define		xIO_TIME_SEC( xxx )				*(int     *)((char *) xxx + IO_TIME_SEC_POINTER)
#define		xIO_TIME_MSEC( xxx )			*(int     *)((char *) xxx + IO_TIME_MSEC_POINTER)
#define		xIO_LOT( xxx )					*(int     *)((char *) xxx + IO_LOT_POINTER)
#define		xIO_LOG( xxx )								((char *) xxx + IO_LOG_POINTER)
#define		xIO_LOG2( xxx )								((char *) xxx + IO_LOG2_POINTER)
#define		xIO_LOG3( xxx )								((char *) xxx + IO_LOG3_POINTER)
#define		xIO_FTM( xxx )					*(int     *)((char *) xxx + IO_FTM_POINTER)
#define		xIO_ACT( xxx )					*(int     *)((char *) xxx + IO_ACT_POINTER)
#define		xIO_STS( xxx )					*(int     *)((char *) xxx + IO_STS_POINTER)
#define		xIO_CHAMBER( xxx )				*(int     *)((char *) xxx + IO_CHAMBER_POINTER)
#define		xIO_ARM( xxx )					*(int     *)((char *) xxx + IO_ARM_POINTER)
#define		xIO_SLOT( xxx )					*(int     *)((char *) xxx + IO_SLOT1_POINTER)
#define		xIO_WAFER( xxx )				*(int     *)((char *) xxx + IO_SLOT2_POINTER)
#define		xIO_SWITCH( xxx )				*(int     *)((char *) xxx + IO_SWITCH_POINTER)
#define		xIO_DUMMY( xxx )				*(int     *)((char *) xxx + IO_DUMMY_POINTER)
#define		xIO_ETC( xxx )					*(int     *)((char *) xxx + IO_ETC_POINTER)
#define		xIO_RECIPE( xxx )							((char *) xxx + IO_RECIPE_POINTER)

#define		IO_TIME_YEAR( ind )				xIO_TIME_YEAR( IO_TBL_ADDR( ind ) )	
#define		IO_TIME_MON( ind )				xIO_TIME_MON( IO_TBL_ADDR( ind ) )	
#define		IO_TIME_DAY( ind )				xIO_TIME_DAY( IO_TBL_ADDR( ind ) )	
#define		IO_TIME_HOUR( ind )				xIO_TIME_HOUR( IO_TBL_ADDR( ind ) )	
#define		IO_TIME_MIN( ind )				xIO_TIME_MIN( IO_TBL_ADDR( ind ) )	
#define		IO_TIME_SEC( ind )				xIO_TIME_SEC( IO_TBL_ADDR( ind ) )	
#define		IO_TIME_MSEC( ind )				xIO_TIME_MSEC( IO_TBL_ADDR( ind ) )	
#define		IO_LOT( ind )					xIO_LOT( IO_TBL_ADDR( ind ) )
#define		IO_LOG( ind )					xIO_LOG( IO_TBL_ADDR( ind ) )
#define		IO_LOG2( ind )					xIO_LOG2( IO_TBL_ADDR( ind ) )
#define		IO_LOG3( ind )					xIO_LOG3( IO_TBL_ADDR( ind ) )
#define		IO_FTM( ind )					xIO_FTM( IO_TBL_ADDR( ind ) )
#define		IO_ACT( ind )					xIO_ACT( IO_TBL_ADDR( ind ) )
#define		IO_STS( ind )					xIO_STS( IO_TBL_ADDR( ind ) )
#define		IO_CHAMBER( ind )				xIO_CHAMBER( IO_TBL_ADDR( ind ) )
#define		IO_ARM( ind )					xIO_ARM( IO_TBL_ADDR( ind ) )
#define		IO_SLOT( ind )					xIO_SLOT( IO_TBL_ADDR( ind ) )
#define		IO_WAFER( ind )					xIO_WAFER( IO_TBL_ADDR( ind ) )
#define		IO_SWITCH( ind )				xIO_SWITCH( IO_TBL_ADDR( ind ) )
#define		IO_DUMMY( ind )					xIO_DUMMY( IO_TBL_ADDR( ind ) )
#define		IO_ETC( ind )					xIO_ETC( IO_TBL_ADDR( ind ) )
#define		IO_RECIPE( ind )				xIO_RECIPE( IO_TBL_ADDR( ind ) )

int GET_RUN_LD_CONTROL( int mode ) { // 2014.11.13
	//
	if ( LD_RUN_CONTROL <= 0 ) return 0;
	//
	switch( mode ) {
	case 1 : // TM-inf
		if ( LD_RUN_MSG_TM ) return 0;
		break;
	case 2 : // PM-inf
		if ( LD_RUN_MSG_PM ) return 0;
		break;
	case 3 : // BM-inf
		if ( LD_RUN_MSG_BM ) return 0;
		break;
	case 4 : // FM-inf
		if ( LD_RUN_MSG_FM ) return 0;
		break;
	case 5 : // AL
		if ( LD_RUN_MSG_AL ) return 0;
		break;
	case 6 : // IC
		if ( LD_RUN_MSG_IC ) return 0;
		break;
	case 7 : // BM-SIDE
		if ( LD_RUN_MSG_BMSIDE ) return 0;
		break;
	case 8 : // PM-PROCESS
		if ( LD_RUN_MSG_PMPRCS ) return 0;
		break;
	}
	//
	if      ( ( LD_RUN_MODE == 0 ) || ( LD_RUN_MODE == 3 ) ) return 1;
	else if ( ( LD_RUN_MODE == 1 ) || ( LD_RUN_MODE == 4 ) ) return 2;
	//
	return 3;
	//	
}
//
void GET_TIME_DATA( char *data , int *year , int *month , int *day , int *time , int *min , int *sec , int *msec ) {
	char data1[64];
	char data2[64];
	char data3[64];

	STR_SEPERATE_CHAR( data  , '/' , data1 , data2 , 63 );	*year = atoi( data1 );
	STR_SEPERATE_CHAR( data2 , '/' , data1 , data3 , 63 );	*month = atoi( data1 );
	STR_SEPERATE_CHAR( data3 , ' ' , data1 , data2 , 63 );	*day = atoi( data1 );
	STR_SEPERATE_CHAR( data2 , ':' , data1 , data3 , 63 );	*time = atoi( data1 );
	STR_SEPERATE_CHAR( data3 , ':' , data1 , data2 , 63 );	*min = atoi( data1 );
	STR_SEPERATE_CHAR( data2 , ':' , data1 , data3 , 63 );	*sec = atoi( data1 );
	STR_SEPERATE_CHAR( data3 , '.' , data1 , data2 , 63 );	*msec = atoi( data1 );
}

int Get_Module_ID_From_String( int mode , char *mdlstr , int badch );

void Get_Wafer_ID_From_String( int mode , int ch , char *str , int *arm , int *slot , int *wafer , int *sw , int *dummy , char *recipe ) {
	char data1[64];
	char data2[64];
	char data3[64];
	char data4[64];
	int s1 , s2 , w1 , w2;
	//
	*dummy = 0;
	//
	if ( mode == 0 ) { // FM
		//
		if      ( str[0] == 'A' ) {
			*arm = 1;
			*sw = 0;
		}
		else if ( str[0] == 'B' ) {
			*arm = 2;
			*sw = 0;
		}
		else {
			//
			STR_SEPERATE_CHAR( str   , ',' , data1 , data2 , 63 );
			//
			if ( ( ch >= CM1 ) && ( ch < PM1 ) ) {
				//
				STR_SEPERATE_CHAR( data1 , ':' , data3 , data4 , 63 );	s1  = atoi( data3 );	w1 = s1;
				//
				STR_SEPERATE_CHAR( data2 , ':' , data3 , data4 , 63 );	s2  = atoi( data3 );	w2 = s2;
			}
			else {
				//
				STR_SEPERATE_CHAR( data1 , ':' , data3 , data4 , 63 );	s1  = atoi( data3 );	w1 = atoi( data4 );	if ( w1 <= 0 ) w1 = s1;
				//
				STR_SEPERATE_CHAR( data2 , ':' , data3 , data4 , 63 );	s2  = atoi( data3 );
				//
				if ( data4[0] == 'D' ) {
					*dummy = 1;
					w2 = atoi( data4 + 1 );	if ( w2 <= 0 ) w2 = s2;
				}
				else {
					w2 = atoi( data4 );	if ( w2 <= 0 ) w2 = s2;
				}
				//
			}
			//
			if      ( ( s1 > 0 ) && ( s2 > 0 ) ) {
				*arm = 0;
				*sw = 0;
				*slot = ( s2 * 100 ) + s1;
				*wafer = ( w2 * 100 ) + w1;
			}
			else if ( s1 > 0 ) {
				*arm = 1;
				*sw = 0;
				*slot = s1;
				*wafer = w1;
			}
			else if ( s2 > 0 ) {
				*arm = 2;
				*sw = 0;
				*slot = s2;
				*wafer = w2;
			}
		}
		//
	}
	else if ( mode == 100 ) { // PM
		STR_SEPERATE_CHAR( str   , ':' , data3 , data2 , 63 );	*slot = atoi( data3 );	*wafer = atoi( data3 );
		STR_SEPERATE_CHAR( data2 , ':' , data3 , data4 , 63 );	strcpy( recipe , data3 );
	}
	else if ( mode > 0 ) { // tm
		//
		if      ( str[0] == 'A' ) {
			*arm = 1;
			*sw = 0;
		}
		else if ( str[0] == 'B' ) {
			*arm = 2;
			*sw = 0;
		}
		else {
			STR_SEPERATE_CHAR( str   , ':' , data1 , data3 , 63 );	*slot  = atoi( data1 );
			STR_SEPERATE_CHAR( data3 , ':' , data1 , data2 , 63 );
			//
			if ( data1[0] == 'D' ) {
				*dummy = 1;
				*wafer = atoi( data1 + 1 );
			}
			else {
				*wafer = atoi( data1 );
			}
			//
			STR_SEPERATE_CHAR( data2 , ':' , data1 , data3 , 63 );
			//
			if      ( STRCMP_L( data1 , "A" ) ) {
				*arm = 1;
				*sw = 0;
			}
			else if ( STRCMP_L( data1 , "B" ) ) {
				*arm = 2;
				*sw = 0;
			}
			else if ( STRCMP_L( data1 , "AS" ) ) {
				*arm = 1;
				*sw = 1;
			}
			else if ( STRCMP_L( data1 , "BS" ) ) {
				*arm = 2;
				*sw = 1;
			}
		}
	}
}

int LD_InsPos_Data( int TIME_YEAR , int TIME_MON , int TIME_DAY , int TIME_HOUR , int TIME_MIN , int TIME_SEC , int TIME_MSEC , int FTM , int ACT , int STS ) {
	int i;
	//
	for ( i = 0 ; i < LD_DATA_COUNT ; i++ ) {
		//=========================================================================
		if ( TIME_YEAR < IO_TIME_YEAR(i) ) {
			break;
		}
		else if ( TIME_YEAR > IO_TIME_YEAR(i) ) {
			continue;
		}
		//
		if ( TIME_MON < IO_TIME_MON(i) ) {
			break;
		}
		else if ( TIME_MON > IO_TIME_MON(i) ) {
			continue;
		}
		//
		if ( TIME_DAY < IO_TIME_DAY(i) ) {
			break;
		}
		else if ( TIME_DAY > IO_TIME_DAY(i) ) {
			continue;
		}
		//
		if ( TIME_HOUR < IO_TIME_HOUR(i) ) {
			break;
		}
		else if ( TIME_HOUR > IO_TIME_HOUR(i) ) {
			continue;
		}
		//
		if ( TIME_MIN < IO_TIME_MIN(i) ) {
			break;
		}
		else if ( TIME_MIN > IO_TIME_MIN(i) ) {
			continue;
		}
		//
		if ( TIME_SEC < IO_TIME_SEC(i) ) {
			break;
		}
		else if ( TIME_SEC > IO_TIME_SEC(i) ) {
			continue;
		}
		//
		if ( TIME_MSEC < IO_TIME_MSEC(i) ) {
			break;
		}
		else if ( TIME_MSEC > IO_TIME_MSEC(i) ) {
			continue;
		}
		//=========================================================================
		if ( ( IO_FTM(i) < 0 ) || ( IO_FTM(i) == 99 ) ) continue;
		//
		if ( STS == 9 ) continue;
		break;
		//
		//=========================================================================
	}
	return i;
}

int LD_Set_Data( HWND hdlg , char *filename ) {
	FILE *fpt;
	int i , j , f , r;

	if ( ( fpt = fopen( filename , "w" ) ) == NULL ) return 1;
	//
	for ( j = 0 ; j < 9 ; j++ ) {
		//
		if      ( j == 0 ) r = -2;
		else if ( j == 1 ) r = 0;
		else if ( j == 2 ) r = 1;
		else if ( j == 3 ) r = 2;
		else if ( j == 4 ) r = 3;
		else if ( j == 5 ) r = 4;
		else if ( j == 6 ) r = 5;
		else if ( j == 7 ) r = 100;
		else if ( j == 8 ) r = -1;
		else break;
		//
		f = ( j == 0 ) ? 1 : 0;
		//
		for ( i = 0 ; i < LD_DATA_COUNT ; i++ ) {
			//
			if ( ( r != -2 ) && ( r != IO_FTM(i) ) ) continue;
			//
			if ( f == 0 ) {
				fprintf( fpt , "\n" );
				fprintf( fpt , "\n" );
				fprintf( fpt , "===========================================================================================================\n" );
				fprintf( fpt , "===========================================================================================================\n" );
				fprintf( fpt , "\n" );
				fprintf( fpt , "\n" );
			}
			//
			f = 1;
			//
			fprintf( fpt , "%04d" , i + 1 );
			//
			fprintf( fpt , "\t%04d/%02d/%02d %02d:%02d:%02d" , IO_TIME_YEAR(i) , IO_TIME_MON(i) , IO_TIME_DAY(i) , IO_TIME_HOUR(i) , IO_TIME_MIN(i) , IO_TIME_SEC(i) );
			//
			if ( IO_TIME_MSEC(i) > 0 ) fprintf( fpt , ".%03d" , IO_TIME_MSEC(i) );
			//
			if ( IO_FTM(i) == 99 ) {
				fprintf( fpt , "\tALARM" );
			}
			else {
				fprintf( fpt , "\tLOT%d" , ( IO_LOT(i) <= 0 ) ? 0 : IO_LOT(i) );
			}
			//
			fprintf( fpt , "\t%s\t%s\t%s" , IO_LOG(i) , IO_LOG2(i) , IO_LOG3(i) );
			//
			fprintf( fpt , "\n" );
			//
		}
	}
	fclose( fpt );
	return 0;
}

int LD_DATA_TABLE_CHECK( void **datatable , int datacount , int *maxcount , int incsize ) {
	void *lddata;
	void *lddata2;
	//
	if ( datacount < *maxcount ) return 0;
	//
	lddata = malloc( sizeof(int) * ( *maxcount + incsize ) );
	//
	if ( lddata == NULL ) return -1;
	//
	lddata2 = NULL;
	//
	if ( *maxcount > 0 ) {
		//
		memcpy( lddata , *datatable , ( sizeof(int) * *maxcount ) );
		//
		lddata2 = *datatable;
		//
	}
	//
	*datatable = lddata;
	//
	if ( lddata2 != NULL ) free( lddata2 );
	//
	*maxcount = *maxcount + incsize;
	//
	return 1;
}

int LD_DATA_APPEND( void *datatable , void *data , int *datacount , int maxcount , int insertindex ) {
	int i;
	//
	if ( *datacount >= maxcount ) return 1;
	if ( insertindex > *datacount ) return 2;
	if ( insertindex < 0 ) return 3;
	//
	for ( i = (*datacount-1) ; i >= insertindex ; i-- ) {
		xIO_TBL_ADDR( datatable , i+1 ) = xIO_TBL_ADDR( datatable , i );
	}
	//
	xIO_TBL_ADDR( datatable , insertindex ) = (int) data;
	//
	(*datacount)++;
	//
	return 0;
}

void LD_DATA_CLEAR( void *datatable , int *datacount ) {
	int i;
	//
	for ( i = 0 ; i < *datacount ; i++ ) {
		free( (void *) xIO_TBL_ADDR( datatable , i ) );
	}
	//
	*datacount = 0;
	//
}


int LD_Get_Data( HWND hdlg , char *filename ) {

	int z , offset;
	BOOL bres;
	FILE *fpt;
	//
	void *lddata;
//	void *lddata2;

	char Buffer[1024];
	//
	char TempBuffer[64];
	char Str_Action_Message[64];
	char Str_Module_Message[64];
	char Str_ArmSlot_Message[64];

	if ( ( fpt = fopen( filename , "r" ) ) == NULL ) return 1;
	//
	do {
		bres = Read_Line_From_File4( fpt , Buffer , 1023 );
		//
		if ( *Buffer != 0 ) {
			//------------------------------------------
			if ( LD_DATA_TABLE_CHECK( &LD_TABLE , LD_DATA_COUNT , &LD_MAX_TABLE , 256 ) == -1 ) {
				//
				printf( "Allocate memory for lddata Table Error\n" );
				//
				fclose( fpt );
				//
				return 2;
			}
			//------------------------------------------
			lddata = malloc( IO_TOTAL );
			//
			if ( lddata == NULL ) {
				//
				printf( "Allocate memory for lddata data Error\n" );
				//
				fclose( fpt );
				//
				return 3;
			}
			//------------------------------------------
			//
			xIO_FTM( lddata ) = -1; // FM(0)/TM(1-..)/ALARM(99)/PM(100)
			xIO_ACT( lddata ) = -1; // pick(0)/place(1)/swap(2)/process(11)/processpost(12)/processpre(13)/FMSIDE(21)/TMSIDE(22)/AL(31)/IC(32)
			xIO_STS( lddata ) = -1; // S(0)/A(1)/E(2)/R(9)
			xIO_LOT( lddata ) = 0;
			//
			xIO_CHAMBER( lddata ) = 0;
			xIO_ARM( lddata ) = -1;
			xIO_SLOT( lddata ) = 0;
			xIO_WAFER( lddata ) = 0;
			xIO_SWITCH( lddata ) = 0;
			//
			xIO_DUMMY( lddata ) = 0;
			xIO_ETC( lddata ) = 0;
			//
			strcpy( xIO_RECIPE( lddata ) , "" );
			//
			//------------------------------------------
			//
			z = 0;
			//
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z , 63 );	// Time
			//
				GET_TIME_DATA( TempBuffer ,
					&(xIO_TIME_YEAR(lddata)) , 
					&(xIO_TIME_MON(lddata)) , 
					&(xIO_TIME_DAY(lddata)) , 
					&(xIO_TIME_HOUR(lddata)) , 
					&(xIO_TIME_MIN(lddata)) , 
					&(xIO_TIME_SEC(lddata)) , 
					&(xIO_TIME_MSEC(lddata)) );
			//
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , TempBuffer , z , 63 );	// Lot
			//
				if ( STRNCMP_L( TempBuffer , "LOT" , 3 ) ) {
					xIO_LOT( lddata ) = atoi( TempBuffer + 3 );
				}
				else {
					if ( TempBuffer[0] == 'A' ) {
						xIO_FTM( lddata ) = 99;
					}
				}
			//
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , xIO_LOG( lddata ) , z , 512 );	// Message
			//
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , xIO_LOG2( lddata ) , z , 63 );	// Desp
			//
				STR_SEPERATE_CHAR( xIO_LOG2( lddata ) , ' ' , Str_Action_Message , TempBuffer , 63 );
				STR_SEPERATE_CHAR( TempBuffer , ':' , Str_Module_Message , Str_ArmSlot_Message , 63 );
			//
			z = STR_SEPERATE_CHAR_WITH_POINTER( Buffer , 9 , xIO_LOG3( lddata ) , z , 63 );	// Slot
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			offset = -1;
			//
			if      ( STRCMP_L( Str_Action_Message , "WHFMSTART" ) ) {
				offset = 1;
			}
			else if ( STRCMP_L( Str_Action_Message , "WHFMSUCCESS" ) ) {
				offset = 2;
			}
			else if ( STRCMP_L( Str_Action_Message , "WHFMFAIL" ) ) {
				offset = 3;
			}
			else if ( STRCMP_L( Str_Action_Message , "WHFMFWPRODUCE" ) ) {
				offset = 4;
			}
			else if ( STRCMP_L( Str_Action_Message , "WHFMLWPRODUCE" ) ) {
				offset = 5;
			}
			//
			if ( offset == -1 ) {
				//
				if      ( STRNCMP_L( Str_Action_Message , "WHFM" , 4 ) ) {
					xIO_FTM( lddata ) = 0;
					offset = 4;
				}
				else if ( STRNCMP_L( Str_Action_Message , "WHTM" , 4 ) ) {
					if      ( ( Str_Action_Message[4] >= 'A' ) && ( Str_Action_Message[4] <= 'Z' ) ) {
						xIO_FTM( lddata ) = 1;
						offset = 4;
					}
					else if ( ( Str_Action_Message[5] >= 'A' ) && ( Str_Action_Message[5] <= 'Z' ) ) {
						xIO_FTM( lddata ) = Str_Action_Message[4] - '0';
						offset = 5;
					}
					else if ( ( Str_Action_Message[6] >= 'A' ) && ( Str_Action_Message[6] <= 'Z' ) ) {
						xIO_FTM( lddata ) = ( ( Str_Action_Message[4] - '0' ) * 10 ) + ( Str_Action_Message[5] - '0' );
						offset = 6;
					}
				}
				else if ( STRNCMP_L( Str_Action_Message , "PROCESS_" , 8 ) ) {
					xIO_FTM( lddata ) = 100;
					offset = 8;
				}
				//
				if ( offset != -1 ) {
					//===================================================================================================
					xIO_CHAMBER( lddata ) = Get_Module_ID_From_String( 0 , Str_Module_Message , -1 );
					//===================================================================================================
					if      ( STRNCMP_L( Str_Action_Message + offset , "PICK" , 4 ) ) {
						//
						xIO_ACT( lddata ) = 0;
						//
						offset = offset + 4;
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "PLACE" , 5 ) ) {
						//
						xIO_ACT( lddata ) = 1;
						//
						offset = offset + 5;
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "SWAP" , 4 ) ) {
						//
						xIO_ACT( lddata ) = 2;
						//
						offset = offset + 4;
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "MOVE" , 4 ) ) {
						//
						xIO_ACT( lddata ) = 3;
						//
						offset = offset + 4;
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "POST_" , 5 ) ) {
						//
						xIO_ACT( lddata ) = 12;
						//
						offset = offset + 5;
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "PRE_" , 4 ) ) {
						//
						xIO_ACT( lddata ) = 13;
						//
						offset = offset + 4;
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "GOFMSIDE_END" ) ) {
						//
						xIO_ACT( lddata ) = 21;
						xIO_STS( lddata ) = 0;
						//
						xIO_ETC( lddata ) = atoi( Str_ArmSlot_Message );
						//
						offset = offset + 12;
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "GOTMSIDE_END" ) ) {
						//
						xIO_ACT( lddata ) = 22;
						xIO_STS( lddata ) = 0;
						//
						xIO_ETC( lddata ) = atoi( Str_ArmSlot_Message );
						//
						offset = offset + 12;
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "GOFMSIDE" ) ) {
						//
						xIO_ACT( lddata ) = 21;
						xIO_STS( lddata ) = 9;
						//
						xIO_ETC( lddata ) = atoi( Str_ArmSlot_Message );
						//
						offset = offset + 8;
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "GOTMSIDE" ) ) {
						//
						xIO_ACT( lddata ) = 22;
						xIO_STS( lddata ) = 9;
						//
						xIO_ETC( lddata ) = atoi( Str_ArmSlot_Message );
						//
						offset = offset + 8;
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "AL" , 2 ) ) {
						//
						xIO_ACT( lddata ) = 31;
						xIO_CHAMBER( lddata ) = F_AL;
						//
						offset = offset + 2;
						//
						if ( STRCMP_L( Str_Action_Message + offset , "SUCCESS" ) ) {
							xIO_STS( lddata ) = 0;
							offset = offset + 7;
						}
						else if ( STRCMP_L( Str_Action_Message + offset , "FAIL" ) ) {
							xIO_STS( lddata ) = 1;
							offset = offset + 4;
						}
						else if ( STRCMP_L( Str_Action_Message + offset , "RUN" ) ) {
							xIO_STS( lddata ) = 9;
							offset = offset + 3;
						}
						//
						if      ( STRCMP_L( Str_Module_Message , "A" ) ) {
							xIO_ARM( lddata ) = 1;
						}
						else if ( STRCMP_L( Str_Module_Message , "B" ) ) {
							xIO_ARM( lddata ) = 2;
						}
						//
						xIO_SLOT( lddata ) = 1;
						xIO_WAFER( lddata ) = atoi( Str_ArmSlot_Message );
						//
					}
					else if ( STRNCMP_L( Str_Action_Message + offset , "IC" , 2 ) ) {
						//
						xIO_ACT( lddata ) = 32;
						xIO_CHAMBER( lddata ) = F_IC;
						//
						offset = offset + 2;
						//
						if ( STRCMP_L( Str_Action_Message + offset , "SUCCESS" ) ) {
							xIO_STS( lddata ) = 0;
							offset = offset + 7;
						}
						else if ( STRCMP_L( Str_Action_Message + offset , "FAIL" ) ) {
							xIO_STS( lddata ) = 1;
							offset = offset + 4;
						}
						else if ( STRCMP_L( Str_Action_Message + offset , "RUN" ) ) {
							xIO_STS( lddata ) = 9;
							offset = offset + 3;
						}
						//
						if      ( STRCMP_L( Str_Module_Message , "A" ) ) {
							xIO_ARM( lddata ) = 1;
						}
						else if ( STRCMP_L( Str_Module_Message , "B" ) ) {
							xIO_ARM( lddata ) = 2;
						}
						//
						xIO_SLOT( lddata ) = 1;
						xIO_WAFER( lddata ) = atoi( Str_ArmSlot_Message );
						//
					}
					else {
						if ( xIO_FTM( lddata ) == 100 ) xIO_ACT( lddata ) = 11;
					}
					//===================================================================================================
					if      ( STRCMP_L( Str_Action_Message + offset , "START" ) ) {
						//
						xIO_STS( lddata ) = 9;
						//
						Get_Wafer_ID_From_String( xIO_FTM( lddata ) , xIO_CHAMBER( lddata ) , Str_ArmSlot_Message , &(xIO_ARM( lddata )) , &(xIO_SLOT( lddata )) , &(xIO_WAFER( lddata )) , &(xIO_SWITCH( lddata )) , &(xIO_DUMMY( lddata )) , xIO_RECIPE( lddata ) );
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "END" ) ) {
						//
						xIO_STS( lddata ) = 0;
						//
						Get_Wafer_ID_From_String( xIO_FTM( lddata ) , xIO_CHAMBER( lddata ) , Str_ArmSlot_Message , &(xIO_ARM( lddata )) , &(xIO_SLOT( lddata )) , &(xIO_WAFER( lddata )) , &(xIO_SWITCH( lddata )) , &(xIO_DUMMY( lddata )) , xIO_RECIPE( lddata ) );
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "SUCCESS" ) ) {
						//
						xIO_STS( lddata ) = 0;
						//
						Get_Wafer_ID_From_String( xIO_FTM( lddata ) , xIO_CHAMBER( lddata ) , Str_ArmSlot_Message , &(xIO_ARM( lddata )) , &(xIO_SLOT( lddata )) , &(xIO_WAFER( lddata )) , &(xIO_SWITCH( lddata )) , &(xIO_DUMMY( lddata )) , xIO_RECIPE( lddata ) );
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "FAIL" ) ) {
						//
						xIO_STS( lddata ) = 1;
						//
						Get_Wafer_ID_From_String( xIO_FTM( lddata ) , xIO_CHAMBER( lddata ) , Str_ArmSlot_Message , &(xIO_ARM( lddata )) , &(xIO_SLOT( lddata )) , &(xIO_WAFER( lddata )) , &(xIO_SWITCH( lddata )) , &(xIO_DUMMY( lddata )) , xIO_RECIPE( lddata ) );
						//
					}
					else if ( STRCMP_L( Str_Action_Message + offset , "REJECT" ) ) {
						//
						xIO_STS( lddata ) = 2;
						//
						Get_Wafer_ID_From_String( xIO_FTM( lddata ) , xIO_CHAMBER( lddata ) , Str_ArmSlot_Message , &(xIO_ARM( lddata )) , &(xIO_SLOT( lddata )) , &(xIO_WAFER( lddata )) , &(xIO_SWITCH( lddata )) , &(xIO_DUMMY( lddata )) , xIO_RECIPE( lddata ) );
						//
					}
					//===================================================================================================
				}
			}
			//------------------------------------------
			//
			offset = LD_InsPos_Data( xIO_TIME_YEAR(lddata) , xIO_TIME_MON(lddata) , xIO_TIME_DAY(lddata) , xIO_TIME_HOUR(lddata) , xIO_TIME_MIN(lddata) , xIO_TIME_SEC(lddata) , xIO_TIME_MSEC(lddata) , xIO_FTM( lddata ) , xIO_ACT( lddata ) , xIO_STS( lddata ) );
			//
			//------------------------------------------
			//
			LD_DATA_APPEND( LD_TABLE , lddata , &LD_DATA_COUNT , LD_MAX_TABLE , offset );
			//
			//------------------------------------------

		}
	}
	while ( bres );
	fclose( fpt );
	return 0;
}

void LD_Show_Data( HWND hdlg ) {
	char *szString2[]  = { "ID" , "TIME" , "LOT" , "Log" , "F/TM" , "Arm" , "ACT" , "STS" , "CH" , "SLOT" , "WID"  , "SWITCH" , "DUMMY" , "ETC" , "FM" , "TM" , "TM2" , "TM3" };
	int    szSize2[18] = {  40  ,    140 ,    40 ,   400 ,   40 ,    40 ,      70 ,    40 ,   40 ,     50 ,     50 ,       40 ,   40 ,   40 ,   40 ,   40 ,    40 ,   40  };
	//
	ListView_DeleteAllItems( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) );
	while ( TRUE ) if ( !ListView_DeleteColumn( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 0 ) ) break;
	//
	KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 18 , szString2 , szSize2 );
	ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//
	KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LD_DATA_COUNT );
	//
}

void LD_Button_Data( HWND hdlg ) {
	//
	char Buffer[32];
	//
	LD_RUN_UPDATE = 0;
	//
	if ( LD_RUN_CONTROL == 0 ) {
		//
		KWIN_Item_String_Display( hdlg , IDRETRY , "RUN" );
		//
		if      ( LD_RUN_MODE == 0 ) {
			KWIN_Item_String_Display( hdlg , IDNO , "DIRECT" );
		}
		else if ( LD_RUN_MODE == 1 ) {
			KWIN_Item_String_Display( hdlg , IDNO , "FAST" );
		}
		else if ( LD_RUN_MODE == 2 ) {
			KWIN_Item_String_Display( hdlg , IDNO , "NORMAL" );
		}
		else if ( LD_RUN_MODE == 3 ) {
			KWIN_Item_String_Display( hdlg , IDNO , "DIRECT(U)" );
		}
		else if ( LD_RUN_MODE == 4 ) {
			KWIN_Item_String_Display( hdlg , IDNO , "FAST(U)" );
		}
		else if ( LD_RUN_MODE == 5 ) {
			KWIN_Item_String_Display( hdlg , IDNO , "NORMAL(U)" );
		}
		//
		KWIN_Item_String_Display( hdlg , IDHELP , "STEP" );
		KWIN_Item_Disable( hdlg , IDHELP );
		//
	}
	else if ( LD_RUN_CONTROL == 1 ) { // run
		KWIN_Item_String_Display( hdlg , IDRETRY , "STOP" );
		KWIN_Item_String_Display( hdlg , IDNO , "PAUSE" );
		//
		KWIN_Item_String_Display( hdlg , IDHELP , "STEP" );
		KWIN_Item_Disable( hdlg , IDHELP );
		//
	}
	else if ( LD_RUN_CONTROL == 2 ) { // pause
		KWIN_Item_String_Display( hdlg , IDRETRY , "STOP" );
		KWIN_Item_String_Display( hdlg , IDNO , "RESUME" );
		//
		KWIN_Item_String_Display( hdlg , IDHELP , "STEP" );
		KWIN_Item_Enable( hdlg , IDHELP );
		//
	}
	//
	if ( LD_RUN_BREAK <= 0 ) {
		sprintf( Buffer , "BREAK=N/A" );
	}
	else {
		sprintf( Buffer , "BREAK=%d" , LD_RUN_BREAK );
	}
	//
	KWIN_Item_String_Display( hdlg , IDC_READY_USE_B1 , Buffer );
	//
	if ( LD_RUN_MSG_TM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T1 , "O" );
	else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T1 , "" );
	if ( LD_RUN_MSG_PM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T2 , "O" );
	else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T2 , "" );
	if ( LD_RUN_MSG_BM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T3 , "O" );
	else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T3 , "" );
	if ( LD_RUN_MSG_FM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T4 , "O" );
	else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T4 , "" );
	if ( LD_RUN_MSG_AL )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T5 , "O" );
	else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T5 , "" );
	if ( LD_RUN_MSG_IC )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T6 , "O" );
	else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T6 , "" );
	if ( LD_RUN_MSG_BMSIDE )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T7 , "O" );
	else						KWIN_Item_String_Display( hdlg , IDC_READY_USE_T7 , "" );
	if ( LD_RUN_MSG_PMPRCS )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T8 , "O" );
	else						KWIN_Item_String_Display( hdlg , IDC_READY_USE_T8 , "" );
	//
	if ( LD_RUN_IO_MAINTAIN )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T9 , "MAINTAIN" );
	else						KWIN_Item_String_Display( hdlg , IDC_READY_USE_T9 , "RESET" );
	//
}

void LD_PMWafer_Data( int pmc , int data ) {
	int i;
	//
	for ( i = 0 ; i < (MAX_PM_SLOT_DEPTH-1) ; i++ ) {
		_i_SCH_IO_SET_MODULE_RESULT( pmc , i+1 , data );
	}
	//
}

int Maint_Intf_Spawn( BOOL evt , char *RunStr , char *ElseStr );

BOOL LD_Action_Data( int index ) {
	char RunStr[64];
	char ElseStr[64];
	int lc;
	//
	if ( IO_FTM( index ) == 0 ) { // fm
		//
		lc = MAX_TM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH;
		//
		if      ( IO_ACT( index ) == 0 ) { // pick
			//
			if ( ( IO_STS( index ) == 9 ) && ( IO_CHAMBER( index ) >= CM1 ) && ( IO_CHAMBER( index ) < PM1 ) ) {
				//
				if ( ( IO_SLOT(index) % 100 ) > 0 ) _i_SCH_IO_SET_MODULE_STATUS( IO_CHAMBER( index ) , IO_SLOT(index) % 100 , CWM_PRESENT );
				if ( ( IO_SLOT(index) / 100 ) > 0 ) _i_SCH_IO_SET_MODULE_STATUS( IO_CHAMBER( index ) , IO_SLOT(index) / 100 , CWM_PRESENT );
				//
			}
			//
			if ( IO_CHAMBER( index ) == F_AL ) IO_SLOT(index) = 1; // 2017.09.09
			if ( IO_CHAMBER( index ) == AL ) IO_SLOT(index) = 1; // 2017.09.09
			//
			sprintf( RunStr  , "MAINT_INTERFACEF" );
			//
			switch( IO_ARM(index) ) {
			case 1 :
				sprintf( ElseStr , "PICK_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_SLOT(index) , 0 );
				break;
			case 2 :
				sprintf( ElseStr , "PICK_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , 0 , IO_SLOT(index) );
				break;
			default :
				sprintf( ElseStr , "PICK_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_SLOT(index) % 100 , IO_SLOT(index) / 100 );
				break;
			}
			//
		}
		else if ( IO_ACT( index ) == 1 ) { // place
			//
			if ( IO_CHAMBER( index ) == F_AL ) IO_SLOT(index) = 1; // 2017.09.09
			if ( IO_CHAMBER( index ) == AL ) IO_SLOT(index) = 1; // 2017.09.09
			//
			sprintf( RunStr  , "MAINT_INTERFACEF" );
			//
			switch( IO_ARM(index) ) {
			case 1 :
				sprintf( ElseStr , "PLACE_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_SLOT(index) , 0 );
				break;
			case 2 :
				sprintf( ElseStr , "PLACE_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , 0 , IO_SLOT(index) );
				break;
			default :
				sprintf( ElseStr , "PLACE_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_SLOT(index) % 100 , IO_SLOT(index) / 100 );
				break;
			}
			//
		}
		else if ( IO_ACT( index ) == 2 ) { // swap
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		else if ( IO_ACT( index ) == 3 ) { // move
			sprintf( RunStr  , "MAINT_INTERFACEF" );
			//
			switch( IO_ARM(index) ) {
			case 1 :
				sprintf( ElseStr , "ROTATE_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_SLOT(index) , 0 );
				break;
			case 2 :
				sprintf( ElseStr , "ROTATE_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , 0 , IO_SLOT(index) );
				break;
			default :
				sprintf( ElseStr , "ROTATE_FM|0|%s|%d|%d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_SLOT(index) % 100 , IO_SLOT(index) / 100 );
				break;
			}
			//
		}
		else if ( ( IO_ACT( index ) == 11 ) || ( IO_ACT( index ) == 12 ) || ( IO_ACT( index ) == 13 ) ) { // Process/Post/Pre
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		else if ( IO_ACT( index ) == 21 ) { // fmside
			lc = IO_CHAMBER( index ) - BM1 + MAX_TM_CHAMBER_DEPTH;
			sprintf( RunStr  , "MAINT_INTERFACE%c" , IO_CHAMBER( index ) - BM1 + 'A' );
			sprintf( ElseStr , "WAITING_FOR_FM|0|%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) );
		}
		else if ( IO_ACT( index ) == 22 ) { // tmside
			lc = IO_CHAMBER( index ) - BM1 + MAX_TM_CHAMBER_DEPTH;
			sprintf( RunStr  , "MAINT_INTERFACE%c" , IO_CHAMBER( index ) - BM1 + 'A' );
			sprintf( ElseStr , "WAITING_FOR_TM|0|%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) );
		}
		else if ( IO_ACT( index ) == 31 ) { // al
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		else if ( IO_ACT( index ) == 32 ) { // ic
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		//
		if ( IO_STS( index ) == 9 ) {
			//==============================================================================================================================================================================
			printf( "[%d] %s [%s %s]\n" , index + 1 , IO_LOG( index ) , RunStr , ElseStr ); // 2014.09.04
			//==============================================================================================================================================================================
			Maint_Intf_Spawn( FALSE , RunStr , ElseStr );
		}
		else {
			//==============================================================================================================================================================================
			if ( !Transfer_Maint_Inf_Code_End( lc ) ) return FALSE;
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
		}
		//
	}
	else if ( ( IO_FTM( index ) > 0 ) && ( IO_FTM( index ) < 12 ) ) { // tm
		//
		lc = IO_FTM( index ) - 1;
		//
		if      ( IO_ACT( index ) == 0 ) { // pick
			//
			if ( ( IO_STS( index ) == 9 ) && ( IO_CHAMBER( index ) >= CM1 ) && ( IO_CHAMBER( index ) < PM1 ) ) _i_SCH_IO_SET_MODULE_STATUS( IO_CHAMBER( index ) , IO_SLOT(index) , CWM_PRESENT );
			//
			sprintf( RunStr  , "MAINT_INTERFACE%d" , IO_FTM( index ) );
			sprintf( ElseStr , "PICK_TM%d|0|%s|%c|%d|%d" , IO_FTM( index ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_ARM(index) - 1 + 'A' , IO_WAFER(index) , IO_SLOT(index) );
		}
		else if ( IO_ACT( index ) == 1 ) { // place
			sprintf( RunStr  , "MAINT_INTERFACE%d" , IO_FTM( index ) );
			sprintf( ElseStr , "PLACE_TM%d|0|%s|%c|%d|%d" , IO_FTM( index ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_ARM(index) - 1 + 'A' , IO_WAFER(index) , IO_SLOT(index) );
		}
		else if ( IO_ACT( index ) == 2 ) { // swap
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		else if ( IO_ACT( index ) == 3 ) { // move
			sprintf( RunStr  , "MAINT_INTERFACE%d" , IO_FTM( index ) );
			sprintf( ElseStr , "ROTATE_TM%d|0|%s|%c|%d|%d" , IO_FTM( index ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) , IO_ARM(index) - 1 + 'A' , IO_WAFER(index) , IO_SLOT(index) );
		}
		else if ( ( IO_ACT( index ) == 11 ) || ( IO_ACT( index ) == 12 ) || ( IO_ACT( index ) == 13 ) ) { // Process/Post/Pre
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		else if ( IO_ACT( index ) == 21 ) { // fmside
			lc = IO_CHAMBER( index ) - BM1 + MAX_TM_CHAMBER_DEPTH;
			sprintf( RunStr  , "MAINT_INTERFACE%c" , IO_CHAMBER( index ) - BM1 + 'A' );
			sprintf( ElseStr , "WAITING_FOR_FM|0|%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) );
		}
		else if ( IO_ACT( index ) == 22 ) { // tmside
			lc = IO_CHAMBER( index ) - BM1 + MAX_TM_CHAMBER_DEPTH;
			sprintf( RunStr  , "MAINT_INTERFACE%c" , IO_CHAMBER( index ) - BM1 + 'A' );
			sprintf( ElseStr , "WAITING_FOR_TM|0|%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( index ) ) );
		}
		else if ( IO_ACT( index ) == 31 ) { // al
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		else if ( IO_ACT( index ) == 32 ) { // ic
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
			return TRUE;
		}
		//
		if ( IO_STS( index ) == 9 ) {
			//==============================================================================================================================================================================
			printf( "[%d] %s [%s %s]\n" , index + 1 , IO_LOG( index ) , RunStr , ElseStr ); // 2014.09.04
			//==============================================================================================================================================================================
			Maint_Intf_Spawn( FALSE , RunStr , ElseStr );
		}
		else {
			//==============================================================================================================================================================================
			if ( !Transfer_Maint_Inf_Code_End( lc ) ) return FALSE;
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
		}
		//
	}
	else if ( IO_FTM( index ) == 100 ) { // pm
			//==============================================================================================================================================================================
			printf( "[%d] %s\n" , index + 1 , IO_LOG( index ) ); // 2014.09.04
			//==============================================================================================================================================================================
		if ( ( IO_ACT( index ) == 11 ) || ( IO_ACT( index ) == 12 ) || ( IO_ACT( index ) == 13 ) ) { // Process/Post/Pre
			//
			switch( IO_STS( index ) ) {
			case 9 :	LD_PMWafer_Data( IO_CHAMBER( index ) , WAFERRESULT_PROCESSING ); break;
			case 0 :	LD_PMWafer_Data( IO_CHAMBER( index ) , WAFERRESULT_SUCCESS ); break;
			case 1 :	LD_PMWafer_Data( IO_CHAMBER( index ) , WAFERRESULT_FAILURE ); break;
			case 2 :	LD_PMWafer_Data( IO_CHAMBER( index ) , WAFERRESULT_FAILURE ); break;
			}
			//
		}
	}

	return TRUE;
}

void LD_Action_Thread() {
	//
	while ( TRUE ) {
		//
		if ( LD_RUN_CONTROL > 0 ) { // run/pause
			//
			if ( LD_RUN_INDEX < 0 ) LD_RUN_INDEX = 0;
			//
			if ( LD_RUN_INDEX >= LD_DATA_COUNT ) {
				//
				LD_RUN_CONTROL = 0;
				//
				LD_RUN_UPDATE = 1;
				//
			}
			else {
				//
				if ( LD_RUN_CONTROL == 2 ) { // pause
					//
					if ( LD_RUN_STEP == 0 ) {
						//
						Sleep(1);
						//
						continue;
					}
					//
				}
				else {
					if ( LD_RUN_INDEX == ( LD_RUN_BREAK - 1 ) ) {
						//
						Sleep(1);
						//
						continue;
					}
				}
				//
				if ( LD_Action_Data( LD_RUN_INDEX ) ) {
					//
					LD_RUN_STEP = 0;
					//
					LD_RUN_INDEX++;
					//
					if ( LD_RUN_INDEX == ( LD_RUN_BREAK - 1 ) ) {
						//
						LD_RUN_BREAK++;
						//
						LD_RUN_CONTROL = 2;
						//
						LD_RUN_UPDATE = 1;
						//
					}
					//
					LD_RUN_UPDATE2 = 1;
					//
				}
				//
			}
			//
			Sleep(1);
			//
		}
		else {
			//
			LD_RUN_UPDATE2 = 0;
			//
			Sleep(100);
			//
		}
		//
	}
	//
	LD_RUN_THREAD = 0;
	_endthread();
}

BOOL APIENTRY Gui_IDD_MODULE_LOTDATA_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	int i , est;
	char *szString[]  = { "ID" , "Range" , "Status" , "Time" , "PIO" , "Slot" , "CS" , "ETC" , "ExpTime" , "supdev" };
	int    szSize[10] = {  35  ,      57 ,      86 ,      60 ,    40 ,     50 ,   72 ,   200 ,        72 ,       40 };
	//
	LPNMHDR lpnmh;
	LV_DISPINFO *lpdi;
	char Buffer[1024];
	static int tid;
	//
	struct tm		*Pres_Time;
	//
	time_t tdata;
	//
	static time_t imsitime[MAX_CASSETTE_SIDE][MAX_CASSETTE_SLOT_SIZE];
	static HFONT ixhFont;
	static int closemon;
	//
	switch ( msg ) {
	case WM_DESTROY:
		//
		if ( closemon ) { // 2017.03.16
			KGUI_STANDARD_Set_User_POPUP_Close( 1 ); // 2015.04.01
		}
		//
		if ( ixhFont != NULL ) DeleteFont( ixhFont );
		//
		KillTimer( hdlg , tid );
		break;

	case WM_INITDIALOG:
		//
//		KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01 // 2017.03.16
		//
		//
		if ( LD_MODE != 0 ) {
			//
			closemon = FALSE; // 2017.03.16
			//
			ixhFont = CreateFont( 14 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , DEFAULT_CHARSET , 0 , 0 , 0 , 0 , "Arial" );
			SendMessage( GetDlgItem( hdlg ,IDC_LIST_COMMON_BOX ) , WM_SETFONT , (WPARAM)ixhFont , MAKELPARAM(FALSE,0) );
			//
		}
		else {
			//
			closemon = TRUE; // 2017.03.16
			//
			KGUI_STANDARD_Set_User_POPUP_Open( 1 , hdlg , 1 ); // 2015.04.01 // 2017.03.16
			//
			ixhFont = NULL;
			//
		}
		//
		MoveCenterWindow( hdlg );
		//
		if ( LD_MODE == 0 ) {
			sprintf( Buffer , "LOT %d" , LD_PAD_INDEX + 1 );
			KWIN_Item_String_Display( hdlg , IDYES , Buffer );
			//
			if ( LD_ALL_DISP == 0 ) {
				KWIN_Item_String_Display( hdlg , IDRETRY , "Simple" );
			}
			else {
				KWIN_Item_String_Display( hdlg , IDRETRY , "All" );
			}
			//
			KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , 10 , szString , szSize );
			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			KWIN_LView_Init_CallBack_Insert( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , MAX_CASSETTE_SLOT_SIZE );
			//
			tid = SetTimer( hdlg , 3000 , 500 , NULL );
			//
		}
		else {
			//
			LD_Button_Data( hdlg );
			//
			LD_Show_Data( hdlg );
			//
			tid = SetTimer( hdlg , 3000 , 25 , NULL );
			//
		}
		return TRUE;

	case WM_RBUTTONDOWN:
		if ( LD_MODE == 0 ) {
			if ( IDYES == MessageBox( hdlg , "Do you want to Change Mode?(Current-LOT View Mode)", "Confirmation", MB_ICONQUESTION | MB_YESNO ) ) {
				EndDialog( hdlg , 2 );
			}
		}
		else {
			if ( IDYES == MessageBox( hdlg , "Do you want to Change Mode?(Current-LOT Simulate Mode)", "Confirmation", MB_ICONQUESTION | MB_YESNO ) ) {
				EndDialog( hdlg , 1 );
			}
		}
		return TRUE;

	case WM_TIMER:
		//
		if ( LD_MODE == 0 ) {
			//
			est = LOT_DATA_GET_ESTIMATE_TIME( LD_PAD_INDEX , &tdata , &(LOT_DATA_EST_SUPPLY_INTERVAL[LD_PAD_INDEX]) );
			//
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( _i_SCH_CLUSTER_Get_PathStatus( LD_PAD_INDEX , i ) == SCHEDULER_CM_END ) {
					if ( imsitime[LD_PAD_INDEX][i] == 0 ) {
						imsitime[LD_PAD_INDEX][i] = tdata;
					}
				}
				else {
					imsitime[LD_PAD_INDEX][i] = 0;
				}
			}
			//
			if ( est <= 0 ) {
				sprintf( Buffer , "LOT%d is [N/A]" , LD_PAD_INDEX + 1 );
			}
			else {
				//
				Pres_Time = localtime( &tdata );
				//
				sprintf( Buffer , "LOT%d is [Remain=%d,Target=%04d/%02d/%02d %02d:%02d:%02d]" , LD_PAD_INDEX + 1 , est , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
			}
			//
			SetWindowText( hdlg , Buffer );
			InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , NULL , FALSE );
			//
		}
		else {
			//
			if ( LD_RUN_UPDATE  == 1 ) LD_Button_Data( hdlg );
			if ( LD_RUN_UPDATE2 == 1 ) {
				//
				LD_RUN_UPDATE2 = 0;
				//
				ListView_SetItemState( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , LD_RUN_INDEX , LVIS_SELECTED , LVIS_SELECTED );
				//
				InvalidateRect( GetDlgItem( hdlg , IDC_LIST_COMMON_BOX ) , NULL , FALSE );
			}
			//
			if ( LD_RUN_CONTROL == 0 ) {
				sprintf( Buffer , "IDLE" );
			}
			else if ( LD_RUN_CONTROL == 1 ) { // run
				sprintf( Buffer , "RUNNING [%d/%d]" , LD_RUN_INDEX , LD_DATA_COUNT );
			}
			else if ( LD_RUN_CONTROL == 2 ) { // pause
				sprintf( Buffer , "PAUSE [%d/%d]" , LD_RUN_INDEX , LD_DATA_COUNT );
			}
			else {
				sprintf( Buffer , "UNKNOWN" );
			}
			//
			SetWindowText( hdlg , Buffer );
		}
		return TRUE;

	case WM_NOTIFY :
		lpnmh = (LPNMHDR) lParam;
		switch( lpnmh->code ) {
		case LVN_GETDISPINFO :
			lpdi = (LV_DISPINFO *) lParam;
			//
			Buffer[0] = 0; // 2016.02.02
			//
			if ( LD_MODE == 0 ) {
				//
				switch( lpdi->item.iSubItem ) {
				case 0 : // ID
					sprintf( Buffer , "%d" , lpdi->item.iItem );
					break;

				case 1 : // Range
					if ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) == -1 ) {
						sprintf( Buffer , "" );
					}
					else if ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) {
						sprintf( Buffer , "<%d>" , _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) );
					}
					else {
						if ( _i_SCH_CLUSTER_Get_PathDo( LD_PAD_INDEX , lpdi->item.iItem ) == PATHDO_WAFER_RETURN ) {
							sprintf( Buffer , "[%d]:RET" , _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) );
						}
						else {
							sprintf( Buffer , "[%d]:%d" , _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) , _i_SCH_CLUSTER_Get_PathDo( LD_PAD_INDEX , lpdi->item.iItem ) );
						}
					}
					break;

				case 2 : // Status
					if ( !LD_ALL_DISP && ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) ) {
						sprintf( Buffer , "" );
					}
					else {
						switch( _i_SCH_CLUSTER_Get_PathStatus( LD_PAD_INDEX , lpdi->item.iItem ) ) {
						case SCHEDULER_READY			:
							if ( LD_ALL_DISP ) {
								strcpy( Buffer , "" );
							}
							else {
								strcpy( Buffer , "Ready" );
							}
							break;
						case SCHEDULER_SUPPLY			: strcpy( Buffer , "SUPPLY" ); break;
						case SCHEDULER_STARTING			: strcpy( Buffer , "STARTING" ); break;
						case SCHEDULER_RUNNING			: strcpy( Buffer , "RUNNING" ); break;
						case SCHEDULER_RUNNINGW			: strcpy( Buffer , "RUNNINGW" ); break;
						case SCHEDULER_RUNNING2			: strcpy( Buffer , "RUNNING2" ); break;
						case SCHEDULER_WAITING			: strcpy( Buffer , "WAITING" ); break;
						case SCHEDULER_BM_END			: strcpy( Buffer , "BM_END" ); break;
						case SCHEDULER_CM_END			: strcpy( Buffer , "CM_END" ); break;
						case SCHEDULER_RETURN_REQUEST	: strcpy( Buffer , "RET_REQ" ); break;
						default							: sprintf( Buffer , "%d" , _i_SCH_CLUSTER_Get_PathStatus( LD_PAD_INDEX , lpdi->item.iItem ) ); break;
						}
					}
					break;

				case 3 : // Time
					if ( !LD_ALL_DISP && ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) ) {
						sprintf( Buffer , "" );
					}
					else {
						if ( ( _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , lpdi->item.iItem ) == 0 ) && ( _i_SCH_CLUSTER_Get_EndTime( LD_PAD_INDEX , lpdi->item.iItem ) == 0 ) ) {
							sprintf( Buffer , "-" );
						}
						else if ( _i_SCH_CLUSTER_Get_EndTime( LD_PAD_INDEX , lpdi->item.iItem ) == 0 ) {
							if ( !_i_SCH_SYSTEM_USING_RUNNING( LD_PAD_INDEX ) ) {
								sprintf( Buffer , "" );
							}
							else {
								sprintf( Buffer , "*(%d)" , ( GetTickCount() - _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , lpdi->item.iItem ) ) / 1000 );
							}
						}
						else {
							sprintf( Buffer , "%d" , ( _i_SCH_CLUSTER_Get_EndTime( LD_PAD_INDEX , lpdi->item.iItem ) - _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , lpdi->item.iItem ) ) / 1000 );
						}
					}
					break;

				case 4 : // Path
					if ( !LD_ALL_DISP && ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) ) {
						sprintf( Buffer , "" );
					}
					else {
						sprintf( Buffer , "%d:%d" , _i_SCH_CLUSTER_Get_PathIn( LD_PAD_INDEX , lpdi->item.iItem ) - CM1 + 1, _i_SCH_CLUSTER_Get_PathOut( LD_PAD_INDEX , lpdi->item.iItem ) - CM1 + 1 );
					}
					break;

				case 5 : // Slot
					if ( !LD_ALL_DISP && ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) ) {
						sprintf( Buffer , "" );
					}
					else {
						sprintf( Buffer , "%d:%d" , _i_SCH_CLUSTER_Get_SlotIn( LD_PAD_INDEX , lpdi->item.iItem ) , _i_SCH_CLUSTER_Get_SlotOut( LD_PAD_INDEX , lpdi->item.iItem ) );
					}
					break;

				case 6 : // CS
					if ( !LD_ALL_DISP && ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) ) {
						sprintf( Buffer , "" );
					}
					else {
						sprintf( Buffer , "%d:%d" , _i_SCH_CLUSTER_Get_ClusterIndex( LD_PAD_INDEX , lpdi->item.iItem ) , _i_SCH_CLUSTER_Get_SupplyID( LD_PAD_INDEX , lpdi->item.iItem ) );
					}
					break;

				case 7 : // ETC
					if ( !LD_ALL_DISP && ( _i_SCH_CLUSTER_Get_PathRange( LD_PAD_INDEX , lpdi->item.iItem ) < 0 ) ) {
						sprintf( Buffer , "" );
					}
					else {
						sprintf( Buffer , "W%d:F%d:E%d:PH%d:CC%d:CP%d:B%d:OP%d:ST%d:LS%d"
							, _i_SCH_CLUSTER_Get_SwitchInOut( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_FailOut( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_Extra( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_PathHSide( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_CPJOB_CONTROL( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_CPJOB_PROCESS( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_Buffer( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_Optimize( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_Stock( LD_PAD_INDEX , lpdi->item.iItem )
							, _i_SCH_CLUSTER_Get_LotSpecial( LD_PAD_INDEX , lpdi->item.iItem )
							);
					}
					break;

				case 8 : // ET
					if ( imsitime[LD_PAD_INDEX][lpdi->item.iItem] <= 0 ) {
						sprintf( Buffer , "" );
					}
					else {
						Pres_Time = localtime( &(imsitime[LD_PAD_INDEX][lpdi->item.iItem]) );
	//					sprintf( Buffer , "%04d/%02d/%02d %02d:%02d:%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
						sprintf( Buffer , "%02d %02d:%02d:%02d" , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
					}
					break;

				case 9 : // supdev
					est = -1;
					for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
						if ( _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , i ) <= 0 ) {
						}
						else {
							if ( est == -1 ) {
								est = i;
							}
							else {
								if ( _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , i ) < _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , est ) ) {
									est = i;
								}
							}
						}
					}
					//
					if ( est == -1 ) {
						sprintf( Buffer , "" );
					}
					else {
						if ( est == lpdi->item.iItem ) {
							sprintf( Buffer , "*" );
						}
						else {
							if ( _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , lpdi->item.iItem ) <= 0 ) {
								sprintf( Buffer , "" );
							}
							else {
								sprintf( Buffer , "%d" , ( _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , lpdi->item.iItem ) - _i_SCH_CLUSTER_Get_StartTime( LD_PAD_INDEX , est ) ) / 1000 );
							}
						}
					}
					break;
				}
//				lstrcpy( lpdi->item.pszText , Buffer ); // 2016.02.02
			}
			else {
				//
				switch( lpdi->item.iSubItem ) {
				case 0 : // ID
					if ( ( LD_RUN_CONTROL != 0 ) && ( LD_RUN_INDEX == lpdi->item.iItem ) ) {
						strcpy( Buffer , "*" );
					}
					else {
						sprintf( Buffer , "%d" , lpdi->item.iItem );
					}
					break;

				case 1 : // Time
					if ( IO_TIME_MSEC(lpdi->item.iItem) > 0 ) {
						sprintf( Buffer , "%4d/%2d/%2d %2d:%2d:%2d.%03d" , IO_TIME_YEAR( lpdi->item.iItem ) , IO_TIME_MON( lpdi->item.iItem ) , IO_TIME_DAY( lpdi->item.iItem ) , IO_TIME_HOUR( lpdi->item.iItem ) , IO_TIME_MIN( lpdi->item.iItem ) , IO_TIME_SEC( lpdi->item.iItem ) , IO_TIME_MSEC( lpdi->item.iItem ) );
					}
					else {
						sprintf( Buffer , "%4d/%2d/%2d %2d:%2d:%2d" , IO_TIME_YEAR( lpdi->item.iItem ) , IO_TIME_MON( lpdi->item.iItem ) , IO_TIME_DAY( lpdi->item.iItem ) , IO_TIME_HOUR( lpdi->item.iItem ) , IO_TIME_MIN( lpdi->item.iItem ) , IO_TIME_SEC( lpdi->item.iItem ) );
					}
					break;

				case 2 : // lot
					if ( IO_LOT( lpdi->item.iItem ) > 0 ) {
						sprintf( Buffer , "%d" , IO_LOT( lpdi->item.iItem ) );
					}
					else {
						strcpy( Buffer , "" );
					}
					break;

				case 3 : // log
					sprintf( Buffer , "%s" , IO_LOG( lpdi->item.iItem ) );
					break;

				case 4 : // ftm
					if     ( IO_FTM( lpdi->item.iItem ) < 0 ) {
						strcpy( Buffer , "" );
					}
					else if ( IO_FTM( lpdi->item.iItem ) == 99 ) {
						strcpy( Buffer , "ALARM" );
					}
					else if ( IO_FTM( lpdi->item.iItem ) == 100 ) {
						strcpy( Buffer , "PM" );
					}
					else if ( IO_FTM( lpdi->item.iItem ) == 0 ) {
						strcpy( Buffer , "FM" );
					}
					else {
						sprintf( Buffer , "TM%d" , IO_FTM( lpdi->item.iItem ) );
					}
					break;

				case 5 : // arm
					if ( IO_ACT( lpdi->item.iItem ) >= 11 ) {
						strcpy( Buffer , "-" );
					}
					else {
						if     ( IO_ARM( lpdi->item.iItem ) < 0 ) {
							strcpy( Buffer , "" );
						}
						else if ( IO_ARM( lpdi->item.iItem ) == 0 ) {
							strcpy( Buffer , "AB" );
						}
						else if ( IO_ARM( lpdi->item.iItem ) == 1 ) {
							strcpy( Buffer , "A" );
						}
						else if ( IO_ARM( lpdi->item.iItem ) == 2 ) {
							strcpy( Buffer , "B" );
						}
						else {
							sprintf( Buffer , "%d" , IO_ARM( lpdi->item.iItem ) );
						}
					}
					break;

				case 6 : // ACT
					if ( IO_ACT( lpdi->item.iItem ) == 0 ) {
						strcpy( Buffer , "PICK" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 1 ) {
						strcpy( Buffer , "PLACE" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 2 ) {
						strcpy( Buffer , "SWAP" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 3 ) {
						strcpy( Buffer , "MOVE" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 11 ) {
						strcpy( Buffer , "PROCESS" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 12 ) {
						strcpy( Buffer , "PROCESS(POST)" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 13 ) {
						strcpy( Buffer , "PROCESS(PRE)" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 21 ) {
						strcpy( Buffer , "FMSIDE" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 22 ) {
						strcpy( Buffer , "TMSIDE" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 31 ) {
						strcpy( Buffer , "AL" );
					}
					else if ( IO_ACT( lpdi->item.iItem ) == 32 ) {
						strcpy( Buffer , "IC" );
					}
					else {
						strcpy( Buffer , "" );
					}
					break;

				case 7 : // STS
					if ( IO_STS( lpdi->item.iItem ) == 0 ) {
						strcpy( Buffer , "S" );
					}
					else if ( IO_STS( lpdi->item.iItem ) == 1 ) {
						strcpy( Buffer , "A" );
					}
					else if ( IO_STS( lpdi->item.iItem ) == 2 ) {
						strcpy( Buffer , "E" );
					}
					else if ( IO_STS( lpdi->item.iItem ) == 9 ) {
						strcpy( Buffer , "R" );
					}
					else {
						strcpy( Buffer , "" );
					}
					break;

				case 8 : // ch
					//
					if ( IO_CHAMBER( lpdi->item.iItem ) <= 0 ) {
						strcpy( Buffer , "" );
					}
					else { 
						sprintf( Buffer , "%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( IO_CHAMBER( lpdi->item.iItem ) ) );
					}
					//
					break;

				case 9 : // slot1
					if     ( IO_SLOT( lpdi->item.iItem ) <= 0 ) {
						if ( ( IO_ACT( lpdi->item.iItem ) == 21 ) || ( IO_ACT( lpdi->item.iItem ) == 22 ) ) {
							strcpy( Buffer , "-" );
						}
						else {
							strcpy( Buffer , "" );
						}
					}
					else {
						sprintf( Buffer , "%d" , IO_SLOT( lpdi->item.iItem ) );
					}
					break;

				case 10 : // slot2
					if     ( IO_WAFER( lpdi->item.iItem ) <= 0 ) {
						if ( ( IO_ACT( lpdi->item.iItem ) == 21 ) || ( IO_ACT( lpdi->item.iItem ) == 22 ) ) {
							strcpy( Buffer , "-" );
						}
						else {
							strcpy( Buffer , "" );
						}
					}
					else {
						sprintf( Buffer , "%d" , IO_WAFER( lpdi->item.iItem ) );
					}
					break;

				case 11 : // sw
					if     ( IO_SWITCH( lpdi->item.iItem ) < 0 ) {
						sprintf( Buffer , "%s" , IO_RECIPE( lpdi->item.iItem ) );
					}
					else if ( IO_SWITCH( lpdi->item.iItem ) == 0 ) {
						strcpy( Buffer , "" );
					}
					else {
						sprintf( Buffer , "%d" , IO_SWITCH( lpdi->item.iItem ) );
					}
					break;

				case 12 : // Dummy
					if     ( IO_DUMMY( lpdi->item.iItem ) != 0 ) {
						strcpy( Buffer , "D" );
					}
					else {
						strcpy( Buffer , "" );
					}
					break;

				case 13 : // etc
					if     ( IO_ETC( lpdi->item.iItem ) != 0 ) {
						sprintf( Buffer , "%d" , IO_ETC( lpdi->item.iItem ) );
					}
					else {
						strcpy( Buffer , "" );
					}
					break;

				case 14 : // fm
				case 15 : // Tm1
				case 16 : // Tm2
				case 17 : // Tm3
				case 18 : // Tm4
					if ( IO_FTM( lpdi->item.iItem ) == ( lpdi->item.iSubItem - 14 ) ) {
						if ( IO_ACT( lpdi->item.iItem ) >= 11 ) {
							strcpy( Buffer , "" );
						}
						else {
							if ( IO_STS( lpdi->item.iItem ) == 0 ) {
								strcpy( Buffer , "(-----" );
							}
							else if ( IO_STS( lpdi->item.iItem ) == 1 ) {
								strcpy( Buffer , "(-----" );
							}
							else if ( IO_STS( lpdi->item.iItem ) == 2 ) {
								strcpy( Buffer , "(-----" );
							}
							else if ( IO_STS( lpdi->item.iItem ) == 9 ) {
								strcpy( Buffer , "-----)" );
							}
							else {
								strcpy( Buffer , "" );
							}
						}
					}
					else {
						strcpy( Buffer , "" );
					}
					break;
				}
//				lstrcpy( lpdi->item.pszText , Buffer ); // 2016.02.02
			}
			//
			lstrcpy( lpdi->item.pszText , Buffer ); // 2016.02.02
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
		//===============================================================================================================
		//===============================================================================================================
		case IDC_READY_USE_T1 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_TM )	LD_RUN_MSG_TM = FALSE;
				else					LD_RUN_MSG_TM = TRUE;
				if ( LD_RUN_MSG_TM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T1 , "O" );
				else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T1 , "" );
			}
			break;
		case IDC_READY_USE_T2 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_PM )	LD_RUN_MSG_PM = FALSE;
				else					LD_RUN_MSG_PM = TRUE;
				if ( LD_RUN_MSG_PM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T2 , "O" );
				else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T2 , "" );
			}
			break;
		case IDC_READY_USE_T3 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_BM )	LD_RUN_MSG_BM = FALSE;
				else					LD_RUN_MSG_BM = TRUE;
				if ( LD_RUN_MSG_BM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T3 , "O" );
				else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T3 , "" );
			}
			break;
		case IDC_READY_USE_T4 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_FM )	LD_RUN_MSG_FM = FALSE;
				else					LD_RUN_MSG_FM = TRUE;
				if ( LD_RUN_MSG_FM )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T4 , "O" );
				else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T4 , "" );
			}
			break;
		case IDC_READY_USE_T5 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_AL )	LD_RUN_MSG_AL = FALSE;
				else					LD_RUN_MSG_AL = TRUE;
				if ( LD_RUN_MSG_AL )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T5 , "O" );
				else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T5 , "" );
			}
			break;
		case IDC_READY_USE_T6 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_IC )	LD_RUN_MSG_IC = FALSE;
				else					LD_RUN_MSG_IC = TRUE;
				if ( LD_RUN_MSG_IC )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T6 , "O" );
				else					KWIN_Item_String_Display( hdlg , IDC_READY_USE_T6 , "" );
			}
			break;
		case IDC_READY_USE_T7 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_BMSIDE )	LD_RUN_MSG_BMSIDE = FALSE;
				else						LD_RUN_MSG_BMSIDE = TRUE;
				if ( LD_RUN_MSG_BMSIDE )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T7 , "O" );
				else						KWIN_Item_String_Display( hdlg , IDC_READY_USE_T7 , "" );
			}
			break;
		case IDC_READY_USE_T8 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_MSG_PMPRCS )	LD_RUN_MSG_PMPRCS = FALSE;
				else						LD_RUN_MSG_PMPRCS = TRUE;
				if ( LD_RUN_MSG_PMPRCS )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T8 , "O" );
				else						KWIN_Item_String_Display( hdlg , IDC_READY_USE_T8 , "" );
			}
			break;

		case IDC_READY_USE_T9 :
			if ( LD_MODE == 1 ) {
				if ( LD_RUN_IO_MAINTAIN )	LD_RUN_IO_MAINTAIN = FALSE;
				else						LD_RUN_IO_MAINTAIN = TRUE;
				if ( LD_RUN_IO_MAINTAIN )	KWIN_Item_String_Display( hdlg , IDC_READY_USE_T9 , "MAINTAIN" );
				else						KWIN_Item_String_Display( hdlg , IDC_READY_USE_T9 , "RESET" );
			}
			break;

		//===============================================================================================================
		//===============================================================================================================
		case IDCANCEL :
		case IDCLOSE :
			EndDialog( hdlg , 0 );
			return TRUE;
		case IDOK :
			if ( LD_MODE == 0 ) {
				InvalidateRect( hdlg , NULL , TRUE );
			}
			else {
				//
				if ( LD_RUN_CONTROL == 0 ) {
					//
					if ( UTIL_Get_OpenSave_File_Select( hdlg , FALSE , Buffer , 255 , "" , "*.*" , "Select Lot Log File" ) ) {
						//
						est = LD_Get_Data( hdlg , Buffer );
						//
						LD_RUN_INDEX = 0;
						//
						LD_Show_Data( hdlg );
						//
						if ( est != 0 ) {
							sprintf( Buffer , "LD_Get_Data Fail [%d]" , est ); 
							MessageBox( hdlg , Buffer , "Error", MB_ICONQUESTION );
						}
						//
					}
					//
				}
			}
			return TRUE;
		case IDABORT :
			if ( LD_MODE == 0 ) {
				InvalidateRect( hdlg , NULL , TRUE );
			}
			else {
				//
				if ( UTIL_Get_OpenSave_File_Select( hdlg , TRUE , Buffer , 255 , "" , "*.*" , "Select Lot Log File for Save" ) ) {
					//
					est = LD_Set_Data( hdlg , Buffer );
					//
					if ( est != 0 ) {
						sprintf( Buffer , "LD_Set_Data Fail [%d]" , est ); 
						MessageBox( hdlg , Buffer , "Error", MB_ICONQUESTION );
					}
					//
				}
				//
			}
			return TRUE;
		case IDYES :
			if ( LD_MODE == 0 ) {
				LD_PAD_INDEX++;
				if ( LD_PAD_INDEX >= MAX_CASSETTE_SIDE ) LD_PAD_INDEX = 0;
				sprintf( Buffer , "LOT %d" , LD_PAD_INDEX + 1 );
				KWIN_Item_String_Display( hdlg , IDYES , Buffer );
				InvalidateRect( hdlg , NULL , TRUE );
			}
			else {
				if ( LD_RUN_CONTROL == 0 ) {
					if ( IDYES == MessageBox( hdlg , "Do you want to Clear of All Data?", "Confirmation", MB_ICONQUESTION | MB_YESNO ) ) {
						//
						LD_DATA_CLEAR( LD_TABLE , &LD_DATA_COUNT );
						//
						LD_Show_Data( hdlg );
						//
					}
				}
			}
			return TRUE;
		case IDHELP :
			if ( LD_MODE == 0 ) {
			}
			else {
				//
				if ( LD_RUN_CONTROL == 0 ) {
				}
				else if ( LD_RUN_CONTROL == 1 ) { // run
				}
				else if ( LD_RUN_CONTROL == 2 ) { // pause
					LD_RUN_STEP = 1;
				}
				//
			}
			return TRUE;
		case IDC_READY_USE_B1 :
			if ( LD_MODE == 0 ) {
			}
			else {
				i = LD_RUN_BREAK;
				if ( MODAL_DIGITAL_BOX2( hdlg , "Select break index(0=N/A)" , "Select" , 0 , LD_DATA_COUNT , &i ) ) {
					//
					LD_RUN_BREAK = i;
					//
					LD_RUN_UPDATE = 1;
					//
				}
			}
			return TRUE;
		case IDNO :
			if ( LD_MODE == 0 ) {
			}
			else {
				//
				if ( LD_RUN_CONTROL == 0 ) {
					LD_RUN_MODE++;
					if ( LD_RUN_MODE > 5 ) LD_RUN_MODE = 0;
				}
				else if ( LD_RUN_CONTROL == 1 ) { // run
					LD_RUN_STEP = 0;
					LD_RUN_CONTROL = 2;
				}
				else if ( LD_RUN_CONTROL == 2 ) { // pause
					LD_RUN_CONTROL = 1;
				}
				//
				LD_Button_Data( hdlg );
				//
			}
			return TRUE;
		case IDRETRY :
			if ( LD_MODE == 0 ) {
				//
				if ( LD_ALL_DISP == 0 ) {
					LD_ALL_DISP = 1;
					KWIN_Item_String_Display( hdlg , IDRETRY , "All" );
				}
				else {
					LD_ALL_DISP = 0;
					KWIN_Item_String_Display( hdlg , IDRETRY , "Simple" );
				}
				//
				InvalidateRect( hdlg , NULL , TRUE );
				//
			}
			else {
				//
				if ( LD_RUN_CONTROL == 0 ) {
					//
					if ( Get_RunPrm_RUNNING_CLUSTER()) return TRUE;
					if ( Get_RunPrm_RUNNING_TRANSFER()) return TRUE;
					//
					if ( LD_RUN_THREAD == 0 ) {
						//
						if ( _beginthread( (void *) LD_Action_Thread , 0 , 0 ) == -1 ) return TRUE;
						//
						LD_RUN_THREAD = 1;
						//
					}
					//
					if ( !LD_RUN_IO_MAINTAIN ) _dWRITE_FUNCTION_EVENT( -1 , "CASSETTE SIM" );
					//
					LD_RUN_INDEX = 0;
					//
					LD_RUN_CONTROL = 1;
					//
				}
				else if ( LD_RUN_CONTROL == 1 ) { // run
					LD_RUN_CONTROL = 0;
				}
				else if ( LD_RUN_CONTROL == 2 ) { // pause
					LD_RUN_CONTROL = 0;
				}
				//
				LD_Button_Data( hdlg );
				//
			}
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

//
void GUI_LotData_Show( HWND hWnd ) {
	int Res;
	Res = GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE( ( LD_MODE == 0 ) ? IDD_MODULE_LOTDATA_PAD : IDD_MODULE_LOTDATA_PAD2 ) , hWnd , Gui_IDD_MODULE_LOTDATA_PAD_Proc , (LPARAM) NULL );
	//
	if      ( Res == 1 ) {
		LD_MODE = 0;
	}
	else if ( Res == 2 ) {
		LD_MODE = 1;
	}
}

//=====================================================================================================
// 2006.08.11
//=====================================================================================================
BOOL SCHEDULING_LOT_PREDICT_TIME_CHECK_INIT = FALSE;
int Address_IO_For_Lot_End_Estimate[MAX_CASSETTE_SIDE];

void SCHEDULING_LOT_PREDICT_TIME_CHECK( int idx ) {
	int i , est;
	struct tm		*Pres_Time;
	time_t tdata;
	char Buffer[22];

	if ( !_i_SCH_PRM_GET_LOT_END_ESTIMATION() ) return;
	//
	if ( !SCHEDULING_LOT_PREDICT_TIME_CHECK_INIT ) { // 2010.02.01
		//
		SCHEDULING_LOT_PREDICT_TIME_CHECK_INIT = TRUE;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) Address_IO_For_Lot_End_Estimate[i] = -2;
		//
	}
	//
	for ( i = ( ( idx == -1 ) ? 0 : idx ) ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//----------------
		if ( Address_IO_For_Lot_End_Estimate[i] == -2 ) {
			if ( i == 0 )	Address_IO_For_Lot_End_Estimate[i] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.ESTIMATION_TIME" );
			else			Address_IO_For_Lot_End_Estimate[i] = _FIND_FROM_STRINGF( _K_S_IO , "CTC.ESTIMATION_TIME%d" , i + 1 );
		}
		//----------------
		if ( Address_IO_For_Lot_End_Estimate[i] == -1 ) continue;
		//----------------
//		est = LOT_DATA_GET_ESTIMATE_TIME( i , &tdata , &(LOT_DATA_EST_SUPPLY_INTERVAL[LD_PAD_INDEX]) ); // 2006.09.25
		est = LOT_DATA_GET_ESTIMATE_TIME( i , &tdata , &(LOT_DATA_EST_SUPPLY_INTERVAL[i]) ); // 2006.09.25
		//
		if ( est <= 0 ) {
			strcpy( Buffer , "" );	
		}
		else {
			Pres_Time = localtime( &tdata );
			//
			sprintf( Buffer , "%04d/%02d/%02d %02d:%02d:%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
		}
		//----------------
		_dWRITE_STRING( Address_IO_For_Lot_End_Estimate[i] , Buffer	, &est );
		//----------------
		if ( idx != -1 ) break;
		//----------------
	}
}




























//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================


#define MAX_FLOW_MAP_COUNT	256

int  _FLOW_HAS_GRP0_FM = 0;
int	 _FLOW_MAP_S_COUNT = 0;

char _FLOW_MAP_S_GROUP[MAX_FLOW_MAP_COUNT];
int	 _FLOW_MAP_S_MODULE[MAX_FLOW_MAP_COUNT];
int	 _FLOW_MAP_S_MODULE_E[MAX_FLOW_MAP_COUNT];

int	 _FLOW_MAP_D_COUNT = 0;

char _FLOW_MAP_D_GROUP[MAX_FLOW_MAP_COUNT];
char _FLOW_MAP_D_MODE[MAX_FLOW_MAP_COUNT]; // pick,place
int	 _FLOW_MAP_D_MODULE[MAX_FLOW_MAP_COUNT];
char _FLOW_MAP_D_MODULE_C[MAX_FLOW_MAP_COUNT];
int	 _FLOW_MAP_D_MODULE_E[MAX_FLOW_MAP_COUNT];

void _FLOW_MAP_INIT() {
	_FLOW_MAP_S_COUNT = 0;
	_FLOW_MAP_D_COUNT = 0;
	_FLOW_HAS_GRP0_FM = _i_SCH_PRM_GET_MODULE_MODE( FM ) ? 1 : 0;
}

int _FLOW_MAP_S_USING( int ch , char grp ) {
	int i , xi;
	//
	xi = -1;
	//
	for ( i = 0 ; i < _FLOW_MAP_S_COUNT ; i++ ) {
		if ( _FLOW_MAP_S_GROUP[i] < 0 ) {
			if ( xi == -1 ) xi = i;
		}
		else if ( _FLOW_MAP_S_GROUP[i] == grp ) {
			if ( _FLOW_MAP_S_MODULE[i] == ch ) {
				return 2;
			}
		}
	}
	//
	if ( _FLOW_MAP_S_COUNT >= MAX_FLOW_MAP_COUNT ) {
		if ( xi == -1 ) {
			return 0;
		}
		else {
			_FLOW_MAP_S_GROUP[xi] = grp;
			_FLOW_MAP_S_MODULE[xi] = ch;
			return 3;
		}
	}
	//
	_FLOW_MAP_S_GROUP[_FLOW_MAP_S_COUNT] = grp;
	_FLOW_MAP_S_MODULE[_FLOW_MAP_S_COUNT] = ch;
	//
	_FLOW_MAP_S_COUNT++;
	//
	return 1;
	//
}

BOOL _FLOW_MAP_S_DELETE( int ch , char grp ) {
	int i;
	//
	for ( i = 0 ; i < _FLOW_MAP_S_COUNT ; i++ ) {
		if ( _FLOW_MAP_S_GROUP[i] == grp ) {
			if ( _FLOW_MAP_S_MODULE[i] == ch ) {
				//
				_FLOW_MAP_S_GROUP[i] = -1;
				//
				return TRUE;
			}
		}
	}
	//
	return FALSE;
	//
}

BOOL _FLOW_MAP_S_FIND( int ch , char grp ) {
	int i;
	//
	for ( i = 0 ; i < _FLOW_MAP_S_COUNT ; i++ ) {
		if ( _FLOW_MAP_S_GROUP[i] == grp ) {
			if ( _FLOW_MAP_S_MODULE[i] == ch ) {
				return TRUE;
			}
		}
	}
	//
	return FALSE;
	//
}

int _FLOW_MAP_D_ADD( int ch , char mode , char grp ) {
	int i , xi;
	//
	xi = -1;
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) {
			if ( xi == -1 ) xi = i;
		}
		else if ( _FLOW_MAP_D_GROUP[i] == grp ) {
			//
			if ( _FLOW_MAP_D_MODE[i] == mode ) {
				if ( _FLOW_MAP_D_MODULE[i] == ch ) return 2;
			}
			//
		}
		//
	}
	//
	if ( _FLOW_MAP_D_COUNT >= MAX_FLOW_MAP_COUNT ) {
		if ( xi == -1 ) {
			return 0;
		}
		else {
			//
			_FLOW_MAP_D_GROUP[xi] = grp;
			_FLOW_MAP_D_MODE[xi] = mode;
			_FLOW_MAP_D_MODULE[xi] = ch;
			//
			return 3;
		}
	}
	//
	_FLOW_MAP_D_GROUP[_FLOW_MAP_D_COUNT] = grp;
	_FLOW_MAP_D_MODE[_FLOW_MAP_D_COUNT] = mode;
	_FLOW_MAP_D_MODULE[_FLOW_MAP_D_COUNT] = ch;
	//
	_FLOW_MAP_D_COUNT++;
	//
	return 1;
	//
}

int _FLOW_MAP_D_ADD_E( int ch , char mode , char grp , int che ) {
	int i;
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) {
			//
			_FLOW_MAP_D_GROUP[i] = grp;
			_FLOW_MAP_D_MODE[i] = mode;
			_FLOW_MAP_D_MODULE[i] = ch;
			_FLOW_MAP_D_MODULE_E[i] = che;
			//
			return 3;
		}
		else if ( _FLOW_MAP_D_GROUP[i] == grp ) {
			//
			if ( _FLOW_MAP_D_MODE[i] == mode ) {
				if ( _FLOW_MAP_D_MODULE[i] == ch ) {
					if ( _FLOW_MAP_D_MODULE_E[i] == che ) {
						return 2;
					}
				}
			}
			//
		}
		//
	}
	//
	if ( _FLOW_MAP_D_COUNT >= MAX_FLOW_MAP_COUNT ) return 0;
	//
	_FLOW_MAP_D_GROUP[_FLOW_MAP_D_COUNT] = grp;
	_FLOW_MAP_D_MODE[_FLOW_MAP_D_COUNT] = mode;
	_FLOW_MAP_D_MODULE[_FLOW_MAP_D_COUNT] = ch;
	_FLOW_MAP_D_MODULE_E[_FLOW_MAP_D_COUNT] = che;
	//
	_FLOW_MAP_D_COUNT++;
	//
	return 1;
	//
}

BOOL _FLOW_MAP_D_DELETE( int ch , char mode , char grp ) {
	int i;
	BOOL find;
	//
	find = FALSE;
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] == grp ) {
			//
			if ( _FLOW_MAP_D_MODE[i] == mode ) {
				//
				if ( _FLOW_MAP_D_MODULE[i] == ch ) {
					//
					find = TRUE;
					//
					_FLOW_MAP_D_GROUP[i] = -1;
					//
				}
			}
			//
		}
		//
	}
	//
	return find;
	//
}

int _FLOW_MAP_D_USING( int ch , char mode , char grp , BOOL Using ) {
	int Res;
	//

	if ( !Using ) {
		//
		_FLOW_MAP_D_DELETE( ch , mode , grp );
		//
		if ( !_FLOW_MAP_S_DELETE( ch , grp ) ) return 1;
		//
		if ( mode == MACRO_PICK ) {
			Res = _FLOW_MAP_D_ADD( ch , MACRO_PLACE , grp );	if ( Res == 0 ) return 0;
		}
		else {
			Res = _FLOW_MAP_D_ADD( ch , MACRO_PICK , grp );	if ( Res == 0 ) return 0;
		}
		//
	}
	else {
		//
		if ( _FLOW_MAP_S_FIND( ch , grp ) ) return 2;
		//
		Res = _FLOW_MAP_D_ADD( ch , mode , grp );	if ( Res == 0 ) return 0;
		//
	}
	//
	return 3;
	//
}

void _FLOW_MAP_D_SORT() {
	int i , j , ri;
	char rc;
	//
	// group - mode - module
	//
	for ( i = 0 ; i < (_FLOW_MAP_D_COUNT-1) ; i++ ) {
		for ( j = (i+1) ; j < _FLOW_MAP_D_COUNT ; j++ ) {
			//
			if ( ( _FLOW_MAP_D_GROUP[i] < 0 ) && ( _FLOW_MAP_D_GROUP[j] >= 0 ) ) {
				//
				rc = _FLOW_MAP_D_GROUP[i];
				_FLOW_MAP_D_GROUP[i] = _FLOW_MAP_D_GROUP[j];
				_FLOW_MAP_D_GROUP[j] = rc;
				//
				rc = _FLOW_MAP_D_MODE[i];
				_FLOW_MAP_D_MODE[i] = _FLOW_MAP_D_MODE[j];
				_FLOW_MAP_D_MODE[j] = rc;
				//
				ri = _FLOW_MAP_D_MODULE[i];
				_FLOW_MAP_D_MODULE[i] = _FLOW_MAP_D_MODULE[j];
				_FLOW_MAP_D_MODULE[j] = ri;
				//
				ri = _FLOW_MAP_D_MODULE_E[i];
				_FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE_E[j];
				_FLOW_MAP_D_MODULE_E[j] = ri;
				//
			}
			else if ( ( _FLOW_MAP_D_GROUP[i] >= 0 ) && ( _FLOW_MAP_D_GROUP[j] >= 0 ) ) {
				if ( _FLOW_MAP_D_GROUP[i] > _FLOW_MAP_D_GROUP[j] ) {
					//
					rc = _FLOW_MAP_D_GROUP[i];
					_FLOW_MAP_D_GROUP[i] = _FLOW_MAP_D_GROUP[j];
					_FLOW_MAP_D_GROUP[j] = rc;
					//
					rc = _FLOW_MAP_D_MODE[i];
					_FLOW_MAP_D_MODE[i] = _FLOW_MAP_D_MODE[j];
					_FLOW_MAP_D_MODE[j] = rc;
					//
					ri = _FLOW_MAP_D_MODULE[i];
					_FLOW_MAP_D_MODULE[i] = _FLOW_MAP_D_MODULE[j];
					_FLOW_MAP_D_MODULE[j] = ri;
					//
					ri = _FLOW_MAP_D_MODULE_E[i];
					_FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE_E[j];
					_FLOW_MAP_D_MODULE_E[j] = ri;
					//
				}
				else if ( _FLOW_MAP_D_GROUP[i] == _FLOW_MAP_D_GROUP[j] ) {
					//
					if ( _FLOW_MAP_D_MODE[i] > _FLOW_MAP_D_MODE[j] ) {
						//
						rc = _FLOW_MAP_D_GROUP[i];
						_FLOW_MAP_D_GROUP[i] = _FLOW_MAP_D_GROUP[j];
						_FLOW_MAP_D_GROUP[j] = rc;
						//
						rc = _FLOW_MAP_D_MODE[i];
						_FLOW_MAP_D_MODE[i] = _FLOW_MAP_D_MODE[j];
						_FLOW_MAP_D_MODE[j] = rc;
						//
						ri = _FLOW_MAP_D_MODULE[i];
						_FLOW_MAP_D_MODULE[i] = _FLOW_MAP_D_MODULE[j];
						_FLOW_MAP_D_MODULE[j] = ri;
						//
						ri = _FLOW_MAP_D_MODULE_E[i];
						_FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE_E[j];
						_FLOW_MAP_D_MODULE_E[j] = ri;
						//
					}
					else if ( _FLOW_MAP_D_MODE[i] == _FLOW_MAP_D_MODE[j] ) {
						//
						if ( _FLOW_MAP_D_MODULE[i] > _FLOW_MAP_D_MODULE[j] ) {
							//
							rc = _FLOW_MAP_D_GROUP[i];
							_FLOW_MAP_D_GROUP[i] = _FLOW_MAP_D_GROUP[j];
							_FLOW_MAP_D_GROUP[j] = rc;
							//
							rc = _FLOW_MAP_D_MODE[i];
							_FLOW_MAP_D_MODE[i] = _FLOW_MAP_D_MODE[j];
							_FLOW_MAP_D_MODE[j] = rc;
							//
							ri = _FLOW_MAP_D_MODULE[i];
							_FLOW_MAP_D_MODULE[i] = _FLOW_MAP_D_MODULE[j];
							_FLOW_MAP_D_MODULE[j] = ri;
							//
							ri = _FLOW_MAP_D_MODULE_E[i];
							_FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE_E[j];
							_FLOW_MAP_D_MODULE_E[j] = ri;
							//
						}
						else if ( _FLOW_MAP_D_MODULE[i] == _FLOW_MAP_D_MODULE[j] ) {
							//
							if ( _FLOW_MAP_D_MODULE_E[i] > _FLOW_MAP_D_MODULE_E[j] ) {
								//
								rc = _FLOW_MAP_D_GROUP[i];
								_FLOW_MAP_D_GROUP[i] = _FLOW_MAP_D_GROUP[j];
								_FLOW_MAP_D_GROUP[j] = rc;
								//
								rc = _FLOW_MAP_D_MODE[i];
								_FLOW_MAP_D_MODE[i] = _FLOW_MAP_D_MODE[j];
								_FLOW_MAP_D_MODE[j] = rc;
								//
								ri = _FLOW_MAP_D_MODULE[i];
								_FLOW_MAP_D_MODULE[i] = _FLOW_MAP_D_MODULE[j];
								_FLOW_MAP_D_MODULE[j] = ri;
								//
								ri = _FLOW_MAP_D_MODULE_E[i];
								_FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE_E[j];
								_FLOW_MAP_D_MODULE_E[j] = ri;
								//
							}
							//
						}
						//
					}
				}
			}
		}
	}
	//
	for ( i = (_FLOW_MAP_D_COUNT-1) ; i >= 0 ; i-- ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] >= 0 ) {
			_FLOW_MAP_D_COUNT = (i+1);
			break;
		}
		//
	}
	//
}

void _FLOW_MAP_SORT() {
	int i , j , ri , six;
	char rc;
	int chmap[MAX_CHAMBER] , chmapx[MAX_CHAMBER];
	//
	// group - module
	//
	for ( i = 0 ; i < (_FLOW_MAP_S_COUNT-1) ; i++ ) {
		for ( j = (i+1) ; j < _FLOW_MAP_S_COUNT ; j++ ) {
			//
			if ( _FLOW_MAP_S_GROUP[j] < 0 ) continue;
			//
			if ( ( _FLOW_MAP_S_GROUP[i] < 0 ) && ( _FLOW_MAP_S_GROUP[j] >= 0 ) ) {
				//
				rc = _FLOW_MAP_S_GROUP[i];
				_FLOW_MAP_S_GROUP[i] = _FLOW_MAP_S_GROUP[j];
				_FLOW_MAP_S_GROUP[j] = rc;
				//
				ri = _FLOW_MAP_S_MODULE[i];
				_FLOW_MAP_S_MODULE[i] = _FLOW_MAP_S_MODULE[j];
				_FLOW_MAP_S_MODULE[j] = ri;
				//
			}
			else if ( ( _FLOW_MAP_S_GROUP[i] >= 0 ) && ( _FLOW_MAP_S_GROUP[j] >= 0 ) ) {
				//
				if ( _FLOW_MAP_S_GROUP[i] > _FLOW_MAP_S_GROUP[j] ) {
					//
					rc = _FLOW_MAP_S_GROUP[i];
					_FLOW_MAP_S_GROUP[i] = _FLOW_MAP_S_GROUP[j];
					_FLOW_MAP_S_GROUP[j] = rc;
					//
					ri = _FLOW_MAP_S_MODULE[i];
					_FLOW_MAP_S_MODULE[i] = _FLOW_MAP_S_MODULE[j];
					_FLOW_MAP_S_MODULE[j] = ri;
					//
				}
				else if ( _FLOW_MAP_S_GROUP[i] == _FLOW_MAP_S_GROUP[j] ) {
					if ( _FLOW_MAP_S_MODULE[i] > _FLOW_MAP_S_MODULE[j] ) {
						//
						rc = _FLOW_MAP_S_GROUP[i];
						_FLOW_MAP_S_GROUP[i] = _FLOW_MAP_S_GROUP[j];
						_FLOW_MAP_S_GROUP[j] = rc;
						//
						ri = _FLOW_MAP_S_MODULE[i];
						_FLOW_MAP_S_MODULE[i] = _FLOW_MAP_S_MODULE[j];
						_FLOW_MAP_S_MODULE[j] = ri;
						//
					}
				}
			}
		}
	}
	//
	for ( i = (_FLOW_MAP_S_COUNT-1) ; i >= 0 ; i-- ) {
		//
		if ( _FLOW_MAP_S_GROUP[i] >= 0 ) {
			_FLOW_MAP_S_COUNT = (i+1);
			break;
		}
		//
	}
	//
	for ( i = 0 ; i < _FLOW_MAP_S_COUNT ; i++ ) _FLOW_MAP_S_MODULE_E[i] = _FLOW_MAP_S_MODULE[i];
	//
	six = 0;
	//
	for ( i = 1 ; i < _FLOW_MAP_S_COUNT ; i++ ) {
		if ( _FLOW_MAP_S_GROUP[i] == _FLOW_MAP_S_GROUP[six] ) {
			//
			if ( (_FLOW_MAP_S_MODULE_E[six]+1) == _FLOW_MAP_S_MODULE[i] ) {
				//
				_FLOW_MAP_S_GROUP[i] = -1;
				_FLOW_MAP_S_MODULE_E[six] = _FLOW_MAP_S_MODULE[i];
				//
			}
			else {
				six = i;
			}
			//
		}
		else {
			six = i;
		}
	}
	//
	//====================================================================================================================================
	//====================================================================================================================================
	//====================================================================================================================================
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) _FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE[i];
	//
	_FLOW_MAP_D_SORT();
	//
	//======================================================================
	//======================================================================
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) _FLOW_MAP_D_MODULE_E[i] = _FLOW_MAP_D_MODULE[i];
	//
	six = 0;
	//
	for ( i = 1 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] == _FLOW_MAP_D_GROUP[six] ) {
			if ( _FLOW_MAP_D_MODE[i] == _FLOW_MAP_D_MODE[six] ) {
				//
				if ( (_FLOW_MAP_D_MODULE_E[six]+1) == _FLOW_MAP_D_MODULE[i] ) {
					_FLOW_MAP_D_GROUP[i] = -1;
					_FLOW_MAP_D_MODULE_E[six] = _FLOW_MAP_D_MODULE[i];
				}
				else {
					six = i;
				}
				//
			}
			else {
				six = i;
			}
		}
		else {
			six = i;
		}
	}
	//
	//======================================================================
	//======================================================================
	//
	while ( TRUE ) {
		//
		six = 0;
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) chmap[i] = 0;

		for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
			//
			if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
			//
			if ( chmap[_FLOW_MAP_D_MODULE[i]] == 0 ) {
				chmap[_FLOW_MAP_D_MODULE[i]] = _FLOW_MAP_D_MODULE_E[i];
			}
			else {
				if ( chmap[_FLOW_MAP_D_MODULE[i]] > _FLOW_MAP_D_MODULE_E[i] ) {
					chmap[_FLOW_MAP_D_MODULE[i]] = _FLOW_MAP_D_MODULE_E[i];
				}
			}
		}
		//
		for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
			//
			if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[i] == _FLOW_MAP_D_MODULE_E[i] ) continue;
			//
			if ( chmap[_FLOW_MAP_D_MODULE[i]] == 0 ) continue;
			//
			if ( chmap[_FLOW_MAP_D_MODULE[i]] < _FLOW_MAP_D_MODULE_E[i] ) {
				//
				six = 1;
				//
				_FLOW_MAP_D_ADD_E( _FLOW_MAP_D_MODULE[i] , _FLOW_MAP_D_MODE[i] , _FLOW_MAP_D_GROUP[i] , chmap[_FLOW_MAP_D_MODULE[i]] );
				_FLOW_MAP_D_ADD_E( chmap[_FLOW_MAP_D_MODULE[i]] + 1 , _FLOW_MAP_D_MODE[i] , _FLOW_MAP_D_GROUP[i] , _FLOW_MAP_D_MODULE_E[i] );
				//
				_FLOW_MAP_D_GROUP[i] = -1;
				//
			}
			//
		}
		//
		if ( six == 0 ) break;
		//
		_FLOW_MAP_D_SORT();
		//
	}
	//
	//======================================================================
	//======================================================================
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT ; i++ ) {
		//
		_FLOW_MAP_D_MODULE_C[i] = 0;
		//
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
		//
		if ( _FLOW_MAP_D_MODULE[i] != _FLOW_MAP_D_MODULE_E[i] ) _FLOW_MAP_D_MODULE_C[i] = -1;
		//
	}
	//
	for ( i = 0 ; i < (_FLOW_MAP_D_COUNT-1) ; i++ ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
		//
		if ( _FLOW_MAP_D_MODULE_C[i] >= 0 ) continue;
		//
		if ( _FLOW_MAP_D_MODULE[i] == _FLOW_MAP_D_MODULE_E[i] ) continue;
		//
		//
		for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
			chmap[j] = 0;
			chmapx[j] = 0;
		}
		//
		//
		for ( j = i ; j < _FLOW_MAP_D_COUNT ; j++ ) {
			//
			if ( _FLOW_MAP_D_GROUP[j] < 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE_C[j] >= 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[j] == _FLOW_MAP_D_MODULE_E[j] ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[i] != _FLOW_MAP_D_MODULE[j] ) continue;
			//
			chmap[_FLOW_MAP_D_MODULE_E[j]]++;
			//
		}
		//
		six = 0;
		//
		for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
			if ( chmap[j] > 0 ) {
				//
				six++;
				//
				chmapx[j] = six;
				//
			}
		}
		//
		for ( j = i ; j < _FLOW_MAP_D_COUNT ; j++ ) {
			//
			if ( _FLOW_MAP_D_GROUP[j] < 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE_C[j] >= 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[j] == _FLOW_MAP_D_MODULE_E[j] ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[i] != _FLOW_MAP_D_MODULE[j] ) continue;
			//
			_FLOW_MAP_D_MODULE_C[j] = chmapx[_FLOW_MAP_D_MODULE_E[j]];
			//
		}
		//
	}
	//
}

int _FLOW_MAP_DATA_SD_MAKING() {
	int i , t , g , Res;
	//
	_FLOW_MAP_INIT();
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) != 1 ) continue;
		//
		g = _i_SCH_PRM_GET_MODULE_GROUP( i );
		//
		if ( g < 0 ) continue;
		if ( g >= MAX_TM_CHAMBER_DEPTH ) continue;
		//
		Res = _FLOW_MAP_S_USING( i , (char) g + _FLOW_HAS_GRP0_FM );	if ( Res == 0 ) return 1;
		//
	}
	//
	for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) != 1 ) continue;
		//
		g = _i_SCH_PRM_GET_MODULE_GROUP( i );
		//
		if ( g < 0 ) continue;
		if ( g >= MAX_TM_CHAMBER_DEPTH ) continue;
		//
		for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
			//
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( t , i ) ) {
			case -1 :
				break;
			case 0 : // x
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , (char) t + _FLOW_HAS_GRP0_FM , FALSE );	if ( Res == 0 ) return 1;
				//
				break;
			case 1 : // o
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , (char) t + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
				//
				break;
			}
			//
			switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( t , i ) ) {
			case -1 :
				break;
			case 0 : // x
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) t + _FLOW_HAS_GRP0_FM , FALSE );	if ( Res == 0 ) return 1;
				//
				break;
			case 1 : // o
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) t + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
				//
				break;
			}
			//
		}
		//
	}
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) != 1 ) continue;
		//
		g = _i_SCH_PRM_GET_MODULE_GROUP( i );
		//
		if ( g < 0 ) continue;
		if ( g >= MAX_TM_CHAMBER_DEPTH ) continue;
		//
		switch( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( g , i ) ) {
		case BUFFER_IN_MODE : // g-1 > g >
			//
			Res = _FLOW_MAP_D_USING( i , MACRO_PICK  , (char) g + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
			//
			if ( ( g + _FLOW_HAS_GRP0_FM ) > 0 ) {
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) ( g + _FLOW_HAS_GRP0_FM - 1 ) , TRUE );	if ( Res == 0 ) return 1;
				//
			}
			//
			break;
		case BUFFER_OUT_MODE :
			//
			Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) g + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
			//
			if ( ( g + _FLOW_HAS_GRP0_FM ) > 0 ) {
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , (char) ( g + _FLOW_HAS_GRP0_FM - 1 ) , TRUE );	if ( Res == 0 ) return 1;
				//
			}
			//
			break;
		case BUFFER_SWITCH_MODE :
			//
			Res = _FLOW_MAP_D_USING( i , MACRO_PICK  , (char) g + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
			Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) g + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
			//
			if ( ( g + _FLOW_HAS_GRP0_FM ) > 0 ) {
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK  , (char) ( g + _FLOW_HAS_GRP0_FM - 1 ) , TRUE );	if ( Res == 0 ) return 1;
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) ( g + _FLOW_HAS_GRP0_FM - 1 ) , TRUE );	if ( Res == 0 ) return 1;
				//
			}
			//
			break;
		case BUFFER_IN_S_MODE :
			//
			Res = _FLOW_MAP_S_USING( i , (char) g + _FLOW_HAS_GRP0_FM );	if ( Res == 0 ) return 1;
			//
			break;
		case BUFFER_OUT_S_MODE :
			//
			Res = _FLOW_MAP_S_USING( i , (char) g + _FLOW_HAS_GRP0_FM );	if ( Res == 0 ) return 1;
			//
			break;
		}
		//
		if ( _FLOW_HAS_GRP0_FM == 1 ) {
			//
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) ) {
			case -1 :
				break;
			case 0 : // x
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , 0 , FALSE );	if ( Res == 0 ) return 1;
				//
				break;
			case 1 : // o
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , 0 , TRUE );	if ( Res == 0 ) return 1;
				//
				break;
			}
			//
			switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) ) {
			case -1 :
				break;
			case 0 : // x
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , 0 , FALSE );	if ( Res == 0 ) return 1;
				//
				break;
			case 1 : // o
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , 0 , TRUE );	if ( Res == 0 ) return 1;
				//
				break;
			}
			//
		}
		//
		for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
			//
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( t , i ) ) {
			case -1 :
				break;
			case 0 : // x
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , (char) t + _FLOW_HAS_GRP0_FM , FALSE );	if ( Res == 0 ) return 1;
				//
				break;
			case 1 : // o
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PICK , (char) t + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
				//
				break;
			}
			//
			switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( t , i ) ) {
			case -1 :
				break;
			case 0 : // x
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) t + _FLOW_HAS_GRP0_FM , FALSE );	if ( Res == 0 ) return 1;
				//
				break;
			case 1 : // o
				//
				Res = _FLOW_MAP_D_USING( i , MACRO_PLACE , (char) t + _FLOW_HAS_GRP0_FM , TRUE );	if ( Res == 0 ) return 1;
				//
				break;
			}
			//
		}
		//
	}
	//
	_FLOW_MAP_SORT();
	//
	return 0;
}





void _FLOW_MAP_PRINTF_DATA_SD( FILE *fpt ) {
	int i;
	char namestr[64];
	//
	for ( i = 0 ; i < _FLOW_MAP_S_COUNT  ; i++ ) {
		//
		if ( _FLOW_MAP_S_GROUP[i] < 0 ) continue;
		//
		if ( _FLOW_MAP_S_MODULE[i] == _FLOW_MAP_S_MODULE_E[i] ) {
			if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[i] ) ) ) {
				sprintf( namestr , "[%7s %7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , "" );
			}
			else {
				sprintf( namestr , "[%7s:%7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[i] ) );
			}
		}
		else {
			if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[i] ) ) ) {
				if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE_E[i] ) ) ) {
					sprintf( namestr , "[%7s %7s]~[%7s %7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , "" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[i] ) , "" );
				}
				else {
					sprintf( namestr , "[%7s %7s]~[%7s:%7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , "" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE_E[i] ) );
				}
			}
			else {
				if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE_E[i] ) ) ) {
					sprintf( namestr , "[%7s:%7s]~[%7s %7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[i] ) , "" );
				}
				else {
					sprintf( namestr , "[%7s:%7s]~[%7s:%7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE_E[i] ) );
				}
			}
		}
		//
		fprintf( fpt , "S	[%4d]	[%4d]	%s\n" , i , _FLOW_MAP_S_GROUP[i] , namestr );
		//
		//
	}
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT  ; i++ ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
		//
		if ( _FLOW_MAP_D_MODULE[i] == _FLOW_MAP_D_MODULE_E[i] ) {
			if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) ) ) {
				sprintf( namestr , "[%7s %7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , "" );
			}
			else {
				sprintf( namestr , "[%7s:%7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) );
			}
		}
		else {
			if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) ) ) {
				if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE_E[i] ) ) ) {
					sprintf( namestr , "[%7s %7s]~[%7s %7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , "" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) , "" );
				}
				else {
					sprintf( namestr , "[%7s %7s]~[%7s:%7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , "" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE_E[i] ) );
				}
			}
			else {
				if ( STRCMP_L( _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE_E[i] ) ) ) {
					sprintf( namestr , "[%7s:%7s]~[%7s %7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) , "" );
				}
				else {
					sprintf( namestr , "[%7s:%7s]~[%7s:%7s]" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE_E[i] ) );
				}
			}
		}
		//
		fprintf( fpt , "D	[%4d]	%4d	%5s	%s" , i , _FLOW_MAP_D_GROUP[i] , ( _FLOW_MAP_D_MODE[i] == 0 ) ? "PICK" : "PLACE" , namestr );
		//
		if ( _FLOW_MAP_D_MODULE_C[i] > 0 ) {
			fprintf( fpt , "	[%c]\n" , _FLOW_MAP_D_MODULE_C[i] - 1 + 'a' );
		}
		else {
			fprintf( fpt , "\n" );
		}
		//
	}
	//
	fprintf( fpt , "\n" );
	//
}


#define MAX_FLOW_MAP_BUF	1024

int  pcount , maxt , pclast;
int  module[MAX_FLOW_MAP_BUF];
char module_c[MAX_FLOW_MAP_BUF];
char placing[MAX_FLOW_MAP_BUF][MAX_TM_CHAMBER_DEPTH+2];
char picking[MAX_FLOW_MAP_BUF][MAX_TM_CHAMBER_DEPTH+2];

void _FLOW_MAP_CHART_DRAW_INIT( FILE *fpt , int namemode , BOOL mrtrn ) {
	int i , t , x , ss;
	char tms[64];
	//
	pclast = -1;
	//
	for ( x = 0 ; x < 3 ; x++ ) {
		//
		ss = -1;
		//
		for ( i = 0 ; i <= maxt ; i++ ) {
			//
			if ( ss != -1 ) fprintf( fpt , "%13s" , "" );
			//
			for ( t = (ss+1) ; t < i ; t++ ) fprintf( fpt , "%20s" , "" );
			//
			ss = i;
			//
			if ( x == 1 ) {
				//
				if ( _FLOW_HAS_GRP0_FM == 0 ) {
					if ( _i_SCH_PRM_GET_MODULE_MODE( TM + i ) ) {
						sprintf( tms , "TM%d" , i + 1 );
					}
					else {
						sprintf( tms , "T%d-X" , i + 1 );
					}
				}
				else {
					if ( i == 0 ) {
						if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
							strcpy( tms , "FM" );
						}
						else {
							strcpy( tms , "FM-X" );
						}
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_MODE( TM + i - 1 ) ) {
							sprintf( tms , "TM%d" , i );
						}
						else {
							sprintf( tms , "T%d-X" , i );
						}
					}
				}
				//
				fprintf( fpt , "|%5s|" , tms );
				//
			}
			else {
				fprintf( fpt , "-------" );
			}
			//
			//
		}
		//
		fprintf( fpt , "\n" );
		//
	}
	//
	if ( mrtrn ) fprintf( fpt , "\n" );
	//
}

void _FLOW_MAP_CHART_DRAW_START( FILE *fpt , int namemode , char dir , char grpplace , int mdl , char mdl_c , char grppick ) {
	//
	char tmplace[64];
	char tmpick[64];
	char namestr[64];
	int i;
	//
	if ( _FLOW_HAS_GRP0_FM == 0 ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( TM + grpplace ) ) {
			sprintf( tmplace , "(TM%d)" , grpplace + 1 );
		}
		else {
			sprintf( tmplace , "(T%d-X)" , grpplace + 1 );
		}
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( TM + grppick ) ) {
			sprintf( tmpick  , "(TM%d)" , grppick + 1 );
		}
		else {
			sprintf( tmpick  , "(T%d-X)" , grppick + 1 );
		}
		//
	}
	else {
		//
		if ( grpplace <= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				strcpy( tmplace , "(FM)" );
			}
			else {
				strcpy( tmplace , "(F-X)" );
			}
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( TM + grpplace - 1 ) ) {
				sprintf( tmplace , "(TM%d)" , grpplace );
			}
			else {
				sprintf( tmplace , "(T%d-X)" , grpplace );
			}
		}
		if ( grppick <= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
				strcpy( tmpick , "(FM)" );
			}
			else {
				strcpy( tmpick , "(F-X)" );
			}
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MODE( TM + grppick - 1 ) ) {
				sprintf( tmpick  , "(TM%d)" , grppick );
			}
			else {
				sprintf( tmpick  , "(T%d-X)" , grppick );
			}
		}
		//
	}
	//
	if ( namemode == 0 ) {
		strcpy( namestr , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) );
	}
	else {
		strcpy( namestr , _i_SCH_SYSTEM_GET_MODULE_NAME( mdl ) );
	}
	//
	if ( grppick < 0 ) {
		//
		if ( pclast == grpplace ) {
			//
			if ( mdl_c == 0 ) {
				fprintf( fpt , " %c %7s -\n" , dir , namestr );
			}
			else {
				fprintf( fpt , " %c %5s:%c -\n" , dir , namestr , mdl_c - 1 + 'a' );
			}
			//
		}
		else {
			//
			if ( pclast != -1 ) fprintf( fpt , "\n" );
			//
			for ( i = 0 ; i < grpplace ; i++ ) fprintf( fpt , "%20s" , "" );
			//
			if ( mdl_c == 0 ) {
				fprintf( fpt , "%7s %c %7s -\n" , tmplace , dir , namestr );
			}
			else {
				fprintf( fpt , "%7s %c %5s:%c -\n" , tmplace , dir , namestr , mdl_c - 1 + 'a' );
			}
			//
		}
		//
		pclast = -1;
		//
	}
	else {
		//
		if ( pclast == grpplace ) {
			//
			if ( mdl_c == 0 ) {
				fprintf( fpt , " %c %7s" , dir , namestr );
			}
			else {
				fprintf( fpt , " %c %5s:%c" , dir , namestr , mdl_c - 1 + 'a' );
			}
			//
			for ( i = ( grpplace + 1 ) ; i < grppick ; i++ ) fprintf( fpt , "%20s" , "" );
			//
			fprintf( fpt , " %c %7s" , dir , tmpick );
			//
		}
		else {
			//
			if ( pclast != -1 ) fprintf( fpt , "\n" );
			//
			for ( i = 0 ; i < grpplace ; i++ ) fprintf( fpt , "%20s" , "" );
			//
			if ( mdl_c == 0 ) {
				fprintf( fpt , "%7s %c %7s" , tmplace , dir , namestr );
			}
			else {
				fprintf( fpt , "%7s %c %5s:%c" , tmplace , dir , namestr , mdl_c - 1 + 'a' );
			}
			//
			for ( i = ( grpplace + 1 ) ; i < grppick ; i++ ) fprintf( fpt , "%20s" , "" );
			//
			fprintf( fpt , " %c %7s" , dir , tmpick );
			//
		}
		//
		pclast = grppick;
		//
	}
	//
}


void _FLOW_MAP_CHART_DRAW_END( FILE *fpt ) {
	fprintf( fpt , "\n" );
	pclast = -1;
}

void _FLOW_MAP_CHART_DRAW_SD( FILE *fpt , int namemode ) {
	int i , j , t , f , ss;
	int use[MAX_FLOW_MAP_COUNT];
	int tx[MAX_TM_CHAMBER_DEPTH+1];
	//
	for ( i = 0 ; i < MAX_FLOW_MAP_COUNT  ; i++ ) use[i] = 0;
	//
	fprintf( fpt , "\n" );
	//
	_FLOW_MAP_CHART_DRAW_INIT( fpt , 0 , FALSE );
	//
	for ( i = 0 ; i < ( MAX_TM_CHAMBER_DEPTH + 1 ) ; i++ ) tx[i] = 0;
	//
	for ( i = 0 ; i < _FLOW_MAP_S_COUNT  ; i++ ) {
		//
		if ( _FLOW_MAP_S_GROUP[i] < 0 ) continue;
		//
		tx[_FLOW_MAP_S_GROUP[i]] = 1;
		//
	}
	//
	ss = -1;
	//
	for ( i = 0 ; i < ( MAX_TM_CHAMBER_DEPTH + _FLOW_HAS_GRP0_FM ) ; i++ ) {
		//
		if ( tx[i] == 0 ) continue;
		//
		if ( ss != -1 ) fprintf( fpt , "%13s" , "" );
		//
		for ( t = (ss+1) ; t < i ; t++ ) fprintf( fpt , "%20s" , "" );
		//
		ss = i;
		//
		fprintf( fpt , "%7s" , "  |  " );
		//
	}
	//
	fprintf( fpt , "\n\n" );
	//
	//
	while ( TRUE ) {
		//
		f = 0;
		//
		ss = -1;
		//
//  0               1               2               3
//    TM1 >   BM5 >   TM2 >   BM8 >   TM3 >  PM11 >   TM4 >   BM9 >   TM5 >  BM10 >   TM6
//  +++++@@@@@@@@@@@+++++@@@@@@@@@@@+++++@@@@@@@@@@@+++++@@@@@@@@@@@+++++@@@@@@@@@@@+++++
//
//  +++++@@@@@@@@@@@
//                  +++++
//                       @@@@@@@@@@@+++++@@@@@@@@@@@
//                                                  +++++
//                                                       @@@@@@@@@@@
//                                                                  +++++
//  +++++@@@@@@@@@@@+++++@@@@@@@@@@@



		for ( i = 0 ; i < ( MAX_TM_CHAMBER_DEPTH + _FLOW_HAS_GRP0_FM ) ; i++ ) {
			//
			for ( j = 0 ; j < _FLOW_MAP_S_COUNT  ; j++ ) {
				//
				if ( _FLOW_MAP_S_GROUP[j] != i ) continue;
				//
				if ( use[j] == 2 ) continue;
				//
				if ( ss != -1 ) fprintf( fpt , "%13s" , "" );
				//
				for ( t = (ss+1) ; t < i ; t++ ) fprintf( fpt , "%20s" , "" );
				//
				ss = i;
				//
				if ( namemode == 0 ) {
					if ( _FLOW_MAP_S_MODULE[j] == _FLOW_MAP_S_MODULE_E[j] ) {
						fprintf( fpt , "%7s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[j] ) );
						use[j] = 2;
					}
					else {
						if ( use[j] == 0 ) {
							fprintf( fpt , "%7s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE[j] ) );
							use[j] = 1;
						}
						else {
							fprintf( fpt , "~%6s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_S_MODULE_E[j] ) );
							use[j] = 2;
						}
					}
				}
				else {
					if ( _FLOW_MAP_S_MODULE[j] == _FLOW_MAP_S_MODULE_E[j] ) {
						fprintf( fpt , "%7s" , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[j] ) );
						use[j] = 2;
					}
					else {
						if ( use[j] == 0 ) {
							fprintf( fpt , "%7s" , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE[j] ) );
							use[j] = 1;
						}
						else {
							fprintf( fpt , "~%6s" , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_S_MODULE_E[j] ) );
							use[j] = 2;
						}
					}
				}
				//
				f = 1;
				//
				break;
				//
			}
			//
		}
		//
		if ( f == 0 ) break;
		//
		fprintf( fpt , "\n" );
		//
	}
	//
	fprintf( fpt , "\n" );
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT  ; i++ ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
		//
		if ( _FLOW_MAP_D_MODULE[i] == _FLOW_MAP_D_MODULE_E[i] ) continue;
		if ( _FLOW_MAP_D_MODULE_C[i] <= 0 ) continue;
		//
		for ( j = 0 ; j < i ; j++ ) {
			//
			if ( _FLOW_MAP_D_GROUP[j] < 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[j] == _FLOW_MAP_D_MODULE_E[j] ) continue;
			if ( _FLOW_MAP_D_MODULE_C[j] <= 0 ) continue;
			//
			if ( _FLOW_MAP_D_MODULE[i] == _FLOW_MAP_D_MODULE[j] ) {
				if ( _FLOW_MAP_D_MODULE_E[i] == _FLOW_MAP_D_MODULE_E[j] ) {
					if ( _FLOW_MAP_D_MODULE_C[i] == _FLOW_MAP_D_MODULE_C[j] ) {
						break;
					}
				}
			}
		}
		//
		if ( j == i ) {
			if ( namemode == 0 ) {
				fprintf( fpt , ">> [%5s:%c] = [%s]~[%6s]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _FLOW_MAP_D_MODULE_C[i] -1 + 'a' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( _FLOW_MAP_D_MODULE_E[i] ) );
			}
			else {
				fprintf( fpt , ">> [%5s:%c] = [%s]~[%6s]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) , _FLOW_MAP_D_MODULE_C[i] -1 + 'a' , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE[i] ) , _i_SCH_SYSTEM_GET_MODULE_NAME( _FLOW_MAP_D_MODULE_E[i] ) );
			}
		}
		//
	}
	//
}

int _FLOW_MAP_CHART_DRAW_RUN( FILE *fpt , int namemode , char grp0 , int mode , BOOL subcall ) { // mode 0:plc<pic 1:plc>pic 2:plc-?    return 0:No  1:ok_endx 2:ok_endO
	int i , j , f , dg;
	char grp;
	int run;
	//
	run = 0;
	//
	if ( grp0 < 0 ) {
		//
		grp = -1;
		//
		for ( i = 0 ; i < pcount ; i++ ) {
			//
			if ( mode == 1 ) {
				//
				for ( j = 0 ; j <= maxt ; j++ ) {
					//
					if ( ( picking[i][j] > 0 ) && ( picking[i][j] < 10 ) ) {
						//
						if ( grp == -1 ) {
							grp = j;
						}
						else {
							if ( grp > j ) grp = j;
						}
						//
						break;
						//
					}
				}
			}
			else {
				//
				for ( j = 0 ; j <= maxt ; j++ ) {
					//
					if ( ( placing[i][j] > 0 ) && ( placing[i][j] < 10 ) ) {
						//
						if ( grp == -1 ) {
							grp = j;
						}
						else {
							if ( grp > j ) grp = j;
						}
						//
						break;
						//
					}
				}
			}
			//
		}
		//
		if ( grp == -1 ) return 0;
		//
	}
	else {
		grp = grp0;
	}
	//
	if ( mode == 1 ) {
		//
		for ( i = 0 ; i < pcount ; i++ ) {
			//
			if ( ( picking[i][grp] <= 0 ) || ( picking[i][grp] >= 10 ) ) continue;
			//
			f = 0;
			//
			for ( j = 0 ; j <= maxt ; j++ ) {
				//
				if ( grp == j ) continue;
				//
				if ( ( placing[i][j] > 0 ) && ( placing[i][j] < 10 ) ) {
					//
					if ( grp > j ) continue;
					//
					picking[i][grp] += 10;
					placing[i][j] += 10;
					//
					_FLOW_MAP_CHART_DRAW_START( fpt , namemode , '<' , grp , module[i] , module_c[i] , j );
					//
					switch( _FLOW_MAP_CHART_DRAW_RUN( fpt , namemode , j , mode , TRUE ) ) {
					case 1 :
						run = 1;
						break;
					case 2 :
						run = 2;
						break;
					default :
						run = 1;
						break;
					}
					//
					if ( !subcall ) {
						if ( run == 1 ) {
							run = 2;
							_FLOW_MAP_CHART_DRAW_END( fpt );
						}
					}
					//
					f = 1;
					//
				}
			}
			//
			//
			if ( f == 0 ) {
				//
				for ( j = 0 ; j <= maxt ; j++ ) {
					//
					if ( grp == j ) continue;
					//
					if ( grp > j ) continue;
					//
					if ( placing[i][j] >= 10 ) {
						//
						picking[i][grp] += 10;
						//
						_FLOW_MAP_CHART_DRAW_START( fpt , namemode , '<' , grp , module[i] , module_c[i] , j );
						//
						f = 1;
						//
						run = 2;
						//
						break;
					}
				}
				//
			}
			//
		}
	}
	else {
		for ( i = 0 ; i < pcount ; i++ ) {
			//
			if ( ( placing[i][grp] <= 0 ) || ( placing[i][grp] >= 10 ) ) continue;
			//
			f = 0;
			//
			dg = 0;
			//
			for ( j = 0 ; j <= maxt ; j++ ) {
				//
				if ( grp == j ) continue;
				//
				if ( ( picking[i][j] > 0 ) && ( picking[i][j] < 10 ) ) {
					//
					if ( mode == 0 ) {
						if ( grp > j ) {
							dg = 1;
							continue;
						}
					}
					else {
						f = 1;
						continue;
					}
					//
					placing[i][grp] += 10;
					picking[i][j] += 10;
					//
					_FLOW_MAP_CHART_DRAW_START( fpt , namemode , '>' , grp , module[i] , module_c[i] , j );
					//
					switch( _FLOW_MAP_CHART_DRAW_RUN( fpt , namemode , j , mode , TRUE ) ) {
					case 1 :
						run = 1;
						break;
					case 2 :
						run = 2;
						break;
					default :
						run = 1;
						break;
					}
					//
					if ( !subcall ) {
						if ( run == 1 ) {
							run = 2;
							_FLOW_MAP_CHART_DRAW_END( fpt );
						}
					}
					//
					f = 1;
					//
				}
			}
			//
			if ( f == 0 ) {
				//
				if ( mode == 0 ) {
					//
					if ( dg == 0 ) {
						//
						for ( j = 0 ; j <= maxt ; j++ ) {
							//
							if ( grp == j ) continue;
							//
							if ( grp > j ) continue;
							//
							if ( picking[i][j] >= 10 ) {
								//
								placing[i][grp] += 10;
								//
								_FLOW_MAP_CHART_DRAW_START( fpt , namemode , '>' , grp , module[i] , module_c[i] , j );
								//
								f = 1;
								//
								run = 2;
								//
								break;
							}
						}
						//
					}
					//
				}
				else {
					//
					placing[i][grp] += 10;
					//
					for ( j = 0 ; j <= maxt ; j++ ) {
						//
						if ( grp == j ) continue;
						//
						if ( picking[i][j] >= 10 ) {
							//
							_FLOW_MAP_CHART_DRAW_START( fpt , namemode , '+' , grp , module[i] , module_c[i] , j );
							//
							f = 1;
							//
						}
					}
					//
					if ( f == 0 ) {
						_FLOW_MAP_CHART_DRAW_START( fpt , namemode , '+' , grp , module[i] , module_c[i] , -1 );
					}
					//
					run = 2;
				}
			}
			//
		}
	}
	//
	return run;
	//
}

void _FLOW_MAP_DATA_CHART_MAKING( FILE *fpt , int namemode ) {
	//
	int i , j;
	//
	pcount = 0;
	//
	maxt = 0;
	//
	for ( i = 0 ; i < _FLOW_MAP_D_COUNT  ; i++ ) {
		//
		if ( _FLOW_MAP_D_GROUP[i] < 0 ) continue;
		//
		for ( j = 0 ; j < pcount ; j++ ) {
			if ( module[j] == _FLOW_MAP_D_MODULE[i] ) {
				if ( module_c[j] == _FLOW_MAP_D_MODULE_C[i] ) {
					break;
				}
			}
		}
		//
		if ( j == pcount ) { // add
			//
			if ( pcount >= MAX_FLOW_MAP_BUF ) break;
			//
			module[pcount] = _FLOW_MAP_D_MODULE[i];
			module_c[pcount] = _FLOW_MAP_D_MODULE_C[i];
			//
			for ( j = 0 ; j < ( MAX_TM_CHAMBER_DEPTH + 2 ) ; j++ ) {
				placing[pcount][j] = 0;
				picking[pcount][j] = 0;
			}
			//
			if ( _FLOW_MAP_D_MODE[i] == 0 ) {
				picking[pcount][_FLOW_MAP_D_GROUP[i]] = 1;
				picking[pcount][MAX_TM_CHAMBER_DEPTH+1] = 1;
			}
			else {
				placing[pcount][_FLOW_MAP_D_GROUP[i]] = 1;
				placing[pcount][MAX_TM_CHAMBER_DEPTH+1] = 1;
			}
			//
			pcount++;
			//
		}
		else { // update
			//
			if ( _FLOW_MAP_D_MODE[i] == 0 ) {
				picking[j][_FLOW_MAP_D_GROUP[i]] = 1;
				picking[j][MAX_TM_CHAMBER_DEPTH+1] = 1;
			}
			else {
				placing[j][_FLOW_MAP_D_GROUP[i]] = 1;
				placing[j][MAX_TM_CHAMBER_DEPTH+1] = 1;
			}
			//
		}
		//
		if ( _FLOW_MAP_D_GROUP[i] > maxt ) maxt = _FLOW_MAP_D_GROUP[i];
		//
	}
	//
	for ( i = 0 ; i < pcount  ; i++ ) {
		//
		if ( ( picking[i][MAX_TM_CHAMBER_DEPTH+1] == 0 ) || ( placing[i][MAX_TM_CHAMBER_DEPTH+1] == 0 ) ) {
			//
			for ( j = 0 ; j < _FLOW_MAP_S_COUNT  ; j++ ) {
				//
				if ( _FLOW_MAP_S_GROUP[j] < 0 ) continue;
				//
				if ( ( _FLOW_MAP_S_MODULE[j] <= module[i] ) && ( _FLOW_MAP_S_MODULE_E[j] >= module[i] ) ) {
					break;
				}
				//
			}
			//
			if ( j != _FLOW_MAP_S_COUNT ) {
				//
				if ( picking[i][MAX_TM_CHAMBER_DEPTH+1] == 0 ) {
					picking[i][_FLOW_MAP_S_GROUP[j]] = 1;
					picking[i][MAX_TM_CHAMBER_DEPTH+1] = 2;
				}
				if ( placing[i][MAX_TM_CHAMBER_DEPTH+1] == 0 ) {
					placing[i][_FLOW_MAP_S_GROUP[j]] = 1;
					placing[i][MAX_TM_CHAMBER_DEPTH+1] = 2;
				}
				//
				if ( _FLOW_MAP_S_GROUP[j] > maxt ) maxt = _FLOW_MAP_S_GROUP[j];
				//
			}
		}
		//
	}
	//
	//
	fprintf( fpt , "-	------	" );
	//
	if ( _FLOW_HAS_GRP0_FM == 0 ) {
		for ( j = 0 ; j <= maxt ; j++ ) fprintf( fpt , "%c" , j + '0' );
	}
	else {
		fprintf( fpt , "F" );
		for ( j = 1 ; j <= maxt ; j++ ) fprintf( fpt , "%c" , j - 1 + '0' );
	}
	//
	fprintf( fpt , "	[-------]	" );
	//
	if ( _FLOW_HAS_GRP0_FM == 0 ) {
		for ( j = 0 ; j <= maxt ; j++ ) fprintf( fpt , "%c" , j + '0' );
	}
	else {
		fprintf( fpt , "F" );
		for ( j = 1 ; j <= maxt ; j++ ) fprintf( fpt , "%c" , j - 1 + '0' );
	}
	//
	fprintf( fpt , "\n\n" );
	//
	for ( i = 0 ; i < pcount  ; i++ ) {
		//
		fprintf( fpt , "X	[%4d]	" , i );
		//
		for ( j = 0 ; j <= maxt ; j++ ) fprintf( fpt , "%c" , ( placing[i][j] == 0 ) ? ' ' : 'O' );
		//
		//
		if ( namemode == 0 ) {
			if ( module_c[i] <= 0 ) {
				fprintf( fpt , "	[%7s]	" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module[i] ) );
			}
			else {
				fprintf( fpt , "	[%5s:%c]	" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( module[i] ) , module_c[i] - 1 + 'a' );
			}
		}
		else {
			if ( module_c[i] <= 0 ) {
				fprintf( fpt , "	[%7s]	" , _i_SCH_SYSTEM_GET_MODULE_NAME( module[i] ) );
			}
			else {
				fprintf( fpt , "	[%5s:%c]	" , _i_SCH_SYSTEM_GET_MODULE_NAME( module[i] ) , module_c[i] - 1 + 'a' );
			}
		}
		//
		for ( j = 0 ; j <= maxt ; j++ ) fprintf( fpt , "%c" , ( picking[i][j] == 0 ) ? ' ' : 'O' );
		//
		fprintf( fpt , "\n" );
	}
	//
	fprintf( fpt , "\n" );
	//
}
//



void _FLOW_MAP_DATA_DEFAULT( FILE *fpt ) {
	int i , j;
	SYSTEMTIME		SysTime;

	GetLocalTime( &SysTime );

	fprintf( fpt , "TIME\t%04d/%02d/%02d %02d:%02d:%02d\n\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond );
	//
	fprintf( fpt , "[---] [-----][----------] [GRP]\n" );
	//
	for ( i = CM1 ; i < MDL_LAST ; i++ ) {
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( i ) != 1 ) continue;
		//
		fprintf( fpt , "[%3d] [%5s:%10s] [%3d]\n" , i , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_SYSTEM_GET_MODULE_NAME( i ) , _i_SCH_PRM_GET_MODULE_GROUP( i ) );
		//
	}
	//
	fprintf( fpt , "\n" );
	//
	//-----------------------------------------------------------------------------------------------------------------------
	//
	for ( j = 0 ; j < MAX_TM_CHAMBER_DEPTH ; j++ ) {
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) != -1 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			//
			fprintf( fpt , " GROUP[%2d] PICK  " , j );
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) ) {
				case -1 :
					break;
				case 0 :
					fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				case 1 :
					fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PICK_GROUP( j , i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) != -1 ) break;
		}
		if ( i != MAX_CHAMBER ) {
			//
			fprintf( fpt , " GROUP[%2d] PLACE " , j );
			//
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
				switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) ) {
				case -1 :
					break;
				case 0 :
					fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				case 1 :
					fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
					break;
				default :
					fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( j , i ) );
					break;
				}
			}
			fprintf( fpt , "\n" );
		}
		//
	}
	//-----------------------------------------------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) != -1 ) break;
	}
	if ( i != MAX_CHAMBER ) {
		fprintf( fpt , " GROUP[FM] PICK  " );
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) ) {
			case -1 :
				break;
			case 0 :
				fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
				break;
			case 1 :
				fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
				break;
			default :
				fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH , i ) );
				break;
			}
		}
		fprintf( fpt , "\n" );
	}
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) != -1 ) break;
	}
	if ( i != MAX_CHAMBER ) {
		fprintf( fpt , " GROUP[FM] PLACE " );
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) ) {
			case -1 :
				break;
			case 0 :
				fprintf( fpt , "(%s=X)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
				break;
			case 1 :
				fprintf( fpt , "(%s=O)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
				break;
			default :
				fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH , i ) );
				break;
			}
		}
		fprintf( fpt , "\n" );
	}
	//-----------------------------------------------------------------------------------------------------------------------
	for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) > 0 ) break;
	}
	if ( i != MAX_CHAMBER ) {
		fprintf( fpt , " GROUP-CNT PICK  " );
		//
		for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
			switch( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) {
			case -1 :
				break;
			default :
				fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) );
				break;
			}
		}
		fprintf( fpt , "\n" );
	}
	//
	for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) > 0 ) break;
	}
	if ( i != MAX_CHAMBER ) {
		fprintf( fpt , " GROUP-CNT PLACE " );
		//
		for ( i = 1 ; i < MAX_CHAMBER ; i++ ) {
			switch( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) ) {
			case -1 :
				break;
			default :
				fprintf( fpt , "(%s=%d)" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) , _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , i ) );
				break;
			}
		}
		fprintf( fpt , "\n" );
	}
	//-----------------------------------------------------------------------------------------------------------------------
	fprintf( fpt , "\n" );
	//-----------------------------------------------------------------------------------------------------------------------
}


void _FLOW_MAP_CHART() { // MODULE_FLOW_MAP_CHART
	int Res;
	FILE *fpt;
	char filename[256];
	char head[64];
	//
	_IO_GET_SYSTEM_FOLDER( head , 63 );
	sprintf( filename , "%s/mdlmap.log" , head );
	//
	fpt = fopen( filename , "w" );
	//
	if ( fpt == NULL ) {
		printf( "\nMODULE_FLOW_MAP_CHART Fail\n" );
		return;
	}
	//
	printf( "MODULE_FLOW_MAP_CHART START\n" );
	//
	fprintf( fpt , "MODULE_FLOW_MAP_CHART\n\n" );
	//
	_FLOW_MAP_DATA_DEFAULT( fpt );
	//
	Res = _FLOW_MAP_DATA_SD_MAKING();
	//
	if ( Res != 0 ) fprintf( fpt , "_FLOW_MAP_DATA_SD_MAKING() has a error (%d)\n\n" , Res );
	//
	_FLOW_MAP_PRINTF_DATA_SD( fpt );
	//
	//
	//
	//
	//
	fprintf( fpt , "\n" );
	//
	for ( Res = 0 ; Res < 2 ; Res++ ) {
		//
		fprintf( fpt , "####################################################################################################################################################\n" );
		fprintf( fpt , "####################################################################################################################################################\n\n\n" );
		//
		_FLOW_MAP_DATA_CHART_MAKING( fpt , Res );
		//
		_FLOW_MAP_CHART_DRAW_INIT( fpt , Res , TRUE );
		//
		while ( TRUE ) {
			if ( 0 == _FLOW_MAP_CHART_DRAW_RUN( fpt , Res , -1 , 0 , FALSE ) ) break;
		}
		while ( TRUE ) {
			if ( 0 == _FLOW_MAP_CHART_DRAW_RUN( fpt , Res , -1 , 1 , FALSE ) ) break;
		}
		while ( TRUE ) {
			if ( 0 == _FLOW_MAP_CHART_DRAW_RUN( fpt , Res , -1 , 2 , FALSE ) ) break;
		}
		//
		_FLOW_MAP_CHART_DRAW_SD( fpt , Res );
		//
		fprintf( fpt , "\n\n\n" );
		//
	}
	//
	//
	fclose( fpt );
	//
	printf( "MODULE_FLOW_MAP_CHART END\n" );
}





