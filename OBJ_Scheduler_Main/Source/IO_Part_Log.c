#include "default.h"

#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "Robot_Handling.h"
#include "IO_Part_data.h"

//------------------------------------------------------------------------------------------
int Address_CTC_DONE_CARR[MAX_CASSETTE_SIDE];
int Address_CTC_DONE_WFR[MAX_CASSETTE_SIDE];
int Address_CTC_TARGET_WFR[MAX_CASSETTE_SIDE];
//------------------------------------------------------------------------------------------
int Address_CTC_INFO_SAVE;
int Address_CTC_CASS_COUNT[MAX_CHAMBER];
int Address_CTC_PICK_COUNT[MAX_CHAMBER];
int Address_CTC_PLACE_COUNT[MAX_CHAMBER];
int Address_CTC_PROCESS_COUNT[MAX_CHAMBER];
int Address_CTC_FAIL_COUNT[MAX_CHAMBER];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void LOG_MTL_INIT() {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
//		Address_CTC_DONE_CARR[i]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.DONE_CARR_STATUS%s" , MULMDLSTR[ i ] ); // 2011.01.06
		Address_CTC_DONE_CARR[i]  = -2; // 2011.01.06
		//
//		Address_CTC_DONE_WFR[i]   = _FIND_FROM_STRINGF( _K_D_IO , "CTC.DONE_WFR_STATUS%s" , MULMDLSTR[ i ] ); // 2011.01.06
		Address_CTC_DONE_WFR[i]   = -2; // 2011.01.06
		//
//		Address_CTC_TARGET_WFR[i] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.TARGET_WFR_COUNT%s" , MULMDLSTR[ i ] ); // 2011.01.06
		Address_CTC_TARGET_WFR[i] = -2; // 2011.01.06
	}

	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
		//
		Address_CTC_CASS_COUNT[i]    = -1;
		//
		Address_CTC_PICK_COUNT[i]    = -1;
		Address_CTC_PLACE_COUNT[i]   = -1;
		//
		Address_CTC_PROCESS_COUNT[i] = -1;
		Address_CTC_FAIL_COUNT[i]    = -1;
	}
	Address_CTC_INFO_SAVE		 = _FIND_FROM_STRING( _K_D_IO , "CTC.INFO_SAVE" );

	if ( Address_CTC_INFO_SAVE >= 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i + CM1 ) ) continue; // 2002.09.13
			//
//			Address_CTC_CASS_COUNT[i+CM1]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.CM%d_CASS_COUNT" , i + 1 ); // 2011.01.06
			Address_CTC_CASS_COUNT[i+CM1]  = -2; // 2011.01.06
			//
//			Address_CTC_PICK_COUNT[i+CM1]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.CM%d_PICK_COUNT" , i + 1 ); // 2011.01.06
//			Address_CTC_PLACE_COUNT[i+CM1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.CM%d_PLACE_COUNT" , i + 1 ); // 2011.01.06
			Address_CTC_PICK_COUNT[i+CM1]  = -2; // 2011.01.06
			Address_CTC_PLACE_COUNT[i+CM1] = -2; // 2011.01.06
		}
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i + PM1 ) ) continue; // 2002.09.13
//			Address_CTC_PICK_COUNT[i+PM1]    = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_PICK_COUNT" , i + 1 ); // 2011.01.06
//			Address_CTC_PLACE_COUNT[i+PM1]   = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_PLACE_COUNT" , i + 1 ); // 2011.01.06
			Address_CTC_PICK_COUNT[i+PM1]    = -2; // 2011.01.06
			Address_CTC_PLACE_COUNT[i+PM1]   = -2; // 2011.01.06
			//
//			Address_CTC_PROCESS_COUNT[i+PM1] = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_PROCESS_COUNT" , i + 1 ); // 2011.01.06
//			Address_CTC_FAIL_COUNT[i+PM1]    = _FIND_FROM_STRINGF( _K_D_IO , "CTC.PM%d_FAIL_COUNT" , i + 1 ); // 2011.01.06
			Address_CTC_PROCESS_COUNT[i+PM1] = -2; // 2011.01.06
			Address_CTC_FAIL_COUNT[i+PM1]    = -2; // 2011.01.06
		}
		//==============================================================
		// 2006.12.11
		//==============================================================
		for ( i = 0 ; i < MAX_BM_CHAMBER_DEPTH ; i++ ) {
			if ( !_i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) continue;
//			Address_CTC_PICK_COUNT[i+BM1]   = _FIND_FROM_STRINGF( _K_D_IO , "CTC.BM%d_PICK_COUNT" , i + 1 ); // 2011.01.06
//			Address_CTC_PLACE_COUNT[i+BM1]  = _FIND_FROM_STRINGF( _K_D_IO , "CTC.BM%d_PLACE_COUNT" , i + 1 ); // 2011.01.06
			Address_CTC_PICK_COUNT[i+BM1]   = -2; // 2011.01.06
			Address_CTC_PLACE_COUNT[i+BM1]  = -2; // 2011.01.06
		}
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void LOG_MTL_SET_DONE_CARR_COUNT( int side , int data ) {
//	IO_ADD_WRITE_DIGITAL_TH( Address_CTC_DONE_CARR[side] , data ); // 2011.01.06
	IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_DONE_CARR[side]) , FALSE , data , "CTC.DONE_CARR_STATUS%s" , MULMDLSTR[ side ] ); // 2011.01.06
}

void LOG_MTL_SET_DONE_WFR_COUNT( int side , int data ) {
//	IO_ADD_WRITE_DIGITAL_TH( Address_CTC_DONE_WFR[side] , data ); // 2011.01.06
	IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_DONE_WFR[side]) , FALSE , data , "CTC.DONE_WFR_STATUS%s" , MULMDLSTR[ side ] ); // 2011.01.06
}

void LOG_MTL_SET_TARGET_WAFER_COUNT( int side , int data ) {
//	IO_ADD_WRITE_DIGITAL_TH( Address_CTC_TARGET_WFR[side] , data ); // 2011.01.06
	IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_TARGET_WFR[side]) , FALSE , data , "CTC.TARGET_WFR_COUNT%s" , MULMDLSTR[ side ] ); // 2011.01.06
}

int LOG_MTL_GET_DONE_CARR_COUNT( int side ) {
//	return( IO_ADD_READ_DIGITAL( Address_CTC_DONE_CARR[side] ) ); // 2011.01.06
	return( IO_ADD_READ_DIGITAL_NAME( &(Address_CTC_DONE_CARR[side]) , "CTC.DONE_CARR_STATUS%s" , MULMDLSTR[ side ] ) ); // 2011.01.06
}

int LOG_MTL_GET_DONE_WFR_COUNT( int side ) {
//	return( IO_ADD_READ_DIGITAL( Address_CTC_DONE_WFR[side] ) ); // 2011.01.06
	return( IO_ADD_READ_DIGITAL_NAME( &(Address_CTC_DONE_WFR[side]) , "CTC.DONE_WFR_STATUS%s" , MULMDLSTR[ side ] ) ); // 2011.01.06
}

void LOG_MTL_MOVE_DATA_COUNT( int tside , int sside ) { // 2012.03.20
	//
	LOG_MTL_SET_DONE_CARR_COUNT( tside , LOG_MTL_GET_DONE_CARR_COUNT( tside ) + LOG_MTL_GET_DONE_CARR_COUNT( sside ) );
	LOG_MTL_SET_DONE_WFR_COUNT( tside , LOG_MTL_GET_DONE_WFR_COUNT( tside ) + LOG_MTL_GET_DONE_WFR_COUNT( sside ) );
	//
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void _i_SCH_IO_MTL_SAVE() {
	IO_ADD_WRITE_DIGITAL_TH( Address_CTC_INFO_SAVE , 1 );
}

//------------------------------------------------------------------------------------------
void _i_SCH_IO_MTL_ADD_PICK_COUNT( int chamber , int count ) {
//	if ( IO_ADD_APPEND_DIGITAL( Address_CTC_PICK_COUNT[chamber] , count ) ) _i_SCH_IO_MTL_SAVE(); // 2011.01.06

	// 2011.01.06
	if      ( ( chamber >= CM1 ) && ( chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		if ( IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PICK_COUNT[chamber]) , TRUE , count , "CTC.CM%d_PICK_COUNT" , chamber - CM1 + 1 ) ) _i_SCH_IO_MTL_SAVE();
	}
	else if ( ( chamber >= PM1 ) && ( chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PICK_COUNT[chamber]) , TRUE , count , "CTC.PM%d_PICK_COUNT" , chamber - PM1 + 1 ) ) _i_SCH_IO_MTL_SAVE();
	}
	else if ( ( chamber >= BM1 ) && ( chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		if ( IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PICK_COUNT[chamber]) , TRUE , count , "CTC.BM%d_PICK_COUNT" , chamber - BM1 + 1 ) ) _i_SCH_IO_MTL_SAVE();
	}

}

void _i_SCH_IO_MTL_ADD_PLACE_COUNT( int chamber , BOOL done , int side , int count ) {
	//
	// 2011.01.06
/*
	if ( Address_CTC_PLACE_COUNT[chamber] < 0 ) {
		if ( done ) {
			if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
				IO_ADD_APPEND_DIGITAL( Address_CTC_DONE_WFR[side] , count );
			}
		}
		return;
	}
	IO_ADD_APPEND_DIGITAL( Address_CTC_PLACE_COUNT[chamber] , count );
	if ( done ) {
		if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
			IO_ADD_APPEND_DIGITAL( Address_CTC_DONE_WFR[side] , count );
		}
	}
*/
	//
	// 2011.01.06
	if      ( ( chamber >= CM1 ) && ( chamber < ( MAX_CASSETTE_SIDE + CM1 ) ) ) {
		IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PLACE_COUNT[chamber]) , TRUE , count , "CTC.CM%d_PLACE_COUNT" , chamber - CM1 + 1 );
	}
	else if ( ( chamber >= PM1 ) && ( chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PLACE_COUNT[chamber]) , TRUE , count , "CTC.PM%d_PLACE_COUNT" , chamber - PM1 + 1 );
	}
	else if ( ( chamber >= BM1 ) && ( chamber < ( MAX_BM_CHAMBER_DEPTH + BM1 ) ) ) {
		IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PLACE_COUNT[chamber]) , TRUE , count , "CTC.BM%d_PLACE_COUNT" , chamber - BM1 + 1 );
	}
	//
	if ( done ) {
		if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) {
			IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_DONE_WFR[side]) , TRUE , count , "CTC.DONE_WFR_STATUS%s" , MULMDLSTR[ side ] );
		}
	}
	//
	_i_SCH_IO_MTL_SAVE();
}

void LOG_MTL_ADD_PROCESS_COUNT( int chamber , int count ) {
//	if ( IO_ADD_APPEND_DIGITAL( Address_CTC_PROCESS_COUNT[chamber] , count ) ) _i_SCH_IO_MTL_SAVE(); // 2011.01.16
	if ( ( chamber >= PM1 ) && ( chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_PROCESS_COUNT[chamber]) , TRUE , count , "CTC.PM%d_PROCESS_COUNT" , chamber - PM1 + 1 ) ) _i_SCH_IO_MTL_SAVE();
	}
}

void LOG_MTL_ADD_FAIL_COUNT( int chamber , int count ) {
//	if ( IO_ADD_APPEND_DIGITAL( Address_CTC_FAIL_COUNT[chamber] , count ) ) _i_SCH_IO_MTL_SAVE(); / 2011.01.16
	if ( ( chamber >= PM1 ) && ( chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_FAIL_COUNT[chamber]) , TRUE , count , "CTC.PM%d_FAIL_COUNT" , chamber - PM1 + 1 ) ) _i_SCH_IO_MTL_SAVE();
	}
}

void LOG_MTL_ADD_CASS_COUNT( int chamber ) {
//	if ( IO_ADD_APPEND_DIGITAL( Address_CTC_CASS_COUNT[chamber] , 1 ) ) _i_SCH_IO_MTL_SAVE(); // 2011.01.06
	if ( IO_ADD_WRITE_DIGITAL_TH_NAME( &(Address_CTC_CASS_COUNT[chamber]) , TRUE , 1 , "CTC.CM%d_CASS_COUNT" , chamber - CM1 + 1 ) ) _i_SCH_IO_MTL_SAVE(); // 2011.01.06
}

void LOG_MTL_STATUS( int chamber , char *data ) {
	if ( chamber == 0 ) {
		if ( Address_CTC_INFO_SAVE >= 0 ) strcpy( data , "Run" );
		else                              strcpy( data , "N/A" );
	}
	else if ( ( chamber >= CM1 ) && ( chamber < PM1 ) ) {
		if ( Address_CTC_CASS_COUNT[chamber] >= 0 )	 strcpy( data , "Run" );
		else                                         strcpy( data , "N/A" );
		if ( Address_CTC_PICK_COUNT[chamber] >= 0 )  strcat( data , "  Run" );
		else                                         strcat( data , "  N/A" );
		if ( Address_CTC_PLACE_COUNT[chamber] >= 0 ) strcat( data , "  Run" );
		else                                         strcat( data , "  N/A" );
	}
	else if ( ( chamber >= PM1 ) && ( chamber < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ) ) {
		if ( Address_CTC_PICK_COUNT[chamber] >= 0 )    strcpy( data , "Run" );
		else                                           strcpy( data , "N/A" );
		if ( Address_CTC_PLACE_COUNT[chamber] >= 0 )   strcat( data , "  Run" );
		else                                           strcat( data , "  N/A" );
		if ( Address_CTC_PROCESS_COUNT[chamber] >= 0 ) strcat( data , "  Run" );
		else                                           strcat( data , "  N/A" );
		if ( Address_CTC_FAIL_COUNT[chamber] >= 0 ) strcat( data , "  Run" );
		else                                           strcat( data , "  N/A" );
	}
	else {
		strcpy( data , "" );
	}
}
