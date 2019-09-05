#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_log.h"
#include "Robot_Animation.h"
#include "Equip_Handling.h"
#include "sch_estimate.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Clear Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_CLEAR_CHAMBER( int Chamber , int Finger , int Slot ) {
	char Param[64];
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return SYS_SUCCESS;
	sprintf( Param , "CLEAR %c_%s %d" , Finger - TA_STATION + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot );
	return _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , Param );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip PM MOVE Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_PM_MOVE_CONTROL( int Chamber , int i1 , int i2 , int i3 , int i4 , int i5 , int mode , int tms , int appstyle ) {
	int Res;
	BOOL update;

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) {
		if ( appstyle == 0 ) { // 2007.04.30
			if ( _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( 0 ) == 1 ) ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE( Chamber , &update , mode , tms );
			if ( update ) _i_SCH_IO_MTL_SAVE();
		}
		if ( appstyle != 2 ) { // 2007.04.30
			Sleep(1000); // 2004.02.27
		}
		return SYS_SUCCESS;
	}
	if      ( appstyle == 1 ) { // 2007.04.30
//		_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber] , "MOVE %d %d %d %d %d" , i1 , i2 , i3 , i4 , i5 ); // 2008.04.03
//		Res = SYS_SUCCESS; // 2008.04.03
		if ( _dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber] , "MOVE %d %d %d %d %d" , i1 , i2 , i3 , i4 , i5 ) ) { // 2008.04.03
			Res = SYS_SUCCESS; // 2008.04.03
		}
		else {
			Res = SYS_ABORTED; // 2008.04.03
		}
	}
	else if ( appstyle == 2 ) { // 2007.04.30
		Res = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
	}
	else {
		Res = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber] , "MOVE %d %d %d %d %d" , i1 , i2 , i3 , i4 , i5 );
	}
	if ( appstyle == 0 ) { // 2007.04.30
		if ( Res == SYS_SUCCESS ) {
			if ( _i_SCH_PRM_GET_RB_ROBOT_ANIMATION( 0 ) == 1 ) ROBOT_ANIMATION_SPECIAL_RUN_FOR_PM_MOVE( Chamber , &update , mode , tms );
			if ( update ) _i_SCH_IO_MTL_SAVE();
		}
	}
	return Res;
}




















