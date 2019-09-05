//================================================================================
#include "INF_default.h"
//===========================================================================================================================
#include "INF_CimSeqnc.h"
//===========================================================================================================================
#include "INF_EQ_Enum.h"
#include "INF_Equip_Handling.h"
#include "INF_sch_macro.h"
#include "INF_User_Parameter.h" // 2016.11.24
//================================================================================


void SCHEDULING_EQ_INTERFACE_STRING_APPEND_TRANSFER_SUB_STYLE_0( int Side , char *data ) {
	switch( Side ) {
	case 1 : strcat( data , " TRANSFER" ); break;
	case 2 : strcat( data , " TRANSFER2" ); break;
	case 3 : strcat( data , " TRANSFER3" ); break;
	case 4 : strcat( data , " TRANSFER4" ); break;
	case 5 : strcat( data , " TRANSFER5" ); break;
	}
}


int SCHEDULING_EQ_GATE_CLOSE_STYLE_0( int tmside , int Chamber ) { // 2010.05.21
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

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Fail Confirm Control // 2006.02.03
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULING_EQ_SET_FAIL_CONFIRMATION_STYLE_0( int Chamber , int slot ) {
	int Address;
	char buffer[32];
	switch( _SCH_EQ_INTERFACE_FUNCTION_ADDRESS( Chamber , &Address ) ) {
	case 0 :
		return SYS_SUCCESS;
		break;
	case 1 :
		sprintf( buffer , "FAIL_CONFIRMATION %d" , slot );
//		_dRUN_SET_FUNCTION( Address , buffer ); // 2008.04.03
		if ( !_dRUN_SET_FUNCTION( Address , buffer ) ) return SYS_ABORTED; // 2008.04.03
		return SYS_SUCCESS;
		break;
	default :
		return SYS_ABORTED;
		break;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Possible / Done for AL 0 , Sub 2
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int SCHEDULING_EQ_CM_SUB2_PICKPLACE_POSSIBLECOMPLETE_STYLE_0( int Chamber , int PickMode , int PossibleMode , int DoSlot , int Finger , int SrcCm , int SourceSlot , int order ) {
	char Param[64] , Buffer[64];
	int Address;
	//
	switch( _SCH_EQ_INTERFACE_FUNCTION_ADDRESS( Chamber , &Address ) ) {
	case 0 :
		return SYS_SUCCESS;
		break;
	case 1 :
		if ( Chamber >= PM1 ) return SYS_ABORTED;
		break;
	default :
		return SYS_ABORTED;
		break;
	}
	//----------------------------------------------------------
	if ( PickMode ) {
		strcpy( Param , "PICK" );
	}
	else {
		strcpy( Param , "PLACE" );
	}
	//----------------------------------------------------------
	if ( PossibleMode ) {
		strcat( Param , "_POSSIBLE" );
	}
	else {
		strcat( Param , "_COMPLETE" );
	}
	//----------------------------------------------------------
	sprintf( Buffer , " %d/%c/CM%d/%d" , DoSlot , Finger - TA_STATION + 'A' , SrcCm - CM1 + 1 , SourceSlot );
	strcat( Param , Buffer );
	//----------------------------------------------------------
	if ( order > 0 ) {
		sprintf( Buffer , " ORDER%d" , order );
		strcat( Param , Buffer );
	}
	//----------------------------------------------------------
	return _dRUN_FUNCTION( Address , Param );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int SCHEDULING_EQ_SUB4_INTERFACE_PREPARE_PLACE_CANCEL_STYLE_0( int tmside , int Chamber , int Finger , int Slot , int Depth , int Transfer , int SrcCas ) {
	int Address_P , Address_T;
	int Mp , Mt;
	BOOL PM_Run , TM_Run;
	char ParamT[64] , ParamP[64] , Buffer[64];
	//
	if ( Chamber == AL || Chamber == IC ) return SYS_SUCCESS;
	//
	if ( _SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( TM + tmside ) == 1 ) { // 2016.11.24 // 0:def, 1:notuse 2:use
		TM_Run = FALSE;
	}
	else {
		switch( _SCH_EQ_INTERFACE_FUNCTION_ADDRESS( TM + tmside , &Address_T ) ) {
		case 0 :
			TM_Run = FALSE;
			break;
		case 1 :
			TM_Run = TRUE;
			break;
		default :
			return SYS_ABORTED;
			break;
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_CANCEL( Chamber     ) == 1 ) { // 2016.11.24 // 0:def, 1:notuse 2:use
		PM_Run = FALSE;
	}
	else {
		switch( _SCH_EQ_INTERFACE_FUNCTION_ADDRESS( Chamber , &Address_P ) ) {
		case 0 :
			PM_Run = FALSE;
			break;
		case 1 :
			PM_Run = TRUE;
			break;
		default :
			return SYS_ABORTED;
			break;
		}
	}
	//
	if ( !TM_Run && !PM_Run ) return SYS_SUCCESS;
	//
	strcpy( ParamT , "PREPARE_SEND_CANCEL" );
	strcpy( ParamP , "PREPARE_RECV_CANCEL" );
	//
	if      ( Chamber <  PM1 ) {
		sprintf( Buffer , " %d %c/%d/%d/%d" , Slot , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
	}
	else if ( Chamber >= BM1 ) {
		sprintf( Buffer , " %d %c/%d/%d/%d" , Depth , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
	}
	else {
		sprintf( Buffer , " %d %c/%d/%d/%d" , Depth , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
	}
	strcat( ParamP , Buffer );
	if      ( Chamber <  PM1 ) {
		sprintf( Buffer , " %c_CM%d %d" , Finger - TA_STATION + 'A' , Chamber - CM1 + 1 , Slot );
	}
	else if ( Chamber >= BM1 ) {
		sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth );
	}
	else {
		sprintf( Buffer , " %c_PM%d %d" , Finger - TA_STATION + 'A' , Chamber - PM1 + 1 , Depth );
	}
	strcat( ParamT , Buffer );
	//
	SCHEDULING_EQ_INTERFACE_STRING_APPEND_TRANSFER_SUB_STYLE_0( Transfer , ParamP );
	SCHEDULING_EQ_INTERFACE_STRING_APPEND_TRANSFER_SUB_STYLE_0( Transfer , ParamT );
	//
	if ( TM_Run && PM_Run ) {
		Mt = SYS_RUNNING;
		Mp = SYS_RUNNING;
		_dRUN_SET_FUNCTION_FREE( Address_T , ParamT );
		_dRUN_SET_FUNCTION_FREE( Address_P , ParamP );
	}
	else if ( TM_Run ) {
		Mt = SYS_RUNNING;
		Mp = SYS_SUCCESS;
		_dRUN_SET_FUNCTION_FREE( Address_T , ParamT );
	}
	else if ( PM_Run ) {
		Mt = SYS_SUCCESS;
		Mp = SYS_RUNNING;
		_dRUN_SET_FUNCTION_FREE( Address_P , ParamP );
	}
	else {
		Mt = SYS_SUCCESS;
		Mp = SYS_SUCCESS;
	}
	while ( TRUE ) {
		if ( TM_Run && ( Mt == SYS_RUNNING ) ) {
			Mt = _dREAD_FUNCTION( Address_T );
		}
		if ( PM_Run && ( Mp == SYS_RUNNING ) ) {
			Mp = _dREAD_FUNCTION( Address_P );
		}
		if ( ( Mt != SYS_RUNNING ) && ( Mp != SYS_RUNNING ) ) {
			if ( ( Mt == SYS_ABORTED ) || ( Mp == SYS_ABORTED ) )
				return SYS_ABORTED;
			else
				break;
		}
		Sleep(1);
	}
	return SYS_SUCCESS;
}

