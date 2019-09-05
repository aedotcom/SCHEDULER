#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "sch_prm_FBTPM.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "Equip_Handling.h"
#include "FA_Handling.h"
#include "sch_estimate.h"

extern int  _SCH_TAG_BM_LAST_PLACE; // 2008.03.24
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_CARRIER_TM_WAFER_FREE( int TMATM ) {
	int i;
	for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
		if ( i < _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) {
			if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , i ) ) {
				if ( WAFER_STATUS_IN_TM(TMATM,i) > 0 ) return FALSE;
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Carrier HanfOff
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_CARRIER_HANDOFF( BOOL InMode , int Chamber1 , int Chamber2 ) {
	int Res1 , Res2;
	BOOL Run;
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( ( Chamber1 < 0 ) && ( Chamber2 < 0 ) ) return SYS_SUCCESS;
	if ( Chamber1 < 0 ) Chamber1 = Chamber2;
	if ( Chamber2 < 0 ) Chamber2 = Chamber1;
	//
	if ( InMode ) {
		if ( Chamber1 >= CM1 ) {
			_IO_CONSOLE_PRINTF( "==> MTLOUT HANDOFF IN = %s =[1]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber1 ) );
		}
		if ( ( Chamber2 >= CM1 ) && ( Chamber1 != Chamber2 ) ) {
			_IO_CONSOLE_PRINTF( "==> MTLOUT HANDOFF IN = %s =[2]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber2 ) );
		}
	}
	else {
		if ( Chamber1 >= CM1 ) {
			_IO_CONSOLE_PRINTF( "==> MTLOUT HANDOFF OUT = %s =[1]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber1 ) );
		}
		if ( ( Chamber2 >= CM1 ) && ( Chamber1 != Chamber2 ) ) {
			_IO_CONSOLE_PRINTF( "==> MTLOUT HANDOFF OUT = %s =[2]\n" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber2 ) );
		}
	}
	//
	if ( InMode ) {
		if (
			( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 4 ) )
			&&
			( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 4 ) )
			)
			return SYS_SUCCESS;
	}
	else {
		if (
			( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) >= 4 ) )
			&&
			( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) >= 4 ) )
			)
			return SYS_SUCCESS;
	}

	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		Run = TRUE;
		if ( InMode &&
			( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 4 ) ) ) Run = FALSE;
		if ( !InMode &&
			( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) >= 4 ) ) ) Run = FALSE;
		if ( Run ) {
			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
				if ( InMode )
					Res1 = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_IN AUTO" );
				else
					Res1 = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_OUT AUTO" );
				if ( Res1 == SYS_ABORTED ) return SYS_ABORTED;
			}
		}
		if ( Chamber1 != Chamber2 ) {
			Run = TRUE;
			if ( InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 4 ) ) ) Run = FALSE;
			if ( !InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) >= 4 ) ) ) Run = FALSE;
			if ( Run ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber2 ) && ( Address_FUNCTION_INTERFACE[Chamber2] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( InMode )
						Res2 = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] , "HAND_OFF_IN AUTO" );
					else
						Res2 = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] , "HAND_OFF_OUT AUTO" );
				}
				else {
					Res2 = SYS_SUCCESS;
				}
			}
			else {
				Res2 = SYS_SUCCESS;
			}
		}
		else {
			Res2 = SYS_SUCCESS;
		}
	}
	else {
		if ( Chamber1 == Chamber2 ) {
			Run = TRUE;
			if ( InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 4 ) ) ) Run = FALSE;
			if ( !InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) >= 4 ) ) ) Run = FALSE;
			if ( Run ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( InMode )
						Res1 = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_IN AUTO" );
					else
						Res1 = _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_OUT AUTO" );
				}
				else {
					Res1 = SYS_SUCCESS;
				}
			}
			else {
				Res1 = SYS_SUCCESS;
			}
			return Res1;
		}
		else {
			Run = TRUE;
			if ( InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 4 ) ) ) Run = FALSE;
			if ( !InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber1 ) >= 4 ) ) ) Run = FALSE;
			if ( Run ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					Res1 = SYS_RUNNING;
					if ( InMode ) {
//						_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_IN AUTO" ); // 2008.04.03
						if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_IN AUTO" ) ) Res1 = SYS_ABORTED; // 2008.04.03
					}
					else {
//						_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_OUT AUTO" ); // 2008.04.03
						if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] , "HAND_OFF_OUT AUTO" ) ) Res1 = SYS_ABORTED; // 2008.04.03
					}
				}
				else {
					Res1 = SYS_SUCCESS;
				}
			}
			else {
				Res1 = SYS_SUCCESS;
			}
			Run = TRUE;
			if ( InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 2 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 4 ) ) ) Run = FALSE;
			if ( !InMode &&
				( ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 0 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) == 1 ) || ( _i_SCH_PRM_GET_AUTO_HANDOFF( Chamber2 ) >= 4 ) ) ) Run = FALSE;
			if ( Run ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber2 ) && ( Address_FUNCTION_INTERFACE[Chamber2] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					Res2 = SYS_RUNNING;
					if ( InMode ) {
//						_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] , "HAND_OFF_IN AUTO" ); // 2008.04.03
						if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] , "HAND_OFF_IN AUTO" ) ) Res2 = SYS_ABORTED; // 2008.04.03
					}
					else {
//						_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] , "HAND_OFF_OUT AUTO" ); // 2008.04.03
						if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] , "HAND_OFF_OUT AUTO" ) ) Res2 = SYS_ABORTED; // 2008.04.03
					}
				}
				else {
					Res2 = SYS_SUCCESS;
				}
			}
			else {
				Res2 = SYS_SUCCESS;
			}
			while ( ( Res1 == SYS_RUNNING ) || ( Res2 == SYS_RUNNING ) ) {
				if ( Res1 == SYS_RUNNING ) Res1 = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] );
				if ( Res2 == SYS_RUNNING ) Res2 = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] );
				Sleep(1);
			}
			if ( ( Res1 == SYS_ABORTED ) || ( Res2 == SYS_ABORTED ) ) Res2 = SYS_ABORTED;
		}
	}
	return Res2;
}
//
int EQUIP_CARRIER_HANDOFF_OUT( int Chamber ) {
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	if ( ( Chamber < CM1 ) || ( Chamber >= PM1 ) ) return SYS_SUCCESS;
	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) && ( Address_FUNCTION_INTERFACE[Chamber] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
		return( _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , "HAND_OFF_OUT AUTO" ) );
	}
	return SYS_SUCCESS;
}


//BOOL EQUIP_CARRIER_ABORT_CHECK( int side , int ManualMode ) { // 2008.03.18 // 2008.06.06
BOOL EQUIP_CARRIER_ABORT_CHECK( int side , int ManualMode , BOOL syscheckskip ) { // 2008.03.18 // 2008.06.06
	if ( !syscheckskip ) { // 2008.06.06
//		if ( SIGNAL_MODE_ABORT_GET( side ) ) return TRUE; // 2016.01.12
		if ( SIGNAL_MODE_ABORT_GET( side ) > 0 ) return TRUE; // 2016.01.12
	}
	if ( ManualMode == 1 ) {
		if ( FA_MAPPING_CANCEL_GET( side ) == 1 ) return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Carrier Mapping
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_CARRIER_MAPPING( int side , BOOL msgoff , BOOL FAMode , BOOL InMode , BOOL SkipCheck , int Chamber1 , int Chamber2 , BOOL Ch1Run , BOOL Ch2Run , int StartSlot , int EndSlot , int ManualMode , int *errcode ) {
	int Result1[MAX_CASSETTE_SIDE] , Result2[MAX_CASSETTE_SIDE];
	int Pre_Mode[MAX_CASSETTE_SIDE]; // 0 : FALSE / 1 : TRUE
	int Seq_Mode[MAX_CASSETTE_SIDE]; // 0 : Multi / 1 : Single
	int Free_Mode[MAX_CASSETTE_SIDE]; // 0 : Always / 1 : Free
	BOOL Run1[MAX_CASSETTE_SIDE] , Run2[MAX_CASSETTE_SIDE];
	BOOL C1Run[MAX_CHAMBER] , C2Run[MAX_CHAMBER];
	BOOL C1CMMode[MAX_CHAMBER] , C2CMMode[MAX_CHAMBER] , FMMode; // 2010.03.10

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	*errcode = 0;
	//
	if ( ( Chamber1 < 0 ) && ( Chamber2 < 0 ) ) return SYS_SUCCESS;
	//
	if ( Chamber1 < 0 ) {
		Chamber1 = Chamber2;
		Ch1Run   = Ch2Run;
	}
	if ( Chamber2 < 0 ) {
		Chamber2 = Chamber1;
		Ch2Run   = Ch1Run;
	}
	//
	C1CMMode[Chamber1] = ( ( Chamber1 >= CM1 ) && ( Chamber1 < PM1 ) ); // 2010.03.10
	C2CMMode[Chamber2] = ( ( Chamber2 >= CM1 ) && ( Chamber2 < PM1 ) ); // 2010.03.10
	//
	FMMode = ( C1CMMode[Chamber1] ) ? _i_SCH_PRM_GET_MODULE_MODE( FM ) : FALSE; // 2010.03.10
	//
	C1Run[Chamber1] = TRUE;
	C2Run[Chamber2] = TRUE;
	//
	if ( SkipCheck ) {
		if ( InMode ) {
			if ( C1CMMode[Chamber1] ) { // 2010.03.10
				switch ( _i_SCH_PRM_GET_MAPPING_SKIP( Chamber1 ) ) {
				case 1 : case 3 : case 5 :
					C1Run[Chamber1] = FALSE;
					break;
				case 6 : case 7 : case 8 :
					if ( FAMode ) C1Run[Chamber1] = FALSE;
					break;
				}
			}
			//
			if ( C2CMMode[Chamber2] ) { // 2010.03.10
				switch ( _i_SCH_PRM_GET_MAPPING_SKIP( Chamber2 ) ) {
				case 1 : case 3 : case 5 :
					C2Run[Chamber2] = FALSE;
					break;
				case 6 : case 7 : case 8 :
					if ( FAMode ) C2Run[Chamber2] = FALSE;
					break;
				}
			}
			if ( ( !C1Run[Chamber1] ) && ( !C2Run[Chamber2] ) ) {
				*errcode = 2;
				return SYS_SUCCESS;
			}
		}
		else {
			if ( C1CMMode[Chamber1] ) { // 2010.03.10
				switch ( _i_SCH_PRM_GET_MAPPING_SKIP( Chamber1 ) ) {
				case 0 : case 1 : case 6 :
					C1Run[Chamber1] = FALSE;
					break;
				}
			}
			if ( C2CMMode[Chamber2] ) { // 2010.03.10
				switch ( _i_SCH_PRM_GET_MAPPING_SKIP( Chamber2 ) ) {
				case 0 : case 1 : case 6 :
					C2Run[Chamber2] = FALSE;
					break;
				}
			}
			if ( ( !C1Run[Chamber1] ) && ( !C2Run[Chamber2] ) ) {
				*errcode = 3;
				return SYS_SUCCESS;
			}
		}
	}
	//
	//
	// 0 : Multi / 1 : Single
	// 0 : Always / 1 : Free

	// TM MODE
	//
	// Multiple							0
	// With Single & When TM Free		1
	// With Single Only					2
	// Multiple & When TM Free			3
	// With Single & (Auto) TM Free		4
	// Multiple & (Auto) TM Free		5
	//
	// Multi+Single & When TM Free		6	1
	// Multi+Single Only				7	2
	// Multi+Single & (Auto) TM Free	8	4
	//
	// With Single & When BM Not Placed	9	1
	// Multiple & When BM Not Placed	10	3
	// Multi+Single & BM Not Placed		11	1
	//
	// FM MODE
	//
	// With Single Only					0
	// With Single & When FM Free		1
	// Multiple							2
	// Multiple & When FM Free			3
	// With Single & (Auto) FM Free		4
	// Multiple & (Auto) FM Free		5
	//
	// Multi+Single & When FM Free		6	1
	// Multi+Single Only				7	0
	// Multi+Single & (Auto) FM Free	8	4
	//
	// With Single & When BM Not Placed	9	1
	// Multiple & When BM Not Placed	10	3
	// Multi+Single & BM Not Placed		11	1
	//

	switch( _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() ) { // 2008.03.19
	case 6 :
	case 7 :
	case 8 :
	case 11 :
		if ( InMode && ( ManualMode != 2 ) ) {
			Pre_Mode[side] = 1;
		}
		else {
			Pre_Mode[side] = 0;
		}
		break;
	default :
		Pre_Mode[side] = 0;
		break;
	}
	//
//	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.03.10
	if ( !FMMode ) { // 2010.03.10
		switch( _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() ) {
		case 1 :
		case 6 : // 2008.03.19
			Free_Mode[side] = 1; Seq_Mode[side] = 1;
			break;
		case 2 :
		case 7 : // 2008.03.19
			Free_Mode[side] = 0; Seq_Mode[side] = 1;
			break;
		case 3 :
			Free_Mode[side] = 1; Seq_Mode[side] = 0;
			break;
		//
		case 4 : // 2004.08.18
		case 8 : // 2008.03.19
			if ( ManualMode != 0 ) {
				Free_Mode[side] = 0; Seq_Mode[side] = 1;
			}
			else {
				Free_Mode[side] = 1; Seq_Mode[side] = 1;
			}
			break;
		case 5 : // 2004.08.18
			if ( ManualMode != 0 ) {
				Free_Mode[side] = 0; Seq_Mode[side] = 0;
			}
			else {
				Free_Mode[side] = 1; Seq_Mode[side] = 0;
			}
			break;
		case 9 : // 2008.03.24
		case 11 : // 2008.03.24
			Free_Mode[side] = 2; Seq_Mode[side] = 1;
			break;
		case 10 : // 2008.03.24
			Free_Mode[side] = 2; Seq_Mode[side] = 0;
			break;
		//
		default : Free_Mode[side] = 0; Seq_Mode[side] = 0; break;
		}
	}
	else {
		switch( _i_SCH_PRM_GET_UTIL_MAPPING_WHEN_TMFREE() ) {
		case 1 :
		case 6 : // 2008.03.19
			Free_Mode[side] = 1; Seq_Mode[side] = 1;
			break;
		case 2 :
			Free_Mode[side] = 0; Seq_Mode[side] = 0;
			break;
		case 3 :
			Free_Mode[side] = 1; Seq_Mode[side] = 0;
			break;
		//
		case 4 : // 2008.03.19
		case 8 : // 2008.03.19
			if ( ManualMode != 0 ) {
				Free_Mode[side] = 0; Seq_Mode[side] = 1;
			}
			else {
				Free_Mode[side] = 1; Seq_Mode[side] = 1;
			}
			break;
		case 5 : // 2008.03.19
			if ( ManualMode != 0 ) {
				Free_Mode[side] = 0; Seq_Mode[side] = 0;
			}
			else {
				Free_Mode[side] = 1; Seq_Mode[side] = 0;
			}
			break;
		case 9 : // 2008.03.24
		case 11 : // 2008.03.24
			Free_Mode[side] = 2; Seq_Mode[side] = 1;
			break;
		case 10 : // 2008.03.24
			Free_Mode[side] = 2; Seq_Mode[side] = 0;
			break;
		default :
			Free_Mode[side] = 0; Seq_Mode[side] = 1;
			break;
		}
	}
	//-----------------------------------------------------------------------------------------------------
	// 2008.03.18
	//-----------------------------------------------------------------------------------------------------
	if ( Pre_Mode[side] == 1 ) {
		if ( ManualMode == 1 ) FA_MAPPING_SIGNAL_SET( side , 3 ); // 2008.03.19
		//
		if ( Chamber1 == Chamber2 ) {
			if ( C1Run[Chamber1] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( GET_RUN_LD_CONTROL(0) <= 0 ) { // 2014.01.09
						if ( _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP PRE" ) != SYS_SUCCESS ) {
							*errcode = 4;
							return SYS_ABORTED;
						}
					}
				}
			}
		}
		else {
			Run1[side] = FALSE;
			Run2[side] = FALSE;
			Result1[side] = SYS_SUCCESS;
			Result2[side] = SYS_SUCCESS;
			if ( C1Run[Chamber1] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( GET_RUN_LD_CONTROL(0) <= 0 ) { // 2014.01.09
						Run1[side] = TRUE;
						Result1[side] = SYS_RUNNING;
	//					_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP PRE" ); // 2008.04.03
						if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP PRE" ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
					}
				}
			}
			if ( C2Run[Chamber2] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber2 ) && ( Address_FUNCTION_INTERFACE[Chamber2] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( GET_RUN_LD_CONTROL(0) <= 0 ) { // 2014.01.09
						Run2[side] = TRUE;
						Result2[side] = SYS_RUNNING;
	//					_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP PRE" ); // 2008.04.03
						if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP PRE" ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
					}
				}
			}
			while ( ( Result1[side] == SYS_RUNNING ) || ( Result2[side] == SYS_RUNNING ) ) {
				if ( Result1[side] == SYS_RUNNING ) Result1[side] = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] );
				if ( Result2[side] == SYS_RUNNING ) Result2[side] = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] );
				Sleep(1);
			}
			if ( Run1[side] ) {
				if ( Result1[side] == SYS_ABORTED ) {
					*errcode = 5;
					return SYS_ABORTED;
				}
			}
			if ( Run2[side] ) {
				if ( Result2[side] == SYS_ABORTED ) {
					*errcode = 6;
					return SYS_ABORTED;
				}
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------
	if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) {
		*errcode = 7;
		return SYS_ABORTED; // 2008.03.19
	}
	//-----------------------------------------------------------------------------------------------------
//	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.03.10
	if ( !FMMode ) { // 2010.03.10
		//------------------------------------------------------------------------
		// 2008.03.20
		//------------------------------------------------------------------------
		if ( Seq_Mode[side] == 1 ) {
			EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE );
			if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) {
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE );
				*errcode = 8;
				return SYS_ABORTED;
			}
		}
		if ( Free_Mode[side] >= 1 ) {
			while ( TRUE ) {
				//
				if ( EQUIP_CARRIER_TM_WAFER_FREE(0) ) {
					//
					EnterCriticalSection( &CR_TMRUN[0] );
					//
					if ( EQUIP_CARRIER_TM_WAFER_FREE(0) ) break;
					//
					LeaveCriticalSection( &CR_TMRUN[0] );
				}
				Sleep(1);
				if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) {
					if ( Seq_Mode[side] == 1 ) LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE );
					*errcode = 9;
					return SYS_ABORTED;
				}
			}
		}
		//------------------------------------------------------------------------
	}
	else {
		//------------------------------------------------------------------------
		// 2008.03.20
		//------------------------------------------------------------------------
		if ( ( Seq_Mode[side] == 1 ) && ( Free_Mode[side] == 0 ) ) {
			EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE );
			if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) { // 2008.03.19
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE );
				*errcode = 10;
				return SYS_ABORTED;
			}
		}
		else if ( Free_Mode[side] == 1 ) {
			while ( TRUE ) {
				if ( ( WAFER_STATUS_IN_FM(0,TA_STATION) <= 0 ) && ( WAFER_STATUS_IN_FM(0,TB_STATION) <= 0 ) ) {
					EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE );
					if ( ( WAFER_STATUS_IN_FM(0,TA_STATION) <= 0 ) && ( WAFER_STATUS_IN_FM(0,TB_STATION) <= 0 ) ) {
						break;
					}
					LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE );
				}
				Sleep(1);
				if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) {
					*errcode = 11;
					return SYS_ABORTED; // 2008.03.19
				}
			}
		}
		else if ( Free_Mode[side] == 2 ) {
			while ( TRUE ) {
				if ( _SCH_TAG_BM_LAST_PLACE == 0 ) {
					EnterCriticalSection( &CR_FEM_MAP_PICK_PLACE );
					if ( _SCH_TAG_BM_LAST_PLACE == 0 ) {
						break;
					}
					LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE );
				}
				else { // 2008.09.24
					if ( !_i_SCH_SYSTEM_USING_ANOTHER_RUNNING( side ) ) { // 2008.09.24
						_SCH_TAG_BM_LAST_PLACE = 0;
					}
				}
				Sleep(1);
				if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) {
					*errcode = 12;
					return SYS_ABORTED; // 2008.03.19
				}
			}
		}
		//------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------
	// 2008.03.19
	//-----------------------------------------------------------------------------------------------------
	if ( EQUIP_CARRIER_ABORT_CHECK( side , ManualMode , msgoff ) ) {
//		if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.03.10
		if ( !FMMode ) { // 2010.03.10
			if ( Seq_Mode[side] == 1 ) {
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
			}
			if ( Free_Mode[side] >= 1 ) LeaveCriticalSection( &CR_TMRUN[0] );
		}
		else {
			if ( ( Seq_Mode[side] == 1 ) && ( Free_Mode[side] == 0 ) ) {
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
			}
			else if ( Free_Mode[side] >= 1 ) {
				LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
			}
		}
		*errcode = 13;
		return SYS_ABORTED;
	}
	//-----------------------------------------------------------------------------------------------------
	if ( ManualMode == 1 ) FA_MAPPING_SIGNAL_SET( side , 4 ); // 2008.03.19
	//-----------------------------------------------------------------------------------------------------
	if ( Seq_Mode[side] == 1 ) {
		if ( C1Run[Chamber1] ) {
			if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
				if ( InMode ) {
					if ( SkipCheck ) {
						//
						if ( C1CMMode[Chamber1] ) { // 2010.03.10
							if ( !msgoff ) { // 2006.05.17
								FA_ACCEPT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 1 );
							}
						}
						//
						if ( Ch1Run )
							Result1[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot );
						else
							Result1[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot );
					}
					else {
						Result1[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN" );
					}
				}
				else {
					if ( SkipCheck ) {
						//
						if ( C2CMMode[Chamber2] ) { // 2010.03.10
							if ( !msgoff ) { // 2006.05.17
								FA_ACCEPT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 2 );
							}
						}
						//
						if ( Ch1Run )
							Result1[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot );
						else
							Result1[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot );
					}
					else {
						Result1[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT" );
					}
				}
				if ( Result1[side] == SYS_ABORTED ) {
					if ( C1CMMode[Chamber1] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_REJECT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 1 , "" );
								else          FA_REJECT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 2 , "" );
							}
						}
					}
				}
				else {
					if ( C1CMMode[Chamber1] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
					Result1[side] = SYS_SUCCESS;
				}
			}
			else {
				if ( C1CMMode[Chamber1] ) { // 2010.03.10
					if ( !msgoff ) { // 2006.05.17
						if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
							if ( InMode ) FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 1 );
							else          FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 2 );
						}
					}
				}
				Result1[side] = SYS_SUCCESS;
			}
		}
		else {
			Result1[side] = SYS_SUCCESS;
		}
		Result2[side] = Result1[side];
		if ( ( Chamber1 != Chamber2 ) && ( Result1[side] != SYS_ABORTED ) ) {
			if ( C2Run[Chamber2] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber2 ) && ( Address_FUNCTION_INTERFACE[Chamber2] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( InMode ) {
						if ( SkipCheck ) {
							//
							if ( C2CMMode[Chamber2] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 1 );
								}
							}
							//
							if ( Ch2Run )
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot );
							else
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot );
						}
						else {
							Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN" );
						}
					}
					else {
						if ( SkipCheck ) {
							//
							if ( C2CMMode[Chamber2] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 2 );
								}
							}
							//
							if ( Ch2Run )
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot );
							else
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot );
						}
						else {
							Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT" );
						}
					}
					if ( Result2[side] == SYS_ABORTED ) {
						//
						if ( C2CMMode[Chamber2] ) { // 2010.03.10
							if ( !msgoff ) { // 2006.05.17
								if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
									if ( InMode ) FA_REJECT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 1 , "" );
									else          FA_REJECT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 2 , "" );
								}
							}
						}
						//
					}
					else {
						//
						if ( C2CMMode[Chamber2] ) { // 2010.03.10
							if ( !msgoff ) { // 2006.05.17
								if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
									if ( InMode ) FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 1 );
									else          FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 2 );
								}
							}
						}
						//
						Result2[side] = SYS_SUCCESS;
					}
				}
				else {
					//
					if ( C2CMMode[Chamber2] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
					//
					Result2[side] = SYS_SUCCESS;
				}
			}
		}
	}
	else {
		if ( Chamber1 == Chamber2 ) {
			if ( C1Run[Chamber1] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					if ( InMode ) {
						if ( SkipCheck ) {
							//
							if ( C1CMMode[Chamber1] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 1 );
								}
							}
							//
							if ( Ch1Run )
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot );
							else
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot );
						}
						else {
							Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN" );
						}
					}
					else {
						if ( SkipCheck ) {
							//
							if ( C1CMMode[Chamber1] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 2 );
								}
							}
							//
							if ( Ch1Run )
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot );
							else
								Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot );
						}
						else {
							Result2[side] = _dRUN_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT" );
						}
					}
					if ( Result2[side] == SYS_ABORTED ) {
						//
						if ( C1CMMode[Chamber1] ) { // 2010.03.10
							if ( !msgoff ) { // 2006.05.17
								if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
									if ( InMode ) FA_REJECT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 1 , "" );
									else          FA_REJECT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 2 , "" );
								}
							}
						}
						//
					}
					else {
						//
						if ( C1CMMode[Chamber1] ) { // 2010.03.10
							if ( !msgoff ) { // 2006.05.17
								if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
									if ( InMode ) FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 1 );
									else          FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 2 );
								}
							}
						}
						//
						Result2[side] = SYS_SUCCESS;
					}
				}
				else {
					//
					if ( C1CMMode[Chamber1] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
					//
					Result2[side] = SYS_SUCCESS;
				}
			}
			else {
				Result2[side] = SYS_SUCCESS;
			}
		}
		else {
			Run1[side] = FALSE;
			Run2[side] = FALSE;
			if ( C1Run[Chamber1] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber1 ) && ( Address_FUNCTION_INTERFACE[Chamber1] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					Run1[side] = TRUE;
					Result1[side] = SYS_RUNNING;
					if ( InMode ) {
						if ( SkipCheck ) {
							//
							if ( C1CMMode[Chamber1] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 1 );
								}
							}
							//
							if ( Ch1Run )
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
							else
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
						}
						else {
//							_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN" ); // 2008.04.03
							if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP IN" ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
						}
					}
					else {
						if ( SkipCheck ) {
							//
							if ( C1CMMode[Chamber1] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 2 );
								}
							}
							//
							if ( Ch1Run )
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
							else
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
						}
						else {
//							_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT" ); // 2008.04.03
							if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber1] , "CASSETTE_MAP OUT" ) ) Result1[side] = SYS_ABORTED; // 2008.04.03
						}
					}
				}
				else {
					Result1[side] = SYS_SUCCESS;
					//
					if ( C1CMMode[Chamber1] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
					//
				}
			}
			else {
				Result1[side] = SYS_SUCCESS;
			}
			if ( C2Run[Chamber2] ) {
				if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber2 ) && ( Address_FUNCTION_INTERFACE[Chamber2] >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) ) {
					Run2[side] = TRUE;
					Result2[side] = SYS_RUNNING;
					if ( InMode ) {
						if ( SkipCheck ) {
							//
							if ( C2CMMode[Chamber2] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 1 );
								}
							}
							//
							if ( Ch2Run )
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN %d %d" , StartSlot , EndSlot ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
							else
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN %d %d USING" , StartSlot , EndSlot ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
						}
						else {
//							_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN" ); // 2008.04.03
							if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP IN" ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
						}
					}
					else {
						if ( SkipCheck ) {
							//
							if ( C2CMMode[Chamber2] ) { // 2010.03.10
								if ( !msgoff ) { // 2006.05.17
									FA_ACCEPT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 2 );
								}
							}
							//
							if ( Ch2Run )
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT %d %d" , StartSlot , EndSlot ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
							else
//								_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot ); // 2008.04.03
								if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT %d %d USING" , StartSlot , EndSlot ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
						}
						else {
//							_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT" ); // 2008.04.03
							if ( !_dRUN_SET_FUNCTIONF( Address_FUNCTION_INTERFACE[Chamber2] , "CASSETTE_MAP OUT" ) ) Result2[side] = SYS_ABORTED; // 2008.04.03
						}
					}
				}
				else {
					Result2[side] = SYS_SUCCESS;
					//
					if ( C2CMMode[Chamber2] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
					//
				}
			}
			else {
				Result2[side] = SYS_SUCCESS;
			}
			while ( ( Result1[side] == SYS_RUNNING ) || ( Result2[side] == SYS_RUNNING ) ) {
				if ( Result1[side] == SYS_RUNNING ) Result1[side] = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber1] );
				if ( Result2[side] == SYS_RUNNING ) Result2[side] = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber2] );
				Sleep(1);
			}
			if ( Run1[side] ) {
				if ( Result1[side] == SYS_ABORTED ) {
					//
					if ( C1CMMode[Chamber1] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_REJECT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 1 , "" );
								else          FA_REJECT_MESSAGE( Chamber1 - 1 , FA_MAPPING , 2 , "" );
							}
						}
					}
					//
				}
				else {
					if ( C1CMMode[Chamber1] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber1 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
				}
			}
			if ( Run2[side] ) {
				if ( Result2[side] == SYS_ABORTED ) {
					//
					if ( C2CMMode[Chamber2] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_REJECT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 1 , "" );
								else          FA_REJECT_MESSAGE( Chamber2 - 1 , FA_MAPPING , 2 , "" );
							}
						}
					}
					//
				}
				else {
					//
					if ( C2CMMode[Chamber2] ) { // 2010.03.10
						if ( !msgoff ) { // 2006.05.17
							if ( _i_SCH_SYSTEM_FA_GET( side ) != 0 ) {
								if ( InMode ) FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 1 );
								else          FA_RESULT_MESSAGE( Chamber2 - 1 , FA_MAP_COMPLETE , 2 );
							}
						}
					}
					//
				}
			}
			if ( ( Result1[side] == SYS_ABORTED ) || ( Result2[side] == SYS_ABORTED ) ) Result2[side] = SYS_ABORTED;
		}
	}
	//
//	if ( !_i_SCH_PRM_GET_MODULE_MODE( FM ) ) { // 2010.03.10
	if ( !FMMode ) { // 2010.03.10
		if ( Seq_Mode[side] == 1 ) {
//			LeaveCriticalSection( &CR_xFEMRUN ); // 2008.03.20
			LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
		}
		if ( Free_Mode[side] >= 1 ) LeaveCriticalSection( &CR_TMRUN[0] );
	}
	else {
		if ( ( Seq_Mode[side] == 1 ) && ( Free_Mode[side] == 0 ) ) {
//			LeaveCriticalSection( &CR_xFEMRUN ); // 2008.03.20
			LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
		}
		else if ( Free_Mode[side] >= 1 ) {
//			LeaveCriticalSection( &CR_xFEMRUN ); // 2008.03.20
			LeaveCriticalSection( &CR_FEM_MAP_PICK_PLACE ); // 2008.03.20
		}
	}
	//
	*errcode = 19;
	//
	return Result2[side];
}

