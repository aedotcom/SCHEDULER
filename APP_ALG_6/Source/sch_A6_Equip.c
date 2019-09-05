//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_Equip_Handling.h"
#include "INF_sch_macro.h"
#include "INF_User_Parameter.h"
//================================================================================



//-----------------------------------------------------------------------
void ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE_STYLE_6( int ch , BOOL *update , int mode , int tms ) {
	int wsrc1 , wsts1 , wres1 , wsts , wr;
	//-----------------------------------------------------------------------
	*update = FALSE;
	//-----------------------------------------------------------------------
	wsrc1 = _SCH_IO_GET_MODULE_SOURCE( 20000 + 0 , TA_STATION );
	wsts1 = _SCH_IO_GET_MODULE_STATUS( 20000 + 0 , TA_STATION );
	wres1 = _SCH_IO_GET_MODULE_RESULT( 20000 + 0 , TA_STATION ); // 2004.01.05
	//-----------------------------------------------------------------------
	wsts = _SCH_IO_GET_MODULE_STATUS( 20000 + tms , TB_STATION );
	if ( wsts > 0 ) {
		_SCH_IO_SET_MODULE_SOURCE( 20000 + 0 , TA_STATION , _SCH_IO_GET_MODULE_SOURCE( 20000 + tms , TB_STATION ) );
		wr = _SCH_IO_GET_MODULE_RESULT( 20000 + tms , TB_STATION );
		if ( wr < 3 ) _SCH_IO_SET_MODULE_RESULT( 20000 + 0 , TA_STATION , wr ); // 2004.01.05
		_SCH_IO_SET_MODULE_STATUS( 20000 + 0 , TA_STATION , wsts );
	}
	else {
		_SCH_IO_SET_MODULE_STATUS( 20000 + 0 , TA_STATION , 0 );
	}
	//-----------------------------------------------------------------------
	if ( mode == 0 ) {
		wsts = _SCH_IO_GET_MODULE_STATUS( ch , 3 );
		if ( wsts > 0 ) {
			_SCH_IO_SET_MODULE_SOURCE( 20000 + tms , TB_STATION , _SCH_IO_GET_MODULE_SOURCE( PM1 , 3 ) ); // 2004.01.05 // 2007.05.02
			wr = _SCH_IO_GET_MODULE_RESULT( PM1 , 3 ); // 2007.05.02
			if ( wr < 3 ) _SCH_IO_SET_MODULE_RESULT( 20000 + tms , TB_STATION , wr ); // 2004.01.05
			_SCH_IO_SET_MODULE_STATUS( 20000 + tms , TB_STATION , wsts );
		}
		else {
			_SCH_IO_SET_MODULE_STATUS( 20000 + tms , TB_STATION , 0 );
		}
		//-----------------------------------------------------------------------
		wsts = _SCH_IO_GET_MODULE_STATUS( ch , 2 );
		if ( wsts > 0 ) {
			_SCH_IO_SET_MODULE_SOURCE( PM1 , 3 , _SCH_IO_GET_MODULE_SOURCE( PM1 , 2 ) ); // 2007.05.02
			wr = _SCH_IO_GET_MODULE_RESULT( PM1 , 2 ); // 2007.05.02
			if ( wr < 3 ) _SCH_IO_SET_MODULE_RESULT( PM1 , 3 , wr ); // 2004.01.05 // 2007.05.02
			_SCH_IO_SET_MODULE_STATUS( ch , 3 , wsts );
		}
		else {
			_SCH_IO_SET_MODULE_STATUS( ch , 3 , 0 );
		}
	}
	else { // 2006.12.18
		wsts = _SCH_IO_GET_MODULE_STATUS( ch , 2 );
		if ( wsts > 0 ) {
			_SCH_IO_SET_MODULE_SOURCE( 20000 + tms , TB_STATION , _SCH_IO_GET_MODULE_SOURCE( PM1 , 2 ) );
			wr = _SCH_IO_GET_MODULE_RESULT( PM1 , 2 ); // 2007.05.02
			if ( wr < 3 ) _SCH_IO_SET_MODULE_RESULT( 20000 + tms , TB_STATION , wr );
			_SCH_IO_SET_MODULE_STATUS( 20000 + tms , TB_STATION , wsts );
		}
		else {
			_SCH_IO_SET_MODULE_STATUS( 20000 + tms , TB_STATION , 0 );
		}
	}
	//-----------------------------------------------------------------------
	wsts = _SCH_IO_GET_MODULE_STATUS( ch , 1 );
	if ( wsts > 0 ) {
		_SCH_IO_SET_MODULE_SOURCE( PM1 , 2 , _SCH_IO_GET_MODULE_SOURCE( ch , 1 ) ); // 2004.01.05
		wr = _SCH_IO_GET_MODULE_RESULT( ch , 1 );
		if ( wr < 3 ) _SCH_IO_SET_MODULE_RESULT( PM1 , 2 , wr ); // 2004.01.05 // 2007.05.02
		_SCH_IO_SET_MODULE_STATUS( ch , 2 , wsts );
	}
	else {
		_SCH_IO_SET_MODULE_STATUS( ch , 2 , 0 );
	}
	//-----------------------------------------------------------------------
	if ( wsts1 > 0 ) {
		_SCH_IO_SET_MODULE_SOURCE( ch , 1 , wsrc1 );
		if ( wres1 < 3 ) _SCH_IO_SET_MODULE_RESULT( ch , 1 , wres1 ); // 2004.01.05
		_SCH_IO_SET_MODULE_STATUS( ch , 1 , wsts1 );
	}
	else {
		_SCH_IO_SET_MODULE_STATUS( ch , 1 , 0 );
	}
	*update = TRUE;
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip PM MOVE Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULING_EQ_PM_MOVE_CONTROL_FOR_STYLE_6( int Chamber , int i1 , int i2 , int i3 , int i4 , int i5 , int mode , int tms , int appstyle ) {
	int Address;
	int Res;
	BOOL update;
	char buffer[48];
	//
	switch( _SCH_EQ_INTERFACE_FUNCTION_ADDRESS( Chamber , &Address ) ) {
	case 0 :
		if ( appstyle == 0 ) { // 2007.04.30
			if ( _SCH_PRM_GET_RB_ROBOT_ANIMATION( 0 ) == 1 ) ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE_STYLE_6( Chamber , &update , mode , tms );
			if ( update ) _SCH_IO_MTL_SAVE();
		}
		if ( appstyle != 2 ) { // 2007.04.30
			Sleep(1000); // 2004.02.27
		}
		return SYS_SUCCESS;
		break;
	case 1 :
		break;
	default :
		return SYS_ABORTED;
		break;
	}
	//
	if      ( appstyle == 1 ) { // 2007.04.30
		sprintf( buffer , "MOVE %d %d %d %d %d" , i1 , i2 , i3 , i4 , i5 );
//		_dRUN_SET_FUNCTION( Address , buffer ); // 2008.04.03
//		Res = SYS_SUCCESS; // 2008.04.03
		if ( _dRUN_SET_FUNCTION( Address , buffer ) ) Res = SYS_SUCCESS; // 2008.04.03
		else                                          Res = SYS_ABORTED; // 2008.04.03
	}
	else if ( appstyle == 2 ) { // 2007.04.30
		Res = _dREAD_FUNCTION( Address );
	}
	else {
		sprintf( buffer , "MOVE %d %d %d %d %d" , i1 , i2 , i3 , i4 , i5 );
		Res = _dRUN_FUNCTION( Address , buffer );
	}
	if ( appstyle == 0 ) { // 2007.04.30
		if ( Res == SYS_SUCCESS ) {
			if ( _SCH_PRM_GET_RB_ROBOT_ANIMATION( 0 ) == 1 ) ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE_STYLE_6( Chamber , &update , mode , tms );
			if ( update ) _SCH_IO_MTL_SAVE();
		}
	}
	return Res;
}


int SCHEDULING_EQ_GATE_CLOSE_STYLE_6( int tmside , int Chamber ) { // 2010.05.21
	char Buffer[32];
	int Address;
	//
	if ( Chamber == AL || Chamber == IC ) return SYS_SUCCESS;
	//
	switch( _SCH_EQ_INTERFACE_FUNCTION_ADDRESS( TM + tmside , &Address ) ) {
	case 0 :
		return SYS_SUCCESS;
		break;
	case 1 :
		if      ( Chamber <  PM1 ) {
			sprintf( Buffer , "CLOSE_GATE CM%d 1" , Chamber - CM1 + 1 );
		}
		else if ( Chamber >= BM1 ) {
			sprintf( Buffer , "CLOSE_GATE BM%d 1" , Chamber - BM1 + 1 );
		}
		else {
			sprintf( Buffer , "CLOSE_GATE PM%d 1" , Chamber - PM1 + 1 );
		}
		if ( !_dRUN_SET_FUNCTION( Address , Buffer ) ) return SYS_ABORTED;
		return SYS_SUCCESS;
		break;
	default :
		return SYS_ABORTED;
		break;
	}
}
