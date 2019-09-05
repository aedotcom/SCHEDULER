#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "Equip_Handling.h"
#include "System_Tag.h"
#include "sch_EIL.h"
#include "sch_estimate.h"

#include "INF_sch_CommonUser.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Begin End Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_EQ_BEGIN_END_RUN_SUB( int Chamber , BOOL End , char *appendstring , int Disable ) {
	char Buffer[256];
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	//
	// 2016.10.22
	//
	if ( End ) {
		if      ( ( Chamber >= CM1 ) && ( Chamber < PM1 ) ) {
			EST_CM_END( Chamber );
		}
		else if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
			EST_PM_END( Chamber );
		}
	}
	else {
		if      ( ( Chamber >= CM1 ) && ( Chamber < PM1 ) ) {
			EST_CM_BEGIN( Chamber );
		}
		else if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
			EST_PM_BEGIN( Chamber );
		}
		else if ( ( Chamber >= BM1 ) && ( Chamber < TM ) ) {
			EST_BM_BEGIN( Chamber );
		}
	}
	//
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.10
		if (
			( ( Chamber >= PM1 ) && ( Chamber < AL ) ) ||
			( ( Chamber >= BM1 ) && ( Chamber < TM ) ) ) {
			_EIL_FMMODE_EQ_BEGIN_END_RUN( Chamber , End , appendstring , Disable );
			return;
		}
	}
	//========================
	if ( End ) {
		if ( ( Chamber >= CM1 ) && ( Chamber < PM1 ) ) {
			_IO_CONSOLE_PRINTF( "==> MTLOUT END = %s =\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) );
		}
	}
	else {
		if ( ( Chamber >= CM1 ) && ( Chamber < PM1 ) ) {
			_IO_CONSOLE_PRINTF( "==> MTLOUT READY           = %s =\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) );
		}
	}
	//
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) ) return; // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return; // 2014.01.09
	//
	if ( End )	{
		if ( strlen( appendstring ) <= 0 ) {
			if      ( Disable == 1 ) { // 2003.10.08
				strcpy( Buffer , "END DISABLE" );
			}
			else if ( Disable == 2 ) { // 2005.09.07
				strcpy( Buffer , "END ENABLEPM" );
			}
			else if ( Disable == 3 ) { // 2010.09.26
				strcpy( Buffer , "END MAINT" );
			}
			else {
				strcpy( Buffer , "END" );
			}
		}
		else { // 2003.05.29
			if ( Disable == 1 ) { // 2003.10.08
				sprintf( Buffer , "END DISABLE %s" , appendstring );
			}
			else if ( Disable == 2 ) { // 2005.09.07
				sprintf( Buffer , "END ENABLEPM %s" , appendstring );
			}
			else if ( Disable == 3 ) { // 2010.09.26
				sprintf( Buffer , "END MAINT %s" , appendstring );
			}
			else {
				sprintf( Buffer , "END %s" , appendstring );
			}
		}
	}
	else {
		if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
			if ( ( Chamber >= PM1 ) && ( Chamber < AL ) ) {
				switch( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( Chamber ) ) {
				case 1  :	strcpy( Buffer , "READY 1" );	break;
				case 2  :	strcpy( Buffer , "READY 2" );	break;
				default :	strcpy( Buffer , "READY 0" );	break;
				}
			}
			else {
				strcpy( Buffer , "READY" );
			}
		}
		else {
			if ( strlen( appendstring ) <= 0 ) {
				if ( Disable == 1 ) { // 2003.10.08
					strcpy( Buffer , "READY DISABLE" );
				}
				else if ( Disable == 2 ) { // 2005.09.07
					strcpy( Buffer , "READY ENABLEPM" );
				}
				else if ( Disable == 3 ) { // 2010.09.26
					strcpy( Buffer , "READY MAINT" );
				}
				else {
					strcpy( Buffer , "READY" );
				}
			}
			else { // 2003.05.29
				if ( Disable == 1 ) { // 2003.10.08
					sprintf( Buffer , "READY DISABLE %s" , appendstring );
				}
				else if ( Disable == 2 ) { // 2005.09.07
					sprintf( Buffer , "READY ENABLEPM %s" , appendstring );
				}
				else if ( Disable == 3 ) { // 2010.09.26
					sprintf( Buffer , "READY MAINT %s" , appendstring );
				}
				else {
					sprintf( Buffer , "READY %s" , appendstring );
				}
			}
		}
	}
	//
	_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , Buffer );
	//------------------------------------------------------------------------------------------------------------------
	_IO_CONSOLE_PRINTF( "EQUIP-MSG\t%s=>[%s]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Buffer );
	//------------------------------------------------------------------------------------------------------------------
}

void _i_SCH_EQ_BEGIN_END_RUN( int Chamber , BOOL End , char *appendstring , int Disable ) {
	//
	_i_SCH_EQ_BEGIN_END_RUN_SUB( Chamber , End , appendstring , Disable );
	//
	_SCH_COMMON_MESSAGE_CONTROL_FOR_USER( TRUE , 1 , Chamber , End , Disable , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , appendstring , NULL , NULL ); // 2012.12.04
	//
}
//
int EQUIP_BEGIN_END_STATUS( int Chamber ) {
	//========================
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.10
		if (
			( ( Chamber >= PM1 ) && ( Chamber < AL ) ) ||
			( ( Chamber >= BM1 ) && ( Chamber < TM ) ) ) {
			return _EIL_FMMODE_EQ_BEGIN_END_STATUS( Chamber );
		}
	}
	//========================
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) ) return SYS_SUCCESS; // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return SYS_SUCCESS; // 2014.01.09
	//
	return _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
}

void EQUIP_BM_END_RUN_SUB_CALL( int Chamber , int Disable ) { // 2006.10.24
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( Address_FUNCTION_INTERFACE_SUB_CALL_END[Chamber] < 0 ) ) return; // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(Chamber) || ( Address_FUNCTION_INTERFACE_SUB_CALL_END[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return; // 2014.01.09
	//
	if ( Disable == 1 ) {
		_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_SUB_CALL_END[Chamber] , "DISABLE" );
		//------------------------------------------------------------------------------------------------------------------
		_IO_CONSOLE_PRINTF( "EQUIP-MSG\t%s=>[SUB_CALL-END:DISABLE]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) );
		//------------------------------------------------------------------------------------------------------------------
	}
	else if ( Disable == 2 ) {
		_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_SUB_CALL_END[Chamber] , "ENABLEPM" );
		//------------------------------------------------------------------------------------------------------------------
		_IO_CONSOLE_PRINTF( "EQUIP-MSG\t%s=>[SUB_CALL-END:ENABLEPM]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) );
		//------------------------------------------------------------------------------------------------------------------
	}
	else {
		_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE_SUB_CALL_END[Chamber] , "" );
		//------------------------------------------------------------------------------------------------------------------
		_IO_CONSOLE_PRINTF( "EQUIP-MSG\t%s=>[SUB_CALL-END:]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) );
		//------------------------------------------------------------------------------------------------------------------
	}
}
//
