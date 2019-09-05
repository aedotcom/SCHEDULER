#include "default.h"

#include "EQ_Enum.h"

#include "System_Tag.h"
#include "User_Parameter.h"
#include "Robot_Animation.h"
#include "FA_Handling.h"
#include "IO_Part_data.h"

//-----------------------------------------------------------------------
void ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE( int ch , BOOL *update , int mode , int tms ) {
	int wsrc1 , wsts1 , wres1 , wsts , wr;
	//-----------------------------------------------------------------------
	*update = FALSE;
	//-----------------------------------------------------------------------
	wsrc1 = WAFER_SOURCE_IN_TM( 0 , TA_STATION );
	wsts1 = WAFER_STATUS_IN_TM( 0 , TA_STATION );
//	wres1 = WAFER_RESULT_IN_TM( 0 , TA_STATION ); // 2004.01.05 // 2015.05.27
	wres1 = WAFER_RESULT_IN_TM( 0 , TA_STATION , -1 ); // 2004.01.05 // 2015.05.27
	//-----------------------------------------------------------------------
	wsts = WAFER_STATUS_IN_TM( tms , TB_STATION );
	if ( wsts > 0 ) {
		WAFER_SOURCE_SET_TM( 0 , TA_STATION , WAFER_SOURCE_IN_TM( tms , TB_STATION ) );
//		wr = WAFER_RESULT_IN_TM( tms , TB_STATION ); // 2015.05.27
		wr = WAFER_RESULT_IN_TM( tms , TB_STATION , -1 ); // 2015.05.27
//		if ( wr < 3 ) WAFER_RESULT_SET_TM( 0 , TA_STATION , wr ); // 2004.01.05 // 2015.05.27
		if ( wr < 3 ) WAFER_RESULT_SET_TM( 0 , TA_STATION , -1 , wr ); // 2004.01.05 // 2015.05.27
		WAFER_STATUS_SET_TM( 0 , TA_STATION , wsts );
	}
	else {
		WAFER_STATUS_SET_TM( 0 , TA_STATION , 0 );
	}
	//-----------------------------------------------------------------------
	if ( mode == 0 ) {
		wsts = _i_SCH_IO_GET_MODULE_STATUS( ch , 3 );
		if ( wsts > 0 ) {
			WAFER_SOURCE_SET_TM( tms , TB_STATION , _i_SCH_IO_GET_MODULE_SOURCE( PM1 , 3 ) ); // 2004.01.05 // 2007.05.02
			wr = _i_SCH_IO_GET_MODULE_RESULT( PM1 , 3 ); // 2007.05.02
//			if ( wr < 3 ) WAFER_RESULT_SET_TM( tms , TB_STATION , wr ); // 2004.01.05 // 2015.05.27
			if ( wr < 3 ) WAFER_RESULT_SET_TM( tms , TB_STATION , -1 , wr ); // 2004.01.05 // 2015.05.27
			WAFER_STATUS_SET_TM( tms , TB_STATION , wsts );
		}
		else {
			WAFER_STATUS_SET_TM( tms , TB_STATION , 0 );
		}
		//-----------------------------------------------------------------------
		wsts = _i_SCH_IO_GET_MODULE_STATUS( ch , 2 );
		if ( wsts > 0 ) {
			_i_SCH_IO_SET_MODULE_SOURCE( PM1 , 3 , _i_SCH_IO_GET_MODULE_SOURCE( PM1 , 2 ) ); // 2007.05.02
			wr = _i_SCH_IO_GET_MODULE_RESULT( PM1 , 2 ); // 2007.05.02
			if ( wr < 3 ) _i_SCH_IO_SET_MODULE_RESULT( PM1 , 3 , wr ); // 2004.01.05 // 2007.05.02
			_i_SCH_IO_SET_MODULE_STATUS( ch , 3 , wsts );
		}
		else {
			_i_SCH_IO_SET_MODULE_STATUS( ch , 3 , 0 );
		}
	}
	else { // 2006.12.18
		wsts = _i_SCH_IO_GET_MODULE_STATUS( ch , 2 );
		if ( wsts > 0 ) {
			WAFER_SOURCE_SET_TM( tms , TB_STATION , _i_SCH_IO_GET_MODULE_SOURCE( PM1 , 2 ) );
			wr = _i_SCH_IO_GET_MODULE_RESULT( PM1 , 2 ); // 2007.05.02
//			if ( wr < 3 ) WAFER_RESULT_SET_TM( tms , TB_STATION , wr ); // 2015.05.27
			if ( wr < 3 ) WAFER_RESULT_SET_TM( tms , TB_STATION , -1 , wr ); // 2015.05.27
			WAFER_STATUS_SET_TM( tms , TB_STATION , wsts );
		}
		else {
			WAFER_STATUS_SET_TM( tms , TB_STATION , 0 );
		}
	}
	//-----------------------------------------------------------------------
	wsts = _i_SCH_IO_GET_MODULE_STATUS( ch , 1 );
	if ( wsts > 0 ) {
		_i_SCH_IO_SET_MODULE_SOURCE( PM1 , 2 , _i_SCH_IO_GET_MODULE_SOURCE( ch , 1 ) ); // 2004.01.05
		wr = _i_SCH_IO_GET_MODULE_RESULT( ch , 1 );
		if ( wr < 3 ) _i_SCH_IO_SET_MODULE_RESULT( PM1 , 2 , wr ); // 2004.01.05 // 2007.05.02
		_i_SCH_IO_SET_MODULE_STATUS( ch , 2 , wsts );
	}
	else {
		_i_SCH_IO_SET_MODULE_STATUS( ch , 2 , 0 );
	}
	//-----------------------------------------------------------------------
	if ( wsts1 > 0 ) {
		_i_SCH_IO_SET_MODULE_SOURCE( ch , 1 , wsrc1 );
		if ( wres1 < 3 ) _i_SCH_IO_SET_MODULE_RESULT( ch , 1 , wres1 ); // 2004.01.05
		_i_SCH_IO_SET_MODULE_STATUS( ch , 1 , wsts1 );
	}
	else {
		_i_SCH_IO_SET_MODULE_STATUS( ch , 1 , 0 );
	}
	*update = TRUE;
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
