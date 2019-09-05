#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "Equip_Handling.h"
#include "sch_prm_FBTPM.h"
#include "sch_estimate.h"

#include "INF_sch_CommonUser.h"

int Address_FUNCTION_PROCESS[MAX_CASSETTE_SIDE][MAX_CHAMBER];
int Address_FUNCTION_INTERFACE[MAX_CHAMBER];
int Address_FUNCTION_INTERFACE_FAL;
int Address_FUNCTION_INTERFACE_FIC; // 2005.09.06
int Address_FUNCTION_INTERFACE_FM2; // 2007.06.20
//
int Address_FUNCTION_INTERFACE_SUB_CALL_END[MAX_CHAMBER]; // 2006.10.24
//
//int  _EQUIP_RUNNING2_TAG[MAX_CHAMBER]; // 2012.03.22 // 2012.10.31
int  _EQUIP_RUNNING_TAG = FALSE; // 2008.04.17
long _EQUIP_RUNNING_TAG_TIMER = 0; // 2008.04.17
int  _EQUIP_RUNNING_TAG_COUNT = 0; // 2008.04.17
//================================================================================

extern int EXTEND_SOURCE_USE; // 2019.029.21

void EQUIP_INIT_BEFORE_READFILE() {
	int i;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
//		_EQUIP_RUNNING2_TAG[i] = FALSE; // 2012.03.22 // 2012.10.31
		//
		Address_FUNCTION_INTERFACE[i] = -1;
		Address_FUNCTION_INTERFACE_SUB_CALL_END[i] = -1; // 2006.10.24
	}
	Address_FUNCTION_INTERFACE_FM2 = -1; // 2007.06.21
	//=================================================================================
	/*
	//
	// 2019.02.21
	//
	for ( i = CM1 ; i <= FM ; i++ ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2002.09.13
		if ( ( i == AL ) || ( i == IC ) ) continue;
		Address_FUNCTION_INTERFACE[i] = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_INTERFACE_%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
	}
	//===================================================================================================================
	Address_FUNCTION_INTERFACE_FM2 = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_INTERFACE_%s2" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( FM ) ); // 2007.06.21
	//===================================================================================================================
	//
	*/
	//
	//
	// 2019.02.21
	//
	for ( i = CM1 ; i < FM ; i++ ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue; // 2002.09.13
		if ( ( i == AL ) || ( i == IC ) ) continue;
		Address_FUNCTION_INTERFACE[i] = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_INTERFACE_%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( i ) );
	}
	//
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) || ( EXTEND_SOURCE_USE != 0 ) ) {
		Address_FUNCTION_INTERFACE[FM] = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_INTERFACE_%s" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( FM ) );
		Address_FUNCTION_INTERFACE_FM2 = _FIND_FROM_STRINGF( _K_F_IO , "SCHEDULER_INTERFACE_%s2" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( FM ) ); // 2007.06.21
	}
	//===================================================================================================================
	// 2006.10.24
	//===================================================================================================================
	for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
		if ( !_i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) continue;
		Address_FUNCTION_INTERFACE_SUB_CALL_END[i+BM1]  = _FIND_FROM_STRINGF( _K_F_IO , "BM%d.END" , i + 1 );
	}
	//===================================================================================================================
}


extern BOOL SLOTPROCESSING; // 2016.04.26

void EQUIP_INIT_AFTER_READFILE() {
	int i , j , f;
	//
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			Address_FUNCTION_PROCESS[j][i] = -1;
		}
	}
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		if ( !SLOTPROCESSING ) { // 2016.04.26
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) continue; // 2002.09.13
		}
		//
		f = 0;
		//
		Address_FUNCTION_PROCESS[0][i+PM1]  = _FIND_FROM_STRING( _K_F_IO , _i_SCH_PRM_GET_DFIX_PROCESS_FUNCTION_NAME( 0 , i ) );		if ( Address_FUNCTION_PROCESS[0][i+PM1] >= 0 ) f = 1;
		for ( j = 1 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			if ( STRCMP_L( _i_SCH_PRM_GET_DFIX_PROCESS_FUNCTION_NAME( 0 , i ) , _i_SCH_PRM_GET_DFIX_PROCESS_FUNCTION_NAME( j , i ) ) ) {
				Address_FUNCTION_PROCESS[j][i+PM1] = Address_FUNCTION_PROCESS[0][i+PM1];
			}
			else {
				Address_FUNCTION_PROCESS[j][i+PM1] = _FIND_FROM_STRING( _K_F_IO , _i_SCH_PRM_GET_DFIX_PROCESS_FUNCTION_NAME( j , i ) );
			}
			//
			if ( Address_FUNCTION_PROCESS[j][i+PM1] >= 0 ) f = 1;
			//
		}
		//
		if ( SLOTPROCESSING ) { // 2016.04.26
			if ( f == 1 ) {
				if ( !_i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) {
					_i_SCH_PRM_SET_MODULE_MODE( i + PM1 , TRUE );
				}
			}
		}
		//
	}
	//
	if ( !SLOTPROCESSING ) { // 2016.04.26
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( ( i >= CM1 ) && ( i <= FM ) && ( i != AL ) && ( i != IC ) ) {
				if ( Address_FUNCTION_INTERFACE[i] < 0 ) {
					for ( j = 1 ; j < MAX_CASSETTE_SIDE ; j++ ) Address_FUNCTION_PROCESS[j][i] = -1;
					_i_SCH_PRM_SET_MODULE_MODE( i , FALSE );
				}
			}
		}
		//
	}
	//
	if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() >= 2 ) {
		Address_FUNCTION_INTERFACE_FAL = _FIND_FROM_STRING( _K_F_IO , _i_SCH_PRM_GET_DFIX_FAL_MULTI_FNAME() );
		if ( Address_FUNCTION_INTERFACE_FAL < 0 ) _i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( 0 ); // 2004.09.08
	}
	else {
		Address_FUNCTION_INTERFACE_FAL = -1;
	}

	if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2005.09.06 // 2010.12.22
		if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_FNAME()[0] == 0 ) { // 2005.12.01
			Address_FUNCTION_INTERFACE_FIC = -1;
		}
		else {
			Address_FUNCTION_INTERFACE_FIC = _FIND_FROM_STRING( _K_F_IO , _i_SCH_PRM_GET_DFIX_FIC_MULTI_FNAME() );
			if ( Address_FUNCTION_INTERFACE_FIC == -1 ) _i_SCH_PRM_SET_DFIX_FIC_MULTI_FNAME( "" ); // 2005.12.01
		}
	}
	else if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 2 ) { // 2010.12.22
		Address_FUNCTION_INTERFACE_FIC = -1;
	}
	else if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 3 ) { // 2010.12.22
		Address_FUNCTION_INTERFACE_FIC = -1;
	}
	else {
		Address_FUNCTION_INTERFACE_FIC = -1;
	}
	
//	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) ) { // 2004.09.08
//		if ( Address_FUNCTION_INTERFACE_FAL < 0 ) _i_SCH_PRM_SET_DFIX_FAL_MULTI_CONTROL( FALSE ); // 2004.09.08
//	} // 2004.09.08
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( int Side , char *data ) {
	switch( Side ) {
	case 1 : strcat( data , " TRANSFER" ); break;
	case 2 : strcat( data , " TRANSFER2" ); break;
	case 3 : strcat( data , " TRANSFER3" ); break;
	case 4 : strcat( data , " TRANSFER4" ); break;
	case 5 : strcat( data , " TRANSFER5" ); break;
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void EQUIP_INTERFACE_STRING_APPEND_MESSAGE( BOOL MaintInfUse , char *MsgAppchar ) { // 2013.04.26
	if ( MaintInfUse ) {
		if ( _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() >= 3 ) {
			strcpy( MsgAppchar , "M" );
		}
		else {
			strcpy( MsgAppchar , "" );
		}
	}
	else {
		strcpy( MsgAppchar , "" );
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void EQUIP_INTERFACE_STRING_APPEND_TRANSFER( int Side , char *data ) {
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Side - TR_CHECKING_SIDE + 1 , data );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL EQUIP_INTERFACE_INVALID_STATION( int Chamber ) {
	if ( Chamber < CM1 ) return TRUE;
	if ( Chamber > FM ) return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Interface Check
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_ADDRESS_FM( int fms ) { // 2007.07.24
	if      ( fms == 0 ) return Address_FUNCTION_INTERFACE[FM];
	else if ( fms == 1 ) return Address_FUNCTION_INTERFACE_FM2;
	return -1;
}
//
BOOL EQUIP_INTERFACE_FUNCTION_CONNECT( int Chamber ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return FALSE;
	return TRUE;
}
BOOL EQUIP_PROCESS_FUNCTION_CONNECT( int Chamber ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( Address_FUNCTION_PROCESS[i][Chamber] >= 0 ) return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Handling Function
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_STATUS_PROCESS_FOR_LOG( int Side , int Chamber ) {
	if ( Address_FUNCTION_PROCESS[Side][Chamber] < 0 ) return -1;
	return _dREAD_FUNCTION( Address_FUNCTION_PROCESS[Side][Chamber] );
}
int EQUIP_READ_FUNCTION_FOR_LOG( int Chamber ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return -1;
	return( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] ) );
}
//========================================================================
int EQUIP_READ_FUNCTION( int Chamber ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return SYS_SUCCESS;
	return( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] ) );
}
int EQUIP_WAIT_FUNCTION( int Chamber ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return SYS_SUCCESS;
	return( _dRUN_WAIT_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] ) );
}
int EQUIP_RUN_FUNCTION( int Chamber , char *data ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return SYS_SUCCESS;
	return( _dRUN_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , data ) );
}
void EQUIP_RUN_FUNCTION_ABORT( int Chamber ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return;
	_dRUN_FUNCTION_ABORT( Address_FUNCTION_INTERFACE[Chamber] );
}
BOOL EQUIP_RUN_SET_FUNCTION( int Chamber , char *data ) {
	if ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) return FALSE;
	return _dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , data );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_FM_GET_SYNCH_DATA( int fms ) {
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(FM) ) return 0; // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE(FM) || ( GET_RUN_LD_CONTROL(4) > 0 ) ) return 0; // 2014.01.09
	//
	return _i_SCH_PRM_GET_RB_FM_SYNCH_CHECK( fms );
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip SYS_ERROR AnimCheck
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _i_SCH_EQ_SYSERROR_CHECK_ANIM_NOT_RETURN_CHECK( int Chamber ) { // 2011.04.05
	char RetBuffer[256];
	char ImsiBuffer[32];
	int  z1 , z2;

	if ( ( Chamber < CM1 ) || ( Chamber > FM ) ) return 1;
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) ) return 2; // 2014.01.09
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return 2; // 2014.01.09
	//
	_dREAD_UPLOAD_MESSAGE( Address_FUNCTION_INTERFACE[Chamber] , RetBuffer );
	if ( strlen( RetBuffer ) <= 0 ) return 3;
	z1 = 0;
	while( TRUE ) {
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( RetBuffer , '|' , ImsiBuffer , z1 , 31 );
		if ( z1 == z2 ) break;
		if      ( STRCMP_L( ImsiBuffer , "ANIM_NOT_RETURN" ) ) return 0;
		//
		z1 = z2;
		//
	}
	//
	return 4;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Post Skip Check After Post Recv
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int _i_SCH_EQ_POSTSKIP_CHECK_AFTER_POSTRECV( int Chamber , int *prcs_time , int *post_time ) {
	char RetBuffer[256];
	char ImsiBuffer[32];
	int  z1 , z2 , rpreskip , res;

	*prcs_time = -1; // 2006.02.13
	*post_time = -1; // 2006.02.13
	rpreskip = 0; // 2010.01.27

//	if ( Chamber >= BM1 ) return 0; // 2007.01.02
	if ( ( Chamber >= BM1 ) && ( Chamber < TM ) ) return 0; // 2007.01.02
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) ) return 0; // 2014.01.09
//	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return 0; // 2014.01.09 // 2017.01.03
	//
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) {
		//
		if ( !_SCH_COMMON_GET_UPLOAD_MESSAGE( 10 , Chamber , RetBuffer ) ) { // 2017.01.03
			return 0; // 2014.01.09 // 2017.01.03
		}
		//
	}
	else {
		//
		if ( !_SCH_COMMON_GET_UPLOAD_MESSAGE( 0 , Chamber , RetBuffer ) ) { // 2017.01.03
			_dREAD_UPLOAD_MESSAGE( Address_FUNCTION_INTERFACE[Chamber] , RetBuffer );
		}
		//
	}
	//
	if ( strlen( RetBuffer ) <= 0 ) return 0;
	res = 0; // 2006.02.13
	z1 = 0;
	while( TRUE ) {
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( RetBuffer , '|' , ImsiBuffer , z1 , 31 );
		if ( z1 == z2 ) break;
//		if      ( STRCMP_L( ImsiBuffer , "POSTSKIP" ) ) return 1; // 2006.02.13
		if      ( STRCMP_L( ImsiBuffer , "POSTSKIP" ) ) res = 1;
//		else if ( STRCMP_L( ImsiBuffer , "DUMMYRUN" ) ) return 2; // 2006.02.13
		else if ( STRCMP_L( ImsiBuffer , "DUMMYRUN" ) ) res = 2;
		else if ( STRCMP_L( ImsiBuffer , "PRESKIP"  ) ) rpreskip = 1; // 2010.01.27
		else if ( STRNCMP_L( ImsiBuffer , "PRCSTIME" , 8 ) ) { // 2006.02.13
			if ( strlen( ImsiBuffer + 8 ) >= 1 ) {
				*prcs_time = atoi( ImsiBuffer + 8 ); 
			}
		}
		else if ( STRNCMP_L( ImsiBuffer , "POSTTIME" , 8 ) ) { // 2006.02.13
			if ( strlen( ImsiBuffer + 8 ) >= 1 ) {
				*post_time = atoi( ImsiBuffer + 8 ); 
			}
		}
		z1 = z2;
	}
//	return 0; // 2006.02.13
//	return res; // 2006.02.13 // 2010.01.27
	return ( rpreskip * 10 ) + res; // 2006.02.13 // 2010.01.27
}

int EQUIP_INTERFACE_PICKPLACE_CANCEL( int tmside , int mode , int Chamber , int Finger , int Slot , int Depth , int Transfer , int SrcCas , int abortwhere ) {
	int Mp , Mt;
	BOOL PM_Run , TM_Run;
	char ParamT[64] , ParamP[64] , Buffer[64];
	//
	switch ( _i_SCH_PRM_GET_PICKPLACE_CANCEL_MESSAGE() ) {
	case 2 :
	case 3 :
		break;
	default :
		return SYS_SUCCESS;
		break;
	}
	//
	if ( Chamber == AL || Chamber == IC ) return SYS_SUCCESS;
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( TM + tmside ) || ( Address_FUNCTION_INTERFACE[ TM + tmside ] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) // 2014.01.09
		TM_Run = FALSE;
	else
		TM_Run = TRUE;
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) // 2014.01.09
		PM_Run = FALSE;
	else
		PM_Run = TRUE;
	//
	if ( mode == 0 ) { // pick
		sprintf( ParamT , "PICK_CANCEL %d" , abortwhere );
		sprintf( ParamP , "PICK_CANCEL %d" , abortwhere );
	}
	else { // place
		sprintf( ParamT , "PLACE_CANCEL %d" , abortwhere );
		sprintf( ParamP , "PLACE_CANCEL %d" , abortwhere );
	}
	if      ( Chamber <  PM1 ) {
		sprintf( Buffer , " %d %c/%d/%d/%d" , Slot , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
	}
	else if ( Chamber >= BM1 ) {
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) {
			sprintf( Buffer , " %d %c/%d/%d/%d" , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
		}
		else {
			sprintf( Buffer , " %d %c/%d/%d/%d" , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
		}
	}
	else {
		sprintf( Buffer , " %d %c/%d/%d/%d" , Depth , Finger - TA_STATION + 'A' , SrcCas , tmside + 1 , Slot );
	}
	strcat( ParamP , Buffer );
	if      ( Chamber <  PM1 ) {
		sprintf( Buffer , " %c_CM%d %d" , Finger - TA_STATION + 'A' , Chamber - CM1 + 1 , Slot );
	}
	else if ( Chamber >= BM1 ) {
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) {
			sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) );
		}
		else {
			sprintf( Buffer , " %c_BM%d %d" , Finger - TA_STATION + 'A' , Chamber - BM1 + 1 , Depth + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber ) );
		}
	}
	else {
		sprintf( Buffer , " %c_PM%d %d" , Finger - TA_STATION + 'A' , Chamber - PM1 + 1 , Depth );
	}
	strcat( ParamT , Buffer );
	//
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Transfer , ParamP );
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER_SUB( Transfer , ParamT );
	//
	if ( abortwhere > 0 ) {
		strcat( ParamP , " ABORT" );
		strcat( ParamT , " ABORT" );
	}
	//
	if ( TM_Run && PM_Run ) {
		Mt = SYS_RUNNING;
		Mp = SYS_RUNNING;
//		_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[ TM + tmside ] , ParamT , Address_FUNCTION_INTERFACE[Chamber] , ParamP ); // 2007.12.12
		_dRUN_SET_FUNCTION_FREE( Address_FUNCTION_INTERFACE[ TM + tmside ] , ParamT );
		_dRUN_SET_FUNCTION_FREE( Address_FUNCTION_INTERFACE[Chamber] , ParamP );
	}
	else if ( TM_Run ) {
		Mt = SYS_RUNNING;
		Mp = SYS_SUCCESS;
//		_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[ TM + tmside ] , ParamT ); // 2007.12.12
		_dRUN_SET_FUNCTION_FREE( Address_FUNCTION_INTERFACE[ TM + tmside ] , ParamT ); // 2007.12.12
	}
	else if ( PM_Run ) {
		Mt = SYS_SUCCESS;
		Mp = SYS_RUNNING;
//		_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamP ); // 2007.12.12
		_dRUN_SET_FUNCTION_FREE( Address_FUNCTION_INTERFACE[Chamber] , ParamP ); // 2007.12.12
	}
	else {
		Mt = SYS_SUCCESS;
		Mp = SYS_SUCCESS;
	}
	while ( TRUE ) {
		if ( TM_Run && ( Mt == SYS_RUNNING ) ) {
			Mt = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[ TM + tmside ] );
		}
		if ( PM_Run && ( Mp == SYS_RUNNING ) ) {
			Mp = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
		}
		if ( ( Mt != SYS_RUNNING ) && ( Mp != SYS_RUNNING ) ) {
			if ( ( Mt == SYS_ABORTED ) || ( Mp == SYS_ABORTED ) )
				return SYS_ABORTED;
			else
				break;
		}
//		if ( MANAGER_ABORT() ) return SYS_ABORTED; // 2007.12.12
		Sleep(1);
	}
	return SYS_SUCCESS;
}

void EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( int mode , int Chamber , int Slot1 , int Slot2 , int Transfer , int SrcCas , int abortwhere ) { // 2007.12.12
	char ParamB[256];
	int s1 , s2;
	//
	switch ( _i_SCH_PRM_GET_PICKPLACE_CANCEL_MESSAGE() ) {
	case 1 :
	case 3 :
		break;
	default :
		return;
		break;
	}
	//
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) || ( Address_FUNCTION_INTERFACE[Chamber] < 0 ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return; // 2014.01.09
	//
	if ( Chamber < PM1 ) return;
	if ( Chamber < BM1 ) return;
	if ( Chamber >= TM ) return;
	//
	if ( Slot1 <= 0 ) {
		s1 = 0;
	}
	else {
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) {
			s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
		}
		else {
			s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
		}
	}
	if ( Slot2 <= 0 ) {
		s2 = 0;
	}
	else {
		if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) {
			s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
		}
		else {
			s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
		}
	}
	//======================================================================================
	if ( mode == 0 ) { // pick
		sprintf( ParamB , "PICK_FM_CANCEL %d %d" , s1 , s2 );
	}
	else {
		sprintf( ParamB , "PLACE_FM_CANCEL %d %d" , s1 , s2 );
	}
	//======================================================================================
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER( Transfer , ParamB );
	//
	if ( abortwhere > 0 ) {
		strcat( ParamB , " ABORT" );
	}
	//
	_dRUN_FUNCTION_FREE( Address_FUNCTION_INTERFACE[Chamber] , ParamB );
}


int _i_SCH_EQ_PROCESS_SKIPFORCE_RUN( int mode , int ch ) {
	switch( mode ) {
	case 1 : // Post/skip
		return EQUIP_POST_PROCESS_SKIP_RUN( ch );
		break;
	case 2 : // Post/force
		return EQUIP_POST_PROCESS_FORCE_RUN( ch );
		break;
	default : // main/skip
		return EQUIP_MAIN_PROCESS_SKIP_RUN( ch );
		break;
	}
}

int _i_SCH_EQ_INTERFACE_FUNCTION_ADDRESS( int ch , int *addr ) {
	*addr = -1;
	if ( ch < CM1 ) return -1;
	if ( ch > FM  ) return -1;
	if ( Address_FUNCTION_INTERFACE[ch] < 0 ) return 0;
	*addr = Address_FUNCTION_INTERFACE[ch];
	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( ch ) || ( GET_RUN_LD_CONTROL(0) > 0 ) ) return 0; // 2014.01.09
	//
	return 1;
}

int _i_SCH_EQ_INTERFACE_FUNCTION_STATUS( int ch , BOOL waitfinish ) {
	if ( waitfinish ) {
		return EQUIP_WAIT_FUNCTION( ch );
	}
	else {
		return EQUIP_READ_FUNCTION( ch );
	}
}



int _i_SCH_EQ_PROCESS_FUNCTION_STATUS( int side , int ch , BOOL waitfinish ) {
	if ( waitfinish ) {
		return EQUIP_WAIT_PROCESS( side , ch );
	}
	else {
		return EQUIP_STATUS_PROCESS( side , ch );
	}
}





//================================================================================
BOOL EQUIP_RUNNING_CHECK_SUB( BOOL prcsfunconly ) { // 2008.04.18
	int i , j;
	//=================================================================================
	if ( !prcsfunconly ) { // 2013.09.13
		if ( _EQUIP_RUNNING_TAG ) {
			_EQUIP_RUNNING_TAG = FALSE;
			return TRUE;
		}
		//=================================================================================
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			if ( Address_FUNCTION_INTERFACE[i] != -1 ) {
				if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[i] ) == SYS_RUNNING ) return TRUE;
			}
		}
		//=================================================================================
		if ( Address_FUNCTION_INTERFACE_FM2 != -1 ) {
			if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FM2 ) == SYS_RUNNING ) return TRUE;
		}
		//=================================================================================
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( Address_FUNCTION_INTERFACE_SUB_CALL_END[i+BM1] != -1 ) {
				if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_SUB_CALL_END[i+BM1] ) == SYS_RUNNING ) return TRUE;
			}
		}
		//=================================================================================
		if ( Address_FUNCTION_INTERFACE_FAL != -1 ) {
			if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FAL ) == SYS_RUNNING ) return TRUE;
		}
		//=================================================================================
		if ( Address_FUNCTION_INTERFACE_FIC != -1 ) {
			if ( _dREAD_FUNCTION( Address_FUNCTION_INTERFACE_FIC ) == SYS_RUNNING ) return TRUE;
		}
	}
	//===================================================================================================================
	for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( Address_FUNCTION_PROCESS[j][i+PM1] != -1 ) {
				if ( j > 0 ) {
					if ( Address_FUNCTION_PROCESS[j-1][i+PM1] == Address_FUNCTION_PROCESS[j][i+PM1] ) continue;
				}
				if ( _dREAD_FUNCTION( Address_FUNCTION_PROCESS[j][i+PM1] ) == SYS_RUNNING ) return TRUE;
			}
		}
	}
	//===================================================================================================================
	if ( !prcsfunconly ) { // 2013.09.13
		if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2010.10.12
			//
			for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
				//
				if ( !_i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) continue;
				//
				for ( j = 0 ; j < MAX_PM_SLOT_DEPTH ; j++ ) {
					//
					if ( _i_SCH_MODULE_Get_PM_WAFER( i + PM1 , j ) > 0 ) return FALSE;
					//
				}
			}
			//
			for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
				//
				if ( !_i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) continue;
				//
				for ( j = 0 ; j < 2 ; j++ ) {
					//
					if ( _i_SCH_MODULE_Get_TM_WAFER( i , j ) > 0 ) return FALSE;
					//
				}
			}
			//
			return TRUE;
		}
	}
	//===================================================================================================================
	return FALSE;
	//===================================================================================================================
}


//BOOL EQUIP_RUNNING_CHECK( long timeover ) { // 2008.04.18 // 2018.09.04
BOOL EQUIP_RUNNING_CHECK( long timeover , BOOL *notRun ) { // 2008.04.18 // 2018.09.04
	if      ( timeover < 0 ) {
		_EQUIP_RUNNING_TAG_TIMER = 0;
		_EQUIP_RUNNING_TAG_COUNT = 0;
	}
	else if ( timeover == 0 ) {
		_EQUIP_RUNNING_TAG_TIMER = 0;
	}
	else {
		if ( EQUIP_RUNNING_CHECK_SUB( FALSE ) ) {
			//
			*notRun = FALSE; // 2018.09.04
			//
			_EQUIP_RUNNING_TAG_TIMER = 0;
			_EQUIP_RUNNING_TAG_COUNT = 0;
		}
		else {
			//
			*notRun = TRUE; // 2018.09.04
			//
			if ( _EQUIP_RUNNING_TAG_TIMER == 0 ) {
				_EQUIP_RUNNING_TAG_TIMER = GetTickCount();
			}
			else {
				if ( _EQUIP_RUNNING_TAG_COUNT < 3 ) {
					if ( (signed) ( GetTickCount() - _EQUIP_RUNNING_TAG_TIMER ) >= ( timeover * ( _EQUIP_RUNNING_TAG_COUNT + 1 ) * 1000 ) ) {
						_EQUIP_RUNNING_TAG_COUNT++;
						return FALSE;
					}
				}
				else {
					_EQUIP_RUNNING_TAG_TIMER = 0;
				}
			}
		}
	}
	return TRUE;
}


void EQUIP_RUNNING_STRING_INFO( BOOL debugrun , char *data ) { // 2008.04.18
	if ( _EQUIP_RUNNING_TAG_TIMER == 0 ) {
		if ( debugrun ) {
			strcpy( data , "Debugging" );
		}
		else {
			strcpy( data , "" );
		}
	}
	else {
		if ( debugrun ) {
			sprintf( data , "%d:%d" , ( GetTickCount() - _EQUIP_RUNNING_TAG_TIMER ) , _EQUIP_RUNNING_TAG_COUNT );
		}
		else {
			strcpy( data , "" );
		}
	}
}

BOOL EQUIP_MESAGE_NOTUSE_GATE( BOOL MaintInfUse , int tm , int ch ) { // 2013.04.26
	if ( MaintInfUse ) {
		switch( _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() ) { // "Same|AlwaysUse|AlwaysNotUse|Same(M)|AlwaysUse(M)|AlwaysNotUse(M)"
		case 1 :
		case 4 :
			return FALSE;
		case 2 :
		case 5 :
			return TRUE;
		}
	}
	if ( tm >= 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( tm ) == 1 ) return TRUE;
	}
	if ( ch >= 0 ) {
		if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_GATE( ch ) == 1 ) return TRUE;
	}
	return FALSE;
}


BOOL EQUIP_MESAGE_NOTUSE_PREPRECV( BOOL MaintInfUse , int tm , int ch ) { // 2013.04.26
	if ( MaintInfUse ) {
		switch( _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() ) { // "Same|AlwaysUse|AlwaysNotUse|Same(M)|AlwaysUse(M)|AlwaysNotUse(M)"
		case 1 :
		case 4 :
			return FALSE;
		case 2 :
		case 5 :
			return TRUE;
		}
	}
	//
	if ( tm == FM ) {
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( FM ) >= 2 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( ch ) >= 2 ) return TRUE;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( FM ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( FM ) == 3 ) return TRUE;
		}
	}
	else {
		if ( tm >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( tm ) ) return TRUE;
		}
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( ch ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPRECV( ch ) == 3 ) return TRUE;
		}
	}
	return FALSE;
}

BOOL EQUIP_MESAGE_NOTUSE_PREPSEND( BOOL MaintInfUse , int tm , int ch ) { // 2013.04.26
	if ( MaintInfUse ) {
		switch( _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() ) { // "Same|AlwaysUse|AlwaysNotUse|Same(M)|AlwaysUse(M)|AlwaysNotUse(M)"
		case 1 :
		case 4 :
			return FALSE;
		case 2 :
		case 5 :
			return TRUE;
		}
	}
	//
	if ( tm == FM ) {
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( FM ) >= 2 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( ch ) >= 2 ) return TRUE;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( FM ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( FM ) == 3 ) return TRUE;
		}
	}
	else {
		if ( tm >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( tm ) ) return TRUE;
		}
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( ch ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_PREPSEND( ch ) == 3 ) return TRUE;
		}
	}
	return FALSE;
}



BOOL EQUIP_MESAGE_NOTUSE_POSTRECV( BOOL MaintInfUse , int tm , int ch ) { // 2013.04.26
	if ( MaintInfUse ) {
		switch( _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() ) { // "Same|AlwaysUse|AlwaysNotUse|Same(M)|AlwaysUse(M)|AlwaysNotUse(M)"
		case 1 :
		case 4 :
			return FALSE;
		case 2 :
		case 5 :
			return TRUE;
		}
	}
	//
	if ( tm == FM ) {
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( FM ) >= 2 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( ch ) >= 2 ) return TRUE;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( FM ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( FM ) == 3 ) return TRUE;
		}
	}
	else {
		if ( tm >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( tm ) ) return TRUE;
		}
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( ch ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTRECV( ch ) == 3 ) return TRUE;
		}
	}
	return FALSE;
}

BOOL EQUIP_MESAGE_NOTUSE_POSTSEND( BOOL MaintInfUse , int tm , int ch ) { // 2013.04.26
	if ( MaintInfUse ) {
		switch( _i_SCH_PRM_GET_MESSAGE_MAINTINTERFACE() ) { // "Same|AlwaysUse|AlwaysNotUse|Same(M)|AlwaysUse(M)|AlwaysNotUse(M)"
		case 1 :
		case 4 :
			return FALSE;
		case 2 :
		case 5 :
			return TRUE;
		}
	}
	//
	if ( tm == FM ) {
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( FM ) >= 2 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( ch ) >= 2 ) return TRUE;
		}
		else {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( FM ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( FM ) == 3 ) return TRUE;
		}
	}
	else {
		if ( tm >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( tm ) ) return TRUE;
		}
		if ( ch >= 0 ) {
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( ch ) == 1 ) return TRUE;
			if ( _i_SCH_PRM_GET_MODULE_MESSAGE_NOTUSE_POSTSEND( ch ) == 3 ) return TRUE;
		}
	}
	return FALSE;
}